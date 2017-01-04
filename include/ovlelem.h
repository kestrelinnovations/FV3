// Copyright (c) 1994-2009,2014 Georgia Tech Research Corporation, Atlanta, GA
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



// ovlelem.h

#ifndef OVL_ELEMENT_H
#define OVL_ELEMENT_H 1

#include "common.h"
#include "FvCoreMFC/Include/polyxy.h"
#include "geo_tool/geo_tool_d.h"

#include "maps_d.h"
#include "GenericProjector.h"
#include "..\ovlelem\ovlprim.h"
#include "ovl_d.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "FvMappingGraphics\Include\GraphicsContextWrapper.h"
#include "FvMappingCore\Include\DeviceContextWrapper.h"
#include "Common\SafeArray.h"

#include "fvwutil.h"  // for CList of C_poly_xy
#include <vector>

// Forward Declarations
class OvlSymbolPrimative;
class CFontDlg;
class MapProj;
class CDib;
class CIconImage;
class ViewMapProj;
class CMapInfo;
class CGenericProjector;


enum draw_mode_t { NORMAL_MODE = 0, FOREGROUND_MODE, BACKGROUND_MODE, XOR_MODE };

#define NW_INDEX 0
#define NE_INDEX 1
#define SE_INDEX 2
#define SW_INDEX 3

//*****************************************************************************
// OvlPen
//
// This class is a container for a foreground and background pen, which can be
// used for drawing lines, polylines, circles, etc.  Note it only stores the
// color, style, and width for these two pens,   You must use these values to
// prepare the DC before drawing.  It is meant for use with OvlElement objects.
// The allow_redraw parameter is supposed to be the m_allow_redraw flag for the
// OvlElement using this pen.  Through allow_redraw, this pen can force a draw
// from scratch when there is a change to line style, line width, or the
// background pen is turned on or off.
//*****************************************************************************
class OvlPen
{
private:
   boolean_t m_allow_redraw;
   COLORREF m_color;            // foreground color
   int m_style;            // foreground line style
   int m_width;            // foreground line width
   COLORREF m_color_bg;    // background color (-1 for no background line)
   boolean_t m_background_off;   // should the background be turned off

   IFvPenPtr m_fv_pen_normal;
   IFvPenPtr m_fv_pen_background;
   IFvPenPtr m_fv_pen_foreground;

public:
   // Constructor
   OvlPen();
   OvlPen(boolean_t &allow_redraw);

   // Set the foreground pen.
   int set_foreground_pen(int color, int style, int width);
   int set_foreground_pen(COLORREF color, int style, int width);

   // Set the background pen.  A solid line with a width 2 greater than the
   // foreground width can be drawn with the given color beneath the
   // foreground arc.  Using color of -1 turns off the background.
   int set_background_pen(int color = -1);
   int set_background_pen(COLORREF color, boolean_t turn_off_background);

   // Get the current values defining the foreground pen.
   void get_foreground_pen(int &color, int &style, int &width);
   void get_foreground_pen(COLORREF &color, int &style, int &width);

   // Get the current values defining the background pen.  Returns FALSE if
   // the background color is set to -1 to indicate that the outline should
   // not get drawn.
   boolean_t get_background_pen(int &color, int &style, int &width);
   boolean_t get_background_pen(COLORREF &color, int &style, int &width);

   // Get the colors without having to declare variables for color, style,
   // and width.
   int get_foreground_color();
   int get_background_color();

   // Equal operator  
   OvlPen &operator=(OvlPen &);

   IFvPen* get_fv_pen(gfx::GraphicsContextWrapper* gc, draw_mode_t mode);
};

//*****************************************************************************
// OvlFont
//
// This class is a container for text common text properties which can be used
// in drawing single and multiple line text.  These properties include the font
// name, size, and attirbutes (BOLD, ITALICS, UNDERLINE).  The text color is
// also included in this object, as well as the background color and type.  It
// is meant for use with OvlElement objects.  The allow_redraw parameter is
// supposed to be the m_allow_redraw flag for the OvlElement using this font.
// Through allow_redraw, this font can force a draw from scratch when needed.
//*****************************************************************************
class OvlFont
{
private:
   boolean_t &m_allow_redraw;

   // font
   CString m_font_name;
   int m_font_size;
   int m_font_attributes;

   // foreground color
   int m_fg_color;
   COLORREF m_fg_color_RGB;

   // background type and color
   int m_bg_type;
   int m_bg_color;
   COLORREF m_bg_color_RGB;

   // until everything is converted over to use RGB color values
   // we need to know which color type to use
   bool m_color_type_is_RGB;

   // registry key used by initialize_from_registry and save_in_registry
   CString m_key;

   IFvFontPtr m_fv_font;
   IFvPenPtr m_fv_pen;

public:
   // remove this when everything is converted over to use RGB values
   bool color_type_is_RGB() { return m_color_type_is_RGB; }

   // Constructor
   OvlFont(boolean_t &allow_redraw, CString key = "");
   OvlFont(CString key = "");

   // Set the font.  See utils.h for font names, and font attributes.
   int set_font(const char *name, int size, int attributes = 0);

   // Set the text foreground color.
   int set_foreground(int color);
   int set_foreground_RGB(COLORREF color);

   // Set text background type and color.  See utils.h for text background
   // types.
   int set_background(int type, int color);
   int set_background_RGB(int type, COLORREF color);

   // Get the font name, size, and attributes.
   void get_font(CString &name, int &size, int &attributes);

   // Get the text foreground color.
   void get_foreground(int &color);
   void get_foreground_RGB(COLORREF &color);

   // Get text background type and color.
   void get_background(int &type, int &color);
   void get_background_RGB(int &type, COLORREF &color);

   // Equal operator for CFontDlg.  Used to retrieve data
   // from the font dialog box
   OvlFont &operator=(CFontDlg &);

   // Equal operator  
   OvlFont &operator=(OvlFont &);

   // returns TRUE if two OvlFonts are equal
   int operator==(OvlFont prop);

   // Store values into the OvlFont given a registry key
   int initialize_from_registry(CString key,CString, int, int, int, int, int);

   // Store values into the OvlFont using m_key as the registry key
   int initialize_from_registry(CString, int, int, int, int, int);

   // Store values into the registry given a key
   int save_in_registry(CString key);

   // Store values into the registry using m_key as the registry key
   int save_in_registry();

   // Reads a PFPS formatted font type from the registry
   int ReadFromRegistry(CString strKey, CString strDefaultFontName, int nDefaultFontSize,
      CString strDefaultBackgroundColor, CString strDefaultForegroundColor,
      short nDefaultBackgroundType, bool bDefaultStrikeout,
      bool bDefaultUnderline, bool bDefaultBold, bool bDefaultItalic);

   // Stores a PFPS formatted font type to the registry
   int SaveToRegistry(CString strKey);

   void SetFont(CString strFontName, int nFontSize,
      CString strBackgroundColor, CString strForegroundColor,
      short nBackgroundType, bool bStrikeout,
      bool bUnderline, bool bBold, bool bItalic);

   void GetFont(CString& strFontName, short& nFontSize,
      CString& strBackgroundColor, CString& strForegroundColor,
      short& nBackgroundType, VARIANT_BOOL& bStrikeout,
      VARIANT_BOOL& bUnderline, VARIANT_BOOL& bBold,
      VARIANT_BOOL& bItalic) const;

   // Set the registry key
   void set_registry_key(CString key) { m_key = key; }

   // return size in bytes needed to store members of this class
   int get_block_size(void);

   // Serialize returns an array of bytes corresponding to this object
   void Serialize(BYTE *&block_ptr);

   // Deserialize takes an array of bytes and initializes this object
   void Deserialize(BYTE *&block_ptr);

   // creates a CFont object from this OvlFont
   int CreateFont(CFont &cfont);

   IFvFont* get_fv_font(gfx::GraphicsContextWrapper* gc);
   IFvPen* get_foreground_pen(gfx::GraphicsContextWrapper* gc);

protected:
   // Convert string representing hexadecimal string (RGB) to COLORREF
   COLORREF StringToColorref(CString strColor);

   // Convert COLORREF to hexadecimal string representation
   CString ColorrefToString(COLORREF color) const;
};

//*****************************************************************************
// OvlElement
//
// This class is primarily an abstract base class that provides a frame work
// for drawing and redrawing the graphical primatives that make up an overlay.
// By following this frame work any derived class should be able to redraw
// itself more quickly than it can draw itself for the first time.  The object
// should keep track of changes to it's defining parameters which disable a
// redraw.  Changes to the underlying map must be detected by users of this
// class and objects derived from it.  In other words, when the map center,
// scale, or rotation change it is up to the caller to call the draw() member,
// instead of calling the redraw() member. 
//
// The hit_test(), invalidate(), and get_rect() members all the overlay element
// to help perform common functions needed when working with overlays.  For
// example, the invalidate() member can erase the part of the window (if any)
// covered by an overlay element.
//*****************************************************************************
class OvlElement
{
protected:
   // This flag must be set to TRUE upon a successful completion of the draw()
   // function.  It should be set to FALSE if the underlying data for this
   // overlay element changes.  When it is FALSE the redraw(), hit_test(),
   // invalidate(), and get_rect() members should be disabled.
   boolean_t m_allow_redraw;

   // data that can be stored in an ovlelement, can serve any purpose
   int m_data;

   // To allow the drawing of background color only lines,
   // foreground color only, or both
   draw_mode_t m_draw_mode;

   // helper to get CDeviceContextWrapper from GraphicsContextWrapper
   std::unique_ptr<CDeviceContextWrapper> GetDCWrapper(gfx::GraphicsContextWrapper* gc);

public:
   // Constructor
   OvlElement() : m_allow_redraw(FALSE)
   {
      m_reversed = FALSE;
      m_xor = FALSE;
      m_threshold = WORLD;
      m_data = -1;
      m_draw_mode = NORMAL_MODE;
   }

   // Destructor
   virtual ~OvlElement() {}

   // boolean to indicate the in the geo_to_xy process the ends of the line were reversed
   boolean_t m_reversed;
   boolean_t m_xor;

   // Draw the element from scratch.  This function will get called if there
   // is any change to the underlying map, i.e., scale, center, rotation, etc.
   // This function will get called if there is any change to the underlying
   // data this overlay element represents.  This function must be able to
   // determine if the overlay element is in the current view and draw it, if
   // it is in the view.  Part of this process is making the necessary
   // preparations for the redraw(), hit_test(), invalidate(), and get_rect()
   // members, as all of these functions depend on the action taken by the
   // last call to the draw() function.
   int draw(MapProj* map, CDC* dc);
   int draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   virtual int draw_to_base_map(MapProj* map ) { return SUCCESS; }

   // Redraw the element exactly as it was drawn by the last call to draw().
   int redraw(MapProj* map, CDC* dc);
   int redraw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   virtual boolean_t hit_test(CPoint &point) = 0;

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   virtual void invalidate(boolean_t erase_background = FALSE) = 0;

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen.  
   virtual CRect get_rect() = 0;

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   virtual int prepare_for_redraw(MapProj* map) = 0;

   // default implementation will call prepare_for_redraw with MapProj, for now
   virtual int prepare_for_redraw(
      map_projection_utils::CMapProjWrapper* pMap);

   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   virtual boolean_t in_view() = 0;

   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   virtual int view_draw(MapProj* map, CDC* dc) = 0;
   virtual int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // returns TRUE if the class name is OvlElement
   virtual boolean_t is_kind_of(const char *class_name);

   virtual void set_location(double /*lat*/, double /*lon*/) { }

   // threshold used for ILayer objects
   MapScale m_threshold;

   void set_data(int data) { m_data = data; }
   int get_data() { return m_data; }

   // To set the drawing mode, it returns the previous drawing mode
   draw_mode_t set_draw_mode( draw_mode_t mode );
};

//*****************************************************************************
// OvlRgn
//
// OvlRgn represents a simple or complex region described in terms of
// geographical coordinates and a list of set operations (union, intersection,
// and subtraction)
//*****************************************************************************

// the following struct is used to store a list of region operations
// for an OvlRgn
typedef struct
{
   // region type (0 rectangle, 1 ellipse, 2 polygon)
   int type;

   // list of geographical points
   d_geo_t *geo;

   // count used for number of vertices for a polygon type, should be
   // set to 2 for rectangle or ellipse types
   int count;

   // operation (-1 means "root" region or no-op, 0 AND, 1 DIFF, 2 OR, 3 XOR)
   int operation;

} ovlrgn_op_t;


class OvlRgn : public OvlElement
{
private:
   CList <ovlrgn_op_t *, ovlrgn_op_t*> m_op_list;
   LongSafeArray m_sa_op_type_count;
   FloatSafeArray m_sa_screen_coordinates;
   boolean_t m_in_view;
   IFvBrushPtr m_brush;
   CRect m_rect;

   // width used for border on empty-filled rects
   OvlPen m_pen;
   int m_fill_type;

private:
   // convert geographical coordinates to screen coordinates
   void get_screen_coordinates(MapProj *map, ovlrgn_op_t *op,
      std::vector<float>& point_lst, std::vector<float>& wrapped_point_lst);

   void get_bounding_coordinates(COleSafeArray& points, int count, d_geo_t* ul, d_geo_t* lr);

   void AppendOperation(ovlrgn_op_t* op, std::vector<float>& point_lst);

public:
   void add_operation(int type, COleSafeArray& points, int count,
      int operation);

public:

   // constructor
   OvlRgn();

   // destructor
   ~OvlRgn();

   // Draw the element from scratch.  This function will get called if there
   // is any change to the underlying map, i.e., scale, center, rotation, etc.
   // This function will get called if there is any change to the underlying
   // data this overlay element represents.  This function must be able to
   // determine if the overlay element is in the current view and draw it, if
   // it is in the view.  Part of this process is making the necessary
   // preparations for the redraw(), hit_test(), invalidate(), and get_rect()
   // members, as all of these functions depend on the action taken by the
   // last call to the draw() function.
   int draw(MapProj* map, CDC* dc);

   // Redraw the element exactly as it was drawn by the last call to draw().
   int redraw(MapProj* map, CDC* dc);

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   boolean_t hit_test(CPoint &point);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   void invalidate(boolean_t erase_background = FALSE) { /* unused */ }

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen.  
   CRect get_rect();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   boolean_t in_view();

   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   int view_draw(MapProj* map, CDC* dc);
   int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // returns TRUE if the class name is OvlElement
   boolean_t is_kind_of(const char *class_name);

   // set up the brush used for the region
   void set_brush(OvlPen &pen, int fill_type);
};

//*****************************************************************************
// OvlContour
//
// This abstract base class provides an implementations all OvlElement member
// functions except for prepare_for_redraw().  This class is for geographic
// objects which can be represented as a contour on the map.  Derived
// classes must implement a prepare_for_redraw() that generates a list of
// CPoint pairs for the part of the contour that is in the current view.  Note
// these contours do not have anything to do with elevation values.
//*****************************************************************************
class OvlContour : public OvlElement
{
protected:
  
   // Each pair of points in this list defines a line-segment along the
   // contour.  The prepare_for_redraw() function implemented by a derived
   // class must fill this list with a pairs of points that define a set
   // of line-segments representing the part of this object on the current
   // map.  If no part of the object is on the map, then prepare_for_redraw()
   // must leave m_point_list empty.  m_point_list must always have an even
   // number of points.
   CList <CPoint *, CPoint *> m_point_list;
   // The foreground and background properties of the pen used to draw the
   // line-segments that define this object are stored in this pen.
   OvlPen m_pen;

   // allow embedded text in single line segments.  This member defaults
   // to a blank string.  Set using set_embedded_text().
   CString m_embedded_text;
   OvlFont m_font;

   // used for invalidating the embedded text line
   CList<fv_core_mfc::C_poly_xy*, fv_core_mfc::C_poly_xy*> m_embedded_poly_list;

   // whether the OvlContour was drawn simple (as a straight line)
   bool m_drawn_simple;

public:
   // Constructor
   OvlContour();

   // Destructor
   ~OvlContour();

   // return the list of points
   void get_point_list(CList <CPoint *, CPoint *> *&pnt_lst) { pnt_lst = &m_point_list; }

   // return a copy of the points
   BOOL get_points(CList <CPoint *, CPoint *> & list);

   // Get the OvlPen for this symbol so you can set the foreground and
   // background properties.
   OvlPen &get_pen() { return m_pen; }

   // reverse the points in the list
   void reverse_points();

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   boolean_t hit_test(CPoint &point);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   virtual void invalidate(boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen.  
   CRect get_rect();

   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   boolean_t in_view() { return (m_point_list.GetCount() > 1); }

   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   int view_draw(MapProj* map, CDC* dc);
   int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // returns TRUE if the class name is OvlContour
   virtual boolean_t is_kind_of(const char *class_name);

   // set the embedded text in this line object
   void set_embedded_text(CString txt) { m_embedded_text = txt; }

   // get a reference to the font for the embedded text
   OvlFont &get_font() { return m_font; }

protected:
   // delete all the points in m_point_list
   void delete_points();

   // returns TRUE if the line style is to be continued from one segment to
   // the next.  Override this function if the you want each pair of points
   // to apply the line style independently.
   virtual boolean_t continuous(void) { return TRUE; }

   int draw_line_with_embedded_text(map_projection_utils::CMapProjWrapper* pMap,
      gfx::GraphicsContextWrapper* gc,
      double lat1, double lon1, double lat2, double lon2, int line_type);

   int draw_line_with_embedded_text(MapProj* map, CDC* dc,
      double lat1, double lon1, double lat2, double lon2,
      int line_type);

}; // OvlContour


// GeoSegment contours represent paths along the ground.  They have a begining
// and an end.  They have a length in meters, and the ability for you to get
// a lat-lon and heading at an intermediate point along the path.  The heading
// and the path itself will be defined using either great circle or rhumb-line
// range and bearing functions, based on the specified heading type.
class GeoSegment : public OvlContour
{
public:
   enum heading_type_t {GREAT_CIRCLE = 0, RHUMB_LINE = 1, SIMPLE_LINE = 2};

protected:
   heading_type_t m_heading_type;
   double m_error;

public:
   // Constructor
   GeoSegment(heading_type_t heading_type);

   // Destructor
   ~GeoSegment() {}

   // get/set functions for the error
   double get_error() { return m_error; }
   void set_error(double error) { m_error = error; }

   // get the heading type: rhumb-line vs great circle
   heading_type_t get_heading_type() { return m_heading_type; }

   // Calculate the distance in meters and the bearing in degrees.
   int calc_range_and_bearing(d_geo_t start, d_geo_t end,
      double &distance, double &bearing);

   // Calculate the end point from the start point, the distance in meters,
   // and the bearing in degrees.
   int calc_end_point(d_geo_t start, double distance, double bearing,
      d_geo_t &end);

   // Get the starting point for the segment.
   virtual void get_start(degrees_t &lat, degrees_t &lon);
   virtual d_geo_t get_start() = 0;

   // Get the ending point for the segment.
   virtual void get_end(degrees_t &lat, degrees_t &lon);
   virtual d_geo_t get_end() = 0;

   // Returns the length of this segment in meters
   virtual double get_length() = 0;

   // If 0 <= distance <= get_length() then lat and lon will be set to the
   // point at that distance from the start of the segment, heading
   // will be set to the heading at that point along the segment, and 
   // the function will return TRUE.  Otherwise FALSE is returned and
   // lat, lon, and heading are unchanged.  distance is in meters.  
   virtual boolean_t get_point(double distance, degrees_t &lat, degrees_t &lon, 
      degrees_t &heading);
   virtual boolean_t get_point(double distance, d_geo_t &point, 
      degrees_t &heading) = 0;

   // Get the shortest distance from the point to this segment.  The returned
   // distance will be in meters.  right_of_course will be TRUE if the point is
   // right of course, FALSE otherwise.
   // The return values indicates if the point is along, before, or after
   // the segment.
   // +1 - after
   //  0 - along
   // -1 - before
   // start_to_closest is the distance in meters along the segment from the
   // closest point to the start of the segment, where the closest point is
   // the point "distance" meters away from "point".
   virtual int get_distance(d_geo_t point, double &distance,
      boolean_t &right_of_course, double &start_to_closest) = 0;

   // set the heading type: rhumb-line vs great circle
   // Note: calling this function will force a draw from scratch, but if a
   // change in heading type requires a redefinition of the geographic
   // parameters defining this segment, then it is up to the derived class
   // to over-ride this function with one that resets the object to an
   // uninitialized state.
   virtual int set_heading_type(heading_type_t heading_type);

   // returns TRUE if the segment is clipped at the starting point
   virtual boolean_t is_start_clipped() { return FALSE; }

   // returns TRUE if the segment is clipped at the ending point
   virtual boolean_t is_end_clipped() { return FALSE; }

   // returns TRUE if the class name is GeoSegment
   virtual boolean_t is_kind_of(const char *class_name);

}; // End GeoSegment

// OvlLine contours represent set of lines from lat/lon pair to lat/lon
// pair.  The lines will appear as straight lines on the screen (unlike
// the rhumb line or great circle lines.  Support for 2 or more pairs
// of lat/lons allows this class to draw polylines on the screen.  For
// filled polygons see OvlPolygon.
class OvlLine : public OvlContour
{
private:
   // this is a list of d_geo_t structures that will make up the line
   CList <d_geo_t, d_geo_t> m_geo_list;
   int m_x_offset, m_y_offset;

public:
   // Constructor
   OvlLine();

   // Destructor
   ~OvlLine();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // Uses the same information used by the hit_test(), invalidate(), and 
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   virtual int view_draw(MapProj* map, CDC* dc);
   virtual int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   virtual void invalidate(boolean_t erase_background = FALSE);

   // returns TRUE if the class name is OvlLine
   virtual boolean_t is_kind_of(const char *class_name);

   // add a point to the list of d_geo_t structures that make up the line
   void add_point(d_geo_t point) { m_geo_list.AddTail(point); }

   // set the offset of the line on the viewing surface
   void set_offset(int x_offset, int y_offset) 
   { m_x_offset = x_offset; m_y_offset = y_offset; }
};

// OvlPolygon contours represent set of lines from lat/lon pair to lat/lon
// pair which are closed.  The lines will appear as straight lines on the
// screen (unlike the rhumb line or great circle lines).  OvlPolygons can
// also be filled.  The fill type can be set with set_fill_type()
class OvlPolygon : public OvlContour
{
private:
   // following are lists of lat/lons that will make up the line
   std::vector<degrees_t> m_vecLat;
   std::vector<degrees_t> m_vecLon;

protected:
   // the call to futil.fill_polygon() needs a list of points w/o
   // any redundancy in the points
   std::vector<POINT> m_fill_point_list;

   int m_fill_type;
   CRect m_rect;

   IFvBrushPtr m_brush;

public:
   // Constructor
   OvlPolygon();

   // Destructor
   ~OvlPolygon();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // returns TRUE if the class name is GeoSegment
   virtual boolean_t is_kind_of(const char *class_name);

   // add a point to the list of d_geo_t structures that make up the line
   void add_point(d_geo_t point)
   {
      m_vecLat.push_back(point.lat);
      m_vecLon.push_back(point.lon);
   }

   void set_fill_type(int fill_type);

   // ************************************************************** //
   // OvlContour overrides - these are necessary to support the fill //
   // ************************************************************** //
  
   int view_draw(MapProj* map, CDC* dc);
   int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   void invalidate(boolean_t erase_background = FALSE);
};

// OvlEllipse contours represent an ellipse centered at a lat/lon pair
class OvlEllipse : public OvlPolygon
{
private:
   double m_lat;
   double m_lon;
   double m_vert_km;
   double m_horz_km;
   double m_angle;

   CPoint m_vertices[80];

public:
   // constructor
   OvlEllipse();

   // Destructor
   ~OvlEllipse();

   void set_location(double lat, double lon)
   {
      m_lat = lat;
      m_lon = lon;
      m_allow_redraw = FALSE;
   }

   void set_radius_from_geo(d_geo_t geo)
   {
      degrees_t unused_angle;
      GEO_distance(m_lat, m_lon, geo.lat, geo.lon, &m_vert_km, &unused_angle);
      m_horz_km = m_vert_km;
   }

   // define the ellipse
   void define(double lat, double lon, double vert_km, double horz_km, double angle)
   {
      m_lat = lat;
      m_lon = lon;
      m_vert_km = vert_km;
      m_horz_km = horz_km;
      m_angle = angle;
   }

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // returns TRUE if the class name is GeoSegment
   virtual boolean_t is_kind_of(const char *class_name);

   // get functions
   double get_lat() { return m_lat; }
   double get_lon() { return m_lon; }
   double get_vert() { return m_vert_km; }
   double get_horz() { return m_horz_km; }
   double get_angle() { return m_angle; }
};

class GeoCircleDragger : public OvlEllipse
{
public:
   // Constructor
   GeoCircleDragger();

   // Destructor
   ~GeoCircleDragger() {}

   // Base class over-ride forces XOR draws
   virtual int view_draw(MapProj* map, CDC* dc);
  
   // This function handles the drag operation.  The display will be updated
   // to show the drag operation.
   void on_drag(ViewMapProj *view, CPoint point, UINT flags,
      HCURSOR* pCursor, HintText &hint);
  
   // Called when a drag operation is completed.  The drag box will be removed.
   // The given point will be used to update the internal state of the object.
   // The object will reset to a non-drag state.
   void on_drop(ViewMapProj *view, CPoint point, UINT flags);
  
   // Called when a drag operation is aborted.  The drag box will be removed
   // and the state of the object will be reset.  The object will reset to a
   // non-drag state.
   void on_cancel_drag(ViewMapProj *view);

   void set_first_time(boolean_t ft) { m_first_time = ft; }

private:
   void draw_crosshair(MapProj *map, CDC *dc);

   boolean_t m_first_time;
};

class GeoLine : public GeoSegment
{
private:
   d_geo_t m_start;  // start of line
   d_geo_t m_end;    // end of line
   int m_start_radius;
   int m_end_radius;
   BOOL m_clip_line;

public:
   // Constructors - by default geographic lines have a BRIGHT_WHITE,
   // 1 pixel wide solid foreground line with no background.  Both ends
   // of the line are set to -90.0, -180.0.
   GeoLine(heading_type_t heading_type = GREAT_CIRCLE);

   // Destructor
   ~GeoLine();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // Uses the same information used by the hit_test(), invalidate(), and 
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   virtual int view_draw(MapProj* map, CDC* dc);
   virtual int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   virtual void invalidate(boolean_t erase_background = FALSE);

   // Get the starting point for the segment.
   void get_start(degrees_t &lat, degrees_t &lon) { GeoSegment::get_start(lat, lon); }
   d_geo_t get_start() { return m_start; }

   // Get the ending point for the segment.
   void get_end(degrees_t &lat, degrees_t &lon) { GeoSegment::get_end(lat, lon); }
   d_geo_t get_end() { return m_end; }

   // Returns the length of this segment in meters
   double get_length();

   // If 0 <= distance <= get_length() then lat and lon will be set to the
   // point at that distance from the start of the segment, heading
   // will be set to the heading at that point along the segment, and 
   // the function will return TRUE.  Otherwise FALSE is returned and
   // lat, lon, and heading are unchanged.  distance is in meters.  
   boolean_t get_point(double distance, d_geo_t &point, 
      degrees_t &heading);

   // Get the shortest distance from the point to this segment.  The returned
   // distance will be in meters.  right_of_course will be TRUE if the point is
   // right of course, FALSE otherwise.
   // The return values indicates if the point is along, before, or after
   // the segment.
   // +1 - after
   //  0 - along
   // -1 - before
   // start_to_closest is the distance in meters along the GeoLine from the
   // closest point to the start of the GeoLine, where the closest point is
   // the point "distance" meters away from "point".
   int get_distance(d_geo_t point, double &distance,
      boolean_t &right_of_course, double &start_to_closest);

   // Set the endpoints of the line.
   int set_start(degrees_t start_lat, degrees_t start_lon);
   int set_end(degrees_t end_lat, degrees_t end_lon);

   // The great circle line will be clipped against a circle of the given
   // radius centered at the starting point.  Valid radii range from 1 to
   // 50 pixels.  Use -1 for no clipping.
   int clip_start(int clip_radius = -1);

   // The great circle line will be clipped against a circle of the given
   // radius centered at the ending point.  Valid radii range from 1 to
   // 50 pixels.  Use -1 for no clipping.
   int clip_end(int clip_radius = -1);

   // set the flag the tells the routines whether or not to clip the geo line
   BOOL set_clip_mode(BOOL clip = TRUE);

   // returns TRUE if the line is clipped at the starting point
   boolean_t is_start_clipped() { return (m_start_radius > 0); }

   // returns TRUE if the line is clipped at the ending point
   boolean_t is_end_clipped() { return (m_end_radius > 0); }

   // returns TRUE if the class name is GeoLine otherwise calls GeoSegment::is_kind_of
   boolean_t is_kind_of(const char *class_name);

private:
   int prepare_for_redraw_great_circle(MapProj* map, d_geo_t map_ll, d_geo_t map_ur,
      d_geo_t start, d_geo_t end, boolean_t test_for_wrap);

   int prepare_for_redraw_rhumb_line(MapProj* map, d_geo_t map_ll, d_geo_t map_ur,
      d_geo_t start, d_geo_t end, boolean_t test_for_wrap);

   int prepare_for_redraw_simple(MapProj* map, d_geo_t map_ll, d_geo_t map_ur,
      d_geo_t start, d_geo_t end, boolean_t test_for_wrap);

   int generate_simple_points(MapProj* map, d_geo_t p1, d_geo_t p2, CList <CPoint *, CPoint *>& point_list);

   // Clip the end points according to the starting and ending radii.  Input
   // a copy of m_start and m_end so their values can be preserved.  Returns
   // no_line set to TRUE if the line disappeared as a result of clipping.
   int clip_end_points(MapProj* map, d_geo_t &start, d_geo_t &end, boolean_t &no_line);

   // returns TRUE if the ending point is east of the starting point
   boolean_t is_east_of(void);

}; // end GeoLine


class GeoArc : public GeoSegment
{
private:
   d_geo_t m_start;        // point where the arc begins
   d_geo_t m_end;          // point where the arc ends
   d_geo_t m_center;       // center for the geo arc
   double m_radius;        // radius of the geo arc in meters
   boolean_t m_clockwise;  // which way to go from start to end

public:
   // Constructors - by default arc have a BRIGHT_WHITE, 1 pixel wide solid
   // foreground pen and no background.  Both ends of the arc are set to 
   // -90.0, -180.0.  The radius is set to 0.0.  If the arc is not setup
   // correctly via the define member, the draw and redraw functions will fail.
   GeoArc(heading_type_t heading_type = GREAT_CIRCLE);

   // Destructor
   ~GeoArc() {}

   // returns the number of segments needed to approximate the arc
   int get_number_of_segments();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // Get the starting point for the segment.
   void get_start(degrees_t &lat, degrees_t &lon) { GeoSegment::get_start(lat, lon); }
   d_geo_t get_start() { return m_start; }

   // Get the ending point for the segment.
   void get_end(degrees_t &lat, degrees_t &lon) { GeoSegment::get_end(lat, lon); }
   d_geo_t get_end() { return m_end; }

   // Returns the length of this segment in meters
   double get_length();

   // If 0 <= distance <= get_length() then lat and lon will be set to the
   // point at that distance from the start of the segment, heading
   // will be set to the heading at that point along the segment, and 
   // the function will return TRUE.  Otherwise FALSE is returned and
   // lat, lon, and heading are unchanged.  distance is in meters.  
   boolean_t get_point(double distance, d_geo_t &point, 
      degrees_t &heading);

   // Get the shortest distance from the point to this segment.  The returned
   // distance will be in meters.  right_of_course will be TRUE if the point is
   // right of course, FALSE otherwise.
   // The return values indicates if the point is along, before, or after
   // the segment.
   // +1 - after
   //  0 - along
   // -1 - before
   // start_to_closest is the distance in meters along the GeoArc from the
   // closest point to the start of the GeoArc, where the closest point is
   // the point "distance" meters away from "point".
   int get_distance(d_geo_t point, double &distance,
      boolean_t &right_of_course, double &start_to_closest);

   // set the heading type: rhumb-line vs great circle
   // Note: a change in heading type must be accompanied by a redefinition
   // of the GeoArc.  The calculations involved in the definition are dependent
   // on this value.
   int set_heading_type(heading_type_t heading_type);

   // Get the turn radius - 0.0 for NULL arc.
   double get_radius() { return m_radius; }

   // Get the turn center.
   d_geo_t get_center() { return m_center; }
   void set_center(d_geo_t center) { m_center = center; }

   // Get the heading at the starting point.
   degrees_t get_heading_in();

   // Get the heading at the ending point.
   degrees_t get_heading_out();

   // Returns TRUE if the arc is clockwise, FALSE otherwise.
   boolean_t is_clockwise() { return m_clockwise; }

   // An arc can be defined by the turn radius, turn center, heading from the
   // center to the starting point, turn arc, and turn direction.  Since these
   // values may not be known, alternative methods for defining the arc are
   // provided.  The radius is in meters.  The heading and turn_arc are in
   // degrees.
   int define(d_geo_t center, double radius, double heading_to_start,
      double turn_arc, boolean_t clockwise);

   // Here the arc is defined by specifying the its starting point along with
   // two other lat-lon values.  The great circle line from in to start is
   // tangent to the arc at the starting point.  The great circle line from
   // the ending point (calculated) to out is tangent to the arc at the
   // ending point.  The radius is in meters.  The turn_arc, in degrees, is
   // included as an input to allow this function to detect and correct for
   // an invalid turn direction from the route server.
   int define(d_geo_t in, d_geo_t start, d_geo_t out,
      double radius, double turn_arc, boolean_t clockwise);

   // Here the arc is defined as the arc of the given radius which is tangent
   // to two intersecting great circle lines.  The first is the line from in to
   // point, and it is tangent to the arc at the starting point (calculated).
   // The second is the line from out to point, and it is tangent to the arc
   // at the ending point (calculated).  In this case the arc is always less
   // than 180.0 degrees.  The radius is in meters.
   int define(d_geo_t in, d_geo_t point, d_geo_t out, double radius);

   // Here the arc is defined by specifying the its starting point along with
   // another lat-lon value, the radius, turn arc, and direction.  The great
   // circle line from in to start is tangent to the arc at the starting point.
   // The turn_arc will be used to compute the roll-out point.  The turn arc is
   // in degrees.  The radius is in meters.
   int define(d_geo_t in, d_geo_t start, double radius, double turn_arc,
      boolean_t clockwise);

   // This function will setup a NULL arc, that is, an arc with both starting
   // and ending points at the same point and radius 0.  The purpose of this
   // function is to allow this object to be in a state where it does not
   // exist without having to keep track of this outside of this object.  In
   // this state the draw, redraw, and invalidate members will do nothing, and
   // the hit_test member will always return FALSE.
   int define(d_geo_t point);

   // Define an arc by simply specifing the defining parameters of an arc
   int define(d_geo_t start, d_geo_t end, d_geo_t center,
      double radius, boolean_t clockwise);

   // returns TRUE if the class name is GeoArc otherwise calls
   // GeoSegment::is_kind_of
   boolean_t is_kind_of(const char *class_name);

   // This function will swap the starting and ending points of the arc, and
   // reverse the direction of travel.
   void reverse_direction();

   // generate a list of geoLines for the geoArc
   int generate_geoline_list(CList <GeoSegment *, GeoSegment *> &seg_lst);

private:
   // Called from prepare_for_redraw() if the GeoArc is within the current map
   // area (view).  The heading to the starting point from the center and the
   // arc length in degrees are computed by prepare_for_redraw() as part of the
   // consideration of whether or not the GeoArc is in the view.
   int generate_point_list(MapProj* map, degrees_t heading_to_start, degrees_t arc);

   // compute the heading to m_start from m_center, the heading to m_end
   // from m_center, and the angle of the arc
   int compute_headings_and_arc(degrees_t &heading_to_start,
      degrees_t &heading_to_end, degrees_t &arc);

   // adjust the bounding box to account for quadrant boundary crossing(s)
   int adjust_bounds(degrees_t heading_to_start, degrees_t heading_to_end,
      degrees_t arc, d_geo_t &bound_ll, d_geo_t &bound_ur);

   // Assumes you crossed into the quadrant moving clockwise.  If you cross
   // into a quadrant (0 for 0.0 to 90.0, 3 for 270.0 to 360.0), this function
   // will expand the given bounding box on the arc to include the cross-over
   // point.  Note: bound_ll and bound_ur must be set to define the bounding
   // box surrounding the two arc end points before the first call to this
   // function.
   int expand_bounds(int quadrant, d_geo_t &bound_ll, d_geo_t &bound_ur);
}; // end GeoArc


//*****************************************************************************
// OvlSymbol
//
// This class defines an overlay symbol made up of basic graphic primatives:
// dots, lines, and circles.  These primatives are specified in pixel
// coordinates relative to the anchor point of the symbol.  If you specify
// a non-zero angle all primatives will be rotated relative to the anchor
// point by that angle.
//*****************************************************************************
// gps symbol types
enum ship_symbol_t { AIRL, BOMB, DELT, FIGH, HELI, RECO, SHUT, STEA, TRAN, VIEW };

class OvlSymbol : public OvlElement
{
private:
   CList<OvlSymbolPrimative *, OvlSymbolPrimative *>m_list;
   d_geo_t m_anchor;
   double m_dAnchorAltitudeMeters;
   double m_angle;
   OvlPen m_pen;
   CRect m_rect;
   double m_scale;   //numerator scale
   double m_unit;   //denominator scale

   // set this flag with set_selected() if you want to draw a selection box
   // around the symbol
   boolean_t m_selected;
   int m_selection_border_size;

   CString m_name;

public:
   // Constructor
   // The anchor point is initialized to an invalid value.  You must call
   // set_anchor() with a valid value before the draw() member can be called.
   // The draw() member will return FAILURE until the anchor has been assigned
   // a valid value.  The angle is initialized to 0.0.
   OvlSymbol();

   // Constructor - allows you to set the anchor and angle, but the values
   // are used only if they are valid.
   OvlSymbol(d_geo_t anchor, double angle = 0.0);

   // Destructor
   ~OvlSymbol();

   // get/set the symbol's name
   CString get_name() { return m_name; }
   void set_name(CString name) { m_name = name; }

   // make a copy of the primitives from the given symbol
   void make_copy_from(OvlSymbol *symbol);
   POSITION get_head_pos() { return m_list.GetHeadPosition(); }
   OvlSymbolPrimative *get_next(POSITION &pos) { return m_list.GetNext(pos); }

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   boolean_t hit_test(CPoint &point);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   void invalidate(boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen.  
   CRect get_rect() { return m_rect; }
   void set_rect(CRect rect) { m_rect = rect; }
   CRect get_rect_w_selection();

   // Set the anchor point for this symbol. 
   int set_anchor(d_geo_t anchor);
   d_geo_t get_anchor() { return m_anchor; }

   // Set the rotation angle for this symbol, 0.0 to display unrotated.
   int set_angle(double angle);
   double get_angle() { return m_angle; }

   void SetAltitudeMeters(double dAltMeters) { m_dAnchorAltitudeMeters = dAltMeters; }

   // Will calculate the appropiate angle for set_angle
   double set_rotation(double current_heading );

   // Get the OvlPen for this symbol so you can set the foreground and
   // background pens.
   OvlPen &get_pen() { return m_pen; }

   // Add graphic primatives to this symbol.  Nothing will get drawn until
   // one or more primatives are added to the symbol.
   int add_dot(double x, double y);
   int add_line(double x1, double y1, double x2, double y2);
   int add_circle(double center_x, double center_y, double radius);

   /// <summary>
   /// This method finds the minimum y-val of all primatives and translates the entire symbol
   /// by that amount.  The idea is that the "nose" of a ship can optionally be at the center
   /// of the coordinate system.
   /// </summary>
   double translate_symbol_by_min_y();

   int create_from_string(CString str);
   int read_from_file(CString file_spec);

   // 1998-JUN-12 REB - function to accept normalized vector descriptions
   //this functions uses the m_scale and m_unit to resize these values
   int add_line_normalized(double x1, double y1, double x2, double y2);

   //these functions will apply our scaling values to a single value
   double scale(double value);
   double scale(int value);

   //this functions uses the m_scale and m_unit to resize the radius
   int add_circle_normalized(int center_x, int center_y, double radius);

   //set the scaling coefficient for values passed to add_line_scaled
   double set_scale(double scale) { m_scale = scale; return m_scale; }

   //return the private member
   double get_scale() { return m_scale; }

   //set the unit denominator for scaled values
   double set_unit(double unit) { m_unit = 1 / unit; return m_unit; }
  
   //return the unit value
   double get_unit() { return m_unit; }

   // Remove all of the primatives from this symbol.
   void remove_all_primatives();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);
   int prepare_for_redraw(CGenericProjector* pProj);

   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   boolean_t in_view();

   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   int view_draw(MapProj* map, CDC *dc);
   int view_draw(CGenericProjector* pProj, CDC *pDC);
   int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // this changes the color of the symbol.
   virtual void set_color(int nColor);

   // returns TRUE if the class name is OvlSymbol
   virtual boolean_t is_kind_of(const char *class_name);

   // set/get selected
   boolean_t get_selected() { return m_selected; }
   void set_selected(boolean_t sel) { m_selected = sel; }

   // these can be removed after everything is converted to use CGenericProjector
   int draw(CGenericProjector* pProj, CDC* pDC);
   int redraw(CGenericProjector* map, CDC* dc);
};

class OvlText : public OvlElement
{
private:
   // Font name, size, and attributes, text foreground color, text background
   // color and type.
   OvlFont m_font;

   // String displayed in this text object.  If the string contains
   // "\r\n", then this object contains multi-line text.
   CString m_text;

   // The text is displayed at GEO_TO_VIEW_POINT(m_anchor) + m_offset.  This
   // view coordinate is stored in m_anchor_view.  The orientation relative
   // to m_anchor_view is determined by m_anchor_type.
   d_geo_t m_anchor;
   CPoint m_offset;
   CPoint m_anchor_view;
   int m_anchor_type;

   // bounding rectangle on this object
   CRect m_rect;

   // bounding region of this object
   POINT m_box[4];

   // TRUE for multi_line text, FALSE otherwise
   boolean_t m_multi_line;

   double m_angle;

public:
   // Constructor
   //
   // The anchor point is initialized to an invalid value.  You must call
   // set_anchor() with a valid value before the draw() member can be called.
   // The draw() member will return FAILURE until the anchor has been assigned
   // a valid value.  Note the anchor offset is set to (0,0) and the anchor
   // type is set to UTIL_ANCHOR_UPPER_CENTER by default.
   //
   // The text is set to be an empty string.
   OvlText();

   // Destructor
   ~OvlText() {}

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   boolean_t hit_test(CPoint &point);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   void invalidate(boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen.  
   CRect get_rect();

   // Set the text to be displayed.  Strings must be null terminated, '\0'.
   // Use "\r\n" to end lines ONLY if multi-line text is desired.  Do not
   // end the last line with "\r\n".
   int set_text(const char *text);

   void set_angle(double angle) { m_angle = angle; }

   // set the location of the text
   void set_location(double lat, double lon)
   {
      m_anchor.lat = lat;
      m_anchor.lon = lon;
      m_allow_redraw = FALSE;
   }

   // Set the anchor location, type, and offset.
   // The text is displayed at offset + the view coordinate of anchor.  The
   // orientation relative to that point is determined by the anchor position
   // type.  See utils.h for anchor position types.
   int set_anchor(d_geo_t anchor, int type, CPoint offset);

   // Like previous function, but it assumes an offset of (0,0).  In other
   // words the text anchor point is the view coordinate of anchor.
   int set_anchor(d_geo_t anchor, int type);

   // Set the anchor point for the text.  This function has no effect on the
   // anchor type or the anchor offset.  It only changes the geographic
   // anchor point of the text.
   int set_anchor(d_geo_t anchor);

   // Get the current text string.
   const CString &get_text() { return m_text; }

   // Get the OvlFont for this text object so you can set the font properties.
   OvlFont &get_font() { return m_font; }

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   boolean_t in_view();

   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   int view_draw(MapProj* map, CDC* dc);
   int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // returns TRUE if the class name is OvlText
   virtual boolean_t is_kind_of(const char *class_name);

}; // end OvlText

// OvlBitmap - allows adding a bitmap object to the screen
class OvlBitmap : public OvlElement
{
private:
   CString m_filename;
   IFvBitmapPtr m_fv_bitmap;
   CDib *m_bitmap;
   double m_lat;
   double m_lon;
   int m_anchor_type;

   CRect m_rect;

   boolean_t m_in_view;

public:
   // Constructor
   OvlBitmap();

   // Destructor
   ~OvlBitmap();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // load the bitmap with the given filename
   int load_bitmap(const CString& filename);

   // set the location of the bitmap
   void set_location(double lat, double lon)
   {
      m_lat = lat;
      m_lon = lon;
      m_allow_redraw = FALSE;
   }

   d_geo_t get_location() { d_geo_t tmp = { m_lat, m_lon }; return tmp; }

   // set the location of the bitmap on the screen
   void set_position(double lat, double lon, int anchor_type);

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen
   CRect get_rect() { return m_rect; }

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   virtual boolean_t hit_test(CPoint &point);

   // Invalidate the part of the screen covered by this frame
   void invalidate(boolean_t erase_background = FALSE);

   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   boolean_t in_view();

   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   int view_draw(MapProj* map, CDC* dc);
   int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // returns TRUE if the class name is OvlBitmap
   virtual boolean_t is_kind_of(const char *class_name);
};

// OvlIcon - creates an icon object and handles the loading and drawing of the
// icon to the screen.  This class contains an instance of the OvlText class which
// is used to display the icon's text
class OvlIcon : public OvlElement
{
private:
   double m_lat;
   double m_lon;
   CPoint m_view_coordinates;
  
   CRect m_rect;
   boolean_t m_in_view;
  
   IFvBitmapPtr m_fv_bitmap;
   CIconImage* m_icon_image;
   OvlText *m_text_object;

   int m_icon_set_id;
   int m_offset_x, m_offset_y;
   int m_icon_size;

   CString m_filename;
  
public:
   // Constructor
   OvlIcon();
  
   // Destructor
   ~OvlIcon();

   // get a pointer to the text object
   OvlText *get_text_object() { return m_text_object; }
  
   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);
  
   // load the icon with the given filename
   int load_icon(CString filename);
  
   // set the location of the icon
   void set_location(double lat, double lon);
   void set_offset(int x, int y) { m_offset_x = x; m_offset_y = y; }
   
   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen
   CRect get_rect() { return m_rect; }
  
   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   virtual boolean_t hit_test(CPoint &point);
  
   // Invalidate the part of the screen covered by this frame
   void invalidate(boolean_t erase_background = FALSE);
  
   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   boolean_t in_view();
  
   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   int view_draw(MapProj* map, CDC* dc);
   int view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // returns TRUE if the class name is OvlIcon
   virtual boolean_t is_kind_of(const char *class_name);

   int get_icon_set_handle() { return m_icon_set_id; }
   void set_icon_set_handle(int handle)  { m_icon_set_id = handle; }

   void set_icon_size(int s) { m_icon_size = s; }
};

class GeoBounds : public OvlContour
{
protected:
   degrees_t m_north;
   degrees_t m_east;
   degrees_t m_south;
   degrees_t m_west;

   // list of points for the edit focus box
   CList <CPoint *, CPoint *> m_focus_box_list;

   // m_focus_box_list locations for the edit focus handle points,
   // NULL if the point is off the screen
   // NW, NE, SE, SW
   POSITION m_handles[4];

   // focus box and handles are drawn when true
   boolean_t m_has_edit_focus;

   bool m_no_error_check;

public:
   // Constructor
   GeoBounds();

   // Destructor
   ~GeoBounds();

   // set the geo-bounds via the southwest and northeast corners
   int set_bounds(d_geo_t sw, d_geo_t ne);

   // set the geo-bounds without doing any error checking
   int set_bounds_no_error_check(d_geo_t sw, d_geo_t ne);

   // set the geo-bounds via the southern latitude, western longitude, northern
   // latitude, and eastern longitude
   int set_bounds(degrees_t south, degrees_t west,
      degrees_t north, degrees_t east);

   // get the bounds as corner points
   d_geo_t get_sw();
   d_geo_t get_nw();
   d_geo_t get_ne();
   d_geo_t get_se();

   // get the bounding latitudes and longitudes
   void get_bounds(degrees_t &south, degrees_t &west,
      degrees_t &north, degrees_t &east);

   // an edit focus rectangle and handles will be drawn when this object has
   // the edit focus
   void set_edit_focus(boolean_t edit_focus) { m_has_edit_focus = edit_focus; }
   boolean_t has_edit_focus() const { return m_has_edit_focus; }

   ////////////////////////////////////////////////////////////////////////////
   // Base Class Overides

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.  This is the only
   virtual int prepare_for_redraw(MapProj* map);

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   virtual boolean_t hit_test(CPoint &point);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   virtual void invalidate(boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen.  
   virtual CRect get_rect();

   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   virtual int view_draw(MapProj* map, CDC* dc);

   // returns TRUE if the class name is GeoBounds or a parent class
   virtual boolean_t is_kind_of(const char *class_name);

protected:
   // returns TRUE if the line style is to be continued from one segment to
   // the next.  Override this function if the you want each pair of points
   // to apply the line style independently.
   virtual boolean_t continuous(void) { return FALSE; }

   // End Base Class Overides
   ////////////////////////////////////////////////////////////////////////////

private:
   // deletes the m_focus_box_list points
   void delete_points2();

protected:
   // adds points to the given list that would be required to draw the line of
   // latitude on the given MapProj
   int add_lat_points(MapProj *map, degrees_t lat,
      degrees_t lon1, degrees_t lon2, CList <CPoint *, CPoint *> &list);

   // adds points to the given list that would be required to draw the line of
   // longitude on the given MapProj
   int add_lon_points(MapProj *map, degrees_t lon,
      degrees_t lat1, degrees_t lat2, CList <CPoint *, CPoint *> &list);

   // prepares the focus box for drawing
   int prepare_for_redraw_focus_box(MapProj *map, int dx, int dy,
      degrees_t mid_lon);

   // Returns the position of the point in m_focus_box_list with the given
   // (x, y) values.  Returns NULL for no match found.
   POSITION get_focus_box_point_position(int x, int y);

   // Returns the index in m_handles of the handle hit by point.  Returns -1
   // if none of the points are hit.
   int get_focus_handle_at_point(CPoint point);
}; // End GeoBounds

// The purpose of this class is to handle a drag-operations for objects of
// type GeoBounds
class GeoBoundsDragger : public GeoBounds
{
private:
   int m_nesw;          // edges being modified by the drag operation
   d_geo_t m_previous;  // location of the cursor on the previous call to
   // on_drag or on_selected, where it is initialized

public:
   // Constructor
   GeoBoundsDragger();

   // Destructor
   ~GeoBoundsDragger() {}

   // Base class over-ride forces XOR draws
   virtual int view_draw(MapProj* map, CDC* dc);

   // Copies the state of the given GeoBounds object into this dragger.
   // Call this function once before each call to on_selected.
   void copy(GeoBounds &bounds);

   // Returns TRUE if the object was hit, and sets drag to be TRUE if the
   // object has been placed in a drag state.
   boolean_t on_selected(ViewMapProj *view, CPoint point, UINT flags,
      boolean_t *drag, HCURSOR *cursor);

   // This function handles the drag operation.  The display will be updated
   // to show the drag operation.
   void on_drag(MapProj *map, CDC *pDC, CPoint point, UINT flags,
      HCURSOR* pCursor, HintText &hint);

   // Called when a drag operation is completed.  The drag box will be removed.
   // The given point will be used to update the internal state of the object.
   // The object will reset to a non-drag state.
   void on_drop(MapProj *map, CDC *pDC, CPoint point, UINT flags);

   // Called when a drag operation is aborted.  The drag box will be removed
   // and the state of the object will be reset.  The object will reset to a
   // non-drag state.
   void on_cancel_drag(MapProj *map, CDC *pDC);

   // Called to see of a call to on_selected would start a drag operation.
   boolean_t on_test_selected(MapProj *view, CPoint point, UINT flags,
      HCURSOR *cursor, HintText &hint);

   boolean_t drag_active() { return (m_nesw != 0); }

   void set_modified_edge(int edge) { m_nesw = edge; }

   void set_previous(d_geo_t previous) { m_previous = previous; }

private:
   // Resets the object to a non-drag state.
   void reset();

   // Adjust the shape or position of the object based on the drag state,
   // delta-lat, and delta-lon.
   void apply_drag(degrees_t delta_lat, degrees_t delta_lon);

   // If the point hits one of the edges of the focus box this funtion will
   // return DRAG_N_BIT, DRAG_S_BIT, DRAG_E_BIT, or DRAG_W_BIT.  If no edge is
   // hit, 0 will be returned.
   int get_edge_at_point(MapProj *map, CPoint point);
}; // end GeoBoundsDragger

class OvlBullseye : public OvlElement
{
   CRect m_rect;
   boolean_t m_in_view;

   std::vector<GeoArc *> m_rangeRings;
   std::vector<GeoLine *> m_radials;

   // properties that define a bullseye
   double m_dCenterLat, m_dCenterLon;
   int m_nNumRangeRings;
   double m_dDistBetweenRingsMeters;
   double m_dHeading;
   double m_dRelativeAzimuth;          // Azimuth relative to heading that the bullseye is offset
   int m_nNumRadials;                  // Must be an odd non-negative number
   double m_dAngleBetweenRadials;

   // The foreground and background properties of the pen used to draw the
   // line-segments that define this object are stored in this pen.
   OvlPen m_pen;

public:
   OvlBullseye();
   virtual ~OvlBullseye();

   // Draw the element from scratch.  This function will get called if there
   // is any change to the underlying map, i.e., scale, center, rotation, etc.
   // This function will get called if there is any change to the underlying
   // data this overlay element represents.  This function must be able to
   // determine if the overlay element is in the current view and draw it, if
   // it is in the view.  Part of this process is making the necessary
   // preparations for the redraw(), hit_test(), invalidate(), and get_rect()
   // members, as all of these functions depend on the action taken by the
   // last call to the draw() function.
   int draw(MapProj* map, CDC* dc);

   // Redraw the element exactly as it was drawn by the last call to draw().
   int redraw(MapProj* map, CDC* dc);

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   virtual boolean_t hit_test(CPoint &point);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   virtual void invalidate(boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen.  
   virtual CRect get_rect();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   virtual int prepare_for_redraw(MapProj* map);

   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   virtual boolean_t in_view();

   // Uses the same information used by the hit_test(), invalidate(), and
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   virtual int view_draw(MapProj* map, CDC* dc);

   // returns TRUE if the class name is OvlElement
   virtual boolean_t is_kind_of(const char *class_name);

public:
   void SetCenterLocation(double dCenterLat, double dCenterLon)
   {
      m_dCenterLat = dCenterLat;
      m_dCenterLon = dCenterLon;
   }

   void SetNumRangeRings(int nNumRings) { m_nNumRangeRings = nNumRings; }
   void SetDistBetweenRings(double dDistBetweenRingsMeters) { m_dDistBetweenRingsMeters = dDistBetweenRingsMeters; }
   void SetHeading(double dHeading) { m_dHeading = dHeading; }
   void SetRelativeAzimuth(double dRelativeAzimuth) { m_dRelativeAzimuth = dRelativeAzimuth; }
   void SetNumRadials(int nNumRadials) { m_nNumRadials = nNumRadials; }
   void SetAngleBetweenRadials(double dAngleBetweenRadials) { m_dAngleBetweenRadials = dAngleBetweenRadials; }

   OvlPen& get_pen() { return m_pen; }

   /// <summary>
   /// Compute the starting and ending heading of a range given the number of radials, angle between the radials,
   /// and the relative azimuth.
   /// </summary>
   /// <param name="dHeading">Direction the bullseye is heading</param>
   /// <param name="nNumRadials">The number of radials in the bullseye</param>
   /// <param name="dAngleBetweenRadials">The angle between each radial</param>
   /// <param name="dRelativeAzimuth">The relative azimuth.  Offsets the values headings</param>
   /// <param name="dStartHeading">Returns the starting heading</param>
   /// <param name="dEndHeading">Returns the ending heading</param>
   /// <returns>SUCCESS if succesfull, FAILURE otherwise.</returns>
   static int CalcHeadings(double dHeading, int nNumRadials, double dAngleBetweenRadials, double dRelativeAzimuth,
      double& dStartHeading, double& dEndHeading);

protected:
   // helper function to add a radial from the center latitude longitude to a point a given distance along a range range
   void AddRadial(GeoArc* pRangeRing, double dDistAlongRingMeters);

   // draws the range rings and radials with the given draw mode
   void view_draw_mode(MapProj* map, CDC* dc, draw_mode_t mode);
};

#endif

