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



// OvlRgn.cpp

// OvlRgn represent a simple or complex region described in terms of 
// geographical coordinates and a list of set operations (union, intersection,
// and subtraction)

#include "StdAfx.h"
#include "ovlelem.h"
#include "mapx.h"       // for MapProj

// constructor
OvlRgn::OvlRgn()
{
   m_in_view = FALSE;
}


// destructor
OvlRgn::~OvlRgn()
{
   POSITION position = m_op_list.GetHeadPosition();
   while (position)
   {
      ovlrgn_op_t *op = m_op_list.GetNext(position);
      delete [] op->geo;
      delete op;
   }
}

// Draw the element from scratch.  This function will get called if there
// is any change to the underlying map, i.e., scale, center, rotation, etc.
// This function will get called if there is any change to the underlying
// data this overlay element represents.  This function must be able to
// determine if the overlay element is in the current view and draw it, if
// it is in the view.  Part of this process is making the necessary
// preparations for the redraw(), hit_test(), invalidate(), and get_rect()
// members, as all of these functions depend on the action taken by the
// last call to the draw() function.
int OvlRgn::draw(MapProj* map, CDC* dc)
{
   prepare_for_redraw(map);

   if (m_in_view)
	   view_draw(map, dc);

   m_allow_redraw = TRUE;

   return SUCCESS;
}

// Redraw the element exactly as it was drawn by the last call to draw().
int OvlRgn::redraw(MapProj* map, CDC* dc)
{
   if (m_in_view)
      return view_draw(map, dc);
   return SUCCESS;
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t OvlRgn::hit_test(CPoint &point)
{
   if (m_in_view)
      return m_rect.PtInRect(point);
   return FALSE;
}

// Returns a bounding rectangle on the overlay element.  The CRect
// will be empty if the object is off the screen.   
CRect OvlRgn::get_rect()
{
   if (!m_in_view)
      return CRect(0,0,0,0);

   return m_rect;
}

void OvlRgn::get_screen_coordinates(MapProj *map, ovlrgn_op_t *op, 
                                    std::vector<float>& point_lst,
                                    std::vector<float>& wrapped_point_lst)
{
   // compute the western-most longitude that will be used to handle wrapping
   // around the world
   double west_most_lon = op->geo[0].lon;
   for (int i=1; i<op->count; ++i)
   {
      if (GEO_east_of_degrees(op->geo[i].lon, west_most_lon) == FALSE)
         west_most_lon = op->geo[i].lon;
   }
   // compute the left-most x-coordinate from the west most longitude
   int left_most_x_coord, unused_y;
   map->geo_to_vsurface(0, west_most_lon, &left_most_x_coord, &unused_y);

   // get the pixel around the world which is used to wrap coordinates around the world
   int pixels_around_world;
   map->get_pixels_around_world(&pixels_around_world);

   const double cos_ang = cos(DEG_TO_RAD(map->actual_rotation()));
   const double sin_ang = sin(DEG_TO_RAD(map->actual_rotation()));

   int left = INT_MAX, right = -INT_MAX, top = INT_MAX, bottom = -INT_MAX;
   BOOL wrapped = FALSE;
   for(int i=0;i<op->count;i++)
   {
      int vx, vy;
      int x, y;
      map->geo_to_vsurface(op->geo[i].lat, op->geo[i].lon, &vx, &vy);
      map->geo_to_surface(op->geo[i].lat, op->geo[i].lon, &x, &y);

      // if the virtual x-coord is smaller then the lower left longitude, then
      // it wrapped around the world.  Add pixels_around_world back to the
      // coordinates to get a non-wrapped coordinate.  Otherwise, we leave the
      // coordinates alone
      if (vx < left_most_x_coord)
      {
         x += static_cast<int>(cos_ang * pixels_around_world);
         y += static_cast<int>(sin_ang * pixels_around_world);
         wrapped = TRUE;
      }

      if (x < left)
         left = x;
      if (x > right)
         right = x;
      if (y < top)
         top = y;
      if (y > bottom)
         bottom = y;

      point_lst.push_back(static_cast<float>(x));
      point_lst.push_back(static_cast<float>(y));
   }
   m_rect |= CRect(left, top, right, bottom);

   if (wrapped)
   {
      const size_t size = point_lst.size();
      for (size_t i=0; i<size; i += 2)
      {
         float wx = static_cast<float>(point_lst[i] - cos_ang * pixels_around_world);
         float wy = static_cast<float>(point_lst[i+1] - sin_ang * pixels_around_world);
         wrapped_point_lst.push_back(wx);
         wrapped_point_lst.push_back(wy);
      }
   }
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlRgn::prepare_for_redraw(MapProj* map)
{
   if (map == NULL)
		return FAILURE;

   // If this list is empty, there is nothing to draw
   if (m_op_list.GetCount() == 0)
   {
		m_in_view = FALSE;

      return SUCCESS;
   }

   m_sa_op_type_count.Delete();
   m_sa_screen_coordinates.Delete();
   m_rect.SetRectEmpty();

   POSITION position = m_op_list.GetHeadPosition();
   while (position)
   {
      ovlrgn_op_t* op = m_op_list.GetNext(position);

      std::vector<float> point_list, wrapped_point_list;
      get_screen_coordinates(map, op, point_list, wrapped_point_list);

      AppendOperation(op, point_list);
      
      if (wrapped_point_list.size() > 0)
      {
         ovlrgn_op_t wrapped_op = *op;
         wrapped_op.count = wrapped_point_list.size() / 2;
         AppendOperation(&wrapped_op, wrapped_point_list);
      }
   }

   // calculate the rect of the screen view coordinates
   CRect view_rect(0,0, map->get_surface_width(), 
      map->get_surface_height());

   // does the region lie inside the view?
   m_in_view = m_rect.IntersectRect(&m_rect, &view_rect);

   return SUCCESS;
}

// Returns TRUE if the last call to draw() determined that the object was
// within the current view.  Note this really means that the last call to
// prepare_for_redraw() determined that the object was within the current
// view.
boolean_t OvlRgn::in_view()
{
   return m_in_view;
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int OvlRgn::view_draw(MapProj* map, CDC* dc)
{
	ASSERT(0);
   return FAILURE;
}

int OvlRgn::view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   if (m_brush == NULL)
   {
      COLORREF color;
		int style, width;
		m_pen.get_foreground_pen(color, style, width);

      m_brush = gc->create_brush(color, (BrushStyleEnum)m_fill_type);
   }

   gc->draw_region(&m_sa_op_type_count, &m_sa_screen_coordinates, m_pen.get_fv_pen(gc, m_draw_mode),
      m_brush);

   return SUCCESS;
}

// returns TRUE if the class name is OvlElement
boolean_t OvlRgn::is_kind_of(const char *class_name)
{
   if (CString(class_name) == "OvlRgn")
      return TRUE;
   
   return OvlElement::is_kind_of(class_name);
}

void OvlRgn::get_bounding_coordinates(COleSafeArray& points, int count, d_geo_t* ul, d_geo_t* lr)
{
   long rgIndices[2];

   // get upper-left
   rgIndices[0] = 0;
   rgIndices[1] = 0;
   points.GetElement(rgIndices, &ul->lat);

   rgIndices[1] = 1;
   points.GetElement(rgIndices, &ul->lon);

   // get lower-right
   rgIndices[0] = 1;
   rgIndices[1] = 0;
   points.GetElement(rgIndices, &lr->lat);

   rgIndices[1] = 1;
   points.GetElement(rgIndices, &lr->lon);
}

void OvlRgn::add_operation(int type, COleSafeArray& points, int count,
      int operation)
{
   ovlrgn_op_t *op = new ovlrgn_op_t;

   op->operation = operation;

   if (type == 0 /* rectangle */)
   {
      // add all four coordinates of rectangle to handle rotation
      d_geo_t upper_left, lower_right;
      get_bounding_coordinates(points, count, &upper_left, &lower_right);

      op->geo = new d_geo_t[4];
      op->geo[0] = upper_left;
      op->geo[1].lat = upper_left.lat;
      op->geo[1].lon = lower_right.lon;
      op->geo[2] = lower_right;
      op->geo[3].lat = lower_right.lat;
      op->geo[3].lon = upper_left.lon;

      op->type = 2 /* polygon */;
      op->count = 4;
   }
   else if (type == 1 /* ellipse */)
   {
      d_geo_t upper_left, lower_right;
      get_bounding_coordinates(points, count, &upper_left, &lower_right);

      // compute the geographic coordinates of the ellipse
      d_geo_t center = { (upper_left.lat + lower_right.lat) / 2.0, (upper_left.lon + lower_right.lon) / 2.0 };

      double horz_km, vert_km;
      degrees_t unused_angle;
      GEO_distance(center.lat, center.lon, center.lat, upper_left.lon, &horz_km, &unused_angle);
      GEO_distance(center.lat, center.lon, upper_left.lat, center.lon, &vert_km, &unused_angle);

      const int NUM_ELLIPSE_POINTS = 80;
      op->geo = new d_geo_t[NUM_ELLIPSE_POINTS];
      op->type = 2 /* polygon */;
      op->count = NUM_ELLIPSE_POINTS;

      CGeoEllipsePoints geoEllipse(center.lat, center.lon, horz_km * 1000.0, vert_km * 1000.0, 0.0, 
         NUM_ELLIPSE_POINTS);
      int crnt_index = 0;
      geoEllipse.MoveFirst();
      while (crnt_index < NUM_ELLIPSE_POINTS && geoEllipse.GetNextPoint(op->geo[crnt_index].lat, op->geo[crnt_index].lon))
         ++crnt_index;
   }
   else
   {
      op->type = type;
      op->geo = new d_geo_t[count];
      op->count = count;
      for (int i = 0; i < count; i++)
      {
         d_geo_t geo;
         long rgIndices[2];

         rgIndices[0] = i;

         rgIndices[1] = 0;
         points.GetElement(rgIndices, &geo.lat);

         rgIndices[1] = 1;
         points.GetElement(rgIndices, &geo.lon);

         op->geo[i] = geo;
      }
   }

   m_op_list.AddTail(op);
}

void OvlRgn::AppendOperation(ovlrgn_op_t* op, std::vector<float>& point_list)
{
   long op_type_and_count = op->count;
   switch (op->operation)
   {
   case 0: op_type_and_count |= REGION_OPERATION_AND; break;
   case 1: op_type_and_count |= REGION_OPERATION_DIFF; break;
   case 2: op_type_and_count |= REGION_OPERATION_OR; break;
   case 3: op_type_and_count |= REGION_OPERATION_XOR; break;
   }

   switch (op->type)
   {
   case 0: op_type_and_count |= REGION_TYPE_RECTANGLE; break;
   case 1: op_type_and_count |= REGION_TYPE_ELLIPSE; break;
   case 2: op_type_and_count |= REGION_TYPE_POLYGON; break;
   }

   m_sa_op_type_count.Append(op_type_and_count);
   m_sa_screen_coordinates.Append(&point_list[0], point_list.size());
}

// set up the brush used for the region
void OvlRgn::set_brush(OvlPen &pen, int fill_type)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	COLORREF fill_color;
	int unused_style, unused_width;
	pen.get_foreground_pen(fill_color, unused_style, unused_width);
	m_pen = pen;
	m_fill_type = fill_type;
}
