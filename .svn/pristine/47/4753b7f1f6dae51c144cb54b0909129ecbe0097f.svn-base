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

// DrapedOverlayImage.h
//

#ifndef FALCONVIEW_DRAPED_OVERALY_IMAGE_H
#define FALCONVIEW_DRAPED_OVERALY_IMAGE_H

#include "osg/ImageStream"
#include "osgEarth/TileKey"

#include "FalconView/include/MAPS_D.H"
#include "FalconView/MapParamsListener_Interface.h"

namespace osg
{
   class Image;
   class NodeVisitor;
}
namespace osgEarth
{
   class MapNode;
}

class C_overlay;
class GeospatialViewController;
class LoadDrapedOverlayImageOperation;
class MapType;

class DrapedOverlayImage :
   public osg::ImageStream,
   public MapParamsListener_Interface
{
public:
   DrapedOverlayImage(osgEarth::MapNode* map_node,
      const osgEarth::TileKey& key, int tile_width_and_height,
      const MapType map_type, osg::OperationsThread* operations_thread);

   virtual ~DrapedOverlayImage();

   // If the m_load_image_op is complete then update the contents of this image
   // with the new pixels
   void UpdateImage();

   // Copies the contents of the given image into this image
   void copyImage(osg::Image* image);

   // Update method for osg::Image subclasses that update themselves
   // during the update traversal
   virtual void update(osg::NodeVisitor* nv) override;

   // Give derived classes of DYNAMIC data variance a chance to clear
   // their internal data after the image is loaded to texture
   virtual void deallocateDataAfterLoad() override
   {
      deallocateData();
   }

   // MapParamsListener_Interface overrides
   //
   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

   virtual void Invalidate();

   // Initialization of map params listener
   static void Initialize();

   // Flag unset at program shutdown to disallow any new operations from
   // being added to the operations queue
   static bool s_allow_new_operations;

private:
   osgEarth::MapNode* m_map_node;
   osgEarth::TileKey m_tile_key;
   int m_tile_width_and_height;
   osg::ref_ptr<osg::OperationsThread> m_operations_thread;
   MapType m_map_type;
   bool m_need_to_load_image_on_next_update;

   osg::ref_ptr<LoadDrapedOverlayImageOperation> m_load_image_op;
};

#endif  // FALCONVIEW_DRAPED_OVERALY_IMAGE_H
