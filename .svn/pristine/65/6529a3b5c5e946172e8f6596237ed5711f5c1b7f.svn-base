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



#if !defined(AFX_MDMSINGLE_H__615D2267_636B_11D2_B07E_00104B8C938E__INCLUDED_)
#define AFX_MDMSINGLE_H__615D2267_636B_11D2_B07E_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mdmsingl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMDMSingle dialog

#include "..\resource.h"

class CMDMSingle : public CPropertyPage
{
	DECLARE_DYNCREATE(CMDMSingle)

   BOOL m_bInitialized;
   BOOL m_bSufficientSpace;
	BOOL m_bReadyToActivate;

// Construction
public:
	CMDMSingle();
	~CMDMSingle();
   void Init();
   void Refresh();
   void GetMinimumRect(CRect* pRect);

// Dialog Data
	//{{AFX_DATA(CMDMSingle)
	enum { IDD = IDD_MDM_SINGLE };
	CStatic	m_MinSizeBoxCtrl;
	CButton	m_HelpBtnCtrl;
	CButton	m_TargetPathUsageCtrl;
	CButton	m_FilesSelectedCtrl;
	CStatic	m_TotalDataCtrl;
	CStatic	m_DiskSpaceCtrl;
	CStatic	m_DeleteSelectedCtrl;
	CStatic	m_CurrentUsageCtrl;
	CStatic	m_CopySelectedCtrl;
	CComboBox	m_TypeComboCtrl;
	CButton	m_SelectAllBtnCtrl;
	CButton	m_UnselectBtnCtrl;
	CButton	m_ApplyBtnCtrl;
	CString	m_sCopySelected;
	CString	m_sCurrentUsage;
	CString	m_sDeleteSelected;
	CString	m_sDiskSpace;
	CString	m_sTotalData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMDMSingle)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Map_Data_Manager_Single.htm";}
	// Generated message map functions
	//{{AFX_MSG(CMDMSingle)
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnMdmsingleApply();
	afx_msg void OnMdmsingleSelectall();
	afx_msg void OnMdmsingleUnselect();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMdmsingleType();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelpMdmSingle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMSINGLE_H__615D2267_636B_11D2_B07E_00104B8C938E__INCLUDED_)
