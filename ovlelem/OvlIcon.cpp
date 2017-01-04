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



// OvlIcon.cpp
#include "stdafx.h"
#include "param.h"
#include "ovlelem.h"
#include "map.h"
#include "ovl_mgr.h"

// Constructor
OvlIcon::OvlIcon() : m_icon_image(NULL)
{
   m_text_object = new OvlText();

   m_in_view = FALSE;

   m_icon_set_id = -1;

	m_offset_x = 0;
	m_offset_y = 0;

	m_icon_size = 32;
}

// load the icon with the given filename
int OvlIcon::load_icon(CString filename)
{ 
   // the caller can specify a filename relative to HD_DATA\icons
   if (::GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES)
      m_filename = filename;
   else
      m_filename = PRM_get_registry_string("Main", "HD_DATA") + "\\icons\\" + filename;

   return SUCCESS;
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlIcon::prepare_for_redraw(MapProj* map)
{
	if (map == NULL)
		return FAILURE;

   int x, y;
   m_text_object->prepare_for_redraw(map);

   if (!map->geo_in_surface(m_lat, m_lon))
   {
      m_in_view = FALSE;
      return SUCCESS;
   }  

   // calculate the screen coordinates of the icon
   map->geo_to_surface(m_lat, m_lon, &x, &y);
   m_view_coordinates = CPoint(x + m_offset_x, y + m_offset_y);

   m_rect.SetRect(m_view_coordinates.x, m_view_coordinates.y,
      m_view_coordinates.x, m_view_coordinates.y);
	const int size = (m_icon_size / 2);
   m_rect.InflateRect(size, size);

   CRect view_rect(0,0, map->get_surface_width(), 
      map->get_surface_height());

   // does the icon's bounding rectangle intersect the view
   CRect intersect_rect;
   m_in_view = intersect_rect.IntersectRect(m_rect, view_rect);

   return SUCCESS;
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t OvlIcon::hit_test(CPoint &point)
{
   if (m_in_view)
		return (m_rect.PtInRect(point) || m_text_object->hit_test(point));

	return FALSE;
}

// Invalidate the part of the screen covered by this frame
void OvlIcon::invalidate(boolean_t erase_background /*= FALSE*/)
{
   if (m_in_view)
      OVL_get_overlay_manager()->invalidate_rect(m_rect, erase_background);

   m_text_object->invalidate(erase_background);
}

// Returns TRUE if the last call to draw() determined that the object was
// within the current view.  Note this really means that the last call to
// prepare_for_redraw() determined that the object was within the current
// view.
boolean_t OvlIcon::in_view()
{
   return m_in_view;
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int OvlIcon::view_draw(MapProj* map, CDC* dc)
{
   if (m_icon_image == NULL)
   {
      m_icon_image = CIconImage::load_images(m_filename, "", 0);
      if (m_icon_image == NULL)
         return FAILURE;

      return SUCCESS;
   }

	int start_size = (m_icon_size >= 16 && m_icon_size < 32) ? 16 : 32;

   m_icon_image->draw_icon(dc, m_view_coordinates.x, m_view_coordinates.y, start_size, m_icon_size);
   m_text_object->view_draw(map, dc);

   return SUCCESS;
}

// keeps track of bitmaps created in OvlIcon::view_draw so that only a single bitmap
// is created for each filename
class OvlIconManager
{
   OvlIconManager() { }
   ~OvlIconManager() { }

   std::map<std::string, IFvBitmapPtr> m_bitmaps;
   std::map<std::string, int> m_internal_ref_count;

public:

   static OvlIconManager& GetInstance()
   {
      static OvlIconManager s_icon_manager;
      return s_icon_manager;
   }

   IFvBitmapPtr GetBitmapForFilename(gfx::GraphicsContextWrapper* gc, const std::string filename)
   {
      auto it = m_bitmaps.find(filename);
      if (it == m_bitmaps.end())
      {
         m_bitmaps[filename] = gc->create_bitmap(_bstr_t(filename.c_str()));
         m_internal_ref_count[filename] = 1;
      }
      else
         ++m_internal_ref_count[filename];

      return m_bitmaps[filename];
   }
   void Release(const std::string filename)
   {
      auto it = m_bitmaps.find(filename);
      auto it_ref = m_internal_ref_count.find(filename);
      if (it != m_bitmaps.end())
      {
         if (--it_ref->second == 0)
         {
            m_bitmaps.erase(it);
            m_internal_ref_count.erase(it_ref);
         }
      }
   }
};

// Destructor
OvlIcon::~OvlIcon()
{
   if (m_fv_bitmap)
      OvlIconManager::GetInstance().Release(std::string(m_filename));

   delete m_text_object;
}

int OvlIcon::view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   if (m_fv_bitmap == NULL)
      m_fv_bitmap = OvlIconManager::GetInstance().GetBitmapForFilename(gc, std::string(m_filename));

   gc->draw_bitmap(m_fv_bitmap, 
      static_cast<float>(m_rect.left), 
      static_cast<float>(m_rect.top), 
      static_cast<float>(m_rect.right), 
      static_cast<float>(m_rect.bottom), 0.0f, 1.0f);

   m_text_object->view_draw(pMap, gc);

   return SUCCESS;
}

// set the location of the icon
void OvlIcon::set_location(double lat, double lon)
{
   m_text_object->set_location(lat, lon);
   
   m_lat = lat;
   m_lon = lon;
   m_allow_redraw = FALSE;
}

// returns TRUE if the class name is OvlIcon
boolean_t OvlIcon::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlIcon") == 0)
      return TRUE;
   
   return OvlElement::is_kind_of(class_name);
}