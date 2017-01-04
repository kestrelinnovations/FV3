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

// AChart.h
// Description of CAreaChart that encapsulates an area chart object.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_ACHART_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_ACHART_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/Maps.h"          // for page_map_list_t
#include "FalconView/include/maps_d.h"        // MapScale and MapSource
#include "FalconView/include/mapx.h"          // SettableMapProjImpl
#include "FalconView/include/ovlelem.h"       // OvlContour, GeoBounds
#include "geo_tool/geo_tool_d.h"               // d_geo_t

// this project's headers
#include "FalconView/PrintToolOverlay/labeling.h"      // CLabelingOptions
#include "FalconView/PrintToolOverlay/PrntIcon.h"      // PrintIcon
#include "FalconView/PrintToolOverlay/PageList.h"      // CPrinterPageList

// forward definitions
class CAreaChartPage;
class MapProj;
class PageLayoutFileVersion;

class CAreaChart : public PrintIcon
{
private:
   CPrinterPageList m_aPages;
   CString m_help_text;
   CString m_tool_tip;
   int m_first_selected;
   int m_last_selected;
   CLabelingOptions m_labeling_options;
   int m_calc_return_code;

public:
   static MapSource m_nDefaultSource;
   static MapScale m_nDefaultScale;
   static MapSeries m_nDefaultSeries;
   static ProjectionEnum m_DefaultProjectionType;
   static double m_dDefaultOverlap;

protected:
   SettableMapProjImpl m_mosaic_map;

   int m_scale_percent;
   double m_overlap;
   int m_rows, m_columns;
   GeoBounds m_bounds;
   GeoBoundsDragger m_dragger;

   // Helper Functions
   void Clear();

   // Generates pages to cover the area - returns the number of pages built.
   // The list must be clear when this function is called.
   int Build(bool data_check, bool enable_page_limits, bool enable_err_mes);
   int BuildLambert(bool data_check, bool enable_page_limits,
      bool enable_err_mes);
   int BuildEqualArc(bool data_check, bool enable_page_limits,
      bool enable_err_mes);
   int handle_calc_page_layout_failure(int status, BOOL enable_error_messages);
   BOOL check_page_limits(int num_row, int num_col);

   // Given the region that the area chart must cover, this function figures
   // out the width and height in degrees of each page, the degrees latitude
   // between the center of rows of pages, the degrees longitude between the
   // center of columns of pages, the number of rows and columns of pages,
   // and the actual geographic bounds covered by all those pages.
   int calc_page_layout_parameters(d_geo_t region_ll, d_geo_t region_ur,
      degrees_t &page_width, degrees_t &page_height,
      degrees_t &delta_lat, degrees_t &delta_lon,
      int &num_row, int &num_col,
      d_geo_t &chart_ll, d_geo_t &chart_ur);

   int calc_page_layout_parameters_lambert(d_geo_t region_ll, d_geo_t region_ur,
      double &page_width, double &page_height,
      double &delta_north, double &delta_east,
      int &num_row, int &num_col,
      d_geo_t &chart_ll, d_geo_t &chart_ur);

public:
   CAreaChart(CPrintToolOverlay *parent, d_geo_t ll, d_geo_t ur);
   void initialize(boolean_t reset);
   ~CAreaChart();

   // Read Constructor - ONLY USED FOR READ
   explicit CAreaChart(CPrintToolOverlay *parent);

   // initialize public static members from the registry, if they are not
   // initialized
   static void initialize_defaults();

   // write the public static members to the registry
   static void save_defaults();

   // Get the area chart mosaic.
   const MapProj *get_mosaic_map()
   {
      return &m_mosaic_map;
   }

   // Get the percentage of get_scale() that will be used for a print-out.
   // 100 means get_scale() will be used.  200 means the scale denominator
   // will be doubled.  50 means the scale denominator will be halfed.
   int get_scale_percent() const
   {
      return m_scale_percent;
   }

   // Get the specification for the area chart mosaic.
   MapSpec get_map_spec() const;

   // Get the source for this area chart.
   MapSource get_source() const;

   // Get the scale for this area chart.
   MapScale get_scale() const;

   // Get the series for this area chart.
   MapSeries get_series() const;

   // Get the projection type for this area chart
   ProjectionEnum get_projection_type() const;

   // Get the overlap for this area chart.
   double get_overlap() const
   {
      return m_overlap;
   }

   // Get the center of the area covered by this area chart.  Note this can be
   // different from the center of selection region used to define this area
   // chart.  This function will return false if the last call to Build did not
   // succeed in creating pages to cover the selected area.  The center is not
   // defined in that case.
   bool get_center(d_geo_t *center);

   // Get the dimensions of the area chart.
   CString get_printable_area_string();

   // Set the map type for this area chart.
   int set_map_type(const MapSource &source, const MapScale &scale,
      const MapSeries &series, const ProjectionEnum &projection_type);

   // Set the percentage of get_scale() that will be used for a print-out.
   // 100 means get_scale() will be used.  200 means the scale denominator
   // will be doubled.  50 means the scale denominator will be halfed.
   int set_scale_percent(int percent);

   // Set the overlap for this area chart.
   int set_overlap(double overlap_inches);

   // Rebuild - returns the number of pages built.
   int ReBuild(bool data_check, bool enable_page_limits, bool enable_err_mes);

   // Page Functions
   void RemovePage(CAreaChartPage* pPage);
   void DeletePage(CAreaChartPage* pPage);

   // Places the edit focus on the entire area chart.  If the area chart
   // does not already have the edit focus it will be selected.  If a MapProj
   // is passed in, the part of the view covered by the selection box will be
   // invalidated, so a draw will take place.
   void Select(MapProj *view = NULL);

   CLabelingOptions *get_labeling_options()
   {
      return &m_labeling_options;
   }
   void set_labeling_options(CLabelingOptions *options);

   // C_icon Overrides

   // Returns TRUE if this icon is a instance of the given class.
   virtual boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   virtual const char *get_class_name();

   // Descriptive string functions
   virtual CString get_help_text();
   virtual CString get_tool_tip();

   // Determine if the point is over the icon.
   virtual boolean_t hit_test(CPoint point);

   // PrintIcon Overrides

   // Draw the object.
   virtual void Draw(MapProj *map, CDC* pDC);

   // Erase the object.
   virtual void Invalidate();

   // Returns true if one or more pages in this area chart are selected.
   virtual bool IsSelected() const;

   // If the object is selected it will be unselected and the selection handles
   // will be removed.  Otherwise this function does nothing.
   virtual bool UnSelect();

   // Compute printer page size dependent properties.
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

   // This area chart will add menu items that pertain to it, if any.
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

   // returns TRUE if a page is selected.  Increments page_index.
   virtual boolean_t get_current_page(int *page_index);

   // trys to select a page in this object
   virtual int select_page(int page_number, int *page_index);

   // multiple selection
   void on_control_select(CAreaChartPage *selected);
   void on_shift_select(CAreaChartPage *selected);
   bool reset_selection_range(CAreaChartPage *selected);

   // This function will remove the indicated row of pages.
   int delete_row(ViewMapProj *view, boolean_t first_not_last);

   // This function will remove one column of pages.
   int delete_column(ViewMapProj *view, boolean_t first_not_last);

   // This function must be called in the draw member of the overlay.  In case,
   // an invalidate occurs during a drag operation, the drag lines must be
   // refreshed.
   void DrawDragger(MapProj *map, CDC *dc);
};  // End CAreaChart

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_ACHART_H_
