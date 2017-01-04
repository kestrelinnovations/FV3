// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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

// OverlayEventRouter.cpp
//
#include "stdafx.h"
#include "overlay/OverlayEventRouter.h"

#include "FalconView/getobjpr.h"
#include "include/overlay.h"
#include "include/mapx.h"
#include "include/ovl_mgr.h"
#include "overlay/OverlayBackingStore.h"
#include "overlay/OverlayCOM.h"

OverlayEventRouter::OverlayEventRouter(std::vector<C_overlay*>& overlay_list,
      bool hide_background_overlays, C_overlay* current_overlay,
      OverlayEventRouterCallback_Interface* callback /*= nullptr */) :
m_overlay_list(overlay_list),
m_hide_background_overlays(hide_background_overlays),
m_current_overlay(current_overlay),
m_callback(callback)
{
}

// Generic event routing method that routes an event, or operation, from
// the bottom of the overlay stack to the top. Handles the case where the
// hide-background-overlays options is set as well as visible/non-visible
// overlays.
void OverlayEventRouter::RouteEventBottomToTop(
   OverlayEventRouterCallback_Interface* callback,
   std::function<void(C_overlay *)> operation)
{
   // Process only the current overlay if the hide-background-overlays
   // parameter is set and the overlay is visible.
   if (m_hide_background_overlays)
   {
      if (m_current_overlay && m_current_overlay->get_m_bIsOverlayVisible())
      {
         operation(m_current_overlay);
      }
      return;
   }

   // Otherwise, process all visible overlays from bottom up
   for (auto it = m_overlay_list.rbegin(); it != m_overlay_list.rend(); ++it)
   {
      C_overlay* overlay = *it;

      // skip this overlay if it is hidden
      if (!overlay->get_m_bIsOverlayVisible())
         continue;

      operation(overlay);

      // check to see if caller wants to halt the routing of the event,
      // but not after processing the top-most overlay in the list
      if (it != --m_overlay_list.rend() &&
         callback && callback->HaltOnDrawEventRouting())
      {
         return;
      }
   }
}

// Generic event routing method that routes an event, or operation, from
// the top of the overlay stack to the bottom. If the given operation ever
// returns false, then the routing is halted.
//
// Returns true if any operation returns true. False, otherwise.
bool OverlayEventRouter::RouteEventTopToBottom(
   std::function<bool(C_overlay *)> operation)
{
   // Process only the current overlay if the hide-background-overlays
   // parameter is set and the overlay is visible.
   if (m_hide_background_overlays)
   {
      if (m_current_overlay && m_current_overlay->get_m_bIsOverlayVisible())
         return operation(m_current_overlay);
   }
   // Otherwise, process all visible overlays from bottom up
   else for (auto it = m_overlay_list.begin(); it != m_overlay_list.end(); ++it)
   {
      C_overlay* overlay = *it;

      // skip this overlay if it is hidden
      if (!overlay->get_m_bIsOverlayVisible())
         continue;

      // If the operation returns true then we are done routing events.
      if (operation(overlay))
         return true;
   }

   return false;
}

void OverlayEventRouter::InvalidateOverlays()
{
   // Even if the user specifies a callback it will not be used in the
   // routing of the invalidation event. I.e., all overlays will be invalidated.
   RouteEventBottomToTop(nullptr, [](C_overlay* overlay)
   {
      overlay->invalidate();
   });
}

void OverlayEventRouter::OnDrawOverlays(IActiveMapProj* active_map)
{
   RouteEventBottomToTop(m_callback, [active_map](C_overlay* overlay)
   {
      if (!overlay->IsTopMostOverlay())
         overlay->OnDraw(active_map);
   });
}

void OverlayEventRouter::OnDrawTopMostOverlays(IActiveMapProj* active_map)
{
   RouteEventBottomToTop(m_callback, [active_map](C_overlay* overlay)
   {
      if (overlay->IsTopMostOverlay())
      {
         ActiveMap_TMP activeMapWrapper(active_map);
      
         int opacity = overlay->Opacity(); //OVL_get_overlay_manager()->TopMostOverlayOpacity(overlay);
         overlay->GetOverlayBackingStore()->OnDraw(&activeMapWrapper, opacity);
      }
   });
}

void OverlayEventRouter::OnDrawOverlaysToBaseMap(IActiveMapProj* active_map)
{
   RouteEventBottomToTop(m_callback, [active_map](C_overlay* overlay)
   {
      overlay->OnDrawToBaseMap(active_map);
   });
}

void OverlayEventRouter::AppendMenuItems(
   FalconViewOverlayLib::IFvContextMenu *context_menu,
   FalconViewOverlayLib::IFvMapView *map_view_ptr, const CPoint& point)
{
   MapView* map_view = fvw_get_view();
   RouteEventTopToBottom([=](C_overlay* overlay) -> bool
   {
      // If the user is in General Perspective Projection, only send the event
      // to the mission overlay.
      if (map_view && map_view->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
      {
         if (overlay->get_m_overlayDescGuid() != FVWID_Overlay_Route)
         {
            return false;
         }
      }

      overlay->AppendMenuItems(context_menu, map_view_ptr, point.x, point.y);
      return false;  // never prematurely abort the routing of events
   });
}

bool OverlayEventRouter::CanSnapTo(ViewMapProj* map, const CPoint& point)
{
   return RouteEventTopToBottom([=](C_overlay* overlay)
   {
      return overlay->test_snap_to(map, point) == TRUE;
   });
}

void OverlayEventRouter::DrawToVerticalDisplay(CDC* dc,
      CVerticalViewProjector* vertical_view_proj)
{
   RouteEventBottomToTop(nullptr, [=](C_overlay* overlay)
   {
      overlay->DrawToVerticalDisplay(dc, vertical_view_proj);
   });
}

bool OverlayEventRouter::IsOverlayValid(C_overlay* overlay_to_check)
{
   return RouteEventTopToBottom([=](C_overlay* overlay) -> bool
   {
      if (overlay == overlay_to_check)
      {
         return dynamic_cast<C_overlay *>(overlay_to_check) != nullptr;
      }
      return false;
   });
}

bool OverlayEventRouter::CanAddPixmapsToBaseMap()
{
   return RouteEventTopToBottom([=](C_overlay* overlay)
   {
      return overlay->can_add_pixmaps_to_base_map() ? true : false;
   });
}