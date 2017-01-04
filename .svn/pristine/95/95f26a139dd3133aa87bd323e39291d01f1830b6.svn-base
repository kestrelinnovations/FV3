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
// tstmapsc.cpp
//
// Event generation, validation, and execution routines for map-scale events
//
// Created 3 Oct 96 by D. Pope
//

#include "stdafx.h"
#include "testglob.h"
#include "map.h"
#include "maps.h"
#include "utils.h"


void CMapAutoTester::GenMapScale(test_event_t& event)
{
   if (m_selectedMapTypes.GetSize() > 0)
   {
      int nIndex = rand() % m_selectedMapTypes.GetSize();
      POSITION position = m_selectedMapTypes.FindIndex(nIndex);

      event.info.pMapType = m_selectedMapTypes.GetAt(position);
   }
   else
      event.info.pMapType = NULL;
}

// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ValMapScale(test_event_t& event)
{
   if (event.info.pMapType == NULL)
      return TEST_NON_FATAL_ERROR;

   return TEST_NO_ERROR;
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ExecMapScale(test_event_t event)
{
   CString buf;
   int err;

   MapSource source = event.info.pMapType->get_source();
   MapScale scale = event.info.pMapType->get_scale();
   MapSeries series = event.info.pMapType->get_series();

   buf.Format("(%d) Attempting map scale to: ", gCurrentIteration);
   buf += MAP_get_source_string(source) + " ";
   buf += MAP_get_scale_string(scale);
   if (series.get_string().GetLength())
   {
      buf += " (";
      buf += series.get_string();
      buf += ")";
   }
   buf += "\n";
   LogEntry(buf, true);

   CView* view = UTL_get_active_non_printing_view();
   MapProj* curr = UTL_get_current_view_map(view);

   err = UTL_change_view_map(view, source, scale, series,
                       curr->actual_center_lat(), curr->actual_center_lon(), 
                       curr->requested_rotation(), curr->requested_zoom_percent(),
                       curr->projection_type());

   if (err != SUCCESS)
   {
      // log any errors we get
      LogError(MAP_SCALE, err);

      // don't return if it's a puny no-data error
      if (err == FV_NO_DATA)
         return TEST_NON_FATAL_ERROR;
      else
         return TEST_FATAL_ERROR;
   }
   else
   {
      // update our globals
      gNumScales++;
   }

   return TEST_NO_ERROR;
}
