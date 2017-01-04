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

#pragma once

#ifndef OVL_MGR_H
#define OVL_MGR_H

#include <set>

#include "FctryLst.h"
#include "common.h"
#include "ovl_d.h"
#include "geo_tool/geo_tool_d.h"
#include "utils.h"

//
// forward declarations
//
class ActiveMap;
class CFvOverlayManagerImpl;
class CMainFrame;
class MapView;
class COverlayBackingStore;
class CVerticalViewProjector;
class MapViewInvalidation;
class SnapToInfo;
class OverlayStackChangedObserver_Interface;

namespace osgEarth
{
   class MapNode;
}

#define MAX_NUM_CLIENT_EDITORS 10

// **********************
// overlay object manager

class EditorEntry
{
public:
   CString m_editor_name;
   CString m_stripped;
   UINT m_id;
   GUID m_overlayDescGuid;
};

class C_ovl_mgr
{
   friend class OvlMgrDl;

   class C3DCatchUpTimer : public FVW_Timer
   {
   private:
      C_ovl_mgr* m_pOverlayManager;

   public:
      C3DCatchUpTimer( C_ovl_mgr* pOverlayManager, int period) :
         FVW_Timer( period ),
         m_pOverlayManager( pOverlayManager )
      {
      }

      // override of base class function
      virtual void expired()
      {
         m_pOverlayManager->set_current_view_time_in_3D( m_pOverlayManager->m_dt3DDeferredDateTime );
      }

   }; // C3DCatchUpTimer

private:

   CComObject<CFvOverlayManagerImpl> *m_pFvOverlayManager;

   // Observers of overlay stack changed events
   CRITICAL_SECTION m_overlay_stack_changed_observers_cs;
   std::set<OverlayStackChangedObserver_Interface *> m_overlay_stack_changed_observers;

   CList<C_overlay *, C_overlay *> m_overlay_list;

   // a map of overlay handles to C_overlay pointers
   CMap <int, int, C_overlay*, C_overlay*> m_overlay_map;

   // the handle that is used for the next added overlay
   int m_next_overlay_handle; 

   C_overlay * m_drag;  // overlay being dragged
   HintText m_icon_hint;  // used to store hint copied from an icon
   C_overlay * m_current;  // the top most overlay
   IFvOverlayEditor*       m_pCurrentEditor; // current editor mode, set to NULL if none
   IFvToolbarPtr           m_spCurrentFvToolbar;   // Only for IFvToolbar-based editors
   GUID                    m_currentEditorGuid; // type of current overlay being edited (can be GUID_NULL)
   CString                 m_csCurrentEditorTypeName;
   boolean_t               m_startup_load_active;
   boolean_t               m_exit_active;
   boolean_t m_show_non_current_ovls_flag;
   boolean_t m_auto_save;

   // Context categories created for a specific editor. Saved so they can be
   // removed when the editor is toggled off.
   std::vector<CMFCRibbonCategory *> m_editorContextCategories;

   std::vector<VIEW_MENU_DATA> m_view_menu_data;
   std::vector<EditorEntry> m_editor_list;

   CString m_startup_overlay;

   // File open dialog's parent window handle
   CWnd *m_file_open_parent_wnd;

   std::shared_ptr<COverlayTypeDescriptorList> m_type_descriptor_list;

   osgEarth::MapNode *m_map_node;

public:
   // methods for multi-threaded overlay rendering
   void InitializeOverlayRenderingThreads();
   HANDLE GetOverlayRenderingSemaphore();
   HANDLE GetTerminateRenderingThreadsEvent();
   COverlayBackingStore* GetBackingStoreFromRenderingQueue();
   void DecrementRemainingOverlaysToRender();

    // Public functions
public:
   // Constructor
   C_ovl_mgr();

   FalconViewOverlayLib::IFvOverlayManager *GetFvOverlayManager();

   // Set the root map node used in 3D drawing
   void SetMapNode(osgEarth::MapNode *map_node) { m_map_node = map_node; }

   void RestoreStartupOverlays();

   // an overlay specified on the command line with the /O option should
   // be set here.  This overlay will be opened in do_first_draw_initialization
   void set_startup_overlay(CString startup) { m_startup_overlay = startup; }

   // Destructor
   ~C_ovl_mgr();

   // Sets the overlay manager's CFrameWndEx from which it gets the map view.
   // Call this function before any overlays are added to the list.
   int initialize();

   // Deletes all overlays currently in the overlay manager's list.  Call this
   // function while the frame and view still exist.
   int exit();

   // Save the user setting for automatically saving the overlay status
   // of all open overlays as program exit.
   void save_on_exit(boolean_t auto_save) { m_auto_save = auto_save; }

   // Writes the current overlay configuration to the registry for restoration
   // at a later time.
   void save_overlay_configuration(const CString& config_name);
   void save_overlay_configuration(const CString& config_name, 
      C_overlay* overlay,
      int& index);

   // get current overlay
   C_overlay *get_current_overlay() { return m_current; }

   // Returns TRUE if a drag is in progress, FALSE otherwise.
   boolean_t is_drag_active() { return m_drag != NULL; }

   // Returns TRUE if the exit member is in progress, FALSE otherwise.
   boolean_t is_exit_active() { return m_exit_active; }

   // Returns TRUE if the given pointer corresponds to an open C_overlay in
   // the overlay manager's list.
   boolean_t is_overlay_valid(C_overlay* ovl);

   // Returns a list of all currently opened overlays.
   int get_overlay_list(CList<C_overlay *, C_overlay *> &list);

   // Sets the display order of all currently opened overlays.  The list must
   // contain all currently opened overlays (see get_overlay_list()).  If the
   // list is valid, the display order will be set according to the list -
   // the head is drawn first, the tail last.
   int reorder_overlay_list(CList<C_overlay *, C_overlay *> &list,
      boolean_t invalidate_flag = TRUE);

   // Closes the overlays in the close_list.  All overlays in the list must be
   // currently opened overlays.  If close_list is valid, all overlays in it
   // will be closed.  canel is set to TRUE if the user cancels the close.
   int close_overlays(CList<C_overlay *, C_overlay *> &close_list, 
      boolean_t *cancel);

   // draws all overlays
   void draw(IActiveMapProj *active_map, BOOL bInternalDraw = FALSE);

   // draws all non-top most overlays
   void draw_top_most_overlays(IActiveMapProj *active_map, BOOL bInternalDraw = FALSE);

   // allow overlays to modify base map
   BOOL can_add_pixmaps_to_base_map();
   void draw_to_base_map(IActiveMapProj *active_map);

   void DrawToVerticalDisplay(CDC *pDC, CVerticalViewProjector* pProjector);

   // draw all overlays from a specific overlay up
   void draw_from_overlay(ActiveMap* map, C_overlay* from_ovl);

   // draw all overlays from a specific overlay up -- determines "dc" itself
   // and calls the other overloaded instance of draw_from_overlay()
   void draw_from_overlay(C_overlay* from_ovl);

   // Set mode of operation.  If release_focus is TRUE the release_edit_focus
   // function will be called for the overlay loosing edit focus.  This
   // parameter must be FALSE when the overlay that had edit focus was closed
   // before the call to this function.
   int set_mode(GUID overlayDescGuid, boolean_t release_focus = TRUE);

   // Returns the mode of operation.
   GUID GetCurrentEditor() {
      return m_currentEditorGuid;
   }
   const CString& GetCurrentEditorTypeName() const { return m_csCurrentEditorTypeName; }
   IFvToolbarPtr GetCurrentFvToolbar() const { return m_spCurrentFvToolbar; }
   void PutCurrentFvToolbar( IFvToolbarPtr spFvToolbar ){ m_spCurrentFvToolbar = spFvToolbar; }
   void PutCurrentEditorTypeName( const CString& csTypeName ){ m_csCurrentEditorTypeName = csTypeName; }

   // get_active_overlay - returns the overlay handle being edited.  If not in
   // an edit mode then NULL is returned, otherwise returns the topmost overlay
   int get_active_overlay_handle()
   {
      if (m_pCurrentEditor == NULL)
         return -1;

      return get_overlay_handle(m_current);
   }

   C_overlay *get_active_overlay_ptr()
   {
      if (m_pCurrentEditor == NULL)
         return NULL;

      return m_current;
   }

   // Get the default cursor as a function of mode.  If in a edit mode (route
   // edit, map data manager, etc.) the default cursor of the associated
   // overlay (get_default_cursor() for the associated overlay) is used.  When
   // the mode is NO_EDIT, the overlay manager's default cursor, a cross hair,
   // is returned.
   HCURSOR get_default_cursor();

   // Calls the invalidate() member of all of the overlays, so they will draw
   // themselves from scratch the next time their draw() member is called.
   // This function must be called each time a new map is displayed.
   void invalidate_overlays(boolean_t invalidate_view = TRUE);

   void invalidate_view(boolean_t erase_background = TRUE);

   // Causes the entire map window to be redrawn completely,
   // erase_background = TRUE will cause the background to be cleared before 
   // the window redraws.  Unless the map specification has changed erase_background 
   // should be FALSE.
   void invalidate_all(boolean_t erase_background = TRUE);

   void InvalidateOverlay(C_overlay* pOverlay);
   void InvalidateOverlaysOfType(GUID overlayDescGuid);

   // Causes the given rectangle in the map window to be redrawn.
   // erase_background = TRUE will cause the background to be cleared
   // the window redraws.  Unless the map specification has changed erase_background 
   // should be FALSE.
   void invalidate_rect(LPCRECT rect, boolean_t erase_background = FALSE,
      boolean_t redraw_overlays_from_scratch = TRUE); 

   // Causes the given CRgn in the map window to be redrawn.
   // erase_background = TRUE will cause the background to be cleared
   // before the window redraws.  erase_background should typically be FALSE.
   void invalidate_rgn(CRgn* rgn, boolean_t erase_background = FALSE);

   // Causes the appropriate rectangle in the map window to be redrawn.
   // erase_background = TRUE will cause the background to be cleared before
   // the window redraws.  A rectangle with width = 2 * expand_by.x and
   // hight = 2 * expand_by.y and centered at point will be invalidated in the
   // map window.
   void invalidate_point(d_geo_t point, CPoint expand_by, 
      boolean_t erase_background = FALSE); 

   // Causes the appropriate rectangle in the map window to be redrawn.
   // erase_background = TRUE will cause the background to be cleared before
   // the window redraws.  A rectangle the size of the geo-rectangle
   // definded by ll and ur will be expanded in the x and y dimensions
   // according to expand_by.  A region the size of this expanded rectangle
   // will be invalidated in the map window.
   void invalidate_region(d_geo_t ll, d_geo_t ur, CPoint expand_by,
      boolean_t erase_background = FALSE);

   // These functions are used to perform a map view invalidation from a worker
   // thread.  Such circumstances should be rare, but in the event that you need
   // to invalidate all or part of the map view window from a worker thread, you
   // must use one of these three functions.  They will invalidate the entire
   // window, the given CRect, or the given CRgn respectively.
   void invalidate_from_thread(boolean_t erase_background);
   void invalidate_from_thread(LPCRECT rect, boolean_t erase_background);
   void invalidate_from_thread(CRgn *rgn, boolean_t erase_background);

   // DO NOT CALL THIS FUNCTION.
   // It should only be called by the view window in response to a 
   // WM_INVALIDATE_FROM_THREAD message.
   void on_invalidate_from_thread(MapViewInvalidation *invalidation);

   // The draw member of the current overlay will be called with the DC of the
   // map window.
   int redraw_current_overlay();

   // adds the given overlay to the proper location in the stack of overlays using the overlay's display order
   int AddOverlayToStack(C_overlay *pOverlay, GUID overlayDescGuid);
   void OnOverlayAdded(C_overlay *overlay, boolean_t invalidate);
   int RemoveOverlayFromStack(C_overlay *pOverlay);

   // add/delete overlays to/from overlay list and display
   int add_overlay(C_overlay *overlay, boolean_t current=TRUE, 
      boolean_t invalidate=TRUE);
   int delete_overlay(C_overlay *overlay, boolean_t invalidate=TRUE,
      boolean_t erase_background = FALSE);

   // make this overlay current - if FV is in any edit mode then the
   // edit mode is updated to reflect the current overlay given
   int make_current(C_overlay *overlay, boolean_t invalidate=TRUE);
   int make_current(CString overlay_name, boolean_t invalidate=TRUE);

   // Change the current overlay and redraw the old overlay
   int change_current( C_overlay* overlay );

   // move this overlay to bottom of list
   int move_to_bottom(C_overlay *overlay, boolean_t invalidate=TRUE);
   int move_to_bottom(CString overlay_name, boolean_t invalidate=TRUE);

   // reorder the overlays so that the layer will end up
   // above the given overlay with the given handle
   int move_above(int layer_handle, int overlay_handle);
   int move_above(C_overlay *pOverlayToMove, C_overlay *pInsertBeforeOverlay);

   // reorder the overlays so that the given layer will end up
   // below the given overlay with the given handle
   int move_below(int layer_handle, int overlay_handle);
   int move_below(C_overlay *pOverlayToMove, C_overlay *pInsertBelowOverlay);

   // Calls the selected() member of each overlay in the list, until an overlay
   // returns SUCCESS or it reaches the end of the list.  A "dragable" overlay 
   // is an overlay with active drag() and drop() members.  If select returns
   // drag == TRUE, then the drag() member will get called until a cancel
   // drag or a drop occurs.  If drag == FALSE then no overlay is to be
   // dragged at this time. If no overlay was selected, then this function
   // returns FAILURE.
   //
   // The cursor parameter is used to overide the default cursor (see the
   // get_default_cursor member function).  When it is set to NULL, the default
   // cursor should be used.
   //
   // The hint parameter is used to overide the default hint (see the
   // get_default_hint member function).  When it is set to NULL, the default
   // hint should be used.  
   //
   // The value returned by the get_hint() function is set to the value
   // returned via the hint parameter.  This includes NULL.
   int select(FalconViewOverlayLib::IFvMapView *pMapView, CPoint point, UINT flags, boolean_t *drag);

   int on_left_mouse_button_up(FalconViewOverlayLib::IFvMapView *pMapView, CPoint point, UINT flags);
   int on_right_mouse_button_down(FalconViewOverlayLib::IFvMapView *pMapView, CPoint point, UINT flags);

   int select_vertical(CVerticalViewProjector* pProj, CPoint point, UINT flags, HCURSOR *pCursor, HintText **ppHint);

   // Sets the cursor and icon (tool-tip and/or help text) according to what 
   // will happen if the same inputs are passed to the select function.
   //
   // The cursor parameter is used to overide the default cursor (see the
   // get_default_cursor member function).  When it is set to NULL, the default
   // cursor should be used.
   //
   // The hint parameter is used to overide the default hint (see the
   // get_default_hint member function).  When it is set to NULL, the default
   // hint should be used.  
   //
   // The value returned by the get_hint() function is set to the value
   // returned via the hint parameter.  This includes NULL.
   int test_select(FalconViewOverlayLib::IFvMapView *pMapView, CPoint point, UINT flags, HintText **hint);

   int test_select_vertical(CVerticalViewProjector* pProj, CPoint point, UINT flags, HCURSOR *pCursor, HintText **ppHint);

   // Allow each overlay to add menu items to the menu based on context.
   void menu(FalconViewOverlayLib::IFvMapView *map_view, const CPoint& point,
      FalconViewOverlayLib::IFvContextMenu *context_menu);

   // Test to see if any overlay can do a single point snap to at this point.
   boolean_t test_snap_to(ViewMapProj* map, CPoint point);

   // Returns TRUE when a snap to object for this point is selected, otherwise
   // it returns FALSE.  If multiple items are under the point the user can
   // select one from a list, or they can select cancel.  If the user selects
   // cancel this function returns FALSE.
   boolean_t do_snap_to(ViewMapProj* map, CPoint point, SnapToInfo *info);

   // Returns TRUE if there is a current overlay otherwise
   // it returns FALSE.  All snap_to items are put in the list
   boolean_t do_snap_to(ViewMapProj* map, CPoint point, CList<SnapToInfo *, SnapToInfo *> &snap_to_list);

   // get overlays of a particular type
   C_overlay* get_first_of_type(GUID overlayDescGuid) const;
   C_overlay* get_next_of_type(const C_overlay* ovl, GUID overlayDescGuid) const;
   C_overlay* get_prev_of_type(C_overlay* ovl, GUID overlayDescGuid) const;
   
   // get a specfic file overlay
   C_overlay* get_file_overlay(GUID overlayDescGuid, const char *filespec);
   
   C_overlay * get_first() const;
   C_overlay * get_next(const C_overlay *ovl) const;
   C_overlay * get_last() const;
   C_overlay * get_prev(const C_overlay *ovl) const;

   // calls OpenFileOverlays but sets up the default class based on the current editor (that's all).  This
   // function needs to be removed.
   int open(C_overlay *&ret_overlay);

   // If show_type_list is TRUE, then the user is presented with a list of
   // overlay types, with the type of the current overlay highlighted by
   // default.  After choosing an overlay type the create() member of the class
   // corresponding to the choosen overlay type will be called.  If the flag
   // is FALSE then it is assumed that an overlay of the current overlay type
   // will be created.  Note: only CFvOverlayPersistenceImpl derived classes are included
   // by this function.
   int create(boolean_t show_type_list = TRUE);

   // Switch to the given overlay's edit mode
   int SwitchToEditor(C_overlay* overlay);

   // If the overlay already exists, it is closed.  Otherwise it is opened.
   int toggle_static_overlay(GUID overlayDescGuid);

   // Open a set of overlays
   int OpenOverlays(std::vector<GUID> &overlayDescGuidList,
      std::vector<std::string> &listFileNames);

   // Open or re-open the given file overlay.  If the overlay is already open,
   // it will become the current overlay.  If it is not opened it will be.  If
   // it returns SUCCESS the specified overlay is now the current overlay.
   int OpenFileOverlay(GUID overlayDescGuid, const char *file_specification, C_overlay *&ret_overlay);

   // Open one or more file overlays from the multi-select file overlay dialog.  The given overlay type is
   // used to initialize the default filter and initial directory in the file dialog (can be GUID_NULL).  The
   // last opened overlay is returned.
   int OpenFileOverlays(GUID overlayDescGuid, C_overlay **overlay);

   CWnd *set_file_open_parent_wnd(CWnd *wnd)
   {
      CWnd *old_wnd = m_file_open_parent_wnd;
      m_file_open_parent_wnd = wnd;
      return old_wnd;
   }

   void RegisterEvents(OverlayStackChangedObserver_Interface* observer);
   void UnregisterEvents(OverlayStackChangedObserver_Interface* observer);

public:

   // Return the user friendly name for the given overlay used throughout the user interface
   CString GetOverlayDisplayName(C_overlay *pOverlay);

   // Calls the create member of the given class.  Note: this function will fail
   // if it is called for a StaticOverlay that is already opened.  Returns
   // FAILURE if an error occurs, SUCCESS otherwise.  The value returned in
   // overlay is a pointer to the newly created overlay, it will be set to NULL
   // when a failure occurs or when the user cancels the operation (function
   // still returns SUCCESS).
   int create(GUID overlayDescGuid, C_overlay **overlay);

   // Presents a list of all opened overlays, with the current overlay
   // highlighted by default.  If an overlay is choosen then the close member
   // of the corresponding class is called.  If file_overlays_only is TRUE,
   // then only file overlays are included in the list.
   int close();

   // Prompt the user to save dirty (is_modified()) files, passes back IDYES, 
   // IDNO, or IDCANCEL in return_val, returns FAILURE if it is unable to save
   // a dirty file.  Note allow_cancel must be TRUE to allow the user to cancel
   // the choose Cancel.
   int save_overlay_msg(C_overlay *pOverlay, int *return_val, boolean_t allow_cancel);

   // If show_overlay_list is TRUE, then the user is presented with a list of
   // open overlays that are dirty, with the top most overlay highlighted by
   // default.  After choosing an overlay the save() member of the overlay
   // is be called.  If the flag is FALSE then the current overlay is saved.
   // Note: only CFvOverlayPersistenceImpl derived classes are included by this function.
   int save(boolean_t show_overlay_list = TRUE);

   // If show_overlay_list is TRUE, then the user is presented with a list of
   // open overlays, with the current overlay highlighted by default.  After
   // choosing an overlay the save_as() member of the overlay is be called.
   // If the flag is FALSE then the save_as() member of the current overlay
   // is called.
   // Note: only CFvOverlayPersistenceImpl derived classes are included by this function.
   int save_as(boolean_t show_overlay_list = TRUE);

   // Does a save for a specific overlay - must be file overlay, OR a static
   // overlay with get_saved() == TRUE and a defined file specification.  That
   // is get_specification must return full file specication for the data file
   // for the given overlay.
   int save(C_overlay *overlay);

   // Does a save_as for a specific file overlay.
   int save_as(C_overlay *overlay, CWnd *pParentWnd = NULL, CString file_specification = "");

   // Save all file overlays.
   int save_all();

   // Get the associated CIconImage class structure for a given overlay
   CIconImage* get_icon_image(C_overlay* pOverlay);

   // Put up the "Reorder Overlays" dialog, to allow the user to change the
   // display order for all currently open overlays.
   int reorder_overlays_dialog();

   // Puts up the "Overlay Options" dialog, to allow the user to change the
   // display options for the different overlay types.  If overlayDescGuid
   // is specified, then the first page that is associated with that overlay
   // type will be selected.  If propertyPageUid is specified, then the page
   // with that Uid will be selected initially.
   int overlay_options_dialog(GUID overlayDescGuid = GUID_NULL, GUID propertyPageUid = GUID_NULL);

   // Returns the C_overlay with the given file specification, if it is
   // already opened.  Otherwise it returns NULL.
   C_overlay *is_file_open(const char *file_specification, GUID overlayDescGuid);

   // This function should be called from the static open member of all file
   // overlay classes.  It will remove the duplicate from the overlay list
   // and return it, if it is found.  It returns NULL otherwise.
   C_overlay *remove_duplicate_file_overlay(C_overlay *overlay);

   // This function is called with the file overlay pointer returned by the
   // remove_duplicate_file_overlay, assuming it isn't NULL.  It will return
   // TRUE if the user chose to revert back to the saved version of duplicate.
   // To revert back you must delete duplicate, which has already been removed
   // from the overlay manager's list by the call to 
   // remove_duplicate_file_overlay, and proceed as usual through your open
   // function.  When revert is not choosen, you must delete the newly created
   // overlay and return duplicate.
   boolean_t test_for_revert(C_overlay *duplicate);

   // Hide or show all overlays other than the current ("de-clutter"). TRUE will
   // enable show of all other overlays, FALSE will disable show.
   void show_other_overlays(boolean_t show_flag);
   boolean_t all_overlays_visible() { return m_show_non_current_ovls_flag; }
   void reset_display_node_visibility();

   // This function is called to add menu items to pMenu for the Overlay
   // Manager... This is used for the the popup menu handlers
   void append_overlay_menu(CMenu* pMenu, boolean_t open_items);

   // Frees the memory used to store the Overlay->View menu item information.
   void free_view_menu_data();

   // Dynamicallly build the "editor" types for the Tools Menu.
   // This function is called by the MainFrm OnInitMenu code
   // handler.
   void BuildEditorRibbonPanel(CMFCRibbonPanel* pEditorsPanel, 
      CMFCRibbonPanel* pMapDataManagerPanel,
      CMFCRibbonCategory* pEditorCategory
      );

   void InitializeEditorList();

   // Toggle the editor implemented by the given overlay type
   void toggle_editor_by_command_id(UINT nCmdId);
   void toggle_editor(GUID overlayDescGuid);
   
   // This function is called by the mainframe to give the overlay mgr a chance to 
   // change the message string that is displayed in the status bar for a menu 
   // item (or tool button).
   // return TRUE if the message is provided. Return FALSE to use the default 
   // implementation.
   boolean_t get_message_string(UINT nID, CString& rMessage);

   // Handle the "Show all overlays" command from the Overlay menu.
   // We need to do two things: (1) make sure that that hide background option
   // is turned off, and (2) togle on every hidden overlay.
   void show_all_overlays();

   // Handles a hit from a menu item generated by "build_overlay_menu".
   void handle_overlay_view_select(int menu_item);

   // Return the overlay type descriptor associated with a given command ID
   OverlayTypeDescriptor* GetOverlayMenuDesc(UINT nID);

   // On behalf of the CMainFrm class, handles the nUpdateOverlayManagerViewList
   // that gets generated by ON_UPDATE_COMMAND_UI_RANGE for the Overlay->View
   // menu items that are dynamically created.
   int overlay_view_update(CCmdUI* pCmdUI);

   // Tests for the existance of a given file spec in the Overlay->View registry
   // files.
   boolean_t test_file_menu(const char* file_spec);

   // Receives all keydown messeges from MapView and passes them to
   // the current editable overlay... a return of false will allow the
   // keystroke to be processed normally in MapView.
   boolean_t offer_keydown(FalconViewOverlayLib::IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
      UINT nFlags);

   // Receives all keyup messeges from MapView and passes them to
   // the current editable overlay... a return of false will allow the
   // keystroke to be processed normally in MapView.
   boolean_t offer_keyup(FalconViewOverlayLib::IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
      UINT nFlags);

   // Receives all ctrl-shift mousewheel messages from MapView and passes them to
   // the current overlay... a return of false will allow the
   // message to be processed normally in MapView.b
   boolean_t offer_mousewheel( FalconViewOverlayLib::IFvMapView *pMapView, SHORT zDelta, UINT nFlags );

   // return the overlay with the display name
   C_overlay *FindOverlayByDisplayName(CString spec)
   {
      POSITION position;
      find_by_name(spec, position);

      if (position)
         return m_overlay_list.GetAt(position);

      return NULL;
   }

   // Look up an overlay by an IFvOverlay interface pointer
   C_overlay *FindOverlay(FalconViewOverlayLib::IFvOverlay *pFvOverlay);

   // returns TRUE if an icon was double-clicked, otherwise FALSE
   BOOL on_dbl_click(FalconViewOverlayLib::IFvMapView *pMapView, UINT flags, CPoint point);

   // give overlay manager a chance to handle the Help Command.
   // returns TRUE if Help was handled. False to let
   // the default handler do the work.
   boolean_t OverrideHelp(UINT nCmdID, UINT nCmd = HELP_CONTEXT);

   // Search for overlay with given specification, return position == NULL if
   // not found. If found position is the position in m_overlay_list.  Note:
   // this function only works for CFvOverlayPersistenceImpl elements.
   void find_by_specification(CString specification, GUID overlayDescGuid, POSITION &position);

   C_overlay *find_by_specification(CString specification, GUID overlayDescGuid)
   {
      POSITION position;
      find_by_specification(specification, overlayDescGuid, position);

      if (position)
         return m_overlay_list.GetAt(position);

      return NULL;
   }

   COverlayTypeDescriptorList* GetTypeDescriptorList()
   {
      return m_type_descriptor_list.get();
   }

// Private Functions
private:
   // Search for overlay with given name return position == NULL if not
   // found. If found position is the position in m_overlay_list.
   void find_by_name(CString name, POSITION &position);

   // Get the position of the first dirty dirty file overlay, NULL for NONE.
   // dirty_overlay_count is set to the number of dirty file overlays.
   POSITION get_dirty_overlay_position(int &dirty_overlay_count);

   // Returns TRUE if the current overlay was changed by this function.
   boolean_t make_current_overlay_match_mode();

   // make the current overlay match the mode, if possible
   void make_current_overlay_match_mode(boolean_t *current_changed,
      boolean_t *current_overlay_matches_mode);

   // make the mode match that of the current overlay.
   int make_mode_match_current_overlay();

   // make mode match that of this overlay
   int make_mode_match_this_overlay(C_overlay *overlay);

   // make this overlay current
   int make_current(C_overlay *overlay, POSITION position, 
      boolean_t invalidate);

   // make this overlay the bottom overlay
   int move_to_bottom(C_overlay *overlay, POSITION position, 
      boolean_t invalidate);

   // Puts up a dialog box to allow the user to select a file or a file specification.  
   // Returns "" for cancel or a full file specification otherwise
   CString get_file_specification_from_user( GUID overlayDescGuid,
      const char *default_name, long *nSaveFormat, CWnd *pParentWnd /* = NULL */);

   int PreCloseOverlay(C_overlay *pOverlay, boolean_t *cancel);

   void SetCurrentEditorGuid(const GUID& editorGuid);

   static CString get_filespec_for_overlay_type(OverlayTypeDescriptor *pOverlayTypeDesc);

   void OnOverlayOrderChanged(BOOL invalidate_flag);

public:
   void OnDirtyFlagChanged(C_overlay* overlay);

public: // needed to make public so the IGPS interface can close a gps trail
   // Close the given overlay.
   int close(C_overlay *overlay);

private:

   // Turn the current editor on or off.  The current mode (m_pCurrentEditor) determines
   // which editor, if any.
   int set_edit_on(boolean_t on);

   // Given a bounding box in device coordinates, the expansion factors
   // (delta_x and delta_y), and the rotation angle this function computes
   // the area of the view that must be invalidated and calls
   // CWnd::InvalidateRect() with the computed rect and the input 
   // erase_background.
   void invalidate_rect(int left, int top, int right, int bottom, int delta_x, 
      int delta_y, double rotation, CWnd *map_view, 
      boolean_t erase_background);

   // Generates a status bar messege for the given item number on the
   // currently displayed Ovleray-view menu.
   //
   CString status_string_for_view_menu(int menu_item);

   // Returns a help string (status bar) that is generated for a specific
   // Overlay Editor menu item in the dynmenu created by "build_editor_menu".
   CString status_string_for_editor(UINT nCmdId);

   // Sets current time in 3D nodes, perhaps deferred
   C3DCatchUpTimer* m_p3DCatchUpTimer;
   DWORD m_dwLastDateTimeUpdateTicks;
   COleDateTime m_dt3DDeferredDateTime;
   void set_current_view_time_in_3D( COleDateTime dateTime );

public:

   // This function will return in TRUE if the current editor mode requires 
   // that the user's ability to rotate the map be disabled.  This function 
   // will return TRUE if any open overlay requires that the ability of the
   // user to rotate the map be disabled.  Otherwise it will return FALSE.
   boolean_t disable_rotation_ui(void);

   // This function will return in TRUE if the current editor mode requires 
   // that the user's ability to change the projection of the map be disabled.  
   // This function will return TRUE if any open overlay requires that the 
   // ability of the user to change the projection of the map be disabled.  
   // Otherwise it will return FALSE.
   boolean_t disable_projection_ui(void);

   // This function will return in TRUE if the current editor mode requires 
   // that the map be North-Up.  This function will return TRUE if any open
   // overlay requires that the map be North-Up.  Otherwise it will return 
   // FALSE.
   boolean_t requires_north_up(void);

   // Returns TRUE if the map must be projected with the EqualArc projection
   // when this editor is active. This function will return TRUE if any open
   // overlay requires that the map be projected with the EqualArc projection.  
   // Otherwise it will return FALSE.  
   boolean_t requires_equal_arc_projection(void);

   // *** VIEW TIME PROCESSING ***

   // This function should step through each overlay ignoring those that are
   // not time-sensitive to get min and max timestamps 
   int get_time_span(COleDateTime& start, COleDateTime& stop);

   // This function will tell each time-sensitive overlay about the new time
   // each overlay will then initiate invalidations and redraws as necessary
   void set_current_view_time(COleDateTime t);

   // other playback notifications
   void OnPlaybackStarted();
   void OnPlaybackStopped();
   void OnPlaybackRateChanged();

   // returns the number of time sensitive overlays opened
   int get_num_time_sensitive_overlays();

   // return the n-th time sensitive overlay
   C_overlay *get_time_sensitive_overlay(int n);

   // paste OLE data object(s) onto an overlay
   boolean_t paste_OLE_data_object(ViewMapProj *map, CPoint pt, 
      COleDataObject* data_object );

   // test to see if we can read clipboard data
   boolean_t can_drop_data( COleDataObject* data_object, CPoint& pt );

   // Update the state of the given overlay editor control
   void OnUpdateOverlayEditor(CCmdUI* pCmdUI);

public:
   // Return the handle of the given overlay.  Returns -1 if not found
   int get_overlay_handle(C_overlay *overlay);

   // Given an overlay handle, return a pointer to an overlay with this handle.
   // Returns NULL if the overlay was not found in the overlay map or if the
   // overlay is not in the overlay manager's list of overlays
   C_overlay *lookup_overlay(int handle);

private:
   // a call to ILayer::RegisterWithMapServer will add a callback pointer
   // to the following list.  Any time an overlay is closed, or whenever FalconView
   // exits, these pointers will be called with the appropriate method to notify
   // the client(s) of these events
   CList <IDispatch FAR*, IDispatch FAR*> m_dispatch_lst;
   void notify_clients_of_close(int overlay_handle, boolean_t program_exiting);
   void notify_clients_of_open(int overlay_handle);
   void notify_clients_of_save(int overlay_handle);

public:
   void notify_clients_of_coverage_update();
   void NotifyClientsFVReady();

public:
   void add_dispatch_ptr(IDispatch FAR* ptr) { m_dispatch_lst.AddTail(ptr); }
   void remove_dispatch_ptr(IDispatch FAR* ptr)
   {
      POSITION position = m_dispatch_lst.Find(ptr);
      if (position)
         m_dispatch_lst.RemoveAt(position);
   }

public:
   // TODO Move to OVl Descriptor
   bool IsTopMostOverlay(C_overlay* overlay);

   int  TopMostOverlayOpacity(C_overlay* overlay);

private:
   int RemoveOverlayFromStack(CList<C_overlay *, C_overlay *> &overlay_list, C_overlay *overlay);
   void DestroyOverlayList(CList<C_overlay *, C_overlay *>& overlay_list);
   void save_overlay_configuration(CList<C_overlay *, C_overlay *>& overlay_list, const CString& config_name);
   POSITION GetTopOverlayPosition(C_overlay* overlay);
   POSITION GetBottomOverlayPosition(C_overlay* overlay);

public:
   ViewMapProj* get_view_map(void);
};
// end C_ovl_mgr definition

// ------------------------------------------------------------------------------
// GLOBAL FUNCTIONS for Overlay Manager

// Returns a point to the overlay manager.  There is only one overlay manager
// per instance of FalconView.  It contains a list of overlays and a list of
// map windows (views).
C_ovl_mgr *OVL_get_overlay_manager();
COverlayTypeDescriptorList *OVL_get_type_descriptor_list();

#endif
