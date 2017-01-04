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



// OvlElementContainer.h
#include "common.h"
#include "maps_d.h"
#include "ovlelem.h"

#ifndef OVLELEMENTCONTAINER_H
#define OVLELEMENTCONTAINER_H

class ObjectGroup 
{
public:
   // constructor
   ObjectGroup() { m_object_list = NULL; m_threshold = WORLD; }

   // destructor
   ~ObjectGroup();

   // the threshold at which the elements in the group should be displayed
   MapScale m_threshold;

   // the list of object contained in this group
   CList <OvlElement *, OvlElement *> *m_object_list;
};

// icon_set_element definition
typedef struct _icon_set_element_t_
{
   _icon_set_element_t_(CString fn, int rt)
   {
      filename = fn;
      rotation = rt;
   }

   _icon_set_element_t_()
   {
      filename = "";
      rotation = 0;
   }

   CString filename;
   int rotation;
} icon_set_element_t;

// CIconSet - contains a list of icon set elements, which
// contain the filename of an icon and its rotation
class CIconSet
{
private:
   CList <icon_set_element_t, icon_set_element_t> m_icon_lst;

public:
   // Add an element to the list
   void add(CString filename, int rotation);

   // Will return a filename of an icon with the given rotation.
   // If the rotation is not found in the list then return "".
   CString get_filename(int rotation);
};

class OvlElementContainer
{
public:
	//constructor
	OvlElementContainer(void);

	// destructor
	virtual ~OvlElementContainer(void);

private:
	// the list of objects that make up this layer
   CList <OvlElement *, OvlElement *> m_object_list;

   CRect m_invalidation_rect;

   // a map of object handles to OvlElement pointers
   CMap <int, int, OvlElement*, OvlElement*> m_object_map;

   // a map of group names to a CList of objects
   CMap <long, long, ObjectGroup*, ObjectGroup*> m_group_map;

   // a map of icon set handles to icon set pointers
   CMap <long, long, CIconSet *, CIconSet *> m_icon_set_map;

   // handle to next object
   int m_next_handle;  

   // handle to next icon set
   int m_next_icon_set_handle;

   // determines whether the layer's objects can be redrawn using the
   // calculations of theh previous draw or if they need to be redrawn 
   // from scratch
   boolean_t m_allow_redraw;

   CRITICAL_SECTION m_critical_section;

   private:
   // the pen that will be used for newly added objects.  The client
   // can change this using a call to SetPen
   OvlPen m_pen;

   // the font that will be used for newly added objects.  The client
   // can change this using a call to SetFont
   OvlFont m_font;

	// angle used for the text objects added with AddText
	double m_text_rotation;

   // the type of line used (simple, rhumbline, or great circle).  The
   // client can change this using a call to SetLineType
   int m_line_type;

   // the type of fill used.  The client can change this using a call
   // to SetFillType
   int m_fill_type;

   // the type of anchor used when adding text to this layer.  This client
   // can change this using a call to SetAnchorType
   int m_anchor_type;

public:
   // set the current pen
   int set_pen(COLORREF fg_color, COLORREF bg_color, boolean_t turn_off_background, 
      int line_width, int line_style);

   // set the current font
   int set_font(const char *name, int size, int attributes,
      int fg_color, int bg_color, int bg_type);

	int set_font_RGB(const char *name, int size, int atrributes,
		COLORREF fg_color, COLORREF bg_color, int bg_type);

	// set the rotation angle for text objects
	int set_text_rotation(double angle);

   // set the line type
   int set_line_type(int line_type);

   // set the fill type
   int set_fill_type(int fill_type);

   // set the anchor type
   int set_anchor_type(int anchor_type);

private:
   // initialize the pen, font, line type, and fill type
   void initialize_defaults();
public:
   // returns the group that the given object is a member of.  Returns
   // NULL if the object is not a member of any group
   ObjectGroup *object_is_member_of(OvlElement *object);

   // return the object id of the given object.  Returns -1 if object
   // was not found in the object map.
   int get_object_id(OvlElement *object);

	// returns true if the object would be visible, false otherwise
	boolean_t object_above_threshold(const map_scale_utils::MapScale& scale, OvlElement *object);
public:

	int modify_object(int object_handle);
	long add_object(OvlElement *object);
	// delete the object with the given object handle
	long delete_object(long object_handle);
	// delete all objects in this layer
	long delete_all_objects();
	// add a line to this layer using the current pen and current line type
	long add_line(double lat1, double lon1, double lat2, double lon2,
				                      long x_offset, long y_offset, 
									  CString embedded_text = "");
	long add_line(COleSafeArray geo_points, int num_points);
	// add a polygon to this layer using the current pen and current fill style
	long add_polygon(COleSafeArray geo_points, int num_points);
	// add an ellipse to this layer using the current pen and current fill style
	long add_ellipse(double lat, double lon, double vert, double horz, 
										  double angle);
	// add text to this layer using the current font and anchor type
	long add_text(double lat, double lon, CString text, 
									   int x_offset, int y_offset);
	// add a bitmap using the currently selected anchor type
	long add_bitmap(const CString& filename, double lat, double lon);
	// add an icon to the layer with the given filename.  The icon's 
	// text will use the current font
	long add_icon(CString filename, double lat, double lon, 
				                       CString icon_text, long icon_set_handle = -1,
								       long rotation =0, int x_offset =0, int y_offset =0);
	// add the array of icons to the layer with the given filename.  The
	// icon's text will use the current font
	long add_icon_multi(CString filename, COleSafeArray geo_points, 
			                                 const VARIANT &icon_text, int num_points);

	// copy the data from the given symbol and create a new symbol with
	// the given paramaters
	long add_symbol(OvlSymbol *symbol, double lat, double lon, double scale_factor,
									     double rotation);
	long add_predefined_symbol(double lat, double lon, int symbolType, double scale_factor,
												    double rotation);
	// add milStd 2525 symbol to the layer
	long add_milstd2525_symbol(double lat, double lon, CString symbol_id);
	// add a georect to the layer using the current pen
	long add_geo_rect(double nw_lat, double nw_lon, double se_lat, double se_lon);
	// add an arc to the layer using the current pen
	long add_arc(double lat, double lon, double radius, double heading_to_start,
				                      double turn_arc, boolean_t clockwise);
	long add_rgn(int type, COleSafeArray points, int count);
	long combine_rgn(int region_handle, int type, 
				                          COleSafeArray points, int count, int operation);
	// move an object to a new location.  Trying to move lines, polylines, or
	// polygons will return FAILURE
	long move_icon(long object_handle, double lat, double lon, 
                          int rotation);
	long move_symbol(long object_handle, double lat, double lon, 
                            double rotation);
	long move_bitmap(long object_handle, double lat, double lon);
	long move_object(OvlElement *object, double lat, double lon);
	// add an object to the group with the given group_name.  If the group
	// does not yet exist then it will be created
	long add_object_to_group(long object_handle, long group_id);
	// move the object in the object list to the top.  This function will move 
	// the object to the end of the object list.  Since the last
	// element in the list is drawn last it will be on the top of the layer.
	long move_object_to_top(long object_handle);
	// move the object in the object list to the bottom.  This function
	// will move the object to the beginning of the list.  Since the first
	// element in the list is drawn first it will be on the bottom of the layer.
	long move_object_to_bottom(long object_handle);
	// set the threshold of the given group.  Any objects contained in the
	// group will not be drawn if the map is at a scale above the given
	// threshold
	long set_group_threshold(int group_handle, MapScale map_scale);



	// set the threshold of the given object.  This object will not be
	// drawn if the map is at a scale above the given threshold
	long set_object_threshold(int object_handle, MapScale map_scale);

	// delete all the objects contained in a group
	long delete_group(int group_handle);


	int draw(MapProj *map, CDC *dc);
   int draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

   int draw_to_base_map(MapProj *map);
	int redraw(MapProj *map, CDC *dc);
   int redraw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc);

	// invalidate the parts of the view covered this layer's current
	// objects as well as a list of objects that have been deleted
	// yet not invalidated (m_object_trash_can)
	void invalidate(boolean_t erase_background = FALSE);

	// return a pointer to the object that is beneath the given point or NULL 
	// if no object is underneath the given point
	OvlElement *hit_test(CPoint point);

	// returns true if the object would be visible, false otherwise
	int create_icon_set();
	int add_icon_to_set(int icon_set_id, int rotation, 
                               CString filename);

	// get/set an particular objects data
	int set_object_data(int object_handle, int object_data);
	int get_object_data(int object_handle, long *object_data);
	OvlElement* GetPrevElement(POSITION& position);
	POSITION GetTailPosition();
};
#endif