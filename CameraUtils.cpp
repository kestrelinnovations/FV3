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

// CameraUtils.cpp
#include "stdafx.h"
#include "CameraUtils.h"

#include "osg/Camera"
#include "osgEarth/SpatialReference"

#include "FalconView/GeospatialView.h"

osg::Matrixd camera_utils::ViewMatrixFromGeo(
   const osgEarth::SpatialReference* srs, double lat, double lon,
   double alt_meters, double heading, double incline, double roll)
{
   double lat_rad = DEG_TO_RAD(lat);
   double lon_rad = DEG_TO_RAD(lon);
   double heading_rad = DEG_TO_RAD(heading);

   // Compute eye point in world coordinates
   osg::Vec3d eye;
   srs->getEllipsoid()->convertLatLongHeightToXYZ(
      lat_rad, lon_rad, alt_meters, eye.x(), eye.y(), eye.z());

   // Build matrix for computing target vector
   osg::Matrixd target_matrix = osg::Matrixd::rotate(
      -heading_rad, osg::Vec3d(1,0,0),
      -lat_rad, osg::Vec3d(0,1,0),
      lon_rad, osg::Vec3d(0,0,1));

   // Compute tangent vector
   osg::Vec3d tangent = target_matrix.preMult(osg::Vec3d(0, 0, 1));

   // Compute non-inclined, non-rolled up vector
   osg::Vec3d up(eye);
   up.normalize();

   // Incline by rotating the target and up vector around the tangent/up-vector
   // cross-product
   osg::Vec3d up_cross_tangent = up ^ tangent;
   osg::Matrixd incline_matrix = osg::Matrixd::rotate(DEG_TO_RAD(incline),
      up_cross_tangent);
   osg::Vec3d target = incline_matrix.preMult(tangent);

   // Roll by rotating the up vector around the target vector
   osg::Matrixd roll_matrix = incline_matrix * osg::Matrixd::rotate(
      DEG_TO_RAD(roll), target);
   up = roll_matrix.preMult(up);

   return osg::Matrixd::lookAt(eye, eye+target, up);
}

double camera_utils::ComputeDegreesPerPixel(
   const osgEarth::SpatialReference* srs,
   osg::Camera* camera, double height_above_ellipsoid)
{
   double fovy, aspectRatio, zNear, zFar;
   camera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

   osg::Matrixd projection_matrix = osg::Matrixd::perspective(
      fovy, aspectRatio, height_above_ellipsoid, 2 * height_above_ellipsoid);

   auto viewport = camera->getViewport();

   // Compute a matrix that takes a screen coordinate to the world coordinate
   // system
   const osg::Matrix window_matrix = viewport->computeWindowMatrix();
   osg::Matrix screen_to_world = osg::Matrix::inverse(
      camera_utils::ViewMatrixFromGeo(srs, 0.0, -90.0, height_above_ellipsoid,
      0.0, 90.0, 0.0) * projection_matrix * window_matrix);

   // Transform the center and a pixel above the center to the world
   // coordinate system
   //
   double center_x = viewport->width() / 2.0;
   double center_y = viewport->height() / 2.0;

   osg::Vec3d c1(center_x, center_y, 0.0);
   osg::Vec3d c2(center_x, center_y + 1.0, 0.0);

   osg::Vec3d c1_world = screen_to_world.preMult(c1);
   osg::Vec3d c2_world = screen_to_world.preMult(c2);

   // Convert the world coordinate to geodetic coordinates
   osg::Vec3d c1_geo, c2_geo;
   double unused;
   srs->transformFromWorld(c1_world, c1_geo, &unused);
   srs->transformFromWorld(c2_world, c2_geo, &unused);

   // Compute the degrees per pixel latitude
   return fabs(c2_geo.y() - c1_geo.y());
}

double camera_utils::ComputeHeightFromDegreesPerPixel(
   const osgEarth::SpatialReference* srs,
   osg::Camera* camera, double degrees_per_pixel)
{
   double high_height = STARTING_HEIGHT_ABOVE_ELLIPSOID_METERS * 2;
   double low_height = 0;

   // Use iterative method to compute height above ellipsoid rather than
   // calculating the closed form inverse of ComputeDegreesPerPixel.
   double center_height;
   for (int i = 0; i < 32; ++i)
   {
      center_height = (high_height + low_height) / 2.0;

      double dpp = camera_utils::ComputeDegreesPerPixel(srs, camera,
         center_height);
      if (fabs(dpp - degrees_per_pixel) < 1e-6)
      {
         return center_height;
      }
      if (dpp < degrees_per_pixel)
         low_height = center_height;
      else
         high_height = center_height;
   }

   //ASSERT(0);  // expecting above loop to converge faster or is World
   return STARTING_HEIGHT_ABOVE_ELLIPSOID_METERS;
}

double camera_utils::ComputeHeightFromMapTypeAndZoom(
   const osgEarth::SpatialReference* srs, osg::Camera* camera,
   const MapType& map_type, int zoom_percent)
{
   double native_height = camera_utils::ComputeHeightFromDegreesPerPixel(
      srs, camera, map_type.get_scale().get_nominal_degrees_lat_per_pixel());

   return 100.0 * native_height / zoom_percent;
}