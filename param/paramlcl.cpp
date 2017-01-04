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
-  FILE NAME:          paramlcl.c
-  LIBRARY NAME:       param
-
-  DESCRIPTION:        
-
-      This file contains functions concerning the initialization of the 
-  param module.  These functions are private to the param module.
-
-  PUBLIC FUNCTIONS: NONE
-
-  PRIVATE FUNCTIONS:  
-
-      PRM_get_module_initialiazed
-      PRM_set_module_initialized
-
-  STATIC FUNCTIONS: NONE
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
-
-  REVISION HISTORY:
-       $Log: paramlcl.c $

  [deleted]
  
 * Revision 1.1  1994/02/26  12:57:52  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include <stddef.h>
#include "common.h"
#include "paramlcl.h"

/*------------------------------------------------------------------
-                         Static Variables
-------------------------------------------------------------------*/


/* 
 *  TRUE if the initialization file has been read in. FALSE otherwise. 
 */

/*
static boolean_t module_initialized=FALSE;


boolean_t prm_get_module_initialized(void)
{
   return module_initialized;
}

void prm_set_module_initialized(boolean_t initialized_value)
{
   module_initialized = initialized_value;
}
*/
