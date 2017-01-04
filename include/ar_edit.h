// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// ar_edit.h

#ifndef AR_EDIT_H
#define AR_EDIT_H

#include "common.h"
#include "overlay.h"
#include "dafroute.h"
#include "map.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "ovl_d.h"
#include "utils.h"

#define MM_AR_SELECT 1
#define MM_AR_TRACK  2
#define MM_AR_ANCHOR 3
#define MM_AR_NAVAID 4
#define MM_AR_POINT  5

#define SOF_DIST 20

class C_ar_obj;
class C_ar_edit;
class CAdoSet;


// ************************************************************************
// ************************************************************************

class C_ar_point
{
public:
   C_ar_point();
   C_ar_point(C_ar_obj* obj);

   ~C_ar_point();

   CString m_icao;
   CString m_id;
   CString m_direction;
   CString m_seq_num;
   CString m_usage_string;
   degrees_t m_lat;
   degrees_t m_lon;
   CPoint m_pt;
   CRect m_rect;
   CString m_nav_flag;
   CString m_nav_id;
   CString m_nav_type;
   CString m_nav_country;
   CString m_nav_key;
   double m_nav_lat;
   double m_nav_lon;
   double m_nav_mag_var;
   double m_nav_slave_var;
   CString m_fix;
   CString m_true_fix;
   CList<CString*, CString*> m_usage;
   CString m_usage_str;
   int m_usage_code;
   BOOL m_bEndPoint;
   BOOL m_bStartPoint;
   BOOL m_selected;
   BOOL m_anchor_point;
   CString m_label;
   double m_mag_var;
   double m_dist; // from origin in meters
   double m_angle; // from origin
   C_ar_obj* m_obj;

   char m_type;
   int m_num;
   // CRect m_rc;

   void add_usage(CString usage);
   BOOL remove_usage(CString usage);
   void get_usage_string(CString & usage);
   BOOL has_usage_string(CString usage, BOOL *unique);
   BOOL has_one_usage(CString & usage);
   BOOL get_first_usage_string(CString & usage);
   BOOL get_next_usage_string(CString & usage);
   void clear_usages();
   void calc_nav_fix(BOOL is_auto_magvar = TRUE, double manual_magvar = 0.0);
   void calc_nav_geo();
   void calc_dist_angle();

   BOOL hit_test(CPoint point);
   void move(SnapToInfo* snap_to_info);
   void set_crossref(SnapToInfo* snap_to_info);

   void copy(C_ar_point *vtx);
   C_ar_point &operator=(C_ar_point &vtx); 
};
// end of C_ar_point

// ************************************************************************
// ************************************************************************

////////  C_ar_obj  //////////////////////////////

class C_ar_obj : public C_icon
{
public:
   CString m_icao;
   CString m_id;
   CString m_direction;
   CString m_country;
   CString m_local_datum;
   CString m_geodetic_datum;
   CString m_prfreq;
   CString m_bufreq;
   CString m_apnset;
   CString m_apxcode;
   CString m_tac_chan;
   CString m_rcvr_chan;
   CString m_tank_chan;
   CString m_rectankchan;
   CString m_altdesc1;
   CString m_fuel1alt1;
   CString m_fuel1alt2;
   CString m_altdesc2;
   CString m_fuel2alt1;
   CString m_fuel2alt2;
   CString m_altdesc3;
   CString m_fuel3alt1;
   CString m_fuel3alt2;
   CString m_unit;
   CString m_atc;
   CString m_cycledate;
   CRect m_rect;
   degrees_t ll_lat, ll_lng, ur_lat, ur_lng;
   d_geo_t m_bnd_ll;
   d_geo_t m_bnd_ur;
   CList<C_ar_point*, C_ar_point*> m_point_list;
   C_ar_point *m_anchor_point;
   CList<CString*, CString*> m_remark_list;
   int m_remark_ptr;
   int m_remark_cnt;
   CString m_remarks;
   CString m_tip_info;
   CString m_label;
   BOOL m_selected;
   BOOL m_created;
   BOOL m_visible;
   C_ar_point *m_selected_vertex;
   C_ar_point *m_start_vertex;
   char m_type;  // TYPE:  A = Anchor, T = Track, B = Anchor or Track


   int m_color;
   CRect m_bounds;
   double m_ll_lat;
   double m_ll_lon;
   double m_ur_lat;
   double m_ur_lon;

   BOOL m_background;

   // defining basic params for anchor
   double m_anchor_length;
   double m_anchor_width;
   double m_lat; // origin
   double m_lon; // origin 
   double m_anchor_pt_lat;  // center of route
   double m_anchor_pt_lon;  // center of route 
   double m_angle;
   BOOL m_anchor_left;

   BOOL m_first_draw;

   double m_ip_dist; // in NM
   double m_exit_dist; // in NM

   // computed params for anchor
   CPoint m_anchor_pt[4];

   CDC *m_dc;

public:
   // Constructor
   C_ar_obj(C_overlay *overlay);

   ~C_ar_obj();

   int draw(ActiveMap* map);
   void draw_xor_anchor(CDC *dc, int x, int y);
   void draw_point(CDC *dc, C_ar_point *pt, BOOL hilighted, BOOL sof);
   void draw_point(CDC *dc, int x, int y, CString usage, BOOL hilighted, BOOL sof);
   void draw_point_handles(CDC *dc, int x, int y);
   void draw_arc(CDC *dc, POINT start, POINT end, POINT center, BOOL clockwise, BOOL xor = FALSE);
   BOOL point_on_arc(CPoint start, CPoint end, CPoint center, BOOL clockwise, CPoint test_pt);
   int draw_racetrack(MapProj* map, CDC *dc, int color_code, BOOL background, BOOL xor = FALSE); 
   int draw_start_end_lines(MapProj* map, CDC *dc); 
   int compute_route(MapProj* map, char type,  // A, T, or B (anchor, track, both)
      CList <C_ar_point*, C_ar_point*> & vtxlist,
      CList <C_dafroute_leg*, C_dafroute_leg*> & leglist,
      CList <C_dafroute_leg*, C_dafroute_leg*> & ptlist);

   void draw_sof_lines(ActiveMap* map, CDC *dc, BOOL selected);
   void draw_point_to_navaid_line(ActiveMap* map, CDC *dc, C_ar_point* pt, int color);
   void compute_anchor_points(ActiveMap* map);
   void make_anchor(MapProj* map);
   CString get_help_text();
   CString get_tool_tip();
   BOOL hit_test(CPoint point);
   BOOL hit_test(CPoint point, C_ar_point **vertex);
   BOOL hit_test(MapProj* map, CPoint testpt, int* index, BOOL *anchor);
   BOOL list_hit_test(CPoint point);
   BOOL racetrack_hit(MapProj* map, CPoint point);
   BOOL sof_line_hit_test(CPoint point);
   const char *get_class_name() { return "C_ar_obj"; }
   void clear_selects(); 
   void clear_points();
   void invalidate();
   int add_point(double lat, double lon);
   BOOL rubber_band_point(MapProj* map, CDC* dc, CPoint oldpt, CPoint newpt, int index);
   void rubber_band_anchor(MapProj* map, CDC* dc, CPoint point, CPoint *anchor_pt); 
   BOOL get_index_before_pt(MapProj* map, CPoint testpt, int *index_before);
   BOOL insert_vertex(int index_before, int x, int y, double lat, double lon); 
   BOOL insert_vertex(MapProj* map, CPoint newpt); 
   BOOL insert_vertex(MapProj* map, CPoint newpt, int* new_vertex_num); 
   BOOL insert_vertex_on_line(MapProj* map, CPoint newpt, int* new_vertex_num); 
   BOOL compute_bounds();
   BOOL point_on_line (int x1, int y1,  /* pt on the line */
      int x2, int y2,   /* pt on the line */
      int xp, int yp,   /* pt to project */
      int* xn, int* yn); // projected point
   BOOL select_point(int index, BOOL selected);
   void delete_route_point(C_ar_point *pt);
   void delete_selected();
   BOOL move_vertex(int index, double lat, double lon);
   C_ar_point* get_vertex(int index);
   CIconImage* get_icon(CString code);
   int find_anchor_point(); 
   // void find_anchor_point(MapProj* map, CPoint point, int *anchor_num, BOOL *left); 
   void set_anchor_point(MapProj* map, CPoint point); 
   // void rubber_band_anchor(MapProj* map, CDC* dc, CPoint point1, CPoint point2, BOOL left); 
   C_ar_obj &operator=(C_ar_obj &);

};
// end of definition of C_ar_obj

// ************************************************************************
// ************************************************************************

class C_ar_edit : 
   public CFvOverlayPersistenceImpl
{
public:
   CIconImage *ip_icon, *cp_icon, *nc_icon, *ex_icon;
   CIconImage *et_icon, *an_icon, *ap_icon, *sel_icon;
   BOOL m_point_selected;
   C_ar_obj* m_selected_point;
   C_ar_obj* m_anchor_dragger;
   int m_anchor_vtx;
   double m_anchor_lat;
   double m_anchor_lon;

   // private variables
   //private:
   color_index_t m_color;
   CRect m_rect;
   char OvlFileName[100];
   degrees_t ll_lat, ll_lng, ur_lat, ur_lng;
   // CDafifUtil* m_pUtil;
   CList<C_ar_obj*, C_ar_obj*> m_obj_list;
   CList<C_ar_point*, C_ar_point*> m_refuel_vertex_hit_list;
   BOOL m_bLabel;
   char m_data[3];
   MapScale m_smallest_labels_scale;
   static MapScale m_smallest_scale;
   boolean_t m_map_displayed;
   BOOL m_background;
   CString m_ovlname;
   CString m_filename;

   int navcnt, navptr;
   int navmax;
   int portcnt, portptr;
   int portmax;
   int dafdisplayed;
   int arcnt;
   int mtrcnt;
   int m_cnt;
   BOOL firstMtrList;
   BOOL m_display_points;
   BOOL m_display_labels;
   BOOL m_display_names;
   BOOL m_display_all;
   BOOL m_display_all_routes;
   BOOL m_drag_lock;
   BOOL m_drag_lock_checked;
   BOOL m_screen_moved;
   CPoint m_fuel_point_selected_pt;
   BOOL m_fuel_point_selected;
   CString m_fuel_select_id;
   CPoint m_fuel_mouse_point;
   BOOL m_fuel_selected_hit_test;
   degrees_t m_fuel_point_selected_lat;
   degrees_t m_fuel_point_selected_lon;
   CString m_fuel_point_selected_usage;
   BOOL m_new_mode;
   BOOL m_screen_moved_mode;
   BOOL m_first_pt;
   CPoint m_cur_pt;
   double m_cur_lat;
   double m_cur_lon;
   CPoint m_old_pt;
   BOOL m_bDragging;
   BOOL m_first_drag;
   HintText m_hint;
   BOOL m_bGotHandle;
   int m_handle;
   CPoint m_offset_pt;
   CPoint m_start_pt;
   CPoint m_end_pt;
   double m_start_lat;
   double m_start_lon;
   double m_end_lat;
   double m_end_lon;
   double m_old_lat;
   double m_old_lon;
   double m_orig_lat;
   double m_orig_lon;
   double m_start_angle;
   double m_cur_angle;
   BOOL m_drag_all;
   BOOL m_originate;
   d_geo_t m_ll;
   d_geo_t m_ur;
   int m_nCurColor;
   C_ar_point *m_cur_vertex;
   C_ar_obj *m_old_route;
   CPoint m_hint_pt;
   CPoint m_hint_pt2;
   int m_vertex_num;
   int m_navaid_vertex_num;
   BOOL m_navaid_vertex_anchor;

   BOOL m_display_sof_points;
   int m_sof_display_radius;

   static int m_cur_vertex_num;
   static C_ar_point *m_cur_point;
   static C_ar_point *m_menu_point;
   static C_ar_obj *m_cur_obj;
   static C_ar_obj *m_selected_obj;
   static C_ar_edit *m_cur_ovl;
   static BOOL m_bGotPt;
   static BOOL m_bEdit;
   static BOOL m_in_poly_mode;
   static BOOL m_bTempSelect;
   static int m_nMode;
   static int m_ovl_cnt;
   static CString m_directory;
   static int m_nTempMode;
   static C_ar_obj* m_pTempObj;
   static CPoint m_temp_start_pt;
   static CPoint m_anchor_pt;
   static double m_cursor_lat;
   static double m_cursor_lon;
   static BOOL m_start_drag;
   static int m_old_cnt;
   static double m_point_lat;
   static double m_point_lon;
   static BOOL m_anchor_ext_pt;
   static C_ar_point * m_anchor_ext_point;
   static CIconImage* m_draw_icon;


   // Constructor
public:                         
   explicit C_ar_edit(CString name = "");

   virtual void Finalize();

   CList<CString*, CString*> m_display_list;

   int clear_ovl();

   int draw(ActiveMap* map);
   int open(ActiveMap* map);
   virtual int save_as(const CString & pathname, long nSaveFormat);

   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point);

   int selected(IFvMapView* pMapView, CPoint point, UINT flags);
   void clear_vertex_hit_list();
   int show_info(C_icon* icon);
   void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list);
   static void point_info(ViewMapProj *map, C_icon *pt);
   static void add_point(ViewMapProj *map, C_icon *pt);
   static void edit_route_info(ViewMapProj *map, C_icon *pt);
   static void edit_anchor_params(ViewMapProj *map, C_icon *pt);
   static void edit_point_info(ViewMapProj *map, C_icon *pt);
   static void edit_point_info_dlg(ViewMapProj *map, C_ar_obj *obj, C_ar_point *ar_pt);
   static void delete_point(ViewMapProj *map, C_icon *pt);
   static void delete_route(ViewMapProj *map, C_icon *pt);
   static BOOL refuel_usage_to_string(CString usage, char * txt, int txt_len);
   static BOOL refuel_string_to_usage(CString txt, CString & usage);
   static void end_poly_line();
   static HCURSOR get_default_cursor();

   int get_route_geo(CString route_id, double *lat, double *lon);
   void release_focus();
   C_ar_obj *line_hit_test(MapProj* map, CPoint point);
   void set_temp_select(BOOL select, HCURSOR* cursor);
   void end_draw();

   void remove_all();
   BOOL remove_route(C_ar_obj* rte);

   static BOOL get_refuel_list(CString type, CList<CString*, CString*> & list);
   void rubber_band_anchor(MapProj* map, CDC* dc, CPoint point1, CPoint point2);

   // determine if the point hits the object's selection region(s)

   C_icon *hit_test(MapProj* map, CPoint point);
   C_icon *icon_hit_test(CPoint point);
   const char *get_class_name() { return "C_ar_edit"; }
   void invalidate_and_clear_selects(); 

   virtual int file_new();
   virtual int open(const CString & filename);

   MapScale get_smallest_scale();
   void set_smallest_scale(MapScale scale); 
   MapScale get_smallest_labels_scale();
   void set_smallest_labels_scale(MapScale scale); 
   void clear_selects(); 
   BOOL plot_option(char ch);
   int add_route(ActiveMap* map, CString & route_id);
   int display_all(ActiveMap* map);
   int display_all_routes(ActiveMap* map);
   BOOL find_id_in_leglist(CString id,
      CList <C_dafroute_leg*, C_dafroute_leg*> & leglist);
   boolean_t test_snap_to(ViewMapProj* map, CPoint point);
   boolean_t do_snap_to(ViewMapProj* map, CPoint point, 
      CList<SnapToInfo *, SnapToInfo *> &snap_to_list);

   // database functions
   BOOL create_db(CAdoSet **set, CString filepath, long nSaveFormat);
   int load(const CString & filename);
   BOOL update(CAdoSet *set);
   BOOL update_point(CAdoSet *set);
   BOOL update_remark(CAdoSet *set);
   int save_to_file(const CString & filename, long nSaveFormat);
   BOOL save_route(CAdoSet *set, C_ar_obj *obj);
   BOOL save_point(CAdoSet *set, C_ar_obj *obj, C_ar_point *pt);
   BOOL save_route_points(CAdoSet *set, C_ar_obj *obj);
   BOOL save_remarks(CAdoSet *set, C_ar_obj *obj);
   void delete_selected();
   void get_date(CString & date);
   int sort_points(CList<C_ar_point*, C_ar_point*> & point_list, BOOL really_sort);
   BOOL ar_point_usage_to_code(CString usage, int *code);

   static int set_edit_on(boolean_t edit);
   void set_line_hint(ViewMapProj* map, CPoint start_pt, CPoint cur_pt, HintText**hint);
   void set_line_hint(ViewMapProj* map, CPoint start_pt, CPoint cur_pt, 
      CPoint end_pt, HintText**hint);

   static void normalize_id(CString id, CString direction, CString &norm_id);

   static void test_all_refuel(); 
   static BOOL route_exists(CString & route_id);
   static void C_ar_edit::set_editor_mode(int mode);
   static int C_ar_edit::get_editor_mode();
   static BOOL valid_overlay(C_ar_edit *ovl);
   static C_ar_edit* get_current_overlay();
   static void toggle_property_dialog();

   BOOL C_ar_edit::valid_object(C_ar_obj *obj);
   CString get_new_segnum();

   // Returns FALSE to indicate the read-only drawing file can not be read.
   // excuse will contain the reason why this is so, and help_id will contain
   // the context sensitive help id.   
   static boolean_t open_read_only_check(const char *filespec, CString &excuse,
      UINT &help_id)
   {
      excuse = "they are MS Access Database files";
      help_id = 29465;
      return FALSE;
   }

   static const char *filename_template() 
   { return "%s\\ar%d.uar"; }
   static const char *get_default_extension_static()
   { return "uar"; }

   virtual const char *get_default_extension() 
   { return get_default_extension_static(); }

   int pre_close(BOOL* cancel);

   // file in ar_mouse.cpp
   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);
   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags);
   virtual int on_right_mouse_button_down(IFvMapView *pMapView, CPoint point, UINT nFlags);
   BOOL override_drag_lock(CString operation, CString object_name);
   int select_select(C_ar_obj *obj, ViewMapProj* map, UINT nFlags, CPoint point, boolean_t *drag);
   BOOL select_anchor_point(C_ar_obj* rte, MapProj* map, CDC* dc, UINT nFlags, CPoint point);
   BOOL select_track_point(C_ar_obj* rte, MapProj* map, CDC* dc, UINT nFlags, CPoint point);
   static BOOL select_point(const CList<CString*, CString*> & strlist, CString & key, CString & usage);
   void drag(ViewMapProj* map, CPoint point, UINT nFlags, 
      HCURSOR *cursor, HintText **hint);
   void drag_select(ViewMapProj* map, UINT nFlags, CPoint point, 
      HCURSOR* cursor, HintText **hint);
   BOOL drag_point(C_ar_obj* obj, ViewMapProj* map, CDC* dc, 
      UINT nFlags, CPoint point, HCURSOR *cursor, HintText **hint);
   void drop(ViewMapProj* map, CPoint point, UINT nFlags);
   void drop_select(ViewMapProj* map, UINT nFlags, CPoint point);
   BOOL drop_line(C_ar_obj* obj, ViewMapProj* map, CDC* dc, UINT nFlags, CPoint point);
   CString get_new_name();

   static CString get_navaid_type(CString usage_str);

   int begin_drag(MapProj* map, CDC *dc, C_ar_obj *obj, CPoint point);
   int move_drag(MapProj* map, CDC *dc, CPoint oldpt, CPoint newpt, HintText**hint);
   int end_drag(MapProj* map, CDC *dc, CPoint oldpt, CPoint newpt);

   int begin_drag_all(MapProj* map, CDC *dc, C_ar_obj *obj, CPoint point);
   int move_drag_all(MapProj* map, CDC *dc, CPoint oldpt, CPoint newpt, HintText**hint);
   int end_drag_all(MapProj* map, CDC *dc, CPoint oldpt, CPoint newpt);

   C_ar_obj* get_selected();
   int enable_property_dialog_button();

   void cancel_drag(ViewMapProj* map);

protected:
   // Protected destructor. Call Release instead
   ~C_ar_edit();

   // private member functions
private:
   void simple_geo_circle(CDC *dc, // "prepared" DC
      double lat, double lon,  // center
      double radius);  // in km

}; // end of C_ar_edit class declaration 

// ************************************************************************
// ************************************************************************

#endif  // #ifndef AR_EDIT_H