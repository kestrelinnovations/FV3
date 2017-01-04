// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

#include "stdafx.h"
#include "mainfrm.h"
#include "FvFrameWndImpl.h"
#include "wm_user.h"
#include "gps.h"
#include "MovingMapOverlay\cdi.h"
#include "MovingMapOverlay\MovingMapFeed.h"
#include "MovingMapOverlay\gpstools.h"
#include "optsheet.h"
#include "optpri.h"
#include "optsec.h"
#include "optdis.h"
#include "message.h"
#include "getobjpr.h"
#include "catalog\cov_ovl.h"
#include "refresh.h"
#include "mapx.h"  // for ViewMapProjImpl
#include "appinfo.h"
#include "overlay\ovlmgrdl.h"
#include "overlay\ovlstartup.h"
#include "securdlg.h"
#include "printdlg.h"
#include "mem.h"
#include "Splash.h"
#include "datachk.h"
#include "SystemHealthDialog.h"
#include "mov_sym.h"
#include "nitf.h"
#include "IconDlg.h"  // for CIconDlg
#include "ILayer.h"   // for Layer::get_num_clients()
#include "favorite.h"
#include "..\Common\map_server_client.h" // for CLIENT_TOOLBAR_x defs
#include "CustomTB.h"
#include "ChartCurrencyDialog.h"
#include "SupplementalDataHandlers.h"
#include "MovingMapOverlay\LoadMovingMapSettingsDialog.h"
#include "TabularEditor\FVTabCtrl.h"
#include "MapViewTabsCtrl.h"
#include "StatusBarInfoPane.h"
#include "StatusBarSetupDialog.h"
#include "StatBarBullseyePropPage.h"
#include "atltypes.h"  // for CSize
#include "showrmk.h"
#include "..\FvCore\Include\Registry.h"
#include "..\FvCore\Include\FvHelp.h"
#include "MapEngineCOM.h"

#include "..\FvCore\Include\GuidStringConverter.h"

#include "ar_edit\factory.h"
#include "pointex\factory.h"
#include "nitf\factory.h"
#include "PrintToolOverlay\factory.h"
#include "catalog\factory.h"
#include "MovingMapOverlay\factory.h"
#include "FctryLst.h"
#include "tamask\factory.h"
#include "SkyViewOverlay\factory.h"
#include "overlay\OverlayTypeCustomInitializerCOM.h"
#include "FvToolMenuItemCOM.h"
#include "RibbonImpl.h"
#include "CCallback.h"

#include "afxmsg_.h"  // for ON_COMMAND_EX

#include "MissionPackageDlg.h"
#include "MAPIWrapper.h"
#include "math.h"

#include "PhotoGeotaggingToolsDialog.h"
#include "CTIA_Ruler.h"

#include "VerticalViewProperties.h"
#include "VerticalViewPropertiesDialog.h"
#include "VerticalViewDisplay.h"
#include "ElementRenderingDlg.h"
#include "overlay/selovl.h"
#include "VVODCurrencyDlg.h"
#include "IUserSimulator.h"

#include "FvDockablePane.h"
#include "overlay\overlay_menu.h"

#include "TabularEditor\TargetGraphicsDlg.h"
#include "SlatToolLauncher.h"

#include "SystemHealthRibbonButton.h"
#include "CustomToolbarMgrEventSink.h"
#include "ClientToolbar.h"
#include "MapMenu.h"
#include "StatusBarManager.h"
#include "PlaybackDialog\viewtime.h"
#include "statbar.h"
#include "FileOverlayListComboBox.h"

#include "FullScreenDialog.h"

#include "FalconView/CameraModeOptionsDialog.h"
#include "FalconView/HudProperties.h"

#include "FalconView/localpnt/localpnt.h"
#include "OverlayStackChangedObserver_Interface.h"

#include "FalconView/GeospatialViewController.h"
#include "FalconView/UIThreadOperation.h"

#include "FalconView/ZoomPercentages.h"
#include "FvSystemHealthCOM.h"

//  elevation
#include "FalconView/GeospatialScene.h"
#include "osgEarthUtil/VerticalScale"

// Scene manager
#include "FalconView\scene_mgr\scene_mgr.h"

#include "GeospatialView.h"

#if 0
#define MYTRACE(x) TRACE(x)
#else
#define MYTRACE(x)
#endif

#define MAX_NUM_EXTERNAL_MAP_SOURCES 50

// static vars
CViewTime* CMainFrame::m_view_time_dlg = nullptr;
boolean_t CMainFrame::m_view_time_dlg_active = FALSE;
CMissionPackageDlg *CMainFrame::m_mission_binder_dlg = nullptr;

// System wide unique identifiers for the route server notification message,
// which is sent the CMainFrame class every time a message is put in the
// FalconView notification queue.
const UINT msg_update_titlebar = RegisterWindowMessage("FVW_Update_TitleBar");
const UINT msg_viewtime_changed = RegisterWindowMessage("FVW_ViewTime_Changed");
const UINT msg_async_fix = RegisterWindowMessage("FVW_AsyncFix");

const int MIN_USER_DEFINED_GROUP_IDENTITY = 20;

const UINT OVERLAY_SUBMENU_INDEX = 4;

const char* MENU_CLASS_NAME = "#32768";

// undocumented message that gets sent to a menu message proc when a menu item is selected
const UINT WM_MENU_ITEM_SELECTED = 0x1EF;

const int RIBBON_BUTTON_IMAGE_FLIGHT_MODE_UNLOCKED = 86;
const int RIBBON_BUTTON_IMAGE_FLIGHT_MODE_LOCKED = 88;

// Override the visual manager so that we can remove the caption upon user
// request
class FalconViewVisualManager : public CMFCVisualManagerOffice2007
{
   DECLARE_DYNCREATE(FalconViewVisualManager)
public:
   FalconViewVisualManager():CMFCVisualManagerOffice2007(){}
   virtual ~FalconViewVisualManager(){}

   virtual BOOL IsOwnerDrawCaption()
   {
      return m_caption_on &&
         CMFCVisualManagerOffice2007::IsOwnerDrawCaption();
   }

   bool m_caption_on;
};

IMPLEMENT_DYNCREATE(FalconViewVisualManager, CMFCVisualManagerOffice2007 )


void MomentaryRibbonButton::OnLButtonDown(CPoint point)
{
   // Capture the mouse
   CMainRibbonBar* pBar = dynamic_cast<CMainRibbonBar*>(GetParentWnd());
   if(pBar != nullptr) {
      pBar->SetCapturedButton(this);
   }

   // Post a custom message
   CWnd* pMainWnd = AfxGetMainWnd();
   if(pMainWnd)
      pMainWnd->PostMessage(WM_MOMENTARY_BUTTON_PRESSED, (WPARAM) m_nID, (LPARAM) 0);

   CMFCRibbonButton::OnLButtonDown(point);
}

void MomentaryRibbonButton::OnLButtonUp(CPoint point)
{
   // Post a custom message
   CWnd* pMainWnd = AfxGetMainWnd();
   if(pMainWnd)
      pMainWnd->PostMessage(WM_MOMENTARY_BUTTON_RELEASED, (WPARAM) m_nID, (LPARAM) 0);

   CMFCRibbonButton::OnLButtonUp(point);

   // Release the mouse
   CMainRibbonBar* pBar = dynamic_cast<CMainRibbonBar*>(GetParentWnd());
   if(pBar != nullptr) {
      pBar->ReleaseCapturedButton();
   }
}

class MapTiltSlider :
   public CMFCRibbonSlider,
   public MapParamsListener_Interface
{
   friend class CMainRibbonBar;
public:
   MapTiltSlider(UINT nID, int nWidth = 100) :
      CMFCRibbonSlider(nID, nWidth)
   {
      SetZoomIncrement(5);
      SetZoomButtons(TRUE);
      SetRange(-89, -14);
      SetPos(-89);
      SetToolTipText(_T("Map Tilt in General Perspective (3D)"));
   }

   virtual void MapParamsChanged(int change_mask, long map_group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override
   {
      if ((change_mask & MAP_PROJ_TILT_CHANGED) != MAP_PROJ_TILT_CHANGED)
         return;

      SetPos(static_cast<int>(map_proj_params.tilt - 0.5));
   }
protected:
   virtual void OnLButtonDown(CPoint point)
   {
      // Capture the mouse
      CMFCRibbonSlider::OnLButtonDown(point);
   }

   virtual void OnLButtonUp(CPoint point)
   {
      // release the mouse if still over slider
      CMFCRibbonSlider::OnLButtonUp(point);
   }
};

// Handler for passing captured mouse-up event to the appropriate button
afx_msg void CMainRibbonBar::OnLButtonUp(UINT nFlags, CPoint point)
{
   if(m_pCapturedButton)
      return m_pCapturedButton->OnLButtonUp(point);

   CMFCRibbonBar::OnLButtonUp(nFlags, point);
}

BEGIN_MESSAGE_MAP(CMainRibbonBar, CMFCRibbonBar)
   //{{AFX_MSG_MAP(CMainRibbonBar)
   ON_WM_LBUTTONUP()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


class CSliderSettleTimer : public FVW_Timer
{
private:
   CSliderSettleTimer();   // Inaccessible
   const SliderSettleTimerTypeEnum  m_eTimerType;

public:
   CSliderSettleTimer( SliderSettleTimerTypeEnum eTimerType ) : m_eTimerType( eTimerType ), FVW_Timer( 2000 ){}
   virtual ~CSliderSettleTimer();

protected:
   virtual VOID expired();
}; // End of CSliderSettleTimer class

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

   BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
      //{{AFX_MSG_MAP(CMainFrame)
      ON_WM_CREATE()
      ON_WM_CLOSE()
      ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
      ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
      ON_COMMAND(ID_VIEW_TITLEBAR, OnViewTitlebar)
      ON_UPDATE_COMMAND_UI(ID_VIEW_TITLEBAR, OnUpdateViewTitlebar)
      ON_COMMAND(ID_VIEW_MAP_TABS_BAR, OnViewMapTabsBar)
      ON_UPDATE_COMMAND_UI(ID_VIEW_MAP_TABS_BAR, OnUpdateViewMapTabsBar)
      ON_COMMAND_EX(ID_VIEW_SPLIT, ToggleVerticalView)
      ON_UPDATE_COMMAND_UI(ID_VIEW_SPLIT, OnUpdateToggleVerticalView)
      ON_COMMAND(ID_VIEW_FILE_OVERLAY_LIST, OnFileOverlayList)
      ON_UPDATE_COMMAND_UI(ID_VIEW_FILE_OVERLAY_LIST, OnUpdateFileOverlayList)
      ON_WM_PALETTECHANGED()
      ON_WM_QUERYNEWPALETTE()
      ON_COMMAND(ID_OPTIONS_FORMAT, OnOptionsFormat)
      ON_WM_GETMINMAXINFO()
      ON_WM_ACTIVATE()
      ON_COMMAND(ID_DATA_PATHS, OnDataPaths)
      ON_COMMAND(ID_TOOLS_DATACHECK, OnDataCheck)
      ON_COMMAND(ID_SYSTEM_HEALTH_RIBBON_BUTTON, OnSystemHealthRibbonButton)
      ON_WM_TIMER()
      ON_COMMAND(ID_OVERLAY_OPTIONS, OnOverlayOptions)
      ON_COMMAND(ID_HIDE_BACKGROUND_OVERLAYS, OnHideBackgroundOverlays)
      ON_UPDATE_COMMAND_UI(ID_HIDE_BACKGROUND_OVERLAYS, OnUpdateHideBackgroundOverlays)
      ON_COMMAND(ID_OVERLAY_SHOWALLOVERLAYS, OnOverlayShowalloverlays)
      ON_COMMAND(ID_OVERLAY_MANAGER, OnOverlayManager)
      ON_COMMAND(ID_OVERLAY_TOGGLE_VVOD, OnToggleVvod)
      ON_COMMAND(ID_OPTIONS_SECURITY, OnOptionsSecurity)
      ON_COMMAND(ID_OPTIONS_PRINTING, OnOptionsPrinting)
      ON_WM_QUERYENDSESSION()
      ON_WM_ENDSESSION()

      ON_COMMAND(ID_FLIP_CURRENCY_UPDATE, OnFLIPCurrencyUpdate)

      ON_COMMAND(ID_VVOD_CURRENCY, OnVVodCurrency)
      ON_COMMAND(ID_DATAADMINISTRATION_VVODIMPORTER, OnVVodImporter)

      // Tools menu
      ON_COMMAND(ID_SYSTEM_HEALTH, OnSystemHealth)
      ON_COMMAND(ID_CUSTOM_TOOLS_MANAGER, OnCustomToolsManager)

      ON_COMMAND(ID_OPTIONS_OVERLAY, OnOverlayOptions)
      ON_COMMAND(ID_OPTIONS_OVERLAY, OnOverlayOptions)
      ON_WM_ACTIVATEAPP()
      ON_WM_SETCURSOR()
      ON_COMMAND(ID_VIEWTIME, OnViewtime)
      ON_UPDATE_COMMAND_UI(ID_VIEWTIME, OnUpdateViewtime)
      ON_COMMAND(ID_PHOTO_GEOTAGGING_TOOL, OnPhotoGeotaggingTool)
      ON_COMMAND(ID_EDIT_TARGET_GRAPHIC_PRINTING, OnTargetGraphicPrinting)
      //}}AFX_MSG_MAP

      ON_MESSAGE(WM_DEVICECHANGE, OnDeviceChange)
      ON_MESSAGE(WM_INSERT_PRINT_MENU_ITEMS, OnInsertPrintMenuItems)
      ON_MESSAGE(WM_REMOVE_PRINT_MENU_ITEMS, OnRemovePrintMenuItems)
      ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
      ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
      ON_MESSAGE(WM_ERROR_REPORT, OnErrorReport)
      ON_MESSAGE(WM_CLOSE_INFORMATION_DIALOG, OnCloseInformationDialog)
      ON_COMMAND(ID_ENTER_CHUM_EDIT_MODE, OnEnterCHUMEditMode)
      ON_MESSAGE(WM_NITF_UTILITY_THREAD_EVENT, OnNITFUtilityThreadEvent)
      ON_WM_SIZE()

      /**************************************************************************
      *  NOTE: for some reason, the toolbar does not catch the following
      *  messages. To work around this problem, the mainframe is catching 
      *  the message and calling the appropriate toolbar message handler.
      ***************************************************************************/
      ON_COMMAND_RANGE(ID_CUSTOM_TB1,ID_CUSTOM_TB20, OnCustomTB)
      ON_UPDATE_COMMAND_UI_RANGE(ID_CUSTOM_TB1,ID_CUSTOM_TB20, OnUpdateCustomTB)
      ON_COMMAND_RANGE(ID_OVERLAY_EDITOR1,ID_OVERLAY_EDITOR40, OnOverlayEditor)
      ON_UPDATE_COMMAND_UI_RANGE(ID_OVERLAY_EDITOR1,ID_OVERLAY_EDITOR40, OnUpdateOverlayEditor)

      ON_CONTROL_RANGE(BN_CLICKED, ID_FIRST_RIBBON_ELEMENT, ID_LAST_RIBBON_ELEMENT, OnClickedRibbonElement)
      ON_UPDATE_COMMAND_UI_RANGE(ID_FIRST_RIBBON_ELEMENT, ID_LAST_RIBBON_ELEMENT, OnUpdateRibbonElement)
      ON_COMMAND_RANGE( ID_FIRST_RIBBON_ELEMENT, ID_LAST_RIBBON_ELEMENT, OnRibbonElement )

      ON_COMMAND_RANGE(IDRANGE_OVERLAY_MENU_ITEM_1, IDRANGE_OVERLAY_MENU_ITEM_LAST, OnOverlayMenuItem)
      ON_UPDATE_COMMAND_UI_RANGE(IDRANGE_OVERLAY_MENU_ITEM_1, IDRANGE_OVERLAY_MENU_ITEM_LAST, OnUpdateOverlayMenuItem)

      ON_COMMAND_RANGE(IDRANGE_FILE_OVERLAY_MENU_ITEM_1, IDRANGE_FILE_OVERLAY_MENU_ITEM_LAST, OnFileOverlayMenuItem)

      // projection menu handlers
      ON_COMMAND_RANGE(ID_MAP_PROJECTION_01, ID_MAP_PROJECTION_20, OnMapProjection)
      ON_UPDATE_COMMAND_UI_RANGE(ID_MAP_PROJECTION_01, ID_MAP_PROJECTION_20, OnUpdateMapProjection)
      ON_UPDATE_COMMAND_UI(IDC_MAP_PROJECTION, OnUpdateMapProjection)

      ON_COMMAND_RANGE(IDRANGE_MAP_MENU_001, IDRANGE_MAP_MENU_500, OnMapMenuEntry)
      ON_UPDATE_COMMAND_UI_RANGE(IDRANGE_MAP_MENU_001, IDRANGE_MAP_MENU_500, OnUpdateMapMenuEntry)

      ON_COMMAND_RANGE(IDRANGE_TOOL_MENU_ITEM_COM_00, IDRANGE_TOOL_MENU_ITEM_COM_24, OnToolMenuItemCOM)
      ON_UPDATE_COMMAND_UI_RANGE(IDRANGE_TOOL_MENU_ITEM_COM_00, IDRANGE_TOOL_MENU_ITEM_COM_24, OnUpdateToolMenuItemCOM)

      ON_COMMAND( ID_ELEVATION_EXAGGERATION_SLIDER, OnElevationExaggerationSlider )
      ON_UPDATE_COMMAND_UI(ID_ELEVATION_EXAGGERATION_SLIDER, OnUpdateElevationExaggeration)
      ON_COMMAND( ID_ELEVATION_EXAGGERATION_EDIT, OnElevationExaggerationEdit )
      ON_UPDATE_COMMAND_UI(ID_ELEVATION_EXAGGERATION_EDIT, OnUpdateElevationExaggeration)

      ON_COMMAND( ID_MAP_3D_MAP_TILT_SLIDER, On3DMapTiltSlider )
      ON_UPDATE_COMMAND_UI( ID_MAP_3D_MAP_TILT_SLIDER, OnUpdate3DMapTilt )

      // Map Zooming
      //
      ON_COMMAND(ID_ZOOM_COMBO, OnZoomCombo)
      ON_UPDATE_COMMAND_UI(ID_ZOOM_COMBO, OnUpdateZoom)

      ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
      ON_COMMAND(ID_ZOOM_IN_STEP, OnZoomInStep)

      ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
      ON_COMMAND(ID_ZOOM_OUT_STEP, OnZoomOutStep)

      // favorites menu handlers
      ON_COMMAND_RANGE(IDRANGE_FAVORITE_MENU_ITEM_1, IDRANGE_FAVORITE_MENU_ITEM_EMPTY, OnFavoriteList)
      ON_UPDATE_COMMAND_UI_RANGE(IDRANGE_FAVORITE_MENU_ITEM_1, IDRANGE_FAVORITE_MENU_ITEM_EMPTY, OnUpdateFavoriteList)
      ON_MESSAGE(WM_FAVORITES_DIRECTORY_CHANGED, OnFavoritesDirectoryChanged)

      ON_REGISTERED_MESSAGE(msg_update_titlebar, OnUpdateTitleBar)
      ON_REGISTERED_MESSAGE(msg_viewtime_changed, OnViewTimeChanged)

      ON_WM_COPYDATA()
      ON_REGISTERED_MESSAGE(msg_async_fix, OnMsgAsyncFix)

      // playback message maps
      ON_COMMAND(ID_PLAYBACK_START, OnPlaybackStart)
      ON_UPDATE_COMMAND_UI(ID_PLAYBACK_START, OnUpdatePlaybackStart)
      ON_COMMAND(ID_PLAYBACK_PLAY, OnPlaybackPlay)
      ON_UPDATE_COMMAND_UI(ID_PLAYBACK_PLAY, OnUpdatePlaybackPlay)
      ON_COMMAND(ID_PLAYBACK_STOP, OnPlaybackStop)
      ON_UPDATE_COMMAND_UI(ID_PLAYBACK_STOP, OnUpdatePlaybackStop)
      ON_COMMAND(ID_PLAYBACK_END, OnPlaybackEnd)
      ON_UPDATE_COMMAND_UI(ID_PLAYBACK_END, OnUpdatePlaybackEnd)

      // mission binder message maps
      ON_COMMAND(ID_OPEN_MISSION_PACKAGE, OnOpenMissionPackage)

      // mission binder message maps
      ON_COMMAND(ID_PUBLISH_COLLABORATION, OnPublishCollaboration)
      ON_COMMAND(ID_SUBSCRIBE_COLLABORATION, OnSubscribeCollaboration)
      ON_UPDATE_COMMAND_UI(ID_PUBLISH_COLLABORATION, OnUpdatePublishCollaboration)

      ON_COMMAND(ID_TOOLS_CALIBRATEPIXELSPERINCH, OnCalibratePixelsPerInch)

      // mail overlay(s) via e-mail
      ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
      ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)

      // overlay toolbar message maps
      ON_COMMAND(ID_OVERLAY_COMBO_BOX, OnOverlayComboBox)
      ON_UPDATE_COMMAND_UI(ID_OVERLAY_COMBO_BOX, OnUpdateOverlayComboBox)
      ON_COMMAND(ID_OVERLAY_PREV, OnOverlayPrevious)
      ON_COMMAND(ID_OVERLAY_NEXT, OnOverlayNext)
      ON_COMMAND(ID_OVERLAY_FILTER_TIME, OnOverlayToolbarFilterTime)
      ON_UPDATE_COMMAND_UI(ID_OVERLAY_FILTER_TIME, OnUpdateOverlayToolbarFilterTime)

      // status bar
      ON_COMMAND_RANGE(IDRANGE_STATUS_BAR_INFO_PANE_00, IDRANGE_STATUS_BAR_INFO_PANE_49, OnStatusBarPane)
      ON_UPDATE_COMMAND_UI_RANGE(IDRANGE_STATUS_BAR_INFO_PANE_00, IDRANGE_STATUS_BAR_INFO_PANE_49, OnUpdateStatusBarPane)

      ON_MESSAGE(WM_MOVING_MAP_POINT_ADDED, OnMovingMapPointAdded)
      ON_MESSAGE(MDM_COVERAGE_CHANGED, OnCoverageChanged)

      ON_COMMAND(ID_OPTIONS_STATUSBARSETUP, OnStatusBarSetup)

      ON_COMMAND(ID_GRAPHIC_DRAWING_PREFS, OnGraphicDrawingPrefs)

      ON_COMMAND(ID_OPTIONS_VERTICAL_DISPLAY_PROPERTIES, OnVerticalDisplayProperties)

      ON_COMMAND(ID_VIEW_NEWTAB, &CMainFrame::OnViewNewTab)

      ON_REGISTERED_MESSAGE(fvw::UIThreadOperation::UI_THREAD_OPERATION, OnUIThreadOperation)

      // Camera modes
      //
      ON_COMMAND(ID_CAMERA_MODE_PAN, OnCameraModePan)
      ON_UPDATE_COMMAND_UI(ID_CAMERA_MODE_PAN, OnUpdateCameraModePan)
      ON_COMMAND(ID_CAMERA_MODE_FLIGHT, OnCameraModeFlight)
      ON_UPDATE_COMMAND_UI(ID_CAMERA_MODE_FLIGHT, OnUpdateCameraModeFlight)
      ON_COMMAND(ID_CAMERA_MODE_GROUND_VIEW, OnCameraModeGroundView)
      ON_UPDATE_COMMAND_UI(ID_CAMERA_MODE_GROUND_VIEW, OnUpdateCameraModeGroundView)

      ON_COMMAND(ID_UNDERSEA_3D_ELEVATIONS, OnUnderseaElevations)
      ON_UPDATE_COMMAND_UI(ID_UNDERSEA_3D_ELEVATIONS, OnUpdateUnderseaElevations)

      ON_COMMAND(ID_TOGGLE_2D_PROJECTION, OnToggle2DProjection)
      ON_UPDATE_COMMAND_UI(ID_TOGGLE_2D_PROJECTION, OnUpdateToggle2DProjection)
      ON_COMMAND(ID_TOGGLE_3D_PROJECTION, OnToggle3DProjection)
      ON_UPDATE_COMMAND_UI(ID_TOGGLE_3D_PROJECTION, OnUpdateToggle3DProjection)

      ON_COMMAND(ID_CAMERA_MODE_OPTIONS, OnCameraModeOptions)

      // 3D handlers for panning and rotating buttons
      ON_MESSAGE(WM_MOMENTARY_BUTTON_PRESSED, OnMomentaryButtonPressed)
      ON_MESSAGE(WM_MOMENTARY_BUTTON_RELEASED, OnMomentaryButtonReleased)

   END_MESSAGE_MAP()


   /////////////////////////////////////////////////////////////////////////////
   // CMainFrame construction/destruction

   CMainFrame::CMainFrame() :
      m_coverage_panel(TRUE),      // status bar displays coverage
         m_fullscreen(FALSE),         // no full screen
         m_doing_size_or_move(FALSE),  // not doing a resize or move operation
         m_dafif_importer_active(FALSE),
         m_precise_geo(TRUE),
         m_colored_coord(FALSE),
         m_next_client_toolbar_handle(1000),
         m_custom_toolbar(nullptr),
         m_pMapTabsBar(nullptr),
         m_pMapViewTabsBar(nullptr),
         m_pStatusBarSetupDialog(nullptr),
         m_nVerticalDisplayHeight(DEFAULT_VERTICAL_DISPLAY_HEIGHT),
         m_pGraphicDrawingPrefsDialog(nullptr),
         m_nDefaultVerticalDisplayHeight(DEFAULT_VERTICAL_DISPLAY_HEIGHT),
         m_pVerticalDisplayPropertiesDialog(nullptr),
         m_bIsMapMenuValid(false),
         m_pFvFrameWnd(nullptr),
         m_pRibbonCOM(nullptr),
         m_overlay_menu(nullptr),
         m_pEditorsPanel(nullptr),
         m_pAdditionalToolsPanel(nullptr),
         m_pOverlaysPanel(nullptr),
         m_pMapDataManagerPanel(nullptr),
         m_favorites_menu(nullptr),
         m_file_overlay_ribbon_btns_group(nullptr),
         m_pCollaborationServer(nullptr),
         m_pSystemHealth(nullptr),
         m_file_overlay_list_cb(nullptr),
         m_pElevationExaggerationSliderSettleTimer(nullptr),
         m_minimizedPanesPanel(nullptr),
         m_flight_mode_button(nullptr), 
         m_pOverlaysCategory(nullptr),
         m_pEditorsCategory(nullptr),
         m_p3DMapTiltSlider(nullptr)
      {
         m_MAPI = new MAPIWrapper;
         m_view_time_dlg = new CViewTime();

         // pointer to the Vertical Display Properties object...
         m_pVerticalDisplayProperties = new CVerticalViewProperties();

         m_statusBarMgr = new CFvStatusBarManager();
         m_statusBarMgr->m_availableInfoPaneList = nullptr;

         m_map_menu = new MapMenu();
      }

      FalconViewOverlayLib::IFvFrameWnd2 *CMainFrame::GetFvFrameWnd()
      {
         if (m_pFvFrameWnd == NULL)
         {
            CComObject<CFvFrameWndImpl>::CreateInstance(&m_pFvFrameWnd);
            m_pFvFrameWnd->AddRef();

            m_pFvFrameWnd->SetFrameWnd(this);
         }

         return m_pFvFrameWnd;
      }

      FalconViewOverlayLib::IRibbon* CMainFrame::GetRibbon()
      {
         if (m_pRibbonCOM == NULL)
         {
            CComObject<CRibbonImpl>::CreateInstance(&m_pRibbonCOM);
            m_pRibbonCOM->AddRef();

            m_pRibbonCOM->SetRibbonBar(&m_wndRibbonBar);
         }

         return m_pRibbonCOM;
      }

      CMainFrame::~CMainFrame()
      {
         for(int i=0;i<m_statusBarMgr->m_arrStatusBars.GetSize();++i)
            delete m_statusBarMgr->m_arrStatusBars[i];

         for (size_t i=0; i<m_map_menu->m_mapMenuPopups.size(); i++)
            delete m_map_menu->m_mapMenuPopups[i];

         delete m_MAPI;
         delete m_custom_toolbar;
         if (m_pMapTabsBar)
         {
            m_pMapTabsBar->DestroyWindow();
            m_pMapTabsBar = nullptr;
         }
         delete m_pMapViewTabsBar;
         delete m_statusBarMgr->m_availableInfoPaneList;
         delete m_statusBarMgr;
         delete m_pVerticalDisplayPropertiesDialog;
         delete m_pVerticalDisplayProperties;

         if (m_pFvFrameWnd != NULL)
            m_pFvFrameWnd->Release();

         if (m_pCollaborationServer)
            m_pCollaborationServer->Release();

         std::map<UINT, CFvToolMenuItemCOM *>::iterator it = m_toolMenuItems.begin();
         for(; it != m_toolMenuItems.end(); it++)
            delete it->second;

         delete m_overlay_menu;
         delete m_map_menu;
         delete m_view_time_dlg;

         delete m_pSystemHealth;
      }

      BOOL CMainFrame::OnCloseMiniFrame(CPaneFrameWnd* pWnd)
      {
         CWnd* pPane = pWnd->GetPane();
         if (pPane)
            pPane->PostMessage(WM_CLOSE);

         return TRUE;
      }

      //
      // Create main ribbon bar
      //

      bool CMainFrame::SortGUIDs( const GUID& a, const GUID& b ) {
         COverlayTypeDescriptorList* potdl = OVL_get_type_descriptor_list();
         OverlayTypeDescriptor* item1 = potdl->GetOverlayTypeDescriptor( a );
         OverlayTypeDescriptor* item2 = potdl->GetOverlayTypeDescriptor( b );
         if (item1->parentDisplayName < item2->parentDisplayName) 
         {
            return false;
         }
         else if (item1->parentDisplayName > item2->parentDisplayName) 
         {
            return true;
         }
         else if (item1->parentDisplayName == item2->parentDisplayName) {
            return potdl->GetOverlayTypeDescriptor( a )->displayName <
               potdl->GetOverlayTypeDescriptor( b )->displayName;
         }
         return potdl->GetOverlayTypeDescriptor( a )->displayName < //this code should never be reached
            potdl->GetOverlayTypeDescriptor( b )->displayName;
      }

      int CMainFrame::create_ribbon_bar()
      {
         if (!m_wndRibbonBar.Create(this))
         {
            ERR_report("Ribbon bar creation failed");
            return FAILURE;
         }

         // we will explicitly enable this once the rest of our categories are
         // created
         m_wndRibbonBar.EnablePrintPreview(FALSE);

         // Build application menu
         build_application_menu();

         m_toolbarImages.SetImageSize(CSize(16, 16));
         m_toolbarImages.Load(IDB_RIBBON_TOOLBAR);

         SetupRibbonBarTabsRow(PRM_get_registry_int("View", "FileOverlayListOn", 0));

         build_map_category();
         build_display_category();
         build_overlay_category();
         build_edit_category();
         build_view_category();
         build_tools_category();
         build_data_admin_category();

         // Setup quick access toolbar commands
         CMFCRibbonQuickAccessToolBarDefaultState qatState;
         static const UINT commands[] =
         { ID_EDIT_UNDO, ID_EDIT_REDO, ID_FILE_SAVE_FV, ID_FILE_SAVE_AS_FV, ID_FILE_NEW,
         ID_FILE_OPEN, ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, 0 };

         int crnt = 0;
         while (commands[crnt] != 0)
         {
            qatState.AddCommand(commands[crnt++]);
         }
         m_wndRibbonBar.SetQuickAccessDefaultState(qatState);

         // Now that our specific categories are set up,  the print preview
         m_wndRibbonBar.EnablePrintPreview();
         m_wndRibbonBar.EnableToolTips();
         m_wndRibbonBar.EnableKeyTips();

         return SUCCESS;
      }

// RibbonButtonDisallowQAT - an CMFCRibbonButton that
// cannot be added to the quick access toolbar
class RibbonButtonDisallowQAT : public CMFCRibbonButton
{
public:
   RibbonButtonDisallowQAT(UINT nID, LPCTSTR lpszText, HICON hIcon) :
      CMFCRibbonButton(nID, lpszText, hIcon)
   {
   }

      // Do not allow the user to add this button to the QAT
   virtual BOOL CanBeAddedToQuickAccessToolBar() const override
   {
      return FALSE;
   }
};

int CMainFrame::SetupRibbonBarTabsRow(BOOL show_file_overlay_controls)
{
   // Because there is no easy way to hide the file overlay controls we need
   // to recreate the elements on the tabs row. Because the ribbon elements
   // can be destroyed at runtime, we use a RibbonButtonsGroupDisallowQAT
   // rather than CMFCRibbonButtonsGroup for each element added to the tabs
   // row
   m_wndRibbonBar.RemoveAllFromTabs();
   m_file_overlay_list_cb = nullptr;

   // Add the file overlay controls to the tabs bar
   if (show_file_overlay_controls)
   {
      // Create a new group for the file overlay controls
      CMFCRibbonButtonsGroup* file_overlay_group =
         new CMFCRibbonButtonsGroup();

      // Create the file overlay list combo box control
      m_file_overlay_list_cb = new FileOverlayListComboBox();

      // Add the controls to the group
      file_overlay_group->AddButton(m_file_overlay_list_cb);
      file_overlay_group->AddButton(new RibbonButtonDisallowQAT(ID_OVERLAY_PREV,
         "Previous Overlay", m_toolbarImages.ExtractIcon(7)));
      file_overlay_group->AddButton(new RibbonButtonDisallowQAT(ID_OVERLAY_NEXT,
         "Next Overlay", m_toolbarImages.ExtractIcon(8)));
      file_overlay_group->AddButton(new RibbonButtonDisallowQAT(
         ID_OVERLAY_FILTER_TIME, "Only List Time Sensitive Overlays",
         m_toolbarImages.ExtractIcon(9)));

      // Add the group and a separator to the tabs row
      m_wndRibbonBar.AddToTabs(file_overlay_group);
      m_wndRibbonBar.AddToTabs(new CMFCRibbonSeparator());
   }

   // See if 3D support is enabled in the registry
   if (Is3DEnabledInRegistry())
   {
      CMFCRibbonButtonsGroup* proj_2d_3d_group = new CMFCRibbonButtonsGroup();
      proj_2d_3d_group->AddButton(new CMFCRibbonButton(ID_TOGGLE_2D_PROJECTION,
         "2D", m_toolbarImages.ExtractIcon(11)));
      proj_2d_3d_group->AddButton(new CMFCRibbonButton(ID_TOGGLE_3D_PROJECTION,
         "3D", m_toolbarImages.ExtractIcon(10)));
      m_wndRibbonBar.AddToTabs(proj_2d_3d_group);
   }

   // Add the search controls to the tabs bar
   CMFCRibbonButtonsGroup* search_group = new CMFCRibbonButtonsGroup();
   search_group->AddButton(new CMFCRibbonEdit(IDC_SEARCH_BAR, 100, "Search"));
   search_group->AddButton(new RibbonButtonDisallowQAT(IDC_SEARCH_BAR, "Search",
      m_toolbarImages.ExtractIcon(1)));
   m_wndRibbonBar.AddToTabs(search_group);

   // Add the help topics button to the tabs bar
   CMFCRibbonButtonsGroup* help_topics_group =
      new CMFCRibbonButtonsGroup();
   help_topics_group->AddButton(new RibbonButtonDisallowQAT(ID_HELP_FINDER,
      "Help Topics...", m_toolbarImages.ExtractIcon(0)));
   m_wndRibbonBar.AddToTabs(help_topics_group);

   m_wndRibbonBar.RecalcLayout();

   return SUCCESS;
}

      void CMainFrame::OnOverlayComboBox()
      {
         if (m_file_overlay_list_cb)
         {
            C_overlay* overlay = reinterpret_cast<C_overlay *>(
               m_file_overlay_list_cb->GetItemData(
               m_file_overlay_list_cb->GetCurSel()));
            if (overlay)
            {
               OVL_get_overlay_manager()->make_current(overlay);
            }
         }
      }

      void CMainFrame::OnUpdateOverlayComboBox(CCmdUI* pCmdUI)
      {
         pCmdUI->Enable(m_file_overlay_list_cb &&
            m_file_overlay_list_cb->GetCount());
      }

      void CMainFrame::OnOverlayPrevious()
      {
         if (m_file_overlay_list_cb)
         {
            const int count = m_file_overlay_list_cb->GetCount();
            if (count >= 2)
            {
               C_overlay* overlay = reinterpret_cast<C_overlay *>(
                  m_file_overlay_list_cb->GetItemData(count - 1));

               OVL_get_overlay_manager()->make_current(overlay);
            }
         }
      }

      void CMainFrame::OnOverlayNext()
      {
         if (m_file_overlay_list_cb)
         {
            if (m_file_overlay_list_cb->GetCount() >= 2)
            {
               C_overlay* first_ovl = reinterpret_cast<C_overlay *>(
                  m_file_overlay_list_cb->GetItemData(0));

               C_overlay* second_ovl = reinterpret_cast<C_overlay *>(
                  m_file_overlay_list_cb->GetItemData(1));

               OVL_get_overlay_manager()->make_current(second_ovl);
               OVL_get_overlay_manager()->move_to_bottom(first_ovl);
            }
         }
      }

      void CMainFrame::OnOverlayToolbarFilterTime()
      {
         if (m_file_overlay_list_cb)
         {
            bool time_sensitive_only =
               m_file_overlay_list_cb->GetShowTimeSensitiveOnly();

            m_file_overlay_list_cb->SetShowTimeSensitiveOnly(!time_sensitive_only);
         }
      }

      void CMainFrame::OnUpdateOverlayToolbarFilterTime(CCmdUI *pCmdUI)
      {
         if (m_file_overlay_list_cb)
         {
            pCmdUI->SetCheck(m_file_overlay_list_cb->GetShowTimeSensitiveOnly());
         }
      }

      class FileOverlayRibbonButtonsGroup : public CMFCRibbonButtonsGroup,   //used to handle events for overlay buttons
         public OverlayStackChangedObserver_Interface
      {
         DECLARE_DYNCREATE(FileOverlayRibbonButtonsGroup)

      public:
         FileOverlayRibbonButtonsGroup()
         {
            OVL_get_overlay_manager()->RegisterEvents(this);
            RepopulateList();
         }

         ~FileOverlayRibbonButtonsGroup()
         {
            OVL_get_overlay_manager()->UnregisterEvents(this);
         }

         // OverlayStackChangedObserver_Interface
         virtual void OverlayAdded(C_overlay* overlay) override
         {
            RepopulateList();
         }
         virtual void OverlayRemoved(C_overlay* overlay) override
         {
            RepopulateList();
         }
         virtual void OverlayOrderChanged() override
         {
            RepopulateList();
         }
         virtual void OverlayFileSpecificationChanged(C_overlay* overlay) override
         {
         }
         virtual void OverlayDirtyChanged(C_overlay* overlay) override
         {
         }

      protected:
         virtual void OnAfterChangeRect(CDC* pDC);
         virtual CSize GetRegularSize(CDC* pDC);
         virtual void OnDraw(CDC* pDC);
         virtual BOOL OnMenuKey(UINT nUpperChar);

         void AddPinnedFileOverlay(const CString& display_title,
            const GUID& desc_guid, const CString& file_string, int& next_id);

      private:
         void RepopulateList();
         void AddFileOverlays(int is_pinned);
      };

      class FileOverlayMenuButton : public CMFCRibbonButton
      {
      public:
         void SetOverlay(C_overlay* p, const GUID& g)
         {
            m_overlay = p;
            m_overlay_desc = g;
         }
         C_overlay* GetOverlay() const { return m_overlay; }
         const GUID& GetOverlayDesc() const { return m_overlay_desc; }

      private:
         C_overlay* m_overlay;
         GUID m_overlay_desc;
      };

#define AFX_SEPARATOR_HEIGHT 4
#define AFX_FILE_MARGIN 4
#define AFX_LABEL_MARGIN 4

      IMPLEMENT_DYNCREATE(FileOverlayRibbonButtonsGroup, CMFCRibbonButtonsGroup)

         void FileOverlayRibbonButtonsGroup::OnAfterChangeRect(CDC* pDC)
      {
         ASSERT_VALID(this);
         ASSERT_VALID(pDC);

         int y = m_rect.top + 2;

         for (int i = 0; i < m_arButtons.GetSize(); i++)
         {
            CMFCRibbonBaseElement* pButton = m_arButtons [i];
            ASSERT_VALID(pButton);

            pButton->SetParentMenu(m_pParentMenu);

            pButton->OnCalcTextSize(pDC);
            CSize sizeButton = pButton->GetSize(pDC);

            CRect rectButton = m_rect;
            rectButton.DeflateRect(1, 0);

            rectButton.top = y;
            rectButton.bottom = y + sizeButton.cy + 2 * AFX_FILE_MARGIN;

            pButton->SetRect(rectButton);
            pButton->OnAfterChangeRect(pDC);

            y = rectButton.bottom;
         }
      }

      CSize FileOverlayRibbonButtonsGroup::GetRegularSize(CDC* pDC)
      {
         ASSERT_VALID(this);
         ASSERT_VALID(pDC);

         int cy = 4;

         for (int i = 0; i < m_arButtons.GetSize(); i++)
         {
            CMFCRibbonBaseElement* pButton = m_arButtons [i];
            ASSERT_VALID(pButton);

            pButton->OnCalcTextSize(pDC);
            CSize sizeButton = pButton->GetSize(pDC);

            cy += sizeButton.cy + 2 * AFX_FILE_MARGIN;
         }

         const int nDefaultSize = 300;

         return CSize(afxGlobalData.GetRibbonImageScale() == 1. ? nDefaultSize :(int)(afxGlobalData.GetRibbonImageScale() *  nDefaultSize), cy);
      }

      void FileOverlayRibbonButtonsGroup::RepopulateList()
      {
         ASSERT_VALID(this);

         RemoveAll();

         int next_id = IDRANGE_FILE_OVERLAY_MENU_ITEM_1;

         AddButton(new CMFCRibbonLabel("Recent Overlays"));
         next_id++;

         CRecentFileList* pMRUFiles = ((CFVApp*)AfxGetApp())->
            GetRecentFileOverlayList();

         TCHAR szCurDir [_MAX_PATH];
         ::GetCurrentDirectory(_MAX_PATH, szCurDir);

         int nCurDir = lstrlen(szCurDir);
         ASSERT(nCurDir >= 0);

         szCurDir [nCurDir] = _T('\\');
         szCurDir [++ nCurDir] = _T('\0');

         int iNumOfFiles = 0; // Actual added to menu

         if (pMRUFiles != NULL)
         {
            for (int i = 0; i < pMRUFiles->GetSize(); i++)
            {
               CString strName;

               if (pMRUFiles->GetDisplayName(strName, i, szCurDir, nCurDir))
               {
                  // Add shortcut number:
                  CString strItem;

                  if (iNumOfFiles == 9)
                  {
                     strItem.Format(_T("1&0 %s"), (LPCTSTR)strName);
                  }
                  else if (iNumOfFiles < 9)
                  {
                     strItem.Format(_T("&%d %s"), iNumOfFiles + 1, (LPCTSTR)strName);
                  }
                  else
                  {
                     strItem = strName;
                  }

                  CMFCRibbonButton* pFile = new CMFCRibbonButton;
                  pFile->SetText(strItem);
                  pFile->SetID(ID_FILE_MRU_FILE1 + i);
                  pFile->SetToolTipText((*pMRUFiles)[i]);

                  AddButton(pFile);

                  iNumOfFiles++;
                  next_id++;
               }
            }
         }

         AddButton(new CMFCRibbonLabel("Pinned File Overlays"));
         next_id++;

         // Add the default local point file to the pinned overlay list.
         // Note, we may need to read this from the XPlan options server instead.
         CString points_display_title = "local.lps";
         CString points_file_spec = PRM_get_registry_string("Main",
            "ReadWriteUserData", "") + "\\Points\\local.lps";
         AddPinnedFileOverlay(points_display_title, FVWID_Overlay_Points,
            points_file_spec, next_id);

         // get the number of files that we're going to look for
         int file_count = PRM_get_registry_int("Overlay Manager\\Menu Files2", "File Count", 0);

         // loop through all files in the Menu Files section of the registry,
         for (int i = 0; i < file_count; i++)
         {
            char index_str[10];
            const errno_t e = _itoa_s(i, index_str, 10);
            CString value = PRM_get_registry_string("Overlay Manager\\Menu Files2",
               index_str, "NONE");

            int cur_pos = 0;
            CString display_title = value.Tokenize(",", cur_pos);
            CString desc_guid = value.Tokenize(",", cur_pos);
            CString file_string = value.Tokenize(",", cur_pos);
            std::string s(desc_guid);
            string_utils::CGuidStringConverter guidConv(s);

            AddPinnedFileOverlay(display_title, guidConv.GetGuid(), file_string,
               next_id);
         }

         AddFileOverlays(next_id);
      }

      void FileOverlayRibbonButtonsGroup::AddPinnedFileOverlay(
         const CString& display_title, const GUID& desc_guid,
         const CString& file_string, int& next_id)
      {
         FileOverlayMenuButton* pFile = new FileOverlayMenuButton;
         pFile->SetText(display_title);

         C_overlay* overlay = OVL_get_overlay_manager()->is_file_open(
            file_string, desc_guid);

         pFile->SetOverlay(overlay, desc_guid);

         // Add a checkmark next to the overlay name if the file is already
         // opened
         if (overlay)
            pFile->SetImageIndex(75, FALSE);

         pFile->SetID(next_id);
         pFile->SetToolTipText(file_string);

         AddButton(pFile);
         next_id++;
      }

      void FileOverlayRibbonButtonsGroup::AddFileOverlays(int next_id)
      {
         AddButton(new CMFCRibbonLabel("Opened File Overlays"));
         next_id++;

         C_ovl_mgr* ovlmgr = OVL_get_overlay_manager();
         COverlayTypeDescriptorList* otdl = OVL_get_type_descriptor_list();
         C_overlay* p = ovlmgr->get_first();
         while (p)
         {
            if (otdl->GetOverlayTypeDescriptor(p->get_m_overlayDescGuid())->
               fileTypeDescriptor.bIsFileOverlay)
            {
               OverlayPersistence_Interface* persistence = 
                  dynamic_cast<OverlayPersistence_Interface *>(p);

               if (persistence)
               {
                  _bstr_t spec;
                  persistence->get_m_fileSpecification(spec.GetAddress());
                  char* file_spec = (char *)spec;

                  // Pinned overlays are added separately
                  if (ovlmgr->test_file_menu(file_spec) == FALSE)
                  {
                     FileOverlayMenuButton* pFile = new FileOverlayMenuButton;
                     pFile->SetText(ovlmgr->GetOverlayDisplayName(p));
                     pFile->SetOverlay(p, p->get_m_overlayDescGuid());

                     pFile->SetID(next_id);
                     pFile->SetImageIndex(75, FALSE);
                     pFile->SetToolTipText(file_spec);

                     AddButton(pFile);
                     next_id++;
                  }
               }
            }

            p = ovlmgr->get_next(p);
         }
      }

      void FileOverlayRibbonButtonsGroup::OnDraw(CDC* pDC)
      {
         ASSERT_VALID(this);
         ASSERT_VALID(pDC);

         if (m_rect.IsRectEmpty())
         {
            return;
         }

         for (int i = 0; i < m_arButtons.GetSize(); i++)
         {
            CMFCRibbonBaseElement* pButton = m_arButtons [i];
            ASSERT_VALID(pButton);

            pButton->OnDraw(pDC);
         }
      }

      BOOL FileOverlayRibbonButtonsGroup::OnMenuKey(UINT nUpperChar)
      {
         ASSERT_VALID(this);

         for (int i = 0; i < m_arButtons.GetSize(); i++)
         {
            CMFCRibbonButton* pButton = DYNAMIC_DOWNCAST(CMFCRibbonButton, m_arButtons [i]);

            if (pButton == NULL)
            {
               continue;
            }

            ASSERT_VALID(pButton);

            CString strLabel = pButton->GetText();

            int iAmpOffset = strLabel.Find(_T('&'));
            if (iAmpOffset >= 0 && iAmpOffset < strLabel.GetLength() - 1)
            {
               TCHAR szChar [2] = { strLabel.GetAt(iAmpOffset + 1), '\0' };
               CharUpper(szChar);

               if ((UINT)(szChar [0]) == nUpperChar && !pButton->IsDisabled())
               {
                  pButton->OnClick(pButton->GetRect().TopLeft());
                  return TRUE;
               }
            }
         }

         return FALSE;
      }


      void CMainFrame::build_application_menu()
      {
         m_applicationButton.SetImage(IDB_MAIN);
         m_applicationButton.SetToolTipText("File");
         m_applicationButton.SetDescription("Click here to see everything you can do with your overlays, including saving, printing or sharing them with others");
         m_applicationButton.SetID(IDC_FILE_FOUND_MSG);

         m_wndRibbonBar.SetApplicationButton(&m_applicationButton, CSize(45, 45));

         CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory("Main Menu", IDB_FVW_MAIN_SMALL, IDB_FVW_MAIN_LARGE);

         pMainPanel->Add(new CMFCRibbonButton(ID_FILE_NEW, "&New", 0, 0 ) );
         pMainPanel->Add(new CMFCRibbonButton(ID_FILE_OPEN, "&Open", 1, 1));
         pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_FV, "&Save", 3, 3));
         pMainPanel->Add(new CMFCRibbonButton(ID_SAVE_ALL, "Save &All", 4, 4));
         pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS_FV, "Save As...", 29, 29));
         pMainPanel->Add(new CMFCRibbonButton(ID_OPEN_MISSION_PACKAGE, "Mission Package...", 24, 24));
         pMainPanel->Add(new CMFCRibbonButton(ID_PUBLISH_COLLABORATION, "Publish Overlay...", -1, -1));

         pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

         CMFCRibbonButton* pExport = new CMFCRibbonButton(ID_EDIT_COPYMAP, "Export Map", 25, 25);
         pExport->AddSubItem(new CMFCRibbonLabel("Export the current map to various formats"));
         pExport->AddSubItem(new CMFCRibbonButton(ID_EDIT_COPYMAP, "To &Clipboard", 25, 25));
         pExport->AddSubItem(new CMFCRibbonButton(ID_EDIT_COPY_MAP_TO_GEOTIFF, "To &GeoTIFF...", 26, 26));
         pExport->AddSubItem(new CMFCRibbonButton(ID_EDIT_COPY_MAP_TO_JPEG, "To &JPEG...", 27, 27));
         pExport->AddSubItem(new CMFCRibbonButton(ID_EDIT_COPY_MAP_TO_PNG, "To &PNG...", 28, 28));
         pExport->AddSubItem(new CMFCRibbonButton(ID_EDIT_COPY_MAP_TO_KMZ, "To &KMZ...", 84, 84));
         pExport->SetDefaultCommand(FALSE);
         pMainPanel->Add(pExport);

         pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

         CMFCRibbonButton* pPrint = new CMFCRibbonButton(ID_MAIN_PRINT, "&Print", 32, 32);
         pPrint->AddSubItem(new CMFCRibbonLabel("Preview and print the map and overlays"));
         pPrint->AddSubItem(new CMFCRibbonButton(ID_MAIN_PRINT_DIRECT, "&Quick Print", 5, 5));
         pPrint->AddSubItem(new CMFCRibbonButton(ID_MAIN_PRINT_PREVIEW, "Print Preview", 30, 30));
         pPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, "P&rint Setup", 31, 31));
         pPrint->AddSubItem(new CMFCRibbonButton(ID_OPTIONS_PRINTING, "Printing Options...", 72, 72));
         pPrint->AddSubItem(new CMFCRibbonSeparator(TRUE));
         pPrint->AddSubItem(new CMFCRibbonButton(ID_TOOLS_CALIBRATEPIXELSPERINCH, "Calibrate Pixels Per Inch", 71, 71));
         pMainPanel->Add(pPrint);

         CMFCRibbonButton* pSend = new CMFCRibbonButton(ID_FILE_SEND_MAIL, "Send", 33, 33);
         pSend->AddSubItem(new CMFCRibbonLabel("Send a copy of the overlay to other people"));
         pSend->AddSubItem(new CMFCRibbonButton(ID_FILE_SEND_MAIL, "M&ail Recipient (as Attachment)...", 33, 33));
         pMainPanel->Add(pSend);

         pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

         CMFCRibbonButton* pHelp = new CMFCRibbonButton(ID_HELP_FINDER, "Help", 21, 21);
         pHelp->AddSubItem(new CMFCRibbonLabel("Help and About"));
         pHelp->AddSubItem(new CMFCRibbonButton(ID_HELP_FINDER, "Help Topics", 21, 21));
         pHelp->AddSubItem(new CMFCRibbonButton(ID_HELP_TIPOFTHEDAY, "Tip of the Day", 34, 34));
         pHelp->AddSubItem(new CMFCRibbonButton(ID_HELP_TECHNICALSUPPORT, "Technical Support", 45, 45));
         pHelp->AddSubItem(new CMFCRibbonButton(ID_APP_ABOUT, "About FalconView", 46, 46));
         pMainPanel->Add(pHelp);

         pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

         pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE_FV, "&Close...", 2, 2)); // Close overlay\nClose overlay

         // The panel will own and destroy the FileOverlayRibbonButtonsGroup
         m_file_overlay_ribbon_btns_group = new FileOverlayRibbonButtonsGroup();
         pMainPanel->AddToRight(m_file_overlay_ribbon_btns_group);

#ifdef GOV_RELEASE
         pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_XPLAN_OPTIONS, "Options", 73));
#endif
         pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, "Exit", 74));
}

class CPanelWithLaunchButton : 
   public CMFCRibbonPanel
{
   // NOTE: Once we move to Visual Studio 2012,
   // We can use this #define if desired, because
   // the RibbonLaunchButton is coming back.
   // #ifdef ENABLE_RIBBON_LAUNCH_BUTTON
   // CMFCRibbonLaunchButton       m_btnLaunch;
   // #endif // ENABLE_RIBBON_LAUNCH_BUTTON

   CMFCRibbonButton  m_btn_launch;
   UINT              m_command_id;
   bool              m_focus_launch;

public:
   DECLARE_DYNCREATE(CPanelWithLaunchButton)
   // Need default constructor for runtime cast
   CPanelWithLaunchButton() : CMFCRibbonPanel(), 
      m_command_id(-1), m_focus_launch(false)  {}

   void  SetLaunchButtonCommandId( UINT nID ) 
      { m_command_id = nID;}
   
   void  SetLaunchButtonTooltip( CString sTip ) 
   { m_btn_launch.SetToolTipText( sTip ); }
   
   CMFCRibbonBaseElement* HitTest(CPoint point, BOOL bCheckPanelCaption)
   {
      m_focus_launch = false;

      CMFCRibbonBaseElement* base_elem = 
         CMFCRibbonPanel::HitTest( point, bCheckPanelCaption );
      
      if ( base_elem != nullptr )
      {
         return base_elem;
      }
      else if ( m_btn_launch.GetRect().PtInRect( point ) )
      {
         m_focus_launch = true;
         return &m_btn_launch;       
      }
      else
      {
         return nullptr;
      }
   }

   CMFCRibbonBaseElement* MouseButtonDown(CPoint point)
   {
      // Give the launch button first crack since we
      // may be hidden/blocked by the caption hit test
      if ( m_btn_launch.GetRect().PtInRect( point ) )
      {
         CMainFrame* frame = fvw_get_frame();
         if (frame != nullptr)
         {
            frame->PostMessage(WM_COMMAND, m_command_id, 0);
         }
         
         // This derives from CMFCRibbonBaseElement, but we may
         // get side effects since we didn't put it in the list
         // of buttons in the Panel.
         return &m_btn_launch;
      }

      return CMFCRibbonPanel::MouseButtonDown( point );
   }

   void DoPaint(CDC* pDC)
   {
      const CSize size_caption = GetCaptionSize(pDC);
      CRect rect_launch = m_rect;
      rect_launch.DeflateRect(1, 1);

      rect_launch.top = rect_launch.bottom - size_caption.cy + 1;
      rect_launch.left = rect_launch.right - size_caption.cy;
      rect_launch.bottom--;
      rect_launch.right--;

      m_btn_launch.SetRect(rect_launch);
      
      CMFCRibbonPanel::DoPaint( pDC );

      // NOTE: This was taken out of the runtimes and so can't be used
      // CMFCVisualManager::GetInstance()->OnDrawRibbonLaunchButton(
      //    pDC, this, m_pParentPanel);
      
      bool is_highlighted = ( IsHighlighted() && m_focus_launch == true );
      CMenuImages::Draw(pDC, CMenuImages::IdLaunchArrow, this->m_btn_launch.GetRect(), 
         is_highlighted == true ? CMenuImages::ImageWhite : CMenuImages::ImageBlack);
   }
};

// Need this to use RUNTIME_CLASS
IMPLEMENT_DYNCREATE(CPanelWithLaunchButton,CMFCRibbonPanel)

void CMainFrame::build_map_category()
{
   CMFCRibbonCategory* pCategory =
      m_wndRibbonBar.AddCategory("Map", IDB_FVW_MAIN_SMALL, IDB_FVW_MAIN_LARGE);

   CMFCRibbonPanel* pPanel;

   // Map Selection panel
   pPanel = pCategory->AddPanel("Map Selection");

   pPanel->Add(new CMFCRibbonButton(ID_MAP_NEW, "", 18, 18));

   m_map_menu->m_ribbon_btn = new DisableAddToQuickAccessToolbarRibbonButton(
      IDC_MAP_MENU, "Maps");
   RebuildMapMenu();
   pPanel->Add(m_map_menu->m_ribbon_btn);

   CMFCRibbonButton* projections = new CMFCRibbonButton(IDC_MAP_PROJECTION,
      "Projection", -1, -1);
   projections->SetMenu(IDR_PROJECTIONS_MENU);

   // See if 3D support is not enabled in the registry and remove General Perspective projection.
   if (!Is3DEnabledInRegistry())
      projections->RemoveSubItem(projections->FindSubItemIndexByID(ID_MAP_PROJECTION_06));

   pPanel->Add(projections);

   m_favorites_menu = new DisableAddToQuickAccessToolbarRibbonButton(
      ID_FAVORITES_LIST, "   Favorites   ");
   UpdateFavoritesMenu();
   pPanel->Add(m_favorites_menu);
   pPanel->Add(new CMFCRibbonButton(ID_FAVORITES_ADD, "Add Favorite", 78, 78));

   // Map Scaling panel
   pPanel = pCategory->AddPanel("Map Scaling");
   pPanel->Add(new CMFCRibbonButton(ID_SCALE_ZOOM, "Scale Tool", 10, 10));
   pPanel->Add(new CMFCRibbonButton(ID_MAP_SCALEIN, "Scale In", 11, 11));
   pPanel->Add(new CMFCRibbonButton(ID_MAP_SCALEOUT, "Scale Out", 12, 12));

   // Map Panning panel
   pPanel = pCategory->AddPanel("Map Panning / Rotation");
   pPanel->Add(new CMFCRibbonButton(ID_HAND_MODE, "Pan Tool", 13, 13));

   CMFCRibbonButtonsGroup* pGroup = new CMFCRibbonButtonsGroup;
   pGroup->AddButton(new MomentaryRibbonButton(ID_MAP_PAN_LEFT, "Pan Left", 16, 16));
   pGroup->AddButton(new MomentaryRibbonButton(ID_MAP_PAN_UP, "Pan Up", 14, 14));
   pGroup->AddButton(new MomentaryRibbonButton(ID_MAP_PAN_DOWN, "Pan Down", 15, 15));
   pGroup->AddButton(new MomentaryRibbonButton(ID_MAP_PAN_RIGHT, "Pan Right", 17, 17));
   pPanel->Add(pGroup);

   pGroup = new CMFCRibbonButtonsGroup;
   pGroup->AddButton(new MomentaryRibbonButton(ID_MAP_ROTATE_CLOCKWISE, "Rotate Left", 47, 47));
   pGroup->AddButton(new MomentaryRibbonButton(ID_MAP_ROTATE_NORTHUP, "North Up", 49, 49));
   pGroup->AddButton(new MomentaryRibbonButton(ID_MAP_ROTATE_COUNTERCLOCKWISE, "Rotate Right", 48, 48));
   pPanel->Add(pGroup);

   // Map Zooming panel
   pPanel = pCategory->AddPanel("Map Zooming");

   CMFCRibbonButton* zoom_btn = new CMFCRibbonButton(ID_SCALE_PERCENT_ZOOM,
      "", 9, 9);
   zoom_btn->SetTextAlwaysOnRight();
   pPanel->Add(zoom_btn);

   pPanel->Add(new ZoomPercentRibbonComboBox(ID_ZOOM_COMBO));
   pPanel->Add(new CMFCRibbonButton(ID_ZOOM_IN, "Zoom In", 51, 51));
   pPanel->Add(new CMFCRibbonButton(ID_ZOOM_OUT, "Zoom Out", 52, 52));

   if (Is3DEnabledInRegistry())
   {
      // Map tilt panel
      pPanel = pCategory->AddPanel("Map Tilt (3D)");
      pPanel->Add(new CMFCRibbonLabel("      Tilt    "));

      m_p3DMapTiltSlider = new MapTiltSlider(ID_MAP_3D_MAP_TILT_SLIDER);
      pPanel->Add(m_p3DMapTiltSlider);

      // Create custom camera mode panel
      CPanelWithLaunchButton* pCameraModePanel = 
         (CPanelWithLaunchButton*)pCategory->AddPanel("Camera Mode (3D)",
         0, RUNTIME_CLASS(CPanelWithLaunchButton));
      
      pCameraModePanel->SetLaunchButtonCommandId( ID_CAMERA_MODE_OPTIONS );
      pCameraModePanel->SetLaunchButtonTooltip( "Show Camera Mode (3D) Options" );
      pCameraModePanel->Add(new CMFCRibbonButton(ID_CAMERA_MODE_PAN, "Standard",
         89, 89));
      m_flight_mode_button = new CMFCRibbonButton(ID_CAMERA_MODE_FLIGHT,
         "Flight", RIBBON_BUTTON_IMAGE_FLIGHT_MODE_LOCKED,
         RIBBON_BUTTON_IMAGE_FLIGHT_MODE_LOCKED);
      pCameraModePanel->Add(m_flight_mode_button);
      pCameraModePanel->Add(new CMFCRibbonButton(ID_CAMERA_MODE_GROUND_VIEW, "Ground", 83, 83));
   }

   // Options panel
   pPanel = pCategory->AddPanel("Map Options");
   pPanel->Add(new CMFCRibbonButton(ID_MAP_OPTIONS, "Map Options", 68, 68));
   pPanel->Add(new CMFCRibbonButton(ID_OPTIONS_FORMAT, "Coordinate Format",
      57, 57));
}

void CMainFrame::build_display_category()
{
   CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory("Display",
      IDB_FVW_MAIN_SMALL, IDB_FVW_MAIN_LARGE);

   // Map Lighting panel
   CMFCRibbonPanel* pPanel = pCategory->AddPanel("Map Lighting");

   pPanel->Add(new CMFCRibbonButton(ID_DIM, "Dim Map", 7, 7));
   pPanel->Add(new CMFCRibbonButton(ID_BRIGHT, "Brighten Map", 8, 8));

   pPanel->Add(new CMFCRibbonButton(ID_DECREASE_CONTRAST, "Decrease Contrast", 55, 55));
   pPanel->Add(new CMFCRibbonButton(ID_INCREASE_CONTRAST, "Increase Contrast", 56, 56));
   pPanel->Add(new CMFCRibbonButton(ID_VIEW_ADJUST_BRIGHT_CONTRAST, "Adjust Brightness/Contrast", 53, 53));
   pPanel->Add(new CMFCRibbonButton(ID_VIEW_AUTO_BRIGHT_CONTRAST, "Auto Mode Brightness/Contrast", 54, 54));

   // Elevation panel
   //
   if (Is3DEnabledInRegistry())
   {     
      pPanel = pCategory->AddPanel("Terrain (3D)");

      pPanel->Add(new CMFCRibbonLabel("Elevation Exaggeration"));

      // The slider width will be controlled by the edit box (below)
      m_pElevationExaggerationSlider =
         new CMFCRibbonSlider( ID_ELEVATION_EXAGGERATION_SLIDER, 10 );

      // Set slider params per application requirements
      m_pElevationExaggerationSlider->SetZoomIncrement( 1 );
      m_pElevationExaggerationSlider->SetZoomButtons( TRUE );
      m_pElevationExaggerationSlider->SetPos(
         m_iElevationExaggerationValue =
            PRM_get_registry_int( "View", "ElevationExaggeration", 1 ), TRUE );
      m_pElevationExaggerationSlider->SetRange(
   #     ifdef _DEBUG
         PRM_get_registry_int( "View", "ElevationExaggerationMin", 0 ),
   #     else
         PRM_get_registry_int( "View", "ElevationExaggerationMin", 1 ),
   #     endif
         PRM_get_registry_int( "View", "ElevationExaggerationMax", 5 ) );
      m_pElevationExaggerationSlider->SetToolTipText(
         _T("Elevation Exaggeration (3D)"));

      pPanel->Add(m_pElevationExaggerationSlider);

      pPanel->Add( m_pElevationExaggerationEdit =
         new CMFCRibbonEdit(ID_ELEVATION_EXAGGERATION_EDIT, 200/*121*/));
      CString cs;
      cs.Format( _T("%d"), m_iElevationExaggerationValue );
      m_pElevationExaggerationEdit->SetEditText( cs );
      m_pElevationExaggerationEdit->SetTextAlign( ES_CENTER );
      m_pElevationExaggerationEdit->SetToolTipText(
         _T("Elevation Exaggeration (3D)"));

      pPanel->SetCenterColumnVert();
      pPanel->SetJustifyColumns();  // Stretches the slider to match the edit box

      pPanel->Add(new CMFCRibbonSeparator());

      // Undersea 3D elevations
      pPanel->Add(new CMFCRibbonButton(ID_UNDERSEA_3D_ELEVATIONS, "Use DBDB",
         85, 85));
      m_bUnderseaElevationsEnable = PRM_get_registry_int("View",
         "Enable_3D_DBDB_Elevation_Data", 1 ) != 0 ? TRUE : FALSE;
   }
}

void CMainFrame::build_overlay_category()
{
   m_pOverlaysCategory = m_wndRibbonBar.AddCategory("Overlays",
      IDB_FVW_MAIN_SMALL, IDB_FVW_MAIN_LARGE);

   // Overlay
   CMFCRibbonPanel* pPanel = m_pOverlaysCategory->AddPanel("Overlay Settings");
   pPanel->Add(new CMFCRibbonButton(ID_OVERLAY_OPTIONS, "Overlay Options", 79, 79));
   pPanel->Add(new CMFCRibbonButton(ID_OVERLAY_MANAGER, "Overlay Manager", 80, 80));
   pPanel->Add(new CMFCRibbonButton(ID_HIDE_BACKGROUND_OVERLAYS, "Hide Background Overlays", 81, 81));

   COverlayTypeDescriptorList* otdl = OVL_get_type_descriptor_list();
   otdl->ResetEnumerator();
   while (otdl->MoveNext())
   {
      OverlayTypeDescriptor* overlay_type = otdl->m_pCurrent;

      // skip file overlays and overlays without display names
      // and non-user controllable top most overlays
      if (overlay_type->fileTypeDescriptor.bIsFileOverlay ||
         overlay_type->displayName.IsEmpty()|| 
         (overlay_type->is_top_most && !overlay_type->is_user_controllable))
      {
         continue;
      }

      m_overlay_types_on_overlays_tab.push_back(
         overlay_type->overlayDescriptorGuid);
   }

   // sort the list of static overlays by display name
   std::sort(m_overlay_types_on_overlays_tab.begin(),
      m_overlay_types_on_overlays_tab.end(), [&](const GUID&a, const GUID& b)
   {
      return otdl->GetOverlayTypeDescriptor(a)->parentDisplayName + "//" + otdl->GetOverlayTypeDescriptor(a)->displayName <
         otdl->GetOverlayTypeDescriptor(b)->parentDisplayName + "//" + otdl->GetOverlayTypeDescriptor(b)->displayName;
   });

   m_pOverlaysPanel = m_pOverlaysCategory->AddPanel((LPCTSTR) "Overlay Types");

   m_pOverlaysPanel->Add(new CMFCRibbonButton(ID_CENTER_CROSSHAIR,
      "Center Crosshair", 19, 19));

   CString activeParentName = "null";  // cannot be empty string for proper panel creation
   CString parentName = "";
   CMFCRibbonPanel* activePanel = m_pOverlaysPanel;


   // add each of them to the Overlays tab
   UINT crnt_id = IDRANGE_OVERLAY_MENU_ITEM_1;
   std::for_each(m_overlay_types_on_overlays_tab.begin(),
      m_overlay_types_on_overlays_tab.end(), [&](const GUID&a)
   {
      OverlayTypeDescriptor* overlay_type = otdl->GetOverlayTypeDescriptor(a);

      parentName = overlay_type->parentDisplayName;
      if (parentName != activeParentName) 
      {
         if (parentName == ((LPCTSTR) "")) 
         {
            activePanel = m_pOverlaysPanel;
            activeParentName = (LPCTSTR) "";
         }
         else 
         {
            activePanel = m_pOverlaysCategory->AddPanel((LPCTSTR)parentName); // reached a new parent category, create a new panel
            activeParentName = parentName;
         }
      }

      CMFCRibbonButton* btn = new CMFCRibbonButton(crnt_id++,
         overlay_type->displayName, overlay_type->pIconImage->get_icon(32), 0,
         overlay_type->pIconImage->get_icon(16)); 

      btn->SetToolTipText(overlay_type->displayName);

      CString desc;
      desc.Format("Toggle the %s overlay", overlay_type->displayName);
      btn->SetDescription(desc);

      activePanel->Add(btn);
   });
}


void CMainFrame::build_edit_category()
{
   m_pEditorsCategory = m_wndRibbonBar.AddCategory("Editors",
      IDB_FVW_MAIN_SMALL, IDB_FVW_MAIN_LARGE);

   m_pEditorsPanel = m_pEditorsCategory->AddPanel("Overlay Editors");

   // Editors panel
   m_pEditorsCategory->AddHidden(new CMFCRibbonButton(ID_EDIT_CUT, "Cut", 41, 41));
   m_pEditorsCategory->AddHidden(new CMFCRibbonButton(ID_EDIT_COPY, "Copy", 42, 42));
   m_pEditorsCategory->AddHidden(new CMFCRibbonButton(ID_EDIT_PASTE, "Paste", 43, 43));
   m_pEditorsCategory->AddHidden(new CMFCRibbonButton(ID_EDIT_UNDO, "Undo", 39, 39));
   m_pEditorsCategory->AddHidden(new CMFCRibbonButton(ID_EDIT_REDO, "Redo", 40, 40));
}  // build_edit_category()

void CMainFrame::build_view_category()
{
   CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory("View", IDB_FVW_MAIN_SMALL, IDB_FVW_MAIN_LARGE);

   // User Interface panel
   CMFCRibbonPanel* pPanel = pCategory->AddPanel("User Interface");
   pPanel->Add(new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, "Status Bar"));
   pPanel->Add(new CMFCRibbonButton(ID_OPTIONS_STATUSBARSETUP, "Status Bar Setup", 59, 59));
   pPanel->AddSeparator();
   pPanel->Add(new CMFCRibbonCheckBox(ID_VIEW_TITLEBAR, "Ribbon Bar"));
   pPanel->Add(new CMFCRibbonButton(ID_OPTIONS_SECURITY, "Security Options", 60, 60));
   pPanel->AddSeparator();
   pPanel->Add(new CMFCRibbonCheckBox(ID_VIEW_MAP_TABS_BAR, "Map Tabs Bar"));
   pPanel->Add(new CMFCRibbonButton(ID_MAP_GROUPS, "Map Groups", 50, 50));
   pPanel->AddSeparator();
   pPanel->Add(new CMFCRibbonButton(ID_GRAPHIC_DRAWING_PREFS, "Graphic Drawing Prefs", 82, 82));
   pPanel->AddSeparator();
   pPanel->Add(new CMFCRibbonCheckBox(ID_VIEW_SPLIT, "Vertical Display Window"));
   pPanel->Add(new CMFCRibbonButton(ID_OPTIONS_VERTICAL_DISPLAY_PROPERTIES, "Vertical Display Properties", 58, 58));
   pPanel->AddSeparator();
   pPanel->Add(new CMFCRibbonButton(ID_VIEW_NEWTAB, "New Display Tab", 44, 44));
   pPanel->AddSeparator();
   pPanel->Add(new CMFCRibbonCheckBox(ID_VIEW_FILE_OVERLAY_LIST,
      "File Overlay List"));
}

void CMainFrame::build_tools_category()
{
   CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory("Tools", IDB_FVW_MAIN_SMALL, IDB_FVW_MAIN_LARGE);

   // Tools panel
   CMFCRibbonPanel* pPanel = pCategory->AddPanel("Tools");
   pPanel->Add(new CMFCRibbonButton(ID_SUBSCRIBE_COLLABORATION, "Collaboration Guide", 63, 63));
   pPanel->Add(new CMFCRibbonButton(ID_PHOTO_GEOTAGGING_TOOL, "Photo Geotagging Tools", 65, 65));

   if (SlatToolLauncher().IsToolAvailable())
      pPanel->Add(new CMFCRibbonButton(ID_SLAT_TOOL, "Solar Lunar Analysis Tool", 62, 62));

   pPanel->Add(new CMFCRibbonButton(ID_EDIT_TARGET_GRAPHIC_PRINTING, "Target Graphic Printing", 61, 61));

   // Playback panel
   pPanel = pCategory->AddPanel("Playback");
   pPanel->Add(new CMFCRibbonButton(ID_VIEWTIME, "Playback Dialog", 20, 20));

   CMFCRibbonButtonsGroup* pGroup = new CMFCRibbonButtonsGroup;
   pGroup->AddButton(new CMFCRibbonButton(ID_PLAYBACK_START, "Set to Beginning", 35, 35));
   pGroup->AddButton(new CMFCRibbonButton(ID_PLAYBACK_PLAY, "Start Playback", 36, 36));
   pGroup->AddButton(new CMFCRibbonButton(ID_PLAYBACK_STOP, "Stop Playback", 37, 37));
   pGroup->AddButton(new CMFCRibbonButton(ID_PLAYBACK_END, "Set to End", 38, 38));
   pPanel->Add(pGroup);

   // Custom tools panel
   m_pCustomToolsPanel = pCategory->AddPanel("Custom");
   m_pCustomToolsPanel->Add(new CMFCRibbonButton(ID_CUSTOM_TOOLS_MANAGER, "Custom Tools Manager", 69, 69));

   // set up custom tool bar for 3rd party tools registered in the registry
   m_custom_toolbar = new CCustomTBClass();
   m_custom_toolbar->AddCustomToolsToPanel(m_pCustomToolsPanel);
}

void CMainFrame::build_data_admin_category()
{
   CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(
      "Data Administration", IDB_FVW_MAIN_SMALL, IDB_FVW_MAIN_LARGE);

   // Data Adminstration panel
   CMFCRibbonPanel* pPanel = pCategory->AddPanel("Data Administration");

   SystemHealthRibbonButton* btn = new SystemHealthRibbonButton(
      ID_SYSTEM_HEALTH_RIBBON_BUTTON, "System Health", 66, 66);

   btn->SetButtonMode();
   pPanel->Add(btn);
   pPanel->AddSeparator();
   pPanel->Add(new CMFCRibbonButton(ID_TOOLS_DATACHECK, "Data Check", 70, 70));

   pPanel->Add(new CMFCRibbonButton(ID_DAFIF_SELECTION, "DAFIF Selection", 76,
      76));

   m_pMapDataManagerPanel = pPanel;
}

void CMainFrame::DockControlBarFlushRight(CToolBar* pBar, CToolBar* pRightOf)
{
   // get MFC to adjust the dimensions of all docked ToolBars so that GetWindowRect will be accurate
   RecalcLayout();

   CRect rect, parentRect, barRect;
   pRightOf->GetWindowRect(&rect);
   pRightOf->GetParent()->GetWindowRect(&parentRect);
   pBar->GetWindowRect(&barRect);

   rect.right = parentRect.right;
   rect.left = parentRect.right - barRect.Width();

   DockControlBar(pBar, AFX_IDW_DOCKBAR_TOP, rect);
}

void CMainFrame::DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf)
{
   CRect rect;
   DWORD dw;
   UINT n;

   // get MFC to adjust the dimensions of all docked ToolBars so that GetWindowRect will be accurate
   RecalcLayout();
   LeftOf->GetWindowRect(&rect);
   rect.OffsetRect(1,0);
   dw=LeftOf->GetBarStyle();
   n = 0;
   n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
   n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
   n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
   n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

   // When we take the default parameters on rect, DockControlBar will dock
   // each Toolbar on a seperate line.  By calculating a rectangle, we in 
   // effect are simulating a Toolbar being dragged to that location and 
   // docked.
   DockControlBar(Bar,n,&rect);
}

void CMainFrame::RunOverlayCustomInitializers()
{
   std::vector<OverlayTypeDescriptor *> overlayTypesToDisable;
   OVL_get_type_descriptor_list()->ResetEnumerator();
   while (OVL_get_type_descriptor_list()->MoveNext())
   {
      OverlayTypeDescriptor *pOverlayTypeDesc =
         OVL_get_type_descriptor_list()->m_pCurrent;

      if (pOverlayTypeDesc->pCustomInitializer != NULL)
      {
         // disable the overlay type in the case of failure
         if (pOverlayTypeDesc->pCustomInitializer->InitializeAtStartup() !=
            SUCCESS)
         {
            pOverlayTypeDesc->pCustomInitializer->TerminateAtShutdown();

            // note that we cannot immediately remove the overlay without
            // invalidating the iterator
            overlayTypesToDisable.push_back(pOverlayTypeDesc);
         }
      }
   }

   for (size_t i=0; i<overlayTypesToDisable.size(); ++i)
   {
      OVL_get_type_descriptor_list()->DisableOverlayType(overlayTypesToDisable[i]);
   }
}

void CMainFrame::RunNonOverlayCustomInitializers()
{
   try
   {
      IFvCustomInitializersConfigPtr spCustomInitializers;
      CO_CREATE(spCustomInitializers, CLSID_FvCustomInitializersConfig);

      spCustomInitializers->Initialize();
      BOOL bRet = spCustomInitializers->SelectAll();
      while (bRet)
      {
         if (spCustomInitializers->IsEnabled)
         {
            CCustomInitializerCOM* pCustomInit = new CCustomInitializerCOM(
               spCustomInitializers->CustomInitializerClsid);
            if (pCustomInit->InitializeAtStartup() != SUCCESS)
            {
               pCustomInit->TerminateAtShutdown();
               delete pCustomInit;
            }
            else
               m_customInitializers.push_back(pCustomInit);
         }

         bRet = spCustomInitializers->MoveNext();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error loading custom initializers - %s",
         (char *)e.Description());
      ERR_report(msg);
   }
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   // if the GUI is hidden - hide the frame and all it's children
   if (fvw_get_app()->is_gui_hidden())
      AfxGetApp()->m_nCmdShow = SW_HIDE;

   if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
      return -1;

   FalconViewVisualManager::SetDefaultManager(
      RUNTIME_CLASS(FalconViewVisualManager));
   FalconViewVisualManager::SetStyle(
      FalconViewVisualManager::Office2007_Silver);

   RunNonOverlayCustomInitializers();
   RunOverlayCustomInitializers();

   // Create the FalconView System Health Status object which is a singleton and used by many inprocess components.
   m_pSystemHealth = new CFvSystemHealthCOM();

   if (create_ribbon_bar() != SUCCESS)
   {
      ERR_report("Failed to create ribbon bar.");
      return -1;
   }

   // initialize overlay manager
   CSplashWnd::Message("Initializing overlay manager...");
   if (OVL_get_overlay_manager()->initialize() != SUCCESS)
   {
      ERR_report("C_ovl_mgr::initialize() failed.");
      return -1;
   }

   EnableDocking(CBRS_ALIGN_ANY);
   CFrameWnd::EnableDocking(CBRS_ALIGN_ANY);

   // enable Visual Studio 2005 style docking window behavior
   CDockingManager::SetDockingMode(DT_SMART);

   // enable Visual Studio 2005 style docking window auto-hide behavior
   EnableAutoHidePanes(CBRS_ALIGN_ANY);

   CreateMapTabsBar(false);   // wait to update tabs until PostCreateWindow

   m_pMapViewTabsBar = new CMapViewTabsBar();
   m_pMapViewTabsBar->Create(this, WS_CHILD|WS_VISIBLE|CBRS_TOP, IDC_MAP_VIEW_TABS_BAR);
   m_pMapViewTabsBar->Invalidate();

   // Enable tool tips for the status bar
   for(int i=0;i<m_statusBarMgr->m_arrStatusBars.GetSize();++i)
      m_statusBarMgr->m_arrStatusBars[i]->CBasePane::SetPaneStyle(
      m_statusBarMgr->m_arrStatusBars[i]->CBasePane::GetPaneStyle() | CBRS_TOOLTIPS);

   // the map tabs should default to off
   if (m_pMapTabsBar != NULL)
      m_pMapTabsBar->ShowPane(FALSE, FALSE, FALSE);

   // Create dockable panes specified in the DockablePanes.xml configuration file
   //
   try
   {
      IDockablePanesConfigPtr spDockablePanesConfig;
      CO_CREATE(spDockablePanesConfig, CLSID_DockablePanesConfig);

      spDockablePanesConfig->Initialize();
      long bRet = spDockablePanesConfig->SelectAll();
      while (bRet)
      {
         if (spDockablePanesConfig->IsEnabled)
         {
            CFvDockablePane* pDockablePane = CreateDockablePane(
               spDockablePanesConfig->ClassId, spDockablePanesConfig->DockablePaneGuid,
               (char *)spDockablePanesConfig->WindowName, (char *)spDockablePanesConfig->IconName,
               CPoint(spDockablePanesConfig->InitialPosX, spDockablePanesConfig->InitialPosY),
               CSize(spDockablePanesConfig->InitialSizeX, spDockablePanesConfig->InitialSizeY),
               spDockablePanesConfig->HasCaption,
               (FalconViewOverlayLib::DockablePaneAlignment)spDockablePanesConfig->InitialAlignment,
               spDockablePanesConfig->AllowableAlignments,
               spDockablePanesConfig->CanClose, spDockablePanesConfig->CanResize,
               CSize(spDockablePanesConfig->MinimumSizeX, spDockablePanesConfig->MinimumSizeY),
               spDockablePanesConfig->CanAutoHide);
         }

         bRet = spDockablePanesConfig->MoveNext();
      }
   }
   catch(_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }

   CSplashWnd::Message("Initializing FalconView...");

   ////////////////////////////////////////////////////////
   //
   // Add and remove items to/from the main menu
   //
   ////////////////////////////////////////////

   // Remove the page layout specific menu items from the File menu.  These
   // menu items are added when the first/only page layout overlay is opened
   // or created, and they are removed when the last/only page layout overlay
   // is closed.
   OnRemovePrintMenuItems(0, 0);

   // Get the Tools menu item, not important anymore
   try
   {
      IFvToolMenuItemsConfig2Ptr spToolMenuItemsConfig;
      CO_CREATE(spToolMenuItemsConfig, CLSID_FvToolMenuItemsConfig);
      spToolMenuItemsConfig->Initialize();
      BOOL bRet = spToolMenuItemsConfig->SelectAll();

      // Add the Additional Tools panel only if there are additonal tools to add.
      if (bRet)
      {
         // Find the category
         const int num_categories = m_wndRibbonBar.GetCategoryCount();
         for (int i=0; i<num_categories; ++i)
         {
            CMFCRibbonCategory* category = m_wndRibbonBar.GetCategory(i);
            CString categoryName = category->GetName();

            if (categoryName == CString("Tools"))
            {
               // Add the panel to the category
               m_pAdditionalToolsPanel = category->AddPanel((LPCTSTR)"Additional Tools");

               UINT menuItemId = IDRANGE_TOOL_MENU_ITEM_COM_00;

               while (bRet && menuItemId <= IDRANGE_TOOL_MENU_ITEM_COM_24)
               {
                  if (spToolMenuItemsConfig->IsEnabled)
                  {
                     // Add the button information, so that we can launch the tool when the button is pressed.
                     CFvToolMenuItemCOM* pToolMenuItem = new CFvToolMenuItemCOM(
                        spToolMenuItemsConfig->MenuItemGuid,
                        spToolMenuItemsConfig->clsid, (char *)spToolMenuItemsConfig->HelpFilename,
                        spToolMenuItemsConfig->HelpId,
                        (char *)spToolMenuItemsConfig->HelpURI);

                     m_toolMenuItems[menuItemId] = pToolMenuItem;

                     // Add the button to the ribbon panel
                     CMFCRibbonButton* addTools = new CMFCRibbonButton(menuItemId, (char *)spToolMenuItemsConfig->DisplayName, 69, 69);
                     m_pAdditionalToolsPanel->Add(addTools);
                     menuItemId++;
                  }
                  bRet = spToolMenuItemsConfig->MoveNext();
               }

               if (bRet)
               {
                  // We had additional items beyond the maximum allowed.  Warn the user in the error log.
                  CString msg;
                  msg.Format("More tools in Tool Menu Items Configuration than allowed.   Maximum - %d", (IDRANGE_TOOL_MENU_ITEM_COM_24 - IDRANGE_TOOL_MENU_ITEM_COM_00) + 1);
                  ERR_report(msg);
               }
               break; // for
            }
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed adding tools menu items - %s",
         (char *)e.Description());
      ERR_report(msg);
   }

   // add the editor buttons to the Editors pane
   if (m_pEditorsPanel)
   {
      OVL_get_overlay_manager()->BuildEditorRibbonPanel(m_pEditorsPanel,
         m_pMapDataManagerPanel, m_pEditorsCategory);
   }

   // Add the "optional" menu items to the Tools menu as needed.
   // In Debug builds, memory debugging items are added.
   // Other test menu item are also added depending on registry settings.
   CString doToolsMenu = PRM_get_registry_string("Developer", "Test_Menu", "NO");
   boolean_t add_tools_menu_items = TO_BOOLEAN_T(doToolsMenu.CompareNoCase("YES") == 0);
#ifdef _DEBUG
   boolean_t add_mem_debug_menu_items = TRUE;
#else
   boolean_t add_mem_debug_menu_items = FALSE;
#endif

   // Add projections to to the projection string map
   CArray <class ProjectionID, const class ProjectionID&> available_projection_array;
   PROJ_get_available_projection_types(&available_projection_array, Is3DEnabledInRegistry());
   CString name;
   for (int z=0; z<available_projection_array.GetSize() && z<20; z++)
   {
      name = available_projection_array.GetAt(z).get_string();

      CString status_txt;
      status_txt.Format("Switch to %s projection", name);

      // we want status bar messages for these menu items.  we create
      // a table of IDs mapping to strings for these projections
      m_projection_string_map.SetAt(ID_MAP_PROJECTION_01 + z, status_txt);
   }

   // if the Vertical Display window was open when Fvw last shut down...
   if (PRM_get_registry_int("VerticalDisplay", "VerticalDisplayOpen", 0))
   {
      // open the Vertical Display window...
      ToggleVerticalView(TRUE);
   }

   m_titlebarVisible = TRUE;

   return 0;
}


CFvDockablePane* CMainFrame::CreateDockablePane(GUID classId, GUID dockablePaneGuid,
   const CString& windowName, const CString& iconName,
   const CPoint& initialPos, const CSize& initialSize, long hasCaption,
   FalconViewOverlayLib::DockablePaneAlignment initialAlignment,
   long allowableAlignments, long canClose, long canResize, const CSize& minimumSize,
   long canAutoHide)
{
   CFvDockablePane* pDockablePane = new CFvDockablePane(classId, dockablePaneGuid);

   // Verify that the initial alignment given is one of the allowable alignments
   if (!(initialAlignment & allowableAlignments))
   {
      CString msg;
      msg.Format("Initial alignment (0x%X) is not one of the allowable alignments (0x%X)",
         initialAlignment, allowableAlignments);
      ERR_report(msg);
      delete pDockablePane;
      return nullptr;
   }

   if (pDockablePane->Create(
      this,
      windowName, iconName, initialPos, initialSize,
      hasCaption, initialAlignment, allowableAlignments,
      canClose, canResize, minimumSize, canAutoHide) == FAILURE)
   {
      string_utils::CGuidStringConverter guidString(dockablePaneGuid);

      CString msg;
      msg.Format("Failed creating dockable pane [dockable pane uid=%s]",
         guidString.GetGuidString().c_str());
      ERR_report(msg);
      return nullptr;
   }

   RecalcLayout();

   return pDockablePane;
}

bool CMainFrame::CreateMinimizedPanesPanel()
{
   CMFCRibbonBar* ribbonBar = GetRibbonBar();

   const int tabCount = ribbonBar->GetCategoryCount();
   CMFCRibbonCategory* pCategory;
   bool found = false;
   for (int iter = 0; iter < tabCount; iter++)
   {
      pCategory = ribbonBar->GetCategory(iter);
      _bstr_t tabName = pCategory->GetName();
      if (tabName == _bstr_t("View"))
      {
         found = true;
         break;
      }
   }

   if (!found)
      return nullptr;

   // Add the "Minimized Panes" panel, if necessary
   found = false;
   CMFCRibbonPanel* pPanel;
   const int panelCount = pCategory->GetPanelCount();
   for (int iter = 0; iter < panelCount; iter++)
   {
      pPanel = pCategory->GetPanel(iter);
      _bstr_t panelName = pPanel->GetName();
      if (panelName == _bstr_t("Minimized Panes"))
      {
         found = true;
         break;
      }
   }

   if (!found)
   {
      m_minimizedPanesPanel = pCategory->AddPanel("Minimized Panes");
      m_minimizedPanesPanel->SetCenterColumnVert(FALSE);
   }

   return true;
}

bool CMainFrame::HideMinimizedPanesPanel()
{
   if (m_minimizedPanesPanel == nullptr)
      return false;

   CMFCRibbonBar* ribbonBar = GetRibbonBar();
   const int tabCount = ribbonBar->GetCategoryCount();
   CMFCRibbonCategory* pCategory;
   for (int i = 0; i < tabCount; i++)
   {
      pCategory = ribbonBar->GetCategory(i);
      _bstr_t tabName = pCategory->GetName();
      if (tabName == _bstr_t("View"))
      {
         const int panelCount = pCategory->GetPanelCount();
         for (int j = 0; j < panelCount; j++)
         {
            if (pCategory->GetPanel(j) == m_minimizedPanesPanel)
            {
               pCategory->RemovePanel(j, TRUE);
               m_minimizedPanesPanel = nullptr;
               ribbonBar->ForceRecalcLayout();
               return true;
            }
         }
      }
   }

   return false;
}

CFvManagedDockablePane* CMainFrame::CreateManagedDockablePane(FalconViewOverlayLib::IFvFrameWnd3Ptr frameWnd,
   GUID classId, GUID dockablePaneGuid,
   const CString& windowName, const CString& iconName,
   const CPoint& initialPos, const CSize& initialSize, long hasCaption,
   FalconViewOverlayLib::DockablePaneAlignment initialAlignment,
   long allowableAlignments, long canClose, long canResize, const CSize& minimumSize,
   long canAutoHide)
{
   CFvManagedDockablePane* pDockablePane = new CFvManagedDockablePane(frameWnd, classId, dockablePaneGuid);

   // Verify that the initial alignment given is one of the allowable alignments
   if (!(initialAlignment & allowableAlignments))
   {
      CString msg;
      msg.Format("Initial alignment (0x%X) is not one of the allowable alignments (0x%X)",
         initialAlignment, allowableAlignments);
      ERR_report(msg);
      delete pDockablePane;
      return nullptr;
   }

   if (pDockablePane->Create(
      this,
      windowName, iconName, initialPos, initialSize,
      hasCaption, initialAlignment, allowableAlignments,
      canClose, canResize, minimumSize, canAutoHide) == FAILURE)
   {
      string_utils::CGuidStringConverter guidString(dockablePaneGuid);

      CString msg;
      msg.Format("Failed creating dockable pane [dockable pane uid=%s]",
         guidString.GetGuidString().c_str());
      ERR_report(msg);
      return nullptr;
   }

   // If the "Minimized Panes" panel has not been created yet, create it.
   if (m_minimizedPanesPanel == nullptr)
   {
      if (!CreateMinimizedPanesPanel())
         return false;
   }

   CMFCRibbonBar* ribbonBar = GetRibbonBar();

   // Create button
   CString buttonText;
   pDockablePane->GetWindowText(buttonText);

   _bstr_t bButtonText(buttonText.AllocSysString());
   _bstr_t bIconName(iconName.AllocSysString());

   CIconImage* pII = CIconImage::load_images( (LPCTSTR) _bstr_t( bIconName ) );
   HICON iconLarge = CopyIcon( pII->get_icon( 32 ));
   HICON iconSmall = CopyIcon( pII->get_icon( 16 ));

   UINT elementId;
   CRibbonImpl::GetNextRibbonElementId(&elementId);
   CMFCRibbonButton* button = new CMFCRibbonButton( elementId, bButtonText,
      CopyIcon( iconLarge ), FALSE, CopyIcon( iconSmall ), TRUE );

   CFvManagedDockablePaneButtonImpl* buttonImpl =
      new CFvManagedDockablePaneButtonImpl();
   buttonImpl->SetRibbonBaseElement(button);
   buttonImpl->SetDockablePane(pDockablePane);
   buttonImpl->SetParentPanel(m_minimizedPanesPanel);
   buttonImpl->SetRibbonBar(ribbonBar);

   button->SetData((DWORD_PTR) buttonImpl);

   pDockablePane->SetButtonImpl(buttonImpl);

   // Add button
   m_minimizedPanesPanel->Add(button);
   ribbonBar->ForceRecalcLayout();
   RecalcLayout();

   return pDockablePane;
}

// Dock one pane to another
void CMainFrame::AttachDockablePane(
   FalconViewOverlayLib::IFvDockablePane* pDockablePane,
   FalconViewOverlayLib::IFvDockablePane* pDockablePaneToAttachTo)
{
   CFvDockablePane* pPane = FindDockablePane( pDockablePane );
   CFvDockablePane* pPaneToAttachTo = FindDockablePane( pDockablePaneToAttachTo );

   if( pPaneToAttachTo != nullptr && pPane != nullptr )
   {
      if( pPane->IsAutoHideMode() )
         pPane->SetAutoHideMode(FALSE, CBRS_ALIGN_ANY );

      // Hide the pane while moving
      pPane->ShowPane( false, false, false );

      // Attach to the new pane
      pPane->AttachToTabWnd( pPaneToAttachTo, DM_SHOW );

      // Show the pane
      pPane->ShowPane( true, false, true );

      RecalcLayout();
   }
}

CFvDockablePane* CMainFrame::FindDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane)
{
   CObList lstBars;
   m_dockManager.GetPaneList(lstBars, TRUE, RUNTIME_CLASS(CFvDockablePane), TRUE);

   POSITION pos = lstBars.GetHeadPosition();
   while (pos)
   {
      CFvDockablePane* pCurrentPane = static_cast<CFvDockablePane *>(lstBars.GetNext(pos));
      if (pDockablePane == pCurrentPane->GetDockablePane())
      {
         return pCurrentPane;
      }
   }
   return nullptr;
}

// Make sure the dockable pane is visible
void CMainFrame::ShowDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane)
{
   CFvDockablePane* pPane = FindDockablePane(pDockablePane);
   if ( pPane != nullptr )
   {
      ShowPane( pPane, TRUE, FALSE, TRUE );
   }
}

// Make sure the dockable pane is visible
void CMainFrame::HideDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane)
{
   CFvDockablePane* pPane = FindDockablePane(pDockablePane);
   if ( pPane != nullptr )
   {
      ShowPane( pPane, FALSE, FALSE, TRUE );
      RecalcLayout();
   }
}

// Minimize the dockable pane
HRESULT CMainFrame::MinimizeDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane)
{
   HideDockablePane(pDockablePane);

   CFvManagedDockablePane* dockablePane =
      static_cast<CFvManagedDockablePane*>(FindDockablePane(pDockablePane));
   if (dockablePane == nullptr)
      return E_FAIL;

   return S_OK;
}

void CMainFrame::CloseDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane)
{
   CFvDockablePane* pPane = FindDockablePane(pDockablePane);
   if ( pPane != nullptr )
   {
      // If the dockable pane is set to auto-hide, then destroying it will
      // not properly clean up its entry in various MFC data structures. Make
      // sure auto-hide is disabled before destroying the pane.
      pPane->SetAutoHideMode(0, CBRS_ALIGN_ANY);
      pPane->DestroyWindow();
      RecalcLayout();
   }
}

void CMainFrame::PostCreateWindow()
{
   m_statusBarMgr->m_availableInfoPaneList = new CStatusBarAvailableInfoPaneList();
   m_statusBarMgr->m_availableInfoPaneList->Init();

   // Initialize the CARDG/ECRG currency panes
#ifdef GOV_RELEASE
   CIconInfoPane *pCurrencyCadrgPane = static_cast<CIconInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_CARDG_CURRENCY));
   CIconInfoPane *pCurrencyCadrgPaneVerbose = static_cast<CIconInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_CARDG_CURRENCY_VERBOSE));

   CIconInfoPane *pCurrencyEcrgPane = static_cast<CIconInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_ECRG_CURRENCY));
   CIconInfoPane *pCurrencyEcrgPaneVerbose = static_cast<CIconInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_ECRG_CURRENCY_VERBOSE));

   if (pCurrencyCadrgPane)
      pCurrencyCadrgPane->SetIcon(0, NULL, "");
   if (pCurrencyCadrgPaneVerbose)
      pCurrencyCadrgPaneVerbose->SetIcon(0, NULL, "CADRG Currency");

   if (pCurrencyEcrgPane)
      pCurrencyEcrgPane->SetIcon(0, NULL, "");
   if (pCurrencyEcrgPaneVerbose)
      pCurrencyEcrgPaneVerbose->SetIcon(0, NULL, "ECRG Currency");
#endif

   // Initialize Map Busy status pane
   CIconInfoPane *pMapBusyStatus = static_cast<CIconInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_MAP_BUSY_STATUS));
   if (pMapBusyStatus)
      pMapBusyStatus->SetIcon(0, IDI_CONNECTED, "Loading Data");

   // initialize map view
   MapView *view = static_cast<MapView *>(GetActiveView());
   if (view)
   {
      // Now that the MapView is initialized, sign up the zoom percent combo
      // box on the ribbon for change events
      ZoomPercentRibbonComboBox* zoom_percent_combo = DYNAMIC_DOWNCAST(
         ZoomPercentRibbonComboBox, m_wndRibbonBar.FindByID(ID_ZOOM_COMBO));
      zoom_percent_combo->RegisterForZoomPercentChanges();

      if (view->InitializeMap() != SUCCESS)
      {
         ERR_report("Unable to initialize map");
      }

      // Sign the map view tabs control up for map view notifications
      view->AddMapParamsListener(m_pMapViewTabsBar->GetCtrl());

      if (Is3DEnabledInRegistry())
         view->AddMapParamsListener(m_p3DMapTiltSlider);
   }

   // if the CurrentStatusBarConfig.xml exists, then use it
   CString strCurrentConfig = PRM_get_registry_string("Main", "USER_DATA", "") +
      "\\StatusBar\\CurrentStatusBarConfig.xml";
   BOOL bStatusBarLoadStatus = LoadStatusBarConfig(strCurrentConfig);

   // If unable to load the CurrentStatusBarConfig.xml, then try loading the default
   if (!bStatusBarLoadStatus)
   {
      // otherwise, load from the default
      CString strDefault = PRM_get_registry_string("Main", "HD_DATA", "") + 
         "\\StatusBar\\DefaultStatusBarConfig.xml";
      LoadStatusBarConfig(strDefault);
   }

   // This is the first chance for overlays created during the initialization
   // process to get hold of the frame window.
   C_ovl_mgr* overlay_manager = OVL_get_overlay_manager();
   C_overlay* overlay = overlay_manager->get_first();
   while( overlay != nullptr )
   {
      overlay->SetFrameWnd( GetFvFrameWnd() ) ;
      overlay = overlay_manager->get_next( overlay );
   }

}

FalconViewOverlayLib::IFvMapView* CMainFrame::GetFvMapView()
{
   if (m_wndSplitter.GetRowCount() > 0)
   {
      CWnd *pWnd = m_wndSplitter.GetPane(0, 0);
      ASSERT( pWnd->IsKindOf(RUNTIME_CLASS(MapView)) );

      MapView* pMapView = static_cast<MapView *>(pWnd);
      if (pMapView != NULL)
      {
         return pMapView->GetFvMapView();
      }
   }

   return NULL;
}

CVerticalViewDisplay *CMainFrame::GetVerticalDisplay()
{
   if (m_wndSplitter.GetRowCount() == 1)
      return NULL;

   CWnd *pWnd = m_wndSplitter.GetPane(1, 0);
   ASSERT( pWnd->IsKindOf(RUNTIME_CLASS(CVerticalViewDisplay)) );

   return static_cast<CVerticalViewDisplay *>(pWnd);
}

CVerticalViewProperties* CMainFrame::GetVerticalDisplayPropertiesObject()
{
   return m_pVerticalDisplayProperties;
}


//
// Override the default GetMessageString in order to substitute a message-specific
// subsitution nvalue into any status bar string that references it.
//
void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
   // if the nID is one of the projections, use the stored status text associated
   // with that ID
   if (m_projection_string_map.Lookup(nID, rMessage) != 0)
      return;

   // if the nID is one of Map->GeoTIFF types
   if (nID >= ID_SOURCE_GEOTIFF0 && nID <= ID_SOURCE_GEOTIFF31)
   {
      rMessage = "Change map to GeoTIFF";
      return;
   }

   // if the nID is one of the Map->Options
   if (nID >= ID_MAP_OPTIONS1 && nID <= ID_MAP_OPTIONS10)
   {
      rMessage = "Display options for the current map";
      return;
   }

   // if the nId is one of the Favorites
   if (nID >= IDRANGE_FAVORITE_MENU_ITEM_1 && nID <= (IDRANGE_FAVORITE_MENU_ITEM_1 + 1000))
   {
      rMessage = "Change map / open overlays stored in Favorite";
      return;
   }

   // give overlay manager and non-standard toolbars a chance to set the message
   if (!OVL_get_overlay_manager()->get_message_string(nID, rMessage))
   {
      CFrameWndEx::GetMessageString(nID, rMessage);
   }
}

BOOL CMainFrame::Is3DEnabledInRegistry()
{
   int reg_enable_3D = PRM_get_registry_int("Main", "Enable3D", 0);

   return (reg_enable_3D == REG_ENABLE_3D);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
   CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
   CFrameWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

BOOL CMainFrame::IsRibbonBarVisible()
{
   return m_wndRibbonBar.IsVisible();
}

int CMainFrame::ShowRibbonBar(boolean_t show)
{
   m_titlebarVisible = show;
   m_wndRibbonBar.ShowWindow(show ? SW_SHOW : SW_HIDE);

   CMFCVisualManager * visual_manager = CMFCVisualManager::GetInstance();
   if ( visual_manager->IsKindOf( RUNTIME_CLASS(FalconViewVisualManager) ) )
   {
      FalconViewVisualManager * fv_visual_manager =
         static_cast<FalconViewVisualManager *>(visual_manager) ;
      fv_visual_manager->m_caption_on = m_titlebarVisible?true:false ;
      if ( m_titlebarVisible )
      {
         DWORD add_style =
            afxGlobalData.DwmIsCompositionEnabled() ? WS_CAPTION : WS_BORDER;
         ModifyStyle( 0, add_style, SWP_FRAMECHANGED ) ;
      }
      fv_visual_manager->RedrawAll();
   }
   else if ( m_titlebarVisible )
      ModifyStyle( 0, WS_CAPTION, SWP_FRAMECHANGED ) ;
   if ( !m_titlebarVisible )
   {
      ModifyStyle( WS_DLGFRAME, 0, SWP_FRAMECHANGED ) ;
   }
   SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER
      | SWP_NOACTIVATE | SWP_FRAMECHANGED ) ;

   RecalcLayout();

   // after changing the layout...
   if ( !m_titlebarVisible )
   {
      int show_message = PRM_get_registry_int("FullScreenMessage","ShowFullScreenMessage", 1);

      if ( show_message == 1 )
      {
         // display a popup so the user knows what is going on
         FullScreenDialog dlg;
         dlg.m_prompt = "You have activated full screen mode.  To restore the "
            "ribbon bar, press\r\n\r\nEscape\r\n\r\nor\r\n\r\nCtrl+Alt+T";

         dlg.DoModal();

         if (dlg.m_noremind)
         {
            // remember not to show this anymore
            PRM_set_registry_int("FullScreenMessage","ShowFullScreenMessage", 0);
         }
      }

   }

   return 0;
}

void CMainFrame::OnViewTitlebar()
{
   ShowRibbonBar(!m_titlebarVisible);
}

void CMainFrame::OnUpdateViewTitlebar(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_titlebarVisible);
}

void CMainFrame::OnViewPlaybackToolbar()
{
   CControlBar *t = GetControlBar(IDR_PLAYBACK);
   ShowControlBar(t, !(t->IsWindowVisible()), FALSE);
}

void CMainFrame::OnUpdateViewPlaybackToolbar(CCmdUI* pCmdUI) 
{
   CControlBar *t = GetControlBar(IDR_PLAYBACK);
   pCmdUI->SetCheck(t->IsWindowVisible());
}

// playback button handlers
void CMainFrame::OnPlaybackStart()
{
   m_view_time_dlg->Reset();
}

void CMainFrame::OnUpdatePlaybackStart(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_view_time_dlg->get_controls_enabled());
}

void CMainFrame::OnPlaybackPlay()
{
   if (!m_view_time_dlg->in_playback())
   {
      // create the playback dialog, so the playback toolbar works
      // properly
      if (m_view_time_dlg->m_hWnd == NULL)
      {
         if (!m_view_time_dlg->Create(IDD_VIEWTIME, NULL))
            AfxMessageBox("Playback Control Dialog initialization failed...");
      }

      m_view_time_dlg->toggle_state();
   }
}

void CMainFrame::OnUpdatePlaybackPlay(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_view_time_dlg->get_controls_enabled());
}

void CMainFrame::OnPlaybackStop()
{
   if (m_view_time_dlg->in_playback())
      m_view_time_dlg->toggle_state();
}

void  CMainFrame::OnUpdatePlaybackStop(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_view_time_dlg->get_controls_enabled());
}

void CMainFrame::OnPlaybackEnd()
{
   m_view_time_dlg->Endset();
}

void CMainFrame::OnUpdatePlaybackEnd(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_view_time_dlg->get_controls_enabled());
}

// Open mission binder dialog
void CMainFrame::OnOpenMissionPackage()
{
   // close print preview window if active
   ClosePrintPreview();

   CString strInitialPath = PRM_get_registry_string("Main",
      "ReadWriteUserData");

   // ask the user for the mission binder file to open
   CString filter = "";
   CString default_ext = "";
   CString path = "";

#if GOV_RELEASE
   // See if XPlan is installed. We only want to support xmp when XPlan is installed, since it relies on XPlan functionality.
   xplan_package_service::IPackageServicePtr ps = NULL;
   HRESULT hr = ps.CreateInstance(xplan_package_service::CLSID_PackageService);
   ps = NULL;

   if (SUCCEEDED(hr))
   {
      filter = "Mission Package Files (*.xmp)|*.xmp|FalconView Mission Package Files (*.fmp)|*.fmp|All Files (*.*)|*.*||";
      default_ext = ".xmp";
      path = strInitialPath + "\\MissionPackages\\*.xmp";
   }
   else
   {
      filter = "FalconView Mission Package Files (*.fmp)|*.fmp|All Files (*.*)|*.*||";
      default_ext = ".fmp";
      path = strInitialPath + "\\MissionPackages\\*.fmp";
   }

#else
   // The open source version of FalconView does not have any support for XPlan.
   filter = "FalconView Mission Package Files (*.fmp)|*.fmp|All Files (*.*)|*.*||";
   default_ext = ".fmp";
   path = strInitialPath + "\\MissionPackages\\*.fmp";
#endif

   CFileDialog dlg(TRUE, (LPCTSTR) default_ext, (LPCTSTR) path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, (LPCTSTR) filter);
   dlg.m_ofn.lpstrTitle = "New / Open";

   if (dlg.DoModal() == IDOK)
   {
      CString path_name = dlg.GetPathName();

      // if the file exist we will Open it
      if (FIL_access(path_name, FIL_EXISTS) == SUCCESS)
      {
         const BOOL create_new = FALSE;
         open_binder(path_name, create_new);
      }
      // otherwise, we will create a New mission binder
      else
      {
         const BOOL create_new = TRUE;
         open_binder(path_name, create_new);
      }
   }
}

void CMainFrame::open_binder(CString& binder_name, BOOL create_new)
{
   if (m_mission_binder_dlg == NULL)
      m_mission_binder_dlg = new CMissionPackageDlg;

   if (m_mission_binder_dlg == NULL)
   {
      ERR_report("[CMainFrame::OnOpenMissionPackage] Unable to create mission package dialog");
      return;
   }

   int status;
   if (create_new)
      status = m_mission_binder_dlg->CreateNew(binder_name);
   else
      status = m_mission_binder_dlg->Load(binder_name);

   if (status != SUCCESS)
   {
      CString msg;
      msg.Format("Unable to successfully %s mission package",
         create_new ? "create" : "load");
      AfxMessageBox(msg);
   }
   else
   {
      if (m_mission_binder_dlg->m_hWnd == NULL)
         m_mission_binder_dlg->Create(IDD_MISSION_PACKAGE, NULL);
   }
}

// Open collaboration dialogs
void CMainFrame::OnSubscribeCollaboration()
{
   if ( m_pCollaborationServer )
   {
      m_pCollaborationServer->Subscribe();
   }
}

void CMainFrame::OnPublishCollaboration()
{
   if ( m_pCollaborationServer )
   {
      m_pCollaborationServer->Publish();
   }
}

void CMainFrame::OnUpdatePublishCollaboration(CCmdUI* pCmdUI)
{
   static VARIANT_BOOL bCanPublish = VARIANT_FALSE;
   if ( m_pCollaborationServer )
   {
      m_pCollaborationServer->CanPublish( &bCanPublish );
   }

   pCmdUI->Enable( bCanPublish == VARIANT_TRUE );
}

void CMainFrame::OnCalibratePixelsPerInch()
{
   static CCTIA_Ruler dlg;
   if ( dlg.DoModal() == IDOK )
   {
      //
   }
}

void CMainFrame::OnFileSendMail()
{
   // ask the user which overlays he would like included in the mission
   // package that will be sent
   SelectOverlayDlg dlg(SelectOverlayDlg::SELECT_OVERLAY, TRUE, FALSE, TRUE, TRUE);

   dlg.set_title("Select overlays to package");
   dlg.set_label("Overlays");
   dlg.set_OK_label("Mail");

   if (dlg.DoModal() == IDOK)
   {
      CMissionPackageDlg package;
      CString package_name;
      CString file_name;

      char tmpPath[MAX_PATH + 1];
      if (::GetTempPath(MAX_PATH, tmpPath) == 0)
      {
         ERR_report("Failed obtaining temporary path via GetTempPath");
         return;
      }

      CString strTemporaryPath(tmpPath);
      if (strTemporaryPath[strTemporaryPath.GetLength() - 1] != '\\')
         strTemporaryPath += '\\';

#if GOV_RELEASE
      file_name = "fvw_mail.xmp";
#else
      file_name = "fvw_mail.fmp";
#endif

      package_name = CString(tmpPath) + file_name;

      // delete the temporary package in case it was left over from a previous operation
      remove(package_name);

      if (package.CreateNew(package_name) == FAILURE)
      {
         CString msg;
         msg.Format("Unable to create temporary mission package %s", package_name);
         ERR_report(msg);
         return;
      }

      int index = 0;
      while (1)
      {
         CString ovl_name(dlg.get_selection(index++));
         if (ovl_name != "")
         {
            // add the overlay to the mission package
            int count = 0;
            C_overlay *overlay = package.get_overlay_full_pathname(ovl_name);
            if (overlay == NULL)
               ERR_report("Unable to find overlay in list");
            else
            {
               CString strFileSpecification;
               OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
               if (pFvOverlayPersistence != NULL)
               {
                  _bstr_t fileSpecification;
                  pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
                  strFileSpecification = (char *)fileSpecification;
               }

               package.add_file_to_binder(strFileSpecification, -1, count);
            }
         }
         else
            break;
      }

      m_MAPI->send_mail(package_name, file_name);

      // delete the temporary package
      remove(package_name);
   }
}

void CMainFrame::OnUpdateFileSendMail(CCmdUI* pCmdUI)
{
   ASSERT(m_MAPI);

   bool bFoundSavedOverlay = false;

   // look to see that at least one file overlay that is named exists.  
   CList<C_overlay*, C_overlay*> overlayList;
   OVL_get_overlay_manager()->get_overlay_list(overlayList);

   POSITION position = overlayList.GetHeadPosition();
   while (position)
   {
      OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlayList.GetNext(position));
      if (pFvOverlayPersistence != NULL)
      {
         long bHasBeenSaved = 0;
         pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);
         if (bHasBeenSaved)
         {
            bFoundSavedOverlay = true;
            break;
         }
      }
   }

   // enable the Send... menu item if saved file exists and mail available
   pCmdUI->Enable(bFoundSavedOverlay && m_MAPI->is_mail_available());
}

int CMainFrame::ShowStatusBar(boolean_t show)
{
   for(int i=0;i<m_statusBarMgr->m_arrStatusBars.GetSize();++i)
      m_statusBarMgr->m_arrStatusBars[i]->ShowWindow(show);

   RecalcLayout();

   return SUCCESS;
}

void CMainFrame::OnViewStatusBar()
{
   for(int i=0;i<m_statusBarMgr->m_arrStatusBars.GetSize();++i)
      m_statusBarMgr->m_arrStatusBars[i]->ShowWindow((m_statusBarMgr->m_arrStatusBars[i]->GetStyle() & WS_VISIBLE) == 0);

   RecalcLayout();
}

void CMainFrame::OnUpdateViewStatusBar(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck((m_statusBarMgr->m_arrStatusBars[0]->GetStyle() & WS_VISIBLE) != 0);
}

CStatusBarInfoPane *CMainFrame::GetStatusBarInfoPane(CString strInfoPaneName)
{
   if (m_statusBarMgr->m_availableInfoPaneList)
      return m_statusBarMgr->m_availableInfoPaneList->GetInfoPane(strInfoPaneName);

   return nullptr;
}

void CMainFrame::OnViewMapTabsBar()
{
   if (m_pMapTabsBar)
   {
      int nCmdShow = (m_pMapTabsBar->GetStyle() & WS_VISIBLE) == 0;

      m_pMapTabsBar->ShowWindow(nCmdShow);
      RecalcLayout();

      if (nCmdShow)
      {
         m_pMapTabsBar->Invalidate();
      }
   }
}

void CMainFrame::OnUpdateViewMapTabsBar(CCmdUI* pCmdUI)
{
   if (m_pMapTabsBar)
      pCmdUI->SetCheck((m_pMapTabsBar->GetStyle() & WS_VISIBLE) != 0);
}

void CMainFrame::OnOptionsFormat()
{
   OnOptionsFormat(0);
}

void CMainFrame::OnOptionsFormat(int nInitialPage /* = 0 */)
{
   COptionsSheet optdlg("Coordinate Format");
   COptionsPri pridlg;
   COptionsSec secdlg;
   COptionsDis disdlg;
   CStatBarBullseyePropPage statBarRangeBearingPage;

   optdlg.m_psh.nStartPage = nInitialPage;

   // Removes the Apply button
   optdlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

   optdlg.AddPage(&disdlg);
   optdlg.AddPage(&pridlg);
   optdlg.AddPage(&secdlg);
   optdlg.AddPage(&statBarRangeBearingPage);

   const int nStatus = optdlg.DoModal();

   if (nStatus == IDOK)
   {
      // update default datum in the status bar
      static CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_PANE_DEFAULT_DATUM));
      if (pTextInfoPane != NULL)
      {
         const int DATUM_LEN = 6;
         char sdatum[DATUM_LEN];
         GEO_get_default_datum(sdatum, DATUM_LEN);
         pTextInfoPane->SetText(sdatum);
      }

      // invalidate the overlays (in particular the doghouses need to be drawn
      // to reflect the change in coordinate format)
      OVL_get_overlay_manager()->invalidate_all();
   }
   else if (nStatus == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_STAT_BAR_BULLSEYE;
   }
}

//
// ============================================================================
//

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
   // set minimum window track size
   lpMMI->ptMinTrackSize.x = 250;
   lpMMI->ptMinTrackSize.y = 250;

   CFrameWndEx::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::OnOptionsSecurity()
{
   CSecurDlg dlg;

   dlg.DoModal();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CFrameWndEx::OnActivate(nState, pWndOther, bMinimized);

   switch (nState)
   {
   case WA_CLICKACTIVE:
      {
         CPoint position;
         CRect rect;
         MapView *view = (MapView *)GetActiveView();

         // get cursor position in view coordinates
         GetCursorPos(&position);
         view->ScreenToClient(&position);

         // get view rect in view coordinates
         view->GetClientRect(&rect);

         // if the cursor is in the view then lose the mouse click and
         // invalidate the view
         if (PtInRect(&rect, position))
         {
            view->lose_mouse_click();
            OVL_get_overlay_manager()->invalidate_all();
            view->UpdateWindow();
         }
         break;
      }
   }
}

LRESULT CMainFrame::OnInsertPrintMenuItems(WPARAM wparam, LPARAM lparam)
{
   return SUCCESS;
}

//
// ============================================================================
//

LRESULT CMainFrame::OnRemovePrintMenuItems(WPARAM wparam, LPARAM lparam)
{
   return SUCCESS;
}

//
// ============================================================================
//

LRESULT CMainFrame::OnEnterSizeMove(WPARAM p1, LPARAM p2)
{
   
   //  Set the flag saying that the frame is in the middle of a size or move.
   //  Don't set the flag if the application is iconized because
   //  when you click on the icon, you get the WM_ENTERSIZEMOVE message, but
   //  you don't get a corresponding WM_EXITSIZEMOVE message unless you move
   //  the icon.
   if (!IsIconic())
      m_doing_size_or_move = TRUE;

   return 0;
}

//
// ============================================================================
//

LRESULT CMainFrame::OnExitSizeMove(WPARAM p1, LPARAM p2)
{
   if (!IsIconic())
   {
      m_doing_size_or_move = FALSE;

      // We always want to redraw the map after a move / resize operation.
      fvw_get_view()->Invalidate(TRUE);
   }

   return 0;
}

LRESULT CMainFrame::OnNITFUtilityThreadEvent(WPARAM p1, LPARAM p2)
{
   C_nitf_ovl* pNITFOverlay =
      reinterpret_cast< C_nitf_ovl* >( OVL_get_overlay_manager()->get_first_of_type( FVWID_Overlay_NitfFiles ) );
   if ( pNITFOverlay == NULL )
   {
      assert( FALSE && "NITF overlay should exist by now" );
      return 0;
   }
   return pNITFOverlay->OnNITFUtilityThreadEvent( p1, p2 );
}

//
// ============================================================================
//

void CMainFrame::OnEnterCHUMEditMode()
{
   // TODO?   Does this method need to be here?
   //CView*       view = NULL;
   //ViewMapProj* map  = NULL;

   //view = GetActiveView();
   //if (!view)
   //   return;

   //map = UTL_get_current_view_map(view);
   //if (!map)
   //   return;

   //C_chum_ovl::set_mode(map->scale(), C_chum_ovl::MM_CHUM_SELECT);
}

//
// ============================================================================
//

LRESULT CMainFrame::OnErrorReport(WPARAM wParam,LPARAM lParam)
{
   // if ui is hidden, don't tell the user what is going on
   bool show_messages = (fvw_get_app()->is_gui_hidden() == false);

   if (wParam == DISPLAY_ERROR_STRING)
   {
      CString *message = (CString *)lParam;
      if (message)
      {
         if (show_messages)
            AfxMessageBox(*message);

         delete message;
      }

      return SUCCESS;
   }

   if (wParam == ROUTE_SERVER_EXCEPTION)
   {
      if (show_messages)
      {
         CString msg;

         msg.Format("The Route Server has thrown an exception.  "
            "See %s for more details.  The Route Server may become unstable, so "
            "you should save changes to your route(s) as soon as possible.",
            (const char *)appErrorFile());
         AfxMessageBox(msg);
      }

      return SUCCESS;
   }

   if (wParam == ROUTE_SERVER_BUSY)
   {
      if (show_messages)
      {
         CString msg;

         msg.Format("The Route Server is busy, so you are unable to change "
            "the route at this time.  Please wait and try again.");
         AfxMessageBox(msg);
      }

      return SUCCESS;
   }


   if (wParam == ROUTE_SERVER_ERROR)
   {
      if (show_messages)
      {
         CString msg;

         msg.Format("The Route Server has indicated a severe error condition.  "
            "See %s for more details.  The Route Server may become unstable, so "
            "you should save changes to your route(s) as soon as possible.",
            (const char *)appErrorFile());
         AfxMessageBox(msg);
      }
      return SUCCESS;
   }

   // TODO?  ECHUM Error Handleing
   //if (wParam == ECHUM_FILE_NEEDS_PARSING)
   //{
   //   C_echum_ovl *echum_ovl = (C_echum_ovl *)lParam;

   //   // make sure a valid C_echum_ovl * is passed to this function
   //   if (!OVL_get_overlay_manager()->is_overlay_valid(echum_ovl))
   //   {
   //      ERR_report("This overlay is not in the overlay manager's list.");
   //      return FAILURE;
   //   }


   //   CView *view = GetActiveView();
   //   if (!view)
   //   {
   //      ERR_report("Couldn't call on_file_needs_parsing() due to a NULL CView.");
   //      return FAILURE;
   //   }
   //   else
   //   {
   //      ViewMapProj *map  = UTL_get_current_view_map(view);
   //      if (!map)
   //      {
   //         ERR_report("Couldn't call on_file_needs_parsing() due to a NULL map object.");
   //         return FAILURE;
   //      }
   //      // let the user parse the ECHUM files
   //      else if (echum_ovl->on_file_needs_parsing(map) != SUCCESS)
   //      {
   //         ERR_report("on_file_needs_parsing() failed.");
   //         return FAILURE;
   //      }
   //   }

   //   return SUCCESS;
   //}

   ERR_report("Unrecognized error in wParam.");
   return FAILURE;
}

LRESULT CMainFrame::OnCloseInformationDialog(WPARAM wParam,LPARAM lParam)
{
   CRemarkDisplay::close_dlg();
   return 0;
}

void CMainFrame::OnDataPaths()
{
   if (CAT_data_paths() != SUCCESS)
      ERR_report("CAT_data_paths() failed.");
}

//
// ============================================================================
//

void CMainFrame::OnDataCheck()
{
   CDataCheckDlg dlg;

   if (dlg.DoModal() != IDOK)
      return;

   boolean_t do_data_check = dlg.m_cov_file_check;
   boolean_t remove_masking = dlg.m_remove_masking;

   if ( remove_masking )
   {
      CFVMessageDlg dlg(this, "Deleting Terrain Masking Data "
         "online map data paths.  Please wait.", "Deleting Terrain Masking Data");
      CWaitCursor wait;

      // get time when message is displayed
      DWORD tick_count = GetTickCount();

      // MATT_FIX
      //C_threat_ovl::DeleteThreatMaskDirectory( AfxGetMainWnd()->m_hWnd );
      while ((GetTickCount() - tick_count) < 2000);
      //call to avoid warning that OnDestroy in derived class will not get called
      dlg.DestroyWindow();  
   }

   if (do_data_check)
   {
      CWaitCursor wait;

      if (MDSWrapper::GetInstance()->RegenerateAllSources() != SUCCESS)
      {
         ERR_report("CMainFrame::OnDataCheck(): MDSWrapper::RegenerateAllSources() failed.");
      }

      //
      // display warning that CADRG may not be current now (since coverage
      // generation may have uncovered previously unseen old CADRG data)
      //
      AfxMessageBox("You should now check the chart currency of your system\n"
         "with \"Tools->Data Administration->Chart Currency\"",
         MB_OK|MB_ICONINFORMATION);
   }
}

void CMainFrame::OnSystemHealthRibbonButton()
{
   ISystemHealthStatusStatePtr spSystemHealthStatusState = m_pSystemHealth->FindIndex(SystemHealthRibbonButton::GetSelectedRow());

   // We have to remove internal overlays here, since they have internal currency dialogs.
   GUID uidSystemHealth = spSystemHealthStatusState->UID;

   if ((uidSystemHealth == uidECRG_UPDATE_DISK) || (uidSystemHealth == uidCADRG_SUPPLEMENT_DISK) || (uidSystemHealth == uidCADRG_TLM_UPDATE_DISK) || (uidSystemHealth == uidECRG_CHARTS) || (uidSystemHealth == uidCADRG_CHARTS))
   {
      // NOTE:  System Health is updated by the Map Catalog / MDSWrapper.cpp when coverage is updated.
      // Startup MDM
      C_overlay* pOverlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MapDataManager);

      if (pOverlay == NULL)
      {
         OVL_get_overlay_manager()->toggle_editor(FVWID_Overlay_MapDataManager);
         pOverlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MapDataManager);
      }

      if (pOverlay != NULL)
      {
         // Switch to the correct tab.
         CoverageOverlay* pcovOverlay = (CoverageOverlay*) pOverlay;

         if ((uidSystemHealth == uidECRG_UPDATE_DISK) || (uidSystemHealth == uidCADRG_SUPPLEMENT_DISK) || (uidSystemHealth == uidCADRG_TLM_UPDATE_DISK))
         {
            pcovOverlay->SetActiveMDMSheet(CMDMSheet::cCHARTINDEX);
         }
         else if ((uidSystemHealth == uidECRG_CHARTS) || (uidSystemHealth == uidCADRG_CHARTS))
         {
            pcovOverlay->DisplayExpiredChartsOnly(true);
            pcovOverlay->SetActiveMDMSheet(CMDMSheet::cMULTIPLE);
         }
      }
   }
   else if (uidSystemHealth == uidVVOD)
   {
      // NOTE:  System Health is updated by the Map Catalog / MDSWrapper.cpp when coverage is updated.
      PostMessage(WM_COMMAND, MAKEWPARAM(ID_DATAADMINISTRATION_VVODIMPORTER, 0), NULL);
   }
   else
   {
      spSystemHealthStatusState->ShowCurrencyDialog();
   }
}

//
// ============================================================================
//

void CMainFrame::OnTimer(UINT nIDEvent)
{
   if (nIDEvent == FV_TOOL_TIP_TIME_OUT)
   {
      UTL_get_timer_list()->tick();
   }
   else if (nIDEvent == FV_MAP_TABS_UPDATE && m_pMapTabsBar)
   {
      m_pMapTabsBar->UpdateTabs();
   }

   CFrameWndEx::OnTimer(nIDEvent); //still want to call this? - kevin
}

//
// Interface to the DAFIF Selection Tool
//

void CMainFrame::select_dafif()
{
#ifdef GOV_RELEASE
   try
   {
      const GUID GUID_SelectDafif = 
      { 0x897401e1, 0x697e, 0x4435, { 0xa1, 0xd8, 0xbc, 0x41, 0x9b, 0x50, 0x1d, 0xc2 } };

      const GUID CLSID_SelectDafif =
      { 0x27bc9510, 0x510a, 0x41f9, { 0xa6, 0x00, 0x94, 0x01, 0xb8, 0xb8, 0x6b, 0x02 } };

      FalconViewOverlayLib::IFvToolMenuItemPtr spToolMenuItem;
      CO_CREATE(spToolMenuItem, CLSID_SelectDafif);

      spToolMenuItem->Initialize(GUID_SelectDafif);
      spToolMenuItem->MenuItemSelected();

      ISystemHealthStatusStatePtr spSystemHealthStatusState = m_pSystemHealth->FindUID(uidSystemHealthDAFIF);
      spSystemHealthStatusState->Invalidate();
   }
   catch(_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
#endif
}

static boolean_t on_close_received = FALSE;
void CMainFrame::OnClose()
{
   // If a user clicked on the system menu's close button and a close procedure is enabled
   // then call it
   if (m_lpfnCloseProc != NULL && !(*m_lpfnCloseProc)(this))
      return;

   // avoid processing OnClose twice
   if (on_close_received)
      return;

   on_close_received = TRUE;

   const int num_editors = OVL_get_type_descriptor_list()->GetNumEditors();

   // clean up COM editors.  This is so that the toolbars do not get stored in the registry in the call to SaveBarState
   OVL_get_type_descriptor_list()->TerminateEditors();

   // save status bar configuration
   CString strCurrentConfig = PRM_get_registry_string("Main", "USER_DATA", "") + "\\StatusBar\\CurrentStatusBarConfig.xml";
   SaveStatusBarConfig(strCurrentConfig);

   OVL_get_overlay_manager()->exit();  // destroy overlays

   OVL_get_type_descriptor_list()->ResetEnumerator();
   while (OVL_get_type_descriptor_list()->MoveNext())
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;

      if (pOverlayTypeDesc->pCustomInitializer != NULL)
         pOverlayTypeDesc->pCustomInitializer->TerminateAtShutdown();
   }

   // terminate custom initializers that are not associated with any overlay
   for (size_t i=0; i<m_customInitializers.size(); ++i)
   {
      CCustomInitializerCOM *pCustomInit = m_customInitializers[i];
      pCustomInit->TerminateAtShutdown();
      delete pCustomInit;
   }
   m_customInitializers.erase(m_customInitializers.begin(), 
      m_customInitializers.end());

   // stop and clean up the collaboration server
   if (m_pCollaborationServer )
   {
      m_pCollaborationServer->Stop();
      m_pCollaborationServer->Release();
      m_pCollaborationServer = nullptr;
   }

   // terminate rendering thread (blocks until termination complete)
   MapView *map_view = fvw_get_view();

   if (map_view != nullptr)
   {
      GeospatialViewController *geospatial_view_controller
         = map_view->GetGeospatialViewController();

      if (geospatial_view_controller != nullptr)
      {
         // Before terminating the rendering thread be sure to terminate the
         // operations thread. Otherwise, operations can get added
         GeospatialScene *geospatial_scene = geospatial_view_controller->GetGeospatialScene();

         if (geospatial_scene != nullptr)
            geospatial_scene->TerminateOperationsThread();

         GeospatialView *geospatial_view
            = geospatial_view_controller->GetGeospatialView();

         if (geospatial_view != nullptr)
            geospatial_view->TerminateRenderingThread();
      }
   }

   if (m_pMapTabsBar)
   {
      m_pMapTabsBar->DestroyWindow();
      m_pMapTabsBar = nullptr;
   }

   CFrameWndEx::OnClose();
}

void CMainFrame::OnOverlayOptions()
{
   OVL_get_overlay_manager()->overlay_options_dialog();
}

void CMainFrame::OnHideBackgroundOverlays()
{
   OVL_get_overlay_manager()->
      show_other_overlays(!OVL_get_overlay_manager()->all_overlays_visible());

   // invalidate the map (necessary for overlays that draw to the base map e.g, shadow)
   //
   MapView *pView = static_cast<MapView *>(GetActiveView());
   if (pView != NULL)
   {
      MapEngineCOM *pMapEngine = pView->get_map_engine();
      if (pMapEngine)
         pMapEngine->SetCurrentMapInvalid();
   }
}

void CMainFrame::OnUpdateHideBackgroundOverlays(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(!OVL_get_overlay_manager()->all_overlays_visible());
}

void CMainFrame::RebuildMapMenu()
{
   m_bIsMapMenuValid = false;

   m_map_menu->m_ribbon_btn->RemoveAllSubItems();

   HMENU hMapMenu = ::CreateMenu();
   build_map_menu(CMenu::FromHandle(hMapMenu));
   m_map_menu->m_ribbon_btn->SetMenu(hMapMenu);
   ::DestroyMenu(hMapMenu);
}

// build the Map menu based on currently available map types
void CMainFrame::build_map_menu(CMenu *map_menu)
{
   // the map menu is only recomputed if coverage changes or a data source online status changes
   if (m_bIsMapMenuValid)
      return;

   UtilMenu util_menu_map(map_menu);

   m_map_menu->m_map_menu_assoc.RemoveAll();

   for (size_t i=0; i<m_map_menu->m_mapMenuPopups.size(); i++)
      delete m_map_menu->m_mapMenuPopups[i];
   m_map_menu->m_mapMenuPopups.clear();

   try
   {
      int commandID = IDRANGE_MAP_MENU_001;
      IMapGroupNamesPtr spMapGroupNames;
      IMapGroupsPtr spMapGroups;

      CO_CREATE(spMapGroupNames, CLSID_MapGroupNames);
      CO_CREATE(spMapGroups, CLSID_MDSUtilMapGroups);

      HRESULT hrGroupNames = spMapGroupNames->SelectAll();
      while (hrGroupNames == S_OK)
      {
         if (spMapGroupNames->m_Disabled == TRUE)
         {
            hrGroupNames = spMapGroupNames->MoveNext();
            continue;
         }

         HRESULT hr = spMapGroups->SelectWithData(spMapGroupNames->m_Identity);
         while (hr == S_OK && spMapGroups->m_DoesDataExist)
         {
            MapMenuEntry entry;
            entry.m_source = MapSource(spMapGroups->m_ProductName);
            entry.m_scale = MapScale(spMapGroups->m_Scale, spMapGroups->m_ScaleUnits);
            entry.m_series = MapSeries(spMapGroups->m_SeriesName);
            entry.m_is_soft_scale = spMapGroupNames->m_IsSoftScale;
            entry.nGroupIdentity = spMapGroupNames->m_Identity;

            if(wcsicmp(spMapGroups->m_MapHandlerName, L"WMS") == 0)
            {
               //that ther is a web map

               // find an existing menu entry for the current group name
               CMenu *pGroupNameMenu = util_menu_map.get_submenu_by_name("Web Service");
               if (pGroupNameMenu == NULL)
               {
                  pGroupNameMenu = util_menu_map.append_submenu("Web Service");
                  m_map_menu->m_mapMenuPopups.push_back(pGroupNameMenu);
               }

               // find an existing menu entry for the current product name
               UtilMenu groupNameMenu(pGroupNameMenu);
               CMenu *pProductNameMenu = groupNameMenu.get_submenu_by_name(spMapGroups->m_ProductName);
               if (pProductNameMenu == NULL)
               {
                  pProductNameMenu = groupNameMenu.append_submenu(spMapGroups->m_ProductName);
                  m_map_menu->m_mapMenuPopups.push_back(pProductNameMenu);
               }
               //entry.m_source = MapSource(spMapGroups->m_SeriesName);
               // add the new menu item

               CString string;
               bool hasItem = false;
               for(int i = 0; i < pProductNameMenu->GetMenuItemCount(); i++){
                  pProductNameMenu->GetMenuString(i,string,MF_BYPOSITION);

                  if(strcmp(string,spMapGroups->m_SeriesName) == 0){
                     hasItem = true;
                     break;
                  }
               }

               if(!hasItem){
                  pProductNameMenu->AppendMenu(MF_ENABLED, commandID, spMapGroups->m_SeriesName);
                  m_map_menu->m_map_menu_assoc.SetAt(commandID, entry);
                  commandID++;
               }

            }
            else if (entry.m_is_soft_scale)
            {
               // Soft scales (e.g, vector or blank map) will have a single entry in the menu
               // that when selected will take the user to the closest map scale of that product
               //
               map_menu->AppendMenu(MF_BYPOSITION | MF_ENABLED, commandID, entry.ToString());
               m_map_menu->m_map_menu_assoc.SetAt(commandID, entry);
               commandID++;

               break;
            }
            // Otherwise, add a new menu to the menu: Group Name | Product Name | Scale/Series
            else if ( spMapGroupNames->m_GroupName.length() > 0 )
            {
               // find an existing menu entry for the current group name
               CMenu *pGroupNameMenu = util_menu_map.get_submenu_by_name(spMapGroupNames->m_GroupName);
               if (pGroupNameMenu == NULL)
               {
                  pGroupNameMenu = util_menu_map.append_submenu(spMapGroupNames->m_GroupName);
                  m_map_menu->m_mapMenuPopups.push_back(pGroupNameMenu);
               }

               // find an existing menu entry for the current product name
               UtilMenu groupNameMenu(pGroupNameMenu);
               CMenu *pProductNameMenu = groupNameMenu.get_submenu_by_name(spMapGroups->m_ProductName);
               if (pProductNameMenu == NULL)
               {
                  pProductNameMenu = groupNameMenu.append_submenu(spMapGroups->m_ProductName);
                  m_map_menu->m_mapMenuPopups.push_back(pProductNameMenu);
               }

               // add the new menu item
               pProductNameMenu->AppendMenu(MF_ENABLED, commandID, entry.ToString());
               m_map_menu->m_map_menu_assoc.SetAt(commandID, entry);
               commandID++;
            }

            hr = spMapGroups->MoveNext();
         }

         hrGroupNames = spMapGroupNames->MoveNext();
      }

      m_bIsMapMenuValid = true;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error building map menu : [%s]", (char *)e.Description());
      ERR_report(msg);
   }
}

CString MapMenuEntry::ToString()
{
   CString strMapMenuItem;

   if (m_is_soft_scale)
   {
      // the menu item user interface string for a soft scale is just the product name (source)
      strMapMenuItem = m_source.get_string();
   }
   else
   {
      // construct the user interface string for the map menu item: Scale/Series
      strMapMenuItem = m_scale.get_string();
      if (m_series.get_string().GetLength())
         strMapMenuItem += " (" + m_series.get_string() + ")";
   }

   /*
   CString hotkey_str = GetHotkeyString();
   if (hotkey_str.GetLength())
   {
   strMapMenuItem += "\t";
   strMapMenuItem += hotkey_str;
   }
   */

   return strMapMenuItem;
}

// For a given map type, return a string for the hotkey.  For example, Tiros World map is F4.
// If no hotkey is associated, then this method returns an empty string
CString MapMenuEntry::GetHotkeyString()
{
   // user defined groups do not have associated hot-keys
   if (nGroupIdentity >= MIN_USER_DEFINED_GROUP_IDENTITY)
      return "";

   if (m_is_soft_scale && m_source == "Blank")
      return "F2";

   typedef struct _hotkey_entry_t_
   {
      _hotkey_entry_t_(MapSource src, MapScale scl, MapSeries srs, CString hkey)
      {
         source = src;
         scale = scl;
         series = srs;
         strHotkey = hkey;
      }

      MapSource source;
      MapScale scale;
      MapSeries series;
      CString strHotkey;

   } hotkey_entry_t;

   hotkey_entry_t hotkey_table[] = {
      hotkey_entry_t(TIROS, WORLD, "*", "F4"),              // '*' handles any series name
      hotkey_entry_t("Ecrg", ONE_TO_5M, GNC_SERIES, "F5"),
      hotkey_entry_t("Ecrg", ONE_TO_2M, JNC_SERIES, "F6"),
      hotkey_entry_t("Ecrg", ONE_TO_1M, ONC_SERIES, "F7"),
      hotkey_entry_t("Ecrg", ONE_TO_500K, TPC_SERIES, "F8"),
      hotkey_entry_t("Ecrg", ONE_TO_250K, JOG_SERIES, "F9"),
      hotkey_entry_t(MapSource("DTED"), MapScale(3.0, MapScale::ARC_SECONDS), "Level 1", "F12")
   };

   const int size = sizeof(hotkey_table) / sizeof(hotkey_entry_t);
   for(int i=0;i<size;++i)
   {
      if (hotkey_table[i].source == m_source &&
         hotkey_table[i].scale == m_scale &&
         (hotkey_table[i].series == m_series || hotkey_table[i].series == "*") )
      {
         return hotkey_table[i].strHotkey;
      }
   }

   return "";
};

void CMainFrame::UpdateFavoritesMenu()
{
   m_favorites_menu->RemoveAllSubItems();

   HMENU hFavoritesMenu = ::CreateMenu();
   FavoriteList *fav_list = FavoriteList::get_instance();
   fav_list->init_menu(CMenu::FromHandle(hFavoritesMenu));
   m_favorites_menu->SetMenu(hFavoritesMenu);
   ::DestroyMenu(hFavoritesMenu);
}

void CMainFrame::OnFavoriteList(UINT nID)
{
   goto_favorite(nID - IDRANGE_FAVORITE_MENU_ITEM_1);
}

void CMainFrame::goto_favorite(int index)
{
   FavoriteList *fav_list = FavoriteList::get_instance();
   MapSpec new_map_spec;
   CList <GUID, GUID> overlayDescGuidList;
   CList <CString, CString> file_spec_lst;

   // change the map location/parameters to the values stored in the favorite
   FavoriteData favorite;
   CString favorite_filename = fav_list->GetFavoriteFilename(index);
   if (fav_list->LoadData(favorite_filename, favorite) == SUCCESS)
      GotoFavorite(&favorite);
}

/* static */
void CMainFrame::GotoFavorite(FavoriteData* favorite)
{
   if (favorite->map_type.get_source() != "")
   {
      MapView *map_view = static_cast<MapView *>(
         UTL_get_active_non_printing_view());

      int status;
      if (!map_view->IsProjectionSupported(favorite->map_proj_params.type))
      {
         AfxMessageBox("The chosen favorite contains an unsupported "
            "map projection type.");
         return;
      }
      else
      {
         status = map_view->ChangeMapType(favorite->map_group_identity,
            favorite->map_type, favorite->map_proj_params);
      }

      // put up meaninigful dialog and leave dialog box up
      if (status != SUCCESS)
      {
         map_view->OnMapError(status);
         return;
      }
      if (map_view->GetProjectionType() ==
         GENERAL_PERSPECTIVE_PROJECTION)
      {
         map_view->GetGeospatialViewController()->
            GetInputDevice()->frame_update_wait = true;
      }

      OVL_get_overlay_manager()->invalidate_all();

      // go ahead and invalidate now so the map gets updated
      map_view->invalidate_view(TRUE);
      map_view->UpdateWindow();
   }

   // if any overlays were stored along with the favorites, then open them
   // up now.  Any existing opened overlays will remain opened.
   OVL_get_overlay_manager()->OpenOverlays(favorite->overlay_types,
      favorite->file_specifications);
}

void CMainFrame::OnUpdateFavoriteList(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(pCmdUI->m_nID != IDRANGE_FAVORITE_MENU_ITEM_EMPTY);
}

LRESULT CMainFrame::OnFavoritesDirectoryChanged(WPARAM, LPARAM)
{
   FavoriteList::get_instance()->OnFavoritesChanged();

   if (AddFavoriteDlg::ms_hWnd != NULL)
      ::PostMessage(AddFavoriteDlg::ms_hWnd, WM_FAVORITES_DIRECTORY_CHANGED, 0, 0);

   if (OrgFavoriteDlg::ms_hWnd != NULL)
      ::PostMessage(OrgFavoriteDlg::ms_hWnd, WM_FAVORITES_DIRECTORY_CHANGED, 0, 0);

   return 0;
}

LPARAM CMainFrame::OnViewTimeChanged(WPARAM wParam, LPARAM lParam)
{
   COleDateTime dt((DATE)CViewTime::m_signal_date);
   OVL_get_overlay_manager()->set_current_view_time(dt);

   return SUCCESS;
}

LPARAM CMainFrame::OnUpdateTitleBar(WPARAM wParam, LPARAM lParam)
{
   FVW_update_title_bar();
   return SUCCESS;
}

BOOL CMainFrame::OnCopyData( CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct )
{
   if (pCopyDataStruct->lpData != NULL)
      CommandLineOverlayOpen(reinterpret_cast<char *>(pCopyDataStruct->lpData));

   return 0;
}

LPARAM CMainFrame::OnMsgAsyncFix(WPARAM wParam, LPARAM lParam)
{
   if (lParam != NULL)
      CommandLineOverlayOpen(reinterpret_cast<char *>(lParam));

   return 0;
}

void CMainFrame::CommandLineOverlayOpen(CString overlay_name)
{
   CString ext = overlay_name.Mid(overlay_name.ReverseFind('.'));

   // Load the mission binder with the specified name
   if ((ext.CompareNoCase(".xmp") == 0) || (ext.CompareNoCase(".fmp") == 0))
   {
      const BOOL create_new = FALSE;
      open_binder(overlay_name, create_new);
      return;
   }

   // Open favorite with specified name
   if (ext.CompareNoCase(".fvx") == 0)
   {
      FavoriteList *fav_list = FavoriteList::get_instance();

      // change the map location/paramaters to the values stored in the favorite
      FavoriteData favorite;
      if (fav_list->LoadData(overlay_name, favorite) == SUCCESS)
         GotoFavorite(&favorite);

      return;
   }

   // If this is a collaboration file, let the collaboration server
   // handle it.
   if (ext.CompareNoCase(".fvc") == 0 )
   {
      if ( m_pCollaborationServer )
      {
         LONG hOverlay = -1;
         _bstr_t bstrUNCPath;
         ASSERT( m_pCollaborationServer );
         VARIANT_BOOL bReadOnly = VARIANT_FALSE;
         VARIANT_BOOL bCreated = VARIANT_FALSE;
         HRESULT hr = m_pCollaborationServer->CreateFromFile( _bstr_t( overlay_name ), &hOverlay, bstrUNCPath.GetAddress(), &bReadOnly, &bCreated );
         if ( hOverlay != -1 )
         {
            OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(OVL_get_overlay_manager()->lookup_overlay( hOverlay ));
            if ( pFvOverlayPersistence && bReadOnly == VARIANT_TRUE )
               pFvOverlayPersistence->put_m_bIsReadOnly(TRUE);
         }

         return;
      }
   }

   // Check for NITF imagery file
   ext.MakeLower();
#ifdef JPEG2000_SUPPORT
   if ( NULL != strstr( ".ntf,.nitf,.tiff,.pdf,.sid,.jp2", ext )
#else
   if ( NULL != strstr( ".ntf,.nitf,.tiff,.pdf,.sid", ext )
#endif
#ifdef JPIP_SUPPORT
      || overlay_name.Left( 5 ).CompareNoCase( "jpip:" ) == 0
#endif
      )
   {
      C_overlay* pOvl = OVL_get_overlay_manager()->get_first_of_type( FVWID_Overlay_NitfFiles );
      if ( pOvl == NULL )
      {
         OVL_get_overlay_manager()->toggle_static_overlay( FVWID_Overlay_NitfFiles );
         pOvl = OVL_get_overlay_manager()->get_first_of_type( FVWID_Overlay_NitfFiles );
      }
      C_nitf_ovl* pNITFOvl = dynamic_cast< C_nitf_ovl* >( pOvl );
      assert( pNITFOvl != NULL );
      if ( pNITFOvl != NULL )
         pNITFOvl->open( overlay_name );
   }

   // Routes and threats cannot be loaded when in this message handler.  Need to post another message
   // to ourselves to avoid the warning - An outgoing call cannot be made since the application is
   // dispatching an input-synchronous call
   if (ext.CompareNoCase(".input-synchronous-fix") == 0)
   {
      overlay_name = overlay_name.Left(overlay_name.ReverseFind('.'));
      ext = overlay_name.Right(overlay_name.GetLength() - overlay_name.ReverseFind('.'));
   }
   else {
      overlay_name += ".input-synchronous-fix"; // this can add 22 characters to the file name

      static char overlayName[MAX_PATH + 22];
      strcpy_s(overlayName, MAX_PATH + 22, overlay_name);

      PostMessage(msg_async_fix, 0, reinterpret_cast<LPARAM>(overlayName));
      return;
   }

   OverlayTypeDescriptor *pOverlayTypeDesc = overlay_type_util::GetOverlayTypeFromFileExtension(ext);
   if (pOverlayTypeDesc != NULL)
   {
      C_overlay *ret_overlay;
      OVL_get_overlay_manager()->OpenFileOverlay(pOverlayTypeDesc->overlayDescriptorGuid, overlay_name, ret_overlay);
   }
}

//
// ============================================================================
//

// Handle the "Show all overlays" command from the Overlay menu.
// We need to do two things: (1) make sure that that hide background option
// is turned off, and (2) togle on every hidden overlay.
void CMainFrame::OnOverlayShowalloverlays()
{
   OVL_get_overlay_manager()->show_all_overlays();
}


//
// ============================================================================
//

// Handlers for the projection submenu off the Maps menu
//
void CMainFrame::OnMapProjection(UINT nID)
{
   int index = nID - ID_MAP_PROJECTION_01;
   MapView *pView = (MapView *) GetActiveView();
   pView->ChangeProjectionType(static_cast<ProjectionEnum>(index));
}

void CMainFrame::OnUpdateMapProjection(CCmdUI* pCmdUI)
{
   // IDC_MAP_PROJECTION is the button on the ribbon that contains the menu
   if (pCmdUI->m_nID == IDC_MAP_PROJECTION)
   {
      pCmdUI->Enable();
      return;
   }

   int index = pCmdUI->m_nID - ID_MAP_PROJECTION_01;
   MapView *pView = static_cast<MapView *>(GetActiveView());

   if ( pView )
   {
      if ( pView->curr_map_projection_ui_disabled() )
      {
         pCmdUI->Enable(FALSE);
      }
      else
      {
         int index = pCmdUI->m_nID - ID_MAP_PROJECTION_01;

         // Place a radio button next to the current map projection
         pCmdUI->SetRadio(index == pView->GetProjectionType());
         pCmdUI->Enable(pView->IsProjectionSupported(
            static_cast<ProjectionEnum>(index)));
      }
   }
}

// Handlers for Map menu entries
//

void CMainFrame::OnMapMenuEntry(UINT nId)
{
   if (nId < IDRANGE_MAP_MENU_001 || nId > IDRANGE_MAP_MENU_500)
      return;

   MapView *pView = static_cast<MapView *>(GetActiveView());

   MapMenuEntry entry;
   m_map_menu->m_map_menu_assoc.Lookup(nId, entry);

   int status;
   if (entry.m_is_soft_scale)
   {
      status = pView->ChangeToClosestScale(entry.nGroupIdentity);
   }
   else
   {
      status = pView->ChangeMapType(entry.nGroupIdentity,
         MapType(entry.m_source, entry.m_scale, entry.m_series));
   }

   if (status == SUCCESS)
      pView->invalidate_view();
   else
      pView->OnMapError(status);
}

void CMainFrame::OnUpdateMapMenuEntry(CCmdUI* pCmdUI)
{
   int nId = pCmdUI->m_nID;

   if (nId < IDRANGE_MAP_MENU_001 || nId > IDRANGE_MAP_MENU_500)
      return;

   if (nId < IDRANGE_MAP_MENU_001 || nId > IDRANGE_MAP_MENU_500)
      return;

   MapView* map_view = static_cast<MapView *>(GetActiveView());
   if (map_view == nullptr)
      return;

   long map_group_identity = map_view->GetMapGroupIdentity();
   const MapType& map_type = map_view->GetMapType();

   MapMenuEntry entry;
   m_map_menu->m_map_menu_assoc.Lookup(nId, entry);

   if (entry.m_is_soft_scale)
   {
      pCmdUI->SetRadio(entry.m_source == map_type.get_source() &&
         entry.nGroupIdentity == map_group_identity);
   }
   else
   {
      MapSource source = map_type.get_source();
      MapScale scale = map_type.get_scale();
      MapSeries series = map_type.get_series();

      pCmdUI->SetRadio(entry.m_source == map_type.get_source() &&
         entry.m_scale == map_type.get_scale() &&
         entry.m_series == map_type.get_series() &&
         entry.nGroupIdentity == map_group_identity);
   }
}

void CMainFrame::OnToolMenuItemCOM(UINT nId)
{
   std::map<UINT, CFvToolMenuItemCOM *>::iterator it = 
      m_toolMenuItems.find(nId);

   if (it != m_toolMenuItems.end())
      it->second->MenuItemSelected();
}

void CMainFrame::OnUpdateToolMenuItemCOM(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void CMainFrame::OnZoomCombo()
{
   ZoomPercentRibbonComboBox* zoom_percent_combo = DYNAMIC_DOWNCAST(
      ZoomPercentRibbonComboBox, m_wndRibbonBar.FindByID(ID_ZOOM_COMBO));

   int zoom_percent;
   if (zoom_percent_combo->GetAndValidateZoomPercent(&zoom_percent))
   {
      MapView *map_view = static_cast<MapView *>(GetActiveView());
      if (map_view)
      {
         // If ChangeZoomPercentage is successfull it will signal the
         // zoom combo box of the change. There is no need to call
         // SetZoomPercent here to update the text.
         if (map_view->ChangeZoomPercent(zoom_percent) != SUCCESS)
            zoom_percent_combo->RevertToLastValidZoomPercentage();
      }
   }
}

void CMainFrame::OnZoomIn()
{
   MapView *map_view = static_cast<MapView *>(GetActiveView());
   if (map_view == nullptr)
      return;

   int current_zoom = map_view->GetMapProjParams().zoom_percent;
   int new_zoom_percent;
   if (ui::GetNextLargestZoomPercent(current_zoom, &new_zoom_percent))
      map_view->ChangeZoomPercent(new_zoom_percent);
}

void CMainFrame::OnZoomInStep()
{
   MapView *map_view = static_cast<MapView *>(GetActiveView());
   if (map_view)
      map_view->ChangeZoomPercentByStep(ui::kZoomPercentageStepSize);
}

void CMainFrame::OnZoomOut()
{
   MapView *map_view = static_cast<MapView *>(GetActiveView());
   if (map_view == nullptr)
      return;

   int current_zoom = map_view->GetMapProjParams().zoom_percent;
   int new_zoom_percent;
   if (ui::GetNextSmallestZoomPercent(current_zoom, &new_zoom_percent))
      map_view->ChangeZoomPercent(new_zoom_percent);
}

void CMainFrame::OnZoomOutStep()
{
   MapView *map_view = static_cast<MapView *>(GetActiveView());
   if (map_view)
      map_view->ChangeZoomPercentByStep(-ui::kZoomPercentageStepSize);
}

void CMainFrame::OnUpdateZoom(CCmdUI* cmd_ui)
{
   MapView *map_view = static_cast<MapView *>(GetActiveView());
   if (map_view)
   {
      cmd_ui->Enable(
         MAP_zoom_percent_valid(map_view->GetMapType().get_scale()));
   }
}

// Handlers for the active Overlay list at the bottom of the Overlay menu
//
void CMainFrame::OnOverlayManagerViewList(UINT nID)
{
   OVL_get_overlay_manager()->handle_overlay_view_select(nID - IDRANGE_OVERLAY_VIEW_MENU_1);
}

void CMainFrame::OnUpdateOverlayManagerViewList(CCmdUI* pCmdUI)
{
   OVL_get_overlay_manager()->overlay_view_update(pCmdUI);
}

void CMainFrame::OnOverlayMenuItem(UINT nID)
{
   GUID& g = m_overlay_types_on_overlays_tab[nID - IDRANGE_OVERLAY_MENU_ITEM_1];
   OVL_get_overlay_manager()->toggle_static_overlay(g);
}

void CMainFrame::OnFileOverlayMenuItem(UINT nID)
{
   if (m_file_overlay_ribbon_btns_group)
   {
      FileOverlayMenuButton* p = dynamic_cast<FileOverlayMenuButton *>(
         m_file_overlay_ribbon_btns_group->GetButton(nID -
         IDRANGE_FILE_OVERLAY_MENU_ITEM_1));

      // The menu item should be a FileOverlayMenuButton. If not, then the
      // IDs may have been assigned incorrectly.
      ASSERT(p);

      if (p)
      {
         // If the menu item has an associated overlay, then close it
         C_overlay* overlay = p->GetOverlay();
         if (overlay)
         {
            OVL_get_overlay_manager()->close(overlay);
         }

         // Otherwise, this must be a pinned overlay
         else
         {
            C_overlay* ret_overlay;
            OVL_get_overlay_manager()->OpenFileOverlay(p->GetOverlayDesc(),
               p->GetToolTipText(), ret_overlay);
         }

         OVL_get_overlay_manager()->invalidate_all();
      }
   }
}

void CMainFrame::OnUpdateOverlayMenuItem(CCmdUI* pCmdUI)
{
   GUID& g = m_overlay_types_on_overlays_tab[pCmdUI->m_nID -
      IDRANGE_OVERLAY_MENU_ITEM_1];
   pCmdUI->SetCheck(OVL_get_overlay_manager()->get_first_of_type(g) != nullptr);
}

//
// ============================================================================
//

void CMainFrame::OnOverlayManager()
{
   OvlMgrDl ovl_mgr_dlg;
   ovl_mgr_dlg.DoModal();
}

void CMainFrame::OnToggleVvod()
{
   // {9162162B-DC09-4D0F-BB68-EACD91CABB1E}
   const GUID FVWID_Overlay_Vvod =
   { 0x9162162B, 0xdc09, 0x4d0f, { 0xbb, 0x68, 0xea, 0xcd, 0x91, 0xca, 0xbb, 0x1e } };

   OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_Vvod);
}

void CMainFrame::OnOptionsPrinting()
{
   CPrintOptDlg dlg;

   dlg.DoModal();
}

BOOL CMainFrame::OnQueryEndSession()
{
   if (!CFrameWndEx::OnQueryEndSession())
      return FALSE;

   // doesn't do anything currently

   return TRUE;
}

void CMainFrame::OnEndSession(BOOL bEnding)
{
   CFrameWndEx::OnEndSession(bEnding);

   // doesn't do anything currently
}

#define VVOD_LOG_FILE_NAME "\\..\\Logs\\VVODCurrency.txt"

void CMainFrame::OnVVodCurrency()
{
   std::string strLogFileName(PRM_get_registry_string(
      "Main", "ReadWriteAppData", ""));
   strLogFileName += VVOD_LOG_FILE_NAME;

   try
   {
      IMDSUtilPtr smpMDSUtil(__uuidof(MDSUtil));

      FILE *pLog = NULL;
      fopen_s(&pLog, strLogFileName.c_str(), "w");
      if (pLog == NULL)
      {
         CString msg;
         msg.Format("Unable to open %s for writing", strLogFileName.c_str());
         AfxMessageBox(msg);
         return;
      }

      fprintf(pLog, "%s", (char *)smpMDSUtil->GetVVodCurrencyReport());
      fclose(pLog);

      if (::ShellExecute(::GetDesktopWindow(), "open", strLogFileName.c_str(), NULL, NULL, SW_SHOW) <= (HINSTANCE) 32)
      {
         CString msg;
         msg.Format("Unable to automatically open the report file.\n"
            "Open %s with a text editor to view the report.",
            (const char*) strLogFileName.c_str());
         AfxMessageBox(msg);
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("VVOD currency report failed: %s", (char *)e.Description());
      ERR_report(msg);
   }
}

void CMainFrame::OnVVodImporter()
{
   CVVODCurrencyDlg dlg;
   dlg.DoModal();
}

void CMainFrame::OnFLIPCurrencyUpdate()
{
   ISystemHealthStatusStatePtr spFLIPStatusState = m_pSystemHealth->FindUID(uidFLIP);
   spFLIPStatusState->Invalidate();
}

void CMainFrame::OnSystemHealth()
{
}

void CMainFrame::OnCustomToolsManager()
{
   try
   {
      ICustomToolbarMgrPtr dlg(__uuidof(CustomToolbarMgr));
      CCustomToolbarMgrEventSink event_sink;

      if (dlg == NULL)
      {
         ERR_report("Unable to create custom tools dialog object");
         return;
      }

      IConnectionPoint *connection_point;
      IConnectionPointContainerPtr con_pt_container = dlg;
      con_pt_container->FindConnectionPoint(__uuidof(_ICustomToolbarMgrEvents) ,&connection_point);

      LPDISPATCH pEventSink = event_sink.GetIDispatch(FALSE);
      DWORD cookie;
      connection_point->Advise(pEventSink, &cookie);

      if (dlg->DoModal("Custom Tools") == IDOK)
      {
         // Remove everything after the custom tools manager button
         while (m_pCustomToolsPanel->GetCount() > 1)
            m_pCustomToolsPanel->Remove(1);

         m_custom_toolbar->AddCustomToolsToPanel(m_pCustomToolsPanel);
      }

      connection_point->Unadvise(cookie);
      connection_point->Release();
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
}

// pressed one of a range of custom toolbar buttons
void CMainFrame::OnCustomTB(UINT nID)
{
   m_custom_toolbar->OnCustomTB(nID);
}

void CMainFrame::OnUpdateCustomTB(CCmdUI* pCmdUI)
{
   m_custom_toolbar->OnUpdateButton(pCmdUI);
}

void CMainFrame::OnOverlayEditor(UINT nID)
{
   OVL_get_overlay_manager()->toggle_editor_by_command_id(nID);
}

void CMainFrame::OnUpdateOverlayEditor(CCmdUI* pCmdUI)
{
   OVL_get_overlay_manager()->OnUpdateOverlayEditor(pCmdUI);
}


void CMainFrame::OnRibbonElement(UINT nID)
{
   CArray<CMFCRibbonBaseElement *, CMFCRibbonBaseElement *> arButtons;
   m_wndRibbonBar.GetElementsByID(nID, arButtons);

   INT_PTR size = arButtons.GetSize();
   for (INT_PTR i=0; i<size; ++i)
   {
      CMFCRibbonBaseElement* pCurrent = arButtons[i];
      CRibbonElementImpl* pRibbonElement =
         reinterpret_cast<CRibbonElementImpl *>(pCurrent->GetData());

      TRACE( _T("OnRibbonElement( %d )\n"), nID );

   }
}


void CMainFrame::OnClickedRibbonElement(UINT nID)
{
   CArray<CMFCRibbonBaseElement *, CMFCRibbonBaseElement *> arButtons;
   m_wndRibbonBar.GetElementsByID(nID, arButtons);

   INT_PTR size = arButtons.GetSize();
   for (INT_PTR i=0; i<size; ++i)
   {
      CMFCRibbonBaseElement* pCurrent = arButtons[i];
      CRibbonElementImpl* pRibbonElement =
         reinterpret_cast<CRibbonElementImpl *>(pCurrent->GetData());

      if ( pRibbonElement != nullptr )
      {
         pRibbonElement->OnClicked(nID);

         // NOTE:  If the item is a custom added button (using ID button range) and the button is added to the 
         //        quick access ribbon bar by the user, GetElementsByID will include the 
         //        button twice in the array.  We only want a single notification for a command ID.
         break; // for
      }
   }
}


void CMainFrame::OnUpdateRibbonElement(CCmdUI* pCmdUI)
{
   CArray<CMFCRibbonBaseElement *, CMFCRibbonBaseElement *> arButtons;
   m_wndRibbonBar.GetElementsByID(pCmdUI->m_nID, arButtons);

   INT_PTR size = arButtons.GetSize();
   for (INT_PTR i=0; i<size; ++i)
   {
      CMFCRibbonBaseElement* pCurrent = arButtons[i];
      CRibbonElementImpl* pRibbonElement =
         reinterpret_cast<CRibbonElementImpl *>(pCurrent->GetData());
      if ( pRibbonElement != nullptr )
      {
         pCmdUI->Enable(pRibbonElement->GetEnabled());
         pCmdUI->SetCheck(pRibbonElement->GetChecked());
      }
   }
}

void CMainFrame::OnPhotoGeotaggingTool()
{
   CPhotoGeotaggingToolsDialog dlg;
   dlg.DoModal();
}

void CMainFrame::OnTargetGraphicPrinting()
{
   MapView *pView = static_cast<MapView *>(GetActiveView());
   if (pView != NULL)
   {
      d_geo_t cursor_pos;
      if (pView->GetCursorPositionGeo(&cursor_pos))
      {
         CList<target_t, target_t> list;
         target_t target;

         target.lat = cursor_pos.lat;
         target.lon = cursor_pos.lon;
         list.AddHead(target);

         CTargetGraphicsDlg dlg;
         dlg.set_selected_list(&list);
         dlg.DoModal();
      }
   }
}

//
// ==========================================================================================
// ==========================================================================================
//
// Methods used to create the splitter window and create the Map and
// Vertical View views.
//
//

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
   // create a splitter window with 1 column and 2 rows ...
   return m_wndSplitter.Create(this,
      2, 1,  // number of rows, columns
      CSize(1, 1),  // minimum pane size
      pContext,
      WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT,
      AFX_IDW_PANE_FIRST);
}

BOOL CMainFrame::ToggleVerticalView(UINT)
{
   // the Vertical Display is currently closed, so open it...
   if (m_wndSplitter.GetRowCount() == 1) 
   {
      // set the member variable to the last Vertical Display view height value
      // from the registry...
      m_nVerticalDisplayHeight = PRM_get_registry_int("VerticalDisplay", 
         "LastViewHeight", 225);

      // the CVerticalDisplayView pane is not currently displayed, so split
      // the Map view into two rows and display it in the new row...
      m_wndSplitter.SplitView(RUNTIME_CLASS(CVerticalViewDisplay), 
         m_nVerticalDisplayHeight);
   }
   else  // the Vertical Display is currently open, so close it...
   {
      // the CVerticalDisplayView pane is currently displayed, so delete it's 
      // row...
      m_wndSplitter.DeleteRow(1);
   }

   // set the registry flag to indicate that the Vertical Display is open...
   PRM_set_registry_int("VerticalDisplay", "VerticalDisplayOpen", 
      m_wndSplitter.GetRowCount() == 2);

   return TRUE;
}

void CMainFrame::OnUpdateToggleVerticalView(CCmdUI *pCmd)
{
   // Disable the Vertical View checkbox when in 3D
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
   {
      pCmd->Enable(map_view->GetProjectionType() !=
         GENERAL_PERSPECTIVE_PROJECTION);
   }

   // if the splitter window currently has more than one row, display a checkmark 
   // next to the View->Vertical View menu item... otherwise, remove the checkmark.
   pCmd->SetCheck(m_wndSplitter.GetRowCount() > 1);
}

void CMainFrame::OnFileOverlayList()
{
   BOOL show_file_overlay_list = m_file_overlay_list_cb == nullptr;

   // save the state in the registry
   PRM_set_registry_int("View", "FileOverlayListOn", show_file_overlay_list);

   SetupRibbonBarTabsRow(show_file_overlay_list);
}

void CMainFrame::OnUpdateFileOverlayList(CCmdUI* pCmd)
{
   pCmd->SetCheck(m_file_overlay_list_cb != nullptr);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
   CFrameWndEx::OnSize(nType, cx, cy);
}


void CDynamicSplitterWnd::SplitView(CRuntimeClass* pRuntimeClass, int nVerticalDisplayHeight)
{
   int nCurrentFvwWindowHeight, nNewMapWindowHeight;

   m_pDynamicViewClass = pRuntimeClass;

   RECT rect;
   GetClientRect(&rect);

   nCurrentFvwWindowHeight = abs(rect.bottom - rect.top);

   // If the current Falconview window height is not greater than the stored Vertical Display window
   // height (plus the 9 pixel thickness of the splitter window divider) plus the 30 pixel minimum
   // allowable Map window height, set the new Map window height equal to the 30 pixel minimum...
   if (nCurrentFvwWindowHeight <= nVerticalDisplayHeight + 9 + MINIMUM_ALLOWABLE_MAP_WINDOW_HEIGHT)
   {
      nNewMapWindowHeight = MINIMUM_ALLOWABLE_MAP_WINDOW_HEIGHT;
   }
   else
   {
      // In order to maintain the same Vertical Display (VD) window height that we had 
      // when the VD was last toggled off, we need to add the height of the splitter 
      // window's horizontal divider (9 pixels) to the last height of the VD window 
      // (nVerticalDisplayHeight). This value is then subtracted from the current total 
      // height of the Fvw client window (rect.bottom - rect.top) to get the new height 
      // of the top splitter row (the Map window) that is passed in to SplitRow() below.
      // In this way the resulting VD window height will be exactly the same as it
      // was when the VD was last toggled off...
      nNewMapWindowHeight = nCurrentFvwWindowHeight - (nVerticalDisplayHeight + 9);
   }

   // The integer height passed in to SplitRow() will be the new Map window height required
   // to maintain the same VD window height as before...
   SplitRow(nNewMapWindowHeight);
}


void CDynamicSplitterWnd::DeleteRow(int rowDelete)
{
   // delete ONLY the bottom (Vertical Display) row, which is row 1...
   if (rowDelete == 1)
      CSplitterWnd::DeleteRow(rowDelete);
}



//////////////////////////////////////////////////////////////////////////////
///////////// These routines support the "modal" child process ///////////////
/////////////      See 'CCustomTBClass' for more details       ///////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// This is called when we are activated. If we still have a child running,
// deflect the activation to him.
//////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnActivateApp(BOOL bActive, DWORD hTask)
{
   if ( InModalState() && m_custom_toolbar )
      m_custom_toolbar->activate_child();
   else
      CFrameWndEx::OnActivateApp(bActive, hTask);
}

//////////////////////////////////////////////////////////////////////////////
// If the user clicks a mouse button in our modal state (child active) then
// activate the child.
//////////////////////////////////////////////////////////////////////////////

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   if ( InModalState() )
      if ( message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN )
         m_custom_toolbar->activate_child();

   return CFrameWndEx::OnSetCursor(pWnd, nHitTest, message);
}

//////////////////////////////////////////////////////////////////////////////
// Before doing standard help stuff, see if the custom toolbar wants to handle
// this topic
//////////////////////////////////////////////////////////////////////////////

void CMainFrame::WinHelp(DWORD dwData, UINT nCmd)
{
   if (nCmd == HELP_CONTEXT)
   {
      int nCmdId = dwData - 0x10000;

      // Determine if the mouse if over a ribbon control
      POINT cursor_pos;
      if (::GetCursorPos(&cursor_pos))
      {
         m_wndRibbonBar.ScreenToClient(&cursor_pos);
         CMFCRibbonBaseElement* element = m_wndRibbonBar.HitTest(cursor_pos,
            TRUE);
         if (element)
            nCmdId = element->GetID();
      }

      if (IDRANGE_TOOL_MENU_ITEM_COM_00 <= nCmdId &&
         nCmdId <= IDRANGE_TOOL_MENU_ITEM_COM_24)
      {
         if (m_toolMenuItems.find(nCmdId) != m_toolMenuItems.end())
         {
            m_toolMenuItems[nCmdId]->LaunchHelp();
            return;
         }
      }

      if (OVL_get_overlay_manager()->OverrideHelp(nCmdId, nCmd))
         return;

      if (nCmdId >= IDRANGE_OVERLAY_MENU_ITEM_1 &&
         nCmdId < IDRANGE_OVERLAY_MENU_ITEM_LAST)
      { 
         auto type_desc = OVL_get_type_descriptor_list()->
            GetOverlayTypeDescriptor(m_overlay_types_on_overlays_tab[nCmdId -
            IDRANGE_OVERLAY_MENU_ITEM_1]);

         if (type_desc != NULL)
         {
            LaunchHtmlHelp(type_desc->contextSensitiveHelp.overlayHelpId,
               HELP_CONTEXT, type_desc->contextSensitiveHelp.helpFileName,
               type_desc->contextSensitiveHelp.overlayHelpURI);

            return;
         }         
      }

      // MFC standard commands fall in the range 0xE000 to 0xEFFF and will be
      // offset by 0xFFF. FalconView command IDs will be offset by 10000;
      UINT helpuri_command_offset = (nCmdId >= 0xE000 && nCmdId <= 0xEFFF) ?
         0xFFF : 10000;

      // See if there is an associated HELPURI stored in the string table
      CString help_uri;
      if (help_uri.LoadString(nCmdId + helpuri_command_offset))
      {
         LaunchHtmlHelp(0, HELP_CONTEXT, "", help_uri);
         return;
      }
   }

   // Want the MainFrame help to go to the URI.
   if (dwData == HID_BASE_RESOURCE+m_nIDHelp)
      dwData = 0;

   LaunchHtmlHelp(dwData, nCmd, "help\\fvw_core.chm", (dwData == 0) ? "fvw.chm" : "");
}

void CMainFrame::LaunchHtmlHelp(DWORD dwData, UINT nCmd, const char* pHelpFilename, const char *uriString)
{
   CWaitCursor wait;
   if (IsFrameWnd())
   {
      // CFrameWndEx windows should be allowed to exit help mode first
      ExitHelpMode();
   }

   // cancel any tracking modes
   SendMessage(WM_CANCELMODE);
   SendMessageToDescendants(WM_CANCELMODE, 0, 0, TRUE, TRUE);

   // need to use top level parent (for the case where m_hWnd is in DLL)
   CWnd* pWnd = GetTopLevelParent();
   pWnd->SendMessage(WM_CANCELMODE);
   pWnd->SendMessageToDescendants(WM_CANCELMODE, 0, 0, TRUE, TRUE);

   // attempt to cancel capture
   HWND hWndCapture = ::GetCapture();
   if (hWndCapture != NULL)
      ::SendMessage(hWndCapture, WM_CANCELMODE, 0, 0);

   CString result = hlp::LaunchHTMLHelp(pWnd->m_hWnd, dwData, nCmd, pHelpFilename, uriString).c_str();
   if (result.GetLength() > 0)
   {
      AfxMessageBox(result);
      ERR_report(result);
   }
}

void CMainFrame::OnViewtime()
{
   // open the view time dialog if it is not already opened
   if (!m_view_time_dlg_active)
   {
      // if the window has already been created successfully then we 
      // just need to show the hidden window and set the active flag
      m_view_time_dlg->ShowWindow(SW_SHOW);
      m_view_time_dlg_active = TRUE;
   }
   // otherwise, close the view time dialog
   else
   {
      m_view_time_dlg->ShowWindow(SW_HIDE);
      m_view_time_dlg_active = FALSE;
   }
}

void CMainFrame::OnUpdateViewtime(CCmdUI* pCmdUI)
{
   // set the push button if the view time dialog is active
   pCmdUI->SetCheck(m_view_time_dlg_active);
}

// add a toolbar to the frame -- used for client applications
// to develop custom toolbars
int CMainFrame::add_client_toolbar(CWnd* parent, CString bitmap_filename,
   int num_buttons_including_separators, IDispatch* pdisp,
   long client_hWnd, CString toolbar_name,
   short initial_position, long* separator_lst,
   int num_separators)
{
   CComObject<ClientToolbar>* toolbar;
   CComObject<ClientToolbar>::CreateInstance(&toolbar);
   if (toolbar->Initialize(bitmap_filename, num_buttons_including_separators,
      pdisp, toolbar_name, separator_lst, num_separators,
      m_next_client_toolbar_handle) == FAILURE)
   {
      return FAILURE;
   }

   m_client_toolbars[m_next_client_toolbar_handle] = toolbar;
   return m_next_client_toolbar_handle++;
}

// set the properties of a client-added toolbar button
int CMainFrame::setup_client_toolbar_button(int toolbar_handle, int button_number,
   CString tooltip_text, CString status_bar_text)
{
   auto it = m_client_toolbars.find(toolbar_handle);
   if (it != m_client_toolbars.end())
   {
      it->second->SetupButton(button_number, tooltip_text, status_bar_text);
      return SUCCESS;
   }

   return FAILURE;
}

// set the properties
int CMainFrame::set_client_toolbar_button_state(int toolbar_handle, int button_number,
   short button_down)
{
   auto it = m_client_toolbars.find(toolbar_handle);
   if (it != m_client_toolbars.end())
   {
      it->second->SetButtonState(button_number, button_down);
      return SUCCESS;
   }

   return FAILURE;
}

// remove the client-added toolbar with the given handle
int CMainFrame::remove_client_toolbar(int toolbar_handle)
{
   auto it = m_client_toolbars.find(toolbar_handle);
   if (it != m_client_toolbars.end())
   {
      try
      {
         it->second->RegisterNotifyEvents(false);
      }
      catch (_com_error& e)
      {
         REPORT_COM_ERROR(e);
      }
      m_client_toolbars.erase(it);
      RecalcLayout();
      return SUCCESS;
   }

   CString msg;
   msg.Format("Toolbar with given handle (%d) not found", toolbar_handle);
   ERR_report(msg);
   return FAILURE;
}

LRESULT CMainFrame::OnDeviceChange(WPARAM nEventType, LPARAM dwData)
{
   CWnd::OnDeviceChange(nEventType, dwData);

   // Have MDSWrapper check the CD-ROMs
   if (MDSWrapper::GetInstance()->DeviceChange(nEventType, dwData) != SUCCESS)
   {
      ERR_report("CMainFrame::OnDeviceChange(): MDSWrapper::DeviceChange() failed.");
   }

   return 0;
}

LRESULT CMainFrame::OnCoverageChanged(WPARAM wParam, LPARAM lParam)
{
   if (MDSWrapper::GetInstance()->m_bstrCovGenDSIds != _bstr_t(""))
   {
      // Allow the supplemental data handlers to process the data sources
      //
      CIdentitiesSet dataSourceIds(MDSWrapper::GetInstance()->m_bstrCovGenDSIds);
      CIdentitiesSet_ITR it = dataSourceIds.Begin();
      while (it != dataSourceIds.End())
      {
         CString strDataSourcePath;
         MDSWrapper::GetInstance()->GetDataSourcePath(&strDataSourcePath, *it);
         HandleSupplementalData(strDataSourcePath);
         it++;
      }

      MDSWrapper::GetInstance()->m_bstrCovGenDSIds = L"";
   }

   MapView *map_view = static_cast<MapView *>(GetActiveView());
   if (map_view != NULL)
   {
      MapEngineCOM *pMapEngine = map_view->get_map_engine();
      if (pMapEngine)
         pMapEngine->CoverageUpdated();
   }

   OVL_get_overlay_manager()->notify_clients_of_coverage_update();

   // update map tabs when coverage changes
   if (m_pMapTabsBar != nullptr &&
      (m_pMapTabsBar->GetStyle() & WS_VISIBLE) != 0 && map_view != nullptr)
   {
      m_pMapTabsBar->MapParamsChanged(
         MapParamsListener_Interface::MAP_TYPE_CHANGED,
         map_view->GetMapGroupIdentity(), map_view->GetMapType(),
         map_view->GetMapProjParams(), map_view->GetMapDisplayParams());
   }

   // the map menu is now invalid and needs to be rebuilt
   RebuildMapMenu();

   return TRUE;
}

void CMainFrame::OnStatusBarPane(UINT nID)
{
   // this handler is required by CMFCStatusBar to avoid disabling
   // the status bar panes
}

void CMainFrame::OnUpdateStatusBarPane(CCmdUI *pCmdUI)
{
   static CTextInfoPane *pCursorLocInfoPane = static_cast<CTextInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_PANE_CURSOR_LOCATION));
   static CTextInfoPane *pSecondaryInfoPane = static_cast<CTextInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_PANE_SECONDARY_CURSOR_LOCATION));
   static CTextInfoPane *pMapTypeInfoPane = static_cast<CTextInfoPane *>(GetStatusBarInfoPane(STATUS_BAR_PANE_MAP_TYPE));

   if (pCursorLocInfoPane && pCmdUI->m_nID == pCursorLocInfoPane->GetId())
      OnUpdateCoordPane(pCmdUI);

   if (pSecondaryInfoPane && pCmdUI->m_nID == pSecondaryInfoPane->GetId())
      OnUpdateCoordPane(pCmdUI);

   if (pMapTypeInfoPane && pCmdUI->m_nID == pMapTypeInfoPane->GetId())
      OnUpdateSourcePane(pCmdUI);
}

void CMainFrame::OnUpdateCoordPane(CCmdUI *pCmdUI)
{
   COLORREF bkcolor;

   if (m_precise_geo && !m_colored_coord)
      return;

   if (m_precise_geo)
   {
      m_colored_coord = FALSE;
      bkcolor = ::GetSysColor(COLOR_3DFACE);
   }
   else
   {
      m_colored_coord = TRUE;
      bkcolor = RGB(255, 255, 64);
   }

   pCmdUI->Enable(TRUE);
   CMFCStatusBar *psb = static_cast<CMFCStatusBar*>(pCmdUI->m_pOther);
   ASSERT_KINDOF(CMFCStatusBar, psb);
   ASSERT_VALID(psb);

   CString panetext;
   psb->GetPaneText(pCmdUI->m_nIndex, panetext);
   CRect rect;
   psb->GetItemRect(pCmdUI->m_nIndex,&rect);
   rect.DeflateRect(1,1);

   CClientDC dc(psb);
   CFont *pOldFont;
   pOldFont=dc.SelectObject(psb->GetFont());
   dc.SetBkColor(bkcolor);
   dc.FillSolidRect(rect,dc.GetBkColor());
   rect.left += 1;
   dc.DrawText(panetext, rect, DT_EXTERNALLEADING | DT_SINGLELINE | DT_VCENTER);
   dc.SelectObject(pOldFont);
}

void CMainFrame::OnUpdateSourcePane(CCmdUI *pCmdUI)
{
   if (m_precise_geo)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}

void CMainFrame::HandleSupplementalData(CString &strDataSourcePath)
{
   VARIANT_BOOL bDataHandled = VARIANT_FALSE;

   // Run for CSD Update.  This is internel, so has to be ran outside of the System Health control.
   CCsdDataHandler csd_handler;
   BOOL csdDataHandled = FALSE;
   csd_handler.HandleData(strDataSourcePath, csdDataHandled);

   if (csdDataHandled)
      bDataHandled = VARIANT_TRUE;

   m_pSystemHealth->DataSourceChangeEvents(_bstr_t(strDataSourcePath), &bDataHandled);
}

LRESULT CMainFrame::OnMovingMapPointAdded(WPARAM wParam, LPARAM lParam)
{
   int nOverlayHandle = static_cast<int>(wParam);
   C_gps_trail *pTrail = static_cast<C_gps_trail *>(OVL_get_overlay_manager()->lookup_overlay(nOverlayHandle));
   if (pTrail)
   {
      if (pTrail->add_point_from_queue() != SUCCESS)
      {
         ERR_report("add_point_from_queue failed");
      }
      pTrail->add_point();
   }

   return 0;
}

void CMainFrame::OnStatusBarSetup()
{
   if (m_pStatusBarSetupDialog == NULL)
   {
      m_pStatusBarSetupDialog = new CStatusBarSetupDialog();
      m_pStatusBarSetupDialog->Create(IDD_STATUS_BAR_SETUP, this);
   }
}


void CMainFrame::OnGraphicDrawingPrefs()
{
   if (m_pGraphicDrawingPrefsDialog == NULL)
   {
      // Create the Graphic Drawing Prefs dialog object...
      m_pGraphicDrawingPrefsDialog = new CElementRenderingDlg( this );

      // Create the Graphic Drawing Prefs dialog ...
      m_pGraphicDrawingPrefsDialog->Create(IDD_ELEMENT_DRAWING_OPT_DLG, this);
   }
}


void CMainFrame::OnVerticalDisplayProperties()
{
   if (m_pVerticalDisplayPropertiesDialog == NULL)
   {
      // create the Vertical Display Properties Dialog object...
      m_pVerticalDisplayPropertiesDialog = new CVerticalViewPropertiesDialog();

      // create the Vertical Display Properties Dialog...
      m_pVerticalDisplayPropertiesDialog->Create(IDD_VERTICAL_DISPLAY_PROPERTIES, this);
   }
}


void CMainFrame::AddStatusBarRow()
{
   const int nVisible = (m_pMapTabsBar->GetStyle() & WS_VISIBLE) == WS_VISIBLE;

   // first remove the map tabs control if it exists (it always need to be added
   // last to be on top of the status bar(s)
   {
      CWaitCursor wait;
      if (m_pMapTabsBar)
      {
         m_pMapTabsBar->DestroyWindow();
         m_pMapTabsBar = nullptr;
      }
   }

   CMapStatusBar *pStatusBar = new CMapStatusBar();
   if (!pStatusBar->Create(this, false))
   {
      ERR_report("Failed to create new status bar row.");
      return;
   }

   pStatusBar->CBasePane::SetPaneStyle(pStatusBar->CBasePane::GetPaneStyle() | CBRS_TOOLTIPS);

   m_statusBarMgr->m_arrStatusBars.Add(pStatusBar);

   CreateMapTabsBar();
   m_pMapTabsBar->ShowWindow(nVisible ? SW_SHOW : SW_HIDE);

   RecalcLayout();
   Invalidate();
}

void CMainFrame::RemoveStatusBarRow()
{
   const int nSize = m_statusBarMgr->m_arrStatusBars.GetSize();

   // needs to be at least one status bar remaining
   if (nSize == 1)
      return;

   delete m_statusBarMgr->m_arrStatusBars[nSize - 1];
   m_statusBarMgr->m_arrStatusBars.RemoveAt(nSize - 1);

   RecalcLayout();
   Invalidate();
}

void CMainFrame::CreateMapTabsBar(bool bUpdateNow /*= true*/)
{
   if (m_pMapTabsBar != NULL)
      return;

   m_pMapTabsBar = new CMapTabsBar;
   if (m_pMapTabsBar)
   {
      m_pMapTabsBar->Create(this, WS_CHILD|WS_VISIBLE|CBRS_BOTTOM,
         IDC_MAP_TABS_BAR);

      m_pMapTabsBar->Invalidate();

      MapView* map_view = static_cast<MapView *>(GetActiveView());
      if (bUpdateNow && map_view != nullptr)
      {
         m_pMapTabsBar->MapParamsChanged(
            MapParamsListener_Interface::MAP_TYPE_CHANGED,
            map_view->GetMapGroupIdentity(), map_view->GetMapType(),
            map_view->GetMapProjParams(), map_view->GetMapDisplayParams());
      }
   }
}

BOOL CMainFrame::SaveStatusBarConfig(CString& strFilename)
{
   try
   {
      DWORD dwStatus = CreateAllDirectories(_bstr_t(strFilename));
      if (dwStatus != NO_ERROR)
      {
         CString msg;
         msg.Format("CreateAllDirectories failed : %d", dwStatus);
         ERR_report(msg);
         return FALSE;
      }

      MSXML2::IXMLDOMDocumentPtr smpXMLDOMDocument;
      CO_CREATE(smpXMLDOMDocument, __uuidof(MSXML2::DOMDocument60));

      MSXML2::IXMLDOMProcessingInstructionPtr smpXMLDOMProcessingInstruction =
         smpXMLDOMDocument->createProcessingInstruction("xml", "version=\"1.0\"");

      smpXMLDOMDocument->appendChild(smpXMLDOMProcessingInstruction);

      bstr_t xml_element = _T("Element");
      _bstr_t xml_ns = _T("urn:FalconView/StatusBar");
      MSXML2::IXMLDOMElementPtr smpRootElement = smpXMLDOMDocument->createNode(xml_element, "StatusBarConfiguration", xml_ns);

      smpXMLDOMDocument->documentElement = smpRootElement;

      for(int i=0; i<m_statusBarMgr->m_arrStatusBars.GetSize(); ++i)
      {
         CMapStatusBar *pStatusBar = m_statusBarMgr->m_arrStatusBars[i];

         MSXML2::IXMLDOMElementPtr smpStatusBarElement = smpXMLDOMDocument->createNode(xml_element, "StatusBar", xml_ns);
         m_statusBarMgr->m_arrStatusBars[i]->SaveConfig(smpXMLDOMDocument, smpStatusBarElement);
         smpRootElement->appendChild(smpStatusBarElement);
      }

      smpXMLDOMDocument->save(_bstr_t(strFilename));
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed saving status bar configuration to %s - %s", strFilename, (char *)e.Description());
      AfxMessageBox(msg);
      return FALSE;
   }

   return TRUE;
}

BOOL CMainFrame::LoadStatusBarConfig(CString& strFilename)
{
   try
   {
      // Load the schema into the scheme cache
      //
      MSXML2::IXMLDOMSchemaCollectionPtr smpSchemaCollection;
      {
         CO_CREATE(smpSchemaCollection, __uuidof(MSXML2::XMLSchemaCache60));

         MSXML2::IXMLDOMDocumentPtr smpSchema;
         CO_CREATE(smpSchema, __uuidof(MSXML2::DOMDocument60));

         smpSchema->async = VARIANT_FALSE;

         CString strSchemaPath = PRM_get_registry_string("Main", "HD_DATA", "") + "\\Schemas\\StatusBarConfig.xsd";
         if (smpSchema->load(_bstr_t(strSchemaPath)) == VARIANT_FALSE)
         {
            CString msg;
            msg.Format("Unable to load schema from %s.  Configuration will not be loaded.", strSchemaPath);
            AfxMessageBox(msg);
            return FALSE;
         }

         _variant_t var;
         var.vt = VT_DISPATCH;
         var.pdispVal = smpSchema;
         var.pdispVal->AddRef();
         smpSchemaCollection->add(_T("urn:FalconView/StatusBar"), var);
      }

      MSXML2::IXMLDOMDocument2Ptr smpXMLDOMDocument;
      CO_CREATE(smpXMLDOMDocument, __uuidof(MSXML2::DOMDocument60));

      smpXMLDOMDocument->async = VARIANT_FALSE;

      _variant_t varSchemas;
      varSchemas.vt = VT_DISPATCH;
      varSchemas.pdispVal = smpSchemaCollection;
      varSchemas.pdispVal->AddRef();
      smpXMLDOMDocument->schemas = varSchemas;

      if (smpXMLDOMDocument->load(_bstr_t(strFilename)) != VARIANT_TRUE)
         return FALSE;

      MSXML2::IXMLDOMParseErrorPtr error = smpXMLDOMDocument->validate();
      if (error->errorCode != 0)
      {
         CString err;
         err.Format("Error validating %s - %s\n", strFilename, (char *)error->reason);
         AfxMessageBox(err);
         return FALSE;
      }

      MSXML2::IXMLDOMElementPtr smpRoot = smpXMLDOMDocument->documentElement;
      MSXML2::IXMLDOMNodeListPtr smpNodeList = smpRoot->childNodes;
      XMLNodePtr smpNode = smpNodeList->nextNode();

      // we want the map tabs bar on top of the status bars.  Delete it here an re-add after
      // the status bars get created
      const int nVisible = m_pMapTabsBar && (m_pMapTabsBar->GetStyle() & WS_VISIBLE) == WS_VISIBLE;
      if (m_pMapTabsBar)
      {
         m_pMapTabsBar->DestroyWindow();
         m_pMapTabsBar = nullptr;
      }

      // store the current visibility of the status bar panes
      int status_bars_cmd_show = SW_SHOW;
      if (m_statusBarMgr->m_arrStatusBars.GetSize() &&
         (m_statusBarMgr->m_arrStatusBars[0]->GetStyle() & WS_VISIBLE) == 0)
      {
         status_bars_cmd_show = SW_HIDE;
      }

      // delete existing status bars
      for(int i=0;i<m_statusBarMgr->m_arrStatusBars.GetSize();++i)
         delete m_statusBarMgr->m_arrStatusBars[i];
      m_statusBarMgr->m_arrStatusBars.RemoveAll();

      while (smpNode != NULL)
      {
         CMapStatusBar *pStatusBar = new CMapStatusBar();
         if (!pStatusBar->Create(this, m_statusBarMgr->m_arrStatusBars.GetCount() == 0))
         {
            ERR_report("Failed to create status bar.");
            return FALSE;
         }
         pStatusBar->ShowWindow(status_bars_cmd_show);

         // This RecalcLayout is necessary in case any of the info panes needs to draw based on
         // the size of the status bar.  Before this call the status bar's clientRect is empty.
         RecalcLayout();

         m_statusBarMgr->m_arrStatusBars.Add(pStatusBar);
         pStatusBar->LoadConfig(smpNode);

         smpNode = smpNodeList->nextNode();
      }

      CreateMapTabsBar(nVisible == TRUE);
      m_pMapTabsBar->ShowWindow(nVisible ? SW_SHOW : SW_HIDE);

      RecalcLayout();
      Invalidate();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to load status bar configuration from %s - %s", strFilename, (char *)e.Description());
      AfxMessageBox(msg);
      return FALSE;
   }

   return TRUE;
}

void CMainFrame::OnViewNewTab()
{
   if (m_pMapViewTabsBar != NULL)
   {
      m_pMapViewTabsBar->AddTab();
   }
}

// get the status bar manager associated with the frame
IFvStatusBarManager *CMainFrame::m_pFvStatusBarManager()
{
   return dynamic_cast<IFvStatusBarManager *>(m_statusBarMgr);
}

void CMainFrame::ClosePrintPreview()
{
   CMainFrame* frame = fvw_get_frame();
   if (frame != nullptr)
   {
      CView* pView = frame->GetActiveView();
      if (pView && pView->IsKindOf(RUNTIME_CLASS(CPreviewView))) 
      {
         CPreviewView* pView = (CPreviewView *)frame->GetActiveView();
         frame->SendMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);
      }
   }
}

//do comparison - if the teststr contains all the same characters
//in the same order as testSubString, regardless of other characters
//than might separate them, consider it a match
BOOL str_contains(const char *testStr,const char *testSubString)
{
   BOOL result = FALSE;
   int k = 0;
   for( int j = 0 ; testStr[j]!='\0' ; j++)
   {
      //look for character match
      if( testSubString[k] == testStr[j] )
      {
         k++;
         if( testSubString[k] == '\0' )
         {
            //we found every letter goal
            result = TRUE;
         }
      }
      //are there other letters?
      else if( result )
      {
         result++;
      }

      //this is a hack to strip everything after and including an elipsis,
      //because otherwise "Print" matches "Print Preview" rather than
      //"Print...||Ctrl P" (Also "Save loses to "Save As")
      if( j>=2 &&
         testStr[j] == '.' &&
         testStr[j-1] == '.' &&
         testStr[j-2]== '.' )
      {
         if( result >= 3)
         {
            return result-3;
         }
         else
         {
            return result;
         }
      }
   }
   return result;
}


// Returns the item identifier with the match given item_locator_string, NULL for not found.
// will attempt "best match" on strings, rather than exact match.
//the input CString represents a single ribbon operator command, where the various subelement
//navigation is encoded as newlines.  eg. "File\n\rExit"
UINT CMainFrame::get_ribbon_item_id( CString item_locator_string ) const
{
   CFvwTextLineList ribbon_locator_list( item_locator_string );

   UINT result = 0;
   if ( ribbon_locator_list.GetCount() >= 2 )
   {
      const CString
         &csCatWanted = ribbon_locator_list.GetNext(),
         &csItemWanted = ribbon_locator_list.GetNext();

      int cCategories = m_wndRibbonBar.GetCategoryCount();
      for ( int iCat = 0; iCat < cCategories; iCat++ )
      {
         CMFCRibbonCategory& cat = *m_wndRibbonBar.GetCategory( iCat );
         CString csCatName = cat.GetName();

         int iCatMatch = str_contains( csCatName, csCatWanted );
         if ( iCatMatch <= 0 )
            continue;
         
         int iItemMatch = INT_MAX;
         CArray< CMFCRibbonBaseElement*, CMFCRibbonBaseElement* > aElements;
         cat.GetElements( aElements );

         int cElems = aElements.GetCount();
         for ( int iElem = 0; iElem < cElems; iElem++ )
         {
            CString csElemName = aElements[ iElem ]->GetText();

            int iMatch = str_contains( csElemName, csItemWanted );
            if ( iMatch <= 0 )
               continue;

            if ( iMatch < iItemMatch )
            {
               UINT ui = aElements[ iElem ]->GetID();
               if ( ui == ID_ZOOM_COMBO
                  ||  ribbon_locator_list.GetCount() == 2 )
               {
                  iItemMatch = iMatch;
                  result = ui;
               }
            }

#if 0 && defined _DEBUG
            ATLTRACE( _T("Cat match=%d, item match=%d, \"%s\", \"%s\", ID=%u\n"),
               iCatMatch, iMatch, csCatName, csElemName, aElements[ iElem ]->GetID() );
#endif
         }  // Element loop
      }  // Category loop
   
      // Last ditch check for special controls
      if ( result == 0
            && csItemWanted == _T("Zoom Control") )
         result = ID_ZOOM_COMBO;

   }  // Two level lookup

   return result;
}

LPARAM CMainFrame::OnUIThreadOperation(WPARAM wparam, LPARAM lparam)
{
   // Extract the UIThreadOperation from the WPARAM, perform the operation,
   // and cleanup memory
   auto op = reinterpret_cast<fvw::UIThreadOperation *>(wparam);
   op->ExecuteOperation();
   delete op;

   return 0;
}

void CMainFrame::OnElevationExaggerationSlider()
{
   // Set up a timer to delay-write the slider position in the registry
   if ( m_pElevationExaggerationSliderSettleTimer == nullptr )
   {
      m_pElevationExaggerationSliderSettleTimer =
      std::make_shared<CSliderSettleTimer>( EXAGGERATION_SLIDER_TIMER_TYPE );
   }

   if ( UTL_get_timer_list()->
         Find(m_pElevationExaggerationSliderSettleTimer.get() ) == nullptr )
   {
      UTL_get_timer_list()->add(
         m_pElevationExaggerationSliderSettleTimer.get() );
   }

   m_iElevationExaggerationValue = m_pElevationExaggerationSlider->GetPos();

   // Show the slider value in the edit box
   CString cs;
   cs.Format( _T("%d"), m_iElevationExaggerationValue );
   m_pElevationExaggerationEdit->SetEditText( cs );

   m_pElevationExaggerationSliderSettleTimer->restart();

   int height_scale = -1;
   MapView *pView = static_cast<MapView *>(GetActiveView());
   if (pView)
   {
      height_scale = m_iElevationExaggerationValue;
      GeospatialScene* scene = pView->GetGeospatialViewController()->GetGeospatialScene();
      scene->SetVerticalScale(height_scale);
      pView->invalidate_view();
   }
}  // OnElevationExaggerationSlider()

void CMainFrame::OnElevationExaggerationEdit()
{
   CString cs = m_pElevationExaggerationEdit->GetEditText();
   int i;
   TCHAR tch;
   if ( 1 == _stscanf_s( cs, _T("%d%c"), &i, &tch, 1 )
      &&  i >= m_pElevationExaggerationSlider->GetRangeMin()
      && i <= m_pElevationExaggerationSlider->GetRangeMax() )
   {
      m_pElevationExaggerationSlider->SetPos( i );
      OnElevationExaggerationSlider();
      return;
   }
   cs.LoadString( IDS_ELEVATION_EXAGGERATION_ERROR_MSG );
   cs.Format( cs, m_pElevationExaggerationSlider->GetRangeMin(),  m_pElevationExaggerationSlider->GetRangeMax() );
   CString csTitle;
   csTitle.LoadString( IDS_ELEVATION_EXAGGERATION_ERROR_TITLE );
   MessageBox( cs, csTitle, MB_OK | MB_ICONWARNING );

   cs.Format( _T("%d"), m_iElevationExaggerationValue );
   m_pElevationExaggerationEdit->SetEditText( cs );
}

void CMainFrame::OnUpdateElevationExaggeration(CCmdUI* cmd_ui)
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
   {
      cmd_ui->Enable(map_view->GetProjectionType() ==
         GENERAL_PERSPECTIVE_PROJECTION);
   }
}

void CMainFrame::On3DMapTiltSlider()
{
   int tilt_value = m_p3DMapTiltSlider->GetPos();

   MapView* map_view = static_cast<MapView *>(GetActiveView());
   if (map_view)
   {
      map_view->ChangeTilt(tilt_value);
   }

}  // OnMapTiltSlider()

void CMainFrame::OnUpdate3DMapTilt( CCmdUI* pCmdUI )
{
   MapView* pView = static_cast< MapView* >( GetActiveView() );

   if ( pView )
   {
      pCmdUI->Enable(
         (pView->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION) &&
         (pView->GetCameraMode() == fvw::CAMERA_MODE_PAN ||
         pView->GetCameraMode() == fvw::CAMERA_MODE_GROUND_VIEW)
         && pView->GetCameraMode() != fvw::CAMERA_MODE_FLIGHT
         );
   }
}

void CMainFrame::OnUnderseaElevations()
{
   m_bUnderseaElevationsEnable = !m_bUnderseaElevationsEnable;
   PRM_set_registry_int( "View", "Enable_3D_DBDB_Elevation_Data",
      m_bUnderseaElevationsEnable ? 1 : 0 );

   // Invalidate the map to refresh 3D tiles
   MapView* pView = static_cast< MapView* >( GetActiveView() );
   if ( pView != NULL )
      pView->redraw_map_from_scratch();
}

void CMainFrame::OnUpdateUnderseaElevations(CCmdUI* cmd_ui)
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
   {
      cmd_ui->SetCheck(m_bUnderseaElevationsEnable ?
         BST_CHECKED : BST_UNCHECKED);

      cmd_ui->Enable(map_view->IsProjectionSupported(
         GENERAL_PERSPECTIVE_PROJECTION));
   }
}

void OnUpdateCameraMode(CMainFrame* frame, CCmdUI* pCmdUI, fvw::CameraMode mode)
{
   MapView* map_view = static_cast<MapView*>(frame->GetActiveView());
   if (map_view)
   {
      if (map_view->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
      {
         if (METERS_TO_FEET(map_view->GetGeospatialViewController()->GetGeospatialView()->
            Viewer()->ComputeAltitudeMSL()) < map_view->GetGeospatialViewController()->
            GetGeospatialView()->Viewer()->m_max_altitude_feet_flight_ground)
         {
            pCmdUI->Enable(TRUE);
            pCmdUI->SetCheck(map_view->GetCameraMode() == mode);
         }
         else
         {
            if (mode == fvw::CAMERA_MODE_PAN)
            {
               pCmdUI->Enable(TRUE);
               pCmdUI->SetCheck(map_view->GetCameraMode() == mode);
            }
            else
            {
               pCmdUI->Enable(FALSE);
            }
         }
      }
      else
         pCmdUI->Enable(FALSE);
   }
}

void CMainFrame::OnCameraModePan()
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
      map_view->SetCameraMode(fvw::CAMERA_MODE_PAN);
}

void CMainFrame::OnUpdateCameraModePan(CCmdUI* pCmdUI)
{
   OnUpdateCameraMode(this, pCmdUI, fvw::CAMERA_MODE_PAN);
}

void CMainFrame::OnCameraModeFlight()
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
   {
      // If already in flight mode, then toggle the flight mode
      // auto MapType update state
      if (map_view->GetCameraMode() == fvw::CAMERA_MODE_FLIGHT)
         map_view->ToggleEnableFlightModeAutoMapType();

      map_view->SetCameraMode(fvw::CAMERA_MODE_FLIGHT);
   }
}

void CMainFrame::OnUpdateCameraModeFlight(CCmdUI* pCmdUI)
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
   {
      int image_idx = map_view->GetEnableFlightModeAutoMapType() ?
         RIBBON_BUTTON_IMAGE_FLIGHT_MODE_UNLOCKED :
         RIBBON_BUTTON_IMAGE_FLIGHT_MODE_LOCKED;

      if (m_flight_mode_button->GetImageIndex(TRUE) != image_idx)
      {
         m_flight_mode_button->SetImageIndex(image_idx, TRUE);
         m_flight_mode_button->Redraw();
      }

      OnUpdateCameraMode(this, pCmdUI, fvw::CAMERA_MODE_FLIGHT);
   }
}

void CMainFrame::OnCameraModeGroundView()
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
      map_view->SetCameraMode(fvw::CAMERA_MODE_GROUND_VIEW);
}

void CMainFrame::OnUpdateCameraModeGroundView(CCmdUI* pCmdUI)
{
   OnUpdateCameraMode(this, pCmdUI, fvw::CAMERA_MODE_GROUND_VIEW);
}

CSliderSettleTimer::~CSliderSettleTimer()
{
   POSITION pos = UTL_get_timer_list()->Find( static_cast< FVW_Timer* >( this ) );
   if ( pos != NULL )
      UTL_get_timer_list()->remove_at( pos );

   // Write the current slider value back to the registry
   switch ( m_eTimerType )
   {
      case EXAGGERATION_SLIDER_TIMER_TYPE:
         CMainFrame* frame = fvw_get_frame();
         if (frame != nullptr)
         {
            PRM_set_registry_int( "View", "ElevationExaggeration",
               frame->m_iElevationExaggerationValue );
         }
         break;
   }
}

void CSliderSettleTimer::expired()
{
   switch ( m_eTimerType )
   {
      case EXAGGERATION_SLIDER_TIMER_TYPE:
         CMainFrame* frame = fvw_get_frame();
         if (frame != nullptr)
         {
            frame->m_pElevationExaggerationSliderSettleTimer = nullptr;
         }
         break;
   }
}

// DisableAddToQuickAccessToolbarRibbonButton
//

IMPLEMENT_DYNCREATE(DisableAddToQuickAccessToolbarRibbonButton, CMFCRibbonButton)

// Modified from afxribbonbutton.cpp to add support for non-QaTable subelements
void DisableAddToQuickAccessToolbarRibbonButton::SetMenu(HMENU hMenu,
   BOOL bIsDefaultCommand /*= FALSE*/, BOOL bRightAlign /*= FALSE*/)
{
   ASSERT_VALID(this);

   m_bIsWindowsMenu = FALSE;
   m_nWindowsMenuItems = 0;

   if (m_bAutodestroyMenu && m_hMenu != NULL)
   {
      ::DestroyMenu(m_hMenu);
   }

   m_bAutodestroyMenu = FALSE;

   if (m_bUseMenuHandle)
   {
      m_hMenu = hMenu;
   }
   else
   {
      CMenu* pMenu = CMenu::FromHandle(hMenu);

      for (int i = 0; i < pMenu->GetMenuItemCount(); i++)
      {
         UINT uiID = pMenu->GetMenuItemID(i);

         switch(uiID)
         {
         case 0:
            {
               CMFCRibbonSeparator* pSeparator = new CMFCRibbonSeparator(TRUE);
               pSeparator->SetDefaultMenuLook();

               AddSubItem(pSeparator);
               break;
            }

         default:
            {
               CString str;
               pMenu->GetMenuString(i, str, MF_BYPOSITION);

               //-----------------------------------
               // Remove standard aceleration label:
               //-----------------------------------
               int iTabOffset = str.Find(_T('\t'));
               if (iTabOffset >= 0)
               {
                  str = m_strText.Left(iTabOffset);
               }

               DisableAddToQuickAccessToolbarRibbonButton* pItem =
                  new DisableAddToQuickAccessToolbarRibbonButton(uiID, str);
               pItem->SetDefaultMenuLook();
               pItem->m_pRibbonBar = m_pRibbonBar;

               if (uiID == -1)
               {
                  pItem->SetMenu(pMenu->GetSubMenu(i)->GetSafeHmenu(), FALSE, bRightAlign);
               }

               AddSubItem(pItem);

               if (uiID >= AFX_IDM_WINDOW_FIRST && uiID <= AFX_IDM_WINDOW_LAST)
               {
                  m_bIsWindowsMenu = TRUE;
               }
            }
         }
      }
   }

   m_bIsDefaultCommand = bIsDefaultCommand;

   if (m_nID == 0 || m_nID == (UINT)-1)
   {
      m_bIsDefaultCommand = FALSE;
   }

   m_bRightAlignMenu = bRightAlign;

   m_sizeTextRight = CSize(0, 0);
   m_sizeTextBottom = CSize(0, 0);

   m_bCreatedFromMenu = TRUE;
}


void CMainFrame::OnToggle2DProjection()
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
      map_view->GetGeospatialViewController()->
         GetInputDevice()->frame_update_wait = true;
      map_view->ChangeProjectionType(EQUALARC_PROJECTION);
}

void CMainFrame::OnUpdateToggle2DProjection(CCmdUI* cmd_ui)
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
   {
      cmd_ui->SetCheck(map_view->GetMapProjParams().type !=
         GENERAL_PERSPECTIVE_PROJECTION);
   }
}

void CMainFrame::OnToggle3DProjection()
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
   {
      map_view->GetGeospatialViewController()->
         GetInputDevice()->frame_update_wait = true;
      map_view->ChangeProjectionType(GENERAL_PERSPECTIVE_PROJECTION);
   }
}

void CMainFrame::OnUpdateToggle3DProjection(CCmdUI* cmd_ui)
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (map_view)
   {
      cmd_ui->SetCheck(map_view->GetMapProjParams().type ==
         GENERAL_PERSPECTIVE_PROJECTION);

      cmd_ui->Enable(
         map_view->IsProjectionSupported(GENERAL_PERSPECTIVE_PROJECTION));
   }
}

void CMainFrame::OnCameraModeOptions()
{
   MapView* map_view = static_cast<MapView*>(GetActiveView());
   if (!map_view)
      return;

   GeospatialViewController* gvc = map_view->GetGeospatialViewController();
   if (!gvc)
      return;

   GeospatialScene* scene = gvc->GetGeospatialScene();
   if (!scene)
      return;

   // Make a local copy of HUD properties
   HudProperties hud_properties = scene->GetHudProperties();

   CameraModeOptionsDialog dlg(&hud_properties);
   if (dlg.DoModal() == IDOK)
   {
      scene->SetHudProperties(hud_properties);
      fvw_prefs::StoreHudProperties(hud_properties);

      if (map_view->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
         map_view->invalidate_view();
   }
}


LRESULT CMainFrame::OnMomentaryButtonPressed(WPARAM wParam, LPARAM lParam)
{
   MapView *view = static_cast<MapView *>(GetActiveView());
   if (view != nullptr &&  view->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      // Set the appropriate flag in the GeoInputDevice class
      UINT id = (UINT) wParam;
      switch(id)
      {
      case ID_MAP_PAN_LEFT:
         view->GetGeospatialViewController()->SetLeftButtonState(true);
         break;
      case ID_MAP_PAN_UP:
         view->GetGeospatialViewController()->SetUpButtonState(true);
         break;
      case ID_MAP_PAN_RIGHT:
         view->GetGeospatialViewController()->SetRightButtonState(true);
         break;
      case ID_MAP_PAN_DOWN:
         view->GetGeospatialViewController()->SetDownButtonState(true);
         break;
      case ID_MAP_ROTATE_CLOCKWISE:
         view->GetGeospatialViewController()->SetRotCwButtonState(true);
         break;
      case ID_MAP_ROTATE_NORTHUP:
         view->GetGeospatialViewController()->SetNorthUpButtonState(true);
         break;
      case ID_MAP_ROTATE_COUNTERCLOCKWISE:
         view->GetGeospatialViewController()->SetRotCcwButtonState(true);
         break;
      }
   }

   return 0;
}


LRESULT CMainFrame::OnMomentaryButtonReleased(WPARAM wParam, LPARAM lParam)
{
   MapView *view = static_cast<MapView *>(GetActiveView());
   if (view != nullptr &&  view->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      UINT id = (UINT) wParam;
      switch(id)
      {
      case ID_MAP_PAN_LEFT:
         view->GetGeospatialViewController()->SetLeftButtonState(false);
         break;
      case ID_MAP_PAN_UP:
         view->GetGeospatialViewController()->SetUpButtonState(false);
         break;
      case ID_MAP_PAN_RIGHT:
         view->GetGeospatialViewController()->SetRightButtonState(false);
         break;
      case ID_MAP_PAN_DOWN:
         view->GetGeospatialViewController()->SetDownButtonState(false);
         break;
      case ID_MAP_ROTATE_CLOCKWISE:
         view->GetGeospatialViewController()->SetRotCwButtonState(false);
         break;
      case ID_MAP_ROTATE_NORTHUP:
         view->GetGeospatialViewController()->SetNorthUpButtonState(false);
         break;
      case ID_MAP_ROTATE_COUNTERCLOCKWISE:
         view->GetGeospatialViewController()->SetRotCcwButtonState(false);
         break;
      }
   }

   return 0;
}
