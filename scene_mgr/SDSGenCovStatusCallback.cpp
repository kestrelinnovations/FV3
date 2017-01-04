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


// SDSGenCovStatusCallback.cpp : Implementation of CGenCovStatusCallback


// stdafx first
#include "stdAfx.h"

// this file's header
#include "SDSGenCovStatusCallback.h"

// system includes
#include <string>
#include <stdio.h>

// third party files

// other FalconView headers

// this project's headers


namespace scene_mgr
{


/////////////////////////////////////////////////////////////////////////////
// CSDSGenCovStatusCallback


// converts # of secs into string formatted as hh:mm:ss

std::string AscTime(time_t nsecs)
{
   int hours, mins, secs;

   hours = static_cast<int>(nsecs / 3600);
   nsecs -= hours * 3600;
   mins = static_cast<int>(nsecs / 60);
   nsecs -= mins * 60;
   secs = static_cast<int>(nsecs % 60);

   const int TMP_LEN = 50;
   CHAR tmp[TMP_LEN];
   sprintf_s( tmp, TMP_LEN, "%02d:%02d:%02d", hours, mins, secs);

   std::string result(tmp);
   return result;
}


CSDSGenCovStatusCallback::CSDSGenCovStatusCallback()
{
   m_boolUserCancelled = false;
   m_hwndParent = 0;

   m_pglobal_dlg = new CSDSGenCovDlg;
   m_pglobal_dlg->Create(IDD_SM_GEN_COV);
   time(&m_tmStartTime);
}

CSDSGenCovStatusCallback::~CSDSGenCovStatusCallback()
{
   if (m_pglobal_dlg)
   {
      m_pglobal_dlg->DestroyWindow();
      delete m_pglobal_dlg;
      m_pglobal_dlg = NULL;
   }
}


HRESULT CSDSGenCovStatusCallback::StatusBegin(const std::string& strStatusText)
{
   // Set dialog parent window
   if (m_hwndParent != 0)
      ::SetWindowLong(m_pglobal_dlg->m_hWnd, GWL_HWNDPARENT, (LONG)m_hwndParent);

   if (strStatusText.size() > 0)
   {
      std::string caption("Sds Coverage Generation: ");
      caption += strStatusText;
      m_pglobal_dlg->SetWindowText(caption.c_str());
   }
   return S_OK;
}


HRESULT CSDSGenCovStatusCallback::StatusEnd(DWORD dwErrorCode, const std::string& strExtraInfo, IErrorInfo *pErrorInfo)
{
   m_pglobal_dlg->UpdateData();

   return S_OK;
}


HRESULT CSDSGenCovStatusCallback::StatusUpdate(const std::string& strDest, const std::string& strSrc,
                                                     long lFileCount,long lPercentDone, long *pCancel)
{
   ATLASSERT(m_pglobal_dlg != NULL);

   m_pglobal_dlg->m_strDataSource = strDest.c_str();
   m_pglobal_dlg->m_strMapSeries = strSrc.c_str();
   m_pglobal_dlg->m_strNumFiles.Format("%d", lFileCount);

   time_t crnttm;
   time(&crnttm);
   crnttm -= m_tmStartTime;

   m_pglobal_dlg->m_strTotalTime = AscTime(crnttm).c_str();

   m_pglobal_dlg->UpdateData(FALSE);
   m_pglobal_dlg->UpdateWindow();

   m_boolUserCancelled = m_pglobal_dlg->m_boolCloseSelected;
   *pCancel = m_boolUserCancelled;

   return S_OK;
}

HRESULT CSDSGenCovStatusCallback::HasUserCancelled(long *pCancelled)
{
   *pCancelled = m_boolUserCancelled;
   return S_OK;
}

HRESULT CSDSGenCovStatusCallback::put_m_hwndParent(long newVal)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_hwndParent = (HWND)newVal;

   return S_OK;
}

};  // namespace scene_mgr
