// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
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



/*
   Filename :  vector.cpp
   Date     :  1998-JUL-6
   Author   :  Ray E. Bornert II
   Purpose  :  implementation code for vectors
*/

//**************************************************************
// INCLUDES
//**************************************************************
#include "stdafx.h"
#include "vector.h"

//**************************************************************
// DEFINES
//**************************************************************

//**************************************************************
// CODE
//**************************************************************

////////////////////////////////////////////////////////////////
// DOT
////////////////////////////////////////////////////////////////

//private: static
float Vector3::dotProduct(const Vector3 &u, const Vector3 &v)
{
   return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

void Vector3::dot(const Vector3 *v, float *dotProduct, int count)
{
   for ( ; count--; v++, dotProduct++)
      *dotProduct = dot(*v);
}

////////////////////////////////////////////////////////////////
// CROSS
////////////////////////////////////////////////////////////////

//private: static
Vector3 Vector3::crossProduct(const Vector3 &u, const Vector3 &v)
{
   Vector3 t;

   t.x = u.y*v.z - u.z*v.y;
   t.y = u.z*v.x - u.x*v.z;
   t.z = u.x*v.y - u.y*v.x;

   return t;
}