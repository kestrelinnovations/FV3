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

// compass_rose.h

#ifndef FALCONVIEW_INCLUDE_CMP_ROSE_H_
#define FALCONVIEW_INCLUDE_CMP_ROSE_H_

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/common.h"
#include "FalconView/include/maps_d.h"
#include "geo_tool/geo_tool_d.h"
// this project's headers

// forward definitions
class MapProj;
class printer_page_info;

// Compass rose class
class CompassRose
{
public:
   enum compass_rose_type_t
   { STAR = 0, ARROW = 1};

private:
   // Line properties
   int m_color;            // foreground color
   int m_style;            // foreground line style
   int m_width;            // foreground line width
   int m_color_bg;         // background color (-1 for no background line)

   // Arrow attributes
   int m_arrow_head_height;
   int m_arrow_head_width;

   // Star attributes
   int m_major_radii;
   int m_minor_radii;

   CString m_north_label;
   // Default font attributes
   int m_north_label_anchor_position;
   CString m_font_name;
   int m_font_height;

   compass_rose_type_t m_rose_type;

public:
   // Constructor (defaults to a STAR shaped rose)
   explicit CompassRose(compass_rose_type_t rose_type = STAR);

   // Destructor
   virtual ~CompassRose();

public:
   // Draw the compass rose.
   int draw(const MapProj *map, CDC *pDC);
   int draw(const MapProj *map, CDC *pDC, printer_page_info *page_info,
            int print_area_width_in_pixels, int print_area_height_in_pixels);

   int set_line_style(int style);
   int set_line_color(int line_color);
   int set_line_thickness(int width);

private:
   int compute_upper_right_rose_anchor(int surface_width, int surface_height,
      CPoint &ur);
   int compute_star_center(const CPoint &ur, CPoint &star_center);
   int draw_back_arrow_head(const MapProj *map, CDC *pDC,
            int cx, int cy, int major, int minor, double rot_angle);
   int draw_arrow_head(const MapProj *map, CDC *pDC,
            int cx, int cy, double rot_angle);

   int compute_rose_inclination_angle(const MapProj *map, CDC *pDC,
         CPoint star_center, double *rot_angle);

   int compute_end_point(const MapProj *map, CDC *pDC,
      CPoint &p1, CPoint &p2, int distance_from_center, degrees_t bearing);

   int scale_to_printed_page(const MapProj *map, CDC *pDC,
      const int is_mosaic_map_component, printer_page_info *page_info,
      int page_width, int page_height, int surface_width, int surface_height,
      CPoint &star_center);

   int draw_north_label(const MapProj *map, CDC *pDC, int x, int y,
      double rot_angle);

   int draw_star_rose(const MapProj *map, CDC *pDC, int x, int y,
      double rot_angle);

   int draw_single_star(const MapProj *map, CDC *pDC, int x, int y,
                  int major, int minor, double rot_angle);

   int draw_dashed_ring(const MapProj *map, CDC *pDC, int cx, int cy,
                    int radius, double rot_angle);

   int draw_arrow_rose(const MapProj *map, CDC *pDC, int cx, int cy,
                    int radius, double rot_angle);
};

#endif  // FALCONVIEW_INCLUDE_CMP_ROSE_H_
