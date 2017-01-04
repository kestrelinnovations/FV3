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

// DrapedOverlayImage.cpp
//

#include "stdafx.h"

#include "DrapedOverlayImageNodeVisitor.h"

#include "osg/Group"
#include "osgEarthDrivers/engine_mp/TileGroup"

#include "FalconView/DrapedOverlayImage.h"

void DrapedOverlayImageNodeVisitor::apply(osg::Group& group)
{
   using namespace osgEarth_engine_mp;
   TileGroup* tile_group = dynamic_cast<TileGroup *>(&group);
   if (tile_group)
   {
      auto tile_node = tile_group->getTileNode();
      if (tile_node != nullptr)
      {
         auto it = tile_node->getTileModel()->_colorData.begin();
         for (; it != tile_node->getTileModel()->_colorData.end();
            ++it)
         {
            osg::Image* image = it->second.getTexture()->getImage();
            if (image != nullptr)
            {
               DrapedOverlayImage* draped_overlay_image =
                  dynamic_cast<DrapedOverlayImage*>(image);
               if (draped_overlay_image != nullptr)
               {
                  draped_overlay_image->Invalidate();
               }
            }
         }
      }
   }

   traverse(group);
}
