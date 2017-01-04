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
-  FILE NAME:    projectors.cpp
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

#define LN_BASE_e       2.71828182845904524

//==========
// Projector
//==========

Projector::Projector() 
{
   m_projection_ready = FALSE;
}


//===============================
// Projector::bind_map_to_surface
//===============================

int Projector::bind_map_to_surface(const MapSource &source, 
   const MapScale &scale, const MapSeries &series, boolean_t printing,
   degrees_t requested_center_lat, degrees_t requested_center_lon, 
   double rotation_angle, int zoom_percent,
   int surface_width, int surface_height, bool &geo_width_violation,
   bool use_mosaic_map_parameters /*=false*/, degrees_t mosaic_std_parallel_1, 
   degrees_t mosaic_std_parallel_2, double mosaic_offset_x,
   double mosaic_offset_y, double mosaic_meters_per_pixel_lat,
   double mosaic_meters_per_pixel_lon,
   degrees_t mosaic_center_lat, degrees_t mosaic_center_lon,
   int mosaic_surface_width, int mosaic_surface_height,
   int mosaic_row, int mosaic_column, int mosaic_num_row, int mosaic_num_col)
{ 
   m_projection_ready = FALSE;

   bool valid_surface;
   bool can_rotate;
   bool can_zoom;
   int minimum_zoom;

   // test surface size, rotation, and zoom for this scale
   if (MAP_test_limits(scale, surface_width, surface_height,valid_surface, 
      can_rotate, can_zoom, minimum_zoom) != SUCCESS)
   {
      ERR_report("MAP_test_limits() failed");
      return FAILURE;
   }

   // make sure surface size is valid
   if (!valid_surface)
   {
      ERR_report("surface size is too large for this scale.");
      return FAILURE;
   }

   // make sure rotation is valid
   if (!can_rotate && rotation_angle != 0.0)
   {
      ERR_report("invalid rotation");
      return FAILURE;
   }

   // make sure zoom percent is valid
   if (((!can_zoom && zoom_percent != 100) || zoom_percent < minimum_zoom) &&
      zoom_percent != NATIVE_ZOOM_PERCENT)
   {
      ERR_report("invalid zoom");
      return FAILURE;
   }

   // The graphics library uses a counter-clockwise angle to specify rotation,
   // rather than a clockwise angle like the map library uses, so convert the 
   // angle from clockwise to counter-clockwise.
   rotation_angle *= -1.0;
   if (rotation_angle < 0.0)
      rotation_angle += 360.0;
   
   int status = m_surface.set_rotation(rotation_angle, surface_width, surface_height);
   if (status != SUCCESS) 
   {
      ERR_report("set_rotation");
      return FAILURE;
   }
   m_actual_surface_width = surface_width;
   m_actual_surface_height = surface_height;
   m_unrotated_width = m_surface.get_vsurface_width();
   m_unrotated_height = m_surface.get_vsurface_height();

   if (use_mosaic_map_parameters)
   {
      m_mosaic_surface_width = mosaic_surface_width;
      m_mosaic_surface_height = mosaic_surface_height;
      m_is_mosaic_map_component = true;
   }
   else
   {
      m_mosaic_surface_width = m_unrotated_width;
      m_mosaic_surface_height = m_unrotated_height;
      m_is_mosaic_map_component = false;
   }
   m_mosaic_row = mosaic_row;
   m_mosaic_column = mosaic_column;


   // 
   // Get the degrees per pixel of the source data
   //
   degrees_t unzoomed_deg_lat_per_pix, unzoomed_deg_lon_per_pix;
   degrees_t zoomed_deg_lat_per_pix, zoomed_deg_lon_per_pix;
   calc_degrees_per_pixel(source, scale, series, printing, requested_center_lat,
      requested_center_lon, zoom_percent, m_unrotated_width, m_unrotated_height,
      &unzoomed_deg_lat_per_pix, &unzoomed_deg_lon_per_pix,
      &zoomed_deg_lat_per_pix, &zoomed_deg_lon_per_pix);

   //
   // Make sure the requested center latitude is valid for this projection type.
   // If not adjust it to an acceptable value, then get new deg/pix values.
   //
   degrees_t actual_center_lat;
   degrees_t actual_center_lon; // in some projections (with radial aspect (polar), the 
   // longitude might get changed
   degrees_t new_zoomed_deg_lat_per_pix, new_zoomed_deg_lon_per_pix;
   degrees_t new_unzoomed_deg_lat_per_pix, new_unzoomed_deg_lon_per_pix;
   if ( SUCCESS != validate_center(source, scale, series, printing, requested_center_lat,
      requested_center_lon, zoom_percent, m_actual_surface_width, m_actual_surface_height,
      m_unrotated_width, m_unrotated_height, unzoomed_deg_lat_per_pix,
      unzoomed_deg_lon_per_pix, zoomed_deg_lat_per_pix, zoomed_deg_lon_per_pix,
      &actual_center_lat, &actual_center_lon,
      &new_unzoomed_deg_lat_per_pix, &new_unzoomed_deg_lon_per_pix,
      &new_zoomed_deg_lat_per_pix, &new_zoomed_deg_lon_per_pix ))
   {
      CString msg;
      msg.Format( "invalid center(%s,%s,%s,%f,%f...)",source.get_string(), scale.get_string(), series.get_string(),requested_center_lat,requested_center_lon);
      ERR_report(msg);
      return FAILURE;
   }

   m_center_lat = actual_center_lat;
   m_center_lon = actual_center_lon;

   //
   // Store deg/pix and resulting pix/deg values
   //
   m_unzoomed_dem_lat_per_pix = new_unzoomed_deg_lat_per_pix;
   m_unzoomed_dem_lon_per_pix = new_unzoomed_deg_lon_per_pix;
   m_dem_lat_per_pix = new_zoomed_deg_lat_per_pix;
   m_pix_per_dem_lat = 1.0/m_dem_lat_per_pix;
   m_dem_lon_per_pix = new_zoomed_deg_lon_per_pix;
   m_pix_per_dem_lon = 1.0/m_dem_lon_per_pix;

   if ((m_pix_per_dem_lon * WORLD_DEG) > 4294967295.0) 
   {
      ERR_report("pixels_around_world is greater than maximum integer");
      return FAILURE;
   }

   m_pixels_around_world = (int)(m_pix_per_dem_lon * WORLD_DEG + 0.5);
   m_half_pixels_around_world = m_pixels_around_world / 2;

   //
   // Initialize any parameters specific to the particular projection
   //
   status = initialize_projection_specific_parameters(scale, 
      m_center_lat, m_center_lon, m_dem_lat_per_pix, m_dem_lon_per_pix, 
      m_actual_surface_width, m_actual_surface_height, use_mosaic_map_parameters,
      mosaic_std_parallel_1, mosaic_std_parallel_2, mosaic_center_lat,
      mosaic_center_lon);
   if(status != SUCCESS) 
   {
      ERR_report("FAILED: set_up_projection_specific_parameters");
      return FAILURE;
   }

   // NOTE: The mpp computation should be done before validate_center (or inside it)
   //
   // Calculate an appropriate meters-per-pixel value
   //
   double meters_per_pixel_lat, meters_per_pixel_lon;
   if (use_mosaic_map_parameters)
   {
      m_mosaic_meters_per_pixel_lat = mosaic_meters_per_pixel_lat;
      m_mosaic_meters_per_pixel_lon = mosaic_meters_per_pixel_lon;
      m_mosaic_offset_x = mosaic_offset_x;
      m_mosaic_offset_y = mosaic_offset_y;
      m_mosaic_num_row = mosaic_num_row;
      m_mosaic_num_col = mosaic_num_col;
   }

   status = calc_meters_per_pixel(scale, m_center_lat, m_center_lon, m_unzoomed_dem_lat_per_pix,
      m_unzoomed_dem_lon_per_pix, zoom_percent, &meters_per_pixel_lat, &meters_per_pixel_lon, printing);
   if(status != SUCCESS) 
   {
      ERR_report("FAILED: calc_meters_per_pixel");
      return FAILURE;
   }

   m_meters_per_pix_lat = meters_per_pixel_lat;
   m_meters_per_pix_lon = meters_per_pixel_lon;

   if (!use_mosaic_map_parameters)
   {
      m_mosaic_meters_per_pixel_lat = m_meters_per_pix_lat;
      m_mosaic_meters_per_pixel_lon = m_meters_per_pix_lon;
   }

   // enable all projection information functions
   m_projection_ready = TRUE;

   //
   // Determine the bounding latitudes and longitudes
   //
   d_geo_t map_ll, map_ur;
   status = calculate_map_bounds(source, scale, series, printing, m_center_lat, m_center_lon,
      zoom_percent, m_unrotated_width, m_unrotated_height, &map_ll, &map_ur);
   if(status != SUCCESS) 
   {
      ERR_report("FAILED: calc_map_bounds");
      return FAILURE;
   }

   // save new map bounds in static variables
   m_top_lat = map_ur.lat;
   m_right_lon = map_ur.lon;
   m_bottom_lat = map_ll.lat;
   m_left_lon = map_ll.lon;

   // save the virtual surface dimensions
   m_virtual_surface_height = (int) (((m_top_lat - m_bottom_lat) / m_dem_lat_per_pix) + 0.5) + 1;
   double temp = m_right_lon - m_left_lon;
   if(m_right_lon < m_left_lon)
      temp += 360;
   m_virtual_surface_width = (int) (( temp / m_dem_lon_per_pix) + 0.5) + 1;

   if (use_mosaic_map_parameters)
   {
      m_virtual_surface_width += 200;
      m_virtual_surface_height += 200;
   }

   // x coordinate at the International Date Line (-180.0)
   m_IDL_x_coord = m_pix_per_dem_lon * (HALF_WORLD_DEG - map_ll.lon); 

   return SUCCESS;
}

//===========================
// Projector::get_vmap_bounds
//===========================
int Projector::get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const 
{
   // if MAP_set_equal_arc_projection() was never successful then fail
   if (m_projection_ready == FALSE) 
   {
      ERR_report("MAP_set_equal_arc_projection() has not been called successfully.");
      return FAILURE;
   }

   map_ur->lat = m_top_lat;
   map_ur->lon = m_right_lon;
   map_ll->lat = m_bottom_lat;
   map_ll->lon = m_left_lon;

   return SUCCESS;
}


//=================================
// Projector::get_degrees_per_pixel
//=================================

int Projector::get_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
   degrees_t *degrees_lon_per_pixel) const
{
   // if the projection has't been set then fail
   if (m_projection_ready == FALSE)
   {
      ERR_report("bind_map_to_surface has not been called");
      return FAILURE;
   }

   *degrees_lat_per_pixel = m_dem_lat_per_pix;
   *degrees_lon_per_pixel = m_dem_lon_per_pix;

   return SUCCESS;
}


//===================================
// Projector::get_pixels_around_world
//===================================
int Projector::get_pixels_around_world(int *x_pixels_around_world) const 
{

   // if the projection has't been set then fail
   if (m_projection_ready == FALSE) 
   {
      ERR_report("bind_map_to_surface has not been called");
      return FAILURE;
   }

   *x_pixels_around_world = m_pixels_around_world;

   return SUCCESS;
}

//==========================================
// Projector::get_unzoomed_degrees_per_pixel
//==========================================
int Projector::get_unzoomed_degrees_per_pixel(degrees_t *unzoomed_degrees_lat_per_pixel,
   degrees_t *unzoomed_degrees_lon_per_pixel) const
{
   // if the projection has't been set then fail
   if (m_projection_ready == FALSE)
   {
      ERR_report("bind_map_to_surface has not been called");
      return FAILURE;
   }

   *unzoomed_degrees_lat_per_pixel = m_unzoomed_dem_lat_per_pix;
   *unzoomed_degrees_lon_per_pixel = m_unzoomed_dem_lon_per_pix;

   return SUCCESS;
}

//======================
// Projector::get_center
//======================
int Projector::get_center(d_geo_t* center) const 
{
   // if the projection has't been set then fail
   if (m_projection_ready == FALSE) 
   {
      ERR_report("set_equal_arc_projection has not been called");
      return FAILURE;
   }

   center->lat = m_center_lat;
   center->lon = m_center_lon;

   return SUCCESS;
}

//===================================
// Projector::get_actual_surface_size
//===================================
void Projector::get_actual_surface_size(int* width, int* height) const 
{
   *width = m_actual_surface_width;
   *height = m_actual_surface_height;
}

//=============================
// Projector::get_vsurface_size
//=============================
void Projector::get_vsurface_size(int* virtual_surface_width,
                                  int* virtual_surface_height) const
{
   *virtual_surface_width = m_virtual_surface_width;
   *virtual_surface_height = m_virtual_surface_height;
}

//================================
// Projector::vsurface_to_geo_rect
//================================
void Projector::vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
   degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const
{
   vsurface_to_geo(ul_x, ul_y, ur_lat, ll_lon);
   vsurface_to_geo(lr_x, lr_y, ll_lat, ur_lon);
}

//================================
// Projector::geo_to_vsurface_rect
//================================
void Projector::geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
   degrees_t ur_lat, degrees_t ur_lon, int *ul_x, int *ul_y, int *lr_x, int *lr_y) const
{
   geo_to_vsurface(ll_lat, ll_lon, ul_x, lr_y); 
   geo_to_vsurface(ur_lat, ur_lon, lr_x, ul_y); 

   // if half_pixels_around_world was between ul_x and lr_x
   if (*ul_x > *lr_x) 
   {
      if (*lr_x < 0)
         *lr_x += m_pixels_around_world;
      else
         *ul_x -= m_pixels_around_world;
   }
} 


// ***********************************************************************
// ***********************************************************************

//===============================
// Projector::vsurface_to_surface
//===============================

int Projector::vsurface_to_surface(int vs_x, int vs_y,
                                   int* s_x, int* s_y) const
{
   degrees_t lat, lon;

   vsurface_to_geo(vs_x, vs_y, &lat, &lon);

   return geo_to_surface(lat, lon, s_x, s_y);
}

// ***********************************************************************
// ***********************************************************************

int Projector::vsurface_to_surface(double vs_x, double vs_y,
                                   double* s_x, double* s_y) const
{
   degrees_t lat, lon;

   vsurface_to_geo(vs_x, vs_y, &lat, &lon);

   return geo_to_surface(lat, lon, s_x, s_y);
}


// ***********************************************************************
// ***********************************************************************

//===============================
// Projector::surface_to_vsurface
//===============================

int Projector::surface_to_vsurface(double s_x, double s_y,
                                   double* vs_x, double* vs_y) const
{
   int ret;
   degrees_t lat, lon;

   ret = surface_to_geo(s_x, s_y, &lat, &lon);
   if(ret != SUCCESS)
      return ret;

   geo_to_vsurface(lat, lon, vs_x, vs_y);

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int Projector::surface_to_vsurface(int s_x, int s_y,
                                   int* vs_x, int* vs_y) const
{
   int ret;
   degrees_t lat, lon;

   ret = surface_to_geo(s_x, s_y, &lat, &lon);
   if(ret != SUCCESS)
      return ret;

   geo_to_vsurface(lat, lon, vs_x, vs_y);

   return SUCCESS;
}


// ***********************************************************************
// ***********************************************************************

//==========================
// Projector::geo_to_surface
//==========================

int Projector::geo_to_surface(degrees_t latitude, degrees_t longitude,
   int *surface_x, int *surface_y) const
{
   int x, y;

   if (surface_x == NULL || surface_y == NULL)
   {
      ERR_report("Invalid parameter to Projector::geo_to_surface");
      return FAILURE;
   }

   // set surface_x, surface_y to reasonable defaults in case of failure
   *surface_x = -2147483647; 
   *surface_y = -2147483647;

   // Project the point
   int status = geo_to_xy(latitude, longitude, &x, &y);
   if(status != SUCCESS) 
   {
      return status;
   }

   // Rotate the point
   if (m_surface.vsurface_to_surface(x, y, surface_x, surface_y) != SUCCESS) 
   {
      ERR_report("vsurface_to_surface");
      return FAILURE;
   }

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int Projector::geo_to_surface(degrees_t latitude, degrees_t longitude,
   double *surface_x, double *surface_y) const
{
   double x, y;

   // Project the point
   int status = geo_to_xy(latitude, longitude, &x, &y);
   if (status != SUCCESS) 
   {
      return status;
   }

   // Rotate the point
   if (m_surface.vsurface_to_surface(x, y, surface_x, surface_y) != SUCCESS) 
   {
      ERR_report("vsurface_to_surface");
      return FAILURE;
   }

   return SUCCESS;
}


// ***********************************************************************
// ***********************************************************************

//==========================
// Projector::surface_to_geo
//==========================
int Projector::surface_to_geo(double surface_x, double surface_y,
   degrees_t *latitude, degrees_t *longitude) const
{
   double vs_x, vs_y;

   // Unrotate the point
   if (m_surface.surface_to_vsurface(surface_x, surface_y, &vs_x, &vs_y) != SUCCESS) 
   {
      ERR_report("surface_to_vsurface");
      return FAILURE;
   }

   // Unproject the point
   return xy_to_geo(vs_x, vs_y, latitude, longitude);
}

// ***********************************************************************
// ***********************************************************************

int Projector::surface_to_geo(int surface_x, int surface_y,
   degrees_t *latitude, degrees_t *longitude) const
{
   int vs_x, vs_y;

   // Unrotate the point
   if (m_surface.surface_to_vsurface(surface_x, surface_y, &vs_x, &vs_y) != SUCCESS) 
   {
      ERR_report("surface_to_vsurface");
      return FAILURE;
   }

   // Unproject the point
   return xy_to_geo(vs_x, vs_y, latitude, longitude);
}

// ***********************************************************************
// ***********************************************************************


//==========================
// Projector::geo_in_surface
//==========================

boolean_t Projector::geo_in_surface(double lat, double lon) const {
   int s_x;
   int s_y;

   return geo_in_surface(lat, lon, &s_x, &s_y);
}


//==========================
// Projector::geo_in_surface
//==========================

boolean_t Projector::geo_in_surface(double lat, double lon,
                                    int* s_x, int* s_y) const
{
   if (geo_to_surface(lat, lon, s_x, s_y) != SUCCESS)
      return FALSE;

   return point_in_surface(*s_x, *s_y);
}


//============================
// Projector::point_in_surface
//============================

boolean_t Projector::point_in_surface(int x, int y) const {

   double lat, lon;

   if(surface_to_geo(x, y, &lat, &lon) != SUCCESS)
      return FALSE;
      
   if(x < 0 || x >= m_surface.get_surface_width() || y < 0 || y >= m_surface.get_surface_height())
      return FALSE;

   return TRUE;
}

//===========================
// Projector::vsurface_to_geo
//===========================

void Projector::vsurface_to_geo(int x_coord, int y_coord,
   degrees_t *latitude, degrees_t *longitude) const
{
   // y projection is the same in any case
   *latitude = m_top_lat - (degrees_t)y_coord * m_dem_lat_per_pix;

   *longitude = (degrees_t)x_coord * m_dem_lon_per_pix + m_left_lon;
   if (*longitude > HALF_WORLD_DEG)
      *longitude -= WORLD_DEG;
   else if (*longitude < -HALF_WORLD_DEG)
      *longitude += WORLD_DEG;
}

// ***********************************************************************
// ***********************************************************************

void Projector::vsurface_to_geo(double x_coord, double y_coord,
   degrees_t *latitude, degrees_t *longitude) const
{
   // y projection is the same in any case
   *latitude = m_top_lat - (degrees_t)y_coord * m_dem_lat_per_pix;

   *longitude = (degrees_t)x_coord * m_dem_lon_per_pix + m_left_lon;
   if (*longitude > HALF_WORLD_DEG)
      *longitude -= WORLD_DEG;
   else if (*longitude < -HALF_WORLD_DEG)
      *longitude += WORLD_DEG;
}


// ***********************************************************************
// ***********************************************************************

//===========================
// Projector::geo_to_vsurface
//===========================

void Projector::geo_to_vsurface(degrees_t latitude, degrees_t longitude,
   double *x_coord, double *y_coord) const
{
   degrees_t x,y;

   // latitude projection is the same in any case
   y = (m_top_lat - latitude) * m_pix_per_dem_lat;

   if (longitude >= m_left_lon) 
      x = (longitude - m_left_lon) * m_pix_per_dem_lon;
   else
      x = ((longitude + HALF_WORLD_DEG) * m_pix_per_dem_lon) + m_IDL_x_coord;

   ASSERT(x >= 0.0);
   ASSERT(x < 2147483648.0);
   ASSERT(y < 2147483648.0);
   ASSERT(y > -2147483648.0);
   ASSERT(m_pixels_around_world  <= 2147483648);

   // convert exact y coordinate into a int
   *y_coord = y;

   // convert exact x cooordinate into a int, x is always >= 0
   *x_coord = x;
}

// ***********************************************************************
// ***********************************************************************

void Projector::geo_to_vsurface(degrees_t latitude, degrees_t longitude,
   int *x_coord, int *y_coord) const
{
   degrees_t x,y;

   geo_to_vsurface(latitude, longitude, &x, &y);


   // convert exact y coordinate into a int
   if (y >= 0.0)
      *y_coord = (int)(y + 0.5);
   else
      *y_coord = (int)(y - 0.5);

   // convert exact x cooordinate into a int, x is always >= 0
   *x_coord = (int)(x + 0.5);

   // keep *x_coord as close to the left edge of the map as posible
   if (*x_coord > m_half_pixels_around_world &&
      *x_coord >= m_virtual_surface_width )
      *x_coord -= m_pixels_around_world;
}

// ***********************************************************************
// ***********************************************************************


//=================================
// Projector::calc_meters_per_pixel
//=================================

int Projector::calc_meters_per_pixel(const MapScale &scale, 
   degrees_t center_lat, degrees_t center_lon,
   double unzoomed_deg_per_pix_lat,
   double unzoomed_deg_per_pix_lon,
   int zoom_percent, double *meters_per_pixel_lat,
   double *meters_per_pixel_lon, boolean_t is_printing)
{
   //
   // Calculate an appropriate meters-per-pixel value
   //
   double m_p_p, dummy;

   {
      double top_lat, bottom_lat;
      if(center_lat >= 0) 
      {
         top_lat = center_lat;
         bottom_lat = center_lat - unzoomed_deg_per_pix_lat;
      }
      else 
      {
         top_lat = center_lat + unzoomed_deg_per_pix_lat;
         bottom_lat = center_lat;
      }

      GEO_geo_to_distance(bottom_lat, center_lon, top_lat, 
         center_lon, &m_p_p, &dummy);

      // Adjust resolution to take zoom into account
      if (zoom_percent != 100) 
      {
         if (zoom_percent > 100) 
         {
            const double factor = ((double)zoom_percent/(double)100);
            m_p_p /= factor;
         }
         else if (zoom_percent < 100) 
         {
            const double factor = ((double)100/(double)zoom_percent);
            m_p_p *= factor;
         }
      }
      *meters_per_pixel_lat = m_p_p;
   }

   if (is_printing)
   {
      double new_lon = center_lon + unzoomed_deg_per_pix_lon;
      if (new_lon > 180.0)
         new_lon -= 360.0;

      if (center_lat > 89 || center_lat < -89)
      {
         *meters_per_pixel_lon = *meters_per_pixel_lat;
         return SUCCESS;
      }

      GEO_geo_to_distance(center_lat, center_lon, center_lat, new_lon, 
         &m_p_p, &dummy);

      // Adjust resolution to take zoom into account
      if (zoom_percent != 100) 
      {
         if (zoom_percent > 100) 
         {
            const double factor = ((double)zoom_percent/(double)100);
            m_p_p /= factor;
         }
         else if (zoom_percent < 100) 
         {
            const double factor = ((double)100/(double)zoom_percent);
            m_p_p *= factor;
         }
      }
      *meters_per_pixel_lon = m_p_p;
   }
   else
      *meters_per_pixel_lon = *meters_per_pixel_lat;

   return SUCCESS;
}


//==================================
// Projector::calc_degrees_per_pixel
//==================================

int Projector::proj_get_degrees_per_pixel(const MapSource &source, const MapScale &scale,
   const MapSeries &series, boolean_t printing,
   degrees_t latitude, int surface_width, int surface_height, 
   degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix)
{
   // WORLD is a special scale - dpp is adjusted to fill the surface
   if (scale == WORLD)
   {
      return MAP_get_degrees_per_pixel_world(surface_width, surface_height,
         *deg_lat_per_pix, *deg_lon_per_pix);
   }

   MapType map_type(source, scale, series);
   return map_type.get_degrees_per_pixel_printer(latitude, deg_lat_per_pix, deg_lon_per_pix);
}

int Projector::calc_degrees_per_pixel(const MapSource &source, 
   const MapScale &scale, const MapSeries &series, boolean_t printing,
   degrees_t center_lat, degrees_t center_lon, 
   int zoom_percent, int surface_width, int surface_height,
   degrees_t* unzoomed_deg_lat_per_pix, degrees_t* unzoomed_deg_lon_per_pix,
   degrees_t* zoomed_deg_lat_per_pix, degrees_t* zoomed_deg_lon_per_pix)
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

   //
   // calc the zoomed degrees/pixel values
   //
   calc_zoomed_degrees_per_pixel(*unzoomed_deg_lat_per_pix, zoom_percent,
      zoomed_deg_lat_per_pix);
   calc_zoomed_degrees_per_pixel(*unzoomed_deg_lon_per_pix, zoom_percent,
      zoomed_deg_lon_per_pix);

   return SUCCESS;
}


int Projector::meridian_convergence_angle(d_geo_t geo_pos, degrees_t *convergence)
{
   int status = FAILURE;

   status = get_convergence(geo_pos, convergence);

   return status;
}


//===========================================================
// Projector::initialize_projection_specific_parameters
//
// Some projections such as Lambert Conformal Conic will need
// to override this. Others such as Equal Arc might not.
//===========================================================

//
//
int Projector::initialize_projection_specific_parameters(const MapScale &scale,
   degrees_t center_lat, degrees_t center_lon, degrees_t zoomed_deg_lat_per_pix,
   degrees_t zoomed_deg_lon_per_pix, int surface_width, int surface_height,
   bool use_mosaic_map_parameters /*=false*/, degrees_t mosaic_std_parallel_1, 
   degrees_t mosaic_std_parallel_2, degrees_t mosaic_center_lat,
   degrees_t mosaic_center_lon)
{
   return SUCCESS;
}

//===================
// PROJ library calls
//===================

int PROJ_create_projector(ProjectionEnum proj_type, Projector **new_proj)
{
   if(proj_type == EQUALARC_PROJECTION)
      *new_proj = new EqualArcProj();
   else if(proj_type == LAMBERT_PROJECTION)
      *new_proj = new LambertProj();
   else if(proj_type == ORTHOGRAPHIC_PROJECTION)
      *new_proj = new OrthoProj();
   else if(proj_type == MERCATOR_PROJECTION)
      *new_proj = new MercatorProj();
   else
      *new_proj = NULL;

   if(*new_proj == NULL) {
      ERR_report("Failure creating projector!");
      return FAILURE;
   }

   return SUCCESS;
}

void PROJ_destroy_projector(Projector *proj) 
{
   delete proj;
}

ProjectionEnum PROJ_get_default_projection_type(void)
{
   return EQUALARC_PROJECTION;
}

void PROJ_get_available_projection_types(
   CArray<ProjectionID, const ProjectionID&> *pArray, BOOL Is3DEnabledInRegistry)
{
   pArray->Add(ProjectionID(EQUALARC_PROJECTION));
   pArray->Add(ProjectionID(LAMBERT_PROJECTION));
   pArray->Add(ProjectionID(MERCATOR_PROJECTION));
   pArray->Add(ProjectionID(ORTHOGRAPHIC_PROJECTION));
   pArray->Add(ProjectionID(AZIMUTHAL_EQUIDISTANT_PROJECTION));

   if (Is3DEnabledInRegistry)
      pArray->Add(ProjectionID(GENERAL_PERSPECTIVE_PROJECTION));
}

