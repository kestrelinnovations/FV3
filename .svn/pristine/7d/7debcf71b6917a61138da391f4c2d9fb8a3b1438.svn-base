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



// clip.h

#ifndef CLIP_H
#define CLIP_H 1

#include "common.h"

// bit flags for gra_clip_point.
#define GRA_CLIP_TOP 0x01
#define GRA_CLIP_BOTTOM 0x02
#define GRA_CLIP_LEFT 0x04
#define GRA_CLIP_RIGHT 0x08

// The line defined by (x1,y1) and (x2,y2) is clipped to the clip box.  If
// the clipped line is inside the clip box then this function returns TRUE.
// Otherwise it returns FALSE.

boolean_t gra_clip_line(int *x1, int *y1, int *x2, int *y2,
   int min_x, int min_y, int max_x, int max_y);

#endif
