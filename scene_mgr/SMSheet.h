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



#ifndef __SCENEMANAGER_SMSHEET_H
#define __SCENEMANAGER_SMSHEET_H

#include "SMManagePage.h"
#include "SMPathsPage.h"
#include "SMBrowsePage.h"
#include "SMSearchPage.h"


namespace scene_mgr
{

// CSMSheet

class CSMSheet : public CPropertySheet
{
   DECLARE_DYNAMIC(CSMSheet)

public:
   const enum SMActive_t {
      cMANAGE,
      cPATHS,
      cBROWSE,
      cSEARCH
   } ;

public:
   CSMSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
   void Refresh();
   void SaveSheetPos(void);
   void SetSheetPos(CWnd *pFrame);
   void NudgeSheetPos(void);

public:
   CSMManagePage m_pgManage;
   CSMPathsPage m_pgPaths;
   CSMBrowsePage m_pgBrowse;
   CSMSearchPage m_pgSearch;

// Implementation
public:
   SMActive_t GetSMPageActive(void);
   void SetSMPageActive(SMActive_t page);
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual BOOL OnInitDialog();

protected:
   virtual void PostNcDestroy();

public:
   afx_msg void OnClose();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
   afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg LRESULT OnCovGenStop(WPARAM, LPARAM);

protected:
   DECLARE_MESSAGE_MAP()
};

};  // namespace scene_mgr

#endif  // #ifndef __SCENEMANAGER_SMSHEET_H
