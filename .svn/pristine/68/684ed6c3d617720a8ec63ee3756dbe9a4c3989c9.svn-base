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



// DataChk.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DataChk.h"
#include "getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CDataCheckDlg dialog


CDataCheckDlg::CDataCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataCheckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataCheckDlg)
	m_cov_file_check = FALSE;
	m_remove_masking = FALSE;
	//}}AFX_DATA_INIT
}


void CDataCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataCheckDlg)
	DDX_Check(pDX, IDC_COV_FILE_CHECK, m_cov_file_check);
	DDX_Check(pDX, IDC_REMOVE_MASKING, m_remove_masking);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataCheckDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CDataCheckDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataCheckDlg message handlers

BOOL CDataCheckDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   m_cov_file_check = TRUE;
	m_remove_masking = FALSE;

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataCheckDlg::OnOK() 
{
   UpdateData(TRUE);
   
	CDialog::OnOK();
}
LRESULT CDataCheckDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

