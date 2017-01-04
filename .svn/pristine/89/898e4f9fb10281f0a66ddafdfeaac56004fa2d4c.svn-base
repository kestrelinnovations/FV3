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
#include "ovlElementContainer.h"
#include "OvlTerrainMask.h"
#include "ovlelem.h"
#include "ovlutil.h" 
#include "Map.h"
#include "..\..\Common\map_server_client.h"
#include "ovl_mgr.h"
#include "OvlMilStd2525.h"

// destructor for class ObjectGroup
ObjectGroup::~ObjectGroup()
{
   if (m_object_list == NULL)
      return;

   // remove the objects from the list but do not delete
   // the elements pointers since they will be deleted 
   // outside the group
   while (!m_object_list->IsEmpty())
      m_object_list->RemoveTail();
}  

void CIconSet::add(CString filename, int rotation)
{
   icon_set_element_t element(filename, rotation);
   m_icon_lst.AddTail(element);
}  

CString CIconSet::get_filename(int rotation)
{
   POSITION position = m_icon_lst.GetHeadPosition();
   while (position)
   {
      icon_set_element_t element = m_icon_lst.GetNext(position);
      if (element.rotation == rotation)
         return element.filename;
   }
   
   // couldn't find the given rotation in the list
   return "";
}

OvlElementContainer::~OvlElementContainer()
{
	EnterCriticalSection(&m_critical_section);

   // free up any memory in the object list or object trash can
   while (!m_object_list.IsEmpty())
      delete m_object_list.RemoveTail();
   
	POSITION pos = m_group_map.GetStartPosition();
   while (pos)
   {
      long key;
		ObjectGroup *p;
      m_group_map.GetNextAssoc(pos, key, p);

      delete p->m_object_list;
		p->m_object_list = NULL;
      delete p;

      m_group_map.RemoveKey(key);
   }  

   pos = m_icon_set_map.GetStartPosition();
   while (pos)
   {
      long key;
      CIconSet* p;
      m_icon_set_map.GetNextAssoc(pos, key, p);

      delete p;
      m_icon_set_map.RemoveKey(key);
   }

	LeaveCriticalSection(&m_critical_section);
	DeleteCriticalSection(&m_critical_section);
}

OvlElementContainer::OvlElementContainer() :
m_font(m_allow_redraw),
m_pen(m_allow_redraw)
{
	m_next_handle = 1;
	m_next_icon_set_handle = 1;
	m_text_rotation = 0.0;

	// initialize the pen, font, line type, and fill type
	initialize_defaults();

	InitializeCriticalSection(&m_critical_section);

   m_invalidation_rect.SetRectEmpty();
}

// initialize the pen, font, line type, and fill type
void OvlElementContainer::initialize_defaults()
{
   // initialize pen
   set_pen(RGB(255,0,0), RGB(0,0,0), FALSE, 3, UTIL_LINE_SOLID);

   // initialize font
   set_font("Arial", 12, /* attributes = */0,
             UTIL_COLOR_WHITE, UTIL_COLOR_BLACK, UTIL_BG_SHADOW);

   // initialize line type
   set_line_type(CLIENT_LINE_TYPE_SIMPLE);

   // initialize fill type
   set_fill_type(CLIENT_FILL_NONE);

   // initialize anchor type
   m_anchor_type = CLIENT_ANCHOR_UPPER_LEFT;
}

// returns the group that the given object is a member of.  Returns
// NULL if the object is not a member of any group
ObjectGroup *OvlElementContainer::object_is_member_of(OvlElement *object)
{
   POSITION position = m_group_map.GetStartPosition();
   POSITION obj_position;

   while (position)
   {
      long key;
      ObjectGroup *object_group;

      m_group_map.GetNextAssoc(position, key, object_group);

      if ((obj_position = object_group->m_object_list->Find(object)) != NULL)
         return object_group;
   }

   return NULL;
}

// return the object id of the given object.  Returns -1 if the object 
// was not found in the object map
int OvlElementContainer::get_object_id(OvlElement *object)
{
   POSITION position = m_object_map.GetStartPosition();

   // loop through each element of the object map
   while (position)
   {
      int key;
      OvlElement *loop_object;

      // get the next key and OvlElement from the object map
      m_object_map.GetNextAssoc(position, key, loop_object);

      // if the object passed in is the current object in the object
      // map then return the associated key
      if (object == loop_object)
         return key;
   }

   // couldn't find the given object in the object map
   return -1;
}
// set the current pen
int OvlElementContainer::set_pen(COLORREF fg_color, COLORREF bg_color, boolean_t turn_off_background, 
                       int line_width, int line_style)
{
   // Set the foreground pen
   if (m_pen.set_foreground_pen(fg_color, line_style, line_width) !=
      SUCCESS)
      return FAILURE;

   // Set the background pen.  A solid line with a width 2 greater than the
   // foreground width can be drawn with the given color beneath the
   // object.  Using color of -1 turns off the background.
   if (m_pen.set_background_pen(bg_color, turn_off_background) != SUCCESS)
      return FAILURE;

   return SUCCESS;
}

// set the current font
int OvlElementContainer::set_font(const char *name, int size, int attributes,
              int fg_color, int bg_color, int bg_type)
{
   // Set the font.  See utils.h for font attributes.
   if (m_font.set_font(name, size, attributes) != SUCCESS)
      return FAILURE;

   // Set the text foreground color.
   if (m_font.set_foreground(fg_color) != SUCCESS)
      return FAILURE;

   // Set text background type and color.  See utils.h for text background
   // types.
   if (m_font.set_background(bg_type, bg_color) != SUCCESS)
      return FAILURE;

   return SUCCESS;
}

// set the current font using RGB values
int OvlElementContainer::set_font_RGB(const char *name, int size, int attributes,
								 COLORREF fg_color, COLORREF bg_color, int bg_type)
{
	// set the font.
	if (m_font.set_font(name, size, attributes) != SUCCESS)
	{
		ERR_report("[OvlElementContainer::set_font_RGB] unable to set font");
		return FAILURE;
	}

	// set the text foreground color
	if (m_font.set_foreground_RGB(fg_color) != SUCCESS)
	{
		ERR_report("[OvlElementContainer::set_font_RGB] unable to set foreground color");
		return FAILURE;
	}

	// set the text background type and color
	if (m_font.set_background_RGB(bg_type, bg_color) != SUCCESS)
	{
		ERR_report("[OvlElementContainer::set_font_RGB] unable to set background type/color");
		return FAILURE;
	}

	return SUCCESS;
}
									 

// set the rotation angle for text objects
int OvlElementContainer::set_text_rotation(double angle)
{
	m_text_rotation = angle;

	return SUCCESS;
}

// set the line type
int OvlElementContainer::set_line_type(int line_type)
{
   m_line_type = line_type;
   
   return SUCCESS;
}

// set the fill type
int OvlElementContainer::set_fill_type(int fill_type)
{
   m_fill_type = fill_type;

   return SUCCESS;
}

// set the anchor type
int OvlElementContainer::set_anchor_type(int anchor_type)
{
   m_anchor_type = anchor_type;

   return SUCCESS;
}

// uses the current pen, font, and fill types to modify the given object
int OvlElementContainer::modify_object(int object_handle)
{
   OvlElement *object;
   
   if (m_object_map.Lookup(object_handle, object) == 0)
      return FAILURE;

   // erase where the object is
   object->invalidate();

   // update the object's properties
   if (object->is_kind_of("OvlLine"))
   {
      OvlPen &pen = ((OvlLine *)object)->get_pen();
      pen = m_pen;

      // set the font (for embedded text)
		OvlFont &font = ((OvlLine *)object)->get_font();
		font = m_font;
   }
	else if (object->is_kind_of("GeoArc"))
   {
      OvlPen &pen = ((GeoArc *)object)->get_pen();
      pen = m_pen;
   }
   else if (object->is_kind_of("GeoLine"))
   {
      OvlPen &pen = ((GeoLine *)object)->get_pen();
      pen = m_pen;

      // set the font (for embedded text)
		OvlFont &font = ((GeoLine *)object)->get_font();
		font = m_font;
   }
   else if (object->is_kind_of("OvlPolygon"))
   {
      OvlPen &pen = ((OvlPolygon *)object)->get_pen();
      pen = m_pen;

      ((OvlPolygon *)object)->set_fill_type(m_fill_type);
   }
   else if (object->is_kind_of("OvlEllipse"))
   {
      OvlPen &pen = ((OvlEllipse *)object)->get_pen();
      pen = m_pen;

      ((OvlEllipse *)object)->set_fill_type(m_fill_type);
   }
   else if (object->is_kind_of("OvlSymbol"))
   {
      OvlPen &pen = ((OvlSymbol *)object)->get_pen();
      pen = m_pen;
   }
   else if (object->is_kind_of("OvlText"))
   {
      OvlFont &font = ((OvlText *)object)->get_font();
		((OvlText *)object)->set_angle(m_text_rotation);
      font = m_font;
   }
	else if (object->is_kind_of("OvlRgn"))
	{
		((OvlRgn *)object)->set_brush(m_pen, m_fill_type);
	}
	else if (object->is_kind_of("OvlIcon"))
	{
		OvlText *text = ((OvlIcon *)object)->get_text_object();
		
		OvlFont &font = text->get_font();
		font = m_font;
	}
	else if (object->is_kind_of("GeoBounds"))
	{
		// set the pen of the geobounds using the currently set pen
		OvlPen &pen = ((GeoBounds *)object)->get_pen();
		pen = m_pen;
	}
	else if (object->is_kind_of("OvlTerrainMask"))
	{
		// set the pen of the geobounds using the currently set pen
		((OvlTerrainMask *)object)->set_brush(m_pen, m_fill_type);
	}


	MapProj *map = OVL_get_overlay_manager()->get_view_map();
	if (map && map->is_projection_set())
	{
		object->prepare_for_redraw((MapProj *)
			OVL_get_overlay_manager()->get_view_map());
	}

   // invalidate again so the object is updated with the new properties
   object->invalidate();

   return SUCCESS;
}

long OvlElementContainer::add_object(OvlElement *object)
{
	EnterCriticalSection(&m_critical_section);

   // add the object to the list
   m_object_list.AddTail(object);
   
   // put object and its handle into the map
   m_object_map.SetAt(m_next_handle, object); 

   // prepare the object for redraw (this does not actually draw
   // anything to the screen).  This will allow an entire collection 
   // of objects to be drawn to the screen at once with a call to invalidate()
	MapProj *map = OVL_get_overlay_manager()->get_view_map();
	if (map && map->is_projection_set())
	{
		object->prepare_for_redraw((MapProj *)
			OVL_get_overlay_manager()->get_view_map());
	}

	m_invalidation_rect |= object->get_rect();

	LeaveCriticalSection(&m_critical_section);

   // return the handle of the object and increment handle
   return m_next_handle++;
}

// delete the object with the given object handle
long OvlElementContainer::delete_object(long object_handle)
{
   OvlElement *object;

   if (m_object_map.Lookup(object_handle, object) == 0)
		return FAILURE;

	EnterCriticalSection(&m_critical_section);
      
   // remove object from map
   m_object_map.RemoveKey(object_handle);         

   // remove from the object list
   POSITION position = m_object_list.Find(object);
   if (position)
   {
      // compute the current invalidation rect for the object
      MapProj *map = OVL_get_overlay_manager()->get_view_map();
      if (map && map->is_projection_set())
      {
         object->prepare_for_redraw((MapProj *)
            OVL_get_overlay_manager()->get_view_map());
      }

      m_object_list.RemoveAt(position);
      m_invalidation_rect |= object->get_rect();
      delete object;
   }

	LeaveCriticalSection(&m_critical_section);
     
   return SUCCESS;
}

// delete all objects in this layer
long OvlElementContainer::delete_all_objects()
{
   POSITION position = m_object_map.GetStartPosition();

	EnterCriticalSection(&m_critical_section);
   
   // loop through all the objects in the object map and delete them
   while (position)
   {
      int object_handle;
      OvlElement *object;
      
      m_object_map.GetNextAssoc(position, object_handle, object);
      
      // remove object from map
      m_object_map.RemoveKey(object_handle);
      
      // remove from the object list
      POSITION obj_position = m_object_list.Find(object);
      if (obj_position)
      {
         m_object_list.RemoveAt(obj_position);
         m_invalidation_rect |= object->get_rect();
         delete object;
      }
   }

	// clear group map
	POSITION pos = m_group_map.GetStartPosition();
   while (pos)
   {
      long key;
		ObjectGroup *p;
      m_group_map.GetNextAssoc(pos, key, p);

      delete p->m_object_list;
		p->m_object_list = NULL;
      delete p;

      m_group_map.RemoveKey(key);
   } 

	LeaveCriticalSection(&m_critical_section);

   m_next_handle = 0;

   return SUCCESS;
}

// add a line to this layer using the current pen and current line type
long OvlElementContainer::add_line(double lat1, double lon1, double lat2, double lon2,
                         long x_offset, long y_offset, 
								 CString embedded_text /*= ""*/)
{
   OvlContour *line;
   
   // create a new line based on the current m_line_type
   switch (m_line_type)
   {
   case CLIENT_LINE_TYPE_RHUMBLINE:
      line = new GeoLine(GeoSegment::RHUMB_LINE);
      break;
   case CLIENT_LINE_TYPE_GREATCIRCLE:
      line = new GeoLine();
      break;
   // simple line style and a catch for any bad m_line_type value
   default:
      line = new OvlLine();
   }

   // define the points for a rhumb line or great circle
   if (m_line_type == CLIENT_LINE_TYPE_RHUMBLINE ||
       m_line_type == CLIENT_LINE_TYPE_GREATCIRCLE)
   {
		GeoLine *geo_line = (GeoLine *)line;

      geo_line->set_start(lat1, lon1);
      geo_line->set_end(lat2, lon2);

		// if the parameter list contains embedded text then
		// add the appropriate settings to the OvlLine object
		if (!embedded_text.IsEmpty())
		{
			// set the text
			geo_line->set_embedded_text(embedded_text);

			// set the font
			OvlFont &font = geo_line->get_font();
			font = m_font;
		}
   }
   // define the points for a simple line
   else
   {
		OvlLine *simple_line = (OvlLine *)line;
      d_geo_t geo1, geo2;
      
      geo1.lat = lat1; 
      geo1.lon = lon1;
      geo2.lat = lat2;
      geo2.lon = lon2;

      simple_line->add_point(geo1);
      simple_line->add_point(geo2);
      simple_line->set_offset(x_offset, y_offset);

		// if the parameter list contains embedded text then
		// add the appropriate settings to the OvlLine object
		if (!embedded_text.IsEmpty())
		{
			// set the text
			simple_line->set_embedded_text(embedded_text);

			// set the font
			OvlFont &font = simple_line->get_font();
			font = m_font;
		}
   }
   
   // set the pen using the currently set pen
   OvlPen &pen = line->get_pen();
   pen = m_pen;

   // add this line to the list of objects and return the object handle
   return add_object(line);
}

long OvlElementContainer::add_line(COleSafeArray geo_points, int num_points)
{
   OvlLine *line = new OvlLine();

   // add each of the vertices of the geo_points array to the line object
   for (int i = 0; i < num_points; i++)
   {
      d_geo_t geo;
      long rgIndices[2];

      rgIndices[0] = i;

      rgIndices[1] = 0;
      geo_points.GetElement(rgIndices, &geo.lat);

      rgIndices[1] = 1;
      geo_points.GetElement(rgIndices, &geo.lon);

		// validate the geo-point
		if (!GEO_valid_degrees(geo.lat, geo.lon))
		{
			delete line;
			return FAILURE;
		}

      line->add_point(geo);
   }

   // set the pen of the line using the currently set pen
   OvlPen &pen = line->get_pen();
   pen = m_pen;

   return add_object(line);
}

// add a polygon to this layer using the current pen and current fill style
long OvlElementContainer::add_polygon(COleSafeArray geo_points, int num_points)
{
   OvlPolygon *polygon = new OvlPolygon();

   // add each of the vertices of the geo_points array to the polygon
   for (int i = 0; i < num_points; i++)
   {
      d_geo_t geo;
      long rgIndices[2];

      rgIndices[0] = i;

      rgIndices[1] = 0;
      geo_points.GetElement(rgIndices, &geo.lat);

      rgIndices[1] = 1;
      geo_points.GetElement(rgIndices, &geo.lon);

      polygon->add_point(geo);
   }

   // set the pen of the polygon using the currently set pen
   OvlPen &pen = polygon->get_pen();
   pen = m_pen;

   // set the fill type of the the polygon using the currently set fill type
   polygon->set_fill_type(m_fill_type);
   
   return add_object(polygon);
}

 // add an ellipse to this layer using the current pen and current fill style
long OvlElementContainer::add_ellipse(double lat, double lon, double vert_km, double horz_km, 
      double angle)
{
   OvlEllipse *ellipse = new OvlEllipse();

   // define the ellipse with the given parameters
   ellipse->define(lat, lon, vert_km, horz_km, angle);

   // set the pen of the ellipse using the currently set pen
   OvlPen &pen = ellipse->get_pen();
   pen = m_pen;

   // set the fill type of the the polygon using the currently set fill type
   ellipse->set_fill_type(m_fill_type);

   return add_object(ellipse);
}

// add text to this layer using the current font and anchor type
long OvlElementContainer::add_text(double lat, double lon, CString text, 
      int x_offset, int y_offset)
{
   d_geo_t geo;
   OvlText *text_object = new OvlText();

   // set the text
   text_object->set_text(text);

   // set the anchor position and offset of the text
   geo.lat = lat;
   geo.lon = lon;
   text_object->set_anchor(geo, m_anchor_type, CPoint(x_offset, y_offset));

  // set the font of the text using the currently set pen
   OvlFont &font = text_object->get_font();
   font = m_font;

	// set the rotation
	text_object->set_angle(m_text_rotation);

   return add_object(text_object);
}

// add a bitmap using the currently selected anchor type
long OvlElementContainer::add_bitmap(const CString& filename, 
   double lat, double lon)
{
   OvlBitmap *bitmap = new OvlBitmap();

   // load the bitmap with the given filename
   if (bitmap->load_bitmap(filename) != SUCCESS)
      return FAILURE;
   
   // set the position using the given lat,lon and current anchor type
   bitmap->set_position(lat, lon, m_anchor_type);

   return add_object(bitmap);
}

// add an icon to the layer with the given filename.  The icon's 
// text will use the current font
long OvlElementContainer::add_icon(CString filename, double lat, double lon, 
                         CString icon_text, long icon_set_handle/*-1*/,
                         long rotation /* =0*/, int x_offset /*=0*/, int y_offset /*=0*/)
{
   CIconSet *icon_set;
   d_geo_t geo;
   OvlIcon *icon = new OvlIcon();

   // if we are going to use an icon set make sure we have a 
   // valid handle
   if (icon_set_handle > 0)
   {
      m_icon_set_map.Lookup(icon_set_handle, icon_set);
      if (icon_set == NULL)
         return FAILURE;
   }

   // set the icon's handle
   icon->set_icon_set_handle(icon_set_handle);

   // load the icon with the given filename
   if (filename != "")
   {
      if (icon->load_icon(filename) != SUCCESS)
         return FAILURE;
   }
   else
   {
      // find the icon name, based on the rotation
      CString filename = icon_set->get_filename(rotation);
      
      // if the given rotation returns a valid filename
      if (filename == "")
         return FAILURE;
      
      if (icon->load_icon(filename) != SUCCESS)
         return FAILURE;
   }

   // set the position using the given lat,lon
   icon->set_location(lat, lon);
	icon->set_offset(x_offset, y_offset);

	// check to see if the size is encoded in the icon's text string
	int i = icon_text.Find('~');
	if (i != -1)
	{
		int size = atoi(icon_text.Mid(i+1));
		icon_text = icon_text.Left(i);
		icon->set_icon_size(size);
	}

   // set the parameters of the text object using the given
   // icon text, the current font, and anchor type = upper center
   {
      // get the text object from the icon object
      OvlText *icon_text_object = icon->get_text_object();

      // set the text
      icon_text_object->set_text(icon_text);
      
      // set the text's anchor position and offset
      geo.lat = lat;
      geo.lon = lon;
      icon_text_object->set_anchor(geo, UTIL_ANCHOR_UPPER_CENTER,
         CPoint(0, 18));   // 18 = (32/2 + 2) since using 32x32 icons 

      // set the font of the text using the currently set pen
      OvlFont &font = icon_text_object->get_font();
      font = m_font;
   }

   return add_object(icon);
}

// add the array of icons to the layer with the given filename.  The
// icon's text will use the current font
long OvlElementContainer::add_icon_multi(CString filename, COleSafeArray geo_points, 
                               const VARIANT &icon_text, int num_points)
{
   int object_handle;

   SAFEARRAY *safe_array = icon_text.parray;
   BSTR *bstrArray;
   SafeArrayAccessData(safe_array, (void **)&bstrArray);

	// first validate each of the points.  Returns FAILURE if ANY of the
	// points is invalid
	//
   int i;
	for(i=0; i< num_points; i++)
	{
		double lat, lon;
      long rgIndices_geo[2];

		// set the indices of the arrays
      rgIndices_geo[0] = i;
      
      // get the latitude from the geo array
      rgIndices_geo[1] = 0;
      geo_points.GetElement(rgIndices_geo, &lat);
      
      // get the longitude from the geo array
      rgIndices_geo[1] = 1;
      geo_points.GetElement(rgIndices_geo, &lon);

		if (!GEO_valid_degrees(lat, lon))
		{
			SafeArrayUnaccessData(safe_array);
			return FAILURE;
		}
	}

   // add each of the icons to the overlay
   for (i = 0; i < num_points; i++)
   {
      double lat, lon;
      long rgIndices_geo[2];
      
      // set the indices of the arrays
      rgIndices_geo[0] = i;
      
      // get the latitude from the geo array
      rgIndices_geo[1] = 0;
      geo_points.GetElement(rgIndices_geo, &lat);
      
      // get the longitude from the geo array
      rgIndices_geo[1] = 1;
      geo_points.GetElement(rgIndices_geo, &lon);
      
      // get the filename from the icon_text array
      CString text((LPTSTR)bstrArray[i]);
      object_handle = add_icon(filename, lat, lon, text);
        
      if (object_handle == -1)
         return FAILURE;
   }
   SafeArrayUnaccessData(safe_array);

   // return the first object handle.  Assure the client that the object
   // handle of the remaining icons will be consecutive
   return object_handle - num_points + 1;
}

// copy the data from the given symbol and create a new symbol with
// the given paramaters
long OvlElementContainer::add_symbol(OvlSymbol *symbol, double lat, double lon, double scale_factor,
      double rotation)
{
   OvlSymbol *new_symbol = new OvlSymbol();

   // copy data from the given symbol to the new symbol
   new_symbol->make_copy_from(symbol);

   // define the symbol with the given parameters
   d_geo_t anchor_pos = { lat, lon };
   new_symbol->set_anchor(anchor_pos);
   new_symbol->set_angle((float)rotation);
   new_symbol->set_scale((float)scale_factor);

   // set the pen of the ellipse using the currently set pen
   OvlPen &pen = new_symbol->get_pen();
   pen = m_pen;

   return add_object(new_symbol);
}

long OvlElementContainer::add_predefined_symbol(double lat, double lon, int symbolType, double scale_factor,
      double rotation)
{
   OvlSymbol *pSymbol = new OvlSymbol();

   switch(symbolType)
   {
   case CLIENT_SYMBOL_SQUARE:
      pSymbol->set_unit(1);
      pSymbol->add_line(1, 1, 1, -1);
      pSymbol->add_line(1, -1, -1, -1);
      pSymbol->add_line(-1, -1, -1, 1);
      pSymbol->add_line(-1, 1, 1, 1);
      break;
      
   case CLIENT_SYMBOL_DIAMOND:
      pSymbol->set_unit(10.0);
      pSymbol->add_line(0, 10, 10, 0);
      pSymbol->add_line(10, 0, 0, -10);
      pSymbol->add_line(0, -10, -10, 0);
      pSymbol->add_line(-10, 0, 0, 10);
      break;
      
   case CLIENT_SYMBOL_TRIANGLE:
      pSymbol->set_unit(-10.0);
      pSymbol->add_line(0, -10, 8, 6);
      pSymbol->add_line(8, 6, -8, 6);
      pSymbol->add_line(-8, 6, 0, -10);
      break;
      
   case CLIENT_SYMBOL_CIRCLE:
      pSymbol->set_unit(1.0);
      pSymbol->add_circle(0, 0, 1);
      break;
      
   case CLIENT_SYMBOL_SMALL_CIRCLE:
      pSymbol->set_unit(1.0);
      pSymbol->add_circle(0, 0, 0.5);
      break;
      
   case CLIENT_SYMBOL_CROSSHAIRS:
      pSymbol->set_unit(10);
      pSymbol->add_line(0, -2, 0, -10);
      pSymbol->add_line(0, 2, 0, 10);
      pSymbol->add_line(-2, 0, -10, 0);
      pSymbol->add_line(2, 0, 10, 0);
      break;
      
   case CLIENT_SYMBOL_TICK_MARK:
      pSymbol->set_unit(4);
      pSymbol->add_line(3, 0, -4, 0);
      break;
      
   default:
      ERR_report("Invalid symbol type specified for AddPredefinedSymbol");
      return FAILURE; //TO DO: change this to UNDEFINED SYMBOL or something
   }

   // define the symbol with the given parameters
   d_geo_t anchor_pos = { lat, lon };
   pSymbol->set_anchor(anchor_pos);
   pSymbol->set_angle((float)rotation);
   pSymbol->set_scale((float)scale_factor);

   // set the pen of the ellipse using the currently set pen
   OvlPen &pen = pSymbol->get_pen();
   pen = m_pen;

   return add_object(pSymbol);
}

// add milStd 2525 symbol to the layer
long OvlElementContainer::add_milstd2525_symbol(double lat, double lon, CString symbol_id)
{
	OvlMilStd2525Symbol *new_symbol = new OvlMilStd2525Symbol(symbol_id.AllocSysString());
	
	// set the symbols location
	new_symbol->set_location(lat, lon);

	return add_object(new_symbol);
}

// add a georect to the layer using the current pen
long OvlElementContainer::add_geo_rect(double nw_lat, double nw_lon, double se_lat, double se_lon)
{
   GeoBounds *geo_rect = new GeoBounds();
   
   // define the geo bounds with the given parameters
   if (geo_rect->set_bounds(se_lat, nw_lon, nw_lat, se_lon) != SUCCESS)
      return FAILURE;
   
   // set the pen of the geobounds using the currently set pen
   OvlPen &pen = geo_rect->get_pen();
   pen = m_pen;
   
   return add_object(geo_rect);
}

// add an arc to the layer using the current pen
long OvlElementContainer::add_arc(double lat, double lon, double radius, double heading_to_start,
                        double turn_arc, boolean_t clockwise)
{
   GeoArc *arc = new GeoArc();

   // define the arc with the given paramaters
   d_geo_t center;
   center.lat = lat;
   center.lon = lon;
   arc->define(center, radius, heading_to_start, turn_arc, clockwise);

   // set the pen of the geoarc using the currently set pen
   OvlPen &pen = arc->get_pen();
   pen = m_pen;

   return add_object(arc);
}

long OvlElementContainer::add_rgn(int type, COleSafeArray points, int count)
{
   OvlRgn *rgn = new OvlRgn;

	rgn->set_brush(m_pen, m_fill_type);
   rgn->add_operation(type, points, count, -1);

   return add_object(rgn);
}

long OvlElementContainer::combine_rgn(int region_handle, int type, 
                            COleSafeArray points, int count, int operation)
{
   OvlElement *object;

   // find the region with the given region handle
   // get a pointer to the object with the given object handle
   if (m_object_map.Lookup(region_handle, object) == 0)
   {
      ERR_report("CombineRegion failure [the given region handle was not found]");
      return FAILURE;
   }

   if (!object->is_kind_of("OvlRgn"))
   {
      ERR_report("CombineRegion failure [the given region handle does not correspond to a region object]");
      return FAILURE;
   }

   ((OvlRgn *)object)->add_operation(type, points, count, operation);

   // update the invalidation rect with the combined region's bounding rect
   MapProj *map = OVL_get_overlay_manager()->get_view_map();
	if (map && map->is_projection_set())
	{
		object->prepare_for_redraw((MapProj *)
			OVL_get_overlay_manager()->get_view_map());
	}
   m_invalidation_rect |= object->get_rect();

   return SUCCESS;
}

// move an object to a new location.  Trying to move lines, polylines, or
// polygons will return FAILURE

long OvlElementContainer::move_icon(long object_handle, double lat, double lon, 
                          int rotation)
{
   OvlElement *object;

   // get a pointer to the object with the given object handle
   if (m_object_map.Lookup(object_handle, object) == 0)
      return FAILURE;
   
   // verify that the object is an icon.  If not then return FAILURE
   if (!object->is_kind_of("OvlIcon"))
      return FAILURE;

   // if this icon has an icon set associated with it then set the
   // icon based upon the given rotation
   OvlIcon *icon = (OvlIcon *)object;
   int icon_set_handle = icon->get_icon_set_handle();
   if (icon_set_handle != -1)
   {
      // find the icon set associated with this icon
      CIconSet *icon_set = NULL;
      m_icon_set_map.Lookup(icon_set_handle, icon_set);
      if (icon_set != NULL)
      {
         // find the icon name, based on the rotation
         CString filename = icon_set->get_filename(rotation);
         
         // if the given rotation returns a valid filename
         if (filename != "")
            icon->load_icon(filename);
      }
   }

   // move the icon
   return move_object(object, lat, lon);
}

long OvlElementContainer::move_symbol(long object_handle, double lat, double lon, 
                            double rotation)
{
   OvlElement *object;

   // get a pointer to the object with the given object handle
   if (m_object_map.Lookup(object_handle, object) == 0)
      return FAILURE;
   
   // verify that the object is an symbol.  If not then return FAILURE
   if (!object->is_kind_of("OvlSymbol"))
      return FAILURE;

   // move the symbol

   // erase where the object was
   object->invalidate();

   // set the new position
   d_geo_t anchor = { lat, lon };
   ((OvlSymbol *)object)->set_anchor(anchor);
   ((OvlSymbol *)object)->set_rotation((float)rotation);

	// pretend to draw so we have the rect for invalidation
	object->prepare_for_redraw((MapProj *)
		OVL_get_overlay_manager()->get_view_map());

   // invalidate the area where the object is now so that is gets drawn
   object->invalidate();

   return SUCCESS;
}

long OvlElementContainer::move_bitmap(long object_handle, double lat, double lon)
{
   OvlElement *object;
   
   // get a pointer to the object with the given object handle
   if (m_object_map.Lookup(object_handle, object) == 0)
      return FAILURE;
   
   // verify that the object is an bitmap.  If not then return FAILURE
   if (!object->is_kind_of("OvlBitmap"))
      return FAILURE;
   
   // move the icon
   return move_object(object, lat, lon);
}

long OvlElementContainer::move_object(OvlElement *object, double lat, double lon)
{
   // erase where the object was
   object->invalidate();

   // set the new position
   object->set_location(lat, lon);

	// pretend to draw so we have the rect for invalidation
	object->prepare_for_redraw((MapProj *)
		OVL_get_overlay_manager()->get_view_map());

   // invalidate the area where the object is now so that is gets drawn
   object->invalidate();

   return SUCCESS;
}

// add an object to the group with the given group_name.  If the group
// does not yet exist then it will be created
long OvlElementContainer::add_object_to_group(long object_handle, long group_id)
{
   OvlElement *object;
   ObjectGroup *group_object;

   // get a pointer to the object with the given object handle
   if (m_object_map.Lookup(object_handle, object) == 0)
      return FAILURE;

   // check to see if the group already exists.  If it does not
   // then add a new group object list with the given group id
   // to the map
   if (m_group_map.Lookup(group_id, group_object) == 0)
   {  
      // create a new list
      group_object = new ObjectGroup;
      group_object->m_object_list = 
         new CList <OvlElement *, OvlElement *>;
      
      // put the key and the object list in the map
      m_group_map.SetAt(group_id, group_object); 
   }
   
   // make sure the object is not already in the list
   if (group_object->m_object_list->Find(object) == NULL)
   {
      // add the object to the list
      group_object->m_object_list->AddTail(object);
   }
   
   return SUCCESS;
}

// move the object in the object list to the top.  This function will move 
// the object to the end of the object list.  Since the last
// element in the list is drawn last it will be on the top of the layer.
long OvlElementContainer::move_object_to_top(long object_handle)
{
   OvlElement *object;

   // get a pointer to the object with the given object handle
   if (m_object_map.Lookup(object_handle, object) == 0)
      return FAILURE;

	EnterCriticalSection(&m_critical_section);

   POSITION position = m_object_list.Find(object);

   if (position)
   {
      m_object_list.RemoveAt(position);
      m_object_list.AddTail(object);
   }

	LeaveCriticalSection(&m_critical_section);

   return SUCCESS;
}

// move the object in the object list to the bottom.  This function
// will move the object to the beginning of the list.  Since the first
// element in the list is drawn first it will be on the bottom of the layer.
long OvlElementContainer::move_object_to_bottom(long object_handle)
{
   OvlElement *object;
   
   // get a pointer to the object with the given object handle
   if (m_object_map.Lookup(object_handle, object) == 0)
      return FAILURE;

	EnterCriticalSection(&m_critical_section);
   
   POSITION position = m_object_list.Find(object);
     
   if (position)
   {
      m_object_list.RemoveAt(position);
      m_object_list.AddHead(object);
   }

	LeaveCriticalSection(&m_critical_section);
   
   return SUCCESS;
}  

// set the threshold of the given group.  Any objects contained in the
// group will not be drawn if the map is at a scale above the given
// threshold
long OvlElementContainer::set_group_threshold(int group_handle, MapScale map_scale)
{
   ObjectGroup *group_object;

   // check to see that the group with the given group_handle 
   // already exists.  If it does then set the threshold
   if (m_group_map.Lookup(group_handle, group_object) != 0)
   {  
      group_object->m_threshold = map_scale;

      return SUCCESS;
   }

   return FAILURE;
}

// set the threshold of the given object.  This object will not be
// drawn if the map is at a scale above the given threshold
long OvlElementContainer::set_object_threshold(int object_handle, MapScale map_scale)
{
   OvlElement *object;
   
   // get a pointer to the object with the given object handle
   if (m_object_map.Lookup(object_handle, object) == 0)
      return FAILURE;
     
   object->m_threshold = map_scale;

   return SUCCESS;
}

// delete all the objects contained in a group
long OvlElementContainer::delete_group(int group_handle)
{
   ObjectGroup *group_object;

   // check to see that the group with the given group_handle 
   // already exists.  If it does then set the threshold
   if (m_group_map.Lookup(group_handle, group_object) != 0)
   {  
      POSITION position = group_object->m_object_list->GetHeadPosition();
      
      while (position)
         delete_object(get_object_id(group_object->m_object_list->
         GetNext(position)));
      
      return SUCCESS;
   }

   return FAILURE;
}  
  
int OvlElementContainer::draw(MapProj *map, CDC *dc)
{
	EnterCriticalSection(&m_critical_section);

   map_scale_utils::MapScale scale(map->scale().GetScale(), map->scale().GetScaleUnits());

	// loop through the list of objects
	POSITION position = m_object_list.GetHeadPosition();
	while (position)
	{
	   OvlElement *object = m_object_list.GetNext(position);

		if (object_above_threshold(scale, object))
		{
			object->set_draw_mode(NORMAL_MODE);
		   object->draw(map, dc);
		}
	}

	LeaveCriticalSection(&m_critical_section);
	return SUCCESS;
}

int OvlElementContainer::draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   // the above method will be deprecated in favor of this one

   EnterCriticalSection(&m_critical_section);
	// loop through the list of objects
	POSITION position = m_object_list.GetHeadPosition();
	while (position)
	{
	   OvlElement *object = m_object_list.GetNext(position);

		if (object_above_threshold(pMap->scale(), object))
		{
			object->set_draw_mode(NORMAL_MODE);
		   object->draw(pMap, gc);
		}
	}

	LeaveCriticalSection(&m_critical_section);

	return SUCCESS;
}

int OvlElementContainer::draw_to_base_map(MapProj *map)
{  
	EnterCriticalSection(&m_critical_section);
	// loop through the list of objects
	POSITION position = m_object_list.GetHeadPosition();
	while (position)
	{
      OvlElement *object = m_object_list.GetNext(position);

      map_scale_utils::MapScale scale(map->scale().GetScale(), map->scale().GetScaleUnits());

		if (object_above_threshold(scale, object))
		{
			object->set_draw_mode(NORMAL_MODE);
			object->draw_to_base_map(map);
		}
	}

	LeaveCriticalSection(&m_critical_section);
	return SUCCESS;
}

int OvlElementContainer::redraw(MapProj *map, CDC *dc)
{
	EnterCriticalSection(&m_critical_section);
   
	POSITION position = m_object_list.GetHeadPosition();
	while (position)
	{
	 OvlElement *object = m_object_list.GetNext(position);
	 
	 // if the object is a member of a group, check the group's threshold
	 ObjectGroup *object_group = object_is_member_of(object);
	 if (object_group == NULL || (object_group != NULL && 
		map->scale() >= object_group->m_threshold))
	    
		// check the object's threshold
		if (map->scale() >= object->m_threshold)
		   object->redraw(map, dc);
	}
	LeaveCriticalSection(&m_critical_section);
	return SUCCESS;
}

int OvlElementContainer::redraw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   // the above method will be deprecated in favor of this one

	EnterCriticalSection(&m_critical_section);

   MapScale scale(pMap->scale().GetScale(), pMap->scale().GetScaleUnits());
   
	POSITION position = m_object_list.GetHeadPosition();
	while (position)
   {
      OvlElement *object = m_object_list.GetNext(position);

      // if the object is a member of a group, check the group's threshold
      ObjectGroup *object_group = object_is_member_of(object);
      if (object_group == NULL || (object_group != NULL && 
         scale >= object_group->m_threshold))

         // check the object's threshold
         if (scale >= object->m_threshold)
            object->redraw(pMap, gc);
   }
	LeaveCriticalSection(&m_critical_section);
	return SUCCESS;
}

// invalidate the parts of the view covered this layer's current
// objects as well as a list of objects that have been deleted
// yet not invalidated
void OvlElementContainer::invalidate(boolean_t erase_background /*= FALSE*/)
{
	EnterCriticalSection(&m_critical_section);

   OVL_get_overlay_manager()->invalidate_rect(m_invalidation_rect);
   m_invalidation_rect.SetRectEmpty();

	LeaveCriticalSection(&m_critical_section);
}

// return a pointer to the object that is beneath the given point or NULL 
// if no object is underneath the given point
OvlElement *OvlElementContainer::hit_test(CPoint point)
{
   POSITION position = m_object_list.GetTailPosition();

	// we need access to the map
	CView *view = UTL_get_active_non_printing_view();
   ViewMapProj *map = NULL;
	if (view)
		map = UTL_get_current_view_map(view);

	if (NULL == map)
		return NULL;

   map_scale_utils::MapScale scale(map->scale().GetScale(), map->scale().GetScaleUnits());
   
   while (position)
   {
      OvlElement *element = m_object_list.GetPrev(position);

      if (element->in_view() &&
			element->hit_test(point) && object_above_threshold(scale, element))
         return element;
   }

   return NULL;
}  

// returns true if the object would be visible, false otherwise
int OvlElementContainer::create_icon_set()
{
   CIconSet *icon_set = new CIconSet;
   
   m_icon_set_map.SetAt(m_next_icon_set_handle, icon_set);
   
   return m_next_icon_set_handle++;
}

int OvlElementContainer::add_icon_to_set(int icon_set_id, int rotation, 
                               CString filename)
{
   CIconSet *icon_set;
   
   // see if we have an icon set with the given id
   m_icon_set_map.Lookup(icon_set_id, icon_set);

   if (icon_set == NULL)
      return FAILURE;

   icon_set->add(filename, rotation);

   return SUCCESS;
}

// get/set an particular objects data
int OvlElementContainer::set_object_data(int object_handle, int object_data)
{
	OvlElement *object;
	
	if (m_object_map.Lookup(object_handle, object) == 0)
		return FAILURE;

	object->set_data(object_data);
	return SUCCESS;
}

int OvlElementContainer::get_object_data(int object_handle, long *object_data)
{
	OvlElement *object;
	
	if (m_object_map.Lookup(object_handle, object) == 0)
		return FAILURE;

	*object_data = object->get_data();
	return SUCCESS;
}

boolean_t OvlElementContainer::object_above_threshold(const map_scale_utils::MapScale& scale, OvlElement *object)
{
	// if the object is a member of a group, check the group's threshold
	ObjectGroup *object_group = object_is_member_of(object);
	if (object_group == NULL || (object_group != NULL && 
		scale >= map_scale_utils::MapScale(object_group->m_threshold.GetScale(), object_group->m_threshold.GetScaleUnits())))
	{		
		// check the object's threshold
		if (scale >= map_scale_utils::MapScale(object->m_threshold.GetScale(), object->m_threshold.GetScaleUnits()))
		{
			return TRUE;
		}
	}

	return FALSE;
}

OvlElement *OvlElementContainer::GetPrevElement(POSITION& position)
{
	return m_object_list.GetPrev(position);
}
POSITION OvlElementContainer::GetTailPosition()
{
	return m_object_list.GetTailPosition();
}