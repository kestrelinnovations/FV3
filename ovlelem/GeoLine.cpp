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



// GeoLine.cpp

#include "stdafx.h"

#include "bldlist.h"
#include "ovlutil.h"     // get_geo_line_coordinates
#include "..\proj\Projectors.h"
#include "ovl_mgr.h"

// Constructors - by default geographic lines have a BRIGHT_WHITE,
// 1 pixel wide solid foreground line with no background.  Both ends
// of the line are set to -90.0, -180.0.
GeoLine::GeoLine(heading_type_t heading_type) : GeoSegment(heading_type)
{
   m_start.lat = -90.0;
   m_start.lon = -180.0;
   m_end.lat = 90.0;
   m_end.lon = 180.0;
   m_start_radius = -1;
   m_end_radius = -1;
   m_clip_line = TRUE;
}

// Destructor
GeoLine::~GeoLine()
{
   // clear poly list
   while (!m_embedded_poly_list.IsEmpty())
      delete m_embedded_poly_list.RemoveTail();
}


// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int GeoLine::prepare_for_redraw(MapProj* map)
{
   if (map == NULL)
      return FAILURE;

   d_geo_t ll, ur;            // virtual map's geo bounds
   int pixels_around_world;   // x pixels around the world
   int width, height;         // virtual map's pixel dimentions
   d_geo_t start, end;        // copy of m_start and m_end
   boolean_t no_line, vertical_line;

   // use the map bounds to clip the line
   if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
   {
      ERR_report("get_vmap_bounds");
      return FAILURE;
   }

   // pixels around the world is used to determine if the line could
   // potentially run off one edge of the screen and come back on the other
   if (map->get_pixels_around_world(&pixels_around_world) != SUCCESS)
   {
      ERR_report("get_pixels_around_world");
      return FAILURE;
   }

   // the map width in pixels is also needed for the line wrap test
   map->get_vsurface_size(&width, &height);

   // delete all points in m_point_list
   delete_points();

   // don't allow redraw, unless this function completes successfully
   m_allow_redraw = FALSE;

   // copy starting and ending points into local variables, so they can be
   // clipped
   start = m_start;
   end = m_end;

   no_line = FALSE;

   // kludge to fix problems with vertical lines
   vertical_line = FALSE;
   if (start.lon == end.lon)
   {
      vertical_line = TRUE;
      if (end.lon < 179.9999)
         end.lon += 0.0000000001;
      else
      {
         start.lon -= 0.0000000001;
         end.lon -= 0.0000000002;
      }
   }

   if (m_clip_line)
   {
      // clip this geo line
      boolean_t dummy;
      MAP_clip_geo_line(map, start, end,
         (m_heading_type == RHUMB_LINE ? 0: 1), &dummy);


      // clip the end points according to starting and ending radius
      if (clip_end_points(map, start, end, no_line) != SUCCESS)
      {
        ERR_report("clip_end_points() failed.");
        return FAILURE;
      }
   }

   // if clipping endpoint results in no line being drawn, then return
   if (no_line)
   {
     m_allow_redraw = TRUE;
     return SUCCESS;
   }

   // check for reversed direction line
   if (GEO_east_of_degrees(start.lon, end.lon))
      m_reversed = TRUE;
   else
      m_reversed = FALSE;

   int status;

   int ltype = get_heading_type();


   // there is an unresolved problem with verticle lines of complex style
   // below is a kludge the draws vertical lines as simple lines if the
   // projection permits

   boolean_t proj_ok;

   // if the projection has straight meridians simple line kludge is ok
   const ProjectionEnum proj_type = map->actual_projection();

   proj_ok = FALSE;
   if ((proj_type == EQUALARC_PROJECTION) ||
      (proj_type == LAMBERT_PROJECTION) ||
      (proj_type == MERCATOR_PROJECTION))
   {
      proj_ok = TRUE;
   }

   if (((ltype == GREAT_CIRCLE) || (ltype == RHUMB_LINE)) &&
      vertical_line && proj_ok)
   {
      // draw vertical great circle and rhump lines as simple lines
      status = prepare_for_redraw_simple(map, ll, ur, start, end,
                               (width >= pixels_around_world/2));
   }
   else
   {
      switch (ltype)
      {
        case GREAT_CIRCLE:
           status = prepare_for_redraw_great_circle(map, ll, ur, start, end,
                                  (width >= pixels_around_world/2));
           break;
        case RHUMB_LINE:
           status = prepare_for_redraw_rhumb_line(map, ll, ur, start, end,
                                  (width >= pixels_around_world/2));
           break;
        default:
           status = prepare_for_redraw_simple(map, ll, ur, start, end,
                                  (width >= pixels_around_world/2));
           break;
      }
   }

   // reverse the points if necessary
   if (m_reversed)
      reverse_points();

   m_allow_redraw = (status == SUCCESS);

   return status;
}

int GeoLine::view_draw(map_projection_utils::CMapProjWrapper* pMap,
   gfx::GraphicsContextWrapper* gc)
{
   // if the embedded text field is empty or the number or geo-coords
   // is greater than two, then pass control to base and draw a regular
   // line
   if (m_embedded_text.IsEmpty())
      return OvlContour::view_draw(pMap, gc);

   const int line_type = (get_heading_type() == GREAT_CIRCLE) ? 3 : 2;
   return draw_line_with_embedded_text(pMap, gc,
      m_start.lat, m_start.lon, m_end.lat, m_end.lon, line_type);
}

// Uses the same information used by the hit_test(), invalidate(), and
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int GeoLine::view_draw(MapProj* map, CDC* dc)
{
   // if the embedded text field is empty or the number or geo-coords
   // is greater than two, then pass control to base and draw a regular
   // line
   if (m_embedded_text.IsEmpty())
      return OvlContour::view_draw(map, dc);

   // draw the geo-line with embedded text
   //
   const int line_type = (get_heading_type() == GREAT_CIRCLE) ? 3 : 2;
   return draw_line_with_embedded_text(map, dc, m_start.lat, m_start.lon,
      m_end.lat, m_end.lon, line_type);
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void GeoLine::invalidate(boolean_t erase_background)
{
   // if the embedded text field is empty or the number or geo-coords
   // is greater than two, then pass control to base and invalidate
   // a regular line
   if (m_embedded_text.IsEmpty())
   {
      OvlContour::invalidate();
      return;
   }

   // if there is text but the bounding poly of the text has not yet been
   // computed, then we will need to invalidate the entire screen.  Better, but
   // currently more difficult, would be to compute the bounding poly of the
   // text in prepare_for_redraw
   if (m_embedded_poly_list.GetCount() == 0)
   {
      OVL_get_overlay_manager()->invalidate_all();
      return;
   }

   COvlkitUtil util;

   CView* view;
   ViewMapProj* map;

   view = UTL_get_active_non_printing_view();
   if (view == NULL)
      return;
   map = UTL_get_current_view_map(view);
   if (map == NULL)
      return;

   // get line attributes
   COLORREF color;
   int style, width;
   m_pen.get_foreground_pen(color, style, width);

   // invalidate the line
   const int line_type = (get_heading_type() == GREAT_CIRCLE) ?
      UTIL_LINE_TYPE_GREAT : UTIL_LINE_TYPE_RHUMB;

   util.invalidate_geo_line(reinterpret_cast<MapProj*>(map),
         m_start.lat, m_start.lon, m_end.lat, m_end.lon,
         width, line_type);

   // invalidate embedded text
   util.invalidate_poly_list(m_embedded_poly_list);
   util.clear_poly_list(m_embedded_poly_list);

   if (m_point_list.GetCount() == 2)
   {
      util.invalidate_line(
         m_point_list.GetHead()->x, m_point_list.GetHead()->y,
         m_point_list.GetTail()->x, m_point_list.GetTail()->y);
   }
}

// set the flag the tells the routines whether or not to clip the geo line\
// returns previous state of clip mode
BOOL GeoLine::set_clip_mode(BOOL clip)
{
   BOOL old_mode = m_clip_line;

   m_clip_line = clip;
   return old_mode;
}


// *************************************************************
// *************************************************************

int GeoLine::prepare_for_redraw_great_circle(MapProj* map,
   d_geo_t map_ll, d_geo_t map_ur,
   d_geo_t start, d_geo_t end, boolean_t test_for_wrap)
{
   if (map == NULL)
      return FAILURE;

   int start_flags;           // NSEW bounds check flags for point 1
   int end_flags;             // NSEW bounds check flags for point 2
   int and_flags;             // start_flags & end_flags
   boolean_t east_of;         // m_end is east of m_start

   // class that builds a CList of CPoints for a great circle list.
   GreatCirclePointList list(m_point_list, m_allow_redraw);

   // handle potential wrap around the world case separately
   if (test_for_wrap)
   {
      int x1, y1;    // device coordinates of western point
      int x2, y2;    // device coordinates of eastern point

      // get device coordinates
      if (GEO_east_of_degrees(end.lon, start.lon))
      {
         map->geo_to_vsurface(start.lat, start.lon, &x1, &y1);
         map->geo_to_vsurface(end.lat, end.lon, &x2, &y2);

         // need to draw the line from west to east, start to end
         east_of = TRUE;
      }
      else  // end is west of start or start.lon == end.lon
      {
         map->geo_to_vsurface(end.lat, end.lon, &x1, &y1);
         map->geo_to_vsurface(start.lat, start.lon, &x2, &y2);

         // need to draw the line from west to east, end to start
         east_of = FALSE;
      }

      // if end is east of start
      if (east_of)
      {
         // Draw the great circle arc from start to end.
         // (x1 > x2) means the arc wraps around off one edge and back onto the
         // other.
         return list.generate_points(map, map_ll, map_ur, start, end,
            x1 > x2, m_clip_line);
      }
      else  // end is west of start or start.lon == end.lon
      {
         // Draw the great circle arc from end to start.
         // (x1 > x2) means the arc wraps around off one edge and back onto the
         // other.
         return list.generate_points(map, map_ll, map_ur, end, start,
            x1 > x2, m_clip_line);
      }
   }  // end potential wrap around the world case

   // get NSEW bounds check flags for each point
   start_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
      map_ur.lat, map_ur.lon, start.lat, start.lon);
   end_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
      map_ur.lat, map_ur.lon, end.lat, end.lon);

   // quick accept - both points on map
   if ((start_flags | end_flags) == 0)
   {
      // if end is east of start
      if (GEO_east_of_degrees(end.lon, start.lon))
      {
         // Draw the great circle arc from start to end.
         return list.generate_points(map, map_ll, map_ur, start, end, FALSE,
            m_clip_line);
      }
      else  // end is west of start or start.lon == end.lon
      {
         // Draw the great circle arc from end to start.
         return list.generate_points(map, map_ll, map_ur, end, start, FALSE,
            m_clip_line);
      }
   }  // end of quick accept - both points on map

   // compute intersection of flags to do clipping
   and_flags = start_flags & end_flags;

   // quick reject - both end points are east of or west of the map so the line
   // won't cross the map window

   if (m_clip_line)
   {
      if (and_flags & (GEO_EAST_OF | GEO_WEST_OF))
      {
        m_allow_redraw = TRUE;
        return SUCCESS;
      }

      // both end points are north of the map window, requires additional
      // testing for great circle lines
      if (and_flags & GEO_NORTH_OF)
      {
        // if top of map is above the equator, then no part of the great circle
        // line will cross the map
        if (map_ur.lat >= 0.0)
        {
          m_allow_redraw = TRUE;
          return SUCCESS;
        }
      }

      // both end points are south of the map window, requires additional
      // testing for great circle lines
      if (and_flags & GEO_SOUTH_OF)
      {
         // if bottom of map is below the equator, then no part of the great
         // circle line will cross the map
         if (map_ll.lat <= 0.0)
         {
            m_allow_redraw = TRUE;
            return SUCCESS;
         }
      }
   }

   // reject lines that go from west of map to east of map but
   // not through the region defined by left_lon and right_lon
   if (((start_flags ^ end_flags) & (GEO_EAST_OF | GEO_WEST_OF)) ==
      (GEO_EAST_OF | GEO_WEST_OF))
   {
      if (start_flags & GEO_EAST_OF)
      {
         if (GEO_east_of_degrees(end.lon, start.lon) && m_clip_line)
         {
            m_allow_redraw = TRUE;
            return SUCCESS;
         }

         // needed to get device coordinates in right order
         east_of = FALSE;
      }
      else
      {
         if (GEO_east_of_degrees(start.lon, end.lon) && m_clip_line)
         {
            m_allow_redraw = TRUE;
            return SUCCESS;
         }

         // needed to get device coordinates in right order
         east_of = TRUE;
      }
   }
   else
      east_of = GEO_east_of_degrees(end.lon, start.lon);

   // if end is east of start
   if (east_of)
   {
      // Draw the great circle arc from start to end.
      return list.generate_points(map, map_ll, map_ur, start, end, FALSE,
         m_clip_line);
   }
   else  // end is west of start or start.lon == end.lon
   {
      // Draw the great circle arc from end to start.
      return list.generate_points(map, map_ll, map_ur, end, start, FALSE,
         m_clip_line);
   }
}
// end of prepare_for_redraw_great_circle

// *************************************************************
// *************************************************************

int GeoLine::prepare_for_redraw_rhumb_line(MapProj* map,
   d_geo_t map_ll, d_geo_t map_ur,
   d_geo_t start, d_geo_t end, boolean_t test_for_wrap)
{
   if (map == NULL)
      return FAILURE;

   RhumbLineClipper line(start, end);

   if (m_clip_line)
   {
      // set the clip bounds to the map bounds
      if (line.set_clip_bounds(map_ll, map_ur) != SUCCESS)
      {
        ERR_report("set_clip_bounds() failed.");
        return FAILURE;
      }
   }

   if (line.generate_view_points(map, m_point_list, m_clip_line) != SUCCESS)
   {
      ERR_report("generate_view_points() failed.");
      return FAILURE;
   }

   m_allow_redraw = TRUE;
   return SUCCESS;
}

// *************************************************************
// *************************************************************

int GeoLine::prepare_for_redraw_simple(MapProj* map,
   d_geo_t map_ll, d_geo_t map_ur,
   d_geo_t start, d_geo_t end, boolean_t test_for_wrap)
{
   if (map == NULL)
      return FAILURE;

   int start_flags;           // NSEW bounds check flags for point 1
   int end_flags;             // NSEW bounds check flags for point 2
   int and_flags;             // start_flags & end_flags
   boolean_t east_of;         // m_end is east of m_start

   // handle potential wrap around the world case separately
   if (test_for_wrap)
   {
      // if end is east of start
      if (GEO_east_of_degrees(end.lon, start.lon))
      {
         // Draw the great circle arc from start to end.
         // (x1 > x2) means the arc wraps around off one edge and back onto the
         // other.
         return generate_simple_points(map, start, end, m_point_list);
      }
      else  // end is west of start or start.lon == end.lon
      {
         // Draw the great circle arc from end to start.
         // (x1 > x2) means the arc wraps around off one edge and back onto the
         // other.
         return generate_simple_points(map, end, start, m_point_list);
      }
   }  // end potential wrap around the world case

   // get NSEW bounds check flags for each point
   start_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
      map_ur.lat, map_ur.lon, start.lat, start.lon);
   end_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
      map_ur.lat, map_ur.lon, end.lat, end.lon);

   // quick accept - both points on map
   if ((start_flags | end_flags) == 0)
   {
      // if end is east of start
      if (GEO_east_of_degrees(end.lon, start.lon))
      {
         // Draw the simple arc from start to end.
         return generate_simple_points(map, start, end, m_point_list);
      }
      else  // end is west of start or start.lon == end.lon
      {
         // Draw the simple arc from end to start.
         return generate_simple_points(map, end, start, m_point_list);
      }
   }  // end of quick accept - both points on map

   // compute intersection of flags to do clipping
   and_flags = start_flags & end_flags;

   // quick reject - both end points are east of or west of the map so the line
   // won't cross the map window

   if (m_clip_line)
   {
      if (and_flags & (GEO_EAST_OF | GEO_WEST_OF))
      {
        m_allow_redraw = TRUE;
        return SUCCESS;
      }

      // both end points are north of the map window, requires additional
      // testing for great circle lines
      if (and_flags & GEO_NORTH_OF)
      {
        // if top of map is above the equator, then no part of the great circle
        // line will cross the map
        if (map_ur.lat >= 0.0)
        {
          m_allow_redraw = TRUE;
          return SUCCESS;
        }
      }

      // both end points are south of the map window, requires additional
      // testing for great circle lines
      if (and_flags & GEO_SOUTH_OF)
      {
         // if bottom of map is below the equator, then no part of the great
         // circle line will cross the map
         if (map_ll.lat <= 0.0)
         {
            m_allow_redraw = TRUE;
            return SUCCESS;
         }
      }
   }

   // reject lines that go from west of map to east of map but
   // not through the region defined by left_lon and right_lon
   if (((start_flags ^ end_flags) & (GEO_EAST_OF | GEO_WEST_OF)) ==
      (GEO_EAST_OF | GEO_WEST_OF))
   {
      if (start_flags & GEO_EAST_OF)
      {
         if (GEO_east_of_degrees(end.lon, start.lon) && m_clip_line)
         {
            m_allow_redraw = TRUE;
            return SUCCESS;
         }

         // needed to get device coordinates in right order
         east_of = FALSE;
      }
      else
      {
         if (GEO_east_of_degrees(start.lon, end.lon) && m_clip_line)
         {
            m_allow_redraw = TRUE;
            return SUCCESS;
         }

         // needed to get device coordinates in right order
         east_of = TRUE;
      }
   }
   else
      east_of = GEO_east_of_degrees(end.lon, start.lon);

   // if end is east of start
   if (east_of)
   {
      // Draw the great circle arc from start to end.
      return generate_simple_points(map, start, end, m_point_list);
   }
   else  // end is west of start or start.lon == end.lon
   {
      // Draw the great circle arc from end to start.
      return generate_simple_points(map, end, start, m_point_list);
   }
}
// end of prepare_for_redraw_simple

int GeoLine::generate_simple_points(MapProj* map, d_geo_t p1, d_geo_t p2,
   CList <CPoint *, CPoint *>& point_list)
{
   MAP_geo_line_points line_points;
   MAP_calc_geo_line(map, p1.lat, p1.lon, p2.lat, p2.lon, &line_points);
   const int line_count = line_points.num_lines();

   //
   // draw the next segment along the great circle line
   //
   // 0, 1, or 2 lines were drawn.
   // Always add all points to the point list, eventhough this will usually
   // mean each point is in the list twice.  Doing it this way will handle
   // all special cases in a straight forward way.
   //
   if (line_count == 1)
   {
      int x1, y1, x2, y2;

      line_points.get_saved_line(0, &x1, &y1, &x2, &y2);
      point_list.AddTail(new CPoint(x1, y1));
      point_list.AddTail(new CPoint(x2, y2));
   }
   else if (line_count == 2)
   {
      int a_x1, a_y1, a_x2, a_y2;

      line_points.get_saved_line(0, &a_x1, &a_y1, &a_x2, &a_y2);
      int b_x1, b_y1, b_x2, b_y2;
      line_points.get_saved_line(1, &b_x1, &b_y1, &b_x2, &b_y2);
      point_list.AddTail(new CPoint(a_x1, a_y1));
      point_list.AddTail(new CPoint(a_x2, a_y2));
      point_list.AddTail(new CPoint(b_x1, b_y1));
      point_list.AddTail(new CPoint(b_x2, b_y2));
   }

   return SUCCESS;
}

// *************************************************************
// *************************************************************

// Returns the length of this segment in meters.
double GeoLine::get_length()
{
   double range, bearing;

   // calculate range and bearing to end point
   if (GEO_calc_range_and_bearing(m_start, m_end, range, bearing,
      get_heading_type() == GREAT_CIRCLE) != SUCCESS)
      return -1.0;

   return range;
}

// If 0 <= distance <= get_length() then lat and lon will be set to the
// point at that distance from the start of the segment, heading
// will be set to the heading at that point along the segment, and
// the function will return TRUE.  Otherwise FALSE is returned and
// lat, lon, and heading are unchanged.  distance is in meters.
boolean_t GeoLine::get_point(double distance, d_geo_t &point,
                             degrees_t &heading)
{
   boolean_t great_circle = (get_heading_type() == GREAT_CIRCLE);
   double range;

   // calculate range and bearing to end point
   if (GEO_calc_range_and_bearing(m_start, m_end, range, heading,
      great_circle) != SUCCESS)
      return FALSE;

   // if distance is out of range
   // (distance - range > 0.000001) is used in place of (distance > range)
   // to handle rounding errors.
   if (distance < 0.0 || distance - range > 0.000001)
      return FALSE;

   // intermediate point is the start point
   if (distance == 0.0)
   {
      point = m_start;
      return TRUE;
   }

   // (range - distance > 0.000001) is used in place of (distance < range),
   // because distances very close to range can cause GEO_calc_r_and_b to
   // return range == 0.0 and heading == 0.0

   // if intermediate point is NOT the end point, then calculate it
   if (range - distance > 0.000001) //(distance < range)
   {
      // calculate intermediate point
      if (GEO_calc_end_point(m_start, distance, heading, point, great_circle)
         != SUCCESS)
         return FALSE;

      // if GREAT_CIRCLE, compute the heading at the intermediate point
      // RHUMB_LINE has a constant heading
      if (great_circle)
      {
         if (GEO_calc_range_and_bearing(point, m_end, range, heading, TRUE) !=
            SUCCESS)
            return FALSE;
      }
   }
   else
   {
      point = m_end;

      // if GREAT_CIRCLE, compute the heading at the intermediate point
      // RHUMB_LINE has a constant heading
      if (great_circle)
      {
         if (GEO_calc_range_and_bearing(
            m_end, m_start, range, heading, TRUE) != SUCCESS)
         {
            return FALSE;
         }

         heading += 180.0;
         if (heading >= 360.0)
            heading -= 360.0;
      }
   }

   return TRUE;
}

// Get the shortest distance from the point to this segment.  The returned
// distance will be in meters.  right_of_course will be TRUE if the point is
// right of course, FALSE otherwise.
// The return values indicates if the point is along, before, or after
// the segment.
// +1 - after
//  0 - along
// -1 - before
// start_to_closest is the distance in meters along the GeoLine from the
// closest point to the start of the GeoLine, where the closest point is
// the point "distance" meters away from "point".
int GeoLine::get_distance(d_geo_t point, double &distance,
   boolean_t &right_of_course, double &start_to_closest)
{
   int status = 0;
   double range, bearing;
   double distance1, angle1;
   double distance2, angle2;
   d_geo_t closest;

   // initialize return values to defaults in case of a failure
   distance = WGS84_a_METERS * PI;  // half way around the world
   start_to_closest = distance;
   right_of_course = FALSE;

   if (!GEO_valid_degrees(point.lat, point.lon))
      return status;

   // calculations differ for great-circle vs rhumb line routes
   boolean_t great_circle = (get_heading_type() == GeoSegment::GREAT_CIRCLE);

   // Get the distance from the starting point to the test point and the angle
   // between the line from end to start and the line from start to point.
   if (GEO_calc_lengths_and_angle_ex(get_end(), get_start(), point,
      great_circle, range, distance1, angle1) != SUCCESS)
      return status;

   // Get the distance from the ending point to the test point and the angle
   // between the line from start to end and the line from end to point.
   if (GEO_calc_lengths_and_angle_ex(get_start(), get_end(), point,
      great_circle, range, distance2, angle2) != SUCCESS)
      return status;

   // If the point is before the start of the line then the magnitude of angle1
   // will be more than 90.0 degrees.
   if (angle1 >= 90.0 || angle1 <= -90.0)
   {
      // Since the world is round it is possible for a point to be both before
      // and after a line.  In this case the distance to the closer of the two
      // end points will be used.
      if (distance1 < distance2)
      {
         distance = distance1;
         start_to_closest = 0.0;
         right_of_course = (angle1 < 0.0);

         // If the straight line from start to point is not the perpendicular,
         // then the point is before the line, otherwise the point is on the
         // line (status defaults to 0).
         if (angle1 > 90.0 || angle1 < -90.0)
            status = -1;

         return status;
      }
   }

   // If the point is after the end of the line then the magnitude of angle2
   // will be more than 90.0 degrees.
   if (angle2 >= 90.0 || angle2 <= -90.0)
   {
      distance = distance2;
      start_to_closest = get_length();
      right_of_course = (angle2 > 0.0);

      // If the straight line from end to point is not the perpendicular,
      // then the point is after the line, otherwise the point is on the
      // line (status defaults to 0).
      if (angle2 > 90.0 || angle2 < -90.0)
         status = 1;

      return status;
   }

   // If you are exactly on course then angle1 should be 0.0
   if (angle1 == 0.0)
   {
      distance = 0.0;
      start_to_closest = distance1;

      return status;
   }

   // If the line has zero length.
   if (range == 0.0)
   {
      distance = distance1;
      start_to_closest = distance;

      return status;
   }

   // If you get here the perpendicular will intersect the line from start
   // to end between start and end.  The point of intersection will be the
   // closest point.  When you have the closest point the angle between the
   // line from closest to start and from closest and point will be 90.0
   // degrees.  closest will be along the line from start to end at some
   // distance less that the length of the line and at a bearing equal to
   // the bearing from start to end.

   // The sign of angle1 determines if you are right or left of course.
   right_of_course = (angle1 < 0.0);

   // Get the bearing from start to end.
   GEO_calc_range_and_bearing(get_start(), get_end(), range, bearing,
      great_circle);

   // The initial guess at the range to the closest point from start.
   range = distance1 * cos(DEG_TO_RAD(angle1));

   // Find the perpendicular distance by finding the point along the line,
   // where the angle between the line from closest to start and the line
   // from closest to point is 90.0 degrees (within some tolerance).
   int i = 0;
   do
   {
      // Get the next guess at the closest point.
      GEO_calc_end_point(get_start(), range, bearing, closest, great_circle);

      // Get the the angle between the line from closest to start and the line
      // from closest to point.
      GEO_calc_lengths_and_angle(get_start(), closest, point, great_circle,
         distance2, distance1, angle1);

      // if the distance from closest to point is a minimum, use it
      if (distance > distance1)
      {
         distance = distance1;
         start_to_closest = range;
      }

      // if angle1 is close enough to 90.0 or we have gone through the loop
      // too many times, just take what you have and be done with it
      if ((89.99999 < angle1 && angle1 < 90.00001) || i > 10)
      {
         //TRACE("Breaking on iteration i=%d, angle1=%0.6lf\n", i, angle1);
         break;
      }

      // if the angle is more than 90.0, closest is further away from start
      if (angle1 > 90.0)
      {
         range += distance1 * cos(DEG_TO_RAD(180.0 - angle1));
      }
      // if the angle is less than 90.0, closest is closer to start
      else if (angle1 < 90.0)
      {
         range -= distance1 * cos(DEG_TO_RAD(angle1));
      }

      i++;

   } while (1);

   return status;
}

// Set the starting point of the great circle line.
int GeoLine::set_start(degrees_t start_lat, degrees_t start_lon)
{
   if (!GEO_valid_degrees(start_lat, start_lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   m_start.lat = start_lat;
   m_start.lon = start_lon;

   // cause redraw to call draw, since end point has changed
   m_allow_redraw = FALSE;

   return SUCCESS;
}

// Set the ending point of the great circle line.
int GeoLine::set_end(degrees_t end_lat, degrees_t end_lon)
{
   if (!GEO_valid_degrees(end_lat, end_lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   m_end.lat = end_lat;
   m_end.lon = end_lon;

   // cause redraw to call draw, since end point has changed
   m_allow_redraw = FALSE;

   return SUCCESS;
}

// The great circle line will be clipped against a circle of the given
// radius centered at the starting point.  Valid radii range from 1 to
// 50 pixels.  Use -1 for no clipping.
int GeoLine::clip_start(int clip_radius)
{
   // check for valid clip radius
   if (clip_radius < -1 || clip_radius > 50 || clip_radius == 0)
   {
      ERR_report("Invalid clip radius.");
      return FAILURE;
   }

   // don't do anything if this value is not different
   if (clip_radius != m_start_radius)
   {
      m_start_radius = clip_radius;

      // cause redraw to call draw, since the actual starting point has changed
      m_allow_redraw = FALSE;
   }

   return SUCCESS;
}

// The great circle line will be clipped against a circle of the given
// radius centered at the ending point.  Valid radii range from 1 to
// 50 pixels.  Use -1 for no clipping.
int GeoLine::clip_end(int clip_radius)
{
   // check for valid clip radius
   if (clip_radius < -1 || clip_radius > 50 || clip_radius == 0)
   {
      ERR_report("Invalid clip radius.");
      return FAILURE;
   }

   // don't do anything if this value is not different
   if (clip_radius != m_end_radius)
   {
      m_end_radius = clip_radius;

      // cause redraw to call draw, since the actual ending point has changed
      m_allow_redraw = FALSE;
   }

   return SUCCESS;
}

// Clip the end points according to the starting and ending radii.  Input
// a copy of m_start and m_end so their values can be preserved.  Returns
// no_line set to TRUE if the line disappeared as a result of clipping.
int GeoLine::clip_end_points(MapProj* map, d_geo_t &start, d_geo_t &end,
   boolean_t &no_line)
{
   degrees_t degrees_lat_per_pixel;
   degrees_t degrees_lon_per_pixel;
   double range;
   double bearing;
   double radius;
   double angle;
   d_geo_t point;
   int center_x, center_y;
   int point_x, point_y;
   int length;
   CFvwUtil *util = CFvwUtil::get_instance();

   // get the degrees per pixel in both directions
   if (map->get_vmap_degrees_per_pixel(&degrees_lat_per_pixel,
      &degrees_lon_per_pixel) != SUCCESS)
   {
      ERR_report("get_vmap_degrees_per_pixel() failed.");
      return FAILURE;
   }

   // latitudes at +/-90 break rhumb line range and bearing calculations
   // because the poles are at infinity in the Mercator projection
   boolean_t start_clip_enabled = TRUE;
   boolean_t end_clip_enabled = TRUE;
   if (get_heading_type() == RHUMB_LINE)
   {
      // if the starting lat is close enough to a pole to get fudged,
      // then we need to draw this rhumb-line as a line of longitude.
      if (GEO_fudge_polar_lat_for_rhumb_line(start.lat))
      {
         start.lon = end.lon;
         start_clip_enabled = FALSE;
      }
      // if the ending lat is close enough to a pole to get fudged,
      // then we need to draw this rhumb-line as a line of longitude.
      if (GEO_fudge_polar_lat_for_rhumb_line(end.lat))
      {
         end.lon = start.lon;
         end_clip_enabled = FALSE;
      }
   }

   // if the starting and ending points are the same, then this is a NULL line
   if (start.lat == end.lat && start.lon == end.lon)
   {
      no_line = TRUE;
      return SUCCESS;
   }

   // if the line is clipped at the starting point
   if (start_clip_enabled && m_start_radius > 0 &&
      map->geo_in_surface(start.lat, start.lon))
   {
      // get range and bearing from start to end
      if (calc_range_and_bearing(start, end, range, bearing) != SUCCESS)
      {
         ERR_report("calc_range_and_bearing() failed.");
         return FAILURE;
      }

      // need angle in radians for the cliping radius
      angle = DEG_TO_RAD(bearing);

      // compute the latitude from the radius and the angle
      point.lat = start.lat +
         (double)m_start_radius * cos(angle) * degrees_lat_per_pixel;
      if (point.lat > 90.0)
         point.lat = 90.0;
      else if (point.lat < -90.0)
         point.lat = -90.0;

      // compute the longitude from the radius and the angle
      point.lon = start.lon +
         (double)m_start_radius * sin(angle) * degrees_lon_per_pixel;
      if (point.lon > 180.0)
         point.lon -= 360.0;
      else if (point.lon < -180.0)
         point.lon += 360.0;

      // use the distance to this point as a measure of the length of the
      // radius in meters
      if (calc_range_and_bearing(start, point, radius, angle) != SUCCESS)
      {
         ERR_report("calc_range_and_bearing() failed.");
         return FAILURE;
      }

      // if clipping the line will make it disappear, then don't draw the line
      if (radius >= range)
      {
         no_line = TRUE;
         return SUCCESS;
      }

      // move this distance along the line from start to end
      if (calc_end_point(start, radius, bearing, point) != SUCCESS)
      {
         ERR_report("calc_end_point() failed.");
         return FAILURE;
      }

      // compute the length of the clipped segment in pixels
      map->geo_to_surface(start.lat, start.lon, &center_x, &center_y);
      map->geo_to_surface(point.lat, point.lon, &point_x, &point_y);
      length = util->magnitude(center_x, center_y, point_x, point_y);

      // make sure the new point is no more than m_start_radius pixels away
      // from the old point
      while (length > m_start_radius)
      {
         // decrease radius to try a closer lat-lon
         radius = radius * 0.95;

         if (calc_end_point(start, radius, bearing, point) != SUCCESS)
         {
            ERR_report("calc_end_point() failed.");
            return FAILURE;
         }

         // compute the length of the clipped segment in pixels
         map->geo_to_surface(start.lat, start.lon, &center_x, &center_y);
         map->geo_to_surface(point.lat, point.lon, &point_x, &point_y);
         length = util->magnitude(center_x, center_y, point_x, point_y);
      }

      // save lat and lon of new starting point
      start.lat = point.lat;
      start.lon = point.lon;
   }

   // if the line is clipped at the ending point
   if (end_clip_enabled && m_end_radius > 0 &&
      map->geo_in_surface(end.lat, end.lon))
   {
      // get range and bearing from end to start
      if (calc_range_and_bearing(end, start, range, bearing) != SUCCESS)
      {
         ERR_report("calc_range_and_bearing() failed.");
         return FAILURE;
      }

      // need angle in radians for the cliping radius
      angle = DEG_TO_RAD(bearing);

      // compute the latitude from the radius and the angle
      point.lat = end.lat +
         (double)m_end_radius * cos(angle) * degrees_lat_per_pixel;
      if (point.lat > 90.0)
         point.lat = 90.0;
      else if (point.lat < -90.0)
         point.lat = -90.0;

      // compute the longitude from the radius and the angle
      point.lon = end.lon +
         (double)m_end_radius * sin(angle) * degrees_lon_per_pixel;
      if (point.lon > 180.0)
         point.lon -= 360.0;
      else if (point.lon < -180.0)
         point.lon += 360.0;

      // use the distance to this point as a measure of the length of the
      // radius in meters
      if (calc_range_and_bearing(end, point, radius, angle) != SUCCESS)
      {
         ERR_report("calc_range_and_bearing() failed.");
         return FAILURE;
      }

      // if clipping the line will make it disappear, then don't draw the line
      if (radius >= range)
      {
         no_line = TRUE;
         return SUCCESS;
      }

      // move this distance along the great circle from end to start
      if (calc_end_point(end, radius, bearing, point) != SUCCESS)
      {
         ERR_report("calc_end_point() failed.");
         return FAILURE;
      }

      // compute the length of the clipped segment in pixels
      map->geo_to_surface(end.lat, end.lon, &center_x, &center_y);
      map->geo_to_surface(point.lat, point.lon, &point_x, &point_y);
      length = util->magnitude(center_x, center_y, point_x, point_y);

      // make sure the new point is no more than m_end_radius pixels away
      // from the old point
      while (length > m_end_radius)
      {
         // decrease radius to try a closer lat-lon
         radius = radius * 0.95;

         if (calc_end_point(end, radius, bearing, point)
            != SUCCESS)
         {
            ERR_report("calc_end_point() failed.");
            return FAILURE;
         }

         // compute the length of the clipped segment in pixels
         map->geo_to_surface(end.lat, end.lon, &center_x, &center_y);
         map->geo_to_surface(point.lat, point.lon, &point_x, &point_y);
         length = util->magnitude(center_x, center_y, point_x, point_y);
      }

      // save lat and lon of new ending point
      end.lat = point.lat;
      end.lon = point.lon;
   }


   // if the starting and ending points are the same, then this is a NULL line
   no_line = (start.lat == end.lat && start.lon == end.lon);

   return SUCCESS;
}

boolean_t GeoLine::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "GeoLine") == 0)
      return TRUE;

   return GeoSegment::is_kind_of(class_name);
}

// returns TRUE if the ending point is east of the starting point
boolean_t GeoLine::is_east_of(void)
{
   degrees_t lat, start_lon, end_lon;
   get_start(lat, start_lon);
   get_end(lat, end_lon);
   return GEO_east_of_degrees(end_lon, start_lon);
}
