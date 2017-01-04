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

#if !defined(AFX_SHP_MNGR_H__CD5E63C0_F650_4F44_8646_9FC2F960C999__INCLUDED_)
#define AFX_SHP_MNGR_H__CD5E63C0_F650_4F44_8646_9FC2F960C999__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// shp_mngr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShapeManagerDlg dialog

class CShapeManagerDlg : public CDialog
{
// Construction
public:
	CShapeManagerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShapeManagerDlg)
	enum { IDD = IDD_SHP_MNGR_DLG };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Shape_File_Manager.htm";}

	CString	m_dest_path;
	CString	m_src_path;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShapeManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShapeManagerDlg)
	afx_msg void OnCopy();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestBrowse();
	afx_msg void OnSrcBrowse();
	afx_msg void OnDblclkSrcList();
	virtual void OnCancel();
	afx_msg void OnDelete();
	afx_msg void OnSelchangeSrcList();
	afx_msg void OnSelchangeDestList();
	afx_msg void OnKillfocusSrcEdit();
	afx_msg void OnKillfocusDestEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void update_list(CString path, int list_id);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHP_MNGR_H__CD5E63C0_F650_4F44_8646_9FC2F960C999__INCLUDED_)