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

// FvMath.h

// cA, sA are cos(angle), sin(angle) respectively

#ifndef FALCONVIEW_MATH
#define FALCONVIEW_MATH

#pragma once

const static osg::Vec3d north_vector(0.0, 0.0, 1.0);

static inline void RotationAroundAxis3D(osg::Vec3d* A, osg::Vec3d* B, float cA, float sA)
{
   osg::Vec3d t1(*A);
   B->set(
      (cA+pow(t1.x(),2)*(1.0-cA))*B->x()+
      (t1.x()*t1.y()*(1.0-cA)-t1.z()*sA)*B->y()+
      (t1.x()*t1.z()*(1.0-cA)+t1.y()*sA)*B->z(),
      (t1.y()*t1.x()*(1.0-cA)+t1.z()*sA)*B->x()+
      (cA+pow(t1.y(),2)*(1.0-cA))*B->y()+
      (t1.y()*t1.z()*(1.0-cA)-t1.x()*sA)*B->z(),
      (t1.z()*t1.x()*(1.0-cA)-t1.y()*sA)*B->x()+
      (t1.z()*t1.y()*(1.0-cA)+t1.x()*sA)*B->y()+
      (cA+pow(t1.z(),2)*(1.0-cA))*B->z()
   );
}

static inline double safe_acos(double x)
{
   if (x < -1.0)
      x = -1.0;
   else if (x > 1.0)
      x = 1.0;
   return acos(x) ;
}

static inline double safe_asin(double x)
{
   if (x < -1.0)
      x = -1.0;
   else if (x > 1.0)
      x = 1.0;
   return asin(x) ;
}

// Linear interpolation between two position, orientation pairs
static inline void Lerp3D(osg::Vec3d *output, osg::Vec3d *A, osg::Vec3d *B, float t) {
   output->x() = A->x() + t*(B->x()-A->x());
   output->y() = A->y() + t*(B->y()-A->y());
   output->z() = A->z() + t*(B->z()-A->z());
}

#endif
