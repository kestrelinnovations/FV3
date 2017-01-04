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

// mdmcatprint.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "mdmcatprint.h"
#include "..\getobjpr.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMCatPrint dialog


CMDMCatPrint::CMDMCatPrint(CWnd* pParent /*=NULL*/)
	: CDialog(CMDMCatPrint::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMDMCatPrint)
	m_sPrintType = _T("");
	//}}AFX_DATA_INIT
}


void CMDMCatPrint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDMCatPrint)
	DDX_Control(pDX, IDC_MDMCATPRINT_LIST, m_PrintListCtrl);
	DDX_LBString(pDX, IDC_MDMCATPRINT_LIST, m_sPrintType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMCatPrint, CDialog)
	//{{AFX_MSG_MAP(CMDMCatPrint)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_BN_CLICKED(ID_MDMCATPRINT_OK, OnMdmcatprintOk)
	ON_BN_CLICKED(ID_MDMCATPRINT_CANCEL, OnMdmcatprintCancel)
	ON_BN_CLICKED(IDC_HELP_MDM_CATPRINT, OnHelpMdmCatprint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMCatPrint message handlers

BOOL CMDMCatPrint::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   m_PrintListCtrl.AddString("Send list to printer");
   m_PrintListCtrl.AddString("Save list as an XML file");

   m_nChoice = m_PrintListCtrl.SetCurSel(0);

   m_nChoice = cUnknown;   // Set to unknown choice in case we Cancel.
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CMDMCatPrint::GetChoice()
{
   return m_nChoice;
}

void CMDMCatPrint::OnMdmcatprintOk() 
{
   m_nChoice = m_PrintListCtrl.GetCurSel();
	CDialog::OnOK();
}

void CMDMCatPrint::OnMdmcatprintCancel() 
{
	CDialog::OnCancel();
}

void CMDMCatPrint::OnHelpMdmCatprint() 
{
	SendMessage(WM_COMMAND, ID_HELP, 0);
}

LRESULT CMDMCatPrint::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

