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

// MapViewTabPrefs.cpp

#include "stdafx.h"
#include "MapViewTabPrefs.h"

#include "FalconView/include/maps.h"
#include "FalconView/include/param.h"
#include "FalconView/include/proj.h"

namespace
{
   const char* SECTION_NAME = "MapViewParams";
   const char* MAP_GROUP_ID_VALUE_NAME = "GroupIdentity";
   const char* MAP_SOURCE_VALUE_NAME = "Source";
   const char* MAP_SCALE_VALUE_NAME = "Scale";
   const char* MAP_SERIES_VALUE_NAME = "Series";
   const char* LATITUDE_VALUE_NAME = "Latitude";
   const char* LONGITUDE_VALUE_NAME = "Longitude";
   const char* ZOOM_PERCENT_VALUE_NAME = "ZoomPercent";
   const char* PROJECTION_TYPE_VALUE_NAME = "ProjectionType";
   const char* TAB_NAME_VALUE_NAME = "TabName";

   const long RASTER_GROUP_ID = 6;

   std::string BuildSectionName(int tab_number)
   {
      std::stringstream ss;
      ss << SECTION_NAME << "\\" << tab_number;
      return ss.str();
   }

   bool DoPrefsExist(int tab_number)
   {
      return tab_number == 0 || PRM_get_registry_int(
         BuildSectionName(tab_number).c_str(),
            MAP_GROUP_ID_VALUE_NAME, -1) != -1;
   }
}

bool MapViewTabPrefs::Read(int tab_number, long* map_group_identity,
   MapType* map_type, MapProjectionParams* map_proj_params,
   std::string* tab_name)
{
   if (!DoPrefsExist(tab_number))
      return false;

   // Defaults if value is not in the registry
   const long DEFAULT_MAP_GROUP_ID = RASTER_GROUP_ID;
   const MapType DEFAULT_MAP_TYPE = MapType(TIROS, WORLD, "TopoBath");
   const MapProjectionParams DEFAULT_MAP_PROJ_PARAMS;
   const char* DEFAULT_TAB_NAME = "%m";

   std::string section = BuildSectionName(tab_number);

   *map_group_identity = PRM_get_registry_int(section.c_str(),
      MAP_GROUP_ID_VALUE_NAME, DEFAULT_MAP_GROUP_ID);

   *map_type = MapType(
      MAP_get_registry_source(section.c_str(), MAP_SOURCE_VALUE_NAME,
         DEFAULT_MAP_TYPE.get_source()),
      MAP_get_registry_scale(section.c_str(), MAP_SCALE_VALUE_NAME,
         DEFAULT_MAP_TYPE.get_scale()),
      MAP_get_registry_series(section.c_str(), MAP_SERIES_VALUE_NAME,
         DEFAULT_MAP_TYPE.get_series()));

   map_proj_params->center.lat = PRM_get_registry_double(section.c_str(),
      LATITUDE_VALUE_NAME, DEFAULT_MAP_PROJ_PARAMS.center.lat);
   map_proj_params->center.lon = PRM_get_registry_double(section.c_str(),
      LONGITUDE_VALUE_NAME, DEFAULT_MAP_PROJ_PARAMS.center.lon);

   map_proj_params->rotation = 0.0;  // not saved in the registry

   map_proj_params->zoom_percent = PRM_get_registry_int(
         section.c_str(), ZOOM_PERCENT_VALUE_NAME,
         DEFAULT_MAP_PROJ_PARAMS.zoom_percent);

   map_proj_params->type =
      MAP_get_registry_projection(section.c_str(), PROJECTION_TYPE_VALUE_NAME,
         PROJ_get_default_projection_type());

   map_proj_params->tilt = -90.0;  // not saved in the registry

   *tab_name = PRM_get_registry_string(section.c_str(), TAB_NAME_VALUE_NAME,
      DEFAULT_TAB_NAME);

   return true;
}

void MapViewTabPrefs::Clear()
{
   PRM_delete_registry_section(SECTION_NAME);
}

void MapViewTabPrefs::Write(int tab_number, long map_group_identity,
   const MapType& map_type, const MapProjectionParams& map_proj_params,
   const std::string& tab_name)
{
   std::string section = BuildSectionName(tab_number);

   PRM_set_registry_int(section.c_str(), MAP_GROUP_ID_VALUE_NAME,
      map_group_identity);

   MAP_set_registry_source(section.c_str(), MAP_SOURCE_VALUE_NAME,
      map_type.get_source());
   MAP_set_registry_scale(section.c_str(), MAP_SCALE_VALUE_NAME,
      map_type.get_scale());
   MAP_set_registry_series(section.c_str(), MAP_SERIES_VALUE_NAME,
      map_type.get_series());

   PRM_set_registry_double(section.c_str(), LATITUDE_VALUE_NAME,
      map_proj_params.center.lat);
   PRM_set_registry_double(section.c_str(), LONGITUDE_VALUE_NAME,
      map_proj_params.center.lon);

   PRM_set_registry_int(section.c_str(), ZOOM_PERCENT_VALUE_NAME,
         map_proj_params.zoom_percent);

   MAP_set_registry_projection(section.c_str(), PROJECTION_TYPE_VALUE_NAME,
         map_proj_params.type);

   PRM_set_registry_string(section.c_str(), TAB_NAME_VALUE_NAME,
      tab_name.c_str());
}
