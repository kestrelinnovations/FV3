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
   Filename :  vector.h
   Date     :  1998-JUL-6
   Author   :  Ray E. Bornert II
   Purpose  :  inline definitions for vectors
*/

//**************************************************************
// GATE
//**************************************************************
#ifndef _VECTOR_H_
#define _VECTOR_H_

//**************************************************************
// FOWARD
//**************************************************************

//**************************************************************
// INCLUDES
//**************************************************************
#include "lineardef.h"

//**************************************************************
// DEFINES
//**************************************************************

//**************************************************************
// CLASS
//**************************************************************

//**************************************************************
// CONSTRUCTORS
//**************************************************************
inline Vector3::Vector3(void)
{
}

inline Vector3::Vector3(float a, float b, float c)
{
   x = a;
   y = b;
   z = c;
}

//**************************************************************
// OPERATIONS
//**************************************************************

////////////////////////////////////////////////////////////////
// ASSIGNMENT
////////////////////////////////////////////////////////////////
inline Vector3 &Vector3::operator=(const Vector3 &v)
{
   x=v.x;
   y=v.y;
   z=v.z;
   return *this;
}

////////////////////////////////////////////////////////////////
// ADDITION
////////////////////////////////////////////////////////////////
inline Vector3 Vector3::operator+(const Vector3 &v) const
{
   return Vector3(x + v.x, y + v.y, z + v.z);
}

inline Vector3 &Vector3::operator+=(const Vector3 &v)
{
   return *this = *this + v;
}

inline void Vector3::add(const Vector3 &u, const Vector3 &v)
{
   *this = u + v;
}

inline void Vector3::add(const Vector3 &u)
{
   *this += u;
}

////////////////////////////////////////////////////////////////
// SUBTRACTION
////////////////////////////////////////////////////////////////
inline Vector3 Vector3::operator-(const Vector3 &v) const
{
   return Vector3(x - v.x, y - v.y, z - v.z);
}

inline Vector3 &Vector3::operator-=(const Vector3 &v)
{
   return *this = *this - v;
}

inline void Vector3::sub(const Vector3 &u, const Vector3 &v)
{
   *this = u - v;
}

inline void Vector3::sub(const Vector3 &u)
{
   *this -= u;
}

////////////////////////////////////////////////////////////////
// MULTIPLICATION
////////////////////////////////////////////////////////////////
inline Vector3 Vector3::operator*(float c) const
{
   return Vector3(x * c, y * c, z * c);
}

inline Vector3 Vector3::operator*(int c) const
{
   return *this * (float)c;
}

inline Vector3 &Vector3::operator*=(float c)
{
   return *this = *this * c;
}

inline Vector3 &Vector3::operator*=(int c)
{
   return *this = *this * c;
}

inline void Vector3::mul(const Vector3 &u, float c)
{
   *this = u * c;
}

inline void Vector3::mul(const Vector3 &u, int c)
{
   *this = u * c;
}

inline void Vector3::mul(float c)
{
   *this *= c;
}

inline void Vector3::mul(int c)
{
   *this *= c;
}

////////////////////////////////////////////////////////////////
// DIVISION
////////////////////////////////////////////////////////////////
inline Vector3 Vector3::operator/(float c) const
{
   ASSERT(c);
   return *this * (1.0f / c);
}

inline Vector3 Vector3::operator/(int c) const
{
   return *this / (float)c;
}

inline Vector3 &Vector3::operator/=(float c)
{
   return *this = *this / c;
}

inline Vector3 &Vector3::operator/=(int c)
{
   return *this = *this / c;
}

inline void Vector3::div(float c)
{
   *this /= c;
}

inline void Vector3::div(int c)
{
   *this /= c;
}

inline void Vector3::div(const Vector3 &u, float c)
{
   *this = u / c;
}

inline void Vector3::div(const Vector3 &u, int c)
{
   *this = u / c;
}

////////////////////////////////////////////////////////////////
// BOOLEAN
////////////////////////////////////////////////////////////////
inline bool Vector3::operator==(const Vector3 &v) const
{
   return (x == v.x && y == v.y && z == v.z);
}

inline bool Vector3::operator!=(const Vector3 &v) const
{
   return (x != v.x || y != v.y || z != v.z);
}

inline bool Vector3::isZero(void) const
{
   return (x == 0.0f && y == 0.0f && z == 0.0f);
}

inline bool Vector3::isNormal(void) const
{
   return ( fabs(1.0f-magnitudeSquared()) < 0.00000013 );
}

////////////////////////////////////////////////////////////////
// NORMALIZATION
////////////////////////////////////////////////////////////////
inline void Vector3::zero(void)
{
   *this = Vector3(0,0,0);
}

inline void Vector3::normalize(void)
{
   *this /= magnitude();
}

////////////////////////////////////////////////////////////////
// CROSS
////////////////////////////////////////////////////////////////
inline Vector3 Vector3::operator%(const Vector3 &v) const
{
   return crossProduct(*this,v);
}

inline Vector3 &Vector3::operator%=(const Vector3 &v)
{
   return *this = *this % v;
}

inline void Vector3::cross(const Vector3 &v)
{
   *this %= v;
}

inline void Vector3::cross(const Vector3 &u, const Vector3 &v)
{
   *this = u % v;
}

////////////////////////////////////////////////////////////////
// DOT
////////////////////////////////////////////////////////////////
inline float Vector3::operator^(const Vector3 &v) const
{
   return dotProduct(*this,v);
}

inline float Vector3::dot(const Vector3 &v) const
{
   return *this ^ v;
}

////////////////////////////////////////////////////////////////
// MAGNITUDE
////////////////////////////////////////////////////////////////
inline float Vector3::magnitude(void) const
{
   return (float)sqrt( dot(*this) );
}

inline float Vector3::magnitudeSquared(void) const
{
   return ( dot(*this) );
}

////////////////////////////////////////////////////////////////
// DISTANCE
////////////////////////////////////////////////////////////////
inline float Vector3::distance(const Vector3 &v) const
{
   return (*this - v).magnitude();
}

inline float Vector3::distanceSquared(const Vector3 &v) const
{
   return (*this - v).magnitudeSquared();
}


//**************************************************************
// GATE END
//**************************************************************
#endif
