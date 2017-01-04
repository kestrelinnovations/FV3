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



#if !defined(AFX_MDMTARGET_H__1B919EB6_643A_11D2_B07F_00104B8C938E__INCLUDED_)
#define AFX_MDMTARGET_H__1B919EB6_643A_11D2_B07F_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mdmtarg.h : header file
//
#include "mdmlctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMTarget dialog

class CMDMTarget : public CDialog
{
   CString m_TargetPath;
   long m_TargetID;

// Construction
public:
	CMDMTarget(CWnd* pParent = NULL);   // standard constructor
   void Refresh(void);

   CString GetTargetPath() { return m_TargetPath; };
   long GetTargetID() { return m_TargetID; };

// Dialog Data
	//{{AFX_DATA(CMDMTarget)
	enum { IDD = IDD_MDM_TARGET };
	CButton	m_HelpBtnCtrl;
	CMDMListCtrl	m_TargetListCtrl;
	CButton	m_OKBtnCtrl;
	CButton	m_CancelBtnCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDMTarget)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMDMTarget)
	afx_msg void OnMdmtargetOk();
	afx_msg void OnMdmtargetCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelpMdmTarget();
	afx_msg void OnDblclkMdmtargetTargetlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMTARGET_H__1B919EB6_643A_11D2_B07F_00104B8C938E__INCLUDED_)
