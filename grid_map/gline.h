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
-  FILE NAME:    grid_line.h
-  LIBRARY NAME: grid_map.lib
-  PROGRAMMER:   Robert Santiago
-  DATE:         June 1999
-
-  DESCRIPTION:
-
-  FUNCTIONS:
-
-  REVISION HISTORY: 
-
-      $Log: grid_line.h $
 * Revision 1.1  1999/06/09  015:48:05  robert
 * Initial revision
 *
-------------------------------------------------------------------*/

#ifndef GRID_LINE_H
#define GRID_LINE_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"
#include "geo_tool_d.h"
#include "maps_d.h" 
#include "grid_map_d.h"
#include "label.h"
#include "grdelem.h"
#include "tickmark.h"


class ActiveMap;

// Line class for lat-lon grid overlay
class GridLine : public LinearGridElement
{
   // GridLine Attributes
protected:
   GeoLine m_geo_grid_line;
   Tickmarks m_geo_tickmarks;
   
static CList <GridLabel*, GridLabel*> m_gridlabel_list;
static CList <GeoLine*, GeoLine*> m_geo_line_list;

   int m_show_labels_on_major_lines;
   int m_show_tickmarks;

	// starting point of grid line in the device window
	CPoint m_start_point;

public:
   GridLine();
   ~GridLine();

public:   
   // Draw the grid line.
   int draw_line(ActiveMap *map, CDC *pDC, d_geo_t& start_angle, d_geo_t& end_angle );
   
static int redraw(ActiveMap *map, CDC *pDC, draw_mode_t draw_mode);

static int reset_label_list();
static int draw_labels(ActiveMap *map, CDC *pDC);

static int reset_geo_line_list();

   // New
   int draw_major_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to );
   int draw_minor_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to );

   int add_geo_line( ActiveMap *map, const d_geo_t &start, const d_geo_t &end, 
		boolean_t ignore_second = FALSE  );

   int generate_label( ActiveMap *map, CDC *pDC, int is_major_line, 
                  d_geo_t& geo_angle, degrees_t minor_spacing );

   int set_labels_on_major_gridline_show_flag( int show_labels_on_major_lines );
   int set_show_ticks( int show_ticks );
   int set_tick_length( int tick_length );


   int get_start_point( ActiveMap *map, CDC *pDC, CPoint *start_point);

   virtual draw_mode_t set_draw_mode( draw_mode_t mode );

   virtual int set_pen_properties(int color, int style, int width, int color_bg);
   virtual int set_line_style(int style);
   virtual int set_line_color( int line_color );
   virtual int set_line_thickness(int width);

   virtual int set_line_type(grid_line_type_t line_type);
};



#endif // GRID_LINE_H
