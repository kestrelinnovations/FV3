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



/*------------------------------------------------------------------
-  FILE NAME:    cov_ovl.cpp
-  LIBRARY NAME: catalog.lib
-
-  DESCRIPTION:
-
-      This file contains definitions of the CoverageOverlay class.
-
-  REVISION HISTORY:
-
-
-       $Log: cov_ovl.cpp $
//
//
// This overlay is being adapted to use the Map Data Server
//  - 10 May, 2004  Joel Odom
//
//
//Revision 1.4  1996/03/26  10:46:08  vinny
//Moved the get_current_type_target_disk_usage() member function from the
//CoverageOverlay class to the CATDataSourceMgr class.  This is so all of the
//data in map_db.cpp is available even when there is no coverage overlay.
//CAT_get_current_disk_usage() was changed to use the CATDataSourceMgr
//member function.
//
//Revision 1.3  1996/03/12  15:56:02  kevin
//fixed bug in CAT_destroy_coverage_overlay().  Previously
//did not set overlay=NULL.
//
//Revision 1.2  1996/03/11  16:15:09  vinny
//Changed find_duplicate_above and find_duplicate_below so they test the
//last icon (top or bottom respectively) in the list.  Also added check to
//handle the case when the given position is in the top data source in
//a call to find_duplicate_above or when the given position is in the bottom
//data source in a call to find_duplicate_below.
//
//Revision 1.1  1996/03/05  18:02:17  vinny
//Initial revision
//
/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "stdafx.h" 
#include "cov_ovl.h"
#include "refresh.h"
#include "param.h"
#include "MBString.h"
#include "..\MdsUtilities\MdsUtilities.h"
#include "OvlFctry.h"
#include "factory.h"
#include "ovlelem.h"  // for GeoBoundsDragger
#include "file.h"
#include "..\mapview.h"
#include "..\getobjpr.h"
#include "..\StatusBarManager.h"
#include "ovl_mgr.h"

#include "FalconView/UIThreadOperation.h"


/* ===========================================================================
 * Statics and Globals
 * ============================seted===============================================
 */

// Static Member Variables of the Coverage Overlay Class
boolean_t CoverageOverlay::m_edit_on = FALSE;
MDSMapTypeVector CoverageOverlay::s_map_type_vector;

// Global variables
int g_drawBrushStyle = DRAWBRUSHSTYLE_HATCH;
int               g_mdm_all_lock_count = 0;
int               g_mdm_copy_lock_count = 0;
int               g_bDataPathsActiveMode = FALSE;
CMDMSheet*        g_pMDMSheet;
CoverageOverlay*  g_pCoverageOverlay = NULL;

// Get draw brush style
int cat_getDrawBrushStyle()
{
   return g_drawBrushStyle;
}

// Set draw brush style
void cat_setDrawBrushStyle(int style)
{
   g_drawBrushStyle = style;
}

void cat_set_coverage_overlay(CoverageOverlay *pOverlay)
{
   g_pCoverageOverlay = pOverlay;
}

// Provide access to the coverage overlay object.
CoverageOverlay *cat_get_coverage_overlay()
{
   return g_pCoverageOverlay;
}

// Provide access to the coverage overlay object.
// Check for valid pointer.
CoverageOverlay *cat_get_valid_coverage_overlay()
{
   if (!g_pCoverageOverlay)
   {
      ERR_report("cat_get_valid_coverage_overlay() returns NULL.");
      ASSERT(g_pCoverageOverlay);
   }
   return g_pCoverageOverlay;
}

// Get pointer to global MDMSheet object pointer.
// Access needed by pages under the property sheet.
// This pointer can point to either the main MDM dialog
// or to the Map Data Paths dialog when it is up.  Only
// one of these can be functional at a time.
CMDMSheet* cat_getMDMSheet()
{
   return g_pMDMSheet;
}

// Set pointer to global MDMSheet object pointer
void cat_setMDMSheet(CMDMSheet* p)
{
   g_pMDMSheet = p;
}

// This routine will find the appropriate index into the drive symbols IDB_DRIVE_TYPE_AND_STATUS bitmap/toolbar/imagelist
// On-line drive symbols have even indices, off-line have odd indices
// Shared devices begin at 18.

int cat_get_image_index(enum DataSourceTypeEnum eDSType, bool bOnline, bool bShared)
{
   return (2  * eDSType) + (bShared ? 18 : 0) + (bOnline ? 0 : 1);
}

// Static support routine to obtain HDDATA value from registry
CString cat_get_hd_data_from_registry()
{
   char hd_data[PRM_MAX_VALUE_LENGTH+1];

// if (PRM_get_value(PRM_HD_DATA, hd_data) != SUCCESS)
// ERR_report("Failed getting hard disk data path.");
   // the block of code above was replaced by this in order to eliminate the use of PRM_get_value
   CString reg_string = PRM_get_registry_string("Main", PRM_HD_DATA);
   strncpy_s(hd_data, PRM_MAX_VALUE_LENGTH+1, reg_string, PRM_MAX_VALUE_LENGTH);

   return hd_data;
}

// Lock or unlock the MDM.  While locked cannot draw, select, etc.
void cat_mdm_lock(int iLock)   // (locks can be nested)
{
   switch (iLock)
   {
      case MDM_COPYLOCK:
         ++g_mdm_copy_lock_count;
         break;
      case MDM_COPYUNLOCK:
         --g_mdm_copy_lock_count;
         break;
      case MDM_ALLLOCK:
         ++g_mdm_all_lock_count;
         break;
      case MDM_ALLUNLOCK:
         --g_mdm_all_lock_count;

         // If the last unlock invalidate the overlay.
         // This is necessary because locked draw messages
         // may have occured that validated a blank display.
         if (g_mdm_all_lock_count == 0)
         {
            CoverageOverlay *overlay = cat_get_coverage_overlay();
            if (overlay)
               overlay->InvalidateOverlay();
         }
         break;
   }
}

// Check if MDM locked.  While locked cannot draw or select but can process msgs
BOOL cat_is_mdm_all_locked(void)   // (locks can be nested)
{
   return (g_mdm_all_lock_count != 0);  // anything non-zero is locked
}

// Check if MDM drawok is locked.  While locked cannot select, process msgs but can draw.
BOOL cat_is_mdm_either_locked(void)   // (locks can be nested)
{
   return (g_mdm_all_lock_count != 0 || g_mdm_copy_lock_count != 0);  // anything non-zero is locked
}

/* ===========================================================================
 * CoverageOverlay Class Members
 * ===========================================================================
 */
CoverageOverlay::CoverageOverlay() :
   m_rectSelected(0, 0, 0, 0),
   mInitialized(false),
   m_bDisplayExpiredChartsOnly(false)
{
   CWaitCursor wait;

   // init geo-dragger
   m_dragger = new GeoBoundsDragger();
   m_dragger->get_pen().set_foreground_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, 3);
   m_is_dragging = false;

   m_bMDMvisible = FALSE;
   m_bMDMautohide = FALSE;
   cat_setMDMSheet(NULL);
   SetMultipleMapSelected(FALSE);
   m_viewBbox.Zero();
   m_bSelectMode = COMPATIBLE_MODE;
   m_bDrawAsRegion = FALSE;   // internal flag used during drawing logic
   MarkCoverageAsDirty(TRUE);

   m_bShowSpec = FALSE;

   cat_set_coverage_overlay(this);

   cat_mdm_lock(MDM_ALLLOCK);   // lock MDM during initialization (unlocked in Initialize call)
   _Initialize();
}

CoverageOverlay::~CoverageOverlay()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

void CoverageOverlay::Finalize()
{
   if (m_dragger) 
   {
      delete m_dragger;
      m_dragger = NULL;
   }

   AddRef();
   new fvw::UIThreadOperation([=]()
   {
      DestroyMDMDialog();
      cat_set_coverage_overlay(NULL);
      Release();
   });
}

// Initialize is called after the coverage overlay gets completely constructed.
void CoverageOverlay::_Initialize()
{
   if (CreateMDMDialog(TRUE) == FAILURE)
      ERR_report("CreateMDMDialog() failed.");

   // start out with all tiles initially unselected
   MDSWrapper::GetInstance()->UnselectAllTiles();

   CWnd *pFrame = UTL_get_frame();
   if (pFrame)
      pFrame->SetFocus();  // Set the focus to the main window after MDM is created.  (allows use of intellimouse)

   cat_mdm_lock(MDM_ALLUNLOCK);   // MDM locked during initialization

   mInitialized = true;
}

// Invalidate the overlay when switching into or out of the catalog tab
void CoverageOverlay::InvalidateAsNeeded(void) 
{
   static CMDMSheet::MDMActive_t last_active_page = CMDMSheet::cSINGLE;
   CMDMSheet::MDMActive_t current_active_page = cat_getMDMSheet()->GetMDMPageActive();

   if (current_active_page != last_active_page && 
      (last_active_page == CMDMSheet::cCATALOG || current_active_page == CMDMSheet::cCATALOG)
      )
      InvalidateOverlay();

   last_active_page = current_active_page;
};

void CoverageOverlay::InvalidateOverlay(void) 
{
   MarkCoverageAsDirty(TRUE);
   OVL_get_overlay_manager()->invalidate_all(FALSE);  // TRUE erases screen
}

void CoverageOverlay::InvalidateOverlay(LPCRECT pRect) 
{
   MarkCoverageAsDirty(TRUE);
   OVL_get_overlay_manager()->invalidate_rect(pRect, FALSE);  // erases rect
};

// Draw the coverage overlay.
int CoverageOverlay::draw(ActiveMap* map)
{
   CWaitCursor cursor;

   int status = SUCCESS;
   CDC* pDC = map->get_CDC();

   // If MDM is locked, ignore message
   // This allows drawing to take place when COPYLOCK is not set
   if (cat_is_mdm_all_locked()) 
      return SUCCESS;

   // If rotated ignore
   // don't draw over rotated maps (need to unrotate the map...)
   if (map->actual_rotation() != 0.0) 
      return SUCCESS;

   // If printing ignore
   // don't draw when printing
   if (pDC->IsPrinting()) 
      return SUCCESS;

   if (m_is_dragging) m_dragger->draw(map, pDC);

   // Don't draw if no map types are selected

   if (!cat_getMDMSheet()->IsMDMCatalogPageActive()) // always draw when showing the cd library tab
   {
      if (s_map_type_vector.size() == 0) return SUCCESS;
   }

   try
   {
      // get bbox of current view and mark list as dirty if view has changed.
      if (GetViewBbox(map, m_viewBbox)) MarkCoverageAsDirty(TRUE);

      // Determine m_bDrawAsRegion

      m_bDrawAsRegion = true;

      if (!IsMultipleMapSelected())
      {
         MDSMapType *map_type = GetOneMDSMapType();

         bool draw_as_region;
         if (map_type) // Sanity Check
         {
            if (MDSWrapper::GetInstance()->GetDrawAsRegion(&draw_as_region, map, map_type) != SUCCESS)
            {
               ERR_report("CoverageOverlay::draw(): MDSWrapper::GetDrawAsRegion() failed.");
               return FAILURE;
            }
         }
         else
         {
            ASSERT(false);
            return SUCCESS;
         }

         m_bDrawAsRegion = draw_as_region;
      }

      // see if registry has forced drawing of rectangles or regions only
      CString sRegistryDrawMode = PRM_get_registry_string("Main", "drawmode");
      if (sRegistryDrawMode == "regions") 
         m_bDrawAsRegion = TRUE;
      else if (sRegistryDrawMode == "rectangles") 
         m_bDrawAsRegion = FALSE;

      // check registry for brush style to use
      CString sRegistryDrawBrushStyle = PRM_get_registry_string("Main", "brushstyle");
      if (sRegistryDrawBrushStyle == "hatch")
         cat_setDrawBrushStyle(DRAWBRUSHSTYLE_HATCH);
      else if (sRegistryDrawBrushStyle == "pattern")
         cat_setDrawBrushStyle(DRAWBRUSHSTYLE_PATTERN);
      else if (sRegistryDrawBrushStyle == "hollow")
         cat_setDrawBrushStyle(DRAWBRUSHSTYLE_HOLLOW);
      else if (sRegistryDrawBrushStyle == "transparent")
         cat_setDrawBrushStyle(DRAWBRUSHSTYLE_TRANSPARENT);
      else
         cat_setDrawBrushStyle(DRAWBRUSHSTYLE_HATCH);

      // If showing the catalog tab, draw library coverage underneath of what is already there
      if (cat_getMDMSheet()->IsMDMCatalogPageActive() &&  // draw when showing the cd library tab
         !cat_getMDMSheet()->m_shtCatalog.IsInstallInProgress())
      {
         CString map_type_string;

         for (size_t i = 0; i < s_map_type_vector.size(); i++)
         {
            CString s;
            s.Format("%s%d", i == 0 ? "" : ",", s_map_type_vector.at(i)->GetFvwType());
            map_type_string += s;
         }

         status = cat_getMDMSheet()->m_shtCatalog.DrawCatalogRegions(map, pDC, map_type_string,
            m_viewBbox, m_rectSelected, m_HighlightedSet);
      }

      // Draw the online coverage if catalog tab not active OR the show online flag is true
      if (!cat_getMDMSheet()->IsMDMCatalogPageActive() || GetShowOnlineFlag())
      {
         if (m_bDrawAsRegion)
         {
            // Draw regions
            if (IsCoverageDirty())
            {
               m_regionList.RemoveAll();
               
               if (MDSWrapper::GetInstance()->GetViewableRegions(
                  &m_regionList, map, &s_map_type_vector, &m_SourceSet, m_bDisplayExpiredChartsOnly) != SUCCESS)
               {
                  ERR_report("CoverageOverlay::draw(): MDSWrapper::GetViewableRegions() failed.");
               }
            }

            m_regionList.DrawRegions(map, pDC); // Draw whatever is in the list
         }
         else
         {
            // Draw rectangles

            if (MDSWrapper::GetInstance()->DrawViewableIcons(&s_map_type_vector, &m_SourceSet, this, map, pDC, NULL, NULL, m_bDisplayExpiredChartsOnly) != SUCCESS)
            {
               ERR_report("CoverageOverlay::draw(): MDSWrapper::DrawViewableIcons() failed.");
            }
         }
      }

      // If showing the catalog tab draw security message as needed
      if (cat_getMDMSheet()->IsMDMCatalogPageActive() &&    // draw when showing the cd library tab
        !cat_getMDMSheet()->m_shtCatalog.IsInstallInProgress())

      {
         // Determine highest priority message type on the CD.

         CString classification;
         for (size_t i = 0; i < s_map_type_vector.size(); i++)
         {
            CString s;
            s.Format("%s%u", i > 0 ? "," : "", s_map_type_vector.at(i)->GetFvwType());
            
            classification += s;
         }
         
         classification = cat_getMDMSheet()->m_shtCatalog.GetHighestSecurityWarning(classification);

         // Draw security message if known
         if (classification != "")
         {
            CRect rect(10,10,510,510);

            CFont font;
            font.CreatePointFont(80, "Arial");
            CFont* pSavFont = pDC->SelectObject(&font);

            COLORREF savColor = pDC->SetTextColor(RGB(0,0,0));

            // Smear black text around for undercoat. (XOR may be preferred)
            for (int y = 0; y < 3; ++y)
               for (int x = 0; x < 3; ++x)
               {
                  CRect tmpRect = rect;
                  tmpRect.OffsetRect(x,y);
                  pDC->DrawText(classification, &tmpRect, 0);
               }

            rect.OffsetRect(1,1);
            pDC->SetTextColor(RGB(255,255,0));
            pDC->DrawText(classification, &rect, 0);

            pDC->SetTextColor(savColor);
            pDC->SelectObject(pSavFont);
         }
      }

      MarkCoverageAsDirty(FALSE);
      status = SUCCESS;
   }
   catch (CResourceException* exception)
   {
      ERR_report_exception("CResourceException caught in CoverageOverlay::draw()", *exception);
      exception->Delete();
      status = FAILURE;
   }
   catch (CException* exception)
   {
      ERR_report_exception("CResourceException caught in CoverageOverlay::draw()", *exception);

      exception->Delete();
      status = FAILURE;
   }

   return status;
}

void CoverageOverlay::HighlightCatalogSet(LongSet& set) 
{
   m_HighlightedSet.RemoveAll();

   for (int i = 0; i < set.GetSize(); i++)
      m_HighlightedSet.Add(set.GetAt(i));

   //InvalidateOverlay();
   OVL_get_overlay_manager()->invalidate_all(FALSE);  // TRUE erases screen
}

BOOL CoverageOverlay::GetViewBbox(MapProj* map, CGeoRect& viewBbox)
{
   d_geo_t map_ur, map_ll;

   // Get the map bounds
   if (map->get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
   {
      ERR_report("get_vmap_bounds() failed.");
      return TRUE;  // failure is considered a change.
   }

   BOOL bChanged = FALSE;

   // Check to see if any have changed.
   if ( (viewBbox.m_ll_lat != map_ll.lat) ||
         (viewBbox.m_ll_lon != map_ll.lon) ||
         (viewBbox.m_ur_lat != map_ur.lat) ||
         (viewBbox.m_ur_lon != map_ur.lon) 
      )
      bChanged = TRUE;

   viewBbox.m_ll_lat = map_ll.lat;
   viewBbox.m_ll_lon = map_ll.lon;
   viewBbox.m_ur_lat = map_ur.lat;
   viewBbox.m_ur_lon = map_ur.lon;

   return bChanged;
}

// is the point above a file icon
C_icon *CoverageOverlay::hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point)
{
   // tool tips and help text are implemented in on_mouse_moved

   return NULL;
}


// Inform the coverage overlay that it is about to be closed, if files are
// selected the user will be warned that their selection will be lost.  If
// the user cancels the close this fucntion will return cancel == TRUE.
int CoverageOverlay::pre_close(boolean_t *cancel)
{
   if (cancel)
      *cancel = FALSE;

   return SUCCESS;
}

// Turn the Map Data Manager on or off.
/* static */
int CoverageOverlay::set_edit_on(boolean_t edit_on)
{
   // Don't call cat_get_coverage_overlay cause this routine
   // gets called before the overlay is created! >:(
   // and cat_get_coverage_overlay asserts if there is no overlay.
   CoverageOverlay *overlay = (CoverageOverlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_MapDataManager);

   // no change - do nothing
   if (m_edit_on == edit_on) 
      return SUCCESS;

   // closing the map data manager
   if (!edit_on)
   {
      // Let MDSWrapper clean up

      if (MDSWrapper::GetInstance()->MDMClosing() != SUCCESS)
      {
         ERR_report("CoverageOverlay::set_edit_on(): MDSWrapper::MDMClosing() failed.");
      }

      // if the coverage overlay exists
      if (overlay)
      {
         // remove the overlay from the list and the display, and delete object
         OVL_get_overlay_manager()->delete_overlay(overlay);
         overlay->Release();
      }
   }
   else
   {
      PopulateMDSMapTypes();
   }

   m_edit_on = edit_on;

   return SUCCESS;
}

// This function is called to get the default cursor associated with the
// Map Data Manager mode.
HCURSOR CoverageOverlay::get_default_cursor()
{
   HCURSOR cursor = NULL;
   int id = 0;

   // Don't call cat_get_coverage_overlay cause this routine
   // gets called before the overlay is created! >:(
   // and cat_get_coverage_overlay asserts if there is no overlay.
   CoverageOverlay *overlay = (CoverageOverlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_MapDataManager);

   if (overlay)
   {
      cursor = overlay->GetCursorType((GetKeyState(VK_CONTROL) < 0) ? MK_CONTROL : 0);
   }
   else
   {
      cursor = AfxGetApp()->LoadCursor(IDC_CROSSHAIR);
   }

   return cursor;
}


// Used to get the default cursor when overtop a fileicon
int CoverageOverlay::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   int status = FAILURE;
   
   if (m_is_dragging)
   {
      HCURSOR cursor = NULL;
      HintText *hintText = NULL;

      drag(OVL_get_overlay_manager()->get_view_map(), point, flags, &cursor, &hintText);

      if (cursor != NULL)
         pMapView->SetCursor(cursor);
      if (hintText != NULL)
      {
         pMapView->SetTooltipText(_bstr_t(hintText->get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hintText->get_help_text()));
      }

      return SUCCESS;
   }

   CoverageOverlay *overlay = (CoverageOverlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_MapDataManager);

   pMapView->SetCursor(overlay->GetCursorType((GetKeyState(VK_CONTROL) < 0) ? MK_CONTROL : 0));

   // Only look up icon if not viewing regions or showing catalog page
   if (!m_bDrawAsRegion || cat_getMDMSheet()->IsMDMCatalogPageActive())
   {
      CRect rc;

      rc.left   = point.x;
      rc.top    = point.y;
      rc.right  = point.x;
      rc.bottom = point.y;
      rc.InflateRect(1, 1);

      map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

      CGeoRect selectBbox;
      selectBbox.MapFromDeviceWindow(&mapProjWrapper, &rc);

      // If showing the catalog tab draw find icons in the cdlib list
      if (cat_getMDMSheet() && cat_getMDMSheet()->IsMDMCatalogPageActive())   // draw when showing the cd library tab
      {

      }
      else
      {
         CString strTooltip, strHelpText;
         if (MDSWrapper::GetInstance()->GetTooltipAndHelpText(&s_map_type_vector, &m_SourceSet, selectBbox, strTooltip, strHelpText, m_bDisplayExpiredChartsOnly))
         {
            m_hint.set_tool_tip(strTooltip);
            m_hint.set_help_text(strHelpText);

            pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
            pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));

            status = SUCCESS;
         }
      }
   }

   return status;
}

int CoverageOverlay::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_is_dragging)
   {
      drop(OVL_get_overlay_manager()->get_view_map(), point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}

// ==========================================================

// If the coverage overlay is the current overlay (Map Data Manager mode),
// then this function starts a file selection (*drag == TRUE).  Otherwise,
// it will make the coverage overlay the current overlay, if the given point
// hits one of the file icons.
int CoverageOverlay::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   C_icon *icon;

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   // see if the cursor hits this overlay
   icon = hit_test(&mapProjWrapper, point);

   // if the coverage overlay is not current, then make it current, iff
   // a file icon was hit
   if (!get_current() && icon)
   {
      // if editing isn't on, turning it on will make the overlay current
      if (get_edit_on())
      {
         // make this the current overlay
         if (OVL_get_overlay_manager()->make_current(this) != SUCCESS)
            ERR_report("make_current() failed.");

         return SUCCESS;
      }
   }

   // if not in MDM mode, see if the user wants to switch to it, iff
   // a file icon was hit
   if (!get_edit_on() && icon)
   {
      // switch to MDM mode
      if (OVL_get_overlay_manager()->set_mode(FVWID_Overlay_MapDataManager) != SUCCESS)
         ERR_report("set_mode() failed.");

      return SUCCESS;
   }

   // only start a selection if in MDM mode and COMPATIBLE_MODE conditions are correct
   if (get_edit_on())
   {
      d_geo_t anchor;
      mapProjWrapper.surface_to_geo(point.x, point.y, &anchor.lat, &anchor.lon);

      // make sure the east bounds does not initially equal the west bounds.  This is because
      // in GeoBounds::prepare_for_redraw, we don't want to be in the condition in which
      // the geo width is less than or equal to zero since this will cause 360 to be added
      // to the geo width which causes the dragger to wrap around the world
      d_geo_t anchor2 = anchor;
      anchor2.lon -= 1.0e-07;

      // wrap using internal CMapProjWrapper until all occurrences have
      // been converted to use the common class
      CMapProjWrapper intWrapper(mapProjWrapper.GetInterfacePtr());
      
      m_dragger->set_bounds_no_error_check(anchor2, anchor);
      m_dragger->set_modified_edge(UTL_IDC_SIZE_SE);
      m_dragger->prepare_for_redraw(&intWrapper);
      m_dragger->set_previous(anchor);
      m_is_dragging = true;

      return SUCCESS;
   }

   return FAILURE;
}

// Updates the rubber band box during a file selection.
void CoverageOverlay::drag(ViewMapProj* map, CPoint point, UINT flags, 
   HCURSOR *cursor, HintText **hint)
{
   static_cast<MapView *>(map->get_CView())->ScrollMapIfPointNearEdge(point.x,
      point.y);

   // update the geobounds dragger
   HintText unused_hint;
   CClientDC dc(map->get_CView());
   m_dragger->on_drag(map, &dc, point, flags, cursor, unused_hint);

   *cursor = GetCursorType(flags);

   // don't care about tool tip or help text for now
   *hint = NULL;
}

// Returns a cursor based on current mode of map data manager
HCURSOR CoverageOverlay::GetCursorType(UINT flags)
{
   int id = 0;
   BOOL bMulti = IsMultipleMapSelected();

   switch (GetSelectMode())
   {
      case COPY_MODE:
         if (flags & MK_CONTROL)
            id = bMulti ? IDC_MULCOPY_ERASE : IDC_SELCOPY_ERASE;
         else
            id = bMulti ? IDC_MULCOPY : IDC_SELCOPY;
         break;
      case DELETE_MODE:
         if (flags & MK_CONTROL)
            id = bMulti ? IDC_MULDELETE_ERASE : IDC_SELDELETE_ERASE;
         else
            id = bMulti ? IDC_MULDELETE : IDC_SELDELETE;
         break;
      case COMPATIBLE_MODE:
         if (flags & MK_CONTROL)
            id = bMulti ? IDC_MULSINGLE_ERASE : IDC_SELSINGLE_ERASE;
         else
            id = bMulti ? IDC_MULSINGLE : IDC_SELSINGLE;
         break;
      default:
         id = IDC_CROSSHAIR;
         break;
   }
   return AfxGetApp()->LoadCursor(id);
}

// Completes a file selection.
void CoverageOverlay::drop(ViewMapProj* map, CPoint point, UINT flags)
{
   if (cat_is_mdm_either_locked())   // If MDM is locked, ignore message
      return;

   CWaitCursor wait;
   CClientDC dc(map->get_CView());
   map_projection_utils::CMapProjWrapper mapProjWrapper(map->GetSettableMapProj());

   BOOL bRefreshOverlay = FALSE;
   CRect affectedBounds;
   CRect affectedBoundsLeft;
   CRect affectedBoundsRight;
   int iSelectCode = CV_UNSELECT_ALL; // Default behavior
   BOOL bUnselecting = FALSE;
   CGeoRect selectBbox;
   CRect rect;

   if (flags & MK_CONTROL)
      bUnselecting = TRUE;

   // Only allow selecting in first instance of FalconView
   if (UTL_get_instance() != 1)
   {
      AfxMessageBox("The Map Data Manager may only be used to view map data "
      "coverage in all but the first instance. This is not the first "
      "instance.");
      goto err_exit;
   }

   // finish drag
   m_dragger->on_drop(map, &dc, point, flags);
   m_is_dragging = false;

   // get device rectangle from dragger
   int ul_x, ul_y, lr_x, lr_y;

   map->geo_to_surface(m_dragger->get_nw().lat, m_dragger->get_nw().lon, &ul_x, &ul_y);
   map->geo_to_surface(m_dragger->get_se().lat, m_dragger->get_se().lon, &lr_x, &lr_y);

   rect.left = ul_x;
   rect.top = ul_y;
   rect.right = lr_x;
   rect.bottom = lr_y;

   // if viewing catalog page then alter behavior to select catalog items only
   if (cat_getMDMSheet()->IsMDMCatalogPageActive())
   {
      // Create rectangle from point if too small
      if (rect.Height() < 4 && rect.Width() < 4)
      {
         rect.left   = point.x;
         rect.top    = point.y;
         rect.right  = point.x;
         rect.bottom = point.y;
         rect.InflateRect(1, 1);
      }

      // Fatten it if either dimension too small because
      // GEO_intersect fails if width is zero!?
      // see comment in GEO_intersect() --> "if either region goes around the world, intersection exists"
      if (rect.Height() < 4 || rect.Width() < 4)
         rect.InflateRect(1, 1);

      selectBbox.MapFromDeviceWindow(&mapProjWrapper, &rect);

      // loads list box with CD's for current maptype (must be done before SelectCatalogIcons call below)
      cat_getMDMSheet()->m_shtCatalog.QuerySet(NULL);
      cat_getMDMSheet()->m_shtCatalog.QueryRect(&selectBbox);

      int status = cat_getMDMSheet()->m_shtCatalog.SelectCatalogRegions(m_viewBbox);

      m_rectSelected = selectBbox;

      InvalidateOverlay();    // invalidate all to force a redraw
   }
   else
   {
      selectBbox.MapFromDeviceWindow(&mapProjWrapper, &rect);

      LongArray data_sources;
      data_sources.Copy(m_SourceSet);

      if (MDSWrapper::GetInstance()->SelectByGeoRect(&iSelectCode, selectBbox, s_map_type_vector,
         data_sources, bUnselecting ? true : false, m_bSelectMode,m_bDisplayExpiredChartsOnly) != SUCCESS)
      {
         ERR_report("CoverageOverlay::drop(): MDSWrapper::SelectByGeoRect() failed.");
      }

      if (iSelectCode == CV_ERRSELECT_ATTEMPTDELETESOURCE)
      {
         AfxMessageBox(
            "You are attempting to select red source files for deleting.\n"
            "Only green target files can be selected for removal.\n\n"
            "Use a different tool to select red source files."
         );
      }
      else if (iSelectCode == CV_ERRSELECT_ATTEMPTCOPYTARGET)
      {
         AfxMessageBox(
            "You are attempting to select green target files for copying.\n"
            "Only red source files can be selected for copying.\n\n"
            "Use a different tool to select green target files."
         );
      }

      bRefreshOverlay = TRUE;
   }

err_exit:

   RefreshMDMDialog();

   if (m_bMDMautohide && !m_bMDMvisible)
   {
      ShowMDMDialog(TRUE);
      m_bMDMautohide = FALSE;
   }

   if (bRefreshOverlay)
   {
      // if showing regions invalidate affected area
      if (m_bDrawAsRegion)
      {
         switch (iSelectCode)
         {
         case CV_SELECT_SOURCE:
         case CV_SELECT_TARGET:
            //InvalidateOverlay(&affectedBoundsLeft);          // <--- TODO: fix affectedBounds and reenable
            //InvalidateOverlay(&affectedBoundsRight);
            //break;
         case CV_UNSELECT_SOURCE:
         case CV_UNSELECT_TARGET:
         case CV_UNSELECT_ALL:
            InvalidateOverlay();    // invalidate all when unselecting
            break;
         }
      }
      else if (cat_getDrawBrushStyle() == DRAWBRUSHSTYLE_TRANSPARENT)
      {
         // when using transparency we must invalidate when unselecting cause
         // redrawing just OR's in new pixels which cannot be seen.
         switch (iSelectCode)
         {
         case CV_SELECT_SOURCE:  // Mark as dirty and force a redraw without erasing screen
         case CV_SELECT_TARGET:
            MarkCoverageAsDirty(TRUE);
            OVL_get_overlay_manager()->draw_from_overlay(this);
            break;
         case CV_UNSELECT_SOURCE:
         case CV_UNSELECT_TARGET:
         case CV_UNSELECT_ALL:
            InvalidateOverlay();    // invalidate all when unselecting
            break;
         }
      }
      else  // Mark as dirty and force a redraw without erasing screen
      {
         MarkCoverageAsDirty(TRUE);
         OVL_get_overlay_manager()->draw_from_overlay(this);
      }
   }

   FVW_SetHelpText(""); // remove any pending messages shown in status bar.
}

// Cancels an in progress file selection.
void CoverageOverlay::cancel_drag(ViewMapProj* map)
{
   CClientDC dc(map->get_CView());
   m_dragger->on_cancel_drag(map, &dc);
   m_is_dragging = false;
}

// Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
boolean_t CoverageOverlay::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT /*nRepCnt*/,
      UINT /*nFlags*/, CPoint /*point*/)
{
   if (!m_edit_on)
      return FALSE;

   if (nChar == VK_ESCAPE && m_is_dragging == true)
   {
      cancel_drag(OVL_get_overlay_manager()->get_view_map());
      return TRUE;
   }

   return FALSE;
}


void CoverageOverlay::SelectAllDisplayedData()
{
   CWaitCursor wait;

   BOOL bRefreshOverlay = FALSE;
   int iSelectCode = CV_UNSELECT_ALL; // Default behavior
   BOOL bUnselecting = FALSE;

   LongArray data_sources;
   data_sources.Copy(m_SourceSet);

   if (MDSWrapper::GetInstance()->SelectAllByDS(&iSelectCode, s_map_type_vector, data_sources, m_bSelectMode) != SUCCESS)
   {
      ERR_report("CoverageOverlay::SelectAllVisibleData(): MDSWrapper::SelectAllByDS() failed.");
   }
   if (iSelectCode == CV_ERRSELECT_ATTEMPTDELETESOURCE)
   {
      AfxMessageBox(
         "You are attempting to select red source files for deleting.\n"
         "Only green target files can be selected for removal.\n\n"
         "Use a different tool to select red source files."
      );
   }
   else if (iSelectCode == CV_ERRSELECT_ATTEMPTCOPYTARGET)
   {
      AfxMessageBox(
         "You are attempting to select green target files for copying.\n"
         "Only red source files can be selected for copying.\n\n"
         "Use a different tool to select green target files."
      );
   }

   bRefreshOverlay = TRUE;

   RefreshMDMDialog();

   if (m_bMDMautohide && !m_bMDMvisible)
   {
      ShowMDMDialog(TRUE);
      m_bMDMautohide = FALSE;
   }

   if (bRefreshOverlay)
   {
      // if showing regions invalidate affected area
      if (m_bDrawAsRegion)
      {
         switch (iSelectCode)
         {
         case CV_SELECT_SOURCE:
         case CV_SELECT_TARGET:
            //InvalidateOverlay(&affectedBoundsLeft);          // <--- TODO: fix affectedBounds and reenable
            //InvalidateOverlay(&affectedBoundsRight);
            //break;
         case CV_UNSELECT_SOURCE:
         case CV_UNSELECT_TARGET:
         case CV_UNSELECT_ALL:
            InvalidateOverlay();    // invalidate all when unselecting
            break;
         }
      }
      else if (cat_getDrawBrushStyle() == DRAWBRUSHSTYLE_TRANSPARENT)
      {
         // when using transparency we must invalidate when unselecting cause
         // redrawing just OR's in new pixels which cannot be seen.
         switch (iSelectCode)
         {
         case CV_SELECT_SOURCE:  // Mark as dirty and force a redraw without erasing screen
         case CV_SELECT_TARGET:
            MarkCoverageAsDirty(TRUE);
            OVL_get_overlay_manager()->draw_from_overlay(this);
            break;
         case CV_UNSELECT_SOURCE:
         case CV_UNSELECT_TARGET:
         case CV_UNSELECT_ALL:
            InvalidateOverlay();    // invalidate all when unselecting
            break;
         }
      }
      else  // Mark as dirty and force a redraw without erasing screen
      {
         MarkCoverageAsDirty(TRUE);
         OVL_get_overlay_manager()->draw_from_overlay(this);
      }
   }

   FVW_SetHelpText(""); // remove any pending messages shown in status bar.
}

// Unselects all.
void CoverageOverlay::ClearSelection(long lDataSource)
{
   if ( (lDataSource == -1) && (s_map_type_vector.size() == 0))  // All DS's and all map series
   {
      MDSWrapper::GetInstance()->UnselectAllTiles();  // this is the fast way.
      return;
   }

   // s_map_type_vector is a <vector> of map series that are selected in the MDM list ctrl.

   MDSMapTypeVector vecMapTypes;
   if (s_map_type_vector.size() == 0)  // nothing in list ctrl -- must do all map series
      MDSWrapper::GetInstance()->GetAllMapTypes(&vecMapTypes, lDataSource, true);
   else
      vecMapTypes = s_map_type_vector;

   CString strLastHandler;
   IDSafeArray saMapSeries;

   for (UINT i = 0; i < vecMapTypes.size(); i++)
   {
      MDSMapType *pMapType = vecMapTypes.at(i);

      CString strMapHandlerName = pMapType->GetMapHandlerName();

      if ( i == 0 )  // first map series?
         strLastHandler = strMapHandlerName;

      if ( strLastHandler.CompareNoCase(strMapHandlerName) == 0)  // same map handler?
         saMapSeries.Append(pMapType->GetMapSeriesId());  // save the map series

      else  // different map handler
      {
         // Unselect on the last handler
         MDSWrapper::GetInstance()->UnselectTilesByHandler(strLastHandler, lDataSource, saMapSeries);
         strLastHandler = strMapHandlerName;  // switch to new handler
         saMapSeries.Delete();
         saMapSeries.Append(pMapType->GetMapSeriesId());
      }
   }
   
   // process last handler
   if (saMapSeries.GetNumElements())
         MDSWrapper::GetInstance()->UnselectTilesByHandler(strLastHandler, lDataSource, saMapSeries);

   InvalidateOverlay();
}

void CoverageOverlay::ClearLibrarySelection()
{
   m_rectSelected.Zero();
   InvalidateOverlay();
}

// ==========================================================

int CoverageOverlay::GetSelectMode()
{
   return m_bSelectMode;
}

void CoverageOverlay::SetSelectMode(int mode)
{
   m_bSelectMode = mode;
}

// Note that CreateMDMDialog, RefreshMDMDialog and DestroyMDMDialog methods must all match
int CoverageOverlay::CreateMDMDialog(BOOL bShow)
{
   int status = FAILURE;

   m_bMDMvisible = FALSE;
   if (cat_getMDMSheet() == NULL)
   {
      // use the frame for the property sheet parent window
      CWnd *pFrame = UTL_get_frame();
      CMDMSheet* pSheet;

      // Create the MDM property sheet and add all pages
      if (!(pSheet = new CMDMSheet("Map Data Manager", pFrame, 0)))
         status = FAILURE;
      else
      {
         cat_setMDMSheet(pSheet);

         // Note that this must be a POPUP window or else it won't hide when printing
         DWORD dwStyle = WS_POPUP|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_CAPTION;
         DWORD dwExStyle = 0; //WS_EX_DLGMODALFRAME|WS_EX_MDICHILD;
         pSheet->m_shtSingle.m_bReadyToActivate = FALSE;
         if (!cat_getMDMSheet()->Create(pFrame, dwStyle, dwExStyle))
         {
            delete cat_getMDMSheet();
            cat_setMDMSheet(NULL);
            return FAILURE;
         }

         pSheet->m_shtSingle.m_bReadyToActivate = TRUE;
         cat_getMDMSheet()->SetSheetPos(pFrame);

         ShowMDMDialog(bShow);

         status = SUCCESS;
      }
   }
   return status;
}

// Note that CreateMDMDialog, RefreshMDMDialog and DestroyMDMDialog methods must all match
int CoverageOverlay::RefreshMDMDialog()
{
   // update dialog box fields and buttons
   if (cat_getMDMSheet())
      cat_getMDMSheet()->Refresh();

   return SUCCESS;
}

// Note that CreateMDMDialog, RefreshMDMDialog and DestroyMDMDialog methods must all match
int CoverageOverlay::DestroyMDMDialog()
{
   cat_getMDMSheet()->SaveSheetPos();
   delete cat_getMDMSheet();

   cat_setMDMSheet(NULL);
   m_bMDMvisible = FALSE;

   return SUCCESS;
}

void CoverageOverlay::ShowMDMDialog(BOOL bShow)
{
   m_bMDMvisible = bShow;

   if (cat_getMDMSheet())
   {
      // hide or reshow existing MDM dialog
      cat_getMDMSheet()->ShowWindow(bShow);

      // force WM_SIZE messages
      cat_getMDMSheet()->NudgeSheetPos();
   }
}

void CoverageOverlay::AutoHideMDMDialog()
{
   m_bMDMvisible = FALSE;
   m_bMDMautohide = TRUE;

   if (cat_getMDMSheet())
      cat_getMDMSheet()->ShowWindow(FALSE);  // hide existing MDM dialog
}

BOOL CoverageOverlay::IsMDMDialogVisible()
{
   return m_bMDMvisible;
}

void CoverageOverlay::SetMultipleMapSelected(boolean_t b)
{
   m_bMultipleMapSelected = b;
}

boolean_t CoverageOverlay::IsMultipleMapSelected()
{
   return s_map_type_vector.size() > 1;
}

void CoverageOverlay::SetSources(LongArray* p)
{
   // Save original set of source ID's to see if set changes
   // This process assumes sets are ordered the same...
   BOOL bChanged = FALSE;
   LongArray setSaveIDArray;
   GetSources(&setSaveIDArray);

   // Copy members of set that are online
   m_SourceSet.RemoveAll();
   for (int i = 0; i < p->GetSize(); ++i)
   {
      m_SourceSet.Add(p->GetAt(i));
      if (i < setSaveIDArray.GetSize() && p->GetAt(i) != setSaveIDArray[i])
         bChanged = TRUE;  // value changed, invalidate
   }

   if (m_SourceSet.GetSize() != setSaveIDArray.GetSize())
      bChanged = TRUE;  // array size changed, invalidate

   // if set has been changed, invalidate
   if (bChanged)
      InvalidateOverlay();
};

void CoverageOverlay::SetAllSources(void)
{
   LongArray data_sources;
   if (MDSWrapper::GetInstance()->GetDataSourceIds(&data_sources, true /* Exclude Offline */, true) != SUCCESS)
   {
      ERR_report("CoverageOverlay::SetAllSources(): MDSWrapper::GetDataSourceIds() failed.");
   }
   
   SetSources(&data_sources);  // call SetSources to filter for online
};

void CoverageOverlay::HideAllSources(void)
{
   LongArray setIDArray;  // empty set
   SetSources(&setIDArray);  // call SetSources to filter for online
};

void CoverageOverlay::GetSources(LongArray* p)
{ 
   // Copy all members of set
   p->RemoveAll();
   for (int i = 0; i < m_SourceSet.GetSize(); ++i)
      p->Add(m_SourceSet.GetAt(i));
};

void CoverageOverlay::RemoveSource(long id)
{
   for (int i = 0; i < m_SourceSet.GetSize(); i++)
   {
      if (m_SourceSet.GetAt(i) == id)
      {
         m_SourceSet.RemoveAt(i);
         return;
      }
   }
}

void CoverageOverlay::AddSource(long id)
{
   for (int i = 0; i < m_SourceSet.GetSize(); i++)
   {
      if (m_SourceSet.GetAt(i) == id) return; // Sanity Check
   }

   m_SourceSet.Add(id);
}

void CoverageOverlay::DisplayExpiredChartsOnly(bool bExpired)
{
   m_bDisplayExpiredChartsOnly = bExpired;
   InvalidateOverlay();
}

void CoverageOverlay::SetActiveMDMSheet(CMDMSheet::MDMActive_t page)
{
   CMDMSheet* pSheet = cat_getMDMSheet();
   if (pSheet != NULL)
   {
      pSheet->SetMDMPageActive(page);
   }
}



//==============================================================================================

// Copy and Delete selected files of current maptype set
int CoverageOverlay::CopySelectedFiles(void)
{
   CWaitCursor wait;
   int status = FAILURE;

   ShowMDMDialog(FALSE);

   if (MDSWrapper::GetInstance()->CopyAndDelete() != SUCCESS)
   {
      ERR_report("CMDMChartIndex::OnMdmmultipleApply(): MDSWrapper::CopyAndDelete() failed.");
   }
   else
   {
      MDSWrapper::GetInstance()->UnselectAllTiles();
   }

   SetAllSources();
   InvalidateOverlay();

   ShowMDMDialog(TRUE);
   RefreshMDMDialog();

   return status;
}

// Get available free space from target drive
__int64 CoverageOverlay::GetFreeSpaceMB()
{
   CString sTargetPath;
   if (MDSWrapper::GetInstance()->GetTargetSourcePath(&sTargetPath) != SUCCESS)
   {
      ERR_report("CoverageOverlay::GetFreeSpaceMB(): MDSWrapper::GetTargetSourcePath() failed.");
      sTargetPath = "C:\\";
   }

   return GetPathFreeSpace(sTargetPath);
}

// Get available free space from target drive and format into string
CString CoverageOverlay::GetFreeSpaceMBAsString(void)
{
   CMBString sMB;

   return sMB.FormatMB(static_cast<double>(GetFreeSpaceMB()));
}

/* static */ void CoverageOverlay::PopulateMDSMapTypes()
{
   // Note that since map type long values are not guaranteed to be the same from one instance
   // of FalconView to the next, then this method may sometimes return a blank map type vector.
   // In general, though, the user will retain the same selection of map types between sessions.

   // Create a dictionary of known map types

   MDSMapTypeVector map_type_vector;

   if (MDSWrapper::GetInstance()->GetAllMapTypes(&map_type_vector) != SUCCESS)
   {
      ERR_report("CoverageOverlay::PopulateMDSMapTypes(): MDSWrapper::GetAllMapTypes() failed.");
   }

   MDSMapTypeToLongMap map_type_map;

   for (size_t i = 0; i < map_type_vector.size(); i++)
   {
      map_type_map[map_type_vector.at(i)] = 1;
   }

   // Retrieve map types from the registry string and validate that they all exist

   s_map_type_vector.clear();

   CString registry_string = PRM_get_registry_string("Map Data Manager", "MDM Displayed Map Types");

   while (registry_string.GetLength() > 0)
   {
      int space_position = registry_string.Find(' ');

      if (space_position == -1) // There are no map types or registry_string bad
      {
         RemoveAllMDSMapTypes();
         return;
      }

      CString map_type_string = registry_string.Left(space_position);
      registry_string = registry_string.Right(registry_string.GetLength() - space_position - 1);

      if (atol(map_type_string + '1') < 1) // registry_string is bad
      {
         RemoveAllMDSMapTypes();
         return;
      }

      MDSMapType *map_type = MDSMapType::GetByLongValue(atol(map_type_string));

      if (map_type_map.find(map_type) == map_type_map.end()) // The map type is unknown
      {
         RemoveAllMDSMapTypes();
         return;
      }

      s_map_type_vector.push_back(map_type);
   }
}

/* static */ void CoverageOverlay::RemoveAllMDSMapTypes()
{
   s_map_type_vector.clear();
   PRM_set_registry_string("Map Data Manager", "MDM Displayed Map Types", "");
}

/* static */ void CoverageOverlay::AddMDSMapType(MDSMapType *map_type)
{
   s_map_type_vector.push_back(map_type);
   const int MAP_TYPE_STRING_LEN = 100;
   char map_type_string[MAP_TYPE_STRING_LEN];
   _ltoa_s(map_type->GetLongValue(), map_type_string, MAP_TYPE_STRING_LEN, 10);
   CString registry_string = PRM_get_registry_string("Map Data Manager", "MDM Displayed Map Types");
   registry_string += CString(map_type_string) + " ";
   PRM_set_registry_string("Map Data Manager", "MDM Displayed Map Types", registry_string);
}

/* static */ MDSMapType *CoverageOverlay::GetOneMDSMapType()
{
   if (s_map_type_vector.size() > 0) return s_map_type_vector.at(0);
   return NULL;
}

/* static */ MDSMapTypeVector *CoverageOverlay::GetMDSMapTypes()
{
   return &s_map_type_vector;
}

