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

#include "grdelem.h"
#include "map.h"
#include "param.h"

LinearGridElement::LinearGridElement() 
{
   m_show_minor_lines = 
         PRM_get_registry_int( "Lat-Long Grid Options", "Show Minor GridLines", 1 );
   
   m_draw_mode = NORMAL_MODE;
}

LinearGridElement::~LinearGridElement() { }

draw_mode_t LinearGridElement::set_draw_mode( draw_mode_t mode )
{
   draw_mode_t old_mode = m_draw_mode;
   m_draw_mode = mode;

   return old_mode;
}

// This is the common algorithm to generate grid based lines 
int LinearGridElement::draw(ActiveMap *map, CDC *pDC, int number_of_lines, 
    degrees_t major_spacing, degrees_t minor_spacing, d_geo_t& map_ll, d_geo_t& map_ur )
{
   int k;
   d_geo_t grid_point, geo_from, geo_to;

   // map_ll and map_ur are in current datum

   // compute the next lat/long that is a multiple of the spacing
   next_nice_angle(minor_spacing, map_ll, &grid_point );

   // the grid lines spans from the map extents
   set_line_fixed_extents( map_ll, map_ur, &geo_from, &geo_to );

   for ( k = 0; k < number_of_lines; k++ ) 
   {
      // Don't generate lines if outside the map extents
      if (is_angle_inside_extents( map_ll, map_ur, grid_point ) )
      {
         // set the geo points for this line
         set_line_points( grid_point, &geo_from, &geo_to );

         // A major line has other attributes (like tickmarks)
         if (map->scale() >= ONE_TO_100K || is_major(major_spacing, grid_point))
         {
            draw_major_line(map, pDC, geo_from, geo_to );

            // generate the label for this line
            generate_label( map, pDC, TRUE, geo_from, major_spacing );

         }
         // draw minor grid lines if its show flag is set
         else if ( m_show_minor_lines == TRUE )
         {
            draw_minor_line(map, pDC, geo_from, geo_to );

            // generate the label for this line
            generate_label( map, pDC, FALSE, geo_from, minor_spacing );
         }
         
         // compute the next grid line position
         increment_geo_angle( minor_spacing, &grid_point );
      } else 
         break;
   }

   return SUCCESS;
}

// compute the next gridline Lat/Long angle;  
int LinearGridElement::increment_geo_angle( degrees_t spacing, d_geo_t *geo_point )
{
   if ( m_line_type == LAT_POINT )
   {
      geo_point->lat += spacing;   
   } else {
      degrees_t geo_lon = geo_point->lon;
      geo_lon += spacing;
      if (geo_lon > MAX_LON_DEG)
         geo_lon -= WORLD_DEG;
      geo_point->lon = geo_lon;
  }

   return SUCCESS;
}

int LinearGridElement::set_line_fixed_extents( d_geo_t& map_ll, d_geo_t& map_ur, 
            d_geo_t* geo_from, d_geo_t* geo_to )
{
   if ( m_line_type == LAT_POINT )
   {
      geo_from->lon = map_ll.lon;
      geo_to->lon = map_ur.lon;
   }
   else
   {
      geo_from->lat = map_ll.lat;  
      geo_to->lat = map_ur.lat;    
   }

   return SUCCESS;
}

int LinearGridElement::set_line_points( d_geo_t& grid_point, d_geo_t* geo_from, d_geo_t* geo_to )
{
   if ( m_line_type == LAT_POINT )
   {
      geo_from->lat = grid_point.lat;  
      geo_to->lat = grid_point.lat;    
   }
   else
   {
      geo_from->lon = grid_point.lon;
      geo_to->lon = grid_point.lon;
   }

   return SUCCESS;
}

int LinearGridElement::is_angle_inside_extents( d_geo_t& start_angle, d_geo_t& end_angle, 
                                    d_geo_t& current_angle )
{
   if ( m_line_type == LAT_POINT )
   {
      if ( current_angle.lat < end_angle.lat )
         return TRUE;
   } else {
      if (GEO_lon_in_range(start_angle.lon, end_angle.lon, current_angle.lon) == TRUE)
         return TRUE;
   }

   return FALSE;
}

// Rounds up to the nearest factor of "spacing"
int LinearGridElement::round_up( degrees_t spacing, degrees_t input_angle, degrees_t *rounded_angle )
{
   ASSERT( spacing != 0.0 );
   degrees_t angle = input_angle;
   degrees_t nice_num = 0.0;
   if ( angle >= 0.0 )
      nice_num = (degrees_t)((int)(angle/spacing) + 1) * spacing;
   else
      nice_num = (degrees_t)((int)(angle/spacing)) * spacing;

   *rounded_angle = nice_num;
   return SUCCESS;
}


int LinearGridElement::next_nice_angle(degrees_t spacing, d_geo_t& input_geo_point, d_geo_t *rounded_geo_point )
{
   if ( m_line_type == LAT_POINT )
   {
      round_up( spacing, input_geo_point.lat, &rounded_geo_point->lat );
   }
   else
   {
      round_up( spacing, input_geo_point.lon, &rounded_geo_point->lon );
      if (rounded_geo_point->lon > HALF_WORLD_DEG)
         rounded_geo_point->lon -= WORLD_DEG;
      if (rounded_geo_point->lon < -HALF_WORLD_DEG)
         rounded_geo_point->lon += WORLD_DEG;

   }

   return SUCCESS; 
}

boolean_t LinearGridElement::is_major(degrees_t spacing, d_geo_t grid_point)
{
   degrees_t geo_point = 0.0;
   degrees_t EPSILON = 0.000001;
   
   if ( m_line_type == LAT_POINT )
      geo_point = fabs(grid_point.lat);
   else
      geo_point = fabs(grid_point.lon);

   degrees_t dmod = fmod( geo_point, spacing );

   if (  dmod <= EPSILON )
      return TRUE;
   
   return FALSE;
}

int LinearGridElement::set_minor_line_show_flag( int show_minor_lines )
{
   m_show_minor_lines = show_minor_lines;
   
   return SUCCESS; 
}


int LinearGridElement::set_line_type(grid_line_type_t type)
{
   m_line_type = type;

   return SUCCESS;
}

int LinearGridElement::set_pen_properties(int color, int style, int width, int color_bg)
{
   m_color = color;
   m_style = style;
   m_width = width;
   m_color_bg = color_bg;

   return SUCCESS; 
}


int LinearGridElement::set_line_color( int line_color )
{
   m_color = line_color;

   return SUCCESS; 
}

int LinearGridElement::set_line_background_color( int color_bg )
{
   m_color_bg = color_bg;

   return SUCCESS; 
}


int LinearGridElement::set_line_style( int line_style )
{
   m_style = line_style;

   return SUCCESS; 
}

int LinearGridElement::set_line_thickness( int thickness )
{
   m_width = thickness;

   return SUCCESS; 
}
