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

// ImportInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\fvw.h"
#include "ElevationDlg.h"

IMPLEMENT_DYNAMIC(BackgroundStatic, CStatic)

BEGIN_MESSAGE_MAP(BackgroundStatic, CStatic)
   ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

BackgroundStatic::BackgroundStatic()
{
   m_brush = CreateSolidBrush(RGB(255,255,255));
}

HBRUSH BackgroundStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
   return m_brush;
}

/////////////////////////////////////////////////////////////////////////////
// CImportInfoDlg dialog


CElevationDlg::CElevationDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CElevationDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CElevationDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void CElevationDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CElevationDlg)
    DDX_Control(pDX, IDC_YNA_ICON, m_icon);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CElevationDlg, CDialog)
   //{{AFX_MSG_MAP(CElevationDlg)
   ON_BN_CLICKED(IDC_ELEV_YES, OnYes)
   ON_BN_CLICKED(IDC_ELEV_NO, OnNo)
   ON_BN_CLICKED(IDC_YESALL, OnYesAll)
   ON_BN_CLICKED(IDC_NOALL, OnNoAll)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CElevationDlg message handlers

BOOL CElevationDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_iconBitmap = ::LoadBitmap(AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDB_QMARK));
   m_icon.SetBitmap(m_iconBitmap);
   
    m_static.SubclassDlgItem(IDC_YNA_STATIC, this);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CElevationDlg::OnYes() 
{
   m_result = YES;
   CDialog::OnOK();
}

void CElevationDlg::OnNo() 
{
   m_result = NO;
   CDialog::OnOK();
}

void CElevationDlg::OnYesAll() 
{
   m_result = YES_ALL;
   CDialog::OnOK();
}

void CElevationDlg::OnNoAll() 
{
   m_result = NO_ALL;
   CDialog::OnOK();
}
