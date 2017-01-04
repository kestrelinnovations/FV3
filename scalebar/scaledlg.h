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



// scaledlg.h : header file
//

#ifndef SCALEDLG_H
#define SCALEDLG_H 1

#include "..\resource.h"
#include "optndlg.h"

class CScaleBarOptionsPage: public CPropertyPage
{
	DECLARE_DYNCREATE(CScaleBarOptionsPage)

	// Construction
public:
	CScaleBarOptionsPage();
	~CScaleBarOptionsPage() { }

	// Dialog Data
	//{{AFX_DATA(CScaleBarOptionsPage)
	enum { IDD = IDD_SCALEBAR_DLG_ALT };
		// NOTE: the ClassWizard will add data members here
	CButton	m_scalebar_on;

   CButton m_vert;
	CButton m_horz;
	CButton m_both;

	CButton m_size_small;
	CButton m_size_large;

	CButton m_units_NM_YD;
	CButton m_units_NM_FT;
	CButton m_units_KM_M;
	//}}AFX_DATA

	int m_units;
	int m_size;
	int m_orientation;
	int m_color;
	int m_backcolor;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleBarOptionsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScaleBarOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnColor();
	afx_msg void OnColorBack();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DrawColorButton();
	void DrawBackColorButton();
};

class CScaleBarPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CScaleBarOptionsPage(); }
};

#endif  // ifndef SCALEDLG_H