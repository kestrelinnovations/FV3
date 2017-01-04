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

// sel_icon.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "param.h"
#include "err.h"
#include "sel_icon.h"
#include "..\getobjpr.h"


CImageList		CIconSelectDlg::m_large_images;
CIconSelectDlg*	CIconSelectDlg::m_icon_dlg = NULL;

/////////////////////////////////////////////////////////////////////////////
// CIconSelectDlg dialog


CIconSelectDlg::CIconSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIconSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIconSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


// *****************************************************************
// *****************************************************************

void CIconSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIconSelectDlg)
	DDX_Control(pDX, IDC_ICON_LIST, m_icon_list);
	//}}AFX_DATA_MAP
}


// *****************************************************************
// *****************************************************************

BEGIN_MESSAGE_MAP(CIconSelectDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CIconSelectDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_ICON_LIST, OnDblclkIconList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconSelectDlg message handlers

void CIconSelectDlg::OnOK() 
{
	int pos;
	char buf[201];

	int cnt = m_icon_list.GetItemCount();

	if (cnt > 0)
	{
		pos = m_icon_list.GetNextItem(-1, LVNI_SELECTED);
		if (pos >= 0)
		{
//			m_icon_text = m_icon_name.GetAt(pos);
			m_icon_list.GetItemText(pos, 0, buf, 200);
			m_icon_text = buf;
		}
	}

	remove_all();
	
	CDialog::OnOK();
}

// *****************************************************************
// *****************************************************************

void CIconSelectDlg::OnCancel() 
{
	remove_all();

	CDialog::OnCancel();
}

// *****************************************************************
// *****************************************************************

void CIconSelectDlg::remove_all()
{
	m_icon_list.DeleteAllItems();
}

// *****************************************************************
// *****************************************************************

CString CIconSelectDlg::get_icon_text() 
{
	return m_icon_text;
}

// *****************************************************************
// *****************************************************************

void CIconSelectDlg::OnDblclkIconList(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	OnOK();
	
//	*pResult = 0;
}

// *****************************************************************
// *****************************************************************

BOOL CIconSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// create the image list: 32x32, no masks, no initial items

	if (m_large_images.m_hImageList)
      m_large_images.DeleteImageList();
	m_large_images.Create(32, 32, TRUE, 10, 500);
	
	// associate the image list with the control
	m_icon_list.SetImageList(&m_large_images, LVSIL_NORMAL);

	load_dir(m_icon_dir);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// *****************************************************************
// *****************************************************************

void CIconSelectDlg::add_icon(CIconImage* add_icon)
{
	// add the icon and grab the position index
	int new_pos_large = m_large_images.Add(add_icon->m_large_icon);

	int rslt = m_icon_list.InsertItem(LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, new_pos_large, 
		add_icon->m_title, (UINT)0, (UINT)0, new_pos_large, 0);
	m_icon_name.SetAtGrow(new_pos_large, add_icon->m_title);
}

// *****************************************************************
// *****************************************************************

int CIconSelectDlg::load_dir(CString item_directory)
{
	CIconImage*			new_image;
	CString				search_dir;
	CString				temp_fn;
	WIN32_FIND_DATA	findFileData;
	CString				new_title;
	CString				icon_root = PRM_get_registry_string("Main", "HD_DATA")+
																			"\\icons\\";
	
	// Determine first if this is an aliased directory... if it is then we'll
	// need to (1) search on the aliases original, and (2) load on the 
	// alias.
	CString orig_directory = CIconImage::get_original_directory(item_directory);
	if (orig_directory != "")
	{
		// if we got here, this IS an alias
		search_dir = orig_directory;
	}
	else
		search_dir = item_directory;

	// get the first filename match
	HANDLE h_dirSearch = 
		FindFirstFile(icon_root+search_dir + "\\*.ico", &findFileData);

	if (h_dirSearch == INVALID_HANDLE_VALUE)
	{
		LPVOID lpMsgBuf;
 
		// output the error return message and return FAILURE
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			 NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			 (LPTSTR) &lpMsgBuf,	 0, NULL);
		ERR_report((char*)lpMsgBuf);
		return FAILURE;
	}

	do
	{
		// get the filename used for the icon load
		temp_fn = item_directory+"\\"+findFileData.cFileName;

		// create a new object for this icon load
		if (new_image = CIconImage::load_images(temp_fn))
			add_icon(new_image);

	} while (FindNextFile(h_dirSearch, &findFileData));

	FindClose(h_dirSearch);
	return SUCCESS;
}

// *****************************************************************
// *****************************************************************

void CIconSelectDlg::set_dir(CString item_directory)
{
	m_icon_dir = item_directory;
}

// *****************************************************************
// *****************************************************************
LRESULT CIconSelectDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

