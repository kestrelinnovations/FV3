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

#ifndef PROJECTORS_H
#define PROJECTORS_H

#include "FalconView/include/common.h"
#include "FalconView/include/proj.h"
#include "FalconView/include/maps.h"
#include "FalconView/include/mapx.h"

#define MAX_MAP_WIDTH_DEG (degrees_t)360.0
#define MAX_MAP_HEIGHT_DEG (degrees_t)180.0

//========================
// Static helper functions
//========================

static int test_map_bounds(d_geo_t map_ll, d_geo_t map_ur) {
   degrees_t geo_width, geo_height;

   /* test for values with in range for Lat-Longs in degrees */
   if ((MIN_LAT_DEG > map_ur.lat) || (map_ur.lat > MAX_LAT_DEG))
      return -1;

   if ((MIN_LON_DEG > map_ur.lon) || (map_ur.lon > MAX_LON_DEG))
      return -2;

   if ((MIN_LAT_DEG > map_ll.lat) || (map_ll.lat > MAX_LAT_DEG))
      return -3;

   if ((MIN_LON_DEG > map_ll.lon) || (map_ll.lon > MAX_LON_DEG))
      return -4;

   /* calculate geographic height and width of the map bounds */
   geo_height = map_ur.lat - map_ll.lat;
   geo_width = map_ur.lon - map_ll.lon;
   if (geo_width <= 0.0)
      geo_width += WORLD_DEG;

   /* test for valid window height */
   if ((geo_height < 0.0) || (geo_height > MAX_MAP_HEIGHT_DEG))
      return -5;

   /* test for valid window width */
   if ((geo_width < 0.0) || (geo_width > MAX_MAP_WIDTH_DEG))
      return -6;

   return 0;
}

static void calc_zoomed_degrees_per_pixel(degrees_t unzoomed_degrees_per_pixel, 
   int zoom_percent, degrees_t* zoomed_degrees_per_pixel)
{
   if (zoom_percent == 100)
      *zoomed_degrees_per_pixel = unzoomed_degrees_per_pixel;
   else if (zoom_percent > 100)
   {
      const double factor = ((double)zoom_percent/100.0);
      *zoomed_degrees_per_pixel = unzoomed_degrees_per_pixel/factor;
   }
   else if (zoom_percent < 100)
   {
      const double factor = (100.0/(double)zoom_percent);
      *zoomed_degrees_per_pixel = unzoomed_degrees_per_pixel * factor;
   }
}


static int proj_get_degrees_per_pixel(const MapSource &source, const MapScale &scale,
   const MapSeries &series, boolean_t printing,
   degrees_t latitude, int surface_width, int surface_height, 
   degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix)
{
   // the MapType will not be used for WORLD scale, but lets makes sure that
   // source, scale, and series are valid any way
   MapType *map_type = MAP_get_map_type(source, scale, series);
   if (map_type == NULL)
      return FAILURE;

   // WORLD is a special scale - dpp is adjusted to fill the surface
   if (scale == WORLD)
   {
      return MAP_get_degrees_per_pixel_world(surface_width, surface_height,
         *deg_lat_per_pix, *deg_lon_per_pix);
   }

   int status;
   if (printing)
      status = map_type->get_degrees_per_pixel_printer(latitude, 
         deg_lat_per_pix, deg_lon_per_pix);
   else
      status = map_type->get_degrees_per_pixel_monitor(latitude,
         deg_lat_per_pix, deg_lon_per_pix);

#ifdef _DEBUG
   if (status == SUCCESS)
   {
      ASSERT(*deg_lat_per_pix != 0.0 && *deg_lon_per_pix != 0.0);
   }
#endif

   return status;
}


// Min/Max latitudes for Mercator and Lambert centered at equator.
#define MERCATOR_MAX_LAT  80.0

//==============================
// LambertProj class declaration
//==============================

class LambertProj : public Projector {
public:
   LambertProj() {};
   virtual ~LambertProj() {};

   int geo_to_xy(degrees_t lat, degrees_t lon, int *x, int *y) const;
   int geo_to_xy(degrees_t lat, degrees_t lon, double *x, double *y) const;
   int xy_to_geo(int x, int y, degrees_t *lat, degrees_t *lon) const;
   int xy_to_geo(double x, double y, degrees_t *lat, degrees_t *lon) const;
   virtual int get_convergence(d_geo_t position, degrees_t *convergence);
   virtual ProjectionID get_projection_type() const;

protected:

   virtual int validate_center(const MapSource &source, const MapScale &scale,
      const MapSeries &series, boolean_t printing, degrees_t requested_center_lat,
      degrees_t requested_center_lon, int zoom_percent, int surface_width,
      int surface_height, int unrotated_width, int unrotated_height,
      degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix, 
      degrees_t zoomed_deg_lat_per_pix, degrees_t zoomed_deg_lon_per_pix, 
      degrees_t* actual_center_lat, degrees_t* actual_center_lon,
      degrees_t *new_unzoomed_deg_lat_per_pix, degrees_t *new_unzoomed_deg_lon_per_pix, 
      degrees_t* new_zoomed_deg_lat_per_pix, degrees_t* new_zoomed_deg_lon_per_pix);

   virtual int initialize_projection_specific_parameters(const MapScale &scale,
      degrees_t center_lat, degrees_t center_lon, degrees_t zoomed_deg_lat_per_pix,
      degrees_t zoomed_deg_lon_per_pix, int screen_width, int screen_height,
      bool use_mosaic_map_parameters =false, degrees_t mosaic_std_parallel_1 = 0, 
      degrees_t mosaic_std_parallel_2 = 0, degrees_t mosaic_center_lat = 0,
      degrees_t mosaic_center_lon = 0);

   virtual int calculate_map_bounds(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, int zoom_percent,
      int unrotated_width, int unrotated_height,
      d_geo_t* map_ll, d_geo_t* map_ur);

   virtual int calc_degrees_per_pixel(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, 
      int zoom_percent, int surface_width, int surface_height,
      degrees_t* unzoomed_deg_lat_per_pix, degrees_t* unzoomed_deg_lon_per_pix,
      degrees_t* zoomed_deg_lat_per_pix, degrees_t* zoomed_deg_lon_per_pix);

   virtual int calc_meters_per_pixel(const MapScale &scale, degrees_t center_lat, degrees_t center_lon, 
      double unzoomed_deg_per_pix_lat, double unzoomed_deg_per_pix_lon, 
      int zoom_percent, double *meters_per_pixel_lat, double *meters_per_pixel_lon,
      boolean_t is_printing);

   // Lambert-specific projection parameters
   degrees_t m_std_parallel_1;
   degrees_t m_std_parallel_2;

   double m_param_n;
   double m_param_F;
   double m_param_rho_0;

   // Mercator equations parameters
   BOOL m_use_mercator_eqns;

public:
   degrees_t get_std_parallel_1() { return m_std_parallel_1; }
   degrees_t get_std_parallel_2() { return m_std_parallel_2; }
};


class MercatorProj : public Projector {
public:
   MercatorProj() {};
   virtual ~MercatorProj() {};

   int geo_to_xy(degrees_t lat, degrees_t lon, int *x, int *y) const;
   int geo_to_xy(degrees_t lat, degrees_t lon, double *x, double *y) const;
   int xy_to_geo(int x, int y, degrees_t *lat, degrees_t *lon) const;
   int xy_to_geo(double x, double y, degrees_t *lat, degrees_t *lon) const;
   virtual int get_convergence(d_geo_t position, degrees_t *convergence);
   virtual ProjectionID get_projection_type() const;

protected:

   virtual int validate_center(const MapSource &source, const MapScale &scale,
      const MapSeries &series, boolean_t printing, degrees_t requested_center_lat,
      degrees_t requested_center_lon, int zoom_percent, int surface_width,
      int surface_height, int unrotated_width, int unrotated_height,
      degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix, 
      degrees_t zoomed_deg_lat_per_pix, degrees_t zoomed_deg_lon_per_pix, 
      degrees_t* actual_center_lat, degrees_t* actual_center_lon,
      degrees_t *new_unzoomed_deg_lat_per_pix, degrees_t *new_unzoomed_deg_lon_per_pix, 
      degrees_t* new_zoomed_deg_lat_per_pix, degrees_t* new_zoomed_deg_lon_per_pix);

   virtual int calculate_map_bounds(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, int zoom_percent,
      int unrotated_width, int unrotated_height,
      d_geo_t* map_ll, d_geo_t* map_ur);

   virtual int calc_degrees_per_pixel(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, 
      int zoom_percent, int surface_width, int surface_height,
      degrees_t* unzoomed_deg_lat_per_pix, degrees_t* unzoomed_deg_lon_per_pix,
      degrees_t* zoomed_deg_lat_per_pix, degrees_t* zoomed_deg_lon_per_pix);

   virtual int calc_meters_per_pixel(const MapScale &scale, degrees_t center_lat, degrees_t center_lon, 
      double unzoomed_deg_per_pix_lat, double unzoomed_deg_per_pix_lon, 
      int zoom_percent, double *meters_per_pixel_lat, double *meters_per_pixel_lon,
      boolean_t is_printing);

   virtual int initialize_projection_specific_parameters(const MapScale &scale,
      degrees_t center_lat, degrees_t center_lon, degrees_t zoomed_deg_lat_per_pix,
      degrees_t zoomed_deg_lon_per_pix, int surface_width, int surface_height,
      bool use_mosaic_map_parameters =false, degrees_t mosaic_std_parallel_1 = 0, 
      degrees_t mosaic_std_parallel_2 = 0, degrees_t mosaic_center_lat = 0,
      degrees_t mosaic_center_lon = 0);

   // Mercator-specific projection parameters
   degrees_t m_std_parallel;
};


class OrthoProj : public Projector {
public:
   OrthoProj() {};
   virtual ~OrthoProj() {};

   int geo_to_xy(degrees_t lat, degrees_t lon, int *x, int *y) const;
   int geo_to_xy(degrees_t lat, degrees_t lon, double *x, double *y) const;
   int xy_to_geo(int x, int y, degrees_t *lat, degrees_t *lon) const;
   int xy_to_geo(double x, double y, degrees_t *lat, degrees_t *lon) const;
   virtual int get_convergence(d_geo_t position, degrees_t *convergence);
   virtual ProjectionID get_projection_type() const;

protected:

   virtual int validate_center(const MapSource &source, const MapScale &scale,
      const MapSeries &series, boolean_t printing, degrees_t requested_center_lat,
      degrees_t requested_center_lon, int zoom_percent, int surface_width,
      int surface_height, int unrotated_width, int unrotated_height,
      degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix, 
      degrees_t zoomed_deg_lat_per_pix, degrees_t zoomed_deg_lon_per_pix, 
      degrees_t* actual_center_lat, degrees_t* actual_center_lon,
      degrees_t *new_unzoomed_deg_lat_per_pix, degrees_t *new_unzoomed_deg_lon_per_pix, 
      degrees_t* new_zoomed_deg_lat_per_pix, degrees_t* new_zoomed_deg_lon_per_pix);

   virtual int calculate_map_bounds(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, int zoom_percent,
      int unrotated_width, int unrotated_height,
      d_geo_t* map_ll, d_geo_t* map_ur);

};


class EqualArcProj : public Projector {
public:
   EqualArcProj() {};
   virtual ~EqualArcProj() {};

   int geo_to_xy(degrees_t lat, degrees_t lon, int *x, int *y) const;
   int geo_to_xy(degrees_t lat, degrees_t lon, double *x, double *y) const;
   int xy_to_geo(int x, int y, degrees_t *lat, degrees_t *lon) const;
   int xy_to_geo(double x, double y, degrees_t *lat, degrees_t *lon) const;
   virtual int get_convergence(d_geo_t position, degrees_t *convergence);
   virtual ProjectionID get_projection_type() const;

protected:
   virtual int validate_center(const MapSource &source, const MapScale &scale,
      const MapSeries &series, boolean_t printing, degrees_t requested_center_lat,
      degrees_t requested_center_lon, int zoom_percent, int surface_width,
      int surface_height, int unrotated_width, int unrotated_height,
      degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix, 
      degrees_t zoomed_deg_lat_per_pix, degrees_t zoomed_deg_lon_per_pix, 
      degrees_t* actual_center_lat, degrees_t* actual_center_lon,
      degrees_t *new_unzoomed_deg_lat_per_pix, degrees_t *new_unzoomed_deg_lon_per_pix, 
      degrees_t* new_zoomed_deg_lat_per_pix, degrees_t* new_zoomed_deg_lon_per_pix);

   virtual int calculate_map_bounds(const MapSource &source, 
      const MapScale &scale, const MapSeries &series, boolean_t printing,
      degrees_t center_lat, degrees_t center_lon, int zoom_percent,
      int unrotated_width, int unrotated_height,
      d_geo_t* map_ll, d_geo_t* map_ur);

};

#endif
