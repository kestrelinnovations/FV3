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



// schartdh.cpp
// Implementation of CStripChartDoghouse class

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/schartdh.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/mem.h"
#include "FalconView/include/fvwutil.h"
#include "FalconView/include/ovl_mgr.h"

// this project's headers

CStripChartDoghouse::CStripChartDoghouse(C_overlay* pParentOverlay) :
   m_pParentOverlay(pParentOverlay)
{
   m_lpPoints = NULL;
   m_lpPolyCounts = NULL;
   clear();
}

CStripChartDoghouse::~CStripChartDoghouse()
{
   clear();
}

// draw the doghouse polygons
int CStripChartDoghouse::draw(CDC *pDC)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // if there are no polygons to draw there is nothing to do
   if (m_num_polys == 0)
      return SUCCESS;

   // create and select the pen and null brush
   CPen pen(PS_SOLID, 2, futil->code2color(m_color));
   CPen *old_pen;
   old_pen = reinterpret_cast<CPen *>(pDC->SelectObject(&pen));
   CBrush* old_brush =
      reinterpret_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH));

   // if there is one polygon, use a call to Polygon since PolyPolygon
   // requires that we have two or more polygons
   if (m_num_polys == 1)
      pDC->Polygon(m_lpPoints, m_lpPolyCounts[0]);
   else
      pDC->PolyPolygon(m_lpPoints, m_lpPolyCounts, m_num_polys);

   // restore the old pen
   pDC->SelectObject(old_pen);
   pDC->SelectObject(old_brush);

   return SUCCESS;
}

// hit_test will return TRUE if the given point is on any doghouse
// polygon
boolean_t CStripChartDoghouse::hit_test(POINT point)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // if there are no polygons there is nothing to test
   if (m_num_polys == 0)
      return FALSE;

   // for each polygon
   POINT *poly_pnt_lst = m_lpPoints;
   for (int i = 0; i < m_num_polys; i++)
   {
      // if the given point is within the set threshold of a polygon
      if (futil->point_on_polygon(point, /*threshold =*/ 3, poly_pnt_lst,
         m_lpPolyCounts[i]))
         return TRUE;

      // point to the next polygon in the list
      poly_pnt_lst += m_lpPolyCounts[i];
   }

   return FALSE;
}

// invalidate will erase the part of the screen covered by the doghouses
void CStripChartDoghouse::invalidate(boolean_t erase_background)
{
   OVL_get_overlay_manager()->InvalidateOverlay(m_pParentOverlay);
}

// add a polygon to the list of polygons
int CStripChartDoghouse::add_polygon(POINT point[6], int num_vertices)
{
   // add the list of points to m_lpPoints
   POINT *pnt;
   if (m_lpPoints == NULL)
   {
      m_num_points = num_vertices;
      m_lpPoints =
         reinterpret_cast<POINT *>(MEM_malloc(sizeof(POINT) * m_num_points));
      pnt = m_lpPoints;
   }
   else
   {
      m_lpPoints = reinterpret_cast<POINT *>(MEM_realloc(m_lpPoints,
         sizeof(POINT) * (m_num_points + num_vertices)));
      pnt = &m_lpPoints[m_num_points];
      m_num_points += num_vertices;
   }
   for (int i = 0; i < num_vertices; i++)
      *pnt++ = point[i];

   // add to the list of number points in each polygon
   if (m_lpPolyCounts == NULL)
      m_lpPolyCounts = reinterpret_cast<int *>(MEM_malloc(sizeof(m_num_polys)));
   else
      m_lpPolyCounts = reinterpret_cast<int *>
      (MEM_realloc(m_lpPolyCounts, sizeof(m_num_polys)*(m_num_polys+1)));
   m_lpPolyCounts[m_num_polys] = num_vertices;

   // increment the number of polygons
   m_num_polys++;

   return SUCCESS;
}

// clear the list of polygons
void CStripChartDoghouse::clear(void)
{
   // free the array of POINT objects that define the vertices of the polygons
   if (m_lpPoints != NULL)
   {
      MEM_free(m_lpPoints);
      m_lpPoints = NULL;
   }

   // free the array of integers, each of which specifies the number of points
   // in one of the polygons in the lpPoints array
   if (m_lpPolyCounts != NULL)
   {
      MEM_free(m_lpPolyCounts);
      m_lpPolyCounts = NULL;
   }

   // reset the number of polygons
   m_num_polys = 0;
   m_num_points = 0;
}

// copy operator
CStripChartDoghouse &CStripChartDoghouse::operator=(
   const CStripChartDoghouse &schart_dh)
{
   POINT points[6];
   POINT *pnt = schart_dh.get_lpPoints();

   // clear out current polygons, if any
   clear();

   // for each polygon
   for (int poly_num = 0; poly_num < schart_dh.get_num_polys(); poly_num++)
   {
      // get the number of points in the current polygon
      int num_points = schart_dh.get_num_points(poly_num);

      // copy each point in the current polygon to the points array
      for (int point_num = 0; point_num < num_points; point_num++)
         points[point_num] = *pnt++;

      // add the polygon to this object
      add_polygon(points, num_points);
   }

   return *this;
}
