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
#include "include/ovl_mgr.h"

#include "catalog/cov_ovl.h"
#include "catalog/factory.h"
#include "FalconView/CCallback.h"
#include "FalconView/CCallback2.h"
#include "FalconView/FileSpec.h"
#include "FalconView/getobjpr.h"  // for CMapView
#include "FalconView/idialognotify.h"
#include "FalconView/layerovl.h"
#include "FalconView/MapView.h"
#include "FalconView/MissionPackageDlg.h"
#include "FalconView/OvlToolConfiguration.h"
#include "FalconView/StatusBarManager.h"
#include "FalconView/tiptimer.h"
#include "FalconView/VerticalViewDisplay.h"
#include "FvCore/Include/GuidStringConverter.h"
#include "include/ar_edit.h"
#include "include/FctryLst.h"
#include "include/mapx.h"
#include "include/mov_sym.h"
#include "include/nitf.h"
#include "include/optndlg.h"
#include "include/PntExprt.h"
#include "include/prnttool.h"
#include "include/refresh.h"
#include "include/SnapTo.h"
#include "include/TabularEditorDlg.h"
#include "include/TAMask.h"
#include "include/wm_user.h"
#include "overlay/FileDialogEx.h"
#include "overlay/FvOverlayManagerImpl.h"
#include "overlay/OverlayBackingStore.h"
#include "overlay/OverlayCOM.h"
#include "overlay/OverlayEventRouter.h"
#include "overlay/OverlayFactoryCOM.h"
#include "overlay/reorder.h"
#include "overlay/selovl.h"
#include "overlay/snptodlg.h"
#include "PlaybackDialog/viewtime.h"
#include "Scene_mgr/sm_factory.h"
#include "TAMask/factory.h"
#include "FalconView/GenericNodeCallback.h"
#include "OverlayStackChangedObserver_Interface.h"
#include "osgEarth/MapNode"
#include "PlaybackTimeChangedNodeVisitor.h"

#define CURRENT_OVL_MGR_REG_VERSION "3.1.0"

const UINT OVERLAY_EDITOR_CONTEXT_ID = 8000;

//****************************** Static Variables ***************************//

static boolean_t active = FALSE;

// Returns a pointer to the overlay manager.  There is only one overlay manager
// per instance of FalconView.  It contains a list of overlays.
C_ovl_mgr *OVL_get_overlay_manager()
{
   // The Overlay Manager
   static C_ovl_mgr overlay_manager;

   return &overlay_manager;
}

COverlayTypeDescriptorList *OVL_get_type_descriptor_list()
{
   return OVL_get_overlay_manager()->GetTypeDescriptorList();
}


// ******************************************************************

// Constructor    
C_ovl_mgr::C_ovl_mgr() : 
m_pFvOverlayManager(NULL), m_map_node(nullptr)
{
   m_drag = NULL;
   m_current = NULL;
   m_pCurrentEditor = NULL;
   m_currentEditorGuid = GUID_NULL;
   m_show_non_current_ovls_flag = TRUE;
   m_startup_load_active = FALSE;
   m_exit_active = FALSE;
   m_next_overlay_handle = 100;
   m_file_open_parent_wnd = NULL;

   m_type_descriptor_list = std::make_shared<COverlayTypeDescriptorList>();
   ::InitializeCriticalSection(&m_overlay_stack_changed_observers_cs);
}

FalconViewOverlayLib::IFvOverlayManager *C_ovl_mgr::GetFvOverlayManager()
{
   if (m_pFvOverlayManager == NULL)
   {
      CComObject<CFvOverlayManagerImpl>::CreateInstance(&m_pFvOverlayManager);
      m_pFvOverlayManager->Initialize();
      m_pFvOverlayManager->AddRef();
   }

   return m_pFvOverlayManager;
}

// -----------------------------------------------------------------------------

// Destructor
C_ovl_mgr::~C_ovl_mgr()
{
   while (!m_overlay_list.IsEmpty())
      m_overlay_list.RemoveTail()->Release();

   ::DeleteCriticalSection(&m_overlay_stack_changed_observers_cs);
}

// -----------------------------------------------------------------------------

// Sets the overlay manager's CFrameWndEx from which it gets the map view.
// Call this function before any overlays are added to the list.
int C_ovl_mgr::initialize()
{
   CString string;
   boolean_t skip_loads = FALSE;

   m_dwLastDateTimeUpdateTicks = 0;
   m_p3DCatchUpTimer = new C3DCatchUpTimer( this, 2000 );   // If no more updates in 2 secs
   UTL_get_timer_list()->add( m_p3DCatchUpTimer );

   // load the overlay order values from the registry
   OVL_get_type_descriptor_list()->LoadDefaultDisplayOrder();

   // find out if our registry section is valid
   string = PRM_get_registry_string("Overlay Manager", 
      "Section version", "1.0");

   // if this registry section is not 2.0, then delete the entire section
   // so that it gets rebuilt
   if (string != CURRENT_OVL_MGR_REG_VERSION)
   {
      if (string == "1.0")
      {
         if (PRM_delete_registry_section("Overlay Manager\\Display Order2") != SUCCESS)
            ERR_report("Overlay manager: could not delete current user's outdated registry section.");
         if (PRM_delete_registry_section("Overlay Manager\\Info") != SUCCESS)
            ERR_report("Overlay manager: could not delete current user's outdated registry section.");
         skip_loads = TRUE;
      }
      else if (string == "3.0.0"  ||  string == "3.0.1"  ||  
         string == "3.0.2" ||  string == "3.0.3"  ||  string == "3.0.4")
      {
         // changed the SAVE attribute for Local points and NON_RESTORE
         // attributes were used for the first time -- so they needed to
         // be established as defaults for all old users.
         if (PRM_delete_registry_section("Overlay Manager\\Info") != SUCCESS)
            ERR_report("Overlay manager: could not delete current user's outdated registry section.");
         skip_loads = TRUE;
      }

      // write out the string to the registry
      PRM_set_registry_string("Overlay Manager\\", 
         "Section version", CURRENT_OVL_MGR_REG_VERSION);
   }

   // read the auto_save variable from the registry
   string = PRM_get_registry_string("Overlay Manager\\Settings", 
      "AutoSave", "Yes");
   m_auto_save = string == "Yes";

   // initialize mode to no edit mode
   m_pCurrentEditor = NULL;
   m_currentEditorGuid = GUID_NULL;

   // start the thread that will pre-load all of the overlay icons
   AfxBeginThread(CIconImage::preload_icons, 0);

   return SUCCESS;
}


// -----------------------------------------------------------------------------

// Deletes all overlays currently in the overlay manager's list.  Call this
// function while the frame and view still exist.
int C_ovl_mgr::exit()
{
   // indicate the overlay manager exit is in progress
   m_exit_active = TRUE;

   // save the current display_order information to the registry
   OVL_get_type_descriptor_list()->SaveDefaultDisplayOrder();

   // free up the previous menu_data array, if it exists
   free_view_menu_data();

   // close the connection to SkyView if it is still open
   Cmov_sym_overlay::destroy_skyview_interface();

   // reset to unitialized state before destroying overlay list
   m_pCurrentEditor = NULL;
   m_currentEditorGuid = GUID_NULL;

   // Destroy the overlays
   DestroyOverlayList(m_overlay_list);

   // destroy the tabular editor dialog if it is still opened
   if (g_tabular_editor_dlg != NULL)
   {
      g_tabular_editor_dlg->m_data_object = NULL;
      CTabularEditorDlg::destroy_dialog();
   }

   // [IKC #2002-0929].  Even if there are no overlays opened, the client would
   // still like to receive the OnFalconViewExit callback.  We do this by, sending
   // an overlay handle of -1.
   notify_clients_of_close(-1, TRUE);

   // free up the previous menu_data array, if it exists
   free_view_menu_data();

   // Clean up the CIconImage buffers
   CIconImage::clean_up();

   // reset everything to unitialized state
   m_drag = NULL;
   m_current = NULL;
   m_show_non_current_ovls_flag = TRUE;
   m_startup_load_active = FALSE;
   m_exit_active = FALSE;

   if (m_pFvOverlayManager != NULL)
      m_pFvOverlayManager->Release();

   m_overlay_map.RemoveAll();

   POSITION pos = UTL_get_timer_list()->Find( m_p3DCatchUpTimer );
   if ( pos != NULL)
      UTL_get_timer_list()->remove_at( pos );
   delete m_p3DCatchUpTimer;
   m_p3DCatchUpTimer = NULL;
   
   return SUCCESS;
}

void C_ovl_mgr::DestroyOverlayList(CList<C_overlay *, C_overlay *>& overlay_list)
{
   const CString auto_save_config_name = "Startup2"; 

   // if necessary, write out the overlay status of all open overlays
   // so that they can be restored at program startup
   if (m_auto_save)
   {
      // start by deleting the entire section, even if the list is 
      // empty (which will mean they startup with no open overlays
      PRM_delete_registry_section("Overlay Manager\\" + auto_save_config_name);
   }

   // To avoid a crash in the case that an overlay changes the order of overlays in pre_close we
   // copy the list of overlays to a temporary buffer
   CList<C_overlay *, C_overlay *> lcl_overlay_list;
   lcl_overlay_list.AddTail(&overlay_list);

   overlay_list.RemoveAll();

   // close/destroy all overlays
   int index = 0;
   while (!lcl_overlay_list.IsEmpty()) 
   {
      // remove head element
      C_overlay* overlay = lcl_overlay_list.RemoveHead();
      const int overlay_handle = get_overlay_handle(overlay);

      // keep m_current valid while items are being removed and deleted
      if (lcl_overlay_list.IsEmpty())
         m_current = NULL;
      else
         change_current( lcl_overlay_list.GetHead() );

      // add the overlay back temporarily so that plug-in overlays can
      // still get back at the overlay with an overlay handle
      overlay_list.AddHead(overlay);
      PreCloseOverlay(overlay, NULL);

      // call exit member of all overlays to notify them that FV is exiting
      if (overlay->exit() != SUCCESS)
      {
         CString msg;

         string_utils::CGuidStringConverter guidConv(overlay->get_m_overlayDescGuid());
         msg.Format("Overlay of type %s failed on exit()", guidConv.GetGuidString().c_str());
         ERR_report(msg);
      }

      if (m_auto_save)
      {
         save_overlay_configuration(auto_save_config_name, overlay, index);
      }

      // tell any connected clients that FalconView is about to exit
      notify_clients_of_close(overlay_handle, TRUE);

      delete_overlay(overlay, FALSE);
      overlay->Release();
      overlay = NULL;
   }

   if (m_auto_save)
   {
      // write out the number of overlays that were put into the registry
      PRM_set_registry_int("Overlay Manager\\" + auto_save_config_name, 
         "Overlay Count", index);
   }
}

// -----------------------------------------------------------------------------

void C_ovl_mgr::save_overlay_configuration(const CString& config_name)
{
   // start by deleting the entire section, even if the list is
   // empty (which will mean they startup with no open overlays
   PRM_delete_registry_section("Overlay Manager\\" + config_name);

   // write out the current collection of overlays to the registry, indicating
   // (a) the overlay class_name, (b) the overlay file_name (if implements IFvOverlayPersistence),
   // and (c) the display order.
   int index = 0;
   POSITION position = m_overlay_list.GetHeadPosition();
   while (position)
   {
      C_overlay* overlay = m_overlay_list.GetNext(position);

      // if this is an unnamed overlay, then the user MUST name it before it can be auto-saved.
      OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
      {
         long bHasBeenSaved = 0;
         long bIsDirty = 0;
         pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);
         pFvOverlayPersistence->get_m_bIsDirty(&bIsDirty);
         if (!bHasBeenSaved)
         {
            // if this overlay is not dirty, then just skip it
            if (!bIsDirty)
               continue;
            // ask the user to save the overlay
            int return_code;
            if (save_overlay_msg(overlay, &return_code, FALSE) != SUCCESS ||
                return_code == IDCANCEL)
            {
               continue;
            }
         }
      }

      save_overlay_configuration(config_name, overlay, index);
   }

   // write out the number of overlays that were put into the registry
   PRM_set_registry_int("Overlay Manager\\" + config_name,
                        "Overlay Count", index);
}

// Writes the current overlay configuration to the registry for restoration
// at a later time.
void C_ovl_mgr::save_overlay_configuration(const CString& config_name, 
                                           C_overlay* overlay, int& index)
{
   CString overlay_stats;
   const int IDENX_STR_LEN = 5;
   char index_str[IDENX_STR_LEN];

   OverlayPersistence_Interface* pFvOverlayPersistence = 
      dynamic_cast<OverlayPersistence_Interface *>(overlay);

   string_utils::CGuidStringConverter guidConv(
      overlay->get_m_overlayDescGuid());

   // grab the show/hide flag first
   overlay_stats = overlay->get_m_bIsOverlayVisible() ?  "Show|" : "Hide|";

   // if it is a file overlay, then write the classname, comma
   // then filename
   if (pFvOverlayPersistence != NULL)
   {
      // if the overlay is still dirty this means the user chose not to save
      // the overlay. In this case, we will not write this entry
      long dirty;
      pFvOverlayPersistence->get_m_bIsDirty(&dirty);
      if (dirty)
      {
         return;
      }

      _bstr_t fileSpecification;
      pFvOverlayPersistence->get_m_fileSpecification(
         fileSpecification.GetAddress());

      overlay_stats += guidConv.GetGuidString().c_str();
      overlay_stats += "|";
      overlay_stats += (char *)fileSpecification;
   }
   // otherwise, just write the classname
   else
   {
      overlay_stats += guidConv.GetGuidString().c_str();
   }

   _itoa_s(index++, index_str, IDENX_STR_LEN, 10);
   PRM_set_registry_string("Overlay Manager\\" + config_name, index_str, 
                           overlay_stats);
}

// Returns TRUE if the given pointer corresponds to an open C_overlay in
// the overlay manager's list.
boolean_t C_ovl_mgr::is_overlay_valid(C_overlay* ovl)
{
   OverlayEventRouter router(CListToVector<>(m_overlay_list), 
      !all_overlays_visible(), m_current);

   return router.IsOverlayValid(ovl) ? TRUE : FALSE;
}

// -----------------------------------------------------------------------------

// Returns a list of all currently opened overlays.
int C_ovl_mgr::get_overlay_list(CList<C_overlay *, C_overlay *> &list)
{
   POSITION previous;
   C_overlay *overlay;

   if (!list.IsEmpty())
   {
      ERR_report("The overlay list is not empty.");
      return FAILURE;
   }

   // get all overlays
   if (!m_overlay_list.IsEmpty())
   {
      previous = m_overlay_list.GetTailPosition();
      do
      {
         overlay = m_overlay_list.GetPrev(previous);
         list.AddHead(overlay);
      } while (previous != NULL);
   }

   return SUCCESS;
}


// -----------------------------------------------------------------------------

// Sets the display order of all currently opened overlays.  The list must
// contain all currently opened overlays (see get_overlay_list()).  If the
// list is valid, the display order will be set according to the list -
// the head is drawn first, the tail last.
int C_ovl_mgr::reorder_overlay_list(CList<C_overlay *, C_overlay *> &list,
   boolean_t invalidate_flag)
{
   POSITION position;
   C_overlay *overlay;

   // make sure list is the same size as m_overlay_list, list must contain the
   // same elements
   if (list.GetCount() != m_overlay_list.GetCount())
   {
      ERR_report("Invalid number of elements.");
      return FAILURE;
   }

   // make sure all overlays in list are currently opened
   position = list.GetHeadPosition();
   while (position)
   {
      overlay = list.GetNext(position);
      if (m_overlay_list.Find(overlay) == NULL)
      {
         ERR_report("Invalid overlay.");
         return FAILURE;
      }
   }

   // copy list into the overlay manager's list
   m_overlay_list.RemoveAll();
   position = list.GetHeadPosition();
   while (position)
   {
      overlay = list.GetNext(position);
      m_overlay_list.AddTail(overlay);
   }

   OnOverlayOrderChanged(invalidate_flag);

   return SUCCESS;
}

void C_ovl_mgr::OnOverlayOrderChanged(BOOL invalidate_flag)
{
   // change the current overlay
   if (m_overlay_list.GetHeadPosition())
      change_current( m_overlay_list.GetHead() );

   // switch to NO_EDIT mode if the current overlay does not match
   // the current mode.
   IFvOverlayEditor *pEditor = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(m_current->get_m_overlayDescGuid())->pOverlayEditor;
   if (m_pCurrentEditor != pEditor || !m_current->get_m_bIsOverlayVisible())
      set_mode(GUID_NULL);

   // redraw the display
   if (invalidate_flag)
      invalidate_view(FALSE);

   // update the title bar
   FVW_update_title_bar();

   if (g_tabular_editor_dlg && m_current)
   {
      g_tabular_editor_dlg->update_overlay_tabs(CTabularEditorDlg::TAB_UPDATE_CHANGE_CURRENT,
         GetOverlayDisplayName(m_current), m_current);

      TabularEditorDataObject* pDataObject = m_current->GetTabularEditorDataObject();
      if (pDataObject != NULL)
      {
         g_tabular_editor_dlg->set_data_object(pDataObject);
         g_tabular_editor_dlg->on_selection_changed();
      }
   }

   // Notify any observers that the overlay list has been reordered
   {
      fv_common::ScopedCriticalSection lock(
         &m_overlay_stack_changed_observers_cs);

      for (auto it=std::begin(m_overlay_stack_changed_observers);
         it != std::end(m_overlay_stack_changed_observers); ++it)
      {
         try
         {
            (*it)->OverlayOrderChanged();
         }
         catch(_com_error& e)
         {
            // report error and contnue
            REPORT_COM_ERROR(e);
         }
      }
   }
}

void C_ovl_mgr::OnDirtyFlagChanged(C_overlay* overlay)
{
   fv_common::ScopedCriticalSection lock(
         &m_overlay_stack_changed_observers_cs);

   // Notify any observers that the overlay list has been reordered
   for (auto it=std::begin(m_overlay_stack_changed_observers);
      it != std::end(m_overlay_stack_changed_observers); ++it)
   {
      try
      {
         (*it)->OverlayDirtyChanged(overlay);
      }
      catch(_com_error& e)
      {
         // report error and contnue
         REPORT_COM_ERROR(e);
      }
   }
}

// -----------------------------------------------------------------------------

// Closes the overlays in the close_list.  All overlays in the list must be
// currently opened overlays.  If close_list is valid, all overlays in it
// will be closed.
int C_ovl_mgr::close_overlays(CList<C_overlay *, C_overlay *> &close_list,
   boolean_t *cancel)
{
   boolean_t change;
   boolean_t match;
   POSITION position;
   C_overlay *overlay;

   // make sure all overlays in close_list are currently opened
   CList<C_overlay*, C_overlay *> local_close_list;
   position = close_list.GetHeadPosition();
   while (position)
   {
      overlay = close_list.GetNext(position);
      if (m_overlay_list.Find(overlay) == NULL)
      {
         ERR_report("Invalid overlay.");
         return FAILURE;
      }
      local_close_list.AddTail(overlay);
   }

   // do pre-close for all overlays and handle cancel
   position = local_close_list.GetHeadPosition();
   while (position)
   {
      overlay = local_close_list.GetNext(position);

      if (m_overlay_list.Find(overlay) == NULL)
        continue;

      //[Bug 4018] New: Unsubscribe not called when user does "Close All"
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL) 
      {
         CMainFrame* frame = fvw_get_frame();
         if ( frame && frame->m_pCollaborationServer ) 
         {
            // make sure this overlay does not exist in any collaborations
            int hoverlay = get_overlay_handle(overlay);

            _bstr_t fileSpecification;
            pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

            frame->m_pCollaborationServer->Unsubscribe(hoverlay, fileSpecification, NULL);
         }
      }

      // inform the overlay that it is about to be closed so it can tie up any
      // loose ends before it is destroyed
      if (PreCloseOverlay(overlay, cancel) != SUCCESS)
      {
         ERR_report("pre_close() failed.");
         return FAILURE;
      }

      // if the close has been canceled then abort
      if (*cancel)
         return SUCCESS;

      // tell any connected clients that this overlay is about to be closed
      notify_clients_of_close(get_overlay_handle(overlay), FALSE);
   }

   // close all the overlays in close list
   position = local_close_list.GetHeadPosition();
   while (position)
   {
      overlay = local_close_list.GetNext(position);

      if (m_overlay_list.Find(overlay) != NULL)
      {
         delete_overlay(overlay, FALSE);
         overlay->Release();
      }
   }

   // maintain the same mode if an overlay of the corresponding type exists,
   // otherwise switch to NULL mode
   make_current_overlay_match_mode(&change, &match);
   if (!match && m_pCurrentEditor != NULL)
      set_mode(GUID_NULL, FALSE);

   // set the user interface state based on overlay/editor requirements
   FVW_refresh_view_map_state();

   // redraw the display
   invalidate_all(FALSE);

   return SUCCESS;
}

namespace
{
class DrawInterruptedCallback : public OverlayEventRouterCallback_Interface
{
public:
   virtual bool HaltOnDrawEventRouting() override
   {
      return FVW_is_draw_interrupted() == TRUE;
   }
};
}

// draw all overlays
void C_ovl_mgr::draw(IActiveMapProj *active_map, BOOL bInternalDraw/*=FALSE*/)
{
   DrawInterruptedCallback callback;

   OverlayEventRouter router(CListToVector<>(m_overlay_list), 
      !all_overlays_visible(), m_current, &callback);

   router.OnDrawOverlays(active_map);

   // For now, any invalidation of the map window will cause the entire
   // vertical view to become invalidated
   CMainFrame* frame = fvw_get_frame();
   if (bInternalDraw && frame && frame->GetVerticalDisplay() != nullptr)
   {
      // Updates the Vertical View backing store.  Only do for internal draws
      frame->GetVerticalDisplay()->RedrawVerticalDisplay();
   }

   // Give the view time dialog a chance to draw the time on the screen
   ActiveMap_TMP activeMapWrapper(active_map);
   CMainFrame::GetPlaybackDialog().draw(&activeMapWrapper);
}

// draw all top most overlays
void C_ovl_mgr::draw_top_most_overlays(IActiveMapProj *active_map, BOOL bInternalDraw/*=FALSE*/)
{
   DrawInterruptedCallback callback;

   // draw top most overlays
   OverlayEventRouter top_most_overlays_router(CListToVector<>(m_overlay_list), 
      false, nullptr, &callback);

   top_most_overlays_router.OnDrawTopMostOverlays(active_map);


   // For now, any invalidation of the map window will cause the entire
   // vertical view to become invalidated
   CMainFrame* frame = fvw_get_frame();
   if (bInternalDraw && frame && frame->GetVerticalDisplay() != nullptr)
   {
      // Updates the Vertical View backing store.  Only do for internal draws
      frame->GetVerticalDisplay()->RedrawVerticalDisplay();
   }

   // Give the view time dialog a chance to draw the time on the screen
   ActiveMap_TMP activeMapWrapper(active_map);
   CMainFrame::GetPlaybackDialog().draw(&activeMapWrapper);
}

BOOL C_ovl_mgr::can_add_pixmaps_to_base_map()
{
   OverlayEventRouter router(CListToVector<>(m_overlay_list), false, nullptr);
   return router.CanAddPixmapsToBaseMap() ? TRUE : FALSE;
}

// allow overlays to modify base map
void C_ovl_mgr::draw_to_base_map(IActiveMapProj *active_map)
{
   DrawInterruptedCallback callback;

   OverlayEventRouter router(CListToVector<>(m_overlay_list), 
      !all_overlays_visible(), m_current, &callback);

   router.OnDrawOverlaysToBaseMap(active_map);
}

void C_ovl_mgr::DrawToVerticalDisplay(CDC *dc,
   CVerticalViewProjector* vertical_view_proj)
{
   DrawInterruptedCallback callback;

   OverlayEventRouter router(CListToVector<>(m_overlay_list), 
      !all_overlays_visible(), m_current, &callback);

   router.DrawToVerticalDisplay(dc, vertical_view_proj);
}


// -----------------------------------------------------------------------------

// draw all overlays from a specific overlay up
void C_ovl_mgr::draw_from_overlay(ActiveMap* map, C_overlay* from_ovl)
{
   // invalidate and immediately redraw
   invalidate_all();
   fvw_get_view()->UpdateWindow();
}


// -----------------------------------------------------------------------------

// draw all overlays from a specific overlay up -- determines "dc" itself
// and calls the other overloaded instance of draw_from_overlay()
void C_ovl_mgr::draw_from_overlay(C_overlay* from_ovl)
{
   // otherwise, if there was a FAILURE doing the GRA_dc stuff, we'll
   // invalidate all and let the Draw function handle it
   invalidate_view(FALSE);
}

// -----------------------------------------------------------------------------

void C_ovl_mgr::reset_display_node_visibility()
{
   auto overlays = CListToVector<>(m_overlay_list);
   std::for_each(overlays.begin(), overlays.end(),[](C_overlay* overlay)
   {
      overlay->ResetDisplayNodeVisibility();
   });
}

void C_ovl_mgr::show_other_overlays(boolean_t show_flag)
{ 
   // turn off the internal flag
   m_show_non_current_ovls_flag = show_flag;

   // if we are going to show the background... then just draw
   // them all; otherwise we need to erase all of the background
   // overlays from the display and redraw the top (the redraw happens 
   // automatically)
   invalidate_all(FALSE);
}

ViewMapProj* C_ovl_mgr::get_view_map(void)
{
   MapView *view = fvw_get_view();

   if (view)
      return view->get_curr_map();

   return nullptr;
}

// Calls the invalidate() member of all of the overlays, so they will draw
// themselves from scratch the next time their draw() member is called.
// This function must be called each time a new map is displayed.
void C_ovl_mgr::invalidate_overlays(boolean_t invalidate_view /*=TRUE*/)
{
   OverlayEventRouter router(CListToVector<>(m_overlay_list), 
      !all_overlays_visible(), m_current);
   router.InvalidateOverlays();

   if (invalidate_view)
      invalidate_all();
}

void C_ovl_mgr::invalidate_view(boolean_t erase_background /* = TRUE */)
{
   CWnd *window = fvw_get_view();

   //
   // Note: make sure to allow animation override here so that, for example,
   // the screen doesn't flash when the map automatically recenters in gps 
   // tracking mode
   //
   if (window)
      UTL_invalidate_view(window, erase_background, TRUE, TRUE);

   // for now, any invalidation of the map window will cause the entire
   // Vertical Display to become invalidated
   CMainFrame* frame = fvw_get_frame();
   if (frame && frame->GetVerticalDisplay() != NULL)
      frame->GetVerticalDisplay()->Invalidate();
}

// Causes the entire map window to be redrawn completely,
// erase_background = TRUE will cause the background to be cleared before 
// the window redraws.  Unless the map specification has changed erase_background 
// should be FALSE.
void C_ovl_mgr::invalidate_all(boolean_t erase_background)
{
   POSITION position = m_overlay_list.GetHeadPosition();
   while (position)
      m_overlay_list.GetNext(position)->invalidate();

   invalidate_view(erase_background);
}

void C_ovl_mgr::InvalidateOverlay(C_overlay* pOverlay)
{
   if (pOverlay != NULL)
      pOverlay->invalidate();

   invalidate_view();
}

void C_ovl_mgr::InvalidateOverlaysOfType(GUID overlayDescGuid)
{
   int nNumOverlaysInvalidated = 0;

   POSITION position = m_overlay_list.GetHeadPosition();
   while (position)
   {
      C_overlay* pOverlay = m_overlay_list.GetNext(position);
      if (pOverlay->get_m_overlayDescGuid() == overlayDescGuid)
      {
         pOverlay->invalidate();
         nNumOverlaysInvalidated++;
      }
   }

   if (nNumOverlaysInvalidated > 0)
      invalidate_view();
}


// -----------------------------------------------------------------------------

// Causes the given rectangle in the map window to be redrawn.
// erase_background = TRUE will cause the background to be cleared
// before the window redraws.  erase_background should typically be FALSE.
void C_ovl_mgr::invalidate_rect(LPCRECT rect, boolean_t erase_background,
   boolean_t redraw_overlays_from_scratch /*=TRUE*/)
{
   CWnd *window = fvw_get_view();

   // no need to invalid a degenerate rect
   if (rect->bottom == 0 && rect->left == 0 && rect->top == 0 &&
      rect->right == 0)
      return;

   if (window)
      UTL_invalidate_view_rect(window, rect, erase_background,
      FALSE, redraw_overlays_from_scratch);

   // for now, any invalidation of the map window will cause the entire
   // vertical view to become invalidated
   CMainFrame* frame = fvw_get_frame();
   if (frame && frame->GetVerticalDisplay() != NULL)
      frame->GetVerticalDisplay()->Invalidate();
}

// -----------------------------------------------------------------------------

// Causes the given CRgn in the map window to be redrawn.
// erase_background = TRUE will cause the background to be cleared
// before the window redraws.  erase_background should typically be FALSE.
void C_ovl_mgr::invalidate_rgn(CRgn* rgn, boolean_t erase_background)
{
   CWnd *window = fvw_get_view();

   if (window)
      UTL_invalidate_view_rgn(window, rgn, erase_background,
      FALSE, TRUE);

   // for now, any invalidation of the map window will cause the entire
   // vertical view to become invalidated
   CMainFrame* frame = fvw_get_frame();
   if (frame && frame->GetVerticalDisplay() != NULL)
      frame->GetVerticalDisplay()->Invalidate();
}


// -----------------------------------------------------------------------------

// Causes the appropriate rectangle in the map window to be redrawn.
// erase_background = TRUE will cause the background to be cleared before
// the window redraws.  A rectangle with width = 2 * expand_by.x and
// hight = 2 * expand_by.y and centered at point will be invalidated in the
// map window.
void C_ovl_mgr::invalidate_point(d_geo_t point, CPoint expand_by, 
   boolean_t erase_background)
{
   CRect rect;
   int x, y;

   MapProj* proj = get_view_map();

   // get the view coordinate of the point for this view
   proj->geo_to_surface(point.lat, point.lon, &x, &y);

   // initialize the invalidate "region" to be the point
   rect.SetRect(x, y, x, y);      

   // inflate rectangle to bound the point
   rect.InflateRect(expand_by.x, expand_by.y);

   // invalidate the rectangle if it is not empty
   if (fvw_get_view() && !rect.IsRectEmpty())
      UTL_invalidate_view_rect(fvw_get_view(), rect, erase_background,
      FALSE, TRUE);

   // for now, any invalidation of the map window will cause the entire
   // vertical view to become invalidated
   CMainFrame* frame = fvw_get_frame();
   if (frame && frame->GetVerticalDisplay() != NULL)
      frame->GetVerticalDisplay()->Invalidate();
}


// -----------------------------------------------------------------------------

// Causes the appropriate rectangle in the map window to be redrawn.
// erase_background = TRUE will cause the background to be cleared before
// the window redraws.  A rectangle the size of the geo-rectangle
// definded by ll and ur will be expanded in the x and y dimensions
// according to expand_by.  A region the size of this expanded rectangle
// will be invalidated in the map window.
void C_ovl_mgr::invalidate_region(d_geo_t ll, d_geo_t ur, CPoint expand_by,
   boolean_t erase_background)
{
   int x1, y1, x2, y2;
   double rotation;

   MapProj* map = get_view_map();

   // get the rotation angle for this view
   rotation = map->actual_rotation();

   // get the device coordinates of the corner points for this view
   map->geo_to_vsurface(ll.lat, ll.lon, &x1, &y2);   // south west
   map->geo_to_vsurface(ur.lat, ur.lon, &x2, &y1);   // north east

   // x1 <= x2 unless the rectangle wraps around the world,
   // in that special case two regions will have to be invalidated
   if (x1 <= x2)
   {
      // invalidate the region defined by the device equivalent of the
      // geo bounds and the device dimensions the geo region must be
      // expanded by.
      invalidate_rect(x1, y1, x2, y2, expand_by.x, expand_by.y, rotation,
         fvw_get_view(), erase_background);
   }
   else
   {
      int pixels_around_world;

      map->get_pixels_around_world(&pixels_around_world);

      // invalidate the region on the left edge of the projection
      invalidate_rect(x1, y1, x2 + pixels_around_world, y2, 
         expand_by.x, expand_by.y, rotation, fvw_get_view(), erase_background);

      // invalidate the region on the right edge of the projection
      invalidate_rect(x1 - pixels_around_world, y1, x2, y2, 
         expand_by.x, expand_by.y, rotation, fvw_get_view(), erase_background);
   }
}


// -----------------------------------------------------------------------------

// The draw member of the current overlay will be called with the DC of the
// map window.
int C_ovl_mgr::redraw_current_overlay()
{
   if (m_current != NULL)
   {
      m_current->invalidate();
      invalidate_all();
   }

   return SUCCESS;
}

// adds the given overlay to the proper location in the stack of overlays using
// the overlay's display order.  Note the overlay's descriptor guid is not yet
// valid
int C_ovl_mgr::AddOverlayToStack(C_overlay *pOverlay, GUID overlayDescGuid)
{
   // find the position in the display order list for this overlay
   const int displayOrder =
      OVL_get_type_descriptor_list()->GetDisplayOrder(overlayDescGuid);

   // The overlay class was not found and incompletely added to the project
   if (displayOrder == -1)
   {
      ASSERT(0);
      ERR_report("Invalid overlay class name.");
      return FAILURE;
   }

   // This is the new overlays are biased to the top approach.
   // It is to insure that the newly added overlay will be above
   // all open overlays that it is supposed to be above (after), 
   // according to the default display order.  However, it may
   // put the overlay above some overlays that the user has moved
   // down in the display order.  Hence this is a top-biased
   // approach.
   POSITION position;
   POSITION insert_before = NULL;
   C_overlay *next_lowest = NULL;
   boolean_t not_found = TRUE;

   // Normally the search starts with the current or top overlay; 
   // however, if an editor is active then the current overlay should
   // be left unchanged.
   position = GetTopOverlayPosition(pOverlay);

   if (position && m_pCurrentEditor != NULL && !IsTopMostOverlay(pOverlay))
   {
      long lReadOnly = 0;
      OverlayPersistence_Interface *pFvOverlayPersistence =
         dynamic_cast<OverlayPersistence_Interface *>(pOverlay);
      if (pFvOverlayPersistence != NULL)
         pFvOverlayPersistence->get_m_bIsReadOnly(&lReadOnly);

      // But if the current mode matches the mode of the added overlay
      // and that overlay is not read-only, then it will be made current.
      IFvOverlayEditor *pEditor = OVL_get_type_descriptor_list()->
         GetOverlayTypeDescriptor(overlayDescGuid)->pOverlayEditor;
      if (m_pCurrentEditor != pEditor || lReadOnly)
         m_overlay_list.GetNext(position);

      // This case should only happen on REVERT... case is when editor
      // is already open though no overlays currently exist in the overlay
      // list, and an overlay of the current edtior's type is opened...
      // stick it at the very top.
      else if (m_pCurrentEditor == pEditor && !lReadOnly)
      {
         insert_before = position;
         not_found = FALSE;
      }
   }

      if (position && IsTopMostOverlay(pOverlay))
   {
      // reset to the top
      position = m_overlay_list.GetHeadPosition();

      // Search for the top most open overlay that the newly open
      // overlay belongs above.
      while (position && not_found)
      {
         // save position of the overlay you are about to get, in
         // case it is the overlay you are looking for
         insert_before = position;

         // get the next overlay moving from the head (top) toward
         // the tail (bottom)
         next_lowest = m_overlay_list.GetNext(position);

         if (IsTopMostOverlay(next_lowest))
         {
            // if next_lowest comes before the new overlay according
            // to the default display order, then the new overlay
            // will be inserted before next_lowest in m_overlay_list,
            // which is AFTER next_lowest in the display order.  Thus
            // it will be ABOVE next_lowest in the display.
            if (OVL_get_type_descriptor_list()->
               GetDisplayOrder(next_lowest->get_m_overlayDescGuid()) <= displayOrder)
               not_found = FALSE;
         }
         else
            not_found = FALSE;
      }
   }

   // Search for the open overlay that the newly open
   // overlay belongs above.
   while (position && not_found)
   {
      // save position of the overlay you are about to get, in
      // case it is the overlay you are looking for
      insert_before = position;

      // get the next overlay moving from the head (top) toward
      // the tail (bottom)
      //next_lowest = GetOverlayList(pOverlay).GetNext(position);
      next_lowest = m_overlay_list.GetNext(position);

      // if next_lowest comes before the new overlay according
      // to the default display order, then the new overlay
      // will be inserted before next_lowest in m_overlay_list,
      // which is AFTER next_lowest in the display order.  Thus
      // it will be ABOVE next_lowest in the display.
      if (OVL_get_type_descriptor_list()->
         GetDisplayOrder(next_lowest->get_m_overlayDescGuid()) <= displayOrder)
         not_found = FALSE;
   }

   // if this overlay belongs above no open overlay, then it
   // belongs below all open overlays - note if the list is
   // empty the head is the tail and this works fine
   if (not_found)
      m_overlay_list.AddTail(pOverlay);
   else
      m_overlay_list.InsertBefore(insert_before, pOverlay);

   return SUCCESS;
}

void C_ovl_mgr::OnOverlayAdded(C_overlay *overlay, boolean_t invalidate)
{
   if (overlay->is_tabular_editable()) 
      if(g_tabular_editor_dlg)
         g_tabular_editor_dlg->update_overlay_tabs(CTabularEditorDlg::TAB_UPDATE_ADD, GetOverlayDisplayName(overlay), overlay);

   // Notify any observers that an overlay has been added to the stack
   {
      fv_common::ScopedCriticalSection lock(
         &m_overlay_stack_changed_observers_cs);
      for (auto it=std::begin(m_overlay_stack_changed_observers);
         it != std::end(m_overlay_stack_changed_observers); ++it)
      {
         try
         {
            (*it)->OverlayAdded(overlay);
         }
         catch(_com_error& e)
         {
            // report error and contnue
            REPORT_COM_ERROR(e);
         }
      }
   }

   // if this overlay was just added to the head, then it is
   // the current overlay
   if (!IsTopMostOverlay(overlay) && overlay == m_overlay_list.GetHead())
   {
      // Only need to call change_current (for redraw) if we are not going to 
      // invalidate all the overlays.
      if ( invalidate )
      {
         make_current(overlay);
         m_current = overlay;
      }
      else
      {
         change_current( overlay );
      }
   }

   // if this is a time-sensitive overlay
   PlaybackTimeSegment_Interface *pFvTimeSegment = dynamic_cast<PlaybackTimeSegment_Interface *>(overlay);
   if (pFvTimeSegment != NULL && pFvTimeSegment->SupportsFvPlaybackTimeSegment())
   {
      // the gant chart in the view time dialog needs to be updated to 
      // reflect the addition of an overlay
      CMainFrame::GetPlaybackDialog().update_gant_chart();
   }

   // if the mission binder dialog is opened, then the overlay list
   // need to be updated
   if (CMainFrame::m_mission_binder_dlg != NULL)
      CMainFrame::m_mission_binder_dlg->update_overlay_list();
   
   const int overlay_handle = get_overlay_handle(overlay);
   notify_clients_of_open(overlay_handle);

   // draw all overlays over fresh map upon request
   if (invalidate)
      invalidate_view(FALSE);

   // update the title bar as necessary
   if (m_current == overlay)
      FVW_update_title_bar();

   FVW_refresh_view_map_state();
}

int C_ovl_mgr::RemoveOverlayFromStack(C_overlay *overlay)
{
   POSITION position;

   // if list is empty then fail
   if (m_overlay_list.IsEmpty())
      return FAILURE;

   // find overlay in the list
   position = m_overlay_list.Find(overlay);

   // if not found, fail
   if (position == NULL)
      return FAILURE;

   // remove the overlay from the list and update m_current
   m_overlay_list.RemoveAt(position);

   return SUCCESS;
}

// -----------------------------------------------------------------------------

// add an overlay to the overlay list, place it in the appropariate update 
// location and update the display
int C_ovl_mgr::add_overlay(C_overlay *overlay, boolean_t current,
   boolean_t invalidate)
{
   // avoid NULL pointers
   if (overlay == NULL)
   {
      ASSERT(0);
      ERR_report("NULL overlay pointer.");
      return FAILURE;
   }

   // Special case is when add_overlay is called during startup... we ignore
   // the default load order and merely add the overlays in the order they
   // appear in the registry, i.e., the order they were in when the
   // configuration was saved.
   if (m_startup_load_active)
   {
      m_current = overlay;
      m_overlay_list.AddHead(overlay);

      // assign an overlay handle to the newly added overlay
      m_overlay_map.SetAt(m_next_overlay_handle++, overlay);

      return SUCCESS;
   }

   if (AddOverlayToStack(overlay, overlay->get_m_overlayDescGuid()) != SUCCESS)
   {
      ERR_report("C_ovl_mgr::add_overlay - unable to add overlay to stack of overlays");
      return FAILURE;
   }

   // assign an overlay handle to the overlay
   m_overlay_map.SetAt(m_next_overlay_handle++, overlay);

   OnOverlayAdded(overlay, invalidate);

   return SUCCESS;
}


// -----------------------------------------------------------------------------

// remove the overlay from the overlay list and
// delete it from the screen
int C_ovl_mgr::delete_overlay(C_overlay *overlay, boolean_t invalidate,
   boolean_t erase_background /*=FALSE*/)
{
   if (RemoveOverlayFromStack(overlay) != SUCCESS)
   {
      ERR_report("C_ovl_mgr::delete_overlay - unable to remove overlay from stack");
      return FAILURE;
   }

   // Notify any observers that an overlay has been removed from the stack
   {
      fv_common::ScopedCriticalSection lock(
         &m_overlay_stack_changed_observers_cs);
      for (auto it=std::begin(m_overlay_stack_changed_observers);
         it != std::end(m_overlay_stack_changed_observers); ++it)
      {
         try
         {
            (*it)->OverlayRemoved(overlay);
         }
         catch(_com_error& e)
         {
            // report error and contnue
            REPORT_COM_ERROR(e);
         }
      }
   }

   // remove the handle associated with this overlay from the overlay map
   m_overlay_map.RemoveKey(get_overlay_handle(overlay));

   if (m_current == overlay)
   {
      if (!m_overlay_list.IsEmpty())
      {
         m_current = (C_overlay *)m_overlay_list.GetHead();

         if (g_tabular_editor_dlg)
         {
            TabularEditorDataObject* pDataObject = m_current->GetTabularEditorDataObject();
            if (pDataObject != NULL)
            {
               g_tabular_editor_dlg->set_data_object(pDataObject, TRUE);
               g_tabular_editor_dlg->on_selection_changed();
            }
         }

         // maintain the same mode if an overlay of the corresponding type exists,
         // otherwise switch to EDITOR_MODE_NONE mode
         boolean_t change, match;
         make_current_overlay_match_mode(&change, &match);
         if (!match && m_pCurrentEditor != NULL)
            set_mode(GUID_NULL, FALSE);
      }
      else
      {
         m_current = NULL;
         set_mode(GUID_NULL, FALSE);
      }
   }

   // draw all overlays over fresh map upon request
   if (invalidate)
      invalidate_view(erase_background);

   // update the Gannt chart in the playback dialog since the overlay list has changed
   PlaybackTimeSegment_Interface *pFvTimeSegment = dynamic_cast<PlaybackTimeSegment_Interface *>(overlay);
   if (pFvTimeSegment != NULL && pFvTimeSegment->SupportsFvPlaybackTimeSegment())
   {
      CMainFrame::GetPlaybackDialog().update_gant_chart();
   }

   // if the mission binder dialog is opened, then the overlay list
   // need to be updated
   if (CMainFrame::m_mission_binder_dlg != NULL)
      CMainFrame::m_mission_binder_dlg->update_overlay_list();

   if (g_tabular_editor_dlg)
      g_tabular_editor_dlg->update_overlay_tabs(CTabularEditorDlg::TAB_UPDATE_DELETE, GetOverlayDisplayName(overlay), overlay);

   overlay->OuterFinalize();

   FVW_update_title_bar();

   return SUCCESS;
}

// -----------------------------------------------------------------------------

int C_ovl_mgr::change_current( C_overlay* overlay )
{
   C_overlay* old_current = m_current;

   // release the edit focus on the current overlay if it has the edit focus
   if (old_current != NULL && m_pCurrentEditor != NULL)
   {
      // if the mode for m_current does not match the current editor
      // mode, then bypass the call to release_edit_focus: this is being
      // done particularly for overlays like the Map Data Coverage that
      // has the overlay closed BEFORE the editor is turned off -- MAY also
      // occur when the current overlay for the current editor is closed.
      IFvOverlayEditor *pEditor = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(old_current->get_m_overlayDescGuid())->pOverlayEditor;
      if (pEditor == m_pCurrentEditor)
         old_current->release_edit_focus();
   }

   m_current = overlay;

   // If there was a previous overlay selected, and we are not already planning to redraw everything,
   // then at least redraw the old overlay so rubber band rectangles and such will go away.
   if ( old_current )
   {
      // draw from the old_current to the new current (top two
      // overlays now)
      draw_from_overlay(old_current);
   }

   if (g_tabular_editor_dlg)
   {
      g_tabular_editor_dlg->update_overlay_tabs(CTabularEditorDlg::TAB_UPDATE_CHANGE_CURRENT,
         GetOverlayDisplayName(overlay), overlay);

      TabularEditorDataObject* pDataObject = overlay->GetTabularEditorDataObject();
      if (pDataObject != NULL)
      {
         g_tabular_editor_dlg->set_data_object(pDataObject);
         g_tabular_editor_dlg->on_selection_changed();
      }
   }

   return SUCCESS;
}

// -----------------------------------------------------------------------------

// make this overlay current
int C_ovl_mgr::make_current(C_overlay *overlay, boolean_t invalidate)
{
   POSITION position;

   // make overlay current
   position = m_overlay_list.Find(overlay);

   // if overlay not found then fail
   if (position == NULL)
      return FAILURE;

   return make_current(overlay, position, invalidate);
}   


// -----------------------------------------------------------------------------

// make this overlay current
int C_ovl_mgr::make_current(CString overlay_name, boolean_t invalidate)
{
   POSITION position;
   C_overlay *overlay;

   // find the overlay with the name
   find_by_name(overlay_name, position);
   if (position == NULL)
      return FAILURE;

   // get the overlay
   overlay = (C_overlay *)m_overlay_list.GetAt(position);

   return make_current(overlay, position, invalidate);
}


// -----------------------------------------------------------------------------

// make this overlay current
int C_ovl_mgr::make_current(C_overlay *overlay, POSITION position, 
   boolean_t invalidate)
{
   C_overlay* old_current;

   // the overlay is already current
   if (m_current == overlay)
      return SUCCESS;

   // save the previous current so that we can redraw it
   // (in case it changes foreground/background display state
   // like routes)
   old_current = m_current;

   // finally, update the title bar to reflect changes
   FVW_update_title_bar();

   if (g_tabular_editor_dlg)
   {
      g_tabular_editor_dlg->update_overlay_tabs(CTabularEditorDlg::TAB_UPDATE_CHANGE_CURRENT,
         GetOverlayDisplayName(overlay), overlay);

      TabularEditorDataObject* pDataObject = overlay->GetTabularEditorDataObject();
      if (pDataObject != NULL)
      {
         g_tabular_editor_dlg->set_data_object(pDataObject, TRUE);
         g_tabular_editor_dlg->on_selection_changed();
      }
   }

   // the list must have at least two elements
   if (m_overlay_list.GetCount() < 2)
      return SUCCESS;

   // move this overlay to the head of dynamic list   
   m_overlay_list.RemoveAt(position);

   // compute top position and insert before
   POSITION top_position = GetTopOverlayPosition(overlay);
   
   m_overlay_list.InsertBefore(top_position, overlay);

   // mark it as the current overlay
   m_current = overlay;

   // make sure the correct editor has been entered now that the current 
   // overlay has changed (if we are in some edit mode)
   if (m_pCurrentEditor != NULL)
      make_mode_match_current_overlay();

   // draw the overlay that has been made the top
   if (invalidate)
   {
      // draw from the old_current to the new current (top two
      // overlays now)
      draw_from_overlay(old_current);
   }

   // Notify any observers that the overlay list has been reordered
   {
      fv_common::ScopedCriticalSection lock(
         &m_overlay_stack_changed_observers_cs);
      for (auto it=std::begin(m_overlay_stack_changed_observers);
         it != std::end(m_overlay_stack_changed_observers); ++it)
      {
         try
         {
            (*it)->OverlayOrderChanged();
         }
         catch(_com_error& e)
         {
            // report error and contnue
            REPORT_COM_ERROR(e);
         }
      }
   }

   return SUCCESS;
}


// -----------------------------------------------------------------------------

// make this overlay the bottom overlay
int C_ovl_mgr::move_to_bottom(C_overlay *overlay, boolean_t invalidate)
{    
   POSITION position;

   // get the position of the overlay
   position = m_overlay_list.Find(overlay);

   // if overlay not found then fail
   if (position == NULL)
      return FAILURE;

   return move_to_bottom(overlay, position, invalidate);
}


// -----------------------------------------------------------------------------

// make this overlay the bottom overlay
int C_ovl_mgr::move_to_bottom(CString overlay_name, boolean_t invalidate)
{
   POSITION position;
   C_overlay *overlay;

   // find the overlay with the name
   find_by_name(overlay_name, position);

   // if overlay not found then fail
   if (position == NULL)
      return FAILURE;

   // remove this overlay from the list
   overlay = (C_overlay *)m_overlay_list.GetAt(position);   

   return move_to_bottom(overlay, position, invalidate);
}


// -----------------------------------------------------------------------------

// make this overlay the bottom overlay
int C_ovl_mgr::move_to_bottom(C_overlay *overlay, POSITION position, 
   boolean_t invalidate)
{
   // the list must have at least two elements
   if (m_overlay_list.GetCount() < 2)
      return SUCCESS;

   // move the overlay to the tail of the list
   m_overlay_list.RemoveAt(position);

   POSITION bottom_position = GetBottomOverlayPosition(overlay);
   m_overlay_list.InsertAfter(bottom_position, overlay);

   OnOverlayOrderChanged(invalidate);
   
   return SUCCESS;
}

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
int C_ovl_mgr::select(FalconViewOverlayLib::IFvMapView *pMapView, CPoint point, UINT flags, boolean_t *drag)
{
   POSITION next;
   C_overlay *overlay;

   // no overlays, so nothing can be selected
   if (m_overlay_list.IsEmpty())
      return FAILURE;

   // test for re-entry
   if (active) 
   {
      return FAILURE;
   }

   try
   {
      // set active flag to guard against re-entry
      active = TRUE;

      // if we are showing all overlays, run selected on all overlays
      if (m_show_non_current_ovls_flag)
      {

         // first see if any overlay have selection locked, if so it gets first shot at
         // handling selection
         next = m_overlay_list.GetHeadPosition();   // point to first overlay in list
         do
         {
            // get this overlay and the position of the next one
            overlay = (C_overlay *)m_overlay_list.GetNext(next);

            // skip this overlay if it is hidden
            if (!overlay->get_m_bIsOverlayVisible())
               continue;

            // skip if ui events is not implemented
            OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(overlay);
            if (pUIEvents == NULL)
               continue;

            OverlayUIEventRoutingOverrides_Interface *pUIEventRouting = dynamic_cast<OverlayUIEventRoutingOverrides_Interface *>(overlay);
            if (pUIEventRouting == NULL)
               continue;

            // see if the overlay has selection locked
            if( pUIEventRouting->get_m_bDirectlyRouteMouseLeftButtonDown() )
            {
               // Call the selected() member of this overlay, it will return SUCCESS
               // if it has been "selected".  It will set drag == TRUE if it wants
               // to be "dragged"
               long bHandled = FALSE;
               pUIEvents->MouseLeftButtonDown(pMapView, point.x, point.y, flags, &bHandled);
               if (bHandled)
               {
                  active = FALSE;      // clear active flag
                  return SUCCESS;
               } 
            }
         } while (next);

         // get the position of the top most overlay
         next = m_overlay_list.GetHeadPosition();
         do
         {
            // get this overlay and the position of the next one
            overlay = (C_overlay *)m_overlay_list.GetNext(next);

            // skip this overlay if it is hidden
            if (!overlay->get_m_bIsOverlayVisible())
               continue;

            // skip if ui events is not implemented
            OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(overlay);
            if (pUIEvents == NULL)
               continue;

            // Call the selected() member of this overlay, it will return SUCCESS
            // if it has been "selected".  It will set drag == TRUE if it wants
            // to be "dragged".
            long bHandled = FALSE;
            pUIEvents->MouseLeftButtonDown(pMapView, point.x, point.y, flags, &bHandled);
            if (bHandled)
            {
               *drag = FALSE; // reset drag flag which is about to be removed from selected()
               active = FALSE;      // clear active flag

               return SUCCESS;
            } 

         } while (next);
      }
      else
      {
         // otherwise, only test the current overlay

         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(m_current);
         if (pUIEvents != NULL)
         {
            long bHandled = FALSE;
            pUIEvents->MouseLeftButtonDown(pMapView, point.x, point.y, flags, &bHandled);

            if (bHandled)
            {
               *drag = FALSE; // reset drag flag which is about to be removed from selected()
               active = FALSE;      // clear active flag

               return SUCCESS;
            }
         }
      }

      active = FALSE;            // clear active flag

      return FAILURE;
   }
   catch(...)
   {
      ERR_report("Exception"); //to do error report
      active = FALSE;            // clear active flag
      throw;
   }

   ASSERT(0);  //should never get here
   active = FALSE;            // clear active flag
   return FAILURE;
}

int C_ovl_mgr::on_left_mouse_button_up(FalconViewOverlayLib::IFvMapView *pMapView, CPoint point, UINT flags)
{
   POSITION next;
   C_overlay *overlay;

   // no overlays, so nothing can be selected
   if (m_overlay_list.IsEmpty())
      return FAILURE;

   // if we are showing all overlays, run selected on all overlays
   if (m_show_non_current_ovls_flag)
   {
      // first see if any overlay have selection locked, if so it gets first shot at
      // handling selection
      next = m_overlay_list.GetHeadPosition();   // point to first overlay in list
      do
      {
         // get this overlay and the position of the next one
         overlay = (C_overlay *)m_overlay_list.GetNext(next);

         // skip this overlay if it is hidden
         if (!overlay->get_m_bIsOverlayVisible())
            continue;

         // skip if ui events is not implemented
         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(overlay);
         if (pUIEvents == NULL)
            continue;

         OverlayUIEventRoutingOverrides_Interface *pUIEventRouting = dynamic_cast<OverlayUIEventRoutingOverrides_Interface *>(overlay);
         if (pUIEventRouting == NULL)
            continue;

         // see if the overlay has selection locked
         if( pUIEventRouting->get_m_bDirectlyRouteMouseLeftButtonUp() )
         {
            // Call the selected() member of this overlay, it will return SUCCESS
            // if it has been "selected".  It will set drag == TRUE if it wants
            // to be "dragged".
            long bHandled = FALSE;
            pUIEvents->MouseLeftButtonUp(pMapView, point.x, point.y, flags, &bHandled);
            if (bHandled)
            {
               return SUCCESS;
            } 
         }
      } while (next);

      // get the position of the top most overlay
      next = m_overlay_list.GetHeadPosition();
      do
      {
         // get this overlay and the position of the next one
         overlay = (C_overlay *)m_overlay_list.GetNext(next);

         // skip this overlay if it is hidden
         if (!overlay->get_m_bIsOverlayVisible())
            continue;

         // skip if ui events is not implemented
         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(overlay);
         if (pUIEvents == NULL)
            continue;

         // Call the selected() member of this overlay, it will return SUCCESS
         // if it has been "selected".  It will set drag == TRUE if it wants
         // to be "dragged".
         long bHandled = FALSE;
         pUIEvents->MouseLeftButtonUp(pMapView, point.x, point.y, flags, &bHandled);
         if (bHandled)
         {
            return SUCCESS;
         } 

      } while (next);
   }
   else
   {
      // otherwise, only test the current overlay
      OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(m_current);
      if (pUIEvents != NULL)
      {
         long bHandled = FALSE;
         pUIEvents->MouseLeftButtonUp(pMapView, point.x, point.y, flags, &bHandled);
         if (bHandled)
         {
            return SUCCESS;
         }
      }
   }

   return FAILURE;
}

int C_ovl_mgr::on_right_mouse_button_down(FalconViewOverlayLib::IFvMapView *pMapView, CPoint point, UINT flags)
{
   POSITION next;
   C_overlay *overlay;

   // no overlays, so nothing can be selected
   if (m_overlay_list.IsEmpty())
      return FAILURE;

   // if we are showing all overlays, run selected on all overlays
   if (m_show_non_current_ovls_flag)
   {
      // get the position of the top most overlay
      next = m_overlay_list.GetHeadPosition();
      do
      {
         // get this overlay and the position of the next one
         overlay = (C_overlay *)m_overlay_list.GetNext(next);

         // skip this overlay if it is hidden
         if (!overlay->get_m_bIsOverlayVisible())
            continue;

         // skip if ui events is not implemented
         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(overlay);
         if (pUIEvents == NULL)
            continue;

         // Call the selected() member of this overlay, it will return SUCCESS
         // if it has been "selected".  It will set drag == TRUE if it wants
         // to be "dragged".
         long bHandled = FALSE;
         pUIEvents->MouseRightButtonDown(pMapView, point.x, point.y, flags, &bHandled);
         if (bHandled)
         {
            return SUCCESS;
         } 

      } while (next);
   }
   else
   {
      // otherwise, only test the current overlay
      OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(m_current);
      if (pUIEvents != NULL)
      {
         long bHandled = FALSE;
         pUIEvents->MouseRightButtonDown(pMapView, point.x, point.y, flags, &bHandled);
         if (bHandled)
         {
            return SUCCESS;
         }
      }
   }

   return FAILURE;
}

int C_ovl_mgr::select_vertical(CVerticalViewProjector* pProj, CPoint point, UINT flags, HCURSOR *pCursor, HintText **ppHint)
{
   // initialize to do nothing by default
   TipTimer::set_text("");
   *pCursor = NULL;
   *ppHint = NULL;

   if (m_show_non_current_ovls_flag)
   {
      POSITION pos = m_overlay_list.GetHeadPosition();
      while (pos)
      {
         // get this overlay and the position of the next one
         C_overlay *pOverlay = m_overlay_list.GetNext(pos);

         // skip this overlay if it is hidden
         if (!pOverlay->get_m_bIsOverlayVisible())
            continue;

         // Call the selected() member of this overlay, it will return SUCCESS
         // if it has been "selected".  It will set drag == TRUE if it wants
         // to be "dragged".
         if (pOverlay->selected_vertical(pProj, point, flags, pCursor, ppHint) == SUCCESS)
         {
            if (*ppHint != NULL)
               TipTimer::set_text((*ppHint)->get_tool_tip());

            return SUCCESS;
         }
      }
   }
   else if (m_current->selected_vertical(pProj, point, flags, pCursor, ppHint) == SUCCESS)
   {
      if (*ppHint != NULL)
         TipTimer::set_text((*ppHint)->get_tool_tip());

      return SUCCESS;
   }

   return FAILURE;
}


// -----------------------------------------------------------------------------

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
//
// Returns SUCCESS if any overlay handles the select.  FAILURE otherwise
int C_ovl_mgr::test_select(FalconViewOverlayLib::IFvMapView *pMapView, CPoint point, UINT flags, HintText **hint)
{
   POSITION next;
   C_overlay *overlay;

   // initialize to do nothing by default
   TipTimer::set_text("");
   *hint = NULL;

   // no overlays, so nothing can be selected
   if (m_overlay_list.IsEmpty())
      return FAILURE;

   // test for re-entry
   if (active) 
      return FAILURE;

   // set active flag to guard against re-entry
   active = TRUE;

   CString last_hint;

   try
   {
      // if all overlays are showing, test all
      if (all_overlays_visible())
      {

         // first see if any overlay have selection locked, if so it gets first shot at
         // handling selection
         next = m_overlay_list.GetHeadPosition();   // point to first overlay in list
         do
         {
            // get this overlay and the position of the next one
            overlay = (C_overlay *)m_overlay_list.GetNext(next);

            // skip this overlay if it is hidden
            if (!overlay->get_m_bIsOverlayVisible())
               continue;

            // skip if ui events is not implemented
            OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(overlay);
            if (pUIEvents == NULL)
               continue;

            OverlayUIEventRoutingOverrides_Interface *pUIEventRouting = dynamic_cast<OverlayUIEventRoutingOverrides_Interface *>(overlay);
            if (pUIEventRouting == NULL)
               continue;

            // see if the overlay has selection locked
            if( pUIEventRouting->get_m_bDirectlyRouteMouseMove() )
            {
               // Call the on_mouse_moved() member of this overlay, it will return
               // SUCCESS if its selected() member would have. 
               long bHandled = FALSE;
               pUIEvents->MouseMove(pMapView, point.x, point.y, flags, &bHandled);
               if (bHandled == TRUE)
               {
                  active = FALSE;      // clear active flag
                  return SUCCESS;
               }
            }
         } while (next);

         // get the position of the top most overlay
         next = m_overlay_list.GetHeadPosition();
         do
         {
            // get this overlay and the position of the next one
            overlay = (C_overlay *)m_overlay_list.GetNext(next);

            // skip this overlay if it is hidden
            if (!overlay->get_m_bIsOverlayVisible())
               continue;

            // skip if ui events is not implemented
            OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(overlay);
            if (pUIEvents == NULL)
               continue;

            // Call the on_mouse_moved() member of this overlay, it will return
            // SUCCESS if its selected() member would have.
            long bHandled = FALSE;
            pUIEvents->MouseMove(pMapView, point.x, point.y, flags, &bHandled);
            if (bHandled == TRUE)
            {
               active = FALSE;      // clear active flag
               return SUCCESS;
            }
            // An overlay can choose to return FAILURE (to allow other overlays to process the event) but still set the status
            // bar text.  In this case, the text is used only in the case that no overlay handles the on_mouse_moved event (all of them
            // return FAILURE).  The topmost overlay that sets the status bar text is used, hence the check for last_hint.
            else if (fvw_get_frame()->m_statusBarMgr->m_currentHelpText.GetLength() > 0 && last_hint.GetLength() == 0)
            {
               last_hint = fvw_get_frame()->m_statusBarMgr->m_currentHelpText;
            }

         } while (next);

         // Handling the case where all overlays returned FAILURE but some overlay still set the status bar text (used as the default hint)
         if (last_hint.GetLength() > 0)
         {
            fvw_get_frame()->m_statusBarMgr->SetStatusBarHelpText(_bstr_t(last_hint));

            active = FALSE;         // clear active flag 
            return SUCCESS;
         }
      }
      // otherwise only test the current overlay
      else 
      {
         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(m_current);
         if (pUIEvents != NULL)
         {
            long bHandled = FALSE;
            pUIEvents->MouseMove(pMapView, point.x, point.y, flags, &bHandled);
            if (bHandled == TRUE)
            {
               active = FALSE;      // clear active flag
               return SUCCESS;
            }
         }
      }
   }
   catch(...)
   {
      ERR_report("exception");
      active = FALSE;         // clear active flag   
      throw;
   }

   active = FALSE;            // clear active flag

   return FAILURE;
}

int C_ovl_mgr::test_select_vertical(CVerticalViewProjector* pProj, CPoint point, UINT flags, HCURSOR *pCursor, HintText **ppHint)
{
   TipTimer::set_text("");

   if (all_overlays_visible())
   {
      // allow overlays to hit test from top most overlay down
      POSITION position = m_overlay_list.GetHeadPosition();
      while (position)
      {
         C_overlay *pOverlay = m_overlay_list.GetNext(position);
         if (pOverlay != NULL)
         {
            if (pOverlay->test_selected_vertical(pProj, point, flags, pCursor, ppHint) == SUCCESS)
            {
               if (*ppHint != NULL)
                  TipTimer::set_text((*ppHint)->get_tool_tip());
               return SUCCESS;
            }
         }
      }
   }
   else if (m_current != NULL)
   {
      if (m_current->test_selected_vertical(pProj, point, flags, pCursor, ppHint) == SUCCESS)
      {
         if (*ppHint != NULL)
            TipTimer::set_text((*ppHint)->get_tool_tip());
         return SUCCESS;
      }
   }

   return FAILURE;
}

// This function will return in TRUE if the current editor mode requires that
// the user's ability to rotate the map be disabled.  This function will return
// TRUE if any open overlay requires that the ability of the user to rotate the
// map be disabled.  Otherwise it will return FALSE.
boolean_t C_ovl_mgr::disable_rotation_ui(void)
{
   // if an editor is active, and it implements the IFvOverlayLimitUserInterface interface, let it disable rotation
   IFvOverlayLimitUserInterface* pLimitUI = dynamic_cast<IFvOverlayLimitUserInterface *>(m_pCurrentEditor);
   if (pLimitUI != NULL)
   {
      long bDisable;
      pLimitUI->get_m_bDisableRotationUI(&bDisable);
      return bDisable;
   }

   return FALSE;
}

// This function will return in TRUE if the current editor mode requires that
// the user's ability to change the projection be disabled.  This function will return
// TRUE if any open overlay requires that the ability of the user to change the 
// projection be disabled.  Otherwise it will return FALSE.
boolean_t C_ovl_mgr::disable_projection_ui(void)
{
   // if an editor is active and implements the IFvOverlayLimitUserInterface, let it disable projection
   IFvOverlayLimitUserInterface* pLimitUI = dynamic_cast<IFvOverlayLimitUserInterface *>(m_pCurrentEditor);
   if (pLimitUI != NULL)
   {
      long bDisable;
      pLimitUI->get_m_bDisableProjectionUI(&bDisable);
      return bDisable;
   }

   return FALSE;
}

// This function will return in TRUE if the current editor mode requires 
// that the map be North-Up.  This function will return TRUE if any open
// overlay requires that the map be North-Up.  Otherwise it will return 
// FALSE.
boolean_t C_ovl_mgr::requires_north_up(void)
{
   // if an editor is active, and it implements the IFvOverlayLimitUserInterface interface, let it require North-Up
   IFvOverlayLimitUserInterface* pLimitUI = dynamic_cast<IFvOverlayLimitUserInterface *>(m_pCurrentEditor);
   if (pLimitUI != NULL)
   {
      long bRequiresNorthUp;
      pLimitUI->get_m_bRequiresNorthUp(&bRequiresNorthUp);
      return bRequiresNorthUp;
   }

   return FALSE;
}

// Returns TRUE if the map must be projected with the EqualArc projection
// when this editor is active. This function will return TRUE if any open
// overlay requires that the map be projected with the EqualArc projection.  
// Otherwise it will return FALSE.  
boolean_t C_ovl_mgr::requires_equal_arc_projection(void)
{
   // if an editor is active, and it implements the IFvOverlayLimitUserInterface interface, let it require EqualArc projection
   IFvOverlayLimitUserInterface* pLimitUI = dynamic_cast<IFvOverlayLimitUserInterface *>(m_pCurrentEditor);
   if (pLimitUI != NULL)
   {
      long bRequiresEqualArc;
      pLimitUI->get_m_bRequiresEqualArc(&bRequiresEqualArc);
      return bRequiresEqualArc;
   }

   return FALSE;
}

// add menu items to the menu based on point 
void C_ovl_mgr::menu(FalconViewOverlayLib::IFvMapView* map_view,
   const CPoint& point, FalconViewOverlayLib::IFvContextMenu *context_menu)
{ 
   OverlayEventRouter router(CListToVector<>(m_overlay_list), 
      !all_overlays_visible(), m_current);

   router.AppendMenuItems(context_menu, map_view, point);
}

// -----------------------------------------------------------------------------

// Test to see if any overlay can do a single point snap to at this point.
boolean_t C_ovl_mgr::test_snap_to(ViewMapProj* map, CPoint point)
{
   OverlayEventRouter router(CListToVector<>(m_overlay_list), 
      !all_overlays_visible(), m_current);

   return router.CanSnapTo(map, point) ? TRUE : FALSE;
}

// -----------------------------------------------------------------------------

// Returns TRUE when a snap to object for this point is selected, otherwise
// it returns FALSE.  If multiple items are under the point the user can
// select one from a list, or they can select cancel.  If the user selects
// cancel this function returns FALSE.
boolean_t C_ovl_mgr::do_snap_to(ViewMapProj* map, CPoint point, SnapToInfo *info)
{
   POSITION position;
   C_overlay *overlay;
   SnapToInfo *snap_to_info;
   CList<SnapToInfo *, SnapToInfo *> snap_to_list;
   int count;

   // this means no current overlay and no overlays
   if (m_current == NULL)
      return FALSE;

   // if we are showing all overlays, then do_snap_to on all overlays
   if (m_show_non_current_ovls_flag)
   {
      // search from the top down
      position = m_overlay_list.GetHeadPosition();
      do
      {
         // get this overlay and the position of the next one
         overlay = (C_overlay *)m_overlay_list.GetNext(position);

         // skip this overlay if it is hidden
         if (!overlay->get_m_bIsOverlayVisible())
            continue;

         // allow this overlay to add its snap to elements to the list, if it 
         // has any at the given point
         overlay->do_snap_to(map, point, snap_to_list);
      } while (position);
   }
   else
      // otherwise, do_snap_to only on current overlay
      m_current->do_snap_to(map, point, snap_to_list);

   // if there are any snap to objects at the point
   count = snap_to_list.GetCount();
   if (count)
   {
      boolean_t cancel;

      // if there is more than 1, then the user is given a choice
      if (count > 1)
      {
         SnapToDlg dlg(fvw_get_frame(), snap_to_list);

         if (dlg.DoModal() == IDOK)
            snap_to_info = dlg.get_selection();
         else
            snap_to_info = NULL;
      }
      else
         snap_to_info = snap_to_list.GetHead();

      // the item to snap to
      if (snap_to_info)
      {
         *info = *snap_to_info;
         cancel = FALSE;

         // if elevation was not filled in by the snap to, try DTED
         if (info->m_elv_src == SnapToInfo::ELV_UNKNOWN)
         {
            int elevation;

            // if DTED is available for the point, fill in the elevation
            if (DTD_get_elevation_in_feet(info->m_lat, info->m_lon, 
               &elevation) == SUCCESS)
            {
               info->m_elv = (short)elevation;
               info->m_elv_src = SnapToInfo::DTED;
            }
         }
      }
      else
         cancel = TRUE;

      // delete all snap to objects in the list
      do
      {
         snap_to_info = snap_to_list.RemoveHead();
         delete snap_to_info;
      } while (snap_to_list.GetCount());

      return (!cancel);
   }

   return FALSE;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Returns TRUE if there is a current overlay otherwise
// it returns FALSE.  All snap_to items are put in the list

boolean_t C_ovl_mgr::do_snap_to(ViewMapProj* map, CPoint point, CList<SnapToInfo *, SnapToInfo *> &snap_to_list)
{
   POSITION position;
   C_overlay *overlay;

   // this means no current overlay and no overlays
   if (m_current == NULL)
      return FALSE;

   // if we are showing all overlays, then do_snap_to on all overlays
   if (m_show_non_current_ovls_flag)
   {
      // search from the top down
      position = m_overlay_list.GetHeadPosition();
      do
      {
         // get this overlay and the position of the next one
         overlay = (C_overlay *)m_overlay_list.GetNext(position);

         // skip this overlay if it is hidden
         if (!overlay->get_m_bIsOverlayVisible())
            continue;

         // allow this overlay to add its snap to elements to the list, if it 
         // has any at the given point
         overlay->do_snap_to(map, point, snap_to_list);
      } while (position);
   }
   else
      // otherwise, do_snap_to only on current overlay
      m_current->do_snap_to(map, point, snap_to_list);


   return TRUE;
}

C_overlay* C_ovl_mgr::get_first_of_type(GUID overlayDescGuid) const
{
   POSITION next = m_overlay_list.GetHeadPosition();
   while (next)
   {
      C_overlay* overlay = m_overlay_list.GetNext(next);
      if (overlay->get_m_overlayDescGuid() == overlayDescGuid)
         return overlay;
   }

   return NULL;
}

C_overlay* C_ovl_mgr::get_next_of_type(const C_overlay* ovl, GUID overlayDescGuid) const
{
   POSITION next;
   C_overlay *overlay;
   BOOL found;

   // get all overlays
   if (m_overlay_list.IsEmpty())
      return NULL;

   found = FALSE;
   next = m_overlay_list.GetHeadPosition();
   do
   {
      overlay = m_overlay_list.GetNext(next);
      if (overlay->get_m_overlayDescGuid() == overlayDescGuid)
      {
         if (found)
            return overlay;
         else
            if (overlay == ovl)
               found = TRUE;
      }
   } while (next != NULL);

   return NULL;
}

C_overlay* C_ovl_mgr::get_prev_of_type(C_overlay* ovl, GUID overlayDescGuid) const
{
   POSITION position = m_overlay_list.Find(ovl);
   m_overlay_list.GetPrev(position);
   while (position)
   {
      C_overlay* overlay = m_overlay_list.GetPrev(position);
      if (overlay->get_m_overlayDescGuid() == overlayDescGuid)
         return overlay;
   }

   return nullptr;
}

// -----------------------------------------------------------------------------
C_overlay* C_ovl_mgr::get_file_overlay(GUID overlayDescGuid, const char *filespec) 
{
   ASSERT(OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid)); // overlay type must be a file overlay

   //find a pointer to the overlay we want
   CString current_spec;
   C_overlay *overlay = get_first_of_type(overlayDescGuid);
   while (overlay)
   {
      //Note: we assume that class_name is a CFVFileOverlay (else assert above)
      OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

         current_spec = (char *)fileSpecification;
         if (current_spec.CompareNoCase(filespec)==0)
            break;   //we've found the requested overlay
      }

      overlay = get_next_of_type(overlay, overlayDescGuid);
   }

   //return the requeted overlay or NULL if we got to the end of the list w/o success
   return overlay;  
}

C_overlay * C_ovl_mgr::get_first() const
{
   POSITION first;
   C_overlay *overlay;

   // get all overlays
   if (m_overlay_list.IsEmpty())
      return NULL;

   first = m_overlay_list.GetHeadPosition();
   if (first != NULL)
   {
      overlay = m_overlay_list.GetNext(first);
      return overlay;
   }

   return NULL;
}

C_overlay * C_ovl_mgr::get_next(const C_overlay * ovl) const
{
   POSITION next;
   C_overlay *overlay;
   BOOL found;

   // get all overlays
   if (m_overlay_list.IsEmpty())
      return NULL;

   found = FALSE;
   next = m_overlay_list.GetHeadPosition();
   do
   {
      overlay = m_overlay_list.GetNext(next);
      if (found)
         return overlay;
      else
         if (overlay == ovl)
            found = TRUE;
   } while (next != NULL);

   return NULL;
}

C_overlay * C_ovl_mgr::get_last() const
{
   if (m_overlay_list.IsEmpty())
      return nullptr;

   return m_overlay_list.GetTail();
}

C_overlay * C_ovl_mgr::get_prev(const C_overlay * ovl) const
{
   POSITION prev;
   C_overlay *overlay;
   BOOL found;

   // get all overlays
   if (m_overlay_list.IsEmpty())
      return NULL;

   found = FALSE;
   prev = m_overlay_list.GetTailPosition();
   do
   {
      overlay = m_overlay_list.GetPrev(prev);
      if (found)
         return overlay;
      else
         if (overlay == ovl)
            found = TRUE;
   } while (prev != NULL);

   return NULL;
}


// -----------------------------------------------------------------------------

// Search for overlay with given name return position == NULL if not
// found. If found position is the position in m_overlay_list.
void C_ovl_mgr::find_by_name(CString name, POSITION &position)
{
   POSITION next;
   C_overlay *overlay;

   // search the overlay list by name
   if (!m_overlay_list.IsEmpty())
   {
      next = m_overlay_list.GetHeadPosition();
      do
      {
         // get this overlay and the position of the next one
         position = next;
         overlay = (C_overlay *)m_overlay_list.GetNext(next);
#if 1
         CString cs = GetOverlayDisplayName(overlay);
         ATLTRACE( _T("Looking for %s, found %s in %p\n"), name, cs, overlay );
         if ( cs == name )
#else

         if (GetOverlayDisplayName(overlay) == name)
#endif
            return;
      } while (next != NULL);
   }

   position = NULL;
}


// -----------------------------------------------------------------------------

// Search for overlay with given specification, return position == NULL if not
// found. If found position is the position in m_overlay_list.  Note: this
// function only works for overlays that implement IFvOverlayPersistence.
void C_ovl_mgr::find_by_specification(CString specification, GUID overlayDescGuid,
   POSITION &position)
{
   POSITION next;
   C_overlay *overlay;

   // search the overlay list by name
   if (!m_overlay_list.IsEmpty())
   {
      next = m_overlay_list.GetHeadPosition();
      do
      {
         // get this overlay and the position of the next one
         position = next;
         overlay = (C_overlay *)m_overlay_list.GetNext(next);

         OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
         if (pFvOverlayPersistence != NULL)
         {
            _bstr_t fileSpecification;
            pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

            CString currentFileSpec = (char *)fileSpecification;
            if (currentFileSpec.CompareNoCase(specification) == 0 && overlay->get_m_overlayDescGuid() == overlayDescGuid)
               return;
         }
      } while (next != NULL);
   }

   position = NULL;
}


// -----------------------------------------------------------------------------

// Get the position of the first dirty overlay, NULL for NONE.
// dirty_overlay_count is set to the number of dirty overlays.
POSITION C_ovl_mgr::get_dirty_overlay_position(int &dirty_overlay_count)
{
   POSITION prev;
   POSITION position;
   POSITION first_dirty_overlay_position = NULL;
   C_overlay *overlay;

   dirty_overlay_count = 0;

   if (m_overlay_list.IsEmpty())
      return NULL;

   prev = m_overlay_list.GetTailPosition();
   do
   {
      // save this position, in case it is the one you want
      position = prev;

      // get this overlay and the position of the next one
      overlay = (C_overlay *)m_overlay_list.GetPrev(prev);

      // if the overlay is dirty increment counter and save its position
      if (overlay->is_modified())
      {
         first_dirty_overlay_position = position;
         dirty_overlay_count++;
      }
   } while (prev != NULL);

   return first_dirty_overlay_position;
}


// -----------------------------------------------------------------------------

// Returns TRUE if the current overlay was changed by this function.
boolean_t C_ovl_mgr::make_current_overlay_match_mode()
{
   boolean_t change;
   boolean_t match;

   make_current_overlay_match_mode(&change, &match);
   return change;
}


// -----------------------------------------------------------------------------

void C_ovl_mgr::make_current_overlay_match_mode(boolean_t *current_changed,
   boolean_t *current_overlay_matches_mode)
{
   POSITION next;
   C_overlay *overlay;

   // initialize return values to FALSE by default
   *current_changed = FALSE;
   *current_overlay_matches_mode = FALSE;

   // no type corresponding to current mode -> NULL
   if (m_pCurrentEditor == NULL)
      return;

   // search from the top down for the first overlay of the corresponding
   // type that is editable (not read-only)
   boolean_t bFound = FALSE;
   next = m_overlay_list.GetHeadPosition();
   while (next && !bFound)
   {
      overlay = m_overlay_list.GetNext(next);
      IFvOverlayEditor *pEditor = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid())->pOverlayEditor;

      long lReadOnly = 0;
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
         pFvOverlayPersistence->get_m_bIsReadOnly(&lReadOnly);

      if (pEditor == m_pCurrentEditor && !lReadOnly && overlay->get_m_bIsOverlayVisible())
         bFound = TRUE;
   }

   // if matching overlay was found, make sure it is the current overlay
   if (bFound)
   {
      // if not current, make it current
      if (overlay != get_current_overlay())
      {
         make_current(overlay);
         *current_changed = TRUE;
      }

      *current_overlay_matches_mode = TRUE;
   }
}
// end of make_current_overlay_match_mode


// -----------------------------------------------------------------------------

// make the mode match the current overlay's mode
int C_ovl_mgr::make_mode_match_current_overlay()
{
   // if there is a current overlay then make the mode match it
   if (m_current)
      return make_mode_match_this_overlay(m_current);
   else
      return set_mode(GUID_NULL);
}
// end of make_mode_match_current_overlay


// make mode match that of this overlay
int C_ovl_mgr::make_mode_match_this_overlay(C_overlay *overlay)
{
   if (overlay == NULL)
   {
      ERR_report("NULL overlay pointer.");
      return FAILURE;
   }

   // get mode for this overlay's type - NULL for none
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
   if (pOverlayTypeDesc == NULL)
   {
      ERR_report("Unable to obtain overlay type descriptor for overlay");
      return FAILURE;
   }

   IFvOverlayEditor* pEditor = pOverlayTypeDesc->pOverlayEditor;

   // change to the appropriate mode if necessary
   if (pEditor != m_pCurrentEditor)
   {
      long lReadOnly = 0;
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
         pFvOverlayPersistence->get_m_bIsReadOnly(&lReadOnly);

      // don't switch to an editor mode if this overlay is read-only
      if (pEditor != NULL && lReadOnly)
      {
         if (m_pCurrentEditor == NULL)
            return SUCCESS;

         pEditor = NULL;
      }

      if (set_mode(pOverlayTypeDesc->overlayDescriptorGuid) != SUCCESS)
      {
         ERR_report("set_mode() failed.");
         return FAILURE;
      }
   }

   return SUCCESS;
}
// end of make_mode_match_this_overlay


class SetModeActive
{
public:
   static bool bSetModeActive;

   SetModeActive() { bSetModeActive = true; }
   ~SetModeActive() { bSetModeActive = false; }
};

bool SetModeActive::bSetModeActive = false;

CString C_ovl_mgr::get_filespec_for_overlay_type(OverlayTypeDescriptor *pOverlayTypeDesc)
{
   CString result;

   DWORD type;
   unsigned char buffer[MAX_PATH];
   DWORD buffer_size = MAX_PATH;

   CString regName = "Default " + pOverlayTypeDesc->displayName + " Database";

   if (PRM_read_registry(HKEY_LOCAL_MACHINE,"Software\\XPlan\\CONFIGURATION",
      regName, &type, (unsigned char*) &buffer, &buffer_size) == SUCCESS)
   {
      result = buffer;

      //check type to see that it was a string
      if (type != REG_SZ)
      {
         CString msg = pOverlayTypeDesc->displayName + " database registry error.";
         ERR_report(msg);
      }

      if (FIL_access((const char*)buffer, FIL_READ_OK) != SUCCESS)
      {
         CString msg = "Could not open default file " + result + ". Opening new file instead.";
         ERR_report(msg);
         result = "";
      }
   }
   else
      result = "";

   return result;
}

int C_ovl_mgr::set_mode(GUID overlayDescGuid, boolean_t release_focus)
{
   boolean_t change;
   boolean_t match;

   // set_mode should not be re-entrant
   if (SetModeActive::bSetModeActive)
      return SUCCESS;

   SetModeActive setModeActive;

   IFvOverlayEditor *pEditor = NULL;
   CString csTypeName;
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc != NULL)
   {
      pEditor = pOverlayTypeDesc->pOverlayEditor;
      csTypeName = pOverlayTypeDesc->displayName;
   }

   if (m_pCurrentEditor == pEditor)  // the edit mode has not changed
      return SUCCESS;

   // release the edit focus on the current overlay if it is being turned off
   if (release_focus && m_current != NULL && m_pCurrentEditor != NULL)
   {
      // if the mode for m_current does not match the current editor
      // mode, then bypass the call to release_edit_focus: this is being
      // done particularly for overlays like the Map Data Coverage in which
      // the overlay is closed BEFORE the editor is turned off -- MAY also
      // occur when the current overlay for the current editor is closed.
      IFvOverlayEditor *pedit = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(m_current->get_m_overlayDescGuid())->pOverlayEditor;
      if (pedit == m_pCurrentEditor)
         m_current->release_edit_focus();
   }

   // turn off current editor tool
   if (set_edit_on(FALSE) != SUCCESS)
   {
      ERR_report("set_edit_on(FALSE) failed.");
      return FAILURE;
   }

   // set the current edit mode equal to the new edit mode...
   m_pCurrentEditor = pEditor;
   SetCurrentEditorGuid(overlayDescGuid);
   m_csCurrentEditorTypeName = csTypeName;

   // turn on new editor tool.  to preserve legacy behavior we will call
   // set_edit_on for internal and legacy plug-ins _before_ creating a new 
   // overlay.  For newer plug-in overlays we will call set_edit_on
   // after (possibly) creating a new overlay
   const bool isNewPlugIn = pOverlayTypeDesc != NULL &&
      (dynamic_cast<COverlayFactoryCOM *>(pOverlayTypeDesc->pOverlayFactory) != NULL || 
      dynamic_cast<CFileOverlayFactoryCOM *>(pOverlayTypeDesc->pOverlayFactory) != NULL);
   if (!isNewPlugIn)
   {
      if (set_edit_on(TRUE) != SUCCESS)
      {
         ERR_report("set_edit_on(TRUE) failed.");
         m_pCurrentEditor = NULL;
         SetCurrentEditorGuid(GUID_NULL);
         return FAILURE;
      }
   }

   /**************************************************************************
   Note: set_edit_on(TRUE) is called before the stuff to create an overlay
   matching the new mode if one does not already exist, because the coverage
   overlay needs to know that a coverage overlay did or did not exist when
   the MDM dialog box was brought up.  Because of this, the order must remain
   as is, and all C_overlay::set_edit_on member functions NOT assume that an
   overlay of their type exists before they are called.  They can assume that
   one will be created.
   ***************************************************************************/

   // if an overlay matching the mode does not exist, create one
   make_current_overlay_match_mode(&change, &match);
   if (!match && pEditor != NULL)
   {
      C_overlay *overlay = NULL;

      CString filespec = get_filespec_for_overlay_type(pOverlayTypeDesc);
      if (filespec != "")
      {
         C_overlay *ret_overlay;

         if (OpenFileOverlay(pOverlayTypeDesc->overlayDescriptorGuid,
            filespec, ret_overlay) != SUCCESS)
         {
            string_utils::CGuidStringConverter guidConv(pOverlayTypeDesc->overlayDescriptorGuid);

            CString msg("open() failed: ");
            msg += guidConv.GetGuidString().c_str();
            ERR_report(msg);
            set_edit_on(FALSE);
            m_pCurrentEditor = NULL;
            m_currentEditorGuid = GUID_NULL;
            return FAILURE;
         }
      }
      // For all other overlay types, create a new overlay of the given class
      else if (create(pOverlayTypeDesc->overlayDescriptorGuid, &overlay) != SUCCESS)
      {
         string_utils::CGuidStringConverter guidConv(pOverlayTypeDesc->overlayDescriptorGuid);

         CString msg("create() failed: ");
         msg += guidConv.GetGuidString().c_str();
         ERR_report(msg);
         set_edit_on(FALSE);  //get out of editor mode we just got in
         m_pCurrentEditor = NULL;
         SetCurrentEditorGuid(GUID_NULL);
         return FAILURE;
      }

      // in some cases newly created overlays are not automatically made current
      make_current_overlay_match_mode();
   }

   if (isNewPlugIn)
   {
      if (set_edit_on(TRUE) != SUCCESS)
      {
         ERR_report("set_edit_on(TRUE) failed.");
         m_pCurrentEditor = NULL;
         SetCurrentEditorGuid(GUID_NULL);
         return FAILURE;
      }
   }

   // call the current overlay, if an editor, to enter its own
   // edit_focus state, if necessery.
   if (m_current  &&  pEditor != NULL)
      m_current->enter_edit_focus();

   FVW_refresh_view_map_state();

   // If the editor mode does not support 3D editing (currently all overlays
   // save for the mission overlay) then change to a 2D map
   MapView* map_view = fvw_get_view();
   if (m_currentEditorGuid != FVWID_Overlay_Route &&
      m_currentEditorGuid != GUID_NULL && map_view &&
      map_view->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      map_view->ChangeProjectionType(EQUALARC_PROJECTION);
   }

   return SUCCESS;
}
// end of set_mode



// -----------------------------------------------------------------------------

// Get the default cursor as a function of mode.  If in a edit mode (route
// edit, map data manager, etc.) the default cursor of the associated
// overlay (get_default_cursor() for the associated overlay) is used.  When
// the mode is NULL, the overlay manager's default cursor, a cross hair,
// is returned.
HCURSOR C_ovl_mgr::get_default_cursor()
{
   // test for re-entry, return NULL so cursor is left unchanged
   if (active)
      return NULL;

   // test for re-entry, return NULL so cursor is left unchanged
   if (active)
      return NULL;

   if (m_pCurrentEditor != NULL)
   {
      long hCursor;
      m_pCurrentEditor->GetDefaultCursor(&hCursor);
      return reinterpret_cast<HCURSOR>(hCursor);
   }

   return AfxGetApp()->LoadCursor(IDC_CROSSHAIR);
}

// -----------------------------------------------------------------------------

// If show_type_list is TRUE, then the user is presented with a list of
// overlay types, with the type of the current overlay highlighted by
// default.  After choosing an overlay type the open() member of the class
// corresponding to the choosen overlay type will be called.  If the flag
// is FALSE then it is assumed that an overlay of the current overlay type
// will be opened.    If file_overlays_only is TRUE, then only file overlays
// are included in the list.  Returns a pointer to the overlay opened
// in ret_overlay
int C_ovl_mgr::open(C_overlay *&ret_overlay)
{
   CString class_name;
   C_overlay *overlay;
   ret_overlay = NULL;

   GUID overlayDescGuid = GUID_NULL;

   if (m_pCurrentEditor != NULL)
   {
      // If there is an overlay editor currently open, use the current editor overlay class
      // as the initial class for the multi-file selection dialog if it is a file overlay
      if (OVL_get_type_descriptor_list()->IsFileOverlay(m_current->get_m_overlayDescGuid()))
         overlayDescGuid = m_current->get_m_overlayDescGuid();
   }

   // call the open member of the selected class
   if (OpenFileOverlays(overlayDescGuid, &overlay) != SUCCESS)
   {
      AfxMessageBox("The overlay you requested could not be opened.",MB_ICONSTOP);
      ERR_report("open() failed.");
      return FAILURE;
   }

   // return a pointer to the overlay just opened
   ret_overlay = overlay;

   return SUCCESS;
}

// This method is used to open a list of overlays in a Favorite.  This needs
// to be moved into the FavoriteList class.
int C_ovl_mgr::OpenOverlays(std::vector<GUID> &overlayDescGuidList, 
   std::vector<std::string> &listFileNames)
{
   const size_t count = overlayDescGuidList.size();
   if (listFileNames.size() != count)
   {
      ERR_report("OpenOverlays failed - sizes of lists must match");
      return FAILURE;
   }

   for (size_t i = 0; i < count; ++i)
   {
      GUID overlayDescGuid = overlayDescGuidList[i];
      CString strFileName = listFileNames[i].c_str();

      // if we are dealing with a static overlay
      if (OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid))
      {
         // if the static overlay is not already open, then open it
         if (get_first_of_type(overlayDescGuid) == NULL)
            toggle_static_overlay(overlayDescGuid);
      }
      // otherwise, this is a file overlay
      else
      {
         // replace USER_DATA or HD_DATA in the string with the
         // appropriate path stored in the registry
         if (strFileName.Find("USER_DATA") != -1)
         {
            strFileName = strFileName.Right(strFileName.GetLength() - 10);
            strFileName = PRM_get_registry_string("Main", "USER_DATA") + "\\" + strFileName;
         }
         if (strFileName.Find("HD_DATA") != -1)
         {
            strFileName = strFileName.Right(strFileName.GetLength() - 8);
            strFileName = PRM_get_registry_string("Main", "HD_DATA") + "\\" + strFileName;
         }

         // if the file overlay is not already opened, then open it
         if (is_file_open(strFileName, overlayDescGuid) == NULL)
         {
            C_overlay *pUnusedOverlay;
            OpenFileOverlay(overlayDescGuid, strFileName, pUnusedOverlay);
         }
      }
   }

   return SUCCESS;
}


// -----------------------------------------------------------------------------

// Presents a list of all opened overlays, with the current overlay
// highlighted by default.  If an overlay is choosen then the close member
// of the corresponding class is called.  If file_overlays_only is TRUE,
// then only file overlays are included in the list.
int C_ovl_mgr::close()
{
   CString overlay_name;
   C_overlay *overlay;
   POSITION position;

   // create a overlay selection dialog in select by type mode - allow
   // multiple selection
   const boolean_t file_overlays_only = TRUE;
   const boolean_t multi_select = TRUE;
   SelectOverlayDlg dlg(SelectOverlayDlg::SELECT_OVERLAY, file_overlays_only,
      FALSE, 0, multi_select); 

   dlg.set_title("Close");
   dlg.set_label("Close");

   // if no selection was made then escape
   if (dlg.DoModal() != IDOK)
      return SUCCESS;

   int index = 0;
   int close_OK = 1;
   do
   {
      // get the name of the overlay the user selected
      overlay_name = CString(dlg.get_selection(index++));

      // get the selected overlay
      find_by_name(overlay_name, position);
      if (position)
      {
         overlay = m_overlay_list.GetAt(position);
         CWaitCursor wait;
         if (close(overlay) == FAILURE)
            close_OK = 0;
      }
   } while (overlay_name != "");

   // note : while the Close dialog is displayed, it is possible for a route to be
   // closed from another route server client

   return close_OK ? SUCCESS : FAILURE;
}
// end of close


// -----------------------------------------------------------------------------

// If show_type_list is TRUE, then the user is presented with a list of
// overlay types, with the type of the current overlay highlighted by
// default.  After choosing an overlay type the create() member of the class
// corresponding to the choosen overlay type will be called.  If the flag
// is FALSE then it is assumed that an overlay of the current overlay type
// will be created.  Note: only file overlay types are included by this function.
int C_ovl_mgr::create(boolean_t show_type_list)
{
   OverlayTypeDescriptor* pOverlayTypeDesc = NULL;

   // if show_type_list then allow the user to select an overlay type from a
   // list of all overlay types
   if (show_type_list || m_current == NULL || dynamic_cast<OverlayPersistence_Interface *>(m_current) == NULL)
   {
      // create a overlay selection dialog in select by type mode
      SelectOverlayDlg dlg(SelectOverlayDlg::SELECT_OVERLAY_TYPE, TRUE, FALSE); 

      // set dialog title and list title
      dlg.set_title("New");
      dlg.set_label("New");

      // no selection was made then escape
      if (dlg.DoModal() != IDOK)
         return SUCCESS;

      // get the class name of the type of overlay the user selected
      pOverlayTypeDesc = dlg.get_selected_overlay_type();
   }
   else if (m_current)
      pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(m_current->get_m_overlayDescGuid());

   // special case (we need a better way to do this) - a NULL factory indicates
   // that Mission Package (a non-overlay type) was selected
   if (pOverlayTypeDesc == NULL) 
   {
      CMainFrame *frame = (CMainFrame *)UTL_get_frame();
      if (frame != NULL)
         frame->open_or_new_package();
      return SUCCESS;
   }

   // call the create member of the selected class
   C_overlay *overlay = NULL;
   if (create(pOverlayTypeDesc->overlayDescriptorGuid, &overlay) != SUCCESS)
   {
      ERR_report("create() failed.");
      return FAILURE;
   }

   // switch to the editing mode for the newly created overlay
   SwitchToEditor(overlay);

   return SUCCESS;
}
// end of create

// Switch to the given overlay's edit mode
int C_ovl_mgr::SwitchToEditor(C_overlay* overlay)
{
   if (overlay)
   {
      OverlayTypeDescriptor* overlay_type =
         OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());

      long bAutoEnterEditMode = 1;
      IFvOverrideDefaultUserInterface* pOverrideDefaultUi = dynamic_cast<IFvOverrideDefaultUserInterface *>(overlay_type->pOverlayEditor);
      if (pOverrideDefaultUi != NULL)
      {
         pOverrideDefaultUi->get_m_bAutoEnterOverlayEditor(&bAutoEnterEditMode);
      }

      if (bAutoEnterEditMode && make_mode_match_this_overlay(overlay) != SUCCESS)
      {
         ERR_report("make_mode_match_this_overlay() failed.");
         return FAILURE;
      }
   }

   return SUCCESS;
}

// Prompt the user to save dirty (is_modified()) files, passes back IDYES, 
// IDNO, or IDCANCEL in return_val, returns FAILURE if it is unable to save
// a dirty file.  Note allow_cancel must be TRUE to allow the user to cancel
// the choose Cancel.
int C_ovl_mgr::save_overlay_msg(C_overlay *pOverlay, int *return_val, boolean_t allow_cancel)
{
   CString message;
   int dlg_result = IDYES;

   // ask the user if they want to save the overlay, if allow_cancel is TRUE
   // they may also choose to cancel the current operation
   short result = DialogNotify::SuppressDialogDisplay( MODAL_DLG_SAVE_CHANGES_TO );
   if (result == 0)
   {
      message.Format("Save changes to %s?", GetOverlayDisplayName(pOverlay));
      if (allow_cancel)
         dlg_result = AfxMessageBox(message, MB_YESNOCANCEL | MB_ICONQUESTION);
      else
         dlg_result = AfxMessageBox(message, MB_YESNO | MB_ICONQUESTION);
   }

   // save the overlay if the user requested it
   if (dlg_result == IDYES)
   {
      if (save(pOverlay) != SUCCESS)
         return FAILURE;
   }

   // return IDYES, IDNO, or IDCANCEL in return_val
   if (return_val != NULL)
      *return_val = dlg_result;

   return SUCCESS;
}

// -----------------------------------------------------------------------------

// If show_overlay_list is TRUE, then the user is presented with a list of
// open overlays that are dirty, with the top most overlay highlighted by
// default.  After choosing an overlay the save() member of the overlay
// is be called.  If the flag is FALSE then the current overlay is saved.
// Note: only overlay types that implement IFvOverlayPersistence are included by this function.
int C_ovl_mgr::save(boolean_t show_overlay_list)
{
   CString overlay_name;
   C_overlay *overlay;

   // This function should not be called if there is no current overlay
   if (m_current == NULL)
   {
      ERR_report("There must be a current overlay.");
      return FAILURE;
   }

   // if show_overlay_list then allow the user to select an overlay from a list of all overlays
   if (show_overlay_list)
   {
      POSITION position;
      int count;

      // get the position of the first dirty overlay, and the number of dirty overlays
      position = get_dirty_overlay_position(count);

      // if no overlays are dirty, save the first file overlay anyway
      if (position == NULL)
      {
         POSITION fileOverlayPos = m_overlay_list.GetHeadPosition();
         while (fileOverlayPos != NULL)
         {
            C_overlay *pOverlay = m_overlay_list.GetNext(fileOverlayPos);
            if (dynamic_cast<OverlayPersistence_Interface *>(pOverlay) != NULL)
            {
               overlay = pOverlay;
               break;
            }
         }

         if (overlay == NULL)
         {
            ERR_report("The current overlay must be a file overlay, or it must be dirty.");
            return FAILURE;
         }
      }

      // if only one overlay is dirty, save it
      else if (count == 1)
      {
         overlay = m_overlay_list.GetAt(position);
      }
      // if more than one overlay is dirty, let the user select
      else
      {
         // create a overlay selection dialog in select by type mode
         SelectOverlayDlg dlg(SelectOverlayDlg::SELECT_OVERLAY, FALSE, TRUE); 

         // set dialog title and list title
         dlg.set_title("Save");
         dlg.set_label("Save");

         // no selection was made then escape
         if (dlg.DoModal() != IDOK)
            return SUCCESS;

         // get the name of the overlay the user selected
         overlay_name = dlg.get_selection();

         // get the position of the selected selected overlay
         find_by_name(overlay_name, position);

         overlay = m_overlay_list.GetAt(position);
      }
   }
   else
   {
      if (dynamic_cast<OverlayPersistence_Interface *>(m_current) == NULL && !m_current->is_modified())
      {
         ERR_report("The current overlay must be a file overlay, or it must be dirty.");
         return FAILURE;
      }

      overlay = m_current;
   }

   ASSERT(overlay);

   return save(overlay);
}
// end of save(boolean_t show_overlay_list)


// -----------------------------------------------------------------------------

// If show_overlay_list is TRUE, then the user is presented with a list of
// open overlays, with the current overlay highlighted by default.  After
// choosing an overlay the save_as() member of the overlay is to be called.
// If the flag is FALSE then the save_as() member of the current overlay is
// is called.
// Note: only overlays that implement IFvOverlayPersistence are included by this function.
int C_ovl_mgr::save_as(boolean_t show_overlay_list)
{
   CString overlay_name;
   C_overlay *overlay = NULL;

   // if show_overlay_list then allow the user to select an overlay from a list
   // of all overlays
   if (show_overlay_list)
   {
      OverlayPersistence_Interface *pFvOverlayPersistence = NULL;

      // For each overlay
      int file_overlay_count = 0;
      POSITION fileOverlayPos = m_overlay_list.GetHeadPosition();
      while (fileOverlayPos != NULL)
      {
        C_overlay* current = m_overlay_list.GetNext(fileOverlayPos);

         // If the overlay's type is a file overlay and its save-as filter is
         // non-empty, then consider it
         OverlayTypeDescriptor* desc =
            OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(
                  current->get_m_overlayDescGuid());
         if (desc->fileTypeDescriptor.bIsFileOverlay &&
               !desc->fileTypeDescriptor.saveFileDialogFilter.IsEmpty())
         {
            overlay = current;
            file_overlay_count++;
            if (file_overlay_count > 1)
               break;
         }
      }

      // If there are no file overlays that meet our criteria then there is
      // nothing to do
      if (file_overlay_count == 0)
         return SUCCESS;

      // if there is more than one file overlay, let them choose
      if (file_overlay_count > 1)
      {
         POSITION position;

         // create a overlay selection dialog in select by type mode
         SelectOverlayDlg dlg(SelectOverlayDlg::SELECT_OVERLAY, TRUE, FALSE, 0, FALSE, TRUE);

         // set dialog title and list title
         dlg.set_title("Save As");
         dlg.set_label("Select File Overlay");
         dlg.set_OK_label("Save As...");

         if ( DialogNotify::SuppressDialogDisplay( MODAL_DLG_SAVEAS_OVERLAY ) )
            return SUCCESS;

         // no selection was made then escape
         if (dlg.DoModal() != IDOK)
            return SUCCESS;

         // get the name of the overlay the user selected
         overlay_name = dlg.get_selection();

         // get the selected overlay
         find_by_name(overlay_name, position);
         overlay = m_overlay_list.GetAt(position);
      }
   }
   else
   {
      // if the current overlay is a file overlay, then use it
      if (dynamic_cast<OverlayPersistence_Interface *>(m_current) == NULL)
      {
         ERR_report("No file overlay was found.");
         return FAILURE;
      }

      overlay = m_current;
   }

   ASSERT(overlay);

   return save_as(overlay);
}
// end of save_as(boolean_t show_overlay_list)


// -----------------------------------------------------------------------------

// Does a save for a specific overlay - must be file overlay
int C_ovl_mgr::save(C_overlay *overlay)
{
   CString message;

   long bHasBeenSaved = 0;
   OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
   if (pFvOverlayPersistence == NULL)
   {
      ERR_report("C_ovl_mgr::save should only be called with file overlays");
      return FAILURE;
   }

   pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);

   _bstr_t fileSpecification;
   pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
   const char *current_ovl_spec = (char *)fileSpecification;

   // if this is a new overlay that has never been saved then call save_as
   if (!bHasBeenSaved || current_ovl_spec == "")
      return save_as(overlay);

   // if the file already exists then it must be checked for read-only status
   if (FIL_access(current_ovl_spec, FIL_EXISTS) == SUCCESS)
   {      
      // if the user doesn't have write access to the file
      if (FIL_access(current_ovl_spec, FIL_WRITE_OK) != SUCCESS)
      {
         if (dynamic_cast<OverlayPersistence_Interface *>(overlay) != NULL)
         {
            message.Format("%s is a Read Only file.\nYou must save your changes to a different file.\n"
               "Save changes to a different file?", current_ovl_spec);
            if (IDYES == AfxMessageBox(message, MB_YESNO))
               return save_as(overlay);
         }
         // The following case should never occur.  To ensure this, CStaticOverlay-derived objects 
         // must not be editable if they come from read-only files
         else
         {
            message.Format("%s is a Read Only file.\nYour changes cannot be saved.\n", current_ovl_spec);
            AfxMessageBox(message);
            return SUCCESS;
         }

         return SUCCESS;
      }
   }

   // inform the overlay that it is about to be saved so it can tie up any
   // loose ends before it attempts to save itself
   boolean_t cancel;
   if (overlay->pre_save(&cancel) != SUCCESS)
   {
      ERR_report("pre_save() failed.");
      return FAILURE;
   }

   // if the save has been canceled then abort
   if (cancel)
      return SUCCESS;

   // call the save member of this overlay...
   message.Format("Saving %s...", current_ovl_spec);
   CFVWaitHelp wait(message);

   const long eSaveFormatUnspecified = 0;
   if ( pFvOverlayPersistence->FileSaveAs(fileSpecification, eSaveFormatUnspecified) != S_OK)
   {
      message.Format("Error saving %s.", current_ovl_spec);
      AfxMessageBox(message);      
      return FAILURE;
   }

   const int overlay_handle = get_overlay_handle(overlay);
   notify_clients_of_save(overlay_handle);

   // update most recently used file list
   FVW_add_to_recent_file_list(overlay->get_m_overlayDescGuid(), current_ovl_spec);

   // update the title bar, in case the status went from dirty to clean
   if (m_current == overlay)
      FVW_update_title_bar();

   return SUCCESS;
}
// end of save(C_overlay *overlay)


// -----------------------------------------------------------------------------

// Does a save_as for a specific file overlay.
int C_ovl_mgr::save_as(C_overlay *overlay, CWnd* pParentWnd /* = NULL */,CString file_specification /* = "" */)
{
   if ( DialogNotify::SuppressDialogDisplay( MODAL_DLG_SAVEAS_OVERLAY ) )
      return SUCCESS;

   CString name;

   OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
   if (pFvOverlayPersistence == NULL)
   {
      ERR_report("C_ovl_mgr::save_as must be passed a file overlay");
      return FAILURE;
   }

   // inform the overlay that it is about to be saved so it can tie up any
   // loose ends before it attempts to save itself
   boolean_t cancel;
   if (overlay->pre_save(&cancel) != SUCCESS)
   {
      ERR_report("pre_save() failed.");
      return FAILURE;
   }

   // if the save_as has been canceled then abort
   if (cancel)
      return SUCCESS;

   // use overlay name if the files has been saved before,
   // use *.ext for new overlays
   long bHasBeenSaved = 0;
   pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);
   if (bHasBeenSaved && OVL_get_overlay_manager()->GetOverlayDisplayName(overlay).GetLength())
      name = OVL_get_overlay_manager()->GetOverlayDisplayName(overlay);
   else
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid() );
      name.Format("*.%s", pOverlayTypeDesc->fileTypeDescriptor.defaultFileExtension);
   }

   const int nOverlayHandle = get_overlay_handle(overlay);

   _bstr_t fileSpecification;
   pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
   CString strSpec = (char *)fileSpecification;

   // get the new name from the user
   long nSaveFormat = 0;
   if (file_specification.GetLength() == 0)
      file_specification = get_file_specification_from_user(overlay->get_m_overlayDescGuid(), name, &nSaveFormat, pParentWnd);

   // get_file_specification_from_user() returns "" if the user selected cancel
   if (file_specification.GetLength() == 0)
      return SUCCESS;

   // since the Save-As dialog can pump messages, an external client can close an overlay while the Save-As dialog
   // is opened.  Check to make sure the overlay still exists
   if (lookup_overlay(nOverlayHandle) == NULL)
   {
      CString msg;
      msg.Format("The overlay %s was already closed by an external client.  Aborting the Save-As operation.", strSpec);
      AfxMessageBox(msg);
      return SUCCESS;
   }


   // save file under the new name
   CString message;
   message.Format("Saving %s...", (const char *)file_specification);
   CFVWaitHelp wait(message);

   if (pFvOverlayPersistence->FileSaveAs( _bstr_t(file_specification), nSaveFormat) != S_OK)
   {
      CString message;
      message.Format("Error saving %s.", file_specification);
      AfxMessageBox(message);
      return FAILURE;
   }

   // Set the read-only flag after the file has been saved
   const long read_only = FIL_access(file_specification, FIL_WRITE_OK) != SUCCESS;
   pFvOverlayPersistence->put_m_bIsReadOnly(read_only);

   pFvOverlayPersistence->put_m_bHasBeenSaved(TRUE);

   const int overlay_handle = get_overlay_handle(overlay);
   notify_clients_of_save(overlay_handle);

   // update most recently used file list
   FVW_add_to_recent_file_list(overlay->get_m_overlayDescGuid(), file_specification);

   // update the title bar, in case the current overlay name changed
   if (m_current == overlay)
      FVW_update_title_bar();

   // if this is a time-sensitve overlay then update the gant chart since
   // the name of othe overlay changed
   PlaybackTimeSegment_Interface *pFvTimeSegment = dynamic_cast<PlaybackTimeSegment_Interface *>(overlay);
   if (pFvTimeSegment != NULL && pFvTimeSegment->SupportsFvPlaybackTimeSegment())
   {
      CMainFrame::GetPlaybackDialog().update_gant_chart();
   }

   // if the mission binder dialog is opened, then the overlay list
   // need to be updated
   if (CMainFrame::m_mission_binder_dlg != NULL)
      CMainFrame::m_mission_binder_dlg->update_overlay_list();

   if (g_tabular_editor_dlg && overlay->is_tabular_editable()) 
   {
      g_tabular_editor_dlg->update_overlay_tabs(CTabularEditorDlg::TAB_UPDATE_CHANGE_DISPLAY_NAME, 
         GetOverlayDisplayName(overlay), overlay);
   }

   // Notify any observers that the overlay list has been reordered
   {
      fv_common::ScopedCriticalSection lock(
         &m_overlay_stack_changed_observers_cs);
      for (auto it=std::begin(m_overlay_stack_changed_observers);
         it != std::end(m_overlay_stack_changed_observers); ++it)
      {
         try
         {
            (*it)->OverlayFileSpecificationChanged(overlay);
         }
         catch(_com_error& e)
         {
            // report error and contnue
            REPORT_COM_ERROR(e);
         }
      }
   }

   return SUCCESS;
}
// end of save_as(C_overlay *overlay)

// Puts up a dialog box to allow the user to select a file or a file specification.  
// Returns "" for cancel or a full file specification otherwise
CString C_ovl_mgr::get_file_specification_from_user( GUID overlayDescGuid,
   const char *default_name, long *nSaveFormat, CWnd *pParentWnd /* = NULL */)
{
   CString default_filter;
   CString default_extension;

   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);

   // set default extension and filter
   default_extension = pOverlayTypeDesc->fileTypeDescriptor.defaultFileExtension;

   if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      default_filter = pOverlayTypeDesc->fileTypeDescriptor.saveFileDialogFilter;

   // make sure there are no double pipes in the filter string...
   default_filter.Replace("||", "|");

   // add the final "All Files" filter to the string...
   default_filter += "All Files (*.*)|*.*||";

   // when doing a save as the user will be prompted if they try to overwrite an existing
   // file and they won't be allowed to choose a read-only file
   CFileDialog dlg(FALSE, default_extension, default_name, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN, 
      default_filter, pParentWnd);
   dlg.m_ofn.nFilterIndex = 1L;

   CString defaultDirectory;
   if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
   {
      defaultDirectory = overlay_type_util::get_most_recent_directory(pOverlayTypeDesc->overlayDescriptorGuid);
      dlg.m_ofn.lpstrInitialDir = defaultDirectory;
   }

   // allow the user to select a file
   if (dlg.DoModal() != IDOK)
      return "";

   // get the index of the dialog file filter selected by the user
   *nSaveFormat = dlg.m_ofn.nFilterIndex;

   return dlg.GetPathName();
}
// END OF get_file_specification_from_user()


// -----------------------------------------------------------------------------

// Save all file overlays.
int C_ovl_mgr::save_all()
{
   POSITION next;
   C_overlay *overlay;

   // nothing to save
   if (m_overlay_list.IsEmpty())
      return SUCCESS;

   next = m_overlay_list.GetHeadPosition();
   do
   {
      overlay = m_overlay_list.GetNext(next);

      if (dynamic_cast<OverlayPersistence_Interface *>(overlay) != NULL || overlay->is_modified())
      {
         if (save(overlay) != SUCCESS)
         {
            ERR_report("save() failed.");
            return FAILURE;
         }
      }
   } while (next != NULL);

   return SUCCESS;
}
// end of save_all


// Open one or more file overlays from the multi-select file overlay dialog.  The given overlay type is
// used to initialize the default filter and initial directory in the file dialog.  The last opened overlay
// is returned.
int C_ovl_mgr::OpenFileOverlays(GUID overlayDescGuid, C_overlay **ret_overlay)
{
   *ret_overlay = NULL;

   CString default_extension;
   CString default_directory;

   // get the factory from the given class name
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);

   // if the factory exists, then get the default extension/directory directly from the factory
   if (pOverlayTypeDesc != NULL)
   {
      if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      {
         default_extension = pOverlayTypeDesc->fileTypeDescriptor.defaultFileExtension;
         default_directory = pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory;
      }
   }
   // otherwise, the factory doesn't exist.  Assign defaults applicable to all overlays
   else
   {
      default_extension = ".*";
      default_directory = PRM_get_registry_string("Main", "ReadWriteUserData");
   }

   CFileOverlayDialog dlg(default_extension, default_directory, m_file_open_parent_wnd);
   if (dlg.DoModal() == IDOK)
   {
      POSITION position = dlg.GetStartPosition();
      while (position)
      {
         CString full_path = dlg.GetNextPathName(position);

         // First, get the factory that corresponds to the current file's extension.
         // This is done by searching for the current extension in the default filter
         // strings of all available factories.  When a match is found, the corresponding
         // factory is used to open the current file.
         const int index = full_path.ReverseFind('.');
         CString ext = full_path.Mid(index);
         OverlayTypeDescriptor *pOverlayTypeDesc = overlay_type_util::GetOverlayTypeFromFileExtension((LPCTSTR)ext);

         // If a matching factory was not found, then the file being opened is not a
         // FalconView file.  So launch the appropriate app to open the current file...
         if (pOverlayTypeDesc == NULL)
         {
            ShellExecute(AfxGetMainWnd()->m_hWnd, "open", full_path, NULL, NULL, SW_SHOWNORMAL);
         }

         else  // otherwise, use the factory we just found to open the current file...
         {
            OpenFileOverlay(pOverlayTypeDesc->overlayDescriptorGuid, full_path, *ret_overlay);
         }
      }

      // invalidate the display so everything is redrawn
      invalidate_all();
   }

   return SUCCESS;
}
// end of open

void C_ovl_mgr::RegisterEvents(OverlayStackChangedObserver_Interface* observer)
{
   fv_common::ScopedCriticalSection lock(
         &m_overlay_stack_changed_observers_cs);
   m_overlay_stack_changed_observers.insert(observer);
}

void C_ovl_mgr::UnregisterEvents(OverlayStackChangedObserver_Interface* observer)
{
   fv_common::ScopedCriticalSection lock(
         &m_overlay_stack_changed_observers_cs);

   auto it = m_overlay_stack_changed_observers.find(observer);
   if (it != std::end(m_overlay_stack_changed_observers))
      m_overlay_stack_changed_observers.erase(it);
}

// Return the user friendly name for the given overlay used throughout the user interface
CString C_ovl_mgr::GetOverlayDisplayName(C_overlay *pOverlay)
{

   // if the overlay implements the IFvOverlayTypeOverrides interface, allow it
   // to override the overlay type's display name, checked first for plugins 
   // wanting to override the display name in title bar / overlay manager
   OverlayTypeOverrides_Interface *pFvOverlayTypeOverrides = dynamic_cast<OverlayTypeOverrides_Interface *>(pOverlay);
   if (pFvOverlayTypeOverrides != NULL && pFvOverlayTypeOverrides->SupportsFvOverlayTypeOverrides())
   {
      _bstr_t displayName;
      pFvOverlayTypeOverrides->get_m_displayName(displayName.GetAddress());
      if ( displayName.length() > 0 )
         return (LPCTSTR) displayName;
   }

   // OverlayPersistence_Interface overlay
   OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pOverlay);
   if (pFvOverlayPersistence != NULL)
   {
      _bstr_t fileSpecification;
      pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

      // if collaboration is enabled, check to see if the given overlay has an
      // alternate file specification that should be used for its display name
      CMainFrame* frame = fvw_get_frame();
      if (frame && frame->m_pCollaborationServer)
      {
         try
         {
            _bstr_t bstrUNC;
            VARIANT_BOOL varIsPublisher;
            VARIANT_BOOL varIsSubscriber;
            VARIANT_BOOL varIsReadOnly;
            VARIANT_BOOL varFound = VARIANT_FALSE;

            int hOverlay = this->get_overlay_handle( pOverlay );
            ICollaborationRouterPtr pRouter;
            frame->m_pCollaborationServer->GetRouter( &pRouter );

            pRouter->GetLocalCollaborationStatus(hOverlay, 
               fileSpecification, 
               bstrUNC.GetAddress(),
               &varIsPublisher,
               &varIsSubscriber,
               &varIsReadOnly,
               &varFound );
            if ( varFound == VARIANT_TRUE )
            {
               CString strUNCPath( (char*)bstrUNC );
               CFileSpec fs( strUNCPath );
               CString strLocalPath = fs.GetFullSpec();
               INT i = strLocalPath.Find("\\noshare\\");
               if ( i != -1 )
                  fileSpecification = strLocalPath.Mid(i+strlen("\\noshare\\"));
            }
         }
         catch(_com_error& e)
         {
            REPORT_COM_ERROR(e);
         }
      }

      CString ret;
      char buf[_MAX_FNAME];
      if (::GetFileTitle(fileSpecification, buf, _MAX_FNAME) != 0)
      {
         ret = CString(::PathFindFileName((char *)fileSpecification));
      }
      else
         ret = CString(buf);

      return ret;
   }

   // otherwise, the overlay is a static overlay, and we will use the display name from the type descriptor
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(pOverlay->get_m_overlayDescGuid());
   if (pOverlayTypeDesc != NULL)
      return pOverlayTypeDesc->displayName;

   return "";
}

// -----------------------------------------------------------------------------

// Calls the create member of the given class.  Note: this function will fail
// if it is called for a StaticOverlay that is already opened.  Returns
// FAILURE if an error occurs, SUCCESS otherwise.  The value returned in
// overlay is a pointer to the newly created overlay, it will be set to NULL
// when a failure occurs or when the user cancels the operation (function
// still returns SUCCESS).
int C_ovl_mgr::create(GUID overlayDescGuid, C_overlay **overlay)
{
   *overlay = NULL;

   OverlayTypeDescriptor *pOverlayTypeDesc =
      OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc == NULL)
   {
      ERR_report("C_ovl_mgr::create failed - the given overlay type descriptor "
         "guid is not valid");
      return FAILURE;
   }

   C_overlay* temp = NULL;
   if (!pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
   {  
      if (toggle_static_overlay(overlayDescGuid) != SUCCESS)
      {
         ERR_report("C_ovl_mgr::create - unable to toggle static overlay");
         return FAILURE;
      }

      *overlay = get_first_of_type(overlayDescGuid);
      return SUCCESS;
   }

   // otherwise, the overlay type is a file overlay
   //

   if (pOverlayTypeDesc->pOverlayFactory->CreateOverlayInstance(&temp) ==
      S_OK &&
      temp != NULL)
   {
      // to support legacy plug-in overlays which use an overlay handle, we need
      // to add the overlay to the overlay map (which associates an overlay
      // handle with the overlay) before calling Initialize
      const int nOverlayHandle = m_next_overlay_handle++;
      m_overlay_map.SetAt(nOverlayHandle, temp);

      if (temp->InternalInitialize(overlayDescGuid) != S_OK)
      {
         ERR_report("C_ovl_mgr::create - Initialization of overlay failed");
         m_overlay_map.RemoveKey(nOverlayHandle);
         temp->Release();
         return FAILURE;
      }

      // verify that the overlay set its overlay descriptor guid in Initialize.
      // The reason we don't just expose a propget for the overlay descriptor
      // guid and set it ourselves is that we want the property to be read-only
      if (temp->get_m_overlayDescGuid() !=
         pOverlayTypeDesc->overlayDescriptorGuid)
      {
         ERR_report("C_ovl_mgr::create - the overlay descriptor guid property "
            "was not set in Initialize()");
         m_overlay_map.RemoveKey(nOverlayHandle);
         temp->Release();
         return FAILURE;
      }

      OverlayPersistence_Interface *pFvOverlayPersistence =
         dynamic_cast<OverlayPersistence_Interface *>(temp);
      if (pFvOverlayPersistence == NULL)
      {
         ERR_report("C_ovl_mgr::create - file overlays must implement the "
            "IFvOverlayPersistence interface");
         m_overlay_map.RemoveKey(nOverlayHandle);
         temp->Release();
         return FAILURE;
      }

      if (AddOverlayToStack(temp, overlayDescGuid) != SUCCESS)
      {
         ERR_report("C_ovl_mgr::create - unable to add overlay to stack");
         return FAILURE;
      }

      // The reason we set m_bHasBeenSaved and m_bIsDirty before calling FileNew
      // is that it gives FileNew a chance to modify these if necesssary and at
      // the same time sets the defaults for typical overlays.

      pFvOverlayPersistence->put_m_bHasBeenSaved(FALSE);
      pFvOverlayPersistence->put_m_bIsDirty(FALSE);

      if (pFvOverlayPersistence->FileNew() != S_OK)
      {
         ERR_report("C_ovl_mgr::create - FileNew failed");
         RemoveOverlayFromStack(temp);
         m_overlay_map.RemoveKey(nOverlayHandle);
         temp->Release();
         return FAILURE;
      }

      // verify that the file specification has been set in FileNew.  The
      // property is read-only
      _bstr_t fileSpecification;
      if (pFvOverlayPersistence->
         get_m_fileSpecification(fileSpecification.GetAddress()) != S_OK ||
         fileSpecification.length() == 0)
      {
         ERR_report("C_ovl_mgr::create - the file specification, "
            "m_fileSpecification, must be set in FileNew");
         RemoveOverlayFromStack(temp);
         m_overlay_map.RemoveKey(nOverlayHandle);
         temp->Release();
         return FAILURE;
      }

      temp->InitializeDisplayElementRootNode();

      OnOverlayAdded(temp, TRUE);

      // in this case, we'll intelligently draw the overlays
      draw_from_overlay(temp);

      // return a pointer to the newly created overlay
      *overlay = temp;
   }
   else
   {  
      return FAILURE;
   }

   return SUCCESS;
}
// end of create


// -----------------------------------------------------------------------------

int C_ovl_mgr::PreCloseOverlay(C_overlay *pOverlay, boolean_t *cancel)
{
   ASSERT(pOverlay != NULL);

   pOverlay->pre_close(cancel);
   if (cancel != NULL && *cancel == TRUE)
      return SUCCESS;

   // ask the user if they want to save a dirty file if the overlay is not 
   // a legacy plug-in overlay.  This is because we could potentially affect
   // the behavior of an existing legacy plug-in which assumes this is
   // never done
   CBaseLayerOvl *pBaseLayerOvl = dynamic_cast<CBaseLayerOvl *>(pOverlay);
   if (pBaseLayerOvl == NULL && pOverlay->is_modified())
   {
      int return_code;

      if (save_overlay_msg(pOverlay, &return_code,(cancel != NULL)) != SUCCESS)
      {
         return FAILURE;
      }

      if (return_code == IDCANCEL)
      {
         if (cancel != NULL)
            *cancel = TRUE;
         return SUCCESS;
      }
   }

   return SUCCESS;
}

// Close the given overlay.
int C_ovl_mgr::close(C_overlay *overlay)
{
   boolean_t cancel;
   boolean_t change;
   boolean_t match;

   // Bugzilla Bug 4007 Error message when turning shadow Overlay off.
   // Don't unsubscribe a static overlay which has no file spec, etc.
   OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
   if (pFvOverlayPersistence != NULL) 
   {
      CMainFrame* frame = fvw_get_frame();
      if ( frame && frame->m_pCollaborationServer ) 
      {
         // make sure this overlay does not exist in any collaborations
         int hoverlay = get_overlay_handle(overlay);

         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

         frame->m_pCollaborationServer->Unsubscribe(hoverlay,fileSpecification,NULL);
      }
   }

   // inform the overlay that it is about to be closed so it can tie up any
   // loose ends before it is destroyed
   if (PreCloseOverlay(overlay, &cancel) != SUCCESS)
   {
      ERR_report("pre_close() failed.");
      return FAILURE;
   }

   // if the close has been canceled then abort
   if (cancel)
      return SUCCESS;

   // tell any connected clients that this overlay is about to be closed
   notify_clients_of_close(get_overlay_handle(overlay), FALSE);

   CWaitCursor wait;

   // remove the overlay from the list and the display, and delete the object
   if (delete_overlay(overlay, TRUE) != SUCCESS)
   {
      ERR_report("delete_overlay() failed.");
      return FAILURE;
   }
   overlay->Release();

   // now that the overlay has been deleted and removed from the ovlmgr list, 
   // refresh the subscribe publish dialogs
   CMainFrame* frame = fvw_get_frame();
   if ( frame && frame->m_pCollaborationServer )
   {
      frame->m_pCollaborationServer->Update();
   }

   // maintain the same mode if an overlay of the corresponding type exists,
   // otherwise switch to NULL mode
   make_current_overlay_match_mode(&change, &match);
   if (!match && m_pCurrentEditor != NULL)
      set_mode(GUID_NULL, FALSE);

   // set the user interface state based on overlay/editor requirements
   FVW_refresh_view_map_state();

   return SUCCESS;
}


// -----------------------------------------------------------------------------

// Turn the current editor on or off.  The current mode (m_pCurrentEditor) determines
// which editor, if any.
int C_ovl_mgr::set_edit_on(boolean_t on)
{
   if (m_pCurrentEditor != NULL)
   {
      HRESULT hr;
      if (on)
      {
         hr = m_pCurrentEditor->ActivateEditor();
      }
      else
      {
         hr = m_pCurrentEditor->DeactivateEditor();
      }

      return hr == S_OK ? SUCCESS: FAILURE;
   }

   return SUCCESS;
}

// -----------------------------------------------------------------------------
void C_ovl_mgr::invalidate_rect(int left, int top, int right, int bottom, int delta_x, 
   int delta_y, double rotation, CWnd *map_view, boolean_t erase_background)
{
   CRect rect(left, top, right, bottom);   

   // inflate rectangle to bound the region
   rect.InflateRect(delta_x, delta_y);

   // If the view is rotated then the view coordinate rectangle that bounds
   // the rotated device coordinate rectangle must be computed.  When rotation
   // is 0.0 view coordinates are the same as device coordinates.
   if (rotation != 0.0)
   {
      double adjusted_angle;
      double cosine;
      double sine;
      double width;
      double height;
      CPoint device_center;
      int vcx, vcy;
      int half_width;
      int half_height;

      // normalize the angle for sine and cosine computation
      if (rotation <= 90.0)
         adjusted_angle = rotation;
      else if (rotation <= 180.0)
         adjusted_angle = rotation - 90.0;
      else if (rotation <= 270.0)
         adjusted_angle = rotation - 180.0;
      else
         adjusted_angle = rotation - 270.0;

      // compute the sine and cosine
      adjusted_angle = DEG_TO_RAD(adjusted_angle);
      cosine = cos(adjusted_angle);
      sine = sin(adjusted_angle);

      // compute the dimensions of the view rectangle which bounds the
      // device coordinate rectangle
      width = (double)rect.Width() * cosine + 
         (double)rect.Height() * sine;
      height = (double)rect.Width() * sine + 
         (double)rect.Height() * cosine;

      // get the center of the rectangle in view coordinates
      MapProj* map = get_view_map();
      device_center = rect.CenterPoint();
      map->vsurface_to_surface(device_center.x, device_center.y, &vcx, &vcy);

      // set the rectangle to a view coordinate rectangle that bounds
      // the rotated device coordinate rectangle
      half_width = (int)(width/2.0 + 0.5);
      half_height = (int)(height/2.0 + 0.5);
      rect.SetRect(vcx, vcy, vcx, vcy);
      rect.InflateRect(half_width, half_height);
   }

   // invalidate the rectangle if it is not empty
   if (map_view && !rect.IsRectEmpty())
      UTL_invalidate_view_rect(map_view, rect, erase_background);

   // for now, any invalidation of the map window will cause the entire
   // vertical view to become invalidated
   CMainFrame* frame = fvw_get_frame();
   if (frame && frame->GetVerticalDisplay() != NULL)
      frame->GetVerticalDisplay()->Invalidate();
}

// -----------------------------------------------------------------------------

// If the overlay already exists, it is closed.  Otherwise it is opened.
int C_ovl_mgr::toggle_static_overlay(GUID overlayDescGuid)
{
   OverlayTypeDescriptor* pOverlayTypeDesc =
      OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc == NULL)
   {
      ERR_report("Unable to retrieve an overlay type");
      return FAILURE;
   }

   C_overlay *pOverlay = NULL;
   if (pOverlay = get_first_of_type(overlayDescGuid))
   {
      return close(pOverlay);
   }
   else
   {
      CWaitCursor wait;

      // open a new overlay via the factory
      if (pOverlayTypeDesc->pOverlayFactory->CreateOverlayInstance(&pOverlay) ==
         S_OK && pOverlay != NULL)
      {
         // to support legacy plug-in overlays which use an overlay handle, we
         // need to add the overlay to the overlay map (which associates an
         // overlay handle with the overlay) before calling Initialize
         const int nOverlayHandle = m_next_overlay_handle++;
         m_overlay_map.SetAt(nOverlayHandle, pOverlay);

         if (pOverlay->InternalInitialize(
            pOverlayTypeDesc->overlayDescriptorGuid) != S_OK)
         {
            ERR_report("C_ovl_mgr::toggle_static_overlay - Initialization of "
               "overlay failed");
            m_overlay_map.RemoveKey(nOverlayHandle);
            pOverlay->Release();
            return SUCCESS;
         }

         // verify that the overlay set its overlay descriptor guid in
         // Initialize.  The reason we don't just expose a propget for the
         // overlay descriptor guid and set it ourselves is that we want the
         // property to be read-only
         if (pOverlay->get_m_overlayDescGuid() !=
            pOverlayTypeDesc->overlayDescriptorGuid)
         {
            ERR_report("C_ovl_mgr::toggle_static_overlay - the overlay "
               "descriptor guid property was not set in Initialize()");
            m_overlay_map.RemoveKey(nOverlayHandle);
            pOverlay->Release();
            return FAILURE;
         }

         if (AddOverlayToStack(pOverlay, overlayDescGuid) != SUCCESS)
         {
            ERR_report("C_ovl_mgr::toggle_static_overlay - unable to add "
               "overlay to stack");
            return FAILURE;
         }

         pOverlay->InitializeDisplayElementRootNode();

         // if a static overlay has an associated editor (Map Data Manager,
         // NITF), then enter the overlay's edit mode
         if (pOverlayTypeDesc->pOverlayEditor != NULL)
         {
            // by default, we automatically enter the associated edit mode.  An
            // editor can override this behavior by implementing
            // IFvOverrideDefaultUserInterface
            long bAutoEnterEditMode = TRUE;

            IFvOverrideDefaultUserInterface* pOverrideDefaultUi =
               dynamic_cast<IFvOverrideDefaultUserInterface *>(
               pOverlayTypeDesc->pOverlayEditor);
            if (pOverrideDefaultUi != NULL)
            {
               pOverrideDefaultUi->get_m_bAutoEnterOverlayEditor(
                  &bAutoEnterEditMode);
            }

            if (bAutoEnterEditMode)
               set_mode(pOverlayTypeDesc->overlayDescriptorGuid);
         }

         OnOverlayAdded(pOverlay, TRUE);
      }
   }

   return SUCCESS;
}

// Put up the "Reorder Overlays" dialog, to allow the user to change the
// display order for all currently open overlays.
int C_ovl_mgr::reorder_overlays_dialog()
{
   ReorderOverlaysDlg dlg;//(get_frame());
   dlg.DoModal();
   return SUCCESS;
}

int C_ovl_mgr::overlay_options_dialog(GUID overlayDescGuid /* = GUID_NULL */,
   GUID propertyPageUid /*= GUID_NULL */)
{
   OverlayOptionsDlg dlg;

   // Use the topmost overlay if there is not a specific one requested
   if (overlayDescGuid == GUID_NULL && propertyPageUid == GUID_NULL)
   {
      C_overlay* pOverlay = get_current_overlay();
      if (pOverlay)
         dlg.SetInitialPropertyPage(pOverlay->get_m_overlayDescGuid(), GUID_NULL);
   }
   else
      dlg.SetInitialPropertyPage(overlayDescGuid, propertyPageUid);

   // Locking the temporary maps is necessary to work around an MFC bug in
   // COccManager::IsDialogMessage when a dialog contains an ActiveX control
   AfxLockTempMaps();      
   dlg.DoModal();
   AfxUnlockTempMaps();

   return SUCCESS;
}

void C_ovl_mgr::RestoreStartupOverlays()
{
   CString string;
   CString overlay_stats;
   CString overlay_class;
   CString overlay_file;
   const int IDENX_STR_LEN = 5;
   char index_str[IDENX_STR_LEN];
   int index;
   boolean_t hide_flag;
   int last_status;
   char* buffer;
   char* hide_string;
   char* lpszOverlayDescGuid;
   char* file_string;

   // see if the display state is to be restored from the INI file
   string = PRM_get_registry_string("Overlay Manager", 
      "Restore Display State", "YES");

   if (string == "YES")
   {
      // get the count of overlays that are available for startup load
      index = PRM_get_registry_int("Overlay Manager\\Startup2", "Overlay Count", 0);

      // set flag to overide the default behavior of add_overlay which is
      // called by the open functions below
      m_startup_load_active = TRUE;

      // loop through all overlays in the Startup section of the registry,
      // toggling the static overlays and opening the file overlays.

      // we start with a decrement because we've zero-based the overlay numbers
      for (index--; index >= 0; index--)
      {
         _itoa_s(index, index_str, IDENX_STR_LEN, 10);
         overlay_stats = PRM_get_registry_string("Overlay Manager\\Startup2", index_str, "NONE");

         if (overlay_stats != "NONE")
         {
            // we'll use the C runtime routine strtok to help us parse
            // the input line by comma delimiter - it needs a char * that
            // it can write to
            buffer = overlay_stats.GetBuffer(overlay_stats.GetLength() + 1);

            char* context;
            hide_string = strtok_s(buffer, "|", &context);
            if (hide_string == NULL)
               continue;

            lpszOverlayDescGuid = strtok_s(NULL, "|", &context);
            if (lpszOverlayDescGuid == NULL)
               continue;

            file_string = strtok_s(NULL, "|", &context);

            string_utils::CGuidStringConverter guidConv(lpszOverlayDescGuid);
            GUID overlayDescGuid = guidConv.GetGuid();

            // check for the existance of the file
            if (OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid))
            {
               // if a file overlay is missing its file specification, or the
               // file does not exist, it can not be restored
               if (file_string == NULL || FIL_access(file_string, FIL_READ_OK) != SUCCESS)
               {
                  overlay_stats.ReleaseBuffer(-1);
                  continue;
               }
            }

            // initialize to FAILURE
            last_status = FAILURE;

            // the string is made up of two (static) or three (file) parts:
            // Show/Hide,class_name[,file_name]
            hide_flag = (strcmp(hide_string, "Hide") == 0);

            // check to see if this overlay type should be restored at startup
            OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
            if (pOverlayTypeDesc != NULL)
            {
               const int nRestoreAtStartup = PRM_get_registry_int("Overlay Manager\\Restore2", guidConv.GetGuidString().c_str(), pOverlayTypeDesc->bDefaultRestoreAtStartup);
               if (!nRestoreAtStartup)
                  continue;
            }

            // if we've got a filename and the class type is file overlay, 
            // then open that way
            if (file_string && OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid))
            {
               C_overlay *ret_overlay;
               last_status = OpenFileOverlay(overlayDescGuid, file_string, ret_overlay);
            }

            // otherwise make sure its a static overlay
            else if (OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid))
            {
               last_status = SUCCESS;
               if (OVL_get_overlay_manager()->get_first_of_type(overlayDescGuid) == NULL)
                  last_status = toggle_static_overlay(overlayDescGuid);
            }
            else
            {
               CString msg;

               last_status = FAILURE;

               msg.Format("%s is not a recognized overlay type", lpszOverlayDescGuid);
               ERR_report(msg);
            }

            // done with the buffer, so it can be released
            overlay_stats.ReleaseBuffer(-1);

            // make sure that the current overlay corresponds to what we just
            // tried to open
            if (last_status == SUCCESS && m_current != NULL)
            {
               ASSERT(m_current);  
               m_current->put_m_bIsOverlayVisible(!hide_flag);
               continue;
            }

            // skip the error log for NO_READ_ONLY error, as the user was
            // alread told about it
            if (last_status != NO_READ_ONLY)
               ERR_report("Unopened startup overlay condition: "
               + overlay_stats);
         }
      }

      // clear flag to overide the default behavior of add_overlay, so it
      // behaves normally for the duration of this execution
      m_startup_load_active = FALSE;
   }

   // if we have a file that needs to be opened on startup (with the /O command
   // line parameter) we should open it now
   CMainFrame* frame = fvw_get_frame();
   if (m_startup_overlay != "" && frame)
      frame->CommandLineOverlayOpen(m_startup_overlay);

   // post a message to the view to redraw itself since overlays could
   // have been opened above - we can't just call invalidate here because we 
   // are currently in the draw loop
   CView *view = fvw_get_view();
   if (view != NULL)
      view->PostMessage(WM_INVALIDATE_VIEW, 0, 0);
}


// -----------------------------------------------------------------------------

// Returns the C_overlay with the given file specification, if it is
// already opened.  Otherwise it returns NULL.
C_overlay *C_ovl_mgr::is_file_open(const char *file_specification, GUID overlayDescGuid)
{
   POSITION position;

   find_by_specification(file_specification, overlayDescGuid, position);
   if (position)
   {
      return m_overlay_list.GetAt(position);
   }

   return NULL;
}

// -----------------------------------------------------------------------------

// This function should be called from the static open member of all file
// overlay classes.  It will remove the duplicate from the overlay list
// and return it, if it is found.  It returns NULL otherwise.
C_overlay *C_ovl_mgr::remove_duplicate_file_overlay(C_overlay *overlay)
{
   POSITION position;
   POSITION next;
   C_overlay *next_overlay;

   OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
   if (pFvOverlayPersistence == NULL)
      return NULL;

   _bstr_t fileSpecification;
   pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

   // file specification of newly open file overlay
   CString specification = (char *)fileSpecification;

   // search the overlay list by for a duplicate overlay
   if (!m_overlay_list.IsEmpty())
   {
      next = m_overlay_list.GetHeadPosition();
      do
      {
         // save this position, in case it is the position of the duplicate
         position = next;

         // get the overlay at next and the position of the next one
         next_overlay = m_overlay_list.GetNext(next);

         OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(next_overlay);
         if (pFvOverlayPersistence != NULL)
         {
            _bstr_t fileSpecification;
            pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
            CString duplicateFileSpec = (char *)fileSpecification;

            // compare file specifications
            if (specification.CompareNoCase(duplicateFileSpec) == 0)
            {
               // if a duplicate was found
               if (next_overlay->get_m_overlayDescGuid() == overlay->get_m_overlayDescGuid() )
               {
                  delete_overlay(next_overlay);

                  // if the tabular editor dialog is opened
                  if (g_tabular_editor_dlg != NULL)
                  {
                     // and the current overlay opened in the dialog is overlay being deleted, then
                     // close the tabular editor dialog
                     TabularEditorDataObject* pCrntDataObject = g_tabular_editor_dlg->get_data_object();
                     if (pCrntDataObject != NULL && pCrntDataObject->get_parent_overlay() == next_overlay)
                        CTabularEditorDlg::destroy_dialog();
                  }

                  return next_overlay;
               }
               else
                  ERR_report("Overlays of different kind with the same "
                  "file specification.");
            }
         }

      } while (next);
   }

   return NULL;
}


// -----------------------------------------------------------------------------

// This function is called with the file overlay pointer returned by the
// remove_duplicate_file_overlay, assuming it isn't NULL.  It will return
// TRUE if the user chose to revert back to the saved version of duplicate.
// To revert back you must delete duplicate, which has already been removed
// from the overlay manager's list by the call to 
// remove_duplicate_file_overlay, and proceed as usual through your open
// function.  When revert is not choosen, you must delete the newly created
// overlay and return duplicate.
boolean_t C_ovl_mgr::test_for_revert(C_overlay *duplicate)
{
   CString msg;
   int result;

   // Ask the user if they want to revert back to saved version of this
   // overlay.  No is the default button.
   msg.Format("Do you want to revert to saved '%s'?", 
      OVL_get_overlay_manager()->GetOverlayDisplayName(duplicate));
   result = AfxMessageBox(msg, MB_YESNO | MB_DEFBUTTON2);

   return (result == IDYES);  // TRUE -> revert
}


// -----------------------------------------------------------------------------

// Open or re-open the given file overlay.  If the overlay is already open,
// it will become the current overlay.  If it is not opened it will be.  If
// it returns SUCCESS the specified overlay is now the current overlay.
int C_ovl_mgr::OpenFileOverlay(GUID overlayDescGuid, const char *file_specification, C_overlay *&ret_overlay)
{
   C_overlay* overlay = NULL;
   ret_overlay = NULL;

   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc == NULL)
   {
      ERR_report("No overlay type was found with the given guid");
      return FAILURE;
   }

   // First, determine if the file that is being opened has already been subscribed to
   //
   _bstr_t bstrUNCPath;
   _bstr_t bstrLocalPath( file_specification );
   LONG hOverlay = -1;
   VARIANT_BOOL bExists = VARIANT_FALSE;
   VARIANT_BOOL bReadOnly = VARIANT_FALSE;
   VARIANT_BOOL bSubscribed = VARIANT_FALSE;

   CMainFrame* frame = fvw_get_frame();
   if (frame && frame->m_pCollaborationServer && bstrLocalPath.length() != 0 )
   {
      HRESULT hr = frame->m_pCollaborationServer->SubscribeToExisting(
         bstrLocalPath, bstrUNCPath.GetAddress(), 
         &hOverlay, &bExists, &bReadOnly, &bSubscribed );
   }

   // If we are already subscribed to the overlay with the given file specification, then set the existing overlay's
   // specification (why?), and set the read-only flag on the overlay if necessary
   if ( bSubscribed == VARIANT_TRUE ) 
   {
      overlay = lookup_overlay( hOverlay );

      ASSERT( overlay );
      CFvOverlayPersistenceImpl *pOvlPersistence = dynamic_cast<CFvOverlayPersistenceImpl *>(overlay);
      if ( overlay && pOvlPersistence != NULL) 
      {
         pOvlPersistence->set_specification( (char*)bstrLocalPath );

         // set the read only flag from the collaboration
         OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
         if (pFvOverlayPersistence != NULL)
            pFvOverlayPersistence->put_m_bIsReadOnly(bReadOnly == VARIANT_TRUE);
      }

      return SUCCESS;
   }

   // Otherwise, either the collaboration did not exist or we did not subscribe to it

   // Check for a duplicate overlay with the same name
   //
   C_overlay *pExistingDuplicate = find_by_specification(file_specification, overlayDescGuid);
   if (pExistingDuplicate != NULL)
   {
      // if the user wants to reopen the file and load it up from scratch then we should 
      // delete the duplicate overlay and reload it below
      if (test_for_revert(pExistingDuplicate) == TRUE)
      {
         OverlayPersistence_Interface *overlay_persistence = dynamic_cast<OverlayPersistence_Interface *>(pExistingDuplicate);
         if (overlay_persistence != nullptr &&
            overlay_persistence->SupportsRevertOverride() )
         {
            return overlay_persistence->
               Revert(_bstr_t(file_specification)) == S_OK ? SUCCESS : FAILURE;
         }
         else
         {
            boolean_t cancel = FALSE;
            pExistingDuplicate->pre_close(&cancel);
            if (cancel == TRUE)
            {
               ret_overlay = pExistingDuplicate;
               return SUCCESS;
            }

            delete_overlay(pExistingDuplicate);
            pExistingDuplicate->Release();
         }
      }
      // otherwise, there is nothing to do
      else
      {
         ret_overlay = pExistingDuplicate;
         return SUCCESS;
      }
   }

   if (pOverlayTypeDesc->pOverlayFactory->CreateOverlayInstance(&overlay) == S_OK && overlay != NULL)
   {
      // to support legacy plug-in overlays which use an overlay handle, we need to add the overlay to the 
      // overlay map (which associates an overlay handle with the overlay) before calling Initialize
      const int nOverlayHandle = m_next_overlay_handle++;
      m_overlay_map.SetAt(nOverlayHandle, overlay);

      if (overlay->InternalInitialize(pOverlayTypeDesc->overlayDescriptorGuid) != S_OK)
      {
         ERR_report("C_ovl_mgr::OpenFileOverlay - Initialization of overlay failed");
         m_overlay_map.RemoveKey(nOverlayHandle);
         overlay->Release();
         return SUCCESS;
      }

      // verify that the overlay set its overlay descriptor guid in Initialize.  The reason we don't just expose a
      // propget for the overlay descriptor guid and set it ourselves is that we want the property to be read-only
      if (overlay->get_m_overlayDescGuid() != pOverlayTypeDesc->overlayDescriptorGuid)
      {
         ERR_report("C_ovl_mgr::OpenFileOverlay - the overlay descriptor guid property was not set in Initialize()");
         m_overlay_map.RemoveKey(nOverlayHandle);
         overlay->Release();
         return FAILURE;
      }

      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence == NULL)
      {
         ERR_report("C_ovl_mgr::OpenFileOverlay - file overlays must implement the IFvOverlayPersistence interface");
         m_overlay_map.RemoveKey(nOverlayHandle);
         overlay->Release();
         return FAILURE;
      }

      // The reason we set m_bHasBeenSaved, m_bIsDirty, and m_bIsReadOnly before calling FileOpen is that it
      // gives FileOpen a chance to modify these if necesssary and at the same time sets the defaults
      // for typical overlays.

      pFvOverlayPersistence->put_m_bHasBeenSaved(TRUE);
      pFvOverlayPersistence->put_m_bIsDirty(FALSE);

      // Set the file read only if it was in a collaboration
      if ( bExists == VARIANT_TRUE )
      {
         pFvOverlayPersistence->put_m_bIsReadOnly(TRUE);
      }
      else
      { 
         // Set the read-only flag (m_read_only) to indicate if the file is/is not a read-only file
         // if the user has write access to the current file, then it is not a read-only file...
         if (FIL_access(file_specification, FIL_WRITE_OK) == SUCCESS)
         {
            // Since this is not a read-only file, set the m_read_only flag to FALSE
            // for this instance of the C_Overlay class...
            pFvOverlayPersistence->put_m_bIsReadOnly(FALSE);
         }
         // we know that the file being opened is read-only...
         else if (FIL_access(file_specification, FIL_EXISTS) == SUCCESS)
         {
            // Set the m_read_only flag to TRUE for this instance of the C_Overlay class
            // to indicate that the file is read-only...
            pFvOverlayPersistence->put_m_bIsReadOnly(TRUE);
         }
      }

      // The overlay should be added to the stack before calling FileOpen to 
      // insure that overlays using the ILayer automation interface will work
      // with the given overlay handle (we will only do this for legacy plug-ins)
      const bool isLegacyPlugIn = pOverlayTypeDesc != NULL &&
         (dynamic_cast<CLayerOvlFactory *>(pOverlayTypeDesc->pOverlayFactory) != NULL);
      if (isLegacyPlugIn && AddOverlayToStack(overlay, overlayDescGuid) != SUCCESS)
      {
         ERR_report("C_ovl_mgr::OpenFileOverlay - unable to add overlay to stack");
         return FAILURE;
      }

      if (pFvOverlayPersistence->FileOpen(_bstr_t(file_specification)) != S_OK)
      {
         ERR_report("C_ovl_mgr::OpenFileOverlay - FileOpen failed");
         m_overlay_map.RemoveKey(nOverlayHandle);

         // The Points overlay, which if the file cannot be found, will open the overlay 
         // options dialog.  The property page allows the user to open / close Point 
         // overlays which can have the side affect of setting the current overlay to 
         // the one we are about to delete.  The current overlay should not be set to an
         // overlay that is in the process of being opened.
         if (m_current == overlay)
         {
            m_current = NULL;
            set_mode(GUID_NULL, FALSE);
         }

         if (isLegacyPlugIn && RemoveOverlayFromStack(overlay) == SUCCESS)
            overlay->Release();
         return FAILURE;
      }

      // Add the overlay to the stack if it hasn't already been added
      if (!isLegacyPlugIn && AddOverlayToStack(overlay, overlayDescGuid) != SUCCESS)
      {
         ERR_report("C_ovl_mgr::OpenFileOverlay - unable to add overlay to stack");
         return FAILURE;
      }

      // verify that the file specification has been set in FileOpen.  The property is read-only
      _bstr_t fileSpecification;
      if (pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress()) != S_OK ||
         fileSpecification.length() == 0)
      {
         ERR_report("C_ovl_mgr::OpenFileOverlay - the file specification, m_fileSpecification, must be set in FileOpen");
         m_overlay_map.RemoveKey(nOverlayHandle);
         RemoveOverlayFromStack(overlay);
         overlay->Release();
         return FAILURE;
      }

      // Initialize the map/screen display elements
      overlay->InitializeDisplayElementRootNode();

      // if the currently-selected file IS a FalconView-registered file type, set the value of the 
      // most_recent_directory key in the registry to the path of this file...
      overlay_type_util::set_most_recent_directory(pOverlayTypeDesc->overlayDescriptorGuid, file_specification);

      FVW_add_to_recent_file_list(overlayDescGuid, file_specification);     

      // invalidate the display so everything is redrawn
      invalidate_all();
      
      OnOverlayAdded(overlay, TRUE);

      ret_overlay = overlay;
   }
   else
   {
      ERR_report("Unable to create instance of overlay");
      return FAILURE;
   }

   return SUCCESS;
}

// Provides the sort comparison function for sorting the Overlay->View menu
int menu_data_compare(const void *arg1, const void *arg2)
{
   VIEW_MENU_DATA* item1 = (VIEW_MENU_DATA*)arg1;
   VIEW_MENU_DATA* item2 = (VIEW_MENU_DATA*)arg2;

   CString item1_str(item1->display_title);
   CString item2_str(item2->display_title);

   return item1_str.CompareNoCase(item2_str);
}

// -----------------------------------------------------------------------------

// This function is called to add menu items to pMenu for the Overlay
// Manager... This is used for the the popup menu handlers
void C_ovl_mgr::append_overlay_menu(CMenu* pMenu, boolean_t open_items)
{
   C_overlay* overlay;
   int i = 0;
   UINT flags;
   UINT menu_id;

   int first_file_item;
   int first_loaded_item;
   int index = 0;
   int static_count=0;
   int file_count;
   int loaded_count=0;
   const int INDEX_STR_LEN = 10;
   char index_str[INDEX_STR_LEN];
   CString current_filename;
   CString value;
   char* buffer;
   char* lpszOverlayDescGuid;
   char* file_string;
   char* display_title;
   boolean_t is_open;
   CString display_string;

   // free up the previous menu_data array, if it exists
   free_view_menu_data();

   if (!open_items)
   {
      VIEW_MENU_DATA data;
      m_view_menu_data.push_back(data);

      // first two items duplicate the File menu's New and Open
      m_view_menu_data[index].display_title = "New...";
      index++;

      m_view_menu_data.push_back(data);

      m_view_menu_data[index].display_title = "Open...";
      index++;


      // now, loop through loop through all static overlay factories and add those
      OVL_get_type_descriptor_list()->ResetEnumerator();
      while ( OVL_get_type_descriptor_list()->MoveNext() )
      {
         OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;

         // figure out if this is a static overlay, and if so add it
         if (!pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
         {
            // we skip static overlays with editors by convention
            if (pOverlayTypeDesc->pOverlayEditor != NULL)
               continue;

            // the factory will be omitted if the request string is empty
            if (pOverlayTypeDesc->displayName.IsEmpty())
               continue;

            is_open = get_first_of_type(pOverlayTypeDesc->overlayDescriptorGuid) == NULL ? 0 : 1;

            if (!is_open)
            {
               display_string = "Open ";
               display_string += pOverlayTypeDesc->displayName;

               VIEW_MENU_DATA data;
               m_view_menu_data.push_back(data);

               // we don't need to allocate memory here because class_info is static
               m_view_menu_data[index].display_title = display_string;
               m_view_menu_data[index].pOverlayTypeDesc = pOverlayTypeDesc;
               index++;

               // bump our count of the static items on the menu
               static_count++;
            }
         }
      }

      // between the last static and the first file, we'll put a separator line
      first_file_item = index;

      // get the number of files that we're going to look for
      file_count = PRM_get_registry_int("Overlay Manager\\Menu Files2", "File Count", 0);

      // loop through all files in the Menu Files section of the registry,
      for (i = 0; i < file_count; i++)
      {
         _itoa_s(i, index_str, INDEX_STR_LEN, 10);
         value = PRM_get_registry_string("Overlay Manager\\Menu Files2", index_str, "NONE");

         // strtok needs a char * that it can write to
         buffer = value.GetBuffer(value.GetLength() + 1);

         // grab the tokenized parts of the value string
         char* context;
         TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
         VERIFY(display_title = strtok_s(buffer, ",", &context));
         VERIFY(lpszOverlayDescGuid = strtok_s(NULL, ",", &context));
         VERIFY(file_string = strtok_s(NULL, ",", &context));

         string_utils::CGuidStringConverter guidConv(lpszOverlayDescGuid);

         is_open = is_file_open(file_string, guidConv.GetGuid()) ? 1 : 0;
         if (!is_open)
         {
            display_string = "Open ";
            display_string += display_title;

            VIEW_MENU_DATA data;
            m_view_menu_data.push_back(data);

            m_view_menu_data[index].display_title = display_string;
            m_view_menu_data[index].pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(guidConv.GetGuid());
            m_view_menu_data[index].file_string = file_string;

            index++;
         }

         // done with buffer
         value.ReleaseBuffer(-1);
      }

      // File Overlay menu items are only used if the overlay is not already
      // opened.  At the start of the above loop index = 2 + static_count.  
      // In the loop it is increased once for each menu item added.
      file_count = index - static_count - 2;

      // sort each group of overlays by display_title (static, then files)
      if (static_count > 0)
      {
         qsort((void*)&m_view_menu_data[2], static_count,
            sizeof(VIEW_MENU_DATA), menu_data_compare);
      }
      if (file_count > 0)
      {
         qsort((void*)&m_view_menu_data[first_file_item], file_count,
            sizeof(VIEW_MENU_DATA), menu_data_compare);
      }

      // now, loop through the menu data and append the menu items
      for (i = 0; i < index; i++)
      {
         // if this is where we want the separator, then add it
         if (i == first_file_item &&  file_count > 0)
            pMenu->AppendMenu(MF_SEPARATOR);

         // we're passing in a string, so flag that
         flags = MF_STRING | MF_BYCOMMAND;

         // insert the overlay menu item
         menu_id = IDRANGE_OVERLAY_VIEW_MENU_1 + i;
         pMenu->AppendMenu(flags, menu_id, m_view_menu_data[i].display_title);
      }
   }
   else
   {
      // now, loop through loop through all static overlay factories and add those
      OVL_get_type_descriptor_list()->ResetEnumerator();
      while ( OVL_get_type_descriptor_list()->MoveNext() )
      {
         OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;

         // figure out if this is a static overlay, and if so add it
         if (!pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
         {
            // the factory will be omitted if the request string is empty
            if (pOverlayTypeDesc->displayName.IsEmpty())
               continue;

            is_open = get_first_of_type(pOverlayTypeDesc->overlayDescriptorGuid) == NULL ? 0 : 1;

            // only allow regular and user controllable top most overlays
            if (is_open && (!pOverlayTypeDesc->is_top_most || 
               (pOverlayTypeDesc->is_top_most) && pOverlayTypeDesc->is_user_controllable))
            {
               display_string = "Close ";
               display_string += pOverlayTypeDesc->displayName;

               VIEW_MENU_DATA data;
               m_view_menu_data.push_back(data);

               // we don't need to allocate memory here because class_info is static
               m_view_menu_data[index].display_title = display_string;
               m_view_menu_data[index].pOverlayTypeDesc = pOverlayTypeDesc;
               index++;

               // bump our count of the static items on the menu
               static_count++;
            }
         }
      }

      // between the last static and the first file, we'll put a separator line
      first_loaded_item = index;

      // now loop through all other open file overlays, adding any that
      // are not present on the menu already... these get added at the bottom
      // with checkmarks, by definition
      overlay = get_first();
      while (overlay)
      {
         // skip menu insertion if FvOverlayTypeOverrides 
         // UserControllable(False) or is_user_controllable(false) 
         // do not add to menu.
         if (overlay != nullptr)
         {
            long userControllable = TRUE;
            // check the interface overrides implemented at run time.
            overlay->get_m_UserControllable(&userControllable); 
            if (userControllable == 0)  
            {
               overlay = get_next(overlay);
               continue;
            }
            OverlayTypeDescriptor* pOverlayTypeDesc = nullptr;
            pOverlayTypeDesc = OVL_get_type_descriptor_list()->
               GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
            if (pOverlayTypeDesc != nullptr)
            {
               if (!pOverlayTypeDesc->is_user_controllable)
               {
                  overlay = get_next(overlay);
                  continue;
               }
            }
         }
         // if the overlay is a file overlay
         OverlayPersistence_Interface *pFvOverlayPersistence = 
            dynamic_cast<OverlayPersistence_Interface *>(overlay);
         if (pFvOverlayPersistence != NULL)
         {
            _bstr_t fileSpecification;
            pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

            current_filename = GetOverlayDisplayName(overlay);
            // increment our count of "loaded overlays" -- those that are
            // only here in the menu
            loaded_count++;

            display_string = "Close ";
            display_string += current_filename;

            VIEW_MENU_DATA data;
            m_view_menu_data.push_back(data);

            m_view_menu_data[index].display_title = display_string;
            m_view_menu_data[index].pOverlayTypeDesc = 
               OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
            m_view_menu_data[index].file_string = (char *)fileSpecification;
            index++;
         }

         overlay = get_next(overlay);
      }

      // sort each group of overlays by display_title (static, then files)
      if (static_count > 0)
      {
         qsort((void*)&m_view_menu_data[0], static_count,
            sizeof(VIEW_MENU_DATA), menu_data_compare);
      }

      if (loaded_count > 0)
      {
         qsort((void*)&m_view_menu_data[first_loaded_item],  loaded_count,
            sizeof(VIEW_MENU_DATA), menu_data_compare);
      }

      // now, loop through the menu data and append the menu items
      for (i = 0; i < index; i++)
      {
         // if this is where we want the separator, then add it
         if (first_loaded_item != 0 && i == first_loaded_item && loaded_count > 0)
            pMenu->AppendMenu(MF_SEPARATOR);

         // we're passing in a string, so flag that
         flags = MF_STRING | MF_BYCOMMAND;

         // insert the overlay menu item
         menu_id = IDRANGE_OVERLAY_VIEW_MENU_1 + i;
         pMenu->AppendMenu(flags, menu_id, m_view_menu_data[i].display_title);
      }

      // add the Close All menu item if one or more overlays are opened
      if (index > 0)
      { 
         VIEW_MENU_DATA data;
         m_view_menu_data.push_back(data);

         m_view_menu_data[index].display_title = "Close All";

         pMenu->AppendMenu(MF_SEPARATOR);
         pMenu->AppendMenu(MF_STRING | MF_BYCOMMAND, IDRANGE_OVERLAY_VIEW_MENU_1 + index, 
            "Close All");

         index++;
      }
   }
}

// Process the menu command that was generated by a selection
// from the dynamically generated menu list. Overlay_item
// is the item that was selected.
//
void C_ovl_mgr::handle_overlay_view_select(int menu_item)
{
   CString file_name = m_view_menu_data[menu_item].file_string;
   OverlayTypeDescriptor* pOverlayTypeDesc = m_view_menu_data[menu_item].pOverlayTypeDesc;
   C_overlay* ovl;

   if (m_view_menu_data[menu_item].display_title == "Close All")
   {
      boolean_t cancel;
      close_overlays(m_overlay_list, &cancel);
   }

   else if (m_view_menu_data[menu_item].display_title == "New...")
      fvw_get_app()->OnFileNewFv();

   else if (m_view_menu_data[menu_item].display_title == "Open...")
      fvw_get_app()->OnFileOpen();

   else if (pOverlayTypeDesc != NULL && !pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      toggle_static_overlay(pOverlayTypeDesc->overlayDescriptorGuid);

   else
   {
      if (file_name)
      {
         if (ovl = is_file_open(file_name, pOverlayTypeDesc->overlayDescriptorGuid))
            close(ovl);
         else if (pOverlayTypeDesc != NULL)
         {
            C_overlay *ret_overlay = NULL;

            OpenFileOverlay(pOverlayTypeDesc->overlayDescriptorGuid, file_name, ret_overlay);

            // if a menu file failed to open give the user the opportunity to remove it from the list
            if (ret_overlay == NULL && m_view_menu_data[menu_item].bIsMenuFile)
            {
               /* TODO : fix this to work with Application Menu
               CString msg;
               msg.Format("Unable to open %s.  Would you like to remove this file from the Recent Overlay List?", m_view_menu_data[menu_item].display_title);
               if (AfxMessageBox(msg, MB_YESNO) == IDYES)
               {
               }
               */
            }
         }
      }
      else
         ERR_report("Unable to interpret file name/class name.");
   }
}

// Return the overlay type descriptor associated with a given command ID
OverlayTypeDescriptor* C_ovl_mgr::GetOverlayMenuDesc(UINT nID)
{
   size_t nItem = nID - IDRANGE_OVERLAY_VIEW_MENU_1;
   if (0 <= nItem && nItem < m_view_menu_data.size())
   {
      return m_view_menu_data[nItem].pOverlayTypeDesc;
   }

   return NULL;
}

// On behalf of the CMainFrm class, handles the nUpdateOverlayManagerViewList
// that gets generated by ON_UPDATE_COMMAND_UI_RANGE for the Overlay->View
// menu items that are dynamically created.
int C_ovl_mgr::overlay_view_update(CCmdUI* pCmdUI)
{
   if (m_view_menu_data.size() == 0)
      return SUCCESS;

   int item = pCmdUI->m_nID - IDRANGE_OVERLAY_VIEW_MENU_1;
   int is_open = 0;
   OverlayTypeDescriptor* pOverlayTypeDesc = m_view_menu_data[item].pOverlayTypeDesc;

   if (m_view_menu_data[item].display_title == "New...")
      is_open = FALSE;

   else if (m_view_menu_data[item].display_title == "Open...")
      is_open = FALSE;

   // if the factory is is static overlay factory
   else if (pOverlayTypeDesc != NULL && !pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
   {
      is_open = get_first_of_type(pOverlayTypeDesc->overlayDescriptorGuid) == NULL ? 0 : 1;
   }
   // otherwise, it must be a file overlay factory
   else if (pOverlayTypeDesc != NULL)
      is_open = is_file_open(m_view_menu_data[item].file_string, pOverlayTypeDesc->overlayDescriptorGuid) ? 1 : 0;

   pCmdUI->SetCheck(is_open);

   if (m_show_non_current_ovls_flag)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);

   return SUCCESS;
}

// -----------------------------------------------------------------------------
boolean_t C_ovl_mgr::get_message_string(UINT nID, CString& rMessage)
{
   if (nID >= IDRANGE_OVERLAY_VIEW_MENU_1  &&  nID <= IDRANGE_OVERLAY_VIEW_MENU_1 + 1000)
   {
      rMessage = status_string_for_view_menu(nID - IDRANGE_OVERLAY_VIEW_MENU_1);
      return TRUE;
   }
   else if (nID >= ID_OVERLAY_EDITOR1  &&  nID <= ID_OVERLAY_EDITOR40)
   {
      rMessage = status_string_for_editor(nID);
      return TRUE;
   }
   return FALSE;

}

// -----------------------------------------------------------------------------

// 
// Generates a status bar messege for the given item number on the
// currently displayed Ovleray-view menu.
//
CString C_ovl_mgr::status_string_for_view_menu(int menu_item)
{
   CString status;

   if ( (UINT) menu_item < m_view_menu_data.size() )   // Must have item
   {
      if ( m_view_menu_data[menu_item].file_string.GetLength() > 0)
      {
         status.Format("Toggle the overlay file: %s.", 
            m_view_menu_data[menu_item].file_string);
      }
      else
      {
         status.Format("Toggle the %s overlay.", 
            m_view_menu_data[menu_item].display_title);
      }
   }
   return status;
}

// Remove the first ampersand from a string.
inline CString remove_ampersand(CString s)
{
   s.Remove('&');
   return s;
}

void C_ovl_mgr::BuildEditorRibbonPanel(CMFCRibbonPanel* pEditorsPanel, 
      CMFCRibbonPanel* pMapDataManagerPanel,
      CMFCRibbonCategory* pEditorCategory
      )
{
   ASSERT(pEditorsPanel);

   InitializeEditorList();
   CMFCRibbonPanel* pPanel = pEditorsPanel;
   CString activeParentName = "null";  // cannot be empty string for proper panel creation
   CMFCRibbonPanel* pRestorePanel = NULL;

   for (size_t i=0; i<m_editor_list.size(); i++)
   {
      const int current_id = m_editor_list[i].m_id;
      CString current_editor = m_editor_list[i].m_editor_name;

      OverlayTypeDescriptor *pdesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(
         m_editor_list[i].m_overlayDescGuid);

      if (pdesc)
      {
         CString editorName = remove_ampersand(current_editor);

         int pos = editorName.Find('\t');
         if (pos != -1)
            editorName = editorName.Mid(0, pos);

         // The Map Data Manager "editor" makes more sense on the Data
         // Administration tab rather than the editor tab.
         CMFCRibbonButton* btn;
         if (pdesc->overlayDescriptorGuid == FVWID_Overlay_MapDataManager)
         {
            btn = new CMFCRibbonButton(
               m_editor_list[i].m_id, editorName, 23, 23);
            pRestorePanel = pPanel;
            pPanel = pMapDataManagerPanel;
         }
         else if (pdesc->overlayDescriptorGuid == FVWID_Overlay_SceneManager)
         {
            CMainFrame *frame = fvw_get_frame();
            if (frame->Is3DEnabledInRegistry())
            {           
               btn = new CMFCRibbonButton(
                  m_editor_list[i].m_id, editorName, 87, 87);
               pRestorePanel = pPanel;
               pPanel = pMapDataManagerPanel;
            }
            else 
               continue;    // Don't add button.
         }
         else
         {
            if (pPanel == pMapDataManagerPanel)
               pPanel = pRestorePanel;

            CString parentName = pdesc->parentDisplayName;
            if (parentName != activeParentName) 
            {
               if (parentName == ((LPCTSTR) "")) 
               {
                  pPanel = pEditorsPanel;
                  activeParentName = (LPCTSTR) "";
               }
               else 
               {
                  pPanel = pEditorCategory->AddPanel((LPCTSTR)parentName); // reached a new parent category, create a new panel
                  activeParentName = parentName;
               }
            }

            btn = new CMFCRibbonButton(
               m_editor_list[i].m_id,  editorName, 
               pdesc->pIconImage->get_icon(32), FALSE,
               pdesc->pIconImage->get_icon(16));
           
         }

         btn->SetToolTipText(editorName);

         CString desc;
         desc.Format("Toggle %s", editorName);
         btn->SetDescription(desc);

         pPanel->Add(btn);
      }
   }
}

// -----------------------------------------------------------------------------
void C_ovl_mgr::toggle_editor_by_command_id(UINT nCmdId)
{
   auto it = std::find_if(m_editor_list.begin(), m_editor_list.end(),
      [=] (const EditorEntry& e) {
         return e.m_id == nCmdId;
   });
   if (it != m_editor_list.end())
      toggle_editor(it->m_overlayDescGuid);
}

void C_ovl_mgr::toggle_editor(GUID overlayDescGuid)
{
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc != NULL && pOverlayTypeDesc->pOverlayEditor != NULL)
   {
      // toggle the editor
      if (m_pCurrentEditor == pOverlayTypeDesc->pOverlayEditor)
         set_mode(GUID_NULL);
      else
         set_mode(pOverlayTypeDesc->overlayDescriptorGuid);
   }
}

CString C_ovl_mgr::status_string_for_editor(UINT nCmdId)
{
   for (size_t i=0; i < m_editor_list.size(); ++i)
   {
      if (m_editor_list[i].m_id == nCmdId)
      {
         OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(m_editor_list[i].m_overlayDescGuid);
         if (pOverlayTypeDesc->pOverlayEditor != NULL)
            return CString("Toggle ") + remove_ampersand(pOverlayTypeDesc->editorDisplayName);
      }
   }

   return "";
}

// Update the state of the given overlay editor control
void C_ovl_mgr::OnUpdateOverlayEditor(CCmdUI* pCmdUI)
{
   const UINT nID = pCmdUI->m_nID;
   auto it = std::find_if(m_editor_list.begin(), m_editor_list.end(), 
      [=](const EditorEntry& e) {
         return e.m_id == nID;
   }
   );

   if (it != m_editor_list.end())
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->
         GetOverlayTypeDescriptor(it->m_overlayDescGuid);
      pCmdUI->SetCheck(pOverlayTypeDesc->pOverlayEditor == m_pCurrentEditor);
   }
}

void C_ovl_mgr::InitializeEditorList()
{
   // enumerate through each overlay type descriptor.  If the overlay type
   // implements an editor, then add the editor's button to the editor toggle
   // toolbar along with an entry in the editor menu list
   int nCmdId = ID_OVERLAY_EDITOR1;
   OVL_get_type_descriptor_list()->ResetEnumerator();
   while (OVL_get_type_descriptor_list()->MoveNext())
   {
      OverlayTypeDescriptor *pOverlayTypeDesc =
         OVL_get_type_descriptor_list()->m_pCurrent;
      if (pOverlayTypeDesc->pOverlayEditor != NULL)
      {
         if (nCmdId > ID_OVERLAY_EDITOR40)
         {
            ERR_report("Exceeded maximum editors allowed.  Editor disabled:  " + pOverlayTypeDesc->editorDisplayName);
         }
         else
         {
            // add an entry to the editor menu list (used to populate the Tools
            // menu)
            EditorEntry entry;
            entry.m_editor_name = pOverlayTypeDesc->editorDisplayName;
            entry.m_stripped = entry.m_editor_name;
            entry.m_stripped.Remove('&');
            entry.m_id = nCmdId++;
            entry.m_overlayDescGuid = pOverlayTypeDesc->overlayDescriptorGuid;

            m_editor_list.push_back(entry);
         }
      }
   }

   // sort the tools in the list. The Mission editor should come first
   std::sort(m_editor_list.begin(), m_editor_list.end(),
      [](const EditorEntry& a, const EditorEntry& b) -> bool
   {
      if (a.m_overlayDescGuid == FVWID_Overlay_Route)
         return true;
      else if (b.m_overlayDescGuid == FVWID_Overlay_Route)
         return false;

      return a.m_stripped < b.m_stripped;
   });
}

// Handle the "Show all overlays" command from the Overlay menu.
// We need to do two things: (1) make sure that that hide background option
// is turned off, and (2) toggle on every hidden overlay.
void C_ovl_mgr::show_all_overlays() 
{
   POSITION next;
   C_overlay* overlay;

   // make sure we can see the background overlays
   m_show_non_current_ovls_flag = TRUE;

   next = m_overlay_list.GetHeadPosition();
   while (next)
   {
      // get this overlay and the position of the next one
      overlay = (C_overlay *)m_overlay_list.GetNext(next);

      // turn off the hidden flag on all overlays
      overlay->put_m_bIsOverlayVisible(TRUE);
   }

   // re-display with the overlay toggled
   invalidate_all(FALSE);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Frees the memory used to store the Overlay->View menu item information.
void C_ovl_mgr::free_view_menu_data()
{
   m_view_menu_data.erase(m_view_menu_data.begin(), m_view_menu_data.end());
}

// Tests for the existance of a given file spec in the Overlay->View registry
// files.
boolean_t C_ovl_mgr::test_file_menu(const char* file_spec)
{
   int i;
   int file_count;
   int reg_item = INT_MAX;
   const int INDEX_STR_LEN = 10;
   char index_str[INDEX_STR_LEN];
   CString value;
   char* buffer;
   char* display_title;
   char* lpszOverlayDescGuid;
   char* file_string;

   // read the number of entries already in the registry
   file_count = PRM_get_registry_int("Overlay Manager\\Menu Files2", "File Count", 0);

   // loop through each entry looking for a match
   for (i = 0; i < file_count; i++)
   {
      // read the current entry
      _itoa_s(i, index_str, INDEX_STR_LEN, 10);
      value = PRM_get_registry_string("Overlay Manager\\Menu Files2", index_str, "NONE");

      // strtok needs a char * that it can write to
      buffer = value.GetBuffer(value.GetLength() + 1);

      // tokenize the value 
      char* context;
      VERIFY(display_title = strtok_s(buffer, ",", &context));
      VERIFY(lpszOverlayDescGuid = strtok_s(NULL, ",", &context));
      VERIFY(file_string = strtok_s(NULL, ",", &context));

      // see if we have a match... if so, save the current position
      if (_stricmp(file_string, file_spec) == 0)
      {
         value.ReleaseBuffer(-1);
         return TRUE;
      }

      // done with buffer
      value.ReleaseBuffer(-1);
   }

   return FALSE;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Receives all keydown messeges from MapView and passes them to
// the current editable overlay... a return of false will allow the
// keystroke to be processed normally in MapView.
boolean_t C_ovl_mgr::offer_keydown(FalconViewOverlayLib::IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
   UINT nFlags)
{
   // get the mouse pointer position to offer to the overlay
   CPoint mouse_pt;

   // get the current mouse position
   if (!GetCursorPos(&mouse_pt))
      return FALSE;

   // convert that position to view coordinates
   fvw_get_view()->ScreenToClient(&mouse_pt);

   // if an overlay has selection locked than it should also be allowed to process the keystrokes first.  We
   // need to redesign the routing of user interface events to allow overlays to change their priority order
   // for receiving events independent of the display order
   POSITION position = m_overlay_list.GetHeadPosition();
   while (position)
   {
      C_overlay *pOverlay = m_overlay_list.GetNext(position);

      OverlayUIEventRoutingOverrides_Interface *pUIEventRouting = dynamic_cast<OverlayUIEventRoutingOverrides_Interface *>(pOverlay);
      if (pUIEventRouting == NULL)
         continue;

      if ( pUIEventRouting->get_m_bDirectlyRouteKeyDown() )
      {
         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(pOverlay);
         if (pUIEvents != NULL)
         {
            long bHandled = 0;
            pUIEvents->KeyDown(pMapView, mouse_pt.x, mouse_pt.y, nChar, nRepCnt, nFlags, &bHandled);
            if (bHandled)
               return TRUE;
         }
      }
   }

   OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(m_current);
   if (pUIEvents != NULL)
   {
      long bHandled = 0;
      pUIEvents->KeyDown(pMapView, mouse_pt.x, mouse_pt.y, nChar, nRepCnt, nFlags, &bHandled);
      return bHandled;
   }

   return FALSE;
}

// Receives all keyup messeges from MapView and passes them to
// the current editable overlay... a return of false will allow the
// keystroke to be processed normally in MapView.
boolean_t C_ovl_mgr::offer_keyup(FalconViewOverlayLib::IFvMapView *pMapView, UINT nChar, UINT nRepCnt, 
   UINT nFlags)
{
   // get the mouse pointer position to offer to the overlay
   CPoint mouse_pt;

   // get the current mouse position
   if (!GetCursorPos(&mouse_pt))
      return FALSE;

   // convert that position to view coordinates
   fvw_get_view()->ScreenToClient(&mouse_pt);

   // if an overlay has selection locked than it should also be allowed to process the keystrokes first.  We
   // need to redesign the routing of user interface events to allow overlays to change their priority order
   // for receiving events independent of the display order
   POSITION position = m_overlay_list.GetHeadPosition();
   while (position)
   {
      C_overlay *pOverlay = m_overlay_list.GetNext(position);

      OverlayUIEventRoutingOverrides_Interface *pUIEventRouting = dynamic_cast<OverlayUIEventRoutingOverrides_Interface *>(pOverlay);
      if (pUIEventRouting == NULL)
         continue;

      if ( pUIEventRouting->get_m_bDirectlyRouteKeyUp() )
      {
         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(pOverlay);
         if (pUIEvents != NULL)
         {
            long bHandled = 0;
            pUIEvents->KeyUp(pMapView, mouse_pt.x, mouse_pt.y, nChar, nRepCnt, nFlags, &bHandled);
            if (bHandled)
               return TRUE;
         }
      }
   }

   OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(m_current);
   if (pUIEvents != NULL)
   {
      long bHandled = 0;
      pUIEvents->KeyUp(pMapView, mouse_pt.x, mouse_pt.y, nChar, nRepCnt, nFlags, &bHandled);
      return bHandled;
   }

   return FALSE;
}


// Receives all mousewheel messages from MapView and passes them to
// the current overlay... a return of false will allow the
// message to be processed normally in MapView.b
boolean_t C_ovl_mgr::offer_mousewheel( FalconViewOverlayLib::IFvMapView *pMapView, SHORT zDelta, UINT nFlags )
{
   if ( m_current )
   {
      // Get the mouse pointer position to offer to the overlay
      CPoint ptMouse;

      // et the current mouse position
      if ( GetCursorPos( &ptMouse ) )
      {

         // Convert that position to view coordinates
         fvw_get_view()->ScreenToClient( &ptMouse );

         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(m_current);
         if (pUIEvents != NULL)
         {
            long bHandled = FALSE;
            pUIEvents->MouseWheel(pMapView, ptMouse.x, ptMouse.y, zDelta, nFlags, &bHandled);
            return bHandled;
         }
      }
   }
   return FALSE;
};


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Finds the top-most icon and passes the double-click on to that overlay,
// returning TRUE to let the caller know that the double-click was handled
// by someone.  If it finds no icon, it returns FALSE.
BOOL C_ovl_mgr::on_dbl_click(FalconViewOverlayLib::IFvMapView *pMapView, UINT flags, CPoint point)
{
   // if we are showing all overlays, then test for double-click on all overlays
   if (m_show_non_current_ovls_flag)
   {
      // test from the top-down
      POSITION pos = m_overlay_list.GetHeadPosition();
      while (pos != NULL)
      {
         C_overlay *overlay = m_overlay_list.GetNext(pos);

         // skip this overlay if it is not visible
         if (!overlay->get_m_bIsOverlayVisible())
            continue;

         OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(overlay);
         if (pUIEvents == NULL)
            continue;

         long bHandled = FALSE;
         pUIEvents->MouseDoubleClick(pMapView, point.x, point.y, flags, &bHandled);
         if (bHandled)
            return TRUE;
      }
   }
   else if (m_current->get_m_bIsOverlayVisible())
   {
      OverlayUIEvents_Interface *pUIEvents = dynamic_cast<OverlayUIEvents_Interface *>(m_current);
      if (pUIEvents != NULL)
      {
         long bHandled = FALSE;
         pUIEvents->MouseDoubleClick(pMapView, point.x, point.y, flags, &bHandled);
         return bHandled;
      }
   }

   return FALSE;
}

boolean_t C_ovl_mgr::OverrideHelp(UINT nCmdId, UINT nCmd)
{
   CMainFrame* pFrame = fvw_get_frame();
   if (pFrame == NULL)
      return FALSE;

   if (nCmd == HELP_CONTEXT)
   {
      if (nCmdId >= ID_OVERLAY_EDITOR1 && nCmdId <= ID_OVERLAY_EDITOR40)
      {  
         for (size_t i=0; i < m_editor_list.size(); ++i)
         {
            if (m_editor_list[i].m_id == nCmdId)
            {
               OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(m_editor_list[i].m_overlayDescGuid);
               if (pOverlayTypeDesc != NULL)
               {
                  pFrame->LaunchHtmlHelp( pOverlayTypeDesc->contextSensitiveHelp.editorHelpId, HELP_CONTEXT,
                     pOverlayTypeDesc->contextSensitiveHelp.helpFileName, pOverlayTypeDesc->contextSensitiveHelp.editorHelpURI);
               }
            }
         }

         return TRUE;
      }
   }

   return FALSE;
}

// This function should step through each overlay ignoring those that are not
// time-sensitive to get min and max timestamps
int C_ovl_mgr::get_time_span( COleDateTime& start, COleDateTime& stop )
{
   // store start, stop time passed in in case we need to restore them
   COleDateTime old_start = start;
   COleDateTime old_stop = stop;

   COleDateTime current_time;
   CTime time = CTime::GetCurrentTime();
   tm t;
   time.GetGmtTm(&t);
   current_time.SetDateTime(1900 + t.tm_year, t.tm_mon + 1, t.tm_mday, t.tm_hour, 
      t.tm_min, t.tm_sec);

   start = stop = current_time;

   COleDateTime t0,t1;

   bool init=true;
   for(C_overlay *overlay = get_first(); overlay!=NULL; 
      overlay = get_next(overlay))
   {
      PlaybackTimeSegment_Interface *pTimeSegment = dynamic_cast<PlaybackTimeSegment_Interface *>(overlay);
      if (pTimeSegment == NULL || pTimeSegment->SupportsFvPlaybackTimeSegment() == FALSE)
         continue;

      // query the overlay for start and stop times

      DATE begin, end;
      long bTimeSegmentValid;
      pTimeSegment->GetTimeSegment(&begin, &end, &bTimeSegmentValid);

      if (bTimeSegmentValid)
      {
         t0 = begin;
         t1 = end;

         if (t0.GetStatus() == COleDateTime::valid &&
            t1.GetStatus() == COleDateTime::valid)
         {

            // if this is the first time through the loop initialize the start
            // and stop times
            if (init)
            {
               start = t0;
               stop = t1;

               init = false;
            }
            else
            {
               // set the starting time to t0 if t0 is before the current start time
               if (t0 < start)
                  start = t0;

               // set the ending time to t1 if t1 is after the current stop time
               if (t1 > stop)
                  stop = t1;
            }
         }
      }
   }

   // if we don't have a non-zero interval then say so
   if (stop - start == COleDateTimeSpan(0,0,0,0))
   {
      // values shouldn't be changed if we return FAILURE, restore them here
      stop = old_stop;
      start = old_start;

      return FAILURE;
   }

   // all is well
   return SUCCESS;
}

// This function will tell each time-sensitive overlay about the new time each
// overlay will then initiate invalidations and redraws as necessary
void C_ovl_mgr::set_current_view_time( COleDateTime t )
{
   for(C_overlay *overlay = get_first(); overlay!=NULL; 
      overlay = get_next(overlay))
   {
      PlaybackEventsObserver_Interface *pObserver = dynamic_cast<PlaybackEventsObserver_Interface *>(overlay);
      if (pObserver != NULL)
         pObserver->OnPlaybackTimeChanged(t);
   }

   // reset flag to indicate that an overlay set the altitude in UpdateAltitude
   C_TAMask_ovl* tamask = static_cast<C_TAMask_ovl *>(get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask));
   if (tamask)
   {
      tamask->ResetAltitudeUpdated();
   }

   // If in the 3D projection, then invalide the map
   MapView* map_view = fvw_get_view();
   if (map_view &&
      map_view->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)
   {
      map_view->invalidate_view();
   }

   // Don't update 3D faster than once every 900 ms
   DWORD dwT2 = ::GetTickCount();
   if ( 900 > (INT) ( dwT2 - m_dwLastDateTimeUpdateTicks ) )
   {
      m_p3DCatchUpTimer->restart();
      m_dt3DDeferredDateTime = t;
   }
   else
   {
      m_p3DCatchUpTimer->stop();
      m_dwLastDateTimeUpdateTicks = dwT2;
      set_current_view_time_in_3D( t );
   }
}  // set_current_view_time()


void C_ovl_mgr::set_current_view_time_in_3D( COleDateTime dateTime )
{
   // inform all map nodes of the change
   if (m_map_node != nullptr)
   {
      PlaybackTimeChangedNodeVisitor::SetCurrentPlaybackTime( dateTime.m_dt );
      GenericNodeCallback *node_callback = new GenericNodeCallback;
      node_callback->SetOperation(m_map_node, [this]()
      {
         PlaybackTimeChangedNodeVisitor playback_time_changed_node_visitor;
         m_map_node->accept(playback_time_changed_node_visitor);
      });
   }
}

void C_ovl_mgr::OnPlaybackStarted()
{
   for (C_overlay *overlay = get_first(); overlay!=NULL; overlay = get_next(overlay))
   {
      PlaybackEventsObserver_Interface *pObserver = dynamic_cast<PlaybackEventsObserver_Interface *>(overlay);
      if (pObserver != NULL)
         pObserver->OnPlaybackStarted();
   }
}

void C_ovl_mgr::OnPlaybackStopped()
{
   for (C_overlay *overlay = get_first(); overlay!=NULL; overlay = get_next(overlay))
   {
      PlaybackEventsObserver_Interface *pObserver = dynamic_cast<PlaybackEventsObserver_Interface *>(overlay);
      if (pObserver != NULL)
         pObserver->OnPlaybackStopped();
   }
}

void C_ovl_mgr::OnPlaybackRateChanged()
{
   for (C_overlay *overlay = get_first(); overlay!=NULL; overlay = get_next(overlay))
   {
      PlaybackEventsObserver_Interface *pObserver = dynamic_cast<PlaybackEventsObserver_Interface *>(overlay);
      if (pObserver != NULL)
         pObserver->OnPlaybackRateChanged(CMainFrame::GetPlaybackDialog().get_playback_rate());
   }
}

// returns the number of time sensitive overlays opened
int C_ovl_mgr::get_num_time_sensitive_overlays()
{
   int count = 0;

   for(C_overlay *overlay = get_first(); overlay!=NULL; overlay = get_next(overlay))
   {
      PlaybackTimeSegment_Interface *pTimeSegment = dynamic_cast<PlaybackTimeSegment_Interface *>(overlay);
      if (pTimeSegment != NULL && pTimeSegment->SupportsFvPlaybackTimeSegment())
      {
         DATE begin, end;
         long bTimeSegmentValid;
         pTimeSegment->GetTimeSegment(&begin, &end, &bTimeSegmentValid);

         if (bTimeSegmentValid)
            count++;
      }
   }

   return count;
}

// return the n-th time sensitive overlay
C_overlay *C_ovl_mgr::get_time_sensitive_overlay(int n)
{
   for(C_overlay *overlay = get_first(); overlay!=NULL; overlay = get_next(overlay))
   {
      PlaybackTimeSegment_Interface *pTimeSegment = dynamic_cast<PlaybackTimeSegment_Interface *>(overlay);
      if (pTimeSegment != NULL && pTimeSegment->SupportsFvPlaybackTimeSegment())
      {
         DATE begin, end;
         long bTimeSegmentValid;
         pTimeSegment->GetTimeSegment(&begin, &end, &bTimeSegmentValid);

         if (bTimeSegmentValid && !n--)
            return overlay;
      }
   }

   return NULL;
}

// Return the handle of the given overlay, returns -1 if not found
int C_ovl_mgr::get_overlay_handle(C_overlay *overlay)
{
   int handle;
   C_overlay *ovl;

   // loop through the overlays in the overlay map to look
   // for a match
   POSITION pos = m_overlay_map.GetStartPosition();
   while (pos)
   {
      // get the next overlay in the overlay map
      m_overlay_map.GetNextAssoc(pos, handle, ovl);

      // if it matches the given overlay, return the handle
      if (overlay == ovl)
         return handle;
   }

   // the given overlay was not found in the overlay map
   return -1;
}

// Given an overlay handle, return a pointer to an overlay with this handle.
// Returns NULL if the overlay was not found in the overlay map or if the
// overlay is not in the overlay manager's list of overlays
C_overlay *C_ovl_mgr::lookup_overlay(int handle)
{
   C_overlay *overlay = NULL;

   // if there is an overlay in the overlay map with the given handle return it
   if (m_overlay_map.Lookup(handle, overlay))
      return overlay;

   return NULL;
}

// reorder the overlays so that the layer will end up
// above the given overlay with the given handle
int C_ovl_mgr::move_above(int layer_handle, int overlay_handle)
{
   C_overlay *layer = lookup_overlay(layer_handle);

   // make sure that we have a pointer to a layer
   if (layer == NULL)
      return FAILURE;

   // get a pointer to the overlay to move above
   C_overlay *overlay = lookup_overlay(overlay_handle);

   // make sure the pointer is valid
   if (overlay == NULL)
      return FAILURE;

   return move_above(layer, overlay);
}

int C_ovl_mgr::move_above(C_overlay *pOverlayToMove, C_overlay *pInsertBeforeOverlay)
{
   POSITION position_to_insert_before = m_overlay_list.Find(pInsertBeforeOverlay);
   POSITION position = m_overlay_list.Find(pOverlayToMove);
   if (!position || !position_to_insert_before)
      return FAILURE;

   m_overlay_list.RemoveAt(position);
   m_overlay_list.InsertBefore(position_to_insert_before, pOverlayToMove);

   OnOverlayOrderChanged(TRUE);

   return SUCCESS;
}

// reorder the overlays so that the given layer will end up
// below the given overlay with the given handle
int C_ovl_mgr::move_below(int layer_handle, int overlay_handle)
{
   C_overlay *layer = lookup_overlay(layer_handle);

   // make sure that we have a pointer to a layer
   if (layer == NULL)
      return FAILURE;

   // get a pointer to the overlay to move above
   C_overlay *overlay = lookup_overlay(overlay_handle);

   // make sure the pointer is valid
   if (overlay == NULL)
      return FAILURE;

   return move_below(layer, overlay);
}

int C_ovl_mgr::move_below(C_overlay *pOverlayToMove, C_overlay *pInsertBelowOverlay)
{
   POSITION position_to_insert_before = m_overlay_list.Find(pInsertBelowOverlay);
   POSITION position = m_overlay_list.Find(pOverlayToMove);
   if (!position || !position_to_insert_before)
      return FAILURE;

   m_overlay_list.RemoveAt(position);
   m_overlay_list.InsertAfter(position_to_insert_before, pOverlayToMove);

   OnOverlayOrderChanged(TRUE);

   return SUCCESS;
}

// -----------------------------------------------------------------------------
boolean_t C_ovl_mgr::can_drop_data( COleDataObject* data_object, CPoint& pt ) 
{
   if (m_overlay_list.GetCount() == 0)
      return FALSE;

   POSITION pos = m_overlay_list.GetHeadPosition();
   while (pos && !FVW_is_draw_interrupted())
   {
      C_overlay* overlay = (C_overlay *)m_overlay_list.GetNext(pos);

      // return when we have pasted object successfully
      if ( overlay->can_drop_data( data_object, pt ) )
         return TRUE;
   }

   return FALSE;
}


// -----------------------------------------------------------------------------
// paste OleDataObject (used by OLE drag/drop, copy/paste)
boolean_t C_ovl_mgr::paste_OLE_data_object(ViewMapProj *map, CPoint pt, 
   COleDataObject* data_object )
{
   boolean_t rc = FALSE;

   // test if we have dropped a file overlay onto the map
   if ( data_object->IsDataAvailable(CF_HDROP) )
   {
      STGMEDIUM stg_medium;
      HGLOBAL hglobal;

      // get the file name and extension from the COLEDataObject
      data_object->GetData((CLIPFORMAT) CF_HDROP, &stg_medium);
      if (stg_medium.tymed == TYMED_HGLOBAL)
         hglobal = stg_medium.hGlobal;

      // how many files are dropped onto your Form
      // (see DragQueryFile in win32.hlp for details)
      UINT filecount = DragQueryFile( (HDROP)hglobal,0xffffffff,0,0);

      // get the name of these files and see if you can use them
      for(UINT i = 0; i < filecount; ++i)
      {
         char buffer[MAX_PATH];
         DragQueryFile ((HDROP)hglobal, i, buffer, MAX_PATH);
         CString file_path = buffer;
         int index = file_path.ReverseFind('.');
         CString ext = file_path.Mid( index );

         OverlayTypeDescriptor *pOverlayTypeDesc = overlay_type_util::GetOverlayTypeFromFileExtension(ext);
         if (pOverlayTypeDesc != NULL && pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
         {
            C_overlay *ret_overlay;
            rc = OpenFileOverlay(pOverlayTypeDesc->overlayDescriptorGuid, file_path, ret_overlay) == SUCCESS;
         }
      }
   }

   // we have not dropped a file overlay onto the map, so let
   //all of the open overlays have a crack at the dropped file.
   //Once a file overlay says it can handle the COleDataObject,
   //stop processing (we only want it dropped onto the map once).
   POSITION pos = m_overlay_list.GetHeadPosition();
   while (pos && !FVW_is_draw_interrupted())
   {
      C_overlay* overlay = (C_overlay *)m_overlay_list.GetNext(pos);

      // skip this overlay if it is hidden
      if (!overlay->get_m_bIsOverlayVisible())
         continue;

      // return when we have pasted object successfully
      if ( overlay->paste_OLE_data_object( map, &pt, data_object ) )
         return TRUE;
   }

   return rc;
}

void C_ovl_mgr::notify_clients_of_close(int overlay_handle, boolean_t program_exiting)
{
   OLECHAR FAR* szMember;

   // Choose the correct callback based upon if we are simply closing 
   // the layer or if we are closing FalconView
   if (program_exiting)
      szMember = L"OnFalconViewExit";
   else
      szMember = L"OnOverlayClose";

   POSITION position = m_dispatch_lst.GetHeadPosition();
   while (position)
   {
      IDispatch *pdisp = m_dispatch_lst.GetNext(position);

      if (pdisp == NULL)
      {
         ERR_report("Client's dispatch pointer is not set");
         continue;
      }

      try
      {
         fvw::ICallbackPtr callback = pdisp;
         if (callback != NULL)
         {
            if (program_exiting)
               callback->OnFalconViewExit(overlay_handle);
            else
               callback->OnOverlayClose(overlay_handle);
         }
         else
         {
            CCallback callbackImpl(pdisp);
            if (program_exiting)
               callbackImpl.OnFalconViewExit(overlay_handle);
            else
               callbackImpl.OnOverlayClose(overlay_handle);
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("OnFalconViewExit/OnOverlayClose failed - %s", (char *)e.Description());
         ERR_report(msg);
      }
      catch(COleException *e)
      {
         ERR_report_ole_exception("OnFalconViewExit/OnOverlayClose failed", *e);
         e->Delete();
      }
   }
}

void C_ovl_mgr::notify_clients_of_coverage_update()
{
   OLECHAR FAR* szMember = L"OnCoverageChanged";

   POSITION position = m_dispatch_lst.GetHeadPosition();
   while (position)
   {
      IDispatch *pdisp = m_dispatch_lst.GetNext(position);

      if (pdisp == NULL)
      {
         ERR_report("Client's dispatch pointer is not set");
         continue;
      }

      try
      {
         fvw::ICallback2Ptr callback = pdisp;
         if (callback != NULL)
         {
            callback->OnCoverageChanged();
            return;
         }
         else
         {
            CCallback2 callback2Impl(pdisp);
            callback2Impl.OnCoverageChanged();
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("OnCoverageChanged failed - %s", (char *)e.Description());
         ERR_report(msg);
      }
      catch(COleException *e)
      {
         ERR_report_ole_exception("OnCoverageChanged failed", *e);
         e->Delete();
      }
   }
}

void C_ovl_mgr::notify_clients_of_open(int overlay_handle)
{
   OLECHAR FAR* szMember = L"OnOverlayOpened";

   POSITION position = m_dispatch_lst.GetHeadPosition();
   while (position)
   {
      IDispatch *pdisp = m_dispatch_lst.GetNext(position);

      if (pdisp == NULL)
      {
         ERR_report("Client's dispatch pointer is not set");
         continue;
      }

      try
      {
         fvw::ICallback2Ptr callback = pdisp;
         if (callback != NULL)
            callback->OnOverlayOpened(overlay_handle);
         else
         {
            CCallback2 callback2Impl(pdisp);
            callback2Impl.OnOverlayOpened(overlay_handle);
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("OnOverlayOpened failed - %s", (char *)e.Description());
         ERR_report(msg);
      }
      catch(COleException *e)
      {
         ERR_report_ole_exception("OnOverlayOpened failed", *e);
         e->Delete();
      }
   }
}

void C_ovl_mgr::notify_clients_of_save(int overlay_handle)
{
   OLECHAR FAR* szMember = L"OnOverlaySaved";

   POSITION position = m_dispatch_lst.GetHeadPosition();
   while (position)
   {
      IDispatch *pdisp = m_dispatch_lst.GetNext(position);

      if (pdisp == NULL)
      {
         ERR_report("Client's dispatch pointer is not set");
         continue;
      }

      try
      {
         fvw::ICallback2Ptr callback = pdisp;
         if (callback != NULL)
            callback->OnOverlaySaved(overlay_handle);
         else
         {
            CCallback2 callback2Impl(pdisp);
            callback2Impl.OnOverlaySaved(overlay_handle);
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("OnOverlaySaved failed - %s", (char *)e.Description());
         ERR_report(msg);
      }
      catch(COleException *e)
      {
         ERR_report_ole_exception("OnOverlaySaved failed", *e);
         e->Delete();
      }
   }
}

void C_ovl_mgr::NotifyClientsFVReady()
{
   OLECHAR FAR* szMember = L"OnFalconViewReady";

   POSITION position = m_dispatch_lst.GetHeadPosition();
   while (position)
   {
      IDispatch *pdisp = m_dispatch_lst.GetNext(position);

      if (pdisp == NULL)
      {
         ERR_report("Client's dispatch pointer is not set");
         continue;
      }

      try
      {
         fvw::ICallback3Ptr callback = pdisp;
         if (callback != NULL)
            callback->OnFalconViewReady();
         else
         {
            DISPID dispid;
            if (pdisp->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid) == S_OK)
            {
               // Invoke the method.  The arguments in dispparams are in reverse order
               // so the first argument is the one with the highest index in the array
               DISPPARAMS dispparams = {NULL, NULL, 0, 0};
               pdisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispparams, NULL, NULL, NULL);
            }
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("OnFalconViewReady failed - %s", (char *)e.Description());
         ERR_report(msg);
      }
      catch(COleException *e)
      {
         ERR_report_ole_exception("OnPlaybackTimeChanged failed", *e);
         e->Delete();
      }
   }
}

// Look up an overlay by an IFvOverlay interface pointer
C_overlay *C_ovl_mgr::FindOverlay(FalconViewOverlayLib::IFvOverlay *pFvOverlay)
{
   C_overlay *pOverlay = OVL_get_overlay_manager()->get_first();
   while (pOverlay != NULL)
   {
      COverlayCOM *pOverlayCOM = dynamic_cast<COverlayCOM *>(pOverlay);
      if (pOverlayCOM != NULL && pOverlayCOM->GetFvOverlay() == pFvOverlay)
         return pOverlayCOM;

      // query for the IFvOverlay interface.  Some internal overlays implement this
      FalconViewOverlayLib::IFvOverlay* p = dynamic_cast<FalconViewOverlayLib::IFvOverlay*>(pOverlay);
      if (p == pFvOverlay)
         return pOverlay;

      pOverlay = OVL_get_overlay_manager()->get_next(pOverlay);
   }

   return NULL;
}


void C_ovl_mgr::SetCurrentEditorGuid(const GUID& editorGuid)
{
   m_currentEditorGuid = editorGuid;
}

bool C_ovl_mgr::IsTopMostOverlay(C_overlay* overlay)
{
   OverlayTypeDescriptor* pOverlayTypeDesc =
      OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
   if (pOverlayTypeDesc == NULL)
   {
      ERR_report("Unable to retrieve an overlay type");
      return false;
   }

   return pOverlayTypeDesc->is_top_most;
}

POSITION C_ovl_mgr::GetTopOverlayPosition(C_overlay* overlay)
{
   POSITION top_position = nullptr;

   if (IsTopMostOverlay(overlay))
   {
      top_position = m_overlay_list.GetHeadPosition();
   }
   else
   {
      POSITION pos = m_overlay_list.GetHeadPosition();

      while (pos)
      {
         POSITION curr_pos = pos;

         C_overlay* ovl = m_overlay_list.GetNext(pos);

         if (!IsTopMostOverlay(ovl))
         {
            top_position = curr_pos;
            break;
         }         
      }
   }

   return top_position;
}

POSITION C_ovl_mgr::GetBottomOverlayPosition(C_overlay* overlay)
{
   POSITION bottom_position = nullptr;

   if (!IsTopMostOverlay(overlay))
   {
      bottom_position = m_overlay_list.GetTailPosition();
   }
   else
   {
      POSITION pos = m_overlay_list.GetTailPosition();

      while (pos)
      {
         POSITION curr_pos = pos;

         C_overlay* ovl = m_overlay_list.GetPrev(pos);

         if (IsTopMostOverlay(ovl))
         {
            bottom_position = curr_pos;
            break;
         }         
      }
   }

   return bottom_position;
}

int C_ovl_mgr::TopMostOverlayOpacity(C_overlay* overlay)
{
   OverlayTypeDescriptor* pOverlayTypeDesc =
      OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
   if (pOverlayTypeDesc == NULL)
   {
      ERR_report("Unable to retrieve an overlay type");
      return 100;
   }

   return pOverlayTypeDesc->default_opacity;
}

