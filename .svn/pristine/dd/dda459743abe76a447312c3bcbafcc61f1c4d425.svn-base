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



//
// forward declarations
//
class C_overlay_factory;
class CLayerOvlFactory;


#include "OvlFctry.h"
#include "ovl_d.h"   // attribute defines
#include "icallback.h"
#include "optndlg.h"


#pragma once

// defines the maximum number of external client editors implementing
// the ILayerEditor interface allowed
#define MAX_EXTERNAL_EDITORS 20

// the layer editor needs to be wrapped so that users of ILayerEditor
// and ILayerEditor2 need not worry about the distinction
class LayerEditorImpl
{
   CLSID m_classId;
   fvw::ILayerEditor *m_layer_editor;
   fvw::ILayerEditor2 *m_layer_editor2;
   fvw::ILayerEditor3 *m_layer_editor3;

   int m_interface_version;

public:
   bool interface_OK()
   {
      if (m_interface_version == 1)
         return m_layer_editor != NULL;
      else if (m_interface_version == 2)
         return m_layer_editor2 != NULL;
      else if (m_interface_version == 3)
         return m_layer_editor3 != NULL;

      return false;
   }

public:
   LayerEditorImpl(int version) 
   { 
      m_layer_editor = NULL;
      m_layer_editor2 = NULL;
      m_layer_editor3 = NULL;
      m_interface_version = version; 
   }
   CLSID GetClassId() { return m_classId; }
   HRESULT CreateInstance(CString class_ID_string);
   void Release();
   int GetInterfaceVersion() { return m_interface_version; }

   IDispatch* GetDispatchInterface() 
   { 
      if (m_interface_version == 1)
         return IDispatchPtr(m_layer_editor, true).Detach();
      else if (m_interface_version == 2)
         return IDispatchPtr(m_layer_editor2, true).Detach();
      else if (m_interface_version == 3)
         return IDispatchPtr(m_layer_editor3, true).Detach();

      return NULL;
   }

   // ILayerEditor
   HRESULT GetDefaultCursor(long *hcur);
   HRESULT TestSelected(long layer_handle, long object_handle,
          double lat,  double lon,  long flags, 
          long *hcur,  BSTR* tooltip,  BSTR* helptext,
          long *return_val);
   HRESULT Selected( long layer_handle,  long object_handle,
          double lat,  double lon,  long flags,  short* drag,
          long *hcur,  BSTR* tooltip,  BSTR* helptext,
          long *return_val);
   HRESULT OnDrag( long layer_handle,  long object_handle,
          double lat,  double lon,  long flags, 
          long *hcur,  BSTR* tooltip,  BSTR* helptext);
   HRESULT OnDrop( long layer_handle,  long object_handle,
          double lat,  double lon,  long flags);
   HRESULT CancelDrag( long layer_handle,  long object_handle);
   HRESULT SetEditOn( short edit);
   HRESULT Save( long layer_handle,  BSTR file_spec);
   HRESULT DisableProjectionUI( long layer_handle,  short* disable_proj);
   HRESULT DisableRotationUI( long layer_handle,  short* disable_rotation);
   HRESULT RequiresNorthUp( long layer_handle,  short* requires);
   HRESULT RequiresEqualArc( long layer_handle,  short* requires);
   HRESULT GetDefaultExtension( long layer_handle,  BSTR* default_extension);
   HRESULT GetDefaultDirectory( long layer_handle,  BSTR* default_dir);
   HRESULT GetDefaultFilter( long layer_handle,  BSTR* default_filter);
   HRESULT GetNextNewFileName( BSTR* next_new_filename);
   HRESULT GetDispatchPtr( VARIANT* dispatch_pointer);
   HRESULT GetIconName( BSTR* icon_name);
   HRESULT GetEditorName( BSTR* editor_name);
   HRESULT GetEditorToolbarButton( BSTR* button_filename);
   HRESULT OpenFile( long layer_handle,  BSTR filename);
   HRESULT GetEditorStrings( BSTR* menu_text,  BSTR* file_type_text);
   HRESULT OnNewLayer( long layer_handle);
   HRESULT OnKeyDown( long layer_handle,  long character,  long flags,  long* result);
   HRESULT OnKeyUp( long layer_handle,  long character,  long flags,  long* result);

   // ILayerEditor2
   HRESULT GetPropertiesProgID(BSTR* properties_ProgID);
   HRESULT GetPreferenceString(BSTR* preference_str);
   HRESULT SetPreferenceString(BSTR preference_str);
   HRESULT OnPropertiesHelp();

   // ILayerEditor3
   HRESULT TestSelectedEx(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
      long lCursorFlags, long *hcur, BSTR* tooltip, BSTR* helptext,
      long *lReturnVal);
   HRESULT SelectedEx(long layer_handle,  IDispatch *pSettableMapProj, long hDC, long lCursorX,  long lCursorY,
       long lCursorFlags, short* drag, long *hcur, BSTR* tooltip, BSTR* helptext, long *return_val);
   HRESULT OnDragEx(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
       long lCursorFlags, long *hcur, BSTR* tooltip, BSTR* helptext);
   HRESULT OnDropEx(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY, long lCursorFlags);
   HRESULT CancelDragEx(long layer_handle, IDispatch *pSettableMapProj, long hDC);
   HRESULT OnDraw(long layer_handle, IDispatch *pActiveMapProj, long bDrawBeforeLayerObjects);
   HRESULT CanAddPixmapsToBaseMap(long *bCanAddPixmapsToBaseMap);
   HRESULT OnDrawToBaseMap(long layer_handle, IDispatch *pActiveMapProj);
   HRESULT IsSelectionLocked(long layer_handle, long *pSelectionLocked);
   HRESULT OnAddMenuItems(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY, 
      BSTR* pMenuItems);
   HRESULT OnDoubleClickEx(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
      long lCursorFlags);
   HRESULT OnTestSnapTo(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
      long *pCanSnapTo);
   HRESULT OnGetSnapToPoints(long layer_handle, IDispatch *pSettableMapProj, long hDC, long lCursorX, long lCursorY,
      IDispatch *pSnapToPoints);
   HRESULT PreSave(long layer_handle, long *lCancel);
   HRESULT IsReadOnly(long layer_handle, long *pReadOnly);
   HRESULT CanDropOLEDataObject(long layer_handle, long lCursorX, long lCursorY, IDispatch *pSettableMapProj, IDataObject *pDataObject, long *pCanDrop);
   HRESULT PasteOLEDataObject(long layer_handle, long lCursorX, long lCursorY, IDispatch *pSettableMapProj, IDataObject *pDataObject);
};

class CLayerOvlEditor : 
   public IFvOverlayEditor,
   public IFvOverlayLimitUserInterface,
   public IFvOverrideDefaultUserInterface
{
private:
   // this LayerEditorImpl is created by the factory.  The dtor should not delete this object
   LayerEditorImpl *m_layer_editor;
   CString m_bitmap_filename;
   CString m_strEditorDisplayName;

public:
   CLayerOvlEditor(LayerEditorImpl *pLayerEditor);
   virtual ~CLayerOvlEditor();

   bool interface_OK() { return (m_layer_editor && m_layer_editor->interface_OK()); }

   CString GetEditorDisplayName() { return m_strEditorDisplayName; }

   virtual HCURSOR get_default_cursor();
   virtual int set_edit_on(boolean_t edit_on);

   // IFvOverlayEditor
public:
   // Called when the editor mode is activated
   virtual HRESULT ActivateEditor()
   {
      return set_edit_on(TRUE) == SUCCESS ? S_OK : E_FAIL;
   }

   // If the cursor is not set by any overlay, then the current editor's default cursor is used
   virtual HRESULT GetDefaultCursor(long *hCursor)
   {
      *hCursor = reinterpret_cast<long>(get_default_cursor());
      return S_OK;
   }

   // Called when the user leaves this editor mode
   virtual HRESULT DeactivateEditor()
   {
      return set_edit_on(FALSE) == SUCCESS ? S_OK : E_FAIL;
   }

   // IFvOverlayLimitUserInterface
public:
   // Return TRUE if the user should not be able to change the projection of the map
   virtual HRESULT get_m_bDisableProjectionUI(long* pVal);

   // Return TRUE if the user should not be able to rotate the map while the editor is active
   virtual HRESULT get_m_bDisableRotationUI(long* pVal);

   // Return TRUE if the map must be North-Up
   virtual HRESULT get_m_bRequiresNorthUp(long* pVal);

   // Return TRUE if the map must be projected with the Equal Arc projection when this editor is active
   virtual HRESULT get_m_bRequiresEqualArc(long* pVal);

   // Return TRUE if the order of overlays associated with this editor cannot be changed
   virtual HRESULT get_m_bDisableOverlayOrdering(long* pVal);

   // IFvOverrideDefaultUserInterface
public:
   // Return TRUE if FalconView should activate the editor when a static overlay is toggled on or when the user chooses File | New
   // for a file overlay.  Return FALSE otherwise.  FalconView's default behavior is TRUE.
   virtual HRESULT get_m_bAutoEnterOverlayEditor(long* pVal);

   // Normally, FalconView will use the overlay type's icon as the editor toolbar button.  The following method can be
   // used to override the editor toolbar button's image.  Return an empty string if you would like to preserve the default behavior.
   virtual HRESULT get_m_bEditorToolbarButtonBitmap(BSTR* pEditorToolbarButtonBmp);
};

class CLayerOvlFactory : public IFvOverlayFactory
{
public:
   CLayerOvlFactory(int editor_num, CString class_ID_string, int interface_version);

   virtual HRESULT CreateOverlayInstance(C_overlay **ppOverlay);

   virtual ~CLayerOvlFactory();

   CString GetDisplayName() { return m_strDisplayName; }
   CString GetIconFile() { return m_icon_file; }

   // on_help - implement this if your overlay uses an ActiveX control for
   // the overlay options
   void on_help();

   int GetEditorNum() { return m_editor_num; }

   // return a pointer to the ILayerEditor object
   LayerEditorImpl *get_ILayerEditor() { return m_layer_editor; }

   GUID GetOverlayDescriptorGuid() { return m_overlayDescGuid; }

protected:

   virtual C_overlay *CreateOverlay(const DWORD owing_thread_id, CString name,
      IDispatch *pdisp, CString icon_name, LayerEditorImpl *layer_editor);

   // plug-in overlay interface (for both static and file overlays)
   LayerEditorImpl *m_layer_editor;

   GUID m_overlayDescGuid;

   CString m_strDisplayName;
   int m_editor_num;

   CString m_icon_file;

public:

   // methods to support ActiveX property pages
   //
   virtual CString get_prog_ID();
   virtual CString get_preference_string();
   virtual void set_preference_string(CString pref);
   CString m_properties_progID;
};

class CLayerFileOvlFactory : 
   public CLayerOvlFactory,
   public IFvFileOverlayTypeDescriptor
{
   // IFvFileOverlayTypeDescriptor
public:
   // Get the default directory where files for this overlay type are stored
   virtual HRESULT GetDefaultDirectory(BSTR* pbstrDefaultDir);

   // Get the default file extension of files for this overlay type
   virtual HRESULT GetDefaultFileExtension(BSTR* pbstrDefaultExt);

   // Get the filter used in the common file dialog boxes (open/save).  Return one or more pairs of '|' separated
   // filter strings.  The first string in each pair is a display string that describes the filter, and the second
   // string specifies the filter pattern (for example, "*.rte").  Note that you can return a different filter for 
   // the Open vs. Save As dialogs
   virtual HRESULT GetFileDialogFilter(long bIsFileOpenDialog, BSTR* pbstrFilter);

public:
   CLayerFileOvlFactory(int editor_num, CString class_ID_string, int interface_version);


protected:
   virtual C_overlay *CreateOverlay(const DWORD owning_thread_id, CString name,
      IDispatch *pdisp, CString icon_name, LayerEditorImpl *layer_editor);

   // the default extension is retrieved from the client editor upon startup
   CString m_default_extension;
};

class CLayerOvlOCXContainerPage : public OCXContainerPage
{
   CLayerOvlFactory *m_pLayerOvlFactory;

public:
   CLayerOvlOCXContainerPage(CLayerOvlFactory *pLayerOvlFactory) : m_pLayerOvlFactory(pLayerOvlFactory) 
   { 
   }
protected:
   // OCXContainerPage overrides
   //
   virtual CString get_prog_ID() { return m_pLayerOvlFactory->get_prog_ID(); }
   virtual CString get_preference_string() { return m_pLayerOvlFactory->get_preference_string(); }
   virtual void set_preference_string(CString pref) { m_pLayerOvlFactory->set_preference_string(pref); }
};

class CLayerOvlPropertyPage : public CFvOverlayPropertyPageImpl
{
   CLayerOvlFactory *m_pLayerOvlFactory;

public:
   CLayerOvlPropertyPage(CLayerOvlFactory *pLayerOvlFactory) : m_pLayerOvlFactory(pLayerOvlFactory)
   {
   }
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CLayerOvlOCXContainerPage(m_pLayerOvlFactory); }
};


// Factory for legacy C_map_server_ovl overlays
//

extern const GUID FVWID_Overlay_MapServer;

class C_map_server_ovl_factory : public IFvOverlayFactory
{
public:
   C_map_server_ovl_factory();
   virtual C_overlay *open(CString filename);

   virtual HRESULT CreateOverlayInstance(C_overlay **ppOverlay)
   {
      // overlays of this type are created directly by the automation
      // interface
      return E_FAIL;
   }
};