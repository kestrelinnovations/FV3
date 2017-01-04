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



// OvlCont.cpp

//*****************************************************************************
// OvlContour
//
// This abstract base class provides an implementations all OvlElement member
// functions except for prepare_for_redraw().  This class is for geographic
// objects which can be represented as a contour on the map.  Derived
// classes must implement a prepare_for_redraw() that generates a list of
// CPoint pairs for the part of the contour that is in the current view.  Note
// these contours do not have anything to do with elevation values.
//*****************************************************************************

#include "stdafx.h"

#include "FvCoreMFC\Include\fvwutil.h"

#include <mapx.h>
#include "ovlelem.h"
#include "err.h"
#include "ovl_mgr.h"

// Constructor
OvlContour::OvlContour() : OvlElement(),
   m_pen(m_allow_redraw),
   m_font(m_allow_redraw),
   m_drawn_simple(false)
{
   m_pen.set_background_pen(BLACK);
}

// Destructor
OvlContour::~OvlContour()
{
   delete_points();
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t OvlContour::hit_test(CPoint &point)
{
   // the object must be valid and it must contain at least one segment
   if (m_allow_redraw && m_point_list.GetCount() > 1)
   {
      CFvwUtil *util = CFvwUtil::get_instance();
      CPoint *point1;
      CPoint *point2;
      POSITION position;
      int max_distance;
      int color, style;

      // the maximum distance a point can be from the center of the line is a
      // function of the width of the line
      if (!m_pen.get_background_pen(color, style, max_distance))
         m_pen.get_foreground_pen(color, style, max_distance);

      // m_point_list must contain an even number of points
      ASSERT(m_point_list.GetCount() % 2 == 0);

      if (m_drawn_simple)
      {
         point1 = m_point_list.GetHead();
         point2 = m_point_list.GetTail();
         if (util->distance_to_line(point1->x, point1->y, point2->x, point2->y,
            point.x, point.y) < max_distance)
            return TRUE;
      }
      else
      {
         // check all lines in the contour
         position = m_point_list.GetHeadPosition();
         while (position)
         {
            point1 = m_point_list.GetNext(position);
            point2 = m_point_list.GetNext(position);
            if (util->distance_to_line(point1->x, point1->y,
               point2->x, point2->y,
               point.x, point.y) < max_distance)
               return TRUE;
         }
      }
   }

   return FALSE;
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlContour::invalidate(boolean_t erase_background)
{
   // the object must be valid and it must contain at least one segment
   if (m_allow_redraw && m_point_list.GetCount() > 1)
   {
      CFvwUtil *util = CFvwUtil::get_instance();
      CPoint *point1;
      CPoint *point2;
      POSITION position;
      CRect rect;
      int width;
      int color, style;
      int unused_parameter1, unused_parameter2;

      // if background pen is turned-on, use the background pen's width
      if (!m_pen.get_background_pen(color, style, width))
         m_pen.get_foreground_pen(color, style, width);

      // use the foreground pen style
      m_pen.get_foreground_pen(unused_parameter1, style, unused_parameter2);

      // m_point_list must contain an even number of points
      ASSERT(m_point_list.GetCount() % 2 == 0);

      // invalidate lines in the contour
      position = m_point_list.GetHeadPosition();
      while (position)
      {
         point1 = m_point_list.GetNext(position);
         point2 = m_point_list.GetNext(position);
         rect = util->get_bounding_rect(point1->x, point1->y,
            point2->x, point2->y, style, width);
         rect.InflateRect(3,3);
         if (!rect.IsRectEmpty())
            OVL_get_overlay_manager()->invalidate_rect(rect, erase_background);
      }
   }
}

// Returns a bounding rectangle on the overlay element.  The CRect
// will be empty if the object is off the screen.
CRect OvlContour::get_rect()
{
   CRect rect;
   CFvwUtil* pUtil = CFvwUtil::get_instance();

   int width;
   int color, style;

   // if background pen is turned-on, use the background pen's width
   if (!m_pen.get_background_pen(color, style, width))
      m_pen.get_foreground_pen(color, style, width);

   POSITION position = m_point_list.GetHeadPosition();
   while (position)
   {
      CPoint* point1 = m_point_list.GetNext(position);
      CPoint* point2 = m_point_list.GetNext(position);
      CRect currentRect = pUtil->get_bounding_rect(point1->x, point1->y,
         point2->x, point2->y, style, width);
      if (!currentRect.IsRectEmpty())
      {
         currentRect.InflateRect(3,3);
         rect |= currentRect;
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
int OvlContour::view_draw(map_projection_utils::CMapProjWrapper* pMap,
   gfx::GraphicsContextWrapper* gc)
{
   gc->DrawPolyLine(m_pen.get_fv_pen(gc, m_draw_mode), NULL,
      CLineSegmentList(m_point_list));

   return SUCCESS;
}

int OvlContour::view_draw(MapProj* map, CDC* dc)
{
   UtilDraw line(dc);

   int width;
   COLORREF color;
   int style;
   int unused_parameter1, unused_parameter2;

   // If the list is empty then this arc is not in the view, or it is a NULL
   // arc, i.e., it had zero radius or the map scale caused it to be reduced
   // to nothing.
   if (m_point_list.IsEmpty())
      return SUCCESS;

   if ( m_draw_mode == NORMAL_MODE )
   {
      // draw background
      if (m_pen.get_background_pen(color, style, width))
      {
         // don't draw background is color is < 0 (background disabled)
         if (color >= 0)
         {
            // set the pen's color style and width.  The fourth parameter is
            // set to TRUE to indicate a background line is being drawn.  This
            // causes draw_line to use the pattern of a pen of width-2.  This
            // is necessary since increasing line width scales the length of
            // the lines in the pattern.
            m_pen.get_foreground_pen(unused_parameter1, style,
               unused_parameter2);
            line.set_pen(color, style, width, TRUE);

            // m_point_list must contain at least 2 points
            ASSERT(m_point_list.GetCount() > 1);
            // m_point_list must contain an even number of points
            ASSERT(m_point_list.GetCount() % 2 == 0);

            line.DrawPolyLine(CLineSegmentList(m_point_list));
         }
      }

      // draw foreground
      // set foreground pen
      m_pen.get_foreground_pen(color, style, width);
      line.set_pen(color, style, width);

      // m_point_list must contain at least 2 points
      ASSERT(m_point_list.GetCount() > 1);
      // m_point_list must contain an even number of points
      ASSERT(m_point_list.GetCount() % 2 == 0);

      line.DrawPolyLine(CLineSegmentList(m_point_list));
   }
   else if ( m_draw_mode == BACKGROUND_MODE )
   {
      // draw background
      if (m_pen.get_background_pen(color, style, width))
      {
         // set the pen's color style and width.  The fourth parameter is set
         // to TRUE to indicate a background line is being drawn.  This causes
         // draw_line to use the pattern of a pen of width-2.  This is
         // necessary since increasing line width scales the length of the
         // lines in the pattern.
         m_pen.get_foreground_pen(unused_parameter1, style, unused_parameter2);

         line.set_pen(color, style, width, TRUE);

         // m_point_list must contain at least 2 points
         ASSERT(m_point_list.GetCount() > 1);
         // m_point_list must contain an even number of points
         ASSERT(m_point_list.GetCount() % 2 == 0);

         line.DrawPolyLine(CLineSegmentList(m_point_list));
      }
   }
   else if ( m_draw_mode == FOREGROUND_MODE )
   {
      // draw foreground
      // set foreground pen
      m_pen.get_foreground_pen(color, style, width);

      line.set_pen(color, style, width);

      // m_point_list must contain at least 2 points
      ASSERT(m_point_list.GetCount() > 1);
      // m_point_list must contain an even number of points
      ASSERT(m_point_list.GetCount() % 2 == 0);

      line.DrawPolyLine(CLineSegmentList(m_point_list));

   }
   else if ( m_draw_mode == XOR_MODE)
   {
      // use XOR lines
      int old_rop = dc->SetROP2(R2_XORPEN);

      // draw the xor lines in normal mode
      m_draw_mode = NORMAL_MODE;
      int status = OvlContour::view_draw(map, dc);
      m_draw_mode = XOR_MODE;

      // restore DC
      dc->SetROP2(old_rop);
   }
   else
   {
      ERR_report("Invalid drawing mode in OvlContour::view_draw().");
      return FAILURE;
   }

   return SUCCESS;
}

// returns TRUE if the class name is OvlContour
boolean_t OvlContour::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlContour") == 0)
      return TRUE;

   return OvlElement::is_kind_of(class_name);
}

// delete all the points in m_point_list
void OvlContour::delete_points()
{
   // remove and delete all points in the point list
   while (!m_point_list.IsEmpty())
      delete m_point_list.RemoveHead();
}

void OvlContour::reverse_points()
{
   // if the list is empty, there is nothing to do
   if (m_point_list.IsEmpty())
      return;

   POSITION insertAfterPos = m_point_list.AddHead(m_point_list.RemoveTail());
   while (insertAfterPos != m_point_list.GetTailPosition())
      insertAfterPos = m_point_list.InsertAfter(insertAfterPos,
      m_point_list.RemoveTail());
}

BOOL OvlContour::get_points(CList <CPoint *, CPoint *> & list)
{
   CPoint *pt, *npt;
   POSITION next;
   int cnt;

   if (m_point_list.IsEmpty())
      return FALSE;

   // copy the list
   cnt = m_point_list.GetCount();
   next = m_point_list.GetHeadPosition();
   while (next != NULL)
   {
      pt = m_point_list.GetNext(next);
      npt = new CPoint(*pt);
      list.AddTail(npt);
   }

   return TRUE;
}

int OvlContour::draw_line_with_embedded_text(
   map_projection_utils::CMapProjWrapper* map,
   gfx::GraphicsContextWrapper* gc,
   double lat1, double lon1, double lat2, double lon2, int line_type)
{
   CDC dc;
   HDC handle = gc->get_dc();

   // Note that throwing or otherwise exiting this method without detaching will
   // leak this attach.
   dc.Attach(handle);

   // convert list of geo-coords to arrays of lats and longs
   degrees_t lat[2] = { lat1, lat2 };
   degrees_t lon[2] = { lon1, lon2 };

   // get line attributes
   COLORREF bgcolor, color;
   int style, width;
   m_pen.get_background_pen(bgcolor, style, width);
   m_pen.get_foreground_pen(color, style, width);

   // get font attributes
   CString name;
   int size, attributes;
   int text_fg_color, bg_type, text_bg_color;
   COLORREF text_fg_color_rgb, text_bg_color_rgb;
   m_font.get_font(name, size, attributes);
   m_font.get_foreground(text_fg_color);
   m_font.get_foreground_RGB(text_fg_color_rgb);
   m_font.get_background(bg_type, text_bg_color);
   m_font.get_background_RGB(bg_type, text_bg_color_rgb);

   // clear poly list
   while (!m_embedded_poly_list.IsEmpty())
      delete m_embedded_poly_list.RemoveTail();

   int status;
   if (m_font.color_type_is_RGB())
      status = fv_core_mfc::CFvwUtil::draw_geo_text_line_center(
      map, &dc, lat, lon, 2, color, bgcolor, style, width, line_type, FALSE,
      m_embedded_text, name, size, attributes, text_fg_color_rgb,
      text_bg_color_rgb, bg_type, 0, TRUE, TRUE, m_embedded_poly_list);
   else
      status = fv_core_mfc::CFvwUtil::draw_geo_text_line_center(
      map, &dc, lat, lon, 2, color, bgcolor, style, width, line_type, FALSE,
      m_embedded_text, name, size, attributes, text_fg_color,
      text_bg_color, bg_type, 0, TRUE, TRUE, m_embedded_poly_list);

   // if the text wasn't drawn then we will need to draw it underneath the line
   if (status == UTIL_GEO_TEXT_LINE_NO_TEXT)
   {
      int anchor_type;
      int anchor_x, anchor_y;

      anchor_x = (m_point_list.GetHead()->x +
         m_point_list.GetTail()->x) / 2;
      anchor_y = __max(m_point_list.GetHead()->y,
         m_point_list.GetTail()->y) + width + 5;
      anchor_type = UTIL_ANCHOR_UPPER_CENTER;

      POINT cpt[4];
      if (m_font.color_type_is_RGB())
         CFvwUtil::get_instance()->draw_text_RGB(&dc, m_embedded_text,
         anchor_x, anchor_y, anchor_type, name, size, attributes,
         bg_type, text_fg_color_rgb, text_bg_color_rgb, 0, cpt);
      else
         CFvwUtil::get_instance()->draw_text(&dc, m_embedded_text,
         anchor_x, anchor_y, anchor_type, name, size, attributes,
         bg_type, text_fg_color, text_bg_color, 0, cpt);

      fv_core_mfc::C_poly_xy *polyxy = new fv_core_mfc::C_poly_xy;
      for (int j=0; j<4; j++)
         polyxy->add_point(cpt[j].x, cpt[j].y);
      m_embedded_poly_list.AddTail(polyxy);
   }

   dc.Detach();

   return SUCCESS;
}

int OvlContour::draw_line_with_embedded_text(MapProj* map, CDC* dc,
   double lat1, double lon1, double lat2, double lon2,
   int line_type)
{
   // convert list of geo-coords to arrays of lats and longs
   degrees_t lat[2] = { lat1, lat2 };
   degrees_t lon[2] = { lon1, lon2 };

   // get line attributes
   COLORREF bgcolor, color;
   int style, width;
   m_pen.get_background_pen(bgcolor, style, width);
   m_pen.get_foreground_pen(color, style, width);

   // get font attributes
   CString name;
   int size, attributes;
   int text_fg_color, bg_type, text_bg_color;
   COLORREF text_fg_color_rgb, text_bg_color_rgb;
   m_font.get_font(name, size, attributes);
   m_font.get_foreground(text_fg_color);
   m_font.get_foreground_RGB(text_fg_color_rgb);
   m_font.get_background(bg_type, text_bg_color);
   m_font.get_background_RGB(bg_type, text_bg_color_rgb);

   // clear poly list
   while (!m_embedded_poly_list.IsEmpty())
      delete m_embedded_poly_list.RemoveTail();

   int status;
   if (m_font.color_type_is_RGB())
      status = CFvwUtil::get_instance()->draw_geo_text_line_center(
      map, dc, lat, lon, 2, color, bgcolor, style, width, line_type, FALSE,
      m_embedded_text, name, size, attributes, text_fg_color_rgb,
      text_bg_color_rgb, bg_type, 0, TRUE, TRUE, m_embedded_poly_list);
   else
      status = CFvwUtil::get_instance()->draw_geo_text_line_center(
      map, dc, lat, lon, 2, color, bgcolor, style, width, line_type, FALSE,
      m_embedded_text, name, size, attributes, text_fg_color,
      text_bg_color, bg_type, 0, TRUE, TRUE, m_embedded_poly_list);

   // if the text wasn't drawn then we will need to draw it underneath the line
   if (status == UTIL_GEO_TEXT_LINE_NO_TEXT)
   {
      int anchor_type;
      int anchor_x, anchor_y;

      anchor_x = (m_point_list.GetHead()->x +
         m_point_list.GetTail()->x) / 2;
      anchor_y = __max(m_point_list.GetHead()->y,
         m_point_list.GetTail()->y) + width + 5;
      anchor_type = UTIL_ANCHOR_UPPER_CENTER;

      POINT cpt[4];
      if (m_font.color_type_is_RGB())
         CFvwUtil::get_instance()->draw_text_RGB(dc, m_embedded_text,
         anchor_x, anchor_y, anchor_type, name, size, attributes,
         bg_type, text_fg_color_rgb, text_bg_color_rgb, 0, cpt);
      else
         CFvwUtil::get_instance()->draw_text(dc, m_embedded_text,
         anchor_x, anchor_y, anchor_type, name, size, attributes,
         bg_type, text_fg_color, text_bg_color, 0, cpt);

      fv_core_mfc::C_poly_xy *polyxy = new fv_core_mfc::C_poly_xy;
      for (int j=0; j<4; j++)
         polyxy->add_point(cpt[j].x, cpt[j].y);
      m_embedded_poly_list.AddTail(polyxy);
   }

   return SUCCESS;
}