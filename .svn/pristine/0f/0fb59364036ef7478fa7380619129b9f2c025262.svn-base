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

#ifndef MAPX_H
#define MAPX_H

#include "common.h"
#include "map.h" 
#include "geo_tool/geo_tool_d.h"
#include "geo_tool/geo_tool.h" // for GEO_east_of_degrees()
#include "proj.h" // For Projector

#include "err.h"    // for ERR_report
#include "param.h"

// forward declarations
class MapView;
class CMapCache;

#import "MapRenderingEngine.tlb" no_namespace named_guids

class SettableMapProj : public MapProj
{
public:
   SettableMapProj(void) {}

   //
   // marks the spec and the projection as uninitialized (but doesn't
   // neccessarily clean out the values)
   //
   virtual int clear(void) = 0;

   //
   // Set the spec BEFORE calling bind_equal_arc_map_to_surface.
   //
   virtual int set_spec(const MapSpec& spec) = 0;
   //
   // Set the surface size BEFORE calling bind_equal_arc_map_to_surface.
   //
   virtual int set_surface_size(int surface_width, int surface_height) = 0;

   virtual int bind_equal_arc_map_to_surface(void) = 0;
   virtual int bind_equal_arc_map_to_surface_using_geo_bounds(degrees_t northern_lat, 
      degrees_t southern_lat, degrees_t western_lon, degrees_t eastern_lon) = 0;
   virtual int bind_equal_arc_map_to_surface_using_geo_bounds(d_geo_t ul, 
      d_geo_t ur, d_geo_t ll, d_geo_t lr) = 0;
   virtual int bind_equal_arc_map_to_scale_surface_using_geo_bounds(degrees_t northern_lat, 
      degrees_t southern_lat, degrees_t western_lon, degrees_t eastern_lon,
      double surface_width_in_inches, double surface_height_in_inches,
      int surface_scale_percent) = 0;
   virtual int bind_equal_arc_map_to_scale_surface(double surface_width_in_inches,
      double surface_height_in_inches, int surface_scale_percent, 
      bool calc_surface_logical_units = true,
      int surface_width = 0, int surface_height = 0) = 0;
   virtual int bind_equal_arc_map_to_scale_mosaic_component_surface(
      degrees_t northern_lat, degrees_t southern_lat, 
      degrees_t western_lon, degrees_t eastern_lon, 
      double component_surface_width_in_inches, double component_surface_height_in_inches,
      int surface_scale_percent, const MapProj* mosaic_map, 
      int component_map_row, int component_map_col,
      double component_map_vertical_offset_in_inches,
      double component_map_horizontal_offset_in_inches) = 0;
   virtual int bind_equal_arc_map_to_scale_mosaic_component_surface_lambert(
      d_geo_t ul, d_geo_t ur, d_geo_t ll, d_geo_t lr,
      double component_surface_width_in_inches, double component_surface_height_in_inches,
      int surface_scale_percent, const MapProj* mosaic_map, 
      int component_map_row, int component_map_col,
      double component_map_vertical_offset_in_inches,
      double component_map_horizontal_offset_in_inches) = 0;

   //
   // These function return SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
   //
   virtual int bind_equal_arc_map_to_surface_and_check_data(void) = 0;
   virtual int bind_equal_arc_map_to_scale_surface_and_check_data(double surface_width_in_inches,
      double surface_height_in_inches, int surface_scale_percent) = 0;

   //
   // Does a data check after projection has been set.
   //
   // Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
   //
   virtual int data_check_without_setting_projection(void) = 0;

   virtual bool has_invalid_points(void) const = 0;

   virtual double get_meters_per_pixel_lat() = 0;
   virtual double get_meters_per_pixel_lon() = 0;

   ISettableMapProjPtr m_map;
   ISettableMapProj2Ptr m_map2;
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// MapProjWithWriteableChartInfoImplBase defines a few public methods 
// used to set the mosaic map information in a MapProj.
//
class MapProjWithWriteableChartInfoImplBase : public MapProj
{

public:

   MapProjWithWriteableChartInfoImplBase() {}
   virtual ~MapProjWithWriteableChartInfoImplBase() {}

   virtual void set_is_mosaic_map_component(boolean_t is_mosaic_map_component) = 0;
   virtual void set_mosaic_map_ptr(const MapProj* mosaic_map) = 0;
   virtual void set_component_map_row(int row) = 0;
   virtual void set_component_map_column(int col) = 0;
   virtual void clear_mosaic_map_info(void) = 0;
   virtual void set_component_map_row_offset_in_mosaic_map_pixmap(
      double component_map_row_offset_in_mosaic_map_pixmap) = 0;
   virtual void set_component_map_col_offset_in_mosaic_map_pixmap(
      double component_map_col_offset_in_mosaic_map_pixmap) = 0;
   virtual void set_mosaic_to_component_pixel_lat_scale_ratio(
      double mosaic_to_component_pixel_lat_scale_ratio) = 0;
   virtual void set_mosaic_to_component_pixel_lon_scale_ratio(
      double mosaic_to_component_pixel_lon_scale_ratio) = 0;
   virtual void set_component_map_vertical_offset_in_inches(
      double component_map_vertical_offset_in_inches) = 0;
   virtual void set_component_map_horizontal_offset_in_inches(
      double component_map_horizontal_offset_in_inches) = 0;
};

class SettableMapProjImpl;
class MapProjImpl;

class MapProjWithWriteableChartInfoImpl : public MapProjWithWriteableChartInfoImplBase
{

   //
   // Note: It is desirable to separate SettableMapProjImpl from 
   // MapProjWithWriteableChartInfoImpl (that is, to not derive
   // SettableMapProjImpl from MapProjWithWriteableChartInfoImpl), but to at the
   // same time keep MapProjWithWriteableChartInfoImpl implementation details 
   // hidden from children. 
   //
   // Making SettableMapProjImpl a friend of MapProjWithWriteableChartInfoImpl 
   // solves this problem.
   //
   friend SettableMapProjImpl;
   friend MapProjImpl;

public:

   MapProjWithWriteableChartInfoImpl() 
   {
      m_spec_initialized = FALSE;

      m_proj = NULL;
      m_proj_set = FALSE;

      m_surface_dims_set = FALSE;

      m_is_to_scale_projection = FALSE;
      m_to_scale_surface_width_in_inches = 0.0;
      m_to_scale_surface_height_in_inches = 0.0;
      m_surface_scale_percent = 0;
      m_surface_scale_denom = 0.0;

      clear_mosaic_map_info();
   }
   virtual ~MapProjWithWriteableChartInfoImpl()
   {
      clear_mosaic_map_info();

      if(m_proj != NULL) {
         PROJ_destroy_projector(m_proj);
         m_proj = NULL;
      }
   }

   virtual boolean_t is_spec_set(void) const
   {
      return m_spec_initialized;
   }
   virtual boolean_t is_surface_set(void) const
   {
      return m_surface_dims_set;
   }
   virtual boolean_t is_projection_set(void) const
   {
      return m_proj_set;
   }

   //
   // map spec functions
   //
   d_geo_t requested_center(void) const 
   { return spec()->center; }
   MapSource source(void) const 
   { return spec()->source; }
   MapSource actual_source(void) const
   { return spec()->source; }

   // RMP - 12/17/99
   ProjectionEnum projection_type(void) const
   { return m_actual_projection; }

   ProjectionEnum actual_projection(void) const
   { return m_actual_projection; }

   ProjectionEnum requested_projection(void) const
   { return spec()->projection_type; }


   MapScale scale(void) const 
   { return spec()->scale; }
   MapSeries series(void) const 
   { return spec()->series; }
   double requested_rotation(void) const 
   { return spec()->rotation; }
   int requested_zoom_percent(void) const 
   { return spec()->zoom_percent; }
   degrees_t requested_center_lat(void) const 
   { return spec()->center_lat(); }
   degrees_t requested_center_lon(void) const 
   { return spec()->center_lon(); }
   const MapSpec* spec(void) const 
   { ASSERT(is_spec_set()); return &m_spec; }
   virtual double actual_rotation(void) const
   { ASSERT(m_proj_set); return m_actual_rotation; }
   virtual int actual_zoom_percent(void) const
   { ASSERT(m_proj_set); return m_actual_zoom_percent; }

   // 
   // surface functions
   //
   int get_surface_width(void) const 
   { ASSERT(m_surface_dims_set); return m_surface_width; }
   int get_surface_height(void) const 
   { ASSERT(m_surface_dims_set); return m_surface_height; }
   int get_surface_size(int* surface_width, int* surface_height) const
   {
      ASSERT(m_surface_dims_set);
      if (!m_surface_dims_set)
         return FAILURE;
      *surface_width = m_surface_width;
      *surface_height = m_surface_height;
      return SUCCESS;
   }

   //
   // map projection wrapper functions
   //
   virtual int get_actual_center(d_geo_t* center) const
   {
      return proj()->get_center(center);
   }

   virtual degrees_t actual_center_lat(void) const
   { 
      d_geo_t center; 
      return (get_actual_center(&center) == SUCCESS ? center.lat : 0.0);
   }
   virtual degrees_t actual_center_lon(void) const 
   { 
      d_geo_t center; 
      return (get_actual_center(&center) == SUCCESS ? center.lon : 0.0);
   }

   int get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const
   {
      return proj()->get_vmap_bounds(map_ll, map_ur);
   }

   virtual d_geo_t vmap_sw_corner(void) const
   {
      d_geo_t sw, dummy;
      if (get_vmap_bounds(&sw, &dummy) == SUCCESS)
         return sw;
      else
      {
         d_geo_t tmp;
         tmp.lat = 0.0;
         tmp.lon = 0.0;
         return tmp;
      }
   }
   virtual d_geo_t vmap_ne_corner(void) const
   {
      d_geo_t ne, dummy;
      if (get_vmap_bounds(&dummy, &ne) == SUCCESS)
         return ne; 
      else
      {
         d_geo_t tmp;
         tmp.lat = 0.0;
         tmp.lon = 0.0;
         return tmp;
      }
   }
   virtual d_geo_t vmap_nw_corner(void) const
   {
      d_geo_t sw, ne;
      if (get_vmap_bounds(&sw, &ne) == SUCCESS)
      {
         d_geo_t nw;
         nw.lat = ne.lat;
         nw.lon = sw.lon;
         return nw;
      }
      else
      {
         d_geo_t tmp;
         tmp.lat = 0.0;
         tmp.lon = 0.0;
         return tmp;
      }
   }
   virtual d_geo_t vmap_se_corner(void) const
   {
      d_geo_t sw, ne;
      if (get_vmap_bounds(&sw, &ne) == SUCCESS)
      {
         d_geo_t se;
         se.lat = sw.lat;
         se.lon = ne.lon;
         return se;
      }
      else
      {
         d_geo_t tmp;
         tmp.lat = 0.0;
         tmp.lon = 0.0;
         return tmp;
      }
   }
   virtual degrees_t vmap_southern_bound(void) const
   { return vmap_sw_corner().lat; }
   virtual degrees_t vmap_northern_bound(void) const
   { return vmap_ne_corner().lat; }
   virtual degrees_t vmap_western_bound(void) const
   { return vmap_sw_corner().lon; }
   virtual degrees_t vmap_eastern_bound(void) const
   { return vmap_ne_corner().lon; }
   int get_vmap_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
      degrees_t *degrees_lon_per_pixel) const
   {
      return proj()->get_degrees_per_pixel(degrees_lat_per_pixel,
         degrees_lon_per_pixel); 
   }

   virtual degrees_t vmap_degrees_per_pixel_lat(void) const
   {
      degrees_t dpp_lat, dummy;
      return (get_vmap_degrees_per_pixel(&dpp_lat, &dummy) == SUCCESS ? dpp_lat : 0.0);
   }

   virtual degrees_t vmap_degrees_per_pixel_lon(void) const
   {
      degrees_t dpp_lon, dummy;
      return (get_vmap_degrees_per_pixel(&dummy, &dpp_lon) == SUCCESS ? dpp_lon : 0.0);
   }

   int get_vmap_bounds_at_pixel_edges(d_geo_t *map_ll, d_geo_t *map_ur) const
   { 
      d_geo_t map_ll_at_pixel_center;
      d_geo_t map_ur_at_pixel_center;

      if (proj()->get_vmap_bounds(&map_ll_at_pixel_center, &map_ur_at_pixel_center) != SUCCESS)
         return FAILURE;

      //
      // adjust each bound by half of a pixel to get the bounds at the pixel edges
      //

      const degrees_t dpp_lat = vmap_degrees_per_pixel_lat();
      map_ur->lat = map_ur_at_pixel_center.lat + dpp_lat/2.0;
      map_ll->lat = map_ll_at_pixel_center.lat - dpp_lat/2.0;

      const degrees_t dpp_lon = vmap_degrees_per_pixel_lon();
      map_ll->lon = map_ll_at_pixel_center.lon - dpp_lon/2.0;
      if (map_ll->lon < -180.0)
         map_ll->lon += 360.0;
      map_ur->lon = map_ur_at_pixel_center.lon + dpp_lon/2.0;
      if (map_ur->lon >= 180.0)
         map_ur->lon -= 360.0;

      return SUCCESS;
   }

   int get_pixels_around_world(int *x_pixels_around_world) const
   {
      return proj()->get_pixels_around_world(x_pixels_around_world);
   }

   void get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const
   {
      proj()->get_vsurface_size(virtual_surface_width, virtual_surface_height);
   }

   int vwidth(void) const
   {
      return proj()->vwidth();
   }

   int vheight(void) const
   {
      return proj()->vheight();
   }

   // ***********************************************************************
   // ***********************************************************************

   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      int *x_coord, int *y_coord) const
   {
      proj()->geo_to_vsurface(latitude, longitude, x_coord, y_coord);
   }

   // ***********************************************************************
   // ***********************************************************************

   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      double *x_coord, double *y_coord) const
   {
      proj()->geo_to_vsurface(latitude, longitude, x_coord, y_coord);
   }

   // ***********************************************************************
   // ***********************************************************************

   void geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
      degrees_t ur_lat, degrees_t ur_lon,
      int *ul_x, int *ul_y, int *lr_x, int *lr_y) const
   { 
      proj()->geo_to_vsurface_rect(ll_lat, ll_lon, ur_lat, ur_lon,
         ul_x, ul_y, lr_x, lr_y);
   }

   // ***********************************************************************
   // ***********************************************************************

   void vsurface_to_geo(int x_coord, int y_coord,
      degrees_t *latitude, degrees_t *longitude) const
   {
      proj()->vsurface_to_geo(x_coord, y_coord, latitude, longitude);
   }

   // ***********************************************************************
   // ***********************************************************************

   void vsurface_to_geo(double x_coord, double y_coord,
      degrees_t *latitude, degrees_t *longitude) const
   {
      proj()->vsurface_to_geo(x_coord, y_coord, latitude, longitude);
   }

   // ***********************************************************************
   // ***********************************************************************

   void vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
      degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const
   {
      proj()->vsurface_to_geo_rect(ul_x, ul_y, lr_x, lr_y,
         ll_lat, ll_lon, ur_lat, ur_lon);
   }

   // ***********************************************************************
   // ***********************************************************************

   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      int *surface_x, int *surface_y) const
   {
      return proj()->geo_to_surface(latitude, longitude, surface_x, surface_y);
   }

   // ***********************************************************************
   // ***********************************************************************

   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      double *surface_x, double *surface_y) const
   {
      return proj()->geo_to_surface(latitude, longitude, surface_x, surface_y);
   }

   // ***********************************************************************
   // ***********************************************************************

   int surface_to_geo(int surface_x, int surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   {
      return proj()->surface_to_geo(surface_x, surface_y, latitude, longitude);
   }

   // ***********************************************************************
   // ***********************************************************************

   int surface_to_geo(double surface_x, double surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   {
      return proj()->surface_to_geo(surface_x, surface_y, latitude, longitude);
   }

   // ***********************************************************************
   // ***********************************************************************

   boolean_t geo_in_surface(double lat, double lon) const
   {
      return proj()->geo_in_surface(lat, lon);
   }

   boolean_t geo_in_surface(double lat, double lon, int* s_x, int* s_y) const
   {
      return proj()->geo_in_surface(lat, lon, s_x, s_y);
   }

   boolean_t point_in_surface(int x, int y) const
   {
      return proj()->point_in_surface(x, y);
   }

   // ***********************************************************************
   // ***********************************************************************

   int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const
   {
      return proj()->vsurface_to_surface(vs_x, vs_y, s_x, s_y);
   }

   // ***********************************************************************
   // ***********************************************************************

   int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const
   {
      return proj()->vsurface_to_surface(vs_x, vs_y, s_x, s_y);
   }

   // ***********************************************************************
   // ***********************************************************************

   int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const
   {
      return proj()->surface_to_vsurface(s_x, s_y, vs_x, vs_y);
   }

   BOOL geoline_to_surface(double dLat1, double dLon1, double dLat2, double dLon2,
      int* x1, int* y1, int* x2, int* y2, int* wrapped_x1, int* wrapped_y1, int* wrapped_x2, int* wrapped_y2)
   {
      // not implemented on this class
      ASSERT(0);
      return FALSE;
   }

   int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const
   {
      return proj()->surface_to_vsurface(s_x, s_y, vs_x, vs_y);
   }

   virtual int get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence);

   //
   // to-scale functions
   //

   virtual boolean_t is_to_scale_projection(void) const
   {
      return m_is_to_scale_projection;
   }
   virtual double to_scale_surface_width_in_inches(void) const
   {
      ASSERT(is_to_scale_projection());

      if (!is_to_scale_projection())
         return 0.0;
      else
         return m_to_scale_surface_width_in_inches;
   }
   virtual double to_scale_surface_height_in_inches(void) const
   {
      ASSERT(is_to_scale_projection());

      if (!is_to_scale_projection())
         return 0.0;
      else
         return m_to_scale_surface_height_in_inches;
   }
   virtual int surface_scale_percent(void) const
   {
      ASSERT(is_to_scale_projection());

      if (!is_to_scale_projection())
         return 0;
      else
         return m_surface_scale_percent;
   }
   virtual double surface_scale_denom(void) const
   {
      ASSERT(is_to_scale_projection());

      if (!is_to_scale_projection())
         return 0.0;
      else
         return m_surface_scale_denom;
   }

   //
   // Mosaic map functions
   //

   virtual boolean_t is_mosaic_map_component(void) const
   {
      return m_is_mosaic_map_component;
   }
   virtual const MapProj* get_mosaic_map(void) const
   {
      ASSERT(is_mosaic_map_component());
      ASSERT(m_mosaic_map != NULL);

      return m_mosaic_map;
   }
   virtual int component_map_row(void) const
   {
      ASSERT(is_mosaic_map_component());

      return m_component_map_row;
   }
   virtual int component_map_column(void) const
   {
      ASSERT(is_mosaic_map_component());

      return m_component_map_column;
   }
   virtual int mosaic_surface_to_component_surface(int mosaic_x, int mosaic_y,
      int* component_x, int* component_y) const;
   virtual double component_map_vertical_offset_in_inches(void) const
   {
      ASSERT(is_mosaic_map_component());

      return m_component_map_vertical_offset_in_inches;
   }
   virtual double component_map_horizontal_offset_in_inches(void) const
   {
      ASSERT(is_mosaic_map_component());

      return m_component_map_horizontal_offset_in_inches;
   }
   virtual int mosaic_physical_to_component_physical(int mosaic_x, int mosaic_y,
      double component_map_print_width_in_inches, 
      double component_map_print_height_in_inches,
      double component_map_print_width_in_pixels, 
      double component_map_print_height_in_pixels,
      int* component_x, int* component_y) const;

   //
   // MapProjWithWriteableChartInfoImplBase overrides
   //
   virtual void set_is_mosaic_map_component(boolean_t is_mosaic_map_component)
   {
      m_is_mosaic_map_component = is_mosaic_map_component;
   }
   virtual void set_mosaic_map_ptr(const MapProj* mosaic_map)
   {
      m_mosaic_map = mosaic_map;
   }
   virtual void set_component_map_row(int row)
   {
      m_component_map_row = row;
   }
   virtual void set_component_map_column(int col)
   {
      m_component_map_column = col;
   }
   virtual void clear_mosaic_map_info(void)
   {
      m_is_mosaic_map_component = FALSE;
      m_mosaic_map = NULL;
      m_component_map_row = 0;
      m_component_map_column = 0;
      m_component_map_row_offset_in_mosaic_map_pixmap = 0.0;
      m_component_map_column_offset_in_mosaic_map_pixmap = 0.0;
      m_mosaic_to_component_pixel_lat_scale_ratio = 0.0;
      m_mosaic_to_component_pixel_lon_scale_ratio = 0.0;
      m_component_map_vertical_offset_in_inches = 0.0;
      m_component_map_horizontal_offset_in_inches = 0.0;
   }
   virtual void set_component_map_row_offset_in_mosaic_map_pixmap(
      double component_map_row_offset_in_mosaic_map_pixmap)
   {
      m_component_map_row_offset_in_mosaic_map_pixmap = 
         component_map_row_offset_in_mosaic_map_pixmap;
   }
   virtual void set_component_map_col_offset_in_mosaic_map_pixmap(
      double component_map_col_offset_in_mosaic_map_pixmap)
   {
      m_component_map_column_offset_in_mosaic_map_pixmap = 
         component_map_col_offset_in_mosaic_map_pixmap;
   }
   virtual void set_mosaic_to_component_pixel_lat_scale_ratio(
      double mosaic_to_component_pixel_lat_scale_ratio)
   {
      m_mosaic_to_component_pixel_lat_scale_ratio =
         mosaic_to_component_pixel_lat_scale_ratio;
   }
   virtual void set_mosaic_to_component_pixel_lon_scale_ratio(
      double mosaic_to_component_pixel_lon_scale_ratio)
   {
      m_mosaic_to_component_pixel_lon_scale_ratio =
         mosaic_to_component_pixel_lon_scale_ratio;
   }
   virtual void set_component_map_vertical_offset_in_inches(
      double component_map_vertical_offset_in_inches)
   {
      m_component_map_vertical_offset_in_inches = 
         component_map_vertical_offset_in_inches;
   }
   virtual void set_component_map_horizontal_offset_in_inches(
      double component_map_horizontal_offset_in_inches)
   {
      m_component_map_horizontal_offset_in_inches = 
         component_map_horizontal_offset_in_inches;
   }

protected:

private:

   // requested and then current map (after base map has been displayed)

   // KLUDGE - this var should be private but needs to be public to fix a problem
public:
   MapSpec m_spec;

private:
   boolean_t m_spec_initialized;

   //   map_proj m_proj;
   //   map_proj* proj(void) const
   //      { ASSERT(m_proj_set); return const_cast<map_proj*>(&m_proj); }
   boolean_t m_proj_set;

   // RMP - Added 12/14/99
   Projector *m_proj;
   Projector *proj(void) const
   {
      ASSERT(m_proj_set);
      return m_proj;
   }
   // RMP -End

   //
   // the actual (i.e. not requested) rotation and zoom percent
   //
   double m_actual_rotation;
   int m_actual_zoom_percent;

   //
   // the actual (not requested) projection 
   //
   ProjectionEnum m_actual_projection;

   boolean_t m_surface_dims_set;  // whether the surface dimensions have been set
   int m_surface_width;
   int m_surface_height;

private:

   //
   // to-scale info
   //
   boolean_t m_is_to_scale_projection;
   double m_to_scale_surface_width_in_inches;
   double m_to_scale_surface_height_in_inches;
   int m_surface_scale_percent;
   double m_surface_scale_denom;
   //
   // mosaic map info
   //
   boolean_t m_is_mosaic_map_component;
   const MapProj* m_mosaic_map;
   int m_component_map_row;
   int m_component_map_column;
   double m_component_map_row_offset_in_mosaic_map_pixmap;
   double m_component_map_column_offset_in_mosaic_map_pixmap;
   double m_mosaic_to_component_pixel_lat_scale_ratio;
   double m_mosaic_to_component_pixel_lon_scale_ratio;
   double m_component_map_vertical_offset_in_inches;
   double m_component_map_horizontal_offset_in_inches;
};


class ViewMapProj;

class SettableMapProjImpl : public SettableMapProj
{

public:
   SettableMapProjImpl()
   {
      m_data_check_result = FAILURE;

      m_data_check_done = FALSE;

      clear();
   }
   virtual ~SettableMapProjImpl()
   {
   }

protected:

   //
   // Note: MapProjWithWriteableChartInfoImpl is used rather than
   // MapProjWithWriteableChartInfo because SettableMapProjImpl needs to be 
   // to be able to access private members of MapProjWithWriteableChartInfoImpl
   // (and is hence of a friend of MapProjWithWriteableChartInfoImpl).
   //
   virtual MapProjWithWriteableChartInfoImpl* map(void) const
   {
      return const_cast<MapProjWithWriteableChartInfoImpl*>(&m_impl);
   }

private:

   // KLUDGE - this var should be private but needs to be public to fix a problem
public:
   MapProjWithWriteableChartInfoImpl m_impl;


public:

   //
   // MapProj wrappers
   //

   virtual boolean_t is_projection_set(void) const
   { return map()->is_projection_set(); }
   virtual boolean_t is_spec_set(void) const
   { return map()->is_spec_set(); }
   virtual boolean_t is_surface_set(void) const
   { return map()->is_surface_set(); }
   d_geo_t requested_center(void) const 
   { return map()->requested_center(); }
   MapSource source(void) const 
   { return map()->source(); }
   MapSource actual_source(void) const
   { return map()->source(); }
   MapScale scale(void) const 
   { return map()->scale(); }
   MapSeries series(void) const 
   { return map()->series(); }

   // RMP - 12/17/99
   ProjectionEnum projection_type(void) const
   { return map()->projection_type(); }

   ProjectionEnum actual_projection(void) const
   { return map()->actual_projection(); }

   ProjectionEnum requested_projection(void) const
      //{ return spec()->projection_type; }
   { return map()->requested_projection(); }


   double requested_rotation(void) const 
   { return map()->requested_rotation(); }
   int requested_zoom_percent(void) const 
   { return map()->requested_zoom_percent(); }
   degrees_t requested_center_lat(void) const 
   { return map()->requested_center_lat(); }
   degrees_t requested_center_lon(void) const 
   { return map()->requested_center_lon(); }
   const MapSpec* spec(void) const 
   { return map()->spec(); }
   int get_surface_width(void) const 
   { return map()->get_surface_width(); }
   int get_surface_height(void) const 
   { return map()->get_surface_height(); }
   virtual int get_surface_size(int* surface_width, int* surface_height) const
   { return map()->get_surface_size(surface_width, surface_height); }
   virtual double actual_rotation(void) const
   { return map()->actual_rotation(); }
   virtual int actual_zoom_percent(void) const
   { return map()->actual_zoom_percent(); }
   virtual int get_actual_center(d_geo_t* center) const
   { return map()->get_actual_center(center); }
   virtual degrees_t actual_center_lat(void) const
   { return map()->actual_center_lat(); }
   virtual degrees_t actual_center_lon(void) const
   { return map()->actual_center_lon(); }
   void get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const
   { map()->get_vsurface_size(virtual_surface_width, virtual_surface_height); }
   int vwidth(void) const
   { return map()->vwidth(); }
   int vheight(void) const
   { return map()->vheight(); }
   int get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const
   { return map()->get_vmap_bounds(map_ll, map_ur); }
   virtual d_geo_t vmap_sw_corner(void) const
   { return map()->vmap_sw_corner(); }
   virtual d_geo_t vmap_nw_corner(void) const
   { return map()->vmap_nw_corner(); }
   virtual d_geo_t vmap_ne_corner(void) const
   { return map()->vmap_ne_corner(); }
   virtual d_geo_t vmap_se_corner(void) const
   { return map()->vmap_se_corner(); }
   virtual degrees_t vmap_southern_bound(void) const
   { return map()->vmap_southern_bound(); }
   virtual degrees_t vmap_northern_bound(void) const
   { return map()->vmap_northern_bound(); }
   virtual degrees_t vmap_western_bound(void) const
   { return map()->vmap_western_bound(); }
   virtual degrees_t vmap_eastern_bound(void) const
   { return map()->vmap_eastern_bound(); }
   int get_vmap_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
      degrees_t *degrees_lon_per_pixel) const
   {
      return map()->get_vmap_degrees_per_pixel(degrees_lat_per_pixel,
         degrees_lon_per_pixel); 
   }
   virtual degrees_t vmap_degrees_per_pixel_lat(void) const
   { return map()->vmap_degrees_per_pixel_lat(); }
   virtual degrees_t vmap_degrees_per_pixel_lon(void) const
   { return map()->vmap_degrees_per_pixel_lon(); }
   int get_vmap_bounds_at_pixel_edges(d_geo_t *map_ll, d_geo_t *map_ur) const
   { return map()->get_vmap_bounds_at_pixel_edges(map_ll, map_ur); }
   int get_pixels_around_world(int *x_pixels_around_world) const
   { return map()->get_pixels_around_world(x_pixels_around_world); }
   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      int *x_coord, int *y_coord) const
   { map()->geo_to_vsurface(latitude, longitude, x_coord, y_coord); }
   void geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
      degrees_t ur_lat, degrees_t ur_lon,
      int *ul_x, int *ul_y, int *lr_x, int *lr_y) const
   { 
      map()->geo_to_vsurface_rect(ll_lat, ll_lon, ur_lat, ur_lon,
         ul_x, ul_y, lr_x, lr_y);
   }
   void vsurface_to_geo(int x_coord, int y_coord,
      degrees_t *latitude, degrees_t *longitude) const
   { map()->vsurface_to_geo(x_coord, y_coord, latitude, longitude); }
   void vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
      degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const
   {
      map()->vsurface_to_geo_rect(ul_x, ul_y, lr_x, lr_y,
         ll_lat, ll_lon, ur_lat, ur_lon);
   }
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      int *surface_x, int *surface_y) const
   { return map()->geo_to_surface(latitude, longitude, surface_x, surface_y); }
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      double *surface_x, double *surface_y) const
   { return map()->geo_to_surface(latitude, longitude, surface_x, surface_y); }
   int surface_to_geo(int surface_x, int surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   { return map()->surface_to_geo(surface_x, surface_y, latitude, longitude); }
   int surface_to_geo(double surface_x, double surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   { return map()->surface_to_geo(surface_x, surface_y, latitude, longitude); }
   boolean_t geo_in_surface(double lat, double lon) const
   { return map()->geo_in_surface(lat, lon); }
   boolean_t geo_in_surface(double lat, double lon, int* s_x, int* s_y) const
   { return map()->geo_in_surface(lat, lon, s_x, s_y); }
   boolean_t point_in_surface(int x, int y) const
   { return map()->point_in_surface(x, y); }
   int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const
   { return map()->vsurface_to_surface(vs_x, vs_y, s_x, s_y); }
   int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const
   { return map()->vsurface_to_surface(vs_x, vs_y, s_x, s_y); }
   int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const
   { return map()->surface_to_vsurface(s_x, s_y, vs_x, vs_y); }
   int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const
   { return map()->surface_to_vsurface(s_x, s_y, vs_x, vs_y); }
   BOOL geoline_to_surface(double dLat1, double dLon1, double dLat2, double dLon2,
      int* x1, int* y1, int* x2, int* y2, int* wrapped_x1, int* wrapped_y1, int* wrapped_x2, int* wrapped_y2)
   { return map()->geoline_to_surface(dLat1, dLon1, dLat2, dLon2, x1, y1, x2, y2, wrapped_x1, wrapped_y1, wrapped_x2, wrapped_y2); }

   int get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence)
   {
      return map()->get_meridian_covergence( geo_pos, convergence );
   }

   //
   // MapProjWithWriteableChartInfo overrides
   //
   virtual boolean_t is_to_scale_projection(void) const
   { return map()->is_to_scale_projection(); }
   virtual double to_scale_surface_width_in_inches(void) const
   { return map()->to_scale_surface_width_in_inches(); }
   virtual double to_scale_surface_height_in_inches(void) const
   { return map()->to_scale_surface_height_in_inches(); }
   virtual int surface_scale_percent(void) const
   { return map()->surface_scale_percent(); }
   virtual double surface_scale_denom(void) const
   { return map()->surface_scale_denom(); }

   virtual boolean_t is_mosaic_map_component(void) const
   {
      return map()->is_mosaic_map_component();
   }
   virtual const MapProj* get_mosaic_map(void) const
   {
      return map()->get_mosaic_map();
   }
   virtual int component_map_row(void) const
   {
      return map()->component_map_row();
   }
   virtual int component_map_column(void) const
   {
      return map()->component_map_column();
   }
   virtual int mosaic_surface_to_component_surface(int mosaic_x, int mosaic_y,
      int* component_x, int* component_y) const
   {
      return map()->mosaic_surface_to_component_surface(mosaic_x, mosaic_y,
         component_x, component_y);
   }
   virtual double component_map_vertical_offset_in_inches(void) const
   {
      return map()->component_map_vertical_offset_in_inches();
   }
   virtual double component_map_horizontal_offset_in_inches(void) const
   {
      return map()->component_map_horizontal_offset_in_inches();
   }
   virtual int mosaic_physical_to_component_physical(int mosaic_x, int mosaic_y,
      double component_map_print_width_in_inches, 
      double component_map_print_height_in_inches,
      double component_map_print_width_in_pixels, 
      double component_map_print_height_in_pixels,
      int* component_x, int* component_y) const
   {
      return map()->mosaic_physical_to_component_physical(mosaic_x, mosaic_y,
         component_map_print_width_in_inches, component_map_print_height_in_inches,
         component_map_print_width_in_pixels, component_map_print_height_in_pixels,
         component_x, component_y);
   }

   // 
   // SettableMapProj overrides
   //

   virtual int clear(void)
   {

      //
      // make sure to mark the projection as invalid
      //
      map()->m_proj_set = FALSE;
      map()->clear_mosaic_map_info();

      map()->m_is_to_scale_projection = FALSE;
      map()->m_to_scale_surface_width_in_inches = 0.0;
      map()->m_to_scale_surface_height_in_inches = 0.0;
      map()->m_surface_scale_percent = 0;
      map()->m_surface_scale_denom = 0.0;

      return SUCCESS;
   }

   virtual int set_spec(const MapSpec& spec)
   {
      m_data_check_done = FALSE;

      // KLUDGE - ACCESSSING PRIVATE MEMBERS

      map()->m_spec = spec;
      if (m_map != NULL)
      {
         map()->m_spec.m_dBrightness = m_map->get_brightness();
         m_map->get_contrast(&map()->m_spec.m_dContrast, &map()->m_spec.m_nContrastMidval);
      }

      map()->m_spec_initialized = TRUE;

      map()->m_proj_set = FALSE;

      map()->clear_mosaic_map_info();

      return SUCCESS;
   }

   virtual int set_surface_size(int surface_width, int surface_height)
   {
      m_data_check_done = FALSE;

      // KLUDGE - ACCESSSING PRIVATE MEMBERS

      map()->m_surface_width = surface_width;
      map()->m_surface_height = surface_height;

      map()->m_surface_dims_set = TRUE;

      map()->m_proj_set = FALSE;

      map()->clear_mosaic_map_info();

      return SUCCESS;
   }

   //
   // must be successfully called before any other member functions 
   // can be called
   //
   virtual int bind_equal_arc_map_to_surface(void);

   //
   // extensions
   //

   virtual int bind_equal_arc_map_to_scale_surface(double surface_width_in_inches,
      double surface_height_in_inches, int surface_scale_percent, 
      bool calc_surface_logical_units = true,
      int surface_width = 0, int surface_height = 0);

   virtual int bind_equal_arc_map_to_surface_using_geo_bounds(degrees_t northern_lat, 
      degrees_t southern_lat, degrees_t western_lon, degrees_t eastern_lon);
   virtual int bind_equal_arc_map_to_surface_using_geo_bounds(d_geo_t ul, 
      d_geo_t ur, d_geo_t ll, d_geo_t lr);
   virtual int bind_equal_arc_map_to_scale_surface_using_geo_bounds(degrees_t northern_lat, 
      degrees_t southern_lat, degrees_t western_lon, degrees_t eastern_lon,
      double surface_width_in_inches, double surface_height_in_inches,
      int surface_scale_percent);


   virtual int bind_equal_arc_map_to_scale_mosaic_component_surface(
      degrees_t northern_lat, degrees_t southern_lat, 
      degrees_t western_lon, degrees_t eastern_lon, 
      double component_surface_width_in_inches, double component_surface_height_in_inches,
      int surface_scale_percent, const MapProj* mosaic_map, 
      int component_map_row, int component_map_col,
      double component_map_vertical_offset_in_inches,
      double component_map_horizontal_offset_in_inches);
   virtual int bind_equal_arc_map_to_scale_mosaic_component_surface_lambert(
      d_geo_t ul, d_geo_t ur, d_geo_t ll, d_geo_t lr,
      double component_surface_width_in_inches, double component_surface_height_in_inches,
      int surface_scale_percent, const MapProj* mosaic_map, 
      int component_map_row, int component_map_col,
      double component_map_vertical_offset_in_inches,
      double component_map_horizontal_offset_in_inches);


   //
   // These functions return SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
   //
   virtual int bind_equal_arc_map_to_surface_and_check_data(void);
   virtual int bind_equal_arc_map_to_scale_surface_and_check_data(double surface_width_in_inches,
      double surface_height_in_inches, int surface_scale_percent);

   //
   // Does a data check after projection has been set.
   // Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
   //
   virtual int data_check_without_setting_projection(void);

   // Check if the map surface has any invalid or invisible points
   // due to a change in scale/projection
   virtual bool has_invalid_points(void) const;


   //protected:
   // KLUDGE - THIS SHOULD BE PROTECTED
public:

   Projector* proj() { return map()->proj(); }

private:

   //
   // Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
   //
   int get_and_test_coverage_list() const;

protected:

   //
   // extension
   //
   // Returns SUCCESS / FAILURE.  If the reason for failure is that the
   // virtual map will span more than 360 degrees of longitude the 
   // geo_width_violation will be true.
   virtual int lcl_bind_equal_arc_map_to_surface(boolean_t is_to_scale,
      boolean_t use_printer_deg_per_pixel_values,
      degrees_t adjusted_center_lat, bool &geo_width_violation);

private:

   int m_data_check_result;

   boolean_t m_data_check_done;

public:

   // Calculates the surface dimensions in logical units and the scale
   // denominator if this MapProj is used on a surface with the given
   // dimensions in inches and scale percent.
   //
   // Returns SUCCESS/FAILURE.  Check surface_size_valid upon SUCCESS to see
   // if the computed surface dimensions are actually valid. Dimensions are
   // invalid when they require more logical units than there are pixels of
   // data in the world.
   int calc_surface_logical_units_and_scale_denom(
      double surface_width_in_inches, double surface_height_in_inches,
      int surface_scale_percent, double &surface_scale_denom,
      int &surface_width_in_logical_units, 
      int &surface_height_in_logical_units, bool &surface_size_valid,
      degrees_t &adjusted_center_lat);

   virtual double get_meters_per_pixel_lat() { return 0.0; }
   virtual double get_meters_per_pixel_lon() { return 0.0; }
};

class MapProjImpl : public SettableMapProj
{

public:
   MapProjImpl()
   {
      m_data_check_result = FAILURE;

      m_data_check_done = FALSE;

      m_map = NULL;

      clear();
   }
   virtual ~MapProjImpl()
   {
   }

protected:

   //
   // Note: MapProjWithWriteableChartInfoImpl is used rather than
   // MapProjWithWriteableChartInfo because SettableMapProjImpl needs to be 
   // to be able to access private members of MapProjWithWriteableChartInfoImpl
   // (and is hence of a friend of MapProjWithWriteableChartInfoImpl).
   //
   virtual MapProjWithWriteableChartInfoImpl* map(void) const
   {
      return const_cast<MapProjWithWriteableChartInfoImpl*>(&m_impl);
   }

private:

   // KLUDGE - this var should be private but needs to be public to fix a problem
public:
   MapProjWithWriteableChartInfoImpl m_impl;
   MapSpec m_map_spec;

public:

   //
   // MapProj wrappers
   //

   virtual boolean_t is_projection_set(void) const
   {
      int is_set;
      m_map->is_projection_set(&is_set);
      return is_set;
   }
   virtual boolean_t is_spec_set(void) const
   {
      int is_set;
      m_map->is_spec_set(&is_set);
      return is_set;
   }
   virtual boolean_t is_surface_set(void) const
   {
      int is_set;
      m_map->is_surface_set(&is_set);
      return is_set;
   }
   d_geo_t requested_center(void) const 
   {
      d_geo_t center;
      m_map->requested_center(&center.lat, &center.lon);
      return center;
   }
   MapSource source(void) const 
   {
      _bstr_t src;
      m_map->source(src.GetAddress());
      MapSource ret = MapSource(src);
      return ret;
   }
   MapSource actual_source(void) const
   {
      _bstr_t actual_src = m_map2->get_actual_source();
      return MapSource(actual_src);
   }
   MapScale scale(void) const 
   {
      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      m_map->scale(&dScale, &eScaleUnits);

      if (eScaleUnits == MAP_SCALE_WORLD)
         return MapScale(WORLD);

      MapScale::unit_t units;
      switch(eScaleUnits)
      {
      case MAP_SCALE_NM: units = MapScale::NM; break;
      case MAP_SCALE_KILOMETER: units = MapScale::KILOMETER; break;
      case MAP_SCALE_METERS: units = MapScale::METERS; break;
      case MAP_SCALE_ARC_SECONDS: units = MapScale::ARC_SECONDS; break;
      case MAP_SCALE_ARC_MINUTES: units = MapScale::ARC_MINUTES; break;
      }

      if (eScaleUnits == MAP_SCALE_DENOMINATOR)
         return MapScale(static_cast<int>(dScale));

      return MapScale(dScale, units);
   }
   MapSeries series(void) const
   {
      _bstr_t series_str;
      m_map->series(series_str.GetAddress());
      MapSeries ret = MapSeries(series_str);
      return ret;
   }

   // RMP - 12/17/99
   ProjectionEnum projection_type(void) const
   {
      return actual_projection();
   }

   ProjectionEnum actual_projection(void) const
   {
      ProjectionEnum proj;
      m_map->actual_projection(&proj);
      return proj;
   }

   ProjectionEnum requested_projection(void) const
   {
      ProjectionEnum proj;
      m_map->requested_projection(&proj);
      return proj;
   }

   double requested_rotation(void) const 
   {
      double rot;
      m_map->requested_rotation(&rot);
      return rot;
   }
   int requested_zoom_percent(void) const 
   {
      int zoom;
      m_map->requested_zoom_percent(&zoom);
      return zoom;
   }
   degrees_t requested_center_lat(void) const 
   {
      double lat;
      m_map->requested_center_lat(&lat);
      return lat;
   }
   degrees_t requested_center_lon(void) const 
   {
      double lon;
      m_map->requested_center_lon(&lon);
      return lon;
   }
   const MapSpec* spec(void) const 
   {
      return &m_map_spec;
   }

   int get_surface_width(void) const 
   {
      int width;
      m_map->get_surface_width(&width);
      return width;
   }
   int get_surface_height(void) const 
   {
      int height;
      m_map->get_surface_height(&height);
      return height;
   }
   virtual int get_surface_size(int* surface_width, int* surface_height) const
   {
      int result;
      m_map->get_surface_size(surface_width, surface_height, &result);
      return result;
   }
   virtual double actual_rotation(void) const
   {
      double rot;
      m_map->actual_rotation(&rot);
      return rot;
   }
   virtual int actual_zoom_percent(void) const
   {
      int zoom;
      m_map->actual_zoom_percent(&zoom);
      return zoom;
   }
   virtual int get_actual_center(d_geo_t* center) const
   {
      int result;
      m_map->get_actual_center(&center->lat, &center->lon, &result);
      return result;
   }
   virtual degrees_t actual_center_lat(void) const
   {
      double lat;
      m_map->actual_center_lat(&lat);
      return lat;
   }
   virtual degrees_t actual_center_lon(void) const
   {
      double lon;
      m_map->actual_center_lon(&lon);
      return lon;
   }
   void get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const
   {
      m_map->get_vsurface_size(virtual_surface_width, virtual_surface_height,
         EQUALARC_VSURFACE);
   }
   int vwidth(void) const
   {
      int vwidth;
      m_map->vwidth(&vwidth, EQUALARC_VSURFACE);
      return vwidth;
   }
   int vheight(void) const
   {
      int vheight;
      m_map->vheight(&vheight, EQUALARC_VSURFACE);
      return vheight;
   }
   int get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const
   {
      int result;
      m_map->get_vmap_bounds(&map_ll->lat, &map_ll->lon, &map_ur->lat, &map_ur->lon,
         &result, EQUALARC_VSURFACE);
      return result;
   }
   virtual d_geo_t vmap_sw_corner(void) const
   {
      d_geo_t corner;
      m_map->vmap_sw_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
      return corner;
   }
   virtual d_geo_t vmap_nw_corner(void) const
   {
      d_geo_t corner;
      m_map->vmap_nw_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
      return corner;
   }
   virtual d_geo_t vmap_ne_corner(void) const
   {
      d_geo_t corner;
      m_map->vmap_ne_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
      return corner;
   }
   virtual d_geo_t vmap_se_corner(void) const
   {
      d_geo_t corner;
      m_map->vmap_se_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
      return corner;
   }
   virtual degrees_t vmap_southern_bound(void) const
   {
      double bound;
      m_map->vmap_southern_bound(&bound, EQUALARC_VSURFACE);
      return bound;
   }
   virtual degrees_t vmap_northern_bound(void) const
   {
      double bound;
      m_map->vmap_northern_bound(&bound, EQUALARC_VSURFACE);
      return bound;
   }
   virtual degrees_t vmap_western_bound(void) const
   {
      double bound;
      m_map->vmap_western_bound(&bound, EQUALARC_VSURFACE);
      return bound;
   }
   virtual degrees_t vmap_eastern_bound(void) const
   {
      double bound;
      m_map->vmap_eastern_bound(&bound, EQUALARC_VSURFACE);
      return bound;
   }
   int get_vmap_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
      degrees_t *degrees_lon_per_pixel) const
   {
      int result;
      m_map->get_vmap_degrees_per_pixel(degrees_lat_per_pixel,
         degrees_lon_per_pixel, &result, EQUALARC_VSURFACE);
      return result;
   }
   virtual degrees_t vmap_degrees_per_pixel_lat(void) const
   {
      double dpp;
      m_map->vmap_degrees_per_pixel_lat(&dpp, EQUALARC_VSURFACE);
      return dpp;
   }
   virtual degrees_t vmap_degrees_per_pixel_lon(void) const
   {
      double dpp;
      m_map->vmap_degrees_per_pixel_lon(&dpp, EQUALARC_VSURFACE);
      return dpp;
   }
   int get_vmap_bounds_at_pixel_edges(d_geo_t *map_ll, d_geo_t *map_ur) const
   {
      int result;
      m_map->get_vmap_bounds_at_pixel_edges(&map_ll->lat, &map_ll->lon, &map_ur->lat,
         &map_ur->lon, &result, EQUALARC_VSURFACE);
      return result;
   }
   int get_pixels_around_world(int *x_pixels_around_world) const
   {
      int result;
      m_map->get_pixels_around_world(x_pixels_around_world, &result);
      return result;
   }
   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      int *x_coord, int *y_coord) const
   {
      m_map->geo_to_vsurface(latitude, longitude, x_coord, y_coord, EQUALARC_VSURFACE);
   }
   void geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
      degrees_t ur_lat, degrees_t ur_lon,
      int *ul_x, int *ul_y, int *lr_x, int *lr_y) const
   {
      m_map->geo_to_vsurface_rect(ll_lat, ll_lon, ur_lat, ur_lon, ul_x, ul_y,
         lr_x, lr_y, EQUALARC_VSURFACE);
   }
   void vsurface_to_geo(int x_coord, int y_coord,
      degrees_t *latitude, degrees_t *longitude) const
   {
      m_map->vsurface_to_geo(x_coord, y_coord, latitude, longitude, EQUALARC_VSURFACE);
   }
   void vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
      degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const
   {
      m_map->vsurface_to_geo_rect(ul_x, ul_y, lr_x, lr_y, ll_lat, ll_lon, ur_lat, ur_lon,
         EQUALARC_VSURFACE);
   }
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      int *surface_x, int *surface_y) const
   {
      int result;
      m_map->geo_to_surface(latitude, longitude, surface_x, surface_y, &result);

      // FalconView is not checking the NONVISIBLE_RESULT
      return result == FAILURE ? FAILURE : SUCCESS;
   }
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      double *surface_x, double *surface_y) const
   {
      int result;
      m_map->geo_to_surface_dbl(latitude, longitude, surface_x, surface_y, &result);

      // FalconView is no checking the NONVISIBLE_RESULT
      return result == FAILURE ? FAILURE : SUCCESS;
   }
   int surface_to_geo(int surface_x, int surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   {
      int result;
      m_map->surface_to_geo(surface_x, surface_y, latitude, longitude, &result);
      return result;
   }
   int surface_to_geo(double surface_x, double surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   {
      int result;
      m_map->surface_to_geo_dbl(surface_x, surface_y, latitude, longitude, &result);
      return result;
   }
   boolean_t geo_in_surface(double lat, double lon) const
   {
      int in_surface;
      m_map->geo_in_surface(lat, lon, &in_surface);
      return in_surface;
   }
   boolean_t geo_in_surface(double lat, double lon, int* s_x, int* s_y) const
   {
      int in_surface;
      m_map->geo_in_surface_scr(lat, lon, s_x, s_y, &in_surface);
      return in_surface;
   }
   boolean_t point_in_surface(int x, int y) const
   {
      int in_surface;
      m_map->point_in_surface(x, y, &in_surface);
      return in_surface;
   }
   int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const
   {
      int result;
      m_map->vsurface_to_surface(vs_x, vs_y, s_x, s_y, &result, EQUALARC_VSURFACE);
      return result;
   }
   int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const
   {
      int result;
      m_map->vsurface_to_surface_dbl(vs_x, vs_y, s_x, s_y, &result, EQUALARC_VSURFACE);
      return result;
   }
   int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const
   {
      int result;
      m_map->surface_to_vsurface(s_x, s_y, vs_x, vs_y, &result, EQUALARC_VSURFACE);
      return result;
   }
   int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const
   {
      int result;
      m_map->surface_to_vsurface_dbl(s_x, s_y, vs_x, vs_y, &result, EQUALARC_VSURFACE);
      return result;
   }
   BOOL geoline_to_surface(double dLat1, double dLon1, double dLat2, double dLon2,
      int* x1, int* y1, int* x2, int* y2, int* wrapped_x1, int* wrapped_y1, int* wrapped_x2, int* wrapped_y2)
   {
      return m_map2->geoline_to_surface(dLat1, dLon1, dLat2, dLon2,
         x1, y1, x2, y2, wrapped_x1, wrapped_y1, wrapped_x2, wrapped_y2);
   }

   int get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence)
   {
      int result;
      m_map->get_meridian_covergence(geo_pos.lat, geo_pos.lon, convergence, &result);
      return result;
   }

   //
   // MapProjWithWriteableChartInfo overrides
   //

   virtual boolean_t is_to_scale_projection(void) const
   {
      int to_scale;
      m_map->is_to_scale_projection(&to_scale);
      return to_scale;
   }
   virtual double to_scale_surface_width_in_inches(void) const
   {
      double width;
      m_map->to_scale_surface_width_in_inches(&width);
      return width;
   }
   virtual double to_scale_surface_height_in_inches(void) const
   {
      double height;
      m_map->to_scale_surface_height_in_inches(&height);
      return height;
   }
   virtual int surface_scale_percent(void) const
   {
      int percent;
      m_map->surface_scale_percent(&percent);
      return percent;
   }
   virtual double surface_scale_denom(void) const
   {
      double scale;
      m_map->surface_scale_denom(&scale);
      return scale;
   }
   virtual boolean_t is_mosaic_map_component(void) const
   {
      return map()->is_mosaic_map_component();
   }
   virtual const MapProj* get_mosaic_map(void) const
   {
      return map()->get_mosaic_map();
   }
   virtual int component_map_row(void) const
   {
      return map()->component_map_row();
   }
   virtual int component_map_column(void) const
   {
      return map()->component_map_column();
   }
   virtual int mosaic_surface_to_component_surface(int mosaic_x, int mosaic_y,
      int* component_x, int* component_y) const
   {
      return map()->mosaic_surface_to_component_surface(mosaic_x, mosaic_y,
         component_x, component_y);
   }
   virtual double component_map_vertical_offset_in_inches(void) const
   {
      return map()->component_map_vertical_offset_in_inches();
   }
   virtual double component_map_horizontal_offset_in_inches(void) const
   {
      return map()->component_map_horizontal_offset_in_inches();
   }
   virtual int mosaic_physical_to_component_physical(int mosaic_x, int mosaic_y,
      double component_map_print_width_in_inches, 
      double component_map_print_height_in_inches,
      double component_map_print_width_in_pixels, 
      double component_map_print_height_in_pixels,
      int* component_x, int* component_y) const
   {
      return map()->mosaic_physical_to_component_physical(mosaic_x, mosaic_y,
         component_map_print_width_in_inches, component_map_print_height_in_inches,
         component_map_print_width_in_pixels, component_map_print_height_in_pixels,
         component_x, component_y);
   }

   // 
   // SettableMapProj overrides
   //

   virtual int clear(void)
   {

      //
      // make sure to mark the projection as invalid
      //
      map()->m_proj_set = FALSE;
      map()->clear_mosaic_map_info();

      map()->m_is_to_scale_projection = FALSE;
      map()->m_to_scale_surface_width_in_inches = 0.0;
      map()->m_to_scale_surface_height_in_inches = 0.0;
      map()->m_surface_scale_percent = 0;
      map()->m_surface_scale_denom = 0.0;

      return SUCCESS;
   }

   virtual int set_spec(const MapSpec& spec)
   {
      m_data_check_done = FALSE;

      // KLUDGE - ACCESSSING PRIVATE MEMBERS

      map()->m_spec = spec;
      if (m_map != NULL)
      {
         map()->m_spec.m_dBrightness = m_map->get_brightness();
         m_map->get_contrast(&map()->m_spec.m_dContrast, &map()->m_spec.m_nContrastMidval);
      }

      map()->m_spec_initialized = TRUE;

      map()->m_proj_set = FALSE;

      map()->clear_mosaic_map_info();

      return SUCCESS;
   }

   virtual int set_surface_size(int surface_width, int surface_height)
   {
      m_data_check_done = FALSE;

      // KLUDGE - ACCESSSING PRIVATE MEMBERS

      map()->m_surface_width = surface_width;
      map()->m_surface_height = surface_height;

      map()->m_surface_dims_set = TRUE;

      map()->m_proj_set = FALSE;

      map()->clear_mosaic_map_info();

      return SUCCESS;
   }

   //
   // must be successfully called before any other member functions 
   // can be called
   //
   virtual int bind_equal_arc_map_to_surface(void) { return FAILURE; }

   //
   // extensions
   //

   virtual int bind_equal_arc_map_to_scale_surface(double surface_width_in_inches,
      double surface_height_in_inches, int surface_scale_percent, 
      bool calc_surface_logical_units = true,
      int surface_width = 0, int surface_height = 0) { return FAILURE; }

   virtual int bind_equal_arc_map_to_surface_using_geo_bounds(degrees_t northern_lat, 
      degrees_t southern_lat, degrees_t western_lon, degrees_t eastern_lon) { return FAILURE; }
   virtual int bind_equal_arc_map_to_surface_using_geo_bounds(d_geo_t ul, 
      d_geo_t ur, d_geo_t ll, d_geo_t lr) { return FAILURE; }
   virtual int bind_equal_arc_map_to_scale_surface_using_geo_bounds(degrees_t northern_lat, 
      degrees_t southern_lat, degrees_t western_lon, degrees_t eastern_lon,
      double surface_width_in_inches, double surface_height_in_inches,
      int surface_scale_percent) { return FAILURE; }


   virtual int bind_equal_arc_map_to_scale_mosaic_component_surface(
      degrees_t northern_lat, degrees_t southern_lat, 
      degrees_t western_lon, degrees_t eastern_lon, 
      double component_surface_width_in_inches, double component_surface_height_in_inches,
      int surface_scale_percent, const MapProj* mosaic_map, 
      int component_map_row, int component_map_col,
      double component_map_vertical_offset_in_inches,
      double component_map_horizontal_offset_in_inches) { return FAILURE; }
   virtual int bind_equal_arc_map_to_scale_mosaic_component_surface_lambert(
      d_geo_t ul, d_geo_t ur, d_geo_t ll, d_geo_t lr,
      double component_surface_width_in_inches, double component_surface_height_in_inches,
      int surface_scale_percent, const MapProj* mosaic_map, 
      int component_map_row, int component_map_col,
      double component_map_vertical_offset_in_inches,
      double component_map_horizontal_offset_in_inches) { return FAILURE;}


   //
   // These functions return SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
   //
   virtual int bind_equal_arc_map_to_surface_and_check_data(void) { return FAILURE;}
   virtual int bind_equal_arc_map_to_scale_surface_and_check_data(double surface_width_in_inches,
      double surface_height_in_inches, int surface_scale_percent) { return FAILURE; }

   //
   // Does a data check after projection has been set.
   // Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
   //
   virtual int data_check_without_setting_projection(void) { return FAILURE; }

   // Check if the map surface has any invalid or invisible points
   // due to a change in scale/projection
   virtual bool has_invalid_points(void) const { return false; }

   virtual double get_meters_per_pixel_lat()
   {
      return m_map->get_meters_per_pixel_lat();
   }

   virtual double get_meters_per_pixel_lon()
   {
      return m_map->get_meters_per_pixel_lon();
   }


   //protected:
   // KLUDGE - THIS SHOULD BE PROTECTED
public:

   Projector* proj() { return NULL; }

private:

   //
   // Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
   //
   int get_and_test_coverage_list() const { return FAILURE; }

protected:

   //
   // extension
   //
   // Returns SUCCESS / FAILURE.  If the reason for failure is that the
   // virtual map will span more than 360 degrees of longitude the 
   // geo_width_violation will be true.
   virtual int lcl_bind_equal_arc_map_to_surface(boolean_t is_to_scale,
      boolean_t use_printer_deg_per_pixel_values,
      degrees_t adjusted_center_lat, bool &geo_width_violation) { return FAILURE; }

private:

   int m_data_check_result;

   boolean_t m_data_check_done;

public:

   // Calculates the surface dimensions in logical units and the scale
   // denominator if this MapProj is used on a surface with the given
   // dimensions in inches and scale percent.
   //
   // Returns SUCCESS/FAILURE.  Check surface_size_valid upon SUCCESS to see
   // if the computed surface dimensions are actually valid. Dimensions are
   // invalid when they require more logical units than there are pixels of
   // data in the world.
   int calc_surface_logical_units_and_scale_denom(
      double surface_width_in_inches, double surface_height_in_inches,
      int surface_scale_percent, double &surface_scale_denom,
      int &surface_width_in_logical_units, 
      int &surface_height_in_logical_units, bool &surface_size_valid,
      degrees_t &adjusted_center_lat) { return FAILURE; }
};

/////////////////////////////////////////////////////////////////////////////

// a MapProj potentitally associated with a view window
class ViewMapProjImpl : public ViewMapProj
{

public:

   ViewMapProjImpl(void)
   {
      m_view = NULL;
      m_map = NULL;
   }
   ViewMapProjImpl(CWnd* view)
   {
      m_view = view;
      m_map = NULL;
   }
   virtual ~ViewMapProjImpl(void) {}


   //
   // MapProj wrappers
   //

   virtual boolean_t is_projection_set(void) const
   { return map()->is_projection_set(); }
   virtual boolean_t is_spec_set(void) const
   { return map()->is_spec_set(); }
   virtual boolean_t is_surface_set(void) const
   { return map()->is_surface_set(); }
   d_geo_t requested_center(void) const 
   { return map()->requested_center(); }
   MapSource source(void) const 
   { return map()->source(); }
   MapSource actual_source(void) const
   { return map()->actual_source(); }
   MapScale scale(void) const 
   { return map()->scale(); }
   MapSeries series(void) const 
   { return map()->series(); }
   double requested_rotation(void) const 
   { return map()->requested_rotation(); }
   int requested_zoom_percent(void) const 
   { return map()->requested_zoom_percent(); }
   degrees_t requested_center_lat(void) const 
   { return map()->requested_center_lat(); }
   degrees_t requested_center_lon(void) const 
   { return map()->requested_center_lon(); }

   ProjectionEnum projection_type(void)  const // RMP - Added 12/21/99
   { return map()->projection_type(); }

   ProjectionEnum actual_projection(void) const
   { return map()->actual_projection(); }

   ProjectionEnum requested_projection(void) const
   { return map()->requested_projection(); }


   const MapSpec* spec(void) const 
   { return map()->spec(); }
   int get_surface_width(void) const 
   { return map()->get_surface_width(); }
   int get_surface_height(void) const 
   { return map()->get_surface_height(); }
   virtual int get_surface_size(int* surface_width, int* surface_height) const
   { return map()->get_surface_size(surface_width, surface_height); }
   virtual double actual_rotation(void) const
   { return map()->actual_rotation(); }
   virtual int actual_zoom_percent(void) const
   { return map()->actual_zoom_percent(); }
   virtual int get_actual_center(d_geo_t* center) const
   { return map()->get_actual_center(center); }
   virtual degrees_t actual_center_lat(void) const
   { return map()->actual_center_lat(); }
   virtual degrees_t actual_center_lon(void) const
   { return map()->actual_center_lon(); }
   void get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const
   { map()->get_vsurface_size(virtual_surface_width, virtual_surface_height); }
   int vwidth(void) const
   { return map()->vwidth(); }
   int vheight(void) const
   { return map()->vheight(); }
   int get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const
   { return map()->get_vmap_bounds(map_ll, map_ur); }
   virtual d_geo_t vmap_sw_corner(void) const
   { return map()->vmap_sw_corner(); }
   virtual d_geo_t vmap_nw_corner(void) const
   { return map()->vmap_nw_corner(); }
   virtual d_geo_t vmap_ne_corner(void) const
   { return map()->vmap_ne_corner(); }
   virtual d_geo_t vmap_se_corner(void) const
   { return map()->vmap_se_corner(); }
   virtual degrees_t vmap_southern_bound(void) const
   { return map()->vmap_southern_bound(); }
   virtual degrees_t vmap_northern_bound(void) const
   { return map()->vmap_northern_bound(); }
   virtual degrees_t vmap_western_bound(void) const
   { return map()->vmap_western_bound(); }
   virtual degrees_t vmap_eastern_bound(void) const
   { return map()->vmap_eastern_bound(); }
   int get_vmap_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
      degrees_t *degrees_lon_per_pixel) const
   {
      return map()->get_vmap_degrees_per_pixel(degrees_lat_per_pixel,
         degrees_lon_per_pixel); 
   }
   virtual degrees_t vmap_degrees_per_pixel_lat(void) const
   { return map()->vmap_degrees_per_pixel_lat(); }
   virtual degrees_t vmap_degrees_per_pixel_lon(void) const
   { return map()->vmap_degrees_per_pixel_lon(); }
   int get_vmap_bounds_at_pixel_edges(d_geo_t *map_ll, d_geo_t *map_ur) const
   { return map()->get_vmap_bounds_at_pixel_edges(map_ll, map_ur); }
   int get_pixels_around_world(int *x_pixels_around_world) const
   { return map()->get_pixels_around_world(x_pixels_around_world); }
   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      int *x_coord, int *y_coord) const
   { map()->geo_to_vsurface(latitude, longitude, x_coord, y_coord); }
   void geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
      degrees_t ur_lat, degrees_t ur_lon,
      int *ul_x, int *ul_y, int *lr_x, int *lr_y) const
   { 
      map()->geo_to_vsurface_rect(ll_lat, ll_lon, ur_lat, ur_lon,
         ul_x, ul_y, lr_x, lr_y);
   }
   void vsurface_to_geo(int x_coord, int y_coord,
      degrees_t *latitude, degrees_t *longitude) const
   { map()->vsurface_to_geo(x_coord, y_coord, latitude, longitude); }
   void vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
      degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const
   {
      map()->vsurface_to_geo_rect(ul_x, ul_y, lr_x, lr_y,
         ll_lat, ll_lon, ur_lat, ur_lon);
   }
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      int *surface_x, int *surface_y) const
   { return map()->geo_to_surface(latitude, longitude, surface_x, surface_y); }
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      double *surface_x, double *surface_y) const
   { return map()->geo_to_surface(latitude, longitude, surface_x, surface_y); }
   int surface_to_geo(int surface_x, int surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   { return map()->surface_to_geo(surface_x, surface_y, latitude, longitude); }
   int surface_to_geo(double surface_x, double surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   { return map()->surface_to_geo(surface_x, surface_y, latitude, longitude); }
   boolean_t geo_in_surface(double lat, double lon) const
   { return map()->geo_in_surface(lat, lon); }
   boolean_t geo_in_surface(double lat, double lon, int* s_x, int* s_y) const
   { return map()->geo_in_surface(lat, lon, s_x, s_y); }
   boolean_t point_in_surface(int x, int y) const
   { return map()->point_in_surface(x, y); }
   int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const
   { return map()->vsurface_to_surface(vs_x, vs_y, s_x, s_y); }
   int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const
   { return map()->vsurface_to_surface(vs_x, vs_y, s_x, s_y); }
   int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const
   { return map()->surface_to_vsurface(s_x, s_y, vs_x, vs_y); }
   int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const
   { return map()->surface_to_vsurface(s_x, s_y, vs_x, vs_y); }
   BOOL geoline_to_surface(double dLat1, double dLon1, double dLat2, double dLon2,
      int* x1, int* y1, int* x2, int* y2, int* wrapped_x1, int* wrapped_y1, int* wrapped_x2, int* wrapped_y2)
   { return map()->geoline_to_surface(dLat1, dLon1, dLat2, dLon2, x1, y1, x2, y2, wrapped_x1, wrapped_y1, wrapped_x2, wrapped_y2); }

   int get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence)
   {
      return map()->get_meridian_covergence( geo_pos, convergence );  
   }

   virtual boolean_t is_to_scale_projection(void) const
   { return map()->is_to_scale_projection(); }
   virtual double to_scale_surface_width_in_inches(void) const
   { return map()->to_scale_surface_width_in_inches(); }
   virtual double to_scale_surface_height_in_inches(void) const
   { return map()->to_scale_surface_height_in_inches(); }
   virtual int surface_scale_percent(void) const
   { return map()->surface_scale_percent(); }
   virtual double surface_scale_denom(void) const
   { return map()->surface_scale_denom(); }

   virtual boolean_t is_mosaic_map_component(void) const
   {
      return map()->is_mosaic_map_component();
   }
   virtual const MapProj* get_mosaic_map(void) const
   {
      return map()->get_mosaic_map();
   }
   virtual int component_map_row(void) const
   {
      return map()->component_map_row();
   }
   virtual int component_map_column(void) const
   {
      return map()->component_map_column();
   }
   virtual int mosaic_surface_to_component_surface(int mosaic_x, int mosaic_y,
      int* component_x, int* component_y) const
   {
      return map()->mosaic_surface_to_component_surface(mosaic_x, mosaic_y,
         component_x, component_y);
   }
   virtual double component_map_vertical_offset_in_inches(void) const
   {
      return map()->component_map_vertical_offset_in_inches();
   }
   virtual double component_map_horizontal_offset_in_inches(void) const
   {
      return map()->component_map_horizontal_offset_in_inches();
   }
   virtual int mosaic_physical_to_component_physical(int mosaic_x, int mosaic_y,
      double component_map_print_width_in_inches, 
      double component_map_print_height_in_inches,
      double component_map_print_width_in_pixels, 
      double component_map_print_height_in_pixels,
      int* component_x, int* component_y) const
   {
      return map()->mosaic_physical_to_component_physical(mosaic_x, mosaic_y,
         component_map_print_width_in_inches, component_map_print_height_in_inches,
         component_map_print_width_in_pixels, component_map_print_height_in_pixels,
         component_x, component_y);
   }

   //
   // ViewMapProj extensions
   //

   CWnd* get_CView(void) const
   {
      ASSERT(m_view != NULL);
      return m_view;
   }

   //
   // SettableMapProj overrides
   //
   virtual int set_spec(const MapSpec& spec)
   {
      return map()->set_spec(spec);
   }
   virtual int set_surface_size(int surface_width, int surface_height)
   {
      return map()->set_surface_size(surface_width, surface_height);
   }
   virtual int bind_equal_arc_map_to_surface(void)
   {
      return map()->bind_equal_arc_map_to_surface();
   }

private:

   CWnd* m_view;

   SettableMapProj* map(void) const 
   {
      ASSERT(m_map);
      return m_map;
   }

public:
   SettableMapProj *m_map;

public:
   void set_map(SettableMapProj *map) 
   {
      m_map = map;
   }
   ISettableMapProj *GetSettableMapProj() 
   {
      ASSERT(map());
      return map()->m_map;
   }
};


class ActiveMap_TMP : public ActiveMap
{
   CDC m_dc;
public:

   ActiveMap_TMP(IActiveMapProj *pActiveMap);
   ActiveMap_TMP() { m_map = NULL; m_interface_ptr = NULL; }

   ~ActiveMap_TMP();

   ISettableMapProjPtr m_map;
   ISettableMapProj2Ptr m_map2;

   MapSpec m_map_spec;

   virtual CDC* get_CDC();

   //
   // MapProj wrappers


   //

   virtual boolean_t is_projection_set(void) const
   {
      int is_set;
      m_map->is_projection_set(&is_set);
      return is_set;
   }
   virtual boolean_t is_spec_set(void) const
   {
      int is_set;
      m_map->is_spec_set(&is_set);
      return is_set;
   }
   virtual boolean_t is_surface_set(void) const
   {
      int is_set;
      m_map->is_surface_set(&is_set);
      return is_set;
   }
   d_geo_t requested_center(void) const 
   {
      d_geo_t center;
      m_map->requested_center(&center.lat, &center.lon);
      return center;
   }
   MapSource source(void) const 
   {
      _bstr_t src;
      m_map->source(src.GetAddress());
      MapSource ret = MapSource(src);
      return ret;
   }
   MapSource actual_source(void) const
   {
      _bstr_t actual_src = m_map2->get_actual_source();
      return MapSource(actual_src);
   }
   MapScale scale(void) const 
   {
      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      m_map->scale(&dScale, &eScaleUnits);

      MapScale::unit_t units;
      switch(eScaleUnits)
      {
      case MAP_SCALE_WORLD: 
         return MapScale(WORLD);
         break;

      case MAP_SCALE_DENOMINATOR:
         return MapScale(static_cast<int>(dScale));
         break;

      case MAP_SCALE_NM: 
         units = MapScale::NM; 
         break;

      case MAP_SCALE_MILE: 
         units = MapScale::MILE; 
         break;

      case MAP_SCALE_KILOMETER: 
         units = MapScale::KILOMETER; 
         break;

      case MAP_SCALE_METERS: 
         units = MapScale::METERS; 
         break;

      case MAP_SCALE_YARDS: 
         units = MapScale::YARDS;
         break;

      case MAP_SCALE_FEET: 
         units = MapScale::FEET; 
         break;

      case MAP_SCALE_INCHES: 
         units = MapScale::INCHES; 
         break;

      case MAP_SCALE_ARC_DEGREES: 
         units = MapScale::ARC_DEGREES; 
         break;

      case MAP_SCALE_ARC_MINUTES: 
         units = MapScale::ARC_MINUTES; 
         break;

      case MAP_SCALE_ARC_SECONDS: 
         units = MapScale::ARC_SECONDS; 
         break;

      default:
         // Unsupported values in the class
         units = MapScale::ARC_DEGREES;
         ASSERT(0);
      }

      return MapScale(dScale, units);
   }
   MapSeries series(void) const
   {
      _bstr_t series_str;
      m_map->series(series_str.GetAddress());
      MapSeries ret = MapSeries(series_str);
      return ret;
   }

   // RMP - 12/17/99
   ProjectionEnum projection_type(void) const
   {
      return actual_projection();
   }

   ProjectionEnum actual_projection(void) const
   {
      ProjectionEnum proj;
      m_map->actual_projection(&proj);
      return proj;
   }

   ProjectionEnum requested_projection(void) const
   {
      ProjectionEnum proj;
      m_map->requested_projection(&proj);
      return proj;
   }

   double requested_rotation(void) const 
   {
      double rot;
      m_map->requested_rotation(&rot);
      return rot;
   }
   int requested_zoom_percent(void) const 
   {
      int zoom;
      m_map->requested_zoom_percent(&zoom);
      return zoom;
   }
   degrees_t requested_center_lat(void) const 
   {
      double lat;
      m_map->requested_center_lat(&lat);
      return lat;
   }
   degrees_t requested_center_lon(void) const 
   {
      double lon;
      m_map->requested_center_lon(&lon);
      return lon;
   }
   const MapSpec* spec(void) const 
   {
      return &m_map_spec;
   }

   int get_surface_width(void) const 
   {
      int width;
      m_map->get_surface_width(&width);
      return width;
   }
   int get_surface_height(void) const 
   {
      int height;
      m_map->get_surface_height(&height);
      return height;
   }
   virtual int get_surface_size(int* surface_width, int* surface_height) const
   {
      int result;
      m_map->get_surface_size(surface_width, surface_height, &result);
      return result;
   }
   virtual double actual_rotation(void) const
   {
      double rot;
      m_map->actual_rotation(&rot);
      return rot;
   }
   virtual int actual_zoom_percent(void) const
   {
      int zoom;
      m_map->actual_zoom_percent(&zoom);
      return zoom;
   }
   virtual int get_actual_center(d_geo_t* center) const
   {
      int result;
      m_map->get_actual_center(&center->lat, &center->lon, &result);
      return result;
   }
   virtual degrees_t actual_center_lat(void) const
   {
      double lat;
      m_map->actual_center_lat(&lat);
      return lat;
   }
   virtual degrees_t actual_center_lon(void) const
   {
      double lon;
      m_map->actual_center_lon(&lon);
      return lon;
   }
   void get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const
   {
      m_map->get_vsurface_size(virtual_surface_width, virtual_surface_height,
         EQUALARC_VSURFACE);
   }
   int vwidth(void) const
   {
      int vwidth;
      m_map->vwidth(&vwidth, EQUALARC_VSURFACE);
      return vwidth;
   }
   int vheight(void) const
   {
      int vheight;
      m_map->vheight(&vheight, EQUALARC_VSURFACE);
      return vheight;
   }
   int get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const
   {
      int result;
      m_map->get_vmap_bounds(&map_ll->lat, &map_ll->lon, &map_ur->lat, &map_ur->lon,
         &result, EQUALARC_VSURFACE);
      return result;
   }
   virtual d_geo_t vmap_sw_corner(void) const
   {
      d_geo_t corner;
      m_map->vmap_sw_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
      return corner;
   }
   virtual d_geo_t vmap_nw_corner(void) const
   {
      d_geo_t corner;
      m_map->vmap_nw_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
      return corner;
   }
   virtual d_geo_t vmap_ne_corner(void) const
   {
      d_geo_t corner;
      m_map->vmap_ne_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
      return corner;
   }
   virtual d_geo_t vmap_se_corner(void) const
   {
      d_geo_t corner;
      m_map->vmap_se_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
      return corner;
   }
   virtual degrees_t vmap_southern_bound(void) const
   {
      double bound;
      m_map->vmap_southern_bound(&bound, EQUALARC_VSURFACE);
      return bound;
   }
   virtual degrees_t vmap_northern_bound(void) const
   {
      double bound;
      m_map->vmap_northern_bound(&bound, EQUALARC_VSURFACE);
      return bound;
   }
   virtual degrees_t vmap_western_bound(void) const
   {
      double bound;
      m_map->vmap_western_bound(&bound, EQUALARC_VSURFACE);
      return bound;
   }
   virtual degrees_t vmap_eastern_bound(void) const
   {
      double bound;
      m_map->vmap_eastern_bound(&bound, EQUALARC_VSURFACE);
      return bound;
   }
   int get_vmap_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
      degrees_t *degrees_lon_per_pixel) const
   {
      int result;
      m_map->get_vmap_degrees_per_pixel(degrees_lat_per_pixel,
         degrees_lon_per_pixel, &result, EQUALARC_VSURFACE);
      return result;
   }
   virtual degrees_t vmap_degrees_per_pixel_lat(void) const
   {
      double dpp;
      m_map->vmap_degrees_per_pixel_lat(&dpp, EQUALARC_VSURFACE);
      return dpp;
   }
   virtual degrees_t vmap_degrees_per_pixel_lon(void) const
   {
      double dpp;
      m_map->vmap_degrees_per_pixel_lon(&dpp, EQUALARC_VSURFACE);
      return dpp;
   }
   int get_vmap_bounds_at_pixel_edges(d_geo_t *map_ll, d_geo_t *map_ur) const
   {
      int result;
      m_map->get_vmap_bounds_at_pixel_edges(&map_ll->lat, &map_ll->lon, &map_ur->lat,
         &map_ur->lon, &result, EQUALARC_VSURFACE);
      return result;
   }
   int get_pixels_around_world(int *x_pixels_around_world) const
   {
      int result;
      m_map->get_pixels_around_world(x_pixels_around_world, &result);
      return result;
   }
   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      int *x_coord, int *y_coord) const
   {
      m_map->geo_to_vsurface(latitude, longitude, x_coord, y_coord, EQUALARC_VSURFACE);
   }
   void geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
      degrees_t ur_lat, degrees_t ur_lon,
      int *ul_x, int *ul_y, int *lr_x, int *lr_y) const
   {
      m_map->geo_to_vsurface_rect(ll_lat, ll_lon, ur_lat, ur_lon, ul_x, ul_y,
         lr_x, lr_y, EQUALARC_VSURFACE);
   }
   void vsurface_to_geo(int x_coord, int y_coord,
      degrees_t *latitude, degrees_t *longitude) const
   {
      m_map->vsurface_to_geo(x_coord, y_coord, latitude, longitude, EQUALARC_VSURFACE);
   }
   void vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
      degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const
   {
      m_map->vsurface_to_geo_rect(ul_x, ul_y, lr_x, lr_y, ll_lat, ll_lon, ur_lat, ur_lon,
         EQUALARC_VSURFACE);
   }
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      int *surface_x, int *surface_y) const
   {
      int result;
      m_map->geo_to_surface(latitude, longitude, surface_x, surface_y, &result);

      if (result == NONVISIBLE_RESULT)
      {
         *surface_x = -2147483647;
         *surface_y = -2147483647;
      }

      return result == FAILURE ? FAILURE : SUCCESS;
   }
   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      double *surface_x, double *surface_y) const
   {
      int result;
      m_map->geo_to_surface_dbl(latitude, longitude, surface_x, surface_y, &result);

      // FalconView is no checking the NONVISIBLE_RESULT
      return result == FAILURE ? FAILURE : SUCCESS;
   }
   int surface_to_geo(int surface_x, int surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   {
      int result;
      m_map->surface_to_geo(surface_x, surface_y, latitude, longitude, &result);
      return result;
   }
   int surface_to_geo(double surface_x, double surface_y,
      degrees_t *latitude, degrees_t *longitude) const
   {
      int result;
      m_map->surface_to_geo_dbl(surface_x, surface_y, latitude, longitude, &result);
      return result;
   }
   boolean_t geo_in_surface(double lat, double lon) const
   {
      int in_surface;
      m_map->geo_in_surface(lat, lon, &in_surface);
      return in_surface;
   }
   boolean_t geo_in_surface(double lat, double lon, int* s_x, int* s_y) const
   {
      int in_surface;
      m_map->geo_in_surface_scr(lat, lon, s_x, s_y, &in_surface);
      return in_surface;
   }
   boolean_t point_in_surface(int x, int y) const
   {
      int in_surface;
      m_map->point_in_surface(x, y, &in_surface);
      return in_surface;
   }
   int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const
   {
      int result;
      m_map->vsurface_to_surface(vs_x, vs_y, s_x, s_y, &result, EQUALARC_VSURFACE);
      return result;
   }
   int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const
   {
      int result;
      m_map->vsurface_to_surface_dbl(vs_x, vs_y, s_x, s_y, &result, EQUALARC_VSURFACE);
      return result;
   }
   int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const
   {
      int result;
      m_map->surface_to_vsurface(s_x, s_y, vs_x, vs_y, &result, EQUALARC_VSURFACE);
      return result;
   }
   int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const
   {
      int result;
      m_map->surface_to_vsurface_dbl(s_x, s_y, vs_x, vs_y, &result, EQUALARC_VSURFACE);
      return result;
   }
   BOOL geoline_to_surface(double dLat1, double dLon1, double dLat2, double dLon2,
      int* x1, int* y1, int* x2, int* y2, int* wrapped_x1, int* wrapped_y1, int* wrapped_x2, int* wrapped_y2)
   {
      return m_map2->geoline_to_surface(dLat1, dLon1, dLat2, dLon2,
         x1, y1, x2, y2, wrapped_x1, wrapped_y1, wrapped_x2, wrapped_y2);
   }

   int get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence)
   {
      int result;
      m_map->get_meridian_covergence(geo_pos.lat, geo_pos.lon, convergence, &result);
      return result;
   }

   virtual boolean_t is_to_scale_projection(void) const
   {
      int to_scale;
      m_map->is_to_scale_projection(&to_scale);
      return to_scale;
   }
   virtual double to_scale_surface_width_in_inches(void) const
   {
      double width;
      m_map->to_scale_surface_width_in_inches(&width);
      return width;
   }
   virtual double to_scale_surface_height_in_inches(void) const
   {
      double height;
      m_map->to_scale_surface_height_in_inches(&height);
      return height;
   }
   virtual int surface_scale_percent(void) const
   {
      int percent;
      m_map->surface_scale_percent(&percent);
      return percent;
   }
   virtual double surface_scale_denom(void) const
   {
      double scale;
      m_map->surface_scale_denom(&scale);
      return scale;
   }

   //
   // Mosaic map functions
   //

   virtual boolean_t is_mosaic_map_component(void) const { return FALSE; }
   virtual const MapProj* get_mosaic_map(void) const { return NULL; }
   virtual int component_map_row(void) const { return 0; }
   virtual int component_map_column(void) const { return 0; }
   virtual int mosaic_surface_to_component_surface(int mosaic_x, int mosaic_y,
      int* component_x, int* component_y) const { return FAILURE; }
   virtual double component_map_vertical_offset_in_inches(void) const { return 0.0; }
   virtual double component_map_horizontal_offset_in_inches(void) const { return 0.0; }
   virtual int mosaic_physical_to_component_physical(int mosaic_x, int mosaic_y,
      double component_map_print_width_in_inches, 
      double component_map_print_height_in_inches,
      double component_map_print_width_in_pixels, 
      double component_map_print_height_in_pixels,
      int* component_x, int* component_y) const { return FAILURE; }
};

/////////////////////////////////////////////////////////////////////////////////////
// map functions that don't rely on or set any state information 
/////////////////////////////////////////////////////////////////////////////////////

// Given the center latitude and the size of a square pixel in meters, this
// function returns the degrees per pixel values for data with the given 
// resolution in an equal arc projection.
int MAP_resolution_to_degrees(degrees_t center_lat, double resolution_meters, 
   degrees_t &degrees_lat_per_pixel, degrees_t &degrees_lon_per_pixel);

// Given the center latitude and the desired MapScale, this function returns
// the nominal degrees per pixel values for an equal arc projection map.  This
// function will not work for WORLD scale, MAP_get_degrees_per_pixel_world.
int MAP_get_degrees_per_pixel_monitor(degrees_t center_lat, const MapScale &scale,
   degrees_t &degrees_lat_per_pixel, degrees_t &degrees_lon_per_pixel);

// World Overview is a special case where the degrees per pixel values are a
// direct function of the surface dimensions in pixels.  The center latitude
// will always be 0.0.  Since a world overview map will stretch or squash a
// world wide data set so that it fills the surface, the MapScale value of
// the data being displayed is irrelevant.
int MAP_get_degrees_per_pixel_world(int surface_width, int surface_height,
   degrees_t &degrees_lat_per_pixel, degrees_t &degrees_lon_per_pixel);


#endif