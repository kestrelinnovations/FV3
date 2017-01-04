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



// localpnt.h

#ifndef LOCALPNT_H
#define LOCALPNT_H 1

#pragma once

#include "FalconView/include/maps_d.h"
#include "FalconView/include/overlay.h"
#include "FalconView/include/ovl_d.h"

//forward
class C_localpnt_point;
class CLinksEditDlg;
class CLocalPtEditDlg;
class CLocalpntEditDlgControl;
class CLocalPointSet;
class CMinLocalPointSet;
enum elevation_result_t;
struct FVPOINT;
class MapScale;
class OvlFont;
namespace points
{
   class GroupEnumerator;
   class GroupFeature;
   class PointsEnumerator;
   class PointFeature;
}
class TabularEditorDataObject;
class LocalPointDataObject;
class MapProj;
class ActiveMap;
class CTabularEditorDlg;
class CPointOverlayFonts;
class C_lpt_ovl;
class PointsSpatialiteDriver;
class PointsSpatialiteReaderDriver;

///////////////////////////////////////////////////////////////////////

// corresponds to Group_Names table in the file
struct POINT_GROUP
{
   CString name;
   BOOL search;
};

// corresponds to Links table in the file
struct POINT_LINK
{
   CString id;
   CString group_name;
   CString link_name;
};

typedef struct
{
   CString id, group_name, description, dtd_id;
   float horz_acc, vert_acc;
   int elevation;
   CString elevation_source;
   CString elevation_source_map_handler_name;
   short elevation_source_map_series_id;
   double latitude, longitude;
   double altitude_meters;
   CString comment, icon;
   CString country_code;
   char area_code;
   char qlt;

   int index;
} point_info_t;

typedef CList<C_localpnt_point *, C_localpnt_point *> LocalPointList;

class C_localpnt_ovl : 
   public IDispatchImpl<FalconViewOverlayLib::IFvOverlayExportablePoints, &__uuidof(FalconViewOverlayLib::IFvOverlayExportablePoints), &FalconViewOverlayLib::LIBID_FalconViewOverlayLib, /* wMajor = */ 1>,
   public CFvOverlayPersistenceImpl,
   public OverlayElement_Interface,
   public OverlayElementEnumerator_Interface
{
   //TO DO: other reasons why a friend are...
   friend CLocalPtEditDlg;  //so it can set m_editor_dialog to NULL

   friend C_lpt_ovl;
   friend PointsSpatialiteDriver;
   friend PointsSpatialiteReaderDriver;

   // we need CMainFrame::OnDisplayLocalPointIconDialog() as a friend so
   // it can call the private get_display_threshold()
   // friend void CMainFrame::OnDisplayLocalPointIconDialog();

   
public:
   enum EditorMode {SELECT, ADD}; //get and set the editor mode (e.g., "select" or "add")
   static CLocalpntEditDlgControl m_EditDialogControl;  //controls for modeless dialog chum editor
   CLinksEditDlg* m_LinkEditDialog;

public:
   // these lists are only used internally before a database has
   // been created (i.e, after a file->new)
   CList<POINT_GROUP *, POINT_GROUP *> m_group_lst;
   CList<POINT_LINK *, POINT_LINK *> m_links_lst;

   boolean_t get_show_overlay() { return m_show_overlay; }
   void set_show_overlay(boolean_t s) { m_show_overlay = s; }

   void set_target_graphic_printing(boolean_t t) { m_target_graphic_printing_on = t; }
   void set_target_point(CString id, CString group) { m_target_point_id = id; m_target_point_group = group; }

   // private variables
private:

   CRect m_rect;  //TO DO: is this used?

   boolean_t m_target_graphic_printing_on;
   CString m_target_point_id, m_target_point_group;

   //C_localpnt_point* m_current_selection;
   C_localpnt_point* m_current_hilight;
   LocalPointList m_selected_points;
   LocalPointList m_hilighted_points;

   CIconImage* m_image_in_drag;
   CIconImage* m_back_image;

   // used to uniquely identify which local point we're dragging (if any)
   CString m_ID_in_drag;
   CString m_group_name_in_drag;

   boolean_t m_in_drag; //are we dragging something? (yes/no)
   BOOL m_exceeded_drag_threshold;  //have we exceeded the drag threshold at some point? (yes/no)

   BOOL m_bDragging; // are we in drag mode (different than m_in_drag since that doesn't get set until the cursor actually moves)

   HintText m_hint;

   static boolean_t m_bEdit;  //is the editor up? (yes/no)
   CPoint m_drag_start_loc; 
   static EditorMode m_editor_mode;  //(e.g., select, add, etc)

   CToolTipCtrl *m_tool_tips;

   // used to figure out the edit context switches when scale changes relative
   // to the display threshold
   static boolean_t        m_above_threshold;   // currently above the threshold
   static EditorMode       m_current_tool;      // used to restore tool when back under threshold

   //CString m_filespec;  // file specification of overlay
   //CString m_name;      // m_filespec less the path

   CStringArray m_links;// holds the list of links

   bool  m_reload_on_next_draw;

   // stores the value of the display/label threshold so it doesn't
   // need to be read from the registry everytime we need it
   MapScale m_display_threshold;
   MapScale m_label_threshold;

   // this flag can be toggled off even when the overlay is still
   // opened - it is initially set to TRUE upon opening
   boolean_t m_show_overlay;

   CList<C_icon*, C_icon*> m_exportedPoints;
   POSITION m_crntExportedPointsPos;

protected:

   //DAO stuff
   CDaoDatabase *m_database;

   CPointOverlayFonts *m_pPointOverlayFonts;
   LocalPointDataObject *m_data_object;

public:

   virtual void SetOverlayFont(OvlFont& font, bool bClearGroupAndPointFonts = false);
   virtual void SetGroupFont(OvlFont& font, std::string strGroupName, bool bClearPointFonts = false);
   virtual void SetPointFont(OvlFont& font, FVPOINT p);
   virtual OvlFont& GetFont(std::string strPointId, std::string strGroupName);

public:
   // the options page should call this function to update the
   // display threshold and label threshold
   void set_display_threshold(MapScale map_scale) { m_display_threshold = map_scale; }
   void set_label_threshold(MapScale map_scale) { m_label_threshold = map_scale; }

   MapScale get_display_threshold() { return m_display_threshold; }

   // Returns TRUE if this overlay can be edited by the tabular editor
   virtual boolean_t is_tabular_editable() { return TRUE; }
   virtual TabularEditorDataObject* GetTabularEditorDataObject();

   // toggle the tabular editor for this overlay
   void OnTabularEditor();

   void add_current_selection(C_localpnt_point* point); 
   void set_current_selection(C_localpnt_point* point);
   C_localpnt_point *get_current_selection() {
      if (m_selected_points.GetCount() == 0)
      {
         return NULL;
      }
      return m_selected_points.GetHead();
   }
   void remove_point_from_selections(C_localpnt_point *pPoint);

   virtual int file_new();
   virtual int open(const CString& filespec); //open local point overlay
   int clear_ovl();

   static CString id_to_series_name(CString handler_name, long series_id);

protected:
   virtual int ReadPointsFromFile(const CString& strFilespec);
   virtual int SavePointsToFile(CString& strFilespec);

   virtual int ReadGroupsFromFile(const CString& strFilespec);
   virtual int SaveGroupsToFile(CString& strFilespec);

   virtual int ReadLinksFromFile(const CString& strFilespec);
   virtual int SaveLinksToFile(CString& strFilespec);

   virtual int ReadFontsFromFile(const CString& strFilespec);
   virtual int SaveFontsToFile(CString& strFilespec);

   //private member function
private:
   //delete points and remove from list functions
   virtual int delete_point_from_overlay_and_database(C_localpnt_point* point, bool bNotifyClients = true);

   //Icon Dialog (used to drag icons out to map)
   static void display_icon_dlg();
   static void close_icon_dlg();

   void unhilight_current_hilight();
   void set_current_hilight(C_localpnt_point* point);


//   int add_to_point_table(C_localpnt_point *point);

   //database operations
   static void clear_field_info( CDaoFieldInfo &fi);  //used by create db
   int commit_or_rollback_transaction(BOOL* cancel=NULL);
   int save_or_abandon_changes(BOOL save);
   int prompt_to_save_or_abandon_changes_then_peform_request(BOOL* cancel =NULL);

   int create_table_with_points_structure_no_indices_etc(CDaoDatabase* database, const char* table_name);
   int execute_query(CString &SQL, CString query_name ="",
      COleVariant* param0 = NULL, COleVariant* param1 = NULL);
   virtual int update_point_in_database_because_point_moved(C_localpnt_point *point, SnapToInfo *snap_to_info);
   int load_list_from_points_table(CDaoDatabase* database, MapProj* map);
   POSITION get_local_point_position_in_list(const char* id, const char* group_name) const;
   BOOL override_drag_lock_move(); //called by drop()
   BOOL override_drag_lock_delete(); //called by delete_local_point()
   BOOL we_are_below_the_label_threshold(const MapProj* map);
   BOOL we_are_below_the_display_threshold(const MapProj* map);

   bool crosses_date_line(const d_geo_t in_ll, const d_geo_t in_ur) const;

   int split_across_date_line(const d_geo_t in_ll, const d_geo_t in_ur,
      d_geo_t& piece1_ll, d_geo_t& piece1_ur, d_geo_t& piece2_ll, d_geo_t& piece2_ur) const;

   int load_points_in_non_dateline_intersecting_rectangle(MapProj *map, CDaoDatabase *database,
       const d_geo_t ll, const d_geo_t ur, CList<C_icon *, C_icon *> *list) const;

   // Determines if the given local point is the one that
   // the user is dragging across the screen. It's useful
   // when you're iterating through all the local points.
   bool is_point_in_drag(C_localpnt_point* local_point);

   // Stores all the information that needs to be kept
   // about the local point that the user has begun to
   // drag across the screen.
   void store_drag_info(C_localpnt_point* local_point);

protected:
   int get_group_name_info(const CString& group_name, BOOL& exists, BOOL& search) const;
   int set_group_name_search(const CString& group_name, BOOL search);

   //new stuff
   //TO DO: comment these
   void close_and_delete_dao_objects();
   void unselect_current_selections();

   //delete all of the records in the deleted and modified tables
   int create_and_open_member_database(); 

public:
   C_localpnt_ovl(CString name = "");    // Constructor

BEGIN_COM_MAP(C_localpnt_ovl)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvOverlay)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvOverlayExportablePoints)
END_COM_MAP()

   virtual HRESULT Initialize(GUID overlayDescGuid);

protected:
   // use Release instead
   virtual ~C_localpnt_ovl(); // Destructor

    // IFvOverlayExportablePoints Methods
public:
   STDMETHOD(get_PointType)(long* pPointType);

   // Return a filter string representing the current overlay's properties
   STDMETHOD(raw_GetFilterFromProperties)(BSTR* pbstrFilter);

   // Set an overlay's properties based on the given filter string
   STDMETHOD(raw_SetPropertiesFromFilter)(BSTR bstrFilter);

   // enumeration of exportable points
   //
   STDMETHOD(raw_SelectPointsInRectangle)(double dLowerLeftLat, double dLowerLeftLon, 
      double dUpperRightLat, double dUpperRightLon, long *pbResult);
   STDMETHOD(raw_MoveNext)(long *pbResult);

   STDMETHOD(get_Latitude)(double* pdLatitude);
   STDMETHOD(get_Longitude)(double* pdLongitude);
   STDMETHOD(get_PointKey)(BSTR* pbstrPointKey);

   virtual void set_modified(boolean_t mod_flag);


   //TO DO: evaluate which ones of these should be public
   int fill_local_point_list_from_database(MapProj* map);

   int load_points_in_rectangle(MapProj *map, const d_geo_t ll, const d_geo_t ur, 
      CList<C_icon *, C_icon *> *list, CPointOverlayFilter *pFilter);

   CPointOverlayFilter *create_point_overlay_filter(const CString& registry_key);

   CString get_filter_str();
   bool properties_differ(CString filter);
   void set_properties_from_filter(CString filter);

   int open_edit_dialog_and_set_focus_to_current_selection_if_any();
   int close_edit_dialog_but_ask_to_save_changes();
   
   CDaoDatabase* get_database_pointer() {return m_database;}
   
   int update_point_in_points_table(C_localpnt_point *point,
      CString& db_id, CString& db_group_name, int command=0);
   int add_to_group_name_table(CString group_name, BOOL search = TRUE, BOOL update_tabular_editor = TRUE);

   //hilight/selection 
   C_localpnt_point* get_current_hilight() {return m_current_hilight;}

   // edit mode functions
   static EditorMode get_editor_mode() {return m_editor_mode;}
   static void set_editor_mode(EditorMode mode);  //set the editor mode to the specified mode
   virtual void enter_edit_focus();   //see overlay.h
   virtual void release_edit_focus(); //see overlay.h
   boolean_t get_edit_mode() { return m_bEdit; };
   //TO DO: need this?
   //BOOL does_edit_dialog_exist() {return (m_edit_dialog!=NULL);}


   //turn on/off the local point editor dialog
   static int set_edit_on(boolean_t edit);
   
   //drag & drop functions - see overlay.h for more info
   void drag(ViewMapProj* map, CPoint point, UINT nFlags, 
      HCURSOR *cursor, HintText **hint);
   void drag_select(CWnd* view, UINT nFlags, CPoint point, HCURSOR* cursor);
   virtual void drop(ViewMapProj* map, CPoint point, UINT nFlags);
   virtual void cancel_drag(ViewMapProj* map);
   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);
   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags);
   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);
   //handles test_selected and selected function calls
   int selected_union_test_selected(MapProj* map, CPoint point, UINT flags, 
      boolean_t *drag, HCURSOR *cursor, HintText **hint, BOOL test);

   //class identification functions
   const char *get_class_name() { return "C_localpnt_ovl"; }

   //keyboard input processing
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT nChar,
                                     UINT nRepCnt, UINT nFlags, CPoint mouse_pt);

   //drawing

   int draw(ActiveMap* map);
   
   void set_reload_on_next_draw();

   static void edit_obj_properties_callback(ViewMapProj *map, C_icon *point);
   static void edit_obj_properties(C_localpnt_point* point);

   //put up remark dialog
   int show_info(const char* id, const char* group_name);
   int show_info(C_localpnt_point* point);
   static void show_info_callback(ViewMapProj *map, C_icon *point);

   //used for popup menu
   void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list);

   virtual boolean_t paste_OLE_data_object( ViewMapProj* map, CPoint* pPt, COleDataObject* data_object);
      
   static HCURSOR get_default_cursor();

   virtual int receive_new_point(ViewMapProj* map, CPoint pt, UINT flags, 
      C_drag_item* drag_item);
   
   // determine if the point hits the object's selection region(s)
   C_icon *hit_test(MapProj* map, CPoint point);
   C_icon *icon_hit_test(const MapProj* map, CPoint point);
   
   //snap to functions
   boolean_t test_snap_to(ViewMapProj* map, CPoint point);
   boolean_t do_snap_to(ViewMapProj* map, CPoint point, 
      CList<SnapToInfo *, SnapToInfo *> &snap_to_list);

   // Returns FALSE to indicate the read-only local point file can not be read.
   // excuse will contain the reason why this is so, and help_id will contain
   // the context sensitive help id.
   static boolean_t open_read_only_check(const char *filespec, CString &excuse,
      UINT &help_id)
   {
      excuse = "they are MS Access Database files";
      help_id = 29465;
      return FALSE;
   }

   // Excerpt from overlay.h:
   // Inform the overlay that it is about to be closed, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the close is canceled.  When this function is called with cancel
   // set to NULL the overlay will be closed no matter what.
   virtual int pre_close(BOOL* cancel);

   static BOOL create_db(CString filespec);

   void release_focus();  //release info box focus

   // Save changes to the local point database
   virtual int save();

   // saves the points overlay into an access database with the given name
   virtual int save_as(const CString & pathname, long nSaveFormat);

   //TO DO: are the following called?
   static const char *filename_template() 
   { return "%s\\local%d.lpt"; }
   static const char *get_default_extension_static()
   { return "lpt"; }
   
   virtual const char *get_default_extension() { return get_default_extension_static(); }

   static CString get_local_point_database_filespec();
   static int set_local_point_database_filespec(CString &text);
   void check_that_database_is_open_and_reopen_as_needed();

   C_localpnt_point* find_local_point_in_list(const char* id, const char* group_name) const;

   static void set_snap_to_info(C_localpnt_point *point, SnapToInfo *snap_to_info);

   int save_as_lps(const CString & pathname);
   int open_lps(const CString & pathname);

private:
   void ClearGroups();
   C_localpnt_point* ReadPoint(points::PointsEnumerator* enumerator);
   void StorePoint(points::PointsEnumerator* enumerator);
   void StoreGroup(points::GroupEnumerator* group_enumerator);

   points::GroupFeature ConvertToGroupFeature(POINT_GROUP* group);
   points::PointFeature ConvertToPointFeature(C_localpnt_point* point);

protected:
#ifdef GOV_RELEASE
void SaveToFeaturesOptionsEditor(const CString & pathname);
#endif

public:
   // get the elevation
   void get_best_point_elevation(double lat, double lon, int& elevation, CString& elevation_source,
                                 CString& new_elev_src_map_handler_name, short& new_elev_src_map_series_id);


   //new stuff
   //tO DO: put this in order
   C_localpnt_point* get_next_in_point_list_cycle(POSITION &pos, int direction);
   void cycle(MapProj* map, POSITION pos, int direction = 1 ); 
   static void delete_local_point_callback(ViewMapProj *map, C_icon *point);

   static void target_graphic_printing_callback(ViewMapProj *map, C_icon *point);
   void target_graphic_printing_callback_hlpr(C_icon *point);

   static void overlay_element_callback(ViewMapProj* pMap, C_icon* pIcon, LPARAM lpData);
   void overlay_element_callback_hlpr(C_localpnt_point* pPoint, int nMenuItemIndex);

   // request that the overlay manager close the overlay, return cancel = TRUE if
   // close was canceled by the user (I don't know how to keep the user from 
   // having the oportunity to cancel - Kevin)
   virtual int Close(BOOL &cancel);

   // Get the full file specification of the local point database.
   // virtual function from C_overlay.
   //const CString & get_specification() const { return m_filespec; }

   // Get the file name of the local point database.
   // virtual function from C_overlay.
   //const CString & get_name() const { return m_name; }

   // callback and implementation functions for bringing
   // up the Links Edit dialog (and filling it appropriately)
   static void edit_local_point_links_callback(ViewMapProj *map, C_icon *point);
   static void edit_local_point_links(C_localpnt_point* point);

   // updates and fills the Links Edit dialog as necessary
   void set_link_edit_dlg_focus(C_localpnt_point* point);

   // callback and implementation functions for Links Edit dlg to
   // save the links for the given handle (i.e. a gievn local point)
   static void static_save_links(CString handle, CStringArray &links);
   virtual void save_links(CString handle, CStringArray &links);

   // callback and implementation functions for viewing a link
   // selected from the right-button context menu
   static void view_link_callback(ViewMapProj *map, C_icon *point, LPARAM index);
   static void view_link(C_localpnt_point* point, LPARAM index);

   // Fills the "links" parameter with the appropriate strings
   // representing that point's links
   void get_links_for_point(C_localpnt_point* point, CStringArray &links);
   void get_links_for_point(CString id, CString group_name, CStringArray &links);
   void get_links_for_point_unnamed(CString id, CString group_name, CStringArray &links);
   virtual void AddLink(CString id, CString group_name, CString strLink);

   // creates a Links table in the localpoint database, and searches
   // for any existing links in the Points table and copies them there
   void convert_localpnt_database();

   // creates a links table with the name in "table_name"; if a database
   // is supplied, it creates it there, otherwise it uses CDBOpenerCloser
   // to open the localpoint database
   int  create_links_table(CString table_name, CDaoDatabase* database = NULL);

   // deletes all records in the Links table for the point specified
   // by "id" and "group_name"
   virtual void delete_links_for_point(CString id, CString group_name);

   // activates links for the item double-clicked, if any; called
   // by C_ovl_mgr::on_dbl_click()
   virtual int on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point);

   // Takes run-time information and sets the properties for the point's
   // record in the database.
   void set_point_properties_in_db(C_localpnt_point *point, SnapToInfo *snap_to_info, 
                                   CLocalPointSet *set, BOOL bMapElevationLookup = TRUE);

   // more C_overlay overrides 
   boolean_t get_drag_lock_state();
   void set_drag_lock_state(boolean_t drag_state);

   CString m_comment;
   CString m_dtd_id;
   CString m_description;
   CString m_id;
   CString m_group_name;
   double    m_latitude;
   double    m_longitude;
   CString m_elevation_source;
   int m_elevation;
   float m_vert_accuracy;
   float m_horz_accuracy;

   virtual int apply_changes(C_localpnt_point* point, point_info_t point_info, BOOL redraw);
   virtual int apply_changes(C_localpnt_point* point, CString icon,
      CString comment, CString dtd_id, CString description, CString id, 
      CString group_name, double latitude, double longitude,
      double altitude_meters, CString elevation_source,
      CString elevation_source_map_handler_name,
      short elevation_source_map_series_id,
      int elevation, float vert_accuracy, float horz_accuracy, BOOL redraw,
      bool bNotifyClients = true);


   int update_point_record(C_localpnt_point* point, CString icon, CString comment, CString dtd_id, CString description, CString id, 
                           CString group_name, double lat, double lon, CString elevation_source,
                           CString elevation_source_map_handler_name, short elevation_source_map_series_id,
                           int elevation, float vert_accuracy, float horz_accuracy);


   int lookup_info(C_localpnt_point *point, point_info_t *point_info);
   int lookup_info(C_localpnt_point *point, CString& id, 
                   CString& group_name, CString &description, CString &m_dtd_id, 
                   float &horz_accuracy, float &vert_accuracy, int &elevation, CString &elevation_source,
                   CString &elevation_source_map_handler_name, short &elevation_source_map_series_id,
                   double &latitude, double &longitude, double& altitude_meters,
                   CString &comment, CString &icon,
                   CString &country_code, char &area_code, char &qlt);

   // obtain the list of group names for the overlay
   CString get_group_list();

   int delete_local_point(MapProj* map, C_localpnt_point* point, bool bNotifyClients = true);

   boolean_t does_key_exist(CString id, CString group_name);

   virtual int insert_point(ViewMapProj* map, C_localpnt_point *point, SnapToInfo* snap_to_info,
      BOOL bSetModified = TRUE, BOOL bMapElevationLookup = TRUE, bool bNotifyClients = true, bool bCallUpdateData = true);

   bool m_isImporting;
   bool m_allSelected;
   elevation_result_t m_elevationResult;
   void set_importing(bool isImporting);

    // IFvOverlayElement
public:
   // registers varDispatch to receive callbacks
   virtual HRESULT RegisterForCallbacks(VARIANT varDispatch);

   // Updates an element of the overlay using the given XML string
   virtual HRESULT UpdateElement(BSTR bstrElementXml);

   // IFvOverlayElementEnumerator
public:
   // The XML that describes the current element
   virtual BSTR bstrCurrentXml();

   // Resets the enumerator.  After a call to Reset, the current element will be NULL until MoveNext is called
   virtual HRESULT Reset(); 

   // Moves to the next element in enumeration
   virtual HRESULT MoveNext(long *pbRet);

   std::string GetOverlayGuid() { return m_strOverlayGuid; }   

   void Add3DPoint(C_localpnt_point* point);
   void Update3DPoint(C_localpnt_point* point);
   void Remove3DPoint(C_localpnt_point* point);

   void SetWorkItemsStream(IStream* stream)
   {
      m_work_items_stream = stream;
   }
   FalconViewOverlayLib::IWorkItemsPtr GetMarshaledWorkItems();

protected:
   std::vector<fvw::IOverlayElementCallbackPtr> m_vecOverlayElementCallbacks;
   void OnElementModified(C_localpnt_point *pPoint, bool bDeleteElement);

   std::string m_strOverlayGuid;
   POSITION m_posEnumerator;
   CString m_strCurrentElementXml;

   CList<CString, CString> m_listObjectMenuItems;

   // Used to queue up work for 3D elements in the backgruond 
   IStream* m_work_items_stream;
   FalconViewOverlayLib::IWorkItemsPtr m_marshaled_work_items;

}; /* end of C_localpnt_ovl class declaration */

namespace points_overlay_utils
{
   void Add3DPoint(FalconViewOverlayLib::IDisplayElements* display_elements,
      C_localpnt_point* point, OvlFont& point_font);

   void Update3DPoint(FalconViewOverlayLib::IDisplayElements* display_elements,
      C_localpnt_point* point, OvlFont& point_font);
}

#endif    // ifndef LOCALPNT_H
