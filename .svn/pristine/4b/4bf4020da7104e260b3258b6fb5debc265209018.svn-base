// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// MdmBrowse.cpp : implementation file
// 

#include "stdafx.h"
#include "NitfBrowse.h"
#include "Common\ProcessComError.h"
#include "nitf_util.h"
#include "..\getobjpr.h"





// ******************************************************************************
// ******************************************************************************

CNitfBrowse::CNitfBrowse(CWnd* pParent /*=NULL*/, UINT nIDTemplate /*=IDD_MDM_BROWSE*/)
//: CDialog(CNitfBrowse::IDD, pParent)
: CDialog(nIDTemplate, pParent)
{
	set_target_directory("");
	m_bUserTypedFolderName = false;
	m_nIDTemplate = nIDTemplate;

	//{{AFX_DATA_INIT(CNitfBrowse)
	m_strFolderName = _T("");
	m_chkJukebox = FALSE;
	//}}AFX_DATA_INIT
}

// ******************************************************************************
// ******************************************************************************

CNitfBrowse::~CNitfBrowse()
{
}


// ******************************************************************************
// ******************************************************************************

void CNitfBrowse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNitfBrowse)
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


// ******************************************************************************
// ******************************************************************************

BEGIN_MESSAGE_MAP(CNitfBrowse, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
//{{AFX_MSG_MAP(CNitfBrowse)
	ON_BN_CLICKED(IDC_ADD_FOLDER_BUTTON, OnAddFolderButton)
	ON_BN_CLICKED(IDC_DEL_FOLDER_BUTTON, OnDelFolderButton)
	ON_BN_CLICKED(IDC_JUKEBOX, OnJukebox)
	ON_WM_CREATE()
	ON_EN_UPDATE(IDC_FOLDER_NAME_EDIT, OnUpdateFolderNameEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// ******************************************************************************
// ******************************************************************************

//-----------------------------------------------------------------------------
// Code stub was inserted by ClassWizard for initializing the Dialog Box.
//-----------------------------------------------------------------------------
BOOL CNitfBrowse::OnInitDialog() 
{
	CoInitialize(NULL);

	CDialog::OnInitDialog();

	// Hourglass on!
	CWaitCursor temp;

	//	Change the title of this dialog if specified
	if (!m_sWindowText.IsEmpty())
		SetWindowText(m_sWindowText);

	// Attach system image list to list ctrl (for the icons)

	SHFILEINFO		sfi;
	memset(&sfi, 0, sizeof(sfi));

	HIMAGELIST	hSysImageList = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), 0,
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

	//
	// Now, if the Class member m_InitialDir is not empty, then try to parse it
	// and walk down it's directory structure so that we can begin the tree display
	// there. If it does not exist, display a warning box.
	//

	if (strlen(get_target_directory()) > 0)
		m_treeBrowseCtrl.SetTargetDir(get_target_directory());

	m_treeBrowseCtrl.FillTree();

	return TRUE;	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}	// CNitfBrowse::OnInitDialog() 


// ******************************************************************************
// ******************************************************************************

//-----------------------------------------------------------------------------
// Code stub was inserted by ClassWizard and is called to handle dialog box 
// messages
//-----------------------------------------------------------------------------
BOOL CNitfBrowse::OnNotify(WPARAM	wParam, LPARAM	lParam, LRESULT*	pResult) 
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
}	// CNitfBrowse::OnNotify



// See if this folder has map data

bool CNitfBrowse::ShellFolderContainMapData(IShellFolderObject &Folder, bool bSearchNextLevel) const
{
	CString strPathName = (LPCSTR)Folder.m_bstrFullPathName;
	CString pathname;
	CNitfUtil util;

	// add a trailing slash if there isn't one
	if ( strPathName.Right( 1 ) != "\\" )
		strPathName += "\\";

//      if ( TIF_file_exists(strPathName) || VPF_database_exists(strPathName) )
//         return true;
      

	if (util.geo_nitf_file_exists(strPathName))
		return TRUE;

	if (util.geo_tiff_file_exists(strPathName))
		return TRUE;

	if (util.geo_mrsid_file_exists(strPathName))
		return TRUE;

	if (!bSearchNextLevel)
		return FALSE;

	try
	{
		// Enumerate through this folder's children
		Folder.RestartChildren();

		IShellFolderObjectPtr pFolderChild;

		// loop through each child at current level
		while ( (pFolderChild = Folder.GetNextChild()) != NULL)
		{
			long folderAttributes = pFolderChild->m_Attributes;

			if (folderAttributes & SFGAO_FOLDER)
			{
				// get the friendly name of this child folder
				CString strFriendlyName = (LPCSTR)pFolderChild->m_bstrDisplayName;
				strFriendlyName.MakeUpper( );

				pathname = strPathName;
				pathname += "\\";
				pathname += strFriendlyName;
				pathname += "\\";

				if (util.geo_nitf_file_exists(pathname))
					return TRUE;

				if (util.geo_tiff_file_exists(pathname))
					return TRUE;

				if (util.geo_mrsid_file_exists(pathname))
					return TRUE;

				// are we supposed to check sub folders (children)?
				if (bSearchNextLevel)
				{
					if (ShellFolderContainMapData(pFolderChild, false))   // search recursively down tree
						return true;
				}
			}
		}
	}
	catch (_com_error err)
	{
		ProcessComError(err, _T("CNitfBrowse::ShellFolderContainMapData"));
	}

	return false;
}

// ******************************************************************************
// ******************************************************************************

// adds the current folder name in the edit control to the add files list control

void CNitfBrowse::OnAddFolderButton() 
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
		IShellFolderObjectPtr	pSNShellFolder;

		if (m_bUserTypedFolderName)
		{
			HRESULT hr = pSNShellFolder.CreateInstance(__uuidof(ShellFolderObject));
			if (hr != S_OK)
				return;	// need a better error handler

			hr = pSNShellFolder->AttachToFolder(_bstr_t(m_strFolderName));
			if (hr != S_OK)
			{
				AfxMessageBox("   This folder can not be accessed.   ", MB_OK);
				return;
			}
		}

		else
		{
			// Get the SNShellFolder object for the selected entry in the tree control
			pSNShellFolder = (IShellFolderObject*)m_treeBrowseCtrl.GetCrntShellFolder();
			ASSERT(pSNShellFolder != NULL);
		}	

		long	folderAttributes = pSNShellFolder->m_Attributes;
		bool	bHasMapData = ShellFolderContainMapData(*pSNShellFolder, m_chkJukebox != 0);
		set_target_directory(m_strFolderName, bHasMapData);
		if (bHasMapData)
			ShowMapDataFound(folderAttributes);

		if (m_bTargetHasMapData == false)
		{
			//TO DO: kevin: put this in the string table
			if (AfxMessageBox("This folder contains no map data.\n\n"
				"You should only pick a map data folder that displays \"Map data found\" \n"
				"at the bottom of the dialog when you select it.  The only exception is \n"
				"when you are adding a brand new empty folder that you are going to\n"
				"copy map data to.\n\n"
				"Are you sure that you want to add this folder?",
				MB_YESNO) != IDYES)
			return;
		}

		//m_chkJukebox = FALSE;

		LVITEM   lvItem;
		memset(&lvItem, 0, sizeof(lvItem));

		lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;

		lvItem.iImage = pSNShellFolder->GetIconIndex(0);	// get the icon for this folder
		_bstr_t		bstrPathName = pSNShellFolder->m_bstrFullPathName;

		// if this is a jukebox, add the jukebox icon to the item (as the state icon)
		if (m_chkJukebox)
		{
         TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
         bstrPathName += "+";
         lvItem.pszText = bstrPathName;
			lvItem.mask |= LVIF_STATE;
			lvItem.stateMask = LVIS_STATEIMAGEMASK;
			lvItem.state = INDEXTOSTATEIMAGEMASK(1);
			m_chkJukebox = FALSE;
			UpdateData(FALSE);
		}
      else
      {
         lvItem.pszText = bstrPathName;
      }

		lvItem.lParam = (LPARAM)(IShellFolderObject*)pSNShellFolder;		// store SNShellFolder object with item
		pSNShellFolder->AddRef();

		// insert new item into list control
		int n = m_listAddFolders.InsertItem(&lvItem);
		ASSERT(n != -1);

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
		ProcessComError(err, _T("CNitfBrowse::OnAddFolderButton"));
	}


}


// ******************************************************************************
// ******************************************************************************

void CNitfBrowse::OnDelFolderButton() 
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
		// deleting item messes up pos so we have to start over
		pos = m_listAddFolders.GetFirstSelectedItemPosition();
	}

	if (m_listAddFolders.GetItemCount() == 0)
		m_OKButton.EnableWindow(FALSE);
}


// ******************************************************************************
// ******************************************************************************

// When OK is pressed, copy all the IShellFolder objects in the add paths list
// to our persistent m_listAddFolders array.  The add paths list ctrl will be deleted
// when DoModal() completes.

void CNitfBrowse::OnOK() 
{
	if (m_nIDTemplate == IDD_MDM_BROWSE)
	{
		for (int i = 0; i < m_listAddFolders.GetItemCount(); i++)
		{
		m_arrayAddFolders.Add((IShellFolderObject*)m_listAddFolders.GetItemData(i),
							(m_listAddFolders.GetItemState(i, LVIS_STATEIMAGEMASK) != 0) );
		}
	}

	CDialog::OnOK();
}


// ******************************************************************************
// ******************************************************************************

void CNitfBrowse::OnJukebox() 
{
   UpdateData();

   // if they toggled jukebox option, then recheck for map data
   OnSelectedFolderViewTree(m_strFolderName);
}




// ******************************************************************************
// ******************************************************************************

BEGIN_EVENTSINK_MAP(CNitfBrowse, CDialog)
    //{{AFX_EVENTSINK_MAP(CNitfBrowse)
	ON_EVENT(CNitfBrowse, IDC_FOLDER_VIEW_TREE, 1 /* FolderSelected */, OnSelectedFolderViewTree, VTS_BSTR)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// ******************************************************************************
// ******************************************************************************

// The user has selected a folder

void CNitfBrowse::OnSelectedFolderViewTree(LPCTSTR strFolderName) 
{
	UpdateData();

	try
	{
		IShellFolderObjectPtr	pCrntSNShellFolder((IShellFolderObject*)m_treeBrowseCtrl.GetCrntShellFolder());

		if (pCrntSNShellFolder == NULL)
			return;     // no current selected folder

		// Hourglass on!
		CWaitCursor temp;

		m_strFolderName = strFolderName;    // save folder name
		// hide both data found msgs
		m_ctrlDataFoundRO.ShowWindow(SW_HIDE);
		m_ctrlDataFound.ShowWindow(SW_HIDE);
		m_bUserTypedFolderName = false;

		long folderAttributes = pCrntSNShellFolder->m_Attributes;

		// if it's part of the file system & we can link to it, then we can map it
		if ( (folderAttributes & SFGAO_FILESYSTEM ) &&  (folderAttributes & SFGAO_CANLINK) )
		{
			bool bHasMapData = ShellFolderContainMapData(*pCrntSNShellFolder, m_chkJukebox != 0);
			set_target_directory(m_strFolderName, bHasMapData);
			if (bHasMapData)
				ShowMapDataFound(folderAttributes);

			if (m_nIDTemplate == IDD_MDM_BROWSE)
				m_btnAddFolder.EnableWindow(TRUE);
		}
		else
		{
			// we can't use the selected folder
			set_target_directory("");
			if (m_nIDTemplate == IDD_MDM_BROWSE)
				m_btnAddFolder.EnableWindow(FALSE);
		}
   }
   catch (_com_error err)
	{
		ProcessComError(err, _T("CNitfBrowse::OnSelectedFolderViewTree"));
	}


   UpdateData(FALSE);
}


// ******************************************************************************
// ******************************************************************************

void CNitfBrowse::OnUpdateFolderNameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	m_bUserTypedFolderName = true;
	m_btnAddFolder.EnableWindow(TRUE);
}


// ******************************************************************************
// ******************************************************************************

bool CNitfBrowse::SearchForItemText(LPCSTR pstrItemText) 
{
   for (int i = 0; i < m_listAddFolders.GetItemCount(); i++)
   {
      if (m_listAddFolders.GetItemText(i, 0).CompareNoCase(pstrItemText) == 0)
		  return true;
   }
   return false;
}


// ******************************************************************************
// ******************************************************************************

void CNitfBrowse::ShowMapDataFound(long folderAttributes)
{
	// is it a read only folder?
	if ( (folderAttributes & SFGAO_READONLY) || !(folderAttributes & SFGAO_CANDELETE) )
		m_ctrlDataFoundRO.ShowWindow(SW_SHOW);
	else
		m_ctrlDataFound.ShowWindow(SW_SHOW);
}

// ******************************************************************************
// ******************************************************************************
LRESULT CNitfBrowse::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

