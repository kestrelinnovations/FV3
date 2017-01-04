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



// ExpirationDialog.cpp : implementation file
//
 

#include "stdafx.h"
#include "resource.h"
#include "ExpirationDialog.h"
#include "utils\appexpiration.h"
#include "getobjpr.h"
#include "getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CExpirationDialog dialog


CExpirationDialog::CExpirationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CExpirationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExpirationDialog)
	m_date_code = _T("");
	//}}AFX_DATA_INIT
}


void CExpirationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpirationDialog)
	DDX_Text(pDX, IDC_DATE_CODE, m_date_code);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExpirationDialog, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CExpirationDialog)
	ON_WM_GETDLGCODE()
	ON_BN_CLICKED(ID_HELP, OnHelp) 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExpirationDialog message handlers

BOOL CExpirationDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

   m_email.SubclassDlgItem(IDC_EMAIL, this);
   m_email.m_link = "mailto:falconview@gtri.gatech.edu";

   m_url.SubclassDlgItem(IDC_URL, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

UINT CExpirationDialog::OnGetDlgCode() 
{
	return CDialog::OnGetDlgCode() | DLGC_WANTCHARS | DLGC_WANTALLKEYS;
}

void CExpirationDialog::OnOK() 
{
	UpdateData();

	//set the application expiration date
	if (!m_date_code.IsEmpty())
		fvw_get_app()->m_expiration->set_expiration_date(m_date_code);
		
	CDialog::OnOK();
}

void CExpirationDialog::OnHelp() 
{
	// help technical support
	AfxGetApp()->WinHelp(0x180A9);	
}
LRESULT CExpirationDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

