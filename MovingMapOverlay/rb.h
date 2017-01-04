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



#ifndef _RB_H_
#define _RB_H_

class RangeBearing;
class C_gps_trail;
class MapProj;

#include "ovl_d.h"         // definition of C_icon
#include "geo_tool_d.h"    // for d_geo_t type definition
#include "rbview.h"
#include "snapto.h"
#include "FvMappingCore\Include\MapProjWrapper.h"

// RangeBearingData - this class stores the necessary information to draw
// and calculate range and bearings
class RangeBearingData
{
public:
   RangeBearingData()
   {
		m_from_latitude = 0.0;
		m_from_longitude = 0.0;
		m_to_latitude = 0.0;
		m_to_longitude = 0.0;
      m_range = 0.0;
      m_bearing = 0.0;
   }  

   // label of the range and bearing object
   CString m_label;
   
   // time the object was started and stopped
   COleDateTime m_time_started;
   COleDateTime m_time_stopped;
   
   // from/to locations
   double m_from_latitude, m_from_longitude;
	double m_to_latitude, m_to_longitude;

	// from/to descriptions
	CString m_from_description;
	CString m_to_description;
   
   // should the course be displayed as mag heading? (otherwise true heading)
   boolean_t m_mag;
   
   // should the range be displayed as nm? (otherwise km)
   boolean_t m_units_nm;

   // actual range and bearing
   double m_range, m_bearing;
};

// RangeBearingManager - the range and bearing manager stores a list of range
// and bearing objects.  The class communicates with the
// range and bearing page and insures that they are updated.  This class will 
// receive the time change notifications and tell the range and bearing 
// objects to update themselves
class RangeBearingManager
{
public:
   // constructor
   RangeBearingManager();
   
   // destructor
   ~RangeBearingManager();
   
   void set_parent_overlay(C_gps_trail *parent) { m_parent_gps_overlay = parent; }

   // given a RangeBearing object, return the zero-based index into the list.
   // Return -1 if not found.
   int get_rb_index(RangeBearing *rb_obj);

   // get a RangeBearing object given an index
   RangeBearing *get_rb_obj(int index);

private:
   CList <RangeBearing *, RangeBearing *> m_rb_list;
   
private:
   C_gps_trail *m_parent_gps_overlay;
   RangeBearingData m_old_dialog_data;
   boolean_t m_old_already_added;
   int m_old_index;
	int m_old_type; /* 0 = NONE, 1 = FROM, 2 = TO */

public:
	// return a count of the number of range/bearing objects
	int get_count() { return m_rb_list.GetCount(); }

   // add - will open a dialog box in which information
   // for a new range/bearing object is entered.  If the gps tools 
   // dialog is opened it will be updated
   void add();
   
   // add - will open a dialog box in which information
   // for a new range/bearing object is entered.  If the gps tools 
   // dialog is opened it will be updated. The dialog will be initialized
   // with values stored at closing as well as the given position
   void add(double lat, double lon, SnapToInfo snap_to, boolean_t snap_to_valid);
   
   // modify - will open the info dialog box for editing the
   // range/bearing info
   void modify(int index);
   void modify(int index, double lat, double lon, 
		SnapToInfo snap_to, boolean_t snap_to_valid);

   // change the location of range/bearing object with the given index
   void modify_location(int index, double lat, double lon, 
      CString description = "");

	// modify - will modify a range and bearing object with the given data
   void modify(int index, RangeBearingData &data);

   int add_to_list(RangeBearingData data);
   
   // set the current time, this will determine if the r/b objects need to
   // be drawn and where
   void set_current_view_time(COleDateTime time, d_geo_t end_location);
   
   // invalidate the area of the screen covered by the range/bearing objects
   void invalidate(boolean_t erase_background =FALSE);
   
   // stop - will stop a range/bearing object at the current time.  
   // Range/Bearing objects that are stopped will not be displayed on the screen
   void stop(int index);
   
   // delete_obj - will delete the range/bearing object at the given 
   // index
   void delete_obj(int index);
   
   // view - will open a separate window and display the range/bearing 
   // information of the object with the given index
   void view(int index);

   // Determine if the point hits any of the range/bearing objects in the list.
   // Return TRUE if this is the case.
   boolean_t hit_test(CPoint point);

   // determine if the point hits any of the range/bearing objects in the list,
   // if so then return a pointer to the range/bearing icon
   C_icon *hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point);
   
public:
   // draw the range/bearing objects to the screen
   int draw(MapProj *map, CDC *pDC, COleDateTime current_time);
   
   // initialize_dialog - will take the current range/bearing objects and 
   // use them to initialize the coast track dialog (if the dialog is 
   // opened)
   void initialize_dialog();

	// how many bytes will it take to write this coast track to a file
   int get_block_size();
   int Serialize(BYTE *&block);
   int deserialize(BYTE *block, C_overlay *overlay);

	// get the time span for the current coast tracks.  Returns TRUE if 
	// the start and end time are valid.
   boolean_t get_view_time_span(COleDateTime &begin, COleDateTime &end);
};

class RangeBearing : public C_icon
{
public:

   // constructor
   RangeBearing(C_overlay *overlay);
	~RangeBearing();

private:
   // handle to an optional window that can display the range and bearing
   RangeBearingView m_view;

   // range is stored in meters, bearing in degrees

   RangeBearingData m_rb_data;

   boolean_t m_position_set;
   CPoint m_position1, m_position2;

public:
   RangeBearingData get_data() { return m_rb_data; }
   void set_data(RangeBearingData data) { m_rb_data = data; }
	void reset_position_set() { m_position_set = FALSE; }

   void swap_direction();
	void remove_references();
	void add_references();

public:
   //
   // C_icon overrides

   // Returns TRUE if this icon is a instance of the given class.
   boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   const char *get_class_name() { return "RangeBearing"; }

   // Descriptive string functions
   CString get_help_text();
   CString get_tool_tip();

   // Determine if the point hits the coast track
   boolean_t hit_test(CPoint point);

   HintText get_hint();

public:
   // invalidate the part of the screen covered by the range bearing object
   void invalidate(boolean_t erase_background =FALSE);

   // update the range and bearing data of this range/bearing object.  The range
   // and bearing should be calculated based upon the given location
   void update(MapProj *map, d_geo_t new_location, boolean_t from_not_to);

   // view - will open a separate window and display the range/bearing 
   // information of the object with the given index
   void view();
   void update_view();

   // draw the range/bearing ojbect to the view
   int draw(MapProj *map, CDC *pDC);

   // does this range/bearing object exist at the given time
   boolean_t exists_at_time(COleDateTime time);

	void update_page();

	// how many bytes will it take to write this coast track data to a file
   int get_block_size();
   int Serialize(BYTE *&block);
   int deserialize(BYTE *&block);
};

class RangeBearingDragger
{
private:
   int m_radius;
   d_geo_t m_anchor;

public:
   void set_anchor(d_geo_t anchor, int radius) { m_anchor = anchor; m_radius = radius; }
   void draw(MapProj *map, CPoint point) { }
   void erase(MapProj *map) { }
   boolean_t get_moved() { return TRUE; }
};

#endif
