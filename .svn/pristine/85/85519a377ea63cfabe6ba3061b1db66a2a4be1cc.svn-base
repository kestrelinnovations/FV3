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

#if !defined(AFX_GRIDMAPOPTIONSPAGE_H__B0DD7589_4B34_11D3_8AA0_00105A9B4BDB__INCLUDED_)
#define AFX_GRIDMAPOPTIONSPAGE_H__B0DD7589_4B34_11D3_8AA0_00105A9B4BDB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// grid_pp.h : header file
//
#include "..\fontdlg.h"
#include "optndlg.h"
#include "..\lwidthCB.h"
#include "..\colorCB.h"

/////////////////////////////////////////////////////////////////////////////
// GridMapOptionspage dialog

class GridMapOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(GridMapOptionsPage)

// Construction
public:
	GridMapOptionsPage();
	~GridMapOptionsPage();

static void on_apply_label_font_cb( CFontDlg *dlg );

// Dialog Data
	//{{AFX_DATA(GridMapOptionsPage)
	enum { IDD = IDD_OVL_GRIDMAP_DLG };
	CSliderCtrl	m_tick_length;
	CLWidthCB	m_gridline_thickness_cb;
	CColorCB	m_gridline_color_cb;
	BOOL	m_show_minor_gridlines;
	BOOL	m_show_labels_on_major_line;
	BOOL	m_show_tick_marks;
	BOOL	m_grid_map_on;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(GridMapOptionsPage)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   void enable_based_on_grid_type();

	// Generated message map functions
	//{{AFX_MSG(GridMapOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnColorChange();
	afx_msg void OnThicknessChange();
	afx_msg void OnToggle();
	afx_msg void OnLabelFont();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int m_old_grid_type;
};

class CCoordinateGridPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new GridMapOptionsPage(); }
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDMAPOPTIONSPAGE_H__B0DD7589_4B34_11D3_8AA0_00105A9B4BDB__INCLUDED_)
