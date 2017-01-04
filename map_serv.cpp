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



#include "stdafx.h"
#include "map_serv.h"
#include "ovlElementContainer.h"
#include "..\..\Common\map_server_client.h"

/*
//these are defined in service.h 
#define AUTO_PT_TURNPOINT        0  //circle
#define AUTO_PT_IP               1  //square
#define AUTO_PT_TARGET           2  //triangle
#define AUTO_PT_OAP              3  //crosshairs
#define AUTO_PT_VRP              4  //diamond
#define AUTO_PT_SMALL_TURNPOINT  5  //small_circle
*/



C_map_server_ovl::C_map_server_ovl(CString name) :
	CStaticLayerOvl(::GetCurrentThreadId(), name, nullptr, "")
{
}

//----------------------------------------------------------------------------
C_map_server_ovl::C_map_server_ovl() :
	CStaticLayerOvl()
{
}


//----------------------------------------------------------------------------

C_map_server_ovl::~C_map_server_ovl() 
{
}

// IFvOverlayTypeOverrides
//

// Override the display name defined in the overlay's factory.  This is used to support
// per-instance overlay display names (e.g., an overlay created via the ILayer::CreateLayer method)
HRESULT C_map_server_ovl::get_m_displayName(BSTR *pVal)
{
   *pVal = _bstr_t(m_displayName).Detach();
   return S_OK;
}

// This relative icon will override the icon defined in the overlay's factory.  This
// is used to support per-instance overlay icons (e.g., an overlay created via the
// ILayer::CreateLayerEx interface).  This icon is displayed in the overlay manager's
// current list of opened overlays.  The filename can either be the full specification
// or a path relative to HD_DATA\icons.
HRESULT C_map_server_ovl::get_m_iconFilename(BSTR* pVal)
{
   *pVal = _bstr_t(m_icon_name).Detach();
   return S_OK;
}

long C_map_server_ovl::add_predefined_symbol(double lat, double lon, 
   int symbol, int color)
{
   return GetOvlElementContainer()->add_predefined_symbol(lat, lon, 
      symbol, 1.0, 0.0);
}

//----------------------------------------------------------------------------
long C_map_server_ovl::add_line(double lat1, double lon1, double lat2,
   double lon2, int color)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // line type default
   GetOvlElementContainer()->set_line_type(CLIENT_LINE_TYPE_SIMPLE);

   // color
   GetOvlElementContainer()->set_pen(futil->code2color(color),  UTIL_COLOR_RGB_BLACK, 
      0, 3, CLIENT_LINE_SOLID);

   return GetOvlElementContainer()->add_line(lat1, lon1, lat2, lon2, 0, 0);
}

//----------------------------------------------------------------------------
long C_map_server_ovl::add_text(double lat, double lon, 
   const char* text_string, int color, int x_offset, int y_offset)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   if ( GetOvlElementContainer()->set_font("ARIAL", 16, 0, 
      color, UTIL_COLOR_RGB_BLACK, UTIL_BG_SHADOW) != SUCCESS )
   {
      return FAILURE;
   }

   return GetOvlElementContainer()->add_text(lat, lon, text_string, x_offset, y_offset);
}

//----------------------------------------------------------------------------
long C_map_server_ovl::add_ellipse(double lat, double lon, double vertical, 
   double horizontal, double rotation, long color, long width, long style)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // line type default
   if (GetOvlElementContainer()->set_line_type(CLIENT_LINE_TYPE_SIMPLE) != SUCCESS)
   {
      return FAILURE;
   }

   if ( GetOvlElementContainer()->set_fill_type(CLIENT_FILL_NONE) != SUCCESS)
   {
      return FAILURE;
   }

   // color
   GetOvlElementContainer()->set_pen(futil->code2color(color),  UTIL_COLOR_RGB_BLACK, 
      0, 3, CLIENT_LINE_SOLID);

   return GetOvlElementContainer()->add_ellipse(lat, lon, vertical, horizontal, rotation);
}

long C_map_server_ovl::delete_object(long handle)
{
   return GetOvlElementContainer()->delete_object(handle);
}
