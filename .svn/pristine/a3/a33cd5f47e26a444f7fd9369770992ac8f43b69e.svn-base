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



// shp_brws.h : implementation file
// 


#include "stdafx.h"
#include "BrowseFolderDlg.h"
#include "..\Common\ProcessComError.h"
#include "getobjpr.h"


BrowseFolderDlg::BrowseFolderDlg(CWnd* pParent /*=NULL*/)
: CDialog(BrowseFolderDlg::IDD, pParent)
{
   set_target_directory("");

	//{{AFX_DATA_INIT(BrowseFolderDlg)
	m_CurrentSelection = _T("");
	//}}AFX_DATA_INIT
}


void BrowseFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BrowseFolderDlg)
	DDX_Control(pDX, IDOK, m_ctlIDOK);
	DDX_Control(pDX, IDC_FILE_FOUND_MSG, m_ctrFileFoundMsg);
	DDX_Text(pDX, IDC_AddDirectoryName, m_CurrentSelection);
	DDX_Control(pDX, IDC_BROWSEFORFOLDERCTRL, m_ctrlBrowseForFolder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BrowseFolderDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
//{{AFX_MSG_MAP(BrowseFolderDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/****************************************************************************
*																									 *
*	C  L  A  S  S      W  I  Z  A  R  D      G  E  N  E  R  A  T  E  D		 *
*							C  O  D  E      S  T  U  B  S									 *
*																									 *
/***************************************************************************/


//-----------------------------------------------------------------------------
// Code stub was inserted by ClassWizard for initializing the Dialog Box.
//-----------------------------------------------------------------------------

BOOL BrowseFolderDlg::OnInitDialog() 
{
   CoInitialize(NULL);

	CDialog::OnInitDialog();
	
	// Hourglass on!
	CWaitCursor temp;
	
	//	Change the title of this dialog if specified
	if (!m_sWindowText.IsEmpty())
		SetWindowText(m_sWindowText);

	//
	// Now, if the Class member m_InitialDir is not empty, then try to parse it
	// and walk down it's directory structure so that we can begin the tree display
	// there. If it does not exist, display a warning box.
	//

	if (strlen(get_target_directory()) > 0)
      m_ctrlBrowseForFolder.SetTargetDir(get_target_directory());

   if (m_ctrlBrowseForFolder.FillTree() != S_OK)
   	AfxMessageBox((CString)get_target_directory() + " not found.", MB_OK | MB_ICONINFORMATION, 0);

	// if we're not looking for a particular file, let them pick anything
	if (m_strRequestedFilename.GetLength() == 0)
		m_ctlIDOK.EnableWindow(TRUE);

	return TRUE;	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}	// DirSelectTreeControl::OnInitDialog() 


// ************************************************************************
// ************************************************************************


BEGIN_EVENTSINK_MAP(BrowseFolderDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(BrowseFolderDlg)
	ON_EVENT(BrowseFolderDlg, IDC_BROWSEFORFOLDERCTRL, 1 /* FolderSelected */, OnFolderSelectedDirViewTree, VTS_BSTR)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// The user has selected a folder

void BrowseFolderDlg::OnFolderSelectedDirViewTree(LPCTSTR strFolderName) 
{
	try
	{
		UpdateData();

		m_CurrentSelection = strFolderName;    // save folder name

		set_target_directory(m_CurrentSelection);

		// are we looking for a particular file?
      /*
		if (m_strRequestedFilename.GetLength())
		{
			IShellFolderObject* pSNShellFolder = (IShellFolderObject*)m_ctrlBrowseForFolder.GetCrntShellFolder();

			// Enumerate through this folder's children
			pSNShellFolder->RestartChildren();

			// loop through each child at current level

			while(1)
			{
				_bstr_t	bstrChildName = pSNShellFolder->GetNextChildName()->m_bstrDisplayName;
				if (bstrChildName.length() == 0)
					break;

				if (m_strRequestedFilename.CompareNoCase(bstrChildName) == 0)
				{
					m_ctrFileFoundMsg.ShowWindow(SW_SHOW);
					m_ctlIDOK.EnableWindow(TRUE);
					m_strRequestedFileFullPath = (char*)(pSNShellFolder->PathName + "\\" + bstrChildName);
				}
			}
   
			pSNShellFolder->Release();
		}
      */

		UpdateData(FALSE);
	}

   catch (_com_error err)
	{
		ProcessComError(err, _T("BrowseFolderDlg::OnMdmcatalogInstall"));
	}

}
LRESULT BrowseFolderDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

