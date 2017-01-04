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

// CameraUtils.h

#include "osg/Matrixd"

namespace osg
{
   class Camera;
}
namespace osgEarth
{
   class SpatialReference;
}

namespace camera_utils
{
osg::Matrixd ViewMatrixFromGeo(const osgEarth::SpatialReference* srs,
      double lat, double lon, double alt_meters,
      double heading, double incline, double roll);

// Given a height above ellipsoid compute degrees per pixel
double ComputeDegreesPerPixel(const osgEarth::SpatialReference* srs,
   osg::Camera* camera, double height_above_ellipsoid);

double ComputeHeightFromDegreesPerPixel(
   const osgEarth::SpatialReference* srs, osg::Camera* camera,
   double degrees_per_pixel);

double ComputeHeightFromMapTypeAndZoom(
   const osgEarth::SpatialReference* srs, osg::Camera* camera,
   const MapType& map_type, int zoom_percent);
}