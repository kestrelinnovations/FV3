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
#include "HudProperties.h"

#include "param.h"

namespace
{
   const char* HUD_KEY = "HUD";

   const char* HUD_ON_VALUE = "HudOn";
   int HUD_ON_DEFAULT = 1;

   const char* HUD_COLOR_VALUE = "HudColor";
   COLORREF HUD_COLOR_DEFAULT = RGB(255, 255, 0);

   const char* ATTITUDE_ON_VALUE = "AttitudeOn";
   int HUD_ATTITUDE_ON_DEFAULT = 1;

   const char* EXTRA_INFORMATION_ON_VALUE = "ExtraInformationOn";
   int HUD_EXTRA_INFORMATION_ON_DEFAULT = 1;

   const char* SHOW_HEADING_ONLY_VALUE = "ShowHeadingOnly";
   int SHOW_HEADING_ONLY_DEFAULT = 0;

   const char* MAX_ALT_METERS_VALUE = "MaxAltMeters";
   double MAX_ALT_METERS_DEFAULT = FEET_TO_METERS(1000.0);

   const char* MAX_VELOCITY_KTS_VALUE = "MaxVelocityKts";
   double MAX_VELOCITY_KTS_DEFAULT = 600.0;

   const char* TARGET_LAT_VALUE = "TargetLat";
   double TARGET_LAT_DEFAULT = 0.0;
   
   const char* TARGET_LON_VALUE = "TargetLon";
   double TARGET_LON_DEFAULT = 0.0;

   const char* TARGET_ALT_METERS_VALUE = "TargetAltMeters";
   double TARGET_ALT_METERS_DEFAULT = FEET_TO_METERS(1000.0);
}

HudProperties fvw_prefs::LoadHudProperties()
{
   HudProperties ret;
   ret.hud_on = PRM_get_registry_int(HUD_KEY, HUD_ON_VALUE, HUD_ON_DEFAULT);

   ret.color = PRM_get_registry_int(HUD_KEY, HUD_COLOR_VALUE,
      HUD_COLOR_DEFAULT);
   ret.attitude_on = PRM_get_registry_int(HUD_KEY, ATTITUDE_ON_VALUE,
      HUD_ATTITUDE_ON_DEFAULT);
   ret.extra_information_on = PRM_get_registry_int(HUD_KEY,
      EXTRA_INFORMATION_ON_VALUE, HUD_EXTRA_INFORMATION_ON_DEFAULT);
   ret.show_heading_only = PRM_get_registry_int(HUD_KEY,
      SHOW_HEADING_ONLY_VALUE, SHOW_HEADING_ONLY_DEFAULT);

   ret.max_alt_meters = PRM_get_registry_double(HUD_KEY, MAX_ALT_METERS_VALUE,
      MAX_ALT_METERS_DEFAULT);
   ret.max_velocity_kts = PRM_get_registry_double(HUD_KEY,
      MAX_VELOCITY_KTS_VALUE, MAX_VELOCITY_KTS_DEFAULT);

   ret.target.lat = PRM_get_registry_double(HUD_KEY, TARGET_LAT_VALUE,
      TARGET_LAT_DEFAULT);
   ret.target.lon = PRM_get_registry_double(HUD_KEY, TARGET_LON_VALUE,
      TARGET_LON_DEFAULT);
   ret.target_alt_meters = PRM_get_registry_double(HUD_KEY,
      TARGET_ALT_METERS_VALUE, TARGET_ALT_METERS_DEFAULT);

   return ret;
}

void fvw_prefs::StoreHudProperties(const HudProperties& hud_properties)
{
   PRM_set_registry_int(HUD_KEY, HUD_ON_VALUE, hud_properties.hud_on);

   PRM_set_registry_int(HUD_KEY, HUD_COLOR_VALUE, hud_properties.color);
   PRM_set_registry_int(HUD_KEY, ATTITUDE_ON_VALUE,
      hud_properties.attitude_on);
   PRM_set_registry_int(HUD_KEY, EXTRA_INFORMATION_ON_VALUE,
      hud_properties.extra_information_on);
   PRM_set_registry_int(HUD_KEY, SHOW_HEADING_ONLY_VALUE,
      hud_properties.show_heading_only);

   PRM_set_registry_double(HUD_KEY, MAX_ALT_METERS_VALUE,
      hud_properties.max_alt_meters);
   PRM_set_registry_double(HUD_KEY, MAX_VELOCITY_KTS_VALUE,
      hud_properties.max_velocity_kts);

   PRM_set_registry_double(HUD_KEY, TARGET_LAT_VALUE,
      hud_properties.target.lat);
   PRM_set_registry_double(HUD_KEY, TARGET_LON_VALUE,
      hud_properties.target.lon);
   PRM_set_registry_double(HUD_KEY, TARGET_ALT_METERS_VALUE,
      hud_properties.target_alt_meters);
}