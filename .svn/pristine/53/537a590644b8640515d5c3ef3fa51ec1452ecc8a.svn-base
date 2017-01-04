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



// line.cpp

#include "stdafx.h"
#include "fvwutil.h"
#include "err.h"
#include "map.h"
#include "graphics.h"
#include "LineSegmentRenderer.h"

#define UTIL_STANDARD_STYLE_TYPE ((unsigned char)0x01)
#define UTIL_NORMAL_STYLE_TYPE   ((unsigned char)0x02)

// Constructor 
UtilDraw::UtilDraw(CDC *dc) : 
m_clip_rect(SHRT_MIN, SHRT_MIN, SHRT_MAX, SHRT_MAX),
GraphicsUtilities(dc->GetSafeHdc())
{
	m_cdc = dc;
}

// Select the pen to be used by the drawing member functions.  If the DC
// this object is not defined this function returns FAILURE.  You must call
// this function at least once before calling a drawing member of this 
// function.  Set the optional parameter is_background_line to TRUE if you are
// drawing a non-solid line style and the required pattern is the pattern produced
// if the width of the line was actually two less.
int UtilDraw::set_pen(int color, int style, int width, boolean_t is_background_line /*=FALSE*/)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	return UtilDraw::set_pen(futil->code2color(color), style, width, is_background_line);
}  

int UtilDraw::set_pen(COLORREF color, int style, int width, BOOL is_background_line /*= FALSE*/)
{
	// save style and width
	m_style = style;
	return GraphicsUtilities::set_pen(color,style,width,is_background_line);
}


// Set the DC used by this object.  The DC must be set in order for the
// set_pen() or the drawing member functions to work.  If the DC is set in
// the constructor, you don't have to call this function.
int UtilDraw::set_dc(CDC *dc)
{
   if (dc == NULL)
      return FAILURE;

   m_cdc = dc;
   GraphicsUtilities::set_dc(dc->GetSafeHdc());

   return SUCCESS;
}

// Set the clip rectangle used to avoid passing integer values outside of
// the range of a SHORT to the WIN32 API, because the API will not properly 
// clip large values on Windows 95 (and probably Windows 98 too).
int UtilDraw::set_clip_rect(MapProj *map)
{
   int width, height;

   if (map == NULL)
   {
      ERR_report("NULL MapProj");
      return FAILURE;
   }

   // use height and width to define clipping region
	map->get_surface_size(&width, &height);

   if (width+100 > SHRT_MAX || height+100 > SHRT_MAX)
   {
      ERR_report("Surface size is too large for a short.");
      return FAILURE;
   }

	m_clip_rect.SetRect(-100, -100, width+100, height+100);

   return SUCCESS;
}

// *************************************************************
// *************************************************************

// drawing functions
int UtilDraw::draw_line(MapProj* map, CPoint p1, CPoint p2)
{
   return draw_line(map, p1.x, p1.y, p2.x, p2.y);
}

// *************************************************************
// *************************************************************

int UtilDraw::draw_line(MapProj* map, int x1, int y1, int x2, int y2)
{
   // if a non-NULL map object was passed, set the clip rect from it
	if (map != NULL)
	{
      if (set_clip_rect(map) != SUCCESS)
      {
         ERR_report("set_clip_rect() failed.");
         return FAILURE;
      }
   }

	return draw_line(x1, y1, x2, y2);
}

// *************************************************************
// *************************************************************

int UtilDraw::draw_line(int x1, int y1, int x2, int y2)
{
	return GraphicsUtilities::draw_line(x1, y1, x2, y2);
}


// *************************************************************
// *************************************************************

int UtilDraw::draw_arc(boolean_t fill, int ul_x, int ul_y, int lr_x, int lr_y,
   int from_x, int from_y, int to_x, int to_y)
{
   return GraphicsUtilities::draw_arc(fill, ul_x, ul_y, lr_x, lr_y, from_x, from_y, to_x, to_y);
}

int UtilDraw::draw_circle(boolean_t fill, int center_x, 
   int center_y, int radius)
{
   return GraphicsUtilities::draw_circle(fill, center_x, center_y, radius);
}

int UtilDraw::draw_ellipse(boolean_t fill, int ul_x, int ul_y, 
   int lr_x, int lr_y)
{
   return GraphicsUtilities::draw_ellipse(fill, ul_x, ul_y, lr_x, lr_y);
}

// *************************************************************
// *************************************************************

int UtilDraw::draw_rectangle(boolean_t fill, int ul_x, int ul_y, int lr_x, 
   int lr_y)
{
   return GraphicsUtilities::draw_rectangle(fill, ul_x, ul_y, lr_x, lr_y);
}

int UtilDraw::set_pixel(int x, int y, int color)
{
   m_cdc->SetPixelV(x, y, GRA_get_color_ref(color));
   return SUCCESS;
}

// Returns TRUE if the given rectangle intersects m_clip_rect, FALSE 
// otherwise.
boolean_t UtilDraw::intersect_clip_rect(CRect &rect)
{
   CRect intersect;

   if (rect.IsRectEmpty())
      return FALSE;

   return intersect.IntersectRect(m_clip_rect, rect);
}

// draw_diamond_at() takes a center point and a rotation 
// and draws a diamond (rotated rectangle) at the given point
void UtilDraw::draw_diamond(int center_x, int center_y, double theta)
{
   CPoint points[4];
   CFvwUtil *futil = CFvwUtil::get_instance();
   int old_style = m_style;
   
   futil->rotate_pt(center_x-GetWidth()/2+1,center_y-GetWidth()/2+1,
      &points[0].x, &points[0].y,theta,center_x,center_y);
   
   futil->rotate_pt(center_x+GetWidth()/2-1,center_y-GetWidth()/2+1,
      &points[1].x, &points[1].y,theta,center_x,center_y);
   
   futil->rotate_pt(center_x+GetWidth()/2-1,center_y+GetWidth()/2-1,
      &points[2].x, &points[2].y,theta,center_x,center_y);
   
   futil->rotate_pt(center_x-GetWidth()/2+1,center_y+GetWidth()/2-1,
      &points[3].x, &points[3].y,theta,center_x,center_y);
   
   // Draw the calculated polygon 
   int old_width = GetWidth();
   
   set_pen(GetColor(), m_style, 1, IsBackgroundLine());
   CBrush brush(GetColor());
   CBrush *oldbrush = (CBrush*) m_cdc->SelectObject(&brush);
   
   // if cpt[i] = cpt[j] for all i,j=0,..,4 Polygon will not 
   // draw anything.  To correct this, draw a line of 0 length
   if (points[0] == points[1] && points[1] == points[2] && 
      points[2] == points[3] && points[3] == points[0])
   {
      set_pen(GetColor(), UTIL_LINE_SOLID, 2, IsBackgroundLine());
      draw_line(points[0].x, points[0].y, points[0].x, points[0].y);
   }
   else
      m_cdc->Polygon(points, 4);
   
   set_pen(GetColor(), old_style, old_width, IsBackgroundLine());
   m_cdc->SelectObject(oldbrush);
   brush.DeleteObject();
}

// draw the "No Map Data" symbol
void UtilDraw::draw_no_map_data_symbol(CPoint center, int radius)
{
   draw_circle(/*fill=*/ FALSE, center.x, center.y, radius);
   
   draw_line(NULL, CPoint((int)(center.x + 0.70710678118654*radius),
      (int)(center.y - 0.70710678118654*radius)), 
      CPoint((int)(center.x -0.70710678118654*radius),
      (int)(center.y + 0.707106781186548*radius)));
}

// draw the 'page center' symbol
void UtilDraw::draw_page_center_symbol(CPoint center)
{
   draw_line(NULL, center.x + 4, center.y, center.x + 9, center.y);
   draw_line(NULL, center.x - 4, center.y, center.x - 9, center.y);
   draw_line(NULL, center.x, center.y + 4, center.x, center.y + 9);
   draw_line(NULL, center.x, center.y - 4, center.x, center.y - 9);
}


// CLineSegmentList - a MFC CList of CPoints.  Every two points defines a line
//    segment (i.e., vertices are duplicated)
CLineSegmentList::CLineSegmentList(CList <CPoint *, CPoint *> &listPoints) :
   m_listPoints(listPoints),
   m_position(NULL)
{
   if ( (m_listPoints.GetCount() % 2) != 0)
      ERR_report("Point list must contain an even number of points");
}

void CLineSegmentList::MoveFirst()
{
   m_position = m_listPoints.GetHeadPosition();
}

BOOL CLineSegmentList::GetNextSegment(int &x1, int &y1, int &x2, int &y2)
{
   if (m_position == NULL)
      return FALSE;

   CPoint *p1 = m_listPoints.GetNext(m_position);
   if (m_position == NULL)
   {
      ERR_report("Point list must contain an even number of points");
      return FALSE;
   }

	CPoint *p2 = m_listPoints.GetNext(m_position);

   x1 = p1->x;
   y1 = p1->y;
   x2 = p2->x;
   y2 = p2->y;

   return TRUE;
}

// CLineSegmentResizableArray - an array of CPoints.  Vertices are not duplicated
CLineSegmentResizableArray::CLineSegmentResizableArray(CPoint *pPoints, int nNumPoints) :
   m_nCurrentIndex(0),
   m_bClosed(TRUE)
{
   for (int i=0; i<nNumPoints; ++i)
      m_vecPoints.push_back(pPoints[i]);
}

CLineSegmentResizableArray::CLineSegmentResizableArray(POINT *pPoints, int nNumPoints)
{
   for (int i=0; i<nNumPoints; ++i)
      m_vecPoints.push_back(pPoints[i]);
}

void CLineSegmentResizableArray::MoveFirst()
{
   m_nCurrentIndex = 0;
}

BOOL CLineSegmentResizableArray::GetNextSegment(int &x1, int &y1, int &x2, int &y2)
{
   // Automatically close the polygon
   if ((m_nCurrentIndex + 1) == m_vecPoints.size())
   {
      if (m_bClosed)
      {
         x1 = m_vecPoints[m_nCurrentIndex].x;
         y1 = m_vecPoints[m_nCurrentIndex].y;
         x2 = m_vecPoints[0].x;
         y2 = m_vecPoints[0].y;
         m_nCurrentIndex++;

         return TRUE;
      }
      // otherwise, polyline is not closed
      return FALSE;
   }

   if ((m_nCurrentIndex + 1) > m_vecPoints.size())
      return FALSE;

   x1 = m_vecPoints[m_nCurrentIndex].x;
   y1 = m_vecPoints[m_nCurrentIndex].y;
   x2 = m_vecPoints[m_nCurrentIndex + 1].x;
   y2 = m_vecPoints[m_nCurrentIndex + 1].y;

   m_nCurrentIndex++;

   return TRUE;
}


// CLineSegmentArrayIndices

CLineSegmentArrayIndices::CLineSegmentArrayIndices(CPoint *pPoints, int *pIndices, int nNumPoints) :
   m_pPoints(pPoints),
   m_pIndices(pIndices),
   m_nNumPoints(nNumPoints),
   m_nCurrentIndex(0)
{
}
   
void CLineSegmentArrayIndices::MoveFirst()
{
   m_nCurrentIndex = 0;      
}

BOOL CLineSegmentArrayIndices::GetNextSegment(int &x1, int &y1, int &x2, int &y2)
{
   if ((m_nCurrentIndex + 1) >= m_nNumPoints)
      return FALSE;

   x1 = m_pPoints[m_pIndices[m_nCurrentIndex]].x;
   y1 = m_pPoints[m_pIndices[m_nCurrentIndex]].y;
   x2 = m_pPoints[m_pIndices[m_nCurrentIndex + 1]].x;
   y2 = m_pPoints[m_pIndices[m_nCurrentIndex + 1]].y;

   m_nCurrentIndex++;

   return TRUE;
}
