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

// WARP_QueryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WARP_QueryDlg.h"
#include "..\getobjpr.h"



// CWARP_QueryDlg dialog

IMPLEMENT_DYNAMIC(CWARP_QueryDlg, CDialog)

CWARP_QueryDlg::CWARP_QueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWARP_QueryDlg::IDD, pParent)
{

}

CWARP_QueryDlg::~CWARP_QueryDlg()
{
}

void CWARP_QueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWARP_QueryDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_BN_CLICKED(IDC_BUTTON1, &CWARP_QueryDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CWARP_QueryDlg message handlers

void CWARP_QueryDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}
LRESULT CWARP_QueryDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

