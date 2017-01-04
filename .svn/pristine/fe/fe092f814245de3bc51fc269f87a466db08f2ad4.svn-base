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



#if !defined(AFX_MDMMULTIPLE_H__615D2264_636B_11D2_B07E_00104B8C938E__INCLUDED_)
#define AFX_MDMMULTIPLE_H__615D2264_636B_11D2_B07E_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mdmmult.h : header file
//
#include "mdmlctrl.h"

class CoverageOverlay;

/////////////////////////////////////////////////////////////////////////////
// CMDMMultiple dialog

class CMDMMultiple : public CPropertyPage
{
   DECLARE_DYNCREATE(CMDMMultiple)

   BOOL m_bInitialized;
   BOOL m_bRefreshing;
   BOOL m_bSufficientSpace;

// Construction
public:
   CMDMMultiple();
   ~CMDMMultiple();
   void Refresh();
   void EnableUnselecting(BOOL b);
   void GetMinimumRect(CRect* pRect);

// Dialog Data
   //{{AFX_DATA(CMDMMultiple)
   enum { IDD = IDD_MDM_MULTIPLE };
   CStatic   m_MinSizeBoxCtrl;
   CStatic   m_FreeSpace2Ctrl;
   CStatic   m_TotalDeleteTitleCtrl;
   CStatic   m_StaticSpacerCtrl;
   CStatic   m_TotalDeleteCtrl;
   CStatic   m_TotalCopyCtrl;
   CStatic   m_FreeSpaceTitleCtrl;
   CStatic   m_FreeSpaceCtrl;
   CComboBox   m_ViewComboCtrl;
   CButton   m_HelpBtnCtrl;
   CButton   m_SelectAllBtnCtrl;
   CButton   m_SelectAllDataBtnCtrl;
   CButton   m_ClearBtnCtrl;
   CButton   m_AutoHideBtnCtrl;
   CButton   m_ApplyBtnCtrl;
   CMDMListCtrl   m_TypeListCtrl;
   CButton  m_NotCurrentCtrl;
   CButton  m_CurrencyReportCtrl;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CMDMMultiple)
   public:
   virtual BOOL OnSetActive();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Map_Data_Manager_Multiple.htm";}
   // Generated message map functions
   //{{AFX_MSG(CMDMMultiple)
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   virtual BOOL OnInitDialog();
   afx_msg void OnMdmmultipleApply();
   afx_msg void OnMdmmultipleAutohide();
   afx_msg void OnMdmmultipleSelectalldata();
   afx_msg void OnMdmmultipleClear();
   afx_msg void OnMdmmultipleSelectall();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnHelpMdmMultiple();
   afx_msg void OnSelchangeMdmmultipleViewcombo();
   afx_msg void OnNMRclickMdmmultipleTypelist(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnScaleToExtents();
   afx_msg void OnNotCurrent();
   afx_msg void OnReport();
   //}}AFX_MSG
   LRESULT OnSelectionChanged(WPARAM wParam,LPARAM lParam);
   LRESULT OnColumnHeaderSized(WPARAM wParam,LPARAM lParam);

   DECLARE_MESSAGE_MAP()

private:
   void UpdateOverlayMDSMapTypes();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMMULTIPLE_H__615D2264_636B_11D2_B07E_00104B8C938E__INCLUDED_)
