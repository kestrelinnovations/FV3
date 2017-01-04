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
-  FILE NAME:           prm_ini.c
-  LIBRARY NAME:        param
-
-  DESCRIPTION:
-
-      This file contains the functions necessary for reading in the 
-  initialization file.
-
-  PUBLIC FUNCTIONS:    
-
-      PRM_process_ini_file
-
-  PRIVATE FUNCTIONS: NONE
-
-  STATIC FUNCTIONS:  
-
-      read_line
-      process_line
-      parse_line
-      strip_trailing_slash_if_directory
-      parse_variable_name
-      parse_variable_value
-      strip_leading_white_space
-      check_for_correct_assignment
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
   
 

-  REVISION HISTORY:

 [deleted]

 * Revision 1.1  1994/02/26  12:58:31  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "param.h"
#include "err.h"
#include "file.h"
#include "paramlcl.h"

/*------------------------------------------------------------------
-                           Definitions
-------------------------------------------------------------------*/


/*------------------------------------------------------------------
-                         Static Variables
-------------------------------------------------------------------*/

/* 
 *  the number of the initialization file line currently being processed
 */
//static int line_num;

/*
 *  TRUE if HD_DATA was assigned a value when reading in ini file
 */
//static boolean_t hd_data_set = FALSE;


/*------------------------------------------------------------------
-  FUNCTION NAME:       PRM_process_ini_file
-  PROGRAMMER:          Rob Gue
-  DATE:                January 1993
-
-  PURPOSE:             
-
-      Read the initialization file and set the parameters accordingly.
-
-  PARAMETERS: NONE
-
-  RETURN VALUES:
-
-      SUCCESS
-      FAILURE
-
-  PRECONDITIONS: 
-
-
-  DESCRIPTION:
-
-------------------------------------------------------------------*/

/*
int PRM_process_ini_file(const char *ini_file_name)
{
   char value[PRM_MAX_VALUE_LENGTH + 1]; // buffer for reading ini values


   // must be set for PRM_get_value() and PRM_set_value() 
   prm_set_module_initialized(TRUE);

    
   //  test for valid PRM_HD_DATA 
    
   PRM_get_value(PRM_HD_DATA, value);
   if (FIL_access(value, FIL_EXISTS) == FAILURE)
   {
      PRM_set_value(PRM_HD_DATA, "");
      ERR_report("cannot get HD data");
      prm_set_module_initialized(FALSE);
      return FAILURE;
   }

   return SUCCESS;
}
*/
