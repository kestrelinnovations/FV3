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
#include "ovlelem.h"
#include "err.h"
#include "map.h"
#include "maps.h"
#include "fvwutil.h"
#include "graphics.h"
#include "ovl_mgr.h"

///////////////////////////////////////////////////////////////////////////////
// GeoBounds
///////////////////////////////////////////////////////////////////////////////

// Constructor
GeoBounds::GeoBounds()
{
   m_south = m_north = -90.0;
   m_west = m_east = -180.0;
   m_has_edit_focus = FALSE;
	m_no_error_check = false;
   delete_points2();
}

// Destructor
GeoBounds::~GeoBounds()
{ 
   delete_points2();
}

// set the geo-bounds via the southwest and northeast corners
int GeoBounds::set_bounds(d_geo_t sw, d_geo_t ne)
{
   return set_bounds(sw.lat, sw.lon, ne.lat, ne.lon);
}

// set the geo-bounds without doing any error checking
int GeoBounds::set_bounds_no_error_check(d_geo_t sw, d_geo_t ne)
{
   m_south = sw.lat;
   m_west = sw.lon;
   m_north = ne.lat;
   m_east = ne.lon;

	m_no_error_check = true;
   
   // cause redraw to call draw, since bounds have changed
   m_allow_redraw = FALSE;
   
   return SUCCESS;
}

// set the geo-bounds via the southern latitude, western longitude, northern
// latitude, and eastern longitude
int GeoBounds::set_bounds(degrees_t south, degrees_t west, 
                          degrees_t north, degrees_t east)
{
   if (!GEO_valid_degrees(south, west))
   {
      ERR_report("Invalid southwest.");
      return FAILURE;
   }
   
   if (!GEO_valid_degrees(north, east))
   {
      ERR_report("Invalid northeast.");
      return FAILURE;
   }
   
   if (north <= south)
   {
      ERR_report("Invalid bounds.");
      return FAILURE;
   }

   m_north = north;
   m_east = east;
   m_south = south;
   m_west = west;
   
   // cause redraw to call draw, since bounds have changed
   m_allow_redraw = FALSE;
   
   return SUCCESS;
}

// get the bounds as corner points
d_geo_t GeoBounds::get_sw()
{
   d_geo_t point;
   
   point.lat = m_south;
   point.lon = m_west;
   
   return point;
}

d_geo_t GeoBounds::get_nw()
{
   d_geo_t point;
   
   point.lat = m_north;
   point.lon = m_west;
   
   return point;
}

d_geo_t GeoBounds::get_ne()
{
   d_geo_t point;
   
   point.lat = m_north;
   point.lon = m_east;
   
   return point;
}

d_geo_t GeoBounds::get_se()
{
   d_geo_t point;
   
   point.lat = m_south;
   point.lon = m_east;
   
   return point;
}

// get the bounding latitudes and longitudes
void GeoBounds::get_bounds(degrees_t &south, degrees_t &west, 
                           degrees_t &north, degrees_t &east)
{
   north = m_north;
   east = m_east;
   south = m_south;
   west = m_west;
}

////////////////////////////////////////////////////////////////////////////
// Base Class Overides

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.  This is the only
int GeoBounds::prepare_for_redraw(MapProj* map)
{
	if (map == NULL)
		return FAILURE;

   d_geo_t ll, ur;            // virtual map's geo bounds
   
   // use the map bounds to clip
   if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
   {
      ERR_report("get_vmap_bounds");
      return FAILURE;
   }
   
   // delete all points in m_point_list
   delete_points();
   
   // delete all points in m_focus_box_list
   delete_points2();
   
   // don't allow redraw, unless this function completes successfully
   m_allow_redraw = FALSE;
   
   // if these bounds do not intersect with the map, there is nothing to draw
   if (GEO_intersect(ll.lat, ll.lon, ur.lat, ur.lon,
      m_south, m_west, m_north, m_east) == FALSE)
   {
      m_allow_redraw = TRUE;
      return SUCCESS;
   }
   
   // compute the region's width in degrees
   degrees_t geo_width = m_east - m_west;
   if (geo_width <= 0.0)
      geo_width += 360.0;
   
   // if the geo_width is more than 180 degrees, it will have to be broken in
   // half because of MAP_ assumes the shortest line, so we'll just split it
   // in half all the time
   degrees_t mid_lon = m_west + geo_width / 2.0;
   if (mid_lon > 180.0)
      mid_lon -= 360.0;
   
   // northern edge
   add_lat_points(map, m_north, m_west, mid_lon, m_point_list);
   add_lat_points(map, m_north, mid_lon, m_east, m_point_list);
   
   // eastern edge
   add_lon_points(map, m_east, m_north, m_south, m_point_list);
   
   // southern edge
   add_lat_points(map, m_south, m_east, mid_lon, m_point_list);
   add_lat_points(map, m_south, mid_lon, m_west, m_point_list);
   
   // western edge
   add_lon_points(map, m_west, m_south, m_north, m_point_list);
   
   // if at least one line was generated you must generate the points for the
   // edit focus rectangle
   if (m_point_list.IsEmpty() == FALSE)
   {
      // get the geo-bounds for a focus box drawn to the given MapProj
      if (prepare_for_redraw_focus_box(map, 10, 10, mid_lon) != SUCCESS)
      {
         ERR_report("prepare_for_redraw_focus_box() failed.");
         return FAILURE;
      }
   }
   
   m_allow_redraw = TRUE;
   
   return SUCCESS;
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t GeoBounds::hit_test(CPoint &point)
{
   // if edit focus on test the focus box
   if (has_edit_focus())
   {
      // the object must be valid and the focus box must have at least 1 point
      if (m_allow_redraw && m_focus_box_list.GetCount() > 1)
      {
         CFvwUtil *util = CFvwUtil::get_instance();
         CPoint *point1;
         CPoint *point2;
         POSITION position;
         
         // test the handles first, since they are drawn last
         if (get_focus_handle_at_point(point) != -1)
            return TRUE;
         
         // m_focus_box_list must contain an even number of points
         ASSERT(m_focus_box_list.GetCount() % 2 == 0);
         
         // check all lines in the bounds
         position = m_focus_box_list.GetHeadPosition();
         while (position)
         {
            point1 = m_focus_box_list.GetNext(position);
            point2 = m_focus_box_list.GetNext(position);
            if (util->distance_to_line(point1->x, point1->y, point2->x, point2->y,
               point.x, point.y) < 2)
               return TRUE;
         }
      }
   }
   
   // hit the bounds themselves
   if (OvlContour::hit_test(point))
      return TRUE;
   
   return FALSE;
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void GeoBounds::invalidate(boolean_t erase_background)
{
   // Invalidate the polyline.
   OvlContour::invalidate(erase_background);
   
   // Invalidate the focus box, if it exists
   if (has_edit_focus())
   {
      // the object must be valid and it must contain at least one segment
      if (m_allow_redraw && m_focus_box_list.GetCount() > 1)
      {
         CFvwUtil *util = CFvwUtil::get_instance();
         CPoint *point1;
         CPoint *point2;
         POSITION position;
         CRect rect;
         
         // m_point_list must contain an even number of points
         ASSERT(m_focus_box_list.GetCount() % 2 == 0);
         
         // invalidate lines in the focus box
         position = m_focus_box_list.GetHeadPosition();
         while (position)
         {
            point1 = m_focus_box_list.GetNext(position);
            point2 = m_focus_box_list.GetNext(position);
            rect = util->get_bounding_rect(point1->x, point1->y, 
               point2->x, point2->y, SOLID, 1);
            if (!rect.IsRectEmpty())
               OVL_get_overlay_manager()->invalidate_rect(rect, erase_background);
         }
         
         // invalidate handles
         int i;
         for (i=0; i<4; i++)
         {
            if (m_handles[i])
            {
               point1 = m_focus_box_list.GetAt(m_handles[i]);
               GRA_get_handle_rect(*point1, rect);
               OVL_get_overlay_manager()->invalidate_rect(rect, erase_background);
            }
         }         
      }
   }
}

// Returns a bounding rectangle on the overlay element.  The CRect
// will be empty if the object is off the screen.   
CRect GeoBounds::get_rect()
{
   // get the bounding rectangle on the lines
   CRect rect = OvlContour::get_rect();
   
   // if we are in the view and we have the edit focus we must expand the rect
   // to include the edit focus box and handles
   if (!rect.IsRectEmpty() && has_edit_focus())
   {
      // the object must be valid and it must contain at least one segment
      if (m_allow_redraw && m_focus_box_list.GetCount() > 1)
      {
         CFvwUtil *util = CFvwUtil::get_instance();
         CPoint *point1;
         CPoint *point2;
         POSITION position;
         
         // m_point_list must contain an even number of points
         ASSERT(m_focus_box_list.GetCount() % 2 == 0);
         
         // add lines in the focus box
         position = m_focus_box_list.GetHeadPosition();
         while (position)
         {
            point1 = m_focus_box_list.GetNext(position);
            point2 = m_focus_box_list.GetNext(position);
            rect |= util->get_bounding_rect(point1->x, point1->y, 
               point2->x, point2->y, UTIL_LINE_DASH2, 1);
         }
         
         // add CRect for handles
         int i;
         CRect handle_rect;
         for (i=0; i<4; i++)
         {
            if (m_handles[i])
            {
               point1 = m_focus_box_list.GetAt(m_handles[i]);
               GRA_get_handle_rect(*point1, handle_rect);
               rect != handle_rect;
            }
         }         
      }
   }
   
   return rect;
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int GeoBounds::view_draw(MapProj *map, CDC *dc)
{
   // Do the view draw for the polyline.
   if (OvlContour::view_draw(map, dc) != SUCCESS)
      return FAILURE;
   
   // Do the view draw for the edit focus box, if we have edit focus and we
   // are not printing.
   if (has_edit_focus() && !dc->IsPrinting())
   {
      // draw the focus box
      if (m_allow_redraw && m_focus_box_list.GetCount() > 1)
      {
         UtilDraw line(dc);
         
         // m_focus_box_list must contain at least 2 points
         ASSERT(m_focus_box_list.GetCount() > 1);
         // m_focus_box_list must contain an even number of points
         ASSERT(m_focus_box_list.GetCount() % 2 == 0);
         
         line.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_DASH2, 3, TRUE);
         line.DrawPolyLine(CLineSegmentList(m_focus_box_list));
         
         line.set_pen(UTIL_COLOR_BLUE, UTIL_LINE_DASH2, 1);
         line.DrawPolyLine(CLineSegmentList(m_focus_box_list)); 
         
         // draw the handles
         int i;
         CPoint *point;
         for (i=0; i <4; i++)
         {
            if (m_handles[i])
            {
               point = m_focus_box_list.GetAt(m_handles[i]);
               GRA_draw_handle(dc, *point, TRUE);
            }
         }
      }
   }
   
   return SUCCESS;
}

// returns TRUE if the class name is GeoBounds or a parent class
boolean_t GeoBounds::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "GeoBounds") == 0)
      return TRUE;
   
   return OvlContour::is_kind_of(class_name);
}

// delete all points in m_focus_box_list
void GeoBounds::delete_points2()
{
   while (m_focus_box_list.GetHeadPosition())
      delete m_focus_box_list.RemoveHead();
   
   // clear m_handles
   memset(m_handles, NULL, (size_t)(sizeof(int) * 4));
}

// adds points to the given list that would be required to draw the line of
// latitude on the given MapProj
int GeoBounds::add_lat_points(MapProj *map, degrees_t lat, degrees_t lon1, 
                              degrees_t lon2, CList <CPoint *, CPoint *> &list)
{
   // draw the next segment along the arc
   MAP_geo_line_points line_points;
   MAP_calc_geo_line(map, lat, lon1, lat, lon2, &line_points);
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
   
   return SUCCESS;
}

// adds points to the given list that would be required to draw the line of
// longitude on the given MapProj
int GeoBounds::add_lon_points(MapProj *map, degrees_t lon, degrees_t lat1, 
                              degrees_t lat2, CList <CPoint *, CPoint *> &list)
{
   // draw the next segment along the arc
   MAP_geo_line_points line_points;
   MAP_calc_geo_line(map, lat1, lon, lat2, lon, &line_points);
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
   
   return SUCCESS;
}

// prepares the focus box for drawing
int GeoBounds::prepare_for_redraw_focus_box(MapProj *map, int dx, int dy, 
                                            degrees_t mid_lon)
{
	if (map == NULL)
		return FAILURE;

   degrees_t south, west, north, east;
   int ul_x, ul_y, lr_x, lr_y;
   
   // get the vsurface coordinates for your geo-bounds
   map->geo_to_vsurface_rect(m_south, m_west, m_north, m_east, 
      &ul_x, &ul_y, &lr_x, &lr_y);
   
   // the focus box will be larger by (dx, dy)
   CRect rect(ul_x, ul_y, lr_x, lr_y);
   rect.InflateRect(dx, dy);
   
   // get the geo-coordinates of the focus box
   map->vsurface_to_geo_rect(rect.left, rect.top, rect.right, rect.bottom,
      &south, &west, &north, &east);
   
   // clip to avoid coordinates that are out of range, e.g., north of the
   // north pole or south of the south pole
   if (south < -89.999999)
      south = 89.999999;
   if (north > 89.999999)
      north = 89.999999;
   
   // northern edge
   add_lat_points(map, north, west, mid_lon, m_focus_box_list);
   add_lat_points(map, north, mid_lon, east, m_focus_box_list);
   
   // eastern edge
   add_lon_points(map, east, north, south, m_focus_box_list);
   
   // southern edge
   add_lat_points(map, south, east, mid_lon, m_focus_box_list);
   add_lat_points(map, south, mid_lon, west, m_focus_box_list);
   
   // western edge
   add_lon_points(map, west, south, north, m_focus_box_list);
   
   // set indices for the handles
   int x, y;
   
   if (map->geo_in_surface(north, west, &x, &y))
      m_handles[NW_INDEX] = get_focus_box_point_position(x, y);
   else
      m_handles[NW_INDEX] = NULL;
   
   if (map->geo_in_surface(north, east, &x, &y))
      m_handles[NE_INDEX] = get_focus_box_point_position(x, y);
   else
      m_handles[NE_INDEX] = NULL;
   
   if (map->geo_in_surface(south, east, &x, &y))
      m_handles[SE_INDEX] = get_focus_box_point_position(x, y);
   else
      m_handles[SE_INDEX] = NULL;
   
   if (map->geo_in_surface(south, west, &x, &y))
      m_handles[SW_INDEX] = get_focus_box_point_position(x, y);
   else
      m_handles[SW_INDEX] = NULL;
   
   return SUCCESS;
}

// Returns the position of the point in m_focus_box_list with the given (x, y)
// values.  Returns NULL for no match found.
POSITION GeoBounds::get_focus_box_point_position(int x, int y)
{
   CPoint *point;
   POSITION position;
   POSITION next = m_focus_box_list.GetHeadPosition();
   while (next)
   {
      // save the position of the point you are about to get
      position = next;
      
      // get this point and the position of the next point
      point = m_focus_box_list.GetNext(next);
      
      if (point->x == x && point->y == y)
         return position;
   }
   
   return NULL;
}

// Returns the index in m_handles of the handle hit by point.  Returns -1
// if none of the points are hit.
int GeoBounds::get_focus_handle_at_point(CPoint point)
{
   int i;
   CRect rect;
   CPoint *handle;
   
   for (i=0; i<4; i++)
   {
      if (m_handles[i])
      {
         handle = m_focus_box_list.GetAt(m_handles[i]);
         GRA_get_handle_rect(*handle, rect);
         if (rect.PtInRect(point))
            return i;
      }
   }
   
   return -1;
}

///////////////////////////////////////////////////////////////////////////////
// GeoBoundsDragger
///////////////////////////////////////////////////////////////////////////////

// Constructor
GeoBoundsDragger::GeoBoundsDragger() : GeoBounds()
{
   m_nesw = 0;
   m_previous.lat = -90.0;
   m_previous.lon = -180.0;
}

// Base class over-ride forces XOR draws
int GeoBoundsDragger::view_draw(MapProj* map, CDC* dc)
{
   // use XOR lines
   int old_rop = dc->SetROP2(R2_XORPEN);
   
   // call OvlContour::view_draw instead of GeoBounds::view_draw to insure that
   // the edit focus box will never be drawn as part of a GeoBoundsDragger
   int status = OvlContour::view_draw(map, dc);
   
   // restore DC
   dc->SetROP2(old_rop);
   
   return status;
}

// Copies the state of the given GeoBounds object into this dragger.
// Call this function once before each call to on_selected.
void GeoBoundsDragger::copy(GeoBounds &bounds)
{
   // there should be no drag in progress
   ASSERT(m_nesw == 0);
   
   // copy the bounds from the given object
   set_bounds(bounds.get_sw(), bounds.get_ne());
   
   // you'll need to have edit focus in on_selected if the object you are
   // dragging has edit focus
   set_edit_focus(bounds.has_edit_focus());
   
   // copy the line width, but not the style or color
   int color, style, width;
   
   // if the background pen is on - use it's width
   if (bounds.get_pen().get_background_pen(color, style, width))
      get_pen().set_foreground_pen(BRIGHT_WHITE, SOLID, width);
   // otherwise use the foreground pen's width
   else
   {
      bounds.get_pen().get_foreground_pen(color, style, width);
      get_pen().set_foreground_pen(BRIGHT_WHITE, SOLID, width);
   }
}

// Returns TRUE if the object was hit, and sets drag to be TRUE if the
// object has been placed in a drag state.
boolean_t GeoBoundsDragger::on_selected(ViewMapProj *view, CPoint point, UINT flags, 
                                        boolean_t *drag, HCURSOR *cursor)
{
   // there should be no drag in progress
   ASSERT(m_nesw == 0);
   
   // setup internal state so hit_test works
   prepare_for_redraw(view);
   
   // initialize m_previous
   view->surface_to_geo(point.x, point.y, &m_previous.lat, &m_previous.lon);
   
   // it the point does not hit the bounds or its selection box, do nothing
   if (!hit_test(point))
      return FALSE;
   
   // now that the hit-test has been done, make sure the edit focus is not set
   // so the edit focus box does not get drawn during the drag operation
   set_edit_focus(FALSE);
   
   // if the point hits one of the handles, we start a corner drag
   int index = get_focus_handle_at_point(point);
   if (index != -1)
   {
      switch (index)
      {
      case NW_INDEX:
         m_nesw = UTL_IDC_SIZE_NW;
         break;
         
      case NE_INDEX:
         m_nesw = UTL_IDC_SIZE_NE;
         break;
         
      case SE_INDEX:
         m_nesw = UTL_IDC_SIZE_SE;
         break;
         
      case SW_INDEX:
         m_nesw = UTL_IDC_SIZE_SW;
         break;
      }
   }
   // if the point hits the bounds themselves, we will start a move
   else if (OvlContour::hit_test(point))
   {
      m_nesw = UTL_IDC_SIZE_NSEW;
   }
   else
   {
      m_nesw = get_edge_at_point(view, point);
      if (m_nesw == 0)
         return FALSE;
   }
   
   *cursor = UTL_get_IDC_SIZE_cursor(m_nesw, 360.0 - view->actual_rotation());
   *drag = TRUE;
   
   // if you don't draw it here you will be left in the wrong state
   CClientDC dc(view->get_CView());
   prepare_for_redraw(view);
   draw(view, &dc);

   return TRUE;
}

// This function handles the drag operation.  The display will be updated 
// to show the drag operation.
void GeoBoundsDragger::on_drag(MapProj *map, CDC *pDC, CPoint point, UINT flags, 
                               HCURSOR *cursor, HintText &hint)
{
   // not in a drag state
   ASSERT(m_nesw != 0);
   
   // clip point to the view window
   int width, height;
   map->get_surface_size(&width, &height);
   if (point.x < 0)
      point.x = 0;
   else if (point.x >= width)
      point.x = width - 1;
   if (point.y < 0)
      point.y = 0;
   else if (point.y >= height)
      point.y = height - 1;
   
   // compute the lat-lon at the cursor
   degrees_t lat, lon;
   map->surface_to_geo(point.x, point.y, &lat, &lon);
   
   // compute the cursor movement in latitude and longitude
   degrees_t delta_lat = lat - m_previous.lat;
   degrees_t delta_lon = lon - m_previous.lon;
   if (delta_lon < -180.0)
      delta_lon = 360.0 + delta_lon;
   else if (delta_lon > 180.0)
      delta_lon = 360.0 - delta_lon;
   
   // erase the previous box
   if (m_allow_redraw)
      view_draw(map, pDC);

   // apply the change in size, shape, or position
   apply_drag(delta_lat, delta_lon);

   // draw the box in its new size, shape, or position
   draw(map, pDC);
      
   // set the cursor
   *cursor = UTL_get_IDC_SIZE_cursor(m_nesw, 360.0 - map->actual_rotation());
   
   // clear the hint - we are not using it
   hint.set_help_text("");
   hint.set_tool_tip("");
   
   // update m_previous for the next call
   m_previous.lat = lat;
   m_previous.lon = lon;
}

// Called when a drag operation is completed.  The drag box will be removed.
// The given point will be used to update the internal state of the object.
// The object will reset to a non-drag state.
void GeoBoundsDragger::on_drop(MapProj *map, CDC *pDC, CPoint point, UINT flags)
{
   // not in a drag state
   ASSERT(m_nesw != 0);
   
   // clip point to the view window
   int width, height;
   map->get_surface_size(&width, &height);
   if (point.x < 0)
      point.x = 0;
   else if (point.x >= width)
      point.x = width - 1;
   if (point.y < 0)
      point.y = 0;
   else if (point.y >= height)
      point.y = height - 1;
   
   // compute the lat-lon at the cursor
   degrees_t lat, lon;
   map->surface_to_geo(point.x, point.y, &lat, &lon);
   
   // compute the cursor movement in latitude and longitude
   degrees_t delta_lat = lat - m_previous.lat;
   degrees_t delta_lon = lon - m_previous.lon;
   if (delta_lon < -180.0)
      delta_lon = 360.0 + delta_lon;
   else if (delta_lon > 180.0)
      delta_lon = 360.0 - delta_lon;
   
   // erase the box
   if (m_allow_redraw)
      view_draw(map, pDC);

   // apply the change in size, shape, or position
   apply_drag(delta_lat, delta_lon);
   
   // go to the no drag state
   reset();
}

// Called when a drag operation is aborted.  The drag box will be removed
// and the state of the object will be reset.  The object will reset to a
// non-drag state.
void GeoBoundsDragger::on_cancel_drag(MapProj *map, CDC *pDC)
{
   // not in a drag state
   ASSERT(m_nesw != 0);
   
   // erase the box
   if (m_allow_redraw)
      view_draw(map, pDC);
   
   // go to the no drag state
   reset();
}

// Called to see of a call to on_selected would start a drag operation.
boolean_t GeoBoundsDragger::on_test_selected(MapProj *view, CPoint point, 
                                             UINT flags, HCURSOR *cursor, HintText &hint)
{
   int nesw;
   
   // there should be no drag in progress
   ASSERT(m_nesw == 0);
   
   // setup internal state so hit_test works
   prepare_for_redraw(view);
   
   // it the point does not hit the bounds or its selection box, do nothing
   if (!hit_test(point))
      return FALSE;
   
   // if the point hits one of the handles, we would start a corner drag
   int index = get_focus_handle_at_point(point);
   if (index != -1)
   {
      switch (index)
      {
      case NW_INDEX:
         nesw = UTL_IDC_SIZE_NW;
         hint.set_help_text("Resize North-West");
         hint.set_tool_tip("Resize North-West");
         break;
         
      case NE_INDEX:
         nesw = UTL_IDC_SIZE_NE;
         hint.set_help_text("Resize North-East");
         hint.set_tool_tip("Resize North-East");
         break;
         
      case SE_INDEX:
         nesw = UTL_IDC_SIZE_SE;
         hint.set_help_text("Resize South-East");
         hint.set_tool_tip("Resize South-East");
         break;
         
      case SW_INDEX:
         nesw = UTL_IDC_SIZE_SW;
         hint.set_help_text("Resize South-West");
         hint.set_tool_tip("Resize South-West");
         break;
      }
   }
   // if the point hits the bounds themselves, we would  start a move
   else if (OvlContour::hit_test(point))
   {
      nesw = UTL_IDC_SIZE_NSEW;
      hint.set_help_text("Move");
      hint.set_tool_tip("Move");
   }
   // test for an edge drag
   else
   {
      nesw = get_edge_at_point(view, point);
      if (nesw == 0)
         return FALSE;
      
      switch (nesw)
      {
      case UTL_IDC_SIZE_N:
         hint.set_help_text("Resize North");
         hint.set_tool_tip("Resize North");
         break;
         
      case UTL_IDC_SIZE_S:
         hint.set_help_text("Resize South");
         hint.set_tool_tip("Resize South");            
         break;
         
      case UTL_IDC_SIZE_E:
         hint.set_help_text("Resize East");
         hint.set_tool_tip("Resize East");
         break;
         
      case UTL_IDC_SIZE_W:
         hint.set_help_text("Resize West");
         hint.set_tool_tip("Resize West");
         break;
      }
   }
   
   *cursor = UTL_get_IDC_SIZE_cursor(nesw, 360.0 - view->actual_rotation());
   
   return TRUE;
}

// Resets the object to a non-drag state.
void GeoBoundsDragger::reset()
{
   m_nesw = 0;
   m_previous.lat = -90.0;
   m_previous.lon = -180.0;
}

// Adjust the shape or position of the object based on the drag state, 
// delta-lat, and delta-lon.
void GeoBoundsDragger::apply_drag(degrees_t delta_lat, degrees_t delta_lon)
{
   degrees_t south, west, north, east;
   
   // get current bounds
   get_bounds(south, west, north, east);
   
   // northern edge
   if (m_nesw & UTL_IDC_SIZE_N)
   {
      north += delta_lat;
      if (north > 90.0)
      {
         north = 90.0;
         TRACE("North > 90.0\n");
      }
      else if (north < m_south)
      {
			if (m_no_error_check)
			{
				north = m_south;
				south = north;
				m_nesw &= ~UTL_IDC_SIZE_N;
				m_nesw |= UTL_IDC_SIZE_S;
			}
			else
			{
				north = m_south + 1.0e-07;            // ~0.43776 inches
				TRACE("North < South\n");
			}
      }
   }
   
   // southern edge
   if (m_nesw & UTL_IDC_SIZE_S)
   {
      south += delta_lat;
      if (south < -90.0)
      {
         south = -90.0;
         TRACE("South < -90.0\n");
      }
      else if (south > m_north)
      {
			if (m_no_error_check)
			{
				north = south;
				south = m_north;
				m_nesw &= ~UTL_IDC_SIZE_S;
				m_nesw |= UTL_IDC_SIZE_N;
			}
			else
			{
				south = m_north - 1.0e-07;            // ~0.43776 inches
				TRACE("South > North\n");
			}
      }
   }
   
   // eastern edge
   if (m_nesw & UTL_IDC_SIZE_E)
   {
      east += delta_lon;
      if (east > 180.0)
         east -= 360.0;
      else if (east < -180.0)
         east += 360.0;
      
      // if the western edge is not also being changed by delta_lon, we must
      // insure that we do not swap the eastern and western edges
      if ((m_nesw & UTL_IDC_SIZE_W) == 0)
      {
         // If you move the eastern edge to the east, the current eastern egde
         // should be between western edge and the new eastern edge.
         if (delta_lon > 0.0 && !GEO_lon_in_range(m_west, east, m_east))
         {
				if (m_no_error_check)
				{
					east = m_west;
					west = east;
					m_nesw &= ~UTL_IDC_SIZE_E;
					m_nesw |= UTL_IDC_SIZE_W;
				}
				else
				{
					// do not allow the edges to cross
					east = m_west - 1.0e-07;            // ~0.43776 inches
					if (east < -180.0)
						east += 360.0;            
				}
         }
         
         // If you move the eastern edge to the west, the new eastern edge
         // should be between the western edge and the current eastern edge.
         if (delta_lon < 0.0 && !GEO_lon_in_range(m_west, m_east, east))
         {
				if (m_no_error_check)
				{
					east = m_west;
					west = east;
					m_nesw &= ~UTL_IDC_SIZE_E;
					m_nesw |= UTL_IDC_SIZE_W;
				}
				else
				{
					// do not allow the eges to cross
					east = m_west + 1.0e-07;            // ~0.43776 inches
					if (east > 180.0)
						east -= 360.0;
				}
         }
      }
   }
   
   if (m_nesw & UTL_IDC_SIZE_W)
   {
      west += delta_lon;
      if (west > 180.0)
         west -= 360.0;
      else if (west < -180.0)
         west += 360.0;
      
      // if the eastern edge is not also being changed by delta_lon, we must
      // insure that we do not swap the eastern and western edges
      if ((m_nesw & UTL_IDC_SIZE_E) == 0)
      {
         // If you move the western edge to the west, the current western egde
         // should be between new western edge and the eastern edge.
         if (delta_lon < 0.0 && !GEO_lon_in_range(west, m_east, m_west))
         {
				if (m_no_error_check)
				{
					west = m_east;
					east = west;
					m_nesw &= ~UTL_IDC_SIZE_W;
					m_nesw |= UTL_IDC_SIZE_E;
				}
				else
				{
					// do not allow the edges to cross
					west = m_east + 1.0e-07;            // ~0.43776 inches
					if (west > 180.0)
						west -= 360.0;             
				}
         }
         
         // If you move the western edge to the east, the new western edge
         // should be between the current western edge and the eastern edge.
         if (delta_lon > 0.0 && !GEO_lon_in_range(m_west, m_east, west))
         {
				if (m_no_error_check)
				{
					west = m_east;
					east = west;
					m_nesw &= ~UTL_IDC_SIZE_W;
					m_nesw |= UTL_IDC_SIZE_E;
				}
				else
				{
					// do not allow the eges to cross
					west = m_east - 1.0e-07;            // ~0.43776 inches
					if (west < -180.0)
						west += 360.0; 
				}
         }
      }
   }

	// insure that north != south and east != west
	if (north == south)
		south -= 1.0e-07;
	if (east == west)
		east += 1.0e-07;
   
   set_bounds(south, west, north, east);
}

// If the point hits one of the edges of the focus box this funtion will return
// UTL_IDC_SIZE_N, UTL_IDC_SIZE_S, UTL_IDC_SIZE_E, or UTL_IDC_SIZE_W.  If no 
// edge is hit, 0 will be returned.
int GeoBoundsDragger::get_edge_at_point(MapProj *map, CPoint point)
{
   // the object must be valid and the focus box must have at least 1 point
   if (m_allow_redraw && m_focus_box_list.GetCount() > 1)
   {
      CFvwUtil *util = CFvwUtil::get_instance();
      CPoint *point1;
      CPoint *point2;
      POSITION position;
      
      // m_focus_box_list must contain an even number of points
      ASSERT(m_focus_box_list.GetCount() % 2 == 0);
      
      // find the line that this point hits
      position = m_focus_box_list.GetHeadPosition();
      while (position)
      {
         point1 = m_focus_box_list.GetNext(position);
         point2 = m_focus_box_list.GetNext(position);
         if (util->distance_to_line(point1->x, point1->y, point2->x, point2->y,
            point.x, point.y) < 2)
         {
            d_geo_t p1, p2;
            
            // get the geographic coordinates of the line that was hit
            map->surface_to_geo(point1->x, point1->y, &p1.lat, &p1.lon);
            map->surface_to_geo(point2->x, point2->y, &p2.lat, &p2.lon);
            
            // When the map is rotated geo_to_surface followed by 
            // surface_to_geo on two lat-lons with equal latitudes, or equal
            // longitudes, may not yield lat-lons with exactly equal latitudes
            // or longitudes.  In a perfect world a line of latitude would be
            // defined by (p1.lat == p2.lat && p1.lon != p2.lon) and a line of
            // longitude would be defined by (p1.lon == p2.lon && p1.lat != 
            // p2.lat).  To catch the exact case as well as near misses the
            // delta-lat/delta-lon comparison is used.
            
            // if a line of latitude was hit
            //if (p1.lat == p2.lat && p1.lon != p2.lon)
            if (GEO_delta_lat(p1.lat, p2.lat) < GEO_delta_lon(p1.lon, p2.lon))
            {
               degrees_t d_north, d_south;
               degrees_t lat, lon;
               
               // get the lat-lon at the cursor
               map->surface_to_geo(point.x, point.y, &lat, &lon);
               
               // compute the "distance" from the cursor to each edge
               d_north = GEO_delta_lat(m_north, lat);
               d_south = GEO_delta_lat(m_south, lat);
               
               if (d_north < d_south)
                  return UTL_IDC_SIZE_N;
               else
                  return UTL_IDC_SIZE_S;
            }
            // if a line of longitude was hit
            else //if (p1.lon == p2.lon && p1.lat != p2.lat)
            {
               degrees_t d_east, d_west;
               degrees_t lat, lon;
               
               // get the lat-lon at the cursor
               map->surface_to_geo(point.x, point.y, &lat, &lon);
               
               // compute the "distance" from the cursor to each edge
               d_east = GEO_delta_lon(m_east, lon);
               d_west = GEO_delta_lon(m_west, lon);
               
               if (d_east < d_west)
                  return UTL_IDC_SIZE_E;
               else
                  return UTL_IDC_SIZE_W;
            }
         }
      }
   }
   
   return 0;
}

// Constructor
GeoCircleDragger::GeoCircleDragger()
{
   
}

// Base class over-ride forces XOR draws
int GeoCircleDragger::view_draw(MapProj* map, CDC* dc)
{
   // use XOR lines
   int old_rop = dc->SetROP2(R2_XORPEN);
   
   // call OvlContour::view_draw instead of GeoBounds::view_draw to insure that
   // the edit focus box will never be drawn as part of a GeoCircleDragger
   int status = OvlContour::view_draw(map, dc);

   // draw a crosshair at the center of the geocircle.
   if (!m_first_time)
      draw_crosshair(map, dc);
   m_first_time = FALSE;
   
   // restore DC
   dc->SetROP2(old_rop);
   
   return status;
}

// This function handles the drag operation.  The display will be updated 
// to show the drag operation.
void GeoCircleDragger::on_drag(ViewMapProj *view, CPoint point, UINT flags, 
                               HCURSOR* pCursor, HintText &hint)
{
   // clip point to the view window
   int width, height;
   view->get_surface_size(&width, &height);
   if (point.x < 0)
      point.x = 0;
   else if (point.x >= width)
      point.x = width - 1;
   if (point.y < 0)
      point.y = 0;
   else if (point.y >= height)
      point.y = height - 1;
   
   // compute the lat-lon at the cursor
   d_geo_t geo;
   view->surface_to_geo(point.x, point.y, &geo.lat, &geo.lon);
   
   // Get a DC
   CClientDC dc(view->get_CView());

   // erase the previous box and crosshair
   if (m_allow_redraw)
   {
      view_draw(view, &dc);
   }

   set_radius_from_geo(geo);

   // draw the box in its new size, shape, or position along with the crosshair
      draw(view, &dc);
   
   // clear the hint - we are not using it
   hint.set_help_text("");
   hint.set_tool_tip("");   
}

// Called when a drag operation is completed.  The drag box will be removed.
// The given point will be used to update the internal state of the object.
// The object will reset to a non-drag state.
void GeoCircleDragger::on_drop(ViewMapProj *view, CPoint point, UINT flags)
{
   // clip point to the view window
   int width, height;
   view->get_surface_size(&width, &height);
   if (point.x < 0)
      point.x = 0;
   else if (point.x >= width)
      point.x = width - 1;
   if (point.y < 0)
      point.y = 0;
   else if (point.y >= height)
      point.y = height - 1;
   
   // compute the lat-lon at the cursor
   d_geo_t geo;
   view->surface_to_geo(point.x, point.y, &geo.lat, &geo.lon);
   
   // Get a DC
   CClientDC dc(view->get_CView());

   // erase the box and the crosshair
   if (m_allow_redraw)
   {
      view_draw(view, &dc);
   }

   // apply the change in size, shape, or position
   set_radius_from_geo(geo);
}

// Called when a drag operation is aborted.  The drag box will be removed
// and the state of the object will be reset.  The object will reset to a
// non-drag state.
void GeoCircleDragger::on_cancel_drag(ViewMapProj *view)
{
   // Get a DC
   CClientDC dc(view->get_CView());

   // erase the box and the crosshair
   if (m_allow_redraw)
   {
      view_draw(view, &dc);
   }
}

void GeoCircleDragger::draw_crosshair(MapProj *map, CDC *dc)
{
   // get the center of the geocircle
   int center_x, center_y;
   map->geo_to_surface(get_lat(), get_lon(), &center_x, &center_y);

   // draw the crosshairs at the center of the geocircle
   UtilDraw util(dc);
   util.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, 2);
   util.draw_line(center_x + 2, center_y, center_x + 7, center_y);
   util.draw_line(center_x - 2, center_y, center_x - 7, center_y);
   util.draw_line(center_x, center_y + 2, center_x, center_y + 7);   
   util.draw_line(center_x, center_y - 2, center_x, center_y - 7);
}
