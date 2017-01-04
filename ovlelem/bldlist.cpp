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



// bldlist.cpp

#include "stdafx.h"
#include "bldlist.h"
#include "err.h"
#include "map.h"
#include "maps.h"
#include "fvwutil.h"

#define EARTH_RADIUS 6378.0

// GreatCirclePointList - this class will be deprecated in favor of
// CGreatCirclePoints/CGreatCircleLineSegments classes above
//

GreatCirclePointList::GreatCirclePointList(CList <CPoint *, CPoint *> &list,
                                           boolean_t &allow_redraw) :
   m_point_list(list), m_allow_redraw(allow_redraw)
{
   ASSERT(m_point_list.IsEmpty());
}


// Fills in m_point_list with the points for a great circle line from p1 to
// p2.  If wrap it TRUE, the line goes off the eastern edge of the map and
// back onto the western edge of the map.
//
// IMPORTANT: p2 must be east of p1, or the points must share a common
// longitude.
int GreatCirclePointList::generate_points(IDrawingToolsProjection* map,
   d_geo_t map_ll, d_geo_t map_ur, d_geo_t p1, d_geo_t p2, boolean_t wrap,
   boolean_t clip)
{
   int old_flags;       // bounds check flag of last point
   int new_flags;       // bounds check flag of this point
   degrees_t lat;
   degrees_t lon;
   degrees_t prev_lat;
   degrees_t prev_lon;
   radians_t beta;      // angle between p1 and p2
   radians_t rotate_x;  // rotation about x to move p1 and p2 into x-y plane
   radians_t rotate_y;  // rotation about y to move p1 and p2 into x-y plane
   radians_t angle_p1;  // angle between x' and p1' in the rotated system
   radians_t angle;     // angle between p1 and an intermediate point
   radians_t delta_angle;  // change in angle between each point along the arc
   int count = 0;
   boolean_t north_south_check_enabled = TRUE;
   double tf;
   int screen_width, screen_height;

   map->get_vsurface_size(&screen_width, &screen_height);

   // special case: the great circle is a line of longitude
   // test for being very close together
   tf = fabs(p1.lon - p2.lon);
   if ((tf < 0.0000000001) || (tf > 359.999999999))
   {
      if (p1.lat > map_ur.lat)
         p1.lat = map_ur.lat;
      else if (p1.lat < map_ll.lat)
         p1.lat = map_ll.lat;

      if (p2.lat > map_ur.lat)
         p2.lat = map_ur.lat;
      else if (p2.lat < map_ll.lat)
         p2.lat = map_ll.lat;

      MAP_geo_line_points line_points;
      MAP_calc_geo_line(map, p1.lat, p2.lon, p2.lat, p2.lon, &line_points);
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

   // get the angles needed to calculate intermediate points along the arc
   // between p1 and p2
   if (get_angles(p1.lat, p1.lon, p2.lat, p2.lon,
      &beta, &rotate_x, &rotate_y, &angle_p1) != SUCCESS)
   {
      ERR_report("get_angles() failed.");
      return FAILURE;
   }

   // compute the step size the angle will be changed by between each
   // intermediate point along the arc from P1 to P2
   if (get_delta_angle(map, &delta_angle) != SUCCESS)
   {
      ERR_report("get_delta_angle() failed.");
      return FAILURE;
   }

   // initialize lat and lon to starting point
   lat = p1.lat;
   lon = p1.lon;

   // if the starting point is outside of the map bounds, then find a starting
   // lat, lon just outside of the map bounds
   if ((GEO_in_bounds(map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon,
      lat, lon) == FALSE) && clip)
   {
      int xor_flags;    // change in flags, old to new
      radians_t step;
      radians_t min_step;

      // initial step size is set to 1/100th the arc length of the line from P1
      // to P2 to speed up search
      step = beta / 100.0;

      // if the arc is too short use delta_angle as the searching step size
      if (step < delta_angle)
      {
         if (delta_angle < beta / 2.0)
            step = delta_angle;
         else
            step = delta_angle = beta / 2.0;
      }

      // if the map is jumped over while step = delta_angle/16, then the GC arc
      // must not intersect with the map
      min_step = delta_angle / 16.0;

      // uses the maps bounds to compute a better starting point for the search
      // of the cross over point than angle = 0.0
      angle = compute_starting_angle(map, map_ll, map_ur, rotate_x, rotate_y,
         angle_p1);

      // this great circle misses the map
      if (angle == -1.0)
      {
         m_allow_redraw = TRUE;
         return SUCCESS;
      }

      ASSERT(angle >= 0.0);

      if (angle >= beta)
         angle = beta - step;

      // get flags for p1, which is off the map
      old_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
         map_ur.lat, map_ur.lon, lat, lon);

      // compute the starting point
      if (calc_intermediate_geo(angle, rotate_x, rotate_y, angle_p1,
         &lat, &lon) != SUCCESS)
      {
         ERR_report("calc_intermediate_geo() failed.");
         return FAILURE;
      }
      count++;

      // initialize new_flags to bounds flags on starting point
      new_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
         map_ur.lat, map_ur.lon, lat, lon);

      // compare flags at p1 and the starting point
      xor_flags = old_flags ^ new_flags;

      // if the flags have changed
      if (xor_flags != 0)
      {
         degrees_t geo_width;

         geo_width = map_ur.lon - map_ll.lon;
         if (geo_width < 0.0)
            geo_width += WORLD_DEG;

         // if one point is north of the map and the other is south of it,
         // then do the search from delta_angle forward
         if (xor_flags & (GEO_NORTH_OF | GEO_SOUTH_OF) && geo_width >= 180.0)
         {
            angle = delta_angle;

            // recompute the starting point
            if (calc_intermediate_geo(angle, rotate_x, rotate_y, angle_p1,
               &lat, &lon) != SUCCESS)
            {
               ERR_report("calc_intermediate_geo() failed.");
               return FAILURE;
            }
            count++;

            // reinitialize new_flags to bounds flags on starting point
            new_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
               map_ur.lat, map_ur.lon, lat, lon);
         }
      }

      // if the starting point is east of the map and the ending point is not
      // east of the western edge of the map, the arc missed the map
      if ((new_flags & GEO_EAST_OF) && GEO_east_of_degrees(map_ll.lon, p2.lon))
      {
         m_allow_redraw = TRUE;
         return SUCCESS;
      }

      // if the initial point is on the map, backup until you are within 10.0
      // times delta_angle of the map, and set the step down to delta_angle
      if (new_flags == 0)
      {
         // reduce the step size since the starting point is on the map
         if (step > 10.0 * delta_angle)
            step = 10.0 * delta_angle;

         // in this case the starting point is so close to the edge of the map
         // that is should be used
         if (angle < step)
            step = angle;

         // step backwards by 10.0 * delta_angle
         do
         {
            angle -= step;

            // compute the previous point along the arc
            if (calc_intermediate_geo(angle, rotate_x, rotate_y, angle_p1,
               &lat, &lon) != SUCCESS)
            {
               ERR_report("calc_intermediate_geo() failed.");
               return FAILURE;
            }
            count++;

            new_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
               map_ur.lat, map_ur.lon, lat, lon);

         } while (new_flags == 0 && angle > 0.0);

         // make sure angle is positive
         if (angle < 0.0)
            angle = 0.0;

         step = delta_angle;
      }

      // find the first point within delta_angle of one of the map edges
      old_flags = new_flags;
      while (old_flags != 0 && angle < beta)
      {
         angle += step;
         prev_lat = lat;
         prev_lon = lon;

         // don't go past end of arc
         if (angle > beta)
            angle = beta;

         // compute the next point along the arc
         if (calc_intermediate_geo(angle, rotate_x, rotate_y, angle_p1,
            &lat, &lon) != SUCCESS)
         {
            ERR_report("calc_intermediate_geo() failed.");
            return FAILURE;
         }
         count++;

         // get the bounds check flags for the new point
         new_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
            map_ur.lat, map_ur.lon, lat, lon);

         // no change will result in xor_flags = 0, change will set the bits
         // in xor_flags that are different between old_flags and new_flags
         xor_flags = old_flags ^ new_flags;

         // if the flags have changed
         if (xor_flags != 0)
         {
            // if the new point is on the map, step backwards by 10*delta_angle
            // until it is off the map again, then set step to delta_angle
            // and continue stepping forward again
            if (new_flags == 0)
            {
               // if step size is delta_angle or less, then the previous point
               // is the last point within delta_angle of an outside edge of
               // the map bounds, the previous point is the correct starting
               // point
               if (step <= delta_angle)
               {
                  lat = prev_lat;
                  lon = prev_lon;

                  // if the starting point is near either pole then the initial
                  // value of angle must be adjusted to shorten the effective
                  // angle step size in the polar regions
                  if (lat > 70.0 || lat < -70.0)
                  {
                     angle -= step;
                     angle += delta_angle / 5.0;
                  }

                  break;
               }

               // reduce the step size since the last point is on the map
               if (step > 10.0 * delta_angle)
                  step = 10.0 * delta_angle;

               // step backwards until you are off the map again
               do
               {
                  angle -= step;

                  // compute the previous point along the arc
                  if (calc_intermediate_geo(angle, rotate_x, rotate_y, angle_p1,
                     &lat, &lon) != SUCCESS)
                  {
                     ERR_report("calc_intermediate_geo() failed.");
                     return FAILURE;
                  }
                  count++;

                  new_flags = GEO_bounds_check_degrees(map_ll.lat, map_ll.lon,
                     map_ur.lat, map_ur.lon, lat, lon);

               } while (new_flags == 0);

               // reduce the step size further so you can step back onto the
               // map, this check is needed when a step of delta_angle jumps
               // over a corner of the map - below
               if (step > delta_angle)
                  step = delta_angle;
            } // end if new point is on map
            else
            {
               // If the longitude of eastern edge of the map was crossed,
               // without crossing the western, northern, or southern edge
               // at the same time, then this line doesn't intersect the map.
               if (xor_flags == GEO_EAST_OF && (old_flags == GEO_NORTH_OF ||
                  old_flags == GEO_SOUTH_OF))
               {
                  m_allow_redraw = TRUE;
                  return SUCCESS;
               }

               // crossed from W to NW, NW to W, NW to N, W to SW, SW to W,
               // or SW to S but still not in the map window, so contine
               if (xor_flags == GEO_NORTH_OF || xor_flags == GEO_SOUTH_OF ||
                  xor_flags == GEO_WEST_OF)
               {
                  old_flags = new_flags;
                  continue;
               }

               // a cross from GEO_EAST_OF to GEO_WEST_OF half way around
               // the world must be ignored
               if ((old_flags & GEO_EAST_OF) && (new_flags & GEO_WEST_OF))
               {
                  old_flags = new_flags;
                  continue;
               }

               // if the corner of the map is stepped over even at the minimum
               // step size, then this line does not intersect the map
               if (step == min_step)
               {
                  m_allow_redraw = TRUE;
                  return SUCCESS;
               }

               // step backwards one step and cut the step size by 2,
               // limiting it to be no less than min_step, then continue
               angle -= step;
               step /= 2.0;
               if (step < min_step)
                  step = min_step;

               // reset lat and lon to match the value of angle
               lat = prev_lat;
               lon = prev_lon;

               // reset new_flags to match the value of angle and continue
               new_flags = old_flags;
            } // end else new point is off map
         }  // end new point is in different region from old map

         // set old_flags to new_flags
         old_flags = new_flags;
      }  // end while

      // line doesn't cross the map
      if (angle == beta)
      {
         m_allow_redraw = TRUE;
         return SUCCESS;
      }

   }  // end starting point is off the map
   else
   {
      if (delta_angle < beta)
      {
         // if starting point is near either pole than initialize angle to a
         // fraction of the normal angle step size
         if (lat > 70.0 || lat < -70.0)
            angle = delta_angle / 5.0;
         else
            angle = delta_angle;
      }
      else
         angle = beta;  // just draw straight line
   }

   ASSERT(angle >= 0.0 && angle <= beta);

   if (count >= 25)
      //TRACE("count = %d to find start of line\n", count);
   ASSERT(count < 150);

   // while the angle between p1 and the intermediate point is less than the
   // angle between p1 and p2, compute the next point along the great circle
   // path from p1 to p2
   while (angle < beta)
   {
      prev_lat = lat;
      prev_lon = lon;

      // compute the next point along the arc
      if (calc_intermediate_geo(angle, rotate_x, rotate_y, angle_p1,
         &lat, &lon) != SUCCESS)
      {
         ERR_report("calc_intermediate_geo() failed.");
         return FAILURE;
      }
      count++;

      // draw the next segment along the great circle line
      MAP_geo_line_points line_points;
      MAP_calc_geo_line(map, prev_lat, prev_lon, lat, lon, &line_points);
      int line_count = line_points.num_lines();
      if (!clip && (line_count < 1))
      {
         int x1, y1, x2, y2;

         map->geo_in_surface(prev_lat, prev_lon, &x1, &y1);
         map->geo_in_surface(lat, lon, &x2, &y2);
         // prevent screen wrap
         if ((x1 > screen_width) && (x2 < 0))
            x2 = -x2;
         if ((x1 < 0) && (x2 > screen_width))
            x2 = -x2;
         line_points.save_line(0, x1, y1, x2, y2);
         line_count = 1;
      }


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
      else if (line_count == 0 && m_point_list.GetCount())
      {
         // if line crossed the eastern edge of the map AND this line
         // does not wrap around the world, then it is not coming back
         if (GEO_east_of_degrees(lon, map_ur.lon) &&
            GEO_east_of_degrees(p2.lon, map_ur.lon) && wrap == FALSE)
            break;

         // only check north and south cross-over once
         if (north_south_check_enabled)
         {
            degrees_t mid_lat;
            degrees_t mid_lon;
            degrees_t mid_angle;

            // if the line crossed over the northern edge
            if (lat > map_ur.lat)
            {
               // if point and p2 are north of map, its not coming back
               if (p2.lat > map_ur.lat)
                  break;

               // compute the lat-lon at the middle of the remain part of the
               // great circle line
               mid_angle = angle + (beta - angle) / 2.0;
               if (calc_intermediate_geo(mid_angle, rotate_x, rotate_y,
                  angle_p1, &mid_lat, &mid_lon) != SUCCESS)
               {
                  ERR_report("calc_intermediate_geo() failed.");
                  return FAILURE;
               }
               count++;

               // if the midpoint is north east of the map AND this line does
               // not wrap around the world, then it is not coming back
               if (mid_lat > map_ur.lat &&
                  GEO_east_of_degrees(mid_lon, map_ur.lon) &&
                  GEO_east_of_degrees(p2.lon, map_ur.lon) && wrap == FALSE)
                  break;
            }

            // if the line crossed over the southern edge
            if (lat < map_ll.lat)
            {
               // if point and p2 are south of map, its not coming back
               if (p2.lat < map_ll.lat)
                  break;

               // compute the lat-lon at the middle of the remain part of the
               // great circle line
               mid_angle = angle + (beta - angle) / 2.0;
               if (calc_intermediate_geo(mid_angle, rotate_x, rotate_y,
                  angle_p1, &mid_lat, &mid_lon) != SUCCESS)
               {
                  ERR_report("calc_intermediate_geo() failed.");
                  return FAILURE;
               }
               count++;

               // if the midpoint is south east of the map AND this line does
               // not wrap around the world, then it is not coming back
               if (mid_lat < map_ll.lat &&
                  GEO_east_of_degrees(mid_lon, map_ur.lon) &&
                  GEO_east_of_degrees(p2.lon, map_ur.lon) && wrap == FALSE)
                  break;
            }

            // don't check north and south cross over again
            north_south_check_enabled = FALSE;
         }
      }

      // increment angle to the next intermediate point
      if (lat > 70.0 || lat < -70.0)
         angle += delta_angle / 5.0;
      else
         angle += delta_angle;
   }

   // the line still on the map
   if (angle >= beta)
   {
      // draw the next segment along the great circle line
      MAP_geo_line_points line_points;
      MAP_calc_geo_line(map, lat, lon, p2.lat, p2.lon, &line_points);
      int line_count = line_points.num_lines();
      if (!clip && (line_count < 1))
      {
         int x1, y1, x2, y2;

         map->geo_in_surface(prev_lat, prev_lon, &x1, &y1);
         map->geo_in_surface(lat, lon, &x2, &y2);
         // prevent screen wrap
         if ((x1 > screen_width) && (x2 < 0))
            x2 = -x2;
         if ((x1 < 0) && (x2 > screen_width))
            x2 = -x2;
         line_points.save_line(0, x1, y1, x2, y2);
         line_count = 1;
      }

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
   }

   return SUCCESS;
}
// end of generate_points

// *************************************************************
// calculate xyz coordinates from latitude and longitude, assuming the
// earth is a perfect sphere with radius EARTH_RADIUS km
int GreatCirclePointList::lat_lon_to_xyz(degrees_t latitude,
   degrees_t longitude, double *x, double *y, double *z)
{
   double theata;
   double phi;
   double temp;

   // Note there is an implied conversion from (lat, lon) to a spherical
   // coordinate (rho, phi, theata), where rho is the radius of the earth for
   // latitudes and longitudes.  The z-axis is the earth's axis of rotation.
   // The earth's center is the center of the rectangular coordinate system.
   // The x and y axes are in the equatorial plane.

   // longitude must be between 0.0 and 360.0
   if (longitude < 0.0)
      longitude += 360.0;

   // compute angle with x-axis in radians, the x-axis goes from the earth's
   // center to N 0.0, E 0.0.
   theata = DEG_TO_RAD(longitude);

   // compute angle with the z-axis in radians, the z-axis is the earth's
   // axis of rotation
   phi = DEG_TO_RAD(90.0 - latitude);

   // intermediate value R sin(phi)
   temp = EARTH_RADIUS * sin(phi);

   // R cos(theata) sin(phi)
   *x = cos(theata) * temp;
   *y = sin(theata) * temp;
   *z = EARTH_RADIUS * cos(phi);

   return SUCCESS;
}

// calculate latitude and longitude from the xyz coordinate, assuming the
// earth is a perfect sphere with a radius EARTH_RADIUS km
int GreatCirclePointList::xyz_to_lat_lon(double x, double y, double z,
   degrees_t *latitude, degrees_t *longitude)
{
   double r;

   if (z > EARTH_RADIUS || -z > EARTH_RADIUS)
   {
      ERR_report("Invalid z coordinate.");
      return FAILURE;
   }

   // compute latitude
   *latitude = 90.0 - RAD_TO_DEG(acos(z / EARTH_RADIUS));

   // compute longitude - breaks at x = 0 and y = 0
   r = sqrt(x * x + y * y);
   if (r > 0.0)
   {
      *longitude = RAD_TO_DEG(acos(x / r));

      if (y < 0.0)
         *longitude = -(*longitude);
   }
   else
   {
      *longitude = 0.0;

      ASSERT(z == EARTH_RADIUS || -z == EARTH_RADIUS);
   }


   ASSERT(GEO_valid_degrees(*latitude, *longitude));

   return SUCCESS;
}

// given two vectors, P1(x1, y1, z1) and P2(x2, y2, z2), compute the CCW
// angle between them, beta.  In addition, compute rotate_y and rotate_x,
// which define the coordinate transformation from the earth's system to
// a rotated system with P1 and P2 in the x' - y' plane.
int GreatCirclePointList::get_parameters(double x1, double y1, double z1,
   double x2, double y2, double z2,
   radians_t *beta, radians_t *rotate_x, radians_t *rotate_y)
{
   double nx, ny, nz;   // normal vector to P1 - P2 plane
   double length;

   // check for valid P1
   length = sqrt(x1 * x1 + y1 * y1 + z1 * z1);
   if (length < (EARTH_RADIUS - 1.0) || length > (EARTH_RADIUS + 1.0))
   {
      ERR_report("Invalid value for P1.");
      return FAILURE;
   }

   // check for valid P2
   length = sqrt(x2 * x2 + y2 * y2 + z2 * z2);
   if (length < (EARTH_RADIUS - 1.0) || length > (EARTH_RADIUS + 1.0))
   {
      ERR_report("Invalid value for P2.");
      return FAILURE;
   }

   // N = P1 x P2
   nx = y1 * z2 - z1 * y2;
   ny = z1 * x2 - x1 * z2;
   nz = x1 * y2 - y1 * x2;

   // compute the length of the normal vector, N
   length = sqrt(nx * nx + ny * ny + nz * nz);

   // Check for two points on the opposite ends of the earth, i.e., beta will
   // be PI (180.0 degrees).  In this case there are an infinite number of
   // great circles between P1 and P2.  It is also true that P1 = -P2 and the
   // cross product, N, will be (0, 0, 0).
   if (length < 0.000001)
   {
      // the angle between P1 and P2 must be 180.0 degrees
      *beta = PI;

      length = sqrt(z1 * z1 + y1 * y1);

      // check for P1 = (R, 0, 0) to avoid divide by 0
      if (length > 0.0)
      {
         // Use the great circle path from P1 to P2 that goes through the point
         // N 0.0, E 0.0.  That is P2 = (R, 0, 0)
         // nx = 0.0;
         // ny = R * z1;
         // nz = -R * y1;

         // rotate_y = acos(nz / sqrt(nx * nx + nz * nz)) = acos(-y1/|y1|)
         if (y1 < 0.0)
            *rotate_y = 0.0;
         else
           *rotate_y = PI;

         // rotate_x = asin(ny / ||N||) = asin(z1 / sqrt(y1 * y1 + z1 * z1))
         // nz >= 0.0
         if (y1 <= 0.0)
            *rotate_x = asin(z1 / length);         // -HALF_PI <= a <= HALF_PI
         else
         {
            // ny < 0.0
            if (z1 < 0.0)
               *rotate_x = -(PI + asin(z1 / length)); // -PI < a < -HALF_PI
            else
               *rotate_x = PI - asin(z1 / length);    // HALF_PI < a <= PI
         }

         return SUCCESS;
      }

      // Use the great circle path from P1 to P2 that goes through the point
      // N 0.0, E 90.0.  That is P2 = (0, R, 0)
      // nx = 0.0;
      // ny = 0.0;
      // nz = 1.0;
      *rotate_y = 0.0;
      *rotate_x = 0.0;

      return SUCCESS;
   }

   const double cosBeta = (x1 * x2 + y1 * y2 + z1 * z2) /
      (EARTH_RADIUS * EARTH_RADIUS);
   const double EPS = 1e-15;

   // general case cos(beta) is the (P1 dot P2) / (R * R)
   if ( (cosBeta < 1 + EPS) && (cosBeta > 1 - EPS))
      *beta = 0;
   else if ( (cosBeta < -1 + EPS) && (cosBeta > -1 - EPS))
      *beta = PI;
   else
      *beta = acos(cosBeta);

   // normalize N
   nx = nx / length;
   ny = ny / length;
   nz = nz / length;

   // general case for rotation about x-axis
   if (nz >= 0.0)
      *rotate_x = asin(ny);            // -HALF_PI <= a <= HALF_PI
   else
   {
      if (ny < 0.0)
         *rotate_x = -(PI + asin(ny)); // -PI < a < -HALF_PI
      else
         *rotate_x = PI - asin(ny);    // HALF_PI < a <= PI
   }

   // general case for rotation about y-axis, 0 <= rotate_y <= PI
   if (nz == 0.0)
      *rotate_y = HALF_PI;
   else
      *rotate_y = acos(nz / sqrt(nx * nx + nz * nz));

   // Use sign of nx to determine which side of the y - z plane N is on, and
   // adjust rotate_y so it is the CCW angle from z-axis to the projection of
   // N into the x - z plane.
   if (nx < 0.0)
      *rotate_y = -(*rotate_y);        // -PI < rotate_y < 0

   return SUCCESS;
}

// Rotate the point P(x, y, z) in the same way the normal vector N would have
// to be rotated to align it with the z-axis, where N is the normal vector to
// the P1 x P2.  The values for rotate_x and rotate_y for this normal vector
// are computed by the get_parameters member function, above.
// Performs Rx(rotate_x) Ry(-rotate_y) P = P'.
int GreatCirclePointList::forward_conversion(double x, double y, double z,
   radians_t rotate_x, radians_t rotate_y,
   double *x_out, double *y_out, double *z_out)
{
   *x_out = x * cos(-rotate_y) - z * sin(rotate_y);

   *y_out = -x * sin(rotate_x) * sin(rotate_y) + y * cos(rotate_x) -
      z * sin(rotate_x) * cos(-rotate_y);

   *z_out = x * cos(rotate_x) * sin(rotate_y) + y * sin(rotate_x) +
      z * cos(rotate_x) * cos(-rotate_y);

   return SUCCESS;
}

// Opposite of forward_conversion. Performs Ry(rotate_y) Rx(-rotate_x) P' = P
int GreatCirclePointList::reverse_conversion(double x, double y, double z,
   radians_t rotate_x, radians_t rotate_y,
   double *x_out, double *y_out, double *z_out)
{
   *x_out = x * cos(rotate_y) - y * sin(rotate_y) * sin(rotate_x) +
      z * sin(rotate_y) * cos(-rotate_x);

   *y_out = y * cos(-rotate_x) + z * sin(rotate_x);

   *z_out = -x * sin(rotate_y) - y * cos(rotate_y) * sin(rotate_x) +
      z * cos(rotate_y) * cos(-rotate_x);

   return SUCCESS;
}

int GreatCirclePointList::get_angles(degrees_t lat1, degrees_t lon1,
   degrees_t lat2, degrees_t lon2,
   radians_t *beta, radians_t *rotate_x, radians_t *rotate_y,
   radians_t *angle_p1)
{
   double x1, y1, z1;   // vector P1 for lat1, lon1
   double x2, y2, z2;   // vector P2 for lat2, lon2

   // compute vector P1 for lat1, lon1
   if (lat_lon_to_xyz(lat1, lon1, &x1, &y1, &z1) != SUCCESS)
   {
      ERR_report("lat_lon_to_xyz() failed on point 1.");
      return FAILURE;
   }

   // compute vector P2 for lat2, lon2
   if (lat_lon_to_xyz(lat2, lon2, &x2, &y2, &z2) != SUCCESS)
   {
      ERR_report("lat_lon_to_xyz() failed on point 2.");
      return FAILURE;
   }

   // compute the angle between P1 and P2 and the rotation angles to align
   // the normal vector for the P1, P2 plane with the z-axis
   if (get_parameters(x1, y1, z1, x2, y2, z2, beta, rotate_x, rotate_y) !=
      SUCCESS)
   {
      ERR_report("get_parameters() failed.");
      return FAILURE;
   }

   double x, y, z;   // vector P1'

   // apply the rotation about x and y axes to place P1 in the x - y plane,
   // so the points between P1 and P2 along the great circle path between them
   // can be computed from the angle to the intermediate point and the angle
   // of P1 after rotation, angle_p1.
   if (forward_conversion(x1, y1, z1, *rotate_x, *rotate_y, &x, &y, &z) !=
      SUCCESS)
   {
      ERR_report("forward_conversion() failed.");
      return FAILURE;
   }

   // compute the CCW (+) angle for P1', the rotated P1
   if (x == 0.0)
   {
      if (y < 0.0)
         *angle_p1 = PI + HALF_PI;  // 3PI/2 or 270 degrees
      else
         *angle_p1 = HALF_PI;       // PI/2 or 90 degrees
   }
   else
   {
      if (x > 0.0)
      {
         if (y < 0.0)
            // 4th quadrant atan() returns between 0 and -HALF_PI
            *angle_p1 = atan(y / x) + TWO_PI;
         else
            // 1rst quadrant atan() returns between 0 and HALF_PI
            *angle_p1 = atan(y / x);
      }
      else
      {
         // 2nd quadrant atan() returns between 0 and -HALF_PI
         // 3rd quadrant atan() returns between 0 and HALF_PI
         *angle_p1 = atan(y / x) + PI;
      }
   }

   return SUCCESS;
}

int GreatCirclePointList::calc_intermediate_geo(radians_t beta,
   radians_t rotate_x, radians_t rotate_y, radians_t angle_p1,
   degrees_t *lat, degrees_t *lon)
{
   double x, y, z;
   double x_out, y_out, z_out;

   x = EARTH_RADIUS * cos(angle_p1 + beta);
   y = EARTH_RADIUS * sin(angle_p1 + beta);
   z = 0.0;

   if (reverse_conversion(x, y, z, rotate_x, rotate_y,
      &x_out, &y_out, &z_out) != SUCCESS)
   {
      ERR_report("reverse_conversion() failed.");
      return FAILURE;
   }

   if (xyz_to_lat_lon(x_out, y_out, z_out, lat, lon) != SUCCESS)
   {
      ERR_report("xyz_to_lat_lon() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

int GreatCirclePointList::get_angle(degrees_t lat, degrees_t lon,
   radians_t rotate_x, radians_t rotate_y, radians_t angle_p1,
   radians_t *angle, double *z_out)
{
   double x, y, z;               // vector P
   double x_rot, y_rot, z_rot;   // vector P'

   // compute vector P for lat, lon
   if (lat_lon_to_xyz(lat, lon, &x, &y, &z) != SUCCESS)
   {
      ERR_report("lat_lon_to_xyz() failed.");
      return FAILURE;
   }

   // apply the rotation about x and y axes to place P' in the coordinate
   // system where P1 and P2 are in the x-y plane
   if (forward_conversion(x, y, z, rotate_x, rotate_y,
      &x_rot, &y_rot, &z_rot) != SUCCESS)
   {
      ERR_report("forward_conversion() failed.");
      return FAILURE;
   }

   // compute the CCW (+) angle for the projection of P' into the x - y plane
   if (x_rot == 0.0)
   {
      if (y_rot < 0.0)
         *angle = PI + HALF_PI;  // 3PI/2 or 270 degrees
      else
         *angle = HALF_PI;       // PI/2 or 90 degrees
   }
   else
   {
      if (x_rot > 0.0)
      {
         if (y_rot < 0.0)
            // 4th quadrant atan() returns between 0 and -HALF_PI
            *angle = atan(y_rot / x_rot) + TWO_PI;
         else
            // 1rst quadrant atan() returns between 0 and HALF_PI
            *angle = atan(y_rot / x_rot);
      }
      else
      {
         // 2nd quadrant atan() returns between 0 and -HALF_PI
         // 3rd quadrant atan() returns between 0 and HALF_PI
         *angle = atan(y_rot / x_rot) + PI;
      }
   }

   // normalize angle relative to the angle of P1 in the x'-y' plane
   if (*angle < angle_p1)
      *angle += TWO_PI;

   // the sign of z_out indicates if the point is above or below the x'-y'
   // plane
   *z_out = z_rot;

   return SUCCESS;
}


int GreatCirclePointList::get_delta_angle(IDrawingToolsProjection* map,
   radians_t *delta_angle)
{
   degrees_t degrees_lat_per_pixel;
   degrees_t degrees_lon_per_pixel;
   degrees_t lon;
   double bearing;
   double pixel_width;  // pixel width in km along the center row of the map
   double pixel_height; // pixel height in km

   // get the degrees per pixel in both directions
   if (map->get_vmap_degrees_per_pixel(&degrees_lat_per_pixel,
      &degrees_lon_per_pixel) != SUCCESS)
   {
      ERR_report("get_vmap_degrees_per_pixel() failed.");
      return FAILURE;
   }

   // insure that delta_angle is always non-zero, this is needed because the
   // GEO_geo_to_distance function will return range = 0.0 for small values
   // of degrees per pixel lat or lon
   if (degrees_lon_per_pixel < 2.0e-006)
   {
      *delta_angle = 1.0e-7;
      return SUCCESS;
   }

   // get the pixel height in meters
   // note Robert: If the center of projection is on either pole, then adding
   // one pixel worth of degrees to compute distance and bearing below would
   // fail because the latitude of the second point would exceed 90 degrees for
   // latitudes.  Change the direction of the pixel according to the distance
   // to the pole

   degrees_t center_lat = map->actual_center_lat();
   degrees_t center_lon = map->actual_center_lon();
   degrees_t end_lat;
   if ( center_lat >= 0.0 )
   {
      end_lat = center_lat + degrees_lat_per_pixel;
      if ( end_lat > 90.0 )
      {
         end_lat = center_lat;
         center_lat = center_lat - degrees_lat_per_pixel;
      }
   }
   else
   {
      end_lat = center_lat - degrees_lat_per_pixel;
      if ( end_lat < -90.0 )
      {
         end_lat = center_lat;
         center_lat = center_lat + degrees_lat_per_pixel;
      }
   }


   if (GEO_geo_to_distance(center_lat, center_lon, end_lat, center_lon,
      &pixel_height, &bearing) != SUCCESS)
   {
      ERR_report("GEO_geo_to_distance() failed.");
      return FAILURE;
   }

   // convert to km
   pixel_height *= 0.001;

   // compute the longitude one pixel width to the east of the west edge
   lon = map->actual_center_lon() + degrees_lon_per_pixel;
   if (lon > 180.0)
      lon -= 360.0;

   // get the pixel width in meters along the center line
   if (GEO_geo_to_distance(map->actual_center_lat(), map->actual_center_lon(),
      map->actual_center_lat(), lon,
      &pixel_width, &bearing) != SUCCESS)
   {
      ERR_report("GEO_geo_to_distance() failed.");
      return FAILURE;
   }

   // convert to km
   pixel_width *= 0.001;

   // divide the line up into 20 pixel long segments (roughly)
   *delta_angle = 10.0 * (pixel_width + pixel_height) / EARTH_RADIUS;

   return SUCCESS;
}

radians_t GreatCirclePointList::compute_starting_angle(
   IDrawingToolsProjection* map, d_geo_t map_ll, d_geo_t map_ur,
   radians_t rotate_x, radians_t rotate_y, radians_t angle_p1)
{
   radians_t ll_angle;
   radians_t ul_angle;
   radians_t corner_angle;
   double ll_z;
   double ul_z;
   double corner_z;

   // get the angle the the lower left corner of the map makes with the
   // x'-y' plane
   get_angle(map_ll.lat, map_ll.lon, rotate_x, rotate_y, angle_p1,
      &ll_angle, &ll_z);

   // get the angle the the upper left corner of the map makes with the
   // x'-y' plane
   get_angle(map_ur.lat, map_ll.lon, rotate_x, rotate_y, angle_p1,
      &ul_angle, &ul_z);

   // check for lower left corner intersect
   if (ll_z == 0.0 && ul_z != 0.0)
      return ll_angle - angle_p1;

   // check for upper left corner intersect
   if (ul_z == 0.0 && ll_z != 0.0)
      return ul_angle - angle_p1;

   // check for western map boundary cross-over
   if ((ll_z >= 0.0 && ul_z <= 0.0) ||
      (ll_z <= 0.0 && ul_z >= 0.0))
   {
      // get_angle adds TWO_PI to angles less than angle_p1
      double ll_angle_norm = ll_angle >= TWO_PI ? ll_angle - TWO_PI : ll_angle;
      double ul_angle_norm = ul_angle >= TWO_PI ? ul_angle - TWO_PI : ul_angle;

      // start the search with the minimum of these two angles
      if (ll_angle_norm < ul_angle_norm)
      {
         // If angle_p1 falls between the angles to the corner points, it is
         // your only safe choice.  ll_angle could be past the end of the line.
         if (ll_angle_norm < angle_p1 && angle_p1 < ul_angle_norm)
            return 0.0;
         else
            return ll_angle - angle_p1;
      }
      else
      {
         // If angle_p1 falls between the angles to the corner points, it is
         // your only safe choice.  ul_angle could be past the end of the line.
         if (ul_angle_norm < angle_p1 && angle_p1 < ll_angle_norm)
            return 0.0;
         else
            return ul_angle - angle_p1;
      }
   }

   // The clip test being used by this function is simple.  The P1 x P2
   // plane splits the globe into two semispheres.  In the rotated system
   // P1' and P2' are in the x' - y' plane.  Any point on the service of the
   // globe that has a z' coordinate of 0 is also in the x' - y' plane,
   // and since all points are on the surface of the sphere, points with
   // z' = 0 and EARTH_RADIUS^2 = x' * x' + y' * y' are also on the great
   // circle implied by P1 and P2.  It follows that any points with positive
   // non-zero z' are on the surface of one semisphere, while those with a
   // negitive z' are on the surface of the opposite semisphere.  For any
   // two points it is always true that if point A has z' >= 0 while point
   // B has z' <= 0, the great circle path between them must cross the great
   // circle implied by P1 and P2.  For a line of longitude, which is a
   // great circle, this test conclusively determines if a great circle
   // crosses a line of longitude between two points A and B.  For a line of
   // latitude it is true that the great circle intersects the line of
   // latitude between A and B if it intersects the great circle path from
   // A to B.  However, it is not true that the great circle doesn't
   // intersect the line of latitude between A and B just because it doesn't
   // intersect the great circle path from A to B.  For this reason a line
   // of latitude must be split in half.  The test is conclusive if neither
   // half intersects the great circle.

   // the center longitude will be used to split the northern or southern
   // edge in half, below, if necessary

   // get the angle the north pole makes with the x'-y' plane
   get_angle(90.0, map_ll.lon, rotate_x, rotate_y, angle_p1,
      &corner_angle, &corner_z);

   // check for western map longitude cross-over north of the map
   if ((corner_z >= 0.0 && ul_z <= 0.0) ||
      (corner_z <= 0.0 && ul_z >= 0.0))
   {
      // get the angle and z' value for the midpoint of the northern map
      // boundary
      get_angle(map->actual_center_lat(), map->actual_center_lon(),
         rotate_x, rotate_y, angle_p1, &corner_angle, &corner_z);

      // check for midpoint intersect
      if (corner_z == 0.0 && ul_z != 0.0)
         return corner_angle - angle_p1;

      // check for western half of northern map boundary cross-over
      if ((corner_z >= 0.0 && ul_z <= 0.0) ||
         (corner_z <= 0.0 && ul_z >= 0.0))
      {
         // the minimum of these two angles will give a point outside the
         // map window
         if (corner_angle < ul_angle)
            return corner_angle - angle_p1;
         else
            return ul_angle - angle_p1;
      }

      // shift to the eastern half of the northern map boundary
      ul_z = corner_z;
      ul_angle = corner_angle;

      // get the angle and z' value for upper right corner of the map
      get_angle(map_ur.lat, map_ur.lon, rotate_x, rotate_y, angle_p1,
         &corner_angle, &corner_z);

      // check for upper right map corner intersect
      if (corner_z == 0.0 && ul_z != 0.0)
         return corner_angle - angle_p1;

      // check for eastern half of northern map boundary cross-over
      if ((corner_z >= 0.0 && ul_z <= 0.0) ||
         (corner_z <= 0.0 && ul_z >= 0.0))
      {
         // the minimum of these two angles will give a point outside the
         // map window
         if (corner_angle < ul_angle)
            return corner_angle - angle_p1;
         else
            return ul_angle - angle_p1;
      }

      // this great circle does not intersect with the map
      return -1.0;
   }

   // get the angle the south pole makes with the x'-y' plane
   get_angle(-90.0, map_ll.lon, rotate_x, rotate_y, angle_p1,
      &corner_angle, &corner_z);

   // check for western longitude cross-over south of the map
   if ((corner_z >= 0.0 && ll_z <= 0.0) ||
      (corner_z <= 0.0 && ll_z >= 0.0))
   {
      // get the angle and z' value for the midpoint of the southern map
      // boundary
      get_angle(map->actual_center_lat(), map->actual_center_lon(),
         rotate_x, rotate_y, angle_p1, &corner_angle, &corner_z);

      // check for midpoint intersect
      if (corner_z == 0.0 && ll_z != 0.0)
         return corner_angle - angle_p1;

      // check for western half of northerm map boundary cross-over
      if ((corner_z >= 0.0 && ll_z <= 0.0) ||
         (corner_z <= 0.0 && ll_z >= 0.0))
      {
         // the minimum of these two angles will give a point outside the
         // map window
         if (corner_angle < ll_angle)
            return corner_angle - angle_p1;
         else
            return ll_angle - angle_p1;
      }

      // shift to the eastern half of the southern map boundary
      ll_z = corner_z;
      ll_angle = corner_angle;

      // get the angle the the lower right corner of the map makes with the
      // x'-y' plane
      get_angle(map_ll.lat, map_ur.lon, rotate_x, rotate_y, angle_p1,
         &corner_angle, &corner_z);

      // check for upper right corner intersect
      if (corner_z == 0.0 && ll_z != 0.0)
         return corner_angle - angle_p1;

      // check for northerm map boundary cross over
      if ((corner_z >= 0.0 && ll_z <= 0.0) ||
         (corner_z <= 0.0 && ll_z >= 0.0))
      {
         // the minimum of these two angles will give a point outside the
         // map window
         if (corner_angle < ll_angle)
            return corner_angle - angle_p1;
         else
            return ll_angle - angle_p1;
      }

      // this great circle does not intersect with the map
      return -1.0;
   }

   // If you get here one of three things should be true about the great
   // circle: it is a line of longitude, it is the equator, or the map is
   // a world overview display.  The line of longitude is a special case
   // that should be handled separately outside of this function.  The
   // equator should have been filtered out by geo-clipping against the map
   // bounds, or it should have gone through the western edge of the map.
   // If you are on world overview display, the starting point should be on
   // the map so this function shouldn't be called.  All together if you get
   // here, something is wrong.
//   ASSERT(0);

   return 0.0;
}


// RhumbLineClipper
//

RhumbLineClipper::RhumbLineClipper(d_geo_t p1, d_geo_t p2) : Mercator(0.0)
{
   ASSERT(GEO_valid_degrees(p1.lat, p1.lon));
   ASSERT(GEO_valid_degrees(p2.lat, p2.lon));

   if (GEO_east_of_degrees(p1.lon, p2.lon))
   {
      m_start = p2;
      m_end = p1;
   }
   else
   {
      m_start = p1;
      m_end = p2;
   }

   degrees_t center_lon;
   degrees_t geo_width;

   // compute the longitude spanned by this line
   geo_width = m_end.lon - m_start.lon;
   if (geo_width < 0.0)
      geo_width += WORLD_DEG;

   center_lon = m_start.lon + geo_width / 2.0;
   if (center_lon > HALF_WORLD_DEG)
      center_lon -= WORLD_DEG;

   set_center_lon(center_lon);

   m_ll.lat = -89.99;
   m_ll.lon = -180.0;
   m_ur.lat = 89.99;
   m_ur.lon = 180.0;
   set_clip_bounds(m_ll, m_ur);
}

int RhumbLineClipper::set_clip_bounds(d_geo_t ll, d_geo_t ur)
{
   if (!GEO_valid_degrees(ll.lat, ll.lon))
   {
      ERR_report("Invalid ll lat-lon.");
      return FAILURE;
   }

   if (!GEO_valid_degrees(ur.lat, ur.lon))
   {
      ERR_report("Invalid ur lat-lon.");
      return FAILURE;
   }

   if (ur.lat <= ll.lat)
   {
      ERR_report("Invalid bounds.");
      return FAILURE;
   }

   // since a latitude of +90.0 is invalid in the Mercator project, the
   // maximum value must be limited to +89.99
   if (ur.lat > 89.99)
      ur.lat = 89.99;

   // since a latitude of -90.0 is invalid in the Mercator project, the
   // minimum value must be limited to -89.99
   if (ll.lat < -89.99)
      ll.lat = -89.99;

   if (ur.lat <= ll.lat)
   {
      ERR_report("Invalid bounds.");
      return FAILURE;
   }

   m_ll = ll;
   m_ur = ur;

   // convert lat-lon bounds to Mercator coordinates
   m_west_x = lon_to_x(m_ll.lon);
   m_south_y = lat_to_y(m_ll.lat);
   m_east_x = lon_to_x(m_ur.lon);
   m_north_y = lat_to_y(m_ur.lat);

   return SUCCESS;
}

boolean_t RhumbLineClipper::get_clipped_points(d_geo_t points[4],
   int &line_count, boolean_t clip)
{
   int start_flags;           // NSEW bounds check flags for m_start
   int end_flags;             // NSEW bounds check flags for m_end

   // get NSEW bounds check flags for each point
   start_flags = GEO_bounds_check_degrees(m_ll.lat, m_ll.lon,
      m_ur.lat, m_ur.lon, m_start.lat, m_start.lon);
   end_flags = GEO_bounds_check_degrees(m_ll.lat, m_ll.lon,
      m_ur.lat, m_ur.lon, m_end.lat, m_end.lon);

   // quick reject - both end points are off the map in the same way,
   // e.g., both North, East, South, or West of the map.
   if (start_flags & end_flags)
   {
      if (clip)
      {
         line_count = 0;
         return FALSE;
      }
      else
      {
         line_count = 1;

         points[0] = m_start;
         points[1] = m_end;
         return TRUE;
      }


   }

   // quick accept - but you must check for wrap around
   if ((start_flags | end_flags) == 0)
   {
      // if you wrap around the world
      if (wrap_around() && GEO_lon_in_range(m_start.lon, m_end.lon, m_ur.lon))
      {
         line_count = 2;

         // the first line starts at m_start
         points[0] = m_start;

         // and ends when the line crosses over the bounds' eastern edge
         points[1].lat = get_lat_at_lon(m_ur.lon);
         points[1].lon = m_ur.lon;

         // the second line starts when the line crosses over the bounds'
         // western edge
         points[2].lat = get_lat_at_lon(m_ll.lon);
         points[2].lon = m_ll.lon;

         // and ends at m_end
         points[3] = m_end;

         // For lines of constant latitude the get_lat_at_lon will have a small
         // round off error, which causes the intermediate latitudes to be
         // different from m_start.lat and m_end.lat.  Sometimes this difference
         // can be big enough to cause a the geo_to_surface function to return
         // different results for the intermediate points.
         if (m_start.lat == m_end.lat)
            points[1].lat = points[2].lat = m_end.lat;
      }
      else
      {
         line_count = 1;

         points[0] = m_start;
         points[1] = m_end;
      }

      return TRUE;
   }

   // if the clip bounds wrap around this Mercator projection
   if (wrap_around())
   {
      // Break the clip bounds in two and clip the line against each
      // separately.  There may be 1 or 2 lines in this case.
      int count;
      d_geo_t temp_points[4];
      double temp_x;

      // First replace the left longitude in the clip region with the
      // left longitude in the Mercator projection, and clip the line
      // against that region.
      temp_x = m_west_x;
      m_west_x = get_min_x();
      if (get_clipped_points(temp_points, count, clip))
      {
         ASSERT(count == 1);
         line_count = 1;
         points[0] = temp_points[0];
         points[1] = temp_points[1];
      }
      else
         line_count = 0;
      m_west_x = temp_x;

      // Second replace the right longitude in the clip region with the
      // right longitude in the Mercator projection, and clip the line
      // against that region.
      temp_x = m_east_x;
      m_east_x = get_max_x();
      if (get_clipped_points(temp_points, count, clip))
      {
         int i = 2 * line_count;

         ASSERT(count == 1);
         line_count++;
         points[i] = temp_points[0];
         points[i + 1] = temp_points[1];
      }
      m_east_x = temp_x;

      // TRACE("wrap with start_flags = %d, end_flags = %d, "
      //    "and line_count = %d\n", start_flags, end_flags, line_count);

      return (line_count != 0);
   }
   else
   {
      points[0] = m_start;
      points[1] = m_end;

      // the line crosses the clip region, there is one line
      if (clip_line(start_flags ^ end_flags, points[0], points[1]))
      {
         line_count = 1;
         return TRUE;
      }
   }

   line_count = 0;
   return FALSE;
}

degrees_t RhumbLineClipper::get_lat_at_lon(degrees_t lon)
{
   double x1, y1, x2, y2;     // Mercator coordinates for P1 and P2
   double dx, dy;             // run and rise

   // convert lat-lon values to Mercator coordinates
   x1 = lon_to_x(m_start.lon);
   y1 = lat_to_y(m_start.lat);
   x2 = lon_to_x(m_end.lon);
   y2 = lat_to_y(m_end.lat);

   dx = x2 - x1;  // run
   dy = y2 - y1;  // rise

   ASSERT(x1 <= x2);

   // if finite slope
   if (dx != 0.0)
   {
      double b;
      double m;
      double y;

      // y = mx + b
      m = dy / dx;
      b = y1 - m * x1;
      y = m * lon_to_x(lon) + b;

      if (y1 < y2)
         ASSERT(y >= y1 && y <= y2);
      else
         ASSERT(y <= y1 && y >= y2);

      return y_to_lat(y);
   }

   return m_start.lat;
}

boolean_t RhumbLineClipper::clip_line(int xor, d_geo_t &p1, d_geo_t &p2)
{
   double x1, y1, x2, y2;     // Mercator coordinates for P1 and P2
   double tE, tL;             // t at entering and leaving points
   double dx, dy;             // run and rise

   ASSERT(m_west_x < m_east_x);
   ASSERT(m_south_y < m_north_y);

   // convert lat-lon values to Mercator coordinates
   x1 = lon_to_x(p1.lon);
   y1 = lat_to_y(p1.lat);
   x2 = lon_to_x(p2.lon);
   y2 = lat_to_y(p2.lat);

   // parametric line P(t) = P1 + (P2 - P1)t
   dx = x2 - x1;  // run
   dy = y2 - y1;  // rise
   tE = 0.0;      // t at P1
   tL = 1.0;      // t at P2

   // clip to western edge if crossed
   if (xor & GEO_WEST_OF)
   {
      if (clip_t(dx, m_west_x - x1, tE, tL) == FALSE)
         return FALSE;
   }

   // clip to eastern edge if crossed
   if (xor & GEO_EAST_OF)
   {
      if (clip_t(-dx, x1 - m_east_x, tE, tL) == FALSE)
         return FALSE;
   }

   // clip to southern edge if crossed
   if (xor & GEO_SOUTH_OF)
   {
      if (clip_t(dy, m_south_y - y1, tE, tL) == FALSE)
         return FALSE;
   }

   // clip to northern edge if crossed
   if (xor & GEO_NORTH_OF)
   {
      if (clip_t(-dy, y1 - m_north_y, tE, tL) == FALSE)
         return FALSE;
   }

   // compute leaving intersection, if tL has moved
   if (tL < 1.0)
   {
      if (dx < 0)
         x2 = x1 + tL * dx;
      else
         x2 = x1 + tL * dx;

      if (dy < 0)
         y2 = y1 + tL * dy;
      else
         y2 = y1 + tL * dy;
   }

   // compute entering intersection, if tE has moved
   if (tE > 0.0)
   {
      if (dx < 0)
         x1 = x1 + tE * dx;
      else
         x1 = x1 + tE * dx;

      if (dy < 0)
         y1 = y1 + tE * dy;
      else
         y1 = y1 + tE * dy;
   }

   // Convert clipped Mercator coordinates back to lat-lon values
   p1.lat = y_to_lat(y1);
   p1.lon = x_to_lon(x1);
   p2.lat = y_to_lat(y2);
   p2.lon = x_to_lon(x2);

   return TRUE;
}

boolean_t RhumbLineClipper::clip_t(double denom, double num,
                                   double &tE, double &tL)
{
   double t;

   if (denom > 0.0)                 // PE intersection
   {
      t = (float)num/(float)denom;  // t at intersection
      if (t > tL)                   // PE point after leaving
         return FALSE;

      if (t > tE)                   // new PE point found
         tE = t;
   }
   else if (denom < 0.0)            // PL intersection
   {
      t = (float)num/(float)denom;  // t at intersection
      if (t < tE)                   // PL point before entering
         return FALSE;

      if (t < tL)                   // new PL point found
         tL = t;
   }
   else if (num > 0.0)              // line is parallel to edge and outside
      return FALSE;

   return TRUE;
}


boolean_t RhumbLineClipper::wrap_around()
{
   // if the clip bounds wrap around in the Mercator projection.
   return (m_east_x <= m_west_x);
}

// This function will generate a list of view coordinates for this rhumb
// line on the current map.  The point list passed to this function must
// be empty, or this function will return FAILURE.  Upon SUCCESS the
// list will be empty if no part of this rhumb line intersects the current
// map, or the list shall contain an even number of points.  Each pair of
// points defines a line in view coordinates for the current map.
int RhumbLineClipper::generate_view_points(IDrawingToolsProjection* map,
   CList <CPoint *, CPoint *> &list, boolean_t clip)
{
   d_geo_t points[4];
   int line_count;
   int cnt;

   if (list.GetCount() != 0)
   {
      ERR_report("The point list must be empty.");
      return FAILURE;
   }

   // if the line is not clipped, generate the list of view coordinates
   // for this rhumb line
   if (get_clipped_points(points, line_count, clip))
   {
      degrees_t dpp_lat, dpp_lon;

      if (map->get_vmap_degrees_per_pixel(&dpp_lat, &dpp_lon) != SUCCESS)
      {
         ERR_report("get_vmap_degrees_per_pixel() failed.");
         return FAILURE;
      }

      int i=0;
      while (i <= line_count)
      {
         cnt = list.GetCount();
         get_view_point(map, dpp_lat, dpp_lon, points[i], points[i + 1], list);

         i += 2;
      }
   }

   return SUCCESS;
}

// Compute the number of line segments needed to draw a rhumb line.
int RhumbLineClipper::number_of_segments( const d_geo_t &p1, const d_geo_t &p2,
         degrees_t dpp_lat, degrees_t dpp_lon, double dx, double dy,
         double x1, double y1, double x2, double y2 )
{

   // Temporary fix. Need to change the coordinates space from Mercator to
   // surface to do the thinning
   return 20;

   // Same hemisphere
   if ( p1.lat * p2.lat >= 0 )
   {
      double delta_x, delta_y;
      degrees_t mid_lon, mid_lat;
      degrees_t geo_width;

      // compute the mid point latitude for this line
      mid_lat = (p1.lat + p2.lat) / 2.0;
      if ((mid_lat + dpp_lat) < 89.99)
         delta_y = lat_to_y(mid_lat + dpp_lat) - lat_to_y(mid_lat);
      else
         delta_y = lat_to_y(mid_lat) - lat_to_y(mid_lat - dpp_lat);

      // compute the longitude spanned by this line
      geo_width = p2.lon - p1.lon;
      if (geo_width < 0.0)
         geo_width += WORLD_DEG;

      // compute the mid point longitude of this line
      mid_lon = p1.lon + geo_width / 2.0;
      if (mid_lon > HALF_WORLD_DEG)
         mid_lon -= WORLD_DEG;

      // set this longitude mid point as the new center
      // longitude for Mercator projection
      set_center_lon(mid_lon);


      // compute the width of a pixel at the middle longitude defined by the end
      // points of this line
      delta_x = lon_to_x(mid_lon + dpp_lon);

      // pixel length in the direction of the geo line in Mercator coords
      double pixel_length = sqrt(delta_x * delta_x + delta_y * delta_y);

      // line length of the geo line in Mercator coords
      double line_length = sqrt(dx * dx + dy * dy);

      // line length in device coords
      double line_length_in_pixels = line_length/pixel_length;


      // Convert the mid point of the geographic line to Mercator coordinates
      double xm = lon_to_x(mid_lon);
      double ym = lat_to_y(mid_lat);


      // Compute the distance (in pixels) of the mid point to the Mercator line
      CFvwUtil *futil = CFvwUtil::get_instance();
      double distance_in_pixels =
            futil->distance_to_line(x1, y1, x2, y2, xm, ym ) / pixel_length;

      double pixels_per_segment;

      if ( distance_in_pixels <= 1.0 )
         pixels_per_segment = line_length_in_pixels;
      else
         pixels_per_segment =  line_length_in_pixels / distance_in_pixels;

      // Bound the pixels per segment
      if ( pixels_per_segment < 20 )
         pixels_per_segment = 20;

      // number of segments for this rhumb line
      int segments = (int)(line_length_in_pixels / pixels_per_segment);

      return segments;
   }
   // Cross Equator
   else
   {
      d_geo_t p_equator;
      double x_equator, y_equator = 0.0;

      double m;
      if ( dx != 0.0 )
      {
         m = dy / dx;
         x_equator = x2 - (1/m)*(y2-y_equator);
         p_equator.lon = x_to_lon(x_equator);
      }
      else
      {
         x_equator = x2;
         p_equator.lon = p2.lon;
      }

      p_equator.lat = 0.0;

      double dx1, dy1, dx2, dy2;
      dx1 = (x_equator - x1);
      dy1 = (y_equator - y1);
      dx2 = (x2 - x_equator);
      dy2 = (y2 - y_equator);

      // compute two individual segment values....
      return number_of_segments( p1, p_equator, dpp_lat, dpp_lon,
                     dx1, dy1, x1, y1, x_equator, y_equator ) +
            number_of_segments( p_equator, p2, dpp_lat, dpp_lon,
                     dx2, dy2, x_equator, y_equator, x2, y2 );
   }
}


// Adds the view coordinates for the given pair of points to list.  Returns
// FALSE if the given points result in no points being added.  This function
// assumes that p1 and p2 have already been clipped.  It is meant to be used
// with the points returned by get_clipped_points().
boolean_t RhumbLineClipper::get_view_point(IDrawingToolsProjection* map,
   degrees_t dpp_lat, degrees_t dpp_lon, d_geo_t p1, d_geo_t p2,
   CList <CPoint *, CPoint *> &list)
{
   double x1, y1, x2, y2;     // Mercator coordinates for P1 and P2
   double dx, dy;             // run and rise

   // Original version
   degrees_t mid_lat;
   double delta_x, delta_y;

   int steps;
   double delta_t;
   BOOL on_screen = FALSE;
   d_geo_t scr_ll, scr_ur, line_ll, line_ur;

   map->get_vmap_bounds(&scr_ll, &scr_ur);
   line_ll.lat = p1.lat;
   line_ur.lat = p2.lat;
   if (p2.lat < p1.lat)
   {
      line_ll.lat = p2.lat;
      line_ur.lat = p1.lat;
   }
   line_ll.lon = p1.lon;
   line_ur.lon = p2.lon;
   if (GEO_east_of_degrees(p1.lon, p2.lon))
   {
      line_ll.lon = p2.lon;
      line_ur.lon = p1.lon;
   }
   if (GEO_intersect_degrees(line_ll, line_ur, scr_ll, scr_ur))
      on_screen = TRUE;

   // convert lat-lon values to Mercator coordinates
   x1 = lon_to_x(p1.lon);
   y1 = lat_to_y(p1.lat);
   x2 = lon_to_x(p2.lon);
   y2 = lat_to_y(p2.lat);

   // parametric line P(t) = P1 + (P2 - P1)t
   dx = x2 - x1;  // run
   dy = y2 - y1;  // rise

   // if this line isn't a line after all
   if (dx == 0.0 && dy == 0.0)
      return FALSE;

   // Old implementation

   // compute the height of a pixel at the middle latitude defined by the end
   // points of this line
   mid_lat = (p1.lat + p2.lat) / 2.0;
   if ((mid_lat + dpp_lat) < 89.99)
      delta_y = lat_to_y(mid_lat + dpp_lat) - lat_to_y(mid_lat);
   else
      delta_y = lat_to_y(mid_lat) - lat_to_y(mid_lat - dpp_lat);

   // compute the width of a pixel at the middle longitude defined by the end
   // points of this line
   delta_x = lon_to_x(get_center_lon() + dpp_lon);


   // The rhumb line path from P1 to P2 will be approximated by steps line
   // segments.  steps will be determined the length of the line in pixels.
   steps = (int)
      sqrt((dx * dx + dy * dy) / (delta_x * delta_x + delta_y * delta_y)) / 20;

   if (!on_screen)
      steps = 5;

   if (steps > 1000)
      steps = 1000;

   if (steps > 1)
      delta_t = 1.0 / (double)steps;
   else
      delta_t = 1.0;

   //TRACE("steps = %d, dx = %lf, dy = %lf, pix_width = %lf, "
   //   "pix_height = %lf\n", steps, dx, dy, delta_x, delta_y);

   // For lines of constant latitude the y_to_lat will have a small
   // round off error, which causes the intermediate latitudes to be
   // different from p1.lat and p2.lat.  Sometimes this difference
   // can be big enough to cause a the geo_to_surface function to return
   // different y values for the intermediate points.
   bool changing_lat = (p1.lat != p2.lat);

   degrees_t prev_lat;
   degrees_t prev_lon;

   // compute the end-points for each of the approximating line segments, and
   // add them to list
   int init_count = list.GetCount();
   double t = delta_t;
   degrees_t lat = p1.lat;
   degrees_t lon = p1.lon;
   for (int i=1; i<steps; i++)
   {
      // save previous lat-lon
      prev_lat = lat;
      prev_lon = lon;

      // calculate the next lat-lon
      lon = x_to_lon(x1 + dx * t);
      if (changing_lat)
         lat = y_to_lat(y1 + dy * t);

      // get the points for this line segment
      MAP_geo_line_points line_points;
      MAP_calc_geo_line(map, prev_lat, prev_lon, lat, lon, &line_points);
      const int line_count = line_points.num_lines();

      // 0, 1, or 2 lines were drawn.
      // Always add all points to the point list, eventhough this will usually
      // mean each point is in the list twice.  Doing it this way will handle
      // all special cases in a straight forward way.
      if (line_count == 1)
      {
         int x1, y1, x2, y2;
         line_points.get_saved_line(0, &x1, &y1, &x2, &y2);
         list.AddTail(new CPoint(x1, y1));
         list.AddTail(new CPoint(x2, y2));
      }
      else if (line_count == 2)
      {
         int a_x1, a_y1, a_x2, a_y2;
         line_points.get_saved_line(0, &a_x1, &a_y1, &a_x2, &a_y2);
         int b_x1, b_y1, b_x2, b_y2;
         line_points.get_saved_line(1, &b_x1, &b_y1, &b_x2, &b_y2);
         list.AddTail(new CPoint(a_x1, a_y1));
         list.AddTail(new CPoint(a_x2, a_y2));
         list.AddTail(new CPoint(b_x1, b_y1));
         list.AddTail(new CPoint(b_x2, b_y2));
      }

      t += delta_t;
   }

   // get the points for the last segment
   MAP_geo_line_points line_points;
   MAP_calc_geo_line(map, lat, lon, p2.lat, p2.lon, &line_points);
   const int line_count = line_points.num_lines();

   // 0, 1, or 2 lines were drawn.
   // Always add all points to the point list, eventhough this will usually
   // mean each point is in the list twice.  Doing it this way will handle
   // all special cases in a straight forward way.
   if (line_count == 1)
   {
      int x1, y1, x2, y2;
      line_points.get_saved_line(0, &x1, &y1, &x2, &y2);
      list.AddTail(new CPoint(x1, y1));
      list.AddTail(new CPoint(x2, y2));
   }
   else if (line_count == 2)
   {
      int a_x1, a_y1, a_x2, a_y2;
      line_points.get_saved_line(0, &a_x1, &a_y1, &a_x2, &a_y2);
      int b_x1, b_y1, b_x2, b_y2;
      line_points.get_saved_line(1, &b_x1, &b_y1, &b_x2, &b_y2);
      list.AddTail(new CPoint(a_x1, a_y1));
      list.AddTail(new CPoint(a_x2, a_y2));
      list.AddTail(new CPoint(b_x1, b_y1));
      list.AddTail(new CPoint(b_x2, b_y2));
   }

   return (list.GetCount() > init_count);
}


