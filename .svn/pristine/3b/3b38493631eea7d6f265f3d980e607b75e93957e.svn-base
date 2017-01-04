// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

//{{AFX_INCLUDES()
#include "..\browseforfolder.h"
//}}AFX_INCLUDES


// MdmBrowse.h : header file
//
#include "..\resource.h"
#include "MDSWrapper.h"

struct IShellFolderObject;

//////////////////////////////////
// this class contains shell folder information 77or every folder selected in the dialog for addition 

class AddFoldersArray
{
public:

   ~AddFoldersArray();

   int GetSize()
   {
      return m_arrayAddFolders.GetSize();
   }

   IShellFolderObject* GetFolder(int n)
   {
      return m_arrayAddFolders.GetAt(n).m_pShellFolder;
   }

   bool IsFolderJukebox(int n)
   {
      return m_arrayAddFolders.GetAt(n).m_bJukebox;
   }

   bool IsFolderRemote(int n)
   {
      return m_arrayAddFolders.GetAt(n).m_bRemote;
   }

   CString GetConnectString(int n)
   {
      return m_arrayAddFolders.GetAt(n).m_connect_string;
   }

   void Add(IShellFolderObject* pFolder, bool bJukebox, bool bRemote, CString connect_string);

private:

   struct AddFolderStruct
   {
      IShellFolderObject* m_pShellFolder;
      bool m_bJukebox;
      bool m_bRemote;
      CString m_connect_string;
   };

   CArray<AddFolderStruct, AddFolderStruct>  m_arrayAddFolders;
};


/////////////////////////////////////////////////////////////////////////////
// CMdmBrowse dialog

// Function definition for the function that gets called back to determine if the 
// 'Map Data Found' message should be displayed
typedef bool (*fnFolderContainsMapData)(IShellFolderObject &Folder, bool bSearchNextLevel);

class CMdmBrowse : public CDialog
{
// Construction
public:
   CMdmBrowse(CWnd* pParent = NULL, UINT nIDTemplate = IDD_MDM_BROWSE);   // standard constructor
   ~CMdmBrowse();

   afx_msg void OnDropFiles( HDROP hDropInfo );

   void set_window_text(CString s)
   {
      m_sWindowText = s;
   }

   void SetFolderContainsMapDataCallback(fnFolderContainsMapData folderContainsMapData)
   {
      m_fnFolderContainsMapData = folderContainsMapData;
   }

   AddFoldersArray   m_arrayAddFolders;

   // Dialog Data
   //{{AFX_DATA(CMdmBrowse)
   enum { IDD = IDD_MDM_BROWSE };

   CEdit m_editFolderName;
   CStatic m_ctrlDataFoundRO;
   CStatic m_ctrlDataFound;
   CButton m_btnDelFolder;
   CButton m_btnAddFolder;
   CListCtrl m_listAddFolders;
   CButton m_OKButton;
   CString m_strFolderName;
   CBrowseForFolder  m_treeBrowseCtrl;
   BOOL  m_chkJukebox;

   BOOL  m_chkNewMds;    // For New Map Data Server
   CButton m_ctlNewMds;

   //}}AFX_DATA

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMdmBrowse)
protected:  
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   //}}AFX_VIRTUAL

// Implementation
protected:
   enum DATATYPE_FOUND {UNVERIFIED_MAP_DATA = -1, NO_MAP_DATA = 0, LOCAL_MAP_DATA = 1, DMDS_MAP_DATA = 2};

   enum DATATYPE_FOUND m_eMapDataFound;
   CString m_sWindowText;
   bool m_bUserTypedFolderName, m_bExpanding;
   UINT m_nIDTemplate;
   StringToLongMap m_remote_paths;

   CImageList  m_jukeboxImageList;

   bool    SearchForItemText(LPCSTR pstrItemText);
   void    ShowMapDataFound(bool readwrite);
   enum DATATYPE_FOUND DoesFolderContainMapData(IShellFolderObjectPtr pCrntSNShellFolder, bool bJukebox);
   IShellFolderObjectPtr GetCurrentShellFolder();
   void DisplayMapDataFoundBoxes(IShellFolderObjectPtr smpShellFolder);

   fnFolderContainsMapData m_fnFolderContainsMapData;

   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Browse_for_Map_Data.htm";}
   // Generated message map functions
   //{{AFX_MSG(CMdmBrowse)
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   virtual BOOL OnInitDialog();
   afx_msg void OnAddFolderButton();
   afx_msg void OnLocateFolderButton();
   afx_msg void OnDelFolderButton();
   afx_msg void OnCancelButton();
   virtual void OnOK();
   afx_msg void OnSelectedFolderViewTree(LPCTSTR strFolderName);
   afx_msg void OnJukebox();
   afx_msg void OnUpdateFolderNameEdit();
   afx_msg void OnNewMds();    // For New Map Data Server
   DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
