// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.



#pragma once

#ifndef FVWUTIL_H
#define FVWUTIL_H 1

#include "FvCoreMFC/Include/polyxy.h"
#include "utils.h"
#include "polyline.h"
#include "..\ovlelem\bldlist.h"

////////////////////////////////////////////
//  Colors definition
////////////////////////////////////////////
#define RS_NUM_COLORS 8                     // Number of colors available

#define RS_COLOR_BRIGHT_BLACK              0    // RGB(  0,  0,  0)
#define RS_COLOR_BRIGHT_RED               13    // RGB(255,  0,  0)
#define RS_COLOR_BRIGHT_GREEN             14    // RGB(  0,255,  0)
#define RS_COLOR_BRIGHT_YELLOW            15    // RGB(255,255,  0)
#define RS_COLOR_BRIGHT_BLUE              16    // RGB(  0,  0,255)
#define RS_COLOR_BRIGHT_MAGENTA           17    // RGB(255,  0,255)
#define RS_COLOR_BRIGHT_CYAN              18    // RGB(  0,255,255)
#define RS_COLOR_BRIGHT_WHITE             19    // RGB(255,255,255)

// Forward declaration
class C_overlay;

typedef struct
{
   char code[3];
   char abbrev[3];
   char name[41];
} util_state_t;

typedef struct
{
   char code[3];
   char name[41];
} util_country_t;

class C_vertex
{
public:

   C_vertex(){};
   virtual ~C_vertex(){};

   double m_lat;
   double m_lon;
};

// ************************************************************
// ************************************************************

class C_3d_vertex : public C_vertex
{
public:

   C_3d_vertex(){};
   virtual ~C_3d_vertex(){};

   double m_alt;
};

// ************************************************************
// ************************************************************

// forward declaration
class CPoly_vertex;

class CFvwUtil
{
private:
   CFvwUtil();
   static CFvwUtil *m_instance;

public:

   static void create_instance() { m_instance = new CFvwUtil; }
   static CFvwUtil *get_instance() { return m_instance; }
   static void destroy_instance() { delete m_instance; }

   util_state_t *m_state;
   util_state_t *m_country;
   int m_statecnt;
   int m_countrycnt;
   int m_country_pos;

   void pixels_to_km(MapProj* map, int x1, int y1, int x2, int y2,
                     double *distx, double *disty);

   void add_tab(char * tstr);

   // make a string a specified length, truncate or add spaces
   void set_text_length(char * tstr, int setlen);

   // get width in pixels of line specified by meters width
   int get_calculated_width(MapProj* map, int meters_wide, double lat, double lon);

   // Function to lookup a DAFIF value from a Fix/Point string.
   long db_lookup(CString &string, SnapToInfo &db_info,
      boolean_t fix_point_search);

   // construct an array of points that define an ellipse
   void construct_oval
   (
      int x, int y,  // center of oval
      int major,     // semi-major axis in pixels
      int minor,     // semi-minor axis in pixels
      double angle,  // CW rotation of oval
      int num_vertex,// number of vertices in polygon
      POINT* pt      // array of points for polygon
   );

   // convert an angle in degrees to be from 0 to 359 or
   int normalize_angle(int angle);
   double normalize_radians(double angle);

   // convert geo to within limits
   int normalize_geo(double *lat, double *lon);

   // convert an north up heading to a cartesian angle and vice versa
   double heading_to_cartesian_angle(double heading);
   double cartesian_angle_to_heading(double angle);

   // convert an angle in degrees to be from 0 to 359.9
   double normalize_angle(double angle);
   BOOL geo2text(double lat, double lon, char* slat, int slat_len, char* slon, int slon_len);
   BOOL text2geo(char* slat, char* slon, double* lat, double* lon);
   BOOL point_in_polygon(CPoint point, POINT *pgon, int numverts);
   BOOL point_on_polygon(CPoint point, int threshold, POINT *pgon, int numverts);
   BOOL polygon_is_clockwise(POINT *pgon, int numverts);
   BOOL polygon_is_clockwise2(POINT *p, int numverts);
   BOOL polygon_is_clockwise(degrees_t *lat, degrees_t *lon, int numverts);
   int point_distance(CPoint pt1, CPoint pt2);
   double line_angle(CPoint &start_pt, CPoint &end_pt);

   double line_angle(int x1, int y1,// start point of line
                     int x2, int y2); // end point of line

   double line_angle(double x1, double y1, // start point of line
                     double x2, double y2); // end point of line
   void point_to_vector(int start_x, int start_y,
                        int end_x, int end_y,
                        int *magnitude, double *angle);
   void vector_to_point(int start_x, int start_y,
                        int magnitude, double angle,
                        int *end_x, int *end_y);

   int magnitude(int x1, int y1, int x2, int y2);
   double magnitude(double x1, double y1, double x2, double y2);

   BOOL CFvwUtil::line_segment_intersection( double ax1, double ay1, double ax2, double ay2,
                                 double bx1, double by1, double bx2, double by2,
                                 double *cx, double *cy);

   BOOL geo_line_segment_intersection( double lat11, double lon11, double lat12, double lon12,
                              double lat21, double lon21, double lat22, double lon22,
                              double *xlat, double *xlon);

   void rotate_pt
   (
      int oldx, int oldy,  // original point
      int *newx, int *newy,// new point
      double ang,          // angle in degrees
      int ctrx, int ctry   // center point
   );

   void rotate_pt
   (
      int oldx, int oldy,  // original point
      long *newx, long *newy,// new point
      double ang,          // angle in degrees
      int ctrx, int ctry   // center point
   );

   // get the perpendicular distance from a point to a line
   int distance_to_line
   (
      int x1, int y1,// pt on the line
      int x2, int y2,// pt on the line
      int xp, int yp // pt to test
   );

   // get the perpendicular distance from a point to a line using doubles
   double distance_to_line
   (
      double x1, double y1,//pt on the line
      double x2, double y2,//pt on the line
      double xp, double yp //pt to test
   );

   // get the point on a line that is closest to an exterior point
   int closest_point_on_line (    int ax, int ay,  /* pt on the line */
                                    int bx, int by,  /* pt on the line */
                                    int tx, int ty,  /* pt to test */
                           int *cx, int *cy );  // closest point on line

   int distance_to_geo_line(  MapProj* map,
                              degrees_t lat1, degrees_t lon1,  // coords of start point
                              degrees_t lat2, degrees_t lon2,  // coords of end point
                              int px, int py);  // point to test
   double limit(double num, double min, double max);
   int limit(int num, int min, int max);
   int limit_to_short(int num);

   // this function uses MAP_geo_to_view to convert coordinates, them limits to short
   int geo_to_view_short(MapProj* map, degrees_t lat, degrees_t lon, int *x, int *y);

   // the following functions call APIs after limiting ranges to short values
   // this is to fix problems with Win 95's handling of longer than short params
   void move_to(CDC* dc, int x, int y);
   void line_to(CDC* dc, int x, int y);
   void rectangle(CDC* dc, int x1, int y1, int x2, int y2);
   void rectangle(CDC* dc, LPCRECT rc);
   void arc(CDC* dc, CPoint ctr, CPoint p1, CPoint p2, int color, int width, BOOL xor);
   void ellipse(CDC* dc, int x1, int y1, int x2, int y2);
   void ellipse(CDC* dc, LPCRECT rc);
   void polygon(CDC* dc, POINT *pt, int numpt);

   // fills the interior of a polygon
   void fill_polygon(CDC* dc, POINT* fill_pnt_list, int num_points,
                       COLORREF fill_color, int fill_type,
                     int surface_width = 0, int surface_height = 0);
   void fill_polygon(CDC* dc, POINT *pnt_list, int num_points,
                       int fill_color, int fill_type,
                     int surface_width = 0, int surface_height = 0);

   // polygon draw with lines (no fill)
   void empty_polygon(CDC* dc, POINT *pt, int numpt);
   void empty_polygon(CDC* dc, POINT *pt, int numpt, int color, int width);
   void polyline(CDC* dc, POINT *pnt_list, int num_points, int color, int width);

   void line_xor(CDC* dc, int width, CPoint pt1, CPoint pt2);
   void rectangle_xor(CDC* dc, int width, CPoint pt1, CPoint pt2);

   boolean_t is_valid_color(int color);
   COLORREF code2color(int code);
   int color2code(COLORREF color);
   int code2palette(int color);
   int code2fill(int code);
   COLORREF string_to_color(CString color);
   CString color_to_string(COLORREF color);
   int linear_color_to_fvw_color(int lin_color);
   int fvw_color_to_linear_color(int fvw_color);

   // returns a color that is the dark equivalent of the given color
   int get_dark_color(int color);

   void parse_the_text( __in CString text, __inout CList<CString *, CString *> &text_list);
   void set_BSTR(CString str, BSTR * bstr);

   // compute the corners of a polygon (rotated rectangle) that encloses a block of text
   //
   //      0*****1
   //      *     *
   //      *     *
   //      *     *
   //      3*****2
   //
   //      corners returned
   void compute_text_poly( int tx, int ty,         // x/y of anchor point
                           int anchor_pos,         // position of anchor point
                           int width, int height,  // height and width of text
                           double text_angle,      // angle of text
                           POINT *cpt              // OUT - corners of rectangle enclosing text
                           );

   // get the size of a text string in particular font, size, and atributes
   void get_text_size
   (
      CDC *dc,             // pointer to DC to draw in
      CString text,        // text to draw
      CString font_name,   // font name
      int font_size,       // font size in points
      int font_attrib,     // font attributes (bold, italic, etc)
      int *width,          //  OUT - width of drawn text
      int *height,         //  OUT - height of drawn text
      BOOL pad_spaces=TRUE, // default is TRUE
      double angle = 0.0   // default is 0
   );

   void draw_text
   (
      CDC *dc,             // pointer to DC to draw in
      CString text,        // text to draw
      int x, int y,        // screen x,y position
      int anchor_pos,      // reference position of text
      CString font,        // font name
      int font_size,       // font size in points
      int font_attrib,     // font attributes (bold, italic, etc)
      int background,      // background type
      int text_color,      // code for text color
      int back_color,      // code for background color
      double angle,        // angle of text
      POINT *cpt,          // 4 point array defining the text polygon corners
      BOOL pad_spaces=TRUE // default is TRUE
   );

   static void draw_text_RGB(
               CDC *dc,            // pointer to DC to draw in
               CString text,       // text to draw
               int x, int y,       // screen x,y position
               int anchor_pos,     // reference position of text
               CString font,       // font name
               int font_size,      // font size in points
               int font_attrib,    // font attributes (bold, italic, etc)
               int background,     // background type
               COLORREF color,     // RGB for text color
               COLORREF bkcolor,   // RGB for background color
               double angle,       // angle of text
               POINT *cpt,         // 4 point array defining the text polygon corners
               BOOL pad_spaces = TRUE    // default is TRUE
            );

   // draw a multi-line text box
   // RESTRICTIONS: (as of 4/10/97)
   //    1. Only an angle of 0.0 is supported
   //    2. Only backgrounds of UTIL_BG_RECT and UTIL_BG_3D are supported

   void draw_multi_line_text
   (
      CDC *dc,             // pointer to DC to draw in
      CString text,        // text to draw
      int x, int y,        // screen x,y position
      int anchor_pos,      // reference position of text
      CString font,        // font name
      int font_size,       // font size in points
      int font_attrib,     // font attributes (bold, italic, etc)
      int background,      // background type
      int text_color,      // code for text color
      int back_color,      // code for background color
      double angle,        // angle of text
      POINT *cpt,          // 4 point array defining the text polygon corners
      BOOL pad_spaces=TRUE // default is TRUE
   );

   void draw_multi_line_text_RGB(CDC *dc, CString text, int x, int y,
      int anchor_pos, CString font, int font_size, int font_attrib,
      int background, COLORREF text_color, COLORREF back_color,
      double angle, POINT *cpt, BOOL pad_spaces=TRUE);

   // draws multi line text with the text aligned within the text box
   // the alignment parameter can be one of the following
   //     UTIL_TEXT_MULTI_ALIGN_LEFT
   //     UTIL_TEXT_MULTI_ALIGN_CENTER
   //     UTIL_TEXT_MULTI_ALIGN_RIGHT

   void draw_multi_line_text_aligned
   (
      CDC *dc,             // pointer to DC to draw in
      CString text,        // text to draw
      int x, int y,        // screen x,y position
      int anchor_pos,      // reference position of text
      CString font,        // font name
      int font_size,       // font size in points
      int font_attrib,     // font attributes (bold, italic, etc)
      int background,      // background type
      int text_color,      // code for text color
      int back_color,      // code for background color
      double angle,        // angle of text
      int alignment,       // alignment of text within box
      POINT *cpt,          // 4 point array defining the text polygon corners
      BOOL pad_spaces=TRUE // default is TRUE
   );

   void draw_multi_line_text_aligned_RGB(
      CDC *dc,          // pointer to DC to draw in
      CString text,     // text to draw
      int x, int y,     // screen x,y position
      int anchor_pos,   // reference position of text
      CString font,     // font name
      int font_size,    // font size in points
      int font_attrib,  // font attributes (bold, italic, etc)
      int background,   // background type
      COLORREF color,   // RGB value for foreground color
      COLORREF bkcolor,   // RGB value for background color
      double angle,     // angle of text
      int alignment,    // alignment of text within box
      POINT *cpt,       // 4 point array defining the text polygon corners
      BOOL pad_spaces = TRUE   // default is TRUE
   );

   // get the couners of a text block without drawing it
   // RESTRICTIONS: (as of 4/11/97)
   //    1. Only an angle of 0.0 is supported
   //    2. Only backgrounds of UTIL_BG_RECT and UTIL_BG_3D are supported
   void get_multi_line_bounds
   (
      CDC *dc,             // pointer to DC to draw in
      CString text,        // text to draw
      int x, int y,        // screen x,y position
      int anchor_pos,      // reference position of text
      CString font,        // font name
      int font_size,       // font size in points
      int font_attrib,     // font attributes (bold, italic, etc)
      int background,      // background type
      double angle,        // angle of text
      POINT *cpt,          // 4 point array defining the text polygon corners
      BOOL pad_spaces=TRUE // default is TRUE
   );

   BOOL geo_line_to_xy(MapProj* map,
                            degrees_t lat1,
                            degrees_t lon1,
                            degrees_t lat2,
                            degrees_t lon2,
                            int *num_line,  // number of lines (1 or 2)
                            int *px1, int *py1,
                            int *px2, int *py2,
                            int *px3, int *py3,
                            int *px4, int *py4);

   /*
   void draw_line
   (
      MapProj* map,
      CDC* dc,
      int x1, int y1,// beginning point
      int x2, int y2,// ending point
      int color,     // code for color
      int style,     // code for style
      int width      // line width
   );
   */

   // compute the bounding rectangle for a line from point 1 to point 2 with the
   // given style and width
   CRect get_bounding_rect(int x1, int y1, int x2, int y2, int style, int width);

   // draw ellipse with black background
   void draw_geo_ellipse
   (
      MapProj* map,
      CDC* dc,
      double lat,    // latitude of center
      double lon,    // longitude of center
      double vert,   // vertical axis in kilometers
      double horz,   // horizontal axis in kilometers
      double angle,  // angle of smaj in degrees from north CW
      int color,     // color code of edge
      int fill_color,// color code of fill
      int width,     // width of edge in pixels
      int line_style,// line style of edge
      int fill_style,// type of fill
      BOOL xor,      // TRUE = draw XOR, edge only
      int bd_pt_cnt, // IN number of points in polygon
      POINT* bd_pt,  // OUT -- vertices of   polygon
      CRect * bounds // OUT -- bounds in view coordinates
   );

   // draw ellipse using straight lines with black background
   void draw_geo_ellipse_simple
   (
      MapProj* map,
      CDC* dc,
      double lat,    // latitude of center
      double lon,    // longitude of center
      double vert,   // vertical axis in kilometers
      double horz,   // horizontal axis in kilometers
      double angle,  // angle of smaj in degrees from north CW
      int color,     // color code of edge
      int fill_color,// color code of fill
      int width,     // width of edge in pixels
      int line_style,// line style of edge
      int fill_style,// type of fill
      BOOL xor,      // TRUE = draw XOR, edge only
      int bd_pt_cnt, // IN number of points in polygon
      POINT* bd_pt,  // OUT -- vertices of   polygon
      CRect * bounds // OUT -- bounds in view coordinates
   );

   // draw ellipse without black background
   void draw_geo_ellipse2
   (
      MapProj* map,
      CDC* dc,
      double lat, // latitude of center
      double lon, // longitude of center
      double vert, // vertical axis in kilometers
      double horz, // horizontal axis in kilometers
      double angle, // angle of smaj in degrees from north CW
      int color,    // color code of edge
      int fill_color, // color code of fill
      int width,      // width of edge in pixels
      int line_style, // line style of edge
      int fill_style, // type of fill
      BOOL xor,       // TRUE = draw XOR, edge only
      int bd_pt_cnt,   // IN number of points in polygon
      POINT* bd_pt,    // OUT -- vertices of polygon
      CRect * bounds  // OUT -- bounds in view coordinates
   );

   void get_geo_ellipse_bounds
   (
      MapProj* map,
      double lat, // latitude of center
      double lon, // longitude of center
      double vert, // semi-major axis in kilometers
      double horz, // horizontal axis in kilometers
      double angle, // angle of smaj in degrees from north CW
      CRect * bounds,  // OUT -- bounds in view coordinates
      d_geo_t * ll,  // OUT -- lower left geo point
      d_geo_t * ur   // OUT -- upper right geo point
   );

   // compute the center of a less than half world wide geo box
   void compute_center_geo(d_geo_t ll, d_geo_t ur, degrees_t *lat, degrees_t *lon);

   boolean_t clip_t(int denom, int num, float *tE, float *tL);
   int clip_point(int x, int y, CRect rc);

   // returns the intersection with a the rect bounds given the two
   // endpoint of a line
   CPoint get_intersection(CPoint p1, CPoint p2, CRect rect);

   // returns TRUE for visible and FALSE otherwise
   boolean_t clip_line
   (
      int *x1, int *y1, // first point in line
      int *x2, int *y2, // last point in line
      CRect rc          // rectangle to clip to
   );

   // attempt to execute the file using its associated program,
   // return error code if fail
   // WARNING -- some applications cause an error to be reported even if they
   //            perform properly
   BOOL shell_execute(char * filename, int* error_code);
   BOOL shell_execute(char * filename, char * parameters, int* error_code);
   // display error code of shell_execute
   BOOL shell_execute_error_display(int error_code);

   int ensure_directory(const char *path);

   void build_string(char* outstr, char* instr, int num);
   void remove_leading_zeros(char *txt);
   // convert a CString to a string
   void cstring_to_string(CString in, char *out, int max_char_len);

   BOOL find_country(char *code, char *name);
   BOOL get_first_country(char *name, const int NAME_LEN, char *code, const int CODE_LEN);
   BOOL get_next_country(char *name, const int NAME_LEN, char *code, const int CODE_LEN);
   BOOL find_state(char *code, char *name);
   BOOL find_state_abbrev(char *code, char *abbrev);
   // determines if a point is in a client window
   boolean_t is_point_in_client_area(CWnd* view, CPoint pt);

   // converts a color from the route server to a FalconView color
   int rscolor2color(short color);

   // converts a FalconView color to a route server color
   short color2rscolor(int color);

   // rounds num to the resolution of res (1, 0.1, 0.01, 0.001)
   // returns num on error
   double round_to_res(double num, double res);

   // round to nearest integer
   int round(double val);

   // Returns TRUE if the application specified by its window
   // name is open and not minimized
   boolean_t GetExternalApplicationIsOpen(CString windowName, HWND *hWnd,
     boolean_t minimized_is_closed = TRUE);

    // open/close CFPS executable
   void open_cfps(void);
   void close_cfps(void);

   // converts a float to a string with the given number of decimal places.
   // performs rounding
   CString ftoa(double value, int decimal_places);

   void draw_geo_line(MapProj* map, CDC* dc,
                            degrees_t lat1,
                            degrees_t lon1,
                            degrees_t lat2,
                            degrees_t lon2,
                            int color,       // code for color, -1 for XOR line
                            int style,       // code for style
                            int width,       // line width in pixels
                            int line_type,   // simple=1, rhumb=2, great_circle=3
                            BOOL normal,     // draw normal line
                            BOOL background,
                            int bgcolor = UTIL_COLOR_BLACK);   // draw background line

   void draw_geo_line(MapProj* map, CDC* dc,
                            degrees_t lat1,
                            degrees_t lon1,
                            degrees_t lat2,
                            degrees_t lon2,
                            COLORREF color_rgb, // RGB color
                            int style,          // code for style
                            int width,          // line width in pixels
                            int line_type,         // simple=1, rhumb=2, great_circle=3
                            BOOL normal,        // draw normal line
                            BOOL background,
                            COLORREF bgcolor_rgb = RGB(0, 0, 0));    // draw background line

   void draw_geo_line2(MapProj* map, CDC* dc,
                            degrees_t lat1,
                            degrees_t lon1,
                            degrees_t lat2,
                            degrees_t lon2,
                            COLORREF color_rgb, // RGB color
                            int style,          // code for style
                            int width,          // line width in pixels
                            int line_type,         // simple=1, rhumb=2, great_circle=3
                            BOOL normal,        // draw normal line
                            BOOL background, COLORREF bgcolor_rgb);     // draw background line

   void draw_geo_line2(MapProj* map, CDC* dc,
                       IGeographicContourIterator &contour,
                       COLORREF color_rgb,               // RGB color
                       int style,                     // code for style
                       int width,                     // line width in pixels
                       BOOL normal,                   // draw normal line
                       BOOL background, COLORREF bgcolor_rgb);                // draw background line

   int draw_geo_text_line_vertex_offset(MapProj* map, CDC* dc,
                           degrees_t *lat,   // array of lats
                           degrees_t *lon,   // arrat if longs
                           int ptcnt,        // number of vertexes
                           int color,        // code for color, -1 for XOR line
                           int style,        // code for style
                           int width,        // line width in pixels
                           int line_type,   // simple=1, rhumb=2, great_circle=3
                           CString text,     // text for line vertexe
                           CString font,       // font name
                           int font_size,      // font size in points
                           int font_attrib,    // font attributes (bold, italic, etc)
                           int font_fg_color,   // font main color
                           int font_bg_color,   // font background color
                           int font_bg_type, // font background type
                           BOOL normal,      // draw normal line
                           BOOL background,  // draw background line
                           CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list);  // text polygons

   int draw_geo_text_line_vertex(MapProj* map, CDC* dc,
                           degrees_t *lat,   // array of lats
                           degrees_t *lon1,  // arrat if longs
                           int ptcnt,        // number of vertexes
                           int color,        // code for color, -1 for XOR line
                           int style,        // code for style
                           int width,        // line width in pixels
                           int line_type,   // simple=1, rhumb=2, great_circle=3
                           BOOL polygon,     // true if line is a polygon
                           CString text,     // text for line vertexe
                           CString font,       // font name
                           int font_size,      // font size in points
                           int font_attrib,    // font attributes (bold, italic, etc)
                           int font_fg_color,   // font main color
                           int font_bg_color,   // font background color
                           int font_bg_type, // font background type
                           BOOL normal,      // draw normal line
                           BOOL background,  // draw background line
                           CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list);  // text polygons

   // returns SUCCESS if all text was draw, FAILURE is no text was drawn, 1 otherwise
   int draw_geo_text_line_center(MapProj* map, CDC* dc,
                           degrees_t *lat,   // array of lats
                           degrees_t *lon1,  // arrat if longs
                           int ptcnt,        // number of vertexes
                           COLORREF color,      // RGB color, -1 for XOR line
                           COLORREF bgcolor,  // RGB background color
                           int style,        // code for style
                           int width,        // line width in pixels
                           int line_type,   // simple=1, rhumb=2, great_circle=3
                           BOOL polygon,     // true if line is a polygon
                           CString text,     // text for line vertexe
                           CString font,       // font name
                           int font_size,      // font size in points
                           int font_attrib,    // font attributes (bold, italic, etc)
                           int font_fg_color,   // font main color
                           int font_bg_color,   // font background color
                           int font_bg_type, // font background type
                           int text_type,    // auto = 1, top = 2, or bottom = 3
                           BOOL normal,      // draw normal line
                           BOOL background,  // draw background line
                           CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list);  // text polygons

   // same function with code for color
   int draw_geo_text_line_center(MapProj* map, CDC* dc,
                           degrees_t *lat,   // array of lats
                           degrees_t *lon,   // arrat if longs
                           int ptcnt,        // number of vertexes
                           int color,        // code for color, -1 for XOR line
                           int bg_color,     // code for background color
                           int style,        // code for style
                           int width,        // line width in pixels
                           int line_type,    // simple=1, rhumb=2, great_circle=3
                           BOOL polygon,     // true if line is a polygon
                           CString text,     // text for line vertexe
                           CString font,       // font name
                           int font_size,      // font size in points
                           int font_attrib,    // font attributes (bold, italic, etc)
                           int font_fg_color,   // font main color
                           int font_bg_color,   // font background color
                           int font_bg_type, // font background type
                           int text_type,    // auto = 0, top = 1, or bottom = 2
                           BOOL normal,      // draw normal line
                           BOOL background,  // draw background line
                           CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list);  // text polygons

   // same function with RGB for font
   int draw_geo_text_line_center(MapProj* map, CDC* dc,
                           degrees_t *lat,   // array of lats
                           degrees_t *lon,   // arrat if longs
                           int ptcnt,        // number of vertexes
                           COLORREF color,    // RGB color, -1 for XOR line
                           COLORREF bgcolor,  // RGB background color
                           int style,        // code for style
                           int width,        // line width in pixels
                           int line_type,   // simple=1, rhumb=2, great_circle=3
                           BOOL polygon,     // true if line is a polygon
                           CString text,     // text for line vertexe
                           CString font,       // font name
                           int font_size,      // font size in points
                           int font_attrib,    // font attributes (bold, italic, etc)
                           COLORREF font_fg_color, // RGB font main color
                           COLORREF font_bg_color, // RGB font background color
                           int font_bg_type, // font background type
                           int text_type,    // auto = 0, top = 1, or bottom = 2
                           BOOL normal,      // draw normal line
                           BOOL background,  // draw background line
                           CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*>  &poly_list);  // text polygons

   int wgs84_to_current_datum(double inlat, double inlon, double *outlat, double *outlon);
   int current_datum_to_wgs84(double inlat, double inlon, double *outlat, double *outlon);

   /// returns the width and height of the desktop window
   void get_screen_size(UINT *screen_width, UINT *screen_height);

   /// shifts the given rect so that no part of the rect (if possible) is off
   /// the screen
   void make_rect_visible_on_screen(CRect &rect);

   CString get_path_from_user(const CString &title, const CString &initialDirectory = "");
};

// ********************************************************************


// ********************************************************************
// CFvwTextList - class to hold output from CFvwUtil::parse_the_text()
// ********************************************************************
class CFvwTextLineList
{
private:
   CList< CString*, CString* >   m_lpcsTextLines;
   POSITION                      m_pos;
   CFvwTextLineList();         // Inaccessible, must initialize with strings

public:
   CFvwTextLineList( const CString& csTextLines )
   {
      CFvwUtil::get_instance()->parse_the_text( csTextLines, m_lpcsTextLines );
      GetHeadPosition();
   }

   ~CFvwTextLineList()
   {
      while( !m_lpcsTextLines.IsEmpty() )
         delete  m_lpcsTextLines.RemoveTail();
   }

   POSITION GetHeadPosition()
   {
      return m_pos = m_lpcsTextLines.GetHeadPosition();
   }

   const CString& GetNext()
   {
      return *m_lpcsTextLines.GetNext( m_pos );
   }

   BOOL IsMore()
   {
      return m_pos != NULL;
   }

   INT GetCount()
   {
      return m_lpcsTextLines.GetCount();
   }
}; // class CFvwTextLineList
#endif
