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

// tm_shp.h -- shape file overlay header

#ifndef TM_SHP_H
#define TM_SHP_H 1

#include "geo_tool_d.h"
#include "map.h"
#include "MAPS_D.H"
#include "DBASE.H"

typedef unsigned char uchar;

/************************************************************************/
/*                             SHP Support.                             */
/************************************************************************/
typedef  struct
{
    FILE        *fpSHP;
    FILE       *fpSHX;

    int        nShapeType;    /* SHPT_* */
    
    int        nFileSize;     /* SHP file */

    int         nRecords;
    int     nMaxRecords;
    int     *panRecOffset;
    int     *panRecSize;

    double  adBoundsMin[4];
    double  adBoundsMax[4];

    int     bUpdated;
} shp_info_t;

typedef shp_info_t * shp_handle;

/* -------------------------------------------------------------------- */
/*      Shape types (nSHPType)                                          */
/* -------------------------------------------------------------------- */
#define SHPT_NULL 0
#define SHPT_POINT   1
#define SHPT_ARC  3
#define SHPT_POLYGON 5
#define SHPT_MULTIPOINT 8
#define SHPT_POINTZ  11
#define SHPT_ARCZ 13
#define SHPT_POLYGONZ   15
#define SHPT_MULTIPOINTZ 18
#define SHPT_POINTM  21
#define SHPT_ARCM 23
#define SHPT_POLYGONM   25
#define SHPT_MULTIPOINTM 28
#define SHPT_MULTIPATCH 31


/* -------------------------------------------------------------------- */
/*      Part types - everything but SHPT_MULTIPATCH just uses           */
/*      SHPP_RING.                                                      */
/* -------------------------------------------------------------------- */

#define SHPP_TRISTRIP   0
#define SHPP_TRIFAN  1
#define SHPP_OUTERRING  2
#define SHPP_INNERRING  3
#define SHPP_FIRSTRING  4
#define SHPP_RING 5

/* -------------------------------------------------------------------- */
/*      shp_obj_t - represents on shape (without attributes) read       */
/*      from the .shp file.                                             */
/* -------------------------------------------------------------------- */
typedef struct
{
    int     nSHPType;

    int     nShapeId; /* -1 is unknown/unassigned */

    int     nParts;
    int     *panPartStart;
    int     *panPartType;
    
    int     nVertices;
    double  *padfX;
    double  *padfY;
    double  *padfZ;
    double  *padfM;

    double  dfXMin;
    double  dfYMin;
    double  dfZMin;
    double  dfMMin;

    double  dfXMax;
    double  dfYMax;
    double  dfZMax;
    double  dfMMax;
} shp_obj_t;


#define HANDLE_NONE  0
#define HANDLE_UPPER_LEFT  1
#define HANDLE_UPPER_RIGHT 2
#define HANDLE_LOWER_RIGHT 3
#define HANDLE_LOWER_LEFT  4
#define HANDLE_IMG         5

#define TYPE_MAP  1
#define TYPE_FILE  2

typedef struct
{
   double ll_lat;
   double ll_lon;
   double ur_lat;
   double ur_lon;
} bounds_t;

typedef shp_info_t * shp_handle;

// ********************************************************************

// ********************************************************************
// ********************************************************************

class C_tm_shp_read
{
public:
   C_tm_shp_read();
   ~C_tm_shp_read();

   uchar   *pabyRec;
   int  nBufSize;

   shp_handle SHPOpen( const char * pszShapeFile, const char * pszAccess );
   void SHPGetInfo( shp_handle hSHP, int * pnEntities, int * pnShapeType,
      double * padfMinBound, double * padfMaxBound );
   void  SHPClose( shp_handle hSHP );

   // Read only the necessary elements of a shape file record without having
   // to allocate a 104 byte shp_obj_t
   int SHPReadObjectLatLonZ(shp_handle psSHP, int hEntity,
      double* lat, double* lon, double* z);

   int get_cdb_data(CDbase *dbf, int recnum,
      int cnam_index, CString & cnam,
      int ao1_index, double *ao1,
      int scalx_index = -1, double* scale_x = nullptr,
      int scaly_index = -1, double* scale_y = nullptr,
      int scalz_index = -1, double* scale_z = nullptr);

   int get_cnam(CDbase* cdb, int recnum, int cnam_index, CString& cnam);
};


// values for m_shp_mode
#define MM_SHP_NONE        0
#define MM_SHP_SELECT      1
#define MM_SHP_POINT       2
#define MM_SHP_LINE        3
#define MM_SHP_POLYLINE    4
#define MM_SHP_POLYGON     5
#define MM_SHP_FREEHAND    6

#define SHP_FILTER_NONE  0
#define SHP_FILTER_LIST  1
#define SHP_FILTER_RANGE 2

#define SHP_FILTER_OP_AND 1
#define SHP_FILTER_OP_OR  2

#endif   // #ifndef SHP_H
