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

// GeoInputDevice.h

#ifndef FALCONVIEW_GEOINPUTDEVICE_H_
#define FALCONVIEW_GEOINPUTDEVICE_H_

#include "FalconView/CameraModes.h"
#include "Viewpoint3d.h"
#include "getobjpr.h" // for fvw_get_view() MapView
#include "MouseClickResource.h"
#include "osgGA/FlightManipulator"
#include "osgGA/GUIEventHandler"

#include "utils.h"  // for FVW_Timer

#pragma warning(disable:4482)

namespace osgEarth
{
   class SpatialReference;
   class Viewpoint;
}

namespace osgViewer
{
   class View;
}

class GeospatialViewer;
class MapView;
class GeospatialViewController;
class HUD;
class GeospatialEarthManipulator;

class FlightInputDeviceStateType
{
    friend HUD;
    friend GeospatialEarthManipulator;
public:
   FlightInputDeviceStateType::FlightInputDeviceStateType();

   bool changeStateRequest;
   bool animating;
   bool throwStart;
   bool throwEnd;
   bool throwing;
   float mouseX, mouseY;
   double deltaTimeThrow;
   boolean_t panning; // Pan Tool's state, not the action of panning
   bool spacebarPressedStart;
   bool spacebarPressedEnd;
   bool leftKeyDown, leftButtonDown;
   bool rightKeyDown, rightButtonDown;
   bool upKeyDown, upButtonDown;
   bool downKeyDown, downButtonDown;
   bool rotCcwButtonDown;
   bool rotCwButtonDown;
   bool northUpButtonDown;
   bool arrowKeyDownStart;
   bool complexArrowKeyThrow; // Arrow key throw with multiple directions eg. UP | LEFT
   bool allowNewArrowKeyInput;
   const osgEarth::SpatialReference* m_srs;
   GeospatialViewer* m_geo_viewer;
   d_geo_t action_center;
   bool pressed;
   bool right_pressed;
   int debugCount; // debugging purposes
   GeospatialViewController* gvc;
   bool north; 
   bool east;
   bool south;
   bool west;
   bool moveUp;
   bool moveDown;
   double move_speed; 
   double move_speed_vertical;
   bool rollLeft;
   bool rollRight;
   double roll_speed;
   double yaw_speed;
   double pitch_speed;
   bool insertKeyDown;
   bool deleteKeyDown;
   bool shift;
   bool control;
   bool space;
   bool num_0;
   bool num_1;
   bool num_2;
   bool num_3;
   bool num_4;
   bool num_5;
   bool num_6;
   bool num_7;
   bool num_8;
   bool num_9;
   double mouse_wheel_change_value;
   bool plus_key;
   bool minus_key;
   bool control_freeze;
   bool velocity_freeze;
   bool north_orient;
   bool top_down_view;
   bool direct_down;
   bool mouse_moved_pressed;
   bool frame_update_wait;
   bool last_frame_movement;
   bool transition_animation;
   int tilt_value;
   bool handled_shift;
   bool handled_tilt;
   bool escape;

   // variables that used to be global
   float mouseXS;
   float mouseYS;
   float mouseXF;
   float mouseYF;
   float oldMouseX;
   float oldMouseY;
   bool first_run;
   bool H_last_frame;
   bool W_last_frame;
   bool handled_key_W;
   bool handled_key_S;
   bool handled_key_A;
   bool handled_key_D;
   bool handled_key_L;
   bool render_as_wireframe;
   SYSTEMTIME eventStart;
   SYSTEMTIME eventEnd;
   SYSTEMTIME noMoveStart;
   SYSTEMTIME noMoveEnd;

   ~FlightInputDeviceStateType()
   {
      POSITION pos = UTL_get_timer_list()->Find
         (&m_flight_mode_auto_map_type_timer);
      if (pos)
         UTL_get_timer_list()->remove_at(pos);
   }
   fvw::CameraMode m_cameraMode;
   fvw::CameraMode m_old_cameraMode;
private:
   std::function<void(void)> m_operation;

   // Private class used for flight mode automatic MapType timer
   //
   // Flight mode can update the MapType automatically as the altitude of the
   // camera is changed. The computation used to determine the best map type
   // for the given altitude is fairly expensive. Because of this, we only want
   // to do so periodically.
   class FlightModeAutoMapTypeTimer : public FVW_Timer
   {
   public:
      FlightModeAutoMapTypeTimer() : FVW_Timer(1000) 
      {
         start();
      }
      void SetCameraPosition(const d_geo_t& location, double alt_meters)
      {
         m_camera_location = location;
         m_camera_alt_meters = alt_meters;
      }

      virtual void expired() override;

   private:
      d_geo_t m_camera_location;
      double m_camera_alt_meters;
   } m_flight_mode_auto_map_type_timer;

   // Flag indicating whether or not flight mode will automatically update
   // the MapType based on the camera's altitude
   bool m_enable_flight_mode_auto_map_type;

   // Add or remove the FlightModeAutoMapTypeTimer based on the camera mode
   // and m_flight_mode_auto_map_type_timer flag.
   void UpdateFlightModeAutoMapTypeTimer();

public:
   bool OnMouseMoved(UINT nFlags, CPoint point);
   
   void ToggleEnableFlightModeAutoMapType();
   bool GetEnableFlightModeAutoMapType() const
   {
      return m_enable_flight_mode_auto_map_type;
   }
   void ResetKeyboard();
   bool SetCameraMode(fvw::CameraMode camera_mode);
   bool OnMouseLeftPressed(UINT nFlags, CPoint point);
   bool OnLeftMouseRelease(UINT nFlags, CPoint point);

   void SetActiveCam(fvw::CameraMode mode);
   fvw::CameraMode GetActiveCam() const;
   fvw::CameraMode GetLastCamMode() const;

   const osgEarth::SpatialReference* GetSRS()
   {
      return m_srs;
   }

   void SetIsPanning(boolean_t _panning)
   {
      panning = _panning;
   }

   void SetCameraPosition(const d_geo_t& location, double alt_meters)
   {
      m_flight_mode_auto_map_type_timer.SetCameraPosition(location,
         alt_meters);
   }

   bool SetUpButtonState(bool state)
   {
      if (state && !upButtonDown)
         arrowKeyDownStart = true;
      upButtonDown = state;
      return true;
   }

   bool SetDownButtonState(bool state)
   {
      if (state && !downButtonDown)
         arrowKeyDownStart = true;
      downButtonDown = state;
      return true;
   }

   bool SetLeftButtonState(bool state)
   {
      if (state && !leftButtonDown)
         arrowKeyDownStart = true;
      leftButtonDown = state;
      return true;
   }

   bool SetRightButtonState(bool state)
   {
      if (state && !rightButtonDown)
         arrowKeyDownStart = true;
      rightButtonDown = state;
      return true;
   }

   bool SetRotCwButtonState(bool state)
   {
      rotCwButtonDown = state;
      return true;
   }

   bool SetRotCcwButtonState(bool state)
   {
      rotCcwButtonDown = state;
      return true;
   }

   bool SetNorthUpButtonState(bool state)
   {
      northUpButtonDown = state;
      return true;
   }

   bool GetUpCommandStatus() { return upButtonDown || upKeyDown; }

   bool GetDownCommandStatus() { return downButtonDown || downKeyDown; }

   bool GetLeftCommandStatus() { return leftButtonDown || leftKeyDown; }

   bool GetRightCommandStatus() { return rightButtonDown || rightKeyDown; }

   bool GetRotCwCommandStatus() { return rotCwButtonDown; }

   bool GetRotCcwCommandStatus() { return rotCcwButtonDown; }

   bool GetNorthUpCommandStatus() { return northUpButtonDown || north_orient; }

   void UpdateKeyboardState();
};

class GeoKeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
   virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
   virtual void accept(osgGA::GUIEventHandlerVisitor& v)
   {
      v.visit(*this);
   }

public:
   GeoKeyboardEventHandler(const osgEarth::SpatialReference* srs,
      FlightInputDeviceStateType* tids) :
   m_spatial_reference(srs),
      m_flightInputDeviceState(tids)
   {
      m_flightInputDeviceState->m_srs = srs;
   }

   FlightInputDeviceStateType* GetFlightInputDeviceState() const
   {
      return m_flightInputDeviceState;
   }

protected:
   const osgEarth::SpatialReference* m_spatial_reference;
   FlightInputDeviceStateType* m_flightInputDeviceState;
};

#endif  // FALCONVIEW_GEOINPUTDEVICE_H_
