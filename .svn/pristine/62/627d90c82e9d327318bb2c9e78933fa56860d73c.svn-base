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

// FvVerticalViewImpl.h
//

#pragma once

class CVerticalViewDisplay;

// Implements IFvVerticalView defined in FalconViewOverlay.tlb
//
class CFvVerticalViewImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvVerticalView, &FalconViewOverlayLib::IID_IFvVerticalView, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   CVerticalViewDisplay *m_pVerticalView;

public:
   CFvVerticalViewImpl() : m_pVerticalView(NULL)
   {
   }

BEGIN_COM_MAP(CFvVerticalViewImpl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvVerticalView)
END_COM_MAP()

   void SetVerticalView(CVerticalViewDisplay *pVerticalView)
   {
      m_pVerticalView = pVerticalView;
   }

// IFvVerticalView

   // get the HWND associated with the view
   STDMETHOD(get_hWnd)(long *pHwnd);

   // get the frame window associated with the view
   STDMETHOD(get_FrameWnd)(FalconViewOverlayLib::IFvFrameWnd** ppFrameWnd);

   // get the current vertical view projection associated with the view
   STDMETHOD(get_CurrentVerticalViewProj)(FalconViewOverlayLib::IFvVerticalViewProjector **ppVerticalViewProj);
   
   // get the overlay manager
   STDMETHOD(get_OverlayManager)(FalconViewOverlayLib::IFvOverlayManager** ppOverlayManager);

   // set the tooltip for the map view.  The tooltip will be displayed at the current mouse
   // position in 1/8 of second.  If the given text matches the text of a tooltip that is already 
   // being displayed and the cursor is within a certain threshold then the tooltip will not be cleared.
   // Also, if the mouse position is outside the view then no tooltip will be displayed.  
   STDMETHOD(raw_SetTooltipText)(BSTR tooltipText);
};