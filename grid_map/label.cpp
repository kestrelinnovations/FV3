// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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
-  FILE NAME:    grid_label.c
-  LIBRARY NAME: grid_map.lib
-
-  DESCRIPTION:
-      This file contains the Lat-Long grid label implementation routines.
-
-  PUBLIC FUNCTIONS:
-
-
-  PRIVATE FUNCTIONS: NONE
-
-  STATIC FUNCTIONS:
-
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
-
-  REVISION HISTORY:
-
-       $Log: grid_ovl.cpp $
 * Revision 1.1  1999/06/09  09:37:02  Robert
 * Initial revision
 * 
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/


#include "stdafx.h"

#include "label.h"
#include "map.h"
#include "err.h"
#include "param.h"


/*------------------------------------------------------------------
-                            Statics 
-------------------------------------------------------------------*/
static CArray<CRect*, CRect*> lat_label_bound_rect_array;
static CRect current_lon_label_rect;

int GridLabel::m_color;
int GridLabel::m_back_color = UTIL_COLOR_BLACK;
CString GridLabel::m_font_name;
int GridLabel::m_font_size;
int GridLabel::m_font_attrib;
int GridLabel::m_background = UTIL_BG_NONE;

static boolean_t overlap( CRect &r1, CRect &r2);



int GridLabel::get_properties( int *color, int *bg_color, CString *font_name,
                          int *font_size, int *font_attrib, int *background )
{
   *color = m_color;
   *bg_color = m_back_color;
   *font_name = m_font_name;
   *font_size = m_font_size;
   *font_attrib = m_font_attrib;
   *background = m_background;

   return SUCCESS;
}

int GridLabel::set_properties( int color, int bg_color, CString font_name,
                          int font_size, int font_attrib, int background )
{
   m_color = color;
   m_back_color = bg_color;
   m_font_name = font_name;
   m_font_size = font_size;
   m_font_attrib = font_attrib;
   m_background = background;
 
   return SUCCESS;
}

// Grid Overlay Class Implementation
int GridLabel::init_bound_rect_array()
{
   lat_label_bound_rect_array.SetSize(0);
   
   return SUCCESS;
}

// Constructors
GridLabel::GridLabel() : m_font(m_redraw), m_longitude_grid_spacing(0.0),
	m_rotation_angle(0.0)
{
   current_lon_label_rect.SetRectEmpty(); 
}

GridLabel::GridLabel(double rotation) : m_font(m_redraw), m_longitude_grid_spacing(0.0),
	m_rotation_angle(rotation)
{
   current_lon_label_rect.SetRectEmpty(); 
}

GridLabel::~GridLabel() { }

// Draw the grid label. ul is in view coordinates
int GridLabel::generate(ActiveMap *map, CDC *pDC, degrees_t geo_angle, int type, 
         CPoint ul, degrees_t minor_grid_spacing)
{
   m_label_type = type;

   if ( type == LON_POINT )
      m_longitude_grid_spacing = minor_grid_spacing;

   // For mosaic maps generate labels only on 
   // topmost and leftmost pages (index 0 for both)
   if (map->is_mosaic_map_component())
   {
      if ( m_label_type == LAT_POINT )
      {
         int column = map->component_map_column();
         // Only generate latitude labels for 1st column (left)
         if ( column != 0 )
            return SUCCESS;
      }
      else  
      {  
         int row = map->component_map_row();
         // Only generate longitude labels for 1st row (top)
         if ( row != 0 )
            return SUCCESS;
      }
   }

   m_ul_point.x = ul.x;
   m_ul_point.y = ul.y;

   int label_format;
   calculate_grid_line_format(minor_grid_spacing, &label_format);
   generate_grid_label( pDC, geo_angle, label_format);

   set_and_adjust_anchor(map, pDC, geo_angle );

   calculate_label_bounds( map, pDC );

   return SUCCESS;
}

int GridLabel::draw(ActiveMap *map, CDC *pDC)
{
   if ( !off_map_bounds(map) )
      if ( !overlap(map) )
         return draw_label( map, pDC );

   return SUCCESS;
}

int GridLabel::draw_label( ActiveMap *map, CDC *pDC )
{                
   int ul_x = m_ul_point.x;
   int ul_y = m_ul_point.y;

   CFvwUtil *futil = CFvwUtil::get_instance();
   POINT cpt[4];

   // NOTE: Here the rotation should take into account the rotation of the geoline this
   // label belongs to. That is, the tangential angle at the starting point of the line
   double rot_angle = m_rotation_angle;
   
   int font_size = m_font_size;

   // scale the test up based on zoom percent when printing
   if ( pDC->IsPrinting() )
   {
	   double adjust_percentage = (double)PRM_get_registry_int("Printing", 
		   "FontAdjustSizePercentage", 0);
	   font_size += static_cast<int>(font_size*adjust_percentage/100.0);
   }


   // Draw the foreground label 
   futil->draw_text( pDC, m_label,ul_x, ul_y, m_anchor_position, m_font_name, font_size, 
      m_font_attrib, m_background, m_color, m_back_color, rot_angle, cpt, TRUE );

   return SUCCESS;
}

int GridLabel::calculate_grid_line_format(degrees_t distance, int *label_format ) 
{
   // Distance here represents the minor grid line spacing!!
   if ( distance >= 1.0 )
      *label_format = DEGREES_FORMAT;
   else if ( distance >= MIN_TO_DEG(1.0) && distance <= MIN_TO_DEG(59.0) )
      *label_format = DEGREES_MINUTES_FORMAT;
   else if ( distance >= SEC_TO_DEG(1.0) && distance <= SEC_TO_DEG(59.0) )
      *label_format = DEGREES_MINUTES_SECONDS_FORMAT;
   else
      *label_format = DEGREES_MINUTES_SECONDS_10THS_FORMAT;

   return SUCCESS;
}

int GridLabel::generate_grid_label( CDC *pDC, degrees_t geo_angle, int format )
{
   const int LABEL_LEN = 20;
   char label[LABEL_LEN];
   char dir_char;
   int degrees;
   int minutes;
   int seconds;
   int tens_of_second;

   if (m_label_type == LAT_POINT)
   {
      if (geo_angle < 0.0)
      {
         geo_angle = -geo_angle;
         dir_char = 'S';
      }
      else
         dir_char = 'N';
   }
   else if (m_label_type == LON_POINT)
   {
      if (geo_angle < 0.0)
      {
         geo_angle = -geo_angle;
         dir_char = 'W';
      }
      else
         dir_char = 'E';
   }
   else
   {
      ERR_report("Invalid point_type.");
      return FAILURE;
   }

   switch(format)
   { 
      case DEGREES_FORMAT:
         degrees = (int)(geo_angle + 0.5);
         if (m_label_type == LAT_POINT)
            sprintf_s(label, LABEL_LEN, "%c %02d\260", dir_char, degrees);
         else
            sprintf_s(label, LABEL_LEN, "%c %03d\260", dir_char, degrees);
         break;

      case DEGREES_MINUTES_FORMAT:
         degrees = (int)geo_angle;
         minutes = (int)((geo_angle - (degrees_t)degrees) * 60.0 + 0.5);
         if (minutes == 60)
         {
            minutes = 0;
            degrees++;
         }
         if (m_label_type == LAT_POINT)
            sprintf_s(label, LABEL_LEN, "%c %02d\260 %02d\'", dir_char, degrees, minutes);
         else
            sprintf_s(label, LABEL_LEN, "%c %03d\260 %02d\'", dir_char, degrees, minutes);
         break;

      case DEGREES_MINUTES_SECONDS_FORMAT:
         degrees = (int)geo_angle;
         minutes = (int)((geo_angle - (degrees_t)degrees) * 60.0);
         seconds = (int)(((geo_angle - (degrees_t)degrees) * 60.0 -
            (degrees_t)minutes) * 60.0 + 0.5);
         if (seconds == 60)
         {
            seconds = 0;
            minutes++;
            if (minutes == 60)
            {
               minutes = 0;
               degrees++;
            }
         }
         if (m_label_type == LAT_POINT)
            sprintf_s(label, LABEL_LEN, "%c %02d\260 %02d\' %02d\"", dir_char, degrees, minutes,
               seconds);
         else
            sprintf_s(label, LABEL_LEN, "%c %03d\260 %02d\' %02d\"", dir_char, degrees, minutes,
               seconds);
         break;

      case DEGREES_MINUTES_SECONDS_10THS_FORMAT:
         degrees = (int)geo_angle;
         minutes = (int)((geo_angle - (degrees_t)degrees) * 60.0);
         seconds = (int)(((geo_angle - (degrees_t)degrees) * 60.0 -
            (degrees_t)minutes) * 60.0);
         tens_of_second = (int)((((geo_angle - (degrees_t)degrees) * 60.0 -
            (degrees_t)minutes) * 60.0 - (degrees_t)seconds) * 10.0 + 0.5);
         if (tens_of_second == 10)
         {
            tens_of_second = 0;
            seconds++;
            if (seconds == 60)
            {
               seconds = 0;
               minutes++;
               if (minutes == 60)
               {
                  minutes = 0;
                  degrees++;
               }
            }
         }
         if (m_label_type == LAT_POINT)
            sprintf_s(label, LABEL_LEN, "%c %02d\260 %02d\' %02d.%d\"",
               dir_char, degrees, minutes, seconds, tens_of_second);
         else
            sprintf_s(label, LABEL_LEN, "%c %03d\260 %02d\' %02d.%d\"",
               dir_char, degrees, minutes, seconds, tens_of_second);
         break;

      default:
         ERR_report("Invalid Lat-Long format.");
         return FAILURE;
   }

   m_label = label;
   return SUCCESS;
}


int GridLabel::set_and_adjust_anchor(ActiveMap *map, CDC *pDC, degrees_t geo_angle )
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   double rot_angle = m_rotation_angle;

   int  label_width, label_height;
   futil->get_text_size(pDC, m_label, m_font_name, m_font_size, 0, 
                     &label_width, &label_height, TRUE, rot_angle );

   int screen_height, screen_width;
   map->get_surface_size(&screen_width, &screen_height);

   int margin_offset = 3;
   // For latitudes
   if ( m_label_type == LAT_POINT ) 
   {
      if ( geo_angle >= 0.0 )
         m_anchor_position = UTIL_ANCHOR_UPPER_LEFT;
      else
         m_anchor_position = UTIL_ANCHOR_LOWER_LEFT;

      if ( rot_angle == 0.0 ) 
      {
         return SUCCESS;

      } else if ( rot_angle > 0.0 && rot_angle < 90.0 ) {
         // left screen edge
         if ( m_ul_point.x == 0 ) 
         {
            if ( geo_angle >= 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = (double)label_height*tan(rad_angle);
               dx = (int)( (d*cos(rad_angle) ) + 0.5 );
               dy = (int)( (d*sin(rad_angle) ) + 0.5 );

               m_ul_point.x += (dx + margin_offset);
               m_ul_point.y += (dy + margin_offset);
            }
         }
         else // top screen edge
         {
            if ( geo_angle < 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = (double)label_height/tan(rad_angle);
               dx = (int)( (d*cos(rad_angle) ) + 0.5 );
               dy = (int)( (d*sin(rad_angle) ) + 0.5 );

               m_ul_point.x += (dx + margin_offset);
               m_ul_point.y += (dy + margin_offset);
            }
         }
      } 
      else if ( rot_angle == 90.0 ) 
      {
         return SUCCESS;

      } 
      else if ( rot_angle > 90.0 && rot_angle < 180.0 ) 
      {
         // top screen edge
         if ( m_ul_point.y == 0 ) 
         {
            if ( geo_angle >= 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_height/tan(rad_angle));
               dx = (int)( (d*cos(rad_angle) ) + 0.5 );
               dy = (int)( (d*sin(rad_angle) ) + 0.5 );

               m_ul_point.x += (dx - margin_offset);
               m_ul_point.y += (dy + margin_offset);
            }
         } 
         else // right screen edge
         {
            if ( geo_angle < 0.0 )
            {
               // This is a quick fix
               m_ul_point.x -= 3;

               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_height*tan(rad_angle));
               dx = (int)( (d*cos(rad_angle) ) + 0.5 );
               dy = (int)( (d*sin(rad_angle) ) + 0.5 );

               m_ul_point.x += (dx - margin_offset);
               m_ul_point.y += (dy + margin_offset);
            }
         }
      }  else if ( rot_angle == 180.0 ) 
      {
         return SUCCESS;
      
      } else if ( rot_angle > 180.0 && rot_angle < 270.0 ) {
         // right screen edge
         if ( m_ul_point.x == screen_width ) 
         {
            if ( geo_angle >= 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_height*tan(rad_angle));
               dx = (int)( (d*cos(rad_angle) ) + 0.5 );
               dy = (int)( (d*sin(rad_angle) ) + 0.5 );

               m_ul_point.x += (dx - margin_offset);
               m_ul_point.y += (dy - margin_offset);
            }
         } 
         else  // bottom
         {
            if ( geo_angle < 0.0 )
            {
               m_ul_point.y -= 3;

               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_height/tan(rad_angle));
               dx = (int)( (d*cos(rad_angle) ) + 0.5 );
               dy = (int)( (d*sin(rad_angle) ) + 0.5 );

               m_ul_point.x += (dx - margin_offset);
               m_ul_point.y += (dy - margin_offset);
            }
         }
      } else if ( rot_angle == 270.0 ) 
      {
         return SUCCESS;

      }  else if ( rot_angle > 270.0 && rot_angle < 360.0 ) 
      {
         // bottom screen edge
         if ( m_ul_point.y == screen_height ) 
         {
            if ( geo_angle >= 0.0 )
            {
               m_ul_point.y -= 3;
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_height/tan(rad_angle));
               dx = (int)( (d*cos(rad_angle) ) + 0.5 );
               dy = (int)( (d*sin(rad_angle) ) + 0.5 );

               m_ul_point.x += (dx + margin_offset);
               m_ul_point.y += (dy - margin_offset);
            }
         } 
         else  // left
         {
            if ( geo_angle < 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_height*tan(rad_angle));
               dx = (int)( (d*cos(rad_angle) ) + 0.5 );
               dy = (int)( (d*sin(rad_angle) ) + 0.5 );

               m_ul_point.x += (dx + margin_offset);
               m_ul_point.y += (dy - margin_offset);
            }
         }
      }
      else 
      {
         ERR_report("Rotation angle out of bounds in "
            "GridLabel::set_and_adjust_anchor().");
         return FAILURE;
      }

   } 
   else if ( m_label_type == LON_POINT )
   {      // For Longitudes
      // Establish the anchor point first
	   if (abs(m_ul_point.y - screen_height) < abs(m_ul_point.y))
	   {
		  if ( geo_angle >= 0.0 )
			 m_anchor_position = UTIL_ANCHOR_LOWER_RIGHT;
		  else
			 m_anchor_position = UTIL_ANCHOR_LOWER_LEFT;

		  // kludge
		  if (map->actual_rotation() == 0.0)
			m_rotation_angle = 0.0;
	   }
	   else
	   {
		  if ( geo_angle >= 0.0 )
			 m_anchor_position = UTIL_ANCHOR_UPPER_RIGHT;
		  else
			 m_anchor_position = UTIL_ANCHOR_UPPER_LEFT;
	   }

      // now adjust (translate the point) 
      if ( rot_angle == 0.0 ) 
      {
         return SUCCESS;
      } 
      else if ( rot_angle > 0.0 && rot_angle < 90.0 ) 
      {
         // top screen edge
         if ( m_ul_point.y == 0 ) 
         {
            if ( geo_angle >= 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_width*tan(rad_angle));
               dx = (int)( (d*sin(rad_angle) ) + 0.5 );
               dy = (int)( (d*cos(rad_angle) ) + 0.5 );

               m_ul_point.x -= dx;
               m_ul_point.y += dy;
            }
         } 
         else 
         {
            if ( geo_angle < 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_width/tan(rad_angle));
               dx = (int)( (d*sin(rad_angle) ) + 0.5 );
               dy = (int)( (d*cos(rad_angle) ) + 0.5 );

               m_ul_point.x -= dx;
               m_ul_point.y += dy;
            }
         }
      } 
      else if ( rot_angle == 90.0 ) 
      {
         return SUCCESS;
      
      } 
      else if ( rot_angle > 90.0 && rot_angle < 180.0 ) 
      {
         // right screen edge
         if ( m_ul_point.x == screen_width ) 
         {
            if ( geo_angle >= 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_width/tan(rad_angle));
               dx = (int)( (d*sin(rad_angle) ) + 0.5 );
               dy = (int)( (d*cos(rad_angle) ) + 0.5 );

               m_ul_point.x -= dx;
               m_ul_point.y += dy;
            }
         } 
         else 
         {  // bottom screen edge
            if ( geo_angle < 0.0 )
            {
               m_ul_point.y -= 3;

               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_width*tan(rad_angle));
               dx = (int)( (d*sin(rad_angle) ) + 0.5 );
               dy = (int)( (d*cos(rad_angle) ) + 0.5 );

               m_ul_point.x -= dx;
               m_ul_point.y += dy;
            }
         }
      } else if ( rot_angle == 180.0 ) {
         return SUCCESS;

      } 
      else if ( rot_angle > 180.0 && rot_angle < 270.0 ) 
      {
         // bottom screen edge
         if ( m_ul_point.y == screen_height ) 
         {
            if ( geo_angle >= 0.0 )
            {
               m_ul_point.y -= 3;

               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_width*tan(rad_angle));
               dx = (int)( (d*sin(rad_angle) ) + 0.5 );
               dy = (int)( (d*cos(rad_angle) ) + 0.5 );

               m_ul_point.x -= dx;
               m_ul_point.y += dy;
            }
         } 
         else 
         {     // left screen edge
            if ( geo_angle < 0.0 )
            {
               m_ul_point.x += 3;

               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_width/tan(rad_angle));
               dx = (int)( (d*sin(rad_angle) ) + 0.5 );
               dy = (int)( (d*cos(rad_angle) ) + 0.5 );

               m_ul_point.x -= dx;
               m_ul_point.y += dy;
            }
         }
      } 
      else if ( rot_angle == 270.0 ) 
      {
         return SUCCESS;

      } 
      else if ( rot_angle > 270.0 && rot_angle < 360.0 ) 
      {
         // left screen edge
         if ( m_ul_point.x == 0 ) 
         {
            if ( geo_angle >= 0.0 )
            {
               m_ul_point.x += 3;

               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_width/tan(rad_angle));
               dx = (int)( (d*sin(rad_angle) ) + 0.5 );
               dy = (int)( (d*cos(rad_angle) ) + 0.5 );

               m_ul_point.x -= dx;
               m_ul_point.y += dy;
            }
         } 
         else 
         {  // top screen edge
            if ( geo_angle < 0.0 )
            {
               int dx = 0, dy = 0;
               double rad_angle = DEG_TO_RAD(rot_angle);
               double d = fabs((double)label_width*tan(rad_angle));
               dx = (int)( (d*sin(rad_angle) ) + 0.5 );
               dy = (int)( (d*cos(rad_angle) ) + 0.5 );

               m_ul_point.x -= dx;
               m_ul_point.y += dy;
            }
         }
      } 
      else 
      {
         ERR_report("Rotation angle out of bounds in "
            "GridLabel::set_and_adjust_anchor().");
         return FAILURE;
      }
   } 
   else 
   {
     ERR_report("Invalid grid label type in"
            "GridLabel::set_and_adjust_anchor().");
         return FAILURE;
   }

   return SUCCESS;
}

int GridLabel::calculate_label_bounds( ActiveMap *map, CDC *pDC )
{
   double rot_angle = map->actual_rotation();
   CFvwUtil *futil = CFvwUtil::get_instance();
   int  label_width, label_height;

   if ( m_label.IsEmpty() ) {
      ERR_report("Empty grid line label in calculate_label_bounds().");
      return FAILURE;
   }

   futil->get_text_size(pDC, m_label, m_font_name, m_font_size,0, 
                     &label_width, &label_height, TRUE, rot_angle );

   CPoint label_bound_points[4];
   futil->compute_text_poly( m_ul_point.x, m_ul_point.y, m_anchor_position,
         label_width, label_height, rot_angle, label_bound_points );

   // Calcuate the bounding rectangle 
   if ( rot_angle == 0.0 )
   {
      // Create the bound rectangle for a non-rotated label.
      m_bound_rect.left = label_bound_points[0].x;
      m_bound_rect.right = label_bound_points[1].x;
      m_bound_rect.top = label_bound_points[0].y;
      m_bound_rect.bottom = label_bound_points[2].y;
   } 
   else 
   {
      // This creates a bound rectangle for a rotated label.
      int t,b,l,r;
      t = label_bound_points[0].y;
      b = label_bound_points[0].y;
      l = label_bound_points[0].x;
      r = label_bound_points[0].x;

      for ( int i = 1; i < 4; i++ )
      {
         if ( t > label_bound_points[i].y )
            t = label_bound_points[i].y;
         if ( b < label_bound_points[i].y )
            b = label_bound_points[i].y;
         if ( l > label_bound_points[i].x )
            l = label_bound_points[i].x;
         if ( r < label_bound_points[i].x )
            r = label_bound_points[i].x;
      }

      m_bound_rect.left = l;
      m_bound_rect.right = r;
      m_bound_rect.top = t;
      m_bound_rect.bottom = b;
   } // End if (rotation == 0.0)

   if ( m_label_type == LAT_POINT )
   {
      CRect *p_bound_rect = NULL;
      p_bound_rect = new CRect(m_bound_rect);
      if ( !p_bound_rect )
      {
         ERR_report("GridLabel::calculate_label_bounds() failed (CRect *) memory alloc.");
         return FAILURE;
      }

      lat_label_bound_rect_array.Add( p_bound_rect );
   }

   return SUCCESS;
}

// Return a copy of the bounding rectangle for "this" label
int GridLabel::get_bounding_rect( CRect *rect )
{
   ASSERT( rect );
   
   *rect = m_bound_rect;

   return SUCCESS;
}

// Check if the bounds of the label exceeds the bounds of the 
// screen view rectangle. 
int GridLabel::off_map_bounds(ActiveMap *map)
{
   int screen_width, screen_height;
   map->get_surface_size(&screen_width, &screen_height);

   if ( m_bound_rect.left < 0 || m_bound_rect.right > screen_width ||
        m_bound_rect.top < 0 || m_bound_rect.bottom > screen_height  ) 
      return TRUE;

   return FALSE;
}

// Check if this label (longitude label) intersects (or overlap)
// anyone of the latitude labels whose bound rectangles are stored 
// in the CArray "label_array"

int GridLabel::overlap(ActiveMap *map)
{
   // This test shall currently be done to longitude labels only
   // Latitude labels are drawn first
   if ( m_label_type == LAT_POINT )
      return FALSE;

   // First check if the bounds of this long label overlap with 
   // each of the lat bounds stored in the array
   int label_count = lat_label_bound_rect_array.GetSize();
   if(label_count > 0)
   {
      // Store here each of the lat label bounds to be tested 
      // against this label's bound rect.
      CRect *pRect = NULL;

      // Use this rectangle to store and check if the lat and lon label
      // rectangles overlap (or intersect)
      CRect overlap_rect;

      // Check for overlapping each of the lat label bounding rects
      for(int i = 0; i < label_count; i++)
      {
         pRect = lat_label_bound_rect_array[i];
         ASSERT(pRect);
         if (overlap_rect.IntersectRect( &m_bound_rect, pRect) )
            return TRUE;
      }
   }
/*

   // Second, check if the individual longitude labels overlap with 
   // each other (a problem a higher latitudes and large scales)

   // get the long label width in pixels
   int label_width = m_bound_rect.Width();
   
   // get th edegrees per pixel in latitude to compute the distance 
   // between longitudes
   degrees_t deg_per_pixel_lat, deg_per_pixel_lon;
   map->get_vmap_degrees_per_pixel(&deg_per_pixel_lat, &deg_per_pixel_lon);

   ASSERT( deg_per_pixel_lat > 0.0 );
   // need to compute the minor gridline spacing
   
   int longitude_spacing_in_pixels;
   longitude_spacing_in_pixels = (int)((m_longitude_grid_spacing / deg_per_pixel_lat)+0.5);

   TRACE( "longitude_spacing_in_pixels = %d\n", longitude_spacing_in_pixels );

   int x_ul, y_ul, x_ur, y_ur;
   CPoint ul_point = m_bound_rect.TopLeft();

   x_ul = ul_point.x;
   y_ul = ul_point.y;
   x_ur = x _ul + m_bound_rect.Width();
   y_ur = y_ul;
*/

   // Need to know about the previous lon label rectangle to test
   // for overlapping


   return FALSE;
}

// test to see if the two rectangles, r1 and r2, intersect
static boolean_t overlap( CRect &r1, CRect &r2)
{
   CRect overlap_rect;

   if (overlap_rect.IntersectRect( &r1, &r2) )
      return TRUE;

    return FALSE;
}   

// Public member so clients of this class (and controlling 
// the creation of labels) can delete the currently stored
// bounding rectangles (of latitudes)

// Change the name to remove lat_rectangles
int GridLabel::remove_labels()
{
   int label_count = lat_label_bound_rect_array.GetSize();
   if(label_count != 0)
   {
      CRect *pRect = NULL;
      for(int i = 0; i < label_count; i++)
      {
         pRect = lat_label_bound_rect_array[i]; 
         ASSERT(pRect);
         delete pRect;
      }
      lat_label_bound_rect_array.RemoveAll();
   }
   return SUCCESS;
}

int GridLabel::set_foreground_color( int fg_color )
{
   m_color = fg_color;

   return SUCCESS;
}

int GridLabel::set_background_color( int bg_color )
{
   m_back_color = bg_color;

   return SUCCESS;
}
