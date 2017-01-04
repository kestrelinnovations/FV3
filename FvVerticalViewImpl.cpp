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

// FvVerticalViewImpl.cpp
//

#include "stdafx.h"
#include "FvVerticalViewImpl.h"
#include "VerticalViewDisplay.h"
#include "VerticalViewProperties.h"
#include "getobjpr.h"
#include "TipTimer.h"

// get the HWND associated with the view
STDMETHODIMP CFvVerticalViewImpl::get_hWnd(long *pHwnd)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *pHwnd = reinterpret_cast<long>(m_pVerticalView->GetSafeHwnd());
   return S_OK;
}

// get the frame window associated with the vertical view
STDMETHODIMP CFvVerticalViewImpl::get_FrameWnd(FalconViewOverlayLib::IFvFrameWnd** pFrameWnd)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *pFrameWnd = NULL;

   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      *pFrameWnd = pFrame->GetFvFrameWnd();
      (*pFrameWnd)->AddRef();
   }

   return S_OK;
}

// get the current vertical view projection
STDMETHODIMP CFvVerticalViewImpl::get_CurrentVerticalViewProj(FalconViewOverlayLib::IFvVerticalViewProjector **pVerticalViewProj)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

	ViewMapProj *pMapProj = UTL_get_current_view_map();
   CMainFrame *pFrame = fvw_get_frame();

   *pVerticalViewProj = NULL;

   if (pMapProj != NULL && pFrame != NULL)
   {
      CVerticalViewProperties *pProperties = pFrame->GetVerticalDisplayPropertiesObject();

      if (pProperties != NULL)
      {
         CRect clientRect;
	      m_pVerticalView->GetClientRect(clientRect);

         CComObject<CFvVerticalViewProjectorImpl> *pProj;
         CComObject<CFvVerticalViewProjectorImpl>::CreateInstance(&pProj);
         pProj->Initialize(pMapProj, pProperties->m_dMinAltitudeMeters, pProperties->m_dMaxAltitudeMeters, clientRect.Height());
         pProj->AddRef();
         *pVerticalViewProj = pProj;
      }
   }

   return S_OK;
}

// get the overlay manager
STDMETHODIMP CFvVerticalViewImpl::get_OverlayManager(FalconViewOverlayLib::IFvOverlayManager** ppOverlayManager)
{
   *ppOverlayManager = OVL_get_overlay_manager()->GetFvOverlayManager();
   (*ppOverlayManager)->AddRef();

   return S_OK;
}

// set the tooltip for the map view.  The tooltip will be displayed at the current mouse
// position in 1/8 of second.  If the given text matches the text of a tooltip that is already 
// being displayed and the cursor is within a certain threshold then the tooltip will not be cleared.
// Also, if the mouse position is outside the view then no tooltip will be displayed.  
STDMETHODIMP CFvVerticalViewImpl::raw_SetTooltipText(BSTR tooltipText)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_pVerticalView->SetTooltipText(tooltipText);

   return S_OK;
}