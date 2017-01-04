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
// tstmapmv.cpp
//
// Event generation, validation, and execution routines for map-move events
//
// Created 3 Oct 96

#include "stdafx.h"
#include "testglob.h"
#include "geo_tool.h"
#include "utils.h"

void CMapAutoTester::GenMapMove(test_event_t& event)
{
   //// generate random lat-long within constraints
   if(gTestFlags & TEST_CONUS_ONLY)
   {
      // CONUS only (eyeballed these)
      // Upper Left:   N50 W125
      // Lower right:  N25 W68
      event.info.coord.lat = ((double) rand() / (double) RAND_MAX) * 25.0 + 25.0;
      event.info.coord.lon = ((double) rand() / (double) RAND_MAX) * (-57.0) + (-68.0);
   }
   else
   {
      // anywhere in the world
      event.info.coord.lat = ((double) rand() / (double) RAND_MAX) * 180.0 - 90.0;
      event.info.coord.lon = ((double) rand() / (double) RAND_MAX) * 360.0 - 180.0;
   }
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ValMapMove(test_event_t& event)
{
   CView* view = UTL_get_active_non_printing_view();
   const int err = UTL_change_view_map_center(view, event.info.coord.lat, event.info.coord.lon);

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
int CMapAutoTester::ExecMapMove(test_event_t event)
{
   CString buf;
   const int STR_LEN = 256;
   char latLonStr[STR_LEN];
   int err;

   GEO_lat_lon_to_string(event.info.coord.lat, event.info.coord.lon, latLonStr, STR_LEN);

   buf.Format("(%d) Attempting map move to:  %s\n", gCurrentIteration, latLonStr);
   LogEntry(buf, true);

   CView* view = UTL_get_active_non_printing_view();
   err = UTL_change_view_map_center(view, event.info.coord.lat, event.info.coord.lon);

   if (err != SUCCESS)
   {
      // log any errors we get
      LogError(MAP_MOVE, err);

      // don't bomb if it's a puny no-data error
      if (err == FV_NO_DATA)
         return TEST_NON_FATAL_ERROR;
      else
         return TEST_FATAL_ERROR;
   }
   else
   {
      // update our globals
      gCurrentLat = event.info.coord.lat;
      gCurrentLon = event.info.coord.lon;
      gNumMoves++;
   }

   return TEST_NO_ERROR;
}
