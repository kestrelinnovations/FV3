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


// SMBrowsePaths.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SMBrowsePathsDlg.h"

// system includes
#include <afxdialogex.h>

// third party files

// other FalconView headers
#include "ProcessComError.h"

// this project's headers


namespace scene_mgr
{

// AddFoldersArray implementation
//

SceneFoldersArray::~SceneFoldersArray()
{
   for (int i = 0; i < GetSize(); i++)
      GetFolder(i)->Release();
}

void SceneFoldersArray::Add(IShellFolderObject* pFolder, bool bJukebox, bool bRemote, CString connect_string)
{
   pFolder->AddRef();

   SceneFolderStruct item;
   item.m_pShellFolder = pFolder;
   item.m_bJukebox = bJukebox;
   item.m_bRemote = bRemote;
   item.m_connect_string = connect_string;
   m_arraySceneFolders.Add(item);
}


// CSMBrowsePathsDlg dialog

IMPLEMENT_DYNAMIC(CSMBrowsePathsDlg, CDialog)

CSMBrowsePathsDlg::CSMBrowsePathsDlg(CWnd* pParent /*=NULL*/, UINT nIDTemplate /*=IDD_MDM_BROWSE*/)
   : CDialog(nIDTemplate, pParent)
{
   m_bUserTypedFolderName = false;
   m_bExpanding = false;
   m_strFolderName = _T("");
}

CSMBrowsePathsDlg::~CSMBrowsePathsDlg()
{
}

void CSMBrowsePathsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_SMBROWSEPATHS_EDIT1, m_strFolderName);
   DDX_Control(pDX, IDC_SMBROWSEPATHS_EDIT1, m_editFolderName);
   DDX_Control(pDX, IDC_SMBROWSEPATHS_FOLDERVIEWTREE, m_treeBrowseCtrl);
   DDX_Control(pDX, IDC_SMBROWSEPATHS_DELETE, m_btnDelFolder);
   DDX_Control(pDX, IDC_SMBROWSEPATHS_ADD, m_btnAddFolder);
   DDX_Control(pDX, IDC_SMBROWSEPATHS_FOLDERLIST, m_listAddFolders);
   DDX_Control(pDX, IDOK, m_OKButton);
}


BEGIN_MESSAGE_MAP(CSMBrowsePathsDlg, CDialog)
   ON_WM_CREATE()
   ON_BN_CLICKED(IDC_SMBROWSEPATHS_ADD, OnAddFolderButton)
   ON_BN_CLICKED(IDC_SMBROWSEPATHS_DELETE, OnDelFolderButton)
   ON_EN_UPDATE(IDC_SMBROWSEPATHS_EDIT1, OnUpdateFolderNameEdit)
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CSMBrowsePathsDlg, CDialog)
   ON_EVENT(CSMBrowsePathsDlg, IDC_SMBROWSEPATHS_FOLDERVIEWTREE, 1 /* FolderSelected */, OnSelectedFolderViewTree, VTS_BSTR)
END_EVENTSINK_MAP()


// CSMBrowsePathsDlg message handlers

BOOL CSMBrowsePathsDlg::OnInitDialog()
{
   CoInitialize(NULL);
   CDialog::OnInitDialog();

   // Hourglass on!
   CWaitCursor temp;

   // Attach system image list to list ctrl (for the icons)
   SHFILEINFO      sfi;
   memset(&sfi, 0, sizeof(sfi));

   HIMAGELIST   hSysImageList = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), 0,
                                      &sfi, sizeof(SHFILEINFO),
                                      SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
   if (hSysImageList)
   {
      ListView_SetImageList(m_listAddFolders, hSysImageList, LVSIL_SMALL);
   }

   m_listAddFolders.InsertColumn(0, "", LVCFMT_LEFT);

   m_treeBrowseCtrl.FillTree();

   return TRUE;   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

// adds the current folder name in the edit control to the add files list control
void CSMBrowsePathsDlg::OnAddFolderButton()
{
   // Hourglass on!
   CWaitCursor temp;

   UpdateData();

   // see if the requested folder is already in the list or is blank
   if ((m_strFolderName == "") || SearchForItemText(m_strFolderName))
   {
      ::MessageBeep(MB_ICONEXCLAMATION);
      return;
   }

   try
   {
      IShellFolderObjectPtr smpShellFolderObject = GetCurrentShellFolder();
      if (smpShellFolderObject == NULL)
      {
         AfxMessageBox("This path cannot be accessed.");
         return;
      }

#if 0
      if (m_eMapDataFound == UNVERIFIED_MAP_DATA)
         m_eMapDataFound = DoesFolderContainMapData(smpShellFolderObject, m_chkJukebox != 0);
#endif

      LVITEM lvItem;
      memset(&lvItem, 0, sizeof(lvItem));

      lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
      lvItem.iImage = smpShellFolderObject->GetIconIndex(0); // get the icon for this folder

      lvItem.pszText = (LPSTR)(LPCSTR)m_strFolderName;
#if 0
      if (m_eMapDataFound == NO_MAP_DATA)
      {
         if (AfxMessageBox("This folder contains no map data.\n\n"
            "You should only pick a map data folder that displays \"Map data found\" "
            "at the bottom of the dialog when you select it. The only exception is "
            "when you are adding a brand new empty folder that you are going to "
            "copy map data to.\n\n"
            "Are you sure that you want to add this folder?",
            MB_YESNO) != IDYES)
            return;
      }
#endif

      lvItem.lParam = (LPARAM)(IShellFolderObject*) smpShellFolderObject; // store SNShellFolder object with item
      smpShellFolderObject->AddRef();

      // insert new item into list control
      VERIFY(m_listAddFolders.InsertItem(&lvItem) != -1);

      // get width (in pixels) of the new path (and add a little for the space after the 2 icons)
      int strwidth = m_listAddFolders.GetStringWidth(lvItem.pszText) + (::GetSystemMetrics(SM_CXSMICON) * 2) + 16;

      // increase column width if this string is longer than current width.
      if (m_listAddFolders.GetColumnWidth(0) < strwidth)
      {
         m_listAddFolders.SetColumnWidth(0, strwidth);
      }

      m_OKButton.EnableWindow(TRUE);
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CSMBrowsePathsDlg::OnAddFolderButton"));
   }
}

void CSMBrowsePathsDlg::OnDelFolderButton()
{
   POSITION pos = m_listAddFolders.GetFirstSelectedItemPosition();

   if (pos == 0)
   {
      ::MessageBeep(MB_ICONEXCLAMATION);
      return;
   }

   // delete all the selected paths in the list control
   while (pos)
   {
      int nItem = m_listAddFolders.GetNextSelectedItem(pos);
      m_listAddFolders.DeleteItem(nItem);
      //m_remote_paths.RemoveKey(m_listAddFolders.GetItemText(nItem, 0));

      // deleting item messes up pos so we have to start over
      pos = m_listAddFolders.GetFirstSelectedItemPosition();
   }

   if (m_listAddFolders.GetItemCount() == 0)
      m_OKButton.EnableWindow(FALSE);
}

IShellFolderObjectPtr CSMBrowsePathsDlg::GetCurrentShellFolder()
{
   IShellFolderObjectPtr smpShellFolderObject;
   try
   {
      if (m_bUserTypedFolderName)
      {
         CO_CREATE(smpShellFolderObject, __uuidof(ShellFolderObject));

         if (smpShellFolderObject->AttachToFolder(_bstr_t(m_strFolderName)) != S_OK)
            return NULL;            // This path cannot be accessed.
      }
      else         // user selected an item in the tree control
      {
         // Get the SNShellFolder object for the selected entry in the tree control
         smpShellFolderObject = (IShellFolderObject*)m_treeBrowseCtrl.GetCrntShellFolder();
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CSMBrowsePathsDlg::GetCurrentShellFolder"));
   }
   ASSERT(smpShellFolderObject != NULL);
   return smpShellFolderObject;
}

bool CSMBrowsePathsDlg::SearchForItemText(LPCSTR pstrItemText)
{
   for (int i = 0; i < m_listAddFolders.GetItemCount(); i++)
   {
      if (m_listAddFolders.GetItemText(i, 0).CompareNoCase(pstrItemText) == 0)
        return true;
   }
   return false;
}

void CSMBrowsePathsDlg::OnUpdateFolderNameEdit()
{
   // TODO: If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CDialog::OnInitDialog()
   // function to send the EM_SETEVENTMASK message to the control
   // with the ENM_UPDATE flag ORed into the lParam mask.

   m_bUserTypedFolderName = true;
   //m_eMapDataFound = UNVERIFIED_MAP_DATA;

   if (m_btnAddFolder.m_hWnd)
      m_btnAddFolder.EnableWindow(TRUE);
}

// The user has selected a folder
void CSMBrowsePathsDlg::OnSelectedFolderViewTree(LPCTSTR strFolderName)
{
   if (m_bExpanding)
      return;

   IShellFolderObjectPtr smpShellFolder((IShellFolderObject*) m_treeBrowseCtrl.GetCrntShellFolder());

   if (smpShellFolder == NULL)
   {
      ASSERT(0);     // does this ever happen?
      return; // no current selected folder
   }

   m_bUserTypedFolderName = false;

   DisplayMapDataFoundBoxes(smpShellFolder);
}

void CSMBrowsePathsDlg::DisplayMapDataFoundBoxes(IShellFolderObjectPtr smpShellFolder)
{
   if (smpShellFolder == NULL)
      return; // no current selected folder

   try
   {
      // Hourglass on!
      CWaitCursor temp;

      // hide both data found msgs
#if 0
      m_ctrlDataFoundRO.ShowWindow(SW_HIDE);
      m_ctrlDataFound.ShowWindow(SW_HIDE);
#endif
      m_strFolderName = (LPCSTR)smpShellFolder->m_bstrFullPathName;

#if 0
      m_eMapDataFound = DoesFolderContainMapData(smpShellFolder, m_chkJukebox != 0);

      if (m_eMapDataFound != NO_MAP_DATA)
      {
         if (m_eMapDataFound == LOCAL_MAP_DATA)
            ShowMapDataFound(TestRWAccess(m_strFolderName));
         else        //(eMapDataFound == DMDS_MAP_DATA)
            ShowMapDataFound(false);
      }
#endif
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMdmBrowse::DisplayMapDataFoundBoxes"));
   }
   UpdateData(FALSE);
}

// When OK is pressed, copy all the IShellFolder objects in the add paths list
// to our persistent m_listAddFolders array.  The add paths list ctrl will be deleted
// when DoModal() completes.
void CSMBrowsePathsDlg::OnOK()
{
   for (int i = 0; i < m_listAddFolders.GetItemCount(); i++)
   {
      IShellFolderObject *folder = (IShellFolderObject*)m_listAddFolders.GetItemData(i);

      try
      {
         CString text = m_listAddFolders.GetItemText(i, 0);

         //long l;
         m_arraySceneFolders.Add(folder, (m_listAddFolders.GetItemState(i, LVIS_STATEIMAGEMASK) != 0),
            /* m_remote_paths.Lookup(text, l) ? true :*/  false, text);
      }
      catch (_com_error err)
      {
         ProcessComError(err, _T("CSMBrowsePathsDlg::OnOK"));
      }
   }

   CDialog::OnOK();
}

};  // namespace scene_mgr
