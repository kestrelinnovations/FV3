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


#ifndef SCENE_MGR_H
#define SCENE_MGR_H

#include "SMSheet.h"


/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"
#include "overlay.h"
#include "geo_tool_d.h"

class ActiveMap;
class MapProj;

// Map Data Manager modes and select options.
// bit combinations are critical as sometimes get OR'ed
#define SM_SINGLE_MODE           0x0000
#define SM_MULTIPLE_MODE         0x0001
#define SM_MULTIPLE_HIDEMODE     0x0002
#define SM_COPY_MODE             0x0010
#define SM_DELETE_MODE           0x0020
#define SM_COMPATIBLE_MODE       0x0030
#define SM_ABORT_MODE            0x0040
#define SM_UNSELECT_MODIFIER     0x1000
#define SM_UNSELECT_SOURCE_MODE  (SM_COPY_MODE|SM_UNSELECT_MODIFIER)
#define SM_UNSELECT_TARGET_MODE  (SM_DELETE_MODE|SM_UNSELECT_MODIFIER)
#define SM_UNSELECT_ALL_MODE     (SM_COPY_MODE|SM_DELETE_MODE|SM_UNSELECT_MODIFIER)



namespace scene_mgr
{

// Displays the Paths tab of the Scene Data Manager dialog as a modal dialog
// and synchronizes the source list if OK is pressed.
int sm_data_paths();


//###############################################################################
//    STUBS for old code
//###############################################################################

#if 0  // RP
enum unit_t {_NM, MILE, KILOMETER, METERS, _YARDS, _FEET, INCHES, 
      ARC_DEGREES, ARC_MINUTES, ARC_SECONDS, NO_UNIT};
#endif

}; // namespace scene_mgr

#endif  // #ifndef SCENE_MGR_H