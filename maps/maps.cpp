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
-  FILE NAME:    maps.cpp
-  LIBRARY NAME: maps.lib
-
-  DESCRIPTION:
-
-      In addition to the map library initialization and clean-up routines
-  MAP_open() and MAP_close(), this file contains all of the top level
-  map library functions which are used to display or refresh the map.
-  A map is specified by a center, source, and scale. This file contains
-  functions to operate on (or directly set) one or more of these values.
-  Then to display the new map at the new center, source, or scale, call
-  MAP_do_display().
-
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "stdafx.h"
#include "errx.h"
#include "category.h"


/*------------------------------------------------------------------
-                     Static Function Prototypes
-------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////
// printing to scales status vars + functions
///////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------
-  FUNCTION NAME: MAP_open
-
-  PURPOSE:
-
-      Do map library initialization tasks upon startup.
-
-  DESCRIPTION:
-
-      The INI file must be read in and processed before any of it's
-  values can be read, so it is done here. The graphics mode is set
-  according to the INI files graphics mode. The back and store is
-  created. The map specifications are initialize, and the map
-  projection is set up to match the default map.
-------------------------------------------------------------------*/

int MAP_open(void)
{
   if (map_create_categories() != SUCCESS)
   {
      map_destroy_categories();
      ERR_report("map_create_categories failed");
      return FAILURE;
   }

   return SUCCESS;
}


/*------------------------------------------------------------------
-  FUNCTION NAME: MAP_close
-
-  PURPOSE:
-
-      Clean up after the map library before exit.
-
-  DESCRIPTION:
-
-      Free the back and store memory and close the graphics library,
-  thus switching back to text mode.
-------------------------------------------------------------------*/

int MAP_close(void)
{
   map_destroy_categories();

   return SUCCESS;
}
