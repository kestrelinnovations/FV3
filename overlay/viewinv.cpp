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



// viewinv.cpp

// This file contains map view invalidations classes and functions.

#include "stdafx.h"
#include "ovl_mgr.h"
#include "wm_user.h"   // WM_INVALIDATE_FROM_THREAD
#include "..\mapview.h"
#include "viewinv.h"
#include "overlay.h"
#include "utils.h"
#include "..\getobjpr.h"

//*****************************************************************************
// Map View Invalidate Class Implementations

// Base Class Constructor
MapViewInvalidation::MapViewInvalidation(boolean_t erase_background) : 
   m_erase_background(erase_background) {}

// A map view invalidation object for invalidating the entire window.
MapViewInvalidateAll::MapViewInvalidateAll(boolean_t erase_background) :
   MapViewInvalidation(erase_background) {}

void MapViewInvalidateAll::invalidate(CWnd *wnd)
{
   if (wnd)
      UTL_invalidate_view(wnd, m_erase_background, TRUE, TRUE);
}


// A map view invalidation object for invalidating a rectangle in the window.
MapViewInvalidateRect::MapViewInvalidateRect(LPCRECT rect, 
   boolean_t erase_background) : MapViewInvalidation(erase_background), m_rect(rect) 
{
}

void MapViewInvalidateRect::invalidate(CWnd *wnd)
{
   if (wnd)
      UTL_invalidate_view_rect(wnd, &m_rect, m_erase_background, FALSE, TRUE);
}


// A map view invalidation object for invalidating a CRgn in the window.
MapViewInvalidateRgn::MapViewInvalidateRgn(CRgn *rgn, 
   boolean_t erase_background) : MapViewInvalidation(erase_background)
{
   m_rgn.CopyRgn(rgn);
}

void MapViewInvalidateRgn::invalidate(CWnd *wnd)
{
   if (wnd)
      UTL_invalidate_view_rgn(wnd, &m_rgn, m_erase_background, FALSE, TRUE);
}

MapViewInvalidateMap::MapViewInvalidateMap(boolean_t erase_background) :
   MapViewInvalidation(erase_background)
{
}

void MapViewInvalidateMap::invalidate(CWnd *wnd)
{
   MapView *pMapView = (MapView *)UTL_get_active_non_printing_view();
   if (pMapView != NULL)
   {
      pMapView->set_current_map_invalid();
      pMapView->invalidate_view();
   }
}


//*****************************************************************************
// C_ovl_mgr invalidate from thread implementations.

// These functions are used to perform a map view invalidation from a worker
// thread.  Such circumstances should be rare, but in the event that you need
// to invalidate all or part of the map view window from a worker thread, you
// must use one of these three functions.  They will invalidate the entire
// window, the given CRect, or the given CRgn respectively.

void C_ovl_mgr::invalidate_from_thread(boolean_t erase_background)
{
   on_invalidate_from_thread(new MapViewInvalidateAll(erase_background));   
}

void C_ovl_mgr::invalidate_from_thread(LPCRECT rect, boolean_t erase_background)
{
   on_invalidate_from_thread(new MapViewInvalidateRect(rect, erase_background));
}

void C_ovl_mgr::invalidate_from_thread(CRgn *rgn, boolean_t erase_background)
{
   on_invalidate_from_thread(new MapViewInvalidateRgn(rgn, erase_background));
}

void C_ovl_mgr::on_invalidate_from_thread(MapViewInvalidation *invalidation)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())

   CWinApp *app = AfxGetApp();
   CWinThread *thread = AfxGetThread();
   CWnd *view = fvw_get_view();

   ASSERT(app);
   ASSERT(thread);
   ASSERT(view);

   // If anything goes wrong, do nothing.
   if (app == NULL || thread == NULL || view == NULL || invalidation == NULL)
   {
      delete invalidation;
      return;
   }

   // if the current thread is the main thread, then process this invalidation
   // directly
   if ( app->m_nThreadID == GetCurrentThreadId() )
   {
      // invalidate the correct part of the map view window
      invalidation->invalidate(view);

      // delete the MapViewInvalidation object
      delete invalidation;
   }
   // otherwise forward the invalidation to the main thread view window
   else
      view->PostMessage(WM_INVALIDATE_FROM_THREAD, (WPARAM)invalidation);
}
