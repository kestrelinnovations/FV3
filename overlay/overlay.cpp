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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// overlay.cpp: C_overlay Implementation

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/include/overlay.h"

// system includes
// third party files
#include "..\getobjpr.h"

// other FalconView headers
#include "Common/ComErrorObject.h"
#include "Common/ScopedCriticalSection.h"

// this project's headers
#include "FalconView/DisplayElementRootNode.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/GenericNodeCallback.h"
#include "FalconView/include/file.h"
#include "FalconView/include/mapx.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/ovlElementContainer.h"
#include "FalconView/include/OverlayElements.h"
#include "FalconView/OverlayInvalidatedListener_Interface.h"
#include "FalconView/include/OvlFctry.h"
#include "FalconView/include/param.h"
#include "FalconView/include/PntExprt.h"
#include "FalconView/mapview.h"
#include "FalconView/overlay/OverlayBackingStore.h"

boolean_t C_overlay::get_valid()
{
   return m_valid;
}

void C_overlay::set_valid(boolean_t valid)
{
   m_valid = valid;
}

// Constructor
C_overlay::C_overlay() :
   m_pOverlayBackingStore(nullptr),
   m_display_element_root_node(nullptr)
{
   m_valid = FALSE;
   m_bIsOverlayVisible = TRUE;
   m_ovlElementContainer = new OvlElementContainer();
   ::InitializeCriticalSection(&m_drawing_critical_section);
   ::InitializeCriticalSection(&m_invalidate_listener_critical_section);

   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->AddMapParamsListener(this);
}

// Destructor - virtual
C_overlay::~C_overlay()
{
   ::DeleteCriticalSection(&m_invalidate_listener_critical_section);
   ::DeleteCriticalSection(&m_drawing_critical_section);
   delete m_pOverlayBackingStore;
   delete m_ovlElementContainer;

   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->RemoveMapParamsListener(this);

}

OvlElementContainer* C_overlay::GetOvlElementContainer()
{
   return m_ovlElementContainer;
}

boolean_t C_overlay::get_current()
{
   return (OVL_get_overlay_manager()->get_current_overlay() == this);
}

boolean_t C_overlay::is_modified()
{
   OverlayPersistence_Interface* pFvOverlayPersistence =
      dynamic_cast<OverlayPersistence_Interface *>(this);
   if (pFvOverlayPersistence != NULL)
   {
      long bIsDirty;
      pFvOverlayPersistence->get_m_bIsDirty(&bIsDirty);

      return bIsDirty;
   }

   return FALSE;
}

// Set the modified attribute for this overlay.  It is FALSE
// by default when the overlay is created.
void C_overlay::set_modified(boolean_t mod_flag)
{
   OverlayPersistence_Interface* pFvOverlayPersistence =
      dynamic_cast<OverlayPersistence_Interface *>(this);
   if (pFvOverlayPersistence != NULL)
   {
      long bAlreadyDirty = 0;
      pFvOverlayPersistence->get_m_bIsDirty(&bAlreadyDirty);
      pFvOverlayPersistence->put_m_bIsDirty(mod_flag != 0);

      if (bAlreadyDirty != mod_flag)
      {
         // System wide unique identifier for the update falconview title bar
         static const UINT msg_update_titlebar =
            RegisterWindowMessage("FVW_Update_TitleBar");

         // send a message to the frame, telling it to update the title bar
         CFrameWndEx *frame = UTL_get_frame();
         if (frame)
            frame->PostMessage(msg_update_titlebar, 0, 0);

         // Notify any observers that the modified flag has changed
         OVL_get_overlay_manager()->OnDirtyFlagChanged(this);
      }
   }
}

// CFvOverlayPersistenceImpl Implementation

// Constructor
CFvOverlayPersistenceImpl::CFvOverlayPersistenceImpl(const CString &pathname)
{
   m_bHasBeenSaved = FALSE;
   m_bIsDirty = FALSE;
   m_fileSpecification = pathname;
   m_bIsReadOnly = FALSE;
}

// get full file specification
const CString & CFvOverlayPersistenceImpl::get_specification() const
{
   return m_fileSpecification;
}

// set full file specification
void CFvOverlayPersistenceImpl::set_specification(const CString & specification)
{
   m_fileSpecification = specification;
   return;
}

BackingStoreTypeEnum C_overlay::GetOverlayBackingStoreType()
{
   OverlayTypeDescriptor* pOverlayDesc = OVL_get_type_descriptor_list()->
      GetOverlayTypeDescriptor(get_m_overlayDescGuid());
   if (pOverlayDesc != NULL)
      return pOverlayDesc->backingStoreType;

   // default is no backing store
   return BACKINGSTORE_TYPE_NONE;
}

COverlayBackingStore* C_overlay::GetOverlayBackingStore()
{
   if ((GetOverlayBackingStoreType() == BACKINGSTORE_TYPE_24BPP || 
        GetOverlayBackingStoreType() == BACKINGSTORE_TYPE_32BPP) &&
      m_pOverlayBackingStore == NULL)
   {
      m_pOverlayBackingStore =
         new COverlayBackingStore(this, GetOverlayBackingStoreType());
   }

   return m_pOverlayBackingStore;
}

void C_overlay::SetupActiveMap(IActiveMapProj *map, ActiveMap_TMP& active_map)
{
   active_map.m_interface_ptr = map;

   map->GetSettableMapProj(&active_map.m_map);

   // query for the ISettableMapProj2 interface
   active_map.m_map2 = active_map.m_map;

   active_map.get_actual_center(&active_map.m_map_spec.center);
   active_map.m_map_spec.source = active_map.source();
   active_map.m_map_spec.scale = active_map.scale();
   active_map.m_map_spec.series = active_map.series();
   active_map.m_map_spec.rotation = active_map.actual_rotation();
   active_map.m_map_spec.zoom_percent = active_map.actual_zoom_percent();
   if (active_map.requested_zoom_percent() == NATIVE_ZOOM_PERCENT)
      active_map.m_map_spec.zoom_percent = NATIVE_ZOOM_PERCENT;
   else if (active_map.requested_zoom_percent() == TO_SCALE_ZOOM_PERCENT)
      active_map.m_map_spec.zoom_percent = TO_SCALE_ZOOM_PERCENT;
   active_map.m_map_spec.projection_type = active_map.projection_type();
   active_map.m_map_spec.m_dBrightness = active_map.m_map->get_brightness();
   active_map.m_map->get_contrast(&active_map.m_map_spec.m_dContrast,
      &active_map.m_map_spec.m_nContrastMidval);
}

int C_overlay::OnDrawToBaseMap(IActiveMapProj *map)
{
   fv_common::ScopedCriticalSection lock(&m_drawing_critical_section);
   ActiveMap_TMP active_map;
   SetupActiveMap(map, active_map);
   const int ret = draw_to_base_map(&active_map);

   return ret;
}

// force a draw from scratch
void C_overlay::invalidate()
{
   m_valid = FALSE;

   const BackingStoreTypeEnum eBackingStoreType = GetOverlayBackingStoreType();

   if (eBackingStoreType == BACKINGSTORE_TYPE_24BPP ||
      eBackingStoreType == BACKINGSTORE_TYPE_32BPP)
   {
      if (m_pOverlayBackingStore != NULL)
         m_pOverlayBackingStore->SetInvalid();
   }

   fv_common::ScopedCriticalSection lock(&m_invalidate_listener_critical_section);
   std::for_each(m_overlay_invalidated_listeners.begin(),
      m_overlay_invalidated_listeners.end(),
      [] (OverlayInvalidatedListener_Interface* listener)
   {
      listener->Invalidate();
   });
}


// Add or remove an overlay invalidate listener
void C_overlay::AddOverlayInvalidatedListener(
   OverlayInvalidatedListener_Interface* listener)
{
   fv_common::ScopedCriticalSection lock(
      &m_invalidate_listener_critical_section);

   // Only add a listener once.
   auto location = std::find( m_overlay_invalidated_listeners.begin(),
      m_overlay_invalidated_listeners.end(), listener);
   if (location == m_overlay_invalidated_listeners.end())
   {
      m_overlay_invalidated_listeners.push_back(listener);
   }
}

void C_overlay::RemoveOverlayInvalidatedListener(
   OverlayInvalidatedListener_Interface* listener)
{
   fv_common::ScopedCriticalSection lock(&m_invalidate_listener_critical_section);
   auto location = std::find( m_overlay_invalidated_listeners.begin(),
      m_overlay_invalidated_listeners.end(), listener);
   if ( location != m_overlay_invalidated_listeners.end() )
      m_overlay_invalidated_listeners.erase(location);
}

// IFvOverlayContextMenu implemenation
//

HRESULT C_overlay::AppendMenuItems(
   FalconViewOverlayLib::IFvContextMenu *pContextMenu,
   FalconViewOverlayLib::IFvMapView *pMapView, long x, long y)
{
   CList<CFVMenuNode *, CFVMenuNode *> menuItems;
   menu(OVL_get_overlay_manager()->get_view_map(), CPoint(x, y), menuItems);

   POSITION position = menuItems.GetHeadPosition();
   while (position)
   {
      CFVMenuNode *pContextMenuItem = menuItems.GetNext(position);
      if (pContextMenuItem != NULL)
      {
         CComObject<CFVMenuNodeContextMenuItem> *pMenuItem = NULL;
         CComObject<CFVMenuNodeContextMenuItem>::CreateInstance(&pMenuItem);
         pMenuItem->Initialize(pContextMenuItem);

         pContextMenu->AppendMenuItem(pMenuItem);
      }
   }

   return S_OK;
}

// IFvOverlayRender implementation
//

HRESULT C_overlay::OnDraw(IActiveMapProj *pActiveMap)
{
   fv_common::ScopedCriticalSection lock(&m_drawing_critical_section);
   const BackingStoreTypeEnum eBackingStoreType = GetOverlayBackingStoreType();

   ActiveMap_TMP activeMapWrapper(pActiveMap);

   // If the overlay's backing store type is none, we are printing, or in 3D Mode.
   if (eBackingStoreType == BACKINGSTORE_TYPE_NONE ||   
       activeMapWrapper.get_CDC()->IsPrinting() || 
       fvw_get_view()->GetProjectionType() == GENERAL_PERSPECTIVE_PROJECTION)   
   {  
      // render the overlay as normal  
      return draw(&activeMapWrapper);  
   }  
   else if (eBackingStoreType == BACKINGSTORE_TYPE_24BPP ||  
      eBackingStoreType == BACKINGSTORE_TYPE_32BPP)  
   {  
      COverlayBackingStore* pBackingStore = GetOverlayBackingStore();  
      if (pBackingStore != NULL)  
         return GetOverlayBackingStore()->OnDraw(&activeMapWrapper);  
   }  
   else  
   {  
      ERR_report("Unknown backing store type");  
   }  
  
   return FAILURE;  
}

// IFvOverlayUIEvents implementation
//

HRESULT C_overlay::MouseMove(FalconViewOverlayLib::IFvMapView *, long x, long y,
   long nFlags, long *pbHandled)
{
   // internal overlays still use the C++ IFvMapView interface
   IFvMapView *pMapView = static_cast<IFvMapView *>(fvw_get_view());
   *pbHandled = on_mouse_moved(pMapView, CPoint(x, y), nFlags) == SUCCESS;
   return S_OK;
}

// MouseLeftButtonDown occurs when the left mouse button is pressed while the
// mouse pointer is over the given view
HRESULT C_overlay::MouseLeftButtonDown(FalconViewOverlayLib::IFvMapView *,
   long x, long y, long nFlags, long *pbHandled)
{
   // internal overlays still use the C++ IFvMapView interface
   IFvMapView *pMapView = static_cast<IFvMapView *>(fvw_get_view());
   *pbHandled = selected(pMapView, CPoint(x, y), nFlags) == SUCCESS;
   return S_OK;
}

// MouseLeftButtonUp occurs when the left mouse button is released while the
// mouse point is over the given view
HRESULT C_overlay::MouseLeftButtonUp(FalconViewOverlayLib::IFvMapView *,
   long x, long y, long nFlags, long *pbHandled)
{
   // internal overlays still use the C++ IFvMapView interface
   IFvMapView *pMapView = static_cast<IFvMapView *>(fvw_get_view());
   *pbHandled = on_left_mouse_button_up(pMapView, CPoint(x, y), nFlags) == SUCCESS;
   return S_OK;
}

// MouseRightButtonDown occurs when the right mouse button is pressed while the
// mouse pointer is over the given view.
HRESULT C_overlay::MouseRightButtonDown(FalconViewOverlayLib::IFvMapView *,
   long x, long y, long nFlags, long *pbHandled)
{
   // internal overlays still use the C++ IFvMapView interface
   IFvMapView *pMapView = static_cast<IFvMapView *>(fvw_get_view());
   *pbHandled = on_right_mouse_button_down(pMapView, CPoint(x, y), nFlags) == SUCCESS;
   return S_OK;
}

// MouseDoubleClick occurs when the mouse button is double clicked over the
// given view
HRESULT C_overlay::MouseDoubleClick(FalconViewOverlayLib::IFvMapView *,
   long x, long y, long nFlags, long *pbHandled)
{
   // internal overlays still use the C++ IFvMapView interface
   IFvMapView *pMapView = static_cast<IFvMapView *>(fvw_get_view());
   *pbHandled = on_dbl_click(pMapView, nFlags, CPoint(x, y)) == SUCCESS;
   return S_OK;
}

// MouseWheel occurs when the user rotates the mouse wheel while the mouse
// pointer is over the given view
HRESULT C_overlay::MouseWheel(FalconViewOverlayLib::IFvMapView *,
   long x, long y, long zDelta, long nFlags, long *pbHandled)
{
   // internal overlays still use the C++ IFvMapView interface
   IFvMapView *pMapView = static_cast<IFvMapView *>(fvw_get_view());
   *pbHandled = offer_mousewheel(pMapView, static_cast<SHORT>(zDelta), nFlags, CPoint(x, y));
   return S_OK;
}

// KeyDown occurs when a key is pressed
HRESULT C_overlay::KeyDown(FalconViewOverlayLib::IFvMapView *, long x, long y,
   long nChar, long nRepCnt, long nFlags, long *pbHandled)
{
   // internal overlays still use the C++ IFvMapView interface
   IFvMapView *pMapView = static_cast<IFvMapView *>(fvw_get_view());
   *pbHandled = offer_keydown(pMapView, nChar, nRepCnt, nFlags, CPoint(x, y));
   return S_OK;
}

// KeyUp occurs when a key is released
HRESULT C_overlay::KeyUp(FalconViewOverlayLib::IFvMapView *, long x, long y,
   long nChar, long nRepCnt, long nFlags, long *pbHandled)
{
   // internal overlays still use the C++ IFvMapView interface
   IFvMapView *pMapView = static_cast<IFvMapView *>(fvw_get_view());
   *pbHandled = offer_keyup(pMapView, nChar, nRepCnt, nFlags, CPoint(x, y));
   return S_OK;
}

void C_overlay::OuterFinalize()
{
   if (m_display_element_root_node)
      m_display_element_root_node->Finalize();

   fv_common::ScopedCriticalSection lock(&m_drawing_critical_section);
   Finalize();

   if (m_display_element_root_node)
   {
      MapView* map_view = fvw_get_view();
      if (map_view)
      {
         GeospatialScene* scene = map_view->GetGeospatialViewController()->
            GetGeospatialScene();
         if (scene)
         {
            osg::Group* root_node = scene->RootNode();

            // Delete the DisplayElementRootNode during the update traversal since
            // destroying the root osg::Node will update the scene graph
            GenericNodeCallback* callback = new GenericNodeCallback;
            DisplayElementRootNode* dern = m_display_element_root_node;
            callback->SetOperation(root_node, [dern]()
            {
               delete dern;
            });
            m_display_element_root_node = nullptr;
         }
      }

      // If any of the above conditionals fails, delete the root node here
      delete m_display_element_root_node;
   }
}

bool C_overlay::IsTopMostOverlay()
{
   return OVL_get_overlay_manager()->IsTopMostOverlay(this);
}

int C_overlay::Opacity()
{
   return OVL_get_overlay_manager()->TopMostOverlayOpacity(this);
}

void C_overlay::InitializeDisplayElementRootNode()
{
   // Don't initialize the display provider until 3D is toggled on
   MapView* map_view = fvw_get_view();
   if (map_view == nullptr ||
      map_view->GetMapProjParams().type != GENERAL_PERSPECTIVE_PROJECTION ||
      m_display_element_root_node != nullptr)
   {
      return;
   }

   m_display_element_root_node = new DisplayElementRootNode(this);
}

void C_overlay::MapParamsChanged(int change_mask,
   long map_group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params,
   const MapDisplayParams& map_display_params)
{
   if ( (change_mask & MAP_PROJ_TYPE_CHANGED) &&
      m_display_element_root_node == nullptr)
   {
      InitializeDisplayElementRootNode();
   }
}

// First step in marshaling the IFvOverlay interface pointer to another
// thread. This call must be made from the user interface thread.
IStream* C_overlay::PrepareMarshaledStream()
{
   IStream* marshaled_stream;
   if (::CoMarshalInterThreadInterfaceInStream(
      FalconViewOverlayLib::IID_IFvOverlay, GetFvOverlay(), &marshaled_stream)
         != S_OK)
   {
      marshaled_stream = nullptr;
      ERR_report("C_overlay::PrepareMarshaledStream failed");
   }

   return marshaled_stream;
}

void C_overlay::ResetDisplayNodeVisibility()
{
   if (m_display_element_root_node != nullptr)
   {
      bool visible = get_m_bIsOverlayVisible() == TRUE;
      C_ovl_mgr* manager = OVL_get_overlay_manager();
      if (visible &&
         !manager->all_overlays_visible() &&
         this != manager->get_current_overlay())
      {
         visible = false;
      }

      m_display_element_root_node->SetVisible(visible);
   }
}

HRESULT C_overlay::put_m_bIsOverlayVisible(long bIsOverlayVisible)
{
   m_bIsOverlayVisible = bIsOverlayVisible;
   ResetDisplayNodeVisibility();
   return S_OK;
}
