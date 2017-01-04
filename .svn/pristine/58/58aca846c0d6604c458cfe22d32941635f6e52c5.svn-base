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

#ifndef MAPS_H
#define MAPS_H 1

/*------------------------------------------------------------------
-                            Includes
- -----------------------------------------------------------------*/

#include <afxcoll.h> // for CStringList
#include "common.h"
#include "geo_tool/geo_tool_d.h"
#include "maps_d.h"


/*------------------------------------------------------------------
- Center-to-bounds return codes
------------------------------------------------------------------*/

#define WINDOW_PAST_NORTH_POLE -10
#define WINDOW_PAST_SOUTH_POLE -20 
#define WINDOW_MORE_THAN_360_DEGREES -30
#define INVALID_WINDOW_CENTER -1 
#define INVALID_WINDOW_UR -2
#define INVALID_WINDOW_LL -3
#define INVALID_WINDOW_LAT -4
#define INVALID_WINDOW_LON -5
#define WINDOW_BOUNDS_SUCCESS 0 


/*------------------------------------------------------------------
-                            Typedefs 
- -----------------------------------------------------------------*/

// forward declarations
class CMapType;
class DataSource;
class CovListSingleSource;
class ActiveMap;
interface IDrawingToolsProjection;
class MapProj;
class SettableMapProj;

//
// for use with MAP_calc_geo_line
//
class MAP_geo_line_points
{

public:
   // Constructor
   MAP_geo_line_points();

   void reset(void);

   // returns 0, 1 or 2
   int num_lines(void) const;

   // line_index must be 0 or 1
   int save_line(int line_index, int x1, int y1, int x2, int y2);

   // line_index must be 0 or 1
   int get_saved_line(int line_index, int* x1, int* y1, int* x2, 
      int* y2) const;

   // This allows you to access the values in m_saved_points.  If the index
   // is negative or if it is greater than or equal to 4 * num_lines(), this
   // function returns -1.
   // The order of the x and y values in m_saved_points is: x1, y1, x2, y2,
   // x3, y3, x4, y4.
   int operator[] (int index);

private:

   int m_num_lines;
   int m_saved_points[8];
};


//
// the following types describe submaps and pages which can contain multiple
// submaps
//

typedef enum {PAGE_ORIENTATION_LANDSCAPE, 
              PAGE_ORIENTATION_PORTRAIT,
              PAGE_ORIENTATION_DEFAULT} page_orientation_t;

//
// defines a submap on a page
//
class subpage_map_t
{

public:

   subpage_map_t(const SettableMapProj* subpage_map,
      double subpage_width_in_inches, double subpage_height_in_inches,
      double subpage_offset_from_top_in_inches, 
      double subpage_offset_from_left_in_inches) :
         m_subpage_map(subpage_map), 
         m_subpage_width_in_inches(subpage_width_in_inches),
         m_subpage_height_in_inches(subpage_height_in_inches),
         m_subpage_offset_from_top_in_inches(subpage_offset_from_top_in_inches),
         m_subpage_offset_from_left_in_inches(subpage_offset_from_left_in_inches)
   {
      m_print_map_type_and_scale = FALSE;
      m_print_chart_series_and_date = FALSE;
      m_print_date_and_time = FALSE;
      m_print_echum_currency = FALSE;
      m_print_dafif_currency = FALSE;
      m_print_cadrg_currency = FALSE;

      m_print_compass_rose = FALSE;
      m_print_compass_rose_when_not_north_up = FALSE;
   }

   const SettableMapProj* subpage_map(void) const 
   {
      return m_subpage_map;
   }

   double m_subpage_width_in_inches;
   double m_subpage_height_in_inches;
   double m_subpage_offset_from_top_in_inches;
   double m_subpage_offset_from_left_in_inches;

   boolean_t m_print_map_type_and_scale;
   boolean_t m_print_chart_series_and_date;
   boolean_t m_print_date_and_time;
   boolean_t m_print_echum_currency;
   boolean_t m_print_dafif_currency;
   boolean_t m_print_cadrg_currency;
   boolean_t m_print_compass_rose;
   boolean_t m_print_compass_rose_when_not_north_up;

private:

   const SettableMapProj* m_subpage_map;
};

//
// A list of all of the submaps on a page. Also includes the page orientation.
//
class subpage_map_list_t : public CList<subpage_map_t*, subpage_map_t*>
{

public:

   subpage_map_list_t()
   {
      m_page_orientation = PAGE_ORIENTATION_DEFAULT;
   }
   subpage_map_list_t(page_orientation_t page_orientation)
   {
      m_page_orientation = page_orientation;
   }
   virtual ~subpage_map_list_t() {}

   page_orientation_t page_orientation(void) const
   {
      return m_page_orientation;
   }

private:

   page_orientation_t m_page_orientation;
};

//
// A list of pages
//
typedef CList<subpage_map_list_t*, subpage_map_list_t*> page_map_list_t;

/*------------------------------------------------------------------
-                           Functions 
- -----------------------------------------------------------------*/

/* Set up map library. Must be called ONCE at start up before any other
   MAP_*() functions are called. */

int MAP_open(void);


/* Clean up after the map library. Must be called ONCE before exit if
   the map library was used. */

int MAP_close(void);

boolean_t MAP_zoom_percent_valid(const MapScale& scale);

int MAP_calc_screen_size_for_a_rotated_map(int surface_width, int surface_height,
   int* vwidth, int* vheight, double rotation);

/* get map category from source */

MapCategory MAP_get_category(MapSource source);

// RE-ARCH
// Copies all of the MapType objects in the input category into map_list.
// map_list must be empty when it is passed into this function.  The MapType
// pointers placed in map_list are the actual MapType objects used inside of
// the maps library.  No new MapTypes are created by this function.  When
// map_list is destroyed, it should simply be emptied.  It's elements MUST
// NOT be deleted.  Note the default behavior for ~CList is to delete the list
// itself but not delete any of the data stored in it's elements, which is the
// behavior expected by this function.
int MAP_get_map_list(MapCategory category, 
                     CList<MapType *, MapType *> &map_list,
                     BOOL available_maps_only = FALSE);

// Copies all of the MapType objects with the given source into map_list.
// map_list must be empty when it is passed into this function.  The MapType
// pointers placed in map_list are the actual MapType objects used inside of
// the maps library.  No new MapTypes are created by this function.  When
// map_list is destroyed, it should simply be emptied.  It's elements MUST
// NOT be deleted.  Note the default behavior for ~CList is to delete the list
// itself but not delete any of the data stored in it's elements, which is the
// behavior expected by this function.
int MAP_get_map_list(const MapSource &source, 
   CList<MapType *, MapType *> &map_list);

// Copies all of the MapType objects with the given source and series into map_list.
// map_list must be empty when it is passed into this function.  The MapType
// pointers placed in map_list are the actual MapType objects used inside of
// the maps library.  No new MapTypes are created by this function.  When
// map_list is destroyed, it should simply be emptied.  It's elements MUST
// NOT be deleted.  Note the default behavior for ~CList is to delete the list
// itself but not delete any of the data stored in it's elements, which is the
// behavior expected by this function.
int MAP_get_map_list(const MapSource &source, const MapSeries &series, 
                     CList<MapType *, MapType *> &map_list);

// Copyies all of the MapType objects available into the map_list from
// every category
int MAP_get_map_list(CList<MapType *, MapType *> &map_list);

// Returns the MapType object that has the given source, scale, and series.
// Returns NULL for failure.
MapType *MAP_get_map_type(const MapSource &source, const MapScale &scale, 
   const MapSeries &series);

/* get map category as a string, from the category */

CString MAP_get_category_string(MapCategory category);

/* get map category as a string, from the source */

CString MAP_get_category_string(MapSource source);

/* get map source as a string */

CString MAP_get_source_string(MapSource source);

/* get map scale as a string */

CString MAP_get_scale_string(MapScale scale);

/* get map scale from a string */

MapScale MAP_get_scale_from_string(CString this_scale);

// Converts source dependent scale string into a scale.  You need this function
// so "100 meter" can be converted to DTED_SCALE.

MapScale MAP_get_scale_from_string(MapSource source, CString scale);

/* Returns TRUE if rotation is valid for the given map scale,
   and FALSE otherwise.  If the scale is WORLD then it returns
   FALSE.  If the virtual map size is larger than the data set,
   then the function returns FALSE. */

boolean_t MAP_rotation_valid(MapScale scale, int surface_width, int surface_height);

// Tests the given scale, surface_width, and surface_heights for limits:
//    -valid_surface is false if the surface size is too large for this scale
//    -can_rotate is false if the rotation must be 0
//    -can_zoom is false if the zoom percent must be 100
//    -minimum_zoom will be between 1 and 100 
int MAP_test_limits(const MapScale &scale, int surface_width, int surface_height,
   bool &valid_surface, bool &can_rotate, bool &can_zoom, int &minimum_zoom);

// Tests to see if a to-scale map is possible for the given source, scale,
// series, and center on a width_inches x height_inches surface.
//    -valid_surface is false if the surface size is too large for this scale
//    -can_rotate is false if the rotation must be 0
//    -can_zoom is false if the zoom percent must be 100
//    -minimum_zoom will be between 1 and 100
// This function will fail if scale == WORLD.
int MAP_test_limits(const MapSource &source, const MapScale &scale, 
   const MapSeries &series, degrees_t center_lat, degrees_t center_lon,
   double width_inches, double height_inches,
   bool &valid_surface, bool &can_rotate, bool &can_zoom, int &minimum_zoom);

//
// These functions fetch DTED elevations and should only be used for
// infrequent requests (e.g. for dted polling).
//
// First NIMA DTED is searched to find the desired elevation.  If it is not
// found, CMS DTED is searched.
//
// These functions return SUCCESS, FAILURE, or FV_NO_DATA (if no dted
// data is available at the requested elevation).
//
int DTD_get_elevation_in_meters(degrees_t lat, degrees_t lon, INT2* elev);
int DTD_get_elevation_in_feet(degrees_t lat, degrees_t lon, INT4* elev, long *vert_accuracy_ft = NULL);

/*------------------------------------------------------------------
-  FUNCTION NAME: MAP_calc_geo_line
-
-  PURPOSE:
-
-      Computes the surface points for the shortest line between 
-  points, delta longitude < 180.0 degrees.  These points can then
-  later be used for drawing a geo line.
-
-  DESCRIPTION:
-
-      Lines are checked to see if they are off the screen completely,
-  as this test saves a lot of usless computation when the line is
-  off the screen and it adds little overhead. For non-clipped lines
-  both points are converted to screen coordinates such that point 1
-  is west of point 2. Except when the line goes off the left edge of
-  the screen if point 1 is west of point 2 then x1 < x2. This exception
-  must be handled separately as explained below. In order to determine
-  what line is to be drawn without requiring the points to be ordered
-  east to west it is necessary to assume that the desired line is the
-  line which covers less than 180 degrees of longitude. 
-------------------------------------------------------------------*/
//
// line_points will contain the points in surface coordinates, not in
// vsurface coordinates
//
int MAP_calc_geo_line(IDrawingToolsProjection* pmap, degrees_t lat1, degrees_t lon1, 
   degrees_t lat2, degrees_t lon2, MAP_geo_line_points* line_points);

// Clip a geo line represented by <geo_from, geo_to>.
int MAP_clip_geo_line(MapProj *pmap, d_geo_t &geo_from, d_geo_t &geo_to, 
                      boolean_t great_circle_not_rhumb_line, boolean_t *clipped);


int MAP_get_surface_size_in_logical_units_for_a_to_scale_surface(
   double surface_width_in_inches, double surface_height_in_inches,
   degrees_t center_lat, degrees_t center_lon, double rotation_angle, 
   degrees_t unzoomed_degrees_per_pixel_lat, degrees_t unzoomed_degrees_per_pixel_lon,
   double surface_scale_denom, int* surface_width_in_logical_units,
   int* surface_height_in_logical_units);
int MAP_scale_t_to_scale_denominator(MapScale s, double* d);

//
// converts a scale denominator to a string of the form,
// for example, "1:3,333,333"
//
CString MAP_convert_scale_denominator_to_scale_string(int scale_denom);


// compute the center of the bounding rectangle
int MAP_bounds_to_center(d_geo_t map_ll, d_geo_t map_ur, d_geo_t *center);
int MAP_center_to_bounds(d_geo_t center, 
   degrees_t deg_lat_per_pix, degrees_t deg_lon_per_pix,
   int screen_width, int screen_height, d_geo_t *map_ll, d_geo_t *map_ur);

// Read a MapCategory from the registry.
MapCategory MAP_get_registry_category(const char* section, const char* value_name, 
   const MapCategory &default_category = NULL_CATEGORY);

// Write a MapCategory to the registry.
int MAP_set_registry_category(const char* section, const char* value_name, 
   const MapCategory &category);

// Read a MapSource from the registry.
MapSource MAP_get_registry_source(const char* section, const char* value_name, 
   const MapSource &default_source = NULL_SOURCE);

// Write a MapSource to the registry.
int MAP_set_registry_source(const char* section, const char* value_name, 
   const MapSource &source);

// Read a MapScale from the registry.
MapScale MAP_get_registry_scale(const char* section, const char* value_name, 
   const MapScale &default_scale = NULL_SCALE);

// Write a MapScale to the registry.
int MAP_set_registry_scale(const char* section, const char* value_name, 
   const MapScale &scale);

// Read a MapSeries from the registry.
MapSeries MAP_get_registry_series(const char* section, const char* value_name, 
   const MapSeries &default_series = NULL_SERIES);

// Write a MapSeries to the registry.
int MAP_set_registry_series(const char* section, const char* value_name, 
   const MapSeries &series);

// Read a projection_t (ProjectionID) from the registry.
ProjectionEnum MAP_get_registry_projection(const char* section, const char* value_name, 
   const ProjectionEnum &def_proj_type/* = NULL_SOURCE*/);

// Write a projection_t (ProjectionID) to the registry.
int MAP_set_registry_projection(const char* section, const char* value_name, 
   const ProjectionEnum &proj_type);

#endif