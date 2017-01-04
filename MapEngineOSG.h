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

// MapEngineOSG.h
//

#ifndef FALCONVIEW_MAPENGINEOSG_H_
#define FALCONVIEW_MAPENGINEOSG_H_

#include "FalconView/CameraModes.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/MapRenderingEngine_Interface.h"

class MapEngineCOM;

class MapEngineOSG : public MapRenderingEngine_Interface
{
public:
   MapEngineOSG(GeospatialViewController* gvc);
   ~MapEngineOSG();

   virtual void EnableDraw(bool enable) override;

   virtual void SetSurfaceDimensions(int width, int height) override;

   virtual int ChangeCenter(const d_geo_t& center) override;
   virtual int ChangeCenterAndRotation(const d_geo_t& center,
      double rotation) override;

   virtual int ReinitializeFromMapParams(long group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

   virtual int ChangeMapType(long group_identity, const MapType& map_type,
      const MapProjectionParams& proj_params,
      int override_no_mapdata = FALSE) override;

   virtual int ChangeMapType(long group_identity,
      const MapType& map_type, int override_no_mapdata = FALSE) override;
   virtual int ChangeMapType(long group_identity, const MapType& map_type,
      const d_geo_t& center, int override_no_mapdata = FALSE) override;

   virtual int ChangeToClosestScale(long group_identity,
      MapType* new_map_type) override;
   virtual int ChangeToClosestScale(const MapCategory& category,
      MapType* new_map_type);
   virtual int ChangeToClosestScale(const MapCategory& category,
      const d_geo_t& center, MapType* new_map_type);
   virtual int ChangeToClosestScale(const MapCategory& category,
      const MapScale& scale, const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;

   virtual int ChangeToClosestMapType(long group_identity,
      const MapType& current_map_type,
      const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;

   virtual int ChangeProjectionType(ProjectionEnum projection_type) override;

   virtual int ChangeRotation(double rotation) override;

   virtual void ChangeBrightness(double brightness) override;

   virtual void ChangeContrast(double brightness, double contrast, int contrast_midval) override;

   virtual int ChangeZoomPercent(int zoom_percent) override;

   virtual int ChangeZoomPercent(int zoom_percent,
      const d_geo_t& center) override;

   virtual int ChangeTilt(double tilt_degrees) override;

   virtual int ChangeToSmallestScale(const MapCategory& category,
      const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;
   virtual int ChangeToLargestScale(const MapCategory& category,
      const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;

   virtual int ChangeToBest(const d_geo_t& center,
      MapType* new_map_type) override;
   virtual int ChangeToBest(const MapSource& source, const MapScale& scale,
      const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;

   virtual void Invalidate() override;
   virtual void SetCurrentMapInvalid() override;

   virtual int ScaleIn(MapType* new_map_type) override;
   virtual int ScaleIn(const d_geo_t& center, MapType* new_map_type) override;

   virtual int ScaleOut(MapType* new_map_type) override;
   virtual int ScaleOut(const d_geo_t& center, MapType* new_map_type) override;

   virtual int PanMap(int pan_dir, d_geo_t* new_center) override;

   virtual fvw::CameraMode GetCameraMode() override;
   virtual void SetCameraMode(fvw::CameraMode camera_mode) override;

   virtual void ToggleEnableFlightModeAutoMapType() override;
   virtual bool GetEnableFlightModeAutoMapType() override;

private:
   GeospatialViewController* m_geospatial_view_controller;

   // Used for orthographic projection computations
   MapEngineCOM* m_map_engine_com;

   int GetCurrentZoomPercentOrDefault();

   void UpdateViewpoint(const MapType& map_type,
      const MapProjectionParams& map_proj_params);
   void UpdateViewpointZoom(const MapType& map_type, int zoom_percent);
   void UpdateViewpointCenterAndZoom(const MapType& map_type,
      const d_geo_t& center, int zoom_percent);

};

#endif  // FALCONVIEW_MAPENGINEOSG_H_
