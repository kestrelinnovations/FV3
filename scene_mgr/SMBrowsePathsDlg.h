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


// SMBrowsePathsDlg.h : header file
//

#ifndef __SCENEMANAGER_SMBROWSEPATHSDLG_H
#define __SCENEMANAGER_SMBROWSEPATHSDLG_H


#include "..\browseforfolder.h"
#include "..\resource.h"


namespace scene_mgr
{

//////////////////////////////////
// this class contains shell folder information for every folder selected in the dialog for addition 

class SceneFoldersArray
{
public:

   ~SceneFoldersArray();

   int GetSize()
   {
      return m_arraySceneFolders.GetSize();
   }

   IShellFolderObject* GetFolder(int n)
   {
      return m_arraySceneFolders.GetAt(n).m_pShellFolder;
   }

   bool IsFolderJukebox(int n)
   {
      return m_arraySceneFolders.GetAt(n).m_bJukebox;
   }

   bool IsFolderRemote(int n)
   {
      return m_arraySceneFolders.GetAt(n).m_bRemote;
   }

   CString GetConnectString(int n)
   {
      return m_arraySceneFolders.GetAt(n).m_connect_string;
   }

   void Add(IShellFolderObject* pFolder, bool bJukebox, bool bRemote, CString connect_string);

private:

   struct SceneFolderStruct
   {
      IShellFolderObject* m_pShellFolder;
      bool m_bJukebox;
      bool m_bRemote;
      CString m_connect_string;
   };

   CArray<SceneFolderStruct, SceneFolderStruct>  m_arraySceneFolders;
};


// CSMBrowsePathsDlg dialog
class CSMBrowsePathsDlg : public CDialog
{
   DECLARE_DYNAMIC(CSMBrowsePathsDlg)

public:
   CSMBrowsePathsDlg(CWnd* pParent = NULL, UINT nIDTemplate = IDD_SM_BROWSE_PATHS);   // standard constructor
   virtual ~CSMBrowsePathsDlg();

// Dialog Data
   enum { IDD = IDD_SM_BROWSE_PATHS };

   SceneFoldersArray   m_arraySceneFolders;

   CEdit m_editFolderName;
   //CStatic m_ctrlDataFoundRO;
   //CStatic m_ctrlDataFound;
   CButton m_btnDelFolder;
   CButton m_btnAddFolder;
   CListCtrl m_listAddFolders;
   CButton m_OKButton;
   CString m_strFolderName;
   CBrowseForFolder  m_treeBrowseCtrl;

protected:
   bool m_bUserTypedFolderName, m_bExpanding;

   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   afx_msg void OnAddFolderButton();
   afx_msg void OnDelFolderButton();
   afx_msg void OnUpdateFolderNameEdit();
   afx_msg void OnSelectedFolderViewTree(LPCTSTR strFolderName);
   virtual void OnOK();

   IShellFolderObjectPtr GetCurrentShellFolder();
   bool SearchForItemText(LPCSTR pstrItemText);
   void DisplayMapDataFoundBoxes(IShellFolderObjectPtr smpShellFolder);

   DECLARE_EVENTSINK_MAP()
   DECLARE_MESSAGE_MAP()
};

};  // namespace scene_mgr

#endif  // #ifndef __SCENEMANAGER_SMBROWSEPATHSDLG_H