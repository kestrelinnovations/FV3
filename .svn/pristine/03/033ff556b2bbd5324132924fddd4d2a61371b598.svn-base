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

// FvOverlayManagerImpl.h
//

#pragma once

#include "IFvOverlayManagerEventsCP.h"
#include "ovl_mgr.h"
#include "OverlayStackChangedObserver_Interface.h"

class C_overlay;
class COverlayCOM;

// Implements IFvOverlayManager defined in FalconViewOverlay.tlb
//
class CFvOverlayManagerImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvOverlayManager2, &FalconViewOverlayLib::IID_IFvOverlayManager2, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>,
   public IConnectionPointContainerImpl<CFvOverlayManagerImpl>,
   public CProxyIFvOverlayManagerEvents<CFvOverlayManagerImpl>,
   OverlayStackChangedObserver_Interface
{
   // current overlay used in the enumeration methods
   C_overlay *m_pCurrentOverlay;
   GUID m_overlayDescGuid;

   enum { SELECT_ALL, SELECT_BY_OVERLAY_TYPE } m_eEnumeratorType;

public:
   CFvOverlayManagerImpl();
   ~CFvOverlayManagerImpl();

   BEGIN_COM_MAP(CFvOverlayManagerImpl)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvOverlayManager)
      COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvOverlayManager2)
      COM_INTERFACE_ENTRY(IConnectionPointContainer)
   END_COM_MAP()

   void Initialize()
   {
   }

   BEGIN_CONNECTION_POINT_MAP(CFvOverlayManagerImpl)
      CONNECTION_POINT_ENTRY(__uuidof(FalconViewOverlayLib::IFvOverlayManagerEvents))
   END_CONNECTION_POINT_MAP()

   // IFvOverlayManager
public:
   // Overlay creation and deletion
   //

   // toggles on/off a static overlay with the given overlay descriptor guid.  Returns a pointer to the 
   // overlay if the overlay was toggled on.  Returns NULL otherwise.  Note that if the overlay does not implement
   // IFvOverlay then the overlay will still be toggled, however ppOverlay will be NULL.
   STDMETHOD(raw_ToggleStaticOverlay)(GUID overlayDescGuid, FalconViewOverlayLib::IFvOverlay **ppOverlay);

   // open a file overlay of the given overlay type and filename.  Returns a pointer to the overlay
   // opened if successful.  Note that if the overlay does not implement IFvOverlay then the overlay 
   // will still be toggled, however ppOverlay will be NULL.
   STDMETHOD(raw_OpenFileOverlay)(GUID overlayDescGuid, BSTR fileName, FalconViewOverlayLib::IFvOverlay ** ppOverlay);

   // create a new instance of an overlay of the given type.  Returns a pointer to the overlay created
   // if successful.  Note that if the overlay does not implement IFvOverlay then the overlay will still be 
   // toggled, however ppOverlay will be NULL.
   STDMETHOD(raw_CreateOverlay)(GUID overlayDescGuid, FalconViewOverlayLib::IFvOverlay **ppOverlay);

   // Active Editor

   // retrieves the overlay descriptor guid of the active editor's overlay type, GUID_NULL for none
   STDMETHOD(raw_SetCurrentEditor)(GUID overlayDescGuid);
   STDMETHOD(get_CurrentEditor)(GUID *pOverlayDescGuid);

   // Enumeration
   //

   // SelectAll will select all overlays in the overlay stack.  Use MoveNext to enumerate through the 
   // overlays (from top of the stack to the bottom of the stack).  pbResult will be TRUE if there is at
   // least one overlay in the stack, FALSE otherwise.  Use the CurrentOverlay property to retrieve the overlay.
   STDMETHOD(raw_SelectAll)(long *pbResult);

   // SelectByOverlayDescGuid will select all overlays in the stack of the given overlay type.  Use MoveNext to enumerate
   // through the overlays (from top of the stack to bottom of the stack).  pbResult will be TRUE if CurrentOverlay points to the next overlay, FALSE otherwise.
   // 
   STDMETHOD(raw_SelectByOverlayDescGuid)(GUID overlayDescGuid, long *pbResult);
   STDMETHOD(raw_MoveNext)(long *pbResult);
   STDMETHOD(get_CurrentOverlay)(FalconViewOverlayLib::IFvOverlay **ppOverlay);

   // Manipulation of overlay stack
   //

   // Adds an existing, already Initialized overlay to the overlay stack
   STDMETHOD(raw_AddOverlayToStack)(FalconViewOverlayLib::IFvOverlay *pOverlay);

   // Removes the given overlay from the overlay stack
   STDMETHOD(raw_RemoveOverlayFromStack)(FalconViewOverlayLib::IFvOverlay *pOverlay);

   // MoveOverlayAbove will move one overlay above another one.  pInsertAboveOverlay can be NULL in which
   // case pOverlayToMove will move to the top of the overlay stack
   STDMETHOD(raw_MoveOverlayAbove)(FalconViewOverlayLib::IFvOverlay *pOverlayToMove, FalconViewOverlayLib::IFvOverlay *pInsertAboveOverlay);

   // MoveOverlayBelow will move one overlay below another one.  pInsertBelowOverlay can be NULL in which
   // case pOverlayToMove will move to the bottom of the overlay stack
   STDMETHOD(raw_MoveOverlayBelow)(FalconViewOverlayLib::IFvOverlay *pOverlayToMove, FalconViewOverlayLib::IFvOverlay *pInsertBelowOverlay);

   // Overlay type queries
   //

   STDMETHOD(raw_IsOverlayTypeEnabled)(GUID overlayDescGuid, long *pbEnabled);

   // Miscellaneous
   //

   // Show the overlay options dialog box starting at the page associated with the given overlay type descriptor
   // Note: overlayDescGuid can be GUID_NULL in which case the overlay options dialog box is opened up
   // to the first page.
   STDMETHOD(raw_ShowOverlayOptionsDialog)(GUID overlayDescGuid);

   // IFvOverlayManager2
   //
   STDMETHOD(raw_CloseOverlay)(FalconViewOverlayLib::IFvOverlay* pOverlayToClose);
   STDMETHOD(raw_GetOverlayHandle)(FalconViewOverlayLib::IFvOverlay* pOverlay, long *pHandle);
   STDMETHOD(raw_GetOverlayForHandle)(long handle, FalconViewOverlayLib::IFvOverlay** ppOverlay);

   // OverlayStackChangedObserver_Interface
   //
   virtual void OverlayAdded(C_overlay* overlay) override;
   virtual void OverlayRemoved(C_overlay* overlay) override;
   virtual void OverlayOrderChanged() override;
   virtual void OverlayFileSpecificationChanged(C_overlay* overlay) override;
   virtual void OverlayDirtyChanged(C_overlay* overlay) override;
};
