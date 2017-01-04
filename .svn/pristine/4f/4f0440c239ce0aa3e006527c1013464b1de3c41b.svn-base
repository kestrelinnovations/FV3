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

// LoadDrapedOverlayImageOperation.h
//

#include "FalconView/include/MAPS_D.H"
#include "osg/OperationThread"
#include "osgEarth/TileKey"

namespace osgEarth
{
   class MapNode;
   class TileKey;
}
class DrapedOverlayImage;

// LoadDrapedOverlayImageOperation can be used to load an image in a background thread
class LoadDrapedOverlayImageOperation : public osg::Operation
{
public:
   LoadDrapedOverlayImageOperation(DrapedOverlayImage* draped_overlay_image,
      osgEarth::MapNode* map_node, const osgEarth::TileKey& key,
      int tile_width_and_height, const MapType& map_type);
   ~LoadDrapedOverlayImageOperation();

   void operator()(osg::Object* );

   void ResetDrapedOverlayImage()
   {
      m_draped_overlay_image = nullptr;
   }

   bool m_done;
   osg::ref_ptr<osg::Image> m_image;

private:
   void LoadImage();

   osg::ref_ptr<DrapedOverlayImage> m_draped_overlay_image;
   osgEarth::MapNode* m_map_node;
   osgEarth::TileKey m_tile_key;
   int m_tile_width_and_height;
   MapType m_map_type;
   unsigned int m_min_level_of_detail;
};
