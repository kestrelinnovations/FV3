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

// TGSummary.cpp : implementation file
//

#include "stdafx.h"
#include "..\fvw.h"
#include "TGSummary.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CTGSummary dialog


CTGSummary::CTGSummary(CWnd* pParent /*=NULL*/)
	: CDialog(CTGSummary::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTGSummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTGSummary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTGSummary)
	DDX_Control(pDX, IDC_HTML, m_html_ctrl);
	DDX_Control(pDX, IDC_EDIT1, m_edit_ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTGSummary, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CTGSummary)
	ON_BN_CLICKED(IDC_HTML, OnHtml)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTGSummary message handlers

BOOL CTGSummary::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_edit_ctrl.SetWindowText(m_info_text);

	if (!m_show_html)
		m_html_ctrl.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTGSummary::OnHtml() 
{
	CString full_path = m_output_path + "\\index.html";
	ShellExecute(AfxGetMainWnd()->m_hWnd, "open", full_path, NULL, NULL, SW_SHOWNORMAL);
}
LRESULT CTGSummary::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

