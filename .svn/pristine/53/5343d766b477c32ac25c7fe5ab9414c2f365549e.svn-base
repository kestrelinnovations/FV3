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



// prnt_dlg.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "resource.h"
#include "prnt_dlg.h"
#include "printdlg.h"
#include "PrntTool.h"

/////////////////////////////////////////////////////////////////////////////
// custom_print_dialog dialog


custom_print_dialog::custom_print_dialog(BOOL bPrintSetupOnly,
   DWORD dwFlags, CWnd* pParentWnd) : 
      CPrintDialog(bPrintSetupOnly, dwFlags, pParentWnd)
{
	//{{AFX_DATA_INIT(custom_print_dialog)
	//}}AFX_DATA_INIT
}


void custom_print_dialog::DoDataExchange(CDataExchange* pDX)
{
	CPrintDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(custom_print_dialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(custom_print_dialog, CPrintDialog)
	//{{AFX_MSG_MAP(custom_print_dialog)
	ON_BN_CLICKED(IDC_MAP_BUTTON, OnMapButton)
	ON_BN_CLICKED(IDC_PRINT_DLG_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// custom_print_dialog message handlers

BOOL custom_print_dialog::OnInitDialog() 
{
   CPrintDialog::OnInitDialog();

   //
   // resize the "collate" icon in order to display it properly.
   // this is needed because of a limitation in the resource editor
   // that disallows resizing of icon controls
   //
   CRect rect;
   GetDlgItem(ico3)->GetWindowRect(&rect);
   ScreenToClient(&rect);
   rect.InflateRect(0, 0, 87, 6);
   GetDlgItem(ico3)->MoveWindow(&rect);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


void custom_print_dialog::OnMapButton() 
{
   // if we have called the print dialog from the print tool
   // then use the alternate dialog box for map options which
   // removes the 'Map Area Options' group
   if (CPrintToolOverlay::get_print_from_tool())
   {
      CPrintOptDlg dlg("Use Alternate Dialog");
      dlg.DoModal();
   }
   else
   {
      CPrintOptDlg dlg;
      dlg.DoModal();
   }
}

void custom_print_dialog::OnHelp()
{
	AfxGetApp()->WinHelp(HID_BASE_RESOURCE + IDC_CUSTOM_PRINT_DLG_HELP);
}
