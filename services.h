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



// services.h : header file
//
#ifndef SERVICES
#define SERVICES

#include "map_serv.h"
#include "maps_d.h"

/////////////////////////////////////////////////////////////////////////////
// map_server command target

//forward
class C_gps_trail;
class GRA_dib_section_pixmap_t;
class SettableMapProj;
class map_server;

class MapServerUtil
{
public:
	
	int get_map_corners(const MapProj* map, COleSafeArray* corners);
	
	// client category #define from a map_category_t.  Returns -1 for failure.
	int convert_to_client_category(MapCategory server_category);
	
	// MapCategory from a client category #define.  Returns NULL_CATEGORY
	// for failure.
	MapCategory convert_to_map_category_t(int client_category);

	int get_map_corners_in_variant(const SettableMapProj* map, 
   VARIANT* corners_variant);

   int corners_to_variant(SettableMapProj *map, COleSafeArray *corners);

   // convert client overlay type into overlay type guid
   GUID TypeToOverlayGuid(int type);

   // retreive a constant from an overlay type guid
   int OverlayGuidToType(GUID overlayDescGuid);
};

extern MapServerUtil g_map_server_util;


// ICallback implementation needed since the map server implementation
// is based on CStaticLayerOvl which uses ICallback to get the tooltip,
// help text, and info box text.

class CMapServerCallback :
   public ATL::CComObjectRootEx<ATL::CComSingleThreadModel>,
   public IDispatchImpl<fvw::ICallback, &__uuidof(fvw::ICallback), &fvw::LIBID_fvw, /* wMajor = */ 4>
{
public:

   CMapServerCallback() 
      : m_pMapServer(nullptr)
   {
   }
   virtual ~CMapServerCallback()
   {
   }


BEGIN_COM_MAP(CMapServerCallback)
   COM_INTERFACE_ENTRY2(IDispatch, fvw::ICallback)
   COM_INTERFACE_ENTRY(fvw::ICallback)
END_COM_MAP()

   virtual void Initialize(map_server* pMapServer)
   {
      m_pMapServer = pMapServer;
   }
   

	// ICallback
public:
	STDMETHOD(raw_GetHelpText)(long layer_handle, long object_handle, BSTR * help_text);
	STDMETHOD(raw_GetInfoText)(long layer_handle, long object_handle, BSTR * title_bar_txt, BSTR * dialog_txt, long * result);
	STDMETHOD(raw_GetMenuItems)(long layer_handle, long object_handle, BSTR * menu_text) { return S_OK; }
	STDMETHOD(raw_GetTimeSpan)(long layer_handle, DATE * begin, DATE * end) { return S_OK; }
	STDMETHOD(raw_GetToolTip)(long layer_handle, long object_handle, BSTR * tool_tip);
   STDMETHOD(raw_OnFalconViewExit)(long layer_handle) { return S_OK; }
	STDMETHOD(raw_OnOverlayClose)(long layer_handle) { return S_OK; }
	STDMETHOD(raw_OnSelected)(long layer_handle, long object_handle, long fv_parent_hWnd, double latitude, double longitude, long * result);
	STDMETHOD(raw_OnToolbarButtonPressed)(long toolbar_id, long button_number) { return S_OK; }
	STDMETHOD(raw_SetCurrentViewTime)(long layer_handle, DATE date) { return S_OK; }
	STDMETHOD(raw_OnMouseClick)(long click_id, double latitude, double longitude) { return S_OK; }
	STDMETHOD(raw_OnMouseClickCanceled)(long click_id) { return S_OK; }
	STDMETHOD(raw_OnSnapToInfo)(long click_id, double lat, double lon, long point_type, BSTR key_text) { return S_OK; }
	STDMETHOD(raw_OnSnapToInfoCanceled)(long click_id) { return S_OK; }
	STDMETHOD(raw_OnGeoRectBounds)(long click_id, double NW_lat, double NW_lon, double SE_lat, double SE_lon) { return S_OK; }
	STDMETHOD(raw_OnGeoRectBoundsCanceled)(long click_id) { return S_OK; }
	STDMETHOD(raw_OnGeoCircleBounds)(long click_id, double lat, double lon, double radius) { return S_OK; }
	STDMETHOD(raw_OnGeoCircleBoundsCanceled)(long click_id) { return S_OK; }
	STDMETHOD(raw_OnPreClose)(long layer_handle, long * cancel) { return S_OK; }
	STDMETHOD(raw_OnDoubleClicked)(long layer_handle, long object_handle, long fvw_parent_hWnd, double lat, double lon) { return S_OK; }

protected:
   map_server* m_pMapServer;
};

typedef std::pair<long, long> HandleKeyType;
typedef std::map<HandleKeyType, std::string> HandlesToStringMap;

class map_server : public CCmdTarget
{
	DECLARE_DYNCREATE(map_server)
   DECLARE_OLECREATE(map_server)

	map_server();           // protected constructor used by dynamic creation
   	
private:
   
   // ICallback implementation to handle the tooltip,
   // help text and info dialog text
   CComObject<CMapServerCallback> *m_callback;
   HandlesToStringMap m_object_tootip_map;
   HandlesToStringMap m_object_helptext_map;
   HandlesToStringMap m_object_comment_map;


	HWND m_client_window_handle;
	CString m_client_name;
	int CloseStaticOverlay(GUID overlayDescGuid);
	int OpenStaticOverlay(GUID overlayDescGuid);
 	int CloseFileOverlay(GUID overlayDescGuid, const char* filespec);
	int OpenFileOverlay(GUID overlayDescGuid, const char* filespec);

	BOOL m_client_registered_with_server;

   int m_next_overlay_handle; //the handle to the next overlay layer to be allocated
   int m_next_gps_handle;     //the handle to the next gps trail to be created

   //maps of handles to pointers
   CMap <int, int, C_map_server_ovl*, C_map_server_ovl*> m_overlay_map;  //map of layer handles to map server layers
   CMap <int, int, C_gps_trail*, C_gps_trail*> m_gps_map;          //map of layer handles to map
   
   static int m_num_clients;    // number of clients connected to the map server
                              
   static void increment_num_clients(void) { m_num_clients++; }                            
   static void decrement_num_clients(void) { m_num_clients--; }      
   

   //client server constant (#define/enum) conversions
   //color
 	int translate_client_color(int client_color);
  
   //lookup the handle in the overlay map and return a pointer to the overlay
   C_map_server_ovl* LookupOverlay(long handle) const;

public:
   map_server(bool no_automation);
	virtual ~map_server();


// Attributes
public:


// Operations
public:
   std::string get_tooltip(long layer_handle, long object_handle);
   std::string get_help_text(long layer_handle, long object_handle);
   std::string get_title_bar(long layer_handle, long object_handle);
   std::string get_dialog_txt(long layer_handle, long object_handle);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(map_server)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL


// Implementation
static int get_num_clients(void)  //only acceses static members
      { return m_num_clients; }                            
 

protected:

	// Generated message map functions
	//{{AFX_MSG(map_server)
		// NOTE - the ClassWizard will add and remove member functions here.

   //}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(map_server)
	afx_msg long CreateLayer(LPCTSTR layer_name);
	afx_msg long DeleteLayer(long handle);
	afx_msg long RegisterLayer(long handle);
	afx_msg long SetMapDisplay(double lat, double lon, double rotation, long client_category, long scale, long zoom, long mask);
	afx_msg long Refresh();
	afx_msg long AddEllipse(long handle, double lat, double lon, double vertical, double horizontal, double rotation, long color);
	afx_msg long AddPredefinedSymbol(long layer_handle, double lat, double lon, long symbol, long color);
	afx_msg long AddText(long layer_handle, double lat, double lon, LPCTSTR text, long color, long x_offset, long y_offset);
	afx_msg long DeleteObject(long layer_handle, long object_handle);
	afx_msg long AddLine(long handle, double lat1, double lon1, double lat2, double lon2, long color);
	afx_msg long RegisterWithMapServer(LPCTSTR client_name, long window_handle);
	afx_msg long GetElevation(double lat, double lon, long FAR* elevation);
	afx_msg long GetMapDimensions(long FAR* height, long FAR* width);
	afx_msg long SetObjectTooltip(long layer_handle, long object_handle, LPCTSTR tooltip);
	afx_msg long SetObjectComment(long layer_handle, long object_handle, LPCTSTR comment);
	afx_msg long SetObjectHelpText(long layer_handle, long object_handle, LPCTSTR help_text);
	afx_msg long GetDegreesPerPixel(double lat, double lon, long category, long scale, double FAR* deg_lat_per_pix, double FAR* deg_lon_per_pix);
	afx_msg long GetMapDisplay(double FAR* lat, double FAR* lon, double FAR* rotation, long FAR* category, long FAR* scale, long FAR* zoom);
	afx_msg long OpenOverlay(long type, LPCTSTR filespec);
	afx_msg long CloseOverlay(long type, LPCTSTR filespec);
	afx_msg long CreateLayerEx(LPCTSTR layer_name, LPCTSTR icon_name);
	afx_msg long CAPSPrint(LPCTSTR text, BOOL print_to_scale, BOOL show_map_info);
	afx_msg long CreateGPSTrail(LPCTSTR filespec, long reserved, long options, long sample_period);
	afx_msg long AddGPSPoint(long handle, float latitude, float longitude, float speed_knots, float speed_km_hr, float true_heading, float magnetic_heading, float msl, float time, short day, short month, short year, short satellites);
	afx_msg SCODE GetBlockDTEDinFeet(double NW_lat, double NW_lon, double SE_lat, double SE_lon, short number_NS_samples, short number_EW_samples, short DTED_type, VARIANT FAR* array);
	afx_msg SCODE GetMouseClick(long main_window_handle, long FAR* click_id);
	afx_msg long CreateMap(double center_lat, double center_lon, long category, long scale, double rotation_angle, long zoom_percent, double brightness, short draw_overlays, long pixmap_width, long pixmap_height, short draw_map_if_no_data, short* no_data, VARIANT* dib, VARIANT* corners);
	afx_msg long AddMovableSymbol(long layer_handle, long symbol_type, long symbol_flags);
	afx_msg long DeleteMovableSymbol(long layer_handle, long symbol_handle);
	afx_msg long MoveMovableSymbol(long layer_handle, long symbol_handle, double lat, double lon, double altitude, double heading);
	afx_msg long RequestLayerNotification(long layer_handle, long hWnd, long type);
	afx_msg long GetLatLonPositionInMap(double center_lat, double center_lon, long category, long scale, double rotation_angle, long zoom_percent, long pixmap_width, long pixmap_height, long pixel_column, long pixel_row, double FAR* pixel_latitude, double FAR* pixel_longitude);
	afx_msg long CreateMapFromGeoBounds(double southern_lat, double western_lon, double northern_lat, double eastern_lon, long category, long scale, double brightness, short draw_overlays, short draw_map_if_no_data, short FAR* no_data, long FAR* pixmap_width, long FAR* pixmap_height, VARIANT FAR* dib, VARIANT FAR* corners);
	afx_msg long CreateFixedSizeMapFromGeoBounds(double southern_lat, double western_lon, double northern_lat, double eastern_lon, long category, long scale, double brightness, short draw_overlays, short draw_map_if_no_data, long pixmap_width, long pixmap_height, short FAR* no_data, VARIANT FAR* dib, VARIANT FAR* corners);
	afx_msg long CalcPixmapGeoCorners(double center_lat, double center_lon, long category, long scale, double rotation_angle, long zoom_percent, long pixmap_width, long pixmap_height, VARIANT FAR* corners, short check_for_data, short FAR* no_data);
	afx_msg long CalcPixmapSizeFromGeoBounds(double southern_lat, double western_lon, double northern_lat, double eastern_lon, long category, long scale, long FAR* pixmap_width, long FAR* pixmap_height, VARIANT FAR* corners, short check_for_data, short FAR* no_data);
   //}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

#endif
