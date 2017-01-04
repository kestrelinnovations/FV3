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

// GeospatialEarthManipulator.cpp

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/GeospatialEarthManipulator.h"

// system includes
#include <algorithm>

// third party files
#include "osgEarth/DPLineSegmentIntersector"
#include "osgViewer/View"
#include "osgEarth/MapNode"

// other FalconView headers

// this project's headers
#include "FalconView/EarthManipulatorEvents_Interface.h"
#include "FalconView/UIThreadOperation.h"
#include "FalconView/getobjpr.h"

#include "FalconView\GeospatialViewController.h"
#include "FalconView\GeoInputDevice.h"
#include "FalconView\GeospatialViewer.h"
#include "FalconView\GeospatialView.h"
#include "FalconView\OSGVariables.h"
#include "FalconView\FvMath.h"
#include "FalconView\CameraUtils.h"
#include "FalconView\RenderFunctions.h"
#include "geo_tool_d.h"

#define LOCK_HEIGHT_ON_THROW // defining this prevents camera height from changing in a throw

namespace
{
    // a reasonable approximation of cosine interpolation
    double
    smoothStepInterp( double t ) {
        return (t*t)*(3.0-2.0*t);
    }

    // rough approximation of pow(x,y)
    double
    powFast( double x, double y ) {
        return x/(x+y-y*x);
    }

    // accel/decel curve (a < 0 => decel)
    double
    accelerationInterp( double t, double a ) {
        return a == 0.0? t : a > 0.0? powFast( t, a ) : 1.0 - powFast(1.0-t, -a);
    }
}

namespace
{
    // Callback that notifies the manipulator whenever the terrain changes
    // around its center point.
    struct ManipTerrainCallback : public osgEarth::TerrainCallback
    {
        ManipTerrainCallback(GeospatialEarthManipulator* manip) : _manip(manip) { }

        void onTileAdded( const osgEarth::TileKey& key, osg::Node* tile, osgEarth::TerrainCallbackContext& context )
        {
            const osgEarth::GeoPoint& centerMap = _manip->centerMap();
            if ( _manip.valid() && key.getExtent().contains(centerMap.x(), centerMap.y()) )
            {
                //_manip->recalculateCenter();
            }
        }

        osg::observer_ptr<GeospatialEarthManipulator> _manip;
    };
}

GeospatialEarthManipulator::GeospatialEarthManipulator()
{
   _homeViewpointDuration = 0.0;
   no_intersection_matrix_set = false;
   viewer = nullptr;
}

static double
normalizeAzimRad( double input ) {
   if(fabs(input) > 2*osg::PI)
      input = fmod(input,2*osg::PI);
   if( input < -osg::PI ) input += osg::PI*2.0;
   if( input > osg::PI ) input -= osg::PI*2.0;
   return input;
}

void
GeospatialEarthManipulator::getLocalEulerAngles( double* out_azim, double* out_pitch ) const
{
   osg::Matrix m = getMatrix() * osg::Matrixd::inverse(_centerLocalToWorld);
   osg::Vec3d look = -getUpVector( m );
   osg::Vec3d up   =  getFrontVector( m );
    
   look.normalize();
   up.normalize();

   if ( out_azim )
   {
      if ( look.z() < -0.9 )
         *out_azim = atan2( up.x(), up.y() );
      else if ( look.z() > 0.9 )
         *out_azim = atan2( -up.x(), -up.y() );
      else
         *out_azim = atan2( look.x(), look.y() );

      *out_azim = normalizeAzimRad( *out_azim );
   }

   if ( out_pitch )
   {
      *out_pitch = asin( look.z() );
   }
}

void GeospatialEarthManipulator::ActivateModeFrom(fvw::CameraMode previous_camera_mode)
{
   PerformChangeToPanMode change(viewer, this);
   if (previous_camera_mode == fvw::CAMERA_MODE_NONE) // when transitioning from 2D
      change.WaitForAnimation(2.0);
   else if (previous_camera_mode == fvw::CAMERA_MODE_FLIGHT)
      change.WaitForAnimation(1.0);
   else if (previous_camera_mode == fvw::CAMERA_MODE_GROUND_VIEW)
      change.WaitForAnimation(2.0);
}

PerformChangeToPanMode::PerformChangeToPanMode(
   GeospatialViewer* viewer, GeospatialEarthManipulator* manipulator):
m_viewer(viewer),
m_manipulator(manipulator)
{
}

PerformChangeFromPanModeTo2D::PerformChangeFromPanModeTo2D(
   GeospatialViewer* viewer, GeospatialEarthManipulator* manipulator):
m_viewer(viewer),
m_manipulator(manipulator)
{
}

void GeospatialEarthManipulator::Activate2D()
{
   PerformChangeFromPanModeTo2D change(viewer, this);
   change.WaitForAnimation(2.0);
}

void PerformChangeFromPanModeTo2D::AnimationFrame(double t)
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
         double current_altitude_MSL = m_viewer->ComputeAltitudeMSL();
         double elevation = max(altitude - altitude_MSL, 2.0);
         double height_change = max(future_elevation - elevation, 0.0);

         osg::Vec3d delta_altitude(m_position_normalized);
         if (altitude < height_min)
         {
            delta_altitude.operator*=(height_min - (altitude + height_change));
            delta_altitude.operator+=(m_position_normalized.operator*(height_change));
            m_position.operator+=(delta_altitude);
            m_forward.operator+=(delta_altitude);
            m_manipulator->m_desired_zoom_percent = 800;
         }
         else if (altitude > height_max)
         {
            delta_altitude.operator*=(height_max - (altitude + height_change));
            delta_altitude.operator+=(m_position_normalized.operator*(height_change));
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
      double altitude_MSL = m_viewer->ComputeAltitudeMSL();
      hud->m_agl_alt_value = METERS_TO_FEET(altitude_local);
      hud->m_msl_alt_value = METERS_TO_FEET(altitude_MSL);
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
         map_view->NotifyTiltUpdate(-89.9);
      }
      m_viewer->requestRedraw();
   }
}

void PerformChangeToPanMode::AnimationFrame(double t)
{
   FlightInputDeviceStateType* fIDevState = m_viewer->fIDevState;
   osg::ref_ptr<osg::Camera> _camera = m_viewer->getCamera();
   MapView* map_view = fvw_get_view();
   long elevation_meters = 0.0;
   fIDevState->throwing = false;

   const osg::EllipsoidModel* ellipsoid_model = fIDevState->GetSRS()->getEllipsoid();
   if (t == 0.0)
   {
      _camera->getViewMatrixAsLookAt(m_position,m_forward,m_up);
      m_position_normalized.set(m_position.x(), m_position.y(), m_position.z());
      m_position_normalized.normalize();
      m_forward_normalized.set(m_forward-m_position);
      m_forward_normalized.normalize();
      if (fIDevState->GetLastCamMode() == fvw::CAMERA_MODE_FLIGHT || 
         fIDevState->GetLastCamMode() == fvw::CAMERA_MODE_GROUND_VIEW)
      {
         double animationTime = t;
         fIDevState->animating = true;
         _camera->getViewMatrixAsLookAt(m_position_old,m_forward_old,m_up_old);

         // check to see if PAN camera is too close to the ground
         double height_z;
         ellipsoid_model->convertXYZToLatLongHeight(m_position.x(), m_position.y(), m_position.z(),
            m_viewer->new_center_local.lat, m_viewer->new_center_local.lon, height_z);
         elevation_meters = m_viewer->GetDTEDElevation(
            osg::RadiansToDegrees(m_viewer->new_center_local.lat),
            osg::RadiansToDegrees(m_viewer->new_center_local.lon));
         osg::Vec3d position2;
         ellipsoid_model->convertLatLongHeightToXYZ(m_viewer->new_center_local.lat, m_viewer->new_center_local.lon, -5000.0,
            position2.x(), position2.y(), position2.z());
         position2.x() = m_position.x() - position2.x();
         position2.y() = m_position.y() - position2.y();
         position2.z() = m_position.z() - position2.z();
         double relative_angle = 0.0;
         if (height_z - elevation_meters < 1000.0)
         {
            m_position_normalized.set(position2);
            m_position_normalized.normalize();
            m_position_normalized.operator*=(1000.0 - (height_z - elevation_meters));
            m_position.operator+=(m_position_normalized);
            m_forward.operator+=(m_position_normalized);

            _camera->setViewMatrixAsLookAt(m_position, m_forward, m_up);
            m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
            m_manipulator->getLocalEulerAngles(0L, &relative_angle);
            _camera->setViewMatrixAsLookAt(m_position_old, m_forward_old, m_up_old);
            m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
         }
         else
         {
            m_manipulator->getLocalEulerAngles(0L, &relative_angle);
         }

         // Remove roll from camera for earth manipulator mode
         osg::Vec3d tempUp(position2.x() ,position2.y(),position2.z());
         tempUp.normalize();
         osg::Vec3d right;
         osg::Vec3d tempForward(m_forward-m_position);
         tempForward.normalize();
         right = tempForward^tempUp;
         right.normalize();
         m_up = right^tempForward;
         double angle_to_up_r = safe_acos(m_up*tempUp);
         double angle_to_up_d = RAD_TO_DEG(safe_acos(m_up*tempUp));

         // compute maximum angle of view w.r.t. horizon as function of altitude
         double current_altitude_MSL = m_viewer->ComputeAltitudeMSL();
         double max_pitch = m_manipulator->getSettings()->getMaxPitch();
          if (relative_angle > DEG_TO_RAD(max_pitch) || (tempForward*tempUp > 0.0))
         //if (angle_to_up_d < -RAD_TO_DEG(relative_angle) || (tempForward*tempUp > 0.0))
         {
            double rot_angle = DEG_TO_RAD(max_pitch);
            RotationAroundAxis3D(&right,&tempUp,cos(rot_angle),sin(rot_angle));
            tempUp.normalize();
            tempForward.set(right^tempUp);
            tempForward.normalize();
            tempForward.operator*=(-1.0);
            m_forward.set(m_position+tempForward);
            m_up = right^tempForward;
         }

         // Discriminant check "(B^2 - 4AC) from quadratic". If less than zero then camera is
         // pointing to spot not on earth. Not allowed in
         // earth manipulator mode. Changes forward to be tangent
         // to the earth if needed.
         double hh = pow(tempForward*m_position, 2.0);
         double ff = 4.0*pow(tempForward*m_position, 2.0);
         double gg =  4*(tempForward*tempForward)*((m_position*m_position)- pow(EARTHS_RADIUS,2));

         if (4.0*pow(tempForward*m_position, 2.0) -
            4*(tempForward*tempForward)*((m_position*m_position)- pow(EARTHS_RADIUS,2)) < 0.0)
         {
            osg::Vec3d normal(right.operator*(-1.0));
            osg::Vec3d n2(normal^tempUp);
            double theta = safe_acos(EARTHS_RADIUS/m_position.length());
            m_forward = tempUp.operator*(cos(theta)) + n2.operator*=(sin(theta));
            m_forward = m_forward.operator*(EARTHS_RADIUS*0.99); // 0.99 < 1.0 protects from decimal errors
            osg::Vec3d forwardN(m_forward-m_position);
            forwardN.normalize();
            m_forward = m_position + forwardN;
            m_up = right^forwardN;
         }
      }
      else if (fIDevState->GetLastCamMode() == fvw::CAMERA_MODE_NONE) // transitioning from 2D
      {
         m_viewer->setCameraManipulator(m_manipulator, false);
         fIDevState->animating = true;
         fIDevState->transition_animation = true;
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
         double elev = m_viewer->GetDTEDElevation(map_proj_params.center.lat, map_proj_params.center.lon);
         if (elev == MISSING_DTED_ELEVATION)
            elev = 0.0;
         ellipsoid_model->convertLatLongHeightToXYZ(DEG_TO_RAD(map_proj_params.center.lat), 
            DEG_TO_RAD(map_proj_params.center.lon), height_natural , //+ elev,
            m_position.x(), m_position.y(), m_position.z());
         m_position_normalized.set(m_position);
         m_position_normalized.normalize();
         m_position_old.set(m_position);
         m_position_old.operator+=(m_position_normalized.operator*(elev));
         osg::Vec3d to_east_vector = north_vector^m_position_normalized;
         to_east_vector.normalize();
         m_up_old.set(m_position_normalized^to_east_vector);
         m_up_old.normalize();
         RotationAroundAxis3D(&m_position_normalized, &m_up_old, 
            cos(DEG_TO_RAD(map_proj_params.rotation)),
            sin(DEG_TO_RAD(map_proj_params.rotation)));
         m_up_old.normalize();
         m_forward_old.set(m_position_normalized);
         m_forward_old.operator*=(-1.0);
         // now to apply a slight amount of tilt to avoid being orthogonal to the earth's surface
         // this will allow the camera to actually have a heading
         osg::Vec3d right(m_forward_old^m_up_old);
         double angle1 = RAD_TO_DEG(safe_acos(m_forward_old*m_up_old));
         right.normalize();
         RotationAroundAxis3D(&right, &m_up_old, cos(DEG_TO_RAD(0.1)), sin(DEG_TO_RAD(0.1)));
         m_up_old.normalize();
         m_forward_old.set(m_up_old^right);
         m_forward_old.normalize();
         m_forward.set(m_forward_old);
         m_forward_old.operator+=(m_position_old);
         m_forward.operator+=(m_position);
         m_up.set(m_up_old);
      }
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
         fIDevState->transition_animation = false;
         fIDevState->animating = false;
      }
      // Interpolate between camera positions as transition occurs to new manipulator
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
      double altitude_local = m_viewer->ComputeAltitude();
      double altitude_MSL = m_viewer->ComputeAltitudeMSL();
      OSGUserVars::SetAltitude(altitude_local);
      auto hud =
         map_view->GetGeospatialViewController()->GetGeospatialScene()->GetHud();
      hud->m_agl_alt_value = METERS_TO_FEET(altitude_local);
      hud->m_msl_alt_value = METERS_TO_FEET(altitude_MSL);
      hud->Update();

      if (fIDevState->GetActiveCam() != fvw::CAMERA_MODE_FLIGHT && 
         fIDevState->GetActiveCam() != fvw::CAMERA_MODE_GROUND_VIEW
         || fIDevState->GetLastCamMode() == fvw::CAMERA_MODE_NONE)
      {
         m_manipulator->no_intersection_matrix_set = true;
         m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
      }
      if (fIDevState->GetActiveCam() != fvw::CAMERA_MODE_GROUND_VIEW || true)
      {
         m_viewer->requestRedraw();
      }

      hud->m_heading = m_viewer->ComputeHeading();

      if (!fIDevState->animating)
      {
         // Transition to earth manipulator mode
         m_viewer->setCameraManipulator(m_manipulator, false);
         _camera->setViewMatrixAsLookAt(m_position,m_forward,m_up);
         m_manipulator->no_intersection_matrix_set = true;
         m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
         osg::Vec3d new_screen_center;
         osg::Vec3d tempUp;
         bool centerFound = false;
         double xIntersect2 = _camera->getViewport()->width()/2.0;
         double yIntersect = _camera->getViewport()->height()/2.0;
         osgUtil::LineSegmentIntersector::Intersections intersections;
         m_position_normalized.set(m_position);
         m_position_normalized.normalize();
         map_view->NotifyTiltUpdate(m_viewer->ComputeIncline());
         if (fIDevState->GetLastCamMode() != fvw::CAMERA_MODE_NONE)
         {
            for (int i=0; i<2; i++)
            {
               if (m_viewer->computeIntersections(xIntersect2, yIntersect, intersections, 0xffffffff))
               {
                  {
                     using namespace osgUtil;
                     using namespace osg;
                     int firstOnly = 0;

                     //  The first intersection of earth is correct since
                     //  the earth is convex (ellipsoid)
                     for (LineSegmentIntersector::Intersections::const_iterator iter = intersections.begin();
                     firstOnly < 1;
                     firstOnly++)
                     {
                     tempUp = iter->getWorldIntersectPoint();
                     new_screen_center.set(tempUp);
                     tempUp.normalize();
                     }
                     if ((m_position_normalized*tempUp) > -0.15)
                     {
                     centerFound = true;
                     break;
                     }
                  }
               }
               else // prevent errors with vectors falling between two polygons
               {
                  xIntersect2++;
                  yIntersect++;
               }
            }
            if (centerFound)
            {
               if (fIDevState->GetLastCamMode() == fvw::CAMERA_MODE_FLIGHT || 
                  fIDevState->GetLastCamMode() == fvw::CAMERA_MODE_GROUND_VIEW)
               {
                  // WARNING: While new_screen_center is the screen center at 0 meters AGL,
                  // osgEarth expects the screen center to be based on 0 meters MSL. This 
                  // means unless the user is looking straight down or the center of the 
                  // screen is at 0 MSL, these centers will be different. Therefore, 
                  // a correction is needed for the "fake" center of screen for osgEarth.
                  osg::Vec3d lookVector(new_screen_center-m_position);
                  lookVector.normalize();
                  osg::Vec3d eye(m_position);
                  double e = WGS84_a_METERS;
                  double p = WGS84_b_METERS;
                  double a = (pow(lookVector.x()/e, 2.0) +
                      pow(lookVector.y()/e, 2.0) +
                      pow(lookVector.z()/p, 2.0));
                  double b = 2.0*((eye.x()/e)*(lookVector.x()/e) +
                      (eye.y()/e)*(lookVector.y()/e) +
                      (eye.z()/p)*(lookVector.z()/p));
                  double c = (pow(eye.x()/e,2.0) +
                      pow(eye.y()/e,2.0) + 
                      pow(eye.z()/p,2.0)) - 1.0;
                  if ((pow(b,2) - 4*a*c) > 0)
                  {
                     double t = (-b -pow(pow(b,2)-4*a*c,0.5))/(2*a); 
                     new_screen_center = eye + lookVector.operator*(t);
                  }
               }
               double height_z;
               ellipsoid_model->convertXYZToLatLongHeight(new_screen_center.x(), new_screen_center.y(), new_screen_center.z(),
                  m_viewer->new_center_viewer.lat, m_viewer->new_center_viewer.lon, height_z);
               if (height_z == MISSING_DTED_ELEVATION)
                  height_z = 0.0;
               Viewpoint3d new_view;
               map_view->GetGeospatialViewController()->
                  GetViewpoint3D(&new_view);
               new_view.y = RAD_TO_DEG(m_viewer->new_center_viewer.lat);
               new_view.x = RAD_TO_DEG(m_viewer->new_center_viewer.lon);
               new_view.range = sqrt(
                  pow(new_screen_center.x()-m_position.x(), 2.0) +
                  pow(new_screen_center.y()-m_position.y(), 2.0) +
                  pow(new_screen_center.z()-m_position.z(), 2.0)) + height_z;
               map_view->GetGeospatialViewController()->
                  SetViewpoint3D(new_view);
               m_viewer->UpdateCenter(m_viewer->new_center_viewer, fIDevState->GetActiveCam());
               map_view->NotifyTiltUpdate(m_viewer->ComputeIncline());
            }
         } 
         else // transition from 2D to 3D
         {
            _camera->setViewMatrixAsLookAt(m_position,m_forward,m_up);
            m_manipulator->no_intersection_matrix_set = true;
            m_manipulator->setByInverseMatrix(_camera->getViewMatrix());
            map_view->NotifyTiltUpdate(m_viewer->ComputeIncline());
         }
      }
   }
}

void GeospatialEarthManipulator::recalculateCenter( const osg::CoordinateFrame& frame )
{
    osg::ref_ptr<osg::Node> safeNode = _node.get();
    if ( safeNode.valid() )
    {
        bool hitFound = false;

        //osg::Vec3d eye = getMatrix().getTrans();

        // need to reintersect with the terrain
        double ilen = safeNode->getBound().radius()*0.25f;

        osg::Vec3d up = getUpVector(frame);

        osg::Vec3d ip1;
        osg::Vec3d ip2;
        // extend coordonate to fall on the edge of the boundingbox see http://www.osgearth.org/ticket/113
        bool hit_ip1 = intersect(_center - up * ilen * 0.1, _center + up * ilen, ip1);
        bool hit_ip2 = intersect(_center + up * ilen * 0.1, _center - up * ilen, ip2);

        // Next few lines of code prevent the camera from jumping around due to
        // changes in elevation in the terrain
        ip1.normalize(); ip2.normalize();
        //ip1.operator*=(EARTHS_RADIUS); ip2.operator*=(EARTHS_RADIUS);
        double lat, lon, height;
        osg::Vec3d temp_radius;

        getSRS()->getEllipsoid()->convertXYZToLatLongHeight(_center.x(), _center.y(), 
            _center.z(), lat, lon, height);
        getSRS()->getEllipsoid()->convertLatLongHeightToXYZ(lat, lon, 0.0, 
            temp_radius.x(), temp_radius.y(), temp_radius.z());
        ip1.operator*=(temp_radius.length());
        ip2.operator*=(temp_radius.length());

        if (hit_ip1)
        {
            if (hit_ip2)
            {
                setCenter( (_center-ip1).length2() < (_center-ip2).length2() ? ip1 : ip2 );
                hitFound = true;
            }
            else
            {
                setCenter( ip1 );
                hitFound = true;
            }
        }
        else if (hit_ip2)
        {
            setCenter( ip2 );
            hitFound = true;
        }

        if (hitFound)
        {
#if 0
            // recalculate the distance based on the current eyepoint:
            double oldDistance = _distance;
            double newDistance = (eye-_center).length();
            setDistance( newDistance );
            OE_NOTICE << "OLD = " << oldDistance << ", NEW = " << newDistance << std::endl;
#endif
        }

        else // if (!hitFound)
        {
            // ??
            //OE_DEBUG<<"EarthManipulator unable to intersect with terrain."<<std::endl;
        }
    }
}

void GeospatialEarthManipulator::setCenter( const osg::Vec3d& worldPos )
{
    _center = worldPos;
    createLocalCoordFrame( worldPos, _centerLocalToWorld );
    if ( _cached_srs.valid() )
    {
        _centerMap.fromWorld( _cached_srs.get(), worldPos );
    }
}

void GeospatialEarthManipulator::AddListener(
   EarthManipulatorEvents_Interface* listener)
{
   m_listeners.push_back(listener);
   listener->DistanceChanged(getDistance());
}

void GeospatialEarthManipulator::RemoveListener(
   EarthManipulatorEvents_Interface* listener)
{
   m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(),
      listener), m_listeners.end());
}

void GeospatialEarthManipulator::setByMatrix(const osg::Matrixd& matrix)
{
   osg::Vec3d lookVector(- matrix(2,0),-matrix(2,1),-matrix(2,2));
   osg::Vec3d eye(matrix(3,0),matrix(3,1),matrix(3,2));

   _centerRotation = makeCenterRotation(_center);

   osg::ref_ptr<osg::Node> safeNode = _node.get();

   if ( !safeNode.valid())
   {
      setCenter( eye + lookVector );
      setDistance( lookVector.length() );
      _rotation = matrix.getRotate().inverse() * _centerRotation.inverse();
      no_intersection_matrix_set = false;
      return;
   }

   // need to reintersect with the terrain
   const osg::BoundingSphere& bs = safeNode->getBound();
   float distance = (eye-bs.center()).length() + safeNode->getBound().radius();
   osg::Vec3d start_segment = eye;
   osg::Vec3d end_segment = eye + lookVector*distance;

   osg::Vec3d ip;
   bool hitFound = false;
   if (!no_intersection_matrix_set)
   {
       if (intersect(start_segment, end_segment, ip))
       {
          setCenter( ip );
          _centerRotation = makeCenterRotation(_center);
    #ifndef LOCK_HEIGHT_ON_THROW
          setDistance( (eye-ip).length()); 
    #endif

          osg::Matrixd rotation_matrix = osg::Matrixd::translate(0.0,0.0,-_distance)*
             matrix*
             osg::Matrixd::translate(-_center);
          _rotation = rotation_matrix.getRotate() * _centerRotation.inverse();
          hitFound = true;
       }
   }

   //  Possible that code from earth manipulator above can falsely miss a collision
   if (!hitFound) 
   {
      //double a = lookVector*lookVector;
      //double b = (lookVector.operator*(2))*eye;
      //double c = eye*eye - pow(EARTHS_RADIUS,2);

      // higher precision below
      double e = WGS84_a_METERS;
      double p = WGS84_b_METERS;
      double a = (pow(lookVector.x()/e, 2.0) +
          pow(lookVector.y()/e, 2.0) +
          pow(lookVector.z()/p, 2.0));
      double b = 2.0*((eye.x()/e)*(lookVector.x()/e) +
          (eye.y()/e)*(lookVector.y()/e) +
          (eye.z()/p)*(lookVector.z()/p));
      double c = (pow(eye.x()/e,2.0) +
          pow(eye.y()/e,2.0) + 
          pow(eye.z()/p,2.0)) - 1.0;


      if ((pow(b,2) - 4*a*c) > 0)
      {
         double t = (-b -pow(pow(b,2)-4*a*c,0.5))/(2*a); 
         ip = eye + lookVector.operator*(t);
         setCenter( ip );
         _centerRotation = makeCenterRotation(_center);
         setDistance( (eye-ip).length());
         osg::Matrixd rotation_matrix = osg::Matrixd::translate(0.0,0.0,-_distance)*
            matrix*
            osg::Matrixd::translate(-_center);
         _rotation = rotation_matrix.getRotate() * _centerRotation.inverse();
         hitFound = true;
      }
   }

   if (!hitFound)
   {
      osg::CoordinateFrame eyeCoordFrame;
      createLocalCoordFrame( eye, eyeCoordFrame );

      osg::Vec3d eyeUp = getUpVector(eyeCoordFrame);

      if (intersect(eye + eyeUp*distance, eye - eyeUp*distance, ip))
      {
         setCenter( ip );
         _centerRotation = makeCenterRotation(_center);
         setDistance((eye-ip).length());
         _rotation.set(0,0,0,1);
         hitFound = true;
      }
   }

   //osg::CoordinateFrame coordinateFrame;
   //createLocalCoordFrame( _center, coordinateFrame );
   _previousUp = getUpVector(_centerLocalToWorld);

   recalculateRoll();
   //recalculateLocalPitchAndAzimuth();
}

bool GeospatialEarthManipulator::established()
{
#ifdef USE_OBSERVER_NODE_PATH
    bool needToReestablish = (!_csn.valid() || _csnObserverPath.empty()) && _node.valid();
#else
    bool needToReestablish = !_csn.valid() && _node.valid();
#endif

    if ( needToReestablish )
    {
        osg::ref_ptr<osg::Node> safeNode = _node.get();
        if ( !safeNode.valid() )
            return false;

        // find a map node.
        osgEarth::MapNode* mapNode = osgEarth::MapNode::findMapNode( safeNode.get(), 0x01 );
        if ( mapNode )
        {
            _terrainCallback = new ManipTerrainCallback( this );
            mapNode->getTerrain()->addTerrainCallback( _terrainCallback );
        }

        // find a CSN node - if there is one, we want to attach the manip to that
        _csn = osgEarth::findRelativeNodeOfType<osg::CoordinateSystemNode>( safeNode.get(), 0x01 );

        if ( _csn.valid() )
        {
            _node = _csn.get();

#if USE_OBSERVER_NODE_PATH
            _csnObserverPath.setNodePathTo( _csn.get() );
#endif

            if ( !_homeViewpoint.isSet() )
            {
                if ( _has_pending_viewpoint )
                {
                    setHomeViewpoint(
                        _pending_viewpoint);

                    _has_pending_viewpoint = false;
                }
                //If we have a CoordinateSystemNode and it has an ellipsoid model
                else if ( _csn->getEllipsoidModel() )
                {
                    setHomeViewpoint( 
                        osgEarth::Viewpoint(osg::Vec3d(-90,0,0), 0, -89,
                        _csn->getEllipsoidModel()->getRadiusEquator()*3.0 ) );
                }
                else
                {
                    setHomeViewpoint( osgEarth::Viewpoint(
                        safeNode->getBound().center(),
                        0, -89.9, 
                        safeNode->getBound().radius()*2.0) );
                }
            }

            if ( !_has_pending_viewpoint )
                setViewpoint( _homeViewpoint.get(), _homeViewpointDuration );
            else
                setViewpoint( _pending_viewpoint, _pending_viewpoint_duration_s );

            _has_pending_viewpoint = false;
        }

        // reset the srs cache:
        _cached_srs = NULL;
        _srs_lookup_failed = false;

        //OE_DEBUG << "[EarthManip] new CSN established." << std::endl;
    }

    return _csn.valid() && _node.valid();
}

void GeospatialEarthManipulator::setNode(osg::Node* node)
{
    // you can only set the node if it has not already been set, OR if you are setting
    // it to NULL. (So to change it, you must first set it to NULL.) This is to prevent
    // OSG from overwriting the node after you have already set on manually.
    if ( node == 0L || !_node.valid() )
    {
        _node = node;
        _csn = 0L;

        if ( _viewCamera.valid() && _cameraUpdateCB.valid() )
        {
            _viewCamera->removeUpdateCallback( _cameraUpdateCB.get() );
            _cameraUpdateCB = 0L;
        }

        _viewCamera = 0L;

#ifdef USE_OBSERVER_NODE_PATH
        _csnObserverPath.clearNodePath();
#endif
        _csnPath.clear();
        reinitialize();

        // this might be unnecessary..
        established();
    }
}

bool GeospatialEarthManipulator::handleMouseAction( const Action& action, osg::View* view )
{
    // return if less then two events have been added.
    if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

    //if ( osgEarth::getNotifyLevel() > osg::INFO )
    //    dumpActionInfo( action, osg::DEBUG_INFO );

    double dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
    double dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();

    // return if there is no movement.
    if (dx==0 && dy==0) return false;

    // here we adjust for action scale, global sensitivy
    dx *= _settings->getMouseSensitivity();
    dy *= _settings->getMouseSensitivity();

    applyOptionsToDeltas( action, dx, dy );

    // in "continuous" mode, we accumulate the deltas each frame - thus
    // the deltas act more like speeds.
    if ( _continuous )
    {
        _continuous_dx += dx * 0.01;
        _continuous_dy += dy * 0.01;
        if (action._type == ACTION_ZOOM)
        {
            fvw_get_view()->GetGeospatialViewController()->GetInputDevice()->
               mouse_moved_pressed = true;
            drag(dx, dy, view);
        }
    }
    else
    {
        if (action._type == ACTION_ZOOM)
        {
           OSGUserVars::SetAltitude(fvw_get_view()->GetGeospatialViewController()->
              GetGeospatialView()->Viewer()->ComputeAltitude());
        }
        handleMovementAction( action._type, dx, dy, view );
    }

    return true;
}

void GeospatialEarthManipulator::zoom( double dx, double dy, osg::View* )
{    
    double scale = 1.0f + dy;
    setDistance( _distance * scale );    
    // TODO: need to constrain for zoom %
}

void GeospatialEarthManipulator::drag( double dx, double dy, osg::View* view )
{
    int x = 3;
}

void GeospatialEarthManipulator::pan( double dx, double dy )
{
    //OE_NOTICE << "pan " << dx << "," << dy <<  std::endl;
    if (!_tether_node.valid())
    {
        double scale = -0.3f*_distance;
        double old_azim;
        getLocalEulerAngles( &old_azim );

        osg::Matrixd rotation_matrix;
        rotation_matrix.makeRotate( _rotation * _centerRotation  );

        // compute look vector.
        osg::Vec3d lookVector = -getUpVector(rotation_matrix);
        osg::Vec3d sideVector = getSideVector(rotation_matrix);
        osg::Vec3d upVector = getFrontVector(rotation_matrix);

        osg::Vec3d localUp = _previousUp;

        osg::Vec3d forwardVector =localUp^sideVector;
        sideVector = forwardVector^localUp;

        forwardVector.normalize();
        sideVector.normalize();

        osg::Vec3d dv = forwardVector * (dy*scale) + sideVector * (dx*scale);

        // save the previous CF so we can do azimuth locking:
        osg::CoordinateFrame oldCenterLocalToWorld = _centerLocalToWorld;

        double lengi = _center.length();
        //_center.normalize();
        //_center.operator*=(4520013.0);
        // move the center point:
        setCenter( _center + dv );

        // need to recompute the intersection point along the look vector.
        osg::ref_ptr<osg::Node> safeNode = _node.get();
        if (safeNode.valid())
        {
            recalculateCenter( oldCenterLocalToWorld ); // disabled june 2013

            osg::Vec3d new_localUp = getUpVector( _centerLocalToWorld );

            osg::Quat pan_rotation;
            pan_rotation.makeRotate( localUp, new_localUp );

            if ( !pan_rotation.zeroRotation() )
            {
                _centerRotation = _centerRotation * pan_rotation;
                _previousUp = new_localUp;
            }
            else
            {
                //OE_DEBUG<<"New up orientation nearly inline - no need to rotate"<<std::endl;
            }

            if ( _settings->getLockAzimuthWhilePanning() )
            {
                double new_azim;
                getLocalEulerAngles( &new_azim );

                double delta_azim = new_azim - old_azim;
                //OE_NOTICE << "DeltaAzim" << delta_azim << std::endl;

                osg::Quat q;
                q.makeRotate( delta_azim, new_localUp );
                if ( !q.zeroRotation() )
                {
                    _centerRotation = _centerRotation * q;
                }
            }
        }

        //recalculateLocalPitchAndAzimuth();
    }
    else
    {
        double scale = _distance;
        _offset_x += dx * scale;
        _offset_y += dy * scale;

        //Clamp values within range
        if (_offset_x < -_settings->getMaxXOffset()) _offset_x = -_settings->getMaxXOffset();
        if (_offset_y < -_settings->getMaxYOffset()) _offset_y = -_settings->getMaxYOffset();
        if (_offset_x > _settings->getMaxXOffset()) _offset_x = _settings->getMaxXOffset();
        if (_offset_y > _settings->getMaxYOffset()) _offset_y = _settings->getMaxYOffset();
    }
}

void GeospatialEarthManipulator::cancelViewpointTransition()
{
   // Complete previous transition, if necessary
   if (_setting_viewpoint)
   {
      osgEarth::Viewpoint new_vp(
         _start_viewpoint.getFocalPoint() + _delta_focal_point,
         _start_viewpoint.getHeading() + _delta_heading,
         _start_viewpoint.getPitch() + _delta_pitch,
         _start_viewpoint.getRange() + _delta_range +
         (sin(osg::PI)*_arc_height),
         _start_viewpoint.getSRS() );

      _setting_viewpoint = false;
      setViewpoint( new_vp, 0.0 );
   }

   _setting_viewpoint = false;
}

void GeospatialEarthManipulator::setViewpoint( const osgEarth::Viewpoint& vp, double duration_s )
{
    if ( !established() ) 
    {
        _pending_viewpoint = vp;
        _pending_viewpoint_duration_s = duration_s;
        _has_pending_viewpoint = true;
    }
    else if ( duration_s > 0.0 )
    {
        // xform viewpoint into map SRS
        osg::Vec3d vpFocalPoint = vp.getFocalPoint();
        if ( _cached_srs.valid() && vp.getSRS() && !_cached_srs->isEquivalentTo( vp.getSRS() ) )
        {
            vp.getSRS()->transform( vp.getFocalPoint(), _cached_srs.get(), vpFocalPoint );
        }

        _start_viewpoint = getViewpoint();
        
        _delta_heading = vp.getHeading() - _start_viewpoint.getHeading(); //TODO: adjust for crossing -180
        _delta_pitch   = vp.getPitch() - _start_viewpoint.getPitch();
        _delta_range   = vp.getRange() - _start_viewpoint.getRange();
        _delta_focal_point = vpFocalPoint - _start_viewpoint.getFocalPoint(); // TODO: adjust for lon=180 crossing

        while( _delta_heading > 180.0 ) _delta_heading -= 360.0;
        while( _delta_heading < -180.0 ) _delta_heading += 360.0;

        // adjust for geocentric date-line crossing
        if ( _is_geocentric)
        {
           int count = 3;  // used to prevent infinite loop
           while(_delta_focal_point.x() > 180.0 && count--)
              _delta_focal_point.x() -= 360.0;

           count = 3;
           while(_delta_focal_point.x() < -180.0 && count--)
              _delta_focal_point.x() += 360.0;
        }

        // calculate an acceleration factor based on the Z differential
        double h0 = _start_viewpoint.getRange() * sin( osg::DegreesToRadians(-_start_viewpoint.getPitch()) );
        double h1 = vp.getRange() * sin( osg::DegreesToRadians( -vp.getPitch() ) );
        double dh = (h1 - h0);

        // calculate the total distance the focal point will travel and derive an arc height:
        double de;
        if ( _is_geocentric && (vp.getSRS() == 0L || vp.getSRS()->isGeographic()) )
        {
            osg::Vec3d startFP = _start_viewpoint.getFocalPoint();
            double x0,y0,z0, x1,y1,z1;
            _cached_srs->getEllipsoid()->convertLatLongHeightToXYZ(
                osg::DegreesToRadians( _start_viewpoint.y() ), osg::DegreesToRadians( _start_viewpoint.x() ), 0.0, x0, y0, z0 );
            _cached_srs->getEllipsoid()->convertLatLongHeightToXYZ(
                osg::DegreesToRadians( vpFocalPoint.y() ), osg::DegreesToRadians( vpFocalPoint.x() ), 0.0, x1, y1, z1 );
            de = (osg::Vec3d(x0,y0,z0) - osg::Vec3d(x1,y1,z1)).length();
        }
        else
        {
            de = _delta_focal_point.length();
        }

        _arc_height = 0.0;
        if ( _settings->getArcViewpointTransitions() )
        {         
            _arc_height = osg::maximum( de - fabs(dh), 0.0 );
        }

        // calculate acceleration coefficients
        if ( _arc_height > 0.0 )
        {
            // if we're arcing, we need seperate coefficients for the up and down stages
            double h_apex = 2.0*(h0+h1) + _arc_height;
            double dh2_up = fabs(h_apex - h0)/100000.0;
            _set_viewpoint_accel = log10( dh2_up );
            double dh2_down = fabs(h_apex - h1)/100000.0;
            _set_viewpoint_accel_2 = -log10( dh2_down );
        }
        else
        {
            // on arc => simple unidirectional acceleration:
            double dh2 = (h1 - h0)/100000.0;
            _set_viewpoint_accel = fabs(dh2) <= 1.0? 0.0 : dh2 > 0.0? log10( dh2 ) : -log10( -dh2 );
            if ( fabs( _set_viewpoint_accel ) < 1.0 ) _set_viewpoint_accel = 0.0;
        }
        
        if ( _settings->getAutoViewpointDurationEnabled() )
        {
            double maxDistance = _cached_srs->getEllipsoid()->getRadiusEquator();
            double ratio = osg::clampBetween( de/maxDistance, 0.0, 1.0 );
            ratio = accelerationInterp( ratio, -4.5 );
            double minDur, maxDur;
            _settings->getAutoViewpointDurationLimits( minDur, maxDur );
            duration_s = minDur + ratio*(maxDur-minDur);
        }
        
        // don't use _time_s_now; that's the time of the last event
        _time_s_set_viewpoint = osg::Timer::instance()->time_s();
        _set_viewpoint_duration_s = duration_s;

        _setting_viewpoint = true;
        
        _thrown = false;
        _task->_type = TASK_NONE;
    }
    else
    {
        osg::Vec3d new_center = vp.getFocalPoint();

        // start by transforming the requested focal point into world coordinates:
        if ( getSRS() )
        {
            // resolve the VP's srs. If the VP's SRS is not specified, assume that it
            // is either lat/long (if the map is geocentric) or X/Y (otherwise).
            osg::ref_ptr<const osgEarth::SpatialReference> vp_srs = vp.getSRS()? vp.getSRS() :
                _is_geocentric? getSRS()->getGeographicSRS() :
                getSRS();

    //TODO: streamline
            if ( !getSRS()->isEquivalentTo( vp_srs.get() ) )
            {
                osg::Vec3d local = new_center;
                // reproject the focal point if necessary:
                vp_srs->transform2D( new_center.x(), new_center.y(), getSRS(), local.x(), local.y() );
                new_center = local;
            }

            // convert to geocentric coords if necessary:
            if ( _is_geocentric )
            {
                osg::Vec3d geocentric;

                getSRS()->getEllipsoid()->convertLatLongHeightToXYZ(
                    osg::DegreesToRadians( new_center.y() ),
                    osg::DegreesToRadians( new_center.x() ),
                    new_center.z(),
                    geocentric.x(), geocentric.y(), geocentric.z() );

                new_center = geocentric;            
            }
        }

        // now calculate the new rotation matrix based on the angles:


        double new_pitch = osg::DegreesToRadians(
            osg::clampBetween( vp.getPitch(), _settings->getMinPitch(), _settings->getMaxPitch() ) );

        double new_azim = normalizeAzimRad( osg::DegreesToRadians( vp.getHeading() ) );

        setCenter( new_center );
        setDistance( vp.getRange() );

        _previousUp = getUpVector( _centerLocalToWorld );

        _centerRotation = getRotation( new_center ).getRotate().inverse();

        osg::Quat azim_q( new_azim, osg::Vec3d(0,0,1) );
        osg::Quat pitch_q( -new_pitch -osg::PI_2, osg::Vec3d(1,0,0) );

        osg::Matrix new_rot = osg::Matrixd( azim_q * pitch_q );

        _rotation = osg::Matrixd::inverse(new_rot).getRotate();
    }
}

void GeospatialEarthManipulator::setDistance(double distance)
{
   double agl_altitude_meters = 0.0;
   double min_distance = 0.0;
   if (viewer)
   {
      min_distance = viewer->m_min_altitude_meters_pan_mode;
      agl_altitude_meters = viewer->ComputeDistanceToCurrentLocation();
      if (agl_altitude_meters < min_distance && 
         agl_altitude_meters > 0.1 && getDistance() > distance)
      {
         return;
      }
   }


   // Call the base class implementation
   osgEarth::Util::EarthManipulator::setDistance(distance);

   // Let the listeners know that the distance to the earth has
   // been updated
   std::for_each(m_listeners.begin(), m_listeners.end(), [=](
      EarthManipulatorEvents_Interface* listener)
   {
      listener->DistanceChanged(distance);
   });
}

void GeospatialEarthManipulator::rotate(double dx, double dy)
{
   osgEarth::Util::EarthManipulator::rotate(dx, dy);

   double heading, tilt;
   getLocalEulerAngles(&heading, &tilt);

   new fvw::UIThreadOperation([=]()
   {
      MapView* map_view = fvw_get_view();
      if (map_view)
      {
         double heading_deg = -osg::RadiansToDegrees(heading);
         if (heading_deg < 0.0)
            heading_deg += 360.0;

         map_view->UpdateRotationAndTilt(heading_deg,
            osg::RadiansToDegrees(tilt));
      }
   });
}

// For now, this method should only be called from an event handler (i.e.,
// a keypress of mouse event) as it depends on the state of the camera.
//
void GeospatialEarthManipulator::ComputeMapBounds(osg::View* view,
   d_geo_rect_t* geo_rect)
{
   int surface_width = view->getCamera()->getViewport()->width();
   int surface_height = view->getCamera()->getViewport()->height();

   // Get current pitch
   double unused, pitch;
   getLocalEulerAngles(&unused, &pitch);

   // Compute a rotation quaternion for 0 degree heading (preserving pitch)
   osg::Quat azim_q(0.0, osg::Vec3d(0, 0, 1));
   osg::Quat pitch_q(-pitch - osg::PI_2, osg::Vec3d(1, 0, 0));
   osg::Matrix new_rot = osg::Matrixd(azim_q * pitch_q);
   osg::Quat rotation = osg::Matrixd::inverse(new_rot).getRotate();

   // Compute view matrix using local rotation quat rather than _rotation
   osg::Matrixd view_matrix = osg::Matrixd::translate(-_center) *
      osg::Matrixd::rotate(_centerRotation.inverse()) *
      osg::Matrixd::rotate(rotation.inverse()) *
      osg::Matrixd::translate(_offset_x, _offset_y, -_distance);

   // Compute upper-right latitude and lower-left longitude
   //
   osg::Vec3d world_coords;
   if (screenToWorld(surface_width / 2.f, surface_height - 1.f,
      view, world_coords))
   {
      osg::Vec3d geo;
      if (getSRS()->transformFromWorld(world_coords, geo))
      {
         geo_rect->ur.lat = geo.y();
         geo_rect->ll.lon = geo.x();
      }
   }
   else
   {
      geo_rect->ur.lat = _centerMap.y()  + 90.0;
      if (geo_rect->ur.lat > 90.0)
         geo_rect->ur.lat = 90.0;

      geo_rect->ll.lon = _centerMap.x() - 179.999999;
      if (geo_rect->ll.lon <= -180.0)
         geo_rect->ll.lon += 360.0;
   }

   // Compute lower-left latitude and upper-right longitude
   //
   if (screenToWorld(surface_width / 2., 0.f, view, world_coords))
   {
      osg::Vec3d geo;
      if (getSRS()->transformFromWorld(world_coords, geo))
      {
         geo_rect->ll.lat = geo.y();
         geo_rect->ur.lon = geo.x();
      }
   }
   else
   {
      geo_rect->ll.lat = _centerMap.y()  - 90.0;
      if (geo_rect->ll.lat < -90.0)
         geo_rect->ll.lat = -90.0;

      geo_rect->ur.lon = _centerMap.x() + 179.999999;
      if (geo_rect->ur.lon >= -180.0)
         geo_rect->ur.lon -= 360.0;
   }
}
