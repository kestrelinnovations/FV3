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

// DrapedOverlayImage.cpp
//

#include "stdafx.h"

#include "DrapedOverlayImage.h"

#include "osgEarth/ImageUtils"
#include "osgEarth/MapNode"

#include "Common/ScopedCriticalSection.h"

#include "FalconView/DrapedOverlayImageNodeVisitor.h"
#include "FalconView/GenericNodeCallback.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/getobjpr.h"
#include "FalconView/include/maps_d.h"
#include "FalconView/include/overlay.h"
#include "FalconView/LoadDrapedOverlayImageOperation.h"
#include "FalconView/MapRenderingOnTiles.h"
#include "FalconView/MapView.h"
#include "FalconView/UIThreadOperation.h"

namespace
{
// Private class to notify images of map param changes in a thread safe manner
class MapParamsListener : public MapParamsListener_Interface
{
public:
   MapParamsListener()
   {
      ::InitializeCriticalSection(&m_critical_section);
   }
   ~MapParamsListener()
   {
      // Enter the critical section to make sure we're not destroyed while
      // iterating.
      ::EnterCriticalSection(&m_critical_section);
      ::DeleteCriticalSection(&m_critical_section);
   }

   void AddListener(MapParamsListener_Interface* listener)
   {
      fv_common::ScopedCriticalSection lock(&m_critical_section);

      // Only add a listener once.
      auto location = std::find( m_listeners.begin(), m_listeners.end(),
         listener);
      if (location == m_listeners.end())
      {
         m_listeners.push_back(listener);
      }
   }

   void RemoveListener(MapParamsListener_Interface* listener)
   {
      fv_common::ScopedCriticalSection lock(&m_critical_section);
      m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(),
         listener), m_listeners.end());
   }

   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override
   {
      // If the map type hasn't changed there is nothing to do
      if ((change_mask & MAP_TYPE_CHANGED) != MAP_TYPE_CHANGED)
         return;

      // First, notify each listener that the map type has changed
      {
         fv_common::ScopedCriticalSection lock(&m_critical_section);

         // If the only reference to the current DrapedOverlayImage is by
         // its m_load_image_op then calling MapParamsChanged can end up
         // deleting the DrapedOverlayImage (from the call to Invalidate).
         // Since we are iterating through the listeners, deleting a
         // DrapedOverlayImage will cause the iterator after the point of
         // erasure to become invalidated and cause a potential crash. To get
         // around this, we can iterate in the reverse since previous values
         // in the vector will not be invalidated.
         std::for_each(m_listeners.rbegin(), m_listeners.rend(),
            [&](MapParamsListener_Interface* listener)
         {
            listener->MapParamsChanged(change_mask, map_group_identity,
               map_type, map_proj_params, map_display_params);
         });
         // allow the scope loss to release the lock before proceeding
      }

      // Then, invalidate the map
      MapView* map_view = fvw_get_view();
      if (map_view)
      {
         map_view->invalidate_view();
      }
   }

private:

   CRITICAL_SECTION m_critical_section;
   std::vector<MapParamsListener_Interface *> m_listeners;

} s_map_params_listener;

// Fixed blank image
const int BLANK_IMAGE_WIDTH = 256;
const int BLANK_IMAGE_HEIGHT = 256;
const int BLANK_IMAGE_SIZE_BYTES = BLANK_IMAGE_WIDTH * BLANK_IMAGE_HEIGHT * 4;
unsigned char s_blank_image[BLANK_IMAGE_SIZE_BYTES];

}

bool DrapedOverlayImage::s_allow_new_operations = true;

/* static */
void DrapedOverlayImage::Initialize()
{
   static bool s_initialized = false;
   if (!s_initialized)
   {
      s_initialized = true;
      MapView* map_view = fvw_get_view();
      if (map_view)
         map_view->AddMapParamsListener(&s_map_params_listener);

      memset(s_blank_image, 0, BLANK_IMAGE_SIZE_BYTES);
   }
}

DrapedOverlayImage::DrapedOverlayImage(osgEarth::MapNode* map_node,
   const osgEarth::TileKey& key, int tile_width_and_height,
   const MapType map_type, osg::OperationsThread* operations_thread) :
m_tile_key(key),
m_tile_width_and_height(tile_width_and_height),
m_operations_thread(operations_thread),
m_map_node(map_node),
m_map_type(map_type),
m_need_to_load_image_on_next_update(true)
{
   // Blank images for overlay should be transparent since it they are drawn
   // over the map.
   setImage(BLANK_IMAGE_WIDTH, BLANK_IMAGE_HEIGHT, 1, GL_RGBA, GL_RGBA,
      GL_UNSIGNED_BYTE, s_blank_image, osg::Image::NO_DELETE);

   s_map_params_listener.AddListener(this);
   Invalidate();
}

DrapedOverlayImage::~DrapedOverlayImage()
{
   s_map_params_listener.RemoveListener(this);

   if (m_load_image_op.valid())
   {
      m_load_image_op->ResetDrapedOverlayImage();
   }
}

void DrapedOverlayImage::MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params)
{
   // If the map type hasn't changed there is nothing to do
   if ((change_mask & MAP_TYPE_CHANGED) != MAP_TYPE_CHANGED)
      return;

   if (m_map_type != map_type)
   {
      m_map_type = map_type;
      Invalidate();
   }
}

void DrapedOverlayImage::Invalidate()
{
   m_need_to_load_image_on_next_update = true;
   if (m_load_image_op.valid())
      m_load_image_op->ResetDrapedOverlayImage();
}

// If the m_load_image_op is complete then update the contents of this image
// with the new pixels
void DrapedOverlayImage::UpdateImage() 
{
   if (m_load_image_op.valid() && m_load_image_op->m_done)
   {
      osg::ref_ptr<osg::Image> image = m_load_image_op->m_image.get();
      if (image.valid())
      {
         copyImage(image.get());
      }
      m_load_image_op = nullptr;
   }
}

// Copies the contents of the given image into this image.
void DrapedOverlayImage::copyImage(osg::Image* image)
{
   if (image)
   {
      // Transfer ownership of data to this Image
      setImage(image->s(), image->t(), image->r(),
         image->getInternalTextureFormat(), image->getPixelFormat(),
         image->getDataType(), image->releaseData(),
         image->getAllocationMode(), image->getPacking());
   }
}

// Update method for osg::Image subclasses that update themselves
// during the update traversal
void DrapedOverlayImage::update(osg::NodeVisitor* nv)
{
   if (m_need_to_load_image_on_next_update)
   {
      m_need_to_load_image_on_next_update = false;

      // If there is a LoadImageOperation associated with this object, then it
      // will need to be reset before reassigning m_load_image_op
      if (m_load_image_op.valid())
      {
         m_load_image_op->ResetDrapedOverlayImage();
      }

      // Load the new map in a background thread
      m_load_image_op = new LoadDrapedOverlayImageOperation(this, m_map_node,
         m_tile_key, m_tile_width_and_height, m_map_type);

      if (m_operations_thread.valid() &&
         DrapedOverlayImage::s_allow_new_operations)
      {
         m_operations_thread->add(m_load_image_op.get());
      }
   }
}
