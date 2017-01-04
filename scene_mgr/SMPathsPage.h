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



// SMPathsPage.h : implementation file
//

#ifndef __SCENEMANAGER_SMPATHSPAGE_H
#define __SCENEMANAGER_SMPATHSPAGE_H


#include "..\resource.h"
#include "SMLCTRL.H"

#include <string>



namespace scene_mgr
{

// CSMPathsPage dialog

class CSMPathsPage : public CPropertyPage
{
   DECLARE_DYNAMIC(CSMPathsPage)

   HBITMAP m_hMoveUpBtnBitmap;
   HBITMAP m_hMoveDownBtnBitmap;
   BOOL m_bInitialized;
   BOOL m_bShowTargetControls;

public:
   CSMPathsPage();
   virtual ~CSMPathsPage();
   void Refresh();
   void ShowTargetControls(BOOL bShow) { m_bShowTargetControls = bShow; }

// Dialog Data
   enum { IDD = IDD_SM_PATHS };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/sdmgr/Scene_Data_Manager_Paths.htm";}

   DECLARE_MESSAGE_MAP()

public:
   CEdit m_TargetEditCtrl;
   CButton m_AddBtnCtrl;
   CButton m_ChangeBtnCtrl;
   CButton m_DataCheckBtnCtrl;
   CButton m_HelpBtnCtrl;
   CButton m_MoveDownBtnCtrl;
   CButton m_MoveUpBtnCtrl;
   CButton m_RemoveBtnCtrl;
   CButton m_SelectAllBtnCtrl;
   CSMListCtrl m_SourceListCtrl;
   CStatic m_TargetNameCtrl;

   afx_msg void OnSelectAll();
   afx_msg void OnMoveUp();
   afx_msg void OnMoveDown();
   afx_msg void OnAdd();
   afx_msg void OnRemove();
   afx_msg void OnDataCheck();
   afx_msg void OnHelp();
   afx_msg void OnChange();
   virtual BOOL OnInitDialog();

   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   LRESULT OnColumnHeaderSized(WPARAM wParam,LPARAM lParam);
};

};  // namespace scene_mgr

#endif  // #ifndef __SCENEMANAGER_SMPATHSPAGE_H