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

// FvMapViewImpl.cpp
//

#include "stdafx.h"
#include "FvMapViewImpl.h"

#include "GeospatialViewController.h"
#include "GeospatialView.h"
#include "getobjpr.h"
#include "mapview.h"
#include "mapx.h"
#include "SnapTo.h"
#include "mainfrm.h"

// get the HWND associated with the view
STDMETHODIMP CFvMapViewImpl::get_hWnd(long *pHwnd)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *pHwnd = reinterpret_cast<long>(m_pMapView->m_hWnd());
   return S_OK;
}

// get the frame window associated with the view
STDMETHODIMP CFvMapViewImpl::get_FrameWnd(FalconViewOverlayLib::IFvFrameWnd** pFrameWnd)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CMainFrame* frame = m_pMapView->GetFrame();
   if (frame)
   {
      *pFrameWnd = frame->GetFvFrameWnd();
      (*pFrameWnd)->AddRef();
   }
   else
      *pFrameWnd = nullptr;

   return S_OK;
}

// get the current map projection (ISettableMapProj) associated with the view
STDMETHODIMP CFvMapViewImpl::get_CurrentMapProj(IUnknown **pMapProj)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   MapView* view = fvw_get_view();
   if ((view == nullptr) || view->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      *pMapProj = nullptr;
      return S_OK;
   }

   *pMapProj = m_pMapView->get_curr_map()->GetSettableMapProj();
   (*pMapProj)->AddRef();

   return S_OK;
}

// get the overlay manager
STDMETHODIMP CFvMapViewImpl::get_OverlayManager(FalconViewOverlayLib::IFvOverlayManager** ppOverlayManager)
{
   *ppOverlayManager = OVL_get_overlay_manager()->GetFvOverlayManager();
   (*ppOverlayManager)->AddRef();

   return S_OK;
}

// returns TRUE if the view is being smooth-scrolled
STDMETHODIMP CFvMapViewImpl::get_IsScrolling(long *bIsScrolling)
{
   *bIsScrolling = MapView::m_scrolling;
   return S_OK;
}

// set the cursor for the map view
STDMETHODIMP CFvMapViewImpl::raw_SetCursor(long hCursor)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   return m_pMapView->SetCursor(reinterpret_cast<HCURSOR>(hCursor));
}

// set the tooltip for the map view.  The tooltip will be displayed at the current mouse
// position in 1/8 of second.  If the given text matches the text of a tooltip that is already 
// being displayed and the cursor is within a certain threshold then the tooltip will not be cleared.
// Also, if the mouse position is outside the view then no tooltip will be displayed.  
STDMETHODIMP CFvMapViewImpl::raw_SetTooltipText(BSTR tooltipText)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   return m_pMapView->SetTooltipText(tooltipText);
}

// Returns TRUE if any overlay has a snappable point in view at the given screen coordinates
STDMETHODIMP CFvMapViewImpl::raw_CanSnapTo(long x, long y, long *pbCanSnapTo)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   ViewMapProj *pViewMapProj = OVL_get_overlay_manager()->get_view_map();
   if (pViewMapProj != NULL)
      *pbCanSnapTo = OVL_get_overlay_manager()->test_snap_to(pViewMapProj, CPoint(x, y));
   else
      *pbCanSnapTo = FALSE;
      
   return S_OK;
}

// Get a snappable point in the view at the given screen coordinates.  The framework is responsbile
// for picking a single point in the case that there are multiple snappable points at the given location.
// note: pSnapToPointsList is an object that implements ISnapToPointsList
STDMETHODIMP CFvMapViewImpl::raw_GetSnappablePoint(long x, long y, IUnknown **pSnapToPointsList)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *pSnapToPointsList = NULL;

   ViewMapProj *pViewMapProj = OVL_get_overlay_manager()->get_view_map();
   if (pViewMapProj != NULL)
   {
      SnapToInfo info;

      // convert from info -> ISnapToPointsList object
      if (OVL_get_overlay_manager()->do_snap_to(pViewMapProj, CPoint(x, y), &info) == TRUE)
      {
         *pSnapToPointsList = info.ConvertToSnapToPointsList();
      }
   }

   return S_OK;
}

STDMETHODIMP CFvMapViewImpl::raw_ScrollMapIfPointNearEdge(long x, long y)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_pMapView->ScrollMapIfPointNearEdge(x, y);

   return S_OK;
}

// IFvMapViewCamera implementation
//
STDMETHODIMP CFvMapViewImpl::raw_GetCameraPosition(long* values_valid,
   double* lat, double* lon,
   double* alt_meters, double* heading, double* incline, double* roll)
{
   *values_valid = FALSE;
   if (m_pMapView != nullptr)
   {
      GeospatialViewController* controller =
         m_pMapView->GetGeospatialViewController();
      if (controller != nullptr)
      {
         GeospatialView* view = controller->GetGeospatialView();
         if (view != nullptr)
         {
            GeospatialViewer* viewer = view->Viewer();
            if (viewer != nullptr)
            {
               viewer->GetCameraPosition(lat, lon, alt_meters);
               *values_valid = TRUE;
            }
         }
      }
   }

   return S_OK;
}


STDMETHODIMP CFvMapViewImpl::raw_AttachToCamera(
   FalconViewOverlayLib::ICameraEvents* camera_events)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (camera_events == nullptr)
      return E_POINTER;

   m_pMapView->AttachToCamera(camera_events);

   return S_OK;
}

/// <summary>Indicates to the map view that your overlay will no longer
/// be pushing updates to the camera position.</summary>
STDMETHODIMP CFvMapViewImpl::raw_DetachFromCamera()
{
   m_pMapView->AttachToCamera(nullptr);
   return S_OK;
}

STDMETHODIMP CFvMapViewImpl::raw_SetCameraPositionWhenAttached(double lat,
   double lon, double alt_meters, double heading, double incline, double roll)
{
   MapView::AttachedCameraParams camera_params = { lat, lon, alt_meters,
      heading, incline, roll };

   m_pMapView->SetCameraPositionWhenAttached(camera_params);
   return S_OK;
}