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

#ifndef FALCONVIEW_INCLUDE_OVERLAY_ELEMENTS_H
#define FALCONVIEW_INCLUDE_OVERLAY_ELEMENTS_H

// std header included here because of use of header in multiple projects
#include <tuple>

namespace osg
{
   class Group;
   class Image;
   class Node;
   template<class T> class ref_ptr;
}

namespace osgEarth
{
   class MapNode;

   namespace Annotation
   {
      class FeatureNode;
   }

   namespace Symbology
   {
      class AltitudeSymbol;
      class Geometry;
      class Style;
   }

   namespace Util
   {
      namespace Controls
      {
         class ControlCanvas;
         class Control;
         class ImageControl;
      }
   }
}

class DelayedMatrixTransformAdder;
class DisplayElementRootNode;
class DtedElevationLookup;
class GeospatialViewController;
struct ImageCache;
class ScreenOverlaysCollection;

enum ScreenAnchorUnits
{
   SCREEN_ANCHOR_UNITS_PIXELS = 0, // from lower left
   SCREEN_ANCHOR_UNITS_INSET_PIXELS = 1, // from upper right
   SCREEN_ANCHOR_UNITS_FRACTION = 2 // from lower left (1.0 is far right)
};

// OverlayElements - manages a collection of features for a particular overlay
class OverlayElements
{
public:
   ~OverlayElements();

   void SetParentNode(DisplayElementRootNode* display_element_root,
      osg::Group* parent_node, bool generate_object_handles = true);
   void ResetParentNode();

   // Resource creation
   //
   long CreateImageFromFilename(BSTR filename);
   long CreateImageFromRawBytes(BYTE* raw_bytes, long num_bytes);
   long CreateImageFromRawBytes( IStream* image_stream );
   long GetImageWidth(long image_handle);
   long GetImageHeight(long image_handle);
   long CreateModelFromFilename(BSTR filename, double distance_to_load_meters,
      double distance_to_load_texture_meters, BSTR default_texture_name,
      double bounding_sphere_radius_meters);

   // State
   //

   long SetFont(const std::string& font_name, float font_size,
      COLORREF fg_color, COLORREF bg_color);

   // set the current pen
   long SetPen(float fg_red, float fg_green, float fg_blue, float bg_red,
      float bg_green, float bg_blue, float opacity, long turn_off_background,
      float line_width, long line_style);

   // set the current brush
   long SetBrush(float red, float green, float blue, float opacity);

   long SetAltitudeMode(FalconViewOverlayLib::AltitudeMode mode);

   long SetIconAlignmentMode(FalconViewOverlayLib::IconAlignmentMode mode);
   long SetIconDeclutterMode( BOOL bDeclutter );

   void SetTimeInterval(DATE begin, DATE end);

   // Overlay Elements

   long AddArc(double lat, double lon, double radius, double heading_to_start,
      double turn_arc, BOOL clockwise);

   long AddGroundOverlay(long image_handle, double north, double west,
      double south, double east);

   long AddModel(long model_handle,
      const FalconViewOverlayLib::ModelInstance& model_instance);

   long AddModels(BSTR filename, SAFEARRAY* model_instances);

   long AddBitmap(const std::string& filename, double lat, double lon);

   long AddEllipse(double lat, double lon, double alt_meters,
      double vertical_meters, double horizontal_meters, double rotation_deg);

   long AddLine(
      const std::vector< std::tuple<double, double, double> >& coords);

   long AddImage(long image_handle,
      const std::vector< std::pair<double, double> >& lat_lons,
      const std::vector<double>& altitudes_meters,
      const std::vector<std::string>& icon_text,
      double scale, double heading_degrees);

   long AddPolygon(
      const std::vector< std::tuple<double, double, double> > &coords,
      const std::vector<long> &point_counts, bool extruded);

   // AddScreenOverlay will add the given image stream to the center of
   // the surface. This will be extended to allow additional control of the
   // position and rotation of the screen overlay.

   long AddScreenOverlay(IStream* image_stream);

   long AddScreenOverlay(long image_handle,
      double image_anchor_x, double image_anchor_y,
      double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
      double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
      double rotation_center_x, double rotation_center_y,
      double rotation_degrees, double x_scale, double y_scale);

   long AddScreenOverlay(osg::ref_ptr<osg::Image> image,
      double image_anchor_x, double image_anchor_y,
      double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
      double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
      double rotation_center_x, double rotation_center_y,
      double rotation_degrees, double x_scale, double y_scale);

   static void InitScreenOverlayImageControl(
      osg::ref_ptr<osgEarth::Util::Controls::ImageControl> image_control,
      double image_anchor_x, double image_anchor_y,
      double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
      double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
      double rotation_center_x, double rotation_center_y,
      double rotation_degrees, double x_scale, double y_scale);

   static void InitControlScreenAnchors(
      osgEarth::Util::Controls::Control* control,
      double screen_anchor_x, ScreenAnchorUnits screen_anchor_x_units,
      double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units);

   long ShowScreenOverlay(long screen_overlay_handle, bool show_not_hide);

   long AddCameraPositionIndicatorControl(double lat, double lon,
      double alt_meters, double screen_anchor_x,
      ScreenAnchorUnits screen_anchor_x_units,
      double screen_anchor_y, ScreenAnchorUnits screen_anchor_y_units,
      COLORREF color);

   long AddText(double lat, double lon, double alt_meters,
      const std::string& text, long x_offset, long y_offset);

   long ModifyPosition(long object_handle, double lat, double lon,
      double alt);

   long DeleteObject(long object_handle);
   long DeleteAllObjects();

   void StartMonitoringForRegion(DisplayElementRootNode* dern,
      FalconViewOverlayLib::IRegionListener* region_listener,
      double northern_bound, double southern_bound, double eastern_bound,
      double western_bound, double min_lod_pixels, double max_lod_pixels,
      double significant_size_meters);

   osg::Group* GetParentModelNode() const
   {
      return m_parent_model_node;
   }

private:
   OverlayElements(){}  // Not accessible, no default

   // Only allow the GeospatialViewController to create an instance since it
   // knows how to aquire the map node
   friend GeospatialViewController;
   OverlayElements(osgEarth::MapNode* map_node,
      osgEarth::Util::Controls::ControlCanvas* canvas);

   long AddFromGeometry(osgEarth::Symbology::Geometry* geometry,
      bool extruded = false);
   long AddFromGeometry(osgEarth::Symbology::Geometry* geometry,
      bool extruded, osgEarth::Symbology::Style* style);
   long AddNode(osg::Node* node);
   osgEarth::Annotation::FeatureNode* CreateFromGeometry(
      osgEarth::Symbology::Geometry* geometry, bool extruded,
      osgEarth::Symbology::Style* style);

   long SetAltitudeMode(osgEarth::Symbology::AltitudeSymbol* alt_sym,
      FalconViewOverlayLib::AltitudeMode mode);

   // The object containing the overlay, display provider, and handles for
   // a particular overlay
   DisplayElementRootNode* m_display_element_root;

   // MapNode -> osg::Group (m_parent) 0..-> FeatureNodes
   osgEarth::MapNode* m_map_node;
   osgEarth::Util::Controls::ControlCanvas* m_control_canvas;
   osg::Group* m_parent_node;

   // All models are attached to the following node
   osg::Group* m_parent_model_node;

   ImageCache* m_image_cache;

   // The collection of screen overlays that were added with AddScreenOverlay
   ScreenOverlaysCollection* m_screen_overlays;
   long m_next_screen_overlay_handle;

   // Current style (pen, font, and so forth)
   osgEarth::Symbology::Style* m_current_style;

   // background style for lines
   osgEarth::Symbology::Style* m_background_style;

   // Begin and end times are added to all elements if either the begin or end
   // time is not 0.0
   DATE m_begin_time, m_end_time;

   // Used for elevation lookups for certain altitude modes
   DtedElevationLookup* m_dted;

   // Use to update a collection of MatrixTransform -> ModelPagedLOD during the
   // update traversal when this OverlayElements instance is destroyed.
   DelayedMatrixTransformAdder* m_delayed_xform_adder;

   // Flag used to determine if objects should be generated or not.
   bool m_generate_object_handles;
   long GetHandleForNode(osg::Node* node);
};

// osg::Image utilities that are used by OverlayElements
namespace osg_image_utils
{
   osg::Image* GdiplusBitmapToImage(Gdiplus::Bitmap* bitmap);
   osg::Image* ImageFromFilename(BSTR filename);
   osg::Image* ImageFromStream(IStream* stream);
}


#endif  // FALCONVIEW_INCLUDE_OVERLAY_ELEMENTS_H