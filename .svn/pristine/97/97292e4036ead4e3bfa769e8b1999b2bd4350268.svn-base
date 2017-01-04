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

// ContourPoint.h: interface for the CContourPoint class.
// Daniel V. Longhurst
// March 7, 2002
//
// A contour point belongs to 2 contour "cells".  Each cell is a 
// grouping of 4 elevation points.  Contour lines pass through these
// cells as they traverse the terrain.
// 
// Create separate lists for these cells, each list being all the 
// points that are found to lie on a contour level.  Each point is 
// identified by the 2 cell ids that join at the edge the point is on
// Note below Edge1 belongs to this cell and the cell above it.
//
//       ElevationTL    Edge1    Elevation TR
//                   100      200
//             Edge2              Edge 4
//                   200      100
//       ElevationBL    Edge3    Elevation BR
//       
// The Cell ID is a DWORD.  The upper word is an _int16 which is the
// column number, while the lower word is the row number.  The contour
// level is not noted on the point because that should be apparrent by 
// keeping all points of a single level in a single list
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTOURPOINT_H__70654221_8B93_4AEC_8FA4_E4BC85C23E05__INCLUDED_)
#define AFX_CONTOURPOINT_H__70654221_8B93_4AEC_8FA4_E4BC85C23E05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NUM_NEIGHBORS 8


struct CellID
{
   CellID() { ID = 0; }
   CellID(unsigned _int16 r, unsigned _int16 c)
   {
      Row = r;
      Col = c;
   }

   // Use the x/y within the sub-tile for the ID.
   union{
      struct 
      {
         unsigned _int16 Col;
         unsigned _int16 Row;
      };

      unsigned long ID;
   };

   bool operator<(const CellID& cell) const
   {
      return ID < cell.ID;
   }
};

struct CContourPoint  
{
public:
   CContourPoint();
   CContourPoint(int CellRow1, int CellCol1, int CellRow2, 
                 int CellCol2, double Lat, double Lon);

   CellID m_CellID1;
   CellID m_CellID2;

   // Position of the contour point
   double m_Lat;
   double m_Lon;
   int   m_ScreenX;
   int   m_ScreenY;
};


#endif // !defined(AFX_CONTOURPOINT_H__70654221_8B93_4AEC_8FA4_E4BC85C23E05__INCLUDED_)
