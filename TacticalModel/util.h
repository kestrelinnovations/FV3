// util.h

#pragma once

#include "geo_tool_d.h"

#define	GEO_A_over_B     1 //- top edge of B intersects A
#define	GEO_B_over_A     2 //- top edge of A intersects B
#define	GEO_A_left_B     4 //- left edge of B intersects A
#define	GEO_B_left_A     8 //- left edge of A intersects B
#define	GEO_A_in_B_lat  16 //- latitudes of A completely in B
#define	GEO_A_in_B_lon  32 //- longitudes of A completely in B
#define	GEO_A_in_B_all  48 //- A completely in B
#define	GEO_B_in_A_lat  64 //- latitudes of B completely in A
#define	GEO_B_in_A_lon 128 //- longitudes of B completely in A
#define	GEO_B_in_A_all 192 //- B completely in A

/*
#define UTIL_COLOR_BLACK		   0
#define UTIL_COLOR_DARK_RED	   1
#define UTIL_COLOR_DARK_GREEN		2
#define UTIL_COLOR_DARK_YELLOW	3
#define UTIL_COLOR_DARK_BLUE		4 
#define UTIL_COLOR_DARK_MAGENTA	5
#define UTIL_COLOR_DARK_CYAN		6
#define UTIL_COLOR_LIGHT_GRAY	   7
#define UTIL_COLOR_MONEY_GREEN	8
#define UTIL_COLOR_SKY_BLUE		9
#define UTIL_COLOR_CREAM		   10
#define UTIL_COLOR_MEDIUM_GRAY	11
#define UTIL_COLOR_DARK_GRAY		12
#define UTIL_COLOR_RED		      13
#define UTIL_COLOR_GREEN		   14
#define UTIL_COLOR_YELLOW	      15
#define UTIL_COLOR_BLUE		      16
#define UTIL_COLOR_MAGENTA	      17
#define UTIL_COLOR_CYAN		      18
#define UTIL_COLOR_WHITE		   19

// fill types
#define UTIL_FILL_NONE			0
#define UTIL_FILL_HORZ			1
#define UTIL_FILL_VERT			2
#define UTIL_FILL_BDIAG			3
#define UTIL_FILL_FDIAG			4
#define UTIL_FILL_CROSS			5
#define UTIL_FILL_DIAGCROSS	6
#define UTIL_FILL_SOLID			7
#define UTIL_FILL_SHADE			8

// anchor postion type
#define UTIL_UPSIDE_DOWN           0
#define UTIL_ANCHOR_LOWER_LEFT     1
#define UTIL_ANCHOR_UPPER_LEFT     2
#define UTIL_ANCHOR_LOWER_CENTER   3
#define UTIL_ANCHOR_UPPER_CENTER   5
#define UTIL_ANCHOR_LOWER_RIGHT    6
#define UTIL_ANCHOR_UPPER_RIGHT    8
#define UTIL_ANCHOR_CENTER_LEFT   10
#define UTIL_ANCHOR_CENTER_RIGHT  11
#define UTIL_ANCHOR_CENTER_CENTER 12

// font attributes
#define UTIL_FONT_BOLD       1
#define UTIL_FONT_ITALIC     2
#define UTIL_FONT_UNDERLINE  4
#define UTIL_FONT_STRIKEOUT  8

// text background type
#define UTIL_BG_NONE         0
#define UTIL_BG_SHADOW       1
#define UTIL_BG_RECT         2
#define UTIL_BG_3D           3

// font names
#define UTIL_FONT_NAME_ARIAL			"Arial"
#define UTIL_FONT_NAME_ARIAL_BLACK	"Arial Black"
#define UTIL_FONT_NAME_COURIER		"Courier New"
#define UTIL_FONT_NAME_TIMES			"Times"

#define UTIL_FIL_READ_OK   4
#define UTIL_FIL_WRITE_OK  2
#define UTIL_FIL_EXISTS    0

// bit flags for clip_point.
#define UTIL_CLIP_TOP		0x01
#define UTIL_CLIP_BOTTOM	0x02
#define UTIL_CLIP_LEFT		0x04
#define UTIL_CLIP_RIGHT		0x08

// line style
#define UTIL_LINE_SOLID				PS_SOLID
#define UTIL_LINE_DOT				PS_DOT
#define UTIL_LINE_DASH				PS_DASH
#define UTIL_LINE_DASHDOT			PS_DASHDOT
#define UTIL_LINE_DASHDOTDOT		PS_DASHDOTDOT
#define UTIL_LINE_RAILROAD			101
#define UTIL_LINE_POWERLINE		102
#define UTIL_LINE_ZIGZAG			103
#define UTIL_LINE_ARROW_START		104
#define UTIL_LINE_ARROW_END		105
#define UTIL_LINE_FEBA_START		106
#define UTIL_LINE_FEBA_END			107
#define UTIL_LINE_FLOT_START		108
#define UTIL_LINE_FLOT_END			109
#define UTIL_LINE_DELETE			110

#define UTIL_LINE_ROUND_DOT		111
#define UTIL_LINE_SQUARE_DOT		112   
#define UTIL_LINE_DASH2				113
#define UTIL_LINE_DASH_DOT			114
#define UTIL_LINE_LONG_DASH		115
#define UTIL_LINE_TMARK				116
#define UTIL_LINE_DIAMOND			117
#define UTIL_LINE_ARROW				118
#define UTIL_LINE_DASH_EXT			119
#define UTIL_LINE_DASH_EXT2		120
#define UTIL_LINE_BORDER_START	121
#define UTIL_LINE_BORDER_END		122
#define UTIL_LINE_NOTCHED        123
#define UTIL_LINE_FLOT2_START		124
#define UTIL_LINE_FLOT2_END		125
#define UTIL_LINE_FLOT3_START		126
#define UTIL_LINE_FLOT3_END		127
#define UTIL_LINE_FEBA2_START		128
#define UTIL_LINE_FEBA2_END		129
#define UTIL_LINE_WIRE				130
#define UTIL_LINE_ARROW2_START		131
#define UTIL_LINE_ARROW2_END		132

#define UTIL_LINE_XOR				998
#define UTIL_LINE_NONE				999

#define UTIL_LINE_TYPE_SIMPLE  1
#define UTIL_LINE_TYPE_RHUMB   2
#define UTIL_LINE_TYPE_GREAT	 3

#define UTIL_FIL_END_OF_FILE 99
*/


class CUtil
{
public:
   CUtil(){}

   int geo_east_of(double a, double b);
   void compute_center_geo(d_geo_t ll, d_geo_t ur, double *lat, double *lon);
   int geo_to_xyz(double lat, double lon, double height, double *x, double *y, double *z);
   int xyz_to_geo(double x, double y, double z, double *lat, double *lon, double *height);
   int center_of_mass(d_geo_t *geo, int numpts, d_geo_t *center_geo);
   BOOL geo_lon_in_range(double left_lon, double right_lon, double point_lon);
   BOOL geo_intersect(double ll_A_lat, double ll_A_lon,
				   double ur_A_lat, double ur_A_lon,
				   double ll_B_lat, double ll_B_lon,
				   double ur_B_lat, double ur_B_lon);
   BOOL geo_intersect(d_geo_t ll_A, d_geo_t ur_A, d_geo_t ll_B, d_geo_t ur_B);

	double line_angle(int x1, int y1,      // start point of line
                      int x2, int y2);      // end point of line
	int distance_to_line (   int x1, int y1,  // pt on the line 
                                int x2, int y2,  // pt on the line 
                                int xp, int yp ); // pt to test 
	BOOL point_in_polygon(int px, int py, double *poly_x, double *poly_y, int numverts);
	BOOL point_in_polygon(POINT tp, POINT *pts, int numverts);
	BOOL point_on_polygon(CPoint point, int threshold, POINT *pgon, int numverts);
	BOOL polygon_is_clockwise(POINT *pgon, int numverts);

	double line_angle(POINT p1,      // start point of line
                      POINT p2);      // end point of line

	double line_angle(double x1, double y1,      // start point of line
                      double x2, double y2);      // end point of line
	BOOL line_segment_intersection(	double ax1, double ay1, double ax2, double ay2,
									double bx1, double by1, double bx2, double by2,
									double *cx, double *cy);
	CString extract_path( const CString& csFilespec );
	CString extract_filename( const CString& csFilespec );
	CString extract_extension(CString fullname);
	BOOL create_directory(const CString& dirname);
	int round(double val);
	int magnitude(int x1, int y1, int x2, int y2);
	void draw_text(
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
	void get_text_size
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
	void get_text_size
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
	void get_multi_line_bounds
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
	void parse_the_text(CString text, CList<CString *, CString *> &text_list);
	UINT anchor2textalign(int anchor);
	int code2fill(int code);
	COLORREF code2color(int code);
	void compute_text_poly(	int tx, int ty,			// x/y of anchor point
								int anchor_pos,			// position of anchor point
                                int width, int height,	// height and width of text
								double text_angle,		// angle of text
                                POINT *cpt);					// OUT - corners of rectangle enclosing text



};