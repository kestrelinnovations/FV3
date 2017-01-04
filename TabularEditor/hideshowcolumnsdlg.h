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

#if !defined(AFX_HIDESHOWCOLUMNSDLG_H__08F9ECE4_367F_402D_8A2C_BDE4CAC8C8C1__INCLUDED_)
#define AFX_HIDESHOWCOLUMNSDLG_H__08F9ECE4_367F_402D_8A2C_BDE4CAC8C8C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// hideshowcolumnsdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHideShowColumnsDlg dialog

class TabularEditorDataObject;

class CHideShowColumnsDlg : public CDialog
{
protected:
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
// Construction
public:
	CHideShowColumnsDlg(CWnd* pParent = NULL);   // standard constructor
	void set_data_object(TabularEditorDataObject *d) { m_data_object = d; }

// Dialog Data
	//{{AFX_DATA(CHideShowColumnsDlg)
	enum { IDD = IDD_TAB_HIDESHOW_COL };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/View_or_Hide_Columns_Overlay_Tabular_Editor.htm";}

	CTreeCtrl m_tree_ctrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHideShowColumnsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHideShowColumnsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	TabularEditorDataObject *m_data_object;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HIDESHOWCOLUMNSDLG_H__08F9ECE4_367F_402D_8A2C_BDE4CAC8C8C1__INCLUDED_)