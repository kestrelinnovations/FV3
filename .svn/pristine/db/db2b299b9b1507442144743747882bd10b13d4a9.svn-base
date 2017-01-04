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



// OvlBitmap.cpp
#include "stdafx.h"
#include "ovlutil.h"     // get_geo_line_coordinates
#include "map.h"
#include "dib.h"
#include "ovl_mgr.h"

// Constructor
OvlBitmap::OvlBitmap()
{
   m_bitmap = NULL;
   m_in_view = FALSE;
}

// Destructor
OvlBitmap::~OvlBitmap()
{
   delete m_bitmap;
}

// load the bitmap with the given filename
int OvlBitmap::load_bitmap(const CString& filename)
{ 
   m_filename = filename;
   return SUCCESS;
}

// set the position and anchor type of the bitmap
void OvlBitmap::set_position(double lat, double lon, int anchor_type)
{
   m_lat = lat;
   m_lon = lon;
   m_anchor_type = anchor_type;
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlBitmap::prepare_for_redraw(MapProj* map)
{
	if (map == NULL)
		return FAILURE;

   if (m_bitmap == NULL)
   {
      m_bitmap = new CDib();
      if (!m_bitmap->Load(m_filename))
         return FAILURE;
   }

   int x, y;
   CSize bitmap_size = m_bitmap->GetSize();
   
   // calculate the bounding rect of the bitmap
   map->geo_to_surface(m_lat, m_lon, &x, &y);
   m_rect = CRect(x, y, x + bitmap_size.cx, y + bitmap_size.cy);

   // depending on the anchor position we need to adjust the rect
   switch (m_anchor_type)
   {
   case UTIL_ANCHOR_LOWER_LEFT:
      m_rect.OffsetRect(0, -bitmap_size.cy);
      break;
   case UTIL_ANCHOR_UPPER_LEFT:
      m_rect.OffsetRect(0,0);
      break;
   case UTIL_ANCHOR_LOWER_CENTER:
      m_rect.OffsetRect(-bitmap_size.cx/2,-bitmap_size.cy);
      break;
   case UTIL_ANCHOR_UPPER_CENTER:
      m_rect.OffsetRect(-bitmap_size.cx/2, 0);
      break;
   case UTIL_ANCHOR_LOWER_RIGHT:
      m_rect.OffsetRect(-bitmap_size.cx, -bitmap_size.cy);
      break;
   case UTIL_ANCHOR_UPPER_RIGHT:
      m_rect.OffsetRect(-bitmap_size.cx, 0);
      break;
   case UTIL_ANCHOR_CENTER_LEFT:
      m_rect.OffsetRect(0, -bitmap_size.cy/2);
      break;
   case UTIL_ANCHOR_CENTER_RIGHT:
      m_rect.OffsetRect(-bitmap_size.cx, -bitmap_size.cy/2);
      break;
   case UTIL_ANCHOR_CENTER_CENTER:
      m_rect.OffsetRect(-bitmap_size.cx/2,-bitmap_size.cy/2);
      break;
   }

   // calculate the rect of the screen view coordinates
   CRect view_rect(0,0, map->get_surface_width(), 
      map->get_surface_height());

   // does the bitmap lie inside the view?
   CRect intersect_rect;
   m_in_view = intersect_rect.IntersectRect(m_rect, view_rect);
   
   return SUCCESS;
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t OvlBitmap::hit_test(CPoint &point)
{
	if (m_in_view)
		return m_rect.PtInRect(point);

	return FALSE;
}

// Invalidate the part of the screen covered by this frame
void OvlBitmap::invalidate(boolean_t erase_background /*= FALSE*/)
{
   if (m_in_view)
      OVL_get_overlay_manager()->invalidate_rect(m_rect, 
      erase_background);
}

// Returns TRUE if the last call to draw() determined that the object was
// within the current view.  Note this really means that the last call to
// prepare_for_redraw() determined that the object was within the current
// view.
boolean_t OvlBitmap::in_view()
{
   return m_in_view;
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int OvlBitmap::view_draw(MapProj* map, CDC* dc)
{
	m_bitmap->Draw(*dc, &m_rect, NULL, FALSE, NULL, FALSE);

   return SUCCESS;
}

int OvlBitmap::view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   if (m_fv_bitmap == NULL)
      m_fv_bitmap = gc->create_bitmap(_bstr_t(m_filename));

   return gc->draw_bitmap(m_fv_bitmap, 
      static_cast<float>(m_rect.left), 
      static_cast<float>(m_rect.top), 
      static_cast<float>(m_rect.right), 
      static_cast<float>(m_rect.bottom), 0.0f, 1.0f);
}

// returns TRUE if the class name is OvlIcon
boolean_t OvlBitmap::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlBitmap") == 0)
      return TRUE;
   
   return OvlElement::is_kind_of(class_name);
}
