// Copyright (c) 1994-2012,2014 Georgia Tech Research Corporation, Atlanta, GA
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



// mouse.cpp : implementation of the MapView class mouse/cursor stuff
//

#include "stdafx.h"
#include "mapview.h"
#include "tiptimer.h"
#include "dted_tmr.h"
#include "mapx.h"
#include "MovingMapOverlay\csttrack.h"  // for CoastTrackManager
#include "MovingMapOverlay\rb.h"        // for RangeBearingManager
#include "ar_edit.h"
#include "ar_edit\factory.h"
#include "nitf\factory.h"
#include "nitf.h"
#include "StatusBarInfoPane.h"
#include "StatBarBullseyePropPage.h"
#include "SystemHealthDialog.h"
#include "OvlFctry.h"
#include "VerticalViewDisplay.h"
#include "VerticalViewProperties.h"
#include "VerticalViewPropertiesDialog.h"
#include "showrmk.h"
#include "..\Common\SafeArray.h"
#include "..\Common\ComErrorObject.h"
#include "ContextMenuItems.h"
#include "StatusBarManager.h"
#include "MapEngineCOM.h"
#include "maps.h"
#include "MouseClickResource.h"
#include "ZoomPercentages.h"
#include "GeospatialViewController.h"
#include "GeoInputDevice.h"

#define VD_SCROLL_WHEEL_INCREMENT 200  // the altitude increment applied for each notch of mouse scroll wheel movement

// last cursor position - used to determine if the cursor should be located
// at the map center after a Scale In, Scale Out, or Center
static CPoint last_cursor_xy;

// forward declarations (defined in prnttool\PrntPage)
bool MAP_get_next_scale_in_position(const CList<MapType *, MapType *> &map_list,
   const POSITION &position, POSITION &in);
bool MAP_get_next_scale_out_position(const CList<MapType *, MapType *> &map_list,
   const POSITION &position, POSITION &out);




// MapChangeTool
//

// constructor
MapChangeTool::MapChangeTool(MapView *view)
{
   m_map_view = view; 
   m_zoom_tool_point = CPoint(-1, -1);
   m_zoom_window_drag = FALSE;
}

boolean_t MapChangeTool::is_kind_of(CString s)
{
   return s == "MapChangeTool";
}

// return the center of the region dragged in geo coordinates
d_geo_t MapChangeTool::get_center_geo(ViewMapProjImpl *map, CPoint point)
{
   d_geo_t center_geo;
   int center_x, center_y;

   if (m_zoom_window_rectLast.Width () > 2 &&
      m_zoom_window_rectLast.Height() > 2)
   {
      center_x = (m_zoom_window_rectLast.left + 
         m_zoom_window_rectLast.right) / 2;
      center_y = (m_zoom_window_rectLast.top + 
         m_zoom_window_rectLast.bottom) / 2;
   }
   else
   {
      center_x = point.x;
      center_y = point.y;
   }

   map->surface_to_geo(center_x, center_y, 
      &center_geo.lat, &center_geo.lon);

   return center_geo;
}

// return the upper right coordinate of the region dragged 
// in geo coordinates
d_geo_t MapChangeTool::get_upper_right_geo(ViewMapProjImpl *map)
{
   d_geo_t upper_right_geo;

   map->surface_to_geo(m_zoom_window_rectLast.right, 
      m_zoom_window_rectLast.top, &upper_right_geo.lat, 
      &upper_right_geo.lon);

   return upper_right_geo;
}

void MapChangeTool::on_left_button_down(ViewMapProjImpl *map, CPoint point, UINT nFlags)
{
   if ((nFlags & MK_CONTROL) != MK_CONTROL)
   {
      // if we are entering drag, set up the pivot and the initial rect
      if (m_zoom_window_drag == FALSE)
      {
         m_zoom_window_ptReference = point;
         m_zoom_window_rectLast.SetRect(point.x, point.y, point.x, point.y);
      }

      // we are now performing a zoom window drag
      m_zoom_window_drag = TRUE;
   }
}

void MapChangeTool::on_mouse_move(ViewMapProjImpl *map, CPoint point, UINT nFlags,
   HCURSOR& cursor)
{
   // Get a DC
   CClientDC dc(map->get_CView());
   CRect rect;
   m_map_view->GetClientRect(&rect);

   if((nFlags & MK_CONTROL) == MK_CONTROL)
   {
      cursor = AfxGetApp()->LoadCursor(IDC_MAGNIFY_MINUS);
      m_zoom_window_drag = FALSE;
   }
   else
      cursor = AfxGetApp()->LoadCursor(IDC_MAGNIFY_PLUS);

   // make sure the tooltip is cleared if necessary
   if (!rect.PtInRect(point))
   {
      CTipDisplay tip;
      tip.clear();
   }

   // if perfoming a zoom window then handle it here
   if (m_zoom_window_drag)
   {
      dc.DrawFocusRect(m_zoom_window_rectLast);

      m_zoom_window_rectLast.SetRect(m_zoom_window_ptReference.x, 
         m_zoom_window_ptReference.y, point.x, point.y);

      m_zoom_window_rectLast.NormalizeRect();
      dc.DrawFocusRect(m_zoom_window_rectLast);

      // if the cursor is outside of the view, then use the no drop cursor

      if (!rect.PtInRect(point))
      {
         cursor = AfxGetApp()->LoadCursor(IDC_NODROP);
      }
   }
}

void MapChangeTool::cancel_drag(ViewMapProjImpl *map)
{
   if (m_zoom_window_drag)
   {
      CClientDC dc(map->get_CView());

      dc.DrawFocusRect(m_zoom_window_rectLast);

      m_zoom_window_drag = FALSE;
   }
}

// MapZoomTool
//

// constructor
MapZoomTool::MapZoomTool(MapView *view) : MapChangeTool(view)
{

}

boolean_t MapZoomTool::is_kind_of(CString s)
{
   if (s == "MapZoomTool")
      return TRUE;

   return MapChangeTool::is_kind_of(s);
}

CString MapZoomTool::get_help_text()
{
   return "Press Esc or the Zoom Tool button to exit this tool";
}

void MapZoomTool::on_left_button_up(ViewMapProjImpl *map, CPoint point, UINT nFlags)
{
   d_geo_t center_geo;

   // get the center of the window dragged
   center_geo = get_center_geo(map, point);

   // Get a DC
   CClientDC dc(map->get_CView());

   // if the cursor in the view
   CRect rect;
   m_map_view->GetClientRect(&rect);
   if (rect.PtInRect(point))
   {
      if ((nFlags & MK_CONTROL) == MK_CONTROL)
         map->surface_to_geo(point.x, point.y, &center_geo.lat, &center_geo.lon);
      else if (m_zoom_window_drag)
      {
         // remove rubber band box
         dc.DrawFocusRect(m_zoom_window_rectLast);
      }

      int new_zoom_percent = map->actual_zoom_percent();
      if (calc_zoom_percent(map, point, nFlags, new_zoom_percent) == SUCCESS)
      {
         // change the zoom percent and center
         m_map_view->ChangeZoomPercent(new_zoom_percent, center_geo);
         m_map_view->invalidate_view(FALSE);
      }
   }
}

void MapZoomTool::on_mouse_move(ViewMapProjImpl *map, CPoint point, UINT nFlags, 
   HCURSOR& cursor)
{
   MapChangeTool::on_mouse_move(map, point, nFlags, cursor);

   CRect rect;
   m_map_view->GetClientRect(&rect);

   // draw the help text next to the current point
   if (m_zoom_tool_point != point && rect.PtInRect(point))
   {
      int new_zoom_percent = map->actual_zoom_percent();

      if (calc_zoom_percent(map, point, nFlags, new_zoom_percent) != SUCCESS)
      {
         //m_map_view->GetFrame()->m_MainFrameToolBar->m_zoom_combo.zoom_in(&new_zoom_percent, FALSE);
      }

      CString zoom_text;
      zoom_text.Format("%d%%", new_zoom_percent);
      CTipDisplay tip;
      tip.clear();

      CWnd *pFGWnd = m_map_view->GetForegroundWindow();  //cab GetForgroundWindow can return null if FalconView gets it's focus stolen
      if (pFGWnd && (pFGWnd->m_hWnd == AfxGetMainWnd()->m_hWnd))
         tip.display(m_map_view, point.x + 22, point.y - 7, zoom_text);

      m_zoom_tool_point = point;
   }
}

void MapZoomTool::on_key_down(ViewMapProjImpl *map, CPoint point, UINT nFlags)
{
   // remove rubber band box if necessary
   cancel_drag(map);

   // update tool tip if this isn't a repeated press
   if ((nFlags & (1 << 14))  ==  0)
   {
      int new_zoom_percent = map->actual_zoom_percent();
      //m_map_view->GetFrame()->m_MainFrameToolBar->m_zoom_combo.zoom_out(&new_zoom_percent, FALSE);
      CString zoom_text;
      zoom_text.Format("%d%%", new_zoom_percent);
      CTipDisplay tip;
      tip.clear();
      tip.display(m_map_view, point.x + 22, point.y - 7, zoom_text);
   }
}

void MapZoomTool::on_key_up(ViewMapProjImpl *map, CPoint point, UINT nFlags)
{
   int new_zoom_percent = map->actual_zoom_percent();
   //m_map_view->GetFrame()->m_MainFrameToolBar->m_zoom_combo.zoom_out(&new_zoom_percent, FALSE);
   CString zoom_text;
   zoom_text.Format("%d%%", new_zoom_percent);
   CTipDisplay tip;
   tip.clear();
   tip.display(m_map_view, point.x + 22, point.y - 7, zoom_text);
}

// calculates a new zoom percent for the zoom tool
int MapZoomTool::calc_zoom_percent(ViewMapProjImpl *map, CPoint point, UINT nFlags, int& new_zoom_percent)
{
   if ((nFlags & MK_CONTROL) == MK_CONTROL)
   {
      //m_map_view->GetFrame()->m_MainFrameToolBar->m_zoom_combo.zoom_out(&new_zoom_percent, FALSE);
   }
   else if (m_zoom_window_drag)
   {
      // calculate the new zoom percent
      if (m_zoom_window_rectLast.Width () > 2 &&
         m_zoom_window_rectLast.Height() > 2)
      {
         // get the width/height of the screen
         int width, height;
         map->get_surface_size(&width, &height);

         double delta_w = (double)width / (double)m_zoom_window_rectLast.Width();
         double delta_h = (double)height / (double)m_zoom_window_rectLast.Height();
         double delta = __min(delta_w, delta_h);

         new_zoom_percent = (int)(new_zoom_percent*delta);
      }
      else
      {
         //m_map_view->GetFrame()->m_MainFrameToolBar->m_zoom_combo.zoom_in(&new_zoom_percent, FALSE);
      }
   }
   // otherwise, we must have canceled the drag operation
   else
      return FAILURE;

   // limit zoom percent to [10, 800]
   if (new_zoom_percent > 800)
      new_zoom_percent = 800;
   if (new_zoom_percent < 10)
      new_zoom_percent = 10;

   return SUCCESS;
}

// MapScaleTool
//

// constructor
MapScaleTool::MapScaleTool(MapView *view) : MapChangeTool(view)
{
   m_map_group_identity = view->get_map_engine()->GetCurrentGroupIdentity();
}

boolean_t MapScaleTool::is_kind_of(CString s)
{
   if (s == "MapScaleTool")
      return TRUE;

   return MapChangeTool::is_kind_of(s);
}

CString MapScaleTool::get_help_text()
{
   return "Press Esc or the Scale Tool button to exit this tool";
}

void MapScaleTool::on_left_button_up(ViewMapProjImpl *map, CPoint point, UINT nFlags)
{
   d_geo_t center_geo, upper_right_geo;
   int width, height;

   // get the center of the window dragged
   center_geo = get_center_geo(map, point);

   // Get the upper right coordinate
   upper_right_geo = get_upper_right_geo(map);

   // get the width/height of the screen
   map->get_surface_size(&width, &height);

   // Get a DC
   CClientDC dc(map->get_CView());

   // if the cursor is not in the view, there is nothing to do
   CRect rect;
   m_map_view->GetClientRect(&rect);
   if (!rect.PtInRect(point))
   {
      return;
   }

   if (m_zoom_window_drag)
   {
      // remove rubber band box
      dc.DrawFocusRect(m_zoom_window_rectLast);
   }

   // if we are scaling out, then do just that
   if ((nFlags & MK_CONTROL) == MK_CONTROL)
   {
      int status = m_map_view->get_map_engine()->scale_out(center_geo.lat, center_geo.lon, TRUE);
      if (status != SUCCESS)
      {
         CTipDisplay tip;
         tip.clear();
         m_map_view->OnMapError(status);
      }
      m_map_view->invalidate_view(FALSE);
   }
   // otherwise, pick the closest scale available
   else
   {
      // first try to change to the target scale.  If that is not 
      // available we we change to the next scale OUT
      MapSource mapSource;
      MapScale mapScale;
      MapSeries mapSeries;
      if (calc_next_scale(map, nFlags, mapSource, mapScale, mapSeries) == SUCCESS)
      {
         int status;

         if (m_zoom_window_rectLast.Width () <= 2 ||
            m_zoom_window_rectLast.Height() <= 2)
         {
            status = m_map_view->get_map_engine()->scale_in(center_geo.lat, 
               center_geo.lon, TRUE);
         }
         else
         {
            status = m_map_view->get_map_engine()->change_map_type(
               m_map_group_identity, mapSource, mapScale, mapSeries, 
               center_geo.lat, center_geo.lon, map->actual_rotation(), 
               map->actual_zoom_percent(), map->requested_projection(), TRUE);
         }

         if (status == SUCCESS)
            m_map_view->invalidate_view(FALSE);
         else
         {
            CTipDisplay tip;
            tip.clear();
            m_map_view->OnMapError(status);
         }
      }
   }
}

void MapScaleTool::on_mouse_move(ViewMapProjImpl *map, CPoint point, UINT nFlags,
   HCURSOR& cursor)
{
   MapChangeTool::on_mouse_move(map, point, nFlags, cursor);

   CRect rect;
   m_map_view->GetClientRect(&rect);

   // draw the help text next to the current point
   if (m_zoom_tool_point != point && rect.PtInRect(point))
   {
      show_scale_tool_tooltip(map, point, nFlags, TRUE);
      m_zoom_tool_point = point;
   }
}

void MapScaleTool::on_key_down(ViewMapProjImpl *map, CPoint point, UINT nFlags)
{
   // remove rubber band box if necessary
   cancel_drag(map);

   // update tool tip if this isn't a repeated press
   if ((nFlags & (1 << 14))  ==  0)
      show_scale_tool_tooltip(map, point, nFlags, TRUE);
}

void MapScaleTool::on_key_up(ViewMapProjImpl *map, CPoint point, UINT nFlags)
{
   show_scale_tool_tooltip(map, point, nFlags, TRUE);
}

int MapScaleTool::calc_next_scale(ViewMapProjImpl *map, UINT nFlags,
   MapSource &mapSource, MapScale &mapScale,
   MapSeries &mapSeries,
   boolean_t force_next /*=FALSE*/)
{
   // initialize a map engine with the current map
   //
   MapEngineCOM map_engine;
   map_engine.init(FALSE, FALSE);

   d_geo_t center;
   map->get_actual_center(&center);

   map_engine.set_view_dimensions(map->get_surface_width(), map->get_surface_height());
   map_engine.change_map_type(m_map_group_identity, map->source(), map->scale(), map->series(), 
      center.lat, center.lon, map->actual_rotation(), map->requested_zoom_percent(),
      map->projection_type());
   map_engine.ApplyMap();

   // if we are scaling out one scale
   if ((nFlags & MK_CONTROL) == MK_CONTROL)
   {
      if (map_engine.scale_out(FALSE) == SUCCESS)
      {
         mapSource = map_engine.get_curr_map()->source();
         mapScale = map_engine.get_curr_map()->scale();
         mapSeries = map_engine.get_curr_map()->series();
         return SUCCESS;
      }

      return FAILURE;
   }
   else if (!m_zoom_window_drag && force_next)
   {
      if (map_engine.scale_in(FALSE) == SUCCESS)
      {
         mapSource = map_engine.get_curr_map()->source();
         mapScale = map_engine.get_curr_map()->scale();
         mapSeries = map_engine.get_curr_map()->series();
         return SUCCESS;
      }

      return FAILURE;
   }
   else if (!m_zoom_window_drag)
      return FAILURE;

   // calculate the new scale.  If the bounding box of the
   // dragged rect is smaller than 2 pixels wide and 2 pixels
   // high, then go ahead and try to scale in one scale.  Otherwise,
   // we will calculate the new scale based upon the area of the
   // dragged bounding rect
   if (m_zoom_window_rectLast.Width () < 2 &&
      m_zoom_window_rectLast.Height() < 2)
   {
      if (map_engine.scale_in(FALSE) == SUCCESS)
      {
         mapSource = map_engine.get_curr_map()->source();
         mapScale = map_engine.get_curr_map()->scale();
         mapSeries = map_engine.get_curr_map()->series();
         return SUCCESS;
      }

      return FAILURE;
   }

   // get the effective scale before resize
   int effective_scale;
   {
      double d;

      // get the current scale's denominator
      MAP_scale_t_to_scale_denominator(map->scale(), &d);

      // get the current effective scale
      if (map->scale() == WORLD)
      {
         // world scale depends on the width of the view (since we stretch
         // the world to fit the view)
         double deg_per_pix_lon = 360.0 / map->get_surface_width();
         degrees_t nominal_deg_per_pix = map->scale().get_nominal_degrees_lat_per_pixel();
         d *= (deg_per_pix_lon / nominal_deg_per_pix);
      }

      effective_scale = static_cast<int>(d * 100.0/map->actual_zoom_percent() + 0.5);
   }

   // initialize source, scale, series to the current map
   mapSource = map->source();
   mapScale = map->scale();
   mapSeries = map->series();

   const int width = map->get_surface_width();
   const int height = map->get_surface_height();
   CPoint ptCenter = m_zoom_window_rectLast.CenterPoint();

   while (map_engine.scale_in(FALSE) == SUCCESS)
   {
      double d;
      MAP_scale_t_to_scale_denominator(map_engine.get_curr_map()->scale(), &d);
      double f_in = static_cast<int>(d * (100.0/map_engine.get_curr_map()->actual_zoom_percent()) + 0.5);

      // At this point f_in is the effective scale of the next scale
      // in.  Setting f_in = -(1.0 - f_in / effective_scale) / 2.0
      // should cause in_rect.InflateRect to yield a rectangle that
      // closely matches the bounds if this rect had the next scale
      // in
      f_in = -(1.0 - f_in / effective_scale) / 2.0;

      CRect in_rect(ptCenter.x - width/2, ptCenter.y - height/2, 
         ptCenter.x + width/2, ptCenter.y + height/2);

      in_rect.InflateRect((int)(f_in*width),(int)(f_in*height));

      // if the rectangle of the next map scale totally encloses
      // the rectangle the user dragged out, then remember it.  The first
      // time that this does not hold, this means that scaling in will
      // not display the entire area so we will break out of the loop.
      if (in_rect.PtInRect(m_zoom_window_rectLast.TopLeft()) &&
         in_rect.PtInRect(m_zoom_window_rectLast.BottomRight()) )
      {
         mapSource = map_engine.get_curr_map()->source();
         mapScale = map_engine.get_curr_map()->scale();
         mapSeries = map_engine.get_curr_map()->series();
      }
      else
         break;
   }

   return SUCCESS;
}

void MapScaleTool::show_scale_tool_tooltip(ViewMapProjImpl *map,
   CPoint point, UINT nFlags, 
   boolean_t force_next /*= FALSE*/)
{
   MapSource mapSource;
   MapScale mapScale;
   MapSeries mapSeries;
   if (calc_next_scale(map, nFlags, mapSource, mapScale, mapSeries, force_next) == SUCCESS)
   {
      CTipDisplay tip;
      tip.clear();

      CWnd *pFGWnd = m_map_view->GetForegroundWindow();  //cab GetForgroundWindow can return null if FalconView gets it's focus stolen
      if (pFGWnd && (pFGWnd->m_hWnd == AfxGetMainWnd()->m_hWnd))
         tip.display(m_map_view, point.x + 22, point.y - 7, mapScale.get_string());
   }
}

/////////////////////////////////////////////////////////////////////////////
// MapView message handlers for mouse and cursor

BOOL MapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   // if the cursor is in the client area of the view, 
   // then set the cursor to m_cursor
   if (pWnd == this && nHitTest == HTCLIENT)
   {
      SetCursor(m_cursor);
      return TRUE;
   }

   return CView::OnSetCursor(pWnd, nHitTest, message);
}

static SmoothPanTimer *g_SmoothPan_timer = NULL;

void MapView::OnMouseMove(UINT nFlags, CPoint point)
{
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      GetGeospatialViewController()->OnMouseMoved(nFlags, point);
      
      // Basic cursor support for 3D projection
      if (m_hand_mode || m_force_hand_scroll)
      {
         if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
            ::SetCursor(AfxGetApp()->LoadCursor(IDC_CLOSED_HAND));
         else
            ::SetCursor(AfxGetApp()->LoadCursor(IDC_HAND0));
      }
      else
         ::SetCursor(OVL_get_overlay_manager()->get_default_cursor());

      return;
   }
   HintText *hint = NULL;
   static CMainFrame *frame = (CMainFrame *) GetParentFrame();
   const int STR_LEN = 70;
   char str[STR_LEN];
   double dpp_lat, dpp_lon, dpp;

   // for some reason, if the taskbar is set to Autohide (there may be other
   // conditions or a more general condition) then OnMouseMove is constantly
   // called even if the mouse does not move.  Check to make sure the mouse
   // has moved before processing this event. 
   if (!m_new_map && ((nFlags & MK_LBUTTON) != MK_LBUTTON) && last_cursor_xy == point)
      return;

   // don't process mouse moves before first map is drawn
   if (m_first_map_up == FALSE)
      return;

   // save mouse flags
   m_mouse_flags = nFlags;

   // make sure the tooltip for the scale/zoom mode is cleared if necessary
   CRect client_rect;
   GetClientRect(&client_rect);

   if (!client_rect.PtInRect(point) && m_map_change_tool != NULL)
   {
      // make sure the zoom/scale mode toolbar gets hidden
      CTipDisplay tip;
      tip.clear();
   }

   // Reset the view's cursor to NULL.  If the cursor has not been set by the end of this method, it will be set by get_default_cursor
   m_cursor = NULL;

   // If the tooltip is not set in one of the overlay's test_selected methods it will be cleared
   m_bTooltipWasSet = FALSE;

   // If the help text is not set, then it will be cleared
   frame->m_statusBarMgr->m_currentHelpText = "";

   MSG msg;
   BOOL b = ::PeekMessageA( &msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE );
   if ( m_scrolling && m_hand_scroll )
   {
      if ( 0 != ( nFlags & MK_LBUTTON )
         && m_old_point != point
         && ( !b || 0 == ( msg.wParam & MK_LBUTTON ) ) ) // No trailing mouse move with L-button still down
      {
         // Allow scrolling on the surface of the map only
         degrees_t lat, lon;
         if ( get_curr_map()->surface_to_geo(point.x, point.y, &lat, &lon) != SUCCESS )
            return;

         // ** NOTE : this can be done once when scrolling is started
         int screen_width, screen_height;

         // get the current width and height of the screen
         if (get_curr_map()->get_surface_size(&screen_width, &screen_height) != SUCCESS)
            return;
         // ** END

         int x, y;

         if ( get_curr_map()->geo_to_surface(m_geo_scroll_origin.lat, m_geo_scroll_origin.lon,
            &x, &y) != SUCCESS )
            return;

         int scroll_amount_x = x - point.x;
         int scroll_amount_y = y - point.y;

         int new_x = (screen_width / 2) + scroll_amount_x;
         int new_y = (screen_height / 2) + scroll_amount_y;

#if 0    // TRAC 4316 
         if (get_curr_map()->point_in_surface(new_x, new_y))
#endif
         {
            d_geo_t new_center;
            get_curr_map()->surface_to_geo(new_x, new_y, &new_center.lat,
               &new_center.lon);

            m_map_engine->change_center(new_center.lat, new_center.lon);
            m_map_params.SetCenter(new_center);
            invalidate_view(FALSE);
         }

         m_old_point = point;
      }

      SetViewCursor(AfxGetApp()->LoadCursor(IDC_CLOSED_HAND));
   }

   // places the cursor at the same geographic location it was at the time
   // the new map call (recenter, scale in, scale out, etc) was initiated 
   if (m_new_map && m_move_cursor && m_cursor_position_geo_valid)
   {
      // if the user has not moved the cursor since new map was
      // initiated, assume they want the cursor to maintain the
      // same geo location it had over the previous map
      CPoint delta(point - last_cursor_xy);
      if (delta.x < 10 && delta.x > -10 &&
         delta.y < 10 && delta.y > -10)
      {
         int x, y;

         get_curr_map()->geo_to_surface(m_cursor_position_geo.lat,
            m_cursor_position_geo.lon, &x, &y);

         point.x = x;
         point.y = y;

         //
         // move the cursor to its last geo location
         //
         {
            CPoint tmp(x, y);
            ClientToScreen(&tmp);

            //
            // note: the cursor didn't usually change positions just by
            // doing a SetCursorPos.  Doing the ::ShowCursor before and
            // after seems to fix that problem.
            //
            ::ShowCursor(FALSE);
            ::SetCursorPos(tmp.x, tmp.y);
            ::ShowCursor(TRUE);
         }
      }
   }
   last_cursor_xy = point;

   // Get current cursor position in geo coordinates. Note that the general
   // perspective projection, driven by MapEngineOSG, will update the cursor's
   // geographic location (for now)
   int status = SUCCESS;
   bool is_cursor_in_view = IsCursorInView();
   if (is_cursor_in_view &&
      GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
   {
      d_geo_t cursor_pos;
      status = get_curr_map()->surface_to_geo(point.x, point.y,
         &cursor_pos.lat, &cursor_pos.lon);

      // Sets m_cursor_position_geo and m_cursor_position_geo_valid
      SetCursorPositionGeo(cursor_pos, status == SUCCESS);
   }

   get_curr_map()->get_vmap_degrees_per_pixel(&dpp_lat, &dpp_lon);
   dpp = dpp_lat < dpp_lon ? dpp_lat : dpp_lon;

   BOOL goodmap = TRUE;
   BOOL precisemap = TRUE;

   // get current cursor position in geo coordinates and
   // set coordinate_format from format string
   if (status != SUCCESS) 
   {
      goodmap = FALSE;
   }
   else
   {
      // the picture tool in the drawing toolkit can display map images that are not georeferenced
      // check for cursor over one of these invalid map areas
      
      FalconViewOverlayLib::IFvOverlayManagerPtr pOverlayManager = OVL_get_overlay_manager()->GetFvOverlayManager();

      long nResult = pOverlayManager->SelectByOverlayDescGuid(FVWID_Overlay_Drawing);
   
      while (nResult)
      {
         IDrawingOverlayPtr drawingOverlay = pOverlayManager->CurrentOverlay;
         if ( drawingOverlay != NULL )
         {
            if ( drawingOverlay->OverFalseMap(point.x, point.y) )
            {
               goodmap = FALSE;
               break;
            }
         }

         nResult = pOverlayManager->MoveNext();
      }
            
      // the NITF can display map images that are not precisely georeferenced
      // check for cursor over one of these overlaid images
      C_nitf_ovl *nitfovl;

      nitfovl = (C_nitf_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_NitfFiles);
      if (nitfovl != NULL)
      {
         if (nitfovl->image_hit_test(point.x, point.y))
         {
            precisemap = FALSE;
         }
      }
   }

   frame->m_precise_geo = precisemap;

   static CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_PANE_CURSOR_LOCATION));
   static CTextInfoPane *pSecondaryInfoPane = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_PANE_SECONDARY_CURSOR_LOCATION));
   static CTextInfoPane *pElevationInfoPane = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION));
   static CTextInfoPane *pElevationInfoPaneSecondary = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION_SECONDARY));
   static CTextInfoPane *pElevationInfoPaneVerbose = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION_VERBOSE));
   static CTextInfoPane *pElevationInfoPaneSecondaryVerbose = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION_SECONDARY_VERBOSE));
   static CTextInfoPane *pBullseyeCoordinatesInfoPane = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_BULLSEYE_COORDINATES));

   // reset the elevation pane
   if (pElevationInfoPane != NULL)
      pElevationInfoPane->SetText("");
   if (pElevationInfoPaneSecondary != NULL)
      pElevationInfoPaneSecondary->SetText("");
   if (pElevationInfoPaneVerbose != NULL)
      pElevationInfoPaneVerbose->SetText("");
   if (pElevationInfoPaneSecondaryVerbose != NULL)
      pElevationInfoPaneSecondaryVerbose->SetText("");

   if (goodmap)
   {
      if (m_cursor_position_geo_valid)
      {
         GEO_lat_lon_to_string(m_cursor_position_geo.lat,
            m_cursor_position_geo.lon, dpp, str, STR_LEN);

         CString strLatLon(str);

         if (pTextInfoPane != NULL)
            pTextInfoPane->SetText(strLatLon);

         if (pSecondaryInfoPane != NULL)
         {
            char display[PRM_MAX_VALUE_LENGTH+1];
            const int STR_LEN = 70;
            char str[STR_LEN];
            GEO_get_default_display(display, PRM_MAX_VALUE_LENGTH+1);

            // swap temporarily to other format
            if (strcmp(display, "PRIMARY") == 0)
               GEO_set_default_display( "SECONDARY" );
            else
               GEO_set_default_display( "PRIMARY" );

            GEO_lat_lon_to_string(m_cursor_position_geo.lat,
               m_cursor_position_geo.lon, dpp, str, STR_LEN);
            pSecondaryInfoPane->SetText(str);

            // swap back to original format
            if (strcmp(display, "PRIMARY") == 0)
               GEO_set_default_display( "PRIMARY" );
            else
               GEO_set_default_display( "SECONDARY" );
         }
      }

      if (pBullseyeCoordinatesInfoPane != NULL && pBullseyeCoordinatesInfoPane->GetParentStatusBar() != NULL)
      {
         double dDistance, dBearing;
         GEO_calc_range_and_bearing(CStatBarBullseyePropPage::m_dLat,
            CStatBarBullseyePropPage::m_dLon, m_cursor_position_geo.lat,
            m_cursor_position_geo.lon, &dDistance, &dBearing, TRUE);

         if (CStatBarBullseyePropPage::m_bCourseMagNotTrue)
         {
            // compute the local magnetic variation
            int year, month, alt, rslt;
            double magvar;
            SYSTEMTIME time;
            GetSystemTime(&time);
            year = time.wYear;
            month = time.wMonth;
            alt = 0;
            rslt = GEO_magnetic_variation(CStatBarBullseyePropPage::m_dLat, CStatBarBullseyePropPage::m_dLon, 
               year, month, alt, &magvar);
            if (rslt != SUCCESS)
               magvar = 0.0;

            dBearing -= magvar;
            if (dBearing < 0.0)
               dBearing += 360.0;
            else if (dBearing > 360.0)
               dBearing -= 360.0;
         }

         CString heading_txt;
         heading_txt.Format("%.0f", dBearing);

         // make sure the bearing is of the form xxx
         while(heading_txt.GetLength() < 3)
            heading_txt = "0" + heading_txt;

         // append the degrees symbol
         heading_txt += "°";

         // tack on the mag/true units to the end of the heading
         heading_txt += CStatBarBullseyePropPage::m_bCourseMagNotTrue ? "M" : "T";

         double dRange = (CStatBarBullseyePropPage::m_bUnitsNMnotKM) ?
            METERS_TO_NM(dDistance) : dDistance / 1000.0;

         CString range_txt;
         range_txt.Format("%0.1f", dRange);

         // tack on the NM/km units to the end of the range
         range_txt += (CStatBarBullseyePropPage::m_bUnitsNMnotKM) ? "NM" : "km";

         CString strRangeBearing;
         strRangeBearing.Format("%s/%s", heading_txt, range_txt);

         pBullseyeCoordinatesInfoPane->SetText(strRangeBearing);
      }
   }
   else
   {
      if (pTextInfoPane != NULL)
         pTextInfoPane->SetText("");
      if (pSecondaryInfoPane != NULL)
         pSecondaryInfoPane->SetText("");
   }

   // ***** end update running lat/lon

   // don't change cursor when popup menu is up
   if (m_popup)
      return;

   // recapture mouse if needed
   if (m_left_button_down && this != GetCapture())
      SetCapture();

   // also need to recapture the mouse if we are in scale/zoom percent mode 
   // and the cursor is outside the application window
   if (m_map_change_tool != NULL) 
   {
      CWnd *pFGWnd = GetForegroundWindow();  //cab GetForgroundWindow can return null if FalconView gets it's focus stolen
      if (pFGWnd && (pFGWnd->m_hWnd == AfxGetMainWnd()->m_hWnd))
         SetCapture();
   }

   if (m_get_position_from_map)
   {
      // Check to see if the cursor is over an icon that can be snapped to with
      // the current context.  test_snap_to() returns TRUE in this case.
      if (OVL_get_overlay_manager()->test_snap_to(get_curr_map(), point))
      {
         SetViewCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

         m_hint->set_help_text("Snap to point");
         m_hint->set_tool_tip("Snap To");
         hint = m_hint;

         // we have to do this because the current arch assumes that the
         // hint was set by a call to selected, test_selected, etc...
         TipTimer::set_text(m_hint->get_tool_tip());
      }
      else
      {
         SetViewCursor(AfxGetApp()->LoadCursor(IDC_GET_MOUSE_CLICK_CURSOR));
      }
   }
   else if (m_map_change_tool != NULL && !m_hand_scroll && !m_force_hand_scroll)
   {
      HCURSOR cursor = NULL;
      m_map_change_tool->on_mouse_move(get_curr_map(), point, nFlags, cursor);
      if (cursor != NULL)
      {
         SetViewCursor(cursor);
      }
   }
   else if (m_mouse_click_resource->is_active())
   {
      HCURSOR cursor = NULL;
      if (m_drag && m_mouse_click_resource->get_click_type() != SINGLE_CLICK)
      {
         m_mouse_click_resource->on_drag(get_curr_map(), point, nFlags, &cursor, &hint);
      }
      else if (m_mouse_click_resource->get_click_type() == SINGLE_CLICK)
      {
         cursor = AfxGetApp()->LoadCursor(IDC_GET_MOUSE_CLICK_CURSOR);
      }
      else if (m_mouse_click_resource->get_click_type() == DRAG_GEORECT)
      {
         cursor = AfxGetApp()->LoadCursor(IDC_RECTANGLE_CURSOR);
      }
      else if (m_mouse_click_resource->get_click_type() == DRAG_GEOCIRCLE)
      {  
         cursor = AfxGetApp()->LoadCursor(IDC_OVAL_CURSOR);
      }

      if (cursor != NULL)
         SetViewCursor(cursor);
   }

   // else test select at this point
   else if (!m_hand_scroll)
   {
      // reset the tooltip is over a moving object flag.  If an overlay sets 
      // this flag, then the tooltip will not get erased when invalidating an
      // area
      m_tooltip_over_moving_object = FALSE;

      int iResult = OVL_get_overlay_manager()->test_select(GetFvMapView(), point, nFlags, &hint);

      if (iResult != SUCCESS && is_cursor_in_view)
      {
         // now allow the Map to handle the hint
         try
         {
            IDispatchPtr dispatch;
            m_map_engine->GetMapHandler(m_map_engine->get_curr_map()->source(), &dispatch);

            // query for map information interface
            IMapUserInterfacePtr smpMapUserInterface = dispatch;

            hint = NULL;

            if (smpMapUserInterface != NULL)
            {
               long lCursor;
               _bstr_t bstrTooltipText, bstrHelpText;
               MapStatusCodeEnum status = smpMapUserInterface->GetHelpHint(
                  m_map_engine->get_curr_map()->m_map,
                  m_cursor_position_geo.lat, m_cursor_position_geo.lon,
                  &lCursor,
                  bstrTooltipText.GetAddress(), bstrHelpText.GetAddress());

               if (status == MAP_STATUS_OK)
               {
                  HCURSOR cursor = (HCURSOR)lCursor;
                  if (cursor != NULL)
                  {
                     SetViewCursor(cursor);
                  }

                  m_hint->set_help_text((char *)bstrHelpText);
                  m_hint->set_tool_tip((char *)bstrTooltipText);
                  hint = m_hint;

                  // we have to do this because the current arch assumes that the
                  // hint was set by a call to selected, test_selected, etc...
                  TipTimer::set_text(m_hint->get_tool_tip());
               }
            }
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("OnLeftMouseDown failed: %s", (char *)e.Description());
            ERR_report(msg);
         }
      }
   }

   // override the cursor if we are forcing a hand drag and the point is inside
   // the view
   if ((!m_hand_scroll && (m_force_hand_scroll || m_hand_mode)) &&
      client_rect.PtInRect(point))
   {
      SetViewCursor(AfxGetApp()->LoadCursor(IDC_HAND0));
   }

   // otherwise, use the default cursor if none has been set
   else if (m_cursor == NULL)
   {
      SetViewCursor(OVL_get_overlay_manager()->get_default_cursor());
   }

   static CTextInfoPane *pHelpTextInfoPane = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));

   // use overlay hints for help text and tool tip
   if (m_hand_mode)
   {
      if (pHelpTextInfoPane)
         pHelpTextInfoPane->SetText("Press Esc or the Continuous Pan button to exit this tool");
      clear_tool_tip();
   }
   else if (m_force_hand_scroll)
   {
      if (pHelpTextInfoPane)
         pHelpTextInfoPane->SetText("");
      clear_tool_tip();
   }
   else if (hint) 
   {
      // set status bar text
      if (pHelpTextInfoPane)
         pHelpTextInfoPane->SetText(hint->get_help_text());

      // put up tool tip
      if (!m_bTooltipWasSet)
         do_tool_tip(hint, point);
   }
   // if we are in the Zoom Tool mode, then put help text for exiting the
   // mode in the status bar
   else if (m_map_change_tool != NULL)
   {
      if (pHelpTextInfoPane)
         pHelpTextInfoPane->SetText(m_map_change_tool->get_help_text());
   }
   else if (!m_bTooltipWasSet)
   {
      // clear status bar text
      if (pHelpTextInfoPane && frame->m_statusBarMgr->m_currentHelpText.GetLength() == 0)
         pHelpTextInfoPane->SetText("");

      // clear the tool tip if it exist
      clear_tool_tip();
   }

   // Update the map meta data
   static CTextInfoPane *pMetaDataInfoPane = static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(STATUS_BAR_MAP_META_DATA));
   if (pMetaDataInfoPane != NULL && !m_scrolling)
   {
      CString strMetaData;

      if (pMetaDataInfoPane->GetParentStatusBar() != NULL && 
         (get_curr_map()->actual_source() == CADRG || get_curr_map()->source() == CIB))
      {
         try
         {
            ICadrgMapInformationPtr smpCadrgMapInformation;
            CO_CREATE(smpCadrgMapInformation, __uuidof(CadrgMapHandler));

            d_geo_t map_ll, map_ur;
            get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);

            MapScale scale = get_curr_map()->scale();

            degrees_t lat, lon;
            get_curr_map()->surface_to_geo(point.x, point.y, &lat, &lon);

            _bstr_t bstrMapDesig, bstrEditionId, bstrSignificantDate;
            smpCadrgMapInformation->GetRpfInfoEx(scale.GetScale(), scale.GetScaleUnits(), 
               _bstr_t(get_curr_map()->series().get_string()), lat, lon,
               bstrMapDesig.GetAddress(), bstrEditionId.GetAddress(), bstrSignificantDate.GetAddress());

            strMetaData = (char *)smpCadrgMapInformation->GetFormattedRpfInfo(_bstr_t(get_curr_map()->series().get_string()), bstrMapDesig,
               bstrEditionId, bstrSignificantDate);
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("Error getting CADRG meta data: %s", (char *)e.Description());
            ERR_report(msg);
         }
      }
      else if (pMetaDataInfoPane->GetParentStatusBar() != NULL && 
         get_curr_map()->source() == "DTED")
      {
         try
         {
            IDtedMapInformationPtr smpDtedMapInformation;
            CO_CREATE(smpDtedMapInformation, __uuidof(DtedMapHandler));

            MapScale scale = get_curr_map()->scale();

            degrees_t lat, lon;
            get_curr_map()->surface_to_geo(point.x, point.y, &lat, &lon);

            _bstr_t bstrMapDesig, bstrSignificantDate;
            long lEditionNum;
            smpDtedMapInformation->GetDtedInfoEx(scale.GetScale(), scale.GetScaleUnits(), 
               _bstr_t(get_curr_map()->series().get_string()), lat, lon,
               bstrMapDesig.GetAddress(), &lEditionNum, bstrSignificantDate.GetAddress());

            if (bstrMapDesig.length() > 0)
               strMetaData.Format("%s, Ed %d, %s", (char *)bstrMapDesig, lEditionNum, (char *)bstrSignificantDate);
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("Error getting DTED meta data: %s", (char *)e.Description());
            ERR_report(msg);
         }
      }
      else if (pMetaDataInfoPane->GetParentStatusBar() != NULL && get_curr_map()->actual_source() == ECRG)
      {
         try
         {
            IEcrgMapInformationPtr spEcrgMapInformation;
            CO_CREATE(spEcrgMapInformation, __uuidof(EcrgMapHandler));

            d_geo_t map_ll, map_ur;
            get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);

            MapScale scale = get_curr_map()->scale();

            degrees_t lat, lon;
            get_curr_map()->surface_to_geo(point.x, point.y, &lat, &lon);

            _bstr_t bstrMapDesig, bstrEditionId, bstrSignificantDate;
            spEcrgMapInformation->GetEcrgInfoEx(scale.GetScale(), scale.GetScaleUnits(), 
               _bstr_t(get_curr_map()->series().get_string()), lat, lon,
               bstrMapDesig.GetAddress(), bstrEditionId.GetAddress(), bstrSignificantDate.GetAddress());

            strMetaData = (char *)spEcrgMapInformation->GetFormattedEcrgInfo(_bstr_t(get_curr_map()->series().get_string()), bstrMapDesig,
               bstrEditionId, bstrSignificantDate);
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("Error getting ECRG meta data: %s", (char *)e.Description());
            ERR_report(msg);
         }
      }

      pMetaDataInfoPane->SetText(strMetaData);
   }
}



void MapView::OnLButtonDown(UINT nFlags, CPoint point)
{
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      m_left_button_down = TRUE;
      m_mouse_flags = nFlags;
      GetGeospatialViewController()->OnLeftMousePressed(nFlags, point);
      return;
   }
   SHORT lbutton_state;
   boolean_t drag;
   CMainFrame *frame = (CMainFrame *) GetParentFrame();

   // don't process mouse input before first map is drawn
   if (m_first_map_up == FALSE)
      return;

   // lose the mouse click when getting focus - unless a client application
   // is requesting  a mouse click from the map
   if (m_lose_mouse_click && !m_mouse_click_resource->is_active())
      return;

   // save mouse flags
   m_mouse_flags = nFlags;

   // save cursor position
   m_mouse_down_point = point;
   get_curr_map()->surface_to_geo(point.x, point.y, &m_mouse_down_lat, &m_mouse_down_lon);

   // capture all mouse inputs
   m_left_button_down = TRUE;
   SetCapture();

   // the tool-tip will not get erased if the m_tooltip_over_moving_object
   // flag is set.  We want to force the tool-tip to get erased
   m_tooltip_over_moving_object = FALSE;

   // erase tool tip
   clear_tool_tip();

   // get the mouse state before the call to select, so lost left mouse button
   // up can be detected after the call to select()
   if (GetSystemMetrics(SM_SWAPBUTTON))
      lbutton_state = GetAsyncKeyState(VK_RBUTTON);
   else
      lbutton_state = GetAsyncKeyState(VK_LBUTTON);

   // if the mouse click resource handles the left mouse button down then we can
   // just return
   if (m_mouse_click_resource->on_left_button_down(get_curr_map(), point, m_drag))
   {
      return;
   }

   // don't allow scrolling outside map's visible surface
   double lat, lon;
   if ( get_curr_map()->surface_to_geo(point.x, point.y, &lat, &lon) != SUCCESS )
      return;


   if (m_get_position_from_map)
   {
      // reset the flag...
      m_get_position_from_map = FALSE;

      // convert mouse point to geo location
      double lat, lon;
      if ( get_curr_map()->surface_to_geo(point.x, point.y, &lat, &lon) != SUCCESS )
         return;

      // if we can snap to the point
      SnapToInfo snap_to;
      boolean_t bSnapToStatus;
      if (bSnapToStatus = OVL_get_overlay_manager()->test_snap_to(get_curr_map(), point))
      {
         OVL_get_overlay_manager()->do_snap_to(get_curr_map(), point, &snap_to);
         lat = snap_to.m_lat;
         lon = snap_to.m_lon;
      }

      // get a pointer to the current gps trail
      C_gps_trail *gps_trail = C_gps_trail::get_topmost_trail();

      // call the proper function based on the type
      switch(m_get_position_from_map_type)
      {
      case GET_POSITION_ADD_COAST_TRACK:
         gps_trail->get_coast_track_mgr()->add_coast_track(lat, lon);
         break;

      case GET_POSITION_MODIFY_COAST_TRACK:
         gps_trail->get_coast_track_mgr()->modify_coast_track(m_get_position_from_map_index, lat, lon);
         break;

      case GET_POSITION_ADD_RANGE_BEARING:
         gps_trail->get_rb_mgr()->add(lat, lon, snap_to, bSnapToStatus);
         break;

      case GET_POSITION_MODIFY_RANGE_BEARING:
         gps_trail->get_rb_mgr()->modify(m_get_position_from_map_index, lat, lon, snap_to, bSnapToStatus);
         break;

      case GET_POSITION_STAT_BAR_BULLSEYE:
         CStatBarBullseyePropPage::m_dLat = lat;
         CStatBarBullseyePropPage::m_dLon = lon;

         if (bSnapToStatus)
            CStatBarBullseyePropPage::m_strDescription = snap_to.m_description_rs;
         else
            CStatBarBullseyePropPage::m_strDescription = "Fixed Location";

         frame->OnOptionsFormat(3); // start the dialog on the bullseye page
         break;
      }

      return;
   }

   if (m_force_hand_scroll || m_hand_mode)
   {
      // enable left mouse button double click
      m_lbutton_double_click_enable = TRUE;

      m_drag = FALSE;

      // Select the closed hand icon to denote that the map can be scrolled.  
      // Also set a flag, m_scrolling, so we know that we are in this case.
      SetViewCursor(AfxGetApp()->LoadCursor(IDC_CLOSED_HAND));

      m_scrolling = TRUE;
      m_hand_scroll = TRUE;

      m_scroll_origin = m_old_point = point;
      get_curr_map()->surface_to_geo(point.x, point.y, 
         &m_geo_scroll_origin.lat, &m_geo_scroll_origin.lon);
   }
   // handle the dragging of the zoom window
   else if (m_map_change_tool != NULL)
   {
      m_map_change_tool->on_left_button_down(get_curr_map(), point, nFlags); 
   }
   // check for selection and dragging
   else
   {
      int iResult = OVL_get_overlay_manager()->select(GetFvMapView(), point, nFlags, &drag);

      if ( iResult == SUCCESS )
      {
         // We used to want to disable the ability to double-click an icon,
         // but now we want the double-click to bring up any links associated
         // with the icon (if any); I'm leaving the code here, though, in
         // case we need to make changes in the future to disable double-clicks
         // in certain situations
         //m_lbutton_double_click_enable = FALSE;
         m_lbutton_double_click_enable = TRUE;
      }
      else
      {
         BOOL mouse_handled = FALSE;

         // Allow the map a chance to handle the left mouse down
         try
         {
            IDispatchPtr dispatch;
            m_map_engine->GetMapHandler(m_map_engine->get_curr_map()->source(), &dispatch);

            // query for map information interface
            IMapUserInterfacePtr smpMapUserInterface = dispatch;

            if (smpMapUserInterface != NULL)
            {
               _bstr_t bstrHeading, bstrText;
               VARIANT_BOOL bIsRichText;
               smpMapUserInterface->GetInfoAtLocation(m_map_engine->get_curr_map()->m_map, m_mouse_down_lat, m_mouse_down_lon,
                  bstrHeading.GetAddress(), bstrText.GetAddress(), &bIsRichText);

               if (bstrText.length() != 0)
               {
                  MapInfoFormatEnum format_type = MAP_INFO_PLAIN_TEXT;
                  if ( bIsRichText == VARIANT_TRUE )
                     format_type = MAP_INFO_RICH_TEXT;

                  CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, (char *)bstrText, (char *)bstrHeading, NULL, format_type);

                  mouse_handled = TRUE;
               }
            }
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("OnLeftMouseDown failed: %s", (char *)e.Description());
            ERR_report(msg);
         }

         // if the mouse wasn't handled by anything, then start the smooth pan timer
         if (!mouse_handled)
         {
            // since no overlay or map object was hit, close the info dlg if it's up
            CRemarkDisplay::close_dlg();

            if (!g_SmoothPan_timer)  //if we don't have a timer yet...
            {
               const UINT timeout = 500; // 1/2 second
               if (!(g_SmoothPan_timer = new SmoothPanTimer(this, timeout))) //create one
                  ERR_report("new SmoothPanTimer failed.");

               UTL_get_timer_list()->add(g_SmoothPan_timer); //add to timer list
            }   

            // reset and start timer
            g_SmoothPan_timer->reset();
            g_SmoothPan_timer->start();
         }
      }
   }

   // get left mouse button state
   if (GetSystemMetrics(SM_SWAPBUTTON))
      lbutton_state = GetAsyncKeyState(VK_RBUTTON);
   else
      lbutton_state = GetAsyncKeyState(VK_LBUTTON);

   // guard against lost left mouse button up
   if (!m_left_button_down || lbutton_state == 0 || 
      !(lbutton_state & 0x8000) || (lbutton_state & 0x0001))
   {
      nFlags &= ~MK_LBUTTON;
      OnLButtonUp(nFlags, point);
   }
}

void MapView::OnLButtonUp(UINT nFlags, CPoint point)
{
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      GetGeospatialViewController()->OnLeftMouseRelease(nFlags, point);
      m_left_button_down = FALSE;
      m_lose_mouse_click = FALSE;
      return;
   }
   // don't process mouse input until first map is up
   if (m_first_map_up == FALSE)
      return;

   // destroy the smooth pan timer
   if (g_SmoothPan_timer)
   {
      if (g_SmoothPan_timer->m_force_smooth_pan)
         m_force_hand_scroll = FALSE;

      POSITION pos = UTL_get_timer_list()->Find(g_SmoothPan_timer);
      if (pos)
         UTL_get_timer_list()->remove_at(pos);

      delete g_SmoothPan_timer;
      g_SmoothPan_timer = NULL;
   }

   // lose the mouse click when getting focus
   if (m_lose_mouse_click && !m_mouse_click_resource->is_active())
   {
      m_lose_mouse_click = FALSE;
      m_mouse_click_resource->on_cancel_drag(get_curr_map());

      // Send ESC key to all overlays to cancel a drag operation if necessary
      OVL_get_overlay_manager()->offer_keydown(GetFvMapView(), VK_ESCAPE, 0, 0);
      return;
   }

   // invalidate the view one last time if we are dragging and scrolling
   if (m_drag && m_scrolling)
   {
      m_scrolling = FALSE;
      invalidate_view(FALSE);
   }

   // save mouse flags
   m_mouse_flags = nFlags;

   // left mouse button is now up, release capture and clear the down flag
   m_left_button_down = FALSE;
   ReleaseCapture();

   // perform drop, if a drag is in progress
   if (m_mouse_click_resource->on_drop(get_curr_map(), point, nFlags))
   {
      m_drag = FALSE;
      SetViewCursor(OVL_get_overlay_manager()->get_default_cursor());
   }
   else if (m_drag)
   {
      m_drag = FALSE;
   }
   else
      OVL_get_overlay_manager()->on_left_mouse_button_up(GetFvMapView(), point, nFlags);

   // finish zoom window, if one is taking place
   if (m_map_change_tool != NULL)
   {
      m_map_change_tool->on_left_button_up(get_curr_map(), point, nFlags);

      // toggle the tool off (non-sticky)
      delete m_map_change_tool;
      m_map_change_tool = NULL;

      SetViewCursor(m_old_cursor);
   }

   // finish scrolling map, if a scroll is in progress
   if (m_scrolling && m_hand_scroll)
   {
      m_scrolling = FALSE;
      m_hand_scroll = FALSE;

      // get the current width and height of the screen
      int screen_width, screen_height;
      if (get_curr_map()->get_surface_size(&screen_width, &screen_height) != SUCCESS)
         return;

      int status = SUCCESS;

      int delta_x = (screen_width / 2) + m_scroll_origin.x - point.x;
      int delta_y = (screen_height / 2) + m_scroll_origin.y - point.y;

      int x, y;
      get_curr_map()->geo_to_surface(m_geo_scroll_origin.lat, m_geo_scroll_origin.lon,
         &x, &y);

      int scroll_amount_x = x - point.x;
      int scroll_amount_y = y - point.y;

      int new_x = (screen_width / 2) + scroll_amount_x;
      int new_y = (screen_height / 2) + scroll_amount_y;

      if (get_curr_map()->point_in_surface(new_x, new_y))
      {
         status = m_map_engine->change_center(new_x,new_y);

         d_geo_t new_center;
         get_curr_map()->surface_to_geo(new_x, new_y, &new_center.lat,
            &new_center.lon);
         m_map_params.SetCenter(new_center);

         // force the map to redraw from scratch
         m_map_engine->SetCurrentMapInvalid();
      }

      if (status == SUCCESS)
      {
         invalidate_view(FALSE);
      }
      else
         OnMapError(status);
   }
}

LRESULT MapView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      GeospatialViewController* pgsvc = GetGeospatialViewController();
      if ( pgsvc != nullptr )
      {
         FlightInputDeviceStateType* pfidst = pgsvc->GetInputDevice();
         if ( pfidst != nullptr )
         {
            short zDelta = min( max( (short) HIWORD(wParam), -1), +1 );  // wheel rotation
            switch ( pfidst->GetActiveCam() )
            {
            case fvw::CAMERA_MODE_FLIGHT:
               {
                  pfidst->mouse_wheel_change_value = zDelta;
                  return S_OK;
               }
            case fvw::CAMERA_MODE_GROUND_VIEW:
               {
                  return S_OK;
               }
            case fvw::CAMERA_MODE_PAN:
               {
                  if ( pfidst->shift || pfidst->control )
                  {
                     pfidst->mouse_wheel_change_value = zDelta;
                     return S_OK;
                  }
               }
            }  // switch
         }
      }
   }

   int status = FAILURE;

   int fwKeys = LOWORD(wParam);  // key flags
   short zDelta = (short)HIWORD(wParam);  // wheel rotation
   BOOL over_this_view = FALSE;
   CFrameWnd *parent_frame;


   CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
   CPoint view_pt(pt);

   ScreenToClient(&view_pt);

   if ((parent_frame = GetParentFrame()) == NULL)
      return TRUE;

   // Pass any mouse wheel message to the current overlay.  Done if handled
   if ( OVL_get_overlay_manager()->offer_mousewheel( GetFvMapView(), zDelta, fwKeys ) )
      return TRUE;

   CRect clientRect;
   GetClientRect(&clientRect);

   if (clientRect.PtInRect(view_pt))
   {
      m_move_cursor = TRUE;
      over_this_view = TRUE;
   }
   else
   {
      CVerticalViewDisplay *pVerticalDisplay;
      CVerticalViewProperties *pVerticalDisplayProperties;
      CMainFrame *pFvwMainFrame = static_cast<CMainFrame*>(parent_frame);

      if (pFvwMainFrame)
      {
         pVerticalDisplay = pFvwMainFrame->GetVerticalDisplay();
         pVerticalDisplayProperties = pFvwMainFrame->m_pVerticalDisplayProperties;

         if (pVerticalDisplay && pVerticalDisplayProperties)
         {
            CPoint vertDisplayPt(pt);
            pVerticalDisplay->ScreenToClient(&vertDisplayPt);

            CRect vertDisplayRect;
            pVerticalDisplay->GetClientRect(&vertDisplayRect);

            // if the mouse pointer is within the Vertical Display window...
            if (vertDisplayRect.PtInRect(vertDisplayPt))
            {
               int BaseAltitude = pVerticalDisplayProperties->m_nBaseAltitudeFeet;
               int AltitudeRange = pVerticalDisplayProperties->m_nAltitudeRangeFeet;
               int MaxAltitudeRangeValue = pVerticalDisplayProperties->m_nAltitudeRangeMaximumValue;

               // if the control key is depressed, modify the Altitude Range...
               if ((fwKeys & (MK_CONTROL | MK_SHIFT)) == MK_CONTROL)
               {
                  int nAltitudeRangeDelta = (zDelta/WHEEL_DELTA) * VD_SCROLL_WHEEL_INCREMENT;  // one mouse wheel notch = 100 feet MSL
                  AltitudeRange += nAltitudeRangeDelta;

                  // enforce boundary conditions to keep the Altitude Range
                  // within the limits defined for the Vertical Display...
                  if (AltitudeRange < MIN_MSL_RANGE)
                     AltitudeRange = MIN_MSL_RANGE;
                  else if (AltitudeRange > MaxAltitudeRangeValue)
                     AltitudeRange = MaxAltitudeRangeValue;

                  // if the Vertical Display Properties Dialog is open, update it with the new value
                  // for the Altitude Range and then ask it to redraw the Vertical Display graph
                  if (pVerticalDisplayProperties->VDPropertiesDialogIsOpen())
                  {
                     // update the Altitude Range value in the Vertical Display Properties dialog...
                     if (pVerticalDisplayProperties->m_pVerticalDisplayPropertiesDialog)
                     {
                        pVerticalDisplayProperties->m_pVerticalDisplayPropertiesDialog->UpdateAltitudeRange(AltitudeRange);
                        pVerticalDisplayProperties->m_pVerticalDisplayPropertiesDialog->RedrawVerticalDisplay();
                     }
                  }
                  // otherwise, update the Vertical Display Properties object with the current
                  // altitude values, the redraw the Vertical Display from here...
                  else
                  {
                     // update the Vertical Display Properties altitude values...
                     pVerticalDisplayProperties->UpdateAltitudes(BaseAltitude, AltitudeRange, MaxAltitudeRangeValue);

                     // if the Vertical Display is open, call Invalidate() on the mainframe to update it...
                     if (pVerticalDisplayProperties->VDIsOpen())
                     {
                        pVerticalDisplay->RedrawVerticalDisplay();
                        pFvwMainFrame->Invalidate();
                     }
                  }
               }
               else // the control key is not depressed, so modify the Base Altitude...
               {
                  int nBaseAltitudeDelta = (zDelta/WHEEL_DELTA) * VD_SCROLL_WHEEL_INCREMENT;  // one mouse wheel notch = 100 feet MSL
                  BaseAltitude += nBaseAltitudeDelta;

                  // enforce boundary conditions to keep the Base Altitude
                  // within the limits defined for the Vertical Display...
                  if (BaseAltitude < MIN_BASE_MSL)
                     BaseAltitude = MIN_BASE_MSL;
                  else if (BaseAltitude > MAX_BASE_MSL)
                     BaseAltitude = MAX_BASE_MSL;

                  // if the Vertical Display Properties Dialog is open, update it with the new value
                  // for the Base Altitude and then ask it to redraw the Vertical Display graph
                  if (pVerticalDisplayProperties->VDPropertiesDialogIsOpen())
                  {
                     // update the Base Altitude value in the Vertical Display Properties dialog...
                     if (pVerticalDisplayProperties->m_pVerticalDisplayPropertiesDialog)
                     {
                        pVerticalDisplayProperties->m_pVerticalDisplayPropertiesDialog->UpdateBaseAltitude(BaseAltitude);
                        pVerticalDisplayProperties->m_pVerticalDisplayPropertiesDialog->RedrawVerticalDisplay();
                     }
                  }
                  // otherwise, update the Vertical Display Properties object with the current
                  // altitude values, the redraw the Vertical Display from here...
                  else
                  {
                     // calculate the new maximum allowable value for the Altitude Range, such that the maximum possible
                     // altitude in the Vertical Display graph (BaseAltitude + maximum Altitude Range)
                     // remains equal to MAX_MSL_IN_VERTICAL_DISPLAY...
                     MaxAltitudeRangeValue = MAX_MSL_IN_VERTICAL_DISPLAY - BaseAltitude;

                     // if the current Altitude Range value is greater than the new Altitude Range maximum
                     // value, set the current value equal to the new maximum...
                     if (AltitudeRange > MaxAltitudeRangeValue)
                        AltitudeRange = MaxAltitudeRangeValue;

                     // update the Vertical Display Properties altitude values...
                     pVerticalDisplayProperties->UpdateAltitudes(BaseAltitude, AltitudeRange, MaxAltitudeRangeValue);

                     // if the Vertical Display is open, call Invalidate() on the mainframe to update it...
                     if (pVerticalDisplayProperties->VDIsOpen())
                     {
                        pVerticalDisplay->RedrawVerticalDisplay();
                        pFvwMainFrame->Invalidate();
                     }
                  }
               }

               return true;  // the mouse pointer is within the Vertical Display window, so
               // return true here so that the map doesn't get scaled
            }
         }
      }
   }

   // CONTROL is for ZOOM in/out
   if ((fwKeys & (MK_CONTROL | MK_SHIFT)) == MK_CONTROL)
   {
      // do nothing if zoom is not valid for the current scale
      if (MAP_zoom_percent_valid(GetMapType().get_scale()) == FALSE)
         return TRUE;

      int zoom_percent = GetMapProjParams().zoom_percent;
      int new_zoom_percent;
      if (zDelta > 0)
      {
         if (!ui::GetNextLargestZoomPercent(zoom_percent, &new_zoom_percent))
            return TRUE;
      }
      else
      {
         if (!ui::GetNextSmallestZoomPercent(zoom_percent, &new_zoom_percent))
            return TRUE;
      }

      if (m_cursor_position_geo_valid)
         status = ChangeZoomPercent(new_zoom_percent, m_cursor_position_geo);
      else
         status = ChangeZoomPercent(new_zoom_percent);
   }
   // SHIFT is for STEP in/out
   else if ( ( fwKeys & ( MK_SHIFT | MK_CONTROL ) ) == MK_SHIFT )
   {
      const int step_size = (zDelta > 0) ?
            ui::kZoomPercentageStepSize : -ui::kZoomPercentageStepSize;

      if (m_cursor_position_geo_valid)
         ChangeZoomPercentByStep(step_size, m_cursor_position_geo);
      else
         ChangeZoomPercentByStep(step_size);
   }
   // PLAIN is for SCALE in/out
   else if ( ( fwKeys & ( MK_SHIFT | MK_CONTROL ) ) == 0 )
   {
      if (zDelta > 0)
      {
         if (m_cursor_position_geo_valid)
            status = ScaleIn(m_cursor_position_geo);
         else
            status = ScaleIn();
      }
      else
      {
         if (m_cursor_position_geo_valid)
            status = ScaleOut(m_cursor_position_geo);
         else
            status = ScaleOut();
      }

      if (status == SUCCESS)
         m_move_cursor = TRUE;
   }

   // Probably control-shift
   else
      status = SUCCESS;

   return TRUE;
}

//static 
void MapView::MapContextMenuCallback(CString strMenuItemText, LPARAM)
{
   try
   {
      MapView *pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
      if (pView)
      {
         IDispatchPtr dispatch;
         pView->get_map_engine()->GetMapHandler(pView->get_map_engine()->get_curr_map()->source(), 
            &dispatch);

         // query for map information interface
         IMapUserInterfacePtr smpMapUserInterface = dispatch;

         _bstr_t bstrHeading, bstrText;
         VARIANT_BOOL bIsRichText;

         smpMapUserInterface->GetInfoOnMenuItem(_bstr_t(strMenuItemText), bstrHeading.GetAddress(), bstrText.GetAddress(), &bIsRichText);

         if (bstrText.length() != 0)
         {
            MapInfoFormatEnum format_type = MAP_INFO_PLAIN_TEXT;
            if ( bIsRichText == VARIANT_TRUE )
               format_type = MAP_INFO_RICH_TEXT;

            CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, (char *)bstrText, 
               (char *)bstrHeading, NULL, format_type);
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnMenuItemSelected failed: %s", (char *)e.Description());
      ERR_report(msg);
   }
}

void MapView::OnRButtonDown(UINT nFlags, CPoint point)
{
   bool is_general_perspective_projection =
      (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION);
   if (is_general_perspective_projection)
   {
      GetGeospatialViewController()->GetInputDevice()->right_pressed = true;
      GetGeospatialViewController()->OnRightMousePressed(nFlags, point);
      m_lose_mouse_click = FALSE;
   }

   // save cursor position
   ViewMapProj* curr_map = get_curr_map();
   FalconViewOverlayLib::IFvMapViewPtr map_view = GetFvMapView();
   m_mouse_down_point = point;

   if (is_general_perspective_projection)
   {
      if (m_cursor_position_geo_valid)
      {
         m_mouse_down_lat = m_cursor_position_geo.lat;
         m_mouse_down_lon = m_cursor_position_geo.lon;
      }
      else
         return;
   }
   else
   {
      curr_map->surface_to_geo(point.x, point.y,
         &m_mouse_down_lat, &m_mouse_down_lon);
   }
   CView::OnRButtonDown(nFlags, point);
}

void MapView::OnRButtonUp(UINT nFlags, CPoint point)
{
   /*
      @TODO: since GENERAL_PERSPECTIVE_PROJECTION actions can occur here too,
      need to rely less on  curr_map which will be corrupt if the user starts
      in GENERAL_PERSPECTIVE_PROJECTION or otherwise the calls to it will 
      not be up to date. Using m_map_params is the way to go forward.
   */
   bool is_general_perspective_projection =
      (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION);
   if (is_general_perspective_projection)
   {
      GetGeospatialViewController()->GetInputDevice()->right_pressed = false;
      m_lose_mouse_click = FALSE;
   }

   UINT id = ID_OVERLAY_POPUP;
   CMenu menu;

   // don't process mouse input until first map is up in 2D
   if (m_first_map_up == FALSE && 
      GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
      return;

   // save mouse flags
   m_mouse_flags = nFlags;

   // menu clear items; use to be at the end in the previous synchronous CMenu
   // menu display call. Now using the CMFCPopupMenu object asnyc display. Also
   // clear out the disabled/checked vectors we have to use becauase new menu
   // object does not honor the MF_DISABLED / MF_CHECKED flag values
   if (m_pContextMenu != NULL)
   {
      m_pContextMenu->ClearMenuItems();
      if (!m_nDisabledMenuIDs.empty())
      {
         m_nDisabledMenuIDs.clear();
      }
      if (!m_nCheckedMenuIDs.empty())
      {
         m_nCheckedMenuIDs.clear();
      }  
   }

   // see if mouse is being released in same position as it was clicked
   bool same_position_release = false;
   if (m_mouse_down_point.operator==(point))
   {
      same_position_release = true;
   }
   
   // save cursor position
   ViewMapProj* curr_map = get_curr_map();
   FalconViewOverlayLib::IFvMapViewPtr map_view = GetFvMapView();
   m_mouse_down_point = point;
   

   // the tool-tip will not get erased if the m_tooltip_over_moving_object
   // flag is set.  We want to force the tool-tip to get erased
   m_tooltip_over_moving_object = FALSE;

   // erase tool tip
   clear_tool_tip();

   // cancel drag
   if (!is_general_perspective_projection && m_drag)
   {
      m_mouse_click_resource->on_cancel_drag(curr_map);

      // Send ESC key to all overlays to cancel a drag operation if necessary
      OVL_get_overlay_manager()->offer_keydown(map_view, VK_ESCAPE, 0, 0);
   }

   // If the point is outside of the view window then do nothing.
   // This should only occur when capture is set because the left
   // mouse button is down.
   if (m_left_button_down)
   {
      // If this window has the mouse captured - release it.  Note if a 
      // icon manager drag was just cancelled above, then it called 
      // ReleaseCapture().  For this reason, it is not necessarily true
      // that any window has capture at this time.
      if (this == GetCapture())
         ReleaseCapture();

      return;
   }

   if (!is_general_perspective_projection)
   {
      if (OVL_get_overlay_manager()->on_right_mouse_button_down(
         map_view, point, nFlags) == SUCCESS)
      {
         return;
      }
   }

   // find out if there are recognized modifiers held down
   if ((nFlags & ( MK_SHIFT | MK_CONTROL ) ) == MK_SHIFT)
   {
      // SHIFT + RIGHT MOUSE DOWN = show list of overlays to open

      // create the popup menu
      menu.CreatePopupMenu();

      OVL_get_overlay_manager()->append_overlay_menu(&menu, FALSE);

      // display pop-up menu
      SetCursor(m_cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      RECT rect;
      GetWindowRect(&rect);
      m_popup = TRUE;
      m_need_to_redraw_after_popup = FALSE;
      // MFCPopupMenu Extension to support any number of menu items and mouse
      // wheel scrolling in VS 2012 version
      CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
      if ( !pPopupMenu->Create(this, rect.left + point.x, rect.top + point.y,
         menu.Detach(), TRUE, TRUE) )
      {
         FreeOverlayMenu();
      } 
      
      menu.DestroyMenu();
      m_popup = FALSE;

      if (m_need_to_redraw_after_popup)
         OVL_get_overlay_manager()->invalidate_all();

      UpdateWindow();
   }
   else if ((nFlags & ( MK_SHIFT | MK_CONTROL ) ) == MK_CONTROL)
   {
      // CONTROL + RIGHT MOUSE DOWN = show list of overlays to close

      // create the popup menu
      menu.CreatePopupMenu();

      OVL_get_overlay_manager()->append_overlay_menu(&menu, TRUE);

      // display pop-up menu if one or more entries were added to it
      if (menu.GetMenuItemCount() > 0)
      {
         SetViewCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
         RECT rect;
         GetWindowRect(&rect);
         m_popup = TRUE;
         m_need_to_redraw_after_popup = FALSE;
         // MFCPopupMenu Extension to support any number of menu items and mouse
         // wheel scrolling in VS 2012 version
         CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
         if ( !pPopupMenu->Create(this, rect.left + point.x, rect.top + point.y,
            menu.Detach(), TRUE, TRUE) )
         {
            FreeOverlayMenu();
         } 
         menu.DestroyMenu();
         m_popup = FALSE;

         if (m_need_to_redraw_after_popup)
            OVL_get_overlay_manager()->invalidate_all();

         UpdateWindow();
      }
   }

   // check if both control and shift keys are up
   else if ((( nFlags & ( MK_SHIFT | MK_CONTROL )) == 0) && 
      same_position_release)
   {
      UINT standard_item_count;

      // create nodes for the standard menu items and add them to the list
      CComObject<CFVMenuNodeContextMenuItem> *pMenuItem = NULL;

      CComObject<CCenterMapContextMenuItem>* pCenterMapMenuItem = NULL;
      CComObject<CCenterMapContextMenuItem>::CreateInstance(
         &pCenterMapMenuItem);
      m_pContextMenu->AppendMenuItem(pCenterMapMenuItem);

      CComObject<CScaleInContextMenuItem>* pScaleInMenuItem = NULL;
      CComObject<CScaleInContextMenuItem>::CreateInstance(&pScaleInMenuItem);
      pScaleInMenuItem->Initialize(m_cursor_position_geo,
         m_cursor_position_geo_valid);
      m_pContextMenu->AppendMenuItem(pScaleInMenuItem);

      CComObject<CScaleOutContextMenuItem>* pScaleOutMenuItem = NULL;
      CComObject<CScaleOutContextMenuItem>::CreateInstance(&pScaleOutMenuItem);
      pScaleOutMenuItem->Initialize(m_cursor_position_geo,
         m_cursor_position_geo_valid);
      m_pContextMenu->AppendMenuItem(pScaleOutMenuItem);

      CComObject<COverlayOptionsContextMenuItem>* pOverlayOptionsMenuItem =
         NULL;
      CComObject<COverlayOptionsContextMenuItem>::CreateInstance(
         &pOverlayOptionsMenuItem);
      m_pContextMenu->AppendMenuItem(pOverlayOptionsMenuItem);
      standard_item_count = 4;

      // only add North Up if the map is rotated and rotation is enabled
      if (!curr_map_is_forced_north_up() && fvw_get_view()->
         m_map_params.GetMapProjParams().rotation > 0.0)
      {
         CComObject<CNorthUpContextMenuItem>* pNorthUpMenuItem = NULL;
         CComObject<CNorthUpContextMenuItem>::CreateInstance(&pNorthUpMenuItem);
         m_pContextMenu->AppendMenuItem(pNorthUpMenuItem);
         standard_item_count++;
      }

      // let overlay add menu items
      if (is_general_perspective_projection)
      {
         // Hack storing coordinates in longs - 10^7 gives us a range from
         // -1 800 000 000 to 1 800 000 000 inside the long for a "natural"
         // value mapping and 6 significant figures.
         long fake_lat = static_cast<long>(m_mouse_down_lat*10000000);
         long fake_lon = static_cast<long>(m_mouse_down_lon*10000000);
         CPoint fake_point(fake_lon, fake_lat);
         OVL_get_overlay_manager()->menu(map_view, fake_point, m_pContextMenu);
      }
      else
      {
         OVL_get_overlay_manager()->menu(map_view, point, m_pContextMenu);

         // Give the map a chance to add menu items
         try
         {
            IDispatchPtr dispatch;
            m_map_engine->GetMapHandler(m_map_engine->get_curr_map()->source(),
               &dispatch);

            // query for map information interface
            IMapUserInterfacePtr smpMapUserInterface = dispatch;

            if (smpMapUserInterface != NULL)
            {
               _variant_t varMenuItemNames = smpMapUserInterface->GetMenuItems(
                  m_map_engine->get_curr_map()->m_map,
                  m_mouse_down_lat, m_mouse_down_lon);

               BstrSafeArray saMenuItemNames(varMenuItemNames);
               const int nNumElements = saMenuItemNames.GetNumElements();
               for(int i=0;i<nNumElements;++i)
               {
                  CComObject<CFVMenuNodeContextMenuItem>::CreateInstance(
                     &pMenuItem);
                  pMenuItem->Initialize(new CLparamMenuItem(
                     (char *)_bstr_t(saMenuItemNames[i]), 0,
                     MapContextMenuCallback));
                  m_pContextMenu->AppendMenuItem(pMenuItem);
               }
            }
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format(
               "OnRMouseDown menu items retrieval from map handler failed: %s",
               (char *)e.Description());
            ERR_report(msg);
         }
      }

      CComObject<CGetMapInfoContextMenuItem>* pGetMapInfoMenuItem = NULL;
      CComObject<CGetMapInfoContextMenuItem>::CreateInstance(
         &pGetMapInfoMenuItem);
      m_pContextMenu->AppendMenuItem(pGetMapInfoMenuItem);

      // create the popup menu
      menu.CreatePopupMenu();

      CString label;
      // add the menu items in the list to the popup menu
      // or the appropriate submenus (as stored in the menus array)
      m_pContextMenu->ResetEnumerator();
      UINT uiMenuItemFlags = 0;
      while (m_pContextMenu->MoveNext())
      {
         // insert a separator after the standard menu items
         if (id == (ID_OVERLAY_POPUP + standard_item_count))
            menu.AppendMenu(MF_SEPARATOR);

         // get the next menu item
         FalconViewOverlayLib::IFvContextMenuItemPtr spMenuItem =
            m_pContextMenu->m_pCrntMenuItem;

         // if this node is a separator
         if (spMenuItem->MenuItemName == _bstr_t(L"SEPARATOR"))
         {
            menu.AppendMenu(MF_SEPARATOR);

            // make sure we are not at the end of the list
            if (!m_pContextMenu->MoveNext())
               continue;

            // get the next node and process it - there is no need to waste
            // an ID on the separator nor do we need to have a map from the
            // separator to the node that created it
            spMenuItem = m_pContextMenu->m_pCrntMenuItem;
         }
         
         // if the item belongs in a sub-menu 
         label = (char *)spMenuItem->MenuItemName;      
         // Builds the menu with multiple levels if label is delimeted by "\"
         AddMenuItem(menu.m_hMenu, label, id, spMenuItem);
         
         // store mapping between the id and the node
         m_overlay_menu_map.SetAt(id, spMenuItem);
         id++;
      }

      // display pop-up menu
      SetCursor(m_cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      RECT rect;
      GetWindowRect(&rect);
      m_popup = TRUE;

      m_need_to_redraw_after_popup = FALSE;

      // MFCPopupMenu Extension to support any number of menu items and mouse
      // wheel scrolling in VS 2012 version
      CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
      if ( !pPopupMenu->Create(this, rect.left + point.x, rect.top + point.y,
         menu.Detach(), TRUE, TRUE) )
      {
          FreeOverlayMenu();
      }
      
      menu.DestroyMenu();
      m_popup = FALSE;

      if (m_need_to_redraw_after_popup)
         OVL_get_overlay_manager()->invalidate_all();

      UpdateWindow();
      
   }

   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      GetGeospatialViewController()->GetInputDevice()->right_pressed = false;
      m_lose_mouse_click = FALSE;
      return;
   }

   // don't lose right mouse click when getting focus - just clear the flag
   m_lose_mouse_click = FALSE;  
}

void MapView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   // lose the mouse click when getting focus
   if (m_lose_mouse_click)
      return;

   if (!m_cursor_position_geo_valid)
      return;

   // save cursor position/
   // TODO: can probably eliminate m_mouse_down_lat/lon
   m_mouse_down_lat = m_cursor_position_geo.lat;
   m_mouse_down_lon = m_cursor_position_geo.lon;

   // the tool-tip will not get erased if the m_tooltip_over_moving_object
   // flag is set.  We want to force the tool-tip to get erased
   m_tooltip_over_moving_object = FALSE;

   // erase tool tip
   clear_tool_tip();

   // if we are in 3D or the overlay manager doesn't handle the double-click,
   // then try to center the map at the double-click point
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION ||
      !OVL_get_overlay_manager()->on_dbl_click(GetFvMapView(), nFlags, point))
   {
      // Don't center map if the Shift or Ctrl keys are pressed, or either
      // of the right or middle mouse buttons are down.
      if (m_lbutton_double_click_enable && nFlags == MK_LBUTTON)
      {
         ChangeCenter(m_cursor_position_geo);
      }
   }
}

void CBaseView::do_tool_tip(HintText *hint, CPoint point)
{
   CTipDisplay tip;
   CRect client;

   // can't be NULL
   if (hint == NULL)
   {
      ERR_report("NULL hint.");
      return;
   }

   // if cursor is outside of view, then remove tool tip
   GetClientRect(client);
   client.InflateRect(-1, -1);
   if (!client.PtInRect(point))
   {
      // the tool-tip will not get erased if the m_tooltip_over_moving_object
      // flag is set.  Since the cursor is outside the view, we want to
      // force the tool-tip to get erased
      m_tooltip_over_moving_object = FALSE;

      clear_tool_tip();

      return;
   }

   // don't move tip if the hint is unchanged for an active tip
   if (tip.is_active() && hint->get_tool_tip() == TipTimer::get_text())
   {
      int delta_x;
      int delta_y;

      // compute the distance in the x direction the cursor has moved since the
      // tool tip was put up
      if (m_tool_tip_point.x < point.x)
         delta_x = point.x - m_tool_tip_point.x;
      else
         delta_x = m_tool_tip_point.x - point.x;
      // compute the distance in the y direction the cursor has moved since the
      // tool tip was put up
      if (m_tool_tip_point.y < point.y)
         delta_y = point.y - m_tool_tip_point.y;
      else
         delta_y = m_tool_tip_point.y - point.y;

      // if the cursor movement is below the threshold, then leave the tip up
      if (delta_x < 20 && delta_y < 20)
      {
         return;
      }
   }

   // change in hint, remove old tip if present
   if (TipTimer::get_text().GetLength() > 0)
   {
      // the tool-tip will not get erased if the m_tooltip_over_moving_object
      // flag is set.  Since the tool tip is changing, we want to erase the
      // tool-tip even if it was over a moving object
      m_tooltip_over_moving_object = FALSE;

      tip.clear();
   }

   // set timer to bring up the tool tip in 1/8 of a second
   if (!m_pTipTimer)
   {
      // if one doesn't already exist, create it, add it to the timer list
      m_pTipTimer = new TipTimer(this,125);
      UTL_get_timer_list()->add(m_pTipTimer);
   }  

   m_pTipTimer->set_period(125);
   m_pTipTimer->reset();
   m_pTipTimer->start();
}

//-----------------------------------------------------------------------------
void MapView::OnOLEDrag( COleDataObject* data_object, CPoint point )
{
   CTipDisplay tip;

   // Check to see if the cursor is over an icon that can be snapped to with
   // the current context.  test_snap_to() returns TRUE in this case.
   if (OVL_get_overlay_manager()->test_snap_to(get_curr_map(), point))
   {
      HintText *hint = m_hint;

      m_hint->set_help_text("Snap to point");
      m_hint->set_tool_tip("Snap To");

      // we have to do this because the current arch assumes that the
      // hint was set by a call to selected, test_selected, etc...
      TipTimer::set_text(m_hint->get_tool_tip());

      // but the timer/mousemove messages are never pumped
      // while doing the OLE drag, so just show the tip now..
      if ( m_pTipTimer && m_pTipTimer->stopped() )
         m_pTipTimer->expired();
      else
         // set timer to bring up the tool tip in 1/8 of a second...
         do_tool_tip( hint, point );

      return;
   }

   AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
   clear_tool_tip();
   return;
}

void CBaseView::clear_tool_tip()
{
   CTipDisplay tip;

   // if tool tip timer was started stop it
   if (m_pTipTimer)
      m_pTipTimer->stop();

   // remove tool tip if it is up.  To preserve the tool tip
   // over moving objects, an overlay should set the flag 
   // m_tooltip_over_moving_object accordingly in the test_selected() function
   if (tip.is_active() && !m_tooltip_over_moving_object)
      tip.clear();

   // clear the tool tip hint
   if (TipTimer::get_text().GetLength() > 0)
   {
      TipTimer::set_text("");
      m_tool_tip_point.x = -1;
      m_tool_tip_point.y = -1;
      ClipCursor(NULL);       // free the cursor to move anywhere
   }
}

void MapView::do_special_handling_for_links_submenu(CMenu &links_menu)
{
   int          i = 0;
   int          j = 0;
   int x = 0;
   CArray<BOOL, BOOL> duplicates;
   CStringArray current_menu_strings, new_menu_strings;

   // put the menu items into "current_menu_strings"
   for (x = 0; x < links_menu.GetMenuItemCount(); x++)
   {
      CString temp;
      links_menu.GetMenuString(x, temp, MF_BYPOSITION);
      if (temp != "")
         current_menu_strings.Add(temp);
   }

   // split all paths and get all text from "current_menu_strings" into "new_menu_strings"
   for (i = 0; i < current_menu_strings.GetSize(); i++)
   {
      CString current = current_menu_strings[i];

      CString drive, dir, file, ext;

      _splitpath_s(
         current,
         drive.GetBuffer(_MAX_DRIVE), _MAX_DRIVE,
         dir.GetBuffer(_MAX_DIR), _MAX_DIR,
         file.GetBuffer(_MAX_FNAME), _MAX_FNAME,
         ext.GetBuffer(_MAX_EXT), _MAX_EXT);

      drive.ReleaseBuffer();
      dir.ReleaseBuffer();
      file.ReleaseBuffer();
      ext.ReleaseBuffer();

      new_menu_strings.SetAtGrow(i, file + ext);
   }

   // compare each string in "new_menu_strings" with all the others
   duplicates.SetSize(new_menu_strings.GetSize());
   for (i = 0; i < duplicates.GetSize(); i++)
      duplicates[i] = FALSE;

   for (i = 0; i < new_menu_strings.GetSize(); i++)
      for (j = i + 1; j < new_menu_strings.GetSize(); j++)
         // if there's a match, remember the ones that are dupes
         if (new_menu_strings[i] == new_menu_strings[j])
         {
            duplicates[i] = TRUE;
            duplicates[j] = TRUE;
         }

         // go back and replace the dupes with their full paths attached
         //   for (i = 0; i < duplicates.GetSize(); i ++)
         //   {
         //      if (duplicates[i])
         //         new_menu_strings[i] = current_menu_strings[i];
         //   }

         // put all changed strings in "new_menu_strings" back into the menu
         if (links_menu.GetMenuItemCount() == (UINT)new_menu_strings.GetSize())
         {
            for (i = 0; i < duplicates.GetSize(); i++)
            {
               // replace the menu string at this position with the one from new_menu_strings
               if (!duplicates[i])
                  links_menu.ModifyMenu(
                  (UINT)i,
                  MF_BYPOSITION | MF_STRING,
                  links_menu.GetMenuItemID((UINT)i),
                  new_menu_strings[i]);
            }
         }
}

// This is a recursive function which will add the item "itemText" to the menu with the
// given ID number. The "itemText" will be parsed for delimiting "\" characters for levels between
// popup menus. If a popup menu does not exist, it will be created and inserted at the end of the menu
// FV Context Menu Request - Multiple Pull-Right Menus on Right-Click Menu
bool MapView::AddMenuItem(HMENU hTargetMenu, const CString& itemText, UINT itemID, 
                          FalconViewOverlayLib::IFvContextMenuItemPtr spMenuItem)
{
   bool bSuccess = false;

   ASSERT(itemText.GetLength() > 0);
   ASSERT(hTargetMenu != NULL);

   // first, does the menu item have any required submenus to be found/created?
   if (itemText.Find('\\') >= 0)
   {
      // yes, we need to do a recursive call on a submenu handle and with that sub
      // menu name removed from itemText

      // get the popup menu name
      CString popupMenuName = itemText.Left(itemText.Find('\\'));

      // get the rest of the menu item name minus the delimiting '\' character
      CString remainingText = itemText.Right(itemText.GetLength() - 
                                             popupMenuName.GetLength() - 1);

      // See whether the popup menu already exists
      int itemCount = ::GetMenuItemCount(hTargetMenu);
      bool bFoundSubMenu = false;
      MENUITEMINFO menuItemInfo;

      memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
      menuItemInfo.cbSize = sizeof(MENUITEMINFO);
      menuItemInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
      for (int itemIndex = 0 ; itemIndex < itemCount && !bFoundSubMenu ; itemIndex++)
      {
         ::GetMenuItemInfo(hTargetMenu, itemIndex, TRUE,  &menuItemInfo);
         if (menuItemInfo.hSubMenu != 0)
         {
            // this menu item is a popup menu (non popups give 0)
            TCHAR    buffer[MAX_PATH];
            ::GetMenuString(hTargetMenu, itemIndex, buffer, MAX_PATH, MF_BYPOSITION);
            if (popupMenuName == buffer)
            {
               // this is the popup menu we have to add to
               bFoundSubMenu = true;
            }
         }
      }
      // If exists, do recursive call, else create do recursive call and then insert it
      if (bFoundSubMenu)
      {
         bSuccess = AddMenuItem(menuItemInfo.hSubMenu, remainingText, itemID, spMenuItem);
      }
      else
      {
         // we need to create a new sub menu and insert it
         HMENU hPopupMenu = ::CreatePopupMenu();
         if (hPopupMenu != NULL)
         {
            bSuccess = AddMenuItem(hPopupMenu, remainingText, itemID, spMenuItem);
            if (bSuccess)
            {
               if (::AppendMenu(hTargetMenu, MF_POPUP, (UINT)hPopupMenu, popupMenuName) > 0)
               {
                  bSuccess = true;
                  // hPopupMenu now owned by hTargetMenu, we do not need to destroy it
               }
               else
               {
                  // failed to insert the popup menu
                  bSuccess = false;
                  // stop a resource leak
                  ::DestroyMenu(hPopupMenu);
               }
            }
         }
      }        
   }
   else
   {
      // The CMFCPopupMenu does not honor the MF_ENABLED / MF_CHECKED flags,
      // we have to set the values in a command UI handler, we are storing the
      // disabled and checked IDs for in vectors to check inside the UI handler
      if (!spMenuItem->MenuItemEnabled)
      {
         m_nDisabledMenuIDs.push_back(itemID);
      }
      if (spMenuItem->MenuItemChecked)
      {
         m_nCheckedMenuIDs.push_back(itemID);
      }

      UINT nFlags = MF_STRING;
      // no sub menus required, add this item to this HMENU item ID of 0 means we are adding a separator      
      if (::AppendMenu(hTargetMenu, nFlags, itemID, itemText) > 0)
      {
         // we successfully added the item to the menu
         bSuccess = true;
      }      
   }

   return bSuccess;
}