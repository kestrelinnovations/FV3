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



// rb.cpp : range and bearing manager and object implementation file
//
#include "stdafx.h"
#include "rbdlg.h"
#include "map.h"
#include "..\getobjpr.h"
#include "gpstools.h"   
#include "csttrack.h"
#include "param.h"
#include "..\PlaybackDialog\viewtime.h"
#include "ovl_mgr.h"

// constructor
RangeBearingManager::RangeBearingManager()
{
}

// destructor
RangeBearingManager::~RangeBearingManager()
{
   while (!m_rb_list.IsEmpty())
      delete m_rb_list.RemoveTail();
}

// given a RangeBearing object return the zero-based index into the list.
// Return -1 if not found.
int RangeBearingManager::get_rb_index(RangeBearing *rb_obj)
{
   POSITION position = m_rb_list.Find(rb_obj);
   
   // if the element was not found
   if (!position)
      return -1;
   
   int index = 0;
   POSITION loop_pos = m_rb_list.GetHeadPosition();
   while (loop_pos != position)
   {
      m_rb_list.GetNext(loop_pos);
      index++;
   }
   
   return index;
}

// get a RangeBearing object given an index
RangeBearing *RangeBearingManager::get_rb_obj(int index)
{
   POSITION position = m_rb_list.FindIndex(index);
   if (position)
      return m_rb_list.GetAt(position);
   
   return NULL;
}

// add - will add a range/bearing data to the list of r/b objects
void RangeBearingManager::add()
{
   // construct a dialog to retrieve the range/bearing data
   RangeBearingDataDialog dlg;
   
   dlg.set_rb_mgr(this);
   dlg.set_ctrack_mgr(m_parent_gps_overlay->get_coast_track_mgr());
   dlg.set_already_added(FALSE);
   
   // set default data for a new range/bearing objects
   {
      // create a new label
      CString label_str;
      label_str.Format("RB%d", m_rb_list.GetCount() + 1);
      
      // set the default data
      SnapToInfo invalid_snap;
      RangeBearingData data;
      data.m_label = label_str;
      data.m_from_latitude = 0.0;
      data.m_from_longitude = 0.0;
      data.m_to_latitude = 0.0;
      data.m_to_longitude = 0.0;
      data.m_mag = TRUE;
      data.m_units_nm = TRUE;
      data.m_from_description = "Fixed Location";
      data.m_to_description = "Fixed Location";

      dlg.SetData(data, invalid_snap, FALSE);
   }
   
   int result = dlg.DoModal();
   if (result == IDOK && !dlg.get_already_added())
   {
      add_to_list(dlg.GetData());
   }
   else if (result == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_ADD_RANGE_BEARING;
      m_old_dialog_data = dlg.GetData();
      m_old_already_added = dlg.get_already_added();
      m_old_index = dlg.get_index();
      m_old_type = dlg.get_type();
   }
}

// modify - will open the info dialog box for editing the
// range/bearing info
void RangeBearingManager::modify(int index)
{
   // construct a dialog to modify the range/bearing data
   RangeBearingDataDialog dlg;

   POSITION position = m_rb_list.FindIndex(index);
   if (position == NULL)
      return;

   RangeBearingData &data = m_rb_list.GetAt(position)->
      get_data();
   SnapToInfo invalid_snap;
   dlg.SetData(data, invalid_snap, FALSE);
   dlg.set_index(index);
   dlg.set_rb_mgr(this);
   dlg.set_ctrack_mgr(m_parent_gps_overlay->get_coast_track_mgr());
   dlg.set_already_added(TRUE);
   
   int result = dlg.DoModal();
   if (result == IDOK)
   {
      modify(index, dlg.GetData());
   }
   else if (result == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_MODIFY_RANGE_BEARING;
      m_old_dialog_data = dlg.GetData();
      m_old_already_added = dlg.get_already_added();
      m_old_index = dlg.get_index();
      m_old_type = dlg.get_type();
   }
}

void RangeBearingManager::modify(int index, double lat, double lon, 
   SnapToInfo snap_to, boolean_t snap_to_valid)
{
   // construct a dialog to retrieve the coast track data
   RangeBearingDataDialog dlg;
   
   dlg.set_index(m_old_index);
   dlg.set_already_added(m_old_already_added);
   dlg.set_is_modified(TRUE); // changed due to the new lat,lon
   dlg.set_rb_mgr(this);
   dlg.set_ctrack_mgr(m_parent_gps_overlay->get_coast_track_mgr());
   dlg.set_type(m_old_type); // 0 - none, 1 - FROM, 2 - TO
   
   // 'from' location gets the new lat/lon
   if (m_old_type == 1/*FROM*/)
   {
      m_old_dialog_data.m_from_latitude = lat;
      m_old_dialog_data.m_from_longitude = lon;
   }
   // otherwise, the 'to' location gets the new lat/lon
   else
   {
      m_old_dialog_data.m_to_latitude = lat;
      m_old_dialog_data.m_to_longitude = lon;
   }

   dlg.SetData(m_old_dialog_data, snap_to, snap_to_valid);
   
   int result = dlg.DoModal();
   if (result == IDOK)
   {
      modify(index, dlg.GetData());
   }
   else if (result == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_MODIFY_RANGE_BEARING;
      m_old_dialog_data = dlg.GetData();
      m_old_already_added = dlg.get_already_added();
      m_old_index = dlg.get_index();
      m_old_type = dlg.get_type();
   }
}

// change the location of range/bearing object with the given index
void RangeBearingManager::modify_location(int index, double lat, double lon,
                                          CString description /*=""*/)
{
   POSITION position = m_rb_list.FindIndex(index);
   
   // couldn't find the r/b object with the given index
   if (!position)
      return;
   
   RangeBearingData data = m_rb_list.GetAt(position)->get_data();
   
   if (m_old_type == 1 /*FROM*/)
   {
      data.m_from_latitude = lat;
      data.m_from_longitude = lon;
   }
   else if (m_old_type == 2 /*TO*/)
   {
      data.m_to_latitude = lat;
      data.m_to_longitude = lon;
   }

   if (description != "")
      data.m_label = description;

   m_rb_list.GetAt(position)->set_data(data);

   // update the view window if one exists
   m_rb_list.GetAt(position)->update_view();
   
   // update the range/bearing page if it is opened
   if (C_gps_trail::is_gps_tools_opened())
   {
      RangeBearingPage *rb_page = 
         (RangeBearingPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_RANGE_BEARING);
      
      // if this range/bearing object has not yet been created 
      // (based on the given time) then "grey" it out
      // current_time < data.m_time_started)
      
      // get the data from the current coast track and use it to
      // update the dialog's data
      rb_page->update(index, &data);
   }
}

// modify - will modify a range and bearing object with the given data
void RangeBearingManager::modify(int index, RangeBearingData &data)
{
   invalidate(); 

   POSITION position = m_rb_list.FindIndex(index);
   if (position)
   {
      data.m_time_started = m_rb_list.GetAt(position)->get_data().m_time_started;
      m_rb_list.GetAt(position)->remove_references();
      m_rb_list.GetAt(position)->set_data(data);
      m_rb_list.GetAt(position)->add_references();
   }

   m_rb_list.GetAt(position)->reset_position_set();

   // the gps overlay has changed and needs to be redrawn
   m_parent_gps_overlay->set_modified(TRUE);
   OVL_get_overlay_manager()->InvalidateOverlay(m_parent_gps_overlay);

   // update the r/b page, if it is open
   m_rb_list.GetAt(position)->update_page();
}

// add - will open a dialog box in which information
// for a new range/bearing object is entered.  If the gps tools 
// dialog is opened it will be updated. The dialog will be initialized
// with values stored at closing as well as the given position
void RangeBearingManager::add(double lat, double lon, SnapToInfo snap_to,
   boolean_t snap_to_valid)
{
   // construct a dialog to retrieve the coast track data
   RangeBearingDataDialog dlg;
   
   dlg.set_index(m_old_index);
   dlg.set_already_added(m_old_already_added);
   dlg.set_is_modified(TRUE); // changed due to the new lat,lon
   dlg.set_rb_mgr(this);
   dlg.set_ctrack_mgr(m_parent_gps_overlay->get_coast_track_mgr());
   dlg.set_type(m_old_type); // 0 - none, 1 - FROM, 2 - TO
   
   // 'from' location gets the new lat/lon
   if (m_old_type == 1/*FROM*/)
   {
      m_old_dialog_data.m_from_latitude = lat;
      m_old_dialog_data.m_from_longitude = lon;
   }
   // otherwise, the 'to' location gets the new lat/lon
   else
   {
      m_old_dialog_data.m_to_latitude = lat;
      m_old_dialog_data.m_to_longitude = lon;
   }

   dlg.SetData(m_old_dialog_data, snap_to, snap_to_valid);
   
   int result = dlg.DoModal();
   if (result == IDOK && !dlg.get_already_added())
   {
      add_to_list(dlg.GetData());
   }
   else if (result == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_ADD_RANGE_BEARING;
      m_old_dialog_data = dlg.GetData();
      m_old_already_added = dlg.get_already_added();
      m_old_index = dlg.get_index();
      m_old_type = dlg.get_type();
   }
}

// add_to_list - will add a range bearing object to the list
int RangeBearingManager::add_to_list(RangeBearingData data)
{
   // add the new coast track and set its data
   RangeBearing *rb_obj = new RangeBearing(m_parent_gps_overlay);
   m_rb_list.AddTail(rb_obj);
   rb_obj->set_data(data);

   // special cases for moving objects.  When the from/to description
   // is 'GPS Ship', one of the coast tracks, or 'Rehearsal Ship' we need
   // to setup a callback mechanism so that when one of these moving 
   // objects changes, the range and bearing object will get updated
   rb_obj->add_references();

   // update the view time dialog
   CMainFrame::GetPlaybackDialog().on_new_absolute_time(data.m_time_started);

   // Force the playback time to match the current time
   CMainFrame::GetPlaybackDialog().set_current_time(data.m_time_started);
   
   // add the r/b object to the dialog, if it is open
   if (C_gps_trail::is_gps_tools_opened())
   {
      RangeBearingPage *rb_page = 
         (RangeBearingPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_RANGE_BEARING);
      rb_page->add(&data);
   }

   // the gps overlay has changed
   m_parent_gps_overlay->set_modified(TRUE);
   OVL_get_overlay_manager()->InvalidateOverlay(m_parent_gps_overlay);
   
   // return the 0-based index of the newly added coast track
   return m_rb_list.GetCount() - 1;
}

// set the current time, this will determine if the r/b objects need to
// be drawn and if so, where theyshould be drawn
void RangeBearingManager::set_current_view_time (COleDateTime time,
                                                d_geo_t end_location)
{
   invalidate();
}

// invalidate the area of the screen covered by the coast tracks
void RangeBearingManager::invalidate(boolean_t erase_background /*=FALSE*/)
{
   POSITION position = m_rb_list.GetHeadPosition();
   while (position)
   {
      m_rb_list.GetNext(position)->invalidate(erase_background);
   }
}

// stop_coast_track - will stop a coast track at the current time.  
// Coast tracks that are stopped will not be displayed on the screen
void RangeBearingManager::stop(int index)
{
   POSITION position = m_rb_list.FindIndex(index);
   if (position)
   {
      RangeBearingData rb_data;

      // get  the current coast track data
      rb_data = m_rb_list.GetAt(position)->get_data();

      // modifify the stop time with the current system time if the stop time
      // doesn't already exist
      if (rb_data.m_time_stopped.GetStatus() == COleDateTime::null)
      {
         COleDateTime current_time;
         CTime time = CTime::GetCurrentTime();
         tm t1;
         time.GetGmtTm(&t1);
         current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
            t1.tm_min, t1.tm_sec);

         rb_data.m_time_stopped = current_time;
         m_rb_list.GetAt(position)->set_data(rb_data);

         // update the view time dialog
         CMainFrame::GetPlaybackDialog().on_new_absolute_time(rb_data.m_time_stopped);
         
         // if the GPS tools dialog is opened then we need to update the dialog
         if (C_gps_trail::is_gps_tools_opened())
         {
            rb_data = m_rb_list.GetAt(position)->get_data();

            RangeBearingPage *rb_page= 
               (RangeBearingPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_RANGE_BEARING);
            
            // get the data from the current coast track and use it to
            // update the dialog's data
            rb_page->update(index, &rb_data);
         }

         m_rb_list.GetAt(position)->invalidate();
      }
   }
   
   // if all coast tracks are stopped then we can stop the timer
   position = m_rb_list.GetHeadPosition();
   boolean_t all_stopped = TRUE;
   while (position)
   {
      RangeBearingData rb_data = m_rb_list.GetNext(position)->
         get_data();

      if (rb_data.m_time_stopped.GetStatus() == COleDateTime::null)
      {
         position = NULL;
         all_stopped = FALSE;
      }
   }

   // the gps overlay has changed
   m_parent_gps_overlay->set_modified(TRUE);
   OVL_get_overlay_manager()->InvalidateOverlay(m_parent_gps_overlay);
}

// delete_obj - will delete the range/bearing object at the given 
// index
void RangeBearingManager::delete_obj(int index)
{
   POSITION position = m_rb_list.FindIndex(index);
   if (position)
   {
      m_rb_list.GetAt(position)->invalidate();
      delete m_rb_list.GetAt(position);
      m_rb_list.RemoveAt(position);
   }
   
   // update the coast track dialog, if it is open
   if (C_gps_trail::is_gps_tools_opened())
   {
      RangeBearingPage *rb_page= 
         (RangeBearingPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_RANGE_BEARING);
      rb_page->delete_obj(index);
   }  
   
   // the gps overlay has changed
   m_parent_gps_overlay->set_modified(TRUE);
}

// view - will open a separate window and display the range/bearing 
// information of the object with the given index
void RangeBearingManager::view(int index)
{
   POSITION position = m_rb_list.FindIndex(index);

   if (position)
      m_rb_list.GetAt(position)->view();
}

// draw the range/bearing objects to the screen
int RangeBearingManager::draw(MapProj *map, CDC *pDC, COleDateTime current_time)
{
   // loop through each of the r/b objects and draw them to the view
   POSITION position = m_rb_list.GetHeadPosition();
   while (position)
   {
      RangeBearing *rb_obj = m_rb_list.GetNext(position);
      
      // if the current time is within the coast tracks valid range then
      // draw the coast track
      if (rb_obj->exists_at_time(current_time))
      {
         rb_obj->draw(map, pDC);
      }
   }
   
   return SUCCESS;
}  

// initialize_dialog - will take the current range/bearing objects and 
// use them to initialize the coast track dialog (if the dialog is 
// opened)
void RangeBearingManager::initialize_dialog()
{
   if (!C_gps_trail::is_gps_tools_opened())
      return;
   
   POSITION position = m_rb_list.GetHeadPosition();
   while (position)
   {
      RangeBearingPage *rb_page = 
         (RangeBearingPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_RANGE_BEARING);
      
      RangeBearingData rb_data= m_rb_list.GetNext(position)->get_data();
      rb_page->add(&rb_data);
   }
}

// Determine if the point hits any of the range/bearing objects in the list.
// Return TRUE if this is the case.
boolean_t RangeBearingManager::hit_test(CPoint point)
{
   POSITION position = m_rb_list.GetHeadPosition();
   while (position)
   {
      if (m_rb_list.GetNext(position)->hit_test(point))
         return TRUE;
   }

   return FALSE;
}

// determine if the point hits any of the range/bearing objects in the list,
// if so then return a pointer to the range/bearing icon
C_icon *RangeBearingManager::hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point)
{
   POSITION position = m_rb_list.GetHeadPosition();
   while (position)
   {
      RangeBearing *rb_obj = m_rb_list.GetNext(position);
      if (rb_obj->hit_test(point))
         return (C_icon *)rb_obj;
   }
   
   return NULL;
}

// get the time span for the current coast tracks.  Returns TRUE if 
// the start and end time are valid.
boolean_t RangeBearingManager::get_view_time_span(COleDateTime &begin, 
   COleDateTime &end)
{
   // if there are no coast tracks then there does not exist a begin/end time
   if (m_rb_list.GetCount() == 0)
      return FALSE;

   POSITION position = m_rb_list.GetHeadPosition();
   boolean_t initialized = FALSE;
   while (position)
   {
      RangeBearingData data = m_rb_list.GetNext(position)->get_data();
      
      if (!initialized)
      {
         begin = data.m_time_started;
         
         if (data.m_time_stopped.GetStatus() == COleDateTime::null)
         {
            COleDateTime current_time;
            CTime time = CTime::GetCurrentTime();
            tm t1;
         time.GetGmtTm(&t1);
         current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
            t1.tm_min, t1.tm_sec);

            end = current_time;
         }
         else
            end = data.m_time_stopped;
         
         initialized = TRUE;
      }
      
      // if the starting time of the current coast track is before 'begin'
      if (data.m_time_started < begin)
         begin = data.m_time_started;
      
      // if the stopping time of the current coast track is after 'end' and 
      // the ending time is valid
      if (data.m_time_stopped.GetStatus() != COleDateTime::null &&
         data.m_time_stopped > end)
         end = data.m_time_stopped;
   }

   return TRUE;
}

//
// RangeBearing implementation
//

// constructor
RangeBearing::RangeBearing(C_overlay *overlay) : C_icon(overlay)
{  
   m_position_set = FALSE;
}

RangeBearing::~RangeBearing()
{
   // remove any references to this object
   (dynamic_cast<C_gps_trail *>(m_overlay))->remove_range_bearing_reference(this);
   CoastTrackManager *ctrack_mgr = (dynamic_cast<C_gps_trail *>(m_overlay))->get_coast_track_mgr();
   for(int i=0;i<ctrack_mgr->get_count();i++)
   {
      CoastTrack *ctrack = ctrack_mgr->get_at(i);
      ctrack->remove_range_bearing_reference(this);
   }
}

// Returns TRUE if this icon is a instance of the given class.
boolean_t RangeBearing::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "RangeBearing") == 0)
      return TRUE;
   
   return C_icon::is_kind_of(class_name);
}

HintText RangeBearing::get_hint()
{
   HintText hint;

   hint.set_help_text(get_help_text());
   hint.set_tool_tip(get_tool_tip());

   return hint;
}

CString RangeBearing::get_help_text()
{
   return CString(m_rb_data.m_label);
}

CString RangeBearing::get_tool_tip()
{
   return CString(m_rb_data.m_label);
}

// Determine if the point hits the range/bearing object
boolean_t RangeBearing::hit_test(CPoint point)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // are we within 3 pixels from the line
   return futil->distance_to_line(m_position1.x, m_position1.y, 
      m_position2.x, m_position2.y, point.x, point.y) < 3;
}

// invalidate the part of the screen covered by the range bearing object
void RangeBearing::invalidate(boolean_t erase_background /*=FALSE*/)
{  
   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

// update the range and bearing data of this range/bearing object.  The range
// and bearing should be calculated based upon the given location
void RangeBearing::update(MapProj *map, d_geo_t new_location, boolean_t from_not_to)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   int x1, y1, x2, y2;

   // invalidate where the object was
   if (map != NULL)
   {
      int status1 = map->geo_to_surface(m_rb_data.m_from_latitude, m_rb_data.m_from_longitude, 
         &x1, &y1);
      int status2 = map->geo_to_surface(m_rb_data.m_to_latitude, m_rb_data.m_to_longitude, 
         &x2, &y2);
      if (status1 == SUCCESS && status2 == SUCCESS)
         OVL_get_overlay_manager()->invalidate_rect(
         futil->get_bounding_rect(x1, y1, x2, y2, UTIL_LINE_SOLID, 5));
   }

   if (from_not_to)
   {
      m_rb_data.m_from_latitude = new_location.lat;
      m_rb_data.m_from_longitude = new_location.lon;
   }
   else
   {
      m_rb_data.m_to_latitude = new_location.lat;
      m_rb_data.m_to_longitude = new_location.lon;
   }

   d_geo_t start;
   start.lat = m_rb_data.m_from_latitude;
   start.lon = m_rb_data.m_from_longitude;

   d_geo_t end;
   end.lat = m_rb_data.m_to_latitude;
   end.lon = m_rb_data.m_to_longitude;

   // invalidate where the object is now
   if (map != NULL)
   {
      map->geo_to_surface(m_rb_data.m_from_latitude, m_rb_data.m_from_longitude, 
         &x1, &y1);
      map->geo_to_surface(m_rb_data.m_to_latitude, m_rb_data.m_to_longitude, 
         &x2, &y2);
      OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
      m_position_set = TRUE;
      m_position1.x = x1;
      m_position1.y = y1;
      m_position2.x = x2;
      m_position2.y = y2;
   }
   
   GEO_calc_range_and_bearing(start, end, m_rb_data.m_range, 
      m_rb_data.m_bearing, TRUE);

   if (m_rb_data.m_mag)
   {
      // compute the local magnetic variation
      int year, month, alt, rslt;
      double magvar;
      SYSTEMTIME time;
      GetSystemTime(&time);
      year = time.wYear;
      month = time.wMonth;
      alt = 0;
      rslt = GEO_magnetic_variation(start.lat, start.lon, year, month, alt, &magvar);
      if (rslt != SUCCESS)
         magvar = 0.0;

      m_rb_data.m_bearing -= magvar;
      if (m_rb_data.m_bearing < 0.0)
         m_rb_data.m_bearing += 360.0;
      else if (m_rb_data.m_bearing > 360.0)
         m_rb_data.m_bearing -= 360.0;
   }
   
   // update the independent view window if necessary
   update_view();

   // if the GPS tools dialog is opened then we need to update the
   // current position in the dialog
   if (C_gps_trail::is_gps_tools_opened())
   {
      RangeBearingPage *rb_page= 
         (RangeBearingPage *)C_gps_trail::m_gpstools_dialog.GetPage(
         PAGE_INDEX_RANGE_BEARING);

      // get the data from the current coast track and use it to
      // update the dialog's data
      C_gps_trail *gps = dynamic_cast<C_gps_trail *>(m_overlay);
      rb_page->update(gps->get_rb_mgr()->get_rb_index(this), &m_rb_data);
   }
}

// view - will open a separate window and display the range/bearing 
// information of the object with the given index
void RangeBearing::view()
{
   // if the window does not exist
   if (!IsWindow(m_view.m_hWnd))
   {
      const int nPosX = PRM_get_registry_int("MovingMap", "PosX", 0);
      const int nPosY = PRM_get_registry_int("MovingMap", "PosY", 0);
      const int nWidth = PRM_get_registry_int("MovingMap", "Width", 340);
      const int nHeight = PRM_get_registry_int("MovingMap", "Height", 75);

      CRect view_position_and_size_rect(nPosX,nPosY, nPosX + nWidth, nPosY + nHeight);
      CString title_bar;

      title_bar.Format("From <%s> To <%s>", m_rb_data.m_from_description,
         m_rb_data.m_to_description);
      
      CString className = AfxRegisterWndClass(CS_BYTEALIGNWINDOW, 
         0, 0, 0);
      
      // create the window
      m_view.CreateEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED, className, title_bar, 
         WS_SYSMENU|WS_POPUP|WS_CAPTION|WS_VISIBLE|WS_THICKFRAME, 
         view_position_and_size_rect, AfxGetMainWnd(), 0);

      m_view.SetLayeredWindowAttributes(RANGE_BEARING_VIEW_COLOR_KEY, 0, LWA_COLORKEY);

      // the view needs to make callbacks to the range bearing object
      // it is associated with.  Set the view's range bearing object here
      m_view.set_range_bearing_obj(this);

      update_view();
   }
}  

void RangeBearing::remove_references()
{
   CoastTrackManager *ctrack_mgr = (dynamic_cast<C_gps_trail *>(m_overlay))->get_coast_track_mgr();

   (dynamic_cast<C_gps_trail *>(m_overlay))->remove_range_bearing_reference(this);
   for(int i=0;i<ctrack_mgr->get_count();i++)
   {
      CoastTrack *ctrack = ctrack_mgr->get_at(i);
      ctrack->remove_range_bearing_reference(this);
   }
}

void RangeBearing::add_references()
{
   CoastTrackManager *ctrack_mgr = (dynamic_cast<C_gps_trail *>(m_overlay))->get_coast_track_mgr();

   if (m_rb_data.m_from_description == "Self")
      (dynamic_cast<C_gps_trail *>(m_overlay))->add_range_bearing_reference(this, TRUE);
   else
      for(int i=0;i<ctrack_mgr->get_count();i++)
      {
         CoastTrack *ctrack = ctrack_mgr->get_at(i);
         if (m_rb_data.m_from_description == ctrack->get_coast_track_data().get_label())
         {
            ctrack->add_range_bearing_reference(this, TRUE);
            break;
         }
      }
      if (m_rb_data.m_to_description == "Self")
         (dynamic_cast<C_gps_trail *>(m_overlay))->add_range_bearing_reference(this, FALSE);
      else
         for(int i=0;i<ctrack_mgr->get_count();i++)
         {
            CoastTrack *ctrack = ctrack_mgr->get_at(i);
            if (m_rb_data.m_to_description == ctrack->get_coast_track_data().get_label())
            {
               ctrack->add_range_bearing_reference(this, FALSE);
               break;
            }
         }
}

void RangeBearing::swap_direction() 
{
   // remove any references to this object
   remove_references();

   // swap the description and lat/lons
   {
      double tmp_lat, tmp_lon;
      CString tmp_description;

      tmp_lat = m_rb_data.m_from_latitude;
      tmp_lon = m_rb_data.m_from_longitude;
      tmp_description = m_rb_data.m_from_description;

      m_rb_data.m_from_latitude = m_rb_data.m_to_latitude;
      m_rb_data.m_from_longitude = m_rb_data.m_to_longitude;
      m_rb_data.m_from_description = m_rb_data.m_to_description;

      m_rb_data.m_to_latitude = tmp_lat;
      m_rb_data.m_to_longitude = tmp_lon;
      m_rb_data.m_to_description = tmp_description;
   }

   reset_position_set();

   // the gps overlay has changed and needs to be redrawn
   m_overlay->set_modified(TRUE);
   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);

   // update the r/b page, if it is open
   update_page();

   // special cases for moving objects.  When the from/to description
   // is 'GPS Ship', one of the coast tracks, or 'Rehearsal Ship' we need
   // to setup a callback mechanism so that when one of these moving 
   // objects changes, the range and bearing object will get updated
   add_references();
}

void RangeBearing::update_view()
{
   if (IsWindow(m_view.m_hWnd))
   {
      double range;
      if (m_rb_data.m_units_nm)
         range = METERS_TO_NM(m_rb_data.m_range);
      else
         range = m_rb_data.m_range / 1000;
      
      m_view.set_range_bearing(range, m_rb_data.m_bearing, m_rb_data.m_mag,
         m_rb_data.m_units_nm);

      CString title_bar;
      title_bar.Format("From <%s> To <%s>", m_rb_data.m_from_description,
         m_rb_data.m_to_description);
      m_view.SetWindowText(title_bar);
   }
}

// draw the range/bearing object to the screen
int RangeBearing::draw(MapProj *map, CDC *pDC)
{
   UtilDraw util(pDC);
   int x1, y1;
   int x2, y2;
   
   // get the screen coordinates of the coast track's current
   // position and set this for the r/b object
   map->geo_to_surface(m_rb_data.m_from_latitude, m_rb_data.m_from_longitude, 
      &x1, &y1);
   map->geo_to_surface(m_rb_data.m_to_latitude, m_rb_data.m_to_longitude, 
      &x2, &y2);

   if (!m_position_set)
   {
      d_geo_t start;
      start.lat = m_rb_data.m_from_latitude;
      start.lon = m_rb_data.m_from_longitude;

      d_geo_t end;
      end.lat = m_rb_data.m_to_latitude;
      end.lon = m_rb_data.m_to_longitude;

      GEO_calc_range_and_bearing(start, end, m_rb_data.m_range, 
         m_rb_data.m_bearing, TRUE);

      if (m_rb_data.m_mag)
      {
         // compute the local magnetic variation
         int year, month, alt, rslt;
         double magvar;
         SYSTEMTIME time;
         GetSystemTime(&time);
         year = time.wYear;
         month = time.wMonth;
         alt = 0;
         rslt = GEO_magnetic_variation(start.lat, start.lon, year, month, alt, &magvar);
         if (rslt != SUCCESS)
            magvar = 0.0;
         m_rb_data.m_bearing -= magvar;
         if (m_rb_data.m_bearing < 0.0)
            m_rb_data.m_bearing += 360.0;
         else if (m_rb_data.m_bearing > 360.0)
            m_rb_data.m_bearing -= 360.0;
      }

      m_position_set = TRUE;
      m_position1.x = x1;
      m_position1.y = y1;
      m_position2.x = x2;
      m_position2.y = y2;

      // update the independent view window if necessary
      update_view();

      // if the GPS tools dialog is opened then we need to update the
      // current position in the dialog
      if (C_gps_trail::is_gps_tools_opened())
      {
         RangeBearingPage *rb_page= 
            (RangeBearingPage *)C_gps_trail::m_gpstools_dialog.GetPage(
            PAGE_INDEX_RANGE_BEARING);

         // get the data from the current coast track and use it to
         // update the dialog's data
         C_gps_trail *gps = dynamic_cast<C_gps_trail *>(m_overlay);
         rb_page->update(gps->get_rb_mgr()->get_rb_index(this), &m_rb_data);
      }
   }

   // define the line thickness
   const int line_width = 3;
   
   util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_DASH2, line_width + 2, TRUE);
   util.draw_line(x1, y1, x2, y2);

   util.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_DASH2, line_width);
   util.draw_line(x1, y1, x2, y2);

   return SUCCESS;
}

// does this range/bearing object exist at the given time
boolean_t RangeBearing::exists_at_time(COleDateTime dt)
{
   // assume the coast track exists if we are given an invalid date time
   if (dt.GetStatus() == COleDateTime::null)
      return FALSE;
   
   // TRUE if dt is later then the time started and before the time ended, if 
   // ended
   return (m_rb_data.m_time_started <= dt &&
      (m_rb_data.m_time_stopped.GetStatus() == COleDateTime::null ||
      dt <= m_rb_data.m_time_stopped));
}

void RangeBearing::update_page()
{
   if (C_gps_trail::is_gps_tools_opened())
   {
      RangeBearingPage *rb_page = 
         (RangeBearingPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_RANGE_BEARING);;
      C_gps_trail *gps = dynamic_cast<C_gps_trail *>(m_overlay);
      rb_page->update(gps->get_rb_mgr()->get_rb_index(this), &m_rb_data);
   }
}

// *** Serialization
//

// how many bytes will it take to write the coast tracks to a file
int RangeBearingManager::get_block_size()
{
   POSITION position = m_rb_list.GetHeadPosition();
   int size = 0;

   // write out the number of range and bearing objects
   size += sizeof(int);

   // write out data for the coast tracks
   while (position)
      size += m_rb_list.GetNext(position)->get_block_size();

   return size;
}

int RangeBearingManager::Serialize(BYTE *&block)
{
   // number of range/bearing objects
   int num_rb_obj = m_rb_list.GetCount();
   memcpy(block, &num_rb_obj, sizeof(int));
   block += sizeof(int);

   // write out data for each of the range and bearing objects
   POSITION position = m_rb_list.GetHeadPosition();
   while (position)
      m_rb_list.GetNext(position)->Serialize(block);

   return SUCCESS;
}

int RangeBearingManager::deserialize(BYTE *block, C_overlay *overlay)
{
   // clear the current range/bearing objects from memory
   while (!m_rb_list.IsEmpty())
      delete m_rb_list.RemoveTail();

   // get the number of range/bearing objects
   int num_rb_obj;
   memcpy(&num_rb_obj, block, sizeof(int));
   block += sizeof(int);

   for(int i=0; i<num_rb_obj;i++)
   {
      m_rb_list.AddTail(new RangeBearing(overlay));
      m_rb_list.GetTail()->deserialize(block);
   }

   return SUCCESS;
}

// how many bytes will it take to write this coast track data to a file
int RangeBearing::get_block_size()
{
   int size = 0;

   // size of the label plus the bytes for the label
   size += sizeof(int) + m_rb_data.m_label.GetLength();
   
   // time the object was started and stopped
   size += sizeof(double);
   size += sizeof(double);
   
   // from/to locations
   size += sizeof(double)*4;

   // from/to descriptions
   size += sizeof(int) + m_rb_data.m_from_description.GetLength();
   size += sizeof(int) + m_rb_data.m_to_description.GetLength();
   
   // should the course be displayed as mag heading? (otherwise true heading)
   size += sizeof(boolean_t);
   
   // should the range be displayed as nm? (otherwise km)
   size += sizeof(boolean_t);

   return size;
}

int RangeBearing::Serialize(BYTE *&block)
{
   // write out the size of the label
   int label_length = m_rb_data.m_label.GetLength();
   memcpy(block, &label_length, sizeof(int));
   block += sizeof(int);

   // write out the string
   if (label_length != 0)
   {
      memcpy(block, m_rb_data.m_label.GetBuffer(label_length), label_length);
      block += label_length;
   }

   // write out time started
   {
      double time_started = (double)m_rb_data.m_time_started.m_dt;

      memcpy(block, &time_started, sizeof(double));
      block += sizeof(double);
   }

   // write out time stopped
   {
      double time_stopped;

      // if the range/bearing is not stopped we should time stamp
      // it with the current time
      if (m_rb_data.m_time_stopped.GetStatus() == COleDateTime::null)
      {
         COleDateTime current_time;
         CTime time = CTime::GetCurrentTime();
         tm t1;
         time.GetGmtTm(&t1);
         current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
            t1.tm_min, t1.tm_sec);

         time_stopped = (double)current_time.m_dt;
      }
      else
         time_stopped = (double)m_rb_data.m_time_stopped.m_dt;

      memcpy(block, &time_stopped, sizeof(double));
      block += sizeof(double);
   }

   // write out from latitude
   memcpy(block, &(m_rb_data.m_from_latitude), sizeof(double));
   block += sizeof(double);

   // write out from longitude
   memcpy(block, &(m_rb_data.m_from_longitude), sizeof(double));
   block += sizeof(double);

   // write out to latitude
   memcpy(block, &(m_rb_data.m_to_latitude), sizeof(double));
   block += sizeof(double);

   // write out to longitude
   memcpy(block, &(m_rb_data.m_to_longitude), sizeof(double));
   block += sizeof(double);

   // write out the size of the from description
   label_length = m_rb_data.m_from_description.GetLength();
   memcpy(block, &label_length, sizeof(int));
   block += sizeof(int);

   // write out the string
   if (label_length != 0)
   {
      memcpy(block,m_rb_data.m_from_description.GetBuffer(label_length), label_length);
      block += label_length;
   }

   // write out the size of the to description
   label_length = m_rb_data.m_to_description.GetLength();
   memcpy(block, &label_length, sizeof(int));
   block += sizeof(int);

   // write out the string
   if (label_length != 0)
   {
      memcpy(block,m_rb_data.m_to_description.GetBuffer(label_length), label_length);
      block += label_length;
   }

   // write out should the course be displayed as mag heading
   memcpy(block, &(m_rb_data.m_mag), sizeof(boolean_t));
   block += sizeof(boolean_t);

   // write out should the range be displayed as nm
   memcpy(block, &(m_rb_data.m_units_nm), sizeof(boolean_t));
   block += sizeof(boolean_t);

   return SUCCESS;
}

int RangeBearing::deserialize(BYTE *&block)
{
   // get the size of the label
   int label_length;
   memcpy(&label_length, block, sizeof(int));
   block += sizeof(int);
   
   // get out the string
   if (label_length != 0)
   {
      char tmp_buffer[256];

      memcpy(tmp_buffer, block, label_length);
      tmp_buffer[label_length] = '\0';
      block += label_length;
      m_rb_data.m_label = CString(tmp_buffer);
   }

   // read time started
   double time_started; 
   memcpy(&time_started, block, sizeof(double));
   m_rb_data.m_time_started = COleDateTime((DATE)time_started);
   block += sizeof(double);

   // read time stopped
   double time_stopped; 
   memcpy(&time_stopped, block, sizeof(double));
   m_rb_data.m_time_stopped = COleDateTime((DATE)time_stopped);
   block += sizeof(double);

   // read in from latitude
   memcpy(&(m_rb_data.m_from_latitude), block, sizeof(double));
   block += sizeof(double);

   // read in from longitude
   memcpy(&(m_rb_data.m_from_longitude), block, sizeof(double));
   block += sizeof(double);

   // read in to latitude
   memcpy(&(m_rb_data.m_to_latitude), block, sizeof(double));
   block += sizeof(double);

   // read in to longitude
   memcpy(&(m_rb_data.m_to_longitude), block, sizeof(double));
   block += sizeof(double);

   // get the size of the from description string
   memcpy(&label_length, block, sizeof(int));
   block += sizeof(int);
   
   // get out the string
   if (label_length != 0)
   {
      char tmp_buffer[256];

      memcpy(tmp_buffer, block, label_length);
      tmp_buffer[label_length] = '\0';
      block += label_length;
      m_rb_data.m_from_description = CString(tmp_buffer);
   }

   // get the size of the to description string
   memcpy(&label_length, block, sizeof(int));
   block += sizeof(int);
   
   // get out the string
   if (label_length != 0)
   {
      char tmp_buffer[256];

      memcpy(tmp_buffer, block, label_length);
      tmp_buffer[label_length] = '\0';
      block += label_length;
      m_rb_data.m_to_description = CString(tmp_buffer);
   }

   // read in should the course be displayed as mag heading
   memcpy(&(m_rb_data.m_mag), block, sizeof(boolean_t));
   block += sizeof(boolean_t);

   // read in should the range be displayed as nm
   memcpy(&(m_rb_data.m_units_nm), block, sizeof(boolean_t));
   block += sizeof(boolean_t);

   add_references();

   return SUCCESS;
}
