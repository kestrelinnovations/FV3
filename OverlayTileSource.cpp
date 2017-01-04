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

// OverlayTileSoure.cpp

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/OverlayTileSource.h"

// system includes

// third party files
#include "osgEarth/MapNode"
#include "osgEarth/Registry"

// other FalconView headers

// this project's headers
#include "FalconView/DrapedOverlayImage.h"
#include "FalconView/DrapedOverlayImageNodeVisitor.h"
#include "FalconView/include/MAPS_D.H"
#include "FalconView/include/overlay.h"
#include "FalconView/GenericNodeCallback.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/getobjpr.h"
#include "FalconView/MapEngineCOM.h"
#include "FalconView/MapRenderingOnTiles.h"
#include "FalconView/MapView.h"
#include "FalconView/OsgEarthMapEngineCallback.h"
#include "FalconView/UIThreadOperation.h"

OverlayTileSource::OverlayTileSource(osgEarth::MapNode* map_node,
   const osgEarth::TileSourceOptions& options,
   GeospatialViewController* view_controller, const MapType& map_type,
   ProjectionEnum projection_type) :
m_map_node(map_node),
m_view_controller(view_controller),
osgEarth::TileSource(options),
m_map_type(map_type),
m_proj_type(projection_type),
m_need_to_invalidate(true)
{
   initialize(nullptr, nullptr);

   m_operations_thread = new osg::OperationsThread;
   m_operations_thread->start();

   DrapedOverlayImage::Initialize();

   // Sign up for map change notifications
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->AddMapParamsListener(this);

   C_ovl_mgr* manager = OVL_get_overlay_manager();
   manager->RegisterEvents(this);

   C_overlay* overlay = manager->get_first();
   while ( overlay != nullptr )
   {
      overlay->AddOverlayInvalidatedListener(this);
      overlay = manager->get_next(overlay);
   }
   Invalidate();
}

OverlayTileSource::~OverlayTileSource()
{
   C_ovl_mgr* manager = OVL_get_overlay_manager();
   C_overlay* overlay = manager->get_first();
   while ( overlay != nullptr )
   {
      overlay->RemoveOverlayInvalidatedListener(this);
      overlay = manager->get_next(overlay);
   }

   manager->UnregisterEvents(this);

   AbortCreatingAllImages();
}

void OverlayTileSource::initialize(const osgDB::Options* options,
   const osgEarth::Profile* profile)
{
   if (profile)
      setProfile(profile);
   else
      setProfile(osgEarth::Registry::instance()->getGlobalGeodeticProfile());
}

osg::Image* OverlayTileSource::createImage(
   const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress )
{
   return new DrapedOverlayImage(m_map_node, key, getOptions().tileSize().value(),
      m_map_type, m_operations_thread);
}

void OverlayTileSource::MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params)
{
   if (change_mask & MAP_TYPE_CHANGED)
   {
      AbortCreatingAllImages();

      m_map_type = map_type;
   }
   if (change_mask & MAP_PROJ_TYPE_CHANGED)
   {
      // If we are going from 3D to 2D, then reset the flag used to indicate
      // we need to invalidate the draped overlay tiles
      if (m_proj_type == GENERAL_PERSPECTIVE_PROJECTION &&
         map_proj_params.type != GENERAL_PERSPECTIVE_PROJECTION)
      {
         m_need_to_invalidate = true;
      }

      m_proj_type = map_proj_params.type;
   }
}

void OverlayTileSource::AbortCreatingAllImages()
{
   if (m_operations_thread.valid())
      m_operations_thread->removeAllOperations();
}

void OverlayTileSource::TerminateOperationsThread()
{
   if (m_operations_thread.valid())
   {
      // Disallow any new operations from being added to the operation queue
      DrapedOverlayImage::s_allow_new_operations = false;

      // Eliminate any pending operations from the queue to avoid a deadlock
      // from main thread waiting on operation queue to terminate and operation
      // thread waiting on UI thread to render an overlay
      MSG msg;
      while (::PeekMessage(&msg, NULL,
         fvw::UIThreadOperation::UI_THREAD_OPERATION,
         fvw::UIThreadOperation::UI_THREAD_OPERATION, PM_REMOVE))
      {
      }

      m_operations_thread->cancel();
   }
}

void OverlayTileSource::OverlayAdded(C_overlay* overlay)
{
   overlay->AddOverlayInvalidatedListener(this);
   Invalidate();
}

void OverlayTileSource::OverlayOrderChanged()
{
   C_ovl_mgr* manager = OVL_get_overlay_manager();
   manager->reset_display_node_visibility();
   Invalidate();
}

void OverlayTileSource::OverlayRemoved(C_overlay* overlay)
{
   overlay->RemoveOverlayInvalidatedListener(this);
   Invalidate();
}

void OverlayTileSource::Invalidate()
{
   if (m_need_to_invalidate || m_proj_type == GENERAL_PERSPECTIVE_PROJECTION)
   {
      m_need_to_invalidate = false;

      GenericNodeCallback* node_callback = new GenericNodeCallback;
      node_callback->SetOperation(m_map_node, [this]()
      {
         DrapedOverlayImageNodeVisitor visitor;
         m_map_node->accept(visitor);
      });
   }
}
