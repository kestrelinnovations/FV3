// Copyright (c) 1994-2009,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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

// shp.h -- shape file overlay header

#ifndef SHP_H
#define SHP_H 1

#include "overlay.h"
#include "maps_d.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "..\shp\shapefil.h"

#ifdef HEAP_TRACE
#  define HEAP_REPORT( where ) HeapTrace( where );
#else
#  define HEAP_REPORT( where )
#endif

// forward declarations
struct SHPDisplayProperties;
struct SHPObject;
class CDbase;
class HintText;

static const COLORREF SELECTED_SHAPE_COLOR = RGB(255, 255, 0);
static const int SHAPE_NOT_DISPLAYED = INT_MAX;

// values for m_shp_mode
#define MM_SHP_NONE 0
#define MM_SHP_SELECT 1
#define MM_SHP_POINT 2
#define MM_SHP_LINE 3
#define MM_SHP_POLYLINE 4
#define MM_SHP_POLYGON 5
#define MM_SHP_FREEHAND 6

#define SHP_FILTER_NONE  0
#define SHP_FILTER_LIST  1
#define SHP_FILTER_RANGE 2

#define SHP_FILTER_OP_AND 1
#define SHP_FILTER_OP_OR  2

typedef std::set< std::string > FieldExclusionValues;
typedef std::map< std::string, FieldExclusionValues > FileFieldExclusions; // Unresolved field names
typedef std::map< int, FieldExclusionValues > RecordFieldExclusions; // Resolved field names

class UtilDraw;

class C_shp_filter
{
   CString m_icon_text;

public:
   C_shp_filter(CString text = "", BOOL selected = FALSE);
   ~C_shp_filter();

   SHPDisplayProperties *m_pDisplayProperties;

   CString m_text;
   BOOL m_selected;
   int m_num;
   int m_label_code;

   CString GetIconText() { return m_icon_text; }
   void SetIconText(CString iconText);

   C_shp_filter &operator=(C_shp_filter &); 
};
// end of C_shp_filter

class C_shp_found_item
{
public:
   C_shp_found_item()
   {
      m_lat = 0.0;
      m_lon = 0.0;
      m_recnum = -1;
   }

   ~C_shp_found_item()
   {
   }

   int m_recnum;
   int m_idnum;
   CString m_filename;
   CString m_fieldname;
   CString m_found_str1;
   CString m_found_str2;
   double m_lat;
   double m_lon;
};
// end of C_shp_found_item

class CBaseShapeFileOverlay
{
protected:
   struct XY { DOUBLE x,y; };

   // array of shape objects displayed in the overlay
   std::vector< SHPObjectPtr > m_vecShpObjects;

   double m_dHalfWorldDot;
   double m_dWorldX, m_dWorldY;  // Around the world in the current view// Ditto as double
   int m_iWorldX, m_iWorldY;     // Ditto as int
   std::vector< POINT > m_aTempPoints;  // For world unwrapping

   MapScale m_smallest_scale;
   MapScale m_smallest_labels_scale;

   d_geo_t
      m_gptCurScrnLL, m_gptCurScrnUR,  // Current draw
      m_gptPrevScrnLL, m_gptPrevScrnUR;   // Previous draw
   BOOL m_bMapChanged;

   // base title used in the Get Info dialog box
   CString m_infoDialogBaseTitle;

   // pointer to the selected shape object, NULL if none
   SHPObject* m_selected_obj;

   // stores tooltip and help text for a shape object
   HintText* m_hint;

public:
   CBaseShapeFileOverlay();
   ~CBaseShapeFileOverlay();

   // specifies the type of shape objects in the overlay.  All objects in m_vecShpObjects are of the same type
   int m_type;

   HeapUtility::CHeapAllocator m_clsHeapAllocator;

   // If TRUE, then a polygon will only be hit tested on its edges
   BOOL m_near_line_mode;

   // determine if the point hits the object's selection region(s)
   SHPObject* ShapeHitTest(map_projection_utils::CMapProjWrapper* map, const CPoint& pt);
   bool ShapeHitTest(SHPObject* pShapeObject, const CPoint& pt);

   // draw the array of the shape objects to the given map
   int DrawShapeObjects(ActiveMap* map);

   virtual BOOL IsAreaLabelEnabled( const SHPObject& ) const { return TRUE; }

   int OnTestSelected(map_projection_utils::CMapProjWrapper* map, const CPoint& point, HCURSOR *cursor, HintText **hint);

   int OnSelected(C_overlay* pOverlay, map_projection_utils::CMapProjWrapper* map, const CPoint& point, boolean_t *drag, HCURSOR *cursor, HintText **hint);

   void OnMenu(C_overlay* pOverlay, ViewMapProj* map, const CPoint& point, CList<CFVMenuNode*,CFVMenuNode*> & list);
   static void OnMenuItemSelected(ViewMapProj *map, C_overlay* pOverlay, LPARAM lParam);

   virtual void clear_ovl();

protected:
   // drawing routines for shape objects
   void DrawShapeObjectWithParts( ActiveMap* pMap, SHPObject* pShapeObject );
   void DrawShapeObject( ActiveMap* pMap, SHPObject* pShapeObject, BOOL selected );
   void DrawShapeObject( CDC* pDC, SHPObject* pShapeObject, BOOL selected );
   void DrawShapeObjectPoint( CDC* pDC, SHPObject* pShapeObject, BOOL selected );
   void DrawShapeObjectPolyline( CDC* pDC, SHPObject* pShapeObject, BOOL selected );
   void DrawShapeObjectPolygon( CDC *pDC, SHPObject* pShapeObject, BOOL selected );
   void DrawShapeObjectLabels( ActiveMap* pMap, SHPObject* pShapeObject, int iFontSize, BOOL selected );
   void DrawShapeObjectPolylineLabels( CDC* pDC, SHPObject* pShapeObject, int iFontSize, BOOL selected );
   void DrawShapeObjectPolygonLabels( ActiveMap* pMap, SHPObject* pShapeObject, int iFontSize, BOOL selected );
   void draw_point_label(CDC *dc, int x, int y, const char* lpstrLabel) const;
   void draw_line_label( CDC* dc, int x1, int y1, int x2, int y2, int iFontSize,
      const char* lpstrLabel) const;
   void draw_line_labels( CDC *dc, int cVertices, const POINT* pptVertices, int iFontSize, LPCSTR pszLabel ) const;
   std::unique_ptr<POINT[]> DrawPolyline( SHPObject* pShapeObject, BOOL selected,
      UtilDraw* utilDraw, int pen_size_sel, int pen_size_bkgd, int pen_size_norm,
      size_t iPart, int iBias );
   std::unique_ptr<POINT[]> ApplyBias( const SHPObject* pShapeObject, int nNumVertices,
      const POINT* pCurrentPart, int iBias );

   // sort routine for sorting shape objects by their display order
   static bool SortShapesByDisplayOrder( const SHPObjectPtr& a, const SHPObjectPtr& b);

   // retrieves a field with the given name from a .dbf file
   static CString GetField(CDbase* pDbase, LPCSTR fieldName);

   int show_info( C_overlay* pOverlay, SHPObject* pShapeObject );

   // if return TRUE - msg contains list of field name and data
   // if return FALSE - msg contains error message
   BOOL get_shp_info( const CString& filename, int recnum, CString & msg) const;

   // returns the shape file filename (without extension) associated with the
   // given shape object
   virtual std::string GetFilenameFromShape(const SHPObject* pShapeObject) const = 0;

   // returns the hide above threshold for shape objects
   virtual MapScale  get_smallest_scale() = 0;
   virtual void      set_smallest_scale( MapScale ) = 0;

   // returns the hide above threshold for shape object labels
   virtual MapScale  get_smallest_labels_scale() = 0;
   virtual void      set_smallest_labels_scale( MapScale ) = 0;

   // Returns the hide BELOW threshold for shape object labels
   virtual MapScale  get_largest_area_labels_scale(){ return NULL_SCALE; }
   virtual void      set_largest_area_labels_scale( MapScale ){};

   // Whether to try to keep area labels in view
   virtual BOOL      KeepAreaLabelsInView() const { return FALSE; }// Default to no

   // Check for and extract localized shape files from larger master shapes
   int extract_local_shape_files( LPCSTR pszFolderIn, LPCSTR pszFolderOut, LPCSTR pszBaseShapefileName,
      const FileFieldExclusions& mpRecordExclusions, CString& csErrorMsg );

#ifdef HEAP_TRACE
   void              HeapTrace( LPCTSTR pszWhere );
#endif
}; // class CBaseShapeFileOverlay


class C_shp_ovl : 
   public CBaseShapeFileOverlay,
   public CFvOverlayPersistenceImpl
{
protected:
   CString m_icon_text;

public:
   SHPDisplayProperties* m_pDisplayProperties;
   CString GetIconText() { return m_icon_text; }
   void SetIconText(const CString& iconText);

   BOOL m_error_reported;
   int m_label_code;

   static CString m_found_item;

public:
   std::string m_filename;  // full path of the shape file excluding the extention
   std::string m_tooltip_fieldname1;
   std::string m_tooltip_fieldname2;
   std::string m_statbar_fieldname1;
   std::string m_statbar_fieldname2;

   // filter related vars
   int m_filter_mode;
   int m_field_op;
   CString m_fieldname1;
   CString m_fieldname2;
   CString m_fielddata1;
   CString m_fielddata2;
   CString m_field1_text1;
   CString m_field1_text2;
   CString m_field2_text1;
   CString m_field2_text2;

   CString m_filter_fieldname;  
   BOOL m_filter_selects;
   CList<C_shp_filter* , C_shp_filter*> m_filter_list;
   CList<C_shp_filter* , C_shp_filter*> m_fieldnum_list;
   C_shp_filter m_range_filter;

   // Constructor
public:                         
   C_shp_ovl(CString name = "");

   // CFvOverlayPersistenceImpl overrides
public:
   // Inform the overlay that it is about to be saved, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the save is canceled.
   virtual int pre_save(boolean_t *cancel)
   {
      // Shape files cannot be saved
      *cancel = TRUE;
      return SUCCESS;
   }

   static const char *get_default_extension_static() { return "shp"; }
   virtual const char *get_default_extension() { return get_default_extension_static(); }

   // Returns TRUE to indicate the read-only drawing file can be read.
   // excuse will contain the reason why this is so, and help_id will contain
   // the context sensitive help id.   
   static boolean_t open_read_only_check(const char *filespec, CString &excuse, UINT &help_id)
   {
      excuse = "";
      help_id = 29465;
      return TRUE;
   }

   // C_overlay overrides
public:
   virtual int draw(ActiveMap* map);

   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);

   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   virtual void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list);

   virtual void release_focus();

   const char *get_class_name() { return "C_shp_ovl"; }

public:

   virtual int open(const CString& filename);
   int save(CString filename);

   // cannot save Shape files
   virtual int save_as(const CString & pathname, long nSaveFormat) { return FAILURE; }

   void clear_filter_list();

   // re-apply filter and label properties to shape objects
   void OnSettingsChanged();

   void load_properties();
   void save_properties();
   void fill_filter_list(); 
   int save_pref_file(CString &error_txt); 

   static int LoadPreferences(const std::string& shape_filename,
      SHPDisplayProperties* display_properties, int* filter_mode,
      BOOL* near_line_mode, int* label_code,
      std::string* tooltip_fieldname1, std::string* tooltip_fieldname2,
      std::string* statbar_fieldname1, std::string* statbar_fieldname2,
      CString* fieldname1, CString* fieldname2,
      CString* field1_text1, CString* field1_text2,
      CString* field2_text1, CString* field2_text2,
      int* field_op, CString* filter_fieldname,
      CString* icon_text, CList<C_shp_filter* , C_shp_filter*>* filter_list,
      std::string *error_txt);
   void assign_filter_prefs();
   BOOL is_pref_file_writable();
   static int readline(CFile *file, char *buf, int maxlen);
   static BOOL is_valid_ovl_ptr(C_overlay *testovl); 

   // search functions
public:
   static CString GetSearchSuffix(const CString& str);
   static BOOL is_shape_search_key(const CString& str);
   static int search(const CString& keystr, d_geo_t *center_geo, CString & filename);
   static BOOL select_item(const CList<CString*, CString*> & strlist, int *num);

   int search(const CString& keystr, int mode, BOOL search_all, BOOL top_only, CList<C_shp_found_item*, C_shp_found_item*> & found_list);
   BOOL search_db(const CString& filename, const CString& fieldname1, const CString& fieldname2, const CString& keystr, 
      int mode, BOOL search_all, BOOL top_only, CList<C_shp_found_item*, C_shp_found_item*> & found_list);
   static CString soundex(const CString& input);
   static void remove_spaces(CString & text);
   BOOL filter_hit() const;
   static BOOL is_number(const CString& txt);

   virtual void Finalize();

   // CBaseShapeFileOverlay overrides
protected:

   // returns the filename associated with the given shape object
   virtual std::string GetFilenameFromShape( const SHPObject* pShapeObject ) const override;

   // returns the hide above threshold for shape objects
   virtual MapScale get_smallest_scale() override; 
   virtual void set_smallest_scale( MapScale ) override;

   // returns the hide above threshold for shape object labels
   virtual MapScale get_smallest_labels_scale() override;
   virtual void set_smallest_labels_scale( MapScale ) override;

   // Protected destructor. Call Release instead
   ~C_shp_ovl();

}; // end of C_shp_ovl class declaration 

// ************************************************************************
// ************************************************************************

#endif  // #ifndef SHP_H
