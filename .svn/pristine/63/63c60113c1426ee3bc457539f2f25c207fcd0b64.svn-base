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

#if !defined(AFX_SKYVIEWOPTIONSPAGE_H__D9A86CD4_64BB_42B5_9DBE_2B7C10477682__INCLUDED_)
#define AFX_SKYVIEWOPTIONSPAGE_H__D9A86CD4_64BB_42B5_9DBE_2B7C10477682__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkyViewOptionsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkyViewOptionsPage dialog

#include "../MovingMapOverlay/button.h"			// for CFVWColorButton
#include "ovlelem.h"					// for OvlFont
#include "optndlg.h"

class CFontDlg;

class SkyViewProperties
{
public:
	SkyViewProperties();
	int initialize_from_registry();
	int save_to_registry();

	CString m_symbol_type;

	BOOL m_show_loaded_area;
	COLORREF m_loaded_area_color;

	BOOL m_show_visible_area;
	COLORREF m_visible_area_color;

	BOOL m_hide_vehicle_when_attached;
	BOOL m_show_altitude;
	OvlFont m_altitude_font;
};

class CSkyViewOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSkyViewOptionsPage)

// Construction
public:
	CSkyViewOptionsPage();
	~CSkyViewOptionsPage();

// Dialog Data
	//{{AFX_DATA(CSkyViewOptionsPage)
	enum { IDD = IDD_OVL_SKYVIEW };
	CComboBox	m_ship_type_combo;
	CButton	m_loaded_area_check;
	CFVWColorButton m_loaded_area_color;
	CButton	m_visible_area_check;
	CFVWColorButton m_visible_area_color;
	CButton	m_hide_when_attached_check;
	CButton	m_altitude_check;	
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSkyViewOptionsPage)
	protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSkyViewOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoadedAreaColor();
	afx_msg void OnVisibleAreaColor();
	afx_msg void OnModified();
	afx_msg void OnFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static void on_apply_font(CFontDlg *dlg);
};

class CSkyviewPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CSkyViewOptionsPage(); }
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKYVIEWOPTIONSPAGE_H__D9A86CD4_64BB_42B5_9DBE_2B7C10477682__INCLUDED_)
