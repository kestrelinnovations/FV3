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
-  FILE NAME:    grid_map.h
-  LIBRARY NAME: grid_map
-
-  DESCRIPTION:
-
-      This file contains the grid_map library's display and information
-  routines. The grid_map library is dependent on maps.lib, tiros.lib, and
-  cms.lib.
-
-------------------------------------------------------------------*/

#ifndef GRID_MAP_H
#define GRID_MAP_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"
#include "maps_d.h" 
#include "geo_tool_d.h"
#include "overlay.h"
#include "..\grid_map\gline.h"


//
// forward delclarations
//
class ActiveMap;
class GridLabel;

class GridLine;

/*------------------------------------------------------------------
-                       Function Prototypes 
-------------------------------------------------------------------*/

// Overlay class for lat-lon grid overlay
class GridOverlay : public FvOverlayImpl
{
private:
   MapScale m_largest_scale;  // the largest scale it is displayed at
   // New design: Robert Santiago 5/5/1999
   GridLine geo_lat_line;
   GridLine geo_lon_line;

   CList <GridLabel*, GridLabel*> m_gridlabel_list;

   int m_show_minor_gridlines;
   int m_show_labels_on_major_lines;
   int m_show_tickmarks;

   int m_color;            // foreground color
   int m_style;            // foreground line style
   int m_width;            // foreground line width
   int m_color_bg;         // background color (-1 for no background line)

   int m_font_color;
   int m_font_bg_color;
   CString m_font_name;
   int m_font_size;
   int m_font_attrib;
   int m_font_background;   

   int m_grid_type;

public:
   // Constructor
   GridOverlay();

   virtual void Finalize();

   // *****************************************
   // C_overlay Functions Overrides
public:
   // Returns a string identifying the class this object is an instance of.
   const char *get_class_name() { return "GridOverlay"; }

   int on_mouse_moved(IFvMapView *pMapView, CPoint /*point*/, UINT /*flags*/) { return FAILURE; }

   // Draw the coverage overlay.  Creates the file icons, as needed.  Sets the
   // properties of each file icon: on top, on screen, on target, selected.
   int draw(ActiveMap* map);
   void draw_major_zone_labels_corners(ActiveMap* map, CDC *dc);
   void draw_major_zone_labels_center(ActiveMap* map, CDC *dc);
   void draw_major_zones(ActiveMap* map, CDC *dc);
   int draw_minor_grids(ActiveMap* map, CDC *dc);
   int draw_minor_grid_gratules_100(ActiveMap* map, CDC *dc, CString major_zone, CString minor_zone,
      CString easting, CString northing);
   int draw_minor_grid_gratules_1000(ActiveMap* map, CDC *dc, CString major_zone, CString minor_zone,
      CString easting, CString northing);
   int draw_minor_grid_gratules(ActiveMap* map, CDC *dc, CString major_zone, CString minor_zone);
   int draw_minor_grid_labels(ActiveMap* map, CDC *dc, CString major_zone, CString minor_zone);
   int edge_of_zone_lat(double clat, double *edge_lat);
   int edge_of_zone_lat(double clat, double llz_lon, int width, 
      double *edge_lat_left, double *edge_lat_right);
   int draw_latlong_grid(ActiveMap* map);
   int draw_utm_grid(ActiveMap* map);
   BOOL is_standard_major_grid(char * milgrid);
   int GridOverlay::draw_minor_grids_in_3_degree_zone(ActiveMap* map, CDC *dc, char* datum,
      double llz_lat, double llz_lon);
   int draw_minor_grids_in_6_degree_zone(ActiveMap* map, CDC *dc, char* datum,
      double llz_lat, double llz_lon);
   int draw_minor_grids_in_9_degree_zone(ActiveMap* map, CDC *dc, char* datum,
      double llz_lat, double llz_lon);
   int draw_minor_grids_in_12_degree_zone(ActiveMap* map, CDC *dc, char* datum,
      double llz_lat, double llz_lon);
   int draw_minor_grids_in_a_major_zone(ActiveMap* map, CDC *dc, char* datum, double clon,
      double llz_lat, double llz_lon, 
      double urz_lat, double urz_lon);


   void set_grid_type(int type) { m_grid_type = type; }
   int set_grid_line_color( int line_color );
   int set_grid_line_style( int line_style );
   int set_grid_line_thickness( int line_thickness );
   int set_grid_tick_show_flag( int tick_show_flag );
   int set_grid_tick_length( int tick_length );

   int set_minor_gridline_show_flag( int show_minor_gridlines );
   int set_labels_on_major_gridline_show_flag( int show_labels_on_major_lines );
   int set_grid_label_properties( int label_color, int label_bg_color, CString label_font_name, 
      int label_size, int label_attrib, int background );
   int get_grid_label_properties( int *label_color, int *label_bg_color, CString *label_font_name, 
      int *label_size, int *label_attrib, int *background  );

   int get_major_zone_extents(CString major_zone, d_geo_t *ll, d_geo_t *ur);
   int get_minor_zone_extents(CString major_zone, CString minor_zone, 
      CString easting, CString northing, d_geo_t *ll, d_geo_t *ur);
   int get_minor_zone_corners(CString major_zone, CString minor_zone, d_geo_t * bnd);
   BOOL is_zone_on_screen(ActiveMap* map, CString major_zone, CString minor_zone,
      CString easting, CString northing);

   int draw_gars(ActiveMap *map);

private:
   // This function displays a Lat-Long grid in the given ActiveMap.
   int display_grid_overlay(ActiveMap* map ); 

   int draw_grid(ActiveMap *map, CDC *pDC, d_geo_t map_ll, d_geo_t map_ur );

protected:
   // Protected destructor. Call Release instead
   ~GridOverlay();
};
#endif
