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

#include "stdafx.h"
#include "..\getobjpr.h"
#include "mapx.h"
#include "utils.h"
#include "..\MapEngineCOM.h"
#include "geo_tool.h"

namespace
{
   MapView* GetMapView(CView* view)
   {
      ASSERT(view);
      ASSERT(view->IsKindOf(RUNTIME_CLASS(MapView)));

      if (view && view->IsKindOf(RUNTIME_CLASS(MapView)))
      {
         return static_cast<MapView*>(view);
      }

      return nullptr;
   }
}

ViewMapProj* UTL_get_current_view_map(CView* view)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return nullptr;
   }

   return map_view->get_curr_map();
}

ViewMapProj* UTL_get_current_view_map()
{
   return UTL_get_current_view_map(UTL_get_active_non_printing_view());
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map(CView* view, const MapSource &source, 
   const MapScale &scale, const MapSeries &series,
   degrees_t latitude, degrees_t longitude, double rotation,
   int zoom_percent, const ProjectionEnum &projection_type,
   boolean_t override_no_data /* = FALSE */)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   // Note that change_map_type can return FV_NO_DATA and
   // COV_FILE_OUT_OF_SYNC.
   MapProjectionParams proj_params;
   proj_params.center.lat = latitude;
   proj_params.center.lon = longitude;
   proj_params.rotation = rotation;
   proj_params.type = projection_type;
   proj_params.zoom_percent = zoom_percent;

   return map_view->ChangeMapType(0, MapType(source, scale, series),
      proj_params);
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map_center(CView* view, degrees_t lat, degrees_t lon)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   // Note that change_center can return FV_NO_DATA and COV_FILE_OUT_OF_SYNC.
   return map_view->ChangeCenter(lat, lon);
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map_rotation(CView* view, double angle,
   degrees_t lat, degrees_t lon)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   // Note that change_rotation can return FV_NO_DATA and COV_FILE_OUT_OF_SYNC (really? /rc225)
   return map_view->ChangeCenterAndRotation( lat, lon, angle );
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map_to_closest_scale(CView* view, MapCategory cat,
   MapScale curr_scale, degrees_t center_lat, degrees_t center_lon, 
   double rot_angle, int zoom_percent, const ProjectionEnum &proj_type)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   // Note that change_to_closest_scale can return FV_NO_DATA and
   // COV_FILE_OUT_OF_SYNC.
   MapProjectionParams proj_params;
   proj_params.center.lat = center_lat;
   proj_params.center.lon = center_lon;
   proj_params.rotation = rot_angle;
   proj_params.type = proj_type;
   proj_params.zoom_percent = zoom_percent;

   return map_view->ChangeToClosestScale(cat, curr_scale, proj_params);
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map_to_closest(CView* view, MapCategory cat,
   MapSource curr_source, MapScale curr_scale, MapSeries curr_series, 
   degrees_t center_lat, degrees_t center_lon, double rot_angle, 
   int zoom_percent, const ProjectionEnum &proj_type)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   MapType map_type(curr_source, curr_scale, curr_series);

   MapProjectionParams proj_params;
   proj_params.center.lat = center_lat;
   proj_params.center.lon = center_lon;
   proj_params.rotation = rot_angle;
   proj_params.type = proj_type;
   proj_params.zoom_percent = zoom_percent;

   return map_view->ChangeToClosestMapType(
      cat.GetGroupId(), map_type, proj_params);
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map_to_smallest_scale(CView* view, MapCategory cat,
   degrees_t center_lat, degrees_t center_lon, 
   double rot_angle, int zoom_percent, const ProjectionEnum &proj_type)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   // Note that change_to_smallest_scale can return FV_NO_DATA and
   // COV_FILE_OUT_OF_SYNC.
   MapProjectionParams proj_params;
   proj_params.center.lat = center_lat;
   proj_params.center.lon = center_lon;
   proj_params.rotation = rot_angle;
   proj_params.type = proj_type;
   proj_params.zoom_percent = zoom_percent;

   return map_view->ChangeToSmallestScale(cat, proj_params);
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map_to_largest_scale(CView* view, MapCategory cat,
   degrees_t center_lat, degrees_t center_lon, 
   double rot_angle, int zoom_percent, const ProjectionEnum &proj_type)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   // Note that change_to_largest_scale can return FV_NO_DATA and
   // COV_FILE_OUT_OF_SYNC.
   MapProjectionParams proj_params;
   proj_params.center.lat = center_lat;
   proj_params.center.lon = center_lon;
   proj_params.rotation = rot_angle;
   proj_params.type = proj_type;
   proj_params.zoom_percent = zoom_percent;

   return map_view->ChangeToLargestScale(cat, proj_params);
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map_to_best(CView* view, degrees_t lat, degrees_t lon)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   // Note that change_to_best can return FV_NO_DATA and
   // COV_FILE_OUT_OF_SYNC.
   return map_view->ChangeToBest(lat, lon);
}

// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
int UTL_change_view_map_to_best(CView* view, degrees_t lat, degrees_t lon,
   const MapSource &source, const MapScale &scale, double rot, int zoom_percent,
   const ProjectionEnum &proj_type)
{
   MapView* map_view = GetMapView(view);
   if (map_view == nullptr)
   {
      return FAILURE;
   }

   // Note that change_to_best can return FV_NO_DATA and
   // COV_FILE_OUT_OF_SYNC.
   MapProjectionParams proj_params;
   proj_params.center.lat = lat;
   proj_params.center.lon = lon;
   proj_params.rotation = rot;
   proj_params.type = proj_type;
   proj_params.zoom_percent = zoom_percent;

   return map_view->ChangeToBest(source, scale, proj_params);
}


int UTL_scale_to_extents(CView* pView, const MapSource &map_source,
   const MapScale &map_scale, const MapSeries &map_series,
   const d_geo_rect_t &boundingRect)
{
   MapSource lcl_map_source(map_source);
   MapScale lcl_map_scale(map_scale);
   MapSeries lcl_map_series(map_series);

   CRect surfaceRect;
   pView->GetClientRect(surfaceRect);

   MapEngineCOM mapEngine;
   mapEngine.init(FALSE, FALSE);
   mapEngine.set_view_dimensions(surfaceRect.Width(), surfaceRect.Height());

   // for CADRG, we want to use the Raster group (not the Raster legacy group)
   if (lcl_map_source == "CADRG")
      lcl_map_source = MapSource("ECRG");

   // VPF fix
   if (lcl_map_source == "Vector Smart Map" ||
      lcl_map_source == "Digital Nautical Chart" ||
      lcl_map_source == "World Vector Shoreline")
   {
      lcl_map_series = "";
   }

   degrees_t center_lat = (boundingRect.ll.lat + boundingRect.ur.lat) / 2.0;
   degrees_t center_lon = (boundingRect.ll.lon + boundingRect.ur.lon) / 2.0;

   mapEngine.change_map_type(lcl_map_source, lcl_map_scale, lcl_map_series, 
      center_lat, center_lon, 0.0, 100, EQUALARC_PROJECTION, TRUE);
   mapEngine.ApplyMap();

   // scale-out until the bounding rect is enclosed completely within the
   // bounds of the map
   int status = SUCCESS;
   d_geo_t map_ll, map_ur;
   mapEngine.get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);
   while (!GEO_enclose(map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon,
      boundingRect.ll.lat, boundingRect.ll.lon, boundingRect.ur.lat,
      boundingRect.ur.lon))
   {
      boolean_t min_scale;
      if (mapEngine.scale_out(FALSE, TRUE, &min_scale) != SUCCESS ||
         min_scale == TRUE)
      {
         status = FAILURE;
         break;
      }
      mapEngine.get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);
   }

   // compute the size of the bounding rect on the closest map
   int boundingRectSize_x = 0, boundingRectSize_y = 0;
   if (status == SUCCESS)
   {
      int ll_x, ll_y, ur_x, ur_y;
      mapEngine.get_curr_map()->geo_to_vsurface(boundingRect.ll.lat,
         boundingRect.ll.lon, &ll_x, &ll_y);
      mapEngine.get_curr_map()->geo_to_vsurface(boundingRect.ur.lat,
         boundingRect.ur.lon, &ur_x, &ur_y);
      boundingRectSize_x = ur_x - ll_x;
      boundingRectSize_y = ll_y - ur_y;
   }

   // if the map type found above exceeds some threshold (projected rectangle
   // is too small) then fall back to blank map instead
   if (boundingRectSize_x < 25 || boundingRectSize_y < 25)
   {
      mapEngine.change_to_closest_scale(BLANK_CATEGORY, lcl_map_source, lcl_map_scale,
         lcl_map_series, center_lat, center_lon, 0.0, 100, EQUALARC_PROJECTION);

      mapEngine.ApplyMap();

      // scale out until the bounding rect completely fits within the surface
      d_geo_t map_ll, map_ur;
      mapEngine.get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);
      while (!GEO_enclose(map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon,
         boundingRect.ll.lat, boundingRect.ll.lon, boundingRect.ur.lat,
         boundingRect.ur.lon))
      {
         boolean_t min_scale;
         if (mapEngine.scale_out(FALSE, TRUE, &min_scale) != SUCCESS ||
            min_scale == TRUE)
         {
            break;
         }
         mapEngine.get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);
      }
   }

   // update the map
   SettableMapProj* map = mapEngine.get_curr_map();
   return UTL_change_view_map(pView, map->source(), map->scale(),
      map->series(), center_lat, center_lon, 0.0, 100, EQUALARC_PROJECTION);
}
