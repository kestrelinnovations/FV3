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

// MapDisplayParams.cpp

#include "stdafx.h"
#include "MapParams.h"

#include "FalconView/MapParamsListener_Interface.h"

MapParams::MapParams() : m_initialized(false)
{
}

void MapParams::Initialize(long map_group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params)
{
   m_initialized = true;
   SetMapTypeAndProjectionParams(map_group_identity, map_type, map_proj_params);
}

void MapParams::AddListener(MapParamsListener_Interface* listener)
{
   m_listeners.push_back(listener);
}

void MapParams::RemoveListener(MapParamsListener_Interface* listener)
{
   m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(),
      listener), m_listeners.end());
}

void MapParams::NotifyListeners(int mask)
{
   if (m_initialized)
   {
      std::for_each(m_listeners.begin(), m_listeners.end(),
         [&](MapParamsListener_Interface* listener)
      {
         listener->MapParamsChanged(mask, m_map_group_identity, m_map_type,
            m_map_proj_params, m_map_display_params);
      });
   }
}

void MapParams::SetMapGroupIdentity(long map_group_identity)
{
   m_map_group_identity = map_group_identity;
   NotifyListeners(MapParamsListener_Interface::MAP_GROUP_CHANGED);
}

void MapParams::SetMapType(const MapType& map_type)
{
   m_map_type = map_type;
   NotifyListeners(MapParamsListener_Interface::MAP_TYPE_CHANGED);
}

void MapParams::SetMapType(const MapType& map_type, const d_geo_t& center)
{
   m_map_type = map_type;
   m_map_proj_params.center = center;
   NotifyListeners(
      MapParamsListener_Interface::MAP_TYPE_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_CENTER_CHANGED);
}

void MapParams::SetMapType(long map_group_identity, const MapType& map_type)
{
   m_map_group_identity = map_group_identity;
   m_map_type = map_type;
   NotifyListeners(
      MapParamsListener_Interface::MAP_GROUP_CHANGED |
      MapParamsListener_Interface::MAP_TYPE_CHANGED);
}

void MapParams::SetMapType(long map_group_identity, const MapType& map_type,
      const d_geo_t& center)
{
   m_map_group_identity = map_group_identity;
   m_map_type = map_type;
   m_map_proj_params.center = center;
   NotifyListeners(
      MapParamsListener_Interface::MAP_GROUP_CHANGED |
      MapParamsListener_Interface::MAP_TYPE_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_CENTER_CHANGED);
}

void MapParams::SetMapTypeAndProjectionParams(long map_group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params)
{
   m_map_group_identity = map_group_identity;
   m_map_type = map_type;
   m_map_proj_params = map_proj_params;
   NotifyListeners(
      MapParamsListener_Interface::MAP_GROUP_CHANGED |
      MapParamsListener_Interface::MAP_TYPE_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_PARAMS_CHANGED);
}

void MapParams::SetCenter(const d_geo_t& center)
{
   m_map_proj_params.center = center;
   NotifyListeners(MapParamsListener_Interface::MAP_PROJ_CENTER_CHANGED);
}

void MapParams::SetProjectionType(ProjectionEnum proj_type)
{
   m_map_proj_params.type = proj_type;
   NotifyListeners(MapParamsListener_Interface::MAP_PROJ_TYPE_CHANGED);
}

void MapParams::SetRotation(double rotation)
{
   m_map_proj_params.rotation = rotation;
   NotifyListeners(MapParamsListener_Interface::MAP_PROJ_ROTATION_CHANGED);
}

void MapParams::SetZoomPercent(int zoom_percent)
{
   m_map_proj_params.zoom_percent = zoom_percent;
   NotifyListeners(MapParamsListener_Interface::MAP_PROJ_ZOOM_PERCENT_CHANGED);
}

void MapParams::SetZoomPercent(int zoom_percent, const d_geo_t& center)
{
   m_map_proj_params.zoom_percent = zoom_percent;
   m_map_proj_params.center = center;
   NotifyListeners(
      MapParamsListener_Interface::MAP_PROJ_ZOOM_PERCENT_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_CENTER_CHANGED);
}

void MapParams::SetTilt(double tilt_degrees)
{
   m_map_proj_params.tilt = tilt_degrees;
   NotifyListeners(MapParamsListener_Interface::MAP_PROJ_TILT_CHANGED);
}

void MapParams::SetRotationAndTilt(double rotation, double tilt_degrees)
{
   m_map_proj_params.rotation = rotation;
   m_map_proj_params.tilt = tilt_degrees;
   NotifyListeners(
      MapParamsListener_Interface::MAP_PROJ_ROTATION_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_TILT_CHANGED);
}

void MapParams::SetMapDisplayParams(const MapDisplayParams& map_display_params)
{
   m_map_display_params = map_display_params;
   NotifyListeners(MapParamsListener_Interface::MAP_DISPLAY_PARAMS_CHANGED);
}

void MapParams::SetBrightness(double brightness)
{
   m_map_display_params.brightness = brightness;
   NotifyListeners(MapParamsListener_Interface::MAP_DISPLAY_BRIGHTNESS_CHANGED);
}

void MapParams::SetContrast(double brightness, double contrast, int contrast_midval)
{
   m_map_display_params.bw_brightness = brightness;
   m_map_display_params.bw_contrast = contrast;
   m_map_display_params.bw_contrast_midval = contrast_midval;
   NotifyListeners(MapParamsListener_Interface::MAP_DISPLAY_CONTRAST_CHANGED);
}

void MapParams::SetSurfaceSize(int surface_width, int surface_height)
{
   m_map_display_params.surface_width = surface_width;
   m_map_display_params.surface_height = surface_height;
   NotifyListeners(
      MapParamsListener_Interface::MAP_DISPLAY_SURFACE_SIZE_CHANGED);
}

void MapParams::SetAutoEnhanceCIB(long auto_contrast)
{
   m_map_display_params.auto_enhance_CIB = auto_contrast;
}
