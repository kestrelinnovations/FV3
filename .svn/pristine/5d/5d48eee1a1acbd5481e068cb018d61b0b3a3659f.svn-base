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
#include "param.h"   // for PRM_
#include "layerovl.h"

//----------------------------------------------------------------------------
// CLayerOvlFactory / CLayerOvlFactorEditor
//----------------------------------------------------------------------------

CLayerOvlFactory::CLayerOvlFactory(int editor_num, CString class_ID_string, int interface_version)
{
   // the name is based on the given editor number
   m_editor_num = editor_num;

   m_layer_editor = new LayerEditorImpl(interface_version);
   HRESULT hresult = m_layer_editor->CreateInstance(class_ID_string);

   if (m_layer_editor->interface_OK())
   {
      _bstr_t bstr, bstr2;
      m_layer_editor->GetEditorStrings(bstr.GetAddress(), bstr2.GetAddress());

      m_strDisplayName = CString((char *)bstr);

      _bstr_t bstr3;
      m_layer_editor->GetIconName(bstr3.GetAddress());

      m_icon_file = CString((char *)bstr3);

      if (interface_version >= 2)
      {
         _bstr_t bstr4;
         m_layer_editor->GetPropertiesProgID(bstr4.GetAddress());
         m_properties_progID = CString((char *)bstr4);
      }

      m_overlayDescGuid = m_layer_editor->GetClassId();

      // There can be two different plug-in overlays that use the same object for creation.  We need to
      // insure that that overlay descriptor's guid is unique.  This will no longer be required when the
      // overlay descriptor guid is separated from the factory/editor/overlay objects
      m_overlayDescGuid.Data1 += editor_num;
   }
}

HRESULT CLayerOvlFactory::CreateOverlayInstance(C_overlay **ppOverlay)
{
   if (m_layer_editor == NULL)
      return NULL;

   // get the IDispatch pointer from the editor
   VARIANT variant;
   variant.vt = VT_DISPATCH;
   variant.pdispVal =  NULL;

   m_layer_editor->GetDispatchPtr(&variant);

   // get the icon name from the editor, relative to the FalconView icon
   // directory
   _bstr_t icon_name;
   m_layer_editor->GetIconName(icon_name.GetAddress());

   // create a new CLayerOvl with the new parameters
   *ppOverlay = CreateOverlay(::GetCurrentThreadId(), "", variant.pdispVal,
      (char *)icon_name, m_layer_editor);

   return S_OK;
}

CLayerOvlFactory::~CLayerOvlFactory()
{
   try
   {
      if (m_layer_editor != NULL && m_layer_editor->interface_OK())
         m_layer_editor->Release();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed releasing interface for editor #%d: Reason - %s", m_editor_num, (char *)e.Description());
      ERR_report(msg);
   }

   delete m_layer_editor;
}

// ActiveX support

// on_help - implement this if your overlay uses an ActiveX control for
// the overlay options
void CLayerOvlFactory::on_help()
{
   if (m_layer_editor != NULL)
      m_layer_editor->OnPropertiesHelp();
}

CString CLayerOvlFactory::get_prog_ID()
{
   return m_properties_progID;
}

CString CLayerOvlFactory::get_preference_string()
{
   if (m_layer_editor != NULL)
   {
      _bstr_t preference;

      if (SUCCEEDED(m_layer_editor->GetPreferenceString(preference.GetAddress())))
         return CString((char *)preference);
   }

   return "";
}

void CLayerOvlFactory::set_preference_string(CString pref)
{
   if (m_layer_editor != NULL)
   {
      const int length = pref.GetLength();
      char *buffer = pref.GetBuffer(length);
      bstr_t bstr(buffer);

      if (!SUCCEEDED(m_layer_editor->SetPreferenceString(bstr)))
      {
         ERR_report("SetPreferenceString failed");
      }
   }
}

C_overlay *CLayerOvlFactory::CreateOverlay(const DWORD owning_thread_id,
   CString name, IDispatch *pdisp, CString icon_name,
   LayerEditorImpl *layer_editor)
{
   CComObject<CStaticLayerOvl> *overlay;
   CComObject<CStaticLayerOvl>::CreateInstance(&overlay);
   overlay->AddRef();
   overlay->Setup(owning_thread_id, "", pdisp, icon_name, layer_editor);

   return overlay;
}

C_overlay *CLayerFileOvlFactory::CreateOverlay(const DWORD owning_thread_id,
   CString name, IDispatch *pdisp, CString icon_name,
   LayerEditorImpl *layer_editor)
{
   CComObject<CLayerOvl> *overlay;
   CComObject<CLayerOvl>::CreateInstance(&overlay);
   overlay->AddRef();
   overlay->Setup(owning_thread_id, name, pdisp, icon_name, layer_editor);

   return overlay;
}

CLayerFileOvlFactory::CLayerFileOvlFactory(int editor_num, CString class_ID_string, int interface_version) :
      CLayerOvlFactory(editor_num, class_ID_string, interface_version)
{
   // get the default extension
   if (m_layer_editor->interface_OK())
   {
      _bstr_t extension;
      if (SUCCEEDED(m_layer_editor->GetDefaultExtension(-1, extension.GetAddress())))
      {
         m_default_extension = CString((char *)extension);
         m_default_extension = "." + m_default_extension;
      }
   }
}

// Get the default directory where files for this overlay type are stored
HRESULT CLayerFileOvlFactory::GetDefaultDirectory(BSTR* pbstrDefaultDir)
{
   if (m_layer_editor != NULL)
      return m_layer_editor->GetDefaultDirectory(-1, pbstrDefaultDir);

   return E_FAIL;
}

// Get the default file extension of files for this overlay type
HRESULT CLayerFileOvlFactory::GetDefaultFileExtension(BSTR* pbstrDefaultExt)
{
   *pbstrDefaultExt = _bstr_t(m_default_extension).Detach();
   return S_OK;
}

// Get the filter used in the common file dialog boxes (open/save).  Return one or more pairs of '|' separated
// filter strings.  The first string in each pair is a display string that describes the filter, and the second
// string specifies the filter pattern (for example, "*.rte").  Note that you can return a different filter for 
// the Open vs. Save As dialogs
HRESULT CLayerFileOvlFactory::GetFileDialogFilter(long bIsFileOpenDialog, BSTR* pbstrFilter)
{
   if (m_layer_editor != NULL)
   {
      int layer_handle = - 1;
      C_overlay * overlay = OVL_get_overlay_manager()->get_first_of_type(m_overlayDescGuid);

      if (overlay != NULL)
         layer_handle = OVL_get_overlay_manager()->get_overlay_handle(overlay);
      
      try
      {
         return m_layer_editor->GetDefaultFilter(layer_handle, pbstrFilter);
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::GetDefaultFilter failed");
         m_layer_editor->Release();
      }
   }

   *pbstrFilter = _bstr_t("").Detach();
   return E_FAIL;
}

CLayerOvlEditor::CLayerOvlEditor(LayerEditorImpl *pLayerEditor) :
   m_layer_editor(pLayerEditor)
{
   if (m_layer_editor != NULL && m_layer_editor->interface_OK())
   {
      try
      {
         // obtain the editor name from the editor object.  This
         // name is used in the tooltip for the editor toolbar button
         // and is listed in the Tools menu.
         _bstr_t bstr;
         if (SUCCEEDED(m_layer_editor->GetEditorName(bstr.GetAddress())))
            m_strEditorDisplayName = CString((char *)bstr);

         // obtain the editor toolbar button bitmap's filename
         _bstr_t bstr2;
         if (SUCCEEDED(m_layer_editor->GetEditorToolbarButton(bstr2.GetAddress())))
            m_bitmap_filename = CString((char *)bstr2);
      }
      catch(_com_error &)
      {
         // If GetEditorName or GetEditorToolbarButton fail, then this editor will
         // not be registered
         m_layer_editor->Release();
      }
   }
}

HCURSOR CLayerOvlEditor::get_default_cursor()
{
   if (m_layer_editor != NULL)
   {
      long cursor;

      try
      {
         if (SUCCEEDED(m_layer_editor->GetDefaultCursor(&cursor)))
            return (HCURSOR)cursor;
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::GetDefaultCursor failed");
      }
   }

   return NULL;
}

int CLayerOvlEditor::set_edit_on(boolean_t edit_on)
{
   if (m_layer_editor != NULL)
   {
      try
      {
         if (SUCCEEDED(m_layer_editor->SetEditOn(edit_on)))
            return SUCCESS;
      }
      catch(_com_error e)
      {
         ERR_report("ILayerEditor::SetEditOn failed");
      }
   }

   return FAILURE;
}

CLayerOvlEditor::~CLayerOvlEditor()
{
}

// IFvOverlayLimitUserInterface

// Return TRUE if the user should not be able to change the projection of the map
HRESULT CLayerOvlEditor::get_m_bDisableProjectionUI(long* pVal)
{
   *pVal = FALSE;

   try
   {
      short ret;
      m_layer_editor->DisableProjectionUI(-1, &ret);
      *pVal = ret;
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("ILayerEditor::DisableProjectionUI failed: %s", (char *)e.Description());
      ERR_report(msg);
   }

   return S_OK;
}

// Return TRUE if the user should not be able to rotate the map while the editor is active
HRESULT CLayerOvlEditor::get_m_bDisableRotationUI(long* pVal)
{
   *pVal = FALSE;

   try
   {
      short ret;
      m_layer_editor->DisableRotationUI(-1, &ret);
      *pVal = ret;
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("ILayerEditor::DisableRotationUI failed: %s", (char *)e.Description());
      ERR_report(msg);
   }

   return S_OK;
}

// Return TRUE if the map must be North-Up
HRESULT CLayerOvlEditor::get_m_bRequiresNorthUp(long* pVal)
{
   *pVal = FALSE;

   try
   {
      short ret;
      m_layer_editor->RequiresNorthUp(-1, &ret);
      *pVal = ret;
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("ILayerEditor::RequiresNorthUp failed: %s", (char *)e.Description());
      ERR_report(msg);
   }

   return S_OK;
}

// Return TRUE if the map must be projected with the Equal Arc projection when this editor is active
HRESULT CLayerOvlEditor::get_m_bRequiresEqualArc(long* pVal)
{
   *pVal = FALSE;

   try
   {
      short ret;
      m_layer_editor->RequiresEqualArc(-1, &ret);
      *pVal = ret;
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("ILayerEditor::RequiresEqualArc failed: %s", (char *)e.Description());
      ERR_report(msg);
   }

   return S_OK;
}

// Return TRUE if the order of overlays associated with this editor cannot be changed
HRESULT CLayerOvlEditor::get_m_bDisableOverlayOrdering(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if FalconView should activate the editor when a static overlay is toggled on or when the user chooses File | New
// for a file overlay.  Return FALSE otherwise.  FalconView's default behavior is TRUE.
HRESULT CLayerOvlEditor::get_m_bAutoEnterOverlayEditor(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}

// Normally, FalconView will use the overlay type's icon as the editor toolbar button.  The following method can be
// used to override the editor toolbar button's image.  Return an empty string if you would like to preserve the default behavior.
HRESULT CLayerOvlEditor::get_m_bEditorToolbarButtonBitmap(BSTR* pEditorToolbarButtonBmp)
{
   *pEditorToolbarButtonBmp = _bstr_t(m_bitmap_filename).Detach();
   return S_OK;
}

HRESULT LayerEditorImpl::CreateInstance(CString class_ID_string)
{
   HRESULT hresult = E_FAIL;

   // obtain the classID from the class ID string
   USES_CONVERSION;
   if (CLSIDFromString((LPOLESTR) T2COLE(class_ID_string),
      &m_classId) != S_OK)
   {
      CString msg;
      msg.Format("Unable to get the 3rd party Plug-in's external interface's class id from class id string = %s",
         class_ID_string);
      ERR_report(msg);
   }
   
   // ILayerEditor
   if (m_interface_version == 1)
   {
      hresult = CoCreateInstance(m_classId, NULL, CLSCTX_ALL, 
         fvw::IID_ILayerEditor, (void FAR* FAR*)&m_layer_editor);
   }
   // ILayerEditor2
   else if (m_interface_version == 2)
   {
      hresult = CoCreateInstance(m_classId, NULL, CLSCTX_ALL, 
         fvw::IID_ILayerEditor2, (void FAR* FAR*)&m_layer_editor2);
   }
   // ILayerEditor3
   else if (m_interface_version == 3)
   {
      hresult = CoCreateInstance(m_classId, NULL, CLSCTX_ALL,
         fvw::IID_ILayerEditor3, (void FAR* FAR*)&m_layer_editor3);
   }

   if (FAILED(hresult))
   {
      CString msg;
      msg.Format("Unable to create ILayerEditor object for 3rd party plugin: class Id string = %s.  CoCreateInstance failed with HRESULT = %X",
         class_ID_string, hresult);
      ERR_report(msg);
   }

   return hresult;
}

void LayerEditorImpl::Release()
{
   if (m_interface_version == 1)
   {
      m_layer_editor->Release();
      m_layer_editor = NULL;
   }

   else if (m_interface_version == 2)
   {
      m_layer_editor2->Release();
      m_layer_editor2 = NULL;
   }

   else if (m_interface_version == 3)
   {
      m_layer_editor3->Release();
      m_layer_editor3 = NULL;
   }
}

HRESULT LayerEditorImpl::GetDefaultCursor(long *hcur)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetDefaultCursor(hcur);

   else if (m_interface_version == 2)
      return m_layer_editor2->GetDefaultCursor(hcur);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetDefaultCursor(hcur);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::TestSelected(long layer_handle, long object_handle,
          double lat,  double lon,  long flags, 
          long *hcur,  BSTR* tooltip,  BSTR* helptext,
          long *return_val)
{
   if (m_interface_version == 1)
      return m_layer_editor->TestSelected(layer_handle, object_handle, lat, lon,
      flags, hcur, tooltip, helptext, return_val);

   else if (m_interface_version == 2)
      return m_layer_editor2->TestSelected(layer_handle, object_handle, lat, lon,
      flags, hcur, tooltip, helptext, return_val);

   else if (m_interface_version == 3)
      return m_layer_editor3->TestSelected(layer_handle, object_handle, lat, lon,
      flags, hcur, tooltip, helptext, return_val);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::Selected( long layer_handle,  long object_handle,
          double lat,  double lon,  long flags,  short* drag,
          long *hcur,  BSTR* tooltip,  BSTR* helptext,
          long *return_val)
{
   if (m_interface_version == 1)
      return m_layer_editor->Selected(layer_handle, object_handle, lat, lon,
      flags, drag, hcur, tooltip, helptext, return_val);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->Selected(layer_handle, object_handle, lat, lon,
      flags, drag, hcur, tooltip, helptext, return_val);

   else if (m_interface_version == 3)
      return m_layer_editor3->Selected(layer_handle, object_handle, lat, lon,
      flags, drag, hcur, tooltip, helptext, return_val);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnDrag( long layer_handle,  long object_handle,
          double lat,  double lon,  long flags, 
          long *hcur,  BSTR* tooltip,  BSTR* helptext)
{
   if (m_interface_version == 1)
      return m_layer_editor->OnDrag(layer_handle, object_handle,
      lat, lon, flags, hcur, tooltip, helptext);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->OnDrag(layer_handle, object_handle,
      lat, lon, flags, hcur, tooltip, helptext);

   else if (m_interface_version == 3)
      return m_layer_editor3->OnDrag(layer_handle, object_handle,
      lat, lon, flags, hcur, tooltip, helptext);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnDrop( long layer_handle,  long object_handle,
               double lat,  double lon,  long flags)
{
   if (m_interface_version == 1)
      return m_layer_editor->OnDrop(layer_handle, object_handle,
      lat, lon, flags);

   else if (m_interface_version == 2)
      return m_layer_editor2->OnDrop(layer_handle, object_handle,
      lat, lon, flags);

   else if (m_interface_version == 3)
      return m_layer_editor3->OnDrop(layer_handle, object_handle,
      lat, lon, flags);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::CancelDrag( long layer_handle,  long object_handle)
{
   if (m_interface_version == 1)
      return m_layer_editor->CancelDrag(layer_handle, object_handle);

   else if (m_interface_version == 2)
      return m_layer_editor2->CancelDrag(layer_handle, object_handle);

   else if (m_interface_version == 3)
      return m_layer_editor3->CancelDrag(layer_handle, object_handle);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::SetEditOn( short edit)
{
   if (m_interface_version == 1)
      return m_layer_editor->SetEditOn(edit);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->SetEditOn(edit);

   else if (m_interface_version == 3)
      return m_layer_editor3->SetEditOn(edit);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::Save( long layer_handle,  BSTR file_spec)
{
   if (m_interface_version == 1)
      return m_layer_editor->Save(layer_handle, file_spec);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->Save(layer_handle, file_spec);

   else if (m_interface_version == 3)
      return m_layer_editor3->Save(layer_handle, file_spec);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::DisableProjectionUI( long layer_handle,  short* disable_proj)
{
   if (m_interface_version == 1)
      return m_layer_editor->DisableProjectionUI(layer_handle, disable_proj);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->DisableProjectionUI(layer_handle, disable_proj);

   else if (m_interface_version == 3)
      return m_layer_editor3->DisableProjectionUI(layer_handle, disable_proj);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::DisableRotationUI( long layer_handle,  short* disable_rotation)
{
   if (m_interface_version == 1)
      return m_layer_editor->DisableRotationUI(layer_handle, disable_rotation);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->DisableRotationUI(layer_handle, disable_rotation);

   else if (m_interface_version == 3)
      return m_layer_editor3->DisableRotationUI(layer_handle, disable_rotation);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::RequiresNorthUp( long layer_handle,  short* requires)
{
   if (m_interface_version == 1)
      return m_layer_editor->RequiresNorthUp(layer_handle, requires);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->RequiresNorthUp(layer_handle, requires);

   else if (m_interface_version == 3)
      return m_layer_editor3->RequiresNorthUp(layer_handle, requires);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::RequiresEqualArc( long layer_handle,  short* requires)
{
   if (m_interface_version == 1)
      return m_layer_editor->RequiresEqualArc(layer_handle, requires);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->RequiresEqualArc(layer_handle, requires);

   else if (m_interface_version == 3)
      return m_layer_editor3->RequiresEqualArc(layer_handle, requires);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetDefaultExtension( long layer_handle,  BSTR* default_extension)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetDefaultExtension(layer_handle, default_extension);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetDefaultExtension(layer_handle, default_extension);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetDefaultExtension(layer_handle, default_extension);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetDefaultDirectory( long layer_handle,  BSTR* default_dir)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetDefaultDirectory(layer_handle, default_dir);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetDefaultDirectory(layer_handle, default_dir);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetDefaultDirectory(layer_handle, default_dir);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetDefaultFilter( long layer_handle,  BSTR* default_filter)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetDefaultFilter(layer_handle, default_filter);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetDefaultFilter(layer_handle, default_filter);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetDefaultFilter(layer_handle, default_filter);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetNextNewFileName( BSTR* next_new_filename)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetNextNewFileName(next_new_filename);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetNextNewFileName(next_new_filename);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetNextNewFileName(next_new_filename);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetDispatchPtr( VARIANT* dispatch_pointer)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetDispatchPtr(dispatch_pointer);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetDispatchPtr(dispatch_pointer);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetDispatchPtr(dispatch_pointer);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetIconName( BSTR* icon_name)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetIconName(icon_name);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetIconName(icon_name);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetIconName(icon_name);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetEditorName( BSTR* editor_name)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetEditorName(editor_name);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetEditorName(editor_name);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetEditorName(editor_name);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetEditorToolbarButton( BSTR* button_filename)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetEditorToolbarButton(button_filename);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetEditorToolbarButton(button_filename);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetEditorToolbarButton(button_filename);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::OpenFile( long layer_handle,  BSTR filename)
{
   if (m_interface_version == 1)
      return m_layer_editor->OpenFile(layer_handle, filename);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->OpenFile(layer_handle, filename);

   else if (m_interface_version == 3)
      return m_layer_editor3->OpenFile(layer_handle, filename);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetEditorStrings( BSTR* menu_text,  BSTR* file_type_text)
{
   if (m_interface_version == 1)
      return m_layer_editor->GetEditorStrings(menu_text, file_type_text);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->GetEditorStrings(menu_text, file_type_text);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetEditorStrings(menu_text, file_type_text);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::OnNewLayer( long layer_handle)
{
   if (m_interface_version == 1)
      return m_layer_editor->OnNewLayer(layer_handle);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->OnNewLayer(layer_handle);

   else if (m_interface_version == 3)
      return m_layer_editor3->OnNewLayer(layer_handle);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::OnKeyDown( long layer_handle,  long character,  long flags,  long* result)
{
   if (m_interface_version == 1)
      return m_layer_editor->OnKeyDown(layer_handle, character, flags, result);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->OnKeyDown(layer_handle, character, flags, result);

   else if (m_interface_version == 3)
      return m_layer_editor3->OnKeyDown(layer_handle, character, flags, result);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::OnKeyUp( long layer_handle,  long character,  long flags,  long* result)
{
   if (m_interface_version == 1)
      return m_layer_editor->OnKeyUp(layer_handle, character, flags, result);
   
   else if (m_interface_version == 2)
      return m_layer_editor2->OnKeyUp(layer_handle, character, flags, result);

   else if (m_interface_version == 3)
      return m_layer_editor3->OnKeyUp(layer_handle, character, flags, result);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::GetPropertiesProgID(BSTR* properties_ProgID)
{
   if (m_interface_version == 2)
      return m_layer_editor2->GetPropertiesProgID(properties_ProgID);

   else if (m_interface_version == 3)
      return m_layer_editor3->GetPropertiesProgID(properties_ProgID);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::GetPreferenceString(BSTR* preference_str)
{
   if (m_interface_version == 2)
      return m_layer_editor2->GetPreferenceString(preference_str);

   if (m_interface_version == 3)
      return m_layer_editor3->GetPreferenceString(preference_str);

   return E_NOTIMPL;
}
HRESULT LayerEditorImpl::SetPreferenceString(BSTR preference_str)
{
   if (m_interface_version == 2)
      return m_layer_editor2->SetPreferenceString(preference_str);

   if (m_interface_version == 3)
      return m_layer_editor3->SetPreferenceString(preference_str);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnPropertiesHelp()
{
   if (m_interface_version == 2)
      return m_layer_editor2->OnPropertiesHelp();

   if (m_interface_version == 3)
      return m_layer_editor3->OnPropertiesHelp();

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::TestSelectedEx(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
      long lCursorFlags, long *hcur, BSTR* tooltip, BSTR* helptext,
      long *lReturnVal)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->TestSelectedEx(layer_handle, pSettableMapProj, hDC, lCursorX, lCursorY,
         lCursorFlags, hcur, tooltip, helptext, lReturnVal);
   }

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::SelectedEx(long layer_handle,  IDispatch *pSettableMapProj, long hDC, long lCursorX,  long lCursorY,
       long lCursorFlags, short* drag, long *hcur, BSTR* tooltip, BSTR* helptext, long *return_val)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->SelectedEx(layer_handle, pSettableMapProj, hDC, lCursorX, lCursorY,
         lCursorFlags, drag, hcur, tooltip, helptext, return_val);
   }

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnDragEx(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
       long lCursorFlags, long *hcur, BSTR* tooltip, BSTR* helptext)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->OnDragEx(layer_handle, pSettableMapProj, hDC, lCursorX, lCursorY,
         lCursorFlags, hcur, tooltip, helptext);
   }

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnDropEx(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY, long lCursorFlags)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->OnDropEx(layer_handle, pSettableMapProj, hDC, lCursorX, lCursorY, lCursorFlags);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::CancelDragEx(long layer_handle, IDispatch *pSettableMapProj, long hDC)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->CancelDragEx(layer_handle, pSettableMapProj, hDC);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnDraw(long layer_handle, IDispatch *pActiveMapProj, long bDrawBeforeLayerObjects)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->OnDraw(layer_handle, pActiveMapProj, bDrawBeforeLayerObjects);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::CanAddPixmapsToBaseMap(long *bCanAddPixmapsToBaseMap)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->CanAddPixmapsToBaseMap(bCanAddPixmapsToBaseMap);
   }

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnDrawToBaseMap(long layer_handle, IDispatch *pActiveMapProj)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->OnDrawToBaseMap(layer_handle, pActiveMapProj);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::IsSelectionLocked(long layer_handle, long *pSelectionLocked)
{
   if (m_interface_version == 3)
      return m_layer_editor3->raw_IsSelectionLocked(layer_handle, pSelectionLocked);

   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnAddMenuItems(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY, 
                       BSTR* pMenuItems)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->OnAddMenuItems(layer_handle, pSettableMapProj, hDC, lCursorX, lCursorY, pMenuItems);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnDoubleClickEx(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
                        long lCursorFlags)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->OnDoubleClickEx(layer_handle, pSettableMapProj, hDC, lCursorX, lCursorY,
         lCursorFlags);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnTestSnapTo(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
                     long *pCanSnapTo)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->raw_OnTestSnapTo(layer_handle, pSettableMapProj, hDC, lCursorX, lCursorY,
         pCanSnapTo);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::OnGetSnapToPoints(long layer_handle, IDispatch *pSettableMapProj, long hDC, 
                                           long lCursorX, long lCursorY, IDispatch *pSnapToPoints)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->OnGetSnapToPoints(layer_handle, pSettableMapProj, hDC, lCursorX, lCursorY, pSnapToPoints);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::PreSave(long layer_handle, long *lCancel)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->raw_PreSave(layer_handle, lCancel);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::IsReadOnly(long layer_handle, long *pReadOnly)
{
   if (m_interface_version == 3)
   {
      return m_layer_editor3->raw_IsReadOnly(layer_handle, pReadOnly);
   }
   
   return E_NOTIMPL;
}


HRESULT LayerEditorImpl::CanDropOLEDataObject(long layer_handle, long lCursorX, long lCursorY, IDispatch *pSettableMapProj, IDataObject *pDataObject, long *pCanDrop)
{
   if (m_interface_version == 3)
   {
      IUnknownPtr smpUnknown(pDataObject);
      return m_layer_editor3->raw_CanDropOLEDataObject(layer_handle, lCursorX, lCursorY, pSettableMapProj, smpUnknown, pCanDrop);
   }
   
   return E_NOTIMPL;
}

HRESULT LayerEditorImpl::PasteOLEDataObject(long layer_handle, long lCursorX, long lCursorY, IDispatch *pSettableMapProj, IDataObject *pDataObject)
{
   if (m_interface_version == 3)
   {
      IUnknownPtr smpUnknown(pDataObject);
      return m_layer_editor3->PasteOLEDataObject(layer_handle, lCursorX, lCursorY, pSettableMapProj, smpUnknown);
   }
   
   return E_NOTIMPL;
}

// Map server overlay
//

// {700A7E93-35A9-4b39-B235-748418824E34}
const GUID FVWID_Overlay_MapServer = 
   { 0x700a7e93, 0x35a9, 0x4b39, { 0xb2, 0x35, 0x74, 0x84, 0x18, 0x82, 0x4e, 0x34 } };

C_map_server_ovl_factory::C_map_server_ovl_factory()
{
}

C_overlay *C_map_server_ovl_factory::open(CString filename)
{
   // there is no mechanism to open a IMap_server overlay
   return NULL;
}
