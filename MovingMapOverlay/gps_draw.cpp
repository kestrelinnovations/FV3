// Copyright (c) 1994-2011,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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
#include "wm_user.h"
#include "TabularEditorDlg.h"
#include "graphics.h"
#include "err.h"
#include "mov_sym.h"    // the moving symbol overlay allow control of the 3d eyepoint 
#include "..\SkyViewOverlay\skyview.h"
#include "..\SkyViewOverlay\factory.h"
#include "map.h"
#include "csttrack.h"
#include "rb.h"
#include "Common\map_server_client.h"   // contains message types for the WM_COPY message
#include "PredictivePath.h"
#include "PredictivePathRenderer.h"
#include "factory.h"
#include "FctryLst.h"
#include "..\TaMask\factory.h"
#include "TAMask.h"     // For access to TAMask overlay existance/status
#include "cdi.h"
#include "..\getobjpr.h"
#include "..\VerticalViewDisplay.h" // for CVerticalViewProjector
#include "showrmk.h"
#include "ovlElementContainer.h"
#include "..\StatusBarInfoPane.h"
#include "..\overlay\OverlayCOM.h"
#include "..\PlaybackDialog\viewtime.h"
#include "ovl_mgr.h"

#define CONSTANT_UPDATE 0
#define DRAW_RECENTER_RECT 0

int C_gps_trail::draw(ActiveMap* map)
{
   COleDateTime draw_time;

   // set the heading based on the current map
   m_north_up_angle = get_current_heading(0);

   // get the draw time
   draw_time = CMainFrame::GetPlaybackDialog().get_current_time();

   // draw the coast tracks
   get_coast_track_mgr()->draw(map, map->get_CDC(), draw_time);

   // draw the range/bearing objects
   get_rb_mgr()->draw(map, map->get_CDC(), draw_time);

   GetOvlElementContainer()->draw(map, map->get_CDC());

   if (get_valid())
      return redraw(map);
   else
      return new_draw(map);        

   return SUCCESS;
}

// Derived classes can implement this method if they need to draw to the 
// vertical view
int C_gps_trail::DrawToVerticalDisplay(CDC *pDC, CVerticalViewProjector* pProjector)
{
   d_geo_t anchor = {m_current_point.get_latitude(), m_current_point.get_longitude() };

   // there is nothing to draw if there isn't a current point
   if (anchor.lat == -1000 && anchor.lon == -1000)
      return SUCCESS;

   if (m_current_point.m_rect.IsRectEmpty())
      return SUCCESS;

   m_shipSymbol_2->SetAltitudeMeters(m_current_point.m_msl);
   m_shipSymbol_2->draw(pProjector, pDC);

   short sElevMeters;
   if (DTD_get_elevation_in_meters(m_current_point.get_latitude(), m_current_point.get_longitude(), &sElevMeters) == SUCCESS)
   {
      int rX, rY;
      if (pProjector->ToSurface(m_current_point.get_latitude(), m_current_point.get_longitude(), sElevMeters, rX, rY) == SUCCESS)
      {
         UtilDraw utilDraw(pDC);

         utilDraw.set_pen(RGB(0, 0, 0), UTIL_LINE_SOLID, 5, TRUE);
         utilDraw.draw_line(rX - 8, rY, rX + 8, rY);

         COLORREF color = RGB(255, 255, 255);
         C_TAMask_ovl *pOverlay = static_cast<C_TAMask_ovl *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask));
         if (pOverlay != NULL)
            color = pOverlay->ConvertAltitudeToColor(m_current_point.m_msl, sElevMeters);

         utilDraw.set_pen(color, UTIL_LINE_SOLID, 3);
         utilDraw.draw_line(rX - 8, rY, rX + 8, rY);
      }
   }

   return SUCCESS;
}

void C_gps_trail::ship_draw(ActiveMap *map, CRect clip_rect /*=CRect(-1,-1,-1,-1)*/)
{
   m_current_point.build_display_elements();

   // draw bottom display elements (underneath the ship symbol)
   m_current_point.draw_display_list(map, map->get_CDC(), FALSE);

   // draw the ship symbol if it is on the screen
   int x,y;
   int iResult = point_transform( map, &m_current_point, x,y );
   if (iResult < 0)
      m_current_point.m_rect.SetRectEmpty();

   CFvwUtil *futil = CFvwUtil::get_instance();
   const bool bDisplaySymbol = m_show_symbol && iResult >= 0;

   if (bDisplaySymbol && iResult >= 0)
   {
      d_geo_t anchor = {m_current_point.get_latitude(), m_current_point.get_longitude() };
      m_shipSymbol->set_anchor(anchor);
      m_shipSymbol_2->set_anchor(anchor);
      m_shipSymbol->set_rotation(m_north_up_angle);
      m_shipSymbol_2->set_rotation(m_north_up_angle < 180.0 ? 90.0 : 270.0);
      m_shipSymbol->set_selected(m_selected_point == &m_current_point);
      m_shipSymbol->draw(map, map->get_CDC());

      // if the feed is frozen draw an X on the ship to indicate this
      if (is_frozen())
      {
         UtilDraw util(map->get_CDC());

         util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, 4);
         util.draw_no_map_data_symbol(CPoint(x,y), static_cast<int>(m_shipSymbol->get_scale()));

         util.set_pen(UTIL_COLOR_RED, UTIL_LINE_SOLID, 2);
         util.draw_no_map_data_symbol(CPoint(x,y), static_cast<int>(m_shipSymbol->get_scale())); 
      }
   }

   // draw the range ring if it is on
   if (m_properties.GetBullseyeProperties()->get_bullseye_on() && m_current_point.m_true_heading != -1.0)
   {
      m_bullseye.SetCenterLocation(m_current_point.get_latitude(), m_current_point.get_longitude());
      m_bullseye.SetNumRangeRings(m_properties.GetBullseyeProperties()->get_num_range_rings());
      m_bullseye.SetDistBetweenRings(m_properties.GetBullseyeProperties()->get_range_radius());
      m_bullseye.SetHeading(m_current_point.m_true_heading);
      m_bullseye.SetRelativeAzimuth(m_properties.GetBullseyeProperties()->get_relative_azimuth());
      m_bullseye.SetNumRadials(m_properties.GetBullseyeProperties()->get_num_radials());
      m_bullseye.SetAngleBetweenRadials(m_properties.GetBullseyeProperties()->get_angle_between_radials());

      OvlPen& bullseyePen = m_bullseye.get_pen();
      bullseyePen.set_foreground_pen(m_properties.get_ship_fg_color(), UTIL_LINE_SOLID, m_properties.get_ship_line_size());
      bullseyePen.set_background_pen(m_properties.get_ship_bg_color());

      m_bullseye.draw(map, map->get_CDC());
   }

   // draw the altitude label if it is on
   if (bDisplaySymbol && m_properties.get_display_altitude_label() && m_current_point.m_msl != GPS_UNKNOWN_ALTITUDE)
   {
      POINT cpt[4];
      const int ship_size = m_properties.get_ship_size();

      double alt_ft = METERS_TO_FEET(m_current_point.m_msl) / 100.0;
      CString alt_str;
      alt_str.Format("%d", (int)(alt_ft + 0.5));
      while (alt_str.GetLength() < 3)
         alt_str = "0" + alt_str;

      CString font_name;
      int size, attributes;
      int fg_color;
      int bg_type, back_color;
      OvlFont &font = m_properties.get_nickname_font();
      font.get_font(font_name, size, attributes);
      font.get_foreground(fg_color);
      font.get_background(bg_type, back_color);

      int width, height;
      futil->get_text_size(map->get_CDC(), alt_str, font_name, 
         size, attributes, &width, &height);

      int x_offset = -(int)((ship_size + width / 2) * sin(DEG_TO_RAD(90 - m_north_up_angle - map->actual_rotation())) + 0.5);
      int y_offset = -(int)((ship_size + width / 2) * cos(DEG_TO_RAD(90 - m_north_up_angle - map->actual_rotation())) + 0.5);

      futil->draw_text(map->get_CDC(), alt_str, x + x_offset, y + y_offset, 
         UTIL_ANCHOR_CENTER_CENTER, font_name, size, attributes,
         bg_type, fg_color, back_color, 0.0, cpt);

      m_altitude_label_bounds = CRect(cpt[0].x, cpt[0].y, cpt[2].x, cpt[2].y);
   }

   if (bDisplaySymbol && m_properties.get_nickname_label().GetLength() && m_properties.GetDisplayNickname())
   {
      POINT cpt[4];
      const int ship_size = m_properties.get_ship_size();

      CString font_name;
      int size, attributes;
      int fg_color;
      int bg_type, back_color;
      OvlFont &font = m_properties.get_nickname_font();
      font.get_font(font_name, size, attributes);
      font.get_foreground(fg_color);
      font.get_background(bg_type, back_color);

      int width, height;
      futil->get_text_size(map->get_CDC(), m_properties.get_nickname_label(), font_name, 
         size, attributes, &width, &height);

      int x_offset = -(int)((ship_size + width / 2) * sin(DEG_TO_RAD(270 - m_north_up_angle - map->actual_rotation())) + 0.5);
      int y_offset = -(int)((ship_size + width / 2) * cos(DEG_TO_RAD(270 - m_north_up_angle - map->actual_rotation())) + 0.5);

      int x, y;
      map->geo_to_surface(m_current_point.get_latitude(), m_current_point.get_longitude(),
         &x, &y);
      futil->draw_text(map->get_CDC(), m_properties.get_nickname_label(), x + x_offset, y + y_offset, 
         UTIL_ANCHOR_CENTER_CENTER, font_name, size, attributes,
         bg_type, fg_color, back_color, 0.0, cpt);

      m_nickname_label_bounds = CRect(cpt[0].x, cpt[0].y, cpt[2].x, cpt[2].y);
   }

   draw_predictive_points(map);

   if (bDisplaySymbol)
   {
      m_current_point.m_rect = m_shipSymbol->get_rect();
      ASSERT(m_current_point.m_rect != CRect(0,0,0,0));
   }

   // draw top display elements (above the ship symbol)
   m_current_point.draw_display_list(map, map->get_CDC(), TRUE);
}

void C_gps_trail::cdi_update()
{
   //gotta have a cdi
   if (!m_cdi)
      return;

   if (!C_gps_trail::get_toggle('CDI'))
      return;

   //get the overlay manager
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   // get the topmost moving map trail
   C_gps_trail *pTrail = dynamic_cast<C_gps_trail *>(ovl_mgr->get_first_of_type(FVWID_Overlay_MovingMapTrail));

   //only update the topmost moving map trail
   if (this != pTrail)
      return;

   // get the topmost route
   COverlayCOM *pRoute = dynamic_cast<COverlayCOM *>(ovl_mgr->get_first_of_type(FVWID_Overlay_Route));

   //update the cdi
   //hopefull this mechanism will prevent an update from occuring after
   //the window has been destroyed
   if (IsWindow(m_cdi->m_hWnd))
      m_cdi->PostMessage( WM_USER_UPDATE, (WPARAM)pRoute, (LPARAM)&m_current_point );
}

void C_gps_trail::update_3d_camera()
{
   if (skyview_overlay::IsOverlayTypeEnabled())
   {
      update_skyview();
   }
   else
   {
      GPSPointIcon& position = get_current_position();
      if (m_update_3d_camera)
      {

         // Update the internal camera position if currently in the 3D projection
         MapView* map_view = fvw_get_view();
         if (map_view)
         {
            GPSPointIcon& position = get_current_position();
            MapView::AttachedCameraParams camera_params = {
               position.get_latitude(), position.get_longitude(),
               position.m_msl, position.m_true_heading, 5.0, 0.0 };
               map_view->SetCameraPositionWhenAttached(camera_params);
         }
      }

      // Add or update the ship icon
#if 1
#if 1
      DWORD dwTicks = ::GetTickCount();
      if ( 900 > (INT) ( dwTicks - m_dwLastShip3DUpdateTicks ) )  // If too soon
      {
         m_p3DCatchUpTimer->restart();   // Throttle, wake up later
      }
      else
      {
         m_p3DCatchUpTimer->stop();                  // Will be up-to-date
         m_dwLastShip3DUpdateTicks = dwTicks;      // To prevent overruns
#else
      {
#endif
         ATL::CCritSecLock lock( s_idThreadInterlock );

         if ( m_ptShip3D.m_3DHandle != 0  // Ship drawn
            && !m_update_3d_camera        // Not attached by camera
            && 2.0 < fabs(
               fmod( m_ptShip3D.m_dHeadingDeg - position.m_true_heading + 540.0, 360.0 ) - 180.0 ) )
         {
            Remove3DPoint( m_ptShip3D.m_3DHandle );   // Need to rotate ship graphic
            m_ptShip3D.m_3DHandle = 0;
         }

         m_ptShip3D.m_gptLatLon.lat = position.get_latitude();
         m_ptShip3D.m_gptLatLon.lon = position.get_longitude();
         m_ptShip3D.m_dAltMeters = position.m_msl;
         m_ptShip3D.m_dHeadingDeg = position.m_true_heading;
         m_ptShip3D.m_dSpeedFtPerSec = KNOTS_TO_FEET_PER_S( position.m_speed_knots );
         m_ptShip3D.m_dtDateTime = position.get_date_time();
         Update3DPoint( &m_ptShip3D );
      }
#endif
   }
}

void C_gps_trail::update_skyview()
{
   if (!m_update_3d_camera)
   {
      return;
   }

   // if the overlay is opened and there is a symbol created on it
   if (Cmov_sym_overlay::GetStateIndicators()->m_symbol != NULL &&
      s_skyview_object_handle != -1)
   {
      // Update SkyView's playback.  The current values of playback time advancement
      // enablement and playback time advancement multiplier are cached to avoid
      // unnecessary invocations of SkyView COM methods.

      Cmov_sym_overlay::m_skyview_interface->SetPlaybackTime(
         CMainFrame::GetPlaybackDialog().get_current_time());

      double new_multiplier = 
         CMainFrame::GetPlaybackDialog().get_playback_rate() * 
         (CMainFrame::GetPlaybackDialog().is_playback_reversed() ? -1.0 : 1.0);

      if (new_multiplier != s_sv_playback_time_advancement_multiplier)
      {
         Cmov_sym_overlay::m_skyview_interface->SetPlaybackTimeAdvancementMultiplier(new_multiplier);
         s_sv_playback_time_advancement_multiplier = new_multiplier;
      }

      bool enable = CMainFrame::GetPlaybackDialog().in_playback() ? true : false;

      if (enable != s_sv_playback_time_advancement_enabled)
      {
         Cmov_sym_overlay::m_skyview_interface->EnablePlaybackTimeAdvancement(enable);
         s_sv_playback_time_advancement_enabled = enable;
      }
   }
}

void C_gps_trail::update_TAMask()
{
   // If the Terrain Altitude Mask overlay is enabled
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   C_TAMask_ovl *pTerrainAltMaskOverlay = static_cast<C_TAMask_ovl*>(ovl_mgr->get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask));
   if (pTerrainAltMaskOverlay != NULL)
   {
      // get the altitude of the ship's current position
      float altMeters = get_current_position().m_msl;
      float altFeet;

      // convert an unknown GPS altitude to an unknown TAMask altitude if necessary
      if (altMeters == GPS_UNKNOWN_ALTITUDE)
         altFeet = -99999.0f;
      else
         altFeet = static_cast<float>(METERS_TO_FEET(altMeters));

      // update the altitude in the TAMask overlay.  The TAMask overlay will decide which overlay
      // is top-most and whether or not to use the Alt.
      pTerrainAltMaskOverlay->UpdateAltitude(get_current_position().get_latitude(),
         get_current_position().get_longitude(), altFeet, get_current_position().m_true_heading, this, m_properties.GetBullseyeProperties());
   }
}

float C_gps_trail::get_current_heading(int tracking_center_num)
{
   // if we are viewing an alternative center, then the heading returned
   // should be the heading from the ship's position to the alt. center
   if (tracking_center_num != 0)
   {
      double lat, lon;
      m_current_point.get_center(tracking_center_num, lat, lon);

      // tracking center num will be reset to 0 if the center was not found.
      // In this case, we will fall out of this conditional and compute the
      // heading as normal
      if (m_tracking_center_num != 0)
      {
         double distance, bearing;
         GEO_calc_range_and_bearing(m_current_point.get_latitude(), 
            m_current_point.get_longitude(), lat, lon, &distance, &bearing, TRUE);

         return static_cast<float>(bearing);
      }
   }

   // use the true heading from the current position point, if it is set
   if (m_current_point.m_true_heading != -1.0)
      return m_current_point.m_true_heading;

   // compute north_up_angle from points in list if possible
   if (get_icon_list()->get_count() > 0)
   {
      GPSPointIcon *point = NULL;
      POSITION next;
      float delta_lat, delta_lon;

      // get current_position point and initialize next to position
      // of point immediately after current_position in the list
      //PLAYBACK
      //next = get_icon_list()->get_last_position();
      next = get_last_position();

      // find the closes point in the list that produces
      // non-zero delta_lat and delta_lon
      do
      {
         point = (GPSPointIcon *)get_icon_list()->get_prev(next);
      }
      while ( (point->get_latitude() == m_current_point.get_latitude() && 
         point->get_longitude() == m_current_point.get_longitude()) &&
         next != NULL);

      if (next != NULL && point != NULL)
      {
         // compute change in position in geo coordinates
         delta_lat = m_current_point.get_latitude() - point->get_latitude() ;
         delta_lon = m_current_point.get_longitude() - point->get_longitude();
         if (delta_lon < -180.0)
            delta_lon += (float)360.0;
         else if (delta_lon > 180.0)
            delta_lon -= (float)360.0;

         double delta_x;
         double delta_y;
         float north_up_angle;

         delta_x = (double)delta_lon / m_deg_per_pixel.lon;
         delta_y = (double)delta_lat / m_deg_per_pixel.lat;

         // compute clockwise angle ranging from 0.0 to 360.0
         north_up_angle = (float)RAD_TO_DEG(atan2(delta_x, delta_y));
         if (delta_y < 0.0)
            north_up_angle += (float)180.0;
         else if (delta_x < 0.0)
            north_up_angle += (float)360.0;

         return north_up_angle;
      }
   }

   // if the heading is unknown, and it can't be calculated, then assume north
   // by default
   return 0.0f; 
}

void C_gps_trail::set_new_map(MapProj *map, int x, int y)
{
   float point_angle;
   float map_rotation;
   int window_width, window_height;
   int new_center_x, new_center_y;
   double delta_x, delta_y;
   d_geo_t new_center;
   int status;

   // can't recenter map if a drag is active
   if (OVL_get_overlay_manager()->is_drag_active())
      return;

   // if map scale is 1:80 M or World overview, just center on the current
   // position
   if (map->scale() == WORLD || map->scale() == ONE_TO_80M)
   {
      CView *view = UTL_get_active_non_printing_view();
      status = UTL_change_view_map_center
      (
         view,
         m_current_point.get_latitude(), 
         m_current_point.get_longitude()
      );
     
      if (status == SUCCESS)
      {
         invalidate_all(FALSE);
      }
      else
      {
         // get the best map
         
         CView *view= UTL_get_active_non_printing_view();
         status = UTL_change_view_map_to_best
         (
            view,
            m_current_point.get_latitude(), 
            m_current_point.get_longitude()
         );

         if (status == SUCCESS)
         {
            invalidate_all(FALSE);
         }
      }

      return;
   }

   // get current map rotation
   map_rotation = (float)map->actual_rotation();

   // get the current convergence angle based on the current position
   // and map projection
   double convergence_angle;
   d_geo_t location = 
   { m_current_point.get_latitude(), m_current_point.get_longitude() };
   map->get_meridian_covergence(location, &convergence_angle);

   // get window dimensions
   map->get_surface_size(&window_width, &window_height);

   // compute angle relative to map rotation
   point_angle = get_current_heading(m_tracking_center_num) + map_rotation + (float)convergence_angle;
   if (point_angle >= 360.0)
      point_angle -= (float)360.0;

   if (is_autorotating())
   {
      // add relative heading to map rotation to get new map rotation
      map_rotation -= point_angle;
      if (map_rotation < 0.0)
         map_rotation += (float)360.0;
      
      // force rotation angle to zero when near zero
      if (map_rotation < 0.1)
         map_rotation = (float)0.0;
      
      double d_X, d_Y;
      if (m_TcontinuousCentering)
      {
         d_X = m_rotation_frac_pos_x * (double)window_width;
         d_Y = m_rotation_frac_pos_y * (double)window_height;
      }
      // if we are not in smooth scrolling mode then we need to 
      // use preset values until the apron calculation is reworked
      // to support locations other than 0.5*window_width, 5/6*window_height)
      else
      {
         d_X = 0;
         d_Y = 0.333333*(double)window_height;
      }
      
      // current point to new map center offset
      delta_x = d_X*cos(DEG_TO_RAD(point_angle)) + 
         d_Y * sin(DEG_TO_RAD(point_angle));
      delta_y = d_X*sin(DEG_TO_RAD(point_angle)) + 
         -d_Y * cos(DEG_TO_RAD(point_angle));
   }
   else
   {
      if (get_toggle('CCEN'))
         get_delta_xy_continuous(window_width, window_height, point_angle,
         delta_x, delta_y);
      else 
         get_delta_xy_discrete(window_width, window_height, point_angle,
         delta_x, delta_y);
   }

   // compute map center (x,y) from current point to center offset 
   if (delta_x >= 0.0)
      new_center_x = x + (int)(delta_x + 0.5);
   else
      new_center_x = x + (int)(delta_x - 0.5);
   if (delta_y >= 0.0)
      new_center_y = y + (int)(delta_y + 0.5);
   else
      new_center_y = y + (int)(delta_y - 0.5);

   // get new center point
   map->surface_to_geo(new_center_x, new_center_y, 
      &new_center.lat, &new_center.lon);

   // set clockwise rotation in range 0.0 to 360.0

   CView *view = UTL_get_active_non_printing_view();

   if (map->actual_rotation() == map_rotation)
      status = UTL_change_view_map_center(view, new_center.lat, new_center.lon);
   else
      status = UTL_change_view_map_rotation(view, map_rotation, new_center.lat, new_center.lon);

   if (status == SUCCESS)
   {
      invalidate_all(FALSE);
   }
   else
   {
      // get the best map

      CView *view = UTL_get_active_non_printing_view();
      status = UTL_change_view_map_to_best
      (
         view,
         new_center.lat,
         new_center.lon
      );

      if (status == SUCCESS)
      {
         invalidate_all(FALSE);
      }
   }
}

void C_gps_trail::get_delta_xy_discrete(int window_width, int window_height,
                                          double point_angle,
                                          double &delta_x, double &delta_y)
{
   int i,j;
   int f = 1;
   
   // Use the relative point heading to determine which of the
   // 9 perimeter boxes the current point should be placed in
   // on the new map (the window is divided up into a 3 x 3
   // grid of bounding boxes).  The box is choosen to maximize
   // the amount of map in front of the current point icon.
   // The new map center will place the current point icon at
   // the center of the box (i,j).
   if (point_angle != 90.0 || point_angle != 270.0)
   {
      double point_angle_rad;
      double w_to_h;
      double mid_angle;
      
      point_angle_rad = DEG_TO_RAD(point_angle);
      w_to_h = (double)window_width/(double)window_height;
      
      // 0 <= point_angle < 90.0
      // 0 <= tan(point_angle_rad) < infinity
      if (point_angle < 90.0)
      {
         mid_angle = atan(w_to_h);
         if (point_angle_rad <= mid_angle)
         {
            i = (int)(1.0 - tan(point_angle_rad) / w_to_h + 0.5);
            j = 2;
            ASSERT(i==0 || i==1);
         }
         else
         {
            i = 0;
            j = (int)(1.0 + w_to_h / tan(point_angle_rad) + 0.5);
            ASSERT(j==2 || j==1);
         }  
      }
      // 90.0 < point_angle < 180.0
      // -infinity < tan(point_angle_rad) < 0
      else if (point_angle < 180.0)
      {
         mid_angle = PI - atan(w_to_h);
         if (point_angle_rad <= mid_angle)
         {
            i = 0;
            j = (int)(1.0 - w_to_h / tan(point_angle_rad) + 0.5);
            f = -1;
            ASSERT(j==2 || j==1);
         }
         else
         {
            i = (int)(1.0 + tan(point_angle_rad) / w_to_h + 0.5);
            j = 0;
            ASSERT(i==0 || i==1);
         }
      }
      // 180.0 <= point_angle < 270.0
      // 0 <= tan(point_angle_rad) < infinity
      else if (point_angle < 270.0)
      {
         mid_angle = PI + atan(w_to_h);
         if (point_angle_rad <= mid_angle)
         {
            i = (int)(1.0 + tan(point_angle_rad) / w_to_h + 0.5);
            j = 0;
            ASSERT(i==1 || i==2);
         }
         else
         {
            i = 2;
            j = (int)(1.0 - w_to_h / tan(point_angle_rad) + 0.5);
            f = -1;
            ASSERT(j==1 || j==0);
         }
      }
      // 270.0 <= point_angle < 360.0
      // -infinity < tan(point_angle_rad) < 0
      else
      {
         mid_angle = TWO_PI - atan(w_to_h);
         if (point_angle_rad <= mid_angle)
         {
            i = 2;
            j = (int)(1.0 + w_to_h / tan(point_angle_rad) + 0.5);
            f = -1;
            ASSERT(j==1 || j==0);
         }
         else
         {
            i = (int)(1.0 - tan(point_angle_rad) / w_to_h + 0.5);
            j = 2;
            ASSERT(i==1 || i==2);
         }
      }
   }
   else
   {
      if (point_angle == 90.0)
      {
         i = 0;
         j = 1;
      }
      else
      {
         i = 2;
         j = 1;
      }
   }
   
   // current point to new map center offset
   delta_x = (double)window_width * (0.5 - (double)(2 * i + 1) / 6.0);
   delta_y = f * (double)window_height * (0.5 - (double)(2 * j + 1) / 6.0);
}

void C_gps_trail::get_delta_xy_continuous(int window_width, int window_height,
                                          double point_angle,
                                          double &delta_x, double &delta_y)
{
   double i,j;
   int f = 1;
   
   // Use the relative point heading to determine which of the
   // 9 perimeter boxes the current point should be placed in
   // on the new map (the window is divided up into a 3 x 3
   // grid of bounding boxes).  The box is choosen to maximize
   // the amount of map in front of the current point icon.
   // The new map center will place the current point icon at
   // the center of the box (i,j).
   if (point_angle != 90.0 || point_angle != 270.0)
   {
      double point_angle_rad;
      double w_to_h;
      double mid_angle;
      
      point_angle_rad = DEG_TO_RAD(point_angle);
      w_to_h = (double)window_width/(double)window_height;
      
      // 0 <= point_angle < 90.0
      // 0 <= tan(point_angle_rad) < infinity
      if (point_angle < 90.0)
      {
         mid_angle = atan(w_to_h);
         if (point_angle_rad <= mid_angle)
         {
            i = (1.0 - tan(point_angle_rad) / w_to_h + 0.5);
            j = 2;
         }
         else
         {
            i = 0;
            j = (1.0 + w_to_h / tan(point_angle_rad) + 0.5);
         }  
      }
      // 90.0 < point_angle < 180.0
      // -infinity < tan(point_angle_rad) < 0
      else if (point_angle < 180.0)
      {
         mid_angle = PI - atan(w_to_h);
         if (point_angle_rad <= mid_angle)
         {
            i = 0;
            j = (1.0 - w_to_h / tan(point_angle_rad) + 0.5);
            f = -1;
         }
         else
         {
            i = (1.0 + tan(point_angle_rad) / w_to_h + 0.5);
            j = 0;
         }
      }
      // 180.0 <= point_angle < 270.0
      // 0 <= tan(point_angle_rad) < infinity
      else if (point_angle < 270.0)
      {
         mid_angle = PI + atan(w_to_h);
         if (point_angle_rad <= mid_angle)
         {
            i = (1.0 + tan(point_angle_rad) / w_to_h + 0.5);
            j = 0;
         }
         else
         {
            i = 2;
            j = (1.0 - w_to_h / tan(point_angle_rad) + 0.5);
            f = -1;
         }
      }
      // 270.0 <= point_angle < 360.0
      // -infinity < tan(point_angle_rad) < 0
      else
      {
         mid_angle = TWO_PI - atan(w_to_h);
         if (point_angle_rad <= mid_angle)
         {
            i = 2;
            j = (1.0 + w_to_h / tan(point_angle_rad) + 0.5);
            f = -1;
         }
         else
         {
            i = (1.0 - tan(point_angle_rad) / w_to_h + 0.5);
            j = 2;
         }
      }
   }
   else
   {
      if (point_angle == 90.0)
      {
         i = 0.0;
         j = 1.0;
      }
      else
      {
         i = 2.0;
         j = 1.0;
      }
   }
   
   // current point to new map center offset
   delta_x = (double)window_width * (0.5 - (double)(2 * i + 1) / 6.0);
   delta_y = f * (double)window_height * (0.5 - (double)(2 * j + 1) / 6.0);
}

void C_gps_trail::set_scrolling_track( double nx, double ny, double angle )
{
   //angle must be from -180 to 180
   if (angle > 180) angle = 180;
   if (angle <-180) angle =-180;

   m_scroll_track_anchor_angle = angle;

   //relative placement must be normalized between 0 and 1
   if (nx < 0) nx=0;
   if (nx > 1) nx=1;
   if (ny < 0) ny=0;
   if (ny > 1) ny=1;

   m_scroll_track_anchor_x = nx;
   m_scroll_track_anchor_y = ny;
}

void C_gps_trail::set_scrolling_track( CPoint p0, CPoint p1 )
{
   //ANGLE
   // -180 bottom of screen
   //  -90 left of screen
   //    0 top of screen
   //   90 right of screen
   //  180 bottom of screen
   double angle=0;
   double dx = p1.x - p0.x;
   double dy = p1.y - p0.y;
   double radians = atan2(dy,dx);
   int degrees = (int)RAD_TO_DEG(radians) + 90;
   if (degrees < 0)
      degrees += 360;
   degrees %= 360;
   if (degrees > 180)
      degrees -= 180;
   angle=degrees;

   //PLACEMENT
   CRect crect;
   AfxGetMainWnd()->GetClientRect(&crect);
   CSize csize = crect.Size();
   double rx = (double)p0.x / (double)csize.cx;
   double ry = (double)p0.y / (double)csize.cy;

   set_scrolling_track( rx, ry, angle );
}

void C_gps_trail::orient_map_to_scrolling_track(ActiveMap *map)
{
   orient_map_to_scrolling_track(map, m_scroll_track_anchor_x,
      m_scroll_track_anchor_y, m_scroll_track_anchor_angle);
}

/*
   The parameters are given in view coordinates.
   This routine will orient the current view map such that
   the plane will be at shipPoint and the current track of the 
   plane based on true heading will be coincident with the
   screen line from shipPoint to trackPoint
*/
void C_gps_trail::orient_map_to_scrolling_track(ActiveMap *map, double x, 
                                                double y, double angle )
{
   int status;
/*
   //move ship to the center of the screen
//   status = MAP_set_center(m_current_point.m_latitude,m_current_point.m_longitude);
   status = MAP_set_center(0,0);

   //did it work
   if (status != SUCCESS)
      return;
*/
   // rotate according to current settings
   double heading = get_current_heading(m_tracking_center_num);

   // adjust to the scroll track angle
   double map_angle = angle - heading;
   if (map_angle < 0)
      map_angle += 360;

   // rotate the map
   CView *view = UTL_get_active_non_printing_view();

   status = UTL_change_view_map(view, map->source(), map->scale(), 
      map->series(), m_current_point.get_latitude(), m_current_point.get_longitude(), 
      (double)map_angle, map->actual_zoom_percent(), map->projection_type());

   if (status != SUCCESS)
      return;

   //m_north_up_angle = (float)map_angle;

   // get the screen space dimensions
   int screen_width, screen_height;
   map->get_surface_size(&screen_width, &screen_height);

   // calc the screen anchor point where the ship should be stationary
   int anchor_x = (int)(x * screen_width);
   int anchor_y = (int)(y * screen_height);

   //transform the anchor to a lat lon
   degrees_t lat, lon;
   map->surface_to_geo(anchor_x, anchor_y, &lat, &lon);

   //move the map back to the anchor spot
//   status = MAP_set_rotation(map_angle,lat,lon);
//   status = MAP_set_center(lat,lon);

   if (status != SUCCESS)
      return;

   //all is well
   invalidate_all();

}

// Causes the entire map window to be redrawn completely if no overlay is
// being dragged when it is called.  It will do nothing otherwise.
void C_gps_trail::invalidate_all(boolean_t erase)
{
   // cause the new_draw member to get called to draw from scratch
   invalidate();

   // since the trail is being drawn from scratch this region can be
   // cleared
   m_invalidate_rect.SetRectEmpty();

   // cause the display to get drawn
   OVL_get_overlay_manager()->invalidate_all(erase);

   GetOvlElementContainer()->invalidate(erase);
}

// If an overlay is being dragged when this function is called it will add
// the given rectangle to an internal rectangle and return.  If no overlay
// is being dragged it will invalidate the saved invalidate rectangle plus
// the given rectangle.
void C_gps_trail::invalidate_rect(CRect &rect)
{
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   // skip invalidate if drag is active
   if (ovl_mgr->is_drag_active())
   {
      if (m_invalidate_rect.IsRectEmpty())
         m_invalidate_rect = rect;
      else
         m_invalidate_rect |= rect;

      return;
   }

   // if previous rectangles were put in m_invalidate_rect because a
   // drag was active, then the union of those rects will be invalidated
   if (!m_invalidate_rect.IsRectEmpty())
   {
      ovl_mgr->invalidate_rect(m_invalidate_rect, FALSE);
      m_invalidate_rect.SetRectEmpty();
   }

   // invalidate the given rectangle
   if (!rect.IsRectEmpty())
      ovl_mgr->invalidate_rect(rect, FALSE);
}

void C_gps_trail::midnight_rollover_check( GPSPointIcon& prev, GPSPointIcon& next )
{
   /*
   The time value in next
   should ALWAYS be greater than or equal to 
   the time value in prev.

   Because of the COleDateTime code in GPSPointIcon,
   we should always have a valid default date according to COleDateTime
   this will typically be 1899-DEC-30 midnight

   Our valid_date() function will return FALSE for all dates before
   GPS_VALID_BASE_DATE which is somewhere before 1980 and it will
   return TRUE for all dates beyond this mark.

   We have 2 dates - prev and next
   their dates can only be valid and invalid
   which is to say present in the data or not present in the data

   So there can only be a combination of 4 date states
   for prev and next:

   prev     next        action
   ---------------------------------------------------------------
   valid    valid     - copy prev date to next date & inc forward
   valid    invalid   - copy prev date to next date & inc forward
   invalid  valid     - this case will be trivially rejected
   invalid  invalid   - copy prev date to next date & inc forward
   */

   //We !MUST! have be valid COleDateTime values
   if (prev.m_dateTime.GetStatus()!=COleDateTime::valid)
      return;
   if (next.m_dateTime.GetStatus()!=COleDateTime::valid)
      return;

   //there's nothing to do if the date's are in the correct order
   if (prev.m_dateTime <= next.m_dateTime)
      return;

   /*
   ok prev point time is greater than next point time
   this implies rollover at midnight
   the only other possibility is a corrupt data stream
   we are not attempting to handle that situation here
   */

   //get the next point time
   float nextTime = next.get_time();

   //copy prev date/time to next date/time
   next.m_dateTime = prev.m_dateTime;

   //restore the original time value
   next.set_time(nextTime);

   //this should iterate one time only   
   while (prev.m_dateTime > next.m_dateTime)
      next.m_dateTime += COleDateTimeSpan(1);

}

/*
8888888888888888888888888888888888888888888888
   THE DISPLAY LIST ROUTINES

   THE NEW AND IMPROVED GPS CODE
8888888888888888888888888888888888888888888888
*/
int C_gps_trail::new_draw(ActiveMap* map)
{
   // handle any mapscale changes
   handle_mapscale_changes(map);

   // handle any mapmovements or rotations
   handle_mapproj_changes();

   // create the visible list
   display_list_calc(map);

   // draw the visible list
   display_list_draw(map);

   // draw the plane
   ship_draw(map);

   // set the new apron
   auto_center_bounding_box_calc(map);
   
   // valid
   set_valid(TRUE);

   // all is well
   return SUCCESS;
}

int C_gps_trail::redraw(ActiveMap* map)
{
   //get the pDC
   CDC* pDC = map->get_CDC();

   // get clipping rectangle
   CRect clip_rect;
   int box_status = pDC->GetClipBox(&clip_rect);

   //draw the visible list
   display_list_draw( map, clip_rect );

   //draw the plane
   ship_draw(map, clip_rect);

   //valid
   set_valid(true);
   
   //all is well
   return SUCCESS;
}

void C_gps_trail::display_list_add( GPSPointIcon *point )
{
   m_display_list.push_back(point);
}

void C_gps_trail::display_list_reset()
{
   m_display_list.erase(m_display_list.begin(), m_display_list.end());
}

int C_gps_trail::point_transform( ActiveMap *map, GPSPointIcon *next, int& x, int& y)
{
   //MAP BOUNDS
   d_geo_t ll, ur;
   if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
      return -1;

   //TIME TEST
   if (is_in_the_future(next))
      return -2;
   //if (!is_in_past(next_point))
   //   continue -3;

   //MAP TEST
   if
   (
      !GEO_in_bounds
      (
         ll.lat,
         ll.lon,
         ur.lat,
         ur.lon,
         (degrees_t)next->get_latitude(), 
         (degrees_t)next->get_longitude()
      )
   )
      return -4;

   //VIEW TEST
   map->geo_to_surface
   (
      (degrees_t)next->get_latitude(),
      (degrees_t)next->get_longitude(),
      &x,
      &y
   );

   if (!map->point_in_surface(x, y))
      return -5;

   //BOUNDING BOX
   next->m_rect.SetRect(x-m_radius, y-m_radius, x+m_radius, y+m_radius);

   //PASSED!!!
   return (0);
}

void C_gps_trail::display_list_calc( ActiveMap *map )
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   //RESET
   display_list_reset();

   //BUTTON TEST
   if (!display_trail_points())
      return;

   // determine which points will be shown for this mapscale 
   set_viewable_points(map);
   m_display_list_valid = FALSE;

   // get the map bounds
   d_geo_t ll, ur;
   if (map->get_vmap_bounds(&ll, &ur) != SUCCESS)
      return;

   // get the icon type from the properties object
   gps_icon_t icon_type = m_properties.get_trail_icon_type();

   //get the first point
   POSITION p;
   GPSPointIcon *prev = NULL;
   GPSPointIcon *next = NULL;
   GPSPointIcon *last_set = NULL;

   //WALK THE LIST
   for(next = prev = get_icon_list()->get_first(p); next;
   (p) ? next=get_icon_list()->get_next(p) : next=NULL)
   {
      //PROXIMITY TEST
      if (!next->m_viewable)
         continue;
      
      //TIME TEST
      next->m_show_point = !is_in_the_future(next);
      
      //MAP TEST
      if (icon_type == GPS_ICON_ARROW && last_set != NULL)
      {
         d_geo_t g1, g2;
         
         g1.lat = next->get_latitude();
         g1.lon = next->get_longitude();
         g2.lat = last_set->get_latitude();
         g2.lon = last_set->get_longitude();
         
         if (!GEO_intersect(__min(g1.lat, g2.lat), __min(g1.lon, g2.lon), 
            __max(g1.lat, g2.lat), __max(g1.lon, g2.lon),
            ll.lat, ll.lon, ur.lat, ur.lon))
         {
            next->m_rect.SetRect(-1,-1,-1,-1);
            last_set = next;
            continue;
         }
      }
      else
      {
         if (!GEO_in_bounds(ll.lat - 100*m_deg_per_pixel.lat,
            ll.lon - 100*m_deg_per_pixel.lon, ur.lat + 100*m_deg_per_pixel.lat,
            ur.lon + 100*m_deg_per_pixel.lon, next->get_latitude(), 
            next->get_longitude()) && map->scale() != WORLD)
         {
            next->m_rect.SetRect(-1, -1, -1, -1);
            last_set = next;
            continue;
         }
      }
      
      //VIEW TEST
      int x,y;
      map->geo_to_surface(next->get_latitude(), next->get_longitude(), &x, &y);
      
      //PASSED!!!
      
      // set trail icon's bounding box
      next->m_x = x;
      next->m_y = y;
      
      if (icon_type == GPS_ICON_ARROW && last_set != NULL)
      {
         int distance;
         
         // get the center of the previous point
         int prev_x, prev_y;
         map->geo_to_surface(last_set->get_latitude(), last_set->get_longitude(), 
            &prev_x, &prev_y);
         CPoint prev_center = CPoint(prev_x, prev_y);

         // get the center of this point
         CPoint center = CPoint(x, y);
         
         // calculate the distance between these points
         distance = futil->point_distance(center, prev_center);
         
         // now the line should be drawn proportional to this distance.  
         // Calculate the starting x,y values for the arrow.
         double k = (2.0/3.0);
         double radians = atan2((double)(center.y - prev_center.y), (double)(center.x - prev_center.x));
         next->m_arrow_start.x = center.x - (int)(k*distance*cos(radians));
         next->m_arrow_start.y = center.y - (int)(k*distance*sin(radians));
         
         int x1 = center.x;
         int x2 = next->m_arrow_start.x;
         int y1 = center.y;
         int y2 = next->m_arrow_start.y;
         
         next->m_rect.SetRect(__min(x1,x2), min(y1,y2), __max(x1,x2),
            __max(y1,y2));
         next->m_rect.InflateRect(8,8);
      }
      else
      {
         next->m_rect.SetRect(x-m_radius, y-m_radius, x+m_radius, y+m_radius);
      }
      
      // add this point to the display list
      next->m_prev = last_set;

      display_list_add(next);
      
      last_set = next;
      prev=next;
   }

   //the list is good
   m_display_list_valid=true;
}

void C_gps_trail::display_list_draw( ActiveMap *map, CRect clip )
{
   CDC* pDC = map->get_CDC();

   //trail point on/off?
   if (!display_trail_points())
      return;

   // if we were passed in a valid clip rect then we will 
   // perform clipping
   boolean_t clipping = (clip != CRect(-1,-1,-1,-1));

   // need to have something to clip against if we are clipping
   if (clipping && clip.IsRectEmpty())
      return;

   // if the GPS Info box is no longer up then we need to unselect
   // the selected point if one exists
   if ((!get_info_box_up() && !m_edit_on && !m_comment_dlg_up) && m_selected_point)
   {
      invalidate_object(m_selected_point, TRUE);
      m_selected_point->set_selected(FALSE);
      m_selected_point = NULL;
      m_shipSymbol->set_selected(FALSE);

      // recalc the display list
      display_list_calc(map);
   }

   // get the current convergence angle based on the current position
   // and map projection
   double convergence_angle;
   d_geo_t location = 
   { m_current_point.get_latitude(), m_current_point.get_longitude() };
   map->get_meridian_covergence(location, &convergence_angle);

   //tell the point module about the map rotation
   GPSPointIcon::m_map_rotation = map->actual_rotation() + convergence_angle;

   // setup the links icon
   HICON hLinkIcon = AfxGetApp()->LoadIcon(IDI_LINK);

   //draw every point inside the clip rect
   const size_t nSize = m_display_list.size();
   for(size_t i=0; i<nSize; i++)
   {  
      GPSPointIcon *pGpsPointIcon = m_display_list[i];

      if (pGpsPointIcon != NULL)
      {
         boolean_t draw_point = pGpsPointIcon->m_show_point;

         // we may need to turn off this point if a time out
         // has been set and the point lies before the current time
         // minus the timeout
         if (m_properties.get_timeout_on() && m_properties.get_timeout_seconds() > 0)
         {
            COleDateTimeSpan trail_timeout(0,0,0,m_properties.get_timeout_seconds());
            
            COleDateTime time = m_current_view_time - trail_timeout;
            if (pGpsPointIcon->m_dateTime < time)
               draw_point = FALSE;
         }
         
         if (clipping)
         {
            // intersect the point's bounding rect with the clip rect
            CRect exposed = pGpsPointIcon->m_rect;
            exposed &= clip;
            
            // if these two rects do not intersect then there is no need to
            // draw the point
            draw_point &= !exposed.IsRectEmpty();
         }

         if (draw_point)
         {
            pGpsPointIcon->draw( map, pDC, m_properties.get_trail_icon_type(),
               m_properties.get_trail_icon_fg_color());  

            if (pGpsPointIcon->GetLinksCount() > 0)
               pDC->DrawIcon(pGpsPointIcon->m_x, pGpsPointIcon->m_y, hLinkIcon);
         }
      }
   }
}

// Draw predictive pointts in 2D
void C_gps_trail::draw_predictive_points(ActiveMap *map)
{
   CPredictivePathProperties* pPredictivePathProp = m_properties.GetPredictivePathProperties();

   if (!pPredictivePathProp->get_predictive_path_on())
      return;

   // don't draw predictive points if the trail is not receiving any new points
   if (is_frozen())
      return;

   CPredictivePathRenderer::RenderCenterLine(map, map->get_CDC(), m_pPredictivePath, pPredictivePathProp);
   CPredictivePathRenderer::RenderTrendDots(map, map->get_CDC(), m_pPredictivePath, pPredictivePathProp);
}

// Update predictive points in 3D scene
void C_gps_trail::update_predictive_points()
{
   CPredictivePathProperties* pPredictivePathProp = m_properties.GetPredictivePathProperties();

   // If on and receiving new points
   if ( !pPredictivePathProp->get_predictive_path_on() || is_frozen() )
      m_aPredictivePoints3D.clear();
   else
   {
      // Draw centerline




      // For each trend dot
      size_t cTrendDots = pPredictivePathProp->get_num_trend_dots();
      if ( cTrendDots != m_aPredictivePoints3D.size() )
      {
         m_aPredictivePoints3D.clear();   // Make a clean start
         m_aPredictivePoints3D.resize( cTrendDots );
      }

      for ( size_t i = 0; i < cTrendDots; ++i )
      {
         // Find the relative predictive path point
         if ( m_pPredictivePath->MoveTo( pPredictivePathProp->get_trend_dot_time( i ) - 1 ) )
         {
            ATL::CCritSecLock lock( s_idThreadInterlock );
            GPS3DPoint& ppt = m_aPredictivePoints3D[ i ];
            ppt.m_pGPSTrail = this;
            ppt.m_ePointType = GPS3DPoint::GPSPT_NORM_PATH;
            ppt.m_gptLatLon.lat = m_pPredictivePath->get_m_Latitude();
            ppt.m_gptLatLon.lon = m_pPredictivePath->get_m_Longitude();
            ppt.m_dAltMeters = m_pPredictivePath->get_AltMeters();
            ppt.m_dHeadingDeg = m_pPredictivePath->get_HeadingDeg();
            ppt.m_dSpeedFtPerSec = m_pPredictivePath->get_SpeedFtPerSec();
            Update3DPoint( &ppt );
         }
      }  // Trend dot loop
   }

}  // draw_predictive_points()


C_icon *C_gps_trail::hit_test(map_projection_utils::CMapProjWrapper *view, CPoint point)
{
   // check to see if we hit a coast track icon.  If so, return
   // the pointer to the Coast Track icon
   if (m_coast_track_mgr->hit_test(point))
   {
      return (C_icon *)m_coast_track_mgr->hit_test(view, point);
   }

   // check to see if we hit a range and bearing object.  Return the
   // pointer to the object if this is the case
   if (m_rb_mgr->hit_test(point))
   {
      return (C_icon *)m_rb_mgr->hit_test(view, point);
   }

   //current point
   if (m_current_point.hit_test(point))
   {
      m_last_hit = &m_current_point;
      return &m_current_point;
   }

   const size_t nSize = m_display_list.size();

   // [Bug 4770] give points with comments first shot at the hit test
   for (size_t i=0; i<nSize; i++)
   {
      if (m_display_list[i] != NULL && m_display_list[i]->m_show_point && 
         m_display_list[i]->has_comment())
      {
         if (m_display_list[i]->hit_test(point))
         {
            m_last_hit = m_display_list[i];
            return m_display_list[i];
         }
      }
   }

   // all the other displayed points
   for (size_t i=0; i<nSize;i++)
   {
      if (m_display_list[i] != NULL && m_display_list[i]->m_show_point &&
         !m_display_list[i]->has_comment())
      {
         if (m_display_list[i]->hit_test(point))
         {
            m_last_hit = m_display_list[i];
            return m_display_list[i];
         }
      }
   }
   /*
   if(GetOvlElementContainer()->hit_test(point))
   {
   return (C_icon *)GetOvlElementContainer()->hit_test(point);
   }*/

   return NULL;
}

void C_gps_trail::auto_center_bounding_box_calc( ActiveMap *map )
{
   if (!is_autocentering())
      return;
   
   if (m_TcontinuousCentering)
   {
      m_auto_center_rect.SetRectEmpty();
      return;
   }
   
   // set auto-center bounding box
   if (is_autocentering())
   {
      int window_width, window_height;
      int ul_x, ul_y;
      int box_width, box_height;
      
      map->get_surface_size(&window_width, &window_height);
      
      // Outter box is from (W/10, H/10) to (9W/10, 9H/10).  Inner box is a 
      // W/5 x H/2 box centered at (W/2, 5H/6).  The auto-center box is the
      // intersection, or (2W/5, H/2) to (3W/5, 9H/10).
       if (is_autorotating())
      {
         box_width = window_width / 5;
         box_height = 2 * window_height / 5;
         ul_x = 2 * box_width;
         ul_y = window_height / 2;
         
         m_auto_center_rect.SetRect(ul_x, ul_y,
            ul_x + box_width + 1, ul_y + box_height + 1);
      }
      // Outter box is from (W/10, H/10) to (9W/10, 9H/10).  Inner box size and
      // location are a function of the position of the current position icon.
      // The auto-center box is the intersection.
      else
      {
         // set to outter box
         box_width = 4 * window_width / 5;
         box_height = 4 * window_height / 5; 
         ul_x = (window_width - box_width)/2;
         ul_y = (window_height - box_height)/2;
         m_auto_center_rect.SetRect(ul_x, ul_y,
            ul_x + box_width + 1, ul_y + box_height + 1);
         
         // use the location of the current position icon to define the inner
         // rectangle only if it is in the outter box
         CPoint center = m_current_point.m_rect.CenterPoint();
         if (m_auto_center_rect.PtInRect(center))
         {
            int index;
            
            // boxes centered at W/2 are W/4 wide
            index = 3 * center.x / window_width;
            if (index == 1)
            {
               box_width = window_width / 4;
               ul_x = 3 * window_width / 8;
            }
            // boxes centered at W/4 or 3W/4 are W/2 wide
            else
            {
               box_width = window_width / 2;
               ul_x = index * window_width / 4;
            }
            
            // boxes centered at H/2 are H/4 high
            index = 3 * center.y / window_height;
            if (index == 1)
            {
               box_height = window_height / 4;
               ul_y = 3 * window_height / 8;
            }
            // boxes centered at H/4 or 3H/4 are H/2 high
            else
            {
               box_height = window_height / 2;
               ul_y = index * window_height / 4;
            }            
            
            CRect inner_rect(ul_x, ul_y, 
               ul_x + box_width + 1, ul_y + box_height + 1);
            
            // set auto-center rect to the intersection of the outter and inner rectangles
            m_auto_center_rect &= inner_rect;
         }
      }
   }
   
#if DRAW_RECENTER_RECT
   // draw bounding box
   CDC *dc = map->get_CDC();
   UtilDraw util(dc);

   util.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, 3);
   util.draw_rectangle(FALSE, m_auto_center_rect.left,
      m_auto_center_rect.top, m_auto_center_rect.right - 1,
      m_auto_center_rect.bottom - 1);
   m_rect |= m_auto_center_rect;
#endif
}

void C_gps_trail::selected_draw( CDC *pDC )
{
   // gotta have something to draw
   if (m_selected_point == NULL)
      return;
   
   // we've already drawn the ship if it's selected
   if (m_selected_point == &m_current_point)
      return;
   
   // get the rectangle for the point
   CRect rect = m_selected_point->m_rect;
   
   // if the point is off the screen
   if (rect == CRect(-1,-1,-1,-1))
      return;
   
   // enlarge to our standard select size if necessary
   gps_icon_t icon_type = m_properties.get_trail_icon_type();

   // if we are not dealing with the arrow trail style then it
   // is not necessary to inflate the rect since this was already
   // included in the arrow's bounding box calculation
   if (icon_type != GPS_ICON_ARROW)
      rect.InflateRect( POINT_SELECTED_BORDER, POINT_SELECTED_BORDER );
   
   // draw info box focus rectangle
   GRA_draw_hilight_rect(pDC, rect);
}

void C_gps_trail::remarkdlg_update()
{
   //should we close the remark display
   if (m_show_communications)
   if (this == dynamic_cast<C_gps_trail *>(CRemarkDisplay::get_focus_overlay()))
      CRemarkDisplay::close_dlg();
}

void C_gps_trail::cycle_tracking_center_num()
{
   m_tracking_center_num++;
   map_update(UTL_get_current_view_map(), TRUE);
}

void C_gps_trail::map_update(MapProj* map, boolean_t force_update /*=FALSE*/)
{
   //TRIVIAL REJECTIONS
   if (!is_autocentering())
      return;

   if (!GEO_valid_degrees(m_current_point.get_latitude(), m_current_point.get_longitude()))
      return;

   //TRANSFORM CURRENT POINT
   int x, y;
   double lat, lon;
   m_current_point.get_center(m_tracking_center_num, lat, lon);
   map->geo_to_surface(lat, lon, &x, &y);

   //SCROLLING TRACK
   //if (get_toggle('CCEN'))
   //{
   //   if (is_autorotating())
   //      orient_map_to_scrolling_track(map);
   //   else
   //      orient_map_to_scrolling_track( 0.5, 0.5, 0 );
   //      auto_center_and_rotate(map);
   //   return;
   //}

   //APRON BOUNDS
   boolean_t inApron = m_auto_center_rect.PtInRect(CPoint(x,y));

   //VIEW BOUNDS
   boolean_t inView = map->geo_in_surface((degrees_t)lat, (degrees_t)lon);

   //RECENTER CHECK
   if (!(inView && inApron) || get_toggle('CCEN') || force_update)
   {
      // If auto-center is turned on, center the map and return.
      // If auto-rotate is also turned on, then the rotation will also be set.
      // This function returns TRUE if auto-center is turned on, and FALSE otherwise.
//      auto_center_and_rotate();
      set_valid(FALSE);
      set_new_map(map,x,y);
   }
}

void C_gps_trail::on_new_end_point( GPSPointIcon *end_point )
{
   //default to no point
   m_new_end_point = NULL;

   //gotta be a clean point to make the cut
   if (!end_point->is_valid())
      return;

   //create a new point, the first point in the trail
   m_new_end_point = new GPSPointIcon(this, CRect(0,0,0,0) );

   //gotta have a point
   if (m_new_end_point == NULL)
      return;

   // copy the data from m_current_point into this point
   *m_new_end_point = *end_point;

   //it's invisible 
   m_new_end_point->m_rect.SetRectEmpty();

   // add point to icon list
   get_icon_list()->add_point( m_new_end_point, this );

   // since we have successfully added a new point, we can close the 
   // GPS communications dialog if it is opened
   remarkdlg_update();

   // this overlay is now dirty since a point has been added
   if (m_gps_options != 0)  // if not file read mode
   {
      long current_row;
      if (g_tabular_editor_dlg)
      {
         g_tabular_editor_dlg->set_paint_mode(FALSE);
         current_row = g_tabular_editor_dlg->get_top_row();
      }

      set_modified(TRUE);

      // make sure we set the focus in the tabular editor to the point just added
      if (g_tabular_editor_dlg)
      {
         g_tabular_editor_dlg->set_top_row(current_row);
         g_tabular_editor_dlg->set_paint_mode(TRUE);
      }
   }

   // If SkyView is being updated, add the point to the motion playback of the
   // camera platform.
   if (m_update_3d_camera && (s_sampled_motion_playback != NULL))
   {
      try
      {
         s_sampled_motion_playback->AddSample(
            end_point->m_dateTime,
            end_point->get_latitude(),
            end_point->get_longitude(),
            end_point->m_msl,
            end_point->m_true_heading,
            0.0,  // constant pitch
            0.0); // constant bank
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("SampleMotionPlayback::AddSample failed: (%s)", (char *)e.Description());
         ERR_report(msg);
      }
   }
}

//NEW ADD POINT
int C_gps_trail::add_point()
{
   //FILE READ MODE
   if (m_gps_options == 0)
   {
      //ADD A TRAIL POINT RECORD
      on_new_end_point(&m_next_point);
      return SUCCESS;
   }

   // reset the timers used to detect a frozen feed and predictive path since 
   // a point has arrived
   const int overlay_handle = OVL_get_overlay_manager()->get_overlay_handle(this);
   CMainFrame *frame = fvw_get_frame();
   if (frame)
   {
      int timeout = m_properties.get_frozen_feed_timeout();

      if (timeout != -1)
      {
         m_pFrozenTrailTimer->restart();
      }
   }

   m_pPredictivePathTimer->restart();
   m_predictive_path_timer_delta = 0;

   // if a warning is being displayed, stop it now that we have a new point
   stop_frozen_trail_warning();

   //ADD A TRAIL POINT RECORD
   on_new_end_point(&m_next_point);

   //EXIT IF WE'RE BEHIND
   if (!m_listAddPointQueue.IsEmpty())
   {
      // remove all WM_MOVING_MAP_POINT_ADDED messages from message queue
      MSG msg;
      while (::PeekMessage(&msg, NULL, WM_MOVING_MAP_POINT_ADDED, WM_MOVING_MAP_POINT_ADDED, PM_REMOVE))
      {
      }

      while (!m_listAddPointQueue.IsEmpty())
         add_point_from_queue();

      return SUCCESS;
   }

   // if the view time dialog is not opened, then set the current view time 
   // to the time of the last point in the opened trail
   if (!CMainFrame::IsPlaybackDialogActive())
   {
      m_current_view_time = m_icon_list.get_last()->m_dateTime;
   }

   // MOVE THE SHIP IN TIME
   //
   CViewTime& playbackDialog = CMainFrame::GetPlaybackDialog();

   COleDateTime dt( m_next_point.get_date_time() );
   playbackDialog.on_new_absolute_time(m_next_point.get_date_time());

   // Force the playback time to match the current time
   playbackDialog.set_current_time(m_next_point.get_date_time());

   //MAKE THE CHANGE
   if (!m_current_point.is_valid())
      set_current_point(&m_next_point);

   {
      MapView *pView = fvw_get_view();

      if (pView != NULL)
         pView->PostMessage(WM_INVALIDATE_LAYER_OVERLAY, 0, -1); //overlay_handle);
   }

   set_valid(FALSE);

   // notify anybody who is requesting a change in the current point
   notify_clients(m_next_point);

   // update the status bar's help text information pane
   int point_list_length;
   get_point_list_length(&point_list_length);
   if (point_list_length >= 1)
   {
      CMainFrame *pFrame = fvw_get_frame();
      if (pFrame != NULL)
      {
         static CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(pFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));
         if (pTextInfoPane != NULL)
            pTextInfoPane->SetText(m_current_point.get_help_text());
      }
   }

   // ALL IS WELL
   return SUCCESS;
}

// send a registered message to any HWND in the notification list
void C_gps_trail::notify_clients(GPSPointIcon &point)
{
   POSITION position = m_hwnd_current_pos_notify_lst.GetHeadPosition();
   while (position)
   {
      HWND hWnd = m_hwnd_current_pos_notify_lst.GetNext(position);
      BYTE data[11+sizeof(short)+sizeof(double)*6+sizeof(DATE)+sizeof(long)];

      // create the data block
      strncpy_s((char*) &data[0], 11, "FALCONVIEW", 11);  
      ((short&) data[11]) = FV_GPS_CURRENT_PT_MSG;
      ((long &) data[13]) = OVL_get_overlay_manager()->get_overlay_handle(this);
      ((double&) data[17]) = point.get_latitude();
      ((double&) data[25]) = point.get_longitude();
      ((double&) data[33]) = point.m_msl;
      ((double&) data[41]) = point.m_true_heading;
      ((double&) data[49]) = point.m_magnetic_heading;
      ((double&) data[57]) = point.m_speed_knots;
      ((DATE&) data[65]) = point.m_dateTime;

      // current point information will be sent with a WM_COPYDATA message to the window
      // requesting notification
      COPYDATASTRUCT copydata;
      copydata.cbData = sizeof(data);
      copydata.lpData = (void *)&data;

      // make sure the hWnd is a valid window before trying to send 
      // a message to it
      if (!::IsWindow(hWnd))
      {
         ERR_report("Invalid window handle in current position message");
      }
      // otherwise, we can send the message
      else
      {
         DWORD result;

         SendMessageTimeout(hWnd, WM_COPYDATA, (WPARAM)(HWND)AfxGetMainWnd()->m_hWnd,
            (LPARAM)&copydata, SMTO_BLOCK, 5000, &result);
      }
   }
}

void C_gps_trail::set_current_point( GPSPointIcon *point )
{
   // save old current point
   m_previous_point = m_current_point;

   //set the next current point
   m_current_point = *point;

   // fill in missing data in m_current_point that is calculatable
   m_current_point.calc();
}
