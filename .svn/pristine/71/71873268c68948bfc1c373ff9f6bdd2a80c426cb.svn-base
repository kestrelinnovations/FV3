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



// map_serv.h

#ifndef MAP_SERV_H
#define MAP_SERV_H

#include "LayerOvl.h"


//predefined symbols
#define AUTO_PT_TURNPOINT        0  //circle
#define AUTO_PT_IP               1  //square
#define AUTO_PT_TARGET           2  //triangle
#define AUTO_PT_OAP              3  //crosshairs
#define AUTO_PT_VRP              4  //diamond
#define AUTO_PT_SMALL_TURNPOINT  5  //small_circle
#define AUTO_TICK_MARK           6  //tick mark


class C_map_server_ovl : 
   public CStaticLayerOvl
{
   CString m_icon_name;  //icon name displayed by overlay manager  //TO DO: document specifics
   HWND m_client_window_handle;
   CString m_client_name;  //name of client application

public:
   
   //class identification functions
   const char *get_class_name() { return "C_map_server_ovl"; }

	void set_icon_name(const char* icon_name) {m_icon_name=icon_name;}
   void SetClientWindowHandle(HWND handle) {m_client_window_handle = handle;}
   void SetClientName(CString& name) {m_client_name = name;}
   HWND SetClientWindowHandle() {return m_client_window_handle;}
   CString GetClientName() {return m_client_name;}

   //delete the object with the given object handle
	long delete_object(long handle);

	long add_ellipse(double lat, double lon, double vertical, 
      double horizontal, double rotation, long color, long width, long style);
	
   long add_line(double lat1, double lon1, double lat2, double lon2,
      int color);

   long add_predefined_symbol(double lat, double lon, 
      int symbol, int color);

   long add_text(double lat, double lon, 
      const char* text_string, int color, int x_offset=0, int y_offset=0);

   C_map_server_ovl(CString name);
   C_map_server_ovl();

   virtual ~C_map_server_ovl();

   // IFvOverlayTypeOverrides
public:
   // Override the display name defined in the overlay's factory.  This is used to support
   // per-instance overlay display names (e.g., an overlay created via the ILayer::CreateLayer method)
   HRESULT get_m_displayName(BSTR *pVal);

   // This relative icon will override the icon defined in the overlay's factory.  This
   // is used to support per-instance overlay icons (e.g., an overlay created via the
   // ILayer::CreateLayerEx interface).  This icon is displayed in the overlay manager's
   // current list of opened overlays.  The filename can either be the full specification
   // or a path relative to HD_DATA\icons.
   HRESULT get_m_iconFilename(BSTR* pVal);
};

#endif  // ifndef MAP_SERV_H
