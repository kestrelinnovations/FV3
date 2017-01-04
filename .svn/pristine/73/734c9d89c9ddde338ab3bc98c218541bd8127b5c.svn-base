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



#pragma once

#ifndef PROJ_H
#define PROJ_H 1

/*------------------------------------------------------------------
-                            Includes
- -----------------------------------------------------------------*/
#include "maps_d.h"  // for ProjectionID
#include "rect.h"

//========================
// Projection return codes
//========================

#define INVALID_POINT 100
//#define NONVISIBLE_RESULT 101  // now defined in MapRenderingEngine's type library

//================
// Projector class
//================

class Projector 
{

public:

   Projector();
   virtual ~Projector() {}

   //
   // must be successfully called before any other member functions 
   // can be called
   //
   int bind_map_to_surface(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t requested_center_lat, degrees_t requested_center_lon, 
      double rotation_angle, int zoom_percent,
      int surface_width, int surface_height, bool &geo_width_violation,
      bool use_mosaic_map_parameters = false, degrees_t mosaic_std_parallel_1 = 0, 
      degrees_t mosaic_std_parallel_2 = 0, 
      double mosaic_offset_x = 0,
      double mosaic_offset_y = 0, 
      double mosaic_meters_per_pixel_lat = 0, 
      double mosaic_meters_per_pixel_lon = 0,
      degrees_t m_mosaic_center_lat = 0, degrees_t mosaic_center_lon = 0,
      int mosaic_surface_width = 0, int mosaic_surface_height = 0,
      int mosaic_row = 0, int mosaic_column = 0,
      int mosaic_num_row = 0, int mosaic_num_col = 0);

   double get_meters_per_pixel_lat() { return m_meters_per_pix_lat; }
   double get_meters_per_pixel_lon() { return m_meters_per_pix_lon; }

   int get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const;
   int get_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
      degrees_t *degrees_lon_per_pixel) const;
   int get_unzoomed_degrees_per_pixel(degrees_t *unzoomed_degrees_lat_per_pixel,
      degrees_t *unzoomed_degrees_lon_per_pixel) const;

   int get_pixels_around_world(int *x_pixels_around_world) const;

   int get_center(d_geo_t* center) const;
   double get_rotation_angle(void) const {
      return m_surface.get_rotation();
   }

   void get_surface_size(int* surf_width, int* surf_height) const {
      *surf_width = surface_width();
      *surf_height = surface_height();
   }

   int surface_width(void) const {
      return actual_width();
   }

   int surface_height(void) const {
      return actual_height();
   }

   //
   // "virtual map" functions
   //
   void get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const;

   int vwidth(void) const {
      return m_virtual_surface_width;
   }

   int vheight(void) const {
      return m_virtual_surface_height;
   }


   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      int *x_coord, int *y_coord) const;
   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      double *x_coord, double *y_coord) const;
   void geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
      degrees_t ur_lat, degrees_t ur_lon,
      int *ul_x, int *ul_y, int *lr_x, int *lr_y) const;
   void vsurface_to_geo(int x_coord, int y_coord,
      degrees_t *latitude, degrees_t *longitude) const;
   void vsurface_to_geo(double x_coord, double y_coord,
      degrees_t *latitude, degrees_t *longitude) const;
   void vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
      degrees_t *ll_lat, degrees_t *ll_lon, 
      degrees_t *ur_lat, degrees_t *ur_lon) const;

   //
   // Convert a world coordinate to the surface coordinate of the closest
   // pixel center.
   //
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      int *surface_x, int *surface_y) const;
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      double *surface_x, double *surface_y) const;

   //
   // Convert a surface coordinate to the geographic location at the center
   // of that pixel.
   //
   int surface_to_geo(int surface_x, int surface_y,
      degrees_t *latitude, degrees_t *longitude) const;
   int surface_to_geo(double surface_x, double surface_y,
      degrees_t *latitude, degrees_t *longitude) const;

   int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const;
   int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const;
   int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const;
   int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const;

   boolean_t geo_in_surface(double lat, double lon) const;
   boolean_t geo_in_surface(double lat, double lon, int* s_x, int* s_y) const;
   boolean_t point_in_surface(int x, int y) const;

   virtual ProjectionID get_projection_type() const = 0;
   
   virtual int get_convergence(d_geo_t position, degrees_t *convergence) = 0;

   virtual int meridian_convergence_angle(d_geo_t geo_pos, degrees_t *convergence);

/*
#if EQUAL_ARC_DEBUG
   void print_map_param(void);
#endif
*/
public:

   virtual int geo_to_xy(degrees_t latitude, degrees_t longitude,
      int *x, int *y) const = 0;

   virtual int geo_to_xy(degrees_t latitude, degrees_t longitude,
      double *x, double *y) const = 0;

   virtual int xy_to_geo(int x, int y, degrees_t *latitude,
      degrees_t *longitude) const = 0;

   virtual int xy_to_geo(double x, double y, degrees_t *latitude,
      degrees_t *longitude) const = 0;


   //
   // the only reason calculate_map_bounds is static is just to make it
   // explicit that it uses no member vars or functions
   //

protected:

   virtual int calculate_map_bounds(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, int zoom_percent,
      int unrotated_width, int unrotated_height,
      d_geo_t* map_ll, d_geo_t* map_ur) = 0;

   virtual int validate_center(const MapSource &source, const MapScale &scale,
      const MapSeries &series, boolean_t printing, degrees_t requested_center_lat,
      degrees_t requested_center_lon, int zoom_percent, int surface_width,
      int surface_height, int unrotated_width, int unrotated_height,
      degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix, 
      degrees_t zoomed_deg_lat_per_pix, degrees_t zoomed_deg_lon_per_pix, 
      degrees_t* actual_center_lat, degrees_t* actual_center_lon, 
      degrees_t *new_unzoomed_lat_per_pix, degrees_t *new_unzoomed_lon_per_pix, 
      degrees_t* new_zoomed_deg_lat_per_pix, degrees_t* new_zoomed_deg_lon_per_pix) = 0;

    virtual int initialize_projection_specific_parameters(const MapScale &scale,
      degrees_t center_lat, degrees_t center_lon, degrees_t zoomed_deg_lat_per_pix,
      degrees_t zoomed_deg_lon_per_pix, int surface_width, int surface_height,
      bool use_mosaic_map_parameters =false, degrees_t mosaic_std_parallel_1 = 0, 
      degrees_t mosaic_std_parallel_2 = 0, degrees_t mosaic_center_lat = 0,
      degrees_t mosaic_center_lon = 0);

protected:

   rotatable_rectangle m_surface;
   int m_unrotated_width;
   int m_unrotated_height;

   int m_mosaic_surface_width;
   int m_mosaic_surface_height;
   int m_mosaic_row;
   int m_mosaic_column;

   //
   // "surface map" functions
   //
   void get_actual_surface_size(int* surface_width, int* surface_height) const;

   int actual_width(void) const {
      return m_actual_surface_width;
   }

   int actual_height(void) const {
      return m_actual_surface_height;
   }

   // The base class provides a Default implementation
   virtual int calc_meters_per_pixel(const MapScale &scale, degrees_t center_lat, degrees_t center_lon,
      double unzoomed_deg_per_pix_lat, double unzoomed_deg_per_pix_lon, 
      int zoom_percent, double *meters_per_pixel_lat, double *meters_per_pixel_lon,
      boolean_t is_printing);
   
   virtual int calc_degrees_per_pixel(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, 
      int zoom_percent, int surface_width, int surface_height,
      degrees_t* unzoomed_deg_lat_per_pix, degrees_t* unzoomed_deg_lon_per_pix,
      degrees_t* zoomed_deg_lat_per_pix, degrees_t* zoomed_deg_lon_per_pix);

   int proj_get_degrees_per_pixel(const MapSource &source, const MapScale &scale,
      const MapSeries &series, boolean_t printing,
      degrees_t latitude, int surface_width, int surface_height, 
      degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix);

   //
   // status of projection parameters
   //
   boolean_t m_projection_ready;

   //
   // the actual center lat used to compute the bounds (which may be different
   // from the requested center lat)
   //
   degrees_t m_center_lat;

   //
   // this is the same as requested center lon (that is, the center longitude
   // never gets adjusted)
   //
   degrees_t m_center_lon;


   // the center longitude used in the geo_to_xy and xy_to_geo calculations.  Could
   // be different then m_center_lon if a mosaic map is being used
   degrees_t m_center_lon_for_calculations;

   //
   // current map / display window boundaries
   //
   degrees_t m_top_lat;
   degrees_t m_right_lon;
   degrees_t m_bottom_lat;
   degrees_t m_left_lon; 

   //
   // geo-device / device-geo conversion constants
   //

   double m_meters_per_pix_lat;
   double m_meters_per_pix_lon;
   double m_mosaic_meters_per_pixel_lat;
   double m_mosaic_meters_per_pixel_lon;
   double m_mosaic_offset_x;
   double m_mosaic_offset_y;
   int m_mosaic_num_row;
   int m_mosaic_num_col;
   degrees_t m_pix_per_dem_lat;
   degrees_t m_pix_per_dem_lon;
   degrees_t m_dem_lat_per_pix;
   degrees_t m_dem_lon_per_pix;
   degrees_t m_unzoomed_dem_lat_per_pix;
   degrees_t m_unzoomed_dem_lon_per_pix;
   degrees_t m_IDL_x_coord;
   int m_pixels_around_world;
   int m_half_pixels_around_world;

   //
   // width of the "virtual" screen in to which the
   // map is drawn.  The virtual screen width may 
   // be larger than the actual screen size if 
   // the map needs to be rotated.
   //
   int m_virtual_surface_width;   

   //
   // height of the "virtual" screen in to which the
   // map is drawn.  The virtual screen height may 
   // be larger than the actual screen size if 
   // the map needs to be rotated.
   //
   int m_virtual_surface_height; 

   //
   // the dimensions of the "actual" (i.e. non-virtual) surface
   // in logical units.  That is, it is the view size or the page 
   // size in logical units.
   //
   int m_actual_surface_width;
   int m_actual_surface_height;

   bool m_is_mosaic_map_component;
};




//============================
// PROJ library function calls
//============================

int PROJ_create_projector(ProjectionEnum proj_type, Projector **new_proj);

void PROJ_destroy_projector(Projector *proj);

ProjectionEnum PROJ_get_default_projection_type(void);

void PROJ_get_available_projection_types(CArray<ProjectionID, const ProjectionID&> *pArray, BOOL Is3DEnabledInRegistry);

#endif