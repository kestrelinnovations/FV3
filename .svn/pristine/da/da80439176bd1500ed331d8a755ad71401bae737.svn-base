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
   Filename :  lineardef.h
   Date     :  1998-JUL-9
   Author   :  Ray E. Bornert II
   Purpose  :  class definitions for Vectors & Matrices
*/

#ifndef _LINEARDEF_H_
#define _LINEARDEF_H_

////////////////////////////////////////////////////////////////
// VECTOR3
////////////////////////////////////////////////////////////////
class Vector3
{
public:
   //allow references scalars or as an array
   union
   {
      float a[3];
      struct
      {
      float x;
      float y;
      float z;
      };
   };

public:
   //CONSTRUCTOR
   Vector3(void);
   Vector3(float a, float b, float c);

   //ASSIGNMENT
   Vector3 &operator=(const Vector3 &v);

   //ADDITION
   Vector3 operator+(const Vector3 &v) const;
   Vector3 &operator+=(const Vector3 &v);
   void   add(const Vector3 &u, const Vector3 &v);
   void   add(const Vector3 &u);

   //SUBTRACTION
   Vector3 operator-(const Vector3 &v) const;
   Vector3 &operator-=(const Vector3 &v);
   void   sub(const Vector3 &u, const Vector3 &v);
   void   sub(const Vector3 &u);

   //MULTIPLICATION
   Vector3 operator*(float c) const;
   Vector3 operator*(int c) const;
   Vector3 &operator*=(float c);
   Vector3 &operator*=(int c);
   void   mul(float c);
   void   mul(int c);
   void   mul(const Vector3 &u, float c);
   void   mul(const Vector3 &u, int c);

   //DIVISION
   Vector3 operator/(float c) const;
   Vector3 operator/(int c) const;
   Vector3 &operator/=(float c);
   Vector3 &operator/=(int c);
   void   div(float c);
   void   div(int c);
   void   div(const Vector3 &u, float c);
   void   div(const Vector3 &u, int c);

   //BOOLEAN
   bool  operator==(const Vector3 &v) const;
   bool  operator!=(const Vector3 &v) const;
   bool  isZero(void) const;
   bool  isNormal(void) const;

   //NORMALIZATION
   void   zero(void);
   void   normalize(void);

   //CROSS
   Vector3 operator%(const Vector3 &v) const;
   Vector3 &operator%=(const Vector3 &v);
   void    cross(const Vector3 &v);
   void    cross(const Vector3 &u, const Vector3 &v);

   //DOT
   float  operator^(const Vector3 &v) const;
   float  dot(const Vector3 &v) const;
   void   dot(const Vector3 *v, float *dotProduct, int count);

   //MAGNITUDE
   float  magnitude(void) const;
   float  magnitudeSquared(void) const;

   //DISTANCE
   float  distance(const Vector3 &b) const;
   float  distanceSquared(const Vector3 &b) const;

private:
   static
   Vector3 crossProduct(const Vector3 &u, const Vector3 &v);

   static
   float  dotProduct(const Vector3 &u, const Vector3 &v);

};

#endif
