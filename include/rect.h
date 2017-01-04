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



#ifndef GRA_RECT_H
#define GRA_RECT_H

typedef double GRA_rotation_angle_t;

//
// This class encapsulates the calculations and information needed to rotate
// a rectangle (in logical units).  This rectangle is called the surface.  This
// class calculates the larger-than-the-surface, non-rotated rectangle that 
// is needed to ensure that the rotated surface will be comletely filled.  This
// larger, non-rotated rectangle is referred to as the virtual surface.
//
// This class translates coordinates between the larger, unrotated virtual 
// surface and the rotated surface.
//
class rotatable_rectangle
{

public:

   rotatable_rectangle();
   virtual ~rotatable_rectangle() {}

   int set_rotation(GRA_rotation_angle_t angle,
      int surface_width, int surface_height);
   GRA_rotation_angle_t get_rotation(void) const
   {
      return m_rot_angle;
   }

   //
   // virtual surface functions
   //
   int get_vsurface_width(void) const 
   {
      return m_virtual_surface_width;
   }
   int get_vsurface_height(void) const
   {
      return m_virtual_surface_height;
   }
   void get_vsurface_size(int* vsurface_width, int* vsurface_height) const
   {
      *vsurface_width = m_virtual_surface_width;
      *vsurface_height = m_virtual_surface_height;
   }

   //
   // surface functions
   //
   int get_surface_width(void) const
   {
      return m_actual_surface_width; 
   }
   int get_surface_height(void) const
   {
      return m_actual_surface_height; 
   }
   void get_surface_size(int* surface_width, int* surface_height) const;

   //
   // conversion functions - converts from the virtual surface 
   // coordinates to actual surface coordinates and vice-versa
   //
   int surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const;
   int surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const;
   int vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const;
   int vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const;

   virtual int rotate_and_translate_point(double x, double y, double* new_x, double* new_y) const;
   virtual int rotate_and_translate_point(int x, int y, int* new_x, int* new_y) const;
   virtual int untranslate_and_unrotate_point(double x, double y, 
      double* new_x, double* new_y) const;
   virtual int untranslate_and_unrotate_point(int x, int y, 
      int* new_x, int* new_y) const;

protected:

   virtual int set_rotation_angle(GRA_rotation_angle_t angle);

   /*
    *  translate a screen coordinate from the (possibly) larger virtual
    *  surface to the actual surface coordinates
    */
   virtual int translate_point(double x, double y, double* trans_x, double* trans_y) const;
   virtual int translate_point(int x, int y, int* trans_x, int* trans_y) const;
   virtual int untranslate_point(double x, double y, double* untrans_x, double* untrans_y) const;
   virtual int untranslate_point(int x, int y, int* untrans_x, int* untrans_y) const;

   virtual int rotate_point(double x, double y, double* rot_x, double* rot_y) const;
   virtual int rotate_point(int x, int y, int* rot_x, int* rot_y) const;
   virtual int unrotate_point(double rot_x, double rot_y, double* unrot_x, double* unrot_y) const;
   virtual int unrotate_point(int rot_x, int rot_y, int* unrot_x, int* unrot_y) const;

protected:

   //
   //  the angle of rotation in degrees
   //
   //  m_rot_angle > 0  =>  counter clockwise rotation
   //
   GRA_rotation_angle_t m_rot_angle;

   double m_cos;           // cos of m_rot_angle
   double m_sin;           // sin of m_rot_angle
   double m_cos_opposite;  // the cos of -1*m_rot_angle
   double m_sin_opposite;  // the sin of -1*m_rot_angle
   int m_rot_x;            // the x coordinant of rotation (surface coordinate)
   int m_rot_y;            // the y coordinant of rotation (surface coordinate)
   int m_rot_y_opp;        // -1*m_rot_y (surface coordinate)

   //
   // The dimensions of the larger, unrotated "virtual" surface.  
   // Only valid if the surface is in fact rotated.
   //
   int m_virtual_surface_width;
   int m_virtual_surface_height;

   //
   // The dimensions of the possibly rotated actual surface
   //
   int m_actual_surface_width;
   int m_actual_surface_height;

   //
   // The offset of the actual surface within the virtual surface .
   // Only valid if the surface is in fact rotated.
   //
   int m_surface_ul_x_in_vsurface;
   int m_surface_ul_y_in_vsurface;
};


#endif
