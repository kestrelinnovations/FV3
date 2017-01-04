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


#ifndef LINEAR_GRID_ELEMENT_H
#define LINEAR_GRID_ELEMENT_H 1

#include "common.h"
#include "geo_tool_d.h"
#include "maps_d.h" 
#include "grid_map_d.h"
#include "ovlelem.h"



class ActiveMap;

// Linear grid base class for lat-lon grid overlay
class LinearGridElement
{
protected:
   // All linear grid elements share this attributes
   int m_color;      // line foreground color
   int m_color_bg;   // line background color
   int m_style;      // line style (solid)
   int m_width;      // line width 

   draw_mode_t m_draw_mode; // this line's drawing mode

   grid_line_type_t m_line_type;  // latitude or longitude line

   int m_show_minor_lines; // draw minor grid lines in addition to major grid lines

 //  draw_mode_t m_draw_mode; // drawing background only, foreground only, or both

public:
   // Constructor
   LinearGridElement();
   
   // Destructor
   virtual ~LinearGridElement();

// *****************************************
public:   
   virtual int set_pen_properties(int color, int style, int width, int color_bg);
   virtual int set_line_style(int style);
   virtual int set_line_color( int color );
   virtual int set_line_background_color( int color_bg );
   virtual int set_line_thickness(int width);

   virtual int set_line_type(grid_line_type_t line_type);
   int set_minor_line_show_flag( int show_minor_lines );
  
   virtual draw_mode_t set_draw_mode( draw_mode_t mode );      

//   int draw(ActiveMap *map, CDC *pDC, int number_of_lines,
//            int is_major_line, degrees_t spacing, d_geo_t& map_ll, d_geo_t& map_ur );
 
   // Newest 
   int draw(ActiveMap *map, CDC *pDC, int number_of_lines, 
            degrees_t major_spacing, degrees_t minor_spacing, d_geo_t& map_ll, d_geo_t& map_ur );


protected:   
   virtual int draw_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to ) = 0;

   // Newest
   virtual int draw_major_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to ) = 0;
   virtual int draw_minor_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to ) = 0;


   int is_angle_inside_extents( d_geo_t& start_angle, d_geo_t& end_angle, 
                                    d_geo_t& current_angle );
   int next_nice_angle(degrees_t spacing, d_geo_t& input_geo_point, d_geo_t *rounded_geo_point );
   int round_up( degrees_t spacing, degrees_t input_angle, degrees_t *rounded_angle );
   int increment_geo_angle( degrees_t spacing, d_geo_t *geo_point );
   int set_line_fixed_extents( d_geo_t& map_ll, d_geo_t& map_ur, 
            d_geo_t* geo_from, d_geo_t* geo_to );
   
   int set_line_points( d_geo_t& grid_point, d_geo_t* geo_from, d_geo_t* geo_to );

   virtual int generate_label( ActiveMap *map, CDC *pDC, int is_major_line,  
                  d_geo_t& geo_angle, degrees_t minor_spacing ) = 0;
   
   boolean_t is_major(degrees_t spacing, d_geo_t grid_point);

};

#endif // LINEAR_GRID_ELEMENT_H