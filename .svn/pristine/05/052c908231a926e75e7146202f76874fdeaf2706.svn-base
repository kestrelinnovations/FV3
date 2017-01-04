// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#include "stdafx.h"
#include "FalconView/include/OverlayElements.h"
#include "FalconView/ovlelem/CameraPositionIndicatorControl.h"

#include "osg/ComputeBoundsVisitor"
#include "osg/Group"
#include "osg/ref_ptr"
#include "osg/OperationThread"
#include "osg/TextureRectangle"
#include "osgEarth/Capabilities"
#include "osgEarth/Config"
#include "osgEarth/Registry"
#include "osgEarth/ShaderGenerator"
#include "osgEarthAnnotation/EllipseNode"
#include "osgEarthAnnotation/FeatureNode"
#include "osgEarthAnnotation/ImageOverlay"
#include "osgEarthAnnotation/ModelNode"
#include "osgEarthAnnotation/PlaceNode"
#include "osgEarthSymbology/AltitudeSymbol"
#include "osgEarthSymbology/Geometry"
#include "osgEarthUtil/Controls"

#include "Common/ComErrorObject.h"
#include "Common/SafeArray.h"
#include "FalconView/DisplayElementRootNode.h"
#include "FalconView/GenericNodeCallback.h"
#include "FalconView/include/err.h"
#include "FalconView/include/maps.h"
#include "FalconView/include/PlaybackTimeChangedNodeVisitor.h"
#include "FvCore/Include/FvCoreInclude.h"
#include "FvMappingGraphics/Include/GraphicsUtilities.h"
#include "FvMappingGraphics/Include/LineStyles.h"

// Private class for managing a collection of screen overlays
class ScreenOverlaysCollection
{
public:
   void Add(long handle,
      osgEarth::Util::Controls::ImageControl* screen_overlay)
   {
      m_screen_overlays[handle] = screen_overlay;
   }
   osgEarth::Util::Controls::ImageControl* Find(long handle)
   {
      auto it = m_screen_overlays.find(handle);
      if (it == m_screen_overlays.end())
         return nullptr;

      return it->second;
   }
private:
   // Handle -> ImageControl
   std::map<long, osg::ref_ptr<osgEarth::Util::Controls::ImageControl>>
      m_screen_overlays;
};

class DtedElevationLookup
{
public:
   DtedElevationLookup()
   {
   }
   ~DtedElevationLookup()
   {
      try
      {
         if (m_dted != nullptr)
            m_dted->Terminate();
      }
      catch (_com_error& e)
      {
         REPORT_COM_ERROR(e);
      }
   }
   long GetElevationMeters(double lat, double lon)
   {
      try
      {
         if (m_dted == nullptr)
            CO_CREATE(m_dted, CLSID_Dted);

         short level_used;
         long result = m_dted->GetElevation(lat, lon, 0,
            DTED_ELEVATION_METERS, &level_used);

         // Default to zero if elevation data is missing
         if (result == MISSING_DTED_ELEVATION ||
             result == PARTIAL_DTED_ELEVATION)
         {
            return 0;
         }

         return result;
      }
      catch (_com_error& e)
      {
         REPORT_COM_ERROR(e);
      }
      return 0;
   }
private:
   IDtedPtr m_dted;
};

namespace
{
   osgEarth::AltitudeMode AltitudeModeFromStyle(
      osgEarth::Symbology::Style* style)
   {
      osgEarth::Symbology::AltitudeSymbol* alt_sym =
         style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();

      return alt_sym->clamping() ==
         osgEarth::Symbology::AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN ?
            osgEarth::ALTMODE_RELATIVE : osgEarth::ALTMODE_ABSOLUTE;
   }

   double GetAltitudeForMode(DtedElevationLookup* lookup,
      osgEarth::optional<osgEarth::Symbology::AltitudeSymbol::Clamping> mode,
      double lat, double lon, double alt_meters)
   {
      if (mode == osgEarth::Symbology::AltitudeSymbol::CLAMP_ABSOLUTE)
      {
         return alt_meters;
      }

      long elevation_meters = lookup->GetElevationMeters(lat, lon);

      if (mode ==
         osgEarth::Symbology::AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN)
      {
         return alt_meters + elevation_meters;
      }

      // if (mode ==  osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN)
      {
         return elevation_meters;
      }
   }

   osg::Matrixd GetMatrixTransformForModel(
      const osgEarth::SpatialReference* srs, DtedElevationLookup* lookup,
      osgEarth::optional<osgEarth::Symbology::AltitudeSymbol::Clamping> mode,
      const FalconViewOverlayLib::ModelInstance& model_instance)
   {
      // If necessary, do a DTED lookup to determine the absolute altitude of
      // the model
      double alt_meters = GetAltitudeForMode(lookup, mode, model_instance.lat,
         model_instance.lon, model_instance.alt_meters);

      // set the location of the model. Note it uses longitude, latitude,
      // altitude (not lat, lon)
      osgEarth::GeoPoint geoPoint(srs, model_instance.lon, model_instance.lat,
         alt_meters, osgEarth::ALTMODE_ABSOLUTE);

      osg::Matrixd local2world;
      geoPoint.createLocalToWorld(local2world);
      osg::Matrixd rot;
      rot.makeRotate(osg::DegreesToRadians(model_instance.heading),
         osg::Vec3(0,0,1), osg::DegreesToRadians(model_instance.pitch),
         osg::Vec3(1,0,0), osg::DegreesToRadians(model_instance.roll),
         osg::Vec3(0,1,0));

      return osg::Matrixd::scale(model_instance.scale_x,
         model_instance.scale_y, model_instance.scale_z) * rot * local2world;
   }

   // private class for handling region activation
   class ActiveRegionNode : public osg::Group
   {
   public:
      ActiveRegionNode(DisplayElementRootNode* dern,
         FalconViewOverlayLib::IRegionListener* region_listener) :
      m_display_element_root_node(dern),
         m_region_listener(region_listener),
         m_has_been_activated(false),
         m_last_traversed_frame_num(0)
      {
      }

      void traverse(osg::NodeVisitor& nv)
      {
         if (nv.getTraversalMode() == nv.TRAVERSE_ACTIVE_CHILDREN &&
            nv.getVisitorType() == nv.CULL_VISITOR)
         {
            // Update time stamp
            m_last_traversed_frame_num = nv.getFrameStamp() ?
               nv.getFrameStamp()->getReferenceTime() : 0.0;

            if (!m_has_been_activated)
            {
               m_has_been_activated = true;
               m_display_element_root_node->RegionActivated(
                  m_region_listener, this);
            }
         }

         osg::Group::traverse(nv);
      }

      unsigned int GetLastTraversedFrameNum() const
      {
         return m_last_traversed_frame_num;
      }

      void childRemoved(unsigned int /*pos*/,
         unsigned int /*numChildrenToRemove*/) override
      {
         m_has_been_activated = false;
      }

      osg::BoundingSphere computeBound() const override
      {
         // Always use our parent RegionLOD's bound
         return getParent(0)->getBound();
      }

   protected:
      virtual ~ActiveRegionNode() { }

   private:
      DisplayElementRootNode* m_display_element_root_node;
      FalconViewOverlayLib::IRegionListenerPtr m_region_listener;
      bool m_has_been_activated;
      unsigned int m_last_traversed_frame_num;
   };

   // private class for handling exclusive subregions
   class ExclusiveSubregionsNode : public osg::Group
   {
   public:
      ExclusiveSubregionsNode(DisplayElementRootNode* dern,
         FalconViewOverlayLib::IRegionListener* region_listener) :
      m_display_element_root_node(dern),
         m_region_listener(region_listener),
         m_subregions_added(false),
         m_last_traversed_frame_num(0)
      {
      }

      void traverse(osg::NodeVisitor& nv) override
      {
         if (nv.getTraversalMode() == nv.TRAVERSE_ACTIVE_CHILDREN &&
            nv.getVisitorType() == nv.CULL_VISITOR)
         {
            // Update time stamp
            m_last_traversed_frame_num = nv.getFrameStamp() ?
               nv.getFrameStamp()->getReferenceTime() : 0.0;

            if (!m_subregions_added)
            {
               m_subregions_added = true;
               m_display_element_root_node->InitializeSubregions(
                  m_region_listener, this);
            }
         }

         osg::Group::traverse(nv);
      }

      unsigned int GetLastTraversedFrameNum() const
      {
         return m_last_traversed_frame_num;
      }

      void childRemoved(unsigned int /*pos*/,
         unsigned int /*numChildrenToRemove*/) override
      {
         m_subregions_added = false;
      }

      void CanActivateSubregions(osg::NodeVisitor& nv, bool* can_activate,
         bool* ready_to_render);

      osg::BoundingSphere computeBound() const override
      {
         // Always use our parent RegionLOD's bound
         return getParent(0)->getBound();
      }

   private:
      DisplayElementRootNode* m_display_element_root_node;
      FalconViewOverlayLib::IRegionListener* m_region_listener;
      bool m_subregions_added;
      unsigned int m_last_traversed_frame_num;
   };

   const double REGIONLOD_EXPIRE_TIME_FRAMES = 15 * 60;

   class RegionLOD : public osg::LOD
   {
   public:
      RegionLOD::RegionLOD(osgEarth::MapNode* map_node) :
         m_map_node(map_node), m_active_region_node(nullptr),
            m_subregions_node(nullptr)
         {
         }

         // Initialize children pointers rather than having to get them during
         // each traversal
         void childInserted(unsigned int /*pos*/)
         {
            if (m_active_region_node == nullptr)
            {
               m_active_region_node = static_cast<ActiveRegionNode*>(getChild(0));
            }
            else
            {
               m_subregions_node = static_cast<ExclusiveSubregionsNode*>(
                  getChild(1));
            }
         }

         ActiveRegionNode* GetActiveRegionNode() const
         {
            return m_active_region_node;
         }

         void traverse(osg::NodeVisitor& nv) override
         {
            switch(nv.getTraversalMode())
            {
            case(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN):
               std::for_each(_children.begin(),_children.end(),
                  osg::NodeAcceptOp(nv));
               break;
            case(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN):
               {
                  // Before checking for region activation, allow the
                  // ExclusiveSubregionsNode to become properly initialized
                  if (m_subregions_node->getNumChildren() == 0)
                  {
                     m_subregions_node->accept(nv);
                  }
                  else
                  {
                     bool can_activate_self = CanActivate(nv);
                     bool can_render_self =
                        m_active_region_node->getNumChildren() > 0;

                     bool can_activate_subregions;
                     bool ready_to_render_subregions;
                     m_subregions_node->CanActivateSubregions(nv,
                        &can_activate_subregions, &ready_to_render_subregions);

                     // If this RegionLOD can be activated and none of the
                     // subregions can or if the subregions are not yet ready
                     // to render then accept the ActiveRegionNode
                     if ( (can_activate_self && !can_activate_subregions) ||
                        (can_render_self && !ready_to_render_subregions) )
                     {
                        m_active_region_node->accept(nv);
                     }
                     // Otherwise, accept the ExclusiveSubregionsNode if they
                     // can be activated
                     if (can_activate_subregions)
                     {
                        m_subregions_node->accept(nv);
                     }
                  }
                  break;
               }
            default:
               break;
            }

            // check to see if child node has expired (hasn't been traversed in
            // a certain period of time). If not, then it will be removed in the
            // update traversal
            if (nv.getTraversalMode() ==
               osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN &&
               nv.getVisitorType() == nv.CULL_VISITOR)
            {
               double current_time_stamp = nv.getFrameStamp() ?
                  nv.getFrameStamp()->getReferenceTime() : 0.0;

               if (IsActiveRegionNodeExpired(current_time_stamp))
               {
                  GenericNodeCallback* callback = new GenericNodeCallback;
                  ActiveRegionNode* region = m_active_region_node;
                  callback->SetOperation(m_map_node, [region]()
                  {
                     region->removeChild(0, 1);
                  });

               }
               if (IsSubregionsNodeExpired(current_time_stamp))
               {
                  GenericNodeCallback* callback = new GenericNodeCallback;
                  ExclusiveSubregionsNode* subregions = m_subregions_node;
                  callback->SetOperation(m_map_node, [subregions]()
                  {
                     subregions->removeChildren(0, subregions->getNumChildren());
                  });
               }
            }
         }

         bool CanActivate(osg::NodeVisitor& nv)
         {
            // The first child node is the ActiveRegionNode
            float required_range = computeRequiredRange(nv);
            return _rangeList[0].first <= required_range &&
               required_range <_rangeList[0].second;
         }

   protected:
      virtual ~RegionLOD() { }

   private:
      osgEarth::MapNode* m_map_node;
      ActiveRegionNode* m_active_region_node;
      ExclusiveSubregionsNode* m_subregions_node;

      bool IsActiveRegionNodeExpired(unsigned int crnt_frame_num) const
      {
         if (m_active_region_node->getNumChildren() > 0)
         {
            if (crnt_frame_num -
               m_active_region_node->GetLastTraversedFrameNum() >
               REGIONLOD_EXPIRE_TIME_FRAMES)
            {
               return true;
            }
         }

         return false;
      }

      bool IsSubregionsNodeExpired(unsigned int crnt_frame_num) const
      {
         if (m_subregions_node->getNumChildren() > 0)
         {
            if (crnt_frame_num -
               m_subregions_node->GetLastTraversedFrameNum() >
               REGIONLOD_EXPIRE_TIME_FRAMES)
            {
               return true;
            }
         }

         return false;
      }
   };

   // A RegionLOD that overrides the computation of required range based
   // on a bounding sphere with a radius that differs from the bounding
   // sphere of the geographical bounds
   class RegionLODSigSize : public RegionLOD
   {
   public:
      RegionLODSigSize(osgEarth::MapNode* map_node,
         const osg::BoundingSphere& bsphere) :
      RegionLOD(map_node), m_bsphere(bsphere)
      {
      }

   private:
      float computeRequiredRange(osg::NodeVisitor& nv) override
      {
         float required_range = 0.0f;
         osg::CullStack* cullStack = dynamic_cast<osg::CullStack*>(&nv);
         if (cullStack && cullStack->getLODScale())
         {
            required_range = cullStack->clampedPixelSize(m_bsphere)
               / cullStack->getLODScale();
         }
         else
         {
            // fallback to selecting the highest res tile by
            // finding out the max range
            for(unsigned int i=0;i<_rangeList.size();++i)
            {
               required_range = osg::maximum(required_range,
                  _rangeList[i].first);
            }
         }

         return required_range;
      }
      osg::BoundingSphere m_bsphere;
   };

   void ExclusiveSubregionsNode::CanActivateSubregions(
      osg::NodeVisitor& nv, bool* can_activate, bool* ready_to_render)
   {
      // ExclusiveSubregionsNode -> Group -> RegionLODs
      osg::Group* group = getChild(0)->asGroup();

      *can_activate = false;
      *ready_to_render = true;
      unsigned int num_children = group->getNumChildren();
      for (unsigned int i=0; i<num_children; ++i)
      {
         RegionLOD* plod = dynamic_cast<RegionLOD*>(group->getChild(i));
         if (plod != nullptr)
         {
            if (!plod->CanActivate(nv))
            {
               *can_activate = false;
               *ready_to_render = false;
               return;
            }

            // There is at least one active subregion and all of them can
            // be active
            *can_activate = true;

            ActiveRegionNode* region = plod->GetActiveRegionNode();
            *ready_to_render &= region && region->getNumChildren() > 0;
         }
      }
   }
}

// Private structure for handling the caching of osg::Images for the lifetime
// of the OverlayElements
struct ImageCache
{
   ImageCache() : next_image_handle(1)
   {
   }

   std::map<long, osg::ref_ptr<osg::Image>> image_handle_to_image;
   std::map<std::wstring, long> image_filename_to_handle;
   long next_image_handle;
};

OverlayElements::OverlayElements(osgEarth::MapNode* map_node,
   osgEarth::Util::Controls::ControlCanvas* canvas) : m_map_node(map_node),
   m_parent_node(nullptr),
   m_control_canvas(canvas),
   m_display_element_root(nullptr),
   m_next_screen_overlay_handle(1),
   m_begin_time(0.0), m_end_time(0.0),
   m_parent_model_node(nullptr),
   m_delayed_xform_adder(nullptr)
{
   m_current_style = new osgEarth::Symbology::Style;
   m_background_style = new osgEarth::Symbology::Style;

   // Setup defaults for pen, font, and altitude type
   //
   SetPen(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, false, 3.0f,
      UTIL_LINE_SOLID);
   SetFont("Arial", 12, RGB(255, 255, 255), RGB(0, 0, 0));

   osgEarth::Symbology::AltitudeSymbol* alt_sym =
      m_current_style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();
   alt_sym->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;

   alt_sym =
      m_background_style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();
   alt_sym->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;

   m_screen_overlays = new ScreenOverlaysCollection;
   m_dted = new DtedElevationLookup;

   m_image_cache = new ImageCache;
}

void OverlayElements::SetParentNode(
   DisplayElementRootNode* display_element_root, osg::Group* parent_node,
   bool generate_object_handles /* = true */)
{
   m_display_element_root = display_element_root;
   display_element_root->SetControlCanvas(m_control_canvas);
   m_parent_node = parent_node;
   m_parent_node->ref();
   m_generate_object_handles = generate_object_handles;
}

class DelayedMatrixTransformAdder
{
public:
   DelayedMatrixTransformAdder(osgEarth::MapNode* map_node) :
      m_map_node(map_node)
   {
   }

   void AddChild(osg::Group* parent, osg::Node* child)
   {
      m_xforms_to_add_in_update_traversal.push_back(std::make_pair(parent,
         child));
   }

   ~DelayedMatrixTransformAdder()
   {
      if (!m_xforms_to_add_in_update_traversal.empty())
      {
         GenericNodeCallback* callback = new GenericNodeCallback;
         auto xforms = m_xforms_to_add_in_update_traversal;
         callback->SetOperation(m_map_node, [xforms]()
         {
            auto it = xforms.begin();
            while (it != xforms.end())
            {
               // Add MatrixTransform -> ModelPagedLOD
               it->first->addChild(it->second);
               ++it;
            }
         });
      }
   }

private:
   osgEarth::MapNode* m_map_node;
   std::vector<std::pair<osg::ref_ptr<osg::Group>, osg::ref_ptr<osg::Node>>>
      m_xforms_to_add_in_update_traversal;
};

OverlayElements::~OverlayElements()
{
   delete m_screen_overlays;
   delete m_current_style;
   delete m_background_style;
   delete m_dted;
   if (m_parent_node)
      m_parent_node->unref();
   delete m_image_cache;
   delete m_delayed_xform_adder;
}

// set the current pen
long OverlayElements::SetPen(float fg_red, float fg_green, float fg_blue,
   float bg_red, float bg_green, float bg_blue, float opacity,
   long turn_off_background, float line_width, long line_style)
{
   osgEarth::optional<osgEarth::Symbology::Stroke> stroke =
      m_current_style->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke();
   osgEarth::optional<osgEarth::Symbology::Stroke> background_stroke =
      m_background_style->getOrCreate<osgEarth::Symbology::LineSymbol>()->
      stroke();

   stroke->color() = osgEarth::Color(fg_red, fg_green, fg_blue, opacity);
   background_stroke->color() = osgEarth::Color(bg_red, bg_green, bg_blue,
      opacity);
   stroke->width() = line_width;
   background_stroke->width() = line_width + 2;

   switch (line_style)
   {
   // currently unsupported - use SOLID
   // Also, see SegmentAttributePage::UpdateStyleNote when changing.
   case UTIL_LINE_ROUND_DOT:
   case UTIL_LINE_DIAMOND:
   case UTIL_LINE_ARROW:

   case UTIL_LINE_SOLID:
      stroke->stipple() = 0xffff;
      break;
   case UTIL_LINE_SQUARE_DOT:
      stroke->stipple() = 0xfcfc;
      break;
   case UTIL_LINE_DASH_DOT:
      stroke->stipple() = 0xff8c;
      break;
   case UTIL_LINE_LONG_DASH:
      stroke->stipple() = 0xfffc;
      break;
   }

   m_current_style->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke() =
      stroke;
   m_background_style->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()=
      background_stroke;

   return SUCCESS;
}

// set the current brush
long OverlayElements::SetBrush(
   float red, float green, float blue, float opacity)
{
   m_current_style->getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->
      color() = osgEarth::Color(red, green, blue, opacity);

   return SUCCESS;
}

long OverlayElements::SetAltitudeMode(
   osgEarth::Symbology::AltitudeSymbol* alt_sym,
   FalconViewOverlayLib::AltitudeMode mode)
{
   using namespace FalconViewOverlayLib;
   using namespace osgEarth::Symbology;

   switch (mode)
   {
   case ALTITUDE_MODE_CLAMP_TO_TERRAIN:
      alt_sym->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
      break;
   case ALTITUDE_MODE_RELATIVE_TO_TERRAIN:
      alt_sym->clamping() = AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN;
      break;
   case ALTITUDE_MODE_ABSOLUTE:
      alt_sym->clamping() = AltitudeSymbol::CLAMP_ABSOLUTE;
      break;
   default:
      return FAILURE;
   }

   return SUCCESS;
}

long OverlayElements::SetAltitudeMode(
   FalconViewOverlayLib::AltitudeMode mode)
{
   osgEarth::Symbology::AltitudeSymbol* alt_sym = 
      m_current_style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();
   if (SetAltitudeMode(alt_sym, mode) == SUCCESS)
   {
      alt_sym =
         m_background_style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();
      return SetAltitudeMode(alt_sym, mode);
   }
   return FAILURE;

}

long OverlayElements::SetIconAlignmentMode(
   FalconViewOverlayLib::IconAlignmentMode mode)
{
   using namespace FalconViewOverlayLib;
   using namespace osgEarth::Symbology;

   osgEarth::Symbology::IconSymbol* icon_symbol =
      m_current_style->getOrCreate<osgEarth::Symbology::IconSymbol>();
   switch (mode)
   {
   case ALIGN_LEFT_TOP:
      icon_symbol->alignment() = IconSymbol::ALIGN_LEFT_TOP;
      break;
   case ALIGN_LEFT_CENTER:
      icon_symbol->alignment() = IconSymbol::ALIGN_LEFT_CENTER;
      break;
   case ALIGN_LEFT_BOTTOM:
      icon_symbol->alignment() = IconSymbol::ALIGN_LEFT_BOTTOM;
      break;

   case ALIGN_CENTER_TOP:
      icon_symbol->alignment() = IconSymbol::ALIGN_CENTER_TOP;
      break;
   case ALIGN_CENTER_CENTER:
      icon_symbol->alignment() = IconSymbol::ALIGN_CENTER_CENTER;
      break;
   case ALIGN_CENTER_BOTTOM:
      icon_symbol->alignment() = IconSymbol::ALIGN_CENTER_BOTTOM;
      break;

   case ALIGN_RIGHT_TOP:
      icon_symbol->alignment() = IconSymbol::ALIGN_RIGHT_TOP;
      break;
   case ALIGN_RIGHT_CENTER:
      icon_symbol->alignment() = IconSymbol::ALIGN_RIGHT_CENTER;
      break;
   case ALIGN_RIGHT_BOTTOM:
      icon_symbol->alignment() = IconSymbol::ALIGN_RIGHT_BOTTOM;
      break;

   default:
      return FAILURE;
   }

   return SUCCESS;
}  // SetIconAlignmentMode()

long OverlayElements::SetIconDeclutterMode( BOOL bDeclutter )
{
   using namespace FalconViewOverlayLib;
   using namespace osgEarth::Symbology;

   osgEarth::Symbology::IconSymbol* pIconSymbol =
      m_current_style->getOrCreate< osgEarth::Symbology::IconSymbol >();
   pIconSymbol->declutter() = bDeclutter ? true : false;
   return SUCCESS;
}

long OverlayElements::CreateImageFromFilename(BSTR filename)
{
   // First, see if the user has already requested an image with the given
   // filename (prevent it from being multiply loaded)
   auto it = m_image_cache->image_filename_to_handle.find(filename);
   if (it != m_image_cache->image_filename_to_handle.end())
      return it->second;

   osg::ref_ptr<osgDB::ReaderWriter::Options> options =
      new osgDB::ReaderWriter::Options;
   auto result = osgDB::Registry::instance()->readImage(
      (char *)_bstr_t(filename), options);

   if (result.validImage())
   {
      m_image_cache->image_handle_to_image[m_image_cache->next_image_handle] =
         result.takeImage();
      m_image_cache->image_filename_to_handle[filename] =
         m_image_cache->next_image_handle;
      return m_image_cache->next_image_handle++;
   }

   return -1;
}

long OverlayElements::CreateImageFromRawBytes(BYTE* raw_bytes, long num_bytes)
{
   // Create an IStream instance from the given bytes
   IStreamPtr image_stream;
   HGLOBAL h = ::GlobalAlloc(GMEM_MOVEABLE, num_bytes);
   if ( h == nullptr )
   {
      ERR_report( "GlobalAlloc() failed" );
      return -1;
   }
   LPVOID pmem = ::GlobalLock(h);
   ::RtlCopyMemory( pmem, raw_bytes, num_bytes );
   ::CreateStreamOnHGlobal( h, TRUE, &image_stream );
   ::GlobalUnlock( h );

   return CreateImageFromRawBytes( image_stream );
}  // CreateImageFromRawBytes( bytes )

long OverlayElements::CreateImageFromRawBytes( IStream* image_stream )
{
   if ( image_stream != nullptr )
   {   
      osg::Image* image;
      if ( nullptr != ( image = osg_image_utils::ImageFromStream( image_stream ) ) )
      {
         m_image_cache->image_handle_to_image[m_image_cache->next_image_handle]
            = image;
         return m_image_cache->next_image_handle++;
      }
   }
   return -1;
}  // CreateImageFromRawBytes( stream )

long OverlayElements::GetImageWidth(long image_handle)
{
   auto image_it = m_image_cache->image_handle_to_image.find(image_handle);
   if (image_it == m_image_cache->image_handle_to_image.end())
   {
      CString msg;
      msg.Format(
         "GetImageWidth failed: invalid image handle: %d", image_handle);
      ERR_report(msg);
      return -1;
   }

   return image_it->second->s();
}

long OverlayElements::GetImageHeight(long image_handle)
{
   auto image_it = m_image_cache->image_handle_to_image.find(image_handle);
   if (image_it == m_image_cache->image_handle_to_image.end())
   {
      CString msg;
      msg.Format(
         "GetImageHeight failed: invalid image handle: %d", image_handle);
      ERR_report(msg);
      return -1;
   }

   return image_it->second->t();
}

long OverlayElements::CreateModelFromFilename(BSTR filename,
   double distance_to_load_meters, double distance_to_load_texture_meters,
   BSTR default_texture_name, double bounding_sphere_radius_meters)
{
   return m_display_element_root->CreateModelFromFilename(filename,
      distance_to_load_meters, distance_to_load_texture_meters,
      default_texture_name, bounding_sphere_radius_meters);
}

long OverlayElements::SetFont(const std::string& font_name, float font_size,
   COLORREF fg_color, COLORREF bg_color)
{
   osgEarth::Annotation::TextSymbol* text_symbol =
      m_current_style->getOrCreate<osgEarth::Annotation::TextSymbol>();

   std::string font_name_with_extension = font_name;
   font_name_with_extension += ".ttf";

   text_symbol->font() = font_name_with_extension;
   text_symbol->size() = font_size;
   text_symbol->fill()->color() = osgEarth::Color(
      GetRValue(fg_color) / 255.0f, GetGValue(fg_color) / 255.0f,
      GetBValue(fg_color) / 255.0f);
   text_symbol->halo()->color() = osgEarth::Color(
      GetRValue(bg_color) / 255.0f, GetGValue(bg_color) / 255.0f,
      GetBValue(bg_color) / 255.0f);

   return SUCCESS;
}

long OverlayElements::GetHandleForNode(osg::Node* node)
{
   // If the parent node is an ActiveRegion, then deleting of objects in the
   // region is unsupported (as the region will be added/removed atomically)
   return m_generate_object_handles ?
      m_display_element_root->AddNode(node) : MAXLONG;
}

long OverlayElements::AddArc(double lat, double lon, double radius,
   double heading_to_start, double turn_arc, BOOL clockwise)
{
   // compute the heading to the arc end point by using the turn_arc and the arc
   // direction
   double heading_to_end;
   if (clockwise)
   {
      // plus the turn arc for clockwise arcs
      heading_to_end = heading_to_start + turn_arc;
      if (heading_to_end >= 360.0)
         heading_to_end -= 360.0;
   }
   else
   {
      // minus the turn arc for counter-clockwise arcs
      heading_to_end = heading_to_start - turn_arc;
      if (heading_to_end < 0.0)
         heading_to_end += 360.0;
   }

   osgEarth::Annotation::EllipseNode* ellipse_node =
      new osgEarth::Annotation::EllipseNode(m_map_node,
         osgEarth::GeoPoint(m_map_node->getMapSRS(), lon, lat),
         osgEarth::Distance(radius),
         osgEarth::Distance(radius),
         osgEarth::Angle(0.0), *m_current_style,
         osgEarth::Angle(heading_to_start),
         osgEarth::Angle(heading_to_end));

   // Add the feature node to the parent node
   m_parent_node->addChild(ellipse_node);

   return GetHandleForNode(ellipse_node);
}

long OverlayElements::AddGroundOverlay(long image_handle,
   double north, double west, double south, double east)
{
   auto image_it = m_image_cache->image_handle_to_image.find(image_handle);
   if (image_it == m_image_cache->image_handle_to_image.end())
   {
      CString msg;
      msg.Format(
         "AddGroundOverlay failed: invalid image handle: %d", image_handle);
      ERR_report(msg);
      return -1;
   }

   osgEarth::Annotation::ImageOverlay* image_overlay =
      new osgEarth::Annotation::ImageOverlay(m_map_node, image_it->second,
         true);

   image_overlay->setCorners(osg::Vec2d(west, south), osg::Vec2d(east, south),
      osg::Vec2d(west, north), osg::Vec2d(east, north));

   PlaybackTimeChangedNodeVisitor::SetNodeTimeIntervalAndMask(
      m_begin_time, m_end_time, image_overlay);

   // Add the feature node to the parent node
   m_parent_node->addChild(image_overlay);

   return GetHandleForNode(image_overlay);
}

namespace
{
   class ModelMatrixTransform : public osg::MatrixTransform
   {
   public:
      ModelMatrixTransform(const osg::Matrix& mtx, double scale) :
         osg::MatrixTransform(mtx), m_child_paged_lod(nullptr)
      {
         m_scale_factor = static_cast<float>(scale);
      }

      void traverse(osg::NodeVisitor& nv) override
      {
         if (m_child_paged_lod)
            m_child_paged_lod->setScaleFactor(m_scale_factor);

         osg::MatrixTransform::traverse(nv);
      }

      void childInserted(unsigned int /*pos*/)
      {
         m_child_paged_lod = dynamic_cast<ModelPagedLOD* >(getChild(0));
      }

   private:
      float m_scale_factor;
      ModelPagedLOD* m_child_paged_lod;
   };
}

long OverlayElements::AddModel(long model_handle,
   const FalconViewOverlayLib::ModelInstance& model_instance)
{
   osg::Node* model = m_display_element_root->GetModel(model_handle);
   if (model == nullptr)
   {
      CString msg;
      msg.Format("AddModel failed: invalid model handle: %d", model_handle);
      ERR_report(msg);
      return -1;
   }

   osgEarth::Symbology::AltitudeSymbol *alt_sym =
      m_current_style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();

   osg::Matrixd mtx = GetMatrixTransformForModel(
      m_map_node->getMap()->getProfile()->getSRS(), m_dted,
      alt_sym->clamping(), model_instance);

   if (m_parent_model_node == nullptr)
   {
      m_parent_model_node = new osg::Group;
      m_parent_node->addChild(m_parent_model_node);
   }

   ModelMatrixTransform* xform = new ModelMatrixTransform(
      mtx, max(model_instance.scale_x, max(model_instance.scale_y,
         model_instance.scale_z)));
   m_parent_model_node->addChild(xform);

   // Because the model (potentially a ModelPagedLOD) is shared between work
   // items, it can be modified (for instance, children added) during the
   // update traversal. That means we cannot also modify it in the work item.
   if (!m_delayed_xform_adder)
      m_delayed_xform_adder = new DelayedMatrixTransformAdder(m_map_node);
   m_delayed_xform_adder->AddChild(xform, model);

   // Assumes models are always added in region listeners or will last
   // for the lifetime of the overlay (i.e., cannot be deleted once added).
   // This is so that we can optimize the scene graph as necessary.
   return MAXLONG;
}

long OverlayElements::AddModels(BSTR filename, SAFEARRAY* model_instances)
{
   // First, attempt to load the model with the given filename
   std::string model_name_str = (char *)_bstr_t(filename);
   osg::Node* model = osgDB::readNodeFile(model_name_str);
   if (model == nullptr)
      return -1;

   // Temporarily attach to SafeArray (be sure to call Detach so the SAFEARRAY
   // is not destroyed)
   SafeArray<FalconViewOverlayLib::ModelInstance, VT_RECORD> sa;
   sa.Attach(model_instances);

   int num_instances = sa.GetNumElements();

   const osgEarth::Capabilities& caps =
      osgEarth::Registry::instance()->getCapabilities();

   int max_texture_size = caps.getMaxRectTextureSize();
   unsigned int max_matrices_per_row = max_texture_size / 4u;

   if (num_instances > (int)max_matrices_per_row * max_texture_size)
   {
      CString msg;
      msg.Format("AddModels failed [Invalid Argument]: "
         "Number of instances given, %d, exceeds maximum number possible "
         "on this GPU, %d", num_instances,
         max_matrices_per_row * max_texture_size);
      ERR_report(msg);
      sa.Detach();
      return -1;
   }

   unsigned int matrices_per_row = static_cast<unsigned int>(
      ceil(sqrt(static_cast<double>(num_instances))));
   matrices_per_row = min(matrices_per_row, max_matrices_per_row);
   int texture_size = matrices_per_row * 4;

   // create texture to encode all model instances
   //
   unsigned int width = texture_size;
   unsigned int height = num_instances / matrices_per_row + 1u;

   osg::ref_ptr<osg::Image> image = new osg::Image;
   image->allocateImage(width, height, 1, GL_RGBA, GL_FLOAT);
   image->setInternalTextureFormat(GL_RGBA32F_ARB);

   const osgEarth::SpatialReference* srs =
      m_map_node->getMap()->getProfile()->getSRS();

   osgEarth::Symbology::AltitudeSymbol *alt_sym =
      m_current_style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();

   // calculate the bounding box for the model
   osg::ComputeBoundsVisitor cbv;
   model->accept(cbv);
   const osg::BoundingBox& model_bounds = cbv.getBoundingBox();

   osg::BoundingBox bbox;
   auto clamping = alt_sym->clamping();
   float* data = (float *)image->data();
   for (int i=0; i<num_instances; ++i)
   {
      osg::Matrixf matrix = GetMatrixTransformForModel(srs, m_dted, clamping,
         sa[i]);

      // Update the bounding box for the collection of models
      for (int c=0; c<8; ++c)
         bbox.expandBy(model_bounds.corner(c) * matrix);

      memcpy(data, matrix.ptr(), 16 * sizeof(float));
      data += 16;
   }

   m_parent_node->addChild(model);

   osg::ref_ptr<osg::TextureRectangle> texture =
      new osg::TextureRectangle(image);
   texture->setInternalFormat(GL_RGBA32F_ARB);
   texture->setSourceFormat(GL_RGBA);
   texture->setSourceType(GL_FLOAT);
   //texture->setTextureSize(width, height);
   texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
   texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
   texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
   texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
   texture->setUnRefImageDataAfterApply(true);

   osgEarth::ShaderGenerator gen(osgEarth::Registry::stateSetCache(),
      num_instances, texture, &bbox);
   model->accept(gen);

   sa.Detach();

   return SUCCESS;
}

long OverlayElements::AddBitmap(const std::string& filename, double lat,
   double lon)
{
   osg::ref_ptr<osg::Image> image =
      osg_image_utils::ImageFromFilename(_bstr_t(filename.c_str()));

   if (!image.valid())
   {
      ERR_report("AddBitmap failed: unable to load image");
      return -1;
   }

   osgEarth::Annotation::ImageOverlay* image_overlay =
      new osgEarth::Annotation::ImageOverlay(m_map_node, image);

   image_overlay->setCenter(lon, lat);

   PlaybackTimeChangedNodeVisitor::SetNodeTimeIntervalAndMask(
      m_begin_time, m_end_time, image_overlay);

   // Add the feature node to the parent node
   m_parent_node->addChild(image_overlay);

   return GetHandleForNode(image_overlay);
}

long OverlayElements::AddEllipse(double lat, double lon, double alt_meters,
   double vertical_meters, double horizontal_meters, double rotation_deg)
{
   using namespace osgEarth::Annotation;

   // Remove the IconSymbol
   osg::ref_ptr<IconSymbol> icon = m_current_style->getSymbol<IconSymbol>();
   if (icon.valid())
      m_current_style->removeSymbol(icon);

   osgEarth::Annotation::EllipseNode* ellipse_node =
      new osgEarth::Annotation::EllipseNode(
         m_map_node,
         osgEarth::GeoPoint(m_map_node->getMapSRS(), lon, lat, alt_meters,
            AltitudeModeFromStyle(m_current_style)),
         osgEarth::Distance(vertical_meters),
         osgEarth::Distance(horizontal_meters),
         osgEarth::Angle(rotation_deg), *m_current_style,
         0.0, 360.0, false, osg::CopyOp::SHALLOW_COPY);

   PlaybackTimeChangedNodeVisitor::SetNodeTimeIntervalAndMask(
      m_begin_time, m_end_time, ellipse_node);

   // Add the feature node to the parent node
   m_parent_node->addChild(ellipse_node);

   if (icon.valid())
      m_current_style->addSymbol(icon);

   return GetHandleForNode(ellipse_node);
}

long OverlayElements::AddLine(
   const std::vector< std::tuple<double, double, double> >& coords)
{
   osgEarth::Symbology::LineString* line_string =
      new osgEarth::Symbology::LineString(coords.size());
   for (auto it = coords.begin(); it != coords.end(); ++it)
   {
      double lat, lon, alt;
      std::tie(lat, lon, alt) = *it;
      line_string->push_back(lon, lat, alt);
   }

   osgEarth::Annotation::FeatureNode* background_line =
      CreateFromGeometry(line_string, false, m_background_style);
   osgEarth::Annotation::FeatureNode* line =
      CreateFromGeometry(line_string, false, m_current_style);

   osg::Group* group = new osg::Group;

   PlaybackTimeChangedNodeVisitor::SetNodeTimeIntervalAndMask(
      m_begin_time, m_end_time, group);

   // order matters - first added last drawn
   group->addChild(line);
   group->addChild(background_line);

   return AddNode(group);
}

long OverlayElements::AddImage(long image_handle,
      const std::vector< std::pair<double, double> >& lat_lons,
      const std::vector<double>& altitudes_meters,
      const std::vector<std::string>& icon_text,
      double scale, double heading_degrees)
{
   auto image_it = m_image_cache->image_handle_to_image.find(image_handle);
   if (image_it == m_image_cache->image_handle_to_image.end())
   {
      CString msg;
      msg.Format("AddImage failed: invalid image handle: %d", image_handle);
      ERR_report(msg);
      return -1;
   }

   // Handle rotation
   osgEarth::Annotation::IconSymbol* icon_symbol =
      m_current_style->getOrCreate<osgEarth::Annotation::IconSymbol>();
   icon_symbol->heading() = heading_degrees;

   // Handle scaling
   osg::Image* image = nullptr;
   osg::ref_ptr<osg::Image> scaled_image;
   if (scale == 1.0)
      image = image_it->second;
   else
   {
      scaled_image = new osg::Image(*image_it->second);
      scaled_image->scaleImage(
         static_cast<int>(scaled_image->s() * scale + 0.5),
         static_cast<int>(scaled_image->t() * scale + 0.5), 1);
      image = scaled_image.get();
   }

   int last_handle_added = -1;

   osgEarth::Symbology::AltitudeSymbol *alt_sym =
      m_current_style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();

   auto lat_lon_it = lat_lons.begin();
   auto alt_it = altitudes_meters.begin();
   auto text_it = icon_text.begin();
   for (; lat_lon_it != lat_lons.end() && text_it != icon_text.end();
      ++lat_lon_it, ++alt_it, ++text_it)
   {
      double lat = lat_lon_it->first;
      double lon = lat_lon_it->second;
      double alt = GetAltitudeForMode(m_dted, alt_sym->clamping(), lat, lon,\
         *alt_it);
      osgEarth::Annotation::PlaceNode* place_node =
         new osgEarth::Annotation::PlaceNode(m_map_node,
            osgEarth::GeoPoint(m_map_node->getMapSRS(), lon, lat,
               alt, osgEarth::ALTMODE_ABSOLUTE),
            image, *text_it, *m_current_style, osg::CopyOp::SHALLOW_COPY);

      PlaybackTimeChangedNodeVisitor::SetNodeTimeIntervalAndMask(
         m_begin_time, m_end_time, place_node);

      // Add the feature node to the parent node
      m_parent_node->addChild(place_node);

      last_handle_added = GetHandleForNode(place_node);
   }

   // Return the first handle to the caller
   return last_handle_added - lat_lons.size() + 1;
}

long OverlayElements::AddPolygon(
   const std::vector< std::tuple<double, double, double> > &coords,
   const std::vector<long> &point_counts, bool extruded)
{
   // coords is all of the polygon coordinates, including the coordinates of
   // any inner rings.  point_counts is a vector that tells how many coordinates
   // are in each ring.  Outer boundry is wound CCW and inner rings are CW.

   osgEarth::Symbology::Polygon* polygon = new osgEarth::Symbology::Polygon(
      point_counts[0]); // reserve capacity of outer ring

   // add the outer ring

   size_t coords_index = 0;

   while (coords_index < (size_t)point_counts[0])
   {
      double lat, lon, alt;
      std::tie(lat, lon, alt) = coords[coords_index];
      polygon->push_back(osg::Vec3d(lon, lat, alt));
      ++coords_index;
   }

   // add inner rings

   osgEarth::RingCollection &holes = polygon->getHoles();

   for (size_t inner_ring_num = 1; inner_ring_num < point_counts.size();
      ++inner_ring_num)
   {
      size_t count = point_counts[inner_ring_num];
      osg::ref_ptr<osgEarth::Ring> ring = new osgEarth::Ring;
      holes.push_back(ring);

      for (size_t i = 0; i < count; ++i)
      {
         double lat, lon, alt;
         std::tie(lat, lon, alt) = coords[coords_index];
         ring->push_back(osg::Vec3d(lon, lat, alt));
         ++coords_index;
      }
   }

   return AddFromGeometry(polygon, extruded);
}

long OverlayElements::AddScreenOverlay(IStream* image_stream)
{
   osg::ref_ptr<osg::Image> image =
      osg_image_utils::ImageFromStream(image_stream);

   if (!image.valid())
   {
      ERR_report("AddScreenOverlay failed: unable to load image");
      return -1;
   }

   return AddScreenOverlay(image, 0.5, 0.5, 0.5, SCREEN_ANCHOR_UNITS_FRACTION,
      0.5, SCREEN_ANCHOR_UNITS_FRACTION, 0.5, 0.5, 0.0, 1.0, 1.0);
}

long OverlayElements::AddScreenOverlay(long image_handle,
   double image_anchor_x, double image_anchor_y,
   double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
   double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
   double rotation_center_x, double rotation_center_y,
   double rotation_degrees, double x_scale, double y_scale)
{
   auto image_it = m_image_cache->image_handle_to_image.find(image_handle);
   if (image_it == m_image_cache->image_handle_to_image.end())
   {
      CString msg;
      msg.Format(
         "AddScreenOverlay failed: invalid image handle: %d", image_handle);
      ERR_report(msg);
      return -1;
   }

   return AddScreenOverlay(image_it->second,
      image_anchor_x, image_anchor_y,
      screen_anchor_x, screen_anchor_x_units,
      screen_anchor_y, screen_anchor_y_units,
      rotation_center_x, rotation_center_y, rotation_degrees,
      x_scale, y_scale);
}

long OverlayElements::AddScreenOverlay(osg::ref_ptr<osg::Image> image,
   double image_anchor_x, double image_anchor_y,
   double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
   double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
   double rotation_center_x, double rotation_center_y,
   double rotation_degrees, double x_scale, double y_scale)
{
   // initialize an image control

   osg::ref_ptr<osgEarth::Util::Controls::ImageControl> image_control =
      new osgEarth::Util::Controls::ImageControl(image);

   InitScreenOverlayImageControl(image_control, image_anchor_x, image_anchor_y,
      screen_anchor_x, screen_anchor_x_units, screen_anchor_y,
      screen_anchor_y_units, rotation_center_x, rotation_center_y,
      rotation_degrees, x_scale, y_scale);

   // TODO: time sensitivity

   // add the control in a rendering thread

   GenericNodeCallback* callback = new GenericNodeCallback;
   auto control_canvas = m_control_canvas;
   callback->SetOperation(m_map_node, [control_canvas, image_control]()
   {
      control_canvas->addControl(image_control.get());
   });

   // generate a handle and possible add to display element root

   long handle = m_next_screen_overlay_handle++;

   if (m_display_element_root != nullptr)
   {
      // This is an overlay, so we have to remove the image control when
      // the overlay is cleaned up.  We don't do this step with the center
      // crosshair.
      handle = m_display_element_root->AddControl(image_control);
   }

   m_screen_overlays->Add(handle, image_control);

   return handle;
}

/*static*/ void OverlayElements::InitScreenOverlayImageControl(
   osg::ref_ptr<osgEarth::Util::Controls::ImageControl> image_control,
   double image_anchor_x, double image_anchor_y,
   double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
   double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
   double rotation_center_x, double rotation_center_y,
   double rotation_degrees, double x_scale, double y_scale)
{
   image_control->setAnchorX(image_anchor_x);
   image_control->setAnchorY(image_anchor_y);

   InitControlScreenAnchors(image_control.get(), screen_anchor_x,
      screen_anchor_x_units, screen_anchor_y, screen_anchor_y_units);

   image_control->setRotationCenterX(rotation_center_x);
   image_control->setRotationCenterY(rotation_center_y);
   image_control->setRotation(-rotation_degrees);
}

/* static */ void OverlayElements::InitControlScreenAnchors(
   osgEarth::Util::Controls::Control* control,
   double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
   double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units)
{
   switch (screen_anchor_x_units)
   {
   case SCREEN_ANCHOR_UNITS_PIXELS:
      control->setHorizAlign(
         osgEarth::Util::Controls::Control::ALIGN_OFFSET_PIXELS);
      break;
   case SCREEN_ANCHOR_UNITS_INSET_PIXELS:
      control->setHorizAlign(
         osgEarth::Util::Controls::Control::ALIGN_INSET_PIXELS);
      break;
   case SCREEN_ANCHOR_UNITS_FRACTION:
      control->setHorizAlign(
         osgEarth::Util::Controls::Control::ALIGN_OFFSET_FRACTION);
      break;
   }

   switch (screen_anchor_y_units)
   {
   case SCREEN_ANCHOR_UNITS_PIXELS:
      control->setVertAlign(
         osgEarth::Util::Controls::Control::ALIGN_OFFSET_PIXELS);
      break;
   case SCREEN_ANCHOR_UNITS_INSET_PIXELS:
      control->setVertAlign(
         osgEarth::Util::Controls::Control::ALIGN_INSET_PIXELS);
      break;
   case SCREEN_ANCHOR_UNITS_FRACTION:
      control->setVertAlign(
         osgEarth::Util::Controls::Control::ALIGN_OFFSET_FRACTION);
      break;
   }

   control->setX(screen_anchor_x);
   control->setY(screen_anchor_y);
}

long OverlayElements::ShowScreenOverlay(long screen_overlay_handle,
   bool show_not_hide)
{
   auto screen_overlay = m_screen_overlays->Find(screen_overlay_handle);
   if (screen_overlay == nullptr)
   {
      ERR_report("ShowScreenOverlay failed: invalid handle");
      return -1;
   }

   screen_overlay->setVisible(show_not_hide);
   return SUCCESS;
}

long OverlayElements::AddCameraPositionIndicatorControl(
   double lat, double lon, double alt_meters,
   double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
   double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
   COLORREF color)
{
   const double length_in_pixels = 30;

   d_geo_t target = { lat, lon };

   // Convert target lat, lon, and altitude to world space
   osg::Vec3d target_vec;
   m_map_node->getMap()->getSRS()->getEllipsoid()->convertLatLongHeightToXYZ(
      osg::DegreesToRadians(lat), osg::DegreesToRadians(lon),
      alt_meters, target_vec.x(), target_vec.y(), target_vec.z());

   CameraPositionIndicatorControl* cpic = new CameraPositionIndicatorControl(
      target, target_vec, length_in_pixels, color);
   OverlayElements::InitControlScreenAnchors(cpic, screen_anchor_x,
      screen_anchor_x_units, screen_anchor_y, screen_anchor_y_units);

   GenericNodeCallback* callback = new GenericNodeCallback;
   auto control_canvas = m_control_canvas;
   callback->SetOperation(m_map_node, [cpic, control_canvas]()
   {
      control_canvas->addControl(cpic);
   });

   // generate a handle and possible add to display element root
   return m_display_element_root->AddControl(cpic);
}

long OverlayElements::AddText(double lat, double lon, double alt_meters,
   const std::string& text, long x_offset, long y_offset)
{
   osgEarth::Annotation::TextSymbol* text_symbol =
      m_current_style->getOrCreate<osgEarth::Annotation::TextSymbol>();
   text_symbol->pixelOffset() = osg::Vec2s(x_offset, y_offset);

   osgEarth::Symbology::AltitudeSymbol *alt_sym =
      m_current_style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();
   alt_meters = GetAltitudeForMode(m_dted, alt_sym->clamping(), lat, lon,
      alt_meters);

   osgEarth::GeoPoint geoPoint(m_map_node->getMap()->getProfile()->getSRS(),
      lon, lat, alt_meters, osgEarth::ALTMODE_ABSOLUTE);

   osgEarth::Annotation::PlaceNode* place_node =
      new osgEarth::Annotation::PlaceNode(m_map_node, geoPoint, text,
      *m_current_style, osg::CopyOp::SHALLOW_COPY);

   PlaybackTimeChangedNodeVisitor::SetNodeTimeIntervalAndMask(
      m_begin_time, m_end_time, place_node);

   // Add the feature node to the parent node
   m_parent_node->addChild(place_node);

   return GetHandleForNode(place_node);
}

osgEarth::Annotation::FeatureNode* OverlayElements::CreateFromGeometry(
   osgEarth::Symbology::Geometry* geometry, bool extruded,
   osgEarth::Symbology::Style* style)
{
   // create the feature
   osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(
      geometry, m_map_node->getMapSRS());
   feature->geoInterp() = osgEarth::Symbology::GEOINTERP_GREAT_CIRCLE;

   // set clamping based on the current style

   osgEarth::Symbology::AltitudeSymbol* current_alt_sym =
      style->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();

   *feature->style()->getOrCreate<osgEarth::Symbology::LineSymbol>() =
      *style->getOrCreate<osgEarth::Symbology::LineSymbol>();

   if (geometry->getType() == osgEarth::Geometry::TYPE_POLYGON)
   {
      *feature->style()->getOrCreate<osgEarth::Symbology::PolygonSymbol>() =
         *style->getOrCreate<osgEarth::Symbology::PolygonSymbol>();
   }

   osgEarth::Symbology::AltitudeSymbol *alt_sym =
      feature->style()->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();
   alt_sym->clamping() = current_alt_sym->clamping();

   // set extrusion
   if (extruded)
   {
      // see http://forum.osgearth.org/Extruded-Polygons-td7583529.html
      osgEarth::Symbology::ExtrusionSymbol *extrusion_sym =
         feature->style()->getOrCreate<osgEarth::Symbology::ExtrusionSymbol>();
      extrusion_sym->wallStyleName() = "foo";
   }

   return new osgEarth::Annotation::FeatureNode(m_map_node, feature);
}

long OverlayElements::AddNode(osg::Node* node)
{
   // add a feature node to the parent node
   m_parent_node->addChild(node);

   // save and return the feature handle
   return GetHandleForNode(node);
}

long OverlayElements::AddFromGeometry(osgEarth::Symbology::Geometry* geometry,
   bool extruded, osgEarth::Symbology::Style* style)
{
   osgEarth::Annotation::FeatureNode* feature_node =
      CreateFromGeometry(geometry, extruded, style);
   PlaybackTimeChangedNodeVisitor::SetNodeTimeIntervalAndMask(
      m_begin_time, m_end_time, feature_node);
   return AddNode(feature_node);
}

long OverlayElements::AddFromGeometry(osgEarth::Symbology::Geometry* geometry,
   bool extruded /*= false*/)
{
   return AddFromGeometry(geometry, extruded, m_current_style);
}

long OverlayElements::ModifyPosition(long object_handle, double lat, double lon,
      double alt)
{
   using namespace osgEarth::Annotation;

   osg::Node* node = m_display_element_root->FindNode(object_handle);
   if (node == nullptr)
   {
      CString msg;
      msg.Format("Invalid object handle: %d", object_handle);
      ERR_report(msg);
      return FAILURE;
   }

   PositionedAnnotationNode* positioned_annotation_node =
      dynamic_cast<PositionedAnnotationNode *>(node);
   if (node == nullptr)
   {
      ERR_report("ModifyPosition called on Invalid object type.");
      return FAILURE;
   }

   GenericNodeCallback* callback = new GenericNodeCallback;
   osgEarth::AltitudeMode altMode = AltitudeModeFromStyle( m_current_style );
   const SpatialReference* pSRS = m_map_node->getMapSRS();
   callback->SetOperation(m_map_node, [=]()
   {
      positioned_annotation_node->setPosition(osgEarth::GeoPoint(
         pSRS, lon, lat, alt, altMode ));
   });

   return SUCCESS;
}

long OverlayElements::DeleteObject(long object_handle)
{
   long result = FAILURE;
   osg::Node* node = m_display_element_root->FindAndRemoveNode(object_handle);
   if (node != nullptr)
   {
      // Delete the node from its parent
      GenericNodeCallback* callback = new GenericNodeCallback;
      callback->SetOperation(m_map_node, [node](){
         osg::Node* parent_node = node->getParent(0);
         if (parent_node)
         {
            osg::Group* parent_group_node = parent_node->asGroup();
            if (parent_group_node)
               parent_group_node->removeChild(node);
         }
      });
      result = SUCCESS;
   }
   else
   {
      auto screen_overlay =
         m_display_element_root->FindAndRemoveControl(object_handle);
      if (screen_overlay != nullptr)
      {
         result = SUCCESS;
      }
   }

   if (result != SUCCESS)
   {
      CString msg;
      msg.Format("Invalid object handle: %d", object_handle);
      ERR_report(msg);
      return FAILURE;
   }

   return result;
}

long OverlayElements::DeleteAllObjects()
{
   return m_display_element_root->DeleteAllObjects();
}

void OverlayElements::StartMonitoringForRegion(DisplayElementRootNode* dern,
      FalconViewOverlayLib::IRegionListener* region_listener,
      double northern_bound, double southern_bound, double eastern_bound,
      double western_bound, double min_lod_pixels, double max_lod_pixels,
      double significant_size_meters)
{
   const osgEarth::SpatialReference* srs =
      m_map_node->getMapSRS()->getGeographicSRS();

   osgEarth::GeoExtent extent(srs, western_bound, southern_bound,
      eastern_bound, northern_bound);

   // find the ECEF LOD center point
   double x, y;
   extent.getCentroid( x, y );
   osg::Vec3d lod_center;
   extent.getSRS()->transform(osg::Vec3d(x,y,0), srs->getECEF(), lod_center);

   // compute the tile's radius
   double d = 0.5 * osgEarth::GeoMath::distance(
      osg::DegreesToRadians(extent.yMin()),
      osg::DegreesToRadians(extent.xMin()),
      osg::DegreesToRadians(extent.yMax()),
      osg::DegreesToRadians(extent.xMax()));

   if (min_lod_pixels < 0.0)
      min_lod_pixels = 0.0;
   if (max_lod_pixels < 0.0)
      max_lod_pixels = FLT_MAX;

   RegionLOD* plod = nullptr;
   if (significant_size_meters < 0.0)
   {
      plod = new RegionLOD(m_map_node);
   }
   // Significant size specified. This value will be used when computing
   // required ranges.
   else
   {
      osg::BoundingSphere bsphere(lod_center, significant_size_meters);
      plod = new RegionLODSigSize(m_map_node, bsphere);
   }

   plod->setRangeMode( osg::LOD::PIXEL_SIZE_ON_SCREEN );
   plod->setRange(0, min_lod_pixels, max_lod_pixels);
   plod->setRange(1, max_lod_pixels, FLT_MAX);
   plod->setCenter(lod_center);
   plod->setRadius(d);

   ActiveRegionNode* active_region_node = new ActiveRegionNode(dern,
      region_listener);
   plod->addChild(active_region_node);

   ExclusiveSubregionsNode* subregions_node = new ExclusiveSubregionsNode(
      dern, region_listener);
   plod->addChild(subregions_node);

   m_parent_node->addChild(plod);
}


void OverlayElements::SetTimeInterval(DATE begin, DATE end)
{
   m_begin_time = begin;
   m_end_time = end;
}

// osg::Image utilities implementation
//
osg::Image* osg_image_utils::GdiplusBitmapToImage(Gdiplus::Bitmap* bitmap)
{
   Gdiplus::Status status = bitmap->RotateFlip(Gdiplus::RotateNoneFlipY);

   Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());

   osg::Image* image = new osg::Image;
   image->allocateImage(rect.Width, rect.Height, 1, GL_RGBA,
      GL_UNSIGNED_BYTE);

   Gdiplus::BitmapData bitmap_data;
   bitmap_data.Width = rect.Width;
   bitmap_data.Height = rect.Height;
   bitmap_data.Stride = rect.Width * 4;
   bitmap_data.PixelFormat = PixelFormat32bppARGB;
   bitmap_data.Scan0 = image->data();

   bitmap->LockBits(&rect,
      Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeUserInputBuf,
      PixelFormat32bppARGB, &bitmap_data);

   // Swap R and B components
   size_t size = rect.Width * rect.Height * 4;
   unsigned char* p = reinterpret_cast<unsigned char*>(bitmap_data.Scan0);
   for (size_t i=0; i<size; i+=4)
   {
      std::swap(*p, *(p + 2));
      p += 4;
   }

   bitmap->UnlockBits(&bitmap_data);

   return image;
}

osg::Image* osg_image_utils::ImageFromFilename(BSTR filename)
{
   // Construct an image associated with the filename
   Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(filename);
   if (bitmap == nullptr || bitmap->GetLastStatus() != Gdiplus::Ok)
   {
      CString msg;
      msg.Format("ImageFromFilename failed: Unable to load %s",
         (char *)_bstr_t(filename));
      ERR_report(msg);
      delete bitmap;
      return nullptr;
   }

   osg::Image* ret = GdiplusBitmapToImage(bitmap);
   delete bitmap;
   
   ret->setFileName( (LPCSTR) _bstr_t( filename ) );
   return ret;
}

osg::Image* osg_image_utils::ImageFromStream(IStream* stream)
{
   // Construct an image associated with the filename
   Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromStream(stream);
   do
   {
      CStringA msg;
      if (bitmap != nullptr)
      {
         Gdiplus::Status rslt( bitmap->GetLastStatus() );
         if ( rslt == Gdiplus::Ok )
            break;
      
         msg.Format( "Gdiplus::Bitmap::FromStream() returned error %d", rslt );
      }
      else
         msg.Format( "Gdiplus::Bitmap::FromStream failed" );

      ERR_report( msg );
      return nullptr;

   } while ( FALSE );

   osg::Image* ret = GdiplusBitmapToImage(bitmap);
   delete bitmap;
   return ret;

}  // ImageFromStream()

// End of OverlayElements.cpp

