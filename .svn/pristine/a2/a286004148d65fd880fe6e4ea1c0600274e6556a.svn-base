// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#ifndef FALCONVIEW_MAPENGINE_H_
#define FALCONVIEW_MAPENGINE_H_

#include <vector>

#include "utils.h"  // for UTL_TimingStatistics, UTL_timing_on
#include "maps_d.h"
#include "..\Common\ComErrorHandler.h"

#include "FalconView/CameraModes.h"
#include "FalconView/MapRenderingEngine_Interface.h"

#pragma once

#import "MapRenderingEngine.tlb" no_namespace named_guids

// forward declarations
class SettableMapProj;

class MapEngineCOM;

class MapEngineCallbackInterface :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CComErrorHandler,
   public IDispatchImpl<IMapRenderingEngineCallback3,
      &IID_IMapRenderingEngineCallback3, &LIBID_MAPENGINELib>
{
public:
BEGIN_COM_MAP(MapEngineCallbackInterface)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(IMapRenderingEngineCallback)
   COM_INTERFACE_ENTRY(IMapRenderingEngineCallback2)
   COM_INTERFACE_ENTRY(IMapRenderingEngineCallback3)
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

   STDMETHOD(raw_InvalidateMap)();

   STDMETHOD(raw_DrawTopMostOverlays)(IActiveMapProj *map);

   MapEngineCOM *m_map_engine;
};

struct MAP_OPTIONS_ENTRY;


// This is the COM version of the rendering engine
//
class MapEngineCOM : public MapRenderingEngine_Interface
{
public:
   // MapRenderingEngine_Interface overrides
   virtual void EnableDraw(bool enable) override
   {
      // Draw is frame by frame; there is nothing to disable
   }

   virtual void SetSurfaceDimensions(int width, int height) override;

   virtual int ChangeCenter(const d_geo_t& center) override;
   virtual int ChangeCenterAndRotation(const d_geo_t& center,
      double rotation) override;

   virtual int ReinitializeFromMapParams(long group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

   virtual int ChangeMapType(long group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      int override_no_mapdata = FALSE) override;

   virtual int ChangeMapType(long group_identity,
      const MapType& map_type, int override_no_mapdata = FALSE) override;
   virtual int ChangeMapType(long group_identity, const MapType& map_type,
      const d_geo_t& center, int override_no_mapdata = FALSE) override;

   virtual int ChangeToClosestScale(long group_identity,
      MapType* new_map_type) override;
   virtual int ChangeToClosestScale(const MapCategory& category,
      MapType* new_map_type) override;
   virtual int ChangeToClosestScale(const MapCategory& category,
      const d_geo_t& center, MapType* new_map_type) override;
   virtual int ChangeToClosestScale(const MapCategory& category,
      const MapScale& scale, const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;
   virtual int ChangeToClosestMapType(long group_identity,
      const MapType& current_map_type,
      const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;

   virtual int ChangeProjectionType(ProjectionEnum projection_type) override;

   virtual int ChangeRotation(double rotation) override;
   virtual void ChangeBrightness(double brightness) override;
   virtual void ChangeContrast(double brightness, double contrast, int contrast_midval) override;

   virtual int ChangeZoomPercent(int zoom_percent) override;
   virtual int ChangeZoomPercent(int zoom_percent,
      const d_geo_t& center) override;

   virtual int ChangeTilt(double tilt_degrees) override
   {
      return SUCCESS;
   }

   virtual int ChangeToSmallestScale(const MapCategory& category,
      const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;
   virtual int ChangeToLargestScale(const MapCategory& category,
      const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;

   virtual int ChangeToBest(const d_geo_t& center,
      MapType* new_map_type) override;
   virtual int ChangeToBest(const MapSource& source, const MapScale& scale,
      const MapProjectionParams& map_proj_params,
      MapType* new_map_type) override;

   virtual void  Invalidate() override;
   virtual void SetCurrentMapInvalid() override;

   virtual int ScaleIn(MapType* new_map_type) override;
   virtual int ScaleIn(const d_geo_t& center, MapType* new_map_type) override;
   virtual int ScaleOut(MapType* new_map_type) override;
   virtual int ScaleOut(const d_geo_t& center, MapType* new_map_type) override;

   virtual int PanMap(int pan_dir, d_geo_t* new_center) override;

   virtual fvw::CameraMode GetCameraMode() override
   {
      return fvw::CAMERA_MODE_NONE;
   }
   virtual void SetCameraMode(fvw::CameraMode camera_mode) override
   {
      // noop
   }
   virtual void ToggleEnableFlightModeAutoMapType() override
   {
   }
   virtual bool GetEnableFlightModeAutoMapType() override
   {
      return false;
   }

protected:
   void CheckCachedMDSOperations();
   bool m_CoverageInUse;
   bool m_MDSUpdateCached;

   SettableMapProj* m_curr_map;   // the current map in the view
   boolean_t m_current_map_invalid;

   int m_view_width, m_view_height;

   //
   // m_redraw_overlays_from_scratch_when_animating is a flag used to indicate
   // when the entire map (including overlays) should not be refreshed
   //
   boolean_t m_redraw_overlays_from_scratch_when_animating;

   //
   // override_no_data allows a blank map to be displayed when there is no
   // data for that map.  This is used when panning in an area where you run
   // out of coverage in order to continue allowing panning.
   //
   boolean_t m_override_no_data;

   BOOL m_auto_enhance_CIB;

   IMapRenderingEnginePtr m_map_rendering_engine;
   IMapRenderingEngine2Ptr m_map_rendering_engine2;
   IMapRenderingEngineCallback* m_map_engine_callback;

public:
   MapEngineCOM();
   virtual ~MapEngineCOM();

   int init(BOOL bCopyMapOptionsFromView = TRUE, BOOL bInitCallback = TRUE,
      BOOL bInitMapOptions = FALSE, BOOL bInternalDraw = FALSE,
      HWND output_wnd = NULL, BOOL allow_invalid_points_on_surface = FALSE);
   int init(IMapRenderingEngineCallback* callback,
      BOOL bCopyMapOptionsFromView = TRUE, BOOL bInitMapOptions = FALSE,
      BOOL bInternalDraw =FALSE, HWND output_wnd = NULL,
      BOOL allow_invalid_points_on_surface = FALSE);
   IMapRenderingEngineCallback* GetCallback()
   {
      return m_map_engine_callback;
   }

   int init_map_options();

   int uninit();

   // call set_view_dimensions whenever the size of the window that the map is
   // being drawn to changes typically in your framework's OnSize
   //
   void set_view_dimensions(double width, double height,
      SurfaceUnitsEnum eUnits = SURFACE_UNITS_PIXELS);
   void SetViewport(double center_lat, double center_lon,
      double width, double height,
      SurfaceUnitsEnum eSurfaceUnits = SURFACE_UNITS_PIXELS);

   virtual void set_redraw_overlays_from_scratch_when_animating(
      boolean_t redraw)
   {
      m_redraw_overlays_from_scratch_when_animating = redraw;
   }
   boolean_t get_redraw_overlays_from_scratch_when_animating()
   {
      return m_redraw_overlays_from_scratch_when_animating;
   }

   virtual SettableMapProj* get_curr_map() const
   {
      ASSERT(m_curr_map != NULL);

      return m_curr_map;
   }

   int GetCurrentGroupIdentity();

   //
   // geo-based map change functions
   //
   // These need to be public for the UTL functions
   // (e.g. UTL_change_view_map_center).
   //
   // The clear if not success means to reset the requested map object if
   // the data check returns anything other than SUCCESS.
   //
   // These functions return SUCCESS, FAILURE, FV_NO_DATA or
   // COV_FILE_OUT_OF_SYNC.
   //

   int change_map_type(const MapSource &source, const MapScale &scale,
      const MapSeries &series, degrees_t latitude, degrees_t longitude,
      double rotation, int zoom_percent, ProjectionEnum projection_type,
      boolean_t overide_no_data = FALSE,
      boolean_t clear_if_not_success = TRUE);

   int change_map_type(int nGroupIdentity, const MapSource &source,
      const MapScale &scale, const MapSeries &series,
      degrees_t latitude, degrees_t longitude,
      double rotation, int zoom_percent, ProjectionEnum projection_type,
      boolean_t overide_no_data = FALSE,
      boolean_t clear_if_not_success = TRUE);

   int change_map_type(const MapSource &source, const MapScale &scale,
      const MapSeries &series, degrees_t latitude, degrees_t longitude,
      boolean_t clear_if_not_success = TRUE);

   int change_map_type(const MapSource &source, const MapScale &scale,
      const MapSeries &series, boolean_t clear_if_not_success = TRUE,
      boolean_t override_no_data = FALSE);

   int change_map_type(int nGroupIdentity, const MapSource &source,
      const MapScale &scale, const MapSeries &series,
      boolean_t clear_if_not_success = TRUE,
      boolean_t override_no_data = FALSE);

   int change_map_type(int nGroupIdentity, const MapSource &source,
      const MapScale &scale, const MapSeries &series,
      degrees_t latitude, degrees_t longitude,
      boolean_t overide_no_data = FALSE, boolean_t clear_if_not_success = TRUE);

   int change_zoom_percent(int zoom_percent,
      boolean_t clear_if_not_success = TRUE);
   int change_zoom_percent(int zoom_percent, degrees_t lat, degrees_t lon,
      boolean_t clear_if_not_success = TRUE);
   int change_rotation(double angle, boolean_t clear_if_not_success = TRUE);
   int change_rotation(double angle, degrees_t lat, degrees_t lon,
      boolean_t clear_if_not_success = TRUE);
   int change_center(degrees_t lat, degrees_t lon,
      boolean_t clear_if_not_success = TRUE);
   int change_projection(ProjectionEnum projection,
      boolean_t clear_if_not_success = TRUE);

   //
   // These functions return SUCCESS, FAILURE, FV_NO_DATA or
   // COV_FILE_OUT_OF_SYNC.
   //
   int scale_in(degrees_t lat, degrees_t lon,
      boolean_t display_wait_cursor = TRUE,
      boolean_t clear_if_not_success = TRUE);
   int scale_in(boolean_t display_wait_cursor = TRUE,
      boolean_t clear_if_not_success = TRUE);

   int scale_out(degrees_t lat, degrees_t lon,
      boolean_t display_wait_cursor = TRUE,
      boolean_t clear_if_not_success = TRUE,
      boolean_t *min_scale = NULL);
   int scale_out(boolean_t display_wait_cursor = TRUE,
      boolean_t clear_if_not_success = TRUE, boolean_t *min_scale = NULL);

   int change_to_closest_scale(MapCategory cat, const MapSource &source,
      MapScale scale, MapSeries series, double lat, double lon,
      double rot_angle, int zoom_percent, ProjectionEnum proj_type);
   int change_to_closest_scale(const MapSource &source,
      MapScale curr_scale, degrees_t center_lat, degrees_t center_lon,
      double rot_angle, int zoom_percent, ProjectionEnum proj_type);
   int change_to_closest_scale(MapCategory cat,
      MapScale curr_scale, degrees_t center_lat, degrees_t center_lon,
      double rot_angle, int zoom_percent, ProjectionEnum proj_type);
   int change_to_closest_scale(MapCategory cat, degrees_t center_lat,
      degrees_t center_lon);
   int change_to_closest_scale(const MapSource &source, const MapSeries &series,
      MapScale curr_scale, degrees_t center_lat, degrees_t center_lon,
      double rot_angle, int zoom_percent, ProjectionEnum proj_type);
   int change_to_closest_scale(MapCategory cat);

   int change_to_closest_scale(const MapSource &source, MapScale curr_scale);

   int change_to_smallest_scale(MapCategory cat, degrees_t center_lat,
      degrees_t center_lon, double rot_angle, int zoom_percent,
      ProjectionEnum proj_type);

   int change_to_largest_scale(MapCategory cat, degrees_t center_lat,
      degrees_t center_lon, double rot_angle, int zoom_percent,
      ProjectionEnum proj_type);

   int change_to_best(degrees_t lat, degrees_t lon,
      boolean_t clear_if_not_success = TRUE);
   int change_to_best(degrees_t lat, degrees_t lon, const MapSource &source,
      const MapScale &scale, double rot, int zoom_percent,
      ProjectionEnum proj_type, boolean_t clear_if_not_success = TRUE);

   int set_map_type_from_geo_bounds(const MapSource &source,
      const MapScale &scale, const MapSeries &series,
      double southern_lat, double western_lon,
      double northern_lat, double eastern_lon,
      double rotation, int zoom_percent,
      ProjectionEnum projection, boolean_t overide_no_data,
      double *surface_width, double *surface_height,
      boolean_t clear_if_not_success = TRUE);

   int set_fixed_size_map_type_from_geo_bounds(const MapSource &source,
      const MapScale &scale, const MapSeries &series,
      double southern_lat, double western_lon,
      double northern_lat, double eastern_lon, int surface_width,
      int surface_height);

   //
   // view-relative map change functions
   //
   // These functions return SUCCESS, FAILURE, FV_NO_DATA and
   // COV_FILE_OUT_OF_SYNC.
   //
   int change_center(int x, int y, boolean_t clear_if_not_success = TRUE);
   int pan_percent(double percent_width, double percent_height,
      double percent_of_panning, boolean_t clear_if_not_success = TRUE);

   void set_brightness(double bright);
   double get_brightness();
   void set_contrast(double contrast, int contrast_midval);
   void calculate_brightness_contrast(double* brightness, double* contrast, int* midval);

   // returns SUCCESS, FAILURE, E_PREPARE_FOR_OUTPUT, MEMORY_ERROR,
   // FATAL_MAP_DISPLAY_ERROR
   int draw_map(CDC* pDC, boolean_t display_wait_cursor,
      boolean_t *map_drawn_from_scratch);

   int print_map(CDC* dc, boolean_t is_printing_to_scale);

   int copy_map_to_clipboard();
   int save_PNG(const CString &filename);
   int save_BMP(const CString &filename);
   int save_JPG(const CString &filename, int quality);
   int save_GeoTIFF(const CString &filename);
   int save_KMZ(const CString &filename);

   int GetMapHandler(const MapSource &source, IDispatch** handler);
   int GetMapHandlerByGuid(const GUID& map_handler_guid, IDispatch** handler);
   IMapRenderingEnginePtr &GetRenderingEngine()
   {
      return m_map_rendering_engine;
   }

   const std::vector<MAP_OPTIONS_ENTRY>& GetOptionsList() const;

   int SetIsPrinting(BOOL isPrinting, BOOL printToScale);
   int DataCheckEnabled(BOOL enabled);
   int SetPixelsPerInchForToScaleZoom(double dPixelsPerInch);
   int SetToScaleFactor(double dToScaleFactor);
   int GetToScaleFactor(double* pToScaleFactor);

   int ApplyMap();

   int DrawMapToDIB(VARIANT *dib);
   int DrawMapToResizedDIB(int nPixmapWidth, int nPixmapHeight, VARIANT * dib);
   int DrawMapToRGBA(unsigned char* image);

   void DataSourceOrderUpdated();
   void CoverageUpdated();
   void DataSourceStatusChange(DWORD DataSourceId, VARIANT_BOOL IsOnline);
   void MDMWaitStatusChange(long /* MDSUtilStatusUpdateTypesEnum */ StatusType,
      VARIANT_BOOL Started);
   void MDMBeforeDeleteTiles(VARIANT Identities, BSTR MapHandlerName);

   // Used to flag internal operations so FalconView can determine if the draw
   // is an internal one.  This is a KLUDGE KLUDGE KLUDGE to keep the Vertical
   // View window from being drawn after an automation interface call (upsets
   // the overlay internal states)
   BOOL m_bInternalDraw;


public:
   static void ConvertScale(MapScale scale, double *pScale,
      MapScaleUnitsEnum *pScaleUnits);
   static MapScaleUnitsEnum convert_units(MapScale::unit_t);

protected:
   // called before a map is changed
   void OnPreChangeMap();
   int HandleMapStatusCode(MapStatusCodeEnum status);
   int HandleOfflineDataSources(CComBSTR &bstrOutOfSyncDSs,
      CComBSTR &bstrOfflineDSs);
   void SetMapSpec();
   int PanPercent(double percent_width, double percent_height,
      double percent_of_panning, d_geo_t* new_center);

   std::vector<MAP_OPTIONS_ENTRY> m_map_options_list;
};

#endif  // FALCONVIEW_MAPENGINE_H_
