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

#if !defined(AFX_DATAEXPORTDLG_H__147D48B9_BF50_456C_8C61_C087553C5BF4__INCLUDED_)
#define AFX_DATAEXPORTDLG_H__147D48B9_BF50_456C_8C61_C087553C5BF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataExportDlg.h : header file
//

class FVGridCtrl;
class TabularEditorDataObject;

/////////////////////////////////////////////////////////////////////////////
// CDataExportDlg dialog

class CDataExportDlg : public CDialog
{
// Construction
public:
	CDataExportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataExportDlg)
	enum { IDD = IDD_TAB_EXPORT };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Export_Overlay_Tabular_Editor.htm";}

	CButton m_tab_delimited_ctrl;
	CButton m_comma_delimited_ctrl;
	CString	m_output_path;
	CProgressCtrl	m_progress_ctrl;
   CButton m_ckExportHeaders;
	//}}AFX_DATA

	FVGridCtrl *m_FVGrid;
	TabularEditorDataObject *m_data_object;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDataExportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowsePath();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAEXPORTDLG_H__147D48B9_BF50_456C_8C61_C087553C5BF4__INCLUDED_)