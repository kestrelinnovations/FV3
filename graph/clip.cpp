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
#include <stdio.h>

#include "clip.h"
#include "common.h"
#include "utils.h"   // for UTL_is_win32_nt

static boolean_t clip_t(int denom, int num, float *tE, float *tL);

static
int lcl_clip_point(int x, int y, int min_x, int min_y, int max_x, int max_y)
{
   int flags = 0;

   if (x > max_x)
      flags = GRA_CLIP_RIGHT;
   else if (x < min_x)
      flags = GRA_CLIP_LEFT;

   if (y > max_y)
      flags |= GRA_CLIP_TOP;
   else if (y < min_y)
      flags |= GRA_CLIP_BOTTOM;

   return flags;
}       

// returns TRUE for visible and FALSE otherwise

boolean_t gra_clip_line(int *x1, int *y1, int *x2, int *y2,
   int min_x, int min_y, int max_x, int max_y)
{
   int flags1;    // bit flags TBRL for point 1
   int flags2;    // bit flags TBRL for point 2
   int xor;       // flags1 XOR flags2
   float tE, tL;  // t at entering and leaving point
   int dx, dy;    // run and rise

   //
   // Define the clip box by setting its width and height.
   // The clip box is from (min_x-100, min_y-100) to (max_x+ 100, max_y+ 100), in order
   // to insure that you do not have to worry about line thickness, style, or
   // icon size when clipping a point or line.  
   //
   // Because of SHORT limitations on Windows 95, the width and height must 
   // be no more than 100 less then SHRT_MAX.
   //
   min_x -= 100;
   min_y -= 100;
   max_x += 100;
   max_y += 100;

   //
   // get clip flags for points
   //
   // NOTE: make sure to call lcl_clip_point here (rather than gra_clip_point) to
   // ensure that the clip bounds don't get adjusted twice.
   //
   flags1 = lcl_clip_point(*x1, *y1, min_x, min_y, max_x, max_y);
   flags2 = lcl_clip_point(*x2, *y2, min_x, min_y, max_x, max_y); 

   // quick accept 
   if ((flags1 | flags2) == 0)
      return TRUE;
 
   // quick reject
   if (flags1 & flags2)
      return FALSE;

   // parametric line P(t) = P1 + (P2 - P1)t 
   dx = *x2 - *x1;		// run
   dy = *y2 - *y1;		// rise
   tE = (float)0.0;		// t at P1 
   tL = (float)1.0;		// t at P2

   // bit(s) set in xor indicate which edges are crossed
   xor = flags1 ^ flags2;

   // clip to left edge if crossed 
   if (xor & GRA_CLIP_LEFT)
   {
      if (clip_t(dx, min_x - *x1, &tE, &tL) == FALSE)
         return FALSE;
   }

   // clip to right edge if crossed 
   if (xor & GRA_CLIP_RIGHT)
   {
      if (clip_t(-dx, *x1 - max_x, &tE, &tL) == FALSE)
         return FALSE;
   }

   // clip to bottom edge if crossed 
   if (xor & GRA_CLIP_BOTTOM)
   {
      if (clip_t(dy, min_y - *y1, &tE, &tL) == FALSE)
         return FALSE;
   }

   // clip to top edge if crossed 
   if (xor & GRA_CLIP_TOP)
   {
      if (clip_t(-dy, *y1 - max_y, &tE, &tL) == FALSE)
         return FALSE;
   }

   // compute leaving intersection, if tL has moved
   if (tL < 1.0)
   {
      if (dx < 0)
         *x2 = *x1 + (int)(tL * (float)dx - 0.5);
      else
         *x2 = *x1 + (int)(tL * (float)dx + 0.5);

      if (dy < 0)
         *y2 = *y1 + (int)(tL * (float)dy - 0.5);
      else
         *y2 = *y1 + (int)(tL * (float)dy + 0.5);
   }
   
   // compute leaving intersection, if tE has moved
   if (tE > 0.0)
   {
      if (dx < 0)
         *x1 = *x1 + (int)(tE * (float)dx - 0.5);
      else
         *x1 = *x1 + (int)(tE * (float)dx + 0.5);

      if (dy < 0)
         *y1 = *y1 + (int)(tE * (float)dy - 0.5);
      else
         *y1 = *y1 + (int)(tE * (float)dy + 0.5);
   }

   return TRUE;      
}


static
boolean_t clip_t(int denom, int num, float *tE, float *tL)
{
   float t;

   if (denom > 0)                   // PE intersection
   {
      t = (float)num/(float)denom;  // t at intersection
      if (t > *tL)			         // PE point after leaving 
         return FALSE;
      
      if (t > *tE)                  // new PE point found
         *tE = t;
   }
   else if (denom < 0)              // PL intersection
   {
      t = (float)num/(float)denom;  // t at intersection
      if (t < *tE)                  // PL point before entering
         return FALSE;
      
      if (t < *tL)                  // new PL point found
         *tL = t;
   }
   else if (num > 0)                // line is parallel to edge and outside
      return FALSE;

   return TRUE;
}
