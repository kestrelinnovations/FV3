// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// MapEngineCOM.cpp

#include "stdafx.h"

#include "MapEngineCOM.h"

#include <vector>
#include "mapview.h"
#include "mapx.h"
#include "refresh.h"
#include "catalog\MDSWrapper.h"
#include "getobjpr.h"
#include "TabularEditor\FVTabCtrl.h"
#include "maps.h"
#include "MapOptionsEntry.h"

CComModule _Module;

STDMETHODIMP MapEngineCallbackInterface::raw_is_draw_interrupted(
   VARIANT_BOOL *is_interrupted)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())

   *is_interrupted = FVW_is_draw_interrupted() ? VARIANT_TRUE : VARIANT_FALSE;

   return S_OK;
}

STDMETHODIMP MapEngineCallbackInterface::raw_invalidate_overlays()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())

   OVL_get_overlay_manager()->invalidate_overlays(FALSE);

   return S_OK;
}

STDMETHODIMP MapEngineCallbackInterface::raw_draw_overlays(IActiveMapProj *map)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())

   if (m_map_engine != nullptr) // FalconView may be closing
      OVL_get_overlay_manager()->draw(map, m_map_engine->m_bInternalDraw);

   return S_OK;
}

STDMETHODIMP MapEngineCallbackInterface::raw_draw_to_base_map(
   IActiveMapProj *map)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())

   OVL_get_overlay_manager()->draw_to_base_map(map);

   return S_OK;
}

STDMETHODIMP MapEngineCallbackInterface::raw_get_redraw_overlays_from_scratch(
   VARIANT_BOOL *redraw_from_scratch)
{
   if (m_map_engine != nullptr) // FalconView may be closing
   {
      *redraw_from_scratch
         = m_map_engine->get_redraw_overlays_from_scratch_when_animating() ?
         VARIANT_TRUE : VARIANT_FALSE;
   }
   else
   {
      *redraw_from_scratch = VARIANT_FALSE;
   }

   return S_OK;
}


STDMETHODIMP MapEngineCallbackInterface::raw_can_add_pixmaps_to_base_map(
   VARIANT_BOOL *can_add_pixmaps_to_base_map)
{
   *can_add_pixmaps_to_base_map =
      OVL_get_overlay_manager()->can_add_pixmaps_to_base_map() ?
      VARIANT_TRUE : VARIANT_FALSE;

   return S_OK;
}

STDMETHODIMP MapEngineCallbackInterface::raw_InvalidateMap()
{
   if (m_map_engine != nullptr) // FalconView may be closing
      m_map_engine->SetCurrentMapInvalid();

   MapView *view = fvw_get_view();
   if (view != nullptr)
      view->Invalidate();

   return S_OK;
}

STDMETHODIMP MapEngineCallbackInterface::raw_DrawTopMostOverlays(IActiveMapProj *map)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())

   if (m_map_engine != nullptr) // FalconView may be closing
      OVL_get_overlay_manager()->draw_top_most_overlays(
      map, m_map_engine->m_bInternalDraw);

   return S_OK;
}

MapEngineCOM::MapEngineCOM() :
   m_map_engine_callback(NULL)
{
   m_curr_map = NULL;

   m_current_map_invalid = FALSE;
   m_redraw_overlays_from_scratch_when_animating = FALSE;
   m_override_no_data = FALSE;

   m_CoverageInUse = false;
   m_MDSUpdateCached = false;
}

MapEngineCOM::~MapEngineCOM()
{
   if (m_map_engine_callback != NULL)
   {
      MapEngineCallbackInterface *callback
         = dynamic_cast<MapEngineCallbackInterface*>(m_map_engine_callback);

      if (callback != nullptr)
      {
         // set the internal MapEngineCOM pointer to null so that we can test
         // to be sure that this is valid when we receive callbacks after
         // destruction of this object
         callback->m_map_engine = nullptr;
      }

      m_map_engine_callback->Release();
   }
   delete m_curr_map;
}

void MapEngineCOM::CheckCachedMDSOperations()
{
   m_CoverageInUse = false;

   if (m_MDSUpdateCached)
   {
      CoverageUpdated();
      DataSourceOrderUpdated();
   }

   m_MDSUpdateCached = false;
}

// called before a map is changed
void MapEngineCOM::OnPreChangeMap()
{
   // cancel any map tabs update
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      CMapTabsBar *pMapTabs = pFrame->GetMapTabsBar();
      if (pMapTabs != NULL)
         pMapTabs->CancelUpdateTabs();
   }
}

int MapEngineCOM::init(BOOL bCopyMapOptionsFromView /*= TRUE*/,
   BOOL bInitCallback /*=TRUE*/,
   BOOL bInitMapOptions /*= FALSE*/,
   BOOL bInternalDraw /*=FALSE*/,
   HWND output_wnd /*= NULL*/,
   BOOL allow_invalid_points_on_surface /* = FALSE */)
{
   CComObject<MapEngineCallbackInterface>* map_engine_callback = nullptr;
   if ( bInitCallback )
   {
      CComObject<MapEngineCallbackInterface>::CreateInstance(
         &map_engine_callback);
      map_engine_callback->m_map_engine = this;
   }

   return init(map_engine_callback, bCopyMapOptionsFromView, bInitMapOptions,
      bInternalDraw, output_wnd, allow_invalid_points_on_surface);
}

int MapEngineCOM::init(IMapRenderingEngineCallback* callback,
   BOOL bCopyMapOptionsFromView /*= TRUE*/, BOOL bInitMapOptions /*= FALSE*/,
   BOOL bInternalDraw /*=FALSE*/, HWND output_wnd /*= NULL*/,
   BOOL allow_invalid_points_on_surface /* = FALSE */)
{
   m_curr_map = new MapProjImpl();
   if (m_curr_map == NULL)
   {
      ERR_report("error allocating map");
      return FAILURE;
   }

   HRESULT hr = m_map_rendering_engine.CreateInstance(
      __uuidof(MapRenderingEngine));

   if (FAILED(hr))
   {
      AfxMessageBox("Failed to create an instance of the map rendering engine."
         "MapRenderingEngine.dll may need to be registered.");
      return FAILURE;
   }

   try
   {
      m_map_rendering_engine2 = m_map_rendering_engine;

      if (callback != nullptr)
      {
         m_map_engine_callback = callback;
         m_map_engine_callback->AddRef();
      }

      m_map_rendering_engine->Init(m_map_engine_callback);
      m_map_rendering_engine->m_allowInvalidPointsOnSurface =
         allow_invalid_points_on_surface ? VARIANT_TRUE : VARIANT_FALSE;

      if (m_map_rendering_engine2 != NULL && output_wnd != NULL)
         m_map_rendering_engine2->Initialize(reinterpret_cast<long>(output_wnd),
         GRAPHICS_CONTEXT_TYPE_D2D);

      m_map_rendering_engine->GetCurrentMap(&m_curr_map->m_map);
      m_curr_map->m_map2 = m_curr_map->m_map;

      if (bInitMapOptions)
         init_map_options();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Map rendering engine Init failed: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   if (bCopyMapOptionsFromView)
   {
      MapEngineCOM *pActiveViewMapEngine =
         reinterpret_cast<MapEngineCOM*>(MapView::GetActiveViewMapEngine());

      if (pActiveViewMapEngine != NULL)
      {
         const std::vector<MAP_OPTIONS_ENTRY>& listOptions =
            pActiveViewMapEngine->GetOptionsList();

         const size_t size = listOptions.size();
         for (size_t i = 0; i < size; ++i)
         {
            const MAP_OPTIONS_ENTRY& entry = listOptions[i];

            try
            {
               if (entry.map_render_options != NULL)
               {
                  entry.map_render_options->CopyProperties(
                     m_map_rendering_engine,
                     pActiveViewMapEngine->GetRenderingEngine());
               }
            }
            catch(_com_error &e)
            {
               CString msg;
               msg.Format("Unable to copy map properties [%s] from the view's "
                  "map rendering engine : %s",
                  entry.class_id.c_str(), (char *)e.Description());
               ERR_report(msg);
            }
         }

         // copy brightness from view
         set_brightness(pActiveViewMapEngine->get_brightness());
      }
   }

   double dPixelsPerInch = PRM_get_registry_double("CTIA", "PPI", 96.0);
   SetPixelsPerInchForToScaleZoom(dPixelsPerInch);

   m_bInternalDraw = bInternalDraw;

   return SUCCESS;
}

int MapEngineCOM::init_map_options()
{
   int i = 1;
   while (i <= 10)
   {
      CString strKey;
      strKey.Format("Map Options\\%d", i);

      MAP_OPTIONS_ENTRY map_options_entry;

      map_options_entry.class_id =
         PRM_get_registry_string(strKey, "classIdString", "");

      // if the class id string exists...
      if (map_options_entry.class_id.size() > 0)
      {
         try
         {
            CLSID clsid;

            THROW_IF_NOT_OK(CLSIDFromString(_bstr_t(
               map_options_entry.class_id.c_str()), &clsid));
            THROW_IF_NOT_OK(CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER,
               fvw::IID_IMapRenderOptions,
               reinterpret_cast<void **>(
               &map_options_entry.map_render_options)));

            map_options_entry.map_render_options->Init(m_map_rendering_engine);
            m_map_options_list.push_back(map_options_entry);
         }
         catch (_com_error &e)
         {
            CString msg;
            msg.Format("Error initializing map options %s: %s",
               map_options_entry.class_id.c_str(), (char *)e.Description());
            ERR_report(msg);
         }
      }

      i++;
   }

   return SUCCESS;
}

const std::vector<MAP_OPTIONS_ENTRY>& MapEngineCOM::GetOptionsList() const
{
   return m_map_options_list;
}

int MapEngineCOM::uninit()
{
   int ret = SUCCESS;

   if (m_map_rendering_engine != NULL)
      m_map_rendering_engine.Release();

   try
   {
      m_map_options_list.clear();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error: %d", (char *)e.Description());
      ERR_report(msg);
      ret = FAILURE;
   }

   return ret;
}

// call set_view_dimensions whenever the size of the window that the map is
// being drawn to changes typically in your framework's OnSize
//
void MapEngineCOM::set_view_dimensions(double width, double height,
   SurfaceUnitsEnum eUnits /*= SURFACE_UNITS_PIXELS*/)
{
   try
   {
      m_map_rendering_engine->SetSurfaceDimensions(width, height, eUnits);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
   }
}

void MapEngineCOM::SetViewport(double center_lat, double center_lon,
   double width, double height,
      SurfaceUnitsEnum eSurfaceUnits /*= SURFACE_UNITS_PIXELS*/)
{
   try
   {
      m_map_rendering_engine->SetViewport(center_lat, center_lon, width, height,
         eSurfaceUnits);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
   }
}

// setting this will force the map to draw from scratch the next time the map is
// drawn
void MapEngineCOM::SetCurrentMapInvalid()
{
   try
   {
      MapStatusCodeEnum status = m_map_rendering_engine->SetCurrentMapInvalid(
         VARIANT_TRUE);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
   }
}

int MapEngineCOM::GetCurrentGroupIdentity()
{
   int nGroupIdentity = -1;
   try
   {
      nGroupIdentity = m_map_rendering_engine->m_nCrntGroupIdentity;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
   }

   return nGroupIdentity;
}

void MapEngineCOM::ConvertScale(MapScale scale, double *pScale,
   MapScaleUnitsEnum *pScaleUnits)
{
   if (scale == WORLD)
   {
      *pScale = 1.0;
      *pScaleUnits = MAP_SCALE_WORLD;
      return;
   }

   switch (scale.get_resolution_unit())
   {
   case MapScale::NM: *pScaleUnits = MAP_SCALE_NM; break;
   case MapScale::MILE: *pScaleUnits = MAP_SCALE_MILE; break;
   case MapScale::KILOMETER: *pScaleUnits = MAP_SCALE_KILOMETER; break;
   case MapScale::METERS: *pScaleUnits = MAP_SCALE_METERS; break;
   case MapScale::YARDS: *pScaleUnits = MAP_SCALE_YARDS; break;
   case MapScale::FEET: *pScaleUnits = MAP_SCALE_FEET; break;
   case MapScale::INCHES: *pScaleUnits = MAP_SCALE_INCHES; break;
   case MapScale::ARC_DEGREES: *pScaleUnits = MAP_SCALE_ARC_DEGREES; break;
   case MapScale::ARC_MINUTES: *pScaleUnits = MAP_SCALE_ARC_MINUTES; break;
   case MapScale::ARC_SECONDS: *pScaleUnits = MAP_SCALE_ARC_SECONDS; break;
   };

   if (scale.get_resolution() == 0.0)
   {
      *pScaleUnits = MAP_SCALE_DENOMINATOR;
      *pScale = scale.get_scale();
   }
   else
      *pScale = scale.get_resolution();
}

MapScaleUnitsEnum MapEngineCOM::convert_units(MapScale::unit_t units)
{
   switch (units)
   {
   case MapScale::NM: return MAP_SCALE_NM;
   case MapScale::MILE: return MAP_SCALE_MILE;
   case MapScale::KILOMETER: return MAP_SCALE_KILOMETER;
   case MapScale::METERS: return MAP_SCALE_METERS;
   case MapScale::YARDS: return MAP_SCALE_YARDS;
   case MapScale::FEET: return MAP_SCALE_FEET;
   case MapScale::INCHES: return MAP_SCALE_INCHES;
   case MapScale::ARC_DEGREES: return MAP_SCALE_ARC_DEGREES;
   case MapScale::ARC_MINUTES: return MAP_SCALE_ARC_MINUTES;
   case MapScale::ARC_SECONDS: return MAP_SCALE_ARC_SECONDS;
   };

   ERR_report("Unknown units");
   return MAP_SCALE_NM;
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::change_map_type(const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   degrees_t latitude, degrees_t longitude,
   double rotation, int zoom_percent, ProjectionEnum projection_type,
   boolean_t overide_no_data /*=FALSE*/,
   boolean_t clear_if_not_success)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());
      long mask = SOURCE_VALID | SCALE_VALID |
         SERIES_VALID | LOCATION_VALID |
         ROTATION_VALID | ZOOM_VALID |
         PROJECTION_VALID;

      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;
      VARIANT_BOOL override = overide_no_data ? VARIANT_TRUE : VARIANT_FALSE;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->SetMapType(map_source,
         dScale, eScaleUnits, map_series, latitude, longitude, rotation,
         zoom_percent, projection_type, mask, clear, override);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_map_type(int nGroupIdentity, const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   degrees_t latitude, degrees_t longitude,
   double rotation, int zoom_percent, ProjectionEnum projection_type,
   boolean_t overide_no_data /*= FALSE*/,
   boolean_t clear_if_not_success /*=TRUE*/)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());
      long mask = SOURCE_VALID | SCALE_VALID |
         SERIES_VALID | LOCATION_VALID |
         ROTATION_VALID | ZOOM_VALID |
         PROJECTION_VALID;

      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;
      VARIANT_BOOL override = overide_no_data ? VARIANT_TRUE : VARIANT_FALSE;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         nGroupIdentity, map_source, dScale, eScaleUnits, map_series,
         latitude, longitude, rotation, zoom_percent,
         projection_type, mask, clear, override);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_map_type(int nGroupIdentity, const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   degrees_t latitude, degrees_t longitude,
   boolean_t overide_no_data /*= FALSE*/,
   boolean_t clear_if_not_success /*=TRUE*/)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());
      long mask = SOURCE_VALID | SCALE_VALID |
         SERIES_VALID | LOCATION_VALID;

      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;
      VARIANT_BOOL override_flag =
         overide_no_data ? VARIANT_TRUE : VARIANT_FALSE;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         nGroupIdentity, map_source, dScale, eScaleUnits, map_series,
         latitude, longitude, 0.0, 100, EQUALARC_PROJECTION, mask, clear,
         override_flag);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::change_map_type(const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   degrees_t latitude, degrees_t longitude,
   boolean_t clear_if_not_success)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());
      long mask = SOURCE_VALID | SCALE_VALID |
         SERIES_VALID | LOCATION_VALID;

      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->SetMapType(map_source,
         dScale, eScaleUnits, map_series, latitude, longitude, 0.0, 0,
         EQUALARC_PROJECTION, mask, clear, VARIANT_FALSE);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_map_type(const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   boolean_t clear_if_not_success, boolean_t override_no_data)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());
      long mask = SOURCE_VALID | SCALE_VALID |
         SERIES_VALID;

      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;
      VARIANT_BOOL override = override_no_data ? VARIANT_TRUE : VARIANT_FALSE;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->SetMapType(map_source,
         dScale, eScaleUnits, map_series, 0.0, 0.0, 0.0, 0,
         EQUALARC_PROJECTION, mask, clear, override);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_map_type(int nGroupIdentity, const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   boolean_t clear_if_not_success /*=TRUE*/,
   boolean_t override_no_data /*=FALSE*/)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());
      long mask = SOURCE_VALID | SCALE_VALID | SERIES_VALID;

      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;
      VARIANT_BOOL override = override_no_data ? VARIANT_TRUE : VARIANT_FALSE;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         nGroupIdentity, map_source, dScale, eScaleUnits, map_series,
         0.0, 0.0, 0.0, 0, EQUALARC_PROJECTION, mask, clear, override);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::scale_in(degrees_t lat, degrees_t lon,
   boolean_t display_wait_cursor /*= TRUE*/,
   boolean_t clear_if_not_success)
{
   try
   {
      CWaitCursor* pWaitCursor = NULL;
      if (display_wait_cursor)
         pWaitCursor = new CWaitCursor;

      OnPreChangeMap();

      const VARIANT_BOOL lat_lon_valid = VARIANT_TRUE;
      MapStatusCodeEnum status = m_map_rendering_engine->ScaleIn(lat, lon,
         lat_lon_valid, clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      delete pWaitCursor;

      SetCurrentMapInvalid();

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::scale_in(boolean_t display_wait_cursor /*= TRUE*/,
   boolean_t clear_if_not_success)
{
   try
   {
      CWaitCursor* pWaitCursor = NULL;
      if (display_wait_cursor)
         pWaitCursor = new CWaitCursor;

      OnPreChangeMap();

      const VARIANT_BOOL lat_lon_valid = VARIANT_FALSE;
      MapStatusCodeEnum status = m_map_rendering_engine->ScaleIn(0.0, 0.0,
         lat_lon_valid, clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      delete pWaitCursor;

      SetCurrentMapInvalid();

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::scale_out(degrees_t lat, degrees_t lon,
   boolean_t display_wait_cursor /*= TRUE*/,
   boolean_t clear_if_not_success /*=TRUE*/, boolean_t *min_scale /*= NULL*/)
{
   if (min_scale)
      *min_scale = FALSE;

   try
   {
      CWaitCursor* pWaitCursor = NULL;
      if (display_wait_cursor)
         pWaitCursor = new CWaitCursor;

      OnPreChangeMap();

      const VARIANT_BOOL lat_lon_valid = VARIANT_TRUE;
      MapStatusCodeEnum status = m_map_rendering_engine->ScaleOut(lat, lon,
         lat_lon_valid, clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      delete pWaitCursor;

      SetCurrentMapInvalid();

      if (status == MAP_STATUS_MIN_SCALE && min_scale)
         *min_scale = TRUE;

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::scale_out(boolean_t display_wait_cursor /*= TRUE*/,
   boolean_t clear_if_not_success /*=TRUE*/,
   boolean_t *min_scale /*= NULL*/)
{
   if (min_scale)
      *min_scale = FALSE;

   try
   {
      CWaitCursor* pWaitCursor = NULL;
      if (display_wait_cursor)
         pWaitCursor = new CWaitCursor;

      OnPreChangeMap();

      const VARIANT_BOOL lat_lon_valid = VARIANT_FALSE;
      MapStatusCodeEnum status = m_map_rendering_engine->ScaleOut(0.0, 0.0,
         lat_lon_valid, clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      delete pWaitCursor;

      SetCurrentMapInvalid();

      if (status == MAP_STATUS_MIN_SCALE && min_scale)
         *min_scale = TRUE;

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC or FV_NO_DATA.
//
int MapEngineCOM::change_to_closest_scale(MapCategory cat,
   const MapSource &source, MapScale scale, MapSeries series,
   double lat, double lon, double rot_angle,
   int zoom_percent, ProjectionEnum proj_type)
{
   try
   {
      _bstr_t category = _bstr_t(MapCategory(cat).get_string());
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());

      long mask = CATEGORY_VALID |
         SOURCE_VALID | SCALE_VALID |
         SERIES_VALID | LOCATION_VALID |
         ROTATION_VALID | ZOOM_VALID |
         PROJECTION_VALID;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToClosestScale(
         category, map_source, dScale, eScaleUnits, map_series, lat, lon,
         rot_angle, zoom_percent, proj_type, mask, VARIANT_TRUE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_closest_scale(const MapSource &source,
      MapScale curr_scale, degrees_t center_lat, degrees_t center_lon,
      double rot_angle, int zoom_percent, ProjectionEnum proj_type)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(curr_scale, &dScale, &eScaleUnits);

      long mask = SOURCE_VALID | SCALE_VALID |
         LOCATION_VALID |
         ROTATION_VALID | ZOOM_VALID |
         PROJECTION_VALID;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToClosestScale(
         _bstr_t(MAP_get_category(source).get_string()), map_source, dScale,
         eScaleUnits, _bstr_t(""), center_lat, center_lon, rot_angle,
         zoom_percent, proj_type, mask, VARIANT_TRUE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_closest_scale(MapCategory cat,
      MapScale curr_scale, degrees_t center_lat, degrees_t center_lon,
      double rot_angle, int zoom_percent, ProjectionEnum proj_type)
{
   try
   {
      _bstr_t category = _bstr_t(MapCategory(cat).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(curr_scale, &dScale, &eScaleUnits);

      long mask = CATEGORY_VALID |
         SCALE_VALID | LOCATION_VALID |
         ROTATION_VALID | ZOOM_VALID |
         PROJECTION_VALID;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToClosestScale(
         category, _bstr_t(""), dScale, eScaleUnits, _bstr_t(""),
         center_lat, center_lon, rot_angle, zoom_percent, proj_type, mask,
         VARIANT_TRUE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_closest_scale(MapCategory cat, degrees_t center_lat,
      degrees_t center_lon)
{
   try
   {
      _bstr_t category = _bstr_t(MapCategory(cat).get_string());

      long mask = CATEGORY_VALID | LOCATION_VALID;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToClosestScale(
         category, _bstr_t(""), 0.0, MAP_SCALE_NM,
         _bstr_t(""), center_lat, center_lon, 0.0, 0, EQUALARC_PROJECTION,
         mask, VARIANT_TRUE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_closest_scale(const MapSource &source,
   const MapSeries &series, MapScale curr_scale,
   degrees_t center_lat, degrees_t center_lon,
   double rot_angle, int zoom_percent, ProjectionEnum proj_type)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(curr_scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());

      long mask = SOURCE_VALID |
         SCALE_VALID | SERIES_VALID |
         LOCATION_VALID |
         ROTATION_VALID | ZOOM_VALID |
         PROJECTION_VALID;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToClosestScale(
         _bstr_t(MAP_get_category(source).get_string()), map_source, dScale,
         eScaleUnits, map_series, center_lat, center_lon, rot_angle,
         zoom_percent, proj_type, mask, VARIANT_TRUE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_closest_scale(MapCategory cat)
{
   try
   {
      _bstr_t category = _bstr_t(MapCategory(cat).get_string());

      long mask = CATEGORY_VALID;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToClosestScale(
         category, _bstr_t(""), 0.0, MAP_SCALE_NM,
         _bstr_t(""), 0.0, 0.0, 0.0, 0, EQUALARC_PROJECTION,
         mask, VARIANT_TRUE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_closest_scale(const MapSource &source,
   MapScale curr_scale)
{
   try
   {
      long mask = SOURCE_VALID | SCALE_VALID;

      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(curr_scale, &dScale, &eScaleUnits);

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToClosestScale(
         _bstr_t(MAP_get_category(source).get_string()), map_source, dScale,
         eScaleUnits, _bstr_t(""), 0.0, 0.0, 0.0, 0, EQUALARC_PROJECTION,
         mask, VARIANT_TRUE);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_smallest_scale(MapCategory cat,
   degrees_t center_lat, degrees_t center_lon,
   double rot_angle, int zoom_percent, ProjectionEnum proj_type)
{
   try
   {
      _bstr_t category = _bstr_t(MapCategory(cat).get_string());

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToSmallestScale(
         category, center_lat, center_lon, rot_angle, zoom_percent, proj_type);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_largest_scale(MapCategory cat, degrees_t center_lat,
   degrees_t center_lon, double rot_angle, int zoom_percent,
   ProjectionEnum proj_type)
{
   try
   {
      _bstr_t category = _bstr_t(MapCategory(cat).get_string());

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToLargestScale(
         category, center_lat, center_lon, rot_angle, zoom_percent, proj_type);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_best(degrees_t lat, degrees_t lon,
                                       boolean_t clear_if_not_success/*=TRUE*/)
{
   try
   {
      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;

      long mask = LOCATION_VALID;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToBest(
         _bstr_t(""), 0.0, MAP_SCALE_NM, lat, lon, 0.0, 0,
         EQUALARC_PROJECTION, mask, clear);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::change_to_best(degrees_t lat, degrees_t lon,
   const MapSource &source, const MapScale &scale, double rot, int zoom_percent,
   ProjectionEnum proj_type, boolean_t clear_if_not_success/*=TRUE*/)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;

      long mask = SOURCE_VALID | SCALE_VALID |
         LOCATION_VALID |
         ROTATION_VALID | ZOOM_VALID |
         PROJECTION_VALID;

      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->ChangeToBest(
         map_source, dScale, eScaleUnits, lat, lon, rot, zoom_percent,
         proj_type, mask, clear);

      if (status == MAP_STATUS_OK)
      {
         m_map_rendering_engine->ApplyMap();
      }

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::set_map_type_from_geo_bounds(const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   double southern_lat, double western_lon,
   double northern_lat, double eastern_lon, double rotation, int zoom_percent,
   ProjectionEnum projection, boolean_t overide_no_data,
   double *surface_width, double *surface_height,
   boolean_t clear_if_not_success/*=TRUE*/)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());

      VARIANT_BOOL clear = clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE;
      VARIANT_BOOL override = overide_no_data ? VARIANT_TRUE : VARIANT_FALSE;

      MapStatusCodeEnum status =
         m_map_rendering_engine->SetMapTypeFromGeoBounds(map_source, dScale,
         eScaleUnits, map_series, southern_lat, western_lon, northern_lat,
         eastern_lon, rotation, zoom_percent, projection, clear, override,
         surface_width, surface_height);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::set_fixed_size_map_type_from_geo_bounds(const MapSource &source,
      const MapScale &scale, const MapSeries &series,
      double southern_lat, double western_lon,
      double northern_lat, double eastern_lon, int surface_width,
      int surface_height)
{
   try
   {
      _bstr_t map_source = _bstr_t(MapSource(source).get_string());

      double dScale;
      MapScaleUnitsEnum eScaleUnits;
      ConvertScale(scale, &dScale, &eScaleUnits);

      _bstr_t map_series = _bstr_t(MapSeries(series).get_string());

      MapStatusCodeEnum status =
         m_map_rendering_engine2->SetFixedSizeMapTypeFromGeoBounds(map_source,
         dScale, eScaleUnits, map_series,
         southern_lat, western_lon, northern_lat, eastern_lon,
         surface_width, surface_height);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::change_center(degrees_t lat, degrees_t lon,
   boolean_t clear_if_not_success)
{
   try
   {
      OnPreChangeMap();
      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         GetCurrentGroupIdentity(), _bstr_t(""), 0, MAP_SCALE_NM,
         _bstr_t(""), lat, lon, 0.0, 0, EQUALARC_PROJECTION, LOCATION_VALID,
         clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE, VARIANT_FALSE);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::change_rotation(double angle, degrees_t lat, degrees_t lon,
   boolean_t clear_if_not_success)
{
   try
   {
      OnPreChangeMap();
      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         GetCurrentGroupIdentity(),
         _bstr_t(""), 0, MAP_SCALE_NM,
         _bstr_t(""), lat, lon, angle, 0, EQUALARC_PROJECTION,
         LOCATION_VALID | ROTATION_VALID,
         clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE, VARIANT_FALSE);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::change_rotation(double angle, boolean_t clear_if_not_success)
{
   try
   {
      OnPreChangeMap();
      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         GetCurrentGroupIdentity(),
         _bstr_t(""), 0, MAP_SCALE_NM,
         _bstr_t(""), 0.0, 0.0, angle, 0, EQUALARC_PROJECTION,
         ROTATION_VALID,
         clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE, VARIANT_FALSE);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::change_projection(ProjectionEnum projection,
   boolean_t clear_if_not_success)
{
   try
   {
      OnPreChangeMap();

      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         GetCurrentGroupIdentity(),
         _bstr_t(""), 0, MAP_SCALE_NM,
         _bstr_t(""), 0.0, 0.0, 0.0, 0, projection, PROJECTION_VALID,
         clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE, VARIANT_FALSE);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::change_zoom_percent(int zoom_percent,
   boolean_t clear_if_not_success)
{
   try
   {
      OnPreChangeMap();
      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         GetCurrentGroupIdentity(),
         _bstr_t(""), 0, MAP_SCALE_NM,
         _bstr_t(""), 0.0, 0.0, 0.0, zoom_percent, EQUALARC_PROJECTION,
         ZOOM_VALID,
         clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE, VARIANT_FALSE);

      // reset to scale factor
      SetToScaleFactor(1.0);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::change_zoom_percent(int zoom_percent,
   degrees_t lat, degrees_t lon,
   boolean_t clear_if_not_success)
{
   try
   {
      OnPreChangeMap();
      MapStatusCodeEnum status = m_map_rendering_engine->SetMapTypeInGroup(
         GetCurrentGroupIdentity(),
         _bstr_t(""), 0, MAP_SCALE_NM,
         _bstr_t(""), lat, lon, 0.0, zoom_percent, EQUALARC_PROJECTION,
         LOCATION_VALID | ZOOM_VALID,
         clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE, VARIANT_FALSE);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//////////////////////////////////////////////////////////////////////////
// view-relative map change functions
//////////////////////////////////////////////////////////////////////////

int MapEngineCOM::change_center(int x, int y, boolean_t clear_if_not_success)
{
   try
   {
      OnPreChangeMap();
      MapStatusCodeEnum status = m_map_rendering_engine->ChangeCenter(x, y,
         clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int MapEngineCOM::pan_percent(double percent_width, double percent_height,
   double percent_of_panning, boolean_t clear_if_not_success)
{
   try
   {
      OnPreChangeMap();
      MapStatusCodeEnum status = m_map_rendering_engine->PanPercent(
         percent_width, percent_height, percent_of_panning,
         clear_if_not_success ? VARIANT_TRUE : VARIANT_FALSE);

      return HandleMapStatusCode(status);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

void MapEngineCOM::set_brightness(double bright)
{
   try
   {
      m_map_rendering_engine->SetBrightness(bright);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("SetBrightness failed : %s", (char *)e.Description());
      ERR_report(msg);
   }
}

double MapEngineCOM::get_brightness()
{
   try
   {
      double bright;
      m_map_rendering_engine->GetBrightness(&bright);
      return bright;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("SetBrightness failed : %s", (char *)e.Description());
      ERR_report(msg);
      return 0.0;
   }
}

void MapEngineCOM::set_contrast(double contrast, int contrast_midval)
{
   try
   {
      m_map_rendering_engine->SetContrast(contrast, contrast_midval);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("SetContrast failed : %s", (char *)e.Description());
      ERR_report(msg);
   }
}

void MapEngineCOM::calculate_brightness_contrast(double* brightness,
   double* contrast, int* midval)
{
   try
   {
      MapSource map_source = m_curr_map->source();
      if (map_source == "CIB")
      {
         IRPFMapRenderOptionsPtr smpRPFOptions =
            m_map_rendering_engine->GetMapHandler(_bstr_t("CIB"));
         smpRPFOptions->CalculateAutoBrightContrast(brightness, contrast, midval);
      }
      else
      {
         IGeoTiffMapRenderOptionsPtr smpGeoTiffOptions =
            m_map_rendering_engine->GetMapHandler(_bstr_t("GeoTIFF"));
         smpGeoTiffOptions->CalculateAutoBrightContrast(
            brightness, contrast, midval);
      }
      set_brightness(*brightness);
      set_contrast(*contrast, *midval);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("CalculateAutoBrightContrast failed : %s",
         (char *)e.Description());
      ERR_report(msg);
   }
}

int MapEngineCOM::draw_map(CDC* pDC, boolean_t display_wait_cursor,
   boolean_t *map_drawn_from_scratch)
{
   m_CoverageInUse = true;

   *map_drawn_from_scratch = 0;

   long hDC = reinterpret_cast<long>(pDC->m_hDC);

   VARIANT_BOOL wait_cursor_on =
      display_wait_cursor ? VARIANT_TRUE : VARIANT_FALSE;

   try
   {
      CComBSTR bstrOutOfSyncDSs, bstrOfflineDSs;
      MapStatusCodeEnum status = m_map_rendering_engine->DrawMap(
         hDC, wait_cursor_on, (long *)map_drawn_from_scratch,
         &bstrOutOfSyncDSs, &bstrOfflineDSs);

      if (status != MAP_STATUS_OK)
      {
         ERR_report("DrawMap failed");
         CheckCachedMDSOperations();
         return FAILURE;
      }

      HandleOfflineDataSources(bstrOutOfSyncDSs, bstrOfflineDSs);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error drawing : %s", (char *)e.Description());
      ERR_report(msg);
      CheckCachedMDSOperations();
      return FAILURE;
   }

   SetMapSpec();
   CheckCachedMDSOperations();

   return SUCCESS;
}

void MapEngineCOM::SetMapSpec()
{
   // this can be removed after all occurences of getting the spec() from
   // FV are changed to ask the map directly for the map parameters
   MapProjImpl * map = reinterpret_cast<MapProjImpl *>(m_curr_map);
   map->get_actual_center(&map->m_map_spec.center);
   map->m_map_spec.source = map->source();
   map->m_map_spec.scale = map->scale();
   map->m_map_spec.series = map->series();
   map->m_map_spec.rotation = map->actual_rotation();
   map->m_map_spec.zoom_percent = map->actual_zoom_percent();
   if (map->requested_zoom_percent() == NATIVE_ZOOM_PERCENT)
      map->m_map_spec.zoom_percent = NATIVE_ZOOM_PERCENT;
   else if (map->requested_zoom_percent() == TO_SCALE_ZOOM_PERCENT)
      map->m_map_spec.zoom_percent = TO_SCALE_ZOOM_PERCENT;
   map->m_map_spec.projection_type = map->projection_type();
   map->m_map_spec.m_dBrightness = map->m_map->get_brightness();
   map->m_map->get_contrast(
      &map->m_map_spec.m_dContrast, &map->m_map_spec.m_nContrastMidval);
}

int MapEngineCOM::print_map(CDC* dc, boolean_t is_printing_to_scale)
{
   m_CoverageInUse = true;

   long hDC = reinterpret_cast<long>(dc->m_hDC);
   long hAttribDC = reinterpret_cast<long>(dc->m_hAttribDC);

   try
   {
      m_map_rendering_engine->m_printToScale =
         is_printing_to_scale ? VARIANT_TRUE : VARIANT_FALSE;

      CComBSTR bstrOutOfSyncDSs, bstrOfflineDSs;

      MapStatusCodeEnum status = m_map_rendering_engine->PrintMap(
         hDC, hAttribDC, &bstrOutOfSyncDSs, &bstrOfflineDSs);

      HandleOfflineDataSources(bstrOutOfSyncDSs, bstrOfflineDSs);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error printing : %s", (char *)e.Description());
      ERR_report(msg);

      CheckCachedMDSOperations();
      return FAILURE;
   }

   CheckCachedMDSOperations();
   return SUCCESS;
}

int MapEngineCOM::copy_map_to_clipboard()
{
   try
   {
      m_map_rendering_engine->CopyMapToClipboard();
      return SUCCESS;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::save_BMP(const CString &filename)
{
   try
   {
      m_map_rendering_engine->SaveBMP(_bstr_t(filename));
      return SUCCESS;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::save_PNG(const CString &filename)
{
   try
   {
      m_map_rendering_engine->SavePNG(_bstr_t(filename));
      return SUCCESS;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::save_JPG(const CString &filename, int quality)
{
   try
   {
      m_map_rendering_engine->SaveJPG(_bstr_t(filename), quality);
      return SUCCESS;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::save_GeoTIFF(const CString &filename)
{
   try
   {
      m_map_rendering_engine->SaveGeoTIFF(_bstr_t(filename));
      return SUCCESS;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::save_KMZ(const CString &filename)
{
   try
   {
      m_map_rendering_engine2->SaveKMZ(_bstr_t(filename));
      return SUCCESS;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapEngineCOM: (%s)",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
}

int MapEngineCOM::GetMapHandler(const MapSource &source, IDispatch **handler)
{
   try
   {
      *handler = m_map_rendering_engine->GetMapHandler(
         _bstr_t(source.get_string())).Detach();
   }
   catch(_com_error &e)
   {
      *handler = NULL;

      CString msg;
      msg.Format("Error: %s", (char *)e.Description());
      ERR_report(msg);

      return FAILURE;
   }

   return SUCCESS;
}

int MapEngineCOM::GetMapHandlerByGuid(const GUID& map_handler_guid,
   IDispatch** handler)
{
   try
   {
      *handler = m_map_rendering_engine2->GetMapHandlerByGuid(
         map_handler_guid);

      return SUCCESS;
   }
   catch (_com_error& e)
   {
      *handler = NULL;
      REPORT_COM_ERROR(e);
      return FAILURE;
   }
}

int MapEngineCOM::SetIsPrinting(BOOL isPrinting, BOOL printToScale)
{
   try
   {
      m_map_rendering_engine->m_isPrinting =
         isPrinting ? VARIANT_TRUE : VARIANT_FALSE;
      m_map_rendering_engine->m_printToScale =
         printToScale ? VARIANT_TRUE : VARIANT_FALSE;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (SetIsPrinting): %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return SUCCESS;
}

int MapEngineCOM::DataCheckEnabled(BOOL enabled)
{
   try
   {
      m_map_rendering_engine->m_dataCheckEnabled =
         enabled ? VARIANT_TRUE : VARIANT_FALSE;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (DataCheckEnabled): %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return SUCCESS;
}

int MapEngineCOM::SetPixelsPerInchForToScaleZoom(double dPixelsPerInch)
{
   try
   {
      m_map_rendering_engine->m_dPixelsPerInchForToScaleZoom = dPixelsPerInch;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (SetPixelsPerInchForToScaleZoom): %s",
         (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return SUCCESS;
}

int MapEngineCOM::SetToScaleFactor(double dToScaleFactor)
{
   try
   {
      m_map_rendering_engine->m_dToScaleFactor = dToScaleFactor;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (SetToScaleFactor): %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return SUCCESS;
}

int MapEngineCOM::GetToScaleFactor(double* pToScaleFactor)
{
   try
   {
      *pToScaleFactor = m_map_rendering_engine->m_dToScaleFactor;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (SetToScaleFactor): %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return SUCCESS;
}

int MapEngineCOM::ApplyMap()
{
   try
   {
      m_map_rendering_engine->ApplyMap();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (ApplyMap): %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return SUCCESS;
}

int MapEngineCOM::DrawMapToDIB(VARIANT *dib)
{
   MapStatusCodeEnum status;
   try
   {
      status = m_map_rendering_engine->DrawMapToDIB(dib);
      if (status != SUCCESS)
      {
         ERR_report("Unable to get DIB");
         return FAILURE;
      }
      m_map_rendering_engine->ApplyMap();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (DrawMapToDIB): %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return HandleMapStatusCode(status);
}

int MapEngineCOM::DrawMapToResizedDIB(int nPixmapWidth, int nPixmapHeight,
   VARIANT * dib)
{
   MapStatusCodeEnum status;
   try
   {
      status = m_map_rendering_engine->DrawMapToResizedDIB(nPixmapWidth,
         nPixmapHeight, dib);
      if (status != SUCCESS)
      {
         ERR_report("Unable to get DIB");
         return FAILURE;
      }
      m_map_rendering_engine->ApplyMap();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (DrawMapToResizedDIB): %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return HandleMapStatusCode(status);
}

int MapEngineCOM::DrawMapToRGBA(unsigned char* image)
{
   MapStatusCodeEnum status;
   try
   {
      status = m_map_rendering_engine2->DrawMapToRGBA(image);
      if (status != SUCCESS)
      {
         ERR_report("Unable to draw map to RGBA");
         return FAILURE;
      }
      m_map_rendering_engine->ApplyMap();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (DrawMapToRGBA): %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return HandleMapStatusCode(status);
}

void MapEngineCOM::DataSourceOrderUpdated()
{
   if (m_CoverageInUse)
   {
      m_MDSUpdateCached = true;
      return;
   }

   IMDSUtilNotifyEventsPtr smpMDSUtilNotifyEvents = m_map_rendering_engine;
   if (smpMDSUtilNotifyEvents == NULL)
      return;

   try
   {
      smpMDSUtilNotifyEvents->DataSourceOrderUpdated();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (DataSourceOrderUpdated callback): %s",
         (char *)e.Description());
      ERR_report(msg);
   }

   SetCurrentMapInvalid();
   MapView *pView = fvw_get_view();
   if (pView != NULL)
      pView->invalidate_view(FALSE);
}

void MapEngineCOM::CoverageUpdated()
{
   if (m_CoverageInUse)
   {
      m_MDSUpdateCached = true;
      return;
   }

   IMDSUtilNotifyEventsPtr smpMDSUtilNotifyEvents = m_map_rendering_engine;
   if (smpMDSUtilNotifyEvents == NULL)
      return;

   try
   {
      smpMDSUtilNotifyEvents->CoverageUpdated();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (CoverageUpdated callback): %s",
         (char *)e.Description());
      ERR_report(msg);
   }
}

void MapEngineCOM::DataSourceStatusChange(DWORD DataSourceId,
   VARIANT_BOOL IsOnline)
{
   if (m_CoverageInUse)
   {
      m_MDSUpdateCached = true;
      return;
   }

   IMDSUtilNotifyEventsPtr smpMDSUtilNotifyEvents = m_map_rendering_engine;
   if (smpMDSUtilNotifyEvents == NULL)
      return;

   try
   {
      smpMDSUtilNotifyEvents->DataSourceStatusChange(DataSourceId, IsOnline);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (DataSourceStatusChange callback): %s",
         (char *)e.Description());
      ERR_report(msg);
   }
}

void MapEngineCOM::MDMWaitStatusChange(
   long /* MDSUtilStatusUpdateTypesEnum */ StatusType, VARIANT_BOOL Started)
{
   if (m_CoverageInUse)
   {
      m_MDSUpdateCached = true;
      return;
   }

   IMDSUtilNotifyEventsPtr smpMDSUtilNotifyEvents = m_map_rendering_engine;
   if (smpMDSUtilNotifyEvents == NULL)
      return;

   try
   {
      smpMDSUtilNotifyEvents->MDMWaitStatusChange(StatusType, Started);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (MDMWaitStatusChange callback): %s",
         (char *)e.Description());
      ERR_report(msg);
   }
}

void MapEngineCOM::MDMBeforeDeleteTiles(VARIANT Identities, BSTR MapHandlerName)
{
   if (m_CoverageInUse)
   {
      m_MDSUpdateCached = true;
      return;
   }

   IMDSUtilNotifyEventsPtr smpMDSUtilNotifyEvents = m_map_rendering_engine;
   if (smpMDSUtilNotifyEvents == NULL)
      return;

   try
   {
      smpMDSUtilNotifyEvents->MDMBeforeDeleteTiles(Identities, MapHandlerName);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error (MDMBeforeDeleteTiles callback): %s",
         (char *)e.Description());
      ERR_report(msg);
   }
}

int MapEngineCOM::HandleMapStatusCode(MapStatusCodeEnum status)
{
   switch (status)
   {
   case MAP_STATUS_OK:
      return SUCCESS;

   case MAP_STATUS_NO_DATA:
      return FV_NO_DATA;

   case MAP_STATUS_INVALID_MAP_TYPE:
      return FAILURE;

   case MAP_STATUS_FAIL:
      return FAILURE;

      // user tried to scale in but there is no more available data
      // at the requested location
   case MAP_STATUS_MAX_SCALE:
      return FV_NO_DATA;

      // no warning for scaling out when there is no more available data
   case MAP_STATUS_MIN_SCALE:
      return SUCCESS;

   case MAP_STATUS_SURFACE_SIZE_TOO_LARGE:
      return SURFACE_SIZE_TOO_LARGE;
   };

   return SUCCESS;
}

int MapEngineCOM::HandleOfflineDataSources(CComBSTR &bstrOutOfSyncDSs,
   CComBSTR &bstrOfflineDSs)
{
   CIdentitiesSet outOfSyncDataSourceIds(bstrOutOfSyncDSs);
   CIdentitiesSet offlineDataSourceIds(bstrOfflineDSs);

   // notify the map data manager that there are datasources out of sync
   if (outOfSyncDataSourceIds.Size() > 0)
   {
      // If BCP can be reloaded, do that instead of complete regenerate.
      bool bcp_was_updated = false;
      if (MDSWrapper::GetInstance()->LoadLatestBCP(&bcp_was_updated) != SUCCESS)
      {
         ERR_report("MapEngineCOM::HandleOfflineDataSources(): "
            "MDSWrapper::LoadLatestBCP() failed.");
      }

      if (!bcp_was_updated)
      {
         CString msg;
         msg += "Coverage on the following paths has changed:\n\n";

         // count of the number of out-of-sync local data sources
         int nLocalOutOfSync = 0;

         CIdentitiesSet_ITR itr;
         for (itr = outOfSyncDataSourceIds.Begin();
            itr != outOfSyncDataSourceIds.End(); itr++)
         {
            long lDataSourceId = *itr;
            CString path;
            if (MDSWrapper::GetInstance()->
               GetDataSourcePath(&path, lDataSourceId) == SUCCESS)
            {
               nLocalOutOfSync++;
               msg += "\t";
               msg += path;
               msg += "\n";
            }
         }
         msg += "\nWould you like to regenerate coverage on these paths?";

         if (nLocalOutOfSync > 0 && AfxMessageBox(msg, MB_YESNO) == IDYES)
         {
            for (itr = outOfSyncDataSourceIds.Begin();
               itr != outOfSyncDataSourceIds.End(); itr++)
               MDSWrapper::GetInstance()->GenerateCoverage(*itr, false);

            MDSWrapper::GetInstance()->KickoffGenerationSeries(false);
         }
      }

      // If there are any remote data sources, notify the user
      CString msg;
      msg.Format(
         "There is out of sync coverage data on the following remote paths:\n");

      // count of the number of out-of-sync remote data sources
      int nRemoteOutOfSync = 0;

      const long lNumServers = outOfSyncDataSourceIds.NumberOfServers();
      for (long i = 0; i < lNumServers; i++)
      {
         long lServerId = outOfSyncDataSourceIds.GetNthServer(i);

         if (lServerId != 0)
         {
            nRemoteOutOfSync++;
            CString path;
            MDSWrapper::GetInstance()->GetDataSourcePath(&path, lServerId);
            msg += "\t";
            msg += path;
            msg += "\n";
         }
      }
      msg += "\nYou will need to regenerate coverage on these machines.";

      if (nRemoteOutOfSync > 0)
         AfxMessageBox(msg);
   }

   // notify the map data manager that there are datasources offline
   CIdentitiesSet_ITR itr;
   for (itr = offlineDataSourceIds.Begin();
      itr != offlineDataSourceIds.End(); ++itr)
   {
      long lDataSourceId = *itr;
      MDSWrapper::GetInstance()->CheckDataSourceStatus(lDataSourceId);
   }

   // offline remote servers
   const long lNumServers = offlineDataSourceIds.NumberOfServers();
   for (long i = 0 ; i < lNumServers ; ++i)
   {
      const long lServerId = offlineDataSourceIds.GetNthServer(i);
      if (lServerId != 0)
         MDSWrapper::GetInstance()->CheckDataSourceStatus(lServerId);
   }

   return SUCCESS;
}

int MapEngineCOM::ReinitializeFromMapParams(long group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params)
{
   SetSurfaceDimensions(map_display_params.surface_width,
      map_display_params.surface_height);

   int ret = change_map_type(group_identity, map_type.get_source(),
      map_type.get_scale(), map_type.get_series(),
      map_proj_params.center.lat, map_proj_params.center.lon,
      map_proj_params.rotation, map_proj_params.zoom_percent,
      map_proj_params.type, TRUE, TRUE);

   if (ret == SUCCESS)
      ApplyMap();

   return ret;
}

int MapEngineCOM::ChangeMapType(long group_identity, const MapType& map_type,
   int override_no_mapdata)
{
   return change_map_type(group_identity, map_type.get_source(),
      map_type.get_scale(), map_type.get_series(), TRUE,
      override_no_mapdata);
}

int MapEngineCOM::ChangeMapType(long group_identity, const MapType& map_type,
      const d_geo_t& center, int override_no_mapdata /*= FALSE*/)
{
   return change_map_type(group_identity, map_type.get_source(),
      map_type.get_scale(), map_type.get_series(), center.lat,
      center.lon, TRUE, override_no_mapdata);
}

int MapEngineCOM::ChangeMapType(long group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      int override_no_mapdata /* = FALSE */)
{
   int ret = change_map_type(group_identity, map_type.get_source(),
         map_type.get_scale(), map_type.get_series(),
         map_proj_params.center.lat, map_proj_params.center.lon,
         map_proj_params.rotation, map_proj_params.zoom_percent,
         map_proj_params.type, override_no_mapdata, TRUE);

   if (ret == SUCCESS)
      ApplyMap();

   return ret;
}

int MapEngineCOM::ChangeCenter(const d_geo_t& center)
{
   return change_center(center.lat, center.lon);
}

int MapEngineCOM::ChangeProjectionType(ProjectionEnum projection_type)
{
   return change_projection(projection_type);
}

int MapEngineCOM::ChangeRotation(double rotation)
{
   return change_rotation(rotation);
}

int MapEngineCOM::ChangeCenterAndRotation(const d_geo_t& center,
   double rotation)
{
   return change_rotation(rotation, center.lat, center.lon);
}

void MapEngineCOM::ChangeBrightness(double brightness)
{
   set_brightness(brightness);
}

void MapEngineCOM::ChangeContrast(double brightness, double contrast, int contrast_midval)
{
   set_brightness(brightness);
   set_contrast(contrast, contrast_midval);
}

int MapEngineCOM::ChangeZoomPercent(int zoom_percent)
{
   return change_zoom_percent(zoom_percent);
}

int MapEngineCOM::ChangeZoomPercent(int zoom_percent, const d_geo_t& center)
{
   return change_zoom_percent(zoom_percent, center.lat, center.lon);
}

void MapEngineCOM::Invalidate()
{
    set_redraw_overlays_from_scratch_when_animating(TRUE);
}

int MapEngineCOM::ScaleIn(MapType* new_map_type)
{
   int status = scale_in();
   if (status == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return status;
}

int MapEngineCOM::ScaleIn(const d_geo_t& center, MapType* new_map_type)
{
   int status = scale_in(center.lat, center.lon);
   if (status == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return status;
}

int MapEngineCOM::ScaleOut(MapType* new_map_type)
{
   int status = scale_out();
   if (status == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return status;
}

int MapEngineCOM::ScaleOut(const d_geo_t& center, MapType* new_map_type)
{
   int status = scale_out(center.lat, center.lon);
   if (status == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return status;
}

int MapEngineCOM::ChangeToClosestScale(long group_identity,
   MapType* new_map_type)
{
   // the product name (source) matches the category name for "soft scale"
   int ret = change_to_closest_scale(GetCategoryFromGroupId(group_identity));
   if (ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return ret;
}

int MapEngineCOM::ChangeToClosestScale(const MapCategory& category,
   MapType* new_map_type)
{
   int ret = change_to_closest_scale(category);
   if(ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }
   return ret;
}

int MapEngineCOM::ChangeToClosestScale(const MapCategory& category,
   const d_geo_t& center, MapType* new_map_type)
{
   int ret = change_to_closest_scale(category);
   if(ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }
   return ret;
}

int MapEngineCOM::ChangeToClosestScale(const MapCategory& category,
   const MapScale& scale, const MapProjectionParams& map_proj_params,
   MapType* new_map_type)
{
   int ret = change_to_closest_scale(category, scale,
      map_proj_params.center.lat, map_proj_params.center.lon,
      map_proj_params.rotation, map_proj_params.zoom_percent,
      map_proj_params.type);

   if (ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }
   return ret;
}

int MapEngineCOM::ChangeToClosestMapType(long group_identity,
      const MapType& current_map_type,
      const MapProjectionParams& map_proj_params, MapType* new_map_type)
{
   int ret = change_to_closest_scale(GetCategoryFromGroupId(group_identity),
      current_map_type.get_source(),
      current_map_type.get_scale(), current_map_type.get_series(),
      map_proj_params.center.lat, map_proj_params.center.lon,
      map_proj_params.rotation, map_proj_params.zoom_percent,
      map_proj_params.type);

   if (ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return ret;
}

int MapEngineCOM::ChangeToSmallestScale(const MapCategory& category,
   const MapProjectionParams& map_proj_params, MapType* new_map_type)
{
   int ret = change_to_smallest_scale(category,
      map_proj_params.center.lat, map_proj_params.center.lon,
      map_proj_params.rotation, map_proj_params.zoom_percent,
      map_proj_params.type);

   if (ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return ret;
}

int MapEngineCOM::ChangeToLargestScale(const MapCategory& category,
   const MapProjectionParams& map_proj_params, MapType* new_map_type)
{
   int ret = change_to_largest_scale(category,
      map_proj_params.center.lat, map_proj_params.center.lon,
      map_proj_params.rotation, map_proj_params.zoom_percent,
      map_proj_params.type);

   if (ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return ret;
}

int MapEngineCOM::ChangeToBest(const d_geo_t& center,
   MapType* new_map_type)
{
   int ret = change_to_best(center.lat, center.lon);

   if (ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return ret;
}

int MapEngineCOM::ChangeToBest(const MapSource& source, const MapScale& scale,
   const MapProjectionParams& map_proj_params,
   MapType* new_map_type)
{
   int ret = change_to_best(
      map_proj_params.center.lat, map_proj_params.center.lon,
      source, scale,
      map_proj_params.rotation, map_proj_params.zoom_percent,
      map_proj_params.type);

   if (ret == SUCCESS)
   {
      *new_map_type = MapType(
         get_curr_map()->source(),
         get_curr_map()->scale(),
         get_curr_map()->series());
   }

   return ret;
}

void MapEngineCOM::SetSurfaceDimensions(int width, int height)
{
   set_view_dimensions(width, height);
}

int MapEngineCOM::PanPercent(double percent_width, double percent_height,
   double percent_of_panning, d_geo_t* new_center)
{
   int ret = pan_percent(percent_width, percent_height, percent_of_panning);
   if (ret == SUCCESS)
   {
      ApplyMap();
      get_curr_map()->get_actual_center(new_center);
   }
   return ret;
}

int MapEngineCOM::PanMap(int pan_dir, d_geo_t* new_center)
{
   static const double PERCENTAGE_OF_PANNING = 0.5;

   double dx = 0;
   double dy = 0;

   if ((pan_dir & MapView::PAN_DIRECTION_LEFT) == MapView::PAN_DIRECTION_LEFT)
      dx -= PERCENTAGE_OF_PANNING;
   if ((pan_dir & MapView::PAN_DIRECTION_RIGHT) == MapView::PAN_DIRECTION_RIGHT)
      dx += PERCENTAGE_OF_PANNING;
   if ((pan_dir & MapView::PAN_DIRECTION_UP) == MapView::PAN_DIRECTION_UP)
      dy += PERCENTAGE_OF_PANNING;
   if ((pan_dir & MapView::PAN_DIRECTION_DOWN) == MapView::PAN_DIRECTION_DOWN)
      dy -= PERCENTAGE_OF_PANNING;

   return PanPercent(dx, dy, PERCENTAGE_OF_PANNING, new_center);
}