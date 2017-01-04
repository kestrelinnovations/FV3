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

// SChart.h
// Description of CStripChart class that encapsulates a strip chart object.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_SCHART_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_SCHART_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/maps_d.h"     // MapScale and MapSource
#include "FalconView/include/PrntTool.h"
// this project's headers
#include "FalconView/PrintToolOverlay/labeling.h"   // CLabelingOptions
#include "FalconView/PrintToolOverlay/PageList.h"   // CPrinterPageList
#include "FalconView/PrintToolOverlay/PrntIcon.h"   // PrintIcon
// forward definitions
class CStripChartPage;
class PageLayoutFileVersion;
class COverlayCOM;

class CStripChart : public PrintIcon
{
public:
   enum eOrientation
   { NorthUp = 0, FollowRoute = 1, NULL_ORIENTATION};

   static eOrientation m_DefaultOrientation;
   static MapSource m_nDefaultSource;
   static MapScale m_nDefaultScale;
   static MapSeries m_nDefaultSeries;
   static double m_dDefaultOverlap;
   static ProjectionEnum m_DefaultProjectionType;

private:
   int m_route_id;            // the ID of the route the strip chart is for
   CString m_route_name;      // the file specification for the associated route
   CPrinterPageList m_aPages;
   CString m_help_text;
   CString m_tool_tip;
   boolean_t m_need_to_invalidate_doghouse;
   int m_calc_return_code;

   eOrientation m_Orientation;
   int m_first_selected;
   int m_last_selected;
   CLabelingOptions m_labeling_options;

protected:
   // Helper Functions
   void Clear();
   bool PageCoversLeg(CStripChartPage* pPage, d_geo_t geoStart, d_geo_t geoEnd,
      double dLegLength, double& distance, double& bearing);
   int Build(bool data_check, bool enable_page_limits);
   bool page_limit_abort(const d_geo_t &center,
      const MapSource &source, const MapScale &scale,
      const MapSeries &series, const int scale_percent,
      const double route_length);

public:
   // Construction/Destruction
   CStripChart(CPrintToolOverlay *parent, int route_id);
   void initialize();
   ~CStripChart();

   // Read Constructor - ONLY USED FOR READ
   explicit CStripChart(CPrintToolOverlay *parent);

   // initialize public static members from the registry, if they are not
   // initialized
   static void initialize_defaults();

   // write the public static members to the registry
   static void save_defaults();

   // Rebuild
   int ReBuild(bool data_check, bool enable_page_limits);

   // Return the route that this strip chart is for, or NULL for none.
   COverlayCOM *GetRoute();

   // Page Functions
   void RemovePage(CStripChartPage* pPage);
   void DeletePage(CStripChartPage* pPage);
   void AddPage(CPrinterPage* pPage, const POINT_ON_ROUTELEG& pointOnRouteLeg);

   // Orientation
   eOrientation GetOrientation() const
   {
      return m_Orientation;
   }
   void SetOrientation(eOrientation Orientation);

   CLabelingOptions *get_labeling_options()
   {
      return &m_labeling_options;
   }
   void set_labeling_options(CLabelingOptions *options);

   boolean_t get_need_to_invalidate_doghouse()
   {
      return m_need_to_invalidate_doghouse;
   }
   void set_need_to_invalidate_doghouse(boolean_t set)
   {
      m_need_to_invalidate_doghouse = set;
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

   // Returns true if one or more pages in this strip chart are selected.
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

   // This strip chart will add menu items that pertain to it, if any.
   virtual void on_menu(MapProj *map, CPoint point,
      CList<CFVMenuNode*, CFVMenuNode*> &list);

   // Serialization
   virtual void Serialize(CArchive *ar, PageLayoutFileVersion *version);

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
   void on_control_select(CStripChartPage *selected);
   void on_shift_select(CStripChartPage *selected);
   bool reset_selection_range(CStripChartPage *selected);

   // This function is called by the overlay to notify the strip chart that
   // a change has occured to the route.  That change is indicated by flags.
   // flags contains bit flags defined in RouteOverlay.idl.
   int OnRouteChange(int flags);

   // Check to see if the route is OK for a save.  If it is not, try
   // to fix the problem.  If the problem can not be fixed return false.
   // When false is returned cancel may have been set to TRUE by the user,
   // in which case the save should be canceled.
   bool test_route_for_save(boolean_t *cancel);

private:
   // If a route with the given file specification is already opened, then that
   // route is returned.  If not, an attempt will be made to open the route
   // with the given name.  Upon success that route will be returned,
   // otherwise, NULL is returned.
   COverlayCOM *get_route_from_specification(const char *specification);
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_SCHART_H_
