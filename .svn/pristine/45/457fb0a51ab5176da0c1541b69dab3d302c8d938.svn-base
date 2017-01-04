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



/////////////////////////////////////////////////////////////////////////
// testglob.h

// Items of internal interest to the test suite routines

#ifndef _TESTGLOB_H
#define _TESTGLOB_H

#include "test.h"
#include "maps_d.h"

#include "resource.h"
#include "overlay.h"
#include "overlay\OvlMgrDl.h"
#include "common.h"
//#include <afxtempl.h>

class C_overlay_factory;

//////// extern globals

extern test_flags_t gTestFlags;

extern CStdioFile gTestLogFile;

extern int gCurrentZoom;

extern double gCurrentLat,
              gCurrentLon;

extern ProjectionEnum gCurrentProjection;

extern int gCurrentElevExaggeration;

extern fvw::CameraMode gCurrentCameraMode;

extern int gCurrent3DMapTilt;

extern long gCurrentIteration;

extern UINT gNumIterations;

extern int gNumMoves,
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

extern CString gTestFileFolderPath;

extern CTime gStartTime;

// for static overlay toggling
extern CArray<OverlayTypeDescriptor*> gSelectedStaticOvlList;  // to store the names of the static overlays selected from
                                                               // the test dialog's overlay property page list box
extern CArray<int, int> gSelectedStaticOvlCounters;  // to keep track of how many times each static
                                                     // overlay has been toggled during the test

// for overlay file opening/closing
extern CStringArray gTestFileNames;  // to store the current set of test overlay files
extern CPtrArray    gOpenOverlayPointers;  // to store pointers to open test file overlays
extern CStringArray gOpenOverlayClassTitles;  // to store class titles for open test file overlays
extern CStringArray gOpenOverlayFilePaths;  // to store file paths for open test file overlays


//  Return value defines

#define TEST_NO_ERROR 0
#define TEST_NON_FATAL_ERROR 1
#define TEST_FATAL_ERROR 2

//////// function prototypes

#endif // _TESTGLOB_H
