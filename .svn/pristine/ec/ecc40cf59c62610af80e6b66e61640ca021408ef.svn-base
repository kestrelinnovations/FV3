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



#if !defined(AFX_MDMTYPES_H__1B919EB3_643A_11D2_B07F_00104B8C938E__INCLUDED_)
#define AFX_MDMTYPES_H__1B919EB3_643A_11D2_B07F_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mdmtypes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMDMTypes dialog

class CMDMTypes : public CPropertyPage
{
	DECLARE_DYNCREATE(CMDMTypes)

// Construction
public:
	CMDMTypes();
	~CMDMTypes();
   void GetMinimumRect(CRect* pRect);

// Dialog Data
	//{{AFX_DATA(CMDMTypes)
	enum { IDD = IDD_MDM_TYPES };
	CButton	m_HelpBtnCtrl;
	CListBox	m_UseListBoxCtrl;
	CListBox	m_AvailListBoxCtrl;
	CButton	m_RemoveOneBtnCtrl;
	CButton	m_RemoveAllBtnCtrl;
	CButton	m_CopyOneBtnCtrl;
	CButton	m_CopyAllBtnCtrl;
	CButton	m_CancelBtnCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMDMTypes)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMDMTypes)
	afx_msg void OnMdmtypesCancel();
	afx_msg void OnMdmtypesCopyall();
	afx_msg void OnMdmtypesCopyone();
	afx_msg void OnMdmtypesRemoveall();
	afx_msg void OnMdmtypesRemoveone();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelpMdmTypes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMTYPES_H__1B919EB3_643A_11D2_B07F_00104B8C938E__INCLUDED_)
