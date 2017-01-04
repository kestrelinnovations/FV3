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



// expirationdialog.h
 
#if !defined(AFX_EXPIRATIONDIALOG_H__9D2CD344_6FE7_11D1_9C85_0060972D1F5F__INCLUDED_)
#define AFX_EXPIRATIONDIALOG_H__9D2CD344_6FE7_11D1_9C85_0060972D1F5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ExpirationDialog.h : header file
//

#include "StatLink.h"

/////////////////////////////////////////////////////////////////////////////
// CExpirationDialog dialog

class CExpirationDialog : public CDialog
{
// Construction
	CString m_input_buffer;
	BOOL m_lbdown_was_in_url;

public:
	CExpirationDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExpirationDialog)
	enum { IDD = IDD_EXPIRED };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/FalconView_Overview.htm";}
	CString	m_date_code;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpirationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExpirationDialog)
	virtual BOOL OnInitDialog();
	afx_msg UINT OnGetDlgCode(); 
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
   UTL_StaticLink m_email;
   UTL_StaticLink m_url;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPIRATIONDIALOG_H__9D2CD344_6FE7_11D1_9C85_0060972D1F5F__INCLUDED_)