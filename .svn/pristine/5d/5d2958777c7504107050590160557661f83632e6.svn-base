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

/////////////////////////////////////////////////////////////////////////////
// TAMask.cpp : Terrain Avoidance Mask overlay code
//
// overlay methods/functions
//
// 2002/12/03 :: DVL 
//    The m_MinClearance option for the overlay was removed.  The top (red) 
//    contour is to be at the aircraft's altitude.  There is no "threshold"
//    level below the aircraft to set a minimum acceptable clearance (i.e. 
//    25 feet or so).  The yellow line may change in altitude from user to 
//    user, but the red (critical) line will always mean the same thing.
//
//    The (source code) option of setting a minimum clearance threshold
//    below the aircraft remains in case this option is desired in the future
//
// 2005/03/15 :: The clearances are now referred to as m_WarnClearance,
//    m_cautionClearance, and m_OKClearance (new)  The top warning clearance
//    can now have a buffer below the aircraft altitude.  If the bottom 
//    clearance is not set, the entire screen will be colored.  This will 
//    usually be set to designate a particular safe altitude "channel"
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "mapx.h"
#include "..\VerticalViewDisplay.h"
#include "showrmk.h"
#include "..\mapview.h" // for MapView
#include "refresh.h"    // for FVW_is_draw_interrupted()
#include "TAMask.h"
#include "TAMaskStatus.h"
#include "wm_user.h"
#include "..\overlay\viewinv.h"
#include "MaskClipRgn.h"
#include "Common\SafeArray.h"
#include "contour.h"
#include "..\MovingMapOverlay\factory.h"
#include "FctryLst.h"
#include "..\mainfrm.h"
#include "..\PlaybackDialog\viewtime.h"
#include "..\StatusBarManager.h"
#include "ovl_mgr.h"
#include "..\getobjpr.h"

#include "TAMaskParamListener_Interface.h"

//------------------------------------------------------------------------------
//-------------------------------- C_TAMask_point ------------------------------
//------------------------------------------------------------------------------

C_TAMask_point::C_TAMask_point() : m_displayIcon(NULL)
{
}



C_TAMask_point::~C_TAMask_point()
{
}


boolean_t C_TAMask_point::is_kind_of(const char* class_name)
{
   if (!strcmp(class_name, "C_TAMask_point"))
      return TRUE;
   else
      return C_icon::is_kind_of(class_name);
}



const char *C_TAMask_point::get_class_name()
{ 
   return "C_TAMask_point"; 
};




CString C_TAMask_point::get_help_text()
{
   CString temp;
   const int STR_LEN = 41;
   char lat_lon_str[STR_LEN];
   GEO_lat_lon_to_string(m_latitude, m_longitude, lat_lon_str, STR_LEN);

   temp.Format("On-screen peak elevation %.0f ft MSL at (%s)", METERS_TO_FEET(m_Elevation), lat_lon_str);

   return temp;
}


CString C_TAMask_point::get_tool_tip()
{
   CString temp;

   temp.Format("Peak elevation : %.0f ft MSL", METERS_TO_FEET(m_Elevation));

   return temp;
}



boolean_t C_TAMask_point::hit_test(CPoint point)
{
   return TRUE;
}



boolean_t C_TAMask_point::in_view(MapProj* map)
{
   return TRUE;
}


const static int x_offs=11;
const static int y_offs=-12;

void C_TAMask_point::draw(CDC* pDC, int x, int y, boolean_t hilighted, boolean_t draw_labels)
{
   if (!hilighted)
      m_displayIcon->draw_icon(pDC, x+x_offs, y+y_offs, 32);
   else
      m_displayIcon->draw_hilighted(pDC, x+x_offs, y+y_offs, 32);

   CFvwUtil *fvwutl = CFvwUtil::get_instance();
   CPoint cpt[4];

   if (draw_labels)
   {
      CString label;
      label.Format("%.0f ft MSL", METERS_TO_FEET(m_Elevation));

      int text_x_offset = x_offs;
      int font_size = 12;

      if (pDC->IsPrinting())
      {
         double icon_adjust_percentage = (double)PRM_get_registry_int("Printing", 
            "IconAdjustSizePercentage", 0);
         text_x_offset = (int)(text_x_offset + text_x_offset*icon_adjust_percentage/100.0);

         double text_adjust_percentage = (double)PRM_get_registry_int("Printing", 
            "FontAdjustSizePercentage", 0);
         font_size = (int)(font_size + font_size*text_adjust_percentage/100.0);
      }

      // draw the altitude block
      fvwutl->draw_text(pDC, label,
         x + text_x_offset, y,
         UTIL_ANCHOR_CENTER_LEFT, UTIL_FONT_NAME_ARIAL, font_size, UTIL_FONT_BOLD, 
         UTIL_BG_RECT, UTIL_COLOR_BLACK, UTIL_COLOR_MAGENTA, 0, cpt);
   }
}

void C_TAMask_point::invalidate()
{
   if (m_displayIcon)
      m_displayIcon->invalidate(m_x+x_offs, m_y+y_offs, 32); 
}



C_TAMask_ovl *C_TAMask_point::get_overlay() 
{ 
   return dynamic_cast<C_TAMask_ovl *>(m_overlay); 
}














//------------------------------------------------------------------------------
//-------------------------------- C_TAMask_ovl ----------------------------------
//------------------------------------------------------------------------------
CString  C_TAMask_ovl::m_specification = "";
double   C_TAMask_ovl::m_CurrentLat = -999.9;
double   C_TAMask_ovl::m_CurrentLon = -999.9;
int      C_TAMask_ovl::m_CurrentElevation= -32767;

C_TAMask_ovl::C_TAMask_ovl() : 
m_Tiles(NULL), m_RegistrySection("TAMask"), m_ContoursValid(false),
   m_screen_mask(NULL), m_screen_mask_size(0),
   m_dpp_lat_DTED(-9999), m_dpp_lon_DTED(-9999),
   m_current_selection_pt(NULL),
   m_MissingData(false), m_PreferencesUpdated(false), m_StatusDlgShowing(false),
   m_DrawSkipped(false),
   m_altitude_updated(false),
   m_altitude_updated_by(NULL)
{
   m_pMaskClipRgn = new CMaskNullClipRgn();

   m_specification = "TAMask Lines Overlay";
   m_overlay_name = "TAMask Lines Overlay";
   strncpy_s(m_class_name, 20, "C_TAMask_ovl", sizeof(m_class_name));

   m_Unit = 1;

   m_ThinningLevel = 3;

   m_peak_icon = CIconImage::load_images("TAMask\\Pennant.ico");
   m_selection_pt.m_displayIcon = m_peak_icon;

   m_StatusDlg = NULL;

   // Set up the user options
   IXMLPrefMgrPtr PrefMgr;
   PrefMgr.CreateInstance(CLSID_XMLPrefMgr);

   if(PrefMgr != NULL)
   {
      PrefMgr->ReadFromRegistry("Software\\XPlan\\FalconView\\TAMask");

      SetTestAlt((float)PrefMgr->GetValueINT("TestAlt", 2500));
      m_Altitude = m_TestAlt;

      m_Sensitivity = PrefMgr->GetValueINT("Sensitivity", 25);

      m_Source = PrefMgr->GetValueINT("DTEDSource", 1);

      m_ShowWarnLevel = PrefMgr->GetValueBOOL("ShowWarnLevel", VARIANT_TRUE) != VARIANT_FALSE;
      m_ShowCautionLevel = PrefMgr->GetValueBOOL("ShowCautionLevel", VARIANT_TRUE) != VARIANT_FALSE;
      m_ShowOKLevel = PrefMgr->GetValueBOOL("ShowOKLevel", VARIANT_TRUE) != VARIANT_FALSE;
      m_ShowNoDataMask = PrefMgr->GetValueBOOL("ShowNoDataMask", VARIANT_TRUE) != VARIANT_FALSE;

      m_WarnColor = PrefMgr->GetValueINT("WarnColor", RGB(255, 0, 0));
      m_CautionColor = PrefMgr->GetValueINT("CautionColor", RGB(255, 255, 0));
      m_OKColor = PrefMgr->GetValueINT("OKColor", RGB(0, 255, 0));
      m_NoDataColor = PrefMgr->GetValueINT("NoDataColor", RGB(255, 0, 128));

      m_WarnClearance = (float)PrefMgr->GetValueINT("WarnClearance", 100);
      m_CautionClearance = (float)PrefMgr->GetValueINT("CautionClearance", 300);
      m_OKClearance = (float)PrefMgr->GetValueINT("OKClearance", 500);

      m_DrawContours = PrefMgr->GetValueBOOL("DrawContours", VARIANT_FALSE) != VARIANT_FALSE;
      m_DrawMask = PrefMgr->GetValueBOOL("DrawMask", VARIANT_TRUE) != VARIANT_FALSE;
      m_ShowLabels = PrefMgr->GetValueBOOL("DrawLabels", VARIANT_TRUE) != VARIANT_FALSE;

      m_LabelColor = PrefMgr->GetValueINT("LabelColor", RGB(0, 192, 0));
      m_LabelBackgroundColor = PrefMgr->GetValueINT("LabelBackgroundColor", RGB(0,0,0));
      m_LabelFontName = (LPCTSTR)PrefMgr->GetValueBSTR("LabelFontName", "Arial");
      m_LabelSize = PrefMgr->GetValueINT("LabelSize", 10);
      m_LabelAttributes = PrefMgr->GetValueINT("LabelAttributes", 0);
      m_LabelBackground = PrefMgr->GetValueINT("LabelBackground", 0);

      m_DisplayThreshold = MAP_get_scale_from_string((LPCTSTR)PrefMgr->GetValueBSTR("DisplayThreshold", "1:2 M"));
      m_LabelThreshold = MAP_get_scale_from_string((LPCTSTR)PrefMgr->GetValueBSTR("LabelThreshold", "1:500 K"));

      m_Shading = PRM_get_registry_int("TAMask", "Shading", 50);


      // We must have a preference string to use the new status window
      m_StatusDlg = new CTAMaskStatus;
      m_StatusDlg->set_preferences(PrefMgr->GetXMLString());
   }
   else
   {
      CString msg;
      msg.Format("FalconView XMLPrefMgr preferences manager object not registered, defaults preferences used");
      ERR_report(msg);

      m_DisplayThreshold = MAP_get_scale_from_string("1:2 M");
      m_LabelThreshold = MAP_get_scale_from_string("1:250 K");

      m_WarnClearance = 0.0f;
      m_WarnColor = 0x000000FF;

      m_CautionClearance = 500;
      m_CautionColor = 0x0000FFFF;

      m_OKClearance = 500;
      m_OKColor = 0x0000FFFF;

      m_NoDataColor = 0x008000FF;

      m_TestAlt = 2500;
      m_Altitude = m_TestAlt;

      m_DrawContours = TRUE;
      m_DrawMask = TRUE;

      m_Shading = 50;

      m_Source = 1;

      m_LabelColor = UTIL_COLOR_RED;
      m_LabelBackgroundColor = UTIL_COLOR_BLACK;
      m_LabelFontName = UTIL_FONT_NAME_COURIER;
      m_LabelSize = 10;
      m_LabelAttributes = 0;
      m_LabelBackground = 0;
      m_ShowLabels = TRUE;

      m_Sensitivity = 25;
   }

   // KLUDGE
   //
   // these are passed via registry to draw contour line font even 
   // though this is a bad way to do this -- change to a status setting 
   // for contour lines?  Parameters?
   //
   // Kludge is also in each of the accessors below
   PRM_set_registry_int( "TAMask", "LabelColor", m_LabelColor);
   PRM_set_registry_int( "TAMask", "LabelBackgroundColor", m_LabelBackgroundColor);
   PRM_set_registry_string( "TAMask", "LabelFontName", m_LabelFontName);
   PRM_set_registry_int( "TAMask", "LabelSize", m_LabelSize );
   PRM_set_registry_int( "TAMask", "LabelAttributes", m_LabelAttributes);
   PRM_set_registry_int( "TAMask", "LabelBackground", m_LabelBackground);


   // Initialize DTED access for the overlay
   m_spDted.CreateInstance(__uuidof(Dted));
   if (m_spDted == NULL)
   {
      CString msg;
      msg.Format("FalconView DTED data retrieval object not registered, TAMask overlay unusable");
      ERR_report(msg);
   }
   else
   {
      m_spDted->m_bFillMissingDted = TRUE;
      m_spDted->m_bFillPartialDted = TRUE;
   }

   memset(m_bDtedTypesUsed, 0, sizeof(m_bDtedTypesUsed));
}

HRESULT C_TAMask_ovl::InternalInitialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;

   // invalidate the current map so that the backing store is refreshed
   MapView *map_view = (MapView *)UTL_get_active_non_printing_view();
   if (map_view)
      map_view->set_current_map_invalid();

   // signal overlays with the current time so they can setup initial altitudes / clipping if necessary
   // set the current view time of all overlays to the given date time
   static const UINT msg_viewtime_changed = RegisterWindowMessage("FVW_ViewTime_Changed");
   CWnd *wnd = AfxGetMainWnd();
   if (wnd)
      ::PostMessage(wnd->m_hWnd, msg_viewtime_changed, 0, 0);

   return S_OK;
}

// Inform the overlay that it is about to be closed, so it can perform
// any necessary clean up.  If the overlay returns cancel == TRUE then
// the close is canceled.  When this function is called with cancel
// set to NULL the overlay will be closed no matter what.
int C_TAMask_ovl::pre_close(boolean_t *cancel)
{
   if (cancel)
      *cancel = FALSE;

   // the base map is no longer valid now that the shadow
   // overlay has been turned off

   MapView *view = (MapView *)UTL_get_active_non_printing_view();

   if (view)
      view->set_current_map_invalid();

   return SUCCESS;
}


//------------------------------- ~C_TAMask_ovl ----------------------------------
C_TAMask_ovl::~C_TAMask_ovl()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

void C_TAMask_ovl::Finalize()
{
   delete m_pMaskClipRgn;
   m_pMaskClipRgn = NULL;
   delete[] m_screen_mask;
   m_screen_mask = NULL;
   m_screen_mask_size = 0;

   invalidate_selected();

   unload_tiles();

   // clean_up does not discard "only" this icon.  leave it in the static cache
   // which is cleaned on shutdown
   //if (m_peak_icon)
   //   m_peak_icon->clean_up();

   if (m_spDted != NULL)
      m_spDted->Terminate();

   if (m_StatusDlg)
   {
      if (m_StatusDlg->GetSafeHwnd())
         m_StatusDlg->DestroyWindow();

      delete m_StatusDlg;
      m_StatusDlg = NULL;
   }
   CMainFrame::GetPlaybackDialog().Signal();
}

//------------------------------------------------------------------------------
//--------------------------- Member Functions ---------------------------------
//------------------------------------------------------------------------------

//------------------------------ get_class_name --------------------------------
const char* C_TAMask_ovl::get_class_name(void)
{
   return m_class_name;
}

//--------------------------------- get_name -----------------------------------
const CString& C_TAMask_ovl::get_name() const
{
   return m_overlay_name;
}

//-------------------------- get_display_threshold -----------------------------
MapScale C_TAMask_ovl::get_display_threshold()
{
   return m_DisplayThreshold;
}

// add map symbols commands to right-click menu
void C_TAMask_ovl::menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   MapScale CurrentMapScale(map->scale());
   map_projection_utils::CMapProjWrapper mapProjWrapper(map->GetSettableMapProj());

   if (  (CurrentMapScale < get_display_threshold())
      && 
      ! (get_display_threshold()==500000 && map->source().get_string() == "DTED")
      )
   {
      // Overlay is not active, no items to add to the 
      return;
   }

   MapView* map_view = fvw_get_view();
   if ( map_view &&
      map_view->GetMapProjParams().type == GENERAL_PERSPECTIVE_PROJECTION )
   {
      m_CurrentLat = 1e-7 * point.y; m_CurrentLon = 1e-7 * point.x;  // See mouse.cpp - fake_lat/lon
   }
   else
      map->surface_to_geo(point.x, point.y, &m_CurrentLat, &m_CurrentLon);
   int retval = DTD_get_elevation_in_feet(m_CurrentLat, m_CurrentLon, &m_CurrentElevation);

   if (retval==SUCCESS && m_CurrentElevation != MAXLONG)
   {
      CString str;
      str.Format("Set TAMask altitude %d Feet", m_CurrentElevation);

      // We must have elevation data or there is nothing to do
      invalidate_selected();
      m_current_selection_pt = (C_TAMask_point*)hit_test(&mapProjWrapper, point, false);
      if (m_current_selection_pt)
      {
         list.AddTail(new CCOverlayMenuItem("TAMask Peak information", this, &point_info));
      }
      else
      {
         list.AddTail(new CCOverlayMenuItem("TAMask information", this, &point_info));
      }

      // Allow user to set a local altitude
      list.AddTail(new CCOverlayMenuItem(str, this, &SetUserAltitude));

      // Allow a toggle of the status window
      list.AddTail(new CCOverlayMenuItem("Toggle TAMask Status window", this, &ToggleStatusWindow));
   }
}

void C_TAMask_ovl::point_info(C_overlay *static_overlay)
{
   if (static_overlay == NULL)
      return;

   C_TAMask_ovl *thisovl = (C_TAMask_ovl*) static_overlay;

   thisovl->show_info();
}


//-------------------------------- show_info -----------------------------------
void C_TAMask_ovl::show_info()
{
   CString edit_string("Illegal input to Show_info()"), title("Terrain Avoidance overlay");
   CString str;
   const int STR_LEN = 255;
   char temp[STR_LEN];
   bool set = false;

   if (m_current_selection_pt != NULL)
   {
      GEO_lat_lon_to_string(m_current_selection_pt->m_latitude, m_current_selection_pt->m_longitude, temp, STR_LEN);

      int elev = int((m_current_selection_pt->m_Elevation/0.3048) + 0.5);

      title = "Terrain Avoidance Detected Peak information";

      edit_string.Format(
         "Peak On-Screen Altitude of %d Feet MSL located\n"
         "Location\t : %s\n\n",
         elev, temp);

      set = true;
   } 
   else if (m_CurrentLat != -999.9) // check for legal point
   {
      release_focus();

      GEO_lat_lon_to_string(m_CurrentLat, m_CurrentLon, temp, STR_LEN);

      title = "Terrain Avoidance Mask information";

      edit_string.Format(
         "Current Altitude %d Feet MSL\n\n"
         "Mouse Location\t : %s\n"
         "Elevation\t\t : %d Feet MSL\n\n",
         (int)m_Altitude, temp, m_CurrentElevation);

      set = true;
   }

   if (set)
   {
      if (m_ShowWarnLevel)
      {
         str.Format("Warning Clearance %d Feet at %d Feet MSL\n", (int)m_WarnClearance, (int)(m_Altitude - m_WarnClearance));
         edit_string += str;
      }

      if (m_ShowCautionLevel)
      {
         str.Format("Caution Clearance %d Feet at %d Feet MSL\n", (int)m_CautionClearance, (int)(m_Altitude - m_CautionClearance));
         edit_string += str;
      }

      if (m_ShowOKLevel)
      {
         if (m_OKClearance!=-99999.0)
            str.Format("Clear/OK Clearance %d Feet at %d Feet MSL", (int)m_OKClearance, (int)(m_Altitude - m_OKClearance));
         else
            str = "Clear/OK Clearance set to \"Full Mask\"";
         edit_string += str;
      }
   }

   // append the DTED types used to generate the mask
   edit_string += "\n\nThe following DTED Levels were used to generate the Terrain Altitude Mask: ";
   CString strDtedLevels;
   for (int i=0; i<3; i++)
   {
      if (m_bDtedTypesUsed[i])
      {
         if (strDtedLevels.GetLength() > 0)
            strDtedLevels += ", ";

         str.Format("DTED Level %d", i + 1);
         strDtedLevels += str;
      }
   }
   edit_string += strDtedLevels;

   CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, edit_string, title, this);

   return;
}

void C_TAMask_ovl::SetUserAltitude(C_overlay *static_overlay)
{
   if (static_overlay == NULL)
      return;

   C_TAMask_ovl *thisovl = (C_TAMask_ovl*) static_overlay;

   // Called when the display altitude should change.
   thisovl->m_TestAlt = static_cast<float>(m_CurrentElevation);
   thisovl->m_altitude_updated_by = NULL;
   thisovl->SelectNullClipRgn();
   thisovl->InvalidateMap();

   std::for_each(thisovl->m_listeners.begin(), thisovl->m_listeners.end(),
      [thisovl](TAMaskParamListener_Interface* listener)
   {
      listener->AltitudeChanged(thisovl->m_TestAlt);
   });

   // This signals that the altitude feed was lost.
   if (thisovl->m_StatusDlg)
      thisovl->m_StatusDlg->set_altitude((int)m_CurrentElevation);

   // update the TA Mask OptionsString key (XML string) in the registry with
   // the new Test Altitude value...
   IXMLPrefMgrPtr PrefMgr;
   PrefMgr.CreateInstance(CLSID_XMLPrefMgr);
   if(PrefMgr != NULL)
   {
      PrefMgr->ReadFromRegistry("Software\\XPlan\\FalconView\\TAMask");
      PrefMgr->SetValueINT("TestAlt", m_CurrentElevation);
      PrefMgr->WriteToRegistry("Software\\XPlan\\FalconView\\TAMask");
   }

   thisovl->invalidate();
   OVL_get_overlay_manager()->invalidate_all(true);

   // Force the map window to be redrawn since the mask has changed
   MapView *view = (MapView *)UTL_get_active_non_printing_view();
   if (view)
   {
      view->set_current_map_invalid();
      view->Invalidate();
      //view->RedrawWindow();
   }

   thisovl->release_focus();
}



void C_TAMask_ovl::ToggleStatusWindow(C_overlay *static_overlay)
{
   if (static_overlay == NULL)
      return;

   C_TAMask_ovl *thisovl = (C_TAMask_ovl*) static_overlay;
   CTAMaskStatus* pDlg = thisovl->m_StatusDlg;

   if (pDlg != NULL)
   {
      if (pDlg->GetSafeHwnd() && pDlg->IsWindowVisible())
      {
         pDlg->ShowWindow(SW_HIDE);
         thisovl->m_StatusDlgShowing = false;
      }
      else
      {
         // create the dialog if necessary
         if (pDlg->GetSafeHwnd() == NULL)
            pDlg->Create(IDD_TAMASK_STATUS_DISPLAY, AfxGetMainWnd());

         pDlg->ShowWindow(SW_SHOW);
         pDlg->GetParent()->SetFocus();
         thisovl->m_StatusDlgShowing = true;
      }
   }

   thisovl->release_focus();
}

//------------------------------- unload_tiles ----------------------------------
void C_TAMask_ovl::unload_tiles()
{
   clear_tiles(m_Tiles);

   POSITION pos = m_DataCache.GetStartPosition();
   if (pos)
   {
      int key;
      DataItem* pItem;

      while (pos)
      {
         m_DataCache.GetNextAssoc(pos, key, pItem);
         delete pItem;
      }

      m_DataCache.RemoveAll();
   }

   memset(m_bDtedTypesUsed, 0, sizeof(m_bDtedTypesUsed));
}  


//------------------------------- clear_tiles ----------------------------------
int C_TAMask_ovl::clear_tiles(CMapLongToContourTile *&tiles)
{
   if (!tiles) 
      // NULL check
      return SUCCESS;

   POSITION next;
   int tile_id;
   CContourTile *tile;

   next = tiles->GetStartPosition();
   while (next)
   {
      tiles->GetNextAssoc(next, tile_id, tile);
      tiles->RemoveKey(tile_id);

      delete tile;
   }

   delete tiles;
   tiles = NULL;

   return SUCCESS;
}

//--------------------------------- hit_test -----------------------------------
C_icon* C_TAMask_ovl::hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point, bool Verbose)
{
   C_icon* TopItem=NULL;

   if (!m_Tiles)
      return NULL;

   // convert to MapScale from map_scale_utils::MapScale until these classes are merged
   map_scale_utils::MapScale s = map->scale();
   MapScale scale(s.GetScale(), s.GetScaleUnits());

   // if we are above our display threshold, then we do nothing here
   if ((scale >= get_display_threshold())
      || (get_display_threshold()==500000 && map->source() == _bstr_t(L"DTED"))
      )
   {
      d_geo_t  geo_point;

      // convert the CPoint point to a d_geo_t point
      map->vsurface_to_geo(point.x, point.y, &geo_point.lat, &geo_point.lon);
      map->surface_to_geo(point.x, point.y, &geo_point.lat, &geo_point.lon);

      // Iterate to each tile in the list of current tiles
      POSITION pos = m_MaxLocation.GetHeadPosition();
      while (pos)
      {
         bool hit = false;

         d_geo_t loc;

         loc = m_MaxLocation.GetNext(pos);

         // Check for within 1/2 dted pixel or w/in 32 pixels
         double dLat = fabs(geo_point.lat - loc.lat);
         if (dLat * 2 <= m_dpp_lat_DTED)
         {
            // Lat is close, check Lon value too

            double dLon = fabs(geo_point.lon - loc.lon);
            if (dLon * 2 <= m_dpp_lon_DTED)
            {
               hit = true;
            }
         }
         else
         {
            // Lat lon test failed, check the screen coordinate test (w/in size of icon)
            double scr_x, scr_y;

            map->geo_to_surface(loc.lat, loc.lon, &scr_x, &scr_y);
            if (point.x > scr_x-5 && point.x < scr_x + 29 
               && point.y < scr_y+4 && point.y > scr_y - 30)
            {
               hit = true;
            }
         }

         if (hit)
         {
            m_selection_pt.m_latitude = loc.lat;
            m_selection_pt.m_longitude = loc.lon;
            m_selection_pt.m_Elevation = m_MaxElevation;

            TopItem = (C_icon*)&m_selection_pt;
         }
      } // End iterating through the libraries
   }

   return (TopItem);
}


//------------------------------- test_snap_to ---------------------------------
boolean_t C_TAMask_ovl::test_snap_to(ViewMapProj* map, CPoint point)
{
   return false;
}

//-------------------------------- do_snap_to ----------------------------------
boolean_t C_TAMask_ovl::do_snap_to(ViewMapProj* map, CPoint point,
   CList<SnapToInfo*, SnapToInfo*> &snap_to_list)
{
   return false;
}

//---------------------------- get_specification` -------------------------------
const CString& C_TAMask_ovl::get_specification() const
{
   return m_specification;
}

//------------------------------ release_focus ---------------------------------
void C_TAMask_ovl::release_focus()
{
   invalidate_selected();
}

//--------------------------- invalidate_selected ------------------------------
void C_TAMask_ovl::invalidate_selected()
{
   if (m_current_selection_pt)
   {
      m_current_selection_pt->invalidate();
      m_current_selection_pt = NULL;
   }
}

int C_TAMask_ovl::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT /*flags*/)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   C_icon *pIcon = hit_test(&mapProjWrapper, point);
   if (pIcon != NULL)
   {
      m_hintText = *pIcon;
      pMapView->SetTooltipText(_bstr_t(m_hintText.get_tool_tip()));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hintText.get_help_text()));

      return SUCCESS;
   }

   return FAILURE;
}

//--------------------------------- selected -----------------------------------
int C_TAMask_ovl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   int return_value = FAILURE;

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   // get the current point
   C_TAMask_point* const pt = static_cast<C_TAMask_point*>(hit_test(&mapProjWrapper, point));

   if (pt)
   {
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

      // clear the old selection, if there is one
      invalidate_selected();

      // select this point
      m_current_selection_pt = pt;

      // update the overlay's status information
      m_CurrentLat = pt->m_latitude;
      m_CurrentLon = pt->m_longitude;
      m_CurrentElevation = pt->m_Elevation;

      DTD_get_elevation_in_feet(m_CurrentLat, m_CurrentLon, &m_CurrentElevation);

      mapProjWrapper.geo_to_surface(m_current_selection_pt->m_latitude, m_current_selection_pt->m_longitude, 
         &m_current_selection_pt->m_x, &m_current_selection_pt->m_y);
      m_current_selection_pt->draw(&dc, m_current_selection_pt->m_x, m_current_selection_pt->m_y, true, false);

      // show info on the new selection
      show_info();

      return_value = SUCCESS;
   }

   return return_value;
}

//----------------------------------- draw -------------------------------------
int C_TAMask_ovl::draw(ActiveMap* map)
{
   if (m_PreferencesUpdated && m_StatusDlg)
   {
      m_PreferencesUpdated = false;

      IXMLPrefMgrPtr PrefMgr;
      PrefMgr.CreateInstance(CLSID_XMLPrefMgr);

      if(PrefMgr != NULL)
      {
         PrefMgr->SetValueBOOL("ShowWarnLevel", (m_ShowWarnLevel) ? VARIANT_TRUE : VARIANT_FALSE);
         PrefMgr->SetValueBOOL("ShowCautionLevel", (m_ShowCautionLevel) ? VARIANT_TRUE : VARIANT_FALSE);
         PrefMgr->SetValueBOOL("ShowOKLevel", (m_ShowOKLevel) ? VARIANT_TRUE : VARIANT_FALSE);

         PrefMgr->SetValueINT("WarnColor", m_WarnColor);
         PrefMgr->SetValueINT("CautionColor", m_CautionColor);
         PrefMgr->SetValueINT("OKColor", m_OKColor);

         PrefMgr->SetValueINT("WarnClearance", static_cast<long>(m_WarnClearance));
         PrefMgr->SetValueINT("CautionClearance", static_cast<long>(m_CautionClearance));
         PrefMgr->SetValueINT("OKClearance", static_cast<long>(m_OKClearance));

         PrefMgr->SetValueINT("TestAlt", static_cast<long>(m_TestAlt));

         m_StatusDlg->set_preferences(PrefMgr->GetXMLString());
      }
   }
   else
   {
      m_PreferencesUpdated = false;
   }

   // Check if we need to draw
   if (m_DrawSkipped)
      return SUCCESS;

   //
   // If we're not at a scale where we're supposed to be
   // displaying any data, then just return successfully.
   //
   MapScale CurrentMapScale(map->scale());

   if (  (CurrentMapScale < get_display_threshold())
      && 
      ! (get_display_threshold()==500000 && map->source().get_string() == "DTED")
      )
   {
      // Hide the status window if we zoom out past the hide above
      if (m_StatusDlgShowing && m_StatusDlg!=NULL)
      {
         m_StatusDlg->ShowWindow(SW_HIDE);
      }

      // Clear the peak elevation flag(s);
      m_MaxLocation.RemoveAll();

      return SUCCESS;
   }

   // Check to see if the status window needs to be restored, but do not change the focus
   if (m_StatusDlgShowing && m_StatusDlg!=NULL)
   {
      if (map->get_CDC()->IsPrinting())
      {
         if (m_StatusDlg->IsWindowVisible())
            m_StatusDlg->ShowWindow(SW_HIDE);
      }
      else if (!m_StatusDlg->IsWindowVisible())
      {
         m_StatusDlg->ShowWindow(SW_SHOW);
         m_StatusDlg->GetParent()->SetFocus(); 
      }
   }

   int local_showlabels = (m_ShowLabels && 
      ((CurrentMapScale >= m_LabelThreshold) || 
      (m_LabelThreshold==500000 && map->source().get_string() == "DTED"))
      );

   //int zoom = map->requested_zoom_percent();
   //if (zoom == NATIVE_ZOOM_PERCENT) 
   //{
   //   // "native zoom requested"
   //   zoom = 100;
   //}

   if (m_DrawContours)
   {
      bool RegenerateContours = !m_ContoursValid;

      // Draw the TAMask as contour lines
      POSITION next = (m_Tiles) ? m_Tiles->GetStartPosition() : NULL;
      while (next)
      {
         // Check if the user is interacting with the interface (e.g., panning)
         if (FVW_is_draw_interrupted())
         {
            set_valid(true);
            return SUCCESS;
         }

         int tile_id = 0;
         CContourTile *current_tile = NULL;

         m_Tiles->GetNextAssoc(next, tile_id, current_tile);

         //current_tile->prepare_for_draw(map, data_needs_refreshing);

         bool tileContourValid = !RegenerateContours;

         // Generate contour lines if the levels changed or this is a 
         // First time draw for the tile
         if (RegenerateContours || !current_tile->m_ContourLines.m_DataReady)
         {
            tileContourValid = current_tile->m_ContourLines.TraceClearanceContours (
               current_tile->m_pItem->data, 
               current_tile->m_pItem->hPoints, current_tile->m_pItem->vPoints, 
               (m_ShowWarnLevel) ? (m_Altitude - m_WarnClearance)*0.3048f : -99999.0f, 
               (m_ShowCautionLevel) ? (m_Altitude - m_CautionClearance)*0.3048f : -99999.0f, 
               (!m_ShowOKLevel || m_OKClearance==-99999.0) ? -99999.0f : (m_Altitude - m_OKClearance)*0.3048f, 
               current_tile->m_pItem->SW_Lat, current_tile->m_pItem->SW_Lon, 
               current_tile->m_pItem->NE_Lat, 
               current_tile->m_pItem->NE_Lon);
         }

         // Check for User interrupted the creation of the contours
         if(tileContourValid)
         {
            // We do not need to double check the user specification on whether to draw particular
            // contours becuase they simply will not exist in the contour lists if the user
            // did not request them.

            current_tile->draw_TA_contours(map, 100, map->get_CDC(), 
               false, local_showlabels, m_Unit==1,
               m_WarnColor, (int)((m_Altitude - m_WarnClearance)*304.8f), 
               m_CautionColor, (int)((m_Altitude - m_CautionClearance)*304.8f),
               m_OKColor, (int)((m_Altitude - m_OKClearance)*304.8f));
         }
      }
   }

   // Flag the masks as current & correct.  The mask was generated in draw_to_base_map
   // this should be set if we get here when asked to RegenerateContours above, or if 
   // we are not drawing contour lines as the needed work is already done.
   m_ContoursValid = true;



   //-------------------- begin draw for peak marking icons -----------------

   // Draw the peak(s)
   d_geo_t loc;
   int x, y;

#define UNKNOWN_DTED -32767 // -32767 is the flag for unknown DTED elevation
   POSITION pos = m_MaxLocation.GetHeadPosition();
   while (pos)
   {
      loc = m_MaxLocation.GetNext(pos);

      m_selection_pt.m_latitude = loc.lat;
      m_selection_pt.m_longitude = loc.lon;
      m_selection_pt.m_Elevation = m_MaxElevation;

      if (m_MaxElevation > UNKNOWN_DTED) 
      {
      map->geo_to_surface(m_selection_pt.m_latitude, m_selection_pt.m_longitude, &x, &y);
      m_selection_pt.draw(map->get_CDC(), x, y, false, local_showlabels);
   }
   }

   // Draw selected items highlighted.
   if (m_current_selection_pt && m_current_selection_pt->m_Elevation > UNKNOWN_DTED)
   {
      map->geo_to_surface(m_current_selection_pt->m_latitude, m_current_selection_pt->m_longitude, 
         &m_current_selection_pt->m_x, &m_current_selection_pt->m_y);
      m_current_selection_pt->draw(map->get_CDC(), m_current_selection_pt->m_x, m_current_selection_pt->m_y, true, false);
   }

   //-------------------- end draw labels -----------------
   set_valid(true);


   return SUCCESS;
}

int C_TAMask_ovl::DrawToVerticalDisplay(CDC *pDC, CVerticalViewProjector* pProjector)
{
   // DVL :: Too many side effects for now.  Implement later.
   // 
   // Since offscreen draws from automation interfaces recalculate the TAMask, 
   // the flag is wrong, so moves about on the screen during vertical profile display

   return SUCCESS;

   POSITION pos = m_MaxLocation.GetHeadPosition();
   while (pos)
   {
      d_geo_t loc = m_MaxLocation.GetNext(pos);

      m_selection_pt.m_latitude = loc.lat;
      m_selection_pt.m_longitude = loc.lon;
      m_selection_pt.m_Elevation = m_MaxElevation;

      int x, y;
      pProjector->ToSurface(m_selection_pt.m_latitude, m_selection_pt.m_longitude, m_MaxElevation, x, y);
      m_selection_pt.draw(pDC, x, y, FALSE, FALSE);
   }

   return SUCCESS;
}

COLORREF C_TAMask_ovl::ConvertAltitudeToColor(double dAltitudeMeters, double dElevationMeters)
{
   const double dWarnLevel = dAltitudeMeters - FEET_TO_METERS(m_WarnClearance);
   const double dCautionLevel = dAltitudeMeters - FEET_TO_METERS(m_CautionClearance);
   const double dOKLevel = (m_OKClearance==-99999.0) ? -32767 : dAltitudeMeters - FEET_TO_METERS(m_OKClearance);

   if (dElevationMeters >= dWarnLevel)
      return m_WarnColor;

   else if (dElevationMeters >= dCautionLevel)
      return m_CautionColor;

   else if (dElevationMeters >= dOKLevel)
      return m_OKColor;

   return RGB(255, 255, 255);
}

int C_TAMask_ovl::draw_to_base_map(ActiveMap* map)
{
   // Assume we will be needing to draw the contours & vertical view items later
   m_DrawSkipped = false;

   // if the overlay that updated the altitude last was closed, then reset clip region
   if (m_altitude_updated_by && OVL_get_overlay_manager()->get_overlay_handle(m_altitude_updated_by) == -1)
   {
      unload_tiles();
      SelectNullClipRgn();
   }

   MapScale CurrentMapScale(map->scale());
   if (  (CurrentMapScale < get_display_threshold())
      && 
      ! (get_display_threshold()==500000 && map->source().get_string() == "DTED")
      )
   {
      m_DrawSkipped = true;
      return SUCCESS;
   }

   if (MapView::m_scrolling)
   {
      m_DrawSkipped = true;
      return SUCCESS;
   }


   // Get the clip area bounds
   d_geo_t clip_ll, clip_ur;

   HRESULT hr = E_FAIL;

   if (m_pMaskClipRgn != NULL)
   {
      hr = m_pMaskClipRgn->ComputeClippedGeoBounds(map, clip_ll.lat, clip_ll.lon, clip_ur.lat, clip_ur.lon);
   }

   if (FAILED(hr))
   {
      ERR_report("Unable to compute clipped geo-bounds");
      return FAILURE;
   }

   // There is nothing to do if the clipped area is entirely offscreen
   if (hr == S_FALSE)
   {
      m_DrawSkipped = true;
      return SUCCESS;
   }

   // the "tile width" can change on map scale change
   static MapScale LastMapScale = CurrentMapScale;
   if (LastMapScale != CurrentMapScale)// || dynamic_cast<CMaskCircularClipRgn *>(m_pMaskClipRgn) != NULL)
   {
      LastMapScale = CurrentMapScale;
      m_ContoursValid = false;
      unload_tiles( );
   }

   // If there is enough altitude change, catch it
   if (abs(int(m_Altitude - m_TestAlt)) >= m_Sensitivity)
      m_Altitude = m_TestAlt;

   // Remove the old tiles -- contours need to be reloaded.  It is possible that only a display
   // optin has changed, so reloading should be prevented in that case.  Reloading only needs to 
   // be done in the case of a change in the number of contour lines desired (i.e. interval change).
   // Even then, a reduction by a multiple could be handled in draw, or removal of extra lines
   // from the current tile.
   static float old_WarnClearance = m_WarnClearance;
   static float old_CautionClearance = m_CautionClearance;
   static float old_OKClearance = m_OKClearance;
   static float old_Altitude = m_Altitude;
   static bool old_ShowWarnLevel = m_ShowWarnLevel;
   static bool old_ShowCautionLevel = m_ShowCautionLevel;
   static bool old_ShowOKLevel = m_ShowOKLevel;
   static bool old_ShowNoDataMask = m_ShowNoDataMask;
   static int old_DrawMask = m_DrawMask;

   if (  old_WarnClearance != m_WarnClearance
      || old_CautionClearance != m_CautionClearance
      || old_OKClearance != m_OKClearance
      || old_Altitude != m_Altitude
      || old_ShowWarnLevel != m_ShowWarnLevel
      || old_ShowCautionLevel != m_ShowCautionLevel
      || old_ShowOKLevel != m_ShowOKLevel
      || old_ShowNoDataMask != m_ShowNoDataMask
      || old_DrawMask != m_DrawMask)
   {
      // Force re-loading of all tiles
      // unload_tiles();
      m_ContoursValid = false;
      old_WarnClearance = m_WarnClearance;
      old_CautionClearance = m_CautionClearance;
      old_OKClearance = m_OKClearance;
      old_Altitude = m_Altitude;
      old_ShowWarnLevel = m_ShowWarnLevel;
      old_ShowCautionLevel = m_ShowCautionLevel;
      old_ShowOKLevel = m_ShowOKLevel;
      old_ShowNoDataMask = m_ShowNoDataMask;
      old_DrawMask = m_DrawMask;
   }

   // Both don't usually change at once.  But, coming from user option
   // changing from colored DTED if the user changed altitude position
   // (i.e. a temporary testalt), both do change at once.  In a GPS 
   // trail, both don't change,... unless you are using the playback dialog
   //
   // Call to load new data / refresh the list to the screen poaition
   if (load_tiles(map) == FAILURE)
   {
      return FAILURE;
   }


   // No data was found at all, just return
   if (!m_Tiles || m_Tiles->GetCount()<=0)
      return SUCCESS;


   IGraphicsContextPtr GC;
   map->m_interface_ptr->GetGraphicsContext(&GC);

   // Some various return failures that are possible 
   if (map->m_interface_ptr==NULL || !m_Tiles || !GC)
      return SUCCESS;


   // Set up the color indexes to be used
   const unsigned char OKColorIdx = (m_ShowOKLevel) ? 242 : 0;
   const unsigned char CautionColorIdx = (m_ShowCautionLevel) ? 241 : OKColorIdx;
   const unsigned char WarnColorIdx = (m_ShowWarnLevel) ? 240 : CautionColorIdx;
   const unsigned char NoDataColorIdx = (m_ShowNoDataMask) ? 243 : 0;


   POSITION next = NULL;
   CContourTile *current_tile;

   // generate masks as necessary and discover full extents
   // 
   // Note: we only have to generate/update tiles which fall within the clip area

   double latmin=9999, latmax=-9999;
   double lonmin=9999, lonmax=-9999;
   int tile_width, tile_height;

   m_MissingData = false;

   next = m_Tiles->GetStartPosition();
   current_tile = NULL;
   while (next)
   {
      // Check if the user is interacting with the interface (e.g., panning)
      if (FVW_is_draw_interrupted())
      {
         set_valid(true);
         return SUCCESS;
      }

      int tile_id = 0;
      m_Tiles->GetNextAssoc(next, tile_id, current_tile);

      // If we were:
      // - Forced to reload the data tiles
      // - Have no Pixmap (ContourMask) data area
      // - or Are clipping to a bullseye region (which is likely to change every time)
      // ... regenerate the mask and flag contours for regeneration
      if (!m_ContoursValid || current_tile->m_ContourMask == NULL || dynamic_cast<CMaskCircularClipRgn *>(m_pMaskClipRgn) != NULL)
      {
         m_ContoursValid = false;

         delete [] current_tile->m_ContourMask; // ok to delete NULL

         int len = (current_tile->m_pItem->vPoints)*(current_tile->m_pItem->hPoints);

         if (dynamic_cast<CMaskCircularClipRgn *>(m_pMaskClipRgn) != NULL)
         {
            // We need a new data area for the clipped data
            if (current_tile->m_pItem->data == current_tile->m_pItem->src_data)
               current_tile->m_pItem->data = new short[current_tile->m_pItem->hPoints * current_tile->m_pItem->vPoints];

            // clip elevation data to the masking region
            m_pMaskClipRgn->ClipElevationData(current_tile->m_pItem->src_data, current_tile->m_pItem->data, 
               current_tile->m_pItem->hPoints, current_tile->m_pItem->vPoints, 
               current_tile->m_pItem->SW_Lat, current_tile->m_pItem->SW_Lon, 
               m_dpp_lat_DTED, m_dpp_lon_DTED);
         }
         else if (current_tile->m_pItem->data != current_tile->m_pItem->src_data)
         {
            delete [] current_tile->m_pItem->data;
            current_tile->m_pItem->data = current_tile->m_pItem->src_data;
         }

         short *data = current_tile->m_pItem->data;

         current_tile->m_ContourMask = new unsigned char [len];

         // We can now be setting much of the screen normally. Remove this 
         // initialization in favor of specific assignment below
         //memset(current_tile->m_ContourMask, 0, len);

         short WarnLevel = (short)((m_Altitude - m_WarnClearance)*0.3048);
         short CautionLevel = (short)((m_Altitude - m_CautionClearance)*0.3048);
         short OKLevel = (m_OKClearance==-99999.0) ? -32767 : (short)((m_Altitude - m_OKClearance)*0.3048);

         // Generate mask for tile
         int VStart=0, VEnd=current_tile->m_pItem->vPoints;
         int HStart=0, HWidth=current_tile->m_pItem->hPoints;

         current_tile->m_MaxElevation = -32767;
         for(int j=VStart;j<VEnd-1;++j)
         {
            for(int i=HStart;i<HWidth-1;++i)
            {
               long position = j*HWidth + i;

               short elev = data[position];

               // from map rendering engine\common.h
               // IMAGE_CLT_START_INDEX_ALPHA 240
               if (elev == -32767)
               {
                  current_tile->m_ContourMask[position] = NoDataColorIdx;
                  current_tile->m_MissingData = true;
                  continue;
               }

               if (m_DrawMask)
               {
                  if (elev == ELEVATION_DATA_CLIPPED)
                     current_tile->m_ContourMask[position] = 0;
                  else if (elev >= WarnLevel)
                     current_tile->m_ContourMask[position] = WarnColorIdx;
                  else if (elev >= CautionLevel)
                     current_tile->m_ContourMask[position] = CautionColorIdx;
                  else if (elev >= OKLevel)
                     current_tile->m_ContourMask[position] = OKColorIdx;
                  else
                     current_tile->m_ContourMask[position] = 0;
               }
               else
               {
                  current_tile->m_ContourMask[position] = 0;
               }


               // Remove the "multiple equal peak detection" by only adding a single peak
               // Flat areas cause a system near lockup
               //
               // search for the tile's peaks
               if (elev != ELEVATION_DATA_CLIPPED && (current_tile->m_MaxElevation== -32767 
                  //    || elev >= current_tile->m_MaxElevation)
                  || elev > current_tile->m_MaxElevation))
               {
                  //if (elev > current_tile->m_MaxElevation)
                  // Clear the old list of locations
                  current_tile->m_MaxLocation.RemoveAll();

                  current_tile->m_MaxElevation = elev;
                  d_geo_t location;
                  location.lat = ((((double)j)/(VEnd-VStart-1)) 
                     * (current_tile->m_bounds.ur.lat-current_tile->m_bounds.ll.lat) 
                     + (current_tile->m_bounds.ll.lat) );
                  location.lon = ((((double)i)/(HWidth-HStart-1)) 
                     * (current_tile->m_bounds.ur.lon-current_tile->m_bounds.ll.lon) 
                     + (current_tile->m_bounds.ll.lon) );

                  current_tile->m_MaxLocation.AddHead(location);
               }
            }
         }
      }

      if (current_tile->m_bounds.ll.lat < latmin) latmin = current_tile->m_bounds.ll.lat;
      if (current_tile->m_bounds.ur.lat > latmax) latmax = current_tile->m_bounds.ur.lat;
      if (current_tile->m_bounds.ll.lon < lonmin) lonmin = current_tile->m_bounds.ll.lon;
      if (current_tile->m_bounds.ur.lon > lonmax) lonmax = current_tile->m_bounds.ur.lon;

      if (current_tile->m_MissingData)
         m_MissingData = true;
   }

   if (m_DrawMask || m_MissingData)
   {
      // All tiles are the same size, just get the size from the last one.
      // Note :: the 1 pixel overlap in the output mask
      tile_width= current_tile->m_pItem->hPoints-1;
      tile_height = current_tile->m_pItem->vPoints-1;

      // remove the outside "overlap pixel" from the outside edge of the full tile range
      latmax -= m_dpp_lat_DTED;
      lonmax -= m_dpp_lon_DTED;

      // We need the extents of the mask, so expand the DTED post extents by 1/2 dpp
      double half_dted_latpix = m_dpp_lat_DTED/2;
      double half_dted_lonpix = m_dpp_lon_DTED/2;
      lonmin -= half_dted_lonpix;
      latmin -= half_dted_latpix;
      lonmax += half_dted_lonpix;
      latmax += half_dted_latpix;


      // Assemble the various pixmaps into a screen mask
      int tile_ul_x, tile_ul_y, tile_lr_x, tile_lr_y;
      map->geo_to_vsurface_rect(latmin, lonmin, latmax, lonmax, 
         &tile_ul_x, &tile_ul_y, &tile_lr_x, &tile_lr_y);

      const int new_width = tile_lr_x - tile_ul_x + 1;
      const int new_height = tile_lr_y - tile_ul_y + 1;

      int rows = static_cast<int>((latmax-latmin) / 0.2 + 0.5);
      int cols = static_cast<int>((lonmax-lonmin) / 0.2 + 0.5);

      int NeededMaskSize = tile_width*cols * tile_height*rows;
      if (m_screen_mask_size != NeededMaskSize)
      {
         m_screen_mask_size = NeededMaskSize;
         delete[] m_screen_mask;
         m_screen_mask = new unsigned char[m_screen_mask_size];
      }


      // No need to zero the output mask -- full array write
      //memset(screen_mask, 0, tile_width*tile_height*rows*cols);


      // Generate the "full" mask
      //    1 pixel overlap on each tile, + 1 pixel extra on the last tile
      const int fullwidth = cols*tile_width;
      next = (m_Tiles) ? m_Tiles->GetStartPosition() : NULL;
      while (next)
      {
         int tile_id = 0;

         m_Tiles->GetNextAssoc(next, tile_id, current_tile);

         // Look for tiles which do not intersect the desired display area (i.e. when range ring is on)
         // If they dont intersect, they do not need to have their display mask copied to the output area.
         if (!(GEO_intersect_degrees(clip_ll, clip_ur, current_tile->m_bounds.ll, current_tile->m_bounds.ur) ||
            GEO_enclose_degrees(clip_ll, clip_ur, current_tile->m_bounds.ll, current_tile->m_bounds.ur))
            )
         {
            continue;
         }

         int row = static_cast<int>((current_tile->m_bounds.ll.lat - latmin) / 0.2 + 0.5);
         int col = static_cast<int>((current_tile->m_bounds.ll.lon - lonmin) / 0.2 + 0.5);

         // tiles overlap by a pixel, account for it
         int offset = row*tile_height*fullwidth + col*tile_width;

         for(int j=0;j<tile_height;++j)
         {
            int subtile_offset = j*fullwidth + offset;

            // account for (actual) overlap of the tiles
            int tile_offset = j*(tile_width+1);

            for(int i=0;i<tile_width;++i)
            {
               // This "if" allows a peak "area" to be flagged -- testing (remove later)
               //if (*((current_tile->m_pItem->data) + tile_offset + i) >= m_MaxElevation)
               //   // "Peaks" mask 
               //   m_screen_mask[subtile_offset + i] = 242;
               //else
               // Elevation mask
               m_screen_mask[subtile_offset + i] = current_tile->m_ContourMask[tile_offset + i];
            }
         }
      }


      // At this time, all tiles have been assembled into a pixmap which is
      // larger than the screen display area.  Shift the "screen area" contained in the
      // pixmap to the upper left of the pixmap to be passed.  This reduces the 
      // size of the pixmap passed into resize_pixmap which slows when the
      // pixmap needs to be drastically enlarged.
      //
      // The existing pixmap is aligned to a 2/10 degree boundary.  The pixmap
      // drawn to the screen should be aligned to the dted post boundary

      // We have the actual screen boundary, align it to DTED posts
      // This is done by finding the largest dted post rectangle on the 
      // screen. If the posts are over 1/2 DTED pixel from the edge, then
      // we will need an extra post off-screen
      d_geo_t screen_ll = clip_ll;
      d_geo_t screen_ur = clip_ur;

      double testpos;
      testpos = ceil(screen_ll.lon/m_dpp_lon_DTED) * m_dpp_lon_DTED;
      if (testpos-screen_ll.lon > half_dted_lonpix)
         screen_ll.lon = testpos - m_dpp_lon_DTED;
      else
         screen_ll.lon = testpos;

      testpos = ceil(screen_ll.lat/m_dpp_lat_DTED) * m_dpp_lat_DTED;
      if (testpos-screen_ll.lat > half_dted_latpix)
         screen_ll.lat = testpos - m_dpp_lat_DTED;
      else
         screen_ll.lat = testpos;

      testpos = floor(screen_ur.lon/m_dpp_lon_DTED) * m_dpp_lon_DTED;
      if (screen_ur.lon-testpos > half_dted_lonpix)
         screen_ur.lon = testpos + m_dpp_lon_DTED;
      else
         screen_ur.lon = testpos;

      testpos = floor(screen_ur.lat/m_dpp_lat_DTED) * m_dpp_lat_DTED;
      if (screen_ur.lat-testpos > half_dted_latpix)
         screen_ur.lat = testpos + m_dpp_lat_DTED;
      else
         screen_ur.lat = testpos;


      // Now shift to the left and up 1/2 DTED post.  If we are on a small 
      // scale map, this will align the dted square posts (large mask pixels)
      // with the "cursor elevations"
      screen_ll.lon -= half_dted_lonpix;
      screen_ll.lat -= half_dted_latpix;
      screen_ur.lon += half_dted_lonpix;
      screen_ur.lat += half_dted_latpix;



      // get the XY range of the screen
      int scr_ul_x, scr_ul_y, scr_lr_x, scr_lr_y;
      map->geo_to_vsurface_rect(screen_ll.lat, screen_ll.lon, screen_ur.lat, screen_ur.lon, 
         &scr_ul_x, &scr_ul_y, &scr_lr_x, &scr_lr_y);
      int scr_width = scr_lr_x - scr_ul_x + 1;
      int scr_height = scr_lr_y - scr_ul_y + 1;


      // Calculate the size of the destination pixmap (on-screen portion of the total)
      int dst_width = min( (int)(((screen_ur.lon - screen_ll.lon)/m_dpp_lon_DTED)+0.5), cols*tile_width);
      int dst_height= min( (int)(((screen_ur.lat - screen_ll.lat)/m_dpp_lat_DTED)+0.5), rows*tile_height);

      // Calculate the offset of the needed sub-pixmap in the overall tile pixmap
      int x_offset = static_cast<int>(((scr_ul_x - tile_ul_x)/(double)new_width) * (cols*tile_width) + 0.5);
      while (x_offset > cols*tile_width - dst_width)
         dst_width--;
      ASSERT(dst_width > 0);

      int y_offset = static_cast<int>(((tile_lr_y - scr_lr_y)/(double)new_height) * (rows*tile_height) + 0.5);
      while (y_offset > rows*tile_height - dst_height)
         dst_height--;
      ASSERT(dst_height > 0);

      if (dst_height > 0 && dst_width > 0)
      {
         //// just shift the sub-pixmap to the UL of the overall pixmap to save a
         //// memory alloc of another pixmap.  The memory of the larger will be saved
         //// unless the needed size changes
         int dst_offset = 0;
         int src_offset = x_offset + y_offset*fullwidth;
         for (int j=0; j<dst_height; j++)
         {
            memcpy(&m_screen_mask[dst_offset], &m_screen_mask[src_offset], dst_width);
            dst_offset += dst_width;
            src_offset += fullwidth;
         }

         // Palette for TAMask overlay
         unsigned char palette[15] = { 
            GetRValue(m_WarnColor), GetGValue(m_WarnColor),  GetBValue(m_WarnColor),         // Warning Level
            GetRValue(m_CautionColor), GetGValue(m_CautionColor),  GetBValue(m_CautionColor),// Caution clearance Level
            GetRValue(m_OKColor), GetGValue(m_OKColor),  GetBValue(m_OKColor),               // OK Clearance Level
            GetRValue(m_NoDataColor), GetGValue(m_NoDataColor), GetBValue(m_NoDataColor),     // No Data mask
            0, 255, 255       // Peaks mask (use for internal testing)
         };

         // Mask must be currently passed as 1 mask for the screen
         // Note: the first row of the image corresponds to the lowest row on the screen.  This
         // is due to how the bitmap is eventually draw on the screen (via StretchDIBits)
         GC->PutPixmapAlphaBlend(scr_ul_x, scr_ul_y, 
            dst_width, dst_height, 
            scr_width, scr_height,
            m_screen_mask, 4, palette, 128, EQUALARC_VSURFACE);
      }
   }



   // We need an ordered list for the on-screen elevation check.
   // Add the tiles in m_Tiles to a sorted list
   std::vector<CContourTile*> SortedTiles;
   int tile_id;
   next = (m_Tiles) ? m_Tiles->GetStartPosition() : NULL;
   while (next)
   {
      m_Tiles->GetNextAssoc(next, tile_id, current_tile);

      SortedTiles.push_back(current_tile);
   }

   std::sort(SortedTiles.begin(), SortedTiles.end(), CContourTile::compare);

   m_MaxLocation.RemoveAll();

   // Reset the screen rect to the screen's actual boundaries
   // so an elevation is not detected or drawn off-screen.
   if (m_pMaskClipRgn == NULL || 
      m_pMaskClipRgn->ComputeClippedGeoBounds(map, clip_ll.lat, clip_ll.lon, clip_ur.lat, clip_ur.lon) != SUCCESS)
   {
      ERR_report("Unable to compute clipped geo-bounds");
      return FAILURE;
   }
   clip_ur.lat += m_dpp_lat_DTED;
   clip_ur.lon += m_dpp_lon_DTED;
   clip_ll.lat -= m_dpp_lat_DTED;
   clip_ll.lon -= m_dpp_lon_DTED;

   RECT WinRect;
   map->get_CDC()->GetClipBox(&WinRect);

   // See if the highest tile's max_elevation point is on-screen
   // If it is, we are done looking
   m_MaxElevation = -32767;
   for (std::vector<CContourTile*>::iterator it = SortedTiles.begin();
      it != SortedTiles.end(); 
      it++)
   {
      // If m_MaxElevation is higher than the next tile's 
      // Max elevation, we have found the highest elevation
      // on the screen
      CContourTile* current_tile = *it;

      if (m_MaxElevation > current_tile->m_MaxElevation)
      {
         // The previous tile's max was not on screen, but it
         // still had a higher elevation than this tile, done
         break;
      }

      if (current_tile->MaxElevInBounds(clip_ll, clip_ur))
      {
         // Insure that the point is truely in view
         d_geo_t loc = current_tile->m_MaxLocation.GetHead();
         int pt_x, pt_y;
         map->geo_to_surface(loc.lat, loc.lon, &pt_x, &pt_y);
         if (pt_x >= WinRect.left && pt_y>=WinRect.top && pt_x<=WinRect.right && pt_y<=WinRect.bottom)
         {
            // This tile's max is on screen, we're done
            m_MaxElevation = current_tile->m_MaxElevation;

            m_MaxLocation.RemoveAll();

            m_MaxLocation.AddHead(&current_tile->m_MaxLocation);

            break;
         }
      }


      // Search the on-screen part of the current tile 
      // and get the Max elevation in that portion
      d_geo_t ll, ur;
      ll.lat = max(clip_ll.lat, current_tile->m_bounds.ll.lat);
      ll.lon = max(clip_ll.lon, current_tile->m_bounds.ll.lon);
      ur.lat = min(clip_ur.lat, current_tile->m_bounds.ur.lat);
      ur.lon = min(clip_ur.lon, current_tile->m_bounds.ur.lon);

      int Xmin, Xmax, Ymin, Ymax;
      current_tile->GetPixLocation(ll.lat, ll.lon, Xmin, Ymin);
      current_tile->GetPixLocation(ur.lat, ur.lon, Xmax, Ymax);

      short elev;

      for(int x=Xmin; x<Xmax; x++)
      {
         for (int y=Ymin; y<Ymax; y++)
         {
            // If this tile's elevation is higher than the current
            // m_MaxElevation found, reset the MaxElevation to this
            // tile's on screen maximum elevation
            elev = current_tile->GetElevation(x, y);

            d_geo_t loc;

            if (elev > m_MaxElevation)
            {
               current_tile->GetPixGeoLocation(x, y, loc.lat, loc.lon);

               // Insure that the point we retrieved is truely in view.
               int pt_x, pt_y;
               map->geo_to_surface(loc.lat, loc.lon, &pt_x, &pt_y);
               if (pt_x >= WinRect.left && pt_y>=WinRect.top && pt_x<=WinRect.right && pt_y<=WinRect.bottom)
               {
                  m_MaxElevation = elev;

                  if (m_MaxLocation.GetCount() > 0)
                     m_MaxLocation.RemoveAll();

                  m_MaxLocation.AddTail(loc);
               }
               //ASSERT(loc.lat >= screen_ll.lat && loc.lon >= screen_ll.lon && loc.lat <= screen_ur.lat && loc.lon <= screen_ur.lon);
            }
            // DISABLE MULTIPLE EQUAL PEAK MARKING
            //else if (elev == m_MaxElevation)
            //{
            //   current_tile->GetPixGeoLocation(x, y, loc.lat, loc.lon);
            //   m_MaxLocation.AddTail(loc);
            //}
         }
      }
   }
  
   return SUCCESS;
}

//-------------------------------- load_tiles ----------------------------------
int C_TAMask_ovl::load_tiles(ActiveMap* map)
{
   static int last_setting = m_Source;

   if (last_setting != m_Source)
      // User request changed, unload held tiles if needed
   {
      last_setting = m_Source;
      unload_tiles();
   }

   // TODO :: Make a determination here of the best DTED level to load.  

   // What about partial dted from several sources?

   // 3 levels of DTED are currently supported, but trust the caller.
   // (DTED_Seconds != 1.0 && DTED_Seconds != 3.0 && DTED_Seconds != 0.4)
   //    return FAILURE;

   double DTED_Seconds;
   int DTED_Type;
   switch (m_Source)
   {
   default:
   case 0:

   case 1:  DTED_Seconds = 3.0;
      DTED_Type = 1;
      break;

   case 2:  DTED_Seconds = 1.0;
      DTED_Type = 2;
      break;

   case 3:  DTED_Seconds = 0.4;
      DTED_Type = 3;
      break;
   }

   load_tiles(map, DTED_Type, static_cast<float>(DTED_Seconds));

   return SUCCESS;
}

int C_TAMask_ovl::load_tiles(ActiveMap* map, int DTED_Type, float DTED_Seconds, bool Second_Chance/*= false*/)
{
   // Since DTED is now loaded by a get_block function, rather than in sub-tiles as before, 
   // this has two affects on this function.  
   //    1) contour mask/lines are calculated for the entire screen at once
   //    2) caching is lost
   //
   // Therefore, cut the screen into 2/10 degree slices (a DTED tile was previously
   // cut into 10x10 subtiles, or 2/10 degree areas) and use this for caching of the areas.

   // Step 1 :: Determing the sampling rate for the screen

   // Determine the DTED sampling for that position
   double dpp_lat, dpp_lon;
   map->get_vmap_degrees_per_pixel(&dpp_lat, &dpp_lon);

   // get the screen location
   d_geo_t screen_ll, screen_ur;

   // Still call this due to side effects,.. but the cache will still need the full screen area
   //if (m_pMaskClipRgn == NULL ||
   if(m_pMaskClipRgn->ComputeClippedGeoBounds(map, screen_ll.lat, screen_ll.lon, screen_ur.lat, screen_ur.lon) != SUCCESS)
   {
      ERR_report("Unable to compute clipped geo-bounds");
      return FAILURE;
   }

   //if (map->get_vmap_bounds(&screen_ll, &screen_ur) != SUCCESS)
   //{
   //   ERR_report("Failed getting vmap bounds");
   //   return FAILURE;
   //}


   double dpp_lat_DTED = SEC_TO_DEG(DTED_Seconds);
   double dpp_lon_DTED;
   double DTED_Zone_Conversion[5][2] = { {50.0, 1}, {70.0, 2}, {75.0, 3}, {80.0, 4}, {90.0, 6}};
   float screen_center_lat = static_cast<float>(fabs((screen_ur.lat + screen_ll.lat) / 2.0));
   for(int i=0; i<5; ++i)
   {
      if (screen_center_lat <= DTED_Zone_Conversion[i][0])
      {
         dpp_lon_DTED = SEC_TO_DEG(DTED_Seconds) * DTED_Zone_Conversion[i][1];
         break;
      }
   }


   // Get the screen size
   double screen_geo_height = screen_ur.lat - screen_ll.lat;
   double screen_geo_width = screen_ur.lon - screen_ll.lon;
   if (screen_geo_width < 0.0)
      screen_geo_width += 360.0;

   if ((fabs(m_dpp_lat_DTED - dpp_lat_DTED) > 0.0001) || (fabs(m_dpp_lon_DTED - dpp_lon_DTED) > 0.0001))
   {
      unload_tiles();

      m_dpp_lon_DTED = dpp_lon_DTED;
      m_dpp_lat_DTED = dpp_lat_DTED;
   }

   long number_NS_samples = static_cast<long>(0.2/m_dpp_lat_DTED+0.5 + 1);
   long number_EW_samples = static_cast<long>(0.2/m_dpp_lon_DTED+0.5 + 1);


   // TODO :: special case for crossing 180 E/W !!!!


   // Step 2 :: Load the Dted data in tiles as needed
   // Get the DTED elevation data covering the same area as the "current map"
   // but sliced into 2/10 degree subtiles
   //
   // Set the left edge
   double BottomLat = (floor(screen_ll.lat * 5.0))/5.0;
   double LeftLon   = (floor(screen_ll.lon * 5.0))/5.0;

   //
   // Set the right edge, subtract a little to guarantee bounds
   double UpperLat = (ceil(screen_ur.lat * 5.0))/5.0 - 0.00001;
   double RightLon = (ceil(screen_ur.lon * 5.0))/5.0 - 0.00001;

   // load tiles -- traverse field of tiles by ll corners
   for (double lon=LeftLon; lon < RightLon; lon+=0.2)
   {
      for (double lat=BottomLat; lat<UpperLat; lat+=0.2)
      {
         int tile_id = ((int)(lon*10))*10000 + (int)(lat*10);
         DataItem *item=NULL;
         m_DataCache.Lookup(tile_id, item);

         if(!item || item->data==NULL)
         {
            item = new DataItem;

            item->tile_id = tile_id;

            item->NE_Lat = lat + 0.2;
            item->NE_Lon = lon + 0.2;
            item->SW_Lat = lat;
            item->SW_Lon = lon;

            item->hPoints = number_EW_samples;
            item->vPoints = number_NS_samples;


            // Protect agaist off-screen tiles entering the list
            d_geo_t tile_ll = {item->SW_Lat, item->SW_Lon};
            d_geo_t tile_ur = {item->NE_Lat, item->NE_Lon};

            if (GEO_intersect_degrees(screen_ll, screen_ur, tile_ll, tile_ur) ||
               GEO_enclose_degrees(screen_ll, screen_ur, tile_ll, tile_ur))
            {
               load_single_tile(item, DTED_Type);

               m_DataCache.SetAt(item->tile_id, item);
            }
            else 
            {
               delete item;
            }
         }
      }
   }


   // Step 3 :: Clean up the data cache
   //
   // Convert the data to tiles then delete the data items from the cache since the data
   // is no longer needed.  Check for each tile in m_Tiles to make sure we do not 
   // regenerate already existing tiles.  


   // Previously, data would be freed after contour generation.  Keep the data now as 
   // it may be used for both contour and mask generation
   //
   //Leave the m_DataCache entries, just free the
   // allocated memeory for the data block.  If the entry exists, the DLL will not 
   // reload the data either.



   int nTiles = m_DataCache.GetCount();
   POSITION pos;
   int key=0;// was uninitialized, used below!
   CContourTile* tile;

   if (nTiles)
   {
      CMapLongToContourTile* new_tiles = new CMapLongToContourTile;
      // Prevent collisions of tile_ids 
      new_tiles->InitHashTable(137);

      CMapLongToContourTile* old_tiles = m_Tiles;

      pos = m_DataCache.GetStartPosition();
      DataItem* pItem;

      // expand the bounds of the screen to avoid immediately discarding
      // recently read data
      d_geo_t expanded_screen_ll = screen_ll;
      d_geo_t expanded_screen_ur = screen_ur;
      {
         const double delta_lat = 1.0;
         const double delta_lon = 1.0;
         expanded_screen_ll.lat -= delta_lat;
         if (expanded_screen_ll.lat < -90.0)
            expanded_screen_ll.lat = -90.0;

         expanded_screen_ur.lat += delta_lat;
         if (expanded_screen_ur.lat > 90.0)
            expanded_screen_ur.lat = 90.0;

         expanded_screen_ll.lon -= delta_lon;
         if (expanded_screen_ll.lon < -180.0)
            expanded_screen_ll.lon += 360.0;

         expanded_screen_ur.lon += delta_lon;
         if (expanded_screen_ur.lon > 180.0)
            expanded_screen_ur.lon -= 360.0;
      }

      if (pos) do 
      {
         m_DataCache.GetNextAssoc(pos, key, pItem);

         if (old_tiles && old_tiles->Lookup(pItem->tile_id, tile))
         {
            d_geo_t tile_ll={pItem->SW_Lat, pItem->SW_Lon};
            d_geo_t tile_ur={pItem->NE_Lat, pItem->NE_Lon};

            if (GEO_intersect_degrees(screen_ll, screen_ur, tile_ll, tile_ur) ||
               GEO_enclose_degrees(screen_ll, screen_ur, tile_ll, tile_ur))
            {
               // Tile already generated in old list and on screen, move it to new list
               new_tiles->SetAt(pItem->tile_id, tile);
               old_tiles->RemoveKey(pItem->tile_id);
            }
            else if (!GEO_intersect_degrees(expanded_screen_ll, expanded_screen_ur, tile_ll, tile_ur) &&
               !GEO_enclose_degrees(expanded_screen_ll, expanded_screen_ur, tile_ll, tile_ur))
            {
               m_DataCache.RemoveKey(pItem->tile_id);
               delete pItem;
            }
         }
         else if (!new_tiles->Lookup(pItem->tile_id, tile))
         {
            // Create a new tile for the retrieved data
            tile = new CContourTile;
            if (tile)
            {
               tile->set_edge_thinning(m_ThinningLevel);
               tile->m_bounds.ll.lat = pItem->SW_Lat;
               tile->m_bounds.ll.lon = pItem->SW_Lon;
               tile->m_bounds.ur.lat = pItem->NE_Lat;
               tile->m_bounds.ur.lon = pItem->NE_Lon;
               tile->m_pItem = pItem;

               // Added the tile id - Robert
               tile->m_id = pItem->tile_id;


               // Contours or masks will be generated on draw as needed
               // Just hold the data for now


               new_tiles->SetAt(key, tile);
            }
            else 
            {
               // out of memory
               ERR_report("Contour lines Load_tile - Out of memory");
               break;
            }
         }
      } while (pos != NULL);

      clear_tiles(old_tiles);
      m_Tiles = new_tiles;
   }

   // ROUGH CHECK !!!
   // Look for tiles from the other DTED map type.  The tile ID's should be identical.  The 
   // logic above throws away data from sub-tiles that do not intersect the screen.  Data in
   // the cache is deleted and set to null as it is used in the overlay tiles list.  The
   // position entry is left in the data cache with a NULL for the data so that the data
   // will not be reloaded on the next DTED request.
   //
   // DTED1 and DTED2 have the same tile boundaries -- so the same sub-tile ID's.  This should
   // allow for a quick search for "holes" where source data is missing.  This does not do 
   // anything for areas where the DTED data is specified as invalid.
   //

   //if (! Second_Chance)
   //load_tiles(map, (DTED_Seconds == 1.0) ? 3.0f : 1.0f, true);

   return SUCCESS;
}



int C_TAMask_ovl::load_single_tile(DataItem *item, int DTED_Type)
{
   // Load the tile data
   //
   // Note :: an extra post of data is taken to the north and east for a 1 pixel
   //         tile overlap.  This is used by the contour lines, not for the mask.

   if (item->data != NULL || m_spDted==NULL)
      return FAILURE;

   _variant_t block;
   try
   {
      IDted2Ptr spDted2 = m_spDted;
      _variant_t varDtedTypesUsed;
      block = spDted2->GetBlockDTEDEx(item->NE_Lat, item->SW_Lon, item->SW_Lat, item->NE_Lon,
         item->vPoints, item->hPoints, DTED_Type, DTED_ELEVATION_METERS, &varDtedTypesUsed);

      LongSafeArray saDtedTypesUsed(varDtedTypesUsed);
      for (int i=0; i<saDtedTypesUsed.GetNumElements(); i++)
      {
         m_bDtedTypesUsed[ saDtedTypesUsed[i] - 1] = true;
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed retrieving block of DTED data.  Description: %s", e.Description());
      ERR_report(msg);

      return FAILURE;
   }

   item->src_data = new short[item->vPoints*item->hPoints];

   if(item->src_data != NULL)
   {
      // flip the data to match algorithm (algorithm should be changed to reflect how
      // data is coming back from GetBlockDTED)
      long *data;

      SafeArrayAccessData(block.parray, reinterpret_cast<void **>(&data));

      for(unsigned src_j=0, dst_j=item->vPoints-1; src_j < item->vPoints; ++src_j, --dst_j)
      {
         for(unsigned i=0; i < item->hPoints; ++i)
         {
            long elev = data[i*item->vPoints + src_j];
            item->src_data[dst_j*item->hPoints + i] = (short)((elev != MAXLONG) ? elev : -32767);
         }
      }

      SafeArrayUnaccessData(block.parray);
   }

   if (dynamic_cast<CMaskCircularClipRgn *>(m_pMaskClipRgn) != NULL)
   {
      // We need a new data area for the clipped data
      item->data = new short[item->hPoints * item->vPoints];

      // clip elevation data to the masking region
      m_pMaskClipRgn->ClipElevationData(item->src_data, item->data, item->hPoints, item->vPoints, 
         item->SW_Lat, item->SW_Lon, m_dpp_lat_DTED, m_dpp_lon_DTED);
   }
   else
   {
      item->data = item->src_data;
   }

   return (item->data != NULL) ? SUCCESS : FAILURE;
}


void C_TAMask_ovl::InvalidateMap()
{
   // Set the mask to redraw.  It may not actually draw, but draw_to_base_map
   // needs to be called for part of the processing and data load.

   // Force the map window to be redrawn since the mask has changed
   MapView *view = (MapView *)UTL_get_active_non_printing_view();
   if (view)
   {
      // Set the overlay to redraw
      invalidate();
      OVL_get_overlay_manager()->invalidate_all(true);

      view->set_current_map_invalid();
      view->Invalidate();
   }
}

void C_TAMask_ovl::SetAltitude(float altitude, C_overlay* pOverlay) 
{
   if ((altitude != -99999.0f) && (fabs(m_TestAlt - altitude) >= m_Sensitivity))
   {
      // Called when the display altitude should change.
      m_TestAlt = altitude;

      // update the TA Mask OptionsString key (XML string) in the registry with
      // the new Test Altitude value...
      IXMLPrefMgrPtr PrefMgr;
      PrefMgr.CreateInstance(CLSID_XMLPrefMgr);
      if(PrefMgr != NULL)
      {
         PrefMgr->ReadFromRegistry("Software\\XPlan\\FalconView\\TAMask");
         PrefMgr->SetValueINT("TestAlt", static_cast<long>(altitude));
         PrefMgr->WriteToRegistry("Software\\XPlan\\FalconView\\TAMask");
      }

      InvalidateMap();
   }

   // This signals that the altitude feed was lost.
   if (m_StatusDlg)
      m_StatusDlg->set_gps_altitude((int)altitude, pOverlay);

   m_altitude_updated_by = pOverlay;

   // Notify listeners that the altitude has changed
   std::for_each(m_listeners.begin(), m_listeners.end(),
      [this](TAMaskParamListener_Interface* listener)
   {
      listener->AltitudeChanged(m_TestAlt);
   });
}

STDMETHODIMP C_TAMask_ovl::raw_UpdateAltitude(double dCenterLat, 
   double dCenterLon, double dAltitudeFeet, double dTrueHeading, 
   IUnknown * pFvOverlay, struct tamo::BullseyeProperties bullseyeProperties)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   FalconViewOverlayLib::IFvOverlayPtr spFvOverlay = pFvOverlay;
   C_overlay *pOverlay = OVL_get_overlay_manager()->FindOverlay(spFvOverlay);

   CBullseyeProperties properties;
   properties.set_bullseye_on(bullseyeProperties.bBullseyeOn == TRUE);
   properties.set_range_radius(bullseyeProperties.dRangeRadiusMeters);
   properties.set_num_range_rings(bullseyeProperties.nNumRangeRings);
   properties.set_num_radials(bullseyeProperties.nNumRadials);
   properties.set_angle_between_radials(static_cast<int>(bullseyeProperties.dAngleBetweenRadialsDegrees));
   properties.set_offset_enabled(bullseyeProperties.bOffsetEnabled);
   properties.set_relative_azimuth(bullseyeProperties.dRelativeAzimuthDegrees);
   properties.set_clip_tamask(bullseyeProperties.bClipMask);

   UpdateAltitude(dCenterLat, dCenterLon, static_cast<float>(dAltitudeFeet), dTrueHeading, pOverlay, &properties);

   return S_OK;
}

// update the altitude from the given overlay.  Determines if the given overlay is top-most and clips
// the mask based on the given bullseye properties
void C_TAMask_ovl::UpdateAltitude(double dCenterLat, double dCenterLon,
   float altitudeFeet, double dTrueHeading, C_overlay* pOverlay, CBullseyeProperties* pBullseyeProperties)
{
   // The first overlay that attempts to update the altitude since the last
   // draw to base map wins
   bool update_altitude = m_altitude_updated == false;
   if (update_altitude)
   {
      SetAltitude(altitudeFeet, pOverlay);
      m_altitude_updated = true;
   }

   // If the bullseye is enabled for this overlay the terrain altitude mask will
   // be clipped to this geo-circle
   BOOL bBullseyeChanged;
   if ( bBullseyeChanged = pBullseyeProperties != NULL
      && *pBullseyeProperties != m_BullseyePropertiesRef )
      m_BullseyePropertiesRef = *pBullseyeProperties;

   if ( pBullseyeProperties != NULL
      && ( update_altitude || bBullseyeChanged )
      && pBullseyeProperties->get_bullseye_on() 
      && pBullseyeProperties->get_clip_tamask() )
   {
      const double dTotalRangeMeters = pBullseyeProperties->get_range_radius() * 
         pBullseyeProperties->get_num_range_rings();

      // Compute the starting and ending heading based on the current ship's true heading
      // and the current settings of the bullseye
      //
      const int nNumRadials = pBullseyeProperties->get_num_radials();
      const double dAngleBetweenRadials = pBullseyeProperties->get_angle_between_radials();
      const double dRelativeAzimuth = pBullseyeProperties->get_relative_azimuth();

      double dStartHeading, dEndHeading;
      if (OvlBullseye::CalcHeadings(dTrueHeading, nNumRadials, dAngleBetweenRadials,
         dRelativeAzimuth, dStartHeading, dEndHeading) == FAILURE)
      {
         ERR_report("Unable to compute starting and ending headings");

         // fall back to a NULL clip region in this case
         SelectNullClipRgn();
         return;
      }

      double sweepAngle = (nNumRadials - 1) * dAngleBetweenRadials;

      // setup a circular clip region in the terrain altitude mask overlay with the current position, range of the
      // bullseye, and starting and ending heading computed above 
      SelectCircularClipRgn(dCenterLat, dCenterLon,
         dTotalRangeMeters, dStartHeading, dEndHeading, sweepAngle);
   }
   else if ( update_altitude || bBullseyeChanged )
   {
      // The bullseye is turned off.  Setup a null clip region to disable the clipping region
      // and to compute the terrain altitude mask for the entire screen.
      SelectNullClipRgn();
   }
}  // End of UpdateAltitude()

void C_TAMask_ovl::SelectCircularClipRgn(double dCenterLat, double dCenterLon, double dRadiusMeters,
   double dStartHeading, double dEndHeading, double dSweepAngle)
{
   // if we are changing from a non-circular clip region to a circular clip region then the map needs to be invalidated
   CMaskCircularClipRgn *pCircularClipRgn = dynamic_cast<CMaskCircularClipRgn *>(m_pMaskClipRgn);
   if (pCircularClipRgn == NULL)
   {
      delete m_pMaskClipRgn;
      InvalidateMap();

      m_pMaskClipRgn = new CMaskCircularClipRgn(dCenterLat, dCenterLon, dRadiusMeters,
         dStartHeading, dEndHeading, dSweepAngle);
   }
   // otherwise, check to see if the circular clip region is changing radius or sweep angle and invalidate if necessary
   else 
   {
      if (pCircularClipRgn->GetRadiusMeters() != dRadiusMeters ||
         pCircularClipRgn->GetSweepAngle() != dSweepAngle ||
         pCircularClipRgn->DidScreenPositionChange(dCenterLat, dCenterLon) ||
         pCircularClipRgn->DidStartHeadingChange(dStartHeading) )
      {
         InvalidateMap();  
      }

      pCircularClipRgn->Reinitialize(dCenterLat, dCenterLon, dRadiusMeters,
         dStartHeading, dEndHeading, dSweepAngle);
   }
}

void C_TAMask_ovl::SelectNullClipRgn()
{
   // if we are changing from a circular clip region to a non-circular clip region then the tiles have to be re-configured from 
   // having clipped elevation data areas to normal
   if (dynamic_cast<CMaskCircularClipRgn *>(m_pMaskClipRgn) != NULL)
   {
      //unload_tiles();
      InvalidateMap();
   }

   POSITION next = (m_Tiles) ? m_Tiles->GetStartPosition() : NULL;
   CContourTile *current_tile = NULL;

   // reset the DataItem objects to clear the "clipped" data areas
   while (next)
   {
      int tile_id;
      m_Tiles->GetNextAssoc(next, tile_id, current_tile);
      if (current_tile->m_pItem->data != current_tile->m_pItem->src_data)
      {
         delete current_tile->m_pItem->data;

         current_tile->m_pItem->data = current_tile->m_pItem->src_data;
      }
   }

   delete m_pMaskClipRgn;
   m_pMaskClipRgn = new CMaskNullClipRgn();
}

void C_TAMask_ovl::SetLabelColor(int LabelColor)
{
   invalidate();

   m_LabelColor = LabelColor; 

   // KLUDGE
   PRM_set_registry_int( "TAMask", "LabelColor", m_LabelColor);
}

void C_TAMask_ovl::SetLabelBackgroundColor(int LabelBackgroundColor)
{
   invalidate();

   m_LabelBackgroundColor = LabelBackgroundColor; 

   // KLUDGE
   PRM_set_registry_int( "TAMask", "LabelBackgroundColor", m_LabelBackgroundColor);
}

void C_TAMask_ovl::SetLabelFontName(const CString& LabelFontName)
{
   invalidate();

   m_LabelFontName = LabelFontName; 

   // KLUDGE
   PRM_set_registry_string( "TAMask", "LabelFontName", m_LabelFontName);
}

void C_TAMask_ovl::SetLabelSize(int LabelSize)
{
   invalidate();

   m_LabelSize = LabelSize; 

   // KLUDGE
   PRM_set_registry_int( "TAMask", "LabelSize", m_LabelSize );
}

void C_TAMask_ovl::SetLabelAttributes(int LabelAttributes)
{
   invalidate();

   m_LabelAttributes = LabelAttributes; 

   // KLUDGE
   PRM_set_registry_int( "TAMask", "LabelAttributes", m_LabelAttributes);
}

void C_TAMask_ovl::SetLabelBackground(int LabelBackground)
{
   invalidate();

   m_LabelBackground = LabelBackground; 

   // KLUDGE
   PRM_set_registry_int( "TAMask", "LabelBackground", m_LabelBackground);
}

void C_TAMask_ovl::SetReferenceAltitudeBreakpoints(float warning_ft,
   float caution_ft, float ok_ft)
{
   m_WarnClearance = warning_ft;
   m_CautionClearance = caution_ft;
   m_OKClearance = ok_ft;
   InvalidateMap();
   m_PreferencesUpdated=true;

   std::for_each(m_listeners.begin(), m_listeners.end(),
      [warning_ft, caution_ft, ok_ft](TAMaskParamListener_Interface* listener)
   {
      listener->ReferenceAltitudeBreakpointsChanged(warning_ft, caution_ft,
         ok_ft);
   });
}