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

// PrntTool.h
// Description of CPrintToolOverlay class for managing the different types of
// chart printing - currently area charts and strip charts.

#ifndef FALCONVIEW_INCLUDE_PRNTTOOL_H_
#define FALCONVIEW_INCLUDE_PRNTTOOL_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/common.h"
#include "FalconView/include/Maps.h"     // for page_map_list_t
#include "FalconView/include/overlay.h"  // C_overlay, CFvOverlayPersistenceImpl
#include "FalconView/include/ovl_d.h"    // HintText
// this project's headers
// forward definitions
class PrintIconList;
class PrintIcon;
class CStripChart;
class CPrinterPage;
class SettableMapProj;
class CVirtualPageLayout;

class CPrintToolOverlay :
   public IDispatchImpl<PrintToolOverlayLib::IPrintToolOverlay,
      &PrintToolOverlayLib::IID_IPrintToolOverlay,
      &PrintToolOverlayLib::LIBID_PrintToolOverlayLib>,
   public CFvOverlayPersistenceImpl
{
public:
   // Overlay Modes
   typedef enum
   { EDIT_MODE_SELECT, EDIT_MODE_PAGE, EDIT_MODE_AREA } EditModeType;

   static bool m_snap_to_leg;
   static bool m_align_to_leg;

protected:
   HintText m_HintText;

private:
   PrintIconList *m_list;
   CVirtualPageLayout *m_virtual_page_layout;

   // Reference objects for drag and drop
   CPoint    m_ptReference;
   CRect     m_rectLast;
   PrintIcon *m_dragger;
   CPoint    m_drag_origin;
   boolean_t m_moved;

   static boolean_t m_bEdit;  // Flag indicating that we are in edit mode
   static EditModeType m_EditMode;  // Current Edit Mode
   static int m_nFileCount;  // Count of all files open of this type of overlay
   static CString m_sDirectory;  // Directory to store files for this overlay
   static boolean_t m_print_active;

   // These are static because they will apply to all pages based on the
   //   current printer driver
   static double m_dPageWidthIn;
   static double m_dPageHeightIn;

   // Printer DC used to get printable area
   static CDC m_dcPrint;

protected:
   // use Release instead
   CPrintToolOverlay::~CPrintToolOverlay();

public:
   // Construction/Destruction
   CPrintToolOverlay::CPrintToolOverlay(CString sName = "C:\\");
   virtual void Finalize();

   BEGIN_COM_MAP(CPrintToolOverlay)
      COM_INTERFACE_ENTRY(PrintToolOverlayLib::IPrintToolOverlay)
      COM_INTERFACE_ENTRY_CHAIN(CFvOverlayPersistenceImpl)
   END_COM_MAP()

   virtual HRESULT InternalInitialize(GUID overlayDescGuid);

   // IPrintToolOverlay
   //
   STDMETHOD(raw_OnRouteChanged)(long nRouteId,
      PrintToolOverlayLib::RouteChangeFlagsEnum nChangeFlags,
      long *pbHasStripChart);

   // I/O
   virtual int file_new();
   virtual int open(const CString& sPathname);
   virtual int save_as(const CString& sPathname, long nSaveFormat);
   virtual const char *get_default_extension()
   {
      return get_default_extension_static();
   }

   // Drawing
   //

   virtual int draw(ActiveMap *map);
   int draw(MapProj *map, CDC *dc);

   // RTTI/Info Functions
   virtual const char* get_class_name();

   // Drag & Drop
   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);
   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);
   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point,
      UINT nFlags);

   // This function is called to get the default cursor associated with the
   // current route editing mode.
   static HCURSOR get_default_cursor();

   virtual void drag(IFvMapView *pMapView, CPoint point, UINT flags,
      HCURSOR* pCursor, HintText** ppHint);
   virtual void cancel_drag(IFvMapView *pMapView);
   virtual void drop(IFvMapView *pMapView, CPoint point, UINT flags);

   // Menu
   virtual C_icon *hit_test(MapProj* map, CPoint point);
   virtual void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,
      CFVMenuNode*> & list);

   // Key handling
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT nChar,
      UINT nRepCnt, UINT nFlags, CPoint point);

   // Focus handling
   virtual void release_edit_focus();

   // Messages
   virtual int pre_close(boolean_t *cancel = NULL);
   virtual int pre_save(boolean_t *cancel);

   // erase, rebuild, and draw all strip charts in this overlay
   void update_all_strip_charts();

   // create strip charts for any open routes
   void CreateStripCharts();

   void AddPage(CPrinterPage* pPage);
   void RemovePage(CPrinterPage* pPage);

   PrintIconList *get_icon_list()
   {
      return m_list;
   }

   // Returns true if get_icon_list is empty.
   bool is_empty();

   static void set_printable_area_from_print_DC();

   static double GetPageWidthInInches();
   static double GetPageHeightInInches();
   static double GetMaxPageOverlapInInches();

   static void apply_defaults_to_strip_charts();
   static void apply_defaults_to_area_charts();
   static void apply_defaults_to_single_pages();

   // get the virtual page width from the page layout object
   double GetVirtualPageWidthInInches();

   // get the virtual page height from the page layout object
   double GetVirtualPageHeightInInches();

   // return the maximum overlap in inches which is defined as the minimum
   // of the virtual width and virtual height
   double GetMaxVirtualPageOverlapInInches();

   // return the virtual page layout object of the current print tool overlay
   CVirtualPageLayout *get_virtual_page_layout(void)
   { return m_virtual_page_layout; }

   void set_virtual_page_layout(CVirtualPageLayout vpage_layout);

   static CVirtualPageLayout *get_vpage_layout(void);

   // when the page layout tool or the route editor are active, the foreground
   // color should only be used in the current overlay
   boolean_t use_background_color();

   // This function is called in each page layout overlay each time the printer
   // defaults change.  Returns SUCCESS, FAILURE, or SURFACE_TOO_LARGE.
   int OnSetPrinterDefaults();

   // This function is called in each page layout overlay each time a route
   // is changed.  Returns true if at least one page layout overlay has a
   // strip chart for this route.
   bool OnRouteChange(int route_id, int flags);

   PrintIcon *get_dragger(void)
   {
      return m_dragger;
   }
   static boolean_t m_dragging;  // are we dragging?

   // display the properties pages for the single page, area chart, and
   // strip chart pages
   void DoProperties(void);

   static void SetPrinterDefaults(int *failure_count,
      int *invalid_surface_count);
   static CPrintToolOverlay* GetActiveOverlay();

   static void SetEditMode(EditModeType mode)
   {
      m_EditMode = mode;
   }
   static EditModeType GetEditMode()
   {
      return m_EditMode;
   }

   // Printing interface
   static int add_map_projections_to_list(page_map_list_t *list,
      boolean_t start_new_page = TRUE,
      double offset_from_left = -1,
      double offset_from_top = -1,
      page_orientation_t orientation = PAGE_ORIENTATION_DEFAULT);

   static boolean_t set_print_from_tool(boolean_t active);
   static boolean_t get_print_from_tool()
   {
      return m_print_active;
   }

   // Get the print DC
   static CDC &GetPrintDC()
   {
      return m_dcPrint;
   }

   // get whether or not the printer dc was created successfully
   static boolean_t get_CreatePrinterDC_OK(void);

   // Are we in edit mode?
   static boolean_t IsEditing()
   {
      return m_bEdit;
   }

   // get_current_page returns which page should we begin print preview
   // at.  Returns the index of the first selected page or 1 if no pages
   // are selected
   static int get_current_page();

   // return the number of maps on a given strip chart page
   int get_num_maps_per_page();

   // selects the given page number of the current print tool overlay
   static int select_page(int page_number);

   // unselect all pages in the current print layout overlay
   static void unselect_all();

   // Notify all open CPrintToolOverlay overlays the the operation indicated by
   // flags has been performed on the route indicated by route_id.  flags
   // should contain bit flags, which are defined in RouteOverlay.idl.  Returns
   // true if at least one page layout overlay has a strip chart for this route.
   static bool RouteChange(int route_id, int flags);

   // File name/dir stuff
   static LPCSTR filename_template();
   static LPCSTR get_default_extension_static();

   // Edit Mode
   static int set_edit_on(boolean_t bEdit);
   static void EditPageProperties(ViewMapProj *map, C_icon* pIcon);
   static void DeletePage(ViewMapProj *map, C_icon* pIcon);
   static void EditStripChartProperties(ViewMapProj *map, C_icon* pIcon);
   static void DeleteStripChart(ViewMapProj *map, C_icon* pIcon);
   static void EditStripChartPage(ViewMapProj *map, C_icon* pIcon);
   static void DeleteStripChartPage(ViewMapProj *map, C_icon* pIcon);
   static void EditAreaChartProperties(ViewMapProj *map, C_icon* pIcon);
   static void DeleteAreaChart(ViewMapProj *map, C_icon* pIcon);
   static void DeleteAreaChartRow(ViewMapProj *map, C_icon* pIcon);
   static void DeleteAreaChartColumn(ViewMapProj *map, C_icon* pIcon);
   static void DeleteAreaChartPage(ViewMapProj *map, C_icon* pIcon);

   static void AddPageToStripChart(ViewMapProj *map,
      C_icon* pIcon, LPARAM lparam);

   static void print_preview(ViewMapProj *map, C_icon *pIcon);
   static void edit_overlay(ViewMapProj *map, C_icon *pIcon);
   static void north_up(ViewMapProj *map, C_icon *pIcon);

   CStripChart *GetStripChart(int route_id);
};

struct POINT_ON_ROUTELEG
{
   long nRouteId;
   double dLat;
   double dLon;
   double dHeading;
};


#endif  // FALCONVIEW_INCLUDE_PRNTTOOL_H_
