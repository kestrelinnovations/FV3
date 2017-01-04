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



// sopendlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "sopendlg.h"
#include "getobjpr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectOpenDlg dialog


CSelectOpenDlg::CSelectOpenDlg(CWnd* pParent)
	: CDialog(CSelectOpenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectOpenDlg)
	//}}AFX_DATA_INIT
}


void CSelectOpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectOpenDlg)
	DDX_Control(pDX, IDC_LIST1, m_list_overlay_types);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectOpenDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CSelectOpenDlg)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSelectOpenDlg message handlers

BOOL CSelectOpenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int index,count;
    for	(count=0;m_types[count]!=NULL;count++)
	{
		index = m_list_overlay_types.InsertString(count,m_types[count]);
		if ((index==LB_ERR)||(index==LB_ERRSPACE))
			return FALSE;
	} 
	m_list_overlay_types.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectOpenDlg::OnDblclkList1() 
{
	OnOK();	
}

void CSelectOpenDlg::OnOK() 
{
	m_overlay_selected = m_list_overlay_types.GetCurSel();
	CDialog::OnOK();
}
LRESULT CSelectOpenDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

