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

// OverlayEditorCOM.h
//

#pragma once

#include "OvlFctry.h"
#include "Common/ComErrorObject.h"
#include "FalconView/include/ToolbarEventSink.h"

class OverlayEditorToolbarEventSink;

class COverlayEditorCOM : 
   public IFvOverlayEditor,
   public IFvOverlayLimitUserInterface,
   public IFvOverrideDefaultUserInterface
{
   CLSID m_classId;
   _bstr_t m_displayName;

   FalconViewOverlayLib::IFvOverlayEditorPtr m_spFvOverlayEditor;
   FalconViewOverlayLib::IFvOverlayEditor2Ptr m_spFvOverlayEditor2;
   FalconViewOverlayLib::IFvOverlayLimitUserInterfacePtr m_spFvOverlayLimitUserInterface;
   FalconViewOverlayLib::IFvOverrideDefaultUserInterfacePtr m_spFvOverrideDefaultUserInterface;

   IFvToolbarPtr m_spFvToolbar;
   CComObject<OverlayEditorToolbarEventSink> *m_pToolbarEvents;

   UINT m_nToolbarCtrlId;

   BOOL m_bCurrentEditor;

public:
   COverlayEditorCOM(CLSID classId, _bstr_t displayName);
   ~COverlayEditorCOM();

   // IFvOverlayEditor
public:
   // Called when the editor mode is activated
   virtual HRESULT ActivateEditor();

   // If the cursor is not set by any overlay, then the current editor's default cursor is used
   virtual HRESULT GetDefaultCursor(long *hCursor);

   // Called when the user leaves this editor mode
   virtual HRESULT DeactivateEditor();

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

   void OnToolbarButtonPressed(long lButtonId);
   void OnToolbarClosed();
   void OnCustomizeToolbar();

protected:
   FalconViewOverlayLib::IFvMapView* COverlayEditorCOM::GetFvMapView();
};

// OverlayEditorToolbarEventSink handles toolbar events from the toolbar
// created by COverlayEditorCOM
class OverlayEditorToolbarEventSink : public ToolbarEventSink
{
public:
   STDMETHOD(raw_OnButtonPressed)(long lButtonId) 
   { 
      m_parent->OnToolbarButtonPressed(lButtonId);
      return S_OK; 
   }

   STDMETHOD(raw_OnClosed)() 
   { 
      m_parent->OnToolbarClosed();
      return S_OK; 
   }

   STDMETHOD(raw_OnCustomizeToolbar)() 
   { 
      m_parent->OnCustomizeToolbar();
      return S_OK; 
   }
   void Initialize(COverlayEditorCOM *pParent)
   {
      m_parent = pParent;
   }

private:
   COverlayEditorCOM *m_parent;
};