// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#ifndef FALCONVIEW_GEOSPATIAL_VIEW_H
#define FALCONVIEW_GEOSPATIAL_VIEW_H

class GeospatialViewController;
class UpdateCameraAndTexGenCallback;
class PreRenderCallBack;
class GeoKeyboardEventHandler;
class MapEngineOSG;
class FlightInputDeviceStateType;

#include "GeospatialViewer.h"
#include "GeospatialEarthManipulator.h"
#include "osgGA/AnimationPathManipulator"

#define STARTING_HEIGHT_ABOVE_ELLIPSOID_METERS 25000000.0

namespace osg
{
   class Node;
}

namespace osgEarth
{
   namespace Util
   {
      class EarthManipulator;
      namespace Controls
      {
         class ControlCanvas;
      }
   }
}

namespace osgViewer
{
   class Viewer;
}

namespace fvw
{
   class FlightViewManipulator : public osgGA::FirstPersonManipulator
   {
   public:
      fvw::FlightViewManipulator::FlightViewManipulator();
      bool handleKeyDown( const osgGA::GUIEventAdapter&,
         osgGA::GUIActionAdapter& ) override
      {
         return false;
      }

      GeospatialViewer* viewer;
      int m_desired_zoom_percent;
      void ActivateModeFrom(fvw::CameraMode previous_camera_mode);
      void Activate2D();
      int frames_to_redraw;
   };

   class PerformChangeToFlightMode : public AnimatedTransitionCallback
   {
   public:
      PerformChangeToFlightMode(GeospatialViewer* viewer, FlightViewManipulator* manipulator);
   private:
      void AnimationFrame(double t) override;
      GeospatialViewer* m_viewer;
      FlightViewManipulator* m_manipulator;

      // These are set only once
      osg::Vec3d m_position_old;
      osg::Vec3d m_forward_old;
      osg::Vec3d m_up_old;
      osg::Vec3d m_position;
      osg::Vec3d m_forward;
      osg::Vec3d m_up;
   };

   class PerformChangeFromFlightModeTo2D : public AnimatedTransitionCallback
   {
   public:
      PerformChangeFromFlightModeTo2D(GeospatialViewer* viewer, FlightViewManipulator* manipulator);
   private:
      void AnimationFrame(double t) override;
      GeospatialViewer* m_viewer;
      FlightViewManipulator* m_manipulator;

      // These are set only once
      osg::Vec3d m_position_old;
      osg::Vec3d m_forward_old;
      osg::Vec3d m_up_old;
      osg::Vec3d m_position;
      osg::Vec3d m_forward;
      osg::Vec3d m_up;
      osg::Vec3d m_position_normalized;
      osg::Vec3d m_forward_normalized;
   };

   class GroundViewManipulator : public osgGA::FirstPersonManipulator
   {
   public:
      fvw::GroundViewManipulator::GroundViewManipulator();
      bool handleKeyDown( const osgGA::GUIEventAdapter&,
         osgGA::GUIActionAdapter& ) override
      {
         return false;
      }

      GeospatialViewer* viewer;
      int m_max_angle_ground;
      int m_desired_zoom_percent;
      void ActivateModeFrom(fvw::CameraMode previous_camera_mode);
      void Activate2D();
      int frames_to_redraw;
   };

   class PerformChangeToGroundMode : public AnimatedTransitionCallback
   {
   public:
      PerformChangeToGroundMode(GeospatialViewer* viewer, GroundViewManipulator* manipulator);
   private:
      void AnimationFrame(double t) override;
      GeospatialViewer* m_viewer;
      GroundViewManipulator* m_manipulator;

      // These are set only once
      osg::Vec3d m_position_old;
      osg::Vec3d m_forward_old;
      osg::Vec3d m_up_old;
      osg::Vec3d m_position;
      osg::Vec3d m_forward;
      osg::Vec3d m_up;
   };

   class PerformChangeFromGroundModeTo2D : public AnimatedTransitionCallback
   {
   public:
      PerformChangeFromGroundModeTo2D(GeospatialViewer* viewer, GroundViewManipulator* manipulator);
   private:
      void AnimationFrame(double t) override;
      GeospatialViewer* m_viewer;
      GroundViewManipulator* m_manipulator;

      // These are set only once
      osg::Vec3d m_position_old;
      osg::Vec3d m_forward_old;
      osg::Vec3d m_up_old;
      osg::Vec3d m_position;
      osg::Vec3d m_forward;
      osg::Vec3d m_up;
      osg::Vec3d m_position_normalized;
      osg::Vec3d m_forward_normalized;
   };
}

// The GeospatialView handles the rendering of Geospatial data. It also wraps
// the GeospatialViewer class which implements the osgViewer::Viewer class.
class GeospatialView
{
   // Below friend classes allow GeospatialViewController to 
   // call SetCameraManipulator, SetSceneData, and Viewer
   friend GeospatialViewController;
   friend GeospatialViewer;
   friend MapView;
   friend UpdateCameraAndTexGenCallback;
   friend PreRenderCallBack;
   friend GeoKeyboardEventHandler;
   friend MapEngineOSG;
   friend FlightInputDeviceStateType;

public:
   GeospatialView();

   GeospatialViewer* Viewer() const
   {
      return m_viewer;
   }

   void Initialize(const osgEarth::SpatialReference* srs, HWND hwnd,
      int window_width, int window_height, GeospatialViewController* gvc);
   void BeginRendering();
   void TerminateRenderingThread(); // blocks until thread has exited

   osgEarth::Util::Controls::ControlCanvas* GetControlCanvas() const
   {
      return m_control_canvas;
   }

   void EnableDraw(BOOL enabled);

private:
   osg::ref_ptr<GeospatialViewer> m_viewer;
   HANDLE m_rendering_thread;
   osg::ref_ptr<osg::Referenced> windata;
   osg::ref_ptr<osg::GraphicsContext::Traits> traits;

   osg::ref_ptr<GeospatialEarthManipulator> m_camera_earth_manipulator;
   osg::ref_ptr<fvw::FlightViewManipulator> m_camera_flight_manipulator;
   osg::ref_ptr<fvw::GroundViewManipulator> m_camera_walk_manipulator;

   // Control canvas assocatied with the Viewer
   osgEarth::Util::Controls::ControlCanvas* m_control_canvas;

   static bool s_is_rendering_enabled;
   static WNDPROC s_mapview_wndproc;
   static WNDPROC s_chained_osg_to_mapview_wndproc;

   void SetSceneData(osg::Node* node);

   // Entry point for rendering thread
   static unsigned __stdcall Render(void* ptr);
   
   // Handles windows messages based on the enabled state of this renderer
   //
   // The chain of windows procedures looks one of the following:
   //
   //    GeospatialView -> GraphicsWindowWin32 -> MapView, if enabled OR
   //    GeospatialView -> MapView, otherwise
   //
   static LRESULT CALLBACK WindowProc(
      HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

   DISABLE_COPY_AND_ASSIGN_CTORS(GeospatialView);
};

#endif