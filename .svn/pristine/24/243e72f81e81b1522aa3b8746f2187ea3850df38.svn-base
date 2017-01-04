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



// SMBrowsePage.h : implementation file
//



#ifndef __SCENEMANAGER_SMBROWSEPAGE_H
#define __SCENEMANAGER_SMBROWSEPAGE_H

#include "..\resource.h"
#include <afxcmn.h>


namespace scene_mgr
{

// CSMBrowsePage dialog

class CSMBrowsePage : public CPropertyPage
{
   DECLARE_DYNAMIC(CSMBrowsePage)

public:
   CSMBrowsePage();
   virtual ~CSMBrowsePage();

// Dialog Data
   enum { IDD = IDD_SM_BROWSE };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()

   void UpdateOverlaySDSSceneTypes(long scene_id);

public:
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnBnClickedSmbrowseCenterOn();
   afx_msg void OnBnClickedSmbrowseOpen();
   afx_msg void OnBnClickedSmbrowseHelp();
   afx_msg void OnSelChangedSmbrowseTree(NMHDR* pNMHDR, LRESULT* pResult);

   virtual BOOL OnSetActive();
   virtual BOOL OnInitDialog();

   CButton m_CenterOnBtn;
   CButton m_OpenBtn;
   CButton m_HelpBtn;
   CTreeCtrl m_BrowseTree;
   CImageList m_treeImages;
   BOOL m_bInitialized;


   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/sdmgr/Scene_Data_Manager_Browse.htm";}
   int Refresh();
   int UpdateScenes();

};

};  // namespace scene_mgr

#endif  // #ifndef __SCENEMANAGER_SMBROWSEPAGE_H