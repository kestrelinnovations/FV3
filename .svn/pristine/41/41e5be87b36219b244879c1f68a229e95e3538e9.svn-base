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

// GeospatialViewController.h

#ifndef FALCONVIEW_GEOSPATIALVIEWCONTROLLER_H
#define FALCONVIEW_GEOSPATIALVIEWCONTROLLER_H

#include "osg/ref_ptr"
#include "FalconView/Viewpoint3d.h"
#include "GeospatialScene.h"
#include "osgGA/FlightManipulator"

class GeospatialScene;
class GeospatialView;
class GeospatialEarthManipulator; //mine
class MapTypeSelectorCallback;

//#define DEBUG_CAMERA debugging polygon selection

#define TRAVERSE_NODE 0xFFFFFFFF
#define DISABLE_TRAVERSE_NODE 0

namespace osgEarth
{
   namespace Util
   {
      class EarthManipulator;
   }
   class SpatialReference;
}

class EarthManipulatorEvents_Interface;
class OverlayElements;
class SettableMapProj;
class ViewMapProjImpl;
class GeospatialViewer;
class UpdateCameraAndTexGenCallback;
class SlopeShaderCallback;
class GeospatialScene::OverlayStackObserver;
class MapTiltSlider;

extern const double ANIMATION_TRANSITION_TIME_SECONDS;

class GeospatialViewController
{
   friend CMainFrame;
   friend MapView;
   friend GeospatialViewer;
   friend UpdateCameraAndTexGenCallback;
   friend GeospatialScene::OverlayStackObserver;
   friend MapEngineOSG;
   friend MapTiltSlider;
   friend FlightInputDeviceStateType;

public:
   GeospatialViewController();
   ~GeospatialViewController();
   void SetViewshed(bool b);
   d_geo_t* GetActionCenter();
   void OnMouseMoved(UINT nFlags, CPoint point);
   void OnRightMousePressed(UINT nFlags, CPoint point);
   FlightInputDeviceStateType* GetInputDevice();
   void OnLeftMousePressed(UINT nFlags, CPoint point);
   void OnLeftMouseRelease(UINT nFlags, CPoint point);
   const osgEarth::SpatialReference* GeospatialViewController::GetSRS();
   bool SetUpButtonState(bool pressed);
   bool SetDownButtonState(bool pressed);
   bool SetLeftButtonState(bool pressed);
   bool SetRightButtonState(bool pressed);
   bool SetRotCwButtonState(bool pressed);
   bool SetNorthUpButtonState(bool pressed);
   bool SetRotCcwButtonState(bool pressed);

   void Initialize(CWnd *wnd, int window_width, int window_height);

   // Start the rendering thread
   void BeginRendering();

   // Add or remove an earth manipulator listener
   void AddEarthManipulatorListener(EarthManipulatorEvents_Interface* listener);
   void RemoveEarthManipulatorListener(
      EarthManipulatorEvents_Interface* listener);

   void GetViewpoint3D(Viewpoint3d* viewpoint);
   void SetViewpoint3D(const Viewpoint3d& viewpoint,
      double duration_s = ANIMATION_TRANSITION_TIME_SECONDS);
   void UpdateViewpoint3D();

   OverlayElements* CreateOverlayElements();

   void Invalidate();
   void RedrawMapFromScratch();

   void EnableDraw(BOOL enabled);

   void SetIsPanning(BOOL panning);

   GeospatialView* GetGeospatialView() const
   {
      return m_geospatial_view;
   }

   GeospatialScene* GetGeospatialScene()
   {
      return m_geospatial_scene;
   }

private:

   // The Geospatial scene contains and manages the map, elevation, overlays,
   // and model data.
   GeospatialScene* m_geospatial_scene;

   // The GeospatialView handles the rendering of the Geospatial data
   GeospatialView* m_geospatial_view;

   SettableMapProj* m_current_map_proj;

   MapTypeSelectorCallback* m_map_type_selector;
   Viewpoint3d m_viewpoint_3d;

   // The collection of listeners of map type change events
   CRITICAL_SECTION m_viewpoint_critical_section;

   DISABLE_COPY_AND_ASSIGN_CTORS(GeospatialViewController);
};

#endif  // FALCONVIEW_GEOSPATIALVIEWCONTROLLER_H
