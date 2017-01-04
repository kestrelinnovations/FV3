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

//{{AFX_INCLUDES()
#include "..\browseforfolder.h"
//}}AFX_INCLUDES


// MdmBrowse.h : header file
//
#include "..\resource.h"

#import "ShellFolderObjectServer.tlb" no_namespace


//////////////////////////////////
// this class contains shell folder information for every folder selected in the dialog for addition 

class AddFoldersArray
{
public:
   ~AddFoldersArray()
   {
      for (int i = 0; i < GetSize(); i++)
         GetFolder(i)->Release();
   }
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
   void Add(IShellFolderObject* pFolder, bool bJukebox)
   {
      pFolder->AddRef();

      AddFolderStruct   item;
      item.m_pShellFolder = pFolder;
      item.m_bJukebox = bJukebox;
      m_arrayAddFolders.Add(item);
   }

private:
   struct AddFolderStruct
   {
      IShellFolderObject*   m_pShellFolder;
      bool              m_bJukebox;
   };

   CArray<AddFolderStruct, AddFolderStruct>  m_arrayAddFolders;
};


/////////////////////////////////////////////////////////////////////////////
// CNitfBrowse dialog

class CNitfBrowse : public CDialog
{
// Construction
public:
   CNitfBrowse(CWnd* pParent = NULL, UINT nIDTemplate = IDD_MDM_BROWSE);   // standard constructor
   ~CNitfBrowse();

   afx_msg void OnDropFiles( HDROP hDropInfo );
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);

   CString get_target_directory(void) const
   {
      return m_TargetDirectory;
   }
   void set_target_directory(const char* target_directory, bool bHasMapData = false)
   {
      m_TargetDirectory = target_directory;
      m_bTargetHasMapData = bHasMapData;
   }
   void set_window_text(CString s)
   {
      m_sWindowText = s;
   }

   AddFoldersArray   m_arrayAddFolders;



// Dialog Data
   //{{AFX_DATA(CNitfBrowse)
   enum { IDD = IDD_MDM_BROWSE };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Browse_for_Map_Data.htm";}

   CEdit m_editFolderName;
   CStatic m_ctrlDataFoundRO;
   CStatic m_ctrlDataFound;
   CButton m_btnDelFolder;
   CButton m_btnAddFolder;
   CListCtrl m_listAddFolders;
   CButton m_OKButton;
   CString m_strFolderName;
   CBrowseForFolder m_treeBrowseCtrl;
   BOOL m_chkJukebox;

   BOOL m_chkNewMds;  // For New Map Data Server
   CButton m_ctlNewMds;

   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CNitfBrowse)
   protected:  
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   //}}AFX_VIRTUAL

// Implementation
protected:
   CString m_TargetDirectory;
   bool m_bTargetHasMapData;
   bool m_bUserTypedFolderName;
   UINT m_nIDTemplate;
   CString m_sWindowText;

   CImageList m_jukeboxImageList;

   bool ShellFolderContainMapData(IShellFolderObject &Folder, bool bSearchNextLevel) const;
   bool SearchForItemText(LPCSTR pstrItemText);
   void ShowMapDataFound(long folderAttributes);

   BOOL file_is_georeferenced(CString filename) const;
   BOOL geo_tiff_file_exists(CString pathname) const;
   BOOL geo_nitf_file_exists(CString pathname) const;



   // Generated message map functions
   //{{AFX_MSG(CNitfBrowse)
   virtual BOOL OnInitDialog();
   afx_msg void OnAddFolderButton();
   afx_msg void OnDelFolderButton();
   virtual void OnOK();
   afx_msg void OnSelectedFolderViewTree(LPCTSTR strFolderName);
   afx_msg void OnJukebox();
   afx_msg void OnUpdateFolderNameEdit();
   afx_msg void OnNewMds();  // For New Map Data Server
   DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};