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



#pragma once

#include "common.h"

//
// Definition File:		overlay_pp.h
//

// Purpose:					contains class definitions for the base class overlay 
//								options property page.

#ifndef OVERLAY_PP_H
#define OVERLAY_PP_H 1

// This class is designed to be the base class for static overlay property pages.  Do not use this class 
// for file overlays.

class COverlayPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COverlayPropertyPage)

// Construction
public:
	COverlayPropertyPage(GUID overlayDescGuid = GUID_NULL, UINT pp_id=0);
	~COverlayPropertyPage();

	virtual void toggle_control_states(boolean_t turn_on);

   // the type of overlay of the derived class
	GUID m_overlayDescGuid;

// Dialog Data
	//{{AFX_DATA(COverlayPropertyPage)
	CComboBox	m_display_threshold_control;
	CComboBox	m_threshold_control;
	CString		m_label_threshold;
	CString		m_display_threshold;
	BOOL			m_overlay_toggle;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COverlayPropertyPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COverlayPropertyPage)
	virtual BOOL OnInitDialog();
	virtual void OnToggleOverlay();
	afx_msg void OnModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif 
