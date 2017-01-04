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

#if !defined(AFX_CONTOUR_PP_H__E8B75ADF_0EA4_4176_8240_FE58935FED1A__INCLUDED_)
#define AFX_CONTOUR_PP_H__E8B75ADF_0EA4_4176_8240_FE58935FED1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// contour_pp.h : header file
//

#include <afxdlgs.h>
#include "..\resource.h"
#include <afxdtctl.h>
#include "optndlg.h"

/////////////////////////////////////////////////////////////////////////////
// CContourOptionsPage dialog

class CContourOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CContourOptionsPage)

// Construction
public:
	CContourOptionsPage();
	~CContourOptionsPage();

// Dialog Data
	//{{AFX_DATA(CContourOptionsPage)
	enum { IDD = IDD_OVL_CONTOUR };
	CButton	   m_overlay_on;
	CComboBox	m_DisplayThreshold;
	CComboBox	m_LabelThreshold;
   CComboBox   m_IntervalUnit;      // 0-meters, 1-feet
   CComboBox   m_Divisions;         // number of divisions for minor contours
   CButton     m_Fixed;
   CSliderCtrl m_Density;
	CButton		m_ShowLabels;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CContourOptionsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CContourOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnModified();
	afx_msg void OnColorChange();
	afx_msg void OnLabelFont();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   //afx_msg void OnMethod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DrawColorButton();
   void DrawContourExample(HDC hDC);
   static void ApplyLabelFontOptions( CFontDlg *font_dlg );

private:

   float    m_MajorInterval;  // Contour line interval
   int      m_LastUnit;       // Used to detect/react to unit changes
	COLORREF m_color;          // Contour line color
   int      m_Source;         // DTED Source selection (0-auto)
};


class CContourLinesPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CContourOptionsPage(); }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTOUR_PP_H__E8B75ADF_0EA4_4176_8240_FE58935FED1A__INCLUDED_)
