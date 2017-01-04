// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// DisplayElementRootNode.h

#ifndef FALCONVIEW_DISPLAYELEMENTROOTNODE_H_
#define FALCONVIEW_DISPLAYELEMENTROOTNODE_H_

#include "geo_tool_d.h"
#include "osg/Group"
#include "osg/PagedLOD"
#include "osg/ref_ptr"

class C_overlay;
class CameraPositionIndicatorControl;
namespace osg
{
   class Matrixd;
   class Node;
}

namespace osgEarth
{
   namespace Util
   {
      namespace Controls
      {
         class Control;
         class ControlCanvas;
      }
   }
}

interface DisplayElementProvider_Interface;

// DisplayElementRootNode represents the node in scene graph corresponding
// to a collection of display elements, usually an overlay.
//
class DisplayElementRootNode
{
public:
   DisplayElementRootNode(C_overlay* overlay);
   void Finalize();
   ~DisplayElementRootNode();

   // Accessors
   //
   C_overlay* GetAssociatedOverlay() const
   {
      return m_overlay;
   }
   void SetNode(osg::Group* node);
   osg::Group* GetNode() const
   {
      return m_node;
   }
   void SetProvider(DisplayElementProvider_Interface* provider)
   {
      m_provider = provider;
   }

   void SetControlCanvas(
      osgEarth::Util::Controls::ControlCanvas *control_canvas)
   {
      m_control_canvas = control_canvas;
   }
   bool CanAddWorkItems() const
   {
      // Work items can be added until the DisplayElementRootNode instance
      // is terminated
      return m_can_add_work_items;
   }

   // Methods
   //
   long QueueWorkItem(FalconViewOverlayLib::IWorkItem* work_item,
      osg::Group* parent_node);
   void WaitForWorkItem(long work_item_handle, long cancel_pending_work_item);

   void RegionActivated(FalconViewOverlayLib::IRegionListener* region_listener,
      osg::Group* parent_node);
   void InitializeSubregions(
      FalconViewOverlayLib::IRegionListener* region_listener,
      osg::Group* parent_node);

   void SetVisible(bool visible);

   // Node map manipulation
   //

   // Add a new node to the node map and return handle
   long AddNode(osg::Node* node);
   
   // Find a node given a handle. Returns nullptr if not found.
   osg::Node* FindNode(long handle);

   // Finds a node given the handle and removes it from the node map. Returns
   // nullptr if not found.
   osg::Node* FindAndRemoveNode(long handle);

   long DeleteAllObjects();

   // Models
   //
   long CreateModelFromFilename(BSTR filename, double distance_to_load_meters,
      double distance_to_load_texture_meters, BSTR default_texture_name,
      double bounding_sphere_radius_meters);
   osg::Node* DisplayElementRootNode::GetModel(long model_handle);

   // Adds an control to this root node
   long AddControl(osgEarth::Util::Controls::Control *control);
   osgEarth::Util::Controls::Control* FindAndRemoveControl(long handle);
   osgEarth::Util::Controls::Control* FindControl(long handle);

   static std::vector<CameraPositionIndicatorControl* >
      s_camera_position_indicators;

   static void UpdateCameraPositionIndicators(
      const d_geo_t& camera_location, const osg::Matrixd& view_matrix);

private:
   C_overlay* m_overlay;
   osg::ref_ptr<osg::Group> m_node;
   DisplayElementProvider_Interface* m_provider;
   osgEarth::Util::Controls::ControlCanvas *m_control_canvas;

   CRITICAL_SECTION m_handle_map_cs;  // used for m_controls, too
   std::map<long, osg::ref_ptr<osg::Node>> m_handle_to_node_map;
   std::map<long, osg::ref_ptr<osgEarth::Util::Controls::Control> > m_controls;
   long m_next_handle;

   typedef std::list<std::pair<std::wstring, long>> ModelCacheLRU;
   ModelCacheLRU m_model_cache_lru;

   std::map<long, std::pair<osg::ref_ptr<osg::Node>, ModelCacheLRU::iterator>>
      m_model_handle_to_node;
   std::map<std::wstring, long> m_model_filename_to_handle;
   long m_next_model_handle;

   // Flag indicating whether or not work items associated with this
   // instance can be queued
   bool m_can_add_work_items;

   // threadpool environment associated with the overlay
   TP_CALLBACK_ENVIRON m_tp_callback_environ;

   // the cleanup group associated with the callback environment
   PTP_CLEANUP_GROUP m_tp_cleanup_group;
};

class ModelPagedLOD : public osg::PagedLOD
{
public:
   ModelPagedLOD() : m_scale_factor(1.0f)
   {
   }

   virtual bool addChild(osg::Node *child) override;
   float getDistanceFromEyePoint(osg::NodeVisitor& nv) override;

   // To properly compute the distance to the model's center when the parent
   // node is a MatrixTransform a scale factor will need to be applied. The
   // MatrixTransform should sets this value in its traversal.
   void setScaleFactor(float scale_factor)
   {
      m_scale_factor = scale_factor;
   }

private:
   float m_scale_factor;
};

#endif  // FALCONVIEW_DISPLAYELEMENTROOTNODE_H_
