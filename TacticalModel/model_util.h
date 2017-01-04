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

// util.h

#pragma once

#define OVL_IN_FALCONVIEW 1

#if OVL_IN_FALCONVIEW

#else
#define GEO_east_of_degrees(lon1, lon2) CUtil::geo_east_of(lon1, lon2)

#define GEO_intersect(lat1, lon1, lat2, lon2, lat3, lon3, lat4, lon4)  \
   CUtil::geo_intersect(lat1, lon1, lat2, lon2, lat3, lon3, lat4, lon4)
#endif


#include "geo_tool_d.h"
#include "TacModel.h"

#define FORMAT_DEGREES 1
#define FORMAT_DEGREES_MINUTES 2
#define FORMAT_DEGREES_MINUTES_SECONDS 3

#define UTIL_FIL_READ_OK   4
#define UTIL_FIL_WRITE_OK  2
#define UTIL_FIL_EXISTS    0
#define UTIL_FIL_END_OF_FILE 99


#define  GEO_A_over_B     1 //- top edge of B intersects A
#define  GEO_B_over_A     2 //- top edge of A intersects B
#define  GEO_A_left_B     4 //- left edge of B intersects A
#define  GEO_B_left_A     8 //- left edge of A intersects B
#define  GEO_A_in_B_lat  16 //- latitudes of A completely in B
#define  GEO_A_in_B_lon  32 //- longitudes of A completely in B
#define  GEO_A_in_B_all  48 //- A completely in B
#define  GEO_B_in_A_lat  64 //- latitudes of B completely in A
#define  GEO_B_in_A_lon 128 //- longitudes of B completely in A
#define  GEO_B_in_A_all 192 //- B completely in A


class CUtil
{
public:
   CUtil(){}

   static int geo_east_of(double a, double b);
   static void compute_center_geo(d_geo_t ll, d_geo_t ur, double *lat, double *lon);
   static int geo_to_xyz(double lat, double lon, double height, double *x, double *y, double *z);
   static int xyz_to_geo(double x, double y, double z, double *lat, double *lon, double *height);
   static int center_of_mass(d_geo_t *geo, int numpts, d_geo_t *center_geo);
   static BOOL geo_lon_in_range(double left_lon, double right_lon, double point_lon);
   static BOOL geo_in_bounds(double ll_lat, double ll_lon,
                           double ur_lat, double ur_lon,
                           double p_lat,  double p_lon);
   static BOOL geo_intersect(double ll_A_lat, double ll_A_lon,
               double ur_A_lat, double ur_A_lon,
               double ll_B_lat, double ll_B_lon,
               double ur_B_lat, double ur_B_lon);
   static BOOL geo_intersect(d_geo_t ll_A, d_geo_t ur_A, d_geo_t ll_B, d_geo_t ur_B);

   static double line_angle(int x1, int y1,      // start point of line
                      int x2, int y2);      // end point of line
   static int distance_to_line (   int x1, int y1,  // pt on the line 
                                int x2, int y2,  // pt on the line 
                                int xp, int yp ); // pt to test 
   static BOOL point_in_polygon(int px, int py, double *poly_x, double *poly_y, int numverts);
   static BOOL point_in_polygon(POINT tp, POINT *pts, int numverts);
   static BOOL point_on_polygon(CPoint point, int threshold, POINT *pgon, int numverts);
   static BOOL polygon_is_clockwise(POINT *pgon, int numverts);

   static double line_angle(POINT p1,      // start point of line
                      POINT p2);      // end point of line

   static double line_angle(double x1, double y1,      // start point of line
                      double x2, double y2);      // end point of line
   static BOOL line_segment_intersection( double ax1, double ay1, double ax2, double ay2,
                           double bx1, double by1, double bx2, double by2,
                           double *cx, double *cy);
   static CString extract_path( const CString& csFilespec );
   static CString extract_filename( const CString& csFilespec );
   static CString extract_extension(CString fullname);
   static BOOL create_directory(const CString& dirname);
   static int round(double val);
   static int magnitude(int x1, int y1, int x2, int y2);
   static void draw_text(
               CDC *dc,            // pointer to DC to draw in
               CString text,       // text to draw
               int x, int y,       // screen x,y position
               int anchor_pos,     // reference position of text
               CString font,       // font name
               int font_size,      // font size in points
               int font_attrib,    // font attributes (bold, italic, etc)
               int background,     // background type
               int text_color,     // code for text color
               int back_color,     // code for background color
               double angle,       // angle of text
               POINT *cpt,         // 4 point array defining the text polygon corners
               BOOL pad_spaces = TRUE     // default is TRUE
               );
   static void get_text_size
   (
      CDC *dc,          // pointer to DC to draw in
      CString text,     // text to draw
      CString font_name,// font name
      int font_size,    // font size in points
      int font_attrib,  // font attributes (bold, italic, etc)
      int *width,       // width of draw text
      int *height,      // height of draw text
      BOOL pad_spaces = TRUE  // default is TRUE
   );
   static void get_text_size
   (
      CDC *dc,          // pointer to DC to draw in
      CString text,     // text to draw
      CString font_name,// font name
      int font_size,    // font size in points
      int font_attrib,  // font attributes (bold, italic, etc)
      int *width,       // width of draw text
      int *height,      // height of draw text
      BOOL pad_spaces,   // default is TRUE
      double angle     // default is 0
   );
   static void get_multi_line_bounds
   (
      CDC *dc,          // pointer to DC to draw in
      CString text,     // text to draw
      int x, int y,     // screen x,y position
      int anchor_pos,   // reference position of text
      CString font,     // font name
      int font_size,    // font size in points
      int font_attrib,  // font attributes (bold, italic, etc)
      int background,   // background type
      double angle,     // angle of text
      POINT *cpt,       // 4 point array defining the text polygon corners
      BOOL pad_spaces   // default is TRUE
   );
   static void parse_the_text(CString text, CList<CString *, CString *> &text_list);
   static UINT anchor2textalign(int anchor);
   static int code2fill(int code);
   static COLORREF code2color(int code);
   static void compute_text_poly( int tx, int ty,      // x/y of anchor point
                                  int anchor_pos,                // position of anchor point
                                  int width, int height,   // height and width of text
                                  double text_angle,      // angle of text
                                  POINT *cpt);       // OUT - corners of rectangle enclosing text
   static int file_access(const char *path, int mode);
   CString get_system_time();
   CString path_to_dir_name(CString path);
   int get_dir_list(CString path, CStringArray * list);
   int get_first_file(CString path, CString filespec, CString & filename);
   int get_file_list(CString path, CString filespec, CStringArray * list);
   int get_deep_file_list(CString path, CStringArray * list);
   int unzip_dir(CString dir, CString temp_dir);
   CString get_data_path();
   CString get_temp_dir();
   int lod_from_dir(CString dir, CString & lod, CString & uref);
   int dir_to_tile_bounds(CString dir, double *ll_lat, double *ll_lon, double *ur_lat, double *ur_lon);
   int dir_to_tile_bounds_str(CString dir, CString & north_str, CString & east_str);
   int dir_to_geocell_bounds(CString dir, double *ll_lat, double *ll_lon, double *ur_lat, double *ur_lon);
   CString get_read_index_dir(CString root_path);
   CString get_write_index_dir(CString root_path);
//   int lat_lon_to_string(double lat, double lon, double dpp, int lat_lon_format, char *lat_lon_str);
   static BOOL valid_overlay(C_model_ovl *ovl);
   static double log2(const double x);
   static _bstr_t T2WFS( LPCTSTR pszFilespec ); // Return wide unicode in \\?\ format

   BOOL delete_directory(CString path); // deletes a directory and its files


};