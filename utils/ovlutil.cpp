// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// ovlutil.cpp

#include "stdafx.h"
#include "ovlutil.h"
#include "mem.h"
#include "ovl_mgr.h"
#include "..\proj\Projectors.h"

double angle_inc[80] = {000.0, 000.25, 000.5, 001.0, 002.0,
                        004.0,  008.0, 015.0, 024.0, 034.0,
                        045.0,  056.0, 066.0, 075.0, 082.0,
                        086.0,  088.0, 089.0, 089.5, 089.75,
                        090.0, 090.25, 090.5, 091.0, 092.0,
                        094.0,  098.0, 105.0, 114.0, 124.0,
                        135.0,  146.0, 156.0, 165.0, 172.0,
                        176.0,  178.0, 179.0, 179.5, 179.75,
                        180.0, 180.25, 180.5, 181.0, 182.0,
                        184.0,  188.0, 195.0, 204.0, 214.0,
                        225.0,  236.0, 246.0, 255.0, 262.0,
                        266.0,  268.0, 269.0, 269.5, 269.75,
                        270.0, 270.25, 270.5, 271.0, 272.0,
                        274.0,  278.0, 285.0, 294.0, 304.0,
                        315.0,  326.0, 336.0, 345.0, 352.0,
                        356.0,  358.0, 359.0, 359.5, 359.75};

COvlkitUtil::COvlkitUtil()
{
}

COLORREF COvlkitUtil::code2color(int code)
{
   CFvwUtil *util = CFvwUtil::get_instance();

   return util->code2color(code);
}
// end of code2color

// *************************************************************
// *************************************************************

int COvlkitUtil::color2code(COLORREF color)
{
   CFvwUtil *util = CFvwUtil::get_instance();

   return util->color2code(color);
}

// *************************************************************
// *************************************************************

void COvlkitUtil::draw_line(MapProj* map, CDC* dc,
                         int x1, int y1,  // beginning point
                         int x2, int y2,  // ending point
                         int color,  // code for color, -1 for XOR line
                         int backcolor,  // code for backcolor, -1 for none
                         int style,  // code for style
                         int width)  // line width
{
   if (color == -1)
      style = UTIL_LINE_XOR;

   UtilDraw dutil(dc);

   if (backcolor > -1)
   {
      dutil.set_pen(backcolor, style, width + 2);
      dutil.draw_line(x1, y1, x2, y2);
   }
   dutil.set_pen(color, style, width);
   dutil.draw_line(x1, y1, x2, y2);
}
// end of draw_line

// *************************************************************
// *************************************************************

void COvlkitUtil::draw_line_rgb(MapProj* map, CDC* dc,
                         int x1, int y1,  // beginning point
                         int x2, int y2,  // ending point
                         COLORREF color,  // code for color, -1 for XOR line
                         COLORREF backcolor,  // code for backcolor, -1 for none
                         int style,  // code for style
                         int width)  // line width
{
   if (color == -1)
      style = UTIL_LINE_XOR;

   UtilDraw dutil(dc);

   if (backcolor > -1)
   {
      dutil.set_pen(backcolor, style, width + 2);
      dutil.draw_line(x1, y1, x2, y2);
   }
   dutil.set_pen(color, style, width);
   dutil.draw_line(x1, y1, x2, y2);
}
// end of draw_line_rgb

// *************************************************************
// *************************************************************

void COvlkitUtil::geo_draw_line(MapProj* map, CDC* dc,
                         degrees_t lat1,
                         degrees_t lon1,
                         degrees_t lat2,
                         degrees_t lon2,
                         int color,  // code for color, -1 for XOR line
                         int backcolor,  // code for backcolor, -1 for none
                         int style,  // code for style
                         int line_width)  // line width in pixels, if < 0 in meters
{
   d_geo_t ll, ur;
   int x1, y1;          /* device coordinates of western point */
   int x2, y2;          /* device coordinates of eastern point */
   int p1_flags;        /* NSEW bounds check flags for point 1 */
   int p2_flags;        /* NSEW bounds check flags for point 2 */
   boolean_t east_of;   /* point 2 is east of point 1 */
   int pixels_around_world;
   int screen_width, screen_height;
   int width;
   BOOL reversed;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (line_width > 0)
      width = line_width;
   else
      width = futil->get_calculated_width(map, -line_width, lat1, lon1);

   // clip width so that 1 <= width <= 9
   if (width < 1)
      width = 1;
   if (width > 14)
      width = 14;

   // check for screen intersection
   map->get_vmap_bounds(&ll, &ur);
   map->get_pixels_around_world(&pixels_around_world);
   map->get_vsurface_size(&screen_width, &screen_height);

   // treat world overview as a special case
   if (screen_width >= (pixels_around_world/ 2))
   {
      // get device coordinates
      if (GEO_east_of_degrees(lon2, lon1))
      {
         futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
         futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
      }
      else         /* p2 is west of p1 or lon1=lon2 */
      {
         futil->geo_to_view_short(map, lat2, lon2, &x1, &y1);
         futil->geo_to_view_short(map, lat1, lon1, &x2, &y2);
      }

      if (x1 > x2)
      {
         // wrap around world
         draw_line(map, dc, x1, y1, x2 + pixels_around_world, y2,
               color, backcolor, style, width);
         draw_line(map, dc, x1 - pixels_around_world, y1, x2, y2,
               color, backcolor, style, width);
      }
      else
         draw_line(map, dc, x1, y1, x2, y2, color, backcolor, style, width);

      return;
   }

   // get NSEW bounds check flags for each point
   p1_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
      ur.lat, ur.lon, lat1, lon1);
   p2_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
      ur.lat, ur.lon, lat2, lon2);

   // quick accept - both points on map
   if ((p1_flags | p2_flags) == 0)
   {
      // get device coordinates
      futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
      futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
//      geo_to_view(map, lat1, lon1, &x1, &y1);
//      geo_to_view(map, lat2, lon2, &x2, &y2);

      draw_line(map, dc, x1, y1, x2, y2, color, backcolor, style, width);

      return;
   }

   // quick reject
   if (p1_flags & p2_flags)
      return;

   // reject lines that go from west of map to east of map but
   // not through the region defined by left_lon and right_lon
   if (((p1_flags ^ p2_flags) & (GEO_EAST_OF | GEO_WEST_OF)) ==
      (GEO_EAST_OF | GEO_WEST_OF))
   {
      if (p1_flags & GEO_EAST_OF)
      {
         if (GEO_east_of_degrees(lon2, lon1))
            return;

         // needed to get device coordinates in right order
         east_of = FALSE;
      }
      else
      {
         if (GEO_east_of_degrees(lon1, lon2))
            return;

         // needed to get device coordinates in right order
         east_of = TRUE;
      }
   }
   else
      east_of = GEO_east_of_degrees(lon2, lon1);

   // get device coordinates
   if (east_of)
   {
      futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
      futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
//      geo_to_view(map, lat1, lon1, &x1, &y1);
//      geo_to_view(map, lat2, lon2, &x2, &y2);
      reversed = FALSE;
   }
   else
   {
      futil->geo_to_view_short(map, lat2, lon2, &x1, &y1);
      futil->geo_to_view_short(map, lat1, lon1, &x2, &y2);
//      geo_to_view(map, lat2, lon2, &x1, &y1);
//      geo_to_view(map, lat1, lon1, &x2, &y2);
      reversed = TRUE;
   }

   if (reversed)
      draw_line(map, dc, x2, y2, x1, y1, color, backcolor, style, width);
   else
      draw_line(map, dc, x1, y1, x2, y2, color, backcolor, style, width);

}
// end of geo_draw_line

// *************************************************************
// *************************************************************

void COvlkitUtil::geo_draw_line_2(MapProj* map, CDC* dc,
                         degrees_t lat1,
                         degrees_t lon1,
                         degrees_t lat2,
                         degrees_t lon2,
                         int color,  // code for color, -1 for XOR line
                         int backcolor,  // code for backcolor, -1 for none
                         int style,  // code for style
                         int line_width,  // line width in pixels, if < 0 in meters
                         int line_type)   // simple=1, rhumb=2, great_circle=3
{
   if (line_width == 0)  // don't try to draw zero width lines
      return;

   CFvwUtil *futil = CFvwUtil::get_instance();

   futil->draw_geo_line(map, dc, lat1, lon1, lat2, lon2, color, style,
      line_width, line_type, TRUE, backcolor >= 0);
}
// end of geo_draw_line_2

// *************************************************************
// *************************************************************

// draw a polygon base on sides that are drawn using points from geoline

int COvlkitUtil::geo_draw_polygon_rgb(MapProj* map, CDC* dc, int numverts,
                        degrees_t *plat, degrees_t *plon,
                        COLORREF color,  // rgb color, -1 for XOR line
                        COLORREF backcolor,  // code for backcolor, -1 for none
                        int style,  // code for style
                        int line_width,  // line width in pixels, if < 0 in meters
                        int line_type,  // simple=1, rhumb=2, great_circle=3
                        COLORREF fill_color,
                        int fill_style)
{
   POINT *pt = NULL;
   int num_points, rslt;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (fill_style == UTIL_FILL_NONE)
      return SUCCESS;

   if (numverts < 3)
      return FAILURE;

   if (!geo_polygon_on_screen(map, numverts, plat, plon))
      return FAILURE;

   if (line_type == 1)
   {
      pt = (POINT*) malloc(numverts * sizeof(POINT));
      num_points = numverts;
      rslt = geo_polygon_to_view(map, numverts, plat, plon, pt);
   }
   else
   {
      rslt = geo_polygon_to_view(map, numverts, line_type, plat, plon, &num_points, &pt);
   }

   if (rslt != SUCCESS)
   {
      return FAILURE;
   }

   int surface_width = 0, surface_height = 0;
   if (map)
      map->get_surface_size(&surface_width, &surface_height);
   futil->fill_polygon(dc, pt, num_points, fill_color, fill_style,
                       surface_width, surface_height);

   if (pt != NULL)
      free(pt);

   return SUCCESS;
}
// end of geo_draw_polygon

// *************************************************************
// *************************************************************

// draw a polygon base on sides that are drawn using points from geoline

int COvlkitUtil::geo_draw_polygon(MapProj* map, CDC* dc, int numverts,
                        degrees_t *plat, degrees_t *plon,
                        int color,  // code for color, -1 for XOR line
                        int backcolor,  // code for backcolor, -1 for none
                        int style,  // code for style
                        int line_width,  // line width in pixels, if < 0 in meters
                        int line_type,   // simple=1, rhumb=2, great_circle=3
                        int fill_color,
                        int fill_style)
{
   COLORREF rgb_color, rgb_color_back, rgb_color_fill;
   int rslt;
   CFvwUtil *futil = CFvwUtil::get_instance();

   rgb_color = futil->code2color(color);
   rgb_color_back = futil->code2color(backcolor);
   rgb_color_fill = futil->code2color(fill_color);

   if (color == -1)
      rgb_color = -1;
   if (backcolor == -1)
      rgb_color_back = -1;
   if (fill_color == -1)
      rgb_color_fill = -1;

   rslt = geo_draw_polygon_rgb(map, dc, numverts, plat, plon, rgb_color, rgb_color_back, style,
                  line_width, line_type, rgb_color_fill, fill_style);

   return rslt;
}
// end of geo_draw_polygon

// *************************************************************
// *************************************************************

// draw a polygon base on sides that are drawn using points from geoline
// get multiple points per side based on line type

int COvlkitUtil::geo_draw_polygon1(MapProj* map, CDC* dc, int numverts,
                        degrees_t *plat, degrees_t *plon,
                        int color,  // code for color, -1 for XOR line
                        int backcolor,  // code for backcolor, -1 for none
                        int style,  // code for style
                        int line_width,  // line width in pixels, if < 0 in meters
                        int line_type,   // simple=1, rhumb=2, great_circle=3
                        int fill_color,
                        int fill_style)
{
   POINT *pt = NULL;
   int num_points, rslt;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (numverts < 3)
      return FAILURE;

   if (!geo_polygon_on_screen(map, numverts, plat, plon))
      return FAILURE;

   if (fill_style == UTIL_FILL_NONE)
      return SUCCESS;

   rslt = geo_polygon_to_view(map, numverts, line_type, plat, plon, &num_points, &pt);
   if (rslt != SUCCESS)
   {
      return FAILURE;
   }

   int surface_width = 0, surface_height = 0;
   if (map)
      map->get_surface_size(&surface_width, &surface_height);

   futil->fill_polygon(dc, pt, num_points, fill_color, fill_style,
      surface_width, surface_height);

   if (pt != NULL)
      free(pt);

   return SUCCESS;
}
// end of geo_draw_polygon1

// *************************************************************
// *************************************************************

// draw a polygon base on sides that are drawn using points from geoline

int COvlkitUtil::geo_draw_polygon2(MapProj* map, CDC* dc, int numverts,
                        degrees_t *plat, degrees_t *plon,
                        int color,  // code for color, -1 for XOR line
                        int backcolor,  // code for backcolor, -1 for none
                        int style,  // code for style
                        int line_width,  // line width in pixels, if < 0 in meters
                        int *line_type,   // simple=1, rhumb=2, great_circle=3
                        int fill_color,
                        int fill_style)
{
   POINT *pt;
   int num_points;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (numverts < 3)
      return FAILURE;

   if (!geo_polygon_on_screen(map, numverts, plat, plon))
      return FAILURE;

   const int rslt = geo_polygon_to_view(map, numverts, line_type, plat, plon, &num_points, &pt);
   if (rslt != SUCCESS)
   {
      return FAILURE;
   }

   if ((fill_style != UTIL_FILL_NONE))
   {
      int surface_width = 0, surface_height = 0;
      if (map)
         map->get_surface_size(&surface_width, &surface_height);

      futil->fill_polygon(dc, pt, num_points, fill_color, fill_style,
         surface_width, surface_height);
   }

   free(pt);

   return SUCCESS;
}
// end of geo_draw_polygon

// *************************************************************
// *************************************************************

// invalidate a polygon that are drawn using points from geoline

int COvlkitUtil::invalidate_geo_polygon(MapProj* map, int numverts, degrees_t *plat, degrees_t *plon, int line_type)
{
   GeoSegment::heading_type_t rhumb_line = GeoSegment::RHUMB_LINE;
   GeoSegment::heading_type_t gc_line = GeoSegment::GREAT_CIRCLE;
   CList <CPoint *, CPoint *> list;
   int minx, miny, maxx, maxy;
   int  k, x1, y1, x2, y2;
   int screen_width, screen_height;
   GeoLine line;
   CPoint *lpt;
   BOOL onscreen = TRUE;

   if (numverts < 1)
      return FAILURE;

   map->get_vsurface_size(&screen_width, &screen_height);

   map->geo_to_surface(plat[0], plon[0], &minx, &miny);
   maxx = minx;
   maxy = miny;

   // check for all lines onscreen
   for (k=0; k<numverts-1; k++)
   {
      map->geo_to_surface(plat[k], plon[k], &x1, &y1);
      map->geo_to_surface(plat[k+1], plon[k+1], &x2, &y2);
      if ((x1 < 0) && (x2 < 0))
         onscreen = FALSE;
      if ((y1 < 0) && (y2 < 0))
         onscreen = FALSE;
      if ((x1 > screen_width) && (x2 > screen_width))
         onscreen = FALSE;
      if ((y1 > screen_height) && (y2 > screen_height))
         onscreen = FALSE;
   }

   if (onscreen && (line_type != UTIL_LINE_TYPE_SIMPLE))
   {
      for (k=0; k<numverts-1; k++)
      {
         if (line_type == UTIL_LINE_TYPE_GREAT)
            line.set_heading_type(gc_line);
         line.set_start(plat[k], plon[k]);
         line.set_end(plat[k+1], plon[k+1]);
         map->geo_to_surface(plat[k], plon[k], &x1, &y1);
         map->geo_to_surface(plat[k+1], plon[k+1], &x2, &y2);
         if ((x1 < 0) && (x2 < 0))
            onscreen = FALSE;
         if ((y1 < 0) && (y2 < 0))
            onscreen = FALSE;
         if ((x1 > screen_width) && (x2 > screen_width))
            onscreen = FALSE;
         if ((y1 > screen_height) && (y2 > screen_height))
            onscreen = FALSE;
         line.prepare_for_redraw(map);
         if (GEO_east_of_degrees(plon[k], plon[k+1]))
            line.reverse_points();

         if (line.get_points(list))
         {
            while (!list.IsEmpty())
            {
               lpt = list.RemoveHead();
               if (minx > lpt->x)
                  minx = lpt->x;
               if (maxx < lpt->x)
                  maxx = lpt->x;
               if (miny > lpt->y)
                  miny = lpt->y;
               if (maxy < lpt->y)
                  maxy = lpt->y;
               delete lpt;
            }
         }
      }

      // get points from last line
      if (line_type == UTIL_LINE_TYPE_GREAT)
         line.set_heading_type(gc_line);
      line.set_start(plat[numverts-1], plon[numverts-1]);
      line.set_end(plat[0], plon[0]);
      line.prepare_for_redraw(map);
      if (GEO_east_of_degrees(plon[numverts-1], plon[0]))
         line.reverse_points();

      if (line.get_points(list))
      {
         while (!list.IsEmpty())
         {
            lpt = list.RemoveHead();
            if (minx > lpt->x)
               minx = lpt->x;
            if (maxx < lpt->x)
               maxx = lpt->x;
            if (miny > lpt->y)
               miny = lpt->y;
            if (maxy < lpt->y)
               maxy = lpt->y;
            delete lpt;
         }
      }
   }
   else
   {
      // offscreen
      for (k=0; k<numverts; k++)
      {
         map->geo_to_surface(plat[k], plon[k], &x1, &y1);
         if (minx > x1)
            minx = x1;
         if (maxx < x1)
            maxx = x1;
         if (miny > y1)
            miny = y1;
         if (maxy < y1)
            maxy = y1;
      }
   }

   CRect rc;
   rc.SetRect(minx-7, miny-7, maxx+7, maxy+7);
   OVL_get_overlay_manager()->invalidate_rect(&rc);

   return SUCCESS;
}
// end of invalidate_geo_polygon

// *************************************************************
// *************************************************************

BOOL COvlkitUtil::geo_line_hit_test(MapProj* map,
                                    degrees_t lat1,
                                    degrees_t lon1,
                                    degrees_t lat2,
                                    degrees_t lon2,
                                    int line_width,  // line width in pixels
                                    int line_type,  // UTIL_LINE_TYPE_SIMPLE, UTIL_LINE_TYPE_RHUMB, or UTIL_LINE_TYPE_GREAT
                                    CPoint test_pt)
{
   GeoSegment::heading_type_t heading_type;
   POINT points[4];
   int num_points;
   CFvwUtil *futil = CFvwUtil::get_instance();

   switch(line_type)
   {
      case UTIL_LINE_TYPE_SIMPLE:

         get_geo_draw_line_points(map, lat1, lon1, lat2, lon2, points, &num_points);

         if (futil->distance_to_line(points[0].x, points[0].y, points[1].x, points[1].y, test_pt.x, test_pt.y) < 3)
            return TRUE;
         else if (num_points == 2)
            return FALSE;
         else if (futil->distance_to_line(points[2].x, points[2].y, points[3].x, points[3].y, test_pt.x, test_pt.y) < 3)
            return TRUE;
         else
            return FALSE;
         break;
      case UTIL_LINE_TYPE_RHUMB:
         heading_type = GeoSegment::RHUMB_LINE; break;
      case UTIL_LINE_TYPE_GREAT:
         heading_type = GeoSegment::GREAT_CIRCLE; break;
      default:
         ERR_report("Invalid line type");
         return FALSE;
   }

   GeoLine geoline(heading_type);
   geoline.set_start(lat1, lon1);
   geoline.set_end(lat2, lon2);
   geoline.get_pen().set_foreground_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, line_width);
   geoline.prepare_for_redraw(map);
   return geoline.hit_test(test_pt);
}

// *************************************************************
// *************************************************************

void COvlkitUtil::invalidate_line(int x1, int y1, int x2, int y2)
{
   const int rise = abs(y1 - y2);
   const int run  = abs(x1 - x2);

   // see if this line segment meets our minimum size requirements
   if (rise < 12  ||  run < 12)
   {
      CRect inval_rect(x1, y1, x2, y2);
      inval_rect.NormalizeRect();
      inval_rect.InflateRect(36, 36);
      OVL_get_overlay_manager()->invalidate_rect(inval_rect, FALSE);
   }
   // otherwise, compute the midpoint and invalidate the two line segments
   else
   {
      CPoint mid_point(min(x1, x2) + run/2, min(y1, y2) + rise/2);
      invalidate_line(x1, y1, mid_point.x, mid_point.y);
      invalidate_line(mid_point.x, mid_point.y, x2, y2);
   }
}

// *************************************************************
// *************************************************************

void COvlkitUtil::invalidate_geo_line(MapProj* map,
                                    degrees_t lat1,
                                    degrees_t lon1,
                                    degrees_t lat2,
                                    degrees_t lon2,
                                    int line_width,  // line width in pixels
                                    int line_type)  // UTIL_LINE_TYPE_SIMPLE, UTIL_LINE_TYPE_RHUMB, or UTIL_LINE_TYPE_GREAT
{
   GeoSegment::heading_type_t heading_type;
   int type;

   type = line_type;

   // force a valid line type
   if ((type != UTIL_LINE_TYPE_RHUMB) && (type != UTIL_LINE_TYPE_GREAT))
      type = UTIL_LINE_TYPE_SIMPLE;

   // invalidate handles at the vertexes
   CRect rc;
   int x, y;

   map->geo_to_surface(lat1, lon1, &x, &y);
   rc.top = y - 5;
   rc.bottom = y + 5;
   rc.left = x - 5;
   rc.right = x + 5;
   OVL_get_overlay_manager()->invalidate_rect(rc, FALSE);
   map->geo_to_surface(lat2, lon2, &x, &y);
   rc.top = y - 5;
   rc.bottom = y + 5;
   rc.left = x - 5;
   rc.right = x + 5;
   OVL_get_overlay_manager()->invalidate_rect(rc, FALSE);

   switch(line_type)
   {
      case UTIL_LINE_TYPE_SIMPLE:
         {
            CFvwUtil *futil = CFvwUtil::get_instance();
            POINT points[4];
            int num_points;

            get_geo_draw_line_points(map, lat1, lon1, lat2, lon2, points, &num_points);

            invalidate_line(points[0].x, points[0].y, points[1].x, points[1].y);
            if (num_points == 4)
               invalidate_line(points[2].x, points[2].y, points[3].x, points[3].y);
            return;
         }
         break;
      case UTIL_LINE_TYPE_RHUMB:
         heading_type = GeoSegment::RHUMB_LINE; break;
      case UTIL_LINE_TYPE_GREAT:
         heading_type = GeoSegment::GREAT_CIRCLE; break;
      default:
         ERR_report("Invalid line type");
   }

   GeoLine geoline(heading_type);
   geoline.set_start(lat1, lon1);
   geoline.set_end(lat2, lon2);
   geoline.get_pen().set_foreground_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, line_width);
   geoline.prepare_for_redraw(map);
   geoline.invalidate();
}

// *************************************************************
// *************************************************************

void COvlkitUtil::invalidate_poly_list(CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*> & poly_list)
{
   POSITION next;
   CRect rc;
   BOOL yet;
   POINT pt;
   fv_core_mfc::C_poly_xy *polyxy;
   int cnt1, cnt2;

   if (poly_list.IsEmpty())
      return;

   cnt1 = poly_list.GetCount();
   un_dupe_poly_list(poly_list);
   cnt2 = poly_list.GetCount();

   next = poly_list.GetHeadPosition();
   while (next != NULL)
   {
      polyxy = poly_list.GetNext(next);
      yet = polyxy->get_first_point(pt);
      if (yet)
      {
         rc.top = pt.y;
         rc.bottom = pt.y;
         rc.left = pt.x;
         rc.right = pt.x;
      }
      while (yet)
      {
         yet = polyxy->get_next_point(pt);
         if (yet)
         {
            if (pt.y < rc.top)
               rc.top = pt.y;
            if (pt.y > rc.bottom)
               rc.bottom = pt.y;
            if (pt.x < rc.left)
               rc.left = pt.x;
            if (pt.x > rc.right)
               rc.right = pt.x;
         }
      }

      rc.InflateRect(5,5);
      OVL_get_overlay_manager()->invalidate_rect(rc, FALSE);
   }
}
// invalidate_poly_list

// *************************************************************
// *************************************************************

void COvlkitUtil::un_dupe_poly_list(CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*> & poly_list)
{
   POSITION next, tpos;
   CRect rc, trc;
   BOOL yet, notdone;
   POINT pt;
   fv_core_mfc::C_poly_xy *polyxy;
   int start, cnt, total;

   if (poly_list.IsEmpty())
      return;

   total = poly_list.GetCount();
   if (total < 2)
      return;

   start = 1;
   notdone = TRUE;
   while (notdone)
   {
      cnt = 0;
      next = poly_list.GetHeadPosition();
      tpos = next;
      while (next != NULL)
      {
         polyxy = poly_list.GetNext(next);
         yet = polyxy->get_first_point(pt);
         if (yet)
         {
            rc.top = pt.y;
            rc.bottom = pt.y;
            rc.left = pt.x;
            rc.right = pt.x;
         }
         while (yet)
         {
            yet = polyxy->get_next_point(pt);
            if (yet)
            {
               if (pt.y < rc.top)
                  rc.top = pt.y;
               if (pt.y > rc.bottom)
                  rc.bottom = pt.y;
               if (pt.x < rc.left)
                  rc.left = pt.x;
               if (pt.x > rc.right)
                  rc.right = pt.x;
            }
         }
         cnt++;
         if (cnt < start)
            continue;

         if (cnt == start)
         {
            trc = rc;
         }
         else
         {
            if (rc.EqualRect(trc))
            {
               poly_list.RemoveAt(tpos);
               total--;
            }
         }
         tpos = next;
      }
      start++;
      if (start >= total)
         notdone = FALSE;
   }
}
// un_dupe_poly_list

// *************************************************************
// *************************************************************

void COvlkitUtil::clear_poly_list(CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*> & poly_list)
{
   fv_core_mfc::C_poly_xy *polyxy;

   // clear the text poly list
   while (!poly_list.IsEmpty())
   {
      polyxy = poly_list.RemoveHead();
      delete polyxy;
   }
}
// clear_poly_list

// *************************************************************
// *************************************************************

void COvlkitUtil::geo_draw_line_offset(MapProj* map, CDC* dc,
                         degrees_t lat1,
                         degrees_t lon1,
                         degrees_t lat2,
                         degrees_t lon2,
                         int offx,
                         int offy,
                         int color,  // code for color, -1 for XOR line
                         int backcolor,  // code for backcolor, -1 for none
                         int style,  // code for style
                         int line_width)  // line width in pixels, if < 0 in meters
{
   d_geo_t ll, ur;
   int x1, y1;          /* device coordinates of western point */
   int x2, y2;          /* device coordinates of eastern point */
   int p1_flags;        /* NSEW bounds check flags for point 1 */
   int p2_flags;        /* NSEW bounds check flags for point 2 */
   boolean_t east_of;   /* point 2 is east of point 1 */
   int pixels_around_world;
   int screen_width, screen_height;
   int width;
   BOOL reversed;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (line_width > 0)
      width = line_width;
   else
      width = futil->get_calculated_width(map, -line_width, lat1, lon1);

   // clip width so that 1 <= width <= 9
   if (width < 1)
      width = 1;
   if (width > 9)
      width = 9;

   // check for screen intersection
   map->get_vmap_bounds(&ll, &ur);
   map->get_pixels_around_world(&pixels_around_world);
   map->get_vsurface_size(&screen_width, &screen_height);

   // treat world overview as a special case
   if (screen_width == pixels_around_world)
   {
      // get device coordinates
      if (GEO_east_of_degrees(lon2, lon1))
      {
         futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
         futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
      }
      else         /* p2 is west of p1 or lon1=lon2 */
      {
         futil->geo_to_view_short(map, lat2, lon2, &x1, &y1);
         futil->geo_to_view_short(map, lat1, lon1, &x2, &y2);
      }

      if (x1 > x2)
      {
         // wrap around world
         draw_line(map, dc, x1+offx, y1+offy, x2+offx + pixels_around_world, y2+offy,
               color, backcolor, style, width);
         draw_line(map, dc, x1+offx - pixels_around_world, y1+offy, x2+offx, y2+offy,
               color, backcolor, style, width);
      }
      else
      {
      draw_line(map, dc, x2+offx-pixels_around_world, y2+offy, x1+offx-pixels_around_world, y1+offy, color, backcolor, style, width);
      draw_line(map, dc, x2+offx, y2+offy, x1+offx, y1+offy, color, backcolor, style, width);
      draw_line(map, dc, x2+offx+pixels_around_world, y2+offy, x1+offx+pixels_around_world, y1+offy, color, backcolor, style, width);
      }
      return;
   }

   // get NSEW bounds check flags for each point
   p1_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
      ur.lat, ur.lon, lat1, lon1);
   p2_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
      ur.lat, ur.lon, lat2, lon2);

   // quick accept - both points on map
   if ((p1_flags | p2_flags) == 0)
   {
      // get device coordinates
      futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
      futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
//      geo_to_view(map, lat1, lon1, &x1, &y1);
//      geo_to_view(map, lat2, lon2, &x2, &y2);

      draw_line(map, dc, x1+offx, y1+offy, x2+offx, y2+offy, color, backcolor, style, width);

      return;
   }

   // quick reject
//   if (p1_flags & p2_flags)
//      return;

   // reject lines that go from west of map to east of map but
   // not through the region defined by left_lon and right_lon
   if (((p1_flags ^ p2_flags) & (GEO_EAST_OF | GEO_WEST_OF)) ==
      (GEO_EAST_OF | GEO_WEST_OF))
   {
      if (p1_flags & GEO_EAST_OF)
      {
         if (GEO_east_of_degrees(lon2, lon1))
            return;

         // needed to get device coordinates in right order
         east_of = FALSE;
      }
      else
      {
         if (GEO_east_of_degrees(lon1, lon2))
            return;

         // needed to get device coordinates in right order
         east_of = TRUE;
      }
   }
   else
      east_of = GEO_east_of_degrees(lon2, lon1);

   // get device coordinates
   if (east_of)
   {
      futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
      futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
//      geo_to_view(map, lat1, lon1, &x1, &y1);
//      geo_to_view(map, lat2, lon2, &x2, &y2);
      reversed = FALSE;
   }
   else
   {
      futil->geo_to_view_short(map, lat2, lon2, &x1, &y1);
      futil->geo_to_view_short(map, lat1, lon1, &x2, &y2);
//      geo_to_view(map, lat2, lon2, &x1, &y1);
//      geo_to_view(map, lat1, lon1, &x2, &y2);
      reversed = TRUE;
   }


   if (x1 > x2)
   {
      draw_line(map, dc, x1+offx+pixels_around_world, y1+offy, x2+ (2*offx+pixels_around_world), y2+offy, color, backcolor, style, width);
      draw_line(map, dc, x1+offx, y1+offy, x2+offx + pixels_around_world, y2+offy, color, backcolor, style, width);
      draw_line(map, dc, x1+offx-pixels_around_world, y1+offy, x2+offx, y2+offy, color, backcolor, style, width);
   }
   else
   {
      draw_line(map, dc, x2+offx-pixels_around_world, y2+offy, x1+offx-pixels_around_world, y1+offy, color, backcolor, style, width);
      draw_line(map, dc, x2+offx, y2+offy, x1+offx, y1+offy, color, backcolor, style, width);
      draw_line(map, dc, x2+offx+pixels_around_world, y2+offy, x1+offx+pixels_around_world, y1+offy, color, backcolor, style, width);
   }

}
// end of geo_draw_line_offset

int geo_to_view_short(MapProj* map, degrees_t lat, degrees_t lon, int *x, int *y)
{
   return map->geo_to_surface(lat, lon, x, y);
}

int geo_to_view_short(MapProj* map, degrees_t lat, degrees_t lon, long *x, long *y)
{
   int rslt, tx, ty;

   rslt = map->geo_to_surface(lat, lon, &tx, &ty);
   *x = tx;
   *y = ty;

   return rslt;
}

void COvlkitUtil::get_geo_draw_line_points(MapProj* map,
                         degrees_t lat1,
                         degrees_t lon1,
                         degrees_t lat2,
                         degrees_t lon2,
                         POINT *pt,
                         int *num_pts)
{
      d_geo_t ll, ur;
   int x1, y1;          /* device coordinates of western point */
   int x2, y2;          /* device coordinates of eastern point */
   int p1_flags;        /* NSEW bounds check flags for point 1 */
   int p2_flags;        /* NSEW bounds check flags for point 2 */
   boolean_t east_of;   /* point 2 is east of point 1 */
   int pixels_around_world;
   int screen_width, screen_height;
   int num;
   BOOL swap;
   num = 0;
   CFvwUtil *futil = CFvwUtil::get_instance();

   // init the return value
   *num_pts = 0;

   // check for screen intersection
   map->get_vmap_bounds(&ll, &ur);
   map->get_pixels_around_world(&pixels_around_world);
   map->get_vsurface_size(&screen_width, &screen_height);

   // treat world overview as a special case
   if (screen_width == pixels_around_world)
   {
      // get device coordinates
      if (GEO_east_of_degrees(lon2, lon1))
      {
         futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
         futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
//         geo_to_device(map, lat1, lon1, &x1, &y1);
//         geo_to_device(map, lat2, lon2, &x2, &y2);
         swap = FALSE;
      }
      else         /* p2 is west of p1 or lon1=lon2 */
      {
         futil->geo_to_view_short(map, lat2, lon2, &x1, &y1);
         futil->geo_to_view_short(map, lat1, lon1, &x2, &y2);
//         geo_to_device(map, lat2, lon2, &x1, &y1);
//         geo_to_device(map, lat1, lon1, &x2, &y2);
         swap = TRUE;
      }

      if (x1 > x2)
      {
         // wrap around world
         if (swap)
         {
            pt[num].x = x2 + pixels_around_world;
            pt[num].y = y2;
            num++;
            pt[num].x = x1;
            pt[num].y = y1;
            num++;
            pt[num].x = x2;
            pt[num].y = y2;
            num++;
            pt[num].x = x1 - pixels_around_world;
            pt[num].y = y1;
            num++;
         }
         else
         {
            pt[num].x = x1;
            pt[num].y = y1;
            num++;
            pt[num].x = x2 + pixels_around_world;
            pt[num].y = y2;
            num++;
            pt[num].x = x1 - pixels_around_world;
            pt[num].y = y1;
            num++;
            pt[num].x = x2;
            pt[num].y = y2;
            num++;
         }
      }
      else
      {
         if (swap)
         {
            pt[num].x = x2;
            pt[num].y = y2;
            num++;
            pt[num].x = x1;
            pt[num].y = y1;
            num++;
         }
         else
         {
            pt[num].x = x1;
            pt[num].y = y1;
            num++;
            pt[num].x = x2;
            pt[num].y = y2;
            num++;
         }
      }
      *num_pts = num;
      return;
   }

   // get NSEW bounds check flags for each point
   p1_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
      ur.lat, ur.lon, lat1, lon1);
   p2_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
      ur.lat, ur.lon, lat2, lon2);

   // quick accept - both points on map
   if (((p1_flags | p2_flags) == 0) || (p1_flags & p2_flags))
   {
      // get device coordinates
      geo_to_surface(map, lat1, lon1, &x1, &y1);
      geo_to_surface(map, lat2, lon2, &x2, &y2);

      pt[num].x = x1;
      pt[num].y = y1;
      num++;
      pt[num].x = x2;
      pt[num].y = y2;
      num++;
      *num_pts = num;
      return;
   }

   // quick reject
   if (p1_flags & p2_flags)
      return;

   // reject lines that go from west of map to east of map but
   // not through the region defined by left_lon and right_lon
   if (((p1_flags ^ p2_flags) & (GEO_EAST_OF | GEO_WEST_OF)) ==
      (GEO_EAST_OF | GEO_WEST_OF))
   {
      if (p1_flags & GEO_EAST_OF)
      {
         if (GEO_east_of_degrees(lon2, lon1))
            return;

         // needed to get device coordinates in right order
         east_of = FALSE;
      }
      else
      {
         if (GEO_east_of_degrees(lon1, lon2))
            return;

         // needed to get device coordinates in right order
         east_of = TRUE;
      }
   }
   else
      east_of = GEO_east_of_degrees(lon2, lon1);

   // get device coordinates
   if (east_of)
   {
      futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
      futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
//      geo_to_device(map, lat1, lon1, &x1, &y1);
//      geo_to_device(map, lat2, lon2, &x2, &y2);
      swap = FALSE;
   }
   else
   {
      futil->geo_to_view_short(map, lat2, lon2, &x1, &y1);
      futil->geo_to_view_short(map, lat1, lon1, &x2, &y2);
//      geo_to_device(map, lat2, lon2, &x1, &y1);
//      geo_to_device(map, lat1, lon1, &x2, &y2);
      swap = TRUE;
   }

   if (x1 > x2)
   {
      if (swap)
      {
         pt[num].x = x2 + pixels_around_world;
         pt[num].y = y2;
         num++;
         pt[num].x = x1;
         pt[num].y = y1;
         num++;
      }
      else
      {
         pt[num].x = x1;
         pt[num].y = y1;
         num++;
         pt[num].x = x2 + pixels_around_world;
         pt[num].y = y2;
         num++;
      }
   }
   else
   {
      if (swap)
      {
         pt[num].x = x2;
         pt[num].y = y2;
         num++;
         pt[num].x = x1;
         pt[num].y = y1;
         num++;
      }
      else
      {
         pt[num].x = x1;
         pt[num].y = y1;
         num++;
         pt[num].x = x2;
         pt[num].y = y2;
         num++;
      }
   }
   *num_pts = num;
}
// end of get_geo_draw_line_points

// *************************************************************
// ********************************************************************

// optimized for 80 segments

void COvlkitUtil::draw_geo_ellipse(MapProj* map, CDC* dc, double lat, double lon,
                                    double vert, double horz, double angle,
                                    int line_color, int fill_color, int width,
                                    int line_style, int fill_style, BOOL xor,
                                    int bd_pt_cnt, POINT* bd_pt,
                                    CRect * bounds)
{
   draw_geo_ellipse(map, dc, lat, lon, vert, horz,  angle,  line_color, fill_color,
                     width, line_style, fill_style, xor,  TRUE,
                     bd_pt_cnt, bd_pt, bounds);

}
// end of draw_geo_ellipse

// ********************************************************************
// ********************************************************************

// optimized for 80 segments
// this version makes the background line optional

void COvlkitUtil::draw_geo_ellipse(MapProj* map, CDC* dc, double lat, double lon,
                                    double vert, double horz, double angle,
                                    int line_color, int fill_color, int width,
                                    int line_style, int fill_style, BOOL xor,
                                    BOOL background, int bd_pt_cnt, POINT* bd_pt,
                                    CRect * bounds)
{
   COLORREF color_rgb, fill_color_rgb;
   CFvwUtil *futil = CFvwUtil::get_instance();

   color_rgb = futil->code2color(line_color);
   fill_color_rgb = futil->code2color(fill_color);

   draw_geo_ellipse_rgb(map, dc, lat, lon, vert, horz, angle, color_rgb, fill_color_rgb, width, line_style, fill_style,
                     xor, background, bd_pt_cnt, bd_pt, bounds);
}

// ********************************************************************
// ********************************************************************

// optimized for 80 segments
// this version makes the background line optional

void COvlkitUtil::draw_geo_ellipse_rgb(MapProj* map, CDC* dc, double lat, double lon,
                                    double vert, double horz, double angle,
                                    COLORREF line_color, COLORREF fill_color, int width,
                                    int line_style, int fill_style, BOOL xor,
                                    BOOL background, int bd_pt_cnt, POINT* bd_pt,
                                    CRect * bounds)
{
   int k;
   CPen whitepen;
   CPen *oldpen;
   int x, y, tx, ty;
   double rang,tang, radius, ang, tmpang;
   double maxrad,minrad;
   double maxRadSq,minRadSq,RadSq;
   double sinang, cosang;
   double ainc, nlat, nlon;
   double dtmp;
   int olddrawmode;
   int pixels;
   int style;
   int screen_width, screen_height;
   BOOL bigscr, fill;
   POINT *pt;
   POINT ll, ur;
   degrees_t *plat, *plon;
   COvlkitUtil util;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (bd_pt_cnt < 4)
      return;
   if (bd_pt_cnt > 500)
      return;

   if ((vert < 0.0) || (horz < 0.0))
      return;

   if (vert < 0.001)
      vert = 0.001;

   if (horz < 0.001)
      horz = 0.001;

   style = line_style;
   if (xor)
      style = UTIL_LINE_XOR;

   map->get_pixels_around_world(&pixels);
   map->get_vsurface_size(&screen_width, &screen_height);
   if (((double) pixels / (double) screen_width) < 2.0)
      bigscr = TRUE;
   else
      bigscr = FALSE;

   // find the center x,y
   futil->geo_to_view_short(map, lat, lon, &x, &y);

   maxrad = vert;
   minrad = horz;
   maxRadSq = maxrad * maxrad;
   minRadSq = minrad * minrad;
   RadSq = maxRadSq * minRadSq;

   // prevent division by zero
   if ((maxrad == 0) && (minrad == 0))
      return;

   plat = (degrees_t*) MEM_malloc((bd_pt_cnt+1) * sizeof(degrees_t));
   if (plat == NULL)
   {
      ERR_report("Unable to allocate memory for draw_geo_ellipse");
      return;
   }

   plon = (degrees_t*) MEM_malloc((bd_pt_cnt+1) * sizeof(degrees_t));
   if (plon == NULL)
   {
      MEM_free(plat);
      ERR_report("Unable to allocate memory for draw_geo_ellipse");
      return;
   }
   pt = (POINT*) MEM_malloc((bd_pt_cnt+1) * sizeof(POINT));
   if (pt == NULL)
   {
      MEM_free(plat);
      MEM_free(plon);
      ERR_report("Unable to allocate memory for draw_geo_ellipse");
      return;
   }

   ainc = 360.0/ (double) bd_pt_cnt;
   ang = 0.0;

   for (k=0; k<bd_pt_cnt; k++)
   {
      tang = ang;
      tang += angle;
      if (tang > 360.0)
         tang -= 360.0;
      rang = DEG_TO_RAD(ang);
      sinang = sin(rang);
      cosang = cos(rang);
      dtmp = (minRadSq*cosang*cosang)+(maxRadSq*sinang* sinang);
      if ((dtmp > 0.0) && (RadSq > 0.0))
         radius = sqrt(RadSq / dtmp);
      else
      {
         if (vert == 0.0)
            radius = horz;
         else
            radius = vert;
         tmpang = angle + 180;
         if (tmpang > 360.0)
            tmpang -= 360.0;
         if ((tang != angle) && (tang != tmpang))
            radius = 0;
      }
      GEO_distance_to_geo(lat, lon, radius*1000.0 , tang, &nlat, &nlon);
      plat[k] = nlat;
      plon[k] = nlon;
      if (bd_pt_cnt == 80)
         ang = angle_inc[k];
      else
         ang = ang + ainc;
      geo_to_surface(map, nlat, nlon, &tx, &ty);
      pt[k].x = tx;
      pt[k].y = ty;
   }


   // assign the points
   ll.x = pt[0].x;
   ll.y = pt[0].y;
   ur.x = pt[0].x;
   ur.y = pt[0].y;
   for (k=0; k<bd_pt_cnt; k++)
   {
      bd_pt[k] = pt[k];
      if (pt[k].x < ll.x)
         ll.x = pt[k].x;
      if (pt[k].x > ur.x)
         ur.x = pt[k].x;
      if (pt[k].y < ll.y)
         ll.y = pt[k].y;
      if (pt[k].y > ur.y)
         ur.y = pt[k].y;
   }
   bounds->SetRect(ll.x, ll.y, ur.x, ur.y);

   int wid;
   COLORREF color;

   color = line_color;
   if (xor)
      color = -1;

   wid = width;
   if (xor)
      wid = 2;

   fill = (fill_style != UTIL_FILL_NONE) && !bigscr;
   if (get_oval_width(map, lat, lon, vert, horz, angle) > 100.0)
      fill = FALSE;

   // draw the fill
   if (!xor && fill)
      futil->fill_polygon(dc, pt, bd_pt_cnt, fill_color, fill_style, screen_width, screen_height);

   if (wid > 0)
   {
      if (background && !xor)
      {
         for (k=0; k<bd_pt_cnt-1; k++)
            futil->draw_geo_line(map, dc, plat[k], plon[k], plat[k+1], plon[k+1],
                           UTIL_COLOR_BLACK, PS_SOLID, wid+2,
                           UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
         futil->draw_geo_line(map, dc, plat[bd_pt_cnt-1], plon[bd_pt_cnt-1], plat[0], plon[0],
                        UTIL_COLOR_BLACK, PS_SOLID, wid+2,
                        UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
      }
      for (k=0; k<bd_pt_cnt-1; k++)
         futil->draw_geo_line(map, dc, plat[k], plon[k], plat[k+1], plon[k+1],
                        color, style, wid, UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
      futil->draw_geo_line(map, dc, plat[bd_pt_cnt-1], plon[bd_pt_cnt-1], plat[0], plon[0],
                     color, style, wid, UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
   }

   if (xor)
   {
      // draw the center cross
      olddrawmode = dc->SetROP2(R2_XORPEN);
      whitepen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
      oldpen = (CPen*) dc->SelectObject(&whitepen);
      futil->move_to(dc, x - 9, y);
      futil->line_to(dc, x + 10, y);
      futil->move_to(dc, x,     y - 9);
      futil->line_to(dc, x,     y + 10);
      dc->SetROP2(olddrawmode);
      dc->SelectObject(oldpen);
      whitepen.DeleteObject();
   }
   // clean up
   MEM_free(pt);
   MEM_free(plat);
   MEM_free(plon);
}
// end of draw_geo_ellipse_rgb

// ********************************************************************
// ********************************************************************

int COvlkitUtil::geo_to_surface(MapProj* map, double lat, double lon, int *x, int *y)
{
   int tx, ty, px, mx, pixels;
   int screen_width, screen_height;

   map->get_pixels_around_world(&pixels);
   map->get_vsurface_size(&screen_width, &screen_height);
   mx = screen_width / 2;

   map->geo_to_surface(lat, lon, &tx, &ty);
   if (tx < 0)
      px = tx + pixels;
   else
      px = tx - pixels;

   if (abs(px - mx) < abs(tx - mx))
      tx = px;

   *x = tx;
   *y = ty;

   return SUCCESS;
}

// ********************************************************************
// ********************************************************************

// draw an ellipse based on pixels only
void COvlkitUtil::draw_simple_ellipse(MapProj* map, CDC* dc, double lat, double lon,
                                    double vert, double horz,  // in pixels
                                    double angle,
                                    int line_color, int fill_color, int width,
                                    int line_style, int fill_style, BOOL xor,
                                    BOOL background, int bd_pt_cnt, POINT* bd_pt,
                                    CRect * bounds)
{
   int k;
   CBrush brush;
   CPen pen, whitepen;
   CPen *oldpen;
   COLORREF color, fcolor;
   int x, y;
   double rang, tang, ang;
   double sinang, cosang;
   double ainc;
   int olddrawmode;
   BOOL fill;
   POINT *pt;
   POINT ll, ur;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (bd_pt_cnt < 4)
      bd_pt_cnt = 4;
   if (bd_pt_cnt > 500)
      bd_pt_cnt = 500;

   if (vert < 1.0)
      vert = 1.0;

   if (horz < 1.0)
      horz = 1.0;

   // find the center x,y
   futil->geo_to_view_short(map, lat, lon, &x, &y);

   pt = (POINT*) MEM_malloc((bd_pt_cnt+1) * sizeof(POINT));
   if (pt == NULL)
   {
      ERR_report("Unable to allocate memory for draw_simple_ellipse");
      return;
   }

   ainc = 360.0/ (double) bd_pt_cnt;
   ang = 0.0;

   for (k=0; k<bd_pt_cnt; k++)
   {
      tang = ang;
      tang += angle;
      if (tang > 360.0)
         tang -= 360.0;
      rang = DEG_TO_RAD(ang);
      sinang = sin(rang);
      cosang = cos(rang);

      pt[k].x = x + (int) ((cosang * horz) + 0.5);
      pt[k].y = y + (int) ((sinang * vert) + 0.5);
      ang = ang + ainc;
   }

   // assign the points
   ll.x = pt[0].x;
   ll.y = pt[0].y;
   ur.x = pt[0].x;
   ur.y = pt[0].y;
   for (k=0; k<bd_pt_cnt; k++)
   {
      bd_pt[k] = pt[k];
      if (pt[k].x < ll.x)
         ll.x = pt[k].x;
      if (pt[k].x > ur.x)
         ur.x = pt[k].x;
      if (pt[k].y < ll.y)
         ll.y = pt[k].y;
      if (pt[k].y > ur.y)
         ur.y = pt[k].y;
   }
   bounds->SetRect(ll.x, ll.y, ur.x, ur.y);

   color = futil->code2color(line_color);
   fcolor = futil->code2color(fill_color);
   brush.CreateSolidBrush(fcolor);
   pen.CreatePen(PS_SOLID, width, color);
   CBrush* oldbrush = dc->SelectObject(&brush);
   oldpen = dc->SelectObject(&pen);
   CBrush nullbrush;
   nullbrush.CreateStockObject(NULL_BRUSH);
   dc->SelectObject(&nullbrush);
   brush.DeleteObject();
   fill = TRUE;
   switch (fill_style)
   {
      case UTIL_FILL_HORZ:
      case UTIL_FILL_VERT:
      case UTIL_FILL_BDIAG:
      case UTIL_FILL_FDIAG:
      case UTIL_FILL_CROSS:
      case UTIL_FILL_DIAGCROSS:
         brush.CreateHatchBrush(futil->code2fill(fill_style), fcolor);
         break;
      case UTIL_FILL_SOLID:
         brush.CreateSolidBrush(fcolor);
         break;
      default:
         fill = FALSE;
   }
   if (fill)
      dc->SelectObject(&brush);

   int oldbkmode = dc->SetBkMode(TRANSPARENT);

   if (xor)
   {
      olddrawmode = dc->SetROP2(R2_XORPEN);
      whitepen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
      dc->SelectObject(&whitepen);
      dc->SelectObject(&nullbrush);
   }


   if (fill)
      futil->polygon(dc, pt, bd_pt_cnt);
   else
      futil->empty_polygon(dc, pt, bd_pt_cnt);


   if (!xor && background)
   {
      CPen blackpen;
      blackpen.CreatePen(PS_SOLID, width+2, RGB(0,0,0));
      dc->SelectObject(&nullbrush);
      if (fill_style != UTIL_FILL_SOLID)
      {
         dc->SelectObject(&blackpen);
         futil->empty_polygon(dc, pt, bd_pt_cnt);
      }
      dc->SelectObject(&pen);
      futil->empty_polygon(dc, pt, bd_pt_cnt);
      blackpen.DeleteObject();
   }

   if (xor)
   {
      // draw the center cross
      futil->move_to(dc, x - 9, y);
      futil->line_to(dc, x + 10, y);
      futil->move_to(dc, x,     y - 9);
      futil->line_to(dc, x,     y + 10);
      dc->SetROP2(olddrawmode);
      dc->SelectObject(oldpen);
      whitepen.DeleteObject();
   }

   // clean up
   dc->SelectObject(oldpen);
   dc->SelectObject(oldbrush);
   dc->SetBkMode(oldbkmode);
   pen.DeleteObject();
   brush.DeleteObject();
   nullbrush.DeleteObject();
   MEM_free(pt);
}
// end of draw_simple_ellipse

// ********************************************************************
// ********************************************************************

void COvlkitUtil::draw_geo_ellipse_edged(MapProj* map, CDC* dc, double lat, double lon,
                                    double vert, double horz, double angle,
                                    int line_color, int width,
                                    int bd_pt_cnt, POINT* bd_pt,
                                    CRect * bounds)
{
   int k;
   CPen pen;
   COLORREF color;
   CFvwUtil *futil = CFvwUtil::get_instance();
   int x, y, x1, y1, x2, y2;
   int max_width;
   double rang,tang, radius, ang;
   double maxrad,minrad;
   double maxRadSq,minRadSq,RadSq;
   double sinang, cosang;
   double ainc, nlat, nlon;
   double dtmp;

   if (bd_pt_cnt < 4)
      return;
   if (bd_pt_cnt > 500)
      return;

   // find the center x,y
   futil->geo_to_view_short(map, lat, lon, &x, &y);

   // get bounds
   bounds->SetRect(x, y, x, y);

   maxrad = vert;
   minrad = horz;
   maxRadSq = maxrad * maxrad;
   minRadSq = minrad * minrad;
   RadSq = maxRadSq * minRadSq;

   // prevent division by zero
   if ((maxrad == 0) && (minrad == 0))
      return;

   ainc = 360.0/ (double) bd_pt_cnt;
   ang = 0.0;

   for (k=0; k<bd_pt_cnt; k++)
   {
      tang = ang;
      tang += angle;
      if (tang > 360.0)
         tang -= 360.0;
      rang = DEG_TO_RAD(ang);
      sinang = sin(rang);
      cosang = cos(rang);
      dtmp = (minRadSq*cosang*cosang)+(maxRadSq*sinang* sinang);
       // prevent division by zero
       if (dtmp == 0.0)
          return;
      radius = sqrt(RadSq / dtmp);
      GEO_distance_to_geo(lat, lon, radius*1000.0 , tang, &nlat, &nlon);
      futil->geo_to_view_short(map, nlat, nlon, &x2, &y2);

      bd_pt[k].x = x2;
      bd_pt[k].y = y2;
      if (bounds->top > bd_pt[k].y)
         bounds->top = bd_pt[k].y;
      if (bounds->bottom < bd_pt[k].y)
         bounds->bottom = bd_pt[k].y;
      if (bounds->left > bd_pt[k].x)
         bounds->left = bd_pt[k].x;
      if (bounds->right < bd_pt[k].x)
         bounds->right = bd_pt[k].x;
       ang = ang + ainc;
   }

   max_width = bounds->right - bounds->left;

   color = futil->code2color(line_color);
   pen.CreatePen(PS_SOLID, width, color);
   CPen* oldpen = dc->SelectObject(&pen);

   if (max_width < 50)
   {
      for (k=0; k<bd_pt_cnt; k++)
      {
         if (k >= bd_pt_cnt - 1)
         {
            x1 = bd_pt[bd_pt_cnt - 1].x;
            y1 = bd_pt[bd_pt_cnt - 1].y;
            x2 = bd_pt[0].x;
            y2 = bd_pt[0].y;
         }
         else
         {
            x1 = bd_pt[k].x;
            y1 = bd_pt[k].y;
            x2 = bd_pt[k+1].x;
            y2 = bd_pt[k+1].y;
         }
         futil->move_to(dc, x1, y1);
         futil->line_to(dc, x2, y2);
      }
      dc->SelectObject(oldpen);
      pen.DeleteObject();
      return;
   }

   // process non-standard styles
   CPoint move_to[500], line_to[500];
   int cnt, midx, len, j;
   double ang2;

   for (k=0; k<bd_pt_cnt; k++)
   {
      if (k >= bd_pt_cnt - 1)
      {
         x1 = bd_pt[bd_pt_cnt - 1].x;
         y1 = bd_pt[bd_pt_cnt - 1].y;
         x2 = bd_pt[0].x;
         y2 = bd_pt[0].y;
      }
      else
      {
         x1 = bd_pt[k].x;
         y1 = bd_pt[k].y;
         x2 = bd_pt[k+1].x;
         y2 = bd_pt[k+1].y;
      }
      // compute the "flat" version
      len = futil->magnitude(x1, y1, x2, y2);
      ang2 = futil->line_angle(x1,y1, x2, y2);
      cnt = 0;
      // draw the line
      move_to[cnt].x = x1;
      move_to[cnt].y = y1;
      line_to[cnt].x = x1 + len;
      line_to[cnt].y = y1;
      cnt++;
      if ((len > 2) && (max_width > 50))
      {
         // draw the crossties
         midx = x1 + (len / 2);
         move_to[cnt].x = midx;
         move_to[cnt].y = y1;
         line_to[cnt].x = midx;
         line_to[cnt].y = y1 + 6;
         cnt++;
      }

      // rotate and draw the fancy line
      int tx1, ty1, tx2, ty2;

      for (j=0; j<cnt; j++)
      {
         futil->rotate_pt(move_to[j].x, move_to[j].y, &tx1, &ty1, ang2, x1, y1);
         futil->move_to(dc, tx1, ty1);
         futil->rotate_pt(line_to[j].x, line_to[j].y, &tx2, &ty2, ang2, x1, y1);
         futil->line_to(dc, tx2, ty2);
      }
   }

   dc->SelectObject(oldpen);
   pen.DeleteObject();
}
// end of draw_geo_ellipse_edged

// ********************************************************************
// ********************************************************************

// draw an ellipse based on pixels only
void COvlkitUtil::draw_simple_ellipse_rgb(MapProj* map, CDC* dc, double lat, double lon,
                                    double vert, double horz,  // in pixels
                                    double angle,
                                    COLORREF line_color, COLORREF fill_color, int width,
                                    int line_style, int fill_style, BOOL xor,
                                    BOOL background, int bd_pt_cnt, POINT* bd_pt,
                                    CRect * bounds)
{
   int k;
   CBrush brush;
   CPen pen, whitepen;
   CPen *oldpen;
   int x, y;
   double rang, tang, ang;
   double sinang, cosang;
   double ainc;
   int olddrawmode;
   BOOL fill;
   POINT *pt;
   POINT ll, ur;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (bd_pt_cnt < 4)
      bd_pt_cnt = 4;
   if (bd_pt_cnt > 500)
      bd_pt_cnt = 500;

   if (vert < 1.0)
      vert = 1.0;

   if (horz < 1.0)
      horz = 1.0;

   // find the center x,y
   futil->geo_to_view_short(map, lat, lon, &x, &y);

   pt = (POINT*) MEM_malloc((bd_pt_cnt+1) * sizeof(POINT));
   if (pt == NULL)
   {
      ERR_report("Unable to allocate memory for draw_simple_ellipse");
      return;
   }

   ainc = 360.0/ (double) bd_pt_cnt;
   ang = 0.0;

   for (k=0; k<bd_pt_cnt; k++)
   {
      tang = ang;
      tang += angle;
      if (tang > 360.0)
         tang -= 360.0;
      rang = DEG_TO_RAD(ang);
      sinang = sin(rang);
      cosang = cos(rang);

      pt[k].x = x + (int) ((cosang * horz) + 0.5);
      pt[k].y = y + (int) ((sinang * vert) + 0.5);
      ang = ang + ainc;
   }

   // assign the points
   ll.x = pt[0].x;
   ll.y = pt[0].y;
   ur.x = pt[0].x;
   ur.y = pt[0].y;
   for (k=0; k<bd_pt_cnt; k++)
   {
      bd_pt[k] = pt[k];
      if (pt[k].x < ll.x)
         ll.x = pt[k].x;
      if (pt[k].x > ur.x)
         ur.x = pt[k].x;
      if (pt[k].y < ll.y)
         ll.y = pt[k].y;
      if (pt[k].y > ur.y)
         ur.y = pt[k].y;
   }
   bounds->SetRect(ll.x, ll.y, ur.x, ur.y);

   brush.CreateSolidBrush(fill_color);
   pen.CreatePen(PS_SOLID, width, line_color);
   CBrush* oldbrush = dc->SelectObject(&brush);
   oldpen = dc->SelectObject(&pen);
   CBrush nullbrush;
   nullbrush.CreateStockObject(NULL_BRUSH);
   dc->SelectObject(&nullbrush);
   brush.DeleteObject();
   fill = TRUE;
   switch (fill_style)
   {
      case UTIL_FILL_HORZ:
      case UTIL_FILL_VERT:
      case UTIL_FILL_BDIAG:
      case UTIL_FILL_FDIAG:
      case UTIL_FILL_CROSS:
      case UTIL_FILL_DIAGCROSS:
         brush.CreateHatchBrush(futil->code2fill(fill_style), fill_color);
         break;
      case UTIL_FILL_SOLID:
         brush.CreateSolidBrush(fill_color);
         break;
      default:
         fill = FALSE;
   }
   if (fill)
      dc->SelectObject(&brush);

   int oldbkmode = dc->SetBkMode(TRANSPARENT);

   if (xor)
   {
      olddrawmode = dc->SetROP2(R2_XORPEN);
      whitepen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
      dc->SelectObject(&whitepen);
      dc->SelectObject(&nullbrush);
   }


   if (fill)
      futil->polygon(dc, pt, bd_pt_cnt);
   else
      futil->empty_polygon(dc, pt, bd_pt_cnt);


   if (!xor && background)
   {
      CPen blackpen;
      blackpen.CreatePen(PS_SOLID, width+2, RGB(0,0,0));
      dc->SelectObject(&nullbrush);
      if (fill_style != UTIL_FILL_SOLID)
      {
         dc->SelectObject(&blackpen);
         futil->empty_polygon(dc, pt, bd_pt_cnt);
      }
      dc->SelectObject(&pen);
      futil->empty_polygon(dc, pt, bd_pt_cnt);
      blackpen.DeleteObject();
   }

   if (xor)
   {
      // draw the center cross
      futil->move_to(dc, x - 9, y);
      futil->line_to(dc, x + 10, y);
      futil->move_to(dc, x,     y - 9);
      futil->line_to(dc, x,     y + 10);
      dc->SetROP2(olddrawmode);
      dc->SelectObject(oldpen);
      whitepen.DeleteObject();
   }

   // clean up
   dc->SelectObject(oldpen);
   dc->SelectObject(oldbrush);
   dc->SetBkMode(oldbkmode);
   pen.DeleteObject();
   brush.DeleteObject();
   nullbrush.DeleteObject();
   MEM_free(pt);
}
// end of draw_simple_ellipse_rgb

// ********************************************************************
// ********************************************************************

void COvlkitUtil::draw_geo_ellipse_edged_rgb(MapProj* map, CDC* dc, double lat, double lon,
                                    double vert, double horz, double angle,
                                    COLORREF color, int width,
                                    int bd_pt_cnt, POINT* bd_pt,
                                    CRect * bounds)
{
   int k;
   CPen pen;
   CFvwUtil *futil = CFvwUtil::get_instance();
   int x, y, x1, y1, x2, y2;
   int max_width;
   double rang,tang, radius, ang;
   double maxrad,minrad;
   double maxRadSq,minRadSq,RadSq;
   double sinang, cosang;
   double ainc, nlat, nlon;
   double dtmp;

   if (bd_pt_cnt < 4)
      return;
   if (bd_pt_cnt > 500)
      return;

   // find the center x,y
   futil->geo_to_view_short(map, lat, lon, &x, &y);

   // get bounds
   bounds->SetRect(x, y, x, y);

   maxrad = vert;
   minrad = horz;
   maxRadSq = maxrad * maxrad;
   minRadSq = minrad * minrad;
   RadSq = maxRadSq * minRadSq;

   // prevent division by zero
   if ((maxrad == 0) && (minrad == 0))
      return;

   ainc = 360.0/ (double) bd_pt_cnt;
   ang = 0.0;

   for (k=0; k<bd_pt_cnt; k++)
   {
      tang = ang;
      tang += angle;
      if (tang > 360.0)
         tang -= 360.0;
      rang = DEG_TO_RAD(ang);
      sinang = sin(rang);
      cosang = cos(rang);
      dtmp = (minRadSq*cosang*cosang)+(maxRadSq*sinang* sinang);
       // prevent division by zero
       if (dtmp == 0.0)
          return;
      radius = sqrt(RadSq / dtmp);
      GEO_distance_to_geo(lat, lon, radius*1000.0 , tang, &nlat, &nlon);
      futil->geo_to_view_short(map, nlat, nlon, &x2, &y2);

      bd_pt[k].x = x2;
      bd_pt[k].y = y2;
      if (bounds->top > bd_pt[k].y)
         bounds->top = bd_pt[k].y;
      if (bounds->bottom < bd_pt[k].y)
         bounds->bottom = bd_pt[k].y;
      if (bounds->left > bd_pt[k].x)
         bounds->left = bd_pt[k].x;
      if (bounds->right < bd_pt[k].x)
         bounds->right = bd_pt[k].x;
       ang = ang + ainc;
   }

   max_width = bounds->right - bounds->left;

   pen.CreatePen(PS_SOLID, width, color);
   CPen* oldpen = dc->SelectObject(&pen);

   if (max_width < 50)
   {
      for (k=0; k<bd_pt_cnt; k++)
      {
         if (k >= bd_pt_cnt - 1)
         {
            x1 = bd_pt[bd_pt_cnt - 1].x;
            y1 = bd_pt[bd_pt_cnt - 1].y;
            x2 = bd_pt[0].x;
            y2 = bd_pt[0].y;
         }
         else
         {
            x1 = bd_pt[k].x;
            y1 = bd_pt[k].y;
            x2 = bd_pt[k+1].x;
            y2 = bd_pt[k+1].y;
         }
         futil->move_to(dc, x1, y1);
         futil->line_to(dc, x2, y2);
      }
      dc->SelectObject(oldpen);
      pen.DeleteObject();
      return;
   }

   // process non-standard styles
   CPoint move_to[500], line_to[500];
   int cnt, midx, len, j;
   double ang2;

   for (k=0; k<bd_pt_cnt; k++)
   {
      if (k >= bd_pt_cnt - 1)
      {
         x1 = bd_pt[bd_pt_cnt - 1].x;
         y1 = bd_pt[bd_pt_cnt - 1].y;
         x2 = bd_pt[0].x;
         y2 = bd_pt[0].y;
      }
      else
      {
         x1 = bd_pt[k].x;
         y1 = bd_pt[k].y;
         x2 = bd_pt[k+1].x;
         y2 = bd_pt[k+1].y;
      }
      // compute the "flat" version
      len = futil->magnitude(x1, y1, x2, y2);
      ang2 = futil->line_angle(x1,y1, x2, y2);
      cnt = 0;
      // draw the line
      move_to[cnt].x = x1;
      move_to[cnt].y = y1;
      line_to[cnt].x = x1 + len;
      line_to[cnt].y = y1;
      cnt++;
      if ((len > 2) && (max_width > 50))
      {
         // draw the crossties
         midx = x1 + (len / 2);
         move_to[cnt].x = midx;
         move_to[cnt].y = y1;
         line_to[cnt].x = midx;
         line_to[cnt].y = y1 + 6;
         cnt++;
      }

      // rotate and draw the fancy line
      int tx1, ty1, tx2, ty2;

      for (j=0; j<cnt; j++)
      {
         futil->rotate_pt(move_to[j].x, move_to[j].y, &tx1, &ty1, ang2, x1, y1);
         futil->move_to(dc, tx1, ty1);
         futil->rotate_pt(line_to[j].x, line_to[j].y, &tx2, &ty2, ang2, x1, y1);
         futil->line_to(dc, tx2, ty2);
      }
   }

   dc->SelectObject(oldpen);
   pen.DeleteObject();
}
// end of draw_geo_ellipse_edged_rgb

// ********************************************************************
// ********************************************************************

void COvlkitUtil::get_geo_ellipse_bounds(MapProj* map, double lat, double lon,
                                       double vert, double horz,
                                       double angle, CRect * bounds,
                                       d_geo_t * ll, d_geo_t * ur)
{
   int k;
   int x, y, x2, y2;
   double rang,tang, radius, ang;
   double maxrad,minrad;
   double maxRadSq,minRadSq,RadSq;
   double sinang, cosang;
   double ainc, nlat, nlon;
   int bd_pt_cnt = 60;
   double ValOf_DEG_TO_RAD = 0.01745329251994;
   CFvwUtil *futil = CFvwUtil::get_instance();

   // find the center x,y
   futil->geo_to_view_short(map, lat, lon, &x, &y);
   bounds->SetRect(x, y, x, y);
   ll->lat = lat;
   ll->lon = lon;
   ur->lat = lat;
   ur->lon = lon;

   maxrad = vert;
   minrad = horz;
   maxRadSq = maxrad * maxrad;
   minRadSq = minrad * minrad;
   RadSq = maxRadSq * minRadSq;

   // prevent division by zero
   if ((maxrad == 0) && (minrad == 0))
      return;

   ainc = 360.0/ (double) bd_pt_cnt;
   ang = 0.0;

   for (k=0; k<bd_pt_cnt; k++)
   {
      tang = ang;
      tang += angle;
      if (tang > 360.0)
         tang -= 360.0;
       rang = DEG_TO_RAD(ang);
       sinang = sin(rang);
       cosang = cos(rang);
       if (RadSq == 0.0)
          radius = 0.0;
       else
          radius = sqrt(RadSq / ((minRadSq*cosang*cosang)+(maxRadSq*sinang* sinang)));
       GEO_distance_to_geo(lat, lon, radius*1000.0 , tang, &nlat, &nlon);
       futil->geo_to_view_short(map, nlat, nlon, &x2, &y2);
       ang = ang + ainc;
       if (ll->lat > nlat)
          ll->lat = nlat;
       if (ur->lat < nlat)
          ur->lat = nlat;
       if (GEO_east_of_degrees(ll->lon, nlon) && GEO_east_of_degrees(lon, nlon))
          ll->lon = nlon;
       if (GEO_east_of_degrees(nlon, ur->lon) && GEO_east_of_degrees(nlon, lon))
          ur->lon = nlon;
      if (bounds->top > y2)
         bounds->top = y2;
      if (bounds->bottom < y2)
         bounds->bottom = y2;
       if (bounds->left > x2)
         bounds->left = x2;
      if (bounds->right < x2)
         bounds->right = x2;
   }
}
// end of get_geo_ellipse_bounds

// *************************************************************
// *************************************************************

int COvlkitUtil::get_geo_polygon_bounds(int numverts, double *lat, double *lon,
                              d_geo_t * ll, d_geo_t * ur, double *lon_extent)
{
   int k;

   ll->lat = 0.0;
   ll->lon = 0.0;
   ur->lat = 0.0;
   ur->lon = 0.0;

   if (numverts < 3)
      return FAILURE;

   ll->lat = lat[0];
   ll->lon = lon[0];
   ur->lat = lat[0];
   ur->lon = lon[0];

   for (k=1; k<numverts; k++)
   {
       if (ll->lat > lat[k])
          ll->lat = lat[k];
       if (ur->lat < lat[k])
          ur->lat = lat[k];
       if (GEO_east_of_degrees(ll->lon, lon[k]))
          ll->lon = lon[k];
       if (GEO_east_of_degrees(lon[k], ur->lon))
          ur->lon = lon[k];
   }

   if (ll->lon <= 0.0)
      *lon_extent = ur->lon - ll->lon;
   else if ((ll->lon > 0.0) && (ur->lon > 0.0))
      *lon_extent = ur->lon - ll->lon;
   else
      *lon_extent = 360.0 + ur->lon - ll->lon;

   return SUCCESS;
}
// end of get_geo_polygon_bounds

// *************************************************************
// *************************************************************

// return the width of the polygon in degrees

double COvlkitUtil::get_oval_width(MapProj* map, double lat, double lon,
                           double vert, double horz, double angle)
{
   CRect rc;
   d_geo_t ll, ur;
   double east, west, width;

   get_geo_ellipse_bounds( map, lat, lon, vert, horz, angle, &rc, &ll, &ur);
   west = ll.lon;
   east = ur.lon;
   if (west <= east)
   {
      width = east - west;
      return width;
   }

   width = (180.0 - west) + (180.0 + east);
   return width;
}
// end of get_oval_width

// *************************************************************
// *************************************************************

void COvlkitUtil::DrawBasicButton(CDC * dc, RECT rc)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CPen darkpen, graypen, lightpen;
   CBrush graybrush;

   graybrush.CreateSolidBrush(futil->code2color(UTIL_COLOR_MONEY_GREEN));
   darkpen.CreatePen(PS_SOLID, 1, RGB (0, 0, 0));
   graypen.CreatePen(PS_SOLID, 1, futil->code2color(UTIL_COLOR_DARK_GRAY));
   lightpen.CreatePen(PS_SOLID, 1, RGB (255, 255, 255));
   CPen *oldpen = dc->SelectObject(&graypen);
   CBrush *oldbrush = dc->SelectObject(&graybrush);

   futil->rectangle(dc, &(rc));

   dc->SelectObject(&lightpen);

   futil->move_to(dc, rc.right-1, rc.top);
   futil->line_to(dc, rc.right-1, rc.bottom-1);
   futil->line_to(dc, rc.left, rc.bottom-1);

   dc->SelectObject(oldbrush);
   dc->SelectObject(oldpen);
   graybrush.DeleteObject();
   darkpen.DeleteObject();
   graypen.DeleteObject();
   lightpen.DeleteObject();
}
// end of DrawBasicButton

// *************************************************************
// ********************************************************************

BOOL COvlkitUtil::polygon_is_clockwise(degrees_t *plat, degrees_t *plon, int ptcnt)
{
   double ang1, ang2, angsum, angdif;
   int k;
   double x, y;
   double minlat, maxlat, minlon, maxlon, latdif, londif, range, epsilon;
   double latsum, lonsum, ctrlat, ctrlon, latinc, loninc;
   BOOL found, clockwise;
   CFvwUtil *futil = CFvwUtil::get_instance();

   // find the bounds of polygon
   minlat = maxlat = plat[0];
   minlon = maxlon = plon[0];
   for (k=1; k<ptcnt; k++)
   {
      if (plat[k] < minlat)
         minlat = plat[k];
      if (plat[k] > maxlat)
         maxlat = plat[k];
      if (plon[k] < minlon)
         minlon = plon[k];
      if (plon[k] > maxlon)
         maxlon = plon[k];
   }
   latdif = maxlat - minlat;
   londif = maxlon - minlon;

   // find center of mass
   latsum = lonsum = 0.0;
   for (k=0; k<ptcnt; k++)
   {
      latsum += plat[k];
      lonsum += plon[k];
   }
   ctrlat = latsum / (double) ptcnt;
   ctrlon = lonsum / (double) ptcnt;

   // test for inside polygon
   if (!point_in_polygon(ctrlat, ctrlon, plat, plon, ptcnt))
   {
      // find a point inside the polygon
      latinc = latdif / 100.0;
      loninc = londif / 100.0;
      found = FALSE;
      x = minlon;
      y = minlat;
      while ((y < maxlat) && !found)
      {
         x = minlon;
         while ((x < maxlon) && !found)
         {
            if (point_in_polygon(y, x, plat, plon, ptcnt))
            {
               ctrlat = y;
               ctrlon = x;
               found = TRUE;
               break;
            }
            x += loninc;
         }
         y += latinc;
      }
   }

   range = latdif;
   if (londif < range)
      range = londif;
   epsilon = range / 1000.0;

   angsum = 0.0;

   for (k=0; k<ptcnt-1; k++)
   {
      ang1 = futil->line_angle(ctrlat, ctrlon, plat[k], plon[k]);
      ang2 = futil->line_angle(ctrlat, ctrlon, plat[k+1], plon[k+1]);
      angdif = ang2 - ang1;
      if (angdif < -180.0)
         angdif += 360.0;
      if (angdif > 180.0)
         angdif -= 360.0;
      if (fabs(angdif) > 150.0)
         continue;
      angsum += angdif;
   }

   // get the last one
   ang1 = futil->line_angle(ctrlat, ctrlon, plat[ptcnt-1], plon[ptcnt-1]);
   ang2 = futil->line_angle(ctrlat, ctrlon, plat[0], plon[0]);
   angdif = ang2 - ang1;
   if (angdif < -180.0)
      angdif += 360.0;
   if (angdif > 180.0)
      angdif -= 360.0;
   angsum += angdif;

   if (angsum > 0.0)
      clockwise = TRUE;
   else
      clockwise = FALSE;

   // if the polygon crosses the IDL it will appear to be "inside-out" to reverse the finding
   if (londif > 180.0)
      clockwise = !clockwise;

   return clockwise;
}

// ********************************************************************
// ********************************************************************

BOOL COvlkitUtil::point_in_polygon(double testlat, double testlon, double *plat, double *plon, int numverts)
{
   int c, i, j;
   double x, y;
   c = 0;

   x = testlon;
   y = testlat;
   for (i = 0, j = numverts - 1; i < numverts; j = i++)
   {
      if ((((plat[i] <= y) && (y < plat[j])) ||
        ((plat[j] <= y) && (y < plat[i]))) &&
         (x < (plon[j] - plon[i]) * (y - plat[i]) /
         (plat[j] - plat[i]) + plon[i]))
         c = !c;
   }
   if (c == 0)
      return FALSE;
   else
      return TRUE;
}
// end of point_in_polygon

// ********************************************************************
// ********************************************************************

BOOL COvlkitUtil::point_in_geo_polygon(MapProj* map,
                           int numverts, // number points in polygon
                           int *line_type,   // array of types: simple=1, rhumb=2, great_circle=3
                           degrees_t *plat, degrees_t *plon, // polygon points
                           CPoint test_point)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   int rslt;
   POINT *pt = NULL;
   int numpt;
   BOOL hit;

   rslt = geo_polygon_to_view(map, numverts, line_type, plat, plon, &numpt, &pt);
   if (rslt != SUCCESS)
      return FALSE;

   hit = futil->point_in_polygon(test_point, pt, numpt);

   if (pt != NULL)
      free(pt);

   return hit;
}
// end of point_in_geo_polygon

// *************************************************************
// *************************************************************

BOOL COvlkitUtil::point_on_geo_polygon(MapProj* map,
                           int numverts, // number points in polygon
                           int *line_type,   // array of types: simple=1, rhumb=2, great_circle=3
                           degrees_t *plat, degrees_t *plon, // polygon points
                           CPoint test_point)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   int rslt;
   POINT *pt = NULL;
   int numpt, k, tx, ty, ndist;
   BOOL hit, notdone;

   rslt = geo_polygon_to_view(map, numverts, line_type, plat, plon, &numpt, &pt);
   if (rslt != SUCCESS)
      return FALSE;

   notdone = TRUE;
   hit = FALSE;
   tx = pt[0].x;
   ty = pt[0].y;
   k = 1;
   while (notdone && !hit)
   {
      ndist = futil->distance_to_line(tx, ty, pt[k].x, pt[k].y, test_point.x, test_point.y);
      if (ndist < 7)
         hit = TRUE;
      tx = pt[k].x;
      ty = pt[k].y;
      k++;
      if (k >= numpt)
         notdone = FALSE;
   }

   if (pt != NULL)
      free(pt);

   return hit;
}
// end of point_on_geo_polygon

// *************************************************************
// *************************************************************

// tests if point in both within the polygon and near an edge

BOOL COvlkitUtil::point_in_and_on_geo_polygon(MapProj* map,
                           int numverts, // number points in polygon
                           int *line_type,   // array of types: simple=1, rhumb=2, great_circle=3
                           degrees_t *plat, degrees_t *plon, // polygon points
                           CPoint test_point)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   int rslt;
   POINT *pt = NULL;
   int numpt, k, tx, ty, ndist;
   BOOL hit, notdone;

   rslt = geo_polygon_to_view(map, numverts, line_type, plat, plon, &numpt, &pt);
   if (rslt != SUCCESS)
      return FALSE;

   notdone = TRUE;
   hit = FALSE;
   tx = pt[0].x;
   ty = pt[0].y;
   k = 1;
   while (notdone && !hit)
   {
      ndist = futil->distance_to_line(tx, ty, pt[k].x, pt[k].y, test_point.x, test_point.y);
      if (ndist < 7)
         hit = TRUE;
      tx = pt[k].x;
      ty = pt[k].y;
      k++;
      if (k >= numpt)
         notdone = FALSE;
   }

   if (hit)
      hit = futil->point_in_polygon(test_point, pt, numpt);

   if (pt != NULL)
      free(pt);

   return hit;
}
// end of point_in_and_on_geo_polygon

// *************************************************************
// *************************************************************

// this routine assumes that the polygon intersects the screen
// it also assumes that the polgon is less wide than half the earth

int COvlkitUtil::geo_polygon_to_view(MapProj* map, int numverts,
                                      degrees_t *plat, degrees_t *plon,
                                      POINT *pt)
{
   int pixels_around_world;
   int screen_width, screen_height;
   degrees_t left_lon, right_lon, center_lon;
   degrees_t top_lat, bot_lat, center_lat;
   d_geo_t ll, ur;
   int left_ndx, right_ndx;
   int k, tx, ty, center_x;
   CFvwUtil *futil = CFvwUtil::get_instance();
   BOOL bigscr;

   if (numverts < 2)
      return FAILURE;

   map->get_pixels_around_world(&pixels_around_world);
   map->get_vsurface_size(&screen_width, &screen_height);
   if (screen_width > (pixels_around_world / 3))
      bigscr = TRUE;
   else
      bigscr = FALSE;

   // find the leftmost vertex
   left_lon = plon[0];
   right_lon = plon[0];
   top_lat = plat[0];
   bot_lat = plat[0];
   left_ndx = 0;
   right_ndx = 0;
   for (k=1; k<numverts; k++)
   {
      if (GEO_east_of_degrees(left_lon, plon[k]))
      {
         left_ndx = k;
         left_lon = plon[k];
      }
      if (GEO_east_of_degrees(plon[k], right_lon))
      {
         right_ndx = k;
         right_lon = plon[k];
      }
      if (plat[k] > top_lat)
         top_lat = plat[k];
      if (plat[k] < bot_lat)
         bot_lat = plat[k];
   }

   ll.lat = bot_lat;
   ll.lon = left_lon;
   ur.lat = top_lat;
   ur.lon = right_lon;
   futil->compute_center_geo(ll, ur, &center_lat, &center_lon);

   futil->geo_to_view_short(map, center_lat, center_lon, &center_x, &ty);

   // convert the vertexes
   for (k=0; k<numverts; k++)
   {
      futil->geo_to_view_short(map, plat[k], plon[k], &tx, &ty);
      if (bigscr)
      {
         if (GEO_east_of_degrees(plon[k], center_lon) && (tx < center_x))
            tx += pixels_around_world;
         if (GEO_east_of_degrees(center_lon, plon[k]) && (tx > center_x))
            tx -= pixels_around_world;
      }
      pt[k].x = tx;
      pt[k].y = ty;
   }
   return SUCCESS;
}
// end of geo_polygon_to_view

// *************************************************************
// *************************************************************

// this routine assumes that the polygon intersects the screen
// it also assumes that the polgon is less wide than half the earth

int COvlkitUtil::geo_polygon_to_view(MapProj* map, int numverts,
                           int line_type,   // simple=1, rhumb=2, great_circle=3
                           degrees_t *plat, degrees_t *plon, int *num_points, POINT **vpt)
{
   GeoSegment::heading_type_t simple_line = GeoSegment::SIMPLE_LINE;
   GeoSegment::heading_type_t rhumb_line = GeoSegment::RHUMB_LINE;
   GeoSegment::heading_type_t gc_line = GeoSegment::GREAT_CIRCLE;
   CList <CPoint *, CPoint *> list;
   int cnt, k;
   GeoLine *line;
   POINT *pt;
   CPoint *lpt;
   int endx, endy, tx, ty;
   BOOL start = TRUE;
   BOOL first = TRUE;
   BOOL reverse;
   d_geo_t ll, ur;
   int maxvert = 4000;

   *num_points = 0;

   if (numverts < 3)
      return FAILURE;

   pt = (POINT*) malloc(maxvert * sizeof(POINT));
   if (pt == NULL)
      return FAILURE;

   map->get_vmap_bounds(&ll, &ur);

   cnt = 0;

   for (k=0; k<numverts-1; k++)
   {
      if (GEO_line_test_degrees(ll.lat, ll.lon, ur.lat, ur.lon, plat[k], plon[k], plat[k+1], plon[k+1]))
      {
         line = new GeoLine(rhumb_line);
         if (line_type == UTIL_LINE_TYPE_GREAT)
            line->set_heading_type(gc_line);
         if (line_type == UTIL_LINE_TYPE_SIMPLE)
            line->set_heading_type(simple_line);
         line->set_clip_mode(FALSE);
         line->set_start(plat[k], plon[k]);
         line->set_end(plat[k+1], plon[k+1]);
         line->prepare_for_redraw(map);

         geo_to_surface(map, plat[k], plon[k], &tx, &ty);
         endx = tx;
         endy = ty;

         if (line->get_points(list))
         {
            lpt = list.GetHead();
            if ((abs(lpt->x - endx) > 2) || (abs(lpt->y - endy) > 2))
               reverse = TRUE;
            else
               reverse = FALSE;

            start = TRUE;
            while (!list.IsEmpty())
            {
               if (reverse)
                  lpt = list.RemoveTail();
               else
                  lpt = list.RemoveHead();

               if (cnt >= maxvert)
               {
                  maxvert += 1000;
                  pt = (POINT*) realloc(pt, maxvert * sizeof(POINT));
                  if (pt == NULL)
                  {
                     delete line;
                     return FAILURE;
                  }
               }
               pt[cnt].x = lpt->x;
               pt[cnt].y = lpt->y;
               tx = lpt->x;
               ty = lpt->y;
               cnt++;
               if (start && !first)
               {
                  if ((tx != endx) && (ty != endy))
                     endx = 0;
               }
               delete lpt;
               endx = tx;
               endy = ty;
               start = FALSE;
               first = FALSE;
            }
         }
         delete line;
      }
      else
      {
         POINT tpt[4];
         int numpt;

         get_geo_draw_line_points(map, plat[k], plon[k], plat[k+1], plon[k+1], tpt, &numpt);
         {
            ASSERT(numpt < 3);
            if (cnt >= maxvert)
            {
               maxvert += 1000;
               pt = (POINT*) realloc(pt, maxvert * sizeof(POINT));
               if (pt == NULL)
               {
                  delete line;
                  return FAILURE;
               }
            }
            pt[cnt].x = tpt[0].x;
            pt[cnt].y = tpt[0].y;
            cnt++;
            pt[cnt].x = tpt[1].x;
            pt[cnt].y = tpt[1].y;
            cnt++;
         }
      }
   }

   // get points from last line
   if (GEO_line_test_degrees(ll.lat, ll.lon, ur.lat, ur.lon, plat[numverts-1], plon[numverts-1], plat[0], plon[0]))
   {
      line = new GeoLine(rhumb_line);
      if (line_type == UTIL_LINE_TYPE_GREAT)
         line->set_heading_type(gc_line);
      if (line_type == UTIL_LINE_TYPE_SIMPLE)
         line->set_heading_type(simple_line);
      line->set_clip_mode(FALSE);
      line->set_start(plat[numverts-1], plon[numverts-1]);
      line->set_end(plat[0], plon[0]);
      line->prepare_for_redraw(map);
      if (GEO_east_of_degrees(plon[numverts-1], plon[0]))
         line->reverse_points();

      if (line->get_points(list))
      {
         while (!list.IsEmpty())
         {
            lpt = list.RemoveHead();
            if (cnt >= maxvert)
            {
               maxvert += 1000;
               pt = (POINT*) realloc(pt, maxvert * sizeof(POINT));
               if (pt == NULL)
               {
                  delete line;
                  return FAILURE;
               }
            }
            pt[cnt].x = lpt->x;
            pt[cnt].y = lpt->y;
            cnt++;
            delete lpt;
         }
      }
      delete line;
   }
   else
   {
      POINT tpt[4];
      int numpt;

      get_geo_draw_line_points(map, plat[numverts-1], plon[numverts-1], plat[0], plon[0], tpt, &numpt);
      ASSERT(numpt < 3);
      if (cnt >= maxvert)
      {
         maxvert += 1000;
         pt = (POINT*) realloc(pt, maxvert * sizeof(POINT));
         if (pt == NULL)
         {
            delete line;
            return FAILURE;
         }
      }
      pt[cnt].x = tpt[0].x;
      pt[cnt].y = tpt[0].y;
      cnt++;
      pt[cnt].x = tpt[1].x;
      pt[cnt].y = tpt[1].y;
      cnt++;
   }

   // copy to points
   *vpt = (POINT*) malloc(cnt * sizeof(POINT));
   if (*vpt == NULL)
   {
      free(pt);
      ERR_report("Unable to allocate memory for geo_polygon_to_view");
      return FAILURE;
   }

   memcpy((void*) *vpt, (void*) pt, cnt * sizeof(POINT));
   *num_points = cnt;
   free(pt);

   return SUCCESS;
}
// end of geo_polygon_to_view

// *************************************************************
// *************************************************************

// this routine assumes that the polygon intersects the screen
// it also assumes that the polgon is less wide than half the earth

int COvlkitUtil::geo_polygon_to_view(MapProj* map, int numverts,
                           int *line_type,   // simple=1, rhumb=2, great_circle=3
                           degrees_t *plat, degrees_t *plon, int *num_points, POINT **vpt)
{
   GeoSegment::heading_type_t simple_line = GeoSegment::SIMPLE_LINE;
   GeoSegment::heading_type_t rhumb_line = GeoSegment::RHUMB_LINE;
   GeoSegment::heading_type_t gc_line = GeoSegment::GREAT_CIRCLE;
   CList <CPoint *, CPoint *> list;
   GeoLine *line;
   POINT *pt;
   CPoint *lpt;
   int endx, endy, startx, starty, tx, ty;
   int cnt;
   BOOL start = TRUE;
   BOOL first = TRUE;
   BOOL reverse;
   d_geo_t ll, ur, tll, tur;
   int k, msize, p1, p2;
   double latwidth, lonwidth;
   CFvwUtil *futil = CFvwUtil::get_instance();

   msize = 40000;
   *num_points = 0;

   if (numverts < 3)
      return FAILURE;

   map->get_vmap_bounds(&ll, &ur);

   pt = (POINT*) malloc(msize * sizeof(POINT));

   cnt = 0;

   for (k=0; k<numverts; k++)
   {
      if (k > numverts-2)
      {
         p1 = numverts-1;
         p2 = 0;
      }
      else
      {
         p1 = k;
         p2 = k+1;
      }

      tll.lat = plat[p1];
      tll.lon = plon[p1];
      tur.lat = plat[p2];
      tur.lon = plon[p2];
      if (tur.lat < tll.lat)
      {
         tll.lat = plat[p2];
         tur.lat = plat[p1];
      }
      if (GEO_east_of_degrees(tll.lon, tur.lon))
      {
         tll.lon = plon[p2];
         tur.lon = plon[p1];
      }
      // kludge: expand it a bit to account for GC and rhumb line
      latwidth = tur.lat - tll.lat;
      lonwidth = tur.lon - tll.lon;
      if (lonwidth > 180.0)
         lonwidth = 360.0 - lonwidth;
      if (lonwidth < -180.0)
         lonwidth = 360.0 + lonwidth;
      latwidth /= 4.0;
      lonwidth /= 4.0;
      tll.lat -= latwidth;
      if (tll.lat < -90.0)
         tll.lat = -89.999999;
      tur.lat += latwidth;
      if (tur.lat > 90.0)
         tur.lat = 89.999999;
      tll.lon -= lonwidth;
      if (tll.lon < -180.0)
         tll.lon += 360.0;
      tur.lon += lonwidth;
      if (tur.lon > 180.0)
         tur.lon -= 360.0;

      if (GEO_intersect_degrees(ll, ur, tll, tur))
      {
         line = new GeoLine(rhumb_line);
         if (line_type[k] == UTIL_LINE_TYPE_GREAT)
            line->set_heading_type(gc_line);
         if (line_type[k] == UTIL_LINE_TYPE_SIMPLE)
            line->set_heading_type(simple_line);
         line->set_clip_mode(FALSE);
         line->set_start(plat[p1], plon[p1]);
         line->set_end(plat[p2], plon[p2]);
         line->prepare_for_redraw(map);

         // get the line segment endpoint in pixels
         geo_to_surface(map, plat[p1], plon[p1], &tx, &ty);
         startx = tx;
         starty = ty;
         geo_to_surface(map, plat[p2], plon[p2], &tx, &ty);
         endx = tx;
         endy = ty;

         // add the explicit point
         pt[cnt].x = startx;
         pt[cnt].y = starty;
         cnt++;

         if (line->get_points(list))
         {
            // find which end is closer
            int d, d2;
            lpt = list.GetHead();
            d = futil->magnitude(startx, starty, lpt->x, lpt->y);
            d2 = futil->magnitude(endx, endy, lpt->x, lpt->y);
            if (d2 < d)
               reverse = TRUE;
            else
               reverse = FALSE;

            start = TRUE;
            while (!list.IsEmpty())
            {
               if (reverse)
                  lpt = list.RemoveTail();
               else
                  lpt = list.RemoveHead();

               pt[cnt].x = lpt->x;
               pt[cnt].y = lpt->y;
               tx = lpt->x;
               ty = lpt->y;
               cnt++;
               if (cnt >= msize)
               {
                  msize += 20000;
                  pt = (POINT*) realloc(pt, msize * sizeof(POINT));
                  if (pt == NULL)
                     return FAILURE;
               }

               if (start && !first)
               {
                  if ((tx != endx) && (ty != endy))
                     endx = 0;
               }
               delete lpt;
               endx = tx;
               endy = ty;
               start = FALSE;
               first = FALSE;
            }
         }
         delete line;
      }
      else
      {
         POINT tpt[4];
         int numpt;

         get_geo_draw_line_points(map, plat[p1], plon[p1], plat[p2], plon[p2], tpt, &numpt);
         if (numpt == 2)
         {
            pt[cnt].x = tpt[0].x;
            pt[cnt].y = tpt[0].y;
            cnt++;
            ASSERT(numpt < 3);
            if (cnt >= msize)
            {
               msize += 20000;
               pt = (POINT*) realloc(pt, msize * sizeof(POINT));
               if (pt == NULL)
                  return FAILURE;
            }
            pt[cnt].x = tpt[1].x;
            pt[cnt].y = tpt[1].y;
            cnt++;
         }
      }
   }

   // copy to points
   *vpt = (POINT*) malloc(cnt * sizeof(POINT));
   if (*vpt == NULL)
   {
      free(pt);
      ERR_report("Unable to allocate memory for geo_polygon_to_view");
      return FAILURE;
   }

   memcpy((void*) *vpt, (void*) pt, cnt * sizeof(POINT));
   *num_points = cnt;

   free(pt);

   return SUCCESS;
}
// end of geo_polygon_to_view

// *************************************************************
// *************************************************************

BOOL COvlkitUtil::geo_polygon_on_screen(MapProj* map, int numverts, degrees_t *plat, degrees_t *plon)
{
   d_geo_t map_ll, map_ur, pll, pur;
   double minlat, minlon, maxlat, maxlon;
   int k;

   if (map->get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
      return FAILURE;

   // find the bounds of the polygon
   minlat = plat[0];
   maxlat = plat[0];
   minlon = plon[0];
   maxlon = plon[0];
   for (k=0; k<numverts; k++)
   {
      if (plat[k] < minlat)
         minlat = plat[k];
      if (plat[k] > maxlat)
         maxlat = plat[k];
      if (GEO_east_of_degrees(minlon, plon[k]))
         minlon = plon[k];
      if (GEO_east_of_degrees(plon[k], maxlon))
         maxlon = plon[k];
   }
   pll.lat = minlat;
   pll.lon = minlon;
   pur.lat = maxlat;
   pur.lon = maxlon;

   if (!GEO_intersect_degrees(pll, pur, map_ll, map_ur))
      if (!GEO_enclose_degrees(pll, pur, map_ll, map_ur))
         return FALSE;

   return TRUE;
}

// *************************************************************
// *************************************************************

void COvlkitUtil::add_line_to_polygon_list(POINT start, POINT end,
         CList<C_ovlkit_screen_polygon*, C_ovlkit_screen_polygon*> & plist)
{
   POSITION next;
   C_ovlkit_small_vertex *hvtx;
   C_ovlkit_small_vertex *tvtx;
   C_ovlkit_small_vertex *vtx;
   C_ovlkit_screen_polygon *poly;
   BOOL found;
   int cnt;

   next = plist.GetHeadPosition();
   if (next == NULL)
   {
      poly = new C_ovlkit_screen_polygon();
      vtx = new C_ovlkit_small_vertex(OP_MOVETO, start.x, start.y);
      vtx->m_x2 = end.x;
      vtx->m_y2 = end.y;
      poly->m_line_list.AddTail(vtx);
      plist.AddTail(poly);
      return;
   }

   found = FALSE;
   while (next != NULL)
   {
      poly = plist.GetNext(next);
      cnt =  poly->m_line_list.GetCount();
      hvtx = poly->m_line_list.GetHead();
      tvtx = poly->m_line_list.GetTail();
      if (((tvtx->m_x == start.x) && (tvtx->m_y == start.y)) ||
          ((tvtx->m_x == end.x) && (tvtx->m_y == end.y))  ||
          ((tvtx->m_x2 == start.x) && (tvtx->m_y2 == start.y)) ||
          ((tvtx->m_x2 == end.x) && (tvtx->m_y2 == end.y))  ||
          ((hvtx->m_x == start.x) && (hvtx->m_y == start.y)) ||
          ((hvtx->m_x == end.x) && (hvtx->m_y == end.y)) ||
          ((hvtx->m_x2 == start.x) && (hvtx->m_y2 == start.y)) ||
          ((hvtx->m_x2 == end.x) && (hvtx->m_y2 == end.y)))
      {
         vtx = new C_ovlkit_small_vertex(OP_MOVETO, start.x, start.y);
         vtx->m_x2 = end.x;
         vtx->m_y2 = end.y;
         poly->m_line_list.AddTail(vtx);
         found = TRUE;
      }
   }
   if (!found)
   {
      poly = new C_ovlkit_screen_polygon();
      vtx = new C_ovlkit_small_vertex(OP_MOVETO, start.x, start.y);
      vtx->m_x2 = end.x;
      vtx->m_y2 = end.y;
      poly->m_line_list.AddTail(vtx);
      plist.AddTail(poly);
   }
   return;
}
// end of add_line_to_polygon_list

// *************************************************************
// *************************************************************

void COvlkitUtil::get_geo_line_coordinates(MapProj *map,
                         d_geo_t geo1, d_geo_t geo2,
                         CPoint &p1, CPoint &p2,
                         CPoint &p1_wrap, CPoint &p2_wrap)
{
   d_geo_t ll, ur;
   int x1, y1;          /* device coordinates of western point */
   int x2, y2;          /* device coordinates of eastern point */
   int p1_flags;        /* NSEW bounds check flags for point 1 */
   int p2_flags;        /* NSEW bounds check flags for point 2 */
   boolean_t east_of;   /* point 2 is east of point 1 */
   int pixels_around_world;
   int screen_width, screen_height;
   BOOL reversed;
   CFvwUtil *futil = CFvwUtil::get_instance();

   double lat1 = geo1.lat;
   double lon1 = geo1.lon;
   double lat2 = geo2.lat;
   double lon2 = geo2.lon;

   p1_wrap = p2_wrap = CPoint(-1, -1);

   // check for screen intersection
   map->get_vmap_bounds(&ll, &ur);
   map->get_pixels_around_world(&pixels_around_world);
   map->get_surface_size(&screen_width, &screen_height);

   // treat world overview as a special case
   if (screen_width >= (pixels_around_world/ 2))
   {
      // get device coordinates
      if (GEO_east_of_degrees(lon2, lon1))
      {
         futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
         futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);
      }
      else         /* p2 is west of p1 or lon1=lon2 */
      {
         futil->geo_to_view_short(map, lat2, lon2, &x1, &y1);
         futil->geo_to_view_short(map, lat1, lon1, &x2, &y2);
      }

      if (x1 > x2)
      {
         // wrap around world
         p1 = CPoint(x1, y1);
         p2 = CPoint (x2 + pixels_around_world, y2);
         p1_wrap = CPoint(x1 - pixels_around_world, y1);
         p2_wrap = CPoint(x2, y2);
      }
      else
      {
         p1 = CPoint(x1, y1);
         p2 = CPoint(x2, y2);
      }

      return;
   }

   // get NSEW bounds check flags for each point
   p1_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
     ur.lat, ur.lon, lat1, lon1);
   p2_flags = GEO_bounds_check_degrees(ll.lat, ll.lon,
     ur.lat, ur.lon, lat2, lon2);

   // quick accept - both points on map
   if ((p1_flags | p2_flags) == 0)
   {
      // get device coordinates
      futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
      futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);

      p1 = CPoint(x1, y1);
      p2 = CPoint(x2, y2);

      return;
   }

   // quick reject
   if (p1_flags & p2_flags)
   {
      // we need to set the points to something since the caller does
      // not know if this function succeeds or not
      p1 = CPoint(-100, -100);
      p2 = CPoint(-100, -100);
      return;
   }

   // reject lines that go from west of map to east of map but
   // not through the region defined by left_lon and right_lon
   if (((p1_flags ^ p2_flags) & (GEO_EAST_OF | GEO_WEST_OF)) ==
      (GEO_EAST_OF | GEO_WEST_OF))
   {
      if (p1_flags & GEO_EAST_OF)
      {
         if (GEO_east_of_degrees(lon2, lon1))
            return;

         // needed to get device coordinates in right order
         east_of = FALSE;
      }
      else
      {
         if (GEO_east_of_degrees(lon1, lon2))
            return;

         // needed to get device coordinates in right order
         east_of = TRUE;
      }
   }
   else
      east_of = GEO_east_of_degrees(lon2, lon1);

   // get device coordinates
   if (east_of)
   {
      futil->geo_to_view_short(map, lat1, lon1, &x1, &y1);
      futil->geo_to_view_short(map, lat2, lon2, &x2, &y2);

      int vx1, vx2, vy;
      map->geo_to_vsurface(lat1, lon1, &vx1, &vy);
      map->geo_to_vsurface(lat2, lon2, &vx2, &vy);
      if (vx2 < vx1)
         x2 += pixels_around_world;
      reversed = FALSE;
   }
   else
   {
      futil->geo_to_view_short(map, lat2, lon2, &x1, &y1);
      futil->geo_to_view_short(map, lat1, lon1, &x2, &y2);

      int vx1, vx2, vy;
      map->geo_to_vsurface(lat1, lon1, &vx1, &vy);
      map->geo_to_vsurface(lat2, lon2, &vx2, &vy);
      if (vx1 < vx2)
         x1 += pixels_around_world;
      reversed = TRUE;
   }

   if (reversed)
   {
      p1 = CPoint(x2, y2);
      p2 = CPoint(x1, y1);
   }
   else
   {
      p1 = CPoint(x1, y1);
      p2 = CPoint(x2, y2);
   }
}

// *************************************************************
// *************************************************************

// this routine takes a geo_polygon and returns two lists:
// line_list is a list of move_to line_to items to draw the outline,
// polygon_list is a list of polygons that are all completely on the screen


int COvlkitUtil::geo_polygon_to_view(MapProj* map, int numverts,
                                      degrees_t *plat, degrees_t *plon,
                                      CList<C_ovlkit_screen_polygon*, C_ovlkit_screen_polygon*> & polygon_list)

{
   int k;
   POINT pt[1000];
   int num_pt;
   POSITION next;
   C_ovlkit_small_vertex *vtx;
   C_ovlkit_small_vertex *hvtx;
   C_ovlkit_screen_polygon *poly;

   if (numverts < 2)
      return FAILURE;

   // is the polygon completely on the screen, simplest case
   for (k=0; k<numverts-1; k++)
   {
      get_geo_draw_line_points(map, plat[k], plon[k], plat[k+1], plon[k+1],
                                 pt,  &num_pt);
      add_line_to_polygon_list(pt[0], pt[1], polygon_list);
      if (num_pt > 2)
      {
         add_line_to_polygon_list(pt[2], pt[3], polygon_list);
      }

   }

   // do the last line
   get_geo_draw_line_points(map, plat[k], plon[k], plat[0], plon[0],
                              pt,  &num_pt);
   add_line_to_polygon_list(pt[0], pt[1], polygon_list);
   if (num_pt > 2)
   {
      add_line_to_polygon_list(pt[2], pt[3], polygon_list);
   }

   // ensure closure of polygons
   next = polygon_list.GetHeadPosition();
   while (next != NULL)
   {
      poly = polygon_list.GetNext(next);
      hvtx = poly->m_line_list.GetHead();
      vtx = new C_ovlkit_small_vertex(OP_LINETO, hvtx->m_x, hvtx->m_y);
      vtx->m_x2 = hvtx->m_x2;
      vtx->m_y2 = hvtx->m_y2;
      poly->m_line_list.AddTail(vtx);
   }

   return SUCCESS;
}
// end of geo_polygon_to_view


// *************************************************************
// *****************************************************************

// this routine returns the maximum of the world in the the current view
//

int COvlkitUtil::percent_world_in_view(MapProj* map)
{
   d_geo_t ll, ur;
   int pixels_around_world;
   int screen_width, screen_height;
   degrees_t lat[10], lon[10];
   degrees_t tlat, tlon;
   double half_world_meters1;
   double half_world_meters2;
   double half_world_meters3;
   double screen_meters1;
   double screen_meters2;
   double screen_meters3;
   double dist, ang;
   double world_percent, percent;

   if (map->scale() == WORLD)
      return 100;

   map->get_vmap_bounds(&ll, &ur);
   map->get_pixels_around_world(&pixels_around_world);
   map->get_surface_size(&screen_width, &screen_height);

   map->surface_to_geo(0, 0, &tlat, &tlon);
   lat[0] = tlat;
   lon[0] = tlon;
   map->surface_to_geo(screen_width / 2, 0, &tlat, &tlon);
   lat[1] = tlat;
   lon[1] = tlon;
   map->surface_to_geo(screen_width, 0, &tlat, &tlon);
   lat[2] = tlat;
   lon[2] = tlon;
   map->surface_to_geo(0, screen_height / 2, &tlat, &tlon);
   lat[3] = tlat;
   lon[3] = tlon;
   map->surface_to_geo(screen_width / 2, screen_height / 2, &tlat, &tlon);
   lat[4] = tlat;
   lon[4] = tlon;
   map->surface_to_geo(screen_width, screen_height / 2, &tlat, &tlon);
   lat[5] = tlat;
   lon[5] = tlon;
   map->surface_to_geo(0, screen_height, &tlat, &tlon);
   lat[6] = tlat;
   lon[6] = tlon;
   map->surface_to_geo(screen_width / 2, screen_height, &tlat, &tlon);
   lat[7] = tlat;
   lon[7] = tlon;
   map->surface_to_geo(screen_width, screen_height, &tlat, &tlon);
   lat[8] = tlat;
   lon[8] = tlon;

   GEO_geo_to_distance(lat[0], 0.0, lat[0], 180.0, &half_world_meters1, &ang);
   GEO_geo_to_distance(lat[3], 0.0, lat[3], 180.0, &half_world_meters2, &ang);
   GEO_geo_to_distance(lat[6], 0.0, lat[6], 180.0, &half_world_meters3, &ang);

   GEO_geo_to_distance(lat[0], lon[0], lat[1], lon[1], &screen_meters1, &ang);
   GEO_geo_to_distance(lat[1], lon[1], lat[2], lon[2], &dist, &ang);
   screen_meters1 += dist;
   GEO_geo_to_distance(lat[3], lon[3], lat[4], lon[4], &screen_meters2, &ang);
   GEO_geo_to_distance(lat[4], lon[4], lat[5], lon[5], &dist, &ang);
   screen_meters2 += dist;
   GEO_geo_to_distance(lat[6], lon[6], lat[7], lon[7], &screen_meters3, &ang);
   GEO_geo_to_distance(lat[7], lon[7], lat[8], lon[8], &dist, &ang);
   screen_meters3 += dist;

   // prevent divide by zero
   if (half_world_meters1 < 1.0)
      return 100;
   world_percent = screen_meters1 / half_world_meters1;
   if (half_world_meters2 < 1.0)
      return 100;
   percent = screen_meters2 / half_world_meters2;
   if (percent > world_percent)
      world_percent = percent;
   if (half_world_meters3 < 1.0)
      return 100;
   percent = screen_meters3 / half_world_meters3;
   if (percent > world_percent)
      world_percent = percent;
   world_percent *= 100.0;
   world_percent += 0.5;
   return (int) world_percent;
}
// end of percent_world_in_view

// ****************************************************************
// ****************************************************************

CString COvlkitUtil::get_temp_path()
{
   CString sdata;
   char buf[100];
   CString filename;

   int rslt = GetTempPath(sizeof(buf), buf);
   if (rslt < 1)
   {
      CString error_msg = "Cannot get temporary file path";
      AfxMessageBox(error_msg);
      filename = "";
      return filename;
   }

   sdata = buf;

   sdata += "\\";
   return sdata;
}
// end of get_temp_path

// ****************************************************************
// ****************************************************************

CString COvlkitUtil::get_temp_jpeg_name()
{
   CString filename;

   filename = get_temp_path();
   filename += "temp.jpg";

   return filename;
}

// ****************************************************************
// ****************************************************************

// get the size of a section JPEG of the current input image

int COvlkitUtil::get_jpeg_size(CString filename, int & image_width, int & image_height, int & jpeg_size)
{
   int hFile, rslt;
   OFSTRUCT ofs;
   DWORD hiBits;
   CString error_message;

   // get the size
   // open temp jpeg file
   if ((hFile = OpenFile(filename, &ofs, OF_READ)) == -1)
   {
      CString msg;

      msg.Format("Unable to open temporary file -- %s", filename);
      AfxMessageBox(msg);
      return FAILURE;
   }

   jpeg_size = GetFileSize((HANDLE) hFile, &hiBits);

   _lclose(hFile);

   try
   {
      IImageLibPtr smpImageLib;
      smpImageLib.CreateInstance(__uuidof(ImageLib));

      int err;
      CComBSTR err_msg;
      HRESULT hr = smpImageLib->load(_bstr_t(filename), &image_width, &image_height, &err, &err_msg);

      rslt = (err == 0) ? SUCCESS : FAILURE;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to load JPEG to obtain size : %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return rslt;
}
// end of get_temp_jpeg_size


// ****************************************************************
// ****************************************************************

// add carets to a number string

void COvlkitUtil::add_carets(CString &num)
{
   int len, pos, cnt, k;
   CString tstr, decstr("");

   tstr = num;
   len = num.GetLength();
   pos = num.Find('.');
   if (pos >= 0)
   {
      decstr = num.Right(len - pos);
      tstr = num.Left(pos);
   }

   len = tstr.GetLength();
   if (len < 4)
      return;

   cnt = len / 3;

   for (k=1; k<=cnt; k++)
   {
      pos = len - (3 * k);
      tstr.Insert(pos, ",");
   }
   num = tstr + decstr;
}

// ****************************************************************
// ****************************************************************

int COvlkitUtil::imagelib_load(CString filename, int *width, int *height, CString & error_msg)
{
   int len, err_code;
   BSTR berr_msg;
   IImageLibPtr imagelib;

   HRESULT hr = imagelib.CreateInstance(__uuidof(ImageLib));

   if (hr != S_OK)
   {
      error_msg = "Error creating instance of ImageLib";
      return FAILURE;
   }

   try
   {
      len = filename.GetLength();
      imagelib->load(_bstr_t(filename.GetBuffer(len)), width, height, &err_code, &berr_msg);
      if (err_code != 0)
      {
         error_msg = berr_msg;
         return FAILURE;
      }
   }
   catch(COleException *e)
   {
      e->Delete();
      return FAILURE;
   }
   catch(CMemoryException *e)
   {
      e->Delete();
      return FAILURE;
   }
   catch(...)
   {
      return FAILURE;
   }

   error_msg = "";
   return SUCCESS;
}
// end of imagelib_load

// ****************************************************************
// ****************************************************************

int COvlkitUtil::get_imagelib_subimage(CString filename, double factor,
                              int image_offset_x, int image_offset_y,
                              int width, int height,
                              unsigned char *red_array,
                              unsigned char *green_array,
                              unsigned char *blue_array,
                              CString & error_msg)
{
   int rslt;
   BYTE *img;
   VARIANT var_image_data;
   int size, len;
   int err_code;
   BSTR berr_msg;
   int x, img_width, img_height;
   IImageLibPtr imagelib;

   HRESULT hr = imagelib.CreateInstance(__uuidof(ImageLib));

   try
   {
      len = filename.GetLength();
      imagelib->load(_bstr_t(filename.GetBuffer(len)), &img_width, &img_height, &err_code, &berr_msg);
      if (err_code != 0)
      {
         error_msg = berr_msg;
         return FAILURE;
      }
   }
   catch(COleException *e)
   {
      e->Delete();
      return FAILURE;
   }
   catch(CMemoryException *e)
   {
      e->Delete();
      return FAILURE;
   }
   catch(...)
   {
      return FAILURE;
   }


   size = width * height;

   try
   {
      imagelib->get_zoomed_rgb_subimage(factor, image_offset_x, image_offset_y, width, height, &var_image_data, &err_code, &berr_msg);

      if (err_code != 0)
      {
         error_msg = berr_msg;
         return FAILURE;
      }
   }
   catch(COleException *e)
   {
      e->Delete();
      error_msg = "Unable to use ImageLib.dll";
      return FAILURE;
   }
   catch(CMemoryException *e)
   {
      e->Delete();
      error_msg = "Unable to use ImageLib.dll";
      return FAILURE;
   }
   catch(...)
   {
      error_msg = "Unable to use ImageLib.dll";
      return FAILURE;
   }


   rslt = SUCCESS;

   COleSafeArray image_data_sa(var_image_data);
   try
   {
      image_data_sa.AccessData((void **)&img);
   }
   catch(COleException *e)
   {
      e->Delete();
      return -1;
   }
   catch(CMemoryException *e)
   {
      e->Delete();
      return -1;
   }

   for (x=0; x<size; x++)
   {
      red_array[x] = img[(x*3)+0];
      green_array[x] = img[(x*3)+1];
      blue_array[x] = img[(x*3)+2];
   }

   image_data_sa.UnaccessData();

   image_data_sa.Clear();

   VariantClear(&var_image_data);

   return SUCCESS;
}
// end of get_imagelib_subimage

// ****************************************************************
// ****************************************************************

int COvlkitUtil::save_imagelib_image(CString filename, int width, int height, BYTE *img_data, int quality, CString &error_msg)
{
   int size, len;
   BSTR berr_msg;
   int err_code;
   BYTE *img = NULL;
   IImageLibPtr imagelib;

   HRESULT hr = imagelib.CreateInstance(__uuidof(ImageLib));

   len = filename.GetLength();
   size = width * height * 3;

   COleSafeArray image_data;
   try
   {
      unsigned long num_elements = size;
      image_data.Create(VT_UI1, 1, &num_elements);
   }
   catch(CMemoryException *e)
   {
      AfxMessageBox("Out of memory error writing PNG file.");
      e->Delete();
      return FAILURE;
   }

   try
   {
      image_data.AccessData((void **)&img);
   }
   catch(COleException *e)
   {
      e->Delete();
      return FAILURE;
   }
   catch(CMemoryException *e)
   {
      e->Delete();
      return FAILURE;
   }

   memcpy(img, img_data, size);

   image_data.UnaccessData();
   imagelib->write_jpeg_file(_bstr_t(filename.GetBuffer(len)), width, height, quality, image_data, &err_code, &berr_msg);
   if (err_code == 0)
      return SUCCESS;

   error_msg = berr_msg;

   return FAILURE;
}

// ****************************************************************
// ****************************************************************

inline char COvlkitUtil::base64_encode_char(unsigned char uc)
{
   if (uc < 26)
   {
      return 'A'+uc;
   }
   if (uc < 52)
   {
      return 'a'+(uc-26);
   }
   if (uc < 62)
   {
      return '0'+(uc-52);
   }
   if (uc == 62)
   {
      return '+';
   }
   return '/';
};

// ****************************************************************
// ****************************************************************

inline unsigned char COvlkitUtil::base64_decode_char(char c)
{
   if (c >= 'A' && c <= 'Z')
   {
      return c - 'A';
   }
      if (c >= 'a' && c <= 'z')
   {
      return c - 'a' + 26;
   }
   if (c >= '0' && c <= '9')
   {
      return c - '0' + 52;
   }
   if (c == '+')
   {
      return 62;
   }
   return 63;
}

// ****************************************************************
// ****************************************************************

inline bool COvlkitUtil::is_char_base64(char c)
{
   if (c >= 'A' && c <= 'Z')
   {
      return true;
   }
   if (c >= 'a' && c <= 'z')
   {
      return true;
   }
   if (c >= '0' && c <= '9')
   {
      return true;
   }
   if (c == '+')
   {
      return true;
   };
   if (c == '/')
   {
      return true;
   };
   if (c == '=')
   {
      return true;
   };
   return false;
}

// ****************************************************************
// ****************************************************************

int COvlkitUtil::base64_encode(const std::vector<unsigned char> & data, std::string & basestr)
{
   int size, by1, by2, by3, by4, by5, by6, by7;

   size = data.size();

   if (size == 0)
   {
      return FAILURE;
   }

   for (int i=0;i<size;i+=3)
   {
      by1 = by2 = by3 = by4 = by5 = by6 = by7 = 0;
      by1 = data[i];

      if (i+1 < size)
      {
         by2 = data[i+1];
      }
      if (i+2 < size)
      {
         by3 = data[i+2];
      }
      by4 = by1 >> 2;
      by5 = ((by1 & 0x3) << 4) | (by2 >> 4);
      by6 = ((by2 & 0xf) << 2) | (by3 >> 6);
      by7 = by3 & 0x3f;

      basestr += base64_encode_char(by4);
      basestr += base64_encode_char(by5);

      if (i+1 < size)
      {
         basestr += base64_encode_char(by6);
      }
      else
      {
         basestr += "=";
      }
      if (i+2 < size)
      {
         basestr += base64_encode_char(by7);
      }
      else
      {
         basestr += "=";
      }
      if (i % (76/4*3) == 0)
      {
         basestr += "\r\n";
      }
   }
   return SUCCESS;
}
// end of base64_encode

// ****************************************************************
// ****************************************************************

std::vector<unsigned char> COvlkitUtil::base64_decode(const std::string & base_str)
{
   std::vector<unsigned char> data;
   std::string basestr;
   int len;

   len = base_str.length();

   // filter out the non-base64 chars (CR/LF, etc.)
   for (int j=0; j<len; j++)
   {
      if (is_char_base64(base_str[j]))
      {
         basestr += base_str[j];
      }
   }
   if (basestr.length() == 0)
   {
      return data;
   }
   for (size_t i=0; i < basestr.length();i+=4)
   {
      char c1, c2, c3, c4;
      unsigned char by1, by2, by3, by4;

      c1 = c2 = c3 = c4 = 'A';

      c1 = basestr[i];
      if (i+1 < basestr.length())
      {
         c2 = basestr[i+1];
      }
      if (i+2 < basestr.length())
      {
         c3 = basestr[i+2];
      }
      if (i+3 < basestr.length())
      {
         c4 = basestr[i+3];
      }

      by1 = by2 = by3 = by4 = 0;

      by1 = base64_decode_char(c1);
      by2 = base64_decode_char(c2);
      by3 = base64_decode_char(c3);
      by4 = base64_decode_char(c4);

      data.push_back( (by1 << 2) | (by2 >> 4) );
      if (c3 != '=')
      {
         data.push_back( ((by2 & 0xf)<<4) | (by3>>2) );
      }
      if (c4 != '=')
      {
         data.push_back( ((by3 & 0x3) << 6) | by4 );
      }
   }
   return data;
}
// end of base64_decode

// ****************************************************************
// ****************************************************************

C_ovlkit_small_vertex::C_ovlkit_small_vertex(int opcode, int x, int y)
{
   m_opcode = opcode;
   m_x = x;
   m_y = y;
}

// *******************************************************************

C_ovlkit_small_vertex::~C_ovlkit_small_vertex()
{

}

// *******************************************************************
// *******************************************************************

C_ovlkit_screen_polygon::C_ovlkit_screen_polygon()
{
}

// ******************************************************************
// ******************************************************************

C_ovlkit_screen_polygon::~C_ovlkit_screen_polygon()
{
   clear();
}

// ******************************************************************
// ******************************************************************

void C_ovlkit_screen_polygon::clear()
{
   C_ovlkit_small_vertex* vtx;

   if ( m_line_list.IsEmpty() )
      return;

   while ( !m_line_list.IsEmpty() )
   {
      vtx = m_line_list.RemoveHead();
      if (vtx != NULL)
         delete vtx;
   }
}

// ******************************************************************
// ********************************************************************
/*
int C_ovlkit_screen_polygon::rectangle_in_polygon(int width, int height, CPoint *ctr_pt)
{
   POSITION next;
   C_ovlkit_small_vertex* vtx;
   int minx, miny, maxx, maxy;
   int sumx, sumy, cnt, incx, incy, stx, sty;
   int hh, hw;
   bool first = true;
   CPoint pt(0, 0);
   CPoint pt1, pt2, pt3, pt4;
   CRect rc;

   if (m_line_list.IsEmpty())
      return FAILURE;

   hw = width / 2;
   hh = height / 2;

   cnt = 0;
   next = m_line_list.GetHeadPosition();
   while (next != NULL)
   {
      vtx = m_line_list.GetNext(next);
      if (first)
      {
         sumx = vtx->m_x;
         sumy = vtx->m_y;
         minx = maxx = vtx->m_x;
         miny = maxy = vtx->m_y;
         first = false;
      }
      else
      {
         sumx += vtx->m_x;
         sumy += vtx->m_y;
         if (vtx->m_x > maxx)
            maxx = vtx->m_x;
         if (vtx->m_x < minx)
            minx = vtx->m_x;
         if (vtx->m_y > maxy)
            maxy = vtx->m_y;
         if (vtx->m_y < miny)
            miny = vtx->m_y;
      }
      cnt++;
   }

   pt.x = sumx / cnt;
   pt.y = sumy / cnt;

   pt1.x = pt.x - hw;
   pt1.y = pt.y - hh;
   pt2.x = pt.x + hw;
   pt2.y = pt.y - hh;
   pt3.x = pt.x + hw;
   pt3.y = pt.y + hh;
   pt4.x = pt.x - hw;
   pt4.y = pt.y + hh;

   if (point_in_polygon(pt1))
   {
      *ctr_pt = pt;
      return SUCCESS;
   }

   stx = pt.x;
   sty = pt.y;

   // try going up
   incy = (pt.y - miny) / 10;
   while (pt.y > miny)
   {
      pt.y -= incy;
      if (point_in_polygon(pt))
      {
         *ctr_pt = pt;
         return SUCCESS;
      }
   }

   // try going down
   pt.x = stx;
   pt.y = sty;
   incy = (maxy - pt.y) / 10;
   while (pt.y < maxy)
   {
      pt.y += incy;
      if (point_in_polygon(pt))
      {
         *ctr_pt = pt;
         return SUCCESS;
      }
   }

   // try going left
   pt.x = stx;
   pt.y = sty;
   incx = (pt.x - minx) / 10;
   while (pt.x > minx)
   {
      pt.x -= incx;
      if (point_in_polygon(pt))
      {
         *ctr_pt = pt;
         return SUCCESS;
      }
   }

   // try going right
   pt.x = stx;
   pt.y = sty;
   incx = (maxx - pt.x) / 10;
   while (pt.x < maxx)
   {
      pt.x += incx;
      if (point_in_polygon(pt))
      {
         *ctr_pt = pt;
         return SUCCESS;
      }
   }

   return FAILURE;
}

*/

// ******************************************************************
// ********************************************************************

int C_ovlkit_screen_polygon::center_of_polygon(CPoint *ctr_pt)
{
   POSITION next;
   C_ovlkit_small_vertex* vtx;
   int minx, miny, maxx, maxy;
   int sumx, sumy, cnt, incx, incy, stx, sty;
   bool first = true;
   CPoint pt(0, 0);

   if (m_line_list.IsEmpty())
      return FAILURE;

   cnt = 0;
   next = m_line_list.GetHeadPosition();
   while (next != NULL)
   {
      vtx = m_line_list.GetNext(next);
      if (first)
      {
         sumx = vtx->m_x;
         sumy = vtx->m_y;
         stx = sumx;
         sty = sumy;
         minx = maxx = vtx->m_x;
         miny = maxy = vtx->m_y;
         first = false;
         cnt++;
      }
      else
      {
         if ((next != NULL) || (vtx->m_x != stx) || (vtx->m_y != sty))
         {
            sumx += vtx->m_x;
            sumy += vtx->m_y;
            if (vtx->m_x > maxx)
               maxx = vtx->m_x;
            if (vtx->m_x < minx)
               minx = vtx->m_x;
            if (vtx->m_y > maxy)
               maxy = vtx->m_y;
            if (vtx->m_y < miny)
               miny = vtx->m_y;
            cnt++;
         }
      }
   }

   pt.x = sumx / cnt;
   pt.y = sumy / cnt;

   if (point_in_polygon(pt))
   {
      *ctr_pt = pt;
      return SUCCESS;
   }

   stx = pt.x;
   sty = pt.y;

   // try going up
   incy = (pt.y - miny) / 10;
   while (pt.y > miny)
   {
      pt.y -= incy;
      if (point_in_polygon(pt))
      {
         *ctr_pt = pt;
         return SUCCESS;
      }
   }

   // try going down
   pt.x = stx;
   pt.y = sty;
   incy = (maxy - pt.y) / 10;
   while (pt.y < maxy)
   {
      pt.y += incy;
      if (point_in_polygon(pt))
      {
         *ctr_pt = pt;
         return SUCCESS;
      }
   }

   // try going left
   pt.x = stx;
   pt.y = sty;
   incx = (pt.x - minx) / 10;
   while (pt.x > minx)
   {
      pt.x -= incx;
      if (point_in_polygon(pt))
      {
         *ctr_pt = pt;
         return SUCCESS;
      }
   }

   // try going right
   pt.x = stx;
   pt.y = sty;
   incx = (maxx - pt.x) / 10;
   while (pt.x < maxx)
   {
      pt.x += incx;
      if (point_in_polygon(pt))
      {
         *ctr_pt = pt;
         return SUCCESS;
      }
   }

   return FAILURE;
}

// ******************************************************************
// ********************************************************************

BOOL C_ovlkit_screen_polygon::point_in_polygon(CPoint testpt)
{
   POSITION next;
   C_ovlkit_small_vertex* vtx;
   int cnt, k;
   POINT *pt;
   BOOL rslt;

   if ( m_line_list.IsEmpty() )
      return FALSE;

   cnt = m_line_list.GetCount();
   pt = new POINT[cnt];
   k = 0;

   next = m_line_list.GetHeadPosition();
   while (next != NULL)
   {
      vtx = m_line_list.GetNext(next);
      pt[k].x = vtx->m_x;
      pt[k].y = vtx->m_y;
      k++;
   }

   rslt = point_in_polygon(testpt, pt, cnt);

   delete [] pt;

   return rslt;
}
// end of point_in_polygon

// ********************************************************************
// ********************************************************************

BOOL C_ovlkit_screen_polygon::point_in_polygon(CPoint point, POINT *pgon, int numverts)
{
   int c, i, j;
   int x, y;
   c = 0;

   x = point.x;
   y = point.y;
   for (i = 0, j = numverts - 1; i < numverts; j = i++)
   {
      if ((((pgon[i].y <= y) && (y < pgon[j].y)) || ((pgon[j].y <= y) && (y < pgon[i].y))) &&
         ((double)x < (double)(pgon[j].x - pgon[i].x) * (double)(y - pgon[i].y) /
         (double)(pgon[j].y - pgon[i].y) + (double)pgon[i].x))
         c = !c;
   }
   if (c == 0)
      return FALSE;
   else
      return TRUE;
}
// end of point_in_polygon

// ********************************************************************
// ********************************************************************

BOOL C_ovlkit_screen_polygon::point_on_polygon(CPoint testpt)
{
   POSITION next;
   C_ovlkit_small_vertex* vtx;
   int cnt, k;
   POINT *pt;
   BOOL rslt;
   CFvwUtil *futil = CFvwUtil::get_instance();

   if ( m_line_list.IsEmpty() )
      return FALSE;

   cnt = m_line_list.GetCount();
   pt = new POINT[cnt];
   k = 0;

   next = m_line_list.GetHeadPosition();
   while (next != NULL)
   {
      vtx = m_line_list.GetNext(next);
      pt[k].x = vtx->m_x;
      pt[k].y = vtx->m_y;
      k++;
   }

   rslt = futil->point_on_polygon(testpt, 2, pt, cnt);

   delete [] pt;

   return rslt;
}
// end of point_on_polygon
