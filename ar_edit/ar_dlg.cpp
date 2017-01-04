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

// ar_dlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "ar_dlg.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CUserArAnchorParamsDlg dialog


CUserArAnchorParamsDlg::CUserArAnchorParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserArAnchorParamsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserArAnchorParamsDlg)
	m_length_str = _T("");
	m_width_str = _T("");
	//}}AFX_DATA_INIT

	m_obj = NULL;
}


void CUserArAnchorParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserArAnchorParamsDlg)
	DDX_Text(pDX, IDC_AR_LENGTH, m_length_str);
	DDX_Text(pDX, IDC_AR_WIDTH, m_width_str);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserArAnchorParamsDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CUserArAnchorParamsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserArAnchorParamsDlg message handlers

void CUserArAnchorParamsDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

// *****************************************************************
// *****************************************************************

void CUserArAnchorParamsDlg::OnOK() 
{
	double tf;

	UpdateData(TRUE);

	tf = atof(m_length_str);
	if ((tf > 0.0) && (tf < 5000.0))
		m_obj->m_anchor_length = tf;
	
	tf = atof(m_width_str);
	if ((tf > 0.0) && (tf < 5000.0))
		m_obj->m_anchor_width = tf;
	
	CDialog::OnOK();
}

// *****************************************************************
// *****************************************************************

void CUserArAnchorParamsDlg::set_route_ptr(C_ar_obj *obj)
{
	m_obj = obj;
}

// *****************************************************************
// *****************************************************************


BOOL CUserArAnchorParamsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_obj != NULL)
	{
		m_length_str.Format("%6.1f", m_obj->m_anchor_length);
		m_width_str.Format("%6.1f", m_obj->m_anchor_width);
		UpdateData(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
LRESULT CUserArAnchorParamsDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

