// Copyright (c) 1994-2009,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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

// shpread.h

#ifndef SHPREAD_H
#define SHPREAD_H

#include <afxtempl.h> // for CList
#include "dbase.h"
#include "map.h"
#include "shp.h"
#include "shapefil.h"


#define HANDLE_NONE  0
#define HANDLE_UPPER_LEFT  1
#define HANDLE_UPPER_RIGHT 2
#define HANDLE_LOWER_RIGHT 3
#define HANDLE_LOWER_LEFT  4
#define HANDLE_IMG 5

#define TYPE_MAP  1
#define TYPE_FILE  2

typedef struct
{
   double ll_lat;
   double ll_lon;
   double ur_lat;
   double ur_lon;
} shp_bounds_t;

typedef SHPInfo * SHPHandle;

// ********************************************************************

// ********************************************************************
// ********************************************************************

class CShapeRead
{
public:
   CShapeRead( CBaseShapeFileOverlay* pShpOvl ) : m_pShpOvl( pShpOvl ),
            m_pgptCaptureBndLL( nullptr ), m_pgptCaptureBndUR( nullptr ){}
   ~CShapeRead();

   int open( __in ViewMapProj* map, __in const CString& filename, __out std::string& error_txt,
            __inout SHPObjectMap& mpShpObjects,
            __in const d_geo_t& target_bnd_ll, __in const d_geo_t& target_bnd_ur,   // Required data bounds
            __inout d_geo_t& cache_bnd_ll, __inout d_geo_t& cache_bnd_ur,     // Previous (cached) and updated data bounds
            __in bool trim_cache,                                             
            __out d_geo_t* pactive_bnd_ll = nullptr, __out d_geo_t* pactive_bnd_ur = nullptr,     // Bounds of data found in search_bnd
            __out d_geo_t* pfile_bnd_ll = nullptr, __out d_geo_t* pfile_bnd_ur = nullptr );       // File data bounds

   int open( __in ViewMapProj* map, __in const CString& filename, __out std::string& error_txt, 
            __out SHPObjectVec* pVecShpObjects,
            __in const d_geo_t& search_bnd_ll, __in const d_geo_t& search_bnd_ur );   // Required data bounds

   int open( __in ViewMapProj* map, __in const CString& filename, __out std::string& error_txt, 
                     __out SHPObjectVec* pVecShpObjects );

   SHPHandle SHPOpen( const char * pszShapeFile, const char * pszAccess );
   SHPHandle SHPCreate( const char * pszShapeFile, int nShapeType );
    void SHPGetInfo( SHPHandle hSHP, int * pnEntities, int * pnShapeType,
                  double * padfMinBound, double * padfMaxBound );
   BOOL SHPReadVertices( SHPHandle psSHP, int hEntity, int * pnVCount,
                        double ** verts, int * pnPartCount, int ** ppanParts );
   int SHPWriteVertices( SHPHandle hSHP, int nVCount, int nPartCount, 
              int * panParts, double * pdVertices);
   void SHPReadBounds( SHPHandle hSHP, int iShape, double * padBounds );
   void SHPClose( SHPHandle hSHP );
   void SHPWriteHeader( SHPHandle psSHP );
   void SHPSetBounds( BYTE * pabyRec, SHPObject * psShape );
   void SHPComputeExtents( SHPObject * psObject );
   SHPObject * SHPCreateObject( int nSHPType, int nShapeId, int nParts,
                 int * panPartStart, int * panPartType,
                 int nVertices, double * padfX, double * padfY,
                 double * padfZ, double * padfM );
   SHPObject * SHPCreateSimpleObject( int nSHPType, int nVertices,
                       double * padfX, double * padfY,
                       double * padfZ );
   int SHPWriteObject(SHPHandle psSHP, int nShapeId, SHPObject * psObject );
   bool SHPReadObject( SHPHandle psSHP, int hEntity, SHPObject& shape );
   const char * SHPTypeName( int nSHPType );
   const char * SHPPartTypeName( int nPartType );
   void InitializeShapeData( int32 nParts, int32 nVertices, SHPObject& shape );

private:
   // Methods
   CShapeRead(){} // Inaccessible
   bool ReadIntoShapeObjectPolygonOrArc( SHPObject& shape, const unsigned char* pabyRec, int cRecSize );
   bool ReadIntoShapeObjectMultipoint( SHPObject& shape, const unsigned char *pabyRec, int cRecSize );
   bool ReadIntoShapeObjectPoint( SHPObject& shape, const unsigned char *pabyRec, int cRecSize );
   void CaptureParts( int nParts, int * panPartStart, int * panPartType, SHPObject& shape );
   void CaptureMultiPointParts( int nParts, SHPObject& shape );
   void CaptureVertices(int nVertices, double * padfX, double * padfY,
               double * padfZ, double * padfM, bool bHasM, bool bHasZ, SHPObject *psObject);

   // Data
   int m_field_cnt;
   std::auto_ptr< unsigned char > m_apuchReadBuf;
   int m_nReadBufSize;
   CBaseShapeFileOverlay* m_pShpOvl;
   d_geo_t *m_pgptCaptureBndLL, *m_pgptCaptureBndUR; // Keep read shapes within these limits

#ifdef _DEBUG
   int m_cParts;
   int m_cVertices;
   int m_cTotalItems;
#endif

}; // class CShapeRead

#endif   // #ifndef SHPOPEN_H
