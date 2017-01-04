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
-  FILE NAME:    lambert.cpp
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



//=================================
// LambertProj projection functions
//=================================

// ***********************************************************************
// ***********************************************************************

//=======================
// LambertProj::xy_to_geo
//=======================

int LambertProj::xy_to_geo(double x, double y, degrees_t *lat, degrees_t *lon) const 
{
	if (!m_projection_ready) 
	{
		*lat = 0.0;
		*lon = 0.0;
      ERR_report("Projector has not been properly initialized!");
		return FAILURE;
	}

   // (Un)project the point to a geo coordinate
   double xx = x;
   double yy = y;

	// for mosaic map pages, we need to convert the given x, y of the page
	// to the corresponding x,y of the mosaic map
	if (m_is_mosaic_map_component)
	{
		double scale_x = (double)(m_mosaic_surface_width +(m_mosaic_offset_x*(m_mosaic_num_col-1)))/ 
			(m_mosaic_num_col * (double)m_actual_surface_width);
		xx = xx*scale_x + (double)m_mosaic_column*(double)(m_mosaic_surface_width+
			(m_mosaic_offset_x*(m_mosaic_num_col-1)))/m_mosaic_num_col - 
			(m_mosaic_offset_x * m_mosaic_column);

		double scale_y = (double)(m_mosaic_surface_height +(m_mosaic_offset_y*(m_mosaic_num_row-1)))/ 
			(m_mosaic_num_row * (double)m_actual_surface_height);
		yy = yy * scale_y + (double)m_mosaic_row*(double)(m_mosaic_surface_height+
			(m_mosaic_offset_y*(m_mosaic_num_row-1)))/m_mosaic_num_row - 
			(m_mosaic_offset_y * m_mosaic_row);
	}

   if(m_unrotated_width > 0)
      xx = xx -0.5*m_mosaic_surface_width;
	
	xx *= m_mosaic_meters_per_pixel_lon;

   yy = -yy;
   if(m_unrotated_height > 0)
      yy = yy + 0.5*m_mosaic_surface_height;
   yy *= m_mosaic_meters_per_pixel_lat;


   if (m_use_mercator_eqns) 
	{
      *lat = RAD_TO_DEG(PI/2 - 2*atan(exp(-yy/WGS84_a_METERS)));
      *lon = RAD_TO_DEG(xx/WGS84_a_METERS + DEG_TO_RAD(m_center_lon_for_calculations));
   }
   else 
	{
	   double theta;
	   double rho = sqrt(xx*xx + (m_param_rho_0 - yy)*(m_param_rho_0 - yy));
	   if(m_param_n < 0.0) 
		{
   		theta = atan2(-xx, yy-m_param_rho_0);
		   rho = -1.0*fabs(rho);
	   }
	   else 
		{
   		theta = atan2(xx, m_param_rho_0-yy);
		   rho = fabs(rho);
	   }

		// prevent division by zero
		if ((rho == 0.0) || (m_param_n == 0.0)) {
			ASSERT(0);
			return FAILURE;
		}
   	*lat = RAD_TO_DEG(2*atan(pow(WGS84_a_METERS*m_param_F/rho, 1/m_param_n))-0.5*PI);
   	*lon = RAD_TO_DEG(theta/m_param_n + DEG_TO_RAD(m_center_lon_for_calculations));
   }
	if (*lon < -180.0) 
		*lon += 360.0;
	else if(*lon > 180.0) 
		*lon -= 360.0;
	ASSERT(*lat > -90.0);
	ASSERT(*lat < 90.0);
	ASSERT(*lon > -180.0);
	ASSERT(*lon < 180.0);

	return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int LambertProj::xy_to_geo(int x, int y, degrees_t *lat, degrees_t *lon) const 
{
	int rslt;

	rslt = xy_to_geo((double) x, (double) y, lat, lon);
	return rslt;
}

// ***********************************************************************
// ***********************************************************************


//=======================
// LambertProj::geo_to_xy
//=======================

int LambertProj::geo_to_xy(degrees_t lat, degrees_t lon, double *x, double *y) const 
{
   // Make sure projector has been initialized correctly
	if (!m_projection_ready) 
	{
		*x = 0.0;
		*y = 0.0;
      ERR_report("Projector has not been properly initialized!");
		return FAILURE;
	}

   degrees_t delta_lon = lon - m_center_lon_for_calculations;
   if(delta_lon < -180.0) 
		delta_lon += 360.0;
   else if(delta_lon > 180.0) 
		delta_lon -= 360.0;

   // Convert geo coordinate to projected, non-rotated point
   double xx, yy;
   if(m_use_mercator_eqns) 
	{
      xx = WGS84_a_METERS*DEG_TO_RAD(delta_lon)/m_mosaic_meters_per_pixel_lon;
      yy = -WGS84_a_METERS*log(tan(PI/4 + DEG_TO_RAD(lat/2)))/m_mosaic_meters_per_pixel_lat;
   }
   else 
	{
      double rho;
      if((m_param_n > 0.0 && lat == 90.0) || (m_param_n < 0.0 && lat == -90.0))
         rho = 0.0;
      else 
		{
         double tan_value = tan(0.25*PI+0.5*DEG_TO_RAD(lat));
         double power = pow(tan_value, m_param_n);
         rho = WGS84_a_METERS * m_param_F / power;
      }
      double theta = m_param_n*DEG_TO_RAD(delta_lon);

      xx = rho*sin(theta)/m_mosaic_meters_per_pixel_lon;
      yy = (rho*cos(theta) - m_param_rho_0)/m_mosaic_meters_per_pixel_lat;
   }

	

   if(m_unrotated_width > 0)
		xx = xx + ((double)m_mosaic_surface_width*0.5);
   *x = xx;

   if(m_unrotated_height > 0)
		yy += ((double)m_mosaic_surface_height*0.5);

	*y = yy;

	// for mosaic map pages, we need to convert the given x, y of the page
	// to the corresponding x,y of the mosaic map
	if (m_is_mosaic_map_component)
	{
		double scale_x = (double)(m_mosaic_surface_width +(m_mosaic_offset_x*(m_mosaic_num_col-1)))/ 
			(m_mosaic_num_col * (double)m_actual_surface_width);

		xx = (xx - (double)m_mosaic_column*(double)(m_mosaic_surface_width+
			(m_mosaic_offset_x*(m_mosaic_num_col-1)))/m_mosaic_num_col + 
			(m_mosaic_offset_x * m_mosaic_column))/scale_x ;
		
		double scale_y = (double)(m_mosaic_surface_height +(m_mosaic_offset_y*(m_mosaic_num_row-1)))/ 
			(m_mosaic_num_row * (double)m_actual_surface_height);
		yy = (yy- (double)m_mosaic_row*(double)(m_mosaic_surface_height+
			(m_mosaic_offset_y*(m_mosaic_num_row-1)))/m_mosaic_num_row + 
			(m_mosaic_offset_y * m_mosaic_row))/ scale_y;

		*x = xx;
		*y = yy;
	}

	return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int LambertProj::geo_to_xy(degrees_t lat, degrees_t lon, int *x, int *y) const 
{
	int rslt;
	double xx, yy;

	rslt = geo_to_xy(lat, lon, &xx, &yy);

   if (xx >= 0.0)
      *x = (int)(xx + 0.5);
   else
      *x = (int)(xx - 0.5);

   if (yy >= 0.0)
      *y = (int)(yy + 0.5);
   else
      *y = (int)(yy - 0.5);

	return rslt;
}

// ***********************************************************************
// ***********************************************************************



//==================================
// LambertProj::calculate_map_bounds
//==================================

int LambertProj::calculate_map_bounds(const MapSource &source, 
   const MapScale &scale, const MapSeries &series, boolean_t printing,
   degrees_t center_lat, degrees_t center_lon, int zoom_percent,
   int unrotated_width, int unrotated_height,
   d_geo_t* map_ll, d_geo_t* map_ur) 
{
	if (!m_projection_ready) 
	{
      ERR_report("Projector has not been properly initialized!");
      return FAILURE;
   }

	if ( m_use_mercator_eqns )
	{
		degrees_t top, bottom, left, right;

		if (!m_projection_ready) {
			ERR_report("Projector has not been properly initialized!");
			return FAILURE;
		}

		ASSERT(unrotated_width > 0 && unrotated_height > 0);
		xy_to_geo(0,0,&top,&left);
		xy_to_geo(unrotated_width-1,unrotated_height-1,&bottom,&right);

		if(left <= -180.0)
			left += 360.0;
		if(right >= 180.0)
			right -= 360.0;

		map_ll->lat = bottom;
		map_ll->lon = left;
		map_ur->lat = top;
		map_ur->lon = right;
	}
	else
	{
		int x, y;

		degrees_t dummy, top, bottom, left, right, lat, lon;
		ASSERT(unrotated_width > 0 && unrotated_height > 0);
		if(center_lat >= 0.0) {
			// Check to see if north pole is visible.  If so, NUEAM
			// must extend to the pole and cover full 360deg of longitude.
			geo_to_xy(89.99, center_lon, &x, &y);
			if(x >= 0 && x < unrotated_width &&
				y >= 0 && y < unrotated_height)
			{
				left = center_lon - 179.99;
				if(left <= -180.0) 
					left += 360.0;
	
				right = center_lon + 179.99;
		      if(right >= 180.0) 
					right -= 360.0;
	
				top = 89.99;
				bottom = 89.99;

				xy_to_geo(0, 0, &lat, &lon);
				if(bottom > lat)
					bottom = lat;
				xy_to_geo(unrotated_width-1, 0, &lat, &lon);
				if(bottom > lat)
					bottom = lat;
				xy_to_geo(0, unrotated_height-1, &lat, &lon);
				if(bottom > lat)
					bottom = lat;
				xy_to_geo(unrotated_width-1, unrotated_height-1, &lat, &lon);
				if(bottom > lat)
					bottom = lat;
			}
			else if (m_is_mosaic_map_component)
			{
				xy_to_geo(-100,0,&dummy,&left);
				xy_to_geo(unrotated_width-1+100,0,&dummy,&right);
				xy_to_geo(unrotated_width/2,-100,&top,&dummy);
				xy_to_geo(0,unrotated_height-1+100,&bottom,&dummy);
			}
			else 
			{
				xy_to_geo(0,0,&dummy,&left);
				xy_to_geo(unrotated_width-1,0,&dummy,&right);
				xy_to_geo(unrotated_width/2,0,&top,&dummy);
				xy_to_geo(0,unrotated_height-1,&bottom,&dummy);
			}
		}
		else 
		{
			// Check to see if south pole is visible.  If so, NUEAM must extend
			// southward to the pole and cover full 360deg of longitude.
			geo_to_xy(-89.99, center_lon, &x, &y);
			if(x >= 0 && x < unrotated_width &&
				y >= 0 && y < unrotated_height)
			{
				left = center_lon - 179.99; 
				right = center_lon + 179.99; 

				bottom = -89.99;
				top = -89.99;

				if(left <= -180.0) left += 360.0;
				if(right >= 180.0) right -= 360.0;

				xy_to_geo(0, 0, &lat, &lon);
				if(top < lat)
					top = lat;
				xy_to_geo(unrotated_width-1, 0, &lat, &lon);
				if(top < lat)
					top = lat;
				xy_to_geo(0, unrotated_height-1, &lat, &lon);
				if(top < lat)
					top = lat;
				xy_to_geo(unrotated_width-1, unrotated_height-1, &lat, &lon);
				if(top < lat)
					top = lat;
			}
			else if (m_is_mosaic_map_component)
			{
				xy_to_geo(-100,unrotated_height-1,&dummy,&left);
				xy_to_geo(unrotated_width-1+100,unrotated_height-1,&dummy,&right);
				xy_to_geo(0,-100,&top,&dummy);
				xy_to_geo(unrotated_width/2,unrotated_height-1+100,&bottom,&dummy);
			}
			else 
			{
				xy_to_geo(0,unrotated_height-1,&dummy,&left);
				xy_to_geo(unrotated_width-1,unrotated_height-1,&dummy,&right);
				xy_to_geo(0,0,&top,&dummy);
				xy_to_geo(unrotated_width/2,unrotated_height-1,&bottom,&dummy);
			}
		}
	
		if(left <= -180.0)
			left += WORLD_DEG;

		if(right >= 180.0)
			right -= WORLD_DEG;

		map_ur->lat = top;
		map_ur->lon = right;
		map_ll->lat = bottom;
		map_ll->lon = left;
	}

	return SUCCESS;
}


//==================================
// LambertProj::calc_degrees_per_pixel
//==================================

int LambertProj::calc_degrees_per_pixel(const MapSource &source, 
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

		*unzoomed_deg_lon_per_pix = WORLD_DEG/(double)(surface_width);
		//*unzoomed_deg_lat_per_pix = *unzoomed_deg_lon_per_pix;
	   *unzoomed_deg_lat_per_pix = HALF_WORLD_DEG/(double)(surface_height);

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


//=======================================================
// LambertProj::initialize_projection_specific_parameters
//=======================================================

int LambertProj::initialize_projection_specific_parameters(const MapScale &scale,
	degrees_t center_lat, degrees_t center_lon, degrees_t zoomed_deg_lat_per_pix,
   degrees_t zoomed_deg_lon_per_pix, int screen_width, int screen_height,
	bool use_mosaic_map_parameters /*=false*/, degrees_t mosaic_std_parallel_1, 
	degrees_t mosaic_std_parallel_2, degrees_t mosaic_center_lat, 
	degrees_t mosaic_center_lon)
{
	// Calculate standard parallels - Standard parallel #1 is the northernmost, #2 is the southernmost
	if (use_mosaic_map_parameters)
	{
		m_std_parallel_1 = mosaic_std_parallel_1;
		m_std_parallel_2 = mosaic_std_parallel_2;
	}
   else 
	{
		if ( scale == WORLD )
		{
			m_std_parallel_1 = center_lat;
			
			if ( center_lat >= 0.0 )
				m_std_parallel_2 = 30.0;
			else
				m_std_parallel_2 = -30.0;
		}
		else
		{
			m_std_parallel_1 = center_lat + ((double)(screen_height*zoomed_deg_lat_per_pix))/3.0;
			m_std_parallel_2 = center_lat - ((double)(screen_height*zoomed_deg_lat_per_pix))/3.0;
		}
	}

   // Keep in mind that standard parallels should not be placed at poles!
   if (m_std_parallel_1 >= 90.0) 
	{
      m_std_parallel_1 = 89.99;

      m_std_parallel_2 = 2*center_lat - 90.0;
      if(m_std_parallel_2 > m_std_parallel_1) 
		{
         m_std_parallel_2 = m_std_parallel_1;
      }
   }

   if (m_std_parallel_2 <= -90.0) 
	{
      m_std_parallel_2 = -89.99;

      m_std_parallel_1 = 2*center_lat + 90.0;
      if(m_std_parallel_1 < m_std_parallel_2) {
         m_std_parallel_1 = m_std_parallel_2;
      }
   }

   // If the map is centered within one pixel of the Equator, use Mercator equations instead of the
   // standard LCC equations.  This prevents the Lambert equations from blowing up due to the fact
   // that m_param_n would be zero in this case.
   double limit = fabs(zoomed_deg_lat_per_pix);
   if (center_lat < limit && center_lat > -limit) 
	{
      m_use_mercator_eqns = TRUE;

		m_center_lon_for_calculations = center_lon;

      return SUCCESS;
   }

   m_use_mercator_eqns = FALSE;

   // Calculation of m_param_n
   if (m_std_parallel_1 == m_std_parallel_2)  // Equivalent to having only one standard parallel
		m_param_n = sin(DEG_TO_RAD(m_std_parallel_1));
   else {

      double tf = cos(DEG_TO_RAD(m_std_parallel_1));
      double tf2 = cos(DEG_TO_RAD(m_std_parallel_2));
      if (tf2 == 0.0) {  // Should only occur if m_std_parallel_2 is at a pole.  Therefore, standard
                         // parallels should not be placed at poles.
         ASSERT(FALSE);
         ERR_report("Error setting up params.");
         return FAILURE;
      }
      m_param_n = tf / tf2;
      m_param_n = log(m_param_n);

      tf = tan(0.25*PI+0.5*DEG_TO_RAD(m_std_parallel_1));
      if (tf == 0.0) {  // Should only occur if m_std_parallel_1 is at the South Pole (-90deg).
                        // Once again, don't place standard parallels at poles!
         ASSERT(FALSE);
         ERR_report("Error setting up params.");
         return FAILURE;
      }

      tf2 = tan(0.25*PI+0.5*DEG_TO_RAD(m_std_parallel_2));
      if (tf2 == 0.0) {  // DON'T PLACE STANDARD PARALLELS AT POLES!!!!
         ASSERT(FALSE);
         ERR_report("Error setting up params.");
         return FAILURE;
      }

      tf2 /= tf;
      tf2 = log(tf2);
      if (tf2 == 0.0) {  // Should only occur if m_std_parallel_1 == m_std_parallel_2.  This is checked
                         // and handled earlier on in this function, so this should never happen.
         ASSERT(FALSE);
         ERR_report("Error setting up params.");
         return FAILURE;
      }
      m_param_n /= tf2;
   }

   if (m_param_n == 0.0) { // The only case in which m_param_n would be zero is if the projection is centered on the
                           // equator (i.e. m_std_parallel_1 == -m_std_parallel_2).  In this case, Mercator projection
                           // equations should be used.  This is checked and handled earlier on in this function, so
                           // this should never happen.
      ASSERT(FALSE);
      ERR_report("Error setting up params.");
      return FAILURE;
   }

   m_param_F = cos(DEG_TO_RAD(m_std_parallel_1))*pow(tan(0.25*PI+0.5*DEG_TO_RAD(m_std_parallel_1)),m_param_n)/m_param_n;

	if (use_mosaic_map_parameters)
	{
		m_center_lon_for_calculations = mosaic_center_lon;
		m_param_rho_0 = WGS84_a_METERS*m_param_F/pow(tan(0.25*PI+0.5*DEG_TO_RAD(mosaic_center_lat)),m_param_n);
	}
	else
	{
		m_center_lon_for_calculations = center_lon;
		m_param_rho_0 = WGS84_a_METERS*m_param_F/pow(tan(0.25*PI+0.5*DEG_TO_RAD(center_lat)),m_param_n);
	}

   return SUCCESS;
}


//=============================
// LambertProj::validate_center
//=============================

int LambertProj::validate_center(const MapSource &source, const MapScale &scale,
   const MapSeries &series, boolean_t printing, degrees_t requested_center_lat,
   degrees_t requested_center_lon, int zoom_percent, int surface_width,
   int surface_height, int unrotated_width, int unrotated_height,
   degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix, 
   degrees_t zoomed_deg_lat_per_pix, degrees_t zoomed_deg_lon_per_pix, 
   degrees_t* actual_center_lat, degrees_t* actual_center_lon,
	degrees_t *new_unzoomed_deg_lat_per_pix, degrees_t *new_unzoomed_deg_lon_per_pix, 
	degrees_t* new_zoomed_deg_lat_per_pix, degrees_t* new_zoomed_deg_lon_per_pix)
{
   ASSERT(requested_center_lat >= -90.0 && requested_center_lat <= 90.0);

#if 1

	// constraint the projection to use Mercator equations when 
	// at World scale
	if ( scale == WORLD )
	{
		*actual_center_lat = 0.0;
	}
	else
	{
		// if either pole is visible on the map, adjust the center of 
		// projection to the closest pole
		degrees_t degrees_center_to_pole;
		double pixels_center_to_pole;

		// North pole adjustment
		if ( requested_center_lat >= 0.0 )
		{
			degrees_center_to_pole = 90.0 - requested_center_lat;
			if ( degrees_center_to_pole > 0.0 )
			{
				pixels_center_to_pole = degrees_center_to_pole / unzoomed_deg_lat_per_pix;
				if ( (int)(pixels_center_to_pole+0.5) < ( surface_height / 2 ) )
					*actual_center_lat = 89.99;			
				else
					*actual_center_lat = requested_center_lat;
			}
		}// South pole adjustment
		else
		{
			degrees_center_to_pole = 90.0 - fabs(requested_center_lat);
			if ( degrees_center_to_pole > 0.0 )
			{
				pixels_center_to_pole = degrees_center_to_pole / unzoomed_deg_lat_per_pix;
				if ( (int)(pixels_center_to_pole+0.5) < ( surface_height / 2 ) )
					*actual_center_lat = -89.99;			
				else
					*actual_center_lat = requested_center_lat;
			}
		}
	}

#else
	*actual_center_lat = requested_center_lat;
#endif
	
	*actual_center_lon = requested_center_lon;

   *new_unzoomed_deg_lat_per_pix = unzoomed_deg_lat_per_pix;
   *new_unzoomed_deg_lon_per_pix = unzoomed_deg_lon_per_pix;
   *new_zoomed_deg_lat_per_pix = zoomed_deg_lat_per_pix;
   *new_zoomed_deg_lon_per_pix = zoomed_deg_lon_per_pix;
   return SUCCESS;
}


//=================================
// LambertProj::get_projection_type
//=================================

ProjectionID LambertProj::get_projection_type() const {
   return ProjectionID(LAMBERT_PROJECTION);
}


//=============================
// LambertProj::get_convergence
//=============================

int LambertProj::get_convergence(d_geo_t position, degrees_t *convergence) {
   if(m_projection_ready != TRUE) {
      ERR_report("Projector has not been properly initialized!");
      return FALSE;
   }

	if(m_use_mercator_eqns) 
		*convergence = 0.0;
	else
	{	
		//degrees_t dlon = fabs(position.lon - m_center_lon);

		degrees_t dlon =  GEO_delta_lon(position.lon, m_center_lon_for_calculations);

		if (dlon > 180.0)
			dlon = 360 - dlon;

		degrees_t angle_of_conv = m_param_n*dlon;

		*convergence = angle_of_conv;
	}

   return TRUE;
}


//=================================
// LambertProj::calc_meters_per_pixel
//=================================

int LambertProj::calc_meters_per_pixel(const MapScale &scale, degrees_t center_lat, degrees_t center_lon, 
		double unzoomed_deg_per_pix_lat, double unzoomed_deg_per_pix_lon, 
		int zoom_percent, double *meters_per_pixel_lat, double *meters_per_pixel_lon,
		boolean_t is_printing)
{
	int status = FAILURE;

	if ( scale == WORLD && m_use_mercator_eqns )
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
		y = WGS84_a_METERS*log(tan(PI/4 + DEG_TO_RAD(MERCATOR_MAX_LAT/2.0)));
		m_p_p = fabs(y / static_cast<double>(m_actual_surface_height/2.0));

		*meters_per_pixel_lat = m_p_p;

		status = SUCCESS;
	}
	else
	{
		// compute the meters per pixel of latitude
		status = Projector::calc_meters_per_pixel(scale, center_lat, center_lon, 
				unzoomed_deg_per_pix_lat, unzoomed_deg_per_pix_lon, 
				zoom_percent, meters_per_pixel_lat, meters_per_pixel_lon, is_printing);
	}

	return status;
}

