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

// GeospatialViewer.cpp

#include "stdafx.h"
#include "FalconView/CameraUtils.h"
#include "FalconView/DisplayElementRootNode.h"
#include "FalconView/GeospatialViewer.h"
#include "FalconView/GeospatialView.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/MapView.h"
#include <osgDB/Registry>
#include <cmath>
#include <math.h>
#include <mmsystem.h>
#include "osgGA/GUIActionAdapter"
#include "FalconView/UIThreadOperation.h"
#include "FalconView/include/param.h"  // for PRM_MAX_VALUE_LENGTH
#include "geo_tool.h"
#include "FalconView/StatusBarInfoPane.h"
#include "FalconView/GenericNodeCallback.h"
#include "osgEarthDrivers/engine_mp/TilePagedLOD"
#include "osgEarth/MapNode"
#include "ElevationDataTileSource.h"
#include "FvCore/Include/Registry.h"
#include "RenderFunctions.h"
#include "common.h"
#include "OSGVariables.h"
#include "FvCore/Include/TimeUtils.h"
#include "err.h"
#include "FvMath.h"
#include "Common/ScopedCriticalSection.h"

using namespace time_utils;

// Below 2 matrices for testing camera interpolation. Do not edit.
/*
osg::Matrixd* mat = new osg::Matrixd(-0.70629797503140412,-0.43474450883825611,0.55869525011541066,0,
                                       0.70615629707942840,-0.48826515103848805,0.51277717029566783,0,
                                       0.049864361638425996,0.75669964603952733,0.65185825999372460,0,
                                       1403.7498962505131,-33.773532265721216,-26366279.370205317,1);

osg::Matrixd* mat2 = new osg::Matrixd(0.89880546395071992,0.049797445122073888,-0.43550999119612066,0,
                                       -0.42190615977353313,0.36783073282363959,-0.82867107125540107,0,
                                       0.11892825701859489,0.92855843459081133,0.35161812130887216,0,
                                       1794.5334317057277,2826.2824735371778,-26361198.118503600,1);

osg::Matrixd* KCOS = new osg::Matrixd(0.96496000034021734,0.097863917349615304,0.097863917349615304,0,
                                       -0.26239529919373789,0.35652696717999388,-0.89668123022312907,0,
                                       -0.00095115952210564477,0.92914540054526629,0.36971329424452720,0,
                                       -1520.5481022647234,-1818236.9222548706,-7442770.8122350499,1);
*/

// Below variables are for animating the camera when
// swapping between camera manipulators.
double animationTime = 0.0;
bool started = false;
osg::Vec3d forward2(14731691.830201114,13519018.369411323,17187032.552097872);
osg::Vec3d up2(-0.43474450865811498,-0.48826515087315225,0.75669964624970720);

// Throwing
osg::Vec3d forwardThrowStart;
osg::Vec3d positionThrowStart;
osg::Vec3d upThrowStart;
osg::Vec3d forwardThrowEnd;
osg::Vec3d positionThrowEnd;
osg::Vec3d upThrowEnd;
osg::Vec3d positionNormalStart;
osg::Vec3d positionNormalEnd;
osg::Vec3d circleNormalPosition;
osg::Vec3d circleNormalForward;
osg::Vec3d circlePerpendicularPosition;
osg::Vec3d circlePerpendicularForward;
osg::Vec3d circlePerpendicularUp;
osg::Vec3d upRadial;
osg::Vec3d forwardRadial;
osg::Vec3d to_east_vector(0.0, 0.0, 0.0);
osg::Vec3d to_north_vector(0.0, 0.0, 0.0);
SYSTEMTIME oldTime2, newTime2;

//#define DISABLE_TILE_TRAVERSAL_WALK_MANIPULATOR // disables tile traversal in walk manipulator

// below variables handle throwing in PAN mode
double height1, height2, elapsedPosition, angularVelocity, startingAngularVelocity;

//  Throw variables
bool arrowKeyThrow;
const double elapsedPositionDurationArrow = 0.0; // animation in seconds
const double elapsedPositionDurationMouse = 1.0;
volatile double timeTotal;
int mandatoryFrameRedraw = 2;

// Enable/Disable panning control
SYSTEMTIME spacebarStart;
SYSTEMTIME spacebarEnd;
bool attemptingPanEnable;

// TTVS controls
bool NSEW_first = false;
SYSTEMTIME old_frame, new_frame;

namespace
{

static ElevationDataTileSource* s_elevation_data_tile_source = nullptr;

ElevationDataTileSource* GetElevationDataTileSource()
{
   if (s_elevation_data_tile_source == nullptr)
   {
      auto view = fvw_get_view();
      if (view != nullptr)
      {
         auto controller = view->GetGeospatialViewController();
         if (controller != nullptr)
         {
            auto scene = controller->GetGeospatialScene();
            if (scene != nullptr)
            {
               auto map_node = scene->MapNode();
               if (map_node != nullptr)
               {
                  auto map = map_node->getMap();
                  if (map != nullptr)
                  {
                     auto layer = map->getElevationLayerAt(0);
                     if( layer != nullptr)
                     {
                        auto source = layer->getTileSource();
                        if (source != nullptr)
                        {
                           s_elevation_data_tile_source =
                              dynamic_cast<ElevationDataTileSource*>(
                              layer->getTileSource());
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return s_elevation_data_tile_source;
}

long GetElevation(double lat, double lon)
{
   static const long error_value = 0;

   auto source = GetElevationDataTileSource();
   if (source == nullptr)
   {
      return error_value;
   }

   if (_isnan(lat) || _isnan(lon))
   {
      return error_value;
   }


   short elevation_type_used;
   long elevation_meters;
   IDtedPtr spDted = source->GetElevationReaderForCurrentThread();
   if ( !(BOOL) spDted )
      return error_value;
   
   // IDted::raw_GetElevation does not approve of longitude values < -180.0
   // or > 180.0
   if (lon < -180.0)
   {
      lon += 360.0;
   }
   else if (lon > 180.0)
   {
      lon -= 360.0;
   }

   // REFACTOR:  Review and correct for intended behavior?
   auto hr = spDted->raw_GetElevation(
      lat, lon, 0, DTED_ELEVATION_METERS,
      &elevation_type_used, &elevation_meters);
   if ( hr != S_OK )
   {
      CStringA msg;
      msg.Format( "raw_GetElevation() failed, hr = %x", hr );
      ERR_report( msg );
      return error_value;
   }
   hr = spDted->raw_GetElevation(
      lat, lon, 0, DTED_ELEVATION_METERS,
      &elevation_type_used, &elevation_meters);

   if ( FAILED( hr ) || elevation_meters == MISSING_DTED_ELEVATION )
   {
      elevation_meters = error_value;
   }
   // END REFACTOR

   return elevation_meters;
}  // GetElevation(lat,lon)

void ResetAnimationVariables(double* elapsed_position,
   double* angular_velocity, volatile double* time_total)
{
   *elapsed_position = 0;
   *time_total = 0;
   *angular_velocity = 0;
}

void ResetArrowKeyState(FlightInputDeviceStateType* state)
{
   state->upKeyDown = false;
   state->downKeyDown = false;
   state->leftKeyDown = false;
   state->rightKeyDown = false;
}

void ResetGlobalMovementState(const fvw::CameraMode& mode,
   FlightInputDeviceStateType* state, double* elapsed_position,
   double* angular_velocity, volatile double* time_total)
{
   if (mode == fvw::CAMERA_MODE_PAN)
   {
      ResetAnimationVariables(elapsed_position, angular_velocity, time_total);
      ResetArrowKeyState(state);

      state->complexArrowKeyThrow = false;
      if (!state->animating)
         state->allowNewArrowKeyInput = true;
   }
   else if (mode == fvw::CAMERA_MODE_FLIGHT)
   {
      ResetAnimationVariables(elapsed_position, angular_velocity, time_total);
      state->complexArrowKeyThrow = false;
      state->allowNewArrowKeyInput = true;
   }
   else if (mode == fvw::CAMERA_MODE_GROUND_VIEW)
   {
      // Intentionally stubbed out to demand no key resets should occur here
   }
}
}

GeospatialViewer::GeospatialViewer(const osgEarth::SpatialReference* srs,
   GeospatialView* parent, GeospatialViewController* gvc) :

m_rendering_enabled(FALSE)
{
   parentView = parent;
   fIDevState = new FlightInputDeviceStateType;
   fIDevState->gvc = gvc;
   fIDevState->m_geo_viewer = this;
   fIDevState->m_srs = srs;
   fIDevState->move_speed = 308.0; // speed for moving with TTVS controls
   fIDevState->move_speed_vertical = 308.0;
   fIDevState->roll_speed = 45.0;
   fIDevState->yaw_speed = 45.0;
   fIDevState->pitch_speed = 4.5;
   MapChangeStateRequest = false;
   updateNextFrame = false;
   m_max_altitude_feet_flight_ground = 500000;
   parentView->m_camera_earth_manipulator->viewer = this;
   parentView->m_camera_flight_manipulator->viewer = this;
   parentView->m_camera_walk_manipulator->viewer = this;
   m_min_altitude_meters_pan_mode = 100.0;
   new_center_viewer.lat = 0.0;
   new_center_viewer.lon = 0.0;
   new_center_viewer_camera.lat = 0.0;
   new_center_viewer_camera.lon = 0.0;
   new_center_local.lat = 0.0;
   new_center_local.lon = 0.0;
   new_center_request = false;

   MapView* map_view = fvw_get_view();
   if (map_view == nullptr)
   {
      throw(new std::exception("MapView cannot be null"));
   }

   GeospatialViewController* controller =
      map_view->GetGeospatialViewController();
   if (controller == nullptr)
   {
      throw(new std::exception("GeospatialViewController cannot be null"));
   }

   GeospatialScene* scene = controller->GetGeospatialScene();
   if (scene == nullptr)
   {
      throw(new std::exception("GeospatialScene cannot be null"));
   }

   HUD* hud = scene->m_HUD;
   if (hud == nullptr)
   {
      throw(new std::exception("HUD cannot be null"));
   }

   hud->control_freeze = &(fIDevState->control_freeze);
   hud->velocity_freeze = &(fIDevState->velocity_freeze);
   hud->Initialize(this);

#ifndef DISABLE_OSG_KEYBOARD
   // for handling key presses that must happen before all messages
   flightEventHandler = new GeoKeyboardEventHandler(srs, fIDevState);
   this->addEventHandler(flightEventHandler);
#else
   flightEventHandler = nullptr;
#endif

   // For testing camera below:
   seconds = -1;
   m_throwDamping = 1.0;
   height_above_ground = 6; // height for walk camera above ground
   meters_per_second = 100; // speed for walking on ground

   // Effectively combine the graphics operation thread and the rendering
   // thread. This is because there are one or more nodes in the scene graph
   // that should be marked with DYNAMIC data variance but are not (and/or
   // other bugs in open scene graph). Until these node types are tracked down,
   // these threads will be combined to eliminate the possibility, however
   // rare, of a crash.
   _threadingModel = osgViewer::ViewerBase::SingleThreaded;

   setQuitEventSetsDone(false);
   setKeyEventSetsDone(0);
}

void GeospatialViewer::UpdateCenter(const r_geo_t& new_center_viewer,
   const fvw::CameraMode& mode)
{
   if (mode == fvw::CAMERA_MODE_FLIGHT ||
      mode == fvw::CAMERA_MODE_GROUND_VIEW)
   {
      new fvw::UIThreadOperation([=]()
      {
         // non pan center update
         d_geo_t new_center_viewer_degrees;
         new_center_viewer_degrees.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
         new_center_viewer_degrees.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
         fvw_get_view()->UpdateCenter(new_center_viewer_degrees);
      });
   }
   else if (mode == fvw::CAMERA_MODE_PAN)
   {
      new fvw::UIThreadOperation([=]()
      {
         fvw_get_view()->GetGeospatialViewController()->
            GetGeospatialView()->Viewer()->PanCenterUpdate();
      });
   }
}

GeospatialViewer::~GeospatialViewer()
{
   delete fIDevState;
}

void GeospatialViewer::PanCenterUpdate()
{
   auto view = fvw_get_view();
   if (view == nullptr)
   {
      return;
   }

   Viewpoint3d new_view_local;
   view->GetGeospatialViewController()->GetViewpoint3D(&new_view_local);

   GeospatialViewer* viewer = view->GetGeospatialViewController()->
      GetGeospatialView()->Viewer();

   // Different manipulators have different centers - need to refactor so that
   // there is a single center for all manipulators.
   new_center_local.lat = RAD_TO_DEG(new_center_viewer.lat);
   new_center_local.lon = RAD_TO_DEG(new_center_viewer.lon);
   view->UpdateCenter(new_center_local);
}

long GeospatialViewer::GetDTEDElevation(double lat, double lon)
{
   return GetElevation(lat, lon);
}

void GeospatialViewer::SetThrowDamping(double value) {
   m_throwDamping = value;
}

GeoKeyboardEventHandler* GeospatialViewer::GetFlightEventHandler()
{
   return flightEventHandler;
}

double GeospatialViewer::GetTimeSinceLastUpdate()
{
   if (time_since_last_frame > 2.0)
      return 0.016666;
   return time_since_last_frame;
}

void GeospatialViewer::SetMapParamsRotation(double new_rotation)
{
   fvw_get_view()->m_map_params.SetRotation(new_rotation);
}

void GeospatialViewer::SetMapParamsZoom(int new_zoom)
{
   // WARNING: Not safe to use during locks of the mapview thread.
   fvw_get_view()->m_map_params.SetZoomPercent(new_zoom);
}

unsigned int frame_number_old = 0U;
bool shift_first_frame = false;
osg::Vec3d loc_world;
osg::Vec3d loc_world_perp;
osg::Vec3d loc_world_norm;
SYSTEMTIME old_frame_absolute;
bool target_location = false;
osg::Vec2d heading_vector;
osg::Vec2d vec2d_north(0.0, 1.0);
double min_ground_distance = 2.0;
bool action_performed = false;
int frame_of_update = 0;

void GeospatialViewer::UpdateCamera()
{
   // These global variables are always set.
    new_center_request = false;
    GetSystemTime(&new_frame);

    MapView* map_view = fvw_get_view();
    if (map_view == nullptr)
    {
       return;
    }

    // Is there anything to do?
    if (map_view->GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION && !fIDevState->animating)
    {
        return;
    }

    long elevation_meters_update = 0.0;
    double msl_meters = 0.0;
    const osg::EllipsoidModel* ellipsoid_model = fIDevState->GetSRS()->getEllipsoid();

    unsigned int frame_number_new = getFrameStamp()->getFrameNumber();
    GetSystemTime(&new_frame);

    if (DeltaTimeSec(old_frame, new_frame) < 0.001)
    {  // prevent update from hogging CPU. Sometimes no measurable amount of time
       // passes between camera updates
       return;
    }

    if (frame_number_new != frame_number_old)
    {
        SYSTEMTIME new_frame_absolute;
        GetSystemTime(&new_frame_absolute);
        time_since_last_frame = DeltaTimeSec(old_frame_absolute, new_frame_absolute);
        time_since_last_frame = GetTimeSinceLastUpdate();
        frame_number_old = frame_number_new;
        GetSystemTime(&old_frame_absolute);

        // handle manipuators that are actively counting frames for their own purposes
        if (parentView->m_camera_walk_manipulator->frames_to_redraw > 0)
        {
           parentView->m_camera_walk_manipulator->frames_to_redraw--;
        }
        if (parentView->m_camera_flight_manipulator->frames_to_redraw > 0)
        {
           parentView->m_camera_flight_manipulator->frames_to_redraw--;
        }
        if (parentView->m_camera_earth_manipulator->frames_to_redraw  > 0)
        {
           parentView->m_camera_earth_manipulator->frames_to_redraw --;
        }

        // set up manipuators that are waiting for new frames for their own purposes
        if (fIDevState->frame_update_wait)
        {
           fIDevState->frame_update_wait = false;
           parentView->m_camera_walk_manipulator->frames_to_redraw = 2;
           parentView->m_camera_flight_manipulator->frames_to_redraw = 2;
           parentView->m_camera_earth_manipulator->frames_to_redraw = 2;
        }
    }
    else
    {
        // prevents a queue of the same key events between frames
        //fIDevState->allowNewArrowKeyInput = false;
    }

    if (fIDevState->spacebarPressedStart) {
        fIDevState->spacebarPressedStart = false;
        GetSystemTime(&spacebarStart);
        attemptingPanEnable = true;
    }

    // Reassigns osg controls depending on if changing earth manipulators
    if (MapChangeStateRequest)
    {
        MapChangeStateRequest = false;
        if (fIDevState->panning) {
            parentView->m_camera_earth_manipulator->getSettings()->bindMouse(
            osgEarth::Util::EarthManipulator::ACTION_PAN,
            osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON);
        }
        else
        {
        parentView->m_camera_earth_manipulator->getSettings()->bindMouse(
            osgEarth::Util::EarthManipulator::ACTION_NULL,
            osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON);
        }
    }

    OSGUserVars::SetAnimationState(fIDevState->animating); 

    d_geo_t* hud_lat_lon = &map_view->GetGeospatialViewController()->
       GetGeospatialScene()->m_HUD->m_lat_lon;
    double hud_lat, hud_lon, alt_hud;
    GetCameraPosition(&hud_lat, &hud_lon, &alt_hud);
    hud_lat_lon->lat = hud_lat;
    hud_lat_lon->lon = hud_lon;

    // prevent falconview from consuming all key presses when not in focus
    if (map_view->m_has_focus)
    {   // update keyboard state now before handling different control modes
        fIDevState->UpdateKeyboardState();
    }
    else
    {
        fIDevState->ResetKeyboard();
    }
    if (fIDevState->control_freeze)
    {
        fIDevState->ResetKeyboard();
    }

    bool upCommanded = fIDevState->GetUpCommandStatus();
    bool downCommanded = fIDevState->GetDownCommandStatus();
    bool leftCommanded = fIDevState->GetLeftCommandStatus();
    bool rightCommanded = fIDevState->GetRightCommandStatus();
    bool rotCwCommanded = fIDevState->GetRotCwCommandStatus();
    bool rotCcwCommanded = fIDevState->GetRotCcwCommandStatus();
    bool northUpCommanded = fIDevState->GetNorthUpCommandStatus();

    if (!fIDevState->animating && fIDevState->GetActiveCam() == fvw::CAMERA_MODE_GROUND_VIEW)
    {
        bool control_arrow_movement = false;
        if (fIDevState->control_freeze || fIDevState->velocity_freeze)
        {
            if (map_view->GetGeospatialViewController()->GetGeospatialScene()->
            m_HUD->time_oscillator_seconds != new_frame.wSecond % 2)
            {
                requestRedraw();
            }
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
                m_HUD->time_oscillator_seconds = (new_frame.wSecond % 2);
        }

        meters_per_second = KNOTS_TO_METERS_PER_S(map_view->
           GetGeospatialViewController()->GetGeospatialScene()->m_HUD->
           m_max_vel);

        new_center_request = false;
        osg::Vec3d position;
        osg::Vec3d forward;
        osg::Vec3d up;
        _camera->getViewMatrixAsLookAt(position,forward,up);
        osg::Vec3d tempUp(position.x(),position.y(),position.z());
        tempUp.normalize();
        osg::Vec3d right;
        osg::Vec3d tempForward(forward-position);
        tempForward.normalize();
        right.set(tempForward^tempUp);
        right.normalize();
        osg::Vec3d forwardPlanar(tempUp ^ right);
        forwardPlanar.normalize();

        if ((upCommanded || downCommanded || leftCommanded || rightCommanded)
           && !fIDevState->control)
        {
            new_center_request = true;
        }
        if (rotCwCommanded  // Rotate CW button on toolbar pressed
           || rotCcwCommanded // Rotate CCW button on toolbar pressed
           || ((upCommanded || downCommanded
           || leftCommanded || rightCommanded)
           && fIDevState->control)) // pivot viewer's camera
        {
           control_arrow_movement = true;
        }

        if (control_arrow_movement)
        {
           double pitch_angle = 0.0;
           double yaw_angle = 0.0;
           if (fIDevState->arrowKeyDownStart)
           {
              GetSystemTime(&oldTime2);
              fIDevState->arrowKeyDownStart = false;
           }
           GetSystemTime(&newTime2);
           double timeElapsed = min(max(DeltaTimeSec(oldTime2, newTime2),0.00), 0.5); // seconds

           // pitch before yaw
           if (upCommanded || downCommanded)
           {
               double max_angle_ground = parentView->m_camera_walk_manipulator->m_max_angle_ground;
               double new_pitch = RAD_TO_DEG(safe_acos(tempUp*tempForward));
               if (new_pitch < (90.0 - max_angle_ground) && upCommanded) // no more than +- 45.0 degees w.r.t. horizon
               {
                  pitch_angle = (max_angle_ground - 0.2);
                  tempForward.set(forwardPlanar);
               }
               else if (new_pitch > (90.0 + max_angle_ground) && downCommanded)
               {
                  pitch_angle = -(max_angle_ground + 0.2);
                  tempForward.set(forwardPlanar);
               }
               else
               {
                  pitch_angle += timeElapsed*(upCommanded*fIDevState->pitch_speed -
                  downCommanded*fIDevState->pitch_speed);
               }
               if (new_pitch > (90.0 - max_angle_ground) || new_pitch < (90.0 + max_angle_ground))
               {
                  pitch_angle = DEG_TO_RAD(pitch_angle);
                  RotationAroundAxis3D(&right, &tempForward, cos(pitch_angle),sin(pitch_angle));
                  RotationAroundAxis3D(&right, &up, cos(pitch_angle),sin(pitch_angle));
                  forward = position + tempForward;
                  _camera->setViewMatrixAsLookAt(position,forward,up);
                  parentView->m_camera_walk_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                  // @TODO: reference max, mine slider values
                  double new_angle = ComputeIncline();
                  double middle_scale = (-89.0-(-14.0))/2.0;
                  double scale_factor = (-89.0-middle_scale)/max_angle_ground;
                  map_view->NotifyTiltUpdate(middle_scale - new_angle*scale_factor);
                  action_performed = true;
               }
           }

           // do not use an else if statement here to allow pitch, yaw at same time.
           if (leftCommanded || rightCommanded
              || rotCwCommanded || rotCcwCommanded)
           {
               yaw_angle += timeElapsed*(
                  (leftCommanded || rotCcwCommanded)
                  *fIDevState->yaw_speed*0.5 -
                  (rightCommanded || rotCwCommanded)
                  *fIDevState->yaw_speed*0.5);
               yaw_angle = DEG_TO_RAD(yaw_angle);
               RotationAroundAxis3D(&tempUp, &tempForward, cos(yaw_angle),sin(yaw_angle));
               RotationAroundAxis3D(&tempUp, &up, cos(yaw_angle),sin(yaw_angle));
               forward = position + tempForward;
               _camera->setViewMatrixAsLookAt(position,forward,up);
               parentView->m_camera_walk_manipulator->setByInverseMatrix(_camera->getViewMatrix());
               map_view->GetGeospatialViewController()->GetGeospatialScene()->
                  m_HUD->m_heading = ComputeHeading();
               action_performed = true;
           }
        }  // if (control_arrow_movement)

        if (fIDevState->handled_shift)
        {
            osg::Vec3d position, forward, up, positionN, forwardN, right, planar_forward;
            _camera->getViewMatrixAsLookAt(position,forward,up);
            positionN.set(position);
            positionN.normalize();
            up.set(positionN);
            forwardN.set(forward-position);
            forwardN.normalize();
            right.set(forwardN^positionN);
            right.normalize();
            planar_forward.set(positionN^right);
            planar_forward.normalize();

            double max_angle_ground = parentView->m_camera_walk_manipulator->m_max_angle_ground;
            double old_angle = fIDevState->tilt_value;
            double slope = (2.0*max_angle_ground)/(-14.0-(-89.0));
            double intercept = -max_angle_ground - slope*(-89.0);
            double angle = slope*old_angle + intercept;

            angle = DEG_TO_RAD(angle);
            RotationAroundAxis3D(&right, &planar_forward, cos(angle), sin(angle));
            RotationAroundAxis3D(&right, &up, cos(angle), sin(angle));

            _camera->setViewMatrixAsLookAt(position, position + planar_forward, up);
            parentView->m_camera_walk_manipulator->setByInverseMatrix(_camera->getViewMatrix());
            action_performed = true;
        }

        if (fIDevState->num_5) // level out camera's view
        {   // makes camera view 3 degrees below horizon (ensures center screen pixel has a texel)
            tempForward.set(forwardPlanar);
            up.set(tempUp);
            double pitch_angle = DEG_TO_RAD(-3.0);
            RotationAroundAxis3D(&right, &tempForward, cos(pitch_angle),sin(pitch_angle));
            RotationAroundAxis3D(&right, &up, cos(pitch_angle),sin(pitch_angle));
            forward = position + tempForward;
            _camera->setViewMatrixAsLookAt(position,forward,up);
            parentView->m_camera_walk_manipulator->setByInverseMatrix(_camera->getViewMatrix());
            action_performed = true;
        }

        if (new_center_request)
        {
            if (fIDevState->arrowKeyDownStart)
            {
                GetSystemTime(&oldTime2);
                fIDevState->arrowKeyDownStart = false;
            }
            GetSystemTime(&newTime2);
            double timeElapsed = max(DeltaTimeSec(oldTime2, newTime2),0.00); // seconds

            if (upCommanded)
                position.operator+=(forwardPlanar.operator*(timeElapsed*meters_per_second));
            if (downCommanded)
                position.operator-=(forwardPlanar.operator*(timeElapsed*meters_per_second));
            if (leftCommanded)
                position.operator-=(right.operator*(timeElapsed*meters_per_second));
            if (rightCommanded)
                position.operator+=(right.operator*(timeElapsed*meters_per_second));

            double height_z;
            ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
            new_center_viewer_camera.lat, new_center_viewer_camera.lon, height_z);
            double msl, ellipsoid;
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
               MapNode()->getTerrain()->getHeight(fIDevState->GetSRS(),
               RAD_TO_DEG(new_center_viewer_camera.lon),
               RAD_TO_DEG(new_center_viewer_camera.lat),&msl, &ellipsoid);

            std::pair<double, double> height_OSG = OSGUserVars::GetCameraHeight();

            ellipsoid_model->convertLatLongHeightToXYZ(new_center_viewer_camera.lat, new_center_viewer_camera.lon, max(height_OSG.first, msl) +
            height_above_ground + 1.0, position.x(), position.y(), position.z());
            osg::Vec3d positionN;
            positionN.set(position);
            positionN.normalize();
            //up.set(positionN);
            forward.set(position+tempForward);
            ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
            new_center_viewer_camera.lat, new_center_viewer_camera.lon, height_z);
            new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
            new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
            OSGUserVars::SetCameraLatLon(new_center_local.lon, new_center_local.lat);
            _camera->setViewMatrixAsLookAt(position,forward,up);
            parentView->m_camera_walk_manipulator->setByInverseMatrix(_camera->getViewMatrix());
            OSGUserVars::SetAltitude(ComputeAltitude());

            double msl_meters;
            _camera->getViewMatrixAsLookAt(position,forward,up);
            ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                new_center_viewer_camera.lat, new_center_viewer_camera.lon, msl_meters);
            new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
            new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
                m_HUD->m_msl_alt_value = METERS_TO_FEET(msl_meters);
            OSGUserVars::SetAltitude(msl_meters);
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
                m_HUD->m_agl_alt_value = METERS_TO_FEET(2);
            GetSystemTime(&oldTime2);
            requestRedraw();
        }


         // handle logic that is waiting for a new frame
         if (parentView->m_camera_walk_manipulator->frames_to_redraw > 0)
         {
            ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                new_center_viewer_camera.lat, new_center_viewer_camera.lon, msl_meters);
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
               m_HUD->m_heading = ComputeHeading();
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
                m_HUD->m_msl_alt_value = METERS_TO_FEET(msl_meters);
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
                m_HUD->m_agl_alt_value = METERS_TO_FEET(2);
            requestRedraw();
         }

         if (fIDevState->mouse_moved_pressed || control_arrow_movement || fIDevState->num_5)
         {
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
               m_HUD->m_heading = ComputeHeading();
            fIDevState->mouse_moved_pressed = false;
            if (action_performed)
            {
               requestRedraw();
            }
         }
    } // end ground mode handling

    if (!fIDevState->animating && fIDevState->GetActiveCam() == fvw::CAMERA_MODE_FLIGHT)
    {
        if (fIDevState->control_freeze || fIDevState->velocity_freeze)
        {
            if (map_view->GetGeospatialViewController()->GetGeospatialScene()->
            m_HUD->time_oscillator_seconds != new_frame.wSecond % 2)
            {
                requestRedraw();
            }
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
                m_HUD->time_oscillator_seconds = (new_frame.wSecond % 2);
        }

        // now update heading, pitch, roll information
        osg::Vec2d heading_vector;
        osg::Vec2d right_vector;
        osg::Vec3d right;
        osg::Vec3d right_local;
        double alt_meters_flight = 0.0;
        double heading = 0.0; // don't precompute heading
        double incline = 0.0;
        double roll = 0.0;
        double yaw = 0.0;
        double pitch = 0.0;
        double shift_speed_check = 1.0;
        action_performed = false; // draws if set to true
        osg::Vec3d position;
        osg::Vec3d forward;
        osg::Vec3d up;
        _camera->getViewMatrixAsLookAt(position, forward, up);
        osg::Vec3d forwardN;
        forwardN.set(forward - position);
        forwardN.normalize();
        long elevation_meters_flight = 0.0;

        ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
            new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters_flight);
        // elevation query to prevent camera from going into ground
        // need ground (0 meters) to get true up vector for this location on earth
        osg::Vec3d position2;
        ellipsoid_model->convertLatLongHeightToXYZ(new_center_viewer_camera.lat, new_center_viewer_camera.lon, 0.0,
            position2.x(), position2.y(), position2.z());
        up2.set(position-position2);
        msl_meters = up2.length();
        up2.normalize();
        incline = min(max(-(90.0 - RAD_TO_DEG(safe_acos(forwardN*up2))), -89.9), 89.9);
        up2.set(position);
        up2.normalize();
        right.set(forwardN ^ up2);
        right.normalize();
        roll = 90.0 - RAD_TO_DEG(safe_acos(up*right));
        double local_speed_arc = fIDevState->move_speed*(WGS84_a_METERS/position.length())*
            (1.0/111320.0); // conversion specific in this case of meters to degrees;
        double frame_time = DeltaTimeSec(old_frame, new_frame);
        to_east_vector.set(north_vector^up2);
        to_east_vector.normalize();
        to_north_vector.set(up2^to_east_vector);
        to_north_vector.normalize();
        heading_vector.x() = (forwardN)*(to_east_vector);
        heading_vector.y() = (forwardN)*(to_north_vector);
        heading_vector.normalize();
        if (heading_vector.x() > 0.0) //(*to_north_vector^(*heading_vector))*(*up2) > 0.0
        {
            heading = RAD_TO_DEG(safe_acos(vec2d_north*heading_vector));
        }
        else
        {
            heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*heading_vector))) + 180.0;
        }
        new_center_local.lat = osg::RadiansToDegrees(new_center_viewer_camera.lat);
        new_center_local.lon = osg::RadiansToDegrees(new_center_viewer_camera.lon);

        double* heading_local = &map_view->GetGeospatialViewController()->
           GetGeospatialScene()->m_HUD->m_heading;
        if (::GetAsyncKeyState(VK_MENU) & 0x8000)
        {
            shift_speed_check = map_view->GetGeospatialViewController()->
               GetGeospatialScene()->m_HUD->m_shift_multiply_speed;
        }
        /* Arrow keys and N,S,E,W movements are allowed at the same time if and only if
            the arrow key event happens before a N,S,E,W event. Otherwise N,S,E,W will be
            disabled as this is the functionality of TTVS. */
        if ((upCommanded || downCommanded || leftCommanded ||
            rightCommanded || fIDevState->moveUp || fIDevState->moveDown) &&
            !(fIDevState->north || fIDevState->south || fIDevState->east || fIDevState->west))
        {
            NSEW_first = false;
        }
        else if (!(upCommanded || downCommanded || leftCommanded ||
            rightCommanded || fIDevState->moveUp || fIDevState->moveDown) &&
            (fIDevState->north || fIDevState->south || fIDevState->east || fIDevState->west))
        {
            NSEW_first = true;
        }
        else if (!(upCommanded || downCommanded || leftCommanded ||
            rightCommanded || fIDevState->moveUp || fIDevState->moveDown) &&
            !(fIDevState->north || fIDevState->south || fIDevState->east || fIDevState->west))
        {  // no key presses
            NSEW_first = false;
        }

        // start control handling
        if (!fIDevState->space)
        {
            if (fIDevState->north || fIDevState->south || fIDevState->east || fIDevState->west)
            {
                elevation_meters_flight = GetElevation(
                RAD_TO_DEG(new_center_viewer_camera.lat),
                RAD_TO_DEG(new_center_viewer_camera.lon));
                if ((upCommanded || downCommanded || leftCommanded ||
                    rightCommanded || fIDevState->moveUp || fIDevState->moveDown) && NSEW_first)
                {
                    fIDevState->north = false;
                    fIDevState->south = false;
                    fIDevState->east = false;
                    fIDevState->west = false;
                }
                else
                {  // Ensure only one N,S,E,W as per TTVS controls
                    double shift_speed = shift_speed_check;
                    if (msl_meters < 1000.0)
                    {
                        shift_speed = 1.0;
                    }
                    if (fIDevState->north &&
                        !(fIDevState->south || fIDevState->east || fIDevState->west))
                    {
                        new_center_request = true;
                        SetCameraPosition(new_center_local.lat + shift_speed*local_speed_arc*frame_time, new_center_local.lon,
                            max(alt_meters_flight, ((double)elevation_meters_flight)+min_ground_distance), heading, incline, roll);
                        parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        action_performed = true;
                    }
                    else if (fIDevState->south &&
                        !(fIDevState->north || fIDevState->east || fIDevState->west))
                    {
                        new_center_request = true;
                        SetCameraPosition(new_center_local.lat - shift_speed*local_speed_arc*frame_time, new_center_local.lon,
                            max(alt_meters_flight, ((double)elevation_meters_flight)+min_ground_distance), heading, incline, roll);
                        parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        action_performed = true;
                    }
                    else if (fIDevState->east &&
                        !(fIDevState->north || fIDevState->south || fIDevState->west))
                    {
                        new_center_request = true;
                        SetCameraPosition(new_center_local.lat, new_center_local.lon + shift_speed*local_speed_arc*frame_time,
                            max(alt_meters_flight, ((double)elevation_meters_flight)+min_ground_distance), heading, incline, roll);
                        parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        action_performed = true;
                    }
                    else if (fIDevState->west &&
                        !(fIDevState->north || fIDevState->south || fIDevState->east))
                    {
                        new_center_request = true;
                        SetCameraPosition(new_center_local.lat, new_center_local.lon - shift_speed*local_speed_arc*frame_time,
                            max(alt_meters_flight, ((double)elevation_meters_flight)+min_ground_distance), heading, incline, roll);
                        parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        action_performed = true;
                    }
                }
            }
            if (!(fIDevState->north || fIDevState->south || fIDevState->east || fIDevState->west))
            {  // check to see if roll is allowed now
                if ((fIDevState->num_4 || fIDevState->num_6) && !(upCommanded
                    || downCommanded || leftCommanded || rightCommanded))
                {
                    roll += fIDevState->roll_speed*fIDevState->num_6*frame_time -
                        fIDevState->roll_speed*fIDevState->num_4*frame_time;
                    roll = min(max(roll, -89.0), 89.0);
                    if (roll > -89.0 && roll < 89.0)
                    {
                        SetCameraPosition(new_center_local.lat, new_center_local.lon,
                            alt_meters_flight, heading, incline, roll);
                        parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        action_performed = true;
                    }
                }
            }

            /********************************/
            // below handling must come after above. eg. if requested, yaw must be last operation
            // will need to recompute some variables too.
            /********************************/
            _camera->getViewMatrixAsLookAt(position,forward,up);
            forwardN.set(forward-position);
            forwardN.normalize();
            right_local.set(forwardN^up);
            right_local.normalize();
            osg::Vec3d positionN;
            positionN.set(position);
            positionN.normalize();
            circlePerpendicularForward.set(positionN^right_local);
            circlePerpendicularForward.normalize();
            circleNormalForward.set(right_local);

            osg::Vec3d pos_up;
            double focus_angle = 0.0;
            if (northUpCommanded)
            {
               osg::Vec3d positionNL(position);
               positionNL.normalize();

               // update heading
               to_east_vector.set(north_vector^positionNL);
               to_east_vector.normalize();
               to_north_vector.set(positionNL^to_east_vector);
               to_north_vector.normalize();
               forwardN.set(forward-position);
               heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
               heading_vector.y() = (forwardN)*(to_north_vector);
               heading_vector.normalize();
               if (heading_vector.x() > 0.0) // 0 <= heading < 180
               {
                  heading = RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)));
                  focus_angle = DeltaTimeSec(old_frame, new_frame);
                  if (heading - RAD_TO_DEG(focus_angle) < 0.0)
                  {
                        focus_angle = DEG_TO_RAD(heading);
                  }
               }
               else // 180 <= heading <= 360
               {
                  heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)))) + 180.0;
                  focus_angle = -DeltaTimeSec(old_frame, new_frame);
                  if (heading - RAD_TO_DEG(focus_angle) > 360.0)
                  {
                        focus_angle = -DEG_TO_RAD(360.0-heading);
                  }
               }
               pos_up.set(up);
               RotationAroundAxis3D(&positionN, &forwardN, cos(focus_angle), sin(focus_angle));
               RotationAroundAxis3D(&positionN, &pos_up, cos(focus_angle), sin(focus_angle));
               _camera->setViewMatrixAsLookAt(position, position+forwardN, pos_up);
               dynamic_cast<GeospatialEarthManipulator*>(parentView->
                  m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
               parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
               action_performed = true;
               // can't insert other controls when doing a north up command
               fIDevState->ResetKeyboard();
               fIDevState->north_orient = true;
            } // end north orient handling

            // enable God's eye view
            if (fIDevState->direct_down)
            {
               // frame_time;
               _camera->getViewMatrixAsLookAt(position,forward,up);
               forwardN.set(forward-position);
               forwardN.normalize();
               up2.set(position);
               up2.normalize();
               double incline = min(max(-(89.0 - RAD_TO_DEG(safe_acos(forwardN*up2))), -88.5), 88.5);
               double frame_time = DeltaTimeSec(old_frame, new_frame);
               double dIncline = frame_time*fIDevState->pitch_speed*8.0;
               right.set(forwardN ^ up2);
               right.normalize();
               double roll = 0.0;

               if (incline + dIncline > 88.0)
               {
                  incline = 88.5;
               }
               if (incline < 88.5)
               {
                  incline = max(min(incline + dIncline, 88.0), 14.0);

                  double heading = ComputeHeading();

                  ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                       new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters_flight);
                  new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
                  new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);

                  // compute roll but do not change it
                  roll = 89.9 - RAD_TO_DEG(safe_acos(up*right));

                  SetCameraPosition(new_center_local.lat, new_center_local.lon,
                     alt_meters_flight, heading, incline, roll);
                  parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                  fIDevState->ResetKeyboard();
                  fIDevState->direct_down = true;
                  action_performed = true;
               }
            } // end handling God's eye view

            // handle tilting, rotating around a fixed point
            if ((fIDevState->shift && !fIDevState->control) || fIDevState->top_down_view)
            {
               osg::Vec3d right_vector(0,0,0);
               osg::Vec3d pos_up(0,0,0);
               double focus_angle = 0.0;
               double focus_angle_vert = 0.0;
               _camera->getViewMatrixAsLookAt(position,forward,up);
               if ((fIDevState->shift || fIDevState->top_down_view)
                  && !shift_first_frame)
               {
                   shift_first_frame = true;
                   target_location = true;
               }
               else
               {
                   shift_first_frame = false;
                   loc_world.set(0,0,0);
               }
               if (fIDevState->shift || fIDevState->top_down_view)
               {
                   if (fIDevState->num_4 || leftCommanded ||
                       fIDevState->num_6 || rightCommanded ||
                       fIDevState->num_8 || upCommanded ||
                       fIDevState->num_2 || downCommanded ||
                       fIDevState->top_down_view)
                   {
                       if (target_location)
                       {
                           osgUtil::LineSegmentIntersector::Intersections mid_screen;
                           computeIntersections(getCamera()->getViewport()->width()/2.0,
                               getCamera()->getViewport()->height()/2.0, mid_screen);
                           if (!mid_screen.empty())
                           {
                               int firstOnly = 0;
                               for (osgUtil::LineSegmentIntersector::Intersections::const_iterator iter = mid_screen.begin();
                                   firstOnly < 1;
                                   firstOnly++)
                               {
                                   loc_world.set(iter->getWorldIntersectPoint());
                                   positionN.set(loc_world);
                                   positionN.normalize();
                                   loc_world_perp.set(up^positionN);
                                   loc_world_perp.normalize();
                                   forwardN.set(position);
                                   forwardN.normalize();
                                   if (forwardN*positionN > -0.2)
                                   {
                                       focus_angle = 0.0;
                                       target_location = false;
                                   }
                               }
                           }
                       } // end compute target location
                       if (loc_world.length() > 0)
                       {
                           positionN.set(loc_world);
                           positionN.normalize();
                           // left, right before up, down
                           if (fIDevState->num_4 || leftCommanded ||
                               fIDevState->num_6 || rightCommanded)
                           {
                               if (fIDevState->num_4 || leftCommanded)
                               {
                                   //focus_angle = GetTimeSinceLastUpdate()/1.33;
                                   focus_angle = DeltaTimeSec(old_frame, new_frame);
                               }
                               if (fIDevState->num_6 || rightCommanded)
                               {
                                   //focus_angle = -GetTimeSinceLastUpdate()/1.33;
                                   focus_angle = -DeltaTimeSec(old_frame, new_frame);
                               }
                               pos_up.set(position+up);
                               RotationAroundAxis3D(&positionN, &position, cos(focus_angle), sin(focus_angle));
                               RotationAroundAxis3D(&positionN, &pos_up, cos(focus_angle), sin(focus_angle));

                               _camera->setViewMatrixAsLookAt(position, loc_world, pos_up-position);
                               dynamic_cast<GeospatialEarthManipulator*>(parentView->
                                   m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                               parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                               OSGUserVars::SetAltitude(ComputeAltitude());
                               *heading_local = ComputeHeading();
                               action_performed = true;
                           }
                           if ((fIDevState->num_8 || upCommanded ||
                               fIDevState->num_2 || downCommanded ||
                               fIDevState->top_down_view))
                           {
                               _camera->getViewMatrixAsLookAt(position2,forward2,up2);
                               if (fIDevState->num_8 || upCommanded || fIDevState->top_down_view)
                               {
                                   //focus_angle = GetTimeSinceLastUpdate()/1.33;
                                   focus_angle = -min(max(DeltaTimeSec(old_frame, new_frame), 0.04), 0.25)*1.5;
                               }
                               if (fIDevState->num_2 || downCommanded)
                               {
                                   //focus_angle = -GetTimeSinceLastUpdate()/1.33;
                                   focus_angle = min(max(DeltaTimeSec(old_frame, new_frame), 0.04), 0.25);
                                   if (focus_angle > 0.05)
                                   {
                                      int x = 3;
                                   }
                               }
                               pos_up.set(position2+up2-loc_world);
                               position2.operator-=(loc_world);

                               loc_world_perp.set(up2^positionN);
                               loc_world_perp.normalize();
                               loc_world_norm.set(loc_world_perp^positionN);
                               loc_world_norm.normalize();

                               RotationAroundAxis3D(&loc_world_perp, &position2, cos(focus_angle), sin(focus_angle));
                               RotationAroundAxis3D(&loc_world_perp, &pos_up, cos(focus_angle), sin(focus_angle));
                               up2.set(pos_up-position2);
                               up2.normalize();

                               //double testB = up2*positionN;
                               if (up2*positionN > cos(DEG_TO_RAD(10.0)))
                               {
                                   focus_angle = DEG_TO_RAD(10.0);

                                   loc_world_norm.operator*=(position2.length());
                                   RotationAroundAxis3D(&loc_world_perp, &loc_world_norm, cos(-focus_angle), sin(-focus_angle));
                                   positionN.set(loc_world_norm-loc_world);
                                   positionN.normalize();
                                   position2.set(loc_world_norm + loc_world);
                                   up2.set(loc_world_perp^positionN);
                                   up2.normalize();
                                   fIDevState->last_frame_movement = false;
                               }
                               else if  (up2*positionN < cos(DEG_TO_RAD(88.0)))
                               {
                                   focus_angle = DEG_TO_RAD(88.0);

                                   loc_world_norm.operator*=(position2.length());
                                   RotationAroundAxis3D(&loc_world_perp, &loc_world_norm, cos(-focus_angle), sin(-focus_angle));
                                   positionN.set(loc_world_norm-loc_world);
                                   positionN.normalize();
                                   position2.set(loc_world_norm + loc_world);
                                   up2.set(loc_world_perp^positionN);
                                   up2.normalize();
                                   fIDevState->last_frame_movement = false;
                               }
                               else
                               {
                                   position2.operator+=(loc_world);
                                   fIDevState->last_frame_movement = true;
                               }
                               if (fIDevState->last_frame_movement)
                               {
                                  _camera->setViewMatrixAsLookAt(position2, loc_world, up2);
                                  dynamic_cast<GeospatialEarthManipulator*>(parentView->
                                      m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                                  parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                                  OSGUserVars::SetAltitude(ComputeAltitude());
                                  action_performed = true;
                               }
                               forwardN.set(loc_world-position2);
                               forwardN.normalize(); // needed later
                           } // end arrow key handling
                       } // end pivot handling
                   } // end handling of keys that depend on location in center of screen
               }
            }

            if (!fIDevState->control && !fIDevState->shift)
            {
                elevation_meters_flight = GetElevation(
                   RAD_TO_DEG(new_center_viewer_camera.lat),
                   RAD_TO_DEG(new_center_viewer_camera.lon));
                if (!(upCommanded || downCommanded) &&
                    (leftCommanded || rightCommanded))
                {
                    double shift_speed = shift_speed_check;
                    if (msl_meters < 1000.0)
                    {
                        shift_speed = 1.0;
                    }
                    double alt_2 = up2*(right_local)*frame_time*fIDevState->move_speed*rightCommanded -
                        up2*(right_local)*frame_time*fIDevState->move_speed*leftCommanded;
                    right_vector.x() = (right_local)*(to_east_vector)*rightCommanded -
                        (right_local)*(to_east_vector)*leftCommanded;
                    right_vector.y() = (right_local)*(to_north_vector)*rightCommanded -
                        (right_local)*(to_north_vector)*leftCommanded;
                    double dx = shift_speed*right_vector.x()*frame_time*local_speed_arc;
                    double dy = shift_speed*right_vector.y()*frame_time*local_speed_arc;
                    double angle = safe_asin(0.5*sqrt(dx*dx + dy*dy)/WGS84_a_METERS);

                    _camera->getViewMatrixAsLookAt(position, forward, up);
                    ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                       new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters_flight);
                    alt_meters_flight += shift_speed*alt_2;
                    new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
                    new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
                    new_center_local.lon += dx;
                    new_center_local.lat += dy;

                    RotationAroundAxis3D(&circlePerpendicularForward, &position, cos(angle),
                            sin(angle));
                    RotationAroundAxis3D(&circlePerpendicularForward, &forward, cos(angle),
                            sin(angle));
                    forwardN.set(forward-position);
                    forwardN.normalize();
                    up2.set(position);
                    up2.normalize();

                    to_east_vector.set(north_vector^up2);
                    to_east_vector.normalize();
                    to_north_vector.set(up2^to_east_vector);
                    to_north_vector.normalize();
                    heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
                    heading_vector.y() = (forwardN)*(to_north_vector);
                    heading_vector.normalize();
                    if (heading_vector.x() > 0.0)
                    {
                        heading = RAD_TO_DEG(safe_acos(vec2d_north*heading_vector));
                    }
                    else
                    {
                        heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*heading_vector))) + 180.0;
                    }
                    *heading_local = heading;

                    SetCameraPosition(new_center_local.lat, new_center_local.lon,
                        max(alt_meters_flight, ((double)elevation_meters_flight)+min_ground_distance), heading, incline, roll); //heading, incline, roll);
                    parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    OSGUserVars::SetAltitude(ComputeAltitude());
                    new_center_request = true;
                    action_performed = true;
                }
                if ((upCommanded || downCommanded) &&
                    !(leftCommanded || rightCommanded))
                {
                    double shift_speed = shift_speed_check;
                    if (msl_meters < 1000.0)
                    {
                        shift_speed = 1.0;
                    }
                    double alt_2 = up2*(forwardN)*frame_time*fIDevState->move_speed*upCommanded -
                        up2*(forwardN)*frame_time*fIDevState->move_speed*downCommanded;
                    right_vector.x() = (forwardN)*(to_east_vector)*upCommanded -
                        (forwardN)*(to_east_vector)*downCommanded;
                    right_vector.y() = (forwardN)*(to_north_vector)*upCommanded -
                        (forwardN)*(to_north_vector)*downCommanded;
                    double dx = shift_speed*right_vector.x()*frame_time*local_speed_arc;
                    double dy = shift_speed*right_vector.y()*frame_time*local_speed_arc;
                    double angle = safe_asin(0.5*sqrt(dx*dx + dy*dy)/WGS84_a_METERS);

                    _camera->getViewMatrixAsLookAt(position, forward, up);
                    ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                       new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters_flight);
                    alt_meters_flight += shift_speed*alt_2;
                    new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
                    new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
                    new_center_local.lon += dx;
                    new_center_local.lat += dy;

                    RotationAroundAxis3D(&circleNormalForward, &position, cos(angle),
                            sin(angle));
                    RotationAroundAxis3D(&circleNormalForward, &forward, cos(angle),
                            sin(angle));
                    forwardN.set(forward-position);
                    forwardN.normalize();
                    up2.set(position);
                    up2.normalize();

                    to_east_vector.set(north_vector^up2);
                    to_east_vector.normalize();
                    to_north_vector.set(up2^to_east_vector);
                    to_north_vector.normalize();
                    heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
                    heading_vector.y() = (forwardN)*(to_north_vector);
                    heading_vector.normalize();
                    if (heading_vector.x() > 0.0)
                    {
                        heading = RAD_TO_DEG(safe_acos(vec2d_north*heading_vector));
                    }
                    else
                    {
                        heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*heading_vector))) + 180.0;
                    }
                    *heading_local = heading;

                    SetCameraPosition(new_center_local.lat, new_center_local.lon,
                        max(alt_meters_flight, ((double)elevation_meters_flight)+min_ground_distance), heading, incline, roll); //heading, incline, roll);
                    parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    OSGUserVars::SetAltitude(ComputeAltitude());
                    new_center_request = true;
                    action_performed = true;
                }
            } // end handling of keys affected by the control and shift keys not being pressed

            // insert, delete handling for throttle
            if (!(fIDevState->control) && !(fIDevState->shift))
            {
                double * throttle = &map_view->GetGeospatialViewController()->
                   GetGeospatialScene()->m_HUD->m_throttle_value;

                if (fIDevState->mouse_wheel_change_value != 0 && !(fIDevState->shift) &&
                    !fIDevState->velocity_freeze && !fIDevState->control_freeze)
                {
                    *throttle += (fIDevState->mouse_wheel_change_value/10.0);
                    *throttle = min(max(*throttle, -1.0), 1.0);
                    fIDevState->mouse_wheel_change_value = 0;
                    action_performed = true;
                }
                else if ((fIDevState->insertKeyDown || fIDevState->deleteKeyDown) &&
                    !fIDevState->velocity_freeze && !fIDevState->control_freeze)
                {
                    *throttle += (frame_time*fIDevState->insertKeyDown -
                        frame_time*fIDevState->deleteKeyDown);
                    *throttle = min(max(*throttle, -1.0), 1.0);
                    action_performed = true;
                }
                if (abs(*throttle) > 0.05 && !fIDevState->velocity_freeze && !fIDevState->control_freeze)
                {
                    double shift_speed = shift_speed_check;
                    if (msl_meters < 1000.0)
                    {
                        shift_speed = 1.0;
                    }
                    double alt_2 = up2*(forwardN)*frame_time*fIDevState->move_speed*(*throttle);
                    alt_meters_flight += alt_2*shift_speed;
                    if (*throttle > 0.0)
                    {
                        right_vector.x() = (forwardN)*(to_east_vector);
                        right_vector.y() = (forwardN)*(to_north_vector);
                    }
                    else
                    {
                        right_vector.x() = (forwardN)*(to_east_vector);
                        right_vector.y() = (forwardN)*(to_north_vector);
                    }
                    double dx = shift_speed*right_vector.x()*frame_time*local_speed_arc*(*throttle);
                    double dy = shift_speed*right_vector.y()*frame_time*local_speed_arc*(*throttle);
                    double angle = safe_asin(0.5*sqrt(dx*dx + dy*dy)/WGS84_a_METERS);

                    new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
                    new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
                    new_center_local.lon += dx;
                    new_center_local.lat += dy;

                    RotationAroundAxis3D(&circleNormalForward, &position, cos(angle),
                            sin(angle));
                    RotationAroundAxis3D(&circleNormalForward, &forward, cos(angle),
                            sin(angle));
                    forwardN.set(forward-position);
                    forwardN.normalize();
                    up2.set(position);
                    up2.normalize();

                    to_east_vector.set(north_vector^up2);
                    to_east_vector.normalize();
                    to_north_vector.set(up2^to_east_vector);
                    to_north_vector.normalize();
                    heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
                    heading_vector.y() = (forwardN)*(to_north_vector);
                    heading_vector.normalize();
                    if (heading_vector.x() > 0.0)
                    {
                        heading = RAD_TO_DEG(safe_acos(vec2d_north*heading_vector));
                    }
                    else
                    {
                        heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*heading_vector))) + 180.0;
                    }
                    *heading_local = heading;

                    elevation_meters_flight = GetElevation(
                       new_center_local.lat, new_center_local.lon);

                    SetCameraPosition(new_center_local.lat, new_center_local.lon,
                        max(alt_meters_flight, ((double)elevation_meters_flight)+min_ground_distance), heading, incline, roll); //heading, incline, roll);
                    parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    OSGUserVars::SetAltitude(ComputeAltitude());
                    new_center_request = true;
                    action_performed = true;
                }
            } // end insert, delete handling for throttle

            // camera movement up, down
            if (fIDevState->moveUp || fIDevState->moveDown)
            {
                double shift_speed = shift_speed_check;
                if (msl_meters < 1000.0)
                {
                    shift_speed = 1.0;
                }
                alt_meters_flight += shift_speed*(fIDevState->move_speed_vertical*fIDevState->moveUp*frame_time -
                    fIDevState->move_speed_vertical*fIDevState->moveDown*frame_time);
                elevation_meters_flight = GetElevation(
                       new_center_local.lat, new_center_local.lon);
                SetCameraPosition(new_center_local.lat, new_center_local.lon,
                    max(alt_meters_flight, ((double)elevation_meters_flight)+min_ground_distance), heading, incline, roll);
                parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                OSGUserVars::SetAltitude(ComputeAltitude());
                action_performed = true;
            } // end camera movement up, down

            // pitch and yaw handling last
            if (((upCommanded || downCommanded || leftCommanded || rightCommanded) && (fIDevState->control))
               || rotCwCommanded || rotCcwCommanded)
            {
                if ((upCommanded || downCommanded) &&
                    !(leftCommanded || rightCommanded))
                {
                    double delta_pitch = fIDevState->pitch_speed*upCommanded*frame_time -
                        fIDevState->pitch_speed*downCommanded*frame_time;
                    if (::GetAsyncKeyState(VK_MENU) & 0x8000)
                        delta_pitch *= 8.0;
                    pitch += delta_pitch;
                    RotationAroundAxis3D(&right_local, &forwardN, cos(DEG_TO_RAD(pitch)),
                        sin(DEG_TO_RAD(pitch)));
                    RotationAroundAxis3D(&right_local, &up, cos(DEG_TO_RAD(pitch)),
                        sin(DEG_TO_RAD(pitch)));
                    _camera->setViewMatrixAsLookAt(position, position+forwardN, up);
                    parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    action_performed = true;
                }
                else if (!(upCommanded || downCommanded) &&
                    (leftCommanded || rightCommanded || rotCwCommanded || rotCcwCommanded))
                {
                    yaw += fIDevState->yaw_speed*(leftCommanded || rotCcwCommanded)*frame_time -
                        fIDevState->yaw_speed*(rightCommanded || rotCwCommanded)*frame_time;
                    RotationAroundAxis3D(&up, &forwardN, cos(DEG_TO_RAD(yaw)),
                        sin(DEG_TO_RAD(yaw)));

                    up2.set(position);
                    up2.normalize();
                    to_east_vector.set(north_vector^up2);
                    to_east_vector.normalize();
                    to_north_vector.set(up2^to_east_vector);
                    to_north_vector.normalize();
                    heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
                    heading_vector.y() = (forwardN)*(to_north_vector);
                    heading_vector.normalize();
                    if (heading_vector.x() > 0.0)
                    {
                        heading = RAD_TO_DEG(safe_acos(vec2d_north*heading_vector));
                    }
                    else
                    {
                        heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*heading_vector))) + 180.0;
                    }
                    *heading_local = heading;

                    _camera->setViewMatrixAsLookAt(position, position+forwardN, up);
                    parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    action_performed = true;
                }
            }

            // if numpad 5 key, level out camera
            if (fIDevState->num_5)
            {
                up2.set(position);
                up2.normalize();
                right.set(forwardN^up2);
                right.normalize();

                double e = WGS84_a_METERS;
                double p = WGS84_b_METERS;
                double a = (pow(forwardN.x()/e, 2.0) +
                   pow(forwardN.y()/e, 2.0) +
                   pow(forwardN.z()/p, 2.0));
                double b = 2.0*((position.x()/e)*(forwardN.x()/e) +
                   (position.y()/e)*(forwardN.y()/e) +
                   (position.z()/p)*(forwardN.z()/p));
                double c = (pow(position.x()/e,2.0) +
                   pow(position.y()/e,2.0) + 
                   pow(position.z()/p,2.0)) - 1.0;
               if ((pow(b,2) - 4*a*c) < 0)
               { // looking above horizon
                  double angle = RAD_TO_DEG(safe_asin(WGS84_a_METERS/position.length())) - 90.0 - 2.0;
                  forwardN.set(up2^right);
                  up.set(right^forwardN);
                  RotationAroundAxis3D(&right, &forwardN, cos(DEG_TO_RAD(angle)), sin(DEG_TO_RAD(angle)));
                  RotationAroundAxis3D(&right, &up2, cos(DEG_TO_RAD(angle)), sin(DEG_TO_RAD(angle)));
               }
               else
               {  // looking below horizon
                  up.set(right^forwardN);
               }
               _camera->setViewMatrixAsLookAt(position, position+forwardN, up);
               parentView->m_camera_flight_manipulator->setByInverseMatrix(_camera->getViewMatrix());
               fIDevState->ResetKeyboard();
               action_performed = true;
            }

            // compute angle wrt horizon, also compute heading
            if ((upCommanded || downCommanded) || fIDevState->pressed)
            {
                map_view->GetGeospatialViewController()->GetGeospatialScene()->
                   m_HUD->m_angle_pitch = forwardN*(circlePerpendicularForward);

                up2.set(position);
                up2.normalize();
                to_east_vector.set(north_vector^up2);
                to_east_vector.normalize();
                to_north_vector.set(up2^to_east_vector);
                to_north_vector.normalize();
                heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
                heading_vector.y() = (forwardN)*(to_north_vector);
                heading_vector.normalize();
                if (heading_vector.x() > 0.0)
                {
                    heading = RAD_TO_DEG(safe_acos(vec2d_north*heading_vector));
                }
                else
                {
                    heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*heading_vector))) + 180.0;
                }
                *heading_local = heading;
                if (fIDevState->last_frame_movement)
                {
                  action_performed = true;
                }
            }
        }
        else
        {  // want to stop all motion if spacebar is pressed
            action_performed = true;
            fIDevState->ResetKeyboard();
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
               m_HUD->m_throttle_value = 0.0;
        }

         // handle logic that is waiting for a new frame
         if (parentView->m_camera_flight_manipulator->frames_to_redraw > 0)
         {
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
               m_HUD->m_heading = ComputeHeading();
            requestRedraw();
         }

        // request a redraw and update HUD
        if (action_performed || fIDevState->mouse_moved_pressed)
        {
            HUD* hud = map_view->GetGeospatialViewController()->GetGeospatialScene()->
               m_HUD;
            hud->m_msl_alt_value = METERS_TO_FEET(ComputeAltitudeMSL());
            OSGUserVars::SetAltitude(hud->m_msl_alt_value);
            hud->m_agl_alt_value =
               METERS_TO_FEET(ComputeAltitude());
            if (fIDevState->mouse_moved_pressed || northUpCommanded)
            {
                hud->m_heading = ComputeHeading();
                fIDevState->mouse_moved_pressed = false;
            }
            _camera->getViewMatrixAsLookAt(position, forward, up);
            osg::Vec3d positionN(-position);
            positionN.normalize();
            osg::Vec3d forwardN(forward-position);
            forwardN.normalize();

            double e = WGS84_a_METERS;
            double p = WGS84_b_METERS;
            osg::Vec3d lookVector(forward-position);
            lookVector.normalize();
            double a = (pow(lookVector.x()/e, 2.0) +
               pow(lookVector.y()/e, 2.0) +
               pow(lookVector.z()/p, 2.0));
            double b = 2.0*((position.x()/e)*(lookVector.x()/e) +
                  (position.y()/e)*(lookVector.y()/e) +
                  (position.z()/p)*(lookVector.z()/p));
            double c = (pow(position.x()/e,2.0) +
                  pow(position.y()/e,2.0) + 
                  pow(position.z()/p,2.0)) - 1.0;
            double determinant = pow(b,2) - 4*a*c;
            if (determinant < 0)
            {
               osg::Vec3d desired_forward(positionN);
               osg::Vec3d planar(positionN^forwardN);
               planar.normalize();
               double below_msl_safety = 1.0;
               double angle = safe_asin((p*below_msl_safety)/position.length());
               RotationAroundAxis3D(&planar, &desired_forward, cos(angle), sin(angle));
               desired_forward.normalize();
               OSGUserVars::SetDesiredForwardVec(&desired_forward);
            }
            requestRedraw();
        }
    } // end flight mode handling

    else if (!fIDevState->animating && fIDevState->GetActiveCam() == fvw::CAMERA_MODE_PAN)
    { // Handle throwing the earth in pan mode
        if (fIDevState->control_freeze || fIDevState->velocity_freeze)
        {
            if (map_view->GetGeospatialViewController()->GetGeospatialScene()->
               m_HUD->time_oscillator_seconds != new_frame.wSecond % 2)
            {
                requestRedraw();
            }
            map_view->GetGeospatialViewController()->GetGeospatialScene()->
                m_HUD->time_oscillator_seconds = (new_frame.wSecond % 2);
        }

        if (fIDevState->throwStart)
        { //  User clicks mouse
            _camera->getViewMatrixAsLookAt(positionThrowStart,forwardThrowStart,upThrowStart);
            positionNormalStart = positionThrowStart;
            positionNormalStart.normalize();
            fIDevState->throwStart = false;
            timeTotal = 0;
            elapsedPosition = 0;
        }
        if (fIDevState->throwEnd)
        { //  User lets go of mouse click
            arrowKeyThrow = false;
            _camera->getViewMatrixAsLookAt(positionThrowEnd,forwardThrowEnd,upThrowEnd);
            osg::Vec3d forwardNormal(forwardThrowEnd - positionThrowEnd);
            forwardNormal.normalize();
            positionNormalEnd = positionThrowEnd;
            positionNormalEnd.normalize();
            fIDevState->throwing = true;
            fIDevState->throwEnd = false;
            angularVelocity = safe_acos(positionNormalStart*(positionNormalEnd))/fIDevState->deltaTimeThrow;
            startingAngularVelocity = angularVelocity;

            // Circle axes group 1
            circleNormalPosition = positionNormalStart^positionNormalEnd;
            circleNormalPosition.normalize();
            height2 = cos(safe_acos(circleNormalPosition*forwardNormal));
            forwardRadial = forwardThrowEnd - circleNormalPosition.operator*(height2);
            height1 = cos(safe_acos(circleNormalPosition*upThrowEnd));
            upRadial = positionThrowEnd + upThrowEnd - circleNormalPosition.operator*(height1);

            // Circle axes group 2
            circlePerpendicularPosition = circleNormalPosition^positionNormalEnd;
            circlePerpendicularForward = circleNormalPosition^forwardRadial;
            circlePerpendicularForward.normalize();
            circlePerpendicularUp = circleNormalPosition^upRadial;
            circlePerpendicularUp.normalize();

            // Circle axes group 3
            circlePerpendicularPosition.operator*=(positionThrowEnd.length());
            circlePerpendicularForward.operator*=(forwardRadial.length());
            circlePerpendicularUp.operator*=(upRadial.length());

            elapsedPosition = 0;
            GetSystemTime(&oldTime);
        } // end throw computing

        if (fIDevState->arrowKeyDownStart)
        {
           fIDevState->throwing = false;
        }

         if (fIDevState->arrowKeyDownStart)
         {
            GetSystemTime(&oldTime2);
            fIDevState->arrowKeyDownStart = false;
         }

        int direction = 0; // clockwise or counterclockwise movement on circular path
        bool control_key_override = false;
        bool shift_key_override = false;
        bool action_performed = false; // draws if set to true


        if (fIDevState->allowNewArrowKeyInput)
        {
            double heading = 0.0; // don't precompute heading
            osg::Vec2d vec2d_north(0.0, 1.0);
            double alt_meters = 0.0;
            long elevation_meters_pan = 0.0;

            if (fIDevState->plus_key && !fIDevState->minus_key)
            {
                double zoom_rate = -0.005;
                //if (fIDevState->alt)
                    //zoom_rate *= 3.0;
                dynamic_cast<GeospatialEarthManipulator*>(parentView->
                    m_camera_earth_manipulator.get())->zoom(0,zoom_rate,this);
                action_performed = true;
            }
            else if (!fIDevState->plus_key && fIDevState->minus_key)
            {
                double zoom_rate = 0.005;
                //if (fIDevState->alt)
                    //zoom_rate *= 3.0;
                dynamic_cast<GeospatialEarthManipulator*>(parentView->
                    m_camera_earth_manipulator.get())->zoom(0,zoom_rate,this);
                action_performed = true;
            }

            // enable God's eye view
            if (fIDevState->direct_down)
            {
               // frame_time;
               osg::Vec3d position;
               osg::Vec3d forward;
               osg::Vec3d up;
               _camera->getViewMatrixAsLookAt(position,forward,up);
               osg::Vec3d forwardN;
               forwardN.set(forward-position);
               forwardN.normalize();
               up2.set(position);
               up2.normalize();
               double incline = min(max(-(90.0 - RAD_TO_DEG(safe_acos(forwardN*up2))), -89.9), 89.9);
               double frame_time = DeltaTimeSec(old_frame, new_frame);
               double dIncline = frame_time*fIDevState->pitch_speed*8.0;

               if (incline + dIncline > 89.5)
               {
                  incline = 89.9;
               }
               if (incline < 89.9)
               {
                  incline = max(min(incline + dIncline, 89.5), 14.0);

                  double heading = ComputeHeading();

                  ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                       new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters);
                  // elevation query to prevent camera from going into ground
                  elevation_meters_pan = GetElevation(
                     osg::RadiansToDegrees(new_center_viewer_camera.lat),
                     osg::RadiansToDegrees(new_center_viewer_camera.lon));
                  new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
                  new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
                  SetCameraPosition(new_center_local.lat, new_center_local.lon,
                     max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                  parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                  new_center_request = true;
                  action_performed = true;
               }
            } // end handling God's eye view

            if (!fIDevState->control && fIDevState->shift)
            {
                if (!upCommanded && !downCommanded &&
                    !leftCommanded && !rightCommanded)
                {
                    fIDevState->shift = true; // to allow reuse of shift+arrow functionality
                    if (fIDevState->mouse_wheel_change_value > 0.99)
                    {
                        fIDevState->downKeyDown = true;
                        shift_key_override = true;
                        fIDevState->mouse_wheel_change_value = 0.0;
                    }
                    else if (fIDevState->mouse_wheel_change_value < -0.99)
                    {
                        fIDevState->upKeyDown = true;
                        shift_key_override = true;
                        fIDevState->mouse_wheel_change_value = 0.0;
                    }
                }
            }
            if (fIDevState->control && !fIDevState->shift)
            {
                if (fIDevState->mouse_wheel_change_value > 0.99)
                {
                    fIDevState->leftKeyDown = true;
                    control_key_override = true;
                    fIDevState->mouse_wheel_change_value = 0.0;
                }
                else if (fIDevState->mouse_wheel_change_value < -0.99)
                {
                    fIDevState->rightKeyDown = true;
                    control_key_override = true;
                    fIDevState->mouse_wheel_change_value = 0.0;
                }
            }
            if (fIDevState->num_8 || upCommanded ||
                fIDevState->num_2 || downCommanded ||
                fIDevState->num_4 || leftCommanded ||
                fIDevState->num_6 || rightCommanded ||
                fIDevState->north || fIDevState->south ||
                fIDevState->east || fIDevState->west ||
                rotCwCommanded || rotCcwCommanded)
            {
                // now update heading, pitch information
                osg::Vec2d right_vector;
                osg::Vec3d right;
                osg::Vec3d right_local;
                osg::Vec3d forward_planar;
                double incline = 0.0;
                double yaw = 0.0;
                double pitch = 0.0;
                double min_ground_distance = 100.0;
                double shift_speed_check = 1.0;
                osg::Vec3d position;
                osg::Vec3d forward;
                osg::Vec3d up;
                _camera->getViewMatrixAsLookAt(position,forward,up);
                double fovy, view_ratio, p_near, p_far;
                _camera->getProjectionMatrixAsPerspective(fovy, view_ratio, p_near, p_far);
                osg::Vec3d forwardN;
                forwardN.set(forward-position);
                forwardN.normalize();

                ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                   new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters);
                // elevation query to prevent camera from going into ground
                elevation_meters_pan = GetElevation(
                    osg::RadiansToDegrees(new_center_viewer_camera.lat),
                   osg::RadiansToDegrees(new_center_viewer_camera.lon));
                // need ground (0 meters) to get true up vector for this location on earth
                osg::Vec3d position2;
                ellipsoid_model->convertLatLongHeightToXYZ(new_center_viewer_camera.lat, new_center_viewer_camera.lon, 0.0,
                    position2.x(), position2.y(), position2.z());

                up2.set(position-position2);
                msl_meters = up2.length();
                double width_meters = 2*msl_meters*atan(DEG_TO_RAD(22.5));
                up2.normalize();
                incline = min(max(-(90.0 - RAD_TO_DEG(safe_acos(forwardN*up2))), -89.9), 89.9);
                up2.set(position);
                up2.normalize();
                right.set(forwardN^up2);
                right.normalize();
                double local_speed_arc = width_meters*(WGS84_a_METERS/position.length())*
                    (1.0/111320.0); // conversion specific in this case of meters to degrees;
                double frame_time = DeltaTimeSec(old_frame, new_frame);
                //frame_time = time_since_last_frame;
                to_east_vector.set(north_vector^up2);
                to_east_vector.normalize();
                to_north_vector.set(up2^to_east_vector);
                to_north_vector.normalize();
                heading_vector.x() = (forwardN)*(to_east_vector);
                heading_vector.y() = (forwardN)*(to_north_vector);
                heading_vector.normalize();
                if (heading_vector.x() > 0.0) //(*to_north_vector^(*heading_vector))*(*up2) > 0.0
                {
                    heading = RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)));
                }
                else
                {
                    heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)))) + 180.0;
                }
                new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
                new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
                right_local.set(forwardN^up);
                right_local.normalize();
                forward_planar.set(up2^right_local);
                osg::Vec3d positionN;
                positionN.set(position);
                positionN.normalize();
                circlePerpendicularForward.set(positionN^right_local);
                circlePerpendicularForward.normalize();
                circleNormalForward.set(right_local);
                double shift_speed = 1.0;
                if (msl_meters > 1000.0 && ::GetAsyncKeyState(VK_MENU) & 0x8000)
                {
                    shift_speed = map_view->GetGeospatialViewController()->
                        GetGeospatialScene()->m_HUD->m_shift_multiply_speed;
                }
                if (fIDevState->shift && fIDevState->north &&
                    !(fIDevState->south || fIDevState->east || fIDevState->west))
                {
                    new_center_request = true;
                    SetCameraPosition(new_center_local.lat + local_speed_arc*frame_time, new_center_local.lon,
                        max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                    parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    action_performed = true;
                }
                else if (fIDevState->shift && fIDevState->south &&
                    !(fIDevState->north || fIDevState->east || fIDevState->west))
                {
                    new_center_request = true;
                    SetCameraPosition(new_center_local.lat - local_speed_arc*frame_time, new_center_local.lon,
                        max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                    parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    action_performed = true;
                }
                else if (fIDevState->shift && fIDevState->east &&
                    !(fIDevState->north || fIDevState->south || fIDevState->west))
                {
                    new_center_request = true;
                    SetCameraPosition(new_center_local.lat, new_center_local.lon + local_speed_arc*frame_time,
                        max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                    parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    action_performed = true;
                }
                else if (fIDevState->shift && fIDevState->west &&
                    !(fIDevState->north || fIDevState->south || fIDevState->east))
                {
                    new_center_request = true;
                    SetCameraPosition(new_center_local.lat, new_center_local.lon - local_speed_arc*frame_time,
                        max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                    parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                    action_performed = true;
                }

                if (!fIDevState->shift && (fIDevState->num_8 || upCommanded ||
                    fIDevState->num_2 || downCommanded))
                {
                    _camera->getViewMatrixAsLookAt(position,forward,up);
                    ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                       new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters);

                    new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
                    new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
                    if (!fIDevState->control)
                    {   // pan movement forward, backward
                        shift_speed = 1.0;
                        right_vector.x() = (forward_planar)*(to_east_vector)*upCommanded -
                            (forward_planar)*(to_east_vector)*downCommanded;
                        right_vector.y() = (forward_planar)*(to_north_vector)*upCommanded -
                            (forward_planar)*(to_north_vector)*downCommanded;

                        double dx = shift_speed*right_vector.x()*frame_time*local_speed_arc;
                        double dy = shift_speed*right_vector.y()*frame_time*local_speed_arc;
                        double angle = safe_asin(0.5*sqrt(dx*dx + dy*dy)/WGS84_a_METERS);
                        new_center_local.lon += dx;
                        new_center_local.lat += dy;

                        RotationAroundAxis3D(&circleNormalForward, &position, cos(angle),
                                sin(angle));
                        RotationAroundAxis3D(&circleNormalForward, &forward, cos(angle),
                                sin(angle));
                        // after rotation, need to recompute
                        osg::Vec3d forwardN;
                        forwardN.set(forward-position);
                        forwardN.normalize();
                        up2.set(position);
                        up2.normalize();

                        // update heading
                        to_east_vector.set(north_vector^up2);
                        to_east_vector.normalize();
                        to_north_vector.set(up2^to_east_vector);
                        to_north_vector.normalize();
                        heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
                        heading_vector.y() = (forwardN)*(to_north_vector);
                        heading_vector.normalize();
                        if (heading_vector.x() > 0.0)
                        {
                            heading = RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)));
                        }
                        else
                        {
                            heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)))) + 180.0;
                        }

                        SetCameraPosition(new_center_local.lat, new_center_local.lon,
                            max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                        dynamic_cast<GeospatialEarthManipulator*>(parentView->
                            m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                        parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        new_center_request = true;
                        action_performed = true;
                    } // end pan movement forward, backward
                    else
                    {   // local pitch change
                        double direction = 0.0;
                        if (upCommanded || fIDevState->num_8)
                            direction = -1.0;
                        else if (downCommanded || fIDevState->num_2)
                            direction = 1.0;

                        double relative_angle = 0.0;
                        dynamic_cast<GeospatialEarthManipulator*>(parentView->
                            m_camera_earth_manipulator.get())->getLocalEulerAngles(0L, &relative_angle);
                        relative_angle = RAD_TO_DEG(abs(relative_angle));
                        incline += 1.0*frame_time*direction*15.0;
                        incline = max(min(incline, 89.0), 14.0);

                        SetCameraPosition(new_center_local.lat, new_center_local.lon,
                            max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                        dynamic_cast<GeospatialEarthManipulator*>(parentView->
                            m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                        parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        _camera->getViewMatrixAsLookAt(position, forward, up);
                        map_view->NotifyTiltUpdate(ComputeIncline());
                        action_performed = true;
                        new_center_request = true;
                    } // end local pitch change
                }
                if (!fIDevState->shift && (fIDevState->num_4 || leftCommanded ||
                    fIDevState->num_6 || rightCommanded || rotCwCommanded || rotCcwCommanded))
                {
                   _camera->getViewMatrixAsLookAt(position,forward,up);
                    ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                       new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters);
                    new_center_local.lat = RAD_TO_DEG(new_center_viewer_camera.lat);
                    new_center_local.lon = RAD_TO_DEG(new_center_viewer_camera.lon);
                    if (!fIDevState->control && !rotCwCommanded && !rotCcwCommanded)
                    {   // pan movement left, right
                        double shift_speed = shift_speed_check;
                        if (msl_meters < 1000.0)
                        {
                            shift_speed = 1.0;
                        }
                        right_vector.x() = (right_local)*(to_east_vector)*rightCommanded -
                            (right_local)*(to_east_vector)*leftCommanded;
                        right_vector.y() = (right_local)*(to_north_vector)*rightCommanded -
                            (right_local)*(to_north_vector)*leftCommanded;
                        double dx = shift_speed*right_vector.x()*frame_time*local_speed_arc;
                        double dy = shift_speed*right_vector.y()*frame_time*local_speed_arc;
                        double angle = safe_asin(0.5*sqrt(dx*dx + dy*dy)/WGS84_a_METERS);
                        new_center_local.lon += dx;
                        new_center_local.lat += dy;

                        RotationAroundAxis3D(&circlePerpendicularForward, &position, cos(angle),
                            sin(angle));
                        RotationAroundAxis3D(&circlePerpendicularForward, &forward, cos(angle),
                            sin(angle));
                        // after rotation, need to recompute
                        osg::Vec3d forwardN;
                        forwardN.set(forward-position);
                        forwardN.normalize();
                        up2.set(position);
                        up2.normalize();

                        // update heading
                        to_east_vector.set(north_vector^up2);
                        to_east_vector.normalize();
                        to_north_vector.set(up2^to_east_vector);
                        to_north_vector.normalize();
                        heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
                        heading_vector.y() = (forwardN)*(to_north_vector);
                        heading_vector.normalize();
                        if (heading_vector.x() > 0.0)
                        {
                            heading = RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)));
                        }
                        else
                        {
                            heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)))) + 180.0;
                        }

                        SetCameraPosition(new_center_local.lat, new_center_local.lon,
                            max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                        dynamic_cast<GeospatialEarthManipulator*>(parentView->
                            m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                        parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        new_center_request = true;
                        action_performed = true;
                    } // end pan movement left, right
                    else if (!control_key_override)
                    {   // local heading change
                        double direction = 0.0;
                        if (fIDevState->num_4 || leftCommanded || rotCcwCommanded)
                            direction = -1.0;
                        else if (fIDevState->num_6 || rightCommanded || rotCwCommanded)
                            direction = 1.0;

                        heading += 1.0*frame_time*direction*15.0;
                        if (heading < 0.0)
                        {
                            heading += 360.0;
                        }
                        else if (heading > 360.0)
                        {
                            heading -= 360.0;
                        }
                        new_center_request = true;

                        SetCameraPosition(new_center_local.lat, new_center_local.lon,
                            max(alt_meters, ((double)elevation_meters_pan)+min_ground_distance), heading, incline, 0.0);
                        dynamic_cast<GeospatialEarthManipulator*>(parentView->
                            m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                        parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                        action_performed = true;
                    } // end local heading change
                }
            } // end handling of items that don't depend on center of screen
            osg::Vec3d right_vector(0,0,0);
            osg::Vec3d pos_up(0,0,0);
            double focus_angle = 0.0;
            double focus_angle_vert = 0.0;
            osg::Vec3d position;
            osg::Vec3d forward;
            osg::Vec3d up;
            _camera->getViewMatrixAsLookAt(position,forward,up);
            if ((fIDevState->shift || northUpCommanded ||
                  fIDevState->top_down_view || control_key_override)
                  && !shift_first_frame && !target_location && loc_world.length() < 0.01)
            {
                  shift_first_frame = true;
                  target_location = true;
            }
            else
            {
               if (shift_first_frame)
                  shift_first_frame = false;
               if (!fIDevState->shift)
               {
                  target_location = false;
                  loc_world.set(0,0,0);
               }
            }
            if ((!fIDevState->control || control_key_override)
                && (fIDevState->shift || northUpCommanded ||
                fIDevState->top_down_view || control_key_override))
            {
                if (fIDevState->num_4 || leftCommanded ||
                    fIDevState->num_6 || rightCommanded ||
                    fIDevState->num_8 || upCommanded ||
                    fIDevState->num_2 || downCommanded ||
                    northUpCommanded || fIDevState->top_down_view ||
                    fIDevState->handled_shift)
                {
                    if (target_location)
                    {
                        osgUtil::LineSegmentIntersector::Intersections mid_screen;
                        computeIntersections(getCamera()->getViewport()->width()/2.0,
                            getCamera()->getViewport()->height()/2.0, mid_screen);
                        if (!mid_screen.empty())
                        {
                            int firstOnly = 0;
                            for (osgUtil::LineSegmentIntersector::Intersections::const_iterator iter = mid_screen.begin();
                                firstOnly < 1;
                                firstOnly++)
                            {
                                double height = 0.0;
                                loc_world.set(iter->getWorldIntersectPoint());
                                ellipsoid_model->convertXYZToLatLongHeight(loc_world.x(), loc_world.y(), loc_world.z(),
                                   new_center_viewer.lat, new_center_viewer.lon, height);
                                osg::Vec3d positionN;
                                positionN.set(loc_world);
                                positionN.normalize();
                                loc_world_perp.set(up^positionN);
                                loc_world_perp.normalize();
                                osg::Vec3d forwardN;
                                forwardN.set(position);
                                forwardN.normalize();
                                if (forwardN*positionN > -0.2)
                                {
                                    focus_angle = 0.0;
                                    target_location = false;
                                }
                                else
                                {
                                   loc_world.set(0,0,0);
                                   loc_world_perp.set(0,0,0);
                                }
                            }
                        }
                    } // end compute target location
                    if (loc_world.length() > 0)
                    {
                        osg::Vec3d positionN;
                        positionN.set(loc_world);
                        positionN.normalize();
                        osg::Vec3d distance_vec(position-loc_world);
                        CheckPivotCameraUsingPerp(&loc_world, &loc_world_perp, &distance_vec);
                        // left, right before up, down
                        if (fIDevState->num_4 || leftCommanded ||
                            fIDevState->num_6 || rightCommanded ||
                            northUpCommanded)
                        {
                            if (fIDevState->num_4 || leftCommanded)
                            {
                                //focus_angle = GetTimeSinceLastUpdate()/1.33;
                                focus_angle = DeltaTimeSec(old_frame, new_frame);
                            }
                            if (fIDevState->num_6 || rightCommanded)
                            {
                                //focus_angle = -GetTimeSinceLastUpdate()/1.33;
                                focus_angle = -DeltaTimeSec(old_frame, new_frame);
                            }
                            if (northUpCommanded)
                            {
                                osg::Vec3d positionNL(position);
                                positionNL.normalize();
                                new_center_request = true;

                                // update heading
                                to_east_vector.set(north_vector^positionNL);
                                to_east_vector.normalize();
                                to_north_vector.set(positionNL^to_east_vector);
                                to_north_vector.normalize();
                                osg::Vec3d forwardN;
                                forwardN.set(forward-position);
                                heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
                                heading_vector.y() = (forwardN)*(to_north_vector);
                                heading_vector.normalize();
                                if (heading_vector.x() > 0.0) // 0 <= heading < 180
                                {
                                    heading = RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)));
                                    focus_angle = DeltaTimeSec(old_frame, new_frame);
                                    if (heading - RAD_TO_DEG(focus_angle) < 0.0)
                                    {
                                        focus_angle = DEG_TO_RAD(heading);
                                    }
                                }
                                else // 180 <= heading <= 360
                                {
                                    heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)))) + 180.0;
                                    focus_angle = -DeltaTimeSec(old_frame, new_frame);
                                    if (heading - RAD_TO_DEG(focus_angle) > 360.0)
                                    {
                                        focus_angle = -DEG_TO_RAD(360.0-heading);
                                    }
                                }
                            }

                            pos_up.set(position+up);
                            RotationAroundAxis3D(&positionN, &position, cos(focus_angle), sin(focus_angle));
                            RotationAroundAxis3D(&positionN, &pos_up, cos(focus_angle), sin(focus_angle));

                            _camera->setViewMatrixAsLookAt(position, loc_world, pos_up-position);
                            dynamic_cast<GeospatialEarthManipulator*>(parentView->
                                m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                            parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                            action_performed = true;
                        }
                        if (fIDevState->num_8 || upCommanded ||
                            fIDevState->num_2 || downCommanded ||
                            fIDevState->top_down_view)
                        {
                            osg::Vec3d position2;
                            _camera->getViewMatrixAsLookAt(position2,forward2,up2);
                            if (fIDevState->num_8 || upCommanded || fIDevState->top_down_view)
                            {
                                //focus_angle = GetTimeSinceLastUpdate()/1.33;
                                focus_angle = -min(max(DeltaTimeSec(old_frame, new_frame), 0.04), 0.25)*1.5;
                            }
                            if (fIDevState->num_2 || downCommanded)
                            {
                                //focus_angle = -GetTimeSinceLastUpdate()/1.33;
                                focus_angle = min(max(DeltaTimeSec(old_frame, new_frame), 0.04), 0.25);
                            }
                            pos_up.set(position2+up2-loc_world);
                            position2.operator-=(loc_world);

                            loc_world_perp.set(up2^positionN);
                            loc_world_perp.normalize();
                            loc_world_norm.set(loc_world_perp^positionN);
                            loc_world_norm.normalize();

                            RotationAroundAxis3D(&loc_world_perp, &position2, cos(focus_angle), sin(focus_angle));
                            RotationAroundAxis3D(&loc_world_perp, &pos_up, cos(focus_angle), sin(focus_angle));
                            up2.set(pos_up-position2);
                            up2.normalize();

                            //double testB = up2*positionN;
                            if (up2*positionN > cos(DEG_TO_RAD(14.0)))
                            {
                                focus_angle = DEG_TO_RAD(14.0);

                                loc_world_norm.operator*=(position2.length());
                                RotationAroundAxis3D(&loc_world_perp, &loc_world_norm, cos(-focus_angle), sin(-focus_angle));
                                positionN.set(loc_world_norm-loc_world);
                                positionN.normalize();
                                position2.set(loc_world_norm + loc_world);
                                up2.set(loc_world_perp^positionN);
                                up2.normalize();
                                fIDevState->last_frame_movement = false;
                            }
                            else if  (up2*positionN < cos(DEG_TO_RAD(89.0)))
                            {
                                focus_angle = DEG_TO_RAD(89.0);

                                loc_world_norm.operator*=(position2.length());
                                RotationAroundAxis3D(&loc_world_perp, &loc_world_norm, cos(-focus_angle), sin(-focus_angle));
                                positionN.set(loc_world_norm-loc_world);
                                positionN.normalize();
                                position2.set(loc_world_norm + loc_world);
                                up2.set(loc_world_perp^positionN);
                                up2.normalize();
                                fIDevState->last_frame_movement = false;
                            }
                            else
                            {
                                int incline = ComputeIncline();
                                map_view->NotifyTiltUpdate(incline);
                                position2.operator+=(loc_world);
                                fIDevState->last_frame_movement = true;
                            }
                            if (fIDevState->last_frame_movement)
                            {
                               _camera->setViewMatrixAsLookAt(position2, loc_world, up2);
                               dynamic_cast<GeospatialEarthManipulator*>(parentView->
                                   m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                               parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                               OSGUserVars::SetAltitude(ComputeAltitude());
                               action_performed = true;
                               if (control_key_override || shift_key_override)
                                   requestRedraw();
                            }
                        } // end arrow key handling
                    } // end pivot handling
                } // end handling of keys that depend on location in center of screen
            }
            if (fIDevState->moveUp || fIDevState->moveDown)
            {
                _camera->getViewMatrixAsLookAt(position,forward,up);
                osg::Vec3d forwardN;
                forwardN.set(forward-position);
                forwardN.normalize();
                ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                    new_center_viewer_camera.lat, new_center_viewer_camera.lon, alt_meters);
                double time_elapsed = DeltaTimeSec(old_frame, new_frame);
                double alt_speed_check = 1.0;

                // elevation query to prevent camera from going into ground
                elevation_meters_pan = GetElevation(
                    osg::RadiansToDegrees(new_center_viewer_camera.lat),
                    osg::RadiansToDegrees(new_center_viewer_camera.lon));
                osg::Vec3d position2;
                ellipsoid_model->convertLatLongHeightToXYZ(new_center_viewer_camera.lat, new_center_viewer_camera.lon, 0.0,
                    position2.x(), position2.y(), position2.z());
                up2.set(position-position2);
                msl_meters = up2.length();


                if (msl_meters > 1000.0 && ::GetAsyncKeyState(VK_MENU) & 0x8000)
                {
                    alt_speed_check = map_view->GetGeospatialViewController()->
                        GetGeospatialScene()->m_HUD->m_shift_multiply_speed;
                }
                alt_meters += alt_speed_check*(fIDevState->move_speed_vertical*fIDevState->
                    moveUp*time_elapsed - fIDevState->move_speed_vertical*fIDevState->
                    moveDown*time_elapsed);

                up2.normalize();
                double incline = min(max(-(90.0 - RAD_TO_DEG(safe_acos(forwardN*up2))), -89.9), 89.9);
                double heading = ComputeHeading();
                SetCameraPosition(osg::RadiansToDegrees(new_center_viewer_camera.lat),
                   osg::RadiansToDegrees(new_center_viewer_camera.lon), max(alt_meters, 
                   ((double)elevation_meters_pan)+min_ground_distance),heading,incline,0.0);
                dynamic_cast<GeospatialEarthManipulator*>(parentView->
                    m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
                parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
                OSGUserVars::SetAltitude(ComputeAltitude());
                new_center_request = true;
                action_performed = true;
            }

            if (action_performed || fIDevState->mouse_moved_pressed)
            {
               if (fIDevState->mouse_moved_pressed)
               {
                  new_center_request = true;
                  fIDevState->mouse_moved_pressed = false;
                  /*
                  if (map_view->m_has_focus)
                  {
                     new_center_request = true;
                  }
                  else
                  {
                     // possible for pressed to still be true if
                     // falconview loses focus and OnLButtonUp does
                     // not execute
                     fIDevState->pressed = false;
                  }
                  fIDevState->mouse_moved_pressed = false;
                  */
               }
               HUD* hud = map_view->GetGeospatialViewController()->GetGeospatialScene()->
                  m_HUD;
               hud->m_heading = ComputeHeading();
               hud->m_msl_alt_value = METERS_TO_FEET(ComputeAltitudeMSL());
               OSGUserVars::SetAltitude(hud->m_msl_alt_value);
               hud->m_agl_alt_value = METERS_TO_FEET(ComputeAltitude());
               requestRedraw();
            }

            // handle logic that is waiting for a new frame, often caused
            // by transitions, throws, etc...
            if (parentView->m_camera_earth_manipulator->frames_to_redraw > 0)
            {
               ellipsoid_model->convertXYZToLatLongHeight(position.x(), position.y(), position.z(),
                  new_center_viewer_camera.lat, new_center_viewer_camera.lon, msl_meters);
               elevation_meters_pan = GetElevation(RAD_TO_DEG(new_center_viewer_camera.lat),
                  RAD_TO_DEG(new_center_viewer_camera.lon));
               map_view->GetGeospatialViewController()->GetGeospatialScene()->
                  m_HUD->m_heading = ComputeHeading();
               map_view->GetGeospatialViewController()->GetGeospatialScene()->
                    m_HUD->m_msl_alt_value = METERS_TO_FEET(msl_meters);
               map_view->GetGeospatialViewController()->GetGeospatialScene()->
                  m_HUD->m_agl_alt_value =
                  METERS_TO_FEET(msl_meters - elevation_meters_pan);
               requestRedraw();
            }

        } // end allow new arrow key input
   } // end earth manipualtor PAN

   if (fIDevState->throwing) // FOCUS mode throws do not enable the fIDevState->throwing flag
   { //  Animate throws in pan mode after mouse click release

      GetSystemTime(&newTime);
      double timeElapsed = DeltaTimeSec(oldTime, newTime);
      //timeElapsed = max(timeElapsed, 0.001);
      timeTotal = max(timeTotal+0.001,timeTotal+timeElapsed); // Ensures time continues moving
      if (!arrowKeyThrow)
      {
         elapsedPosition += timeElapsed*angularVelocity;
         angularVelocity = startingAngularVelocity*(pow(timeTotal-elapsedPositionDurationMouse, 2));
         if (fIDevState->space) // stops a throw
             angularVelocity = 0.0;
         if (angularVelocity<0.0001 || timeTotal > elapsedPositionDurationMouse)
         {
            timeElapsed = 0;
            fIDevState->throwing = false;
            timeTotal = 0;
            elapsedPosition = 0;
            GetSystemTime(&old_frame);
            return;
         }
      }
      else
      {
         if (leftCommanded || rightCommanded
               || upCommanded || downCommanded)
         {
            if (elapsedPositionDurationArrow > 0.0)
               elapsedPosition = timeTotal*angularVelocity/elapsedPositionDurationArrow;
            if (timeTotal>=elapsedPositionDurationArrow)
            {
               elapsedPosition = angularVelocity;
            }
            if (timeTotal>=elapsedPositionDurationArrow)
            {
               mandatoryFrameRedraw--;
            }
         }
      }
      if ((upCommanded || downCommanded) &&
         !fIDevState->complexArrowKeyThrow) // simple arrow key throw
      {
         positionThrowStart = positionThrowEnd.operator*(cos(elapsedPosition))
            + circlePerpendicularPosition.operator*(sin(elapsedPosition));

         forwardThrowStart = forwardRadial.operator*(cos(elapsedPosition))
            + circlePerpendicularForward.operator*(sin(elapsedPosition));

         upThrowStart = upRadial.operator*(cos(elapsedPosition))
            + circlePerpendicularUp.operator*(sin(elapsedPosition));
      }
      else if ((leftCommanded || rightCommanded) &&
         !fIDevState->complexArrowKeyThrow) // simple arrow key throw
      {
         positionThrowStart = positionThrowEnd.operator*(cos(elapsedPosition))
            + circlePerpendicularPosition.operator*(sin(elapsedPosition));

         forwardThrowStart = forwardRadial.operator*(cos(elapsedPosition))
            + circlePerpendicularForward.operator*(sin(elapsedPosition))
            + circleNormalPosition.operator*(height2);

         upThrowStart = upRadial.operator*(cos(elapsedPosition))
            + circlePerpendicularUp.operator*(sin(elapsedPosition))
            + circleNormalPosition.operator*(height1);
      }
      else if (fIDevState->complexArrowKeyThrow) // complex arrow key throw
      {
         positionThrowStart = positionThrowEnd.operator*(cos(elapsedPosition))
            + circlePerpendicularPosition.operator*(sin(elapsedPosition));

         forwardThrowStart = forwardRadial.operator*(cos(elapsedPosition))
            + circlePerpendicularForward.operator*(sin(elapsedPosition))
            + circleNormalPosition.operator*(height2);

         upThrowStart = upRadial.operator*(cos(elapsedPosition))
            + circlePerpendicularUp.operator*(sin(elapsedPosition))
            + circleNormalPosition.operator*(height1);
      }
      else if (fIDevState->throwing) // non arrow key throw
      {
         positionThrowStart = positionThrowEnd.operator*(cos(elapsedPosition))
            + circlePerpendicularPosition.operator*(sin(elapsedPosition));

         forwardThrowStart = forwardRadial.operator*(cos(elapsedPosition))
            + circlePerpendicularForward.operator*(sin(elapsedPosition))
            + circleNormalPosition.operator*(height2);

         upThrowStart = upRadial.operator*(cos(elapsedPosition))
            + circlePerpendicularUp.operator*(sin(elapsedPosition))
            + circleNormalPosition.operator*(height1);
      }
      if (arrowKeyThrow && (timeTotal>=elapsedPositionDurationArrow))
      {
         if(mandatoryFrameRedraw == 0)
         {
            mandatoryFrameRedraw = 2;
            fIDevState->complexArrowKeyThrow = false;
            timeTotal = 0;
            elapsedPosition = 0;
            fIDevState->throwing = false;
            arrowKeyThrow = false;
            ResetArrowKeyState(fIDevState);
         }
      }

      if (fIDevState->throwing)
      {
         upThrowStart = upThrowStart - positionThrowStart;
         upThrowStart.normalize();
         _camera->setViewMatrixAsLookAt(positionThrowStart,forwardThrowStart,upThrowStart);
         dynamic_cast<GeospatialEarthManipulator*>(parentView->
             m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
         parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
         GetSystemTime(&oldTime);
         map_view->GetGeospatialViewController()->GetGeospatialScene()->
            m_HUD->m_heading = ComputeHeading();
      }
      else // just stopped throwing right now
      {
         ResetAnimationVariables(&elapsedPosition, &angularVelocity,
            &timeTotal);
         ResetArrowKeyState(fIDevState);
         fIDevState->complexArrowKeyThrow = false;
         map_view->GetGeospatialViewController()->GetGeospatialScene()->
            m_HUD->m_heading = ComputeHeading();
      } // end of just stopped throwing
   } // end throwing handling
   else // not throwing
   {
      // reset keyboard state depending on camera mode
      ResetGlobalMovementState(fIDevState->GetActiveCam(), fIDevState,
         &elapsedPosition, &angularVelocity, &timeTotal);
   }

   if (!arrowKeyThrow &&
      (fIDevState->throwing) &&
      fIDevState->GetActiveCam() != fvw::CAMERA_MODE_GROUND_VIEW)
   {
      new_center_request= true;
   }

   if (new_center_request)
   {
      if (frame_of_update != frame_number_new)
      {
         frame_of_update = frame_number_new;
         if (fIDevState->GetActiveCam() == fvw::CAMERA_MODE_PAN)
         {
            osgUtil::LineSegmentIntersector::Intersections mid_screen;
            computeIntersections(getCamera()->getViewport()->width()/2.0,
                  getCamera()->getViewport()->height()/2.0, mid_screen);
            if (!mid_screen.empty())
            {
               osg::Vec3d position, forward, up;
               _camera->getViewMatrixAsLookAt(position, forward, up);
               int firstOnly = 0;
               for (osgUtil::LineSegmentIntersector::Intersections::const_iterator iter = mid_screen.begin();
                  firstOnly < 1;
                  firstOnly++)
               {
                  target_location = false;
                  double height = 0.0;
                  loc_world.set(iter->getWorldIntersectPoint());
                  osg::Vec3d positionN;
                  positionN.set(loc_world);
                  positionN.normalize();
                  osg::Vec3d forwardN;
                  forwardN.set(position);
                  forwardN.normalize();
                  if (forwardN*positionN > -0.2)
                  {
                     ellipsoid_model->convertXYZToLatLongHeight(loc_world.x(), loc_world.y(), loc_world.z(),
                        new_center_viewer.lat, new_center_viewer.lon, height);
                     UpdateCenter(new_center_viewer, fIDevState->GetActiveCam());
                  }
               }
            } // end compute target location
         }
         else
         {
            UpdateCenter(new_center_viewer_camera, fIDevState->GetActiveCam());
         }
      }
      else
      {
         if (fIDevState->GetActiveCam() == fvw::CAMERA_MODE_PAN)
         {
            UpdateCenter(new_center_viewer, fIDevState->GetActiveCam());
         }
         else
         {
            UpdateCenter(new_center_viewer_camera, fIDevState->GetActiveCam());
         }
      }
   }
   GetSystemTime(&old_frame);
#ifdef DEBUG_CAMERA
   map_view->GetGeospatialViewController()->GetGeospatialScene()->
      debug_cam->setViewMatrix(_camera->getViewMatrix());
#endif
}

double xx = 0.0;

void GeospatialViewer::CheckPivotCameraUsingPerp(osg::Vec3d* target, osg::Vec3d* perp, osg::Vec3d* distance)
{
   osg::Vec3d position, forward, up, pos_up, positionN, bearing_vector;
   _camera->getViewMatrixAsLookAt(position, forward, up);
   positionN.set(position);
   positionN.normalize();
   fv_common::ScopedCriticalSection lock(&fvw_get_view()->
      GetGeospatialViewController()->m_viewpoint_critical_section);
   double angle = DEG_TO_RAD(min(max(fIDevState->tilt_value, -89), -15));
   if (fIDevState->handled_shift)
   {
      // compute bearing vector at 0 degrees first
      bearing_vector.set(*perp^positionN);
      bearing_vector.normalize();
      bearing_vector.operator*=(distance->length());
      pos_up.set(positionN);
      // rotate bearing vector
      RotationAroundAxis3D(perp, &bearing_vector, cos(angle), sin(angle));
      RotationAroundAxis3D(perp, &pos_up, cos(angle), sin(angle));
      position.set(*target+bearing_vector);
      pos_up.normalize();
      forward.set(*target-position);
      forward.normalize();
      forward.operator+=(position);
      _camera->setViewMatrixAsLookAt(position, forward, pos_up);
      dynamic_cast<GeospatialEarthManipulator*>(parentView->
         m_camera_earth_manipulator.get())->no_intersection_matrix_set = true;
      parentView->m_camera_earth_manipulator->setByInverseMatrix(_camera->getViewMatrix());
      action_performed = true;
      HUD* hud = fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->GetHud();
      hud->m_agl_alt_value = METERS_TO_FEET(ComputeAltitude());
      hud->m_msl_alt_value = METERS_TO_FEET(ComputeAltitudeMSL());
      fIDevState->handled_tilt = true;
   }
}

void GeospatialViewer::EstimateViewpointUsingCamera(osgEarth::Viewpoint* view)
{
   //@TODO, starting point for loading tiles while looking above horizon
}

double GeospatialViewer::ComputeAltitude()
{
   double msl_temp;
   double lat_temp;
   double lon_temp;
   GeospatialViewer::GetCameraPosition(&lat_temp,
   &lon_temp, &msl_temp);

   double ground_temp = GetElevation(lat_temp, 
   lon_temp);
   return msl_temp - ground_temp;
}

double GeospatialViewer::ComputeAltitudeMSL()
{
   osg::Vec3d eye, center, up;
      _camera->getViewMatrixAsLookAt(eye, center, up);
   double inner_lat, inner_lon, inner_alt_meters;
   fIDevState->GetSRS()->getEllipsoid()->convertXYZToLatLongHeight(eye.x(),
      eye.y(), eye.z(), inner_lat, inner_lon, inner_alt_meters);
   return inner_alt_meters;
}

double GeospatialViewer::ComputeDistanceToCurrentLocation()
{
   double msl_temp = 0.0;
   double lat_temp = 0.0;
   double lon_temp = 0.0;
   GeospatialViewer::GetCameraPosition(&lat_temp,
   &lon_temp, &msl_temp);

   if (msl_temp == 0.0 && lat_temp == 0.0 && lon_temp == 0.0)
      return 0.0;
   // if pan
   if (fIDevState->GetActiveCam() == fvw::CAMERA_MODE_PAN)
   {
      lat_temp = RAD_TO_DEG(new_center_viewer.lat);
      lon_temp = RAD_TO_DEG(new_center_viewer.lon);
   }

   double ground_temp = GetElevation(lat_temp, 
   lon_temp);
   return msl_temp - ground_temp;
}

double GeospatialViewer::ComputeHeading()
{
    osg::Vec3d pos,forw,up;
    _camera->getViewMatrixAsLookAt(pos,forw,up);
    osg::Vec3d positionNL(pos);
    positionNL.normalize();
    osg::Vec2d heading_vector;
    osg::Vec2d vec2d_north(0.0,1.0);
    double heading = 0.0;

    // update heading
    to_east_vector.set(north_vector^positionNL);
    to_east_vector.normalize();
    to_north_vector.set(positionNL^to_east_vector);
    to_north_vector.normalize();
    osg::Vec3d forwardN;
    forwardN.set(forw-pos);
    heading_vector.x() = (forwardN)*(to_east_vector); //recompute east and north
    heading_vector.y() = (forwardN)*(to_north_vector);
    heading_vector.normalize();
    if (heading_vector.x() > 0.0) // 0 <= heading < 180
    {
        heading = RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)));
    }
    else // 180 <= heading <= 360
    {
        heading = (180.0 - RAD_TO_DEG(safe_acos(vec2d_north*(heading_vector)))) + 180.0;
    }
    heading = min(max(heading, 0.0), 359.99999);
    return heading;
}

double GeospatialViewer::ComputeIncline()
{
    osg::Vec3d pos,forw,up;
    _camera->getViewMatrixAsLookAt(pos,forw,up);
    osg::Vec3d forwardN(forw-pos);
    forwardN.normalize();
    double incline = 0.0;
    const osg::EllipsoidModel* ellipsoid_model = fIDevState->GetSRS()->getEllipsoid();
    osg::Vec3d positionN(ellipsoid_model->computeLocalUpVector(pos.x(), pos.y(), pos.z()));
    incline = 90.0 - RAD_TO_DEG(safe_acos(positionN*forwardN));
    return incline;
}

void GeospatialViewer::GetCameraPosition(double* lat, double* lon,
   double* alt_meters /*, double heading, double incline, double roll*/)
{
    osg::Vec3d eye, center, up;
    _camera->getViewMatrixAsLookAt(eye, center, up);
    if (eye.length() < 0.01)
    {
       //likely the camera has not been initialized
       *lat = 0;
       *lon = 0;
       *alt_meters = 0;
       return;
    }
    double inner_lat, inner_lon, inner_alt_meters;
    fIDevState->GetSRS()->getEllipsoid()->convertXYZToLatLongHeight(eye.x(),
       eye.y(), eye.z(), inner_lat, inner_lon, inner_alt_meters);

    *lat = RAD_TO_DEG(inner_lat);
    *lon = RAD_TO_DEG(inner_lon);
    *alt_meters = inner_alt_meters;
}

void GeospatialViewer::SetCameraPosition(double lat, double lon,
   double alt_meters, double heading, double incline, double roll)
{
   OSGUserVars::SetCameraLatLon(lat, lon);
   _camera->setViewMatrix(camera_utils::ViewMatrixFromGeo(fIDevState->GetSRS(),
      lat, lon, alt_meters, heading, incline, roll));
}

void GeospatialViewer::updateTraversal()
{
   /*
    if (fIDevState->gvc->GetGeospatialScene()->viewshed != nullptr)
       fIDevState->gvc->GetGeospatialScene()->viewshed->Update();
    */

    if (_done) return;

    double beginUpdateTraversal = osg::Timer::instance()->delta_s(_startTick, osg::Timer::instance()->tick());

    _updateVisitor->reset();
    _updateVisitor->setFrameStamp(getFrameStamp());
    _updateVisitor->setTraversalNumber(getFrameStamp()->getFrameNumber());

    _scene->updateSceneGraph(*_updateVisitor);

    // if we have a shared state manager prune any unused entries
    if (osgDB::Registry::instance()->getSharedStateManager())
        osgDB::Registry::instance()->getSharedStateManager()->prune();

    // update the Registry object cache.
    osgDB::Registry::instance()->updateTimeStampOfObjectsInCacheWithExternalReferences(*getFrameStamp());
    osgDB::Registry::instance()->removeExpiredObjectsInCache(*getFrameStamp());

    if (_updateOperations.valid())
    {
        _updateOperations->runOperations(this); //graphics operations
    }

    if (_incrementalCompileOperation.valid())
    {
        // merge subgraphs that have been compiled by the incremental compiler operation.
        _incrementalCompileOperation->mergeCompiledSubgraphs(getFrameStamp());
    }

    {
        // Do UpdateTraversal for slaves with their own subgraph
        for(unsigned int i=0; i<getNumSlaves(); ++i)
        {
            osg::View::Slave& slave = getSlave(i);
            osg::Camera* camera = slave._camera.get();
            if(camera && !slave._useMastersSceneData)
            {
                camera->accept(*_updateVisitor);
            }
        }
    }

    {
        // call any camera update callbacks, but only traverse that callback, don't traverse its subgraph
        // leave that to the scene update traversal.
        osg::NodeVisitor::TraversalMode tm = _updateVisitor->getTraversalMode();
        _updateVisitor->setTraversalMode(osg::NodeVisitor::TRAVERSE_NONE);

        _camera->getUpdateCallback();
        if (_camera.valid() && _camera->getUpdateCallback())
        {
           _camera->accept(*_updateVisitor);
        }
        for(unsigned int i=0; i<getNumSlaves(); ++i)
        {
            osg::View::Slave& slave = getSlave(i);
            osg::Camera* camera = slave._camera.get();
            if (camera && slave._useMastersSceneData && camera->getUpdateCallback())
            {
                camera->accept(*_updateVisitor);
            }
        }
        _updateVisitor->setTraversalMode(tm);
    }

    MapView* map_view = fvw_get_view();
    if (_cameraManipulator.valid())
    {
       setFusionDistance( getCameraManipulator()->getFusionDistanceMode(),
          getCameraManipulator()->getFusionDistanceValue() );

       // Determine if the camera is attached to an entity. If so, use its
       // camera position
       bool camera_set = false;
       if (map_view != nullptr)
       {
          MapView::AttachedCameraParams* camera_params =
             map_view->GetCameraPositionIfAttached();
          if (camera_params)
          {
             // Get current position and convert to proper view matrix
             SetCameraPosition(camera_params->lat, camera_params->lon,
                camera_params->alt_meters, camera_params->heading,
                camera_params->pitch, camera_params->roll);
             camera_set = true;
          }
       }

       if ((!fIDevState->animating || fIDevState->transition_animation) && !camera_set)
       {
          osg::Vec3d position;
          osg::Vec3d forward;
          osg::Vec3d up;
          _camera->getViewMatrixAsLookAt(position, forward, up);
          _cameraManipulator->updateCamera(*_camera);
          if (fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->
               m_HUD->m_HUD_update_request)
          {
             fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->
               m_HUD->m_HUD_update_request = false;
             parentView->m_camera_earth_manipulator->frames_to_redraw = 2;
          }
          _camera->getViewMatrixAsLookAt(position, forward, up);
       }
    }

    // Update camera position indicators with new camera eyepoint
    osg::Vec3d eye, center, up_local;
    _camera->getViewMatrixAsLookAt(eye, center, up_local);
    double lat, lon, height;
    fIDevState->GetSRS()->getEllipsoid()->convertXYZToLatLongHeight(eye.x(),
       eye.y(), eye.z(), lat, lon, height);

    d_geo_t camera_location = { RAD_TO_DEG(lat), RAD_TO_DEG(lon) };

    DisplayElementRootNode::UpdateCameraPositionIndicators(camera_location,
       _camera->getViewMatrix());

    // Set the camera's altitude in the fvDevState. This is used by the flight
    // mode's automatic MapType handler
    fIDevState->SetCameraPosition(camera_location, height);

    updateSlaves();

    if (getViewerStats() && getViewerStats()->collectStats("update"))
    {
        double endUpdateTraversal = osg::Timer::instance()->delta_s(_startTick, osg::Timer::instance()->tick());

        // update current frames stats
        getViewerStats()->setAttribute(_frameStamp->getFrameNumber(), "Update traversal begin time", beginUpdateTraversal);
        getViewerStats()->setAttribute(_frameStamp->getFrameNumber(), "Update traversal end time", endUpdateTraversal);
        getViewerStats()->setAttribute(_frameStamp->getFrameNumber(), "Update traversal time taken", endUpdateTraversal-beginUpdateTraversal);
    }

    // Now that we are done with the update traversal, store off the camera's
    // viewpoint for use in other locations.
    if (map_view != nullptr)
    {
       map_view->GetGeospatialViewController()->UpdateViewpoint3D();
    }
}

// TODO: This should only be done when the cursor position changes
void GeospatialViewer::ScenegraphIntersections()
{
   osgViewer::View* view = this;
   bool valid_lat_lon = false;
   d_geo_t location;
   CString location_text, secondary_location_text;
   osgUtil::LineSegmentIntersector::Intersections intersections;
   osg::Vec3d intersectWorld;
   osg::Vec3d geo;

   // If the cursor position didn't change there is nothing to do
   static float last_mouse_x = -1.0;
   static float last_mouse_y = -1.0;
   if (fIDevState->mouseX == last_mouse_x &&
       fIDevState->mouseY == last_mouse_y)
   {
      return;
   }
   last_mouse_x = fIDevState->mouseX;
   last_mouse_y = fIDevState->mouseY;

   double local_y = getCamera()->getViewport()->height() - fIDevState->mouseY;

   // If the position of the mouse is over the globe we will populate the
   // location text with the corresponding geographic coordinates
   if (view->computeIntersections(fIDevState->mouseX, local_y, intersections, 0xffffffff))
   {
      {
        using namespace osgUtil;
        using namespace osg;
        int item = 0;
        // The first intersection of earth is correct since
        // the earth is convex (sphere)
        for (LineSegmentIntersector::Intersections::const_iterator iter = intersections.begin();
            item < 1;
            item++)
        {
            intersectWorld = iter->getWorldIntersectPoint();
        }
      }
      if (fIDevState->GetSRS()->transformFromWorld(intersectWorld, geo))
      {
         valid_lat_lon = true;
         location.lat = geo.y();
         location.lon = geo.x();
         //fIDevState->action_center = location;

         // Convert the geographic coordinates to a string
         char str[GEO_MAX_LAT_LON_STRING];
         GEO_lat_lon_to_string(location.lat, location.lon, str,
            GEO_MAX_LAT_LON_STRING);
         location_text = str;

         // Convert the geographic coordinates into the secondary display format
         //
         char display[PRM_MAX_VALUE_LENGTH+1];
         GEO_get_default_display(display, PRM_MAX_VALUE_LENGTH+1);

         // Temporarily swap to the other format
         if (strcmp(display, "PRIMARY") == 0)
            GEO_set_default_display( "SECONDARY" );
         else
            GEO_set_default_display( "PRIMARY" );

         GEO_lat_lon_to_string(location.lat, location.lon, str,
            GEO_MAX_LAT_LON_STRING);
         secondary_location_text = str;

         // swap back to original format
         if (strcmp(display, "PRIMARY") == 0)
            GEO_set_default_display( "PRIMARY" );
         else
            GEO_set_default_display( "SECONDARY" );
      }
   }

   CMainFrame* frame = fvw_get_frame();
   CTextInfoPane *location_text_pane =
      static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(
      STATUS_BAR_PANE_CURSOR_LOCATION));

   CTextInfoPane *secondary_location_text_pane =
      static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(
         STATUS_BAR_PANE_SECONDARY_CURSOR_LOCATION));

   if (location_text_pane && secondary_location_text_pane)
   {
      new fvw::UIThreadOperation([=]()
      {
         // Set the location pane's text
         location_text_pane->SetText(location_text);
         secondary_location_text_pane->SetText(secondary_location_text);

         MapView* view = fvw_get_view();
         if (view)
            view->SetCursorPositionGeo(location, valid_lat_lon);
      });
   }
}

void GeospatialViewer::eventTraversal()
{
   MapView* map_view = fvw_get_view();

   // The MapView can be null during a print preview, for instance
   if (map_view == nullptr)
      return;

   UpdateCamera();

   auto hud =
      map_view->GetGeospatialViewController()->GetGeospatialScene()->m_HUD;
   if (hud && hud->GetHudProperties().hud_on)
   {
      hud->Update();
   }

   ScenegraphIntersections();
   osgViewer::Viewer::eventTraversal();
}

static int s_time_initial_rendering_pass = false;
static DWORD s_start_time = 0;
static DWORD s_end_time = 0;

void GeospatialViewer::EnableDraw(BOOL enabled)
{
   m_rendering_enabled = enabled;
   if (enabled && reg::get_registry_int("Main", "RenderTimingEnabled", 0))
   {
      s_time_initial_rendering_pass = true;
      s_start_time = s_end_time = timeGetTime();
   }
}

bool GeospatialViewer::checkNeedToDoFrame()
{
   if (getViewerStats() && getViewerStats()->collectStats("frame_rate"))
      return true;

   bool ret = osgViewer::Viewer::checkNeedToDoFrame();
   s_is_updating_frames = ret;
   if (!s_time_initial_rendering_pass || !m_rendering_enabled)
      return ret;

   DWORD crnt_time = timeGetTime();
   if (ret)
   {
      // update end time since we are still rendering
      s_end_time = crnt_time;
   }
   else
   {
      // Has it been more than 3 seconds since we've rendered anything?
      // If so, we will write the total time it took to render the initial
      // map to the log file.
      if (crnt_time - s_end_time > 3000)
      {
         std::string log_file =
            reg::get_registry_string("Main", "RenderTimingLog", "");

         FILE* f = fopen(log_file.c_str(), "w");
         fprintf(f, "%d\n", s_end_time - s_start_time);
         fclose(f);

         // disable further timing results (currently only good for initial
         // pass to eliminate any effects from caching, etc.)
         s_time_initial_rendering_pass = false;
      }
   }

   return ret;
}

void GeospatialViewer::renderingTraversals()
{
   if (m_rendering_enabled)
      osgViewer::Viewer::renderingTraversals();
   else
      _requestRedraw = false;
}
