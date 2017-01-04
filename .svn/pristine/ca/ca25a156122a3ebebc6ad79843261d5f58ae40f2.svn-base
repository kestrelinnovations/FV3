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

// AboutDlg.cpp

#include "stdafx.h"

#include "AboutDlg.h"
#include "appinfo.h"
#include "..\getobjpr.h"


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD),
   m_lInitialPage(0)
{
   //{{AFX_DATA_INIT(CAboutDlg)
   //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
   const int APPINFOSTR_LEN = 256;
	char		appInfoStr[APPINFOSTR_LEN];
	char		appVersion[MAX_VERSION_SIZE];
	char		appName[80];
	char		appCopyright[80];

	strcpy_s(appInfoStr, APPINFOSTR_LEN, project_product_name(appName, 80));
	strcat_s(appInfoStr, APPINFOSTR_LEN, ", version ");
	strcat_s(appInfoStr, APPINFOSTR_LEN, project_version_str(appVersion, MAX_VERSION_SIZE));
	strcat_s(appInfoStr, APPINFOSTR_LEN, "\n");
	strcat_s(appInfoStr, APPINFOSTR_LEN, project_copyright(appCopyright, 80));
	strcat_s(appInfoStr, APPINFOSTR_LEN, "\nAll rights reserved.");

   SetDlgItemText(IDC_FalconView_Version_Block, appInfoStr);

   //
   //  set up the hotlinks in the dialog
   //
   m_website.SubclassDlgItem(IDC_FV_WEB_SITE, this);
   m_mission_planning_website.SubclassDlgItem(IDC_MP_WEB_SITE, this);

   // set the initial page displayed in the contacts OCX control
   //
   CWnd *pContactsOCX = GetDlgItem(IDC_ABOUTBOX1);
   if (pContactsOCX != NULL)
      pContactsOCX->SetProperty(0x1, VT_I4, m_lInitialPage);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
LRESULT CAboutDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

