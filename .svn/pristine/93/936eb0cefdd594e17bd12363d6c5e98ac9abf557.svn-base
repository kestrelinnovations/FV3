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



#if !defined(AFX_AREDT_PP_H__CFAB60C5_8081_11D2_B5C2_00104B242B2B__INCLUDED_)
#define AFX_AREDT_PP_H__CFAB60C5_8081_11D2_B5C2_00104B242B2B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// aredt_pp.h : header file
//

#include "optndlg.h"

/////////////////////////////////////////////////////////////////////////////
// CArEditOvlOptionsPage dialog

class CArEditOvlOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CArEditOvlOptionsPage)

// Construction
public:
	CArEditOvlOptionsPage();
	~CArEditOvlOptionsPage();

// Dialog Data
	//{{AFX_DATA(CArEditOvlOptionsPage)
	enum { IDD = IDD_OVL_AR_EDIT_DLG };
	BOOL	m_background;
	CString	m_orbit_width_str;
   CString		m_display_threshold;
	//}}AFX_DATA

	int m_color;
	BOOL m_old_background;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CArEditOvlOptionsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CArEditOvlOptionsPage)
	virtual void OnOK();
	afx_msg void OnArEditColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnBackground();
	afx_msg void OnRbNima();
	afx_msg void OnRbSof();
	afx_msg void OnCloseupCbRadius();
	afx_msg void OnKillfocusOrbitWidth();
	afx_msg void OnChangeOrbitWidth();
   afx_msg void OnModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DrawColorButton();

};

class CTrackOrbitPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CArEditOvlOptionsPage(); }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AREDT_PP_H__CFAB60C5_8081_11D2_B5C2_00104B242B2B__INCLUDED_)
