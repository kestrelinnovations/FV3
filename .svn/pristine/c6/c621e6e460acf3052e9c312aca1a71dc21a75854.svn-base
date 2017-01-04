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

// PrntPage.h
// Description of CPrinterPage class for managing printer page rectangles for
// the Printing Tools Overlay.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_PRNTPAGE_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_PRNTPAGE_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/common.h"
#include "FalconView/include/maps_d.h"     // d_geo_t, degrees_t
#include "FalconView/include/mapx.h"       // SettableMapProjImpl
#include "FalconView/include/PrntTool.h"
// this project's headers
#include "FalconView/PrintToolOverlay/PrntIcon.h"   // PrintIcon
#include "FalconView/PrintToolOverlay/labeling.h"   // CLabelingOptions
// forward definitions

// MAX_POINT_COUNT must be large enough for m_points to hold two polygons
// because of world wrap.  MAX_POINT_COUNT has to be at least 16.  m_points
// contain the center point and corner points for each edge, for a minimum
// of 8 point per polygon.
#define MAX_POINT_COUNT 16

// Number of points in 1 of those two polygons.
#define HALF_MAX_POINT_COUNT MAX_POINT_COUNT / 2

// m_points has a corner point every CORNER_POINT_SPACING
#define CORNER_POINT_SPACING HALF_MAX_POINT_COUNT/4

// m_points has a center point every CENTER_POINT_SPACING
#define CENTER_POINT_SPACING CORNER_POINT_SPACING
#define FIRST_CENTER_POINT CENTER_POINT_SPACING/2

class CPrinterPageList;
class MapProj;
class SettableMapProjImpl;
class COverlayCOM;

class CPrinterPage : public PrintIcon
{
public:
   enum
   { /*NumSides   = 4, */ BoundingRectOffset = 10 };

   static MapSource m_nDefaultSource;
   static MapScale m_nDefaultScale;
   static MapSeries m_nDefaultSeries;
   static ProjectionEnum m_DefaultProjectionType;

protected:
   int m_nLineWidth;          // Line width for drawing
   int m_state;

   CLabelingOptions m_labeling_options;

   // Container for the to-scale map that this page represents.
   SettableMapProjImpl m_map_proj;
   int m_scale_percent;

   // The view points for the polygon used to draw the page boundary, and the
   // number of points.
   // 0  - no intersection with the view
   // 4  - the page was drawn once
   // 8  - world wrap
   CPoint m_points[MAX_POINT_COUNT];
   int m_point_count;

   // The view coordinate for the center of the page.
   CPoint m_view_center;

   // Used for a backup copy of the page during a drag-operation.
   CPrinterPage *m_backup;
   d_geo_t m_reference_point_geo;
   d_geo_t m_drag_map_center;
   double m_resize_factor;

   // used to store the cursor location (one of the types defined in utils.h)
   int m_nesw;

protected:
   // Drawing
   void DrawSquareHandle(CDC* pDC, const CPoint& pt) const;
   void InvalidateSquareHandle(const CPoint& pt) const;
   void DrawEllipseHandle(CDC* pDC, const CPoint& pt) const;
   void InvalidateEllipseHandle(const CPoint& pt) const;
   void DrawBoundingRect(CDC* pDC) const;
   void InvalidateBoundingRect() const;

   // Some view coordinate functions.
   // These functions return TRUE if some part of the page is in the view,
   // FALSE otherwise.
   boolean_t GetBoundingRectPoints(CPoint aPnts[MAX_POINT_COUNT]) const;

   // These functions return the number of handles.  Returns 0 if the page
   // is not in the view.  4 for normal draws, and 8 if the page wraps around
   // the world.
   int GetSquareHandlePoints(CPoint aPnts[8]) const;
   int GetEllipseHandlePoints(CPoint aPnts[8]) const;

   // Calculate the view points for this CPrinterPage on the given map.
   boolean_t CalcViewPoints(MapProj *map);

   virtual COLORREF GetNormalPenColor() const;
   virtual bool SnapToLegEnabled() const;
   virtual bool AlignToLegEnabled() const;

   // Creates a new copy of this object.  Derived classes must over-ride.
   virtual CPrinterPage *create_copy();

   static void set_printable_area_from_print_DC()
   {
      CPrintToolOverlay::set_printable_area_from_print_DC();
   }

   // Set / Clear the BOUND_TO_SURFACE flag.
   void SetBoundToSurface(bool calc_succeeded);

   // Test the BOUND_TO_SURFACE flag.
   bool BoundToSurface() const;

   // Calculates the view points for the edge of this CPrinterPage defined by
   // p1 and p2.  The 0, 2, or 4 view points will be added to m_points,
   // depending on how the line intersects the map.  Each new point added will
   // be placed at m_point_count, and m_point_count will be incremented once for
   // each point.
   void add_points_for_edge(MapProj *map, d_geo_t p1, d_geo_t p2);

public:
   // Construction/Destruction
   explicit CPrinterPage(CPrintToolOverlay *parent);
   void initialize(boolean_t reset);
   virtual ~CPrinterPage();

   // initialize public static members from the registry, if they are not
   // initialized
   static void initialize_defaults();

   // write the public static members to the registry
   static void save_defaults();

   // Copy operator.
   CPrinterPage& operator =(const CPrinterPage& page);

   SettableMapProj *get_SettableMapProj()
   {
      return &m_map_proj;
   }

   // Get the specification for the page.
   MapSpec get_map_spec() const;

   // Get the center point for the page.
   d_geo_t get_center() const;

   // Get the source for this page.
   MapSource get_source() const;

   // Get the scale for this page.
   MapScale get_scale() const;

   // Get the series for this page.
   MapSeries get_series() const;

   // Get the projection for this page
   ProjectionEnum get_projection_type() const;

   // Get the percentage of get_scale() that will be used for a print-out.
   // 100 means get_scale() will be used.  200 means the scale denominator
   // will be doubled.  50 means the scale denominator will be halfed.
   int get_scale_percent() const
   {
      return m_scale_percent;
   }

   // Get the rotation.
   double get_rotation() const;

   // Set the endpoints of the line.
   virtual int set_center(degrees_t lat, degrees_t lon);
   virtual int set_center(d_geo_t center)
   { return CPrinterPage::set_center(center.lat, center.lon); }

   // Set the map type for this page.
   int set_map_type(const MapSource &source, const MapScale &scale,
      const MapSeries &series, ProjectionEnum projection_type,
      bool use_mosaic_map_parameters = false,
      SettableMapProjImpl *mosaic_map = NULL,
      int row = 0, int col = 0, double mosaic_offset_x = 0,
      double mosaic_offset_y = 0, int num_row = 0, int num_col = 0);

   // Set the percentage of get_scale() that will be used for a print-out.
   // 100 means get_scale() will be used.  200 means the scale denominator
   // will be doubled.  50 means the scale denominator will be halfed.
   int set_scale_percent(int percent);

   // Set the rotation.
   virtual int set_rotation(double angle);

   // get the distance from the bottom center of the page to the top center
   // of the page
   double get_height_in_meters();

   // get the distance from the left center of the page to the right center
   // of the page
   double get_width_in_meters();

   // dragging
   virtual bool CanMove() const
   {
      return true;
   };
   virtual bool CanRotate() const
   {
      return true;
   };
   virtual bool CanResize() const
   {
      return true;
   };

   // Coordinate/View related functions
   // You must initialize num_points to the size of the input array.
   // num_points will be set to the actual number of points received.
   void GetViewPoints(CPoint aPoints[], int *num_points) const;

   // Returns a list of lat-lon values representing the perimeter of the page.
   // The first point is the upper left corner of the page.  The points are in
   // clockwise order as you move around the edge of the page.
   //
   // 8 points are returned instead of 4, to handle the case of an edge that
   // spans more than 180 degrees of longitude.  The coordinates of the corner
   // points are at the even indices.  The coordinates of the edge midpoints
   // are at the odd indices.
   void GetGeoPoints(d_geo_t aDegrees[8]) const;

   // Returns the view coordinate of the page.
   CPoint &GetCenterViewCoords()
   {
      return m_view_center;
   }

   // Returns true if the page was successfully bound to the surface.
   // Use surface_check of true to detect invalid surface size without
   // reporting errors.
   virtual bool SetCenterPtFromViewCoords(MapProj *map, const CPoint& pt,
      bool surface_check = false);

   // Translation
   void OffsetByViewCoords(MapProj *map, const CSize& sizeOffset);
   void Rotate(MapProj *map, double angle);

   // returns TRUE if this page is in the view
   bool in_view() const
   {
      return (m_point_count != 0);
   }

   // returns TRUE if this page wraps around the world
   bool wrapped() const
   {
      return (m_point_count == MAX_POINT_COUNT);
   }

   // Selection
   void Select();

   // Move
   void BeginMove();
   void EndMove();
   bool Moving() const;

   // Rotate
   void BeginRotate();
   void EndRotate();
   bool Rotating() const;

   // Resize
   void BeginResize();
   void EndResize();
   bool Resizing() const;

   // Route legs/ Layout
   BOOL PTOverRouteLeg(MapProj *map, CPoint pt,
      POINT_ON_ROUTELEG *pointOnRouteLeg) const;

   // Return true if the page was modified, false otherwise.
   bool SnapToRouteLeg(MapProj *map, CPoint ptCenter, bool north_up = false);
   bool SnapToRouteLeg(const POINT_ON_ROUTELEG& pointOnRouteLeg,
      bool north_up = false);
   void AlignToRouteLeg(COverlayCOM *route, bool north_up = false);

   // Hit test functions
   boolean_t PtOnRect(const CPoint& pt);
   boolean_t PtOnElHandle(const CPoint& pt);

   // returns TRUE if the given pt is on a resize handle.  nesw will
   // be one of the cursor locations defined in utils.h
   boolean_t PtOnResizeHandle(const CPoint& pt);
   boolean_t PtOnResizeHandle(const CPoint& pt, int &nesw);
   boolean_t point_on_no_map_data_symbol(const CPoint& pt);
   boolean_t point_on_page_center_symbol(const CPoint& pt);

   double GetVirtualPageWidthInInches()
   {
      return get_parent()->GetVirtualPageWidthInInches();
   }
   double GetVirtualPageHeightInInches()
   {
      return get_parent()->GetVirtualPageHeightInInches();
   }
   double GetMaxVirtualPageOverlapInInches()
   {
      return get_parent()->GetMaxVirtualPageOverlapInInches();
   }

   static double GetPageWidthInInches()
   {
      return CPrintToolOverlay::GetPageWidthInInches();
   }
   static double GetPageHeightInInches()
   {
      return CPrintToolOverlay::GetPageHeightInInches();
   }
   static double GetMaxPageOverlapInInches()
   {
      return CPrintToolOverlay::GetMaxPageOverlapInInches();
   }

   static CString get_printable_area_string();
   static CString get_map_scale_string(const MapSource &source,
      const MapScale &scale, const MapSeries &series);
   static CString get_map_type_string(const MapSource &source,
      const MapScale &scale, const MapSeries &series);
   static bool parse_map_type_string(const char *str, MapSource *source,
      MapScale *scale, MapSeries *series);

   // gets the default scale percent for the given map type
   static int get_scale_percent(const MapSource &source, const MapScale &scale,
      const MapSeries &series);

   // sets the default scale percent for the given map type
   static bool set_scale_percent(const MapSource &source, const MapScale &scale,
      const MapSeries &series, int percent);

   CLabelingOptions *get_labeling_options()
   {
      return &m_labeling_options;
   }
   void set_labeling_options(CLabelingOptions *options)
   {
      m_labeling_options = *options;
   }

   // Returns TRUE if this icon is a instance of the given class.
   virtual boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   virtual const char *get_class_name();

   // Descriptive string functions
   virtual CString get_help_text();
   virtual CString get_tool_tip();

   // Determine if the point is over the icon.
   virtual boolean_t hit_test(CPoint point);

   // Draw the object.
   virtual void Draw(MapProj *map, CDC* pDC);

   // Erase the object.
   virtual void Invalidate();

   // Returns true if this page is selected.
   virtual bool IsSelected() const;

   // If the object is selected it will be unselected and the selection handles
   // will be removed.  Otherwise this function does nothing.
   virtual bool UnSelect();

   // Bind the map projection object for this page based on the current printer
   // paper size.  Returns SUCCESS, FAILURE, or SURFACE_TOO_LARGE. If
   // surface_check is true, the surface size will be tested and
   // SURFACE_TOO_LARGE will be returned if the surface size is to large for the
   // current map spec and printer paper size.  surface_check should be false if
   // you already know the surface size is valid.
   virtual int Calc(bool data_check = true, bool surface_check = false);

   // If the point hits an object the pointer to that object is returned.
   // For simple objects the function will return the "this" pointer or NULL.
   // For a compound object the function may return the "this" pointer, NULL,
   // or a pointer to another PrintIcon which represents the component of
   // compound object hit by pt.
   virtual PrintIcon *GetAtViewCoords(const CPoint& pt);

   // This function handles the selected operation when an object is selected,
   // in the current overlay and the Page Layout Tool is active.  It assumes it
   // is called under the right circumstances.
   // This function returns TRUE if some action is taken, FALSE otherwise.  If
   // TRUE, drag and cursor will be set accordingly.  When *drag is set to TRUE
   // one or more of on_drag, on_drop, or cancel_drag members will get called,
   // before the drag-operation is completed.  A drag operation can be a resize,
   // move, rotate, etc.
   virtual boolean_t on_selected(ViewMapProj *view, CPoint point, UINT flags,
      boolean_t *drag, HCURSOR *cursor);

   // This function handles the drag operation.  If a PrintIcon returns drag
   // equal to TRUE, then on_drag will be called each time the object moves.
   // The display should be updated to show the drag operation.
   virtual void on_drag(IFvMapView *pMapView, CPoint point, UINT flags,
      HCURSOR* pCursor, HintText &hint);

   // Called when a drag operation is completed.  The change should be applied
   // and the display should be updated.
   virtual void on_drop(IFvMapView *pMapView, CPoint point, UINT flags);

   // Called when a drag operation is aborted.  There state of the PrintIcon
   // must return to what it was before the drag operation began.  The object
   // is no longer being dragged.  Any drag lines on the display need to be
   // removed.
   virtual void on_cancel_drag(IFvMapView *pMapView);

   // This function handles the test_selected operation for the object the
   // point is above, if it is in the current overlay and the Page Layout Tool
   // is active.  It assumes it is called under the right circumstances.
   // This function returns TRUE when the cursor and hint are set.
   virtual boolean_t on_test_selected(MapProj *view, CPoint point, UINT flags,
      HCURSOR *cursor, HintText &hint);

   // If the object is selected, it must invalidate itself and return TRUE.
   // If one or more components of the object are selected, then the selected
   // component(s) should be invalidated and destroyed, and the function should
   // return FALSE.
   virtual boolean_t on_delete(ViewMapProj *view);

   // This single page will add menu items that pertain to it, if any.
   virtual void on_menu(MapProj *map, CPoint point,
      CList<CFVMenuNode*, CFVMenuNode*> &list);

   // Serialization
   virtual void Serialize(CArchive *ar, PageLayoutFileVersion*);

   // Returns TRUE if the object in a valid state after a Serialize-Load, FALSE
   // otherwise.
   virtual boolean_t WasLoadOK();

   // Add your map projection(s) to the given list.
   virtual int add_map_projections_to_list(page_map_list_t *list,
      boolean_t start_new_page = TRUE,
      double offset_from_left = -1,
      double offset_from_top = -1,
      page_orientation_t orientation =PAGE_ORIENTATION_DEFAULT);

   // returns TRUE if a page is selected.  Increments page_index if
   // not selected
   virtual boolean_t get_current_page(int *page_index);

   // trys to select a page in this object
   virtual int select_page(int page_number, int *page_index);

   // This function must be called in the draw member of the overlay.  In case,
   // an invalidate occurs during a drag operation, this function will draw
   // the page in it's original position.
   void DrawBackup(MapProj *map, CDC* pDC);
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_PRNTPAGE_H_
