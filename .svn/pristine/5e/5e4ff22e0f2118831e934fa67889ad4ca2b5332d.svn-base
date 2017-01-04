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



#include "stdafx.h"
#include "mov_sym.h"
#include "maps.h"
#include "showrmk.h"  //CRemarkDisplay
#include "param.h"   // PMR_get_registry_string()
#include "graphics.h"
#include "map.h"
#include "err.h"
#include "skyview.h"
#include "IconDlg.h"
#include "file.h"
#include "LinksDlg.h"
#include "mem.h"
#include "SkyViewOptionsPage.h"
#include "gps.h"
#include "factory.h"
#include "FctryLst.h"
#pragma warning(disable:4995)
#include "..\mainfrm.h"
#include "OvlFctry.h"
#include "drag_utils.h"
#include "..\mapview.h"
#include "..\overlay\OverlayCOM.h"
#include "..\getobjpr.h"
#include "SnapTo.h"
#include "..\PlaybackDialog\viewtime.h"
#include "..\StatusBarManager.h"
#include "ovl_mgr.h"

// file version defines
#define OBJ_MAJOR_VERSION 3
#define OBJ_MINOR_VERSION 20
#define OBJ_REVISION 0

// inline helper functions
inline double rotation_ang(CPoint center, CPoint orig, CPoint mov)
{
   double rot_rad;
   rot_rad = atan2((double)(orig.x-center.x),(double)(orig.y-center.y))
      - atan2((double)(mov.x-center.x),(double)(mov.y-center.y));

   return RAD_TO_DEG(rot_rad);

}

// initialize static mumber variables
Cmov_sym_overlay::enumEditorMode  Cmov_sym_overlay::m_editor_mode = SELECT_SYMBOL_MODE;
SkyViewInterface *Cmov_sym_overlay::m_skyview_interface = NULL;
bool Cmov_sym_overlay::m_deleting_skyview_interface = false;
Csym_icon* Cmov_sym_overlay::m_pviewpoint = NULL;
Csym_icon* Cmov_sym_overlay::m_pinfopoint = NULL;
CList <notify_wnd*, notify_wnd*> Cmov_sym_overlay::m_window_list(10);
BOOL Cmov_sym_overlay::s_show_skyview_symbol = TRUE;
boolean_t Cmov_sym_overlay::m_bEdit = FALSE;
BOOL Cmov_sym_overlay::m_snap_to_legs = FALSE;
COverlayCOM* Cmov_sym_overlay::m_plocked_route = NULL;
CPoint Cmov_sym_overlay::m_drag_start_loc;
BOOL Cmov_sym_overlay::m_in_drag = FALSE;
BOOL Cmov_sym_overlay::m_exceeded_drag_threshold = FALSE;
short Cmov_sym_overlay::m_file_count = 1;
CLinksEditDlg* Cmov_sym_overlay::m_link_edit_dlg = NULL;
ObjectDlg Cmov_sym_overlay::m_obj_properties_dlg;
boolean_t Cmov_sym_overlay::m_object_icons_initialized = FALSE;
CList<CIconImage *, CIconImage *> Cmov_sym_overlay::m_object_icon_list;
boolean_t g_remark_dlg_active = FALSE;
Cmov_sym_overlay::StateIndicators* Cmov_sym_overlay::s_pStateIndicators;

Cmov_sym_overlay::Cmov_sym_overlay(CString name) 
: CFvOverlayPersistenceImpl(name),
m_current_selection(NULL),
m_current_hilight(NULL),
m_bDragging(FALSE)
{
   // create an icon list for the two skyview icons (cube and sphere), and each
   // of the .obj files in the objects/skyview directory.  The list is
   // static, so we only need to do this once
   if (!m_object_icons_initialized)
   {
      create_object_icons();
      m_object_icons_initialized = TRUE;
   }
}

Cmov_sym_overlay::~Cmov_sym_overlay()
{
	// Most, if not all, destruction should be accomplished in Finalize.  
	// Due to C# objects registering for notifications, pointers to this object may not be 
	// released until the next garbage collection.
}

void Cmov_sym_overlay::Finalize()
{
   while (!m_window_list.IsEmpty())
      delete m_window_list.RemoveHead();

   // deallocate objects
   while (!m_object_lst.IsEmpty())
      delete m_object_lst.RemoveHead();
}

notify_wnd *Cmov_sym_overlay::get_notify_window(HWND wnd)
{
   POSITION pos;
   notify_wnd *pnw=NULL;

   pos = m_window_list.GetHeadPosition();
   while (pos)
   {
      pnw = m_window_list.GetNext(pos);
      if(pnw->hwnd == wnd)
      {
         return pnw;
      }
   }

   return NULL;


}

int Cmov_sym_overlay::draw(ActiveMap *map)
{
   ISettableMapProjPtr settable_map_proj;
   map->m_interface_ptr->GetSettableMapProj(&settable_map_proj);
   map_projection_utils::CMapProjWrapper mapProjWrapper(settable_map_proj);

   Csym_icon *sym=NULL;

   // if we're not the current overlay, and we haven't already
   // done so, we need to unselect our currently selected item,
   // or else it can look like two objects are selected at
   // once
   if (!get_current() && !m_already_unselected_current_selection)
   {
      if (m_current_selection)
         OVL_get_overlay_manager()->InvalidateOverlay(this);
      m_already_unselected_current_selection = TRUE;
   }

   if (get_current())
      m_already_unselected_current_selection = FALSE;

   // draw the objects in foward order so that the last element
   // in the list is drawn on top
   POSITION position = m_object_lst.GetHeadPosition();
   while (position)
   {
      SkyViewObject *object = m_object_lst.GetNext(position);
      
      object->draw(&mapProjWrapper, map->get_CDC(), 
         object == m_current_selection && m_bEdit && get_current(),
         object == m_current_hilight);
   }

   // draw the symbol last so that it gets drawn on top of everything.
   // Only draw the symbol if this is the top-most skyview overlay
   C_overlay *skyview_overlay = 
      OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SkyView);
   if (skyview_overlay == this && s_show_skyview_symbol)
   {  
      GetStateIndicators()->draw(map);
   }

   return SUCCESS;
}

Cmov_sym_overlay::StateIndicators::StateIndicators()
{ 
   m_symbol = NULL; 
   m_terrain_bounds = NULL;
   m_altitude_text = NULL;

   for (int i=0;i<4;++i)
      m_frustum_line[i] = NULL;

   m_properties = new SkyViewProperties;
   m_properties->initialize_from_registry();
}

Cmov_sym_overlay::StateIndicators::~StateIndicators()
{
   if (m_properties)
      delete m_properties;
}

void Cmov_sym_overlay::StateIndicators::draw(ActiveMap *map)
{
   if (m_properties->m_show_loaded_area && m_terrain_bounds)
   {
      OvlPen &pen = m_terrain_bounds->get_pen();
      COLORREF color;
      int style, width;
      pen.get_foreground_pen(color, style, width);
      pen.set_foreground_pen(m_properties->m_loaded_area_color, style, width);

      m_terrain_bounds->draw(map, map->get_CDC());
   }

   if (m_properties->m_show_visible_area)
   {
      int i;
      for (i=0;i<4;++i)
      {
         if (m_frustum_line[i])
         {
            OvlPen &pen = m_frustum_line[i]->get_pen();
            COLORREF color;
            int style, width;
            pen.get_foreground_pen(color, style, width);
            pen.set_foreground_pen(m_properties->m_visible_area_color, style, width);
         }
      }

      for (i=0;i<4;++i)
      {
         if (m_frustum_line[i])
         {
            m_frustum_line[i]->set_draw_mode(BACKGROUND_MODE);
            m_frustum_line[i]->draw(map, map->get_CDC());
         }
      }
      for (i=0;i<4;++i)
      {
         if (m_frustum_line[i])
         {
            m_frustum_line[i]->set_draw_mode(FOREGROUND_MODE);
            m_frustum_line[i]->draw(map, map->get_CDC());
         }
      }
   }

   const BOOL bAttachedToCamera = m_skyview_interface != NULL ? m_skyview_interface->camera_attached_to_symbol() : FALSE;
   if (m_symbol && (!bAttachedToCamera || !m_properties->m_hide_vehicle_when_attached))
   {
      m_symbol->draw(map);
   }

   if (GetStateIndicators()->m_altitude_text && m_properties->m_show_altitude)
      GetStateIndicators()->m_altitude_text->draw(map, map->get_CDC());
}

void Cmov_sym_overlay::StateIndicators::clear_state_indicators()
{
   if (m_symbol != NULL)
      DeleteSymbol();
   else
      destroy_skyview_interface();  // still need to shut down the skyview interface even if a symbol wasn't created

   if (m_terrain_bounds)
   {
      delete m_terrain_bounds;
      m_terrain_bounds = NULL;
   }

   for (int i=0;i<4;++i)
   {
      delete m_frustum_line[i];
      m_frustum_line[i] = NULL;
   }

   if (m_altitude_text)
   {
      delete m_altitude_text;
      m_altitude_text = NULL;
   }

   OVL_get_overlay_manager()->InvalidateOverlaysOfType(FVWID_Overlay_SkyView);
}

void Cmov_sym_overlay::StateIndicators::update_altitude(double Latitude, double Longitude, double Heading, double Altitude)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CView *pView = UTL_get_active_non_printing_view();
   ViewMapProj *map = UTL_get_current_view_map(pView);

   if (m_altitude_text && map)
   {
      m_altitude_text->prepare_for_redraw(map);
   }

   delete m_altitude_text;
   m_altitude_text = NULL;

   if (!pView || !map)
      return;

   m_altitude_text = new OvlText;
   
   const double ship_size = m_symbol->get_ship_symbol()->get_scale();
   
   double alt_ft = Altitude / 100.0;
   CString alt_str;
   alt_str.Format("%d", (int)(alt_ft + 0.5));
   while (alt_str.GetLength() < 3)
      alt_str = "0" + alt_str;
   
   CString font_name;
   int size, attributes;
   int fg_color;
   int bg_type, back_color;
   OvlFont &font = GetStateIndicators()->m_properties->m_altitude_font;
   font.get_font(font_name, size, attributes);
   font.get_foreground(fg_color);
   font.get_background(bg_type, back_color);

   OvlFont &alt_font = m_altitude_text->get_font();
   alt_font = GetStateIndicators()->m_properties->m_altitude_font;
   
   int width = 0, height = 0;
   CDC *dc = pView->GetDC();
   if (dc)
   {
      futil->get_text_size(dc, alt_str, font_name, size, attributes, &width, &height);
      pView->ReleaseDC(dc);
   }
   
   int x_offset = -(int)((ship_size + width / 2) * sin(DEG_TO_RAD(90 - Heading) + 0.5));
   int y_offset = -(int)((ship_size + width / 2) * cos(DEG_TO_RAD(90 - Heading) + 0.5));
   
   
   d_geo_t geo = { Latitude, Longitude };
   m_altitude_text->set_location(Latitude,Longitude);
   m_altitude_text->set_text(alt_str);
   m_altitude_text->set_anchor(geo, UTIL_ANCHOR_CENTER_CENTER, 
      CPoint(x_offset, y_offset));

   m_altitude_text->prepare_for_redraw(map);
   
   OVL_get_overlay_manager()->InvalidateOverlaysOfType(FVWID_Overlay_SkyView);
}

void Cmov_sym_overlay::StateIndicators::redefine_symbol()
{
   if (!m_symbol)
      return;
   
   OvlSymbol *sym = m_symbol->get_ship_symbol();
      
   sym->remove_all_primatives();

   // define the symbol based on the type; fallback to a bomber
   // symbol if neccessary
   if (SymbolContainer::GetInstance()->define_symbol(sym, m_properties->m_symbol_type) 
      != SUCCESS)
   {
      sym->set_unit(24.0f);
      sym->add_line(0,-24,0,24);
      sym->add_line(0,-4,-24,8);
      sym->add_line(0,-4,24,8);
      sym->add_line(0,21,-9,24);
      sym->add_line(0,21,9,24);
   }
}

void Cmov_sym_overlay::StateIndicators::update_terrain_bounds(BOOL terrain_is_loaded,
         double terrain_bounding_lat_south, double terrain_bounding_lon_west, 
         double terrain_bounding_lat_north, double terrain_bounding_lon_east)
{
   ViewMapProj *map = UTL_get_current_view_map();

   if (m_terrain_bounds)
   {
      // invalidate where the terrain bounds where
      if (map)
      {
         m_terrain_bounds->prepare_for_redraw(map);
      }
   }

   if (terrain_is_loaded)
   {
      if (m_terrain_bounds == NULL)
      {
         m_terrain_bounds = new GeoBounds;
         OvlPen &pen = m_terrain_bounds->get_pen();
         pen.set_foreground_pen(RGB(255,255,255), UTIL_LINE_SOLID, 2);
      }
      
      m_terrain_bounds->set_bounds(terrain_bounding_lat_south, terrain_bounding_lon_west,
         terrain_bounding_lat_north, terrain_bounding_lon_east);
      
      // invalidate where the terrain bounds are now
      if (map)
      {
         m_terrain_bounds->prepare_for_redraw(map);
      }
   }
   else
   {
      delete m_terrain_bounds;
      m_terrain_bounds = NULL;
   }

   OVL_get_overlay_manager()->InvalidateOverlaysOfType(FVWID_Overlay_SkyView);
}

bool Cmov_sym_overlay::StateIndicators::find_intersection(
   double lat1, double lon1,
   double camera_lat, double camera_lon,
   double north_lat, double west_lon, 
   double south_lat, double east_lon, 
   double &intersect_lat, double &intersect_lon)
{
   // compute the heading from the camera the corner frustum point
   double unused_d;
   double bearing;
   GEO_calc_range_and_bearing(camera_lat, camera_lon, lat1, lon1, 
                                 &unused_d, &bearing, true);

   // create a line that extends from the location of the ship
   // out far enough for intersection testing
   double lat2, lon2;
   const double distance = 100187.17 / 2.0;
   GEO_calc_end_point(lat1, lon1, distance, bearing, &lat2, &lon2, true);

   if (GEO_great_circle_intersection(lat1, lon1, lat2, lon2, 
      south_lat, west_lon, north_lat, west_lon, &intersect_lat, &intersect_lon))
      return true;

   if (GEO_great_circle_intersection(lat1, lon1, lat2, lon2, 
      north_lat, west_lon, north_lat, east_lon, &intersect_lat, &intersect_lon))
      return true;

   if (GEO_great_circle_intersection(lat1, lon1, lat2, lon2, 
      north_lat, east_lon, south_lat, east_lon, &intersect_lat, &intersect_lon))
      return true;

   if (GEO_great_circle_intersection(lat1, lon1, lat2, lon2, 
      south_lat, east_lon, south_lat, west_lon, &intersect_lat, &intersect_lon))
      return true;

   return false;
}

void Cmov_sym_overlay::StateIndicators::update_frustum(
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
   double terrain_point_seen_at_lower_right_screen_point_lon)
{
   ViewMapProj *map = UTL_get_current_view_map();

   for (int i=0;i<4;++i)
   {
      if (m_frustum_line[i] && map)
      {
         m_frustum_line[i]->prepare_for_redraw(map);
      }
      delete m_frustum_line[i];
      m_frustum_line[i] = NULL;
   }

   if (terrain_point_seen_at_lower_left_screen_point_found && 
       terrain_point_seen_at_lower_right_screen_point_found)
   {
      m_frustum_line[0] = new GeoLine;

      m_frustum_line[0]->set_start(terrain_point_seen_at_lower_left_screen_point_lat,
         terrain_point_seen_at_lower_left_screen_point_lon);
      m_frustum_line[0]->set_end(terrain_point_seen_at_lower_right_screen_point_lat,
         terrain_point_seen_at_lower_right_screen_point_lon);

      OvlPen &pen = m_frustum_line[0]->get_pen();
      pen.set_foreground_pen(RGB(255,255, 255), UTIL_LINE_SOLID, 2);
   }

   if (terrain_point_seen_at_lower_left_screen_point_found && 
       terrain_point_seen_at_upper_left_screen_point_found)
   {
      m_frustum_line[1] = new GeoLine;

      m_frustum_line[1]->set_start(terrain_point_seen_at_lower_left_screen_point_lat,
         terrain_point_seen_at_lower_left_screen_point_lon);
      m_frustum_line[1]->set_end(terrain_point_seen_at_upper_left_screen_point_lat,
         terrain_point_seen_at_upper_left_screen_point_lon);

      OvlPen &pen = m_frustum_line[1]->get_pen();
      pen.set_foreground_pen(RGB(255,255, 255), UTIL_LINE_SOLID, 2);
   }
   else if (terrain_point_seen_at_lower_left_screen_point_found && terrain_is_loaded)
   {
      double intersect_lat, intersect_lon;

      if (find_intersection(
         terrain_point_seen_at_lower_left_screen_point_lat, 
         terrain_point_seen_at_lower_left_screen_point_lon,
         camera_lat, camera_lon,       
         terrain_bounding_lat_north, terrain_bounding_lon_west, 
         terrain_bounding_lat_south, terrain_bounding_lon_east,
         intersect_lat, intersect_lon) == TRUE)
      {
         m_frustum_line[1] = new GeoLine;

         m_frustum_line[1]->set_start(terrain_point_seen_at_lower_left_screen_point_lat,
            terrain_point_seen_at_lower_left_screen_point_lon);
         m_frustum_line[1]->set_end(intersect_lat,
            intersect_lon);

         OvlPen &pen = m_frustum_line[1]->get_pen();
         pen.set_foreground_pen(RGB(255,255, 255), UTIL_LINE_DASH2, 2);
      }
   }

   if (terrain_point_seen_at_lower_right_screen_point_found && 
       terrain_point_seen_at_upper_right_screen_point_found)
   {
      m_frustum_line[2] = new GeoLine;

      m_frustum_line[2]->set_start(terrain_point_seen_at_lower_right_screen_point_lat,
         terrain_point_seen_at_lower_right_screen_point_lon);
      m_frustum_line[2]->set_end(terrain_point_seen_at_upper_right_screen_point_lat,
         terrain_point_seen_at_upper_right_screen_point_lon);

      OvlPen &pen = m_frustum_line[2]->get_pen();
      pen.set_foreground_pen(RGB(255,255, 255), UTIL_LINE_SOLID, 2);
   }
   else if (terrain_point_seen_at_lower_right_screen_point_found && terrain_is_loaded)
   {
      double intersect_lat, intersect_lon;

      if (find_intersection(
         terrain_point_seen_at_lower_right_screen_point_lat, 
         terrain_point_seen_at_lower_right_screen_point_lon,
         camera_lat, camera_lon, 
         terrain_bounding_lat_north, terrain_bounding_lon_west, 
         terrain_bounding_lat_south, terrain_bounding_lon_east,
         intersect_lat, intersect_lon) == TRUE)
      {
         m_frustum_line[2] = new GeoLine;

         m_frustum_line[2]->set_start(terrain_point_seen_at_lower_right_screen_point_lat,
            terrain_point_seen_at_lower_right_screen_point_lon);
         m_frustum_line[2]->set_end(intersect_lat, intersect_lon);

         OvlPen &pen = m_frustum_line[2]->get_pen();
         pen.set_foreground_pen(RGB(255,255,255), UTIL_LINE_DASH2, 2);
      }
   }

   if (terrain_point_seen_at_upper_left_screen_point_found && 
       terrain_point_seen_at_upper_right_screen_point_found)
   {
      m_frustum_line[3] = new GeoLine;

      m_frustum_line[3]->set_start(terrain_point_seen_at_upper_left_screen_point_lat,
            terrain_point_seen_at_upper_left_screen_point_lon);
      m_frustum_line[3]->set_end(terrain_point_seen_at_upper_right_screen_point_lat,
         terrain_point_seen_at_upper_right_screen_point_lon);

      OvlPen &pen = m_frustum_line[3]->get_pen();
      pen.set_foreground_pen(RGB(255,255, 255), UTIL_LINE_SOLID, 2);
   }

   if (map)
   {
      for (int i=0;i<4;++i)
      {
         if (m_frustum_line[i])
         {
            m_frustum_line[i]->prepare_for_redraw(map);
         }
      }
   }

   OVL_get_overlay_manager()->InvalidateOverlaysOfType(FVWID_Overlay_SkyView);
}

BOOL Cmov_sym_overlay::is_viewer_registered()
{
   return (get_viewer_name() != "");
}

CString Cmov_sym_overlay::get_viewer_name()
{
   static CString viewer_name = PRM_get_registry_string("3D_viewer","viewer_name","");
   return viewer_name;
}

int Cmov_sym_overlay::open_skyview()
{
   // toggle the connection to skyview
   
   if (m_skyview_interface != NULL)
   {
      destroy_skyview_interface();
      return SUCCESS;
   }

   m_skyview_interface = new SkyViewInterface;
   if (m_skyview_interface->init() != SUCCESS)
   {
      delete m_skyview_interface;
      m_skyview_interface = NULL;
      return FAILURE;
   }
   OnConnectionInitialized();

   return SUCCESS;
}

void Cmov_sym_overlay::OnConnectionInitialized()
{
   // signal any overlays that implement the ISkyViewConnectionEvents interface that the SkyView connection
   // has been opened
   try
   {
      COverlayCOM *pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Route));
      while (pOverlay)
      {
         ISkyViewConnectionEventsPtr spConnectionEvents = pOverlay->GetFvOverlay();
         if (spConnectionEvents != NULL)
            spConnectionEvents->OnSkyViewConnectionInitialized();

         pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->get_next_of_type(pOverlay, FVWID_Overlay_Route));
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unassociating playback objects failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
}

void Cmov_sym_overlay::OnConnectionTerminating()
{
   // signal any overlays that implement the ISkyViewConnectionEvents interface that the SkyView connection
   // is about to be shutdown
   try
   {
      COverlayCOM *pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Route));
      while (pOverlay)
      {
         ISkyViewConnectionEventsPtr spConnectionEvents = pOverlay->GetFvOverlay();
         if (spConnectionEvents != NULL)
            spConnectionEvents->OnSkyViewConnectionTerminating();

         pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->get_next_of_type(pOverlay, FVWID_Overlay_Route));
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unassociating playback objects failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
}

void Cmov_sym_overlay::OnUpdateSkyview(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_skyview_interface != NULL);
}

int Cmov_sym_overlay::center_symbols(MapProj* map)
{
   d_geo_t geo;

   // no symbols so just return
   if (GetStateIndicators()->m_symbol == NULL)
      return SUCCESS;

   if(map->get_actual_center(&geo) != FAILURE)
   {
      set_symbol_position(geo.lat,geo.lon,map);
      return SUCCESS;
   }

   return FAILURE;
}

int Cmov_sym_overlay::skyview_object_dlg(MapProj* map)
{
   if (CIconDragDlg::get_dlg() == NULL)
   {
      CIconDragDlg* icon_dlg = new CIconDragDlg("3D Object Icons");
      icon_dlg->set_dlg(icon_dlg);

      POSITION position = m_object_icon_list.GetHeadPosition();
      while (position)
         icon_dlg->add_icon(m_object_icon_list.GetNext(position));

      icon_dlg->sort_icons();
   }

   else if (!CIconDragDlg::get_dlg()->IsWindowVisible())
      CIconDragDlg::get_dlg()->ShowWindow(SW_SHOW);

   return SUCCESS;
}

// create an additional icon for each object file in 
// HD_DATA/objects/skyview and add it to the list.  The list of
// CIconImage's will be added to the 3D Object Palette and can
// be dropped onto the map
void Cmov_sym_overlay::create_object_icons()
{
   // load the two 'standard' icons that should have been installed
   CIconImage *image = CIconImage::load_images("skyview\\Cube.ico", "Cube", 0); 
   if (image)
      m_object_icon_list.AddTail(image);
         
   image = CIconImage::load_images("skyview\\Sphere.ico", "Sphere", 0); 
   if (image)
      m_object_icon_list.AddTail(image);

   image = CIconImage::load_images("skyview\\Pyramid.ico", "Pyramid", 0); 
   if (image)
      m_object_icon_list.AddTail(image);

   image = CIconImage::load_images("skyview\\Cylinder.ico", "Cylinder", 0); 
   if (image)
      m_object_icon_list.AddTail(image);
         
   //
   // add any .obj files found
   CString object_path = PRM_get_registry_string("Main","HD_DATA");
   object_path += "\\objects\\skyview\\";
   WIN32_FIND_DATA find_file_data;
   HANDLE handle = FindFirstFile(object_path + "*.obj", &find_file_data);
   
   if (handle == INVALID_HANDLE_VALUE)
      TRACE("No SkyView object were found in the objects/skyview directory");
   else 
   {
      // remove the ".obj" to retrieve the title - 4 characters
      CString title;
      title = find_file_data.cFileName;
      title = title.Left(title.GetLength()-4);

      image = CIconImage::load_images("skyview\\obj.ico", title, 0, TRUE);
   
      // add the object to the list
      m_object_icon_list.AddTail(image);
      
      while(FindNextFile(handle, &find_file_data))
      {
         // remove the ".obj" - 4 characters
         CString title;
         title = find_file_data.cFileName;
         title = title.Left(title.GetLength()-4);

         image = CIconImage::load_images("skyview\\obj.ico", title, 0, TRUE);
      
         // add the object to the list
         m_object_icon_list.AddTail(image);
      }
      
      FindClose(handle);
   }
}

int Cmov_sym_overlay::pre_close(BOOL* cancel)
{
   // indicate no cancel if cancel boolean was passed in
   if (cancel)
   {
      *cancel = FALSE;
   }

   // destroy the skyview interface if this is the last skyview overlay
   C_overlay *overlay = OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);

   int i = 0;
   while (overlay)
   {
      i++;
      overlay = OVL_get_overlay_manager()->get_next_of_type(overlay, FVWID_Overlay_SkyView);
   }

   if (i == 1)
      destroy_skyview_interface();

   m_pviewpoint = NULL;

   // close the icon palette, if open
   if (CIconDragDlg::get_dlg())
      CIconDragDlg::get_dlg()->close_dlg();
   
   // close the Link Editor, if open
   if (m_link_edit_dlg)
   {
      delete m_link_edit_dlg;
      m_link_edit_dlg = NULL;
   }
   
   // close the object properties dialog, if open
   if (m_obj_properties_dlg.m_hWnd)
   {
      // force to save changes   
      m_obj_properties_dlg.set_focus(NULL);
      
      // destroy the window
      m_obj_properties_dlg.DestroyWindow();
   }

   // remove the info box if this overlay has its focus
   if (this == CRemarkDisplay::get_focus_overlay())
      CRemarkDisplay::close_dlg();

   return SUCCESS;
}

// Receives all keydown messeges from the Overlay Manager and filters them 
// for this overlay... a return of false will allow the keystroke to be 
// processed normally in MapView
boolean_t Cmov_sym_overlay::offer_keydown(IFvMapView *pMapView, UINT nChar, 
                                          UINT nRepCnt, UINT nFlags, 
                                          CPoint point)
{
   if (!m_bEdit)
      return FALSE;

   if ((nFlags & 0x000000FF) == 0x1D &&
      m_in_drag && m_current_selection != NULL)
   {
      SetCursor(AfxGetApp()->LoadCursor(IDC_ADD_CURSOR));
      
      return TRUE;
   }
   
   switch (nChar)
   {
   case VK_DELETE:
      {
         // if this is a repeated key messege, then skip ths notice
         // cause we don't want delete repeats to occur
         if ((nFlags & (1 << 14))  !=  0)
            return TRUE;
         
         if (m_current_selection)
         {
            delete_point(OVL_get_overlay_manager()->get_view_map(), m_current_selection);
            return TRUE;
         }
         break;
      }
   case VK_ESCAPE:
      if (m_bDragging)
      {
         cancel_drag(pMapView);
         return TRUE;
      }
   }
      
   return FALSE;
}

// delete the given point from the object list and cycle to the next visible
// point
int Cmov_sym_overlay::delete_point(ViewMapProj *map, SkyViewObject *object)
{
   // if the edit dialog is up and contains this point, set editor dialog focus to
   // NULL before deleting the point in focus, do NOT update point before deleting

   //cycle to next point
   cycle(map, object);

   // get the position of the given object
   POSITION pos = NULL;
   pos = m_object_lst.Find(object);
   
   // delete the object from the list
   if (pos)
   {
      SkyViewObject *object = m_object_lst.GetAt(pos);
      CRect rect = object->get_rect();
      rect.InflateRect(GRA_handle_size(),GRA_handle_size());
      m_object_lst.RemoveAt(pos);

      if (m_skyview_interface)
         m_skyview_interface->DeleteObject(object->get_handle());

      delete object;
      OVL_get_overlay_manager()->InvalidateOverlay(this);

      // the overlay has been changed, set the modified flag
      set_modified(TRUE);

      return SUCCESS;
   }
   else
      return FAILURE;
}

// cycle to the next object in the list that is visible on the screen
int Cmov_sym_overlay::cycle(ViewMapProj *map, SkyViewObject *object, 
                            boolean_t direction_forward /*=TRUE*/)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(map->GetSettableMapProj());

   POSITION initial_pos = m_object_lst.Find(object);
   POSITION current_position = initial_pos;
   
   object = get_next_object(current_position, direction_forward);
   if (initial_pos)
   {
      for (;;)
      {
         // if we've been all the way around the loop
         if (current_position == initial_pos)
            break;
         
         // get the next object in the list with the given direction
         object = get_next_object(current_position, direction_forward);
         
         // if this point is not in bounds, then skip it
         if (!object->in_view(map))
            continue;

         // invalidate the old object
         if (m_current_selection)
            OVL_get_overlay_manager()->InvalidateOverlay(this);
         
         // set the current selection to this object and draw it now
         CClientDC dc(map->get_CView());
         object->draw(&mapProjWrapper, &dc, TRUE, m_current_hilight == object);
         
         // set editor dialog focus to this new point
         if (m_obj_properties_dlg.m_hWnd)
            m_obj_properties_dlg.set_focus(object);
         
         // set the links focus to the current selection
         if (m_link_edit_dlg)
            m_link_edit_dlg->set_focus("SkyView Links", &(object->m_links));

         m_current_selection = object;

         return SUCCESS;
      }
   }
   
   // no element to cycle to so set the current selection to NULL
   m_current_selection = NULL;
   
   // set editor dialog focus to this new point
   if (m_obj_properties_dlg.m_hWnd)
      m_obj_properties_dlg.set_focus(NULL);
         
   return FAILURE;
}

// return the next object in the list.  loop around in the given direction
SkyViewObject *Cmov_sym_overlay::get_next_object(POSITION& position, 
                                                 boolean_t direction_forward)
{
   SkyViewObject *object; 

   if (direction_forward)
   {
      // forward loop
      if (position)
         object = m_object_lst.GetNext(position); 
      if (position == NULL)
         position = m_object_lst.GetHeadPosition();
   }
   else
   {
      // backwards loop
      object = m_object_lst.GetPrev(position); 
      if (position == NULL)
         position = m_object_lst.GetTailPosition();
   }

   return object;
}

// Receives all keydown messeges from the Overlay Manager and filters them 
// for this overlay... a return of false will allow the keystroke to be 
// processed normally in MapView
boolean_t Cmov_sym_overlay::offer_keyup(IFvMapView *pMapView, UINT nChar, 
                                        UINT nRepCnt, UINT nFlags, 
                                        CPoint point)
{
   if (!m_bEdit)
      return FALSE;

   if ((nFlags & 0x000000FF) == 0x1D &&
      m_in_drag && m_current_selection != NULL)
   {
      SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      
      return TRUE;
   }

   return FALSE;
}

// called by CRemarkDisplay when this overlay looses the info box focus
void Cmov_sym_overlay::release_focus()
{
   set_info_box_focus(NULL);

   if (m_current_hilight)
   {
      OVL_get_overlay_manager()->InvalidateOverlay(this);
      m_current_hilight = NULL;
   }

   g_remark_dlg_active =  FALSE;
}

void Cmov_sym_overlay::release_edit_focus()
{
   if (m_pviewpoint)
      m_pviewpoint->unselect();
}


int Cmov_sym_overlay::set_edit_on(boolean_t edit)
{
   m_bEdit = edit;
   if (edit)
   {
      set_editor_mode(SELECT_SYMBOL_MODE);
   }
   // if we are leaving the editor
   else
   {
      // turn off the icon dialog if it is opened
      if (CIconDragDlg::get_dlg())
         CIconDragDlg::get_dlg()->close_dlg();
      
      // close the Link Editor, if open
      if (m_link_edit_dlg)
      {
         delete m_link_edit_dlg;
         m_link_edit_dlg = NULL;
      }
      
      // close the object properties dialog, if open
      if (m_obj_properties_dlg.m_hWnd)
      {
         // force to save changes   
         m_obj_properties_dlg.set_focus(NULL);
         
         // destroy the window
         m_obj_properties_dlg.DestroyWindow();
      }
   }

   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// add menu items to the menu 
//
void Cmov_sym_overlay::menu(ViewMapProj *map, CPoint point,
   CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(map->GetSettableMapProj());

   C_icon *icon = hit_test(&mapProjWrapper, point);

   if (icon == NULL)
      return;

   // if the point is over the symbol
   else if (icon->is_kind_of("Csym_icon"))
   {
      Csym_icon *psym = (Csym_icon *)icon;

      // if we are in edit mode
      if (m_bEdit)
      {
         if (m_snap_to_legs)
         {
            list.AddTail(new CCIconMenuItem("Disable Snap to Route",
               icon, &snap_disable));
         }
         else
         {
            list.AddTail(new CCIconMenuItem("Enable Snap to Route",
               icon, &snap_enable));
         }
      }
      
      list.AddTail(new CCIconMenuItem("Get Symbol Info...",
         psym, &get_symbol_info));
   }
   // if the point is over a skyview object
   else if (icon->is_kind_of("SkyViewObject"))
   {
      SkyViewObject *object = (SkyViewObject *)icon;

      // Set the current point as selected.  This is for when
      // a user has one point selected, then right-clicks another
      // point.  The new point needs to be marked as selected.
      if (get_current() && m_bEdit)
      {
         if (m_current_selection)
            OVL_get_overlay_manager()->InvalidateOverlay(this);
         
         // set the links focus to the current selection
         if (m_link_edit_dlg)
            m_link_edit_dlg->set_focus("SkyView Links", &(object->m_links));
         
         m_current_selection = object;
         OVL_get_overlay_manager()->InvalidateOverlay(this);
         
         // set editor dialog focus to this new point
         if (m_obj_properties_dlg.m_hWnd)
            m_obj_properties_dlg.set_focus(m_current_selection);  
      }
      
      list.AddTail(new CCIconMenuItem("Get Object Info...",
         icon, &get_object_info));

      if (get_current() && m_bEdit)
      {
         list.AddTail(new CCIconMenuItem("Edit Object Info...",
            icon, &edit_object_info));

         // if the links dialog is not already opened
         if (m_link_edit_dlg == NULL || m_link_edit_dlg->GetHwnd() == NULL)
         {
            list.AddTail(new CCIconMenuItem("Edit Object Links...", icon,
               &edit_object_links));
         }

         list.AddTail(new CCIconMenuItem("Delete Object",
            icon, &delete_object));
      }

      object->add_links_to_menu(list);
   }
}

C_icon *Cmov_sym_overlay::hit_test(map_projection_utils::CMapProjWrapper *map, CPoint point)
{
   POSITION position;
   Csym_icon *sym=NULL;

   if (GetStateIndicators()->m_symbol && GetStateIndicators()->m_symbol->hit_test(point))
      return GetStateIndicators()->m_symbol;

   // see if the point is over a SkyView object.  We traverse this
   // list backwards because the objects are drawn in this order
   position = m_object_lst.GetTailPosition();
   while (position)
   {  
      SkyViewObject *object = m_object_lst.GetPrev(position);
      if (object->hit_test(point))
         return object;
   }

   return NULL;
}

int Cmov_sym_overlay::on_mouse_moved(IFvMapView *pMapView, CPoint point, 
                                    UINT flags)
{
   Csym_icon *sym=NULL;

   if (m_bDragging)
   {
      HCURSOR cursor = NULL;
      HintText *hintText = NULL;

      drag(pMapView, point, flags, &cursor, &hintText);

      if (cursor != NULL)
         pMapView->SetCursor(cursor);
      if (hintText != NULL)
      {
         pMapView->SetTooltipText(_bstr_t(hintText->get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hintText->get_help_text()));
      }

      return SUCCESS;
   }

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   
   if (!m_bEdit)
   {
      // ------------ BEGIN: NOT 3d viewer MODE --------------
      
      // see if the cursor is over the symbol
      C_icon *icon = hit_test(&mapProjWrapper, point);
      if (icon == NULL)
         return FAILURE;
      else if (icon->is_kind_of("Csym_icon"))
      {
         sym = (Csym_icon *)icon;

         CString s;
         // convert lat-lon to formatted string in default format and datum
         GEO_lat_lon_to_string(sym->get_latitude(), sym->get_longitude(), s.GetBuffer(255), 255);
         s.ReleaseBuffer();
         m_hint.set_tool_tip(s);
         m_hint.set_help_text(s);
         pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
         pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
         return SUCCESS;
      }
      // if the cursor is over a skyview object
      else if(icon->is_kind_of("SkyViewObject"))
      {
         SkyViewObject *object = (SkyViewObject *)icon;

         CString strText("SkyView object: ");

         m_hint.set_tool_tip(strText + object->get_tool_tip());
         m_hint.set_help_text(strText + object->get_help_text());
         pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
         pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
         
         // if we are not over a point, then return without handling the select
         return SUCCESS;
      }
      // unknown icon type
      else 
         return FAILURE;
      
      // ------------ END: NOT IN viewer MODE --------------
   }  
   else
   {
      if (get_editor_mode() == SELECT_SYMBOL_MODE)  //if using select tool...
      {
         
         // --------------------- BEGIN: SELECT TOOL ------------------
         
         if (m_pviewpoint) // somthing already selected so check for move or rotate
         {
            //see if we hit the tracker rectangle
            int hit = m_pviewpoint->hit_test(point);
            
            if ( hit==Csym_icon::ROTATE)
            {
               if (m_skyview_interface != NULL && m_skyview_interface->can_rotate_symbol()) // hit rotate handle && symbol can be rotated
               {
                  m_hint.set_tool_tip("Rotate");
                  m_hint.set_help_text("Change heading");
                  pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_ROTATE_CURSOR));
                  pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
                  pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
                  return SUCCESS;
               }
               else
               {
                  m_hint.set_tool_tip("Heading Locked");
                  m_hint.set_help_text("The symbol is under program control and can't be dragged");
                  pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
                  pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
                  pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
                  return SUCCESS;
               }
            }
            else if (hit==Csym_icon::IN_RECT)
            {
               if (m_skyview_interface != NULL && m_skyview_interface->can_move_symbol()) // hit symbol and the symbol can be moved
               {
                  m_hint.set_tool_tip("Move");
                  m_hint.set_help_text("Change position");
                  pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR));
                  pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
                  pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
                  return SUCCESS;               
               }
               else
               {
                  m_hint.set_tool_tip("Position Locked");
                  m_hint.set_help_text("The symbol is under program control and can't be dragged");
                  pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
                  pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
                  pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
                  return SUCCESS;
               }
            } 
         }
         
         C_icon *icon = hit_test(&mapProjWrapper,point);
         if (icon == NULL)
         {
            return FAILURE;
         }
         // if the cursor is over the symbol
         else if (icon->is_kind_of("Csym_icon"))
         {
            sym = (Csym_icon *)icon;
            if (m_skyview_interface != NULL && m_skyview_interface->can_move_symbol())
            {
               m_hint.set_tool_tip("Move");
               m_hint.set_help_text("Change position");
               pMapView->SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR));
               pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
               pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
            }
            else
            {
               m_hint.set_tool_tip("Position Locked");
               m_hint.set_help_text("The symbol is under program control and can't be dragged");
               pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
               pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
               pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
            }
            return SUCCESS;         
         }
         // if the cursor is over a skyview object
         else if (icon->is_kind_of("SkyViewObject"))
         {
            
            SkyViewObject *object = (SkyViewObject *)icon;
            pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
            if (get_current())
            {
               CString strTooltip("Select ");
               strTooltip += object->get_tool_tip();

               CString strHelptext("Select ");
               strHelptext += object->get_help_text();

               m_hint.set_tool_tip(strTooltip);
               m_hint.set_help_text(strHelptext);
            }
            else
            {
               CString s;
               s.Format("Select %s", OVL_get_overlay_manager()->GetOverlayDisplayName(this));
               m_hint.set_tool_tip(s);
            }
            pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
            pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
            return SUCCESS;
         }
         // unknown icon type
         else
            return FAILURE;

         // --------------------- END: SELECT TOOL ------------------
         
      } //if select tool      
      else
      {
         
         // --------------------- BEGIN: different TOOL ------------------
         
         ASSERT(FALSE); //only  one tool right now
      }
   }  //else - (matching: if not in skyview editor mode)
   return FAILURE;

}

int Cmov_sym_overlay::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_bDragging)
   {
      drop(pMapView, point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}

// This function is called to get the default cursor associated with the
// current mode.
HCURSOR Cmov_sym_overlay::get_default_cursor()
{
   return AfxGetApp()->LoadCursor(IDC_SELECT_CURSOR);
}

// Allows the overlay manager to pass double-click notices to the route,
// If the point hits a turn point or additional point with links, then those
// links will be displayed.
int Cmov_sym_overlay::on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point) 
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   C_icon *icon = hit_test(&mapProjWrapper, point);

   // if the point hit a turn point or an additional point
   if (icon && (icon->is_kind_of("SkyViewObject")))
   {
      SkyViewObject *obj = (SkyViewObject *)icon;
      obj->view_all_links();
      return SUCCESS;
   }

   return FAILURE;
}

int Cmov_sym_overlay::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{ 
   Csym_icon *sym=NULL;
   
   ASSERT(pMapView);

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   
   if (OVL_get_overlay_manager()->GetCurrentEditor() != FVWID_Overlay_SkyView)
   {
      // ------------ BEGIN: NOT 3d viewer MODE --------------
      
      // see if the cursor is over an icon
      C_icon *icon = hit_test(&mapProjWrapper, point);
      if(icon == NULL)
      {
         // if we are not over a point, then return without handling the select
         return FAILURE;
      }
      // if the point is over the symbol
      else if (icon->is_kind_of("Csym_icon"))
      {
         CString s;
         sym = (Csym_icon *)icon;

         return SUCCESS;
      }
      // if the point is over a skyview object
      else if (icon->is_kind_of("SkyViewObject"))
      {
         // unhilight the current selection
         if (m_current_hilight)
            OVL_get_overlay_manager()->InvalidateOverlay(this);

         // we are not in the editor so show the info dialog box
         // for the selected point
         show_object_info(OVL_get_overlay_manager()->get_view_map(), (SkyViewObject *)icon);

         return SUCCESS;

      }
      // unknown icon type
      else
         return FAILURE;
      
      // ------------ END: NOT IN viewer MODE --------------
   }  
   else
   {
      if (get_editor_mode() == SELECT_SYMBOL_MODE)  //if using select tool...
      {
         
         // --------------------- BEGIN: SELECT TOOL ------------------
         
         if (m_pviewpoint) // somthing already selected so check for move or rotate
         {
            //see if we hit the tracker rectangle
            int hit = m_pviewpoint->hit_test(point);
            
            if ( hit==Csym_icon::ROTATE)
            {
               if (m_skyview_interface != NULL && m_skyview_interface->can_rotate_symbol()) // hit rotate handle && symbol can be rotated
               {
                  m_bDragging = TRUE;
                  m_drag_mode = Rotate;
                  return SUCCESS;
               }
               else
               {
                  return SUCCESS;
               }
            }
            else if (hit==Csym_icon::IN_RECT)
            {
               if (m_skyview_interface != NULL && m_skyview_interface->can_move_symbol()) // hit symbol and the symbol can be moved
               {
                  m_bDragging = TRUE;
                  m_drag_mode = Move;
                  return SUCCESS;               
               }
               else
               {
                  return SUCCESS;
               }
            } 
         }
         
         C_icon *icon = hit_test(&mapProjWrapper,point);
         if (icon && icon->is_kind_of("Csym_icon"))
         {
            sym = (Csym_icon *)icon;
            if (sym &&  (m_skyview_interface != NULL && m_skyview_interface->can_move_symbol()))
            {
               m_bDragging = TRUE;
               m_drag_mode = Move;
            }

            // unselect any current selection
            if (m_current_selection)
            {
               OVL_get_overlay_manager()->InvalidateOverlay(this);
               m_current_selection = NULL;

               // set editor dialog focus to NULL
               if (m_obj_properties_dlg.m_hWnd)
                  m_obj_properties_dlg.set_focus(NULL);
               
               // set the links focus to the current selection
               if (m_link_edit_dlg)
               {
                  CStringArray unused;
                  m_link_edit_dlg->set_focus("None", &unused);
               }
            }
            
            set_current_selection(sym);

            return SUCCESS;
         }
         else if (icon && icon->is_kind_of("SkyViewObject"))
         {
            SkyViewObject *object = (SkyViewObject *)icon;

            // if this isn't already the current overlay, then make it so
            if (!get_current())
               OVL_get_overlay_manager()->make_current(this);

            // if the remark dialog is up AND we have the focus, then go ahead 
            // with point info for this hit point
            if (CRemarkDisplay::test_active()  &&  
               (CRemarkDisplay::get_focus_overlay() == this))
            {
               // unhilight the current selection
               if (m_current_hilight)
                  OVL_get_overlay_manager()->InvalidateOverlay(this);

               // show the info dialog box for the selected point
               show_object_info(OVL_get_overlay_manager()->get_view_map(), (SkyViewObject *)icon);
            }

            // unselect any current selection
            if (m_current_selection && m_current_selection != object)
               OVL_get_overlay_manager()->InvalidateOverlay(this);

            // unselect the symbol
            if (m_pviewpoint)
            {
               m_pviewpoint->unselect();
               m_pviewpoint = NULL;
            }

            if (m_current_selection != object)
            {
               // set the links focus to the current selection
               if (m_link_edit_dlg)
                  m_link_edit_dlg->set_focus("SkyView Links", &(object->m_links));
               
               // select the hit member, but go ahead and draw immediately rather 
               // than posting invalidation
               m_current_selection = object;

               CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));
               object->draw(&mapProjWrapper, &dc, TRUE, m_current_hilight == object);
               
               // set editor dialog focus to this new point
               if (m_obj_properties_dlg.m_hWnd)
                  m_obj_properties_dlg.set_focus(m_current_selection);
            }

            // tell overlay manager that we can drag this object
            m_drag_start_loc = point;
            m_bDragging = TRUE;

            return SUCCESS;
         }
         
         // --------------------- END: SELECT TOOL ------------------
         
      } //if select tool      
      else
      {
         
         // --------------------- BEGIN: different TOOL ------------------
         
         ASSERT(FALSE); //only  one tool right now
      }
   }  //else - (matching: if not in point export editor mode)
   
   return FAILURE;
}

// ----------------------------------------------------------------------------
//sets the current hilight to point, invalidating the screen as necessary
//Note: draws by invalidation (not immediate)
void Cmov_sym_overlay::set_current_selection(Csym_icon* icon)
{
   //Note: point may be NULL
   
   if (m_pviewpoint != icon)  //if the point is not already in focus
   {
      if (m_pviewpoint)
      {
         m_pviewpoint->unselect();
      }

      m_pviewpoint=icon;
      if (m_pviewpoint)
      {
         m_pviewpoint->draw_selected();
      }
   }
}

// ----------------------------------------------------------------------------
//sets the current hilight to point, invalidating the screen as necessary
//Note: draws by invalidation (not immediate)
void Cmov_sym_overlay::set_info_box_focus(Csym_icon* icon)
{
   //Note: point may be NULL
   
   if (m_pinfopoint != icon)  //if the point is not already in focus
   {
      if (m_pinfopoint)
      {
         m_pinfopoint->un_infoselect();
      }

      m_pinfopoint=icon;
      if (m_pinfopoint)
      {
         m_pinfopoint->draw_infoselected();
      }
   }
}

      
void Cmov_sym_overlay::drag(IFvMapView* pMapView, CPoint point, UINT nFlags, HCURSOR *cursor, HintText **hint)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

   pMapView->ScrollMapIfPointNearEdge(point.x, point.y);

   // if we are dragging a symbol
   if (m_pviewpoint != NULL)
      drag_symbol(&mapProjWrapper, &dc, point, nFlags, cursor, hint);

   // if we are dragging a skyview object
   else if (m_current_selection != NULL)
      drag_object(&mapProjWrapper, &dc, point, nFlags, cursor, hint);
}

void Cmov_sym_overlay::drag_symbol(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint point, UINT nFlags, HCURSOR *cursor, HintText **hint)
{
   *cursor = NULL;
   *hint = NULL;

   // don't do anything at all if we haven't moved and we're not yet 
   // in drag mode
   if (m_drag_start_loc == point  &&  !m_in_drag)
      return;

   //see if we have exceeded the drag threshold, if so note that
   if (!m_exceeded_drag_threshold && !drag_utils::in_drag_threshold(m_drag_start_loc, point))
   {
      m_exceeded_drag_threshold = TRUE;
   }  
   // set the cursor to the move cursor if we are within the threshold region
   // and have not yet exceeded the drag threshold
   if (m_exceeded_drag_threshold )
   {
      if (m_drag_mode == Move)
      {
         *cursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
         
         // if we haven't started the drag yet, do so now
         if (!m_in_drag)
         {
            m_pviewpoint->begin_drag(map, pDC, point);
            m_in_drag = TRUE;
         }
         
         
         // Check to see if the cursor is over an icon that can be snapped to with
         // the current context.  test_snap_to() returns TRUE in this case.
         if (OVL_get_overlay_manager()->test_snap_to(OVL_get_overlay_manager()->get_view_map(), point))
         {
            *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
            m_hint.set_help_text("Snap to point");
            m_hint.set_tool_tip("Snap To");
            *hint = &m_hint;
         }
         else
         {
            double lat, lon, msl, heading;
            COverlayCOM *pRoute = over_route_leg(point, &lat, &lon, &msl, &heading);
            if (pRoute != NULL)
            {
               *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
               m_hint.set_help_text("Snap to route");
               m_hint.set_tool_tip("Snap To");
               *hint = &m_hint;
               if (m_snap_to_legs||(nFlags &MK_SHIFT))
               {
                  m_plocked_route = pRoute;
               }
               
            }
         }
         if (m_plocked_route)
         {
            m_pviewpoint->move_drag_leg(map, pDC, point,m_plocked_route);
         }
         else
         {
            m_pviewpoint->move_drag(map, pDC, point);
         }
      }
      else if (m_drag_mode == Rotate)
      {
         CFvwUtil *futil = CFvwUtil::get_instance();
         double rotation;
         CString s;
         
         *cursor = AfxGetApp()->LoadCursor(IDC_ROTATE_CURSOR);
         // if we haven't started the drag yet, do so now
         if (!m_in_drag)
         {
            m_pviewpoint->begin_rotate(map,pDC,point);
            m_in_drag = TRUE;
         }
         
         rotation = m_pviewpoint->move_rotate(map, pDC, point);
         s.Format("heading = %3.3f", futil->normalize_angle(rotation));
         m_hint.set_help_text(s);
         m_hint.set_tool_tip(s);
         *hint = &m_hint;
      }
      send_drag_to_all_windows(m_pviewpoint);
   }
}

void Cmov_sym_overlay::drag_object(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint point, UINT nFlags, HCURSOR *cursor, HintText **hint)
{
   // don't do anything at all if we haven't moved and we're not yet 
   // in drag mode
   if (m_drag_start_loc == point && !m_in_drag)
      return;
   
   // see if we have exceeded the drag threshold, if so note that
   if (!m_exceeded_drag_threshold)
   {
      if (!drag_utils::in_drag_threshold(m_drag_start_loc, point))
         m_exceeded_drag_threshold=TRUE;
   }
   
   // set the cursor to the move cursor if we are within the threshold region
   // and have not yet exceeded the drag threshold
   if (!m_exceeded_drag_threshold && drag_utils::in_drag_threshold(m_drag_start_loc, point))
      *cursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
   else
      *cursor = NULL;
   
   // if we haven't started the drag yet, do so now
   if (!m_in_drag)
   {
      m_current_selection->get_icon_image()->
         begin_drag(OVL_get_overlay_manager()->get_view_map(), m_drag_start_loc, 
         (LPARAM)m_current_selection);
      
      m_in_drag = TRUE;
   }
   
   // now, continue the drag from that start location to the current point
   m_current_selection->get_icon_image()->move_drag(OVL_get_overlay_manager()->get_view_map(), point);

   // update the object in skyview to reflect the current location
   if (m_skyview_interface && m_current_selection->get_handle() != -1)
   {
      double lat, lon;
      map->surface_to_geo(point.x, point.y, &lat, &lon);
      m_skyview_interface->SetPosition(m_current_selection->get_handle(), 
         lat, lon, m_current_selection->get_altitude_msl());
   }
   
   // if we are copying set the cursor to the add point cursor
   if (nFlags & MK_CONTROL)
      *cursor = AfxGetApp()->LoadCursor(IDC_ADD_CURSOR);

   // Check to see if the cursor is over an icon that can be snapped to with
   // the current context.  test_snap_to() returns TRUE in this case.
   if (OVL_get_overlay_manager()->test_snap_to(OVL_get_overlay_manager()->get_view_map(), point))
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      m_hint.set_help_text("Snap to point");
      m_hint.set_tool_tip("Snap To");
      *hint = &m_hint;
   }
}


void Cmov_sym_overlay::cancel_drag(IFvMapView* pMapView)
{
   m_bDragging = FALSE;

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

   // if we are dragging symbol
   if (m_pviewpoint)
   {
      if (m_in_drag)
      {
         ASSERT(m_pviewpoint);
         if (m_drag_mode == Move)
            m_pviewpoint->cancel_drag(&mapProjWrapper, &dc);
         if (m_drag_mode == Rotate)
            m_pviewpoint->cancel_rotate(&mapProjWrapper, &dc);
         
         m_in_drag = FALSE;
      }
      POSITION pos = m_window_list.GetHeadPosition();
      send_drop_to_all_windows(m_pviewpoint); 
      m_plocked_route = NULL;
   }
   // otherwise, cancel the drag on the skyview object
   else if (m_current_selection)
   {
      if (m_in_drag)
      {
         m_current_selection->get_icon_image()->cancel_drag(OVL_get_overlay_manager()->get_view_map());
         m_in_drag = FALSE;

         // update object's position in skyview
         if (m_skyview_interface)
            m_skyview_interface->SetPosition(m_current_selection->get_handle(), 
               m_current_selection->get_lat(), m_current_selection->get_lon(), 
               m_current_selection->get_altitude_msl());
      }
   }
}

void Cmov_sym_overlay::drop(IFvMapView *pMapView, CPoint point, UINT flags)
{
   m_bDragging = FALSE;

   // dropping the symbol
   if (m_pviewpoint != NULL)
      drop_symbol(pMapView, point, flags);
   else if (m_current_selection != NULL)
      drop_object(pMapView, point, flags);
}

void Cmov_sym_overlay::drop_symbol(IFvMapView *pMapView, CPoint point, 
                                   UINT flags)
{
   CPoint               end_loc;
   degrees_t      lat;
   degrees_t      lon;

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));
   
   if (m_in_drag)
   {
      //if we haven't moved out of threshold rect, then cancel the drop operation
      if (!m_exceeded_drag_threshold && drag_utils::in_drag_threshold(m_drag_start_loc, point))
      {
         if (m_drag_mode == Move)
            m_pviewpoint->cancel_drag(&mapProjWrapper, &dc);
         if (m_drag_mode == Rotate)
            m_pviewpoint->cancel_rotate(&mapProjWrapper, &dc);
      }
      else
      {
         
         if (m_drag_mode == Move)
         {
            if (m_plocked_route)
            {
               end_loc = m_pviewpoint->end_drag_leg(&mapProjWrapper, &dc, point,m_plocked_route);
               mapProjWrapper.surface_to_geo(end_loc.x, end_loc.y, &lat, &lon);
               m_pviewpoint->set_location( lat,lon);
               m_plocked_route = NULL;
               
            }
            else
            {
               end_loc = m_pviewpoint->end_drag(&mapProjWrapper, &dc, point);
               
               // Check to see if the cursor is over an icon that can be snapped to with
               // the current context.  test_snap_to() returns TRUE in this case.
               if (OVL_get_overlay_manager()->test_snap_to(OVL_get_overlay_manager()->get_view_map(), point))
               {
                  SnapToInfo snapToInfo;
                  
                  if (OVL_get_overlay_manager()->do_snap_to(OVL_get_overlay_manager()->get_view_map(), point, &snapToInfo))
                     m_pviewpoint->set_location(snapToInfo.m_lat, snapToInfo.m_lon);
               }
               else 
               {  
                  mapProjWrapper.surface_to_geo(end_loc.x, end_loc.y, &lat, &lon);
                  m_pviewpoint->set_location( lat,lon);
                  
                  // see if we are on a route leg and snap to it
                  double lat, lon, msl, heading;
                  if (over_route_leg(point, &lat, &lon, &msl, &heading) != NULL )
                  {
                     m_pviewpoint->set_location(lat, lon);
                     m_pviewpoint->set_msl(static_cast<float>(msl));
                     m_pviewpoint->set_heading(heading);
                  }
               }
            }
         }
         if (m_drag_mode == Rotate)
         {
            m_pviewpoint->end_rotate(&mapProjWrapper, &dc, point);
         }

         GetStateIndicators()->update_altitude(m_pviewpoint->get_latitude(), 
            m_pviewpoint->get_longitude(), m_pviewpoint->get_heading(), m_pviewpoint->get_msl());
         
         m_pviewpoint->draw_selected();
         
         send_drop_to_all_windows(m_pviewpoint);
      }
      // turn off drag mode
      m_in_drag = FALSE;
      m_exceeded_drag_threshold=FALSE;
   }
}

void Cmov_sym_overlay::drop_object(IFvMapView *pMapView, CPoint point, UINT flags)
{
   if (m_in_drag)
   {
      map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

      // if we haven't moved out of threshold rect, then cancel the drop operation
      if (drag_utils::in_drag_threshold(m_drag_start_loc, point))
         m_current_selection->get_icon_image()->cancel_drag(OVL_get_overlay_manager()->get_view_map());
      else
      {
         SkyViewObject* object = NULL;

         // end the drag and get the point information LPARAM from begin drag call
         m_current_selection->get_icon_image()->end_drag(OVL_get_overlay_manager()->get_view_map(), point, (LPARAM*)&object);

         // dragging while holding down the CONTROL key will duplicate an existing
         // icon object
         if (object != NULL && (flags & MK_CONTROL) )
         {
            C_drag_item di;
            di.m_item_icon = m_current_selection->get_icon_image();
            receive_new_point(OVL_get_overlay_manager()->get_view_map(), point, flags, &di);
         }
         else
         {
            //if the drag lock is UNLOCKED, of the user chose to ignore it... 
            //(and we're not going to crash because object == NULL)
            if (/*(!get_drag_lock_state() || override_drag_lock_move()) && */object != NULL)
            {
               // invalide the original locations select area
               OVL_get_overlay_manager()->InvalidateOverlay(this);

               d_geo_t geo_location;

               SnapToInfo* snap_to_info = NULL;
               if (OVL_get_overlay_manager()->test_snap_to(OVL_get_overlay_manager()->get_view_map(), point))
               {
                  snap_to_info = new SnapToInfo();
                  memset(snap_to_info, 0, sizeof(snap_to_info));

                  if (OVL_get_overlay_manager()->do_snap_to(OVL_get_overlay_manager()->get_view_map(), point, snap_to_info))
                  {
                     geo_location.lat  = snap_to_info->m_lat;
                     geo_location.lon = snap_to_info->m_lon;
                  }
               }
               else
               {
                  mapProjWrapper.surface_to_geo(point.x, point.y, &geo_location.lat,
                     &geo_location.lon);
               }

               //
               // set the new location of the object
               object->set_position(geo_location);

               // update object's position in skyview
               if (m_skyview_interface)
                  m_skyview_interface->SetPosition(object->get_handle(), 
                  object->get_lat(), object->get_lon(), 
                  object->get_altitude_msl());

               if (m_obj_properties_dlg.m_hWnd)
                  m_obj_properties_dlg.set_focus(object);

               // mark the new location for drawing selected
               object->draw(&mapProjWrapper, &dc, TRUE, FALSE);

               // invalide the rectangle in the new location
               OVL_get_overlay_manager()->InvalidateOverlay(this);

               // throw the object at the end of the list so that it
               // gets drawn on top
               POSITION pos = m_object_lst.Find(object);
               m_object_lst.RemoveAt(pos);
               m_object_lst.AddTail(object);

               // the overlay has been changed, set the modified flag
               set_modified(TRUE);
            }
            else
               // get the overlay manager to cancel this drag
               // operation and also reset the drag flags
               cancel_drag(pMapView);
         }
      }
      
      // turn off drag mode
      m_in_drag  = FALSE;
      m_exceeded_drag_threshold=FALSE;
   }
}

// Performs an object specific "receive" of a dragged icon from DragSel.
// The receive_new_point() member of an overlay is called if and only if
// it is currently the edit-mode overlay.
int Cmov_sym_overlay::receive_new_point(ViewMapProj* map, CPoint point, UINT flags, 
                              C_drag_item* drag_item)
{
   SkyViewObject *object = new SkyViewObject(this);
   
   if (object == NULL)
      return FAILURE;

   // set the object's display icon
   object->set_icon_image(drag_item->m_item_icon);

   // obtain the geographical coordinates for this icon use this to
   // set the location of the object
   d_geo_t geo_location;
   map->surface_to_geo(point.x, point.y, &geo_location.lat, &geo_location.lon);
   object->set_position(geo_location);
   
   // set the default altitude for the object (should be at height + elevation)
   int elevation;
   if (DTD_get_elevation_in_feet(object->get_lat(), object->get_lon(), &elevation) == FV_NO_DATA)
      elevation = 0;

// object->set_altitude_msl((float)METERS_TO_FEET(object->get_height()) + elevation);
   object->set_altitude_msl((float)elevation); // put new objects at 0 AGL (CAB 4/27/01)

   // add this to our chain of SkyViewObjects
   add_object(object);

   object->set_title(drag_item->m_title);

   // add the object to skyview
   if (m_skyview_interface)
   {
      if (drag_item->m_title == "Cube")
         object->set_handle(m_skyview_interface->Add3DShape(3));
      else if (drag_item->m_title == "Sphere")
         object->set_handle(m_skyview_interface->Add3DShape(1));
      else if (drag_item->m_title == "Pyramid")
         object->set_handle(m_skyview_interface->Add3DShape(4));
      else if (drag_item->m_title == "Cylinder")
         object->set_handle(m_skyview_interface->Add3DShape(6));
      else
      {
         CString obj_filename;
         float l,w,h; //length, width ,height
         obj_filename = PRM_get_registry_string("Main","HD_DATA") + 
            "\\objects\\skyview\\" + drag_item->m_title + ".obj";
         object->set_handle(m_skyview_interface->Add3DObject(obj_filename));

         // for objects that we load, set the size to the size in the ".obj" file
         m_skyview_interface->GetObjectSize(object->get_handle(),&l,&w,&h);
         object->set_length(l);
         object->set_width(w);
         object->set_height(h);

      }
      
      update_object_in_skyview(object);
   }

   // if there is currently a selected point then 
   // invalidate it since it is no longer the selected point
   if (m_current_selection)
      OVL_get_overlay_manager()->InvalidateOverlay(this);
   
   // unselect the symbol
   if (m_pviewpoint)
   {
      m_pviewpoint->unselect();
      m_pviewpoint = NULL;
   }

   if (m_link_edit_dlg)
      m_link_edit_dlg->set_focus("SkyView Links", &(object->m_links));

   // set to draw as selected on the next draw cycle
   m_current_selection = object;

   map_projection_utils::CMapProjWrapper mapProjWrapper(map->GetSettableMapProj());
   CClientDC dc(map->get_CView());
   object->draw(&mapProjWrapper, &dc, TRUE, FALSE);
   
   // set editor dialog focus to this new point
   if (m_obj_properties_dlg.m_hWnd)
      m_obj_properties_dlg.set_focus(m_current_selection);

   // the overlay has been changed, set the modified flag
   set_modified(TRUE);

   return SUCCESS;
}

void Cmov_sym_overlay::update_object_in_skyview(SkyViewObject *object)
{
   if (m_skyview_interface)
   {
      // set the position of the object
      m_skyview_interface->SetPosition(object->get_handle(),
         object->get_lat(), object->get_lon(), object->get_altitude_msl());
      
      // set the orientation of the object
      m_skyview_interface->SetOrientation(object->get_handle(), 
         object->get_heading(), object->get_pitch(), object->get_roll());
      
      // set the size of the object (in meters)
      m_skyview_interface->Size3DObject(object->get_handle(), 
         object->get_length(),object->get_width(),  object->get_height());

      // set whether or not the object is solid or wireframe
      m_skyview_interface->SetDisplay(object->get_handle(),
         object->is_solid_not_wireframe());
      
      // set the object's color
      COLORREF color = object->get_color();
      m_skyview_interface->SetColor(object->get_handle(), 
         (float)GetRValue(color) / 255.0f, (float)GetGValue(color) / 255.0f, 
         (float)GetBValue(color) / 255.0f);
   }
}

long Cmov_sym_overlay::AddSymbol(long SymbolType, long SymbolFlags)
{
   ASSERT(GetStateIndicators()->m_symbol == NULL);

   Csym_icon *sym;
   
   sym = new Csym_icon;
   sym->set_id(-1);


   if (m_skyview_interface != NULL)
   {
      m_skyview_interface->set_can_rotate_symbol((SymbolFlags & Cmov_sym_overlay::ROTATE) == Cmov_sym_overlay::ROTATE);
      m_skyview_interface->set_can_move_symbol((SymbolFlags & Cmov_sym_overlay::MOVE) == Cmov_sym_overlay::MOVE);
   }

   GetStateIndicators()->m_symbol = sym;

   GetStateIndicators()->redefine_symbol();

   // initiate the connection back to skyView automation interfaces
   if (!m_skyview_interface)
   {
      m_skyview_interface = new SkyViewInterface;
      m_skyview_interface->init();
   }
   OnConnectionInitialized();

   // let each skyview overlay add its objects to skyview
   Cmov_sym_overlay *overlay = (Cmov_sym_overlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);
   while (overlay)
   {
      overlay->add_object_list_to_skyview();
      overlay = (Cmov_sym_overlay *)OVL_get_overlay_manager()->
         get_next_of_type(overlay, FVWID_Overlay_SkyView);
   }

   return SUCCESS;                                 
}

// add each of the objects in the object list to SkyView via skyview's
// automation interface
void Cmov_sym_overlay::add_object_list_to_skyview()
{
   // if we have not already added the skyview objects to skyview
   // then do so now
   POSITION position = m_object_lst.GetHeadPosition();
   while (position)
   {
      SkyViewObject *object = m_object_lst.GetNext(position);
      
      // an object with a handle of -1 means that the object has not
      // yet been added on the skyView side
      if (object->get_handle() < 0)
      {
         CString title = object->get_title();
         
         if (title == "Cube")
            object->set_handle(m_skyview_interface->Add3DShape(3));
         else if (title == "Sphere")
            object->set_handle(m_skyview_interface->Add3DShape(1));
         else if (title == "Pyramid")
            object->set_handle(m_skyview_interface->Add3DShape(4));
         else if (title == "Cylinder")
            object->set_handle(m_skyview_interface->Add3DShape(6));
         else
         {
            CString obj_filename;
            obj_filename = PRM_get_registry_string("Main","HD_DATA") + 
               "\\objects\\skyview\\" + title + ".obj";
            object->set_handle(m_skyview_interface->Add3DObject(obj_filename));
         }
         
         update_object_in_skyview(object);
      }
   }
}

long Cmov_sym_overlay::RequestNotification(long WindowHdl, long MessageTypes)
{

   notify_wnd *pnw;
   POSITION pos;

   pnw = get_notify_window((HWND)WindowHdl);
   if (pnw) // already listed so change or delete
   {
      if( MessageTypes==NONE)
      {
         pos = m_window_list.Find(pnw);
         m_window_list.RemoveAt(pos); 
         delete pnw;
      }
      else
      {
         pnw->msg_types = MessageTypes;
         pnw->retries_left = 10;
      }
   }
   else
   {
      if (MessageTypes != NONE)
      {
         pnw = new notify_wnd;
         pnw->hwnd = (HWND)WindowHdl;
         pnw->msg_types = MessageTypes;
         pnw->retries_left = 10;
         m_window_list.AddTail(pnw);
      }
   }
   return SUCCESS;                            // right now we handle only one symbol
   
}


long Cmov_sym_overlay::AddSymbolWithOwner(long WindowHdl, long SymbolType, long MessageTypes)
{
   int id;
   id = AddSymbol(SymbolType);
   RequestNotification(WindowHdl,MessageTypes);
   return id;                                 // right now we handle only one symbol

}


long Cmov_sym_overlay::MoveSymbol(double Latitude, double Longitude, double Altitude, double Heading)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CRect start,end,inv;
   Csym_icon *sym=NULL;
   CView *pView;
   ViewMapProj *map;

   if ((Latitude < -180.0) || (Latitude > 180.0))
      return FAILURE;
   if ((Longitude < - 360) || (Longitude > 720))
      return FAILURE;

   sym = GetStateIndicators()->m_symbol;
   if (!sym) 
      return FAILURE;

   if (m_in_drag && (sym == m_pviewpoint))
   { // if we are currently dragging the symbol, ignore the move message
      return SUCCESS; // CAB should we return success failure or somthing else
   }

   pView = UTL_get_active_non_printing_view();
   if (pView == NULL)
   { // we are print previewing so we dont need to invalidate anything;
      return SUCCESS;
   }

   map = UTL_get_current_view_map(pView);
   if (!map)
   {
      ERR_report("Failure getting current map projection.");
      return SUCCESS; // since there isno map proj we just wont invalidate
      
   }

   sym->set_msl((float)Altitude); // no redraw needed here so just change it

   if ((sym->get_latitude() == Latitude) && // these require a redraw so make sure somthing really changed
      (sym->get_longitude() == Longitude) &&
      (sym->get_heading() == Heading) &&
      (sym->get_msl() == (float)Altitude))
      return SUCCESS;

   // calculate the old rectangle (where to invalidate from)
   sym->update_position(map);
   start = sym->get_rect();

   // calculate the new rectangle (where to invalidate to)
   sym->set_location(Latitude,Longitude);
   sym->set_heading(Heading);
   sym->update_position(map); // apply new lat lon
   end = sym->get_rect();
   
   inv = start & end; // check for overlap
   if (inv.IsRectEmpty())
   { // no overlap
      OVL_get_overlay_manager()->invalidate_rect(start, FALSE);
      OVL_get_overlay_manager()->invalidate_rect(end, FALSE);

   }
   else
   {  // invalidate the union of the rect
      inv = start| end;
      OVL_get_overlay_manager()->invalidate_rect(inv, FALSE);
   }

   GetStateIndicators()->update_altitude(Latitude, Longitude, Heading, Altitude);

   return SUCCESS;
}

long Cmov_sym_overlay::DeleteSymbol()
{
   Csym_icon *sym=NULL;

   sym = GetStateIndicators()->m_symbol;
   if (!sym) 
      return FAILURE;

   if (m_pviewpoint == sym)
      m_pviewpoint = NULL;

   delete GetStateIndicators()->m_symbol;
   GetStateIndicators()->m_symbol = NULL;

   destroy_skyview_interface();

   OVL_get_overlay_manager()->invalidate_all();

   // removing the symbol implies the skyview connection was closed.  Reset
   // the object handles to -1 for each of the overlays
   Cmov_sym_overlay *overlay = (Cmov_sym_overlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);
   while (overlay)
   {
      overlay->reset_object_handles();
      overlay = (Cmov_sym_overlay *)OVL_get_overlay_manager()->
         get_next_of_type(overlay, FVWID_Overlay_SkyView);
   }

   return SUCCESS;
}

// static
void Cmov_sym_overlay::destroy_skyview_interface()
{
   if (m_skyview_interface && !m_deleting_skyview_interface)
   {
      m_deleting_skyview_interface = true;

      // stop playback to avoid WM_TIMER messages to be processed while shutting down
      const boolean_t bInPlayback = CMainFrame::GetPlaybackDialog().in_playback();
      if (bInPlayback)
         CMainFrame::GetPlaybackDialog().toggle_state();

      OnConnectionTerminating();

      m_skyview_interface->uninit();
      delete m_skyview_interface;
      m_skyview_interface = NULL;

      m_deleting_skyview_interface = false;

      // turn playback back on if necessary
      if (bInPlayback)
         CMainFrame::GetPlaybackDialog().toggle_state();
   }
}

// reset each of the object handles to -1 in the object list
void Cmov_sym_overlay::reset_object_handles()
{
   POSITION position = m_object_lst.GetHeadPosition();
   while (position)
      m_object_lst.GetNext(position)->set_handle(-1);
}

long Cmov_sym_overlay::set_symbol_position(double Latitude, double Longitude, double Altitude, double Heading)
{
   CView *pView;
   ViewMapProj *map;

   CRect start,end,inv;
   Csym_icon *sym=NULL;

   sym = GetStateIndicators()->m_symbol;
   if (!sym) 
      return FAILURE;

   if (m_in_drag && (sym == m_pviewpoint))
   { // if we are currently dragging the symbol, ignore the move message
      return SUCCESS; // CAB should we return success failure or somthing else
   }

   sym->set_msl((float)Altitude); // no redraw needed here so just chang it
 
   if ((sym->get_latitude() == Latitude) && // these require a redraw so make sure somthing really changed
      (sym->get_longitude() == Longitude) &&
      (sym->get_heading() == Heading))
      return SUCCESS;

   sym->set_location(Latitude,Longitude);
   sym->set_heading(Heading);
   
   start = sym->get_rect();
   pView = UTL_get_active_non_printing_view();
   if (pView == NULL)
   { // we are print previewing so we dont need to invalidate anything;
      return SUCCESS;
   }

   map = UTL_get_current_view_map(pView);
   if (!map)
   {
      ERR_report("Failure getting current map projection.");
      return SUCCESS; // since there isno map proj we just wont invalidate
      
   }

   // invalidate the old position of the symbol before the move
   sym->invalidate();

   // apply new lat lon now done in draw
   sym->set_location(Latitude,Longitude);
   sym->update_position(map); 

   // invalidate where the symbol is now
   sym->invalidate();

   send_drop_to_all_windows(sym);

   return SUCCESS;

}


long Cmov_sym_overlay::set_symbol_position(double Latitude, double Longitude, MapProj *map)
{

   CRect start,end,inv;
   Csym_icon *sym=NULL;

   sym = GetStateIndicators()->m_symbol;
   if (!sym) 
      return FAILURE;

   if (m_in_drag && (sym == m_pviewpoint))
   { // if we are currently dragging the symbol, ignore the move message
      return SUCCESS; // CAB should we return success failure or somthing else
   }

   if ((sym->get_latitude() == Latitude) && // these require a redraw so make sure somthing really changed
      (sym->get_longitude() == Longitude))
      return SUCCESS;

   // invalidate the old position of the symbol before the move
   sym->invalidate();

   // apply new lat lon now done in draw
   sym->set_location(Latitude,Longitude);
   sym->update_position(map); 

   // invalidate where the symbol is now
   sym->invalidate();

   send_drop_to_all_windows(sym);

   return SUCCESS;
}

void Cmov_sym_overlay::get_symbol_info(ViewMapProj *map, C_icon *icon)
{
   Csym_icon *sym = (Csym_icon *)icon;
   
   // popup the info box for this point
   show_symbol_info(sym);

   // set the info box focus
   Cmov_sym_overlay *overlay = (Cmov_sym_overlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);
   if (overlay)
      overlay->set_info_box_focus(sym);
}

void Cmov_sym_overlay::snap_enable(ViewMapProj *map, C_icon *icon)
{
   Csym_icon *sym = (Csym_icon *)icon;
   Cmov_sym_overlay *ovl = (Cmov_sym_overlay *)(sym->get_overlay());
   ovl->set_snap_to_legs(TRUE);
}
void Cmov_sym_overlay::snap_disable(ViewMapProj *map, C_icon *icon)
{
   Csym_icon *sym = (Csym_icon *)icon;
   Cmov_sym_overlay *ovl = (Cmov_sym_overlay *)(sym->get_overlay());
   ovl->set_snap_to_legs(FALSE);

}

void Cmov_sym_overlay::get_object_info(ViewMapProj *map, C_icon *icon)
{
   Cmov_sym_overlay* overlay;

   // get the topmost skyview overlay
   overlay = (Cmov_sym_overlay*) OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);

   if (overlay && icon->is_kind_of("SkyViewObject"))
      overlay->get_object_info_hlpr(map, (SkyViewObject *)icon);
}

void Cmov_sym_overlay::get_object_info_hlpr(ViewMapProj *map, 
                                            SkyViewObject *icon)
{
   // unhilight the current selection
   if (m_current_hilight)
      m_current_hilight->invalidate();

   show_object_info(map, icon);
}

void Cmov_sym_overlay::show_object_info(ViewMapProj *map, SkyViewObject *object)
{
   CString info_text = object->get_info_text();

   // open the dialog box and send it the display string
   m_current_hilight = object;

   map_projection_utils::CMapProjWrapper mapProjWrapper(map->GetSettableMapProj());
   CClientDC dc(map->get_CView());
   object->draw(&mapProjWrapper, &dc, 
      object == m_current_selection && m_bEdit && get_current(), TRUE);

   CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, info_text, "3D Object Information", this);
}

void Cmov_sym_overlay::edit_object_info(ViewMapProj *map, C_icon *icon)
{
   Cmov_sym_overlay* overlay;
   
   // get the topmost skyview overlay
   overlay = (Cmov_sym_overlay*) OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);

   // if is the editor is not up...then create it
   if (!Cmov_sym_overlay::m_obj_properties_dlg.m_hWnd)
   {
      m_obj_properties_dlg.Create(ObjectDlg::IDD);
      m_obj_properties_dlg.ShowWindow(SW_SHOW);
   }

   m_obj_properties_dlg.set_focus(overlay->get_current_selection());
}

void Cmov_sym_overlay::edit_object_links(ViewMapProj *map, C_icon *icon)
{
   // if the Link Editor doesn't exist yet, create it
   if (m_link_edit_dlg == NULL)
      m_link_edit_dlg = new CLinksEditDlg(&save_links);
   
   // if the window isn't created, create it
   if (m_link_edit_dlg->GetHwnd() == NULL)
      m_link_edit_dlg->Create();

   // set the current selection as the link focus
   Cmov_sym_overlay *overlay = (Cmov_sym_overlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);
   if (overlay && overlay->get_current_selection())
      m_link_edit_dlg->set_focus("SkyView Links", 
         &(overlay->get_current_selection()->m_links));
}

void Cmov_sym_overlay::save_links(CString handle, CStringArray &links)
{
   // if dlg not started in info dlg, return
   if (handle.Compare("SkyView Links"))
      return;

   // "Edit Object Links..." only shows up for the current overlay.  So, grab
   // the topmost skyview overlay
   C_overlay *overlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SkyView);
   if (overlay)
      ((Cmov_sym_overlay *)overlay)->save_links_hlpr(handle, links);
}

void Cmov_sym_overlay::save_links_hlpr(CString handle, CStringArray& links)
{
   if (m_current_selection == NULL)
      return;

   // store the links in the current selected object
   int size = links.GetSize();
   m_current_selection->m_links.RemoveAll();
   for (int i=0; i<size; i++)
       m_current_selection->m_links.SetAtGrow(i, links[i]);

   // make the overlay dirty
   set_modified(TRUE);
}

void Cmov_sym_overlay::delete_object(ViewMapProj *map, C_icon *icon)
{
   if (icon && icon->is_kind_of("SkyViewObject"))
   {
      Cmov_sym_overlay* overlay= 
         (Cmov_sym_overlay *)OVL_get_overlay_manager()->
         get_first_of_type(FVWID_Overlay_SkyView);

      if (overlay)
         overlay->delete_point(map, (SkyViewObject *)icon);
   }
}

void Cmov_sym_overlay::show_symbol_info(Csym_icon *sym)
{
   C_overlay *overlay = OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);

   if (sym && overlay)
   {
      const int EDIT_LEN = 300;
      char edit[EDIT_LEN];
      const int LINE_LEN = 80;
      char line[LINE_LEN];
      const int STR_LEN = 41;
      char lat_lon_str[STR_LEN];
      const int DATUM_LEN = 6;
      char datum[DATUM_LEN];
      
      // convert lat-lon to formated string in default format and datum
      GEO_lat_lon_to_string(sym->get_latitude(), sym->get_longitude(), 
         lat_lon_str, STR_LEN);
      
      // get the default datum, so it can be included with the location string
      GEO_get_default_datum(datum, DATUM_LEN);
      
      // put Location string in edit string
      sprintf_s(edit, EDIT_LEN, "Location: (%s)  %s\r\n", datum, lat_lon_str);
      
      // put elevation in edit string if available
      sprintf_s(line, LINE_LEN, "Altitude: %0.0f ft (MSL)\r\n",sym->get_msl());
      strcat_s(edit, EDIT_LEN, line);
      
      // add heading line
      sprintf_s(line, LINE_LEN, "Heading: %03.0f\260 True\r\n", sym->get_heading());
      strcat_s(edit, EDIT_LEN, line);
      
      // display the GPS point info in the info dialog box      
      CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, edit, "Moving Symbol Information", overlay);
      g_remark_dlg_active = TRUE;
   }   
}

void Cmov_sym_overlay::send_drop_to_all_windows(Csym_icon* sym)
{
   notify_wnd *pnw = NULL;
   POSITION pos;
   pos = m_window_list.GetHeadPosition();
   while (pos)
   {
      pnw = m_window_list.GetNext(pos);
      if ((pnw->msg_types & DROP) && (pnw->retries_left > 0))
      {
         if(sym->send_drop_message(pnw->hwnd))
         {
            pnw->retries_left--;
         }
      }
   }
   
   
}
void Cmov_sym_overlay::send_drag_to_all_windows(Csym_icon* sym)
{
    notify_wnd *pnw = NULL;
      POSITION pos = m_window_list.GetHeadPosition();
      while (pos)
      {
         pnw = m_window_list.GetNext(pos);
         if ((pnw->msg_types & DRAG) && (pnw->retries_left > 0))
         {
            if(sym->send_drag_message(pnw->hwnd))
            {
               pnw->retries_left--;
            }
         }
      }
     
}



//////////////////////////////////////////////////////////////////////////////
// Over Route Leg
// Returns a pointer to a route leg if the page is over one
// based on similar code in the print tool overlay
//////////////////////////////////////////////////////////////////////////////
COverlayCOM *Cmov_sym_overlay::over_route_leg(CPoint pt, double* lat, double* lon, double* msl, double* heading) const
{
   MapView *pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pView == NULL)
      return NULL;

   try
   {
      // Get the first route overlay and start looping
      COverlayCOM* pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Route));
      while( pOverlay != NULL )
      {
         IRouteOverlayPtr spRouteOverlay = pOverlay->GetFvOverlay();
         if (spRouteOverlay != NULL)
         {
            const long routeIndex = spRouteOverlay->GetRouteInFocus();

            long bIsRouteCalced;
            if (spRouteOverlay->IsPointOverRouteLeg(routeIndex, pView->GetFvMapView(), pt.x, pt.y, lat, lon, msl, heading, &bIsRouteCalced) == TRUE &&
               bIsRouteCalced)
            {
               return pOverlay;
            }

            // Try and get the next route overlay
            pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->get_next_of_type(pOverlay, FVWID_Overlay_Route));
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Cmov_sym_overlay::over_route_leg failed - %s", (char *)e.Description());
      ERR_report(msg);
   }

   return NULL;
}
    

//*****************************************************************************
//
//  Csym_icon methods
//
//*****************************************************************************


Csym_icon::Csym_icon()
{
   m_id = 0;
   m_latitude = 0.0;
   m_longitude = 0.0;
   m_true_heading = 0.0;
   m_msl = 100.0f;
   m_screen_loc.x = 0;
   m_screen_loc.y = 0;

   m_ship_symbol = new OvlSymbol;

   define_symbol_fighter();

   m_foreground = RGB(255,255,255);
   m_background = RGB(0,0,0);
   OvlPen &pen = m_ship_symbol->get_pen();
   pen.set_foreground_pen(m_foreground, UTIL_LINE_SOLID, 2);
   pen.set_background_pen(m_background);

   m_ship_symbol->set_scale(25.0f);

   in_drag = FALSE;
   in_rotate = FALSE;
   m_rotate_handles = TRUE;
   m_selected = FALSE;
   m_infoselected = FALSE;
}

//*****************************************************************************
//*****************************************************************************

Csym_icon::~Csym_icon()
{
   delete m_ship_symbol;
}


//*****************************************************************************
//*****************************************************************************

boolean_t Csym_icon::is_kind_of(const char *class_name)
{
   if (!strcmp(class_name, "Csym_icon"))
      return TRUE;
   else
      return C_icon::is_kind_of(class_name);
}


//*****************************************************************************
//*****************************************************************************

int Csym_icon::hit_test(CPoint point)
{
   m_rect = m_ship_symbol->get_rect();

   if (rotate_handles_hit(m_rect, point))
   {
      return (int)ROTATE;
   }

   if (m_rect.PtInRect(point))
      return (int)IN_RECT;
   else
      return (int)NONE;
}

struct LocStruc
{
   char     header[11]; // == "FALCONVIEW"
   short    type;       // == 1
   long     ID;        // symbol ID
   double   Lat;
   double   Lon;
   double   Alt;
   double   Heading;
};

int Csym_icon::send_drop_message(HWND notify_wnd)
{
   // code stolen from mouseClickResource.cpp
   if (notify_wnd == NULL)
      return FAILURE;


   LocStruc data; 
   
   strcpy_s(data.header,11,"FALCONVIEW");
   data.type = 1; // mouse click is 0 
   data.ID =m_id;
   data.Lat = m_latitude;
   data.Lon = m_longitude;
   data.Alt = m_msl;
   data.Heading = m_true_heading;


   // typedef struct tagCOPYDATASTRUCT {  // cds 
   //    DWORD dwData; 
   //    DWORD cbData; 
   //    PVOID lpData; 
   // } COPYDATASTRUCT; 
 
   //Members
   //dwData: Specifies up to 32 bits of data to be passed to the receiving application. 
   //cbData: Specifies the size, in bytes, of the data pointed to by the lpData member. 
   //lpData: Pointer to data to be passed to the receiving application. This member can be NULL. 

   COPYDATASTRUCT cds;
   cds.dwData = 0;  
   cds.cbData = sizeof(data);
   cds.lpData = (void*) &data;


   //Note: sending of the message and setting the owner to NULL
   // should probably be atomic

   CWaitCursor wait;

   //send message
   DWORD result;
   BOOL sms = SendMessageTimeout(notify_wnd, WM_COPYDATA, (WPARAM)(HWND)AfxGetMainWnd()->m_hWnd,
      (LPARAM)&cds, SMTO_BLOCK, 5000, &result);  //5 seconds

   if (!sms)
   {
      return FAILURE;
   }

   return SUCCESS;


}

int Csym_icon::send_drag_message(HWND notify_wnd)
{
   // code stolen from mouseClickResource.cpp
   if (notify_wnd == NULL)
      return FAILURE;


   LocStruc data; 


   strcpy_s(data.header,11,"FALCONVIEW");
   data.type = 2; // drop = 1 
   data.ID =m_id;
   data.Lat = m_latitude;
   data.Lon = m_longitude;
   data.Alt = m_drag_msl;
   data.Heading = m_drag_heading;


   // typedef struct tagCOPYDATASTRUCT {  // cds 
   //    DWORD dwData; 
   //    DWORD cbData; 
   //    PVOID lpData; 
   // } COPYDATASTRUCT; 
 
   //Members
   //dwData: Specifies up to 32 bits of data to be passed to the receiving application. 
   //cbData: Specifies the size, in bytes, of the data pointed to by the lpData member. 
   //lpData: Pointer to data to be passed to the receiving application. This member can be NULL. 

   COPYDATASTRUCT cds;
   cds.dwData = 0;  
   cds.cbData = sizeof(data);
   cds.lpData = (void*) &data;


   //Note: sending of the message and setting the owner to NULL
   // should probably be atomic

   CWaitCursor wait;

   //send message
   DWORD result;
   BOOL sms = SendMessageTimeout(notify_wnd, WM_COPYDATA, (WPARAM)(HWND)AfxGetMainWnd()->m_hWnd,
      (LPARAM)&cds, SMTO_BLOCK, 5000, &result);  //5 seconds

   if (!sms)
   {
      return FAILURE;
   }

   return SUCCESS;


}

void Csym_icon::set_symbol(long type)
{
   // remove all primatives, so symbol can be redefined.
   m_ship_symbol->remove_all_primatives();

   // define the symbol based on the type
   switch(type)
   {
   case AIRL: define_symbol_airlift(); break;
   case BOMB: define_symbol_bomber(); break;
   case FIGH: define_symbol_fighter(); break;
   case HELI: define_symbol_helicopter(); break;
   case VIEW: define_symbol_viewport(); break;
   }
}

void Csym_icon::define_symbol_airlift()
{
   m_ship_symbol->set_unit(24.0f);

   // fuselage
   m_ship_symbol->add_line(0,-24,0,24);

   // wing
   m_ship_symbol->add_line(-24,0,24,0);

   // tail
   m_ship_symbol->add_line(-8,24,8,24);
}

void Csym_icon::define_symbol_bomber()
{
   m_ship_symbol->set_unit(24.0f);

   // fuselage
   m_ship_symbol->add_line(0,-24,0,24);

   // left wing
   m_ship_symbol->add_line(0,-4,-24,8);

   // right wing
   m_ship_symbol->add_line(0,-4,24,8);

   // left tail
   m_ship_symbol->add_line(0,21,-9,24);

   //right tail
   m_ship_symbol->add_line(0,21,9,24);
}

void Csym_icon::define_symbol_fighter()
{
   m_ship_symbol->set_unit(40.0f);

   //left side
   m_ship_symbol->add_line(0, -40,  -7,  -5);
   m_ship_symbol->add_line(  -7,  -5, -27,  10);
   m_ship_symbol->add_line( -27,  10, -27,  18);
   m_ship_symbol->add_line( -27,  18,  -7,  18);
   m_ship_symbol->add_line(  -7,  18,  -7,  23);
   m_ship_symbol->add_line(  -7,  23, -16,  31);
   m_ship_symbol->add_line( -16,  31, -16,  36);
   m_ship_symbol->add_line( -16,  36, -14,  38);
   m_ship_symbol->add_line( -14,  38,   0,  38);
   //right side
   m_ship_symbol->add_line(   0, -40,   7,  -5);
   m_ship_symbol->add_line(   7,  -5,  27,  10);
   m_ship_symbol->add_line(  27,  10,  27,  18);
   m_ship_symbol->add_line(  27,  18,   7,  18);
   m_ship_symbol->add_line(   7,  18,   7,  23);
   m_ship_symbol->add_line(   7,  23,  16,  31);
   m_ship_symbol->add_line(  16,  31,  16,  36);
   m_ship_symbol->add_line(  16,  36,  14,  38);
   m_ship_symbol->add_line(  14,  38,   0,  38);
}

void Csym_icon::define_symbol_helicopter()
{
   m_ship_symbol->set_unit(24.0f);

   // fuselage
   m_ship_symbol->add_line(0,-24,0,24);

   // left blade
   m_ship_symbol->add_line(-12,-24,12,0);

   // right blade
   m_ship_symbol->add_line(12,-24,-12,0);

   // tail
   m_ship_symbol->add_line(-4,24,4,24);
}

void Csym_icon::define_symbol_viewport()
{
   m_ship_symbol->set_unit(24.0f);
   
   m_ship_symbol->add_line(-12, -24,   0,  0);
   m_ship_symbol->add_line(0, 0,   12,  -24);
}
//*****************************************************************************
//*****************************************************************************

CString Csym_icon::get_help_text()
{
 
   m_help_text.Format("Heading: %3.3f, Altitude: %3.0f(MSL)",m_true_heading,m_msl);

   return m_help_text;
 }

//*****************************************************************************
//*****************************************************************************


CString Csym_icon::get_tool_tip()
{

   return get_help_text();
}
//*****************************************************************************
//*****************************************************************************

CString Csym_icon::get_alt_string()
{
   CString s;

   s.Format("%3.1f",m_msl);

   return s;
}



//*****************************************************************************
// 
//*****************************************************************************

BOOL Csym_icon::EllipseHandleHit(const CPoint&hdl_loc, const CPoint& test_pt ) const
{
   CRect rect( hdl_loc, hdl_loc );
   rect.InflateRect( (GRA_handle_size() / 2), (GRA_handle_size() / 2) );
   rect.NormalizeRect();
   return (rect.PtInRect(test_pt));
}


//////////////////////////////////////////////////////////////////////////////
// Draw Ellipse Handle
//////////////////////////////////////////////////////////////////////////////
void Csym_icon::DrawEllipseHandle( CDC* pDC, const CPoint& pt ) const
{
   CPen pen( PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHTTEXT) );
   CBrush brush( GetSysColor(COLOR_HIGHLIGHT) );
   
   CPen* ppenOld = (CPen*) pDC->SelectObject(&pen);
   CBrush* pbrushOld = (CBrush*) pDC->SelectObject(&brush);

   CRect rect( pt, pt );
   rect.InflateRect( (GRA_handle_size() / 2), (GRA_handle_size() / 2) );
   rect.NormalizeRect();

   pDC->Ellipse( rect );

   // Restore DC
   if ( ppenOld )
      pDC->SelectObject( ppenOld );
   if ( pbrushOld )
      pDC->SelectObject( pbrushOld );
}


void Csym_icon::draw_rotate_handles( CDC* pDC, const CRect& rect ) const
{

   DrawEllipseHandle(pDC,CPoint((rect.left+rect.right)/2,rect.top));
   DrawEllipseHandle(pDC,CPoint((rect.left+rect.right)/2,rect.bottom));
   DrawEllipseHandle(pDC,CPoint(rect.left,(rect.bottom+rect.top)/2));
   DrawEllipseHandle(pDC,CPoint(rect.right,(rect.bottom+rect.top)/2));
}

BOOL Csym_icon::rotate_handles_hit(const CRect& rect, const CPoint& pt) const
{
   BOOL Hit = FALSE;
   Hit |= EllipseHandleHit(CPoint((rect.left+rect.right)/2,rect.top),pt);
   Hit |= EllipseHandleHit(CPoint((rect.left+rect.right)/2,rect.bottom),pt);
   Hit |= EllipseHandleHit(CPoint(rect.left,(rect.bottom+rect.top)/2),pt);
   Hit |= EllipseHandleHit(CPoint(rect.right,(rect.bottom+rect.top)/2),pt);
   return Hit;
}


void Csym_icon::draw(ActiveMap *map)
{
//   float symbol_rot;

    CDC *pDC = map->get_CDC();
   // get the screen coordinates
   if (!in_drag && !in_rotate)
   {
      m_ship_symbol->prepare_for_redraw(map);

      // draw the hilight rect, if necessary, first so that the symbol gets
      // drawn on top of it
      if (m_infoselected)
      {     
         CRect rect = m_ship_symbol->get_rect();
         GRA_draw_hilight_rect(pDC, rect);

      }
      update_position(map);
      m_ship_symbol->draw(map, pDC);
      m_last_draw_was_XOR = FALSE;
      
      if (m_selected)
      {
         CRect rect = m_ship_symbol->get_rect();
         GRA_draw_select_box(pDC, rect, TRUE);
         if(m_rotate_handles)
         { 
            draw_rotate_handles(pDC,rect);
         }
      }
   }
   else
   {
      if (m_last_draw_was_XOR)
      {
         int old_rop;

         old_rop = pDC->GetROP2();
         pDC->SetROP2(R2_XORPEN);
         m_ship_symbol->draw(map,pDC); 
         pDC->SetROP2(old_rop);
         m_last_draw_was_XOR = TRUE;
      }  ; //TRACE("Dont Draw Symbol->\n");
   }
}

void Csym_icon::update_position(MapProj *map)
{
   int   x, y;
   float symbol_rot;

   CFvwUtil *futil = CFvwUtil::get_instance();

   if (!in_drag && !in_rotate)
   {
      
      futil->geo_to_view_short(map,m_latitude, m_longitude, &x, &y);
      m_screen_loc.x = x;
      m_screen_loc.y = y;
      symbol_rot = (float)m_true_heading;
      
      d_geo_t anchor = {m_latitude, m_longitude};
      m_ship_symbol->set_anchor(anchor);
      m_ship_symbol->set_rotation(futil->normalize_angle(symbol_rot));
      m_ship_symbol->prepare_for_redraw(map);
      
      m_rect = m_ship_symbol->get_rect();
      m_rect.InflateRect(GRA_handle_size(),GRA_handle_size());
      
      if (g_remark_dlg_active)
         Cmov_sym_overlay::show_symbol_info(this);
   }
}

void Csym_icon::invalidate()
{
//   TRACE("Invalidate \n");
   m_rect = m_ship_symbol->get_rect();
   m_rect.InflateRect(GRA_handle_size(),GRA_handle_size());
   OVL_get_overlay_manager()->invalidate_rect(m_rect, FALSE);
}

void Csym_icon::draw_selected()
{
   OvlPen &pen = m_ship_symbol->get_pen();
   pen.set_foreground_pen(RGB(255,255,0), UTIL_LINE_SOLID, 2);

   m_selected=TRUE;
   invalidate();
}

void Csym_icon::unselect()
{
   OvlPen &pen = m_ship_symbol->get_pen();
   pen.set_foreground_pen(m_foreground, UTIL_LINE_SOLID, 2);
   pen.set_background_pen(m_background);

   m_selected=FALSE;
   invalidate();
}

void Csym_icon::draw_infoselected()
{
   m_infoselected=TRUE;
   invalidate();
    
}

void Csym_icon::un_infoselect()
{
   // get the screen coordinates
   //update_position();
   m_infoselected=FALSE;
   invalidate();

}

void Csym_icon::begin_drag(map_projection_utils::CMapProjWrapper* map, CDC *pDC, CPoint pt)
{
   int old_rop;

   if (in_drag)
      return;


   in_drag = TRUE;
   m_drag_loc = m_screen_loc;
   m_drag_offset = m_drag_loc - pt;
   m_drag_heading = m_true_heading;
   m_drag_msl = m_msl;
   m_drag_geoloc.lat = m_latitude;
   m_drag_geoloc.lon = m_longitude;
   m_old_rect = m_ship_symbol->get_rect();
   m_old_rect.InflateRect(GRA_handle_size(),GRA_handle_size());
   OvlPen &pen = m_ship_symbol->get_pen();
   pen.set_foreground_pen(RGB(255,255,255), UTIL_LINE_SOLID, 2);
   pen.set_background_pen(RGB(0,0,0));

   // draw first XOR so the first mov_drag restores the original location
   if (!m_last_draw_was_XOR)
   {
      old_rop = pDC->GetROP2();
      pDC->SetROP2(R2_XORPEN);

      CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());
      m_ship_symbol->draw(&mapProjWrapper, pDC); 
      pDC->SetROP2(old_rop);
      m_last_draw_was_XOR = TRUE;
   }
}

CPoint Csym_icon::end_drag(map_projection_utils::CMapProjWrapper *map, CDC *pDC,CPoint pt)
{
   CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());
   if (m_last_draw_was_XOR)
   {
      int old_rop;
      
      // erase last drag box
      old_rop = pDC->GetROP2();
      pDC->SetROP2(R2_XORPEN);
      m_ship_symbol->draw(&mapProjWrapper, pDC); 
      pDC->SetROP2(old_rop);
      m_last_draw_was_XOR = TRUE;
   }

   // erase original position
   OVL_get_overlay_manager()->invalidate_rect(m_old_rect);

   // set new position
   d_geo_t anchor = { m_latitude, m_longitude };
   m_ship_symbol->set_anchor(anchor);
   m_ship_symbol->prepare_for_redraw(&mapProjWrapper);
   invalidate();

   in_drag = FALSE;
   return (pt+m_drag_offset);
}

CPoint Csym_icon::move_drag(map_projection_utils::CMapProjWrapper* map, CDC *pDC,CPoint pt)
{
   CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());

   if (pt+m_drag_offset == m_drag_loc)
      return m_drag_loc;
   
   int old_rop;

   old_rop = pDC->GetROP2();
   pDC->SetROP2(R2_XORPEN);
   if (m_last_draw_was_XOR)
   {
      m_ship_symbol->draw(&mapProjWrapper, pDC);    // erase last position
   }
   else
   {
      TRACE("===NOT OFFED ===");
   }
   // move it to a new location
   m_drag_loc = pt+m_drag_offset;
   map->surface_to_geo(m_drag_loc.x,m_drag_loc.y,&m_latitude,&m_longitude);
   
   d_geo_t anchor = { m_latitude, m_longitude };
   m_ship_symbol->set_anchor(anchor); 

   m_ship_symbol->draw(&mapProjWrapper, pDC); 
   pDC->SetROP2(old_rop);
   m_last_draw_was_XOR = TRUE;
   return m_drag_loc;
   
}

void Csym_icon::cancel_drag(map_projection_utils::CMapProjWrapper *map, CDC *pDC)
{
   int old_rop;
   CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());

   if (m_last_draw_was_XOR)
   {
      old_rop = pDC->GetROP2();
      pDC->SetROP2(R2_XORPEN);
      m_ship_symbol->draw(&mapProjWrapper, pDC); 
      pDC->SetROP2(old_rop);
   }

   in_drag = FALSE;
   
   m_ship_symbol->set_anchor(m_drag_geoloc);

   OvlPen &pen = m_ship_symbol->get_pen();
   pen.set_foreground_pen(m_foreground, UTIL_LINE_SOLID, 2);
   pen.set_background_pen(m_background);

   m_ship_symbol->draw(&mapProjWrapper, pDC); // draw at original location
   m_last_draw_was_XOR = FALSE;
   map->surface_to_geo(m_screen_loc.x,m_screen_loc.y,&m_latitude,&m_longitude);
}

void Csym_icon::begin_rotate(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt)
{
   int old_rop;
   
   ASSERT(!in_drag);
   in_rotate = TRUE;
   m_drag_loc = m_screen_loc;
   m_drag_offset = pt;
   m_drag_heading = m_true_heading;
   m_drag_geoloc.lat = m_latitude;
   m_drag_geoloc.lon = m_longitude;
   m_old_rect = m_ship_symbol->get_rect();
   m_old_rect.InflateRect(GRA_handle_size(),GRA_handle_size());

   OvlPen &pen = m_ship_symbol->get_pen();
   pen.set_foreground_pen(RGB(255,255,255), UTIL_LINE_SOLID, 2);
   pen.set_background_pen(RGB(0,0,0));

   // draw first XOR so the first mov_drag restores the original location
   if (!m_last_draw_was_XOR)
   {
      old_rop = pDC->GetROP2();
      pDC->SetROP2(R2_XORPEN);
      CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());
      m_ship_symbol->draw(&mapProjWrapper, pDC); 
      pDC->SetROP2(old_rop);
      m_last_draw_was_XOR = TRUE;
   }
}

double Csym_icon::end_rotate(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt)
{
   CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());
   int old_rop;

   // erase last drag box
   if (m_last_draw_was_XOR)
   {
      old_rop = pDC->GetROP2();
      pDC->SetROP2(R2_XORPEN);
      m_ship_symbol->draw(&mapProjWrapper, pDC); 
      pDC->SetROP2(old_rop);
      m_last_draw_was_XOR = TRUE;
   }
   in_rotate=FALSE;

   // erase original
   OVL_get_overlay_manager()->invalidate_rect(m_old_rect);

   m_true_heading += rotation_ang(m_screen_loc,m_drag_offset,pt);
   if (m_true_heading >= 360.0)
      m_true_heading -= 360.0;
   else if (m_true_heading < 0.0)
      m_true_heading += 360.0;

   return m_true_heading;
}

 // rotate til begin pt is on a line from center to pt 
double Csym_icon::move_rotate(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());
   int old_rop;
   double rot,symbol_rot;

   old_rop = pDC->GetROP2();
   pDC->SetROP2(R2_XORPEN);
   if (m_last_draw_was_XOR)
   {
      m_ship_symbol->draw(&mapProjWrapper, pDC);
   }
   else
   {
      TRACE("===NOT OFFED ===\n");
   }


   rot = (float)rotation_ang(m_screen_loc,m_drag_offset,pt); // amount we have moved mouse
   m_drag_heading = rot+m_true_heading;
   // symbol_rot = original rotation + amount we moved + map rotation
   symbol_rot = m_drag_heading;

   d_geo_t anchor = { m_latitude, m_longitude };
   m_ship_symbol->set_anchor(anchor);
   m_ship_symbol->set_rotation(futil->normalize_angle(symbol_rot));
   m_ship_symbol->draw(&mapProjWrapper, pDC);
   pDC->SetROP2(old_rop);
   m_last_draw_was_XOR = TRUE;

   return m_drag_heading;

}

void Csym_icon::cancel_rotate(map_projection_utils::CMapProjWrapper *map, CDC *pDC)
{
   CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());
   CFvwUtil *futil = CFvwUtil::get_instance();
   int old_rop;
   
   old_rop = pDC->GetROP2();
   pDC->SetROP2(R2_XORPEN);
   if (m_last_draw_was_XOR)
      m_ship_symbol->draw(&mapProjWrapper, pDC); 
   pDC->SetROP2(old_rop);
   in_rotate=FALSE;

   invalidate();

   m_ship_symbol->set_anchor(m_drag_geoloc);
   m_ship_symbol->set_rotation(futil->normalize_angle(m_drag_heading));
   m_ship_symbol->draw(&mapProjWrapper, pDC);
   m_last_draw_was_XOR = FALSE;
}

int Csym_icon::get_leg_position_info(d_geo_t geo, COverlayCOM *pRoute, double *clat,double *clon,float *new_alt, double *new_head) const
{
   try
   {
      IRouteOverlayPtr spRouteOverlay = pRoute->GetFvOverlay();
      if (spRouteOverlay != NULL)
      {
         const long routeIndex = spRouteOverlay->GetRouteInFocus();

         double distance;
         long start_tpt_id, end_tpt_id;
         double alt;
         if (spRouteOverlay->ClosestPointAlongRoute(routeIndex, geo.lat, geo.lon, &distance, clat, clon, &alt, new_head, &start_tpt_id,
            &end_tpt_id) == S_OK)
         {
            *new_alt = static_cast<float>(alt);
            return SUCCESS;
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Csym_icon::get_leg_position_info failed - %s", (char *)e.Description());
      ERR_report(msg);
   }

   return FAILURE;
}

CPoint Csym_icon::move_drag_leg(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt, COverlayCOM *pRoute )
{
   CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());
   CFvwUtil *futil = CFvwUtil::get_instance();

   double symbol_rot;
   int old_rop,sx,sy;
   d_geo_t geo;

   old_rop = pDC->GetROP2();
   pDC->SetROP2(R2_XORPEN);
   if (m_last_draw_was_XOR)
   {
      m_ship_symbol->draw(&mapProjWrapper, pDC);    // erase last position

   }
   else
   {
      TRACE("===NOT OFFED ===");
   }

   // move it to a new location on the route leg
   map->surface_to_geo(pt.x+m_drag_offset.x, pt.y+m_drag_offset.y, &geo.lat
      , &geo.lon);

   if(0==get_leg_position_info(geo, pRoute, &m_latitude,&m_longitude,&m_drag_msl,&m_drag_heading))
   {

      futil->geo_to_view_short(&mapProjWrapper,m_latitude,m_longitude,&sx,&sy);
      m_drag_loc.x = sx;
      m_drag_loc.y = sy;
   }
   else
   {
      m_latitude = geo.lat;
      m_longitude = geo.lon;

   }
   symbol_rot = m_drag_heading;

   d_geo_t anchor = { m_latitude, m_longitude };
   m_ship_symbol->set_anchor(anchor);
   m_ship_symbol->set_rotation(futil->normalize_angle(symbol_rot));
   m_ship_symbol->draw(&mapProjWrapper, pDC); 
   pDC->SetROP2(old_rop);
   m_last_draw_was_XOR = TRUE;

   return m_drag_loc;

}

CPoint Csym_icon::end_drag_leg(map_projection_utils::CMapProjWrapper *map, CDC *pDC, CPoint pt, COverlayCOM *pRoute)
{
   CMapProjWrapper mapProjWrapper(map->GetInterfacePtr());
   CFvwUtil *futil = CFvwUtil::get_instance();

   int old_rop,sx,sy;
   d_geo_t geo;
   double lat,lon;

   if (m_last_draw_was_XOR)
   {
      old_rop = pDC->GetROP2();
      pDC->SetROP2(R2_XORPEN);
      m_ship_symbol->draw(&mapProjWrapper, pDC);    // erase last position
      pDC->SetROP2(old_rop);
   }
   else
   {
      TRACE("===NOT OFFED ===");
   }

   // erase original position
   OVL_get_overlay_manager()->invalidate_rect(m_old_rect);

   in_drag = FALSE;
   
   // move it to a new location on the route leg
   map->surface_to_geo(pt.x+m_drag_offset.x, pt.y+m_drag_offset.y, &geo.lat, &geo.lon);
   
   if (0 == get_leg_position_info(geo,pRoute,&lat,&lon,&m_msl,&m_true_heading))
   {
      futil->geo_to_view_short(&mapProjWrapper,lat,lon,&sx,&sy);
      m_screen_loc.x = sx;
      m_screen_loc.y = sy;
   }
   else
   {
      m_screen_loc = m_drag_loc;
      m_true_heading = m_drag_heading;
      m_msl = m_drag_msl;
   }

   return m_screen_loc;
}

// CFVFile Overlay overrides
//
int Cmov_sym_overlay::file_new()
{
   static int file_count = 1;

   // build the instance filename from the template
   CString defaultDir = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_SkyView)->fileTypeDescriptor.defaultDirectory;
   m_fileSpecification.Format("%s\\SkyView%d.svw", defaultDir, file_count++);

   return SUCCESS;
}

int Cmov_sym_overlay::open(const CString &filespec) 
{
   CFile file;
   CFileException ex;
   int byte_count;
   char signature[4];
   int block_size;
   BYTE *block;

   // check to make sure we don't have an empty filespec
   if (filespec.IsEmpty())
   {
      ERR_report("Passed an empty filespec.");
      return FAILURE;
   }

   // make sure the object list is cleared before opening the file
   while (!m_object_lst.IsEmpty())
      delete m_object_lst.RemoveHead();
   
   // try to open a skyview object file for reading
   if (!file.Open(filespec, CFile::modeRead | CFile::shareExclusive, &ex))
   {
      ERR_report_exception("Couldn't open source file", ex);
      return FAILURE;
   }

   // read and validate the header
   char header[6];
   file.Read(header, 6);
   if (strncmp(header, "FVWOBJ", 6) != 0)
      return FAILURE;
   
   // read and validate the file version number
   BYTE major, minor;
   short revision;
   file.Read(&major, 1);
   file.Read(&minor, 1);
   file.Read(&revision, sizeof(short));

   if ((major >> 4)*10 + (major & 0x0F) != OBJ_MAJOR_VERSION || 
      (minor >> 4)*10 + (minor & 0x0F) != OBJ_MINOR_VERSION || 
      revision != OBJ_REVISION)
      return FAILURE;

   do
   {
      // get block signature.  If the number of bytes returned
      // from the Read() function is < 0 then we are probably at
      // the end of the file.  In this case, get out of the loop
      byte_count = file.Read(signature, 4);
      
      if (byte_count == 4)
      {
         // get number of bytes in the current block
         file.Read(&block_size, sizeof(int));
         
         if (block_size > 0)
         {
            // get the block of data 
            block = new BYTE[block_size];
            file.Read(block, block_size);
            
            // perform the appropriate function based on the block's 4 byte signature
            
            // skyview object data
            if (strncmp(signature,"OBJ0",4) == 0)
            {
               Deserialize(block);
            }
            
            delete [] block;
         }
      }
      
   } while (byte_count > 0);

   // run through the list of objects and determine if there are 
   // any .obj referenced that do not exist on this machine.  We will
   // give the user a chance to continue without these objects or to abort the
   // loading
   boolean_t done = FALSE;
   boolean_t objects_were_removed = FALSE;
   while (!done)
   {
      POSITION position = m_object_lst.GetHeadPosition();
      while (position)
      {
         SkyViewObject *object = m_object_lst.GetNext(position);

         CString title = object->get_title();

         // construct a filename from the object's title
         CString obj_filename;
         obj_filename = PRM_get_registry_string("Main","HD_DATA") + 
               "\\objects\\skyview\\" + title  + ".obj";

         // check to see that this object exists
         if (title != "Sphere" && title != "Cube" && title != "Pyramid" &&
            title != "Cylinder" &&
            _access(obj_filename, FIL_EXISTS) != 0)
         {
            // ask the user if he wants to continue reading even
            // though the file does not exist
            CString msg;
            msg.Format("The object file %s could not be found on this system\r\n\r\nWould you like to continue loading without these objects?", 
               object->get_title() + ".obj");
            int result = AfxMessageBox(msg, MB_YESNO);

            // if the user doesn't want to continue then return FAILURE
            if (result == IDNO)
               return FAILURE;
            // otherwise, remove all other occurences of the object and
            // set a flag stating that objects were removed
            else
            {
               POSITION inner_position = m_object_lst.GetHeadPosition();
               while (inner_position)
               {
                  SkyViewObject *remove_obj = m_object_lst.GetAt(inner_position);
                  if (remove_obj->get_title() == title)
                  {
                     delete m_object_lst.GetAt(inner_position);
                     m_object_lst.RemoveAt(inner_position);
                     inner_position = m_object_lst.GetHeadPosition();
                     continue;
                  }

                  m_object_lst.GetNext(inner_position);
               }

               objects_were_removed = TRUE;

               // start over and check if any other objects are missing
               position = m_object_lst.GetHeadPosition();
               continue;
            }
         }
      }

      done = TRUE;
   }

   if (m_skyview_interface)
      add_object_list_to_skyview();

   // update specification
   if (objects_were_removed)
   {
      // build the instance filename from the template
      CString file_name;
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_SkyView);
      file_name.Format("%s\\SkyView%d.svw", pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory, m_file_count++);
      
      m_fileSpecification = file_name;
   }
   else
   {
      m_fileSpecification = filespec;

      // initialize the saved flag to be TRUE
      put_m_bHasBeenSaved(TRUE);
   }
   
   // set modified flag to FALSE since the file was just opened.  If we
   // removed objects from the list, then set this to TRUE
   if (objects_were_removed)
      set_modified(TRUE);
   else
      set_modified(FALSE);

   return SUCCESS;
}

// Inform the overlay that it is about to be saved, so it can perform
// any necessary clean up.  If the overlay returns cancel == TRUE then
// the save is canceled.
int Cmov_sym_overlay::pre_save(boolean_t *cancel)
{
   // if there are no objects, then display a warning and set
   // cancel to TRUE so that the save is aborted
   if (m_object_lst.GetCount() == 0)
   {
      AfxMessageBox("There are no 3D objects to be saved.", MB_OK|MB_ICONINFORMATION); 
      *cancel = TRUE;
   }
   else
      *cancel = FALSE;

   return SUCCESS;
}

int Cmov_sym_overlay::save_as(const CString &filespec, long nSaveFormat) 
{
   CFile file;
   CFileException ex;
   
   // get the number of bytes required to store the object data
   int block_size = get_block_size();
   
   // allocate block
   BYTE *block = new BYTE[block_size];
   BYTE *block_ptr = block;
   
   // get the skyview object data
   if (Serialize(block_ptr) != SUCCESS)
   {
      ERR_report("Unable to serialize skyview data.");
      
      delete [] block;
      return FAILURE;
   }
   
   // try to open a skyview file for writing
   if (!file.Open(filespec, CFile::modeWrite |
      CFile::shareExclusive | CFile::modeCreate, &ex))
   {
      ERR_report_exception("Couldn't open source file", ex);
      delete [] block;  
      return FAILURE;
   }
   
   // write the block of data to disk and close the file
   file.Write(block, block_size);
   file.Close();
   
   // free the allocated memory
   delete [] block;
   
   // set the spec for the overlay
   m_fileSpecification = filespec;
   
   // set modified flag for this overlay
   set_modified(FALSE);

   return SUCCESS; 
}

// get_block_size - returns the number of bytes required to write this
// overlay's data to disk
int Cmov_sym_overlay::get_block_size()
{
   int size = 0;
   
   // size of file header and version info
   size += 6 + 2 + sizeof(short);

   // signature + block size for objects block
   size += 4 + sizeof(int);

   // number of objects
   size += sizeof(int);

   POSITION position = m_object_lst.GetHeadPosition();
   while (position)
      size += m_object_lst.GetNext(position)->get_block_size();

   return size;
}

// serialize this overlays data into the given block
int Cmov_sym_overlay::Serialize(BYTE *&block_ptr)
{
   const int SIGNATURE_LEN = 5;
   char signature[SIGNATURE_LEN];
   const int HEADER_LEN = 7;
   char header[HEADER_LEN];

   // write the file's header
   strcpy_s(header, HEADER_LEN, "FVWOBJ");
   memcpy(block_ptr, header, 6);
   block_ptr += 6;
   
   // write the version info
   BYTE major = ((int)OBJ_MAJOR_VERSION/10 << 4) | 
      (OBJ_MAJOR_VERSION-(10*((int)OBJ_MAJOR_VERSION/10)));
   BYTE minor = ((int)OBJ_MINOR_VERSION/10 << 4) | 
      (OBJ_MINOR_VERSION-(10*((int)OBJ_MINOR_VERSION/10)));
   memcpy(block_ptr, &major, 1);
   block_ptr++;
   memcpy(block_ptr, &minor, 1);
   block_ptr++;
   short revision = OBJ_REVISION;
   memcpy(block_ptr, &revision, sizeof(short));
   block_ptr += sizeof(short);

   // 
   // write the object block
   //

   // write the signature
   strcpy_s(signature, SIGNATURE_LEN, "OBJ0");
   memcpy(block_ptr, signature, 4);
   block_ptr += 4;

   // write the block size
   {
      int block_size = 0;
      
      // number of elements
      block_size += sizeof(int);
      
      POSITION position = m_object_lst.GetHeadPosition();
      while (position)
         block_size += m_object_lst.GetNext(position)->get_block_size();
      
      memcpy(block_ptr, &block_size, sizeof(int));
      block_ptr += sizeof(int);
   }

   // write the number elements
   int num_elements = m_object_lst.GetCount();
   memcpy(block_ptr, &num_elements, sizeof(int));
   block_ptr += sizeof(int);

   // write each of the elements data
   POSITION position = m_object_lst.GetHeadPosition();
   while (position)
      m_object_lst.GetNext(position)->Serialize(block_ptr);

   return SUCCESS;
}

int Cmov_sym_overlay::Deserialize(BYTE *block)
{
   // read the number of elements
   int num_elements;
   memcpy(&num_elements, block, sizeof(int));
   block += sizeof(int);

   // add a new object and read in its data from disk for each element
   for(int i=0; i<num_elements; i++)
   {
      SkyViewObject *object = new SkyViewObject(this);
      object->Deserialize(block);

      // set the objects CIconImage based on its title
      POSITION position = m_object_icon_list.GetHeadPosition();
      while (position)
      {
         CIconImage *image = m_object_icon_list.GetNext(position);
         if (image->get_item_title() == object->get_title())
         {
            object->set_icon_image(image);
            break;
         }
      }

      m_object_lst.AddTail(object);
   }

   return SUCCESS;
}

// get default extension - this extension is the same for
// an entire derived class of CFvOverlayPersistenceImpl objects
const char *Cmov_sym_overlay::get_default_extension() 
{ 
   return "svw"; 
}
