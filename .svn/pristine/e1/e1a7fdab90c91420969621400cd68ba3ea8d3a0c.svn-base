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

// FvMapViewImpl.h
//

#pragma once

class MapView;

// Implements IFvMapView defined in FalconViewOverlay.tlb
//
class CFvMapViewImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvMapView,
      &FalconViewOverlayLib::IID_IFvMapView,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>,
   public IDispatchImpl<FalconViewOverlayLib::IFvMapViewCamera,
      &FalconViewOverlayLib::IID_IFvMapViewCamera,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   MapView *m_pMapView;

public:
   CFvMapViewImpl() : m_pMapView(NULL)
   {
   }

BEGIN_COM_MAP(CFvMapViewImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IFvMapView)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvMapView)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvMapViewCamera)
END_COM_MAP()

   void SetMapView(MapView *pMapView)
   {
      m_pMapView = pMapView;
   }

// IFvMapView

   // get the HWND associated with the view
   STDMETHOD(get_hWnd)(long *pHwnd);

   // get the frame window associated with the view
   STDMETHOD(get_FrameWnd)(FalconViewOverlayLib::IFvFrameWnd** ppFrameWnd);

   // get the current map projection (ISettableMapProj) associated with the view
   STDMETHOD(get_CurrentMapProj)(IUnknown **ppMapProj);
   
   // get the overlay manager
   STDMETHOD(get_OverlayManager)(FalconViewOverlayLib::IFvOverlayManager** ppOverlayManager);

   // returns TRUE if the view is being smooth-scrolled
   STDMETHOD(get_IsScrolling)(long *bIsScrolling);

   // set the cursor for the map view
   STDMETHOD(raw_SetCursor)(long hCursor);

   // set the tooltip for the map view.  The tooltip will be displayed at the current mouse
   // position in 1/8 of second.  If the given text matches the text of a tooltip that is already 
   // being displayed and the cursor is within a certain threshold then the tooltip will not be cleared.
   // Also, if the mouse position is outside the view then no tooltip will be displayed.  
   STDMETHOD(raw_SetTooltipText)(BSTR tooltipText);

   // Returns TRUE if any overlay has a snappable point in view at the given screen coordinates
   STDMETHOD(raw_CanSnapTo)(long x, long y, long *pbCanSnapTo);

   // Get a snappable point in the view at the given screen coordinates.  The framework is responsbile
   // for picking a single point in the case that there are multiple snappable points at the given location.
   // note: pSnapToPointsList is an object that implements ISnapToPointsList
   STDMETHOD(raw_GetSnappablePoint)(long x, long y, IUnknown **pSnapToPointsList);

   // Scrolls the center of the map associated with this view 
   // if the given point is close enough to one or more edges of the view
   STDMETHOD(raw_ScrollMapIfPointNearEdge)(long x, long y);

// IFvMapViewCamera

   STDMETHOD(raw_GetCameraPosition)(long* values_valid,
      double* lat, double* lon,
      double* alt_meters, double* heading,
      double* incline, double* roll);
   STDMETHOD(raw_AttachToCamera)(
      FalconViewOverlayLib::ICameraEvents* camera_events);
   STDMETHOD(raw_DetachFromCamera)();
   STDMETHOD(raw_SetCameraPositionWhenAttached)(double lat, double lon,
      double alt_meters, double heading, double incline, double roll);
};