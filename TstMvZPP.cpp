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



// TstMvZPP.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TstMvZPP.h"

/////////////////////////////////////////////////////////////////////////////
// CTestMoveZoomPP property page

IMPLEMENT_DYNCREATE(CTestMoveZoomPP, CPropertyPage)

CTestMoveZoomPP::CTestMoveZoomPP() : CPropertyPage(CTestMoveZoomPP::IDD)
{
   //{{AFX_DATA_INIT(CTestMoveZoomPP)
   m_testMove = FALSE;
   m_numIterations = 100;
   m_conusOnly = FALSE;
   m_testScale = FALSE;
   m_testZoom = FALSE;
   m_nRandomSeed = 0;
   m_bWriteToErrorLog = FALSE;
   //}}AFX_DATA_INIT
}

CTestMoveZoomPP::~CTestMoveZoomPP()
{
}

void CTestMoveZoomPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTestMoveZoomPP)
   DDX_Check(pDX, IDC_TEST_MOVE, m_testMove);
   DDX_Text(pDX, IDC_NUM_ITERATIONS, m_numIterations);
   DDV_MinMaxUInt(pDX, m_numIterations, 1, 10000);
   DDX_Check(pDX, IDC_CONUS_ONLY, m_conusOnly);
   DDX_Check(pDX, IDC_TEST_MAP_SCALE, m_testScale);
   DDX_Check(pDX, IDC_TEST_MAP_ZOOM, m_testZoom);
   DDX_Text(pDX, IDC_RANDOM_SEED, m_nRandomSeed);
   DDX_Check(pDX, IDC_WRITE_TO_ERROR_LOG, m_bWriteToErrorLog);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestMoveZoomPP, CPropertyPage)
   //{{AFX_MSG_MAP(CTestMoveZoomPP)
   ON_BN_CLICKED(IDC_TEST_MOVE, OnClickTestMove)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestMoveZoomPP message handlers

void CTestMoveZoomPP::OnClickTestMove() 
{
   // enable or disable the subitems depending on the parent
   UpdateData(TRUE);
   if (m_testMove)
   {
      GetDlgItem(IDC_CONUS_ONLY)->EnableWindow(TRUE);
   }
   else
   {
      GetDlgItem(IDC_CONUS_ONLY)->EnableWindow(FALSE);
   }
}

BOOL CTestMoveZoomPP::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   // Disable the sub-items until we click their parent
   GetDlgItem(IDC_CONUS_ONLY)->EnableWindow(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
