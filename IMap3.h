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

// IMap3.h
//

class Map3 : public CCmdTarget
{
	DECLARE_DYNCREATE(Map3)
   DECLARE_OLECREATE(Map3)

	Map3();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Map3)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~Map3();

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(Map3)
	afx_msg long GetMapDisplayEx(BSTR* pMapSource, double* pScale, int* pScaleUnits, BSTR* pMapSeries,
            double* pCenterLat, double* pCenterLon, double* pRotation, int* pZoomPercent, int* pProjection);
	afx_msg long SetMapDisplayEx(LPCTSTR strMapSource, double dScale, int nScaleUnits, LPCTSTR strMapSeries,
            double dCenterLat, double dCenterLon, double dRotation, int nZoomPercent, int nProjection);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};