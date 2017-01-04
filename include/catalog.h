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



/*------------------------------------------------------------------
-  FILE NAME:         catalog.h
-  LIBRARY NAME:      catalog
-
-  DESCRIPTION:
-
-      The purpose of the Map Catalog Module is to provide a set of high level
-  routines to display CMS and TIROS map data coverage over a base map, and
-  to allow users to control what map data is on their hard disk with a set
-  of tools which copy map files from the CD-ROM to the hard disk and delete
-  files from the hard disk.
-
-------------------------------------------------------------------*/

#ifndef CATALOG_H
#define CATALOG_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"
#include "overlay.h"
#include "geo_tool_d.h"

class ActiveMap;
class MapProj;

//	Map Data Manager modes and select options.
// bit combinations are critical as sometimes get OR'ed
#define MDM_SINGLE_MODE			0x0000
#define MDM_MULTIPLE_MODE		0x0001
#define MDM_MULTIPLE_HIDEMODE	0x0002
#define COPY_MODE					0x0010
#define DELETE_MODE				0x0020
#define COMPATIBLE_MODE       0x0030
#define ABORT_MODE            0x0040
#define UNSELECT_MODIFIER     0x1000
#define UNSELECT_SOURCE_MODE  (COPY_MODE|UNSELECT_MODIFIER)
#define UNSELECT_TARGET_MODE  (DELETE_MODE|UNSELECT_MODIFIER)
#define UNSELECT_ALL_MODE     (COPY_MODE|DELETE_MODE|UNSELECT_MODIFIER)

// Displays the Paths tab of the Map Data Manager dialog as a modal dialog
// and synchronizes the source list if OK is pressed.
int CAT_data_paths();


//###############################################################################
//		STUBS for old code
//###############################################################################

enum unit_t {_NM, MILE, KILOMETER, METERS, _YARDS, _FEET, INCHES, 
      ARC_DEGREES, ARC_MINUTES, ARC_SECONDS, NO_UNIT};


#endif
