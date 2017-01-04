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

#if !defined(AFX_IMPORTINFODLG_H__05E4FE28_4266_4960_AD6B_7ED06D774D9B__INCLUDED_)
#define AFX_IMPORTINFODLG_H__05E4FE28_4266_4960_AD6B_7ED06D774D9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportInfoDlg dialog

class CImportPropSheet;

class CImportInfoDlg : public CDialog
{
// Construction
public:
	CImportInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportInfoDlg)
	enum { IDD = IDD_TAB_IMPORT_INFO };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Import_Information_Overlay_Tabular_Editor.htm";}

	CEdit	m_edit_ctrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSaveSettings();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_info_text;
	CImportPropSheet *m_property_sheet;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTINFODLG_H__05E4FE28_4266_4960_AD6B_7ED06D774D9B__INCLUDED_)