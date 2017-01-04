// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
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




// LayerOvl.h

#ifndef LAYEROVL_H
#define LAYEROVL_H

#include "overlay.h"
#include "ovlelem.h"
//#include "ovlkit.h"
#include "file.h"
#include "err.h"
#include "maps_d.h"
#include "icallback.h"
#include "factory.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "FvMappingGraphics\Include\FvMappingGraphicsInclude.h"

class C_ovlkit_object;
class ObjectGroup;

// CBaseLayerOvl - contains all the common functionality required for both CStaticLayerOvl and CLayerOvl
//
class CBaseLayerOvl : 
   public OverlayTypeOverrides_Interface,
   public PlaybackEventsObserver_Interface,
   public PlaybackTimeSegment_Interface
{
public:
   CBaseLayerOvl(const DWORD owning_thread_id, C_overlay *overlay);
   ~CBaseLayerOvl();

   // OverlayTypeOverrides_Interface
public:
   // Override the display name defined in the overlay's factory.  This is used to support
   // per-instance overlay display names (e.g., an overlay created via the ILayer::CreateLayer method)
   HRESULT CBaseLayerOvl::get_m_displayName(BSTR *pVal);

   // This relative icon will override the icon defined in the overlay's factory.  This
   // is used to support per-instance overlay icons (e.g., an overlay created via the
   // ILayer::CreateLayerEx method).  This icon is displayed in the overlay manager's
   // current list of opened overlays.  The filename can either be the full specification
   // or a path relative to HD_DATA\icons.
   HRESULT get_m_iconFilename(BSTR* pVal);

   // PlaybackEventsObserver_Interface
public:
   virtual HRESULT OnPlaybackStarted()
   {
      return S_OK;
   }
   virtual HRESULT OnPlaybackStopped()
   {
      return S_OK;
   }
   virtual HRESULT OnPlaybackTimeChanged(DATE dateCurrentTime);
   virtual HRESULT OnPlaybackRateChanged(long lNewPlaybackRate)
   {
      return S_OK;
   }

   void set_view_time_enabled(boolean_t enable) { m_view_time_enabled = enable; }
   boolean_t get_view_time_enabled() { return m_view_time_enabled; }

protected:
   C_overlay *m_pOverlay;

   // overlay display name.  If empty, we will use the overlay type's display name
   CString m_displayName;

   // icon name displayed by overlay manager
   CString m_icon_name; 

   LayerEditorImpl *m_layer_editor;

   // should the layer be hidden
   boolean_t m_hide_layer;

   // the viewing threshold of this layer
   MapScale m_threshold;

   // dispatch pointer to the client's callback object
   IDispatch *m_pdisp;

   HintText m_hint;

   // object id that is currently being dragged
   int m_drag_object_id;

   C_ovlkit_object *m_cicon;

   boolean_t m_view_time_enabled;

   BOOL m_bDragging;

protected:
   int draw(ActiveMap* map);
   int draw_to_base_map(ActiveMap* map);
   BOOL can_add_pixmaps_to_base_map();
   int selected(IFvMapView* pMapView, CPoint point, UINT flags);
   bool is_selection_locked();
   void drag(ViewMapProj *view, CPoint point, UINT flags, HCURSOR *cursor, 
      HintText **hint);
   void cancel_drag(ViewMapProj *view);
   void drop(ViewMapProj *view, CPoint point, UINT flags);
   int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);
   int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags);
   boolean_t offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, UINT nFlags, CPoint point);
   boolean_t offer_keyup(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, UINT nFlags, CPoint point);

   // Note: This appears to be dead code
   //C_icon *hit_test(MapProj* map, CPoint point);

   void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode *, CFVMenuNode *> &list);
   int on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point);
   int redraw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);
   boolean_t paste_OLE_data_object( ViewMapProj* map, CPoint* pPt, COleDataObject* data_object);
   boolean_t can_drop_data( COleDataObject* data_object, CPoint& pt );
   int pre_save(boolean_t *cancel);
   int pre_close(BOOL* cancel);
   boolean_t test_snap_to(ViewMapProj* map, CPoint point);
   boolean_t do_snap_to(ViewMapProj* map, CPoint point, CList<SnapToInfo *, SnapToInfo *> & snap_to_list);
   long is_read_only();

   // Returns the beginning and ending times for the overlay's time segment.  If the time segment is valid 
   // set pTimeSegmentValue to a non-zero integer, otherwise set it to zero.  An example where the time segment
   // would be invalid is an uncalculated route.  In this case, the overlay is not accounted for in the overall
   // clock time and does not show up in the playback dialog's gannt chart
   virtual HRESULT GetTimeSegment( DATE *pBegin, DATE *pEnd, long *pTimeSegmentValid);

   boolean_t get_view_time_span(COleDateTime& begin, COleDateTime& end);

   // Returns the color that is used by the playback dialog's gannt chart for this overlay
   virtual HRESULT GetGanntChartColor(COLORREF *pColor)
   {
      *pColor = RGB(255, 0, 0);
      return S_OK;
   }

protected:
   int draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // handle the selection of an object, returns FAILURE if nothing was
   // performed
   int on_selected(OvlElement *element, d_geo_t location);

   // call the client's GetInfoText function to get the text used in the 
   // info text dialog
   int info_text_dialog(OvlElement *element);

   // return the object id of the given object.  Returns -1 if object
   // was not found in the object map.
   int get_object_id(OvlElement *object);

   // get the tool tip of the given element
   CString get_tool_tip(OvlElement *element);

   // get the help text of the given element
   CString get_help_text(OvlElement *element);

   // return a pointer to the object that is beneath the given point or NULL 
   // if no object is underneath the given point
   OvlElement *hit_test(CPoint point);

   // returns true if the object would be visible, false otherwise
   boolean_t object_above_threshold(MapProj *map, OvlElement *object);

   int AddMenuItemsFromString(CString strMenuItems, CList<CFVMenuNode *, CFVMenuNode *> &listMenuNodes,
      int nObjectHandle);

   CString get_menu_items_from_client(OvlElement *element, CPoint point, long menu_id);

public:
   long DisplayMask( const CString& mask_file, 
      double flight_altitude,
      long base_elevation, 
      long mask_color, 
      long brush_style);

   // return a pointer to the ILayerEditor interface
   LayerEditorImpl *get_ILayerEditor() { return m_layer_editor; }

   int open_file(CString filename);

   // set the threshold of this layer.  The layer will not draw if the
   // map is at a scale above this threshold
   long set_threshold(MapScale map_scale) 
   { 
      m_threshold = map_scale; 
      return SUCCESS; 
   }

protected:
   void SetOwningThreadId(const DWORD owning_thread_id)
   {
      m_owning_thread_id = owning_thread_id;
   }

private:
   // STA COM operations must be performed on the owning thread.
   DWORD m_owning_thread_id;
};

class CLayerOvl : 
   public CBaseLayerOvl,
   public CFvOverlayPersistenceImpl
{
public:
   // constructor
   CLayerOvl();
   CLayerOvl(const DWORD owning_thread_id, CString name, IDispatch *pdisp,
      CString icon_name, LayerEditorImpl *layer_editor = NULL);
   CLayerOvl(const DWORD owning_thread_id, CString name, IDispatch *pdisp);

   virtual void Finalize();

   // Setup - used to setup CLayerOvl's parameters when using the
   // default constructor
   void Setup(const DWORD owning_thread_id, CString name, IDispatch *pdisp,
      CString icon_name, LayerEditorImpl *layer_editor);

   // *****************************************
   // C_overlay Function Overrides
public:

   // draw the layer's OvlElements to this DC
   virtual int draw(ActiveMap* map);
   virtual int redraw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   // Derived classes can implement this method if they need to
   // draw to the base map before the overlays are drawn with a call
   // to draw().  This is used, for example, in the shadow overlay to 
   // modify the shades of the base map.  The base map in the dc will be 
   // a north-up equal arc map (before rotatation and before projection).
   virtual int draw_to_base_map(ActiveMap* map);
   int CLayerOvl::OnDrawToBaseMap(IActiveMapProj *map);

   // Each overlay is given a chance to modify the base map in draw_to_base_map.  They
   // can either add new alpha blended pixmaps via IGraphicContext's PutPixmapAlpha or 
   // they can add a pixmap via PutPixmap.  If this overlay will be drawing pixmaps
   // to the base map with PutPixmap the rendering engine has to be setup to do this.  Return
   // TRUE if your overlay will call IGraphicsContext::PutPixmap in draw_to_base_map
   virtual BOOL can_add_pixmaps_to_base_map();

   // Sets the cursor and icon (help text and cursor) to indicate what will
   // happen if the selected member is called with the same parameters.  
   // Returns SUCCESS whenever the selected member would have returned SUCCESS,
   // FAILURE otherwise.  Use default implementation to do nothing.
   //
   // The cursor parameter is used to overide the default cursor (see the
   // C_ovl_mgr::get_default_cursor function).  Setting it to NULL indicates
   // you do not want to overide the default.
   //
   // The hint parameter is used to overide the default hint (see the
   // C_ovl_mgr::get_default_hint function).  Setting it to NULL indicates you
   // do not want to overide the default.
   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);

   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags);

   // Performs an object specific function when the object is selected.  Based
   // on context, i.e., the state or mode of the application as well as this
   // overlay, the overlay will determine what action, if any, will take place.
   // This function will return SUCCESS to indicate that this overlay processed 
   // the selection, so no other overlay will process this selection.  If 
   // selected() returns drag == TRUE then the drag() member of the overlay 
   // will be called each time a drag occurs, until a cancel-drag or a drop 
   // occurs.  The cancel_drag() member is called when a cancel-drag occurs.
   // The drop() member is called when a drop occurs.  The meaning of the terms
   // drag and drop are overlay specific.
   //
   // The cursor parameter is used to overide the default cursor (see the
   // C_ovl_mgr::get_default_cursor function).  Setting it to NULL indicates
   // you do not want to overide the default.
   //
   // The hint parameter is used to overide the default hint (see the
   // C_ovl_mgr::get_default_hint function).  Setting it to NULL indicates you
   // do not want to overide the default.
   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   // This function allows an overlay to temporarily lock the selection so that only it
   // will process mouse operations. Specifically, when "Display Airways in Region" or
   // "Display Military Training Routes in Region" is selected from the right-click menu,
   // they need to have exclusive mouse control until the operation is completed. The
   // default implementation is that selection locking is disabled. An overlay can lock
   // selection by overriding this function and returning true when exclusive mouse 
   // control is required. Returns true if selection locking is enabled or false if 
   // selection locking is disabled.
   virtual bool is_selection_locked();

   // This function handles route drags started from the selected() member
   // function.  Route drags include moving an existing turn point or 
   // additional point, inserting a turn point along an existing route leg,
   // adding an additional point, or appending a turn point to the end of the
   // route.  This function updates the display, cursor, and hint for the
   // current point and drag object.
   virtual void drag(ViewMapProj *view, CPoint point, UINT flags, HCURSOR *cursor, 
      HintText **hint);

   // This function completes the route drag operation started by the 
   // selected() member and contiued by the drag() member.
   virtual void drop(ViewMapProj *view, CPoint point, UINT flags);

   // This function cancels the route drag operation started by the 
   // selected() member and contiued by the drag() member.
   virtual void cancel_drag(ViewMapProj *view);

   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point);

   // Receives all keyup messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keyup(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
      UINT nFlags, CPoint point);

   // allows the overlay manager to pass double-click notices to overlays
   virtual int on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point);

   // Note: This appears to be dead code

   // determine if the point hits the object's
   //virtual C_icon *hit_test(MapProj* map, CPoint point);

   // add menu items to the popup menu based on context
   virtual void menu(ViewMapProj* map, CPoint point, 
      CList<CFVMenuNode *, CFVMenuNode *> &list);

   // paste OLE data object(s) onto an overlay. Defaults to not being handled
   virtual boolean_t paste_OLE_data_object( ViewMapProj* map, CPoint* pPt, 
      COleDataObject* data_object);
   // test to see if we can read clipboard data
   virtual boolean_t can_drop_data( COleDataObject* data_object, CPoint& pt );

public:

   // ILayerEditor methods
public:

   virtual int file_new();

   virtual int open(const CString & /*pathname*/);

   // save the file to the given name
   virtual int save_as(const CString & pathname, long nSaveFormat);

   // Inform the overlay that it is about to be saved, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the save is canceled.
   virtual int pre_save(boolean_t *cancel);

   // Inform the overlay that it is about to be closed, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the close is canceled.  When this function is called with cancel
   // set to NULL the overlay will be closed no matter what.
   virtual int pre_close(BOOL* cancel);

   // test to see if this overlay can do a single point snap to at this point
   virtual boolean_t test_snap_to(ViewMapProj* map, CPoint point);

   // get the snap to objects for this overlay at this point
   virtual boolean_t do_snap_to(ViewMapProj* map, CPoint point, CList<SnapToInfo *, SnapToInfo *> & snap_to_list);

public:
   void set_file_specification(const CString& fileSpecification)
   {
      m_fileSpecification = fileSpecification;
   }

protected:
   // Protected destructor. Call Release instead
   virtual ~CLayerOvl();
};

class CStaticLayerOvl : 
   public CBaseLayerOvl,
   public FvOverlayImpl
{
public:
   CStaticLayerOvl();
   CStaticLayerOvl(const DWORD owning_thread_id, CString displayName,
      IDispatch *pdisp, CString icon_name,
      LayerEditorImpl *layer_editor = NULL);

   // Setup - used to setup CStaticLayerOvl's parameters when using the
   // default constructor
   void Setup(const DWORD owning_thread_id, CString displayName,
      IDispatch *pdisp, CString icon_name, LayerEditorImpl *layer_editor);

   CString GetDisplayName();

   // C_overlay overrides
public:

   virtual HRESULT InternalInitialize(GUID overlayDescGuid);

   // Derived classes must implement this function.
   // When m_valid == TRUE do a full window redraw, otherwise draw
   // from scratch. draw() should set m_valid to TRUE when it is done.
   virtual int draw(ActiveMap* map);

   // Derived classes can implement this method if they need to
   // draw to the base map before the overlays are drawn with a call
   // to draw().  This is used, for example, in the shadow overlay to 
   // modify the shades of the base map.  The base map in the dc will be 
   // a north-up equal arc map (before rotatation and before projection).
   virtual int draw_to_base_map(ActiveMap* /*map*/);

   // Each overlay is given a chance to modify the base map in draw_to_base_map.  They
   // can either add new alpha blended pixmaps via IGraphicContext's PutPixmapAlpha or 
   // they can add a pixmap via PutPixmap.  If this overlay will be drawing pixmaps
   // to the base map with PutPixmap the rendering engine has to be setup to do this.  Return
   // TRUE if your overlay will call IGraphicsContext::PutPixmap in draw_to_base_map
   virtual BOOL can_add_pixmaps_to_base_map();

   // Performs an object specific function when the object is selected.  Based
   // on context, i.e., the state or mode of the application as well as this
   // overlay, the overlay will determine what action, if any, will take place.
   // This function will return SUCCESS to indicate that this overlay processed 
   // the selection, so no other overlay will process this selection.  If 
   // selected() returns drag == TRUE then the drag() member of the overlay 
   // will be called each time a drag occurs, until a cancel-drag or a drop 
   // occurs.  The cancel_drag() member is called when a cancel-drag occurs.
   // The drop() member is called when a drop occurs.  The meaning of the terms
   // drag and drop are overlay specific.
   //
   // The cursor parameter is used to overide the default cursor (see the
   // C_ovl_mgr::get_default_cursor function).  Setting it to NULL indicates
   // you do not want to overide the default.
   //
   // The hint parameter is used to overide the default hint (see the
   // C_ovl_mgr::get_default_hint function).  Setting it to NULL indicates you
   // do not want to overide the default.
   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   // This function allows an overlay to temporarily lock the selection so that only it
   // will process mouse operations. Specifically, when "Display Airways in Region" or
   // "Display Military Training Routes in Region" is selected from the right-click menu,
   // they need to have exclusive mouse control until the operation is completed. The
   // default implementation is that selection locking is disabled. An overlay can lock
   // selection by overriding this function and returning true when exclusive mouse 
   // control is required. Returns true if selection locking is enabled or false if 
   // selection locking is disabled.
   virtual bool is_selection_locked();

   // This function handles route drags started from the selected() member
   // function.  Route drags include moving an existing turn point or 
   // additional point, inserting a turn point along an existing route leg,
   // adding an additional point, or appending a turn point to the end of the
   // route.  This function updates the display, cursor, and hint for the
   // current point and drag object.
   virtual void drag(ViewMapProj *view, CPoint point, UINT flags, HCURSOR *cursor, 
      HintText **hint);

   // This function cancels the route drag operation started by the 
   // selected() member and contiued by the drag() member.
   virtual void cancel_drag(ViewMapProj *view);

   // This function completes the route drag operation started by the 
   // selected() member and contiued by the drag() member.
   virtual void drop(ViewMapProj *view, CPoint point, UINT flags);

   // Sets the cursor and icon (help text and cursor) to indicate what will
   // happen if the selected member is called with the same parameters.  
   // Returns SUCCESS whenever the selected member would have returned SUCCESS,
   // FAILURE otherwise.  Use default implementation to do nothing.
   //
   // The cursor parameter is used to overide the default cursor (see the
   // C_ovl_mgr::get_default_cursor function).  Setting it to NULL indicates
   // you do not want to overide the default.
   //
   // The hint parameter is used to overide the default hint (see the
   // C_ovl_mgr::get_default_hint function).  Setting it to NULL indicates you
   // do not want to overide the default.
   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);

   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags);

   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point);

   // Receives all keyup messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keyup(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
      UINT nFlags, CPoint point);

   // Note: This appears to be dead code

   // determine if the point hits the object's
   //virtual C_icon *hit_test(MapProj* map, CPoint point);

   // add menu items to the popup menu based on context
   virtual void menu(ViewMapProj* map, CPoint point, 
      CList<CFVMenuNode *, CFVMenuNode *> &list);

   // allows the overlay manager to pass double-click notices to overlays
   virtual int on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point);

   // paste OLE data object(s) onto an overlay. Defaults to not being handled
   virtual boolean_t paste_OLE_data_object( ViewMapProj* /*map*/, CPoint* /*pPt*/, 
      COleDataObject* /*data_object*/ );

   // test to see if we can read clipboard data
   virtual boolean_t can_drop_data( COleDataObject* /*data_object*/, CPoint& /*pt*/ );

   // Inform the overlay that it is about to be saved, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the save is canceled.
   virtual int pre_save(boolean_t *cancel);

   // Inform the overlay that it is about to be closed, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the close is canceled.  When this function is called with cancel
   // set to NULL the overlay will be closed no matter what.
   virtual int pre_close(BOOL* cancel);

   // test to see if this overlay can do a single point snap to at this point
   virtual boolean_t test_snap_to(ViewMapProj* map, CPoint point);

   // get the snap to objects for this overlay at this point
   virtual boolean_t do_snap_to(ViewMapProj* map, CPoint point, CList<SnapToInfo *, SnapToInfo *> & snap_to_list);

protected:
   // Protected destructor. Call Release instead
   ~CStaticLayerOvl();
};

#endif   // ifndef LAYEROVL_H
