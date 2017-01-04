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



/////////////////////////////////////////////////////////////////////////////
// tstopenovlfile.cpp
//
// Event generation, validation, and execution routines for 
// open overlay file events
//
// Created 6 Jan 05 by W Stark


#include "stdafx.h"
#include "testglob.h"
#include "OvlFctry.h"


// ----------------------------------------------------------------------------------------------

// Generate an open overlay file event
void CMapAutoTester::GenOpenOvlFile(test_event_t& event)
{
   if (gTestFileNames.GetSize() != 0) // if there is at least one overlay file in the list
   {
      // get a random number between 0 and (# of overlay files - 1) to serve as the index
      // into the overlay file CStringArray (gTestFileNames)...
      event.info.testFileIndex = rand() % gTestFileNames.GetSize();
   }
}


// ----------------------------------------------------------------------------------------------

// Validate an open overlay file event
int CMapAutoTester::ValOpenOvlFile(test_event_t& event)
{
   CString buf;

   if (gTestFileNames.GetSize() == 0)  // if there are no overlay files in the list
   {
      // write change to test log
      buf.Format("(%d) Attempting to open an overlay file, but there are none in the specified test file directory so ABORTING OPEN OVERLAY FILE TEST...\n", gCurrentIteration);
      LogEntry(buf, true);

      return TEST_NON_FATAL_ERROR;  // no need to continue...
   }

   // Otherwise there is at least one overlay file in the list, so proceed with the test validation...

   // make sure the index into the test file array is valid
   if (event.info.testFileIndex < 0 || event.info.testFileIndex >= gTestFileNames.GetSize())
      return TEST_FATAL_ERROR;

   return TEST_NO_ERROR;
}


// ----------------------------------------------------------------------------------------------

// Execute an open overlay file event
int CMapAutoTester::ExecOpenOvlFile(test_event_t event)
{
   CString buf;
   int index = 0;

   // get the path to the current randomly-selected test file...
   CString current_file_path = gTestFileNames.GetAt(event.info.testFileIndex);

   // get the current test file's extension...
   index = current_file_path.ReverseFind('.');
   CString ext = current_file_path.Mid(index);

   // get the factory that corresponds to the current test file's extension...
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   OverlayTypeDescriptor *pOverlayTypeDesc = overlay_type_util::GetOverlayTypeFromFileExtension((LPCTSTR)ext);

   // if the returned factory is NULL, then the currently-selected file IS NOT a
   // FalconView-registered file type.  In this case return TEST_NON_FATAL_ERROR...
   if (pOverlayTypeDesc == NULL)
   {
      // write change to test log
      buf.Format("(%d) Attempting to open file %s, but it's factory is NULL so ABORTING OPEN OVERLAY FILE TEST...\n", gCurrentIteration, current_file_path);
      LogEntry(buf, true);

      return TEST_NON_FATAL_ERROR;
   }

   // get the class title that corresponds to the current class name
   CString class_title = pOverlayTypeDesc->displayName;

   // now check to see if the current file is already stored in 
   // the gOpenOverlayFilePaths global CStringArray.  If it is, then the
   // current file has already been opened by the test suite but has not
   // yet been closed.  This being the case, the current file need not be
   // opened again and must definitely NOT be added again to the three
   // global arrays that record the currently-open overlay files.
   for (int i=0; i<gOpenOverlayFilePaths.GetSize(); i++)
   {
      if (gOpenOverlayFilePaths.GetAt(i) == current_file_path)
      {
         // write change to test log
         buf.Format("(%d) Attempting to open file %s, but it is already open so ABORTING OPEN OVERLAY FILE TEST...\n", gCurrentIteration, current_file_path);
         LogEntry(buf, true);

         // this file has previously been opened and not yet been 
         // closed, so abort this test attempt...
         return TEST_NON_FATAL_ERROR;
      }
   }

   // write change to test log
   buf.Format("(%d) Attempting to open a %s file (%s)\n", gCurrentIteration, class_title, current_file_path);
   LogEntry(buf, true);

   // use the overlay manager to open the current test file
   C_overlay *current_overlay = NULL;
   if (ovl_mgr->OpenFileOverlay(pOverlayTypeDesc->overlayDescriptorGuid, current_file_path, current_overlay) == FAILURE)
      return TEST_FATAL_ERROR;

   // add the pointer, class title and file path entries that correspond to 
   // the overlay that we just opened...
   gOpenOverlayPointers.Add(current_overlay);
   gOpenOverlayClassTitles.Add(class_title);
   gOpenOverlayFilePaths.Add(current_file_path);


   // update globals and counters
   gNumOvlFileOpens++;

   return TEST_NO_ERROR;
}
