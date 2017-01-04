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

#ifndef FALCONVIEW_DISPLAYELEMENTSIMPL_H_
#define FALCONVIEW_DISPLAYELEMENTSIMPL_H_

class DisplayElementRootNode;
namespace osg
{
   class Group;
}
class OverlayElements;

// Implements IDisplayElements defined in FalconViewOverlay.tlb
//
class DisplayElementsImpl :
   public CComObjectRootEx<CComMultiThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IDisplayElements,
      &FalconViewOverlayLib::IID_IDisplayElements,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>,
   public IDispatchImpl<FalconViewOverlayLib::IIntervisiblityDisplayElements,
      &FalconViewOverlayLib::IID_IIntervisiblityDisplayElements,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
public:
   DisplayElementsImpl();
   ~DisplayElementsImpl();

   void Initialize(DisplayElementRootNode* dern, osg::Group* parent_node,
      bool generate_object_handles = true);

   osg::Group* GetParentModelNode() const;

BEGIN_COM_MAP(DisplayElementsImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IDisplayElements)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IDisplayElements)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IIntervisiblityDisplayElements)
END_COM_MAP()

   // IDisplayElements

   STDMETHOD(raw_CreateImageFromFilename)( __in BSTR filename, __out long* image_handle);

   STDMETHOD(raw_CreateImageFromRawBytes)( __in BYTE* raw_bytes, __in long num_bytes,
      __out long* image_handle);

   STDMETHOD(raw_CreateImageFromVariant)( __in VARIANT varData,   // VT_UI1 or (IDispatch*) IStream
      __out long* image_handle);

   STDMETHOD(raw_GetImageWidth)( __in long image_handle, __out long *width);

   STDMETHOD(raw_GetImageHeight)( __in long image_handle, __out long *height);

   STDMETHOD(raw_CreateModelFromFilename)(BSTR filename,
      double distance_to_load_meters, double distance_to_load_texture_meters,
      BSTR default_texture_name, double bounding_sphere_radius_meters,
      long* model_handle);

   STDMETHOD(raw_SetAltitudeMode)(FalconViewOverlayLib::AltitudeMode mode);

   STDMETHOD(raw_SetPen)(float fg_red, float fg_green, float fg_blue,
      float bg_red, float bg_green, float bg_blue, float opacity,
      long turn_off_background, float line_width, long line_style);

   STDMETHOD(raw_SetBrush)(float red, float green, float blue, float opacity);

   STDMETHOD(raw_SetIconAlignmentMode)(
      FalconViewOverlayLib::IconAlignmentMode mode);

   STDMETHOD(raw_SetIconDeclutterMode)( long declutter );
   STDMETHOD(raw_SetFont)(BSTR font_name, float size,
      COLORREF fg_color, COLORREF bg_color);

   STDMETHOD(raw_SetTimeInterval)(
      FalconViewOverlayLib::TimeInterval time_interval);

   STDMETHOD(raw_AddEllipse)(double lat, double lon, double vertical_meters,
      double alt_meters, double horizontal_meters, double rotation_degrees,
      long* object_handle);

   STDMETHOD(raw_AddImage)(long image_handle, double lat, double lon,
      double alt, double scale, double heading_degrees, BSTR label_text,
      long* object_handle);

   STDMETHOD(raw_AddText)(double lat, double lon, double alt_meters,
      BSTR text, long x_offset, long y_offset, long* object_handle);

   STDMETHOD(raw_AddLine)(SAFEARRAY *geo_points, long *object_handle);

   STDMETHOD(raw_AddPolygon)(SAFEARRAY *geo_points,
      SAFEARRAY* point_counts, long extruded, long *object_handle);

   STDMETHOD(raw_AddGroundOverlay)(long image_handle, double north,
      double west, double south, double east, long* object_handle);

   STDMETHOD(raw_AddModel)(long model_handle,
      FalconViewOverlayLib::ModelInstance model_instance, long* object_handle);

   STDMETHOD(raw_AddModels)(BSTR filename, SAFEARRAY* model_instances,
      long* result);

   STDMETHOD(raw_AddScreenOverlay)(long image_handle, double image_anchor_x,
      double image_anchor_y, double screen_anchor_x,
      FalconViewOverlayLib::ScreenAnchorUnits screen_anchor_x_units,
      double screen_anchor_y,
      FalconViewOverlayLib::ScreenAnchorUnits screen_anchor_y_units,
      double rotation_center_x, double rotation_center_y,
      double rotation_degrees, double x_scale, double y_scale,
      long* object_handle);

   STDMETHOD(raw_AddCameraPositionIndicatorControl)(
      double target_lat, double target_lon, double target_alt_meters,
      double screen_anchor_x,
      FalconViewOverlayLib::ScreenAnchorUnits screen_anchor_x_units,
      double screen_anchor_y,
      FalconViewOverlayLib::ScreenAnchorUnits screen_anchor_y_units,
      COLORREF color, long* object_handle);

   STDMETHOD(raw_ModifyPosition)(long object_handle, double lat, double lon,
      double alt);

   STDMETHOD(raw_DeleteObject)(long object_handle);
   STDMETHOD(raw_DeleteAllObjects)();

   STDMETHOD(raw_StartMonitoringForRegion)(
      FalconViewOverlayLib::IRegionListener* region_listener,
      double northern_bound, double southern_bound, double eastern_bound,
      double western_bound, double min_lod_pixels, double max_lod_pixels,
      double significant_size_meters);

   // IIntervisiblityDisplayElements
   STDMETHOD(raw_CreateMask)(double lat, double lon, double range_meters,
      double observer_height_meters, long* intervisibility_handle);

   STDMETHOD(raw_UpdateMask)(long intervisibility_handle, double transparency,
      double flight_altitude_meters, long invert_mask, COLORREF mask_color);

   STDMETHOD(raw_DeleteMask)(long intervisibility_handle);

private:
   DisplayElementRootNode* m_display_element_root;
   OverlayElements* m_osg_display_elements;
};

#endif  // FALCONVIEW_DISPLAYELEMENTSIMPL_H_
