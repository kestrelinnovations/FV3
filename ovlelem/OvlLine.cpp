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



// OvlLine.cpp

// OvlLine contours represent set of lines from lat/lon pair to lat/lon
// pair.  The lines will appear as straight lines on the screen (unlike
// the rhumb line or great circle lines.  Support for 2 or more pairs
// of lat/lons allows this class to draw polylines on the screen.  For 
// filled polygons see OvlPolygon.

#include "stdafx.h"
#include "ovlutil.h"     // get_geo_line_coordinates
#include "mapx.h"

// Constructor
OvlLine::OvlLine()
{
   m_x_offset = m_y_offset = 0;  
}

// destructor
OvlLine::~OvlLine()
{
	m_geo_list.RemoveAll();

	// clear poly list
	while (!m_embedded_poly_list.IsEmpty())
		delete m_embedded_poly_list.RemoveTail();
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlLine::prepare_for_redraw(MapProj* map)
{
	if (map == NULL)
		return FAILURE;

   COvlkitUtil ovlkit_util;
   CPoint p1, p2, p1_wrap, p2_wrap;
	
   // need at list two geo points
   if (m_geo_list.GetCount() < 2)
      return SUCCESS;
	
	// delete all points in m_point_list
	delete_points();
	
	// loop through the list of geo points and add screen coordinate
	// points to the point list
	POSITION position1 = m_geo_list.GetHeadPosition();
	POSITION position2 = m_geo_list.GetHeadPosition();
	m_geo_list.GetNext(position2);
	
	while (position2)
	{
		// convert the lat, lon pair of the two vertices into
		// screen coordinates and add to the point list
		ovlkit_util.get_geo_line_coordinates(map, m_geo_list.GetNext(position1), 
			m_geo_list.GetNext(position2), 
			p1, p2, p1_wrap, p2_wrap);
		
		m_point_list.AddTail(new CPoint(p1.x + m_x_offset, p1.y + m_y_offset));
		m_point_list.AddTail(new CPoint(p2.x + m_x_offset, p2.y + m_y_offset));
		
		// if the line wrapped around the world
		if (p1_wrap != CPoint(-1, -1))
		{
			m_point_list.AddTail(new CPoint(p1_wrap.x + m_x_offset, 
				p1_wrap.y + m_y_offset));
			m_point_list.AddTail(new CPoint(p2_wrap.x + m_x_offset, 
				p2_wrap.y + m_y_offset));
		}
	}
	
	m_allow_redraw = TRUE;
	
   return SUCCESS;
}

int OvlLine::view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   // if the embedded text field is empty or the number or geo-coords
	// is greater than two, then pass control to base and draw a regular
	// line
	if (m_embedded_text.IsEmpty() || m_geo_list.GetCount() > 2)
		return OvlContour::view_draw(pMap, gc);

   d_geo_t p1 = m_geo_list.GetHead();
   d_geo_t p2 = m_geo_list.GetTail();

   return draw_line_with_embedded_text(pMap, gc, p1.lat, p1.lon, p2.lat, p2.lon, 1);
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int OvlLine::view_draw(MapProj* map, CDC* dc)
{
	// if the embedded text field is empty or the number or geo-coords
	// is greater than two, then pass control to base and draw a regular
	// line
	if (m_embedded_text.IsEmpty() || m_geo_list.GetCount() > 2)
		return OvlContour::view_draw(map, dc);

   d_geo_t p1 = m_geo_list.GetHead();
   d_geo_t p2 = m_geo_list.GetTail();

   return draw_line_with_embedded_text(map, dc, p1.lat, p1.lon,
      p2.lat, p2.lon, 1);
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlLine::invalidate(boolean_t erase_background)
{
	// if the embedded text field is empty or the number or geo-coords
	// is greater than two, then pass control to base and invalidate
	// a regular line
	if (m_embedded_text.IsEmpty() || m_geo_list.GetCount() > 2)
	{
		OvlContour::invalidate();
		return;
	}

	COvlkitUtil util;

	CView* view;
	ViewMapProj* map;
		
	view = UTL_get_active_non_printing_view();
	if (view == NULL)
		return;
	map = UTL_get_current_view_map(view);
	if (map == NULL)
		return;

	// get line attributes
	COLORREF color;
	int style, width;
	m_pen.get_foreground_pen(color, style, width);

	if (m_geo_list.GetHead().lat != m_geo_list.GetTail().lat ||
		 m_geo_list.GetHead().lon != m_geo_list.GetTail().lon)
	{
		// invalidate the line
		util.invalidate_geo_line(reinterpret_cast<MapProj*>(map), 
			m_geo_list.GetHead().lat, m_geo_list.GetHead().lon,
			m_geo_list.GetTail().lat, m_geo_list.GetTail().lon,
			width, UTIL_LINE_TYPE_SIMPLE);
		
		if (m_point_list.GetCount() == 2)
			util.invalidate_line(m_point_list.GetHead()->x, m_point_list.GetHead()->y, 
			m_point_list.GetTail()->x, m_point_list.GetTail()->y);
	}

	// invalidate embedded text
	util.invalidate_poly_list(m_embedded_poly_list);
}

// returns TRUE if the class name is GeoSegment
boolean_t OvlLine::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlLine") == 0)
      return TRUE;
   
   return OvlContour::is_kind_of(class_name);
}
