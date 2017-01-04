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
// test.cpp

// Test suite external interface function implementations.
// Created by David Pope  30 July 96

// The test suite operates by getting input parameters from the calling
// routine, then executing a set of tests based on those parameters.
// The suite uses a statistical test model, in that "events" are randomly
// generated and executed.  It is hoped that by executing enough events,
// bugs in the system will make themselves known.

// One of the parameters gathered from the caller is a number of
// iterations.  For each iteration, an event is generated, validated,
// and then (probably) executed.  Events are very simple occurrences,
// such as (Zoom to 150%), (Scale to GNC), or (Move to 30N 140W).  Only
// one major map parameter is altered at a time, so that if the current
// scale is GNC, the map center is at 30N 140W, and the current zoom is
// 150%, an event to "Scale to JNC" will result in a scale of JNC, a
// map center at 30N 140W, and a zoom factor of 150%.

// It should be clear that certain operations (events) are incompatible
// with certain map settings.  For example, if the map is at
//       (30N 140W, GNC, 50%)
// and a (Scale to TIROS 1:20M) event is generated, the zoom factor
// must also be altered to 100%, since TIROS does not support zooming out.
// The event cycle thus becomes Generation, Validation, and Execution.
// Steps must be taken in the Validation step to ensure the viability
// of the operation, or else it must return a nonfatal error if this
// isn't possible.

// Which brings us to the next point, that the return values used
// within the suite are TEST_NO_ERROR_, TEST_NON_FATAL_ERROR, and
// TEST_FATAL_ERROR.  The intent is that testing should continue for
// nonfatal errors and abort for fatal errors.

// General notes:
//
//    The user of this suite must call Init() to set things up,
//    then a sequence of SetFlags() calls to tell the test suite how
//    to behave, then Execute() to run the tests, and finally
//    Finish() to clean up afterwards.
//
//    Test settings are stored in a suite-global flags variable.  See
//    test.h for particular flag constants.  SetFlags() and GetFlags()
//    should be used whenever accessing these flags from outside the
//    suite.

#include "stdafx.h"
#include "appinfo.h"
#include "getobjpr.h"
#include "param.h"
#include "testglob.h"
#include "tstmapmv.h"
#include "tstmapsc.h"
#include "tstmapzm.h"
#include "tstovtog.h"
#include "tstopenovlfile.h"
#include "tstcloseovlfile.h"
#include "errx.h"
#include "map.h"
#include "geo_tool.h"
#include "proj.h"  //  For PROJ_get_default_projection_type()
#include "OvlFctry.h"
#include "..\Common\ComErrorObject.h"
#include "utils.h"
#include "maps.h"

#include "FalconView/MapView.h"

////////// Test suite global variables ///////////////////////////////
test_flags_t gTestFlags;

CStdioFile gTestLogFile;

int gCurrentZoom;

double gCurrentLat, 
   gCurrentLon;

ProjectionEnum gCurrentProjection;

int gCurrentElevExaggeration;

fvw::CameraMode gCurrentCameraMode;

int gCurrent3DMapTilt;

long gCurrentIteration;

UINT gNumIterations;

int gNumMoves,
   gNumScales,
   gNumZooms,
   gNumProjChanges,
   gNumTiltChanges,
   gNumElevExaggChanges,
   gNumCameraModeChanges,
   gNumOvlToggles,
   gNumOvlFileOpens,
   gNumOvlFileCloses,
   gNumOpenOverlayAborts,
   gNumCloseOverlayAborts,
   gNumNoData;

CString gTestFileFolderPath;

CTime gStartTime;

// for static overlay toggling
CArray<OverlayTypeDescriptor *> gSelectedStaticOvlList;  // to store the names of the static overlays selected from
// the test dialog's overlay property page list box
CArray<int, int> gSelectedStaticOvlCounters;  // to keep track of how many times each static
// overlay has been toggled during the test

// for overlay file opening/closing
CStringArray gTestFileNames;  // to store the current set of test overlay files
CPtrArray gOpenOverlayPointers;  // to store pointers to open test file overlays
CStringArray gOpenOverlayClassTitles;  // to store class titles for open test file overlays
CStringArray gOpenOverlayFilePaths;  // to store file paths for open test file overlays


////////// External interface functions ///////////////////////////////

// Gets the path to the location of FVW.exe...
static int get_full_path_log_name(CString& name)
{
   static boolean_t initialized = FALSE;
   static char full_path[_MAX_PATH];

   if (initialized != TRUE)
   {
      ZeroMemory( full_path, sizeof(full_path) );
      ExpandEnvironmentStrings( "%PUBLIC%", full_path, MAX_PATH );
      PathAppend(full_path, "Public XPlan AppData\\Logs\\");

      initialized = TRUE;
   }

   name = full_path;

   return SUCCESS;
}

// Returns test_log.txt with the same path as FVW.exe...
CString Test_log_name(void)
{
   CString path;
   get_full_path_log_name(path);
   path += "test_log.txt";
   return path;
}

// Returns last_status.txt with the same path as FVW.exe...
CString Last_status_name(void)
{
   CString path;
   get_full_path_log_name(path);
   path += "last_status.txt";
   return path;
}

// Initializes the test suite, including opening the test error file.
// Must be called before using the other methods in the CMapAutoTester class
int CMapAutoTester::Init(void)
{
   // create the log file
   try
   {
      gTestLogFile.Open(Test_log_name(), CFile::modeCreate | CFile::modeWrite);
   }
   catch (CFileException* e) {
      e->Delete();}

   try
   {
      gTestLogFile.Close();   // close the test log file
   }
   catch(...)
   {
      AfxMessageBox("Error closing test log file");
   }

   // get timestamp for log file...
   gStartTime = CTime::GetCurrentTime();
   CString s, buf;
   char ver[MAX_VERSION_SIZE];

   project_version_str(ver, MAX_VERSION_SIZE);
   s = gStartTime.Format( "%A, %B %d, %Y  %I:%M:%S %p" );
   buf.Format("%s version %s test suite activity log file\nCreated %s\n",
      appShortName(), ver, s);

   LogEntry(buf);

   // Get OS version information...
   OSVERSIONINFO info;
   info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

   if (::GetVersionEx(&info) == TRUE)
   {
      CString os;

      if (info.dwPlatformId == VER_PLATFORM_WIN32s)
         os = "Win32s";
      else if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
         os = "Windows";
      else if (info.dwPlatformId == VER_PLATFORM_WIN32_NT)
         os = "Windows NT";

      buf.Format("%s version %d.%d build %d %s\n",
         (const char*) os, info.dwMajorVersion, info.dwMinorVersion, 
         info.dwBuildNumber, info.szCSDVersion);
      LogEntry(buf);
   }

   // get the current map scale and map center (lat, lon)...
   MapView* map_view =
      static_cast<MapView *>(UTL_get_active_non_printing_view());

   double dCurrentMapCenterLat = 0.0;
   double dCurrentMapCenterLon = 0.0;
   int nCurrentMapZoom = 0;
   ProjectionEnum eCurrentMapProjection = EQUALARC_PROJECTION;
   int nCurrentElevExaggeration = 1;
   fvw::CameraMode eCurrentCameraMode = fvw::CAMERA_MODE_PAN;
   int nCurrent3DMapTilt = 0;
   MapScale CurrentMapScale;

   if (map_view != nullptr)
   {
      const MapProjectionParams& map_proj_params = map_view->GetMapProjParams();

      dCurrentMapCenterLat = map_proj_params.center.lat;
      dCurrentMapCenterLon = map_proj_params.center.lon;
      nCurrentMapZoom = map_proj_params.zoom_percent;
      eCurrentMapProjection = map_proj_params.type;
      CurrentMapScale = map_view->GetMapType().get_scale();

      // TODO: get current values for nCurrentElevExaggeration, eCurrentCameraMode, nCurrent3DMapTilt
   }


   // set suite global variables...
   if (nCurrentMapZoom)
      gCurrentZoom = nCurrentMapZoom;  // set global to current zoom value
   else
      gCurrentZoom = 100;  // default value

   if (dCurrentMapCenterLat && dCurrentMapCenterLon)
   {
      gCurrentLat = dCurrentMapCenterLat;  // set globals to current map center values
      gCurrentLon = dCurrentMapCenterLon;
   }
   else
   {
      gCurrentLat = 34.0;  // default values - Atlanta @ N 34  W 84 (approx)
      gCurrentLon = -84.0;
   }
   gCurrentProjection = eCurrentMapProjection;
   gCurrentElevExaggeration = nCurrentElevExaggeration;
   gCurrentCameraMode = eCurrentCameraMode;
   gCurrent3DMapTilt = nCurrent3DMapTilt;

   gCurrentIteration = 0;
   gNumMoves = 0;
   gNumScales = 0;
   gNumZooms = 0;
   gNumProjChanges = 0;
   gNumTiltChanges = 0;
   gNumElevExaggChanges = 0;
   gNumCameraModeChanges = 0;
   gNumOvlToggles = 0;
   gNumOvlFileOpens = 0;
   gNumOvlFileCloses = 0;
   gNumOpenOverlayAborts = 0;
   gNumCloseOverlayAborts = 0;
   gNumNoData = 0;

   // clear all test flags
   ClearFlags(ALL_TEST_FLAGS);

   // redraw map at standard starting position     
   int err = UTL_change_view_map_to_best(map_view, gCurrentLat, gCurrentLon, 
      "CADRG", MapScale(2000000), 0.0, gCurrentZoom, gCurrentProjection);

   if (err != SUCCESS)
   {
      ERR_report("set_to_best failed setting initial map to JNC near Atlanta");
      return FAILURE;
   }

   ForceRedraw();
   return SUCCESS;
}

void CMapAutoTester::SetRandomSeed(unsigned int nSeed /* = 0 */)
{
   // seed the random number generator...
   if (nSeed == 0)
      nSeed = static_cast<unsigned int>(gStartTime.GetTime());

   srand(nSeed);

   CString buf;
   buf.Format("Random number seed = %ld\n\n", nSeed);
   LogEntry(buf);
}

void CMapAutoTester::SetWriteToErrorLog(BOOL bWriteToErrorLog)
{
   m_bWriteToErrorLog = bWriteToErrorLog;
}

// Deinitialize the test suite...
void CMapAutoTester::Finish(void)
{
   int open_overlay_flag = 0;
   int static_overlays_toggled_during_cleanup = 0;
   int overlay_files_closed_during_cleanup = 0;
   CString buf;


   DumpStatus();

   // get timestamp for log file...
   CTime t = CTime::GetCurrentTime();
   CString s = t.Format( "%A, %B %d, %Y  %I:%M:%S %p" );
   buf.Format("\nTesting finished: %s\n", s);
   LogEntry(buf);

   // check the static overlay toggle counters.  If a counter value is found to be odd, toggle the
   // static overlay that corresponds to that counter to return the static overlay settings to
   // what they were prior to testing...
   int num_counters = gSelectedStaticOvlCounters.GetSize();
   if (num_counters)
   {
      C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

      for (int i=0; i<num_counters; i++)
      {
         if (gSelectedStaticOvlCounters.GetAt(i) % 2)
         {
            // toggle this static overlay...
            ovl_mgr->toggle_static_overlay(gSelectedStaticOvlList.GetAt(i)->overlayDescriptorGuid);
            static_overlays_toggled_during_cleanup++;
         }
      }

      if (static_overlays_toggled_during_cleanup)
      {
         // write to test log
         buf.Format("\n\n%d static overlays were toggled during test cleanup.\n\n", static_overlays_toggled_during_cleanup);
         LogEntry(buf);
      }
   }

   // clear the test file names array...
   gTestFileNames.RemoveAll();

   // clear the toggle test's static overlay names array...
   gSelectedStaticOvlList.RemoveAll();

   // check the gOpenOverlayFilePaths global CStringArray to see if there are 
   // any overlay files still open.  If there are, close them...
   if (gOpenOverlayFilePaths.GetSize())
   {
      for (int i=0; i<gOpenOverlayFilePaths.GetSize(); i++)
      {
         // get the pointer to the open overlay...
         C_overlay *current_overlay_ptr = (C_overlay*)gOpenOverlayPointers.GetAt(i);

         // use the overlay manager to close the overlay...
         C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

         if (ovl_mgr->close(current_overlay_ptr) == FAILURE)
            open_overlay_flag = 1;

         overlay_files_closed_during_cleanup++;
      }

      // remove all entries from the global arrays...
      gOpenOverlayPointers.RemoveAll();
      gOpenOverlayClassTitles.RemoveAll();
      gOpenOverlayFilePaths.RemoveAll();

      if (overlay_files_closed_during_cleanup)
      {
         // write to test log...
         buf.Format("\n\n%d overlay files were closed during test cleanup.\n\n", overlay_files_closed_during_cleanup);
         LogEntry(buf);
      }
   }

   if (open_overlay_flag)
      AfxMessageBox("One or more overlay files could not be closed during test cleanup.  Please make sure that all overlay files are closed.");
}


// Sets the global test flags indicated by the flags parameter.
// No flags are cleared by this function.  Returns the resulting global
// test flags.  See test.h for the flag values...
test_flags_t CMapAutoTester::SetFlags(test_flags_t flags)
{
   gTestFlags |= flags;

   return gTestFlags;
}


// Clears the global test flags indicated by the flags parameter.
// No flags are set by this function.  Returns the resulting global
// test flags.  See test.h for the flag values...
test_flags_t CMapAutoTester::ClearFlags(test_flags_t flags)
{
   flags ^= 0xFFFFFFFF;  // bitwise NOT
   gTestFlags &= flags;

   return gTestFlags;
}


// Sets the global number of iterations...
void CMapAutoTester::SetIterations(UINT numIterations)
{
   gNumIterations = numIterations;
}


/////////////////////////////////////////////////////////////////////////////
// The mother of all test routines.  This is the driver for all the tests.
/////////////////////////////////////////////////////////////////////////////

int CMapAutoTester::Execute(void)
{
   test_event_t event;
   int retval;

   // if no test type flags were checked - abort test...
   if ((gTestFlags & TEST_TYPE_ALL) == 0)
      return FAILURE;

   for ( UINT i=0 ; i < gNumIterations ; i++ )
   {
      Sleep(PRM_get_registry_int("Developer","Test_Delay",0));
      gCurrentIteration = i;

      // Generate and execute an event (relies on C's short-circuiting)...
      if ((retval = GenerateEvent(event)) == TEST_FATAL_ERROR)
         return FAILURE;

      if ((retval = ValidateEvent(event)) == TEST_FATAL_ERROR)
         return FAILURE;
      else if (retval == TEST_NON_FATAL_ERROR)      // skip execution
      {
         if (event.type == TEST_CLOSE_OVL_FILE)
         {
            i--;
            gNumCloseOverlayAborts++;
            continue;
         }
         else if (event.type == TEST_OPEN_OVL_FILE)
         {
            i--;
            gNumOpenOverlayAborts++;
            continue;
         }
         else
         {
            i--;
            gNumNoData++;
            continue;
         }
      }

      if ((retval = ExecuteEvent(event)) == TEST_FATAL_ERROR)
         return FAILURE;
      else if (retval == TEST_NON_FATAL_ERROR)      // execution was aborted
      {
         if (event.type == TEST_OPEN_OVL_FILE)
         {
            i--;
            gNumOpenOverlayAborts++;
            continue;
         }
         else
         {
            i--;
            gNumNoData++;
            continue;
         }
      }

      // See if there's anything going on...
      if (UTL_MessagePump(VK_ESCAPE))
         return SUCCESS;
   }

   return SUCCESS;
}


// Generate a random event, with appropriate event information.
// E.g., if it's a map-move event generate a lat-long; if it's a
// map-scale event generate a target map scale...
// Return values:
//    TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::GenerateEvent(test_event_t& event)
{
   boolean_t      done = FALSE;
   unsigned long  maskCompare;
   int            type;

   // if no test type flags were checked...
   if ((gTestFlags & TEST_TYPE_ALL) == 0)
      return TEST_FATAL_ERROR;

   // Select event type...
   while (!done)
   {
      // get random number between 0 and (NUM_EVENT_TYPES - 1)...
      type = rand() % NUM_EVENT_TYPES;
      maskCompare = MIN_EVENT_TYPE << type;

      if (gTestFlags & maskCompare)
      {
         done = TRUE;
         event.type = maskCompare;
      }
   }

   //// Generate event information...
   done = FALSE;

   switch(event.type)
   {
   case TEST_3D_ELEV_EXAG:
      GenElevExaggeration(event);
      break;
   case TEST_3D_MAP_TILT:
      GenMapTilt(event);
      break;
   case TEST_3D_CAMERA_MODES:
      GenCameraMode(event);
      break;
   case TEST_MAP_PROJ:
      GenMapProj(event);
      break;
   case TEST_MAP_MOVE:
      GenMapMove(event);
      break;
   case TEST_MAP_SCALE:
      GenMapScale(event);
      break;
   case TEST_MAP_ZOOM:
      GenMapZoom(event);
      break;
   case TEST_OVL_TOGGLE:
      GenOvlToggle(event);
      break;
   case TEST_OPEN_OVL_FILE:
      GenOpenOvlFile(event);
      break;
   case TEST_CLOSE_OVL_FILE:
      GenCloseOvlFile(event);
      break;
   default:
      break;
   }

   return TEST_NO_ERROR;
}


// This function contains all the specialized rules that are not
// appropriate to place in GenerateEvent().  For example, you
// can't zoom out on TIROS, so if we're currently on TIROS and this
// is a request to zoom to a factor < 100%, adjust the zoom factor
// to 100%.
// The result should be a valid event.  If for some reason it is
// not possible to get a valid event from the event type passed in,
// return FAILURE (and re-check the code, since this should not
// be possible...).
// Return values:
//    TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ValidateEvent(test_event_t& event)
{
   switch(event.type)
   {
   case TEST_3D_ELEV_EXAG:
      return ValElevExaggeration(event);
      break;
   case TEST_3D_MAP_TILT:
      return ValMapTilt(event);
      break;
   case TEST_3D_CAMERA_MODES:
      return ValCameraMode(event);
      break;
   case TEST_MAP_PROJ:
      return ValMapProj(event);
      break;
   case TEST_MAP_MOVE:
      return ValMapMove(event);
      break;
   case TEST_MAP_SCALE:
      return ValMapScale(event);
      break;
   case TEST_MAP_ZOOM:
      return ValMapZoom(event);
      break;
   case TEST_OVL_TOGGLE:
      return ValOvlToggle(event);
      break;
   case TEST_OPEN_OVL_FILE:
      return ValOpenOvlFile(event);
      break;
   case TEST_CLOSE_OVL_FILE:
      return ValCloseOvlFile(event);
      break;
   default:
      ASSERT(FALSE);
      return TEST_FATAL_ERROR;
      break;
   }
}


// This function executes the specified event.  It assumes that the
// event is valid (i.e., has been passed through ValidateEvent())
// Return values:
//    TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ExecuteEvent(test_event_t event)
{
   CString buf;
   CWaitCursor wc;      // nifty
   int retval;

   switch(event.type)
   {
   case TEST_3D_ELEV_EXAG:  /////////////////////////////////////////////////
      if ((retval = ExecElevExaggeration(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_3D_MAP_TILT:  //////////////////////////////////////////////////
      if ((retval = ExecMapTilt(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_3D_CAMERA_MODES:  //////////////////////////////////////////////
      if ((retval = ExecCameraMode(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_MAP_PROJ:  /////////////////////////////////////////////////////
      if ((retval = ExecMapProj(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_MAP_MOVE:  /////////////////////////////////////////////////////
      if ((retval = ExecMapMove(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_MAP_SCALE: /////////////////////////////////////////////////////
      if ((retval = ExecMapScale(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_MAP_ZOOM: //////////////////////////////////////////////////////
      if ((retval = ExecMapZoom(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_OVL_TOGGLE: ////////////////////////////////////////////////////
      if ((retval = ExecOvlToggle(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_OPEN_OVL_FILE: /////////////////////////////////////////////////
      if ((retval = ExecOpenOvlFile(event)) != TEST_NO_ERROR)
         return retval;
      break;
   case TEST_CLOSE_OVL_FILE: /////////////////////////////////////////////////
      if ((retval = ExecCloseOvlFile(event)) != TEST_NO_ERROR)
         return retval;
      break;

   default: /////////////////////////////////////////////////////////////////
      buf.Format("ERROR: TOP-LEVEL INTERNAL TEST ERROR (file %s, line %d)",
         __FILE__, __LINE__);
      LogEntry(buf);
      DumpStatus();
      return TEST_FATAL_ERROR;
      break;
   }

   ForceRedraw();

   return TEST_NO_ERROR;
}




/////////////////////////////////////////////////////////////////////////////
// Helper routines
/////////////////////////////////////////////////////////////////////////////

// Logs an arbitrary text entry in the test log file
// and records map status at the time of the entry into Last_Status...
void CMapAutoTester::LogEntry(const char* str, bool bAppendTimeStamp /* = false */)
{
   BOOL status;

   // create or recreate the Last_Status file and record the current state 
   // before the next iteration is attempted...
   try 
   {
      status = gTestLogFile.Open(Last_status_name(), CFile::modeCreate | CFile::modeWrite);

      if (status)
      {
         CString buf;
         const int STR_LEN = 256;
         char latLonStr[STR_LEN];  // make sure that the buffer is large enough

         // get the current map projection...
         MapView* view =
            static_cast<MapView *>(UTL_get_active_non_printing_view());

         if (view == nullptr)
         {
            ERR_report("Failure getting active, non-printing MapView.");
            return;
         }

         const MapType& map_type = view->GetMapType();
         const MapProjectionParams& map_proj_params =
            view->GetMapProjParams();

         // get the center as a formated string...
         GEO_lat_lon_to_string(map_proj_params.center.lat,
            map_proj_params.center.lon, latLonStr, STR_LEN);

         // Iteration #, Map Source, Scale, Center, Zoom, and Rotation will be
         // written to the file...
         buf.Format("Iteration: %d\nMap Scale: %s %s\nMap Center: %s\n"
            "Zoom: %d%%\nRotation: %lf\n\nOverlays:\n", 
            gCurrentIteration, 
            map_type.get_source_string(),
            map_type.get_scale_string(),
            latLonStr, map_proj_params.zoom_percent,
            map_proj_params.rotation);

         gTestLogFile.WriteString(buf);
         gTestLogFile.Flush();

         // writes out active overlays...
         C_overlay *ovl = OVL_get_overlay_manager()->get_first();
         while (ovl)
         {
            // use get name to output name...
            buf.Format("\t%s\n", OVL_get_overlay_manager()->GetOverlayDisplayName(ovl));
            gTestLogFile.WriteString(buf);
            gTestLogFile.Flush();

            // get next overlay...
            ovl = OVL_get_overlay_manager()->get_next(ovl);
         }

         // closes Last_Status file...
         gTestLogFile.Close();
      }
      else
         ERR_report("CFile::Open() failed.");
   }
   catch (CFileException* e) 
   {
      e->Delete();
      ERR_report("CFileException thrown while trying to write last status file.");
   }

   // record the input string in the test log...
   try 
   {
      // opens the test log for appending...
      status = gTestLogFile.Open(Test_log_name(), CFile::modeNoTruncate | 
         CFile::modeCreate | 
         CFile::modeWrite);
      if (status)
      {
         CString msg;

         if (m_bWriteToErrorLog && bAppendTimeStamp)
         {
            msg.Format("[TESTER] %s", str);
            msg.Replace('\n', ' ');
            WriteToLogFile(_bstr_t(msg));
         }

         if (bAppendTimeStamp)
         {
            time_t tt = time(NULL);
            struct tm td, *today = &td;
            localtime_s( &td, &tt );

            char pszStr[30];
            strftime(pszStr, sizeof(pszStr) - 1, "%m/%d/%y %H:%M:%S ", today);

            msg.Format("%s %s", pszStr, str);
         }
         else
            msg = str;

         // writes str to test log...
         gTestLogFile.SeekToEnd();
         gTestLogFile.WriteString(msg);
         gTestLogFile.Flush();

         // close the file...
         gTestLogFile.Close();
      }
      else
         ERR_report("CFile::Open() failed.");
   }
   catch (CFileException *e) 
   {
      e->Delete();
      ERR_report("CFileException thrown while trying to append the test log.");
   }
}

// Logs an error entry in the test log file...
void CMapAutoTester::LogError(test_domain_t domain, int error)
{
   CString buf;
   buf.Format("(%d) ERROR: ", gCurrentIteration);

   switch(domain)
   {
   case MAP_MOVE:
   case MAP_SCALE:
   case MAP_ZOOM:
      switch(error)
      {
      case FAILURE:
         buf += "Unknown map failure\n";
         break;
      case FV_NO_DATA:
         buf += "No map data for desired region\n";
         break;
      case COV_FILE_OUT_OF_SYNC:
         buf += "Coverage file out of sync\n";
         break;
      case MEMORY_ERROR:
         buf += "Memory error while displaying map\n";
         break;
      case FATAL_MAP_DISPLAY_ERROR:
         buf += "Fatal map display error\n";
         break;
      default:
         buf += "TEST PROCEDURE ERROR (MAP_MOVE)\n";
         break;
      }
      break;
   default:
      {
         CString tmp;

         tmp.Format("ERROR: TOP-LEVEL INTERNAL TEST ERROR (file %s, line %d)",
            __FILE__, __LINE__);
         buf += tmp;
         DumpStatus();
         ASSERT(FALSE);
      }
      break;
   }

   LogEntry(buf, true);
}


// Clears the main FV window and redraws before returning...
void CMapAutoTester::ForceRedraw(void)
{
   fvw_get_view()->RedrawWindow();
}


// Dumps out all sorts of useful information to the test log file.
// If you add new event types, be sure to include them in the output
// and the calculations here...
void CMapAutoTester::DumpStatus(void)
{
   CString buf;
   MapSource source;
   MapScale scale;
   MapSeries series;
   CString display_title, class_name;

   buf.Format("\nTest Run Status:\n");
   LogEntry(buf);

   // print out map location...
   const int STR_LEN = 256;
   char latLonStr[STR_LEN];

   GEO_lat_lon_to_string(gCurrentLat, gCurrentLon, latLonStr, STR_LEN);
   buf.Format("\tMap center: %s\n", latLonStr);
   LogEntry(buf);

   // print out map zoom factor...
   buf.Format("\tMap zoom factor: %d%%\n", gCurrentZoom);
   LogEntry(buf);


   // print out total # of each type of operation...
   buf.Format("\n\t# Successful move Ops               : %d\n", gNumMoves);
   LogEntry(buf);
   buf.Format("\t# Successful Scale Ops              : %d\n", gNumScales);
   LogEntry(buf);
   buf.Format("\t# Successful Zoom Ops               : %d\n", gNumZooms);
   LogEntry(buf);
   buf.Format("\t# Successful Proj Ops               : %d\n", gNumProjChanges);
   LogEntry(buf);
   buf.Format("\t# Successful Tilt Ops               : %d\n", gNumTiltChanges);
   LogEntry(buf);
   buf.Format("\t# Successful Camera Mode Ops               : %d\n", gNumCameraModeChanges);
   LogEntry(buf);
   buf.Format("\t# Successful Elevation Scale Ops               : %d\n", gNumElevExaggChanges);
   LogEntry(buf);
   buf.Format("\t# Successful Overlay Toggle Ops     : %d\n", gNumOvlToggles);
   LogEntry(buf);

   int num_static_overlays = gSelectedStaticOvlList.GetSize();
   for (int i=0; i<num_static_overlays; i++)
   {
      OverlayTypeDescriptor* pOverlayTypeDesc = gSelectedStaticOvlList.GetAt(i);
      buf.Format("\t\t# Successful %s Toggles\t\t: %d\n", pOverlayTypeDesc->displayName, gSelectedStaticOvlCounters.GetAt(i));
      LogEntry(buf);
   }

   buf.Format("\t# Successful Overlay File Open Ops  : %d\n", gNumOvlFileOpens);
   LogEntry(buf);
   buf.Format("\t# Successful Overlay File Close Ops : %d\n", gNumOvlFileCloses);
   LogEntry(buf);

   // print out total # of successful operations...
   long totalSuccessfulOps = gNumMoves + gNumScales + gNumZooms 
      + gNumOvlToggles + gNumOvlFileOpens + gNumOvlFileCloses;
   buf.Format("\tTotal # successful operations       : %d\n", totalSuccessfulOps);
   LogEntry(buf);

   buf.Format("\n\t# Aborted Overlay File Open Ops     : %d\n", gNumOpenOverlayAborts);
   LogEntry(buf);
   buf.Format("\t# Aborted Overlay File Close Ops    : %d\n", gNumCloseOverlayAborts);
   LogEntry(buf);
   buf.Format("\t# Aborted Ops Due To No Map Data    : %d\n", gNumNoData);
   LogEntry(buf);

   // print out total # of aborted operations...
   long totalAbortedOps = gNumOpenOverlayAborts + gNumCloseOverlayAborts + gNumNoData;
   buf.Format("\tTotal # aborted operations          : %d\n", totalAbortedOps);
   LogEntry(buf);


   // print out elapsed time...
   CTime now = CTime::GetCurrentTime();
   CTimeSpan testInterval = now - gStartTime;
   buf.Format("\n\tElapsed time: %s\n",
      testInterval.Format( "%D days, %H hours, %M min, %S sec" ));
   LogEntry(buf);
   buf.Format("\tAvg seconds per operation: %.4f\n",
      (double) testInterval.GetTotalSeconds() / (double) totalSuccessfulOps);
   LogEntry(buf);
}
