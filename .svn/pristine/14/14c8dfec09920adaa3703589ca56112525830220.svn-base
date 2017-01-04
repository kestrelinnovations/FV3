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

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

/////////////////////////////////////////////////////////////////////////////
// Map

#include "stdafx.h"
#include "IMap.h"
#include "..\Common\map_server_client.h"
#include "getobjpr.h"
#include "mapx.h"
#include "elevs.h"   // for block dted
#include "services.h"  // for MapServerUtil
#include "getobjpr.h"   // for fvw_get_frame()
#include "StatusBarInfoPane.h"
#include "..\Common\ComErrorObject.h"
#include "..\Common\SafeArray.h"
#include "statbar.h"
#include "MouseClickResource.h"
#include "MapEngineCOM.h"
#include "maps.h"

// get a map type (source, scale, series) from client category and client scale.
// Returns SUCCESS/FAILURE.
static int from_client(int client_category, int map_handle, 
   MapSource &source, MapScale &scale, MapSeries &series);

// class MapStatusBusy - used to set busy state in status bar icon
class MapStatusBusy
{
public:
   MapStatusBusy() { SetState(IDI_DISCONNECTED); }
   ~MapStatusBusy() { SetState(IDI_CONNECTED); }

protected:

   void SetState(UINT nResourceId)
   {
      CMainFrame *pFrame = fvw_get_frame();
      if (pFrame != NULL)
      {
         CIconInfoPane *pMapBusyStatus = static_cast<CIconInfoPane *>(pFrame->GetStatusBarInfoPane(STATUS_BAR_MAP_BUSY_STATUS));
         if (pMapBusyStatus)
         {
            pMapBusyStatus->SetIcon(0, nResourceId, "Loading Data");
            CMapStatusBar *pStatusBar = pMapBusyStatus->GetParentStatusBar();
            if (pStatusBar != NULL)
               pStatusBar->UpdateWindow();
         }
      }
   }
};

IMPLEMENT_DYNCREATE(Map, CCmdTarget)

// {E353BC6E-E8AB-11D3-80B9-00C04F60B086}
IMPLEMENT_OLECREATE(Map, "FalconView.Map", 0xE353BC6E, 0xE8AB, 0x11D3, 
                    0x80, 0xB9, 0x00, 0xC0, 0x4F, 0x60, 0xB0, 0x86)

Map::Map()
{
   EnableAutomation();

   m_pdisp = NULL;

   m_auto_contrast_on = true;
   m_cib_brightness = 0;
   m_cib_contrast = 0;
}

Map::~Map()
{
   try
   {
      if (m_pdisp != NULL)
         m_pdisp->Release();
   }
   catch(_com_error &)
   {
   }
}

void Map::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(Map, CCmdTarget)
   //{{AFX_MSG_MAP(Map)
   // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Map, CCmdTarget)
   //{{AFX_DISPATCH_MAP(Map)
   DISP_FUNCTION(Map, "GetMouseClick", GetMouseClick, VT_I4, VTS_I4 VTS_PI4)
   DISP_FUNCTION(Map, "GetSnapToInfo", GetSnapToInfo, VT_I4, VTS_I4 VTS_I4 VTS_PI4)
   DISP_FUNCTION(Map, "GetElevation", GetElevation, VT_I4, VTS_R8 VTS_R8 VTS_PI4)
   DISP_FUNCTION(Map, "GetMapDimensions", GetMapDimensions, VT_I4, VTS_PI4 VTS_PI4)
   DISP_FUNCTION(Map, "GetDegreesPerPixel", GetDegreesPerPixel, VT_I4, VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(Map, "CAPSPrint", CAPSPrint, VT_I4, VTS_BSTR VTS_BOOL VTS_BOOL)
   DISP_FUNCTION(Map, "GetBlockDTEDinFeet", GetBlockDTEDinFeet, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I2 VTS_I2 VTS_I2 VTS_PVARIANT)
   DISP_FUNCTION(Map, "CreateMapFromGeoBounds", CreateMapFromGeoBounds, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I2 VTS_I2 VTS_PI2 VTS_PI4 VTS_PI4 VTS_PVARIANT VTS_PVARIANT)
   DISP_FUNCTION(Map, "CreateFixedSizeMapFromGeoBounds", CreateFixedSizeMapFromGeoBounds, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I2 VTS_I2 VTS_I4 VTS_I4 VTS_PI2 VTS_PVARIANT VTS_PVARIANT)
   DISP_FUNCTION(Map, "CalcPixmapSizeFromGeoBounds", CalcPixmapSizeFromGeoBounds, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_PI4 VTS_PI4 VTS_PVARIANT VTS_I2 VTS_PI2)
   DISP_FUNCTION(Map, "GetMapDisplayString", GetMapDisplayString, VT_I4, VTS_I4 VTS_I4 VTS_PBSTR)
   DISP_FUNCTION(Map, "QueryMapTypes", QueryMapTypes, VT_I4, VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PI4 VTS_BOOL)
   DISP_FUNCTION(Map, "GetGeoRectBounds", GetGeoRectBounds, VT_I4, VTS_I4 VTS_PI4)
   DISP_FUNCTION(Map, "GetGeoCircleBounds", GetGeoCircleBounds, VT_I4, VTS_I4 VTS_PI4)
   DISP_FUNCTION(Map, "SetMapDisplay", SetMapDisplay, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(Map, "GetMapDisplay", GetMapDisplay, VT_I4, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4)
   DISP_FUNCTION(Map, "CreateMap", CreateMap, VT_I4, VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I2 VTS_I4 VTS_I4 VTS_I2 VTS_PI2 VTS_PVARIANT VTS_PVARIANT)
   DISP_FUNCTION(Map, "CalcPixmapGeoCorners", CalcPixmapGeoCorners, VT_I4, VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PVARIANT VTS_I2 VTS_PI2)
   DISP_FUNCTION(Map, "GetLatLonPositionInMap", GetLatLonPositionInMap, VT_I4, VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_R8 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PR8 VTS_PR8)
   DISP_FUNCTION(Map, "SetCIBParameters", SetCIBParameters, VT_I4, VTS_I2 VTS_R8 VTS_R8)
   DISP_FUNCTION(Map, "GetMouseClickCallback", GetMouseClickCallback, VT_I4, VTS_PI4)
   DISP_FUNCTION(Map, "GetSnapToInfoCallback", GetSnapToInfoCallback, VT_I4, VTS_I4 VTS_PI4)
   DISP_FUNCTION(Map, "GetGeoRectBoundsCallback", GetGeoRectBoundsCallback, VT_I4, VTS_PI4)
   DISP_FUNCTION(Map, "GetGeoCircleBoundsCallback", GetGeoCircleBoundsCallback, VT_I4, VTS_PI4)
   DISP_FUNCTION(Map, "SetDispatchPtr", SetDispatchPtr, VT_I4, VTS_VARIANT)
   DISP_FUNCTION(Map, "CancelClickRequest", CancelClickRequest, VT_I4, VTS_I4)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMap to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {E353BC6C-E8AB-11D3-80B9-00C04F60B086}
static const IID IID_IMap =
{ 0xe353bc6c, 0xe8ab, 0x11d3, { 0x80, 0xb9, 0x0, 0xc0, 0x4f, 0x60, 0xb0, 0x86 } };

BEGIN_INTERFACE_MAP(Map, CCmdTarget)
   INTERFACE_PART(Map, IID_IMap, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Map message handlers

long Map::GetMouseClick(long main_window_handle, long FAR* click_id) 
{
   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   *click_id = view->
      m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickWindowsMessageCallback((HWND) main_window_handle), SINGLE_CLICK);
   
   return SUCCESS;
}

long Map::GetSnapToInfo(long main_window_handle, long point_type_filter, 
                         long FAR* click_id) 
{
   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   *click_id = view->
      m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickWindowsMessageCallback((HWND) main_window_handle), SINGLE_CLICK);
   
   view->m_mouse_click_resource->set_snap_to(point_type_filter);
   
   return SUCCESS;
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

long Map::GetElevation(double lat, double lon, long FAR* elevation) 
{
   int ret = SUCCESS;
   
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
      ret = -10;  // no elevation value exists
   
   return ret;
}

long Map::GetMapDimensions(long FAR* height, long FAR* width) 
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

long Map::GetDegreesPerPixel(double lat, double lon, 
                             long category, long map_handle, 
                             double FAR* deg_lat_per_pix, 
                             double FAR* deg_lon_per_pix) 
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
   if (from_client(category, map_handle, map_source, map_scale, map_series) != 
      SUCCESS)
   {
      ERR_report("from_client() failed.");
      return FAILURE;
   }
   
   MapEngineCOM map_engine;
   map_engine.init(FALSE);
   
   try
   {
      IDispatchPtr handler;
      map_engine.GetMapHandler(map_source, &handler);
      IMapRenderPtr render = handler;
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

long Map::CAPSPrint(LPCTSTR text, BOOL print_to_scale, BOOL show_map_info) 
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

long Map::GetBlockDTEDinFeet(double NW_lat, double NW_lon, 
                              double SE_lat, double SE_lon, 
                              short number_NS_samples, short number_EW_samples, 
                              short DTED_type, VARIANT FAR* array) 
{
   if (!(array && array->vt == VT_EMPTY))  //make sure array is empty
   {
      ERR_report("Variant must be initialized or cleared");
      return FAILURE;
   }

   IDtedPtr dted;
   HRESULT hr = dted.CreateInstance(__uuidof(Dted));

   if (FAILED(hr))
   {
      ERR_report("Failed to create Dted COM object");
      return FAILURE;
   }
   
   try
   {
      _variant_t varBlock = dted->GetBlockDTED(NW_lat, NW_lon, SE_lat, SE_lon, number_NS_samples,
         number_EW_samples, DTED_type, DTED_ELEVATION_FEET);
      dted->Terminate();

      *array = varBlock.Detach();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed Getting Block DTED: (%s)", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
   
   return SUCCESS;
}

// for internal usage (instead of needing the VARIANT).  The data array should
// already be allocated
long get_block_dted_in_feet(double NW_lat, double NW_lon, 
                              double SE_lat, double SE_lon, 
                              short number_NS_samples, short number_EW_samples, 
                              short DTED_type, short* array)
{
   try 
   {

      DTED_CACHE_CLASS dted_cache;
      dted_cache.set_DTED_type(DTED_type);

      // calculate grid cell spacing 

      double total_EW_degrees = (SE_lon - NW_lon);
      if (total_EW_degrees <= 0)
         total_EW_degrees += 360;         //correct for international date line
      
      const double EW_spacing =  total_EW_degrees / number_EW_samples; 
      const double NS_spacing =  (NW_lat - SE_lat) / number_NS_samples; 

      //x and y named as conventional x/y axis
      for (long y=0; y< number_NS_samples; y++)
      {
         for (long x=0; x<number_EW_samples; x++)
         {
            double lat = NW_lat-(y+0.5)*NS_spacing;
            double lon = NW_lon+(x+0.5)*EW_spacing;
            if (lon > 180)  //correct for international date line
               lon -=360;
            
            //get elevation from cache
            boolean_t found;
            int elevation;
            int result = dted_cache.get_elev_in_feet( lat, lon, &found, &elevation);
            if (!found || result !=SUCCESS)
               elevation = MAXLONG;  

            // NOTE :: SAFEARRAYs are in memory in column major fashion, structure is copied here
            *(array+x*number_NS_samples+y) = elevation;
         }
      }  
   }
   catch (CMemoryException *e)
   {
      e->Delete();
      ERR_report("GetBlockDTEDinFeet OCMemoryException");
      
      return FAILURE;
   }

   return SUCCESS;
}

long Map::CreateMapFromGeoBounds(double southern_lat, double western_lon, 
                                 double northern_lat, double eastern_lon, 
                                 long category, long map_handle, double brightness, 
                                 short draw_overlays, short draw_map_if_no_data, 
                                 short FAR* no_data, long FAR* pixmap_width, 
                                 long FAR* pixmap_height, VARIANT FAR* dib, 
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

   MapStatusBusy mapStatusBusy;
   
   MapSource source;
   MapScale scale;
   MapSeries series;
   from_client(category, map_handle, source, scale, series);
   
   try
   {
      MapEngineCOM map_engine;
      map_engine.init(TRUE, draw_overlays);
      
      double surface_width, surface_height;
      int status = map_engine.set_map_type_from_geo_bounds(source, scale, series,
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

long Map::CreateFixedSizeMapFromGeoBounds(double southern_lat, double western_lon, 
                                          double northern_lat, double eastern_lon, 
                                          long category, long map_handle, 
                                          double brightness, short draw_overlays, 
                                          short draw_map_if_no_data, long pixmap_width, 
                                          long pixmap_height, short FAR* no_data, 
                                          VARIANT FAR* dib, VARIANT FAR* corners) 
{
   MapStatusBusy mapStatusBusy;

   //
   // check to see if the variant is initialized already
   //
   if (!(dib && dib->vt == VT_EMPTY) ||
      !(corners && corners->vt == VT_EMPTY))
   {
      ERR_report("variant not cleared");
      return FAILURE;
   }
   
   MapSource source;
   MapScale scale;
   MapSeries series;
   from_client(category, map_handle, source, scale, series);
   
   try
   {
      MapEngineCOM map_engine;
      map_engine.init(TRUE, draw_overlays);
      
      double surface_width, surface_height;
      int status = map_engine.set_map_type_from_geo_bounds(source, scale, series, 
         southern_lat, western_lon, northern_lat, eastern_lon, 0.0, 100,
         EQUALARC_PROJECTION, draw_map_if_no_data, &surface_width, &surface_height);
      
      *no_data = (status == FV_NO_DATA);
      
      //
      // If the client doesn't want a blank map drawn when there is no data,
      // then return FAILURE with the no_data flag set to TRUE.  Note - this
      // behavior is different then CreateMap which returns SUCCESS in the case
      //
      if (!draw_map_if_no_data && *no_data)
         return FAILURE;
      
      SettableMapProj *settable_map = map_engine.get_curr_map();
      
      if (status == SUCCESS || draw_map_if_no_data)
      {
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

         if (draw_overlays)
            OVL_get_overlay_manager()->invalidate_all();
      }
   }
   catch(_com_error &)
   {
      ERR_report("Failed creating map");
      return FAILURE;
   }
   
   return SUCCESS;
}

long Map::CalcPixmapSizeFromGeoBounds(double southern_lat, double western_lon, 
                                      double northern_lat, double eastern_lon, 
                                      long category, long map_handle, 
                                      long FAR* pixmap_width, long FAR* pixmap_height, 
                                      VARIANT FAR* corners, short check_for_data, 
                                      short FAR* no_data) 
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
   
   MapSource source;
   MapScale scale;
   MapSeries series;
   from_client(category, map_handle, source, scale, series);
   
   MapEngineCOM map_engine;
   
   try
   {
      map_engine.init(FALSE);
      
      double surface_width, surface_height;
      int status = map_engine.set_map_type_from_geo_bounds(source, scale, series,
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

// map the group identity to automation interface category
int GroupIdentityToCategory(int nGroupIdentity)
{
   int client_category;
   switch(nGroupIdentity)
   {
   case 1: client_category = CLIENT_BLANK_CATEGORY; break;
   case 3: client_category = CLIENT_VMAP_CATEGORY; break;
   case 4: client_category = CLIENT_DNC_CATEGORY; break;
   case 5: client_category = CLIENT_WVS_CATEGORY; break;
   case 6: client_category = CLIENT_RASTER_CATEGORY; break;
   case 7: client_category = CLIENT_DTED_CATEGORY; break;
   default:
      client_category = nGroupIdentity;
   }

   return client_category;
}

// convert automation interface category to group identity
int CategoryToGroupIdentity(int client_category)
{
   int nGroupIdentity;
   switch(client_category)
   {
   case CLIENT_BLANK_CATEGORY: nGroupIdentity = 1; break;
   case CLIENT_VMAP_CATEGORY: nGroupIdentity = 3; break;
   case CLIENT_DNC_CATEGORY: nGroupIdentity = 4; break;
   case CLIENT_WVS_CATEGORY: nGroupIdentity = 5; break;
   case CLIENT_RASTER_CATEGORY: nGroupIdentity = 6; break;
   case CLIENT_DTED_CATEGORY: nGroupIdentity = 7; break;
   default:
      nGroupIdentity = client_category;
   }

   return nGroupIdentity;
}

// convert a client category and map handle into a source, scale, and series
static int from_client(int client_category, int map_handle, 
   MapSource &source, MapScale &scale, MapSeries &series)
{
   try
   {
      IMapGroupsPtr spMapGroups;
      CO_CREATE(spMapGroups, CLSID_MapGroups);

      HRESULT hr = spMapGroups->SelectByIdentity(map_handle);
      if (hr == S_OK)
      {
         source = MapSource( spMapGroups->m_ProductName );
         scale = MapScale( spMapGroups->m_Scale, spMapGroups->m_ScaleUnits );
         series = MapSeries( spMapGroups->m_SeriesName );

         return SUCCESS;
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("from_client failed - %s", (char *)e.Description());
      ERR_report(msg);

      return FAILURE;
   }

   return FAILURE;
}

// return the number of map types have data available
long Map::get_map_type_count(CList <MapType *, MapType *> &map_list)
{
   // WARNING :: This function does not account for RMDS sources
   ASSERT(false);

   int count = 0;

   //TRACE0("\nDump of Map::get_map_type_count");

   POSITION position = map_list.GetHeadPosition();
   while (position)
   {
      MapType* pMapType = map_list.GetNext(position);
      if (pMapType->is_data_available())
      {
         //CString temp("\n");
         //temp += pMapType->get_source_string() + "\t";
         //temp += pMapType->get_scale_string() + "\t";
         //temp += pMapType->get_series_string();
         //TRACE0(temp);

         count++;
      }
   }

   return count;
}

long Map::GetMapDisplayString(long category, long map_handle, BSTR FAR* display_str) 
{
   CList<MapType *, MapType *> map_list;
   USES_CONVERSION;
   
   MapSource source;
   MapScale scale;
   MapSeries series;
   if (from_client(category, map_handle, source, scale, series) != SUCCESS)
   {
      ERR_report("from_client failed");
      return FAILURE;
   }
   
   IMapSeriesStringConverterPtr convert;
   HRESULT hr = convert.CreateInstance(__uuidof(MapSeriesStringConverter));
   if (FAILED(hr))
   {
      CString msg;
      msg.Format("Failed to create MapSeriesStringConverter.  HRESULT = %X", hr);
      ERR_report(msg);
      return FAILURE;
   }
   
   _bstr_t bstr = convert->ToString(_bstr_t(source.get_string()), scale.GetScale(),
      scale.GetScaleUnits(), _bstr_t(series.get_string()), VARIANT_FALSE,
      FORMAT_PRODUCT_NAME_SCALE_SERIES);
   *display_str = bstr.copy();
   
   return SUCCESS;
}

bool IsDataAvailable(MapType *map_type,
                     CList<MapType *, MapType *> &map_list_available)
{
   POSITION position = map_list_available.GetHeadPosition();
   while (position)
   {
      if (map_list_available.GetNext(position) == map_type)
         return true;
   }
   return false;
}

long Map::QueryMapTypes(long category, VARIANT FAR* map_handle_lst, 
                        VARIANT FAR* deg_per_pixel_lst, long FAR* map_type_count, 
                        BOOL available_map_types_only) 
{
   int nGroupIdentity = CategoryToGroupIdentity(category);

   LongSafeArray saMapHandles;
   DoubleSafeArray saDegreesPerPixel;

   try
   {
      IMapGroupsPtr spMapGroups;
      CO_CREATE(spMapGroups, CLSID_MapGroups);

      HRESULT hr = spMapGroups->SelectWithData(nGroupIdentity);
      while (hr == S_OK)
      {
         if ( spMapGroups->m_DoesDataExist || !available_map_types_only )
         {
            saMapHandles.Append( spMapGroups->m_Identity );

            MapScale mapScale( spMapGroups->m_Scale, spMapGroups->m_ScaleUnits );
            saDegreesPerPixel.Append( mapScale.get_nominal_degrees_lat_per_pixel() );
         }

         hr = spMapGroups->MoveNext();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Map::QueryMapTypes failed - %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   *map_type_count = saMapHandles.GetNumElements();
   
   *map_handle_lst = (VARIANT)saMapHandles;
   saMapHandles.Detach();

   *deg_per_pixel_lst = (VARIANT)saDegreesPerPixel;
   saDegreesPerPixel.Detach();

   return SUCCESS;
}
 
long Map::GetGeoRectBounds(long main_window_handle, long FAR* click_id) 
{
   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   *click_id = view->
      m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickWindowsMessageCallback((HWND) main_window_handle), (mouse_click_t)CLIENT_DRAG_GEORECT);

   return SUCCESS;
}

long Map::GetGeoCircleBounds(long main_window_handle, long FAR* click_id) 
{
   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   *click_id = view->
      m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickWindowsMessageCallback((HWND) main_window_handle), (mouse_click_t)CLIENT_DRAG_GEOCIRCLE);

   return SUCCESS;
}

long Map::SetMapDisplay(double lat, double lon, double rotation, 
   long client_category, long map_handle, 
   long zoom, long mask, long projection_type)
{
   //
   // return FAILURE if there is no MapView (e.g. if in print 
   // preview or if on a modal dialog box before startup)
   //
   MapView* view = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (view == NULL)
      return FAILURE;

   long map_group_id = view->GetMapGroupIdentity();
   MapType map_type = view->GetMapType();
   MapProjectionParams map_proj_params = view->GetMapProjParams();

   // If the category is not being set.
   if ((CLIENT_MASK_CATEGORY & mask) == 0)
   {
      // If the scale is being set, we will fail if a map with the requested
      // scale does not exist in the current category.
      if ((CLIENT_MASK_SCALE & mask) != 0)
      {
         map_group_id = MAP_get_category(map_type.get_source()).GetGroupId();
      }
   }
   else
   {
      // If the client_category is invalid, then lets fail now. client_category
      // must be a valid value, or a -1 to indicate no change in MapType, below
      MapCategory requested_category =
         g_map_server_util.convert_to_map_category_t(client_category);
      if (requested_category == NULL_CATEGORY)
         return FAILURE;

      map_group_id = requested_category.GetGroupId();

      // If the category is being set without setting the scale, 
      // CLIENT_CLOSEST_SCALE will be assumed
      if ((CLIENT_MASK_SCALE & mask) == 0)
      {
         mask |= CLIENT_MASK_SCALE;
         map_handle = CLIENT_CLOSEST_SCALE;
      }
   }

   // if lat is being set, set it
   if (CLIENT_MASK_LAT & mask)
      map_proj_params.center.lat = (degrees_t)lat;

   // if lon is being set, set it
   if (CLIENT_MASK_LON & mask)
      map_proj_params.center.lon = (degrees_t) lon;

   // if rotation is being set, set it
   if (CLIENT_MASK_ROTATION & mask)
      map_proj_params.rotation = (double) rotation;

   // if zoom is being set, set it
   if (CLIENT_MASK_ZOOM & mask)
      map_proj_params.zoom_percent = static_cast<int>(zoom);

   // if the projection type is being set, set it
   if (CLIENT_MASK_PROJECTION & mask)
   {
      switch (projection_type)
      {
      case CLIENT_EQUALARC_PROJECTION:
         map_proj_params.type = EQUALARC_PROJECTION;
         break;
      case CLIENT_LAMBERT_PROJECTION:
         map_proj_params.type = LAMBERT_PROJECTION;
         break;
      case CLIENT_MERCATOR_PROJECTION:
         map_proj_params.type = MERCATOR_PROJECTION;
         break;
      case CLIENT_ORTHOGRAPHIC_PROJECTION:
         map_proj_params.type = ORTHOGRAPHIC_PROJECTION;
         break;
      case CLIENT_AZIMUTHAL_EQUIDISTANT:
         map_proj_params.type = AZIMUTHAL_EQUIDISTANT_PROJECTION;
         break;
      default:
         ERR_report("Client passed in an invalid projection type");
         return FAILURE;
      }
   }

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
      if (map_handle == CLIENT_CLOSEST_SCALE ||
         map_handle == CLIENT_SMALLEST_SCALE ||
         map_handle == CLIENT_LARGEST_SCALE)
      {
         scale_is_relative = TRUE;
      }
      // UTL_change_view_map will be used.  We need to get a source, scale,
      // and series from the client_category and map_handle values.
      else
      {
         MapSource source;
         MapScale scale;
         MapSeries series;
         if (from_client(client_category, map_handle, source, scale,
            series) != SUCCESS)
         {
            ERR_report("from_client() failed.");
            return FAILURE;
         }
         map_type = MapType(source, scale, series);
      }
   }

   int status;
   
   //
   // if the specified scale is relative to the current scale, then handle
   // these separately
   //
   if (scale_is_relative)
   {
      MapCategory category = GetCategoryFromGroupId(map_group_id);
      if (map_handle == CLIENT_CLOSEST_SCALE)
      {
         status = UTL_change_view_map_to_closest(view, category,
            map_type.get_source(), map_type.get_scale(), map_type.get_series(),
            map_proj_params.center.lat, map_proj_params.center.lon,
            map_proj_params.rotation, map_proj_params.zoom_percent,
            map_proj_params.type);
      }
      else if (map_handle == CLIENT_SMALLEST_SCALE)
      {
         status = UTL_change_view_map_to_smallest_scale(view, category,
            map_proj_params.center.lat, map_proj_params.center.lon,
            map_proj_params.rotation, map_proj_params.zoom_percent,
            map_proj_params.type);
      }
      else if (map_handle == CLIENT_LARGEST_SCALE)
      {
         status = UTL_change_view_map_to_largest_scale(view, category,
            map_proj_params.center.lat, map_proj_params.center.lon,
            map_proj_params.rotation, map_proj_params.zoom_percent,
            map_proj_params.type);
      }
   }
   else
   {
      status = view->ChangeMapType(map_group_id, map_type, map_proj_params);
   }

   if (status != FAILURE)
   {
      view->Invalidate(TRUE);
      view->UpdateWindow();
   }

   return status;
}

long Map::GetMapDisplay(double FAR* lat, double FAR* lon, double FAR* rotation, 
   long FAR* category, long FAR* map_handle, long FAR* zoom, 
   long FAR* projection_type) 
{
   MapView* view = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (view == NULL)
      return FAILURE;

   const MapProj* curr_map = UTL_get_current_view_map(view);
   if (curr_map == NULL)
      return FAILURE;

   MapEngineCOM *pMapEngine = view->get_map_engine();

   // make sure the current map is valid
   if (!curr_map->is_projection_set() || pMapEngine == NULL)
      return FAILURE;
   
   //set ouput parameters converting as necessary
   *lat = curr_map->actual_center_lat();
   *lon = curr_map->actual_center_lon();
   *rotation = curr_map->actual_rotation();
   *zoom = curr_map->actual_zoom_percent();
   
   int client_map_handle;

   int nGroupIdentity = pMapEngine->GetCurrentGroupIdentity();

   // get the map series id (client_map_handle) for the current map
   try
   {
      IMapGroupsPtr spMapGroups;
      CO_CREATE(spMapGroups, CLSID_MapGroups);

      THROW_IF_NOT_OK(spMapGroups->SelectPositionOnProductScale(nGroupIdentity, _bstr_t(curr_map->source().get_string()), curr_map->scale().GetScale(),
         curr_map->scale().GetScaleUnits(), _bstr_t(curr_map->series().get_string()) ) );

      client_map_handle = spMapGroups->m_Identity;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Map::GetMapDisplay failed - %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   *category = GroupIdentityToCategory( nGroupIdentity );
   *map_handle = client_map_handle;

   ProjectionEnum proj_type = curr_map->actual_projection();
   if (proj_type == EQUALARC_PROJECTION)
      *projection_type = CLIENT_EQUALARC_PROJECTION;
   else if (proj_type == LAMBERT_PROJECTION)
      *projection_type = CLIENT_LAMBERT_PROJECTION;
   else if (proj_type == MERCATOR_PROJECTION)
      *projection_type = CLIENT_MERCATOR_PROJECTION;
   else if (proj_type == ORTHOGRAPHIC_PROJECTION)
      *projection_type = CLIENT_ORTHOGRAPHIC_PROJECTION;
   else if (proj_type == AZIMUTHAL_EQUIDISTANT_PROJECTION)
      *projection_type = CLIENT_AZIMUTHAL_EQUIDISTANT;
   
   return SUCCESS;
}

long Map::CreateMap(double center_lat, double center_lon, long category, 
   long map_handle, double rotation_angle, 
   long zoom_percent, long projection_type, 
   double brightness, short draw_overlays, 
   long pixmap_width, long pixmap_height, 
   short draw_map_if_no_data, short FAR* no_data, 
   VARIANT FAR* dib, VARIANT FAR* corners) 
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
   if (map_handle < 0)
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

   MapStatusBusy mapStatusBusy;
   
   MapSource source;
   MapScale scale;
   MapSeries series;
   from_client(category, map_handle, source, scale, series);
   
   MapEngineCOM map_engine;
   
   try
   {
      map_engine.init(TRUE, draw_overlays);
      map_engine.set_view_dimensions(pixmap_width, pixmap_height);
      
      ProjectionEnum proj_type;
      switch (projection_type)
      {
      case CLIENT_EQUALARC_PROJECTION:
         proj_type = EQUALARC_PROJECTION;
         break;
      case CLIENT_LAMBERT_PROJECTION:
         proj_type = LAMBERT_PROJECTION;
         break;
      case CLIENT_MERCATOR_PROJECTION:
         proj_type = MERCATOR_PROJECTION;
         break;
      case CLIENT_ORTHOGRAPHIC_PROJECTION:
         proj_type = ORTHOGRAPHIC_PROJECTION;
         break;
      case CLIENT_AZIMUTHAL_EQUIDISTANT:
         proj_type = AZIMUTHAL_EQUIDISTANT_PROJECTION;
         break;
      default:
         ERR_report("Client passed in an invalid projection type");
         return FAILURE;
      }
      
      int status = map_engine.change_map_type(source, scale, series, center_lat,
         center_lon, rotation_angle, zoom_percent, proj_type, draw_map_if_no_data);
      
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

long Map::CalcPixmapGeoCorners(double center_lat, double center_lon, 
   long category, long map_handle, 
   double rotation_angle, long zoom_percent, 
   long projection_type, long pixmap_width, 
   long pixmap_height, VARIANT FAR* corners, 
   short check_for_data, short FAR* no_data) 
{
   *no_data = TRUE;
   
   //
   // if the scale is a relative one (i.e. one relative to the current map such as
   // CLIENT_CLOSEST_SCALE), then it is invalid
   //
   if (map_handle < 0)
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
   
   MapSource source;
   MapScale scale;
   MapSeries series;
   from_client(category, map_handle, source, scale, series);
   
   try
   {
      MapEngineCOM map_engine;
      
      map_engine.init(FALSE);
      map_engine.set_view_dimensions(pixmap_width, pixmap_height);
      
      ProjectionEnum proj_type;
      switch (projection_type)
      {
      case CLIENT_EQUALARC_PROJECTION:
         proj_type = EQUALARC_PROJECTION;
         break;
      case CLIENT_LAMBERT_PROJECTION:
         proj_type = LAMBERT_PROJECTION;
         break;
      case CLIENT_MERCATOR_PROJECTION:
         proj_type = MERCATOR_PROJECTION;
         break;
      case CLIENT_ORTHOGRAPHIC_PROJECTION:
         proj_type = ORTHOGRAPHIC_PROJECTION;
         break;
      case CLIENT_AZIMUTHAL_EQUIDISTANT:
         proj_type = AZIMUTHAL_EQUIDISTANT_PROJECTION;
         break;
      default:
         ERR_report("Client passed in an invalid projection type");
         return FAILURE;
      }
      
      int status = map_engine.change_map_type(source, scale, series,
         center_lat, center_lon, rotation_angle, zoom_percent,
         proj_type, TRUE, FALSE);

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

long Map::GetLatLonPositionInMap(double center_lat, double center_lon, long category, 
                                 long map_handle, double rotation_angle, long zoom_percent, 
                                 long projection_type, long pixmap_width, long pixmap_height, 
                                 long pixel_column, long pixel_row, 
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
   if (from_client(category, map_handle, map_source, map_scale, map_series) != 
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
      
      ProjectionEnum proj_type;
      switch (projection_type)
      {
      case CLIENT_EQUALARC_PROJECTION:
         proj_type = EQUALARC_PROJECTION;
         break;
      case CLIENT_LAMBERT_PROJECTION:
         proj_type = LAMBERT_PROJECTION;
         break;
      case CLIENT_MERCATOR_PROJECTION:
         proj_type = MERCATOR_PROJECTION;
         break;
      case CLIENT_ORTHOGRAPHIC_PROJECTION:
         proj_type = ORTHOGRAPHIC_PROJECTION;
         break;
      case CLIENT_AZIMUTHAL_EQUIDISTANT:
         proj_type = AZIMUTHAL_EQUIDISTANT_PROJECTION;
         break;
      default:
         ERR_report("Client passed in an invalid projection type");
         return FAILURE;
      }
      
      map_engine.change_map_type(map_source, map_scale, map_series, 
         center_lat, center_lon, rotation_angle, zoom_percent,
         proj_type, FALSE, FALSE);
      
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

long Map::SetCIBParameters(short auto_contrast, double brightness, double contrast) 
{
   // bounds checks if auto contrast is on
   //
   if (auto_contrast)
   {
      if (brightness < -1.0 || brightness > 1.0)
         return FAILURE;

      if (contrast < -1.0 || contrast > 1.0)
         return FAILURE;
   }

   // setup the parameters for CIB auto contrast
   m_auto_contrast_on = auto_contrast ? true : false;
   m_cib_brightness = brightness;
   m_cib_contrast = contrast;

   return SUCCESS;
}

long Map::GetMouseClickCallback(long FAR* click_id) 
{
   if (m_pdisp == NULL)
      return E_FAIL;

   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   *click_id = view->
      m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickDispatchCallback(m_pdisp), SINGLE_CLICK);

   return SUCCESS;
}

long Map::GetSnapToInfoCallback(long point_type_filter, long FAR* click_id) 
{
   if (m_pdisp == NULL)
      return FAILURE;

   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   *click_id = view->
      m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickDispatchCallback(m_pdisp), SINGLE_CLICK);
   
   // set the click type as single click w/snap to flag
   view->m_mouse_click_resource->set_snap_to(point_type_filter);
   
   return SUCCESS;
}

long Map::GetGeoRectBoundsCallback(long FAR* click_id) 
{
   if (m_pdisp == NULL)
      return FAILURE;

   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   *click_id = view->
      m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickDispatchCallback(m_pdisp), (mouse_click_t)CLIENT_DRAG_GEORECT);

   return SUCCESS;
}

long Map::GetGeoCircleBoundsCallback(long FAR* click_id) 
{
   if (m_pdisp == NULL)
      return FAILURE;

   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   *click_id = view->
      m_mouse_click_resource->ReceiveMouseClickEvents(new CMouseClickDispatchCallback(m_pdisp), (mouse_click_t)CLIENT_DRAG_GEOCIRCLE);

   return SUCCESS;
}

long Map::SetDispatchPtr(const VARIANT FAR& dispatch_pointer) 
{
   return SetDispatch_hlpr(dispatch_pointer);
}

long Map::SetDispatch_hlpr(VARIANT FAR dispatch_pointer)
{
   try
   {
      if (m_pdisp != NULL)
         m_pdisp->Release();

      // store the dispatch pointer to the client's callback object
      m_pdisp = _variant_t(dispatch_pointer);

      return SUCCESS;
   }
   catch(_com_error &)
   {
      return FAILURE;
   }
}

long Map::CancelClickRequest(long click_id) 
{
   MapView* view = fvw_get_view();
   
   // check to see that we received a valid pointer
   if (view == NULL)
      return FAILURE;

   return view->m_mouse_click_resource->cancel_click_request(click_id);
}
