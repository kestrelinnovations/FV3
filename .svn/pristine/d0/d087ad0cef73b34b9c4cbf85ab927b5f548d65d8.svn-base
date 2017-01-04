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

// GeospatialEarthManipulator.h

#ifndef FALCONVIEW_GEOSPATIALEARTHMANIPULATOR_H_
#define FALCONVIEW_GEOSPATIALEARTHMANIPULATOR_H_
#pragma warning(disable:4250)

// system includes
#include <vector>

// third party files
#include "osgEarth/Viewpoint"
#include "osgEarthUtil/Common"
#include "osgEarthUtil/EarthManipulator"
#include "osgEarth/NodeUtils"

// other FalconView headers
#include "FalconView/AnimatedTransitionCallback.h"

// this project's headers

// forward definitions
class EarthManipulatorEvents_Interface;
class GeospatialView;
class GeospatialViewController;
class GeospatialViewer;
class MapTypeSelectorCallback;

#define EARTHS_RADIUS 6378128.0954479595915541658189133 // meters

//#define DEBUG_CAMERA

namespace osgEarth
{
   class ImageLayer;
   class MapNode;
   class Viewpoint;

   namespace Util
   {
      class SkyNode;
   }
}

namespace osg
{
   class Group;
   class View;
}

namespace fvw
{
   enum CameraMode;
}


class GeospatialEarthManipulator : public osgEarth::Util::EarthManipulator
{
   friend GeospatialView;
   friend GeospatialViewer;
   friend GeospatialViewController;

   // allows inheritance of smart ptr methods
   // such as unref, etc...
   friend osg::ref_ptr<GeospatialEarthManipulator>;

public:
   GeospatialEarthManipulator();

   void AddListener(EarthManipulatorEvents_Interface* listener);
   void RemoveListener(EarthManipulatorEvents_Interface* listener);
   void GeospatialEarthManipulator::setByMatrix(const osg::Matrixd& matrix);
   void GeospatialEarthManipulator::setViewpoint( const osgEarth::Viewpoint& vp, double duration_s );
   void recalculateCenter() { recalculateCenter(_centerLocalToWorld); }
   void GeospatialEarthManipulator::recalculateCenter( const osg::CoordinateFrame& frame );
   void GeospatialEarthManipulator::zoom( double dx, double dy, osg::View* ) override;
   bool GeospatialEarthManipulator::handleMouseAction( const Action& action, osg::View* view ) override;
   void GeospatialEarthManipulator::drag( double dx, double dy, osg::View* view) override;
   void GeospatialEarthManipulator::pan( double dx, double dy ) override;
   void GeospatialEarthManipulator::setCenter( const osg::Vec3d& worldPos );
   bool GeospatialEarthManipulator::established();
   void GeospatialEarthManipulator::setNode(osg::Node* node);
   void GeospatialEarthManipulator::getLocalEulerAngles( double* out_azim, double* out_pitch =0L ) const;

   virtual void cancelViewpointTransition() override;
   void ActivateModeFrom(fvw::CameraMode previous_camera_mode);
   void Activate2D();

   //
   bool no_intersection_matrix_set;
   GeospatialViewer* viewer;
   int m_desired_zoom_percent;

   // Sets the distance from the focal point in world coordinates.
   //
   // The incoming distance value will be clamped within the valid range
   // specified by the settings.
   virtual void setDistance(double distance);

   virtual void home(double /*unused*/) override { }
   virtual void home(const osgGA::GUIEventAdapter& ea,
      osgGA::GUIActionAdapter& us) override { }

   virtual void rotate(double dx, double dy) override;

   void ComputeMapBounds(osg::View* view, d_geo_rect_t* geo_rect);

   int frames_to_redraw;

private:
   std::vector<EarthManipulatorEvents_Interface*> m_listeners;
};

class PerformChangeToPanMode : public AnimatedTransitionCallback
{
public:
   PerformChangeToPanMode(GeospatialViewer* viewer, GeospatialEarthManipulator* manipulator);
private:
   void AnimationFrame(double t) override;
   GeospatialViewer* m_viewer;
   GeospatialEarthManipulator* m_manipulator;

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

class PerformChangeFromPanModeTo2D : public AnimatedTransitionCallback
{
public:
   PerformChangeFromPanModeTo2D(GeospatialViewer* viewer, GeospatialEarthManipulator* manipulator);
private:
   void AnimationFrame(double t) override;
   GeospatialViewer* m_viewer;
   GeospatialEarthManipulator* m_manipulator;

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



#endif  // FALCONVIEW_GEOSPATIALEARTHMANIPULATOR_H_
