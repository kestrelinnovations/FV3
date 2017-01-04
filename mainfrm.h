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



// mainfrm.h : interface of the CMainFrame class
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef MAINFRM_H
#define MAINFRM_H

#define DEFAULT_VERTICAL_DISPLAY_HEIGHT 200  // default pixel height of the Vertical Display window
#define MINIMUM_ALLOWABLE_MAP_WINDOW_HEIGHT 30  // minimum allowable height of the Map window

#define REG_ENABLE_3D 520    // Registry value to set Enable3D key to enable 3D support in FV. 

//
// forward declarations
//
class DataSource;
class CMissionPackageDlg;
class MAPIWrapper;
class CSupplementalDataHandler;
class CMapTabsBar;
class CMapViewTabsBar;
class CStatusBarInfoPane;
class CStatusBarAvailableInfoPaneList;
class CStatusBarSetupDialog;
class CElementRenderingDlg;
class CVerticalViewPropertiesDialog;
class CVerticalViewProperties;
class CVerticalViewDisplay;
class CFvFrameWndImpl;
class CRibbonImpl;
class CFvToolMenuItemCOM;
class CFvDockablePane;
class CFvManagedDockablePane;
class OverlayMenu;
class FileOverlayRibbonButtonsGroup;
class ClientToolbar;
class MapType;
class MapMenu;
class CFvStatusBarManager;
class CCustomTBClass;
class CViewTime;
class MapSpec;
class ProjectionID;
class CCustomInitializerCOM;
class FileOverlayListComboBox;
class CSliderSettleTimer;
class GeospatialScene;
class CFvSystemHealthCOM;
class MapTiltSlider;

interface IFvStatusBarManager;

struct IDispatch;
struct ICollaborationServer;
struct FavoriteData;

#define MAX_MAP_TYPE_OPTIONS 20

#define GET_POSITION_ADD_COAST_TRACK 0
#define GET_POSITION_MODIFY_COAST_TRACK 1
#define GET_POSITION_ADD_RANGE_BEARING 2
#define GET_POSITION_MODIFY_RANGE_BEARING 3
#define GET_POSITION_STAT_BAR_BULLSEYE 4

typedef struct
{
   MapType *map_type;
   CString menu_name;
} map_type_options_t;

enum SliderSettleTimerTypeEnum { EXAGGERATION_SLIDER_TIMER_TYPE };

extern int nMapCount;

BOOL str_contains( LPCSTR testStr, LPCSTR testSubString );  // In MainFrm.cpp



class MomentaryRibbonButton : public CMFCRibbonButton
{
public:
   MomentaryRibbonButton() : CMFCRibbonButton() {}
   MomentaryRibbonButton(UINT nID, LPCTSTR lpszText, int nSmallImageIndex = -1,
      int nLargeImageIndex = -1, BOOL bAlwaysShowDescription = FALSE)
      : CMFCRibbonButton(nID, lpszText, nSmallImageIndex, nLargeImageIndex, bAlwaysShowDescription) {}

public:
   virtual void OnLButtonDown(CPoint point);
   virtual void OnLButtonUp(CPoint point);
};


// CDynamicSplitterWnd
class CDynamicSplitterWnd : public CSplitterWnd
{
public:
   void SplitView(CRuntimeClass* pRuntimeClass, int nVerticalDisplayHeight);
   void DeleteRow(int rowDelete);

};


// Superclass to allow ribbonbar to be hidden/minimized programmatically
class CMainRibbonBar : public CMFCRibbonBar
{
   class CMainRibbonCategory: public CMFCRibbonCategory
   {
   public:
      void ShowElements( BOOL bShow )
      {
        CMFCRibbonCategory::ShowElements( bShow );
      }
   };

public:
   CMainRibbonBar() : m_pCapturedButton( nullptr ) {}

   BOOL IsMinimized()
   {
      return m_dwHideFlags == AFX_RIBBONBAR_HIDE_ELEMENTS;
   }

   void Minimize( BOOL bMinimize )
   {
      CMainRibbonCategory* cc = static_cast< CMainRibbonCategory* >( GetActiveCategory() );
      if (cc != NULL)
      {
         cc->ShowElements( !bMinimize );
         RedrawWindow();
      }
   }

   // Designate a momentary button to receive a mouse up event even if the mouse
   // has moved away from it or this ribbon.
   void SetCapturedButton(MomentaryRibbonButton *pButton)
   {
      m_pCapturedButton = pButton;

      SetCapture();
   }

   // Release the mouse capture and clear out the designated momentary button
   void ReleaseCapturedButton()
   {
      ReleaseCapture();

      m_pCapturedButton = nullptr;
   }

protected:
   //{{AFX_MSG(CMainRibbonBar)
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   // Button which will receive captured mouse-up event
   MomentaryRibbonButton * m_pCapturedButton;
}; // CMainRibbonBar


class DisableAddToQuickAccessToolbarRibbonButton : public CMFCRibbonButton
{
   DECLARE_DYNCREATE(DisableAddToQuickAccessToolbarRibbonButton)

public:
   DisableAddToQuickAccessToolbarRibbonButton()
   {
   }
   DisableAddToQuickAccessToolbarRibbonButton(UINT uiID, const CString& str) :
      CMFCRibbonButton(uiID, str)
   {
   } 

   void SetMenu(HMENU hMenu, BOOL bIsDefaultCommand = FALSE,
      BOOL bRightAlign = FALSE);

   virtual BOOL CanBeAddedToQuickAccessToolBar() const { return FALSE; }
};


interface IFvFrameWnd
{
   // get the status bar manager associated with the frame
   virtual IFvStatusBarManager *m_pFvStatusBarManager() = 0;
};

class CMainFrame : 
   public CFrameWndEx, 
   public IFvFrameWnd
{
   friend class C_localpnt_ovl;
   friend class UserSimulator;

protected: // create from serialization only
   CMainFrame();
   DECLARE_DYNCREATE(CMainFrame)

   CComObject<CFvFrameWndImpl> *m_pFvFrameWnd;
   CComObject<CRibbonImpl> *m_pRibbonCOM;

public:
   FalconViewOverlayLib::IFvFrameWnd2 *GetFvFrameWnd();
   FalconViewOverlayLib::IRibbon* GetRibbon();

   int m_num_map_type_dialogs;
   map_type_options_t m_map_type_properties_tbl[MAX_MAP_TYPE_OPTIONS];

// Attributes
public:

   // IFvFrameWnd
public:
   // get the status bar manager associated with the frame
   virtual IFvStatusBarManager *m_pFvStatusBarManager();

// Implementation
public:
   virtual ~CMainFrame();

   void PostCreateWindow();   // called after the frame is successfully created

   CMapTabsBar *GetMapTabsBar() { return m_pMapTabsBar; }
   CMapViewTabsBar *GetMapViewTabsBar() { return m_pMapViewTabsBar; }

   // Pointer to the Graphics Drawing Prefs dialog object
   CElementRenderingDlg* m_pGraphicDrawingPrefsDialog;

   // pointer to the modeless Vertical Display Properties Dialog object
   CVerticalViewPropertiesDialog *m_pVerticalDisplayPropertiesDialog;

   // pointer to the Vertical Display Properties object
   CVerticalViewProperties* m_pVerticalDisplayProperties;

   FalconViewOverlayLib::IFvMapView* GetFvMapView();
   CVerticalViewDisplay *GetVerticalDisplay();
   CVerticalViewProperties* GetVerticalDisplayPropertiesObject();

   // Attached one dockable pane to another - this will result in a tabbed pane
   void AttachDockablePane(
      FalconViewOverlayLib::IFvDockablePane* pDockablePane,
      FalconViewOverlayLib::IFvDockablePane* pDockablePaneToAttachTo);
   void CloseDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane);
   CFvDockablePane* CreateDockablePane(GUID classId, GUID dockablePaneGuid,
      const CString& windowName, const CString& iconName,
      const CPoint& initialPos, const CSize& initialSize, long hasCaption,
      FalconViewOverlayLib::DockablePaneAlignment initialAlignment, 
      long allowableAlignments, long canClose, long canResize, const CSize& minimumSize,
      long canAutoHide);
   CFvManagedDockablePane* CreateManagedDockablePane(FalconViewOverlayLib::IFvFrameWnd3Ptr frameWnd,
      GUID classId, GUID dockablePaneGuid,
      const CString& windowName, const CString& iconName,
      const CPoint& initialPos, const CSize& initialSize, long hasCaption,
      FalconViewOverlayLib::DockablePaneAlignment initialAlignment, 
      long allowableAlignments, long canClose, long canResize, const CSize& minimumSize,
      long canAutoHide);
   CFvDockablePane* FindDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane);
   void ShowDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane);
   void HideDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane);
   HRESULT MinimizeDockablePane(FalconViewOverlayLib::IFvDockablePane* pDockablePane);
   bool HideMinimizedPanesPanel();

protected:
   bool CreateMinimizedPanesPanel();
   CMFCRibbonPanel* m_minimizedPanesPanel;

public:
   void UpdateFavoritesMenu();

#ifdef _DEBUG
   virtual  void AssertValid() const;
   virtual  void Dump(CDumpContext& dc) const;
#endif

protected:
   BOOL OnCloseMiniFrame(CPaneFrameWnd* pWnd);

   void CreateMapTabsBar(bool bUpdateNow = true);

public:  // control bar embedded members

   CFvStatusBarManager* m_statusBarMgr;
   
   CStatusBarSetupDialog *m_pStatusBarSetupDialog;

   CStatusBarInfoPane *GetStatusBarInfoPane(CString strInfoPaneName);

   void AddStatusBarRow();
   void RemoveStatusBarRow();

   BOOL SaveStatusBarConfig(CString& strFilename);
   BOOL LoadStatusBarConfig(CString& strFilename);

   CMapTabsBar *m_pMapTabsBar;
   CMapViewTabsBar *m_pMapViewTabsBar;
   CMainRibbonBar m_wndRibbonBar;
   CMFCRibbonPanel* m_pAdditionalToolsPanel; // Used for FvToolMenuItems.xml
   CMFCRibbonPanel* m_pOverlaysPanel; // Used for merging hard coded and external (FvOverlayTypes.xml) buttons
   CMFCRibbonCategory* m_pOverlaysCategory; // Used for managing items on the Overlays ribbon item
   CMFCRibbonCategory* m_pEditorsCategory; // Used for managing items on the Editors ribbon item
   CMFCRibbonPanel* m_pEditorsPanel;
   CMFCRibbonPanel* m_pMapDataManagerPanel;
   CMFCRibbonPanel* m_pCustomToolsPanel;
   CMFCRibbonApplicationButton m_applicationButton;
   CMFCToolBarImages m_toolbarImages;
   OverlayMenu* m_overlay_menu;
   DisableAddToQuickAccessToolbarRibbonButton* m_favorites_menu;
   std::map<long, CComPtr<ClientToolbar>> m_client_toolbars;
   FileOverlayRibbonButtonsGroup* m_file_overlay_ribbon_btns_group;
   FileOverlayListComboBox* m_file_overlay_list_cb;

   CFvSystemHealthCOM* m_pSystemHealth;

   CCustomTBClass *m_custom_toolbar;
   BOOL m_coverage_panel;
   BOOL m_fullscreen;
   BOOL m_dafif_importer_active;
   BOOL m_precise_geo;
   BOOL m_colored_coord;
   int m_next_client_toolbar_handle;

   // projection menu
   CMap <int, int, CString, CString&> m_projection_string_map;

   static CViewTime& GetPlaybackDialog() { return *m_view_time_dlg; }
   static boolean_t IsPlaybackDialogActive() { return m_view_time_dlg_active; }
   static void SetPlaybackDialogActive(boolean_t a) { m_view_time_dlg_active = a; }

   // mission binder dialog
   static CMissionPackageDlg *m_mission_binder_dlg;

   // for falconview networked collaborations (thr,points,etc)
   // made static because we need it inside the CFactoryList
   ICollaborationServer* m_pCollaborationServer;

   MAPIWrapper *m_MAPI;

   MapMenu* m_map_menu;

   std::vector<CCustomInitializerCOM *> m_customInitializers; 
   std::map<UINT, CFvToolMenuItemCOM *> m_toolMenuItems;

   void RebuildMapMenu();

public:
   // add a toolbar to the frame -- used for client applications
   // to develop custom toolbars
   int add_client_toolbar(CWnd* parent, CString bitmap_filename, 
      int num_buttons, IDispatch* pdisp, long client_hWnd,
      CString toolbar_name, short initial_position,
      long* separator_lst, int num_separators);

   // set the properties of a client-added toolbar button
   int setup_client_toolbar_button(int toolbar_handle, int button_number,
      CString tooltip_text, CString status_bar_text);

   // set the status of a client's toolbar's button (up=0, down=1)
   int set_client_toolbar_button_state(int toolbar_handle, int button_number, 
      short button_down);

   // remove the client-added toolbar with the given handle
   int remove_client_toolbar(int toolbar_handle);



// Overloaded function for creating the Vertical View splitter window
public:
   virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

   CDynamicSplitterWnd m_wndSplitter;
   int m_nLastVertDisplayViewHeight;

   int m_nVerticalDisplayWidth, m_nVerticalDisplayHeight, m_nDefaultVerticalDisplayHeight;

   afx_msg BOOL ToggleVerticalView(UINT);

// Overloaded function for dynamically modifying the status bar
// menu messages.
public:
   virtual void GetMessageString(UINT nID, CString& rMessage) const;

   // This method should probably be a message handler in the MSG_MAP below, but instead
   // the message for this goes to the app, which bounces it here.
   void select_dafif();

// Generated message map functions
protected:
   //{{AFX_MSG(CMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnViewStatusBar();
   afx_msg void OnUpdateViewStatusBar(CCmdUI* pCmdUI);
   afx_msg void OnUpdateViewTitlebar(CCmdUI* pCmdUI);
   afx_msg void OnViewMapTabsBar();
   afx_msg void OnUpdateViewMapTabsBar(CCmdUI* pCmdUI);
   afx_msg void OnUpdateToggleVerticalView(CCmdUI *pCmd);
   afx_msg void OnFileOverlayList();
   afx_msg void OnUpdateFileOverlayList(CCmdUI* pCmd);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnOptionsFormat();
   afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg void OnDataPaths();
   afx_msg void OnDataCheck();
   afx_msg void OnSystemHealthRibbonButton();
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnViewTitlebar();
   afx_msg void OnViewPlaybackToolbar();
   afx_msg void OnUpdateViewPlaybackToolbar(CCmdUI* pCmdUI);
   afx_msg void OnViewEditorToolbar();
   afx_msg void OnUpdateViewEditorToolbar(CCmdUI* pCmdUI);
   afx_msg void OnUpdateAppAbout(CCmdUI* pCmdUI);
   afx_msg void OnClose();
   afx_msg void OnOverlayDisplayOrder();
   afx_msg void OnUpdateOverlayDisplayOrder(CCmdUI* pCmdUI);
   afx_msg void OnOverlayOptions();
   afx_msg void OnOverlays();
   afx_msg void OnHideBackgroundOverlays();
   afx_msg void OnUpdateHideBackgroundOverlays(CCmdUI* pCmdUI);
   afx_msg void OnOverlayShowalloverlays();
   afx_msg void OnOverlayManager();
   afx_msg void OnToggleVvod();
   afx_msg void OnOptionsSecurity();
   afx_msg void OnOptionsPrinting();
   afx_msg BOOL OnQueryEndSession();
   afx_msg void OnEndSession(BOOL bEnding);
   afx_msg void OnMapCurrency();
   afx_msg void OnVVodCurrency();
   afx_msg void OnVVodImporter();
   afx_msg void OnFLIPCurrency();
   afx_msg void OnFLIPImporter();
   afx_msg void OnFLIPCurrencyUpdate();
   afx_msg void OnSystemHealth();
   afx_msg void OnCustomToolsManager();
   
   afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
   
   afx_msg void OnViewtime();
   afx_msg void OnUpdateViewtime(CCmdUI* pCmdUI);
   afx_msg void OnPlaybackStart();
   afx_msg void OnUpdatePlaybackStart(CCmdUI* pCmdUI);
   afx_msg void OnPlaybackPlay();
   afx_msg void OnUpdatePlaybackPlay(CCmdUI* pCmdUI);
   afx_msg void OnPlaybackStop();
   afx_msg void OnUpdatePlaybackStop(CCmdUI* pCmdUI);
   afx_msg void OnPlaybackEnd();
   afx_msg void OnUpdatePlaybackEnd(CCmdUI* pCmdUI);

   afx_msg void OnOpenMissionPackage();

   afx_msg void OnPublishCollaboration();
   afx_msg void OnSubscribeCollaboration();
   afx_msg void OnUpdatePublishCollaboration(CCmdUI* pCmdUI);

   afx_msg void OnCalibratePixelsPerInch();

   afx_msg void OnFileSendMail();
   afx_msg void OnUpdateFileSendMail(CCmdUI* pCmdUI);

   afx_msg void OnOverlayComboBox();
   afx_msg void OnUpdateOverlayComboBox(CCmdUI* pCmdUI);
   afx_msg void OnOverlayNext();
   afx_msg void OnOverlayPrevious();
   afx_msg void OnOverlayToolbarFilterTime();
   afx_msg void OnUpdateOverlayToolbarFilterTime(CCmdUI *pCmdUI);

   afx_msg LRESULT OnDeviceChange(WPARAM nEventType, LPARAM dwData);
   //}}AFX_MSG

   afx_msg void OnEnterCHUMEditMode();

protected:
   afx_msg LRESULT OnNITFUtilityThreadEvent(WPARAM p1, LPARAM p2);

   afx_msg void OnMapProjection(UINT nID);
   afx_msg void OnUpdateMapProjection(CCmdUI* pCmdUI);

   afx_msg void OnMapMenuEntry(UINT nId);
   afx_msg void OnUpdateMapMenuEntry(CCmdUI* pCmdUI);

   afx_msg void OnToolMenuItemCOM(UINT nId);
   afx_msg void OnUpdateToolMenuItemCOM(CCmdUI* pCmdUI);

   afx_msg void OnZoomCombo();
   afx_msg void OnZoomIn();
   afx_msg void OnZoomInStep();
   afx_msg void OnZoomOut();
   afx_msg void OnZoomOutStep();
   afx_msg void OnUpdateZoom(CCmdUI* pCmdUI);
   afx_msg void On3DMapTiltSlider();
   afx_msg void OnUpdate3DMapTilt( CCmdUI* pCmdUI );
   afx_msg void OnElevationExaggerationSlider();
   afx_msg void OnElevationExaggerationEdit();
   afx_msg void OnUpdateElevationExaggeration(CCmdUI* cmd_ui);
   afx_msg void OnFavoriteList(UINT nID);
   afx_msg void OnUpdateFavoriteList(CCmdUI* pCmdUI);
   afx_msg LRESULT OnFavoritesDirectoryChanged(WPARAM, LPARAM);

   afx_msg LPARAM OnUpdateTitleBar(WPARAM wParam, LPARAM lParam);
   afx_msg LPARAM OnViewTimeChanged(WPARAM wParam, LPARAM lParam);
   afx_msg LPARAM OnMsgAsyncFix(WPARAM wParam, LPARAM lParam);

   afx_msg BOOL OnCopyData( CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct );

   /*
   *  NOTE: for some reason, the toolbar does not catch the messages
   *  associated with the foloowing handlers.  To work around this 
   *  problem, the mainframe is catching the message and calling the
   *  appropriate toolbar message handler.
   */
   afx_msg void OnCustomTB(UINT nID);
   afx_msg void OnUpdateCustomTB(CCmdUI* pCmdUI);
   afx_msg void OnOverlayEditor(UINT nID);
   afx_msg void OnUpdateOverlayEditor(CCmdUI* pCmdUI);
   afx_msg void OnRibbonElement(UINT nID);
   afx_msg void OnClickedRibbonElement(UINT nID);
   afx_msg void OnUpdateRibbonElement(CCmdUI* pCmdUI);
   afx_msg void OnUpdateCoordPane(CCmdUI* pCmdUI);
   afx_msg void OnUpdateSourcePane(CCmdUI* pCmdUI);
   afx_msg void OnStatusBarPane(UINT nID);
   afx_msg void OnUpdateStatusBarPane(CCmdUI* pCmdUI);

   afx_msg LRESULT OnMovingMapPointAdded(WPARAM wParam, LPARAM lParam);

   afx_msg void OnStatusBarSetup();

   afx_msg void OnGraphicDrawingPrefs();
   afx_msg void OnVerticalDisplayProperties();
   afx_msg LPARAM OnUIThreadOperation(WPARAM wparam, LPARAM lparam);

   afx_msg void OnCameraModePan();
   afx_msg void OnUpdateCameraModePan(CCmdUI* pCmdUI);
   afx_msg void OnCameraModeFlight();
   afx_msg void OnUpdateCameraModeFlight(CCmdUI* pCmdUI);
   afx_msg void OnCameraModeGroundView();
   afx_msg void OnUpdateCameraModeGroundView(CCmdUI* pCmdUI);

   afx_msg void OnUnderseaElevations();
   afx_msg void OnUpdateUnderseaElevations(CCmdUI* pCmdUI);

   afx_msg void OnToggle2DProjection();
   afx_msg void OnUpdateToggle2DProjection(CCmdUI* cmd_ui);
   afx_msg void OnToggle3DProjection();
   afx_msg void OnUpdateToggle3DProjection(CCmdUI* cmd_ui);

   afx_msg void OnCameraModeOptions();

   DECLARE_MESSAGE_MAP()

public:
   // these are declared public so that MapView can also call them
   afx_msg void OnOverlayManagerViewList(UINT nID);
   afx_msg void OnUpdateOverlayManagerViewList(CCmdUI* pCmdUI);
   afx_msg void OnOverlayMenuItem(UINT nID);
   afx_msg void OnUpdateOverlayMenuItem(CCmdUI* pCmdUI);
   afx_msg void OnFileOverlayMenuItem(UINT nID);
   void CommandLineOverlayOpen(CString overlayName);
   afx_msg void OnPhotoGeotaggingTool();
   afx_msg void OnTargetGraphicPrinting();

   LRESULT OnCoverageChanged(WPARAM wParam, LPARAM lParam);

   LRESULT OnMomentaryButtonPressed(WPARAM wParam, LPARAM lParam);
   LRESULT OnMomentaryButtonReleased(WPARAM wParam, LPARAM lParam);

   void OnOptionsFormat(int nInitialPage /* = 0 */);

public:
   HACCEL GetAccelTable() { return m_hAccelTable; }

   // overload WinHelp to give custom tollbar apps a chance at it
   virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

   void LaunchHtmlHelp(DWORD dwData, UINT nCmd, const char* pHelpFilename, const char *uriString);

protected:
   LRESULT OnInsertPrintMenuItems(WPARAM, LPARAM);
   LRESULT OnRemovePrintMenuItems(WPARAM, LPARAM);
   LRESULT OnEnterSizeMove(WPARAM, LPARAM);
   LRESULT OnExitSizeMove(WPARAM, LPARAM);

   int create_ribbon_bar();
   static bool CMainFrame::SortGUIDs( const GUID& a, const GUID& b ); //sorts GUIDs
   void build_application_menu();
   void build_overlay_category();
   void build_map_category();
   void build_display_category();
   void build_edit_category();
   void build_view_category();
   void build_tools_category();
   void build_data_admin_category();

   LRESULT OnErrorReport(WPARAM wParam,LPARAM lParam);
   LRESULT OnCloseInformationDialog(WPARAM wParam,LPARAM lParam);

   void DockControlBarFlushRight(CToolBar* pBar, CToolBar* pRightOf);
   void DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf);   

public:

   // Don't want to redraw the screen when a move or resize is happening
   // to the main FalconView window.
   boolean_t m_doing_size_or_move;

public:
   // Stuff for painting custom title bar:
   CFont m_fontCaption; // normal system font for active caption
   CFont m_fontAcme; // "ACME" company font (same active/inactive)
   BOOL m_bActive;  // whether active or not;
   CRect m_rcCaption;  // caption rectangle
   CBitmap m_bmCaption[2];  // bitmap captions active/inactive

public:
   // build the Map menu based on currently available map types
   void build_map_menu(CMenu *pMenu);

   // show/hide the status bar
   int ShowStatusBar(boolean_t show);

   // goto the favorite with the given index
   void goto_favorite(int index);
   static void GotoFavorite(FavoriteData* favorite);

   // load/create the mission binder with the given name.  If create_new
   // is TRUE then a new binder will be created.  Otherwise, the binder
   // will be loaded
   void open_binder(CString& binder_name, BOOL create_new);
   void open_or_new_package() { OnOpenMissionPackage(); }

   void HandleSupplementalData(CString &strDataSourcePath);
   afx_msg void OnViewNewTab();

   // Returns the item identifier with the match given item_locator_string, NULL for not found.
   UINT get_ribbon_item_id(CString item_locator_string) const;
   
   // indicate whether the menubar is currently visible or not
   boolean_t m_menubarVisible;
   //store the menu bar for redisplay
   HMENU m_storedMenubar;

   //indicate whether the titlebar is currently visible or not
   boolean_t m_titlebarVisible;

   //show/hide the title bar
   int ShowRibbonBar(boolean_t show);
   BOOL IsRibbonBarVisible();

   int SetupRibbonBarTabsRow(BOOL show_file_overlay_controls);

   void ClosePrintPreview();
   BOOL GetUnderseaElevationsEnable() const { return m_bUnderseaElevationsEnable; }

   // indicate whether 3D is enabled in the registry
   BOOL Is3DEnabledInRegistry();

private:
   // the following flag is set if the map menu needs to be rebuilt (e.g., coverage changed)
   bool m_bIsMapMenuValid;

   std::vector<GUID> m_overlay_types_on_overlays_tab;

   // view time dialog
   static CViewTime* m_view_time_dlg;
   static boolean_t m_view_time_dlg_active;

   void RunOverlayCustomInitializers();
   void RunNonOverlayCustomInitializers();

   friend class         CSliderSettleTimer;

   std::shared_ptr<CSliderSettleTimer>
      m_pElevationExaggerationSliderSettleTimer;
   CMFCRibbonSlider*    m_pElevationExaggerationSlider;
   CMFCRibbonEdit*      m_pElevationExaggerationEdit;
   int                  m_iElevationExaggerationValue;
   BOOL                 m_bUnderseaElevationsEnable;

   MapTiltSlider* m_p3DMapTiltSlider;

   // Flight mode can be toggled between locked and unlocked. The following
   // members are used to handle the UI state for this feature.
   CMFCRibbonButton* m_flight_mode_button;

}; // class CMainFrame


#endif

// End of MainFrm.h
