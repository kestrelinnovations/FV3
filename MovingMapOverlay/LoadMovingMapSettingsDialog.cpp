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

// LoadMovingMapSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "LoadMovingMapSettingsDialog.h"
#include "param.h"

// specifies the size, in chars, of the buffer pointed to by lpstrFile used to store the selected file name(s)
const int SELECTED_FILE_NAMES_BUF_SIZE = 32767;

/////////////////////////////////////////////////////////////////////////////
// CLoadMovingMapSettingsDialog

IMPLEMENT_DYNAMIC(CLoadMovingMapSettingsDialog, CFileDialog)

CLoadMovingMapSettingsDialog::CLoadMovingMapSettingsDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
   m_ofn.Flags |= OFN_EXPLORER | OFN_ENABLETEMPLATE;
   m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_LOAD_MOVING_MAP_SETTINGS);

   // extend the default buffer size of MAX_PATH to allow the user to select a large number of .mms files with long path names
   char* pszFile = new char[SELECTED_FILE_NAMES_BUF_SIZE];
   memset(pszFile, 0, SELECTED_FILE_NAMES_BUF_SIZE);
   m_ofn.nMaxFile = SELECTED_FILE_NAMES_BUF_SIZE;
   m_ofn.lpstrFile = pszFile;
}

CLoadMovingMapSettingsDialog::~CLoadMovingMapSettingsDialog()
{
   delete [] m_ofn.lpstrFile;
}

void CLoadMovingMapSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadMovingMapSettingsDialog)
	DDX_Check(pDX, IDC_AUTO_CONNECT, m_bAutoConnect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadMovingMapSettingsDialog, CFileDialog)
	//{{AFX_MSG_MAP(CLoadMovingMapSettingsDialog)
   ON_BN_CLICKED(IDC_AUTO_CONNECT, OnAutoConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CLoadMovingMapSettingsDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	m_bAutoConnect = PRM_get_registry_int("GPS Options", "mmsAutoConnect", 1);
   UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoadMovingMapSettingsDialog::OnAutoConnect()
{
   UpdateData();
   PRM_set_registry_int("GPS Options", "mmsAutoConnect", m_bAutoConnect);
}
