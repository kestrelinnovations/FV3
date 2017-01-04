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

// MapRenderingEngineTileSource.h
//

#ifndef FALCONVIEW_MAPRENDERINGENGINETILESOURCE_H_
#define FALCONVIEW_MAPRENDERINGENGINETILESOURCE_H_

// third party files
#include "osgEarth/TileSource"

// this project's headers
#include "FalconView/include/MAPS_D.H"
#include "FalconView/MapParamsListener_Interface.h"

// forward declarations
class MapEngineCOM;
class MapRenderingEngineTileSource;
class MapType;
namespace osgEarth
{
   class MapNode;
   class TerrainLayerOptions;
}
namespace osgDB
{
   class DatabasePager;
}

// MapRenderingEngineTileSource - a tile source that uses the map rendering
// engine to generate tiles
class MapRenderingEngineTileSource :
   public osgEarth::TileSource, public MapParamsListener_Interface
{
public:
   MapRenderingEngineTileSource(osgEarth::MapNode* map_node,
    const osgEarth::TileSourceOptions& options = osgEarth::TileSourceOptions(),
    osgDB::DatabasePager* database_pager = nullptr);

   void initialize(const osgDB::Options* options,
      const osgEarth::Profile* profile);

   void SetTerrainLayerOptions(
      osgEarth::TerrainLayerOptions* terrain_layer_options)
   {
      m_terrain_layer_options = terrain_layer_options;
   }

   // Returns a unique name for this tile source. Can be used for caching
   // data, for instance.
   const std::string UniqueName(const MapType& map_type);
   void SetMapType(const MapType& new_map_type);
   void ResetTileNodes();
   void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params);

   // osgEarth::TileSource
   //
   osg::Image* createImage(const osgEarth::TileKey& key,
         osgEarth::ProgressCallback* progress) override;

   osg::HeightField* createHeightField(const osgEarth::TileKey& key,
         osgEarth::TileSource::HeightFieldOperation* prepOp =0L,
         osgEarth::ProgressCallback* progress = 0L) override
   {
      // this tile source only generates images, not height fields
      return nullptr;
   }

   bool isDynamic() const override
   {
      return true;
   }

private:
   // Used to modifiy the cacheId for the parent image layer
   osgEarth::TerrainLayerOptions* m_terrain_layer_options;

   // Specialization for loading generated TIROS maps
   osg::Image* LoadTirosMap(const osgEarth::TileKey& tile_key);

   osgEarth::MapNode* m_map_node;
   
   // The map type associated with this tile source
   MapType m_map_type;

   osgDB::DatabasePager* m_database_pager;

};
#endif  // FALCONVIEW_MAPRENDERINGENGINETILESOURCE_H_
