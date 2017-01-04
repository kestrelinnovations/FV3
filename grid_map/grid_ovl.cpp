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
-  FILE NAME:    grid_ovl.cpp
-  LIBRARY NAME: grid_map
-
-  DESCRIPTION:
-
-      This file contains the Lat-Long grid overlay display routine.
-
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/


#include "stdafx.h"
#include "grid_map.h"
#include "map.h"
#include "param.h"
#include "spacing.h"
#include "err.h"
#include "geotrans.h"
#include "refresh.h"
#include "proj.h"


/*------------------------------------------------------------------
-                            Typedefs
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                     Static Function Prototypes
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-  FUNCTION NAME: GRD_display_grid_overlay
-
-  PURPOSE:
-
-      Display a Lat-Long grid for the current map library projection.
-
-  DESCRIPTION:
-
-      The current geographic map bounds from the map library are
-  used as the source of the map projection. The map bounds determine
-  the spacing between latitude and longitude grid lines and the format
-  of their labels. The grid map can be drawn for any equal arc projection,
-  but the desired projection must be setup via the map library before
-  this function is called. If MAP_get_map_bounds() returns FAILURE
-  then the this function returns FAILURE.
-------------------------------------------------------------------*/

int GridOverlay::display_grid_overlay(ActiveMap* map)
{
    CFvwUtil *futil = CFvwUtil::get_instance();
    int rslt;
    d_geo_t map_ur, map_ll;          // region of screen in degrees

    CDC *dc = map->get_CDC();

    m_font_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Color", UTIL_COLOR_WHITE );
    m_font_bg_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Background Color", UTIL_COLOR_BLACK );

    m_font_name = PRM_get_registry_string( "Lat-Long Grid Options", "Grid Label Font Name", "Courier New" );

    m_font_size = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Size", 20 );
    m_font_attrib = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Attributes", 0);

    m_font_background = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Background", 0);

    // get the map bounds from the map projection
    if (map->get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
    {
      ERR_report("get_vmap_bounds() failed.");
      return FAILURE;
    }

    // convert to current datum
    futil->wgs84_to_current_datum(map_ll.lat, map_ll.lon, &(map_ll.lat), &(map_ll.lon));
    futil->wgs84_to_current_datum(map_ur.lat, map_ur.lon, &(map_ur.lat), &(map_ur.lon));

    int grid_type = PRM_get_registry_int("Lat-Long Grid Options",
        "GridType", 1);

    if (grid_type == 2) // MGRS
   {
        rslt = draw_utm_grid(map);
   }
   else if (grid_type == 3) // GARS
   {
      rslt = draw_gars(map);
   }
    else // Lat / Lon
    {
       rslt = draw_grid( map, dc, map_ll, map_ur );
    }

    if (rslt != SUCCESS)
    {
        ERR_report("drawing of coordinate grid failed.");
        return FAILURE;
    }


   return SUCCESS;
}
// end of display_grid_overlay

// **********************************************************************
// **********************************************************************

int GridOverlay::draw_grid(ActiveMap *map, CDC *pDC,
                           d_geo_t map_ll, d_geo_t map_ur )  // current datum
{
   if ( !get_valid() )
   {
      degrees_t lat_major_grid_spacing;
      degrees_t lat_minor_grid_spacing;
      degrees_t lon_major_grid_spacing;
      degrees_t lon_minor_grid_spacing;

      GridSpacing grid_spacing;
      if ( grid_spacing.get_grid_spacing( LAT_POINT, map->scale(),
            &lat_major_grid_spacing, &lat_minor_grid_spacing ) != SUCCESS ) {
         ERR_report("GridOverlay::draw_grid -> grid_props.get_grid_spacing() failed.");
         return FAILURE;
      }
      if ( grid_spacing.get_grid_spacing( LON_POINT, map->scale(),
            &lon_major_grid_spacing, &lon_minor_grid_spacing ) != SUCCESS ) {
         ERR_report("GridOverlay::draw_grid -> grid_props.get_grid_spacing() failed.");
         return FAILURE;
      }

      if ( lat_major_grid_spacing == 0.0 || lat_minor_grid_spacing == 0.0 )
      {
         return SUCCESS;
      }
      if ( lon_major_grid_spacing == 0.0 || lon_minor_grid_spacing == 0.0 )
      {
         return SUCCESS;
      }

      // COMPUTE NUMBER OF LINES
      // compute the maps geographic dimensions
      degrees_t geo_height = map_ur.lat - map_ll.lat;
      degrees_t geo_width = map_ur.lon - map_ll.lon;
      if (geo_width <= 0.0)
         geo_width += WORLD_DEG;

      // Longitude lines
      int number_of_major_latitudes = (int)ceil(geo_height/lat_major_grid_spacing);
      int number_of_minor_latitudes = (int)ceil(geo_height/lat_minor_grid_spacing);

      // Latitude lines
      int number_of_major_longitudes = (int)ceil(geo_width/lon_major_grid_spacing);
      int number_of_minor_longitudes = (int)ceil(geo_width/lon_minor_grid_spacing);

      // Remove previous lines (and labels), if any
      GridLine::reset_label_list();
      GridLine::reset_geo_line_list();
      Tickmarks::reset_tickmark_point_list();

      // Generate latitudes
      if ( geo_lat_line.draw(map, pDC, number_of_minor_latitudes,
         lat_major_grid_spacing, lat_minor_grid_spacing, map_ll, map_ur ) != SUCCESS )
      {
         ERR_report("GridLine::draw() failed.");
         return FAILURE;
      }

      // generate longitudes
      if ( geo_lon_line.draw(map, pDC, number_of_minor_longitudes,
         lon_major_grid_spacing, lon_minor_grid_spacing, map_ll, map_ur ) != SUCCESS )
      {
         ERR_report("GridLine::draw() failed.");
         return FAILURE;
      }

      set_valid( TRUE );
   }

   // draw the background generated lines and tickmarks
   Tickmarks::redraw(map, pDC, BACKGROUND_MODE, m_color, m_width, m_color_bg);
   GridLine::redraw(map, pDC, BACKGROUND_MODE);

   // draw the foreground generated lines and tickmarks
   Tickmarks::redraw(map, pDC, FOREGROUND_MODE, m_color, m_width, m_color_bg);
   GridLine::redraw(map, pDC, FOREGROUND_MODE);

   // Draw the labels
   if ( GridLine::draw_labels(map, pDC) != SUCCESS )
   {
      ERR_report("GridLine::draw_labels() failed.");
      return FAILURE;
   }

   return SUCCESS;
}
// end of draw_grid

// **********************************************************************
// **********************************************************************

int GridOverlay::set_grid_line_color( int line_color )
{
   m_color = line_color;

   if ( geo_lat_line.set_line_color( line_color ) != SUCCESS )
   {
      ERR_report("GridLine::set_line_color() failed.");
      return FAILURE;
   }

   if ( geo_lon_line.set_line_color( line_color ) != SUCCESS )
   {
      ERR_report("GridLine::set_line_color() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::set_grid_line_style( int line_style )
{
   m_style = line_style;

   if ( geo_lat_line.set_line_style( line_style ) != SUCCESS )
   {
      ERR_report("GridLine::set_line_style() failed.");
      return FAILURE;
   }

   if ( geo_lon_line.set_line_style( line_style ) != SUCCESS )
   {
      ERR_report("GridLine::set_line_style() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::set_grid_line_thickness( int width )
{
   m_width = width;

   if ( geo_lat_line.set_line_thickness( width ) != SUCCESS )
   {
      ERR_report("GridLine::set_line_thickness() failed.");
      return FAILURE;
   }

   if ( geo_lon_line.set_line_thickness( width ) != SUCCESS )
   {
      ERR_report("GridLine::set_line_thickness() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::set_grid_tick_length( int tick_length )
{
   if ( geo_lat_line.set_tick_length( tick_length ) != SUCCESS )
   {
      ERR_report("Tickmarks::set_tick_length() failed.");
      return FAILURE;
   }
   if ( geo_lon_line.set_tick_length( tick_length ) != SUCCESS )
   {
      ERR_report("Tickmarks::set_tick_length() failed.");
      return FAILURE;
   }
   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::set_grid_tick_show_flag( int show_ticks )
{
   m_show_tickmarks = show_ticks;

   if ( geo_lat_line.set_show_ticks( show_ticks ) != SUCCESS )
   {
      ERR_report("GridLine::set_line_thickness() failed.");
      return FAILURE;
   }

   if ( geo_lon_line.set_show_ticks( show_ticks ) != SUCCESS )
   {
      ERR_report("GridLine::set_line_thickness() failed.");
      return FAILURE;
   }


   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::set_minor_gridline_show_flag( int show_minor_gridlines )
{
   m_show_minor_gridlines = show_minor_gridlines;

   if ( geo_lat_line.set_minor_line_show_flag( show_minor_gridlines ) != SUCCESS )
   {
      ERR_report("GridLine::set_minor_gridline_show_flag() failed.");
      return FAILURE;
   }

   if ( geo_lon_line.set_minor_line_show_flag( show_minor_gridlines ) != SUCCESS )
   {
      ERR_report("GridLine::set_minor_gridline_show_flag() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::set_labels_on_major_gridline_show_flag( int show_labels_on_major_lines )
{
   m_show_labels_on_major_lines = show_labels_on_major_lines;

   if ( geo_lat_line.set_labels_on_major_gridline_show_flag(show_labels_on_major_lines)!= SUCCESS )
   {
      ERR_report("GridLine::set_labels_on_major_gridline_show_flag() failed.");
      return FAILURE;
   }

   if ( geo_lon_line.set_labels_on_major_gridline_show_flag(show_labels_on_major_lines) != SUCCESS )
   {
      ERR_report("GridLine::set_labels_on_major_gridline_show_flag() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::set_grid_label_properties( int label_color, int label_bg_color, CString label_font_name,
                                       int label_size, int label_attrib, int background )
{
   if ( GridLabel::set_properties( label_color, label_bg_color, label_font_name,
            label_size, label_attrib, background ) != SUCCESS )
   {
      ERR_report("GridLabel::set_properties() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::get_grid_label_properties( int *label_color, int *label_bg_color, CString *label_font_name,
                                       int *label_size, int *label_attrib, int *background )
{
   if ( GridLabel::get_properties( label_color, label_bg_color, label_font_name,
            label_size, label_attrib, background ) != SUCCESS )
   {
      ERR_report("GridLabel::get_properties() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

// ///////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Grid Overlay Class Implementation

// Constructor
GridOverlay::GridOverlay()
{
   geo_lat_line.set_line_type(LAT_POINT);
   geo_lon_line.set_line_type(LON_POINT);

   m_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Color", UTIL_COLOR_WHITE );
   m_style = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Style", UTIL_LINE_SOLID );
   m_width = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Width", 1 );
   m_color_bg = UTIL_COLOR_BLACK;

   m_show_tickmarks = PRM_get_registry_int( "Lat-Long Grid Options", "Show Tickmarks", 1 );
   m_show_minor_gridlines = PRM_get_registry_int( "Lat-Long Grid Options", "Show Minor GridLines", 1 );
   m_show_labels_on_major_lines = PRM_get_registry_int( "Lat-Long Grid Options",
                  "Show Labels on Major Gridlines", 1 );

   int label_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Color", UTIL_COLOR_WHITE );
   int label_bg_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Background Color", UTIL_COLOR_BLACK );

   CString label_font_name = PRM_get_registry_string( "Lat-Long Grid Options", "Grid Label Font Name", "Courier New" );

   int label_size = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Size", 20 );
   int label_attrib = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Attributes", 0);
   int background = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Background", 0);

   GridLabel::set_properties( label_color, label_bg_color, label_font_name,
         label_size, label_attrib, background );
}

GridOverlay::~GridOverlay()
{
    // Most, if not all, destruction should be accomplished in Finalize.  
    // Due to C# objects registering for notifications, pointers to this object may not be 
    // released until the next garbage collection.
}

void GridOverlay::Finalize()
{
    // delete all genereated grid lines and tickmarks
   GridLine::reset_label_list();
   GridLine::reset_geo_line_list();
   Tickmarks::reset_tickmark_point_list();
}


// **********************************************************************
// **********************************************************************

// draw major UTM grid zones

void GridOverlay::draw_major_zone_labels_corners(ActiveMap* map, CDC *dc)
{
    CFvwUtil *futil = CFvwUtil::get_instance();
    double lat1, lon1;
    double tlat, tlon, map_angle;
    int rslt, tx, ty, tx2, ty2;
   const int MILGRID_LEN = 81;
   const int SLINE_LEN = 4;
    char milgrid[MILGRID_LEN], sline[SLINE_LEN];
    CString tstr;
    POINT cpt[4];
    CGeoTrans trans;
    int utm_zone;
    double utm_northing, utm_easting;
    int screen_width, screen_height;
   const int DATUM_TYPE_LEN = 21;
    char datum_type[21];
   const int DATUM_LEN = 41;
    char datum[DATUM_LEN];

    GEO_get_default_display(datum_type, DATUM_TYPE_LEN);
    if (!strcmp(datum_type, "SECONDARY"))
        GEO_get_secondary_datum(datum, DATUM_LEN);
    else
        GEO_get_primary_datum(datum, DATUM_LEN);

    map->get_surface_size(&screen_width, &screen_height);

    map_angle = map->actual_rotation();

    lat1 = -80.0;

    while (lat1 <= 72.0)
    {
        lon1 = -180.0;
        while (lon1 < 180.0)
        {
            map->geo_to_surface(lat1, lon1, &tx, &ty);
            if ((tx > -30) && (tx < screen_width + 30) &&
                (ty > -30) && (ty < screen_height + 30))
            {
                futil->rotate_pt(tx-5, ty-5, &tx2, &ty2, map_angle, tx, ty);
                map->surface_to_geo(tx2, ty2, &tlat, &tlon);
                rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
                if (rslt == SUCCESS)
                {
                    strncpy_s(sline, SLINE_LEN, milgrid, 3);
                    sline[3] = '\0';
                    tstr = sline;
                    futil->draw_text(dc, tstr, tx2, ty2, UTIL_ANCHOR_LOWER_RIGHT, m_font_name, m_font_size+2,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }

                futil->rotate_pt(tx+5, ty-5, &tx2, &ty2, map_angle, tx, ty);
                map->surface_to_geo(tx2, ty2, &tlat, &tlon);
                rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
                if (rslt == SUCCESS)
                {
                    strncpy_s(sline, SLINE_LEN, milgrid, 3);
                    sline[3] = '\0';
                    tstr = sline;
                    futil->draw_text(dc, tstr, tx2, ty2, UTIL_ANCHOR_LOWER_LEFT, m_font_name, m_font_size+2,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }

                futil->rotate_pt(tx-5, ty+5, &tx2, &ty2, map_angle, tx, ty);
                map->surface_to_geo(tx2, ty2, &tlat, &tlon);
                rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
                if (rslt == SUCCESS)
                {
                    strncpy_s(sline, SLINE_LEN, milgrid, 3);
                    sline[3] = '\0';
                    tstr = sline;
                    futil->draw_text(dc, tstr, tx2, ty2, UTIL_ANCHOR_UPPER_RIGHT, m_font_name, m_font_size+2,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }

                futil->rotate_pt(tx+5, ty+5, &tx2, &ty2, map_angle, tx, ty);
                map->surface_to_geo(tx2, ty2, &tlat, &tlon);
                rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
                if (rslt == SUCCESS)
                {
                    strncpy_s(sline, SLINE_LEN, milgrid, 3);
                    sline[3] = '\0';
                    tstr = sline;
                    futil->draw_text(dc, tstr, tx2, ty2, UTIL_ANCHOR_UPPER_LEFT, m_font_name, m_font_size+2,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }
            }

            lon1 += 6.0;
        }
        lat1 += 8.0;
    }
}
// end of draw_major_zone_labels_corners

// **********************************************************************
// **********************************************************************

// draw major UTM grid zones

void GridOverlay::draw_major_zone_labels_center(ActiveMap* map, CDC *dc)
{
    CFvwUtil *futil = CFvwUtil::get_instance();
    double lat1, lon1;
    double tlat, tlon, map_angle;
    int rslt, tx, ty;
   const int MILGRID_LEN = 81;
   const int SLINE_LEN = 4;
    char milgrid[MILGRID_LEN], sline[SLINE_LEN];
    CString tstr;
    POINT cpt[4];
    CGeoTrans trans;
    int utm_zone;
    double utm_northing, utm_easting;
    int screen_width, screen_height;
   const int DATUM_TYPE_LEN = 21;
    char datum_type[DATUM_TYPE_LEN];
   const int DATUM_LEN = 41;
    char datum[DATUM_LEN];

    GEO_get_default_display(datum_type, DATUM_TYPE_LEN);
    if (!strcmp(datum_type, "SECONDARY"))
        GEO_get_secondary_datum(datum, DATUM_LEN);
    else
        GEO_get_primary_datum(datum, DATUM_LEN);

    map->get_surface_size(&screen_width, &screen_height);

    map_angle = map->actual_rotation();

    // draw the labels 56 degrees and below
    lat1 = -80.0;
    lon1 = -180.0;
    while (lat1 < 56.0)
    {
        lon1 = -180.0;
        while (lon1 < 180.0)
        {
            tlat = lat1 + 4.0;
            tlon = lon1 + 3.0;
            map->geo_to_surface(tlat, tlon, &tx, &ty);
            if ((tx > -30) && (tx < screen_width + 30) &&
                (ty > -30) && (ty < screen_height + 30))
            {
                rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
                if (rslt == SUCCESS)
                {
                    strncpy_s(sline, SLINE_LEN, milgrid, 3);
                    sline[3] = '\0';
                    tstr = sline;
                    futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }
            }

            lon1 += 6.0;
        }
        lat1 += 8.0;
    }

    // draw the labels 56 to 64 degrees
    lon1 = -180.0;
    lat1 = 56.0;
    while (lon1 < 0.0)
    {
        tlat = lat1 + 4.0;
        tlon = lon1 + 3.0;
        map->geo_to_surface(tlat, tlon, &tx, &ty);
        if ((tx > -30) && (tx < screen_width + 30) &&
            (ty > -30) && (ty < screen_height + 30))
        {
            rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
            if (rslt == SUCCESS)
            {
                strncpy_s(sline, SLINE_LEN, milgrid, 3);
                sline[3] = '\0';
                tstr = sline;
                futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }
        lon1 += 6.0;
    }

    tlat = 60.0;
    tlon = 1.5;
    map->geo_to_surface(tlat, tlon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
        if (rslt == SUCCESS)
        {
            strncpy_s(sline, SLINE_LEN, milgrid, 3);
            sline[3] = '\0';
            tstr = sline;
            futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                            m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
        }
    }

    tlat = 60.0;
    tlon = 7.5;
    map->geo_to_surface(tlat, tlon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
        if (rslt == SUCCESS)
        {
            strncpy_s(sline, SLINE_LEN, milgrid, 3);
            sline[3] = '\0';
            tstr = sline;
            futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                            m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
        }
    }

    lon1 = 12.0;
    lat1 = 56.0;
    while (lon1 < 180.0)
    {
        tlat = lat1 + 4.0;
        tlon = lon1 + 3.0;
        map->geo_to_surface(tlat, tlon, &tx, &ty);
        if ((tx > -30) && (tx < screen_width + 30) &&
            (ty > -30) && (ty < screen_height + 30))
        {
            rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
            if (rslt == SUCCESS)
            {
                strncpy_s(sline, SLINE_LEN, milgrid, 3);
                sline[3] = '\0';
                tstr = sline;
                futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }
        lon1 += 6.0;
    }

    // draw the labels 64 to 72 degrees
    lon1 = -180.0;
    lat1 = 64.0;
    while (lon1 < 180.0)
    {
        tlat = lat1 + 4.0;
        tlon = lon1 + 3.0;
        map->geo_to_surface(tlat, tlon, &tx, &ty);
        if ((tx > -30) && (tx < screen_width + 30) &&
            (ty > -30) && (ty < screen_height + 30))
        {
            rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
            if (rslt == SUCCESS)
            {
                strncpy_s(sline, SLINE_LEN, milgrid, 3);
                sline[3] = '\0';
                tstr = sline;
                futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }
        lon1 += 6.0;
    }

    // draw the labels 72 to 84 degrees
    lon1 = -180.0;
    lat1 = 72.0;
    while (lon1 < 0.0)
    {
        tlat = lat1 + 6.0;
        tlon = lon1 + 3.0;
        map->geo_to_surface(tlat, tlon, &tx, &ty);
        if ((tx > -30) && (tx < screen_width + 30) &&
            (ty > -30) && (ty < screen_height + 30))
        {
            rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
            if (rslt == SUCCESS)
            {
                strncpy_s(sline, SLINE_LEN, milgrid, 3);
                sline[3] = '\0';
                tstr = sline;
                futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }
        lon1 += 6.0;
    }

    tlat = 78.0;
    tlon = 4.5;
    map->geo_to_surface(tlat, tlon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
        if (rslt == SUCCESS)
        {
            strncpy_s(sline, SLINE_LEN, milgrid, 3);
            sline[3] = '\0';
            tstr = sline;
            futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                            m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
        }
    }

    tlat = 78.0;
    tlon = 15.0;
    map->geo_to_surface(tlat, tlon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
        if (rslt == SUCCESS)
        {
            strncpy_s(sline, SLINE_LEN, milgrid, 3);
            sline[3] = '\0';
            tstr = sline;
            futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                            m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
        }
    }

    tlat = 78.0;
    tlon = 27.0;
    map->geo_to_surface(tlat, tlon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
        if (rslt == SUCCESS)
        {
            strncpy_s(sline, SLINE_LEN, milgrid, 3);
            sline[3] = '\0';
            tstr = sline;
            futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                            m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
        }
    }

    tlat = 78.0;
    tlon = 37.5;
    map->geo_to_surface(tlat, tlon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
        if (rslt == SUCCESS)
        {
            strncpy_s(sline, SLINE_LEN, milgrid, 3);
            sline[3] = '\0';
            tstr = sline;
            futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                            m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
        }
    }

    lon1 = 42.0;
    lat1 = 72.0;
    while (lon1 < 180.0)
    {
        tlat = lat1 + 6.0;
        tlon = lon1 + 3.0;
        map->geo_to_surface(tlat, tlon, &tx, &ty);
        if ((tx > -30) && (tx < screen_width + 30) &&
            (ty > -30) && (ty < screen_height + 30))
        {
            rslt = trans.convert_geo( tlat, tlon, datum, utm_zone, utm_northing, utm_easting, milgrid, MILGRID_LEN);
            if (rslt == SUCCESS)
            {
                strncpy_s(sline, SLINE_LEN, milgrid, 3);
                sline[3] = '\0';
                tstr = sline;
                futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size+2,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }
        lon1 += 6.0;
    }

}
// end of draw_major_zone_labels_center

// **********************************************************************
// **********************************************************************

// draw major UTM grid zones

void GridOverlay::draw_major_zones(ActiveMap* map, CDC *dc)
{
    CFvwUtil *futil = CFvwUtil::get_instance();
    double lat1, lon1, lat2, lon2;
    double tlat1, tlon1, tlat2, tlon2;
    CString tstr;
    CGeoTrans trans;
    int screen_width, screen_height;
   const int DATUM_TYPE_LEN = 21;
    char datum_type[DATUM_TYPE_LEN];
   const int DATUM_LEN = 41;
    char datum[DATUM_LEN];
    d_geo_t ll, ur;

    // get the map bounds to clip symbol drawing
    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
        return;

    GEO_get_default_display(datum_type, DATUM_TYPE_LEN);
    if (!strcmp(datum_type, "SECONDARY"))
        GEO_get_secondary_datum(datum, DATUM_LEN);
    else
        GEO_get_primary_datum(datum, DATUM_LEN);

    map->get_surface_size(&screen_width, &screen_height);

    lat1 = 56.0;
    lat2 = -80.0;
    lon1 = -180.0;

    // draw the north-south lines 56 degrees and below
    while (lon1 < 180.0)
    {
        lon2 = lon1;
        for (lat1=-80; lat1<56.0; lat1+=8.0)
        {
            lat2 = lat1 + 8.0;
            trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
            trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
            tlon1 = futil->limit(tlon1, -180.0, 180.0);
            tlon2 = futil->limit(tlon2, -180.0, 180.0);
            futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                        m_color, PS_SOLID, m_width+2,
                                        UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
        }
        lon1 += 6.0;
    }

    // draw the north-south lines 56 to 64 degrees
    lat1 = 56.0;
    lat2 = 64.0;
    lon1 = -180.0;
    while (lon1 < 6.0)
    {
        lon2 = lon1;
        trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
        trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
        tlon1 = futil->limit(tlon1, -180.0, 180.0);
        tlon2 = futil->limit(tlon2, -180.0, 180.0);
        futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                    UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
        lon1 += 6.0;
    }
    lon1 = 3.0;
    lon2 = lon1;
    trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
    trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
    tlon1 = futil->limit(tlon1, -180.0, 180.0);
    tlon2 = futil->limit(tlon2, -180.0, 180.0);
    futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
    lon1 = 12.0;
    while (lon1 < 180.0)
    {
        lon2 = lon1;
        trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
        trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
        tlon1 = futil->limit(tlon1, -180.0, 180.0);
        tlon2 = futil->limit(tlon2, -180.0, 180.0);
        futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                    UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
        lon1 += 6.0;
    }

    // draw the north-south lines 64 to 72 degrees
    lat1 = 64.0;
    lat2 = 72.0;
    lon1 = -180.0;
    while (lon1 < 180.0)
    {
        lon2 = lon1;
        trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
        trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
        tlon1 = futil->limit(tlon1, -180.0, 180.0);
        tlon2 = futil->limit(tlon2, -180.0, 180.0);
        futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                    UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
        lon1 += 6.0;
    }

    // draw the north-south lines 72 to 84 degrees
    lat1 = 72.0;
    lat2 = 84.0;
    lon1 = -180.0;
    while (lon1 < 6.0)
    {
        lon2 = lon1;
        trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
        trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
        tlon1 = futil->limit(tlon1, -180.0, 180.0);
        tlon2 = futil->limit(tlon2, -180.0, 180.0);
        futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                    UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
        lon1 += 6.0;
    }
    lon1 = 9.0;
    lon2 = lon1;
    trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
    trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
    futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
    lon1 = 21.0;
    lon2 = lon1;
    trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
    trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
    tlon1 = futil->limit(tlon1, -180.0, 180.0);
    tlon2 = futil->limit(tlon2, -180.0, 180.0);
    futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
    lon1 = 33.0;
    lon2 = lon1;
    trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
    trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
    tlon1 = futil->limit(tlon1, -180.0, 180.0);
    tlon2 = futil->limit(tlon2, -180.0, 180.0);
    futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
    lon1 = 42.0;
    while (lon1 < 180.0)
    {
        lon2 = lon1;
        trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
        trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
        tlon1 = futil->limit(tlon1, -180.0, 180.0);
        tlon2 = futil->limit(tlon2, -180.0, 180.0);
        futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                    UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
        lon1 += 6.0;
    }

    // draw the east-west lines
    lat1 = -80.0;
    while (lat1 <= 72.0)
    {
        lon1 = -180.0;
        while (lon1 < 180.0)
        {
            lat2 = lat1;
            lon2 = lon1+6.0;
            trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
            trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
            tlon1 = futil->limit(tlon1, -180.0, 180.0);
            tlon2 = futil->limit(tlon2, -180.0, 180.0);
            futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                    UTIL_LINE_TYPE_RHUMB, TRUE, TRUE);
            lon1 += 6.0;
        }
        lat1 += 8.0;
    }

    // draw the 84 degree lines
    lon1 = -174.0;
    lon2 = -180.0;
    lat1 = 84.0;
    lat2 = lat1;
    while (lon1 <= 180.0)
    {
        trans.convert_datum( lat1, lon1, tlat1, tlon1, datum, "WGS84");
        trans.convert_datum( lat2, lon2, tlat2, tlon2, datum, "WGS84");
        tlon1 = futil->limit(tlon1, -180.0, 180.0);
        tlon2 = futil->limit(tlon2, -180.0, 180.0);
        futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                    m_color, PS_SOLID, m_width+2,
                                UTIL_LINE_TYPE_RHUMB, TRUE, TRUE);
        lon2 = lon1;
        lon1 += 6.0;
    }

    // draw labels
    MapScale mapscale;
    mapscale = map->scale();

    if (mapscale < ONE_TO_80M)
        return;

    if (mapscale < ONE_TO_10M)
    {
        // draw only one label per major zone
        draw_major_zone_labels_center(map, dc);
    }
    else
    {
        // draw one label in each corner of a major zone
        draw_major_zone_labels_corners(map, dc);

    }

}
// end of draw_major_zones

// **********************************************************************
// **********************************************************************

// draw labels of major and minor zones in corners
//int GridOverlay::draw_corner_labels(ActiveMap* map, CDC *dc)
//{
//
//
//}

// **********************************************************************
// **********************************************************************

// calculate the distance from the center of the major zone to the edge
int GridOverlay::edge_of_zone_lat(double clat, double *edge_lat)
{
    double epsilon = 0.0001;
    double dist, ang, tlat, tlon;
    double deg_dist;
    BOOL notdone = TRUE;
    int max_iter = 10;
    int cnt;

    *edge_lat = clat;
    deg_dist = 3.0;
    cnt = 0;
    GEO_geo_to_distance(clat, 0.0, clat, deg_dist, &dist, &ang);
    while (notdone)
    {
        GEO_distance_to_geo( clat, 0.0, dist, 90.0, &tlat, &tlon);
        if (fabs(tlon-3.0) < epsilon)
        {
            notdone = FALSE;
        }
        else
        {
            if (tlon > 3.0)
                dist -= 20.0;
            else
                dist += 20.0;
        }
        // limit the iterations
        cnt++;
        if (cnt > max_iter)
            notdone = FALSE;
    }
    *edge_lat = tlat;

    return SUCCESS;
}
// end of edge_of_zone_lat

// **********************************************************************
// **********************************************************************

// calculate the distance from the center of the major zone to the edge
int GridOverlay::edge_of_zone_lat(double clat, double llz_lon, int width, double *edge_lat_left, double *edge_lat_right)
{
    double epsilon = 0.0001;
    double dist, ang, tlat, tlon;
    double deg_dist;
    BOOL notdone = TRUE;
    int max_iter = 50;
    int cnt;

    *edge_lat_left = clat;
    *edge_lat_right = clat;

    switch (width)
    {
        case 3:
            deg_dist = 3.0;
            cnt = 0;
            GEO_geo_to_distance(clat, 0.0, clat, deg_dist, &dist, &ang);
            while (notdone)
            {
                GEO_distance_to_geo( clat, 0.0, dist, 270.0, &tlat, &tlon);
                if (fabs(tlon+3.0) < epsilon)
                {
                    notdone = FALSE;
                }
                else
                {
                    if (tlon < -3.0)
                        dist -= 20.0;
                    else
                        dist += 20.0;
                }
                // limit the iterations
                cnt++;
                if (cnt > max_iter)
                    notdone = FALSE;
            }
            *edge_lat_left = tlat;
            *edge_lat_right = clat;
            break;

        case 6:
            deg_dist = 3.0;
            cnt = 0;
            GEO_geo_to_distance(clat, 0.0, clat, deg_dist, &dist, &ang);
            while (notdone)
            {
                GEO_distance_to_geo( clat, 0.0, dist, 90.0, &tlat, &tlon);
                if (fabs(tlon-3.0) < epsilon)
                {
                    notdone = FALSE;
                }
                else
                {
                    if (tlon > 3.0)
                        dist -= 20.0;
                    else
                        dist += 20.0;
                }
                // limit the iterations
                cnt++;
                if (cnt > max_iter)
                    notdone = FALSE;
            }
            *edge_lat_left = tlat;
            *edge_lat_right = tlat;
            break;
        case 9:
            if (llz_lon == 0.0)
            {
                deg_dist = 6.0;
                cnt = 0;
                GEO_geo_to_distance(clat, 0.0, clat, deg_dist, &dist, &ang);
                while (notdone)
                {
                    GEO_distance_to_geo( clat, 0.0, dist, 90.0, &tlat, &tlon);
                    if (fabs(tlon-6.0) < epsilon)
                    {
                        notdone = FALSE;
                    }
                    else
                    {
                        if (tlon > 6.0)
                            dist -= 20.0;
                        else
                            dist += 20.0;
                    }
                    // limit the iterations
                    cnt++;
                    if (cnt > max_iter)
                        notdone = FALSE;
                }
                *edge_lat_right = tlat;

                deg_dist = 3.0;
                cnt = 0;
                GEO_geo_to_distance(clat, 0.0, clat, deg_dist, &dist, &ang);
                notdone = TRUE;
                while (notdone)
                {
                    GEO_distance_to_geo( clat, 0.0, dist, 270.0, &tlat, &tlon);
                    if (fabs(tlon+3.0) < epsilon)
                    {
                        notdone = FALSE;
                    }
                    else
                    {
                        if (tlon < -3.0)
                            dist -= 20.0;
                        else
                            dist += 20.0;
                    }
                    // limit the iterations
                    cnt++;
                    if (cnt > max_iter)
                        notdone = FALSE;
                }
                *edge_lat_left = tlat;
            }
            if ((llz_lon == 3.0) || (llz_lon == 33.0))
            {
                deg_dist = 3.0;
                cnt = 0;
                GEO_geo_to_distance(clat, 0.0, clat, deg_dist, &dist, &ang);
                notdone = TRUE;
                while (notdone)
                {
                    GEO_distance_to_geo( clat, 0.0, dist, 90.0, &tlat, &tlon);
                    if (fabs(tlon-3.0) < epsilon)
                    {
                        notdone = FALSE;
                    }
                    else
                    {
                        if (tlon > 3.0)
                            dist -= 20.0;
                        else
                            dist += 20.0;
                    }
                    // limit the iterations
                    cnt++;
                    if (cnt > max_iter)
                        notdone = FALSE;
                }
                *edge_lat_right = tlat;

                deg_dist = 6.0;
                cnt = 0;
                GEO_geo_to_distance(clat, 0.0, clat, deg_dist, &dist, &ang);
                notdone = TRUE;
                while (notdone)
                {
                    GEO_distance_to_geo( clat, 0.0, dist, 270.0, &tlat, &tlon);
                    if (fabs(tlon+6.0) < epsilon)
                    {
                        notdone = FALSE;
                    }
                    else
                    {
                        if (tlon < -6.0)
                            dist -= 20.0;
                        else
                            dist += 20.0;
                    }
                    // limit the iterations
                    cnt++;
                    if (cnt > max_iter)
                        notdone = FALSE;
                }
                *edge_lat_left = tlat;
            }
            break;
        case 12:
            deg_dist = 6.0;
            cnt = 0;
            GEO_geo_to_distance(clat, 0.0, clat, deg_dist, &dist, &ang);
            while (notdone)
            {
                GEO_distance_to_geo( clat, 0.0, dist, 90.0, &tlat, &tlon);
                if (fabs(tlon-6.0) < epsilon)
                {
                    notdone = FALSE;
                }
                else
                {
                    if (tlon > 6.0)
                        dist -= 20.0;
                    else
                        dist += 20.0;
                }
                // limit the iterations
                cnt++;
                if (cnt > max_iter)
                    notdone = FALSE;
            }
            *edge_lat_left = tlat;
            *edge_lat_right = tlat;
            break;
    }

    return SUCCESS;
}
// end of edge_of_zone_lat

// **********************************************************************
// **********************************************************************

BOOL GridOverlay::is_standard_major_grid(char * milgrid)
{
        return TRUE;
}

// **********************************************************************
// **********************************************************************

int GridOverlay::get_major_zone_extents(CString major_zone, d_geo_t *ll, d_geo_t *ur)
{
    int zone;
   const int SLINE_LEN = 5;
    char sline[SLINE_LEN];
    char let;

    if (major_zone.GetLength() != 3)
        return FAILURE;

    strncpy_s(sline, SLINE_LEN, major_zone, 2);
    sline[2] = '\0';
    zone = atoi(sline);
    let = major_zone[2];

    // check for standard zone
    if (major_zone.Compare("31X") && major_zone.Compare("33X") &&
        major_zone.Compare("35X") && !major_zone.Compare("37X") &&
        major_zone.Compare("31V") && !major_zone.Compare("32V"))
    {
        ll->lon = ((double) (zone-1) * 6.0) - 180.0;
        ur->lon = ll->lon + 6.0;
        switch(let)
        {
            case 'C': ll->lat = -80.0; break;
            case 'D': ll->lat = -72.0; break;
            case 'E': ll->lat = -64.0; break;
            case 'F': ll->lat = -56.0; break;
            case 'G': ll->lat = -48.0; break;
            case 'H': ll->lat = -40.0; break;
            case 'J': ll->lat = -32.0; break;
            case 'K': ll->lat = -24.0; break;
            case 'L': ll->lat = -16.0; break;
            case 'M': ll->lat = -8.0; break;
            case 'N': ll->lat = -0.0; break;
            case 'P': ll->lat = -8.0; break;
            case 'Q': ll->lat = 16.0; break;
            case 'R': ll->lat = 24.0; break;
            case 'S': ll->lat = 32.0; break;
            case 'T': ll->lat = 40.0; break;
            case 'U': ll->lat = 48.0; break;
            case 'V': ll->lat = 56.0; break;
            case 'W': ll->lat = 64.0; break;
            case 'X': ll->lat = 72.0; break;
            default: return FAILURE;
        }

        ur->lat = ll->lat + 8.0;
        if (let == 'X')
            ur->lat = 84.0;

        return SUCCESS;
    }

    // non-standard zone
    if (!major_zone.Compare("31V"))
    {
        ll->lat = 56.0;
        ll->lon = 0.0;
        ur->lat = 64.0;
        ur->lon = 3.0;
        return SUCCESS;
    }

    if (!major_zone.Compare("32V"))
    {
        ll->lat = 56.0;
        ll->lon = 3.0;
        ur->lat = 64.0;
        ur->lon = 12.0;
        return SUCCESS;
    }

    if (!major_zone.Compare("31X"))
    {
        ll->lat = 72.0;
        ll->lon = 0.0;
        ur->lat = 84.0;
        ur->lon = 9.0;
        return SUCCESS;
    }

    if (!major_zone.Compare("33X"))
    {
        ll->lat = 72.0;
        ll->lon = 9.0;
        ur->lat = 84.0;
        ur->lon = 21.0;
        return SUCCESS;
    }

    if (!major_zone.Compare("35X"))
    {
        ll->lat = 72.0;
        ll->lon = 21.0;
        ur->lat = 84.0;
        ur->lon = 33.0;
        return SUCCESS;
    }

    if (!major_zone.Compare("37X"))
    {
        ll->lat = 72.0;
        ll->lon = 33.0;
        ur->lat = 84.0;
        ur->lon = 42.0;
        return SUCCESS;
    }

    return FAILURE;
}
// end of get_major_zone_extents

// **********************************************************************
// **********************************************************************


int GridOverlay::get_minor_zone_corners(CString major_zone, CString minor_zone, d_geo_t * bnd)
{
   const int TMILGRID_LEN = 41;
    char tmilgrid[TMILGRID_LEN];
    double grid_lat[4], grid_lon[4];
    double tlat, tlon, tlat2, tlon2;
    int utm_zone, rslt, k;
   const int S_LEN = 21;
    char sll[S_LEN], slr[S_LEN], sul[S_LEN], sur[S_LEN];
    double utm_northing, utm_easting;
   const int DATUM_LEN = 41;
    char datum[DATUM_LEN];
    CGeoTrans trans;

    if (major_zone.GetLength() != 3)
        return FAILURE;

    if (minor_zone.GetLength() != 2)
        return FAILURE;

    GEO_get_default_datum(datum, DATUM_LEN);

    strcpy_s(sll, S_LEN, "0000000000");
    strcpy_s(slr, S_LEN, "9999900000");
    strcpy_s(sul, S_LEN, "0000099999");
    strcpy_s(sur, S_LEN, "9999999999");
    // find the corners of the minor zone
    strcpy_s(tmilgrid, TMILGRID_LEN, major_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, minor_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, sul);
    rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
    if (rslt != SUCCESS)
        ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
    trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
    grid_lat[0] = tlat2;
    grid_lon[0] = tlon2;
    strcpy_s(tmilgrid, TMILGRID_LEN, major_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, minor_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, sur);
    rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
    if (rslt != SUCCESS)
        ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
    trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
    grid_lat[1] = tlat2;
    grid_lon[1] = tlon2;
    strcpy_s(tmilgrid, TMILGRID_LEN, major_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, minor_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, slr);
    rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
    if (rslt != SUCCESS)
        ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
    trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
    grid_lat[2] = tlat2;
    grid_lon[2] = tlon2;
    strcpy_s(tmilgrid, TMILGRID_LEN, major_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, minor_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, sll);
    rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
    if (rslt != SUCCESS)
        ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
    trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
    grid_lat[3] = tlat2;
    grid_lon[3] = tlon2;

    for (k=0; k<4; k++)
    {
        bnd[k].lat = grid_lat[k];
        bnd[k].lon = grid_lon[k];
    }

    return SUCCESS;
}
// end of get_minor_zone_corners

// **********************************************************************
// **********************************************************************

int GridOverlay::get_minor_zone_extents(CString major_zone, CString minor_zone,
                                        CString easting, CString northing, d_geo_t *ll, d_geo_t *ur)
{
   const int TMILGRID_LEN = 41;
    char tmilgrid[TMILGRID_LEN];
    double grid_lat[4], grid_lon[4];
    double tlat, tlon, tlat2, tlon2;
    int utm_zone, rslt, k, len1, len2;
    double utm_northing, utm_easting;
   const int DATUM_LEN = 41;
    char datum[DATUM_LEN];
   const int S_LEN = 21;
    char sll[S_LEN], slr[S_LEN], sul[S_LEN], sur[S_LEN];
    CGeoTrans trans;
    int east, north;

    if (major_zone.GetLength() != 3)
        return FAILURE;

    if (minor_zone.GetLength() != 2)
        return FAILURE;

    GEO_get_default_datum(datum, DATUM_LEN);

    len1 = easting.GetLength();
    len2 = northing.GetLength();

    if ((len1 == 0) || (len2 == 0) || (len1 != len2))
    {
        strcpy_s(sll, S_LEN, "0000000000");
        strcpy_s(slr, S_LEN, "9999900000");
        strcpy_s(sul, S_LEN, "0000099999");
        strcpy_s(sur, S_LEN, "9999999999");
    }
    else
    {
        east = atoi(easting);
        north = atoi(northing);

        strcpy_s(sll, S_LEN, easting);
        strcat_s(sll, S_LEN, "00000");
        sll[5] = '\0';
        strcat_s(sll, S_LEN, northing);
        strcat_s(sll, S_LEN, "00000");
        sll[10] = '\0';

        strcpy_s(slr, S_LEN, easting);
        strcat_s(slr, S_LEN, "99999");
        slr[5] = '\0';
        strcat_s(slr, S_LEN, northing);
        strcat_s(slr, S_LEN, "00000");
        slr[10] = '\0';

        strcpy_s(sul, S_LEN, easting);
        strcat_s(sul, S_LEN, "00000");
        sul[5] = '\0';
        strcat_s(sul, S_LEN, northing);
        strcat_s(sul, S_LEN, "99999");
        sul[10] = '\0';

        strcpy_s(sur, S_LEN, easting);
        strcat_s(sur, S_LEN, "99999");
        sur[5] = '\0';
        strcat_s(sur, S_LEN, northing);
        strcat_s(sur, S_LEN, "99999");
        sur[10] = '\0';
    }

    // find the corners of the minor zone
    strcpy_s(tmilgrid, TMILGRID_LEN, major_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, minor_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, sll);
    rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
    if (rslt != SUCCESS)
        ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
    trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
    grid_lat[0] = tlat2;
    grid_lon[0] = tlon2;
    strcpy_s(tmilgrid, TMILGRID_LEN, major_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, minor_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, slr);
    rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
    if (rslt != SUCCESS)
        ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
    trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
    grid_lat[1] = tlat2;
    grid_lon[1] = tlon2;
    strcpy_s(tmilgrid, TMILGRID_LEN, major_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, minor_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, sur);
    rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
    if (rslt != SUCCESS)
        ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
    trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
    grid_lat[2] = tlat2;
    grid_lon[2] = tlon2;
    strcpy_s(tmilgrid, TMILGRID_LEN, major_zone);
    strcat_s(tmilgrid, TMILGRID_LEN, minor_zone);
    strcat_s(tmilgrid, sul);
    rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
    if (rslt != SUCCESS)
        ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
    trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
    grid_lat[3] = tlat2;
    grid_lon[3] = tlon2;

    ll->lat = grid_lat[0];
    for (k=1; k<4; k++)
        if (grid_lat[k] < ll->lat)
            ll->lat = grid_lat[k];

    ur->lat = grid_lat[0];
    for (k=1; k<4; k++)
        if (grid_lat[k] > ur->lat)
            ur->lat = grid_lat[k];

    ll->lon = grid_lon[0];
    for (k=1; k<4; k++)
        if (GEO_east_of_degrees(ll->lon, grid_lon[k]))
            ll->lon = grid_lon[k];

    ur->lon = grid_lon[0];
    for (k=1; k<4; k++)
        if (GEO_east_of_degrees(grid_lon[k], ur->lon))
            ur->lon = grid_lon[k];

    return SUCCESS;
}
// end of get_minor_zone_extents

// **********************************************************************
// **********************************************************************

BOOL GridOverlay::is_zone_on_screen(ActiveMap* map, CString major_zone, CString minor_zone,
                                               CString easting, CString northing)
{
    d_geo_t ll, ur;
    d_geo_t gll, gur;
    int rslt;

    if (major_zone.GetLength() != 3)
        return FALSE;

    if (minor_zone.GetLength() == 0)
    {
        rslt = get_major_zone_extents(major_zone, &gll, &gur);
    }
    else
    {
        if (minor_zone.GetLength() != 2)
            return FALSE;
        rslt = get_minor_zone_extents(major_zone, minor_zone, easting, northing, &gll, &gur);
    }
    if (rslt != SUCCESS)
        return FALSE;

    // get the map bounds to clip symbol drawing
    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
        return FALSE;


    // check for intersection
    if (!GEO_enclose(ll.lat, ll.lon, ur.lat, ur.lon, gll.lat, gll.lon, gur.lat, gur.lon) &&
        !GEO_intersect(ll.lat, ll.lon, ur.lat, ur.lon, gll.lat, gll.lon, gur.lat, gur.lon))
        return FALSE;

    return TRUE;
}
// end of is_zone_on_screen

// **********************************************************************
// **********************************************************************

int GridOverlay::draw_minor_grid_gratules(ActiveMap* map, CDC *dc, CString major_zone, CString minor_zone)
{
    d_geo_t ll, ur;
    CGeoTrans trans;
    CFvwUtil *futil = CFvwUtil::get_instance();
    double tlat, tlon, map_angle;
    double lat0, lon0, lat2, lon2;
    int utm_zone, tx, ty, rslt;
    double utm_northing, utm_easting;
    int screen_width, screen_height;
   const int SLINE_LEN = 121;
   const int MILGRID_LEN = 81;
   const int TMILGRID_LEN = 121;
    char milgrid[MILGRID_LEN], tmilgrid[TMILGRID_LEN], sline[SLINE_LEN];
    CString easting, northing;
    CString tstr;
   const int DATUM_LEN = 41;
    char datum[41];
    double grid_lat[4], grid_lon[4];
    double grid_ll_lat, grid_ll_lon, grid_ur_lat, grid_ur_lon;
    int x, y, k;
    char xstr[2], ystr[2];
    MapScale mapscale;
    POINT cpt[4];
    double dist, ang, scr_dist_x, scr_dist_y;
    d_geo_t bnd[4];
    BOOL onscreen = FALSE;
    int width;

major_zone = minor_zone.Left(3);
minor_zone = minor_zone.Mid(3, 2);

    // gratule lines are slightless smaller the grid lines
    width = m_width -1;
    if (width < 1)
        width = 1;

    mapscale = map->scale();

    if (mapscale < ONE_TO_1M)
        return SUCCESS;

    if (!is_zone_on_screen(map, major_zone, minor_zone, "", ""))
        return SUCCESS;

    // get the map bounds to clip symbol drawing
    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
        return FAILURE;

    // get the bounds of the minor grid
    rslt = get_minor_zone_corners(major_zone, minor_zone, bnd);

    map->get_surface_size(&screen_width, &screen_height);

    map_angle = map->actual_rotation();

    GEO_get_default_datum(datum, DATUM_LEN);

    // check for one of the odd major grids

    for (k=0; k<4; k++)
    {
        grid_lat[k] = bnd[k].lat;
        grid_lon[k] = bnd[k].lon;
    }

    grid_ll_lat = grid_lat[0];
    for (k=1; k<4; k++)
        if (grid_lat[k] < grid_ll_lat)
            grid_ll_lat = grid_lat[k];

    grid_ur_lat = grid_lat[0];
    for (k=1; k<4; k++)
        if (grid_lat[k] > grid_ur_lat)
            grid_ur_lat = grid_lat[k];

    grid_ll_lon = grid_lon[0];
    for (k=1; k<4; k++)
        if (GEO_east_of_degrees(grid_ll_lon, grid_lon[k]))
            grid_ll_lon = grid_lon[k];

    grid_ur_lon = grid_lon[0];
    for (k=1; k<4; k++)
        if (GEO_east_of_degrees(grid_lon[k], grid_ur_lon))
            grid_ur_lon = grid_lon[k];

    // check for intersection
    if (!GEO_enclose(ll.lat, ll.lon, ur.lat, ur.lon,
                    grid_ll_lat, grid_ll_lon, grid_ur_lat, grid_ur_lon) &&
        !GEO_intersect(ll.lat, ll.lon, ur.lat, ur.lon,
                    grid_ll_lat, grid_ll_lon, grid_ur_lat, grid_ur_lon))
        return FAILURE;

    // compose milgrid
    strcpy_s(milgrid, MILGRID_LEN, major_zone);
    strcat_s(milgrid, MILGRID_LEN, minor_zone);

    // find the screen distance
    GEO_geo_to_distance(ll.lat, ll.lon, ll.lat, ur.lon, &scr_dist_x, &ang);
    scr_dist_x /= 2.0;;
    GEO_geo_to_distance(ll.lat, ll.lon, ur.lat, ll.lon, &scr_dist_y, &ang);
    scr_dist_y /= 2.0;;

    // draw the horizontal lines
    // find the left vertexes
    for (y=1; y<10; y++)
    {
        sprintf_s(sline, SLINE_LEN, "%1d", y);
        strcpy_s(tmilgrid, TMILGRID_LEN, milgrid);
        strcat_s(tmilgrid, TMILGRID_LEN, "00000");
        strcat_s(tmilgrid, TMILGRID_LEN, sline);
        strcat_s(tmilgrid, TMILGRID_LEN, "0000");
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        rslt = trans.convert_datum( tlat, tlon, lat0, lon0, datum, "WGS84");
        ASSERT(rslt == SUCCESS);
        strcpy_s(tmilgrid, TMILGRID_LEN, milgrid);
        strcat_s(tmilgrid, TMILGRID_LEN, "99999");
        strcat_s(tmilgrid, TMILGRID_LEN, sline);
        strcat_s(tmilgrid, TMILGRID_LEN, "0000");
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        rslt = trans.convert_datum( tlat, tlon, lat2, lon2, datum, "WGS84");
        ASSERT(rslt == SUCCESS);
        futil->draw_geo_line(map, dc, lat0, lon0, lat2, lon2, m_color, PS_SOLID, width,
                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
        tstr = sline;

        if (mapscale <= ONE_TO_100K)
        {
            if (map_angle == 0.0)
            {
                // find the intersection witht the left side of the screen
                if (GEO_great_circle_intersection(ll.lat, ll.lon, ur.lat, ll.lon,
                                                lat0, lon0, lat2, lon2, &tlat, &tlon))
                {
                        map->geo_to_surface(tlat, tlon, &tx, &ty);
                        futil->draw_text(dc, tstr, tx+m_font_size/2, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }
            }
            else
            {
                double clat, clon;

                futil->compute_center_geo(ll, ur, &clat, &clon);

                // find the intersection with the center of the screen
                if (GEO_great_circle_intersection(ll.lat, clon, ur.lat, clon,
                                                lat0, lon0, lat2, lon2, &tlat, &tlon))
                {
                        map->geo_to_surface(tlat, tlon, &tx, &ty);
                        futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }
            }
        }



    }


    // find the top vertexes
    for (x=1; x<10; x++)
    {
        sprintf_s(sline, SLINE_LEN, "%1d", x);
        strcpy_s(tmilgrid, TMILGRID_LEN, milgrid);
        strcat_s(tmilgrid, TMILGRID_LEN, sline);
        strcat_s(tmilgrid, TMILGRID_LEN, "0000");
        strcat_s(tmilgrid, TMILGRID_LEN, "99999");
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        rslt = trans.convert_datum( tlat, tlon, lat0, lon0, datum, "WGS84");
        ASSERT(rslt == SUCCESS);
        strcpy_s(tmilgrid, TMILGRID_LEN, milgrid);
        strcat_s(tmilgrid, TMILGRID_LEN, sline);
        strcat_s(tmilgrid, TMILGRID_LEN, "0000");
        strcat_s(tmilgrid, TMILGRID_LEN, "00000");
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        rslt = trans.convert_datum( tlat, tlon, lat2, lon2, datum, "WGS84");
        ASSERT(rslt == SUCCESS);
        futil->draw_geo_line(map, dc, lat0, lon0, lat2, lon2, m_color, PS_SOLID, width,
                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
        GEO_geo_to_distance(lat0, lon0, lat2, lon2, &dist, &ang);
        tstr = sline;

        if (mapscale <= ONE_TO_100K)
        {
            if ((map_angle == 0.0) && !dc->IsPrinting())
            {
                // find the intersection witht the top of the screen
                if (GEO_great_circle_intersection(ur.lat, ll.lon, ur.lat, ur.lon,
                                                        lat0, lon0, lat2, lon2, &tlat, &tlon))
                {
                        map->geo_to_surface(tlat, tlon, &tx, &ty);
                        futil->draw_text(dc, tstr, tx, ty+5+m_font_size/2, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }
            }
            else
            {
                double clat, clon;

                futil->compute_center_geo(ll, ur, &clat, &clon);

                // find the intersection with the center of the screen
                if (GEO_great_circle_intersection(clat, ll.lon, clat, ur.lon,
                                                lat0, lon0, lat2, lon2, &tlat, &tlon))
                {
                        map->geo_to_surface(tlat, tlon, &tx, &ty);
                        futil->draw_text(dc, tstr, tx, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                                    m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }
            }
        }

    }

    if (mapscale < ONE_TO_100K)
        return SUCCESS;

    // draw the 1000 meter lines
    for (y=0; y<10; y++)
    {
        sprintf_s(ystr, 2, "%1d", y);
        northing = ystr;
        for (x=0; x<10; x++)
        {
            sprintf_s(xstr, 2, "%1d", x);
            easting = xstr;
            draw_minor_grid_gratules_1000(map, dc, major_zone, minor_zone, easting, northing);
        }
    }

    return SUCCESS;
}
// end of draw_minor_grid_gratules

// **********************************************************************
// **********************************************************************


int GridOverlay::draw_minor_grid_gratules_1000(ActiveMap* map, CDC *dc, CString major_zone, CString minor_zone,
                                               CString easting, CString northing)
{
    d_geo_t ll, ur;
    CGeoTrans trans;
    CFvwUtil *futil = CFvwUtil::get_instance();
    double tlat, tlon, tlat2, tlon2;
    int utm_zone, rslt;
    double utm_northing, utm_easting;
   const int SLINE_LEN = 41;
   const int TMILGRID_LEN = 81;
    char tmilgrid[TMILGRID_LEN], sline[SLINE_LEN];
    CString mgrs;
    CString major, east, north, east_first, north_first;
    CString tstr;
    MapScale mapscale;
   const int DATUM_LEN = 41;
    char datum[DATUM_LEN];
    double lat_l[10], lat_r[10], lon_l[10], lon_r[10];
    double lat_t[10], lon_t[10], lat_b[10], lon_b[10];
    double ctr_lat, ctr_lon;
    int x, y, tx, ty;
   char xstr[3], ystr[3];
    double dist, ang, scr_dist_x, scr_dist_y;
    double map_angle;
    POINT cpt[4];
    int width;

    // gratule lines are slightless smaller the grid lines
    width = m_width -1;
    if (width < 1)
        width = 1;

    mapscale = map->scale();

    if (mapscale < ONE_TO_100K)
        return FAILURE;

    if (!is_zone_on_screen(map, major_zone, minor_zone, easting, northing))
        return FAILURE;

    // get the map bounds to clip symbol drawing
    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
        return FAILURE;

    map_angle = map->actual_rotation();
    ctr_lat = (ll.lat + ur.lat) / 2.0;
    ctr_lon = (ll.lon + ur.lon) / 2.0;

    GEO_get_default_datum(datum, DATUM_LEN);

    // find the screen distance
    GEO_geo_to_distance(ll.lat, ll.lon, ll.lat, ur.lon, &scr_dist_x, &ang);
    scr_dist_x /= 2.0;;
    GEO_geo_to_distance(ll.lat, ll.lon, ur.lat, ll.lon, &scr_dist_y, &ang);
    scr_dist_y /= 2.0;;

    // draw the horizontal lines
    for (y=0; y<10; y++)
    {
        sprintf_s(sline, SLINE_LEN, "%1d", y);
        mgrs = major_zone + minor_zone;
        mgrs += easting;
        mgrs += "0000";
        mgrs += northing;
        mgrs += sline;
        mgrs += "000";
        strcpy_s(tmilgrid, TMILGRID_LEN, mgrs);
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
        lat_l[y] = tlat2;
        lon_l[y] = tlon2;

        mgrs = major_zone + minor_zone;
        mgrs += easting;
        mgrs += "9999";
        mgrs += northing;
        mgrs += sline;
        mgrs += "000";
        strcpy_s(tmilgrid, TMILGRID_LEN, mgrs);
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
        lat_r[y] = tlat2;
        lon_r[y] = tlon2;
        if (y > 0)
            futil->draw_geo_line(map, dc, lat_l[y], lon_l[y], lat_r[y], lon_r[y], m_color, PS_SOLID, width,
                                UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
        GEO_geo_to_distance(lat_l[y], lon_l[y], lat_r[y], lon_r[y], &dist, &ang);
        tstr = northing;
        tstr += sline;

        if (mapscale <= ONE_TO_10K)
        {
            if (fabs(map_angle) < 0.01)
            {
               // find the intersection witht the left side of the screen
               if (GEO_great_circle_intersection(ll.lat, ll.lon, ur.lat, ll.lon,
                                               lat_l[y], lon_l[y], lat_r[y], lon_r[y], &tlat, &tlon))
               {
                   map->geo_to_surface(tlat, tlon, &tx, &ty);
                   futil->draw_text(dc, tstr, tx+m_font_size/2, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                               m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }
            }
            else
            {
               // find the intersection witht the center of the screen
               if (GEO_great_circle_intersection(ll.lat, ctr_lon, ur.lat, ctr_lon,
                                               lat_l[y], lon_l[y], lat_r[y], lon_r[y], &tlat, &tlon))
               {
                   map->geo_to_surface(tlat, tlon, &tx, &ty);
                   futil->draw_text(dc, tstr, tx+m_font_size/2, ty, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                               m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
                }
            }
        }
    }

    // draw the vertical lines
    for (x=0; x<10; x++)
    {
        sprintf_s(sline, SLINE_LEN, "%1d", x);
        mgrs = major_zone + minor_zone;
        mgrs += easting;
        mgrs += sline;
        mgrs += "000";
        mgrs += northing;
        mgrs += "9999";
        strcpy_s(tmilgrid, TMILGRID_LEN, mgrs);
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
        lat_t[x] = tlat2;
        lon_t[x] = tlon2;

        mgrs = major_zone + minor_zone;
        mgrs += easting;
        mgrs += sline;
        mgrs += "000";
        mgrs += northing;
        mgrs += "0000";
        strcpy_s(tmilgrid, TMILGRID_LEN, mgrs);
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
        lat_b[x] = tlat2;
        lon_b[x] = tlon2;
        if (x > 0)
            futil->draw_geo_line(map, dc, lat_t[x], lon_t[x], lat_b[x], lon_b[x], m_color, PS_SOLID, width,
                                UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
        GEO_geo_to_distance(lat_t[x], lon_t[x], lat_b[x], lon_b[x], &dist, &ang);
        tstr = easting;
        tstr += sline;


        if (mapscale <= ONE_TO_10K)
        {
            if (fabs(map_angle) < 0.01)
            {
               // find the intersection with the top of the screen
               if (GEO_great_circle_intersection(ur.lat, ll.lon, ur.lat, ur.lon,
                                               lat_t[x], lon_t[x], lat_b[x], lon_b[x], &tlat, &tlon))
               {
                   map->geo_to_surface(tlat, tlon, &tx, &ty);
                   futil->draw_text(dc, tstr, tx, ty+5+m_font_size/2, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                               m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
               }
            }
            else
            {
               // find the intersection with the center of the screen
               if (GEO_great_circle_intersection(ctr_lat, ll.lon, ctr_lat, ur.lon,
                                               lat_t[x], lon_t[x], lat_b[x], lon_b[x], &tlat, &tlon))
               {
                   map->geo_to_surface(tlat, tlon, &tx, &ty);
                   futil->draw_text(dc, tstr, tx, ty+5+m_font_size/2, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                               m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
               }
            }
        }
    }

    if (mapscale <= ONE_TO_10K)
        return SUCCESS;

    // draw the 100 meter lines
    for (y=0; y<10; y++)
    {
      sprintf_s(ystr, 3, "%c%1d", northing[0], y);
        CString northing2 = ystr;
        for (x=0; x<10; x++)
        {
            sprintf_s(xstr, 3, "%c%1d", easting[0], x);
            CString easting2 = xstr;
            draw_minor_grid_gratules_100(map, dc, major_zone, minor_zone, easting2, northing2);
        }
    }

    return SUCCESS;
}
// end of draw_minor_grid_gratules_1000

// **********************************************************************
// **********************************************************************

int GridOverlay::draw_minor_grid_gratules_100(ActiveMap* map, CDC *dc, CString major_zone, CString minor_zone,
                                               CString easting, CString northing)
{
    d_geo_t ll, ur;
    CGeoTrans trans;
    CFvwUtil *futil = CFvwUtil::get_instance();
    double tlat, tlon, tlat2, tlon2;
    int utm_zone, rslt;
    double utm_northing, utm_easting, map_angle;
   const int SLINE_LEN = 41;
   const int TMILGRID_LEN = 81;
    char tmilgrid[TMILGRID_LEN], sline[SLINE_LEN];
    CString mgrs;
    CString major, east, north, east_first, north_first;
    CString tstr;
    MapScale mapscale;
   const int DATUM_LEN = 41;
    char datum[DATUM_LEN];
    double lat_l[10], lat_r[10], lon_l[10], lon_r[10];
    double lat_t[10], lon_t[10], lat_b[10], lon_b[10];
    double ctr_lat, ctr_lon;
    int x, y, tx, ty;
   double dist, ang, scr_dist_x, scr_dist_y;
    POINT cpt[4];
    int width;

    map_angle = map->actual_rotation();

    // gratule lines are slightless smaller the grid lines
    width = m_width -1;
    if (width < 1)
        width = 1;

    mapscale = map->scale();

    if (!is_zone_on_screen(map, major_zone, minor_zone, easting, northing))
        return FAILURE;

    // get the map bounds to clip symbol drawing
    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
        return FAILURE;

    map_angle = map->actual_rotation();
    ctr_lat = (ll.lat + ur.lat) / 2.0;
    ctr_lon = (ll.lon + ur.lon) / 2.0;

    GEO_get_default_datum(datum, DATUM_LEN);

    // find the screen distance
    GEO_geo_to_distance(ll.lat, ll.lon, ll.lat, ur.lon, &scr_dist_x, &ang);
    scr_dist_x /= 2.0;;
    GEO_geo_to_distance(ll.lat, ll.lon, ur.lat, ll.lon, &scr_dist_y, &ang);
    scr_dist_y /= 2.0;;

    // draw the horizontal lines
    for (y=0; y<10; y++)
    {
        sprintf_s(sline, SLINE_LEN, "%1d", y);
        mgrs = major_zone + minor_zone;
        mgrs += easting;
        mgrs += "000";
        mgrs += northing;
        mgrs += sline;
        mgrs += "00";
        strcpy_s(tmilgrid, TMILGRID_LEN, mgrs);
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
        lat_l[y] = tlat2;
        lon_l[y] = tlon2;

        mgrs = major_zone + minor_zone;
        mgrs += easting;
        mgrs += "999";
        mgrs += northing;
        mgrs += sline;
        mgrs += "00";
        strcpy_s(tmilgrid, TMILGRID_LEN, mgrs);
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
        lat_r[y] = tlat2;
        lon_r[y] = tlon2;
        if (y > 0)
            futil->draw_geo_line(map, dc, lat_l[y], lon_l[y], lat_r[y], lon_r[y], m_color, PS_SOLID, width,
                                UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
        GEO_geo_to_distance(lat_l[y], lon_l[y], lat_r[y], lon_r[y], &dist, &ang);
        tstr = northing;
        tstr += sline;

        if (fabs(map_angle) < 0.01)
        {
            // find the intersection witht the left side of the screen
            if (GEO_great_circle_intersection(ll.lat, ll.lon, ur.lat, ll.lon,
                                            lat_l[y], lon_l[y], lat_r[y], lon_r[y], &tlat, &tlon))
            {
                    map->geo_to_surface(tlat, tlon, &tx, &ty);
                    futil->draw_text(dc, tstr, tx+m_font_size/2, ty, UTIL_ANCHOR_CENTER_LEFT, m_font_name, m_font_size,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }
        else
        {
            // find the intersection with the center of the screen
            if (GEO_great_circle_intersection(ll.lat, ctr_lon, ur.lat, ctr_lon,
                                            lat_l[y], lon_l[y], lat_r[y], lon_r[y], &tlat, &tlon))
            {
                    map->geo_to_surface(tlat, tlon, &tx, &ty);
                    futil->draw_text(dc, tstr, tx+m_font_size/2, ty, UTIL_ANCHOR_CENTER_LEFT, m_font_name, m_font_size,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }

    }

    // draw the vertical lines
    for (x=0; x<10; x++)
    {
        sprintf_s(sline, SLINE_LEN, "%1d", x);
        mgrs = major_zone + minor_zone;
        mgrs += easting;
        mgrs += sline;
        mgrs += "00";
        mgrs += northing;
        mgrs += "999";
        strcpy_s(tmilgrid, TMILGRID_LEN, mgrs);
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
        lat_t[x] = tlat2;
        lon_t[x] = tlon2;

        mgrs = major_zone + minor_zone;
        mgrs += easting;
        mgrs += sline;
        mgrs += "00";
        mgrs += northing;
        mgrs += "000";
        strcpy_s(tmilgrid, TMILGRID_LEN, mgrs);
        rslt = trans.convert_milgrid( tmilgrid, datum, tlat, tlon, utm_zone, utm_northing, utm_easting);
        if (rslt != SUCCESS)
            ASSERT(rslt == GEOTRANS_OUTPUT_WARNING);
        trans.convert_datum( tlat, tlon, tlat2, tlon2, datum, "WGS84");
        lat_b[x] = tlat2;
        lon_b[x] = tlon2;
        if (x > 0)
            futil->draw_geo_line(map, dc, lat_t[x], lon_t[x], lat_b[x], lon_b[x], m_color, PS_SOLID, width,
                                UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
        GEO_geo_to_distance(lat_t[x], lon_t[x], lat_b[x], lon_b[x], &dist, &ang);
        tstr = easting;
        tstr += sline;

        
        if (fabs(map_angle) < 0.01)
        {
            // find the intersection with the top of the screen
            if (GEO_great_circle_intersection(ur.lat, ll.lon, ur.lat, ur.lon,
                                            lat_t[x], lon_t[x], lat_b[x], lon_b[x], &tlat, &tlon))
            {
                    map->geo_to_surface(tlat, tlon, &tx, &ty);
                    futil->draw_text(dc, tstr, tx, ty+5+m_font_size/2, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }
        else
        {
            // find the intersection with the center of the screen
            if (GEO_great_circle_intersection(ctr_lat, ll.lon, ctr_lat, ur.lon,
                                            lat_t[x], lon_t[x], lat_b[x], lon_b[x], &tlat, &tlon))
            {
                    map->geo_to_surface(tlat, tlon, &tx, &ty);
                    futil->draw_text(dc, tstr, tx, ty+5+m_font_size/2, UTIL_ANCHOR_CENTER_CENTER, m_font_name, m_font_size,
                                m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);
            }
        }

    }

    return SUCCESS;
}
// end of draw_minor_grid_gratules_100

// **********************************************************************
// **********************************************************************

int GridOverlay::draw_minor_grid_labels(ActiveMap* map, CDC *dc, CString major_zone, CString minor_zone)
{
    d_geo_t ll, ur;
    CGeoTrans trans;
    CFvwUtil *futil = CFvwUtil::get_instance();
    double map_angle;
    int tx, ty, tx2, ty2, rslt;
    int screen_width, screen_height;
    CString tstr;
    POINT cpt[4];
    MapScale mapscale;
   const int DATUM_LEN = 41;
    char datum[41];
    d_geo_t bnd[4];
    BOOL onscreen = FALSE;

major_zone = minor_zone.Left(3);
minor_zone = minor_zone.Mid(3, 2);

    mapscale = map->scale();

    if (mapscale < ONE_TO_5M)
        return SUCCESS;

    if (!is_zone_on_screen(map, major_zone, minor_zone, "", ""))
        return SUCCESS;

    // get the map bounds to clip symbol drawing
    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
        return FAILURE;

    // get the bounds of the minor grid
    rslt = get_minor_zone_corners(major_zone, minor_zone, bnd);
    if (rslt != SUCCESS)
        return FAILURE;

    map->get_surface_size(&screen_width, &screen_height);

    map_angle = map->actual_rotation();

    GEO_get_default_datum(datum, DATUM_LEN);

    // draw the label in the upper left corner
    map->geo_to_surface(bnd[0].lat, bnd[0].lon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        onscreen = TRUE;
        futil->rotate_pt(tx+5, ty+5, &tx2, &ty2, map_angle, tx, ty);
        futil->draw_text(dc, minor_zone, tx2, ty2, UTIL_ANCHOR_UPPER_LEFT, m_font_name, m_font_size,
                        m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);

    }

    // draw the label in the upper right corner
    map->geo_to_surface(bnd[1].lat, bnd[1].lon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        onscreen = TRUE;
        futil->rotate_pt(tx-5, ty+5, &tx2, &ty2, map_angle, tx, ty);
        futil->draw_text(dc, minor_zone, tx2, ty2, UTIL_ANCHOR_UPPER_RIGHT, m_font_name, m_font_size,
                        m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);

    }

    // draw the label in the lower right corner
    map->geo_to_surface(bnd[2].lat, bnd[2].lon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        onscreen = TRUE;
        futil->rotate_pt(tx-5, ty-5, &tx2, &ty2, map_angle, tx, ty);
        futil->draw_text(dc, minor_zone, tx2, ty2, UTIL_ANCHOR_LOWER_RIGHT, m_font_name, m_font_size,
                        m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);

    }

    // draw the label in the lower right corner
    map->geo_to_surface(bnd[3].lat, bnd[3].lon, &tx, &ty);
    if ((tx > -30) && (tx < screen_width + 30) &&
        (ty > -30) && (ty < screen_height + 30))
    {
        onscreen = TRUE;
        futil->rotate_pt(tx+5, ty-5, &tx2, &ty2, map_angle, tx, ty);
        futil->draw_text(dc, minor_zone, tx2, ty2, UTIL_ANCHOR_LOWER_LEFT, m_font_name, m_font_size,
                        m_font_attrib, m_font_background, m_font_color, m_font_bg_color, map_angle, cpt, FALSE);

    }

    return SUCCESS;
}
// end of draw_minor_grid_labels

// **********************************************************************
// **********************************************************************

// Draw the minor grid overlay.
int GridOverlay::draw_minor_grids_in_3_degree_zone(ActiveMap* map, CDC *dc, char* datum,
                                                   double llz_lat, double llz_lon)
{
    double clon, urz_lat, urz_lon;
    int rslt;

    clon = llz_lon + 3;
    urz_lat = llz_lat + 8.0;
    urz_lon = llz_lon + 3.0;

    rslt = draw_minor_grids_in_a_major_zone(map, dc, datum, clon, llz_lat, llz_lon, urz_lat, urz_lon);

    return rslt;
}
// end of draw_minor_grids_in_3_degree_zone

// **********************************************************************
// **********************************************************************

// Draw the minor grid overlay.
int GridOverlay::draw_minor_grids_in_6_degree_zone(ActiveMap* map, CDC *dc, char* datum,
                                                   double llz_lat, double llz_lon)
{
    double clon, urz_lat, urz_lon;
    int rslt;

    clon = llz_lon + 3;
    if (clon > 180.0)
        clon -= 360.0;
    urz_lat = llz_lat + 8.0;
    urz_lon = llz_lon + 6.0;
    if (urz_lon > 180.0)
    {
        urz_lon -= 360.0;
        llz_lon -= 360.0;
    }

    if (urz_lat == 80.0)
        urz_lat = 84.0;

    rslt = draw_minor_grids_in_a_major_zone(map, dc, datum, clon, llz_lat, llz_lon, urz_lat, urz_lon);

    return rslt;
}
// end of draw_minor_grids_in_6_degree_zone

// **********************************************************************
// **********************************************************************

// Draw the minor grid overlay.
int GridOverlay::draw_minor_grids_in_9_degree_zone(ActiveMap* map, CDC *dc, char* datum,
                                                   double llz_lat, double llz_lon)
{
    double clon, urz_lat, urz_lon;
    int rslt;

    if (llz_lon == 0.0)
        clon = 3.0;
    else if (llz_lon == 3.0)
        clon = 9.0;
    else if (llz_lon == 33.0)
        clon = 39.0;
    else
        return FAILURE;

    urz_lat = llz_lat + 8.0;
    urz_lon = llz_lon + 9.0;

    if (urz_lat == 80.0)
        urz_lat = 84.0;

    rslt = draw_minor_grids_in_a_major_zone(map, dc, datum, clon, llz_lat, llz_lon, urz_lat, urz_lon);

    return rslt;
}
// end of draw_minor_grids_in_9_degree_zone

// **********************************************************************
// **********************************************************************

// Draw the minor grid overlay.
int GridOverlay::draw_minor_grids_in_12_degree_zone(ActiveMap* map, CDC *dc, char* datum,
                                                    double llz_lat, double llz_lon)
{
    double clon, urz_lat, urz_lon;
    int rslt;

    clon = llz_lon + 6;
    urz_lat = llz_lat + 8.0;
    urz_lon = llz_lon + 12.0;

    if (urz_lat == 80.0)
        urz_lat = 84.0;

    rslt = draw_minor_grids_in_a_major_zone(map, dc, datum, clon, llz_lat, llz_lon, urz_lat, urz_lon);

    return rslt;
}
// end of draw_minor_grids_in_12_degree_zone

// **********************************************************************
// **********************************************************************

// Draw the minor grid overlay.
int GridOverlay::draw_minor_grids_in_a_major_zone(ActiveMap* map, CDC *dc, char* datum, double clon,
                                                   double llz_lat, double llz_lon,
                                                   double urz_lat, double urz_lon)
{
    double toplat;
    int rslt;
   const int MILGRID_LEN = 81;
    char milgrid[MILGRID_LEN];
    double tnorth, teast;
    int tzone, tx, ty;
    CRect rc;
    CPen pen;
    int px[4], py[4];  // "corners" of the zone
    int nsavedc;
    double clat[200];
    int lat_cnt;
    double lat_l[6][100], lat_r[6][100];
    double lon_l[6][100], lon_r[6][100];
    CGeoTrans trans;
    CFvwUtil *futil = CFvwUtil::get_instance();
    d_geo_t ll, ur;
    int szone, leftmax, rightmax;
    int j, k, cnt1, cnt2;
    double dist, ang;
    double northing;
    double lat1, lon1, tlat, tlon, lat0, lon0, lat2, lon2;
    double tlat1, tlon1, tlat2, tlon2;
    double center_lat, center_lon;
    int start, end, rcnt, width;
    double edge_lat_left, edge_lat_right;
    double left_off, right_off;
    CList<CString*, CString*>  minlist;
    CString *txt, tstr, majzone, lastone;
    POSITION next;
    BOOL found;
    double latinc, loninc, latdif, londif;
    double latdist, londist, latdistinc, londistinc;
    int lwidth;
    double screen_width_meters;

    // gratule lines are slightless smaller the grid lines
    lwidth = m_width -1;
    if (lwidth < 1)
        lwidth = 1;


    // get the map bounds to clip symbol drawing
    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
        return FAILURE;

    GEO_geo_to_distance(ll.lat, ll.lon, ll.lat, ur.lon, &screen_width_meters, &ang);

    // check for intersection
    if (!GEO_enclose(ll.lat, ll.lon, ur.lat, ur.lon, llz_lat, llz_lon, urz_lat, urz_lon) &&
        !GEO_intersect(ll.lat, ll.lon, ur.lat, ur.lon, llz_lat, llz_lon, urz_lat, urz_lon))
        return FAILURE;


    // find the max number of zones to left or right of center
    if (llz_lat <= 0)
        GEO_geo_to_distance(llz_lat, llz_lon, llz_lat, clon, &dist, &ang);
    else
        GEO_geo_to_distance(urz_lat, llz_lon, urz_lat, clon, &dist, &ang);

    dist += 50000.0;  // small fudge

    leftmax = (int) dist / 100000;
    leftmax += 2;

    if (clon == llz_lon)
        leftmax = 0;

    if (llz_lat <= 0)
        GEO_geo_to_distance(llz_lat, clon, llz_lat, urz_lon, &dist, &ang);
    else
        GEO_geo_to_distance(urz_lat, clon, urz_lat, urz_lon, &dist, &ang);

    dist += 50000.0;  // small fudge

    rightmax = (int) dist / 100000;
    rightmax += 2;

    if (clon == urz_lon)
        rightmax = 0;

    left_off = clon - llz_lon;
    right_off = urz_lon - clon;
    width = (int) (urz_lon - llz_lon);

    toplat = urz_lat;
    if (toplat == 80.0)
        toplat = 84.0;

    if (toplat == 0.0)
        toplat = -0.0001;

    // get the center lat and lon
    int screen_width, screen_height;
    map->get_surface_size(&screen_width, &screen_height);

    map->surface_to_geo(screen_width/2, screen_height/2, &center_lat, &center_lon);

    // set clip path so that we draw only in the major zone
    map->geo_to_surface(toplat, llz_lon, &tx, &ty);
    px[0] = tx;
    py[0] = ty;
    map->geo_to_surface(toplat, urz_lon, &tx, &ty);
    px[1] = tx;
    py[1] = ty;
    map->geo_to_surface(llz_lat, urz_lon, &tx, &ty);
    px[2] = tx;
    py[2] = ty;
    map->geo_to_surface(llz_lat, llz_lon, &tx, &ty);
    px[3] = tx;
    py[3] = ty;


    nsavedc = dc->SaveDC();

    pen.CreateStockObject(NULL_PEN);
    dc->SelectObject(&pen);
    dc->BeginPath();
    if (map->actual_projection() == EQUALARC_PROJECTION)
    {
        dc->MoveTo(px[0], py[0]);
        dc->LineTo(px[1], py[1]);
        dc->LineTo(px[2], py[2]);
        dc->LineTo(px[3], py[3]);
        dc->LineTo(px[0], py[0]);
    }
    else
    {
        // draw it in more detail

        // TODO use geo line routine to get many points along lines and draw polygon

        // at the moment use simple case
        dc->MoveTo(px[0], py[0]);
        dc->LineTo(px[1], py[1]);
        dc->LineTo(px[2], py[2]);
        dc->LineTo(px[3], py[3]);
        dc->LineTo(px[0], py[0]);
    }
    dc->EndPath();

    if (!dc->IsPrinting())
        dc->SelectClipPath(RGN_COPY);
    else
        dc->SelectClipPath(RGN_AND);


    // get the zone
    rslt = trans.convert_geo( llz_lat + 1.0, llz_lon+1.0, datum, szone, tnorth, teast, milgrid, MILGRID_LEN );
    if (rslt != SUCCESS)
        return FAILURE;

    rslt = trans.convert_geo( llz_lat, clon, datum, tzone, tnorth, teast, milgrid, MILGRID_LEN );
    if (rslt != SUCCESS)
        return FAILURE;
    if (llz_lat < 0)
        tnorth += 10000000.0;
    start = (int) (tnorth / 100000.0);
    rslt = trans.convert_geo( toplat, clon, datum, tzone, tnorth, teast, milgrid, MILGRID_LEN );
    if (rslt != SUCCESS)
        return FAILURE;
    if (toplat <= 0.0)
        tnorth += 10000000.0;
    end = (int) (tnorth / 100000.0);

    if (start > end)
    {
        k = start;
        start = end;
        end = k;
    }
    start -= 2;
    if (start < 0)
        start = 0;
    end += 2;

    lat_cnt = 0;

    rcnt = 0;

    // draw the horizontal lines and find the lats for the zone in the north
    for (j=start; j<end; j++)
    {
        rcnt++;
        // check for interrupt
        if ((rcnt % 10) == 0)
        {
            if (FVW_is_draw_interrupted())
            {
                if (nsavedc != 0)
                    dc->RestoreDC(nsavedc);
                return FAILURE;
            }
        }

        northing = (double) j* 100000.0;
        if (toplat <= 0.0)
            northing -= 10000000.0;

        rslt = trans.convert_utm(szone, northing, 500000.0, datum, lat0, lon0, milgrid, MILGRID_LEN);
        if (rslt != SUCCESS)
            if (rslt != INVALID_MILGRID_VALUE)
                continue;

        rslt = trans.convert_datum( lat0, lon0, lat1, lon1, datum, "WGS84");
        if (rslt != SUCCESS)
            continue;

        clat[lat_cnt] = lat0;
        if (lat1 < 84.5)//maximum UTM latitude
        {
            if (screen_width_meters < 30000.0)
            {
                rslt = trans.convert_geo( center_lat, center_lon, datum, tzone, tnorth, teast, milgrid, MILGRID_LEN );
                if (rslt != SUCCESS)
                    continue;
                rslt = trans.convert_utm(tzone, northing, teast-screen_width_meters, datum, lat0, lon0, milgrid, MILGRID_LEN);
                if (rslt != SUCCESS)
                    continue;
                rslt = trans.convert_utm(tzone, northing, teast, datum, lat1, lon1, milgrid, MILGRID_LEN);
                if (rslt != SUCCESS)
                    continue;
                rslt = trans.convert_utm(tzone, northing, teast+screen_width_meters, datum, lat2, lon2, milgrid, MILGRID_LEN);
                if (rslt != SUCCESS)
                    continue;

                rslt = trans.convert_datum( lat0, lon0, tlat, tlon, datum, "WGS84");
                if (rslt != SUCCESS)
                    continue;
                lat0 = tlat;
                lon0 = tlon;
                rslt = trans.convert_datum( lat1, lon1, tlat, tlon, datum, "WGS84");
                if (rslt != SUCCESS)
                    continue;
                lat1 = tlat;
                lon1 = tlon;
                rslt = trans.convert_datum( lat2, lon2, tlat, tlon, datum, "WGS84");
                if (rslt != SUCCESS)
                    continue;
                lat2 = tlat;
                lon2 = tlon;

                futil->draw_geo_line(map, dc, lat1, lon1, lat0, lon0,
                                            UTIL_COLOR_BLACK, PS_SOLID, lwidth + 2,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
                futil->draw_geo_line(map, dc, lat1, lon1, lat2, lon2,
                                            UTIL_COLOR_BLACK, PS_SOLID, lwidth + 2,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
                futil->draw_geo_line(map, dc, lat1, lon1, lat0, lon0,
                                            m_color, PS_SOLID, lwidth,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
                futil->draw_geo_line(map, dc, lat1, lon1, lat2, lon2,
                                            m_color, PS_SOLID, lwidth,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
            }
            else
            {
                edge_of_zone_lat(lat0, llz_lon, width, &edge_lat_left, &edge_lat_right);
                // left
                rslt = trans.convert_datum( edge_lat_left, lon0-left_off, tlat, tlon, datum, "WGS84");
                if (rslt != SUCCESS)
                    if (rslt != INVALID_MILGRID_VALUE)
                        continue;
                futil->draw_geo_line(map, dc, lat1, lon1, tlat, tlon,
                                            UTIL_COLOR_BLACK, PS_SOLID, lwidth + 2,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
                futil->draw_geo_line(map, dc, lat1, lon1, tlat, tlon,
                                            m_color, PS_SOLID, lwidth,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);

// test
//futil->draw_geo_line(map, dc, lat1, lon1, tlat, tlon,
//    UTIL_COLOR_RED, PS_SOLID, 3,
//    UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);

                // right
                rslt = trans.convert_datum( edge_lat_right, lon0+right_off, tlat, tlon, datum, "WGS84");
                if (rslt != SUCCESS)
                    if (rslt != INVALID_MILGRID_VALUE)
                        continue;
                futil->draw_geo_line(map, dc, lat1, lon1, tlat, tlon,
                                            UTIL_COLOR_BLACK, PS_SOLID, lwidth + 2,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
                futil->draw_geo_line(map, dc, lat1, lon1, tlat, tlon,
                                            m_color, PS_SOLID, lwidth,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
            }
        }
        else
            j = 150;
        lat_cnt++;
    }

    ASSERT(lat_cnt < 25);

    // find the extents of the screen in the north
    lat1 = clat[0];
    k = 0;
    while ((clat[k] < ll.lat) && (k < lat_cnt))
        k++;
    cnt1 = k;
    while ((clat[k] < ur.lat) && (k < lat_cnt))
        k++;
    cnt2 = k;

    // init the array for north
    if (cnt1 < 1)
        cnt1 = 1;
    for (k=0; k<lat_cnt; k++)
    {
        lat_r[0][k] = clat[k];
        lat_l[0][k] = clat[k];
        lon_r[0][k] = clon;
        lon_l[0][k] = clon;
    }

    // find the position of the nodes for drawing vertical lines in the north
    for (j=0; j<leftmax; j++)
    {
        for (k=0; k<lat_cnt; k++)
        {
            rslt = trans.convert_geo( clat[k], clon, datum, tzone, tnorth, teast, milgrid, MILGRID_LEN );
            if (rslt != SUCCESS)
                if (rslt != INVALID_MILGRID_VALUE)
                    continue;
            teast = 500000.0 - ((double) j * 100000.0);
            rslt = trans.convert_utm(szone, tnorth, teast , datum, lat0, lon0, milgrid, MILGRID_LEN);
            if (rslt != SUCCESS)
                if (rslt != INVALID_MILGRID_VALUE)
                    continue;
            lat_l[j][k] = lat0;
            lon_l[j][k] = futil->limit(lon0, -180.0, 180.);
        }
    }
    for (j=0; j<rightmax; j++)
    {
        for (k=0; k<lat_cnt; k++)
        {
            rslt = trans.convert_geo( clat[k], clon, datum, tzone, tnorth, teast, milgrid, MILGRID_LEN );
            if (rslt != SUCCESS)
                if (rslt != INVALID_MILGRID_VALUE)
                    continue;
            teast = 500000.0 + ((double) j * 100000.0);
            rslt = trans.convert_utm(szone, tnorth, teast , datum, lat0, lon0, milgrid, MILGRID_LEN);
            if (rslt != SUCCESS)
                if (rslt != INVALID_MILGRID_VALUE)
                    continue;
            lat_r[j][k] = lat0;
            lon_r[j][k] = futil->limit(lon0, -180.0, 180.);
        }
    }

    // draw the vertical lines in the north
    for (j=0; j<leftmax-1; j++)
    {
        for (k=1; k<lat_cnt; k++)
        {
            rslt = trans.convert_datum( lat_l[j][k-1], lon_l[j][k-1], tlat1, tlon1, datum, "WGS84");
            if (rslt != SUCCESS)
                if (rslt != INVALID_MILGRID_VALUE)
                    continue;
            rslt = trans.convert_datum( lat_l[j][k], lon_l[j][k], tlat2, tlon2, datum, "WGS84");
            if (rslt != SUCCESS)
                if (rslt != INVALID_MILGRID_VALUE)
                    continue;
            tlon1 = futil->limit(tlon1, -180.0, 180.0);
            tlon2 = futil->limit(tlon2, -180.0, 180.0);
            if (GEO_line_test_degrees(ll.lat, ll.lon, ur.lat, ur.lon, tlat1, tlon1, tlat2, tlon2))
            {
                futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                            UTIL_COLOR_BLACK, PS_SOLID, lwidth + 2,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
                futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                            m_color, PS_SOLID, lwidth,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
            }
        }
    }
    for (j=0; j<rightmax-1; j++)
    {
        for (k=1; k<lat_cnt; k++)
        {
            rslt = trans.convert_datum( lat_r[j][k-1], lon_r[j][k-1], tlat1, tlon1, datum, "WGS84");
            if (rslt != SUCCESS)
                continue;
            rslt = trans.convert_datum( lat_r[j][k], lon_r[j][k], tlat2, tlon2, datum, "WGS84");
            if (rslt != SUCCESS)
                continue;
            tlon1 = futil->limit(tlon1, -180.0, 180.0);
            tlon2 = futil->limit(tlon2, -180.0, 180.0);
            if (GEO_line_test_degrees(ll.lat, ll.lon, ur.lat, ur.lon, tlat1, tlon1, tlat2, tlon2))
            {
                futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                            UTIL_COLOR_BLACK, PS_SOLID, lwidth + 2,
                                            UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
                futil->draw_geo_line(map, dc, tlat1, tlon1, tlat2, tlon2,
                                        m_color, PS_SOLID, lwidth,
                                        UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
            }
        }
    }

    // find this major zone
    tlat = (llz_lat + urz_lat) / 2.0;
    tlon = (llz_lon + urz_lon) / 2.0;
    rslt = trans.convert_geo( tlat, tlon, datum, tzone, tnorth, teast, milgrid, MILGRID_LEN );
    if (rslt != SUCCESS)
        return FAILURE;
    majzone = milgrid;
    majzone = majzone.Left(3);

    // make a list of the minor zones within the major zone
    latdif = urz_lat - llz_lat;
    londif = urz_lon - llz_lon;
    if (urz_lat <= 0.0)
        GEO_geo_to_distance(urz_lat, llz_lon, urz_lat, urz_lon, &dist, &ang);
    else
        GEO_geo_to_distance(llz_lat, llz_lon, llz_lat, urz_lon, &dist, &ang);
    londist = dist;
    GEO_geo_to_distance(llz_lat, llz_lon, urz_lat, llz_lon, &dist, &ang);
    latdist = dist;
    latdistinc = latdist / 50000.0;
    latinc = latdif / latdistinc;
    londistinc = londist / 50000.0;
    loninc = londif / londistinc;

    tlat = llz_lat;
    latinc /= 4.0;
    loninc /= 4.0;
    lastone = "";
    while (tlat < urz_lat + latinc)
    {
        tlon = llz_lon;
        while (tlon < urz_lon)
        {
            rslt = trans.convert_geo( tlat, tlon, datum, tzone, tnorth, teast, milgrid, MILGRID_LEN );
            if (rslt != SUCCESS)
                continue;
            tstr = milgrid;
            tstr = tstr.Left(5);
            if (tstr.Compare(lastone))
            {
                // see if the string is already in list
                found = FALSE;
                next = minlist.GetHeadPosition();
                while (next != NULL)
                {
                    txt = minlist.GetNext(next);
                    if (!txt->Compare(tstr))
                    {
                        found = TRUE;
                        next = NULL;
                    }
                }
                if (!found)
                {
                    txt = new CString;
                    *txt = tstr;
                    minlist.AddTail(txt);
                }
            }
            lastone = tstr;
            tlon += loninc;
        }
        tlat += latinc;
    }
    // draw the labels and gratules for each minor zone on the screen
    while (!minlist.IsEmpty())
    {
        txt = minlist.RemoveHead();
        draw_minor_grid_labels(map, dc, majzone, *txt);
        draw_minor_grid_gratules(map, dc, majzone, *txt);
        delete txt;
    }

    if (nsavedc != 0)
        dc->RestoreDC(nsavedc);

    return SUCCESS;
}
// end of draw_minor_grids_in_a_major_zone

// **********************************************************************
// **********************************************************************

// Draw the minor grid overlay.
int GridOverlay::draw_minor_grids(ActiveMap* map, CDC *dc)
{
    d_geo_t ll, ur;
    CGeoTrans trans;
   const int DATUM_LEN = 101;
    char datum[DATUM_LEN];
   const int DATUM_TYPE_LEN = 21;
    char datum_type[DATUM_TYPE_LEN];
    int x, y, tx;
    int xstart, xend, ystart, yend;
    MapScale mapscale;

    mapscale = map->scale();

    if (mapscale < ONE_TO_20M)
        return FAILURE;


    // get the map bounds to clip symbol drawing
    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
        return FAILURE;

    GEO_get_default_display(datum_type, DATUM_TYPE_LEN);
    if (!strcmp(datum_type, "SECONDARY"))
        GEO_get_secondary_datum(datum, DATUM_LEN);
    else
        GEO_get_primary_datum(datum, DATUM_LEN);

    // draw the grids below N56
    xstart = (int) ll.lon / 6;
    xstart *= 6;
    xend = (int) ur.lon / 6;
    xend *= 6;
    ystart = (int) ll.lat / 8;
    ystart *= 8;
    yend = (int) ur.lat / 8;
    yend *= 8;

    if (ll.lon < 0.0)
        xstart -= 6;

    if (ur.lon > 0.0)
        xend += 6;

    if ((xend - xstart) < 6)
        xend += 6;

    if (ll.lat < 0.0)
        ystart -= 8;

    if (ur.lat > 0.0)
        yend += 8;

    if ((yend - ystart) < 8)
        yend += 8;

    if (xstart < -180)
        xstart = -180;
    if (xend > 180)
        xend = 180;
    if (ystart < -72)
        ystart = -72;
    if (yend > 56)
        yend = 56;

    if (xend < xstart)
        xend += 360;
    if (yend < ystart)
        ystart = yend;

    if (ll.lat < 56.0)
    {
        for (y=ystart; y<=yend; y+=8)
            for (x=xstart; x<=xend; x+=6)
            {
                if ((x != 9) && (y != 56))
                    draw_minor_grids_in_6_degree_zone(map, dc, datum, (double) y, (double) x);
            }
    }

    // draw the grids between 56 and 64
    if ((ll.lat < 64.0) && (ur.lat > 56.0))
    {
        xstart = (int) ll.lon / 6;
        xstart *= 6;
        xend = (int) ur.lon / 6;
        xend *= 6;
        xstart -= 6;
        xend += 6;
        if (xstart < -180)
            xstart = -180;

        if (xend < xstart)
            xend += 360;

        for (x=xstart; x<xend; x+=6)
        {
            tx = x;
            if (tx >= 180)
                tx -= 360;
            if ((tx != 0) && (tx != 6)) // do draw 32V
                draw_minor_grids_in_6_degree_zone(map, dc, datum, 56.0, (double) tx);
        }

        if ( !(ur.lon < 0.0) && !(ll.lon > 3.0) )
        {
            // draw zone 31V
            draw_minor_grids_in_3_degree_zone(map, dc, datum, 56.0, 0.0);
        }

        if ( !(ur.lon < 3.0) && !(ll.lon > 6.0) )
        {
            // draw zone 32V
            draw_minor_grids_in_9_degree_zone(map, dc, datum, 56.0, 3.0);
        }
    }

    // draw the grids between 64 and 72
    if ( !(ll.lat > 72.0) && !(ur.lat < 64.0) )
    {
        for (x=xstart; x<xend; x+=6)
        {
            tx = x;
            if (tx >= 180)
                tx -= 360;
            draw_minor_grids_in_6_degree_zone(map, dc, datum, 64.0, (double) tx);
        }
    }

    // draw the grids above 72
    if ( !(ur.lat < 72.0) )
    {
        for (x=xstart; x<xend; x+=6)
        {
            tx = x;
            if (tx >= 180)
                tx -= 360;
            if ((x < 0) || (x > 36))
                draw_minor_grids_in_6_degree_zone(map, dc, datum, 72.0, (double) tx);
        }

        if ( !(ur.lon < 0.0) && !(ll.lon > 9.0) )
        {
            // draw zone 31X
            draw_minor_grids_in_9_degree_zone(map, dc, datum, 72.0, 0.0);
        }

        if ( !(ur.lon < 9.0) && !(ll.lon > 21.0) )
        {
            // draw zone 33X
            draw_minor_grids_in_12_degree_zone(map, dc, datum, 72.0, 9.0);
        }

        if ( !(ur.lon < 21.0) && !(ll.lon > 33.0) )
        {
            // draw zone 35X
            draw_minor_grids_in_12_degree_zone(map, dc, datum, 72.0, 21.0);
        }

        if ( !(ur.lon < 33.0) && !(ll.lon > 42.0) )
        {
            // draw zone 37X
            draw_minor_grids_in_9_degree_zone(map, dc, datum, 72.0, 33.0);
        }
    }

    return SUCCESS;
}
// end of draw_minor_grids

// **********************************************************************
// **********************************************************************

// Draw the grid overlay.
int GridOverlay::draw_utm_grid(ActiveMap* map)
{
   CDC *dc = map->get_CDC();

    if (m_show_minor_gridlines)
        draw_minor_grids(map, dc);

    draw_major_zones(map, dc);

    return SUCCESS;
}
// end of draw_utm_grid

// **********************************************************************
// **********************************************************************

// Draw the latlong grid overlay.
int GridOverlay::draw_latlong_grid(ActiveMap* map)
{
    return display_grid_overlay(map);
}

// **********************************************************************
// **********************************************************************

// Draw the grid overlay.
int GridOverlay::draw(ActiveMap* map)
{
   int status;
   // display the grid overlay
   status = draw_latlong_grid(map);
   return status;
}

int GridOverlay::draw_gars(ActiveMap *map)
{
   // Constrain lower_bdry_lat and upper_bdry_lat so that upper_bdry_lat > lower_bdry_lat >= 0

    d_geo_t ll, ur;

    if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
   {
      ERR_report("GridOverlay::draw_gars(): get_vmap_bounds() failed.");
        return FAILURE;
   }

   degrees_t lower_bdry_lat = ll.lat;
   degrees_t upper_bdry_lat = ur.lat;

   while (lower_bdry_lat > upper_bdry_lat)
      upper_bdry_lat += 360;

   while (lower_bdry_lat < 0 || upper_bdry_lat < 0)
   {
      lower_bdry_lat += 360;
      upper_bdry_lat += 360;
   }

   // Constrain lower_bdry_lon and upper_bdry_lon so that upper_bdry_lon > lower_bdry_lon >= 0

   degrees_t lower_bdry_lon = ll.lon;
   degrees_t upper_bdry_lon = ur.lon;

   while (lower_bdry_lon > upper_bdry_lon)
      upper_bdry_lon += 360;

   while (lower_bdry_lon < 0 || upper_bdry_lon < 0)
   {
      lower_bdry_lon += 360;
      upper_bdry_lon += 360;
   }

   // Find min_deg_per_pix

   degrees_t deg_per_pix_lat = map->vmap_degrees_per_pixel_lat();
   degrees_t deg_per_pix_lon = map->vmap_degrees_per_pixel_lon();
   degrees_t max_deg_per_pix = deg_per_pix_lat > deg_per_pix_lon ? deg_per_pix_lat : deg_per_pix_lon;
   const int smallest_allowed_spacing = 100;

   // Various variable declarations

   CFvwUtil *futil = CFvwUtil::get_instance();
   CDC *dc = map->get_CDC();
   int width, x, y;
   bool labels_drawn = false;
   degrees_t d, shifted_d, d2, shifted_d2;
   const int COORDINATE_LEN = 8;
   char coordinate[COORDINATE_LEN];
   POINT cpt[4];
   double rotation = map->actual_rotation();
   CGeoTrans geotrans;

   //
   // Draw keypads (calculations stored for use with labels
   //

   width = m_width;

   int keypad_spacing = (int)(1/(12*max_deg_per_pix));
   if (keypad_spacing >= smallest_allowed_spacing)
   {
      for (d = ceil(12*lower_bdry_lon)/12; d <= upper_bdry_lon; d += 1.0/12)
      {
         shifted_d = d;
         while (shifted_d > 180) shifted_d -= 360;
         futil->draw_geo_line(map, dc, ll.lat, shifted_d, ur.lat, shifted_d,
            m_color, PS_SOLID, width, UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
      }

      for (d2 = ceil(12*lower_bdry_lat)/12; d2 <= upper_bdry_lat; d2 += 1.0/12)
      {
         shifted_d2 = d2;
         while (shifted_d2 > 90) shifted_d2 -= 360;
         futil->draw_geo_line(map, dc, shifted_d2, ll.lon, shifted_d2, ur.lon,
            m_color, PS_SOLID, width, UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
      }

      labels_drawn = true;

      for (d = ceil(12*lower_bdry_lon)/12; d <= upper_bdry_lon; d += 1.0/12)
      {
         shifted_d = d + 1.0/24;
         while (shifted_d > 180) shifted_d -= 360;

         for (d2 = ceil(12*lower_bdry_lat)/12; d2 <= upper_bdry_lat; d2 += 1.0/12)
         {
            shifted_d2 = d2 + 1.0/24;
            while (shifted_d2 > 90) shifted_d2 -= 360;
            geotrans.DLL_calc_gars_string(shifted_d2, shifted_d, coordinate, COORDINATE_LEN);
            map->geo_to_surface(shifted_d2, shifted_d, &x, &y);
            futil->draw_text(dc, coordinate, x, y, UTIL_ANCHOR_CENTER_CENTER,
               m_font_name, m_font_size, m_font_attrib, m_font_background, m_font_color, m_font_bg_color,
               rotation, cpt);
         }
      }
   }

   //
   // Draw quadrants
   //

   int quadrant_spacing = (int)(1/(4*max_deg_per_pix));
   if (quadrant_spacing >= smallest_allowed_spacing)
   {
      width = 2*width;

      for (d = ceil(4*lower_bdry_lon)/4; d <= upper_bdry_lon; d += 0.25)
      {
         shifted_d = d;
         while (shifted_d > 180) shifted_d -= 360;
         futil->draw_geo_line(map, dc, ll.lat, shifted_d, ur.lat, shifted_d,
            m_color, PS_SOLID, width, UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
      }

      for (d2 = ceil(4*lower_bdry_lat)/4; d2 <= upper_bdry_lat; d2 += 0.25)
      {
         shifted_d2 = d2;
         while (shifted_d2 > 90) shifted_d2 -= 360;
         futil->draw_geo_line(map, dc, shifted_d2, ll.lon, shifted_d2, ur.lon,
            m_color, PS_SOLID, width, UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
      }

      if (!labels_drawn)
      {
         labels_drawn = true;

         for (d = ceil(4*lower_bdry_lon)/4; d <= upper_bdry_lon; d += 0.25)
         {
            shifted_d = d + 0.125;
            while (shifted_d > 180) shifted_d -= 360;

            for (d2 = ceil(4*lower_bdry_lat)/4; d2 <= upper_bdry_lat; d2 += 0.25)
            {
               shifted_d2 = d2 + 0.125;
               while (shifted_d2 > 90) shifted_d2 -= 360;
               geotrans.DLL_calc_gars_string(shifted_d2, shifted_d, coordinate, COORDINATE_LEN);
               coordinate[6] = '\0'; // Truncate
               map->geo_to_surface(shifted_d2, shifted_d, &x, &y);
               futil->draw_text(dc, coordinate, x, y, UTIL_ANCHOR_CENTER_CENTER,
                  m_font_name, m_font_size, m_font_attrib, m_font_background, m_font_color, m_font_bg_color,
                  rotation, cpt);
            }
         }
      }
   }

   //
   // Draw cells
   //

   int cell_spacing = (int)(1/(2*max_deg_per_pix));
   if (cell_spacing >= smallest_allowed_spacing)
   {
      width = 2*width;

      for (d = ceil(2*lower_bdry_lon)/2; d <= upper_bdry_lon; d += 0.5)
      {
         shifted_d = d;
         while (shifted_d > 180) shifted_d -= 360;
         futil->draw_geo_line(map, dc, ll.lat, shifted_d, ur.lat, shifted_d,
            m_color, PS_SOLID, width, UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
      }

      for (d2 = ceil(2*lower_bdry_lat)/2; d2 <= upper_bdry_lat; d2 += 0.5)
      {
         shifted_d2 = d2;
         while (shifted_d2 > 90) shifted_d2 -= 360;
         futil->draw_geo_line(map, dc, shifted_d2, ll.lon, shifted_d2, ur.lon,
            m_color, PS_SOLID, width, UTIL_LINE_TYPE_GREAT, TRUE, TRUE);
      }

      if (!labels_drawn)
      {
         for (d = ceil(2*lower_bdry_lon)/2; d <= upper_bdry_lon; d += 0.5)
         {
            shifted_d = d + 0.25;
            while (shifted_d > 180) shifted_d -= 360;

            for (d2 = ceil(2*lower_bdry_lat)/2; d2 <= upper_bdry_lat; d2 += 0.5)
            {
               shifted_d2 = d2 + 0.25;
               while (shifted_d2 > 90) shifted_d2 -= 360;
               geotrans.DLL_calc_gars_string(shifted_d2, shifted_d, coordinate, COORDINATE_LEN);
               coordinate[5] = '\0'; // Truncate
               map->geo_to_surface(shifted_d2, shifted_d, &x, &y);
               futil->draw_text(dc, coordinate, x, y, UTIL_ANCHOR_CENTER_CENTER,
                  m_font_name, m_font_size, m_font_attrib, m_font_background, m_font_color, m_font_bg_color,
                  rotation, cpt);
            }
         }
      }
   }

   //
   // Draw full coordinates in lower left
   //

   geotrans.DLL_calc_gars_string(map->actual_center_lat(), map->actual_center_lon(), coordinate, COORDINATE_LEN);
   futil->draw_text(dc, coordinate, 0, map->get_surface_height(), UTIL_ANCHOR_LOWER_LEFT,
      m_font_name, m_font_size, m_font_attrib, m_font_background, m_font_color, m_font_bg_color,
      0, cpt);

   return SUCCESS;
}
