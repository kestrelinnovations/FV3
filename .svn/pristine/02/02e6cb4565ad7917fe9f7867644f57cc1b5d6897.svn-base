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



#include "stdafx.h"
#include "maps.h"  // for MAP_calc_screen_size_for_a_rotated_map
#include "errx.h"
#include "rect.h"


static int rotate_pt(int x, int y, int* new_x, int* new_y,
   double cos_of_angle, double sin_of_angle, 
   int rot_pt_x, int neg_rot_pt_y);

static int rotate_pt(double x, double y, double* new_x, double* new_y,
   double cos_of_angle, double sin_of_angle, 
   double rot_pt_x, double neg_rot_pt_y);

static inline
int round_double(double d)
{
   if (d >= 0)
      return ((int)(d+0.5));
   else
      return ((int)(d-0.5));
}

rotatable_rectangle::rotatable_rectangle()
{
   set_rotation_angle(0.0);

   m_rot_x = 0;
   m_rot_y = 0;
   m_rot_y_opp = 0;

   m_virtual_surface_width = 0;
   m_virtual_surface_height = 0;
   m_surface_ul_x_in_vsurface = 0;
   m_surface_ul_y_in_vsurface = 0;

   m_actual_surface_width = 0;
   m_actual_surface_height = 0;
}

int rotatable_rectangle::set_rotation_angle(GRA_rotation_angle_t angle)
{
   m_rot_angle = angle;

   radians_t rot_angle_in_radians = DEG_TO_RAD(m_rot_angle);

   m_cos = cos(rot_angle_in_radians);
   m_sin = sin(rot_angle_in_radians);
   m_cos_opposite = cos(-1.0*rot_angle_in_radians);
   m_sin_opposite = sin(-1.0*rot_angle_in_radians);

   return SUCCESS;
}

int rotatable_rectangle::set_rotation(GRA_rotation_angle_t angle,
   int surface_width, int surface_height)
{
   if (set_rotation_angle(angle) != SUCCESS)
   {
      ERR_report("set_rotation_angle");;
      return FAILURE;
   }

   if (angle != 0.0)
   {
      int virtual_surface_width;
      int virtual_surface_height;

      if (MAP_calc_screen_size_for_a_rotated_map(surface_width, surface_height, 
         &virtual_surface_width, &virtual_surface_height, angle) != SUCCESS)
      {
         ERR_report("MAP_calc_screen_size");
         return FAILURE;
      }

      m_rot_x = virtual_surface_width/2;
      m_rot_y = virtual_surface_height/2;
      m_rot_y_opp = -1*m_rot_y;

      m_surface_ul_x_in_vsurface = (virtual_surface_width - surface_width)/2;
      m_surface_ul_y_in_vsurface = (virtual_surface_height - surface_height)/2;

      m_virtual_surface_width = virtual_surface_width;
      m_virtual_surface_height = virtual_surface_height;
   }
   else
   {
      // NOTE ************* NEED TO SET THESE VALUES EVEN IF NOT ROTATED;

      m_rot_x = 0;
      m_rot_y = 0;
      m_rot_y_opp = 0;

      m_virtual_surface_width = surface_width;
      m_virtual_surface_height = surface_height;
      
      m_surface_ul_x_in_vsurface = 0;
      m_surface_ul_y_in_vsurface = 0;
   }

   m_actual_surface_width = surface_width;
   m_actual_surface_height = surface_height;

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::translate_point(double x, double y, double* trans_x, 
   double* trans_y) const
{
   *trans_x = x - (double) m_surface_ul_x_in_vsurface;
   *trans_y = y - (double) m_surface_ul_y_in_vsurface;

   return SUCCESS;
}


// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::translate_point(int x, int y, int* trans_x, 
   int* trans_y) const
{
   *trans_x = x - m_surface_ul_x_in_vsurface;
   *trans_y = y - m_surface_ul_y_in_vsurface;

   return SUCCESS;
}


// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::untranslate_point(double x, double y, double* untrans_x, 
   double* untrans_y) const
{
   *untrans_x = x + (double) m_surface_ul_x_in_vsurface;
   *untrans_y = y + (double) m_surface_ul_y_in_vsurface;

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::untranslate_point(int x, int y, int* untrans_x, 
   int* untrans_y) const
{
   *untrans_x = x + m_surface_ul_x_in_vsurface;
   *untrans_y = y + m_surface_ul_y_in_vsurface;

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::rotate_point(int x, int y, int* rot_x, int* rot_y) const
{
   return rotate_pt(x, y, rot_x, rot_y, m_cos, m_sin, m_rot_x, m_rot_y_opp);
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::rotate_point(double x, double y, double* rot_x, double* rot_y) const
{
   return rotate_pt(x, y, rot_x, rot_y, m_cos, m_sin, m_rot_x, m_rot_y_opp);
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::unrotate_point(int rot_x, int rot_y, 
   int* unrot_x, int* unrot_y) const
{
   return rotate_pt(rot_x, rot_y, unrot_x, unrot_y, 
      m_cos_opposite, m_sin_opposite, m_rot_x, m_rot_y_opp);
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::unrotate_point(double rot_x, double rot_y, 
   double* unrot_x, double* unrot_y) const
{
   return rotate_pt(rot_x, rot_y, unrot_x, unrot_y, 
      m_cos_opposite, m_sin_opposite, m_rot_x, m_rot_y_opp);
}

// ***********************************************************************
// ***********************************************************************

static
int rotate_pt(int x, int y, int* new_x, int* new_y,
   double cos_of_angle, double sin_of_angle, 
   int rot_pt_x, int neg_rot_pt_y)
{
   int tmp_x;
   int tmp_y; 
   int tmp2_x;
   int tmp2_y;

   // "flip" coordinate system
   tmp_x = x;
   tmp_y = -1*y;

   // translate rotation point to origin
   tmp_x  -= rot_pt_x;
   tmp_y  -= neg_rot_pt_y;

   //do rotation
   tmp2_x = round_double(tmp_x*cos_of_angle - tmp_y*sin_of_angle);
   tmp2_y = round_double(tmp_x*sin_of_angle + tmp_y*cos_of_angle);

   tmp_x = tmp2_x;
   tmp_y = tmp2_y;

   // untranslate
   tmp_x += rot_pt_x;
   tmp_y += neg_rot_pt_y;

   // "unflip" coordinate system
   *new_x = tmp_x;
   *new_y = -1*tmp_y;

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

static int rotate_pt(double x, double y, double* new_x, double* new_y,
   double cos_of_angle, double sin_of_angle, 
   double rot_pt_x, double neg_rot_pt_y)
{
   double tmp_x;
   double tmp_y; 
   double tmp2_x;
   double tmp2_y;

   // "flip" coordinate system
   tmp_x = x;
   tmp_y = -1*y;

   // translate rotation point to origin
   tmp_x  -= rot_pt_x;
   tmp_y  -= neg_rot_pt_y;

   //do rotation
   tmp2_x = tmp_x*cos_of_angle - tmp_y*sin_of_angle;
   tmp2_y = tmp_x*sin_of_angle + tmp_y*cos_of_angle;

   tmp_x = tmp2_x;
   tmp_y = tmp2_y;

   // untranslate
   tmp_x += rot_pt_x;
   tmp_y += neg_rot_pt_y;

   // "unflip" coordinate system
   *new_x = tmp_x;
   *new_y = -1*tmp_y;

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::rotate_and_translate_point(double x, double y, 
   double* new_x, double* new_y) const
{
   double rot_x; 
   double rot_y; 

   rotate_point(x, y, &rot_x, &rot_y);
   translate_point(rot_x, rot_y, new_x, new_y);

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::rotate_and_translate_point(int x, int y, 
   int* new_x, int* new_y) const
{
   int rot_x; 
   int rot_y; 

   rotate_point(x, y, &rot_x, &rot_y);
   translate_point(rot_x, rot_y, new_x, new_y);

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::untranslate_and_unrotate_point(double x, double y,
   double* new_x, double* new_y) const
{
   double untrans_x;
   double untrans_y;

   untranslate_point(x, y, &untrans_x, &untrans_y);

   if (unrotate_point(untrans_x, untrans_y, new_x, new_y) != SUCCESS)
   {
      ERR_report("unrotate_point failed");
      return FAILURE;
   }

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::untranslate_and_unrotate_point(int x, int y,
   int* new_x, int* new_y) const
{
   int untrans_x;
   int untrans_y;

   untranslate_point(x, y, &untrans_x, &untrans_y);

   if (unrotate_point(untrans_x, untrans_y, new_x, new_y) != SUCCESS)
   {
      ERR_report("unrotate_point failed");
      return FAILURE;
   }

   return SUCCESS;
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::surface_to_vsurface(double s_x, double s_y, double* vs_x, 
   double* vs_y) const
{
   if (get_rotation() == 0.0)
   {
      *vs_x = s_x;
      *vs_y = s_y;
      return SUCCESS;
   }

   return untranslate_and_unrotate_point(s_x, s_y, vs_x, vs_y);
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::surface_to_vsurface(int s_x, int s_y, int* vs_x, 
   int* vs_y) const
{
   if (get_rotation() == 0.0)
   {
      *vs_x = s_x;
      *vs_y = s_y;
      return SUCCESS;
   }

   return untranslate_and_unrotate_point(s_x, s_y, vs_x, vs_y);
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::vsurface_to_surface(double vs_x, double vs_y, double* s_x, 
   double* s_y) const
{
   if (get_rotation() == 0.0)
   {
      *s_x = vs_x;
      *s_y = vs_y;
      return SUCCESS;
   }

   return rotate_and_translate_point(vs_x, vs_y, s_x, s_y);
}

// ***********************************************************************
// ***********************************************************************

int rotatable_rectangle::vsurface_to_surface(int vs_x, int vs_y, int* s_x, 
   int* s_y) const
{
   if (get_rotation() == 0.0)
   {
      *s_x = vs_x;
      *s_y = vs_y;
      return SUCCESS;
   }

   return rotate_and_translate_point(vs_x, vs_y, s_x, s_y);
}

// ***********************************************************************
// ***********************************************************************

void rotatable_rectangle::get_surface_size(int* surface_width, 
   int* surface_height) const
{
   *surface_width = m_actual_surface_width;
   *surface_height = m_actual_surface_height;
}
