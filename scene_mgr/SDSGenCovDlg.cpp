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

// GenCovDlg.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SDSGenCovDlg.h"

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
// CSDSGenCovDlg dialog


CSDSGenCovDlg::CSDSGenCovDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CSDSGenCovDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CGenCovDlg)
   m_strDataSource = _T("");
   m_strMapSeries = _T("");
   m_strNumFiles = _T("");
   m_strTotalTime = _T("");
   //}}AFX_DATA_INIT
   m_boolCloseSelected = false;
}

CSDSGenCovDlg::~CSDSGenCovDlg()
{
   m_dlgWait.DestroyWindow();
}


void CSDSGenCovDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSDSGenCovDlg)
   DDX_Text(pDX, IDC_SM_DATA_SOURCE, m_strDataSource);
   DDX_Text(pDX, IDC_SM_MAP_SERIES, m_strMapSeries);
   DDX_Text(pDX, IDC_SM_NUM_FILES, m_strNumFiles);
   DDX_Text(pDX, IDC_SM_TOTAL_TIME, m_strTotalTime);
   DDX_Control(pDX, IDC_SM_ANIMATION, m_animateCtrl);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSDSGenCovDlg, CDialog)
   //{{AFX_MSG_MAP(CSDSGenCovDlg)
   ON_WM_CLOSE()
   ON_REGISTERED_MESSAGE(SM_COVGEN_START, OnCovGenStart)
   ON_REGISTERED_MESSAGE(SM_COVGEN_STATUS, OnCovGenStatus)
   ON_REGISTERED_MESSAGE(SM_COVGEN_STOP, OnCovGenStop)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDSGenCovDlg message handlers

BOOL CSDSGenCovDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   m_animateCtrl.Open(IDR_SM_AVI_FILEFIND);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CSDSGenCovDlg::OnCovGenStart(WPARAM wPrm, LPARAM lPrm)
{
   covgen_start_t* pStruct = reinterpret_cast<covgen_start_t*>(wPrm);
   if(pStruct != nullptr)
   {
      std::string caption("Scene Coverage Generation: ");
      caption += pStruct->str;
      SetWindowText(caption.c_str());

      delete pStruct;
   }

   return LRESULT(true);
}

LRESULT CSDSGenCovDlg::OnCovGenStatus(WPARAM wPrm, LPARAM lPrm)
{
   covgen_status_t* pStruct = reinterpret_cast<covgen_status_t*>(wPrm);
   if(pStruct != nullptr)
   {
      m_strDataSource = pStruct->strDest.c_str();
      m_strMapSeries = pStruct->strSrc.c_str();
      m_strNumFiles.Format("%d", pStruct->lFileCount);

      //time_t crnttm;
      //time(&crnttm);
      //crnttm -= m_tmStartTime;

      //m_strTotalTime = AscTime(crnttm).c_str();
  
      UpdateData(FALSE);
      UpdateWindow();

      delete pStruct;
   }

   return LRESULT(true);
}

LRESULT CSDSGenCovDlg::OnCovGenStop(WPARAM wPrm, LPARAM lPrm)
{
   UpdateData();

   // Signal the SDM dialog to update the browse tree
   CWnd* pParent = m_pParentWnd;
   if(pParent != nullptr)
   {
      HWND parent_hwnd = pParent->GetSafeHwnd();
      ::PostMessage(parent_hwnd, SM_COVGEN_STOP, (WPARAM) 0, (LPARAM) 0);
   }

   DestroyWindow();

   delete this;

   return LRESULT(true);
}

void CSDSGenCovDlg::OnClose()
{
   m_boolCloseSelected = true;
   if (m_dlgWait.m_hWnd == NULL)
      m_dlgWait.Create(IDD_SM_WAIT);
}

};  // namespace scene_mgr
