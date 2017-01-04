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



// prnt_dlg.h : header file
//

#include <dlgs.h>

/////////////////////////////////////////////////////////////////////////////
// custom_print_dialog dialog

class custom_print_dialog : public CPrintDialog
{
// Construction
public:
	custom_print_dialog(BOOL bPrintSetupOnly, 
           DWORD dwFlags, CWnd* pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(custom_print_dialog)
	enum { IDD = PRINTDLGORD };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(custom_print_dialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(custom_print_dialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnMapButton();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
