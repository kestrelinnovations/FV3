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
// tstmapproj.cpp
//
// Event generation, validation, and execution routines for map-projection events
//
// Created 5 Mar 14 by R. Presley
//

#include "stdafx.h"
#include "testglob.h"
#include "map.h"
#include "proj.h"
#include "utils.h"
#include "GeospatialViewer.h"
#include "GeospatialViewController.h"
#include "GeospatialScene.h"
#include "getobjpr.h"


// Helper function for naming the projections
const char* ProjectionName(ProjectionEnum proj)
{
   CArray <class ProjectionID, const class ProjectionID&> available_projection_array;
   PROJ_get_available_projection_types(&available_projection_array, TRUE);
   for (int z=0; z<available_projection_array.GetSize(); z++)
   {
      if(proj == available_projection_array.GetAt(z).GetProjectionType())
      {
         return available_projection_array.GetAt(z).get_string();
      }
   }
   return "- Invalid Projection Type -";
}


// Helper function for naming the camera modes
const char* CameraModeName(fvw::CameraMode mode)
{
   switch(mode)
   {
   case fvw::CAMERA_MODE_FLIGHT:
      return "Flight Mode";
   case fvw::CAMERA_MODE_GROUND_VIEW:
      return "Ground Mode";
   case fvw::CAMERA_MODE_PAN:
      return "Standard Mode";
   case fvw::CAMERA_MODE_NONE:
      return "None";
   }
   return "- Invalid camera mode  -";
}


void CMapAutoTester::GenMapProj(test_event_t& event)
{
   // select random map projection
   int num_selected_projs = m_selectedMapProjections.GetCount();
   if (num_selected_projs)
   {
      int index = (int) (rand() % m_selectedMapProjections.GetCount());
      POSITION p = m_selectedMapProjections.FindIndex(index);
      event.info.proj = m_selectedMapProjections.GetAt(p);
   }
   else
      event.info.proj = EQUALARC_PROJECTION;
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ValMapProj(test_event_t& event)
{
   // Check for a valid projection type
   CArray <class ProjectionID, const class ProjectionID&> available_projection_array;
   PROJ_get_available_projection_types(&available_projection_array, TRUE);
   for (int z=0; z<available_projection_array.GetSize(); z++)
   {
      CString title = available_projection_array.GetAt(z).get_string();
      ProjectionEnum proj = available_projection_array.GetAt(z).GetProjectionType();
      if(event.info.proj == proj)
         return TEST_NO_ERROR;
   }

   // Projection type was not found
   return TEST_FATAL_ERROR;
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ExecMapProj(test_event_t event)
{
   CString buf;
   buf.Format("(%d) Attempting to change projection to: %s\n", 
            gCurrentIteration, ProjectionName(event.info.proj));
   LogEntry(buf, true);

   MapView* pView = fvw_get_view();
   assert(pView != nullptr);
   const MapType& type = pView->GetMapType();
   const MapProjectionParams& proj_params = pView->GetMapProjParams();
   const int err = UTL_change_view_map(
      pView,
      type.get_source(),
      type.get_scale(),
      type.get_series(),
      proj_params.center.lat,
      proj_params.center.lon,
      proj_params.rotation,
      proj_params.zoom_percent,
      event.info.proj);

   if (err != SUCCESS)
   {
      // log any errors we get
      LogError(MAP_PROJ, err);

      // don't return if it's a puny no-data error
      if (err == FV_NO_DATA)
         return TEST_NON_FATAL_ERROR;
      else
         return TEST_FATAL_ERROR;
   }
   else
   {
      if(event.info.proj == GENERAL_PERSPECTIVE_PROJECTION)
      {
         MSG msg;
         DWORD start_time = timeGetTime();
         while(! s_is_updating_frames)
         {
            if(GetMessage(&msg, NULL, 0, 0) > 0)
            {
               TranslateMessage(&msg);
               DispatchMessage(&msg);
            }
            if(timeGetTime() - start_time > 2000)
               break;
         }

         start_time = timeGetTime();
         while(s_is_updating_frames)
         {
            if(GetMessage(&msg, NULL, 0, 0) > 0)
            {
               TranslateMessage(&msg);
               DispatchMessage(&msg);
            }
            if(timeGetTime() - start_time > 5000)
               break;
         }
      }

      // update our globals
      gCurrentProjection = event.info.proj;
      gNumProjChanges++;
   }

   return TEST_NO_ERROR;
}


void CMapAutoTester::GenMapTilt(test_event_t& event)
{
   // Generate random tilt amount between -90 & -10
   event.info.map_tilt = (int) ((rand() % 81) - 90);
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ValMapTilt(test_event_t& event)
{
   // Make sure we are in a 3D mode
   MapView* pView = fvw_get_view();
   assert(pView != nullptr);
   if (pView)
   {
      ProjectionEnum proj = pView->GetProjectionType();
      if(proj == GENERAL_PERSPECTIVE_PROJECTION)
         return TEST_NO_ERROR;
      else
         return TEST_NON_FATAL_ERROR;
   }

   return TEST_FATAL_ERROR;
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ExecMapTilt(test_event_t event)
{
   CString buf;
   buf.Format("(%d) Attempting to change map tilt value to: %d\n", 
            gCurrentIteration, event.info.map_tilt);
   LogEntry(buf, true);

   MapView* pView = fvw_get_view();
   assert(pView != nullptr);
   if (!pView)
   {
      return TEST_FATAL_ERROR;
   }

   int status = pView->ChangeTilt((double) event.info.map_tilt);
   if (status != SUCCESS)
   {
      // log any errors we get
      LogError(MAP_TILT, status);
      return TEST_FATAL_ERROR;
   }
   else
   {
      pView->invalidate_view();
      ProjectionEnum proj = pView->GetProjectionType();
      if(proj == GENERAL_PERSPECTIVE_PROJECTION)
      {
         MSG msg;
         DWORD start_time = timeGetTime();
         while(! s_is_updating_frames)
         {
            if(GetMessage(&msg, NULL, 0, 0) > 0)
            {
               TranslateMessage(&msg);
               DispatchMessage(&msg);
            }
            if(timeGetTime() - start_time > 2000)
               break;
         }

         start_time = timeGetTime();
         while(s_is_updating_frames)
         {
            if(GetMessage(&msg, NULL, 0, 0) > 0)
            {
               TranslateMessage(&msg);
               DispatchMessage(&msg);
            }
            if(timeGetTime() - start_time > 5000)
               break;
         }
      }

      // update our globals
      gCurrent3DMapTilt = event.info.map_tilt;
      gNumTiltChanges++;
   }

   return TEST_NO_ERROR;
}


void CMapAutoTester::GenCameraMode(test_event_t& event)
{
   // Select random camera mode
   int num_selected_modes = m_selectedCameraModes.GetCount();
   if (num_selected_modes)
   {
      int index = (int) (rand() % num_selected_modes);
      POSITION p = m_selectedCameraModes.FindIndex(index);
      event.info.camera_mode = m_selectedCameraModes.GetAt(p);
   }
   else
      event.info.camera_mode = fvw::CAMERA_MODE_NONE;
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ValCameraMode(test_event_t& event)
{
   // Make sure we are in a 3D mode
   MapView* pView = fvw_get_view();
   assert(pView != nullptr);
   if (pView)
   {
      ProjectionEnum proj = pView->GetProjectionType();
      if(proj == GENERAL_PERSPECTIVE_PROJECTION)
         return TEST_NO_ERROR;
      else
         return TEST_NON_FATAL_ERROR;
   }

   return TEST_FATAL_ERROR;
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ExecCameraMode(test_event_t event)
{
   CString buf;
   buf.Format("(%d) Attempting to change camera mode to: %s\n", 
            gCurrentIteration, CameraModeName(event.info.camera_mode));
   LogEntry(buf, true);

   MapView* pView = fvw_get_view();
   assert(pView != nullptr);
   if (! pView)
   {
      return TEST_FATAL_ERROR;
   }

   // Perform the camera mode change
   pView->SetCameraMode(event.info.camera_mode);
   pView->invalidate_view();

   // Wait for the scene to catch up
   ProjectionEnum proj = pView->GetProjectionType();
   if(proj == GENERAL_PERSPECTIVE_PROJECTION)
   {
      MSG msg;
      DWORD start_time = timeGetTime();
      while(! s_is_updating_frames)
      {
         if(GetMessage(&msg, NULL, 0, 0) > 0)
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
         if(timeGetTime() - start_time > 2000)
            break;
      }

      start_time = timeGetTime();
      while(s_is_updating_frames)
      {
         if(GetMessage(&msg, NULL, 0, 0) > 0)
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
         if(timeGetTime() - start_time > 5000)
            break;
      }
   }

   // Update our globals
   gCurrentCameraMode = event.info.camera_mode;
   gNumCameraModeChanges++;

   return TEST_NO_ERROR;
}


void CMapAutoTester::GenElevExaggeration(test_event_t& event)
{
   // Generate random scale amount between 0 & 5
   event.info.height_scale = (int) (rand() % 6);
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ValElevExaggeration(test_event_t& event)
{
   // Make sure we are in a 3D mode
   MapView* pView = fvw_get_view();
   assert(pView != nullptr);
   if (pView)
   {
      ProjectionEnum proj = pView->GetProjectionType();
      if(proj == GENERAL_PERSPECTIVE_PROJECTION)
         return TEST_NO_ERROR;
      else
         return TEST_NON_FATAL_ERROR;
   }

   return TEST_FATAL_ERROR;
}


// return values: TEST_NO_ERROR, TEST_NON_FATAL_ERROR, TEST_FATAL_ERROR
int CMapAutoTester::ExecElevExaggeration(test_event_t event)
{
   CString buf;
   buf.Format("(%d) Attempting to change elevation exaggeration value to: %d\n", 
            gCurrentIteration, event.info.elev_exaggeration);
   LogEntry(buf, true);

   MapView* pView = fvw_get_view();
   assert(pView != nullptr);
   if (! pView)
   {
      return TEST_FATAL_ERROR;
   }

   // Perform the elevation exaggeration change
   GeospatialScene* scene = pView->GetGeospatialViewController()->GetGeospatialScene();
   scene->SetVerticalScale(event.info.height_scale);
   pView->invalidate_view();

   // Wait for the scene to catch up
   ProjectionEnum proj = pView->GetProjectionType();
   if(proj == GENERAL_PERSPECTIVE_PROJECTION)
   {
      MSG msg;
      DWORD start_time = timeGetTime();
      while(! s_is_updating_frames)
      {
         if(GetMessage(&msg, NULL, 0, 0) > 0)
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
         if(timeGetTime() - start_time > 2000)
            break;
      }

      start_time = timeGetTime();
      while(s_is_updating_frames)
      {
         if(GetMessage(&msg, NULL, 0, 0) > 0)
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
         if(timeGetTime() - start_time > 5000)
            break;
      }
   }

   // Update our globals
   gCurrentElevExaggeration = event.info.elev_exaggeration;
   gNumElevExaggChanges++;

   return TEST_NO_ERROR;
}
