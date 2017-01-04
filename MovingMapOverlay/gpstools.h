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



//
// gpstools.h
// 

#ifndef _GPSTOOLS_H_
#define _GPSTOOLS_H_

// foward declarations
class C_gps_trail;

#define GET_POSITION_FROM_MAP 1638
#define PAGE_INDEX_RANGE_BEARING 0
#define PAGE_INDEX_COAST_TRACK 1

/////////////////////////////////////////////////////////////////////////////
// GPSToolsPropertySheet

class GPSToolsPropertySheet: public CPropertySheet
{
   DECLARE_DYNAMIC(GPSToolsPropertySheet)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GPSToolsPropertySheet)
	protected:
   virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
   virtual void PostNcDestroy();
	//{{AFX_MSG(GPSToolsPropertySheet)
	afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
   afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   void set_focus(C_gps_trail *gps_trail, int page_num);
};

#endif