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



/////////////////////////////////////////////////////////////
// test.h

// Test routine interface definitions
// Created by David Pope  30 July 96

#ifndef _TEST_H
#define _TEST_H

#include "common.h"
#include "mapview.h"

class MapType;
class MapSource;
class MapScale;
class MapSeries;

///////////// typedefs and defines

typedef unsigned long test_flags_t;

typedef enum {
   MAP_MOVE = 0,
   MAP_SCALE,
   MAP_ZOOM,
   MAP_PROJ,
   MAP_TILT,
   MAP_ELEV_EXAGG,
   GENERAL
} test_domain_t;




////// Test flags defines

//// event-type flags

// flags used with event types must remain contiguous!
//////////////////////////////////////////////////////////////////////////////
#define TEST_3D_ELEV_EXAG    0x00002000  // Set = perform 3D elev scale tests//
                                         // Clear = don't test               //
#define TEST_3D_MAP_TILT     0x00001000  // Set = perform 3D map tilt tests  //
                                         // Clear = don't test               //
#define TEST_MAP_MOVE        0x00000800  // Set = perform map movement tests //
                                         // Clear = don't test               //
#define TEST_MAP_SCALE       0x00000400  // Set = test scaling features      //
                                         // Clear = don't test               //
#define TEST_MAP_ZOOM        0x00000200  // Set = test zooming features      //
                                         // Clear = don't test               //
#define TEST_OVL_TOGGLE      0x00000100  // Set = test overlay toggles       //
                                         // Clear = don't test               //
#define TEST_OPEN_OVL_FILE   0x00000080  // Set = test opening overlay files //
                                         // Clear = don't test               //
#define TEST_CLOSE_OVL_FILE  0x00000040  // Set = test closing overlay files //
                                         // Clear = don't test               //
#define TEST_MAP_PROJ        0x00000020  // Set = perform map projection test//
                                         // Clear = don't test               //
#define TEST_3D_CAMERA_MODES 0x00000010  // Set = perform 3D camera mode test//
                                         // Clear = don't test               //
#define TEST_TYPE_ALL        0x00003FF0                                      //
///////////////////////////////////////////////////////////////////////////////
#define NUM_EVENT_TYPES      10       // update these if # of event types changes
#define MIN_EVENT_TYPE     TEST_3D_CAMERA_MODES

//// additional info flags

#define TEST_ORDER_DATA      0x00000008  // Set   = iterate within same map type
                                         // Clear = iterate across map types
#define TEST_CONUS_ONLY      0x00000004  // Set   = Move within CONUS only
                                         // Clear = Move anywhere in world
#define TEST_INFO_UNUSED1    0x00000002
#define TEST_INFO_UNUSED2    0x00000001

//// useful conglomerations
#define TEST_MAP ( TEST_MAP_MOVE | TEST_MAP_SCALE | TEST_MAP_ZOOM | TEST_MAP_PROJ | TEST_3D_CAMERA_MODES | TEST_3D_MAP_TILT | TEST_3D_ELEV_EXAG)

#define ALL_TEST_FLAGS       0xFFFFFFFF


//// miscellaneous constants


////// function prototypes
//

//////// typedefs and internal defines

typedef int event_type_t;


typedef union event_info
{
   struct
   {
      double lat;
      double lon;
   } coord;
   MapType* pMapType;
   long zoomTo;
   int  ovlNum;
   int  testFileIndex;
   ProjectionEnum proj;
   int height_scale;
   int map_tilt;
   int elev_exaggeration;
   fvw::CameraMode camera_mode;
} event_info_t;

typedef struct event
{
   event_type_t type;
   event_info_t info;
} test_event_t;

// external functions
class CMapAutoTester
{
public:
   int            Init(void);
   void SetRandomSeed(unsigned int nSeed = 0);
   void SetWriteToErrorLog(BOOL bWriteToErrorLog);
   int            Execute(void);
   void           Finish(void);
   test_flags_t   SetFlags(test_flags_t flags);
   test_flags_t   ClearFlags(test_flags_t flags);
   void           SetIterations(UINT numIterations);

   void SetSelectedMapTypes(CList<MapType *, MapType *>& selectedMapTypes)
   {
      m_selectedMapTypes.AddTail(&selectedMapTypes);
   }

   void SetSelectedMapProjections(CList<ProjectionEnum, ProjectionEnum>& selectedMapProjections)
   {
      m_selectedMapProjections.AddTail(&selectedMapProjections);
   }

   void SetSelectedCameraModes(CList<fvw::CameraMode, fvw::CameraMode>& selectedCameraModes)
   {
      m_selectedCameraModes.AddTail(&selectedCameraModes);
   }

private:
   CList<MapType *, MapType *> m_selectedMapTypes;
   CList<ProjectionEnum, ProjectionEnum> m_selectedMapProjections;
   CList<fvw::CameraMode, fvw::CameraMode> m_selectedCameraModes;
   BOOL m_bWriteToErrorLog;

private:

   int GenerateEvent(test_event_t& event);
   void GenMapMove(test_event_t& event);
   void GenMapScale(test_event_t& event);
   void GenMapZoom(test_event_t& event);
   void GenMapProj(test_event_t& event);
   void GenOvlToggle(test_event_t& event);
   void GenOpenOvlFile(test_event_t& event);
   void GenCloseOvlFile(test_event_t& event);
   void GenMapTilt(test_event_t& event);
   void GenCameraMode(test_event_t& event);
   void GenElevExaggeration(test_event_t& event);

   int ValidateEvent(test_event_t& event);
   int ValMapMove(test_event_t& event);
   int ValMapScale(test_event_t& event);
   int ValMapZoom(test_event_t& event);
   int ValMapProj(test_event_t& event);
   int ValOvlToggle(test_event_t& event);
   int ValOpenOvlFile(test_event_t& event);
   int ValCloseOvlFile(test_event_t& event);
   int ValMapTilt(test_event_t& event);
   int ValCameraMode(test_event_t& event);
   int ValElevExaggeration(test_event_t& event);

   int ExecuteEvent(test_event_t event);
   int ExecMapMove(test_event_t event);
   int ExecMapScale(test_event_t event);
   int ExecMapZoom(test_event_t event);
   int ExecMapProj(test_event_t event);
   int ExecOvlToggle(test_event_t event);
   int ExecOpenOvlFile(test_event_t event);
   int ExecCloseOvlFile(test_event_t event);
   int ExecMapTilt(test_event_t event);
   int ExecCameraMode(test_event_t event);
   int ExecElevExaggeration(test_event_t event);

   void LogEntry(const char* str, bool bAppendTimeStamp = false);
   void LogError(test_domain_t domain, int error);

   void ForceRedraw(void);

   void DumpStatus(void);
};


#endif // _TEST_H
