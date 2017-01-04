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

#include "StdAfx.h"
#include "StatusBarManager.h"
#include "FvStatusBarManagerImpl.h"
#include "StatusBarInfoPane.h"

// CFvStatusBarManager implementation

CFvStatusBarManager::CFvStatusBarManager() :
m_pStatusBarMgr(NULL)
{

}

FalconViewOverlayLib::IFvStatusBarManager *CFvStatusBarManager::GetStatusBarMgr()
{
   if (m_pStatusBarMgr == NULL)
   {
      CComObject<CFvStatusBarManagerImpl>::CreateInstance(&m_pStatusBarMgr);
      m_pStatusBarMgr->AddRef();

      m_pStatusBarMgr->SetStatusBarMgr(this);
   }

   return m_pStatusBarMgr;
}

CFvStatusBarManager::~CFvStatusBarManager()
{
   if (m_pStatusBarMgr != NULL)
      m_pStatusBarMgr->Release();
}

// sets the STATUS_BAR_PANE_HELP_TEXT info pane's help text.  This interface will eventually change to allow a client
// to retrieve an infopane by name and then use the generic SetText method on the infopane interface
HRESULT CFvStatusBarManager::SetStatusBarHelpText(BSTR helpText)
{
   // This is a temporary member variable that is used in overlay manager's test_select to determine
   // if an overlay set the help text in their MouseMove event handler.  It will eventually be removed.
   m_currentHelpText = CString((char *)_bstr_t(helpText));

   static CTextInfoPane *pHelpTextInfoPane = static_cast<CTextInfoPane *>(m_availableInfoPaneList->GetInfoPane(STATUS_BAR_PANE_HELP_TEXT));
   if (pHelpTextInfoPane != NULL)
      pHelpTextInfoPane->SetText(m_currentHelpText);

   return S_OK;
}