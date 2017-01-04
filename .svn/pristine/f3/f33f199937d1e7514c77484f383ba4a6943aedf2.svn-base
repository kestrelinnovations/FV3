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

// MapRenderingOnTiles.cpp
//

// stdafx first
#include "stdafx.h"

// this file's header
#include "MapRenderingOnTiles.h"

// system includes
// third party files
#include "osg/Image"
#include "osgDB/DatabasePager"
#include "osgEarth/Map"
#include "osgEarth/TileKey"

// other FalconView headers
// this project's headers
#include "MapEngineCOM.h"
#include "MAPS_D.H"
#include "overlay.h"  // for OverlayRender_Interface

#include "FalconView/getobjpr.h"
#include "FalconView/include/err.h"
#include "FalconView/include/OverlayElements.h"
#include "FalconView/include/param.h"
#include "FalconView/MapOptionsEntry.h"
#include "FalconView/MapView.h"
#include "FalconView/UIThreadOperation.h"

const char* NO_MAP_DATA_FILENAME = "\\icons\\system\\NoMapData.png";

namespace fvw
{
unsigned int GetMinLevelOfDetail(double natural_level_of_detail)
{
   // map very low natural levels to the lowest
   if (natural_level_of_detail < 3)
      return 0;

   return static_cast<unsigned int>(floor(natural_level_of_detail) - 2);
}

unsigned int GetMinLevelOfDetail(const MapType& map_type,
   int num_pixels_per_tile_side)
{
   // Compute the degrees per pixel at 6% zoom (the minimum zoom level
   // we support)
   const double zoomed_degrees_per_pixel = 
      map_type.get_scale().get_nominal_degrees_lat_per_pixel() / 0.06;

   double natural_level_of_detail = fvw::GetNaturalLevelOfDetail(
      zoomed_degrees_per_pixel, num_pixels_per_tile_side);
   return fvw::GetMinLevelOfDetail(natural_level_of_detail);
}

unsigned int GetNaturalLevelOfDetail(const MapType& map_type,
   int num_pixels_per_tile_side)
{
   double natural_level_of_detail = fvw::GetNaturalLevelOfDetail(
      map_type.get_scale().get_nominal_degrees_lat_per_pixel(),
      num_pixels_per_tile_side);

   return static_cast<unsigned int>(ceil(natural_level_of_detail - 1));
}

double GetNaturalLevelOfDetail(double degrees_per_pixel, int pixels_per_tile)
{
   double degrees_per_tile = degrees_per_pixel * pixels_per_tile;

   // the natural level of detail is the level that contains the number of tiles
   // such that the resolution of the tile is closest to the resolution of the
   // map type.  Because a level n contains 2^(n-1) tiles, the natural level is
   // 1 + log base 2 (degrees in natural level)
   return 1 + log( 180 / degrees_per_tile) / log( 2.0 );
}

void PrepareForRendering(MapEngineCOM* map_engine,
   const MapType& map_type, const osgEarth::GeoExtent& geo_extent,
   long surface_width, long surface_height, int* status)
{
   osgEarth::Bounds bounds = geo_extent.bounds();

   const double southern_lat = bounds.yMin();
   const double western_lon = bounds.xMin();
   const double northern_lat = bounds.yMax();
   const double eastern_lon = bounds.xMax();

   *status = map_engine->set_fixed_size_map_type_from_geo_bounds(
      map_type.get_source(), map_type.get_scale(), map_type.get_series(),
      southern_lat, western_lon, northern_lat, eastern_lon,
      surface_width, surface_height);
}

void PrepareCallback(OsgEarthMapEngineCallback* callback,
   const MapType& map_type, const osgEarth::GeoExtent& geo_extent,
   double width, double height)
{
   callback->SetExtent(geo_extent);
   callback->SetMapType(map_type);
   callback->SetSurfaceDimensions(width, height);
}

osg::Image* Render(MapEngineCOM* map_engine, int width, int height)
{
   osg::Image* ret = new osg::Image();
   ret->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
   if (map_engine->DrawMapToRGBA(ret->data()) == SUCCESS)
      return ret;

   ret->unref();
   return nullptr;
}

osg::Image* GetSolidDataImage(int width, int height, BYTE gray_value)
{
   osg::Image* result = new osg::Image();
   result->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
   memset(result->data(), gray_value, width * height * 3);
   return result;
}

osg::Image* GetInvalidTileDataImage(int width, int height)
{
   return fvw::GetSolidDataImage(width, height, 16);
}

osg::Image* GetTransparentTileDataImage(int width, int height)
{
   osg::Image* result = new osg::Image();
   result->allocateImage(width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
   memset(result->data(), 0, width * height * 4);

   return result;
}

osg::Image* GetNoMapDataImage(int width, int height)
{
   CString filename = PRM_get_registry_string("Main", "ReadOnlyAppData", "");
   filename += NO_MAP_DATA_FILENAME;

   return osg_image_utils::ImageFromFilename(_bstr_t(filename));
}

struct MapEngine_CopyOptions
{
   MapEngineCOM* map_engine;
   bool copy_options;
};

// ThreadIdToMapEngine maintains a list of MapEngine objects for each thread.
// This is to avoid having to marshal a STA rendering engine to each caller.
struct ThreadIdToMapEngine
{
   std::map<DWORD, MapEngine_CopyOptions*> thread_id_to_map_engine;
   CRITICAL_SECTION cs;

   ThreadIdToMapEngine()
   {
      ::InitializeCriticalSection(&cs);
   }
   ~ThreadIdToMapEngine()
   {
      ::DeleteCriticalSection(&cs);
   }


   MapEngineCOM* GetMapRenderingEngineForCurrentThread(osgEarth::Map* osg_map,
      bool copy_options, bool data_check_enabled)
   {
      const DWORD thread_id = ::GetCurrentThreadId();

      // If a rendering engine was already created on the current thread,
      // return it.
      ATL::CCritSecLock cs(cs);
      auto it = thread_id_to_map_engine.find(thread_id);
      if (it != thread_id_to_map_engine.end())
      {
         MapEngine_CopyOptions* map_engine_copy_options = it->second;
         cs.Unlock();
         if (copy_options && map_engine_copy_options->copy_options)
         {
            CopyMapOptions(map_engine_copy_options->map_engine);
            map_engine_copy_options->copy_options = false;
         }
         return map_engine_copy_options->map_engine;
      }

      cs.Unlock();

      // Otherwise, create a new instance and add it to the collection
      MapEngineCOM* ret;
      if (osg_map == nullptr)
         ret = CreateMapEngine(nullptr);
      else
         ret = CreateMapEngine(fvw::CreateMapEngineCallback(osg_map));

      if (copy_options)
         CopyMapOptions(ret);

      ret->DataCheckEnabled(data_check_enabled);

      cs.Lock();
      MapEngine_CopyOptions* map_engine_copy_options =
         new MapEngine_CopyOptions();
      map_engine_copy_options->map_engine = ret;

      // if we copied the options, we don't need to again.  If we did not copy
      // the options, future uses of this engine might need to.
      map_engine_copy_options->copy_options = !copy_options;
      thread_id_to_map_engine[thread_id] = map_engine_copy_options;
      cs.Unlock();

      // Hook cleanup to the DatabasePager thread exit
      osgDB::DatabasePager::ThreadCleanup::s_clsThreadCleanup.AddCleanup(
         ThreadIdToMapEngine::Cleanup, (LPVOID) this );

      return ret;
   }

   static void __stdcall Cleanup(void* param)
   {
       ThreadIdToMapEngine* thread_id_to_map_engine =
         reinterpret_cast<ThreadIdToMapEngine*>(param);
       thread_id_to_map_engine->CleanupRenderingEngineForCurrentThread();
   }

   void CleanupRenderingEngineForCurrentThread()
   {
      ATL::CCritSecLock cs(cs);
      auto it = thread_id_to_map_engine.find(::GetCurrentThreadId());
      if (it != thread_id_to_map_engine.end())
      {
         delete it->second->map_engine;
         thread_id_to_map_engine.erase(it);
      }
   }

   // Copy map options from the primary MapRenderingEngine instance (the
   // one owned by MapView::m_map_engine). Because this instance was created
   // on the main thread we need to marshal it to this thread.
   void CopyMapOptions(MapEngineCOM* map_engine)
   {
      MapView* map_view = fvw_get_view();
      if (map_view == nullptr)
         return;

      const static bool block_until_complete = true;

      // First, prepare the rendering engine for marshalling in the main thread
      IStream* marshaled_stream;
      new fvw::UIThreadOperation([&]()
      {
         marshaled_stream = map_view->PrepareMarshaledStream();
      }, block_until_complete);

      // Now that the marshalled stream has been prepared in the main thread, we
      // can unmarshal it into this thread
      //
      IMapRenderingEnginePtr marshalled_rendering_engine;
      if (::CoGetInterfaceAndReleaseStream(
         marshaled_stream, IID_IMapRenderingEngine,
         reinterpret_cast<void **>(&marshalled_rendering_engine)) != S_OK)
      {
         ERR_report("Failed unmarshalling map rendering engine to background "
            "thread. Unable to copy map options.");
         return;
      }

      // Copy the properties to the given map engine
      const auto& options_list =
         map_view->get_map_engine()->GetOptionsList();
      for (auto it = options_list.begin(); it != options_list.end(); ++it)
      {
         try
         {
            // marshall this sucker to the place
            IStream* marshaled_options_stream;
            new fvw::UIThreadOperation([&]()
            {
               if (::CoMarshalInterThreadInterfaceInStream(
                  IID_IMapRenderOptions, it->map_render_options,
                  &marshaled_options_stream) != S_OK)
               {
                  marshaled_options_stream = nullptr;
                  ERR_report("Marshaling options failed");
               }

            }, block_until_complete);

            if (marshaled_options_stream != nullptr)
            {

               IMapRenderOptionsPtr marshaled_options;
               if (::CoGetInterfaceAndReleaseStream(
                  marshaled_options_stream, IID_IMapRenderOptions,
                  reinterpret_cast<void **>(&marshaled_options)) != S_OK)
               {
                  ERR_report("Failed unmarshalling map rendering options to "
                     "background thread. Unable to copy map options.");
               }

               marshaled_options->CopyProperties(
                  map_engine->GetRenderingEngine(),
                  marshalled_rendering_engine);
            }
         }
         catch (_com_error& e)
         {
            // Report but continue on so that one map handler failing does
            // not affect the rest.
            REPORT_COM_ERROR(e);
         }
      }

      // Adjust the loading policy of the WMS map handler to prevent it from
      // loading tiles in a background thread (loads from this rendering engine
      // are already done in a background thread).
      try
      {
         IDispatch* disp;
         map_engine->GetMapHandlerByGuid(WMSMAPSERVERLib::CLSID_WMSMapHandler,
            &disp);
         WMSMAPSERVERLib::IWMSMapRenderOptions2Ptr wms_options = disp;
         wms_options->LoadingPolicy = WMSMAPSERVERLib::LOADING_POLICY_SYNC;

         int auto_contrast = map_view->GetMapDisplayParams().auto_enhance_CIB;

         IRPFMapRenderOptionsPtr smpRPFOptions =
            map_engine->GetRenderingEngine()->GetMapHandler(_bstr_t("CIB"));
         smpRPFOptions->SetAutoEnhanceCIB(
            auto_contrast ? VARIANT_TRUE : VARIANT_FALSE);

         IGeoTiffMapRenderOptionsPtr smpGeoTiffOptions =
            map_engine->GetRenderingEngine()->GetMapHandler(_bstr_t("GeoTIFF"));
         smpGeoTiffOptions->SetAutoContrastEnabled(
            auto_contrast ? VARIANT_TRUE : VARIANT_FALSE);
      }
      catch (_com_error& e)
      {
         REPORT_COM_ERROR(e);
      }
   }

   void SetOptionsChanged(bool value)
   {
      std::for_each(thread_id_to_map_engine.begin(),
         thread_id_to_map_engine.end(),
         [&](const std::pair<DWORD, MapEngine_CopyOptions* >& it)
      {
         it.second->copy_options = value;
      });
   }

} s_thread_id_to_map_engine;

void SetOptionsChanged(bool value)
{
   s_thread_id_to_map_engine.SetOptionsChanged(value);
}

MapEngineCOM* GetMapRenderingEngineForCurrentThread(osgEarth::Map* osg_map,
   bool copy_options /*=true*/, bool data_check_enabled /*=true*/)
{
   return s_thread_id_to_map_engine.GetMapRenderingEngineForCurrentThread(
      osg_map, copy_options, data_check_enabled);
}

CComPtr<CComObject<OsgEarthMapEngineCallback>> CreateMapEngineCallback(
   osgEarth::Map* osg_map)
{
   CComObject<OsgEarthMapEngineCallback>* callback;
   CComObject<OsgEarthMapEngineCallback>::CreateInstance(&callback);
   CComPtr<CComObject<OsgEarthMapEngineCallback>> callback_ptr(callback);

   callback->Initialize(osg_map);

   return callback_ptr;
}

MapEngineCOM* CreateMapEngine(
   CComPtr<CComObject<OsgEarthMapEngineCallback>> callback)
{
   ::CoInitialize(NULL);
   MapEngineCOM* engine = new MapEngineCOM();
   engine->init(callback, FALSE);

   return engine;
}
}  // namespace fvw
