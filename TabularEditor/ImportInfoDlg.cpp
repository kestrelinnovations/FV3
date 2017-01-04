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

// ImportInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\fvw.h"
#include "ImportInfoDlg.h"
#include "ImportPropSheet.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CImportInfoDlg dialog


CImportInfoDlg::CImportInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CImportInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportInfoDlg)
	DDX_Control(pDX, IDC_EDIT, m_edit_ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportInfoDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CImportInfoDlg)
	ON_BN_CLICKED(IDC_SAVE_SETTINGS, OnSaveSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportInfoDlg message handlers

BOOL CImportInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_edit_ctrl.SetWindowText(m_info_text);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportInfoDlg::OnSaveSettings() 
{
	m_property_sheet->OnSaveSettings();
}
LRESULT CImportInfoDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

