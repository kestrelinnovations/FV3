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

// FvStatusBarManagerImpl.cpp
//

#include "stdafx.h"
#include "FvStatusBarManagerImpl.h"
#include "statbar.h"
#include "StatusBarManager.h"

STDMETHODIMP CFvStatusBarManagerImpl::raw_SetStatusBarHelpText(BSTR helpText)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   return m_pStatusBarMgr->SetStatusBarHelpText(helpText);
}

STDMETHODIMP CFvStatusBarManagerImpl::raw_CreateProgressBar(BSTR progressBarText, short nRange)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_pStatusBarMgr->m_arrStatusBars[0]->CreateProgressBar((char *)_bstr_t(progressBarText), nRange, 1);

   return S_OK;
}

STDMETHODIMP CFvStatusBarManagerImpl::raw_SetProgressBarPosition(short nPosition)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_pStatusBarMgr->m_arrStatusBars[0]->SetProgressBarPos(nPosition);

   return S_OK;
}

STDMETHODIMP CFvStatusBarManagerImpl::raw_DestroyProgressBar()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_pStatusBarMgr->m_arrStatusBars[0]->DestroyProgressBar();

   return S_OK;
}