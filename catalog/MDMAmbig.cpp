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



// MDMAmbig.cpp : implementation file
//
#include "stdafx.h"
#include "..\resource.h"
#include "MDMAmbig.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CMDMAmbiguous dialog


CMDMAmbiguous::CMDMAmbiguous(CWnd* pParent /*=NULL*/)
	: CDialog(CMDMAmbiguous::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMDMAmbiguous)
	//}}AFX_DATA_INIT

   m_SelectRadioValue = -1;
   m_bSelSrc   = FALSE;
   m_bSelTgt   = FALSE;
   m_bUnSelSrc = FALSE;
   m_bUnSelTgt = FALSE;
}


void CMDMAmbiguous::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDMAmbiguous)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMAmbiguous, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CMDMAmbiguous)
	ON_BN_DOUBLECLICKED(IDC_MDMAMBIG_SELSRC, OnDoubleclickedMdmambigSelsrc)
	ON_BN_DOUBLECLICKED(IDC_MDMAMBIG_SELTGT, OnDoubleclickedMdmambigSeltgt)
	ON_BN_DOUBLECLICKED(IDC_MDMAMBIG_UNSELSRC, OnDoubleclickedMdmambigUnselsrc)
	ON_BN_DOUBLECLICKED(IDC_MDMAMBIG_UNSELTGT, OnDoubleclickedMdmambigUnseltgt)
	ON_BN_CLICKED(IDC_MDMAMBIG_SELSRC, OnMdmambigSelsrc)
	ON_BN_CLICKED(IDC_MDMAMBIG_SELTGT, OnMdmambigSeltgt)
	ON_BN_CLICKED(IDC_MDMAMBIG_UNSELSRC, OnMdmambigUnselsrc)
	ON_BN_CLICKED(IDC_MDMAMBIG_UNSELTGT, OnMdmambigUnseltgt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMAmbiguous message handlers

BOOL CMDMAmbiguous::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   GetDlgItem(IDC_MDMAMBIG_SELSRC)->EnableWindow(m_bSelSrc);
   GetDlgItem(IDC_MDMAMBIG_SELTGT)->EnableWindow(m_bSelTgt);
   GetDlgItem(IDC_MDMAMBIG_UNSELSRC)->EnableWindow(m_bUnSelSrc);
   GetDlgItem(IDC_MDMAMBIG_UNSELTGT)->EnableWindow(m_bUnSelTgt);
   CheckRadioButton(IDC_MDMAMBIG_SELSRC, IDC_MDMAMBIG_UNSELTGT, m_SelectRadioValue);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMDMAmbiguous::EnableRadioValue(int i, BOOL b)
{
   switch (i)
   {
   case IDC_MDMAMBIG_SELSRC:
      m_bSelSrc = b;
      break;
   case IDC_MDMAMBIG_SELTGT:
      m_bSelTgt = b;
      break;
   case IDC_MDMAMBIG_UNSELSRC:
      m_bUnSelSrc = b;
      break;
   case IDC_MDMAMBIG_UNSELTGT:
      m_bUnSelTgt = b;
      break;
   }
}

void CMDMAmbiguous::OnDoubleclickedMdmambigSelsrc() 
{
   m_SelectRadioValue = IDC_MDMAMBIG_SELSRC;
   EndDialog(IDOK);
}

void CMDMAmbiguous::OnDoubleclickedMdmambigSeltgt() 
{
   m_SelectRadioValue = IDC_MDMAMBIG_SELTGT;
   EndDialog(IDOK);
}

void CMDMAmbiguous::OnDoubleclickedMdmambigUnselsrc() 
{
   m_SelectRadioValue = IDC_MDMAMBIG_UNSELSRC;
   EndDialog(IDOK);
}

void CMDMAmbiguous::OnDoubleclickedMdmambigUnseltgt() 
{
   m_SelectRadioValue = IDC_MDMAMBIG_UNSELTGT;
   EndDialog(IDOK);
}


void CMDMAmbiguous::OnMdmambigSelsrc() 
{
   m_SelectRadioValue = IDC_MDMAMBIG_SELSRC;
   CheckRadioButton(IDC_MDMAMBIG_SELSRC, IDC_MDMAMBIG_UNSELTGT, m_SelectRadioValue);
}

void CMDMAmbiguous::OnMdmambigSeltgt() 
{
   m_SelectRadioValue = IDC_MDMAMBIG_SELTGT;
   CheckRadioButton(IDC_MDMAMBIG_SELSRC, IDC_MDMAMBIG_UNSELTGT, m_SelectRadioValue);
}

void CMDMAmbiguous::OnMdmambigUnselsrc() 
{
   m_SelectRadioValue = IDC_MDMAMBIG_UNSELSRC;
   CheckRadioButton(IDC_MDMAMBIG_SELSRC, IDC_MDMAMBIG_UNSELTGT, m_SelectRadioValue);
}

void CMDMAmbiguous::OnMdmambigUnseltgt() 
{
   m_SelectRadioValue = IDC_MDMAMBIG_UNSELTGT;
   CheckRadioButton(IDC_MDMAMBIG_SELSRC, IDC_MDMAMBIG_UNSELTGT, m_SelectRadioValue);
}


void CMDMAmbiguous::OnOK() 
{
	CDialog::OnOK();
}

LRESULT CMDMAmbiguous::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}