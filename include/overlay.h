// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.



/* overlay.h */

#pragma once

#ifndef OVERLAY_H
#define OVERLAY_H

#include "common.h"
#include "icon_dib.h"

#include "../MapParamsListener_Interface.h"

// forward definition of classes
class ActiveMap;
class ActiveMap_TMP;
class CFVMenuNode;
class CIconAlias;
class CIconImage;
class COverlayBackingStore;
class CPointOverlayFilter;
class CVerticalViewProjector;
class C_drag_item;
class C_overlay_factory;
class DisplayElementRootNode;
class HintText;
class MapProj;
class MapSpec;
class OverlayElements;
class OverlayInvalidatedListener_Interface;
class OvlElementContainer;
class SnapToInfo;
class TabularEditorDataObject;
class ViewMapProj;
enum BackingStoreTypeEnum;

interface IFvMapView;

// C_icon_list - template class for defining C_icon lists in C_ovlerlay derived classes
template <class C_icon_derived_class, int grow_by> 
class C_icon_list
{
private:
   CList<C_icon_derived_class *, C_icon_derived_class *>m_list;
   POSITION m_position;

public:
   // Constructor - grow_by should be about 10% of the maximum size or 10,
   // which ever is greater.
   C_icon_list() : m_list(grow_by) { m_position = NULL; }

   // Destructor
   virtual ~C_icon_list() { delete_all(); }

   // add the icon to the end of the list
   void add(C_icon_derived_class *icon) { m_list.AddTail(icon); }

   // add the icon to the beginning of the list
   void add_head(C_icon_derived_class *icon) { m_list.AddHead(icon); }

   // add a list of icons to the end of the list 
   // Note: the MFC (as of VC++5.0 SP3) implimentation of adding a list to a list is not
   // very efficient as it loops through the whole list
   void add(CList <C_icon_derived_class*, C_icon_derived_class*> *pList) { m_list.AddTail(pList); }

   // remove the icon if it is in the list - returns FAILURE if it is not found
   int remove(C_icon_derived_class *icon);

   // removes and deletes all icons from the list
   void delete_all();

   // gets the number of icons in the list
   int get_count() { return m_list.GetCount(); }

   // starts a sequential scan from the head of the list,
   // stopping on the nth element.
   POSITION find_index(int nIndex) { return m_list.FindIndex(nIndex); }

   // removes the specified element from this list
   void remove_at( POSITION position ) { m_list.RemoveAt(position); }

   // returns TRUE when the list is empty.
   boolean_t is_empty() { return m_list.IsEmpty(); }

   // get icons in order (uninterupted) - NULL for none
   C_icon_derived_class *get_first();
   C_icon_derived_class *get_next();

   // get icons in reverse order (uninterupted) - NULL for none
   C_icon_derived_class *get_last();
   C_icon_derived_class *get_prev();

   // returns the icon after previous - NULL for none
   C_icon_derived_class *get_next(C_icon_derived_class *previous);

   // returns the icon before next - NULL for none
   C_icon_derived_class *get_prev(C_icon_derived_class *next);

   // Returns the first icon - NULL for none.  next is set to the POSITION of
   // the next icon in the list - NULL for none.
   C_icon_derived_class *get_first(POSITION &next);

   // Returns the icon at the POSITION next.  next is set to the POSITION of
   // the next icon in the list - NULL for none.  You must insure that next
   // contains a valid value when it is passed to this function (NULL is not
   // a valid value).  You can retrieve the first icon and the POSITION of the
   // second icon in the list using the above get_first() function.  You can
   // also get the POSITION of the first icon by using the get_first_position()
   // member, below.
   C_icon_derived_class *get_next(POSITION &next)
   { return m_list.GetNext(next); }

   // Returns the last icon - NULL for none.  previous is set to the position
   // of the previous icon in the list - NULL for none.
   C_icon_derived_class *get_last(POSITION &previous);

   // Returns the icon at the POSITION previous.  previous is set to the 
   // POSITION of the previous icon in the list - NULL for none.  You must 
   // insure that previous contains a valid value when it is passed to this 
   // function (NULL is not a valid value).  You can retrieve the last icon and
   // the POSITION of the second to last icon in the list using the above 
   // get_last() function.  You can also get the POSITION of the last icon by 
   // using the get_last_position() member, below.
   C_icon_derived_class *get_prev(POSITION &previous)
   { return m_list.GetPrev(previous); }

   // get the POSITION of the first or last icon
   POSITION get_first_position() { return m_list.GetHeadPosition(); }
   POSITION get_last_position() { return m_list.GetTailPosition(); }

   // returns TRUE if the icon is in the list FALSE otherwise
   boolean_t find(C_icon_derived_class *icon)
   { return (m_list.Find(icon) != NULL); }

};
// end of C_icon_list template class definition


// C_icon_list_map - template class for defining a map between CWnd * and C_icon_list derived
// class objects
template <class C_icon_list_derived_class> 
class C_icon_list_map
{
private:
   CMap<CWnd *, CWnd *, 
      C_icon_list_derived_class *, C_icon_list_derived_class *> m_map;

public:
   // Constructor - grow_by should be about 10% of the maximum size or 10,
   // which ever is greater.
   C_icon_list_map() {}

   // Destructor
   virtual ~C_icon_list_map() { delete_all(); }

   // find the icon list for the given CWnd * - returns FAILURE if the key is
   // not found
   int get_icon_list(CWnd *view, C_icon_list_derived_class &icon_list);

   // add a icon list CWnd pair
   void add_pair(CWnd *view, C_icon_list_derived_class *icon_list);

   // removes an icon list CWnd pair, deletes the icon list - returns FAILURE 
   // if the key is not is not found
   int delete_pair(CWnd *view);

   // removes all pairs and deletes all icon list in the map
   void delete_all();
};
// end of C_icon_list_map template class definition


// Overlay_Interface - this interface is required by all overlay instances
interface Overlay_Interface
{
   // Returns the overlay descriptor guid for this overlay.  
   // For now, the signature will match the COM wrapper method for the property
   // virtual HRESULT get_m_overlayDescGuid(GUID *pOverlayDescGuid) = 0;
   virtual GUID get_m_overlayDescGuid() = 0;

   // True if the overlay is visible, false otherwise.
   virtual HRESULT put_m_bIsOverlayVisible(long bIsOverlayVisible) = 0;
   virtual long get_m_bIsOverlayVisible() = 0;

   // Initialize the overlay.  The read-only property m_overlayDescGuid should
   // be set by this method
   virtual HRESULT InternalInitialize(GUID overlayDescGuid) = 0;
};

// OverlayRender_Interface - this optional interface should be implemented to support rendering your overlay
interface OverlayRender_Interface
{
   virtual HRESULT OnDraw(IActiveMapProj *pActiveMap) = 0;
};

// OverlayUIEvents_Interface - this optional interface can be implemented to receive user interface events
interface OverlayUIEvents_Interface
{
   // MouseMove occurs when the mouse pointer is moved in the view
   virtual HRESULT MouseMove(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled) = 0;

   // MouseLeftButtonDown occurs when the left mouse button is pressed while the mouse pointer is over the given view
   virtual HRESULT MouseLeftButtonDown(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled) = 0;

   // MouseLeftButtonUp occurs when the left mouse button is released while the mouse point is over the given view
   virtual HRESULT MouseLeftButtonUp(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled) = 0;

   // MouseRightButtonDown occurs when the right mouse button is pressed while the mouse pointer is over the given view.
   virtual HRESULT MouseRightButtonDown(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled) = 0;

   // MouseWheel occurs when the user rotates the mouse wheel while the mouse pointer is over the given view
   virtual HRESULT MouseWheel(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long zDelta, long nFlags, long *pbHandled) = 0;

   // MouseDoubleClick occurs when the left mouse button is double clicked over the given view
   virtual HRESULT MouseDoubleClick(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled) = 0;

   // KeyDown occurs when a key is pressed
   virtual HRESULT KeyDown(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nChar, long nRepCnt, long nFlags, long *pbHandled) = 0;

   // KeyUp occurs when a key is released
   virtual HRESULT KeyUp(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nChar, long nRepCnt, long nFlags, long *pbHandled) = 0;
};

// IFvOverlayUIEventRoutingOverrides - In most cases events are either sent to the topmost overlay or are routed
//    from the top to the bottom of the overlay stack until the event is handled.  An overlay, in certain conditions,
//    may need to have the first chance at handling these events even if the overlay is not the topmost overlay in 
//    the stack.  This optional interface can be implemented to override the default routing of user interface events.  
//
//    Note that if any of the following methods in this interface returns true, then the overlay will receive that 
//    user interface event bypassing normal event routing.  If the event is not handled, then the event will be routed normally.
interface OverlayUIEventRoutingOverrides_Interface
{
   virtual long get_m_bDirectlyRouteMouseMove() = 0;
   virtual long get_m_bDirectlyRouteMouseLeftButtonDown() = 0;
   virtual long get_m_bDirectlyRouteMouseLeftButtonUp() = 0;
   virtual long get_m_bDirectlyRouteKeyDown() = 0;
   virtual long get_m_bDirectlyRouteKeyUp() = 0;
};

// OverlayContextMenu_Interface - this optional interface can be implemented by an overlay to support a context menu
interface OverlayContextMenu_Interface
{
   // append menu items to the given context menu
   virtual HRESULT AppendMenuItems(FalconViewOverlayLib::IFvContextMenu *pContextMenu, FalconViewOverlayLib::IFvMapView *pMapView, long x, long y) = 0;
};

// OverlayElement_Interface - this interface is optional and can be implemented
// to support methods exposed by the IOverlayElement automation interface
interface OverlayElement_Interface
{
   // registers varDispatch to receive callbacks
   virtual HRESULT RegisterForCallbacks(VARIANT varDispatch) = 0;

   // Updates an element of the overlay using the given XML string
   virtual HRESULT UpdateElement(BSTR bstrElementXml) = 0;

   // By default an overlay implementing this interface supports
   // OverlayElement_Interface.  However, some generic overlays such as
   // COverlayCOM need to dynamically state whether they support
   // OverlayElement_Interface or not.
   virtual BOOL SupportsFvOverlayElement() { return TRUE; }
};

// IFvOverlayElementEnumerator - this interface is optional and can be implemented to support methods exposed by
//    the IOverlayElementEnumerator automation interface
interface OverlayElementEnumerator_Interface
{
   // The XML that describes the current element
   virtual BSTR bstrCurrentXml() = 0;

   // Resets the enumerator.  After a call to Reset, the current element will be NULL until MoveNext is called
   virtual HRESULT Reset() = 0;

   // Moves to the next element in enumeration
   virtual HRESULT MoveNext(long *pbRet) = 0;

   // By default an overlay implementing this interface supports IFvOverlayElementEnumerator.  However, some generic overlays
   // such as COverlayCOM need to dynamically state whether they support IFvOverlayElementEnumerator or not.
   virtual BOOL SupportsFvOverlayElementEnumerator() { return TRUE; }
};

// IFvOverlayPersistence - this interface is required by file based overlays
interface OverlayPersistence_Interface
{
   // Return the file specification of the overlay
   virtual HRESULT get_m_fileSpecification(BSTR* pFileSpecification) = 0;
   virtual HRESULT put_m_fileSpecification(BSTR pFileSpecification) { return S_OK; }

   // True if the file overlay has been persisted.  False otherwise.
   virtual HRESULT put_m_bHasBeenSaved(long pVal) = 0;
   virtual HRESULT get_m_bHasBeenSaved(long* pVal) = 0;

   // True if the file overlay is dirty and needs to be saved.  False otherwise.
   virtual HRESULT put_m_bIsDirty(long pVal) = 0;
   virtual HRESULT get_m_bIsDirty(long* pVal) = 0;

   // True if the file overay is a read-only file overlay
   virtual HRESULT put_m_bIsReadOnly(long pVal) = 0;
   virtual HRESULT get_m_bIsReadOnly(long* pVal) = 0;

   // The overlay has been created from scratch.  The m_fileSpecification property should be initialized
   // in this method.
   virtual HRESULT FileNew() = 0;

   // Open the overlay with the given file specification
   virtual HRESULT FileOpen(BSTR fileSpecification) = 0;

   // Save the overlay to the given file specification and format.  The parameter nSaveFormat corresponds
   // to the selected filter in the Save As dialog.  If the format is unspecified,  then eSaveFileFormat 
   // will be zero (save to the overlay's default file format in this case).
   virtual HRESULT FileSaveAs(BSTR fileSpecification, long nSaveFormat) = 0;

   virtual HRESULT Revert(BSTR fileSpecification){ return E_FAIL; }
   virtual BOOL SupportsRevertOverride(){ return FALSE; }
};

// IFvPlaybackEventsObserver - implement this interface to observe events coming from the playback mechanism
interface PlaybackEventsObserver_Interface
{
   // Called when a playback is started
   virtual HRESULT OnPlaybackStarted() = 0;

   // Called when the playback is stopped
   virtual HRESULT OnPlaybackStopped() = 0;

   // Called whenever the playback time changes
   virtual HRESULT OnPlaybackTimeChanged(DATE dateCurrentTime) = 0;

   // Called whenever the playback rate changes
   virtual HRESULT OnPlaybackRateChanged(long lNewPlaybackRate) = 0;
};

// IFvPlaybackTimeSegment - implement this interface to specify a time segment associated with your overlay
interface PlaybackTimeSegment_Interface
{
   // Returns the beginning and ending times for the overlay's time segment.  If the time segment is valid 
   // set pbTimeSegmentValue to a non-zero integer, otherwise set it to zero.  An example where the time segment
   // would be invalid is an uncalculated route.  In this case, the overlay is not accounted for in the overall
   // clock time and does not show up in the playback dialog's gannt chart
   virtual HRESULT GetTimeSegment( DATE *pBegin, DATE *pEnd, long *pbTimeSegmentValid) = 0;

   // Returns the color that is used by the playback dialog's gannt chart for this overlay
   virtual HRESULT GetGanntChartColor(COLORREF *pColor) = 0;

   // By default an overlay implementing this interface supports IFvPlaybackTimeSegment.  However, some generic overlays
   // such as COverlayCOM need to dynamically state whether they support IFvPlaybackTimeSegment or not.
   virtual BOOL SupportsFvPlaybackTimeSegment() { return TRUE; }
};

// IFvOverlayTypeOverrides - optional interface used to support per-instance overlay meta-data
interface OverlayTypeOverrides_Interface
{
   // Override the display name defined in the overlay's factory.  This is used to support
   // per-instance overlay display names (e.g., an overlay created via the ILayer::CreateLayer method)
   virtual HRESULT get_m_displayName(BSTR *pVal) = 0;

   // This icon will override the icon defined in the overlay's descriptor.  This
   // is used to support per-instance overlay icons (e.g., an overlay created via the
   // ILayer::CreateLayerEx interface).  This icon is displayed in the overlay manager's
   // current list of opened overlays.  The filename can either be the full specification
   // or a path relative to HD_DATA\icons.
   virtual HRESULT get_m_iconFilename(BSTR* pVal) = 0;

   // By default an overlay implementing this interface supports IFvOverlayTypeOverrides.  However, some generic overlays
   // such as COverlayCOM need to dynamically state whether they support IFvOverlayTypeOverrides or not.
   virtual BOOL SupportsFvOverlayTypeOverrides() { return TRUE; }
};

class C_overlay :
   public Overlay_Interface,
   public OverlayUIEvents_Interface,
   public OverlayUIEventRoutingOverrides_Interface,
   public OverlayContextMenu_Interface,
   public OverlayRender_Interface,
   public MapParamsListener_Interface
{
protected:
   GUID m_overlayDescGuid;
   long m_bIsOverlayVisible;

   boolean_t m_valid;
   COverlayBackingStore *m_pOverlayBackingStore;

   // Display element support
   OvlElementContainer* m_ovlElementContainer;

   DisplayElementRootNode* m_display_element_root_node;

public:
   // Constructor
   C_overlay();

   // Initializes the DisplayElementRootNode instance associated with this
   // overlay if the current projection is a 3D projection.
   void InitializeDisplayElementRootNode();

   // Used to lazily initialize display element root node when 3D projection
   // is entered.
   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;


   // overlays should always be deleted by calling Release.  This is because 
   // some internal overlays are derived from CComObjectRootEx and may still 
   // have references to them
   STDMETHOD_(ULONG, AddRef)() = 0;
   STDMETHOD_(ULONG, Release)() = 0;

   virtual FalconViewOverlayLib::IFvOverlayPtr GetFvOverlay() = 0;

   // First step in marshaling the IFvOverlay interface pointer to another
   // thread. This call must be made from the user interface thread.
   IStream* PrepareMarshaledStream();

   void ResetDisplayNodeVisibility();

protected:
   // Destructor - should not be called.  Use Release instead.
   virtual ~C_overlay();

   // Overlay_Interface
public:
   // Returns the overlay descriptor guid for this overlay.  
   virtual GUID get_m_overlayDescGuid()
   {
      return m_overlayDescGuid;
   }

   // True if the overlay is visible, false otherwise.
   virtual HRESULT put_m_bIsOverlayVisible(long bIsOverlayVisible);
   virtual long get_m_bIsOverlayVisible()
   {
      return m_bIsOverlayVisible;
   }

   // Initialize the overlay.  The read-only property m_overlayDescGuid should be set by this method
   virtual HRESULT InternalInitialize(GUID overlayDescGuid)
   {
      m_overlayDescGuid = overlayDescGuid;
      return S_OK;
   }

   void OuterFinalize();

   virtual void Finalize()
   {
   }

   // OverlayUIEvents_Interface
public:
   // MouseMove occurs when the mouse pointer was moved in the view
   virtual HRESULT MouseMove(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseLeftButtonDown occurs when the left mouse button is pressed while the mouse pointer is over the given view
   virtual HRESULT MouseLeftButtonDown(FalconViewOverlayLib::IFvMapView *, long x, long y, long nFlags, long *pbHandled);

   // MouseLeftButtonUp occurs when the left mouse button is released while the mouse point is over the given view
   virtual HRESULT MouseLeftButtonUp(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseRightButtonDown occurs when the right mouse button is pressed while the mouse pointer is over the given view.
   virtual HRESULT MouseRightButtonDown(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseDoubleClick occurs when the mouse button is double clicked over the given view
   virtual HRESULT MouseDoubleClick(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseWheel occurs when the user rotates the mouse wheel while the mouse pointer is over the given view
   virtual HRESULT MouseWheel(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long zDelta, long nFlags, long *pbHandled);

   // KeyDown occurs when a key is pressed
   virtual HRESULT KeyDown(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nChar, long nRepCnt, long nFlags, long *pbHandled);

   // KeyUp occurs when a key is released
   virtual HRESULT KeyUp(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nChar, long nRepCnt, long nFlags, long *pbHandled);

   // OverlayUIEventRoutingOverrides_Interface
public:
   virtual long get_m_bDirectlyRouteMouseMove()
   {
      return is_selection_locked();
   }
   virtual long get_m_bDirectlyRouteMouseLeftButtonDown()
   {
      return is_selection_locked();
   }
   virtual long get_m_bDirectlyRouteMouseLeftButtonUp()
   {
      return is_selection_locked();
   }
   virtual long get_m_bDirectlyRouteKeyDown()
   {
      return is_selection_locked();
   }
   virtual long get_m_bDirectlyRouteKeyUp()
   {
      return is_selection_locked();
   }

   // OverlayContextMenu_Interface
public:
   virtual HRESULT AppendMenuItems(FalconViewOverlayLib::IFvContextMenu *pContextMenu, FalconViewOverlayLib::IFvMapView *pMapView, long x, long y);

   // OverlayRender_Interface
public:
   virtual HRESULT OnDraw(IActiveMapProj *pActiveMap);

   virtual HRESULT OnDrawDraped(IActiveMapProj* pActiveMap)
   {
      return S_OK;
   }
   virtual BOOL SupportsDrawDraped()
   {
      return FALSE;
   }

   // public member functions
public:
   OvlElementContainer* GetOvlElementContainer();

   // Returns TRUE if this overlay can be edited by the tabular editor
   virtual boolean_t is_tabular_editable() { return FALSE; }
   virtual TabularEditorDataObject* GetTabularEditorDataObject() { return NULL; }

   BackingStoreTypeEnum GetOverlayBackingStoreType();
   COverlayBackingStore *GetOverlayBackingStore();

   // Derived classes can implement this method if they need to
   // draw to the base map before the overlays are drawn with a call
   // to draw().  This is used, for example, in the shadow overlay to 
   // modify the shades of the base map.  The base map in the dc will be 
   // a north-up equal arc map (before rotatation and before projection).
   virtual int OnDrawToBaseMap(IActiveMapProj *map);

   // Derived classes can implement this method if they need to draw to the 
   // vertical view
   virtual int DrawToVerticalDisplay(CDC *pDC, CVerticalViewProjector* pProjector) { return SUCCESS; }

   // Each overlay is given a chance to modify the base map in draw_to_base_map.  They
   // can either add new alpha blended pixmaps via IGraphicContext's PutPixmapAlpha or 
   // they can add a pixmap via PutPixmap.  If this overlay will be drawing pixmaps
   // to the base map with PutPixmap the rendering engine has to be setup to do this.  Return
   // TRUE if your overlay will call IGraphicsContext::PutPixmap in draw_to_base_map
   virtual BOOL can_add_pixmaps_to_base_map() { return FALSE; }

   // force a draw from scratch
   virtual void invalidate();

   virtual int selected_vertical(CVerticalViewProjector* /*pProj*/, CPoint /*point*/, UINT /*flags*/,
      HCURSOR *cursor, HintText **hint)
   {
      *cursor = NULL;
      *hint = NULL;

      return FAILURE;
   }

   // Performs an object specific "receive" of a dragged icon from DragSel.
   // The receive_new_point() member of an overlay is called if and only if
   // it is currently the edit-mode overlay.
   virtual int receive_new_point(ViewMapProj* /*map*/, CPoint /*point*/, UINT /*flags*/, 
      C_drag_item* /*item*/)
   { 
      return FAILURE;
   }

   virtual int test_selected_vertical(CVerticalViewProjector* /*proj*/, CPoint /*point*/, UINT /*flags*/, 
      HCURSOR *cursor, HintText **hint)
   {
      *cursor = NULL;
      *hint = NULL;

      return FAILURE;
   }

   // Allow the current editor to release its current edit focus before
   // the editor loses control. This gets called just before the current
   // overlay set_edit_on changes.
   virtual void release_edit_focus() 
   { 
      return; 
   }

   // Allow the current editor to enter its current edit focus 
   // as the editor gains control. This gets called just after the current
   // overlay changes.
   virtual void enter_edit_focus() 
   { 
      return; 
   }

   // test to see if this overlay can do a single point snap to at this point
   virtual boolean_t test_snap_to(ViewMapProj* /*map*/, CPoint /*point*/) 
   { return FALSE; }

   // get the snap to objects for this overlay at this point
   virtual boolean_t do_snap_to(ViewMapProj* /*map*/, CPoint /*point*/, 
      CList<SnapToInfo *, SnapToInfo *> & /*snap_to_list*/)
   { return FALSE; }

   // Inform the overlay that it is about to be closed, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the close is canceled.  When this function is called with cancel
   // set to NULL the overlay will be closed no matter what.
   virtual int pre_close(boolean_t *cancel = NULL)
   {
      if (cancel)
         *cancel = FALSE;

      return SUCCESS;
   }

   // Inform the overlay that FalconView is shutting down
   virtual int exit() { return SUCCESS; }

   // Inform the overlay that it is about to be saved, so it can perform
   // any necessary clean up.  If the overlay returns cancel == TRUE then
   // the save is canceled.
   virtual int pre_save(boolean_t *cancel)
   {
      *cancel = FALSE;
      return SUCCESS;
   }

   // inform the overlay that it has lost focus, so that it can remove select
   // box, etc. if it needs to -- this will be moved to the information dialog box's callback instead
   virtual void release_focus() {}

   // paste OLE data object(s) onto an overlay. Defaults to not being handled
   virtual boolean_t paste_OLE_data_object( ViewMapProj* /*map*/, CPoint* /*pPt*/, 
      COleDataObject* /*data_object*/ ) { return FALSE; }
   // test to see if we can read clipboard data
   virtual boolean_t can_drop_data( COleDataObject* /*data_object*/, CPoint& /*pt*/ ) { return FALSE; }

   // return TRUE if data from the current overlay's data can be copied to the "clipboard" (Edit | Copy will be enabled)
   virtual boolean_t is_copy_to_clipboard_allowed() { return FALSE; }

   // copy data from the current overlay to the "clipboard" (handle Edit | Copy)
   virtual void copy_to_clipboard() { }

   // return TRUE if data can be pasted from the "clipboard" to the current overlay (Edit | Paste will be enabled)
   virtual boolean_t is_paste_from_clipboard_allowed() { return FALSE; }

   // paste data from the "clipboard" to the current overlay
   virtual void paste_from_clipboard() { }

   virtual boolean_t can_undo() { return FALSE; }
   virtual boolean_t can_redo() { return FALSE; }
   virtual void undo() { }
   virtual void redo() { }

   // The remaining methods are specific to C_overlay and do not implement any IFvOverlayx interface method
   //

public:
   void set_valid(boolean_t valid); // { m_valid = valid; }

   boolean_t get_valid(); // const { return m_valid; }

   // test, set and clear the overlays DIRTY flag
   boolean_t is_modified();
   virtual void set_modified(boolean_t mod_flag);

   // Returns TRUE if this is the current overlay.
   boolean_t get_current();

protected:
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
   virtual int selected(IFvMapView* /*map*/, CPoint /*point*/, UINT /*flags*/)
   {
      return FAILURE;
   }

   // Sets the cursor and icon (help text and cursor) to indicate what will
   // happen if the selected member is called with the same parameters.  
   // Returns SUCCESS whenever the selected member would have returned SUCCESS,
   // FAILURE otherwise.  Use default implementation to do nothing.
   //
   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint /*point*/, UINT /*flags*/) 
   { 
      return FAILURE; 
   }

   // Return SUCCESS if the mouse event was handled
   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
   {
      return FAILURE;
   }

   virtual int on_right_mouse_button_down(IFvMapView *pMapView, CPoint point, UINT nFlags)
   {
      return FAILURE;
   }

   // Performs an object specific drag operation based on context.  The
   // drag() member of an overlay is called if and only if its selected()
   // member returned TRUE.  Use the default implementation to do nothing.
   //
   // The cursor parameter is used to overide the default cursor (see the
   // C_ovl_mgr::get_default_cursor function).  Setting it to NULL indicates
   // you do not want to overide the default.
   //
   // The hint parameter is used to overide the default hint (see the
   // C_ovl_mgr::get_default_hint function).  Setting it to NULL indicates you
   // do not want to overide the default.
   virtual void drag(ViewMapProj* /*map*/, CPoint /*point*/, UINT /*flags*/, HCURSOR *cursor, 
      HintText **hint)
   {
      *cursor = NULL;
      *hint = NULL;
   }

   // Cancels the object specific "drag" operation initiated with a call
   // to the selected() which returned drag == TRUE.  The result of calling
   // this function should be to leave the overlay in the state it was in
   // before the call to selected().
   virtual void cancel_drag(ViewMapProj* /*map*/) {}

   // Performs an object specific "drop" operation based on context.  The
   // drop() member of an overlay is called if and only if its selected()
   // member returns TRUE.  This function should complete a "drag" operation
   // initiated with a call to selected() and continued through one or more
   // calls to drag()
   virtual void drop(ViewMapProj* /*map*/, CPoint /*point*/, UINT /*flags*/) {}

   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT /*nChar*/, UINT /*nRepCnt*/,
      UINT /*nFlags*/, CPoint /*point*/)
   { 
      return FALSE; 
   }

   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keyup(IFvMapView *pMapView, UINT /*nChar*/, UINT /*nRepCnt*/, 
      UINT /*nFlags*/, CPoint /*point*/)
   { 
      return FALSE; 
   }

   // Receives ctrl-shift mouse wheel messages from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the message to be
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.  Ctrl-shift mouse-wheel
   // events are normally ignored by MapView.
   virtual boolean_t offer_mousewheel( IFvMapView *pMapView, SHORT /*zDelta*/,
      UINT /*nFlags*/, CPoint /*point*/ )
   { 
      return FALSE;
   }

   // allows the overlay manager to pass double-click notices to overlays
   virtual int on_dbl_click(IFvMapView *pMapView, UINT /*flags*/, CPoint /*point*/) { return FAILURE; }

   // add menu items to the popup menu based on context
   virtual void menu(ViewMapProj* /*map*/, CPoint /*point*/, CList<CFVMenuNode *, CFVMenuNode *> & /*list*/) {}

   // This function allows an overlay to temporarily lock the selection so that only it
   // will process mouse operations. Specifically, when "Display Airways in Region" or
   // "Display Military Training Routes in Region" is selected from the right-click menu,
   // they need to have exclusive mouse control until the operation is completed. The
   // default implementation is that selection locking is disabled. An overlay can lock
   // selection by overriding this function and returning true when exclusive mouse 
   // control is required. Returns true if selection locking is enabled or false if 
   // selection locking is disabled.
   virtual bool is_selection_locked( )
   {
      return false;  // selection locking disabled
   }

   virtual int draw_to_base_map(ActiveMap* /*map*/) { return SUCCESS; }
   void SetupActiveMap(IActiveMapProj *map, ActiveMap_TMP& active_map);

public:

   // for COverlayCOM:IFvOvelay2 SetFrameWnd
   virtual void SetFrameWnd(FalconViewOverlayLib::IFvFrameWnd2* pFrameWnd) {}

   // for COverlayCOM::IFvOverlayTypeOverrides2 UserControllable 
   virtual HRESULT get_m_UserControllable(long* pVal) 
   {  
      return S_OK;  
   }

   // Derived classes must implement this function.
   // When m_valid == TRUE do a full window redraw, otherwise draw
   // from scratch. draw() should set m_valid to TRUE when it is done.
   virtual int draw(ActiveMap* map)
   {
      return SUCCESS;
   }

   // Receives WM_POWERBROADCAST messages from the OS via CMainFrm.  This function does
   // not support the deny_suspend option.  Override to receive power-status event
   // messages
   virtual void power_broadcast( UINT nPowerEvent, UINT nEventData ){ return; }

   // allow the overlay to notify listeners when it is invalidated
   void AddOverlayInvalidatedListener(
      OverlayInvalidatedListener_Interface* listener);
   void RemoveOverlayInvalidatedListener(
      OverlayInvalidatedListener_Interface* listener);

   virtual bool IsTopMostOverlay();
   virtual int Opacity();

private:
   CRITICAL_SECTION m_drawing_critical_section;
   CRITICAL_SECTION m_invalidate_listener_critical_section;
   std::vector<OverlayInvalidatedListener_Interface*>
      m_overlay_invalidated_listeners;
};
// end of C_overlay definition

class FvOverlayImpl : 
   public C_overlay,
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvOverlay, &__uuidof(FalconViewOverlayLib::IFvOverlay), &FalconViewOverlayLib::LIBID_FalconViewOverlayLib, /* wMajor = */ 1>,
   public IDispatchImpl<FalconViewOverlayLib::IFvOverlayRenderer, &__uuidof(FalconViewOverlayLib::IFvOverlayRenderer), &FalconViewOverlayLib::LIBID_FalconViewOverlayLib, /* wMajor = */ 1>
{
public:
   BEGIN_COM_MAP(FvOverlayImpl)
      COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IFvOverlay)
      COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvOverlay)
      COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvOverlayRenderer)
   END_COM_MAP()

   virtual void Finalize() {};

   virtual FalconViewOverlayLib::IFvOverlayPtr GetFvOverlay() override
   {
      return this;
   }

   // FalconViewOverlayLib::IFvOverlay
   //
   STDMETHOD(get_OverlayDescGuid)(GUID * pGuid)
   {
      *pGuid = m_overlayDescGuid;
      return S_OK;
   }
   STDMETHOD(put_IsOverlayVisible)(long pIsOverlayVisible)
   {
      m_bIsOverlayVisible = pIsOverlayVisible;
      return S_OK;
   }
   STDMETHOD(get_IsOverlayVisible)(long * pIsOverlayVisible)
   {
      *pIsOverlayVisible = m_bIsOverlayVisible;
      return S_OK;
   }
   STDMETHOD(put_OverlayEventSink)(IUnknown *pOverlayEventSink)
   {
      return S_OK;
   }
   STDMETHOD(raw_Initialize)(GUID overlayDescGuid)
   {
      return InternalInitialize(overlayDescGuid);
   }
   STDMETHOD(raw_Terminate)(long bCanAbortTermination, long *pbAbortTermination)
   {
      if (bCanAbortTermination)
      {
         boolean_t cancel;
         pre_close(&cancel);
         *pbAbortTermination = cancel;
      }
      else
      {
         pre_close(NULL);
         *pbAbortTermination = FALSE;
      }
      return S_OK;
   }

   // FalconViewOverlayLib::IFvOverlayRenderer
   //
   STDMETHOD(raw_OnDraw)(FalconViewOverlayLib::IFvMapView * pMapView,
      IUnknown * pActiveMap)
   {
      IActiveMapProjPtr active_map = pActiveMap;
      return C_overlay::OnDraw(active_map);
   }
};

class CFvOverlayPersistenceImpl : 
   public FvOverlayImpl,
   public OverlayPersistence_Interface,
   public IDispatchImpl<FalconViewOverlayLib::IFvOverlayPersistence, &__uuidof(FalconViewOverlayLib::IFvOverlayPersistence), &FalconViewOverlayLib::LIBID_FalconViewOverlayLib, /* wMajor = */ 1>
{
protected:
   CString m_fileSpecification;
   long m_bHasBeenSaved;
   long m_bIsDirty;
   boolean_t m_bIsReadOnly;

   // IFvOverlayPersistence
public:
   BEGIN_COM_MAP(CFvOverlayPersistenceImpl)
      COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvOverlayPersistence)
      COM_INTERFACE_ENTRY_CHAIN(FvOverlayImpl)
   END_COM_MAP()

   // Return the file specification of the overlay
   STDMETHOD(get_FileSpecification)(BSTR* pFileSpecification)
   {
      return get_m_fileSpecification(pFileSpecification);
   }
   virtual HRESULT get_m_fileSpecification(BSTR* pFileSpecification)
   {
      *pFileSpecification = _bstr_t(m_fileSpecification).Detach();
      return S_OK;
   }

   virtual void Finalize() {};

   // True if the file overlay has been persisted.  False otherwise.
   STDMETHOD(put_HasBeenSaved)(long pVal)
   {
      return put_m_bHasBeenSaved(pVal);
   }
   virtual HRESULT put_m_bHasBeenSaved(long pVal)
   {
      m_bHasBeenSaved = pVal;
      return S_OK;
   }

   STDMETHOD(get_HasBeenSaved)(long * pVal)
   {
      return get_m_bHasBeenSaved(pVal);
   }
   virtual HRESULT get_m_bHasBeenSaved(long* pVal)
   {
      *pVal = m_bHasBeenSaved;
      return S_OK;
   }

   // True if the file overlay is dirty and needs to be saved.
   // False otherwise.
   STDMETHOD(put_IsDirty)(long pVal)
   {
      return put_m_bIsDirty(pVal);
   }
   virtual HRESULT put_m_bIsDirty(long pVal)
   {
      m_bIsDirty = pVal;
      return S_OK;
   }

   STDMETHOD(get_IsDirty)(long * pVal)
   {
      return get_m_bIsDirty(pVal);
   }
   virtual HRESULT get_m_bIsDirty(long* pVal)
   {
      *pVal = m_bIsDirty;
      return S_OK;
   }

   // True if the file overay is a read-only file overlay
   STDMETHOD(put_IsReadOnly)(long pVal)
   {
      return put_m_bIsReadOnly(pVal);
   }
   virtual HRESULT put_m_bIsReadOnly(long pVal)
   {
      m_bIsReadOnly = pVal;
      return S_OK;
   }

   STDMETHOD(get_IsReadOnly)(long * pVal)
   {
      return get_m_bIsReadOnly(pVal);
   }
   virtual HRESULT get_m_bIsReadOnly(long* pVal)
   {
      *pVal = m_bIsReadOnly;
      return S_OK;
   }

   // The overlay has been created from scratch.  The m_fileSpecification property should be initialized
   // in this method.
   STDMETHOD(raw_FileNew)()
   {
      return FileNew();
   }
   virtual HRESULT FileNew() 
   {
      return file_new() == SUCCESS ? S_OK : E_FAIL;
   }

   // Open the overlay with the given file specification
   STDMETHOD(raw_FileOpen)(BSTR fileSpecification)
   {
      return FileOpen(fileSpecification);
   }
   virtual HRESULT FileOpen(BSTR fileSpecification)
   {
      return open(fileSpecification) == SUCCESS ? S_OK : E_FAIL;
   }

   // Save the overlay to the given file specification and format.  The parameter nSaveFormat corresponds
   // to the selected filter in the Save As dialog.  If the format is unspecified,  then eSaveFileFormat 
   // will be zero (save to the overlay's default file format in this case).
   STDMETHOD(raw_FileSaveAs)(BSTR fileSpecification, long nSaveFormat)
   {
      return FileSaveAs(fileSpecification, nSaveFormat);
   }
   virtual HRESULT FileSaveAs(BSTR fileSpecification, long nSaveFormat)
   {
      return save_as(fileSpecification, nSaveFormat) == SUCCESS ? S_OK : E_FAIL;
   }

protected:
   // constructor/destructor
   CFvOverlayPersistenceImpl(const CString &pathname);

   // create a new overlay from scratch
   virtual int file_new()
   {
      return FAILURE;
   }

   // open from or save to disk file
   virtual int open(const CString & /*pathname*/) = 0;

   // eSaveFileFormat - corresponds to the selected filter in the Save As dialog.  If the format is unspecified, 
   // then eSaveFileFormat will be zero (save to the overlay's default file format in this case).
   virtual int save_as(const CString & pathname, long nSaveFormat) = 0;

   // get full file specification 
   virtual const CString & get_specification() const;

public:
   // set specification - currently only called by Collaboration specific code
   virtual void set_specification(const CString & specification);
};


//*****************************************************************************
// CIconImage class 
class CIconImage
{
public:
   HICON m_mega_icon;  // 48x48 icon image
   HICON m_large_icon;  // 32x32 icon image
   HICON m_small_icon;  // 16x16 icon image
   CString m_title;  // "label"
   CString m_filename;  // complete path to file containing icon
   CString     m_full_pathname;
   int m_lParam;  // caller assigned id
// CString m_orig_name;  // if this is an alias, what is the portion
// that is real? (ie., chum\airport.ico would
// have m_orig_name = "airport.ico"

   enum icon_drawing_mode_t { ICON_NORMAL, ICON_HALFTONE };

   static CMapStringToOb
      m_loaded_images;  // list of already-loaded images
   static CList<CIconAlias*, CIconAlias*>m_aliases;
   // list of already-defined aliases

   static CImageList* m_drag_image;         // drag list to be used for drag operations
   static boolean_t m_cursor_showing;      // indicates whether NO DROP cursor showing
   static boolean_t m_fDragging;         // indicates whether in drag mode
   static HCURSOR m_cursor_no_access;   // circle-slash cursor for NO DROP
   static LPARAM m_lParam_holder;      // holder for caller's LPARAM value

protected:

   // draw the device-independent icon substitute
   int draw_dev_indep_icon(CDC* dc, int x, int y, int width, int height, 
      HICON icon, int new_size = -1);
   icon_dib m_di_icon;  // device-independent icon subsitute
   boolean_t m_di_icon_created;   // TRUE if dev-indep icon has been created
   boolean_t m_di_icon_create_failed; // TRUE if dev-indep icon create failed
   CRect m_di_icon_active_rect;  // Non-blank limits of the DI icon
   
   static boolean_t m_check_subdir;
   icon_drawing_mode_t m_drawing_mode;

public:

   CIconImage()
   {
      m_di_icon_created = FALSE;
      m_di_icon_create_failed = FALSE;
      m_drawing_mode = ICON_NORMAL;
      m_di_icon_active_rect.bottom = -1;  // Not valid
   }

   // get an icon from a CIconImage that is of the appropriate size
   HICON get_icon(int icon_size);
   HICON set_icon( int icon_size, HICON hicon );

   void set_drawing_mode(icon_drawing_mode_t mode) { m_drawing_mode = mode; }

   // Get a rect of the active area of the D.I. icon substitute
   const CRect* get_di_icon_active_rect();

   // read the icons from HD_DATA\icons\item_pathname and populate the
   // members of the CIconImage class.
   static CIconImage* load_images(CString item_pathname, CString item_title="", 
      LPARAM lParam=0, boolean_t always_add = FALSE);

   // cleans up the CMapStringToOb list
   static void clean_up();

   // flag used for threats used to eliminate duplicate icons in subdirectories
   static void check_subdirectories( boolean_t cs ) { m_check_subdir = cs; }

   // centered at the specified CPoint, of the specified Width.  Set new_size
   // to a value greater than 32 to be used to enlarge the icon when printing.
   // E.g, to double the size of the icon, set new_size to 64.
   void draw_icon(CDC *dc, int x, int y, int icon_size, int new_size = -1);

   // draw a HILIGHTED (with grayback background) or SELECTED (with handles) icon of 
   // the appropriate size in the appropriate window, centered at the specified CPoint, 
   // of the specied Width, with optional region invalidation.
   void draw_hilighted(CDC *dc, int x, int y, int icon_size);
   void draw_selected(CDC *dc, int x, int y, int icon_size);


   // draw a bounding box and handles for an icon
   void display_bounds_handles(CDC* dc, int x, int y, int icon_size, BOOL filled);


   // invalidate the display area for an icon, including possible select/highlight areas
   CRect get_invalidate_rect(int x, int y, int icon_size);
   void invalidate(int x, int y, int icon_size);

   // get the item title, filename and lparam
   CString get_item_title() { return m_title; };
   CString get_item_filename() { return m_filename; };
   CString get_item_full_pathname() { return m_full_pathname; }
   int get_lParam() { return m_lParam; };

   void set_item_title(CString new_title) { m_title = new_title; };

   // used for a drag operation of an existing point on the overlay
   //      begin_drag to be called by "select"
   //      move_drag to be called by "drag"
   //      end_drag to be called by "drop"
   int begin_drag(ViewMapProj* map, CPoint point, LPARAM lParam);
   int move_drag(ViewMapProj* map, CPoint point);
   int end_drag(ViewMapProj* map, CPoint point, LPARAM* lParam);
   int cancel_drag(ViewMapProj* map);

   // used by FalconView tool tip "clear" to hide the drag image just before clear
   // and show it again right after (in order to avoid leaving residue
   // from the clear operation.
   int hide_drag();
   int show_drag();

   // used by CMainFrm to preload all overlay icons
   static UINT preload_icons(LPVOID lParam);
   static void load_dir_icons(CString item_directory);

   // used to establish aliases so that we can refer, for example, to "chum\*.ico"
   // when the actual files are "chumred\*.ico". This allows us to maintain
   // consistant filenames (ie., "chum") regardless of some user presentation
   // switch (ie., "red" or "blue" based). 
   //
   // NOTE: When this function is called
   // it loops through all icons looking for m_alias_dir = alias_dir, and
   // replaces the HICON's with IDI_TI_UNKNOWN. Then it scans for all EXISTING
   // icons whose name is orig_dir\*.ico
   // 
   static int set_alias(CString alias_dir, CString orig_dir);
   static CString get_original_directory(CString alias_dir);

#ifdef _DEBUG

   // dumps the list of loaded images to the trace window
   static void dump_list();

#endif

};


//*****************************************************************************
// C_drag_item class 
class C_drag_item
{
public:
   int m_image_num;  // image number in the 
   CString m_title;  // display title of Item
   CDialog* m_which_dialog; // belongs to which icon dialog
   CIconImage* m_item_icon;  // the item icon image class
   LPARAM m_lParam;  // caller defined data

public:
   // get the item title, filename and lparam
   CString get_item_title() { return m_title; };
   CString get_item_filename() { return m_item_icon->get_item_filename(); };
   int get_lParam() { return m_lParam; };

   // get item icon image
   CIconImage* get_item_icon() { return m_item_icon; };
};


//*****************************************************************************
// CIconAlias class 
class CIconAlias
{
public:
   CString m_alias;  // directory for new alias name
   CString m_orig;  // current alias assignment (ie., chum=chumred)
   CList<CIconImage *, CIconImage *> m_alias_list;

public:
   CIconAlias(CString orig, CString alias)
   {
      m_alias = alias;
      m_orig = orig;
   }
};


////////////////////////////////////////////////////////////////////////////////
// C_icon_list template class implementation.
////////////////////////////////////////////////////////////////////////////////

// remove the icon if it is in the list - returns FAILURE if it is not found
template <class C_icon_derived_class, int grow_by>
int C_icon_list<C_icon_derived_class, grow_by>::
   remove(C_icon_derived_class *icon)
{
   POSITION position;

   position = m_list.Find(icon);
   if (position)
   {
      m_list.RemoveAt(position);
      return SUCCESS;
   }

   return FAILURE;
}

// removes and deletes all icons from the list
template <class C_icon_derived_class, int grow_by>
void C_icon_list<C_icon_derived_class, grow_by>::
   delete_all()
{
   C_icon_derived_class *icon;

   while (!m_list.IsEmpty())
   {
      icon = m_list.RemoveHead();
      delete icon;
   }
}

// Returns the first icon in the list - NULL for none.  To continue retrieving
// the icons in order use get_next() repeatedly until it returns NULL.
template <class C_icon_derived_class, int grow_by>
C_icon_derived_class *C_icon_list<C_icon_derived_class, grow_by>::
   get_first()
{
   m_position = m_list.GetHeadPosition();
   if (m_position)
      return m_list.GetNext(m_position);

   return NULL;
}

// Get icons in order (uninterupted) - NULL for none.  Use get_first() to
// retrieve the first icon and initialize get_next() to the second icon in
// the list.
template <class C_icon_derived_class, int grow_by>
C_icon_derived_class *C_icon_list<C_icon_derived_class, grow_by>::
   get_next()
{
   if (m_position)
      return m_list.GetNext(m_position);

   return NULL;
}

// Returns the last icon in the list - NULL for none.  To continue retrieving
// the icons in reverse order use get_prev() repeatedly until it returns NULL.
template <class C_icon_derived_class, int grow_by>
C_icon_derived_class *C_icon_list<C_icon_derived_class, grow_by>::
   get_last()
{
   m_position = m_list.GetTailPosition();
   if (m_position)
      return m_list.GetPrev(m_position);

   return NULL;
}

// Get icons in reverse order (uninterupted) - NULL for none.  Use get_last() to
// retrieve the first icon and initialize get_last() to the second to last icon
// in the list.
template <class C_icon_derived_class, int grow_by>
C_icon_derived_class *C_icon_list<C_icon_derived_class, grow_by>::
   get_prev()
{
   if (m_position)
      return m_list.GetPrev(m_position);

   return NULL;
}

// Returns the icon after previous - NULL for none.
template <class C_icon_derived_class, int grow_by>
C_icon_derived_class *C_icon_list<C_icon_derived_class, grow_by>::
   get_next(C_icon_derived_class *previous)
{
   POSITION position;

   position = m_list.Find(icon);
   if (position)
      return m_list.GetNext(position);

   return NULL;
}

// Returns the icon before next - NULL for none.
template <class C_icon_derived_class, int grow_by>
C_icon_derived_class *C_icon_list<C_icon_derived_class, grow_by>::
   get_prev(C_icon_derived_class *next)
{
   POSITION position;

   position = m_list.Find(icon);
   if (position)
      return m_list.GetPrev(position);

   return NULL;
}

// Returns the first icon - NULL for none.  next is set to the position of the
// next icon in the list - NULL for none.
template <class C_icon_derived_class, int grow_by>
C_icon_derived_class *C_icon_list<C_icon_derived_class, grow_by>::
   get_first(POSITION &next)
{
   next = m_list.GetHeadPosition();
   if (next)
      return m_list.GetNext(next);

   return NULL;
}

// Returns the last icon - NULL for none.  previous is set to the position of
// the previous icon in the list - NULL for none.
template <class C_icon_derived_class, int grow_by>
C_icon_derived_class *C_icon_list<C_icon_derived_class, grow_by>::
   get_last(POSITION &previous)
{
   previous = m_list.GetTailPosition();
   if (previous)
      return m_list.GetPrev(previous);

   return NULL;
}
// end of C_icon_list template class implementation


//*****************************************************************************
// C_icon_list_map template class implementation

// find the icon list for the given CWnd * - returns FAILURE if the key is
// not found
template <class C_icon_list_derived_class>
int C_icon_list_map<C_icon_list_derived_class>::get_icon_list(CWnd *view,
   C_icon_list_derived_class &icon_list)
{
   if (m_map.Lookup(view, icon_list))
      return SUCCESS;

   return FAILURE;
}

// add a icon list CWnd pair
template <class C_icon_list_derived_class>
void C_icon_list_map<C_icon_list_derived_class>::add_pair(CWnd *view, 
   C_icon_list_derived_class *icon_list)
{
   m_map.SetAt(view, icon_list);
}

// removes an icon list CWnd pair, deletes the icon list - returns FAILURE 
// if the key is not is not found
template <class C_icon_list_derived_class>
int C_icon_list_map<C_icon_list_derived_class>::delete_pair(CWnd *view)
{
   C_icon_list_derived_class *icon_list;

   if (m_map.Lookup(view, icon_list))
   {
      m_map.RemoveKey(view);
      delete icon_list;
      return SUCCESS;
   }

   return FAILURE;
}

// removes all pairs and deletes all icon list in the map
template <class C_icon_list_derived_class>
void C_icon_list_map<C_icon_list_derived_class>::delete_all()
{
   POSITION position;
   CWnd *view;
   C_icon_list_derived_class *icon_list;

   position = m_map.GetStartPosition();
   while (position)
   {
      m_map.GetNextAssoc(position, view, icon_list);
      delete icon_list;
   }
}
// end of C_icon_list_map template class implementation


#endif
