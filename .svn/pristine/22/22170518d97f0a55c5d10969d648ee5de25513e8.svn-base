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



#include "stdafx.h"
#include "map.h"
#include "geo_tool.h"
#include "errx.h"
#include "maps.h"
#include "fvwutil.h"

/////////////////////////////////////////////////////////////////////////

static int calc_line_points(IDrawingToolsProjection* map, int x1, int y1, int x2, int y2,
   int line_index, MAP_geo_line_points* line_points);

/////////////////////////////////////////////////////////////////////////

MAP_geo_line_points::MAP_geo_line_points()
{
   reset();
}

void MAP_geo_line_points::reset(void)
{
   m_num_lines = 0;
}

// returns 0, 1 or 2
int MAP_geo_line_points::num_lines(void) const 
{
   return m_num_lines;
}

// must be 0 or 1
int MAP_geo_line_points::save_line(int line_index, int x1, int y1, 
   int x2, int y2)
{
   if (line_index != 0 && line_index != 1)
   {
      ERR_report("invalid index");
      return FAILURE;
   }

   //
   // can't add line 1 before line 0 has been added
   //
   if (line_index == 1 && num_lines() != 1)
   {
      ERR_report("invalid index");
      return FAILURE;
   }

   if (line_index == 0)
   {
      m_saved_points[0] = x1;
      m_saved_points[1] = y1;
      m_saved_points[2] = x2;
      m_saved_points[3] = y2;
      m_num_lines = 1;
   }
   else if (line_index == 1)
   {
      m_saved_points[4] = x1;
      m_saved_points[5] = y1;
      m_saved_points[6] = x2;
      m_saved_points[7] = y2;
      m_num_lines = 2;
   }

   return SUCCESS;
}

int MAP_geo_line_points::get_saved_line(int line_index, int* x1, int* y1, 
   int* x2, int* y2) const
{
   if (line_index != 0 && line_index != 1)
   {
      ERR_report("invalid index");
      return FAILURE;
   }

   if (m_num_lines != 1 && num_lines() != 2)
   {
      ERR_report("invalid num lines");
      return FAILURE;
   }

   if (line_index == 0)
   {
      *x1 = m_saved_points[0];
      *y1 = m_saved_points[1];
      *x2 = m_saved_points[2];
      *y2 = m_saved_points[3];
   }
   else if (line_index == 1 && num_lines() == 2)
   {
      *x1 = m_saved_points[4];
      *y1 = m_saved_points[5];
      *x2 = m_saved_points[6];
      *y2 = m_saved_points[7];
   }
   else
   {
      ERR_report("invalid combo");
      return FAILURE;
   }

   return SUCCESS;
}

int MAP_geo_line_points::operator[] (int index)
{
   // test for completely invalid value
   if (index < 0 || index > 7)
   {
      ERR_report("index is out of range (0..7).");
      return -1;
   }

   // test for a value that is out of range according to the saved points for
   // this object
   if (index >= 4 * m_num_lines)
   {
      ERR_report("Use num_lines().");
      return -1;
   }

   return m_saved_points[index];
}

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

int MAP_calc_geo_line(IDrawingToolsProjection* map, degrees_t lat1, degrees_t lon1, 
   degrees_t lat2, degrees_t lon2, MAP_geo_line_points* line_points)
{
   int x1, y1;          /* device coordinates of western point */
   int x2, y2;          /* device coordinates of eastern point */
   int sx1, sy1;
   int sx2, sy2;
   int p1_flags;        /* NSEW bounds check flags for point 1 */
   int p2_flags;        /* NSEW bounds check flags for point 2 */
   boolean_t east_of;   /* point 2 is east of point 1 */
   int pixels_around_world;
   d_geo_t ll, ur;
   double ratio;
   int map_width;

   line_points->reset();

   // treat world overview as a special case
   
   map->get_pixels_around_world(&pixels_around_world);
   map_width = map->vwidth();
   ratio = (double) pixels_around_world / (double) map_width;
   if (ratio < 2.0)
   {
      if (GEO_east_of_degrees(lon2, lon1))
      {
		  map->geo_to_vsurface(lat1, lon1, &x1, &y1);
		  map->geo_to_vsurface(lat2, lon2, &x2, &y2);
      }
      else         /* p2 is west of p1 or lon1=lon2 */
      {
		  map->geo_to_vsurface(lat2, lon2, &x1, &y1);
		  map->geo_to_vsurface(lat1, lon1, &x2, &y2);
      }

		boolean_t polar;
		MapProj *mproj = (MapProj*) map;
		const ProjectionEnum proj_type = mproj->actual_projection();
		if ((proj_type == EQUALARC_PROJECTION) || (proj_type == MERCATOR_PROJECTION))
			polar = FALSE;
		else
			polar = map->pole_on_surface();

      if ((x1 > x2) && !polar)
      {
         // wrap around world
         double t;
         double x, y;

         x = (double)map->vwidth() - 0.5;
         t = (x - (double)x1)/(double)(x2+pixels_around_world - x1);
         y = y1 + (double)(y2-y1)*t + 0.5;
         calc_line_points(map, x1, y1, (int)x, (int)y, 0, line_points);

         x = -0.5;
         t = (x - (double)(x1 - pixels_around_world))/(double)(x2 - (x1 - pixels_around_world));
         y = y1 + (double)(y2-y1)*t + 0.5;
         calc_line_points(map, (int)x, (int)y, x2, y2, 1, line_points);
      }
      else
         calc_line_points(map, x1, y1, x2, y2, 0, line_points);

      return SUCCESS;
   }

   map->get_vmap_bounds(&ll, &ur);
   // get NSEW bounds check flags for each point   
   p1_flags = GEO_bounds_check_degrees(ll.lat, ll.lon, 
      ur.lat, ur.lon, lat1, lon1);
   p2_flags = GEO_bounds_check_degrees(ll.lat, ll.lon, 
      ur.lat, ur.lon, lat2, lon2);

   // quick accept - both points on map
   if ((p1_flags | p2_flags) == 0)
   {
      // convert the geographic coordinates to screen coordinates
      if ( map->geo_to_surface(lat1, lon1, &sx1, &sy1) == SUCCESS && 
           map->geo_to_surface(lat2, lon2, &sx2, &sy2) == SUCCESS )
      {
         return line_points->save_line(0, sx1, sy1, sx2, sy2);
      }

      return FAILURE;
   }
   
   east_of = GEO_east_of_degrees(lon2, lon1);

   // get device coordinates
   if (east_of)
   {
      map->geo_to_vsurface(lat1, lon1, &x1, &y1);
      map->geo_to_vsurface(lat2, lon2, &x2, &y2);
   }
   else
   {
      map->geo_to_vsurface(lat2, lon2, &x1, &y1);
      map->geo_to_vsurface(lat1, lon1, &x2, &y2);
   }

   if (x1 > x2)
   {
      double x = (double)map->vwidth() - 0.5;
      double t = (x - (double)x1)/(double)(x2+pixels_around_world - x1);
      double y = y1 + (double)(y2-y1)*t + 0.5;

      calc_line_points(map, x1, y1, (int)x, (int)y, 0, line_points);
   }
   else
   {
      if ( map->geo_to_surface(lat1, lon1, &sx1, &sy1) == SUCCESS && 
           map->geo_to_surface(lat2, lon2, &sx2, &sy2) == SUCCESS )
      {
         return line_points->save_line(0, sx1, sy1, sx2, sy2);
      }

      ERR_report("geo_to_surface to FAILED");
      return FAILURE;
   }

   return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////

static
int calc_line_points(IDrawingToolsProjection* map, int x1, int y1, int x2, int y2,
   int line_index, MAP_geo_line_points* line_points)
{
   int sx1, sy1;
   int sx2, sy2;

   // Always compute the view coordinates and save them so a call to
   // MAP_get_view_coordinates_of_last_line() can be used to recover
   // the view coordinates of the line drawn by the MAP_geo_do_line().
	if ( map->vsurface_to_surface(x1, y1, &sx1, &sy1) == SUCCESS && 
        map->vsurface_to_surface(x2, y2, &sx2, &sy2) == SUCCESS )
	{
      return line_points->save_line(line_index, sx1, sy1, sx2, sy2);
	}

   return FAILURE;
}

//*****************************************************************************

static
int get_meters_per_pixel(MapProj *map, double *meters_lat_per_pix, double *meters_lon_per_pix)
{
   degrees_t deg_lat_per_pix;
   degrees_t deg_lon_per_pix;
	degrees_t center_lat, center_lon;
   double kilometers;
   degrees_t angle;

	center_lat = map->actual_center_lat();
	center_lon = map->actual_center_lon();

   if (map->get_vmap_degrees_per_pixel(&deg_lat_per_pix, &deg_lon_per_pix) != SUCCESS)
   {
      ERR_report("get_vmap_degrees_per_pixel failed");
      return FAILURE;
   }

   GEO_distance(center_lat, center_lon, center_lat, center_lon+deg_lon_per_pix,
      &kilometers, &angle);
   *meters_lon_per_pix = (kilometers*1000);
   GEO_distance(center_lat, center_lon, center_lat+deg_lat_per_pix, center_lon,
      &kilometers, &angle);
   *meters_lat_per_pix = (kilometers*1000);

   return SUCCESS;
}


static 
int clip_geo_point( MapProj *map, d_geo_t &geo_from, d_geo_t &geo_to, double TOL, 
							boolean_t *clipped, boolean_t great_circle_not_rhumb_line )
{
	double distance, bearing;

	degrees_t mid_lat = geo_from.lat, mid_lon = geo_from.lon;
	double total_dist;

	GEO_calc_range_and_bearing(geo_from.lat, geo_from.lon, geo_to.lat, geo_to.lon, 
		&distance, &bearing, great_circle_not_rhumb_line);

	distance /= 2.0;
	total_dist = distance;
	
	while ( total_dist > TOL )
	{
		GEO_calc_end_point(geo_from.lat, geo_from.lon, total_dist, bearing, 
			&mid_lat, &mid_lon, great_circle_not_rhumb_line);

		// validate new point, check tol and EXIT HERE!!!!!!!!!
		if ( map->geo_in_surface(mid_lat, mid_lon) )
		{
			distance = distance/2.0;
			total_dist += distance;
			if ( distance <= TOL )
				break;
		}
		else
		{
			total_dist -= distance;
			distance = distance/2.0;
			total_dist += distance;

			if( total_dist <= TOL )
			{
				mid_lat = geo_from.lat;
				mid_lon = geo_from.lon;	
				break;
			}
		}
	}

	*clipped = TRUE;

	geo_to.lat = mid_lat;
	geo_to.lon = mid_lon;

	return SUCCESS;
}


int MAP_clip_geo_line(MapProj *map, d_geo_t &geo_from, d_geo_t &geo_to, 
							 boolean_t great_circle_not_rhumb_line, boolean_t *clipped)
{

	*clipped = FALSE;

	// 1. verify that map is valid and that the projection is set
	ASSERT( map );
	if ( map == NULL )
	{
		ERR_report( "NULL map");
		return FAILURE;
	}

	// 2. Validate each geo point
	if ( !GEO_valid_degrees(geo_from.lat, geo_from.lon) || 
						!GEO_valid_degrees(geo_to.lat, geo_to.lon ) )
	{
		ERR_report( "Invalid geo points");
		return FAILURE;
	}

	// 3. Check if the projection of the geo point to the surface is visible using b
	//    a. INVALID_POINT results from xy_to_geo()
	//		b. NONVISIBLE_RESULT results from geo_to_xy()
	
	boolean_t first_visible, second_visible;
	int dummy1, dummy2; 
	
	if ( map->geo_to_surface(geo_from.lat, geo_from.lon, &dummy1, &dummy2) == SUCCESS )
		first_visible = TRUE;
	else
		first_visible = FALSE;

	if ( map->geo_to_surface(geo_to.lat, geo_to.lon, &dummy1, &dummy2) == SUCCESS )
		second_visible = TRUE;
	else
		second_visible = FALSE;

	//	4. If both points visible, no clipping necessary thus return
	// This need to take into account that in some projections (e.g., Orhto)
	// the end points of a rhumb line can be visible, some points in between may not!
	if ( first_visible && second_visible )
	{
		//TRACE( "Both points of geoline visible\n" );
		return SUCCESS;

		double distance, bearing;
		degrees_t mid_lat, mid_lon;
		
		GEO_calc_range_and_bearing(geo_to.lat, geo_to.lon, geo_from.lat, geo_from.lon, 
			&distance, &bearing, great_circle_not_rhumb_line);

		GEO_calc_end_point(geo_to.lat, geo_to.lon, distance/2.0, bearing, 
				&mid_lat, &mid_lon, great_circle_not_rhumb_line);

		if ( !map->geo_in_surface(mid_lat, mid_lon) )
		{
			//TRACE( "Middle point of geoline NOT visible\n" );
			return SUCCESS;
		}
	}

	if ( !first_visible && !second_visible )
	{
		//TRACE( "Both points of geoline NOT visible\n" );
		return SUCCESS;
	}

	// Compute meters per pixel to set the tolerance (TOL);
	double meters_per_pixel_lat, meters_per_pixel_lon, meters_per_pixel;
	get_meters_per_pixel(map, &meters_per_pixel_lat, &meters_per_pixel_lon);
	meters_per_pixel = (meters_per_pixel_lat + meters_per_pixel_lon) / 2.0;

	// 5. design algorithm to check 1 point or 2 points (geo)
	// 6. Compute the distance from p1 to p2

	double TOL = 3.0 * meters_per_pixel;

	if ( first_visible == FALSE )
		clip_geo_point( map, geo_to, geo_from, TOL, clipped, great_circle_not_rhumb_line );

	if ( second_visible == FALSE )
		clip_geo_point( map, geo_from, geo_to, TOL, clipped, great_circle_not_rhumb_line );

	return SUCCESS;
}
