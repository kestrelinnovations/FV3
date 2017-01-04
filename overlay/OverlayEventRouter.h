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

// OverlayEventRouter.h
//

#ifndef OVERLAY_OVERLAY_EVENT_ROUTER_H
#define OVERLAY_OVERLAY_EVENT_ROUTER_H

class C_overlay;
class CVerticalViewProjector;
class OverlayEventRouterCallback_Interface;
class ViewMapProj;

// A utility to ease transition from CList to std::vector
template<typename T>
std::vector<T> CListToVector(const CList<T, T>& list)
{
   std::vector<T> ret;
   POSITION pos = list.GetHeadPosition();
   while (pos)
   {
      ret.push_back(list.GetNext(pos));
   }
   return ret;
}

// The OverlayEventRouter class is used to encapsulate the handling of
// overlay event routing, including user interface events, to a list of
// overlays.
//
// If the hide_background_overlays option passed to the contructor is true,
// then the event is only routed to the current overlay.
class OverlayEventRouter
{
public:
   OverlayEventRouter(std::vector<C_overlay*>& overlay_list,
      bool hide_background_overlays, C_overlay* current_overlay,
      OverlayEventRouterCallback_Interface* callback = nullptr);

   void InvalidateOverlays();
   void OnDrawOverlays(IActiveMapProj* active_map);
   void OnDrawTopMostOverlays(IActiveMapProj* active_map);
   void OnDrawOverlaysToBaseMap(IActiveMapProj* active_map);
   void AppendMenuItems(FalconViewOverlayLib::IFvContextMenu *context_menu,
      FalconViewOverlayLib::IFvMapView *map_view, const CPoint& point);
   bool CanSnapTo(ViewMapProj* map, const CPoint& point);
   void DrawToVerticalDisplay(CDC* dc,
      CVerticalViewProjector* vertical_view_proj);
   bool IsOverlayValid(C_overlay* overlay_to_check);
   bool CanAddPixmapsToBaseMap();

   // Generic event routing method that routes an event, or operation, from
   // the bottom of the overlay stack to the top. Handles the case where the
   // hide-background-overlays options is set as well as visible/non-visible
   // overlays.
   void OverlayEventRouter::RouteEventBottomToTop(
      OverlayEventRouterCallback_Interface* callback,
      std::function<void(C_overlay *)> operation);

   // Generic event routing method that routes an event, or operation, from
   // the top of the overlay stack to the bottom. If the given operation ever
   // returns false, then the routing is halted.
   //
   // Returns true if any operation returns true. False, otherwise.
   bool OverlayEventRouter::RouteEventTopToBottom(
      std::function<bool(C_overlay *)> operation);

private:
   std::vector<C_overlay *> m_overlay_list;
   bool m_hide_background_overlays;
   C_overlay* m_current_overlay;
   OverlayEventRouterCallback_Interface* m_callback;
};

// OverlayEventRouterCallback_Interface - used to control when routing is
// halted for certain events. For example, the OnDrawOverlays event dispatching
// routine gives a chance to abort the routing after each overlay is drawn.
class OverlayEventRouterCallback_Interface
{
public:
   // Return true to halt the routing of the OnDraw event. This method will
   // be called after each overlay in the stack has a chance to draw.
   virtual bool HaltOnDrawEventRouting() = 0;

   virtual ~OverlayEventRouterCallback_Interface() { }
};

#endif  // OVERLAY_OVERLAY_EVENT_ROUTER_H