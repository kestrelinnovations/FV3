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

// SDSGenCovProgress.cpp : Implementation of CSDSGenCovProgress


// stdafx first
#include "stdAfx.h"

// this file's header
#include "SDSGenCovProgress.h"

// system includes
#include <string>
#include <vector>

// third party files

// other FalconView headers

// this project's headers
#include "SDSGenCovThread.h"
#include "SDSGenCovDlg.h"




namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CGenCovProgress


// This method starts a coverage generation thread

HRESULT CSDSGenCovProgress::GenCov(const std::vector<long>& vectDataSources, void *pStatusCallback)
{
   // Create progress dialog
   CWnd *pParentWnd = CWnd::FromHandle(m_hwndParent);
   m_pGenCovDlg = new CSDSGenCovDlg(pParentWnd);
   m_pGenCovDlg->Create(CSDSGenCovDlg::IDD);

   // Create the coverage generation thread
   m_pthreadGenCov = (SDSGenCovThread *) AfxBeginThread(RUNTIME_CLASS(SDSGenCovThread),
      THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED );
   m_pthreadGenCov->m_vectDataSources = vectDataSources;
   m_pthreadGenCov->m_hwndParent = m_hwndParent;
   m_pthreadGenCov->m_hwndProgressDlg = m_pGenCovDlg->GetSafeHwnd();

   // Start the thread
   m_pthreadGenCov->ResumeThread();

   return S_OK;
}

HWND CSDSGenCovProgress::GetParent()
{
   return m_hwndParent;
}

void CSDSGenCovProgress::SetParent(HWND newVal)
{
   m_hwndParent = newVal;
}

};  // namespace scene_mgr
