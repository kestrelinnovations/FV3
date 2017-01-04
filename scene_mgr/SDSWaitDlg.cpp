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

// SDSWaitDlg.cpp : implementation file
//


// stdafx first
#include "stdAfx.h"

// this file's header
#include "SDSWaitDlg.h"

// system includes

// third party files

// other FalconView headers

// this project's headers




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CSDSWaitDlg dialog


CSDSWaitDlg::CSDSWaitDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CSDSWaitDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CWaitDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   m_hWaitCursor = LoadCursor(NULL, IDC_WAIT);
}

CSDSWaitDlg::~CSDSWaitDlg()
{
}


void CSDSWaitDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSDSWaitDlg)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSDSWaitDlg, CDialog)
   //{{AFX_MSG_MAP(CSDSWaitDlg)
   ON_WM_SETCURSOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDSWaitDlg message handlers


BOOL CSDSWaitDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   SetCursor(m_hWaitCursor);

   return FALSE;
}

};  // namespace scene_mgr
