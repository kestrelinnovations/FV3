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



// nt_warn.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "nt_warn.h"
#include "appinfo.h"
#include "param.h"
#include "getobjpr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// nt_full_drag_warning dialog


nt_full_drag_warning::nt_full_drag_warning(CWnd* pParent /*=NULL*/)
	: CDialog(nt_full_drag_warning::IDD, pParent)
{
	//{{AFX_DATA_INIT(nt_full_drag_warning)
	m_disable_msg = FALSE;
	//}}AFX_DATA_INIT
}


void nt_full_drag_warning::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(nt_full_drag_warning)
	DDX_Check(pDX, IDC_NOSHOWMSG_CHK, m_disable_msg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(nt_full_drag_warning, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(nt_full_drag_warning)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// nt_full_drag_warning message handlers


BOOL nt_full_drag_warning::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(appShortName());

	SetDlgItemText(IDC_NT_WARN_MSG, 
		"Full Drag is enabled.\n"+appShortName()+
		" will not run properly with Full Drag enabled.\n\n"
		"To disable Full Drag:\n\nOpen the Desktop program "
		"item in Control Panel in the Main program group in "
		"Program Manager and then disable "
		"\"Full Drag\" in the Applications box.");

	return TRUE;	// return TRUE unless you set the focus to a control
}	// nt_full_drag_warning::OnInitDialog() 


void nt_full_drag_warning::OnOK() 
{
    UpdateData(TRUE);
    int val = m_disable_msg ? 0 : 1;
    PRM_set_registry_int("Main", "FullDragWarning", val);

	CDialog::OnOK();
}
LRESULT nt_full_drag_warning::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

