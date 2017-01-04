// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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
-  FILE NAME:          prm_val.c
-  LIBRARY NAME:       param
-
-  DESCRIPTION:   
-
-      Provides functions for getting and setting parameter values.
-
-  PUBLIC FUNCTIONS:   
-
-      PRM_get_value
-
-  PRIVATE FUNCTIONS:  
-
-
-  STATIC FUNCTIONS: 
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
-
-  REVISION HISTORY:
-
   
 [deleted]
   
 * Revision 1.1  1994/02/26  12:58:16  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "StdAfx.h"
#include "err.h"
#include "param.h"

//------------------------------------------------------------------

int PRM_get_value(const char *var_name, char *value, int value_len)
{

   if (strcmp(var_name, "HD_DATA") !=0)
   {
      ASSERT(0);
      ERR_report("New behavior: this function can only be used for HD_DATA");
      value[0]='\0';
      return FAILURE;
   }

   /* get value from INI file */
   CString temp;
   temp = PRM_get_registry_string("Main", var_name, value);
   strcpy_s(value,value_len,temp.GetBuffer(temp.GetLength()+1));
   //no need to release buffer since buffer was not modified
   
   return SUCCESS;
}



