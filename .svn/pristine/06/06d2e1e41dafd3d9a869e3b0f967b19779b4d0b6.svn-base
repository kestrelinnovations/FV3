// ovlutil.h

#pragma once

#include "fvwutil.h"
#include "..\resource.h"
#include "err.h"
#include "ovlelem.h"

// op codes for small vertex
#define OP_MOVETO 1
#define OP_LINETO 2
#define MAX_OVAL_PTS 80

// ******************************************************************
// ******************************************************************

class C_ovlkit_small_vertex
{
public:
   C_ovlkit_small_vertex(int opcode, int x, int y);

   ~C_ovlkit_small_vertex();

   int m_opcode;
   int m_x;
   int m_y;
   int m_x2;
   int m_y2;
};
// end of C_ovlkit_small_vertex

// ******************************************************************
// ******************************************************************

class C_ovlkit_screen_polygon
{
public:
   C_ovlkit_screen_polygon();

   ~C_ovlkit_screen_polygon();

   CList<C_ovlkit_small_vertex*, C_ovlkit_small_vertex*> m_line_list;

   void clear();
   BOOL point_in_polygon(CPoint testpt);
   BOOL point_in_polygon(CPoint point, POINT *pgon, int numverts);
   int center_of_polygon(CPoint *ctr_pt);
   BOOL point_on_polygon(CPoint testpt);


};
// end of C_ovlkit_small_vertex

// ******************************************************************
// ******************************************************************

class C_ovlkit_ovl_list
{
public:

   C_ovlkit_ovl_list(int index, C_icon* icon)
   {
      m_index = index;
      m_icon = icon;
   }

   int m_index;
   C_icon* m_icon;
   static int m_last_index;
};
// end of class C_ovlkit_ovl_list


// ******************************************************************
// ******************************************************************

class COvlkitUtil
{
public:

   COvlkitUtil();

   COLORREF code2color(int code);
   int color2code(COLORREF color);

   void draw_line(MapProj* map, CDC* dc,
                   int x1, int y1,  // beginning point
                   int x2, int y2,  // ending point
                   int color,  // code for color
                   int backcolor,  // code for backcolor, -1 for none
                   int style,  // code for style
                   int width);  // line width

   void draw_line_rgb(MapProj* map, CDC* dc,
                   int x1, int y1,  // beginning point
                   int x2, int y2,  // ending point
                   COLORREF color,  // code for color
                   COLORREF backcolor,  // code for backcolor, -1 for none
                   int style,  // code for style
                   int width);  // line width

   void geo_draw_line(MapProj* map, CDC* dc,
                   degrees_t lat1,
                   degrees_t lon1,
                   degrees_t lat2,
                   degrees_t lon2,
                   int color,  // code for color
                   int backcolor,  // code for backcolor, -1 for none
                   int style,  // code for style
                   int width);  // line width

   void geo_draw_line_2(MapProj* map, CDC* dc,
                   degrees_t lat1,
                   degrees_t lon1,
                   degrees_t lat2,
                   degrees_t lon2,
                   int color,  // code for color, -1 for XOR line
                   int backcolor,  // code for backcolor, -1 for none
                   int style,  // code for style
                   int line_width,  // line width in pixels, if < 0 in meters
                   int line_type = UTIL_LINE_TYPE_SIMPLE);   // simple=1, rhumb=2, great_circle=3

   int geo_draw_polygon_rgb(MapProj* map, CDC* dc, int numverts,
                        degrees_t *plat, degrees_t *plon,
                        COLORREF color,  // rgb color, -1 for XOR line
                        COLORREF backcolor,  // code for backcolor, -1 for none
                        int style,  // code for style
                        int line_width,  // line width in pixels, if < 0 in meters
                        int line_type,  // simple=1, rhumb=2, great_circle=3
                        COLORREF fill_color,
                        int fill_style);

   int geo_draw_polygon(MapProj* map, CDC* dc, int numverts,
                        degrees_t *plat, degrees_t *plon,
                        int color,  // code for color, -1 for XOR line
                        int backcolor,  // code for backcolor, -1 for none
                        int style,  // code for style
                        int line_width,  // line width in pixels, if < 0 in meters
                        int line_type,   // simple=1, rhumb=2, great_circle=3
                        int fill_color,
                        int fill_style);

   // get multiple points per side based on line type
   int COvlkitUtil::geo_draw_polygon1(MapProj* map, CDC* dc, int numverts,
                        degrees_t *plat, degrees_t *plon,
                        int color,  // code for color, -1 for XOR line
                        int backcolor,  // code for backcolor, -1 for none
                        int style,  // code for style
                        int line_width,  // line width in pixels, if < 0 in meters
                        int line_type,   // simple=1, rhumb=2, great_circle=3
                        int fill_color,
                        int fill_style);

   int geo_draw_polygon2(MapProj* map, CDC* dc, int numverts,
                        degrees_t *plat, degrees_t *plon,
                        int color,  // code for color, -1 for XOR line
                        int backcolor,  // code for backcolor, -1 for none
                        int style,  // code for style
                        int line_width,  // line width in pixels, if < 0 in meters
                        int *line_type,   // simple=1, rhumb=2, great_circle=3
                        int fill_color,
                        int fill_style);

   BOOL geo_line_hit_test(MapProj* map,
                  degrees_t lat1,
                  degrees_t lon1,
                  degrees_t lat2,
                  degrees_t lon2,
                  int line_width,  // line width in pixels
                  int line_type,  // UTIL_LINE_TYPE_SIMPLE, UTIL_LINE_TYPE_RHUMB, or UTIL_LINE_TYPE_GREAT
                  CPoint test_pt);

   BOOL point_in_geo_polygon(MapProj* map,
                     int numverts, // number points in polygon
                     int *line_type,   // array of types: simple=1, rhumb=2, great_circle=3
                     degrees_t *plat, degrees_t *plon, // polygon points
                     CPoint test_point);

   BOOL point_on_geo_polygon(MapProj* map,
                     int numverts, // number points in polygon
                     int *line_type,   // array of types: simple=1, rhumb=2, great_circle=3
                     degrees_t *plat, degrees_t *plon, // polygon points
                     CPoint test_point);

   BOOL point_in_and_on_geo_polygon(MapProj* map,
                     int numverts, // number points in polygon
                     int *line_type,   // array of types: simple=1, rhumb=2, great_circle=3
                     degrees_t *plat, degrees_t *plon, // polygon points
                     CPoint test_point);

   void invalidate_line(int x1, int y1, int x2, int y2);

   void invalidate_geo_line(MapProj* map,
                  degrees_t lat1,
                  degrees_t lon1,
                  degrees_t lat2,
                  degrees_t lon2,
                  int line_width,  // line width in pixels
                  int line_type);  // UTIL_LINE_TYPE_SIMPLE, UTIL_LINE_TYPE_RHUMB, or UTIL_LINE_TYPE_GREAT

   int invalidate_geo_polygon(MapProj* map, int numverts, degrees_t *plat, degrees_t *plon, int line_type);

   void invalidate_poly_list(CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*> & poly_list);

   void clear_poly_list(CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*> & poly_list);

   void un_dupe_poly_list(CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*> & poly_list);

   void geo_draw_line_offset(MapProj* map, CDC* dc,
                   degrees_t lat1,
                   degrees_t lon1,
                   degrees_t lat2,
                   degrees_t lon2,
                   int offx,
                   int offy,
                   int color,  // code for color, -1 for XOR line
                   int backcolor,  // code for backcolor, -1 for none
                   int style,  // code for style
                   int line_width);  // line width in pixels, if < 0 in meters

   void get_geo_draw_line_points(MapProj* map,
                   degrees_t lat1,
                   degrees_t lon1,
                   degrees_t lat2,
                   degrees_t lon2,
                   POINT *pt,
                   int *num_pts);

   // draw an ellipse based on pixels only
   void draw_simple_ellipse(MapProj* map, CDC* dc, double lat, double lon,
                     double vert, double horz,  // in pixels
                     double angle,
                     int line_color, int fill_color, int width,
                     int line_style, int fill_style, BOOL xor,
                     BOOL background, int bd_pt_cnt, POINT* bd_pt,
                     CRect * bounds);

   // draw an ellipse based on pixels only
   void draw_simple_ellipse_rgb(MapProj* map, CDC* dc, double lat, double lon,
                     double vert, double horz,  // in pixels
                     double angle,
                     COLORREF line_color, COLORREF fill_color, int width,
                     int line_style, int fill_style, BOOL xor,
                     BOOL background, int bd_pt_cnt, POINT* bd_pt,
                     CRect * bounds);

   // this version always draws the background line
   void draw_geo_ellipse(MapProj* map, CDC* dc,  // pointer to DC to draw in
                     double lat, // latitude of center
                     double lon, // longitude of center
                     double vert, // vertical axis in kilometers
                     double horz, // horizontal axis in kilometers
                     double angle, // angle of smaj in degrees from north CW
                     int color,  // color code of edge
                     int fill_color, // color code of fill
                     int width,  // width of edge in pixels
                     int line_style, // line style of edge
                     int fill_style, // type of fill
                     BOOL xor,  // TRUE = draw XOR, edge only
                     int bd_pt_cnt,  // IN number of points in polygon
                     POINT* bd_pt,  // OUT -- vertices of polygon
                     CRect * bounds  // OUT -- bounds in view coordinates
                     );

   // this version makes the background line optional
   void draw_geo_ellipse(MapProj* map, CDC* dc,  // pointer to DC to draw in
                     double lat, // latitude of center
                     double lon, // longitude of center
                     double vert, // vertical axis in kilometers
                     double horz, // horizontal axis in kilometers
                     double angle, // angle of smaj in degrees from north CW
                     int color,  // color code of edge
                     int fill_color, // color code of fill
                     int width,  // width of edge in pixels
                     int line_style, // line style of edge
                     int fill_style, // type of fill
                     BOOL xor,  // TRUE = draw XOR, edge only
                     BOOL background,  // TRUE = draw black background line
                     int bd_pt_cnt,  // IN number of points in polygon
                     POINT* bd_pt,  // OUT -- vertices of polygon
                     CRect * bounds  // OUT -- bounds in view coordinates
                     );

   // this version makes the background line optional
   void draw_geo_ellipse_rgb(MapProj* map, CDC* dc,  // pointer to DC to draw in
                     double lat, // latitude of center
                     double lon, // longitude of center
                     double vert, // vertical axis in kilometers
                     double horz, // horizontal axis in kilometers
                     double angle, // angle of smaj in degrees from north CW
                     COLORREF color,  // color of edge
                     COLORREF fill_color, // color of fill
                     int width,  // width of edge in pixels
                     int line_style, // line style of edge
                     int fill_style, // type of fill
                     BOOL xor,  // TRUE = draw XOR, edge only
                     BOOL background,  // TRUE = draw black background line
                     int bd_pt_cnt,  // IN number of points in polygon
                     POINT* bd_pt,  // OUT -- vertices of polygon
                     CRect * bounds  // OUT -- bounds in view coordinates
                     );

   void get_geo_ellipse_bounds(MapProj* map,
                     double lat, // latitude of center
                     double lon, // longitude of center
                     double vert, // semi-major axis in kilometers
                     double horz, // horizontal axis in kilometers
                     double angle, // angle of smaj in degrees from north CW
                     CRect * bounds,  // OUT -- bounds in view coordinates
                     d_geo_t * ll,  // OUT -- lower left geo point
                     d_geo_t * ur  // OUT -- upper right geo point
                     );

   int get_geo_polygon_bounds(int numverts, double *lat, double *lon,
                              d_geo_t * ll, d_geo_t * ur, double *lon_extent);

   void draw_geo_ellipse_edged(MapProj* map,
                     CDC* dc,
                     double lat, // latitude of center
                     double lon, // longitude of center
                     double vert, // semi-major axis in kilometers
                     double horz, // horizontal axis in kilometers
                     double angle, // angle of smaj in degrees from north CW
                     int color,  // color code of edge
                     int width,  // width of edge in pixels
                     int bd_pt_cnt,  // IN number of points in polygon
                     POINT* bd_pt,  // OUT -- vertices of polygon
                     CRect * bounds  // OUT -- bounds in view coordinates
                     );

   void draw_geo_ellipse_edged_rgb(MapProj* map,
                     CDC* dc,
                     double lat, // latitude of center
                     double lon, // longitude of center
                     double vert, // semi-major axis in kilometers
                     double horz, // horizontal axis in kilometers
                     double angle, // angle of smaj in degrees from north CW
                     COLORREF color,  // color code of edge
                     int width,  // width of edge in pixels
                     int bd_pt_cnt,  // IN number of points in polygon
                     POINT* bd_pt,  // OUT -- vertices of polygon
                     CRect * bounds  // OUT -- bounds in view coordinates
                     );

   double get_oval_width(MapProj* map, double lat, double lon,
                     double vert, // vertical axis in kilometers
                     double horz, // horizontal axis in kilometers
                     double angle);  // angle of smaj in degrees from north CW

   BOOL polygon_is_clockwise(degrees_t *plat, degrees_t *plon, int ptcnt);

   BOOL point_in_polygon(double testlat, double testlon, double *plat, double *plon, int numverts);

   // this routine assumes that the polygon intersects the screen
   // it also assumes that the polgon is less wide than half the earth
   int geo_polygon_to_view(MapProj* map, int numverts,
                       degrees_t *plat, degrees_t *plon,
                       POINT *pt);

   int geo_polygon_to_view(MapProj* map, int numverts,
                     int line_type,   // simple=1, rhumb=2, great_circle=3
                     degrees_t *plat, degrees_t *plon, int *num_points, POINT **vpt);

   int geo_polygon_to_view(MapProj* map, int numverts,
                           degrees_t *plat, degrees_t *plon,
                           CList<C_ovlkit_screen_polygon*, C_ovlkit_screen_polygon*> & polygon_list);

   int geo_polygon_to_view(MapProj* map, int numverts,
                              int *line_type,   // simple=1, rhumb=2, great_circle=3
                              degrees_t *plat, degrees_t *plon, int *num_points, POINT **vpt);

   BOOL geo_polygon_on_screen(MapProj* map, int numverts, degrees_t *plat, degrees_t *plon);

   void add_line_to_polygon_list(POINT start, POINT end,
         CList<C_ovlkit_screen_polygon*, C_ovlkit_screen_polygon*> & plist);

   void get_geo_line_coordinates(MapProj *map,
                   d_geo_t geo1, d_geo_t geo2,
                   CPoint &p1, CPoint &p2,
                         CPoint &p1_wrap, CPoint &p2_wrap);

   void DrawBasicButton(CDC * dc, RECT rc);

   int percent_world_in_view(MapProj* map);
   int geo_to_surface(MapProj* map, double lat, double lon, int *x, int *y);

   CString get_temp_path();
   CString get_temp_jpeg_name();
   int get_jpeg_size(CString filename, int & image_width, int & image_height, int & jpeg_size);
   void add_carets(CString &num);
   int imagelib_load(CString filename, int *width, int *height, CString & error_msg);
   int get_imagelib_subimage(  CString filename, double factor,
                        int image_offset_x, int image_offset_y,
                        int width, int height,
                        unsigned char *red_array,
                        unsigned char *green_array,
                        unsigned char *blue_array,
                        CString & error_msg);
   int save_imagelib_image(CString filename, int width, int height, BYTE *img_data, int quality, CString &error_msg);

   // base 64 functions
   static char base64_encode_char(unsigned char uc);
   static unsigned char base64_decode_char(char c);
   static bool is_char_base64(char c);
   static int base64_encode(const std::vector<unsigned char> & data, std::string & basestr);
   static std::vector<unsigned char> base64_decode(const std::string & base_str);
};
