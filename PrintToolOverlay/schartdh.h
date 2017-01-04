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

// schartdh.h
// Header file for CStripChartDoghouse class

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_SCHARTDH_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_SCHARTDH_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/common.h"
// this project's headers
// forward definitions
class C_overlay;

// CStripChartDoghouse stores a list of doghouse polygons associated with
// a given strip chart page.
class CStripChartDoghouse
{
public:
   explicit CStripChartDoghouse(C_overlay* pParentOverlay);
   ~CStripChartDoghouse();

private:
   C_overlay* m_pParentOverlay;

   // color to draw the polygons
   int m_color;

   // points to an array of POINT objects that define the polygons' vertices
   LPPOINT m_lpPoints;
   int m_num_points;

   // points to an array of integers, each of which specifies the number of
   // points in one of the polygons in the lpPoints array
   LPINT m_lpPolyCounts;

   // The number of entries in the lpPolyCounts array
   int m_num_polys;

public:
   // draw the doghouse polygons
   int draw(CDC *pDC);

   // hit_test will return TRUE if the given point is on any doghouse
   // polygon
   boolean_t hit_test(POINT point);

   // invalidate will erase the part of the screen covered by the doghouses
   void invalidate(boolean_t erase_background = FALSE);

   // add a polygon to the list of polygons
   int add_polygon(POINT point[6], int num_vertices);

   // clear the list of polygons
   void clear(void);

   // copy operator
   CStripChartDoghouse &operator=(const CStripChartDoghouse &);

public:
   void set_color(int color)
   {
      m_color = color;
   }

// operations used in the copy operator
private:
   // Note that this passes the object's internal member outside where it can
   // be modified - the const is not guarenteed
   POINT *get_lpPoints(void) const
   {
      return m_lpPoints;
   }

   int get_num_polys(void) const
   {
      return m_num_polys;
   }
   int get_num_points(int poly_num) const
   {
      return m_lpPolyCounts[poly_num];
   }
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_SCHARTDH_H_
