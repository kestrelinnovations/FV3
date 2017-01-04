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
-  FILE NAME:    equalarc.cpp
-  LIBRARY NAME: proj.lib
-
-  DESCRIPTION:
-
-
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "stdafx.h"
#include "Projectors.h"
#include "errx.h"



//=============================
// EqualArcProj projection functions
//=============================

// ***********************************************************************
// ***********************************************************************

//========================
// EqualArcProj::xy_to_geo
//========================

int EqualArcProj::xy_to_geo(double x, double y, degrees_t *lat, degrees_t *lon) const
{
   // Make sure projector has been initialized correctly
	if (!m_projection_ready) {
		*lat = 0.0;
		*lon = 0.0;
      ERR_report("Projector has not been properly initialized!");
		return FAILURE;
	}

   // y projection is the same in any case
   *lat = m_top_lat - (degrees_t)y * m_dem_lat_per_pix;

   *lon = (degrees_t)x * m_dem_lon_per_pix + m_left_lon;
   if (*lon > HALF_WORLD_DEG)
      *lon -= WORLD_DEG;
   else if (*lon < -HALF_WORLD_DEG)
      *lon += WORLD_DEG;

	return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int EqualArcProj::xy_to_geo(int x, int y, degrees_t *lat, degrees_t *lon) const
{
	int rslt;

	rslt = xy_to_geo((double) x, (double) y, lat, lon);
	return rslt;
}


// ***********************************************************************
// ***********************************************************************

//========================
// EqualArcProj::geo_to_xy
//========================

int EqualArcProj::geo_to_xy(degrees_t lat, degrees_t lon, double *x, double *y) const
{
   // Make sure projector has been initialized correctly
	if (!m_projection_ready) {
		*x = 0;
		*y = 0;
      ERR_report("Projector has not been properly initialized!");
		return FAILURE;
	}

   degrees_t xx,yy;

   // latitude projection is the same in any case
   yy = (m_top_lat - lat) * m_pix_per_dem_lat;

   if (lon >= m_left_lon) 
      xx = (lon - m_left_lon) * m_pix_per_dem_lon;
   else
      xx = ((lon + HALF_WORLD_DEG) * m_pix_per_dem_lon) + m_IDL_x_coord;

   ASSERT(xx >= 0.0);
   ASSERT(xx < 2147483648.0);
   ASSERT(yy < 2147483648.0);
   ASSERT(yy > -2147483648.0);
   ASSERT(m_pixels_around_world  <= 2147483648);

   // convert exact y coordinate into a int
	*y = yy;

   // convert exact x cooordinate into a int, x is always >= 0
   *x = xx;

	return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int EqualArcProj::geo_to_xy(degrees_t lat, degrees_t lon, int *x, int *y) const
{
	int rslt;
	double xx, yy;

	rslt = geo_to_xy(lat, lon, &xx, &yy);

	*x = (int) (xx + 0.5);

   // keep *x as close to the left edge of the map as posible
   if (*x > m_half_pixels_around_world &&
      *x >= m_virtual_surface_width )
      *x -= m_pixels_around_world;


   if (yy >= 0.0)
      *y = (int)(yy + 0.5);
   else
      *y = (int)(yy - 0.5);
	return rslt;
}

// ***********************************************************************
// ***********************************************************************


//==============================
// EqualArcProj::validate_center
//==============================

int EqualArcProj::validate_center(const MapSource &source, const MapScale &scale,
   const MapSeries &series, boolean_t printing, degrees_t requested_center_lat,
   degrees_t requested_center_lon, int zoom_percent, int surface_width,
   int surface_height, int unrotated_width, int unrotated_height,
   degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix, 
   degrees_t zoomed_deg_lat_per_pix, degrees_t zoomed_deg_lon_per_pix, 
   degrees_t* actual_center_lat, degrees_t* actual_center_lon,
	degrees_t *new_unzoomed_deg_lat_per_pix, degrees_t *new_unzoomed_deg_lon_per_pix, 
	degrees_t* new_zoomed_deg_lat_per_pix, degrees_t* new_zoomed_deg_lon_per_pix)
{

   //
   // If the scale is WORLD, the center lat must be 0.0.  Adjust the center lat
   // here (before the degress per pixel calculation) to ensure that the 
   // calculations below get done correctly at WORLD scale.
   //
   // adjust the map's center latitude to keep the map bounds on the virtual
   // surface within 90.0 N and 90.0 S.
   //
   if (scale == WORLD) {
      *actual_center_lat = 0.0;
		*actual_center_lon = requested_center_lon;
   }
   else {
      *actual_center_lat = requested_center_lat;
		*actual_center_lon = requested_center_lon;
      //
      // geo_height is the geo height from the TOP of the top pixel to the
      // BOTTOM of the bottom pixel.  It is not the height from the center
      // of the top pixel to the center of the bottom pixel.
      // This is because any boundary limit should be at the edge of a pixel,
      // not the center.
      // So surface height = (vsurface_height -1) + 0.5 + 0.5  = vsurface_height;
      //
      const degrees_t geo_height = 
         (degrees_t)(unrotated_height) * (zoomed_deg_lat_per_pix);
      if (geo_height > HALF_WORLD_DEG) {
         ERR_report("geo height too large");
         return FAILURE;
      }

      //
      // enforce north pole boundary
      //
      const degrees_t bounding_northern_lat = *actual_center_lat + geo_height/2.0;
      if (bounding_northern_lat > MAX_LAT_DEG)
         *actual_center_lat = MAX_LAT_DEG - geo_height/2.0;

      //
      // enforce south pole boundary
      //
      const degrees_t bounding_southern_lat = *actual_center_lat - geo_height/2.0;
      if (bounding_southern_lat < -MAX_LAT_DEG)
         *actual_center_lat = -MAX_LAT_DEG + geo_height/2.0; 
   }

   //
   // Note that if the map center changes, the degrees per 
   // pixel longitude can change.  So wait until now (that is,
   // after possible adjusting of the requested center lat) to get the actual 
   // degrees lon per pixel that will be used. 
   //
   // The degrees per pixel latitude is constant (for all current supported 
   // map types).  As a safety check make sure that this value did not 
   // change with the map recentering.
   //
   degrees_t former_unzoomed_deg_lat_per_pix = unzoomed_deg_lat_per_pix;
   if (proj_get_degrees_per_pixel(source, scale, series, printing,
      *actual_center_lat, surface_width, surface_height,
      &unzoomed_deg_lat_per_pix, &unzoomed_deg_lon_per_pix) != SUCCESS)
   {
      ERR_report("proj_get_degrees_per_pixel");
      return FAILURE;
   }
   if (unzoomed_deg_lat_per_pix != former_unzoomed_deg_lat_per_pix)
   {
      ERR_report("deg per pixel mismatch");
      return FAILURE;
   }

   *new_unzoomed_deg_lat_per_pix = unzoomed_deg_lat_per_pix;
   *new_unzoomed_deg_lon_per_pix = unzoomed_deg_lon_per_pix;

   //
   // calc the zoomed degrees of longitude per pixel
   //
   calc_zoomed_degrees_per_pixel(unzoomed_deg_lat_per_pix, zoom_percent,
      new_zoomed_deg_lat_per_pix);
   calc_zoomed_degrees_per_pixel(unzoomed_deg_lon_per_pix, zoom_percent,
      new_zoomed_deg_lon_per_pix);



   return SUCCESS;
}


//===================================
// EqualArcProj::calculate_map_bounds
//===================================

int EqualArcProj::calculate_map_bounds(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, int zoom_percent,
      int unrotated_width, int unrotated_height,
      d_geo_t* map_ll, d_geo_t* map_ur) 
{
   //
   // Calculate the geo map bounds using the virtual surface size.
   //
   d_geo_t center;
   center.lat = center_lat;
   center.lon = center_lon;
   int ret = MAP_center_to_bounds(center, m_dem_lat_per_pix,
      m_dem_lon_per_pix, unrotated_width, unrotated_height, map_ll, map_ur);
   if (ret != WINDOW_BOUNDS_SUCCESS) {
      ERR_printf(("map_center_to_bounds returned %d\ncenter lat: %.6lf lon: %.6lf"
         " %lf %lf %d %d",
         ret, center.lat, center.lon, m_dem_lat_per_pix,
         m_dem_lon_per_pix, vwidth(), vheight()));
      return FAILURE;
   }

   //
   // test for valid map bounds
   //
   ret = test_map_bounds(*map_ll, *map_ur);
   if (ret != 0) {
      ERR_printf(("test_map_bounds returned %d.\nll: %.6f %.6f  ur: %.6f %.6f",
         ret, map_ll->lat, map_ll->lon, map_ur->lat, map_ur->lon));
      return FAILURE;
   }

   return SUCCESS;
}


//==============================
// EqualArcProj::get_convergence
//==============================
int EqualArcProj::get_convergence(d_geo_t position, degrees_t *convergence) {
   if(m_projection_ready != TRUE) {
      ERR_report("Projector has not been properly initialized!");
      return FAILURE;
   }

   *convergence = 0.0;

   return SUCCESS;
}



//==================================
// EqualArcProj::get_projection_type
//==================================

ProjectionID EqualArcProj::get_projection_type() const {
   return ProjectionID(EQUALARC_PROJECTION);
}

