// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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



// mapview.h : interface of the MapView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "FalconView/CameraModes.h"
#include "FalconView/EarthManipulatorEvents_Interface.h"
#include "FalconView/include/maps_d.h"
#include "FalconView/MapParams.h"
#include "FalconView/Viewpoint3d.h"
#include "geo_tool/geo_tool_d.h"

// Forward declarations
namespace osgViewer
{
   class Viewer;
}

namespace osg
{
   class Group;
}

namespace osgEarth 
{
   class MapNode;
}


class CFVMapTabCtrl;
class CFV_dib_section;
class CFvMapViewImpl;
class CGeotagPhotosDialog;
class CHourglass;
class CMainFrame;
class CMapDoc;
class CMouseClickResource;
class C_ovl_mgr;
class DTEDTimer;
class FvGraphicsDeviceImpl;
class GC;
class GeospatialViewController;
class GeospatialViewer;
class HintText;
class MapChangeTool;
class MapRenderingEngine_Interface;
class MapEngineCOM;
class MapEngineOSG;
class MapParamsListener_Interface;
class MapProj;
class MapScale;
class MapSeries;
class MapSource;
class MapSpec;
class OverlayElements;
class SettableMapProj;
class SettableMapProjImpl;
class TipTimer;
class ViewMapProj;
class ViewMapProjImpl;
class subpage_map_list_t;
class subpage_map_list_t;
class user_print_info;
class FvCustomDropTarget;
class CSecurityLabelExtensionCOM;


typedef MapSpec map_spec_t;
typedef CList<subpage_map_list_t*, subpage_map_list_t*> page_map_list_t;

interface IFvFrameWnd;

//
// This class is used to describe characteristics of the page being printed.
//
class printer_page_info
{

public:

   //
   //  the actual printable areas for the page
   //
   double page_width_in_pixels;
   double page_height_in_pixels;
   double page_width_in_inches;
   double page_height_in_inches;

   //
   // dimensions for the desired area of page for printing to occur
   //
   double print_area_width_in_pixels;
   double print_area_height_in_pixels;
   double print_area_width_in_inches;
   double print_area_height_in_inches;

   double print_area_offset_from_top_in_inches;
   double print_area_offset_from_left_in_inches;
   int print_area_offset_from_top_in_pixels;
   int print_area_offset_from_left_in_pixels;
};

class CBaseView : public CView
{
protected:
   CPoint m_tool_tip_point;

   TipTimer *m_pTipTimer;

   // an overlay can set this flag to TRUE in its test_selected
   // function so that the tool-tip is not erased on succesive draws
   boolean_t m_tooltip_over_moving_object;

   // The following flag is used in OnMouseMove to determine if the tooltip text was set by any overlay in its
   // test_selected event handler.  This solution is not optimal, but it allows us to convert over the OnMouseMove
   // tooltip handling as well as other event handlers incrementally without breaking everything else.  Eventually,
   // this flag should not be needed at all and should be removed.
   BOOL m_bTooltipWasSet;

public:
   CBaseView();

   CPoint &GetToolTipPoint() { return m_tool_tip_point; }
   void SetToolTipPoint(CPoint point) { m_tool_tip_point = point; }

   void do_tool_tip(HintText *hint, CPoint point);
   void clear_tool_tip();
   void set_tooltip_over_moving_object() { m_tooltip_over_moving_object = TRUE; }

   // set the tooltip for the map view.  The tooltip will be displayed at the current mouse
   // position in 1/8 of second.  If the given text matches the text of a tooltip that is already 
   // being displayed and the cursor is within a certain threshold then the tooltip will not be cleared.
   // Also, if the mouse position is outside the view then no tooltip will be displayed.  
   virtual HRESULT SetTooltipText(BSTR tooltipText);
};

class CFvContextMenuImpl : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvContextMenu, &FalconViewOverlayLib::IID_IFvContextMenu, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   std::vector<FalconViewOverlayLib::IFvContextMenuItemPtr> m_menuItems;
   std::vector<FalconViewOverlayLib::IFvContextMenuItemPtr>::iterator m_currentIt;

   // IFvContextMenu
public:
   STDMETHOD(raw_AppendMenuItem)(FalconViewOverlayLib::IFvContextMenuItem *pContextMenuItem);
   STDMETHOD(raw_AppendSeparator)();

public:
   ~CFvContextMenuImpl();

BEGIN_COM_MAP(CFvContextMenuImpl)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvContextMenu)
END_COM_MAP()

   void ResetEnumerator();
   bool MoveNext();
   FalconViewOverlayLib::IFvContextMenuItemPtr m_pCrntMenuItem; 

   void ClearMenuItems();
};


// IFvMapView - interface implemented by FalconView's map view
//
interface IFvMapView
{
   // get the HWND associated with the view
   virtual HWND m_hWnd() = 0;

   // get the frame window associated with the view
   virtual IFvFrameWnd* m_pFvFrameWnd() = 0;

   // get the current map projection associated with the view
   virtual ISettableMapProjPtr m_pCrntMapProj() = 0;

   // set the cursor for the map view
   virtual HRESULT SetCursor(HCURSOR hCursor) = 0;

   // set the tooltip for the map view.  The tooltip will be displayed at the current mouse
   // position in 1/8 of second.  If the given text matches the text of a tooltip that is already 
   // being displayed and the cursor is within a certain threshold then the tooltip will not be cleared.
   // Also, if the mouse position is outside the view then no tooltip will be displayed.  
   virtual HRESULT SetTooltipText(BSTR tooltipText) = 0;

   virtual void ScrollMapIfPointNearEdge(long x, long y) = 0;
};

class MapView : 
   public CBaseView,
   public IFvMapView,
   public EarthManipulatorEvents_Interface
{
   friend GeospatialViewer;
   friend CMainFrame;

   // IFvMapView
public:
   FalconViewOverlayLib::IFvMapView *GetFvMapView();

   // get the HWND associated with the view
   virtual HWND m_hWnd();

   // get the frame window associated with the view
   virtual IFvFrameWnd* m_pFvFrameWnd();

   // get the current map projection associated with the view
   virtual ISettableMapProjPtr m_pCrntMapProj();

   // set the cursor for the map view
   virtual HRESULT SetCursor(HCURSOR hCursor);

   // set the tooltip for the map view.  The tooltip will be displayed at the current mouse
   // position in 1/8 of second.  If the given text matches the text of a tooltip that is already 
   // being displayed and the cursor is within a certain threshold then the tooltip will not be cleared.
   // Also, if the mouse position is outside the view then no tooltip will be displayed.  
   virtual HRESULT SetTooltipText(BSTR tooltipText)
   {
      return CBaseView::SetTooltipText(tooltipText);
   }

   virtual void ScrollMapIfPointNearEdge(long x, long y);

public:  

   bool GetMapInitialized() { return m_map_initialized; }
   
   void set_current_map_invalid();

   MapEngineCOM *get_map_engine() { return m_map_engine; }
   static MapEngineCOM *GetActiveViewMapEngine() { return pActiveViewMapEngine; }

   ViewMapProjImpl* get_curr_map();

   OverlayElements* CreateOverlayElements();

   // Methods used for marshaling the MapRenderingEngine interface created in
   // the main thread into background renderer threads
   IStream* PrepareMarshaledStream();

public:
   static boolean_t m_get_position_from_map;
   static int m_get_position_from_map_type;
   static int m_get_position_from_map_index;
   HintText* m_hint;

   // is a smooth scroll taking place
   static boolean_t m_scrolling;
   boolean_t m_hand_scroll;
   boolean_t m_hand_mode;
   boolean_t m_force_hand_scroll;

   // this is called when the user pressed the space bar or when the
   // smooth panning timer is triggered
   void force_smooth_pan();

   // should we print the security label (title bar / print out)
   static int m_security_label_on_title_bar;
   static CString m_current_security_class;
   static CString m_mru_standard_label;
   static CString m_custom_label;

   // store the position of the cursor when a mouse button is pressed
   CPoint m_mouse_down_point;
   double m_mouse_down_lat;
   double m_mouse_down_lon;

   BOOL m_bDisplayCenterCrosshair;
   long m_center_crosshair_handle;

   CGeotagPhotosDialog *m_pGeotagPhotosDlg;

   // Overlay elements associated with the view itself
   OverlayElements* m_overlay_elements;

   OverlayElements* GetOverlayElements();

   void ResetDtedTimer(const d_geo_t& geo);
   DTEDTimer* GetDtedTimer() const
   {
      return m_dted_timer;
   }

public:
   bool GetCursorPositionGeo(d_geo_t* cursor_pos)
   {
      *cursor_pos = m_cursor_position_geo;
      return m_cursor_position_geo_valid;
   }

   void SetCursorPositionGeo(const d_geo_t& cursor_pos, bool is_valid)
   {
      m_cursor_position_geo = cursor_pos;
      m_cursor_position_geo_valid = is_valid;
      if (is_valid)
         ResetDtedTimer(cursor_pos);
   }

   boolean_t m_user_interrupted_draw;

   UINT m_mouse_flags;

   // the view coordinates of the scrolling origin
   CPoint m_scroll_origin;
   d_geo_t m_geo_scroll_origin;
   CPoint m_old_point;

   MapChangeTool *m_map_change_tool;

   HCURSOR m_cursor;
   HCURSOR m_old_cursor;
   boolean_t m_drag;
   boolean_t m_lbutton_double_click_enable;
   boolean_t m_first_map_up;     // whether a map has been successfully displayed
   boolean_t m_zoom_enabled;     // whether zoom is enabled for the current map
   boolean_t m_move_cursor;
   boolean_t m_left_button_down;
   boolean_t m_lose_mouse_click;
   boolean_t m_popup;
   boolean_t m_need_to_redraw_after_popup;

   CComObject<CFvContextMenuImpl> *m_pContextMenu;
   CMap<UINT,UINT, FalconViewOverlayLib::IFvContextMenuItemPtr, FalconViewOverlayLib::IFvContextMenuItemPtr> m_overlay_menu_map;

   boolean_t m_new_map;

public:
   //
   // This function returns whether the rotation user interface elements
   // for the current map have been disabled.
   //
   boolean_t curr_map_rotation_ui_disabled(void) const;

   //
   // m_external_disable_rotation_ui indicates whether there is any
   // external entity (e.g. an editor) that requires rotation user interface
   // to be disabled.
   //
   int m_external_disable_rotation_ui;

   //
   // m_external_force_north_up indicates whether there is any
   // external entity (e.g. an editor) that requires the view map to be
   // displayed north-up
   //
   int m_external_force_north_up;

   //
   // indicates whether there is any external entity (e.g. an editor) that 
   // requires the view map to be displayed on Equal Arc projection
   //
   int m_external_force_equal_arc;

   //
   // This function returns whether the current map has been forced to be
   // north up.
   //
   boolean_t curr_map_is_forced_north_up(void);

   // indicates whether there is any external entity (e.g. an editor) that 
   // requires the projection user interface to be disabled.
   //

   // This function returns whether the current map has been forced to be
   // north up.
   //
   boolean_t curr_map_is_forced_to_equal_arc(void) const
   {
      return (TO_BOOLEAN_T(m_external_force_equal_arc));
   }

   int m_external_disable_projection_ui;

public:
   // need access to the state of the projection user interface from outside 
   // the view, so make it public for now

   // This function returns whether the projection user interface elements
   // for the current map have been disabled.
   //
   boolean_t curr_map_projection_ui_disabled(void) const
   {
      return (TO_BOOLEAN_T(m_external_disable_projection_ui));
   }

public:
   COleDropSource& get_drop_source() { return m_drop_source; }

protected: // create from serialization only
   MapView();
   DECLARE_DYNCREATE(MapView)

// Attributes
public:
   // Used to check if the user has interrupted the draw.
   boolean_t is_draw_interrupted();

   // Used to check if the user has interrupted the draw.  This is the same as
   // calling is_draw_interrupted(), except that it will also detect an ESC key
   // press, even if the main window is not in focus.
   boolean_t is_draw_interrupted_ex();

  // Used to check if user has pressed the escape key
   boolean_t is_escape_pressed();

   // Causes the rotation state for the view window to be re-evaluated.  If the
   // map needs to be changed to North Up, it will be.  If the user's ability to
   // rotate the map needs to be enabled or disabled, it will be.
   void refresh_rotation_state(void);

   // Causes the projection state for the view window to be re-evaluated. If the
   // map projection needs to be changed to Eqiual Arc, it will be.  If the user's 
   // ability to change the map projection needs to be enabled or disabled, it will be.
   void refresh_projection_state(void);

   // Causes the view map state for the view window to be re-evaluated. If the
   // map needs to be changed to North Up, Equal Arc, etc., it will be. If the user's 
   // ability to cjhange the map spec needs to be enabled or disabled, it will be.
   void refresh_view_map_state(void);
   
   CMapDoc *GetDocument();
   CMainFrame * GetFrame() {return (CMainFrame *)GetParentFrame();}

// Operations
public:
   void KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) { OnKeyDown(nChar, nRepCnt, nFlags); }
   void lose_mouse_click() { m_lose_mouse_click = TRUE; }

// Implementation
public:
   virtual ~MapView();
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
   
   boolean_t GetFirstMapUp(void) { return m_first_map_up; }
   void SetFirstMapUp(boolean_t first_map_up) {m_first_map_up = first_map_up;}

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

      // OLE Drag and Drop Support
   virtual void OnOLEDrag(COleDataObject* pDataObject, CPoint point);
protected:

   // OLE Drag and Drop Support
   virtual BOOL OnDrop(COleDataObject* pDataObject,
         DROPEFFECT dropEffect, CPoint point);
   virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject,
         DWORD grfKeyState, CPoint point);
   virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
         DWORD grfKeyState, CPoint point);
   virtual void OnDragLeave();
   // End OLE Drag and Drop Support

   virtual BOOL PreCreateWindow(CREATESTRUCT&);
   virtual void OnInitialUpdate();

   virtual void OnUpdate(CView *, LPARAM lHint=0, CObject * pHint=NULL);
   virtual void OnPrepareDC(CDC * pDC, CPrintInfo * pInfo = NULL);
   virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
   virtual void OnPrint(CDC * pDC, CPrintInfo* pInfo);
   virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
   virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
   afx_msg void OnFilePrintPreviewEx();
   void TurnOffIn3d(CCmdUI* pCmdUI);

   BOOL m_bMapTabsBarHiddenDuringPrint;
   BOOL m_bMapViewTabsBarHiddenDuringPrint;

   // Advanced: end print preview mode, move to point
   virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point,
      CPreviewView* pView);

   afx_msg void OnMapOptions();
   afx_msg void OnMapGroups();
   afx_msg void OnUpdateMapGroups(CCmdUI* pCmdUI);
   
   afx_msg void OnScalePercentZoom();
   afx_msg void OnUpdateScalePercentZoom(CCmdUI* pCmdUI);
   afx_msg void OnScaleZoom();
   afx_msg void OnUpdateScaleZoom(CCmdUI* pCmdUI);
   afx_msg void OnHandMode();
   afx_msg void OnUpdateHandMode(CCmdUI* pCmdUI);
   afx_msg void OnOverlayPopup(UINT nId);
   afx_msg void OnOverlayPopupUpdate(CCmdUI* pCmdUI);
   afx_msg void OnEditCopyMap();
   afx_msg void OnEditCopyMapToKMZ();
   afx_msg void OnEditCopyMapToGeoTIFF();
   afx_msg void OnEditCopyMapToJPG();
   afx_msg void OnEditCopyMapToPNG();
   afx_msg void OnEditTargetGraphicPrinting();
   afx_msg void OnUpdateEditCopyMapDisableRotated(CCmdUI* pCmdUI);
   afx_msg LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
   afx_msg void OnUndo();
   afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
   afx_msg void OnRedo();
   afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);

   afx_msg void OnCoordCopy();
   afx_msg void OnEditCopy();
   afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
   afx_msg void OnEditPaste();
   afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);

   afx_msg LRESULT OnInvalidateFromThread(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnInvalidate(WPARAM, LPARAM);

   // publicly accessible message handlers
public:
   afx_msg void OnCenter();
   afx_msg void OnScaleIn();
   afx_msg void OnScaleOut();
   afx_msg void OnMapRotateNorthUp();

// Generated message map functions
protected:
   //{{AFX_MSG(MapView)
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnBright();
   afx_msg void OnDim();
   afx_msg void OnUpdateBright(CCmdUI* pCmdUI);
   afx_msg void OnUpdateDim(CCmdUI* pCmdUI);
   afx_msg void OnMapPanUpdate(CCmdUI* cmd);
   afx_msg void OnMapPanDown();
   afx_msg void OnMapPanLeft();
   afx_msg void OnMapPanRight();
   afx_msg void OnMapPanUp();
   afx_msg void OnMapRotateClockwise();
   afx_msg void OnMapRotateCounterclockwise();
   afx_msg void OnMapLockCurrentMap();
   afx_msg void OnUpdateMapLockCurrentMap(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMapRotateNorthup(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMapRotateCounterclockwise(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMapRotateClockwise(CCmdUI* pCmdUI);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnMapNew();
   afx_msg void OnDestroy();
   
   afx_msg void OnFilePrintPreview();
   afx_msg void OnFilePrintDirect();

   afx_msg void OnFilePrint();
   
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnIncreaseContrast();
   afx_msg void OnUpdateIncreaseContrast(CCmdUI* pCmdUI);
   afx_msg void OnDecreaseContrast();
   afx_msg void OnUpdateDecreaseContrast(CCmdUI* pCmdUI);
   afx_msg void OnViewAdjustBrightContrast();
   afx_msg void OnViewDecreaseContrastCenterValue();
   afx_msg void OnViewIncreaseContrastCenterValue();
   afx_msg void OnUpdateViewAdjustBrightContrast(CCmdUI* pCmdUI);
   afx_msg void OnViewNormalBrightContrast();
   afx_msg void OnUpdateViewNormalBrightContrast(CCmdUI* pCmdUI);
   afx_msg void OnViewAutoBrightContrast();
   afx_msg void OnUpdateViewAutoBrightContrast(CCmdUI* pCmdUI);
   afx_msg void OnViewCalcBrightContrast();
   afx_msg void OnUpdateViewCalcBrightContrast(CCmdUI* pCmdUI);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   
   afx_msg void OnMainPrintDirect();
   afx_msg void OnMainPrintDirectUpdate(CCmdUI* pCmdUI);
   afx_msg void OnMainPrintPreview();
   afx_msg void OnMainPrintPreviewUpdate(CCmdUI* pCmdUI);
   afx_msg void OnMainPrint();
   afx_msg void OnMainPrintUpdate(CCmdUI* pCmdUI);

   afx_msg void OnToggleCenterCrosshair();
   afx_msg void OnUpdateCenterCrosshair(CCmdUI* pCmdUI);

   afx_msg void OnSearchBar();

   //}}AFX_MSG

   afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM lParam);
   afx_msg void OnOverlay(UINT nId);
   afx_msg void OnUpdateOverlay(CCmdUI* pCmdUI);

   // Favorites->Add to Favorites
   afx_msg void OnFavoritesAdd();

   // Favorites->Organize Favorites
   afx_msg void OnFavoritesOrganize();
   afx_msg void OnUpdateFavoritesList(CCmdUI* pCmdUI);
   afx_msg void OnUpdateOverlayList(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMapMenu(CCmdUI* pCmdUI);

   afx_msg void OnSetFocus(CWnd* wndx);
   afx_msg void OnKillFocus(CWnd* wndx);

   afx_msg LRESULT OnInvalidateLayerOverlay(WPARAM, LPARAM lParam);

   DECLARE_MESSAGE_MAP()

   // Defines additional command message handlers that are processed
   // in OnCmdMsg. This is used to effectively chain a series of
   // MFC message maps, allowing us to clean up the MapView message
   // handler.
   //
   // Use the CHAINED_MAPVIEW_MESSAGE_MAP macro, defined in this file,
   // to add yourself to this list.
   static std::vector<CCmdTarget* > s_command_message_handlers;

   // use to flag menuIDs that should be disabled or enabled
   std::vector<UINT> m_nDisabledMenuIDs;
   std::vector<UINT> m_nCheckedMenuIDs;

   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
      AFX_CMDHANDLERINFO* pHandlerInfo) override;

public:
   static void AddCommandMessageHandler(CCmdTarget* message_handler);

public:
   // Define the inclusive range of zoom percentages supported by the MapView
   enum
   {
      MIN_ZOOM_PERCENT = 6,
      MAX_ZOOM_PERCENT = 800
   };

   static const int PAN_DIRECTION_UP = 1;
   static const int PAN_DIRECTION_DOWN = 2;
   static const int PAN_DIRECTION_LEFT = 4;
   static const int PAN_DIRECTION_RIGHT = 8;

   // MapParams manipulation
   //
   int ChangeToClosestScale(long group_identity);
   int ChangeToClosestScale(const MapCategory& cat);
   int ChangeToClosestScale(const MapCategory& cat, const d_geo_t& center);
   int ChangeToClosestScale(const MapCategory& category,
      const MapScale& scale, const MapProjectionParams& map_proj_params);

   int ChangeToClosestMapType(long group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params);

   int ChangeMapType(long group_identity, const MapType& map_type);
   int ChangeMapType(long group_identity, const MapType& map_type,
      const d_geo_t& center);
   int ChangeMapType(long group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      boolean_t override_no_data = FALSE);

   void ChangeRotation(double rotation);

   int ChangeCenter(const d_geo_t& center);
   int ChangeCenter(const degrees_t& lat, const degrees_t& lon);

   int ChangeCenterAndRotation(const d_geo_t& center, double rotation);
   int ChangeCenterAndRotation(const degrees_t& lat, const degrees_t& lon,
      double rotation);

   int ChangeZoomPercent(int zoom_percent);
   int ChangeZoomPercent(int zoom_percent, const d_geo_t& center);
   void ChangeZoomPercentByStep(int step_size);
   void ChangeZoomPercentByStep(int step_size, const d_geo_t& center);

   int ChangeTilt(double tilt_degrees);
   int NotifyTiltUpdate(double tilt_degrees);

   int ChangeToSmallestScale(const MapCategory& category,
      const MapProjectionParams& map_proj_params);
   int ChangeToLargestScale(const MapCategory& category,
      const MapProjectionParams& map_proj_params);

   int ChangeToBest(const degrees_t& lat, const degrees_t& lon);
   int ChangeToBest(const MapSource& source, const MapScale& scale,
      const MapProjectionParams& map_proj_params);

   // MapView camera manipulation
   //

   struct AttachedCameraParams
   {
      double lat;
      double lon;
      double alt_meters;
      double heading;
      double pitch;
      double roll;
   };

   // Set camera_events to nulltpr to Detach camera
   void AttachToCamera(FalconViewOverlayLib::ICameraEvents* camera_events);
   void SetCameraPositionWhenAttached(
      const AttachedCameraParams& camera_params);
   
   // Returns camera parameters with the current camera position if the camera
   // is attached to a moving object. Otherwise, returns nullptr.
   AttachedCameraParams* GetCameraPositionIfAttached();


   // The following update methods should only be called by the 3D earth
   // manipulator.
   void UpdateCenter(const d_geo_t& center);
   void UpdateRotationAndTilt(double rotation, double tilt_degrees);

   int ScaleIn();
   int ScaleIn(const d_geo_t& center);
   int ScaleOut();
   int ScaleOut(const d_geo_t& center);

   void ChangeProjectionType(ProjectionEnum proj_type);

   void PanMap(int pan_dir);

   fvw::CameraMode GetCameraMode() const;
   void SetCameraMode(fvw::CameraMode camera_mode);
   void ToggleEnableFlightModeAutoMapType();
   bool GetEnableFlightModeAutoMapType();

   void AutoBrightContrast();
   void SetBrightness(double brightness);

   // MapParams accessors
   //
   long GetMapGroupIdentity() const
   {
      return m_map_params.GetMapGroupIdentity();
   }
   const MapType& GetMapType() const
   {
      return m_map_params.GetMapType();
   }
   const MapProjectionParams& GetMapProjParams() const
   {
      return m_map_params.GetMapProjParams();
   }
   ProjectionEnum GetProjectionType() const
   {
      return m_map_params.GetMapProjParams().type;
   }
   const MapDisplayParams& GetMapDisplayParams() const
   {
      return m_map_params.GetMapDisplayParams();
   }

   // MapParams change notification
   //
   void AddMapParamsListener(MapParamsListener_Interface* listener);
   void RemoveMapParamsListener(MapParamsListener_Interface* listener);

   bool IsProjectionSupported(ProjectionEnum projID) const
   {
      if (projID == GENERAL_PERSPECTIVE_PROJECTION)
         return m_3D_rendering_supported;

      return true;
   }

   double GetBrightness() const
   {
      return m_map_params.GetMapDisplayParams().brightness;
   }
   double GetContrast() const
   {
      return m_map_params.GetMapDisplayParams().bw_contrast;
   }
   void SetContrast(double brightness, double contrast, int contrast_midval);

   CMouseClickResource* m_mouse_click_resource;

   void OnMapError(int map_error);

   void SetViewCursor(HCURSOR hCursor);

   void set_cursor(HCURSOR cursor) { m_cursor = cursor; }
   void update_title_bar();
   HCURSOR get_cursor() { return m_cursor; }

   void do_CAPS_print(CList<CString*, CString*>* CAPS_lines,
      boolean_t print_to_scale, boolean_t show_map_info);

   //
   // Use these functions rather than the CWnd::Invalidate... ones
   // for invalidating the view.  These are needed to support "animation mode".
   // These are neccesary because the CWnd::Invalidate functions are not
   // virtual (and because we require a greater degree of control - and thus
   // more parameters - than the CWnd::Invalidate functions allow for).
   //
   // allow_animation_override is used to allow the invalidate_view... functions
   // to "override" the bErase value passed in when in animation mode on 
   // a paletted (e.g. 256-color) device.  Only set allow_animation_override
   // to TRUE when the invalidate_view... functions should be allowed to override the
   // the bErase values (e.g. it's ok to change bErase from TRUE to FALSE).
   // This is needed so that, when in animation mode in 256 color mode, you don't
   // have to erase the screen when you (for example) pan the map, because in
   // "normal" (non-animation) mode, the invalidate call in this case
   // specifies bErase=TRUE.
   //
   // redraw_overlays_from_scratch_when_animating is used to distinguish between
   // the two types of refresh cases when you are animating:
   // 1) you want to refresh the entire map at once (i.e. not redraw the
   //    overlays from scratch)
   // 2) you want to refresh the base map only and redraw the overlays from scratch
   //    (i.e. have it work the same way as when you are in non-animation mode).
   //    This behavior is needed, for example, when an overlay changes.
   //
   void invalidate_view(BOOL bErase = TRUE, BOOL allow_animation_override=FALSE,
      BOOL redraw_overlays_from_scratch_when_animating=FALSE);
   void invalidate_view_rect(LPCRECT lpRect, BOOL bErase = TRUE,
      BOOL allow_animation_override=FALSE,
      BOOL redraw_overlays_from_scratch_when_animating=FALSE);
   void invalidate_view_rgn(CRgn* pRgn, BOOL bErase = TRUE,
      BOOL allow_animation_override=FALSE,
      BOOL redraw_overlays_from_scratch_when_animating=FALSE);
   void redraw_map_from_scratch();

   void HandleSearchRequest(const CString& searchRequest);

   int InitializeMap();

   inline bool IsRendering3d(ProjectionEnum projection)
   {
      return projection == GENERAL_PERSPECTIVE_PROJECTION;
   }
   inline bool IsRendering3d()
   {
      return IsRendering3d(GetProjectionType());
   }
   inline bool IsRendering2d()
   {
      return !IsRendering3d();
   }

protected:

   int set_initial_requested_map_in_view_to_the_saved_map(map_spec_t* registry_map_spec);

   //
   // helper function for the invalidate_view... functions
   //
   void calc_erase_value_for_invalidation(BOOL& bErase, 
      BOOL allow_animation_override);

   //
   // map drawing functions
   //
   virtual void draw(CDC* pDC);

   void DrawCenterCrosshair(CDC *pDC, int nCenterX, int nCenterY);

   // printing functions
   //
   int on_print_page(const subpage_map_list_t* page_map_list, CDC* dc, 
      user_print_info* custom_info, boolean_t is_preview, MapEngineCOM *map_engine);
   
   int on_print_subpage_map(CDC* dc, user_print_info* custom_info,  
      const SettableMapProj* requested_map, boolean_t is_print_preview, 
      printer_page_info &page_info, boolean_t print_map_type, boolean_t print_map_info,
      boolean_t print_date_and_time, boolean_t print_echum_currency,
      boolean_t print_dafif_currency, boolean_t print_cadrg_currency, 
      boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up,
      MapEngineCOM *map_engine);

   int fvw_set_printing_clipping_region(CDC* dc, double dstart_x, double dstart_y,
      double dend_x, double dend_y);

   int fvw_print_page(CDC* dc, CFont* font, printer_page_info& page_info, 
      boolean_t print_any_labels,
      SettableMapProj* req_map, boolean_t print_to_scale,
      boolean_t enhance_cib_contrast,
      boolean_t show_map_info, boolean_t show_map_type,
      boolean_t print_cadrg_currency, boolean_t print_dafif_currency,
      boolean_t print_echum_currency, boolean_t print_date_time, 
      boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up,
      MapEngineCOM *map_engine);
   
   int print_to_printer_dc_to_scale(CDC* dc, printer_page_info& page_info,
      SettableMapProj* req_map, MapEngineCOM* map_engine, 
      boolean_t collect_nima_dted_info, 
      boolean_t collect_rpf_info, boolean_t collect_rpf_currency_info,
      boolean_t enhance_cib_contrast);
   int print_to_printer_dc_wysiwyg(CDC* dc, printer_page_info& page_info,
      SettableMapProj* req_map, MapEngineCOM* map_engine,
      boolean_t collect_nima_dted_info, 
      boolean_t collect_rpf_info, boolean_t collect_rpf_currency_info,
      boolean_t enhance_cib_contrast);
   int print_to_printer_dc(CDC* dc, printer_page_info& page_info, SettableMapProj *req_map,
      MapEngineCOM* map_engine,
      boolean_t collect_nima_dted_info, 
      boolean_t collect_rpf_info, boolean_t collect_rpf_currency_info,
      boolean_t enhance_cib_contrast, boolean_t is_printing_to_scale);
   int set_the_to_scale_mapping(CDC* dc, double dwindow_width, double dwindow_height,
      double dviewport_width, double dviewport_height);
   
   //
   // labeling static functions
   //
   int fvw_print_labels(CDC* dc, CFont* font, printer_page_info& page_info,
      const MapProj* curr_map, const MapProj* mosaic_map, 
      boolean_t show_map_type, boolean_t show_scale, boolean_t show_projection, 
      boolean_t print_rpf_info, boolean_t print_nima_dted_info,
      boolean_t print_cadrg_currency, boolean_t print_dafif_currency, 
      boolean_t print_echum_currency, boolean_t print_date_time,
      boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up, 
      C_ovl_mgr* overlay_manager);

   int draw_map_info_caption(const MapProj* curr_map, const MapProj* mosaic_map,
      CDC* dc, const CString& caption, const CSize& text_size,
      int bottom_row_of_current_caption, const printer_page_info& page_info);
   int print_rpf_map_dates(const MapProj* curr_map, const MapProj* mosaic_map,
      CDC* dc, CFont* font, printer_page_info& page_info,
      int vertical_offset);
   int print_nima_dted_map_dates(const MapProj* curr_map, const MapProj* mosaic_map,
      CDC* dc, CFont* font, printer_page_info& page_info, 
      int vertical_offset);
   int print_currency_text(CDC* dc, CFont* font, 
      const MapProj* curr_map, const MapProj* mosaic_map,
      printer_page_info& page_info,
      boolean_t print_date_time, const CString& cadrg_currency,
      const CString& dafif_currency, const CString& echum_currency,
      const CString& vvod_currency, int vertical_offset);
   void get_mosaic_map_dims_in_printer_pixels(const MapProj* mosaic_map,
      const printer_page_info& component_map_page_info,
      double* mosaic_map_width_in_pixels, double* mosaic_map_height_in_pixels);
   void get_map_type_caption(const MapProj* map, CString& caption, 
      boolean_t include_scale);
   int print_map_label(CDC* dc, CFont* font, const MapProj* curr_map, 
      const MapProj* mosaic_map, 
      boolean_t show_map_type, boolean_t show_scale, boolean_t show_projection,
      printer_page_info& page_info, int vertical_offset);
   int print_currency_info(CDC* dc, CFont* font, 
      const MapProj* curr_map, const MapProj* mosaic_map, 
      printer_page_info& page_info, MapSource* source, MapScale* scale,
      boolean_t get_cadrg_currency, boolean_t get_dafif_currency, 
      boolean_t get_echum_currency, boolean_t print_date_time, C_ovl_mgr* overlay_manager,
      int vertical_offset);
   int get_currency_text_for_printing(const MapProj* map_proj, 
      MapSource* source, MapScale* scale, boolean_t get_cadrg_currency, 
      boolean_t get_dafif_currency, boolean_t get_echum_currency,
      C_ovl_mgr* overlay_manager,
      CString* cadrg_currency_text, CString* dafif_currency_text,
      CString* echum_currency_text);
   
   int print_security_label(CDC* dc, CFont* font, const MapProj* curr_map, 
      const MapProj* mosaic_map, const printer_page_info& page_info, 
      int* vertical_size);
   CString get_scale_string(MapScale scale, int surface_scale_percent, 
      double surface_scale_denom);

   int draw_CAPS_info(CDC* dc, printer_page_info& page_info, CFont* CAPS_font,
      int CAPS_font_height_in_pixels, const CList<CString*, CString*>* CAPS_lines);

   int initialize_CAPS_vars(CDC* dc, user_print_info* custom_info,
      int* required_CAPS_vertical_size_in_pixels);

   void setup_custom_print_dialog(CPrintInfo* pInfo);
   
   int setup_custom_info(user_print_info* info, boolean_t print_to_scale,
      boolean_t show_map_info, boolean_t show_map_type,
      boolean_t show_date_time, boolean_t show_cadrg_currency,
      boolean_t show_dafif_currency, boolean_t show_echum_currency,
      boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up,
      boolean_t enhance_cib_contrast, short default_page_orientation,
      boolean_t CAPS_printing, CList<CString*, CString*>* CAPS_lines);

   int print_a_map_and_its_labels(CDC* dc, SettableMapProj* map,  boolean_t is_mosaic_map,
      user_print_info* custom_info, CFont* font, printer_page_info& page_info, 
      boolean_t print_to_scale, boolean_t enhance_cib_contrast, 
      boolean_t default_print_map_type, boolean_t default_print_map_info, 
      boolean_t default_print_date_and_time, boolean_t default_print_echum_currency,
      boolean_t default_print_dafif_currency, boolean_t default_print_cadrg_currency,
      boolean_t default_print_compass_rose, 
      boolean_t default_print_compass_rose_when_not_north_up,
      MapEngineCOM *map_engine);

   int print_mosaic_map_labels_on_a_component_map(CDC* dc, MapProj* map,
      user_print_info* custom_info, CFont* font, printer_page_info& page_info,
      boolean_t print_to_scale);
   
   // print the compass rose
   int print_the_compass_rose( CDC *pDC, const MapProj *curr_map, 
      const MapProj* mosaic_map, printer_page_info &page_info,
      boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up );

protected:

   boolean_t m_is_drawing;  // indicates if in the middle of OnDraw

   //
   // ---- printing (and clipboard) members and functions ----
   //

   // for "controlling" access to the graphics dc - making sure that
   // the screen is not updated while printing
   boolean_t m_printing;  // indicates that printing is occurring
   boolean_t m_refresh_after_printing; // invalidate the screen after 
                                       // printing if TRUE

   // at the beginning of printing the default value is obtained from the
   // print info object.  The value is restored at the end of printing since
   // the print info object's value may have been altered (since we allow 
   // landscape and portrait mixed)
   short m_default_page_orientation;
   
   //
   // the list of requested maps for a strip chart
   //
   page_map_list_t *m_strip_page_map_list;

   //
   // CAPS printing info
   //
   // most of these variables are simply a way to hold info to pass 
   // to the printing functions
   //
   boolean_t m_CAPS_printing; // indicates the CAPS is printing (and hence
                              // that the info in m_CAPS_lines should
                              // be included on the printout)
   CList<CString*, CString*>* m_CAPS_lines;
   boolean_t m_CAPS_print_to_scale;
   boolean_t m_CAPS_show_map_info;

public:

// static void show_near_airways(ViewMapProj *map, C_icon* pt);
// static void remove_near_airways(ViewMapProj *map, C_icon* pt);

   static void MapContextMenuCallback(CString strMenuItemText, LPARAM);

   // This is temporary until the GeospatialViewController and MapView are
   // merged
   GeospatialViewController* GetGeospatialViewController() const
   {
      return m_geospatial_view_controller;
   }

   void UpdateZoomFromCombo(CMFCRibbonComboBox* pCombo);

public:
   // EarthManipulatorEvents_Interface methods
   void DistanceChanged(double distance_meters) override {}

private:

   bool IsCursorInView();
   
   void FreeOverlayMenu();

   // Changes full path-names on the "Links" sub-menu to be just
   // the filename, unless there would be duplicates, then it leaves
   // them as the full path-name
   void do_special_handling_for_links_submenu(CMenu &links_menu);

   // Menu, multiple level sub menus, this is a recursive method to add multiple
   // layers of submenus for the context menu.
   bool AddMenuItem(HMENU hTargetMenu, const CString& itemText, UINT itemID, 
                    FalconViewOverlayLib::IFvContextMenuItemPtr spMenuItem);

   void ResizeOrInvalidateView(bool changed_proj_type);

private:
   //
   // This call is to be used to enable an "external" entity (e.g. an overlay)
   // to force the map be north-up (and to then reset this behavior).  A value of 
   // TRUE for enable increments the counter that keep track of how many external 
   // entities have rotation disabled and cause the map to be redrawn from scratch. 
   // A value of FALSE for enable decrements the counter.
   //
   void external_enable_force_north_up_map(boolean_t force_north_up);

   //
   // This call is to be used to enable an "external" entity (e.g. an overlay)
   // to disable the rotation user interface (and to then reset this behavior).  
   // A value of TRUE for disable increments the counter that keep track of how many 
   // external entities have the rotation UI disabled.  A value of FALSE for disable 
   // decrements the counter.
   //
   void external_disable_rotation_user_interface(boolean_t disable);

   //
   // enable_rotation_user_interface enables or disables the view
   // user interface capabilities for rotation.  In other words, it controls 
   // whether, for example, rotation menu items (and associated accelerator keys)
   // are enabled or disabled.
   //
   void enable_rotation_user_interface(boolean_t enable);


   // This call is to be used to enable an "external" entity (e.g. an overlay)
   // to force the map be Equal Arc (and to then reset this behavior).  A value of 
   // TRUE for enable increments the counter that keep track of how many external 
   // entities have projection disabled and cause the map to be redrawn from scratch. 
   // A value of FALSE for enable decrements the counter.
   //
   void external_enable_equal_arc_projection(boolean_t enable);

   // This call is to be used to enable an "external" entity (e.g. an overlay)
   // to disable the projection user interface (and to then reset this behavior).  
   // A value of TRUE for disable increments the counter that keep track of how many 
   // external entities have the rotation UI disabled.  A value of FALSE for disable 
   // decrements the counter.
   //
   void external_disable_projection_user_interface(boolean_t disable);

   void InitializeMapRenderingEngines();
   MapRenderingEngine_Interface* GetMapRenderingEngineFromProjection(
      ProjectionEnum projection_type) const;

   // Sets the MapView's current rendering. If the current rendering engin
   // was changed by this method then this method will return true.
   bool SetCurrentRenderingEngineFromProjection(ProjectionEnum projection_type, 
      bool allow_camera_change = true);

   // Updates the brightness or contrast parameters based on whether the
   // current MapType is black and white or not
   void UpdateContrast();

   // Validates that i) zoom is enabled and ii) the current zoom plus the
   // given step_size falls within the valid range of zoom percentages. Returns
   // true if this is the case, false otherwise.
   bool ValidateDeltaZoomPercent(int step_size, int* new_zoom_percent);

   DTEDTimer *m_dted_timer;

   // OLE Drag and Drop Support
   COleDropTarget m_drop_target;
   COleDropSource m_drop_source;

   // The current map rendering engine for this MapView
   MapRenderingEngine_Interface* m_current_map_rendering_engine;
   MapRenderingEngine_Interface* m_map_rendering_engines[2];
   
   // A flag set based on the current hardware we are running on
   bool m_3D_rendering_supported;

   // Contains the collection of bindable parameters used by this
   // MapView to render a map.
   MapParams m_map_params;

   // Boolean that is set true/false on windows messages when
   // MapView gains/loses focus
   bool m_has_focus;

   // Information about the entity attached to the camera, if one exists.
   // See AttachToCamera, DetachFromCamera, and SetCameraPositionWhenAttached
   struct
   {
      // Contains a pointer to the camera events of the attached entity
      FalconViewOverlayLib::ICameraEventsPtr camera_events;

      // The current camera parameters of the attached entity
      AttachedCameraParams camera_params;
   } m_attached_entity;

   // The GeospatialViewController provides methods to allow the user to
   // manipulate the GeospatialScene and handles user interface events.
   GeospatialViewController* m_geospatial_view_controller;

   MapEngineCOM *m_map_engine;
   ViewMapProjImpl *m_view_map_proj;

   static MapEngineCOM *pActiveViewMapEngine;

   CComObject<CFvMapViewImpl> *m_pFvMapView;

   // false until after InitializeMap is called
   bool m_map_initialized;

   // If the mouse is within this MapView, the current position on the mouse
   // in geographical coordinates. The cursor position can be invalid if it
   // is outside a valid point on the map (can happen in the general perpective
   // projection).
   d_geo_t m_cursor_position_geo;
   bool m_cursor_position_geo_valid;

   // Security extensions permit customizable security labeling on 
   // printed materials.  In particular, JMPS uses its own security 
   // downgrade mechanism.
   CSecurityLabelExtensionCOM* m_pSecurityLabelMgr;

// Gesture handlers
protected:
   double m_dblAngleStart;
   LONG m_lZoomValueStart;
   LONG m_lGestureWidthStart;
   bool m_bInRotate;
   bool m_bInPan;
   bool m_bInZoom;

   // The method is called upon gesture zoom event</summary>
   virtual BOOL OnGestureZoom(CPoint ptCenter, long lDelta);

   // The method is called upon gesture pan event</summary>
   virtual BOOL OnGesturePan(CPoint ptFrom, CPoint ptTo);

   // The method is called upon gesture rotate event</summary>
   virtual BOOL OnGestureRotate(CPoint ptCenter, double dblAngle);

   // The method is called upon gesture 2 finger tap event</summary>
   virtual BOOL OnGestureTwoFingerTap(CPoint ptCenter);

   // The method is called upon gesture press and tap event</summary>
   virtual BOOL OnGesturePressAndTap(CPoint ptPress, long lDelta);

private:
   FvCustomDropTarget* m_customDropTarget;
};

// MapChangeTool - base class for the MapScaleTool and the MapZoomTool
// classes
//
class MapChangeTool
{
public:
   // constructor
   MapChangeTool(MapView *view);

   virtual boolean_t is_kind_of(CString s);
   virtual CString get_help_text() = 0;
   virtual void on_left_button_down(ViewMapProjImpl *map, CPoint point, UINT nFlags);
   virtual void on_mouse_move(ViewMapProjImpl *map, CPoint point, UINT nFlags, HCURSOR& cursor);

   virtual void on_left_button_up(ViewMapProjImpl *map, CPoint point, UINT nFlags) = 0;

   virtual void on_key_down(ViewMapProjImpl *map, CPoint point, UINT nFlags) = 0;
   virtual void on_key_up(ViewMapProjImpl *map, CPoint point, UINT nFlags) = 0;

   void cancel_drag(ViewMapProjImpl *map);

protected:
   CRect m_zoom_window_rectLast;
   MapView *m_map_view;

   // are we currently dragging a region
   boolean_t m_zoom_window_drag;
   CPoint m_zoom_window_ptReference;

   CPoint m_zoom_tool_point;


protected:
   // return the center of the region dragged in geo coordinates
   d_geo_t get_center_geo(ViewMapProjImpl *map, CPoint point);

   // return the upper right coordinate of the region dragged 
   // in geo coordinates
   d_geo_t get_upper_right_geo(ViewMapProjImpl *map);
};

// MapScaleTool - this class implements the scale tool
//
class MapScaleTool : public MapChangeTool
{
public:
   // constructor
   MapScaleTool(MapView *view);

   virtual boolean_t is_kind_of(CString s);
   virtual CString get_help_text();


   virtual void on_left_button_up(ViewMapProjImpl *map, CPoint point, UINT nFlags);
   virtual void on_mouse_move(ViewMapProjImpl *map, CPoint point, UINT nFlags, HCURSOR& cursor);

   virtual void on_key_down(ViewMapProjImpl *map, CPoint point, UINT nFlags);
   virtual void on_key_up(ViewMapProjImpl *map, CPoint point, UINT nFlags);

private:
   // calculates a new scale for the scale tool
   int calc_next_scale(ViewMapProjImpl *map, UINT nFlags, MapSource &mapSource, MapScale &mapScale,
      MapSeries &mapSeries, boolean_t force_next = FALSE);

   void show_scale_tool_tooltip(ViewMapProjImpl *map, CPoint point, UINT nFlags, boolean_t force_next = FALSE);

   int m_map_group_identity;
};

// MapZoomTool - this class implements the zoom tool
//
class MapZoomTool : public MapChangeTool
{
public:
   // constructor
   MapZoomTool(MapView *view);

   virtual boolean_t is_kind_of(CString s);
   virtual CString get_help_text();
   virtual void on_left_button_up(ViewMapProjImpl *map, CPoint point, UINT nFlags);
   virtual void on_mouse_move(ViewMapProjImpl *map, CPoint point, UINT nFlags, HCURSOR& cursor);

   virtual void on_key_down(ViewMapProjImpl *map, CPoint point, UINT nFlags);
   virtual void on_key_up(ViewMapProjImpl *map, CPoint point, UINT nFlags);

private:

   // calculates a new zoom percent for the zoom tool
   int calc_zoom_percent(ViewMapProjImpl *map, CPoint point, UINT nFlags, int& new_zoom_percent);

};

#ifndef _DEBUG // debug version in mapview.cpp
inline CMapDoc* MapView::GetDocument()
   { return (CMapDoc*) m_pDocument; }

#endif

// Use the CHAINED_MAPVIEW_MESSAGE_MAP macro to define an CCmdTarget derived
// class used to process command messages in MapView's OnCmdMsg.
//
#define CHAINED_MAPVIEW_MESSAGE_MAP(class_name) \
class class_name##Instance \
{ \
public: \
   class_name##Instance() \
   { \
      static class_name handler; \
      MapView::AddCommandMessageHandler(&handler); \
   } \
} s_##class_name;
