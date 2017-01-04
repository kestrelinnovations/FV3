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

// GeospatialViewer.h

#pragma once
#ifndef FALCONVIEW_GEOSPATIAL_VIEWER_H_
#define FALCONVIEW_GEOSPATIAL_VIEWER_H_

class GeospatialViewController;
class GeospatialView;
class MapTypeSelectorCallback;
class MapView;
class UpdateCameraAndTexGenCallback;
class PreRenderCallBack;
class GeoKeyboardEventHandler;
class FlightInputDeviceStateType;
class MapEngineOSG;
class HUD;
class GeospatialEarthManipulator;

namespace fvw
{
   class FlightViewManipulator;
   class GroundViewManipulator;
}

/* Temporary declaration to depricate osg keyboard events until
   there is a better solution */
#define DISABLE_OSG_KEYBOARD

#include "osgViewer\Viewer"
#include "GeoInputDevice.h"
#include "..\..\third_party\open_scene_graph\include\osg\Matrixd"

#include <time.h>

namespace osg
{
   class Matrixd;
}

// Used for 3D map projection testing... want to give the 3D map a chance to 
// change before continuing on to the next test.
static bool s_is_updating_frames = false;

// Class which implements the osgViewer::Viewer class. Wrapped by 
// the GeospatialView class.
class GeospatialViewer : public osgViewer::Viewer
{
   friend GeospatialViewController;
   friend GeospatialView;
   friend MapTypeSelectorCallback;
   friend MapView;
   friend UpdateCameraAndTexGenCallback;
   friend PreRenderCallBack;
   friend GeoKeyboardEventHandler;
   friend FlightInputDeviceStateType;
   friend MapEngineOSG;
   friend HUD;
   friend GeospatialEarthManipulator;
   friend fvw::FlightViewManipulator;
   friend fvw::GroundViewManipulator;

private:
   GeospatialViewer(const osgEarth::SpatialReference* srs,
      GeospatialView* parent, GeospatialViewController* gvc);
   void updateTraversal();
   void eventTraversal();
   void CheckPivotCameraUsingPerp(osg::Vec3d* target, osg::Vec3d* perp, osg::Vec3d* distance);
   void EstimateViewpointUsingCamera(osgEarth::Viewpoint* view);
   GeospatialView* parentView;
   BOOL m_rendering_enabled;
public:
   SYSTEMTIME newTime, oldTime;
   Viewpoint3d m_last_pan_viewpoint;

private:
   // Used to transition from pan to ground
   osg::Matrixd last_pan_matrix;

   double time_since_last_frame;

public:
   ~GeospatialViewer();
   void GeospatialViewer::SetThrowDamping(double value);
   double ComputeDistanceToCurrentLocation();
   double ComputeHeading();
   double ComputeIncline();
   double ComputeAltitude();
   double ComputeAltitudeMSL();
   long GetDTEDElevation(double lat, double lon);
   void UpdateCenter(const r_geo_t& new_center_viewer,
      const fvw::CameraMode& mode);
   double m_max_altitude_feet_flight_ground;
   double m_min_altitude_meters_pan_mode;
   // For testing camera below:
   double seconds;
   
   // Suspend or restart the rendering thread
   void EnableDraw(BOOL enabled);

   virtual bool checkNeedToDoFrame() override;
   void renderingTraversals() override;
   
   // if we have to update camera next frame
   bool updateNextFrame;
   osg::Vec3d* nextCameraFrameValues;

   FlightInputDeviceStateType* GetState()
   {
      return fIDevState;
   }

   void ScenegraphIntersections();
   void SetMapParamsRotation(double new_rotation);
   void SetMapParamsZoom(int new_zoom);
   void UpdateCamera();
   double GetTimeSinceLastUpdate();
   void PanCenterUpdate();
   GeoKeyboardEventHandler* GetFlightEventHandler();

   GeoKeyboardEventHandler* flightEventHandler;
   FlightInputDeviceStateType* fIDevState;
   double m_throwDamping;
   bool MapChangeStateRequest;
   double height_above_ground; // height for walk camera above ground in meters
   double meters_per_second; // speed for walking on ground in meters per second
   r_geo_t new_center_viewer; // center of screen, only relevant for PAN mode
   r_geo_t new_center_viewer_camera; // position straight underneath camera in any mode
   d_geo_t new_center_local;
   boolean new_center_request;

   void SetWalkManipulatorHeight(double new_height)
   {
      height_above_ground = new_height;
   }
   void SetWalkManipulatorSpeed(double new_speed)
   {
      meters_per_second = new_speed;
   }

public:
   void GetCameraPosition(double* lat, double* lon, double* alt_meters
      /*, double heading, double incline, double roll*/);

protected:
   void SetCameraPosition(double lat, double lon, double alt_meters,
      double heading, double incline, double roll);
};

class UpdateFlightPosCallback : public osg::NodeCallback {
public:
   UpdateFlightPosCallback::UpdateFlightPosCallback(FlightInputDeviceStateType* flightIDevState)
      : flightPos(-15.,0.,0.)
   {
      m_flightInputDeviceState = flightIDevState;
   }
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      if(nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
      {
         osg::NodeVisitor::TraversalMode tm = nv->getTraversalMode();
         nv->setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
         traverse(node,nv);
         nv->setTraversalMode(tm);
      }
      else
      {
         traverse(node,nv);
      } 
   }
protected:
   osg::Vec3d flightPos;
   FlightInputDeviceStateType* m_flightInputDeviceState; 
};

#endif  // FALCONVIEW_GEOSPATIAL_VIEWER_H_