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

// OsgEarthMapEngineCallback::.cpp

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/OsgEarthMapEngineCallback.h"

// system includes

// third party files
#include "osgEarth/GeoData"

// other FalconView headers

// this project's headers
#include "FalconView/getobjpr.h"
#include "FalconView/include/overlay.h"
#include "FalconView/LayerOvl.h"
#include "FalconView/MapEngineCOM.h"
#include "FalconView/mapview.h"
#include "FalconView/overlay/OverlayEventRouter.h"
#include "FalconView/TAMask/factory.h"

void OsgEarthMapEngineCallback::Initialize(osgEarth::Map* osg_map)
{
   m_osg_map = osg_map;
}

void OsgEarthMapEngineCallback::SetExtent(const osgEarth::GeoExtent& geo_extent)
{
   // we do not modify the key, but we need to keep it around for the duration
   // of the draw
   m_geo_extent = const_cast<osgEarth::GeoExtent*>(&geo_extent);
}

const MapType& OsgEarthMapEngineCallback::GetMapType() const
{
   return m_map_type;
}

void OsgEarthMapEngineCallback::SetMapType(const MapType& map_type)
{
   m_map_type = map_type;
}

double OsgEarthMapEngineCallback::GetWidth()
{
   return m_width;
}

double OsgEarthMapEngineCallback::GetHeight()
{
   return m_height;
}

void OsgEarthMapEngineCallback::SetSurfaceDimensions(
   double width, double height)
{
   m_width = width;
   m_height = height;
}

STDMETHODIMP OsgEarthMapEngineCallback::raw_is_draw_interrupted(
   VARIANT_BOOL *is_interrupted)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   *is_interrupted = VARIANT_FALSE;
   MapView* mapview = fvw_get_view();
   if (mapview->GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
   {
      *is_interrupted = VARIANT_TRUE;
   }
   return S_OK;
}

STDMETHODIMP OsgEarthMapEngineCallback::raw_invalidate_overlays()
{
   // potentially need to clear out cached tiles
   return S_OK;
}

STDMETHODIMP OsgEarthMapEngineCallback::raw_draw_overlays(IActiveMapProj *map)
{
   C_ovl_mgr* manager = OVL_get_overlay_manager();
   CList<C_overlay *, C_overlay *> list;
   manager->get_overlay_list(list);

   OverlayEventRouter router(CListToVector<>(list),
      !manager->all_overlays_visible(),
      manager->get_current_overlay());

   router.RouteEventBottomToTop(nullptr, [map](C_overlay* overlay)
   {
      if ( overlay_type_util::HandleAsDrapedOverlay(overlay) )
      {
         // Set the overlay to invalid before drawing it.  This is necessary
         // because some internal overlays check the valid flag to decide
         // whether to regenerate rendering objects.  Do not use invalidate,
         // because that  will cause an infinite loop returning to this
         // location.
         overlay->set_valid(FALSE);
         overlay->OnDraw(map);
      }
      else
         overlay->OnDrawDraped(map);
   });

   return S_OK;
}

STDMETHODIMP OsgEarthMapEngineCallback::raw_draw_to_base_map(
   IActiveMapProj *map)
{
   // don't worry about this for right now
   return S_OK;
}

STDMETHODIMP OsgEarthMapEngineCallback::raw_get_redraw_overlays_from_scratch(
   VARIANT_BOOL *redraw_from_scratch)
{
   *redraw_from_scratch = VARIANT_TRUE;
   return S_OK;
}

STDMETHODIMP OsgEarthMapEngineCallback::raw_can_add_pixmaps_to_base_map(
   VARIANT_BOOL *can_add_pixmaps_to_base_map)
{
   *can_add_pixmaps_to_base_map = VARIANT_FALSE;
   return S_OK;
}
