// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// MapEngineOSG.cpp

#include "stdafx.h"
#include "MapEngineOSG.h"

#include "osgEarth/MapNode"

#include "FalconView/CameraUtils.h"
#include "FalconView/GenericNodeCallback.h"
#include "FalconView/GeospatialView.h"
#include "FalconView/getobjpr.h"
#include "FalconView/MapEngineCOM.h"  // used by Scale in/out
#include "FalconView/MapParams.h"

#include "Common/ScopedCriticalSection.h"

MapEngineOSG::MapEngineOSG(GeospatialViewController* gvc) :
m_geospatial_view_controller(gvc),
m_map_engine_com(nullptr)
{
}

MapEngineOSG::~MapEngineOSG()
{
   delete m_map_engine_com;
}

void MapEngineOSG::EnableDraw(bool enable)
{
   m_geospatial_view_controller->EnableDraw(enable);
}

void MapEngineOSG::SetSurfaceDimensions(int width, int height)
{
}

int MapEngineOSG::ChangeCenter(const d_geo_t& center)
{
   Viewpoint3d viewpoint;
   m_geospatial_view_controller->GetViewpoint3D(&viewpoint);
   viewpoint.x = center.lon;
   viewpoint.y = center.lat;
   m_geospatial_view_controller->SetViewpoint3D(viewpoint);
   return SUCCESS;
}

int MapEngineOSG::ChangeCenterAndRotation(const d_geo_t& center,
   double rotation)
{
   Viewpoint3d viewpoint;
   m_geospatial_view_controller->GetViewpoint3D(&viewpoint);
   viewpoint.x = center.lon;
   viewpoint.y = center.lat;
   viewpoint.heading = -rotation;
   m_geospatial_view_controller->SetViewpoint3D(viewpoint);
   return SUCCESS;
}

int MapEngineOSG::ReinitializeFromMapParams(long group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params)
{
   // outdated, updates now handled in the actual manipulators
   UpdateViewpoint(map_type, map_proj_params);
   return SUCCESS;
}

int MapEngineOSG::ChangeMapType(long group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params,
   int override_no_mapdata /* = FALSE */)
{
   UpdateViewpoint(map_type, map_proj_params);
   return SUCCESS;
}

int MapEngineOSG::ChangeMapType(long group_identity,
   const MapType& map_type, int override_no_mapdata /* = FALSE */)
{
   int zoom_percent = GetCurrentZoomPercentOrDefault();
   UpdateViewpointZoom(map_type, zoom_percent);

   return SUCCESS;
}

int MapEngineOSG::ChangeMapType(long group_identity,
   const MapType& map_type, const d_geo_t& center,
   int override_no_mapdata /* = FALSE */)
{
   int zoom_percent = GetCurrentZoomPercentOrDefault();

   UpdateViewpointCenterAndZoom(map_type, center,
      GetCurrentZoomPercentOrDefault());

   return SUCCESS;
}

namespace
{
// Helper method to initialize a MapEngineCOM from the current MapView's
// parameters.
//
// This is used as a stopgap measure for determining the next scale in/out with
// available data.
void MapEngineFromMapParams(MapEngineCOM* & map_engine)
{
   MapView* map_view = fvw_get_view();
   if (map_view == nullptr)
      return;

   if (map_engine == nullptr)
   {
      map_engine = new MapEngineCOM;

      BOOL allow_invalid_points_on_surface = TRUE;
      map_engine->init(FALSE, FALSE, FALSE, FALSE, NULL,
         allow_invalid_points_on_surface);
   }

   const MapDisplayParams& map_disp_params = map_view->GetMapDisplayParams();
   map_engine->SetSurfaceDimensions(map_disp_params.surface_width,
      map_disp_params.surface_height);

   MapProjectionParams proj_params = map_view->GetMapProjParams();
   proj_params.type = ORTHOGRAPHIC_PROJECTION;

   map_engine->ReinitializeFromMapParams(map_view->GetMapGroupIdentity(),
      map_view->GetMapType(), proj_params, map_view->GetMapDisplayParams());

   map_engine->ApplyMap();
}
}

int MapEngineOSG::ChangeToClosestScale(long group_identity,
   MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);
   int ret = m_map_engine_com->ChangeToClosestScale(group_identity,
      new_map_type);

   if (ret == SUCCESS)
   {
      int zoom_percent = GetCurrentZoomPercentOrDefault();
      UpdateViewpointZoom(*new_map_type, zoom_percent);
      return SUCCESS;
   }

   return FAILURE;
}

int MapEngineOSG::ChangeToClosestScale(const MapCategory& category,
   MapType* new_map_type)
{
   return ChangeToClosestScale(category.GetGroupId(), new_map_type);
}

int MapEngineOSG::ChangeToClosestScale(const MapCategory& category,
   const d_geo_t& center, MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);
   int ret = m_map_engine_com->ChangeToClosestScale(category, center,
      new_map_type);

   if (ret == SUCCESS)
   {
      UpdateViewpointCenterAndZoom(*new_map_type, center,
         GetCurrentZoomPercentOrDefault());

      return SUCCESS;
   }

   return FAILURE;
}

MapProjectionParams CreateParamsForCom(MapProjectionParams map_proj_params)
{
   MapProjectionParams lcl_map_proj_params = map_proj_params;
   lcl_map_proj_params.type = ORTHOGRAPHIC_PROJECTION;

   // TODO I bet zoom should not be passed to the COM map engine.  Is this true?
   return lcl_map_proj_params;
}

int MapEngineOSG::ChangeToClosestScale(const MapCategory& category,
      const MapScale& scale, const MapProjectionParams& map_proj_params,
      MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);

   MapProjectionParams lcl_map_proj_params =
      CreateParamsForCom(map_proj_params);

   int ret = m_map_engine_com->ChangeToClosestScale(category, scale,
      lcl_map_proj_params, new_map_type);

   if (ret == SUCCESS)
   {
      UpdateViewpoint(*new_map_type, map_proj_params);
      return SUCCESS;
   }

   return FAILURE;
}

int MapEngineOSG::ChangeToClosestMapType(long group_identity,
   const MapType& current_map_type,
   const MapProjectionParams& map_proj_params,
   MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);

   MapProjectionParams lcl_map_proj_params =
      CreateParamsForCom(map_proj_params);

   int ret = m_map_engine_com->ChangeToClosestMapType(group_identity,
      current_map_type, lcl_map_proj_params, new_map_type);

   if (ret == SUCCESS)
   {
      UpdateViewpoint(*new_map_type, map_proj_params);
      return SUCCESS;
   }

   return FAILURE;
}

int MapEngineOSG::ChangeToSmallestScale(const MapCategory& category,
   const MapProjectionParams& map_proj_params, MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);

   MapProjectionParams lcl_map_proj_params =
      CreateParamsForCom(map_proj_params);

   int ret = m_map_engine_com->ChangeToSmallestScale(category,
      lcl_map_proj_params, new_map_type);

   if (ret == SUCCESS)
   {
      UpdateViewpoint(*new_map_type, map_proj_params);
      return SUCCESS;
   }

   return FAILURE;
}

int MapEngineOSG::ChangeToLargestScale(const MapCategory& category,
   const MapProjectionParams& map_proj_params, MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);

   MapProjectionParams lcl_map_proj_params =
      CreateParamsForCom(map_proj_params);

   int ret = m_map_engine_com->ChangeToLargestScale(category,
      lcl_map_proj_params, new_map_type);

   if (ret == SUCCESS)
   {
      UpdateViewpoint(*new_map_type, map_proj_params);
      return SUCCESS;
   }

   return FAILURE;
}

int MapEngineOSG::ChangeToBest(const d_geo_t& center,
   MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);

   int ret = m_map_engine_com->ChangeToBest(center, new_map_type);

   if (ret == SUCCESS)
   {
      auto map_node = m_geospatial_view_controller->GetGeospatialScene()->
         MapNode();

      GenericNodeCallback* node_callback = new GenericNodeCallback;
      MapType map_type = *new_map_type;
      node_callback->SetOperation(map_node,
         [this, center, map_type]()
      {
         m_geospatial_view_controller->GetGeospatialScene()->
            SetCurrentMapType(map_type);
         ChangeCenter(center);
      });

      return SUCCESS;
   }

   return FAILURE;
}

int MapEngineOSG::ChangeToBest(const MapSource& source, const MapScale& scale,
   const MapProjectionParams& map_proj_params,
   MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);

   MapProjectionParams lcl_map_proj_params =
      CreateParamsForCom(map_proj_params);

   int ret = m_map_engine_com->ChangeToBest(source, scale,
      lcl_map_proj_params, new_map_type);

   if (ret == SUCCESS)
   {
      UpdateViewpoint(*new_map_type, map_proj_params);
      return SUCCESS;
   }

   return FAILURE;
}

int MapEngineOSG::ChangeProjectionType(ProjectionEnum projection_type)
{
   // MapRenderingEngineOsg only supports a single projection. There
   // is nothing to do here.
   return SUCCESS;
}

int MapEngineOSG::ChangeRotation(double rotation)
{
   Viewpoint3d viewpoint;
   m_geospatial_view_controller->GetViewpoint3D(&viewpoint);
   viewpoint.heading = -rotation;
   m_geospatial_view_controller->SetViewpoint3D(viewpoint);
   return SUCCESS;
}

void MapEngineOSG::ChangeBrightness(double brightness)
{
   m_geospatial_view_controller->GetGeospatialScene()->SetBrightness(
      brightness);
}

void MapEngineOSG::ChangeContrast(double brightness, double contrast,
   int contrast_midval)
{
   m_geospatial_view_controller->GetGeospatialScene()->SetContrast(
     brightness, contrast, contrast_midval);
}

int MapEngineOSG::ChangeZoomPercent(int zoom_percent)
{
   // For now, this renderer will treat non-standard zoom percentages as 100%
   //
   if (zoom_percent == TO_SCALE_ZOOM_PERCENT ||
      zoom_percent == NATIVE_ZOOM_PERCENT)
   {
      zoom_percent = 100.0;
   }

   MapView* map_view = fvw_get_view();
   if (map_view)
      UpdateViewpointZoom(map_view->GetMapType(), zoom_percent);

   return SUCCESS;
}

int MapEngineOSG::ChangeZoomPercent(int zoom_percent, const d_geo_t& center)
{
   // For now, this renderer will treat non-standard zoom percentages as 100%
   //
   if (zoom_percent == TO_SCALE_ZOOM_PERCENT ||
      zoom_percent == NATIVE_ZOOM_PERCENT)
   {
      zoom_percent = 100.0;
   }

   MapView* map_view = fvw_get_view();
   if (map_view)
   {
      UpdateViewpointCenterAndZoom(map_view->GetMapType(), center,
         zoom_percent);
   }

   return SUCCESS;
}

int MapEngineOSG::ChangeTilt(double tilt_degrees)
{
   fv_common::ScopedCriticalSection lock(&fvw_get_view()->
      GetGeospatialViewController()->m_viewpoint_critical_section);
   m_geospatial_view_controller->GetInputDevice()->
      tilt_value = tilt_degrees;
   m_geospatial_view_controller->GetInputDevice()->handled_shift = true;
   m_geospatial_view_controller->GetInputDevice()->handled_tilt = false;
   return SUCCESS;
}

void MapEngineOSG::Invalidate()
{
   m_geospatial_view_controller->Invalidate();
}

void MapEngineOSG::SetCurrentMapInvalid()
{
   m_geospatial_view_controller->RedrawMapFromScratch();
}

int MapEngineOSG::ScaleIn(MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);
   int ret = m_map_engine_com->ScaleIn(new_map_type);
   if (ret == SUCCESS)
   {
      int zoom_percent = GetCurrentZoomPercentOrDefault();
      UpdateViewpointZoom(*new_map_type, zoom_percent);
   }

   return ret;
}

int MapEngineOSG::ScaleIn(const d_geo_t& center, MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);
   int ret = m_map_engine_com->ScaleIn(center, new_map_type);
   if (ret == SUCCESS)
   {
      int zoom_percent = GetCurrentZoomPercentOrDefault();
      UpdateViewpointCenterAndZoom(*new_map_type, center, zoom_percent);
   }

   return ret;
}

int MapEngineOSG::ScaleOut(MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);
   int ret = m_map_engine_com->ScaleOut(new_map_type);
   if (ret == SUCCESS)
   {
      int zoom_percent = GetCurrentZoomPercentOrDefault();
      UpdateViewpointZoom(*new_map_type, zoom_percent);
   }

   return ret;
}

int MapEngineOSG::ScaleOut(const d_geo_t& center, MapType* new_map_type)
{
   MapEngineFromMapParams(m_map_engine_com);
   int ret = m_map_engine_com->ScaleOut(center, new_map_type);
   if (ret == SUCCESS)
   {
      int zoom_percent = GetCurrentZoomPercentOrDefault();
      UpdateViewpointCenterAndZoom(*new_map_type, center, zoom_percent);
   }

   return ret;
}

int MapEngineOSG::PanMap(int pan_dir, d_geo_t* new_center)
{
   // Pan should never be handled here for 3D rendering. If it gets here
   // then return failure
   return FAILURE;
}

int MapEngineOSG::GetCurrentZoomPercentOrDefault()
{
   MapView* map_view = fvw_get_view();
   if (map_view)
      return map_view->GetMapProjParams().zoom_percent;

   return 100;
}

void MapEngineOSG::UpdateViewpoint(const MapType& map_type,
   const MapProjectionParams& map_proj_params)
{
   auto map_node = m_geospatial_view_controller->GetGeospatialScene()->
      MapNode();

   GenericNodeCallback* node_callback = new GenericNodeCallback;
   node_callback->SetOperation(map_node, [this, map_type, map_proj_params]()
   {
      m_geospatial_view_controller->GetGeospatialScene()->SetCurrentMapType(
         map_type);

      auto viewer = m_geospatial_view_controller->GetGeospatialView()->
         Viewer();

      Viewpoint3d viewpoint;
      m_geospatial_view_controller->GetViewpoint3D(&viewpoint);
      viewpoint.x = map_proj_params.center.lon;
      viewpoint.y = map_proj_params.center.lat;
      viewpoint.z = 0.0;
      viewpoint.heading = -map_proj_params.rotation;
      viewpoint.pitch = map_proj_params.tilt;
      viewpoint.range = camera_utils::ComputeHeightFromMapTypeAndZoom(
         viewer->GetState()->GetSRS(), viewer->getCamera(), map_type,
         map_proj_params.zoom_percent);

      m_geospatial_view_controller->SetViewpoint3D(viewpoint, 0.0);
   });
}

void MapEngineOSG::UpdateViewpointZoom(const MapType& map_type,
   int zoom_percent)
{
   auto map_node = m_geospatial_view_controller->GetGeospatialScene()->
      MapNode();

   GenericNodeCallback* node_callback = new GenericNodeCallback;
   node_callback->SetOperation(map_node, [this, map_type, zoom_percent]()
   {
      m_geospatial_view_controller->GetGeospatialScene()->SetCurrentMapType(
         map_type);

      auto viewer = m_geospatial_view_controller->GetGeospatialView()->Viewer();

      Viewpoint3d viewpoint;
      m_geospatial_view_controller->GetViewpoint3D(&viewpoint);
      viewpoint.range = camera_utils::ComputeHeightFromMapTypeAndZoom(
         viewer->GetState()->GetSRS(), viewer->getCamera(), map_type,
         zoom_percent);
      m_geospatial_view_controller->SetViewpoint3D(viewpoint);
   });
}

void MapEngineOSG::UpdateViewpointCenterAndZoom(const MapType& map_type,
   const d_geo_t& center, int zoom_percent)
{
   auto map_node = m_geospatial_view_controller->GetGeospatialScene()->
      MapNode();

   GenericNodeCallback* node_callback = new GenericNodeCallback;
   node_callback->SetOperation(map_node, [this, map_type, center, zoom_percent]()
   {
      m_geospatial_view_controller->GetGeospatialScene()->
            SetCurrentMapType(map_type);

      auto viewer = m_geospatial_view_controller->GetGeospatialView()->Viewer();

      Viewpoint3d viewpoint;
      m_geospatial_view_controller->GetViewpoint3D(&viewpoint);
      viewpoint.x = center.lon;
      viewpoint.y = center.lat;
      viewpoint.z = 0;
      viewpoint.range = camera_utils::ComputeHeightFromMapTypeAndZoom(
         viewer->GetState()->GetSRS(), viewer->getCamera(), map_type,
         zoom_percent);
      m_geospatial_view_controller->SetViewpoint3D(viewpoint);
   });
}

fvw::CameraMode MapEngineOSG::GetCameraMode()
{
   auto view = m_geospatial_view_controller->GetGeospatialView();
   if (view)
      return view->Viewer()->GetState()->GetActiveCam();

   return fvw::CAMERA_MODE_NONE;
}

void MapEngineOSG::SetCameraMode(fvw::CameraMode camera_mode)
{
   auto view = m_geospatial_view_controller->GetGeospatialView();
   if (view)
      view->Viewer()->GetState()->SetCameraMode(camera_mode);
}

void MapEngineOSG::ToggleEnableFlightModeAutoMapType()
{
   auto view = m_geospatial_view_controller->GetGeospatialView();
   if (view)
      view->Viewer()->GetState()->ToggleEnableFlightModeAutoMapType();
}

bool MapEngineOSG::GetEnableFlightModeAutoMapType()
{
   auto view = m_geospatial_view_controller->GetGeospatialView();
   if (view)
      return view->Viewer()->GetState()->GetEnableFlightModeAutoMapType();
   return false;
}
