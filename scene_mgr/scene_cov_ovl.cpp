// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



// stdafx first
#include "stdafx.h"

// this file's header
#include "scene_cov_ovl.h"

// system includes
#include "MBString.h"

// third party files

// other FalconView headers
#include "..\getobjpr.h"
#include "..\mapview.h"
#include "..\MdsUtilities\MdsUtilities.h"
#include "..\StatusBarManager.h"
#include "FalconView/UIThreadOperation.h"
#include "file.h"
#include "map.h"
#include "ovl_mgr.h"
#include "ovlelem.h"  // for GeoBoundsDragger
#include "OvlFctry.h"
#include "param.h"
#include "refresh.h"
#include "utils.h"

// this project's headers
#include "scene_mgr.h"
#include "SceneIconItem.h"  // for CGeoRect
#include "SDSWrapper.h"
#include "sm_factory.h"
#include "SMSheet.h"



namespace scene_mgr
{

/* ===========================================================================
 * Statics and Globals
 * ===========================================================================
 */

// Static Member Variables of the Coverage Overlay Class
boolean_t SceneCoverageOverlay::m_edit_on = FALSE;
std::vector<SDSSceneType*> SceneCoverageOverlay::s_scene_type_vector;


// Global variables
int                    g_sm_DrawBrushStyle = SM_DRAWBRUSHSTYLE_HATCH;
int                    g_sm_all_lock_count = 0;
int                    g_sm_copy_lock_count = 0;
int                    g_sm_bDataPathsActiveMode = FALSE;
CSMSheet*              g_sm_pSMSheet;
SceneCoverageOverlay*  g_pCoverageOverlay = NULL;


void sm_set_coverage_overlay(SceneCoverageOverlay *pOverlay)
{
   g_pCoverageOverlay = pOverlay;
}

// Provide access to the coverage overlay object.
SceneCoverageOverlay *sm_get_coverage_overlay()
{
   return g_pCoverageOverlay;
}

// Provide access to the coverage overlay object.
// Check for valid pointer.
SceneCoverageOverlay *sm_get_valid_coverage_overlay()
{
   if (!g_pCoverageOverlay)
   {
      ERR_report("sm_get_valid_coverage_overlay() returns NULL.");
      ASSERT(g_pCoverageOverlay);
   }
   return g_pCoverageOverlay;
}

// Get draw brush style
int sm_getDrawBrushStyle()
{
   return g_sm_DrawBrushStyle;
}

// Set draw brush style
void sm_setDrawBrushStyle(int style)
{
   g_sm_DrawBrushStyle = style;
}

// Get pointer to global SMSheet object pointer.
// Access needed by pages under the property sheet.
// This pointer can point to either the main SM dialog
// or to the Scene Data Paths dialog when it is up.  Only
// one of these can be functional at a time.
CSMSheet* sm_getSMSheet()
{
   return g_sm_pSMSheet;
}

// Set pointer to global SMSheet object pointer
void sm_setSMSheet(CSMSheet* p)
{
   g_sm_pSMSheet = p;
}



/* ===========================================================================
 * SceneCoverageOverlay Class Members
 * ===========================================================================
 */

SceneCoverageOverlay::SceneCoverageOverlay() :
   mInitialized(false)
{
   CWaitCursor wait;

   // init geo-dragger
   m_dragger = new GeoBoundsDragger();
   m_dragger->get_pen().set_foreground_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, 3);
   m_is_dragging = false;

   m_bSMvisible = FALSE;
   m_bSMautohide = FALSE;
   sm_setSMSheet(NULL);
   m_viewBbox.Zero();
   m_bSelectMode = SM_COMPATIBLE_MODE;
   m_bDrawAsRegion = FALSE;   // internal flag used during drawing logic
   MarkCoverageAsDirty(TRUE);

   m_bShowSpec = FALSE;
   sm_set_coverage_overlay(this);
   sm_lock(SM_ALLLOCK);   // lock SDM during initialization (unlocked in Initialize call)
   _Initialize();
}

SceneCoverageOverlay::~SceneCoverageOverlay()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

// Initialize is called after the coverage overlay gets completely constructed.
void SceneCoverageOverlay::_Initialize()
{
   if (CreateSMDialog(TRUE) == FAILURE)
      ERR_report("CreateSMDialog() failed.");

   // start out with all tiles initially unselected
   SDSWrapper::GetInstance()->UnselectAllTiles();

   CWnd *pFrame = UTL_get_frame();
   if (pFrame)
      pFrame->SetFocus();  // Set the focus to the main window after SM is created.  (allows use of intellimouse)

   sm_lock(SM_ALLUNLOCK);   // SM locked during initialization

   mInitialized = true;
}

void SceneCoverageOverlay::Finalize()
{
   if (m_dragger) 
   {
      delete m_dragger;
      m_dragger = NULL;
   }

   AddRef();
   new fvw::UIThreadOperation([=]()
   {
      DestroySMDialog();
      sm_set_coverage_overlay(NULL);
      Release();
   });
}

// Note that CreateSMDialog, RefreshSMDialog and DestroySMDialog methods must all match
int SceneCoverageOverlay::CreateSMDialog(BOOL bShow)
{
   int status = FAILURE;

   m_bSMvisible = FALSE;
   if (sm_getSMSheet() == NULL)
   {
      // use the frame for the property sheet parent window
      CWnd *pFrame = UTL_get_frame();
      CSMSheet* pSheet;

      // Create the SM property sheet and add all pages
      if (!(pSheet = new CSMSheet("Scene Data Manager", pFrame, 0)))
         status = FAILURE;
      else
      {
         sm_setSMSheet(pSheet);

         // Note that this must be a POPUP window or else it won't hide when printing
         DWORD dwStyle = WS_POPUP|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_CAPTION;
         DWORD dwExStyle = 0; //WS_EX_DLGMODALFRAME|WS_EX_MDICHILD;
         if (!sm_getSMSheet()->Create(pFrame, dwStyle, dwExStyle))
         {
            delete sm_getSMSheet();
            sm_setSMSheet(NULL);
            return FAILURE;
         }

         sm_getSMSheet()->SetSheetPos(pFrame);

         ShowSMDialog(bShow);

         status = SUCCESS;
      }
   }
   return status;
}

// Note that CreateSMDialog, RefreshSMDialog and DestroySMDialog methods must all match
int SceneCoverageOverlay::RefreshSMDialog()
{
   // update dialog box fields and buttons
   if (sm_getSMSheet())
      sm_getSMSheet()->Refresh();

   return SUCCESS;
}

// Note that CreateSMDialog, RefreshSMDialog and DestroySMDialog methods must all match
int SceneCoverageOverlay::DestroySMDialog()
{
   sm_getSMSheet()->SaveSheetPos();
   delete sm_getSMSheet();

   sm_setSMSheet(NULL);
   m_bSMvisible = FALSE;

   return SUCCESS;
}

void SceneCoverageOverlay::ShowSMDialog(BOOL bShow)
{
   m_bSMvisible = bShow;

   if (sm_getSMSheet())
   {
      // hide or reshow existing SDM dialog
      sm_getSMSheet()->ShowWindow(bShow);

      // force WM_SIZE messages
      sm_getSMSheet()->NudgeSheetPos();
   }
}

void SceneCoverageOverlay::AutoHideSMDialog()
{
   m_bSMvisible = FALSE;
   m_bSMautohide = TRUE;

   if (sm_getSMSheet())
      sm_getSMSheet()->ShowWindow(FALSE);  // hide existing SDM dialog
}

BOOL SceneCoverageOverlay::IsSMDialogVisible()
{
   return m_bSMvisible;
}


// Draw the coverage overlay.
int SceneCoverageOverlay::draw(ActiveMap* map)
{
   CWaitCursor cursor;

   int status = SUCCESS;
   CDC* pDC = map->get_CDC();

   // If SM is locked, ignore message
   // This allows drawing to take place when COPYLOCK is not set
   if (sm_is_sm_all_locked()) 
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
   if (s_scene_type_vector.size() == 0) return SUCCESS;

   try
   {
      // get bbox of current view and mark list as dirty if view has changed.
      if (GetViewBbox(map, m_viewBbox)) MarkCoverageAsDirty(TRUE);

      // Determine m_bDrawAsRegion

      m_bDrawAsRegion = true;

      if (!AreMultipleSceneTypesSelected())
      {
#if 0  // RP
         SDSSceneType *scene_type = GetOneSDSSceneType();

         bool draw_as_region;
         if (scene_type) // Sanity Check
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
#endif
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
         sm_setDrawBrushStyle(SM_DRAWBRUSHSTYLE_HATCH);
      else if (sRegistryDrawBrushStyle == "pattern")
         sm_setDrawBrushStyle(SM_DRAWBRUSHSTYLE_PATTERN);
      else if (sRegistryDrawBrushStyle == "hollow")
         sm_setDrawBrushStyle(SM_DRAWBRUSHSTYLE_HOLLOW);
      else if (sRegistryDrawBrushStyle == "transparent")
         sm_setDrawBrushStyle(SM_DRAWBRUSHSTYLE_TRANSPARENT);
      else
         sm_setDrawBrushStyle(SM_DRAWBRUSHSTYLE_HATCH);

      // Draw the coverage
      if (SDSWrapper::GetInstance()->DrawViewableIcons(s_scene_type_vector,
         m_SourceSet, this, map, pDC, NULL, NULL) != SUCCESS)
      {
         ERR_report("CoverageOverlay::draw(): "
            "SDSWrapper::DrawViewableIcons() failed.");
      }

      MarkCoverageAsDirty(FALSE);
      status = SUCCESS;
   }
   catch (CResourceException* exception)
   {
      ERR_report_exception("CResourceException caught in SceneCoverageOverlay::draw()", *exception);
      exception->Delete();
      status = FAILURE;
   }
   catch (CException* exception)
   {
      ERR_report_exception("CException caught in SceneCoverageOverlay::draw()", *exception);

      exception->Delete();
      status = FAILURE;
   }

   return status;
}

BOOL SceneCoverageOverlay::GetViewBbox(MapProj* map, CGeoRect& viewBbox)
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


// Used to get the default cursor when overtop a fileicon
int SceneCoverageOverlay::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
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

   SceneCoverageOverlay *overlay = (SceneCoverageOverlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SceneManager);

   pMapView->SetCursor(overlay->GetCursorType((GetKeyState(VK_CONTROL) < 0) ? MK_CONTROL : 0));

   // Only look up icon if not viewing regions or showing catalog page
   if (!m_bDrawAsRegion)
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

      CString strTooltip, strHelpText;
#if 0  // RP
      if (MDSWrapper::GetInstance()->GetTooltipAndHelpText(&s_map_type_vector, &m_SourceSet, selectBbox, strTooltip, strHelpText, m_bDisplayExpiredChartsOnly))
      {
         m_hint.set_tool_tip(strTooltip);
         m_hint.set_help_text(strHelpText);

         pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));

         status = SUCCESS;
      }
#endif
   }

   return status;
}

int SceneCoverageOverlay::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_is_dragging)
   {
      drop(OVL_get_overlay_manager()->get_view_map(), point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}

// ==========================================================

// If the scenecoverage overlay is the current overlay (Scene Data Manager mode),
// then this function starts a file selection (*drag == TRUE).  Otherwise,
// it will make the coverage overlay the current overlay, if the given point
// hits one of the file icons.
int SceneCoverageOverlay::selected(IFvMapView* pMapView, CPoint point, UINT flags)
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

   // if not in SM mode, see if the user wants to switch to it, iff
   // a file icon was hit
   if (!get_edit_on() && icon)
   {
      // switch to SM mode
      if (OVL_get_overlay_manager()->set_mode(FVWID_Overlay_SceneManager) != SUCCESS)
         ERR_report("set_mode() failed.");

      return SUCCESS;
   }

   // only start a selection if in SDM mode and COMPATIBLE_MODE conditions are correct
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
void SceneCoverageOverlay::drag(ViewMapProj* map, CPoint point, UINT flags, 
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


// Turn the Scene Manager on or off.
/* static */
int SceneCoverageOverlay::set_edit_on(boolean_t edit_on)
{
   // Don't call cat_get_coverage_overlay cause this routine
   // gets called before the overlay is created! >:(
   // and cat_get_coverage_overlay asserts if there is no overlay.
   SceneCoverageOverlay *overlay = (SceneCoverageOverlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SceneManager);

   // no change - do nothing
   if (m_edit_on == edit_on) 
      return SUCCESS;

   // closing the scene manager
   if (!edit_on)
   {
      // Let SDSWrapper clean up
      if (SDSWrapper::GetInstance()->SMClosing() != SUCCESS)
      {
         ERR_report("SceneCoverageOverlay::set_edit_on(): SDSWrapper::SMClosing() failed.");
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
      PopulateSDSSceneTypes();
   }

   m_edit_on = edit_on;

   return SUCCESS;
}

// This function is called to get the default cursor associated with the
// Scene Data Manager mode.
HCURSOR SceneCoverageOverlay::get_default_cursor()
{
   HCURSOR cursor = NULL;
   int id = 0;

   // Don't call cat_get_coverage_overlay cause this routine
   // gets called before the overlay is created! >:(
   // and cat_get_coverage_overlay asserts if there is no overlay.
   SceneCoverageOverlay *overlay = (SceneCoverageOverlay *)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SceneManager);

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

// Returns a cursor based on current mode of map data manager
HCURSOR SceneCoverageOverlay::GetCursorType(UINT flags)
{
   int id = 0;
   BOOL bMulti = AreMultipleSceneTypesSelected();

   switch (GetSelectMode())
   {
   case SM_COPY_MODE:
      if (flags & MK_CONTROL)
         id = bMulti ? IDC_MULCOPY_ERASE : IDC_SELCOPY_ERASE;
      else
         id = bMulti ? IDC_MULCOPY : IDC_SELCOPY;
      break;
   case SM_DELETE_MODE:
      if (flags & MK_CONTROL)
         id = bMulti ? IDC_MULDELETE_ERASE : IDC_SELDELETE_ERASE;
      else
         id = bMulti ? IDC_MULDELETE : IDC_SELDELETE;
      break;
   case SM_COMPATIBLE_MODE:
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

/* ===========================================================================
 * Statics and Globals
 * ===========================================================================
 */
// This routine will find the appropriate index into the drive symbols IDB_DRIVE_TYPE_AND_STATUS bitmap/toolbar/imagelist
// On-line drive symbols have even indices, off-line have odd indices
// Shared devices begin at 18.

#if 0  // RP
int cat_get_image_index(enum DataSourceTypeEnum eDSType, bool bOnline, bool bShared)
{
   return (2  * eDSType) + (bShared ? 18 : 0) + (bOnline ? 0 : 1);
}
#endif

// Static support routine to obtain HDDATA value from registry
CString sm_get_hd_data_from_registry()
{
   char hd_data[PRM_MAX_VALUE_LENGTH+1];

// if (PRM_get_value(PRM_HD_DATA, hd_data) != SUCCESS)
// ERR_report("Failed getting hard disk data path.");
   // the block of code above was replaced by this in order to eliminate the use of PRM_get_value
   CString reg_string = PRM_get_registry_string("Main", PRM_HD_DATA);
   strncpy_s(hd_data, PRM_MAX_VALUE_LENGTH+1, reg_string, PRM_MAX_VALUE_LENGTH);

   return hd_data;
}

// Lock or unlock the SM.  While locked cannot draw, select, etc.
void sm_lock(int iLock)   // (locks can be nested)
{
   switch (iLock)
   {
      case SM_COPYLOCK:
         ++g_sm_copy_lock_count;
         break;
      case SM_COPYUNLOCK:
         --g_sm_copy_lock_count;
         break;
      case SM_ALLLOCK:
         ++g_sm_all_lock_count;
         break;
      case SM_ALLUNLOCK:
         --g_sm_all_lock_count;

         // If the last unlock invalidate the overlay.
         // This is necessary because locked draw messages
         // may have occured that validated a blank display.
         if (g_sm_all_lock_count == 0)
         {
            SceneCoverageOverlay *overlay = sm_get_coverage_overlay();
            if (overlay)
               overlay->InvalidateOverlay();
         }
         break;
   }
}

// Check if SM locked.  While locked cannot draw or select but can process msgs
BOOL sm_is_sm_all_locked(void)   // (locks can be nested)
{
   return (g_sm_all_lock_count != 0);  // anything non-zero is locked
}

// Check if SM drawok is locked.  While locked cannot select, process msgs but can draw.
BOOL sm_is_sm_either_locked(void)   // (locks can be nested)
{
   return (g_sm_all_lock_count != 0 || g_sm_copy_lock_count != 0);  // anything non-zero is locked
}

/* ===========================================================================
 * SceneCoverageOverlay Class Members
 * ===========================================================================
 */
// Invalidate the overlay when switching into or out of the catalog tab
void SceneCoverageOverlay::InvalidateAsNeeded(void) 
{
   static CSMSheet::SMActive_t last_active_page = CSMSheet::cMANAGE;
   CSMSheet::SMActive_t current_active_page = sm_getSMSheet()->GetSMPageActive();

   last_active_page = current_active_page;
};

void SceneCoverageOverlay::InvalidateOverlay(void) 
{
   MarkCoverageAsDirty(TRUE);
   OVL_get_overlay_manager()->invalidate_all(FALSE);  // TRUE erases screen
}

void SceneCoverageOverlay::InvalidateOverlay(LPCRECT pRect) 
{
   MarkCoverageAsDirty(TRUE);
   OVL_get_overlay_manager()->invalidate_rect(pRect, FALSE);  // erases rect
};

// is the point above a file icon
C_icon *SceneCoverageOverlay::hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point)
{
   // tool tips and help text are implemented in on_mouse_moved

   return NULL;
}


// Inform the coverage overlay that it is about to be closed, if files are
// selected the user will be warned that their selection will be lost.  If
// the user cancels the close this fucntion will return cancel == TRUE.
int SceneCoverageOverlay::pre_close(boolean_t *cancel)
{
   if (cancel)
      *cancel = FALSE;

   return SUCCESS;
}

// Completes a file selection.
void SceneCoverageOverlay::drop(ViewMapProj* map, CPoint point, UINT flags)
{
   if (sm_is_sm_either_locked())   // If SM is locked, ignore message
      return;

   CWaitCursor wait;
   CClientDC dc(map->get_CView());
   map_projection_utils::CMapProjWrapper mapProjWrapper(map->GetSettableMapProj());

   BOOL bRefreshOverlay = FALSE;
   CRect affectedBounds;
   CRect affectedBoundsLeft;
   CRect affectedBoundsRight;
   int iSelectCode = SM_UNSELECT_ALL; // Default behavior
   BOOL bUnselecting = FALSE;
   CGeoRect selectBbox;
   CRect rect;

   if (flags & MK_CONTROL)
      bUnselecting = TRUE;

   // Only allow selecting in first instance of FalconView
   if (UTL_get_instance() != 1)
   {
      AfxMessageBox("The Scene Data Manager may only be used to view scene data "
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

   selectBbox.MapFromDeviceWindow(&mapProjWrapper, &rect);

   // Add scenes contained within the rectangle to the list of scenes to be
   // copied or deleted
   if (SDSWrapper::GetInstance()->SelectByGeoRect(&iSelectCode, selectBbox,
      s_scene_type_vector, m_SourceSet, bUnselecting ? true : false,
      m_bSelectMode) != SUCCESS)
   {
      ERR_report("SceneCoverageOverlay::drop(): SDSWrapper::SelectByGeoRect() failed.");
   }

   if (iSelectCode == SM_ERRSELECT_ATTEMPTDELETESOURCE)
   {
      AfxMessageBox(
         "You are attempting to select red source files for deleting.\n"
         "Only green target files can be selected for removal.\n\n"
         "Use a different tool to select red source files."
      );
   }
   else if (iSelectCode == SM_ERRSELECT_ATTEMPTCOPYTARGET)
   {
      AfxMessageBox(
         "You are attempting to select green target files for copying.\n"
         "Only red source files can be selected for copying.\n\n"
         "Use a different tool to select green target files."
      );
   }

   bRefreshOverlay = TRUE;

err_exit:

   RefreshSMDialog();

   if (m_bSMautohide && !m_bSMvisible)
   {
      ShowSMDialog(TRUE);
      m_bSMautohide = FALSE;
   }

   if (bRefreshOverlay)
   {
      // if showing regions invalidate affected area
      if (m_bDrawAsRegion)
      {
         switch (iSelectCode)
         {
         case SM_SELECT_SOURCE:
         case SM_SELECT_TARGET:
         case SM_UNSELECT_SOURCE:
         case SM_UNSELECT_TARGET:
         case SM_UNSELECT_ALL:
            InvalidateOverlay();    // invalidate all when unselecting
            break;
         }
      }
      else if (sm_getDrawBrushStyle() == SM_DRAWBRUSHSTYLE_TRANSPARENT)
      {
         // when using transparency we must invalidate when unselecting cause
         // redrawing just OR's in new pixels which cannot be seen.
         switch (iSelectCode)
         {
         case SM_SELECT_SOURCE:  // Mark as dirty and force a redraw without erasing screen
         case SM_SELECT_TARGET:
            MarkCoverageAsDirty(TRUE);
            OVL_get_overlay_manager()->draw_from_overlay(this);
            break;
         case SM_UNSELECT_SOURCE:
         case SM_UNSELECT_TARGET:
         case SM_UNSELECT_ALL:
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
void SceneCoverageOverlay::cancel_drag(ViewMapProj* map)
{
   CClientDC dc(map->get_CView());
   m_dragger->on_cancel_drag(map, &dc);
   m_is_dragging = false;
}

// Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
boolean_t SceneCoverageOverlay::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT /*nRepCnt*/,
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


void SceneCoverageOverlay::SelectAllDisplayedData()
{
   CWaitCursor wait;

   int iSelectCode = SM_UNSELECT_ALL; // Default behavior

   // Make the selection
   if (SDSWrapper::GetInstance()->SelectAllByDS(&iSelectCode,
      s_scene_type_vector, m_SourceSet, m_bSelectMode) != SUCCESS)
   {
      ERR_report("SceneCoverageOverlay::SelectAllVisibleData(): "
         "SDSWrapper::SelectAllByDS() failed.");
   }

   // Notify the user of any selection error
   if (iSelectCode == SM_ERRSELECT_ATTEMPTDELETESOURCE)
   {
      AfxMessageBox(
         "You are attempting to select red source files for deleting.\n"
         "Only green target files can be selected for removal.\n\n"
         "Use a different tool to select red source files."
      );
   }
   else if (iSelectCode == SM_ERRSELECT_ATTEMPTCOPYTARGET)
   {
      AfxMessageBox(
         "You are attempting to select green target files for copying.\n"
         "Only red source files can be selected for copying.\n\n"
         "Use a different tool to select green target files."
      );
   }

   RefreshSMDialog();

   if (m_bSMautohide && !m_bSMvisible)
   {
      ShowSMDialog(TRUE);
      m_bSMautohide = FALSE;
   }

   // if showing regions invalidate affected area
   if (m_bDrawAsRegion)
   {
      switch (iSelectCode)
      {
      case SM_SELECT_SOURCE:
      case SM_SELECT_TARGET:
      case SM_UNSELECT_SOURCE:
      case SM_UNSELECT_TARGET:
      case SM_UNSELECT_ALL:
         InvalidateOverlay();    // invalidate all when unselecting
         break;
      }
   }
   else if (sm_getDrawBrushStyle() == SM_DRAWBRUSHSTYLE_TRANSPARENT)
   {
      // when using transparency we must invalidate when unselecting cause
      // redrawing just OR's in new pixels which cannot be seen.
      switch (iSelectCode)
      {
      case SM_SELECT_SOURCE:  // Mark as dirty and force a redraw without erasing screen
      case SM_SELECT_TARGET:
         MarkCoverageAsDirty(TRUE);
         OVL_get_overlay_manager()->draw_from_overlay(this);
         break;
      case SM_UNSELECT_SOURCE:
      case SM_UNSELECT_TARGET:
      case SM_UNSELECT_ALL:
         InvalidateOverlay();    // invalidate all when unselecting
         break;
      }
   }
   else  // Mark as dirty and force a redraw without erasing screen
   {
      MarkCoverageAsDirty(TRUE);
      OVL_get_overlay_manager()->draw_from_overlay(this);
   }

   FVW_SetHelpText(""); // remove any pending messages shown in status bar.
}


// Unselects all.
void SceneCoverageOverlay::ClearSelection(long lDataSource)
{
   if ( (lDataSource == -1) && (s_scene_type_vector.size() == 0))  // All DS's and all map series
   {
      SDSWrapper::GetInstance()->UnselectAllTiles();  // this is the fast way.
      return;
   }

   // s_scene_type_vector is a <vector> of scene types that are selected in the SM list ctrl.
   std::vector<SDSSceneType*> vecSceneTypes;
   if (s_scene_type_vector.size() == 0)  // nothing in list ctrl -- must do all map series
      SDSWrapper::GetInstance()->GetAllSceneTypes(&vecSceneTypes, lDataSource);
   else
      vecSceneTypes = s_scene_type_vector;

#if 0  // RP
   CString strLastHandler;
   IDSafeArray saMapSeries;

   for (UINT i = 0; i < vecSceneTypes.size(); i++)
   {
      SDSSceneType *pSceneType = vecSceneTypes.at(i);

      CString strMapHandlerName = pSceneType->GetMapHandlerName();

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
#endif

   InvalidateOverlay();
}

// ==========================================================

int SceneCoverageOverlay::GetSelectMode()
{
   return m_bSelectMode;
}

void SceneCoverageOverlay::SetSelectMode(int mode)
{
   m_bSelectMode = mode;
}

boolean_t SceneCoverageOverlay::AreMultipleSceneTypesSelected()
{
   return s_scene_type_vector.size() > 1;
}

void SceneCoverageOverlay::SetSources(std::vector<long>* p)
{
   // Save original set of source ID's to see if set changes
   // This process assumes sets are ordered the same...
   BOOL bChanged = FALSE;
   std::vector<long> setSaveIDArray;
   GetSources(&setSaveIDArray);

   // Copy members of set that are online
   m_SourceSet.clear();
   for (unsigned int i = 0; i < p->size(); ++i)
   {
      m_SourceSet.push_back(p->at(i));
      if (i < setSaveIDArray.size() && p->at(i) != setSaveIDArray[i])
         bChanged = TRUE;  // value changed, invalidate
   }

   if (m_SourceSet.size() != setSaveIDArray.size())
      bChanged = TRUE;  // array size changed, invalidate

   // if set has been changed, invalidate
   if (bChanged)
      InvalidateOverlay();
};

void SceneCoverageOverlay::SetAllSources(void)
{
   std::vector<long> data_sources;
   if (SDSWrapper::GetInstance()->GetDataSourceIds(&data_sources) != SUCCESS)
   {
      ERR_report("SceneCoverageOverlay::SetAllSources(): "
         "SDSWrapper::GetDataSourceIds() failed.");
   }
   
   SetSources(&data_sources);  // call SetSources to filter for online
};

void SceneCoverageOverlay::HideAllSources(void)
{
   std::vector<long> setIDArray;  // empty set
   SetSources(&setIDArray);  // call SetSources to filter for online
};

void SceneCoverageOverlay::GetSources(std::vector<long>* p)
{ 
   // Copy all members of set
   p->clear();
   for(auto it = m_SourceSet.begin(); it != m_SourceSet.end(); it++)
      p->push_back(*it);
};

void SceneCoverageOverlay::RemoveSource(long id)
{
   for(auto it = m_SourceSet.begin(); it != m_SourceSet.end(); it++)
   {
      if (*it == id)
      {
         m_SourceSet.erase(it);
         return;
      }
   }
}

void SceneCoverageOverlay::AddSource(long id)
{
   for(auto it = m_SourceSet.begin(); it != m_SourceSet.end(); it++)
   {
      if (*it == id) return; // Sanity Check
   }

   m_SourceSet.push_back(id);
}

void SceneCoverageOverlay::SetActiveSMSheet(CSMSheet::SMActive_t page)
{
   CSMSheet* pSheet = sm_getSMSheet();
   if (pSheet != NULL)
   {
      pSheet->SetSMPageActive(page);
   }
}



//=============================================================================
// Copy and Delete selected files of current maptype set
int SceneCoverageOverlay::CopySelectedFiles(HWND parent_window)
{
   CWaitCursor wait;
   int status = FAILURE;

   ShowSMDialog(FALSE);

   if (SDSWrapper::GetInstance()->CopyAndDelete(parent_window) != SUCCESS)
   {
      ERR_report("SceneCoverageOverlay::CopySelectedFiles(): "
         "SDSWrapper::CopyAndDelete() failed.");
   }
   else
   {
      SDSWrapper::GetInstance()->UnselectAllTiles();
   }

   SetAllSources();
   InvalidateOverlay();

   ShowSMDialog(TRUE);
   RefreshSMDialog();

   return status;
}

/* static */ void SceneCoverageOverlay::PopulateSDSSceneTypes()
{
   // Note that since scene type long values are not guaranteed to be the same from one instance
   // of FalconView to the next, then this method may sometimes return a blank map type vector.
   // In general, though, the user will retain the same selection of scene types between sessions.

   // Create a dictionary of known scene types

   std::vector<SDSSceneType*> scene_type_vector;
   if (SDSWrapper::GetInstance()->GetAllSceneTypes(&scene_type_vector) != SUCCESS)
   {
      ERR_report("SceneCoverageOverlay::PopulateSDSSceneTypes(): "
         "SDSWrapper::GetAllSceneTypes() failed.");
   }

   std::map<SDSSceneType*, long> scene_type_map;
   for (size_t i = 0; i < scene_type_vector.size(); i++)
   {
      scene_type_map[scene_type_vector.at(i)] = 1;
   }

   // Retrieve scene types from the registry string and validate that they all exist

   s_scene_type_vector.clear();

   CString registry_string = PRM_get_registry_string("Scene Data Manager",
      "SM Displayed Scene Types");

   while (registry_string.GetLength() > 0)
   {
      int space_position = registry_string.Find(' ');

      if (space_position == -1) // There are no map types or registry_string bad
      {
         RemoveAllSDSSceneTypes();
         return;
      }

      CString scene_type_string = registry_string.Left(space_position);
      registry_string = registry_string.Right(registry_string.GetLength() 
         - space_position - 1);

      if (atol(scene_type_string + '1') < 1) // registry_string is bad
      {
         RemoveAllSDSSceneTypes();
         return;
      }

      SDSSceneType *scene_type = SDSSceneType::GetByIndex(atol(scene_type_string));
      if (scene_type_map.find(scene_type) == scene_type_map.end()) // The map type is unknown
      {
         RemoveAllSDSSceneTypes();
         return;
      }

      s_scene_type_vector.push_back(scene_type);
   }
}

/* static */ void SceneCoverageOverlay::RemoveAllSDSSceneTypes()
{
   s_scene_type_vector.clear();
   PRM_set_registry_string("Scene Data Manager", "SM Displayed Scene Types", "");
}

/* static */ void SceneCoverageOverlay::AddSDSSceneType(SDSSceneType *scene_type)
{
   s_scene_type_vector.push_back(scene_type);

   const int SCENE_TYPE_STRING_LEN = 100;
   char scene_type_string[SCENE_TYPE_STRING_LEN];
   _ltoa_s(scene_type->GetSceneTypeId(), scene_type_string,
      SCENE_TYPE_STRING_LEN, 10);
   CString registry_string = PRM_get_registry_string("Scene Data Manager",
      "SM Displayed Scene Types");
   registry_string += CString(scene_type_string) + " ";
   PRM_set_registry_string("Scene Data Manager", "SM Displayed Scene Types",
      registry_string);
}

/* static */ SDSSceneType *SceneCoverageOverlay::GetOneSDSSceneType()
{
   if (s_scene_type_vector.size() > 0) return s_scene_type_vector.at(0);
   return NULL;
}

/* static */ std::vector<SDSSceneType*> *SceneCoverageOverlay::GetSDSSceneTypes()
{
   return &s_scene_type_vector;
}


// Get available free space from target drive
__int64 SceneCoverageOverlay::GetFreeSpaceMB()
{
   std::string sTargetPath;
   if (SDSWrapper::GetInstance()->GetTargetSourcePath(&sTargetPath) != SUCCESS)
   {
      ERR_report("CoverageOverlay::GetFreeSpaceMB(): "
         "MDSWrapper::GetTargetSourcePath() failed.");
      sTargetPath = "C:\\";
   }

   return GetPathFreeSpace(sTargetPath.c_str());
}

#if 0  // RP
// Get available free space from target drive and format into string
CString SceneCoverageOverlay::GetFreeSpaceMBAsString(void)
{
   CMBString sMB;

   return sMB.FormatMB(static_cast<double>(GetFreeSpaceMB()));
}
#endif

};  // namespace scene_mgr