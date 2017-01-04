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

// MapRenderingEngine_Interface.h

#ifndef FALCONVIEW_MAPRENDERINGENGINEINTERFACE_H_
#define FALCONVIEW_MAPRENDERINGENGINEINTERFACE_H_

#include "FalconView/CameraModes.h"
#include "FalconView/include/maps_d.h"

struct MapDisplayParams;

class MapRenderingEngine_Interface
{
public:

   virtual void EnableDraw(bool enable) = 0;

   virtual void SetSurfaceDimensions(int width, int height) = 0;

   virtual int ChangeCenter(const d_geo_t& center) = 0;
   virtual int ChangeCenterAndRotation(const d_geo_t& center,
      double rotation) = 0;

   virtual int PanMap(int pan_dir, d_geo_t* new_center) = 0;

   virtual int ReinitializeFromMapParams(long group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) = 0;

   virtual int ChangeMapType(long group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      int override_no_mapdata = FALSE) = 0;

   virtual int ChangeMapType(long group_identity,
      const MapType& map_type, int override_no_mapdata = FALSE) = 0;
   virtual int ChangeMapType(long group_identity, const MapType& map_type,
      const d_geo_t& center, int override_no_mapdata = FALSE) = 0;

   virtual int ChangeToClosestScale(long group_identity,
      MapType* new_map_type) = 0;
   virtual int ChangeToClosestScale(const MapCategory& category,
      MapType* new_map_type) = 0;
   virtual int ChangeToClosestScale(const MapCategory& category,
      const d_geo_t& center, MapType* new_map_type) = 0;
   virtual int ChangeToClosestScale(const MapCategory& category,
      const MapScale& scale, const MapProjectionParams& map_proj_params,
      MapType* new_map_type) = 0;

   virtual int ChangeToClosestMapType(long group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params,
      MapType* new_map_type) = 0;

   virtual int ChangeProjectionType(ProjectionEnum projection_type) = 0;

   virtual int ChangeRotation(double rotation) = 0;
   virtual void ChangeBrightness(double brightness) = 0;
   virtual void ChangeContrast(double brightness, double contrast, int contrast_midval) = 0;

   virtual int ChangeZoomPercent(int zoom_percent) = 0;
   virtual int ChangeZoomPercent(int zoom_percent, const d_geo_t& center) = 0;

   virtual int ChangeTilt(double tilt_degrees) = 0;

   virtual int ChangeToSmallestScale(const MapCategory& category,
      const MapProjectionParams& map_proj_params, MapType* new_map_type) = 0;
   virtual int ChangeToLargestScale(const MapCategory& category,
      const MapProjectionParams& map_proj_params, MapType* new_map_type) = 0;

   virtual int ChangeToBest(const d_geo_t& center, MapType* new_map_type) = 0;
   virtual int ChangeToBest(const MapSource& source, const MapScale& scale,
      const MapProjectionParams& map_proj_params, MapType* new_map_type) = 0;

   virtual void Invalidate() = 0;
   virtual void SetCurrentMapInvalid() = 0;

   virtual int ScaleIn(MapType* new_map_type) = 0;
   virtual int ScaleIn(const d_geo_t& center, MapType* new_map_type) = 0;
   virtual int ScaleOut(MapType* new_map_type) = 0;
   virtual int ScaleOut(const d_geo_t& center, MapType* new_map_type) = 0;

   virtual fvw::CameraMode GetCameraMode() = 0;
   virtual void SetCameraMode(fvw::CameraMode camera_mode) = 0;

   virtual void ToggleEnableFlightModeAutoMapType() = 0;
   virtual bool GetEnableFlightModeAutoMapType() = 0;

   virtual ~MapRenderingEngine_Interface() { }
};

#endif  // FALCONVIEW_MAPRENDERINGENGINEINTERFACE_H_
