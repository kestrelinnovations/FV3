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

// GeospatialViewController.cpp

#include "stdafx.h"

#include "GeospatialViewController.h"

#include "osgEarth/MapNode"
#include "osgEarthUtil/EarthManipulator"
#include "osgEarthUtil/SkyNode"
#include "osgViewer/Viewer"

#include "Common/ScopedCriticalSection.h"

#include "FalconView/GenericNodeCallback.h"
#include "FalconView/GeospatialScene.h"
#include "FalconView/GeospatialView.h"
#include "FalconView/include/OverlayElements.h"
#include "FalconView/include/mapx.h"
#include "FalconView/MapRenderingEngineTileSource.h"
#include "FalconView/RenderFunctions.h"

#pragma warning(disable:4482)

const double ANIMATION_TRANSITION_TIME_SECONDS = 0.0;

GeospatialViewController::GeospatialViewController() :
m_geospatial_scene(nullptr),
m_geospatial_view(nullptr),
m_map_type_selector(nullptr),
m_current_map_proj(nullptr)
{
   ::InitializeCriticalSection(&m_viewpoint_critical_section);
}

GeospatialViewController::~GeospatialViewController()
{
   delete m_geospatial_scene;
   delete m_geospatial_view;
   delete m_current_map_proj;

   ::DeleteCriticalSection(&m_viewpoint_critical_section);
}

void GeospatialViewController::OnRightMousePressed(UINT nFlags, CPoint point)
{
   //m_geospatial_view->m_viewer->fIDevState->right_pressed_viewshed = true;
   if (GetGeospatialScene()->GetUpdateCameraAndTexGenCallback() != nullptr)
   {
      if (GetGeospatialScene()->GetViewshedEnabledValue())
      {
         if (this->GetGeospatialView()->m_viewer->fIDevState->debugCount == 1)
            GetGeospatialScene()->SetCreateNewViewshed(true);
         //GetGeospatialScene()->viewshed->Update();
      }
      else
      {
         GetGeospatialScene()->SetCreateNewViewshed(false);
         //GetGeospatialScene()->viewshed->Update();
      }
   }
}

FlightInputDeviceStateType* GeospatialViewController::GetInputDevice()
{
    return GetGeospatialView() == nullptr
       ? nullptr : GetGeospatialView()->Viewer()->fIDevState;
}

void GeospatialViewController::SetViewshed(bool b)
{
   GetGeospatialScene()->GetViewshedEnabledUniform()->set(b);
}

d_geo_t* GeospatialViewController::GetActionCenter()
{
   return &m_geospatial_view->m_viewer->fIDevState->action_center;
}

void GeospatialViewController::OnMouseMoved(UINT nFlags, CPoint point)
{
   if (m_geospatial_view != nullptr) // can happen over RDP
      m_geospatial_view->Viewer()->fIDevState->OnMouseMoved(nFlags, point);
}

void GeospatialViewController::OnLeftMousePressed(UINT nFlags, CPoint point)
{
   m_geospatial_view->Viewer()->fIDevState->OnMouseLeftPressed(nFlags, point);
}

void GeospatialViewController::OnLeftMouseRelease(UINT nFlags, CPoint point)
{
   m_geospatial_view->Viewer()->fIDevState->OnLeftMouseRelease(nFlags, point);
}

bool GeospatialViewController::SetUpButtonState(bool pressed)
{
   return m_geospatial_view->Viewer()->fIDevState->
      SetUpButtonState(pressed);
}

bool GeospatialViewController::SetDownButtonState(bool pressed)
{
   return m_geospatial_view->Viewer()->fIDevState->
      SetDownButtonState(pressed);
}

bool GeospatialViewController::SetLeftButtonState(bool pressed)
{
   return m_geospatial_view->Viewer()->fIDevState->
      SetLeftButtonState(pressed);
}

bool GeospatialViewController::SetRightButtonState(bool pressed)
{
   return m_geospatial_view->Viewer()->fIDevState->
      SetRightButtonState(pressed);
}


bool GeospatialViewController::SetRotCwButtonState(bool pressed)
{
   return m_geospatial_view->Viewer()->fIDevState->
      SetRotCwButtonState(pressed);
}

bool GeospatialViewController::SetRotCcwButtonState(bool pressed)
{
   return m_geospatial_view->Viewer()->fIDevState->
      SetRotCcwButtonState(pressed);
}

bool GeospatialViewController::SetNorthUpButtonState(bool pressed)
{
   return m_geospatial_view->Viewer()->fIDevState->
      SetNorthUpButtonState(pressed);
}

const osgEarth::SpatialReference* GeospatialViewController::GetSRS()
{
   return m_geospatial_view->Viewer()->fIDevState->GetSRS();
}

void GeospatialViewController::Initialize(CWnd* wnd, int window_width,
   int window_height)
{
   SYSTEM_INFO sysinfo;
   ::GetSystemInfo(&sysinfo);
   DWORD default_num_pager_threads = max(1,
      (sysinfo.dwNumberOfProcessors / 4));

   osg::DisplaySettings::instance()->setNumOfDatabaseThreadsHint(
      PRM_get_registry_int("Main", "NumOf3dDatabasePagerThreads",
         default_num_pager_threads));

   osg::DisplaySettings::instance()->setNumOfHttpDatabaseThreadsHint(0);

   m_geospatial_scene = new GeospatialScene();
   m_geospatial_scene->Initialize(this, window_width, window_height);

   m_geospatial_view = new GeospatialView();
   m_geospatial_view->Initialize(
      m_geospatial_scene->MapNode()->getMap()->getSRS(),
      wnd->GetSafeHwnd(), window_width, window_height, this);
   m_geospatial_view->m_viewer->SetWalkManipulatorSpeed(100);

   // The initialization of the map tile source depends on the database
   // pager set up by the geospatial view. That is why this call is not
   // currently a part of the scene's Initialize()
   m_geospatial_scene->InitializeMapTileSource();

   m_geospatial_view->SetSceneData(m_geospatial_scene->RootNode());
   m_geospatial_scene->InitializeShaders();

   // give the overlay manager a pointer to the root map node
   OVL_get_overlay_manager()->SetMapNode(m_geospatial_scene->MapNode());
}

void GeospatialViewController::BeginRendering()
{
   if (m_geospatial_view)
      m_geospatial_view->BeginRendering();
}

void GeospatialViewController::AddEarthManipulatorListener(
   EarthManipulatorEvents_Interface* listener)
{
   if (m_geospatial_view != nullptr)
   {
      m_geospatial_view->m_camera_earth_manipulator->AddListener(
         listener);
   }
}

void GeospatialViewController::RemoveEarthManipulatorListener(
   EarthManipulatorEvents_Interface* listener)
{
   if (m_geospatial_view != nullptr)
   {
      m_geospatial_view->m_camera_earth_manipulator->RemoveListener(
         listener);
   }
}

void GeospatialViewController::Invalidate()
{
   if (m_geospatial_view)
      m_geospatial_view->Viewer()->requestRedraw();
}

void GeospatialViewController::RedrawMapFromScratch()
{
   if (m_geospatial_scene)
   {
      auto map_tile_source = m_geospatial_scene->m_map_tile_source;
      if (map_tile_source != nullptr)
      {
         map_tile_source->ResetTileNodes();
      }
   }
}

void GeospatialViewController::EnableDraw(BOOL enabled)
{
   if (m_geospatial_view != nullptr)
      m_geospatial_view->EnableDraw(enabled);
}

void GeospatialViewController::SetIsPanning(BOOL panning)
{
   if (m_geospatial_view != nullptr)
   {
      m_geospatial_view->m_viewer->GetState()->SetIsPanning(panning);
      m_geospatial_view->m_viewer->MapChangeStateRequest = true;
   }
}

void GeospatialViewController::GetViewpoint3D(Viewpoint3d* viewpoint)
{
   fv_common::ScopedCriticalSection lock(&m_viewpoint_critical_section);
   viewpoint->x = m_viewpoint_3d.x;
   viewpoint->y = m_viewpoint_3d.y;
   viewpoint->z = m_viewpoint_3d.z;
   viewpoint->heading = m_viewpoint_3d.heading;
   viewpoint->pitch = m_viewpoint_3d.pitch;
   viewpoint->range = m_viewpoint_3d.range;
}

void GeospatialViewController::UpdateViewpoint3D()
{
   if (m_geospatial_view != nullptr)
   {
      osgEarth::Viewpoint vp = m_geospatial_view->m_camera_earth_manipulator->
         getViewpoint();
      fv_common::ScopedCriticalSection lock(&m_viewpoint_critical_section);
      m_viewpoint_3d.x = vp.x();
      m_viewpoint_3d.y = vp.y();
      m_viewpoint_3d.z = vp.z();
      m_viewpoint_3d.heading = vp.getHeading();
      m_viewpoint_3d.pitch = vp.getPitch();
      m_viewpoint_3d.range = vp.getRange();
   }
}

void GeospatialViewController::SetViewpoint3D(const Viewpoint3d& viewpoint,
   double duration_s /* = ANIMATION_TRANSITION_TIME_SECONDS */)
{
   if (m_geospatial_view != nullptr)
   {
      m_geospatial_view->m_viewer->fIDevState->throwing = false;
      m_geospatial_view->m_viewer->fIDevState->allowNewArrowKeyInput = true;
      osgEarth::Viewpoint vp(viewpoint.x, viewpoint.y, viewpoint.z,
         viewpoint.heading, viewpoint.pitch, viewpoint.range);
      m_geospatial_view->m_camera_earth_manipulator->
         cancelViewpointTransition();
      m_geospatial_view->m_camera_earth_manipulator->
         setViewpoint(vp, duration_s);
      GetGeospatialScene()->
         m_HUD->m_HUD_update_request = true;
   }
}

OverlayElements* GeospatialViewController::CreateOverlayElements()
{
   if (m_geospatial_scene != nullptr)
   {
      return new OverlayElements(m_geospatial_scene->m_map_node,
         m_geospatial_view->GetControlCanvas());
   }

   // Initialize needs to be called first
   return nullptr;
}
