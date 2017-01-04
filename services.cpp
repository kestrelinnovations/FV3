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



// services.cpp : implementation file
//

#include "stdafx.h"
#include "services.h"
#include "getobjpr.h"
#include "..\Common\map_server_client.h"
#include "mapx.h"
#include "mov_sym.h" // moving symbol (AddMovableSymbol() ...)
#include "wm_user.h"
#include "refresh.h"    // FVW_update_title_bar
#include "gps.h"
#include "IMap.h"  // for get_block_dted_in_feet
#include "OvlFctry.h"
#include "MouseClickResource.h"
#include "maps.h"
#include "MapEngineCOM.h"

// includes for overlay factories
#include "factory.h"
#include "FctryLst.h"
#include "ar_edit\factory.h"
#include "PrintToolOverlay\factory.h"
#include "catalog\factory.h"
#include "SkyViewOverlay\factory.h"
#include "pointex\factory.h"
#include "scalebar\factory.h"
#include "grid_map\factory.h"
#include "contour\factory.h"
#include "TAMask\factory.h"
#include "nitf\factory.h"
#include "shp\factory.h"
#include "MovingMapOverlay\factory.h"
#include "TacticalModel\factory.h"


#include "..\FvCore\Include\GuidStringConverter.h"

/*
 *  needs the static appname macro - i.e. PROJECT_APP_NAME
 */
#include "fvw\appname.h"

MapServerUtil g_map_server_util;

// get a map type (source, scale, series) from client category and client scale.
// Returns SUCCESS/FAILURE.
static int from_client(int client_category, int client_scale, 
   MapSource &source, MapScale &scale, MapSeries &series);

// get a client category and client scale from a map type (source, scale, 
// series).  Returns SUCCESS/FAILURE.
static int to_client(const MapSource &source, 
   const MapScale &scale, const MapSeries &series, 
   int &client_category, int &client_scale);

int map_server::m_num_clients = 0;

/////////////////////////////////////////////////////////////////////////////
// map_server

IMPLEMENT_DYNCREATE(map_server, CCmdTarget)

// {248C0FC1-4E71-11CE-A650-02608C3F42B7}
   IMPLEMENT_OLECREATE(map_server, PROJECT_APP_NAME, 0x248c0fc1,
     0x4e71, 0x11ce, 0xa6, 0x50, 0x02, 0x60, 0x8c, 0x3f, 0x42, 0xb7);

static const IID IID_IMap_server =
{ 0x248C0FC0, 0x4E71, 0x11CE, { 0xA6, 0x50, 0x02, 0x60, 0x8C, 0x3F, 0x42, 0xB7 } };

BEGIN_INTERFACE_MAP(map_server, CCmdTarget)
   INTERFACE_PART(map_server, IID_IMap_server, Dispatch)
END_INTERFACE_MAP()

map_server::map_server()
{
   EnableAutomation();
   increment_num_clients();

   m_next_overlay_handle=1;
   m_next_gps_handle=1;
   m_client_registered_with_server = FALSE; //client has not called RestisterWithMapServer
   m_client_window_handle=0;

   CComObject<CMapServerCallback>::CreateInstance(&m_callback);
   m_callback->AddRef();

   m_callback->Initialize(this);
}  

map_server::map_server(bool no_automation)
   : m_callback(NULL)
{
}

map_server::~map_server()
{
   POSITION pos;
   int handle;
   C_map_server_ovl *overlay;

   decrement_num_clients();  //decrement the number of OLE Map Server clients

   //TO DO: kevin: do we want to delete all of the GPS trails created?

   //delete all overlays and remove from overlay manager
   pos = m_overlay_map.GetStartPosition();
   while (pos)
   {
      m_overlay_map.GetNextAssoc(pos, handle, overlay);
      if (OVL_get_overlay_manager()->delete_overlay(overlay, FALSE) != SUCCESS)
      {
         ERR_report("delete_overlay() failed.");
      }

      overlay->Release();
   }

   if (NULL != m_callback)
      m_callback->Release();
}

void map_server::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  This implementation deletes the 
   // object.  Add additional cleanup required for your object before
   // deleting it from memory.
   delete this;

   // if ui is hidden - you want to exit when there are no clients
   if (fvw_get_app()->is_gui_hidden() && map_server::get_num_clients() == 0)
   {
      CMainFrame *frame = fvw_get_frame();
      if (frame)
         frame->PostMessage(WM_CLOSE, 0, 0);
   }
}


BEGIN_MESSAGE_MAP(map_server, CCmdTarget)
   //{{AFX_MSG_MAP(map_server)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(map_server, CCmdTarget)
   //{{AFX_DISPATCH_MAP(map_server)
   DISP_FUNCTION(map_server, "CreateLayer", CreateLayer, VT_I4, VTS_BSTR)
   DISP_FUNCTION(map_server, "DeleteLayer", DeleteLayer, VT_I4, VTS_I4)
   DISP_FUNCTION(map_server, "RegisterLayer", RegisterLayer, VT_I4, VTS_I4)
   DISP_FUNCTION(map_server, "SetMapDisplay", SetMapDisplay, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(map_server, "Refresh", Refresh, VT_I4, VTS_NONE)
   DISP_FUNCTION(map_server, "AddEllipse", AddEllipse, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4)
   DISP_FUNCTION(map_server, "AddPredefinedSymbol", AddPredefinedSymbol, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_I4 VTS_I4)
   DISP_FUNCTION(map_server, "AddText", AddText, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(map_server, "DeleteObject", DeleteObject, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(map_server, "AddLine", AddLine, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4)
   DISP_FUNCTION(map_server, "RegisterWithMapServer", RegisterWithMapServer, VT_I4, VTS_BSTR VTS_I4)
   DISP_FUNCTION(map_server, "GetElevation", GetElevation, VT_I4, VTS_R8 VTS_R8 VTS_PI4)
   DISP_FUNCTION(map_server, "GetMapDimensions", GetMapDimensions, VT_I4, VTS_PI4 VTS_PI4)
   DISP_FUNCTION(map_server, "SetObjectTooltip", SetObjectTooltip, VT_I4, VTS_I4 VTS_I4 VTS_BSTR)
   DISP_FUNCTION(map_server, "SetObjectComment", SetObjectComment, VT_I4, VTS_I4 VTS_I4 VTS_BSTR)
   DISP_FUNCTION(map_server, "SetObjectHelpText", SetObjectHelpText, VT_I4, VTS_I4 VTS_I4 VTS_BSTR)
   DISP_FUNCTION(map_server, "GetDegreesPerPixel", GetDegreesPerPixel, VT_I4, VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(map_server, "GetMapDisplay", GetMapDisplay, VT_I4, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PI4 VTS_PI4 VTS_PI4)
   DISP_FUNCTION(map_server, "OpenOverlay", OpenOverlay, VT_I4, VTS_I4 VTS_BSTR)
   DISP_FUNCTION(map_server, "CloseOverlay", CloseOverlay, VT_I4, VTS_I4 VTS_BSTR)
   DISP_FUNCTION(map_server, "CreateLayerEx", CreateLayerEx, VT_I4, VTS_BSTR VTS_BSTR)
   DISP_FUNCTION(map_server, "CAPSPrint", CAPSPrint, VT_I4, VTS_BSTR VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(map_server, "CreateGPSTrail", CreateGPSTrail, VT_I4, VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(map_server, "AddGPSPoint", AddGPSPoint, VT_I4, VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_I2 VTS_I2 VTS_I2 VTS_I2)
   DISP_FUNCTION(map_server, "GetBlockDTEDinFeet", GetBlockDTEDinFeet, VT_ERROR, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I2 VTS_I2 VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(map_server, "GetMouseClick", GetMouseClick, VT_ERROR, VTS_I4 VTS_PI4)
   DISP_FUNCTION(map_server, "CreateMap", CreateMap, VT_I4, VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I4 VTS_R8 VTS_I2 VTS_I4 VTS_I4 VTS_I2 VTS_PI2 VTS_PVARIANT VTS_PVARIANT)
   DISP_FUNCTION(map_server, "AddMovableSymbol", AddMovableSymbol, VT_I4, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(map_server, "DeleteMovableSymbol", DeleteMovableSymbol, VT_I4, VTS_I4 VTS_I4)
   DISP_FUNCTION(map_server, "MoveMovableSymbol", MoveMovableSymbol, VT_I4, VTS_I4 VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
   DISP_FUNCTION(map_server, "RequestLayerNotification", RequestLayerNotification, VT_I4, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(map_server, "GetLatLonPositionInMap", GetLatLonPositionInMap, VT_I4, VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(map_server, "CreateMapFromGeoBounds", CreateMapFromGeoBounds, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I2 VTS_I2 VTS_PI2 VTS_PI4 VTS_PI4 VTS_PVARIANT VTS_PVARIANT)
   DISP_FUNCTION(map_server, "CreateFixedSizeMapFromGeoBounds", CreateFixedSizeMapFromGeoBounds, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I2 VTS_I2 VTS_I4 VTS_I4 VTS_PI2 VTS_PVARIANT VTS_PVARIANT)
   DISP_FUNCTION(map_server, "CalcPixmapGeoCorners", CalcPixmapGeoCorners, VT_I4, VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I4 VTS_I4 VTS_I4 VTS_PVARIANT VTS_I2 VTS_PI2)
   DISP_FUNCTION(map_server, "CalcPixmapSizeFromGeoBounds", CalcPixmapSizeFromGeoBounds, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_PI4 VTS_PI4 VTS_PVARIANT VTS_I2 VTS_PI2)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// map_server message handlers

//lookup the handle in the overlay map and return a pointer to the overlay
C_map_server_ovl* map_server::LookupOverlay(long handle) const
{
   C_map_server_ovl* overlay = NULL;

   //lookup overlay
   if (m_overlay_map.Lookup(handle,overlay))
   {       
      C_overlay *pOverlay = OVL_get_overlay_manager()->get_first();
      while (pOverlay != NULL)
      {
         if (pOverlay == overlay && pOverlay->get_m_overlayDescGuid() == FVWID_Overlay_MapServer)
            return static_cast<C_map_server_ovl *>(pOverlay);

         pOverlay = OVL_get_overlay_manager()->get_next(pOverlay);
      }
   }

   return NULL;
}

/*
 *  Returns the handle (a handle is >= 0).
 *  If the layer could not be created, -1 is returned.
 */

long map_server::CreateLayer(LPCTSTR layer_name) 
{
   return CreateLayerEx(layer_name, "");
}


long map_server::DeleteLayer(long handle) 
{
   C_map_server_ovl *overlay = LookupOverlay(handle);
   if (!overlay)
      return FAILURE;

   //remove from overlay manager - screen is not invalidated
   if (OVL_get_overlay_manager()->delete_overlay(overlay, FALSE) != SUCCESS)
   {
      //we're in trouble if this fails (i.e., we got here)
      ASSERT(0);
      ERR_report("OVL_get_overlay_manager()->delete_overlay() failed");
   }

   m_overlay_map.RemoveKey(handle);
   
   overlay->Release();

   // refresh the screen
   OVL_get_overlay_manager()->invalidate_all();

   // finally, update the title bar to reflect changes
   FVW_update_title_bar();
   
   return SUCCESS;
}

// OBSELETE
long map_server::RegisterLayer(long handle) 
{
   return SUCCESS;
}

//refresh screen
long map_server::Refresh() 
{
   OVL_get_overlay_manager()->invalidate_all(FALSE);

   return SUCCESS;
}

//delete the object "object_handle" in layer "layer_handle"
long map_server::DeleteObject(long layer_handle, long object_handle)
{
   C_map_server_ovl *overlay = LookupOverlay(layer_handle);
   if (!overlay)
      return FAILURE;

   return overlay->delete_object(object_handle);
}


//add a line to layer "handle" from (lat1,lon1) to (lat2,lon2) in the specified color
long map_server::AddLine(long handle, double lat1, double lon1, double lat2, double lon2, long color) 
{
   // check for valid lat-lon
   if (!GEO_valid_degrees(lat1, lon1) || !GEO_valid_degrees(lat2, lon2))
   {
      CString msg((LPCSTR) IDS_INVALID_LAT_LON);
      ERR_report(msg);
      return FAILURE;
   }
   
   C_map_server_ovl *overlay = LookupOverlay(handle);
   if (!overlay)
      return FAILURE;

   return overlay->add_line(lat1, lon1, lat2, lon2, translate_client_color(color));
}


long map_server::AddEllipse(long handle, double lat, double lon, double vertical, 
   double horizontal, double rotation, long color) 
{
   // check for valid lat-lon
   if (!GEO_valid_degrees(lat, lon))
   {
      CString msg((LPCSTR) IDS_INVALID_LAT_LON);
      ERR_report(msg);
      return FAILURE;
   }
   
   C_map_server_ovl *overlay = LookupOverlay(handle);
   if (!overlay)
      return FAILURE;

   return overlay->add_ellipse(lat, lon, vertical, 
     horizontal, rotation, translate_client_color(color), 0, 0);
}


long map_server::AddPredefinedSymbol(long layer_handle, double lat, double lon,
   long symbol, long color) 
{
   // check for valid lat-lon
   if (!GEO_valid_degrees(lat, lon))
   {
      CString msg((LPCSTR) IDS_INVALID_LAT_LON);
      ERR_report(msg);
      return FAILURE;
   }

   C_map_server_ovl *overlay = LookupOverlay(layer_handle);
   if (!overlay)
      return FAILURE;

   return (long) overlay->add_predefined_symbol(lat, lon, symbol, 
      translate_client_color(color));
}


long map_server::AddText(long layer_handle, double lat, double lon, 
   LPCTSTR text, long color, long x_offset, long y_offset) 
{
   // check for valid lat-lon
   if (!GEO_valid_degrees(lat, lon))
   {
      CString msg((LPCSTR) IDS_INVALID_LAT_LON);
      ERR_report(msg);
      return FAILURE;
   }

   C_map_server_ovl *overlay = LookupOverlay(layer_handle);
   if (!overlay)
      return FAILURE;

   if (text!=NULL)
      return overlay->add_text(lat, lon, text, translate_client_color(color),
         x_offset, y_offset);
   else
      return FAILURE;
}

static int client_categories[] = {CLIENT_BLANK_CATEGORY, CLIENT_VECTOR_CATEGORY,
   CLIENT_RASTER_CATEGORY, CLIENT_DTED_CATEGORY, CLIENT_VMAP_CATEGORY, 
   CLIENT_DNC_CATEGORY, CLIENT_WVS_CATEGORY, -1};

static const MapCategory &get_category(int index)
{ 
   static MapCategory categories[] = {BLANK_CATEGORY, VECTOR_CATEGORY, 
      RASTER_CATEGORY, DTED_CATEGORY, VMAP_CATEGORY, DNC_CATEGORY, WVS_CATEGORY, NULL_CATEGORY};

   if (index < 0 || index > 7)
      return NULL_CATEGORY;

   return categories[index];
}

//convert from clints #define category to map_category_t enum 
MapCategory MapServerUtil::convert_to_map_category_t(int client_category)
{
   int i = 0;

   while (client_categories[i] != -1)
   {
      if (client_categories[i] == client_category)
         return get_category(i);

      i++;
   }

   ASSERT(0);
   return NULL_CATEGORY;
}

int MapServerUtil::convert_to_client_category(MapCategory server_category)
{
   int i = 0;

   while (get_category(i) != NULL_CATEGORY)
   {
      if (get_category(i) == server_category)
         return client_categories[i];

      i++;
   }

   // the given category is not one of the categories defined by the
   // automation interfaces (e.g., "Raster (Legacy)") or it is a 
   // custom category (group)
   return -1;
}

//
// note: Photo 10 meter cannot be displayed by using one of the scales other than 
// LARGEST because it is "ADRI_SCALE", we could associate a source and scale with
// each client 
// NOTE: zoom is not used currently
//
long map_server::SetMapDisplay(double lat, double lon, double rotation,
   long client_category, long scale, long zoom, long mask) 
{
   //
   // return FAILURE if there is no MapView (e.g. if in print 
   // preview or if on a modal dialog box before startup)
   //
   CView* view = UTL_get_active_non_printing_view();
   if (view == NULL)
      return FAILURE;
   //
   // initialize the requested map specs with the current map specs
   //
   const ViewMapProj* curr_map = UTL_get_current_view_map(view);
   if (curr_map == NULL)
      return FAILURE;
   if (!curr_map->is_projection_set())
      return FAILURE;

   d_geo_t req_center;
   req_center.lat = curr_map->actual_center_lat();
   req_center.lon = curr_map->actual_center_lon();
   MapSource req_source = curr_map->source();
   MapScale req_scale = curr_map->scale();
   MapSeries req_series = curr_map->series();
   double req_rotation = curr_map->requested_rotation();
   int req_zoom_percent = curr_map->requested_zoom_percent();
   ProjectionEnum proj_type = PROJ_get_default_projection_type();
   
   // If the category is not being set.
   if ((CLIENT_MASK_CATEGORY & mask) == 0)
   {
      // If the scale is not being set either, then we are just going to use
      // the current source, scale, and series.
      if ((CLIENT_MASK_SCALE & mask) == 0)
      {
         client_category = -1;   // not a valid category
      }
      // If the scale is being set, we will fail if a map with the requested
      // scale does not exist in the current category.
      else
      {
         client_category = g_map_server_util.convert_to_client_category(MAP_get_category(req_source));
      }
   }
   else
   {
      // If the client_category is invalid, then lets fail now. client_category
      // must be a valid value, or a -1 to indicate no change in MapType, below
      if (g_map_server_util.convert_to_map_category_t(client_category) == NULL_CATEGORY)
         return FAILURE;

      // If the category is being set without setting the scale, 
      // CLIENT_CLOSEST_SCALE will be assumed
      if ((CLIENT_MASK_SCALE & mask) == 0)
      {
         mask |= CLIENT_MASK_SCALE;
         scale = CLIENT_CLOSEST_SCALE;
      }
   }

   // if lat is being set, set it
   if (CLIENT_MASK_LAT & mask)
      req_center.lat = (degrees_t)lat;

   // if lon is being set, set it
   if (CLIENT_MASK_LON & mask)
      req_center.lon = (degrees_t) lon;

   // if rotation is being set, set it
   if (CLIENT_MASK_ROTATION & mask)
      req_rotation = (double) rotation;

   // if zoom is being set, set it
   if (CLIENT_MASK_ZOOM & mask)
      req_zoom_percent = static_cast<int>(zoom);

   //
   // scale is relative indicates whether the requested scale is a 
   // relative one (i.e. relative to the current map), i.e.
   // CLIENT_CLOSEST_SCALE, CLIENT_SMALLEST_SCALE or CLIENT_LARGEST_SCALE.
   //
   boolean_t scale_is_relative = FALSE;

   // NOTE: the category code, above, makes it so you will get here with
   // the SCALE bit set OR client_category will be -1.  In other words,
   // either both scale and client_category are being used, or neither are
   // being used.

   // if the scale is being set
   if (CLIENT_MASK_SCALE & mask)
   {
      // The relative scales use special functions.  Note when 
      // scale_is_relative is TRUE the client_category will be used to get
      // the map_category_t needed for the special functions.
      if (scale == CLIENT_CLOSEST_SCALE ||
         scale == CLIENT_SMALLEST_SCALE ||
         scale == CLIENT_LARGEST_SCALE)
      {
         scale_is_relative = TRUE;
      }
      // UTL_change_view_map will be used.  We need to get a source, scale,
      // and series from the client_category and scale values.
      else
      {
         if (from_client(client_category, scale, req_source, req_scale, 
            req_series) != SUCCESS)
         {
            ERR_report("from_client() failed.");
            return FAILURE;
         }
      }
   }

   int status;
   
   //
   // if the specified scale is relative to the current scale, then handle
   // these separately
   //
   if (scale_is_relative)
   {
      MapCategory category;
      if (client_category == -1)
         category = MAP_get_category(req_source);
      else
         category = g_map_server_util.convert_to_map_category_t(client_category);

      if (scale == CLIENT_CLOSEST_SCALE)
      {
         status = UTL_change_view_map_to_closest(view, category,
            req_source, req_scale, req_series, req_center.lat, 
            req_center.lon, req_rotation, req_zoom_percent, proj_type);
      }
      else if (scale == CLIENT_SMALLEST_SCALE)
      {
         status = UTL_change_view_map_to_smallest_scale(view, category,
            req_center.lat, req_center.lon, req_rotation, req_zoom_percent, proj_type);
      }
      else if (scale == CLIENT_LARGEST_SCALE)
      {
         status = UTL_change_view_map_to_largest_scale(view, category,
            req_center.lat, req_center.lon, req_rotation, req_zoom_percent, proj_type);
      }
   }
   else
   {
      status = UTL_change_view_map(view, req_source, req_scale, req_series, 
         req_center.lat, req_center.lon, req_rotation, req_zoom_percent,
         proj_type);
   }

   if (status == SUCCESS)
   {
      view->Invalidate(TRUE);
      view->UpdateWindow();
   }
   
   return status;
}


long map_server::RegisterWithMapServer(LPCTSTR client_name, long window_handle) 
{
   m_client_name=client_name;

   //tO DO: kevin: some verification on window handle - the ICD says we may do this in 
   // the future

   m_client_window_handle= (HWND) window_handle;

   m_client_registered_with_server=TRUE;
   
   return SUCCESS;
}


//returns SUCCESS/FAILURE/FV_NODATA
long map_server::GetElevation(double lat, double lon, long FAR* elevation) 
{
   int result = SUCCESS;
   
   IDtedPtr dted;
   HRESULT hr = dted.CreateInstance(__uuidof(Dted));
   if (FAILED(hr))
   {
      ERR_report("Failed to create Dted COM object");
      return FAILURE;
   }
   
   short DTEDLevelUsed = 0;
   try
   {
      *elevation = dted->GetElevation(lat, lon, 0, DTED_ELEVATION_FEET, &DTEDLevelUsed);
      dted->Terminate();
   }
   catch(_com_error &)
   {
      return FAILURE;
   }
   
   if (*elevation == MISSING_DTED_ELEVATION)
      *elevation = MAXLONG;
   
   if (DTEDLevelUsed == 0)
      result = -10;  // no elevation value exists
   
   
   return result;
}


//return height and width of map in pixels
long map_server::GetMapDimensions(long FAR* height, long FAR* width) 
{
   ASSERT(height);
   ASSERT(width);

   //
   // return FAILURE if there is no MapView (e.g. if in print 
   // preview or if on a modal dialog box before startup)
   //
   MapView* view = fvw_get_view();
   if (view == NULL)
   {
     *height = 0;
     *width = 0;
     return FAILURE;
   }


   CRect rect;
   view->GetClientRect(rect);
   
   *width = rect.BottomRight().x;
   *height = rect.BottomRight().y;

   return SUCCESS;
}


long map_server::SetObjectTooltip(long layer_handle, long object_handle, LPCTSTR tooltip) 
{ 
   C_map_server_ovl *overlay = LookupOverlay(layer_handle);
   if (!overlay)
      return FAILURE;

   return m_object_tootip_map.insert(std::make_pair(
      HandleKeyType(layer_handle, object_handle), std::string(tooltip))).second ? SUCCESS : FAILURE;
}


long map_server::SetObjectComment(long layer_handle, long object_handle, LPCTSTR comment) 
{
   C_map_server_ovl *overlay = LookupOverlay(layer_handle);
   if (!overlay)
      return FAILURE;

   return m_object_comment_map.insert(std::make_pair(
      HandleKeyType(layer_handle, object_handle), std::string(comment))).second ? SUCCESS : FAILURE;
}

long map_server::SetObjectHelpText(long layer_handle, long object_handle, LPCTSTR help_text) 
{
   C_map_server_ovl *overlay = LookupOverlay(layer_handle);
   if (!overlay)
      return FAILURE;

   return m_object_helptext_map.insert(std::make_pair(
      HandleKeyType(layer_handle, object_handle), std::string(help_text))).second ? SUCCESS : FAILURE;
}

long map_server::GetDegreesPerPixel(double lat, double lon, long category, long scale, 
   double FAR* deg_lat_per_pix, double FAR* deg_lon_per_pix) 
{
   // check for valid lat-lon
   if (!GEO_valid_degrees(lat, lon))
   {
      CString msg((LPCSTR) IDS_INVALID_LAT_LON);
      ERR_report(msg);
      return FAILURE;
   }

   MapSource map_source;
   MapScale map_scale;
   MapSeries map_series;

   // If the category/scale combination is invalid this function will fail.  We
   // only need the map_scale to get the degrees per pixel values.
   if (from_client(category, scale, map_source, map_scale, map_series) != 
      SUCCESS)
   {
      ERR_report("from_client() failed.");
      return FAILURE;
   }

   MapEngineCOM map_engine;
   map_engine.init(FALSE);
   
   try
   {
      IDispatch *dispatch;
      map_engine.GetMapHandler(map_source, &dispatch);
      IMapRenderPtr render = dispatch;
      if (render != NULL)
      {
         VARIANT_BOOL bLockDegreesPerPixel;
         render->GetDegreesPerPixelData(_bstr_t(map_source.get_string()),
            map_scale.GetScale(), map_scale.GetScaleUnits(), _bstr_t(map_series.get_string()),
            lat, deg_lat_per_pix, deg_lon_per_pix, &bLockDegreesPerPixel);
      }
      else
      {
         ERR_report("Requested map does not have a renderer");
         return FAILURE;
      }
   }
   catch(_com_error &)
   {
      ERR_report("GetDegreesPerPixelData failed");
      return FAILURE;
   }
   
   
   return SUCCESS;
}


long map_server::GetMapDisplay(double FAR* lat, double FAR* lon, 
   double FAR* rotation, long FAR* category, long FAR* scale, long FAR* zoom) 
{
   CView* view = UTL_get_active_non_printing_view();
   if (view == NULL)
      return FAILURE;
   const MapProj* curr_map = UTL_get_current_view_map(view);
   if (curr_map == NULL)
      return FAILURE;

   // make sure the current map is valid
   if (!curr_map->is_projection_set())
      return FAILURE;

   //set ouput parameters converting as necessary
   *lat = curr_map->actual_center_lat();
   *lon = curr_map->actual_center_lon();
   *rotation = curr_map->actual_rotation();
   *zoom = curr_map->actual_zoom_percent();
      
   int client_category;
   int client_scale;

   if (to_client(curr_map->source(), curr_map->scale(), curr_map->series(), 
      client_category, client_scale) != SUCCESS)
   {
      ERR_report("to_client() failed.");
      return FAILURE;
   }

   *category = client_category;
   *scale = client_scale;

   return SUCCESS;
}

//translates client color to server color
int map_server::translate_client_color(int client_color)
{
   int result;

   switch(client_color)
   {
      case 0 : result = BLACK; break;
      case 1 : result = STD_RED; break;
      case 2 : result = STD_GREEN; break;
      case 3 : result = YELLOW; break;
      case 4 : result = BLUE; break;
      case 5 : result = MAGENTA; break;
      case 6 : result = CYAN; break;
      case 7 : result = LIGHT_GREY; break;
      case 8 : result = PALE_GREEN; break;
      case 9 : result = LIGHT_BLUE; break;
      case 10 : result = OFF_WHITE; break;
      case 11 : result = MEDIUM_GRAY; break;
      case 12 : result = WHITE; break;
      case 13 : result = STD_BRIGHT_RED; break;
      case 14 : result = STD_BRIGHT_GREEN; break;
      case 15 : result = BRIGHT_YELLOW; break;
      case 16 : result = BRIGHT_BLUE; break;
      case 17 : result = BRIGHT_MAGENTA; break;
      case 18 : result = BRIGHT_CYAN; break;
      case 19 : result = BRIGHT_WHITE; break;
      default: 
         ASSERT(0); //client color not found
         result = BRIGHT_WHITE; 
         break;
   }

   return result;
}

long map_server::OpenOverlay(long type, LPCTSTR filespec) 
{
   int result;

   GUID overlayDescGuid = g_map_server_util.TypeToOverlayGuid(type);
   OverlayTypeDescriptor* pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc == NULL)
   {
      CString msg;
      msg.Format("OpenOverlay: could not find overlay type %d", type);
      ERR_report(msg);
      return FAILURE;
   }
   
   //determine whether static of file overlay and call approprate function
   if (OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid))
      result = OpenStaticOverlay(overlayDescGuid);
   else 
      if (OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid))
         result = OpenFileOverlay(overlayDescGuid, filespec);
      else
         result = FAILURE;
   
   return result;
}


long map_server::CloseOverlay(long type, LPCTSTR filespec) 
{
   int result;

   GUID overlayDescGuid = g_map_server_util.TypeToOverlayGuid(type);

   //determine whether static of file overlay and call approprate function
   if (OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid))
      result = CloseStaticOverlay(overlayDescGuid);
   else 
      if (OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid))
         result = CloseFileOverlay(overlayDescGuid, filespec);
      else
         result = FAILURE;
   
   return 0;
}


int map_server::OpenStaticOverlay(GUID overlayDescGuid)
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

int map_server::CloseStaticOverlay(GUID overlayDescGuid)
{
   // if overlay is already closed
   if (!OVL_get_overlay_manager()->get_first_of_type(overlayDescGuid))
      return SUCCESS;

   // WARNING: class_name must be a static overlay
   ASSERT(OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid));

   return OVL_get_overlay_manager()->toggle_static_overlay(overlayDescGuid);  //close overlay
}

int map_server::OpenFileOverlay(GUID overlayDescGuid, const char* filespec)
{
   // WARNING: the overlay type must be a file overlay
   ASSERT(OVL_get_type_descriptor_list()->IsFileOverlay(overlayDescGuid));

   C_overlay *ret_overlay;
   return OVL_get_overlay_manager()->OpenFileOverlay(overlayDescGuid, filespec, ret_overlay);
}

int map_server::CloseFileOverlay(GUID overlayDescGuid, const char* filespec)
{
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager(); //doesn't fail
   C_overlay *overlay= ovl_mgr->get_file_overlay(overlayDescGuid, filespec);
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

/*
 *  Returns the handle (a handle is >= 0).
 *  If the layer could not be created, -1 is returned.
 */
long map_server::CreateLayerEx(LPCTSTR layer_name, LPCTSTR icon_name) 
{
   // TO DO: do not let clients call this function until they register with the map server
   if (!m_client_registered_with_server)
   {
      //TO DO: put up a message box or something?
      INFO_report("Failed to create layer because client was not registered with the map server");
      return FAILURE;
   }


    //FAILURE must be less than 0 otherwise it might be interpreted as a valid return handle
   ASSERT (FAILURE <0); 

   CComObject<C_map_server_ovl> *overlay;
   CComObject<C_map_server_ovl>::CreateInstance(&overlay);


//   if (!(overlay = new C_map_server_ovl(layer_name)))
   if (!overlay)
   {
      ERR_report("CreateInstance failed");
      return FAILURE;  
   }
   overlay->AddRef();
   overlay->Setup(::GetCurrentThreadId(), layer_name, m_callback, icon_name,
      nullptr);
   overlay->Initialize(FVWID_Overlay_MapServer);

   //tO DO: put these in the constructor instead
   //store icon_name, client name, client handle in overlay
   overlay->set_icon_name(icon_name);
   overlay->SetClientWindowHandle(m_client_window_handle);
   overlay->SetClientName(m_client_name);

   //add to overlay manager
   if (OVL_get_overlay_manager()->add_overlay(overlay, FALSE) !=SUCCESS)
   {
      ERR_report("CreateLayer failed");
      return FAILURE;
   }
   
   long overlay_handle = OVL_get_overlay_manager()->get_overlay_handle(overlay);

   //add to layer handle map
   m_overlay_map.SetAt(overlay_handle, overlay);

   return overlay_handle;
}




long map_server::CAPSPrint(LPCTSTR text, BOOL print_to_scale, BOOL show_map_info) 
{

   long result = FAILURE;

   //
   // return FAILURE if there is no MapView (e.g. if in print 
   // preview or if on a modal dialog box before startup)
   //
   MapView *view = fvw_get_view();
   if (view == NULL)
      return FAILURE;
   

   if (view)
   {
      //TO DO: guarantee that we're not reading invalid memory locations

      CList <CString*, CString*> strings_list;
      
      const int MAX_ROWS = 20;
      const int MAX_CHARS_PER_LINE = 80;

      CString s[MAX_ROWS];

      char buffer[MAX_CHARS_PER_LINE+1]; // +1 to allow for NULL at 80

      int row=0;
      int chars;
      const char* ptr = text;
      do
      {
         chars=0;
         //while not at end of line or end of text block
         while (*(ptr+chars) != '\0' && *(ptr+chars) !='\n')
            chars++;
        
         //copy number of chars in new string to buffer <=80
         int safe_chars = (chars < MAX_CHARS_PER_LINE) ? chars : MAX_CHARS_PER_LINE; 
         strncpy_s(buffer, MAX_CHARS_PER_LINE+1, ptr, safe_chars);
         buffer[safe_chars]='\0'; //strncpy doesn't add one if characters copied = 80
      
         //copy from buffer to CString
         s[row]= buffer;
         s[row].TrimRight();   //get rid of trailing newlines,etc
                  
         ptr+=chars+1; //move to beginning of next string

         strings_list.AddTail(&s[row]);  //add cstring* to list
         row++;   
      }
      while (row < MAX_ROWS && *ptr != '\0');  //while less than max rows and not at end of text block

      view->do_CAPS_print(&strings_list, print_to_scale, show_map_info);
     
      //strings_list.RemoveAll();
      
      result = SUCCESS;
   }

   return result;
}


/*
 *  Returns the handle (a handle is >= 0).
 *  If the layer could not be created, -1 is returned.
 */
long map_server::CreateGPSTrail(LPCTSTR filespec, long reserved, long options, long /*sample_period*/) 
{
   ERR_report("CreateGPSTrail is obsolete.  Please use ILayer::CreateOverlay instead");
   return FAILURE;
}



long map_server::AddGPSPoint
(
   long handle,
   float latitude, float longitude,
   float speed_knots, float speed_km_hr,
   float true_heading, float magnetic_heading, 
   float msl,
   float time, short day, short month, short year,
   short satellites
) 
{
   ERR_report("AddGPSPoint is obsolete.  Please use IGPS::AddPoint instead");
   return FAILURE;
}




//
// define the type for the dted cache class
//
#if 1
typedef combo_dted_hit_and_miss_cache DTED_CACHE_CLASS;
#elif 1
typedef nima_dted_hit_and_miss_cache DTED_CACHE_CLASS;
#else
typedef cms_dted_hit_and_miss_cache DTED_CACHE_CLASS;
#endif


SCODE map_server::GetBlockDTEDinFeet(double NW_lat, double NW_lon, 
                                     double SE_lat, double SE_lon, 
                                     short number_NS_samples, 
                                     short number_EW_samples, 
                                     short DTED_type, VARIANT FAR* array) 
{
   IDtedPtr dted;
   HRESULT hr = dted.CreateInstance(__uuidof(Dted));
   if (FAILED(hr))
   {
      ERR_report("Failed to create Dted COM object");
      return FAILURE;
   }
   
   try
   {
      *array = dted->GetBlockDTED(NW_lat, NW_lon, SE_lat, SE_lon, number_NS_samples,
         number_EW_samples, DTED_type, DTED_ELEVATION_FEET);
      dted->Terminate();
   }
   catch(_com_error &)
   {
      return FAILURE;
   }
   
   return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

SCODE map_server::GetMouseClick(long main_window_handle, long FAR* click_id) 
{

   MapView* view = fvw_get_view();
   if (view == NULL)
      return E_FAIL;  //TO DO: kevin: better error code

   *click_id = view->m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickWindowsMessageCallback((HWND) main_window_handle), SINGLE_CLICK);

   return S_OK;
}


//////////////////////////////////////////////////////////////////////////////

long map_server::CreateMap(double center_lat, double center_lon, long category, 
   long scale, double rotation_angle, long zoom_percent, double brightness, 
   short draw_overlays, long pixmap_width, long pixmap_height, 
   short draw_map_if_no_data, short* no_data, VARIANT* dib, VARIANT* corners) 
{
   *no_data = TRUE;

   //
   // make the pixmap have a width and a height of at least 3
   //
   if (pixmap_width < 3 || pixmap_height < 3)
   {
      ERR_report("invalid pixmap dimension");
      return FAILURE;
   }

   //
   // if the scale is a relative one (i.e. one relative to the current map such as
   // CLIENT_CLOSEST_SCALE), then it is invalid
   //
   if (scale < 0)
   {
      ERR_report("invalid scale");
      return FAILURE;
   }

   //
   // check to see if the variant is initialized already
   //
   if (!(dib && dib->vt == VT_EMPTY) ||
      !(corners && corners->vt == VT_EMPTY))
   {
      ERR_report("variant not cleared");
      return FAILURE;
   }

   MapSource map_source;
   MapScale map_scale;
   MapSeries map_series;
   from_client(category, scale, map_source, map_scale, map_series);
   
   try
   {
      MapEngineCOM map_engine;
      map_engine.init(TRUE, draw_overlays);
      map_engine.set_view_dimensions(pixmap_width, pixmap_height);
      
      int status = map_engine.change_map_type(map_source, map_scale, map_series,
         center_lat, center_lon, rotation_angle, zoom_percent,
         EQUALARC_PROJECTION, draw_map_if_no_data);
      
      *no_data = (status == FV_NO_DATA);

      //
      // If the client doesn't want a blank map drawn when there is no data,
      // then return SUCCESS with the no_data flag set to TRUE.
      //
      if (status == FV_NO_DATA && !draw_map_if_no_data)
      {
         return SUCCESS;
      }
      
      SettableMapProj *settable_map = map_engine.get_curr_map();
      
      status = map_engine.DrawMapToDIB(dib);
      COleSafeArray corners_sa;
      MapServerUtil util;
      if (util.corners_to_variant(settable_map, &corners_sa) != SUCCESS)
      {
         ERR_report("Unable to get corners");
         return FAILURE;
      }
      try
      {
         //
         // set the corners variant with the corners safearray
         //
         *corners = corners_sa.Detach();
      }
      catch (COleException* e)
      {
         e->Delete();
         ERR_report("COleException");
         return FAILURE;
      }
   }
   catch(_com_error &)
   {
      ERR_report("Failed creating map");
      return FAILURE;
   }
   
   return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

int MapServerUtil::get_map_corners_in_variant(const SettableMapProj* map, 
                                              VARIANT* corners_variant)
{
   //
   // get the map bounds of the pixel centers of the corner pixels
   //
   COleSafeArray corners_sa;
   if (g_map_server_util.get_map_corners(map, &corners_sa) != SUCCESS)
   {
      ERR_report("get_map_corners");
      return FAILURE;
   }

   try
   {
      //
      // set the corners variant with the corners safearray
      //
      *corners_variant = corners_sa.Detach();
   }
   catch (COleException* e)
   {
      e->Delete();
      ERR_report("COleException");
      return FAILURE;
   }

   return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

long map_server::CalcPixmapGeoCorners(double center_lat, double center_lon, 
   long category, long scale, double rotation_angle, long zoom_percent, 
   long pixmap_width, long pixmap_height, VARIANT FAR* corners,
   short check_for_data, short FAR* no_data) 
{
   *no_data = TRUE;

   //
   // if the scale is a relative one (i.e. one relative to the current map such as
   // CLIENT_CLOSEST_SCALE), then it is invalid
   //
   if (scale < 0)
   {
      ERR_report("invalid scale");
      return FAILURE;
   }

   //
   // make the pixmap have a width and a height of at least 3
   //
   if (pixmap_width < 3 || pixmap_height < 3)
   {
      ERR_report("invalid pixmap dimension");
      return FAILURE;
   }

   //
   // check to see if the variant is initialized already
   //
   if (!(corners && corners->vt == VT_EMPTY))
   {
      ERR_report("variant not cleared");
      return FAILURE;
   }

   MapSource map_source;
   MapScale map_scale;
   MapSeries map_series;

   // convert map server category and scale ints to a map type
   if (from_client(category, scale, 
      map_source, map_scale, map_series) != SUCCESS)
   {
      ERR_report("from_client() failed.");
      return FAILURE;
   } 

   try
   {
      MapEngineCOM map_engine;
      map_engine.init(FALSE);
      map_engine.set_view_dimensions(pixmap_width, pixmap_height);
      
      int status = map_engine.change_map_type(map_source, map_scale, map_series,
         center_lat, center_lon, rotation_angle, zoom_percent,
         EQUALARC_PROJECTION, TRUE, FALSE);

      map_engine.ApplyMap();
      
      *no_data = (status == FV_NO_DATA);
      
      SettableMapProj *settable_map = map_engine.get_curr_map();
      
      COleSafeArray corners_sa;
      MapServerUtil util;
      if (util.corners_to_variant(settable_map, &corners_sa) != SUCCESS)
      {
         ERR_report("Unable to get corners");
         return FAILURE;
      }
      try
      {
         //
         // set the corners variant with the corners safearray
         //
         *corners = corners_sa.Detach();
      }
      catch (COleException* e)
      {
         e->Delete();
         ERR_report("COleException");
         return FAILURE;
      }
   }
   catch(_com_error &)
   {
      ERR_report("Failed creating map");
      return FAILURE;
   }
   
   return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

int MapServerUtil::get_map_corners(const MapProj* map, COleSafeArray* corners)
{
   const int width = map->get_surface_width();
   const int height = map->get_surface_height();

   degrees_t ll_lat;
   degrees_t ll_lon;
   degrees_t ul_lat;
   degrees_t ul_lon;
   degrees_t ur_lat;
   degrees_t ur_lon;
   degrees_t lr_lat;
   degrees_t lr_lon;

   if (map->surface_to_geo(0, height-1, &ll_lat, &ll_lon) != SUCCESS ||
      map->surface_to_geo(0, 0, &ul_lat, &ul_lon) != SUCCESS ||
      map->surface_to_geo(width-1, 0, &ur_lat, &ur_lon) != SUCCESS ||
      map->surface_to_geo(width-1, height-1, &lr_lat, &lr_lon) != SUCCESS)
   {
      ERR_report("surface_to_geo");
      return FAILURE;
   }

   SAFEARRAYBOUND sa_bound;
   sa_bound.cElements = 8;
   sa_bound.lLbound = 0;

   try
   {
      //
      // allocate the safe array
      //
      corners->Create(VT_R8, 1, &sa_bound);

      //
      // get a pointer to the safearray data
      //
      double* ptr;
      corners->AccessData(reinterpret_cast<void**>(&ptr));

      //
      // fill in the corner info
      //
      ptr[0] = ll_lat;
      ptr[1] = ll_lon;
      ptr[2] = ul_lat;
      ptr[3] = ul_lon;
      ptr[4] = ur_lat;
      ptr[5] = ur_lon;
      ptr[6] = lr_lat;
      ptr[7] = lr_lon;

      corners->UnaccessData();
   }
   catch(CMemoryException* e)
   {
      e->Delete();
      ERR_report("COleMemoryException");
      return FAILURE;
   }
   catch(COleException* e)
   {
      e->Delete();
      ERR_report("COleException");
      return FAILURE;
   }

   return SUCCESS;
}

// convert client overlay type into class name
GUID MapServerUtil::TypeToOverlayGuid(int type)
{
   GUID overlayDescGuid = GUID_NULL;

   // for the plug-in overlay architecture
   if (type >= CLIENT_LAYER_EDITOR0 && type <= CLIENT_LAYER_EDITOR19)
   {
      OVL_get_type_descriptor_list()->ResetEnumerator();
      while ( OVL_get_type_descriptor_list()->MoveNext() )
      {
         CLayerOvlFactory* pLayerOvlFactory = dynamic_cast<CLayerOvlFactory *>(OVL_get_type_descriptor_list()->m_pCurrent->pOverlayFactory);
         if (pLayerOvlFactory != NULL && pLayerOvlFactory->GetEditorNum() == (type - CLIENT_LAYER_EDITOR0) )
         {
            overlayDescGuid = OVL_get_type_descriptor_list()->m_pCurrent->overlayDescriptorGuid;
            break;
         }
      }
   }
   else switch (type)
   {
      // file overlays
      case CLIENT_OVL_RTE_ROUTE: overlayDescGuid = FVWID_Overlay_Route; break;
      case CLIENT_OVL_DRAWING: overlayDescGuid = FVWID_Overlay_Drawing; break;
      case CLIENT_OVL_GPS: overlayDescGuid = FVWID_Overlay_MovingMapTrail; break;
      case CLIENT_OVL_THREAT: overlayDescGuid = FVWID_Overlay_Threat; break;
      case CLIENT_OVL_CHUM: overlayDescGuid = FVWID_Overlay_ManualChum; break;
      case CLIENT_OVL_PAGE_LAYOUT: overlayDescGuid = FVWID_Overlay_PageLayout; break;
      case CLIENT_OVL_POINT_EXPORT: overlayDescGuid = FVWID_Overlay_PointExport; break;
      case CLIENT_OVL_SHAPE: overlayDescGuid = FVWID_Overlay_ShapeFile; break;
      case CLIENT_OVL_MOV_SYM: overlayDescGuid = FVWID_Overlay_SkyView; break;
      case CLIENT_OVL_TACTICAL_GRAPHICS: overlayDescGuid = FVWID_Overlay_TacticalGraphics; break;
      case CLIENT_OVL_LOCAL_POINT: overlayDescGuid = FVWID_Overlay_Points; break;
      case CLIENT_OVL_TACTICAL_MODEL: overlayDescGuid = FVWID_Overlay_TacticalModel; break;

      // static overlays
      case CLIENT_OVL_AIRPORT: overlayDescGuid = FVWID_Overlay_Airports; break;
      case CLIENT_OVL_NAVAID: overlayDescGuid = FVWID_Overlay_Navaids; break;
      case CLIENT_OVL_WAYPOINT: overlayDescGuid = FVWID_Overlay_Waypoints; break;
      case CLIENT_OVL_ECHUM: overlayDescGuid = FVWID_Overlay_ElectronicChum; break;
      case CLIENT_OVL_AIMPOINT: overlayDescGuid = FVWID_Overlay_Aimpoints; break;
      case CLIENT_OVL_GRID: overlayDescGuid = FVWID_Overlay_CoordinateGrid; break;
      case CLIENT_OVL_SUAS: overlayDescGuid = FVWID_Overlay_SuasBoundaries; break;
      case CLIENT_OVL_AIRSPACE_BOUNDARY: overlayDescGuid = FVWID_Overlay_AirspaceBoundaries; break;
      case CLIENT_OVL_HELIPORT: overlayDescGuid = FVWID_Overlay_Heliports; break;
      case CLIENT_OVL_PARACHUTE_JUMP: overlayDescGuid = FVWID_Overlay_ParachuteJump; break;
      case CLIENT_OVL_AIRWAY: overlayDescGuid = FVWID_Overlay_Airways; break;
      case CLIENT_OVL_MTR: overlayDescGuid = FVWID_Overlay_MilitaryTrainingRoutes; break;
      case CLIENT_OVL_REFUELING_RTE: overlayDescGuid = FVWID_Overlay_RefuelingRoutes; break;
      case CLIENT_OVL_SCALE_BAR: overlayDescGuid = FVWID_Overlay_ScaleBar; break;
      case CLIENT_OVL_DROPZONE: overlayDescGuid = FVWID_Overlay_Dropzones; break;
      case CLIENT_OVL_VMAP: overlayDescGuid = FVWID_Overlay_VectorSmartMap; break;
      case CLIENT_OVL_DNC: overlayDescGuid = FVWID_Overlay_DigitalNauticalChart; break;
      case CLIENT_OVL_WVS: overlayDescGuid = FVWID_Overlay_WorldVectorShoreline; break;
      case CLIENT_OVL_SHADOW: overlayDescGuid = FVWID_Overlay_Shadow; break;
      case CLIENT_OVL_CONTOUR: overlayDescGuid = FVWID_Overlay_ContourLines; break;
      case CLIENT_OVL_AR_EDIT: overlayDescGuid = FVWID_Overlay_TrackOrbit; break;
      case CLIENT_OVL_IMAP_SERVER: overlayDescGuid = FVWID_Overlay_MapServer; break;
      case CLIENT_OVL_ENVIRONMENTAL: overlayDescGuid = FVWID_Overlay_Environmental; break;
      case CLIENT_OVL_TAMASK: overlayDescGuid = FVWID_Overlay_TerrainAvoidanceMask; break;
   }

   return overlayDescGuid;
}

// retreive a constant from a class name
int MapServerUtil::OverlayGuidToType(GUID overlayDescGuid)
{
   // file overlays
   if (overlayDescGuid == FVWID_Overlay_Route)
      return CLIENT_OVL_RTE_ROUTE;
   if (overlayDescGuid == FVWID_Overlay_Drawing) 
      return CLIENT_OVL_DRAWING;    
   if (overlayDescGuid == FVWID_Overlay_MovingMapTrail) 
      return CLIENT_OVL_GPS;     
   if (overlayDescGuid == FVWID_Overlay_Threat) 
      return CLIENT_OVL_THREAT;        
   if (overlayDescGuid == FVWID_Overlay_ManualChum) 
      return CLIENT_OVL_CHUM;    
   if (overlayDescGuid == FVWID_Overlay_PageLayout)
      return CLIENT_OVL_PAGE_LAYOUT;
   if (overlayDescGuid == FVWID_Overlay_PointExport)
      return CLIENT_OVL_POINT_EXPORT;
   if (overlayDescGuid == FVWID_Overlay_ShapeFile)
      return CLIENT_OVL_SHAPE;
   if (overlayDescGuid == FVWID_Overlay_SkyView) 
      return CLIENT_OVL_MOV_SYM; 
   if (overlayDescGuid == FVWID_Overlay_TacticalGraphics) 
      return CLIENT_OVL_TACTICAL_GRAPHICS;
   if (overlayDescGuid == FVWID_Overlay_Points)
      return CLIENT_OVL_LOCAL_POINT;
   if (overlayDescGuid == FVWID_Overlay_TacticalModel) 
      return CLIENT_OVL_TACTICAL_MODEL;

   // static overlays
   if (overlayDescGuid == FVWID_Overlay_Airports) 
      return CLIENT_OVL_AIRPORT;    
   if (overlayDescGuid == FVWID_Overlay_Navaids) 
      return CLIENT_OVL_NAVAID;     
   if (overlayDescGuid == FVWID_Overlay_Waypoints) 
      return CLIENT_OVL_WAYPOINT;     
   if (overlayDescGuid == FVWID_Overlay_ElectronicChum) 
      return CLIENT_OVL_ECHUM;      
   if (overlayDescGuid == FVWID_Overlay_Aimpoints) 
      return CLIENT_OVL_AIMPOINT;    
   if (overlayDescGuid == FVWID_Overlay_CoordinateGrid) 
      return CLIENT_OVL_GRID;      
   if (overlayDescGuid == FVWID_Overlay_SuasBoundaries) 
      return CLIENT_OVL_SUAS;     
   if (overlayDescGuid == FVWID_Overlay_AirspaceBoundaries)
      return CLIENT_OVL_AIRSPACE_BOUNDARY;
   if (overlayDescGuid == FVWID_Overlay_Heliports)
      return CLIENT_OVL_HELIPORT;
   if (overlayDescGuid == FVWID_Overlay_ParachuteJump)
      return CLIENT_OVL_PARACHUTE_JUMP;
   if (overlayDescGuid == FVWID_Overlay_TrackOrbit)
      return CLIENT_OVL_AR_EDIT;
   if (overlayDescGuid == FVWID_Overlay_Airways)
      return CLIENT_OVL_AIRWAY;
   if (overlayDescGuid == FVWID_Overlay_MilitaryTrainingRoutes)
      return CLIENT_OVL_MTR;
   if (overlayDescGuid == FVWID_Overlay_RefuelingRoutes)
      return CLIENT_OVL_REFUELING_RTE;
   if (overlayDescGuid == FVWID_Overlay_ScaleBar)
      return CLIENT_OVL_SCALE_BAR;
   if (overlayDescGuid == FVWID_Overlay_Dropzones)
      return CLIENT_OVL_DROPZONE;
   if (overlayDescGuid == FVWID_Overlay_MapServer)
      return CLIENT_OVL_IMAP_SERVER;
   if (overlayDescGuid == FVWID_Overlay_Shadow)
      return CLIENT_OVL_SHADOW;
   if (overlayDescGuid == FVWID_Overlay_ContourLines)
      return CLIENT_OVL_CONTOUR;
   if (overlayDescGuid == FVWID_Overlay_MapDataManager)
      return CLIENT_OVL_MAP_DATA_MANAGER;
   // NITF_CONVERT 
   if (overlayDescGuid == FVWID_Overlay_NitfFiles)
      return CLIENT_OVL_NITF;    
   if (overlayDescGuid == FVWID_Overlay_DigitalNauticalChart)
      return CLIENT_OVL_DNC;
   if (overlayDescGuid == FVWID_Overlay_VectorSmartMap)
      return CLIENT_OVL_VMAP;
   if (overlayDescGuid == FVWID_Overlay_Environmental)
      return CLIENT_OVL_ENVIRONMENTAL;
   if (overlayDescGuid == FVWID_Overlay_TerrainAvoidanceMask)
      return CLIENT_OVL_TAMASK;
   if (overlayDescGuid == FVWID_Overlay_WorldVectorShoreline)
      return CLIENT_OVL_WVS;
   
   return -1;
}

//////////////////////////////////////////////////////////////////////////////

long map_server::GetLatLonPositionInMap(double center_lat, double center_lon, 
   long category, long scale, double rotation_angle, long zoom_percent, 
   long pixmap_width, long pixmap_height, long pixel_column, long pixel_row, 
   double FAR* pixel_latitude, double FAR* pixel_longitude) 
{
   if (pixel_column < 0 || pixel_row < 0 || pixel_column >= pixmap_width ||
      pixel_row >= pixmap_height)
   {
      ERR_report("invalid pixel coords");
      return FAILURE;
   }

   MapSource map_source;
   MapScale map_scale;
   MapSeries map_series;

   // convert map server category and scale ints to a map type
   if (from_client(category, scale, map_source, map_scale, map_series) != 
      SUCCESS)
   {
      ERR_report("from_client() failed.");
      return FAILURE;
   } 

   try
   {
      MapEngineCOM map_engine;
      map_engine.init(FALSE);
      map_engine.set_view_dimensions(pixmap_width, pixmap_height);
      
      map_engine.change_map_type(map_source, map_scale, map_series, 
         center_lat, center_lon, rotation_angle, zoom_percent,
         EQUALARC_PROJECTION, FALSE, FALSE);
      
      map_engine.ApplyMap();
      
      SettableMapProj *settable_map = map_engine.get_curr_map();
      
      int result = settable_map->surface_to_geo(pixel_column, pixel_row, 
         pixel_latitude, pixel_longitude);
      
      return result;
   }
   catch(_com_error &)
   {
      ERR_report("Failed creating map");
      return FAILURE;
   }
   
   return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

long map_server::AddMovableSymbol(long layer_handle, long symbol_type, long symbol_flags) 
{
   return Cmov_sym_overlay::AddSymbol(symbol_type,symbol_flags);
}

long map_server::DeleteMovableSymbol(long layer_handle, long symbol_handle) 
{
   return Cmov_sym_overlay::DeleteSymbol();
}

long map_server::MoveMovableSymbol(long layer_handle, long symbol_handle, double lat, double lon, double altitude, double heading) 
{
   return Cmov_sym_overlay::MoveSymbol(lat,lon,altitude,heading);
}

long map_server::RequestLayerNotification(long layer_handle, long hWnd, long type) 
{
   return Cmov_sym_overlay::RequestNotification(hWnd,type);
}

//////////////////////////////////////////////////////////////////////////////

long map_server::CreateMapFromGeoBounds(double southern_lat, double western_lon, 
   double northern_lat, double eastern_lon, long category, long scale, 
   double brightness, short draw_overlays, short draw_map_if_no_data, 
   short FAR* no_data, long FAR* pixmap_width, long FAR* pixmap_height, 
   VARIANT FAR* dib, VARIANT FAR* corners) 
{
   //
   // check to see if the variant is initialized already
   //
   if (!(dib && dib->vt == VT_EMPTY) ||
      !(corners && corners->vt == VT_EMPTY))
   {
      ERR_report("variant not cleared");
      return FAILURE;
   }
   
   MapSource map_source;
   MapScale map_scale;
   MapSeries map_series;
   from_client(category, scale, map_source, map_scale, map_series);
   
   try
   {
      MapEngineCOM map_engine;
      map_engine.init(TRUE, draw_overlays);
      
      double surface_width, surface_height;
      int status = map_engine.set_map_type_from_geo_bounds(map_source,
         map_scale, map_series, 
         southern_lat, western_lon, northern_lat, eastern_lon, 0.0, 100,
         EQUALARC_PROJECTION, draw_map_if_no_data,
         &surface_width, &surface_height);
      
      *no_data = (status == FV_NO_DATA);
      *pixmap_width = static_cast<int>(surface_width);
      *pixmap_height = static_cast<int>(surface_height);

      //
      // If the client doesn't want a blank map drawn when there is no data,
      // then return SUCCESS with the no_data flag set to TRUE.
      //
      if (status == FV_NO_DATA && !draw_map_if_no_data)
      {
         return SUCCESS;
      }
      
      SettableMapProj *settable_map = map_engine.get_curr_map();
      
      status = map_engine.DrawMapToDIB(dib);
      COleSafeArray corners_sa;
      MapServerUtil util;
      if (util.corners_to_variant(settable_map, &corners_sa) != SUCCESS)
      {
         ERR_report("Unable to get corners");
         return FAILURE;
      }
      try
      {
         //
         // set the corners variant with the corners safearray
         //
         *corners = corners_sa.Detach();
      }
      catch (COleException* e)
      {
         e->Delete();
         ERR_report("COleException");
         return FAILURE;
      }
   }
   catch(_com_error &)
   {
      ERR_report("Failed creating map");
      return FAILURE;
   }
   
   return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

typedef struct _sorted_img_t_
{
   DWORD color;
   int index;
   int inc;
   BYTE red;
   BYTE green;
   BYTE blue;
} sorted_img_t;

int compare_colors(const void *arg1, const void *arg2)
{
   return memcmp(arg1, arg2, 32);
}

//////////////////////////////////////////////////////////////////////////////

long map_server::CreateFixedSizeMapFromGeoBounds(double southern_lat, 
   double western_lon, double northern_lat, double eastern_lon, long category, 
   long scale, double brightness, short draw_overlays, short draw_map_if_no_data, 
   long pixmap_width, long pixmap_height, short FAR* no_data, VARIANT FAR* dib, 
   VARIANT FAR* corners) 
{
   //
   // check to see if the variant is initialized already
   //
   if (!(dib && dib->vt == VT_EMPTY) ||
      !(corners && corners->vt == VT_EMPTY))
   {
      ERR_report("variant not cleared");
      return FAILURE;
   }

   MapSource map_source;
   MapScale map_scale;
   MapSeries map_series;
   from_client(category, scale, map_source, map_scale, map_series);
   
   try
   {
      MapEngineCOM map_engine;
      map_engine.init(TRUE, draw_overlays);
      
      double surface_width, surface_height;
      int status = map_engine.set_map_type_from_geo_bounds(map_source, map_scale, map_series,
         southern_lat, western_lon, northern_lat, eastern_lon, 0.0, 100,
         EQUALARC_PROJECTION, draw_map_if_no_data, &surface_width, &surface_height);
      
      *no_data = (status == FV_NO_DATA);
      
      SettableMapProj *settable_map = map_engine.get_curr_map();
      
      status = map_engine.DrawMapToResizedDIB(pixmap_width, pixmap_height, dib);
      COleSafeArray corners_sa;
      MapServerUtil util;
      if (util.corners_to_variant(settable_map, &corners_sa) != SUCCESS)
      {
         ERR_report("Unable to get corners");
         return FAILURE;
      }
      try
      {
         //
         // set the corners variant with the corners safearray
         //
         *corners = corners_sa.Detach();
      }
      catch (COleException* e)
      {
         e->Delete();
         ERR_report("COleException");
         return FAILURE;
      }
   }
   catch(_com_error &)
   {
      ERR_report("Failed creating map");
      return FAILURE;
   }
   
   return SUCCESS;
}

long map_server::CalcPixmapSizeFromGeoBounds(double southern_lat, double western_lon,
   double northern_lat, double eastern_lon, long category, long scale, 
   long FAR* pixmap_width, long FAR* pixmap_height, VARIANT FAR* corners, 
   short check_for_data, short FAR* no_data) 
{
   *no_data = TRUE;
   
   //
   // check to see if the variant is initialized already
   //
   if (!(corners && corners->vt == VT_EMPTY))
   {
      ERR_report("variant not cleared");
      return FAILURE;
   }
   
   MapSource map_source;
   MapScale map_scale;
   MapSeries map_series;
   from_client(category, scale, map_source, map_scale, map_series);
   
   try
   {
      MapEngineCOM map_engine;
      map_engine.init(FALSE);
      
      double surface_width, surface_height;
      int status = map_engine.set_map_type_from_geo_bounds(map_source, map_scale, map_series,
         southern_lat, western_lon, northern_lat, eastern_lon, 0.0, 100,
         EQUALARC_PROJECTION, TRUE, &surface_width, &surface_height);

      if (status == FAILURE)
      {
         ERR_report("CalcPixmapSizeFromGeoBounds : failed setting map type");
         return FAILURE;
      }
      
      *no_data = (status == FV_NO_DATA);
      *pixmap_width = static_cast<int>(surface_width);
      *pixmap_height = static_cast<int>(surface_height);

      map_engine.ApplyMap();
      
      SettableMapProj *settable_map = map_engine.get_curr_map();
      
      COleSafeArray corners_sa;
      MapServerUtil util;
      if (util.corners_to_variant(settable_map, &corners_sa) != SUCCESS)
      {
         ERR_report("Unable to get corners");
         return FAILURE;
      }
      try
      {
         //
         // set the corners variant with the corners safearray
         //
         *corners = corners_sa.Detach();
      }
      catch (COleException* e)
      {
         e->Delete();
         ERR_report("COleException");
         return FAILURE;
      }
   }
   catch(_com_error &)
   {
      ERR_report("Failed creating map");
      return FAILURE;
   }
   
   return SUCCESS;
}

int MapServerUtil:: corners_to_variant(SettableMapProj *map, COleSafeArray *corners)
{
   int width = map->get_surface_width();
   int height = map->get_surface_height();

   degrees_t ll_lat;
   degrees_t ll_lon;
   degrees_t ul_lat;
   degrees_t ul_lon;
   degrees_t ur_lat;
   degrees_t ur_lon;
   degrees_t lr_lat;
   degrees_t lr_lon;

   int result1 = map->surface_to_geo(0, height-1, &ll_lat, &ll_lon);
   int result2 = map->surface_to_geo(0, 0, &ul_lat, &ul_lon);
   int result3 = map->surface_to_geo(width-1, 0, &ur_lat, &ur_lon);
   int result4 = map->surface_to_geo(width-1, height-1, &lr_lat, &lr_lon);

   if (result1 != SUCCESS || result2 != SUCCESS ||
       result3 != SUCCESS || result4 != SUCCESS)
   {
      ERR_report("surface_to_geo");
      return FAILURE;
   }

   SAFEARRAYBOUND sa_bound;
   sa_bound.cElements = 8;
   sa_bound.lLbound = 0;

   try
   {
      //
      // allocate the safe array
      //
      corners->Create(VT_R8, 1, &sa_bound);

      //
      // get a pointer to the safearray data
      //
      double* ptr;
      corners->AccessData(reinterpret_cast<void**>(&ptr));

      //
      // fill in the corner info
      //
      ptr[0] = ll_lat;
      ptr[1] = ll_lon;
      ptr[2] = ul_lat;
      ptr[3] = ul_lon;
      ptr[4] = ur_lat;
      ptr[5] = ur_lon;
      ptr[6] = lr_lat;
      ptr[7] = lr_lon;

      corners->UnaccessData();
   }
   catch(CMemoryException* e)
   {
      e->Delete();
      ERR_report("COleMemoryException");
      return FAILURE;
   }
   catch(COleException* e)
   {
      e->Delete();
      ERR_report("COleException");
      return FAILURE;
   }

   return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Map Server Client Define to internal MapType mappings.
///////////////////////////////////////////////////////////////////////////////

class MapType_to_client_t
{
public:
   MapSource source;
   MapScale scale;
   MapSeries series;
   int client_scale;

   MapType_to_client_t(const MapSource &source, const MapScale &scale,
      const MapSeries &series, int client_scale);
};

MapType_to_client_t::MapType_to_client_t(const MapSource &_source, const MapScale &_scale,
      const MapSeries &_series, int _client_scale)
{
   source = _source;
   scale = _scale;
   series = _series;
   client_scale = _client_scale;
}

#define BLANK_LIST_COUNT 17
#define VECTOR_LIST_COUNT 8
#define RASTER_LIST_COUNT 18
#define DTED_LIST_COUNT 2

static MapType_to_client_t *blank_list(int &count)
{
   // List of BLANK_CATEGORY MapType to/from CLIENT_BLANK_CATEGORY maps.
   static MapType_to_client_t blank_list[BLANK_LIST_COUNT] = {
      MapType_to_client_t(BLANK_MAP, WORLD, NULL_SERIES, CLIENT_WORLD),
      MapType_to_client_t(BLANK_MAP, MapScale(100000000), NULL_SERIES, CLIENT_ONE_TO_100M),
      MapType_to_client_t(BLANK_MAP, MapScale(50000000), NULL_SERIES, CLIENT_ONE_TO_50M),
      MapType_to_client_t(BLANK_MAP, ONE_TO_20M, NULL_SERIES, CLIENT_ONE_TO_20M),
      MapType_to_client_t(BLANK_MAP, ONE_TO_10M, NULL_SERIES, CLIENT_ONE_TO_10M),
      MapType_to_client_t(BLANK_MAP, ONE_TO_5M, NULL_SERIES, CLIENT_ONE_TO_5M),
      MapType_to_client_t(BLANK_MAP, ONE_TO_2M, NULL_SERIES, CLIENT_ONE_TO_2M),
      MapType_to_client_t(BLANK_MAP, ONE_TO_1M, NULL_SERIES, CLIENT_ONE_TO_1M),
      MapType_to_client_t(BLANK_MAP, ONE_TO_500K, NULL_SERIES, CLIENT_ONE_TO_500K),
      MapType_to_client_t(BLANK_MAP, ONE_TO_250K, NULL_SERIES, CLIENT_ONE_TO_250K),
      MapType_to_client_t(BLANK_MAP, ONE_TO_100K, NULL_SERIES, CLIENT_ONE_TO_100K),
      MapType_to_client_t(BLANK_MAP, ONE_TO_50K, NULL_SERIES, CLIENT_ONE_TO_50K),
      MapType_to_client_t(BLANK_MAP, ONE_TO_20K, NULL_SERIES, CLIENT_ONE_TO_20K),
      MapType_to_client_t(BLANK_MAP, ONE_TO_10K, NULL_SERIES, CLIENT_ONE_TO_10K),
      MapType_to_client_t(BLANK_MAP, ONE_TO_5K, NULL_SERIES, CLIENT_ONE_TO_5K),
      MapType_to_client_t(BLANK_MAP, ONE_TO_2K, NULL_SERIES, CLIENT_ONE_TO_2K),
      MapType_to_client_t(BLANK_MAP, ONE_TO_1K, NULL_SERIES, CLIENT_ONE_TO_1K)
   };

   count = BLANK_LIST_COUNT;

   return blank_list;
}

static MapType_to_client_t *vector_list(int &count)
{
   // List of VECTOR_CATEGORY MapType to/from CLIENT_VECTOR_CATEGORY maps.
   //
   // Re-directed to use WVS instead of Vector
   static MapType_to_client_t vector_list[VECTOR_LIST_COUNT] = {
      MapType_to_client_t("World Vector Shoreline", MapScale(2147483647), NULL_SERIES, CLIENT_WORLD),
      MapType_to_client_t("World Vector Shoreline", MapScale(100000000), NULL_SERIES, CLIENT_ONE_TO_100M),
      MapType_to_client_t("World Vector Shoreline", MapScale(50000000), NULL_SERIES, CLIENT_ONE_TO_50M),
      MapType_to_client_t("World Vector Shoreline", ONE_TO_20M, NULL_SERIES, CLIENT_ONE_TO_20M),
      MapType_to_client_t("World Vector Shoreline", ONE_TO_10M, NULL_SERIES, CLIENT_ONE_TO_10M),
      MapType_to_client_t("World Vector Shoreline", ONE_TO_5M, NULL_SERIES, CLIENT_ONE_TO_5M),
      MapType_to_client_t("World Vector Shoreline", ONE_TO_2M, NULL_SERIES, CLIENT_ONE_TO_2M),
      MapType_to_client_t("World Vector Shoreline", ONE_TO_1M, NULL_SERIES, CLIENT_ONE_TO_1M)
   };

   count = VECTOR_LIST_COUNT;

   return vector_list;
}

static MapType_to_client_t *raster_list(int &count)
{
   // List of RASTER_CATEGORY MapType to/from CLIENT_RASTER_CATEGORY maps.
   static MapType_to_client_t raster_list[RASTER_LIST_COUNT] = {
      MapType_to_client_t(TIROS, WORLD, "TopoBath", CLIENT_WORLD),
      MapType_to_client_t(TIROS, MapScale(16.0, MapScale::KILOMETER), "TopoBath", CLIENT_16_KM_SCALE),
      MapType_to_client_t(TIROS, MapScale(8.0, MapScale::KILOMETER), "TopoBath", CLIENT_8_KM_SCALE),
      MapType_to_client_t(TIROS, MapScale(4.0, MapScale::KILOMETER), "TopoBath", CLIENT_4_KM_SCALE),
      MapType_to_client_t(TIROS, MapScale(2.0, MapScale::KILOMETER), "TopoBath", CLIENT_2_KM_SCALE),
      MapType_to_client_t(TIROS, MapScale(1.0, MapScale::KILOMETER), "TopoBath", CLIENT_1_KM_SCALE),
      MapType_to_client_t(CADRG, ONE_TO_5M, GNC_SERIES, CLIENT_ONE_TO_5M),
      MapType_to_client_t(CADRG, ONE_TO_2M, JNC_SERIES, CLIENT_ONE_TO_2M),
      MapType_to_client_t(CADRG, ONE_TO_1M, ONC_SERIES, CLIENT_ONE_TO_1M),
      MapType_to_client_t(CADRG, ONE_TO_500K, TPC_SERIES, CLIENT_ONE_TO_500K),
      MapType_to_client_t(CADRG, ONE_TO_500K, LFC_SERIES, CLIENT_ONE_TO_500K_LFC),
      MapType_to_client_t(CADRG, ONE_TO_250K, JOG_SERIES, CLIENT_ONE_TO_250K),
      MapType_to_client_t(CADRG, ONE_TO_250K, TFC_SERIES, CLIENT_ONE_TO_250K_TFC),
      MapType_to_client_t(CADRG, ONE_TO_100K, TLM_SERIES, CLIENT_ONE_TO_100K),
      MapType_to_client_t(CADRG, ONE_TO_50K, TLM_SERIES, CLIENT_ONE_TO_50K),
      MapType_to_client_t(CIB, ADRI_SCALE, NULL_SERIES, CLIENT_ADRI_SCALE),
      MapType_to_client_t(CIB, ADRI_5M_SCALE, NULL_SERIES, CLIENT_ADRI_5M_SCALE),
      MapType_to_client_t(CIB, ADRI_1M_SCALE, NULL_SERIES, CLIENT_ADRI_1M_SCALE)
   };

   count = RASTER_LIST_COUNT;

   return raster_list;
}

static MapType_to_client_t *dted_list(int &count)
{
   // List of DTED_CATEGORY MapType to/from CLIENT_DTED_CATEGORY maps.
   static MapType_to_client_t dted_list[DTED_LIST_COUNT] = {
      MapType_to_client_t(DTED_NIMA, DTED_SCALE, "Level 1", CLIENT_DTED_SCALE),
      MapType_to_client_t(DTED_CMS, DTED_SCALE, "Level 1", CLIENT_DTED_SCALE)
   };

   count = DTED_LIST_COUNT;

   return dted_list;
}

#define BLANK_LIST_COUNT 17
#define VECTOR_LIST_COUNT 8
#define RASTER_LIST_COUNT 18
#define DTED_LIST_COUNT 2

static int to_client(const MapSource &source, const MapScale &scale, 
   const MapSeries &series, int &client_category, int &client_scale)
{
   // which category list should we use
   long tmp_category = g_map_server_util.convert_to_client_category(MAP_get_category(source));

   // set list to one of the category lists or fail
   MapType_to_client_t *list;
   int count;
   switch (tmp_category)
   {
      case CLIENT_BLANK_CATEGORY:
         list = blank_list(count);
         break;

      case CLIENT_VECTOR_CATEGORY:
      case CLIENT_WVS_CATEGORY:
         client_category = CLIENT_WVS_CATEGORY;
         client_scale = scale.get_to_scale_denominator();
         return SUCCESS;

      case CLIENT_RASTER_CATEGORY:
         list = raster_list(count);
         break;
         
      case CLIENT_DTED_CATEGORY:
         list = dted_list(count);
         break;

      default:
         return FAILURE;
   }

   int i;
   for (i=0; i < count; i++)
   {
      if (list[i].source == source && list[i].scale == scale && 
         list[i].series == series)
      {
         client_category = tmp_category;
         client_scale = list[i].client_scale;
         return SUCCESS;
      }
   }

   return FAILURE;
}

static int from_client(int client_category, int client_scale, 
   MapSource &source, MapScale &scale, MapSeries &series)
{
   // set list to one of the category lists or fail
   MapType_to_client_t *list;
   int count;
   switch (client_category)
   {
      case CLIENT_BLANK_CATEGORY:
         list = blank_list(count);         
         break;

      case CLIENT_VECTOR_CATEGORY:
         switch (client_scale)
         {
         case CLIENT_WORLD:
            scale = MapScale(2147483647);
            break;

         case CLIENT_ONE_TO_100M:
            scale = MapScale(100000000);
            break;

         case CLIENT_ONE_TO_50M:
            scale = MapScale(50000000);
            break;

         case CLIENT_ONE_TO_20M:
            scale = ONE_TO_20M;
            break;

         case CLIENT_ONE_TO_10M:
            scale = ONE_TO_10M;
            break;

         case CLIENT_ONE_TO_5M:
            scale = ONE_TO_5M;
            break;

         case CLIENT_ONE_TO_2M:
            scale = ONE_TO_2M;
            break;

         case CLIENT_ONE_TO_1M:
            scale = ONE_TO_1M;
            break;

         default:
            ERR_report("Invalid client scale.");
            return FAILURE;
         }

         source = MapSource("World Vector Shoreline");
         series = "";
         return SUCCESS;

      case CLIENT_WVS_CATEGORY:
         source = MapSource("World Vector Shoreline");
         scale = MapScale(client_scale);
         series = "";
         return SUCCESS;

      case CLIENT_RASTER_CATEGORY:
         list = raster_list(count);
         break;
         
      case CLIENT_DTED_CATEGORY:
         list = dted_list(count);
         break;

      default:
         ERR_report("Invalid client category.");
         return FAILURE;
   }

   int i;
   for (i=0; i < count; i++)
   {
      if (list[i].client_scale == client_scale)
      {
         source = list[i].source;
         scale = list[i].scale;
         series = list[i].series;
         return SUCCESS;
      }
   }

   ERR_report("Invalid client scale.");
   return FAILURE;
}

// Map Server Client Define to internal MapType mappings.
///////////////////////////////////////////////////////////////////////////////

std::string map_server::get_tooltip(long layer_handle, long object_handle)
{
   std::string tooltip;

   HandlesToStringMap::iterator it = m_object_tootip_map.find(
      std::make_pair(layer_handle, object_handle));
   if ( it != m_object_tootip_map.end() )
   {
      tooltip = it->second;
   }

   return tooltip;
}

std::string map_server::get_help_text(long layer_handle, long object_handle)
{
   std::string help_txt;

   HandlesToStringMap::iterator it = m_object_helptext_map.find(
      std::make_pair(layer_handle, object_handle));
   if ( it != m_object_helptext_map.end() )
   {
      help_txt = it->second;
   }

   return help_txt;
}

std::string map_server::get_title_bar(long layer_handle, long object_handle)
{
   std::string title;

   C_map_server_ovl *overlay = LookupOverlay(layer_handle);
   
   if (overlay)
   {
      title = std::string((LPCSTR)overlay->GetDisplayName());
   }
   
   return title; 
}

std::string map_server::get_dialog_txt(long layer_handle, long object_handle)
{
   std::string comment;

   HandlesToStringMap::iterator it = m_object_comment_map.find(
      std::make_pair(layer_handle, object_handle));
   if ( it != m_object_comment_map.end() )
   {
      comment = it->second;
   }

   return comment;
}

// ICallback
STDMETHODIMP CMapServerCallback::raw_GetInfoText(long layer_handle, long object_handle, BSTR * title_bar_txt, BSTR * dialog_txt, long * result)
{
   TRY_BLOCK
   {
      *title_bar_txt = _bstr_t(m_pMapServer->get_title_bar(layer_handle, object_handle).c_str()).Detach();

      *dialog_txt = _bstr_t(m_pMapServer->get_dialog_txt(layer_handle, object_handle).c_str()).Detach();

      *result = SUCCESS;
   }
   CATCH_BLOCK_RET

   return S_OK;
}

STDMETHODIMP CMapServerCallback::raw_GetHelpText(long layer_handle, long object_handle, BSTR * help_text)
{
   TRY_BLOCK
   {
      *help_text = _bstr_t(m_pMapServer->get_help_text(layer_handle, object_handle).c_str()).Detach();
   }
   CATCH_BLOCK_RET

   return S_OK;
}

STDMETHODIMP CMapServerCallback::raw_GetToolTip(long layer_handle, long object_handle, BSTR * tool_tip)
{
   TRY_BLOCK
   {
      *tool_tip = _bstr_t(m_pMapServer->get_tooltip(layer_handle, object_handle).c_str()).Detach();
   }
   CATCH_BLOCK_RET

   return S_OK;
}


STDMETHODIMP CMapServerCallback::raw_OnSelected(long layer_handle, long object_handle, 
   long fv_parent_hWnd, double latitude, double longitude, long * result)
{
   TRY_BLOCK
   {
      *result = FAILURE;
   }
   CATCH_BLOCK_RET

   return S_OK;
}
