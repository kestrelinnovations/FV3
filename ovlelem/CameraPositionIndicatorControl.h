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


// CameraPositionIndicatorControl.h

#ifndef OVLELEM_CAMERAPOSITIONINDICATORCONTROL_H_
#define OVLELEM_CAMERAPOSITIONINDICATORCONTROL_H_

#include "geo_tool.h"
#include "osgEarthUtil/Controls"

class CameraPositionIndicatorControl : public osgEarth::Util::Controls::Control
{
public:
   CameraPositionIndicatorControl(const d_geo_t& target,
      osg::Vec3d& target_vec,
      double length, COLORREF color) :
      m_target(target), m_target_vec(target_vec),
      m_length(length), m_color(color)
   {
   }

   virtual ~CameraPositionIndicatorControl() { }

   void Update(const d_geo_t& camera_location,
      const osg::Matrixd& view_matrix);

public:
   virtual void draw(const osgEarth::Util::Controls::ControlContext& cx,
      osgEarth::Util::Controls::DrawableList& out );

private:
   // There are currently two implementations of how the heading is
   // computed and drawn, hence the m_target and m_target_vec.
   //  i) Compute the bearing from the camera's latitude, longitude, ignoring
   //  altitude, to the target latitude, longitude (uses m_target)
   //  ii) Compute the vector from the camera's position in 3D space to the
   //  target's position in 3D space and render that vector.
   d_geo_t m_target;
   osg::Vec3d m_target_vec;

   double m_length;
   COLORREF m_color;
   float m_vp_height;
};

#endif  // OVLELEM_CAMERAPOSITIONINDICATORCONTROL_H_