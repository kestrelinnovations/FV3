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



/*------------------------------------------------------------------
-  FILE NAME:    clt.h
-
-  DESCRIPTION:
-
-      Color lookup table functions and defintions.
-      
-------------------------------------------------------------------*/

#ifndef COLORS_H
#define COLORS_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"

/*------------------------------------------------------------------
-                           Definitions
-------------------------------------------------------------------*/

//
// The FalconView logical palette is organized as follows:
//
// The palette contains 256 entries.
//
// The first 10 colors in the palette are the same as the first 10 system colors in
// hardware palette.  The last 10 colors in the palette are the same as the last 
// 10 system colors in hardware palette.  Thus, the FalconView palette mirrors 
// the system palette in this regard.
//
// FalconView reserves a few (currently 4) colors that are not standard Windows
// colors.  These are stored immediately below the upper ten system colors.
// They are used, for example, for the coverage rectangle overlay.
//
// FalconView uses the colors in the range IMAGE_CLT_START_INDEX to IMAGE_CLT_END_INDEX
// (i.e. 10 to 225 - 216 colors total) as the non-reserved, changeable colors
// available for raster maps.  FalconView uses the PAL_FLAG (PC_NOCOLLAPSE) flag
// for these entries because it seems to cause less disruption during dim/bright (less
// colors shift on the screen when the dimmed/brightened palette is realized, 
// thus the window doesn't change drastically before the new dimmed/brightened map 
// is displayed).
//
// The colors in the range from 217 to LOW_NON_STANDARD_COLOR (242) are not
// currently settable in FalconView, but they (like all of the non-system colors
// in the palette) must be different than all of the system colors for the reasons
// described below.  They currently get set to black (which then gets changed
// by the uniqueness function to deconflict it from the system color black).
//
// ALL non-system colors in the palette (i.e. everything but the first ten and 
// the last ten colors) must be different from the system colors in the palette (i.e.
// the first ten and the last ten colors).  This is required for drawing a map into
// an offscreen map (as needed, for example, for "smooth scrolling"/animation mode).
// This was needed because when drawing device-dependent bitmaps to a dib section
// in backing store (whose palette matched FalconView's logical palette), colors 
// would be incorrect.  For example, when drawing icons or when doing a highlight
// rectangle for an icon - which requires transparency, the base map colors that 
// should not have been affected by the transparency would get messed up on certain 
// map types.  This was because the "changeable" colors in FalconView could contain 
// duplicates of system colors (e.g. white or black).  
// So when you drew, for example, an icon, if it
// was looking for black for transparency, it might use index 11 (if that happened
// to contain an RGB value of (0, 0, 0)), rather than index 0. XOR operations only
// work correctly for the first and last ten colors in the palette.  The rest of the palette
// is not organized this way, so the colors would get messed up when it would
// use these non-system-color indices for the device-dependent bitmaps.  Therefore all 
// of the non-reserved colors in the logical palette (and hence the dib section in
// offscreen map/smooth scrolling mode) need to be different from all of the system 
// colors to prevent this from happening.
//

/*------------------------------------------------------------------
-                       Function Prototypes 
-------------------------------------------------------------------*/


/* This function is used to set up the color lookup table in its initial
   state. The initial state is a predefined state which is a function of
   the number of colors available in the current graphics mode (16 or 256). */

int gra_open_clt(void);


#endif
