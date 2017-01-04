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

// ContourPoint.cpp: implementation of the CContourPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ContourPoint.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContourPoint::CContourPoint()
{
   m_CellID1.ID = 0;
   m_CellID2.ID = 0;
}

CContourPoint::CContourPoint(int CellRow1, int CellCol1, int CellRow2, 
                             int CellCol2, double Lat, double Lon) :
         m_Lat(Lat), m_Lon(Lon), m_ScreenX(-1), m_ScreenY(-1)
{
   m_CellID1.Row = CellRow1;
   m_CellID1.Col = CellCol1;

   m_CellID2.Row = CellRow2;
   m_CellID2.Col = CellCol2;
}   