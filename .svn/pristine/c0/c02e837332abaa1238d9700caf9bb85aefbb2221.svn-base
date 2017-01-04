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



// 
// sv_obj.cpp - implementation of SkyViewObject and derived classes

#include "stdafx.h"
#include "mov_sym.h"
#include "map.h"
#include "linksdlg.h"
#include "geo_tool.h"
#include "ovl_mgr.h"

// constructor
SkyViewObject::SkyViewObject(C_overlay *parent_overlay) : C_icon(parent_overlay)
{
	//m_icon_image = NULL;

	// actually, derived classes should specify what their icon looks like
	m_icon_image = CIconImage::load_images("skyview\\square.ico");

	m_handle = -1;

	m_color = RGB(255,0,0);
	m_is_solid_not_wireframe = TRUE;

	// set units to meters
	m_units = FEET;

	// cube specific values
	m_length = (float)FEET_TO_METERS(500);
	m_width = (float)FEET_TO_METERS(500);
	m_height = (float)FEET_TO_METERS(500);
	m_altitude_msl = 2500;

	// initialize heading
	m_heading = 0;
	m_pitch = 0;
	m_roll = 0;
}

// Returns TRUE if this icon is a instance of the given class.
// For example: is_kind_of("C_icon") would return TRUE.  Derived classes
// should implement an is_kind_of function that returns TRUE for their
// class name and returns C_icon::is_kind_of(clase_name) otherwise.
boolean_t SkyViewObject::is_kind_of(const char *class_name)
{
	if (strcmp(class_name, "SkyViewObject") == 0)
		return TRUE;

	return C_icon::is_kind_of(class_name);
}

CString SkyViewObject::get_help_text() 
{ 
   return m_title;
}

CString SkyViewObject::get_tool_tip() 
{ 
   return m_title;
}

// Determine if the point is over the icon.
boolean_t SkyViewObject::hit_test(CPoint point)
{
	return m_rect.PtInRect(point);
}

// draws the current object as a 'selected' or 'hilighted' icon
void SkyViewObject::draw(map_projection_utils::CMapProjWrapper* map, CDC* dc, boolean_t selected, 
		boolean_t hilighted)
{
	boolean_t drawn;

	// calculate screen coordinates of the object
	calc_screen_coords(map);

	if (map->point_in_surface(m_x, m_y))
	{
		drawn = FALSE;
		// display the point's icon (32x32 centered)
		// we can display one of four ways:
		//		plain
		//		selected (with handles in editor)
		//		hilighted (with background)
		//		selected and hilighted (in editor)
		if (hilighted)
		{
			drawn = TRUE;
			m_icon_image->draw_hilighted(dc, m_x, m_y, 32);
		}
		if (selected)
		{
			drawn = TRUE;
			m_icon_image->draw_selected(dc, m_x, m_y, 32);
		}
		if (!drawn)
			m_icon_image->draw_icon(dc, m_x, m_y, 32);


		// set record icon's bounding box
		m_rect.SetRect(m_x - 16, m_y - 16, m_x + 16, m_y + 16);
	}
	else
		// point is not on the screen
		m_rect.SetRectEmpty();
}

void SkyViewObject::calc_screen_coords(map_projection_utils::CMapProjWrapper *map)
{
	// get the screen coordinates of the object
	map->geo_to_surface(m_geo_location.lat, m_geo_location.lon, &m_x, &m_y);
}

// invalidate the area of the screen covered by the icon
void SkyViewObject::invalidate()
{
	OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

// return tooltip text for this object
CString SkyViewObject::get_tooltip()
{
	return "SkyView Object tooltip";
}

// return help text for this object
CString SkyViewObject::get_helptext()
{
	return "SkyView Object help text";
}

boolean_t SkyViewObject::in_view(MapProj *map)
{
	return map->point_in_surface(m_x, m_y);
}

// Add links for this object, if there are any.
void SkyViewObject::add_links_to_menu(CList<CFVMenuNode*,CFVMenuNode*> &list)
{
   CString text;
	
   for(int i=0; i<m_links.GetSize(); i++)
   {
      // build menu title
      text = "Links\\" + m_links[i];
		
      // add menu node
      list.AddTail(new CCIconLparamMenuItem((LPCSTR)text, this, &view_link_callback, i));		
   }
}

// View the link at index for the point represented by icon
// static
void SkyViewObject::view_link_callback(ViewMapProj *map, C_icon* icon, LPARAM lparam)
{
   if (icon && icon->is_kind_of("SkyViewObject"))
      CLinksEditDlg::view_link(((SkyViewObject *)icon)->m_links[(int)lparam]);
}

// View all links attached to this object, if any.
void SkyViewObject::view_all_links()
{
   for(int i=0;i<m_links.GetSize();i++)
      CLinksEditDlg::view_link((LPCSTR)m_links[i]);
}

// return the number of bytes required to write this object to disk
int SkyViewObject::get_block_size()
{
	int size = 0;
	
	// position
	size += sizeof(double) * 2;
	
	// altitude
	size += sizeof(float);
	
	// color
	size += sizeof(COLORREF);
	
	// solid/wireframe flag
	size += sizeof(boolean_t);
	
	// units
	size += sizeof(units_t);
	
	// orientation
	size += sizeof(float) * 6;
	
	// size of the title plus the bytes for the title
   size += sizeof(int) + m_title.GetLength();

	// number of links this object has
	size += sizeof(int);

	// size of link strings plus the bytes for the strings themselves
	for (int i=0;i<m_links.GetSize();i++)
		size += sizeof(int) + m_links[i].GetLength();

	return size;
}

// serialize this objects data into the given block
int SkyViewObject::Serialize(BYTE *&block_ptr)
{
	// position
	memcpy(block_ptr, &m_geo_location.lat, sizeof(double));
	block_ptr += sizeof(double);
	memcpy(block_ptr, &m_geo_location.lon, sizeof(double));
	block_ptr += sizeof(double);

	// altitude
	memcpy(block_ptr, &m_altitude_msl, sizeof(float));
	block_ptr += sizeof(float);

	// color
	memcpy(block_ptr, &m_color, sizeof(COLORREF));
	block_ptr += sizeof(COLORREF);

	// solid/wireframe
	memcpy(block_ptr, &m_is_solid_not_wireframe, sizeof(boolean_t));
	block_ptr += sizeof(boolean_t);

	// units_t
	memcpy(block_ptr, &m_units, sizeof(units_t));
	block_ptr += sizeof(units_t);

	// object dimensions
	memcpy(block_ptr, &m_length, sizeof(float));
	block_ptr += sizeof(float);

	memcpy(block_ptr, &m_width, sizeof(float));
	block_ptr += sizeof(float);

	memcpy(block_ptr, &m_height, sizeof(float));
	block_ptr += sizeof(float);

	// orientation
	memcpy(block_ptr, &m_heading, sizeof(float));
	block_ptr += sizeof(float);

	memcpy(block_ptr, &m_pitch, sizeof(float));
	block_ptr += sizeof(float);

	memcpy(block_ptr, &m_roll, sizeof(float));
	block_ptr += sizeof(float);

	//
	// title
	//
	int size = m_title.GetLength();
   memcpy(block_ptr, &size, sizeof(int));
   block_ptr += sizeof(int);
      
   if (size != 0)
   {
      memcpy(block_ptr,m_title.GetBuffer(size),size);
      block_ptr += size; 
   }

	// each of the links
	int num_links = m_links.GetSize();
	memcpy(block_ptr, &num_links, sizeof(int));
	block_ptr += sizeof(int);

	for (int i=0; i<num_links;i++)
	{
		int size = m_links[0].GetLength();
		memcpy(block_ptr, &size, sizeof(int));
		block_ptr += sizeof(int);

		if (size != 0)
		{
			memcpy(block_ptr, m_links[i].GetBuffer(size),size);
			block_ptr += size;
		}
	}

	return SUCCESS;
}

// deserialize the data from the given block into this object
int SkyViewObject::Deserialize(BYTE *&block_ptr)
{
	// position
	memcpy(&m_geo_location.lat, block_ptr, sizeof(double));
	block_ptr += sizeof(double);
	memcpy(&m_geo_location.lon, block_ptr, sizeof(double));
	block_ptr += sizeof(double);
	
	// altitude
	memcpy(&m_altitude_msl, block_ptr, sizeof(float));
	block_ptr += sizeof(float);
	
	// color
	memcpy(&m_color, block_ptr, sizeof(COLORREF));
	block_ptr += sizeof(COLORREF);
	
	// solid/wireframe
	memcpy(&m_is_solid_not_wireframe, block_ptr, sizeof(boolean_t));
	block_ptr += sizeof(boolean_t);
	
	// units_t
	memcpy(&m_units, block_ptr, sizeof(units_t));
	block_ptr += sizeof(units_t);
	
	// object dimensions
	memcpy(&m_length, block_ptr, sizeof(float));
	block_ptr += sizeof(float);
	
	memcpy(&m_width, block_ptr, sizeof(float));
	block_ptr += sizeof(float);
	
	memcpy(&m_height, block_ptr, sizeof(float));
	block_ptr += sizeof(float);
	
	// orientation
	memcpy(&m_heading, block_ptr, sizeof(float));
	block_ptr += sizeof(float);
	
	memcpy(&m_pitch, block_ptr, sizeof(float));
	block_ptr += sizeof(float);
	
	memcpy(&m_roll, block_ptr, sizeof(float));
	block_ptr += sizeof(float);
	
	// size of the title string
	int size;
	memcpy(&size,block_ptr,sizeof(int));
	block_ptr += sizeof(int);

	// title
	if (size != 0) 
	{
		char tmp_buffer[256];
		memcpy(tmp_buffer,block_ptr,size);
		block_ptr += size;

		tmp_buffer[size] = '\0';
		
		m_title = CString(tmp_buffer);
	}

	// read in each of the links

	int num_links;
	memcpy(&num_links, block_ptr, sizeof(int));
	block_ptr += sizeof(int);

	for (int i=0; i<num_links; i++)
	{
		int size;
		memcpy(&size, block_ptr, sizeof(int));
		block_ptr += sizeof(int);

		if (size != 0)
		{
			char tmp_buffer[256];
			memcpy(tmp_buffer, block_ptr, size);
			block_ptr += size;
			
			tmp_buffer[size] = '\0';
			
			m_links.Add(CString(tmp_buffer));
		}
	}
	
	return SUCCESS;
}

// return the info text displayed in the info dialog for the object
CString SkyViewObject::get_info_text()
{
	CString info_text;
	
	// put title in the top
	info_text = "Name: "+ m_title +"\r\n";

   const int STR_LEN = 41;
	char lat_lon_str[STR_LEN];
   const int DATUM_LEN = 6;
   char datum[DATUM_LEN];
   const int DATUM_TYPE_LEN = 21;
   char display_type[DATUM_TYPE_LEN];
	
   // convert lat-lon to formatted string in default format and datum
   GEO_lat_lon_to_string(m_geo_location.lat, m_geo_location.lon, lat_lon_str, STR_LEN);
	
   // get default datum
   GEO_get_default_display(display_type, DATUM_TYPE_LEN);
   if (strcmp(display_type, "SECONDARY") == 0) 
      GEO_get_secondary_datum(datum, DATUM_LEN);
   else 
      GEO_get_primary_datum(datum, DATUM_LEN);
	
   // put Location string in edit string
   info_text += "Location: (";
	info_text += datum;
	info_text += ")  ";
	info_text += lat_lon_str;
	info_text += "\r\n";
	
	// altitude
	CString alt;
	info_text += "Altitude : ";
	alt.Format("%0.4f", m_altitude_msl);
	info_text += alt + "\r\n";
	
	// object's dimensions
      /*
      COMPILER WRINKLE
      dvl -- 2001/02/08
      I changed this from a float to a double.  If you change this back to a float the 
      following message is given building the release version (on the ending '}' of this function:
            C:\MSVC\fvw 3.2.0\mov_sym\sv_obj.cpp(451) : fatal error C1001: INTERNAL COMPILER ERROR
            (compiler file 'E:\8799\vc98\p2\src\P2\x86\MDmisc.c', line 2262)
            Please choose the Technical Support command on the Visual C++
            Help menu, or open the Technical Support help file for more information
      
      The statement the compiler does not like is the :
         	dim_str.Format("\r\nLength : %0.2f (%s)\r\nWidth : %0.2f\r\nHeight : %0.2f\r\n", 
               length, units_str, width, height);
      following the switch statment below.  It does not like sending a FLOAT VARIABLE into the
      CString::Format where the %f expects a double.  For some reason the automatic conversion
      is not being done here!  A simple cast on the function call DID NOT WORK.
      */
   	double length, width, height;

	CString units_str;
	switch(get_units())
   {
	case NM : 
		length = (float)METERS_TO_NM(m_length);
		width = (float)METERS_TO_NM(m_width);
		height = (float)METERS_TO_NM(m_height);
		units_str.Format("NM");
		break;
	case KM : 
		length = (float) (m_length / 1000.0); 
		width = (float) (m_width / 1000.0);
		height = (float) (m_height / 1000.0);
		units_str.Format("km");
		break;
	case M : 
		length = m_length;
		width = m_width;
		height = m_height;
		units_str.Format("m");
		break;
	case YARDS : 
		length = (float)(METERS_TO_FEET(m_length) / 3.0);
		width = (float)(METERS_TO_FEET(m_width) / 3.0);
		height = (float)(METERS_TO_FEET(m_height) / 3.0);
		units_str.Format("yards");
		break;
	case FEET : 
		length = (float)METERS_TO_FEET(m_length);
		width = (float)METERS_TO_FEET(m_width);
		height = (float)METERS_TO_FEET(m_height);
		units_str.Format("ft");
		break;
	}
	CString dim_str;
	dim_str.Format("\r\nLength : %0.2f (%s)\r\nWidth : %0.2f\r\nHeight : %0.2f\r\n", 
		length, units_str, width, height);
	info_text += dim_str;

	// orientation
	CString orientation_str;
	orientation_str.Format("\r\nHeading : %0.2f°\r\nPitch : %0.2f°\r\nRoll : %0.2f°\r\n",
		m_heading, m_pitch, m_roll);
	info_text += orientation_str;

	// links
	info_text += "\r\nLinks :\r\n";
	for (int i=0;i<m_links.GetSize();i++)
		info_text += CString("\t") + m_links[i] + "\r\n";
	if (m_links.GetSize() == 0)
		info_text += "\tnone\r\n";

	return info_text;
}
