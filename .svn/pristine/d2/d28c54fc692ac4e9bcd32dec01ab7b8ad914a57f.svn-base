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



// SMSearchPage.h : implementation file
//

#ifndef __SCENEMANAGER_SMSEARCHPAGE_H
#define __SCENEMANAGER_SMSEARCHPAGE_H

#include "..\resource.h"
#include "smlctrl.h"


namespace scene_mgr
{

// CSMSearchPage dialog

class CSMSearchPage : public CPropertyPage
{
   DECLARE_DYNAMIC(CSMSearchPage)

   CButton m_SearchBtn;
   CEdit m_SearchText;
   CButton m_CenterOnBtn;
   CButton m_OpenBtn;
   CButton m_HelpBtn;
   CSMListCtrl m_ResultsListCtrl;
   BOOL m_bInitialized;

public:
   CSMSearchPage();
   virtual ~CSMSearchPage();

// Dialog Data
   enum { IDD = IDD_SM_SEARCH };

protected:
   DECLARE_MESSAGE_MAP()

   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/sdmgr/Scene_Data_Manager_Search.htm";}

   void UpdateOverlaySDSSceneTypes(long scene_id);

public:

   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnHelp();
   afx_msg void OnSearch();
   afx_msg void OnBnClickedSmsearchCenterOn();
   afx_msg void OnBnClickedSmsearchOpen();
   afx_msg LRESULT OnSelectionChanged(WPARAM wParam,LPARAM lParam);
};

};  // namespace scene_mgr

#endif  // #ifndef __SCENEMANAGER_SMSEARCHPAGE_H