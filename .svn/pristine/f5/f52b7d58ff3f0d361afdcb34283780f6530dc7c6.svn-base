// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// InternalToolbarImpl.cpp
//

#include "stdafx.h"
#include "InternalToolbarImpl.h"

#include "FalconView/getobjpr.h"
#include "FalconView/include/err.h"

InternalToolbarImpl::~InternalToolbarImpl()
{
   if (m_toolbar_events != NULL)
   {
      try
      {
         m_toolbar_events->RegisterNotifyEvents(false, m_toolbar);
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("Failed unregistering from toolbar events connection point: %s", (char *)e.Description());
         ERR_report(msg);
      }
      m_toolbar_events->Release();
   }
}

void InternalToolbarImpl::ShowToolbar(BOOL show)
{
   if (show)
      InitializeToolbar();

   CMainFrame* frame = fvw_get_frame();
   if (frame)
   {
      CBasePane* pane = frame->GetPane(m_toolbar_resource_id);
      if (pane)
         pane->ShowPane(show, FALSE, FALSE);      
   }
}

void InternalToolbarImpl::InitializeToolbar()
{
   CMainFrame* frame = fvw_get_frame();
   if (m_toolbar == nullptr)
   {
      CO_CREATE(m_toolbar, CLSID_FvToolbar); 

      m_toolbar->Initialize(
         reinterpret_cast<long>(frame->GetSafeHwnd()), m_toolbar_resource_id, 
         frame->GetFvFrameWnd());

      m_toolbar->LoadToolBarFromResource(m_toolbar_resource_id);
       
      // sign up for the toolbar events connection point
      CComObject<InternalToolbarEventSink>::CreateInstance(
         &m_toolbar_events);
      m_toolbar_events->AddRef();
      m_toolbar_events->RegisterNotifyEvents(true, m_toolbar);

      // this must be done on the UI thread, not the COM thread
      frame->RecalcLayout();
   }
}
