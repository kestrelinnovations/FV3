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

// cmp_rose.cpp
// This file contains the Compass Rose implementation routines.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/include/cmp_rose.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/err.h"
#include "FalconView/include/fvwutil.h"
#include "FalconView/include/map.h"
#include "FalconView/mapview.h"

// this project's headers

static inline
int round(double val)
{
   return (val > 0.0 ?
      (static_cast<int> (val + 0.5)) : (static_cast<int> (val - 0.5)));
}

static
int draw_arc(CDC *pDC, int pen_color, int pen_width,
      boolean_t fill, int ul_x, int ul_y, int lr_x, int lr_y,
      int from_x, int from_y, int to_x, int to_y);



CompassRose::CompassRose(compass_rose_type_t rose_type) : m_rose_type(rose_type)
{
   m_major_radii = 100;
   m_minor_radii = 20;

   // Arrow attributes
   m_arrow_head_height = 28;
   m_arrow_head_width  = 24;

   m_north_label = "N";

   // Default font attributes
   m_north_label_anchor_position = UTIL_ANCHOR_LOWER_CENTER;
   m_font_name = "Arial";
   m_font_height = 32;
}

CompassRose::~CompassRose()
{
}

int CompassRose::set_line_color(int line_color)
{
   m_color = line_color;

   if (line_color == UTIL_COLOR_BLACK)
   {
      m_color_bg = UTIL_COLOR_WHITE;
   }
   else
   {
      m_color_bg = UTIL_COLOR_BLACK;
   }

   return SUCCESS;
}

int CompassRose::set_line_thickness(int width)
{
   m_width = width;

   return SUCCESS;
}


int CompassRose::set_line_style(int style)
{
   m_style = style;
   return SUCCESS;
}

int CompassRose::compute_upper_right_rose_anchor(int surface_width,
   int surface_height, CPoint &ur)
{
   m_major_radii = surface_width / 16;
   m_minor_radii = m_major_radii / 5;

   // Arrow attributes
   m_arrow_head_height = m_major_radii / 2;
   m_arrow_head_width  = m_major_radii / 4;

   m_font_height = m_major_radii / 2;

   int percent_from_edge = 5;

   int ur_x_percent = (surface_width*percent_from_edge)/100;
   int ur_y_percent = (surface_height*percent_from_edge)/100;
   int half_rose_height = m_arrow_head_height + m_font_height + m_major_radii/2;

   // Prevent bleeding by setting the ur corner point to the max
   // distance from the center of the rose to its north edge
   CString pos_str = "Right";
   if (pos_str == "Right")
   {
      if (half_rose_height > ur_x_percent)
      {
         ur.x = surface_width - half_rose_height;
         ur.y = half_rose_height;
      }
      else
      {
         ur.x = surface_width - ur_x_percent;
         ur.y = ur_y_percent;
      }
   }
   else
   {
      if (half_rose_height > ur_x_percent)
      {
         ur.x = half_rose_height *2;
         ur.y = half_rose_height;
      }
      else
      {
         ur.x = ur_x_percent*2;
         ur.y = ur_y_percent;
      }
   }

   return SUCCESS;
}


int CompassRose::compute_star_center(const CPoint &ur, CPoint &star_center)
{
   star_center.x = ur.x - m_major_radii/2;
   star_center.y = ur.y + m_arrow_head_height + m_font_height + m_major_radii/2;

   return SUCCESS;
}

int CompassRose::draw_north_label(const MapProj *map, CDC *pDC, int cx, int cy,
   double rot_angle)
{
   int ul_x = cx;
   int ul_y = cy - m_major_radii/2 - m_font_height;
   int nx, ny;

   CFvwUtil *futil = CFvwUtil::get_instance();
   futil->rotate_pt(ul_x, ul_y, &nx, &ny, rot_angle, cx, cy);
   ul_x = nx;
   ul_y = ny;

   POINT cpt[4];

   int color = BLACK;
   int color_bg = BRIGHT_WHITE;

   int lbw = 1;   // label border width
   if (pDC->IsPrinting())
      lbw = 6;


   CString north_label = m_north_label;
   int font_height = m_font_height;

   // Draw the background outline of the label first
   futil->draw_text(pDC, north_label, ul_x + lbw, ul_y + lbw,
      m_north_label_anchor_position, m_font_name, font_height, UTIL_FONT_BOLD,
      UTIL_BG_NONE, color_bg, color, rot_angle, cpt, TRUE);
   futil->draw_text(pDC, north_label, ul_x - lbw, ul_y + lbw,
      m_north_label_anchor_position, m_font_name, font_height, UTIL_FONT_BOLD,
      UTIL_BG_NONE, color_bg, color, rot_angle, cpt, TRUE);
   futil->draw_text(pDC, north_label, ul_x, ul_y + lbw,
      m_north_label_anchor_position, m_font_name, font_height, UTIL_FONT_BOLD,
      UTIL_BG_NONE, color_bg, color, rot_angle, cpt, TRUE);
   futil->draw_text(pDC, north_label, ul_x - lbw, ul_y - lbw,
      m_north_label_anchor_position, m_font_name, font_height, UTIL_FONT_BOLD,
      UTIL_BG_NONE, color_bg, color, rot_angle, cpt, TRUE);
   futil->draw_text(pDC, north_label, ul_x + lbw, ul_y - lbw,
      m_north_label_anchor_position, m_font_name, font_height, UTIL_FONT_BOLD,
      UTIL_BG_NONE, color_bg, color, rot_angle, cpt, TRUE);
   futil->draw_text(pDC, north_label, ul_x, ul_y - lbw,
      m_north_label_anchor_position, m_font_name, font_height, UTIL_FONT_BOLD,
      UTIL_BG_NONE, color_bg, color, rot_angle, cpt, TRUE);
   futil->draw_text(pDC, north_label, ul_x + lbw, ul_y,
      m_north_label_anchor_position, m_font_name, font_height, UTIL_FONT_BOLD,
      UTIL_BG_NONE, color_bg, color, rot_angle, cpt, TRUE);
   futil->draw_text(pDC, north_label, ul_x - lbw, ul_y,
      m_north_label_anchor_position, m_font_name, font_height, UTIL_FONT_BOLD,
      UTIL_BG_NONE, color_bg, color, rot_angle, cpt, TRUE);

   // Draw the foreground label
   futil->draw_text(pDC, north_label, ul_x, ul_y, m_north_label_anchor_position,
      m_font_name, font_height, UTIL_FONT_BOLD, UTIL_BG_NONE, color, color_bg,
      rot_angle, cpt, TRUE);

   return SUCCESS;
}

// Draws a 4 peak star and shades black every other internal triangle
int CompassRose::draw_single_star(const MapProj *map, CDC *pDC, int x, int y,
                                  int major, int minor, double rot_angle)
{
   int i;
   int nx, ny;
   CFvwUtil *futil = CFvwUtil::get_instance();

   const int kNumPoints = 9;
   CPoint star[kNumPoints];
   CPoint triangle[3];

   // center of star
   star[8].x = x;
   star[8].y = y;

   star[0].x = x;
   star[0].y = y + major/2;

   star[1].x = x - minor / 2;
   star[1].y = y + minor/2;

   star[2].x = x - major / 2;
   star[2].y = y;

   star[3].x = star[1].x;
   star[3].y = y - minor/2;

   star[4].x = star[0].x;
   star[4].y = y - major/2;

   star[5].x = x + minor / 2;
   star[5].y = star[3].y;

   star[6].x = x + major / 2;
   star[6].y = star[2].y;

   star[7].x = star[5].x;
   star[7].y = star[1].y;

   for (i = 0; i < kNumPoints; i++)
   {
      futil->rotate_pt(star[i].x, star[i].y, &nx, &ny, rot_angle, x, y);
      star[i].x = nx;
      star[i].y = ny;
   }

   futil->polygon(pDC, star, kNumPoints-1);

   CBrush br(futil->code2color(BLACK));
   CBrush *old_br;

   old_br  = pDC->SelectObject(&br);

   // draw dark triangles
   triangle[0] = star[8];  // center point (same for all)

   triangle[1] = star[0];
   triangle[2] = star[7];
   futil->polygon(pDC, triangle, 3);

   triangle[1] = star[6];
   triangle[2] = star[5];
   futil->polygon(pDC, triangle, 3);

   triangle[1] = star[4];
   triangle[2] = star[3];
   futil->polygon(pDC, triangle, 3);

   triangle[1] = star[2];
   triangle[2] = star[1];
   futil->polygon(pDC, triangle, 3);

   pDC->SelectObject(old_br);

   return SUCCESS;
}

// Draws an arrow shaped compass rose
int CompassRose::draw_dashed_ring(const MapProj *map, CDC *pDC, int cx, int cy,
                    int radius, double rot_angle)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   UtilDraw udraw(pDC);

   int width = m_minor_radii / 2;

   int lw = 16;

   if (!pDC->IsPrinting())
      lw = 3;

   // Draw the background ring
   udraw.set_pen(BLACK, UTIL_LINE_SOLID, width + lw, TRUE);
   udraw.draw_circle(FALSE, cx, cy, radius);
   udraw.set_pen(BRIGHT_WHITE, UTIL_LINE_SOLID, width);
   udraw.draw_circle(FALSE, cx, cy, radius);
   udraw.restore_pen();

   // Draw the dashed ring
   double from_x, from_y, to_x, to_y;
   int x1, y1, x2, y2, nx, ny;
   for (double theta = 0.0; theta < 360.0; theta += 90.0)
   {
      // starting at 90 degrees (12 o'clock)
      double alpha = 90.0-theta;
      if (alpha < 0)
         alpha+=360;

      double rad_angle_from = DEG_TO_RAD(alpha);
      double rad_angle_to = DEG_TO_RAD(alpha+45.0);
      from_x = radius*sin(rad_angle_from);
      from_y = radius*cos(rad_angle_from);
      to_x = radius*sin(rad_angle_to);
      to_y = radius*cos(rad_angle_to);

      x1 = round(from_x) + cx;
      y1 = round(from_y) + cy;
      x2 = round(to_x) + cx;
      y2 = round(to_y) + cy;

      futil->rotate_pt(x1, y1, &nx, &ny, rot_angle, cx, cy);
      x1 = nx;
      y1 = ny;
      futil->rotate_pt(x2, y2, &nx, &ny, rot_angle, cx, cy);
      x2 = nx;
      y2 = ny;

      draw_arc(pDC, BLACK, width+lw, FALSE, cx-radius, cy-radius,
         cx+radius, cy+radius, x1, y1, x2, y2);
   }

   return SUCCESS;
}

int CompassRose::draw_arrow_head(const MapProj *map, CDC *pDC,
            int cx, int cy, double rot_angle)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   const int kNumPoints = 4;
   CPoint star_cap[kNumPoints];

   int x = cx;
   int y = cy-m_major_radii/2-m_arrow_head_height;

   int arrow_height = m_arrow_head_height;
   int arrow_width  = (m_arrow_head_width +32);
   int minor_height = (arrow_height * 5)/8;

   star_cap[0].x = x;
   star_cap[0].y = y;

   star_cap[1].x = star_cap[0].x - arrow_width/2;
   star_cap[1].y = star_cap[0].y + arrow_height;

   star_cap[2].x = star_cap[0].x;
   star_cap[2].y = y + minor_height;

   star_cap[3].x = star_cap[0].x + arrow_width/2;
   star_cap[3].y = star_cap[0].y + arrow_height;

   int nx, ny;
   for (int i = 0; i < kNumPoints; i++)
   {
      futil->rotate_pt(star_cap[i].x, star_cap[i].y,
         &nx, &ny, rot_angle, cx, cy);
      star_cap[i].x = nx;
      star_cap[i].y = ny;
   }
   futil->polygon(pDC, star_cap, 4);


   CBrush br(futil->code2color(BLACK));
   CBrush *old_br;
   old_br  = pDC->SelectObject(&br);

   // draw dark triangle
   CPoint triangle[3];
   triangle[0] = star_cap[0];
   triangle[1] = star_cap[1];
   triangle[2] = star_cap[2];

   futil->polygon(pDC, triangle, 3);

   pDC->SelectObject(old_br);

   return SUCCESS;
}

int CompassRose::draw_back_arrow_head(const MapProj *map, CDC *pDC,
            int cx, int cy, int major, int minor, double rot_angle)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   const int kNumPoints = 4;
   CPoint star_cap[kNumPoints];

   int x = cx;
   int y = cy-m_major_radii/2-m_arrow_head_height;

   int arrow_height = major + m_arrow_head_height*2;
   int arrow_width  = m_arrow_head_width*2;
   int minor_height = (arrow_height * 3)/4;

   star_cap[0].x = x;
   star_cap[0].y = y;

   star_cap[1].x = star_cap[0].x - arrow_width/2;
   star_cap[1].y = star_cap[0].y + arrow_height;

   star_cap[2].x = star_cap[0].x;
   star_cap[2].y = y + minor_height;

   star_cap[3].x = star_cap[0].x + arrow_width/2;
   star_cap[3].y = star_cap[0].y + arrow_height;

   int nx, ny;
   for (int i = 0; i < kNumPoints; i++)
   {
      futil->rotate_pt(star_cap[i].x, star_cap[i].y,
         &nx, &ny, rot_angle, cx, cy);
      star_cap[i].x = nx;
      star_cap[i].y = ny;
   }
   futil->polygon(pDC, star_cap, 4);


   CBrush br(futil->code2color(BLACK));
   CBrush *old_br;
   old_br  = pDC->SelectObject(&br);

   // draw dark triangle
   CPoint triangle[3];
   triangle[0] = star_cap[0];
   triangle[1] = star_cap[1];
   triangle[2] = star_cap[2];

   futil->polygon(pDC, triangle, 3);

   pDC->SelectObject(old_br);

   return SUCCESS;
}

int CompassRose::draw_arrow_rose(const MapProj *map, CDC *pDC, int cx, int cy,
                    int radius, double rot_angle)
{
   draw_dashed_ring(map, pDC, cx, cy, m_major_radii/2, rot_angle);

   draw_back_arrow_head(map, pDC, cx, cy,
            m_major_radii, m_minor_radii, rot_angle);

   return SUCCESS;
}

int CompassRose::draw_star_rose(const MapProj *map, CDC *pDC,
   int x, int y, double rot_angle)
{
   draw_arrow_head(map, pDC, x, y, rot_angle);

   // Draw smaller star (rotated 45 degrees and smaller)
   double rot_angle_minor_star = rot_angle - 45.0;
   if (rot_angle_minor_star < 0.0)
      rot_angle_minor_star += 360.0;
   draw_single_star(map, pDC, x, y,
      (m_major_radii*3)/5,
      (m_minor_radii*3)/5,
      rot_angle_minor_star);

   // Draw larger star
   draw_single_star(map, pDC, x, y, m_major_radii, m_minor_radii, rot_angle);

   // Draw Outline Circles
   UtilDraw udraw(pDC);

   int width = m_minor_radii / 4;
   int radius = m_major_radii /2;

   // Draw the background ring
   udraw.set_pen(BLACK, UTIL_LINE_SOLID, width + 16, TRUE);
   udraw.draw_circle(FALSE, x, y, radius);
   udraw.set_pen(BRIGHT_WHITE, UTIL_LINE_SOLID, width);
   udraw.draw_circle(FALSE, x, y, radius);

   udraw.set_pen(BLACK, UTIL_LINE_SOLID, width/2);
   udraw.draw_circle(FALSE, x, y, radius+width + 4);

   return SUCCESS;
}

int CompassRose::compute_end_point(const MapProj *map, CDC *pDC,
   CPoint &p1, CPoint &p2, int distance_from_center, degrees_t bearing)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   int x1 = p1.x;
   int y1 = p1.y;
   int x2;
   int y2;

   int north_line_length = distance_from_center;

   //  Convert the major radius distance to km
   double dist_x, dist_y;
   futil->pixels_to_km(const_cast<MapProj*>(map), x1, y1,
         x1, y1 + north_line_length, &dist_x, &dist_y);

   // now convert to meters
   double length_meters;
   length_meters = (dist_x * dist_x) + (dist_y * dist_y);
   length_meters = sqrt(length_meters);
   length_meters *= 1000.0;

   // Convert starting point to geo coords
   d_geo_t start, end;
   if (map->surface_to_geo(x1, y1,
         &start.lat, &start.lon) != SUCCESS)
   {
      ERR_report(
         "map->surface_to_geo() failed in CompassRose::compute_end_point().");
      return FAILURE;
   }

   // Compute end coord pointing north
   if (GEO_calc_end_point(start, length_meters, bearing, end, FALSE) != SUCCESS)
   {
      ERR_report(
         "GEO_calc_end_point() failed in CompassRose::compute_end_point().");
      return FAILURE;
   }

   // Compute view coordinates for line and star drawing
   if (map->geo_to_surface(end.lat, end.lon, &x2, &y2) != SUCCESS)
   {
      ERR_report(
         "map->geo_to_surface() failed in CompassRose::compute_end_point().");
      return FAILURE;
   }

   p2.x = x2;
   p2.y = y2;

   return SUCCESS;
}

int CompassRose::scale_to_printed_page(const MapProj *map, CDC *pDC,
   const int is_mosaic_map_component, printer_page_info *page_info,
   int page_width, int page_height, int surface_width, int surface_height,
   CPoint &star_center)
{
   int min_page_length = page_width < page_height ? page_width : page_height;
   int min_surface_length =
      surface_width < surface_height ? surface_width : surface_height;

   // Use the surface width and page width since they are consistent
   // between scaling and not scaling while printing
   double scale_x = round(min_page_length /
      static_cast<double>(min_surface_length));
   double scale_y = scale_x;

   star_center.x = page_width - round(
      static_cast<double>(surface_width - star_center.x)
      * round(page_width/static_cast<double>(surface_width)));
   star_center.y = static_cast<int>(static_cast<double>(star_center.y)
      * round(page_height/static_cast<double>(surface_height))+0.5);

   // scale the font height
   m_font_height = static_cast<int>(
      static_cast<double>(m_font_height) * scale_y+0.5);

   // scale the star major and minor radii
   m_major_radii = static_cast<int>(
      static_cast<double>(m_major_radii) * scale_x+0.5);
   m_minor_radii = static_cast<int>(
      static_cast<double>(m_minor_radii) * scale_y+0.5);

   // scale the arrow head dimensions
   m_arrow_head_width = static_cast<int>(
      static_cast<double>(m_arrow_head_width) * scale_x+0.5);
   m_arrow_head_height = static_cast<int>(
      static_cast<double>(m_arrow_head_height) * scale_x+0.5);

   // Get the physical coordinates on a component map if on a mosaic map
   if (is_mosaic_map_component)
   {
      int temp_x, temp_y;
      map->mosaic_physical_to_component_physical(
            star_center.x, star_center.y,
            page_info->print_area_width_in_inches,
            page_info->print_area_height_in_inches,
            page_info->print_area_width_in_pixels,
            page_info->print_area_height_in_pixels,
            &temp_x, &temp_y);

      star_center.x = temp_x;
      star_center.y = temp_y;
   }


   return SUCCESS;
}

int CompassRose::compute_rose_inclination_angle(const MapProj *map, CDC *pDC,
         CPoint star_center, double *rot_angle)
{
   CPoint end;

   // Calculate the geographical point from the center that
   // is pointing north (bearing 0)
   compute_end_point(map, pDC, star_center, end,
      m_major_radii/2 + m_font_height + m_arrow_head_height, 0.0);

   // calcultate the inclination angle of the north oriented line
   CFvwUtil *futil = CFvwUtil::get_instance();
   double angle = futil->line_angle(end.x, end.y, star_center.x, star_center.y);
   angle = futil->normalize_angle(angle-90.0);

   *rot_angle = angle;

   return SUCCESS;
}


int CompassRose::draw(const MapProj *map, CDC *pDC)
{
   // Get the client's extents
   int surface_width, surface_height;
   if (map->get_surface_size(&surface_width, &surface_height) != SUCCESS)
   {
      ERR_report("map->get_surface_size() failed in CompassRose::draw().");
      return FAILURE;
   }

   int min_surface_length = surface_width < surface_height
                                 ? surface_width : surface_height;

    // Upper right corner of rose
   CPoint ur;
   compute_upper_right_rose_anchor(surface_width, surface_height, ur);


   // Center point of rose (for rotation)
   CPoint star_center;
   compute_star_center(ur, star_center);

   // Compute rose's inclination angle
   double rot_angle;
   compute_rose_inclination_angle(map, pDC, star_center, &rot_angle);

   d_geo_t geo_center;

   map->surface_to_geo(star_center.x, star_center.y,
      &geo_center.lat, &geo_center.lon);

   degrees_t convergence_angle, map_rotation_angle = map->actual_rotation();
   const_cast<MapProj*>(map)->get_meridian_covergence(
      geo_center, &convergence_angle);

   TRACE("Rose Incl angle = %lf, Convergence angle = %lf\n",
      rot_angle, convergence_angle);

   convergence_angle += map_rotation_angle;
   if (convergence_angle > WORLD_DEG)
      convergence_angle -= WORLD_DEG;

   TRACE("Map Rotation angle = %lf, Adjusted Convergence = %lf\n",
      map_rotation_angle, convergence_angle);
   TRACE("---------------------------\n");

   rot_angle = convergence_angle;

   // Now draw the compass rose based on the type
   if (m_rose_type == STAR)
   {
      if (draw_star_rose(map, pDC, star_center.x, star_center.y, rot_angle) !=
         SUCCESS)
      {
         ERR_report("draw_star() failed in CompassRose::draw().");
         return FAILURE;
      }
   }
   else if (m_rose_type == ARROW)
   {
      if (draw_arrow_rose(map, pDC, star_center.x, star_center.y,
                              m_major_radii/2, rot_angle) != SUCCESS)
      {
         ERR_report("draw_arrow_rose() failed in CompassRose::draw().");
         return FAILURE;
      }
   }
   else
   {
      ERR_report("Invalid Compass Rose type in CompassRose::draw().");
      return FAILURE;
   }

   // Draw the North label ("N")
   if (draw_north_label(map, pDC,
         star_center.x, star_center.y, rot_angle) != SUCCESS)
   {
      ERR_report("draw_north_label() failed in CompassRose::draw().");
      return FAILURE;
   }

   return SUCCESS;
}



// Draw the Compass Rose
int CompassRose::draw(const MapProj *map, CDC *pDC,
   printer_page_info *page_info, int print_area_width_in_pixels,
   int print_area_height_in_pixels)
{
   const int is_mosaic_map_component= map->is_mosaic_map_component();

   // Get the client's extents
   int surface_width, surface_height;
   if (map->get_surface_size(&surface_width, &surface_height) != SUCCESS)
   {
      ERR_report("map->get_surface_size() failed in CompassRose::draw().");
      return FAILURE;
   }

   int min_surface_length = surface_width < surface_height
                                 ? surface_width : surface_height;

   // For mosaic maps generate the compass rose  only on
   // topmost and rightmost page
   int page_width = print_area_width_in_pixels;
   int page_height = print_area_height_in_pixels;

    // Upper right corner of rose
   CPoint ur;
   compute_upper_right_rose_anchor(surface_width, surface_height, ur);

   // Center point of rose (for rotation)
   CPoint star_center;
   compute_star_center(ur, star_center);

   // Compute rose's inclination angle
   double rot_angle;
   compute_rose_inclination_angle(map, pDC, star_center, &rot_angle);

   // Now scale to the printed page
   // scale star center point
   scale_to_printed_page(map, pDC, is_mosaic_map_component, page_info,
      page_width, page_height, surface_width, surface_height, star_center);

   // Now draw the compass rose based on the type
   if (m_rose_type == STAR)
   {
      if (draw_star_rose(map, pDC, star_center.x, star_center.y, rot_angle) !=
         SUCCESS)
      {
         ERR_report("draw_star() failed in CompassRose::draw().");
         return FAILURE;
      }
   }
   else if (m_rose_type == ARROW)
   {
      if (draw_arrow_rose(map, pDC, star_center.x, star_center.y,
                              m_major_radii/2, rot_angle) != SUCCESS)
      {
         ERR_report("draw_arrow_rose() failed in CompassRose::draw().");
         return FAILURE;
      }
   }
   else
   {
      ERR_report("Invalid Compass Rose type in CompassRose::draw().");
      return FAILURE;
   }

   // Draw the North label ("N")
   if (draw_north_label(map, pDC,
         star_center.x, star_center.y, rot_angle) != SUCCESS)
   {
      ERR_report("draw_north_label() failed in CompassRose::draw().");
      return FAILURE;
   }

   return SUCCESS;
}


// draws an arc with a custom geometric pen
static
int draw_arc(CDC *pDC, int pen_color, int pen_width,
      boolean_t fill, int ul_x, int ul_y, int lr_x, int lr_y,
      int from_x, int from_y, int to_x, int to_y)
{
   // Check the OS version, WIn95 doesn't handle path's with arcs
   if (UTL_is_win32_windows())
   {
      UtilDraw udraw(pDC);
      udraw.set_pen(pen_color, UTIL_LINE_TMARK, pen_width);

      return udraw.draw_arc(FALSE, ul_x, ul_y, lr_x, lr_y,
         from_x, from_y, to_x, to_y);
   }

   // Windows NT code
   CFvwUtil *futil = CFvwUtil::get_instance();
   CPen *old_pen;
   CPen pen;

   pDC->BeginPath();
   {
      LOGBRUSH log_brush;
      log_brush.lbStyle = BS_SOLID;
      log_brush.lbColor = futil->code2color(pen_color);

      pen.CreatePen(PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_MITER,
            pen_width, &log_brush);

      old_pen = pDC->SelectObject(&pen);

      pDC->Arc(ul_x, ul_y, lr_x, lr_y, from_x, from_y, to_x, to_y);
   }
   pDC->EndPath();
   pDC->StrokePath();

   // Clean up
   pDC->SelectObject(old_pen);

   return SUCCESS;
}
