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

#include "StdAfx.h"
#include "GeospatialView.h"

#include "osgEarthUtil/Controls"
#include "osgViewer/api/win32/GraphicsWindowWin32"
#include "osgViewer/Viewer"
#include "osgViewer/ViewerBase"
#include "osgViewer/ViewerEventHandlers"

#include "FalconView/GeospatialViewController.h"
#include "FvCore/Include/ThreadUtils.h"
#include "FalconView/FvMath.h"
#include "FalconView/OSGVariables.h"
#include "FalconView/RenderFunctions.h"
#include "FalconView/CameraUtils.h"

#pragma warning(disable:4482)

bool GeospatialView::s_is_rendering_enabled = false;
WNDPROC GeospatialView::s_mapview_wndproc = NULL;
WNDPROC GeospatialView::s_chained_osg_to_mapview_wndproc = NULL;
osg::Matrix projMatrix;

GeospatialView::GeospatialView() :
m_viewer(nullptr),
m_rendering_thread(INVALID_HANDLE_VALUE)
{
   m_camera_flight_manipulator = new fvw::FlightViewManipulator();
   m_camera_flight_manipulator->setVerticalAxisFixed(false);
   m_camera_flight_manipulator->setWheelMovement(0.0);
   m_camera_flight_manipulator->setAllowThrow(false);
   m_camera_flight_manipulator->frames_to_redraw = 0;
   m_camera_earth_manipulator = new GeospatialEarthManipulator();
   m_camera_earth_manipulator->frames_to_redraw = 0;
   m_camera_walk_manipulator = new fvw::GroundViewManipulator();
   m_camera_walk_manipulator->setAllowThrow(false);
   m_camera_walk_manipulator->frames_to_redraw = 0;
}

void GeospatialView::TerminateRenderingThread()
{
   if (m_viewer)
   {
      m_viewer->setDone(true);

      // ensures that the rendering thread is not halted
      m_viewer->EnableDraw(true);

      DWORD exit_code;
      if (::GetExitCodeThread(m_rendering_thread, &exit_code) &&
         exit_code == STILL_ACTIVE)
      {
         ::WaitForSingleObject(m_rendering_thread, INFINITE);
      }
   }
}

LRESULT CALLBACK GeospatialView::WindowProc(
   HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   // Switch base on the enabled state of the renderer
   if (s_is_rendering_enabled)
   {
      return ::CallWindowProc(s_chained_osg_to_mapview_wndproc,
         hwnd, uMsg, wParam, lParam);
   }
   else
   {
      return ::CallWindowProc(s_mapview_wndproc, hwnd, uMsg, wParam, lParam);
   }
}

namespace
{
   const double FIELD_OF_VIEW_Y = 15.0;
   const double Z_NEAR = 1.0;
   const double Z_FAR = 1000.0;

   struct ResizedCallback : public osg::GraphicsContext::ResizedCallback
   {
      virtual void resizedImplementation(osg::GraphicsContext* gc, int x,
         int y, int width, int height) override
      {
         auto cameras = gc->getCameras();
         for (auto it = cameras.begin(); it != cameras.end(); ++it)
         {
            // resize doesn't affect Cameras set up with FBO's
            if ((*it)->getRenderTargetImplementation() ==
               osg::Camera::FRAME_BUFFER_OBJECT)
            {
               continue;
            }

            auto viewport = (*it)->getViewport();
            if (viewport)
               viewport->setViewport(0, 0, width, height);

            double half_screen_height = ::GetSystemMetrics(SM_CYSCREEN) / 2.0;
            double z = half_screen_height / tan(DEG_TO_RAD(FIELD_OF_VIEW_Y));
            double half_new_fovy = RAD_TO_DEG(atan2(height / 2.0, z));

            (*it)->setProjectionMatrixAsPerspective(
               2.0 * half_new_fovy,
               static_cast<double>(width)/static_cast<double>(height),
               Z_NEAR, Z_FAR);
         }

         auto traits = const_cast<osg::GraphicsContext::Traits *>(
            gc->getTraits());
         traits->x = x;
         traits->y = y;
         traits->width = width;
         traits->height = height;
      }
   };
}

void GeospatialView::Initialize(const osgEarth::SpatialReference* srs,
   HWND hwnd, int window_width, int window_height, GeospatialViewController* gvc)
{
   // Create the viewer for this window
   m_viewer = new GeospatialViewer(srs, this, gvc);

   // Add a Stats Handler to the viewer
   m_viewer->addEventHandler(new osgViewer::StatsHandler);

   // Init the GraphicsContext Traits
   traits = new osg::GraphicsContext::Traits;

   // Init the Windata Variable that holds the handle for the Window to display
   // OSG in
   windata = new osgViewer::GraphicsWindowWin32::WindowData(hwnd);

   // Setup the traits parameters
   traits->x = 0;
   traits->y = 0;
   traits->width = ::GetSystemMetrics(SM_CXSCREEN);
   traits->height = ::GetSystemMetrics(SM_CYSCREEN);
   traits->windowDecoration = false;
   traits->doubleBuffer = true;
   traits->sharedContext = 0;
   traits->setInheritedWindowPixelFormat = true;
   traits->inheritedWindowData = windata;


   traits->mipMapGeneration = false;
   traits->useMultiThreadedOpenGLEngine = true;

   // Set the window system
   graphicswindow_Win32();

   // Acquire the current window proc for the given HWND. This will be
   // overridden by createGraphicsContext
   s_mapview_wndproc = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_WNDPROC);

   // Create the Graphics Context
   osg::GraphicsContext* gc =
      osg::GraphicsContext::createGraphicsContext(traits.get());

   gc->setResizedCallback(new ResizedCallback());

   // Replace the WNDPROC with ours so we can choose whether to use the
   // mapview or osg_to_mapview_chained WNDPROCs depending on the enabled
   // state of the renderer
   s_chained_osg_to_mapview_wndproc =
      (WNDPROC)::SetWindowLongPtr(hwnd, GWLP_WNDPROC, LONG_PTR(WindowProc));

   // Init a new Camera (Master for this View)
   osg::ref_ptr<osg::Camera> camera = new osg::Camera;
   camera->setName("user_camera");
#ifdef DEBUG_CAMERA
   //camera->addChild(gvc->GetGeospatialScene()->debug_cam);
   //projMatrix.makePerspective( 30.0, traits->width/traits->height, 1.0f, 10000.0f);
   //gvc->GetGeospatialScene()->debug_cam->setProjectionMatrix(projMatrix);
#endif

   // Assign Graphics Context to the Camera
   camera->setGraphicsContext(gc);

   // Set the viewport for the Camera
   camera->setViewport(new osg::Viewport(traits->x, traits->y,
      traits->width, traits->height));

   // Set projection matrix and camera attributes
   {
      camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      camera->setClearColor(osg::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
   }

   // Add the Camera to the Viewer
   m_viewer->setCamera(camera.get());

   // On demand frame scheme
   m_viewer->setRunFrameScheme(osgViewer::ViewerBase::ON_DEMAND);

   {
      using namespace osgEarth::Util;
      using namespace osgGA;
      EarthManipulator::Settings *settings =
         m_camera_earth_manipulator->getSettings();
      settings->setLockAzimuthWhilePanning(false);
      settings->setArcViewpointTransitions(false);

      // The MapView is responsible for handling and dispatching UI events to
      // the rendering engines. We will unbind all keys here.
      //

      // Set up manipulators
      EarthManipulator::ActionOptions options;

      // unbind spacebar
      settings->bindKey(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::KeySymbol::KEY_Space, 0, options);


      settings->bindKey(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::KEY_Space, 0L, options);
      settings->bindKey(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::KEY_Right, 0L, options);
      settings->bindKey(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::KEY_Left, 0L, options);
      settings->bindKey(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::KEY_Up, 0L, options);
      settings->bindKey(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::KEY_Down, 0L, options);

      // disable right click zoom
      settings->bindMouse(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::RIGHT_MOUSE_BUTTON);
      // let falconview handle left clicks
      settings->bindMouse(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::LEFT_MOUSE_BUTTON);
      // shift + left mouse button rotates view around center of screen
      settings->bindMouse(EarthManipulator::ACTION_ROTATE,
          GUIEventAdapter::LEFT_MOUSE_BUTTON,osgGA::GUIEventAdapter::MODKEY_SHIFT);
      // let falconview handle scroll wheel: change map type
      settings->bindScroll(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::SCROLL);
      settings->bindScroll(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::SCROLL_DOWN);
      settings->bindScroll(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::SCROLL_UP);


      settings->bindMouseDoubleClick(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::LEFT_MOUSE_BUTTON, 0, options);
      settings->bindMouseDoubleClick(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::RIGHT_MOUSE_BUTTON);
      settings->bindMouseDoubleClick(EarthManipulator::ACTION_NULL,
         GUIEventAdapter::LEFT_MOUSE_BUTTON,
         osgGA::GUIEventAdapter::MODKEY_CTRL, options);

      // zoom setup
      // left + right drag is zoom
      settings->bindMouse(EarthManipulator::ACTION_ZOOM, 
         GUIEventAdapter::LEFT_MOUSE_BUTTON | GUIEventAdapter::RIGHT_MOUSE_BUTTON);
      // unbind middle mouse before rebind
      settings->bindMouse(EarthManipulator::ACTION_NULL, 
         GUIEventAdapter::MIDDLE_MOUSE_BUTTON);
      settings->bindMouse(EarthManipulator::ACTION_ZOOM, 
         GUIEventAdapter::MIDDLE_MOUSE_BUTTON);
      /* Disabling alt + rmb currently
      settings->bindMouse(EarthManipulator::ACTION_ZOOM,
         GUIEventAdapter::RIGHT_MOUSE_BUTTON, osgGA::GUIEventAdapter::MODKEY_ALT);
      */
   }

   osgEarth::Viewpoint point(-90, 0, 0, 0, -90,
      STARTING_HEIGHT_ABOVE_ELLIPSOID_METERS);
   m_camera_earth_manipulator->setHomeViewpoint(point);
   m_viewer->setCameraManipulator(m_camera_earth_manipulator);

   // Create a control canvas associated with the viewer
   osgViewer::ViewerBase::Views views;
   m_viewer->getViews(views);
   m_control_canvas = osgEarth::Util::Controls::ControlCanvas::get(
      views[0], true);
}

void GeospatialView::SetSceneData(osg::Node* node)
{
   m_viewer->setSceneData(node);
}

void GeospatialView::BeginRendering()
{
   // Realize the Viewer
   m_viewer->realize();

   // Setup the name for the GraphicsThread
   osgViewer::ViewerBase::Contexts contexts;
   m_viewer->getContexts(contexts);
   for (auto citr = contexts.begin(); citr != contexts.end(); ++citr)
   {
      osg::GraphicsContext* gc = (*citr);
      if (gc->getGraphicsThread())
      {
         thread_utils::SetThreadName(gc->getGraphicsThread()->getThreadId(),
            "osgGraphicsThread");
      }
   }

   // Start the thread used to do OSG rendering
   m_rendering_thread = (HANDLE)_beginthreadex(NULL, 0,
      &GeospatialView::Render, m_viewer, 0, NULL);

   thread_utils::SetThreadName(::GetThreadId(m_rendering_thread),
      "osgRenderThread");
}

/* static */
unsigned GeospatialView::Render(void* ptr)
{
   ::CoInitialize(NULL);

   GeospatialViewer* viewer = reinterpret_cast<GeospatialViewer*>(ptr);
   viewer->run();

   ::CoUninitialize();

   return 0;
}

void GeospatialView::EnableDraw(BOOL enabled)
{
   s_is_rendering_enabled = enabled == TRUE;
   m_viewer->EnableDraw(enabled);
}

fvw::FlightViewManipulator::FlightViewManipulator()
{
   viewer = nullptr;
}

fvw::GroundViewManipulator::GroundViewManipulator()
{
   viewer = nullptr;
   m_max_angle_ground = 15.0;
}

void fvw::FlightViewManipulator::ActivateModeFrom(fvw::CameraMode previous_camera_mode)
{
   PerformChangeToFlightMode change(viewer, this);
   change.WaitForAnimation(0.25);
}

void fvw::FlightViewManipulator::Activate2D()
{
   PerformChangeFromFlightModeTo2D change(viewer, this);
   change.WaitForAnimation(2.0);
}

void fvw::GroundViewManipulator::ActivateModeFrom(fvw::CameraMode previous_camera_mode)
{
   PerformChangeToGroundMode change(viewer, this);
   change.WaitForAnimation(2.0);
}

void fvw::GroundViewManipulator::Activate2D()
{
   PerformChangeFromGroundModeTo2D change(viewer, this);
   change.WaitForAnimation(2.0);
}

fvw::PerformChangeToFlightMode::PerformChangeToFlightMode(
   GeospatialViewer* viewer, FlightViewManipulator* manipulator):
m_viewer(viewer),
m_manipulator(manipulator)
{
}

fvw::PerformChangeFromFlightModeTo2D::PerformChangeFromFlightModeTo2D(
   GeospatialViewer* viewer, FlightViewManipulator* manipulator):
m_viewer(viewer),
m_manipulator(manipulator)
{
}

fvw::PerformChangeToGroundMode::PerformChangeToGroundMode(
   GeospatialViewer* viewer, GroundViewManipulator* manipulator):
m_viewer(viewer),
m_manipulator(manipulator)
{
}

fvw::PerformChangeFromGroundModeTo2D::PerformChangeFromGroundModeTo2D(
   GeospatialViewer* viewer, GroundViewManipulator* manipulator):
m_viewer(viewer),
m_manipulator(manipulator)
{
}

void fvw::PerformChangeFromFlightModeTo2D::AnimationFrame(double t)
{
   FlightInputDeviceStateType* fIDevState = m_viewer->fIDevState;
   osg::ref_ptr<osg::Camera> _camera = m_viewer->getCamera();
   MapView* map_view = fvw_get_view();
   HUD* hud = map_view->GetGeospatialViewController()->GetGeospatialScene()->GetHud();
   long elevation_meters = 0.0;
   fIDevState->throwing = false;

   const osg::EllipsoidModel* ellipsoid_model = fIDevState->GetSRS()->getEllipsoid();
   if (t == 0.0)
   {
      // need heading for this frame before changing it
      hud->m_heading = m_viewer->ComputeHeading();
      double heading = max(min(-hud->m_heading + 360.0, 359.99999), 0.0);
      m_viewer->SetMapParamsRotation(-hud->m_heading + 360.0);
      _camera->getViewMatrixAsLookAt(m_position_old,m_forward_old,m_up_old);

      d_geo_t center = map_view->GetMapProjParams().center;
      double future_elevation = m_viewer->GetDTEDElevation(center.lat, center.lon);
      MapProjectionParams map_proj_params = fvw_get_view()->GetMapProjParams();
      int zoom_percent = map_proj_params.zoom_percent;
      if (zoom_percent == TO_SCALE_ZOOM_PERCENT || 
         zoom_percent == NATIVE_ZOOM_PERCENT)
      {
         zoom_percent = 100;
      }
      double height_natural = camera_utils::ComputeHeightFromMapTypeAndZoom(
         m_viewer->GetState()->GetSRS(), m_viewer->getCamera(), map_view->GetMapType(),
         zoom_percent);
      double altitude = height_natural - future_elevation;
      double altitude_MSL = height_natural;

      ellipsoid_model->convertLatLongHeightToXYZ(DEG_TO_RAD(center.lat), 
         DEG_TO_RAD(center.lon), altitude_MSL, m_position.x(), m_position.y(), 
         m_position.z());
      m_position_normalized.set(m_position_old.x(), m_position_old.y(), m_position_old.z());
      m_position_normalized.normalize();

      osg::Vec3d to_east(north_vector^m_position_normalized);
      to_east.normalize();
      osg::Vec3d to_north(m_position_normalized^to_east);
      to_north.normalize();
      osg::Vec3d right(to_east);
      RotationAroundAxis3D(&m_position_normalized, &right, 
         cos(DEG_TO_RAD(-hud->m_heading)), sin(DEG_TO_RAD(-hud->m_heading)));
      right.normalize();
      m_up.set(m_position_normalized^right);
      m_forward.set(m_position_normalized.operator*(-1.0));

      RotationAroundAxis3D(&right, &m_up, cos(DEG_TO_RAD(0.5)), 
         sin(DEG_TO_RAD(0.5)));
      m_up.normalize();
      RotationAroundAxis3D(&right, &m_forward, cos(DEG_TO_RAD(0.5)), 
         sin(DEG_TO_RAD(0.5)));
      m_forward.normalize();

      // compute altitude, zoom changes if any
      double height_natural_100 = camera_utils::ComputeHeightFromMapTypeAndZoom(
         m_viewer->GetState()->GetSRS(), m_viewer->getCamera(), map_view->GetMapType(),
         100);
      double height_min = camera_utils::ComputeHeightFromMapTypeAndZoom(
         m_viewer->GetState()->GetSRS(), m_viewer->getCamera(), map_view->GetMapType(),
         800);
      double height_max = camera_utils::ComputeHeightFromMapTypeAndZoom(
         m_viewer->GetState()->GetSRS(), m_viewer->getCamera(), map_view->GetMapType(),
         6);
      //double current_altitude = m_viewer->ComputeAltitude();
      if (altitude >= height_min && altitude <= height_max)
      {
         osg::Vec3d delta_altitude(m_position_normalized);
         double elevation = max(altitude_MSL - altitude, 0.0);
         double height_change = future_elevation - elevation;
         delta_altitude.operator*=(height_change + future_elevation);

         m_forward.operator+=(m_position);
         m_position.operator+=(delta_altitude);
         m_forward.operator+=(delta_altitude);

         double zoom = 1.0*(altitude + future_elevation)/height_min;
         zoom = (800.0 / zoom) + 0.8;
         int zoom_int = (int)zoom;
         zoom_int = max(min(zoom_int, 800), 6);
         m_manipulator->m_desired_zoom_percent = zoom_int;
      }
      else
      {
         double elevation = max(altitude - altitude_MSL, 2.0);
         osg::Vec3d delta_altitude(m_position_normalized);
         if (altitude < height_min)
         {
            delta_altitude.operator*=(height_min - altitude);
            m_position.operator+=(delta_altitude);
            m_forward.operator+=(delta_altitude);
            m_manipulator->m_desired_zoom_percent = 800;
         }
         else if (altitude > height_max)
         {
            delta_altitude.operator*=(height_max - altitude);
            m_position.operator+=(delta_altitude);
            m_forward.operator+=(delta_altitude);
            m_manipulator->m_desired_zoom_percent = 6;
         }
      }

      fIDevState->animating = true;
      fIDevState->changeStateRequest = false;
      GetSystemTime(&m_viewer->oldTime);
   }
   else
   {
      if (fIDevState->animating)
      {
         GetSystemTime(&m_viewer->newTime);
      }
      // update variables
      if (t >= 1.0) 
      { //finished
         t = 1.0;
         fIDevState->animating = false;
      }

      //logic
      double altitude_local = m_viewer->ComputeAltitude();
      hud->m_agl_alt_value = altitude_local;
      hud->Update();
      // Interpolate between camera positions as transition occurs to new manipulator
      osg::Vec3d position_interpolation;
      Lerp3D(&position_interpolation,&m_position_old,&m_position,t);
      osg::Vec3d up_interpolation;
      Lerp3D(&up_interpolation,&m_up_old,&m_up,t);
      up_interpolation.normalize();
      osg::Vec3d forwardInterpolation;
      Lerp3D(&forwardInterpolation,&m_forward_old,&m_forward,t);
      osg::Vec3d normalForward(forwardInterpolation-position_interpolation);
      normalForward.normalize();
      normalForward += position_interpolation;
      _camera->setViewMatrixAsLookAt(position_interpolation, normalForward, up_interpolation);
      m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
      OSGUserVars::SetAltitude(m_viewer->ComputeAltitude());
      if (!fIDevState->animating)
      {
         _camera->setViewMatrixAsLookAt(m_position,m_forward,m_up);
         m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
         OSGUserVars::SetAltitude(m_viewer->ComputeAltitude());
         map_view->NotifyTiltUpdate(-89.0);
      }
      m_viewer->requestRedraw();
   }
}

void fvw::PerformChangeFromGroundModeTo2D::AnimationFrame(double t)
{
   FlightInputDeviceStateType* fIDevState = m_viewer->fIDevState;
   osg::ref_ptr<osg::Camera> _camera = m_viewer->getCamera();
   MapView* map_view = fvw_get_view();
   HUD* hud = map_view->GetGeospatialViewController()->GetGeospatialScene()->GetHud();
   long elevation_meters = 0.0;
   fIDevState->throwing = false;

   const osg::EllipsoidModel* ellipsoid_model = fIDevState->GetSRS()->getEllipsoid();
   if (t == 0.0)
   {
      // need heading for this frame before changing it
      hud->m_heading = m_viewer->ComputeHeading();
      double heading = max(min(-hud->m_heading + 360.0, 359.99999), 0.0);
      m_viewer->SetMapParamsRotation(-hud->m_heading + 360.0);
      _camera->getViewMatrixAsLookAt(m_position_old,m_forward_old,m_up_old);

      d_geo_t center = map_view->GetMapProjParams().center;
      double future_elevation = m_viewer->GetDTEDElevation(center.lat, center.lon);
      MapProjectionParams map_proj_params = fvw_get_view()->GetMapProjParams();
      int zoom_percent = map_proj_params.zoom_percent;
      if (zoom_percent == TO_SCALE_ZOOM_PERCENT || 
         zoom_percent == NATIVE_ZOOM_PERCENT)
      {
         zoom_percent = 100;
      }
      double height_natural = camera_utils::ComputeHeightFromMapTypeAndZoom(
            m_viewer->GetState()->GetSRS(), m_viewer->getCamera(), map_view->GetMapType(),
            zoom_percent);
      double altitude = height_natural - future_elevation;
      double altitude_MSL = height_natural;

      ellipsoid_model->convertLatLongHeightToXYZ(DEG_TO_RAD(center.lat), 
         DEG_TO_RAD(center.lon), altitude_MSL, m_position.x(), m_position.y(), 
         m_position.z());
      m_position_normalized.set(m_position.x(), m_position.y(), m_position.z());
      m_position_normalized.normalize();

      osg::Vec3d to_east(north_vector^m_position_normalized);
      to_east.normalize();
      osg::Vec3d to_north(m_position_normalized^to_east);
      to_north.normalize();
      osg::Vec3d right(to_east);
      RotationAroundAxis3D(&m_position_normalized, &right, 
         cos(DEG_TO_RAD(-hud->m_heading)), sin(DEG_TO_RAD(-hud->m_heading)));
      right.normalize();
      m_up.set(m_position_normalized^right);
      m_forward.set(m_position_normalized.operator*(-1.0));

      RotationAroundAxis3D(&right, &m_up, cos(DEG_TO_RAD(0.5)), 
         sin(DEG_TO_RAD(0.5)));
      m_up.normalize();
      RotationAroundAxis3D(&right, &m_forward, cos(DEG_TO_RAD(0.5)), 
         sin(DEG_TO_RAD(0.5)));
      m_forward.normalize();

      // compute altitude, zoom changes if any
      double height_natural_100 = camera_utils::ComputeHeightFromMapTypeAndZoom(
         m_viewer->GetState()->GetSRS(), m_viewer->getCamera(), map_view->GetMapType(),
         100);
      double height_min = camera_utils::ComputeHeightFromMapTypeAndZoom(
         m_viewer->GetState()->GetSRS(), m_viewer->getCamera(), map_view->GetMapType(),
         800);
      double height_max = camera_utils::ComputeHeightFromMapTypeAndZoom(
         m_viewer->GetState()->GetSRS(), m_viewer->getCamera(), map_view->GetMapType(),
         6);
      //double current_altitude = m_viewer->ComputeAltitude();
      if (altitude >= height_min && altitude <= height_max)
      {
         osg::Vec3d delta_altitude(m_position_normalized);
         double elevation = max(altitude_MSL - altitude, 0.0);
         double height_change = future_elevation - elevation;
         delta_altitude.operator*=(height_change + future_elevation);

         m_forward.operator+=(m_position);
         m_position.operator+=(delta_altitude);
         m_forward.operator+=(delta_altitude);

         double zoom = 1.0*(altitude + future_elevation)/height_min;
         zoom = (800.0 / zoom) + 0.8;
         int zoom_int = (int)zoom;
         zoom_int = max(min(zoom_int, 800), 6);
         m_manipulator->m_desired_zoom_percent = zoom_int;
      }
      else
      {
         double elevation = max(altitude - altitude_MSL, 2.0);
         osg::Vec3d delta_altitude(m_position_normalized);
         if (altitude < height_min)
         {
            delta_altitude.operator*=(height_min - altitude);
            m_position.operator+=(delta_altitude);
            m_forward.operator+=(delta_altitude);
            m_manipulator->m_desired_zoom_percent = 800;
         }
         else if (altitude > height_max)
         {
            delta_altitude.operator*=(height_max - altitude);
            m_position.operator+=(delta_altitude);
            m_forward.operator+=(delta_altitude);
            m_manipulator->m_desired_zoom_percent = 6;
         }
      }

      fIDevState->animating = true;
      fIDevState->changeStateRequest = false;
      GetSystemTime(&m_viewer->oldTime);
   }
   else
   {
      if (fIDevState->animating)
      {
         GetSystemTime(&m_viewer->newTime);
      }
      // update variables
      if (t >= 1.0) 
      { //finished
         t = 1.0;
         fIDevState->animating = false;
      }

      //logic
      double altitude_local = m_viewer->ComputeAltitude();
      hud->m_agl_alt_value = altitude_local;
      hud->Update();
      // Interpolate between camera positions as transition occurs to new manipulator
      osg::Vec3d position_interpolation;
      Lerp3D(&position_interpolation,&m_position_old,&m_position,t);
      osg::Vec3d up_interpolation;
      Lerp3D(&up_interpolation,&m_up_old,&m_up,t);
      up_interpolation.normalize();
      osg::Vec3d forwardInterpolation;
      Lerp3D(&forwardInterpolation,&m_forward_old,&m_forward,t);
      osg::Vec3d normalForward(forwardInterpolation-position_interpolation);
      normalForward.normalize();
      normalForward += position_interpolation;
      _camera->setViewMatrixAsLookAt(position_interpolation, normalForward, up_interpolation);
      m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
      OSGUserVars::SetAltitude(m_viewer->ComputeAltitude());
      if (!fIDevState->animating)
      {
         _camera->setViewMatrixAsLookAt(m_position,m_forward,m_up);
         m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
         OSGUserVars::SetAltitude(m_viewer->ComputeAltitude());
         map_view->NotifyTiltUpdate(-89.0);
      }
      m_viewer->requestRedraw();
   }
}

void fvw::PerformChangeToFlightMode::AnimationFrame(double t)
{
   FlightInputDeviceStateType* fIDevState = m_viewer->fIDevState;
   osg::ref_ptr<osg::Camera> _camera = m_viewer->getCamera();
   fIDevState->throwing = false;
   const osg::EllipsoidModel* ellipsoid_model = fIDevState->GetSRS()->getEllipsoid();

   if (t == 0.0)
   {
      // Setup initial state
      fIDevState->animating = true;
      _camera->getViewMatrixAsLookAt(m_position_old,m_forward_old,m_up_old);
      _camera->getViewMatrixAsLookAt(m_position,m_forward,m_up);
      fIDevState->changeStateRequest = false;
      GetSystemTime(&m_viewer->oldTime);
      double height_z = 0.0;
      ellipsoid_model->convertXYZToLatLongHeight(m_position_old.x(), m_position_old.y(), m_position_old.z(),
         m_viewer->new_center_viewer_camera.lat, m_viewer->new_center_viewer_camera.lon, height_z);
   }
   else
   {
      MapView* map_view = fvw_get_view();
      long elevation_meters = 0.0;

      if (fIDevState->animating)
      {
         GetSystemTime(&m_viewer->newTime);
      }
      //update variables
      if (t >= 1.0) 
      { //finished
         fIDevState->animating = false;
      }
      double altitude_local = m_viewer->ComputeAltitude();
      auto hud =
         map_view->GetGeospatialViewController()->GetGeospatialScene()->GetHud();
      hud->m_agl_alt_value = METERS_TO_FEET(altitude_local);
      hud->Update();
      // Interpolate between camera positions as transition occurs to new manipulator
      osg::Vec3d position_interpolation;
      Lerp3D(&position_interpolation,&m_position_old,&m_position,t);
      osg::Vec3d up_interpolation;
      Lerp3D(&up_interpolation,&m_up_old,&m_up,t);
      osg::Vec3d forwardInterpolation;
      Lerp3D(&forwardInterpolation,&m_forward_old,&m_forward,t);
      osg::Vec3d normalForward(forwardInterpolation-position_interpolation);
      normalForward.normalize();
      normalForward += position_interpolation;
      _camera->setViewMatrixAsLookAt(position_interpolation, normalForward, up_interpolation);
      OSGUserVars::SetAltitude(m_viewer->ComputeAltitude());
      if (fIDevState->GetActiveCam() != fvw::CAMERA_MODE_GROUND_VIEW)
      {
         m_viewer->requestRedraw();
      }

      if (!fIDevState->animating)
      {
         m_viewer->UpdateCenter(m_viewer->new_center_viewer_camera, fIDevState->GetActiveCam());
         _camera->setViewMatrixAsLookAt(m_position,m_forward,m_up);
         m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
         m_viewer->setCameraManipulator(m_manipulator, false);
         OSGUserVars::SetAltitude(m_viewer->ComputeAltitude());

         HudProperties hud_properties = hud->GetHudProperties();
         if (!hud_properties.hud_on)
         {
            hud_properties.hud_on = true;
         }
         hud->SetHudProperties(hud_properties);

         m_viewer->requestRedraw();
      }
   }
}

void fvw::PerformChangeToGroundMode::AnimationFrame(double t)
{
   FlightInputDeviceStateType* fIDevState = m_viewer->fIDevState;
   osg::ref_ptr<osg::Camera> _camera = m_viewer->getCamera();
   MapView* map_view = fvw_get_view();
   long elevation_meters = 0.0;
   fIDevState->throwing = false;
   const osg::EllipsoidModel* ellipsoid_model = fIDevState->GetSRS()->getEllipsoid();
   if (t == 0.0)
   {
      _camera->getViewMatrixAsLookAt(m_position_old,m_forward_old,m_up_old);
       fIDevState->throwing = false;
      _camera->getViewMatrixAsLookAt(m_position,m_forward,m_up);
      osg::Vec3d positionN;
      positionN.set(m_position.x(), m_position.y(), m_position.z());
      positionN.normalize();
      osg::Vec3d forwardN;
      forwardN.set(m_forward-m_position);
      forwardN.normalize();

      if (fIDevState->GetLastCamMode() == fvw::CAMERA_MODE_PAN)
      {
         double focus_angle;
         map_view->GetGeospatialViewController()->GetViewpoint3D(
            &m_viewer->m_last_pan_viewpoint);
         osgUtil::LineSegmentIntersector::Intersections mid_screen;
         m_viewer->computeIntersections(_camera->getViewport()->width()/2.0,
            _camera->getViewport()->height()/2.0, mid_screen);
         double height_1 = -10000.0;
         if (!mid_screen.empty())
         {
            int firstOnly = 0;
            for (osgUtil::LineSegmentIntersector::Intersections::const_iterator iter = mid_screen.begin();
               firstOnly < 1;
               firstOnly++)
            {
               osg::Vec3d loc_world;
               loc_world.set(iter->getWorldIntersectPoint());
               positionN.set(loc_world);
               positionN.normalize();
               if (forwardN*positionN < 0.0)
               {
                  focus_angle = 0.0;
                  ellipsoid_model->convertXYZToLatLongHeight(loc_world.x(), loc_world.y(), loc_world.z(),
                     m_viewer->new_center_viewer_camera.lat, m_viewer->new_center_viewer_camera.lon, height_1);
               }
               else
               {
                  m_viewer->new_center_viewer_camera.lat = DEG_TO_RAD(m_viewer->m_last_pan_viewpoint.y);
                  m_viewer->new_center_viewer_camera.lon = DEG_TO_RAD(m_viewer->m_last_pan_viewpoint.x);
               }
            }
         }
         else
         {
            m_viewer->new_center_viewer_camera.lat = DEG_TO_RAD(m_viewer->m_last_pan_viewpoint.y);
            m_viewer->new_center_viewer_camera.lon = DEG_TO_RAD(m_viewer->m_last_pan_viewpoint.x);
         }

         OSGUserVars::SetCameraLatLon(osg::RadiansToDegrees(m_viewer->new_center_viewer_camera.lon), 
            osg::RadiansToDegrees(m_viewer->new_center_viewer_camera.lat));
         short elevation = 0;
         long val = m_viewer->GetDTEDElevation(
            osg::RadiansToDegrees(m_viewer->new_center_viewer_camera.lat),
            osg::RadiansToDegrees(m_viewer->new_center_viewer_camera.lon));
         if (val == MISSING_DTED_ELEVATION)
         {
            val = 0.0;
         }
         val = max(val, height_1);

         // @TODO:
         // double tile subdivison using high precision goes here
         // intersection might not equal final LOD. Also need to take into account DBDB.
         // {
         // }

         ellipsoid_model->convertLatLongHeightToXYZ(m_viewer->new_center_viewer_camera.lat, 
            m_viewer->new_center_viewer_camera.lon, val + m_viewer->height_above_ground, m_position.x(), 
            m_position.y(), m_position.z());
         //+10 to ensure long enough vector to get proper normal on ellipsoid
         osg::Vec3d position2;
         ellipsoid_model->convertLatLongHeightToXYZ(m_viewer->new_center_viewer_camera.lat, 
            m_viewer->new_center_viewer_camera.lon, val + m_viewer->height_above_ground + 10.0, position2.x(), 
            position2.y(), position2.z());
         // compute normal on ellipsoid
         osg::Vec3d tempUp(position2-m_position);
         tempUp.normalize();
         double heading = -m_viewer->ComputeHeading();
         osg::Vec3d right = north_vector^tempUp;
         right.normalize();
         // tempForward is now north oriented, apply heading
         RotationAroundAxis3D(&tempUp, &right, cos(DEG_TO_RAD(heading)),
            sin(DEG_TO_RAD(heading)));
         right.normalize();
         osg::Vec3d tempForward(tempUp^right);
         tempForward.normalize();
         // heading now applied, now to roll camera 3 degrees below horizon 
         // to ensure that tiles load
         RotationAroundAxis3D(&right, &tempUp, cos(DEG_TO_RAD(-3.0)),sin(DEG_TO_RAD(-3.0)));
         m_up.set(tempUp);
         m_up.normalize();

         position2.normalize();
         double testing1 = tempUp*position2;
         m_forward.set(m_up^right);

         m_forward.normalize();
         m_forward.operator+=(m_position);
         fIDevState->animating = true;
      }
      else if (fIDevState->GetLastCamMode() == fvw::CAMERA_MODE_FLIGHT)
      {
         OSGUserVars::SetCameraHeight(0, 0);
         fIDevState->animating = true;
         _camera->getViewMatrixAsLookAt(m_position_old,m_forward_old,m_up_old);

         osg::Vec3d tempUp(m_position.x(),m_position.y(),m_position.z());
         tempUp.normalize();
         osg::Vec3d right;
         osg::Vec3d tempForward(m_forward-m_position);
         tempForward.normalize();

         short elevation = 0;
         OSGUserVars::SetCameraLatLon(osg::RadiansToDegrees(m_viewer->new_center_viewer_camera.lon), 
            osg::RadiansToDegrees(m_viewer->new_center_viewer_camera.lat));
         long val = m_viewer->GetDTEDElevation(
            osg::RadiansToDegrees(m_viewer->new_center_viewer_camera.lat),
            osg::RadiansToDegrees(m_viewer->new_center_viewer_camera.lon));
         ellipsoid_model->convertLatLongHeightToXYZ(m_viewer->new_center_viewer_camera.lat, 
            m_viewer->new_center_viewer_camera.lon, val + m_viewer->height_above_ground + 1.0, m_position.x(),
         m_position.y(), m_position.z());
         positionN.set(m_position);
         positionN.normalize();
         //position->set(*position + positionN);
         m_up.set(positionN);
         right = north_vector^positionN;
         right.normalize();

         double heading = -m_viewer->ComputeHeading();
         RotationAroundAxis3D(&m_up, &right, cos(DEG_TO_RAD(heading)),sin(DEG_TO_RAD(heading)));
         RotationAroundAxis3D(&right, &m_up, cos(DEG_TO_RAD(-3.0)),sin(DEG_TO_RAD(-3.0)));
         osg::Vec3d tempForward2(m_up^right);
         m_forward.set(m_position + tempForward2);
      }
      fIDevState->changeStateRequest = false;
      GetSystemTime(&m_viewer->oldTime);
   }
   else
   {
      if (fIDevState->animating)
      {
         osg::Vec3d positionInterpolation;
         Lerp3D(&positionInterpolation,&m_position_old,&m_position,t);
         osg::Vec3d upInterpolation;
         Lerp3D(&upInterpolation,&m_up_old,&m_up,t);
         osg::Vec3d forwardInterpolation;
         Lerp3D(&forwardInterpolation,&m_forward_old,&m_forward,t);
         osg::Vec3d normalForward(forwardInterpolation-positionInterpolation);
         normalForward.normalize();
         normalForward += positionInterpolation;
         _camera->setViewMatrixAsLookAt(positionInterpolation, normalForward, upInterpolation);
         OSGUserVars::SetAltitude(m_viewer->ComputeAltitude());
         GetSystemTime(&m_viewer->newTime);
      }
      if (t >= 1.0) 
      { //finished
         fIDevState->animating = false;
      }
      double altitude_local = m_viewer->ComputeAltitude();
      double altitude_MSL = m_viewer->ComputeAltitudeMSL();
      auto hud =
         map_view->GetGeospatialViewController()->GetGeospatialScene()->GetHud();
      hud->m_agl_alt_value = METERS_TO_FEET(altitude_local);
      hud->m_msl_alt_value = METERS_TO_FEET(altitude_MSL);
      hud->Update();
      if (!fIDevState->animating)
      {
         double height_z = 0.0;
         hud->m_agl_alt_value = METERS_TO_FEET(2.0);
         ellipsoid_model->convertXYZToLatLongHeight(m_position.x(), m_position.y(), m_position.z(),
            m_viewer->new_center_viewer_camera.lat, m_viewer->new_center_viewer_camera.lon, height_z);
         m_viewer->UpdateCenter(m_viewer->new_center_viewer_camera, fIDevState->GetActiveCam());
         _camera->setViewMatrixAsLookAt(m_position,m_forward,m_up);
         m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
         OSGUserVars::SetAltitude(m_viewer->ComputeAltitude());
         double heading = m_viewer->ComputeHeading();
         m_viewer->setCameraManipulator(m_manipulator, false);
         m_manipulator->setByMatrix(_camera->getInverseViewMatrix());
         double height_y;
         ellipsoid_model->convertXYZToLatLongHeight(m_position.x(), m_position.y(), m_position.z(),
         m_viewer->new_center_viewer_camera.lat, m_viewer->new_center_viewer_camera.lon, height_y);
         m_viewer->new_center_request = true;
         m_viewer->renderingTraversals();
         map_view->NotifyTiltUpdate(-54);
      }
   }
}

