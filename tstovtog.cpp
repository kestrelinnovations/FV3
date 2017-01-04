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
// tstovtog.cpp
//
// Event generation, validation, and execution routines for overlay-toggle 
// events
//
// Created 3 Oct 96

#include "stdafx.h"
#include "testglob.h"
#include "OvlFctry.h"

static int num_selected_static_overlays;


// Generate a static overlay toggle event
void CMapAutoTester::GenOvlToggle(test_event_t& event)
{
   // get the number of static overlays currently selected for toggling...
   num_selected_static_overlays = gSelectedStaticOvlList.GetSize();
   
   // get random number between 0 and (# of selected static overlays - 1) to
   // serve as an index into the selected static overlay list...
   event.info.ovlNum = rand() % num_selected_static_overlays;
}


// Validate the static overlay toggle event
int CMapAutoTester::ValOvlToggle(test_event_t& event)
{
   // make sure the index into the selected static overlay list is valid
   if (event.info.ovlNum < 0 || event.info.ovlNum >= num_selected_static_overlays)
      return TEST_FATAL_ERROR;

   return SUCCESS;
}


// Execute the static overlay toggle event
int CMapAutoTester::ExecOvlToggle(test_event_t event)
{
   CString class_name, class_title;
   CString buf;

   // get the class name of the static overlay to be toggled
   OverlayTypeDescriptor* pOverlayTypeDesc = gSelectedStaticOvlList.GetAt(event.info.ovlNum);

   // use the overlay manager to get the class title that corresponds to
   // this static overlay class name...
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   // make sure the index into the static overlay list is valid
   if (event.info.ovlNum < 0 || event.info.ovlNum >= gSelectedStaticOvlList.GetSize())
      return TEST_FATAL_ERROR;

   // write change to test log
   C_overlay *pOverlay = OVL_get_overlay_manager()->get_first_of_type(pOverlayTypeDesc->overlayDescriptorGuid);
   CString strDisplayName = pOverlayTypeDesc->displayName;
   buf.Format("(%d) Attempting to toggle the %s static overlay %s\n", gCurrentIteration, strDisplayName,
      pOverlay == NULL ? "on" : "off");
   LogEntry(buf, true);

   // toggle this static overlay
   if (ovl_mgr->toggle_static_overlay(pOverlayTypeDesc->overlayDescriptorGuid) != SUCCESS)
      return TEST_FATAL_ERROR;

   // update globals and counters
   int current_ovl_toggle_count = gSelectedStaticOvlCounters.GetAt(event.info.ovlNum);
   current_ovl_toggle_count++;
   gSelectedStaticOvlCounters.SetAt(event.info.ovlNum, current_ovl_toggle_count);

   gNumOvlToggles++;


   return SUCCESS;
}
