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


#ifndef TICK_MARK_H
#define TICK_MARK_H 1

#include "common.h"
#include "maps_d.h" 
#include "geo_tool_d.h"
#include "grdelem.h"

//
// forward delclaration
//
class ActiveMap;

class Tickmarks : public LinearGridElement
{
private:
   int m_tick_length;            // length of tickmark line in pixels
   grid_line_type_t m_tick_type; // opposite (perpendicular) to line type (LAT/LON)

static CList <CPoint*, CPoint*> m_tickmark_point_list;

public:
   // Constructor
   Tickmarks();
   
   // Destructor
   virtual ~Tickmarks();

// *****************************************
public:   

   int draw_line( ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to );
   int add_tickmark_points( ActiveMap *map, int x1, int y1, int x2, int y2 );

static int redraw(ActiveMap *map, CDC *pDC, draw_mode_t draw_mode, 
                  int color, int width, int color_bg);

static int reset_tickmark_point_list();

   // New
   int draw_major_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to );
   int draw_minor_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to );

   int set_tick_length(int tick_length);

   // Need to integrate this algorithm with that of LinearGridElement's
   int generate_ticks( ActiveMap *map, CDC *pDC, int number_of_ticks,
      int tick_length, degrees_t tick_spacing, d_geo_t& geo_start, d_geo_t& geo_end );

   // Future use ?
   virtual int generate_label( ActiveMap *map, CDC *pDC, int is_major_line,
               d_geo_t& geo_angle, degrees_t minor_spacing );

   virtual draw_mode_t set_draw_mode( draw_mode_t mode );

   int set_pen_properties(int color, int style, int width, int color_bg);
   int set_line_color( int line_color );
   int set_line_thickness(int width);
   int set_line_style(int style);

   int set_line_type(grid_line_type_t line_type);
};

#endif // TICK_MARK_H