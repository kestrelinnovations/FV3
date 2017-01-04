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
-  FILE NAME:          mem.c
-  LIBRARY NAME:       mem
-
-  DESCRIPTION:
-
-      This file provides equivalent to the standard library memory functions
-  malloc, calloc, realloc, and free.  If this file is compiled with MEM_DEBUG
-  defined, the library will have debugging capabilities.  Otherwise it just 
-  calls the aforementioned standard library functions.
-
-  PUBLIC FUNCTIONS:
-
-      _MEM_malloc
-      _MEM_realloc
-      _MEM_free
-      MEM_block_ok
-
-  PRIVATE FUNCTIONS: NONE
-
-  STATIC FUNCTIONS: NONE
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
-
-  REVISION HISTORY:
-       $Log: mem.cpp $
//Revision 1.2  1995/01/26  17:30:56  gue
//#define MEM_DEBUG so will always be debug
//
//Revision 1.1  1994/10/26  09:25:23  gue
//Initial revision
//
 * Revision 1.5  1994/05/16  11:26:41  gue
 * corrected typo
 * 
 * Revision 1.4  1994/05/16  10:41:43  gue
 * MEM_malloc, MEM_calloc, MEM_realloc: changed so that is any of the calls
 * returns NULL, a call is made to the error library to set a flag indicating
 * that a memory allocation error occurred.
 * 
 * Revision 1.3  1994/04/12  14:40:34  gue
// * Changed #include "memory.h" to #include "malloc.h"
 * 
 * Revision 1.2  1994/04/12  14:38:30  gue
 * Added MEM_heap_ok.
 * 
 * Revision 1.1  1994/02/26  10:54:41  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/


#include "stdafx.h"
#include "mem.h"
#include "err.h"

/*------------------------------------------------------------------
-  FUNCTION NAME:      _MEM_malloc
-  PROGRAMMER:         Rob Gue
-  DATE:               January 1994
-
-  PURPOSE:
-
-      Provide a malloc function with debugging capabilities.
-
-  PARAMETERS:
-
-      size:          size of the desired block (in bytes)
-
-      file:          file name of the file in which _MEM_malloc was called
-
-      line:          line number in file at which _MEM_malloc was called
-
-  RETURN VALUES:
-
-      pointer to a block of memory of the desired size
-      NULL
-
-  PRECONDITIONS: NONE
-
-  EXTERNALS MODIFIED: NONE
-
-  REQUIRED INCLUDES:
-
-      <stdlib.h>
-      mem_lcl.h
-
-  DESCRIPTION:
-
-      If MEM_DEBUG is defined, a "safe" version of malloc is used. Otherwise
-  the function is equivalent to the standard library malloc call.
-------------------------------------------------------------------*/

void *_MEM_malloc(size_t size, char *file, int line)
{
   void *ptr = malloc(size);

   /*
    *  if ptr is NULL, set the flag in the error library indicating that a 
    *  memory allocation error occurred
    */
   if (ptr == NULL)
      ERR_set_mem_error_occurred(TRUE);

   return ptr;
}

/*------------------------------------------------------------------
-  FUNCTION NAME:      _MEM_realloc
-  PROGRAMMER:         Rob Gue
-  DATE:               January 1994
-
-  PURPOSE:
-
-      Provide a realloc function with debugging capabilities.
-
-  PARAMETERS:
-
-      ptr:           pointer to the block of memory to free
-
-      size:          size of the desired block
-
-      file:          file name of the file in which _MEM_realloc was called
-
-      line:          line number in file at which _MEM_realloc was called
-
-  RETURN VALUES:
-
-      pointer to a block of memory of the desired size
-      NULL
-
-  PRECONDITIONS: NONE
-
-  EXTERNALS MODIFIED: NONE
-
-  REQUIRED INCLUDES:
-
-      <stdlib.h>
-      mem_lcl.h
-
-  DESCRIPTION:
-
-      If MEM_DEBUG is defined, a "safe" version of realloc is used. Otherwise
-  the function is equivalent to the standard library realloc call.
-------------------------------------------------------------------*/

void *_MEM_realloc(void *ptr, size_t size, char *file, int line)
{
   void *blk = realloc(ptr, size);

   /*
    *  if ptr is NULL, set the flag in the error library indicating that a 
    *  memory allocation error occurred
    */
   if (blk == NULL)
      ERR_set_mem_error_occurred(TRUE);

   return blk;
}

/*------------------------------------------------------------------
-  FUNCTION NAME:      _MEM_free
-  PROGRAMMER:         Rob Gue
-  DATE:               January 1994
-
-  PURPOSE:
-
-      Provide a free function with debugging capabilities.
-
-  PARAMETERS:
-
-      ptr:              pointer to the block of memory to free
-
-      file:             file name of the file in which _MEM_free was called
-
-      line:             line number in file at which _MEM_free was called
-
-  RETURN VALUES: NONE
-
-  PRECONDITIONS: NONE
-
-  EXTERNALS MODIFIED: NONE
-
-  REQUIRED INCLUDES:
-
-      <stdlib.h>
-      mem_lcl.h
-
-  DESCRIPTION:
-
-      If MEM_DEBUG is defined, a "safe" version of free is used. Otherwise
-  the function is equivalent to the standard library free call.
-------------------------------------------------------------------*/

void _MEM_free(void *ptr, char *file, int line)
{
   free(ptr);
}