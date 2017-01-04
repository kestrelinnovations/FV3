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
-  FILE NAME:    mercator.cpp
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

//========================
// MercatorProj::geo_to_xy
//========================
int MercatorProj::geo_to_xy(degrees_t lat, degrees_t lon, double *x, double *y) const 
{
   if (!m_projection_ready) 
	{
      *x = 0.0;
      *y = 0.0;
      ERR_report("Projector has not been properly initialized!");
      return FAILURE;
   }

   double delta_lon = lon - m_center_lon;
   if (delta_lon < -180.0) 
		delta_lon += 360.0;
   else if(delta_lon > 180.0) 
		delta_lon -= 360.0;

   double xx = WGS84_a_METERS*DEG_TO_RAD(delta_lon)*cos(DEG_TO_RAD(m_std_parallel));
   xx /= m_meters_per_pix_lon; // m_meters_per_pix;

   if(m_unrotated_width > 0)
      xx += ((double)m_unrotated_width)*0.5;
   *x = xx;

   double y0 = WGS84_a_METERS*log(tan(PI/4 + DEG_TO_RAD(m_center_lat/2)))*cos(DEG_TO_RAD(m_std_parallel));
   double yy = WGS84_a_METERS*log(tan(PI/4 + DEG_TO_RAD(lat/2)))*cos(DEG_TO_RAD(m_std_parallel));
   yy = (y0-yy)/m_meters_per_pix_lat;
   if(m_unrotated_height > 0)
      yy += ((double)m_unrotated_height)*0.5;
   *y = yy;

   return SUCCESS;
}


int MercatorProj::geo_to_xy(degrees_t lat, degrees_t lon, int *x, int *y) const 
{
	int rslt;
	double xx, yy;

	rslt = geo_to_xy(lat, lon, &xx, &yy);

   *x = (int)(xx + 0.5);
   *y = (int)(yy + 0.5);

   return rslt;
}


// ***********************************************************************
// ***********************************************************************

//========================
// MercatorProj::xy_to_geo
//========================

int MercatorProj::xy_to_geo(double x, double y, degrees_t *lat, degrees_t *lon) const 
{
   double xx, yy, y0;

   if (!m_projection_ready) {
      *lat = 0.0;
      *lon = 0.0;
      ERR_report("Projector has not been properly initialized!");
      return FAILURE;
   }

   xx = x;
   if(m_unrotated_width > 0)
      xx -= 0.5*m_unrotated_width;

   xx *= m_meters_per_pix_lon; // m_meters_per_pix;
   
	*lon = RAD_TO_DEG((xx/cos(DEG_TO_RAD(m_std_parallel)))/WGS84_a_METERS) + m_center_lon;
   
	if(*lon < -180.0) 
		*lon += 360.0;
   else if(*lon > 180.0) 
		*lon -= 360.0;

   // Note Robert: This computation is a good candidate to go into the  compute params function
   // cos(DEG_TO_RAD(m_std_parallel)) is also a good candidate to be precomputed at proj intitialization
   y0 = WGS84_a_METERS*log(tan(PI/4 + DEG_TO_RAD(m_center_lat/2)))*cos(DEG_TO_RAD(m_std_parallel));

   yy = y;
   if(m_unrotated_height > 0)
      yy -= 0.5*m_unrotated_height;
   
	yy *= -m_meters_per_pix_lat;
   
	yy = yy + y0;
   
	*lat = RAD_TO_DEG(0.5*PI - 2*atan(exp((-yy/cos(DEG_TO_RAD(m_std_parallel)))/WGS84_a_METERS)));
   
	if ((*lat < -90.0) || (*lat > 90.0) || (*lon < -180.0) || (*lon > 180.0))
      return FAILURE;

	return SUCCESS;
}


// ***********************************************************************
// ***********************************************************************

int MercatorProj::xy_to_geo(int x, int y, degrees_t *lat, degrees_t *lon) const 
{
	int rslt;

	rslt = xy_to_geo((double) x, (double) y, lat, lon);
	return rslt;
}

// ***********************************************************************
// ***********************************************************************


//==============================
// MercatorProj::get_convergence
//==============================
int MercatorProj::get_convergence(d_geo_t position, degrees_t *convergence) 
{
   *convergence = 0.0;
   return SUCCESS;
}

//==================================
// MercatorProj::get_projection_type
//==================================
ProjectionID MercatorProj::get_projection_type() const {
   return ProjectionID(MERCATOR_PROJECTION);
}

//========================================================
// MercatorProj::initialize_projection_specific_parameters
//========================================================
int MercatorProj::initialize_projection_specific_parameters(const MapScale &scale,
	degrees_t center_lat, degrees_t center_lon, degrees_t zoomed_deg_lat_per_pix,
   degrees_t zoomed_deg_lon_per_pix, int screen_width, int screen_height,
	bool use_mosaic_map_parameters /*=false*/, degrees_t mosaic_std_parallel_1, 
		degrees_t mosaic_std_parallel_2, degrees_t mosaic_center_lat,
		degrees_t mosaic_center_lon)
{
	// Set standard parallel
   m_std_parallel = center_lat;

   return SUCCESS;
}


//===================================
// MercatorProj::calculate_map_bounds
//===================================

int MercatorProj::calculate_map_bounds(const MapSource &source, 
   const MapScale &scale, const MapSeries &series, boolean_t printing,
   degrees_t center_lat, degrees_t center_lon, int zoom_percent,
   int unrotated_width, int unrotated_height,
   d_geo_t* map_ll, d_geo_t* map_ur)
{

	degrees_t top, bottom, left, right;

	if (!m_projection_ready) {
		ERR_report("Projector has not been properly initialized!");
		return FAILURE;
	}

	ASSERT(unrotated_width > 0 && unrotated_height > 0);

	xy_to_geo(0,0,&top,&left);
	xy_to_geo(unrotated_width-1,unrotated_height-1,&bottom,&right);

	if(left < -180.0)
		left += 360.0;
	if(right > 180.0)
		right -= 360.0;

	map_ll->lat = bottom;
	map_ll->lon = left;
	map_ur->lat = top;
	map_ur->lon = right;

	return SUCCESS;
}

//==================================
// MercatorProj::calc_degrees_per_pixel
//==================================
int MercatorProj::calc_degrees_per_pixel(const MapSource &source, 
   const MapScale &scale, const MapSeries &series, boolean_t printing,
   degrees_t center_lat, degrees_t center_lon, 
   int zoom_percent, int surface_width, int surface_height,
   degrees_t* unzoomed_deg_lat_per_pix, degrees_t* unzoomed_deg_lon_per_pix,
   degrees_t* zoomed_deg_lat_per_pix, degrees_t* zoomed_deg_lon_per_pix)
{
   // WORLD is a special scale - dpp is adjusted to fill the surface
   if (scale == WORLD)
	{
		if (surface_width <= 0 || surface_height <= 0)
		{
			ERR_report("Invalid surface dimensions.");
			return FAILURE;
		}

		double lat_span = fabs(2.0*MERCATOR_MAX_LAT);

		/* measure from center of pixels */
		//*unzoomed_deg_lat_per_pix = lat_span/(double)(surface_height);
		*unzoomed_deg_lon_per_pix = WORLD_DEG/(double)(surface_width);
		*unzoomed_deg_lat_per_pix = *unzoomed_deg_lon_per_pix;
	}
	else
	{
		// 
		// Retrieve degrees/pixel values for raw image data
		//
		if (proj_get_degrees_per_pixel(source, scale, series, printing, 
			center_lat, surface_width, surface_height,
			unzoomed_deg_lat_per_pix, unzoomed_deg_lon_per_pix) != SUCCESS)
		{
			ERR_report("map_get_degrees_per_pixel");
			return FAILURE;
		}
	}

   //
   // calc the zoomed degrees/pixel values
   //
   calc_zoomed_degrees_per_pixel(*unzoomed_deg_lat_per_pix, zoom_percent,
      zoomed_deg_lat_per_pix);
   calc_zoomed_degrees_per_pixel(*unzoomed_deg_lon_per_pix, zoom_percent,
      zoomed_deg_lon_per_pix);

   return SUCCESS;
}


//==============================
// MercatorProj::validate_center
//==============================

int MercatorProj::validate_center(const MapSource &source, const MapScale &scale,
   const MapSeries &series, boolean_t printing, degrees_t requested_center_lat,
   degrees_t requested_center_lon, int zoom_percent, int surface_width,
   int surface_height, int unrotated_width, int unrotated_height,
   degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix, 
   degrees_t zoomed_deg_lat_per_pix, degrees_t zoomed_deg_lon_per_pix, 
   degrees_t* actual_center_lat, degrees_t* actual_center_lon,
	degrees_t *new_unzoomed_deg_lat_per_pix, degrees_t *new_unzoomed_deg_lon_per_pix, 
	degrees_t* new_zoomed_deg_lat_per_pix, degrees_t* new_zoomed_deg_lon_per_pix)
{
   ASSERT(requested_center_lat > -90.0 && requested_center_lat < 90.0);

   if ( scale == WORLD) 
	{
      *actual_center_lat = 0.0;
		*actual_center_lon = requested_center_lon;

	   *new_unzoomed_deg_lat_per_pix = unzoomed_deg_lat_per_pix;
		*new_unzoomed_deg_lon_per_pix = unzoomed_deg_lon_per_pix;
		*new_zoomed_deg_lat_per_pix = zoomed_deg_lat_per_pix;
		*new_zoomed_deg_lon_per_pix = zoomed_deg_lon_per_pix;
	}
   else 
	{
      *actual_center_lat = requested_center_lat;
		*actual_center_lon = requested_center_lon;

		*new_unzoomed_deg_lat_per_pix = unzoomed_deg_lat_per_pix;
		*new_unzoomed_deg_lon_per_pix = unzoomed_deg_lon_per_pix;
		*new_zoomed_deg_lat_per_pix = zoomed_deg_lat_per_pix;
		*new_zoomed_deg_lon_per_pix = zoomed_deg_lon_per_pix;

		ASSERT(unrotated_width > 0 && unrotated_height > 0);

		// Calculate an appropriate meters-per-pixel value
		double meters_per_pixel_lat, meters_per_pixel_lon;
		if ( calc_meters_per_pixel(scale, requested_center_lat, requested_center_lon, unzoomed_deg_lat_per_pix,
			unzoomed_deg_lon_per_pix, zoom_percent, &meters_per_pixel_lat, &meters_per_pixel_lon, printing) != SUCCESS )
		{
			ERR_report("calc_meters_per_pixel");
			return FAILURE;
		}

		// These values will be used in the temporary setting of the 
		// projection params
		m_meters_per_pix_lat = meters_per_pixel_lat;
		m_meters_per_pix_lon = meters_per_pixel_lon;

		m_std_parallel = requested_center_lat;
		m_center_lat = requested_center_lat;
		m_center_lon = requested_center_lon;
		m_projection_ready = TRUE;

		// iterate to find the center that do not exceed the max (north/south) lat
		degrees_t top, bottom, dummy;
		degrees_t delta_lat = unzoomed_deg_lat_per_pix;	// empirical value
		
		degrees_t new_center_lat = requested_center_lat;;

      // enforce north/south Mercator boundary
		if ( requested_center_lat >= MAX_LAT_DEG )
			new_center_lat = MAX_LAT_DEG - delta_lat;
		
		if ( requested_center_lat <= -MAX_LAT_DEG )
			new_center_lat = -MAX_LAT_DEG + delta_lat;
		

      // check if in northern hemisphere
		if ( requested_center_lat >= 0.0 )
		{
			xy_to_geo(0,0,&top,&dummy);

			while ( top > MERCATOR_MAX_LAT )
			{
				// move the center by the delta
				new_center_lat -= delta_lat;

				/// compute the new meters per pixel constant at the new center latitude
				if ( calc_meters_per_pixel(scale, new_center_lat, requested_center_lon, 
					unzoomed_deg_lat_per_pix, unzoomed_deg_lon_per_pix, zoom_percent, &meters_per_pixel_lat,
					&meters_per_pixel_lon, printing) != SUCCESS )
				{
					ERR_report("calc_meters_per_pixel");
					return FAILURE;
				}

				// store the new mpp constant 
				m_meters_per_pix_lat = meters_per_pixel_lat;
				m_meters_per_pix_lon = meters_per_pixel_lon;

				m_std_parallel = new_center_lat;
				m_center_lat = new_center_lat;

				// compute the new top latitude
				xy_to_geo(0,0,&top,&dummy);
			}
		}
		else	// southern hemisphere
		{
			xy_to_geo(unrotated_width-1,unrotated_height-1,&bottom,&dummy);

			while ( bottom < -MERCATOR_MAX_LAT )
			{
				// move the center by the delta
				new_center_lat += delta_lat;

				/// compute the new meters per pixel constant at the new center latitude
				if ( calc_meters_per_pixel(scale, new_center_lat, requested_center_lon, unzoomed_deg_lat_per_pix,
						unzoomed_deg_lon_per_pix,zoom_percent, &meters_per_pixel_lat, &meters_per_pixel_lon, printing) != SUCCESS )
				{
					ERR_report("calc_meters_per_pixel");
					return FAILURE;
				}


				// store the new mpp constant 
				m_meters_per_pix_lat = meters_per_pixel_lat;
				m_meters_per_pix_lon = meters_per_pixel_lon;

				m_std_parallel = new_center_lat;
				m_center_lat = new_center_lat;

				// compute the new top latitude
				xy_to_geo(unrotated_width-1,unrotated_height-1,&bottom,&dummy);
			}
		}

      *actual_center_lat = new_center_lat;

		// undo temporary changes
		m_projection_ready = FALSE;

		if ( requested_center_lat == *actual_center_lat )
			return SUCCESS;

		// Don't know if the following still holds for Mercator

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
   }


	return SUCCESS;
}

//=================================
// MercatorProj::calc_meters_per_pixel
//=================================
int MercatorProj::calc_meters_per_pixel(const MapScale &scale, degrees_t center_lat, degrees_t center_lon,
                                      double unzoomed_deg_per_pix_lat, double unzoomed_deg_per_pix_lon,
                                      int zoom_percent, double *meters_per_pixel_lat, double *meters_per_pixel_lon,
												  boolean_t is_printing)
{
	int status = FAILURE;

	if ( scale == WORLD )
	{
		double m_p_p = 0.0;
		double left_lon, right_lon;
		double dummy;

		double num_of_pixels_lat = 10.0;

		if(center_lon >= 0) {
			right_lon = center_lon;
			left_lon = center_lon - num_of_pixels_lat*m_unzoomed_dem_lon_per_pix;
		}
		else {
			right_lon = center_lon + num_of_pixels_lat*m_unzoomed_dem_lon_per_pix;
			left_lon = center_lon;
		}

		GEO_geo_to_distance(center_lat, left_lon, center_lat, right_lon, &m_p_p, &dummy);

		m_p_p *= (1.0/num_of_pixels_lat);

		ASSERT( m_p_p >= 0.0 );
		*meters_per_pixel_lon = m_p_p;

		// Compute the meters per pixel of latitude. In Mercator we need to limit
		// the latitude to a maximum 

		ASSERT( m_actual_surface_height != 0 );
		double y;
		y = WGS84_a_METERS*log(tan(PI/4 + DEG_TO_RAD(MERCATOR_MAX_LAT/2.0)))*cos(DEG_TO_RAD(m_std_parallel));
		m_p_p = fabs(y / static_cast<double>(m_actual_surface_height/2.0));

		*meters_per_pixel_lat = m_p_p;

		status = SUCCESS;
	}
	else
	{
		// compute the meters per pixel of latitude
		status = Projector::calc_meters_per_pixel(scale, center_lat, center_lon, 
				unzoomed_deg_per_pix_lat, unzoomed_deg_per_pix_lon, zoom_percent, 
				meters_per_pixel_lat, meters_per_pixel_lon, is_printing);
	}

	return status;
}
