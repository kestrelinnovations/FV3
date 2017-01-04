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

#if !defined(AFX_SETPROJECTIONDLG_H__69408CFD_8FC8_11D4_9EA8_00C04F8ED5F5__INCLUDED_)
#define AFX_SETPROJECTIONDLG_H__69408CFD_8FC8_11D4_9EA8_00C04F8ED5F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetProjectionDlg.h : header file
//

// forward references
class MapProj;

/////////////////////////////////////////////////////////////////////////////
// CSetProjectionDlg dialog

class CSetProjectionDlg : public CDialog
{
// Construction
public:
	CSetProjectionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetProjectionDlg)
	enum { IDD = IDD_SET_PROJECTION };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/maps/Set_Map_Projection.htm";}

	CEdit	m_current_projection_eb;
	CComboBox	m_requested_projection_cb;
	//}}AFX_DATA

	MapProj *m_curr_map;

	CRect m_dlg_rect;
	CRect m_proj_item_rect;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetProjectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetProjectionDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnApply();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboRequestedProjection();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void apply_projection();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPROJECTIONDLG_H__69408CFD_8FC8_11D4_9EA8_00C04F8ED5F5__INCLUDED_)