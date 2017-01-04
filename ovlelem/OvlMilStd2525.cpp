// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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



// OvlMilStd2525.cpp
#include "stdafx.h"
#include "OvlMilStd2525.h"
#include "Map.h"
#include "err.h"
#include "ovl_mgr.h"
#include "..\resource.h"
#include "..\mapview.h"
#include "..\getobjpr.h"

static int g_reference_count = 0;

// Constructor
OvlMilStd2525Symbol::OvlMilStd2525Symbol( BSTR milstd_id )
	: m_milstd_id( milstd_id )
{
	g_reference_count++;
   m_in_view = FALSE;
}

// Destructor
OvlMilStd2525Symbol::~OvlMilStd2525Symbol()
{
	SysFreeString( m_milstd_id );
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
int OvlMilStd2525Symbol::draw(MapProj *pmap, CDC *dc)
{
   int res = SUCCESS;

   m_rect = CRect(0,0,0,0);

   prepare_for_redraw( pmap );
   if ( this->in_view() )
   {
#ifdef GOV_RELEASE
      if (m_gsd_renderer == nullptr)
      {
         m_gsd_renderer.CreateInstance(__uuidof(TacticalGraphicsGsdRenderer));
      }

      ATLASSERT(m_gsd_renderer != nullptr);

      if (!m_gsd_renderer)
      {
			ERR_report("Error drawing MilStd2525 symbol. No renderer found");
         return 0;
      }

      MapView *pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
      if (pView == NULL)
      {
			ERR_report("Error drawing MilStd2525 symbol. No view found");
         return 0;
      }

      res = m_gsd_renderer->Render(pView->GetFvMapView(), 
         _bstr_t(m_milstd_id), m_view_coordinates.x, m_view_coordinates.y, 
         reinterpret_cast<long>(dc->GetSafeHdc()));
#else
      return FAILURE;  // not supported in open source release
#endif
   }

	m_allow_redraw = TRUE;

   return SUCCESS;
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlMilStd2525Symbol::prepare_for_redraw(MapProj* pmap)
{
	if (pmap == NULL)
		return FAILURE;

   int x, y;
   if (!pmap->geo_in_surface(m_lat, m_lon))
   {
      m_in_view = FALSE;
      return SUCCESS;
   }  

   // calculate the screen coordinates of the icon
   pmap->geo_to_surface(m_lat, m_lon, &x, &y);
   m_view_coordinates = CPoint(x,y);

   m_in_view = TRUE;
   return SUCCESS;
}

// Redraw the element exactly as it was drawn by the last call to draw().
int OvlMilStd2525Symbol::redraw(MapProj *pmap, CDC *dc)
{
   return view_draw(pmap, dc);
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t OvlMilStd2525Symbol::hit_test(CPoint &point)
{
   if (m_in_view)
		return (m_rect.PtInRect(point));

	return FALSE;
}

// Invalidate the part of the screen covered by this frame
void OvlMilStd2525Symbol::invalidate(boolean_t erase_background /*= FALSE*/)
{
   if (m_in_view)
   {
      int x = m_view_coordinates.x;
      int y = m_view_coordinates.y;

      // assume icons are 32x32 create a rect around the 
      // icon so that it can be invalidated
      CRect rect(x-18, y-18, x+18, y+18);
      OVL_get_overlay_manager()->invalidate_rect(rect, 
      erase_background);
   }
}

// Returns TRUE if the last call to draw() determined that the object was
// within the current view.  Note this really means that the last call to
// prepare_for_redraw() determined that the object was within the current
// view.
boolean_t OvlMilStd2525Symbol::in_view()
{
   return m_in_view;
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int OvlMilStd2525Symbol::view_draw(MapProj* pmap, CDC* dc)
{
   int res = SUCCESS;

   m_rect = CRect(0,0,0,0);

   prepare_for_redraw( pmap );
   if ( this->in_view() )
   {
#ifdef GOV_RELEASE
      if (m_gsd_renderer == nullptr)
      {
         m_gsd_renderer.CreateInstance(__uuidof(TacticalGraphicsGsdRenderer));
      }

      ATLASSERT(m_gsd_renderer != nullptr);
  
      if (!m_gsd_renderer)
      {
			ERR_report("Error drawing MilStd2525 symbol. No renderer found");
         return 0;
      }
      
      MapView *pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
      if (pView == NULL)
      {
			ERR_report("Error drawing MilStd2525 symbol. No view found");
         return 0;
      }

      res = m_gsd_renderer->Render(pView->GetFvMapView(), _bstr_t(m_milstd_id), 
         m_view_coordinates.x, m_view_coordinates.y, 
         reinterpret_cast<long>(dc->GetSafeHdc()));
#else
      return FAILURE;  // not supported in open source release
#endif
   }

	m_allow_redraw = TRUE;
	return SUCCESS;
}

// set the location of the icon
void OvlMilStd2525Symbol::set_location(double lat, double lon)
{
   m_lat = lat;
   m_lon = lon;
   m_allow_redraw = FALSE;
}

// returns TRUE if the class name is OvlMilStd2525Symbol
boolean_t OvlMilStd2525Symbol::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlMilStd2525Symbol") == 0)
      return TRUE;
   
   return OvlElement::is_kind_of(class_name);
}
