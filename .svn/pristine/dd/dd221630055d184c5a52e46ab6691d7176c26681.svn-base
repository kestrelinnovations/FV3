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

// GeoInputDevice.cpp

#include "stdafx.h"
#include "FalconView/GeoInputDevice.h"
#include "GeospatialViewController.h"
#include "RenderFunctions.h"

#include "osg/PositionAttitudeTransform"

#include "Common/SafeArray.h"
#include "FalconView/CameraUtils.h"
#include "FalconView/GeospatialView.h"
#include "FalconView/getobjpr.h"
#include "FalconView/gfx_debug_utils.h"
#include "FalconView/include/err.h"
#include "FvCore/Include/TimeUtils.h"
#include "Common/ScopedCriticalSection.h"

//temporary below
#include "OSGVariables.h"
#include "resource.h" // for temporary handling of key commands

using namespace time_utils;

class HUD;

typedef SafeArray<MapScaleUnitsEnum, VT_UI4> ScaleUnitsSafeArray;

FlightInputDeviceStateType::FlightInputDeviceStateType() : 
   changeStateRequest(false), animating(false), m_cameraMode(fvw::CAMERA_MODE_PAN),
      mouseX(0), mouseY(0), throwStart(false), throwEnd(false), throwing(false), deltaTimeThrow(0.0), panning(0),
      spacebarPressedStart(false), spacebarPressedEnd(false), leftKeyDown(false), rightKeyDown(false),
      upKeyDown(false), downKeyDown(false), arrowKeyDownStart(false), complexArrowKeyThrow(false), 
      allowNewArrowKeyInput(true), m_geo_viewer(nullptr), pressed(false), right_pressed(false), debugCount(0),
      gvc(nullptr), north(false), east(false), south(false), west(false),
      moveUp(false), moveDown(false), move_speed(1.0), move_speed_vertical(1.0), rollLeft(false),
      rollRight(false), roll_speed(1.0), yaw_speed(1.0), pitch_speed(1.0), m_old_cameraMode(fvw::CAMERA_MODE_NONE),
      insertKeyDown(false), deleteKeyDown(false), shift(false), control(false), space(false), 
      num_0(false), num_1(false), num_2(false), num_3(false), num_4(false), num_5(false), num_6(false), 
      num_7(false), num_8(false), num_9(false), m_enable_flight_mode_auto_map_type(false),
      mouse_wheel_change_value(0.0), plus_key(false), minus_key(false), control_freeze(false), 
      velocity_freeze(false), north_orient(false), top_down_view(false), direct_down(false), mouse_moved_pressed(false),
      frame_update_wait(true), last_frame_movement(true), leftButtonDown(false), rightButtonDown(false),
      upButtonDown(false), downButtonDown(false), rotCcwButtonDown(false), rotCwButtonDown(false), northUpButtonDown(false),
      transition_animation(false), tilt_value(-89), handled_shift(false), handled_tilt(true), mouseXS(0), mouseYS(0), 
      mouseXF(0), mouseYF(0), oldMouseX(0), oldMouseY(0), first_run(true), H_last_frame(false), W_last_frame(false), 
      handled_key_W(false), handled_key_S(false), handled_key_A(false), handled_key_D(false), handled_key_L(false), 
      render_as_wireframe(false), escape(false)
   {

   }

bool FlightInputDeviceStateType::SetCameraMode(fvw::CameraMode camera_mode)
{
   // If the camera mode is the same there is nothing to do
   if (GetActiveCam() == camera_mode && !first_run)
   {
      if (GetActiveCam() != fvw::CAMERA_MODE_NONE) // specific 2D transition
         return false;
   }

   // Do not modify camera mode while in the middle of an animation
   if (animating)
      return false;

   changeStateRequest = true;
   OSGUserVars::SetGroundState(camera_mode == fvw::CAMERA_MODE_GROUND_VIEW);
   OSGUserVars::SetFlightState(camera_mode == fvw::CAMERA_MODE_FLIGHT);
   OSGUserVars::SetPanState(camera_mode == fvw::CAMERA_MODE_PAN);

   frame_update_wait = true;

   if (!first_run || camera_mode != fvw::CAMERA_MODE_PAN)
   {
      SetActiveCam(camera_mode);
   }
   first_run = false;

   // Handle transitions from other 3D modes or transitioning from 2D
   if (camera_mode == fvw::CAMERA_MODE_PAN)
      m_geo_viewer->parentView->m_camera_earth_manipulator->ActivateModeFrom(GetLastCamMode());
   else if (camera_mode == fvw::CAMERA_MODE_FLIGHT)
      m_geo_viewer->parentView->m_camera_flight_manipulator->ActivateModeFrom(GetLastCamMode());
   else if (camera_mode == fvw::CAMERA_MODE_GROUND_VIEW)
      m_geo_viewer->parentView->m_camera_walk_manipulator->ActivateModeFrom(GetLastCamMode());

   // Else, transition from current manipulator to 2D
   else if (camera_mode == fvw::CAMERA_MODE_NONE)
   {
      if (GetLastCamMode() == fvw::CAMERA_MODE_PAN)
         m_geo_viewer->parentView->m_camera_earth_manipulator->Activate2D();
      else if (GetLastCamMode() == fvw::CAMERA_MODE_FLIGHT)
         m_geo_viewer->parentView->m_camera_flight_manipulator->Activate2D();
      else if (GetLastCamMode() == fvw::CAMERA_MODE_GROUND_VIEW)
         m_geo_viewer->parentView->m_camera_walk_manipulator->Activate2D();
   }

   // Force the enable-flight-mode-auto-map-type flag to be false when first
   // entering (or leaving) flight mode
   m_enable_flight_mode_auto_map_type = false;
   UpdateFlightModeAutoMapTypeTimer();
   return true;
}

void FlightInputDeviceStateType::SetActiveCam(fvw::CameraMode mode) 
{
   fv_common::ScopedCriticalSection lock(&fvw_get_view()->
      GetGeospatialViewController()->m_viewpoint_critical_section);
   m_old_cameraMode = m_cameraMode;
   m_cameraMode = mode;
}

fvw::CameraMode FlightInputDeviceStateType::GetActiveCam() const{
   fv_common::ScopedCriticalSection lock(&fvw_get_view()->
      GetGeospatialViewController()->m_viewpoint_critical_section);
   return m_cameraMode;
}
fvw::CameraMode FlightInputDeviceStateType::GetLastCamMode() const{
   fv_common::ScopedCriticalSection lock(&fvw_get_view()->
      GetGeospatialViewController()->m_viewpoint_critical_section);
   return m_old_cameraMode;
}

void FlightInputDeviceStateType::ToggleEnableFlightModeAutoMapType()
{
   m_enable_flight_mode_auto_map_type = !m_enable_flight_mode_auto_map_type;
   UpdateFlightModeAutoMapTypeTimer();
}

void FlightInputDeviceStateType::UpdateFlightModeAutoMapTypeTimer()
{
   // Flight mode can update the MapType automatically as the altitude of the
   // camera is changed. The computation used to determine the best map type
   // for the given altitude is fairly expensive. Because of this, we only want
   // to do so periodically.
   if (m_cameraMode == fvw::CAMERA_MODE_FLIGHT &&
      m_enable_flight_mode_auto_map_type)
   {
      // We assume that SetCameraMode is only called from the main thread
      UTL_get_timer_list()->add(&m_flight_mode_auto_map_type_timer);
   }
   else
   {
      // Remove the flight mode auto MapType timer, if necessary
      POSITION pos = UTL_get_timer_list()->Find
         (&m_flight_mode_auto_map_type_timer);
      if (pos)
         UTL_get_timer_list()->remove_at(pos);
   }
}

void FlightInputDeviceStateType::ResetKeyboard()
{
   north = false; 
   east = false;
   south = false;
   west = false;
   moveUp = false;
   moveDown = false;
   rollLeft = false;
   rollRight = false;
   insertKeyDown = false;
   deleteKeyDown = false;
   leftKeyDown = false;
   rightKeyDown = false;
   upKeyDown = false;
   downKeyDown = false;

   space = false;
   shift = false;
   control = false;
   plus_key = false;
   minus_key = false;
   moveUp = false;
   moveDown = false;
   handled_key_L = false;
   handled_key_W = false;
   handled_key_S = false;
   handled_key_A = false;
   handled_key_D = false;
   handled_shift = false;
   handled_tilt = true;
   num_0 = false;
   num_1 = false;
   num_2 = false;
   num_3 = false;
   num_4 = false;
   num_5 = false;
   num_6 = false;
   num_7 = false;
   num_8 = false;
   num_9 = false;
   insertKeyDown = false;
   deleteKeyDown = false;
   mouse_moved_pressed = false;
   top_down_view = false;
   direct_down = false;
   escape = false;
   // warning: Do not set last_frame_movement even though it is a 
   // member of FlightInputDeviceStateType here. It is reset outside
   // of this function
}

void FlightInputDeviceStateType::UpdateKeyboardState()
{
    // handle logic that can't originate off render thread
    handled_key_L = false;
    handled_key_W = false;
    handled_key_S = false;
    handled_key_A = false;
    handled_key_D = false;

    // left mouse button up check
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
    {
      // handles the case when the tilt slider click
      // is released off the button. This must be
      // handled outside the slider class
      if (handled_tilt)
      {
         if (handled_shift)
         {
            handled_shift = false;
            shift = false;
         }
      }
    }

    if (GetAsyncKeyState(0x30) < 0) // '0' key
    {
      // Spit out current scene graph to a .dot file
      char filename[MAX_PATH];
      ::GetTempPath(MAX_PATH, filename);
      ::PathAppend(filename, "fv_scene_graph.dot");
      gfx_debug_utils::SceneGraphToDot(m_geo_viewer->getSceneData(), filename);
    }

    if (GetAsyncKeyState(VK_SHIFT) >= 0)
       last_frame_movement = true;

    if (GetAsyncKeyState(VK_SPACE) < 0)
        space = true;
    else
        space = false;

    if (GetAsyncKeyState(VK_ESCAPE) < 0)
    {
       if (!escape)
       {
          if (fvw_get_view()->m_hand_mode)
          {
            throwing = false;
            fvw_get_view()->m_hand_mode = FALSE;
            fvw_get_view()->GetGeospatialViewController()->SetIsPanning(FALSE);
          }
       }
       escape = true;
    }
    else
    {
       escape = false;
    }

    if (GetAsyncKeyState(VK_SHIFT) < 0 || handled_shift)
    {
        shift = true;
    }
    else
    {
        shift = false;
    }

    if (GetAsyncKeyState(VK_CONTROL) < 0)
        control = true;
    else
        control = false;

    if (GetAsyncKeyState(VK_OEM_PLUS) < 0)
        plus_key = true;
    else
        plus_key = false;

    if (GetAsyncKeyState(VK_OEM_MINUS) < 0)
        minus_key = true;
    else
        minus_key = false;

    if ((GetAsyncKeyState('L') < 0) && (GetAsyncKeyState(VK_CONTROL) >= 0))
    {
        handled_key_L = true;
        num_5 = true;
    }
    else
    {
        handled_key_L = false;
        num_5 = false;
    }

    if ((GetAsyncKeyState('A') < 0) || (GetAsyncKeyState('U') < 0))
    {
        if (shift && !control)
            moveUp = true;
        if (!shift || control)
            moveUp = false;
    }
    else
    {
        moveUp = false;
    }

    if ((GetAsyncKeyState('A') < 0))
    {
        if (!shift && !control)
        {
            handled_key_A = true;
            if (!leftKeyDown)
               arrowKeyDownStart = true;
            leftKeyDown = true;
        }
        else
            leftKeyDown = false;
    }
    else
    {
         if (!(GetAsyncKeyState(VK_LEFT) < 0))
         {
            leftKeyDown = false;
         }
    }

    if ((GetAsyncKeyState('D') < 0))
    {
        if (shift && !control)
            moveDown = true;
        if (!shift || control)
            moveDown = false;
    }
    else
    {
        moveDown = false;
    }

    if ((GetAsyncKeyState('D') < 0))
    {
        if (!shift && !control)
        {
            handled_key_D = true;
            if (!rightKeyDown)
               arrowKeyDownStart = true;
            rightKeyDown = true;
        }
        else
            rightKeyDown = false;
    }
    else
    {
         if (!(GetAsyncKeyState(VK_RIGHT) < 0))
         {
            rightKeyDown = false;
         }
    }

    if ((GetAsyncKeyState('H') < 0))
    {
       if (!H_last_frame)
       {
          auto hud = fvw_get_view()->GetGeospatialViewController()->
             GetGeospatialScene()->m_HUD;
          if (hud)
          {
             HudProperties hud_properties = hud->GetHudProperties();
             hud_properties.hud_on = !hud_properties.hud_on;
             hud->SetHudProperties(hud_properties);
             m_geo_viewer->requestRedraw();
          }
       }
       H_last_frame = true;
    }
    else
    {
        H_last_frame = false;
    }

    if (GetAsyncKeyState('G') < 0)
    {
        if (!shift && !control)
            direct_down = true;
        else
            direct_down = false;
    }
    else
    {
        direct_down = false;
    }

    if ((GetAsyncKeyState('U') < 0))
    {
        if (!shift && !control)
            top_down_view = true;
        else
            top_down_view = false;
    }
    else
    {
        top_down_view = false;
    }

    if (GetAsyncKeyState('N') < 0)
    {
        if (!shift && !control)
            north_orient = true;
        else
            north_orient = false;
    }
    else
    {
        north_orient = false;
    }

    if (GetAsyncKeyState('N') < 0)
    {
        if (shift && !control)
            north = true;
        if (!shift || control)
            north = false;
    }
    else
    {
        north = false;
    }

    // this 'R' check must come after 'N', 'U' key checks
    if (GetAsyncKeyState('R') < 0)
    {
        if (!shift && !control)
        {
            north_orient = true;
            top_down_view = true;
        }
        else
        {
            north_orient = false;
            top_down_view = false;
        }
    }

    if (GetAsyncKeyState('S') < 0)
    {
        if (shift && !control)
        {
            south = true;
        }
        if (!control && !shift)
        {
            handled_key_S = true;
            if (!downKeyDown)
               arrowKeyDownStart = true;
            downKeyDown = true;
        }
        if (!shift || control)
        {
            south = false;
        }
    }
    else
    {
        if (!(GetAsyncKeyState(VK_DOWN) < 0))
        {
           downKeyDown = false;
        }
        south = false;
    }

    if (GetAsyncKeyState('E') < 0)
    {
        if (shift && !control)
            east = true;
        if (!shift || control)
            east = false;
    }
    else
    {
        east = false;
    }

    if (GetAsyncKeyState('W'))
    {
        if (!W_last_frame)
        {
            if (control && shift)
            {
                render_as_wireframe = !render_as_wireframe;
                gfx_debug_utils::RenderNodeAsWireframe(m_geo_viewer->getSceneData(),
                    render_as_wireframe);
                m_geo_viewer->requestRedraw();
            }
        }
        W_last_frame = true;
    }
    else
    {
        W_last_frame = false;
    }

    if (GetAsyncKeyState('W'))
    {
        if (shift && !control)
        {
            west = true;
        }
        if (!shift && !control)
        {
            handled_key_W = true;
            if (!upKeyDown)
               arrowKeyDownStart = true;
            upKeyDown = true;
        }
        if (!shift || control)
        {
            west = false;
        }
    }
    else
    {
         if (!(GetAsyncKeyState(VK_UP) < 0))
         {
            upKeyDown = false;
         }
         west = false;
    }
    
    if (GetAsyncKeyState(VK_NUMPAD0) < 0)
        num_0 = true;
    else
        num_0 = false;
    
    if (GetAsyncKeyState(VK_NUMPAD1) < 0)
        num_1 = true;
    else
        num_1 = false;
    
    if (GetAsyncKeyState(VK_NUMPAD2) < 0)
        num_2 = true;
    else
        num_2 = false;
    
    if (GetAsyncKeyState(VK_NUMPAD3) < 0)
        num_3 = true;
    else
        num_3 = false;

    if (GetAsyncKeyState(VK_NUMPAD4) < 0)
        num_4 = true;
    else
        num_4 = false;
    
    if (GetAsyncKeyState(VK_NUMPAD5) < 0)
        num_5 = true;
    else if (!handled_key_L)
        num_5 = false;
    
    if (GetAsyncKeyState(VK_NUMPAD6) < 0)
        num_6 = true;
    else
        num_6 = false;
    
    if (GetAsyncKeyState(VK_NUMPAD7) < 0)
        num_7 = true;
    else
        num_7 = false;

    if (GetAsyncKeyState(VK_NUMPAD8) < 0)
        num_8 = true;
    else
        num_8 = false;

    if (GetAsyncKeyState(VK_NUMPAD9) < 0)
        num_9 = true;
    else
        num_9 = false;

    if (GetAsyncKeyState(VK_UP) < 0)
    {
        if (!upKeyDown)
           arrowKeyDownStart = true;
        upKeyDown = true;
    }
    else if (!handled_key_W)
    {
        upKeyDown = false;
    }

    if (GetAsyncKeyState(VK_DOWN) < 0)
    {
       if (!downKeyDown)
           arrowKeyDownStart = true;
        downKeyDown = true;
    }
    else if (!handled_key_S)
    {
        downKeyDown = false;
    }

    if (GetAsyncKeyState(VK_LEFT) < 0)
    {
       if (!leftKeyDown)
           arrowKeyDownStart = true;
        leftKeyDown = true;
    }
    else if (!handled_key_A)
    {
        leftKeyDown = false;
    }

    if (GetAsyncKeyState(VK_RIGHT) < 0)
    {
       if (!rightKeyDown)
           arrowKeyDownStart = true;
        rightKeyDown = true;
    }
    else if (!handled_key_D)
    {
        rightKeyDown = false;
    }

    if (GetAsyncKeyState(VK_INSERT) < 0)
        insertKeyDown = true;
    else
        insertKeyDown = false;

    if (GetAsyncKeyState(VK_DELETE) < 0)
        deleteKeyDown = true;
    else
        deleteKeyDown = false;

    if (control && shift && (GetAsyncKeyState('F') & 0x7FFF))
    {
        control_freeze = !control_freeze;
    }

    if ((GetAsyncKeyState('V') & 0x7FFF))
    {
        velocity_freeze = !velocity_freeze;
    }
}

bool FlightInputDeviceStateType::OnMouseLeftPressed(UINT nFlags, CPoint point)
{
   /* Call originates in MapView and not osgGA::GUIEventHandler */
   if (GetActiveCam() == fvw::CAMERA_MODE_PAN) 
   {
      if (throwing)
      {
         // If any throw is currently in progress
         throwing = false;
         complexArrowKeyThrow = false;
         rightKeyDown = false;
         leftKeyDown = false;
         upKeyDown = false;
         downKeyDown = false;
         allowNewArrowKeyInput = true;
      }
      throwStart = true;  
      pressed = true;
      GetSystemTime(&eventStart);
      mouseXS = point.x; mouseYS = point.y;
      return false;
   }
   if (GetActiveCam() == fvw::CAMERA_MODE_FLIGHT) 
   {
      mouseXS = point.x; mouseYS = point.y;
      pressed = true;
   }
   if (GetActiveCam() == fvw::CAMERA_MODE_GROUND_VIEW) 
   {
      mouseXS = point.x; mouseYS = point.y;
      pressed = true;
   }
   return false;
}

bool FlightInputDeviceStateType::OnLeftMouseRelease(UINT nFlags, CPoint point)
{
   if (GetActiveCam() == fvw::CAMERA_MODE_GROUND_VIEW)
   {
      mouseXF = point.x; mouseYF = point.y;
      if ((mouseXS != mouseXF || mouseYS != mouseYF) && pressed)
      {  // force one more HUD update. Viewer will handle setting flag to false
         //mouse_moved_pressed = true;
         frame_update_wait = true;
         //m_geo_viewer->requestRedraw();
      }
      pressed = false;
      return false;
   }
      
   else if (GetActiveCam() == fvw::CAMERA_MODE_FLIGHT)
   {
      mouseXF = point.x; mouseYF = point.y;
      if ((mouseXS != mouseXF || mouseYS != mouseYF) && pressed)
      {  // force one more HUD update. Viewer will handle setting flag to false
         //mouse_moved_pressed = true;
         frame_update_wait = true;
         //m_geo_viewer->requestRedraw();
      }
      pressed = false;
      return false;
   }

   else if (GetActiveCam() == fvw::CAMERA_MODE_PAN)
   {
      if (pressed) // Only true if left mouse button initiated a PUSH event earlier in PAN
      {
         pressed = false;
         GetSystemTime(&eventEnd);
         mouseXF = point.x; mouseYF = point.y;
         GetSystemTime(&noMoveEnd);

         // The mouse must have moved in the past 0.10 seconds to be a possible throw
         // also modifiers like shift, control must not be down
         if ((DeltaTimeSec(noMoveStart, noMoveEnd)) < 0.10)
         {
            deltaTimeThrow = DeltaTimeSec(eventStart, eventEnd);
            if (deltaTimeThrow > 0.01 && deltaTimeThrow < 0.4 &&
               (mouseXS != mouseXF || mouseYS != mouseYF) && !shift && !control)
            {
               throwEnd = true;
            }
         }
      }
   }
   return false;
}

bool FlightInputDeviceStateType::OnMouseMoved(UINT nFlags, CPoint point)
{
    mouseX = point.x;
    mouseY = point.y;

    if (m_geo_viewer == nullptr) 
        return false;
    if (animating)
        return true;
    if (GetActiveCam() == fvw::CAMERA_MODE_FLIGHT)
        return true;

    // Mouse is currently being dragged
    if (pressed)
    {
        GetSystemTime(&noMoveStart);
        oldMouseX = point.x;
        oldMouseX = point.y;
        mouse_moved_pressed = true;
    }
    else
    {
        mouse_moved_pressed = false;
    }
    if (right_pressed)
    {
       mouse_moved_pressed = true;
    }
    return false;
}

void FlightInputDeviceStateType::FlightModeAutoMapTypeTimer::expired()
{
   MapView* map_view = fvw_get_view();
   ASSERT(map_view);  // MapView should be created if the camera mode changed

   // If the change in altitude is not more than 1% different than the last
   // computed value, then do not recompute the MapType
   static double prev_camera_alt_meters = 0.0;
   if (m_camera_alt_meters <= prev_camera_alt_meters*1.01 &&
      m_camera_alt_meters >= prev_camera_alt_meters*0.99)
   {
      start();
      return;
   }

   prev_camera_alt_meters= m_camera_alt_meters;

   // Compute the closest map type with available data
   //
   long group_id = map_view->GetMapGroupIdentity();

   _variant_t pProduct, pScale, pScaleUnits, pSeries;
   try
   {
      static IMapRenderingEnginePtr s_mre;
      static IMapRenderingEngineDataCheckPtr s_data_check;
      if (s_mre == NULL)
      {
         CO_CREATE(s_mre, CLSID_MapRenderingEngine);
         s_mre->Init(nullptr);
         s_mre->m_allowInvalidPointsOnSurface = VARIANT_TRUE;
         s_data_check = s_mre;
      }

      const MapDisplayParams& disp_params = map_view->GetMapDisplayParams();

      s_mre->SetSurfaceDimensions(disp_params.surface_width,
         disp_params.surface_height, SURFACE_UNITS_PIXELS);

      s_data_check->GetAvailableMapSeriesInGroup(group_id,
         m_camera_location.lat, m_camera_location.lon, 0.0, 100,
         ORTHOGRAPHIC_PROJECTION, 0, &pProduct, &pScale, &pScaleUnits,
         &pSeries);
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }

   BstrSafeArray saProduct;
   DoubleSafeArray saScale;
   ScaleUnitsSafeArray saScaleUnits;
   BstrSafeArray saSeries;

   saProduct.Attach(pProduct.parray);
   saScale.Attach(pScale.parray);
   saScaleUnits.Attach(pScaleUnits.parray);
   saSeries.Attach(pSeries.parray);

   // Compute the ideal degrees per pixel for the current camera height
   auto viewer = fvw_get_view()->GetGeospatialViewController()->
      GetGeospatialView()->Viewer();
   double target_dpp = camera_utils::ComputeDegreesPerPixel(
      viewer->GetState()->GetSRS(), viewer->getCamera(), m_camera_alt_meters);

   // For each map type with available data, determine the closest MapType
   // to the target degrees per pixel
   MapType closest_map_type;
   double closest_dpp = DBL_MAX;
   const int num_map_types = saProduct.GetNumElements();
   for (int i=0; i<num_map_types; ++i)
   {
      MapType map_type(MapSource((char *)_bstr_t(saProduct[i])),
         MapScale(saScale[i], saScaleUnits[i]),
         MapSeries((char *)_bstr_t(saSeries[i])));

      double dpp = map_type.get_scale().get_nominal_degrees_lat_per_pixel();
      if (fabs(dpp - target_dpp) < closest_dpp)
      {
         closest_dpp = dpp;
         closest_map_type = map_type;
      }
   }

   saProduct.Detach();
   saScale.Detach();
   saScaleUnits.Detach();
   saSeries.Detach();

   if (num_map_types > 0)
      map_view->ChangeMapType(group_id, closest_map_type);

   start();
}

bool GeoKeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea,
   osgGA::GUIActionAdapter& aa)
{
        FlightInputDeviceStateType* fids = this->m_flightInputDeviceState;
    UINT nChar = 0U;
    int key = 0;;
    switch (ea.getEventType()) //            osgGA::GUIEventAdapter::DRAG
    {
        case osgGA::GUIEventAdapter::MouseButtonMask::RIGHT_MOUSE_BUTTON:
        {
            break;
        }
        case osgGA::GUIEventAdapter::DRAG:
        {
            if (ea.getButtonMask() == 
                osgGA::GUIEventAdapter::MouseButtonMask::RIGHT_MOUSE_BUTTON)
            {

            }
            break;
        }
        default:
            return false;
   }
   return false;
}
