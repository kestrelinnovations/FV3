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

// ContourLists.h: interface for the CContourLists class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTOURLISTS_H__A474BA0B_171A_4D9F_9985_D90EFC3882E2__INCLUDED_)
#define AFX_CONTOURLISTS_H__A474BA0B_171A_4D9F_9985_D90EFC3882E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct CContourPoint;
#include <list>

typedef CList<CContourPoint *, CContourPoint *&> CContourList;

typedef std::multimap<CellID, CContourPoint *> ContourMultiMap;
typedef std::multimap<CellID, CContourPoint *>::iterator ContourMultiMapIt;

class CContourLists  
{
public:
	CContourLists();
	virtual ~CContourLists();

   void ClearLists();

   void TraceContours(short *ElevMap, int width, int height, float interval,
                      double SW_Lat, double SW_Lon, double NE_Lat, double NE_Lon);

   // Returns false if contours are valid (draw interrupted)
   bool TraceClearanceContours(short *ElevMap, int width, int height, float WarnAlt, float CautionAlt, float OKAlt,
                      double SW_Lat, double SW_Lon, double NE_Lat, double NE_Lon);

   void TraceSingleContour(short *ElevMap, int width, int height, float alt1,
                      double SW_Lat, double SW_Lon, double NE_Lat, double NE_Lon);

   void AddPoint(int CellRow1, int CellCol1, int CellRow2, 
                 int CellCol2, double Lat, double Lon, 
                 int contour);

   bool PointsConnected(CContourPoint* p1, CContourPoint* p2);

   bool m_DataReady; // Flag showing if contours have been traced.

   // There is a separate ContourMultiMap for each contour level
   CMap<int, int&, ContourMultiMap*, ContourMultiMap *&> m_contourMultiMaps;

   CArray<CContourList*, CContourList*&> m_Lines;
   CArray<int, int&> m_Levels;

protected:
   void SortPoints();
   void SortPoints(ContourMultiMap* pMultiMapContourPoints, int nContourLevel);
   void CheckForLoopBack(CContourList* pCurrentList, std::list<CContourPoint *>& pointStack);
   BOOL GetNextNeighbor(ContourMultiMap* pMultiMapContourPoints, CContourPoint *p, ContourMultiMapIt& itNeighbor);
};

#endif // !defined(AFX_CONTOURLISTS_H__A474BA0B_171A_4D9F_9985_D90EFC3882E2__INCLUDED_)
