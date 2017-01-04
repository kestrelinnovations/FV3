// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



/////////////////////////////////////////////////////////////////////////////
// tstmapzm.cpp
//
// Event generation, validation, and execution routines for map-zoom events
//
// Created 3 Oct 96 by D. Pope
//

#include "stdafx.h"
#include "testglob.h"
#include "map.h"
#include "utils.h"



void CMapAutoTester::GenMapZoom(test_event_t& event)
{
   // generate random map zoom factor between 10 and 800
   // That is, from 10% (zoom out) to 800% (zoom in)
   event.info.zoomTo = (long)(rand() % 791 + 10);
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ValMapZoom(test_event_t& event)
{
   CView* view = UTL_get_active_non_printing_view();

   MapProj* curr = UTL_get_current_view_map(view);
   const MapSource source = curr->source();
   const MapScale scale = curr->scale();
   const MapSeries series = curr->series();
   const degrees_t center_lat = curr->requested_center_lat();
   const degrees_t center_lon = curr->requested_center_lon();
   const double rot_angle = curr->requested_rotation();

   const ProjectionEnum projection_type = curr->projection_type();
   const int err = UTL_change_view_map(view, source, scale, series,
                              center_lat, center_lon, rot_angle, 
                              event.info.zoomTo, projection_type);

   if (err != SUCCESS)
   {
      if (err == FV_NO_DATA)
         return TEST_NON_FATAL_ERROR;
      else 
         return TEST_FATAL_ERROR;
   }
   else 
      return TEST_NO_ERROR;
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ExecMapZoom(test_event_t event)
{
   CString buf;
   int err;

   buf.Format("(%d) Attempting map zoom to:  %d%%\n", 
            gCurrentIteration, event.info.zoomTo);
   LogEntry(buf, true);

   CView* view = UTL_get_active_non_printing_view();
   const MapProj* curr = UTL_get_current_view_map(view);

   err = UTL_change_view_map(view, 
                       curr->source(), 
                       curr->scale(), 
                       curr->series(), 
                       curr->actual_center_lat(), 
                       curr->actual_center_lon(),
                       curr->requested_rotation(), 
                       curr->requested_zoom_percent(), 
                       curr->projection_type());

   if (err != SUCCESS)
   {
      // log any errors we get
      LogError(MAP_ZOOM, err);

      // don't return if it's a puny no-data error
      if (err == FV_NO_DATA)
         return TEST_NON_FATAL_ERROR;
      else
         return TEST_FATAL_ERROR;
   }
   else
   {
      // update our globals
      gCurrentZoom = event.info.zoomTo;
      gNumZooms++;
   }

   return TEST_NO_ERROR;
}
