// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
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



/*------------------------------------------------------------------
-  FILE NAME:    c_to_b.c
-  LIBRARY NAME: maps.lib
-
-  DESCRIPTION:
-
-      These functions convert between the center \ degrees per pixel
-  map representation and the bounding rectangle map representation.
-  Both forms are equal arc projections.
-
-  PUBLIC FUNCTIONS: NONE
-
-  PRIVATE FUNCTIONS:
-
-      MAP_bounds_to_center
-      map_center_to_bounds
-
-  STATIC FUNCTIONS: NONE
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
-
-  REVISION HISTORY:
-
-       $Log: c_to_b.cpp $
//Revision 1.3  1994/11/28  10:51:20  gue
//map_center_to_bounds: changed to take screen width and height as parameters 
//   rather than calling map_get_virtual_screen_size_for_requested_map
//
//Revision 1.2  1994/10/26  05:40:50  gue
//map_center_to_bounds: changed map_get_virtual_screen_size_for_current_map
//   to map_get_virtual_screen_size_for_requested_map.
//
//Revision 1.1  1994/10/21  11:41:19  gue
//Initial revision
//
 * Revision 1.1  1994/03/17  18:27:50  vinny
 * Initial revision
 * 
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/


#include "stdafx.h"
#include "maps.h"  // For center-to-bounds return codes - RMP
#include "geo_tool.h"


/*------------------------------------------------------------------
-  FUNCTION NAME: MAP_bounds_to_center
-  PROGRAMMER:    Vincent Sollicito
-  DATE:          January 1994
-
-  PURPOSE:
-
-      Converts the bounding rectangle map representation to the
-  center \ degrees per pixel representation of an equal arc map
-  projection.
-
-  PARAMETERS:
-
-      map_ll
-      map_ur
-      center
-
-  RETURN VALUES:
-
-      INVALID_WINDOW_UR
-      INVALID_WINDOW_LL
-      INVALID_WINDOW_LAT
-      INVALID_WINDOW_LON
-      WINDOW_BOUNDS_SUCCESS
-
-  PRECONDITIONS: NONE
-
-  EXTERNALS MODIFIED: NONE
-
-  REQUIRED INCLUDES:
-
-      equarc.h
-      common.h
-      geo_tool.h
-
-  DESCRIPTION: NONE
-------------------------------------------------------------------*/

int MAP_bounds_to_center(d_geo_t map_ll, d_geo_t map_ur, d_geo_t *center)
{
   degrees_t geo_width, geo_height;

   if (GEO_valid_degrees(map_ur.lat, map_ur.lon) != TRUE)
      return INVALID_WINDOW_UR;

   if (GEO_valid_degrees(map_ll.lat, map_ll.lon) != TRUE)
      return INVALID_WINDOW_LL;

   geo_height = map_ur.lat - map_ll.lat;
   if (geo_height <= 0.0)
      return INVALID_WINDOW_LAT;

   geo_width = map_ur.lon - map_ll.lon;
   if (geo_width <= 0.0)
      geo_width += WORLD_DEG;

   if (geo_width < 0.0)
      return INVALID_WINDOW_LON;

   center->lat = map_ll.lat + geo_height/2.0;
   center->lon = map_ll.lon + geo_width/2.0;
   if (center->lon > HALF_WORLD_DEG)
      center->lon -= WORLD_DEG;

   return WINDOW_BOUNDS_SUCCESS;
}


/*------------------------------------------------------------------
-  FUNCTION NAME: map_center_to_bounds
-  PROGRAMMER:    Vincent Sollicito
-  DATE:          January 1994
-
-  PURPOSE:
-
-      Convert the center \ degrees per pixel map representation to
-  the bounding rectangle map representation of an equal arc map
-  projection.
-
-  PARAMETERS:
-
-      center
-      deg_lat_per_pix
-      deg_lon_per_pix
-      map_ll
-      map_ur
-
-  RETURN VALUES:
-
-      INVALID_WINDOW_CENTER
-      WINDOW_PAST_NORTH_POLE
-      WINDOW_PAST_SOUTH_POLE
-      WINDOW_BOUNDS_SUCCESS
-
-  PRECONDITIONS: NONE
-
-  EXTERNALS MODIFIED: NONE
-
-  REQUIRED INCLUDES:
-
-      equarc.h
-      common.h
-      geo_tool.h
-
-  DESCRIPTION:
-
-      The screen dimensions in pixels are combined with the two
-  degrees per pixel inputs to get one half the geographic width
-  and height respectively. Here the geographic width is the distance
-  in degrees longitude from the first column of pixels to the last,
-  measured pixel center to pixel center, and the geographic height
-  is defined in a similar fashion. These values are used to get the
-  geo coordinate at the center of the lower left and upper right
-  pixels. If map_ur.lat > 90.0 then WINDOW_PAST_NORTH_POLE is returned.
-  If map_ll.lat < -90.0 then WINDOW_PAST_SOUTH_POLE is returned.
-------------------------------------------------------------------*/

int MAP_center_to_bounds(d_geo_t center, 
   degrees_t deg_lat_per_pix, degrees_t deg_lon_per_pix,
   int screen_width, int screen_height,
   d_geo_t *map_ll, d_geo_t *map_ur)
{
   degrees_t half_map_width, half_map_height;
   degrees_t longitude, latitude;

   if (GEO_valid_degrees(center.lat, center.lon) != TRUE)
      return INVALID_WINDOW_CENTER;

   /* distance to center of corner pixels from map center */
   if (screen_width % 2 == 0)
      half_map_width = ((degrees_t)(screen_width/2) - 0.5) * deg_lon_per_pix;
   else
      half_map_width = (degrees_t)(screen_width/2) * deg_lon_per_pix;
   if (screen_height % 2 == 0)
      half_map_height = ((degrees_t)(screen_height/2) - 0.5) * deg_lat_per_pix;
   else
      half_map_height = (degrees_t)(screen_height/2) * deg_lat_per_pix;

   /* ur pixel's longitude in degrees */
   longitude = center.lon + half_map_width;
   if (longitude > HALF_WORLD_DEG)
      longitude -= WORLD_DEG;
   map_ur->lon = longitude;

   /* ll pixel's longitude in degrees */ 
   longitude = center.lon - half_map_width;
   if (longitude < -HALF_WORLD_DEG)
      longitude += WORLD_DEG;
   map_ll->lon = longitude;

   degrees_t geo_width = map_ur->lon - map_ll->lon;
   if (geo_width < 0.0)
      geo_width += WORLD_DEG;
   if (geo_width < ((double)(screen_width - 2))*deg_lon_per_pix)
      return WINDOW_MORE_THAN_360_DEGREES;

   /* ur pixel's latitude in degrees */
   latitude = center.lat + half_map_height;
   if (latitude > MAX_LAT_DEG)
      return WINDOW_PAST_NORTH_POLE;
   else
      map_ur->lat = latitude;

   /* ur pixel's latitude in degrees */
   latitude = center.lat - half_map_height;
   if (latitude < MIN_LAT_DEG)
      return WINDOW_PAST_SOUTH_POLE;
   else
      map_ll->lat = latitude; 

   return WINDOW_BOUNDS_SUCCESS;
}
