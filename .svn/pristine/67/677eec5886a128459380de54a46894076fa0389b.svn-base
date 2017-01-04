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

// OverlayPropertyPageCOM.cpp
//

#include "stdafx.h"
#include "OverlayPropertyPageCOM.h"
#include "..\mapview.h"
#include "utils.h"
#include "err.h"

COverlayPropertyPageCOM::COverlayPropertyPageCOM(const CString& displayName, GUID propertyPageGuid, CLSID propertyPageClsid) :
   m_displayName(displayName),
   m_propertyPageGuid(propertyPageGuid),
   m_propertyPageClsid(propertyPageClsid)
{
}

HRESULT COverlayPropertyPageCOM::OnCreate(long hWndParent, CComObject<CPropertyPageEventObserver> *pEventObserver)
{
   try
   {
      CO_CREATE(m_spFvOverlayPropertyPage, m_propertyPageClsid);

      MapView *pMapView = static_cast<MapView *>(UTL_get_active_non_printing_view());
      FalconViewOverlayLib::IFvMapView *pFvMapView = (pMapView == NULL) ? NULL : pMapView->GetFvMapView();
      
      IUnknownPtr spUnknown = pEventObserver;
      m_spFvOverlayPropertyPage->OnCreate(m_propertyPageGuid, pFvMapView, hWndParent, spUnknown);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to create the %s property page - %s", m_displayName, (char *)e.Description());
      ERR_report(msg);

      return E_FAIL;
   }

   return S_OK;
}

HRESULT COverlayPropertyPageCOM::OnApply()
{
   try
   {
      if (m_spFvOverlayPropertyPage != NULL)
         m_spFvOverlayPropertyPage->OnApply();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("IFvOverlayPropertyPage::OnApply failed for the the %s property page - %s", m_displayName, (char *)e.Description());
      ERR_report(msg);

      return E_FAIL;
   }

   return S_OK;
}