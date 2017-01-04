// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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

// OsgEarthMapEngineCallback.cpp

#ifndef FALCONVIEW_OSGEARTHMAPENGINECALLBACK_H_
#define FALCONVIEW_OSGEARTHMAPENGINECALLBACK_H_

#include "FalconView/include/MAPS_D.H"
#include "MapEngineCOM.h"

// forward declarations
struct OverlayRender_Interface;
namespace osgEarth
{
   class GeoExtent;
   class Map;
}

class OsgEarthMapEngineCallback :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CComErrorHandler,
   public IDispatchImpl<IMapRenderingEngineCallback,
      &IID_IMapRenderingEngineCallback, &LIBID_MAPENGINELib>
{
public:
   void Initialize(osgEarth::Map* osg_map);

   void SetExtent(const osgEarth::GeoExtent& geo_extent);
   const MapType& GetMapType() const;
   void SetMapType(const MapType& map_type);

   double GetWidth();
   double GetHeight();
   void SetSurfaceDimensions(double width, double height);

BEGIN_COM_MAP(OsgEarthMapEngineCallback)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(IMapRenderingEngineCallback)
   COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

   STDMETHOD(raw_is_draw_interrupted)(VARIANT_BOOL *is_interrupted);
   STDMETHOD(raw_invalidate_overlays)();
   STDMETHOD(raw_draw_overlays)(IActiveMapProj *map);
   STDMETHOD(raw_draw_to_base_map)(IActiveMapProj *map);
   STDMETHOD(raw_get_redraw_overlays_from_scratch)(
      VARIANT_BOOL *redraw_from_scratch);
   STDMETHOD(raw_can_add_pixmaps_to_base_map)(
      VARIANT_BOOL *can_add_pixmaps_to_base_map);

private:
   osgEarth::Map* m_osg_map;  // there are many maps about - let's be clear
   osgEarth::GeoExtent* m_geo_extent;
   MapType m_map_type;
   double m_width, m_height;
};

#endif  // FALCONVIEW_OSGEARTHMAPENGINECALLBACK_H_
