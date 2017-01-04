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

#ifndef MAP_H
#define MAP_H

#include "common.h"
#include "maps_d.h" 
#include "geo_tool/geo_tool_d.h"
#include "GenericProjector.h"
#include "FvMappingGraphics/Include/IDrawingToolsProjection.h"

// forward declarations
class MapView;
struct ISettableMapProj;
struct IActiveMapProj;

/////////////////////////////////////////////////////////////////////////////////
 
class MapProj : public CGenericProjector, public IDrawingToolsProjection
{
public:

   MapProj() {}
   virtual ~MapProj() {}

   virtual boolean_t is_projection_set(void) const = 0;
   virtual boolean_t is_spec_set(void) const = 0;
   virtual boolean_t is_surface_set(void) const = 0;

   //
   // map spec functions
   //
   virtual d_geo_t requested_center(void) const = 0; 
   virtual MapSource source(void) const = 0; 
   virtual MapSource actual_source(void) const = 0;
   virtual MapScale scale(void) const = 0;
   virtual MapSeries series(void) const = 0;
   virtual double requested_rotation(void) const = 0; 
   virtual int requested_zoom_percent(void) const = 0; 
   virtual degrees_t requested_center_lat(void) const = 0; 
   virtual degrees_t requested_center_lon(void) const = 0;
   virtual const MapSpec* spec(void) const { ASSERT(0); return NULL; }
   virtual ProjectionEnum projection_type(void) const { ASSERT(0); return actual_projection(); }

   virtual ProjectionEnum requested_projection(void) const = 0;
   virtual ProjectionEnum actual_projection(void) const = 0;


   //
   // surface functions
   //
   virtual int get_surface_width(void) const = 0;
   virtual int get_surface_height(void) const = 0;
   virtual int get_surface_size(int* surface_width, int* surface_height) const = 0;

   //
   // get the actual rotation and zoom values.  
   // the requested values for these can be adjusted when the projection is
   // set, so the actual values may not match the requested values.
   //
   virtual double actual_rotation(void) const = 0; 
   virtual int actual_zoom_percent(void) const = 0; 

   //
   // Get the actual center values.  The center lat can be adjusted to keep the map
   // within displayable bounds and thus may be different from the requested center.
   // 
   virtual int get_actual_center(d_geo_t* center) const = 0;
   virtual degrees_t actual_center_lat(void) const = 0;
   virtual degrees_t actual_center_lon(void) const = 0;

   //
   // virtual surface functions
   //
   virtual void get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const = 0;
   virtual int vwidth(void) const = 0;
   virtual int vheight(void) const = 0;

   //
   // virtual map functions
   //
   // These functions return properties for the "virtual map" or vmap.
   // The vmap is the northup map contained in the virtual surface.
   //
   // These bounds are the bounds of the pixel *centers* of the vmap
   // edge pixels, not the bounds of the pixel edges.
   //
   virtual int get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const = 0;
   virtual d_geo_t vmap_sw_corner(void) const = 0;
   virtual d_geo_t vmap_nw_corner(void) const = 0;
   virtual d_geo_t vmap_ne_corner(void) const = 0;
   virtual d_geo_t vmap_se_corner(void) const = 0;
   virtual degrees_t vmap_southern_bound(void) const = 0;
   virtual degrees_t vmap_northern_bound(void) const = 0;
   virtual degrees_t vmap_western_bound(void) const = 0;
   virtual degrees_t vmap_eastern_bound(void) const = 0;
   virtual int get_vmap_degrees_per_pixel(degrees_t *vmap_degrees_lat_per_pixel,
      degrees_t *vmap_degrees_lon_per_pixel) const = 0;
   virtual degrees_t vmap_degrees_per_pixel_lat(void) const = 0;
   virtual degrees_t vmap_degrees_per_pixel_lon(void) const = 0;

   //
   // get_vmap_bounds_at_pixel_edges returns the vmap geo bounds at the pixel 
   // edges (instead of the pixel centers)
   //
   virtual int get_vmap_bounds_at_pixel_edges(d_geo_t *map_ll, d_geo_t *map_ur) const = 0;

   //
   // returns the width of the northup worldwide data set for the map
   //
   virtual int get_pixels_around_world(int *x_pixels_around_world) const = 0;

   //
   // coordinate conversion functions
   //
   virtual void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      int *x_coord, int *y_coord) const = 0;
   virtual void geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
      degrees_t ur_lat, degrees_t ur_lon,
      int *ul_x, int *ul_y, int *lr_x, int *lr_y) const = 0;
   virtual void vsurface_to_geo(int x_coord, int y_coord,
      degrees_t *latitude, degrees_t *longitude) const = 0;
   virtual void vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
      degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const = 0;
   virtual int geo_to_surface(degrees_t latitude, degrees_t longitude,
      int *surface_x, int *surface_y) const = 0;
   virtual int geo_to_surface(degrees_t latitude, degrees_t longitude,
      double *surface_x, double *surface_y) const = 0;
   virtual int surface_to_geo(int surface_x, int surface_y,
      degrees_t *latitude, degrees_t *longitude) const = 0;
   virtual int surface_to_geo(double surface_x, double surface_y,
      degrees_t *latitude, degrees_t *longitude) const = 0;
   virtual boolean_t geo_in_surface(double lat, double lon) const = 0;
   virtual boolean_t geo_in_surface(double lat, double lon, int* s_x, int* s_y) const = 0;
   virtual boolean_t point_in_surface(int x, int y) const = 0;
   virtual int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const = 0;
   virtual int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const = 0;
   virtual int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const = 0;
   virtual int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const = 0;
   virtual BOOL geoline_to_surface(double dLat1, double dLon1, double dLat2, double dLon2,
         int* x1, int* y1, int* x2, int* y2, int* wrapped_x1, int* wrapped_y1, int* wrapped_x2, int* wrapped_y2) = 0;

   virtual int get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence) = 0;

   //
   // to scale functions
   //
   virtual boolean_t is_to_scale_projection(void) const = 0;
   virtual double to_scale_surface_width_in_inches(void) const = 0;
   virtual double to_scale_surface_height_in_inches(void) const = 0;
   virtual int surface_scale_percent(void) const = 0;
   virtual double surface_scale_denom(void) const = 0;

   //
   // Mosaic map functions
   //

   virtual boolean_t is_mosaic_map_component(void) const { ASSERT(0); return FALSE; }
   virtual const MapProj* get_mosaic_map(void) const { ASSERT(0); return NULL; }
   virtual int component_map_row(void) const { ASSERT(0); return 0; }
   virtual int component_map_column(void) const { ASSERT(0); return 0; }
   virtual int mosaic_surface_to_component_surface(int mosaic_x, int mosaic_y,
      int* component_x, int* component_y) const { ASSERT(0); return 0; }
   virtual double component_map_vertical_offset_in_inches(void) const { ASSERT(0); return 0.0; }
   virtual double component_map_horizontal_offset_in_inches(void) const { ASSERT(0); return 0.0; }
   virtual int mosaic_physical_to_component_physical(int mosaic_x, int mosaic_y,
      double component_map_print_width_in_inches, 
      double component_map_print_height_in_inches,
      double component_map_print_width_in_pixels, 
      double component_map_print_height_in_pixels,
      int* component_x, int* component_y) const { ASSERT(0); return 0; }
   
   // CGenericProjector overrides
   //

   virtual bool InSurface(double dLat, double dLon, double dAltitudeMeters)
   {
      return geo_in_surface(dLat, dLon) == TRUE;
   }

   virtual int ToSurface(double dLat, double dLon, double dAltitudeMeters, int& rX, int& rY)
   {
      return geo_to_surface(dLat, dLon, &rX, &rY);
   }

   virtual double ActualRotation() 
   { 
      return actual_rotation();
   }

   virtual int GetSurfaceSize(int* pWidth, int* pHeight)
   {
      return get_surface_size(pWidth, pHeight);
   }

   virtual BOOL pole_on_surface() const ;
};

// CMapProjWrapper - wraps the ISettableMapProj interface and is used provide a legacy MapProj interface to internal overlays
//
class CMapProjWrapper : public MapProj
{
   ISettableMapProjPtr m_spSettableMapProj;
   ISettableMapProj2Ptr m_spSettableMapProj2;

public:
   // constructor
   CMapProjWrapper(ISettableMapProj *pSettableMapProj);

   ISettableMapProj* GetInterfacePtr();

public:
   boolean_t is_projection_set(void) const;

   boolean_t is_spec_set(void) const;

   boolean_t is_surface_set(void) const;

   d_geo_t requested_center(void) const;

   MapSource source(void) const;

   MapSource actual_source(void) const;
   
   MapScale scale(void) const;

   MapSeries series(void) const;

   ProjectionEnum actual_projection(void) const;

   ProjectionEnum requested_projection(void) const;

   double requested_rotation(void) const;

   int requested_zoom_percent(void) const;

   degrees_t requested_center_lat(void) const;

   degrees_t requested_center_lon(void) const;

   int get_surface_width(void) const;

   int get_surface_height(void) const;

   int get_surface_size(int* surface_width, int* surface_height) const;

   double actual_rotation(void) const;

   int actual_zoom_percent(void) const;

   int get_actual_center(d_geo_t* center) const;

   degrees_t actual_center_lat(void) const;

   degrees_t actual_center_lon(void) const;

   void get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const;

   int vwidth(void) const;

   int vheight(void) const;

   int get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const;

   d_geo_t vmap_sw_corner(void) const;

   d_geo_t vmap_nw_corner(void) const;

   d_geo_t vmap_ne_corner(void) const;

   d_geo_t vmap_se_corner(void) const;

   degrees_t vmap_southern_bound(void) const;

   degrees_t vmap_northern_bound(void) const;

   degrees_t vmap_western_bound(void) const;

   degrees_t vmap_eastern_bound(void) const;

   int get_vmap_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
      degrees_t *degrees_lon_per_pixel) const;

   degrees_t vmap_degrees_per_pixel_lat(void) const;

   degrees_t vmap_degrees_per_pixel_lon(void) const;

   int get_vmap_bounds_at_pixel_edges(d_geo_t *map_ll, d_geo_t *map_ur) const;

   int get_pixels_around_world(int *x_pixels_around_world) const;

   void geo_to_vsurface(degrees_t latitude, degrees_t longitude,
      int *x_coord, int *y_coord) const;

   void geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
      degrees_t ur_lat, degrees_t ur_lon,
      int *ul_x, int *ul_y, int *lr_x, int *lr_y) const;

   void vsurface_to_geo(int x_coord, int y_coord,
      degrees_t *latitude, degrees_t *longitude) const;

   void vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
      degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const;

   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      int *surface_x, int *surface_y) const;

   int geo_to_surface(degrees_t latitude, degrees_t longitude,
      double *surface_x, double *surface_y) const;

   int surface_to_geo(int surface_x, int surface_y,
      degrees_t *latitude, degrees_t *longitude) const;

   int surface_to_geo(double surface_x, double surface_y,
      degrees_t *latitude, degrees_t *longitude) const;

   boolean_t geo_in_surface(double lat, double lon) const;

   boolean_t geo_in_surface(double lat, double lon, int* s_x, int* s_y) const;

   boolean_t point_in_surface(int x, int y) const;

   int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const;

   int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const;

   int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const;

   int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const;

   BOOL geoline_to_surface(double dLat1, double dLon1, double dLat2, double dLon2,
         int* x1, int* y1, int* x2, int* y2, int* wrapped_x1, int* wrapped_y1, int* wrapped_x2, int* wrapped_y2);

   int get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence);

   boolean_t is_to_scale_projection(void) const;

   double to_scale_surface_width_in_inches(void) const;

   double to_scale_surface_height_in_inches(void) const;

   int surface_scale_percent(void) const;

   double surface_scale_denom(void) const;
};

class ViewMapProj : public MapProj
{

public:

   ViewMapProj() {}
   ViewMapProj(CWnd* /*view*/) {}
   ~ViewMapProj() {}

   virtual CWnd* get_CView(void) const = 0;

   virtual ISettableMapProj *GetSettableMapProj() = 0;

};

//
// a **REQUESTED** map that is the process of being drawn (i.e. being
// converted to a current map
//
// Its graphics context (i.e. DC) has been prepared and initialized.
//
// a tested map that is about to be or is in the process of being drawn
//
class ActiveMap : public MapProj
{

public:

   ActiveMap() {}

   //
   // **********************************
   // NOTE: this shouldn't really be accessible.  should be part of constructor.
   // don't want people setting this after the fact
   //
   virtual CDC* get_CDC(void) = 0;

   IActiveMapProj *m_interface_ptr;
};

#endif
