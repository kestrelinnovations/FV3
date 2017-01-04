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

// ar_mouse.cpp

#include "stdafx.h"
#include "param.h"
#include "geo_tool.h"
#include "fvwutil.h"
#include "..\resource.h"
#include "err.h"
#include "infodlg.h"
#include "drag_utils.h"
#include "..\mapview.h"
#include "SnapTo.h"
#include "..\StatusBarManager.h"
#include "..\mainfrm.h"
#include "ovl_mgr.h"

static int objtype;
static long *tmpobj;
static CPoint m_old_anchor_pt;
static BOOL got_snap = FALSE;
static BOOL first_point = FALSE;

int C_ar_edit::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   C_ar_obj *obj;
   CString tstr;
   C_ar_point *vtx = NULL;
   int index;
   BOOL hit_line, hit_anchor_pt;
   BOOL vertex_hit = FALSE;

   int psiz = sizeof(C_ar_point);
   int pobj = sizeof(C_ar_obj);

   ViewMapProj* map = OVL_get_overlay_manager()->get_view_map();

   if (m_bDragging)
   {
      HCURSOR cursor = NULL;
      HintText *hintText = NULL;

      drag(map, point, flags, &cursor, &hintText);

      if (cursor != NULL)
         pMapView->SetCursor(cursor);
      if (hintText != NULL)
      {
         pMapView->SetTooltipText(_bstr_t(hintText->get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hintText->get_help_text()));
      }

      return SUCCESS;
   }

   obj = line_hit_test(map, point);
   if (obj == NULL)
   {
      pMapView->SetCursor(get_default_cursor());
      return FAILURE;
   }

   if (!m_bEdit)
      return FAILURE;

   if (!m_map_displayed)
      return FAILURE;

   if (this != OVL_get_overlay_manager()->get_current_overlay())
      return FAILURE;

   if (m_nMode == MM_AR_NAVAID)
   {
      hit_line = obj->hit_test(map, point, &index, &hit_anchor_pt);
      if (index >= 0)
         pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_ARNAVAID_LINE_CURSOR));
      else
         pMapView->SetCursor(get_default_cursor());
      return SUCCESS;
   }

   if (m_nMode == MM_AR_POINT)
   {
      pMapView->SetCursor(get_default_cursor());
      return SUCCESS;
   }

   hit_line = obj->hit_test(map, point, &index, &hit_anchor_pt);
   if (hit_line && ((index >= 0) || hit_anchor_pt))
      vertex_hit = TRUE;
   if (obj->m_type == 'T')
   {
      if (index < 0)
      {
         if (hit_line)
         {
            pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_ADDPOINT_CURSOR));
            CString tstr = "Select Line / Insert Vertex";
            m_hint.set_help_text(tstr);
            m_hint.set_tool_tip(tstr);
            pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
            pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
         }
      }
      else
      {
         pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_CROSS_MOVE_CURSOR));
         CString tstr = "Select Vertex / Move Vertex";
         m_hint.set_help_text(tstr);
         m_hint.set_tool_tip(tstr);
         pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
      }
   }
   else if (obj->m_type == 'A')
   {
      CString tstr;
      if (hit_line && vertex_hit)
      {
         if (hit_anchor_pt)
         {
            pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_ROTATE_CURSOR));
            tstr = "Rotate Orbit";
         }
         else
         {
            pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_CROSS_MOVE_CURSOR));
            tstr = "Move Point";
         }
         m_hint.set_help_text(tstr);
         m_hint.set_tool_tip(tstr);
         pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
      }
      else if (hit_line)
      {
         pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR));
         CString tstr = "Move Orbit";
         m_hint.set_help_text(tstr);
         m_hint.set_tool_tip(tstr);
         pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
      }
      else
         pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_SELECT_CURSOR));
   }
   else
      pMapView->SetCursor(get_default_cursor());

   return SUCCESS;
}

int C_ar_edit::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_bDragging)
   {
      drop(OVL_get_overlay_manager()->get_view_map(), point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}

int C_ar_edit::on_right_mouse_button_down(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_bEdit && C_ar_edit::m_in_poly_mode)
   {
      C_ar_edit::end_poly_line();
      return SUCCESS;
   }

   return FAILURE;
}

CString C_ar_edit::get_new_name()
{
   CString numstr, name, tnumstr;
   int name_num;

   numstr = PRM_get_registry_string("ArEdit", "RouteNameNumber", "000");
   name_num = atoi(numstr);
   name_num++;
   if (name_num > 9999)
      name_num = 0;
   if (name_num < 1000)
      tnumstr.Format("%03d", name_num);
   else
      tnumstr.Format("%04d", name_num);

   PRM_set_registry_string("ArEdit", "RouteNameNumber", tnumstr);

   name = "UAR";
   name += numstr;
   return name;
}

// Receives all keydown messeges from the Overlay Manager and filters them 
// for this overlay... a return of false will allow the keystroke to be 
// processed normally in MapView. This function will get called only if 
// the File Overlay is current and open in its editor.
boolean_t C_ar_edit::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point)
{
   if (!m_bEdit)
      return FALSE;

   if (nChar == VK_DELETE)
   {
      delete_selected();
      OVL_get_overlay_manager()->InvalidateOverlay(this);

      return TRUE;
   }
   else if (nChar == VK_ESCAPE && m_bDragging == TRUE)
   {
      cancel_drag(OVL_get_overlay_manager()->get_view_map());
      return TRUE;
   }

   return FALSE;
}

int C_ar_edit::selected(IFvMapView* pMapView, CPoint point, UINT nFlags)
{
   CWnd* view = CWnd::FromHandle(pMapView->m_hWnd());
   CWindowDC dc(view);

   ViewMapProj* map = OVL_get_overlay_manager()->get_view_map();

   CFvwUtil *futil = CFvwUtil::get_instance();
   double lat, lon;
   C_ar_obj *obj = NULL;
   C_ar_point *pt;
   BOOL over_object = FALSE;
   BOOL hit_anchor_pt;

   m_cur_ovl = this;

   m_bGotPt = FALSE;
   m_offset_pt.x = 0;
   m_offset_pt.y = 0;

   m_drag_all = FALSE;

   m_bDragging = FALSE;

   map->surface_to_geo(point.x, point.y, &m_point_lat, &m_point_lon);

   if (map->get_vmap_bounds(&m_ll, &m_ur) != SUCCESS)
      return FAILURE;

   if (!m_in_poly_mode)
      end_draw();

   if (!m_obj_list.IsEmpty())
   {
      obj = line_hit_test(map, point);

      if (obj != NULL)
      {
         over_object = TRUE;
         m_cur_obj = obj;
      }

      if (obj == NULL)
      {
         if (!m_bEdit ||(m_nMode == MM_AR_SELECT) || (m_nMode == MM_AR_SELECT) || (m_nMode == MM_AR_ANCHOR))
            invalidate_and_clear_selects();
      }

      m_cur_vertex_num = -1;
      m_cur_point = NULL;

      if (obj != NULL)
      {
         if (valid_object(obj))
         {
            obj->hit_test(map, point, &m_cur_vertex_num, &hit_anchor_pt);
            m_cur_point = obj->get_vertex(m_cur_vertex_num);
            if (m_cur_vertex_num < 0)
            {
               obj->clear_selects();
               obj->m_selected = TRUE;
            }
         }
         else
            ASSERT(0);
      }

      if (over_object && (m_nMode != MM_AR_SELECT) && (m_nMode != MM_AR_NAVAID) && (m_nMode != MM_AR_POINT))
      {
         HCURSOR cursor;
         set_temp_select(TRUE, &cursor);
      }

      if (!m_bEdit)
      {
         if (obj != NULL)
         {
            show_info((C_icon*) obj);
            return SUCCESS;
         }
         else
            return FAILURE;
      }

      if (m_bIsReadOnly)
         return SUCCESS;

      m_new_mode = FALSE;

      // if this is not the current overlay, make it current
      if (obj != NULL)
         if (OVL_get_overlay_manager()->get_current_overlay() != this)
            OVL_get_overlay_manager()->make_current(this, TRUE);
   }

   m_selected_obj = obj;

   map->surface_to_geo(point.x, point.y, &lat, &lon);

   m_screen_moved_mode = FALSE;

   switch(m_nMode)
   {
   case MM_AR_SELECT:
      {
         m_originate = FALSE;
         int ret = select_select(obj, OVL_get_overlay_manager()->get_view_map(), nFlags, point, &m_bDragging);
         return ret;
      }
      break;

   case MM_AR_TRACK:
      // add the line to the overlay
      {
         C_ar_obj *rte;

         if (m_cur_obj == NULL) 
         {
            // create a new object
            rte = new C_ar_obj(this);
            m_start_pt = point;
            m_start_lat = m_point_lat;
            m_start_lon = m_point_lon;
            m_first_pt = FALSE;
            m_cur_pt = point;
            m_cur_lat = m_point_lat;
            m_cur_lon = m_point_lon;
            rte->m_color = m_nCurColor;
            rte->m_type = 'T';
            rte->m_id = get_new_name();

            map->surface_to_geo(point.x, point.y, &lat, &lon);
            m_obj_list.AddTail(rte);
            pt = new C_ar_point(rte);
            pt->m_lat = lat;
            pt->m_lon = lon;
            pt->add_usage("IP");
            first_point = TRUE;
            pt->m_type = rte->m_type;
            pt->m_icao = rte->m_icao;
            pt->m_id = rte->m_id;
            pt->m_direction = rte->m_direction;
            rte->m_point_list.AddTail(pt);
            m_cur_point = pt;
            rte->m_bounds.left = point.x;
            rte->m_bounds.right = point.x;
            rte->m_bounds.top = point.y;
            rte->m_bounds.bottom = point.y;
            m_cur_obj = rte;
            m_in_poly_mode = TRUE;
            m_first_drag = TRUE;
         }
         m_old_pt.x = -1000001;
      }
      view->SetCapture();
      m_bGotPt = TRUE;
      m_bDragging = TRUE;
      m_cur_pt = point;
      break;

   case MM_AR_ANCHOR:
      // add the line to the overlay
      {
         C_ar_obj *rte;

         if (m_cur_obj == NULL) 
         {
            m_originate = TRUE;

            // create a new object
            rte = new C_ar_obj(this);
            m_start_pt = point;
            m_start_lat = m_point_lat;
            m_start_lon = m_point_lon;
            m_first_pt = FALSE;
            m_cur_pt = point;
            m_cur_lat = m_point_lat;
            m_cur_lon = m_point_lon;
            rte->m_anchor_pt_lat = m_point_lat;
            rte->m_anchor_pt_lon = m_point_lon;
            rte->m_color = m_nCurColor;
            rte->m_type = 'A';
            rte->m_id = get_new_name();

            // get the default width
            CString sdata = PRM_get_registry_string("ArEdit", "ArEditDefOrbitWidth", "10");
            rte->m_anchor_width = atof(sdata);

            map->surface_to_geo(point.x, point.y, &lat, &lon);
            m_obj_list.AddTail(rte);
            rte->m_bounds.left = point.x;
            rte->m_bounds.right = point.x;
            rte->m_bounds.top = point.y;
            rte->m_bounds.bottom = point.y;
            rte->m_lat = m_point_lat;
            rte->m_lon = m_point_lon;
            rte->m_visible = FALSE;
            m_cur_obj = rte;
            m_in_poly_mode = TRUE;
            m_first_drag = TRUE;
            m_anchor_dragger = rte;
            begin_drag(map, &dc, m_anchor_dragger, point);
            m_anchor_dragger->m_anchor_left = TRUE;

         }
         m_old_pt.x = -1000001;
      }
      view->SetCapture();
      m_bGotPt = TRUE;
      m_bDragging = TRUE;         
      m_cur_pt = point;
      break;

   case MM_AR_NAVAID:
      // add the line to the overlay
      {
         C_ar_point* pt;
         int index;
         POSITION pos;

         m_bGotPt = FALSE;
         if (m_cur_obj == NULL) 
            break;
         obj->hit_test(map, point, &index, &m_navaid_vertex_anchor);
         if (index < 0)
            break;

         m_navaid_vertex_num = 0;
         if (index > 0)
         {
            if (!m_navaid_vertex_anchor)
            {
               pos = obj->m_point_list.FindIndex(index-1);
               if (pos == NULL)
                  return FALSE;
               pt = obj->m_point_list.GetAt(pos);
               m_bGotPt = TRUE;
            }
            else
            {
               pt = &(obj->m_anchor_point[index-1]);
            }
            m_navaid_vertex_num = index-1;
            m_start_pt = pt->m_pt;
            m_cur_pt = pt->m_pt;
            m_start_lat = pt->m_lat;
            m_start_lon = pt->m_lon;
            m_cur_lat = pt->m_lat;
            m_cur_lon = pt->m_lon;
         }

         m_first_drag = TRUE;
         m_old_pt.x = -1000001;
      }
      view->SetCapture();
      m_bDragging = TRUE;
      break;

   case MM_AR_POINT:
      obj = get_selected();
      if (obj != 0)
      {
         m_cursor_lat = m_point_lat;
         m_cursor_lon = m_point_lon;
         add_point(OVL_get_overlay_manager()->get_view_map(), obj);
         OVL_get_overlay_manager()->InvalidateOverlay(this);
      }
      break;
   }

   if (m_cur_obj != NULL)
   {
      if (!valid_object(m_cur_obj))
      {
         ASSERT(0);
         m_cur_obj = NULL;
         return FAILURE;
      }
      else
      {
         if (m_cur_obj->m_type == 'A')
            objtype = MM_AR_ANCHOR;
         else if (m_cur_obj->m_type == 'T')
            objtype = MM_AR_TRACK;
         else 
            objtype = ' ';
         m_cur_obj->m_selected = TRUE;
      }
   }
   return SUCCESS;
} //  end of selected

// *****************************************************************
// *****************************************************************

int C_ar_edit::select_select(C_ar_obj *obj, ViewMapProj* map, UINT nFlags, CPoint point, boolean_t *drag)
{
   CWindowDC dc(map->get_CView());
   BOOL rslt;
   double lat, lon;

   *drag = FALSE;

   m_cur_obj = obj;

   if (m_cur_obj != NULL)
   {
      *m_old_route = *obj;

      if (!valid_object(m_cur_obj))
         ASSERT(0);

      if (m_cur_obj->m_type == 'A')
         objtype = MM_AR_ANCHOR;
      else if (m_cur_obj->m_type == 'T')
         objtype = MM_AR_TRACK;
      else 
         objtype = ' ';
      m_cur_obj->m_selected = TRUE;
   }

   // if there is not object selected return
   if (obj == NULL)
   {
      invalidate_and_clear_selects();
      return FAILURE;
   }

   map->surface_to_geo(point.x, point.y, &lat, &lon);

   m_old_pt = point;
   m_old_lat = lat;
   m_old_lon = lon;
   m_start_pt = point;
   m_start_lat = lat;
   m_start_lon = lon;
   m_bGotPt = TRUE;
   *drag = TRUE;
   m_cur_obj = obj;
   rslt = FALSE;

   if (objtype == MM_AR_ANCHOR)
      rslt = select_anchor_point(obj, map, &dc, nFlags, point);
   else if (objtype == MM_AR_TRACK)
      rslt = select_track_point(obj, map, &dc, nFlags, point);
   else
      rslt = FALSE;

   if (rslt)
   {
      CWnd *pWnd = dc.GetWindow();
      if (pWnd != NULL)
         pWnd->SetCapture();

      *drag = TRUE;
      return SUCCESS;
   }
   return SUCCESS;
}
// end of select_select

// *****************************************************************
// *****************************************************************

BOOL C_ar_edit::select_track_point(C_ar_obj* rte, MapProj* map, CDC* dc,
   UINT nFlags, CPoint point)
{
   int numpt;
   C_ar_point *vtx;
   BOOL hit_anchor_pt;

   if (!valid_object(rte))
      ASSERT(0);

   if (rte->hit_test(map, point, &m_vertex_num, &hit_anchor_pt))
   {
      m_start_pt = point;
      m_start_lat = m_point_lat;
      m_start_lon = m_point_lon;
      m_old_pt = point;
      m_old_lat = m_point_lat;
      m_old_lon = m_point_lon;
      m_bGotPt = TRUE;
      m_first_drag = TRUE;
      m_cur_obj = rte;
      m_hint_pt.x = 0;
      m_hint_pt.y = 0;
      m_hint_pt2.x = 0;
      m_hint_pt2.y = 0;
      if (m_vertex_num >= 0)
      {
         CPoint oldpt = point;
         oldpt.x = -1000001;
         if (m_vertex_num == (rte->m_point_list.GetCount() - 1))
            rte->rubber_band_point(map, dc, point, oldpt, m_vertex_num);

         // figure out the distance and angle hint points
         vtx = NULL;
         numpt = rte->m_point_list.GetCount();
         if (numpt == 2)
         {
            if (m_vertex_num == 1)
               vtx = rte->get_vertex(1);
            else
               vtx = rte->get_vertex(0);
            if (vtx)
               m_hint_pt = vtx->m_pt;
         }
         else
         {
            if (m_vertex_num == 1)
            {
               vtx = rte->get_vertex(1);
               if (vtx)
                  m_hint_pt = vtx->m_pt;
            }
            else if (m_vertex_num == numpt)
            {
               vtx = rte->get_vertex(numpt-2);
               if (vtx)
                  m_hint_pt = vtx->m_pt;
            }
            else
            {
               vtx = rte->get_vertex(m_vertex_num-2);
               if (vtx)
                  m_hint_pt = vtx->m_pt;
               vtx = rte->get_vertex(m_vertex_num);
               if (vtx)
                  m_hint_pt2 = vtx->m_pt;
            }
         }

         if (vtx != NULL)
         {
            CString *type = vtx->m_usage.GetTail();
            m_draw_icon = rte->get_icon(*type);
            if (objtype == MM_AR_ANCHOR)
               m_draw_icon->begin_drag((ViewMapProj*) map, point, (LPARAM) rte);
         }

         clear_selects();
         rte->select_point(m_vertex_num, TRUE);
      }
      else
      {
         m_offset_pt = point;
         clear_selects();
         rte->m_selected = TRUE;
      }
      return TRUE;
   }
   else
      clear_selects();
   return FALSE;
}

BOOL C_ar_edit::select_anchor_point(C_ar_obj* rte, MapProj* map, CDC* dc,
   UINT nFlags, CPoint point)
{
   C_ar_point *vtx;
   BOOL hit_anchor_pt;
   BOOL hit, vertex_hit;

   if (!valid_object(rte))
      ASSERT(0);

   m_anchor_dragger = rte;

   hit = rte->hit_test(map, point, &m_vertex_num, &hit_anchor_pt);
   if ((m_vertex_num > 0) && !hit_anchor_pt)
      m_anchor_ext_pt = TRUE;
   else
      m_anchor_ext_pt = FALSE;

   if (hit && (hit_anchor_pt || (m_vertex_num >= 0)))
   {
      vertex_hit = TRUE;
      vtx = rte->get_vertex(m_vertex_num-1);
      m_anchor_ext_point = vtx;
   }
   else
      vertex_hit = FALSE;

   if (hit)
   {
      m_start_pt = point;
      m_start_lat = m_point_lat;
      m_start_lon = m_point_lon;
      m_old_pt = point;
      m_old_lat = m_point_lat;
      m_old_lon = m_point_lon;
      m_bGotPt = TRUE;
      m_first_drag = TRUE;
      m_cur_obj = rte;
      m_hint_pt.x = 0;
      m_hint_pt.y = 0;
      m_hint_pt2.x = 0;
      m_hint_pt2.y = 0;
      if ((m_vertex_num >= 0) && !hit_anchor_pt)
      {
         /*
         CPoint oldpt = point;
         oldpt.x = -1000001;
         if (m_vertex_num == (rte->m_point_list.GetCount() - 1))
         rte->rubber_band_point(map, dc, point, oldpt, m_vertex_num);

         // figure out the distance and angle hint points
         vtx = NULL;
         numpt = rte->m_point_list.GetCount();
         if (numpt == 2)
         {
         if (m_vertex_num == 1)
         vtx = rte->get_vertex(1);
         else
         vtx = rte->get_vertex(0);
         if (vtx)
         m_hint_pt = vtx->m_pt;
         }
         else
         {
         if (m_vertex_num == 1)
         {
         vtx = rte->get_vertex(1);
         if (vtx)
         m_hint_pt = vtx->m_pt;
         }
         else if (m_vertex_num == numpt)
         {
         vtx = rte->get_vertex(numpt-2);
         if (vtx)
         m_hint_pt = vtx->m_pt;
         }
         else
         {
         vtx = rte->get_vertex(m_vertex_num-2);
         if (vtx)
         m_hint_pt = vtx->m_pt;
         vtx = rte->get_vertex(m_vertex_num);
         if (vtx)
         m_hint_pt2 = vtx->m_pt;
         }
         }
         */
         if (vtx != NULL)
         {
            if (vtx->m_usage.GetCount() > 0)
            {
               CString *type = vtx->m_usage.GetTail();
               m_draw_icon = rte->get_icon(*type);
               if (objtype == MM_AR_ANCHOR)
                  m_draw_icon->begin_drag((ViewMapProj*) map, point, (LPARAM) rte);
            }
         }

         clear_selects();
         rte->select_point(m_vertex_num, TRUE);
      }
      else if (m_vertex_num >= 0)
      {
         m_offset_pt = point;
         clear_selects();
         rte->m_selected = TRUE;

         m_anchor_pt.x = -1000001;
         begin_drag(map, dc, m_anchor_dragger, point);
         m_old_anchor_pt = m_anchor_pt;
      }
      else
      {
         CPoint oldpt = point;
         oldpt.x = -1000000;
         m_offset_pt = point;
         clear_selects();
         rte->m_selected = TRUE;
      }
      if (hit && !vertex_hit)
      {
         begin_drag_all(map, dc, m_anchor_dragger, point);
         m_drag_all = TRUE;
      }

      return TRUE;
   }
   else
      clear_selects();
   return FALSE;
}

void C_ar_edit::drag(ViewMapProj* map, CPoint point, UINT nFlags, 
   HCURSOR *cursor, HintText **hint)
{
   C_ar_obj *obj;
   CFvwUtil *futil = CFvwUtil::get_instance();
   d_geo_t ll, ur;
   BOOL hit_anchor_pt;
   BOOL vertex_hit = FALSE;

   CWindowDC dc(map->get_CView());
   CWnd *view = dc.GetWindow();

   static_cast<MapView *>(map->get_CView())->ScrollMapIfPointNearEdge(point.x,
      point.y);

   if (m_cur_obj == NULL)
      return;

   if (!valid_object(m_cur_obj))
   {
      ASSERT(0);
      return;
   }

   if (!m_bEdit)
      return;

   // get the map bounds to check for map moved
   if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
      return;

   map->surface_to_geo(point.x, point.y, &m_point_lat, &m_point_lon);

   obj = m_cur_obj;

   if (!m_bGotPt)
   {
      if (m_nMode != MM_AR_SELECT)
      {
         *cursor = get_default_cursor();
      }
      else
      {
         if (obj != NULL)
         {
            switch(m_nMode)
            {
            case MM_AR_TRACK:
               {
                  if (m_cur_vertex_num < 0)
                     *cursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR); 
                  else
                     *cursor = AfxGetApp()->LoadCursor(IDC_CROSS_MOVE_CURSOR); 
               }
               break;

            default:
               *cursor = AfxGetApp()->LoadCursor(IDC_SELECT_CURSOR); 
               break;
            }
         }
         else
            *cursor = AfxGetApp()->LoadCursor(IDC_SELECT_CURSOR); 
      }
   }
   else
   {
      CRect clrect;
      view->GetClientRect(clrect);
      if (!clrect.PtInRect(point))
         *cursor = AfxGetApp()->LoadCursor(IDC_NODROP);
   }

   if ((m_ll.lat != ll.lat) || (m_ll.lon != ll.lon) ||
      (m_ur.lat != ur.lat) || (m_ur.lon != ur.lon))
   {
      m_ll = ll;
      m_ur = ur;
      m_screen_moved = TRUE;
      m_screen_moved_mode = TRUE;
      m_offset_pt.x = 0;
      m_offset_pt.y = 0;
   }
   else
      m_screen_moved = FALSE;

   switch(m_nMode)
   {
   case MM_AR_SELECT:
      drag_select(map, nFlags, point, cursor, hint);
      break;

   case MM_AR_TRACK:
      if ((m_cur_pt != point) && 
         (futil->is_point_in_client_area(view, point)))
      {
         int sx, sy;

         map->geo_to_surface(m_start_lat, m_start_lon, &sx, &sy);
         m_start_pt.x = sx;
         m_start_pt.y = sy;

         // erase the old line
         if (!m_screen_moved && m_cur_pt.x > 0)
         {
            futil->line_xor(&dc, 1, m_start_pt, m_cur_pt);
         }

         // draw the new line
         futil->line_xor(&dc, 1, m_start_pt, point);


         // snap to another overlay item if possible
         if (OVL_get_overlay_manager()->test_snap_to(map, point))
         {
            *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
            m_hint.set_help_text("Snap to point");
            m_hint.set_tool_tip("Snap To");
            *hint = &m_hint;
         }
         else
            set_line_hint(map, m_start_pt, point, hint);

         m_old_pt = m_cur_pt;
         m_old_lat = m_cur_lat;
         m_old_lon = m_cur_lon;
         m_cur_pt = point;
         m_cur_lat = m_point_lat;
         m_cur_lon = m_point_lon;
      }
      break;

   case MM_AR_ANCHOR:
      if ((m_cur_pt != point) && 
         (futil->is_point_in_client_area(view, point)))
      {
         int sx, sy;
         BOOL unique, found;

         map->geo_to_surface(m_start_lat, m_start_lon, &sx, &sy);
         m_start_pt.x = sx;
         m_start_pt.y = sy;

         if (m_first_drag)
            found = TRUE;
         else
         {
            found = m_cur_obj->hit_test(map, point, &m_vertex_num, &hit_anchor_pt);
            if (found && (hit_anchor_pt || (m_vertex_num >= 0)))
               vertex_hit = TRUE;
            if (found && (m_cur_vertex != NULL))
            {
               if (!m_cur_vertex->has_usage_string("A", &unique))
                  found = FALSE;
            }
            else
               found = FALSE;
         }
         if (found && m_first_drag)
         {
            {
               // erase the old line
               if (!m_screen_moved && m_cur_pt.x > 0)
               {
               }

               // draw the new line

               move_drag(map, &dc, m_old_pt, point, hint);

               m_old_pt = m_cur_pt;
               m_old_lat = m_cur_lat;
               m_old_lon = m_cur_lon;
               m_cur_pt = point;
               m_cur_lat = m_point_lat;
               m_cur_lon = m_point_lon;
            }
         }
         if (!vertex_hit && !m_first_drag)
            move_drag_all(map, &dc, m_old_pt, point, hint);

      }
      break;

   case MM_AR_NAVAID:
      if (futil->is_point_in_client_area(view, point))
      {
         int sx, sy;

         map->geo_to_surface(m_start_lat, m_start_lon, &sx, &sy);
         m_start_pt.x = sx;
         m_start_pt.y = sy;

         // erase the old line
         if (!m_screen_moved && m_cur_pt.x > 0)
         {
            futil->line_xor(&dc, 1, m_start_pt, m_cur_pt);
         }

         // draw the new line
         futil->line_xor(&dc, 1, m_start_pt, point);

         SnapToInfo* snap_to_info = NULL;
         CList<SnapToInfo *, SnapToInfo *> snap_to_list;
         if (OVL_get_overlay_manager()->test_snap_to(map, point))
         {
            POSITION next;
            if (OVL_get_overlay_manager()->do_snap_to(map, point, snap_to_list))
            {
               if (!snap_to_list.IsEmpty())
               {
                  next = snap_to_list.GetHeadPosition();
                  while (next != NULL)
                  {
                     snap_to_info = snap_to_list.GetNext(next);
                     if (snap_to_info->m_pnt_src_rs == SnapToInfo::NAVAID)
                     {
                        m_hint.set_help_text("Snap to NAVAID");
                        m_hint.set_tool_tip("Snap To");
                        *hint = &m_hint;
                        next = NULL;
                     }
                  }
                  // clear the list
                  while (!snap_to_list.IsEmpty())
                  {
                     snap_to_info = snap_to_list.RemoveHead();
                     delete snap_to_info;
                  }
               }
            }
         }
         m_old_pt = m_cur_pt;
         m_old_lat = m_cur_lat;
         m_old_lon = m_cur_lon;
         m_cur_pt = point;
         m_cur_lat = m_point_lat;
         m_cur_lon = m_point_lon;
      }
      break;

   } // switch
}

void C_ar_edit::drag_select(ViewMapProj* map, UINT nFlags, CPoint point, 
   HCURSOR* cursor, HintText **hint)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CPoint tpt(-1000001, -1000001);
   CRect clrect;

   if (!valid_object(m_cur_obj))
      ASSERT(0);

   CWindowDC dc(map->get_CView());
   CWnd *view = dc.GetWindow();

   view->GetClientRect(clrect);
   if (clrect.PtInRect(point))
      *cursor = AfxGetApp()->LoadCursor(IDC_SELECT_CURSOR);
   else
   {
      *cursor = AfxGetApp()->LoadCursor(IDC_NODROP);
   }

   if (!futil->is_point_in_client_area(view, point))
      return;

   switch(m_nMode)
   {
   case MM_AR_SELECT:
   case MM_AR_TRACK:
      {
         *hint = NULL;
         drag_point(m_cur_obj, map, &dc, nFlags, point, cursor, hint);
         return;
      }
      break;
   case MM_AR_ANCHOR:
      {
      }
   }  // end of case m_nMode == MM_AR_TRACK

}

BOOL C_ar_edit::override_drag_lock(CString operation, CString object_name)
{
   //display "override question and return TRUE" if the user chooses to override the drag lock
   CString msg;
   msg.Format("Object Lock is currently engaged.  Would you like to %s the %s anyway?", 
      operation, object_name); 
   return (AfxMessageBox(msg, MB_YESNO) == IDYES);
}

BOOL C_ar_edit::drag_point(C_ar_obj* obj, ViewMapProj* map, CDC* dc, 
   UINT nFlags, CPoint point, 
   HCURSOR *cursor, HintText **hint)
{
   if (!m_bGotPt)
      return FALSE;

   if (!valid_object(obj))
      ASSERT(0);

   if (m_cur_vertex_num >= 0)
   {
      if (m_screen_moved)
         m_old_pt.x = -1000001;

      // snap to another overlay item if possible
      if (OVL_get_overlay_manager()->test_snap_to(map, point))
      {
         *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
         m_hint.set_help_text("Snap to point");
         m_hint.set_tool_tip("Snap To");
         *hint = &m_hint;
      }
      else
      {
         if ((m_hint_pt2.x != 0) && (m_hint_pt2.y != 0))
         {
            if ((m_hint_pt.x != 0) && (m_hint_pt.y != 0))
               set_line_hint(map, m_hint_pt, point, m_hint_pt2, hint);
         }
         else
         {
            if ((m_hint_pt.x != 0) && (m_hint_pt.y != 0))
               set_line_hint(map, m_hint_pt, point, hint);
         }
      }


      if (!drag_utils::in_drag_threshold(m_cur_pt, point))
      {
      }

      if (objtype != MM_AR_ANCHOR)
         obj->rubber_band_point(map, dc, m_old_pt, point, m_cur_vertex_num);
      else
      {
         if (m_anchor_ext_pt)
         {
            if (m_draw_icon)
               m_draw_icon->move_drag(map, point);
         }
         else
            move_drag(map, dc, m_old_pt, point, hint);
      }

      m_first_drag = FALSE;
   }
   else
   {
      move_drag_all(map, dc, m_old_pt, point, hint);
      if (!drag_utils::in_drag_threshold(m_start_pt, point)) 
      {
         if (obj->m_type == 'T')
         {
            int vertex_num;

            obj->insert_vertex_on_line(map, m_start_pt, &vertex_num);
            m_cur_vertex_num = vertex_num;
            clear_selects();
            obj->select_point(vertex_num, TRUE);
            m_start_pt = point;
            m_bGotPt = TRUE;
         }
      }
   }

   m_old_pt = point;
   m_old_lat = m_point_lat;
   m_old_lon = m_point_lon;
   m_old_anchor_pt = m_anchor_pt;

   return TRUE;
}

void C_ar_edit::drop(ViewMapProj* map, CPoint point, UINT nFlags)
{
   CRect rc;
   CFvwUtil *futil = CFvwUtil::get_instance();
   C_ar_obj * obj;
   C_ar_point *pt;
   SnapToInfo* snap_to_info = NULL;
   CList<SnapToInfo *, SnapToInfo *> snap_to_list;

   m_bDragging = FALSE;

   obj = m_cur_obj;
   if (!valid_object(obj))
   {
      ASSERT(0);
      m_originate = FALSE;
      return;
   }

   if (m_first_drag)
      obj->m_anchor_left = TRUE;

   m_start_drag = FALSE;

   CWindowDC dc(map->get_CView());
   CWnd *view = dc.GetWindow();

   map->surface_to_geo(point.x, point.y, &m_point_lat, &m_point_lon);

   set_modified(TRUE);

   switch(m_nMode)
   {
   case MM_AR_SELECT:
      if (m_bGotPt)
         drop_select(map, nFlags, point);
      else
         clear_selects();
      break;

   case MM_AR_TRACK:
      {
         int pixels;
         map->get_pixels_around_world(&pixels);
         pixels = (pixels / 2) - 10;

         if (/*in_drag_threshold(m_start_pt, point) || */
            (!futil->is_point_in_client_area(view, point)))
         {
            CPoint tpt(-1000001, -1000001);
            m_first_pt = TRUE;
            m_cur_pt = m_start_pt;
            m_cur_lat = m_start_lat;
            m_cur_lon = m_start_lon;
            if (obj->m_point_list.GetCount() < 2)
            {
               CRect rc(point.x -10, point.y - 10, point.x + 10, point.y + 10);
               remove_route(obj);
               m_cur_obj = NULL;
               OVL_get_overlay_manager()->InvalidateOverlay(this);
            }

            break;
         }

         // prevent duplicate points
         if ((m_start_lat == m_point_lat) && (m_start_lon == m_point_lon))
         {
            m_originate = FALSE;
            return;
         }

         m_start_pt = point;
         m_start_lat = m_point_lat;
         m_start_lon = m_point_lon;
         obj->clear_selects();
         pt = new C_ar_point(obj);
         if (OVL_get_overlay_manager()->test_snap_to(map, point))
         {
            if (OVL_get_overlay_manager()->do_snap_to(map, point, snap_to_list))
            {
               CString tstr;

               if (!snap_to_list.IsEmpty())
               {
                  snap_to_info = snap_to_list.GetHead();
                  pt->move(snap_to_info);
                  // clear the list
                  while (!snap_to_list.IsEmpty())
                  {
                     snap_to_info = snap_to_list.RemoveHead();
                     delete snap_to_info;
                  }
               }
            }
            else
            {
               pt->m_lat = m_point_lat;
               pt->m_lon = m_point_lon;
            }
         }
         else
         {
            pt->m_lat = m_point_lat;
            pt->m_lon = m_point_lon;
         }

         ASSERT(GEO_valid_degrees(pt->m_lat, pt->m_lon));

         if (first_point)
            pt->add_usage("CP");
         else
            pt->add_usage("NC");
         first_point = FALSE;
         pt->m_type = obj->m_type;
         pt->m_icao = obj->m_icao;
         pt->m_id = obj->m_id;
         pt->m_direction = obj->m_direction;
         obj->m_point_list.AddTail(pt);
         m_cur_point = pt;
         ReleaseCapture();
         obj->m_selected = TRUE;
         set_modified(TRUE);
         m_drag_lock = FALSE;
      }
      break;

   case MM_AR_ANCHOR:
      {
         if (m_first_drag)
         {
            int pixels;
            map->get_pixels_around_world(&pixels);
            pixels = (pixels / 2) - 10;
            if (drag_utils::in_drag_threshold(m_start_pt, point) ||
               (!futil->is_point_in_client_area(view, point)))
            {
               CPoint tpt(-1000001, -1000001);
               m_first_pt = TRUE;
               m_cur_pt = m_start_pt;
               m_cur_lat = m_start_lat;
               m_cur_lon = m_start_lon;
               m_anchor_dragger->m_anchor_pt_lat = m_point_lat;
               m_anchor_dragger->m_anchor_pt_lon = m_point_lon;
               if (obj->m_point_list.GetCount() < 2)
               {
                  CRect rc(point.x -10, point.y - 10, point.x + 10, point.y + 10);
                  remove_route(obj);
                  OVL_get_overlay_manager()->InvalidateOverlay(this);
                  m_cur_obj = NULL;
               }

               break;
            }

            // prevent duplicate points
            if ((m_start_lat == m_point_lat) && (m_start_lon == m_point_lon))
            {
               m_originate = FALSE;
               return;
            }

            m_start_pt = point;

            end_drag(map, &dc, m_old_pt, point);
            m_anchor_dragger = NULL;

            m_originate = FALSE;

            obj->m_anchor_pt_lat = obj->m_anchor_point[1].m_lat;
            obj->m_anchor_pt_lon = obj->m_anchor_point[1].m_lon;

            int ax, ay, rslt;
            double dist, ang, distx, disty;

            ax = obj->m_anchor_pt[0].x;
            ay = obj->m_anchor_pt[0].y;


            GEO_geo_to_distance(m_start_lat, m_start_lon, m_point_lat, m_point_lon, &dist, &ang);

            obj->m_angle = ang;

            futil->pixels_to_km(map, ax, ay, point.x, point.y, &distx, &disty);
            obj->m_anchor_length = sqrt((distx*distx) + (disty*disty)) / 1.852;
            if (obj->m_anchor_length <= obj->m_anchor_width)
            {
               AfxMessageBox("The Orbit length must be greater than its width.");
               cancel_drag(map);
               m_originate = FALSE;
               m_in_poly_mode = FALSE;
               return;
            }

            obj->make_anchor(map);
            obj->clear_selects();
            ReleaseCapture();
            set_modified(TRUE);
            m_drag_lock = FALSE;
            obj->m_selected = TRUE;

            CUserArEditInfoDlg dlg;

            dlg.set_route_ptr(obj);
            rslt = dlg.DoModal();
            if (rslt != IDOK)
            {
               cancel_drag(map);
               m_originate = FALSE;
               m_first_drag = FALSE;
               end_draw();
               return;
            }
            obj->make_anchor(map);
         }
         m_first_drag = FALSE;
         end_draw();
         obj->m_visible = TRUE;
      }
      break;

   case MM_AR_NAVAID:
      {
         SnapToInfo* snap_to_info = NULL;
         CList<SnapToInfo *, SnapToInfo *> snap_to_list;
         // get lat/lon and screen coords
         if (OVL_get_overlay_manager()->test_snap_to(map, point))
         {
            if (OVL_get_overlay_manager()->test_snap_to(map, point))
            {
               POSITION next;
               if (OVL_get_overlay_manager()->do_snap_to(map, point, snap_to_list))
               {
                  if (!snap_to_list.IsEmpty())
                  {
                     next = snap_to_list.GetHeadPosition();
                     while (next != NULL)
                     {
                        snap_to_info = snap_to_list.GetNext(next);
                        if (snap_to_info->m_pnt_src_rs == SnapToInfo::NAVAID)
                        {
                           C_ar_point* pt;
                           POSITION pos;
                           CString tstr;

                           if (m_navaid_vertex_num >= 0)
                           {
                              if (!m_navaid_vertex_anchor)
                              {
                                 pos = obj->m_point_list.FindIndex(m_navaid_vertex_num);
                                 if (pos == NULL)
                                    break;
                                 pt = obj->m_point_list.GetAt(pos);
                              }
                              else
                                 pt = &(obj->m_anchor_point[m_navaid_vertex_num]);
                              pt->set_crossref(snap_to_info);
                              set_modified(TRUE);
                              // todo: find all other points with the same location and set their navaid and fix
                           }
                           next = NULL;
                        }
                     }
                     // clear the list
                     while (!snap_to_list.IsEmpty())
                     {
                        snap_to_info = snap_to_list.RemoveHead();
                        delete snap_to_info;
                     }
                  }
               }
            }
         }
      }
      break;

   }  // switch

   OVL_get_overlay_manager()->InvalidateOverlay(this);

   m_cur_pt.x = -1000001;

   if (m_bTempSelect)
   {
      HCURSOR cursor;
      set_temp_select(FALSE, &cursor);
   }

   if (!m_in_poly_mode || (m_nMode != MM_AR_TRACK))
      end_draw();

   m_originate = FALSE;

}

void C_ar_edit::drop_select(ViewMapProj* map, UINT nFlags, CPoint point)
{
   CWindowDC dc(map->get_CView());

   if (!m_bGotPt)
      return;

   if (!valid_object(m_cur_obj))
      ASSERT(0);

   switch(m_nMode)
   {
   case MM_AR_SELECT:
   case MM_AR_TRACK:
   case MM_AR_ANCHOR:
      {
         drop_line(m_cur_obj, map, &dc, nFlags, point);
      }
      break;
   default:
      return;
   }
   m_bGotPt = FALSE;
   m_bGotHandle = FALSE;
   m_cur_pt.x = -1000001;
   m_handle = 0;
   ReleaseCapture();
}

BOOL C_ar_edit::drop_line(C_ar_obj* obj, ViewMapProj* map, CDC* dc, UINT nFlags, CPoint point)
{
   C_ar_point* vtx = NULL;
   POINT newpt;
   CFvwUtil *futil = CFvwUtil::get_instance();
   BOOL fix_seg = TRUE;
   SnapToInfo* snap_to_info = NULL;
   CList<SnapToInfo *, SnapToInfo *> snap_to_list;
   BOOL good_snapto = FALSE;

   if (!valid_object(obj))
      ASSERT(0);

   if (!m_bGotPt)
      return FALSE;

   CWnd *pWnd = dc->GetWindow();

   if (objtype == MM_AR_ANCHOR)
   {
      if (!drag_utils::in_drag_threshold(m_start_pt, point) &&
         (futil->is_point_in_client_area(pWnd, point)))
      {
         if (m_anchor_ext_pt)
         {
            double dist, ang;

            if (m_draw_icon)
               m_draw_icon->end_drag(map, point, tmpobj);
            if (OVL_get_overlay_manager()->test_snap_to(map, point))
            {
               if (OVL_get_overlay_manager()->do_snap_to(map, point, snap_to_list))
               {
                  if (!snap_to_list.IsEmpty())
                  {
                     snap_to_info = snap_to_list.GetHead();
                     m_anchor_ext_point->move(snap_to_info);
                     good_snapto = TRUE;
                     // clear the list
                     while (!snap_to_list.IsEmpty())
                     {
                        snap_to_info = snap_to_list.RemoveHead();
                        delete snap_to_info;
                     }
                  }
               }
               else
               {
                  m_anchor_ext_point->m_lat = m_point_lat;
                  m_anchor_ext_point->m_lon = m_point_lon;
               }
            }
            else
            {
               m_anchor_ext_point->m_lat = m_point_lat;
               m_anchor_ext_point->m_lon = m_point_lon;
            }

            GEO_geo_to_distance(obj->m_anchor_pt_lat, obj->m_anchor_pt_lon, m_point_lat, m_point_lon, &dist, &ang);
            m_anchor_ext_point->m_dist = dist / 1000.0;
            m_anchor_ext_point->m_angle = ang - obj->m_angle;
         }
         else
         {
            if (m_drag_all)
               end_drag_all(map, dc, m_old_pt, point);
            else
               end_drag(map, dc, m_old_pt, point);
            obj->make_anchor(map);
            obj->m_selected = TRUE;
            m_anchor_dragger = NULL;
         }
         set_modified(TRUE);
      }

      return TRUE;
   }

   int pixels;
   map->get_pixels_around_world(&pixels);
   pixels = (pixels / 2) - 10;

   newpt.x = -1000001;

   if (m_cur_vertex_num >= 0)
   {
      if (objtype != MM_AR_ANCHOR)
         obj->rubber_band_point(map, dc, m_old_pt, newpt, m_cur_vertex_num);
      else
         if (m_draw_icon)
            m_draw_icon->end_drag(map, point, tmpobj);

      if (!drag_utils::in_drag_threshold(m_start_pt, point) &&
         (futil->is_point_in_client_area(pWnd, point)))
      {
         double lat, lon;
         int x1, x2;
         x1 = -1;
         x2 = -1;
         if (OVL_get_overlay_manager()->test_snap_to(map, point))
         {
            if (OVL_get_overlay_manager()->do_snap_to(map, point, snap_to_list))
            {
               if (!snap_to_list.IsEmpty())
               {
                  C_ar_point *tpt;
                  snap_to_info = snap_to_list.GetHead();
                  tpt = obj->get_vertex(m_cur_vertex_num-1);
                  if (tpt != NULL)
                  {
                     if (objtype == MM_AR_NAVAID)
                        tpt->set_crossref(snap_to_info);
                     else
                        tpt->move(snap_to_info);
                     good_snapto = TRUE;
                  }
                  // clear the list
                  while (!snap_to_list.IsEmpty())
                  {
                     snap_to_info = snap_to_list.RemoveHead();
                     delete snap_to_info;
                  }
               }
            }
            else
            {
               map->surface_to_geo(point.x, point.y, &lat, &lon);
               obj->move_vertex(m_cur_vertex_num, lat, lon);
            }
         }
         else
         {
            map->surface_to_geo(point.x, point.y, &lat, &lon);
            obj->move_vertex(m_cur_vertex_num, lat, lon);
         }

         set_modified(TRUE);
      }
      // clear the navaid info
      vtx = obj->get_vertex(m_cur_vertex_num-1);
      if ((vtx != NULL) && !good_snapto)
      {
         vtx->m_nav_flag = "";
         vtx->m_nav_id = "";
         vtx->m_nav_type = "";
         vtx->m_nav_country = "";
         vtx->m_nav_key = "";
         vtx->m_fix = "";
         // add control point is no points exist
         if (vtx->m_usage.IsEmpty())
            vtx->add_usage("NC");
      }

      obj->select_point(m_cur_vertex_num, TRUE); 
   }

   if (obj->m_type != 'T')
      m_in_poly_mode = FALSE;

   if (!m_in_poly_mode)
      end_draw();

   set_modified(TRUE);
   return TRUE;
}

void C_ar_edit::end_draw()
{
   m_in_poly_mode = FALSE;
   m_bGotPt = FALSE;
   m_cur_vertex_num = -1;
   m_cur_obj = NULL;
}

void C_ar_edit::set_temp_select(BOOL select, HCURSOR* cursor)
{
   *cursor = NULL;
   if (select)
   {
      if (!m_bTempSelect)
      {
         if (m_nMode != MM_AR_SELECT)
         {
            if  (!m_bGotPt)
            {
               m_nTempMode = m_nMode;
               m_pTempObj = m_cur_obj;
               m_temp_start_pt = m_start_pt;
               m_nMode = MM_AR_SELECT;
               m_bTempSelect = TRUE;
               *cursor = get_default_cursor();
               if (!valid_object(m_cur_obj))
                  ASSERT(0);
            }
         }
      }
   }
   else
   {
      if (m_bTempSelect)
      {
         if (!m_bGotPt)
         {
            m_nMode = m_nTempMode;
            m_cur_obj = m_pTempObj;
            m_start_pt = m_temp_start_pt;
            *cursor = get_default_cursor();
            if (!valid_object(m_cur_obj))
               ASSERT(0);
         }
         m_bTempSelect = FALSE;
      }
   }
}
