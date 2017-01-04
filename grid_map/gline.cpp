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
-  FILE NAME:    grid_line.c
-  LIBRARY NAME: grid_map.lib
-
-  DESCRIPTION:
-      This file contains the Lat-Long grid line implementation routines.
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
#include "map.h"
#include "err.h"
#include "param.h"
#include "gline.h"


CList <GridLabel*, GridLabel*> GridLine::m_gridlabel_list;
CList <GeoLine *, GeoLine *> GridLine::m_geo_line_list;
// 
GridLine::GridLine()
{
   m_geo_grid_line.set_heading_type( GeoSegment::RHUMB_LINE );

   // initalize from registry 
   m_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Color", UTIL_COLOR_WHITE );
   m_style = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Style", UTIL_LINE_SOLID );
   m_width = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Width", 1 );
   m_color_bg = PRM_get_registry_int( "Lat-Long Grid Options", 
               "Grid Line Background Color", UTIL_COLOR_BLACK );

   m_show_tickmarks = PRM_get_registry_int( "Lat-Long Grid Options", "Show Tickmarks", 1 );

   m_show_labels_on_major_lines = PRM_get_registry_int( "Lat-Long Grid Options", 
                  "Show Labels on Major Gridlines", 1 );

   set_pen_properties(m_color, m_style, m_width, m_color_bg);
}

GridLine::~GridLine() { }

draw_mode_t GridLine::set_draw_mode( draw_mode_t mode )
{
   LinearGridElement::set_draw_mode( mode );
   
   m_geo_tickmarks.set_draw_mode(mode);

   draw_mode_t old_mode = m_geo_grid_line.set_draw_mode(mode);
   return old_mode;
}


int GridLine::set_pen_properties(int color, int style, int width, int color_bg)
{
   LinearGridElement::set_pen_properties(color, style, width, color_bg);
   m_geo_tickmarks.set_pen_properties(color, UTIL_LINE_TMARK, width, color_bg);

   OvlPen &pen = m_geo_grid_line.get_pen();
   pen.set_foreground_pen(color, style, width);
   pen.set_background_pen(color_bg);

   return SUCCESS;
}

int GridLine::set_line_color( int line_color )
{
   LinearGridElement::set_line_color(line_color);
   m_geo_tickmarks.set_line_color(line_color);
   
  
   if ( line_color == UTIL_COLOR_BLACK )
   {
      m_color_bg = UTIL_COLOR_WHITE; 
   }
   else
   {
      m_color_bg = UTIL_COLOR_BLACK; 
   }
   PRM_set_registry_int( "Lat-Long Grid Options", 
               "Grid Line Background Color", m_color_bg );

   OvlPen &pen = m_geo_grid_line.get_pen();
   pen.set_foreground_pen(line_color, m_style, m_width);
   pen.set_background_pen(m_color_bg);

   return SUCCESS;
}

int GridLine::set_line_thickness(int width)
{
   LinearGridElement::set_line_thickness(width);
   m_geo_tickmarks.set_line_thickness(width);

   OvlPen &pen = m_geo_grid_line.get_pen();
   pen.set_foreground_pen(m_color, m_style, width);

   return SUCCESS;
}

int GridLine::set_line_style(int style)
{
   LinearGridElement::set_line_style(style);
   m_geo_tickmarks.set_line_style(UTIL_LINE_TMARK);

   OvlPen &pen = m_geo_grid_line.get_pen();
   pen.set_foreground_pen(m_color, style, m_width);

   return SUCCESS;
}

int GridLine::set_line_type(grid_line_type_t line_type)
{
   if ( m_geo_tickmarks.set_line_type(line_type) != SUCCESS )
   {   
      ERR_report("GridLine::set_line_type() failed.");
      return FAILURE;
   }

   return LinearGridElement::set_line_type(line_type);
}


// A major line is a minorline with tickmarks
int GridLine::draw_major_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to )
{
   // draw Tickmarks first
   if ( m_show_tickmarks == TRUE )
   {
      if ( m_geo_tickmarks.draw_line(map, pDC, geo_from, geo_to ) != SUCCESS )
      {   
         ERR_report("GridLine::draw_major_line() failed.");
         return FAILURE;
      }
   }

   // then draw the minor line
   return draw_minor_line( map, pDC, geo_from, geo_to );
}

int GridLine::draw_minor_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to )
{
   return draw_line(map, pDC, geo_from, geo_to );
}

// *************************************************************
// *************************************************************

// Draw the grid line. 
int GridLine::draw_line(ActiveMap *map, CDC *pDC, d_geo_t& start_angle, d_geo_t& end_angle )
{
   // For labels...
   m_geo_grid_line.set_start( start_angle.lat, start_angle.lon );
   m_geo_grid_line.set_end( end_angle.lat, end_angle.lon );

   // Draw Line
   if ( m_line_type == LAT_POINT ) 
   {
      // New 
      degrees_t geo_width = end_angle.lon - start_angle.lon;
      if (geo_width <= 0.0)
         geo_width += WORLD_DEG;
   
      if ( geo_width >= 180.0 )
      {
         // Draw the line in three steps: from a to b, from b to c, and from c to d
         d_geo_t a, b, c, d;
         a.lat = start_angle.lat;   
         a.lon = start_angle.lon;
         b.lat = start_angle.lat;   
         b.lon = start_angle.lon + 119.999999;
         c.lat = start_angle.lat;   
         c.lon = start_angle.lon + 239.999999;
         
         // Check if overflowed over HALF_WORLD_DEG (180) and correct
         if ( b.lon > HALF_WORLD_DEG )
           b.lon -= WORLD_DEG;
         if (b.lon < -HALF_WORLD_DEG)
            b.lon += WORLD_DEG;
         if ( c.lon > HALF_WORLD_DEG )
           c.lon -= WORLD_DEG;
         if (c.lon < -HALF_WORLD_DEG)
            c.lon += WORLD_DEG;

         d.lat = end_angle.lat;     
         d.lon = end_angle.lon;
         
         if ( add_geo_line( map, a, b ) != SUCCESS )
         {   
            ERR_report("GridLine::draw_minor_line() failed adding a new line.");
            return FAILURE;
         }

         if ( add_geo_line( map, b, c, TRUE ) != SUCCESS )
         {   
            ERR_report("GridLine::draw_minor_line() failed adding a new line.");
            return FAILURE;
         }
   
         if ( add_geo_line( map, c, d, TRUE ) != SUCCESS )
         {   
            ERR_report("GridLine::draw_minor_line() failed adding a new line.");
            return FAILURE;
         }
   
      }
      else
      {
         // geo width < 180.0
         if ( add_geo_line( map, start_angle, end_angle ) != SUCCESS )
         {   
            ERR_report("GridLine::draw_minor_line() failed adding a new line.");
            return FAILURE;
         }

      }
   }
   else
   {
      // Add longitude line
      if ( add_geo_line( map, start_angle, end_angle ) != SUCCESS )
      {   
         ERR_report("GridLine::draw_minor_line() failed adding a new line.");
         return FAILURE;
      }
   }

    return SUCCESS;
}

// *************************************************************
// *************************************************************

int GridLine::add_geo_line( ActiveMap *map, const d_geo_t &start, const d_geo_t &end,
									boolean_t ignore_second /* = FALSE */)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	d_geo_t tstart, tend;

	tstart.lat = start.lat;
	tstart.lon = start.lon;
	tend.lat = end.lat;
	tend.lon = end.lon;

	GeoLine *p_line = new GeoLine(GeoSegment::RHUMB_LINE);
	if ( p_line == NULL )
	{
		// remove previous lines, if any
		reset_geo_line_list();

		ERR_report("GridLine::add_geo_line() failed memory allocation.");
		return FAILURE;
	}

	// convert current datum to WGS84 for drawing
	futil->current_datum_to_wgs84(start.lat, start.lon, &(tstart.lat), &(tstart.lon));
	futil->current_datum_to_wgs84(end.lat, end.lon, &(tend.lat), &(tend.lon));

   //set line attributes
   OvlPen &pen = p_line->get_pen();
   pen.set_foreground_pen(m_color, m_style, m_width);
   pen.set_background_pen(m_color_bg);

	// set line position
	if ( m_line_type == LAT_POINT ) 
	{
		if (tend.lat != tstart.lat)
			tend.lat = tstart.lat;
		p_line->set_start(tstart.lat, tstart.lon);
		p_line->set_end(tend.lat, tend.lon);
	}
	else
	{
		if (tend.lon != tstart.lon)
			tend.lon = tstart.lon;
		p_line->set_start(tend.lat, tend.lon);
		p_line->set_end(tstart.lat, tstart.lon);
	}
   
	// generate line
	p_line->prepare_for_redraw(map);

	// Compute the starting point of this geoline that intersects
	// the view rectangle; used for the line label

	if ( !ignore_second )
	{
		// Get the first part of the line view points
		CList<CPoint *, CPoint *> *list;
		p_line->get_point_list(list);

		if ( !list->IsEmpty() )
		{
			CDC *pDC = map->get_CDC();
			ASSERT( pDC );
			CFvwUtil *futil = CFvwUtil::get_instance();

			// Get the screen size in a CRect
			CRect screen_rect;
			int screen_width, screen_height;
			map->get_surface_size(&screen_width, &screen_height);

			screen_rect.top = 0;
			screen_rect.left = 0;
			screen_rect.right = screen_width;
			screen_rect.bottom = screen_height;

			int x1;
			int y1;
			int x2;
			int y2;

			bool found = false;
			CPoint *first, *second;
			POSITION pos = list->GetHeadPosition();
			
			first = list->GetNext(pos);
			while( pos && !found )
			{
				second = list->GetNext(pos);

				// geoline points are repeated after the first
				if ( (first->x == second->x) && (first->y == second->y) )
				{
					first = second;
					continue;
				}

				x1 = first->x;
				y1 = first->y;
				x2 = second->x;
				y2 = second->y;

				// check if this line intersects the screen
				if ( futil->clip_line(&x1, &y1, &x2, &y2, screen_rect ) )
				{
					// if after clipping the points are the same get the next point
					if ( (x1 == x2) && (y1 == y2) )
						continue;

					found = true;
				}

				first = second;
			}
			m_start_point.x = x1;
			m_start_point.y = y1;
		}
		else
		{
			m_start_point.x = -1;
			m_start_point.y = -1;
		}
	}


   // add line to list
   m_geo_line_list.AddTail(p_line);

   return SUCCESS;
}

// *************************************************************
// *************************************************************

int GridLine::get_start_point( ActiveMap *map, CDC *pDC, CPoint *start_point)
{
	start_point->x = m_start_point.x;
	start_point->y = m_start_point.y;
	return SUCCESS;
}

int GridLine::set_tick_length( int tick_length )
{
   if ( m_geo_tickmarks.set_tick_length( tick_length ) != SUCCESS ) 
   {
      ERR_report("GridLine::set_grid_tick_length() failed.");
      return FAILURE;
   }

   return SUCCESS; 
}

int GridLine::set_show_ticks( int show_ticks )
{
   m_show_tickmarks = show_ticks;

   return SUCCESS; 
}

int GridLine::set_labels_on_major_gridline_show_flag( int show_labels_on_major_lines )
{
   m_show_labels_on_major_lines = show_labels_on_major_lines;
   
   return SUCCESS; 
}

// First get the starting point of the geographical grid line and project into 
// the screen surface; use the resulting point to plot the label around the edge 
// of the screen view rectangle
int GridLine::generate_label( ActiveMap *map, CDC *pDC, int is_major_line, 
                     d_geo_t& geo_angle, degrees_t minor_spacing )
{
   // if the grid line is a minor grid line and the show labels
   // only on major lines is true, do nothing
   if ( !is_major_line )
      if ( m_show_labels_on_major_lines == TRUE )
         return SUCCESS;
   
   degrees_t geo_deg;
   if ( m_line_type == LAT_POINT )
      geo_deg = geo_angle.lat;
   else
      geo_deg = geo_angle.lon;
   

   CPoint line_view_start_point;

   get_start_point( map, pDC, &line_view_start_point);

   double rotation = map->actual_rotation();

   GridLabel *label = new GridLabel(rotation);
   if ( !label )
   {   
      ERR_report("GridOverlay::draw_label() failed (GridLabel *) memory alloc.");
      return FAILURE;
   }

   label->generate( map, pDC, geo_deg, m_line_type, line_view_start_point, minor_spacing );
   m_gridlabel_list.AddTail(label);
     
   return SUCCESS;
}

// To do: separate the draw into generate_label() and redraw()
int GridLine::draw_labels(ActiveMap *map, CDC *pDC )
{
   POSITION pos = m_gridlabel_list.GetHeadPosition();

   while ( pos != NULL )
   {
      GridLabel *p_label = m_gridlabel_list.GetNext(pos);
      ASSERT (p_label );
      p_label->draw( map, pDC );
   }
      
   //reset_label_list();
   
   return SUCCESS;
}

int GridLine::reset_label_list()
{

   while (!m_gridlabel_list.IsEmpty())
      delete m_gridlabel_list.RemoveHead();

   GridLabel::remove_labels(); 
   
   return SUCCESS;
}  

int GridLine::reset_geo_line_list()
{

   while (!m_geo_line_list.IsEmpty())
      delete m_geo_line_list.RemoveHead();

   return SUCCESS;
}

int GridLine::redraw(ActiveMap *map, CDC *pDC, draw_mode_t draw_mode)
{
   // Draw the grid lines
   POSITION pos = m_geo_line_list.GetHeadPosition();
   while ( pos != NULL )
   {
      GeoLine *p_line = m_geo_line_list.GetNext(pos);
      ASSERT (p_line);
      p_line->set_draw_mode( draw_mode );
      if ( p_line->in_view() )
      {
         if ( p_line->view_draw(map, pDC) != SUCCESS )
         {
            ERR_report("GridLine::redraw() failed.");
            return FAILURE;
         }
      }
   }
   
   return SUCCESS;
}



