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

// MdmBrowse.cpp : implementation file
//
#include "stdafx.h"
#include "MdmBrowse.h"
#include "ProcessComError.h"
#include "MdsUtilities/MdsUtilities.h"
#include "../idialognotify.h"
#include "..\getobjpr.h"

// AddFoldersArray implementation
//

AddFoldersArray::~AddFoldersArray()
{
   for (int i = 0; i < GetSize(); i++)
      GetFolder(i)->Release();
}

void AddFoldersArray::Add(IShellFolderObject* pFolder, bool bJukebox, bool bRemote, CString connect_string)
{
   pFolder->AddRef();

   AddFolderStruct item;
   item.m_pShellFolder = pFolder;
   item.m_bJukebox = bJukebox;
   item.m_bRemote = bRemote;
   item.m_connect_string = connect_string;
   m_arrayAddFolders.Add(item);
}

CMdmBrowse::CMdmBrowse(CWnd* pParent /*=NULL*/, UINT nIDTemplate /*=IDD_MDM_BROWSE*/)
//: CDialog(CMdmBrowse::IDD, pParent)
: CDialog(nIDTemplate, pParent)
{
   m_eMapDataFound = UNVERIFIED_MAP_DATA;
   m_bUserTypedFolderName = false;
   m_nIDTemplate = nIDTemplate;
   m_fnFolderContainsMapData = NULL;
   m_bExpanding = false;

   //{{AFX_DATA_INIT(CMdmBrowse)
   m_strFolderName = _T("");
   m_chkJukebox = FALSE;
   //}}AFX_DATA_INIT
}

CMdmBrowse::~CMdmBrowse()
{
}

void CMdmBrowse::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMdmBrowse)
   DDX_Control(pDX, IDC_FOLDER_NAME_EDIT, m_editFolderName);
   DDX_Control(pDX, IDC_DATA_FOUND_RO_MSG, m_ctrlDataFoundRO);
   DDX_Control(pDX, IDC_DATA_FOUND_MSG, m_ctrlDataFound);
   DDX_Control(pDX, IDOK, m_OKButton);
   DDX_Text(pDX, IDC_FOLDER_NAME_EDIT, m_strFolderName);
   DDX_Control(pDX, IDC_FOLDER_VIEW_TREE, m_treeBrowseCtrl);

   if (m_nIDTemplate == IDD_MDM_BROWSE)
   {
      DDX_Check(pDX, IDC_JUKEBOX, m_chkJukebox);
      DDX_Control(pDX, IDC_DEL_FOLDER_BUTTON, m_btnDelFolder);
      DDX_Control(pDX, IDC_ADD_FOLDER_BUTTON, m_btnAddFolder);
      DDX_Control(pDX, IDC_ADD_FOLDERS_LIST, m_listAddFolders);
   }

   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMdmBrowse, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
//{{AFX_MSG_MAP(CMdmBrowse)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(IDC_ADD_FOLDER_BUTTON, OnAddFolderButton)
   ON_BN_CLICKED(IDC_LOCATE_FOLDER_BUTTON, OnLocateFolderButton)
   ON_BN_CLICKED(IDC_DEL_FOLDER_BUTTON, OnDelFolderButton)
   ON_BN_CLICKED(IDCANCEL, OnCancelButton)
   ON_BN_CLICKED(IDC_JUKEBOX, OnJukebox)
   ON_WM_CREATE()
   ON_EN_UPDATE(IDC_FOLDER_NAME_EDIT, OnUpdateFolderNameEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//-----------------------------------------------------------------------------
// Code stub was inserted by ClassWizard for initializing the Dialog Box.
//-----------------------------------------------------------------------------
BOOL CMdmBrowse::OnInitDialog()
{
   CoInitialize(NULL);

   CDialog::OnInitDialog();

   // Hourglass on!
   CWaitCursor temp;

   //   Change the title of this dialog if specified
   if (!m_sWindowText.IsEmpty())
      SetWindowText(m_sWindowText);

   // Attach system image list to list ctrl (for the icons)

   SHFILEINFO      sfi;
   memset(&sfi, 0, sizeof(sfi));

   HIMAGELIST   hSysImageList = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), 0,
                                      &sfi, sizeof(SHFILEINFO),
                                      SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
   if (hSysImageList)
   {
      ListView_SetImageList(m_listAddFolders, hSysImageList, LVSIL_SMALL);

      // create a state image list for the jukebox icon
      BOOL rc = m_jukeboxImageList.Create(::GetSystemMetrics(SM_CXSMICON),
                                ::GetSystemMetrics(SM_CYSMICON), ILC_COLOR, 2, 1);
      ASSERT(rc == TRUE);

      // the list control can't use state icon position 0, so add the icon twice
      HICON hicon = ::LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_JUKEBOX));
      ASSERT(hicon != NULL);
      m_jukeboxImageList.Add(hicon);
      m_jukeboxImageList.Add(hicon);

      if (m_nIDTemplate ==IDD_MDM_BROWSE)
         m_listAddFolders.SetImageList(&m_jukeboxImageList, LVSIL_STATE);
   }

   if (m_nIDTemplate ==IDD_MDM_BROWSE)
      m_listAddFolders.InsertColumn(0, "", LVCFMT_LEFT);

   m_treeBrowseCtrl.FillTree();

   return TRUE;   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}   // CMdmBrowse::OnInitDialog()


//-----------------------------------------------------------------------------
// Code stub was inserted by ClassWizard and is called to handle dialog box
// messages
//-----------------------------------------------------------------------------
BOOL CMdmBrowse::OnNotify(WPARAM   wParam, LPARAM   lParam, LRESULT*   pResult)
{
   NMLISTVIEW*    pnmlv = (NMLISTVIEW*)lParam;
   NMLVKEYDOWN*   pnkd = (NMLVKEYDOWN*)lParam;

   IShellFolderObject *pFolder;

   NMHDR *pnmh = (NMHDR*)lParam;

   switch (pnmh->code)
   {

   ////////////// List Control (CListCtrl) notification messages

   case LVN_DELETEITEM:    // delete item from add folders list control

      pFolder = (IShellFolderObject*)pnmlv->lParam;
      ASSERT(pFolder != NULL);
      pFolder->Release();
   break;

   case LVN_KEYDOWN:
      if (pnkd->wVKey == 0x2e)      // delete key pressed?
         OnDelFolderButton();
   break;

   case NM_KILLFOCUS:
   case LVN_ITEMCHANGED:   // item selected state changed
      pnmlv = (NMLISTVIEW*)lParam;
      if (m_listAddFolders.GetSelectedCount() > 0)   // any items in add folders list selected?
         m_btnDelFolder.EnableWindow(TRUE);          // there are selected items, so enable del
      else
         m_btnDelFolder.EnableWindow(FALSE);
      break;
   }

   return CDialog::OnNotify(wParam, lParam, pResult);
}   // CMdmBrowse::OnNotify

// adds the current folder name in the edit control to the add files list control
void CMdmBrowse::OnAddFolderButton()
{
   // Hourglass on!
   CWaitCursor temp;

   m_ctrlDataFoundRO.ShowWindow(SW_HIDE);
   m_ctrlDataFound.ShowWindow(SW_HIDE);
   UpdateData();

   HWND tree_hwnd = ::GetDlgItem(m_hWnd, IDC_LAYER_TREE);

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

      if (m_eMapDataFound == UNVERIFIED_MAP_DATA)
         m_eMapDataFound = DoesFolderContainMapData(smpShellFolderObject, m_chkJukebox != 0);

      LVITEM lvItem;
      memset(&lvItem, 0, sizeof(lvItem));

      lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
      lvItem.iImage = smpShellFolderObject->GetIconIndex(0); // get the icon for this folder

      lvItem.pszText = (LPSTR)(LPCSTR)m_strFolderName;
      if (m_eMapDataFound == DMDS_MAP_DATA)
      {
         m_remote_paths.SetAt(lvItem.pszText, 1);     // is this necessary
      }
      else
      {
         if (m_eMapDataFound == NO_MAP_DATA)
         {
            if ( DialogNotify::SuppressDialogDisplay( MODAL_DLG_THIS_FOLDER_CONTAINS_NO_MAP_DATA ) == FALSE )
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
            else
               return;
         }

         if (m_chkJukebox) // if this is a jukebox, add the jukebox icon to the item (as the state icon)
         {
            lvItem.mask |= LVIF_STATE;
            lvItem.stateMask = LVIS_STATEIMAGEMASK;
            lvItem.state = INDEXTOSTATEIMAGEMASK(1);
            m_chkJukebox = FALSE;
            UpdateData(FALSE);
         }
      }

      lvItem.lParam = (LPARAM)(IShellFolderObject*)smpShellFolderObject; // store SNShellFolder object with item
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
      ProcessComError(err, _T("CMdmBrowse::OnAddFolderButton"));
   }
}

void CMdmBrowse::OnDelFolderButton()
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
      m_remote_paths.RemoveKey(m_listAddFolders.GetItemText(nItem, 0));
      // deleting item messes up pos so we have to start over
      pos = m_listAddFolders.GetFirstSelectedItemPosition();
   }

   if (m_listAddFolders.GetItemCount() == 0)
      m_OKButton.EnableWindow(FALSE);
}

// When OK is pressed, copy all the IShellFolder objects in the add paths list
// to our persistent m_listAddFolders array.  The add paths list ctrl will be deleted
// when DoModal() completes.

void CMdmBrowse::OnOK()
{
   if (m_nIDTemplate == IDD_MDM_BROWSE)
   {
      for (int i = 0; i < m_listAddFolders.GetItemCount(); i++)
      {
         IShellFolderObject *folder = (IShellFolderObject*)m_listAddFolders.GetItemData(i);

         try
         {
            CString text = m_listAddFolders.GetItemText(i, 0);

            long l;
            m_arrayAddFolders.Add(folder, (m_listAddFolders.GetItemState(i, LVIS_STATEIMAGEMASK) != 0),
               m_remote_paths.Lookup(text, l) ? true : false, text);
         }
         catch (_com_error err)
         {
            ProcessComError(err, _T("CMdmBrowse::OnOK"));
         }
      }
   }

   CDialog::OnOK();
}

void CMdmBrowse::OnJukebox()
{
   UpdateData();
   DisplayMapDataFoundBoxes(GetCurrentShellFolder());
}


BEGIN_EVENTSINK_MAP(CMdmBrowse, CDialog)
    //{{AFX_EVENTSINK_MAP(CMdmBrowse)
   ON_EVENT(CMdmBrowse, IDC_FOLDER_VIEW_TREE, 1 /* FolderSelected */, OnSelectedFolderViewTree, VTS_BSTR)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


// The user has selected a folder

void CMdmBrowse::OnSelectedFolderViewTree(LPCTSTR strFolderName)
{
   if (m_bExpanding)
      return;

   IShellFolderObjectPtr smpShellFolder((IShellFolderObject*)m_treeBrowseCtrl.GetCrntShellFolder());

   if (smpShellFolder == NULL)
   {
      ASSERT(0);     // does this ever happen?
      return; // no current selected folder
   }

   m_bUserTypedFolderName = false;

   DisplayMapDataFoundBoxes(smpShellFolder);
}


void CMdmBrowse::DisplayMapDataFoundBoxes(IShellFolderObjectPtr smpShellFolder)
{
   if (smpShellFolder == NULL)
      return; // no current selected folder

   try
   {
      // Hourglass on!
      CWaitCursor temp;

      // hide both data found msgs
      m_ctrlDataFoundRO.ShowWindow(SW_HIDE);
      m_ctrlDataFound.ShowWindow(SW_HIDE);
      m_strFolderName = (LPCSTR)smpShellFolder->m_bstrFullPathName;

      m_eMapDataFound = DoesFolderContainMapData(smpShellFolder, m_chkJukebox != 0);

      if (m_eMapDataFound != NO_MAP_DATA)
      {
         if (m_eMapDataFound == LOCAL_MAP_DATA)
            ShowMapDataFound(TestRWAccess(m_strFolderName));
         else        //(eMapDataFound == DMDS_MAP_DATA)
            ShowMapDataFound(false);
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMdmBrowse::DisplayMapDataFoundBoxes"));
   }
   UpdateData(FALSE);
}


// If the user has typed in a folder name in the edit control, this method will create an IShellFolderObject for it.
// Otherwise, it returns the IShellFolderObject for the currently selected folder in the tree control.

IShellFolderObjectPtr CMdmBrowse::GetCurrentShellFolder()
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
      ProcessComError(err, _T("CMdmBrowse::GetCurrentShellFolder"));
   }
   ASSERT(smpShellFolderObject != NULL);
   return smpShellFolderObject;
}

enum CMdmBrowse::DATATYPE_FOUND CMdmBrowse::DoesFolderContainMapData(IShellFolderObjectPtr pCrntSNShellFolder, bool bJukebox)
{
   try
   {
      bool bHasMapData = false;
      if (pCrntSNShellFolder == NULL)
         return NO_MAP_DATA; // no current selected folder

      if (m_nIDTemplate == IDD_MDM_BROWSE && m_btnAddFolder.m_hWnd)
         m_btnAddFolder.EnableWindow(TRUE);

      long folderAttributes = pCrntSNShellFolder->m_Attributes;

      // if it's part of the file system & we can copy it, then we can map it
      if ((folderAttributes & SFGAO_FOLDER ) && (folderAttributes & SFGAO_CANCOPY))
      {
         bool bHasMapData = m_fnFolderContainsMapData(*pCrntSNShellFolder, bJukebox);
         return bHasMapData ? LOCAL_MAP_DATA : NO_MAP_DATA;
      }
      else if ((folderAttributes & SFGAO_FILESYSANCESTOR) &&  (folderAttributes & SFGAO_CANLINK))
      {
         CString strServerName = (const char *)pCrntSNShellFolder->m_bstrFullPathName;
         if (PathIsUNCServer(strServerName))
         {
            // It may be a DMDS
            CString sql_server_name = GetSqlServerName(SQL_SERVER_NAME_SHORT).c_str();

            strServerName = strServerName.Mid(2);   // remove slashes before server name
#ifndef USE_POSTGRESQL
            strServerName += "\\" + sql_server_name;
#endif
            strServerName.MakeUpper();

            bool can_connect = false;
            if (MDSWrapper::GetInstance()->RemoteDataAvailable(&can_connect, strServerName) != SUCCESS)
               ERR_report("CMdmBrowse::OnSelectedFolderViewTree(): MDSWrapper::RemoteDataAvailable() failed.");
            else
            {
               m_strFolderName = strServerName;
               return can_connect ? DMDS_MAP_DATA : NO_MAP_DATA;
            }
         }
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMdmBrowse::DoesFolderContainMapData"));
   }
   if (m_nIDTemplate == IDD_MDM_BROWSE && m_btnAddFolder.m_hWnd)
      m_btnAddFolder.EnableWindow(FALSE);
   return NO_MAP_DATA;
}


void CMdmBrowse::OnUpdateFolderNameEdit()
{
   // TODO: If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CDialog::OnInitDialog()
   // function to send the EM_SETEVENTMASK message to the control
   // with the ENM_UPDATE flag ORed into the lParam mask.

   m_bUserTypedFolderName = true;
   m_eMapDataFound = UNVERIFIED_MAP_DATA;

   if (m_btnAddFolder.m_hWnd)
      m_btnAddFolder.EnableWindow(TRUE);
}

void CMdmBrowse::OnLocateFolderButton()
{
   UpdateData();

   if (strlen(m_strFolderName) > 0)
   {
      // Prepending EXPAND causes the tree control to attempt to expand to m_strFolderName
      // This is a minor hack, but it prevents having to rebuild the control's wrapper class.

      m_bExpanding = true;
      m_treeBrowseCtrl.SetTargetDir("EXPAND" + m_strFolderName);
      m_bExpanding = false;
   }
}

bool CMdmBrowse::SearchForItemText(LPCSTR pstrItemText)
{
   for (int i = 0; i < m_listAddFolders.GetItemCount(); i++)
   {
      if (m_listAddFolders.GetItemText(i, 0).CompareNoCase(pstrItemText) == 0)
        return true;
   }
   return false;
}


void CMdmBrowse::ShowMapDataFound(bool readwrite)
{
   if (readwrite)
      m_ctrlDataFound.ShowWindow(SW_SHOW);
   else
      m_ctrlDataFoundRO.ShowWindow(SW_SHOW);
}

void CMdmBrowse::OnCancelButton()
{
   m_bUserTypedFolderName = false; // Prevents tree control from expanding

   CDialog::OnCancel();
}

LRESULT CMdmBrowse::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}