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

// MapRenderingEngineTileSource.cpp
//

// stdafx first
#include "stdafx.h"

// this file's header
#include "MapRenderingEngineTileSource.h"

// third party files
#include "osg/OperationThread"
#include "osg/NodeVisitor"
#include "osgDB/DatabasePager"
#include "osgEarth/MapNode"
#include "osgEarth/Registry"
#include "osgEarthDrivers/engine_mp/ResetTileNodeVisitor"
#include "osgEarthDrivers/engine_mp/TileGroup"

// other FalconView headers
#include "FalconView/include/err.h"
#include "FalconView/GeospatialView.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/GeospatialViewer.h"
#include "FvCore/Include/Registry.h"
#include "FvCore/Include/StringUtils.h"
#include "FvCore/Include/ThreadUtils.h"
#include "FalconView/getobjpr.h"

// this project's headers
#include "GenericNodeCallback.h"
#include "MapEngineCOM.h"
#include "MapRenderingOnTiles.h"
#include "MAPS_D.H"

// other headers
#include "math.h"

namespace
{
   // Constants used by LoadTirosMap
   const long TIROS_HEADER_SIZE = 54;
   const size_t TIROS_IMAGE_SIZE_BYTES = 256 * 256 * 3;
}

MapRenderingEngineTileSource::MapRenderingEngineTileSource(
   osgEarth::MapNode* map_node, const osgEarth::TileSourceOptions& options,
   osgDB::DatabasePager* database_pager) :
osgEarth::TileSource(options),
m_map_node(map_node),
m_database_pager(database_pager),
m_terrain_layer_options(nullptr)
{
   initialize(nullptr, nullptr);
}

void MapRenderingEngineTileSource::initialize(const osgDB::Options* options,
   const osgEarth::Profile* profile)
{
   if (profile)
   {
      setProfile(profile);
   }
   else
   {
      setProfile(osgEarth::Registry::instance()->getGlobalGeodeticProfile());
   }
   /* listen to map params change, zoom affects 
    osgEarth_engine_mp::TileGroup::s_maxLod */
   fvw_get_view()->AddMapParamsListener(this);;
}

osg::Image* MapRenderingEngineTileSource::createImage(
   const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress )
{
   auto map_type = m_map_type;
   // Special case for TIROS tiles that are at or below LOD = 3
   if (map_type.get_source() == TIROS && key.getLevelOfDetail() <= 3)
   {
      return LoadTirosMap(key);
   }

   const int tile_size = getOptions().tileSize().value();

   unsigned int min_level_of_detail = fvw::GetMinLevelOfDetail(map_type,
      tile_size);
   if (key.getLevelOfDetail() < min_level_of_detail)
   {
      return fvw::GetInvalidTileDataImage(tile_size, tile_size);
   }

   MapEngineCOM* map_engine =
      fvw::GetMapRenderingEngineForCurrentThread(nullptr);

   int status;

   // Special case for TIROS World to allow it to render to subtiles. The
   // alternative is eliminating TIROS World from the available list of map
   // types in 3D.
   if (map_type.get_source() == TIROS && map_type.get_scale() == WORLD)
   {
      fvw::PrepareForRendering(map_engine,
         MapType(TIROS, MapScale(16, MAP_SCALE_KILOMETER), "Topobath"),
         key.getExtent(), tile_size, tile_size, &status);
   }
   else
   {
      fvw::PrepareForRendering(map_engine, map_type, key.getExtent(),
         tile_size, tile_size, &status);
   }

   if (status == SUCCESS)
      return fvw::Render(map_engine, tile_size, tile_size);

   else if (status == FV_NO_DATA)
      return fvw::GetNoMapDataImage(tile_size, tile_size);

   return nullptr;
}

static std::string hd_location = reg::get_registry_string("Main",
   "ReadOnlyAppData", "") + "\\TIROS\\";

// Specialization for loading generated TIROS maps
osg::Image* MapRenderingEngineTileSource::LoadTirosMap(
   const osgEarth::TileKey& tile_key)
{
   // Construct the name of the TIROS frame from the given tile key
   char path[MAX_PATH];
   sprintf_s(path, MAX_PATH, "%swt%d_%03d%03d.wld4", hd_location.c_str(),
      tile_key.getLevelOfDetail(), tile_key.getTileX(), tile_key.getTileY());

   FILE* file = fopen(path, "rb");
   if (!file)
   {
      CString msg;
      msg.Format("Unable to open TIROS file: %s", path);
      ERR_report(msg);
      return nullptr;
   }

   // Seek past the TIROS header
   fseek(file, TIROS_HEADER_SIZE, SEEK_CUR);

   // Read in the TIROS data
   osg::Image* ret = new osg::Image;
   ret->allocateImage(256, 256, 1, GL_RGB, GL_UNSIGNED_BYTE);
   fread(reinterpret_cast<void *>(ret->data()),
      TIROS_IMAGE_SIZE_BYTES, 1, file);

   fclose(file);

   return ret;
}

const std::string MapRenderingEngineTileSource::UniqueName(
   const MapType& map_type)
{
   std::string ret;

   ret += map_type.get_source_string();
   ret += "_";
   ret += map_type.get_scale_string();
   ret += "_";
   ret += map_type.get_series_string();

   string_utils::replace_all(ret, ":", "_");

   return ret;
}

void MapRenderingEngineTileSource::SetMapType(const MapType& new_map_type)
{
   // We assume this is always called from an update traversal

   if (m_map_type != new_map_type)
   {
      if (m_terrain_layer_options)
         m_terrain_layer_options->cacheId() = UniqueName(new_map_type);

      osgEarth_engine_mp::ResetTileNodeVisitor
         reset_tile_node_visitor(m_database_pager);
      auto frame_stamp = fvw_get_view()->GetGeospatialViewController()->
         GetGeospatialView()->Viewer()->getFrameStamp();
      reset_tile_node_visitor.clear_pending(*frame_stamp);
      m_map_node->accept(reset_tile_node_visitor);
      m_map_type = new_map_type;
   }
}

void MapRenderingEngineTileSource::MapParamsChanged(int change_mask,
   long map_group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params,
   const MapDisplayParams& map_display_params)

{ 
   if ((MapParamsListener_Interface::MAP_TYPE_CHANGED | 
      MapParamsListener_Interface::MAP_PROJ_ZOOM_PERCENT_CHANGED | 
      MapParamsListener_Interface::MAP_PROJ_TYPE_CHANGED) 
      & change_mask)
   {
      const int tile_size = getOptions().tileSize().value();
      int zoom_upscale = (log(1.0*map_proj_params.zoom_percent/100)/log(2.0));
      osgEarth_engine_mp::TileGroup::s_maxLod =
         fvw::GetNaturalLevelOfDetail(map_type, tile_size) + 
         zoom_upscale;
   }
}

void MapRenderingEngineTileSource::ResetTileNodes()
{
   GenericNodeCallback* node_callback = new GenericNodeCallback;
   node_callback->SetOperation(m_map_node, [this]()
   {
      osgEarth_engine_mp::ResetTileNodeVisitor
         reset_tile_node_visitor(m_database_pager);
      auto frame_stamp = fvw_get_view()->GetGeospatialViewController()->
         GetGeospatialView()->Viewer()->getFrameStamp();
      reset_tile_node_visitor.clear_pending(*frame_stamp);
      m_map_node->accept(reset_tile_node_visitor);
   });
}
