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

#if !defined(AFX_SELECTTABLEDLG_H__AD80D9BA_3C35_4BC2_A7CA_0E75E9D188CE__INCLUDED_)
#define AFX_SELECTTABLEDLG_H__AD80D9BA_3C35_4BC2_A7CA_0E75E9D188CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectTableDlg.h : header file
//

#include "..\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectTableDlg dialog

class CSelectTableDlg : public CDialog
{
// Construction
public:
	CSelectTableDlg(CWnd* pParent = NULL);   // standard constructor

	void set_table_list(CList<CString, CString> &table_list) { m_table_list.AddTail(&table_list); }
	int get_first_row_contains_header() { return m_first_row_contains_header; }
	int get_sheet_num() { return m_sheet_num; }

// Dialog Data
	//{{AFX_DATA(CSelectTableDlg)
	enum { IDD = IDD_TAB_IMPORT_SELECT_TABLE };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/FalconView_Overview.htm";}
	CButton	m_first_row_ctrl;
	CListCtrl	m_list_ctrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectTableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectTableDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual void OnOK();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageList m_images;
	CList <CString, CString> m_table_list;

	int m_first_row_contains_header;
	int m_sheet_num;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTTABLEDLG_H__AD80D9BA_3C35_4BC2_A7CA_0E75E9D188CE__INCLUDED_)