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



// GeoArc.cpp

#include "stdafx.h"
#include "ovlelem.h"
#include "err.h"
#include "map.h"
#include "maps.h"

// Constructors - by default arc have a BRIGHT_WHITE, 1 pixel wide solid
// foreground pen and no background.  Both ends of the arc are set to  
// -90.0, -180.0.  The radius is set to 0.0.  If the arc is not setup
// correctly via the define member, the draw and redraw functions will fail.
GeoArc::GeoArc(heading_type_t heading_type) : GeoSegment(heading_type)
{
   // set start, end, and center to bogus values
   m_start.lat = -90.0;
   m_start.lon = -180.0;
   m_end.lat = -90.0;
   m_end.lon = -180.0;
   m_center.lat = -90.0;
   m_center.lon = -180.0;

   // no turn arc by default
   m_radius = 0.0;

   // remaining values are just set to valid defaults
   m_clockwise = FALSE;       // counter clockwise
}

// returns the number of segments needed to approximate the arc
int GeoArc::get_number_of_segments()
{
	double heading_to_start;
   double heading_to_end;
   double arc;

	// compute the heading to m_start from m_center, the heading to m_end
   // from m_center, and the angle of the arc
   if (compute_headings_and_arc(heading_to_start, heading_to_end, arc) !=
      SUCCESS)
   {
      ERR_report("compute_headings_and_arc() failed.");
      return -1;
   }

	// compute the number of line segments used to approximate the arc
   return (int)(arc / 2.0) + 1;
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int GeoArc::prepare_for_redraw(MapProj* map)
{
	if (map == NULL)
		return FAILURE;

   d_geo_t map_ur;
   d_geo_t map_ll;
   d_geo_t bound_ur;
   d_geo_t bound_ll;
   double heading_to_start;
   double heading_to_end;
   double arc;

   // delete any points in the list(s)
   delete_points();

   // don't allow redraw, unless this function completes successfully
   m_allow_redraw = FALSE;

   // if the turn radius is 0.0 - this is a NULL arc
   if (m_radius == 0.0)
   {
      m_allow_redraw = TRUE;
      return SUCCESS;
   }
   
   // compute the heading to m_start from m_center, the heading to m_end
   // from m_center, and the angle of the arc
   if (compute_headings_and_arc(heading_to_start, heading_to_end, arc) !=
      SUCCESS)
   {
      ERR_report("compute_headings_and_arc() failed.");
      return FAILURE;
   }

   // use the map bounds to clip the arc
   if (map->get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
   {
      ERR_report("MAP_get_map_bounds() failed.");
      return FAILURE;
   }

   // Compute a bounding box for the arc to compare against the map bounds.
   // If the arc passes through 0, 90, 180, or 270, then the initial bounds
   // must be expanded to account for one or more of these.

   // get an initial estimate for the northern and southern bounds on latitude
   if (m_start.lat > m_end.lat)
   {
      bound_ur.lat = m_start.lat;
      bound_ll.lat = m_end.lat;
   }
   else
   {
      bound_ur.lat = m_end.lat;
      bound_ll.lat = m_start.lat;
   }

   // get an initial estimate for the eastern and western bounds on longitude
   if (GEO_east_of_degrees(m_start.lon, m_end.lon))
   {
      bound_ur.lon = m_start.lon;
      bound_ll.lon = m_end.lon;
   }
   else
   {
      bound_ur.lon = m_end.lon;
      bound_ll.lon = m_start.lon;
   }

   // adjust the bounding box to account for quadrant boundary crossing(s)
   if (adjust_bounds(heading_to_start, heading_to_end, arc, 
      bound_ll, bound_ur) != SUCCESS)
   {
      ERR_report("adjust_bounds() failed.");
      return FAILURE;
   }

   // if this bounding box doesn't intersect the map, don't draw the arc
   if (GEO_intersect_degrees(map_ll, map_ur, bound_ll, bound_ur) == FALSE)
   {
      m_allow_redraw = TRUE;
      return SUCCESS;
   }

   int ret = generate_point_list(map, heading_to_start, arc);
	if (ret == SUCCESS)
		m_allow_redraw = TRUE;

	return ret;
}

// generate a list of geoLines for the geoArc
int GeoArc::generate_geoline_list(CList <GeoSegment *, GeoSegment *> &seg_lst)
{
   double heading_to_start;
   double heading_to_end;
   double arc;
   d_geo_t point;
   degrees_t prev_lat;
   degrees_t prev_lon;
   degrees_t heading;
   degrees_t delta;
   int count;

   // if the turn radius is 0.0 - this is a NULL arc
   if (m_radius == 0.0)
   {
      return SUCCESS;
   }
   
   // compute the heading to m_start from m_center, the heading to m_end
   // from m_center, and the angle of the arc
   if (compute_headings_and_arc(heading_to_start, heading_to_end, arc) !=
      SUCCESS)
   {
      ERR_report("compute_headings_and_arc() failed.");
      return FAILURE;
   }

   // compute the number of line segments used to approximate the arc
   count = (int)(arc  / 2.0) + 1;

   // figure out the sign and magnitude of the step, delta
   if (m_clockwise)
      delta = arc / count;
   else
      delta = -arc / count;

   point = m_start;
   heading = heading_to_start;
   while (count > 1)
   {
      // save previous point
      prev_lat = point.lat;
      prev_lon = point.lon;

      // increment heading for next point
      heading += delta;
      if (heading >= 360.0)
         heading -= 360.0;
      else if (heading < 0.0)
         heading += 360.0;

      // get next point along the arc
      calc_end_point(m_center, m_radius, heading, point);

      // add a geo arc from the previous point to the new point
      POSITION position = seg_lst.AddTail(new GeoLine((GeoSegment::heading_type_t)0));

      GeoSegment *segment = seg_lst.GetAt(position);
      ((GeoLine *)segment)->set_start(prev_lat, prev_lon);
      ((GeoLine *)segment)->set_end(point.lat, point.lon);

      count--;
   }

   // add a geosegment from the previous point to the end
   POSITION position = seg_lst.AddTail(new GeoLine((GeoSegment::heading_type_t)0));

   GeoSegment *segment = seg_lst.GetAt(position);
   ((GeoLine *)segment)->set_start(point.lat, point.lon);
   ((GeoLine *)segment)->set_end(m_end.lat, m_end.lon);

   return SUCCESS;
}

int GeoArc::generate_point_list(MapProj* map, degrees_t heading_to_start, degrees_t arc)
{
   d_geo_t point;
   degrees_t prev_lat;
   degrees_t prev_lon;
   degrees_t heading;
   degrees_t delta;
   int count;

   // compute the number of line segments used to approximate the arc
   count = (int)(arc / 2.0) + 1;

   // figure out the sign and magnitude of the step, delta
   if (m_clockwise)
      delta = arc / count;
   else
      delta = -arc / count;

   point = m_start;
   heading = heading_to_start;
   while (count > 1)
   {
      // save previous point
      prev_lat = point.lat;
      prev_lon = point.lon;

      // increment heading for next point
      heading += delta;
      if (heading >= 360.0)
         heading -= 360.0;
      else if (heading < 0.0)
         heading += 360.0;

      // get next point along the arc
      calc_end_point(m_center, m_radius, heading, point);

      // draw the next segment along the arc
      MAP_geo_line_points line_points;
      MAP_calc_geo_line(map, prev_lat, prev_lon, point.lat, point.lon, &line_points);
      const int line_count = line_points.num_lines();

      // 0, 1, or 2 lines were drawn.
      // Always add all points to the point list, eventhough this will usually
      // mean each point is in the list twice.  Doing it this way will handle
      // all special cases in a straight forward way.
      if (line_count == 1)
      {
         int x1, y1, x2, y2;
         line_points.get_saved_line(0, &x1, &y1, &x2, &y2);
         m_point_list.AddTail(new CPoint(x1, y1));
         m_point_list.AddTail(new CPoint(x2, y2));
      }
      else if (line_count == 2)
      {
         int a_x1, a_y1, a_x2, a_y2;
         line_points.get_saved_line(0, &a_x1, &a_y1, &a_x2, &a_y2);
         int b_x1, b_y1, b_x2, b_y2;
         line_points.get_saved_line(1, &b_x1, &b_y1, &b_x2, &b_y2);
         m_point_list.AddTail(new CPoint(a_x1, a_y1));
         m_point_list.AddTail(new CPoint(a_x2, a_y2));
         m_point_list.AddTail(new CPoint(b_x1, b_y1));
         m_point_list.AddTail(new CPoint(b_x2, b_y2));
      }

      count--;
   }

   // draw last segment to m_end
   MAP_geo_line_points line_points;
   MAP_calc_geo_line(map, point.lat, point.lon, m_end.lat, m_end.lon, &line_points);
   const int line_count = line_points.num_lines();

   // 0, 1, or 2 lines were drawn.
   // Always add all points to the point list, eventhough this will usually
   // mean each point is in the list twice.  Doing it this way will handle
   // all special cases in a straight forward way.
   if (line_count == 1)
   {
      int x1, y1, x2, y2;
      line_points.get_saved_line(0, &x1, &y1, &x2, &y2);
      m_point_list.AddTail(new CPoint(x1, y1));
      m_point_list.AddTail(new CPoint(x2, y2));
   }
   else if (line_count == 2)
   {
      int a_x1, a_y1, a_x2, a_y2;
      line_points.get_saved_line(0, &a_x1, &a_y1, &a_x2, &a_y2);
      int b_x1, b_y1, b_x2, b_y2;
      line_points.get_saved_line(1, &b_x1, &b_y1, &b_x2, &b_y2);
      m_point_list.AddTail(new CPoint(a_x1, a_y1));
      m_point_list.AddTail(new CPoint(a_x2, a_y2));
      m_point_list.AddTail(new CPoint(b_x1, b_y1));
      m_point_list.AddTail(new CPoint(b_x2, b_y2));
   }

   return SUCCESS;
}

// Returns the length of this segment in meters
double GeoArc::get_length()
{
   if (m_radius > 0.0)
   {
      double heading_to_start;
      double heading_to_end;
      double arc;
   
      // compute the heading to m_start from m_center, the heading to m_end
      // from m_center, and the angle of the arc
      if (compute_headings_and_arc(heading_to_start, heading_to_end, arc) !=
         SUCCESS)
      {
         ERR_report("compute_headings_and_arc() failed.");
	     return -1.0;
      }

      return m_radius * DEG_TO_RAD(arc);
   }

   return 0.0;
}

// If 0 <= distance <= get_length() then lat and lon will be set to the
// point at that distance from the start of the segment, heading
// will be set to the heading at that point along the segment, and 
// the function will return TRUE.  Otherwise FALSE is returned and
// lat, lon, and heading are unchanged.  distance is in meters.  
boolean_t GeoArc::get_point(double distance, d_geo_t &point, 
   degrees_t &heading)
{
   if (m_radius > 0.0)
   {
      double heading_to_start;
      double heading_to_end;
      double arc;
   
      // compute the heading to m_start from m_center, the heading to m_end
      // from m_center, and the angle of the arc
      if (compute_headings_and_arc(heading_to_start, heading_to_end, arc) !=
         SUCCESS)
      {
         ERR_report("compute_headings_and_arc() failed.");
	      return FALSE;
      }

      // compute arc length
      degrees_t heading_to_point;
      double length = m_radius * DEG_TO_RAD(arc);

      // if distance is out of range
      if (distance < 0.0 || distance > length)
         return FALSE;

      // intermediate point is the start point
      if (distance == 0.0)
      {
         point = m_start;
         heading_to_point = heading_to_start;
      }
      else
      {
         // compute the arc length in degrees
         arc = RAD_TO_DEG(distance / m_radius);

         // compute the heading to the intermediate point by using arc and the
         // arc direction
         if (m_clockwise)
         {
            // plus the turn arc for clockwise arcs
            heading_to_point = heading_to_start + arc;
            if (heading_to_point >= 360.0)
               heading_to_point -= 360.0;
         }
         else
         {
            // minus the turn arc for counter-clockwise arcs
            heading_to_point = heading_to_start - arc;
            if (heading_to_point < 0.0)
               heading_to_point += 360.0;
         }

         // get the point by moving a distance, radius, along the calculated
         // heading
         calc_end_point(m_center, m_radius, heading_to_point, point);
      }
      
      // compute the tangental heading
      if (get_heading_type() == GREAT_CIRCLE)
      {
         double range;

         // For great circle lines the heading from A to B does not usually equal
         // the heading from B to A + 180 degrees.
         GEO_calc_range_and_bearing(point, m_center, range, heading, TRUE);
         
         if (m_clockwise)
         {
            heading -= 90.0;
            if (heading < 0.0)
               heading += 360.0;
         }
         else
         {
            heading += 90.0;
            if (heading > 360.0)
               heading -= 360.0;
         }
      }
      else
      {
         // For rhumb lines the heading from A to B is equal to the heading
         // from B to A + 180.
         if (m_clockwise == FALSE)
         {
            heading = heading_to_point - 90.0;
            if (heading < 0.0)
               heading += 360.0;
         }
         else
         {
            heading = heading_to_point + 90.0;
            if (heading > 360.0)
               heading -= 360.0;
         }
      }

      return TRUE;
   }

   return FALSE;
}

// Get the shortest distance from the point to this segment.  The returned
// distance will be in meters.  right_of_course will be TRUE if the point is
// right of course, FALSE otherwise.
// The return values indicates if the point is along, before, or after
// the segment.
// +1 - after
//  0 - along
// -1 - before
// start_to_closest is the distance in meters along the GeoArc from the 
// closest point to the start of the GeoArc, where the closest point is
// the point "distance" meters away from "point".
int GeoArc::get_distance(d_geo_t point, double &distance, 
   boolean_t &right_of_course, double &start_to_closest)
{
   int status = 0;
   double range;
   double heading_to_point;
   double heading_to_start;
   double heading_to_end;

   // initialize return values to defaults in case of a failure
   distance = WGS84_a_METERS * PI;  // half way around the world
   start_to_closest = distance;
   right_of_course = FALSE;

   if (!GEO_valid_degrees(point.lat, point.lon))
      return status;

   // calculations differ for great-circle vs rhumb line routes
   boolean_t great_circle = (get_heading_type() == GeoSegment::GREAT_CIRCLE);

   // compute the heading to the starting point from the arc center
   if (GEO_calc_range_and_bearing(m_center, m_start, range, heading_to_start, 
      great_circle) != SUCCESS)
   {
      ERR_report("calc_range_and_bearing() failed.");
      return FAILURE;
   }

   // compute the heading to the ending point from the arc center
   if (GEO_calc_range_and_bearing(m_center, m_end, range, heading_to_end, 
      great_circle) != SUCCESS)
   {
      ERR_report("calc_range_and_bearing() failed.");
      return FAILURE;
   }

   // Get the range and bearing from the arc center to the point.
   if (GEO_calc_range_and_bearing(m_center, point, range, heading_to_point, 
      great_circle) != SUCCESS)
   {
      ERR_report("GEO_calc_range_and_bearing() failed.");
      return FAILURE;
   }

   // heading_to_end and heading_to_point will be normalized so it is as if 
   // heading_to_start is 0.0 and the arc is clockwise.

   // if the arc is counter-clockwise flip all angles
   if (m_clockwise == FALSE)
   {
      heading_to_start = 360 - heading_to_start;
      heading_to_end = 360.0 - heading_to_end;
      heading_to_point = 360.0 - heading_to_point;
   }

   // make heading_to_end relative to heading_to_start
   heading_to_end -= heading_to_start;
   if (heading_to_end < 0.0)
      heading_to_end += 360.0;

   // make heading_to_point relative to heading_to_start
   heading_to_point -= heading_to_start;
   if (heading_to_point < 0.0)
      heading_to_point += 360.0;

   // if off the arc
   if (heading_to_point > heading_to_end)
   {
      double d = fabs(range * cos(DEG_TO_RAD(heading_to_point)));

      // if you are inside the arc
      if (d < m_radius)
         right_of_course = m_clockwise;
      // if you are outside the arc
      else if (d > m_radius)
         right_of_course = (!m_clockwise);

      // if after the arc
      if (heading_to_point < (heading_to_end / 2.0 + 180.0))
      {
         status = 1;

         // the arc's ending point is the closest point
         GEO_calc_range_and_bearing(m_end, point, distance, heading_to_point,
            great_circle);
         start_to_closest = m_radius * DEG_TO_RAD(heading_to_end);
      }
      // else before the arc
      else
      {
         status = -1;

         // the arc's starting point is the closest point
         GEO_calc_range_and_bearing(m_start, point, distance, heading_to_point,
            great_circle);
         start_to_closest = 0.0;
      }
   }
   // else on the arc
   else
   {
      // distance from the arc is the distance from the center to the point
      // and from the point to
      distance = range - m_radius;

      // if you are inside the arc
      if (distance < 0.0)
      {
         distance = -distance;
         right_of_course = m_clockwise;
      }
      // if you are outside the arc
      else if (distance > 0.0)
         right_of_course = (!m_clockwise);
      start_to_closest = m_radius * DEG_TO_RAD(heading_to_point);
   }

   return status;
}

// set the heading type: rhumb-line vs great circle
// Note: a change in heading type must be accompanied by a redefinition
// of the GeoArc.  The calculations involved in the definition are dependent
// on this value.
int GeoArc::set_heading_type(heading_type_t heading_type)
{
   // no change - do nothing
   if (m_heading_type == heading_type)
      return SUCCESS;

   if (heading_type != GREAT_CIRCLE && heading_type != RHUMB_LINE)
   {
      ERR_report("Invalid heading type.");
      return FAILURE;
   }
   
   // make this a NULL arc - a call to one of the other define members must be
   // made to redefine this arc with the new heading type
   if (define(m_start) != SUCCESS)
   {
      ERR_report("define() failed.");
      return FAILURE;
   }

   m_heading_type = heading_type;

   return SUCCESS;
}

// Get the heading at the starting point.
degrees_t GeoArc::get_heading_in()
{
   d_geo_t point;
   degrees_t heading_in;

   if (!get_point(0.0, point, heading_in))
      return 0.0;

   return heading_in;
}

// Get the heading at the ending point.
degrees_t GeoArc::get_heading_out()
{
   d_geo_t point;
   degrees_t heading_out;

   // For some reason the "distance out of range" test in get_point(), 
   // if (distance < 0.0 || distance > length) indicates that distance is
   // greater than length if you pass in get_length().  After careful
   // investigation into the problem it was concluded that the problem
   // results from some sort of a bug in the optimizer.  The bug does not
   // occur in the debug version, where there is no optimization.  Adding
   // an INFO_report to output the value of distance and length when the test
   // is fails stops the problem from occurring.  The computation of the arcs
   // length in the get_length() function is identical to the calculation of
   // length in the get_point() function.  Thus it makes no sense that distance
   // is not exactly equal to length in this case, but it is not.  In any case,
   // by passing the true arc length less 1 micrometer the problem goes away.
   if (!get_point(get_length() - 0.000001, point, heading_out))
      return 0.0;

   return heading_out;
}

// An arc can be defined by the turn radius, turn center, heading from the
// center to the starting point, turn arc, and turn direction.  Since these
// values may not be known, alternative methods for defining the arc are 
// provided.  The radius is in meters.  The heading and turn_arc are in 
// degrees.
int GeoArc::define(d_geo_t center, double radius, double heading_to_start,
   double turn_arc, boolean_t clockwise)
{
   double heading_to_end;

   // check for valid radius
   if (radius <= 0.0)
   {
      ERR_report("radius must be greater than 0.0");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(center.lat, center.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // check for valid heading
   if (heading_to_start < 0.0 || heading_to_start > 360.0)
   {
      ERR_report("Invalid heading.");
      return FAILURE;
   }

   // check for valid turn arc
   if (turn_arc <= 0.0 || turn_arc >= 360.0)
   {
      ERR_report("Invalid turn arc.");
      return FAILURE;
   }

   // set the turn center to the given point
   m_center = center;

   // get the start of the arc by moving a distance, radius, along the calculated
   // heading
   calc_end_point(m_center, radius, heading_to_start, m_start);

   // compute the heading to the arc end point by using the turn_arc and the arc
   // direction
   if (clockwise)
   {
      // plus the turn arc for clockwise arcs
      heading_to_end = heading_to_start + turn_arc;
      if (heading_to_end >= 360.0)
         heading_to_end -= 360.0;
   }
   else
   {
      // minus the turn arc for counter-clockwise arcs
      heading_to_end = heading_to_start - turn_arc;
      if (heading_to_end < 0.0)
         heading_to_end += 360.0;
   }

   // get the end of the arc by moving a distance, radius, along the calculated
   // heading
   calc_end_point(m_center, radius, heading_to_end, m_end);

   // need radius to draw arc
   m_radius = radius;

   // need to know direction of arc in draw member
   m_clockwise = clockwise;

   // disable redraw since the arc definition has changed
   m_allow_redraw = FALSE;

   return SUCCESS;
}

// Here the arc is defined by specifying the its starting point along with
// two other lat-lon values.  The great circle line from in to start is
// tangent to the arc at the starting point.  The great circle line from
// the ending point (calculated) to out is tangent to the arc at the
// ending point.  The radius is in meters.  The turn_arc, in degrees, is
// included as an input to allow this function to detect and correct for
// an invalid turn direction from the route server.
int GeoArc::define(d_geo_t in, d_geo_t start, d_geo_t out, 
   double radius, double turn_arc, boolean_t clockwise)
{
   double range;
   double heading;
   double angle;

   // check for valid radius
   if (radius <= 0.0)
   {
      ERR_report("radius must be greater than 0.0");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(in.lat, in.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(start.lat, start.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(out.lat, out.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // the turn begins at the given starting point
   m_start = start;

   // Compute the heading from the turn start back to the previous point, in.
   calc_range_and_bearing(m_start, in, range, heading);

   // The great circle line from start to in is tangent to the radius of this
   // arc.  The heading from start to the turn center is 90 degrees left or
   // right from the above heading.
   if (clockwise)
   {
      // minus 90 degrees for counter-clockwise arcs
      heading -= 90.0;
      if (heading < 0.0)
         heading += 360.0;
   }
   else
   {
      // plus 90 degrees for clockwise arcs
      heading += 90.0;
      if (heading >= 360.0)
         heading -= 360.0;
   }

   // get the turn center by moving a distance, radius, along the calculated
   // heading
   calc_end_point(m_start, radius, heading, m_center);

   // use the distance from the turn center to the next point (Pf) and the
   // fact that the line from the next point to the arc end point is 
   // perpendicular to the radius of the arc at the arc end point to
   // compute the angle between the vector from C to Pf and C to the arc
   // end point
   calc_range_and_bearing(m_center, out, range, heading);
   // this should never happen, if it does there is an error.
   if (radius > range)
   {
      ERR_report("Turn radius is too large to make the next turn point.");
      define(start);
      return SUCCESS;
   }
   angle = RAD_TO_DEG(acos(radius / range));

   // The turn radius from C to the arc end is at a heading angle degrees left
   // or right from the heading from C to Pf.
   if (clockwise)
   {
      // minus angle degrees for clockwise arcs
      heading -= angle;
      if (heading < 0.0)
         heading += 360.0;
   }
   else
   {
      // plus angle degrees for counter-clockwise arcs
      heading += angle;
      if (heading >= 360.0)
         heading -= 360.0;
   }

   // If the route server indicated a small turn, but the above calculation
   // lead to a large turn, then the turn direction indicated by the route
   // server was incorrect.
   if (turn_arc < 30.0)
   {
      double heading_to_start;

      // get heading from center to starting point
      calc_range_and_bearing(m_center, m_start, range, heading_to_start);

      // Note "heading" contains the heading from the center to the computed
      // ending point.  Here the computed turn arc will be computed and stored
      // in angle.
      if (clockwise)
         angle = heading - heading_to_start;
      else
         angle = heading_to_start - heading;
      if (angle < 0.0)
         angle += 360.0;

      // If a loop will be generated because the route server got the direction
      // wrong, then report and error and try to compensate for the problem.
      // This same function will be called with turn_arc set to 30.0, to avoid
      // coming back this if block and the turn direction reversed.
      if (angle > 180.0)
      {
         ERR_report("Route Server indicated the wrong direction for the "
            "shortest turn.");
         return define(in, start, out, radius, 30.0, (clockwise == FALSE));
      }
   }

   // get the end of the arc by moving a distance, radius, along the calculated
   // heading
   calc_end_point(m_center, radius, heading, m_end);

   // need radius to draw arc
   m_radius = radius;

   // need to know direction of arc in draw member
   m_clockwise = clockwise;

   // disable redraw since the arc definition has changed
   m_allow_redraw = FALSE;

   return SUCCESS;
}

// Here the arc is defined as the arc of the given radius which is tangent
// to two intersecting great circle lines.  The first is the line from in to
// point, and it is tangent to the arc at the starting point (calculated).
// The second is the line from out to point, and it is tangent to the arc
// at the ending point (calculated).  In this case the arc is always less
// than 180.0 degrees.  The radius is in meters.
int GeoArc::define(d_geo_t in, d_geo_t point, d_geo_t out, double radius)
{
   double range;
   double heading_to_in;
   double heading_to_out;
   double heading_to_center;
   double angle;
   double d;

   // check for valid radius
   if (radius <= 0.0)
   {
      ERR_report("radius must be greater than 0.0");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(in.lat, in.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(point.lat, point.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(out.lat, out.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // Compute the angle between the great circle from the point to in and
   // the great circle line from point to the out.  The geometry produced by
   // this definition of an arc indicates that this angle is equal to 
   // 180 - turn_arc.  Further, it indicates that point is equadistant from
   // the arc starting and ending points.  That distance is given by:
   // d = r cos(angle/2) / sin(angle/2).

   // get the heading from the point to the in (also to m_start)
   calc_range_and_bearing(point, in, range, heading_to_in);

   // get the heading from the point to the out (also to m_end)
   calc_range_and_bearing(point, out, range, heading_to_out);

   // compute the angle between these two vectors as well as the direction of
   // the turn arc
   angle = heading_to_in - heading_to_out;
   if (angle == 0.0)
   {
      ERR_report("No arc.");
      return FAILURE;
   }

   // the angle between the vectors is affected by the arc direction and
   // if it crosses 0 degrees
   if (angle > 0.0)
   {
      // cw not crossing 0 degrees
      if (angle < 180.0)
         m_clockwise = TRUE;
      // ccw crossing 0 degrees
      else if (angle > 180.0)
      {
         angle = 360.0 - angle;
         m_clockwise = FALSE;
      }
      // point is along the path from in to out - NULL arc
      else
         return define(point);
   }
   else
   {
      // cw crossing 0 degrees
      if (angle < -180.0)
      {
         angle = 360.0 + angle;
         m_clockwise = TRUE;
      }
      // ccw not crossing 0 degrees
      else if (angle > -180.0)
      {
         angle = -angle;
         m_clockwise = FALSE;
      }
      // point is along the path from in to out - NULL arc
      else
         return define(point);
   }

   // actually need 1/2 this angle in radians
   angle = DEG_TO_RAD(angle / 2.0);

   // distance from point to m_start and point to m_end
   d = radius * cos(angle) / sin(angle);

   // get the starting point by moving d along the heading to in
   calc_end_point(point, d, heading_to_in, m_start);
   
   // get the ending point by moving d along the heading to out
   calc_end_point(point, d, heading_to_out, m_end);

   // to get the heading from point to the turn center, take the heading from
   // point to in plus/minus angle degrees
   angle = RAD_TO_DEG(angle);
   if (m_clockwise)
   {
      // minus angle for a clockwise arc
      heading_to_center = heading_to_in - angle;
      if (heading_to_center < 0.0)
         heading_to_center += 360.0;
   }
   else
   {
      // plus angle for a counter-clockwise arc
      heading_to_center = heading_to_in + angle;
      if (heading_to_center >= 360.0)
         heading_to_center -= 360.0;
   }

   // the distance from point to the arc center is the hypotenuse of a right 
   // triangle with sides of length d and radius.
   range = sqrt(d * d + radius * radius);

   // get the turn center by moving the calculated range at the proper heading
   calc_end_point(point, range, heading_to_center, m_center);

   // need radius to draw arc
   m_radius = radius;

   // disable redraw since the arc definition has changed
   m_allow_redraw = FALSE;

   return SUCCESS;
}


// Here the arc is defined by specifying the its starting point along with
// another lat-lon value, the radius, turn arc, and direction.  The great 
// circle line from in to start is tangent to the arc at the starting point.
// The turn_arc will be used to compute the roll-out point.  The turn arc is
// in degrees.  The radius is in meters.
int GeoArc::define(d_geo_t in, d_geo_t start, double radius, 
   double turn_arc, boolean_t clockwise)
{
   double range;
   double heading;

   // check for valid radius
   if (radius <= 0.0)
   {
      ERR_report("radius must be greater than 0.0");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(in.lat, in.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // check for valid lat-lon
   if (!GEO_valid_degrees(start.lat, start.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   // check for valid turn arc
   if (turn_arc <= 0.0 || turn_arc >= 360.0)
   {
      ERR_report("Invalid turn arc.");
      return FAILURE;
   }

   // the turn begins at the given starting point
   m_start = start;

   // Compute the heading from the turn start back to the previous point, in.
   calc_range_and_bearing(m_start, in, range, heading);

   // The great circle line from start to in is tangent to the radius of this
   // arc.  The heading from start to the turn center is 90 degrees left or
   // right from the above heading.
   if (clockwise)
   {
      // minus 90 degrees for clockwise arcs
      heading -= 90.0;
      if (heading < 0.0)
         heading += 360.0;
   }
   else
   {
      // plus 90 degrees for counter-clockwise arcs
      heading += 90.0;
      if (heading >= 360.0)
         heading -= 360.0;
   }

   // get the turn center by moving a distance, radius, along the calculated
   // heading
   calc_end_point(m_start, radius, heading, m_center);

   // Compute the heading from the turn center to the turn starting point.
   calc_range_and_bearing(m_center, m_start, range, heading);

   // use the turn_arc, turn direction, and the heading from the turn center
   // to the starting point to compute the heading from the turn center to 
   // the turn ending point
   if (clockwise)
   {
      // plus turn_arc degrees for clockwise arcs
      heading += turn_arc;
      if (heading >= 360.0)
         heading -= 360.0;
   }
   else
   {
      // minus turn_arc degrees for counter-clockwise arcs
      heading -= turn_arc;
      if (heading < 0.0)
         heading += 360.0;
   }

   // get the end of the arc by moving a distance, radius, along the calculated
   // heading
   calc_end_point(m_center, radius, heading, m_end);

   // need radius to draw arc
   m_radius = radius;

   // need to know direction of arc in draw member
   m_clockwise = clockwise;

   // disable redraw since the arc definition has changed
   m_allow_redraw = FALSE;

   return SUCCESS;
}


// This function will setup a NULL arc, that is, an arc with both starting
// and ending points at the same point and radius 0.  The purpose of this
// function is to allow this object to be in a state where it does not
// exist without having to keep track of this outside of this object.  In
// this state the draw, redraw, and invalidate members will do nothing, and
// the hit_test member will always return FALSE.
int GeoArc::define(d_geo_t point)
{
   // check for valid lat-lon
   if (!GEO_valid_degrees(point.lat, point.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }

   m_start = point;
   m_end = point;
   m_center = point;
   m_radius = 0.0;
   m_clockwise = FALSE;

   // disable redraw since the arc definition has changed
   m_allow_redraw = FALSE;

   return SUCCESS;
}

// Define an arc by simply specifing the defining parameters of an arc
int GeoArc::define(d_geo_t start, d_geo_t end, d_geo_t center,
      double radius, boolean_t clockwise)
{
   // check for valid lat-lon
   if (!GEO_valid_degrees(start.lat, start.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }
   
   // check for valid lat-lon
   if (!GEO_valid_degrees(end.lat, end.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }
   
   // check for valid lat-lon
   if (!GEO_valid_degrees(center.lat, center.lon))
   {
      ERR_report("Invalid lat-lon.");
      return FAILURE;
   }
   
   // check for valid radius
   if (radius <= 0.0)
   {
      ERR_report("radius must be greater than 0.0");
      return FAILURE;
   }
   
   m_start = start;
   m_end = end;
   m_center = center;
   m_radius = radius;
   m_clockwise = clockwise;

   return SUCCESS;
}


// compute the heading to m_start from m_center, the heading to m_end
// from m_center, and the angle of the arc
int GeoArc::compute_headings_and_arc(degrees_t &heading_to_start, 
   degrees_t &heading_to_end, degrees_t &arc)
{
   double range;
   double angle;

   // compute the heading to the starting point from the arc center
   if (calc_range_and_bearing(m_center, m_start, range, heading_to_start) 
      != SUCCESS)
   {
      ERR_report("calc_range_and_bearing() failed.");
      return FAILURE;
   }

   // compute the heading to the ending point from the arc center
   if (calc_range_and_bearing(m_center, m_end, range, heading_to_end)
      != SUCCESS)
   {
      ERR_report("calc_range_and_bearing() failed.");
      return FAILURE;
   }

   // compute the arc angle
   angle = heading_to_end - heading_to_start;
   if (m_clockwise)
   {
      // arc crosses 0 degrees
      if (angle < 0.0)
         angle = 360.0 + angle;
   }
   else
   {
      // arc does not cross 0 degrees
      if (angle < 0.0)
         angle = -(angle);
      else
         angle = 360.0 - (angle);
   }
   
   // make sure calculated arc is a valid value
   if (angle <= 0.0 && angle >= 360.0)
   {
      ERR_report("Invalid turn arc.");
      return FAILURE;
   }

   arc = angle;

   return SUCCESS;
}

// adjust the bounding box to account for quadrant boundary crossing(s)
int GeoArc::adjust_bounds(degrees_t heading_to_start, degrees_t heading_to_end,
   degrees_t arc, d_geo_t &bound_ll, d_geo_t &bound_ur)
{
   int start_quadrant;
   int end_quadrant;
   int i;

   // which quadrant (0, 1, 2, 3) are the starting and ending points in
   start_quadrant = (int)(heading_to_start / 90.0);
   end_quadrant = (int)(heading_to_end / 90.0);
	if (start_quadrant > 3)
		start_quadrant = 3;
	if (end_quadrant > 3)
		end_quadrant = 3;

   // if the arc crosses any quadrant boundaries, then the bound box must be
   // expanded accordingly
   if (start_quadrant != end_quadrant || arc >= 270.0)
   {
      if (m_clockwise)
      {
         i = start_quadrant;
         do
         {
            // increment to adjacent quadrant
            i++;
            if (i > 3)
               i = 0;

            // expand the bounds to include the quadrant cross-over point
            if (expand_bounds(i, bound_ll, bound_ur) != SUCCESS)
            {
               ERR_report("expand_bounds() failed.");
               return FAILURE;
            }

         } while (i != end_quadrant);
      }
      else
      {
         i = end_quadrant;
         do
         {
            // increment to adjacent quadrant
            i++;
            if (i > 3)
               i = 0;

            // expand the bounds to include the quadrant cross-over point
            if (expand_bounds(i, bound_ll, bound_ur) != SUCCESS)
            {
               ERR_report("expand_bounds() failed.");
               return FAILURE;
            }

         } while (i != start_quadrant);
      }
   }

   return SUCCESS;
}

// Assumes you crossed into the quadrant moving clockwise.  If you cross 
// into a quadrant (0 for 0.0 to 90.0, 3 for 270.0 to 360.0), this function
// will expand the given bounding box on the arc to include the cross-over 
// point.  Note: bound_ll and bound_ur must be set to define the bounding 
// box surrounding the two arc end points before the first call to this
// function.
int GeoArc::expand_bounds(int quadrant, d_geo_t &bound_ll, d_geo_t &bound_ur)
{
   d_geo_t point;
   double heading[4] = {0.0, 90.0, 180.0, 270.0};

   if (quadrant < 0 || quadrant > 3)
   {
      ERR_report("Invalid quadrant.");
      return FAILURE;
   }

   if (calc_end_point(m_center, m_radius, heading[quadrant], point) != SUCCESS)
   {
      ERR_report("calc_end_point() failed.");
      return FAILURE;
   }

   // if the point is north of the northern edge, it becomes the northern edge
   if (point.lat > bound_ur.lat)
      bound_ur.lat = point.lat;

   // if the point is south of the southern edge, it becomes the southern edge
   else if (point.lat < bound_ll.lat)
      bound_ll.lat = point.lat;

   // if the point is east of the eastern edge, it becomes the eastern edge
   if (GEO_east_of_degrees(point.lon, bound_ur.lon))
      bound_ur.lon = point.lon;

   // if the western edge is east of the point, it becomes the western edge
   else if (GEO_east_of_degrees(bound_ll.lon, point.lon))
      bound_ll.lon = point.lon;

   return SUCCESS;
}


// returns TRUE if the class name is GeoLine otherwise calls GeoSegment::is_kind_of
boolean_t GeoArc::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "GeoArc") == 0)
      return TRUE;
   
   return GeoSegment::is_kind_of(class_name);
}

// This function will swap the starting and ending points of the arc, and
// reverse the direction of travel.
void GeoArc::reverse_direction()
{
   d_geo_t temp;

   // swap starting and ending points
   temp = m_start;
   m_start = m_end;
   m_end = temp;

   // reverse the direction of travel from start to end
   m_clockwise = (m_clockwise == FALSE);
}

