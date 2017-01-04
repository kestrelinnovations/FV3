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

// MapParams_UnitTests.cpp
//

#include "stdafx.h"
#include "MapParams.h"

#include "gtest/gtest.h"

#include "FalconView/MapParamsListener_Interface.h"

namespace
{

class MapParamsListener : public MapParamsListener_Interface
{
public:
   MapParamsListener() :
      m_change_mask(CHANGE_MASK_NONE),
      m_map_group_identity(0)
   {
   }

   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override
   {
      m_change_mask = change_mask;
      m_map_group_identity = map_group_identity;
      m_map_type = map_type;
      m_map_proj_params = map_proj_params;
      m_map_display_params = map_display_params;
   }

   int m_change_mask;
   long m_map_group_identity;
   MapType m_map_type;
   MapProjectionParams m_map_proj_params;
   MapDisplayParams m_map_display_params;
};

}

bool operator==(const MapProjectionParams& lhs, const MapProjectionParams& rhs)
{
   return
      lhs.center.lat == rhs.center.lat &&
      lhs.center.lon == rhs.center.lon &&
      lhs.rotation == rhs.rotation &&
      lhs.zoom_percent == rhs.zoom_percent &&
      lhs.type == rhs.type;
}

bool operator==(const MapDisplayParams& lhs, const MapDisplayParams& rhs)
{
   return
      lhs.brightness == rhs.brightness &&
      lhs.bw_contrast == rhs.bw_contrast &&
      lhs.bw_contrast_midval == rhs.bw_contrast_midval &&
      lhs.bw_brightness == rhs.bw_brightness &&
      lhs.auto_enhance_CIB == rhs.auto_enhance_CIB;
}

struct MapParamsTest : public testing::Test
{
   MapParamsListener listener;
   MapParams map_params;

   void SetUp() override
   {
      map_params.AddListener(&listener);
      map_params.Initialize(0, MapType(), MapProjectionParams());
   }
};

TEST_F(MapParamsTest, ValidateSetMapGroupIdentity)
{
   map_params.SetMapGroupIdentity(6);

   EXPECT_EQ(map_params.GetMapGroupIdentity(), 6);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_GROUP_CHANGED);
   EXPECT_EQ(listener.m_map_group_identity, 6);
}

TEST_F(MapParamsTest, ValidateSetMapType)
{
   MapType map_type = MapType("CADRG", MapScale(5000000), "GNC");

   map_params.SetMapType(map_type);

   EXPECT_EQ(map_params.GetMapType(), map_type);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_TYPE_CHANGED);
   EXPECT_EQ(listener.m_map_type, map_type);
}

TEST_F(MapParamsTest, ValidateSetMapTypeAndCenter)
{
   MapType map_type = MapType("CADRG", MapScale(5000000), "GNC");
   d_geo_t center = { 32.0, -84.0 };

   map_params.SetMapType(map_type, center);

   EXPECT_EQ(map_params.GetMapType(), map_type);
   EXPECT_EQ(map_params.GetMapProjParams().center.lat, center.lat);
   EXPECT_EQ(map_params.GetMapProjParams().center.lon, center.lon);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_TYPE_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_CENTER_CHANGED);
   EXPECT_EQ(listener.m_map_type, map_type);
   EXPECT_EQ(listener.m_map_proj_params.center.lat, center.lat);
   EXPECT_EQ(listener.m_map_proj_params.center.lon, center.lon);
}

TEST_F(MapParamsTest, ValidateSetGroupIdAndMapType)
{
   long group_id = 6;
   MapType map_type = MapType("CADRG", MapScale(5000000), "GNC");

   map_params.SetMapType(group_id, map_type);

   EXPECT_EQ(map_params.GetMapGroupIdentity(), group_id);
   EXPECT_EQ(map_params.GetMapType(), map_type);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_GROUP_CHANGED |
      MapParamsListener_Interface::MAP_TYPE_CHANGED);
   EXPECT_EQ(listener.m_map_group_identity, group_id);
   EXPECT_EQ(listener.m_map_type, map_type);
}

TEST_F(MapParamsTest, ValidateSetGroupIdMapTypeAndCenter)
{
   long group_id = 6;
   MapType map_type = MapType("CADRG", MapScale(5000000), "GNC");
   d_geo_t center = { 41.0, -82 };

   map_params.SetMapType(group_id, map_type, center);

   EXPECT_EQ(map_params.GetMapGroupIdentity(), group_id);
   EXPECT_EQ(map_params.GetMapType(), map_type);
   EXPECT_EQ(map_params.GetMapProjParams().center.lat, center.lat);
   EXPECT_EQ(map_params.GetMapProjParams().center.lon, center.lon);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_GROUP_CHANGED |
      MapParamsListener_Interface::MAP_TYPE_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_CENTER_CHANGED);
   EXPECT_EQ(listener.m_map_group_identity, group_id);
   EXPECT_EQ(listener.m_map_type, map_type);
   EXPECT_EQ(listener.m_map_proj_params.center.lat, center.lat);
   EXPECT_EQ(listener.m_map_proj_params.center.lon, center.lon);
}

TEST_F(MapParamsTest, ValidateSetMapTypeAndProjectionParams)
{
   long group_id = 6;
   MapType map_type = MapType("CADRG", MapScale(5000000), "GNC");
   MapProjectionParams proj_params;
   proj_params.center.lat = 32.0;
   proj_params.center.lon = -84.0;
   proj_params.rotation = 45.0;
   proj_params.zoom_percent = 125;
   proj_params.type = MERCATOR_PROJECTION;

   map_params.SetMapTypeAndProjectionParams(group_id, map_type, proj_params);

   EXPECT_EQ(map_params.GetMapGroupIdentity(), group_id);
   EXPECT_EQ(map_params.GetMapType(), map_type);
   EXPECT_EQ(map_params.GetMapProjParams(), proj_params);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_GROUP_CHANGED |
      MapParamsListener_Interface::MAP_TYPE_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_PARAMS_CHANGED);
   EXPECT_EQ(listener.m_map_group_identity, group_id);
   EXPECT_EQ(listener.m_map_type, map_type);
   EXPECT_EQ(listener.m_map_proj_params, proj_params);
}

TEST_F(MapParamsTest, ValidateSetCenter)
{
   d_geo_t center = { 32.0, -84.0 };

   map_params.SetCenter(center);

   EXPECT_EQ(map_params.GetMapProjParams().center.lat, center.lat);
   EXPECT_EQ(map_params.GetMapProjParams().center.lon, center.lon);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_PROJ_CENTER_CHANGED);
   EXPECT_EQ(listener.m_map_proj_params.center.lat, center.lat);
   EXPECT_EQ(listener.m_map_proj_params.center.lon, center.lon);
}

TEST_F(MapParamsTest, ValidateSetProjectionType)
{
   ProjectionEnum type = MERCATOR_PROJECTION;

   map_params.SetProjectionType(type);

   EXPECT_EQ(map_params.GetMapProjParams().type, type);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_PROJ_TYPE_CHANGED);
   EXPECT_EQ(listener.m_map_proj_params.type, type);
}

TEST_F(MapParamsTest, ValidateSetRotation)
{
   double rotation = 45.0;

   map_params.SetRotation(rotation);

   EXPECT_EQ(map_params.GetMapProjParams().rotation, rotation);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_PROJ_ROTATION_CHANGED);
   EXPECT_EQ(listener.m_map_proj_params.rotation, rotation);
}

TEST_F(MapParamsTest, ValidateSetZoomPercent)
{
   int zoom_percent = 125;

   map_params.SetZoomPercent(zoom_percent);

   EXPECT_EQ(map_params.GetMapProjParams().zoom_percent, zoom_percent);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_PROJ_ZOOM_PERCENT_CHANGED);
   EXPECT_EQ(listener.m_map_proj_params.zoom_percent, zoom_percent);
}

TEST_F(MapParamsTest, ValidateSetZoomPercentAndCenter)
{
   int zoom_percent = 125;
   d_geo_t center = { 32.0, -84.0 };

   map_params.SetZoomPercent(zoom_percent, center);

   EXPECT_EQ(map_params.GetMapProjParams().zoom_percent, zoom_percent);
   EXPECT_EQ(map_params.GetMapProjParams().center.lat, center.lat);
   EXPECT_EQ(map_params.GetMapProjParams().center.lon, center.lon);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_PROJ_ZOOM_PERCENT_CHANGED |
      MapParamsListener_Interface::MAP_PROJ_CENTER_CHANGED);
   EXPECT_EQ(listener.m_map_proj_params.zoom_percent, zoom_percent);
   EXPECT_EQ(listener.m_map_proj_params.center.lat, center.lat);
   EXPECT_EQ(listener.m_map_proj_params.center.lon, center.lon);
}

TEST_F(MapParamsTest, ValidateSetMapDisplayParams)
{
   MapDisplayParams display_params;
   display_params.brightness = 1.1;
   display_params.bw_contrast = 0.0;
   display_params.bw_contrast_midval = 35;
   display_params.auto_enhance_CIB = 1;
   display_params.surface_width = 800;
   display_params.surface_height = 600;

   map_params.SetMapDisplayParams(display_params);

   EXPECT_EQ(map_params.GetMapDisplayParams(), display_params);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_DISPLAY_PARAMS_CHANGED);
   EXPECT_EQ(listener.m_map_display_params, display_params);
}

TEST_F(MapParamsTest, ValidateSetBrightness)
{
   double brightness = 2.4;

   map_params.SetBrightness(brightness);

   EXPECT_EQ(map_params.GetMapDisplayParams().brightness, brightness);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_DISPLAY_BRIGHTNESS_CHANGED);
   EXPECT_EQ(listener.m_map_display_params.brightness, brightness);
}

TEST_F(MapParamsTest, ValidateSetContrast)
{
   double brightness = 0.0;
   double contrast = -1.0;
   int contrast_midval = 12;

   map_params.SetContrast(brightness, contrast, contrast_midval);

   EXPECT_EQ(map_params.GetMapDisplayParams().bw_brightness, brightness);
   EXPECT_EQ(map_params.GetMapDisplayParams().bw_contrast, contrast);
   EXPECT_EQ(map_params.GetMapDisplayParams().bw_contrast_midval, contrast_midval);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_DISPLAY_CONTRAST_CHANGED);
   EXPECT_EQ(listener.m_map_display_params.bw_brightness, brightness);
   EXPECT_EQ(listener.m_map_display_params.bw_contrast, contrast);
   EXPECT_EQ(listener.m_map_display_params.bw_contrast_midval, contrast_midval);
}

TEST_F(MapParamsTest, ValidateSetSurfaceSize)
{
   int surface_width = 800;
   int surface_height = 600;

   map_params.SetSurfaceSize(surface_width, surface_height);

   EXPECT_EQ(map_params.GetMapDisplayParams().surface_width, surface_width);
   EXPECT_EQ(map_params.GetMapDisplayParams().surface_height, surface_height);
   EXPECT_EQ(listener.m_change_mask,
      MapParamsListener_Interface::MAP_DISPLAY_SURFACE_SIZE_CHANGED);
   EXPECT_EQ(listener.m_map_display_params.surface_width, surface_width);
   EXPECT_EQ(listener.m_map_display_params.surface_height, surface_height);
}

TEST(MapParams, NoNotificationsBeforeInitializeCalled)
{
   MapParamsListener listener;
   MapParams map_params;
   map_params.AddListener(&listener);

   int surface_width = 800;
   int surface_height = 600;
   map_params.SetSurfaceSize(surface_width, surface_height);

   EXPECT_EQ(listener.m_change_mask, 0);
}
