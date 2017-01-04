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



#pragma once

#ifndef __UTILS_H_
#define __UTILS_H_ 1

#include "common.h"
#include "geo_tool/geo_tool_d.h"
#include "maps_d.h"  // for MapCategory
#include "FvGraphics/Include/LineSegmentEnumerator.h"
#include "FvMappingGraphics/Include/GraphicsUtilities.h"

// for TimingLogger
#include "Common\ComErrorObject.h"
#include <mmsystem.h>

#include <vector>

//
// forward declarations
//
class MapProj;
class ViewMapProj;
class CLineSegmentRenderer;

#define UTIL_COUNTRY_NUM   261
#define UTIL_STATE_NUM      59

//-----------------------------------------------------------------------------
//timers

//--------------------------------------
class TimerList;

TimerList* UTL_get_timer_list(void);

//--------------------------------------

#define TICK_PERIOD 100
    
class FVW_Timer : public CObject
{
   DECLARE_DYNAMIC(FVW_Timer)

   int m_period;   // in ms
   int m_counter;  // in ms
   boolean_t m_stopped; //TRUE if stopped, FALSE if counting

public:

   FVW_Timer(int period);
    
   void start(void) {m_stopped = FALSE;}
   void stop(void) {m_stopped = TRUE;}
   void reset(void) {m_counter = 0;}
   void restart(void) { m_counter = 0; m_stopped = FALSE; }
   void set_period(int period) {m_period = period;}
 
   void tick(void);
   
   virtual void expired()=0;
   virtual ~FVW_Timer() {};
};


class TimerList
{
  
   CList<FVW_Timer*,FVW_Timer*> m_list;
   
   int m_timer_ID;

public:
   TimerList();
   ~TimerList();
   int initialize(void);  //WARNING: this must be called
   void remove_all() {m_list.RemoveAll();}           //removes all timers 
   POSITION Find(FVW_Timer* element, POSITION startAfter = NULL) 
                              {return m_list.Find(element,startAfter);}
   void remove_at(POSITION pos) {m_list.RemoveAt(pos);}  //removes at position
   void delete_all(void);                                //remove & destroy all
   POSITION get_head_position(void);
   //FVW_Timer* get_first(POSITION &pos);  
   FVW_Timer* get_next(POSITION &pos);
   //FVW_Timer* get_first_of_type(POSITION &pos, const CRuntimeClass * Class);  
   FVW_Timer* get_next_of_type(POSITION &pos, const CRuntimeClass * Class);  
   POSITION add(FVW_Timer* timer);  //add timer to list
   void tick(void);  //make all elements in list tick
};

// TimingLogger is used to measure time for various tasks in the test configurations
class TimingLogger
{
   DWORD m_dwStart;
   CString m_logMessage;

public:
   TimingLogger(const CString& logMessage) : m_logMessage(logMessage)
   {
      m_dwStart = timeGetTime();
   }
   ~TimingLogger()
   {
      CString msg;
      msg.Format("[TIMING] %s (%d ms)", m_logMessage, timeGetTime() - m_dwStart);
      WriteToLogFile(_bstr_t(msg));
   }
};

// The purpose of this class is to allow you to temporarily exclude any number
// of circluar or rectangular regions from the invalidate region during a draw
// and restore the DC to it's original state when you are done.
class ExcludeClipRegion
{
private:
   int m_saved_DC;
   CDC *m_dc;

public:
   // Constructor
   // Creates an empty exclude clip region for the given DC.  The DC must
   // be valid.  The state of the DC is saved by the constructor and restored
   // by the destructor.
   ExcludeClipRegion(CDC *dc, bool bSaveDC = true);

   // Destructor
   // Restores the DC passed to the constructor.
   ~ExcludeClipRegion();

   // Adds a rectangle to the no-draw region.
   int add_rect(CRect &rect);

   // Adds a region to the no-draw region
   int add_region(CRgn &rgn);

   // Adds a circle to the no-draw region.
   int add_circle(int x, int y, int radius);

   // Adds a polygon to the clip region.  The function closes the polygon 
   // automatically, if necessary, by assuming a line from the last vertex 
   // to the first.  See CreatePolygonRgn on-line help for more information.
   // The parameters to add_polygon are analogous to those in the 
   // CreatePolygonRgn(LPPOINT lpPoints, int nCount, and int nMode) function.
   int add_polygon(LPPOINT points, int num_points, 
                                      int mode = ALTERNATE);
};

// Various representations of line segments used throughout FalconView -
// those using MFC classes are defined here
//

// CLineSegmentList - a MFC CList of CPoints.  Every two points defines a line
//    segment (i.e., vertices are duplicated)
class CLineSegmentList : public LineSegmentEnumerator
{
   CList<CPoint *, CPoint *> &m_listPoints;
   POSITION m_position;

public:
   CLineSegmentList(CList <CPoint *, CPoint *> &listPoints);

   virtual void MoveFirst();
   virtual BOOL GetNextSegment(int &x1, int &y1, int &x2, int &y2);
};

// CLineSegmentResizableArray - an array of CPoints.  Vertices are not duplicated
class CLineSegmentResizableArray : public LineSegmentEnumerator
{
   std::vector<CPoint> m_vecPoints;
   size_t m_nCurrentIndex;
   bool m_bClosed;

public:
   CLineSegmentResizableArray(CPoint *pPoints, int nNumPoints);
   CLineSegmentResizableArray(POINT *pPoints, int nNumPoints);
   CLineSegmentResizableArray(bool bClosed) :
      m_bClosed(bClosed)
   {
   }

   void AddPoint(CPoint pt)
   {
      m_vecPoints.push_back(pt);
   }

   virtual void MoveFirst();
   virtual BOOL GetNextSegment(int &x1, int &y1, int &x2, int &y2);
};


// CLineSegmentArrayIndices - an array of CPoints.  The line segments are 
//    defined by a set of array indices
class CLineSegmentArrayIndices : public LineSegmentEnumerator
{
   CPoint *m_pPoints;
   int *m_pIndices;
   int m_nNumPoints;

   int m_nCurrentIndex;

public:
   CLineSegmentArrayIndices(CPoint *pPoints, int *pIndices, int nNumPoints);

   virtual void MoveFirst();
   virtual BOOL GetNextSegment(int &x1, int &y1, int &x2, int &y2);
};

// The purpose of this class is to allow graphical objects to be drawn safely
// and efficiently without corrupting the pen in the graphics library.  All of
// the drawing member functions use the DC and pen of this object to draw a
// their type of object.  Users of this class must either delete the instance
// of this class they are using or call the restore_pen member after all
// drawing is complete.
class UtilDraw : public GraphicsUtilities
{
private:
   CDC *m_cdc;
   int m_style;
   CRect m_clip_rect;
public:
   // Constructor 
   UtilDraw(CDC *dc = NULL);

   // Select the pen to be used by the drawing member functions.  If the DC
   // this object is not defined this function returns FAILURE.  You must call
   // this function at least once before calling a drawing member of this 
   // function.  Set the optional parameter is_background_line to TRUE if you are
   // drawing a non-solid line style and the required pattern is the pattern produced
   // if the width of the line was actually two less.
   int set_pen(int color, int style, int width, boolean_t is_background_line = FALSE);
   virtual int set_pen(COLORREF color, int style, int width, BOOL is_background_line = FALSE);

   // Set the DC used by this object.  The DC must be set in order for the
   // set_pen() or the drawing member functions to work.  If the DC is set in
   // the constructor, you don't have to call this function.
   int set_dc(CDC *dc);

   // Set the clip rectangle used to avoid passing integer values outside of
   // the range of a SHORT to the WIN32 API, because the API will not properly 
   // clip large values on Windows 95 (and probably Windows 98 too).
   int set_clip_rect(MapProj *map);

   // *** Drawing functions *** //

   // For the following functions map = NULL is a valid input.  The map object 
   // is used to perform clipping from 32-bit integer coordinates to 16-bit 
   // integer coordinates (Win95 specific)

   // draw_line draws a line between two endpoints with the current pen.  The optional 
   // arguments are used only if a non-solid line is drawn and the line style is to
   // be applied over a set of continuous line segments. 
   // map = NULL is a valid input.  The map object is used to clip the line
   // from 32-bit integer coordinates to 16-bit integer coordinates
   int draw_line(MapProj *map, CPoint p1, CPoint p2);
   int draw_line(MapProj *map, int x1, int y1, int x2, int y2);
   virtual int draw_line(int x1, int y1, int x2, int y2);

   // line, poly-line and shape functions from gra_base_dc class
   int draw_arc(boolean_t fill, int ul_x, int ul_y, int lr_x, int lr_y,
      int from_x, int from_y, int to_x, int to_y);
   int draw_circle(boolean_t fill, int center_x, 
      int center_y, int radius);
   int draw_dot(int center_x, int center_y)
   { 
      return draw_circle(TRUE, center_x, center_y, max(GetWidth() / 2, 1)); 
   }

   int draw_ellipse(boolean_t fill, int ul_x, int ul_y, 
      int lr_x, int lr_y);
   int draw_rectangle(boolean_t fill, int ul_x, int ul_y, int lr_x, 
      int lr_y);
   int set_pixel(int x, int y, int color);

   // draw the no map data symbol
   void draw_no_map_data_symbol(CPoint center, int radius);

   // draw the page center symbol
   void draw_page_center_symbol(CPoint center);

   // draw_diamond_at() takes a center point and a rotation 
   // and draws a diamond (rotated rectangle) at the given point
   void draw_diamond(int center_x, int center_y, double theta);

private:
   //
   boolean_t is_standard_style();
   boolean_t is_custom_draw();

   // Returns TRUE if the given rectangle intersects m_clip_rect, FALSE 
   // otherwise.
   boolean_t intersect_clip_rect(CRect &rect);
   
}; // end UtilDraw

//----------------------------------------------------------------------------
// UtilMenu - class used to house CMenu utility functions
//----------------------------------------------------------------------------
class UtilMenu
{
   CMenu *m_menu;

public:
   UtilMenu(CMenu *menu);

   // Returns the index of the first item in the menu with the given name.  Returns
   // -1 if the menu entry is not found
   int get_menu_pos(const char *name);

   // Returns the submenu with the given name.  NULL if none is found.
   CMenu *get_submenu_by_name(const char *name);

   // Removes all menu items after the one with the given menu string, if a 
   // menu item with that string is found, and returns TRUE.  Otherwise FALSE 
   // is returned.
   boolean_t remove_menu_items_after(CString menu_string);

   // Removes all menu items after the one with the given ID, if a menu item
   // with that ID is found, and returns TRUE.  Otherwise FALSE is returned.
   boolean_t remove_menu_items_after(UINT id);

   // appends a submenu with the given name
   CMenu* append_submenu(const char* pSubmenuName);
};

//-----------------------------------------------------------------------------
// instance information about FalconView

void UTL_set_instance(int instance);
int UTL_get_instance(void);

//-----------------------------------------------------------------------------

// Message pump facility to allow cancel of long operations
int UTL_MessagePump(int virtKey);

// ----------------------------------------------------------------------------
//
// DON'T call these unless you 1) have a good reason and also 2) talk to Vinny
// and Rob about it first.  These are included mainly for
// the overlay manager for the purposes of testing.
//
// wnd MUST be a MapView.  These functions call the special MapView invalidation
// functions instead of the CWnd::invalidate functions.
//
// allow_animation_override is used to allow the invalidate_view... functions
// to "override" the bErase value passed in when in animation mode on 
// a paletted (e.g. 256-color) device.  Only set allow_animation_override
// to TRUE when the invalidate_view... functions should be allowed to override the
// the bErase values (e.g. it's ok to change bErase from TRUE to FALSE).
//
//
void UTL_invalidate_view(CWnd* wnd, BOOL bErase = TRUE, 
   BOOL allow_animation_override=FALSE, 
   BOOL redraw_overlays_from_scratch_when_animating=FALSE);
void UTL_invalidate_view_rect(CWnd* wnd, LPCRECT lpRect, BOOL bErase = TRUE,
   BOOL allow_animation_override=FALSE,
   BOOL redraw_overlays_from_scratch_when_animating=FALSE);
void UTL_invalidate_view_rgn(CWnd* wnd, CRgn* pRgn, BOOL bErase = TRUE,
   BOOL allow_animation_override=FALSE,
   BOOL redraw_overlays_from_scratch_when_animating=FALSE);

//////////////////////////////////////////////////////////////////////////////
// timing functions
//////////////////////////////////////////////////////////////////////////////

//
// Returns whether timing code is enabled (that is, whether timing 
// functions and classes are operating correctly).
//
boolean_t UTL_timing_on(void);

//
// a stopwatch for timing code.
//
// Note that UTL_timing_on must return TRUE for this class to operate 
// properly (although it CAN safely be called when UTL_timing_on returns 
// FALSE - but the return values can not safely be used in this case).
//
class UTL_StopWatch
{

public:

   UTL_StopWatch();

   int start(void);
   int stop(void);

   //
   // get the time after the stopwatch has been started but before it
   // has stopped
   //
   double get_time_now_seconds(void) const;
   double get_time_now_milliseconds(void) const;

   //
   // get the time after the stopwatch has been stopped
   //
   double get_final_seconds(void);
   double get_final_milliseconds(void);

private:

   double m_seconds;
   LARGE_INTEGER m_start_count;
   bool m_started; 

   bool m_time_valid;
};

//
// collects timing statistics in order to provid an average, maximum and minimum
// times.
//
class UTL_TimingStatistics
{

public:

   UTL_TimingStatistics();

   int count(void) const { return m_count; }

   void add_milliseconds(double millisecs);
   void add_seconds(double secs);

   double min_time_in_milliseconds(void) const;
   double max_time_in_milliseconds(void) const;

   double average_time_in_milliseconds(void) const;

private:

   int m_count;
   double m_total_in_millisecs;
   double m_min_time_in_millisecs;
   double m_max_time_in_millisecs;
};

//
// Write an entry to the timing log.
//
// Note that UTL_timing_on must return TRUE for this class to operate 
// properly (although it CAN safely be called when UTL_timing_on returns 
// FALSE - but the return values can not safely be used in this case).
//
void UTL_timing_report(const char* str);

//
// Write an entry to the timing log.
//
// Note that UTL_timing_on must return TRUE for this class to operate 
// properly (although it CAN safely be called when UTL_timing_on returns 
// FALSE - but the return values can not safely be used in this case).
//
// NOTE: This macro MUST be called with a double set a parentheses for
// them to work, e.g. UTL_timing_printf(("%d", 10))
//
#define UTL_timing_printf(x)                     \
   do {                                   \
      CString s;                          \
      s.Format x;                         \
      UTL_timing_report(s);                      \
   } while (0);

int UTL_remove_timing_log_if_it_has_not_been_opened(void);

// --------------------------------------------------------------------

//
// DON'T call these unless you 1) have a good reason and also 2) talk to Vinny
// and Rob about it first.  These are included as the lesser of two evils.  That is,
// using these is better than using functions in getobjpr.h (e.g. fvw_get_view).
//
CWinApp* UTL_get_app(void);
CFrameWndEx* UTL_get_frame(void);
//
// Note this function returns NULL if 1) the active view is not a 
// MapView (for example, when in print preview) or 2) there is no active view
//
CView* UTL_get_active_non_printing_view(void);

// --------------------------------------------------------------------

//
// DON'T call the functions in this section unless you 1) have a good reason and 
// also 2) talk to Vinny and Rob about it first. 
//
// When a part of the specification is not passed as a parameter below,
// the value from the current map in the view is used.
//
// These functions return SUCCESS, FAILURE, FV_NO_DATA and COV_FILE_OUT_OF_SYNC,
//

int UTL_change_view_map(CView* view, const MapSource &source, 
   const MapScale &scale, const MapSeries &series, degrees_t latitude, 
   degrees_t longitude, double rotation, int zoom_percent, 
   const ProjectionEnum &projection_type, boolean_t override_no_data = FALSE);

int UTL_change_view_map_center(CView* view, degrees_t lat, degrees_t lon);
int UTL_change_view_map_rotation(CView* view, double angle,
   degrees_t lat, degrees_t lon);

//
// These functions return SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int UTL_change_view_map_to_smallest_scale(CView* view, MapCategory cat,
   degrees_t center_lat, degrees_t center_lon, 
   double rot_angle, int zoom_percent, const ProjectionEnum &proj_type);
int UTL_change_view_map_to_largest_scale(CView* view, MapCategory cat,
   degrees_t center_lat, degrees_t center_lon, 
   double rot_angle, int zoom_percent, const ProjectionEnum &proj_type);
int UTL_change_view_map_to_closest_scale(CView* view, MapCategory cat,
   MapScale curr_scale, degrees_t center_lat, degrees_t center_lon, 
   double rot_angle, int zoom_percent, const ProjectionEnum &proj_type);
int UTL_change_view_map_to_closest(CView* view, MapCategory cat,
   MapSource curr_source, MapScale curr_scale, MapSeries curr_series, 
   degrees_t center_lat, degrees_t center_lon, double rot_angle, 
   int zoom_percent, const ProjectionEnum &proj_type);
int UTL_change_view_map_to_best(CView* view, degrees_t lat, degrees_t lon);
int UTL_change_view_map_to_best(CView* view, degrees_t lat, degrees_t lon, 
   const MapSource &source, const MapScale &scale, 
   double rot, int zoom_percent, const ProjectionEnum &proj_type);
int UTL_scale_to_extents(CView* pView, const MapSource &map_source,
   const MapScale &map_scale, const MapSeries &map_series,
   const d_geo_rect_t &boundingRect);

// --------------------------------------------------------------------

//
// Never *EVER* call this function without getting approval from both Rob and Vinny
// first.
//
// The map pointer returned is temporary and should NEVER be stored.  Use it
// and get rid of it.
//
ViewMapProj* UTL_get_current_view_map(CView* view);
ViewMapProj* UTL_get_current_view_map();

// --------------------------------------------------------------------

//
// these functions return information about the platform on whcih the exe is running
//
boolean_t UTL_is_win32s(void);
boolean_t UTL_is_win32_windows(void);   // Windows 95 or Windows 98
boolean_t UTL_is_win32_nt(void);

//
// Call this to find out which version of the OS you are running.
// If this number is >= 4, then you know if it either 95, 98 or NT 4.0 (or later).
//
int UTL_get_windows_major_version_number(void);

//
// If UTL_is_win32_windows is true and the minor version number is 0, then it is
// Windows 95.  If UTL_is_win32_windows is true and the minor version number 
// is greater than 0, then it is Windows 98 or later.
//
int UTL_get_windows_minor_version_number(void);

//
// returns TRUE if the operating system is capable of handling
// multiple monitors
//
boolean_t UTL_os_is_multiple_monitor_enabled(void);

// This function returns one of five standard cursors (IDC_SIZEALL, IDC_SIZENS, 
// IDC_SIZENWSE, IDC_SIZEWE, IDC_SIZENESW) based on where an object is
// selected, the map rotation, and the rotation of the object.  nesw indicates
// what part of the object is selected.  See the UTL_IDC_SIZE #defines for a
// list of valid nesw values.
//    -UTL_IDC_SIZE_NSEW will always cause IDC_SIZEALL to be returned.
//    -All other valid nesw inputs will return a cursor that depends on map
//     rotation and object rotation.
//    -Invalid nesw inputs will return IDC_ARROW
HCURSOR UTL_get_IDC_SIZE_cursor(int nesw, double cw_rotation_map, 
                                double cw_rotation_object = 0.0);

// If this function returns true then the user interface for FalconView is 
// hidden.  That means that under no circumstances should we display dialog
// boxes, message boxes, etc.
bool UTL_is_gui_hidden();

#endif
