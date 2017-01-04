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

#if !defined(AFX_RBVIEW_H__0390D283_DD81_11D3_80B8_00C04F60B086__INCLUDED_)
#define AFX_RBVIEW_H__0390D283_DD81_11D3_80B8_00C04F60B086__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// rbview.h : header file
//

#include "ovl_d.h"         // definition of C_icon
#include "geo_tool_d.h"    // for d_geo_t type definition

class RangeBearing;

/////////////////////////////////////////////////////////////////////////////
// RangeBearingView window

#define RANGE_BEARING_VIEW_COLOR_KEY RGB(12,34,56)

class RangeBearingView : public CWnd
{
// Construction
public:
	RangeBearingView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RangeBearingView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~RangeBearingView();

	// Generated message map functions
protected:
	//{{AFX_MSG(RangeBearingView)
   afx_msg void OnPaint();
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg void OnSwapDir();
   afx_msg void OnUpdateCommandSwapDir(CCmdUI* pCmdUI);
   afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   double m_range;
   double m_bearing;
   boolean_t m_mag;
   boolean_t m_units_nm;
   CMenu m_menu;
   RangeBearing *m_range_bearing_obj;

public:
   void set_range_bearing(double range, double bearing, boolean_t mag, boolean_t u)
   {
      m_range = range;
      m_bearing = bearing;
      m_mag = mag;
      m_units_nm = u;
      
      Invalidate();
      UpdateWindow();
   }

   // formats the range and bearing and returns a string
   CString get_text();

   void set_range_bearing_obj(RangeBearing *rb_obj);

private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RBVIEW_H__0390D283_DD81_11D3_80B8_00C04F60B086__INCLUDED_)
