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
#include "resource.h"
#include "ContextMenuItems.h"
#include "MapView.h"
#include "Overflow.h"
#include "utils.h"
#include "mapx.h"
#include "showrmk.h"
#include "MapEngineCOM.h"

// CCenterMapContextMenuItem
//

STDMETHODIMP CCenterMapContextMenuItem::get_MenuItemName(BSTR *pMenuItemName)
{
   *pMenuItemName = _bstr_t("Center Map").Detach();
   return S_OK;
}

STDMETHODIMP CCenterMapContextMenuItem::raw_MenuItemSelected()
{
   MapView* pMapView = static_cast<MapView*>(UTL_get_active_non_printing_view());
   if (pMapView != NULL)
      pMapView->OnCenter();

   return S_OK;
}

// ScaleInOutContextMenuItemBase
//

void ScaleInOutContextMenuItemBase::Initialize(const d_geo_t& cursor_pos_geo,
   bool cursor_pos_valid)
{
   m_cursor_position_geo = cursor_pos_geo;
   m_cursor_position_valid = cursor_pos_valid;
}

// CScaleInContextMenuItem
//

STDMETHODIMP CScaleInContextMenuItem::get_MenuItemName(BSTR *pMenuItemName)
{
   *pMenuItemName = _bstr_t("Scale In").Detach();
   return S_OK;
}

STDMETHODIMP CScaleInContextMenuItem::raw_MenuItemSelected()
{
   MapView* pMapView = static_cast<MapView*>(UTL_get_active_non_printing_view());
   if (pMapView != NULL)
   {
      if (m_cursor_position_valid)
         pMapView->ScaleIn(m_cursor_position_geo);
      else
         pMapView->ScaleIn();
   }
   return S_OK;
}

// CScaleOutContextMenuItem
//

STDMETHODIMP CScaleOutContextMenuItem::get_MenuItemName(BSTR *pMenuItemName)
{
   *pMenuItemName = _bstr_t("Scale Out").Detach();
   return S_OK;
}

STDMETHODIMP CScaleOutContextMenuItem::raw_MenuItemSelected()
{
   MapView* pMapView = static_cast<MapView*>(UTL_get_active_non_printing_view());
   if (pMapView != NULL)
   {
      if (m_cursor_position_valid)
         pMapView->ScaleOut(m_cursor_position_geo);
      else
         pMapView->ScaleOut();
   }

   return S_OK;
}

// COverlayOptionsContextMenuItem
//

STDMETHODIMP COverlayOptionsContextMenuItem::get_MenuItemName(BSTR *pMenuItemName)
{
   *pMenuItemName = _bstr_t("Overlay Options...").Detach();
   return S_OK;
}

STDMETHODIMP COverlayOptionsContextMenuItem::raw_MenuItemSelected()
{
   OVL_get_overlay_manager()->overlay_options_dialog();
   return S_OK;
}

// CNorthUpContextMenuItem
//

STDMETHODIMP CNorthUpContextMenuItem::get_MenuItemName(BSTR *pMenuItemName)
{
   *pMenuItemName = _bstr_t("North Up").Detach();
   return S_OK;
}

STDMETHODIMP CNorthUpContextMenuItem::raw_MenuItemSelected()
{
   MapView* pMapView = static_cast<MapView*>(UTL_get_active_non_printing_view());
   if (pMapView != NULL)
      pMapView->OnMapRotateNorthUp();
   return S_OK;
}

// CGetMapInfoContextMenuItem
//

STDMETHODIMP CGetMapInfoContextMenuItem::get_MenuItemName(BSTR *pMenuItemName)
{
   *pMenuItemName = _bstr_t("Get Map Info...").Detach();
   return S_OK;
}

STDMETHODIMP CGetMapInfoContextMenuItem::raw_MenuItemSelected()
{
   MapView* pMapView = static_cast<MapView*>(UTL_get_active_non_printing_view());
   if (pMapView == NULL)
      return S_OK;

   MapProj* curr_map = pMapView->get_curr_map();

   IDispatchPtr dispatch;
   pMapView->get_map_engine()->GetMapHandler(curr_map->source(), &dispatch);

   // query for map information interface
   IMapInformationPtr map_info = dispatch;

   if (map_info != NULL)
   {
      MapSource source = curr_map->source();
      MapScale scale = curr_map->scale();
      MapSeries series = curr_map->series();

      d_geo_t map_ll, map_ur;
      curr_map->get_vmap_bounds(&map_ll, &map_ur);

      MapInfoFormatEnum format_type;

      CString strInfo;
      try
      {
         bstr_t map_info_str = map_info->GetFormattedMapInfo(_bstr_t(source.get_string()), scale.GetScale(),
            scale.GetScaleUnits(), _bstr_t(series.get_string()), map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon,
            pMapView->m_mouse_down_lat, pMapView->m_mouse_down_lon, &format_type);

         CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, map_info_str, "Map Information", NULL, format_type);
      }
      catch(_com_error &e)
      {
         strInfo.Format("Error retrieving map information.  Reason: %s", (char *)e.Description());
         CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, strInfo, "Map Information", NULL);
      }
   }
   else
      CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, "No map information is available.", "Map Information", NULL);

   return S_OK;
}