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
-  FILE NAME:    ortho.cpp
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




//==================================
// Orthographic projection functions
//==================================


//=====================
// OrthoProj::geo_to_xy
//=====================

int OrthoProj::geo_to_xy(degrees_t lat, degrees_t lon, double *x, double *y) const 
{
	if (!m_projection_ready) 
	{
		*x = 0;
		*y = 0;
      ERR_report("Projector has not been properly initialized!");
		return FAILURE;
	}

   double lat_rad = DEG_TO_RAD(lat);
   double lon_rad = DEG_TO_RAD(lon);
   double center_lat_rad = DEG_TO_RAD(m_center_lat);

   double delta_lon_rad = lon - m_center_lon;
   if(delta_lon_rad < -180.0) 
		delta_lon_rad += 360.0;
   else if(delta_lon_rad > 180.0) 
		delta_lon_rad -= 360.0;
   delta_lon_rad = DEG_TO_RAD(delta_lon_rad);

   int status = SUCCESS;
   double cos_c = sin(center_lat_rad)*sin(lat_rad)
      + cos(center_lat_rad)*cos(lat_rad)*cos(delta_lon_rad);
   if(cos_c < 0.0)
      status = NONVISIBLE_RESULT;

   double xx = WGS84_a_METERS*cos(lat_rad)*sin(delta_lon_rad);
   xx /= m_meters_per_pix_lat;
   if(m_unrotated_width > 0)
      xx += ((double)m_unrotated_width)*0.5;
   *x = xx;

   double yy = -WGS84_a_METERS*(cos(center_lat_rad)*sin(lat_rad) - sin(center_lat_rad)*cos(lat_rad)*cos(delta_lon_rad));
   yy /= m_meters_per_pix_lat;
   if(m_unrotated_height > 0)
      yy += ((double)m_unrotated_height)*0.5;
   *y = yy;

	return status;
}


int OrthoProj::geo_to_xy(degrees_t lat, degrees_t lon, int *x, int *y) const 
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

//=====================
// OrthoProj::xy_to_geo
//=====================

int OrthoProj::xy_to_geo(double x, double y, degrees_t *lat, degrees_t *lon) const 
{
	int status = SUCCESS;

	double tf;

	if (!m_projection_ready) {
		*lat = 0.0;
		*lon = 0.0;
      ERR_report("Projector has not been properly initialized!");
		return FAILURE;
	}

   double xx = x;
   if(m_unrotated_width > 0)
      xx -= 0.5*m_unrotated_width;
   xx *= m_meters_per_pix_lat;

   double yy = y;
   if(m_unrotated_height > 0)
      yy -= 0.5*m_unrotated_height;
   yy *= -m_meters_per_pix_lat;

   double rho = sqrt(xx*xx + yy*yy);
   if (rho == 0.0) {
      *lat = m_center_lat;
      *lon = m_center_lon;
   }
   else {
      // Note that if rho is greater than WGS84_a_METERS, then
      // (x,y) does not fall on the Earth's surface.  This will
      // result in an error.
      double center_lat_rad = DEG_TO_RAD(m_center_lat);
      double center_lon_rad = DEG_TO_RAD(m_center_lon);
		tf = rho/WGS84_a_METERS;

      if (fabs(tf) > 1.0) 
		{  // Point does not lie on the Earth's surface
         //return INVALID_POINT;
			status = INVALID_POINT;
			tf = 1.0;
		}

      double c = asin(tf);
      *lat = RAD_TO_DEG(asin(cos(c)*sin(center_lat_rad) + (yy*sin(c)*cos(center_lat_rad)/rho)));

      tf = (rho*cos(center_lat_rad)*cos(c) - yy*sin(center_lat_rad)*sin(c));
		if (tf == 0.0) {
			ASSERT(0);
			return FAILURE;
		}

      *lon = RAD_TO_DEG(center_lon_rad + atan2(xx*sin(c), tf));
   }
	if(*lon < -180.0) 
		*lon += 360.0;
	else if(*lon > 180.0) 
		*lon -= 360.0;

	ASSERT(*lat >= -90.0);
	ASSERT(*lat <= 90.0);
	ASSERT(*lon > -180.0);
	ASSERT(*lon < 180.0);

	//return SUCCESS;
	return status;
}


// ***********************************************************************
// ***********************************************************************

int OrthoProj::xy_to_geo(int x, int y, degrees_t *lat, degrees_t *lon) const 
{
	int rslt;

	rslt = xy_to_geo((double) x, (double) y, lat, lon);
	return rslt;
}

// ***********************************************************************
// ***********************************************************************


//===========================
// OrthoProj::get_convergence
//===========================

int OrthoProj::get_convergence(d_geo_t position, degrees_t *convergence) 
{
   if(m_projection_ready != TRUE) 
	{
      ERR_report("Projector has not been properly initialized!");
      return FALSE;
   }

	double lat_rad = DEG_TO_RAD(position.lat);
	degrees_t dlon = GEO_delta_lon(position.lon, m_center_lon);
	double sinlat  = sin(lat_rad);

	// angle of convergence is a function (approx) of latitude
	degrees_t angle_of_conv = dlon*sinlat;

	*convergence = angle_of_conv;

   return SUCCESS;
}

//===============================
// OrthoProj::get_projection_type
//===============================
ProjectionID OrthoProj::get_projection_type() const 
{
   return ProjectionID(ORTHOGRAPHIC_PROJECTION);
}

//===========================
// OrthoProj::validate_center
//===========================
int OrthoProj::validate_center(const MapSource &source, const MapScale &scale,
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

   *actual_center_lat = requested_center_lat;
	*actual_center_lon = requested_center_lon;

   *new_unzoomed_deg_lat_per_pix = unzoomed_deg_lat_per_pix;
   *new_unzoomed_deg_lon_per_pix = unzoomed_deg_lon_per_pix;
   *new_zoomed_deg_lat_per_pix = zoomed_deg_lat_per_pix;
   *new_zoomed_deg_lon_per_pix = zoomed_deg_lon_per_pix;
   return SUCCESS;
}


//================================
// OrthoProj::calculate_map_bounds
//================================

int OrthoProj::calculate_map_bounds(const MapSource &source, 
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

   degrees_t dummy, top, bottom, left, right;
   int xx, yy;

   ASSERT(unrotated_width > 0 && unrotated_height > 0);

   if(center_lat >= 0.0) // Centered within the Northern Hemisphere
	{
      // Calculate the upper geo bound.  If the North Pole is visible then the upper limit
      // will obviously be 90.0 degrees.
      int status = geo_to_xy(89.99, center_lon, &xx, &yy);
      if(status == SUCCESS &&
         xx >= 0 && xx < unrotated_width &&
         yy >= 0 && yy < unrotated_height)
      {
         top = 89.99;
         
			left = center_lon - 180.0 + m_dem_lat_per_pix/2.0;
         if(left <= -180.0) left += 360.0;
         
			right = center_lon + 180.0 - m_dem_lat_per_pix/2.0;
         if(right >= 180.0) right -= 360.0;
      }
      else {
         xy_to_geo(unrotated_width/2,0,&top,&dummy);

         // Calculate the left geo bound.
         if(xy_to_geo(0,0,&dummy,&left) != SUCCESS) {
            left = center_lon - 179.99;
            if(left <= -180.0) left += 360.0;
         }

         // Calculate the right geo bound.
         if(xy_to_geo(unrotated_width-1,0,&dummy,&right) != SUCCESS) {
            right = center_lon + 179.99;
            if(right >= 180.0) right -= 360.0;
         }

      }

      // Calculate the bottom geo bound.
      if(xy_to_geo(0,unrotated_height-1,&bottom,&dummy) != SUCCESS) 
		{
			bottom = center_lat - 90.0;
      }
   }
   else 
	{  // Centered within the Southern Hemisphere

      // Calculate the upper geo bound.  If the North Pole is visible then the upper limit
      // will obviously be 90.0 degrees.
      int status = geo_to_xy(-89.99, center_lon, &xx, &yy);
      if(status == SUCCESS &&
         xx >= 0 && xx < unrotated_width &&
         yy >= 0 && yy < unrotated_height)
      {
         bottom = -89.99;

			left = center_lon - 180.0 + m_dem_lat_per_pix/2.0;
         if(left <= -180.0) left += 360.0;
         
			right = center_lon + 180.0 - m_dem_lat_per_pix/2.0;
         if(right >= 180.0) right -= 360.0;
      }
      else 
		{
         xy_to_geo(unrotated_width/2,unrotated_height-1,&bottom,&dummy);

         // Calculate the left geo bound.
         if(xy_to_geo(0,unrotated_height-1,&dummy,&left) != SUCCESS) 
			{
            left = center_lon - 179.99;
            if(left <= -180.0) left += 360.0;
         }

         // Calculate the right geo bound.
         if(xy_to_geo(unrotated_width-1,unrotated_height-1,&dummy,&right) != SUCCESS) 
			{
            right = center_lon + 179.99;
            if(right >= 180.0) right -= 360.0;
         }

      }

      // Calculate the top geo bound.
      if(xy_to_geo(0,0,&top,&dummy) != SUCCESS) 
		{
         top = center_lat + 90.0;
      }
   }

   if(right >= 180.0)
      right -= WORLD_DEG;
   if(left <= -180.0)
      left += WORLD_DEG;

   map_ur->lat = top;
   map_ur->lon = right;
   map_ll->lat = bottom;
   map_ll->lon = left;

	return SUCCESS;
}
