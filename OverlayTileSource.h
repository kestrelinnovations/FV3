// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

// OverlayTileSoure.h
//

#ifndef FALCONVIEW_OVERLAYTILESOURCE_H_
#define FALCONVIEW_OVERLAYTILESOURCE_H_

// third party files
#include "osgEarth/TileSource"

#include "FalconView/include/OverlayStackChangedObserver_Interface.h"
#include "FalconView/OverlayInvalidatedListener_Interface.h"

#include "FalconView/include/MAPS_D.H"
#include "FalconView/MapParamsListener_Interface.h"

// forward definitions
interface IMapRenderingEngineCallback;
interface OverlayRender_Interface;
class GeospatialViewController;
class MapEngineCOM;
class MapType;
class OverlayTileSourceTest;
namespace osg
{
   class Image;
}
namespace osgEarth
{
   class MapNode;
}

// OverlayTileSource - a tile source that renders legacy 2d overlays to tiles
class OverlayTileSource :
   public osgEarth::TileSource,
   public MapParamsListener_Interface,
   public OverlayInvalidatedListener_Interface,
   public OverlayStackChangedObserver_Interface
{
public:
   OverlayTileSource(osgEarth::MapNode* map_node,
      const osgEarth::TileSourceOptions& options,
      GeospatialViewController* view_controller,
      const MapType& map_type, ProjectionEnum proj_type);
   OverlayTileSource(){}
   ~OverlayTileSource();

   void AbortCreatingAllImages();
   void TerminateOperationsThread();

   void initialize(const osgDB::Options* options,
      const osgEarth::Profile* profile);
   osg::Image* createImage(const osgEarth::TileKey& key,
         osgEarth::ProgressCallback* progress);

   osg::HeightField* createHeightField(const osgEarth::TileKey& key,
      osgEarth::TileSource::HeightFieldOperation* prepOp =0L,
      osgEarth::ProgressCallback* progress = 0L)
   {
      // not implemented - legacy overlays have no height information
      return nullptr;
   }

   // MapParamsListener_Interface
   //
   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

   // OverlayInvalidatedListener_Interface overrides
   //
   virtual void Invalidate() override;

   // OverlayStackChangedObserver_Interface overrides
   //
   virtual void OverlayAdded(C_overlay* overlay) override;
   virtual void OverlayRemoved(C_overlay* overlay) override;
   virtual void OverlayOrderChanged() override;
   virtual void OverlayFileSpecificationChanged(C_overlay* overlay) override
   {
   }
   virtual void OverlayDirtyChanged(C_overlay* overlay) override
   {
   }

private:
   osgEarth::MapNode* m_map_node;
   MapType m_map_type;
   ProjectionEnum m_proj_type;
   GeospatialViewController* m_view_controller;

   // Flag indicating whether or not an invalidate is necessary. In 2D, only
   // a single invalidation of the tile is necessary.
   bool m_need_to_invalidate;

   osg::ref_ptr<osg::OperationsThread> m_operations_thread;
};

#endif  // FALCONVIEW_OVERLAYTILESOURCE_H_
