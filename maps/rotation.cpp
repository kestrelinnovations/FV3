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

#include "maps.h"

/*
 *  This is public for the sake of TIROS, which needs to know the screen
 *  size (to figure out degrees per pixel) before calling 
 *  MAP_set_equal_src_projection
 */
int MAP_calc_screen_size_for_a_rotated_map(int surface_width, int surface_height,
   int* vwidth, int* vheight, double rotation)
{
   //
   // If map is rotated, then figure out the size that the larger "virtual"
   // screen needs to be in order to accomodate any rotation.
   //  
   if (rotation != 0.0)
   {
      const double rot_angle_radians = DEG_TO_RAD(rotation);
      *vwidth = 
         static_cast<int>(ceil(surface_width*fabs(cos(rot_angle_radians)) + 
            surface_height*fabs(sin(rot_angle_radians))));
      *vheight = 
         static_cast<int>(ceil(surface_height*fabs(cos(rot_angle_radians)) + 
            surface_width*fabs(sin(rot_angle_radians))));
   }
   else
   {
      *vwidth = surface_width;
      *vheight = surface_height;
   }

   return SUCCESS;
}


/* Returns TRUE if rotation is valid for the given map scale,
   and FALSE otherwise.  If the scale is WORLD then it returns
   FALSE.  If the virtual map size is larger than the data set,
   then the function returns FALSE. */

boolean_t MAP_rotation_valid(MapScale scale, int surface_width, 
                             int surface_height)
{
   bool valid_surface;
   bool can_rotate;
   bool can_zoom;
   int minimum_zoom;

   if (MAP_test_limits(scale, surface_width, surface_height,valid_surface, 
      can_rotate, can_zoom, minimum_zoom) == SUCCESS && valid_surface)
      return can_rotate;

   return false;
}
