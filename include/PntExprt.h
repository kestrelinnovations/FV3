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

#if !defined(PNTEXPRT_H)
#define PNTEXPRT_H

#pragma once

#include "overlay.h"
#include "ovlutil.h"
#include <fstream>
#include "fv_filter.h" // for CPointOverlayFilter
#include "FvMappingCore\Include\MapProjWrapper.h"

class CExportRegionRectangle;
class CExportRegionCircle;
class CExportRegionPoint;
class CExportRegionIcon;

class CPointExportPropertySheet;
class CPointExportCirclePropertySheet;         
class CPointExportRectanglePropertySheet;         
class CPointExportFilterPropertySheet;         

typedef enum eExpIconHitTest
{
   eHitNothing = -1,
   eHitTopLeft,
   eHitTopRight,
   eHitBottomRight,
   eHitBottomLeft
} eExpIconHitTest;


typedef enum DragOperation 
{
   Resize, 
   Move, 
   None
} eDragOperation;

struct EXPORTED_POINT
{
   CString key;
   C_overlay* pOverlay;
};


class CPointExportOverlay : 
   public CFvOverlayPersistenceImpl
{
public:
   //////////////////////////////////////////////////////
   // Public enums
   //////////////////////////////////////////////////////

   enum EditorMode {SELECT_TOOL, POINT_TOOL, LINE_TOOL, RECTANGLE_TOOL, CIRCLE_TOOL, ROUTE_TOOL };

private:

   class CPointExportOverlayPointMap
   {
   public:
      CMapStringToString    m_map;
      CString               m_file_specification;
   };

   //////////////////////////////////////////////////////
   // Private enums
   //////////////////////////////////////////////////////

   static BOOL m_bEdit;
   BOOL m_bDragging;
   eDragOperation m_drag_operation;


   //////////////////////////////////////////////////////
   // Private static member data
   //////////////////////////////////////////////////////

   static EditorMode  m_editor_mode;
   static EditorMode  m_temp_editor_mode;
   static bool m_in_temp_editor_mode;

   static short m_file_count;

   //////////////////////////////////////////////////////
   // Private instance member data
   //////////////////////////////////////////////////////

   HintText                            m_hint;
   CExportRegionIcon*                  m_current_selection;
   CString                             m_export_filespec;
   CExportRegionRectangle*             m_region_in_drag;
   CExportRegionCircle*                m_circle_in_drag;
   d_geo_t                             m_last_drag_pos;
   d_geo_t                             m_drag_pos_on_ellipse;
   eExpIconHitTest                     m_hit;
   int                                 m_fg_color;
   int                                 m_bg_color;
   UINT                                m_line_width;

   // These list are not being used?
   CList <CExportRegionRectangle *, CExportRegionRectangle *> m_rectangle_list;
   CList <CExportRegionRectangle *, CExportRegionRectangle *> m_nonoverlapping_rectangle_list;
   CList <CExportRegionCircle *, CExportRegionCircle *> m_circle_list;
   CList <CExportRegionPoint *, CExportRegionPoint *> m_point_list;

   // An integer handle assigned to each object when added to the icon lists
   int m_next_handle;  

   // given a handle, search the region in the regino map
   CExportRegionIcon* get_region_from_handle(const long handle);

public:
   CMapStringToPtr                      m_mapIconLists; 

private:
   //////////////////////////////////////////////////////
   // Private helper methods
   //////////////////////////////////////////////////////

   // Regional methods
   BOOL     crosses_date_line(CExportRegionRectangle &rect);
   int      split_across_date_line(CExportRegionRectangle &in_rect, CExportRegionRectangle **ppOut1, CExportRegionRectangle **ppOut2);
   bool     IsNorthOf(degrees_t lat1, degrees_t lat2) const;
   bool     IsSouthOf(degrees_t lat1, degrees_t lat2) const;
   bool     IsBetweenEastWest(degrees_t testLon, degrees_t eastLon, degrees_t westLon) const;
   bool     IsBetweenNorthSouth(degrees_t testLat, degrees_t northLat, degrees_t southLat) const;

   CExportRegionRectangle* replace_rubber_band_box_with_region(ViewMapProj *view, CPoint point);
public:
   // selection/unselection/hit test methods
   int      delete_current_selection();
   virtual int      on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);
   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags);
   void     unselect_current_selection();
   void     set_current_selection(CExportRegionIcon* icon);
   void     load_cursor_for_handle( enum eExpIconHitTest hit, HCURSOR * cursor );

private:
   // create a list of nonoverlapping regions covering the same area to get
   // unique points
   void     create_non_dateline_overlapping_list();

   const char* get_default_export_extension(){ return get_default_export_extension_static();}
   const char* get_default_export_filter(boolean_t open_not_save) { return "Point Export Files (*.pex)|*.pex|"; }
   CString     read_from_stream( std::ifstream& in, const CString tag,
      int max_length ) const;

   // Drawing methods
   void InvalidateCurrent();
   void Invalidate();

public:
   int save_to_file(const CString & filename);

public:
   // Region creation
   long  add_rectangle_region( double nw_lat, double nw_lon, double se_lat, double se_lon);
   long     add_circle_region( double center_lat, double center_lon, double radius);

   // Region editing
   long   move_region( const long handle, double offset_lat, double offset_lon);
   long delete_region( const long handle);


   // Point related
   int remove_non_exportable_points( CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list );
   int delete_icon_from_list( CExportRegionIcon*  pIcon );
   int add_icon_to_list( CExportRegionIcon*  pIcon );
   int load_points_in_icon( MapProj *map, C_overlay* pOverlay, CExportRegionIcon* pIcon, CList<EXPORTED_POINT, EXPORTED_POINT&>& );
   int update_points_in_regions();

private:
   CString get_count_tag_for_class( CString sName ) const;
   int write_icon_list_to_stream( std::ostream& out, CList<CExportRegionIcon*, CExportRegionIcon*>* pIconList );
   int read_icon_list_from_stream( std::istream& in, CString sClassName, int count );
   int get_count_and_class_from_string( CString sIn, int& count, CString& sClassName );
   OverlayTypeDescriptor* GetOverlayTypeByDisplayName(const CString& strDisplayName);

public:

   //////////////////////////////////////////////////////
   // Constructor/Destructor
   //////////////////////////////////////////////////////

   explicit CPointExportOverlay(CString name = "");
   virtual void Finalize();

   //////////////////////////////////////////////////////
   // Public static member data
   //////////////////////////////////////////////////////
   static CPointExportPropertySheet          propsheet;
   static CPointExportCirclePropertySheet    propsheet_circle;
   static CPointExportRectanglePropertySheet propsheet_rectangle;

   //////////////////////////////////////////////////////
   // Public  instance member data
   //////////////////////////////////////////////////////

   //////////////////////////////////////////////////////
   // Public base class overrides
   //////////////////////////////////////////////////////

   // hit test/selection
   virtual C_icon *    hit_test(map_projection_utils::CMapProjWrapper* view, CPoint point);
   virtual int         selected(IFvMapView* pMapView, CPoint point, UINT flags);

   // RTTI methods
   virtual const char* get_class_name(void) {return "CPointExportOverlay";}

   // drawing/user interface methods

   virtual int         draw(ActiveMap *map);
   virtual void        drag(ViewMapProj* map, CPoint point, UINT nFlags, HCURSOR *cursor, HintText **hint);
   virtual void        cancel_drag(ViewMapProj *view);
   virtual void        drop(ViewMapProj *map, CPoint point, UINT flags);
   virtual int         pre_close(BOOL* cancel);
   virtual boolean_t   offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, UINT nFlags, CPoint mouse_pt);
   virtual void        release_edit_focus();
   virtual void        release_focus();

   // file manipulation
   virtual int file_new();
   virtual int         open(const CString& filespec);
   virtual const char* get_default_extension(){ return get_default_extension_static();}
   virtual int         save();
   virtual int         save_as(const CString &filespec, long nSaveFormat);
   virtual CString  get_export_filespec();  
   CString  get_export_filename() { return m_export_filespec; }

   void display_properties();



   //////////////////////////////////////////////////////
   // Public static overrides for Overlay Manager
   //////////////////////////////////////////////////////

   void menu(ViewMapProj* map, CPoint point, 
      CList<CFVMenuNode *, CFVMenuNode *> &list);

   // callbacks for menu
   static void region_options( ViewMapProj *map, C_icon *icon, LPARAM lparam);
   static void edit_region(  ViewMapProj *map,C_icon *icon, LPARAM lparam);
   static void edit_circle(  ViewMapProj *map,C_icon *icon, LPARAM lparam);
   static void edit_rectangle(  ViewMapProj *map,C_icon *icon, LPARAM lparam);
   static CPointExportOverlay *callback_validation( C_icon *icon, LPARAM lparam);

   // mode methods
   static EditorMode   get_editor_mode() {return m_editor_mode;}
   static void         set_editor_mode(EditorMode mode);       
   static int          set_edit_on(boolean_t edit);

   // file manipulation
   static const char*     filename_template() { return "%s\\PointExport%d.pex"; }
   static const char*     get_default_extension_static(){ return "pex"; }
   static const char*     get_default_export_extension_static(){ return "pex"; }
   static HCURSOR         get_default_cursor();
   static boolean_t       open_read_only_check(const char *filespec, CString &excuse, UINT &help_id) { return TRUE; }

   // default values stored in registry
   static UINT             get_default_line_width();
   static int              set_default_line_width( UINT nWidth );
   static int              get_default_line_color();
   static int              set_default_line_color( int color );

   //////////////////////////////////////////////////////
   // Public helper methods
   //////////////////////////////////////////////////////
   void                    DoExport(std::ofstream &out, CString filename);

   int                     get_Dafif_date(CString & dafif_date);

   CExportRegionIcon*      get_current_selection() const {return m_current_selection;}

   UINT                    get_line_width() const;
   int                     set_line_width( UINT nWidth );
   int                     get_line_color() const;
   int                     set_line_color( int color );

   static FalconViewOverlayLib::IFvOverlayExportablePointsPtr CPointExportOverlay::GetExportablePointsInterface(C_overlay *overlay);

protected:
   // Protected destructor. Call Release instead
   virtual ~CPointExportOverlay();
};


// Here is a small function which produces results I can live with, but
// I am not sure if people want it added to the overlay toolkit code.  
// Run this by Vinny sometime.
inline int ovl_color_to_dark( int color )
{
   switch( color )
   {
   case BLACK:   //OVLKIT_COLOR_BLACK:
   case STD_RED: //OVLKIT_COLOR_DARK_RED:
   case STD_GREEN: //OVLKIT_COLOR_DARK_GREEN:
   case YELLOW: //OVLKIT_COLOR_DARK_YELLOW:
   case BLUE:   //OVLKIT_COLOR_DARK_BLUE:
   case MAGENTA:   //OVLKIT_COLOR_DARK_MAGENTA:
   case CYAN:   //OVLKIT_COLOR_DARK_CYAN:
   case WHITE:   //OVLKIT_COLOR_DARK_GRAY:
      return color;
   case PALE_GREEN:   //OVLKIT_COLOR_MONEY_GREEN:
      return STD_GREEN;  //OVLKIT_COLOR_DARK_GREEN;
   case LIGHT_BLUE:  //OVLKIT_COLOR_SKY_BLUE:
      return BLUE;   //OVLKIT_COLOR_DARK_BLUE;
   case OFF_WHITE:   //OVLKIT_COLOR_CREAM:
   case LIGHT_GREY:  //OVLKIT_COLOR_LIGHT_GRAY:
      return WHITE;  //OVLKIT_COLOR_DARK_GRAY;
   case MEDIUM_GRAY: //OVLKIT_COLOR_MEDIUM_GRAY:
      return WHITE;  //OVLKIT_COLOR_DARK_GRAY;
   case STD_BRIGHT_RED: //OVLKIT_COLOR_RED:
      return STD_RED;  //OVLKIT_COLOR_DARK_RED;
   case STD_BRIGHT_GREEN:  //OVLKIT_COLOR_GREEN:
      return STD_GREEN;   //OVLKIT_COLOR_DARK_GREEN;
   case BRIGHT_YELLOW:  //OVLKIT_COLOR_YELLOW:
      return YELLOW; //OVLKIT_COLOR_DARK_YELLOW;
   case BRIGHT_BLUE: //OVLKIT_COLOR_BLUE:
      return BLUE;   //OVLKIT_COLOR_DARK_BLUE;
   case BRIGHT_MAGENTA: //OVLKIT_COLOR_MAGENTA:
      return MAGENTA;  //OVLKIT_COLOR_DARK_MAGENTA;
   case BRIGHT_CYAN: //OVLKIT_COLOR_CYAN:
      return CYAN;  //OVLKIT_COLOR_DARK_CYAN;
   case BRIGHT_WHITE:   //OVLKIT_COLOR_WHITE:
      return STD_RED;   //OVLKIT_COLOR_DARK_RED;
   default:
      return STD_RED;   //OVLKIT_COLOR_DARK_RED;
   }
}



#endif  //#if !defined(PNTEXPRT_OVERLAY_H)

