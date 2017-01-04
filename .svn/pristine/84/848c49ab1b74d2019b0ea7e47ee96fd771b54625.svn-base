// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



// SMManagePage.h : implementation file
//

#ifndef __SCENEMANAGER_SMMANAGEPAGE_H
#define __SCENEMANAGER_SMMANAGEPAGE_H

#include "..\resource.h"
#include "SMLCTRL.H"


namespace scene_mgr
{

// Custom status messages for this dialog
static UINT SM_COPYDELETE_COMPLETE = RegisterWindowMessage(_T("SM_COPYDELETE_COMPLETE"));


// CSMManagePage dialog

class CSMManagePage : public CPropertyPage
{
   DECLARE_DYNAMIC(CSMManagePage)

   BOOL m_bInitialized;
   BOOL m_bRefreshing;
   BOOL m_bSufficientSpace;

public:
   CSMManagePage();
   virtual ~CSMManagePage();

// Dialog Data
   enum { IDD = IDD_SM_MANAGE };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/sdmgr/Scene_Data_Manager_Manage.htm";}

   DECLARE_MESSAGE_MAP()

public:
   CStatic m_MinSizeBoxCtrl;
   CButton m_ApplyBtn;
   CButton m_AutoHideBtn;
   CButton m_HelpBtn;
   CSMListCtrl m_TypeList;
   CButton m_SelectAllBtn;
   CButton m_ShowAllBtn;
   CButton m_UnselectBtn;
   CComboBox m_ViewCombo;
   CStatic m_TotalSelectedTitleStatic;
   CStatic m_FreeSpaceTitleStatic;
   CStatic m_FreeSpaceStatic;
   CStatic m_FreeSpaceStatic2;
   CStatic m_TotalCopyStatic;
   CStatic m_TotalDeleteStatic;
   CStatic m_StaticSpacerCtrl;

   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnBnClickedSmmanageShowall();
   afx_msg void OnBnClickedSmmanageApply();
   afx_msg void OnBnClickedSmmanageSelectall();
   afx_msg void OnBnClickedSmmanageUnselect();
   afx_msg void OnBnClickedSmmanageHelp();
   afx_msg void OnBnClickedSmmanageAutohide();
   afx_msg void OnCbnSelchangeSmmanageViewcombo();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnItemchangedSmmanageTypelist(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnNMRclickSmmanageTypelist(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnScaleToExtents();
   afx_msg LRESULT OnCopyDeleteComplete(WPARAM wParam,LPARAM lParam);
   LRESULT OnSelectionChanged(WPARAM wParam,LPARAM lParam);
   LRESULT OnColumnHeaderSized(WPARAM wParam,LPARAM lParam);

   void Refresh();
   void EnableUnselecting(BOOL b);
   void UpdateOverlaySDSSceneTypes();
   void GetMinimumRect(CRect* pRect);
};

};  // namespace scene_mgr

#endif  // #ifndef __SCENEMANAGER_SMMANAGEPAGE_H