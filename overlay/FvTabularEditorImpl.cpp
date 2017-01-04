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
#include "FvTabularEditorImpl.h"
#include "TabularEditorDlg.h"
#include "FalconView/overlay/FvOverlayDataObjectImpl.h"
#include "FalconView/overlay/MultiLocationDlgImpl.h"
#include "overlay.h"


CFvTabularEditorImpl::CFvTabularEditorImpl()
   : m_overlay(nullptr),
     m_pFvOverlayDataObject(nullptr),
     m_pMultiLocationDlg(nullptr),
	  m_pCallback(nullptr)
{
}

CFvTabularEditorImpl::~CFvTabularEditorImpl()
{
	if (m_pFvOverlayDataObject != nullptr)
		m_pFvOverlayDataObject->Release();

   if (m_pMultiLocationDlg != nullptr)
		m_pMultiLocationDlg->Release();

	m_pCallback = nullptr;
}

void CFvTabularEditorImpl::Initialize(C_overlay* overlay, 
   FalconViewTabularEditorLib::IFvTabularEditorDataObject* client_data_object)
{
   m_overlay = overlay;

   if (m_pFvOverlayDataObject == nullptr)
   {
      CComObject<CFvOverlayDataObjectImpl>::CreateInstance(&m_pFvOverlayDataObject);
      m_pFvOverlayDataObject->AddRef();

      m_pFvOverlayDataObject->Initialize(overlay, client_data_object);
   }
}

TabularEditorDataObject* CFvTabularEditorImpl::GetTabularEditorDataObject()
{
   TabularEditorDataObject* data_object = nullptr;

   if (m_pFvOverlayDataObject != nullptr)
   {
      data_object = m_pFvOverlayDataObject->GetDatObject();
   }

   return data_object;
}

STDMETHODIMP CFvTabularEditorImpl::raw_ToggleTabularEditor()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

	if (g_tabular_editor_dlg != nullptr) 
      CTabularEditorDlg::destroy_dialog();
   else
		CTabularEditorDlg::create_dialog(m_overlay->GetTabularEditorDataObject(), this); 

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_SetDataObject(long force_refresh)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (g_tabular_editor_dlg != nullptr)
   {
      g_tabular_editor_dlg->set_data_object(m_overlay->GetTabularEditorDataObject(), force_refresh);
   }

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_IsDataObjectSet(long* set)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (g_tabular_editor_dlg != nullptr && 
       g_tabular_editor_dlg->get_data_object() == m_overlay->GetTabularEditorDataObject() )
      *set = TRUE;
   else
      *set = FALSE;

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_OnSelectionChanged()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (g_tabular_editor_dlg != nullptr)
      g_tabular_editor_dlg->on_selection_changed();

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_UpdateData(long update)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (g_tabular_editor_dlg != nullptr)
      g_tabular_editor_dlg->UpdateData(update);

   return S_OK;
}


STDMETHODIMP CFvTabularEditorImpl::raw_IsVisible(long* visible)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (g_tabular_editor_dlg != nullptr)
      *visible = TRUE;
   else
      *visible = FALSE;

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_SetEnableRendering(long enable_rendering)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (g_tabular_editor_dlg != nullptr)
   {
      g_tabular_editor_dlg->set_paint_mode(enable_rendering);
   }
   
   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_SetTopRow(long top_row)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (g_tabular_editor_dlg != nullptr)
   {
      g_tabular_editor_dlg->set_top_row(top_row);
   }

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_GetTopRow(long* top_row)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (g_tabular_editor_dlg != nullptr)
   {
      *top_row = g_tabular_editor_dlg->get_top_row();
   }

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_GetBaseTabularDataObject(FalconViewTabularEditorLib::IFvBaseTabularEditorDataObject** pBaseTabularEditorDataObject)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

      *pBaseTabularEditorDataObject = m_pFvOverlayDataObject;

      m_pFvOverlayDataObject->AddRef();

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_OnDialogClosed()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

	// notify registered callback
	if (m_pCallback != nullptr)
		m_pCallback->OnTabularEditorDialogClosed();

   return S_OK;
}

STDMETHODIMP CFvTabularEditorImpl::raw_SetTabularEditorCallback(FalconViewTabularEditorLib::ITabularEditorCallback* pCallback)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

	m_pCallback = pCallback;
	
   return S_OK;
}


STDMETHODIMP CFvTabularEditorImpl::get_MultiLocationDlg(IDispatch** dlg)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (m_pMultiLocationDlg == nullptr)
   {
      CComObject<CMultiLocationDlgImpl>::CreateInstance(&m_pMultiLocationDlg);
      m_pMultiLocationDlg->AddRef();
   }

   *dlg = m_pMultiLocationDlg;
   m_pMultiLocationDlg->AddRef();

   return S_OK;
}
