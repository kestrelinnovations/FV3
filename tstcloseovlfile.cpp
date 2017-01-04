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
// tstcloseovlfile.cpp
//
// Event generation, validation, and execution routines for 
// close overlay file events
//
// Created 13 Jan 05 by W Stark


#include "stdafx.h"
#include "testglob.h"



// ----------------------------------------------------------------------------------------------

// Generate a close overlay file event
void CMapAutoTester::GenCloseOvlFile(test_event_t& event)
{
   if (gOpenOverlayPointers.GetSize() != 0) // if there are file overlays currently open
   {
      // get a random number between 0 and (# of open file overlays - 1) to serve as the index
      // into the open file overlays CPtrArray (gOpenFileOverlays)...
      event.info.testFileIndex = rand() % gOpenOverlayPointers.GetSize();
   }
}


// ----------------------------------------------------------------------------------------------

// Validate a close overlay file event
int CMapAutoTester::ValCloseOvlFile(test_event_t& event)
{
   CString buf;
   
   if (gOpenOverlayPointers.GetSize() == 0) // if there are no file overlays currently open
   {
      // write change to test log
      buf.Format("(%d) Attempting to close an overlay file, but none are currently open so ABORTING CLOSE OVERLAY FILE TEST...\n", gCurrentIteration);
      LogEntry(buf, true);

      return TEST_NON_FATAL_ERROR;  // no need to continue...
   }

   // Otherwise there is at least one file overlay currently open, so proceed with the test validation...

   // make sure the index into the open file overlays array is valid
   if (event.info.testFileIndex < 0 || event.info.testFileIndex >= gOpenOverlayPointers.GetSize())
      return TEST_FATAL_ERROR;

   return TEST_NO_ERROR;
}


// ----------------------------------------------------------------------------------------------

// Execute a close overlay file event
int CMapAutoTester::ExecCloseOvlFile(test_event_t event)
{
   CString buf;
   int index = event.info.testFileIndex;  // index for the current randomly-selected overlay
                                          // that is to be closed

   // define the randomly-selected open file overlay pointer and corresponding class title and file path...
   C_overlay *current_overlay_ptr = (C_overlay*)gOpenOverlayPointers.GetAt(index);
   CString current_class_title = gOpenOverlayClassTitles.GetAt(index);
   CString current_file_path = gOpenOverlayFilePaths.GetAt(index);

   // write change to test log
   buf.Format("(%d) Attempting to close a %s file (%s)\n", gCurrentIteration, current_class_title, current_file_path);
   LogEntry(buf, true);

   // use the overlay manager to close the currently open test file overlay
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
      
   if (ovl_mgr->close(current_overlay_ptr) == FAILURE)
      return TEST_FATAL_ERROR;

   // remove the pointer, class title and file path entries that correspond to this overlay...
   gOpenOverlayPointers.RemoveAt(index);
   gOpenOverlayClassTitles.RemoveAt(index);
   gOpenOverlayFilePaths.RemoveAt(index);

   // update globals and counters
   gNumOvlFileCloses++;

   return TEST_NO_ERROR;
}
