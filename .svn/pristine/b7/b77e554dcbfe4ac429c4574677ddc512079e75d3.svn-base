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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// FvOverlayManagerImpl.cpp
//

#include "stdafx.h"
#include "MultiLocationDlgImpl.h"
#include "..\tabulareditor\MultiLocationEditDlg.h"
#include "geo_tool_d.h"


CMultiLocationDlgImpl::CMultiLocationDlgImpl() : m_dlg(nullptr)
{
}

CMultiLocationDlgImpl::~CMultiLocationDlgImpl()
{
   DestroyDialog();
}

void CMultiLocationDlgImpl::DestroyDialog()
{
   if (m_dlg != nullptr)
      delete m_dlg;

   m_dlg = nullptr;
}

STDMETHODIMP CMultiLocationDlgImpl::raw_DoModal(long parent_hwnd, SAFEARRAY* geo_locations, int* res)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   FalconViewTabularEditorLib::GeoLocation* points = 
      reinterpret_cast<FalconViewTabularEditorLib::GeoLocation *>(geo_locations->pvData);

   long num_points = geo_locations->rgsabound->cElements;

   CWnd *wnd = CWnd::FromHandle((HWND)parent_hwnd);
   m_dlg = new CMultiLocationEditDlg(wnd);

   for (int i=0; i<num_points; ++i)
   {
      d_geo_t p = { points[i].lat, points[i].lon };
      m_dlg->add_location(p);
   }

   *res = (int)m_dlg->DoModal();
   
   return S_OK;
}

STDMETHODIMP CMultiLocationDlgImpl::raw_GetLocations(SAFEARRAY** geo_locations)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   SafeArray<FalconViewTabularEditorLib::GeoLocation, VT_RECORD> sa_locations(
      FalconViewTabularEditorLib::LIBID_FalconViewTabularEditorLib, 
      __uuidof(FalconViewTabularEditorLib::GeoLocation));

   for (int i=0; i<m_dlg->GetCount(); ++i)
   {
      d_geo_t p = m_dlg->GetAt(i);

      FalconViewTabularEditorLib::GeoLocation geo = { p.lat, p.lon };

      sa_locations.Append(geo);
   }

   DestroyDialog();

   SafeArrayCopy(&sa_locations, geo_locations);

   return S_OK;
}