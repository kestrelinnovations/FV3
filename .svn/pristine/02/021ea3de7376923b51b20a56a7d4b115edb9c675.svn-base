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
-  FILE NAME:    graphics.h
-  LIBRARY NAME: graphics.lib 
- -----------------------------------------------------------------*/

#ifndef GRAPHICS_H 
#define GRAPHICS_H

/*------------------------------------------------------------------
-                           Includes
- -----------------------------------------------------------------*/

#include "common.h"
#include "utils.h"

/*------------------------------------------------------------------
-                           Functions 
- -----------------------------------------------------------------*/

/* Sets color look up table to a fixed initial state, and graphics
   global variables. This routine must be called before any
   other graphics routine. */

int GRA_open_graphics(void);
void GRA_shutdown_graphics(void);

/* Draws the FalconView hilight rectangles for displaying straight-up
   behind objects that need user hilights.

	cx, cy		is the center of the rectangle
	size			is the length of each edge (square only)
	ul_x, ul_y	is the upper/left corner
	lr_x, lr_y	is the lower/right corner										*/

void GRA_draw_hilight_rect(CDC *dc, CRect border_rect);
void GRA_draw_hilight_rect(CDC *dc, int cx, int cy, int size_out);
void GRA_draw_hilight_rect(CDC *dc, int ul_x, int ul_y, int lr_x, int lr_y);


/* Draws a vertex handle in the system hilight colors... handle size 
	is derived from system settings.

	cx, cy		is the center of the handle
	point			is the center of the handle									
	solid			is the fill status of the handle								*/

void GRA_draw_handle(CDC *dc, int cx, int cy, boolean_t solid);
void GRA_draw_handle(CDC *dc, CPoint point, boolean_t solid);

void GRA_get_handle_rect(CPoint point, CRect &rect);

void GRA_draw_select_box(CDC *dc, CRect border_rect, boolean_t solid);
void GRA_draw_select_box(CDC *dc, int cx, int cy, int size_out, boolean_t solid);
void GRA_draw_select_box(CDC *dc, int ul_x, int ul_y, int lr_x, int lr_y, boolean_t solid);

int GRA_handle_size();

// Given a one of the standard color constants defined above, this function
// returns the COLORREF for that color.  Returns the COLORREF for BRIGHT_WHITE
// by default.  This is the inverse function of GRA_get_color_constant().
COLORREF GRA_get_color_ref(int standard_color_constant);

// Given a COLORREF for one of the standard color constants defined above, this
// function returns that color constant.  Returns BRIGHT_WHITE by default.
// This is the inverse function of GRA_get_color_ref().
int GRA_get_color_constant(COLORREF standard_color);

// Given a COLORREF, this function returns a color constant.
// It returns the index of the color that is closest in value.
// It uses the standard RGB distance to search for the minimum best fit.
int GRA_get_color_constant_best_fit(COLORREF standard_color);

/* 
 *  Transfers 8 BPP image to screen at the specified location. 
 */

#endif
