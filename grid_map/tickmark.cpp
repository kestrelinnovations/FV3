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



-----------------------------------------------------
-  FILE NAME:    tickmarks.cpp
-  LIBRARY NAME: grid_map
-
-  DESCRIPTION:
-
-
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "stdafx.h"
#include "tickmark.h"

#include "err.h"
#include "map.h"
#include "param.h"
#include "fvwutil.h"
#include "ovlelem.h"
#include "spacing.h"


// Statics 
static int adjust_lat_spacing( const MapScale scale, const degrees_t lat, 
                              degrees_t *major_tick_spacing, degrees_t *minor_tick_spacing );

static int compute_tick_end_point( ActiveMap *map, CDC *pDC, grid_line_type_t tick_type,
   const d_geo_t &geo_from, CPoint &p1, CPoint &p2, int distance );


// The tickmark global point list
CList <CPoint*, CPoint*> Tickmarks::m_tickmark_point_list;

Tickmarks::Tickmarks() 
{
   m_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Color", UTIL_COLOR_WHITE );
   m_style = UTIL_LINE_TMARK;
   m_width = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Width", 1 );
   m_color_bg = UTIL_COLOR_BLACK;
   set_pen_properties(m_color, m_style, m_width, m_color_bg);

   m_tick_length = PRM_get_registry_int("Lat-Long Grid Options", "Tickmark Length", 12);
}

Tickmarks::~Tickmarks() 
{
   //TRACE("Tickmarks::~Tickmarks() tick count => %d\n", m_tickmark_point_list.GetCount() );
}

// New
int Tickmarks::draw_major_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to )
{

   return draw_minor_line( map, pDC, geo_from, geo_to );
}

// To be integrated with LinearGridElement
int Tickmarks::draw_minor_line(ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to )
{
   
  return SUCCESS;
}


// To fix the tick spacing in close to pole latitudes
static int adjust_lat_spacing( const MapScale scale, const degrees_t lat, 
                              degrees_t *major_tick_spacing, degrees_t *minor_tick_spacing )
{
   if ( scale == ONE_TO_5M )
   {
      if ( fabs(lat) >= 65.0 )
      {
         *major_tick_spacing = MIN_TO_DEG(15);
         *minor_tick_spacing = 0.0;
      }
   }

   if ( scale == ONE_TO_2M )
   {
      if ( fabs(lat) >= 73.0 )
      {
         *major_tick_spacing = MIN_TO_DEG(30);
         *minor_tick_spacing = MIN_TO_DEG(15);
      }
   }
   
   if ( scale == ONE_TO_1M )
   {
      if ( fabs(lat) >= 64.0 )
      {
         *major_tick_spacing = MIN_TO_DEG(10);
         *minor_tick_spacing = MIN_TO_DEG(5);
      }
   }

   if ( scale == ONE_TO_500K )
   {
      if ( fabs(lat) >= 76.0 )
      {
         *major_tick_spacing = MIN_TO_DEG(5);
         *minor_tick_spacing = MIN_TO_DEG(0);
      }
   }

   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

// Here we draw tickmarks in the direction of this line
int Tickmarks::draw_line( ActiveMap *map, CDC *pDC, d_geo_t& geo_from, d_geo_t& geo_to )
{
   // Spacing attributes for tickmarks
   degrees_t major_tick_spacing;
   degrees_t minor_tick_spacing;

   GridSpacing grid_props;
     
   // For a lat line, draw lon oriented ticks and viceversa
   if ( m_line_type == LAT_POINT )
      m_tick_type = LON_POINT;
   else if ( m_line_type == LON_POINT )
      m_tick_type = LAT_POINT;
   else
   {
      ERR_report(" Invalid line type in Tickmarks::draw_line().");
      return FAILURE;
   }

   if ( grid_props.get_tick_spacing( m_tick_type, map->scale(), 
               &major_tick_spacing, &minor_tick_spacing ) != SUCCESS ) 
   {
      ERR_report("grid_props.get_tick_spacing() failed.");
      return FAILURE;
   }
 
   // If no tick marks defined, do nothing  
   if ( minor_tick_spacing == 0.0 && major_tick_spacing == 0.0 )
      return SUCCESS;

   // adjust the spacing for higher (northern and southern) latitudes
   if ( m_line_type == LAT_POINT )
      adjust_lat_spacing( map->scale(), geo_from.lat, &major_tick_spacing, &minor_tick_spacing );


   // The total number of tickmarks to draw
   degrees_t length;   
   if ( m_line_type == LAT_POINT )
   {
      length = geo_to.lon - geo_from.lon;
      if (length <= 0.0)
         length += WORLD_DEG;
   }
   else
      length = geo_to.lat - geo_from.lat;


   int number_of_major_ticks = 0;
   int number_of_minor_ticks = 0;

   if ( major_tick_spacing != 0.0 )
      number_of_major_ticks = (int)ceil(length/major_tick_spacing);
   
   if ( minor_tick_spacing != 0.0 )
      number_of_minor_ticks = (int)ceil(length/minor_tick_spacing);
   
   // Generate the minor tickmarks 
   if ( number_of_minor_ticks > 0 )
      if ( generate_ticks( map, pDC, number_of_minor_ticks, m_tick_length/2 + m_width, 
         minor_tick_spacing, geo_from, geo_to ) != SUCCESS ) 
      {
         ERR_report("generate_ticks() failed.");
         return FAILURE;
      }
 
   // Generate the major tickmarks 
   if ( number_of_major_ticks > 0 )
      if ( generate_ticks( map, pDC, number_of_major_ticks, m_tick_length + m_width, 
         major_tick_spacing, geo_from, geo_to ) != SUCCESS )
      {
         ERR_report("generate_ticks() failed.");
         return FAILURE;
      }

   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int Tickmarks::generate_label( ActiveMap *map, CDC *pDC, int is_major_line,
               d_geo_t& geo_angle, degrees_t minor_spacing )
{
   // For future use 
   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

static int compute_tick_end_point( ActiveMap *map, CDC *pDC, grid_line_type_t tick_type,
						const d_geo_t &geo_from, CPoint &p1, CPoint &p2, int distance )
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	int x, y;
	int view_x, view_y;
	double tlat, tlon;

	// convert current datum to WGS84 for drawing
	futil->current_datum_to_wgs84(geo_from.lat, geo_from.lon, &tlat, &tlon);

	// project starting point to the screen surface
	map->geo_to_vsurface(tlat, tlon, &x, &y );
	map->vsurface_to_surface( x, y, (int*)&p1.x, (int*)&p1.y);

	if ( tick_type == LON_POINT )
	{
		if ( geo_from.lat >= 0.0 )
		{
			if ( map->vsurface_to_surface( x, y - distance, &view_x, &view_y ) != SUCCESS )
				return FAILURE;
		}
		else
		{
			if ( map->vsurface_to_surface( x, y + distance, &view_x, &view_y ) != SUCCESS )
				return FAILURE;
		}
	}
	else if ( tick_type == LAT_POINT )
	{
		if ( tlon >= 0.0 && tlon <= 180.0)
		{
         if ( map->vsurface_to_surface( x + distance, y, &view_x, &view_y ) != SUCCESS )
				return FAILURE;
		}
		else
		{
         if ( map->vsurface_to_surface( x - distance, y, &view_x, &view_y ) != SUCCESS )
				return FAILURE;
		}
	}
	else
	{
		ERR_report("Invalid ticmark type in compute_tick_end_point().");
		return FAILURE;
	}

	p2.x = view_x;
	p2.y = view_y;

	if ( futil->magnitude(p1.x, p1.y, view_x, view_y) > abs(2*distance) ) 
		return FAILURE;

	return SUCCESS;
}
// end of compute_tick_end_point

// **********************************************************************
// **********************************************************************

// To DO: Integrate this line generation with LinearGridElement::draw()...
int Tickmarks::generate_ticks( ActiveMap *map, CDC *pDC, int number_of_ticks,
      int tick_length, degrees_t tick_spacing, d_geo_t& geo_start, d_geo_t& geo_end )
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	double tlat, tlon;

	if ( tick_spacing == 0.0 )
		return SUCCESS;

   
	// Check against a black foreground line
	// This code need to be placed up in the hierarchy of grid lines
	if ( m_color == UTIL_COLOR_BLACK )
		m_color_bg = UTIL_COLOR_WHITE; 
	else
		m_color_bg = UTIL_COLOR_BLACK; 


	UtilDraw line(pDC);
	CPoint p1, p2;
	d_geo_t grid_point, geo_from;
	int k;

	if ( m_tick_type == LON_POINT )
	{
		round_up(tick_spacing, geo_start.lon, &grid_point.lon );
		geo_from.lat = geo_start.lat;
      
		for ( k = 0; k < number_of_ticks; k++ ) 
		{
			if (GEO_lon_in_range(geo_start.lon, geo_end.lon, grid_point.lon) == TRUE ) 
			{         
				geo_from.lon = grid_point.lon;
        
				// convert current datum to WGS84 for drawing
				futil->current_datum_to_wgs84(geo_from.lat, geo_from.lon, &tlat, &tlon);
				if ( map->geo_in_surface( tlat, tlon ) )
				{
					if ( compute_tick_end_point( map, pDC, m_tick_type, geo_from, 
												p1, p2, tick_length) == SUCCESS )
						add_tickmark_points( map, p1.x, p1.y, p2.x, p2.y );

					// If at equator, draw other side also
					if ( geo_from.lat == 0.0 )
					{            
						if ( compute_tick_end_point( map, pDC, m_tick_type, geo_from, 
										p1, p2, -tick_length) == SUCCESS )
							add_tickmark_points( map, p1.x, p1.y, p2.x, p2.y );
					}

				}

				// compute the next tick longitude
				grid_point.lon += tick_spacing;
				if (grid_point.lon > MAX_LON_DEG) 
					grid_point.lon -= WORLD_DEG;
			} 
			else
				break;
		}
	} 
	else 
	{    
		// Latitude oriented ticks for longitude lines

		round_up(tick_spacing, geo_start.lat, &grid_point.lat );
		geo_from.lon = geo_start.lon;

		for ( k = 0; k < number_of_ticks; k++ ) 
		{
			if (grid_point.lat < geo_end.lat) 
			{
				geo_from.lat = grid_point.lat;

				// convert current datum to WGS84 for drawing
				futil->current_datum_to_wgs84(geo_from.lat, geo_from.lon, &tlat, &tlon);
				if ( map->geo_in_surface(tlat, tlon ) )
				{
					if ( compute_tick_end_point( map, pDC, m_tick_type, geo_from, 
												p1, p2, tick_length) == SUCCESS )
						add_tickmark_points( map, p1.x, p1.y, p2.x, p2.y );

					// If at IDL, draw other side also
					if ( geo_from.lon == 180.0 || geo_from.lon == 0.0 )
					{
						if ( compute_tick_end_point( map, pDC, m_tick_type, geo_from, 
								p1, p2, -tick_length) == SUCCESS )
							add_tickmark_points( map, p1.x, p1.y, p2.x, p2.y );
					}

				}

				grid_point.lat += tick_spacing;
			} 
			else
				break;
		}
	}

   return SUCCESS;
}
// end of generate_ticks

// **********************************************************************
// **********************************************************************

int Tickmarks::redraw(ActiveMap *map, CDC *pDC, draw_mode_t draw_mode, 
                      int color, int width, int color_bg)
{
   UtilDraw utilDraw(pDC);

   if (draw_mode == BACKGROUND_MODE)
      utilDraw.set_pen(color_bg, UTIL_LINE_TMARK, width + 2, TRUE);
   else
      utilDraw.set_pen(color, UTIL_LINE_TMARK, width);

   POSITION pos = m_tickmark_point_list.GetHeadPosition();
   while ( pos != NULL )
   {
      CPoint *p1 = m_tickmark_point_list.GetNext(pos);
      ASSERT (p1);
      CPoint *p2 = m_tickmark_point_list.GetNext(pos);
      ASSERT (p2);

      utilDraw.draw_line(map, p1->x, p1->y, p2->x, p2->y);
   }
   
   return SUCCESS;
}

int Tickmarks::add_tickmark_points( ActiveMap *map, int x1, int y1, int x2, int y2 )
{

   // Allocate memory for new tickmark end points 
   CPoint *p1 = new CPoint(x1, y1);
   if ( p1 == NULL )
   {
      // remove previous lines, if any
      reset_tickmark_point_list();

      ERR_report("Tickmarks::add_tickmark_points() failed memory allocation.");
      return FAILURE;
   }
   CPoint *p2 = new CPoint(x2, y2);
   if ( p2 == NULL )
   {
      // remove previous point
      if ( p1 )
         delete p1;
      
      // remove previous lines, if any
      reset_tickmark_point_list();

      ERR_report("Tickmarks::add_tickmark_points() failed memory allocation.");
      return FAILURE;
   }
   
   // add line points to tickmark list
   m_tickmark_point_list.AddTail(p1);
   m_tickmark_point_list.AddTail(p2);

   return SUCCESS;
}

int Tickmarks::reset_tickmark_point_list()
{

   while (!m_tickmark_point_list.IsEmpty())
      delete m_tickmark_point_list.RemoveHead();

   return SUCCESS;
}

// Set the drawing mode to the GeoLine object
draw_mode_t Tickmarks::set_draw_mode( draw_mode_t mode )
{
   return LinearGridElement::set_draw_mode( mode );
}

int Tickmarks::set_tick_length(int tick_length)
{
   m_tick_length = tick_length;

  return SUCCESS; 
}

int Tickmarks::set_pen_properties(int color, int style, int width, int color_bg)
{
   LinearGridElement::set_pen_properties(color, style, width, color_bg);

   return SUCCESS;
}

int Tickmarks::set_line_color( int line_color )
{
   LinearGridElement::set_line_color(line_color);
   
   if ( line_color == UTIL_COLOR_BLACK )
   {
      m_color_bg = UTIL_COLOR_WHITE; 
   }
   else
   {
      m_color_bg = UTIL_COLOR_BLACK; 
   }

   return SUCCESS;
}

int Tickmarks::set_line_thickness(int width)
{
   LinearGridElement::set_line_thickness(width);

   return SUCCESS;
}

int Tickmarks::set_line_style(int style)
{
   LinearGridElement::set_line_style(style);

   return SUCCESS;
}

   
int Tickmarks::set_line_type(grid_line_type_t line_type)
{
   return LinearGridElement::set_line_type(line_type);
}

