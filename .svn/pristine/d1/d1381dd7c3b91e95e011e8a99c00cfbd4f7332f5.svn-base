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



// bldlist.h

// These clases fill in the CPoint list for the GeoLine class.

#ifndef BUILD_POINT_LIST_H
#define BUILD_POINT_LIST_H 1

#include "geo_tool/geo_tool.h"
#include "utils.h"      // for ILineSegmentEnumerator
#include "FvMappingGraphics/Include/GeographicContourIterator.h"

// This class will eventually be deprecated in favor of the CGreatCirclePoints/CGreatCircleLineSegments classes above.
//
class GreatCirclePointList
{
private:
   CList <CPoint *, CPoint *> &m_point_list;
   boolean_t &m_allow_redraw;

public:
   GreatCirclePointList(CList <CPoint *, CPoint *> &list, 
      boolean_t &allow_redraw);

   // Fills in m_point_list with the points for a great circle line from p1 to 
   // p2.  If wrap it TRUE, the line goes off the eastern edge of the map and 
   // back onto the western edge of the map.
   //
   // IMPORTANT: p2 must be east of p1, or the points must share a common
   // longitude.
   int generate_points(IDrawingToolsProjection* map, d_geo_t map_ll, d_geo_t map_ur,
         d_geo_t p1, d_geo_t p2, boolean_t wrap, boolean_t clip);

private:
   // compute the angles need by calc_intermediate_geo to compute geo's along
   // the great circle from lat1, lon1 to lat2, lon2
   int get_angles(degrees_t lat1, degrees_t lon1, 
      degrees_t lat2, degrees_t lon2,
      radians_t *beta, radians_t *rotate_x, radians_t *rotate_y, 
      radians_t *angle1);

   // Compute a lat-lon for a point beta radians from P1, where beta is the
   // angle between P1 and the desired point.  The inputs angles come from
   // the get_angles function, so the points P1, P2, and lat, lon all lie
   // along the same great circle
   int calc_intermediate_geo(radians_t beta, 
      radians_t rotate_x, radians_t rotate_y, radians_t angle_p1,
      degrees_t *lat, degrees_t *lon);

   // calculate xyz coordinates from latitude and longitude, assuming the
   // earth is a perfect sphere with radius 6378.0 km
   int lat_lon_to_xyz(degrees_t latitude, degrees_t longitude,
      double *x, double *y, double *z);

   // calculate latitude and longitude from the xyz coordinate, assuming the
   // earth is a perfect sphere with a radius 6378.0 km
   int xyz_to_lat_lon(double x, double y, double z,
      degrees_t *latitude, degrees_t *longitude);

   // given to vectors, P1(x1, y1, z1) and P2(x2, y2, z2), compute the CCW
   // angle between them, beta.  In addition, compute rotate_y and rotate_x,
   // which define the coordinate transformation from the earth's system to
   // a rotated system with P1 and P2 in the x' - y' plane.
   int get_parameters(double x1, double y1, double z1,
      double x2, double y2, double z2, 
      radians_t *beta, radians_t *rotate_x, radians_t *rotate_y);

   // Rotate the point P(x, y, z) in the same way the normal vector N would 
   // have to be rotated to align it with the z-axis, where N is the normal
   // vector to the P1 x P2.  The values for rotate_x and rotate_y for this
   // normal vector are computed by the get_parameters member function, above.
   // Performs Ry(rotate_y) Rx(-rotate_x) P = P'.
   int forward_conversion(double x, double y, double z, 
      radians_t rotate_x, radians_t rotate_y,
      double *x_out, double *y_out, double *z_out);

   // Opposite of forward_conversion. Performs Rx(rotate_x) Ry(rotate_y) P' = P
   int reverse_conversion(double x, double y, double z, 
      radians_t rotate_x, radians_t rotate_y,
      double *x_out, double *y_out, double *z_out);

   // get the angle with the x' - y' plane made by the given lat, lon
   int get_angle(degrees_t lat, degrees_t lon, 
      radians_t rotate_x, radians_t rotate_y, radians_t angle_p1,
      radians_t *angle, double *z_out);
   
   // compute the default angle step size from the map resolution (dpp)
   int get_delta_angle(IDrawingToolsProjection* map, radians_t *delta_angle);

   // compute a good starting angle for the search for the cross over angle
   // from outside the map to inside the map
   radians_t compute_starting_angle(IDrawingToolsProjection* map, d_geo_t map_ll, d_geo_t map_ur,
      radians_t rotate_x, radians_t rotate_y, radians_t angle_p1);
}; // End GreatCirclePointList


class RhumbLineClipper : private Mercator
{
private:
   d_geo_t m_start;
   d_geo_t m_end;
   d_geo_t m_ll;
   d_geo_t m_ur;
   double m_west_x;
   double m_east_x;
   double m_south_y;
   double m_north_y;

public:
   // Constructor - create an instance of this class for a particular
   // rhumb line.
   RhumbLineClipper(d_geo_t p1, d_geo_t p2);

   // Define the Geo-bounds against which this rhumb line will be clipped.
   int set_clip_bounds(d_geo_t ll, d_geo_t ur);

   // This function will generate a list of view coordinates for this rhumb
   // line on the current map.  The point list passed to this function must
   // be empty, or this function will return FAILURE.  Upon SUCCESS the
   // list will be empty if no part of this rhumb line intersects the current
   // map, or the list shall contain an even number of points.  Each pair of
   // points defines a line in view coordinates for the current map.
   int generate_view_points(IDrawingToolsProjection* map, CList <CPoint *, CPoint *> &list, boolean_t clip = TRUE);

   // Returns the clipped end points for this rhumb line.  If no part of
   // the line is in the bounds, then line_count is set to zero and FALSE
   // is returned.  If all or part of the line is in the bounds, then
   // this function will return TRUE, line_count will be set to 1 or 2,
   // and points will be filled in with the points for the 1 or 2 lines.
   boolean_t get_clipped_points(d_geo_t points[4], int &line_count, boolean_t clip = TRUE);

private:
   degrees_t get_lat_at_lon(degrees_t lon);
   boolean_t clip_line(int xor, d_geo_t &p1, d_geo_t &p2);
   boolean_t clip_t(double denom, double num, 
                                   double &tE, double &tL);

   // Adds the view coordinates for the given pair of points to list.  Returns
   // FALSE if the given points result in no points being added.  This function
   // assumes that p1 and p2 have already been clipped.  It is meant to be used
   // with the points returned by get_clipped_points().
   boolean_t get_view_point(IDrawingToolsProjection* map, degrees_t dpp_lat, degrees_t dpp_lon,
   d_geo_t p1, d_geo_t p2, CList <CPoint *, CPoint *> &list);

   // Returns TRUE if the clip-bounds wrap around this Mercator projection.
   // Returns FALSE otherwise.
   boolean_t wrap_around();

   // Compute the number of line segments needed to draw a rhumb line.
   int number_of_segments( const d_geo_t &p1, const d_geo_t &p2, 
         degrees_t dpp_lat, degrees_t dpp_lon, double dx, double dy, 
         double x1, double y1, double x2, double y2 );

}; // End RhumbLineClipper

#endif
