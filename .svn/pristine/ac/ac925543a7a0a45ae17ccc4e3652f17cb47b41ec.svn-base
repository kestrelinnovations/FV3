#ifndef _SHAPEFILE_H_INCLUDED
#define _SHAPEFILE_H_INCLUDED

/******************************************************************************
 * $Id: shapefil.h,v 1.20 2001/07/20 13:06:02 warmerda Exp $
 *
 * Project:  Shapelib
 * Purpose:  Primary include file for Shapelib.
 * Author:   Frank Warmerdam, warmerda@home.com
 *
 ******************************************************************************
 * Copyright (c) 1999, Frank Warmerdam
 *
 * This software is available under the following "MIT Style" license,
 * or at the option of the licensee under the LGPL (see LICENSE.LGPL).  This
 * option is discussed in more detail in shapelib.html.
 *
 * --
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * 12-Sep-2011 (GTRI)  Add shape bias min/max
 * Nov-2013 (GTRI)  Update label processing
 *
 * $Log: shapefil.h,v $
 * Revision 1.20  2001/07/20 13:06:02  warmerda
 * fixed SHPAPI attribute for SHPTreeFindLikelyShapes
 *
 * Revision 1.19  2001/05/31 19:20:13  warmerda
 * added DBFGetFieldIndex()
 *
 * Revision 1.18  2001/05/31 18:15:40  warmerda
 * Added support for NULL fields in DBF files
 *
 * Revision 1.17  2001/05/23 13:36:52  warmerda
 * added use of SHPAPI_CALL
 *
 * Revision 1.16  2000/09/25 14:15:59  warmerda
 * added DBFGetNativeFieldType()
 *
 * Revision 1.15  2000/02/16 16:03:51  warmerda
 * added null shape support
 *
 * Revision 1.14  1999/11/05 14:12:05  warmerda
 * updated license terms
 *
 * Revision 1.13  1999/06/02 18:24:21  warmerda
 * added trimming code
 *
 * Revision 1.12  1999/06/02 17:56:12  warmerda
 * added quad'' subnode support for trees
 *
 * Revision 1.11  1999/05/18 19:11:11  warmerda
 * Added example searching capability
 *
 * Revision 1.10  1999/05/18 17:49:38  warmerda
 * added initial quadtree support
 *
 * Revision 1.9  1999/05/11 03:19:28  warmerda
 * added new Tuple api, and improved extension handling - add from candrsn
 *
 * Revision 1.8  1999/03/23 17:22:27  warmerda
 * Added extern "C" protection for C++ users of shapefil.h.
 *
 * Revision 1.7  1998/12/31 15:31:07  warmerda
 * Added the TRIM_DBF_WHITESPACE and DISABLE_MULTIPATCH_MEASURE options.
 *
 * Revision 1.6  1998/12/03 15:48:15  warmerda
 * Added SHPCalculateExtents().
 *
 * Revision 1.5  1998/11/09 20:57:16  warmerda
 * Altered SHPGetInfo() call.
 *
 * Revision 1.4  1998/11/09 20:19:33  warmerda
 * Added 3D support, and use of SHPObject.
 *
 * Revision 1.3  1995/08/23 02:24:05  warmerda
 * Added support for reading bounds.
 *
 * Revision 1.2  1995/08/04  03:17:39  warmerda
 * Added header.
 *
 */

#include <stdio.h>
#include "HeapUtilities.h"
#include <map>
#include <vector>
#include "maps_d.h"

#ifdef USE_DBMALLOC
#  include <dbmalloc.h>
#endif

#if UINT_MAX == 65535
   typedef long int32;
#else
   typedef int int32;
#endif

#ifndef NO_FAST_SHAPES_CLEAR
#  define FAST_SHAPES_CLEAR
#endif

#ifndef SHPAPI_CALL
#  define SHPAPI_CALL
#endif

#define NEW_MAX( maxval, testval ) { if ( (testval) > maxval ) maxval = (testval); }
#define NEW_MIN( minval, testval ) { if ( (testval) < minval ) minval = (testval); }

#define SHPAPI_CALL1(x)      * SHPAPI_CALL

/************************************************************************/
/*                        Configuration options.                        */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*      Should the DBFReadStringAttribute() strip leading and           */
/*      trailing white space?                                           */
/* -------------------------------------------------------------------- */
#define TRIM_DBF_WHITESPACE

/* -------------------------------------------------------------------- */
/*      Should we write measure values to the Multipatch object?        */
/*      Reportedly ArcView crashes if we do write it, so for now it     */
/*      is disabled.                                                    */
/* -------------------------------------------------------------------- */
#define DISABLE_MULTIPATCH_MEASURE

/************************************************************************/
/*                             SHP Support.                             */
/************************************************************************/
typedef struct
{
    FILE        *fpSHP;
    FILE *fpSHX;

    int nShapeType;  /* SHPT_* */

    int nFileSize;  /* SHP file */

    int         nRecords;
    int nMaxRecords;
    int *panRecOffset;
    int *panRecSize;

    double adBoundsMin[4];
    double adBoundsMax[4];

    int bUpdated;
} SHPInfo;

typedef SHPInfo * SHPHandle;

/* -------------------------------------------------------------------- */
/*      Shape types (nSHPType)                                          */
/* -------------------------------------------------------------------- */
#define SHPT_NULL 0
#define SHPT_POINT 1
#define SHPT_ARC 3
#define SHPT_POLYGON 5
#define SHPT_MULTIPOINT 8
#define SHPT_POINTZ 11
#define SHPT_ARCZ 13
#define SHPT_POLYGONZ 15
#define SHPT_MULTIPOINTZ 18
#define SHPT_POINTM 21
#define SHPT_ARCM 23
#define SHPT_POLYGONM 25
#define SHPT_MULTIPOINTM 28
#define SHPT_MULTIPATCH 31


/* -------------------------------------------------------------------- */
/*      Part types - everything but SHPT_MULTIPATCH just uses           */
/*      SHPP_RING.                                                      */
/* -------------------------------------------------------------------- */

#define SHPP_TRISTRIP 0
#define SHPP_TRIFAN 1
#define SHPP_OUTERRING 2
#define SHPP_INNERRING 3
#define SHPP_FIRSTRING 4
#define SHPP_RING 5



// SHPDisplayProperties - stores display properties used for a SHPObject
//
class CIconImage;
struct SHPDisplayProperties
{
   // true if the background should be displayed
   BOOL background;
   COLORREF color_rgb;
   int width;
   int fill_style;
   int line_style;
   int diameter;
   BOOL use_icon;
   CIconImage* pIconImage;

   SHPDisplayProperties::SHPDisplayProperties() :
   pIconImage(nullptr)
   {
   }

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Note: the HeapUtilitY:: forms of the std:: collections are used here in order to delete a shape file load
// by simply destroying the private heap.  None of the collections have non-trival members and can be just
// abandoned.  The standard free() deallocators are too slow when possibly millions of buffers are involved.
// For instance, deleting the test file GeorgiaHighways.shp can take over an hour on a reasonably modest machine.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SHPPart
{
   int part_type;
   int part_start;
   char bias_max;
   char bias_min;

   // Mark part as not drawn
   SHPPart() : part_type( SHPP_RING ), bias_min( +127 ), bias_max( 0 ){}
}; // SHPPart


struct SHPVertex : public d_geo_t
{
   SHPVertex(){}  // For POD initialization
}; // SHPVertex


struct SHPCentroid : public d_geo_t
{
public:
   SHPCentroid()
   { lat = -1000.0; }
}; // SHPCentroid

struct PolygonLabelInfo
{
public:
   SHPCentroid m_gptCentroid;          // Centroid of polygon
   int         m_iLabelX, m_iLabelY;   // Center of label
};


/* -------------------------------------------------------------------- */
/*      SHPObject - represents one shape (without attributes) read      */
/*      from the .shp file.                                             */
/* -------------------------------------------------------------------- */
struct SHPObject
{
    int           nSHPType;
    enum { FEATURE_NONE = 0, FEATURE_M = 1 << 0, FEATURE_Z = 1 << 1,
      FEATURE_POLYGON = 1 << 2, FEATURE_POLYLINE = 1 << 3, FEATURE_POINT = 1 << 4,
      FEATURE_RECORD_ERROR = 1 << 5 };
    UINT          m_mFeatureMask;
    int           nShapeId; /* -1 is unknown/unassigned */

    LPBYTE        m_pbyShapeData;
    d_geo_rect_t  m_grcBounds;

    size_t        m_cParts;
    size_t        m_cVertices;

    double        dfZMin;
    double        dfMMin;
    double        dfZMax;
    double        dfMMax;

   // Memory for the screen coordinates will be allocated a single time and
   // reused each draw
   std::vector< POINT, HeapUtility::HeapableAllocator< POINT > > m_screen_coordinates;

   LPCH     m_pchShapeStrings;
   WORD     m_wHelpTextOffset;
   WORD     m_wLabelOffset;

    // used to sort shapes by the current filter settings
    int displayOrder;

    // do not de-allocate this pointer - it is owned by either the overlay or an individual filter
    SHPDisplayProperties* pDisplayProperties;

   friend class SHPObjectPtr;

   SHPObject( HeapUtility::CHeapAllocator* pHeapAllocator ) :
      m_iRefCount( 0 ),
      displayOrder( -1 ),
      m_pHeapAllocator( pHeapAllocator ),
      m_pbyShapeData( nullptr ),
      dfZMin( 0.0 ), dfZMax( 0.0 ), dfMMin( 0.0 ), dfMMax( 0.0 ),
      m_screen_coordinates( pHeapAllocator ),
      m_pchShapeStrings( nullptr )
      {}

   SHPObject() :
      m_iRefCount( 0 ),
      m_pHeapAllocator( NULL ),
      displayOrder( -1 ),
      m_pbyShapeData( nullptr ),
      dfZMin( 0.0 ), dfZMax( 0.0 ), dfMMin( 0.0 ), dfMMax( 0.0 ),
      m_screen_coordinates( nullptr ),
      m_pchShapeStrings( nullptr )
      {}
      ~SHPObject();

      SHPVertex* GetVertices();
      SHPVertex* GetVertices( size_t iPart );
      SHPPart& GetPart( size_t iPart );
      size_t GetPartSize( size_t iPart );
      d_geo_rect_t& GetGeoBounds( size_t iPart );
      int* GetPolygonScreenBounds();
      PolygonLabelInfo& GetPolygonLabelInfo( size_t iPart );
      DOUBLE* GetMs();
      DOUBLE* GetZs();
      LPCSTR GetToolTip() const;
      LPCSTR GetHelpText() const;
      LPCSTR GetLabel() const;
      BOOL IsToolTipEmpty() const;
      BOOL IsLabelEmpty() const;
      VOID ClearStrings();

private:
   int m_iRefCount;   // For smart pointer
   HeapUtility::CHeapAllocator* m_pHeapAllocator;

}; // SHPObject


// Smart pointer to SHPObject
class SHPObjectPtr
{
private:
   SHPObject* m_pShapeObj;
   void release();

public:
   SHPObjectPtr() : m_pShapeObj( NULL ) {}
   SHPObjectPtr( SHPObject* pShapeObj );
   SHPObjectPtr( const SHPObjectPtr& spShapeObj );
   SHPObjectPtr& operator=( const SHPObjectPtr& spShapeObj );
   SHPObject* get() const { return m_pShapeObj; }
   SHPObject& operator*() const { return *m_pShapeObj; }
   SHPObject* operator->() const { return m_pShapeObj; }
   SHPObject* detach();
    ~SHPObjectPtr();
};  // class SHPObjectPtr
typedef std::map< int, SHPObjectPtr > SHPObjectMap;
typedef SHPObjectMap::iterator SHPObjectMapIter;
typedef std::vector< SHPObjectPtr > SHPObjectVec;


// Level-of-detail shapes
class SHPFileLoD
{
public:
   SHPObjectMap m_mpCachedShapes;
   d_geo_t m_gptCacheLL;
   d_geo_t m_gptCacheUR;
   SHPFileLoD() { clear(); }
   void clear();
};
typedef std::map< int, SHPFileLoD > SHPFileLoDMap;
typedef SHPFileLoDMap::iterator SHPFileLoDMapIter;


/* -------------------------------------------------------------------- */
/*      SHP API Prototypes                                              */
/* -------------------------------------------------------------------- */
SHPHandle SHPAPI_CALL
      SHPOpen( const char * pszShapeFile, const char * pszAccess );
SHPHandle SHPAPI_CALL
      SHPCreate( const char * pszShapeFile, int nShapeType );
void SHPAPI_CALL
      SHPGetInfo( SHPHandle hSHP, int * pnEntities, int * pnShapeType,
                  double * padfMinBound, double * padfMaxBound );

SHPObject SHPAPI_CALL1(*)
      SHPReadObject( SHPHandle hSHP, int iShape );
int SHPAPI_CALL
      SHPWriteObject( SHPHandle hSHP, int iShape, SHPObject * psObject );

void SHPAPI_CALL
      SHPDestroyObject( SHPObject * psObject );
void SHPAPI_CALL
      SHPComputeExtents( SHPObject * psObject );
SHPObject SHPAPI_CALL1(*)
      SHPCreateObject( int nSHPType, int nShapeId,
                       int nParts, int * panPartStart, int * panPartType,
                       int nVertices, double * padfX, double * padfY,
                       double * padfZ, double * padfM );
SHPObject SHPAPI_CALL1(*)
      SHPCreateSimpleObject( int nSHPType, int nVertices,
                             double * padfX, double * padfY, double * padfZ );

void SHPAPI_CALL
      SHPClose( SHPHandle hSHP );

const char SHPAPI_CALL1(*)
      SHPTypeName( int nSHPType );
const char SHPAPI_CALL1(*)
      SHPPartTypeName( int nPartType );

/* -------------------------------------------------------------------- */
/*      Shape quadtree indexing API.                                    */
/* -------------------------------------------------------------------- */

/* this can be two or four for binary or quad tree */
#define MAX_SUBNODE 4

typedef struct shape_tree_node
{
    /* region covered by this node */
    double adfBoundsMin[4];
    double adfBoundsMax[4];

    /* list of shapes stored at this node.  The papsShapeObj pointers
       or the whole list can be NULL */
    int nShapeCount;
    int *panShapeIds;
    SHPObject   **papsShapeObj;

    int nSubNodes;
    struct shape_tree_node *apsSubNode[MAX_SUBNODE];

} SHPTreeNode;

typedef struct
{
    SHPHandle   hSHP;

    int nMaxDepth;
    int nDimension;

    SHPTreeNode *psRoot;
} SHPTree;

SHPTree SHPAPI_CALL1(*)
      SHPCreateTree( SHPHandle hSHP, int nDimension, int nMaxDepth,
                     double *padfBoundsMin, double *padfBoundsMax );
void    SHPAPI_CALL
      SHPDestroyTree( SHPTree * hTree );

int SHPAPI_CALL
      SHPWriteTree( SHPTree *hTree, const char * pszFilename );
SHPTree SHPAPI_CALL
      SHPReadTree( const char * pszFilename );

int SHPAPI_CALL
      SHPTreeAddObject( SHPTree * hTree, SHPObject * psObject );
int SHPAPI_CALL
      SHPTreeAddShapeId( SHPTree * hTree, SHPObject * psObject );
int SHPAPI_CALL
      SHPTreeRemoveShapeId( SHPTree * hTree, int nShapeId );

void SHPAPI_CALL
      SHPTreeTrimExtraNodes( SHPTree * hTree );

int    SHPAPI_CALL1(*)
      SHPTreeFindLikelyShapes( SHPTree * hTree,
                               double * padfBoundsMin,
                               double * padfBoundsMax,
                               int * );
int     SHPAPI_CALL
      SHPCheckBoundsOverlap( double *, double *, double *, double *, int );

/************************************************************************/
/*                             DBF Support.                             */
/************************************************************************/
typedef struct
{
    FILE *fp;

    int         nRecords;

    int nRecordLength;
    int nHeaderLength;
    int nFields;
    int *panFieldOffset;
    int *panFieldSize;
    int *panFieldDecimals;
    char *pachFieldType;

    char *pszHeader;

    int nCurrentRecord;
    int bCurrentRecordModified;
    char *pszCurrentRecord;

    int bNoHeader;
    int bUpdated;
} DBFInfo;

typedef DBFInfo * DBFHandle;

typedef enum {
  FTString,
  FTInteger,
  FTDouble,
  FTInvalid
} DBFFieldType;

#define XBASE_FLDHDR_SZ       32

DBFHandle SHPAPI_CALL
      DBFOpen( const char * pszDBFFile, const char * pszAccess );
DBFHandle SHPAPI_CALL
      DBFCreate( const char * pszDBFFile );

int SHPAPI_CALL
      DBFGetFieldCount( DBFHandle psDBF );
int SHPAPI_CALL
      DBFGetRecordCount( DBFHandle psDBF );
int SHPAPI_CALL
      DBFAddField( DBFHandle hDBF, const char * pszFieldName,
                   DBFFieldType eType, int nWidth, int nDecimals );

DBFFieldType SHPAPI_CALL
      DBFGetFieldInfo( DBFHandle psDBF, int iField,
                       char * pszFieldName, int * pnWidth, int * pnDecimals );

int SHPAPI_CALL
      DBFGetFieldIndex(DBFHandle psDBF, const char *pszFieldName);

int SHPAPI_CALL
      DBFReadIntegerAttribute( DBFHandle hDBF, int iShape, int iField );
double SHPAPI_CALL
      DBFReadDoubleAttribute( DBFHandle hDBF, int iShape, int iField );
const char SHPAPI_CALL1(*)
      DBFReadStringAttribute( DBFHandle hDBF, int iShape, int iField );
int     SHPAPI_CALL
      DBFIsAttributeNULL( DBFHandle hDBF, int iShape, int iField );

int SHPAPI_CALL
      DBFWriteIntegerAttribute( DBFHandle hDBF, int iShape, int iField,
                                int nFieldValue );
int SHPAPI_CALL
      DBFWriteDoubleAttribute( DBFHandle hDBF, int iShape, int iField,
                               double dFieldValue );
int SHPAPI_CALL
      DBFWriteStringAttribute( DBFHandle hDBF, int iShape, int iField,
                               const char * pszFieldValue );
int SHPAPI_CALL
     DBFWriteNULLAttribute( DBFHandle hDBF, int iShape, int iField );

const char SHPAPI_CALL1(*)
      DBFReadTuple(DBFHandle psDBF, int hEntity );
int SHPAPI_CALL
      DBFWriteTuple(DBFHandle psDBF, int hEntity, void * pRawTuple );

DBFHandle SHPAPI_CALL
      DBFCloneEmpty(DBFHandle psDBF, const char * pszFilename );

void SHPAPI_CALL
      DBFClose( DBFHandle hDBF );
char    SHPAPI_CALL
      DBFGetNativeFieldType( DBFHandle hDBF, int iField );

#endif /* ndef _SHAPEFILE_H_INCLUDED */
