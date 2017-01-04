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



#pragma once

/*------------------------------------------------------------------
-  FILE NAME:      mem.h
-  LIBRARY NAME:   mem
-  PROGRAMMER:     Rob Gue
-  DATE:           January 1994
-
-  DESCRIPTION:
-
-      This file is the interface to the memory library.
-
-  FUNCTIONS:
-
-      MEM_malloc  [macro]
-      MEM_realloc [macro]
-      MEM_free    [macro]
-      _MEM_malloc
-      _MEM_realloc
-      _MEM_free
-      MEM_block_ok
-
-  REVISION HISTORY: 
-      $Log: mem.h $
 * Revision 1.2  1994/10/04  19:08:51  gue
 * *** empty log message ***
 * 
 * Revision 1.2  1994/04/12  14:39:34  gue
 * Added MEM_heap_ok.
 * 
 * Revision 1.1  1994/02/26  10:56:45  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

#ifndef MEM_H
#define MEM_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include <stddef.h>
#include <stdio.h>
#include "common.h"

#define MEM_malloc(x)      _MEM_malloc(x, __FILE__, __LINE__)
#define MEM_realloc(x, y)  _MEM_realloc(x, y, __FILE__, __LINE__)
#define MEM_free(x)        _MEM_free(x, __FILE__, __LINE__)

/*------------------------------------------------------------------
-                       Function Prototypes 
-------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *  a safer replacement for malloc
 */
void *_MEM_malloc(size_t size, char *file, int line);

/*
 *  a safer replacement for realloc
 */
void *_MEM_realloc(void *ptr, size_t size, char *file, int line);

/*
 *  a safer replacement for free
 */
void _MEM_free(void *ptr, char *file, int line);

/*
 *  MEM_block_ok is for tracking down the source of memory corruption
 *  errors.  It returns TRUE if the block is ok, otherwise FALSE.
 *  Note that this function is only available when the memory library
 *  was compiled with MEM_DEBUG.
 */
boolean_t MEM_block_ok(void *ptr);

#ifdef __cplusplus
}
#endif

#endif  /* MEM_H */
