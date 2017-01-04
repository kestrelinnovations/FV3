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

#if !defined(AFX_PREDICTIVEPATHOPTIONSPAGE_H__2021EBE1_3976_4EA2_844C_1BF306111C6D__INCLUDED_)
#define AFX_PREDICTIVEPATHOPTIONSPAGE_H__2021EBE1_3976_4EA2_844C_1BF306111C6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PredictivePathOptionsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPredictivePathOptionsPage dialog

#include "..\resource.h"
#include "common.h"
#include "button.h"
#include "gpsstyle.h"

class C_overlay;
class CPredictivePathProperties;

class CPredictivePathOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPredictivePathOptionsPage)

// Construction
public:
	CPredictivePathOptionsPage(CPredictivePathProperties* pProperties = NULL, C_overlay* pOverlay = NULL);
	~CPredictivePathOptionsPage();

   void InitializeControls();

   bool m_bApplyImmediately;

   void store_values();

// Dialog Data
	//{{AFX_DATA(CPredictivePathOptionsPage)
	enum { IDD = IDD_GPS_PREDICTIVE_PATH };
	CButton	m_on_ctrl;
	CButton m_center_line_ctrl;
	CFVWColorButton   m_icon_fgC;
	CSliderCtrl	m_slider_ctrl[4];
	CEdit m_edit_ctrl[4];
	int m_edit_value[4];
	CStatic m_units_ctrl[4];
	CComboBox m_num_dots_ctrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPredictivePathOptionsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnApply();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPredictivePathOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnModified();
	afx_msg void OnPredictiveIconColor();
	afx_msg void OnNumDotsChanged();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CPredictivePathProperties *m_prop;
	C_overlay *m_pOverlay;
	boolean_t m_modified;
	COLORREF m_icon_fg;

	void hide_trend_dot_controls();
	void check_slider_positions();
};

class CMovingMapPredictivePathOptionsPage : public CPredictivePathOptionsPage
{
public:
   CMovingMapPredictivePathOptionsPage(CPredictivePathProperties* pProperties = NULL, C_overlay* pOverlay = NULL) :
      CPredictivePathOptionsPage(pProperties, pOverlay)
   {
   }
protected:
   virtual BOOL OnApply();
};

class CMovingMapPredictivePathPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() 
   { 
      return new CMovingMapPredictivePathOptionsPage(CMovingMapRootPropertyPage::GetProperties()->GetPredictivePathProperties()); 
   }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREDICTIVEPATHOPTIONSPAGE_H__2021EBE1_3976_4EA2_844C_1BF306111C6D__INCLUDED_)
