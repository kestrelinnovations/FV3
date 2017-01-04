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

// LoadDrapedOverlayImageOperation.cpp
//

#include "StdAfx.h"

#include "LoadDrapedOverlayImageOperation.h"

#include "osgEarth/Map"
#include "osgEarth/MapNode"
#include "osgEarth/TileKey"

#include "err.h"
#include "FalconView/DrapedOverlayImage.h"
#include "FalconView/GenericNodeCallback.h"
#include "FalconView/UIThreadOperation.h"
#include "FvCore/Include/Registry.h"
#include "MapEngineCOM.h"
#include "MapRenderingOnTiles.h"
#include "maps_d.h"

LoadDrapedOverlayImageOperation::LoadDrapedOverlayImageOperation(
   DrapedOverlayImage* draped_overlay_image,
   osgEarth::MapNode* map_node,
   const osgEarth::TileKey& key,
   int tile_width_and_height,
   const MapType& map_type) :
   m_draped_overlay_image(draped_overlay_image),
   m_done(false),
   m_tile_width_and_height(tile_width_and_height),
   m_tile_key(key),
   m_map_node(map_node),
   m_map_type(map_type)
{
   m_min_level_of_detail = fvw::GetMinLevelOfDetail(map_type,
      tile_width_and_height);
}

LoadDrapedOverlayImageOperation::~LoadDrapedOverlayImageOperation()
{
}

void LoadDrapedOverlayImageOperation::operator()(osg::Object* )
{
   osg::ref_ptr<DrapedOverlayImage> local_draped_overlay_image =
         m_draped_overlay_image;

   if (local_draped_overlay_image.valid())
   {
      LoadImage();

      GenericNodeCallback* node_callback = new GenericNodeCallback;
      node_callback->SetOperation(m_map_node,
         [local_draped_overlay_image]()
      {
         local_draped_overlay_image->UpdateImage();
      });
   }
}

void LoadImageImpl( double west, double south, double east, double north,
   double tile_size, osgEarth::MapNode* map_node,
   const osgEarth::SpatialReference* srs, MapType& map_type,
   unsigned char* data)
{
   // We already know we are not on the main thread as this object is handled
   // by osgearth's rendering threads.  We need to perform this work on the main
   // thread for these reasons:
   //  - COM objects must be called on their owning thread, for FV, this is the
   // main thread.
   //  - Overlays may be opened or closed at any time, and we can avoid race
   // conditions during a draw with such overlays by enumerating them on the
   // main thread, the only thread that opens and closes them.
   auto on_draw = [&]()
   {
      osgEarth::GeoExtent geo_extent(srs, west, south, east, north);

      MapEngineCOM* map_engine =
         fvw::GetMapRenderingEngineForCurrentThread(map_node->getMap(), false,
         false);

      int ignored_status;
      fvw::PrepareForRendering(map_engine, map_type, geo_extent,
         tile_size, tile_size, &ignored_status);

      // setup the callback with the current state
      IMapRenderingEngineCallback* callback = map_engine->GetCallback();
      OsgEarthMapEngineCallback* osg_earth_callback =
         static_cast<OsgEarthMapEngineCallback*>(callback);
      fvw::PrepareCallback(osg_earth_callback, map_type, geo_extent,
         tile_size, tile_size);

      IOverlayRendererPtr rendering_engine = map_engine->GetRenderingEngine();
      rendering_engine->DrawOverlaysToRGBA(
         south, west, north, east,
         tile_size, tile_size, data);
   };

   new fvw::UIThreadOperation(std::move(on_draw), true);
}

void LoadDrapedOverlayImageOperation::LoadImage()
{
   const double fake_multiplier = 2.0;

   const osgEarth::GeoExtent original_extent = m_tile_key.getExtent();
   const double latitude_delta = (fake_multiplier - 1.0) *
      (original_extent.north() - original_extent.south());
   const double longitude_delta = (fake_multiplier - 1.0) *
      (original_extent.east() - original_extent.west());

   // Split the delta across both sides of the fake tile.
   double west = original_extent.west() - longitude_delta/2;
   double south = original_extent.south() - latitude_delta/2;
   double east = original_extent.east() + longitude_delta/2;
   double north = original_extent.north() + latitude_delta/2;

   // Expanding over the poles causes trouble: there's nothing above or below
   // the poles.  We don't handle clipping as nicely in those areas.
   if ( north > 90 || south < -90)
   {
      west = original_extent.west();
      south = original_extent.south();
      east = original_extent.east();
      north = original_extent.north();
      m_image = new osg::Image();
      m_image->allocateImage(m_tile_width_and_height, m_tile_width_and_height,
         1, GL_RGBA, GL_UNSIGNED_BYTE);
      LoadImageImpl(west, south, east, north,
         m_tile_width_and_height, m_map_node,
         original_extent.getSRS(), m_map_type, m_image->data());
   }
   else
   {
      // Overlays which do not handle clipping appropriately display chopped
      // images where tiles meet.  For example, many overlays display point
      // symbols only if the center of the symbol appears on the image.  This
      // leads to symbols that are partially cut off due to the center appearing
      // just on the edge of one tile and not at all on the adjacent tile.

      // To correct this, we render images larger than will be displayed and
      // only display the proper portion on the tiles.  In the above example,
      // this gives a greater likely hood of the center of a point appearing on
      // the rendered image of the adjancent tile, though the center of the
      // image would not be displayed on that adjacent tile.

      // Longitudes should be between -180 and 180.
      {
         while ( west < -180 )
            west += 360;
         while (east > 180 )
            east -= 360;
      }

      // Render the oversized tile.
      const double fake_tile_size = floor(
         static_cast<double>(m_tile_width_and_height) * fake_multiplier);
      unsigned char* fake_data =
         new unsigned char[fake_tile_size*fake_tile_size*4];
      LoadImageImpl(west, south, east, north,
         fake_tile_size, m_map_node, original_extent.getSRS(),
         m_map_type, fake_data);

      // Prepare the tile that will be stored.
      //
      m_image = new osg::Image();
      m_image->allocateImage(m_tile_width_and_height, m_tile_width_and_height,
         1, GL_RGBA, GL_UNSIGNED_BYTE);

      // Chop off the outside bounds to get to the proper tile size.
      {
         unsigned char* data_ptr = m_image->data();
         unsigned char* fake_data_ptr = fake_data;

         // Get half the difference in size between the oversized tile and the
         // properly sized tile.  This is how much we're cutting off of each
         // edge of the oversized tile.
         int half_size_difference =
            (fake_tile_size - m_tile_width_and_height)/2;

         // Forward to the first byte of data that will be on the properly sized
         // tile.
         fake_data_ptr += static_cast<int>(
            (fake_tile_size * half_size_difference + half_size_difference)*4);

         // Copy the desired data.
         int row_width = m_tile_width_and_height * 4;
         int fake_row_width = fake_tile_size * 4;
         for (int row = 0 ; row < m_tile_width_and_height ; ++row)
         {
            memcpy(data_ptr, fake_data_ptr, row_width);
            data_ptr += row_width;
            fake_data_ptr += fake_row_width;
         }
      }

      delete[] fake_data;
   }

   m_done = true;
}
