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



// OvlPolygon.cpp

// OvlPolygon contours represent set of lines from lat/lon pair to lat/lon
// pair which are closed.  The lines will appear as straight lines on the 
// screen (unlike the rhumb line or great circle lines).  OvlPolygons can
// also be filled.  The fill type can be set with set_fill_type()

#include "stdafx.h"
#include "ovlutil.h"

#include "map.h"
#include "mem.h"
#include "mapx.h"
#include "GeographicContourIterator.h"
#include "ovl_mgr.h"


// Constructor
OvlPolygon::OvlPolygon()
{
}

OvlPolygon::~OvlPolygon()
{
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlPolygon::prepare_for_redraw(MapProj* map)
{
	if (map == NULL)
		return FAILURE;

   COvlkitUtil ovlkit_util;
   CPoint p1, p2, p1_wrap, p2_wrap;

   // need at list two geo points
   if (m_vecLat.size() < 3)
      return SUCCESS;

   // delete all points in m_point_list
   delete_points();

   m_fill_point_list.clear();

   // loop through the list of geo points and add screen coordinate
   // points to the point list
   const size_t size = m_vecLat.size();
   for (size_t i=0; i < size - 1; ++i)
   {
      // convert the lat, lon pair of the two vertices into
      // screen coordinates and add to the point list
      d_geo_t geo1 = { m_vecLat[i], m_vecLon[i] };
      d_geo_t geo2 = { m_vecLat[i+1], m_vecLon[i+1] };
      ovlkit_util.get_geo_line_coordinates(map, geo1, geo2, p1, p2, p1_wrap, p2_wrap);
      
      m_point_list.AddTail(new CPoint(p1.x, p1.y));
      m_point_list.AddTail(new CPoint(p2.x, p2.y));
      
      // if the line wrapped around the world
      if (p1_wrap != CPoint(-1, -1))
      {
         m_point_list.AddTail(new CPoint(p1_wrap.x, p1_wrap.y));
         m_point_list.AddTail(new CPoint(p2_wrap.x, p2_wrap.y));
      }
   }

   // close the polygon
   {
      d_geo_t geo1 = { m_vecLat[size - 1], m_vecLon[size - 1] };
      d_geo_t geo2 = { m_vecLat[0], m_vecLon[0] };
      ovlkit_util.get_geo_line_coordinates(map, geo1, geo2, p1, p2, p1_wrap, p2_wrap);
      
      m_point_list.AddTail(new CPoint(p1.x, p1.y));
      m_point_list.AddTail(new CPoint(p2.x, p2.y));

      // if the line wrapped around the world
      if (p1_wrap != CPoint(-1, -1))
      {
         m_point_list.AddTail(new CPoint(p1_wrap.x, p1_wrap.y));
         m_point_list.AddTail(new CPoint(p2_wrap.x, p2_wrap.y));
      }
   }
  
   // create a fill list of point from the pnt_list.  pnt_list contains 
   // redundant points for all but the first and last point.  Need
   // to take this into account when generating the list
   if (m_fill_type != UTIL_FILL_NONE)
   {
      const size_t size = m_vecLat.size();
      for (size_t i=size - 1, j=0; j<size; i = j++)
      {
         CRhumbLinePoints rhumb_line(map, m_vecLat[i], m_vecLon[i], m_vecLat[j], m_vecLon[j], FALSE);
         rhumb_line.MoveFirst();

         double dLat, dLon;
         while (rhumb_line.GetNextPoint(dLat, dLon))
         {
            int sx, sy;
            map->geo_to_surface(dLat, dLon, &sx, &sy);
            POINT p = { sx, sy };
            m_fill_point_list.push_back(p);
         }
      }
   }
   
	// calculate the bounding rect
	int max_x = -999999;
	int min_x = 999999;
	int max_y = -999999;
	int min_y = 999999;
	POSITION position = m_point_list.GetHeadPosition();
	while (position)
	{
		CPoint *point = m_point_list.GetNext(position);
		if (point->x > max_x)
			max_x = point->x;
		if (point->x < min_x)
			min_x = point->x;
		if (point->y > max_y)
			max_y = point->y;
		if (point->y < min_y)
			min_y = point->y;
	}
	m_rect = CRect(min_x,min_y,max_x,max_y);
	
	// adjust the bounding rect based on the line width
	int color, style, width;
	m_pen.get_foreground_pen(color, style, width);
	m_rect.InflateRect(width + 2, width + 2);

   m_allow_redraw = TRUE;

   return SUCCESS;
}

// returns TRUE if the class name is GeoSegment
boolean_t OvlPolygon::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlPolygon") == 0)
      return TRUE;
   
   return OvlContour::is_kind_of(class_name);
}

// ************************************************************** //
// OvlContour overrides - these are necessary to support the fill //
// ************************************************************** //

int OvlPolygon::view_draw(MapProj* map, CDC* dc)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	
	// fill the polygon if we are not on world overview
	int surface_width, surface_height;
	int pixels_around_world; 
	map->get_surface_size(&surface_width, &surface_height);
	map->get_pixels_around_world(&pixels_around_world);
	if (surface_width < (pixels_around_world / 2))
	{
		COLORREF color;
		int style, width;
		m_pen.get_background_pen(color, style, width);
		
		futil->fill_polygon(dc, &m_fill_point_list[0], m_fill_point_list.size(), color, 
			m_fill_type, surface_width, surface_height);
	}
	
	return OvlContour::view_draw(map, dc);
}

int OvlPolygon::view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
	
	// fill the polygon if we are not on world overview
	int surface_width, surface_height;
	int pixels_around_world; 
	pMap->get_surface_size(&surface_width, &surface_height);
	pMap->get_pixels_around_world(&pixels_around_world);
	if (surface_width < (pixels_around_world / 2) && m_fill_type != UTIL_FILL_NONE)
	{
      COLORREF color;
		int style, width;
		m_pen.get_background_pen(color, style, width);

      if (m_brush == NULL) // || m_brush->GetColor() != color)
         m_brush = gc->create_brush(color, (BrushStyleEnum)m_fill_type);

      gc->DrawPolyLine(NULL, m_brush, 
         CLineSegmentResizableArray(&m_fill_point_list[0], m_fill_point_list.size()));
	}

   return OvlContour::view_draw(pMap, gc);
}

void OvlPolygon::set_fill_type(int fill_type) 
{ 
   if (m_fill_type != fill_type)
   {
      m_fill_type = fill_type; 
      m_brush = NULL;
   }
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlPolygon::invalidate(boolean_t erase_background /*= FALSE*/)
{
   OVL_get_overlay_manager()->invalidate_rect(m_rect);
}

//
//
//
// OvlEllipse contours represent an ellipse centered at a lat/lon pair 

// Constructor
OvlEllipse::OvlEllipse()
{
   m_fill_point_list.resize(80);
}

// Destructor
OvlEllipse::~OvlEllipse()
{
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlEllipse::prepare_for_redraw(MapProj* map)
{
	if (map == NULL)
		return FAILURE;

   CFvwUtil *futil = CFvwUtil::get_instance();

   // delete all points in m_point_list
   delete_points();

   CGeoEllipsePoints geoEllipse(m_lat, m_lon, m_vert_km * 1000.0, m_horz_km * 1000.0, m_angle);
   CGeographicContourLineSegments lineSegments(map, geoEllipse);
   lineSegments.MoveFirst();
   int x1, y1, x2, y2;
   while (lineSegments.GetNextSegment(x1, y1, x2, y2))
   {
      m_point_list.AddTail(new CPoint(x1, y1));
      m_point_list.AddTail(new CPoint(x2, y2));
   }

   m_fill_point_list.clear();

   double dLat, dLon;
   geoEllipse.MoveFirst();
   while (geoEllipse.GetNextPoint(dLat, dLon))
   {
      int x, y;
      map->geo_to_surface(dLat, dLon, &x, &y);
      m_fill_point_list.push_back(CPoint(x, y));
   }

	// calculate the bounding rect
	int max_x = -999999;
	int min_x = 999999;
	int max_y = -999999;
	int min_y = 999999;
	POSITION position = m_point_list.GetHeadPosition();
	while (position)
	{
		CPoint *point = m_point_list.GetNext(position);
		if (point->x > max_x)
			max_x = point->x;
		if (point->x < min_x)
			min_x = point->x;
		if (point->y > max_y)
			max_y = point->y;
		if (point->y < min_y)
			min_y = point->y;
	}
	m_rect = CRect(min_x,min_y,max_x,max_y);
	
	// adjust the bounding rect based on the line width
	int color, style, width;
	m_pen.get_foreground_pen(color, style, width);
	m_rect.InflateRect(width + 2, width + 2);
   
   m_allow_redraw = TRUE;

   return SUCCESS;
}

// returns TRUE if the class name is GeoSegment
boolean_t OvlEllipse::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlEllipse") == 0)
      return TRUE;
   
   return OvlPolygon::is_kind_of(class_name);
}
