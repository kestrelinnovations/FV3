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

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// MovingMapCameraEvents.h

#ifndef MOVINGMAPOVERLAY_MOVINGMAPCAMERAEVENTS_H_
#define MOVINGMAPOVERLAY_MOVINGMAPCAMERAEVENTS_H_

class C_gps_trail;

class MovingMapCameraEvents :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<
      FalconViewOverlayLib::ICameraEvents,
      &FalconViewOverlayLib::IID_ICameraEvents,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
public:
   MovingMapCameraEvents() : m_overlay(nullptr)
   {
   }
   void Initialize(C_gps_trail* overlay)
   {
      m_overlay = overlay;
   }

BEGIN_COM_MAP(MovingMapCameraEvents)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::ICameraEvents)
END_COM_MAP()

// ICameraEvents
   STDMETHOD(raw_OnAttachedCameraChanging)();

private:
   C_gps_trail* m_overlay;
};

#endif  // MOVINGMAPOVERLAY_MOVINGMAPCAMERAEVENTS_H_
