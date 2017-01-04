// Copyright (c) 1994-2009,2013 Georgia Tech Research Corporation, Atlanta, GA
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

//
//

#include "stdafx.h"
#include "OverlayEditorCOM.h"
#include "Common\ComErrorObject.h"
#include "..\getobjpr.h"
#include "err.h"
#include "utils.h"
#include "ovl_mgr.h"

COverlayEditorCOM::COverlayEditorCOM(CLSID classId, _bstr_t displayName) :
m_classId(classId),
m_displayName(displayName),
m_pToolbarEvents(NULL),
m_bCurrentEditor(FALSE)
{

}

COverlayEditorCOM::~COverlayEditorCOM()
{
   if (m_pToolbarEvents != NULL)
   {
      try
      {
         m_pToolbarEvents->RegisterNotifyEvents(false, m_spFvToolbar);
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("Failed unregistering from toolbar events connection point: %s", (char *)e.Description());
         ERR_report(msg);
      }
      m_pToolbarEvents->Release();
   }
}

// Called when the editor mode is activated
HRESULT COverlayEditorCOM::ActivateEditor()
{
   CMainFrame *pFrame = fvw_get_frame();
   HWND hwnd = nullptr;
   FalconViewOverlayLib::IFvFrameWnd2* frame_wnd = nullptr;
   if (pFrame != nullptr)
   {
      hwnd = pFrame->GetSafeHwnd();
      frame_wnd = pFrame->GetFvFrameWnd();
   }

   m_bCurrentEditor = TRUE;

   // If the object has not yet been created, then do so now
   if (m_spFvOverlayEditor == NULL)
   {
      try
      {
         CO_CREATE(m_spFvOverlayEditor, m_classId);

         // query for optional interfaces
         m_spFvOverlayLimitUserInterface = m_spFvOverlayEditor;
         m_spFvOverrideDefaultUserInterface = m_spFvOverlayEditor;
         m_spFvOverlayEditor2 = m_spFvOverlayEditor;

         // create the editor toggle toolbar
         if (pFrame != NULL)
         {
            static UINT nCurrentToolbarCtrlId = IDR_PLUGIN_EDITOR_TOOLBAR_ID;
            m_nToolbarCtrlId = nCurrentToolbarCtrlId++;

            CO_CREATE(m_spFvToolbar, CLSID_FvToolbar);

            long hwnd_long = reinterpret_cast<long>(hwnd);

            m_spFvToolbar->Initialize(hwnd_long, m_nToolbarCtrlId, frame_wnd);

            // Pass the editor type name for possible user simulator
            IFvToolbarSimClientPtr( m_spFvToolbar )->EditorTypeName =
               (LPCTSTR) OVL_get_overlay_manager()->GetCurrentEditorTypeName();

            m_spFvOverlayEditor->EditorToolbar = m_spFvToolbar;

            // sign up for the toolbar events connection point
            CComObject<OverlayEditorToolbarEventSink>::CreateInstance(
               &m_pToolbarEvents);
            m_pToolbarEvents->AddRef();
            m_pToolbarEvents->Initialize(this);
            m_pToolbarEvents->RegisterNotifyEvents(true, m_spFvToolbar);
         }
      }
      catch(_com_error &e)
      {
         REPORT_COM_ERROR(e);
      }
      if ( pFrame != nullptr)
      {
         // this must be done on the UI thread, not the COM thread
         pFrame->RecalcLayout();
      }
   }
   else
   {
      CBasePane* pane = pFrame->GetPane(m_nToolbarCtrlId);
      if (pane != NULL)
         pane->ShowPane(TRUE, FALSE, FALSE);
   }

   HRESULT result = E_FAIL;
   try
   {
      result = m_spFvOverlayEditor->ActivateEditor(GetFvMapView());
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// If the cursor is not set by any overlay, then the current editor's default cursor is used
HRESULT COverlayEditorCOM::GetDefaultCursor(long *hCursor)
{
   HRESULT result = E_FAIL;
   try
   {
      if( m_spFvOverlayEditor != nullptr )
         *hCursor = m_spFvOverlayEditor->DefaultCursor;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Called when the user leaves this editor mode
HRESULT COverlayEditorCOM::DeactivateEditor()
{
   try
   {
      m_bCurrentEditor = FALSE;

      // Hide the editor's toolbar if necessary
      CMainFrame *pFrame = fvw_get_frame();
      if (m_spFvToolbar != NULL && pFrame != NULL)
      {
         CBasePane* pane = pFrame->GetPane(m_nToolbarCtrlId);
         if (pane != NULL)
            pane->ShowPane(FALSE, FALSE, FALSE);
      }

      HRESULT result = E_FAIL;
      FalconViewOverlayLib::IFvMapView* map_view = GetFvMapView();
      result = m_spFvOverlayEditor->DeactivateEditor(map_view);
      return result;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   return E_FAIL;
}

// Return TRUE if the user should not be able to change the projection of the
// map
HRESULT COverlayEditorCOM::get_m_bDisableProjectionUI(long* pVal)
{
   HRESULT result = E_FAIL;
   *pVal = 0;
   try
   {
      if (m_spFvOverlayLimitUserInterface != NULL)
         *pVal = m_spFvOverlayLimitUserInterface->DisableProjectionUI;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

   // Return TRUE if the user should not be able to rotate the map while the editor is active
HRESULT COverlayEditorCOM::get_m_bDisableRotationUI(long* pVal)
{
   HRESULT result = E_FAIL;
   *pVal = 0;
   try
   {
      if (m_spFvOverlayLimitUserInterface != NULL)
         *pVal = m_spFvOverlayLimitUserInterface->DisableRotationUI;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Return TRUE if the map must be North-Up
HRESULT COverlayEditorCOM::get_m_bRequiresNorthUp(long* pVal)
{
   HRESULT result = E_FAIL;
   *pVal = 0;
   try
   {
      if (m_spFvOverlayLimitUserInterface != NULL)
         *pVal = m_spFvOverlayLimitUserInterface->RequiresNorthUp;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Return TRUE if the map must be projected with the Equal Arc projection when this editor is active
HRESULT COverlayEditorCOM::get_m_bRequiresEqualArc(long* pVal)
{
   HRESULT result = E_FAIL;
   *pVal = 0;
   try
   {
      if (m_spFvOverlayLimitUserInterface != NULL)
         *pVal = m_spFvOverlayLimitUserInterface->RequiresEqualArc;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Return TRUE if the order of overlays associated with this editor cannot be changed
HRESULT COverlayEditorCOM::get_m_bDisableOverlayOrdering(long* pVal)
{
   HRESULT result = E_FAIL;
   *pVal = 0;
   try
   {
      if (m_spFvOverlayLimitUserInterface != NULL)
         *pVal = m_spFvOverlayLimitUserInterface->DisableOverlayOrdering;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}


// Return TRUE if FalconView should activate the editor when a static overlay is toggled on or when the user chooses File | New
// for a file overlay.  Return FALSE otherwise.  FalconView's default behavior is TRUE.
HRESULT COverlayEditorCOM::get_m_bAutoEnterOverlayEditor(long* pVal)
{
   HRESULT result = E_FAIL;
   *pVal = TRUE;
   try
   {
      if (m_spFvOverrideDefaultUserInterface != NULL)
         *pVal = m_spFvOverrideDefaultUserInterface->AutoEnterOverlayEditor;
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

// Normally, FalconView will use the overlay type's icon as the editor toolbar button.  The following method can be
// used to override the editor toolbar button's image.  Return an empty string if you would like to preserve the default behavior.
HRESULT COverlayEditorCOM::get_m_bEditorToolbarButtonBitmap(BSTR* pEditorToolbarButtonBmp)
{
   HRESULT result = E_FAIL;
   try
   {
      if (m_spFvOverrideDefaultUserInterface != NULL)
         *pEditorToolbarButtonBmp = m_spFvOverrideDefaultUserInterface->EditorToolbarButtonBitmap.Detach();
      else
         *pEditorToolbarButtonBmp = _bstr_t("").Detach();
      result = S_OK;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   return result;
}

void COverlayEditorCOM::OnToolbarButtonPressed(long lButtonId)
{
   FalconViewOverlayLib::IFvMapView* map_view = GetFvMapView();
   try
   {
      m_spFvOverlayEditor->OnToolbarButtonClick(map_view, lButtonId);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

void COverlayEditorCOM::OnToolbarClosed()
{
   // if the toolbar was closed while we are still the current editor, then we will automatically change
   // the editor mode to none
   if (m_bCurrentEditor)
      OVL_get_overlay_manager()->set_mode(GUID_NULL);
}

void COverlayEditorCOM::OnCustomizeToolbar()
{
   try
   {
      if (m_spFvOverlayEditor2 != NULL)
         m_spFvOverlayEditor2->OnCustomizeToolbar();
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

// helper methods
//

FalconViewOverlayLib::IFvMapView* COverlayEditorCOM::GetFvMapView()
{
   MapView *pMapView = dynamic_cast<MapView *>(UTL_get_active_non_printing_view());
   return pMapView == NULL ? NULL : pMapView->GetFvMapView();
}
