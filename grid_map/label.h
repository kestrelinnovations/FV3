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
-  FILE NAME:    grid_label.h
-  LIBRARY NAME: grid_map.lib
-  PROGRAMMER:   Robert Santiago
-  DATE:         June 1999
-
-  DESCRIPTION:
-      
-
-  FUNCTIONS:
-
-      
-  REVISION HISTORY: 
-
-      $Log: grid_map.h $
 * Revision 1.1  1999/06/09  09:12:30  robert
 * Initial revision
 * 
-------------------------------------------------------------------*/

#ifndef GRID_LABEL_H
#define GRID_LABEL_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/
#include "common.h"
#include "geo_tool_d.h"
#include "maps_d.h" 
#include "grid_map_d.h"
#include "ovlelem.h"

class ActiveMap;

// label class for lat-lon grid overlay
class GridLabel 
{
private:
   // This is the text label to be displayed on an grid element (line)
   CString m_label;

   // Upper left geographic coordinate of this label
   CPoint m_ul_point;

   // Location of the anchor point for this label
   int m_anchor_position; 

   // Bounding rectangle of this label
   CRect m_bound_rect;

	// rotation angle of the label
	double m_rotation_angle;


   // longitude line spacing in degrees, used to compute longitude
   // labels overlaping
   degrees_t m_longitude_grid_spacing;

   // Latitude or longitude label
   int m_label_type;

   OvlFont m_font;
   boolean_t m_redraw;

   // Label font properties
static int m_color;
static int m_back_color;
static CString m_font_name;
static int m_font_size;
static int m_font_attrib;
static int m_background;
	
   int m_anchor_pos;

public:
   // Constructor
	GridLabel();

   GridLabel(double rotation);

   // Destructor
   ~GridLabel();

// *****************************************
public:   
   // Draw the grid label. 
   int generate(ActiveMap *map, CDC *pDC, degrees_t geo_angle, int point_type, 
                     CPoint point_ul, degrees_t minor_grid_spacing);
   int draw(ActiveMap *map, CDC *pDC);

   int get_bounding_rect( CRect *rect );
   int set_foreground_color( int fg_color );
   int set_background_color( int bg_color );
static int set_properties( int color, int bg_color, CString font_name,
                          int font_size, int font_attrib, int background );
static int get_properties( int *color, int *bg_color, CString *font_name,
                          int *font_size, int *font_attrib, int *background );


static int init_bound_rect_array();
static int remove_labels(); // reset() ???

private:
   int calculate_grid_line_format(degrees_t distance, int *label_format );
   int draw_label(ActiveMap *map, CDC *pDC);
   int generate_grid_label(CDC *pDC, degrees_t geo_angle, int format);
   int calculate_label_bounds(ActiveMap *map, CDC *pDC);

   int set_and_adjust_anchor(ActiveMap *map, CDC *pDC, degrees_t geo_angle );
   
   int off_map_bounds(ActiveMap *map);
   int overlap(ActiveMap *map);
};
#endif // GRID_LABEL_H
