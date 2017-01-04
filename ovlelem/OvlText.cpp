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



// OvlText.cpp

#include "stdafx.h"
#include "ovlelem.h"
#include "mapx.h"
#include "..\getobjpr.h"   // fvw_get_frame()
#include "ovl_mgr.h"

//*****************************************************************************
// OvlText Implementation
//
//*****************************************************************************

// Constructor
//
// The anchor point is initialized to an invalid value.  You must call
// set_anchor() with a valid value before the draw() member can be called.
// The draw() member will return FAILURE until the anchor has been assigned
// a valid value.  Note the anchor offset is set to (0,0) and the anchor 
// type is set to UTIL_ANCHOR_UPPER_CENTER by default.
//
// The text is set to be an empty string.  You must call set_text() with
// a valid string before the draw() member can be called.  The draw() member
// will return FAILURE until the text has been set.
OvlText::OvlText() : OvlElement(), m_font(m_allow_redraw), m_text(""),
   m_offset(0,0), m_anchor_type(UTIL_ANCHOR_UPPER_CENTER), m_rect(0,0,0,0)   
{
   m_anchor.lat = m_anchor.lon = -1000.0;
	m_angle = 0.0;
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t OvlText::hit_test(CPoint &point)
{
	if (!get_rect().IsRectEmpty())
	{
		if (get_rect().PtInRect(point))
		{
			CRgn tmpRgn;
			tmpRgn.CreatePolygonRgn(m_box, 4, ALTERNATE);
			
			return tmpRgn.PtInRegion(point);
		}
	}
	return FALSE;
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlText::invalidate(boolean_t erase_background)
{
   CRect rect(get_rect());

   if (!rect.IsRectEmpty())
	{
		CRgn tmpRgn;
		tmpRgn.CreatePolygonRgn(m_box, 4, ALTERNATE);

      OVL_get_overlay_manager()->invalidate_rgn(&tmpRgn, erase_background);
	}
}

// Returns a bounding rectangle on the overlay element.  The CRect
// will be empty if the object is off the screen.   
CRect OvlText::get_rect()
{
   // If m_allow_redraw is FALSE, then m_rect is in an unreliable state, so
   // hit_test, invalidate, and get_rect functions should behave as if the
   // object is not on the screen.
   if (m_allow_redraw == FALSE)
      m_rect.SetRectEmpty();

   return m_rect;
}

// Set the text to be displayed.  Strings must be null terminated, '\0'.
// Use "\r\n" to end lines ONLY if multi-line text is desired.  Do not
// end the last line with "\r\n".
int OvlText::set_text(const char *text)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CList <CString *, CString *> text_lst;

   // if there is no change, do nothing   
   if (m_text == text)
      return SUCCESS;

   // if the string is empty
   if (text[0] == '\0')
   {
      m_text = "";
      m_rect.SetRectEmpty();
      return SUCCESS;
   }

   futil->parse_the_text(text, text_lst);
   m_multi_line = text_lst.GetCount() > 1;
   while (!text_lst.IsEmpty())
      delete text_lst.RemoveTail();

   // a change in the text will force a fresh draw
   m_text = text;
   m_allow_redraw = FALSE;

   return SUCCESS;
}

// Set the anchor location, type, and offset.
// The text is displayed at offset + the view coordinate of anchor.  The
// orientation relative to that point is determined by the anchor position
// type.  See utils.h for anchor position types.
int OvlText::set_anchor(d_geo_t anchor, int type, CPoint offset)
{
   if (type != UTIL_ANCHOR_LOWER_LEFT &&
      type != UTIL_ANCHOR_UPPER_LEFT &&
      type != UTIL_ANCHOR_LOWER_CENTER &&
      type != UTIL_ANCHOR_UPPER_CENTER &&
      type != UTIL_ANCHOR_LOWER_RIGHT &&
      type != UTIL_ANCHOR_UPPER_RIGHT &&
      type != UTIL_ANCHOR_CENTER_LEFT &&
      type != UTIL_ANCHOR_CENTER_RIGHT &&
      type != UTIL_ANCHOR_CENTER_CENTER)
   {
      ERR_report("Invalid type.");
      return FAILURE;
   }

   // set_anchor will fail if the value is invalid
   if (set_anchor(anchor) != SUCCESS)
      return FAILURE;

   if (m_anchor_type != type || m_offset.x != offset.x || m_offset.y != offset.y)
   {
      m_anchor_type = type;
      m_offset = offset;
      m_allow_redraw = FALSE;
   }

   return SUCCESS;
}

// Like previous function, but it assumes an offset of (0,0).  In other
// words the text anchor point is the view coordinate of anchor.
int OvlText::set_anchor(d_geo_t anchor, int type)
{
   CPoint offset(0,0);

   return set_anchor(anchor, type, offset);
}

// Set the anchor point for the text.  This function has no effect on the
// anchor type or the anchor offset.  It only changes the geographic
// anchor point of the text.
int OvlText::set_anchor(d_geo_t anchor)
{
   if (GEO_valid_degrees(anchor.lat, anchor.lon) == FALSE)
   {
      ERR_report("Invalid location.");
      return FAILURE;
   }

   // only set the value if it has changed
   if (m_anchor.lat != anchor.lat || m_anchor.lon != anchor.lon)
   {
      m_anchor = anchor;
      m_allow_redraw = FALSE;
   }

   return SUCCESS;
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlText::prepare_for_redraw(MapProj* map)
{
	if (map == NULL)
		return FAILURE;

   int x, y;

	const double text_angle = m_angle + map->actual_rotation();

   // Disable the redraw function in case any of these tests fail.  The redraw() 
   // function assumes that m_allow_redraw of TRUE implies valid m_anchor, 
   // m_start_point, and m_end_point.
   m_allow_redraw = FALSE;

   // make sure anchor point has be set to a valid value
   if (GEO_valid_degrees(m_anchor.lat, m_anchor.lon) == FALSE)
   {
      ERR_report("Invalid location.");
      return FAILURE;
   }

   // enable the redraw function
   m_allow_redraw = TRUE;

   // an empty rectangle indicates that the item was not drawn
   m_rect.SetRectEmpty();

   // draw the only if the anchor point is in the view and the text has at
   // least one character
   if (m_text.GetLength() > 0 && map->geo_in_surface(m_anchor.lat, m_anchor.lon, 
      &x, &y))
   {
      CFvwUtil *util = CFvwUtil::get_instance();
      CString name;
      int size;
      int attribute;

		int set_rgn = 0;

      // get the font name, size and attribute
      get_font().get_font(name, size, attribute);

      // compute the view-coordinate anchor point
      m_anchor_view.x = x + m_offset.x;
      m_anchor_view.y = y + m_offset.y;

      // need the DC to get the text extent, need the view to get the DC
      CView *view = fvw_get_frame()->GetActiveView();
      if (view)
      {
         CDC* dc = view->GetDC();
         if (dc)
         {
            int bg_type;
            int bg_color;
            
            get_font().get_background(bg_type, bg_color);
            
            // more than one line
            if (m_multi_line)
            {
               util->get_multi_line_bounds(dc, m_text, 
                  m_anchor_view.x, m_anchor_view.y, m_anchor_type,
                  name, size, attribute, bg_type, text_angle, m_box);               
            }
            else
            {
               int width;
               int height;
               
               util->get_text_size(dc, m_text, name, size, attribute, 
                  &width, &height);
               util->compute_text_poly(m_anchor_view.x, m_anchor_view.y, m_anchor_type,
                  width, height, text_angle, m_box);
            }

				// expand the computed text poly out a few pixels so that 
				// the invalidation encompasses the entire text rect
				double center_x, center_y;
				GEO_xy_line_intersection(m_box[3].x, m_box[3].y,
			      m_box[1].x, m_box[1].y, m_box[0].x, m_box[0].y,
					m_box[2].x, m_box[2].y, &center_x, &center_y);
				CPoint center((int)(center_x + 0.5), (int)(center_y + 0.5));
				int i;
				for(i=0;i<4;i++)
				{
					util->rotate_pt(m_box[i].x, m_box[i].y,
                         &m_box[i].x, &m_box[i].y,
                         -text_angle, center.x, center.y);
				}
				m_box[0].x -= 1;
				m_box[0].y -= 1;
				m_box[1].x += 1;
				m_box[1].y -= 1;
				m_box[2].x += 1;
				m_box[2].y += 1;
				m_box[3].x -= 1;
				m_box[3].y += 1;
				for(i=0;i<4;i++)
				{
					util->rotate_pt(m_box[i].x, m_box[i].y,
                         &m_box[i].x, &m_box[i].y,
                         text_angle, center.x, center.y);
				}


				set_rgn = 1;

				CRgn tmpRgn;
				tmpRgn.CreatePolygonRgn(m_box, 4, ALTERNATE);
				tmpRgn.GetRgnBox(&m_rect);
            
            view->ReleaseDC(dc);
         }
      }

      // just in case you cannot get the DC, you have to set m_rect to some
      // thing, because view_draw will not get called if m_rect is empty
      if (!set_rgn)
      {
         CFvwUtil *util = CFvwUtil::get_instance();

         util->compute_text_poly(m_anchor_view.x, m_anchor_view.y, m_anchor_type,
            size + 1, size + 1, text_angle, m_box);

			CRgn tmpRgn;
			tmpRgn.CreatePolygonRgn(m_box, 4, ALTERNATE);
			tmpRgn.GetRgnBox(&m_rect);
      }
   }

   return SUCCESS;
}

// Returns TRUE if the last call to draw() determined that the object was
// within the current view.  Note this really means that the last call to
// prepare_for_redraw() determined that the object was within the current
// view.
boolean_t OvlText::in_view()
{
   return (get_rect().IsRectEmpty() == FALSE);
}

int OvlText::view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   const float text_angle = static_cast<float>(m_angle + pMap->actual_rotation());
   if (m_text.GetLength() > 0)
   {
      IFvFont* font = get_font().get_fv_font(gc);
      IFvPen* pen = get_font().get_foreground_pen(gc);
      gc->draw_text(_bstr_t(m_text), static_cast<float>(m_anchor_view.x),
         static_cast<float>(m_anchor_view.y), (AnchorType)m_anchor_type,
         font, pen, text_angle);
   }

   return SUCCESS;
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int OvlText::view_draw(MapProj* map, CDC* dc)
{
	const double text_angle = m_angle + map->actual_rotation();
   if (m_text.GetLength() > 0)
   {
      CFvwUtil *util = CFvwUtil::get_instance();
      CString name;
      int size;
      int attribute;
      int fg_color;
      int bg_type;
      int bg_color;
      POINT box[4];
		COLORREF fg_colorref, bg_colorref;
      
      // get the font properties
      get_font().get_font(name, size, attribute);
      get_font().get_foreground(fg_color);
		get_font().get_foreground_RGB(fg_colorref);
      get_font().get_background(bg_type, bg_color);
		get_font().get_background_RGB(bg_type, bg_colorref);
      
      // more than one line
      if (m_multi_line)
      {
         if (get_font().color_type_is_RGB())
            util->draw_multi_line_text_RGB(dc, m_text, 
            m_anchor_view.x, m_anchor_view.y, m_anchor_type,
            name, size, attribute, bg_type, fg_colorref, bg_colorref, 
            text_angle, box);
         else
            util->draw_multi_line_text(dc, m_text, 
            m_anchor_view.x, m_anchor_view.y, m_anchor_type,
            name, size, attribute, bg_type, fg_color, bg_color, 
            text_angle, box);
      }
      else
      {
         if (get_font().color_type_is_RGB())
            util->draw_text_RGB(dc, m_text, 
            m_anchor_view.x, m_anchor_view.y, m_anchor_type,
            name, size, attribute, bg_type, fg_colorref, bg_colorref, text_angle, box);
         else
            util->draw_text(dc, m_text, 
            m_anchor_view.x, m_anchor_view.y, m_anchor_type,
            name, size, attribute, bg_type, fg_color, bg_color, text_angle, box);
      }
      
      // if prepare_for_redraw were guaranteed to set m_rect correctly, this would
      // not be necessary
      m_rect.SetRect(__min(box[0].x, __min(box[1].x, __min(box[2].x, box[3].x))), 
							__min(box[0].y, __min(box[1].y, __min(box[2].y, box[3].y))),
							__max(box[0].x, __max(box[1].x, __max(box[2].x, box[3].x))),
							__max(box[0].y, __max(box[1].y, __max(box[2].y, box[3].y))) );
		m_rect.InflateRect(m_rect.Width(), m_rect.Height());

		// enlarge the rect if necessary based on the background type
		if (bg_type == UTIL_BG_3D)
			m_rect.InflateRect(1,1);
   }

   return SUCCESS;
}

// returns TRUE if the class name is OvlText
boolean_t OvlText::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlText") == 0)
      return TRUE;
   
   return OvlElement::is_kind_of(class_name);
}
