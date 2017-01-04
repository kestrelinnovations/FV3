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

// TacModel.h

#ifndef TACTICAL_MODEL_H
#define TACTICAL_MODEL_H

#include "common.h"
#include "overlay.h"
#include "map.h"
#include "maps_d.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "ovl_d.h"
#include "utils.h"
#include "..\TacticalModel\cdb.h"
#include "..\TacticalModel\ttvs.h"

#define MODEL_TVS 1
#define MODEL_CDB 2

#define MODEL_BACK_DISPLAY_NONE     0
#define MODEL_BACK_DISPLAY_IMAGERY  1
#define MODEL_BACK_DISPLAY_ELEV     2


//forward 
class MapProj;
class ActiveMap;
class C_model_ovl;

class C_model_ovl : 
   public CFvOverlayPersistenceImpl
{
public:
   CIconImage  *m_model_icon;
   double m_center_lat;
   double m_center_lon;
   d_geo_t m_ll_bound, m_ur_bound;
   int m_model_type;
   int m_cur_lod;

// private variables
//private:
   CRect m_rect;
   bool m_bLabel;
   char m_data[3];
   MapScale m_smallest_labels_scale;
   static MapScale m_smallest_scale;
   boolean_t m_map_displayed;
   BOOL m_background;
   BOOL m_model_selected;
   CString m_filename;
   CString m_model_filename;
   char OvlFileName[300];

   CList<C_model_item*, C_model_item*>  m_item_list;

//   HintText* m_hint;
   int m_vertex_num;
   static C_model_ovl *m_cur_ovl;

//   static C_model_obj *m_cur_obj;
   C_model_obj *m_cur_obj;

   enum EditorMode {SELECT, ADD}; //get and set the editor mode (e.g., "select" or "add")

private:
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

   static C_model_obj *m_selected_obj;
   static BOOL m_bGotPt;
   static int m_nMode;

// Constructor
public:                         
   explicit C_model_ovl(CString name = "");

   virtual HRESULT Initialize(GUID overlayDescGuid);

   CList<CString*, CString*> m_display_list;

   int clear_ovl();

   int draw(ActiveMap* map);
   int open(ActiveMap* map);
   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point);

   // cannot save Model files
   virtual int save_as(const CString & pathname, long nSaveFormat) { return FAILURE; }
  
   int selected(IFvMapView* pMapView, CPoint point, UINT flags);
   void clear_vertex_hit_list();
   int show_info(C_icon* icon);
   void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list);
   static HCURSOR get_default_cursor();
   static int set_edit_on(boolean_t edit);
   void set_editor_mode(int mode);
   int set_background_type(int back_type);
   int set_elev_exag_factor(float exag);
   static int get_editor_mode();

   void release_focus();

   void remove_all();


   // determine if the point hits the object's selection region(s)

   C_model_obj *hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point);
   const char *get_class_name() { return "C_model_ovl"; }
   void invalidate_and_clear_selects(); 

   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);

   virtual int open(const CString & filename);

   MapScale get_smallest_scale();
   void set_smallest_scale(MapScale scale); 
   MapScale get_smallest_labels_scale();
   void set_smallest_labels_scale(MapScale scale); 
   void clear_selects(); 
   boolean_t test_snap_to(ViewMapProj* map, CPoint point);
   boolean_t do_snap_to(ViewMapProj* map, CPoint point, 
   CList<SnapToInfo *, SnapToInfo *> &snap_to_list);

public:
   static BOOL is_valid_ovl_ptr(C_overlay *testovl); 
   int get_center_geo(double *lat, double *lon);

   int OnTestSelected(map_projection_utils::CMapProjWrapper* map, CPoint point, HCURSOR *cursor, HintText **hint);
   int OnSelected(C_overlay* pOverlay, map_projection_utils::CMapProjWrapper* map, CPoint point, boolean_t *drag, HCURSOR *cursor, HintText **hint);

   int get_min_max_lods(int *minlod, int *maxlod);
   int draw_icon(ActiveMap* map);
   int draw_bounds(ActiveMap* map);
   int draw_model(ActiveMap* map);

   int GetBlockElevationMeters(double ll_lat, double ll_lon,
      double ur_lat, double ur_lon, short number_NS_samples,
      short number_EW_samples, float* elev);

   int get_model_type(CString & type_str, int *type);
   void clear_item_list();
   static CString extract_filename( const CString& csFilespec );

protected:
   // Protected destructor. Call Release instead
   ~C_model_ovl();

public:

   std::string GetOverlayGuid() { return m_strOverlayGuid; }   

   virtual void Finalize();

protected:

   CList<CString, CString> m_listObjectMenuItems;

private:

   std::string m_strOverlayGuid;

}; // end of C_model_ovl class declaration 

#endif  // #ifndef TACTICAL_MODEL_H