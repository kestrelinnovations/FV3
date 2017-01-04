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

// ILayer.cpp : implementation file
//

#include "stdafx.h"
#include "ILayer.h"
#include "refresh.h"    // FVW_update_title_bar
#include "..\Common\map_server_client.h"
#include "..\getobjpr.h"   // fvw_get_frame()
#include "wm_user.h"
#include "FileSpec.h" // for UNC paths
#include "services.h"
#include "MovingMapOverlay\factory.h"
#include "ovlElementContainer.h"
#include "overlay\OverlayCOM.h"
#include "appinfo.h"
#include "FvCore\Include\Facets.h"
#include "PlaybackDialog\viewtime.h"

/////////////////////////////////////////////////////////////////////////////
// Layer - The layer class will house the extended map server calls dealing 
// specifically with layer manipulation.  Rather than a single class as before, 
// the map server is now split into two interfaces (ILayer and IMap).

//
// static variables
//
int Layer::m_num_clients = 0;

IMPLEMENT_DYNCREATE(Layer, CCmdTarget)

// {1E4408DF-7CFF-11D3-80A9-00C04F60B086}
IMPLEMENT_OLECREATE(Layer, "FalconView.Layer", 0x1E4408DF, 0x7CFF, 0x11D3, 
                    0x80, 0xA9, 0x00, 0xC0, 0x4F, 0x60, 0xB0, 0x86)

Layer::Layer() : 
   m_pdisp(NULL)
{
   EnableAutomation();
   
   increment_num_clients();
   
   // client has not yet called RegisterWithMapServer
   m_client_registered_with_server = FALSE; 
   
   // the first icon handle allocated will be a one
   m_next_icon_handle = 1;
   
   // the first symbol handle allocated will be one hundred (so that
   // no conflicts with any internal symbols will occur
   m_next_symbol_handle = 100; 
   
   // initialize the client's window handle to zero
   m_client_window_handle = 0;
   
   // client has not called RegisterWithMapServer
   m_client_registered_with_server = FALSE; 
}

Layer::~Layer()
{
   // decrement the number of OLE Map Server clients
   decrement_num_clients();  
   
   // delete all layer overlays and remove them from overlay manager
   CList <C_overlay *, C_overlay *> overlay_list;
   POSITION position = overlay_list.GetHeadPosition();
   while (position)
   {
      // get the next overlay in the list
      C_overlay *overlay = overlay_list.GetNext(position);

      // if it is a layer overlay then delete it
      if (dynamic_cast<CBaseLayerOvl *>(overlay) != NULL)
      {
         if (OVL_get_overlay_manager()->delete_overlay(overlay, FALSE) != SUCCESS)
         {
            ERR_report("delete_overlay() in ~Layer failed.");
         }

         overlay->Release();
      }
   }

   // delete all symbols that were created
   {
      int symbol_handle;
      OvlSymbol *symbol;
      
      // iterate through the symbols in the symbol map
      POSITION position = m_symbol_map.GetStartPosition();
      while (position)
      {
         m_symbol_map.GetNextAssoc(position, symbol_handle, symbol);
         m_symbol_map.RemoveKey(symbol_handle);
         delete symbol;
      }
   }

   remove_dispatch_ptr(m_pdisp);
}

void Layer::remove_dispatch_ptr(IDispatch* pdisp)
{
   try
   {
      if (pdisp != NULL)
      {
         OVL_get_overlay_manager()->remove_dispatch_ptr(pdisp);
         pdisp->Release();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Callback Release failed: %s", (char *)e.Description());
      ERR_report(msg);
   }
}

void Layer::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(Layer, CCmdTarget)
   //{{AFX_MSG_MAP(Layer)
   // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Layer, CCmdTarget)
   //{{AFX_DISPATCH_MAP(Layer)
   DISP_FUNCTION(Layer, "SetFont", SetFont, VT_I4, VTS_I4 VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "SetLineType", SetLineType, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "SetFillType", SetFillType, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "DeleteLayer", DeleteLayer, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "AddEllipse", AddEllipse, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION(Layer, "AddPolyline", AddPolyline, VT_I4, VTS_I4 VTS_VARIANT VTS_I4)
   DISP_FUNCTION(Layer, "AddPolygon", AddPolygon, VT_I4, VTS_I4 VTS_VARIANT VTS_I4)
   DISP_FUNCTION(Layer, "DeleteObject", DeleteObject, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "Refresh", Refresh, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "SetAnchorType", SetAnchorType, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "AddText", AddText, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_BSTR VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "AddBitmap", AddBitmap, VT_I4, VTS_I4 VTS_BSTR VTS_R8 VTS_R8)
   DISP_FUNCTION(Layer, "AddIcon", AddIcon, VT_I4, VTS_I4 VTS_BSTR VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION(Layer, "AddIconMulti", AddIconMulti, VT_I4, VTS_I4 VTS_BSTR VTS_VARIANT VTS_VARIANT VTS_I4)
   DISP_FUNCTION(Layer, "AddObjectToGroup", AddObjectToGroup, VT_I4, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "MoveObjectToTop", MoveObjectToTop, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "MoveObjectToBottom", MoveObjectToBottom, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "MoveLayerToBottom", MoveLayerToBottom, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "MoveLayerToTop", MoveLayerToTop, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "MoveBitmap", MoveBitmap, VT_I4, VTS_I4 VTS_I4 VTS_R8 VTS_R8)
   DISP_FUNCTION(Layer, "GetProducer", GetProducer, VT_I4, VTS_PBSTR)
   DISP_FUNCTION(Layer, "GetVersion", GetVersion, VT_I4, VTS_PI2 VTS_PI2 VTS_PI2)
   DISP_FUNCTION(Layer, "AddLine", AddLine, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "SetLayerThreshold", SetLayerThreshold, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "SetGroupThreshold", SetGroupThreshold, VT_I4, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "SetObjectThreshold", SetObjectThreshold, VT_I4, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "DeleteGroup", DeleteGroup, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "OpenOverlay", OpenOverlay, VT_I4, VTS_I4 VTS_BSTR)
   DISP_FUNCTION(Layer, "CloseOverlay", CloseOverlay, VT_I4, VTS_I4 VTS_BSTR)
   DISP_FUNCTION(Layer, "MoveLayerAbove", MoveLayerAbove, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "MoveLayerBelow", MoveLayerBelow, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "GetOverlayInfo", GetOverlayInfo, VT_I4, VTS_I4 VTS_PI4 VTS_PBSTR)
   DISP_FUNCTION(Layer, "GetOverlayList", GetOverlayList, VT_I4, VTS_PVARIANT VTS_PI4)
   DISP_FUNCTION(Layer, "GetActiveOverlay", GetActiveOverlay, VT_I4, VTS_NONE)
   DISP_FUNCTION(Layer, "AddDotToSymbol", AddDotToSymbol, VT_I4, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "AddLineToSymbol", AddLineToSymbol, VT_I4, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "AddCircleToSymbol", AddCircleToSymbol, VT_I4, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "AddSymbol", AddSymbol, VT_I4, VTS_I4 VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION(Layer, "MoveSymbol", MoveSymbol, VT_I4, VTS_I4 VTS_I4 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION(Layer, "CreateSymbol", CreateSymbol, VT_I4, VTS_BSTR VTS_PI2)
   DISP_FUNCTION(Layer, "MoveIcon", MoveIcon, VT_I4, VTS_I4 VTS_I4 VTS_R8 VTS_R8 VTS_I4)
   DISP_FUNCTION(Layer, "AddIconToSet", AddIconToSet, VT_I4, VTS_I4 VTS_I4 VTS_I4 VTS_BSTR)
   DISP_FUNCTION(Layer, "AddIconEx", AddIconEx, VT_I4, VTS_I4 VTS_I4 VTS_R8 VTS_R8 VTS_I4 VTS_BSTR)
   DISP_FUNCTION(Layer, "CreateIconSet", CreateIconSet, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "ShowMainToolbar", ShowMainToolbar, VT_I4, VTS_BOOL)
   DISP_FUNCTION(Layer, "ShowEditorToolbar", ShowEditorToolbar, VT_I4, VTS_BOOL)
   DISP_FUNCTION(Layer, "ShowStatusBar", ShowStatusBar, VT_I4, VTS_BOOL)
   DISP_FUNCTION(Layer, "SetupToolbarButton", SetupToolbarButton, VT_I4, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR)
   DISP_FUNCTION(Layer, "RemoveToolbar", RemoveToolbar, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "SetIsTimeSensitive", SetIsTimeSensitive, VT_I4, VTS_I4 VTS_BOOL)
   DISP_FUNCTION(Layer, "ModifyObject", ModifyObject, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "DeleteAllObjects", DeleteAllObjects, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "ShowLayer", ShowLayer, VT_I4, VTS_I4 VTS_BOOL)
   DISP_FUNCTION(Layer, "SetPen", SetPen, VT_I4, VTS_I4 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_BOOL VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "AddGeoRect", AddGeoRect, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION(Layer, "AddArc", AddArc, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BOOL)
   DISP_FUNCTION(Layer, "RegisterWithMapServer", RegisterWithMapServer, VT_I4, VTS_BSTR VTS_I4 VTS_VARIANT)
   DISP_FUNCTION(Layer, "CreateLayer", CreateLayer, VT_I4, VTS_BSTR)
   DISP_FUNCTION(Layer, "CreateLayerEx", CreateLayerEx, VT_I4, VTS_BSTR VTS_BSTR)
   DISP_FUNCTION(Layer, "SetAbsoluteTime", SetAbsoluteTime, VT_I4, VTS_I4 VTS_DATE)
   DISP_FUNCTION(Layer, "CreateOverlay", CreateOverlay, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "CloseOverlayEx", CloseOverlayEx, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "SetToolbarButtonState", SetToolbarButtonState, VT_I4, VTS_I4 VTS_I4 VTS_I2)
   DISP_FUNCTION(Layer, "SetObjectData", SetObjectData, VT_I4, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Layer, "GetObjectData", GetObjectData, VT_I4, VTS_I4 VTS_I4 VTS_PI4)
   DISP_FUNCTION(Layer, "SetPlaybackRate", SetPlaybackRate, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "StartPlayback", StartPlayback, VT_I4, VTS_NONE)
   DISP_FUNCTION(Layer, "StopPlayback", StopPlayback, VT_I4, VTS_NONE)
   DISP_FUNCTION(Layer, "PlaybackBegin", PlaybackBegin, VT_I4, VTS_NONE)
   DISP_FUNCTION(Layer, "PlaybackEnd", PlaybackEnd, VT_I4, VTS_NONE)
   DISP_FUNCTION(Layer, "AddEditorButton", AddEditorButton, VT_I4, VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR)
   DISP_FUNCTION(Layer, "SetModified", SetModified, VT_I4, VTS_I4 VTS_I2)
   DISP_FUNCTION(Layer, "AddToolbar", AddToolbar, VT_I4, VTS_BSTR VTS_I4 VTS_BSTR VTS_I2 VTS_VARIANT VTS_I4)
   DISP_FUNCTION(Layer, "RemoveEditorButton", RemoveEditorButton, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "AddLineWithEmbeddedText", AddLineWithEmbeddedText, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_BSTR)
   DISP_FUNCTION(Layer, "AddMilStd2525Symbol", AddMilStd2525Symbol, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_BSTR)
   DISP_FUNCTION(Layer, "SetTextRotation", SetTextRotation, VT_I4, VTS_I4 VTS_R8)
   DISP_FUNCTION(Layer, "GetModified", GetModified, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "SaveOverlay", SaveOverlay, VT_I4, VTS_I4)
   DISP_FUNCTION(Layer, "AddIconOffset", AddIconOffset, VT_I4, VTS_I4 VTS_BSTR VTS_R8 VTS_R8 VTS_BSTR VTS_I4 VTS_I4)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ILayer to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {1E4408DD-7CFF-11D3-80A9-00C04F60B086}
static const IID IID_ILayer =
{ 0x1e4408dd, 0x7cff, 0x11d3, { 0x80, 0xa9, 0x0, 0xc0, 0x4f, 0x60, 0xb0, 0x86 } };

BEGIN_INTERFACE_MAP(Layer, CCmdTarget)
   INTERFACE_PART(Layer, IID_ILayer, Dispatch)
END_INTERFACE_MAP()

//////////////////////////////////////////////////////////////////////////////
// Layer helper functions

// see if a symbol with the given name exists in the symbol map.  If so,
// return the symbol's handle
int Layer::lookup_symbol_name(CString name)
{
   int symbol_handle;
   OvlSymbol *symbol;

   // iterate through the symbols in the symbol map
   POSITION position = m_symbol_map.GetStartPosition();
   while (position)
   {
      m_symbol_map.GetNextAssoc(position, symbol_handle, symbol);
      
      // symbol with the given name was found
      if (symbol->get_name() == name)
         return symbol_handle;
   }

   // no symbol with the given name was found
   return -1;
}

// lookup the symbol with the given handle.  Return a pointer to the symbol
// if found.  Otherwise return NULL.
OvlSymbol *Layer::lookup_symbol(int symbol_handle)
{
   OvlSymbol *symbol = NULL;
   
   m_symbol_map.Lookup(symbol_handle, symbol);
   
   return symbol;
}

int Layer::OpenStaticOverlay(GUID overlayDescGuid)
{
   // WARNING: class_name must be a static overlay
   ASSERT(OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid));

   // see if overlay is already open
   if (OVL_get_overlay_manager()->get_first_of_type(overlayDescGuid))
   {
      return SUCCESS;
   }

   return OVL_get_overlay_manager()->toggle_static_overlay(overlayDescGuid);
}


int Layer::CloseStaticOverlay(GUID overlayDescGuid)
{
   // if overlay is already closed
   if (!OVL_get_overlay_manager()->get_first_of_type(overlayDescGuid))
      return SUCCESS;

   // WARNING: class_name must be a static overlay
   ASSERT(OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid));

   return OVL_get_overlay_manager()->toggle_static_overlay(overlayDescGuid);  //close overlay
}


int Layer::OpenFileOverlay(GUID overlayDescGuid, const char* filespec)
{
   // WARNING: the overlay type must be a file overlay
   ASSERT(OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid));

   C_overlay *ret_overlay;
   return OVL_get_overlay_manager()->OpenFileOverlay(overlayDescGuid, filespec, ret_overlay);
}


int Layer::CloseFileOverlay(GUID overlayDescGuid, const char* filespec)
{
   C_overlay *overlay;
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager(); //doesn't fail
   overlay = ovl_mgr->get_file_overlay(overlayDescGuid, filespec);
   ASSERT(overlay);

   if (overlay)  //if we found one...
   {
      //close overlay...
      int cancel;
      CList<C_overlay *, C_overlay *> close_list;

      // create the close list
      close_list.AddHead(overlay);

      // request that the overlay manager close this overlay
      return ovl_mgr->close_overlays(close_list, &cancel);
   }
   else
      return FAILURE;   //didn't find overlay
}

/////////////////////////////////////////////////////////////////////////////
// Layer message handlers

long Layer::SetFont(long layer_handle, LPCTSTR font_name, long size, 
                    long attributes, long fg_color, long bg_color, 
                    long bg_type) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

    // make sure this is a CLayerOvl pointer
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // adjust the color values so that they match the UTIL_COLOR defines
   if (fg_color >= 10)
      fg_color += 236;
   if (bg_color >= 10)
      bg_color += 236;

   CFvwUtil* util = CFvwUtil::get_instance();

   return overlay->GetOvlElementContainer()->set_font(font_name, size,
      attributes, fg_color, bg_color, bg_type);
}

long Layer::SetLineType(long layer_handle, long line_type) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->set_line_type(line_type);
}

long Layer::SetFillType(long layer_handle, long fill_type) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->set_fill_type(fill_type);
}

long Layer::DeleteLayer(long layer_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CLayerOvl pointer
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL)
      return FAILURE;

   // remove overlay from overlay manager - (note: screen is not invalidated)
   if (OVL_get_overlay_manager()->delete_overlay(overlay, FALSE) != SUCCESS)
   {
      ASSERT(0);
      ERR_report("OVL_get_overlay_manager()->delete_overlay() failed");
   }

   // delete the overlay object
   overlay->Release();

   // refresh the screen
   OVL_get_overlay_manager()->invalidate_all();

   // finally, update the title bar to reflect changes
   FVW_update_title_bar();
   
   return SUCCESS;
}

long Layer::AddEllipse(long layer_handle, double lat, double lon, 
                       double vertical_km, double horizontal_km, 
                       double rotation) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_ellipse(lat, lon, vertical_km,
      horizontal_km, rotation);
}

long Layer::AddPolyline(long layer_handle, const VARIANT FAR& lat_lon_array,
   long num_points) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_line(lat_lon_array, num_points);
}

long Layer::AddPolygon(long layer_handle, const VARIANT FAR& lat_lon_array, long num_points) 
{
   COleSafeArray geo_points(lat_lon_array);

   // make sure the dimension of the array is 2
   if (geo_points.GetDim() != 2)
   {
      ERR_report("AddPolyline failed [lat_lon_array must have 2 dimensions]");
      return FAILURE;
   }

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_polygon(geo_points,
      num_points);
}

long Layer::DeleteObject(long layer_handle, long object_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->delete_object(object_handle);
}

long Layer::Refresh(long layer_handle) 
{
   MapView *pView = fvw_get_view();

   if (pView != NULL)
   {
      if( pView->m_popup)
      {
         pView->m_need_to_redraw_after_popup = TRUE;
      }
      else
      {
         pView->PostMessage(WM_INVALIDATE_LAYER_OVERLAY, 0, layer_handle);
      }
   }

   return SUCCESS;
}

long Layer::SetAnchorType(long layer_handle, long anchor_type) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->set_anchor_type(anchor_type);
}

long Layer::AddText(long layer_handle, double lat, double lon, 
                    LPCTSTR text, long x_offset, long y_offset) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_text(lat, lon, text, x_offset,
      y_offset);
}

long Layer::AddBitmap(long layer_handle, LPCTSTR filename, double lat, double lon) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // add a bitmap using the currently selected anchor type
   return overlay->GetOvlElementContainer()->add_bitmap(filename, lat, lon);
}

long Layer::AddIcon(long layer_handle, LPCTSTR filename, double lat, 
                    double lon, LPCTSTR icon_text) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // add an icon to the layer with the given filename.  The icon's 
   // text will use the current font
   return overlay->GetOvlElementContainer()->add_icon(filename, lat, lon,
      icon_text);
}

long Layer::AddIconMulti(long layer_handle, LPCTSTR filename, 
                         const VARIANT FAR& lat_lon_array,
                         const VARIANT FAR& icon_text_array, 
                         long num_points) 
{
   COleSafeArray geo_points(lat_lon_array);
   
   // make sure the dimension of the lat/lon array is 2 and the dimension
   // of the text array is 1
   if (geo_points.GetDim() != 2 ||
       (icon_text_array.vt & VT_BSTR) == 0 ||
       (icon_text_array.vt & VT_ARRAY) == 0)
   {
      ERR_report("AddPolygon failed [incorrect dimension in lat/lon or invalid text array]");
      return FAILURE;
   }

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // add the array of icons to the layer with the given filename.  The
   // icon's text will use the current font
   return overlay->GetOvlElementContainer()->add_icon_multi(filename,
      geo_points, icon_text_array, num_points);
}

long Layer::AddObjectToGroup(long layer_handle, long object_handle, long group_id) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_object_to_group(object_handle, group_id);
}

long Layer::MoveObjectToTop(long layer_handle, long object_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->move_object_to_top(object_handle);
}

long Layer::MoveObjectToBottom(long layer_handle, long object_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->move_object_to_bottom(object_handle);
}

long Layer::MoveLayerToBottom(long layer_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CBaseLayerOvl pointer
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL)
      return FAILURE;

   return OVL_get_overlay_manager()->move_to_bottom(overlay);
}

long Layer::MoveLayerToTop(long layer_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CBaseLayerOvl pointer
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL)
      return FAILURE;

   return OVL_get_overlay_manager()->make_current(overlay);
}

long Layer::MoveBitmap(long layer_handle, long object_handle, double lat, double lon) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // move an object to a new location.  Trying to move something other than
   // an bitmap will return FAILURE
   return overlay->GetOvlElementContainer()->move_bitmap(object_handle, lat, lon);
}

// returns the producer of this interface, GTRI
long Layer::GetProducer(BSTR FAR* producer_string) 
{
   *producer_string = _bstr_t("GTRI").Detach();

   return SUCCESS;
}

long Layer::GetVersion(short FAR* major, short FAR* minor, short FAR* revision) 
{
   char version_buf[MAX_VERSION_SIZE];
   project_version_str(version_buf, MAX_VERSION_SIZE);

   std::stringstream ss(version_buf);
   ss.imbue(std::locale(std::locale(), new string_utils::custom_space_facet<char>('.')));
   ss >> *major >> *minor >> *revision;

   return SUCCESS;
}

long Layer::AddLine(long layer_handle, double lat1, double lon1, 
                    double lat2, double lon2, long x_offset, long y_offset) 
{
   if (!GEO_valid_degrees(lat1, lon1))
      return FAILURE;

   if (!GEO_valid_degrees(lat2, lon2))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_line(lat1, lon1, lat2, lon2,
      x_offset, y_offset);
}

long Layer::SetLayerThreshold(long layer_handle, long scale_denominator) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CBaseLayerOvl pointer
   CBaseLayerOvl *pBaseLayerOverlay = dynamic_cast<CBaseLayerOvl *>(overlay);
   if (pBaseLayerOverlay == NULL)
      return FAILURE;

   return pBaseLayerOverlay->set_threshold(MapScale(scale_denominator));
}

long Layer::SetGroupThreshold(long layer_handle, long group_handle, 
                              long scale_denominator) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->set_group_threshold(group_handle, 
      MapScale(scale_denominator));
}

long Layer::SetObjectThreshold(long layer_handle, long object_handle, 
                               long scale_denominator) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;
   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->set_object_threshold(object_handle, 
      MapScale(scale_denominator));
}

long Layer::DeleteGroup(long layer_handle, long group_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;
   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->delete_group(group_handle);
}

long Layer::OpenOverlay(long type, LPCTSTR filespec) 
{
   int result = FAILURE;

   GUID overlayDescGuid = g_map_server_util.TypeToOverlayGuid(type);
   
   // determine whether the class name is that of a static or file 
   // overlay and call appropriate function
   if (OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid))
      result = OpenStaticOverlay(overlayDescGuid);
   else if (OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid))
      result = OpenFileOverlay(overlayDescGuid, filespec);
       
   return result;
}

long Layer::CloseOverlay(long type, LPCTSTR filespec) 
{
   int result = FAILURE;

   GUID overlayDescGuid = g_map_server_util.TypeToOverlayGuid(type);
   
   // determine whether the class name is that of a static or file 
   // overlay and call appropriate function
   if (OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid))
      result = CloseStaticOverlay(overlayDescGuid);
   else if (OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid))
      result = CloseFileOverlay(overlayDescGuid, filespec);
   
   return result;
}

long Layer::MoveLayerAbove(long layer_handle, long overlay_handle) 
{
   return OVL_get_overlay_manager()->move_above(layer_handle, overlay_handle);
}

long Layer::MoveLayerBelow(long layer_handle, long overlay_handle) 
{
   return OVL_get_overlay_manager()->move_below(layer_handle, overlay_handle);
}

long Layer::GetOverlayInfo(long overlay_handle, long FAR* type, BSTR FAR* filespec) 
{
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(overlay_handle);

   // if the overlay handle is invalid
   if (overlay == NULL)
   {
      ERR_report("The given overlay handle is invalid.");
      return FAILURE;
   }

   // special case for plug-in overlays.  Right now the same constant is returned for any plug-in overlay.  It would
   // be nice if we could change the behavior to return CLIENT_LAYER_EDITOR0 + ((CLayerOvl *)overlay)->get_editor_num()
   if (dynamic_cast<CBaseLayerOvl *>(overlay) != NULL)
      *type = CLIENT_OVL_LAYEREDITOR;
   else
   {
      // first, check to see if this is a known plug-in overlay (to preserve backwards compatibility)
      *type = g_map_server_util.OverlayGuidToType(overlay->get_m_overlayDescGuid());

      // if the type is unknown, then we will simply return the constant for a new plug-in overlay
      if (*type == -1 && dynamic_cast<COverlayCOM *>(overlay) != NULL)
         *type = CLIENT_OVL_PLUGIN_OVERLAY;
   }

   // set the filespec if the overlay is not a static overlay
   OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
   if (pFvOverlayPersistence != NULL)
   {
      pFvOverlayPersistence->get_m_fileSpecification(filespec);
   }
   else
      *filespec = _bstr_t("").Detach();

   return SUCCESS;
}

long Layer::GetOverlayList(VARIANT FAR* handle_lst, long FAR* overlay_count) 
{
   CList <C_overlay *, C_overlay *> overlay_list;
   OVL_get_overlay_manager()->get_overlay_list(overlay_list);

   COleSafeArray handle_safearray;
   DWORD num_elements = overlay_list.GetCount();

   // create the overlay handle safe array
   handle_safearray.Create(VT_I4, 1, &num_elements);

   // fill the handle array
   POSITION position = overlay_list.GetHeadPosition();
   for (long i=0;i<overlay_list.GetCount();i++)
   {
      C_overlay *overlay = overlay_list.GetNext(position);

      long handle = OVL_get_overlay_manager()->get_overlay_handle(overlay);
      handle_safearray.PutElement(&i, &handle);
   }

   // set the type list
   *handle_lst = handle_safearray.Detach();
   
   // set the number of overlays
   *overlay_count = num_elements;
      
   return SUCCESS;
}

// GetActiveOverlay - returns the overlay handle being edited.  If not in
// an edit mode then -1 is returned, otherwise returns a handle to the 
// topmost overlay
long Layer::GetActiveOverlay() 
{
   return OVL_get_overlay_manager()->get_active_overlay_handle();
}

long Layer::AddDotToSymbol(long symbol_handle, long x, long y) 
{
   OvlSymbol *symbol = lookup_symbol(symbol_handle);

   // make sure we get a valid pointer
   if (symbol == NULL)
      return FAILURE;

   return symbol->add_dot(x, y);
}

long Layer::AddLineToSymbol(long symbol_handle, long x1, long y1, long x2, long y2) 
{
   OvlSymbol *symbol = lookup_symbol(symbol_handle);

   // make sure we get a valid pointer
   if (symbol == NULL)
      return FAILURE;

   return symbol->add_line(x1, y1, x2, y2);
}

long Layer::AddCircleToSymbol(long symbol_handle, long center_x, long center_y, 
                              long radius) 
{
   OvlSymbol *symbol = lookup_symbol(symbol_handle);

   // make sure we get a valid pointer
   if (symbol == NULL)
      return FAILURE;

   return symbol->add_circle(center_x, center_y, radius);
}

long Layer::AddSymbol(long layer_handle, long symbol_handle, 
                      double latitude, double longitude, 
                      double scale_factor, double rotation) 
{
   if (!GEO_valid_degrees(latitude, longitude))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // get a pointer to the symbol associated with the given symbol handle
   OvlSymbol *symbol = lookup_symbol(symbol_handle);

   // make sure we get a valid pointer
   if (symbol == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_symbol(symbol, latitude, longitude,
      scale_factor, rotation);
}

long Layer::MoveSymbol(long layer_handle, long object_handle, double latitude, 
                       double longitude, double rotation) 
{
   if (!GEO_valid_degrees(latitude, longitude))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // move an object to a new location.  Trying to move something other than
   // an symbol will return FAILURE
   return overlay->GetOvlElementContainer()->move_symbol(object_handle, latitude, 
      longitude, rotation);
}

long Layer::CreateSymbol(LPCTSTR symbol_name, short FAR* symbol_exists) 
{
   int symbol_handle;
   
   // if there is already a symbol with the given symbol name then return
   // the symbol handle and set symbol_exists to 1
   if ( (symbol_handle = lookup_symbol_name(symbol_name)) != -1)
   {
      *symbol_exists = 1;
      return symbol_handle;
   }
   // Otherwise, the symbol doesn't already exist.  It needs to be created
   else
      *symbol_exists = 0;
   
   // create the symbol and add it to the symbol map
   OvlSymbol *new_symbol = new OvlSymbol;
   new_symbol->set_name(symbol_name);
   symbol_handle = m_next_symbol_handle++;
   m_symbol_map.SetAt(symbol_handle, new_symbol);
   
   return symbol_handle;
}

long Layer::MoveIcon(long layer_handle, long object_handle, double lat, 
   double lon, long rotation) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // move an object to a new location.  Trying to move something other than
   // an icon will return FAILURE
   return overlay->GetOvlElementContainer()->move_icon(object_handle, lat, lon, rotation);
}

long Layer::AddIconToSet(long layer_handle, long icon_set_handle, 
                         long rotation, LPCTSTR filename) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_icon_to_set(icon_set_handle, rotation,
      filename);
}

long Layer::AddIconEx(long layer_handle, long icon_set_handle, 
                      double lat, double lon, long rotation, 
                      LPCTSTR icon_text) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // add an icon to the layer with the given filename.  The icon's 
   // text will use the current font
   return overlay->GetOvlElementContainer()->add_icon("", lat, lon, icon_text, 
      icon_set_handle, rotation);
}

long Layer::CreateIconSet(long layer_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // create a new icon set and return its handle.  Will return
   // -1 if unable to create a new set
   return overlay->GetOvlElementContainer()->create_icon_set();
}

long Layer::ShowMainToolbar(BOOL show) 
{
   return SUCCESS;
}

long Layer::ShowEditorToolbar(BOOL show) 
{
   return SUCCESS;
}

long Layer::ShowStatusBar(BOOL show) 
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("ShowStatusBar : unable to get CMainframe pointer");
      return FAILURE;
   }

   return pFrame->ShowStatusBar(show);
}

long Layer::SetupToolbarButton(long toolbar_handle, long button_number, 
                               LPCTSTR tooltip_text, LPCTSTR status_bar_text) 
{
   return fvw_get_frame()->setup_client_toolbar_button(toolbar_handle,
      button_number, tooltip_text, status_bar_text);
}

long Layer::RemoveToolbar(long toolbar_handle) 
{
   return fvw_get_frame()->remove_client_toolbar(toolbar_handle);
}

long Layer::SetIsTimeSensitive(long layer_handle, BOOL enabled) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CBaseLayerOvl pointer
   CBaseLayerOvl *pBaseLayerOvl = dynamic_cast<CBaseLayerOvl *>(overlay);
   if (pBaseLayerOvl == NULL)
      return FAILURE;
   
   // set the time sensitive flag for the overlay if changed
   if (pBaseLayerOvl->get_view_time_enabled() != enabled)
   {
      pBaseLayerOvl->set_view_time_enabled(enabled);
      CMainFrame::GetPlaybackDialog().update_interval();
   }

   return SUCCESS;
}

long Layer::ModifyObject(long layer_handle, long object_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // modify an object with the currently set properties (pen, fill type, and font)
   return overlay->GetOvlElementContainer()->modify_object(object_handle);
}

long Layer::DeleteAllObjects(long layer_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // delete all the objects in the overlay
   return overlay->GetOvlElementContainer()->delete_all_objects();
}

long Layer::ShowLayer(long layer_handle, BOOL show_layer) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;
   
   // show/hide the layer
   boolean_t bPreviouslyVisible = overlay->get_m_bIsOverlayVisible();
   overlay->put_m_bIsOverlayVisible(show_layer);

   // invalidate all
   MapView *pView = fvw_get_view();
   if (pView != NULL)
      pView->PostMessage(WM_INVALIDATE_LAYER_OVERLAY, 0, -1);  

   // Returns nonzero if the layer was previously visible, 0 if the layer was previously hidden
   return bPreviouslyVisible;
}

long Layer::SetPen(long layer_handle, short fg_red, short fg_green, short fg_blue, 
                   short bg_red, short bg_green, short bg_blue, 
                   BOOL turn_off_background, long line_width, long line_style) 
{
   // make sure all of the color values are within range
   if (fg_red < 0 || fg_red > 255 ||
      fg_green < 0 || fg_green > 255 ||
      fg_blue < 0 || fg_blue > 255 ||
      bg_red < 0 || bg_red > 255 ||
      bg_green < 0 || bg_green > 255 ||
      bg_blue < 0 || bg_blue > 255)
   {
      ERR_report("[Layer::SetPen] color value out of range");
      return FAILURE;
   }
   
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->set_pen(
      RGB(fg_red, fg_green, fg_blue), RGB(bg_red, bg_green, bg_blue),
      turn_off_background, line_width, line_style);
}

long Layer::AddGeoRect(long layer_handle, double nw_lat, double nw_lon, double se_lat, double se_lon) 
{
   if (!GEO_valid_degrees(nw_lat, nw_lon))
      return FAILURE;

   if (!GEO_valid_degrees(se_lat, se_lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;   

   // make sure the longitudes aren't exactly equal, otherwise we will be 
   // drawing a degenerate rectangle
   if (nw_lon == se_lon)
      se_lon += 0.00000001;
   
   return overlay->GetOvlElementContainer()->add_geo_rect(nw_lat, nw_lon, se_lat, se_lon);
}

long Layer::AddArc(long layer_handle, double lat, double lon, double radius_m, 
                   double heading_to_start, double turn_arc, BOOL clockwise) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay assoicated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;   
   
   return overlay->GetOvlElementContainer()->add_arc(lat, lon, radius_m,
      heading_to_start, turn_arc, clockwise);
}

long Layer::RegisterWithMapServer(LPCTSTR client_name, long window_handle, 
                                  const VARIANT FAR& dispatch_pointer)
{
   try
   {
      IDispatch* pDisp = _variant_t(dispatch_pointer);

      IDispatch* old_pdisp = m_pdisp;

      // set the client name of this layer
      m_client_name = client_name;

      // set the window handle of the client
      m_client_window_handle = (HWND) window_handle;

      // a client cannot create a layer until they have registed with 
      // the map server.  This will let the CreateLayer(Ex) function that
      // this layer has been registerd
      m_client_registered_with_server = TRUE;

      // get and store a pointer to the IDispatch pointer of the client's
      // callback object
      m_pdisp = pDisp;
      m_owning_thread_id = ::GetCurrentThreadId();
      OVL_get_overlay_manager()->add_dispatch_ptr(m_pdisp);

      // clean up old dispatch interface in case RegisterWithMapServer was already called
      remove_dispatch_ptr(old_pdisp);

      return SUCCESS;
   }
   catch (_com_error &)
   {
      CString msg;
      msg.Format("ILayer::RegisterWithMapServer failed - dispatch_pointer must "
            "be of type VT_DISPATCH. Passed in type was %d. Client name = %s.",
            dispatch_pointer.vt, client_name);
      ERR_report(msg);
      return FAILURE;
   }
}

long Layer::CreateLayer(LPCTSTR layer_name) 
{
   return CreateLayerEx(layer_name, "");
}

long Layer::CreateLayerEx(LPCTSTR layer_name, LPCTSTR icon_name) 
{
   // check to see that client registered with the server
   if (!m_client_registered_with_server)
   {
      ERR_report("CreateLayer failed because the client was not registered with the map server");
      return FAILURE;
   }
   
   // FAILURE must be less than 0 otherwise it might be interpreted 
   // as a valid return handle
   ASSERT (FAILURE <0);

   CComObject<CStaticLayerOvl> *overlay;
   CComObject<CStaticLayerOvl>::CreateInstance(&overlay);
   overlay->AddRef();
   overlay->Setup(m_owning_thread_id, layer_name, m_pdisp, icon_name, nullptr);

   overlay->Initialize(FVWID_Overlay_MapServer);
   
   // add to overlay manager
   if (OVL_get_overlay_manager()->add_overlay(overlay, FALSE) != SUCCESS)
   {
      ERR_report("CreateLayer failed");
      return FAILURE;
   }
   
   // return the layer's handle
   return OVL_get_overlay_manager()->get_overlay_handle(overlay);
}

long Layer::SetAbsoluteTime(long layer_handle, DATE date_time) 
{
   // get a pointer to the overlay assoicated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;
   
   // make sure this is a CLayerOvl pointer
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL)
      return FAILURE;

   // update the view time dialog
   CMainFrame::GetPlaybackDialog().on_new_absolute_time(date_time);

   return SUCCESS;
}

long Layer::CreateOverlay(long type) 
{
   // TODO: Add your dispatch handler code here
   long result = FAILURE;

   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   C_overlay *overlay;

   // get the class name given the overlay type
   GUID overlayDescGuid = g_map_server_util.TypeToOverlayGuid(type);
   OverlayTypeDescriptor* pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc == NULL)
   {
      CString msg;
      msg.Format("CreateOverlay: could not find overlay type %d", type);
      ERR_report(msg);
      return FAILURE;
   }

   // determine whether the class name is that of a static or file 
   // overlay and call appropriate function

   if (OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid))
   {
      result = OpenStaticOverlay(overlayDescGuid);
      overlay = ovl_mgr->get_first_of_type(overlayDescGuid);
   }
   else
   {
      // this hack is used to set the specification of an overlay created via the collaboration subscription dialog
      //
      _bstr_t UNC_spec;
      VARIANT_BOOL found = VARIANT_FALSE;
      CMainFrame* frame = fvw_get_frame();
      if ( frame && frame->m_pCollaborationServer ) 
      {
         frame->m_pCollaborationServer->GetCookie(0,UNC_spec.GetAddress(),&found);
         frame->m_pCollaborationServer->DeleteCookie(0);
      }

      if ( found == VARIANT_TRUE ) {
         CList<C_overlay *, C_overlay *> list;
         ovl_mgr->get_overlay_list(list);
         POSITION position = list.GetHeadPosition();
         while (position) {
            C_overlay *overlay2 = list.GetNext(position);
            OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay2);
            if (pFvOverlayPersistence != NULL) {

               _bstr_t fileSpecification;
               pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

               CFileSpec fs1( fileSpecification );
               CFileSpec fs2( UNC_spec );
               CString fn1 = fs1.GetFileName();
               CString fn2 = fs2.GetFileName();
               fn1.MakeLower(); fn2.MakeLower();
               if ( fn1 == fn2 ) {
                  // compare file names
                  overlay2->set_modified(FALSE);// Don't ask about the "save changes"
                  ovl_mgr->close( overlay2 );  // remove the previous one we found
                  break;
               }
            }
         } 
      }

      result = ovl_mgr->create(pOverlayTypeDesc->overlayDescriptorGuid, &overlay);
      if (result == SUCCESS)
      {
         ovl_mgr->set_mode(pOverlayTypeDesc->overlayDescriptorGuid);

         // hack for collaboration subscription dialog
         if ( found == VARIANT_TRUE ) 
         {
            //Didn't work
            //IFvOverlayPersistence *pFvOverlayPersistence = dynamic_cast<IFvOverlayPersistence*>(overlay);
            //if (pFvOverlayPersistence )
            //{
            //   pFvOverlayPersistence->put_m_fileSpecification( UNC_spec );
            //}

            CFvOverlayPersistenceImpl *pFvOverlayPersistence = dynamic_cast<CFvOverlayPersistenceImpl *>(overlay);
            if (pFvOverlayPersistence == NULL)
            {
               CFileOverlayCOM *pFOC = dynamic_cast<CFileOverlayCOM*>(overlay);
               if (pFOC)
               {
                  //CString strUNCPath( (char*)UNC_spec );
                  //CFileSpec fs( strUNCPath );
                  //CString strLocalPath = fs.GetFullSpec();
                  //INT i = strLocalPath.Find("\\noshare\\");
                  //if ( i != -1 )
                  //   strLocalPath = strLocalPath.Mid(i+strlen("\\noshare\\"));
                  //pFOC->FileSaveAs( _bstr_t(strLocalPath),0);

                  pFOC->FileSaveAs( UNC_spec, -1 );
               }
            }
            else
            {
               pFvOverlayPersistence->set_specification( (char*)UNC_spec );
            }
         }
      }
   }

   // now get the overlay handle for this overlay
   if (result == SUCCESS)
      return ovl_mgr->get_overlay_handle(overlay);

   // return failure if the overlay wasn't created sucessfully
   return FAILURE;
}

long Layer::CloseOverlayEx(long overlay_handle) 
{
   // TODO: Add your dispatch handler code here
   int result = FAILURE;

   // Need to check preconditions on the overlay handle

   // Get the overlay manager
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   // get the overlay from the handle
   C_overlay* overlay = ovl_mgr->lookup_overlay(overlay_handle);

   // if a valid overlay
   if ( overlay )
   {
      // close it
      if ( ovl_mgr->close(overlay) == SUCCESS )
         result = SUCCESS;
   }

   return result;
}

long Layer::SetToolbarButtonState(long toolbar_handle, long button_number, short button_down) 
{
   // set the status of a client's toolbar's button (up=0, down=1)
   return fvw_get_frame()->set_client_toolbar_button_state(toolbar_handle, button_number, 
      button_down);
}

long Layer::SetObjectData(long layer_handle, long object_handle, long object_data) 
{
   // get a pointer to the overlay assoicated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;
  
   // set the object data
   return overlay->GetOvlElementContainer()->set_object_data(object_handle, object_data);
}

long Layer::GetObjectData(long layer_handle, long object_handle, 
   long FAR* object_data) 
{
   // get a pointer to the overlay assoicated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;
   
   // retrieve the object data
   return overlay->GetOvlElementContainer()->get_object_data(object_handle, object_data);
}

long Layer::SetPlaybackRate(long playback_rate) 
{
   if (playback_rate < 1 || playback_rate > 99)
      return FAILURE;

   CMainFrame::GetPlaybackDialog().SetPlaybackRate(playback_rate);
   return SUCCESS;
}

long Layer::StartPlayback() 
{
   if (!CMainFrame::GetPlaybackDialog().in_playback())
      CMainFrame::GetPlaybackDialog().toggle_state();

   return SUCCESS;
}

long Layer::StopPlayback() 
{
   if (CMainFrame::GetPlaybackDialog().in_playback())
      CMainFrame::GetPlaybackDialog().toggle_state();

   return SUCCESS;
}

long Layer::PlaybackBegin() 
{
   CMainFrame::GetPlaybackDialog().Reset();
   return SUCCESS;
}

long Layer::PlaybackEnd() 
{
   CMainFrame::GetPlaybackDialog().Endset();
   return SUCCESS;
}

long Layer::AddEditorButton(long layer_handle, LPCTSTR button_text, 
   LPCTSTR bitmap_filename, LPCTSTR class_ID_string) 
{
   // method depreciated
   return FAILURE;
}

long Layer::SetModified(long layer_handle, short modified) 
{
   // get a pointer to the overlay assoicated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;
   
   // set the object data
   overlay->set_modified(modified);
   return SUCCESS;
}

long Layer::AddToolbar(LPCTSTR filename, long num_buttons, LPCTSTR toolbar_name, 
   short initial_position, const VARIANT FAR& separator_lst, 
   long num_separators) 
{
   if (!m_client_registered_with_server)
      return FAILURE;

   long *separator_data;
   SafeArrayAccessData(separator_lst.parray, (void HUGEP**)&separator_data);
  
   long ret = fvw_get_frame()->add_client_toolbar(AfxGetMainWnd(), filename, 
      num_buttons, m_pdisp, (long)m_client_window_handle,
      toolbar_name, initial_position, separator_data, num_separators);

   SafeArrayUnaccessData(separator_lst.parray);

   return ret;
}

long Layer::RemoveEditorButton(long layer_handle) 
{
   // method depreciated
   return FAILURE;
}

long Layer::AddLineWithEmbeddedText(long layer_handle, double lat1, 
   double lon1, double lat2, 
   double lon2, long x_offset, 
   long y_offset, LPCTSTR embedded_text) 
{
   if (!GEO_valid_degrees(lat1, lon2))
      return FAILURE;

   if (!GEO_valid_degrees(lat2, lon2))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_line(lat1, lon1, lat2, lon2, 
      x_offset, y_offset, embedded_text);
}

long Layer::AddMilStd2525Symbol(long layer_handle, double lat, double lon, 
   LPCTSTR symbol_id) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->add_milstd2525_symbol(lat, lon, symbol_id);

}

long Layer::SetTextRotation(long layer_handle, double rotation_degrees) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   return overlay->GetOvlElementContainer()->set_text_rotation(rotation_degrees);
}

// returns 0 if failure or overlay is not modified, 1 otherwise
long Layer::GetModified(long layer_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return 0;

   return overlay->is_modified();
}

long Layer::SaveOverlay(long layer_handle) 
{
   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);

   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   return OVL_get_overlay_manager()->save(overlay);
}

long Layer::AddIconOffset(long layer_handle, LPCTSTR filename, double lat, double lon, 
   LPCTSTR icon_text, long x_offset, long y_offset) 
{
   if (!GEO_valid_degrees(lat, lon))
      return FAILURE;

   // get a pointer to the overlay associated with the given handle
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   //make sure this is a CBaseLayerOvl or MovingMap(C_gps_trail)
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL && overlay->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;


   // add an icon to the layer with the given filename.  The icon's 
   // text will use the current font
   return overlay->GetOvlElementContainer()->add_icon(filename, lat, lon, icon_text, -1, 0, 
      x_offset, y_offset);
}
