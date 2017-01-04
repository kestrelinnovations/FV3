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

// MapParamsListener_Interface.h

#ifndef FALCONVIEW_MAPPARAMSLISTENER_INTERFACE_H_
#define FALCONVIEW_MAPPARAMSLISTENER_INTERFACE_H_

class MapType;
struct MapProjectionParams;
struct MapDisplayParams;

class MapParamsListener_Interface
{
public:
   static const int CHANGE_MASK_NONE = 0;

   static const int MAP_GROUP_CHANGED = 1;

   static const int MAP_TYPE_CHANGED = 2;

   static const int MAP_PROJ_CENTER_CHANGED = 4;
   static const int MAP_PROJ_ROTATION_CHANGED = 8;
   static const int MAP_PROJ_ZOOM_PERCENT_CHANGED = 16;
   static const int MAP_PROJ_TYPE_CHANGED = 32;
   static const int MAP_PROJ_TILT_CHANGED = 64;
   static const int MAP_PROJ_PARAMS_CHANGED =
      MAP_PROJ_CENTER_CHANGED | MAP_PROJ_ROTATION_CHANGED |
      MAP_PROJ_ZOOM_PERCENT_CHANGED | MAP_PROJ_TYPE_CHANGED |
      MAP_PROJ_TILT_CHANGED;

   static const int MAP_DISPLAY_BRIGHTNESS_CHANGED  = 64;
   static const int MAP_DISPLAY_CONTRAST_CHANGED = 128;
   static const int MAP_DISPLAY_SURFACE_SIZE_CHANGED = 256;
   static const int MAP_DISPLAY_PARAMS_CHANGED =
      MAP_DISPLAY_BRIGHTNESS_CHANGED | MAP_DISPLAY_CONTRAST_CHANGED |
      MAP_DISPLAY_SURFACE_SIZE_CHANGED;

   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) = 0;

   virtual ~MapParamsListener_Interface() { }
};

#endif  // FALCONVIEW_MAPPARAMSLISTENER_INTERFACE_H_