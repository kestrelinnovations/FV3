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



// csttrack.cpp : implementation file
//
#include "stdafx.h"
#include "csttrack.h"
#include "ctrakdlg.h"
#include "map.h"
#include "..\getobjpr.h"
#include "math.h"
#include "gpstools.h"
#include "rb.h"
#include "..\PlaybackDialog\viewtime.h"
#include "..\MapEngineCOM.h"
#include "ovl_mgr.h"

//
// CoastTrackManager implementation - the coast track manager stores a 
// list of coast tracks and coast track intercepts.  The class communicates 
// with the coast track dialog and the intercept dialog(s) and insures that they
// are updated.  This class will receive the time change notifications and
// tell the coast tracks and coast track intercepts to update themselves.
//

// constructor
CoastTrackManager::CoastTrackManager()
{
   m_timer_on = FALSE;
   m_old_dialog_data = new CoastTrackData;
   m_next_vector_color = UTIL_COLOR_GREEN;
}

// destructor
CoastTrackManager::~CoastTrackManager()
{
   while (!m_coast_track_list.IsEmpty())
      delete m_coast_track_list.RemoveTail();

   delete m_old_dialog_data;
}

int CoastTrackManager::get_ctrack_index(CoastTrack *ctrack)
{
   POSITION position = m_coast_track_list.Find(ctrack);

   if (!position)
      return -1;

   int index = 0;
   POSITION loop_pos = m_coast_track_list.GetHeadPosition();
   while (loop_pos != position)
   {
      m_coast_track_list.GetNext(loop_pos);
      index++;
   }

   return index;
}

// add_coast_track - will open a dialog box in which information
// for a new coast track is entered.  If the coast track dialog
// box is opened it will be updated
void CoastTrackManager::add_coast_track()
{
   // construct a dialog to retrieve the coast track data
   CoastTrackDataDialog dlg;

   dlg.set_coast_track_mgr(this);
   dlg.set_already_added(FALSE);

   // set default data for a new coast track
   {
      CoastTrackData data;
      
      ReportData &report_data = data.add_report();
      dlg.set_report_added(TRUE);
      
      // set the default position
      report_data.m_initial_position.lat = 0.0;
      report_data.m_initial_position.lon = 0.0;
      data.set_current_position(0.0, 0.0);
      
      // set the default projected heading
      data.set_projected_heading(GeoSegment::RHUMB_LINE);
      
      // set the default label
      CString label_str;
      label_str.Format("TRACK%d",m_coast_track_list.GetCount() + 1);
      data.set_label(label_str);
 
      // is the course vector on
      CourseVector &course_vector = data.get_course_vector();
      course_vector.m_on = TRUE;
      course_vector.m_is_dist= TRUE;
      course_vector.m_time = 60;
      course_vector.m_distance = NM_TO_METERS(100);
      course_vector.m_units_time = (units_t)SECONDS;
      course_vector.m_units_dist = NM;

      // set heading / velocity
      report_data.m_heading = 90.0;
      report_data.m_velocity = KNOTS_TO_FEET_PER_S(300);
      data.set_units_velocity(KNOTS);

      // set initial trajectory on/off
      data.set_initial_traj_on(FALSE);
      
      dlg.SetData(data);
   }
   
   int result = dlg.DoModal();
   if (result == IDOK && !dlg.get_already_added())
   {
      add(dlg.GetData());
   }
   else if (result == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_ADD_COAST_TRACK;
      *m_old_dialog_data = dlg.GetData();
      m_old_already_added = dlg.get_already_added();
      m_old_index = dlg.get_index();
   }
}

// add_coast_track - will open a dialog box in which information
// for a new coast track is entered.  The dialog will be initialized
// with values stored at closing as well as the given position
// We are assuming that the only place that calls this function is from mouse.cpp
// Which means we are assuming a new report has already been added to the list 
// and thatno new report need be added
void CoastTrackManager::add_coast_track(double lat, double lon)
{
   // construct a dialog to retrieve the coast track data
   CoastTrackDataDialog dlg;

   dlg.set_index(m_old_index);
   dlg.set_already_added(m_old_already_added);
   dlg.set_is_modified(TRUE); // changed due to the new lat,lon
   dlg.set_coast_track_mgr(this);
   dlg.set_modified_report(TRUE);
   dlg.set_report_added(TRUE);
   
   m_old_dialog_data->get_last_report().m_initial_position.lat = lat;
   m_old_dialog_data->get_last_report().m_initial_position.lon = lon;
   m_old_dialog_data->set_current_position(lat, lon);
     
   dlg.SetData(*m_old_dialog_data);
   
   int result = dlg.DoModal();
   if (result == IDOK && !dlg.get_already_added())
   {
      add(dlg.GetData());
   }
   else if (result == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_ADD_COAST_TRACK;
      *m_old_dialog_data = dlg.GetData();
      m_old_already_added = dlg.get_already_added();
      m_old_index = dlg.get_index();
   }
}

// modify_coast_track - will open the info dialog box for editing the
// coast track's info
void CoastTrackManager::modify_coast_track(int index)
{
   // construct a dialog to retrieve the coast track data
   CoastTrackDataDialog dlg;

   dlg.set_already_added(TRUE);
   dlg.set_index(index);
   dlg.set_coast_track_mgr(this);

   // we only want to add a new report to the coast track data list if 
   // one of { initial_pos, heading, velocity} changes.  To know whether
   // or not we need to add a new report we will store a flag in the dialog.
   dlg.set_report_added(FALSE);

   POSITION position = m_coast_track_list.FindIndex(index);
   if (position == NULL)
      return;

   CoastTrackData &data = m_coast_track_list.GetAt(position)->
      get_coast_track_data();

   dlg.SetData(data);
   
   int result = dlg.DoModal();
   if (result == IDOK)
   {
      CoastTrackData &ctrack_data = dlg.GetData();
      boolean_t update_report = dlg.update_report();
      m_coast_track_list.GetAt(position)->invalidate();
      m_coast_track_list.GetAt(position)->set_coast_track_data(ctrack_data, update_report);
      
      // update the coast track to the dialog, if it is open
      if (C_gps_trail::is_gps_tools_opened())
      {
         CoastTrackPage *ctrack_page = 
            (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);
         ctrack_page->update_coast_track(index, &ctrack_data);
      }
      
      // the gps overlay has changed and needs to be redrawn
      m_parent_gps_overlay->set_modified(TRUE);
   }
   else if (result == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_MODIFY_COAST_TRACK;
      MapView::m_get_position_from_map_index = index;
      *m_old_dialog_data = dlg.GetData();
      m_old_report_added = dlg.get_report_added(); 
   }
}

// modify_coast_track - will open the info dialog box for editing the
// coast track's info
void CoastTrackManager::modify_coast_track(int index, double lat, double lon)
{
   POSITION position = m_coast_track_list.FindIndex(index);
   if (position == NULL)
      return;

   // construct a dialog to retrieve the coast track data
   CoastTrackDataDialog dlg;

   dlg.set_already_added(TRUE);
   dlg.set_index(index);
   dlg.set_coast_track_mgr(this);
   dlg.set_is_modified(TRUE); // changed due to the new lat,lon
   dlg.set_modified_report(TRUE);

   // append a new report if we haven't already done so for this modification
   // since the initial_position will be changing
   if (!m_old_report_added)
   {
      // copy the heading and velocity from the last report
      ReportData &old_report = m_old_dialog_data->get_last_report();
      ReportData &new_report = m_old_dialog_data->add_report();
      new_report.m_heading = old_report.m_heading;
      new_report.m_velocity = old_report.m_velocity;
   }

   m_old_dialog_data->get_last_report().m_initial_position.lat = lat;
   m_old_dialog_data->get_last_report().m_initial_position.lon = lon;
   
   dlg.set_report_added(TRUE);
     
   dlg.SetData(*m_old_dialog_data);
   
   int result = dlg.DoModal();
   if (result == IDOK)
   {
      CoastTrackData &ctrack_data = dlg.GetData();
      m_coast_track_list.GetAt(position)->invalidate();
      m_coast_track_list.GetAt(position)->set_coast_track_data(ctrack_data, TRUE);
      
      // update the coast track to the dialog, if it is open
      if (C_gps_trail::is_gps_tools_opened())
      {
         CoastTrackPage *ctrack_page = 
            (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);
         ctrack_page->update_coast_track(index, &ctrack_data);
      }  
      
      // the gps overlay has changed and needs to be redrawn
      m_parent_gps_overlay->set_modified(TRUE);
   }
   else if (result == GET_POSITION_FROM_MAP)
   {
      MapView::m_get_position_from_map = TRUE;
      MapView::m_get_position_from_map_type = GET_POSITION_MODIFY_COAST_TRACK;
      MapView::m_get_position_from_map_index = index;
      *m_old_dialog_data = dlg.GetData();
      m_old_report_added = TRUE;
   }
}

// center_coast_track - will center the map on the given coast track
void CoastTrackManager::center_coast_track(int index)
{
   POSITION position = m_coast_track_list.FindIndex(index);

   if (position)
   {
      CoastTrack *ctrack = m_coast_track_list.GetAt(position);
      CoastTrackData &ctrack_data = ctrack->get_coast_track_data();
      
      // get the current map view
      MapView *map_view = fvw_get_view();
      if (map_view)
      {
         map_view->get_map_engine()->change_center(ctrack_data.get_current_position().lat, 
         ctrack_data.get_current_position().lon);
         OVL_get_overlay_manager()->InvalidateOverlay(ctrack->m_overlay);
      }
   }
}

// delete_coast_track - will delete the coast track at the given 
// index
void CoastTrackManager::delete_coast_track(int index)
{
   POSITION position = m_coast_track_list.FindIndex(index);
   if (position)
   {
      delete m_coast_track_list.GetAt(position);
      m_coast_track_list.RemoveAt(position);
   }
   
   // update the coast track dialog, if it is open
   if (C_gps_trail::is_gps_tools_opened())
   {
      CoastTrackPage *ctrack_page = 
         (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);
      ctrack_page->delete_coast_track(index);
   }  
   
   // if all the coast tracks are deleted we can stop the timer
   if (m_coast_track_list.GetCount() == 0)
   {
      m_timer_on = FALSE;
      KillTimer(NULL, 0);
   }
   
   // the gps overlay has changed
   OVL_get_overlay_manager()->InvalidateOverlay(m_parent_gps_overlay);
   m_parent_gps_overlay->set_modified(TRUE);
}

// change_label - will change the label of a coast track at the 
// given index
void CoastTrackManager::change_label(int index, CString new_label)
{
   POSITION position = m_coast_track_list.FindIndex(index);
   if (position)
   {
      CoastTrackData ctrack_data;

      // get the current coast track data 
      ctrack_data = m_coast_track_list.GetAt(position)->
         get_coast_track_data();

      // modify the label
      ctrack_data.set_label(new_label);
      m_coast_track_list.GetAt(position)->
         set_coast_track_data(ctrack_data);
   }

   // the gps overlay has changed
   m_parent_gps_overlay->set_modified(TRUE);
}

// stop_coast_track - will stop a coast track at the current time.  
// Coast tracks that are stopped will not be displayed on the screen
void CoastTrackManager::stop_coast_track(int index)
{
   POSITION position = m_coast_track_list.FindIndex(index);
   if (position)
   {
      CoastTrackData ctrack_data;

      // get  the current coast track data
      ctrack_data = m_coast_track_list.GetAt(position)->
         get_coast_track_data();

      // modifify the stop time with the current system time if the stop time
      // doesn't already exist
      if (ctrack_data.get_time_stopped().GetStatus() == COleDateTime::null)
      {
         COleDateTime time_stopped;
         CTime time = CTime::GetCurrentTime();
         tm t1;
         time.GetGmtTm(&t1);
         time_stopped.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
            t1.tm_min, t1.tm_sec);

         ctrack_data.set_time_stopped(time_stopped);
         m_coast_track_list.GetAt(position)->
            set_coast_track_data(ctrack_data);
         
         // if the GPS tools dialog is opened then we need to update the dialog
         if (C_gps_trail::is_gps_tools_opened())
         {
            CoastTrackPage *ctrack_page = 
               (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);
            
            // get the data from the current coast track and use it to
            // update the dialog's data
            ctrack_page->update_coast_track(index, &ctrack_data);
         }
      }
   }
   
   // if all coast tracks are stopped then we can stop the timer
   position = m_coast_track_list.GetHeadPosition();
   boolean_t all_stopped = TRUE;
   while (position)
   {
      CoastTrackData &ctrack_data = m_coast_track_list.GetNext(position)->
         get_coast_track_data();

      if (ctrack_data.get_time_stopped().GetStatus() == COleDateTime::null)
      {
         position = NULL;
         all_stopped = FALSE;
      }
   }

   if (all_stopped)
   {
      m_timer_on = FALSE;
      KillTimer(NULL, 0);
   }

   // the gps overlay has changed
   m_parent_gps_overlay->set_modified(TRUE);
}

boolean_t CoastTrackManager::timed_out(COleDateTime current_time)
{
   int update_rate = 
      m_parent_gps_overlay->get_properties()->get_ctrack_update_rate();

   if (current_time - m_timer_start_time > 
      COleDateTimeSpan(0, 0, 0, update_rate))
   {
      m_timer_start_time = current_time;
      return TRUE;
   }

   return FALSE;
}


// add - will add a coast track data to the list of coast tracks
int CoastTrackManager::add(CoastTrackData &ctrack_data)
{
   // get the current date time
   COleDateTime current_time;
   CTime time = CTime::GetCurrentTime();
   tm t1;
   time.GetGmtTm(&t1);
   current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
      t1.tm_min, t1.tm_sec);

   // start the coast track update time if we are about to add the first
   // coast track
   if (!m_timer_on)
   {
      m_timer_on = TRUE;

      // we we set the timer for one second intervals, but will only
      // update the coast tracks based on the update rate.  This is 
      // because the same timer is used for range and bearing updates
      SetTimer(NULL, 0, 1000, C_gps_trail::coast_track_timer_proc);
   }
   
   // add the new coast track and set its data
   m_coast_track_list.AddTail(new CoastTrack(m_parent_gps_overlay,
      m_next_vector_color));
   m_next_vector_color = get_next_color(m_next_vector_color);
   m_coast_track_list.GetTail()->set_coast_track_data(ctrack_data);

   // update the view time dialog
   CMainFrame::GetPlaybackDialog().on_new_absolute_time(
      ctrack_data.get_last_report().m_time_started);
   
   // add the coast track to the dialog, if it is open
   if (C_gps_trail::is_gps_tools_opened())
   {
      CoastTrackPage *ctrack_page = 
         (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);
      ctrack_page->add_coast_track(&ctrack_data);
   }

   m_parent_gps_overlay->set_update_coast_tracks(TRUE);
   CMainFrame::GetPlaybackDialog().on_new_absolute_time(current_time);

   // if the view time dialog isn't opened then we need to 
   // invalidate and update the coast tracks and range/bearing objects
   if (!CMainFrame::IsPlaybackDialogActive())
   {
      m_parent_gps_overlay->get_coast_track_mgr()->invalidate_coast_tracks();

      //need access to the view map
      CView *view = UTL_get_active_non_printing_view();
      if (view)
      {
         MapProj *map = UTL_get_current_view_map(view);
         m_parent_gps_overlay->get_coast_track_mgr()->update_coast_tracks(map, 
            current_time);
      }

      m_parent_gps_overlay->get_coast_track_mgr()->invalidate_coast_tracks();
   }

   // the gps overlay has changed and needs to be redrawn
   m_parent_gps_overlay->set_modified(TRUE);
   OVL_get_overlay_manager()->InvalidateOverlay(m_parent_gps_overlay);
   
   // return the 0-based index of the newly added coast track
   return m_coast_track_list.GetCount() - 1;
}

// modify - will modify a coast track with the given data
void CoastTrackManager::modify(int index, CoastTrackData &data, boolean_t modified_report /*= TRUE*/)
{
   POSITION position = m_coast_track_list.FindIndex(index);
   if (position)
      m_coast_track_list.GetAt(position)->set_coast_track_data(data, modified_report);

   COleDateTime current_time;
   CTime time = CTime::GetCurrentTime();
   tm t1;
   time.GetGmtTm(&t1);
   current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
      t1.tm_min, t1.tm_sec);

   m_parent_gps_overlay->set_update_coast_tracks(TRUE);
   CMainFrame::GetPlaybackDialog().on_new_absolute_time(current_time);

   // if the view time dialog isn't opened then we need to 
   // invalidate and update the coast tracks and range/bearing objects
   if (!CMainFrame::IsPlaybackDialogActive())
   {
      m_parent_gps_overlay->get_coast_track_mgr()->invalidate_coast_tracks();

      //need access to the view map
      CView *view = UTL_get_active_non_printing_view();
      if (view)
      {
         MapProj *map = UTL_get_current_view_map(view);
         m_parent_gps_overlay->get_coast_track_mgr()->update_coast_tracks(map, 
            current_time);
      }

      m_parent_gps_overlay->get_coast_track_mgr()->invalidate_coast_tracks();
   }

   // the gps overlay has changed and needs to be redrawn
   m_parent_gps_overlay->set_modified(TRUE);
   OVL_get_overlay_manager()->InvalidateOverlay(m_parent_gps_overlay);
}

// initialize_dialog - will take the current coast track data and 
// use it to initialize the coast track dialog (if the dialog is 
// opened)
void CoastTrackManager::initialize_dialog()
{
   if (!C_gps_trail::is_gps_tools_opened())
      return;

   CoastTrackPage *ctrack_page = 
            (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);

   POSITION position = m_coast_track_list.GetHeadPosition();
   while (position)
   {
      CoastTrackData &ctrack_data = m_coast_track_list.GetNext(position)->
         get_coast_track_data();
      ctrack_page->add_coast_track(&ctrack_data);
   }

   ctrack_page->set_update_rate(m_parent_gps_overlay->get_properties()->get_ctrack_update_rate());
}

// set the coast track update rate (in seconds)
void CoastTrackManager::set_update_rate(int rate)
{
   m_parent_gps_overlay->get_properties()->set_ctrack_update_rate(rate);
}  

// update_coast_tracks - calculates the new current position of each
// of the coast tracks.  If the GPS tools dialog is opened the current
// position will be updated
void CoastTrackManager::update_coast_tracks(MapProj *map, 
   COleDateTime current_time)
{
   POSITION position = m_coast_track_list.GetHeadPosition();
   boolean_t dlg_open = C_gps_trail::is_gps_tools_opened();

   int index = 0;
   while (position)
   {
      CoastTrack *ctrack = m_coast_track_list.GetNext(position);
      CoastTrackData &ctrack_data = ctrack->get_coast_track_data();

      // update the current position of the coast track if the track is
      // not stopped
      if (ctrack_data.get_time_stopped().GetStatus() == COleDateTime::null ||
         current_time <= ctrack_data.get_time_stopped())
         ctrack->update(map, current_time);

      // if the GPS tools dialog is opened then we need to update the
      // current position in the dialog
      if (dlg_open)
      {
         CoastTrackPage *ctrack_page = 
            (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);
         CoastTrackData &ctrack_data = ctrack->get_coast_track_data();
         
         // if this coast track has not yet been created (based on the given time)
         // then "grey" it out
         //current_time < ctrack_data.m_time_started)
         
         // get the data from the current coast track and use it to
         // update the dialog's data
         ctrack_page->update_coast_track(index, &ctrack_data);
      }
      
      index++;
   }
}

// draw the coast tracks to the screen
int CoastTrackManager::draw(MapProj *map, CDC *pDC, COleDateTime current_time)
{
   // loop through each of the coast tracks and draw them to the view
   POSITION position = m_coast_track_list.GetHeadPosition();
   while (position)
   {
      CoastTrack *ctrack = m_coast_track_list.GetNext(position);

      // if the current time is within the coast tracks valid range then
      // draw the coast track
      if (ctrack->exists_at_time(current_time))
         ctrack->draw(map, pDC);
   }

   return SUCCESS;
}

// invalidate the area of the screen covered by the coast tracks
void CoastTrackManager::invalidate_coast_tracks(boolean_t erase_background /*=FALSE*/)
{
   POSITION position = m_coast_track_list.GetHeadPosition();
   while (position)
   {
      m_coast_track_list.GetNext(position)->invalidate(erase_background);
   }
}

// Determine if the point hits any of the coast tracks in the list
boolean_t CoastTrackManager::hit_test(CPoint point)
{
   POSITION position = m_coast_track_list.GetHeadPosition();
   while (position)
   {
      if (m_coast_track_list.GetNext(position)->hit_test(point))
         return TRUE;
   }

   return FALSE;
}

// determine if the point hits any of the coast tracks in the list,
// if so then return a pointer to the coast track icon
C_icon *CoastTrackManager::hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point)
{
   POSITION position = m_coast_track_list.GetHeadPosition();
   while (position)
   {
      CoastTrack *ctrack = m_coast_track_list.GetNext(position);
      if (ctrack->hit_test(point))
         return (C_icon *)ctrack;
   }

   return NULL;
}

// toggle_selection - will flip the selection status of the given
// coast track and update the GPS tools dialog if necessary
void CoastTrackManager::toggle_selection(CoastTrack *ctrack)
{
   // toggle selection of the given coast track
   ctrack->set_selected(!ctrack->is_selected());
   ctrack->invalidate();

   if (C_gps_trail::is_gps_tools_opened())
   {
      // get the position of the given coast track
      POSITION ctrack_position = m_coast_track_list.Find(ctrack);
      if (ctrack_position == NULL)
         return;
      
      // get the index of the given coast track
      POSITION current_position = m_coast_track_list.GetHeadPosition();
      int index = 0;
      while (current_position != ctrack_position)
      {
         index++;
         m_coast_track_list.GetNext(current_position);
      }

      // get a pointer to the coast track page
      CoastTrackPage *ctrack_page = 
            (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);

      // update the page with the new selection information
      if (ctrack_page)
         ctrack_page->set_selected(index, ctrack->is_selected());
   }
}

// set_selected - will set the state of the coast track with the
// given index
void CoastTrackManager::set_selected(int index, boolean_t is_selected)
{
   //POSITION position = m_coast_track_list.FindIndex(index);
   //if (position)
   //   m_coast_track_list.GetAt(position)->set_selected(is_selected);
}

// set the current time, this will determine if the coast tracks need to
// be drawn and if so, where they should be drawn
void CoastTrackManager::set_current_view_time(MapProj *map, COleDateTime time)
{
   m_current_view_time = time;
   invalidate_coast_tracks();
   update_coast_tracks(map, time);
   invalidate_coast_tracks();
}

// get the time span for the current coast tracks.  Returns TRUE if the start
// and end time are valid.
boolean_t CoastTrackManager::get_view_time_span(COleDateTime &begin, COleDateTime &end)
{
   // if there are no coast tracks then there does not exist a begin/end time
   if (m_coast_track_list.GetCount() == 0)
      return FALSE;
   
   POSITION position = m_coast_track_list.GetHeadPosition();
   boolean_t initialized = FALSE;
   while (position)
   {
      CoastTrackData &data = m_coast_track_list.GetNext(position)->
         get_coast_track_data();
      
      if (!initialized)
      {
         begin = data.get_first_report().m_time_started;
         
         if (data.get_time_stopped().GetStatus() == COleDateTime::null)
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
            end = data.get_time_stopped();
         
         initialized = TRUE;
      }
      
      // if the starting time of the current coast track is before 'begin'
      if (data.get_first_report().m_time_started < begin)
         begin = data.get_first_report().m_time_started;
      
      // if the stopping time of the current coast track is after 'end' and 
      // the ending time is valid
      if (data.get_time_stopped().GetStatus() != COleDateTime::null &&
         data.get_time_stopped() > end)
         end = data.get_time_stopped();
   }
   
   return TRUE;
}

// how many bytes will it take to write the coast tracks to a file
int CoastTrackManager::get_block_size()
{
   POSITION position = m_coast_track_list.GetHeadPosition();
   int size = 0;

   // write out the number of coast tracks
   size += sizeof(int);

   // write out data for the coast tracks
   while (position)
      size += m_coast_track_list.GetNext(position)->get_block_size();

   return size;
}

int CoastTrackManager::Serialize(BYTE *&block)
{
   // number of coast tracks
   int num_ctracks = m_coast_track_list.GetCount();
   memcpy(block, &num_ctracks, sizeof(int));
   block += sizeof(int);

   // write out data for each of the coast tracks
   POSITION position = m_coast_track_list.GetHeadPosition();
   while (position)
      m_coast_track_list.GetNext(position)->Serialize(block);

   return SUCCESS;
}

int CoastTrackManager::deserialize(BYTE *block, C_overlay *overlay)
{
   // clear the current coast tracks from memory
   while (!m_coast_track_list.IsEmpty())
      delete m_coast_track_list.RemoveTail();

   // get the number of coast tracks
   int num_ctracks;
   memcpy(&num_ctracks, block, sizeof(int));
   block += sizeof(int);

   for(int i=0; i<num_ctracks;i++)
   {
      m_coast_track_list.AddTail(new CoastTrack(overlay));
      m_coast_track_list.GetTail()->deserialize(block);
   }

   return SUCCESS;
}

// get the next color for the course vector given the current color
int CoastTrackManager::get_next_color(int current_color)
{
   switch (current_color)
   {
   case UTIL_COLOR_GREEN: return UTIL_COLOR_RED;
   case UTIL_COLOR_RED: return UTIL_COLOR_BLUE;
   case UTIL_COLOR_BLUE: return UTIL_COLOR_YELLOW;
   case UTIL_COLOR_YELLOW: return UTIL_COLOR_MAGENTA;
   case UTIL_COLOR_MAGENTA: return UTIL_COLOR_CYAN;
   case UTIL_COLOR_CYAN: return UTIL_COLOR_DARK_GRAY;
   case UTIL_COLOR_DARK_GRAY: return UTIL_COLOR_GREEN;
   }

   return UTIL_COLOR_GREEN;
}

//
// CoastTrackData implementation 
//

// constructor
CoastTrackData::CoastTrackData(CString label, COleDateTime time_started,
                               double heading, double velocity,
                               d_geo_t initial_position)
{
   m_label = label;
   m_time_stopped.SetStatus(COleDateTime::null);
}

CoastTrackData::CoastTrackData()
{
   // default projected heading type
   m_projected_heading = GeoSegment::RHUMB_LINE;
}

// copy constructor
CoastTrackData &CoastTrackData::operator=(CoastTrackData &data)
{
   m_label = CString(data.m_label);
   m_time_stopped = data.m_time_stopped;

   // the coast tracks current position
   m_current_position = data.m_current_position;

   // Report list
   m_report_lst.RemoveAll();
   m_report_lst.AddTail(&data.m_report_lst);

   m_units_velocity = data.m_units_velocity;

   m_course_vector = data.m_course_vector;

   // projected heading type (either GeoSegment::GREAT_CIRCLE or 
   // GeoSegment::RHUMB_LINE, default is RHUMB_LINE)
   m_projected_heading = data.m_projected_heading;

   // show trajectory from initial position on/off
   m_initial_traj_on = data.m_initial_traj_on;

   return *this;
}

// how many bytes will it take to write this coast track data to a file
int CoastTrackData::get_block_size()
{
   int size = 0;

   // size of the label plus the bytes for the label
   size += sizeof(int) + m_label.GetLength();

   // time stopped
   size += sizeof(double);

   // write out the number of report data structs
   size += sizeof(int);

   // each report needs to write out the time started, heading, velocity, and
   // initial_position
   {
      int num_reports = m_report_lst.GetCount();

      // time started 
      size += sizeof(double) * num_reports;

      // heading
      size += sizeof(double) * num_reports;
      
      // velocity
      size += sizeof(double) * num_reports;
      
      // initial position (lat, lon)
      size += sizeof(double) * 2 * num_reports;
   }

   // velocity units
   size += sizeof(short);

   // course vector
   size += m_course_vector.get_block_size();

   // trajectory from initial on/off
   size += sizeof(boolean_t);

   return size;
}

int CoastTrackData::Serialize(BYTE *&block)
{
   // write out the size of the label
   int label_length = m_label.GetLength();
   memcpy(block, &label_length, sizeof(int));
   block += sizeof(int);

   // write out the string
   if (label_length != 0)
   {
      memcpy(block, m_label.GetBuffer(label_length), label_length);
      block += label_length;
   }

   // write out time stopped
   {
      double time_stopped;

      // if the coast track is not stopped we should time stamp
      // it with the current time
      if (m_time_stopped.GetStatus() == COleDateTime::null)
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
         time_stopped = (double)m_time_stopped.m_dt;

      memcpy(block, &time_stopped, sizeof(double));
      block += sizeof(double);
   }

   // write out the number of reports
   int num_reports = m_report_lst.GetCount();
   memcpy(block, &num_reports, sizeof(int));
   block += sizeof(int);

   POSITION position = m_report_lst.GetHeadPosition();
   while (position)
   {
      ReportData report_data = m_report_lst.GetNext(position);

      // write out time started
      double time_started = (double)report_data.m_time_started.m_dt;
      memcpy(block, &time_started, sizeof(double));
      block += sizeof(double);
      
      // write out the heading
      memcpy(block, &report_data.m_heading, sizeof(double));
      block += sizeof(double);
      
      // write out the velocity
      memcpy(block, &report_data.m_velocity, sizeof(double));
      block += sizeof(double);
      
      // write out initial position
      memcpy(block, &(report_data.m_initial_position.lat), sizeof(double));
      block += sizeof(double);
      
      memcpy(block, &(report_data.m_initial_position.lon), sizeof(double));
      block += sizeof(double);
   }

   // write out the velocity units
   memcpy(block, &m_units_velocity, sizeof(short));
   block += sizeof(short);

   // write out the course vector
   m_course_vector.Serialize(block);

   // write out the trajectory from initial position on/off
   memcpy(block, &m_initial_traj_on, sizeof(boolean_t));
   block += sizeof(boolean_t);

   return SUCCESS;
}

int CoastTrackData::deserialize(BYTE *&block)
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
      m_label = CString(tmp_buffer);
   }

   // read time stopped
   double time_stopped; 
   memcpy(&time_stopped, block, sizeof(double));
   m_time_stopped = COleDateTime((DATE)time_stopped);
   block += sizeof(double);

   // read in the number of reports
   int num_reports;
   memcpy(&num_reports, block, sizeof(int));
   block += sizeof(int);

   for(int i=0;i<num_reports;i++)
   {
      ReportData &report_data = add_report();

      // read time started
      double time_started;
      memcpy(&time_started, block, sizeof(double));
      report_data.m_time_started = COleDateTime((DATE)time_started);
      block += sizeof(double);
      
      // read the heading
      memcpy(&report_data.m_heading, block, sizeof(double));
      block += sizeof(double);
      
      // read the velocity
      memcpy(&report_data.m_velocity, block, sizeof(double));
      block += sizeof(double);
      
      // read initial position
      memcpy(&(report_data.m_initial_position.lat), block, sizeof(double));
      block += sizeof(double);
      
      memcpy(&(report_data.m_initial_position.lon), block, sizeof(double));
      block += sizeof(double);
   }

   // read the velocity units
   memcpy(&m_units_velocity, block, sizeof(short));
   block += sizeof(short);
   
   // read the course vector
   m_course_vector.deserialize(block);

   // read show trajectory from initial position on/off
   memcpy(&m_initial_traj_on, block, sizeof(boolean_t));
   block += sizeof(boolean_t);

   return SUCCESS;
}

ReportData &CoastTrackData::get_report(COleDateTime time)
{
   POSITION position = m_report_lst.GetHeadPosition();
   while (position)
   {
      ReportData data1, data2;
      POSITION old_position = position;
      
      data1 = m_report_lst.GetNext(position);
      
      if (position)
         data2 = m_report_lst.GetAt(position);
      else
         return m_report_lst.GetAt(old_position);

      if (time >= data1.m_time_started && time < data2.m_time_started)
         return m_report_lst.GetAt(old_position);
   }
         
   return m_report_lst.GetTail();
}

// how many bytes will it take to write this course vector data to a file
int CourseVector::get_block_size()
{
   int size = 0;

   // course vector on/off
   size += sizeof(boolean_t);

   // course vector distance or time?
   size += sizeof(boolean_t);

   // time in seconds to project vector
   size += sizeof(double);

   // distance in meters to project vector
   size += sizeof(double);

   // units for dist/time
   size += sizeof(units_t)*2;

   return size;
}

int CourseVector::Serialize(BYTE *&block)
{
   // course vector on/off
   memcpy(block, &m_on, sizeof(boolean_t));
   block += sizeof(boolean_t);

   // course vector distance or time?
   memcpy(block, &m_is_dist, sizeof(boolean_t));
   block += sizeof(boolean_t);

   // time in seconds to project vector
   memcpy(block, &m_time, sizeof(double));
   block += sizeof(double);

   // distance in meters to project vector
   memcpy(block, &m_distance, sizeof(double));
   block += sizeof(double);

   // units for dist/time
   memcpy(block, &m_units_dist, sizeof(units_t));
   block += sizeof(units_t);

   memcpy(block, &m_units_time, sizeof(units_t));
   block += sizeof(units_t);

   return SUCCESS;
}

int CourseVector::deserialize(BYTE *&block)
{
   // course vector on/off
   memcpy(&m_on, block, sizeof(boolean_t));
   block += sizeof(boolean_t);
   
   // course vector distance or time?
   memcpy(&m_is_dist, block, sizeof(boolean_t));
   block += sizeof(boolean_t);
   
   // time in seconds to project vector
   memcpy(&m_time, block, sizeof(double));
   block += sizeof(double);
   
   // distance in meters to project vector
   memcpy(&m_distance, block, sizeof(double));
   block += sizeof(double);
   
   // units for dist/time
   memcpy(&m_units_dist, block, sizeof(units_t));
   block += sizeof(units_t);
   
   memcpy(&m_units_time, block, sizeof(units_t));
   block += sizeof(units_t);

   return SUCCESS;
}

//
// CoastTrack implementation 
//

// constructors
CoastTrack::CoastTrack(C_overlay *overlay, int vector_color) : C_icon(overlay)
{
   // set the pen for the course vector GeoLine
   OvlPen &pen = m_course_vector_line.get_pen();

   pen.set_foreground_pen(vector_color, UTIL_LINE_SOLID, 2);
   pen.set_background_pen(UTIL_COLOR_BLACK);

   // set the pen for the initial vector GeoLine
   OvlPen &pen2 = m_initial_vector_line.get_pen();

   pen2.set_foreground_pen(UTIL_COLOR_WHITE, UTIL_LINE_ROUND_DOT, 4);
   pen.set_background_pen(UTIL_COLOR_BLACK);

   // the coast track is initially not selected
   m_selected = FALSE;

   m_position_set = FALSE;
}

// update the current position of this coast track
void CoastTrack::update(MapProj *map, COleDateTime time)
{
   d_geo_t projected_geo;
   ReportData &report_data = m_track_data.get_report(time);

   // get the position of the coast track at the given time
   d_geo_t current_position;
   get_position(time, &current_position);
   m_track_data.set_current_position(current_position);
   m_position_set = TRUE;

   // if the course vector is on
   if (m_track_data.get_course_vector().m_on)
   {
      // get the endpoint of the course vector
      get_projected_geo(time, m_track_data.get_current_position(),
         &projected_geo);
      
      // set the starting and ending points of the course vector
      m_course_vector_line.set_start(m_track_data.get_current_position().lat,
         m_track_data.get_current_position().lon);
      m_course_vector_line.set_end(projected_geo.lat, 
         projected_geo.lon);
   }

   // if the initial vector is on
   if (m_track_data.get_initial_traj_on())
   {
      // set the starting point of the initial vector
      m_initial_vector_line.set_start(report_data.m_initial_position.lat,
         report_data.m_initial_position.lon);

      // set the new ending point of the initial vector
      m_initial_vector_line.set_end(m_track_data.get_current_position().lat,
         m_track_data.get_current_position().lon);
   }

   // any range and bearing objects that reference this coast track
   // need to be notified of the update
   POSITION position = rb_ref_lst.GetHeadPosition();
   while (position)
   {
      rb_ref_t ref = rb_ref_lst.GetNext(position);
      ref.rb_obj->update(map, current_position, ref.from_not_to);
   }
}

// get the current position of this coast track with the given time
void CoastTrack::get_position(COleDateTime time, d_geo_t *current_position)
{                      
   // get the report with the given time
   ReportData &report_data = m_track_data.get_report(time);
   
   // figure out the difference in time from the starting time to the 
   // given time
   COleDateTimeSpan delta_time = time - report_data.m_time_started;

   // figure out the distance the coast track traveled from the initial 
   // position in meters
   double distance = report_data.m_velocity*delta_time.GetTotalSeconds();
   distance = FEET_TO_METERS(distance);

   // get the new position
   GEO_calc_end_point(report_data.m_initial_position, distance, 
      report_data.m_heading, *current_position, 
      m_track_data.get_projected_heading());
}

// get the endpoint of the course vector at the given position
void CoastTrack::get_projected_geo(COleDateTime time, d_geo_t position,
                                   d_geo_t *projected_geo)
{
      double distance;
      ReportData &report_data = m_track_data.get_report(time);

      if (m_track_data.get_course_vector().m_is_dist)
      {
         distance = m_track_data.get_course_vector().m_distance;
      }  
      else
      {
         distance = FEET_TO_METERS(report_data.m_velocity*
            m_track_data.get_course_vector().m_time);
      }  

      GEO_calc_end_point(position, 
            distance, report_data.m_heading,
            *projected_geo, m_track_data.get_projected_heading());
}

// how many bytes will it take to write this coast track data to a file
int CoastTrack::get_block_size()
{
   int size = m_track_data.get_block_size();
   
   // course vector color
   size += sizeof(int);

   return size;
}

int CoastTrack::Serialize(BYTE *&block)
{
   m_track_data.Serialize(block);

   // write the pen color of the course vector GeoLine
   {
      OvlPen pen = m_course_vector_line.get_pen();
      int vector_color = pen.get_foreground_color();

      memcpy(block, &vector_color, sizeof(int));
      block += sizeof(int);
   }

   return SUCCESS;
}

int CoastTrack::deserialize(BYTE *&block)
{
   m_track_data.deserialize(block);

   // read and set the pen color for the course vector GeoLine
   {
      OvlPen &pen = m_course_vector_line.get_pen();
      int vector_color;

      memcpy(&vector_color, block, sizeof(int));
      block += sizeof(int);
      
      pen.set_foreground_pen(vector_color, UTIL_LINE_SOLID, 2);
   }

   // calculate the position of the coast track at the stop time
   d_geo_t current_position;
   get_position(m_track_data.get_time_stopped(), &current_position);
   m_track_data.set_current_position(current_position);
   m_position_set = TRUE;

   return SUCCESS;
}

// draw the coast track to the view
void CoastTrack::draw(MapProj *map, CDC *pDC)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   UtilDraw util(pDC);
   int x, y;

   // make sure the position has been set, if not then we can't draw
   // the coast track
   if (!m_position_set)
      return;
   
   // draw the velocity vector if it is on
   if (m_track_data.get_course_vector().m_on)
      get_course_vector()->draw(map, pDC);
   
   // draw the vector from the initial position to the coast track
   // if it is on
   if (m_track_data.get_initial_traj_on())
      get_initial_vector()->draw(map, pDC);
   
   // get the screen coordinates of the coast track's current
   // position and set this for the coast track
   d_geo_t current_pos = { m_track_data.get_current_position().lat,
      m_track_data.get_current_position().lon };
   map->geo_to_surface(current_pos.lat, current_pos.lon, &x, &y);
   set_position(CPoint(x, y));

   // define the radius of the circle
   const int radius = 10;

   // define the line thickness
   const int line_width = 3;

   C_gps_trail *pTrail = dynamic_cast<C_gps_trail *>(m_overlay);
   ReportData &report_data = m_track_data.get_report(pTrail->get_current_view_time());

   double convergence_angle;
   map->get_meridian_covergence(current_pos, &convergence_angle);
   double angle = futil->heading_to_cartesian_angle(report_data.m_heading +
      map->actual_rotation() + convergence_angle);

   // calculate the endpoints of the heading arrow
   int x1, y1;
   int x2, y2;
   int px, py;
   {
      x1 = futil->round(x + 5.0*cos(DEG_TO_RAD(angle+90)));
      y1 = futil->round(y - 5.0*sin(DEG_TO_RAD(angle+90)));

      x2 = futil->round(x + 5.0*cos(DEG_TO_RAD(angle-90)));
      y2 = futil->round(y - 5.0*sin(DEG_TO_RAD(angle-90)));

      px = futil->round(x + 17.0*cos(DEG_TO_RAD(angle)));
      py = futil->round(y - 17.0*sin(DEG_TO_RAD(angle)));
   }

   // draw background
   util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, line_width + 2);

   util.draw_circle(FALSE, x, y, radius);

   util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, 2 + 2);
   util.draw_line(x1, y1, px, py);
   util.draw_line(x2, y2, px, py);
   util.draw_line(x1, y1, x2, y2);

   // draw foreground
   util.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, line_width);

   util.draw_circle(FALSE, x, y, radius);

   util.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, 2);
   util.draw_line(x1, y1, px, py);
   util.draw_line(x2, y2, px, py);
   util.draw_line(x1, y1, x2, y2);
}

// invalidate the part of the screen covered by the coast track
void CoastTrack::invalidate(boolean_t erase_background /*=FALSE*/)
{  
   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

// Returns TRUE if this icon is a instance of the given class.
boolean_t CoastTrack::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "CoastTrack") == 0)
      return TRUE;

   return C_icon::is_kind_of(class_name);
}

// Determine if the point hits the coast track
boolean_t CoastTrack::hit_test(CPoint point)
{
   CRect rect(m_position.x, m_position.y, m_position.x, m_position.y);
   rect.InflateRect(13,13);

   return rect.PtInRect(point);
}

HintText CoastTrack::get_hint()
{
   HintText hint;

   hint.set_help_text(get_help_text());
   hint.set_tool_tip(get_tool_tip());

   return hint;
}

CString CoastTrack::get_help_text()
{
   return m_track_data.get_label();
}

CString CoastTrack::get_tool_tip()
{
   return m_track_data.get_label();
}

// does the coast track exist at the given time
boolean_t CoastTrack::exists_at_time(COleDateTime dt)
{
   // assume the coast track exists if we are given an invalid date time
   if (dt.GetStatus() == COleDateTime::null)
      return FALSE;

   // TRUE if dt is later then the time started and before the time ended, if 
   // ended
   return (m_track_data.get_first_report().m_time_started <= dt &&
         (m_track_data.get_time_stopped().GetStatus() == COleDateTime::null ||
          dt <= m_track_data.get_time_stopped()));
}

void CoastTrack::set_coast_track_data(CoastTrackData &data, 
                                      boolean_t update_report /*TRUE*/)
{ 
   // update all info in the coast track if update_report = TRUE and 
   if (update_report)
   {
      m_track_data = data; 
      
      // update the view time dialog with the latest updated time
      CMainFrame::GetPlaybackDialog().on_new_absolute_time(
         m_track_data.get_last_report().m_time_started);
      
      return;
   }

   // otherwise, only update the display options without changing the 
   // position, heading, and speed
   m_track_data.set_label(data.get_label());
   m_track_data.set_projected_heading(data.get_projected_heading());
   m_track_data.set_course_vector(data.get_course_vector());
   m_track_data.set_initial_traj_on(data.get_initial_traj_on());
}

void CoastTrack::add_range_bearing_reference(RangeBearing *rb_obj, 
   boolean_t from_not_to)
{
   rb_ref_lst.AddTail(rb_ref_t(rb_obj, from_not_to));
   rb_obj->update(NULL, m_track_data.get_current_position(), 
      from_not_to);
}

void CoastTrack::remove_range_bearing_reference(RangeBearing *rb_obj)
{
   POSITION position = rb_ref_lst.GetHeadPosition();
   while (position)
   {
      rb_ref_t ref = rb_ref_lst.GetAt(position);
      if (ref.rb_obj == rb_obj)
      {
         rb_ref_lst.RemoveAt(position);
         break;
      }
      rb_ref_lst.GetNext(position);
   }
}