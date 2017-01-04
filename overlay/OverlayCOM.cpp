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

// COverlayCOM
//

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/Overlay/OverlayCOM.h"

// system includes
// third party files
// other FalconView headers
#include "Common/ComErrorObject.h"
#include "FvCore/Include/GuidStringConverter.h"

// this project's headers
#include "FalconView/DisplayElementRootNode.h"
#include "FalconView/getobjpr.h"
#include "FalconView/include/err.h"
#include "FalconView/include/mapx.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/SnapTo.h"
#include "FalconView/mapview.h"
#include "FalconView/PlaybackDialog/viewtime.h"
#include "FalconView/UIThreadOperation.h"
#include "FalconView/VerticalViewDisplay.h"
#include "FalconView/include/TabularEditorDlg.h"
#include "FalconView/overlay/FvTabularEditorImpl.h"
//#include "FalconView/overlay/OverlayBaseDataObject.h"
//#include "FalconView/overlay/FvOverlayDataObjectImpl.h"

COverlayCOM::COverlayCOM(const char* lpszDisplayName, CLSID clsid) :
   // alphabetised...
   m_clsid(clsid),
   m_displayName(lpszDisplayName),
   m_drawing(false),
   m_pFvTabularEditor(nullptr),
   m_pOverlayEventSink(NULL),
   m_reference_count(0)
{
   ::InitializeCriticalSection(&m_critical_section);
   CMainFrame* pFrame = fvw_get_frame();
   m_map_view = pFrame->GetFvMapView();
   AddRef();
}

COverlayCOM::COverlayCOM(const DWORD owning_thread_id,
   FalconViewOverlayLib::IFvOverlay *fv_overlay) :
   // alphabetised...
   m_clsid(GUID_NULL),
   m_pFvTabularEditor(nullptr),
   m_pOverlayEventSink(NULL),
   m_owning_thread_id(owning_thread_id),
   m_reference_count(0),
   m_spFvOverlay(fv_overlay)
{
   ::InitializeCriticalSection(&m_critical_section);
   CMainFrame* pFrame = fvw_get_frame();
   m_map_view = pFrame->GetFvMapView();
   AddRef();
}

COverlayCOM::~COverlayCOM()
{
   if (m_pOverlayEventSink != NULL)
   {
      // the overlay may still have a reference to the overlay event
      // sink which contains a pointer to this COverlayCOM.  However
      // this COverlayCOM is being destroyed.
      m_pOverlayEventSink->SetOverlay(NULL);
      m_pOverlayEventSink->Release();
   }

   if (m_pFvTabularEditor != nullptr)
      m_pFvTabularEditor->Release();

   ::DeleteCriticalSection(&m_critical_section);
}

// Returns the overlay descriptor guid for this overlay.
GUID COverlayCOM::get_m_overlayDescGuid()
{
   GUID result = GUID_NULL;
   try
   {
      result = m_spFvOverlay->OverlayDescGuid;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// True if the overlay is visible, false otherwise.
HRESULT COverlayCOM::put_m_bIsOverlayVisible(long bIsOverlayVisible)
{
   HRESULT result = S_OK;
   try
   {
      m_spFvOverlay->IsOverlayVisible = bIsOverlayVisible;
      ResetDisplayNodeVisibility();
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
      result = E_FAIL;
   }
   return S_OK;
}

long COverlayCOM::get_m_bIsOverlayVisible()
{
   long result = TRUE;
   try
   {
      result = m_spFvOverlay->IsOverlayVisible;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// This method does the actual work
HRESULT COverlayCOM::InternalInitializeHelper(GUID overlayDescGuid)
{
   HRESULT hr = 0;
   try
   {
      m_owning_thread_id = ::GetCurrentThreadId();
      hr = m_spFvOverlay.CreateInstance(m_clsid);
      if (hr != S_OK)
      {
         CString msg;
         msg.Format("Error creating %s overlay {%s} HRESULT = %x",
            m_displayName.c_str(),
            string_utils::CGuidStringConverter(m_clsid).GetGuidString().c_str(),
            hr);
         ERR_report(msg);
         return hr;
      }

      hr = m_spFvOverlay->Initialize(overlayDescGuid);
      if (hr == S_OK)
      {
         PostInitialize();
      }

      return hr;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return E_FAIL;
}

// Initialize the overlay.  The read-only property m_overlayDescGuid should be
// set by this method
HRESULT COverlayCOM::InternalInitialize(GUID overlayDescGuid)
{
   HRESULT result = E_FAIL;
   result = InternalInitializeHelper(overlayDescGuid);
   return result;
}

void COverlayCOM::QueryForInterfaces()
{
   m_spFvOverlay2 = m_spFvOverlay;
   m_spFvOverlayTeardownEvents = m_spFvOverlay;
   m_spFvOverlayRenderer = m_spFvOverlay;
   m_spFvOverlayRenderer3D = m_spFvOverlay;
   m_spFvOverlayBaseMapRenderer = m_spFvOverlay;
   m_spFvOverlayVerticalViewRenderer = m_spFvOverlay;
   m_spFvOverlayUIEvents = m_spFvOverlay;
   m_spFvOverlayVerticalViewUIEvents = m_spFvOverlay;
   m_spFvOverlayUIEventRoutingOverrides = m_spFvOverlay;
   m_spFvOverlayContextMenu = m_spFvOverlay;
   m_spFvOverlaySnappablePoints = m_spFvOverlay;
   m_spFvOverlayCopyPaste = m_spFvOverlay;
   m_spFvOverlayElement = m_spFvOverlay;
   m_spFvOverlayElementEnumerator = m_spFvOverlay;
   m_spFvPlaybackEventsObserver = m_spFvOverlay;
   m_spFvPlaybackTimeInterval = m_spFvOverlay;
   m_spFvOverlayTypeOverrides = m_spFvOverlay;
   m_spFvOverlayTypeOverrides2 = m_spFvOverlayTypeOverrides;
   m_spFvOverlayUndoRedo = m_spFvOverlay;

   m_spFvTabularEditable = m_spFvOverlay;
}

void COverlayCOM::SetEventSink()
{
   // SetEventSink should only be called once...
   ASSERT(m_pOverlayEventSink == NULL);
   CComObject<CFvOverlayEventSink>::CreateInstance(&m_pOverlayEventSink);
   m_pOverlayEventSink->AddRef();
   m_pOverlayEventSink->SetOverlay(this);

   IUnknownPtr overlayEventSink = m_pOverlayEventSink;
   m_spFvOverlay->OverlayEventSink = overlayEventSink;
}

void COverlayCOM::SendInitialPlaybackNotifications()
{
   OnPlaybackRateChanged(CMainFrame::GetPlaybackDialog().get_playback_rate());
   OnPlaybackTimeChanged(CMainFrame::GetPlaybackDialog().get_current_time());
   if (CMainFrame::GetPlaybackDialog().in_playback())
      OnPlaybackStarted();
}

int COverlayCOM::pre_save(boolean_t *cancel)
{
   // plug-in overlay without editors cannot be saved
   OverlayTypeDescriptor* pOverlayDesc = OVL_get_type_descriptor_list()->
      GetOverlayTypeDescriptor(get_m_overlayDescGuid());
   *cancel = pOverlayDesc == NULL;
   return SUCCESS;
}

int COverlayCOM::pre_close(boolean_t *cancel /*= NULL*/)
{
   int result = FAILURE;
   try
   {
      const long bCanAbortTermination = (cancel != NULL);
      const long bAbortTermination =
         m_spFvOverlay->Terminate(bCanAbortTermination);
      if (cancel != NULL)
         *cancel = bAbortTermination;

      if (cancel != NULL && !*cancel)
      {
         // if the overlay implements the IFvOverlayBaseMapRenderer interface,
         // then we'll automatically invalidate the map when the overlay is
         // terminated
         if (m_spFvOverlayBaseMapRenderer != NULL)
         {
            MapView *view =
               static_cast<MapView *>(UTL_get_active_non_printing_view());

            if (view)
               view->set_current_map_invalid();
         }
      }

      result = SUCCESS;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// IFvOverlay2
//

void COverlayCOM::SetFrameWnd(FalconViewOverlayLib::IFvFrameWnd2* pFrameWnd)
{
   try
   {
      if (m_spFvOverlay2 != NULL)
         m_spFvOverlay2->FrameWnd = pFrameWnd;
   }
   catch(_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
}

// IFvOverlayTeardownEvents
//

void COverlayCOM::Finalize()
{
   try
   {
      if (m_spFvOverlayTeardownEvents != NULL)
         m_spFvOverlayTeardownEvents->Finalize();
   }
   catch(_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
}

int COverlayCOM::draw(ActiveMap* map)
{
   const DWORD thread_id = ::GetCurrentThreadId();
   if (thread_id == m_owning_thread_id)
   {
      if(m_drawing==true)
      {
         // We will have to try again later.
         invalidate();
         return S_OK;
      }
      m_drawing = true;
      try
      {
         if (m_spFvOverlayRenderer != NULL)
            m_spFvOverlayRenderer->OnDraw(m_map_view, map->m_interface_ptr);
      }
      catch (_com_error &e)
      {
         REPORT_COM_ERROR(e);
      }
      m_drawing = false;
   }
   else
   {
      auto on_draw = [&]()
      {
         OnDraw(map->m_interface_ptr);
      };
      bool wait_for_operation = true;
      new fvw::UIThreadOperation(std::move(on_draw), wait_for_operation);
   }
   return S_OK;
}

int COverlayCOM::OnDrawToBaseMap(IActiveMapProj *pActiveMap)
{
   try
   {
      CMainFrame* pFrame = fvw_get_frame();
      if (pFrame != NULL && m_spFvOverlayBaseMapRenderer != NULL)
         m_spFvOverlayBaseMapRenderer->OnDraw(
         pFrame->GetFvMapView(), pActiveMap);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

HRESULT COverlayCOM::OnDrawDraped(IActiveMapProj* pActiveMap)
{
   try
   {
      if (m_spFvOverlayRenderer3D != nullptr)
         return m_spFvOverlayRenderer3D->OnDrawDraped(m_map_view, pActiveMap);
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }

   return S_OK;
}

BOOL COverlayCOM::SupportsDrawDraped()
{
   return m_spFvOverlayRenderer3D != nullptr;
}

int COverlayCOM::DrawToVerticalDisplay(CDC *pDC, CVerticalViewProjector*)
{
   int result = FAILURE;
   try
   {
      if (m_spFvOverlayVerticalViewRenderer != NULL)
      {
         CMainFrame *pFrame = fvw_get_frame();
         if (pFrame)
         {
            CVerticalViewDisplay *pVerticalView = pFrame->GetVerticalDisplay();
            if (pVerticalView != NULL)
               m_spFvOverlayVerticalViewRenderer->OnDraw(
               pVerticalView->GetFvVerticalView(),
               reinterpret_cast<long>(pDC->GetSafeHdc()));
         }
      }
      result = SUCCESS;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

int COverlayCOM::test_selected_vertical(CVerticalViewProjector*, CPoint point,
   UINT flags, HCURSOR *, HintText **)
{
   int result = FAILURE;
   try
   {
      if (m_spFvOverlayVerticalViewUIEvents != NULL)
      {
         CMainFrame *pFrame = fvw_get_frame();
         if (pFrame)
         {
            CVerticalViewDisplay *pVerticalView = pFrame->GetVerticalDisplay();
            if (pVerticalView != NULL)
            {
               long bHandled = m_spFvOverlayVerticalViewUIEvents->MouseMove(
                  pVerticalView->GetFvVerticalView(), point.x, point.y, flags);
               result = bHandled ? SUCCESS : FAILURE;
            }
         }
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

int COverlayCOM::selected_vertical(CVerticalViewProjector*, CPoint point,
   UINT flags, HCURSOR *, HintText **)
{
   int result = FAILURE;
   try
   {
      if (m_spFvOverlayVerticalViewUIEvents != NULL)
      {
         CMainFrame *pFrame = fvw_get_frame();
         if (pFrame)
         {
            CVerticalViewDisplay *pVerticalView = pFrame->GetVerticalDisplay();
            if (pVerticalView != NULL)
            {
               long bHandled =
                  m_spFvOverlayVerticalViewUIEvents->MouseLeftButtonDown(
                  pVerticalView->GetFvVerticalView(), point.x, point.y, flags);
               result = bHandled ? SUCCESS : FAILURE;
            }
         }
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

BOOL COverlayCOM::can_add_pixmaps_to_base_map()
{
   return m_spFvOverlayBaseMapRenderer != NULL;
}

// MouseMove occurs when the mouse pointer was moved in the view
HRESULT COverlayCOM::MouseMove(FalconViewOverlayLib::IFvMapView *pMapView,
   long x, long y, long nFlags, long *pbHandled)
{
   try
   {
      if (m_spFvOverlayUIEvents != NULL)
      {
         *pbHandled = m_spFvOverlayUIEvents->MouseMove(pMapView, x, y, nFlags);
      }
      else
      {
         *pbHandled = FALSE;
      }
   }
   catch(_com_error &e)
   {
      *pbHandled = FALSE;
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

// MouseLeftButtonDown occurs when the left mouse button is pressed while the
// mouse pointer is over the given view
HRESULT COverlayCOM::MouseLeftButtonDown(
   FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags,
   long *pbHandled)
{
   try
   {
      if (m_spFvOverlayUIEvents != NULL)
      {
         *pbHandled =
            m_spFvOverlayUIEvents->MouseLeftButtonDown(pMapView, x, y, nFlags);
      }
      else
      {
         *pbHandled = FALSE;
      }

   }
   catch(_com_error &e)
   {
      *pbHandled = FALSE;
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

// MouseLeftButtonUp occurs when the left mouse button is released while the
// mouse point is over the given view
HRESULT COverlayCOM::MouseLeftButtonUp(
   FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags,
   long *pbHandled)
{
   try
   {
      if (m_spFvOverlayUIEvents != NULL)
      {
         *pbHandled =
            m_spFvOverlayUIEvents->MouseLeftButtonUp(pMapView, x, y, nFlags);
      }
      else
      {
         *pbHandled = FALSE;
      }
   }
   catch(_com_error &e)
   {
      *pbHandled = FALSE;
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

// MouseRightButtonDown occurs when the right mouse button is pressed while the
// mouse pointer is over the given view.
HRESULT COverlayCOM::MouseRightButtonDown(
   FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags,
   long *pbHandled)
{
   try
   {
      if (m_spFvOverlayUIEvents != NULL)
      {
         *pbHandled =
            m_spFvOverlayUIEvents->MouseRightButtonDown(pMapView, x, y, nFlags);
      }
      else
      {
         *pbHandled = FALSE;
      }
   }
   catch(_com_error &e)
   {
      *pbHandled = FALSE;
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

// MouseDoubleClick occurs when the mouse button is double clicked over the
// given view
HRESULT COverlayCOM::MouseDoubleClick(
   FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags,
   long *pbHandled)
{
   try
   {
      if (m_spFvOverlayUIEvents != NULL)
      {
         *pbHandled =
            m_spFvOverlayUIEvents->MouseDoubleClick(pMapView, x, y, nFlags);
      }
      else
      {
         *pbHandled = FALSE;
      }
   }
   catch(_com_error &e)
   {
      *pbHandled = FALSE;
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

// MouseWheel occurs when the user rotates the mouse wheel while the mouse
// pointer is over the given view
HRESULT COverlayCOM::MouseWheel(FalconViewOverlayLib::IFvMapView *pMapView,
   long x, long y, long zDelta, long nFlags, long *pbHandled)
{
   try
   {
      if (m_spFvOverlayUIEvents != NULL)
      {
         *pbHandled =
            m_spFvOverlayUIEvents->MouseWheel(pMapView, x, y, zDelta, nFlags);
      }
      else
      {
         *pbHandled = FALSE;
      }
   }
   catch(_com_error &e)
   {
      *pbHandled = FALSE;
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

// KeyDown occurs when a key is pressed
HRESULT COverlayCOM::KeyDown(FalconViewOverlayLib::IFvMapView *pMapView,
   long x, long y, long nChar, long nRepCnt, long nFlags, long *pbHandled)
{
   try
   {
      if (m_spFvOverlayUIEvents != NULL)
      {
         *pbHandled = m_spFvOverlayUIEvents->KeyDown(
            pMapView, x, y, nChar, nRepCnt, nFlags);
      }
      else
      {
         *pbHandled = FALSE;
      }
   }
   catch(_com_error &e)
   {
      *pbHandled = FALSE;
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

// KeyUp occurs when a key is released
HRESULT COverlayCOM::KeyUp(FalconViewOverlayLib::IFvMapView *pMapView,
   long x, long y, long nChar, long nRepCnt, long nFlags, long *pbHandled)
{
   try
   {
      if (m_spFvOverlayUIEvents != NULL)
      {
         *pbHandled = m_spFvOverlayUIEvents->KeyUp(
            pMapView, x, y, nChar, nRepCnt, nFlags);
      }
      else
      {
         *pbHandled = FALSE;
      }
   }
   catch(_com_error &e)
   {
      *pbHandled = FALSE;
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

long COverlayCOM::get_m_bDirectlyRouteMouseMove()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayUIEventRoutingOverrides != NULL)
         result = m_spFvOverlayUIEventRoutingOverrides->DirectlyRouteMouseMove;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

long COverlayCOM::get_m_bDirectlyRouteMouseLeftButtonDown()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayUIEventRoutingOverrides != NULL)
         result =  m_spFvOverlayUIEventRoutingOverrides->
         DirectlyRouteMouseLeftButtonDown;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

long COverlayCOM::get_m_bDirectlyRouteMouseLeftButtonUp()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayUIEventRoutingOverrides != NULL)
         result = m_spFvOverlayUIEventRoutingOverrides->
         DirectlyRouteMouseLeftButtonUp;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

long COverlayCOM::get_m_bDirectlyRouteKeyDown()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayUIEventRoutingOverrides != NULL)
         result = m_spFvOverlayUIEventRoutingOverrides->DirectlyRouteKeyDown;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

long COverlayCOM::get_m_bDirectlyRouteKeyUp()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayUIEventRoutingOverrides != NULL)
         result = m_spFvOverlayUIEventRoutingOverrides->DirectlyRouteKeyUp;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

HRESULT COverlayCOM::AppendMenuItems(
   FalconViewOverlayLib::IFvContextMenu *pContextMenu,
   FalconViewOverlayLib::IFvMapView *pMapView, long x, long y)
{
   try
   {
      if (m_spFvOverlayContextMenu != NULL)
         m_spFvOverlayContextMenu->AppendMenuItems(
         pContextMenu, pMapView, x, y);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return S_OK;
}

// test to see if this overlay can do a single point snap to at this point
boolean_t COverlayCOM::test_snap_to(ViewMapProj* /*map*/, CPoint point)
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlaySnappablePoints != NULL)
         result = m_spFvOverlaySnappablePoints->CanSnapTo(
         m_map_view, point.x, point.y);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// get the snap to objects for this overlay at this point
boolean_t COverlayCOM::do_snap_to(ViewMapProj* /*map*/, CPoint point,
   CList<SnapToInfo *, SnapToInfo *> &snap_to_list)
{
   try
   {
      if (m_spFvOverlaySnappablePoints != NULL)
      {
         ISnapToPointsListPtr spSnapToPointsList;
         CO_CREATE(spSnapToPointsList, CLSID_SnapToPointsList);

         m_spFvOverlaySnappablePoints->GetSnappablePoints(
            m_map_view, point.x, point.y, spSnapToPointsList);

         // convert snap-to points list object into internal CList (for now)
         //
         HRESULT hr = spSnapToPointsList->MoveFirst();
         while (hr == S_OK)
         {
            snap_to_list.AddTail(
               SnapToInfo::CreateFromSnapToPointsList(spSnapToPointsList));
            hr = spSnapToPointsList->MoveNext();
         }
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return TRUE;
}

// paste OLE data object(s) onto an overlay. Defaults to not being handled
boolean_t COverlayCOM::paste_OLE_data_object(ViewMapProj* map, CPoint* pPt,
   COleDataObject* data_object)
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayCopyPaste != NULL)
      {
         IUnknown *pUnknown =
            static_cast<IUnknown *>(data_object->GetIDataObject(FALSE));
         m_spFvOverlayCopyPaste->DropDataObject(
            m_map_view, pPt->x, pPt->y, pUnknown);
         result = TRUE;
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// test to see if we can read clipboard data
boolean_t COverlayCOM::can_drop_data(COleDataObject* data_object, CPoint& pt)
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayCopyPaste != NULL)
      {
         IUnknown *pUnknown =
            static_cast<IUnknown *>(data_object->GetIDataObject(FALSE));
         result = m_spFvOverlayCopyPaste->CanDropDataObject(
            m_map_view, pt.x, pt.y, pUnknown);
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// return TRUE if data from the current overlay's data can be copied to the
// "clipboard" (Edit | Copy will be enabled)
boolean_t COverlayCOM::is_copy_to_clipboard_allowed()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayCopyPaste != NULL)
         result = m_spFvOverlayCopyPaste->CanCopyToClipboard(
         m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// copy data from the current overlay to the "clipboard" (handle Edit | Copy)
void COverlayCOM::copy_to_clipboard()
{
   try
   {
      if (m_spFvOverlayCopyPaste != NULL)
         m_spFvOverlayCopyPaste->CopyToClipboard(m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

// return TRUE if data can be pasted from the "clipboard" to the current overlay
// (Edit | Paste will be enabled)
boolean_t COverlayCOM::is_paste_from_clipboard_allowed()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayCopyPaste != NULL)
         result =  m_spFvOverlayCopyPaste->CanPasteFromClipboard(
         m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// paste data from the "clipboard" to the current overlay
void COverlayCOM::paste_from_clipboard()
{
   try
   {
      if (m_spFvOverlayCopyPaste != NULL)
         m_spFvOverlayCopyPaste->PasteFromClipboard(m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

// IFvOverlayUndoRedo
//
boolean_t COverlayCOM::can_undo()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayUndoRedo != NULL)
         result =  m_spFvOverlayUndoRedo->CanUndo(m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

boolean_t COverlayCOM::can_redo()
{
   long result = FALSE;
   try
   {
      if (m_spFvOverlayUndoRedo != NULL)
         result =  m_spFvOverlayUndoRedo->CanRedo(m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

void COverlayCOM::undo()
{
   try
   {
      if (m_spFvOverlayUndoRedo != NULL)
         m_spFvOverlayUndoRedo->Undo(m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

void COverlayCOM::redo()
{
   try
   {
      if (m_spFvOverlayUndoRedo != NULL)
         m_spFvOverlayUndoRedo->Redo(m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

// registers varDispatch to receive callbacks
HRESULT COverlayCOM::RegisterForCallbacks(VARIANT varDispatch)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvOverlayElement != NULL)
         result = m_spFvOverlayElement->RegisterForCallbacks(varDispatch);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Updates an element of the overlay using the given XML string
HRESULT COverlayCOM::UpdateElement(BSTR bstrElementXml)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvOverlayElement != NULL)
         result = m_spFvOverlayElement->UpdateElement(bstrElementXml);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// By default an overlay implementing this interface supports IFvOverlayElement.
// However, some generic overlays such as COverlayCOM need to dynamically state
// whether they support IFvOverlayElement or not.
BOOL COverlayCOM::SupportsFvOverlayElement()
{
   return m_spFvOverlayElement != NULL;
}

// The XML that describes the current element
BSTR COverlayCOM::bstrCurrentXml()
{
   bool success = false;
   BSTR result;
   try
   {
      if (m_spFvOverlayElementEnumerator != NULL)
      {
         result = m_spFvOverlayElementEnumerator->bstrCurrentXml;
         success = true;
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   if (!success)
      return _bstr_t(L"").Detach();
   return result;
}

// Resets the enumerator.  After a call to Reset, the current element will be
// NULL until MoveNext is called
HRESULT COverlayCOM::Reset()
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvOverlayElementEnumerator != NULL)
         result = m_spFvOverlayElementEnumerator->Reset();
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Moves to the next element in enumeration
HRESULT COverlayCOM::MoveNext(long *pbRet)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvOverlayElementEnumerator != NULL)
      {
         *pbRet = m_spFvOverlayElementEnumerator->MoveNext();
         result = S_OK;
      }
   }
   catch(_com_error &e)
   {
      *pbRet = FALSE;
      REPORT_COM_ERROR(e);
   }
   return result;
}

// By default an overlay implementing this interface supports
// IFvOverlayElementEnumerator.  However, some generic overlays such as
// COverlayCOM need to dynamically state whether they support
// IFvOverlayElementEnumerator or not.
BOOL COverlayCOM::SupportsFvOverlayElementEnumerator()
{
   return m_spFvOverlayElementEnumerator != NULL;
}

// Called when a playback is started
HRESULT COverlayCOM::OnPlaybackStarted()
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvPlaybackEventsObserver != NULL)
         result = m_spFvPlaybackEventsObserver->OnPlaybackStarted(m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Called when the playback is stopped
HRESULT COverlayCOM::OnPlaybackStopped()
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvPlaybackEventsObserver != NULL)
         result =m_spFvPlaybackEventsObserver->OnPlaybackStopped(m_map_view);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Called whenever the playback time changes
HRESULT COverlayCOM::OnPlaybackTimeChanged(DATE dateCurrentTime)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvPlaybackEventsObserver != NULL)
         result = m_spFvPlaybackEventsObserver->OnPlaybackTimeChanged(
         m_map_view, dateCurrentTime);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Called whenever the playback rate changes
HRESULT COverlayCOM::OnPlaybackRateChanged(long lNewPlaybackRate)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvPlaybackEventsObserver != NULL)
      {
         lNewPlaybackRate *=
            CMainFrame::GetPlaybackDialog().is_playback_reversed() ? -1 : 1;
         result = m_spFvPlaybackEventsObserver->OnPlaybackRateChanged(
            m_map_view, lNewPlaybackRate);
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Returns the beginning and ending times for the overlay's time segment.  If
// the time segment is valid set pbTimeSegmentValue to a non-zero integer,
// otherwise set it to zero.  An example where the time segment would be invalid
// is an uncalculated route.  In this case, the overlay is not accounted for in
// the overall clock time and does not show up in the playback dialog's gannt
// chart
HRESULT COverlayCOM::GetTimeSegment(DATE *pBegin, DATE *pEnd,
   long *pbTimeSegmentValid)
{
   HRESULT result = E_FAIL;
   *pbTimeSegmentValid = FALSE;
   try
   {
      if (m_spFvPlaybackTimeInterval != NULL)
      {
         FalconViewOverlayLib::TimeInterval timeInterval =
            m_spFvPlaybackTimeInterval->PlaybackTimeInterval;
         *pBegin = timeInterval.dtBegin;
         *pEnd = timeInterval.dtEnd;
         *pbTimeSegmentValid = timeInterval.bTimeIntervalValid;
         result = S_OK;
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Returns the color that is used by the playback dialog's gannt chart for this
// overlay
HRESULT COverlayCOM::GetGanntChartColor(COLORREF *pColor)
{
   HRESULT result = E_FAIL;
   *pColor = RGB(0, 0, 0);
   try
   {
      if (m_spFvPlaybackTimeInterval != NULL)
      {
         *pColor = m_spFvPlaybackTimeInterval->GanntChartColor;
         result = S_OK;
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// By default an overlay implementing this interface supports
// IFvPlaybackTimeSegment.  However, some generic overlays such as COverlayCOM
// need to dynamically state whether they support IFvPlaybackTimeSegment or not.
BOOL COverlayCOM::SupportsFvPlaybackTimeSegment()
{
   return m_spFvPlaybackTimeInterval != NULL;
}

// Override the display name defined in the overlay's factory.  This is used to
// support per-instance overlay display names (e.g., an overlay created via the
// ILayer::CreateLayer method)
HRESULT COverlayCOM::get_m_displayName(BSTR *pVal)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvOverlayTypeOverrides != NULL)
      {
         *pVal = m_spFvOverlayTypeOverrides->DisplayName.Detach();
         result = S_OK;
      }
      else
      {
         *pVal = _bstr_t("").Detach();
      }
   }
   catch(_com_error &e)
   {
      *pVal = _bstr_t("").Detach();
      REPORT_COM_ERROR(e);
   }
   return result;
}

// This icon will override the icon defined in the overlay's descriptor.  This
// is used to support per-instance overlay icons (e.g., an overlay created via
// the ILayer::CreateLayerEx interface).  This icon is displayed in the overlay
// manager's current list of opened overlays.  The filename can either be the
// full specification or a path relative to HD_DATA\icons.
HRESULT COverlayCOM::get_m_iconFilename(BSTR* pVal)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvOverlayTypeOverrides != NULL)
      {
         *pVal = m_spFvOverlayTypeOverrides->IconFilename.Detach();
         result = S_OK;
      }
      else
      {
         *pVal = _bstr_t("").Detach();
      }
   }
   catch(_com_error &e)
   {
      *pVal = _bstr_t("").Detach();
      REPORT_COM_ERROR(e);
   }
   return result;
}

// The override UserControllable will override the attribute userControllable
// defined in the overlay's descriptor from the overlay config file provided 
// the value is false. There is default value assigned of TRUE becuase it is
// a BOOL. This is used to support per-instance overlay hiding run-time 
// configuration (e.g., an overlay created via the ILayer::CreateLayerEx 
// interface). If FALSE, this will hide overlays in various dialogs and 
// dialog lists.
HRESULT COverlayCOM::get_m_UserControllable(long *pVal)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvOverlayTypeOverrides2 != NULL)
      {
         *pVal = m_spFvOverlayTypeOverrides2->UserControllable;
         result = S_OK;
      }
      else
      {
         *pVal = TRUE;
      }
   }
   catch(_com_error &e)
   {
      *pVal = TRUE;
      REPORT_COM_ERROR(e);
   }
   return result;
}

// By default an overlay implementing this interface supports
// IFvOverlayTypeOverrides.  However, some generic overlays such as COverlayCOM
// need to dynamically state whether they support IFvOverlayTypeOverrides or
// not.
BOOL COverlayCOM::SupportsFvOverlayTypeOverrides()
{
   return m_spFvOverlayTypeOverrides != NULL;
}

// Assuming that initialize has been called, perform some final setup
// operations
void COverlayCOM::PostInitialize()
{
   QueryForInterfaces();
   SetEventSink();

   CMainFrame* pFrame = static_cast<CMainFrame *>(UTL_get_frame());
   if (pFrame)
      SetFrameWnd(pFrame->GetFvFrameWnd());

   SendInitialPlaybackNotifications();

   MapView *view = static_cast<MapView *>(UTL_get_active_non_printing_view());

   // if the overlay implements the IFvOverlayBaseMapRenderer interface, then
   // we'll automatically invalidate the map
   if (m_spFvOverlayBaseMapRenderer != nullptr &&
      view != nullptr)
   {
      view->set_current_map_invalid();
   }

   // if the overlay implements IFvTabularEditable, setup the tabular editor
   if (is_tabular_editable())
   {
      try
      {
         if (m_pFvTabularEditor == nullptr)
         {
            CComObject<CFvTabularEditorImpl>::CreateInstance(&m_pFvTabularEditor);
            m_pFvTabularEditor->AddRef();

            FalconViewTabularEditorLib::IFvTabularEditorDataObjectPtr data_object = m_spFvTabularEditable;

            m_pFvTabularEditor->Initialize(this, data_object);
         }

         m_spFvTabularEditable->SetTabularEditor(m_pFvTabularEditor);
      }
      catch(_com_error &e)
      {
         REPORT_COM_ERROR(e);
      }
   }

   if (view != nullptr &&
      view->IsRendering3d())
   {
      // Synchronize visibility booleans.
      InitializeDisplayElementRootNode();
      ResetDisplayNodeVisibility();
   }
}

// Tabular editor support
// Returns TRUE if this overlay can be edited by the tabular editor
boolean_t COverlayCOM::is_tabular_editable()
{
   boolean_t result = FALSE;

   if (m_spFvTabularEditable != nullptr)
      result = TRUE;

   return result;
}

TabularEditorDataObject* COverlayCOM::GetTabularEditorDataObject()
{
   if (m_pFvTabularEditor != nullptr)
   {
      return m_pFvTabularEditor->GetTabularEditorDataObject();
   }

   return nullptr;
}

FalconViewTabularEditorLib::ITabularEditor* COverlayCOM::GetTabularEditor()
{
   return m_pFvTabularEditor;
}

// CFileOverlayCOM
//

CFileOverlayCOM::CFileOverlayCOM(const char* lpszDisplayName, CLSID clsid) :
COverlayCOM(lpszDisplayName, clsid)
{
}

CFileOverlayCOM::CFileOverlayCOM(const DWORD owning_thread_id,
   FalconViewOverlayLib::IFvOverlay *pFvOverlay) :
   COverlayCOM(owning_thread_id, pFvOverlay)
{
}

// Initialize the overlay.  The read-only property m_overlayDescGuid should be
// set by this method
HRESULT CFileOverlayCOM::InternalInitialize(GUID overlayDescGuid)
{
   HRESULT result = E_FAIL;
   result = COverlayCOM::InternalInitializeHelper(overlayDescGuid);
   if (result == S_OK)
   {
      try
      {
         // query for the required file overlay interface
         QueryForInterfaces();
         if (m_spFvOverlayPersistence == NULL)
         {
            ERR_report("A file overlay must implement the IFvOverlayPersistence"
               " interface");
            result = E_FAIL;
         }
      }
      catch(_com_error &e)
      {
         REPORT_COM_ERROR(e);
         result = E_FAIL;
      }
   }
   return result;
}

void CFileOverlayCOM::QueryForInterfaces()
{
   COverlayCOM::QueryForInterfaces();
   m_spFvOverlayPersistence = m_spFvOverlay;
   m_spFvOverlayPersistence2 = m_spFvOverlay;
}

// Return the file specification of the overlay
HRESULT CFileOverlayCOM::get_m_fileSpecification(BSTR* pFileSpecification)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      *pFileSpecification =
         m_spFvOverlayPersistence->FileSpecification.Detach();
      result = S_OK;
   }
   catch(_com_error &e)
   {
      *pFileSpecification = _bstr_t("").Detach();
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Return the file specification of the overlay
HRESULT CFileOverlayCOM::put_m_fileSpecification(BSTR pFileSpecification)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      m_spFvOverlayPersistence->FileSaveAs(pFileSpecification, -1);
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// True if the file overlay has been persisted.  False otherwise.
HRESULT CFileOverlayCOM::put_m_bHasBeenSaved(long pVal)
{
   HRESULT result = E_FAIL;

   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      m_spFvOverlayPersistence->HasBeenSaved = pVal;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

HRESULT CFileOverlayCOM::get_m_bHasBeenSaved(long* pVal)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      *pVal = m_spFvOverlayPersistence->HasBeenSaved;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// True if the file overlay is dirty and needs to be saved.  False otherwise.
HRESULT CFileOverlayCOM::put_m_bIsDirty(long pVal)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      m_spFvOverlayPersistence->IsDirty = pVal;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

HRESULT CFileOverlayCOM::get_m_bIsDirty(long* pVal)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      *pVal = m_spFvOverlayPersistence->IsDirty;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// True if the file overay is a read-only file overlay
HRESULT CFileOverlayCOM::put_m_bIsReadOnly(long pVal)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      m_spFvOverlayPersistence->IsReadOnly = pVal;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

HRESULT CFileOverlayCOM::get_m_bIsReadOnly(long* pVal)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      *pVal = m_spFvOverlayPersistence->IsReadOnly;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// The overlay has been created from scratch.  The m_fileSpecification property
// should be initialized in this method.
HRESULT CFileOverlayCOM::FileNew()
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      result = m_spFvOverlayPersistence->FileNew();
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Open the overlay with the given file specification
HRESULT CFileOverlayCOM::FileOpen(BSTR fileSpecification)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      result = m_spFvOverlayPersistence->FileOpen(fileSpecification);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Save the overlay to the given file specification and format.  The parameter
// nSaveFormat corresponds to the selected filter in the Save As dialog.  If the
// format is unspecified, then eSaveFileFormat will be zero (save to the
// overlay's default file format in this case).
HRESULT CFileOverlayCOM::FileSaveAs(BSTR fileSpecification, long nSaveFormat)
{
   HRESULT result = E_FAIL;
   try
   {
      ASSERT(m_spFvOverlayPersistence != NULL);
      result = m_spFvOverlayPersistence->FileSaveAs(
         fileSpecification, nSaveFormat);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

HRESULT CFileOverlayCOM::Revert(BSTR fileSpecification)
{
   try
   {
      ASSERT(m_spFvOverlayPersistence2 != NULL);
      return m_spFvOverlayPersistence2->Revert(fileSpecification);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return E_FAIL;
}

BOOL CFileOverlayCOM::SupportsRevertOverride()
{
   return m_spFvOverlayPersistence2 != NULL;
}

// CFvOverlayEventsImpl
//

STDMETHODIMP CFvOverlayEventSink::OnChanged(DISPID dispID)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   switch (dispID)
   {
      // The IsDirty property of the IFvOverlayPersistence interface has changed
   case FalconViewOverlayLib::DISPID_PROP_IS_DIRTY:
      {
         // System wide unique identifier for the update falconview title bar
         static const UINT msg_update_titlebar =
            RegisterWindowMessage("FVW_Update_TitleBar");

         // send a message to the frame, telling it to update the title bar
         CFrameWndEx *frame = UTL_get_frame();
         if (frame != NULL)
            frame->PostMessage(msg_update_titlebar, 0, 0);

         // Notify any observers that the modified flag has changed
         OVL_get_overlay_manager()->OnDirtyFlagChanged(m_pOverlay);
      }
      break;

      // The PlaybackTimeSegment property of the IFvPlaybackTimeSegment
      // interface has changed
   case FalconViewOverlayLib::DISPID_PROP_PLAYBACK_TIME_INTERVAL:
      {
         CMainFrame::GetPlaybackDialog().update_gant_chart();
      }
      break;
   }

   return S_OK;
}

void CDisplayChangeNotifyEventsImpl::SetOverlay(COverlayCOM *pOverlay)
{
   m_pOverlay = pOverlay;
}

STDMETHODIMP CDisplayChangeNotifyEventsImpl::raw_InvalidateOverlay()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (m_pOverlay != NULL)
      OVL_get_overlay_manager()->InvalidateOverlay(m_pOverlay);

   return S_OK;
}

STDMETHODIMP CDisplayChangeNotifyEventsImpl::raw_InvalidateBaseMap()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   MapView *map_view =
      static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (map_view)
   {
      map_view->set_current_map_invalid();
      map_view->invalidate_view();
   }

   return S_OK;
}

STDMETHODIMP CDisplayChangeNotifyEventsImpl::raw_InvalidateVerticalDisplay()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      CVerticalViewDisplay *pDisplay = pFrame->GetVerticalDisplay();
      if (pDisplay != NULL)
      {
         // sets the redraw flag to true (doesn't actually redraw)
         pDisplay->RedrawVerticalDisplay();
         pDisplay->Invalidate();
      }
   }

   return S_OK;
}

// CMapChangeNotifyEventsImpl
//

STDMETHODIMP CMapChangeNotifyEventsImpl::raw_OnRequestMapCenterChange(
   double dLat, double dLon)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CView* pView = UTL_get_active_non_printing_view();
   if (pView != NULL)
   {
      if (UTL_change_view_map_to_best(pView, dLat, dLon) == SUCCESS)
         OVL_get_overlay_manager()->invalidate_all();
   }

   return S_OK;
}

STDMETHODIMP CMapChangeNotifyEventsImpl::raw_OnRequestMapRotationChange(
   double dRotationDegrees)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   MapView* pView =
      static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pView != NULL && pView->get_curr_map() != NULL)
   {
      if (UTL_change_view_map_rotation(pView, dRotationDegrees,
         pView->get_curr_map()->actual_center_lat(),
         pView->get_curr_map()->actual_center_lon()) == SUCCESS)
         OVL_get_overlay_manager()->invalidate_all();
   }

   return S_OK;
}

STDMETHODIMP
   CMapChangeNotifyEventsImpl::raw_OnRequestMapCenterAndRotationChange(
   double dLat, double dLon, double dRotationDegrees)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CView* pView = UTL_get_active_non_printing_view();
   if (pView != NULL)
      if (UTL_change_view_map_rotation(pView, dRotationDegrees, dLat, dLon) ==
         SUCCESS)
         OVL_get_overlay_manager()->invalidate_all();

   return S_OK;
}
