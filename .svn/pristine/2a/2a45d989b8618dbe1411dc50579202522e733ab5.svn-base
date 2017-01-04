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
#include "DisplayElementsImpl.h"

#include "FalconView/DisplayElementRootNode.h"
#include "FalconView/GeospatialScene.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/getobjpr.h"
#include "FalconView/include/OverlayElements.h"
#include "FalconView/include/utils.h"  // for UTL_get_active_non_printing_view
#include "FalconView/MapView.h"

//viewshed
#include "FalconView/RenderFunctions.h"
#include "FalconView/ViewshedNode.h"

#ifdef _DEBUG
long g_cIDisplayElements = 0;
#endif

DisplayElementsImpl::DisplayElementsImpl() :
m_osg_display_elements(nullptr),
m_display_element_root(nullptr)
{
}

DisplayElementsImpl::~DisplayElementsImpl()
{
   delete m_osg_display_elements;
}

void DisplayElementsImpl::Initialize(DisplayElementRootNode* dern,
   osg::Group* parent_node, bool generate_object_handles)
{
   MapView* map_view = static_cast<MapView *>(
      UTL_get_active_non_printing_view());
   if (map_view)
   {
      m_osg_display_elements = map_view->CreateOverlayElements();
      m_osg_display_elements->SetParentNode(dern, parent_node,
         generate_object_handles);
      m_display_element_root = dern;
   }
}

osg::Group* DisplayElementsImpl::GetParentModelNode() const
{
   return m_osg_display_elements->GetParentModelNode();
}

// Resource creation
//

STDMETHODIMP DisplayElementsImpl::raw_CreateImageFromFilename( __in BSTR filename,
   __out long* image_handle)
{
   if (m_osg_display_elements)
   {
      *image_handle = m_osg_display_elements->CreateImageFromFilename(
         filename);
      return S_OK;
   }

   *image_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_CreateImageFromRawBytes(
   __in BYTE* raw_bytes, __in long num_bytes, __in long* image_handle )
{
   if (m_osg_display_elements)
   {
      *image_handle = m_osg_display_elements->CreateImageFromRawBytes(
         raw_bytes, num_bytes);
      return S_OK;
   }

   *image_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_CreateImageFromVariant(
   __in VARIANT varData, __out long* image_handle )
{
   if (m_osg_display_elements)
   {
      switch ( varData.vt )
      {
         case VT_UNKNOWN:
         {
            if ( 0 != ( *image_handle =
                  m_osg_display_elements->CreateImageFromRawBytes( (IStream*) IStreamPtr( varData.punkVal ) ) ) )
               return S_OK;
            break;
         }

         case VT_UI1 | VT_ARRAY:
         case VT_I1 | VT_ARRAY:
         {
            SAFEARRAY* psa = varData.parray;
            if ( psa->cDims == 1
               && psa->rgsabound[ 0 ].lLbound == 0 )
            {
               if ( 0 != ( *image_handle = m_osg_display_elements->CreateImageFromRawBytes(
                  (PBYTE) psa->pvData, psa->rgsabound[ 0 ].cElements ) ) )
                  return S_OK;
            }
            break;
         }

         default:
            break;
      }
   }

   *image_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_GetImageWidth(
   __in long image_handle, __out long *width)
{
   if (m_osg_display_elements)
   {
      *width = m_osg_display_elements->GetImageWidth(image_handle);
      return S_OK;
   }

   *width = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_GetImageHeight(
   __in long image_handle, __out long *height)
{
   if (m_osg_display_elements)
   {
      *height = m_osg_display_elements->GetImageHeight(image_handle);
      return S_OK;
   }

   *height = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_CreateModelFromFilename(BSTR filename,
   double distance_to_load_meters, double distance_to_load_texture_meters,
   BSTR default_texture_name, double bounding_sphere_radius_meters,
   long* model_handle)
{
   if (m_osg_display_elements)
   {
      *model_handle = m_osg_display_elements->CreateModelFromFilename(
         filename, distance_to_load_meters, distance_to_load_texture_meters,
         default_texture_name, bounding_sphere_radius_meters);
      return S_OK;
   }

   *model_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_SetAltitudeMode(
   FalconViewOverlayLib::AltitudeMode mode)
{
   if (m_osg_display_elements)
   {
      m_osg_display_elements->SetAltitudeMode(mode);
      return S_OK;
   }

   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_SetPen(float fg_red, float fg_green,
   float fg_blue, float bg_red, float bg_green, float bg_blue, float opacity,
   long turn_off_background, float line_width, long line_style)
{
   if (m_osg_display_elements)
   {
      m_osg_display_elements->SetPen(fg_red, fg_green, fg_blue, bg_red,
         bg_green, bg_blue, opacity, turn_off_background, line_width,
         line_style);
   }
   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_SetBrush(float red, float green,
   float blue, float opacity)
{
   if (m_osg_display_elements)
      m_osg_display_elements->SetBrush(red, green, blue, opacity);
   return S_OK;
}


STDMETHODIMP DisplayElementsImpl::raw_SetIconAlignmentMode(
   FalconViewOverlayLib::IconAlignmentMode mode)
{
   if (m_osg_display_elements)
   {
      m_osg_display_elements->SetIconAlignmentMode(mode);
      return S_OK;
   }

   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_SetIconDeclutterMode(
   long declutter )
{
   if (m_osg_display_elements)
   {
      m_osg_display_elements->SetIconDeclutterMode( declutter != 0 );
      return S_OK;
   }

   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_SetFont(BSTR font_name, float size,
   COLORREF fg_color, COLORREF bg_color)
{
   if (m_osg_display_elements)
   {
      m_osg_display_elements->SetFont((char *)_bstr_t(font_name), size,
         fg_color, bg_color);
      return S_OK;
   }

   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_SetTimeInterval(
   FalconViewOverlayLib::TimeInterval time_interval)
{
   if (m_osg_display_elements)
   {
      m_osg_display_elements->SetTimeInterval(
         time_interval.bTimeIntervalValid ? time_interval.dtBegin : 0.0,
         time_interval.bTimeIntervalValid ? time_interval.dtEnd : 0.0);
      return S_OK;
   }

   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_AddEllipse(double lat, double lon,
   double alt_meters, double vertical_meters, double horizontal_meters,
   double rotation_degrees, long* object_handle)
{
   if (m_osg_display_elements)
   {
      *object_handle = m_osg_display_elements->AddEllipse(lat, lon,
         alt_meters, vertical_meters, horizontal_meters, rotation_degrees);

      return S_OK;
   }

   *object_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_AddImage(long image_handle, double lat,
   double lon, double alt, double scale, double heading_degrees, BSTR text,
   long* object_handle)
{
   std::string label_text = (char *)_bstr_t(text);

   if (m_osg_display_elements)
   {
      std::vector< std::pair<double, double> > lat_lons;
      lat_lons.push_back(std::make_pair(lat, lon));

      std::vector<double> altitude_meters;
      altitude_meters.push_back(alt);

      std::vector<std::string> label_text_arr;
      label_text_arr.push_back(label_text);

      *object_handle = m_osg_display_elements->AddImage(image_handle,
         lat_lons, altitude_meters, label_text_arr, scale, heading_degrees);

      return S_OK;
   }

   *object_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_AddText(double lat, double lon,
   double alt_meters, BSTR text, long x_offset, long y_offset,
   long* object_handle)
{
   if (m_osg_display_elements)
   {
      std::string str = (char *)_bstr_t(text);

      *object_handle = m_osg_display_elements->AddText(lat, lon,
         alt_meters, str, x_offset, y_offset);

      return S_OK;
   }

   *object_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_AddLine(
   SAFEARRAY *geo_points, long *object_handle)
{
   if (m_osg_display_elements)
   {
      FalconViewOverlayLib::GeoPoint3D *points
         = reinterpret_cast<FalconViewOverlayLib::GeoPoint3D *>(
         geo_points->pvData);
      long num_points = geo_points->rgsabound->cElements;

      std::vector< std::tuple<double, double, double> > coords;

      for (long i = 0; i < num_points; ++i)
      {
         auto t = std::make_tuple(
            points[i].lat, points[i].lon, points[i].alt_meters);
         coords.push_back(t);
      }

      *object_handle = m_osg_display_elements->AddLine(coords);

      return S_OK;
   }

   *object_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_AddPolygon(SAFEARRAY *geo_points,
   SAFEARRAY* point_counts, long extruded, long *object_handle)
{
   if (m_osg_display_elements)
   {
      FalconViewOverlayLib::GeoPoint3D *points
         = reinterpret_cast<FalconViewOverlayLib::GeoPoint3D *>(
         geo_points->pvData);
      long num_points = geo_points->rgsabound->cElements;

      std::vector< std::tuple<double, double, double> > coords;

      for (long i = 0; i < num_points; ++i)
      {
         auto t = std::make_tuple(
            points[i].lat, points[i].lon, points[i].alt_meters);
         coords.push_back(t);
      }

      long *counts = reinterpret_cast<long*>(point_counts->pvData);
      long num_counts = point_counts->rgsabound->cElements;

      std::vector<long> vec_counts;

      for (long i = 0; i < num_counts; ++i)
         vec_counts.push_back(counts[i]);

      *object_handle = m_osg_display_elements->AddPolygon(
         coords, vec_counts, extruded != 0);

      return S_OK;
   }

   *object_handle = -1;
   return E_FAIL;
}

STDMETHODIMP DisplayElementsImpl::raw_AddGroundOverlay(long image_handle,
   double north, double west, double south, double east, long* object_handle)
{
   *object_handle = m_osg_display_elements == nullptr ? -1
      : m_osg_display_elements->AddGroundOverlay(image_handle, north, west,
      south, east);
   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_AddModel(long model_handle,
   FalconViewOverlayLib::ModelInstance model_instance, long* object_handle)
{
   *object_handle = m_osg_display_elements == nullptr ? -1
      : m_osg_display_elements->AddModel(model_handle, model_instance);
   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_AddModels(BSTR filename,
   SAFEARRAY* model_instances, long* result)
{
   *result = m_osg_display_elements->AddModels(filename, model_instances);
   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_AddScreenOverlay(long image_handle,
   double image_anchor_x, double image_anchor_y, double screen_anchor_x,
   FalconViewOverlayLib::ScreenAnchorUnits screen_anchor_x_units,
   double screen_anchor_y,
   FalconViewOverlayLib::ScreenAnchorUnits screen_anchor_y_units,
   double rotation_center_x, double rotation_center_y, double rotation_degrees,
   double x_scale, double y_scale, long* object_handle)
{
   // cast to ScreenAnchorUnits okay unless enums change
   *object_handle = m_osg_display_elements == nullptr ? -1
      : m_osg_display_elements->AddScreenOverlay(image_handle,
      image_anchor_x, image_anchor_y,
      screen_anchor_x, (ScreenAnchorUnits)screen_anchor_x_units,
      screen_anchor_y, (ScreenAnchorUnits)screen_anchor_y_units,
      rotation_center_x, rotation_center_y, rotation_degrees,
      x_scale, y_scale);

   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_AddCameraPositionIndicatorControl(
   double target_lat, double target_lon, double target_alt_meters,
   double screen_anchor_x,
   FalconViewOverlayLib::ScreenAnchorUnits screen_anchor_x_units,
   double screen_anchor_y,
   FalconViewOverlayLib::ScreenAnchorUnits screen_anchor_y_units,
   COLORREF color, long* object_handle)
{
   *object_handle = m_osg_display_elements == nullptr ? -1
      : m_osg_display_elements->AddCameraPositionIndicatorControl(
      target_lat, target_lon, target_alt_meters,
      screen_anchor_x, (ScreenAnchorUnits)screen_anchor_x_units,
      screen_anchor_y, (ScreenAnchorUnits)screen_anchor_y_units,
      color);

   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_ModifyPosition(long object_handle,
   double lat, double lon, double alt)
{
   if (m_osg_display_elements)
      m_osg_display_elements->ModifyPosition(object_handle, lat, lon, alt);

   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_DeleteObject(long object_handle)
{
   if (m_osg_display_elements)
      m_osg_display_elements->DeleteObject(object_handle);

   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_DeleteAllObjects()
{
   if (m_osg_display_elements)
      m_osg_display_elements->DeleteAllObjects();

   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_StartMonitoringForRegion(
   FalconViewOverlayLib::IRegionListener* region_listener,
   double northern_bound, double southern_bound, double eastern_bound,
   double western_bound, double min_lod_pixels, double max_lod_pixels,
   double significant_size_meters)
{
   if (m_osg_display_elements)
   {
      m_osg_display_elements->StartMonitoringForRegion(m_display_element_root,
         region_listener,
         northern_bound, southern_bound, eastern_bound, western_bound,
         min_lod_pixels, max_lod_pixels, significant_size_meters);
   }

   return S_OK;
}

namespace
{
   // Helper method to access the GeospatialScene. Can return nullptr
   GeospatialScene* GetGeospatialScene()
   {
      MapView* map_view = fvw_get_view();
      if (map_view)
      {
         GeospatialViewController* gvc = map_view->GetGeospatialViewController();
         if (gvc)
            return gvc->GetGeospatialScene();
      }

      return nullptr;
   }
}

STDMETHODIMP DisplayElementsImpl::raw_CreateMask(double lat, double lon,
   double range_meters, double observer_height_meters,
   long* intervisibility_handle)
{
   GeospatialScene* scene = GetGeospatialScene();
   if (scene)
   {
      scene->m_controller->GetInputDevice()->action_center.lat = lat;
      scene->m_controller->GetInputDevice()->action_center.lon = lon;
      scene->GetUpdateCameraAndTexGenCallback()->m_far_plane = min(range_meters, 2400.0);
      scene->GetUpdateCameraAndTexGenCallback()->m_height_offset = observer_height_meters;
      // Add a new intervisibility mask and pass back handle to the user
      *intervisibility_handle = 0;
   }

   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_UpdateMask(long intervisibility_handle,
   double transparency, double flight_altitude_meters, long invert_mask,
   COLORREF mask_color)
{
   GeospatialScene* scene = GetGeospatialScene();
   if (scene)
   {
      // update parameters of the intervisibility mask with the given handle
      scene->GetUpdateCameraAndTexGenCallback()->_viewshed_setup = true;
      scene->GetUpdateCameraAndTexGenCallback()->finish_render = true;
      scene->m_viewshed_node->SetViewshedUpdateRequired();
      scene->GetUpdateCameraAndTexGenCallback()->UpdateViewshed();
   }
   return S_OK;
}

STDMETHODIMP DisplayElementsImpl::raw_DeleteMask(long intervisibility_handle)
{
   GeospatialScene* scene = GetGeospatialScene();
   if (scene)
   {
      // delete the intervisibility mask
   }

   return S_OK;
}