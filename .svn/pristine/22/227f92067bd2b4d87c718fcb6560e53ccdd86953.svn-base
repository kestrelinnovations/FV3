// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



// SMAmbiguousDlg.cpp : implementation file
//


// stdafx first
#include "stdafx.h"

// this file's header
#include "SMAmbiguousDlg.h"

// system includes

// third party files

// other FalconView headers
#include "..\resource.h"
#include "..\getobjpr.h"

// this project's headers



/////////////////////////////////////////////////////////////////////////////
// CSMAmbiguousDlg dialog


CSMAmbiguousDlg::CSMAmbiguousDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CSMAmbiguousDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSMAmbiguousDlg)
   //}}AFX_DATA_INIT

   m_SelectRadioValue = -1;
   m_bSelSrc   = FALSE;
   m_bSelTgt   = FALSE;
   m_bUnSelSrc = FALSE;
   m_bUnSelTgt = FALSE;
}


void CSMAmbiguousDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSMAmbiguousDlg)
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSMAmbiguousDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CSMAmbiguousDlg)
   ON_BN_DOUBLECLICKED(IDC_SMAMBIG_SELSRC, OnDoubleclickedSmambigSelsrc)
   ON_BN_DOUBLECLICKED(IDC_SMAMBIG_SELTGT, OnDoubleclickedSmambigSeltgt)
   ON_BN_DOUBLECLICKED(IDC_SMAMBIG_UNSELSRC, OnDoubleclickedSmambigUnselsrc)
   ON_BN_DOUBLECLICKED(IDC_SMAMBIG_UNSELTGT, OnDoubleclickedSmambigUnseltgt)
   ON_BN_CLICKED(IDC_SMAMBIG_SELSRC, OnSmambigSelsrc)
   ON_BN_CLICKED(IDC_SMAMBIG_SELTGT, OnSmambigSeltgt)
   ON_BN_CLICKED(IDC_SMAMBIG_UNSELSRC, OnSmambigUnselsrc)
   ON_BN_CLICKED(IDC_SMAMBIG_UNSELTGT, OnSmambigUnseltgt)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMAmbiguousDlg message handlers

BOOL CSMAmbiguousDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   GetDlgItem(IDC_SMAMBIG_SELSRC)->EnableWindow(m_bSelSrc);
   GetDlgItem(IDC_SMAMBIG_SELTGT)->EnableWindow(m_bSelTgt);
   GetDlgItem(IDC_SMAMBIG_UNSELSRC)->EnableWindow(m_bUnSelSrc);
   GetDlgItem(IDC_SMAMBIG_UNSELTGT)->EnableWindow(m_bUnSelTgt);
   CheckRadioButton(IDC_SMAMBIG_SELSRC, IDC_SMAMBIG_UNSELTGT, m_SelectRadioValue);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CSMAmbiguousDlg::EnableRadioValue(int i, BOOL b)
{
   switch (i)
   {
   case IDC_SMAMBIG_SELSRC:
      m_bSelSrc = b;
      break;
   case IDC_SMAMBIG_SELTGT:
      m_bSelTgt = b;
      break;
   case IDC_SMAMBIG_UNSELSRC:
      m_bUnSelSrc = b;
      break;
   case IDC_SMAMBIG_UNSELTGT:
      m_bUnSelTgt = b;
      break;
   }
}

void CSMAmbiguousDlg::OnDoubleclickedSmambigSelsrc() 
{
   m_SelectRadioValue = IDC_SMAMBIG_SELSRC;
   EndDialog(IDOK);
}

void CSMAmbiguousDlg::OnDoubleclickedSmambigSeltgt() 
{
   m_SelectRadioValue = IDC_SMAMBIG_SELTGT;
   EndDialog(IDOK);
}

void CSMAmbiguousDlg::OnDoubleclickedSmambigUnselsrc() 
{
   m_SelectRadioValue = IDC_SMAMBIG_UNSELSRC;
   EndDialog(IDOK);
}

void CSMAmbiguousDlg::OnDoubleclickedSmambigUnseltgt() 
{
   m_SelectRadioValue = IDC_SMAMBIG_UNSELTGT;
   EndDialog(IDOK);
}

void CSMAmbiguousDlg::OnSmambigSelsrc() 
{
   m_SelectRadioValue = IDC_SMAMBIG_SELSRC;
   CheckRadioButton(IDC_SMAMBIG_SELSRC, IDC_SMAMBIG_UNSELTGT, m_SelectRadioValue);
}

void CSMAmbiguousDlg::OnSmambigSeltgt() 
{
   m_SelectRadioValue = IDC_SMAMBIG_SELTGT;
   CheckRadioButton(IDC_SMAMBIG_SELSRC, IDC_SMAMBIG_UNSELTGT, m_SelectRadioValue);
}

void CSMAmbiguousDlg::OnSmambigUnselsrc() 
{
   m_SelectRadioValue = IDC_SMAMBIG_UNSELSRC;
   CheckRadioButton(IDC_SMAMBIG_SELSRC, IDC_SMAMBIG_UNSELTGT, m_SelectRadioValue);
}

void CSMAmbiguousDlg::OnSmambigUnseltgt() 
{
   m_SelectRadioValue = IDC_SMAMBIG_UNSELTGT;
   CheckRadioButton(IDC_SMAMBIG_SELSRC, IDC_SMAMBIG_UNSELTGT, m_SelectRadioValue);
}


void CSMAmbiguousDlg::OnOK() 
{
   CDialog::OnOK();
}

LRESULT CSMAmbiguousDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}