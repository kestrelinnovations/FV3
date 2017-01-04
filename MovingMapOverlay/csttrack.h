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



#ifndef _CSTTRACK_H
#define _CSTTRACK_H

// csttrack.h : header file for the coast track manager and coast track icons
//
#include "ovl_d.h"         // definition of C_icon
#include "geo_tool_d.h"    // for d_geo_t type definition
#include "ovlelem.h"       // GeoSegment::heading_type_t
#include "gps.h"
#include "FvMappingCore\Include\MapProjWrapper.h"

class CoastTrackData;
class CoastTrack;
class CoastTrackIntercept;
class CoastTrackPage;

#define KNOTS 0
#define KM_PER_H 1
#define SECONDS 0 
#define MINUTES 1

// CoastTrackManager - the coast track manager stores a list of coast
// tracks and coast track intercepts.  The class communicates with the
// coast track dialog and the intercept dialog(s) and insures that they
// are updated.  This class will receive the time change notifications and
// tell the coast tracks and coast track intercepts to update themselves.
class CoastTrackManager
{
public:
   // constructor
   CoastTrackManager();

   // destructor
   ~CoastTrackManager();

private:
   CList <CoastTrack *, CoastTrack *> m_coast_track_list;
   CList <CoastTrackIntercept *, CoastTrackIntercept *> m_intercept_list;
   CoastTrackData *m_old_dialog_data;
   boolean_t m_old_report_added;
   boolean_t m_old_already_added;
   int m_old_index;
   boolean_t m_timer_on;
	COleDateTime m_timer_start_time;
   C_gps_trail *m_parent_gps_overlay;
   int m_next_vector_color;
   COleDateTime m_current_view_time;

   // get the next color for the course vector given the current 
   // color
   int get_next_color(int current_color);

public:
   boolean_t is_timer_on() { return m_timer_on; }
	boolean_t timed_out(COleDateTime current_time);

   void set_parent_overlay(C_gps_trail *parent) { m_parent_gps_overlay = parent; }

   int get_ctrack_index(CoastTrack *ctrack);

public:
   // add_coast_track - will open a dialog box in which information
   // for a new coast track is entered.  If the coast track dialog
   // box is opened it will be updated
   void add_coast_track();

   // add_coast_track - will open a dialog box in which information
   // for a new coast track is entered.  The dialog will be initialized
   // with values stored at closing as well as the given position
   void add_coast_track(double lat, double lon);

   // modify_coast_track - will open the info dialog box for editing the
   // coast track's info
   void modify_coast_track(int index);
   void modify_coast_track(int index, double lat, double lon);

   // center_coast_track - will center the map on the given coast track
   void center_coast_track(int index);

   // delete_coast_track - will delete the coast track at the given 
   // index
   void delete_coast_track(int index);

   // change_label - will change the label of a coast track at the 
   // given index
   void change_label(int index, CString new_label);

   // stop_coast_track - will stop a coast track at the current time.  
   // Coast tracks that are stopped will not be displayed on the screen
   void stop_coast_track(int index);

   // create_intercept - will create a new intercept between two coast
   // tracks
   void create_intercept(int index1, int index2) { }

   // initialize_dialog - will take the current coast track data and 
   // use it to initialize the coast track dialog (if the dialog is 
   // opened)
   void initialize_dialog();

   // update_coast_tracks - calculates the new current position of each
   // of the coast tracks.  If the GPS tools dialog is opened the current
   // position will be updated
   void update_coast_tracks(MapProj *map, COleDateTime current_time);

   // get_count - will return the number of coast tracks in the coast
   // track list
   int get_count() { return m_coast_track_list.GetCount(); }

	CoastTrack *get_at(int index)
	{
		POSITION position = m_coast_track_list.FindIndex(index);
		if (position)
			return m_coast_track_list.GetAt(position);
		return NULL;
	}

   // add - will add a coast track data to the list of coast tracks
   int add(CoastTrackData &ctrack_data);

   // modify - will modify a coast track with the given data
   void modify(int index, CoastTrackData &data, boolean_t modified_report = TRUE);

   // toggle_selection - will flip the selection status of the given
   // coast track and update the GPS tools dialog if necessary
   void toggle_selection(CoastTrack *ctrack);

   // set_selected - will set the state of the coast track with the
   // given index
   void set_selected(int index, boolean_t is_selected);

   // set the current time, this will determine if the coast tracks need to
   // be drawn and if so, where they should be drawn
   void set_current_view_time(MapProj *map, COleDateTime time);

   COleDateTime get_current_view_time() { return m_current_view_time; }

   // get the time span for the current coast tracks.  Returns TRUE if the start
   // and end time are valid.
   boolean_t get_view_time_span(COleDateTime &begin, COleDateTime &end);

   // set the coast track update rate (in seconds)
   void set_update_rate(int rate);

public:
   // draw the coast tracks to the screen
   int draw(MapProj *map, CDC *pDC, COleDateTime current_time);

   // invalidate the area of the screen covered by the coast tracks
   void invalidate_coast_tracks(boolean_t erase_background = FALSE);

   // Determine if the point hits any of the coast tracks in the list
   boolean_t hit_test(CPoint point);

   // determine if the point hits any of the coast tracks in the list,
   // if so then return a pointer to the coast track icon
   C_icon *hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point);

   // how many bytes will it take to write the coast tracks to a file
   int get_block_size();
   int Serialize(BYTE *&block);
   int deserialize(BYTE *block, C_overlay *overlay);
};

class CourseVector
{
public:
   // is the course vector on
   boolean_t m_on;

   // is the course vector distance
   boolean_t m_is_dist;

   // time in seconds to project the location of a coast track
   double m_time; 

   // distance in meters to project the location of a coast track
   double m_distance;

   // units for distance or time
   units_t m_units_dist;
   units_t m_units_time;

   // how many bytes will it take to write this course vector data to a file
   int get_block_size();
   int Serialize(BYTE *&block);
   int deserialize(BYTE *&block);
};

typedef struct _ReportData_
{
	_ReportData_()
	{
		// get the current date time
		COleDateTime current_time;
		CTime time = CTime::GetCurrentTime();
		tm t1;
      time.GetGmtTm(&t1);
		m_time_started.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
			t1.tm_min, t1.tm_sec);
	}

   d_geo_t m_initial_position;
   double m_heading;
   double m_velocity; // ft/s
   COleDateTime m_time_started;
} ReportData;

// CoastTrackData - contains the defining parameters for a coast track
class CoastTrackData 
{
public:
   // constructor
   CoastTrackData();
   CoastTrackData(CString label, COleDateTime time_started, double heading,
      double velocity, d_geo_t initial_position);
   
   // copy constructor
   CoastTrackData &operator=(CoastTrackData &);

private:

   CString m_label;
   COleDateTime m_time_stopped;

   // the coast tracks current position
   d_geo_t m_current_position;

   // Report list
   CList <ReportData, ReportData> m_report_lst;

   short m_units_velocity;

   CourseVector m_course_vector;

   // projected heading type (either GeoSegment::GREAT_CIRCLE or 
   // GeoSegment::RHUMB_LINE, default is RHUMB_LINE)
   GeoSegment::heading_type_t m_projected_heading;

   // show trajectory from initial position on/off
   boolean_t m_initial_traj_on;

public:
   void set_label(CString label) { m_label = label; }
   CString &get_label() { return m_label; }

   void set_time_stopped(COleDateTime stopped) { m_time_stopped = stopped; }
   COleDateTime get_time_stopped() { return m_time_stopped; }

   void set_current_position(d_geo_t pos) { m_current_position = pos; }
   void set_current_position(double lat, double lon) { m_current_position.lat = lat; m_current_position.lon = lon; }
   d_geo_t get_current_position() { return m_current_position; }

   void set_units_velocity(short units) { m_units_velocity = units; }
   short get_units_velocity() { return m_units_velocity; }

   void set_course_vector(CourseVector vector) { m_course_vector = vector; }
   CourseVector &get_course_vector() { return m_course_vector; }
   
   void set_projected_heading(GeoSegment::heading_type_t type) { m_projected_heading = type; }
   GeoSegment::heading_type_t get_projected_heading() { return m_projected_heading; }

   void set_initial_traj_on(boolean_t on) { m_initial_traj_on = on; }
   boolean_t get_initial_traj_on() { return m_initial_traj_on; }

public:

   // how many bytes will it take to write this coast track data to a file
   int get_block_size();
   int Serialize(BYTE *&block);
   int deserialize(BYTE *&block);

   // add a report to the end of the report list and return the new report
   ReportData &add_report() { m_report_lst.AddTail(ReportData()); return m_report_lst.GetTail(); }
   ReportData &get_first_report() { return m_report_lst.GetHead(); }
   ReportData &get_last_report() { return m_report_lst.GetTail(); }
   ReportData &get_report(COleDateTime time);
};

// CoastTrack 
class CoastTrack : public C_icon
{
public:
   // constructors
   CoastTrack(C_overlay *overlay, int last_color = 0);

private:
   CoastTrackData m_track_data;
   CRect m_bounding_rect;
   GeoLine m_course_vector_line;
   GeoLine m_initial_vector_line;
   boolean_t m_selected;
   
   // current view coordinates of center of coast track
   CPoint m_position;

   // has the position (view coordinates) been set yet
   boolean_t m_position_set;

	CList <rb_ref_t, rb_ref_t> rb_ref_lst;
public:
	void add_range_bearing_reference(RangeBearing *rb_obj, 
		boolean_t from_not_to);

	void remove_range_bearing_reference(RangeBearing *rb_obj);

public:
   //
   // C_icon overrides

   // Returns TRUE if this icon is a instance of the given class.
   boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   const char *get_class_name() { return "CoastTrack"; }

   // Descriptive string functions
   CString get_help_text();
   CString get_tool_tip();

   // Determine if the point hits the coast track
   boolean_t hit_test(CPoint point);

   HintText get_hint();

public:
   // draw the coast track to the view
   void draw(MapProj *map, CDC *pDC);

   // invalidate the part of the screen covered by the coast track
   void invalidate(boolean_t erase_background = FALSE);

   // update the current position of this coast track
   void update(MapProj *map, COleDateTime time);

   // update the current position of this coast track with the given time
   void get_position(COleDateTime time, d_geo_t *current_position);

   // get the endpoint of the course vector at the given position
   void get_projected_geo(COleDateTime time, d_geo_t position, d_geo_t *projected_geo);

   // get/set the coast track data                                   
   void set_coast_track_data(CoastTrackData &data, boolean_t update_report = TRUE);
   CoastTrackData &get_coast_track_data() { return m_track_data; }

   // does the coast track exist at the given time
   boolean_t exists_at_time(COleDateTime dt);

public:
   // set the view coordinates of this coast track
   void set_position(CPoint pos) { m_position = pos; m_position_set = TRUE; }

   // get a pointer to the geoline that represents the course vector
   GeoLine *get_course_vector() { return &m_course_vector_line; }

   // get a pointer to the geoline that represents the initial vector
   GeoLine *get_initial_vector() { return &m_initial_vector_line; }

   // get/set selection status of this coast tradck
   void set_selected(boolean_t sel) { m_selected = sel; }
   boolean_t is_selected() { return m_selected; }

   // how many bytes will it take to write this coast track to a file
   int get_block_size();
   int Serialize(BYTE *&block);
   int deserialize(BYTE *&block);
};

// CoastTrackIntercept - an intercept between two coast tracks 
class CoastTrackIntercept
{
private:
   // pointers to the two coast tracks to intercept
   CoastTrack *m_track1;
   CoastTrack *m_track2;
};

#endif

