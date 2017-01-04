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

#ifndef COMMON_H
#define COMMON_H 1

#include <stdlib.h>   // for _MAX_PATH

/*------------------------------------------------------------------
-                           Definitions
-------------------------------------------------------------------*/

//
// GetSystemMetrics values that are not defined in VC++ 5.0
//
#define SM_XVIRTUALSCREEN   76
#define SM_YVIRTUALSCREEN   77
#define SM_CXVIRTUALSCREEN  78
#define SM_CYVIRTUALSCREEN  79
#define SM_CMONITORS        80

/*
 *  TRUE and FALSE constants
 */
#ifndef FALSE
  #define FALSE 0
#endif
#ifndef TRUE
  #define TRUE 1
#endif

/*
 *  for functions that return indices
 */

#define INVALID_INDEX (-1)

/*
 *  return codes
 */
#define SUCCESS       0
#define FAILURE      -1
#define USER_ABORT   -2
#define FV_NO_DATA   -10
#define COV_FILE_OUT_OF_SYNC    -20   /* indicates that a file listed in a
                                         coverage list does not exist in the 
                                         proper place on a data source */
#define ECHUM_FILE_NEEDS_PARSING -21  /* one or more ECHUM files in the download
                                         directory need to be parsed */

#define FV_NOT_READY -25               // first FalconView map has not yet been drawn

#define MEMORY_ERROR -30
#define SURFACE_SIZE_TOO_LARGE  -31

#define FATAL_MAP_DISPLAY_ERROR -40
#define E_PREPARE_FOR_OUTPUT    -41

#define ROUTE_SERVER_EXCEPTION  -50    /* used by the routesvr library */
#define ROUTE_SERVER_BUSY       -51
#define ROUTE_SERVER_ERROR      -52
#define REREAD_ROUTE            -53
#define NO_READ_ONLY -60               /* attempt to open a read-only file,
                                          for an overlay that can not handle
                                          read-only files. */
#define DISPLAY_ERROR_STRING    -54


/*
 *  the maximum length of a file name (including path)
 */
#define MAX_FILE_NAME_LEN _MAX_PATH

// when 'Native' is selected in the zoom percent combo box, this is the
// value that the zoom percent will take on and is used to signify that we
// are in this special case
#define NATIVE_ZOOM_PERCENT 9999

// when 'To Scale' is selected in the zoom percent combo box, the pixels per
// inch of the screen is used to compute the degrees per pixel lat, lon
#define TO_SCALE_ZOOM_PERCENT 8888

/* 
 *  useful constants
 */
#define PI               3.14159265358979323846
#define TWO_PI           6.2831853071795862
#define HALF_PI          1.57079632679489661923
#define ONE_OVER_PI      0.31830988618379067154  /* ( 1 / PI )  */
#define TWO_OVER_PI      0.63661977236758134308  /* ( 2 / PI )  */
#define SQRT2            1.41421356237309504880
#define SQRT1_2          0.70710678118654752440
#define EULER            2.7182818284590452354
#define LOG2E            1.4426950408889634074
#define LOG10E           0.43429448190325182765
#define LN2              0.69314718055994530942
#define LN10             2.30258509299404568402

/*------------------------------------------------------------------
-                            Typedefs 
-------------------------------------------------------------------*/

/*
 *  boolean type - can be TRUE or FALSE 
 */
typedef int boolean_t;

/*
 *  ensures that a value is TRUE for true and FALSE for false rather
 *  than non-zero for true and zero for false
 */
#define TO_BOOLEAN_T(x) ((boolean_t)(x ? TRUE : FALSE))

typedef unsigned char  BYTE;
typedef signed char    INT1;
typedef signed short   INT2;
typedef signed int     INT4;
typedef unsigned char  UINT1;
typedef unsigned short UINT2;
typedef unsigned int   UINT4;
typedef float          FLOAT4;
typedef double         FLOAT8;

typedef FLOAT8 radians_t;
typedef FLOAT8 degrees_t;
typedef FLOAT8 minutes_t;

typedef DWORD				attr_word;

#endif

