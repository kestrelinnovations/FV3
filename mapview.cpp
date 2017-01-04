// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/mapview.h"

// system includes
#include <mmsystem.h>
#include "FvCustomDropTarget.h"

// third party files
#include "geo3/geotrans.h"
#include "osgEarth/Capabilities"
#include "osgEarth/Registry"

// other FalconView headers

// this project's headers
#include "FalconView/commandline.h"
#include "FalconView/CustomMapGroups.h"
#include "FalconView/dted_tmr.h"
#include "FalconView/favorite.h"  // AddFavoriteDlg, OrgFavoriteDlg
#include "FalconView/FvMapViewImpl.h"
#include "FalconView/GeospatialView.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/GoToPropSheet.h"
#include "FalconView/idialognotify.h"
#include "FalconView/include/appinfo.h"
#include "FalconView/include/FctryLst.h"
#include "FalconView/include/fvwutil.h"
#include "FalconView/include/mapx.h"
#include "FalconView/include/OverlayElements.h"
#include "FalconView/include/OvlElementContainer.h"
#include "FalconView/include/showrmk.h"
#include "FalconView/include/shp.h"
#include "FalconView/include/wm_user.h"
#include "FalconView/LayerOvl.h"  // for CLayerOvl
#include "FalconView/mapdoc.h"
#include "FalconView/MapEngineCOM.h"
#include "FalconView/MapEngineOSG.h"
#include "FalconView/MapGoToPropPage.h"
#include "FalconView/MapOptionsDialog.h"
#include "FalconView/MapRenderingOnTiles.h"
#include "FalconView/MapViewTabsCtrl.h"
#include "FalconView/MapViewTabPrefs.h"
#include "FalconView/MouseClickResource.h"
#include "FalconView/MovingMapOverlay/factory.h"
#include "FalconView/overflow.h"  // MenuOverflow
#include "FalconView/PhotoGeotaggingToolsDialog.h"
#include "FalconView/PlaybackDialog/viewtime.h"
#include "FalconView/scradjst.h"
#include "FalconView/shp/factory.h"
#include "FalconView/Splash.h"
#include "FalconView/StatusBarInfoPane.h"
#include "FalconView/TabularEditor/FVTabCtrl.h"
#include "FalconView/tabulareditor/targetgraphicsdlg.h"
#include "FalconView/tiptimer.h"
#include "FalconView/utils/AppExpiration.h"
#include "FalconView/UIThreadOperation.h"
#include "FalconView/VerticalViewProperties.h"
#include "FalconView/VerticalViewDisplay.h"
#include "SecurityLabelExtensionCOM.h"
#include "FalconView/RenderFunctions.h"

// Chained message handlers
#ifdef GOV_RELEASE
#include "FalconView/ar_edit/TrackOrbitCommandMessageHandler.h"
#include "FalconView/pointex/PointExportCommandMessageHandler.h"
#endif
#include "FalconView/catalog/MapDataCoverageCommandMessageHandler.h"
#include "FalconView/scene_mgr/SMCommandMessageHandler.h"
#include "FalconView/localpnt/PointsCommandMessageHandler.h"
#include "FalconView/MovingMapOverlay/MovingMapCommandMessageHandler.h"
#include "FalconView/nitf/TacticalImageryCommandMessageHandler.h"
#include "FalconView/PrintToolOverlay/PageLayoutCommandMessageHandler.h"
#include "FalconView/SkyViewOverlay/SkyViewCommandMessageHandler.h"

namespace
{
const char* CENTER_CROSSHAIR_FILENAME = "\\icons\\system\\center-crosshair.png";

const unsigned int MAP_RENDERING_ENGINE_COM_INDEX = 0;
const unsigned int MAP_RENDERING_ENGINE_OSG_INDEX = 1;

const long BLANK_GROUP_ID = 1;
const long RASTER_GROUP_ID = 6;
const long ELEVATION_GROUP_ID = 7;
}

ViewMapProjImpl* MapView::get_curr_map()
{
   ASSERT(m_map_engine);
   return m_view_map_proj;
}

void MapView::set_current_map_invalid()
{
   // This method is specific to the 2D rendering engine only
   if (m_current_map_rendering_engine == m_map_engine)
   {
      m_map_engine->SetCurrentMapInvalid();
      invalidate_view();
   }
}

OverlayElements* MapView::CreateOverlayElements()
{
   return m_geospatial_view_controller->CreateOverlayElements();
}

// #defines

// TEST_INVALIDATION should be set to 0 for production code.  Setting it to 1
// causes the screen to always be erased when an invalidation done which makes
// it easier to tell what is being invalidated when and where.  This makes it
// easier to spot invalidation inefficiencies and problems.
#define TEST_INVALIDATION 0

// If LOG_INVALIDATIONS defined as 1, the invalidate_view... functions will
// output INFO_printfs for testing purposes.
#define LOG_INVALIDATIONS 0


#define BRIGHTNESS_LIMIT   0.8
#define CONTRAST_LIMIT  0.8
#define VK_E   0x45
#define VK_F 0x46
#define VK_L 0x4C

// static vars
boolean_t MapView::m_get_position_from_map = FALSE;
int MapView::m_get_position_from_map_type;
int MapView::m_get_position_from_map_index;
boolean_t MapView::m_scrolling = FALSE;
#ifdef GOV_RELEASE
int MapView::m_security_label_on_title_bar =
   PRM_get_registry_int("Security", "DisplayLabelOnTitleBar", 1);
CString MapView::m_current_security_class =
   PRM_get_registry_string("Security", "CurrentClass", "STANDARD");
CString MapView::m_mru_standard_label =
   PRM_get_registry_string("Security", "MRUStandardClass",
   "LIMITED DISTRIBUTION");
#else
int MapView::m_security_label_on_title_bar =
   PRM_get_registry_int("Security", "DisplayLabelOnTitleBar", 0);
CString MapView::m_current_security_class =
   PRM_get_registry_string("Security", "CurrentClass", "NONE");
CString MapView::m_mru_standard_label =
   PRM_get_registry_string("Security", "MRUStandardClass", "UNCLASSIFIED");
#endif
CString MapView::m_custom_label =
   PRM_get_registry_string("Security", "MRUCustomClass", "");
MapEngineCOM *MapView::pActiveViewMapEngine = NULL;

std::vector<CCmdTarget* > MapView::s_command_message_handlers;

// MapView

IMPLEMENT_DYNCREATE(MapView, CView)

BEGIN_MESSAGE_MAP(MapView, CView)
   //{{AFX_MSG_MAP(MapView)

   ON_WM_CREATE()
   ON_WM_DESTROY()
   ON_WM_ERASEBKGND()
   ON_WM_SETCURSOR()
   ON_WM_SIZE()
   ON_WM_TIMER()

   ON_COMMAND(ID_MAP_OPTIONS, OnMapOptions)

   ON_COMMAND(ID_MAP_GROUPS, OnMapGroups)
   ON_UPDATE_COMMAND_UI(ID_MAP_GROUPS, OnUpdateMapGroups)

   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_RBUTTONDOWN()
   ON_WM_RBUTTONUP()
   ON_WM_LBUTTONDBLCLK()
   ON_MESSAGE(WM_MOUSEWHEEL, OnMouseWheel)
   
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()

   ON_COMMAND(ID_BRIGHT, OnBright)
   ON_UPDATE_COMMAND_UI(ID_BRIGHT, OnUpdateBright)

   ON_COMMAND(ID_DIM, OnDim)
   ON_UPDATE_COMMAND_UI(ID_DIM, OnUpdateDim)

   ON_COMMAND(ID_MAP_ROTATE_NORTHUP, OnMapRotateNorthUp)
   ON_COMMAND(ID_ROTATE_NORTHUP, OnMapRotateNorthUp)
   ON_UPDATE_COMMAND_UI(ID_MAP_ROTATE_NORTHUP, OnUpdateMapRotateNorthup)

   ON_COMMAND(ID_MAP_PAN_DOWN, OnMapPanDown)
   ON_COMMAND(ID_MAP_PAN_LEFT, OnMapPanLeft)
   ON_COMMAND(ID_MAP_PAN_RIGHT, OnMapPanRight)
   ON_COMMAND(ID_MAP_PAN_UP, OnMapPanUp)

   ON_COMMAND(ID_MAP_ROTATE_CLOCKWISE, OnMapRotateClockwise)
   ON_UPDATE_COMMAND_UI(ID_MAP_ROTATE_CLOCKWISE, OnUpdateMapRotateClockwise)

   ON_COMMAND(ID_MAP_ROTATE_COUNTERCLOCKWISE, OnMapRotateCounterclockwise)
   ON_UPDATE_COMMAND_UI(ID_MAP_ROTATE_COUNTERCLOCKWISE, OnUpdateMapRotateCounterclockwise)

   ON_COMMAND(ID_SCALE_PERCENT_ZOOM, OnScalePercentZoom)
   ON_UPDATE_COMMAND_UI(ID_SCALE_PERCENT_ZOOM, OnUpdateScalePercentZoom)

   ON_COMMAND(ID_SCALE_ZOOM, OnScaleZoom)
   ON_UPDATE_COMMAND_UI(ID_SCALE_ZOOM, OnUpdateScaleZoom)

   ON_COMMAND(ID_HAND_MODE, OnHandMode)
   ON_UPDATE_COMMAND_UI(ID_HAND_MODE, OnUpdateHandMode)

   ON_COMMAND(ID_MAP_SCALEIN, OnScaleIn)
   ON_COMMAND(ID_MAP_SCALEOUT, OnScaleOut)

   ON_COMMAND(ID_MAP_NEW, OnMapNew)  // Map Settings (Go To) dialog

   ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
   ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrintDirect)

   ON_UPDATE_COMMAND_UI(ID_MAP_PAN_DOWN, OnMapPanUpdate)
   ON_UPDATE_COMMAND_UI(ID_MAP_PAN_LEFT, OnMapPanUpdate)
   ON_UPDATE_COMMAND_UI(ID_MAP_PAN_RIGHT, OnMapPanUpdate)
   ON_UPDATE_COMMAND_UI(ID_MAP_PAN_UP, OnMapPanUpdate)
   
   ON_COMMAND(ID_INCREASE_CONTRAST, OnIncreaseContrast)
   ON_UPDATE_COMMAND_UI(ID_INCREASE_CONTRAST, OnUpdateIncreaseContrast)

   ON_COMMAND(ID_DECREASE_CONTRAST, OnDecreaseContrast)
   ON_UPDATE_COMMAND_UI(ID_DECREASE_CONTRAST, OnUpdateDecreaseContrast)

   ON_COMMAND(ID_VIEW_ADJUST_BRIGHT_CONTRAST, OnViewAdjustBrightContrast)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ADJUST_BRIGHT_CONTRAST, OnUpdateViewAdjustBrightContrast)

   ON_COMMAND(ID_VIEW_DECREASECONTRASTCENTERVALUE, OnViewDecreaseContrastCenterValue)

   ON_COMMAND(ID_VIEW_INCREASECONTRASTCENTERVALUE, OnViewIncreaseContrastCenterValue)
   
   ON_COMMAND(ID_VIEW_NORMAL_BRIGHT_CONTRAST, OnViewNormalBrightContrast)
   ON_UPDATE_COMMAND_UI(ID_VIEW_NORMAL_BRIGHT_CONTRAST, OnUpdateViewNormalBrightContrast)

   ON_COMMAND(ID_VIEW_AUTO_BRIGHT_CONTRAST, OnViewAutoBrightContrast)
   ON_UPDATE_COMMAND_UI(ID_VIEW_AUTO_BRIGHT_CONTRAST, OnUpdateViewAutoBrightContrast)

   ON_COMMAND(ID_VIEW_CALC_BRIGHT_CONTRAST, OnViewCalcBrightContrast)
   ON_UPDATE_COMMAND_UI(ID_VIEW_CALC_BRIGHT_CONTRAST, OnUpdateViewCalcBrightContrast)

   
   ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
   ON_COMMAND(ID_MAIN_PRINT_DIRECT, OnMainPrintDirect)
   ON_UPDATE_COMMAND_UI(ID_MAIN_PRINT_DIRECT, OnMainPrintDirectUpdate)
   ON_COMMAND(ID_MAIN_PRINT_PREVIEW, OnMainPrintPreview)
   ON_UPDATE_COMMAND_UI(ID_MAIN_PRINT_PREVIEW, OnMainPrintPreviewUpdate)
   ON_COMMAND(ID_MAIN_PRINT, OnMainPrint)
   ON_UPDATE_COMMAND_UI(ID_MAIN_PRINT, OnMainPrintUpdate)
   
   ON_COMMAND(ID_CENTER_CROSSHAIR, OnToggleCenterCrosshair)
   ON_UPDATE_COMMAND_UI(ID_CENTER_CROSSHAIR, OnUpdateCenterCrosshair)

   ON_COMMAND(IDC_SEARCH_BAR, OnSearchBar)

   ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
   // overlay popup menu
   ON_COMMAND_RANGE(ID_OVERLAY_POPUP, ID_OVERLAY_POPUP_MAX, OnOverlayPopup)
   ON_UPDATE_COMMAND_UI_RANGE(ID_OVERLAY_POPUP, ID_OVERLAY_POPUP_MAX, OnOverlayPopupUpdate)
   // overlay commands
   ON_COMMAND_RANGE(ID_OVERLAY, ID_OVERLAY9, OnOverlay)
   ON_UPDATE_COMMAND_UI_RANGE(ID_OVERLAY, ID_OVERLAY9, OnUpdateOverlay)
   ON_COMMAND(ID_EDIT_COORD_COPY, OnCoordCopy)

   ON_COMMAND(ID_EDIT_UNDO, OnUndo)
   ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)

   ON_COMMAND(ID_EDIT_REDO, OnRedo)
   ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRedo)

   ON_MESSAGE(WM_INVALIDATE_FROM_THREAD, OnInvalidateFromThread)
   ON_MESSAGE(WM_INVALIDATE_VIEW, OnInvalidate)
   ON_MESSAGE(WM_INVALIDATE_LAYER_OVERLAY, OnInvalidateLayerOverlay)

   ON_COMMAND(ID_EDIT_COPYMAP, OnEditCopyMap)
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_MAP_TO_GEOTIFF,
      OnUpdateEditCopyMapDisableRotated)
   ON_COMMAND(ID_EDIT_COPY_MAP_TO_KMZ, OnEditCopyMapToKMZ)
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_MAP_TO_KMZ,
      OnUpdateEditCopyMapDisableRotated)
   ON_COMMAND(ID_EDIT_COPY_MAP_TO_GEOTIFF, OnEditCopyMapToGeoTIFF)
   ON_COMMAND(ID_EDIT_COPY_MAP_TO_JPEG, OnEditCopyMapToJPG)
   ON_COMMAND(ID_EDIT_COPY_MAP_TO_PNG, OnEditCopyMapToPNG)

   ON_COMMAND_RANGE(IDRANGE_OVERLAY_VIEW_MENU_1, IDRANGE_OVERLAY_VIEW_MENU_500,
      CMainFrame::OnOverlayManagerViewList)
   ON_UPDATE_COMMAND_UI_RANGE(IDRANGE_OVERLAY_VIEW_MENU_1,
      IDRANGE_OVERLAY_VIEW_MENU_500,
      CMainFrame::OnUpdateOverlayManagerViewList)

   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)

   ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)

   // Favorites->Add to Favorites
   ON_COMMAND(ID_FAVORITES_ADD, OnFavoritesAdd)

   // Favorites->Organize Favorites
   ON_COMMAND(ID_FAVORITES_ORG, OnFavoritesOrganize)

   ON_UPDATE_COMMAND_UI(ID_FAVORITES_LIST, OnUpdateFavoritesList)
   ON_UPDATE_COMMAND_UI(ID_OVERLAY_LIST, OnUpdateOverlayList)
   ON_UPDATE_COMMAND_UI(IDC_MAP_MENU, OnUpdateMapMenu)
   ON_WM_SETFOCUS()
   ON_WM_KILLFOCUS()

END_MESSAGE_MAP()

using scene_mgr::SMCommandMessageHandler;

#ifdef GOV_RELEASE
CHAINED_MAPVIEW_MESSAGE_MAP(PointExportCommandMessageHandler)
CHAINED_MAPVIEW_MESSAGE_MAP(TrackOrbitCommandMessageHandler)
#endif
CHAINED_MAPVIEW_MESSAGE_MAP(MapDataCoverageCommandMessageHandler)
CHAINED_MAPVIEW_MESSAGE_MAP(SMCommandMessageHandler)
CHAINED_MAPVIEW_MESSAGE_MAP(MovingMapCommandMessageHandler)
CHAINED_MAPVIEW_MESSAGE_MAP(PageLayoutCommandMessageHandler)
CHAINED_MAPVIEW_MESSAGE_MAP(PointsCommandMessageHandler)
CHAINED_MAPVIEW_MESSAGE_MAP(SkyViewCommandMessageHandler)
CHAINED_MAPVIEW_MESSAGE_MAP(TacticalImageryCommandMessageHandler)

/* static */
void MapView::AddCommandMessageHandler(CCmdTarget* message_handler)
{
   s_command_message_handlers.push_back(message_handler);
}

BOOL MapView::OnCmdMsg(UINT nID, int nCode, void* pExtra,
   AFX_CMDHANDLERINFO* pHandlerInfo)
{
   // Give this class the first crack at handling the message
   if (CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
   {
      return TRUE;
   }

   // Next, give the chained message handlers a chance
   static CCmdTarget** start = s_command_message_handlers.data();
   
   CCmdTarget** current = start;
   size_t size = s_command_message_handlers.size();
   while (size--)
   {
      if ((*current)->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
      {
         return TRUE;
      }
      ++current;
   }
   
   return FALSE;
}

// MapView construction/destruction

CBaseView::CBaseView() :
   m_pTipTimer(NULL),
   m_bTooltipWasSet(FALSE),
   m_tool_tip_point(-1, -1)
{
}

MapView::MapView() :
   m_pContextMenu(NULL),
   m_map_initialized(false),
   m_geospatial_view_controller(nullptr),
   m_center_crosshair_handle(-1),
   m_overlay_elements(nullptr),
   m_current_map_rendering_engine(nullptr)
{
   m_map_engine = new MapEngineCOM;

   m_view_map_proj = new ViewMapProjImpl(this);

   m_mouse_click_resource = new CMouseClickResource();

   m_hint = new HintText();

   // is a smooth scroll taking place
   m_scrolling = FALSE;
   m_hand_scroll = FALSE;
   m_hand_mode = FALSE;
   m_force_hand_scroll = FALSE;

   // are we performing a zoom window drag
   m_map_change_tool = NULL;

   m_drag = FALSE;
   m_lbutton_double_click_enable = TRUE;
   m_external_disable_rotation_ui = FALSE;
   m_external_force_equal_arc = FALSE;
   m_external_disable_projection_ui = FALSE;
   m_external_force_north_up = FALSE;
   m_zoom_enabled = TRUE;
   m_cursor_position_geo.lat = m_cursor_position_geo.lon = 0.0;
   m_cursor_position_geo_valid = false;
   m_first_map_up = FALSE;
   m_mouse_flags = 0;
   m_cursor = AfxGetApp()->LoadCursor(IDC_CROSSHAIR);
   m_move_cursor = FALSE;
   m_left_button_down = FALSE;
   m_lose_mouse_click = FALSE;
   m_popup = FALSE;
   m_user_interrupted_draw=FALSE;
   m_is_drawing = FALSE;
   m_printing = FALSE;
   m_CAPS_printing = FALSE;
   m_bDisplayCenterCrosshair = FALSE;
   m_pGeotagPhotosDlg = NULL;

   // make sure to specify to use the no_collapse flag
   //
   
   // Create the custom security label extension manager
   m_pSecurityLabelMgr = new CSecurityLabelExtensionCOM;
   m_pSecurityLabelMgr->Initialize();

   m_strip_page_map_list = NULL;

   m_new_map = FALSE;
   m_dted_timer = NULL;
   m_pFvMapView = NULL;

   CComObject<CFvContextMenuImpl>::CreateInstance(&m_pContextMenu);
   m_pContextMenu->AddRef();

   m_customDropTarget = new FvCustomDropTarget();
}

FalconViewOverlayLib::IFvMapView *MapView::GetFvMapView()
{
   if (m_pFvMapView == NULL)
   {
      CComObject<CFvMapViewImpl>::CreateInstance(&m_pFvMapView);
      m_pFvMapView->AddRef();

      m_pFvMapView->SetMapView(this);
   }

   return m_pFvMapView;
}

MapView::~MapView()
{
   if (m_geospatial_view_controller)
      m_geospatial_view_controller->RemoveEarthManipulatorListener(this);
   delete m_geospatial_view_controller;
   delete m_map_engine;
   delete m_map_rendering_engines[MAP_RENDERING_ENGINE_OSG_INDEX];
   delete m_view_map_proj;

   // free overlay menu list and map memory
   FreeOverlayMenu();

   if (m_pFvMapView != NULL)
      m_pFvMapView->Release();

   if (m_pContextMenu != NULL)
      m_pContextMenu->Release();

   delete m_mouse_click_resource;
   delete m_hint;
   delete m_overlay_elements;
   delete m_customDropTarget;

   // Release the security extensions manager
   delete m_pSecurityLabelMgr;

   // The timer list owns m_dted_timer. There is no need to delete it here.
}

OverlayElements* MapView::GetOverlayElements()
{
   if (!m_overlay_elements)
   {
      m_overlay_elements =
         m_geospatial_view_controller->CreateOverlayElements();
   }

   // may still be null, for example over an RDP session
   return m_overlay_elements;
}

void MapView::ResetDtedTimer(const d_geo_t& geo)
{
   // set timer for DTED elevation
   if (!m_dted_timer)  // if we don't have a timer yet...
   {
      const UINT dted_timeout = 500;  // 1/2 second
      if (!(m_dted_timer = new DTEDTimer(dted_timeout)))  // create one
         ERR_report("new DTEDTimer failed.");

      UTL_get_timer_list()->add(m_dted_timer);  // add to timer list
   }

   // reset and start timer
   m_dted_timer->Reset(geo);
}

// Override of normal context sensitive help processing in order to check for
// invalid help context.  Translate a client area click to contents screen call
LRESULT MapView::OnHelpHitTest(WPARAM, LPARAM lParam)
{
   return ID_HELP_HIT_TEST_DEFAULT;  // contents screen
}
// end of OnCommandHelp


BOOL MapView::PreCreateWindow(CREATESTRUCT& cs)
{
   ASSERT(cs.lpszClass == NULL);       // must not be specified

   cs.lpszClass = AfxRegisterWndClass(
      CS_DBLCLKS | CS_BYTEALIGNCLIENT| CS_HREDRAW | CS_VREDRAW,
      AfxGetApp()->LoadStandardCursor(IDC_ARROW),
      (HBRUSH)GetStockObject(BLACK_BRUSH));
   return TRUE;
}


// Respond to resizing
void MapView::OnSize(UINT nType, int cx, int cy)
{
   CWnd::OnSize(nType, cx, cy);

   if (m_current_map_rendering_engine)
   {
      m_current_map_rendering_engine->SetSurfaceDimensions(cx, cy);
      invalidate_view();
   }

   if (cx && cy)
   {
      // If the window has be resized and not minimized then
      // the map will have to be redrawn from scratch, i.e.
      // the map projection has changed, so the data check
      // test must be performed again.
      if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
      {
         if (m_first_map_up != FALSE &&
            m_current_map_rendering_engine == m_map_engine)
         {
            // Call MAP_set_map_type with the current type, so it will
            // perform a data check for the new window size.
            const int overide_no_data = TRUE;
            const int status = m_current_map_rendering_engine->ChangeMapType(
               GetMapGroupIdentity(), GetMapType(), TRUE);

            if (status != SUCCESS)
            {
               // don't put a message box up while in OnSize
               ChangeToClosestScale(BLANK_GROUP_ID);
            }
         }
      }
   }

   m_map_params.SetSurfaceSize(cx, cy);
}

// MapView drawing
//

void MapView::OnDraw(CDC* pDC)
{
   // If the clip box is empty there is nothing to do
   CRect clip_rect;
   int ret = pDC->GetClipBox(&clip_rect);
   if (ret == NULLREGION)
      return;

   // Invalidate 3D renderer if the clip rect is non-empty
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      m_current_map_rendering_engine->Invalidate();
      return;
   }

   // if we are already performing a draw, then let's get out of here
   if (m_is_drawing)
      return;

   // if we are in the middle of a outgoing COM call, then do not redraw
   // to avoid re-entry
   CWinThread* pThread = AfxGetThread();
   if (pThread && static_cast<CMyMessageFilter *>(pThread->m_pMessageFilter)->
         GetDispatchingPaint())
   {
      return;
   }

   __try
   {
      m_is_drawing = TRUE;
      draw(pDC);
   }
   __finally
   {
      m_is_drawing = FALSE;
   }
}

// The CDC passed in can be nullptr.  In this case, the FvGraphicsDevice
// should be used instead
void MapView::draw(CDC* pDC)
{
   CMainFrame* frame = fvw_get_frame();

   // paint the window black while in the middle of a resize - drawing the
   // map during a resize could potentially be too slow. Also, if the map
   // has not yet been initialized, there is nothing to draw.
   if ( (frame && frame->m_doing_size_or_move) || !m_map_initialized)
   {
      CWnd *pWnd = pDC->GetWindow();

      if (pWnd)
      {
         CRect rect;
         pWnd->GetClientRect(&rect);

         CBrush blackBrush(RGB(0, 0, 0));

         pDC->FillRect(rect, &blackBrush);
      }

      return;
   }

   //
   // remove tool tip before changing the display
   //
   clear_tool_tip();

   const boolean_t display_wait_cursor_when_drawing_from_scratch =
            !(m_scrolling || CMainFrame::GetPlaybackDialog().in_playback());

   //
   // draw the map
   //
   // Note that draw_map_and_animate_if_needed can return SUCCESS, FAILURE,
   // MEMORY_ERROR and FATAL_MAP_DISPLAY_ERROR
   //
   boolean_t map_drawn_from_scratch;
   int ret = m_map_engine->draw_map(pDC,
      display_wait_cursor_when_drawing_from_scratch, &map_drawn_from_scratch);

   //
   // Display an appropriate message now that a map has been drawn and
   // all of the appropriate view variables are set.  Make sure to only
   // display messages after the draw (e.g. graphics objects) has been
   // completely de-initialized.  Displaying a message box before everything
   // has been de-initialized can cause errors by introducing draw messages
   // before the current draw has been completely processed.
   //
   if (ret != SUCCESS && fvw_get_app()->is_gui_hidden() == false)
   {
      // if the first map has not yet been displayed, then we need to fall
      // back to another map
      if (m_first_map_up == FALSE)
      {
         if (m_map_engine->change_map_type(TIROS, WORLD, "TopoBath", 0.0, 0.0,
            0.0, 100, PROJ_get_default_projection_type()) != SUCCESS)
         {
            if (m_map_engine->change_map_type(BLANK_MAP, WORLD, NULL_SERIES,
               0.0, 0.0, 0.0, 100, PROJ_get_default_projection_type()) !=
               SUCCESS)
            {
               ERR_report("Unable to change to Blank Map.  Check to make sure "
                  "the Blank map handler is enabled in the DB.");
            }
         }
         invalidate_view();
         return;
      }

      // display appropriate error messages
      if (ret == MEMORY_ERROR)
      {
         AfxMessageBox("A memory allocation error has occurred.  "
            "Please save your work and exit.");
      }
      else if (ret == FATAL_MAP_DISPLAY_ERROR)
         AfxMessageBox("An error has occurred.  "
         "Please save your work and exit.");
      else if (ret == E_PREPARE_FOR_OUTPUT)
      {
         // If in the middle of printing, set a flag telling the printing stuff
         // to invalidate the screen when done.
         // Otherwise, report an error.
         if (m_printing)
         {
            m_refresh_after_printing = TRUE;
         }
         else
         {
            ERR_report("output_prepare_with_dc failed");
         }

         // need to reset draw interrupt before return
         m_user_interrupted_draw = FALSE;
      }
      else
         OnMapError(ret);

      return;
   }

   // if the map has been redrawn and the Vertical Display window is open,
   // invalidate the Vertical Display object to redraw the Vertical Display...
   if (frame)
   {
      CVerticalViewProperties *pVerticalDisplayProperties =
         frame->m_pVerticalDisplayProperties;
      if (pVerticalDisplayProperties)
      {
         CVerticalViewDisplay *pVerticalDisplay =
            pVerticalDisplayProperties->GetVerticalDisplayObject();
         if (map_drawn_from_scratch && pVerticalDisplay)
               pVerticalDisplay->Invalidate();
      }
   }


   // this should be moved to the overlay rendering
   if (m_bDisplayCenterCrosshair)
   {
      int nCenterX, nCenterY;
      int nRet = get_curr_map()->geo_to_surface(
         get_curr_map()->actual_center_lat(),
         get_curr_map()->actual_center_lon(),
         &nCenterX, &nCenterY);

      if (nRet == SUCCESS)
         DrawCenterCrosshair(pDC, nCenterX, nCenterY);
      else if (nRet != SUCCESS)
         ERR_report("Failed to retrieve center to draw crosshair");
   }

   // update geotagging dialog if opened
   if (map_drawn_from_scratch && m_pGeotagPhotosDlg != NULL)
      m_pGeotagPhotosDlg->UpdateLocation(get_curr_map()->actual_center_lat(),
      get_curr_map()->actual_center_lon());

   //
   // Reset m_redraw_overlays_from_scratch_when_animating to FALSE
   // after a draw, because it must always be FALSE at the start of a draw
   // routine
   m_map_engine->set_redraw_overlays_from_scratch_when_animating(FALSE);

   // if we are changing the map, then reset the 'tooltip is over a moving
   // object' flag.  Otherwise, the tooltip won't get erased in clear_tool_tip
   if (map_drawn_from_scratch)
   {
      m_tooltip_over_moving_object = FALSE;
   }

   // update the title bar, toolbar and status bar
   update_title_bar();

   //
   // set the CTIA text in the status bar
   //
   static CTextInfoPane *pCTIAInfoPane = static_cast<CTextInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_PANE_EFFECTIVE_SCALE));
   if (pCTIAInfoPane != NULL && map_drawn_from_scratch)
   {
      CString strScale;

      CString strType;
      double dScaleFactor = 1.0;
      if (get_curr_map()->requested_zoom_percent() == TO_SCALE_ZOOM_PERCENT &&
         get_curr_map()->source() != "TIROS")
      {
         strType = "To Scale";
         m_map_engine->GetToScaleFactor(&dScaleFactor);
      }
      else
      {
         strType = "Effective Scale";
      }

      // get the current scale's denominator
      double d;
      MAP_scale_t_to_scale_denominator(get_curr_map()->scale(), &d);

      // set the effective scale string 1:X where X is the current scale denom
      // times the zoom percent (which is stored as an integer so we divide by
      // a hundred)
      const int effective =static_cast<int>(
         d * dScaleFactor * (100.0/get_curr_map()->actual_zoom_percent()) +
         0.5);
      strScale.Format("%s: %s", strType,
         effective == WORLD_OVERVIEW ? "World" :
         MAP_convert_scale_denominator_to_scale_string(effective));

      pCTIAInfoPane->SetText(strScale);
   }

   // update default datum in the status bar
   static CTextInfoPane *pDefaultDatumInfoPane = static_cast<CTextInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_PANE_DEFAULT_DATUM));
   if (pDefaultDatumInfoPane != NULL)
   {
      const int DATUM_LEN = 6;
      char sdatum[DATUM_LEN];
      GEO_get_default_datum(sdatum, DATUM_LEN);
      pDefaultDatumInfoPane->SetText(sdatum);
   }

#ifdef GOV_RELEASE
   static CIconInfoPane *pCurrencyCadrgPane = static_cast<CIconInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_CARDG_CURRENCY));
   static CIconInfoPane *pCurrencyCadrgPaneVerbose =
      static_cast<CIconInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_CARDG_CURRENCY_VERBOSE));
   static CIconInfoPane *pCurrencyEcrgPane = static_cast<CIconInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_ECRG_CURRENCY));
   static CIconInfoPane *pCurrencyEcrgPaneVerbose =
      static_cast<CIconInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_ECRG_CURRENCY_VERBOSE));

   if (((pCurrencyCadrgPane &&
      pCurrencyCadrgPane->GetParentStatusBar() != NULL) ||
        (pCurrencyCadrgPaneVerbose &&
        pCurrencyCadrgPaneVerbose->GetParentStatusBar() != NULL) ||
        (pCurrencyEcrgPane &&
        pCurrencyEcrgPane->GetParentStatusBar() != NULL) ||
        (pCurrencyEcrgPaneVerbose &&
        pCurrencyEcrgPaneVerbose->GetParentStatusBar() != NULL))
        && map_drawn_from_scratch)
   {
      UINT nResourceIdCadrg = NULL;
      UINT nResourceIdEcrg = NULL;

      try
      {
         // get current map bounds
         d_geo_t map_ll, map_ur;
         get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);

         MapScale scale = get_curr_map()->scale();

         DATE dtCurrencyDate;

         if (get_curr_map()->actual_source() == CADRG)
         {
            ICadrgMapInformationPtr smpCadrgMapInformation;
            CO_CREATE(smpCadrgMapInformation, CLSID_CadrgMapHandler);

            ChartCurrencyEnum eCadrgCurrent =
               smpCadrgMapInformation->IsCadrgCurrent(
               scale.GetScale(), scale.GetScaleUnits(),
               _bstr_t(get_curr_map()->series().get_string()),
               map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon, &dtCurrencyDate);

            switch (eCadrgCurrent)
            {
            case CHART_CURRENCY_INVALID_CADRG_SERIES:
               nResourceIdCadrg = NULL;
               break;
            case CHART_CURRENCY_CADRG_CURRENT:
               nResourceIdCadrg = IDI_CHECK_MARK;
               break;
            case CHART_CURRENCY_CADRG_NOT_CURRENT:
               nResourceIdCadrg = IDI_CHECKING;
               break;
            }
         }
         else if (get_curr_map()->actual_source() == ECRG)
         {
            IEcrgMapInformationPtr smpEcrgMapInformation;
            CO_CREATE(smpEcrgMapInformation, CLSID_EcrgMapHandler);

            ChartCurrencyEnum eEcrgCurrent =
               smpEcrgMapInformation->IsEcrgCurrent(
               scale.GetScale(), scale.GetScaleUnits(),
               _bstr_t(get_curr_map()->series().get_string()),
               map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon, &dtCurrencyDate);

            switch (eEcrgCurrent)
            {
            case CHART_CURRENCY_INVALID_CADRG_SERIES:
               nResourceIdEcrg = NULL;
               break;
            case CHART_CURRENCY_CADRG_CURRENT:
               nResourceIdEcrg = IDI_CHECK_MARK;
               break;
            case CHART_CURRENCY_CADRG_NOT_CURRENT:
               nResourceIdEcrg = IDI_CHECKING;
               break;
            }
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("Failed determining currency: (%s)",
            (char *)e.Description());
         ERR_report(msg);
      }

      if (pCurrencyCadrgPane)
         pCurrencyCadrgPane->SetIcon(0, nResourceIdCadrg, "");

      if (pCurrencyCadrgPaneVerbose)
         pCurrencyCadrgPaneVerbose->SetIcon(
         0, nResourceIdCadrg, "CADRG Currency");

      if (pCurrencyEcrgPane)
         pCurrencyEcrgPane->SetIcon(0, nResourceIdEcrg, "");

      if (pCurrencyEcrgPaneVerbose)
         pCurrencyEcrgPaneVerbose->SetIcon(0, nResourceIdEcrg, "ECRG Currency");
   }
#endif

   //
   // if this is the first map displayed, then set a flag
   // indicating that the first map has been drawn.
   //
   // Note: we don't want to do the first draw initialization until the
   // command line has been processed.  It is possible to get into this
   // draw before the end of InitInstance.
   if (m_first_map_up == FALSE && ret == SUCCESS)
   {
      m_first_map_up = TRUE;

      // start the collaboration server for points, drawings, etc
      ICollaborationServerPtr sp;
      sp.CreateInstance(__uuidof(CollaborationServer));
      if (sp)
      {
         frame->m_pCollaborationServer = sp.Detach();
         frame->m_pCollaborationServer->Start(
            (LONG)AfxGetMainWnd()->GetSafeHwnd());
      }

      // Now that the map has been set, we can open any startup overlays
      // here the first time this is called.  This was previously done
      // in the overlay manager draw, but some overlays were doing "drawing"
      // in their open() methods that used the UTL_ functions to get
      // the view map.  Since the current map has not been bound to a
      // surface the projection was NULL and FV would crash at startup
      OVL_get_overlay_manager()->NotifyClientsFVReady();

      // [Bug 4580] make sure to set the focus back to the main window since the
      // hidden collaboration dialog can take focus
      if (frame != NULL)
         frame->SetFocus();
   }

   if (map_drawn_from_scratch && ret == SUCCESS)
   {
      m_new_map = TRUE;

      //
      // if the map has changed, the cursor will be relocated and the running
      // lat/lon will be updated iff the cursor hasn't moved since the event
      // that generated the new map display occured
      //
      CPoint position;
      GetCursorPos(&position);
      ScreenToClient(&position);
      OnMouseMove(m_mouse_flags, position);
      m_move_cursor = FALSE;

      m_new_map = FALSE;

      //
      // If the map is not being forced north-up by an "external" force,
      // then enable or disable the rotation menu items when
      // a change of state occurs.
      //
      if (!m_external_force_north_up)
      {
         // Rotation ui is enabled iff not at world scale
         //
         if (get_curr_map()->scale() != WORLD)
         {
            m_external_disable_rotation_ui = FALSE;
            enable_rotation_user_interface(TRUE);
         }
         else if (get_curr_map()->scale() == WORLD)
         {
            m_external_disable_rotation_ui = TRUE;
            enable_rotation_user_interface(FALSE);
         }
      }

      // enable or disable the zoom combo box in the toolbar when
      // a change of state occurs
      //
      const boolean_t zoom_ok_for_new_map =
         MAP_zoom_percent_valid(get_curr_map()->scale());
      // zoom always enabled for 3D
      /*if (m_zoom_enabled && !zoom_ok_for_new_map)
      {
         fvw_get_frame()->m_MainFrameToolBar->enable_zoom_combo(FALSE);
         m_zoom_enabled = FALSE;
      }
      else if (!m_zoom_enabled && zoom_ok_for_new_map)*/
      //{
         //fvw_get_frame()->m_MainFrameToolBar->enable_zoom_combo(TRUE);
         //m_zoom_enabled = TRUE;
      //}
   }

   //
   // reset user interrupt draw
   //
   m_user_interrupted_draw = FALSE;
}

void MapView::DrawCenterCrosshair(CDC *pDC, int nCenterX, int nCenterY)
{
   UtilDraw utilDraw(pDC);

   // Draw background first
   utilDraw.set_pen(RGB(0, 0, 0), UTIL_LINE_SOLID, 4);

   utilDraw.draw_line(nCenterX, nCenterY, nCenterX, nCenterY);
   utilDraw.draw_line(nCenterX, nCenterY - 5, nCenterX, nCenterY - 15);
   utilDraw.draw_line(nCenterX, nCenterY + 5, nCenterX, nCenterY + 15);
   utilDraw.draw_line(nCenterX - 5, nCenterY, nCenterX - 15, nCenterY);
   utilDraw.draw_line(nCenterX + 5, nCenterY, nCenterX + 15, nCenterY);

   // Draw foreground - base the color on the coordinate grid's color
   int nColor = PRM_get_registry_int("Lat-Long Grid Options", "Grid Line Color",
      UTIL_COLOR_WHITE);
   utilDraw.set_pen(nColor, UTIL_LINE_SOLID, 2);

   utilDraw.draw_line(nCenterX, nCenterY, nCenterX, nCenterY);
   utilDraw.draw_line(nCenterX, nCenterY - 5, nCenterX, nCenterY - 15);
   utilDraw.draw_line(nCenterX, nCenterY + 5, nCenterX, nCenterY + 15);
   utilDraw.draw_line(nCenterX - 5, nCenterY, nCenterX - 15, nCenterY);
   utilDraw.draw_line(nCenterX + 5, nCenterY, nCenterX + 15, nCenterY);
}

void MapView::OnInitialUpdate()
{
   CView::OnInitialUpdate();

   // Get the current window size
   CRect rect;
   GetWindowRect(&rect);

   const osgEarth::Capabilities& caps =
      osgEarth::Registry::instance()->getCapabilities();

   CMainFrame *frame = fvw_get_frame();

   m_3D_rendering_supported = 
      (frame->Is3DEnabledInRegistry()) && 
      ((caps.supportsGLSL() && caps.supportsMultiTexture()) ||
      (caps.supportsGLSL(1.30f) && caps.supportsTextureArrays()) ||
      (caps.supportsMultiTexture()));

   m_geospatial_view_controller = new GeospatialViewController;
   if (m_3D_rendering_supported)
   {
      m_geospatial_view_controller->Initialize(this, rect.Width(), rect.Height());
      m_geospatial_view_controller->AddEarthManipulatorListener(this);
   }
}

void MapView::OnUpdate(CView * pSender, LPARAM lHint, CObject * pHint)
{
   if (!lHint)
      invalidate_view(TRUE);
   else
      invalidate_view(FALSE);
}

// MapView diagnostics

#ifdef _DEBUG
void MapView::AssertValid() const
{
   CView::AssertValid();
}

void MapView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}

CMapDoc* MapView::GetDocument()  // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapDoc)));
   return (CMapDoc*) m_pDocument;
}

#endif  // _DEBUG


// MapView message handlers

void MapView::OnMapOptions()
{
   CMapOptionsDlg dlg(this, reinterpret_cast<MapEngineCOM *>(m_map_engine));

   // [Bug 1215].  Locking the temporary maps is necessary to work around an MFC
   // bug in COccManager::IsDialogMessage when a dialog contains an ActiveX
   // control
   AfxLockTempMaps();
   dlg.DoModal();
   AfxUnlockTempMaps();
}

void MapView::OnMapGroups()
{
   CCustomMapGroups dlg;
   if (dlg.DoModal() == IDOK)
   {
      // Build the Map menu based on currently available map types
      CMenu* pMenu = GetMenu();
      CMainFrame* pFrame = (CMainFrame*)UTL_get_frame();
      if (pMenu && pFrame)
         pFrame->RebuildMapMenu();

      m_map_engine->CoverageUpdated();

      // verify that the current map in the current map group is valid
      const int status = m_map_engine->change_map_type(
         m_map_engine->GetCurrentGroupIdentity(),
         get_curr_map()->source(), get_curr_map()->scale(),
         get_curr_map()->series(), FALSE, TRUE);

      if (status != SUCCESS)
      {
         // get the name of the current category (group name)
         MapCategory currentCategory = GetCategoryFromGroupId(
            m_map_engine->GetCurrentGroupIdentity());

         // try changing to the closest map in the group
         if (currentCategory.get_string().GetLength() == 0 ||
            ChangeToClosestScale(currentCategory) != SUCCESS)
         {
            // and if that fails, try changing to the closest map in the Raster
            // group
            if (ChangeToClosestScale(RASTER_CATEGORY) != SUCCESS)
            {
               // and just in case, we'll fall all the way back to Blank map
               ChangeToClosestScale(BLANK_CATEGORY);
            }
         }
      }

      invalidate_view();
   }
}

void MapView::OnUpdateMapGroups(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(1);
}

void MapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   char display[PRM_MAX_VALUE_LENGTH+1];
   C_overlay * ovl = NULL;
   int status = SUCCESS;

   if (nChar == VK_CONTROL && m_map_change_tool != NULL)
   {
      CPoint point;
      GetCursorPos(&point);
      ScreenToClient(&point);

      m_map_change_tool->on_key_down(get_curr_map(), point, nFlags);
      SetViewCursor(AfxGetApp()->LoadCursor(IDC_MAGNIFY_MINUS));
      return;
   }

   // allow the overlay manager a chance to handle the keystroke...
   // if it is not processed by an overlay, then continue
   if (OVL_get_overlay_manager()->offer_keydown(
      GetFvMapView(), nChar, nRepCnt, nFlags))
      return;

   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      switch (nChar)
      {
         case VK_HOME:
         case VK_PRIOR:
         case VK_NEXT:
         case VK_F2:
         case VK_F3:
         case VK_F4:
         case VK_F5:
         case VK_F6:
         case VK_F7:
         case VK_F8:
         case VK_F9:
         case VK_F12:
         case VK_E:
         case VK_L:
         case '3':
         case VK_F:
         case VK_TAB:
         case VK_ESCAPE:
         case 'Z':
            break;
         default:
            return;
      }
   }

   switch (nChar)
   {
      case VK_HOME:
         {
            MapProjectionParams map_proj_params = GetMapProjParams();
            map_proj_params.center.lat = 0;
            map_proj_params.center.lon = 0;
            map_proj_params.zoom_percent = 100;
            map_proj_params.rotation = 0;
            status = ChangeMapType(RASTER_GROUP_ID,
               MapType(TIROS, WORLD, "TopoBath"), map_proj_params);

            if (status == SUCCESS)
               invalidate_view();
         }
         break;

      case VK_PRIOR:  // Page Up
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               ScaleIn(m_cursor_position_geo);
            }
            else
               ScaleIn();
         }
         break;

      case VK_NEXT:  // Page Down
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               ScaleOut(m_cursor_position_geo);
            }
            else
               ScaleOut();
         }
         break;

      case VK_F3:
         {
            C_ovl_mgr* overlay_manager = OVL_get_overlay_manager();
            GUID boundaries = FVWID_Overlay_Boundaries;
            if (overlay_manager->get_first_of_type(boundaries) == nullptr)
            {
               // activate the boundaries overlay
               C_overlay* overlay;
               overlay_manager->create(boundaries, &overlay);
            }

            if (status == SUCCESS)
               invalidate_view();
         }
         // fall through - change to blank map

      case VK_F2:
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               status = ChangeToClosestScale(BLANK_CATEGORY,
                  m_cursor_position_geo);
            }
            else
               status = ChangeToClosestScale(BLANK_CATEGORY);

            if (status == SUCCESS)
               invalidate_view();
         }
         break;

      case VK_F4:
         {
            // Ctrl+F4 closes current map tab
            if (GetKeyState(VK_CONTROL) < 0)
            {
               CMainFrame *pFrame = fvw_get_frame();
               if (pFrame != NULL)
               {
                  CMapViewTabsBar *pMapViewTabs = pFrame->GetMapViewTabsBar();
                  if (pMapViewTabs != NULL)
                     pMapViewTabs->DeleteCurrentTab();
               }
            }
            else
            {
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(TIROS, WORLD, "TopoBath"));
               if (status == SUCCESS)
                  invalidate_view();
            }
         }
         break;

      case VK_F5:
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_5M, GNC_SERIES),
                  m_cursor_position_geo);
            }
            else
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_5M, GNC_SERIES));

            if (status == SUCCESS)
               invalidate_view();
         }
         break;

      case VK_F6:
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_2M, JNC_SERIES),
                  m_cursor_position_geo);
            }
            else
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_2M, JNC_SERIES));

            if (status == SUCCESS)
               invalidate_view();
         }
         break;

      case VK_F7:
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_1M, ONC_SERIES),
                  m_cursor_position_geo);
            }
            else
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_1M, ONC_SERIES));

            if (status == SUCCESS)
               invalidate_view();
         }
         break;

      case VK_F8:
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_500K, TPC_SERIES),
                  m_cursor_position_geo);
            }
            else
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_500K, TPC_SERIES));

            if (status == SUCCESS)
               invalidate_view();
         }
         break;

      case VK_F9:
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_250K, JOG_SERIES),
                  m_cursor_position_geo);
            }
            else
               status = ChangeMapType(RASTER_GROUP_ID,
                  MapType(MapSource("ECRG"), ONE_TO_250K, JOG_SERIES));

            if (status == SUCCESS)
               invalidate_view();
         }
         break;

      case VK_F12:
         {
            if (IsCursorInView() && m_cursor_position_geo_valid)
            {
               m_move_cursor = TRUE;
               status = ChangeMapType(ELEVATION_GROUP_ID,
                  MapType(MapSource("DTED"),
                  MapScale(3.0, MapScale::ARC_SECONDS), "Level 1"),
                  m_cursor_position_geo);
            }
            else
               status = ChangeMapType(ELEVATION_GROUP_ID,
                  MapType(MapSource("DTED"),
                  MapScale(3.0, MapScale::ARC_SECONDS), "Level 1"));

            if (status == SUCCESS)
               invalidate_view();
         }
         break;

      // Ctrl-E changes the map to an equal-arc projection.  We are doing
      // this here rather than adding entries to the accelerator table because
      // the projections are meant to be dynamic so the menu IDs for the
      // projection types could change
      case VK_E:
         {
            if ((GetKeyState(VK_CONTROL) < 0) && (GetKeyState(VK_SHIFT) >= 0)) 
               ChangeProjectionType(EQUALARC_PROJECTION);
         }
         break;

      // Ctrl-L changes the map to a lambert projection
      case VK_L:
         {
            if (GetKeyState(VK_CONTROL) < 0)
               ChangeProjectionType(LAMBERT_PROJECTION);
         }
         break;

      // Ctrl+3 changes the map to a general perspective (3D) projection
      case '3':
         {
            CMainFrame *frame = fvw_get_frame();
            if ((GetKeyState(VK_CONTROL) < 0) && frame->Is3DEnabledInRegistry())
            {     
               ChangeProjectionType(GENERAL_PERSPECTIVE_PROJECTION);
            }
         }
         break;

      case VK_F:
         {
            if ((GetKeyState(VK_CONTROL) < 0) && (GetKeyState(VK_SHIFT) >= 0))
            {
               const boolean_t bCursorInView = IsCursorInView();

               const int STR_LEN = 100;
               char oldLatLonString[STR_LEN];
               if (bCursorInView && m_cursor_position_geo_valid)
               {
                  GEO_lat_lon_to_string(m_cursor_position_geo.lat,
                     m_cursor_position_geo.lon, oldLatLonString, STR_LEN);
               }

               GEO_get_default_display(display, PRM_MAX_VALUE_LENGTH+1);

               if (strcmp(display, "PRIMARY") == 0)
                  GEO_set_default_display("SECONDARY");
               else
                  GEO_set_default_display("PRIMARY");

               // force an update if the datum is changed
               OVL_get_overlay_manager()->invalidate_all();

               // get the cursor location in geo coordinates, returns FALSE if
               // the cursor is outside of the view
               if (bCursorInView && m_cursor_position_geo_valid)
               {
                  const int STR_LEN = 100;
                  char s[STR_LEN];

                  // convert it to a string in the new format
                  GEO_lat_lon_to_string(m_cursor_position_geo.lat,
                     m_cursor_position_geo.lon, s, STR_LEN);

                  // update the running lat-lon in the new format
                  static CTextInfoPane *pTextInfoPane =
                     static_cast<CTextInfoPane *>(
                     GetFrame()->GetStatusBarInfoPane(
                     STATUS_BAR_PANE_CURSOR_LOCATION));
                  if (pTextInfoPane != NULL)
                     pTextInfoPane->SetText(s);

                  static CTextInfoPane *pSecondaryInfoPane =
                     static_cast<CTextInfoPane *>(
                     GetFrame()->GetStatusBarInfoPane(
                     STATUS_BAR_PANE_SECONDARY_CURSOR_LOCATION));
                  if (pSecondaryInfoPane != NULL)
                     pSecondaryInfoPane->SetText(oldLatLonString);
               }
            }
         }
         break;

         // Ctrl+Tab will cycle to the next map favorites tab
      case VK_TAB:
         if (GetKeyState(VK_CONTROL) < 0)
         {
            CMapViewTabsBar* pMapViewTabsBar =
               fvw_get_frame()->GetMapViewTabsBar();
            if (pMapViewTabsBar != NULL)
               pMapViewTabsBar->GetCtrl()->GotoNextTab();
         }

      case VK_ESCAPE:
         {
            if (m_mouse_click_resource->on_cancel())
            {
               // restore the cursor
               HCURSOR cursor = NULL;

               cursor = OVL_get_overlay_manager()->get_default_cursor();

               if (cursor && cursor != m_cursor)
               {
                  m_cursor = cursor;
                  SetCursor(m_cursor);
               }

               OnLButtonUp(0, CPoint(0, 0));
            }
            else if (m_hand_mode)
               m_hand_mode = FALSE;

            if (m_map_change_tool != NULL)
            {
               m_map_change_tool->cancel_drag(get_curr_map());

               // make sure the tooltip is cleared if necessary
               CTipDisplay tip;
               tip.clear();

               delete m_map_change_tool;
               m_map_change_tool = NULL;
            }

            // if something is being dragged, cancel the drag
            if (m_drag)
            {
               m_mouse_click_resource->on_cancel_drag(get_curr_map());
               m_drag = FALSE;
            }
            else
            {
               CMainFrame *frame = fvw_get_frame();
               if (frame != NULL && !frame->IsRibbonBarVisible())
               {
                  frame->ShowRibbonBar(true);
               }
               else
               {
                  CView::OnKeyDown(nChar, nRepCnt, nFlags);
               }
            }
         }
         break;

      // pan left with arrow or numeric key pad 4
      case VK_LEFT:
      case VK_NUMPAD4:
         if (GetKeyState(VK_CONTROL) < 0)
         {
            if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
                OnMapRotateClockwise();
            break;
         }
         if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
            OnMapPanLeft();
         break;

      // pan right with arrow or numeric key pad 6
      case VK_RIGHT:
      case VK_NUMPAD6:
         if (GetKeyState(VK_CONTROL) < 0)
         {
            if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
                OnMapRotateCounterclockwise();
            break;
         }
         if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
            OnMapPanRight();
         break;

      // pan up with arrow or numeric key pad 8
      case VK_UP:
      case VK_NUMPAD8:
         {
            if (GetKeyState(VK_CONTROL) < 0)
            {
               OnMapRotateNorthUp();
               break;
            }
         }
         if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
            OnMapPanUp();
         break;

      // pan down with arrow or numeric key pad 2
      case VK_DOWN:
      case VK_NUMPAD2:
         if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
            OnMapPanDown();
         break;

      // diagonal keys on numeric keypad with numlock on
      case VK_NUMPAD7:
      case VK_NUMPAD9:
      case VK_NUMPAD1:
      case VK_NUMPAD3:
         {
            if (m_current_map_rendering_engine)
            {
               if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
               {
                   int pan_dir = 0;

                   // move left if 7 or 1, else right
                   pan_dir |= (nChar == VK_NUMPAD7 || nChar == VK_NUMPAD1)
                      ? PAN_DIRECTION_LEFT : PAN_DIRECTION_RIGHT;

                   // move up if 7 or 9, else down
                   pan_dir |= (nChar == VK_NUMPAD7 || nChar == VK_NUMPAD9)
                      ? PAN_DIRECTION_UP : PAN_DIRECTION_DOWN;

                   PanMap(pan_dir);
               }
            }
         }
         break;

      // the space key will be used to toggle the smooth scrolling hand
      // mode.  Set the cursor to the hand icon.
      case VK_SPACE:
         force_smooth_pan();
         break;

      case 'S':
         {
         if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
            OnScaleZoom();
         }
         break;

      case 'Z':
         OnScalePercentZoom();
         break;

      default:
         CView::OnKeyDown(nChar, nRepCnt, nFlags);

      return;
   }

   if (status != SUCCESS)
   {
      m_move_cursor = FALSE;
      OnMapError(status);
   }
}


void MapView::force_smooth_pan()
{
   if (!m_force_hand_scroll && !m_hand_scroll)
   {
      m_force_hand_scroll = TRUE;

      // cancel a drag operation if necessary
      OVL_get_overlay_manager()->offer_keydown(GetFvMapView(), VK_ESCAPE, 0, 0);

      SetViewCursor(AfxGetApp()->LoadCursor(IDC_HAND0));
   }

   // hide the zoom/scale tool tooltip if necessary
   if (m_map_change_tool != NULL)
   {
      CTipDisplay tip;
      tip.clear();
   }
}


void MapView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      return;
   }

   // allow the overlay manager a chance to handle the keystroke...
   // if it is not processed by an overlay, then continue
   if (OVL_get_overlay_manager()->offer_keyup(GetFvMapView(), nChar, nRepCnt,
      nFlags))
      return;

   // space bar up implies leaving the smooth scroll hand drag mode.  Reset
   // the cursor if not currently dragging the map
   if (nChar == VK_SPACE)
   {
      m_force_hand_scroll = FALSE;

      HCURSOR cursor = OVL_get_overlay_manager()->get_default_cursor();

      if (cursor && !m_hand_scroll)
      {
         m_cursor = cursor;
         SetCursor(m_cursor);
      }

      if (m_map_change_tool != NULL)
      {
         SetViewCursor(AfxGetApp()->LoadCursor(IDC_MAGNIFY_PLUS));

         CPoint point;
         GetCursorPos(&point);
         ScreenToClient(&point);
         m_map_change_tool->on_key_up(get_curr_map(), point, nFlags);
      }
   }

   CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void MapView::OnCenter()
{
   if (m_cursor_position_geo_valid)
      ChangeCenter(m_cursor_position_geo);
}

void MapView::OnScaleIn()
{
   ScaleIn();
}

int MapView::ScaleIn()
{
   MapType new_map_type;
   int status = m_current_map_rendering_engine->ScaleIn(&new_map_type);

   if (status == SUCCESS)
   {
      m_map_params.SetMapType(new_map_type);
      UpdateContrast();
      invalidate_view();
   }
   else
      OnMapError(status);

   return status;
}

int MapView::ScaleIn(const d_geo_t& center)
{
   MapType new_map_type;
   int status = m_current_map_rendering_engine->ScaleIn(center, &new_map_type);

   if (status == SUCCESS)
   {
      m_map_params.SetMapType(new_map_type, center);
      UpdateContrast();
      invalidate_view();
   }
   else
      OnMapError(status);

   return status;
}

void MapView::OnScaleOut()
{
   ScaleOut();
}

int MapView::ScaleOut()
{
   MapType new_map_type;
   int status = m_current_map_rendering_engine->ScaleOut(&new_map_type);

   if (status == SUCCESS)
   {
      m_map_params.SetMapType(new_map_type);
      UpdateContrast();
      invalidate_view();
   }
   else
      OnMapError(status);

   return status;
}

int MapView::ScaleOut(const d_geo_t& center)
{
   MapType new_map_type;
   int status = m_current_map_rendering_engine->ScaleOut(center, &new_map_type);

   if (status == SUCCESS)
   {
      m_map_params.SetMapType(new_map_type, center);
      UpdateContrast();
      invalidate_view();
   }
   else
      OnMapError(status);

   return status;
}

void MapView::SetBrightness(double brightness)
{
   if (brightness < BRIGHTNESS_LIMIT && 
      brightness > -1*BRIGHTNESS_LIMIT)
   {
      m_current_map_rendering_engine->ChangeBrightness(brightness);
      set_current_map_invalid();
      m_map_params.SetBrightness(brightness);
      invalidate_view();
   }
}

void MapView::OnBright(void)
{
   double brightness = GetBrightness();
   brightness += 0.05;
   SetBrightness(brightness);
}

void MapView::OnDim(void)
{
   double brightness = GetBrightness();
   brightness -= 0.05;
   SetBrightness(brightness);
}

void MapView::OnOverlay(UINT nId)
{
   INFO_report("OnOverlay()");
}

void MapView::OnUpdateOverlay(CCmdUI* pCmdUI)
{
}

void MapView::OnUndo()
{
   C_overlay* crnt_overlay = OVL_get_overlay_manager()->get_current_overlay();
   if (OVL_get_overlay_manager()->GetCurrentEditor() != GUID_NULL &&
      crnt_overlay != NULL && crnt_overlay->can_undo())
      crnt_overlay->undo();
}

void MapView::OnUpdateUndo(CCmdUI* pCmdUI)
{
   // determine if the top-most overlay, being edited, can undo the previous
   // editing operation
   C_overlay* crnt_overlay = OVL_get_overlay_manager()->get_current_overlay();
   pCmdUI->Enable(OVL_get_overlay_manager()->GetCurrentEditor() != GUID_NULL &&
      crnt_overlay != NULL && crnt_overlay->can_undo());
}

void MapView::OnRedo()
{
   C_overlay* crnt_overlay = OVL_get_overlay_manager()->get_current_overlay();
   if (OVL_get_overlay_manager()->GetCurrentEditor() != GUID_NULL &&
      crnt_overlay != NULL && crnt_overlay->can_redo())
      crnt_overlay->redo();
}

void MapView::OnUpdateRedo(CCmdUI* pCmdUI)
{
   // determine if the top-most overlay, being edited, can redo the previous
   // editing operation
   C_overlay* crnt_overlay = OVL_get_overlay_manager()->get_current_overlay();
   pCmdUI->Enable(OVL_get_overlay_manager()->GetCurrentEditor() != GUID_NULL &&
      crnt_overlay != NULL && crnt_overlay->can_redo());
}

void MapView::OnUpdateBright(CCmdUI* pCmdUI)
{
   const MapDisplayParams& map_display_params =
      m_map_params.GetMapDisplayParams();
   const MapType& map_type = m_map_params.GetMapType();

   if (map_type.get_source() == CIB ||
      map_type.get_series() == BLACK_AND_WHITE_SERIES)
   {
      if (map_display_params.auto_enhance_CIB)
         pCmdUI->Enable(FALSE);
      else
      {
         if (map_display_params.brightness >= BRIGHTNESS_LIMIT)
            pCmdUI->Enable(FALSE);
         else
            pCmdUI->Enable(TRUE);
      }

      return;
   }
   else if (map_type.get_source() == BLANK_MAP)
   {
      pCmdUI->Enable(FALSE);
      return;
   }

   if (map_display_params.brightness >= BRIGHTNESS_LIMIT)
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(TRUE);
}

void MapView::OnUpdateDim(CCmdUI* pCmdUI)
{
   const MapDisplayParams& map_display_params =
      m_map_params.GetMapDisplayParams();
   const MapType& map_type = m_map_params.GetMapType();

   if (map_type.get_source() == CIB ||
      map_type.get_series() == BLACK_AND_WHITE_SERIES)
   {
      if (map_display_params.auto_enhance_CIB)
         pCmdUI->Enable(FALSE);
      else
      {
         if (map_display_params.brightness <= -1*BRIGHTNESS_LIMIT)
            pCmdUI->Enable(FALSE);
         else
            pCmdUI->Enable(TRUE);
      }

      return;
   }
   else if (map_type.get_source() == BLANK_MAP)
   {
      pCmdUI->Enable(FALSE);
      return;
   }

   if (map_display_params.brightness <= -1*BRIGHTNESS_LIMIT)
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(TRUE);
}

void MapView::OnMapError(int map_error)
{
   if (DialogNotify::SuppressDialogDisplay(MODAL_DLG_MAP_DISPLAY_ERROR) == TRUE)
      return;

   // put up error messages when MAP_do_display() errors occur
   switch (map_error)
   {
      case SUCCESS:
         break;

      case FV_NO_DATA:
          AfxMessageBox("Data is not available for the selected map."
               "  Use the Map Data Manager to view map data "
               "coverage.", MB_OK | MB_ICONINFORMATION);
         break;

      case MEMORY_ERROR:
         AfxMessageBox("Memory Error.  See the file "
            "\"" + appErrorFile() + "\" in the " + appShortName() +
            " installation directory for more information.");
         break;

      case FATAL_MAP_DISPLAY_ERROR:
         AfxMessageBox("Error encountered.  See the file "
            "\"" + appErrorFile() + "\" in the " + appShortName()+
            " installation directory for more information.");
         break;

      case COV_FILE_OUT_OF_SYNC:
         // Note: COV_FILE_OUT_OF_SYNC is not handled here because it is
         // handled by a message sent to the FalconView window (using
         // FVW_error_report)
         break;

      case FAILURE:
         // Give a more meaningfull error to the user.  The error log still has
         // the gory details for us
         AfxMessageBox("Available Map Data has changed.  "
            "Please refresh the screen");
         break;
   }
}

void MapView::OnMapRotateClockwise(void)
{
   // This handler only applies to 2D projections.  3D panning is handled in the
   // CMainFrame class.
   if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
   {
      double rot_angle = GetMapProjParams().rotation;

      rot_angle += 30.0;
      if (rot_angle >= 360.0)
         rot_angle -= 360.0;

      if (rot_angle < 0.05)
         rot_angle = 0.0;

      ChangeRotation(rot_angle);
   }
}

void MapView::OnMapRotateCounterclockwise(void)
{
   // This handler only applies to 2D projections.  3D panning is handled in the
   // CMainFrame class.
   if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
   {
      double rot_angle = GetMapProjParams().rotation;

      rot_angle -= 30.0;
      if (rot_angle < 0.0)
         rot_angle += 360.0;

      if (rot_angle < 0.05)
         rot_angle = 0.0;

      ChangeRotation(rot_angle);
   }
}

void MapView::OnMapRotateNorthUp()
{
   // This handler only applies to 2D projections.  3D panning is handled in the
   // CMainFrame class.
   if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
   {
      if (GetMapProjParams().rotation != 0.0)
         ChangeRotation(0.);
   }
}

void MapView::ChangeRotation(double rotation)
{
   if (!curr_map_rotation_ui_disabled())
   {
      const int status =
         m_current_map_rendering_engine->ChangeRotation(rotation);
      if (status == SUCCESS)
      {
         invalidate_view(FALSE);
         m_map_params.SetRotation(rotation);
      }
      else
         OnMapError(status);
   }
}

int MapView::ChangeCenter(const d_geo_t& center)
{
   const int status = m_current_map_rendering_engine->ChangeCenter(center);

   if (status == SUCCESS)
   {
      m_move_cursor = TRUE;
      m_map_params.SetCenter(center);
      invalidate_view(FALSE);
   }
   else
      OnMapError(status);
   return status;
}

int MapView::ChangeCenter(const degrees_t& lat, const degrees_t& lon)
{
   d_geo_t center;
   center.lat = lat;
   center.lon = lon;
   return ChangeCenter(center);
}

int MapView::ChangeCenterAndRotation(const d_geo_t& center, double rotation)
{
   const int status = m_current_map_rendering_engine->
      ChangeCenterAndRotation(center, rotation);

   if (status == SUCCESS)
   {
      m_move_cursor = TRUE;
      m_map_params.SetCenter(center);
      m_map_params.SetRotation(rotation);
      invalidate_view(FALSE);
   }
   else
      OnMapError(status);
   return status;
}

int MapView::ChangeCenterAndRotation(const degrees_t& lat, const degrees_t& lon,
      double rotation)
{
   d_geo_t center;
   center.lat = lat;
   center.lon = lon;
   return ChangeCenterAndRotation(center, rotation);
}

void MapView::UpdateCenter(const d_geo_t& center)
{
   // Not intended to move to a new center but update
   // callbacks with camera's new center since rendering
   // engine is already up to date. To update rendering
   // engine use MapView::ChangeCenter instead.
   int status = SUCCESS; // Success until failure
   if (IsRendering2d())
      status = m_current_map_rendering_engine->ChangeCenter(center);

   if (status == SUCCESS)
   {
      m_move_cursor = TRUE;
      m_map_params.SetCenter(center);
      invalidate_view(FALSE);
   }
   else
      OnMapError(status);
}

bool MapView::ValidateDeltaZoomPercent(int step_size, int* new_zoom_percent)
{
   // do nothing if zoom is not valid for the current scale
   if (MAP_zoom_percent_valid(GetMapType().get_scale()) == FALSE)
      return false;

   *new_zoom_percent = GetMapProjParams().zoom_percent + step_size;

   if (*new_zoom_percent < MapView::MIN_ZOOM_PERCENT ||
      *new_zoom_percent > MapView::MAX_ZOOM_PERCENT)
   {
      return false;
   }

   return true;
}

int MapView::ChangeZoomPercent(int zoom_percent)
{
   // Note that FV_NO_DATA or COV_FILE_OUT_OF_SYNC could be returned
   //
   int status = m_current_map_rendering_engine->ChangeZoomPercent(zoom_percent);
   if (status == SUCCESS)
   {
      m_map_params.SetZoomPercent(zoom_percent);
      invalidate_view();
   }
   else
      OnMapError(status);

   return status;
}

int MapView::ChangeZoomPercent(int zoom_percent, const d_geo_t& center)
{
   int status = m_current_map_rendering_engine->ChangeZoomPercent(zoom_percent,
      center);
   if (status == SUCCESS)
   {
      m_map_params.SetZoomPercent(zoom_percent, center);
      invalidate_view();
   }
   else
      OnMapError(status);

   return status;
}

void MapView::ChangeZoomPercentByStep(int step_size)
{
   int new_zoom_percent;
   if (ValidateDeltaZoomPercent(step_size, &new_zoom_percent))
      ChangeZoomPercent(new_zoom_percent);
}

void MapView::ChangeZoomPercentByStep(int step_size, const d_geo_t& center)
{
   int new_zoom_percent;
   if (ValidateDeltaZoomPercent(step_size, &new_zoom_percent))
      ChangeZoomPercent(new_zoom_percent, center);
}

int MapView::ChangeTilt(double tilt_degrees)
{
   if (m_current_map_rendering_engine->ChangeTilt(tilt_degrees) == SUCCESS)
   {
      m_map_params.SetTilt(tilt_degrees);
      invalidate_view(FALSE);
   }

   return SUCCESS;
}

int MapView::NotifyTiltUpdate(double tilt_degrees)
{
   new fvw::UIThreadOperation([=]()
   {
      m_map_params.SetTilt(tilt_degrees);
      invalidate_view(FALSE);
   });
   return SUCCESS;
}

void MapView::UpdateRotationAndTilt(double rotation, double tilt_degrees)
{
   m_map_params.SetRotationAndTilt(rotation, tilt_degrees);
}

void MapView::OnMapPanUpdate(CCmdUI* cmd)
{
   cmd->Enable(TRUE);
}

void MapView::PanMap(int pan_dir)
{
   if (m_current_map_rendering_engine == nullptr)
      return;

   // This handler only applies to 2D projections.  3D panning is handled in the
   // CMainFrame class.
   if (GetProjectionType() != GENERAL_PERSPECTIVE_PROJECTION)
   {
      d_geo_t new_center;
      const int status = m_current_map_rendering_engine->PanMap(pan_dir,
         &new_center);
      if (status == SUCCESS)
      {
         m_map_params.SetCenter(new_center);
         invalidate_view(FALSE);
      }
      else
         OnMapError(status);
   }
}


void MapView::OnMapPanDown()
{
   PanMap(PAN_DIRECTION_DOWN);
}

void MapView::OnMapPanLeft()
{
   PanMap(PAN_DIRECTION_LEFT);
}

void MapView::OnMapPanRight()
{
   PanMap(PAN_DIRECTION_RIGHT);
}

void MapView::OnMapPanUp()
{
   PanMap(PAN_DIRECTION_UP);
}

fvw::CameraMode MapView::GetCameraMode() const
{
   if (m_current_map_rendering_engine)
      return m_current_map_rendering_engine->GetCameraMode();

   return fvw::CAMERA_MODE_NONE;
}

void MapView::SetCameraMode(fvw::CameraMode camera_mode)
{
   if (m_current_map_rendering_engine)
      m_current_map_rendering_engine->SetCameraMode(camera_mode);
}

void MapView::ToggleEnableFlightModeAutoMapType()
{
   if (m_current_map_rendering_engine)
   {
      m_current_map_rendering_engine->
         ToggleEnableFlightModeAutoMapType();
   }
}

bool MapView::GetEnableFlightModeAutoMapType()
{
   if (m_current_map_rendering_engine)
   {
      return m_current_map_rendering_engine->GetEnableFlightModeAutoMapType();
   }
   return false;
}

void MapView::OnUpdateMapRotateClockwise(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!curr_map_rotation_ui_disabled());
}

void MapView::OnUpdateMapRotateCounterclockwise(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!curr_map_rotation_ui_disabled());
}

void MapView::OnUpdateMapRotateNorthup(CCmdUI* pCmdUI)
{
   bool ui_enabled = !curr_map_rotation_ui_disabled();
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      HUD* hud = GetGeospatialViewController()->GetGeospatialScene()->GetHud();
      if (hud->m_heading < 359.9 && hud->m_heading > 0.1)
      {
         pCmdUI->Enable(ui_enabled);
      }
      else
      {
         pCmdUI->Enable(false);
      }
   }
   else
   {
      pCmdUI->Enable(ui_enabled && GetMapProjParams().rotation > 0.0);
   }
}

void MapView::OnScalePercentZoom()
{
   // if we are not currently in a map change tool or we are in the scale tool
   // we will enter the zoom tool when the button is pressed. Otherwise, we will
   // leave from the zoom tool mode
   int enter_scale_percent_zoom_mode = m_map_change_tool == NULL ||
      (m_map_change_tool && m_map_change_tool->is_kind_of("MapScaleTool"));

   if (m_map_change_tool)
   {
      delete m_map_change_tool;
      m_map_change_tool = NULL;
   }

   if (enter_scale_percent_zoom_mode)
   {
      m_map_change_tool = new MapZoomTool(this);

      m_old_cursor = m_cursor;
      SetViewCursor(AfxGetApp()->LoadCursor(IDC_MAGNIFY_PLUS));

      // either the scale percent zoom or scale zoom can be on, but not both
      m_hand_mode = FALSE;
   }
   else
      SetViewCursor(m_old_cursor);
}

void MapView::OnUpdateScalePercentZoom(CCmdUI* pCmdUI)
{
   if (get_curr_map()->is_spec_set())
   {
      const boolean_t zoom_ok_for_new_map =
         MAP_zoom_percent_valid(GetMapType().get_scale());

      if (zoom_ok_for_new_map == FALSE)
      {
         if (m_map_change_tool && m_map_change_tool->is_kind_of("MapZoomTool"))
         {
            delete m_map_change_tool;
            m_map_change_tool = NULL;
         }
      }

      pCmdUI->Enable(zoom_ok_for_new_map);
   }

   pCmdUI->SetCheck(m_map_change_tool != NULL &&
      m_map_change_tool->is_kind_of("MapZoomTool"));
}

void MapView::OnScaleZoom()
{
   // if we are not currently in a map change tool or we are in the zoom tool we
   // will enter the scale tool when the button is pressed.  Otherwise, we will
   // leave from the scale tool mode
   int enter_zoom_mode = m_map_change_tool == NULL ||
      m_map_change_tool->is_kind_of("MapZoomTool");

   if (m_map_change_tool)
   {
      delete m_map_change_tool;
      m_map_change_tool = NULL;
   }

   if (enter_zoom_mode)
   {
      m_map_change_tool = new MapScaleTool(this);

      m_old_cursor = m_cursor;
      SetViewCursor(AfxGetApp()->LoadCursor(IDC_MAGNIFY_PLUS));

      // either the scale percent zoom or scale zoom can be on, but not both
      m_hand_mode = FALSE;
   }
   else
      SetViewCursor(m_old_cursor);
}

void MapView::OnUpdateScaleZoom(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_map_change_tool &&
      m_map_change_tool->is_kind_of("MapScaleTool"));
}

void MapView::OnHandMode()
{
   m_hand_mode = !m_hand_mode;

   m_geospatial_view_controller->SetIsPanning(m_hand_mode);

   if (m_hand_mode)
   {
      m_old_cursor = m_cursor;
      SetViewCursor(AfxGetApp()->LoadCursor(IDC_HAND0));

      if (m_map_change_tool)
      {
         delete m_map_change_tool;
         m_map_change_tool = NULL;
      }
   }
   else
      SetViewCursor(m_old_cursor);
}

void MapView::OnUpdateHandMode(CCmdUI* pCmdUI)
{
   bool hand_mode = m_hand_mode || m_force_hand_scroll;
   if (GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      if (GetGeospatialViewController()->GetInputDevice()->GetActiveCam() == 
         fvw::CAMERA_MODE_PAN)
      {
         if (GetGeospatialViewController()->GetInputDevice()->space && !m_force_hand_scroll)
         {
            force_smooth_pan();
         }
         else if (!GetGeospatialViewController()->GetInputDevice()->space)
         {
            m_force_hand_scroll = false;
         }
         hand_mode = m_hand_mode || m_force_hand_scroll;
         pCmdUI->Enable(true);
         pCmdUI->SetCheck(hand_mode);
         m_geospatial_view_controller->SetIsPanning(hand_mode);
      }
      else
      {
         pCmdUI->Enable(false);
         pCmdUI->SetCheck(false);
      }
   }
   else
   {
      pCmdUI->Enable(true);
      pCmdUI->SetCheck(hand_mode);
   }
}

// Remove all entries from the overlay menu list and delete them.
// Remove all of the entries in the overlay menu map.
void MapView::FreeOverlayMenu()
{
   if (m_pContextMenu != NULL)
      m_pContextMenu->ClearMenuItems();

   m_overlay_menu_map.RemoveAll();

}
// end of FreeOverlayMenu

// **************************************************************

// handle overlay callbacks from popup menu

void MapView::OnOverlayPopup(UINT nId)
{
   try
   {
      FalconViewOverlayLib::IFvContextMenuItemPtr spMenuItem = NULL;

      if (m_overlay_menu_map.Lookup(nId, spMenuItem))
         spMenuItem->MenuItemSelected();
   }
   catch(_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }

   // free overlay menu memory
   FreeOverlayMenu();
}

// handle 
void MapView::OnOverlayPopupUpdate(CCmdUI* pCmdUI)
{
   // disable/check the menu items, we have to do it in the CommandUI handler 
   // because CMFCPopupMenu does not honor the MF_DISABLED | MF_CHECKED FLAGS
   std::vector<UINT>::iterator menuEnabledID = 
            std::find(m_nDisabledMenuIDs.begin(), m_nDisabledMenuIDs.end(), 
            pCmdUI->m_nID);
   if (menuEnabledID != m_nDisabledMenuIDs.end())
   {
      pCmdUI->Enable(FALSE);
   }
   else
   {
      pCmdUI->Enable(TRUE);
   }
   // check any menu items that have been identified
   std::vector<UINT>::iterator menuCheckedID = 
               std::find(m_nCheckedMenuIDs.begin(), m_nCheckedMenuIDs.end(), 
               pCmdUI->m_nID);
   if (menuCheckedID != m_nCheckedMenuIDs.end())
   {
      pCmdUI->SetCheck(TRUE);
   }
}

void MapView::OnMapNew()
{
   // Prepare and invoke the Go To property sheet
   CGoToPropSheet propsheet("Go To", AfxGetMainWnd(), 0);

   // remove the Apply button from the GoTo property sheet
   propsheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

   // Activate the last active property page (as defined in the registry).  If
   // there is no value in the registry, activate the first property page (Map
   // Settings page, index=0)
   int last_active_page_index = PRM_get_registry_int("GoToPropSheet",
      "LastActivePageIndex", 0);
   propsheet.SetActivePage(last_active_page_index);

   // initialize rotation state from view
   propsheet.m_mapGoToPP->m_enable_rotate = !curr_map_is_forced_north_up();

   if (propsheet.DoModal() == -1)
      MessageBox("Unable to create GoTo property sheet", "Error", MB_ICONSTOP);

   if (propsheet.m_mapGoToPP->m_redraw)
   {
      invalidate_view();
      propsheet.m_mapGoToPP->m_redraw = FALSE;
   }
}

void MapView::OnDestroy()
{
   CView::OnDestroy();

   // m_first_map_up is used here to disable any mouse or keyboard inputs
   // that occur after the message queue was flushed in CWnd::DestroyWindow().
   // Note: CWnd::DestroyWindow() flushes the message queue and then it
   // sends a WM_DESTROY message to the CWnd object, which results in
   // this function being called.
   m_first_map_up = FALSE;

   m_map_engine->uninit();
}

void MapView::SetViewCursor(HCURSOR hCursor)
{
   if (hCursor && hCursor != m_cursor)
   {
      m_cursor = hCursor;
      ::SetCursor(m_cursor);
   }
}

void MapView::update_title_bar()
{
   // if the MapView class has already destroyed all documents, then
   // get the heck out of dodge.
   if (!GetDocument())
      return;

   CString title;

#ifdef GOV_RELEASE
   // display the build type, and expiration date if needed
   if (need_to_display_non_release_warning_dialog() &&
      !fvw_get_app()->m_expiration->never_expires())
   {
      if (need_to_display_non_release_warning_dialog())
      {
         // only display build type if
         // need_to_display_non_release_warning_dialog because
         // project_build_type() will likely return "beta" in release versions
         title += project_build_type();
         title += " Release";
      }

      title += " (Expires " +
         fvw_get_app()->m_expiration->get_expiration_date_as_string() +")";
      title += " - ";
   }
#endif

   CString label;   

   if (  m_pSecurityLabelMgr->UseTitleBarDisplay() )
   {
      label = m_pSecurityLabelMgr->TitlebarSecurityText();
   }

   else if (m_security_label_on_title_bar)
   {
      if (m_current_security_class.CompareNoCase("STANDARD") == 0)
         label = m_mru_standard_label;
      else if (m_current_security_class.CompareNoCase("CUSTOM") == 0)
         label = m_custom_label;
   }
   
   if (!label.IsEmpty())
   {
      title += label;
      title += " - ";
   }


   // append current overlay (if any), or "Map" to the title displayed in the
   // title bar
   C_overlay* ovl = OVL_get_overlay_manager()->get_current_overlay();

   if (ovl)
   {
      CString display_name =
         OVL_get_overlay_manager()->GetOverlayDisplayName(ovl);

      if (!display_name.IsEmpty())
      {
         long lReadOnly = 0;
         OverlayPersistence_Interface *pFvOverlayPersistence =
            dynamic_cast<OverlayPersistence_Interface *>(ovl);
         if (pFvOverlayPersistence != NULL)
            pFvOverlayPersistence->get_m_bIsReadOnly(&lReadOnly);

         if (lReadOnly)  // if the overlay is read-only...
            display_name += " [READ-ONLY]";

         if (ovl->is_modified())  // if overlay is dirty...
            display_name += "*";

         title += display_name;
      }
      else
         title += "Map";
   }
   else
      title += "Map";

   GetDocument()->SetTitle(title);
}
// end of update_title_bar

boolean_t MapView::is_draw_interrupted()
{
   //
   //  this stuff messes up print preview, so ignore it if printing or in 3D
   //
   if (m_printing || GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
      return FALSE;

   if (!AfxGetMainWnd())
      return FALSE;  // prevent access violation

   int result;

   if (m_user_interrupted_draw)
      result = TRUE;
   else
   {
      if (GetInputState())
      {
         BOOL interrupted = FALSE;

         MSG msg;

         // check for WM_KEYDOWN
         if (PeekMessage(&msg, AfxGetMainWnd()->m_hWnd, WM_KEYDOWN, WM_KEYDOWN,
            PM_NOREMOVE))
         {
            /*
            WM_KEYDOWN
            nVirtKey = (int) wParam;    // virtual-key code
            lKeyData = lParam;          // key data
            */

            if (msg.wParam == VK_ESCAPE ||
               msg.wParam == VK_PRIOR || msg.wParam == VK_NEXT ||
               msg.wParam == VK_LEFT || msg.wParam == VK_RIGHT ||
               msg.wParam == VK_UP || msg.wParam == VK_DOWN ||
               msg.wParam == VK_NUMPAD7 || msg.wParam == VK_NUMPAD8 ||
               msg.wParam == VK_NUMPAD9 || msg.wParam == VK_NUMPAD4 ||
               msg.wParam == VK_NUMPAD6 || msg.wParam == VK_NUMPAD1 ||
               msg.wParam == VK_NUMPAD2 || msg.wParam == VK_NUMPAD3)
            {
               interrupted = TRUE;

               // if ESC, remove message (because all escape does is abort
               // drawing and we've done that)
               if (msg.wParam == VK_ESCAPE)
                  PeekMessage(&msg, AfxGetMainWnd()->m_hWnd, WM_KEYDOWN,
                  WM_KEYDOWN, PM_REMOVE);
            }
         }

         // check for WM_MOUSEWHEEL
         if (!interrupted && PeekMessage(&msg, AfxGetMainWnd()->m_hWnd,
            WM_MOUSEWHEEL, WM_MOUSEWHEEL, PM_NOREMOVE))
            interrupted = TRUE;

         if (interrupted)
         {
            // we've been interrupted
            m_user_interrupted_draw = TRUE;
         }
      }

      result = m_user_interrupted_draw;
   }

   return result;
}
// end of is_draw_interrupted

// Used to check if the user has interrupted the draw.  This is the same as
// calling is_draw_interrupted(), except that it will also detect an ESC key
// press, even if the main window is not in focus.
boolean_t MapView::is_draw_interrupted_ex()
{
   //
   // this stuff messes up print preview, so ignore it if printing
   //
   if (m_printing)
      return FALSE;

   // if interruption has already been detected
   if (m_user_interrupted_draw)
      return TRUE;

   // if standard draw interrupt is detected
   if (is_draw_interrupted())
      return TRUE;

   MSG msg;

   // check for WM_KEYDOWN
   if (PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE))
   {
      // The Esc key is the only one that will cause an interrupt.
      if (msg.wParam == VK_ESCAPE)
      {
         // Since it is being used to interrupt the draw, it must be removed
         // from the message queue.  Otherwise you will get to actions from
         // one input.
         PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE);

         // Interrupt has been detected, so set the flag.
         m_user_interrupted_draw = TRUE;
      }
   }

   return m_user_interrupted_draw;
}
// end of is_draw_interrupted_ex

// Used to check if user has pressed the escape key
boolean_t MapView::is_escape_pressed()
{
   //
   // this stuff messes up print preview, so ignore it if printing
   //
   if (m_printing)
      return FALSE;

   MSG msg;
   boolean_t pressed = FALSE;

   // check for WM_KEYDOWN
   if (PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE))
   {
      // The Esc key is the only one that will cause an interrupt.
      if (msg.wParam == VK_ESCAPE)
      {
         // Since it is being used to interrupt the draw, it must be removed
         // from the message queue.  Otherwise you will get to actions from
         // one input.
         PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE);

         // Interrupt has been detected, so set the flag.
         pressed = TRUE;
      }
   }

   return pressed;
}
// end of is_escape_pressed

// Causes the rotation state for the view window to be re-evaluated.  If the
// map needs to be changed to North Up, it will be.  If the user's ability to
// rotate the map needs to be enabled or disabled, it will be.
void MapView::refresh_rotation_state(void)
{
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   boolean_t requires_north_up = ovl_mgr->requires_north_up();
   boolean_t disable_rotation_ui = ovl_mgr->disable_rotation_ui();

   external_enable_force_north_up_map(requires_north_up);
   external_disable_rotation_user_interface(disable_rotation_ui);
}
// end of refresh_rotation_state

// Causes the projection state for the view window to be re-evaluated. If the
// map projection needs to be changed to Eqiual Arc, it will be.  If the user's
// ability to change the map projection needs to be enabled or disabled, it will
// be.
void MapView::refresh_projection_state(void)
{
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   boolean_t requires_equal_arc = ovl_mgr->requires_equal_arc_projection();
   boolean_t disable_projection_ui = ovl_mgr->disable_projection_ui();

   external_enable_equal_arc_projection(requires_equal_arc);
   external_disable_projection_user_interface(disable_projection_ui);
}
// end of refresh_projection_state

// Causes the view map state for the view window to be re-evaluated. If the
// map needs to be changed to North Up, Equal Arc, etc., it will be. If the
// user's ability to change the map spec needs to be enabled or disabled, it
// will be.
void MapView::refresh_view_map_state(void)
{
   CRect rect;
   GetClientRect(&rect);

   if (rect.Width() == 0 || rect.Height() == 0)
      return;

   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   // Rotation state
   static double prev_rotation = 0.0;
   static bool prev_rotation_set = false;

   double rotation;

   const MapProjectionParams& map_proj_params = GetMapProjParams();

   // check to see if the overlays require rotation state change
   boolean_t requires_north_up = ovl_mgr->requires_north_up();
   boolean_t disable_rotation_ui = ovl_mgr->disable_rotation_ui();

   if (requires_north_up)
   {
      m_external_force_north_up = TRUE;

      // save the previous rotation to be able to restore it back
      if (map_proj_params.rotation != 0.0 && prev_rotation_set == false)
      {
         prev_rotation = map_proj_params.rotation;
         prev_rotation_set = true;
      }

      // constraint the map to North-Up
      rotation = 0.0;
   }
   else  // disable force north up
   {
      // if we need to remember the previous rotation
      if (prev_rotation_set == true)
      {
         rotation = prev_rotation;
         prev_rotation_set = false;
      }
      else
      {
         rotation = map_proj_params.rotation;
      }

      m_external_force_north_up = FALSE;
   }

   external_disable_rotation_user_interface(disable_rotation_ui);

   // Projection state
   static ProjectionEnum prev_projection = EQUALARC_PROJECTION;
   static bool prev_projection_set = false;

   ProjectionEnum projection;

   // check to see if the overlays require projection state change
   boolean_t requires_equal_arc = ovl_mgr->requires_equal_arc_projection();
   boolean_t disable_projection_ui = ovl_mgr->disable_projection_ui();

   if (requires_equal_arc)
   {
      m_external_force_equal_arc = TRUE;

      // save the previous projection to be able to restore it back
      if (map_proj_params.type != EQUALARC_PROJECTION
         && prev_projection_set == false)
      {
         prev_projection = map_proj_params.type;
         prev_projection_set = true;
      }

      // constraint the map to EqualArc projection
      projection = EQUALARC_PROJECTION;
   }
   else  // disable force equal arc
   {
      // if wee need to remember the previuous map projection
      if (prev_projection_set == true)
      {
         projection = prev_projection;
         prev_projection_set = false;
      }
      else
      {
         projection = map_proj_params.type;
      }

      m_external_force_equal_arc = FALSE;
   }

   external_disable_projection_user_interface(disable_projection_ui);

   // if the requested zoom percent was Native, then make sure we stay
   // at the Native zoom percent
   int zoom_percent;
   if (map_proj_params.zoom_percent == NATIVE_ZOOM_PERCENT)
      zoom_percent = NATIVE_ZOOM_PERCENT;
   else if (map_proj_params.zoom_percent == TO_SCALE_ZOOM_PERCENT)
      zoom_percent = TO_SCALE_ZOOM_PERCENT;
   else
      zoom_percent = map_proj_params.zoom_percent;

   // Now change all the accumulated map spec changes (rotation and projection)
   // at once if any of the map parameters have changed
   if (rotation != map_proj_params.rotation ||
       (zoom_percent != NATIVE_ZOOM_PERCENT &&
        zoom_percent != TO_SCALE_ZOOM_PERCENT &&
        zoom_percent != map_proj_params.zoom_percent) ||
       projection != map_proj_params.type)
   {

      MapProjectionParams new_map_proj_params = map_proj_params;
      new_map_proj_params.rotation = rotation;
      new_map_proj_params.zoom_percent = zoom_percent;
      new_map_proj_params.type = projection;

      int ret = ChangeMapType(GetMapGroupIdentity(), GetMapType(),
         new_map_proj_params);

      // SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA are possible
      if (ret != SUCCESS)
      {
         if (ret == FAILURE)
         {
            MapType map_type = GetMapType();

            CString error_string;
            error_string.Format("change_map_type failed: source = %s, "
               "scale = %s, series = %s, center = %f, %f, rot=%f, zoom = %d, "
               "proj = %s",
               map_type.get_source_string(),
               map_type.get_scale_string(),
               map_type.get_series_string(),
               new_map_proj_params.center.lat,
               new_map_proj_params.center.lon,
               rotation, zoom_percent, ProjectionID(projection).get_string());
            ERR_report(error_string);
         }
         else if (ret == COV_FILE_OUT_OF_SYNC)
         {
            INFO_report("Change map type returned Coverage File Out of Sync");
         }

         prev_projection_set = false;
         prev_rotation_set = false;
      }
      else
         ovl_mgr->invalidate_all();
   }
}

bool MapView::IsCursorInView()
{
   POINT pos;
   RECT rect;

   // get current cursor position in geo coordinates
   GetCursorPos(&pos);
   ScreenToClient(&pos);
   GetClientRect(&rect);
   if (PtInRect(&rect, pos))
   {
      return true;
   }

   return false;
}

void MapView::calc_erase_value_for_invalidation(BOOL& bErase,
   BOOL allow_animation_override)
{
#if TEST_INVALIDATION
   //
   // for testing purposes, set bErase to TRUE always.  This makes it easier to
   // see what is invalidated when and where and to spot invalidation
   // inefficiencies and problems.
   //
   bErase = TRUE;
#else

   // If the dc is not paletted, then never erase the screen.
   bErase = FALSE;

#endif
}
// end of calc_erase_value_for_invalidation

void MapView::invalidate_view(BOOL bErase, BOOL allow_animation_override,
   BOOL redraw_overlays_from_scratch_when_animating)
{
   clear_tool_tip();

   calc_erase_value_for_invalidation(bErase, allow_animation_override);

   if (m_current_map_rendering_engine != nullptr)
      m_current_map_rendering_engine->Invalidate();
   Invalidate(bErase);
}
// end of invalidate_view

void MapView::invalidate_view_rect(LPCRECT lpRect, BOOL bErase,
   BOOL allow_animation_override,
   BOOL redraw_overlays_from_scratch_when_animating)
{
   // clear_tool_tip();

   calc_erase_value_for_invalidation(bErase, allow_animation_override);

   if (m_current_map_rendering_engine != nullptr)
      m_current_map_rendering_engine->Invalidate();
   InvalidateRect(lpRect, bErase);
}
// end of invalidate_view_rect

void MapView::invalidate_view_rgn(CRgn* pRgn, BOOL bErase,
   BOOL allow_animation_override,
   BOOL redraw_overlays_from_scratch_when_animating)
{
   calc_erase_value_for_invalidation(bErase, allow_animation_override);

   if (m_current_map_rendering_engine != nullptr)
      m_current_map_rendering_engine->Invalidate();
   InvalidateRgn(pRgn, bErase);
}
// end of invalidate_view_rgn

void MapView::redraw_map_from_scratch()
{
   clear_tool_tip();

   std::for_each(m_map_rendering_engines, m_map_rendering_engines+2,
      [](MapRenderingEngine_Interface* map_rendering_engine)
   {
      map_rendering_engine->SetCurrentMapInvalid();
   });
   Invalidate();
}

BOOL MapView::OnEraseBkgnd(CDC* pDC)
{
   // The call to clear_tool_tip is made here for cases where
   // CWnd::Invalidate... gets called instead of MapView::invalidate_view_...
   // (which usually handle calling clear_tool_tip - which is more efficient
   // than calling it here).
   clear_tool_tip();

   // return TRUE so that the screen does not get painted black which
   // causes flashing whenever the screen is resized - this includes when
   // a user enters and leaves an editor mode
   return TRUE;
}
// end of OnEraseBkgnd

void MapView::SetContrast(double brightness, double contrast, int contrast_midval)
{
   MapDisplayParams map_display_params = m_map_params.GetMapDisplayParams();
   if (contrast < 1.0 && contrast > -CONTRAST_LIMIT &&
      brightness < BRIGHTNESS_LIMIT && brightness > -1*BRIGHTNESS_LIMIT)
   {
      m_current_map_rendering_engine->ChangeContrast(brightness, contrast, contrast_midval);
      set_current_map_invalid();
      m_map_params.SetContrast(brightness, contrast, contrast_midval);
      invalidate_view();
   }
}

void MapView::OnIncreaseContrast()
{
   MapDisplayParams map_display_params = m_map_params.GetMapDisplayParams();
   map_display_params.bw_contrast += 0.05;
   SetContrast(map_display_params.bw_brightness, map_display_params.bw_contrast,
      map_display_params.bw_contrast_midval);
}

void MapView::OnUpdateIncreaseContrast(CCmdUI* pCmdUI)
{
   const MapDisplayParams& map_display_params =
      m_map_params.GetMapDisplayParams();

   if (map_display_params.bw_contrast >= 1.0)
   {
      pCmdUI->Enable(FALSE);
      return;
   }

   const MapType& map_type = GetMapType();

   if ((map_type.get_source() == CIB ||
       map_type.get_series() == BLACK_AND_WHITE_SERIES)
       && !map_display_params.auto_enhance_CIB)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}
// end of OnUpdateIncreaseContrast

void MapView::OnDecreaseContrast()
{
   MapDisplayParams map_display_params = m_map_params.GetMapDisplayParams();
   map_display_params.bw_contrast -= 0.05;
   SetContrast(map_display_params.bw_brightness,
      map_display_params.bw_contrast, map_display_params.bw_contrast_midval);
}

void MapView::OnUpdateDecreaseContrast(CCmdUI* pCmdUI)
{
   const MapDisplayParams& map_display_params =
      m_map_params.GetMapDisplayParams();

   if (map_display_params.bw_contrast <= -1.0)
   {
      pCmdUI->Enable(FALSE);
      return;
   }

   const MapType& map_type = GetMapType();

   if ((map_type.get_source() == CIB ||
       map_type.get_series() == BLACK_AND_WHITE_SERIES)
       && !map_display_params.auto_enhance_CIB)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}
// end of OnUpdateDecreaseContrast

void MapView::OnViewAdjustBrightContrast()
{
   CScreenAdjust dlg;

   MapDisplayParams map_display_params =
      m_map_params.GetMapDisplayParams();

   dlg.m_brightness = map_display_params.bw_brightness;
   dlg.m_contrast = map_display_params.bw_contrast;
   dlg.m_nMidval = map_display_params.bw_contrast_midval;
   dlg.m_mapview = this;
   invalidate_view(FALSE);
   dlg.DoModal();
   invalidate_view(FALSE);

   map_display_params.bw_brightness = dlg.m_brightness;
   map_display_params.bw_contrast = dlg.m_contrast;
   map_display_params.bw_contrast_midval = dlg.m_nMidval;
   m_map_params.SetMapDisplayParams(map_display_params);
}

void MapView::OnViewDecreaseContrastCenterValue()
{
   const MapDisplayParams& display_params = m_map_params.GetMapDisplayParams();
   double contrast_midval = display_params.bw_contrast_midval;
   if (contrast_midval < 96)
   {
      contrast_midval += 5;
      m_map_engine->set_contrast(display_params.bw_contrast, contrast_midval);
      m_map_engine->SetCurrentMapInvalid();

      m_map_params.SetContrast(display_params.bw_brightness,
         display_params.bw_contrast, contrast_midval);

      invalidate_view(FALSE);
   }
}

void MapView::OnViewIncreaseContrastCenterValue()
{
   const MapDisplayParams& display_params = m_map_params.GetMapDisplayParams();
   double contrast_midval = display_params.bw_contrast_midval;
   if (contrast_midval > 6)
   {
      contrast_midval -= 5;
      m_map_engine->set_contrast(display_params.bw_contrast, contrast_midval);
      m_map_engine->SetCurrentMapInvalid();

      m_map_params.SetContrast(display_params.bw_brightness,
         display_params.bw_contrast, contrast_midval);

      invalidate_view(FALSE);
   }
}

void MapView::OnUpdateViewAdjustBrightContrast(CCmdUI* pCmdUI)
{
   const MapType& map_type = GetMapType();

   if ((map_type.get_source() == CIB ||
       map_type.get_series() == BLACK_AND_WHITE_SERIES)
       && !m_map_params.GetMapDisplayParams().auto_enhance_CIB)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}

// ******************************************************************
// ******************************************************************

void MapView::OnViewNormalBrightContrast()
{
   m_map_engine->set_contrast(0.0, 35);
   m_map_engine->SetCurrentMapInvalid();

   m_map_params.SetContrast(0.0, 0.0, 35);

   invalidate_view(FALSE);
}

// ******************************************************************
// ******************************************************************

void MapView::OnUpdateViewNormalBrightContrast(CCmdUI* pCmdUI)
{
   const MapType& map_type = m_map_params.GetMapType();
   const MapDisplayParams& map_display_params =
      m_map_params.GetMapDisplayParams();

   if ((map_type.get_source() == CIB ||
       map_type.get_series() == BLACK_AND_WHITE_SERIES)
       && !map_display_params.auto_enhance_CIB)
   {
      pCmdUI->Enable((map_display_params.bw_brightness != 0.0) || 
         (map_display_params.bw_contrast != 0.0));
   }
   else
   {
      pCmdUI->Enable(map_display_params.brightness != 0.0);
   }
}

void MapView::OnViewCalcBrightContrast()
{
   AutoBrightContrast();
}

void MapView::AutoBrightContrast()
{
   double brightness, contrast;
   int midval;

   // The 3D map is rendered as a series of tiles and needs to be computed
   // differently.
   if (m_current_map_rendering_engine != m_map_engine)
   {
      brightness = 0;
      contrast = 0.0;
      midval = 35.0;
   }
   else
   {
      m_map_engine->calculate_brightness_contrast(&brightness, &contrast,
         &midval);
   }

   SetContrast(brightness, contrast, midval);
}

void MapView::OnUpdateViewCalcBrightContrast(CCmdUI* pCmdUI)
{
   const MapType& map_type = GetMapType();

   if (map_type.get_source() == CIB ||
       map_type.get_series() == BLACK_AND_WHITE_SERIES)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}

void MapView::OnViewAutoBrightContrast()
{
   int auto_contrast = !GetMapDisplayParams().auto_enhance_CIB;
   m_map_params.SetAutoEnhanceCIB(auto_contrast);
   try
   {
      IRPFMapRenderOptionsPtr smpRPFOptions =
         m_map_engine->GetRenderingEngine()->GetMapHandler(_bstr_t("CIB"));
      smpRPFOptions->SetAutoEnhanceCIB(auto_contrast ? VARIANT_TRUE : VARIANT_FALSE);

      IGeoTiffMapRenderOptionsPtr smpGeoTiffOptions =
         m_map_engine->GetRenderingEngine()->GetMapHandler(_bstr_t("GeoTIFF"));
      smpGeoTiffOptions->SetAutoContrastEnabled(
         auto_contrast ? VARIANT_TRUE : VARIANT_FALSE);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("SetContrast failed : %s", (char *)e.Description());
      ERR_report(msg);
   }

   fvw::SetOptionsChanged(true);
   SetContrast(0.0, 0.0, 35);
   m_current_map_rendering_engine->SetCurrentMapInvalid();
}

void MapView::OnUpdateViewAutoBrightContrast(CCmdUI* pCmdUI)
{
   const MapType& map_type = GetMapType();

   if (map_type.get_source() == CIB ||
      map_type.get_series() == BLACK_AND_WHITE_SERIES)
   {
      pCmdUI->Enable(TRUE);
   }
   else
      pCmdUI->Enable(FALSE);

   const MapDisplayParams& map_display_params =
      m_map_params.GetMapDisplayParams();
   pCmdUI->SetCheck(map_display_params.auto_enhance_CIB);
}

int MapView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CView::OnCreate(lpCreateStruct) == -1)
      return -1;

   const BOOL bCopyPropertiesFromView = FALSE;
   const BOOL bInitCallback = TRUE;
   const BOOL bInitMapOptions = TRUE;
   const BOOL bInternalDraw = TRUE;
   if (m_map_engine->init(bCopyPropertiesFromView, bInitCallback,
      bInitMapOptions, bInternalDraw, GetSafeHwnd()) != SUCCESS)
   {
      ERR_report("Error initializing map engine");

      delete m_view_map_proj;
      m_view_map_proj = NULL;

      return FAILURE;
   }

   pActiveViewMapEngine = m_map_engine;

   m_view_map_proj->set_map(m_map_engine->get_curr_map());

   // register OLE Drag/Drop
   m_drop_target.Register(this);
   return 0;
}

int MapView::InitializeMap()
{
   InitializeMapRenderingEngines();

   // Try to set the requested map in the view to the map spec that was
   // previously saved in the registry (if any).
   //
   MapViewTabPrefs map_view_tab_prefs;

   long map_group_identity;
   MapType map_type;
   MapProjectionParams map_proj_params;
   std::string unused_tab_name;
   map_view_tab_prefs.Read(0, &map_group_identity, &map_type, &map_proj_params,
      &unused_tab_name);

   // Change to an appropriate projection if 3D is not supported
   if (!m_3D_rendering_supported && IsRendering3d(map_proj_params.type))
   {
      map_proj_params.type = EQUALARC_PROJECTION;
   }

   m_map_params.Initialize(map_group_identity, map_type, map_proj_params);

   m_current_map_rendering_engine =
      GetMapRenderingEngineFromProjection(GetProjectionType());

   m_current_map_rendering_engine->EnableDraw(true);

   CRect rect;
   GetClientRect(&rect);
   m_current_map_rendering_engine->SetSurfaceDimensions(rect.Width(),
      rect.Height());

   int status = m_current_map_rendering_engine->ChangeMapType(
      GetMapGroupIdentity(), GetMapType(), GetMapProjParams());

   if (IsRendering3d())
   {
      m_map_rendering_engines[MAP_RENDERING_ENGINE_COM_INDEX]->SetSurfaceDimensions(rect.Width(),
      rect.Height());
      m_map_rendering_engines[MAP_RENDERING_ENGINE_COM_INDEX]->ChangeMapType(
      GetMapGroupIdentity(), GetMapType(), GetMapProjParams());
   }

   // TODO: Handle failure, falling back to various map type as follows
   //
   // Closest map with same source
   // Blank map with same scale
   // TIROS World (equal arc)
   // Blank World (equal arc)
   //

   m_map_initialized = true;

   // Now that the map type has been set up, start the rendering thread
   m_geospatial_view_controller->BeginRendering();

   return SUCCESS;
}

void MapView::InitializeMapRenderingEngines()
{
   m_map_rendering_engines[MAP_RENDERING_ENGINE_COM_INDEX] =
      m_map_engine;

   m_map_rendering_engines[MAP_RENDERING_ENGINE_OSG_INDEX] =
      new MapEngineOSG(m_geospatial_view_controller);
}

void MapView::OnCoordCopy()
{
   // ...
   // Get the currently selected data
   HGLOBAL HGlobalMem;
   unsigned char *ptr;
   d_geo_t location;
   const int STR_LEN = 100;
   char s[STR_LEN], s2[STR_LEN];
   unsigned char ch;
   int len, cnt, k;

   // If the cursor is not in the MapView, then use the center of the map
   if (!IsCursorInView() || !m_cursor_position_geo_valid)
   {
      location = GetMapProjParams().center;
   }
   else
      location = m_cursor_position_geo;

   // convert it to a string in the new format
   GEO_lat_lon_to_string(location.lat, location.lon, s, STR_LEN);

   // strip the deg, min, and sec symbols
   len = strlen(s);
   cnt = 0;
   for (k = 0; k < len; k++)
   {
      ch = s[k];
      if ((ch != 0x22) && (ch != 0x27) && (ch != 0xb0))
      {
         s2[cnt] = ch;
         cnt++;
      }
   }
   s2[cnt] = '\0';
   cnt++;

   HGlobalMem = ::GlobalAlloc(GMEM_MOVEABLE, cnt);

   ptr = (unsigned char*)::GlobalLock(HGlobalMem);

   CopyMemory(ptr, s2, cnt);

   if (!OpenClipboard())
   {
       ERR_report("OpenClipboard");
      ::GlobalUnlock(HGlobalMem);
      return;
   }
   // Remove the current Clipboard contents
   if (!::EmptyClipboard())
   {
       ERR_report("EmptyClipboard");
      ::GlobalUnlock(HGlobalMem);
      ::CloseClipboard();
      return;
   }

   // For the appropriate data formats...
   if (::SetClipboardData(CF_TEXT, HGlobalMem) == NULL)
   {
       ERR_report("SetClipBoardData");
      ::GlobalUnlock(HGlobalMem);
      ::CloseClipboard();
      return;
   }
   ::GlobalUnlock(HGlobalMem);
   CloseClipboard();
}
// end of OnCoordCopy

// ******************************************************************
// ******************************************************************

// Copy/Paste routines
void MapView::OnEditCopy()
{
   // Get a pointer to the current overlay
   C_overlay *pOverlay= OVL_get_overlay_manager()->get_current_overlay();
   if (pOverlay != NULL)
      pOverlay->copy_to_clipboard();
}

void MapView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
   boolean_t enable = FALSE;

   // Get a pointer to the current overlay
   C_overlay *pOverlay= OVL_get_overlay_manager()->get_current_overlay();
   if (pOverlay != NULL)
      enable = pOverlay->is_copy_to_clipboard_allowed();

   pCmdUI->Enable(enable);
}

void MapView::OnEditPaste()
{
   // Get a pointer to the current overlay
   C_overlay *pOverlay= OVL_get_overlay_manager()->get_current_overlay();
   if (pOverlay != NULL)
      pOverlay->paste_from_clipboard();
}

void MapView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
   boolean_t enable = FALSE;

   // Get a pointer to the current overlay
   C_overlay *pOverlay= OVL_get_overlay_manager()->get_current_overlay();
   if (pOverlay != NULL)
      enable = pOverlay->is_paste_from_clipboard_allowed();

   pCmdUI->Enable(enable);
}

//
// This call is to be used to enable an "external" entity (e.g. an overlay)
// to force the map be north-up (and to then reset this behavior).  A value of
// TRUE for enable increments the counter that keep track of how many external
// entities have rotation disabled and cause the map to be redrawn from scratch.
// A value of FALSE for enable decrements the counter.
//
void MapView::external_enable_force_north_up_map(boolean_t enable)
{
   if (enable)
   {
      m_external_force_north_up = TRUE;

      if (get_curr_map()->actual_rotation() != 0.0)
      {
         m_map_engine->change_rotation(0.0);
      }
   }
   else  // disable force north up
   {
      m_external_force_north_up = FALSE;
   }
}

//
// This call is to be used to enable an "external" entity (e.g. an overlay)
// to disable the rotation user interface (and to then reset this behavior).
// A value of TRUE for disable increments the counter that keep track of how
// many external entities have the rotation UI disabled.  A value of FALSE for
// disable decrements the counter.
//
void MapView::external_disable_rotation_user_interface(boolean_t disable)
{
   if (disable)
   {
      m_external_disable_rotation_ui = TRUE;
      enable_rotation_user_interface(FALSE);
   }
   else
   {
      m_external_disable_rotation_ui = FALSE;
      enable_rotation_user_interface(TRUE);
   }
}

//
// enable_rotation_user_interface enables or disables the view
// user interface capabilities for rotation.  In other words, it controls
// whether, for example, rotation menu items (and associated accelerator keys)
// are enabled or disabled.
//
void MapView::enable_rotation_user_interface(boolean_t enable)
{
   UINT flag = enable ? MF_ENABLED : MF_DISABLED;

   CWnd *pMainWnd = AfxGetMainWnd();

   //
   // enable or disable the menu items (and the associated accelerator keys)
   // as desired
   //
   if (pMainWnd)
   {
      CMenu *pMenu = pMainWnd->GetMenu();
      if (pMenu)
      {
         pMenu->EnableMenuItem(ID_MAP_ROTATE_CLOCKWISE, flag);
         pMenu->EnableMenuItem(ID_MAP_ROTATE_COUNTERCLOCKWISE, flag);
      }
   }
}


// This call is to be used to enable an "external" entity (e.g. an overlay)
// to force the map be north-up (and to then reset this behavior).  A value of
// TRUE for enable increments the counter that keep track of how many external
// entities have rotation disabled and cause the map to be redrawn from scratch.
// A value of FALSE for enable decrements the counter.
//
void MapView::external_enable_equal_arc_projection(boolean_t enable)
{
   if (enable)
   {
      m_external_force_equal_arc = TRUE;

      if (GetMapProjParams().type != EQUALARC_PROJECTION)
      {
         ChangeProjectionType(EQUALARC_PROJECTION);
      }
   }
   else  // disable force north up
   {
      m_external_force_equal_arc = FALSE;
   }
}


// This call is to be used to enable an "external" entity (e.g. an overlay)
// to disable the projection user interface (and to then reset this behavior).
// A value of TRUE for disable increments the counter that keep track of how
// many external entities have the rotation UI disabled.  A value of FALSE for
// disable decrements the counter.
//
void MapView::external_disable_projection_user_interface(boolean_t disable)
{
   if (disable)
   {
      m_external_disable_projection_ui = TRUE;
   }
   else
   {
      m_external_disable_projection_ui = FALSE;
   }
}

int MapView::ChangeToClosestScale(long group_identity)
{
   MapType new_map_type;
   int ret = m_current_map_rendering_engine->ChangeToClosestScale(
      group_identity, &new_map_type);
   if (ret == SUCCESS)
   {
      m_map_params.SetMapType(group_identity, new_map_type);
      UpdateContrast();
      invalidate_view(FALSE);
   }

   return ret;
}

int MapView::ChangeToClosestScale(const MapCategory& cat)
{
   return ChangeToClosestScale(cat.GetGroupId());
}

int MapView::ChangeToClosestScale(const MapCategory& cat, const d_geo_t& center)
{
   MapType new_map_type;
   int ret = m_current_map_rendering_engine->ChangeToClosestScale(
      cat, center, &new_map_type);
   if (ret == SUCCESS)
   {
      m_map_params.SetMapType(cat.GetGroupId(), new_map_type, center);
      UpdateContrast();
      invalidate_view(FALSE);
   }

   return ret;
}

int MapView::ChangeToClosestScale(const MapCategory& category,
   const MapScale& scale, const MapProjectionParams& map_proj_params)
{
   bool projection_changed = SetCurrentRenderingEngineFromProjection(map_proj_params.type);

   MapType new_map_type;
   int ret = m_current_map_rendering_engine->ChangeToClosestScale(
      category, scale, map_proj_params, &new_map_type);
   if (ret == SUCCESS)
   {
      m_map_params.SetMapTypeAndProjectionParams(
         category.GetGroupId(), new_map_type, map_proj_params);
      UpdateContrast();
      ResizeOrInvalidateView(projection_changed);
   }

   return ret;
}

int MapView::ChangeToClosestMapType(long group_identity,
   const MapType& map_type, const MapProjectionParams& map_proj_params)
{
   bool projection_changed = SetCurrentRenderingEngineFromProjection(map_proj_params.type);

   MapType new_map_type;
   int ret = m_current_map_rendering_engine->ChangeToClosestMapType(
      group_identity, map_type, map_proj_params, &new_map_type);
   if (ret == SUCCESS)
   {
      m_map_params.SetMapTypeAndProjectionParams(group_identity, new_map_type,
         map_proj_params);
      UpdateContrast();
      ResizeOrInvalidateView(projection_changed);
   }

   return ret;
}

int MapView::ChangeMapType(long group_identity, const MapType& map_type)
{
   int ret = m_current_map_rendering_engine->ChangeMapType(group_identity,
      map_type);

   if (ret == SUCCESS)
   {
      m_map_params.SetMapType(group_identity, map_type);
      UpdateContrast();
      invalidate_view(FALSE);
   }

   return ret;
}

int MapView::ChangeMapType(long group_identity, const MapType& map_type,
   const d_geo_t& center)
{
   int ret = m_current_map_rendering_engine->ChangeMapType(group_identity,
      map_type, center);

   if (ret == SUCCESS)
   {
      m_map_params.SetMapType(group_identity, map_type, center);
      UpdateContrast();
      invalidate_view(FALSE);
   }

   return ret;
}

bool MapView::SetCurrentRenderingEngineFromProjection(
   ProjectionEnum projection_type, bool allow_camera_change /*TRUE*/ )
{
   MapRenderingEngine_Interface* rendering_engine =
      GetMapRenderingEngineFromProjection(projection_type);

   if (m_current_map_rendering_engine != rendering_engine)
   {
      if (projection_type == GENERAL_PERSPECTIVE_PROJECTION)
      {
         // Deactivate editor mode when changing to 3D.
         //
         // Note that this needs to be done before swapping the rendering
         // engine. There are cases where set_mode can trigger a call to change
         // the map type. When this happens, the renderer will be 2D but the
         // current projection will be 3D.
         GUID editor_mode = OVL_get_overlay_manager()->GetCurrentEditor();
         if (editor_mode != GUID_NULL && editor_mode != FVWID_Overlay_Route)
         {
            OVL_get_overlay_manager()->set_mode(GUID_NULL);
         }

         // Deactivate Vertical View window
         CMainFrame* frame = fvw_get_frame();
         if (frame && frame->GetVerticalDisplay())
            frame->ToggleVerticalView(TRUE);
         // until favorites encase the camera mode, will always 
         // set camera to PAN for the favorite
         if (allow_camera_change)
         {
            if (fvw_get_view()->GetGeospatialViewController()->
               GetInputDevice()->GetActiveCam() == fvw::CAMERA_MODE_NONE)
            {
               GetGeospatialViewController()->
                  GetInputDevice()->SetActiveCam(fvw::CAMERA_MODE_PAN);
               m_geospatial_view_controller->GetGeospatialView()->m_viewer->
                  setCameraManipulator(m_geospatial_view_controller->
                  GetGeospatialView()->m_camera_earth_manipulator, false);
            }
         }
      }

      m_current_map_rendering_engine->EnableDraw(false);
      m_current_map_rendering_engine = rendering_engine;
      m_current_map_rendering_engine->EnableDraw(true);

      return true;
   }

   return false;
}

void MapView::ResizeOrInvalidateView(bool changed_proj_type)
{
   if (changed_proj_type)
   {
      // Required since the rendering engine may have been ignoring
      // WM_SIZE messages while inactive.
      WORD surface_width = GetMapDisplayParams().surface_width;
      WORD surface_height = GetMapDisplayParams().surface_height;
      SendMessage(WM_SIZE, 0, MAKELPARAM(surface_width, surface_height));
   }
   else
      invalidate_view(FALSE);
}

int MapView::ChangeMapType(long group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      boolean_t override_no_data /*=FALSE*/)
{
   bool changed_proj_type = SetCurrentRenderingEngineFromProjection(
      map_proj_params.type);

   if (group_identity == 0)
   {
      group_identity = MAP_get_category(map_type.get_source()).GetGroupId();
   }

   int ret = m_current_map_rendering_engine->ChangeMapType(group_identity,
      map_type, map_proj_params, override_no_data);

   if (ret == SUCCESS)
   {
      m_map_params.SetMapTypeAndProjectionParams(group_identity, map_type,
         map_proj_params);
      UpdateContrast();

      ResizeOrInvalidateView(changed_proj_type);
   }

   return ret;
}

void MapView::ChangeProjectionType(ProjectionEnum projection_type)
{
   if (!IsProjectionSupported(projection_type))
      return;

   int status = SUCCESS;

   ProjectionEnum old_proj = GetMapProjParams().type;
   // if changing 3D to 2D, must handle transitions before shutting down the 3D engine
   if (GetMapProjParams().type == GENERAL_PERSPECTIVE_PROJECTION && 
      projection_type != GENERAL_PERSPECTIVE_PROJECTION)
   {
      m_geospatial_view_controller->GetInputDevice()->
         SetCameraMode(fvw::CAMERA_MODE_NONE);
   }

   bool projection_changed = SetCurrentRenderingEngineFromProjection(projection_type, false);
   if (projection_changed)
   {
      // Reinitialize the rendering engine from the current map parameters
      // with the new projection type
      //

      // Zoom changes from 3D -> 2D transitions must occur outside of the thread lock
      // as there are listeners that listen to zoom changes on this thread.
      if (projection_changed && old_proj == GENERAL_PERSPECTIVE_PROJECTION)
      {
         if (m_geospatial_view_controller->GetInputDevice()->m_old_cameraMode == 
            fvw::CAMERA_MODE_PAN)
         {
            m_map_params.SetZoomPercent(m_geospatial_view_controller->
               GetGeospatialView()->m_camera_earth_manipulator->m_desired_zoom_percent);
         }
         else if (m_geospatial_view_controller->GetInputDevice()->m_old_cameraMode == 
            fvw::CAMERA_MODE_FLIGHT)
         {
            m_map_params.SetZoomPercent(m_geospatial_view_controller->
               GetGeospatialView()->m_camera_flight_manipulator->m_desired_zoom_percent);
         }
         else if (m_geospatial_view_controller->GetInputDevice()->m_old_cameraMode == 
            fvw::CAMERA_MODE_GROUND_VIEW)
         {
            m_map_params.SetZoomPercent(m_geospatial_view_controller->
               GetGeospatialView()->m_camera_walk_manipulator->m_desired_zoom_percent);
         }
      }

      MapProjectionParams proj_params = GetMapProjParams(); 
      proj_params.type = projection_type;

      status = m_current_map_rendering_engine->ReinitializeFromMapParams(
         GetMapGroupIdentity(), GetMapType(), proj_params,
         GetMapDisplayParams());
      UpdateContrast();

      // Fall back to TIROS World if there is no data available
      if (status == FV_NO_DATA)
      {
         proj_params.center.lat = 0.0;
         proj_params.center.lon = 0.0;
         proj_params.zoom_percent = 100;
         proj_params.rotation = 0.0;
         status = m_current_map_rendering_engine->ReinitializeFromMapParams(
            RASTER_GROUP_ID, MapType(TIROS, WORLD, "TopoBath"),
            proj_params, GetMapDisplayParams());
         UpdateContrast();
      }

      ResizeOrInvalidateView(projection_changed);
   }
   else if (!curr_map_projection_ui_disabled())
   {
      status = m_current_map_rendering_engine->ChangeProjectionType(
         projection_type);
   }

   if (status == SUCCESS)
   {
      m_map_params.SetProjectionType(projection_type);
      invalidate_view(FALSE);

      if (projection_changed && projection_type == GENERAL_PERSPECTIVE_PROJECTION)
      {
         m_geospatial_view_controller->GetInputDevice()->
            SetCameraMode(fvw::CAMERA_MODE_PAN);
      }
   }
   else
      OnMapError(status);
}

int MapView::ChangeToSmallestScale(const MapCategory& category,
   const MapProjectionParams& map_proj_params)
{
   bool projection_changed = SetCurrentRenderingEngineFromProjection(map_proj_params.type);

   MapType new_map_type;
   int ret = m_current_map_rendering_engine->ChangeToSmallestScale(category,
      map_proj_params, &new_map_type);

   if (ret == SUCCESS)
   {
      m_map_params.SetMapTypeAndProjectionParams(category.GetGroupId(),
         new_map_type, map_proj_params);
      UpdateContrast();
      ResizeOrInvalidateView(projection_changed);
   }

   return ret;
}

int MapView::ChangeToLargestScale(const MapCategory& category,
   const MapProjectionParams& map_proj_params)
{
   bool projection_changed = SetCurrentRenderingEngineFromProjection(map_proj_params.type);

   MapType new_map_type;
   int ret = m_current_map_rendering_engine->ChangeToLargestScale(category,
      map_proj_params, &new_map_type);

   if (ret == SUCCESS)
   {
      m_map_params.SetMapTypeAndProjectionParams(category.GetGroupId(),
         new_map_type, map_proj_params);
      UpdateContrast();
      ResizeOrInvalidateView(projection_changed);
   }

   return ret;
}

int MapView::ChangeToBest(const degrees_t& lat, const degrees_t& lon)
{
   MapType new_map_type;
   d_geo_t center;
   center.lat = lat;
   center.lon = lon;
   int ret = m_current_map_rendering_engine->
      ChangeToBest(center, &new_map_type);

   if (ret == SUCCESS)
   {
      m_map_params.SetMapType(new_map_type, center);
      UpdateContrast();
      invalidate_view(FALSE);
   }

   return ret;
}

int MapView::ChangeToBest(const MapSource& source, const MapScale& scale,
   const MapProjectionParams& map_proj_params)
{
   bool projection_changed = SetCurrentRenderingEngineFromProjection(map_proj_params.type);

   MapType new_map_type;
   int ret = m_current_map_rendering_engine->ChangeToBest(source, scale,
      map_proj_params, &new_map_type);

   if (ret == SUCCESS)
   {
      m_map_params.SetMapTypeAndProjectionParams(
         MAP_get_category(new_map_type.get_source()).GetGroupId(),
         new_map_type, map_proj_params);
      UpdateContrast();
      ResizeOrInvalidateView(projection_changed);
   }

   return ret;
}


void MapView::OnFavoritesAdd()
{
   AddFavoriteDlg dlg;

   dlg.SetMapParams(GetMapGroupIdentity(), GetMapType(), GetMapProjParams());

   // [Bug 1215].  Locking the temporary maps is necessary to work around an MFC
   // bug in COccManager::IsDialogMessage when a dialog contains an ActiveX
   // control
   AfxLockTempMaps();
   dlg.DoModal();
   AfxUnlockTempMaps();
}

void MapView::OnFavoritesOrganize()
{
   OrgFavoriteDlg dlg;

   dlg.DoModal();
}

void MapView::OnUpdateFavoritesList(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void MapView::OnUpdateOverlayList(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void MapView::OnUpdateMapMenu(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void MapView::UpdateZoomFromCombo(CMFCRibbonComboBox* pCombo)
{
   if (pCombo == nullptr)
      return;

   int zoom_percent;
   const int sel = pCombo->GetCurSel();
   if (sel != CB_ERR && pCombo->GetItem(sel) == pCombo->GetEditText())
   {
      zoom_percent = static_cast<int>(pCombo->GetItemData(sel));
   }
   else
   {
      zoom_percent = atoi(pCombo->GetEditText());

      if (zoom_percent < MapView::MIN_ZOOM_PERCENT ||
         zoom_percent > MapView::MAX_ZOOM_PERCENT)
      {
         CString msg;
         msg.Format("The number must be between %d and %d.",
            MapView::MIN_ZOOM_PERCENT, MapView::MAX_ZOOM_PERCENT);
         AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);
      }
   }

   const int status =
      m_current_map_rendering_engine->ChangeZoomPercent(zoom_percent);
   if (status == SUCCESS)
   {
      invalidate_view(FALSE);
      m_map_params.SetZoomPercent(zoom_percent);
   }
   else
      OnMapError(status);
}

LRESULT MapView::OnInvalidateLayerOverlay(WPARAM wParam, LPARAM lParam)
{
   long layer_handle = (long)lParam;

   // specifing -1 for the layer handle will invalidate the entire screen
   if (layer_handle == -1)
   {
      OVL_get_overlay_manager()->invalidate_all();
      return 0;
   }


   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return 0;

   // make sure this is a CBaseLayerOvl pointer or c_gps_trail
   CBaseLayerOvl* pBaseLayerOvl = dynamic_cast<CBaseLayerOvl *>(overlay);
   if (pBaseLayerOvl == NULL &&
      overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return 0;

   // if the overlay implements ILayerEditor3, then we'll need to invalidate the
   // overlay, rather than areas of the
   // screen affected by changes to layer objects
   if (pBaseLayerOvl != NULL && pBaseLayerOvl->get_ILayerEditor() != NULL)
   {
      if (pBaseLayerOvl->get_ILayerEditor()->GetInterfaceVersion() >= 3)
      {
         OVL_get_overlay_manager()->InvalidateOverlay(overlay);
      }
   }

   overlay->GetOvlElementContainer()->invalidate();

   return 0;
}

// Routines for supporting OLE drag and drop
BOOL MapView::OnDrop(COleDataObject* data_object, DROPEFFECT drop_effect,
                        CPoint pt)
{
   ASSERT_VALID(this);
   OnDragLeave();

   boolean_t paste_success = FALSE;

   ViewMapProj* map = UTL_get_current_view_map(this);
   if (map)
   {
      // NB: should be interrupt the draw here?

      paste_success =
         OVL_get_overlay_manager()->paste_OLE_data_object(map, pt, data_object);
   }

   if (!paste_success)
   {
      paste_success = m_customDropTarget->OnDrop(GetFvMapView(), data_object);
   }

   return paste_success;
}

DROPEFFECT MapView::OnDragEnter(COleDataObject* data_object,
   DWORD key_state, CPoint point)
{
   // ASSERT(m_prev_drop_effect == DROPEFFECT_NONE);
   return OnDragOver(data_object, key_state, point);
}

//-----------------------------------------------------------------------------
DROPEFFECT MapView::OnDragOver(COleDataObject* data_object,
   DWORD key_state, CPoint point)
{
   DROPEFFECT drop_effect = DROPEFFECT_NONE;

   if ( data_object->IsDataAvailable(CF_HDROP) || 
               m_customDropTarget->OnDragOver(data_object))
   {
      drop_effect = DROPEFFECT_COPY;
   }

   if (OVL_get_overlay_manager()->can_drop_data(data_object, point))
   {
      OnOLEDrag(data_object, point);

         // check for force link
      if ((key_state & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
         drop_effect = DROPEFFECT_COPY;
      // check for force copy
      else if ((key_state & MK_CONTROL) == MK_CONTROL)
         drop_effect = DROPEFFECT_COPY;
      // check for force move
      else if ((key_state & MK_ALT) == MK_ALT)
         drop_effect = DROPEFFECT_MOVE;
      // if dropping on self make default be move
      else
         drop_effect = DROPEFFECT_MOVE;
   }

   // m_prevDropEffect = dropEffect;
   return drop_effect;
}

//-----------------------------------------------------------------------------
void MapView::OnDragLeave()
{
   // m_prevDropEffect = DROPEFFECT_NONE;
}

//
// This function returns whether the rotation user interface elements
// for the current map have been disabled.
//
boolean_t MapView::curr_map_rotation_ui_disabled(void) const
{
   return (TO_BOOLEAN_T(m_external_disable_rotation_ui));
}

boolean_t MapView::curr_map_is_forced_north_up(void)
{
   return (TO_BOOLEAN_T(m_external_force_north_up));
}

void MapView::OnEditCopyMap()
{
   if (m_map_engine->copy_map_to_clipboard() != SUCCESS)
       AfxMessageBox("Can not copy to the clipboard");
}

afx_msg void MapView::OnUpdateEditCopyMapDisableRotated(CCmdUI* pCmdUI)
{
   // Disable if the map is rotated, the screen crosses international date
   // line, or the projection is not equal arc.
   BOOL enable = !IsRendering3d();

   const MapProj* curr_map = get_curr_map();
   enable = enable && curr_map->actual_rotation() == 0.0;

   if (enable)
   {
      d_geo_t vmap_ll, vmap_ur;
      curr_map->get_vmap_bounds(&vmap_ll, &vmap_ur);
      enable = (vmap_ll.lon < vmap_ur.lon);
   }

   enable = enable && (curr_map->actual_projection() == EQUALARC_PROJECTION);

   pCmdUI->Enable(enable);
}

void MapView::OnEditCopyMapToKMZ()
{
   // display Save As dialog box to allow user to select file name
   CFileDialog file_dlg(FALSE, "kmz", NULL,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      "KMZ Files (*.kmz)|*.kmz|All Files (*.*)|*.*||", NULL);

GET_FILE_NAME:
   if (file_dlg.DoModal() != IDOK)
      return;

   // retrieve file name and test opening for binary writing
   CString file_name = file_dlg.GetPathName();
   FILE *fp = NULL;
   fopen_s(&fp, file_name, "wb");
   if (fp == NULL)
   {
      MessageBox("Error opening file for writing!", MB_OK);
      goto GET_FILE_NAME;
   }
   fclose(fp);

   if (m_map_engine->save_KMZ(file_name) != SUCCESS)
   {
      ERR_report("Failed saving KMZ");
   }
}

void MapView::OnEditCopyMapToGeoTIFF()
{
   // display Save As dialog box to allow user to select file name
   CFileDialog file_dlg(FALSE, "tif", NULL,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      "TIFF Files (*.tif)|*.tif|All Files (*.*)|*.*||", NULL);

GET_FILE_NAME:
   if (file_dlg.DoModal() != IDOK)
      return;

   // retrieve file name and test opening for binary writing
   CString file_name = file_dlg.GetPathName();
   FILE *fp = NULL;
   fopen_s(&fp, file_name, "wb");
   if (fp == NULL)
   {
      MessageBox("Error opening file for writing!", MB_OK);
      goto GET_FILE_NAME;
   }
   fclose(fp);

   if (m_map_engine->save_GeoTIFF(file_name) != SUCCESS)
   {
      ERR_report("Failed saving GeoTIFF");
   }
}

void MapView::OnEditCopyMapToJPG()
{
   // display Save As dialog box to allow user to select file name
   CFileDialog file_dlg(FALSE, "jpg", NULL,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      "JPG Files (*.jpg)|*.jpg|All Files (*.*)|*.*||", NULL);

GET_FILE_NAME:
   if (file_dlg.DoModal() != IDOK)
      return;

   // retrieve file name and test opening for binary writing
   CString file_name = file_dlg.GetPathName();
   FILE *fp = NULL;
   fopen_s(&fp, file_name, "wb");
   if (fp == NULL)
   {
      MessageBox("Error opening file for writing!", MB_OK);
      goto GET_FILE_NAME;
   }
   fclose(fp);

   if (m_map_engine->save_JPG(file_name, 80) != SUCCESS)
   {
      ERR_report("Failed saving JPG");
   }
}

void MapView::OnEditCopyMapToPNG()
{
   // display Save As dialog box to allow user to select file name
   CFileDialog file_dlg(FALSE, "png", NULL,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      "PNG Files (*.png)|*.png|All Files (*.*)|*.*||", NULL);

GET_FILE_NAME:
   if (file_dlg.DoModal() != IDOK)
      return;

   // retrieve file name and test opening for binary writing
   CString file_name = file_dlg.GetPathName();
   FILE *fp = NULL;
   fopen_s(&fp, file_name, "wb");
   if (fp == NULL)
   {
      MessageBox("Error opening file for writing!", MB_OK);
      goto GET_FILE_NAME;
   }
   fclose(fp);

   if (m_map_engine->save_PNG(file_name) != SUCCESS)
   {
      ERR_report("Failed saving PNG");
   }
}

void MapView::OnToggleCenterCrosshair()
{
   m_bDisplayCenterCrosshair = !m_bDisplayCenterCrosshair;

   // If the center-crosshair is now enabled, either add it to the
   // overlay elements associated with the map view or, if the screen overlay
   // is already created, toggle the visibility

   OverlayElements *overlay_elements = GetOverlayElements();

   if (overlay_elements != nullptr)
   {
      if (m_bDisplayCenterCrosshair)
      {
         if (m_center_crosshair_handle == -1)
         {
            CString filename = PRM_get_registry_string(
               "Main", "ReadOnlyAppData", "");
            filename += CENTER_CROSSHAIR_FILENAME;

            // Create stream from center-crosshair resource
            IStream* fileStream;
            HRESULT hr =
               ::SHCreateStreamOnFile(filename, STGM_READ, &fileStream);
            if (FAILED(hr))
            {
               CString msg;
               msg.Format("OnToggleCenterCrosshair failed: Unable to load %s ",
                  filename);
               ERR_report(msg);
               return;
            }

            m_center_crosshair_handle =
               overlay_elements->AddScreenOverlay(fileStream);

            fileStream->Release();
         }
         else
         {
            overlay_elements->ShowScreenOverlay(m_center_crosshair_handle,
               true);
         }
      }

      // If the center-crosshair is now disabled and has been added, then toggle
      // its visibility
      else if (m_center_crosshair_handle != -1)
      {
         overlay_elements->ShowScreenOverlay(m_center_crosshair_handle,
            false);
      }
   }

   invalidate_view();
}

void MapView::OnUpdateCenterCrosshair(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(m_bDisplayCenterCrosshair);
}

void MapView::OnSearchBar()
{
   CMainFrame* pFrame = fvw_get_frame();
   if (!pFrame)
      return;

   CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit,
      pFrame->m_wndRibbonBar. FindByID(IDC_SEARCH_BAR));

   if (pEdit)
   {
      HandleSearchRequest(pEdit->GetEditText());

      // reset the search bar's text back to the empty string
      pEdit->SetEditText("");
   }
}

void MapView::HandleSearchRequest(const CString& searchRequest)
{
   CWaitCursor waitCursor;

   // no need to process an empty string
   if (searchRequest.GetLength() == 0)
      return;

   d_geo_t center = { 0.0, 0.0 };

   d_geo_t geo;
   CString shpFilename;

   // if the search request is a shape file search
   if (C_shp_ovl::is_shape_search_key(searchRequest))
   {
      if (C_shp_ovl::search(searchRequest, &geo, shpFilename) == FAILURE)
      {
         AfxMessageBox("No match found");
         return;
      }

      center.lat = geo.lat;
      center.lon= geo.lon;
   }
   else
   {
      // first, we'll try to parse the request as a location
      CGeoTrans geotrans;
      char geo_string[GEO_MAX_LAT_LON_STRING+1];
      if (geotrans.DLL_location_to_geo(
         searchRequest, "WGS84", center.lat, center.lon, geo_string) != SUCCESS)
      {
         // parsing the request as a location failed.  Next, we'll try to parse
         // it as a fix/point
         CFvwUtil* pFvwUtil = CFvwUtil::get_instance();
         SnapToInfo snapToInfo;
         long result = pFvwUtil->db_lookup(CString(searchRequest), snapToInfo, TRUE);
         if (result > 0)
         {
            center.lat = snapToInfo.m_lat;
            center.lon = snapToInfo.m_lon;
         }
         else if (result < 0)
         {
            // The user cancelled.  Return without an error message.
            return;
         }
         // if a fix/point wasn't found, we'll try searching the top most shape file
         else if (C_shp_ovl::search(searchRequest, &geo, shpFilename) == SUCCESS)
         {
            center = geo;
         }
         // finally, try parsing the request as a description
         else if (pFvwUtil->db_lookup(CString(searchRequest), snapToInfo, FALSE)
            == TRUE)
         {
            center.lat = snapToInfo.m_lat;
            center.lon = snapToInfo.m_lon;
         }
         // otherwise, no match found
         else
         {
            AfxMessageBox("No match found");
            return;
         }
      }
   }

   // if a match was successfully found, then we'll update the center of the
   // map
   ChangeCenter(center);
}

// IFvMapView implementation
//

// get the HWND associated with the view
HWND MapView::m_hWnd()
{
   return GetSafeHwnd();
}

// get the frame window associated with the view
IFvFrameWnd* MapView::m_pFvFrameWnd()
{
   CMainFrame *pFrame = fvw_get_frame();
   return dynamic_cast<IFvFrameWnd *>(pFrame);
}

// get the current map projection associated with the view
ISettableMapProjPtr MapView::m_pCrntMapProj()
{
   ISettableMapProj *pCrntMapProj = m_view_map_proj->GetSettableMapProj();
   return ISettableMapProjPtr(pCrntMapProj, true);
}

// set the cursor for the map view
HRESULT MapView::SetCursor(HCURSOR hCursor)
{
   SetViewCursor(hCursor);
   return S_OK;
}

void MapView::ScrollMapIfPointNearEdge(long x, long y)
{
   int scroll_amount_x = 0;
   int scroll_amount_y = 0;
   int pixels_from_edge = 25;
   int screen_width, screen_height;
   get_curr_map()->get_surface_size(&screen_width, &screen_height);

   boolean_t need_to_invalidate = FALSE;

   // scroll the screen in the appropriate direction depending
   // upon the edge

   // left edge
   if (x < pixels_from_edge)
   {
      scroll_amount_x = -__min(pixels_from_edge - x, pixels_from_edge);

      need_to_invalidate = TRUE;
   }
   // right edge
   if (screen_width - x < pixels_from_edge)
   {
      scroll_amount_x = __min(pixels_from_edge - (screen_width - x),
         pixels_from_edge);
      need_to_invalidate = TRUE;
   }
   // top edge
   if (y < pixels_from_edge)
   {
      scroll_amount_y = -__min(pixels_from_edge  - y, pixels_from_edge);
      need_to_invalidate = TRUE;
   }
   // bottom edge
   if (screen_height - y < pixels_from_edge)
   {
      scroll_amount_y = __min(pixels_from_edge - (screen_height - y),
         pixels_from_edge);
      need_to_invalidate = TRUE;
   }

   if (need_to_invalidate)
   {
      int new_x = (screen_width / 2) + scroll_amount_x;
      int new_y = (screen_height / 2) + scroll_amount_y;
      if (get_curr_map()->point_in_surface(new_x, new_y))
      {
         get_map_engine()->change_center(new_x, new_y);
         invalidate_view(FALSE);
      }
   }
}

// set the tooltip for the map view.  The tooltip will be displayed at the
// current mouse position in 1/8 of second.  If the given text matches the text
// of a tooltip that is already being displayed and the cursor is within a
// certain threshold then the tooltip will not be cleared.  Also, if the mouse
// position is outside the view then no tooltip will be displayed.
HRESULT CBaseView::SetTooltipText(BSTR tooltipText)
{
   CWnd *wnd = AfxGetMainWnd();
   if (wnd->IsTopParentActive() == FALSE)
      return S_OK;

   CTipDisplay tip;
   CRect client;

   CString strTooltipText = (char *)_bstr_t(tooltipText);

   if (strTooltipText.GetLength() == 0)
      return S_OK;

   m_bTooltipWasSet = TRUE;
   TipTimer::set_text(strTooltipText);

   CPoint point;
   GetCursorPos(&point);
   ScreenToClient(&point);

   // if cursor is outside of view, then remove tool tip
   GetClientRect(client);
   client.InflateRect(-1, -1);
   if (!client.PtInRect(point))
   {
      // the tool-tip will not get erased if the m_tooltip_over_moving_object
      // flag is set.  Since the cursor is outside the view, we want to
      // force the tool-tip to get erased
      m_tooltip_over_moving_object = FALSE;

      clear_tool_tip();

      return S_OK;
   }

   // don't move tip if the hint is unchanged for an active tip
   if (tip.is_active() && strTooltipText == TipTimer::get_text())
   {
      int delta_x;
      int delta_y;

      // compute the distance in the x direction the cursor has moved since the
      // tool tip was put up
      if (m_tool_tip_point.x < point.x)
         delta_x = point.x - m_tool_tip_point.x;
      else
         delta_x = m_tool_tip_point.x - point.x;
      // compute the distance in the y direction the cursor has moved since the
      // tool tip was put up
      if (m_tool_tip_point.y < point.y)
         delta_y = point.y - m_tool_tip_point.y;
      else
         delta_y = m_tool_tip_point.y - point.y;

      // if the cursor movement is below the threshold, then leave the tip up
      if (delta_x < 20 && delta_y < 20)
      {
         return S_OK;
      }
   }

   // change in hint, remove old tip if present
   if (TipTimer::get_text().GetLength() > 0)
   {
      // the tool-tip will not get erased if the m_tooltip_over_moving_object
      // flag is set.  Since the tool tip is changing, we want to erase the
      // tool-tip even if it was over a moving object
      m_tooltip_over_moving_object = FALSE;

      tip.clear();
   }

   // set timer to bring up the tool tip in 1/8 of a second
   if (!m_pTipTimer)
   {
      // if one doesn't already exist, create it, add it to the timer list
      m_pTipTimer = new TipTimer(this, 125);
      UTL_get_timer_list()->add(m_pTipTimer);
   }

   m_pTipTimer->set_period(125);
   m_pTipTimer->reset();
   m_pTipTimer->start();

   return S_OK;
}

// CFvContextMenuImpl implementation
//

CFvContextMenuImpl::~CFvContextMenuImpl()
{
   ClearMenuItems();
}

STDMETHODIMP CFvContextMenuImpl::raw_AppendMenuItem(
   FalconViewOverlayLib::IFvContextMenuItem *pContextMenuItem)
{
   if (pContextMenuItem != NULL)
      m_menuItems.push_back(pContextMenuItem);

   return S_OK;
}

STDMETHODIMP CFvContextMenuImpl::raw_AppendSeparator()
{
   CComObject<CFVMenuNodeContextMenuItem> *pMenuItem = NULL;
   CComObject<CFVMenuNodeContextMenuItem>::CreateInstance(&pMenuItem);
   pMenuItem->Initialize(new CFVMenuNode("SEPARATOR"));

   m_menuItems.push_back(pMenuItem);

   return S_OK;
}

void CFvContextMenuImpl::ResetEnumerator()
{
   m_pCrntMenuItem = NULL;
   m_currentIt = m_menuItems.begin();
}

bool CFvContextMenuImpl::MoveNext()
{
   if (m_currentIt == m_menuItems.end())
   {
      m_pCrntMenuItem = NULL;
      return false;
   }

   m_pCrntMenuItem = *m_currentIt++;
   return true;
}

void CFvContextMenuImpl::ClearMenuItems()
{
   m_menuItems.erase(m_menuItems.begin(), m_menuItems.end());
}

MapRenderingEngine_Interface* MapView::GetMapRenderingEngineFromProjection(
   ProjectionEnum projection_type) const
{
   if (projection_type == GENERAL_PERSPECTIVE_PROJECTION)
   {
      return m_map_rendering_engines[MAP_RENDERING_ENGINE_OSG_INDEX];
   }

   return m_map_rendering_engines[MAP_RENDERING_ENGINE_COM_INDEX];
}

void MapView::AddMapParamsListener(MapParamsListener_Interface* listener)
{
   m_map_params.AddListener(listener);
}

void MapView::RemoveMapParamsListener(MapParamsListener_Interface* listener)
{
   m_map_params.RemoveListener(listener);
}


IStream* MapView::PrepareMarshaledStream()
{
   IStream* marshaled_stream;
   if (::CoMarshalInterThreadInterfaceInStream(IID_IMapRenderingEngine,
      m_map_engine->GetRenderingEngine(), &marshaled_stream) != S_OK)
   {
      marshaled_stream = nullptr;
      ERR_report("PrepareMarshaledStream failed");
   }

   return marshaled_stream;
}

void MapView::AttachToCamera(
   FalconViewOverlayLib::ICameraEvents* camera_events)
{
   try
   {
      if (m_attached_entity.camera_events != nullptr &&
         camera_events != m_attached_entity.camera_events)
      {
         m_attached_entity.camera_events->OnAttachedCameraChanging();
      }
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }

   // Don't let another entity prevent us from attaching (assign outside the
   // try-catch)
   m_attached_entity.camera_events = camera_events;
}

void MapView::SetCameraPositionWhenAttached(
   const AttachedCameraParams& camera_params)
{
   m_attached_entity.camera_params = camera_params;
   if (m_current_map_rendering_engine)
      m_current_map_rendering_engine->Invalidate();
}

// Populates the camera_params with the current camera position and returns
// true if the camera is attached to a moving object. Otherwise, returns
// false.
MapView::AttachedCameraParams* MapView::GetCameraPositionIfAttached()
{
   if (m_attached_entity.camera_events == nullptr)
      return nullptr;

   return &m_attached_entity.camera_params;
}

void MapView::OnSetFocus(CWnd* wndx)
{
   m_has_focus = true;
}

void MapView::OnKillFocus(CWnd* wndx)
{
   m_has_focus = false;
}

// Updates the brightness or contrast parameters based on whether the
// current MapType is black and white or not. Usually called after the
// MapType has changed
void MapView::UpdateContrast()
{
   bool is_BW = (GetMapType().get_source() == CIB ||
      GetMapType().get_series() == BLACK_AND_WHITE_SERIES);

   const MapDisplayParams& display_params = m_map_params.GetMapDisplayParams();
   if (is_BW)
   {
      m_current_map_rendering_engine->ChangeContrast(
         display_params.bw_brightness, display_params.bw_contrast,
         display_params.bw_contrast_midval);
   }
   else
   {
      m_current_map_rendering_engine->ChangeContrast(0.0, 0.0, 35);
      m_current_map_rendering_engine->ChangeBrightness(
         display_params.brightness);
   }
}
