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

#include "stdafx.h"
#include "LayerOvl.h"
#include "..\Common\map_server_client.h"
#include "mainfrm.h"
#include "..\Common\ComErrorObject.h" 
#include "ovlElementContainer.h"
#include "mapview.h"
#include "showrmk.h"
#include "Map.h"
#include "CCallback.h"
#include "CCallback2.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "SnapTo.h"
#include "PlaybackDialog\viewtime.h"
#include "StatusBarManager.h"
#include "OvlTerrainMask.h"
#include "ovl_mgr.h"
#include "FalconView/include/mapx.h"
#include "FalconView/UIThreadOperation.h"

// CBaseLayerOvl
//

//temporary hack for viewshed, 19 dec 2013 
#include "getobjpr.h"
#include "GeospatialViewController.h"

CBaseLayerOvl::CBaseLayerOvl(const DWORD owning_thread_id,
   C_overlay *overlay) :
   m_bDragging(FALSE),
   m_hide_layer(FALSE),
   m_owning_thread_id(owning_thread_id),
   m_pOverlay(overlay),
   m_threshold(WORLD),
   m_view_time_enabled(FALSE)
{
}

CBaseLayerOvl::~CBaseLayerOvl()
{
}

// default constructor
CLayerOvl::CLayerOvl() :
   CBaseLayerOvl(::GetCurrentThreadId(), this),
   CFvOverlayPersistenceImpl("")
{
}

// constructor #1
CLayerOvl::CLayerOvl(const DWORD owning_thread_id, CString name,
   IDispatch *pdisp, CString icon_name,
   LayerEditorImpl *layer_editor /*= NULL*/) :
   CBaseLayerOvl(owning_thread_id, this),
   CFvOverlayPersistenceImpl(name)
{
   Setup(owning_thread_id, name, pdisp, icon_name, layer_editor);
}

// constructor #2
CLayerOvl::CLayerOvl(const DWORD owning_thread_id, CString name,
   IDispatch *pdisp) :
   CBaseLayerOvl(owning_thread_id, this),
   CFvOverlayPersistenceImpl(name)
{
   Setup(owning_thread_id, name, pdisp, "", NULL);
}

void CLayerOvl::Setup(const DWORD owning_thread_id, CString name,
   IDispatch *pdisp, CString icon_name, LayerEditorImpl *layer_editor)
{
   SetOwningThreadId(owning_thread_id);
   m_fileSpecification = name;
   m_pdisp = pdisp;
   m_icon_name = icon_name;
   m_layer_editor = layer_editor;
}

// destructor
CLayerOvl::~CLayerOvl()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

void CLayerOvl::Finalize()
{
   // remove the info box if this overlay has its focus
   if (this == CRemarkDisplay::get_focus_overlay())
      CRemarkDisplay::close_dlg();

   // since this overlay is a time-sensitive overlay we need to update
   // the view time dialog if it is opened
   if (get_view_time_enabled())
      CMainFrame::GetPlaybackDialog().update_interval();
}

// couldn't find the given object in the object map// return the object id of the given object.  Returns -1 if the object 
// was not found in the object map
int CBaseLayerOvl::get_object_id(OvlElement *object)
{
   return m_pOverlay->GetOvlElementContainer()->get_object_id(object);
}

// IFvOverlayTypeOverrides
//

// Override the display name defined in the overlay's factory.  This is used to support
// per-instance overlay display names (e.g., an overlay created via the ILayer::CreateLayer method)
HRESULT CBaseLayerOvl::get_m_displayName(BSTR *pVal)
{
   if (m_displayName.IsEmpty())
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(m_pOverlay->get_m_overlayDescGuid());
      *pVal = _bstr_t(pOverlayTypeDesc->displayName).Detach();
   }
   else
      *pVal = _bstr_t(m_displayName).Detach();

   return S_OK;
}

// This relative icon will override the icon defined in the overlay's factory.  This
// is used to support per-instance overlay icons (e.g., an overlay created via the
// ILayer::CreateLayerEx method).  This icon is displayed in the overlay manager's
// current list of opened overlays.  The filename can either be the full specification
// or a path relative to HD_DATA\icons.
HRESULT CBaseLayerOvl::get_m_iconFilename(BSTR* pVal)
{
   *pVal = _bstr_t(m_icon_name).Detach();
   return S_OK;
}

// draw the layer's OvlElements to this DC
int CBaseLayerOvl::draw(ActiveMap* map)
{
   const DWORD thread_id = ::GetCurrentThreadId();
   if (thread_id == m_owning_thread_id)
   {
      const int layer_handle =
         OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

      LayerEditorImpl *pLayerEditor = get_ILayerEditor();

      // Give the client a chance to draw before any layer objects are drawn
      if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
      {
         try
         {
            const long bDrawBeforeLayerObjects = TRUE;
            pLayerEditor->OnDraw(layer_handle, map->m_interface_ptr,
               bDrawBeforeLayerObjects);
         }
         catch(_com_error &)
         {
         }
      }

      // draw the layer objects
      map_projection_utils::CMapProjWrapper mapProjWrapper(
         map->m_interface_ptr);
      gfx::GraphicsContextWrapper gcWrapper(map->m_interface_ptr);
      draw(&mapProjWrapper, &gcWrapper);

      // Give the client a chance to draw after all layer objects have been
      // drawn
      if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
      {
         try
         {
            const long bDrawBeforeLayerObjects = FALSE;
            pLayerEditor->OnDraw(layer_handle, map->m_interface_ptr,
               bDrawBeforeLayerObjects);
         }
         catch(_com_error &)
         {
         }
      }
      m_pOverlay->set_valid(TRUE); 
      return SUCCESS;
   }
   else
   {
      int result;
      auto on_draw = [&]()
      {
         result = draw(map);
      };
      bool wait_for_operation = true;
      new fvw::UIThreadOperation(std::move(on_draw), wait_for_operation);
      return result;
   }
}

int CLayerOvl::draw(ActiveMap* map)
{
   return CBaseLayerOvl::draw(map);
}


int CLayerOvl::OnDrawToBaseMap(IActiveMapProj *map)
{
   ActiveMap_TMP active_map;
   SetupActiveMap(map, active_map);
   const int ret = draw_to_base_map(&active_map);

   return ret;
}

int CLayerOvl::draw_to_base_map(ActiveMap* map)
{
   return CBaseLayerOvl::draw_to_base_map(map);
}

// Derived classes can implement this method if they need to
// draw to the base map before the overlays are drawn with a call
// to draw().  This is used, for example, in the shadow overlay to 
// modify the shades of the base map.  The base map in the dc will be 
// a north-up equal arc map (before rotatation and before projection).
int CBaseLayerOvl::draw_to_base_map(ActiveMap* map)
{
   LayerEditorImpl *pLayerEditor = get_ILayerEditor();

   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

      try
      {
         pLayerEditor->OnDrawToBaseMap(layer_handle, map->m_interface_ptr);
      }
      catch(_com_error &)
      {
      }
   }

   return SUCCESS;
}

BOOL CLayerOvl::can_add_pixmaps_to_base_map() 
{
   return CBaseLayerOvl::can_add_pixmaps_to_base_map();
}

// Each overlay is given a chance to modify the base map in draw_to_base_map.  They
// can either add new alpha blended pixmaps via IGraphicContext's PutPixmapAlpha or 
// they can add a pixmap via PutPixmap.  If this overlay will be drawing pixmaps
// to the base map with PutPixmap the rendering engine has to be setup to do this.  Return
// TRUE if your overlay will call IGraphicsContext::PutPixmap in draw_to_base_map
BOOL CBaseLayerOvl::can_add_pixmaps_to_base_map() 
{ 
   LayerEditorImpl *pLayerEditor = get_ILayerEditor();

   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      try
      {
         long bCanAddPixmapsToBaseMap = 0;
         if (SUCCEEDED(pLayerEditor->CanAddPixmapsToBaseMap(&bCanAddPixmapsToBaseMap)))
         {
            return bCanAddPixmapsToBaseMap;
         }
      }
      catch(_com_error &)
      {
      }
   }

   return FALSE; 
}

int CBaseLayerOvl::draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   if (m_hide_layer)
      return SUCCESS;

   if (m_pOverlay->get_valid())
      return redraw(pMap, gc);
   if (pMap->scale() >= map_scale_utils::MapScale(m_threshold.GetScale(), m_threshold.GetScaleUnits()))
   {
      return m_pOverlay->GetOvlElementContainer()->draw(pMap, gc);
   }
   return SUCCESS;
}

// redraw the layer's OvlElements to this DC
int CBaseLayerOvl::redraw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   if (m_hide_layer)
      return SUCCESS;

   if (pMap->scale() >= map_scale_utils::MapScale(m_threshold.GetScale(), m_threshold.GetScaleUnits()))
   {
      return m_pOverlay->GetOvlElementContainer()->redraw(pMap, gc);
   }
   return SUCCESS;
}

int CLayerOvl::redraw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   return CBaseLayerOvl::redraw(pMap, gc);
}

int CLayerOvl::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   return CBaseLayerOvl::on_mouse_moved(pMapView, point, flags);
}

// Sets the cursor and icon (help text and cursor) to indicate what will
// happen if the selected member is called with the same parameters.  
// Returns SUCCESS whenever the selected member would have returned SUCCESS,
// FAILURE otherwise.  Use default implementation to do nothing.
//
// The cursor parameter is used to overide the default cursor (see the
// C_ovl_mgr::get_default_cursor function).  Setting it to NULL indicates
// you do not want to overide the default.
//
// The hint parameter is used to overide the default hint (see the
// C_ovl_mgr::get_default_hint function).  Setting it to NULL indicates you
// do not want to overide the default.
int CBaseLayerOvl::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   if (m_bDragging)
   {
      HCURSOR cursor = NULL;
      HintText *hintText = NULL;

      drag(OVL_get_overlay_manager()->get_view_map(), point, flags, &cursor, &hintText);

      if (cursor != NULL)
         pMapView->SetCursor(cursor);
      if (hintText != NULL)
      {
         pMapView->SetTooltipText(_bstr_t(hintText->get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hintText->get_help_text()));
      }

      return SUCCESS;
   }

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      _bstr_t bstrTooltip, bstrHelpText;
      long tmp_cursor = 0;
      long lReturn = FAILURE;

      ISettableMapProjPtr spSettableMapProj = pMapView->m_pCrntMapProj();
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));
      const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

      try
      {
         pLayerEditor->TestSelectedEx(layer_handle, spSettableMapProj, hDC,
            point.x, point.y, flags, &tmp_cursor, bstrTooltip.GetAddress(), bstrHelpText.GetAddress(), &lReturn);
      }
      catch(_com_error &)
      {
      }

      if (lReturn == SUCCESS)
      {
         HCURSOR hCursor = reinterpret_cast<HCURSOR>(tmp_cursor);
         if (hCursor != NULL)
            pMapView->SetCursor(hCursor);

         m_hint.set_tool_tip((char *)bstrTooltip);
         m_hint.set_help_text((char *)bstrHelpText);

         pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));

         return SUCCESS;
      }
   }


   OvlElement *element = m_pOverlay->GetOvlElementContainer()->hit_test(point);

   // get the lat/lon location of the point that was clicked
   d_geo_t location;
   int result;
   pMapView->m_pCrntMapProj()->surface_to_geo(point.x, point.y, &location.lat, &location.lon, &result);

   int object_handle;
   if (NULL == element)
      object_handle = -1;
   else
      object_handle = m_pOverlay->GetOvlElementContainer()->get_object_id(element);

   if (pLayerEditor)
   {
      _bstr_t tooltip;
      _bstr_t helptext;
      long tmp_cursor = 0;
      long ret = FAILURE;
      try
      {
         pLayerEditor->TestSelected(layer_handle, 
            object_handle, location.lat, location.lon, (long)flags, 
            &tmp_cursor, tooltip.GetAddress(), helptext.GetAddress(), &ret);

         HCURSOR hCursor = reinterpret_cast<HCURSOR>(tmp_cursor);
         if (hCursor != NULL)
            pMapView->SetCursor(hCursor);
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::TestSelected failed");
      }

      pMapView->SetTooltipText(tooltip);
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(helptext);

      return ret;
   }

   if (element == NULL)
   {
      return FAILURE;
   }
   else
   {
      // set cursor to select cursor
      pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

      pMapView->SetTooltipText(_bstr_t(get_tool_tip(element)));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(get_help_text(element)));
   }

   return SUCCESS;
}

int CLayerOvl::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   return CBaseLayerOvl::on_left_mouse_button_up(pMapView, point, nFlags);
}

int CBaseLayerOvl::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_bDragging)
   {
      drop(OVL_get_overlay_manager()->get_view_map(), point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}

int CLayerOvl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   return CBaseLayerOvl::selected(pMapView, point, flags);
}

// Performs an object specific function when the object is selected.  Based
// on context, i.e., the state or mode of the application as well as this
// overlay, the overlay will determine what action, if any, will take place.
// This function will return SUCCESS to indicate that this overlay processed 
// the selection, so no other overlay will process this selection.  If 
// selected() returns drag == TRUE then the drag() member of the overlay 
// will be called each time a drag occurs, until a cancel-drag or a drop 
// occurs.  The cancel_drag() member is called when a cancel-drag occurs.
// The drop() member is called when a drop occurs.  The meaning of the terms
// drag and drop are overlay specific.
//
// The cursor parameter is used to overide the default cursor (see the
// C_ovl_mgr::get_default_cursor function).  Setting it to NULL indicates
// you do not want to overide the default.
//
// The hint parameter is used to overide the default hint (see the
// C_ovl_mgr::get_default_hint function).  Setting it to NULL indicates you
// do not want to overide the default.
int CBaseLayerOvl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      _bstr_t bstrTooltip, bstrHelpText;
      long tmp_cursor = 0;
      short tmp_drag;
      long lReturn = FAILURE;

      ISettableMapProjPtr pSettableMapProj = pMapView->m_pCrntMapProj();
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));
      const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

      try
      {
         pLayerEditor->SelectedEx(layer_handle, pSettableMapProj, hDC, point.x, point.y, flags,
            &tmp_drag, &tmp_cursor, bstrTooltip.GetAddress(), bstrHelpText.GetAddress(), &lReturn);
      }
      catch(_com_error &)
      {
      }

      if (lReturn == SUCCESS)
      {
         m_bDragging = tmp_drag;

         return SUCCESS;
      }
   }

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   OvlElement *element = m_pOverlay->GetOvlElementContainer()->hit_test(point);

   // get the lat/lon location of the point that was clicked
   d_geo_t location;
   mapProjWrapper.surface_to_geo(point.x, point.y, &location.lat, &location.lon);

   // if the editor is active we will call the ILayerEditor's method Selected
   int object_handle;
   if (NULL == element)
      object_handle = -1;
   else
      object_handle = m_pOverlay->GetOvlElementContainer()->get_object_id(element);

   if (pLayerEditor != NULL)
   {
      _bstr_t tooltip, helptext;
      long tmp_cursor = 0;
      short tmp_drag;
      long ret;

      try
      {
         if (fvw_get_view()->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
         {  
            CLSID CLSID_RangeBearing = {0x5F11B693,0x349F,0x4F09,0xAD,0x7F,0xCD,0xD6,0x6D,0xC4,0x15,0x56};
            //CLSID_RangeBearing.Data1 = 0x5F11B693;
            //CLSID_RangeBearing.Data2 = 0x349F;
            //CLSID_RangeBearing.Data3 = 0x4F09;
            //CLSID_RangeBearing.Data4 = 0x03a2f014;
            if (pLayerEditor->GetClassId() == CLSID_RangeBearing)
            {
               if (GetKeyState(VK_CONTROL) < 0)
               {
                  fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->SetViewshedEnabledValue(1.0f);
               }
               else
                  fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->SetViewshedEnabledValue(0.0f);
               return S_OK;
            }
            else
            {
               return S_OK;
            }
         }
         pLayerEditor->Selected(layer_handle, object_handle, location.lat,
            location.lon, (long)flags, &tmp_drag, &tmp_cursor, tooltip.GetAddress(), helptext.GetAddress(), &ret);

         if (ret == SUCCESS)
            m_bDragging = tmp_drag;
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::Selected failed");
         ret = FAILURE;
      }

      m_drag_object_id = object_handle;

      return ret;
   }

   // if client wishes to do something for OnSelected let them do it now.
   // Otherwise, try calling the client's GetInfoText method and pop up the
   // info text dialog
   if (on_selected(element, location) == FAILURE && element != NULL)
   {
      return info_text_dialog(element);
   }

   return (NULL == element) ? FAILURE : SUCCESS;
}

bool CLayerOvl::is_selection_locked()
{
   return CBaseLayerOvl::is_selection_locked();
}

// This function allows an overlay to temporarily lock the selection so that only it
// will process mouse operations. Specifically, when "Display Airways in Region" or
// "Display Military Training Routes in Region" is selected from the right-click menu,
// they need to have exclusive mouse control until the operation is completed. The
// default implementation is that selection locking is disabled. An overlay can lock
// selection by overriding this function and returning true when exclusive mouse 
// control is required. Returns true if selection locking is enabled or false if 
// selection locking is disabled.
bool CBaseLayerOvl::is_selection_locked()
{
   long layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      try
      {
         long lSelectionLocked;
         pLayerEditor->IsSelectionLocked(layer_handle, &lSelectionLocked);

         return lSelectionLocked != 0;
      }
      catch(_com_error &)
      {
      }
   }

   return false;  // selection locking disabled
}


int CBaseLayerOvl::on_selected(OvlElement *element, d_geo_t location)
{
   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return FAILURE;
   }

   long layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);
   long object_handle;
   if (NULL == element)
      object_handle = -1;
   else
      object_handle = get_object_id(element);
   HWND hWnd = AfxGetMainWnd()->m_hWnd;

   // Retrieve the dispatch identifier for the OnSelected method.
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         return callback->OnSelected(layer_handle,
            object_handle,
            (long)hWnd,
            location.lat,location.lon);
      }

      CCallback callbackImpl(m_pdisp);
      return callbackImpl.OnSelected(layer_handle,
         object_handle,
         (long)hWnd,
         location.lat,location.lon);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnSelected failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("OnSelected failed", *e);
      e->Delete();
   }

   return FAILURE;
}

int CBaseLayerOvl::info_text_dialog(OvlElement *element)
{
   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return FAILURE;
   }

   long layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);
   long object_handle = m_pOverlay->GetOvlElementContainer()->get_object_id(element);

   HWND hWnd = AfxGetMainWnd()->m_hWnd;

   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      long result;
      _bstr_t comment_txt, dialog_title_txt;
      if (callback != NULL)
      {
         result = callback->GetInfoText(
            (long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            object_handle,
            dialog_title_txt.GetAddress(),
            comment_txt.GetAddress());
      }
      else
      {
         CCallback callbackImpl(m_pdisp);
         result = callbackImpl.GetInfoText((long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            object_handle,
            dialog_title_txt.GetAddress(),
            comment_txt.GetAddress());
      }

      // display the dialog
      CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, (char *)comment_txt, (char *)dialog_title_txt, m_pOverlay);

      return result;
   }
   catch (_com_error &e) 
   {
      CString msg;
      msg.Format("GetInfoText failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("GetInfoText failed", *e);
      e->Delete();
   }

   return FAILURE;
}

boolean_t CLayerOvl::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
   UINT nFlags, CPoint point)
{
   return CBaseLayerOvl::offer_keydown(pMapView, nChar, nRepCnt, nFlags, point);
}

// Receives all keydown messeges from the Overlay Manager and filters them 
// for this overlay... a return of false will allow the keystroke to be 
// processed normally in MapView. This function will get called only if 
// the File Overlay is current and open in its editor.
boolean_t CBaseLayerOvl::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
   UINT nFlags, CPoint point)
{
   if (nChar == VK_ESCAPE && m_bDragging == TRUE)
   {
      cancel_drag(OVL_get_overlay_manager()->get_view_map());
      return TRUE;
   }

   if (get_ILayerEditor() != NULL)
   {
      const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);
      long ret;

      try
      {
         get_ILayerEditor()->OnKeyDown(layer_handle, (long)nChar, (long)nFlags,
            &ret);
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::OnKeyDown failed");
         return FALSE;
      }

      return (ret == SUCCESS) ? TRUE : FALSE;
   }

   return FALSE;
}

boolean_t CLayerOvl::offer_keyup(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
   UINT nFlags, CPoint point)
{
   return CBaseLayerOvl::offer_keyup(pMapView, nChar, nRepCnt, nFlags, point);
}

// Receives all keyup messeges from the Overlay Manager and filters them 
// for this overlay... a return of false will allow the keystroke to be 
// processed normally in MapView. This function will get called only if 
// the File Overlay is current and open in its editor.
boolean_t CBaseLayerOvl::offer_keyup(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
   UINT nFlags, CPoint point)
{
   if (get_ILayerEditor() != NULL)
   {
      const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);
      long ret;

      try
      {
         get_ILayerEditor()->OnKeyUp(layer_handle, (long)nChar, (long)nFlags,
            &ret);
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::OnKeyUp failed");
         return FALSE;
      }

      return (ret == SUCCESS) ? TRUE : FALSE;
   }

   return FALSE;
}

// get the tool tip of the given element
CString CBaseLayerOvl::get_tool_tip(OvlElement *element)
{
   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return "";
   }

   long object_handle = m_pOverlay->GetOvlElementContainer()->get_object_id(element);
   if (object_handle == -1)
      return "";

   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      _bstr_t tool_tip_txt;
      if (callback != NULL)
      {
         callback->GetToolTip( (long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            object_handle,
            tool_tip_txt.GetAddress());
      }
      else
      {
         CCallback callbackImpl(m_pdisp);
         callbackImpl.GetToolTip((long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            object_handle,
            tool_tip_txt.GetAddress());
      }

      return CString((char *)tool_tip_txt);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("GetToolTip failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("GetToolTip failed", *e);
      e->Delete();
   }

   return "";
}

// get the help text of the given element
CString CBaseLayerOvl::get_help_text(OvlElement *element)
{
   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return "";
   }

   long object_handle = m_pOverlay->GetOvlElementContainer()->get_object_id(element);
   if (object_handle == -1)
      return "";

   _bstr_t tool_tip_txt;

   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->GetHelpText((long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            object_handle,
            tool_tip_txt.GetAddress());
      }
      else
      {
         CCallback callbackImpl(m_pdisp);
         callbackImpl.GetHelpText((long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            object_handle,
            tool_tip_txt.GetAddress());
      }

      return CString((char *)tool_tip_txt);
   }
   catch (_com_error &e)
   {
      CString msg;
      msg.Format("GetHelpText failed - %s", (char *)e.Description());
      ERR_report(msg);
      return "";
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("GetHelpText failed", *e);
      e->Delete();
   }

   return "";
}




// return a pointer to the object that is beneath the given point or NULL 
// if no object is underneath the given point
OvlElement *CBaseLayerOvl::hit_test(CPoint point)
{
   return m_pOverlay->GetOvlElementContainer()->hit_test(point);
}  

// Note: This appears to be dead code

//C_icon *CLayerOvl::hit_test(MapProj* map, CPoint point)
//{
//   return CBaseLayerOvl::hit_test(map, point);
//}
//
//// determine if the point hits the object's
//C_icon *CBaseLayerOvl::hit_test(MapProj* map, CPoint point)
//{
//OvlElement *element = hit_test(point);
//
//if (element && object_above_threshold(map, element))
//return m_cicon;
//
//return NULL;
//}

// Returns the beginning and ending times for the overlay's time segment.  If the time segment is valid 
// set pTimeSegmentValue to a non-zero integer, otherwise set it to zero.  An example where the time segment
// would be invalid is an uncalculated route.  In this case, the overlay is not accounted for in the overall
// clock time and does not show up in the playback dialog's gannt chart
HRESULT CBaseLayerOvl::GetTimeSegment( DATE *pBegin, DATE *pEnd, long *pTimeSegmentValid)
{
   COleDateTime begin, end;
   *pTimeSegmentValid = get_view_time_span(begin, end);
   *pBegin = begin;
   *pEnd = end;

   return S_OK;
}

// gets the beginning and ending times for this overlay
// returns TRUE if the begining and end time are valid
boolean_t CBaseLayerOvl::get_view_time_span(COleDateTime& begin, COleDateTime& end)
{
   if (!m_view_time_enabled)
      return FALSE;

   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return FALSE;
   }

   try
   {
      DATE date_begin=0;
      DATE date_end=0;
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->GetTimeSpan((long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            &date_begin,
            &date_end);
      }
      else
      {
         CCallback callbackImpl(m_pdisp);
         callbackImpl.GetTimeSpan((long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            &date_begin,
            &date_end);
      }
      begin = COleDateTime(date_begin);
      end = COleDateTime(date_end);
      return TRUE;
   }
   catch (_com_error &e)
   {
      CString msg;
      msg.Format("GetTimeSpan failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("GetTimeSpan failed", *e);
      e->Delete();
   }

   return FALSE;
}

// tell this overlay the current view time
static int in_SetCurrentViewTime = 0;
HRESULT CBaseLayerOvl::OnPlaybackTimeChanged(DATE dateCurrentTime)
{
   if (!m_view_time_enabled)
      return S_OK;

   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return S_OK;
   }

   // do not allow re-entry, otherwise we will get into a deadlock
   if (in_SetCurrentViewTime)
      return S_OK;

   in_SetCurrentViewTime = 1;

   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->SetCurrentViewTime((long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            dateCurrentTime);
      }
      else
      {
         CCallback callbackImpl(m_pdisp);
         callbackImpl.SetCurrentViewTime((long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay),
            dateCurrentTime);
      }
   }
   catch (_com_error &e)
   {
      CString msg;
      msg.Format("SetCurrentViewTime failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("SetCurrentViewTime failed", *e);
      e->Delete();
   }

   in_SetCurrentViewTime = 0;
   return S_OK;
}

int CBaseLayerOvl::AddMenuItemsFromString(CString strMenuItems, CList<CFVMenuNode *, CFVMenuNode *> &listMenuNodes,
   int nObjectHandle)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // parse the menu text received and place into the text_list;
   CList<CString *, CString *> listMenuItems;
   futil->parse_the_text(strMenuItems, listMenuItems);

   // make sure we have an even number of entries in the menu string (one for the
   // label and one for the callback method's name
   if (listMenuItems.GetCount() % 2 != 0)
   {
      while (!listMenuItems.IsEmpty())
         delete listMenuItems.RemoveTail();

      return 0;
   }

   // add each of the labels/methods to the menu
   int nNumAdded = 0;
   POSITION position = listMenuItems.GetHeadPosition();
   while (position)
   {
      CString strLabel = *listMenuItems.GetNext(position);
      CString strMethodName = *listMenuItems.GetNext(position);

      // A colon in the method name denotes an additional string passed
      // to the caller
      int pos = strMethodName.Find(":");
      if (pos != -1)
      {
         CString strMethod = strMethodName.Left(pos);
         CString strMenuItemData = strMethodName.Right(strMethodName.GetLength() - pos - 1);

         listMenuNodes.AddTail(new CCallbackContextMenuItem(strLabel, m_pdisp, strMethod,
            OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay), nObjectHandle, strMenuItemData));
      }
      else
      {
         listMenuNodes.AddTail(new CCallbackContextMenuItem(strLabel, m_pdisp, strMethodName,
            OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay), nObjectHandle, ""));
      }

      nNumAdded++;
   }

   while (!listMenuItems.IsEmpty())
      delete listMenuItems.RemoveTail();

   return nNumAdded;
}

void CLayerOvl::menu(ViewMapProj* map, CPoint point, 
   CList<CFVMenuNode *, CFVMenuNode *> &list)
{
   CBaseLayerOvl::menu(map, point, list);
}

// add menu items to the popup menu based on context
void CBaseLayerOvl::menu(ViewMapProj* map, CPoint point, 
   CList<CFVMenuNode *, CFVMenuNode *> &list)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      ISettableMapProj *pSettableMapProj = map->GetSettableMapProj();
      CClientDC dc(map->get_CView());
      const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

      try
      {
         _bstr_t bstrMenuItems;
         pLayerEditor->OnAddMenuItems(layer_handle, pSettableMapProj, hDC, point.x, point.y, bstrMenuItems.GetAddress());
         AddMenuItemsFromString((char *)bstrMenuItems, list, -1);
      }
      catch(_com_error &)
      {
      }
   }

   static int menu_id = 0;

   menu_id++;

   POSITION position = m_pOverlay->GetOvlElementContainer()->GetTailPosition();
   int num_objects_on_menu = 0;
   int found_hit = 0;
   while (position && num_objects_on_menu < 19)
   {
      OvlElement *element = m_pOverlay->GetOvlElementContainer()->GetPrevElement(position);
      if (element->hit_test(point))
      {
         found_hit = 1;

         // get the object handle associated with the given element
         const int object_handle = get_object_id(element);

         // give the client a chance to add menu items for this object
         const CString menu = get_menu_items_from_client(element, point, menu_id);

         AddMenuItemsFromString(menu, list, object_handle);
         num_objects_on_menu++;
      }
   }

   // if and OvlElement was not hit then pass in -1 to GetMenuItems
   if (!found_hit)
   {
      // give the client a chance to add menu items for this object
      const CString menu = get_menu_items_from_client(NULL, point, menu_id);
      AddMenuItemsFromString(menu, list, -1);
   }
}

CString CBaseLayerOvl::get_menu_items_from_client(OvlElement *element, CPoint point, long menu_id)
{
   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return "";
   }

   long object_handle = m_pOverlay->GetOvlElementContainer()->get_object_id(element);
   //if (object_handle == -1)
   //   return "";

   // convert cursor location to geo coords
   MapView* view;
   ViewMapProj* map;

   double lat, lon;

   view = (MapView *)UTL_get_active_non_printing_view();
   if (view == NULL)
   {
      map = NULL;
   }
   else
   {
      map = UTL_get_current_view_map((CView*)view);
      if (map != NULL)
      {
         if (!map->is_projection_set())
            map = NULL;
      }
   }

   if (map != NULL)
   {
      map->surface_to_geo(point.x, point.y, &lat, &lon);
   }

   const long overlay_handle = (long)OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   _bstr_t menu_items;
   try
   {
      fvw::ICallbackPtr callback1 = m_pdisp;
      fvw::ICallback2Ptr callback2 = m_pdisp;
      if (callback1 != NULL || callback2 != NULL)
      {
         HRESULT hr = S_OK;
         if (callback2 != NULL)
         {
            hr = callback2->GetMenuItemsEx(lat, lon, overlay_handle, object_handle, menu_id, menu_items.GetAddress());
         }

         if (callback1 != NULL && (callback2 == NULL || FAILED(hr)) )
            hr = callback1->GetMenuItems(overlay_handle, object_handle, menu_items.GetAddress());
      }
      else
      {
         try
         {
            CCallback2 callback2Impl(m_pdisp);
            callback2Impl.GetMenuItemsEx(lat, lon, overlay_handle, object_handle, menu_id, menu_items.GetAddress());
         }
         catch(COleException *e)
         {
            e->Delete();

            CCallback callbackImpl(m_pdisp);
            callbackImpl.GetMenuItems(overlay_handle, object_handle, menu_items.GetAddress());
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("GetMenuItems(Ex) failed - %s", (char *)e.Description());
      ERR_report(msg);
      return "";
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("GetMenuItems failed", *e);
      e->Delete();
   }

   return (char *)menu_items;
}

boolean_t CLayerOvl::can_drop_data( COleDataObject* data_object, CPoint& pt )
{
   return CBaseLayerOvl::can_drop_data(data_object, pt);
}

// test to see if we can read clipboard data
boolean_t CBaseLayerOvl::can_drop_data( COleDataObject* data_object, CPoint& pt )
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   ViewMapProj *map = UTL_get_current_view_map();

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (data_object && pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      try
      {
         ISettableMapProj *pSettableMapProj = map->GetSettableMapProj();

         long lCanDrop;
         pLayerEditor->CanDropOLEDataObject(layer_handle, pt.x, pt.y, pSettableMapProj, data_object->m_lpDataObject, &lCanDrop);

         return lCanDrop != 0;
      }
      catch(_com_error &)
      {
      }
   }

   return FALSE;
}

boolean_t CLayerOvl::paste_OLE_data_object( ViewMapProj* map, CPoint* pPt, 
   COleDataObject* data_object )
{
   return CBaseLayerOvl::paste_OLE_data_object(map, pPt, data_object);
}

// paste OLE data object(s) onto an overlay. Defaults to not being handled
boolean_t CBaseLayerOvl::paste_OLE_data_object( ViewMapProj* map, CPoint* pPt, 
   COleDataObject* data_object )
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (data_object && pPt && pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      try
      {
         ISettableMapProj *pSettableMapProj = map->GetSettableMapProj();

         if (pLayerEditor->PasteOLEDataObject(layer_handle, pPt->x, pPt->y, pSettableMapProj, data_object->m_lpDataObject) == S_OK)
            return TRUE;
      }
      catch(_com_error &)
      {
      }
   }

   return FALSE;
}

void CLayerOvl::drag(ViewMapProj *view, CPoint point, UINT flags, HCURSOR *cursor, 
   HintText **hint)
{
   return CBaseLayerOvl::drag(view, point, flags, cursor, hint);
}

//----------------------------------------------------------------------------
// This function handles route drags started from the selected() member
// function.  Route drags include moving an existing turn point or 
// additional point, inserting a turn point along an existing route leg,
// adding an additional point, or appending a turn point to the end of the
// route.  This function updates the display, cursor, and hint for the
// current point and drag object.
//----------------------------------------------------------------------------
void CBaseLayerOvl::drag(ViewMapProj *view, CPoint point, UINT flags, HCURSOR *cursor, 
   HintText **hint)
{
   static_cast<MapView *>(view->get_CView())->ScrollMapIfPointNearEdge(point.x, point.y); 

   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      long tmp_cursor = 0;
      _bstr_t bstrTooltip, bstrHelpText;

      ISettableMapProj *pSettableMapProj = view->GetSettableMapProj();
      CClientDC dc(view->get_CView());
      const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

      try
      {
         if (pLayerEditor->OnDragEx(layer_handle, pSettableMapProj, hDC, point.x, point.y,
            flags, &tmp_cursor, bstrTooltip.GetAddress(), bstrHelpText.GetAddress()) == S_OK)
         {
            *cursor = (HCURSOR)tmp_cursor;
            m_hint.set_tool_tip((char *)bstrTooltip);
            m_hint.set_help_text((char *)bstrHelpText);
            *hint = &m_hint;

            return;
         }
      }
      catch(_com_error &)
      {
      }
   }
   
   // get the location of the point
   d_geo_t location;
   view->surface_to_geo(point.x, point.y, &location.lat, &location.lon);

   if (pLayerEditor != NULL)
   {
      _bstr_t tooltip, helptext;
      long tmp_cursor;

      try
      {
         pLayerEditor->OnDrag(layer_handle, m_drag_object_id, location.lat,
            location.lon, (long)flags, &tmp_cursor, tooltip.GetAddress(), helptext.GetAddress());
         *cursor = (HCURSOR)tmp_cursor;
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::OnDrag failed");
      }

      m_hint.set_tool_tip((char *)tooltip);
      m_hint.set_help_text((char *)helptext);
      *hint = &m_hint;
   }
}

void CLayerOvl::drop(ViewMapProj *view, CPoint point, UINT flags)
{
   CBaseLayerOvl::drop(view, point, flags);
}

//----------------------------------------------------------------------------
// This function completes the route drag operation started by the 
// selected() member and contiued by the drag() member.
//----------------------------------------------------------------------------
void CBaseLayerOvl::drop(ViewMapProj *view, CPoint point, UINT flags)
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   m_bDragging = FALSE;

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      ISettableMapProj *pSettableMapProj = view->GetSettableMapProj();
      CClientDC dc(view->get_CView());
      const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

      try
      {
         if (pLayerEditor->OnDropEx(layer_handle, pSettableMapProj, hDC, point.x, point.y, flags) == S_OK)
         {
            return;
         }
      }
      catch(_com_error &)
      {
      }
   }

   // get the location of the point
   d_geo_t location;
   view->surface_to_geo(point.x, point.y, &location.lat, &location.lon);

   if (pLayerEditor != NULL)
   {
      try
      {
         pLayerEditor->OnDrop(layer_handle, m_drag_object_id, location.lat,
            location.lon, (long)flags);
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::OnDrop failed");
      }
   }
}

void CLayerOvl::cancel_drag(ViewMapProj *view)
{
   return CBaseLayerOvl::cancel_drag(view);
}

//----------------------------------------------------------------------------
// This function cancels the route drag operation started by the 
// selected() member and contiued by the drag() member.
//----------------------------------------------------------------------------
void CBaseLayerOvl::cancel_drag(ViewMapProj *view)
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   m_bDragging = FALSE;

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      ISettableMapProj *pSettableMapProj = view->GetSettableMapProj();
      CClientDC dc(view->get_CView());
      const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

      try
      {
         if (pLayerEditor->CancelDragEx(layer_handle, pSettableMapProj, hDC) == S_OK)
         {
            return;
         }
      }
      catch(_com_error &)
      {
      }
   }

   if (pLayerEditor != NULL)
   {
      const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

      try
      {
         pLayerEditor->CancelDrag(layer_handle, m_drag_object_id);
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::CancelDrag failed");
      }
   }
}

int CLayerOvl::on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point)
{
   return CBaseLayerOvl::on_dbl_click(pMapView, flags, point);
}

//----------------------------------------------------------------------------
// allows the overlay manager to pass double-click notices to overlays
//----------------------------------------------------------------------------
int CBaseLayerOvl::on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point)
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      ISettableMapProjPtr pSettableMapProj = pMapView->m_pCrntMapProj();
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));
      const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

      try
      {
         if (pLayerEditor->OnDoubleClickEx(layer_handle, pSettableMapProj, hDC, point.x, point.y, flags) == S_OK)
            return SUCCESS;
      }
      catch(_com_error &)
      {
      }
   }

   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return FAILURE;
   }

   OvlElement *element = m_pOverlay->GetOvlElementContainer()->hit_test(point);
   long object_handle;
   if (NULL == element)
      object_handle = -1;
   else
      object_handle = m_pOverlay->GetOvlElementContainer()->get_object_id(element);

   HWND hWnd = pMapView->m_hWnd();

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   // get the lat/lon location of the point that was double-clicked
   d_geo_t location;
   mapProjWrapper.surface_to_geo(point.x, point.y, &location.lat, &location.lon);
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnDoubleClicked(layer_handle,
            object_handle,
            (long)hWnd,
            location.lat,location.lon);
      }
      else
      {
         CCallback callbackImpl(m_pdisp);
         callbackImpl.OnDoubleClicked(layer_handle,
            object_handle,
            (long)hWnd,
            location.lat,location.lon);
      }
      return SUCCESS;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnDoubleClicked failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("OnDoubleClicked failed", *e);
      e->Delete();
   }

   return FAILURE;
}

int CLayerOvl::file_new()
{
   try
   {
      // retrieve the instance filename (full pathname) from the editor
      _bstr_t nextNewFilename;
      THROW_IF_NOT_OK(m_layer_editor->GetNextNewFileName(nextNewFilename.GetAddress()));
      m_fileSpecification = (char *)nextNewFilename;

      const int handle = OVL_get_overlay_manager()->get_overlay_handle(this);
      THROW_IF_NOT_OK(m_layer_editor->OnNewLayer(handle));
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("CLayerOvl::file_new failed - %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return SUCCESS;
}

int CLayerOvl::open(const CString & pathname)
{
   return open_file(pathname);
}

// save the file to the given name
int CLayerOvl::save_as(const CString & pathname, long nSaveFormat)
{
   if (get_ILayerEditor() != NULL)
   {
      const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(this);
      CString lcl_pathname(pathname);
      const int length = lcl_pathname.GetLength();
      char *buffer = lcl_pathname.GetBuffer(length);
      bstr_t bstr(buffer);

      try
      {
         if (SUCCEEDED(get_ILayerEditor()->Save(layer_handle, bstr)))
         {
            m_fileSpecification = pathname;
            set_modified(0);
            return SUCCESS;
         }
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::Save failed");
      }
   }

   return FAILURE;
}

int CLayerOvl::pre_save(boolean_t *cancel)
{
   return CBaseLayerOvl::pre_save(cancel);
}

// Inform the overlay that it is about to be saved, so it can perform
// any necessary clean up.  If the overlay returns cancel == TRUE then
// the save is canceled.
int CBaseLayerOvl::pre_save(boolean_t *cancel)
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      try
      {
         long lCancel;
         pLayerEditor->PreSave(layer_handle, &lCancel);
         *cancel = lCancel;

         return SUCCESS;
      }
      catch(_com_error &)
      {
      }
   }

   *cancel = (pLayerEditor == NULL);

   return SUCCESS;
}

int CBaseLayerOvl::open_file(CString filename)
{
   if (get_ILayerEditor() == NULL)
      return SUCCESS;

   // get the layer handle of this layer
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   // call the OpenFile function in the external editor
   char *buffer = filename.GetBuffer(filename.GetLength());
   bstr_t bstr(buffer);

   try
   {
      if (SUCCEEDED(get_ILayerEditor()->OpenFile(layer_handle, bstr)))
      {
         CLayerOvl *pLayerOverlay = dynamic_cast<CLayerOvl *>(this);
         if (pLayerOverlay != NULL)
         {
            pLayerOverlay->set_file_specification(filename);

            // initialize the saved flag to be TRUE (this means the file has been saved with this name before)
            OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(this);
            if (pFvOverlayPersistence != NULL)
            {
               pFvOverlayPersistence->put_m_bHasBeenSaved(TRUE);
               pFvOverlayPersistence->put_m_bIsReadOnly(is_read_only());
            }
         }

         return SUCCESS;
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("ILayerEditor::OpenFile failed - %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return FAILURE;
}

int CLayerOvl::pre_close(BOOL* cancel)
{
   return CBaseLayerOvl::pre_close(cancel);
}

// Inform the overlay that it is about to be closed, so it can perform
// any necessary clean up.  If the overlay returns cancel == TRUE then
// the close is canceled.  When this function is called with cancel
// set to NULL the overlay will be closed no matter what.
int CBaseLayerOvl::pre_close(BOOL* cancel)
{
   if (cancel != NULL)
      *cancel = FALSE;

   if (NULL == m_pdisp)
   {
      ERR_report("Client's callback dispatch pointer is not set - OnPreClose will not be called");
      return SUCCESS;
   }

   long layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
         callback->OnPreClose(layer_handle, (long *)cancel);
      else
      {
         CCallback callbackImpl(m_pdisp);
         callbackImpl.OnPreClose(layer_handle, (long *)cancel);
      }
   }
   catch (_com_error &e)
   {
      CString msg;
      msg.Format("OnPreClose failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      // don't report an error in this case
      e->Delete();
   }

   // Even if the OnPreClose callback failed, we should return SUCCESS.  
   // Otherwise, the overlay won't get closed.
   return SUCCESS;
}

// returns true if the object would be visible, false otherwise
boolean_t CBaseLayerOvl::object_above_threshold(MapProj *map, OvlElement *object)
{
   map_scale_utils::MapScale scale(map->scale().GetScale(), map->scale().GetScaleUnits());
   return m_pOverlay->GetOvlElementContainer()->object_above_threshold(scale, object);
}

long CBaseLayerOvl::DisplayMask( const CString& mask_file, 
   double flight_altitude,
   long base_elevation, 
   long mask_color, 
   long brush_style)
{
   OvlTerrainMask *pMask = new OvlTerrainMask( m_pOverlay, mask_file );

   pMask->set_mask_color( mask_color );
   pMask->set_brush( mask_color, brush_style );
   pMask->set_flight_altitude(flight_altitude);
   pMask->set_base_elevation(base_elevation);

   int rc = m_pOverlay->GetOvlElementContainer()->add_object(pMask);
   m_pOverlay->invalidate();

   // For drawing the mask and progress
   MapView *map_view = (MapView *)UTL_get_active_non_printing_view();
   if (map_view)
   {
      map_view->set_current_map_invalid();
      map_view->invalidate_view();
   }

   return rc;
}

// return TRUE if this overlay cannot be edited
long CBaseLayerOvl::is_read_only()
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      try
      {
         long lReadOnly;
         pLayerEditor->IsReadOnly(layer_handle, &lReadOnly);

         return lReadOnly;
      }
      catch(_com_error &)
      {
      }
   }

   return FALSE;
}

boolean_t CLayerOvl::test_snap_to(ViewMapProj* map, CPoint point)
{
   return CBaseLayerOvl::test_snap_to(map, point);
}

// test to see if this overlay can do a single point snap to at this point
boolean_t CBaseLayerOvl::test_snap_to(ViewMapProj* map, CPoint point)
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      try
      {
         ISettableMapProj *pSettableMapProj = map->GetSettableMapProj();
         CClientDC dc(map->get_CView());
         const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

         long lCanSnapTo = FALSE;
         pLayerEditor->OnTestSnapTo(layer_handle, pSettableMapProj, hDC, point.x, point.y, &lCanSnapTo);

         return lCanSnapTo != 0;
      }
      catch(_com_error &)
      {
      }
   }

   return FALSE;
}

boolean_t CLayerOvl::do_snap_to(ViewMapProj* map, CPoint point, 
   CList<SnapToInfo *, SnapToInfo *> & snap_to_list)
{
   return CBaseLayerOvl::do_snap_to(map, point, snap_to_list);
}

// get the snap to objects for this overlay at this point
boolean_t CBaseLayerOvl::do_snap_to(ViewMapProj* map, CPoint point, 
   CList<SnapToInfo *, SnapToInfo *> & snap_to_list)
{
   const int layer_handle = OVL_get_overlay_manager()->get_overlay_handle(m_pOverlay);
   BOOL bSnapToPointFound = FALSE;

   LayerEditorImpl *pLayerEditor = get_ILayerEditor();
   if (pLayerEditor && pLayerEditor->GetInterfaceVersion() >= 3)
   {
      try
      {
         ISettableMapProj *pSettableMapProj = map->GetSettableMapProj();
         CClientDC dc(map->get_CView());
         const long hDC = reinterpret_cast<long>(dc.GetSafeHdc());

         ISnapToPointsListPtr smpSnapToPointsList;
         HRESULT hr = smpSnapToPointsList.CreateInstance(__uuidof(SnapToPointsList));

         if (pLayerEditor->OnGetSnapToPoints(layer_handle, pSettableMapProj, hDC, point.x, point.y,
            smpSnapToPointsList) == S_OK)
         {
            HRESULT hr = smpSnapToPointsList->MoveFirst();
            while (hr == S_OK)
            {
               bSnapToPointFound = TRUE;
               snap_to_list.AddTail(SnapToInfo::CreateFromSnapToPointsList(smpSnapToPointsList));
               hr = smpSnapToPointsList->MoveNext();
            }

            return TRUE;
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("Plug-in overlay snap to failed: (%s)", (char *)e.Description());
      }
   }

   return bSnapToPointFound;
}

// CStaticLayerOvl
//

CStaticLayerOvl::CStaticLayerOvl() : 
CBaseLayerOvl(::GetCurrentThreadId(), this)
{
}

CStaticLayerOvl::CStaticLayerOvl(const DWORD owning_thread_id,
   CString displayName, IDispatch *pdisp, CString icon_name,
   LayerEditorImpl *layer_editor /*= NULL*/) :
   CBaseLayerOvl(owning_thread_id, this)
{
   Setup(owning_thread_id, displayName, pdisp, icon_name, layer_editor);
}

void CStaticLayerOvl::Setup(const DWORD owning_thread_id, CString displayName,
   IDispatch *pdisp, CString icon_name, LayerEditorImpl *layer_editor)
{
   SetOwningThreadId(owning_thread_id);
   m_displayName = displayName;
   m_pdisp = pdisp;
   m_icon_name = icon_name;
   m_layer_editor = layer_editor;
}

HRESULT CStaticLayerOvl::InternalInitialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;

   // Static overlays will pass their display name into the OpenFile method
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(m_overlayDescGuid);
   if (pOverlayTypeDesc == NULL || open_file(pOverlayTypeDesc->displayName) != SUCCESS)
   {
      ERR_report("CStaticLayerOvl::Initialize - CBaseLayerOvl::open_file failed");
      return E_FAIL;
   }

   return S_OK;
}

CStaticLayerOvl::~CStaticLayerOvl()
{
}

CString CStaticLayerOvl::GetDisplayName() 
{ 
   if (m_displayName.IsEmpty())
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(get_m_overlayDescGuid());
      if (pOverlayTypeDesc != NULL)
         return pOverlayTypeDesc->displayName;
   }

   return m_displayName;
}


// Derived classes must implement this function.
// When m_valid == TRUE do a full window redraw, otherwise draw
// from scratch. draw() should set m_valid to TRUE when it is done.
int CStaticLayerOvl::draw(ActiveMap* map)
{
   return CBaseLayerOvl::draw(map);
}

int CStaticLayerOvl::draw_to_base_map(ActiveMap* map)
{
   return CBaseLayerOvl::draw_to_base_map(map);
}

BOOL CStaticLayerOvl::can_add_pixmaps_to_base_map() 
{
   return CBaseLayerOvl::can_add_pixmaps_to_base_map();
}

int CStaticLayerOvl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   return CBaseLayerOvl::selected(pMapView, point, flags);
}

bool CStaticLayerOvl::is_selection_locked()
{
   return CBaseLayerOvl::is_selection_locked();
}

void CStaticLayerOvl::drag(ViewMapProj *view, CPoint point, UINT flags, HCURSOR *cursor, 
   HintText **hint)
{
   return CBaseLayerOvl::drag(view, point, flags, cursor, hint);
}

void CStaticLayerOvl::cancel_drag(ViewMapProj *view)
{
   return CBaseLayerOvl::cancel_drag(view);
}

void CStaticLayerOvl::drop(ViewMapProj *view, CPoint point, UINT flags)
{
   CBaseLayerOvl::drop(view, point, flags);
}

int CStaticLayerOvl::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   return CBaseLayerOvl::on_mouse_moved(pMapView, point, flags);
}

int CStaticLayerOvl::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   return CBaseLayerOvl::on_left_mouse_button_up(pMapView, point, nFlags);
}

boolean_t CStaticLayerOvl::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
   UINT nFlags, CPoint point)
{
   return CBaseLayerOvl::offer_keydown(pMapView, nChar, nRepCnt, nFlags, point);
}

boolean_t CStaticLayerOvl::offer_keyup(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
   UINT nFlags, CPoint point)
{
   return CBaseLayerOvl::offer_keyup(pMapView, nChar, nRepCnt, nFlags, point);
}


// Note: This appears to be dead code

//C_icon *CStaticLayerOvl::hit_test(MapProj* map, CPoint point)
//{
//   return CBaseLayerOvl::hit_test(map, point);
//}

void CStaticLayerOvl::menu(ViewMapProj* map, CPoint point, 
   CList<CFVMenuNode *, CFVMenuNode *> &list)
{
   CBaseLayerOvl::menu(map, point, list);
}

int CStaticLayerOvl::on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point)
{
   return CBaseLayerOvl::on_dbl_click(pMapView, flags, point);
}

boolean_t CStaticLayerOvl::paste_OLE_data_object( ViewMapProj* map, CPoint* pPt, 
   COleDataObject* data_object )
{
   return CBaseLayerOvl::paste_OLE_data_object(map, pPt, data_object);
}

boolean_t CStaticLayerOvl::can_drop_data( COleDataObject* data_object, CPoint& pt )
{
   return CBaseLayerOvl::can_drop_data(data_object, pt);
}

int CStaticLayerOvl::pre_save(boolean_t *cancel)
{
   return CBaseLayerOvl::pre_save(cancel);
}

int CStaticLayerOvl::pre_close(BOOL* cancel)
{
   return CBaseLayerOvl::pre_close(cancel);
}

boolean_t CStaticLayerOvl::test_snap_to(ViewMapProj* map, CPoint point)
{
   return CBaseLayerOvl::test_snap_to(map, point);
}

boolean_t CStaticLayerOvl::do_snap_to(ViewMapProj* map, CPoint point, 
   CList<SnapToInfo *, SnapToInfo *> & snap_to_list)
{
   return CBaseLayerOvl::do_snap_to(map, point, snap_to_list);
}