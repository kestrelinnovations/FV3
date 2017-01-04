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




// MultiFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fvw.h"
#include "MultiFileDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CMultiFileDialog - custom file dialog
//
// CFileDialog Derived Class
// This class was created to generate a customized version of the standard MFC File Open dialog
// that incorporates a list box containing all available overlay type names.
//

IMPLEMENT_DYNAMIC(CMultiFileDialog, CFileDialog)


CMultiFileDialog::CMultiFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, const char *class_name, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
      m_class_name = class_name;		
      m_open_not_save = bOpenFileDialog;
}


BEGIN_MESSAGE_MAP(CMultiFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CMultiFileDialog)
//	ON_LBN_DBLCLK(IDC_OVERLAY_TYPE_LIST, OnDblclkOverlayTypeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMultiFileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	// TODO: Add extra initialization here.....................

	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMultiFileDialog::OnDblclkOverlayTypeList() 
{
	// TODO: Add your control notification handler code here

	// Add code to set the dialog folder to match the double-clicked string...
	//
	//
	
}
