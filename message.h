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



// fvgencdc.h : header file
//

#include "common.h"

/////////////////////////////////////////////////////////////////////////////
// CFVMessageDlg dialog

class CFVMessageDlg : public CDialog
{
protected:
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
private:
   CWnd *m_parent;
   CString m_message;
   CString m_title;

// Construction
public:
	CFVMessageDlg(CWnd* pParent, const char *message, const char *title);
	CFVMessageDlg(CWnd* pParent);   

// Display the dialog
public:
// Dialog Data
	//{{AFX_DATA(CFVMessageDlg)
	enum { IDD = IDD_MESSAGE };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/FalconView_Overview.htm";}
	CStatic	m_three_line;
	CStatic	m_two_line;
	CStatic	m_one_line;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFVMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFVMessageDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   int set_message_text();
};