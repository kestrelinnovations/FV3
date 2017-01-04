// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// ToolbarEventSink.h
//

#ifndef FALCONVIEW_INCLUDE_TOOLBAREVENTSINK_H_
#define FALCONVIEW_INCLUDE_TOOLBAREVENTSINK_H_

#include "Common/ComErrorObject.h"

// ToolbarEventSink is a base class that implements an event sink for the
// COM toolbar's connection points.
//
// To use, implement a class that derives from this class and implement the
// event notifications that you are interested in. You'll want to at least
// implement raw_OnButtonPressed.
//
// Create an instance, p, of your derived class with the following
//
//    CComObject<OverlayEditorToolbarEventSink>::CreateInstance(&p);
//    p->AddRef();
//    p->RegisterNotifyEvents(true, m_spFvToolbar);
//
// Be sure to unregister and release the instance in your destructor
//
//    p->RegisterNotifyEvents(false, m_spFvToolbar);
//    p->Release();
//
// Because a call to RegisterNotifyEvents can throw a _com_error it needs
// to be wrapped in a try/catch block.
//
class ATL_NO_VTABLE ToolbarEventSink :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<IFvToolbarEvents, &IID_IFvToolbarEvents, &LIBID_FvToolbarServerLib>,
   public IDispatchImpl<IFvToolbarCustomizationEvents, &IID_IFvToolbarCustomizationEvents, &LIBID_FvToolbarServerLib>
{
   DWORD m_dwNotifyCookie, m_dwCustomizationNotifyCookie;

   // IFvToolbarEvents
public:
   STDMETHOD(raw_OnButtonPressed)(long lButtonId)
   {
      return S_OK;
   }
   STDMETHOD(raw_OnHelp)(long lButtonId) 
   { 
      return S_OK; 
   }
   STDMETHOD(raw_OnClosed)()
   {
      return S_OK;
   }
   STDMETHOD(raw_OnModified)()
   {
      return S_OK;
   }

   // IFvToolbarCustomizationEvents
public:
   STDMETHOD(raw_OnCustomizeToolbar)() 
   { 
      return S_OK; 
   }

public:
   ToolbarEventSink() : m_dwNotifyCookie(0), m_dwCustomizationNotifyCookie(0)
   {
   }

   void RegisterNotifyEvents(bool bRegister, IUnknown *pFvToolbar)
   {
      if (bRegister)
      {
         THROW_IF_ERROR(AtlAdvise(pFvToolbar, IUnknownPtr(this),
            IID_IFvToolbarEvents, &m_dwNotifyCookie))
         THROW_IF_ERROR(AtlAdvise(pFvToolbar, IUnknownPtr(this),
            IID_IFvToolbarCustomizationEvents, &m_dwCustomizationNotifyCookie))
      }

      else if (m_dwNotifyCookie != 0 && m_dwCustomizationNotifyCookie != 0)
      {
         THROW_IF_ERROR(AtlUnadvise(pFvToolbar, IID_IFvToolbarEvents,
            m_dwNotifyCookie))
         THROW_IF_ERROR(AtlUnadvise(pFvToolbar, IID_IFvToolbarCustomizationEvents,
            m_dwCustomizationNotifyCookie))
      }
   }

BEGIN_COM_MAP(ToolbarEventSink)
   COM_INTERFACE_ENTRY2(IDispatch, IFvToolbarEvents)
   COM_INTERFACE_ENTRY(IFvToolbarEvents)
   COM_INTERFACE_ENTRY(IFvToolbarCustomizationEvents)
END_COM_MAP()
};

#endif FALCONVIEW_INCLUDE_TOOLBAREVENTSINK_H_
