// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// FvOverlayManagerImpl.cpp
//

#include "stdafx.h"
#include "FvOverlayManagerImpl.h"
#include "OverlayCOM.h"
#include "err.h"
#include "ovl_mgr.h"
#include "refresh.h"    // for FVW_update_title_bar

CFvOverlayManagerImpl::CFvOverlayManagerImpl()
{
   OVL_get_overlay_manager()->RegisterEvents(this);
}

CFvOverlayManagerImpl::~CFvOverlayManagerImpl()
{
   OVL_get_overlay_manager()->UnregisterEvents(this);
}

// Overlay creation and deletion
//

// toggles on/off a static overlay with the given overlay descriptor guid.  Returns a pointer to the 
// overlay if the overlay was toggled on.  Returns NULL otherwise.  Note that if the overlay does not implement
// IFvOverlay then the overlay will still be toggled, however ppOverlay will be NULL.
STDMETHODIMP CFvOverlayManagerImpl::raw_ToggleStaticOverlay(GUID overlayDescGuid, FalconViewOverlayLib::IFvOverlay **ppOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (OVL_get_overlay_manager()->toggle_static_overlay(overlayDescGuid) != SUCCESS)
   {
      ERR_report("toggle_static_overlay failed");
      return E_FAIL;
   }

   COverlayCOM *pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->get_first_of_type(overlayDescGuid));
   if (pOverlay != NULL)
   {
      *ppOverlay = pOverlay->GetFvOverlay();
      (*ppOverlay)->AddRef();
   }
   else
      *ppOverlay = NULL;

   return S_OK;
}

// open a file overlay of the given overlay type and filename.  Returns a pointer to the overlay
// opened if successful.  Note that if the overlay does not implement IFvOverlay then the overlay 
// will still be toggled, however ppOverlay will be NULL.
STDMETHODIMP CFvOverlayManagerImpl::raw_OpenFileOverlay(GUID overlayDescGuid, BSTR fileName, FalconViewOverlayLib::IFvOverlay ** ppOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   C_overlay *ret_overlay;
   if (OVL_get_overlay_manager()->OpenFileOverlay(overlayDescGuid, _bstr_t(fileName), ret_overlay) != SUCCESS)
   {
      ERR_report("OpenFileOverlay failed");
      return E_FAIL;
   }

   COverlayCOM *pOverlay = dynamic_cast<COverlayCOM *>(ret_overlay);
   if (pOverlay != NULL)
   {
      *ppOverlay = pOverlay->GetFvOverlay();
      (*ppOverlay)->AddRef();
   }
   else
      *ppOverlay = NULL;

   return S_OK;
}

// create a new instance of an overlay of the given type.  Returns a pointer to the overlay created
// if successful.  Note that if the overlay does not implement IFvOverlay then the overlay will still be 
// toggled, however ppOverlay will be NULL.
STDMETHODIMP CFvOverlayManagerImpl::raw_CreateOverlay(GUID overlayDescGuid, FalconViewOverlayLib::IFvOverlay **ppOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   C_overlay *ret_overlay;
   if (OVL_get_overlay_manager()->create(overlayDescGuid, &ret_overlay) != SUCCESS)
   {
      ERR_report("create failed");
      return E_FAIL;
   }

   COverlayCOM *pOverlay = dynamic_cast<COverlayCOM *>(ret_overlay);
   if (pOverlay != NULL)
   {
      *ppOverlay = pOverlay->GetFvOverlay();
      (*ppOverlay)->AddRef();
   }
   else
      *ppOverlay = NULL;

   return S_OK;
}

// Active Editor

STDMETHODIMP CFvOverlayManagerImpl::raw_SetCurrentEditor(GUID overlayDescGuid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   OVL_get_overlay_manager()->set_mode(overlayDescGuid);

   return S_OK;
}

// retrieves the overlay descriptor guid of the active editor's overlay type, GUID_NULL for none
STDMETHODIMP CFvOverlayManagerImpl::get_CurrentEditor(GUID *pOverlayDescGuid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *pOverlayDescGuid = OVL_get_overlay_manager()->GetCurrentEditor();
   return S_OK;
}

// Enumeration
//

// SelectAll will select all overlays in the overlay stack.  Use MoveNext to enumerate through the 
// overlays (from top of the stack to the bottom of the stack).  pbResult will be TRUE if there is at
// least one overlay in the stack, FALSE otherwise.  Use the CurrentOverlay property to retrieve the overlay.
STDMETHODIMP CFvOverlayManagerImpl::raw_SelectAll(long *pbResult)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_pCurrentOverlay = OVL_get_overlay_manager()->get_first();
   *pbResult = m_pCurrentOverlay != NULL;
   m_eEnumeratorType = SELECT_ALL;
   return S_OK;
}

// SelectByOverlayDescGuid will select all overlays in the stack of the given overlay type.  Use MoveNext to enumerate
// through the overlays (from top of the stack to bottom of the stack).  pbResult will be TRUE if CurrentOverlay points to the next overlay, FALSE otherwise.
// 
STDMETHODIMP CFvOverlayManagerImpl::raw_SelectByOverlayDescGuid(GUID overlayDescGuid, long *pbResult)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_pCurrentOverlay = OVL_get_overlay_manager()->get_first_of_type(overlayDescGuid);
   *pbResult = m_pCurrentOverlay != NULL;
   m_eEnumeratorType = SELECT_BY_OVERLAY_TYPE;
   m_overlayDescGuid = overlayDescGuid;
   return S_OK;
}

STDMETHODIMP CFvOverlayManagerImpl::raw_MoveNext(long *pbResult)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   switch(m_eEnumeratorType)
   {
   case SELECT_ALL:
      m_pCurrentOverlay = OVL_get_overlay_manager()->get_next(m_pCurrentOverlay);
      break;
   case SELECT_BY_OVERLAY_TYPE:
      m_pCurrentOverlay = OVL_get_overlay_manager()->get_next_of_type(m_pCurrentOverlay, m_overlayDescGuid);
      break;
   }
   *pbResult = m_pCurrentOverlay != NULL;
   return S_OK;
}

STDMETHODIMP CFvOverlayManagerImpl::get_CurrentOverlay(FalconViewOverlayLib::IFvOverlay **ppOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   COverlayCOM *pOverlayCOM = dynamic_cast<COverlayCOM *>(m_pCurrentOverlay);
   if (pOverlayCOM != NULL)
   {
      *ppOverlay = pOverlayCOM->GetFvOverlay();
      (*ppOverlay)->AddRef();
   }
   else
   {
       // query for the IFvOverlay interface.  Some internal overlays implement this
      FalconViewOverlayLib::IFvOverlay* p = dynamic_cast<FalconViewOverlayLib::IFvOverlay*>(m_pCurrentOverlay);
      if (p != NULL)
      {
         p->AddRef();
         (*ppOverlay) = p;
      }
      else
         *ppOverlay = NULL;
   }

   return S_OK;
}

// Manipulation of overlay stack
//

// Adds an existing, already Initialized overlay to the overlay stack
STDMETHODIMP CFvOverlayManagerImpl::raw_AddOverlayToStack(
   FalconViewOverlayLib::IFvOverlay *pOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   try
   {
      // the given overlay cannot be NULL
      if (pOverlay == NULL)
      {
         ERR_report("CFvOverlayStackEvents::OnOverlayAdded failed - pOverlay "
            "parameter invalid, cannot be NULL");
         return E_FAIL;
      }

      COverlayCOM *pOverlayCOM = NULL;

      FalconViewOverlayLib::IFvOverlayPtr spOverlay = pOverlay;
      if (spOverlay == NULL)
      {
         ERR_report("The given overlay must implement IFvOverlay");
         return E_FAIL;
      }

      // determine if the overlay is a file overlay or not and create the
      // appropriate wrapper
      FalconViewOverlayLib::IFvOverlayPersistencePtr spOverlayPersistence =
         spOverlay;
      if (spOverlayPersistence != NULL)
      {
         pOverlayCOM = new CFileOverlayCOM(::GetCurrentThreadId(), spOverlay);
      }
      else
      {
         pOverlayCOM = new COverlayCOM(::GetCurrentThreadId(), spOverlay);
      }

      pOverlayCOM->PostInitialize();

      if (OVL_get_overlay_manager()->add_overlay(pOverlayCOM) != SUCCESS)
      {
         ERR_report("CFvOverlayStackEvents::OnOverlayAdded - "
            "C_ovl_mgr::add_overlay failed");
         return E_FAIL;
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
      return E_FAIL;
   }

   return S_OK;
}

// Removes the given overlay from the overlay stack
STDMETHODIMP CFvOverlayManagerImpl::raw_RemoveOverlayFromStack(FalconViewOverlayLib::IFvOverlay *pOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   try
   {
      // the given overlay cannot be NULL
      if (pOverlay == NULL)
      {
         ERR_report("CFvOverlayStackEvents::OnOverlayRemoved failed - pOverlay parameter invalid, cannot be NULL");
         return E_FAIL;
      }

      // find the COverlayCOM instance corresponding to the given COM object
      C_overlay *pOverlayCOM = OVL_get_overlay_manager()->FindOverlay(pOverlay);
      if (pOverlayCOM != NULL)
      {
         if (OVL_get_overlay_manager()->delete_overlay(pOverlayCOM) != SUCCESS)
         {
            ERR_report("CFvOverlayStackEvents::OnOverlayRemoved - C_ovl_mgr::delete_overlay failed");
            return E_FAIL;
         }
         pOverlayCOM->Release();
         return S_OK;
      }
      else
      {
         ERR_report("Unable to find the given overlay in the overlay stack");
         return E_FAIL;
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
      return E_FAIL;
   }

   return S_OK;
}

// MoveOverlayAbove will move one overlay above another one.  pInsertAboveOverlay can be NULL in which
// case pOverlayToMove will move to the top of the overlay stack
STDMETHODIMP CFvOverlayManagerImpl::raw_MoveOverlayAbove(FalconViewOverlayLib::IFvOverlay *pOverlayToMove, FalconViewOverlayLib::IFvOverlay *pInsertAboveOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   C_overlay *pOverlayCOMToMove = OVL_get_overlay_manager()->FindOverlay(pOverlayToMove);
   if (pOverlayCOMToMove == NULL)
   {
      ERR_report("Unable to find pOverlayToMove in overlay stack");
      return E_FAIL;
   }


   if (pInsertAboveOverlay == NULL)
   {
      OVL_get_overlay_manager()->make_current(pOverlayCOMToMove);
   }
   else
   {
      C_overlay *pOverlayCOMInsertAbove = OVL_get_overlay_manager()->FindOverlay(pInsertAboveOverlay);
      if (pOverlayCOMInsertAbove == NULL)
      {
         ERR_report("Unable to find pInsertAboveOverlay in overlay stack");
         return E_FAIL;
      }
      OVL_get_overlay_manager()->move_above(pOverlayCOMToMove, pOverlayCOMInsertAbove);

      FVW_update_title_bar();
   }

   return S_OK;
}

// MoveOverlayBelow will move one overlay below another one.  pInsertBelowOverlay can be NULL in which
// case pOverlayToMove will move to the bottom of the overlay stack
STDMETHODIMP CFvOverlayManagerImpl::raw_MoveOverlayBelow(FalconViewOverlayLib::IFvOverlay *pOverlayToMove, FalconViewOverlayLib::IFvOverlay *pInsertBelowOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   C_overlay *pOverlayCOMToMove = OVL_get_overlay_manager()->FindOverlay(pOverlayToMove);
   if (pOverlayCOMToMove == NULL)
   {
      ERR_report("Unable to find pOverlayToMove in overlay stack");
      return E_FAIL;
   }

   if (pInsertBelowOverlay == NULL)
   {
      OVL_get_overlay_manager()->move_to_bottom(pOverlayCOMToMove);
   }
   else
   {
      C_overlay *pOverlayCOMInsertBelow = OVL_get_overlay_manager()->FindOverlay(pInsertBelowOverlay);
      if (pOverlayCOMInsertBelow == NULL)
      {
         ERR_report("Unable to find pInsertBelowOverlay in overlay stack");
         return E_FAIL;
      }
      OVL_get_overlay_manager()->move_below(pOverlayCOMToMove, pOverlayCOMInsertBelow);
   }

   FVW_update_title_bar();

   return S_OK;
}

STDMETHODIMP CFvOverlayManagerImpl::raw_IsOverlayTypeEnabled(GUID overlayDescGuid, long *pbEnabled)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *pbEnabled = OVL_get_type_descriptor_list()->IsOverlayEnabled(overlayDescGuid);
   return S_OK;
}

// Show the overlay options dialog box starting at the page associated with the given overlay type descriptor
// Note: overlayDescGuid can be GUID_NULL in which case the overlay options dialog box is opened up
// to the first page.
STDMETHODIMP CFvOverlayManagerImpl::raw_ShowOverlayOptionsDialog(GUID overlayDescGuid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   OVL_get_overlay_manager()->overlay_options_dialog(overlayDescGuid);
   return S_OK;
}

STDMETHODIMP CFvOverlayManagerImpl::raw_CloseOverlay(FalconViewOverlayLib::IFvOverlay* pOverlayToClose)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   C_overlay *overlay = OVL_get_overlay_manager()->FindOverlay(pOverlayToClose);
   if (overlay)
      OVL_get_overlay_manager()->close(overlay);

   return S_OK;
}

STDMETHODIMP CFvOverlayManagerImpl::raw_GetOverlayHandle(FalconViewOverlayLib::IFvOverlay* pOverlay, long *pHandle)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   C_overlay *overlay = OVL_get_overlay_manager()->FindOverlay(pOverlay);
   if (overlay == nullptr)
      return E_FAIL;

   *pHandle = OVL_get_overlay_manager()->get_overlay_handle(overlay);
   if (*pHandle == -1)
      return E_FAIL;

   return S_OK;
}

STDMETHODIMP CFvOverlayManagerImpl::raw_GetOverlayForHandle(long handle, FalconViewOverlayLib::IFvOverlay** ppOverlay)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(handle);
   if (overlay == nullptr)
      return E_FAIL;

   COverlayCOM *pOverlayCOM = dynamic_cast<COverlayCOM *>(overlay);
   if (pOverlayCOM != NULL)
   {
      *ppOverlay = pOverlayCOM->GetFvOverlay();
      (*ppOverlay)->AddRef();
   }
   else
   {
      // query for the IFvOverlay interface.  Some internal overlays implement this
      FalconViewOverlayLib::IFvOverlay* p = dynamic_cast<FalconViewOverlayLib::IFvOverlay*>(overlay);
      if (p != NULL)
      {
         p->AddRef();
         (*ppOverlay) = p;
      }
      else
      {
         *ppOverlay = NULL;
         return E_FAIL;
      }
   }
       
   return S_OK;
}

// OverlayStackChangedObserver_Interface
//
void CFvOverlayManagerImpl::OverlayAdded(C_overlay* overlay)
{
   Fire_OnOverlayOpened(overlay->GetFvOverlay()); 
}

void CFvOverlayManagerImpl::OverlayRemoved(C_overlay* overlay)
{
   Fire_OnOverlayClosed(overlay->GetFvOverlay()); 
}

void CFvOverlayManagerImpl::OverlayOrderChanged()
{
   Fire_OnOverlayOrderChanged(); 
}

void CFvOverlayManagerImpl::OverlayFileSpecificationChanged(C_overlay* overlay)
{
   OverlayPersistence_Interface *pFvOverlayPersistence = 
      dynamic_cast<OverlayPersistence_Interface *>(overlay);
   if (pFvOverlayPersistence)
   {
      _bstr_t filespec;
      pFvOverlayPersistence->get_m_fileSpecification(filespec.GetAddress());
      Fire_OnFileSpecificationChanged(overlay->GetFvOverlay(), 
         filespec);
   }
}

void CFvOverlayManagerImpl::OverlayDirtyChanged(C_overlay* overlay)
{
   OverlayPersistence_Interface *pFvOverlayPersistence = 
      dynamic_cast<OverlayPersistence_Interface *>(overlay);
   if (pFvOverlayPersistence)
   {
      long is_dirty;
      pFvOverlayPersistence->get_m_bIsDirty(&is_dirty);
      Fire_OnIsDirtyChanged(overlay->GetFvOverlay(), is_dirty);
   }
}