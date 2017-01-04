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



#if !defined(AFX_MDMPATHS_H__B041AFB4_62AE_11D2_B07D_00104B8C938E__INCLUDED_)
#define AFX_MDMPATHS_H__B041AFB4_62AE_11D2_B07D_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MDMPaths.h : header file
//

#include "..\resource.h"
#include "mdmlctrl.h"
#include <set>

#import "RMDSTreeServer.tlb" no_namespace, named_guids
#import "ShellFolderObjectServer.tlb" no_namespace

typedef CMap<CString, LPCSTR, CString, LPCSTR> StringToStringMap;

/////////////////////////////////////////////////////////////////////////////
// CMDMPaths dialog

bool FolderContainsMapData(IShellFolderObject &Folder, bool bSearchNextLevel);

class CMDMPaths : public CPropertyPage
{
   DECLARE_DYNCREATE(CMDMPaths)

   BOOL m_bInitialized;
   BOOL m_bShowTargetControls;
   HBITMAP m_hMoveUpBtnBitmap;
   HBITMAP m_hMoveDownBtnBitmap;
   bool m_use_rmds, m_show_polar_copy, m_bInRefresh, m_bUpdatingCheck;

// Construction
public:
   void ShowTargetControls(BOOL bShow) { m_bShowTargetControls = bShow; }
   CMDMPaths();
   ~CMDMPaths();
   void Refresh();
   CString& RemoveBrackets(CString& s);
   void GetMinimumRect(CRect* pRect);

   // Two Generally Useful Functions

   static bool IsPathReadOnly(CString path);
   static bool IsPathSourceOnly(CString path);

   static IRMDSTreePtr s_rmds_tree;
   static CMDMPaths *s_instance;

// Dialog Data
   //{{AFX_DATA(CMDMPaths)
   enum { IDD = IDD_MDM_PATHS };
   CButton m_CopyPolarBtnCtrl;
   CButton m_DataCheckBtnCtrl;
   CStatic m_MinSizeBoxCtrl;
   CButton m_HelpBtnCtrl;
   CStatic m_TargetNameCtrl;
   CEdit m_TargetEditCtrl;
   CButton m_SelectAllBtnCtrl;
   CMDMListCtrl m_SourceListCtrl;
   CButton m_RemoveBtnCtrl;
   CButton m_MoveUpBtnCtrl;
   CButton m_MoveDownBtnCtrl;
   CButton m_ChangeBtnCtrl;
   CButton m_AddBtnCtrl;
   CButton m_SharingBtnCtrl;
   CButton m_ChartUpdateCtrl;

   // cloud icon index into MDMCTRL.m_ImageList. This icon indicates
   // a web source...
   // See MDMCTRL.AddIconToImageList for more information.
   int m_CloudIconIndex;
   
   // because dataSources and webSources may have overlapping
   // identifiers, this will keep a list of the list control entry
   // id's that are known to be webSources
   std::set<int> m_WebSourceEntries;

   //Button to initiate the MDMInstallWebMapType dialog
   CButton m_AddServiceCtrl;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CMDMPaths)
   public:
   virtual BOOL OnSetActive();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Map_Data_Manager_Sources.htm";}
   // Generated message map functions
   //{{AFX_MSG(CMDMPaths)
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnMdmpathsAdd();
   afx_msg void OnMdmpathsAddWebService();
   afx_msg void OnMdmpathsChange();
   afx_msg void OnMdmpathsMovedown();
   afx_msg void OnMdmpathsMoveup();
   afx_msg void OnMdmpathsRemove();
   afx_msg void OnMdmSharing();
   virtual BOOL OnInitDialog();
   afx_msg void OnMdmpathsSelectall();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnHelpMdmPaths();
   afx_msg void OnMdmpathsDatacheck();
   afx_msg void OnItemchangedMdmpathsSourcelist(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnMdmPathsCopyPolar();
   afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnExplorePath();
   afx_msg void OnChartUpdate();
   //}}AFX_MSG
   LRESULT OnColumnHeaderSized(WPARAM wParam,LPARAM lParam);

   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMPATHS_H__B041AFB4_62AE_11D2_B07D_00104B8C938E__INCLUDED_)
