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



#ifndef _FLY_OVLY_H_
#define  _FLY_OVLY_H_
#include "common.h"
#include "ovl_d.h"
#include "overlay.h"
#include "..\SkyViewOverlay\obj_dlg.h"
#include "FvMappingCore\Include\MapProjWrapper.h"

// forward declarations
class SkyViewInterface;
class OvlSymbol;
class CLinksEditDlg;
class GeoBounds;
class GeoLine;
class SkyViewProperties;
class OvlText;
class COverlayCOM;

class Csym_icon : public C_icon
{

private:
   // Members
   long                    m_id;                // symbol identifier
   degrees_t               m_latitude;          // geographic
   degrees_t               m_longitude;         //   coordinates
   CRect                   m_text_rect;         // label bounding rectangle
   C_overlay*              m_overlay;           // overlay for this point
   degrees_t               m_true_heading;
   float                   m_msl;               // mean sea level altitude 

   long                    m_flags;             // can we move or rotate this?
   long                    m_message_types;     // types of messges to forward to the notigy window

   CString                 m_help_text;         //last help text
   OvlSymbol               *m_ship_symbol;

   COLORREF                m_foreground;        // setandard unselected colors
   COLORREF                m_background;

   CPoint                  m_screen_loc;        // updated in update_pos (called by draw()) based on lat lon
   CRect                   m_rect;              // hit test rectangle 

   CPoint                  m_drag_loc;          // used between begin drag and end drag
   d_geo_t                 m_drag_geoloc;       //  ''   ''        ''  ''       ''
   degrees_t               m_drag_heading;      //  ''   ''        ''  ''       ''
   float                   m_drag_msl;          //  ''   ''        ''  ''       ''
   CRect m_old_rect;
   CPoint                  m_drag_offset;       // used to know where the icon was grabed relative to center
   BOOL                    in_drag;
   BOOL                    in_rotate;
   BOOL                    m_last_draw_was_XOR; // use this to verify that a drag has not been interupted by a redraw

   BOOL                    m_selected;          // draw highlight
   BOOL                    m_infoselected;      // draw info box highlight
   BOOL                    m_rotate_handles;    // should highlight have rotate handles?


public:

   enum enumHitType {NONE=0,IN_RECT=1,ROTATE=2};

   // Functions/Operators
   Csym_icon();
   ~Csym_icon();

   void set_id(long id) {m_id = id;};
   long get_id() {return m_id;}

   // note these functions do not dorce a redraw (you must invalidate for that
   void set_location(degrees_t lat, degrees_t lon) {m_latitude = lat;m_longitude = lon;}
   double get_latitude() {return m_latitude;}
   double get_longitude() {return m_longitude;}

   void set_heading(degrees_t heading) {m_true_heading = heading;}
   double get_heading() {return m_true_heading;}

   void set_msl(float elev) {m_msl = elev;}
   float get_msl() {return m_msl;}

   //   HWND get_notify_wnd() {return m_notify_wnd;}
   //   void set_notify_wnd(HWND hwnd) {m_notify_wnd = hwnd;} 

   long get_message_types() {return m_message_types;}
   void set_message_types(long types) { m_message_types = types;}

   C_overlay *get_overlay() {return m_overlay;}

   boolean_t is_kind_of(const char* class_name);
   const char *get_class_name() { return "Csym_icon"; };

   OvlSymbol *get_ship_symbol() { return m_ship_symbol; }

   CString get_help_text();
   CString get_tool_tip();
   CString get_alt_string();

   void set_symbol(long type);

   // returns enum_hit_type
   int hit_test(CPoint point);

   // does a SendMessage of position info to notify_wnd 
   int send_drop_message(HWND notify_wnd);

   // does a SendMessage of position info to notify_wnd 
   int send_drag_message(HWND notify_wnd);

   // select or unselect icon and invalidate area
   void unselect();
   void draw_selected();
   // draw info box and icon and invalidate area
   void un_infoselect();
   void draw_infoselected();

   void draw(ActiveMap *map);

   // apply an changes in lat/lon/heading 
   void update_position(MapProj* map);

   // mark the current rectangle of viewpoint as invalid
   CRect get_rect() {return m_rect;};
   void invalidate();


   // move accorting to mouse input pt
   // return new center point
   void begin_drag(map_projection_utils::CMapProjWrapper* map, CDC *pDC, CPoint pt);
   CPoint end_drag(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt);
   CPoint move_drag(map_projection_utils::CMapProjWrapper* map, CDC *pDC, CPoint pt);
   void cancel_drag(map_projection_utils::CMapProjWrapper *map, CDC *pDC);

   // perform a drag (similar to move_drag()) but constrain the position to the
   // provided route leg.  Returns the new drag position.
   // ppNewLeg gets the new constraining leg in the event that the pt has moved
   // off the end of pLeg.  Therfore ppNewLeg may be pLeg or the next or the previous leg.
   // does not update the lat/lon.
   CPoint move_drag_leg(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt, COverlayCOM *pRoute);
   CPoint end_drag_leg(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt, COverlayCOM *pRoute);


   // rotate until initial grab pt is on a line from center to point pt
   // return new heading
   void begin_rotate(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt);
   double end_rotate(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt);
   double move_rotate(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt); 
   void cancel_rotate(map_projection_utils::CMapProjWrapper *map, CDC *pDC);

private:

   // helper function which takes a lat lon and a route leg and returns the closest lat lon on the leg,
   // the elevation of the leg and the heading of the leg
   // return 0 on success, return 1 if atlitude was not obtained
   int get_leg_position_info(d_geo_t geo, COverlayCOM *pRoute,double *clat,double *clon,float *new_alt, double *new_head) const; 

   void DrawEllipseHandle( CDC* pDC, const CPoint& pt ) const;
   void draw_rotate_handles( CDC* pDC, const CRect& rect ) const;

   BOOL EllipseHandleHit(const CPoint&hdl_loc, const CPoint& test_pt ) const;
   BOOL rotate_handles_hit(const CRect& rect, const CPoint& pt) const; 

   void define_symbol_airlift();
   void define_symbol_bomber();
   void define_symbol_fighter();
   void define_symbol_helicopter();
   void define_symbol_viewport();
};

// SkyViewObject - this class is used to house a skyview object.  A 
// skyview object is displayed as an icon in the FalconView screen and as 
// a 3D model in SkyView.  The functionallity of these objects is analogous
// to two-dimensional local points
class SkyViewObject : public C_icon
{
protected:
   // position of the object
   d_geo_t m_geo_location;

   // msl altitude of the object
   float m_altitude_msl;

   // associated 2-D icon for the object displayed in FV
   CIconImage* m_icon_image;

   // handle to the object in Skyview, assigned by skyview interface
   int m_handle;

   // position of the object in FalconView in screen coordinates
   int m_x, m_y;

   // the bounding rect of the icon on the screen
   CRect m_rect;

   // the color of the object as seen in SkyView
   COLORREF m_color;

   // flag stating whether this object should be display in wireframe or solid
   boolean_t m_is_solid_not_wireframe;

   // units that the values for the object are displayed to the user.  All
   // values, however, are always stored in meters
   units_t m_units;

   // object dimensions
   float m_length;
   float m_width;
   float m_height;

   // orientation
   float m_heading;
   float m_pitch;
   float m_roll;

   CString m_title;

public:
   // constructor
   SkyViewObject(C_overlay *parent_overlay);

   // draws the current object as a 'selected' or 'hilighted' icon
   void draw(map_projection_utils::CMapProjWrapper* map, CDC* dc, boolean_t selected, 
      boolean_t hilighted);

   void calc_screen_coords(map_projection_utils::CMapProjWrapper *map);

   // invalidate the icon
   void invalidate();

   // get/set the object's handle
   void set_handle(int handle) { m_handle = handle; }
   int get_handle() { return m_handle; }

   // C_icon overrides
   //
public:

   // Returns TRUE if this icon is a instance of the given class.
   // For example: is_kind_of("C_icon") would return TRUE.  Derived classes
   // should implement an is_kind_of function that returns TRUE for their
   // class name and returns C_icon::is_kind_of(clase_name) otherwise.
   virtual boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   virtual const char *get_class_name() { return "SkyViewObject"; }

   // Descriptive string functions
   virtual CString get_help_text();
   virtual CString get_tool_tip();

   // Determine if the point is over the icon.
   virtual boolean_t hit_test(CPoint point);

   // methods to return the tooltip and helptext
   virtual CString get_tooltip();
   virtual CString get_helptext();

   //
   // get/set functions
public:

   // get the latitude of the object
   virtual degrees_t get_lat() const { return m_geo_location.lat; }

   // get the longitude of the object
   virtual degrees_t get_lon() const { return m_geo_location.lon; }

   // get the altitude of the object
   virtual float get_altitude_msl() { return m_altitude_msl; }

   // set the position/altitude of the object
   void set_position(degrees_t lat, degrees_t lon)
   {
      m_geo_location.lat = lat;
      m_geo_location.lon = lon;
   }
   void set_position(d_geo_t geo) { m_geo_location = geo; }


   void set_altitude_msl(float alt_msl) { m_altitude_msl = alt_msl; }

   // set the icon displayed for this object
   void set_icon_image(CIconImage *icon) { m_icon_image = icon; }
   CIconImage *get_icon_image() { return m_icon_image; }

   void set_color(COLORREF color) { m_color = color; }
   COLORREF get_color() { return m_color; }

   void set_is_solid_not_wireframe(boolean_t snw) { m_is_solid_not_wireframe = snw; }
   boolean_t is_solid_not_wireframe() { return m_is_solid_not_wireframe; }

   void set_units(units_t u) { m_units = u; }
   units_t get_units() { return m_units; }

   void set_length(float l) { m_length = l; }
   float get_length() { return m_length; }

   void set_width(float w) { m_width = w; }
   float get_width() { return m_width; }

   void set_height(float h) { m_height = h; }
   float get_height() { return m_height; }

   void set_heading(float h) { m_heading = h; }
   float get_heading() { return m_heading; }

   void set_pitch(float p) { m_pitch = p; }
   float get_pitch() { return m_pitch; }

   void set_roll(float r) { m_roll = r; }
   float get_roll() { return m_roll; }

   void set_title(CString t) { m_title = t; }
   CString get_title() { return m_title; }

   // get the bounding rect
   CRect get_rect() { return m_rect; }

   boolean_t in_view(MapProj *map);

public:
   // list of links for this object
   CStringArray m_links;

   // Add links for this object, if there are any.
   void add_links_to_menu(CList<CFVMenuNode*,CFVMenuNode*> &list);

   // View the link at index for the point represented by icon
   static void view_link_callback(ViewMapProj *map, C_icon* icon, LPARAM lparam);

   // return the number of bytes required to write this object to disk
   int get_block_size();

   // serialize this objects data into the given block
   int Serialize(BYTE *&block_ptr);

   // deserialize the data from the given block into this object
   int Deserialize(BYTE *&block);

   // return the info text displayed in the info dialog for the object
   CString get_info_text();

   // View all links attached to this object, if any.
   virtual void view_all_links();
};

struct notify_wnd
{
   HWND hwnd;
   long msg_types;      // bitfield of types of messages to respond to.
   int  retries_left;   // because there is a blocking send, if a send times out decrement
   // the retries and fail,  When retries reach 0 stop trying to send.
};

/////////////////////////////////////////////////////////////////
//  Moving Symbol Overlay:
//  Class that adds an overlay thich supports a com interface for
//  Adding symbols that can be moved/rotated and that notify the client
//  when the user drags/drops them.
//////////////////////////////////////////////////////////////////
class Cmov_sym_overlay : 
   public CFvOverlayPersistenceImpl
{
public:

   enum enumEditorMode {SELECT_SYMBOL_MODE, SELECT_TERRAIN_MODE};

   static ObjectDlg m_obj_properties_dlg;
   static BOOL s_show_skyview_symbol;

private:
   class StateIndicators
   {
   public:

      StateIndicators();
      ~StateIndicators();

      void draw(ActiveMap *map);
      void clear_state_indicators();
      void update_altitude(double Latitude, double Longitude, double Heading, double Altitude);
      void redefine_symbol();

      void update_terrain_bounds(BOOL terrain_is_loaded,
         double terrain_bounding_lat_south, double terrain_bounding_lon_west, 
         double terrain_bounding_lat_north, double terrain_bounding_lon_east);

      void update_frustum(
         double camera_lat, double camera_lon,
         BOOL terrain_is_loaded,
         double terrain_bounding_lat_south, double terrain_bounding_lon_west,
         double terrain_bounding_lat_north, double terrain_bounding_lon_east,

         BOOL terrain_point_seen_at_upper_left_screen_point_found,
         double terrain_point_seen_at_upper_left_screen_point_lat,
         double terrain_point_seen_at_upper_left_screen_point_lon,

         BOOL terrain_point_seen_at_upper_right_screen_point_found,
         double terrain_point_seen_at_upper_right_screen_point_lat,
         double terrain_point_seen_at_upper_right_screen_point_lon,

         BOOL terrain_point_seen_at_lower_left_screen_point_found,
         double terrain_point_seen_at_lower_left_screen_point_lat,
         double terrain_point_seen_at_lower_left_screen_point_lon,

         BOOL terrain_point_seen_at_lower_right_screen_point_found,
         double terrain_point_seen_at_lower_right_screen_point_lat,
         double terrain_point_seen_at_lower_right_screen_point_lon);

      Csym_icon *m_symbol;
      GeoBounds *m_terrain_bounds;
      GeoLine *m_frustum_line[4];
      SkyViewProperties *m_properties;
      OvlText *m_altitude_text;

   private:
      bool find_intersection(double lat1, double lon1, 
         double camera_lat, double camera_lon, double north_lat, double west_lon, 
         double south_lat, double east_lon, double &intersect_lat, double &intersect_lon);

   } static *s_pStateIndicators;

public:
   static StateIndicators* GetStateIndicators()
   {
      if (s_pStateIndicators == NULL)
         s_pStateIndicators = new StateIndicators();

      return s_pStateIndicators;
   }
   static void DestroyStateIndicatorsInstance()
   {
      delete s_pStateIndicators;
      s_pStateIndicators = NULL;
   }

private:
   enum enumDragOperation {Move,Rotate,None} m_drag_operation;
   BOOL m_bDragging;


   static CList <notify_wnd*, notify_wnd*> m_window_list;

   // the collection of SkyView objects - displayed as 2D icons in
   // FalconView, 3D textured objects in SkyView
   CList <SkyViewObject*, SkyViewObject *> m_object_lst;

   // the currently selected SkyView object
   SkyViewObject *m_current_selection;

   // used to erase the edit handles when we leave the overlay
   boolean_t m_already_unselected_current_selection;

   // the currently hilighted SkyView object
   SkyViewObject *m_current_hilight;

public:
   static Csym_icon     *m_pviewpoint; // currently selected symbol
   static Csym_icon     *m_pinfopoint; // symbol with info box focus
   static BOOL           m_in_drag;

private:

   HintText       m_hint;        // what we display 

   static CPoint         m_drag_start_loc;
   static BOOL           m_exceeded_drag_threshold;
   static BOOL           m_snap_to_legs;   // should the viewpoint snap to legs that it is dragged across
   static COverlayCOM       *m_plocked_route;

   static enumEditorMode      m_editor_mode;
   enumDragOperation  m_drag_mode;

   // this static variable holds a pointer to the first instance of the overlay with a symbol on it.
   // since this is a static overlay there should be only one instance.
   static boolean_t m_bEdit;

   static CLinksEditDlg *m_link_edit_dlg;

   // an additional CIconImage is created for each .obj file in 
   // HD_DATA/objects/skyview.  These are then added to the 3d icon palette
   // when it is brought up
   static CList <CIconImage *, CIconImage *> m_object_icon_list;
   static boolean_t m_object_icons_initialized;

public:
   // this holds the encapsulated interface to SkyView.  Gets created/destroyed as
   // m_active_overlay gets set
   static SkyViewInterface *m_skyview_interface;
   static bool m_deleting_skyview_interface;

   void add_object(SkyViewObject *object) { m_object_lst.AddTail(object); }

   // delete the given point from the object list and cycle to the next visible
   // point
   int delete_point(ViewMapProj *map, SkyViewObject *object);

   // cycle to the next object in the list that is visible on the screen
   int cycle(ViewMapProj *map, SkyViewObject *object, 
      boolean_t direction_forward = TRUE);

   // return the next object in the list.  loop around in the given direction
   SkyViewObject *get_next_object(POSITION& position, 
      boolean_t direction_forward);

   SkyViewObject *get_current_selection() { return m_current_selection; }

   void update_object_in_skyview(SkyViewObject *object);

   // add each of the objects in the object list to SkyView via skyview's
   // automation interface
   void add_object_list_to_skyview();

public:
   // Constructor
   Cmov_sym_overlay(CString name = "");

   virtual void Finalize();

protected:
   // Destructor - use Release instead
   ~Cmov_sym_overlay();

   // *****************************************
   // C_overlay Functions Overrides
public:
   enum eMessageTypes {NONE=0,DROP=1,DRAG=2,KEY=4};
   enum eSymbolFlags  {NO_CHANGE=0,MOVE=1,ROTATE=2,ALL=0xFFFFFFFF};

   // Returns a string identifying the class this object is an instance of.
   virtual const char *get_class_name() { return "Cmov_sym_overlay"; }

   virtual int draw(ActiveMap *map);

   // add menu items to the popup menu based on context
   void menu(ViewMapProj *map, CPoint point, CList<CFVMenuNode *,
      CFVMenuNode *> &list);

   // is the cursor over anything important?
   virtual C_icon *hit_test(map_projection_utils::CMapProjWrapper *map, CPoint point);

   // Allows the overlay manager to pass double-click notices to the route,
   // If the point hits a turn point or additional point with links, then those
   // links will be displayed.
   int on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point);

   // mouse down- so check if we want to do somthing (possibly start drag of viewpoint.
   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);
   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags);

   virtual void set_current_selection(Csym_icon *icon);

   // draw the symbol with the info box around it
   virtual void set_info_box_focus(Csym_icon *icon);
   virtual void release_focus();

   virtual void release_edit_focus(); 

   virtual int pre_close(BOOL* cancel);

   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will only get called for
   // the current route when the route editor is active.
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point);

   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will only get called for
   // the current route when the route editor is active.
   virtual boolean_t offer_keyup(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
      UINT nFlags, CPoint point);

   // move the viewpoint
   virtual void drag(IFvMapView* pMapView, CPoint point, UINT nFlags, HCURSOR *cursor, HintText **hint);
   void drag_symbol(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint point, UINT nFlags, HCURSOR *cursor, HintText **hint);
   void drag_object(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint point, UINT nFlags, HCURSOR *cursor, HintText **hint);

   virtual void cancel_drag(IFvMapView* pMapView);

   // droped the viewpoint
   virtual void drop(IFvMapView *pMapView, CPoint point, UINT flags);
   void drop_symbol(IFvMapView *pMapView, CPoint point, UINT flags);
   void drop_object(IFvMapView *pMapView, CPoint point, UINT flags);

   static enumEditorMode   get_editor_mode() {return m_editor_mode;}
   static void set_editor_mode(enumEditorMode mode) {m_editor_mode = mode;}       

   static BOOL get_snap_to_legs() {return m_snap_to_legs;}
   static void set_snap_to_legs(BOOL enable) {m_snap_to_legs=enable;}

   // Performs an object specific "receive" of a dragged icon from DragSel.
   // The receive_new_point() member of an overlay is called if and only if
   // it is currently the edit-mode overlay.
   virtual int receive_new_point(ViewMapProj* map, CPoint point, UINT flags, 
      C_drag_item* item);

   // This function is called to get the default cursor associated with the
   // current mode.
   static HCURSOR get_default_cursor();

   // CFVFile Overlay overrides
public:

   virtual int file_new();
   virtual int open(const CString & pathname);
   virtual int save_as(const CString & pathname, long nSaveFormat);

   // Inform the overlay that it is about to be saved, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the save is canceled.
   virtual int pre_save(boolean_t *cancel);

   // get default extension - this extension is the same for
   // an entire derived class of CFvOverlayPersistenceImpl objects
   virtual const char *get_default_extension();

public:

   //*************************************************
   // Functions called by com interface for adding / moving symbols
   //************************************************
   // add a symbol the WindowHdl (if not NULL) will get a mesage whenever the symbol is droped
   // return -1 on fail and the SymbolID on success
   static long AddSymbol( long SymbolType, long SymbolFlags=ALL);
   static long RequestNotification(long hWnd,long MessageType = DROP);
   static long MoveSymbol(double Latitude, double Longitude, double Altitude, double Heading);
   static long DeleteSymbol();
   static long AddSymbolWithOwner(long WindowHdl, long SymbolType, long MessageTypes=DROP);



   //****************************************************************
   // Functions called by other overlays for finding / moving symbols
   //****************************************************************
   // used to get a list of all available symbols.
   // Start by passing in 0 as the current symbol ID,  The function will return the first 
   // symbol ID,  Continue calling the function with the last returned value and the function
   // will iterate through all symbols.  Returns 0 when no more symbols are available.

   // move the symbol and notify any attached windows that the icon has moved.
   static long set_symbol_position(double Latitude, double Longitude, double Altitude, double Heading); 

   // Static member functions.
public:

   // functions tell if a viewer is registered in the registry and the name if it is.
   static BOOL is_viewer_registered();
   static CString get_viewer_name();

   // create a new instance of this type of overlay, the erase_background flag
   // should be passed to C_ovl_mgr::invalidate_all() after the new overlay
   // is added to the overlay manager's list, use the cancel flag to indicate
   // that the user canceled the create.  Note: create() returns NULL when
   // cancel = TRUE.
   static short m_file_count;

   // edit mode functions
   static int set_edit_on(boolean_t edit);

   // called by popup menu for get info on a symbol 
   static void get_symbol_info(ViewMapProj *map, C_icon *icon);
   static void snap_enable(ViewMapProj *map, C_icon *icon);
   static void snap_disable(ViewMapProj *map, C_icon *icon);

   // called by popup menu for objects
   static void get_object_info(ViewMapProj *map, C_icon *icon);
   void get_object_info_hlpr(ViewMapProj *map, SkyViewObject *icon);
   static void edit_object_info(ViewMapProj *map, C_icon *icon);
   static void edit_object_links(ViewMapProj *map, C_icon *icon);
   static void delete_object(ViewMapProj *map, C_icon *icon);

   // callback for Link Editor dialog.
   static void save_links(CString handle, CStringArray &links);
   void save_links_hlpr(CString handle, CStringArray& links);

   // toolbat buttons buttons
   static int open_skyview();
   static void OnUpdateSkyview(CCmdUI* pCmdUI);

   static int center_symbols(MapProj* map);
   static void OnUpdateCenterSymbols(CCmdUI* pCmdUI);

   static int skyview_object_dlg(MapProj *map);

   // display symbol info in the info (remark) dialog
   static void show_symbol_info(Csym_icon* sym);

   static void destroy_skyview_interface();

   // End C_overlay Functions Overrides
   // *****************************************

   static void send_drop_to_all_windows(Csym_icon* sym);
protected:

   static void OnConnectionInitialized();
   static void OnConnectionTerminating();

   static POSITION get_symbol_pos(long id);
   static notify_wnd *Cmov_sym_overlay::get_notify_window(HWND wnd);

   // display object info in the info dialog
   void show_object_info(ViewMapProj *map, SkyViewObject *object);

   // send a drag or drop notification to all registered windows.
   static void send_drag_to_all_windows(Csym_icon* sym);

   // returns the route lege (if any) that the point is over.
   COverlayCOM *over_route_leg(CPoint pt, double*lat, double* lon, double* msl, double* heading) const;

   static long set_symbol_position(double Latitude, double Longitude,MapProj *map);

   // create an additional icon for each object file in 
   // HD_DATA/objects/skyview and add it to the list.  The list of
   // CIconImage's will be added to the 3D Object Palette and can
   // be dropped onto the map
   static void create_object_icons();

   // get_block_size - returns the number of bytes required to write this
   // overlay's data to disk
   int get_block_size();

   // serialize this overlays data into the given block
   int Serialize(BYTE *&block_ptr);

   // deserialize the data from the given block into this overlay
   int Deserialize(BYTE *block);

   // reset each of the object handles to -1 in the object list
   void reset_object_handles();
};

#endif
