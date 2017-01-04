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



// TacModel.cpp  -- member functions of C_model_ovl


#include "stdafx.h"
#include "param.h"
#include "showrmk.h"
#include "geo_tool.h"
#include "FvCore\Include\RichTextFormatter.h"
#include "..\StatusBarManager.h"
//#include "fvwutil.h"
#include "maps.h"       // for MAP_get_scale_from_string
#include "factory.h"
#include "..\getobjpr.h"
#include "SnapTo.h"
#include "ovl_mgr.h"
#include "err.h"
#include "TacModel.h"
#include "factory.h"


MapScale C_model_ovl::m_smallest_scale;

int C_model_ovl::m_nMode;
int C_model_ovl::m_bEdit;

C_model_ovl::C_model_ovl(CString name /* = "" */) : CFvOverlayPersistenceImpl(name), m_bDragging(FALSE)
{
   // the block of code above was replaced by this in order to eliminate the use of PRM_get_value
   CString reg_string = PRM_get_registry_string("Main", PRM_HD_DATA);
   strncpy_s(OvlFileName, 100, reg_string, (sizeof(OvlFileName)-1));
   strcat_s(OvlFileName, 100, "\\ovlmgr\\");

   CString dir = "ovlmgr\\";
   m_model_icon      = CIconImage::load_images(dir + "TacticalModel.ico");

   m_model_selected = FALSE;
   m_cur_obj = NULL;
   m_model_type = 0;
   m_cur_lod = 0;

   // Generate a GUID for this overlay
   //
   GUID guid;
   ::CoCreateGuid(&guid);

   wchar_t buf[45];
   StringFromGUID2(guid, buf, 45);

   m_strOverlayGuid = (char *)_bstr_t(buf);

   m_ll_bound.lat = m_ll_bound.lon = 0.0;
   m_ur_bound.lat = m_ur_bound.lon = 0.0;
}
// end of constructor of C_model_ovl

C_model_ovl::~C_model_ovl()
{
   // Most, if not all, destruction should be accomplished in Finalize.
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

void C_model_ovl::Finalize()
{
   clear_ovl();

   if (m_model_selected)
      CRemarkDisplay::close_dlg();
}

int C_model_ovl::set_background_type(int back_type)
{
   if (m_cur_obj == NULL)
      return FAILURE;

   if (m_model_type == MODEL_CDB)
   {
       CCdb *cdb = (CCdb*) m_cur_obj;
       cdb->m_back_display_code = back_type;
       return SUCCESS;
   }

   return FAILURE;
}

int C_model_ovl::set_elev_exag_factor(float exag)
{
   if (m_cur_obj == NULL)
      return FAILURE;

   if (m_model_type == MODEL_CDB)
   {
       CCdb *cdb = (CCdb*) m_cur_obj;
       cdb->m_elev_exag = exag;
       return SUCCESS;
   }

   return FAILURE;
}

C_model_obj *C_model_ovl::hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point)
{
   CString mstr;
   CTtvs *ttvs;
   CCdb *cdb;

   if (m_cur_obj == NULL)
      return NULL;

   switch(m_model_type)
   {
      case MODEL_TVS:
         ttvs = (CTtvs*) m_cur_obj;
         if (ttvs->hit_test(point))
            return m_cur_obj;
      case MODEL_CDB:
         cdb = (CCdb*) m_cur_obj;
         if (cdb->hit_test(point))
            return m_cur_obj;
   }

   return NULL;
}
// end of hit_test

int C_model_ovl::show_info(C_icon* icon)
{
   return SUCCESS;
}
// end of show_info

int C_model_ovl::draw(ActiveMap* map)
{
   int rslt;

   rslt = draw_bounds(map);

   set_valid(TRUE);
   return SUCCESS;
}
// end of draw


void C_model_ovl::clear_item_list()
{
   C_model_item *item;

   while (!m_item_list.IsEmpty())
   {
      item = m_item_list.RemoveHead();
      delete item;
   }
}

#define M_LOG2E 1.44269504088896340736 //log2(e)

static double log2(const double x)
{
    return  log(x) * M_LOG2E;
}

int C_model_ovl::draw_model(ActiveMap* map)
{
   CList<model_item_t*, model_item_t*>  model_list;
   char buf[20];
   CTtvs *ttvs;
   CCdb *cdb;
   model_item_t *item;
   C_model_item *tac_item;
   d_geo_t scrn_ll, scrn_ur;
   POSITION next;
   int rslt;
   CString error_msg;
   int lod = 0;

   if (map == NULL)
      return FAILURE;

   if (m_cur_obj == NULL)
      return FAILURE;

   // get the map bounds to clip symbol drawing 
   if (map->get_vmap_bounds(&scrn_ll, &scrn_ur) != SUCCESS)
      return FAILURE;

   // find the LOD (at least for 2D)
   double deg_per_pix = map->vmap_degrees_per_pixel_lat();
   if (deg_per_pix == 0.0)
      return FAILURE;

   double tf = 0.001 / deg_per_pix;

   lod = (int) log2(tf);

   m_cur_lod = lod;

   strncpy_s(buf, m_cur_obj->get_class_name(), 9);

   if (!strncmp(buf, "CTtvs", 5))
   {
      ttvs = (CTtvs*) m_cur_obj;
      rslt = ttvs->get_models_from_database(lod, scrn_ll.lat, scrn_ll.lon, scrn_ur.lat, scrn_ur.lon, 
                                     model_list, error_msg);

   }
   else if (!strncmp(buf, "CCdb", 4))
   {
      cdb = (CCdb*) m_cur_obj;
      cdb->draw_model(map, lod);
      set_valid(TRUE);
      return SUCCESS;
   }
   else
   {
      ERR_report("Invalid Model type.");
      return FAILURE;
   }

   clear_item_list();

   if (rslt != SUCCESS)
      return FAILURE;

   next = model_list.GetHeadPosition();
   while (next != NULL)
   {
      item = model_list.GetNext(next);
      tac_item = new C_cdb_item;
      tac_item->m_angle = item->angle;
      tac_item->m_lat = item->lat;
      tac_item->m_lon = item->lon;
      tac_item->m_path = item->path;
      tac_item->m_filename = item->filename;
      m_item_list.AddTail(tac_item);
   }

    set_valid(TRUE);
   return SUCCESS;
}
// end of draw_model

int C_model_ovl::draw_icon(ActiveMap* map)
{
   double tlat, tlon;
   CDC *dc;
   int icon_adjusted_size = -1;
   int tx, ty;

   get_center_geo(&tlat, &tlon);

   dc = map->get_CDC();
   map->geo_to_surface(tlat, tlon, &tx, &ty);

   boolean_t is_printing = dc->IsPrinting();
   if (is_printing)
   {
      double adjust_percentage = (double)PRM_get_registry_int("Printing", 
         "IconAdjustSizePercentage", 0);
      icon_adjusted_size = (int)(32.0 + 32.0*adjust_percentage/100.0);
   }

   m_model_icon->draw_icon( dc, tx, ty, 32, icon_adjusted_size );

   set_valid(TRUE);
   return SUCCESS;
}

int C_model_ovl::draw_bounds(ActiveMap* map)
{
   CCdb *cdb;
   int lod;
    CString lodstr;

   if (map == NULL)
      return FAILURE;

   if (m_cur_obj == NULL)
      return FAILURE;

   // find the LOD (at least for 2D)
   double deg_per_pix = map->vmap_degrees_per_pixel_lat();
   if (deg_per_pix == 0.0)
      return FAILURE;

   double tf = 0.001 / deg_per_pix;

   lod = (int) log2(tf);

   switch(m_model_type)
   {
      case MODEL_TVS:
//         ttvs->draw_model(map);
         break;

      case MODEL_CDB:
         cdb = (CCdb*) m_cur_obj;
   // for development
         lodstr = PRM_get_registry_string("TacModel", "TacModelLOD", "L00");
         lodstr = lodstr.Right(2);
         lod = atoi(lodstr);

         cdb->draw_model(map, lod);
         break;

      default:
         ERR_report("Invalid Model type.");
         return FAILURE;
   }
   return SUCCESS;
}

int C_model_ovl::GetBlockElevationMeters(double ll_lat, double ll_lon,
   double ur_lat, double ur_lon, short number_NS_samples,
   short number_EW_samples, float* elev)
{
   ASSERT(m_model_type == MODEL_CDB);  // only type currently supported

   CCdb* cdb = static_cast<CCdb*>(m_cur_obj);
   return cdb->GetBlockElevationMeters(ll_lat, ll_lon,  ur_lat, ur_lon,
      number_NS_samples, number_EW_samples, elev);
}

int C_model_ovl::get_model_type(CString & type_str, int *type)
{
   switch (m_model_type)
   {
      case MODEL_TVS:
         type_str = "TVS";
         *type = MODEL_TVS;
         break;

      case MODEL_CDB:
         type_str = "CDB";
         *type = MODEL_CDB;
         break;

      default:
         type_str = "Invalid";
         *type = 0;
         return FAILURE;
   }

   return SUCCESS;
}
// end of get_model_type

// *****************************************************************
// *****************************************************************

int C_model_ovl::open(ActiveMap* map)
{

   return SUCCESS;
}
// end of open

// *****************************************************************
// *****************************************************************

int C_model_ovl::open(const CString& filename)
{

   CWaitCursor waitCursor;
// CTtvs *ttvs;
   CCdb *cdb;
   CString ext;
   int rslt;

   m_fileSpecification = filename;
   m_filename = filename;

   ext = filename.Right(3);
   ext.MakeUpper();

   if (!ext.Compare("TPS"))
   {
      AfxMessageBox("TTVS is temporarily disabled");
      return FAILURE;
/*
      ttvs  = new CTtvs;
      rslt = ttvs->load(filename);
      if (rslt != SUCCESS)
      return FAILURE;
      m_cur_obj = (C_model_obj*) ttvs;
      m_model_type = MODEL_TVS;
*/
   }
   else
   {
      cdb  = new CCdb;
      rslt = cdb->load(filename);
      if (rslt != SUCCESS)
         return FAILURE;
      m_cur_obj = (C_model_obj*) cdb;
      m_model_type = MODEL_CDB;
      m_ll_bound.lat = cdb->m_bnd_ll.lat;
      m_ll_bound.lon = cdb->m_bnd_ll.lon;
      m_ur_bound.lat = cdb->m_bnd_ur.lat;
      m_ur_bound.lon = cdb->m_bnd_ur.lon;
   }

   // apply filter and label properties to the model objects (only once until they are changed in overlay options)
//   OnSettingsChanged();

   set_valid(TRUE);

   return SUCCESS;
}
// end of open


// *****************************************************************
// *****************************************************************

int C_model_ovl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   boolean_t drag;
   HCURSOR cursor;
   HintText *hint = NULL;

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   if (OnSelected(this, &mapProjWrapper, point, &drag, &cursor, &hint) == FAILURE)
   {
      release_focus();
      return FAILURE;
   }

   return SUCCESS;
}

// *****************************************************************
// *****************************************************************

int C_model_ovl::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   HCURSOR hCursor = NULL;
   HintText *hint = NULL;
   int ret = OnTestSelected(&mapProjWrapper, point, &hCursor, &hint);
   if (hCursor != NULL)
      pMapView->SetCursor(hCursor);
   if (ret == SUCCESS && hint != NULL)
   {
      pMapView->SetTooltipText(_bstr_t(hint->get_tool_tip()));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hint->get_help_text()));
   }

   return ret;
}

// *****************************************************************
// *****************************************************************

int C_model_ovl::OnTestSelected(map_projection_utils::CMapProjWrapper* map, CPoint point, HCURSOR *cursor, HintText **hint)
{
   C_model_obj *obj = hit_test(map, point);

   if (obj != NULL)
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

      m_hint.set_tool_tip(obj->m_tool_tip);
      m_hint.set_help_text(obj->m_tool_tip);
      *hint = &m_hint;

      // return that we handled the mouse move
      return SUCCESS;
   }

   // pass through to the next overlay
   return FAILURE;
}

// *****************************************************************
// *****************************************************************

int C_model_ovl::OnSelected(C_overlay* pOverlay, map_projection_utils::CMapProjWrapper* map, CPoint point, boolean_t *drag, HCURSOR *cursor, HintText **hint)
{
   *drag = FALSE;
   *cursor = NULL;
   *hint = NULL;

   return SUCCESS;
}
// end of selected

// *****************************************************************
// *****************************************************************

// Receives all keydown messeges from the Overlay Manager and filters them 
// for this overlay... a return of false will allow the keystroke to be 
// processed normally in MapView. This function will get called only if 
// the File Overlay is current and open in its editor.
boolean_t C_model_ovl::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point)
{
   return FALSE;
}

// *****************************************************************
// *****************************************************************

void C_model_ovl::invalidate_and_clear_selects() 
{ 

}
// end of invalidate_and_clear_selects

// *****************************************************************
// *****************************************************************

int C_model_ovl::clear_ovl()
{
   // remove all points from trail
   remove_all();

   return SUCCESS;
}
// end of clear_all

// *****************************************************************
// *****************************************************************

void C_model_ovl::remove_all()
{
   int mtype, rslt;
   CString mstr;
   CTtvs *ttvs;
   CCdb *cdb;

   if (m_cur_obj == NULL)
      return;

   rslt = get_model_type(mstr, &mtype);
   if (rslt != SUCCESS)
      return;

   switch(mtype)
   {
      case MODEL_TVS:
         ttvs = (CTtvs*) m_cur_obj;
         delete ttvs;
         break;
      case MODEL_CDB:
         cdb = (CCdb*) m_cur_obj;
         delete cdb;
         break;
   }

   m_cur_obj =  NULL;
}
// end of remove_all

// *****************************************************************

boolean_t C_model_ovl::test_snap_to(ViewMapProj* map, CPoint point)
{

   BOOL hit = FALSE;

   return hit;
}
// end of test_snap_to

// *****************************************************************
// *****************************************************************

boolean_t C_model_ovl::do_snap_to(ViewMapProj* map, CPoint point, 
      CList<SnapToInfo *, SnapToInfo *> &snap_to_list)
{
// CFvwUtil *futil = CFvwUtil::get_instance();
// SnapToInfo *info;
   return false;
}
// end of do_snap_to

// *****************************************************************
// *****************************************************************

// add commands to menu
void C_model_ovl::menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list)
{
}
// end of menu

// *****************************************************************
// *****************************************************************

MapScale C_model_ovl::get_smallest_scale()
{ 
   CString  display_above = PRM_get_registry_string("TacModel", 
      "TacModelDisplayAbove", "1:5 M");
   set_smallest_scale(MAP_get_scale_from_string(display_above));
   return m_smallest_scale; 
}


// *****************************************************************
// *****************************************************************

void C_model_ovl::set_smallest_scale(MapScale scale) 
{ 
   m_smallest_scale = scale; 
}

// *****************************************************************
// *****************************************************************

MapScale C_model_ovl::get_smallest_labels_scale()
{ 
   CString display_above = PRM_get_registry_string("TacModel", 
      "TacModelLabelThreshold", "1:250 K");

   set_smallest_labels_scale(MAP_get_scale_from_string(display_above));
   return m_smallest_labels_scale; 
}
// end of get_smallest_labels_scale

// *****************************************************************
// *****************************************************************

void C_model_ovl::set_smallest_labels_scale(MapScale scale) 
{ 
   m_smallest_labels_scale = scale; 
}

// *****************************************************************
// *****************************************************************

void C_model_ovl::release_focus()
{
   invalidate_and_clear_selects();
}
// end of release_focus

// *****************************************************************
// *****************************************************************

BOOL C_model_ovl::is_valid_ovl_ptr(C_overlay *testovl) 
{
   C_overlay *ovl;

   ovl = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TacticalModel);
   if (ovl == NULL)
      return FALSE;

   if (ovl == testovl)
      return TRUE;

   while (ovl != NULL)
   {
      ovl = OVL_get_overlay_manager()->get_next_of_type(ovl, FVWID_Overlay_TacticalModel);
      if (ovl != NULL) 
         if (ovl == testovl)
            return TRUE;
   }

   return FALSE;
}

// end of is_valid_ovl_ptr

// **************************************************************************
// **************************************************************************

int C_model_ovl::get_center_geo(double *lat, double *lon)
{
   int mtype, rslt;
   CString mstr;
   CTtvs *ttvs;
   CCdb *cdb;

   if (m_cur_obj == NULL)
      return NULL;

   rslt = get_model_type(mstr, &mtype);
   if (rslt != SUCCESS)
      return FAILURE;

   switch(mtype)
   {
      case MODEL_TVS:
         ttvs = (CTtvs*) m_cur_obj;
         rslt = ttvs->get_center_geo(lat, lon);
         return rslt;
      case MODEL_CDB:
         cdb = (CCdb*) m_cur_obj;
         rslt = cdb->get_center_geo(lat, lon);
         return rslt;
   }

   return FAILURE;
}

CString C_model_ovl::extract_filename( const CString& csFilespec )
{
   INT iPos;
   iPos = csFilespec.ReverseFind('\\');
   return ( iPos >= 0 ) ? csFilespec.Mid( iPos + 1 ) : _T("");
}

HRESULT C_model_ovl::Initialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;
   return S_OK;
}

int C_model_ovl::set_edit_on(boolean_t edit)
{
   if (m_bEdit == edit)
      return SUCCESS;

   // remove the remark dialog
   CRemarkDisplay::close_dlg();

   m_nMode = ID_MODEL_SELECT;

   m_bEdit = edit;
   if (m_bEdit)
   {
         get_default_cursor();
   }
   else
   {
   }

   C_model_ovl *model_ovl = (C_model_ovl *)OVL_get_overlay_manager()->get_current_overlay();
   if (model_ovl != NULL)
      if (model_ovl->get_m_overlayDescGuid() == FVWID_Overlay_TacticalModel)
         model_ovl->invalidate_and_clear_selects();

   return SUCCESS;
}
// end of set_edit_on

// ****************************************************************
// ****************************************************************

void C_model_ovl::set_editor_mode(int mode)
{
   int oldmode = m_nMode;
   C_model_ovl *model_ovl;

   model_ovl = (C_model_ovl *)OVL_get_overlay_manager()->get_current_overlay();
   if (model_ovl == NULL)
      return;

   // warn user that they can't draw things at this map scale
   if (!model_ovl->m_map_displayed && (mode != ID_MODEL_SELECT))
   {
      AfxMessageBox("Tactical Model edit tools are not supported at scales greater than\n"
                    "the current 'Hide overlay above' scale setting for Tactical Model.\n\n"
                    "(See the Options command on the Overlay menu to change\n"
                    "this value.)");

      OVL_get_overlay_manager()->overlay_options_dialog(FVWID_Overlay_TacticalModel);

      m_nMode = ID_MODEL_SELECT;
      return;
   }

   m_nMode = mode;  

   if (mode == ID_MODEL_PLACE)
   {
      model_ovl = (C_model_ovl *)OVL_get_overlay_manager()->get_current_overlay();
      if (model_ovl != NULL)
         model_ovl->invalidate_and_clear_selects();
   }
}

// ****************************************************************
// ****************************************************************

int C_model_ovl::get_editor_mode()
{
   return m_nMode;  
}

// ****************************************************************
// ****************************************************************

HCURSOR C_model_ovl::get_default_cursor()
{
   HCURSOR cursor;

   cursor = NULL;
   switch(m_nMode)
   {
      case ID_MODEL_SELECT:
         cursor = AfxGetApp()->LoadCursor(IDC_SELECT_CURSOR);
         break;
      case ID_MODEL_PLACE:
         cursor = AfxGetApp()->LoadCursor(IDC_MODEL_PLACE_CURSOR);
         break;
   }

   return cursor;
}
// end of get_default_cursor
