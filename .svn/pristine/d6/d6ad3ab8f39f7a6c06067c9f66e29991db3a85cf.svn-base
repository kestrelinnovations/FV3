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

#if !defined(AFX_AR_DLG_H__2FE237D0_5236_4E3B_B1D0_A80C3A46F134__INCLUDED_)
#define AFX_AR_DLG_H__2FE237D0_5236_4E3B_B1D0_A80C3A46F134__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ar_dlg.h : header file
//

#include "ar_edit.h"


/////////////////////////////////////////////////////////////////////////////
// CUserArAnchorParamsDlg dialog

class CUserArAnchorParamsDlg : public CDialog
{
protected:
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
// Construction
public:
	CUserArAnchorParamsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserArAnchorParamsDlg)
	enum { IDD = IDD_USER_AR_ANCHOR_PARAMS_DLG };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/FalconView_Overview.htm";}
	CString	m_length_str;
	CString	m_width_str;
	//}}AFX_DATA


	C_ar_obj *m_obj;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserArAnchorParamsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserArAnchorParamsDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void set_route_ptr(C_ar_obj *obj);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AR_DLG_H__2FE237D0_5236_4E3B_B1D0_A80C3A46F134__INCLUDED_)