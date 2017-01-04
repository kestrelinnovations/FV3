// Copyright (c) 1994-2011,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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

// shpread.cpp

#include "stdafx.h"
#include "shpread.h"
#include "geo_tool.h"
#include "refresh.h"
#include <assert.h>
#include <sstream>

typedef unsigned char uchar;

#ifndef FALSE
#  define FALSE 0
#  define TRUE 1
#endif

#define ByteCopy( a, b, c ) memcpy( b, a, c )
#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif

static int bBigEndian;


// ********************************************************************
// ********************************************************************

CShapeRead::~CShapeRead()
{
}



/************************************************************************/
/*                              SwapWord()                              */
/*                                                                      */
/*      Swap a 2, 4 or 8 byte word.                                     */
/************************************************************************/

static void SwapWord( int length, void * wordP )

{
    uchar temp;

    for (int i=0; i < length/2; i++ )
    {
   temp = ((uchar *) wordP)[i];
   ((uchar *)wordP)[i] = ((uchar *) wordP)[length-i-1];
   ((uchar *) wordP)[length-i-1] = temp;
    }
}

/************************************************************************/
/*                             SfRealloc()                              */
/*                                                                      */
/*      A realloc cover function that will access a NULL pointer as     */
/*      a valid input.                                                  */
/************************************************************************/

static void * SfRealloc( void * pMem, int nNewSize )

{
    if( pMem == NULL )
        return( (void *) malloc(nNewSize) );
    else
        return( (void *) realloc(pMem,nNewSize) );
}



// ********************************************************************
// ********************************************************************

// This function has been changed to allow the fetching of a bounded subset of the data
// If target_bnd_ll and target_bnd_ur contain valid Lat/Long coordinates, a subset if fetched;
// If invalid, the entire file is fetched
int CShapeRead::open( __in ViewMapProj* map, __in const CString& filename, __out std::string& error_txt, 
                     __out SHPObjectVec* pVecShpObjects )
{
   d_geo_t target_bnd_ll, target_bnd_ur;
   target_bnd_ll.lat = -1000.0;  // Invalid, no limiting
   return open( map, filename, error_txt, pVecShpObjects, target_bnd_ll, target_bnd_ur );
}


int CShapeRead::open( __in ViewMapProj* map, __in const CString& filename, __out std::string& error_txt, 
            __out SHPObjectVec* pVecShpObjects,
            __in const d_geo_t& target_bnd_ll, __in const d_geo_t& target_bnd_ur )
{
   SHPObjectMap mpObjects;
   d_geo_t
      cache_bnd_ll, cache_bnd_ur;
   cache_bnd_ll.lat = +90.0; cache_bnd_ll.lon = +180.0;
   cache_bnd_ur.lat = -90.0; cache_bnd_ur.lon = -180.0;  // Ready to accumulate
   int iResult = open( map, filename, error_txt, mpObjects,
                     target_bnd_ll, target_bnd_ur, cache_bnd_ll, cache_bnd_ur,
                     false, NULL, NULL, NULL, NULL );
   if ( iResult == SUCCESS )
   {
      pVecShpObjects->reserve( mpObjects.size() );
      for ( SHPObjectMapIter it = mpObjects.begin(); it != mpObjects.end(); it++ )
         pVecShpObjects->push_back( it->second );
   }
   return iResult;
}


int CShapeRead::open( __in ViewMapProj* map, __in const CString& filename, __out std::string& error_txt,
            __inout SHPObjectMap& mpShpObjects,
            __in const d_geo_t& target_bnd_ll, __in const d_geo_t& target_bnd_ur,   // Required data bounds
            __inout d_geo_t& cache_bnd_ll, __inout d_geo_t& cache_bnd_ur,     // Previous (cached) and updated data bounds
            __in bool trim_cache,                                             
            __out d_geo_t* pactive_bnd_ll, __out d_geo_t* pactive_bnd_ur,     // Bounds of data found in search_bnd
            __out d_geo_t* pfile_bnd_ll, __out d_geo_t* pfile_bnd_ur )        // File data bounds
{
   BOOL bCheckBounds =
      target_bnd_ll.lat >= -90.0 && target_bnd_ll.lat <= +90.0
      && target_bnd_ur.lat >= -90.0 && target_bnd_ur.lat <= +90.0
      && target_bnd_ll.lon >= -180.0 && target_bnd_ll.lon <= +180.0
      && target_bnd_ur.lon >= -180.0 && target_bnd_ur.lon <= +180.0;

   d_geo_t clip_bnd_ll, clip_bnd_ur;

   if ( !bCheckBounds )
   {
      clip_bnd_ll.lat = cache_bnd_ll.lat = -90.0;   // Whole world
      clip_bnd_ll.lon = cache_bnd_ll.lon = -180.0;
      clip_bnd_ur.lat = cache_bnd_ur.lat = +90.0;
      clip_bnd_ur.lon = cache_bnd_ur.lon = +180.0;
   }
   else  // If desired bounds are meaningful
   {
      // For discarding, save up to 5x5 surrounding
      degrees_t dWidth, dHeight;
      GEO_calc_dimensions_degrees( target_bnd_ll, target_bnd_ur, dWidth, dHeight ); 
      d_geo_t dgptJunk;
      GEO_center_to_bounds( target_bnd_ll, __min( 360.0, 2.0 * dWidth ), __min( 180.0, 2.0 * dHeight ),
         clip_bnd_ll, dgptJunk );
      GEO_center_to_bounds( target_bnd_ur, __min( 360.0, 2.0 * dWidth ), __min( 180.0, 2.0 * dHeight ),
         dgptJunk, clip_bnd_ur );

      // If cache is active, see if everything is already available
      if ( cache_bnd_ll.lat < cache_bnd_ur.lat )
      {
         if( GEO_enclose( cache_bnd_ll.lat, cache_bnd_ll.lon,
                           cache_bnd_ur.lat, cache_bnd_ur.lon,
                           target_bnd_ll.lat, target_bnd_ll.lon,
                           target_bnd_ur.lat, target_bnd_ur.lon ) )
            return SUCCESS;

         // Don't shrink the clip area prematurely
         clip_bnd_ll.lat = __min( clip_bnd_ll.lat, cache_bnd_ll.lat );
         NEW_MAX( clip_bnd_ur.lat, cache_bnd_ur.lat )
         if ( GEO_east_of_degrees( clip_bnd_ll.lon, cache_bnd_ll.lon ) )
            clip_bnd_ll.lon = cache_bnd_ll.lon;
         if ( !GEO_east_of_degrees( clip_bnd_ur.lon, cache_bnd_ur.lon ) )
            clip_bnd_ur.lon = cache_bnd_ur.lon;
      }  // Active cache

      // Something more is needed.  Get one extra screen size in all directions to cache
      GEO_center_to_bounds( target_bnd_ll, __min( 360.0, 1.0 * dWidth ), __min( 180.0, 2.0 * dHeight ),
         cache_bnd_ll, dgptJunk );
      GEO_center_to_bounds( target_bnd_ur, __min( 360.0, 1.0 * dWidth ), __min( 180.0, 2.0 * dHeight ),
         dgptJunk, cache_bnd_ur );

      if ( dWidth >= 180.0 )  // Very wide, use all lons
      {
         clip_bnd_ll.lon = cache_bnd_ll.lon = -180.0;
         clip_bnd_ur.lon = cache_bnd_ur.lon = +180.0;
      }
   }  // Checking against desired bounds

   // Add all elements that we do not already have
   SHPHandle hSHP = SHPOpen( filename, "rb" );
   if ( hSHP == NULL )
   {
      error_txt = "Unable to open: ";
      error_txt += filename;
      return FAILURE;
   }

   int nShapeType, nEntities;
   double adfMinBound[4], adfMaxBound[4];
   SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

   // Check the coordinates of the boundaries
   if ( adfMinBound[1] < -180.0 || adfMinBound[1] > 180.0
      || adfMaxBound[1] < -180.0 || adfMaxBound[1] > 180.0
      || adfMinBound[0] < -360.0 || adfMinBound[0] > 360.0
      || adfMaxBound[0] < -360.0 || adfMaxBound[0] > 360.0 )
   {
      error_txt = "The Shape File: ";
      error_txt += filename;
      error_txt += " has invalid Lat/Long coordinates\n\nThe coordinates of "
         "this file are probably not Lat/Long";
      SHPClose( hSHP );
      return FAILURE;
   }

   if ( pfile_bnd_ll != nullptr && pfile_bnd_ur != nullptr )
   {
      pfile_bnd_ll->lat = adfMinBound[1];
      pfile_bnd_ll->lon = adfMinBound[0];
      pfile_bnd_ur->lat = adfMaxBound[1];
      pfile_bnd_ur->lon = adfMaxBound[0];
   }

   // If there is no intersection of this file with the expanded target, quit
   if ( !GEO_intersect(
            adfMinBound[1], adfMinBound[0],
            adfMaxBound[1], adfMaxBound[0],
            cache_bnd_ll.lat, cache_bnd_ll.lon,
            cache_bnd_ur.lat, cache_bnd_ur.lon ) )
   {
      SHPClose( hSHP );
      return SUCCESS;
   }

   // -------------------------------------------------------------------- 
   // Read all shapes that we don't already have
   // -------------------------------------------------------------------- 
   int errcnt = 0;
   if ( pactive_bnd_ll != nullptr && pactive_bnd_ur != nullptr )
   {
      pactive_bnd_ll->lat = +90.0; pactive_bnd_ll->lon = +180.0;
      pactive_bnd_ur->lat = -90.0; pactive_bnd_ur->lon = -180.0;  // Looking for bounds of drawing elements
   }
   
   SHPObjectMapIter itLastInsert = mpShpObjects.end();   // Last insert
#ifdef _DEBUG
   m_cParts = m_cVertices = 0;
#endif

   m_pgptCaptureBndLL = &cache_bnd_ll; m_pgptCaptureBndUR = &cache_bnd_ur;

   for ( int iEntity = 0; iEntity < nEntities; iEntity++ )
   {
      // If we have already read this entity, check whether to keep it
      SHPObjectMapIter it;
      if ( ( it = mpShpObjects.find( iEntity ) ) != mpShpObjects.end() )
      {
         SHPObject& shape = *it->second;
         if ( !GEO_intersect( cache_bnd_ll.lat, cache_bnd_ll.lon,
                                 cache_bnd_ur.lat, cache_bnd_ur.lon,
                                 shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
                                 shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
         {
            if ( trim_cache &&
               !GEO_intersect( clip_bnd_ll.lat, clip_bnd_ll.lon,
                                 clip_bnd_ur.lat, clip_bnd_ur.lon,
                                 shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
                                 shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
               mpShpObjects.erase( it );  // No longer needed
         }
         else if ( pactive_bnd_ll != nullptr && pactive_bnd_ur != nullptr
            && !GEO_intersect( target_bnd_ll.lat, target_bnd_ll.lon,
                                 target_bnd_ur.lat, target_bnd_ur.lon,
                                 shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
                                 shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
         {
            NEW_MIN( pactive_bnd_ll->lat, shape.m_grcBounds.ll.lat );
            NEW_MIN( pactive_bnd_ll->lon, shape.m_grcBounds.ll.lon );
            NEW_MAX( pactive_bnd_ur->lat, shape.m_grcBounds.ur.lat );
            NEW_MAX( pactive_bnd_ur->lon, shape.m_grcBounds.ur.lon );
         }
         continue;      // We already know this shape object
      }

      // Need to read the shape object from the file
      SHPObjectPtr spShape( new
         ( m_pShpOvl->m_clsHeapAllocator.allocate( sizeof(SHPObject) ) ) // Allocate from private heap
         SHPObject( &m_pShpOvl->m_clsHeapAllocator ) );
      SHPObject& shape = *spShape;
      if ( !SHPReadObject( hSHP, iEntity, shape ) )
         continue;   // Bad read or useless object

      if ( bCheckBounds )
      {
         // Check to see if this shape object is in the cachable area
         if ( !GEO_intersect(
                  shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
                  shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon,
                  cache_bnd_ll.lat, cache_bnd_ll.lon, 
                  cache_bnd_ur.lat, cache_bnd_ur.lon ) )
            continue;
      }

      if ( ( shape.m_mFeatureMask & SHPObject::FEATURE_POLYGON ) != 0
         && shape.m_cVertices < 3 )
      {
         // There must be at least 3 points in each polygon part
         errcnt++;
         continue;
      }

      // Accumulate bounds of target objects
      if ( pactive_bnd_ll != nullptr && pactive_bnd_ur != nullptr
            && !GEO_intersect( target_bnd_ll.lat, target_bnd_ll.lon,
                                 target_bnd_ur.lat, target_bnd_ur.lon,
                                 shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
                                 shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
      {
         NEW_MIN( pactive_bnd_ll->lat, shape.m_grcBounds.ll.lat );
         NEW_MIN( pactive_bnd_ll->lon, shape.m_grcBounds.ll.lon );
         NEW_MAX( pactive_bnd_ur->lat, shape.m_grcBounds.ur.lat );
         NEW_MAX( pactive_bnd_ur->lon, shape.m_grcBounds.ur.lon );
      }

      // Everything is okay, add the shape to our array
      itLastInsert = mpShpObjects.insert( itLastInsert, SHPObjectMap::value_type( iEntity, spShape ) );
   }

   SHPClose( hSHP );

   if ( errcnt > 0 )
   {
      std::stringstream ss;
      ss << "Errors found in ";
      ss << errcnt;
      ss << " objects in ";
      ss << filename;
      error_txt = ss.str();
      // what should be done in the case that errors were found?
   }
#ifdef _DEBUG
   m_cTotalItems = nEntities + m_cParts + m_cVertices;
#endif
 
   return SUCCESS;
}


// ********************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************

/************************************************************************/
/*                          SHPWriteHeader()                            */
/*                                                                      */
/*      Write out a header for the .shp and .shx files as well as the   */
/* contents of the index (.shx) file.                                   */
/************************************************************************/

void CShapeRead::SHPWriteHeader( SHPHandle psSHP )

{
    uchar abyHeader[100];
    int32 i32;
    double dValue;
    int32 *panSHX;

/* -------------------------------------------------------------------- */
/*      Prepare header block for .shp file.                             */
/* -------------------------------------------------------------------- */
    for (int i = 0; i < 100; i++ )
      abyHeader[i] = 0;

    abyHeader[2] = 0x27;  /* magic cookie */
    abyHeader[3] = 0x0a;

    i32 = psSHP->nFileSize/2;  /* file size */
    ByteCopy( &i32, abyHeader+24, 4 );
    if( !bBigEndian ) SwapWord( 4, abyHeader+24 );
    
    i32 = 1000;  /* version */
    ByteCopy( &i32, abyHeader+28, 4 );
    if( bBigEndian ) SwapWord( 4, abyHeader+28 );
    
    i32 = psSHP->nShapeType;  /* shape type */
    ByteCopy( &i32, abyHeader+32, 4 );
    if( bBigEndian ) SwapWord( 4, abyHeader+32 );

    dValue = psSHP->adBoundsMin[0];  /* set bounds */
    ByteCopy( &dValue, abyHeader+36, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+36 );

    dValue = psSHP->adBoundsMin[1];
    ByteCopy( &dValue, abyHeader+44, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+44 );

    dValue = psSHP->adBoundsMax[0];
    ByteCopy( &dValue, abyHeader+52, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+52 );

    dValue = psSHP->adBoundsMax[1];
    ByteCopy( &dValue, abyHeader+60, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+60 );

    dValue = psSHP->adBoundsMin[2];  /* z */
    ByteCopy( &dValue, abyHeader+68, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+68 );

    dValue = psSHP->adBoundsMax[2];
    ByteCopy( &dValue, abyHeader+76, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+76 );

    dValue = psSHP->adBoundsMin[3];  /* m */
    ByteCopy( &dValue, abyHeader+84, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+84 );

    dValue = psSHP->adBoundsMax[3];
    ByteCopy( &dValue, abyHeader+92, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+92 );

/* -------------------------------------------------------------------- */
/*      Write .shp file header.                                         */
/* -------------------------------------------------------------------- */
    fseek( psSHP->fpSHP, 0, 0 );
    fwrite( abyHeader, 100, 1, psSHP->fpSHP );

/* -------------------------------------------------------------------- */
/*      Prepare, and write .shx file header.                            */
/* -------------------------------------------------------------------- */
    i32 = (psSHP->nRecords * 2 * sizeof(int32) + 100)/2;   /* file size */
    ByteCopy( &i32, abyHeader+24, 4 );
    if( !bBigEndian ) SwapWord( 4, abyHeader+24 );
    
    fseek( psSHP->fpSHX, 0, 0 );
    fwrite( abyHeader, 100, 1, psSHP->fpSHX );

/* -------------------------------------------------------------------- */
/*      Write out the .shx contents.                                    */
/* -------------------------------------------------------------------- */
    panSHX = (int32 *) malloc(sizeof(int32) * 2 * psSHP->nRecords);

    for (int i = 0; i < psSHP->nRecords; i++ )
    {
   panSHX[i*2  ] = psSHP->panRecOffset[i]/2;
   panSHX[i*2+1] = psSHP->panRecSize[i]/2;
   if( !bBigEndian ) SwapWord( 4, panSHX+i*2 );
   if( !bBigEndian ) SwapWord( 4, panSHX+i*2+1 );
    }

    fwrite( panSHX, sizeof(int32) * 2, psSHP->nRecords, psSHP->fpSHX );

    free( panSHX );
}

/************************************************************************/
/*                              SHPOpen()                               */
/*                                                                      */
/*      Open the .shp and .shx files based on the basename of the       */
/*      files or either file name.                                      */
/************************************************************************/
   
SHPHandle SHPAPI_CALL
CShapeRead::SHPOpen( const char * pszLayer, const char * pszAccess )

{
    char *pszFullname, *pszBasename;
    SHPHandle psSHP;
    
    uchar *pabyBuf;
    int i;
    double dValue;
    
/* -------------------------------------------------------------------- */
/*      Ensure the access string is one of the legal ones.  We          */
/*      ensure the result string indicates binary to avoid common       */
/*      problems on Windows.                                            */
/* -------------------------------------------------------------------- */
    if ( strcmp(pszAccess,"rb+") == 0 || strcmp(pszAccess,"r+b") == 0
        || strcmp(pszAccess,"r+") == 0 )
        pszAccess = "r+b";
    else
        pszAccess = "rb";
    
/* -------------------------------------------------------------------- */
/* Establish the byte order on this machine.                            */
/* -------------------------------------------------------------------- */
    i = 1;
    if( *((uchar *) &i) == 1 )
        bBigEndian = FALSE;
    else
        bBigEndian = TRUE;

/* -------------------------------------------------------------------- */
/* Initialize the info structure.                                       */
/* -------------------------------------------------------------------- */
    psSHP = (SHPHandle) malloc(sizeof(SHPInfo));

    psSHP->bUpdated = FALSE;

/* -------------------------------------------------------------------- */
/* Compute the base (layer) name.  If there is any extension            */
/* on the passed in filename we will strip it off.                      */
/* -------------------------------------------------------------------- */
    pszBasename = (char *) malloc(strlen(pszLayer)+5);
    strcpy_s( pszBasename, strlen(pszLayer)+5, pszLayer );
    for (i = strlen(pszBasename)-1; 
    i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
          && pszBasename[i] != '\\';
    i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

/* -------------------------------------------------------------------- */
/* Open the .shp and .shx files.  Note that files pulled from           */
/* a PC to Unix with upper case filenames won't work!                   */
/* -------------------------------------------------------------------- */
    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf_s( pszFullname, strlen(pszBasename) + 5, "%s.shp", pszBasename );
    psSHP->fpSHP = NULL;
    fopen_s(&(psSHP->fpSHP), pszFullname, pszAccess );
    if( psSHP->fpSHP == NULL )
    {
        sprintf_s( pszFullname, strlen(pszBasename) + 5, "%s.SHP", pszBasename );
        fopen_s(&(psSHP->fpSHP), pszFullname, pszAccess );
    }
    
    if( psSHP->fpSHP == NULL )
    {
        free( psSHP );
        free( pszBasename );
        free( pszFullname );
        return( NULL );
    }

    setvbuf( psSHP->fpSHP, NULL, _IOFBF, 0x40000 );   // 256K main buffer

    sprintf_s( pszFullname, strlen(pszBasename) + 5, "%s.shx", pszBasename );
    psSHP->fpSHX = NULL;
    fopen_s(&(psSHP->fpSHX), pszFullname, pszAccess );
    if( psSHP->fpSHX == NULL )
    {
        sprintf_s( pszFullname, strlen(pszBasename) + 5, "%s.SHX", pszBasename );
        fopen_s(&(psSHP->fpSHX), pszFullname, pszAccess );
    }
    
    if( psSHP->fpSHX == NULL )
    {
      CString msg;
      msg.Format("Failed to open file \"%s\",\n error code = %d.", pszFullname, errno );
      AfxMessageBox(msg);
        fclose( psSHP->fpSHP );
        free( psSHP );
        free( pszBasename );
        free( pszFullname );
        return( NULL );
    }

    setvbuf( psSHP->fpSHP, NULL, _IOFBF, 0x10000 );   // 64K shx buffer

    free( pszFullname );
    free( pszBasename );

/* -------------------------------------------------------------------- */
/*  Read the file size from the SHP file.                               */
/* -------------------------------------------------------------------- */
    pabyBuf = (uchar *) malloc(100);
    fread( pabyBuf, 100, 1, psSHP->fpSHP );

    psSHP->nFileSize = (pabyBuf[24] * 256 * 256 * 256
         + pabyBuf[25] * 256 * 256
         + pabyBuf[26] * 256
         + pabyBuf[27]) * 2;

/* -------------------------------------------------------------------- */
/*  Read SHX file Header info                                           */
/* -------------------------------------------------------------------- */
    fread( pabyBuf, 100, 1, psSHP->fpSHX );

    if( pabyBuf[0] != 0 
        || pabyBuf[1] != 0 
        || pabyBuf[2] != 0x27 
        || (pabyBuf[3] != 0x0a && pabyBuf[3] != 0x0d) )
    {
      fclose( psSHP->fpSHP );
      fclose( psSHP->fpSHX );
      free( psSHP );
      free( pabyBuf );
      return( NULL );
    }

    psSHP->nRecords = pabyBuf[27] + pabyBuf[26] * 256
      + pabyBuf[25] * 256 * 256 + pabyBuf[24] * 256 * 256 * 256;
    psSHP->nRecords = (psSHP->nRecords*2 - 100) / 8;

    psSHP->nShapeType = pabyBuf[32];

    if ( psSHP->nRecords < 0 || psSHP->nRecords > 256000000 )
    {
      /* this header appears to be corrupt.  Give up. */
      fclose( psSHP->fpSHP );
      fclose( psSHP->fpSHX );
      free( psSHP );
      free( pabyBuf );
      return( NULL );
    }

/* -------------------------------------------------------------------- */
/*      Read the bounds.                                                */
/* -------------------------------------------------------------------- */
    if ( bBigEndian ) 
      SwapWord( 8, pabyBuf+36 );
    memcpy( &dValue, pabyBuf+36, 8 );
    psSHP->adBoundsMin[0] = dValue;

    if ( bBigEndian ) 
      SwapWord( 8, pabyBuf+44 );
    memcpy( &dValue, pabyBuf+44, 8 );
    psSHP->adBoundsMin[1] = dValue;

    if ( bBigEndian ) 
      SwapWord( 8, pabyBuf+52 );
    memcpy( &dValue, pabyBuf+52, 8 );
    psSHP->adBoundsMax[0] = dValue;

    if ( bBigEndian ) 
      SwapWord( 8, pabyBuf+60 );
    memcpy( &dValue, pabyBuf+60, 8 );
    psSHP->adBoundsMax[1] = dValue;

    if ( bBigEndian ) 
      SwapWord( 8, pabyBuf+68 );  /* z */
    memcpy( &dValue, pabyBuf+68, 8 );
    psSHP->adBoundsMin[2] = dValue;
    
    if( bBigEndian ) SwapWord( 8, pabyBuf+76 );
    memcpy( &dValue, pabyBuf+76, 8 );
    psSHP->adBoundsMax[2] = dValue;
    
    if ( bBigEndian ) 
      SwapWord( 8, pabyBuf+84 );  /* z */
    memcpy( &dValue, pabyBuf+84, 8 );
    psSHP->adBoundsMin[3] = dValue;

    if ( bBigEndian ) 
      SwapWord( 8, pabyBuf+92 );
    memcpy( &dValue, pabyBuf+92, 8 );
    psSHP->adBoundsMax[3] = dValue;

    free( pabyBuf );

/* -------------------------------------------------------------------- */
/* Read the .shx file to get the offsets to each record in              */
/* the .shp file.                                                       */
/* -------------------------------------------------------------------- */
    psSHP->nMaxRecords = psSHP->nRecords;

    psSHP->panRecOffset = (int *) malloc(sizeof(int) * MAX(1,psSHP->nMaxRecords) );
    psSHP->panRecSize   = (int *) malloc(sizeof(int) * MAX(1,psSHP->nMaxRecords) );

    if ( psSHP->nRecords == 0)  // new file
      return (psSHP );

    pabyBuf = (uchar *) malloc(8 * MAX(1,psSHP->nRecords) );
    fread( pabyBuf, 8, psSHP->nRecords, psSHP->fpSHX );

    for ( i = 0; i < psSHP->nRecords; i++ )
    {
      int32 nOffset, nLength;

      memcpy( &nOffset, pabyBuf + i * 8, 4 );
      if ( !bBigEndian ) 
         SwapWord( 4, &nOffset );

      memcpy( &nLength, pabyBuf + i * 8 + 4, 4 );
      if ( !bBigEndian ) SwapWord( 4, &nLength );

      psSHP->panRecOffset[i] = nOffset*2;
      psSHP->panRecSize[i] = nLength*2;
    }
    free( pabyBuf );

    return( psSHP );
}

/************************************************************************/
/*                              SHPClose()                              */
/* Close the .shp and .shx files.                                       */
/************************************************************************/

void SHPAPI_CALL
CShapeRead::SHPClose(SHPHandle psSHP )

{
   if ( psSHP == NULL )
      return;

/* -------------------------------------------------------------------- */
/* Update the header if we have modified anything.                      */
/* -------------------------------------------------------------------- */
    if( psSHP->bUpdated )
    {
   SHPWriteHeader( psSHP );
    }

/* -------------------------------------------------------------------- */
/*      Free all resources, and close files.                            */
/* -------------------------------------------------------------------- */
    free( psSHP->panRecOffset );
    free( psSHP->panRecSize );

    fclose( psSHP->fpSHX );
    fclose( psSHP->fpSHP );

    free( psSHP );
}

/************************************************************************/
/*                             SHPGetInfo()                             */
/*                                                                      */
/*      Fetch general information about the shape file.                 */
/************************************************************************/

void SHPAPI_CALL
CShapeRead::SHPGetInfo(SHPHandle psSHP, int * pnEntities, int * pnShapeType,
           double * padfMinBound, double * padfMaxBound )

{
    if( pnEntities != NULL )
        *pnEntities = psSHP->nRecords;

    if( pnShapeType != NULL )
        *pnShapeType = psSHP->nShapeType;

    for (int i = 0; i < 4; i++ )
    {
        if( padfMinBound != NULL )
            padfMinBound[i] = psSHP->adBoundsMin[i];
        if( padfMaxBound != NULL )
            padfMaxBound[i] = psSHP->adBoundsMax[i];
    }
}

/************************************************************************/
/*                             SHPCreate()                              */
/*                                                                      */
/*      Create a new shape file and return a handle to the open         */
/*      shape file with read/write access.                              */
/************************************************************************/

SHPHandle SHPAPI_CALL
CShapeRead::SHPCreate( const char * pszLayer, int nShapeType )

{
    char *pszBasename, *pszFullname;
    int i;
    FILE *fpSHP, *fpSHX;
    uchar abyHeader[100];
    int32 i32;
    double dValue;
    
/* -------------------------------------------------------------------- */
/*      Establish the byte order on this system.                        */
/* -------------------------------------------------------------------- */
    i = 1;
    if( *((uchar *) &i) == 1 )
        bBigEndian = FALSE;
    else
        bBigEndian = TRUE;

/* -------------------------------------------------------------------- */
/* Compute the base (layer) name.  If there is any extension            */
/* on the passed in filename we will strip it off.                      */
/* -------------------------------------------------------------------- */
    pszBasename = (char *) malloc(strlen(pszLayer)+5);
    strcpy_s( pszBasename, strlen(pszLayer)+5, pszLayer );
    for ( i = strlen(pszBasename)-1; 
    i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
          && pszBasename[i] != '\\';
    i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

/* -------------------------------------------------------------------- */
/*      Open the two files so we can write their headers.               */
/* -------------------------------------------------------------------- */
    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf_s( pszFullname, strlen(pszBasename) + 5, "%s.shp", pszBasename );
    fpSHP = NULL;
    fopen_s(&fpSHP, pszFullname, "wb" );
    if( fpSHP == NULL )
    {
         free( pszFullname );
         free( pszBasename );
         return( NULL );
    }

    sprintf_s( pszFullname, strlen(pszBasename) + 5, "%s.shx", pszBasename );
    fpSHX = NULL;
    fopen_s(&fpSHX, pszFullname, "wb" );
    if( fpSHX == NULL )
        return( NULL );

    free( pszFullname );
    free( pszBasename );

/* -------------------------------------------------------------------- */
/*      Prepare header block for .shp file.                             */
/* -------------------------------------------------------------------- */
    for (int i = 0; i < 100; i++ )
      abyHeader[i] = 0;

    abyHeader[2] = 0x27;  /* magic cookie */
    abyHeader[3] = 0x0a;

    i32 = 50;  /* file size */
    ByteCopy( &i32, abyHeader+24, 4 );
    if( !bBigEndian ) SwapWord( 4, abyHeader+24 );
    
    i32 = 1000; /* version */
    ByteCopy( &i32, abyHeader+28, 4 );
    if( bBigEndian ) SwapWord( 4, abyHeader+28 );
    
    i32 = nShapeType;  /* shape type */
    ByteCopy( &i32, abyHeader+32, 4 );
    if( bBigEndian ) SwapWord( 4, abyHeader+32 );

    dValue = 0.0;  /* set bounds */
    ByteCopy( &dValue, abyHeader+36, 8 );
    ByteCopy( &dValue, abyHeader+44, 8 );
    ByteCopy( &dValue, abyHeader+52, 8 );
    ByteCopy( &dValue, abyHeader+60, 8 );

/* -------------------------------------------------------------------- */
/*      Write .shp file header.                                         */
/* -------------------------------------------------------------------- */
    fwrite( abyHeader, 100, 1, fpSHP );

/* -------------------------------------------------------------------- */
/*      Prepare, and write .shx file header.                            */
/* -------------------------------------------------------------------- */
    i32 = 50;  /* file size */
    ByteCopy( &i32, abyHeader+24, 4 );
    if( !bBigEndian ) SwapWord( 4, abyHeader+24 );
    
    fwrite( abyHeader, 100, 1, fpSHX );

/* -------------------------------------------------------------------- */
/*      Close the files, and then open them as regular existing files.  */
/* -------------------------------------------------------------------- */
    fclose( fpSHP );
    fclose( fpSHX );

    return( SHPOpen( pszLayer, "r+b" ) );
}

/************************************************************************/
/*                           _SHPSetBounds()                            */
/*                                                                      */
/*      Compute a bounds rectangle for a shape, and set it into the     */
/*      indicated location in the record.                               */
/************************************************************************/

void SHPSetBounds( uchar * pabyRec, SHPObject * psShape )
{
    ByteCopy( &(psShape->m_grcBounds.ll.lon), pabyRec +  0, 8 );
    ByteCopy( &(psShape->m_grcBounds.ll.lat), pabyRec +  8, 8 );
    ByteCopy( &(psShape->m_grcBounds.ur.lon), pabyRec + 16, 8 );
    ByteCopy( &(psShape->m_grcBounds.ur.lat), pabyRec + 24, 8 );

    if( bBigEndian )
    {
        SwapWord( 8, pabyRec + 0 );
        SwapWord( 8, pabyRec + 8 );
        SwapWord( 8, pabyRec + 16 );
        SwapWord( 8, pabyRec + 24 );
    }
}

/************************************************************************/
/*                         SHPComputeExtents()                          */
/*                                                                      */
/*      Recompute the extents of a shape.  Automatically done by        */
/*      SHPCreateObject().                                              */
/************************************************************************/

void SHPAPI_CALL
CShapeRead::SHPComputeExtents( SHPObject * psObject )

{
/* -------------------------------------------------------------------- */
/*      Build extents for this object.                                  */
/* -------------------------------------------------------------------- */
   if ( psObject->m_cVertices > 0 )
   {
      psObject->m_grcBounds.ur.lon = psObject->m_grcBounds.ur.lat = -DBL_MAX;
      psObject->m_grcBounds.ll.lon = psObject->m_grcBounds.ll.lat = +DBL_MAX;

      const SHPVertex* pVertices = psObject->GetVertices();
      for ( size_t iVertex = 0; iVertex < psObject->m_cVertices; iVertex++ )
      {
         const SHPVertex& vertex = pVertices[ iVertex ];

         psObject->m_grcBounds.ll.lon = MIN( psObject->m_grcBounds.ll.lon, vertex.lon );
         psObject->m_grcBounds.ll.lat = MIN( psObject->m_grcBounds.ll.lat, vertex.lat );

         psObject->m_grcBounds.ur.lon = MAX( psObject->m_grcBounds.ur.lon, vertex.lon );
         psObject->m_grcBounds.ur.lat = MAX( psObject->m_grcBounds.ur.lat, vertex.lat );
      }
    };
}

bool HasM(int nSHPType)
{
   switch ( nSHPType )
   {
      case SHPT_ARCM:
      case SHPT_POINTM:
      case SHPT_POLYGONM:
      case SHPT_MULTIPOINTM:
      case SHPT_ARCZ:
      case SHPT_POINTZ:
      case SHPT_POLYGONZ:
      case SHPT_MULTIPOINTZ:
      case SHPT_MULTIPATCH:
         return true;
   }
   return false;
}

bool HasZ(int nSHPType)
{
   switch ( nSHPType )
   {
      case SHPT_ARCZ:
      case SHPT_POINTZ:
      case SHPT_POLYGONZ:
      case SHPT_MULTIPOINTZ:
      case SHPT_MULTIPATCH:
         return true;
   }

   return false;
}


/* -------------------------------------------------------------------- */
/*      Capture parts.  Note that part type is optional, and            */
/*      defaults to ring.                                               */
/* -------------------------------------------------------------------- */
void CShapeRead::CaptureParts(int nParts, int * panPartStart, int * panPartType,
   SHPObject& shape )
{
   for ( int iPart = 0; iPart < nParts; iPart++ )
   {
      SHPPart& part = shape.GetPart( iPart );
      part.part_start = panPartStart[ iPart ];
      if ( panPartType != nullptr )
         part.part_type = panPartType[ iPart ];
   }
}


void CShapeRead::CaptureMultiPointParts( int nParts, SHPObject& shape )
{
   for ( int iPart = 0; iPart < nParts; iPart++ )
   {
      SHPPart& part = shape.GetPart( iPart );
      part.part_start = iPart;
   }
}

void CShapeRead::InitializeShapeData( int32 nParts, int32 nPoints, SHPObject& shape )
{
   m_pShpOvl->m_clsHeapAllocator.deallocate( shape.m_pbyShapeData );

#ifdef _DEBUG
   m_cParts += nParts;
   m_cVertices += nPoints;
#endif

   shape.m_cParts = nParts;
   shape.m_cVertices = nPoints;

   // The shape data is a concatenation of: vertices, parts, part geobounds,
   //    polygon shape screen limits, part M values, part Z values

   size_t
      cVertexBytes = nPoints * sizeof(SHPVertex) / sizeof(BYTE),
      cPartBytes = nParts * sizeof(SHPPart) / sizeof(BYTE),
      cBoundsBytes = ( nParts <= 1 )
         ? 0 : ( nParts * sizeof(d_geo_rect_t) / sizeof(BYTE) ),
      cPolygonScreenLimitsBytes = ( ( shape.m_mFeatureMask & SHPObject::FEATURE_POLYGON ) == 0 )
         ? 0 : 4 * sizeof(int) / sizeof(BYTE),
      cPolygonLabelInfoBytes = ( ( shape.m_mFeatureMask & SHPObject::FEATURE_POLYGON ) == 0 )
         ? 0 : ( nParts * sizeof(PolygonLabelInfo) / sizeof(BYTE) ),
      cMBytes = ( ( shape.m_mFeatureMask & SHPObject::FEATURE_M ) == 0 )
         ? 0 : ( nPoints * sizeof(DOUBLE) / sizeof(BYTE) ),
      cZBytes = ( ( shape.m_mFeatureMask & SHPObject::FEATURE_Z ) == 0 )
         ? 0 : ( nPoints * sizeof(DOUBLE) / sizeof(BYTE) );

   shape.m_pbyShapeData = (PBYTE) m_pShpOvl->m_clsHeapAllocator.allocate(
      sizeof(BYTE) * ( cVertexBytes + cPartBytes + cBoundsBytes
         + cPolygonScreenLimitsBytes + cPolygonLabelInfoBytes + cMBytes + cZBytes ) );

   SHPVertex* pVertices = shape.GetVertices();
   for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
      new ( pVertices + iVertex ) SHPVertex();


   for ( int iPart = 0; iPart < nParts; iPart++ )
   {
      new ( &shape.GetPart( iPart ) ) SHPPart();
      if ( cPolygonLabelInfoBytes > 0 )
         new ( &shape.GetPolygonLabelInfo( iPart ) ) PolygonLabelInfo();
   }

   // Z and M data don't need initialization

}  // InitializeShapeData()


/* -------------------------------------------------------------------- */
/*      Capture vertices.  Note that Z and M are optional, but X and    */
/*      Y are not.                                                      */
/* -------------------------------------------------------------------- */
void CShapeRead::CaptureVertices(int nVertices, double * padfX, double * padfY,
   double * padfZ, double * padfM, bool bHasM, bool bHasZ, SHPObject *psObject)
{
   assert( padfX != NULL );
   assert( padfY != NULL );

   SHPVertex* pVertices = psObject->GetVertices();
   double* pdZ = ( bHasZ && padfZ != nullptr ) ? psObject->GetZs() : nullptr;
   double* pdM = ( bHasM && padfM != nullptr ) ? psObject->GetMs() : nullptr;

   for (int i = 0; i < nVertices; i++ )
   {
      pVertices[ i ].lon = padfX[ i ];
      pVertices[ i ].lat = padfY[ i ];

      if ( pdZ != nullptr )
         pdZ[ i ] = padfZ[ i ];

      if ( pdM != nullptr )
         pdM[ i ] = padfM[ i ];
   }
}  // CaptureVertices()


/************************************************************************/
/*                          SHPCreateObject()                           */
/*                                                                      */
/*      Create a shape object.                                          */
/************************************************************************/

SHPObject * CShapeRead::SHPCreateObject( int nSHPType, int nShapeId, int nParts,
                 int * panPartStart, int * panPartType,
                 int nVertices, double * padfX, double * padfY,
                 double * padfZ, double * padfM )

{
    SHPObject *psObject;

    bool bHasM = HasM( nSHPType );
    bool bHasZ = HasZ( nSHPType );

    psObject = (SHPObject *) calloc(1,sizeof(SHPObject));
    psObject->nSHPType = nSHPType;
    psObject->nShapeId = nShapeId;

    if( nSHPType == SHPT_ARC ||
       nSHPType == SHPT_POLYGON ||
       nSHPType == SHPT_ARCM ||
       nSHPType == SHPT_POLYGONM ||
       nSHPType == SHPT_ARCZ ||
       nSHPType == SHPT_POLYGONZ ||
       nSHPType == SHPT_MULTIPATCH )
    {
       CaptureParts( nParts, panPartStart, panPartType, *psObject);
    }
    else if( nSHPType == SHPT_MULTIPOINT ||
       nSHPType == SHPT_MULTIPOINTM ||
       nSHPType == SHPT_MULTIPOINTZ )
    {
       CaptureMultiPointParts(1, *psObject);
    }
    else if( nSHPType == SHPT_POINT ||
       nSHPType == SHPT_POINTM ||
       nSHPType == SHPT_POINTZ )
    {
       // A point has a single part.
       int part_start = 0;
       CaptureParts( 1, &part_start, nullptr, *psObject );
    }

    CaptureVertices(nVertices, padfX, padfY, padfZ, padfM,
       bHasM, bHasZ, psObject);

/* -------------------------------------------------------------------- */
/*      Compute the extents.                                            */
/* -------------------------------------------------------------------- */
    SHPComputeExtents( psObject );

    return( psObject );
}

/************************************************************************/
/*                       SHPCreateSimpleObject()                        */
/*                                                                      */
/*      Create a simple (common) shape object.                          */
/************************************************************************/

SHPObject * CShapeRead::SHPCreateSimpleObject( int nSHPType, int nVertices,
                       double * padfX, double * padfY,
                       double * padfZ )

{
    return( SHPCreateObject( nSHPType, -1, 0, NULL, NULL,
                             nVertices, padfX, padfY, padfZ, NULL ) );
}

void AddToMemoryIndex(SHPHandle psSHP)
{
   psSHP->nMaxRecords =(int) ( psSHP->nMaxRecords * 1.3 + 100);

   psSHP->panRecOffset = (int *) 
      SfRealloc(psSHP->panRecOffset,sizeof(int) * psSHP->nMaxRecords );
   psSHP->panRecSize = (int *) 
      SfRealloc(psSHP->panRecSize,sizeof(int) * psSHP->nMaxRecords );
}

int ReadIntoRecordPolygonOrArc(SHPObject* psObject, uchar *pabyRec)
{
   int32 nPoints, nParts;

   nPoints = psObject->m_cVertices;
   nParts = psObject->m_cParts;

   SHPSetBounds( pabyRec + 12, psObject );

   if( bBigEndian ) SwapWord( 4, &nPoints );
   if( bBigEndian ) SwapWord( 4, &nParts );

   ByteCopy( &nPoints, pabyRec + 40 + 8, 4 );
   ByteCopy( &nParts, pabyRec + 36 + 8, 4 );

   int nRecordSize = 52;

   /*
   * Write part start positions.
   */
   for ( int iPart = 0; iPart < nParts; iPart++ )
   {
      const SHPPart& part = psObject->GetPart( iPart );

      memcpy( pabyRec + nRecordSize, &part.part_start, 4 );
      if ( bBigEndian )
         SwapWord( 4, pabyRec + nRecordSize );

      nRecordSize += 4;
   }

   /*
   * Write multipatch part types if needed.
   */
   if ( psObject->nSHPType == SHPT_MULTIPATCH )
   {
      for ( int iPart = 0; iPart < nParts; iPart++ )
      {
         const SHPPart& part = psObject->GetPart( iPart );
         memcpy( pabyRec + nRecordSize, &part.part_type, 4 );
         if ( bBigEndian )
            SwapWord( 4, pabyRec + nRecordSize );
         nRecordSize += 4;
      }
   }

   /*
   * Write the (x,y) vertex values.
   */
   const SHPVertex* pVertices = psObject->GetVertices();
   for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
   {
      memcpy( pabyRec + nRecordSize + 0, &pVertices[ iVertex ].lon, 8 );
      memcpy( pabyRec + nRecordSize + 8, &pVertices[ iVertex ].lat, 8);
      if( bBigEndian )
      {
         SwapWord( 8, pabyRec + nRecordSize + 0 );
         SwapWord( 8, pabyRec + nRecordSize + 8 );
      }
      nRecordSize += 2 * 8;
   }

   /*
   * Write the Z coordinates (if any).
   */
   if ( psObject->nSHPType == SHPT_POLYGONZ
         || psObject->nSHPType == SHPT_ARCZ
         || psObject->nSHPType == SHPT_MULTIPATCH )
   {
      ByteCopy( &(psObject->dfZMin), pabyRec + nRecordSize, 8 );
      if ( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;

      ByteCopy( &(psObject->dfZMax), pabyRec + nRecordSize, 8 );
      if( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;

      const DOUBLE* pdZs = psObject->GetZs();
      assert( pdZs != nullptr && "Z data is missing" );
      for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
      {
#ifdef SKIPIT
         if ( pdZs != nullptr )
            memcpy( pabyRec + nRecordSize, &pdZs[ iVertex ], 8 );
         else
#endif
            ZeroMemory( pabyRec + nRecordSize, 8 );
         if( bBigEndian )
            SwapWord( 8, pabyRec + nRecordSize );
         nRecordSize += 8;
      }
   }

   /*
   * Write the M values, if any.
   */
   if( psObject->nSHPType == SHPT_POLYGONM
      || psObject->nSHPType == SHPT_ARCM
#ifndef DISABLE_MULTIPATCH_MEASURE
      || psObject->nSHPType == SHPT_MULTIPATCH
#endif
      || psObject->nSHPType == SHPT_POLYGONZ
      || psObject->nSHPType == SHPT_ARCZ )
   {
      ByteCopy( &(psObject->dfMMin), pabyRec + nRecordSize, 8 );
      if( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;

      ByteCopy( &(psObject->dfMMax), pabyRec + nRecordSize, 8 );
      if( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;

      const DOUBLE* pdMs = psObject->GetMs();
      assert( pdMs != nullptr && "M data is missing" );
      for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
      {
         if ( pdMs != nullptr )
            memcpy( pabyRec + nRecordSize, &pdMs[ iVertex ], 8 );
         else
            ZeroMemory( pabyRec + nRecordSize, 8 );
         if( bBigEndian )
            SwapWord( 8, pabyRec + nRecordSize );
         nRecordSize += 8;
      }
   }

   return nRecordSize;

}  // eadIntoRecordPolygonOrArc()

int ReadIntoRecordMultipoint(SHPObject* psObject, uchar* pabyRec)
{
   int32 nPoints;

   nPoints = psObject->m_cVertices;

   SHPSetBounds( pabyRec + 12, psObject );

   ByteCopy( &nPoints, pabyRec + 44, 4 );
   if( bBigEndian )
      SwapWord( 4, &pabyRec + 44 );

   const SHPVertex* pVertices = psObject->GetVertices();
   for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
   {
      const SHPVertex& vertex = pVertices[ iVertex ];
      memcpy( pabyRec + 48 + ( iVertex * 16 ) + 0, &vertex.lon, 8 );
      memcpy( pabyRec + 48 + ( iVertex * 16 ) + 8, &vertex.lat, 8 );
      if( bBigEndian )
      {
         SwapWord( 8, pabyRec + 48 + ( iVertex * 16 ) + 0 );
         SwapWord( 8, pabyRec + 48 + ( iVertex * 16 ) + 8 );
      }
   }

   int nRecordSize = 48 + ( 16 * nPoints );

   if ( psObject->nSHPType == SHPT_MULTIPOINTZ )
   {
      ByteCopy( &(psObject->dfZMin), pabyRec + nRecordSize, 8 );
      if( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;

      ByteCopy( &(psObject->dfZMax), pabyRec + nRecordSize, 8 );
      if ( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;

      const DOUBLE* pdZs = psObject->GetZs();
      assert( pdZs != nullptr && "Z data is missing" );
      for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
      {
         if ( pdZs != nullptr )
            memcpy( pabyRec + nRecordSize, &pdZs[ iVertex ], 8 );
         else
            ZeroMemory( pabyRec + nRecordSize, 8 );
         if( bBigEndian )
            SwapWord( 8, pabyRec + nRecordSize );
         nRecordSize += 8;
      }
   }

   if ( psObject->nSHPType == SHPT_MULTIPOINTZ
      || psObject->nSHPType == SHPT_MULTIPOINTM )
   {
      ByteCopy( &(psObject->dfMMin), pabyRec + nRecordSize, 8 );
      if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;

      ByteCopy( &(psObject->dfMMax), pabyRec + nRecordSize, 8 );
      if( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;

      const DOUBLE* pdMs = psObject->GetMs();
      assert( pdMs != nullptr && "M data is missing" );
      for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
      {
         if ( pdMs != nullptr )
            memcpy( pabyRec + nRecordSize, &pdMs[ iVertex ], 8 );
         else
            ZeroMemory( pabyRec + nRecordSize, 8 );
         if( bBigEndian )
            SwapWord( 8, pabyRec + nRecordSize );
         nRecordSize += 8;
      }
   }

   return nRecordSize;
}  // ReadIntoRecordMultipoint()


int ReadIntoRecordPoint(SHPObject* psObject, uchar* pabyRec)
{
   memcpy( pabyRec + 12, &psObject->GetVertices()[ 0 ].lon, 8 );
   memcpy( pabyRec + 20, &psObject->GetVertices()[ 0 ].lat, 8 );
   if( bBigEndian )
   {
      SwapWord( 8, pabyRec + 12 );
      SwapWord( 8, pabyRec + 20 );
   }

   int nRecordSize = 28;

   if ( psObject->nSHPType == SHPT_POINTZ )
   {
      if ( psObject->GetZs() != nullptr )
         memcpy( pabyRec + nRecordSize, &psObject->GetZs()[ 0 ], 8 );
      else
         ZeroMemory( pabyRec + nRecordSize, 8 );
      if( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;
   }

   if ( psObject->nSHPType == SHPT_POINTZ
      || psObject->nSHPType == SHPT_POINTM )
   {
      if ( psObject->GetMs() != nullptr )
         memcpy( pabyRec + nRecordSize, &psObject->GetMs()[ 0 ], 8 );
      else
         ZeroMemory( pabyRec + nRecordSize, 8 );
      if( bBigEndian )
         SwapWord( 8, pabyRec + nRecordSize );
      nRecordSize += 8;
   }

   return nRecordSize;
}  // ReadIntoRecordPoint()


/* -------------------------------------------------------------------- */
/*      Establish where we are going to put this record. If we are      */
/*      rewriting an existing record, and it will fit, then put it      */
/*      back where the original came from.  Otherwise write at the end. */
/* -------------------------------------------------------------------- */
int GetRecordOffset(SHPHandle psSHP, int nShapeId, int nRecordSize)
{
   int nRecordOffset = 0;
   if( nShapeId == -1 || psSHP->panRecSize[nShapeId] < nRecordSize-8 )
   {
      if( nShapeId == -1 )
         nShapeId = psSHP->nRecords++;

      psSHP->panRecOffset[nShapeId] = nRecordOffset = psSHP->nFileSize;
      psSHP->panRecSize[nShapeId] = nRecordSize-8;
      psSHP->nFileSize += nRecordSize;
   }
   else
   {
      nRecordOffset = psSHP->panRecOffset[nShapeId];
   }

   return nRecordOffset;
}

/* -------------------------------------------------------------------- */
/*      Set the shape type, record number, and record size.             */
/* -------------------------------------------------------------------- */
void SetRecordInformation( int nShapeId, SHPObject * psObject, int nRecordSize,
   uchar* pabyRec)
{
   int32 i32 = nShapeId+1;  /* record # */
   if( !bBigEndian ) SwapWord( 4, &i32 );
   ByteCopy( &i32, pabyRec, 4 );

   i32 = (nRecordSize-8)/2;  /* record size */
   if( !bBigEndian ) SwapWord( 4, &i32 );
   ByteCopy( &i32, pabyRec + 4, 4 );

   i32 = psObject->nSHPType;  /* shape type */
   if( bBigEndian ) SwapWord( 4, &i32 );
   ByteCopy( &i32, pabyRec + 8, 4 );
}

/* -------------------------------------------------------------------- */
/*      Write out record.                                               */
/* -------------------------------------------------------------------- */
bool WriteRecord(SHPHandle psSHP, int nRecordOffset, int nRecordSize, uchar* pabyRec)
{
   if( fseek( psSHP->fpSHP, nRecordOffset, 0 ) != 0
      || fwrite( pabyRec, nRecordSize, 1, psSHP->fpSHP ) < 1 )
   {
      return false;
   }
   return true;
}


/* -------------------------------------------------------------------- */
/* Expand file wide bounds based on this shape.                         */
/* -------------------------------------------------------------------- */
void ExpandBounds( SHPHandle psSHP, SHPObject * psObject )
{
   if( psSHP->adBoundsMin[0] == 0.0
      && psSHP->adBoundsMax[0] == 0.0
      && psSHP->adBoundsMin[1] == 0.0
      && psSHP->adBoundsMax[1] == 0.0
      && psObject->nSHPType != SHPT_NULL )
   {
      psSHP->adBoundsMin[0] = psSHP->adBoundsMax[0] =
         psObject->GetVertices()[ 0 ].lon;
      psSHP->adBoundsMin[1] = psSHP->adBoundsMax[1] =
         psObject->GetVertices()[ 0 ].lat;
      if ( psObject->GetZs() != nullptr )
         psSHP->adBoundsMin[2] = psSHP->adBoundsMax[2] = psObject->GetZs()[ 0 ];
      if ( psObject->GetMs() != nullptr )
         psSHP->adBoundsMin[3] = psSHP->adBoundsMax[3] = psObject->GetMs()[ 0 ];
   }

   const SHPVertex* pVertices = psObject->GetVertices();
   for ( size_t iVertex = 0; iVertex < psObject->m_cVertices; iVertex++ )
   {
      const SHPVertex& vertex = pVertices[ iVertex ];

      psSHP->adBoundsMin[0] = MIN( psSHP->adBoundsMin[0], vertex.lon );
      psSHP->adBoundsMax[0] = MAX( psSHP->adBoundsMax[0], vertex.lon );

      psSHP->adBoundsMin[1] = MIN( psSHP->adBoundsMin[1], vertex.lat );
      psSHP->adBoundsMax[1] = MAX( psSHP->adBoundsMax[1], vertex.lat );

      if ( psObject->GetZs() != nullptr )
      {
         psSHP->adBoundsMin[2] = MIN(psSHP->adBoundsMin[2], psObject->GetZs()[ 0 ] );
         psSHP->adBoundsMax[2] = MAX(psSHP->adBoundsMax[2], psObject->GetZs()[ 0 ] );
      }

      if ( psObject->GetMs() != nullptr )
      {
         psSHP->adBoundsMin[3] = MIN(psSHP->adBoundsMin[3], psObject->GetMs()[ 0 ] );
         psSHP->adBoundsMax[3] = MAX(psSHP->adBoundsMax[3], psObject->GetMs()[ 0 ] );
      }
   }
}  // ExpandBounds()


/************************************************************************/
/*                           SHPWriteObject()                           */
/*                                                                      */
/*      Write out the vertices of a new structure.  Note that it is     */
/*      only possible to write vertices at the end of the file.         */
/************************************************************************/

int CShapeRead::SHPWriteObject(SHPHandle psSHP, int nShapeId,
   SHPObject * psObject)
{
   psSHP->bUpdated = TRUE;

   /* -------------------------------------------------------------------- */
   /*      Ensure that shape object matches the type of the file it is     */
   /*      being written to.                                               */
   /* -------------------------------------------------------------------- */
   assert( psObject->nSHPType == psSHP->nShapeType
      || psObject->nSHPType == SHPT_NULL );

   /* -------------------------------------------------------------------- */
   /*      Ensure that -1 is used for appends.  Either blow an             */
   /*      assertion, or if they are disabled, set the shapeid to -1       */
   /*      for appends.                                                    */
   /* -------------------------------------------------------------------- */
   assert( nShapeId == -1
      || (nShapeId >= 0 && nShapeId < psSHP->nRecords) );

   if( nShapeId != -1 && nShapeId >= psSHP->nRecords )
      nShapeId = -1;

   /* -------------------------------------------------------------------- */
   /*      Add the new entity to the in memory index.                      */
   /* -------------------------------------------------------------------- */
   if( nShapeId == -1 && psSHP->nRecords+1 > psSHP->nMaxRecords )
   {
      AddToMemoryIndex(psSHP);
   }

   /* -------------------------------------------------------------------- */
   /*      Initialize record.                                              */
   /* -------------------------------------------------------------------- */
   uchar *pabyRec = (uchar *) malloc(
      psObject->m_cVertices * 4 * sizeof(double)
      + psObject->m_cParts * 8 + 128);

   int nRecordSize;
   if( psObject->nSHPType == SHPT_POLYGON
      || psObject->nSHPType == SHPT_POLYGONZ
      || psObject->nSHPType == SHPT_POLYGONM
      || psObject->nSHPType == SHPT_ARC
      || psObject->nSHPType == SHPT_ARCZ
      || psObject->nSHPType == SHPT_ARCM
      || psObject->nSHPType == SHPT_MULTIPATCH )
   {
      nRecordSize = ReadIntoRecordPolygonOrArc(psObject, pabyRec);
   }
   else if( psObject->nSHPType == SHPT_MULTIPOINT
      || psObject->nSHPType == SHPT_MULTIPOINTZ
      || psObject->nSHPType == SHPT_MULTIPOINTM )
   {
      nRecordSize = ReadIntoRecordMultipoint(psObject, pabyRec);
   }
   else if( psObject->nSHPType == SHPT_POINT
      || psObject->nSHPType == SHPT_POINTZ
      || psObject->nSHPType == SHPT_POINTM )
   {
      nRecordSize = ReadIntoRecordPoint(psObject, pabyRec);
   }
   else if( psObject->nSHPType == SHPT_NULL )
   {
      nRecordSize = 12;
   }
   else
   {
      /* unknown type */
      assert( FALSE );
   }

   int nRecordOffset = GetRecordOffset(psSHP, nShapeId, nRecordSize);

   SetRecordInformation( nShapeId, psObject, nRecordSize, pabyRec);

   bool write_success = WriteRecord(
      psSHP, nRecordOffset, nRecordSize, pabyRec);
   free(pabyRec);
   if (!write_success)
   {
      printf( "Error in fseek() or fwrite().\n" );
      return -1;
   }

   ExpandBounds(psSHP, psObject);

   return( nShapeId  );
}

void InitializeShapeObject(SHPObject* shape)
{
   shape->pDisplayProperties = nullptr;
}

void ClipVerticesToGeographicBounds (SHPObject& shape, int nPoints )
{
   SHPVertex* pVertices = shape.GetVertices();
   for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
   {
      pVertices[ iVertex ].lon = min( max( -180.0, pVertices[ iVertex ].lon ), +180.0 );
      pVertices[ iVertex ].lat = min( max( -90.0, pVertices[ iVertex ].lat ), +90.0 );
   }
}

bool CShapeRead::ReadIntoShapeObjectPolygonOrArc( SHPObject& shape, const uchar *pabyRec, int cRecSize )
{
   if ( 44 + 8 > cRecSize )
   {
      ATLASSERT( _T("Shape record header overrun") );
RecordError:
      shape.m_mFeatureMask |= SHPObject::FEATURE_RECORD_ERROR;
      return false;
   }
   /* -------------------------------------------------------------------- */

   /* Get the X/Y bounds.                                                  */
   /* -------------------------------------------------------------------- */
   memcpy( &shape.m_grcBounds.ll.lon, pabyRec + 8 +  4, 8 );
   memcpy( &shape.m_grcBounds.ll.lat, pabyRec + 8 + 12, 8 );
   memcpy( &shape.m_grcBounds.ur.lon, pabyRec + 8 + 20, 8 );
   memcpy( &shape.m_grcBounds.ur.lat, pabyRec + 8 + 28, 8 );

   if( bBigEndian )
   {
      SwapWord( 8, &shape.m_grcBounds.ll.lon );
      SwapWord( 8, &shape.m_grcBounds.ll.lat );
      SwapWord( 8, &shape.m_grcBounds.ur.lon );
      SwapWord( 8, &shape.m_grcBounds.ur.lat );
   }

   if ( m_pgptCaptureBndLL != nullptr && m_pgptCaptureBndUR != nullptr
      && !GEO_intersect(
            m_pgptCaptureBndLL->lat, m_pgptCaptureBndLL->lon,
            m_pgptCaptureBndUR->lat, m_pgptCaptureBndUR->lon,
            shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
            shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
         return false;     // Not interested in this

   int32 nPoints, nParts;
   int nOffset = 44 + 8;
;

   /* -------------------------------------------------------------------- */
   /*      Extract part/point count, and build vertex and part arrays      */
   /*      to proper size.                                                 */
   /* -------------------------------------------------------------------- */
   memcpy( &nPoints, pabyRec + 40 + 8, 4 );
   memcpy( &nParts, pabyRec + 36 + 8, 4 );

   if( bBigEndian ) 
   {  
      SwapWord( 4, &nPoints );
      SwapWord( 4, &nParts );
   }

   // check for valid data
   if ((nPoints < 0) || (nParts < 0))
      return false;

   InitializeShapeData( nParts, nPoints, shape );

   /* -------------------------------------------------------------------- */
   /*      Copy out the part array from the record.                        */
   /* -------------------------------------------------------------------- */
   if ( nOffset + ( 4 * nParts ) > cRecSize )
   {
      ATLASSERT( _T("Shape record parts overrun") );
      goto RecordError;
   }
   for ( int iPart = 0 ; iPart < nParts; iPart++ )
   {
      int32 i32;
      memcpy( &i32, pabyRec + nOffset + ( iPart * 4 ), 4 );
      if ( bBigEndian )
         SwapWord( 4, &i32 );

      SHPPart& part = shape.GetPart( iPart );
      part.part_start = i32;
      if ( i32 < 0 || i32 >= nPoints )
      {
         ATLASSERT( _T("Shape part-start out of range") );
         goto RecordError;
      }
   }
   nOffset += 4 * nParts;

 
   /* -------------------------------------------------------------------- */
   /*      If this is a multipatch, we will also have parts types.         */
   /* -------------------------------------------------------------------- */
   if ( shape.nSHPType == SHPT_MULTIPATCH )
   {
      if ( nOffset + ( 4 * nParts ) > cRecSize )
      {
         ATLASSERT( _T("Shape record multipatch overrun") );
         goto RecordError;
      }

      for ( int iPart = 0 ; iPart < nParts; iPart++ )
      {
         int32 i32;
         memcpy( &i32, pabyRec + nOffset + ( iPart * 4 ), 4);
         if ( bBigEndian )
            SwapWord( 4, &i32 );
         shape.GetPart( iPart ).part_type = i32;
      }
      nOffset += 4 * nParts;
   }

   /* -------------------------------------------------------------------- */
   /*      Copy out the vertices from the record.                          */
   /* -------------------------------------------------------------------- */
   if ( nOffset + ( 16 * nPoints ) > cRecSize )
      {
         ATLASSERT( _T("Shape record vextex points overrun") );
         goto RecordError;
      }

   SHPVertex* pVertices = shape.GetVertices();
   for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
   {
      SHPVertex& vertex = pVertices[ iVertex ];

      memcpy( &vertex.lon,
         pabyRec + nOffset + ( iVertex * 16 ) + 0,
         8 );

      memcpy( &vertex.lat,
         pabyRec + nOffset + ( iVertex * 16 ) + 8,
         8 );
      
      if ( bBigEndian)
      {
         SwapWord( 8, &vertex.lon );
         SwapWord( 8, &vertex.lat );
      }
   }

   ClipVerticesToGeographicBounds( shape, nPoints );

   nOffset += 16*nPoints;

   /* -------------------------------------------------------------------- */
   /*      If we have a Z coordinate, collect that now.                    */
   /* -------------------------------------------------------------------- */
   if ( ( shape.m_mFeatureMask & SHPObject::FEATURE_Z ) != 0 )
   {
      if ( nOffset + 16 + ( 8 * nPoints ) > cRecSize )
      {
         ATLASSERT( _T("Shape record Z overrun") );
         goto RecordError;
      }

      memcpy( &shape.dfZMin, pabyRec + nOffset + 0, 8 );
      memcpy( &shape.dfZMax, pabyRec + nOffset + 8, 8 );
      if ( bBigEndian )
      {
         SwapWord( 8, &shape.dfZMin );
         SwapWord( 8, &shape.dfZMax );
      }
      nOffset += 16;

      DOUBLE* pdZs = shape.GetZs();
      ATLASSERT( pdZs != nullptr && "Z pointer is missing" );
      for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
      {
         memcpy( pdZs + iVertex, pabyRec + nOffset, 8 );
         if ( bBigEndian)
            SwapWord( 8, pdZs + nOffset );
         nOffset += 8;
      }
   }

   /* -------------------------------------------------------------------- */
   /*      If we have M values, collect them now.                          */
   /* -------------------------------------------------------------------- */
   if ( ( shape.m_mFeatureMask & SHPObject::FEATURE_M ) != 0 )
   {
      if ( nOffset + 16 + ( 8 * nPoints ) > cRecSize )
      {
         ATLASSERT( _T("Shape record M overrun") );
         goto RecordError;
      }
      memcpy( &shape.dfMMin, pabyRec + nOffset + 0, 8 );
      memcpy( &shape.dfMMax, pabyRec + nOffset + 8, 8 );
      if ( bBigEndian )
      {
         SwapWord( 8, &shape.dfMMin );
         SwapWord( 8, &shape.dfMMax );
      }
      nOffset += 16;

      DOUBLE* pdMs = shape.GetMs();
      ATLASSERT( pdMs != nullptr && "M pointer is missing" );
      for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
      {
         memcpy( pdMs + iVertex, pabyRec + nOffset, 8 );
         if ( bBigEndian)
            SwapWord( 8, pdMs + nOffset );
         nOffset += 8;
      }
   }

   return true;
}

bool CShapeRead::ReadIntoShapeObjectMultipoint( SHPObject& shape, const uchar *pabyRec, int cRecSize )
{
   /* -------------------------------------------------------------------- */
   /* Get the X/Y bounds.                                                  */
   /* -------------------------------------------------------------------- */
   memcpy( &(shape.m_grcBounds.ll.lon), pabyRec + 8 +  4, 8 );
   memcpy( &(shape.m_grcBounds.ll.lat), pabyRec + 8 + 12, 8 );
   memcpy( &(shape.m_grcBounds.ur.lon), pabyRec + 8 + 20, 8 );
   memcpy( &(shape.m_grcBounds.ur.lat), pabyRec + 8 + 28, 8 );

   if( bBigEndian ) 
   {
      SwapWord( 8, &(shape.m_grcBounds.ll.lon) );
      SwapWord( 8, &(shape.m_grcBounds.ll.lat) );
      SwapWord( 8, &(shape.m_grcBounds.ur.lon) );
      SwapWord( 8, &(shape.m_grcBounds.ur.lat) );
   }
   
   if ( m_pgptCaptureBndLL != nullptr && m_pgptCaptureBndUR != nullptr
      && !GEO_intersect(
            m_pgptCaptureBndLL->lat, m_pgptCaptureBndLL->lon,
            m_pgptCaptureBndUR->lat, m_pgptCaptureBndUR->lon,
            shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
            shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
         return false;     // Not interested in this

   int32 nPoints;
   int nOffset;

   memcpy( &nPoints, pabyRec + 44, 4 );
   if( bBigEndian )
      SwapWord( 4, &nPoints );

   // Read in vertices
   InitializeShapeData( nPoints, nPoints, shape );
   SHPVertex* pVertices = shape.GetVertices();
   for ( int iVertex = 0; iVertex < nPoints; iVertex++ )
   {
      SHPVertex& vertex = pVertices[ iVertex ];

      memcpy( &vertex.lon, pabyRec + 48 + ( 16 * iVertex ) + 0, 8 );
      memcpy( &vertex.lat, pabyRec + 48 + ( 16 * iVertex ) + 8, 8 );
      if ( bBigEndian)
      {
         SwapWord( 8, &vertex.lon );
         SwapWord( 8, &vertex.lat );
      }
   }

   ClipVerticesToGeographicBounds(shape, nPoints);

   nOffset = 48 + 16*nPoints;

   /* -------------------------------------------------------------------- */
   /*      If we have a Z coordinate, collect that now.                    */
   /* -------------------------------------------------------------------- */
   if( shape.nSHPType == SHPT_MULTIPOINTZ )
   {
      nOffset += 16 + 8*nPoints;
   }

   CaptureMultiPointParts( nPoints, shape );
   return true;

}  // ReadIntoShapeObjectMultipoint()

bool CShapeRead::ReadIntoShapeObjectPoint( SHPObject& shape, const uchar *pabyRec, int cRecSize )
{
   /* -------------------------------------------------------------------- */
   /*      Since no extents are supplied in the record, we will apply      */
   /*      them from the single vertex.                                    */
   /* -------------------------------------------------------------------- */

   /* -------------------------------------------------------------------- */
   /* Get the X/Y bounds.                                                  */
   /* -------------------------------------------------------------------- */
   memcpy( &(shape.m_grcBounds.ll.lon), pabyRec + 8 + 4, 8 );
   memcpy( &(shape.m_grcBounds.ll.lat), pabyRec + 8 + 12, 8 );

   if( bBigEndian ) 
   {
      SwapWord( 8, &(shape.m_grcBounds.ll.lon) );
      SwapWord( 8, &(shape.m_grcBounds.ll.lat) );
   }

   // Clip vertex to geographic bounds
   shape.m_grcBounds.ur.lat = shape.m_grcBounds.ll.lat =
      __min( +90.0, __max( -90.0, shape.m_grcBounds.ll.lat ) );
   shape.m_grcBounds.ll.lon = shape.m_grcBounds.ur.lon =
      __min( +180.0, __max( -180.0, shape.m_grcBounds.ll.lon ) );

   if ( m_pgptCaptureBndLL != nullptr && m_pgptCaptureBndUR != nullptr
      && !GEO_intersect(
            m_pgptCaptureBndLL->lat, m_pgptCaptureBndLL->lon,
            m_pgptCaptureBndUR->lat, m_pgptCaptureBndUR->lon,
            shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
            shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
         return false;     // Not interested in this

   InitializeShapeData( 1, 1, shape );
   shape.GetVertices()[ 0 ].lat = shape.m_grcBounds.ll.lat;
   shape.GetVertices()[ 0 ].lon = shape.m_grcBounds.ll.lon;

   int nOffset = 20 + 8;

   /* -------------------------------------------------------------------- */
   /*      If we have a Z coordinate, collect that now.                    */
   /* -------------------------------------------------------------------- */
   if( shape.nSHPType == SHPT_POINTZ )
   {
      nOffset += 8;
   }

   // A point has a single part.
   int part_start = 0;
   CaptureParts( 1, &part_start, nullptr, shape );
   return true;
}

/************************************************************************/
/*                          SHPReadObject()                             */
/*                                                                      */
/*      Read the vertices, parts, and other non-attribute information   */
/* for one shape.                                                       */
/************************************************************************/

bool CShapeRead::SHPReadObject( SHPHandle psSHP, int hEntity, SHPObject& shape )
{
/* -------------------------------------------------------------------- */
/*      Validate the record/entity number.                              */
/* -------------------------------------------------------------------- */
    if( hEntity < 0 || hEntity >= psSHP->nRecords )
        return false;

/* -------------------------------------------------------------------- */
/*      Ensure our record buffer is large enough.                       */
/* -------------------------------------------------------------------- */
   int cRecSize = psSHP->panRecSize[ hEntity ] + 8;
   if ( m_apuchReadBuf.get() == NULL
         || m_nReadBufSize < cRecSize )
      m_apuchReadBuf.reset( new uchar[ m_nReadBufSize = cRecSize ] );
   uchar* pabyRec = m_apuchReadBuf.get();

/* -------------------------------------------------------------------- */
/*      Read the record.                                                */
/* -------------------------------------------------------------------- */
    fseek( psSHP->fpSHP, psSHP->panRecOffset[hEntity], 0 );
    fread( pabyRec, cRecSize, sizeof(BYTE), psSHP->fpSHP );

/* -------------------------------------------------------------------- */
/* Allocate and minimally initialize the object.                        */
/* -------------------------------------------------------------------- */
   InitializeShapeObject(&shape);

   shape.nShapeId = hEntity;

    memcpy( &shape.nSHPType, pabyRec + 8, 4 );
    if( bBigEndian )
       SwapWord( 4, &(shape.nSHPType) );

    switch ( shape.nSHPType )
    {
      case SHPT_POLYGON:
      case SHPT_POLYGONZ:
      case SHPT_POLYGONM:
         shape.m_mFeatureMask = SHPObject::FEATURE_POLYGON;
         break;

      case SHPT_POINT:
      case SHPT_MULTIPOINT:
      case SHPT_POINTZ:
      case SHPT_MULTIPOINTZ:
      case SHPT_POINTM:
      case SHPT_MULTIPOINTM:
         shape.m_mFeatureMask = SHPObject::FEATURE_POINT;
         break;

      case SHPT_ARC:
      case SHPT_ARCZ:
      case SHPT_ARCM:
         shape.m_mFeatureMask = SHPObject::FEATURE_POLYLINE;
         break;

      default:
         shape.m_mFeatureMask = SHPObject::FEATURE_NONE;
    }

    if ( HasM( shape.nSHPType ) )
       shape.m_mFeatureMask |= SHPObject::FEATURE_M;

    if ( HasZ( shape.nSHPType ) )
       shape.m_mFeatureMask |= SHPObject::FEATURE_Z;

     switch ( shape.nSHPType )
    {
      case SHPT_POLYGON:
      case SHPT_POLYGONZ:
      case SHPT_POLYGONM:
      case SHPT_ARC:
      case SHPT_ARCZ:
      case SHPT_ARCM:
      case SHPT_MULTIPATCH:
         return ReadIntoShapeObjectPolygonOrArc( shape, pabyRec, cRecSize );

      case SHPT_MULTIPOINT:
      case SHPT_MULTIPOINTM:
      case SHPT_MULTIPOINTZ:
         return ReadIntoShapeObjectMultipoint( shape, pabyRec, cRecSize );

      case SHPT_POINT:
      case SHPT_POINTM:
      case SHPT_POINTZ:
         return ReadIntoShapeObjectPoint( shape, pabyRec, cRecSize );

    } // switch ( nSHPType )

    return false;      // Unknown, ignore

}  // SHPReadObject()


/************************************************************************/
/*                            SHPTypeName()                             */
/************************************************************************/

const char * CShapeRead::SHPTypeName( int nSHPType )
{
    switch( nSHPType )
    {
      case SHPT_NULL:
        return "NullShape";

      case SHPT_POINT:
        return "Point";

      case SHPT_ARC:
        return "Arc";

      case SHPT_POLYGON:
        return "Polygon";

      case SHPT_MULTIPOINT:
        return "MultiPoint";
        
      case SHPT_POINTZ:
        return "PointZ";

      case SHPT_ARCZ:
        return "ArcZ";

      case SHPT_POLYGONZ:
        return "PolygonZ";

      case SHPT_MULTIPOINTZ:
        return "MultiPointZ";
        
      case SHPT_POINTM:
        return "PointM";

      case SHPT_ARCM:
        return "ArcM";

      case SHPT_POLYGONM:
        return "PolygonM";

      case SHPT_MULTIPOINTM:
        return "MultiPointM";

      case SHPT_MULTIPATCH:
        return "MultiPatch";

      default:
        return "UnknownShapeType";
    }
}

/************************************************************************/
/*                          SHPPartTypeName()                           */
/************************************************************************/

const char * CShapeRead::SHPPartTypeName( int nPartType )
{
    switch( nPartType )
    {
      case SHPP_TRISTRIP:
        return "TriangleStrip";
        
      case SHPP_TRIFAN:
        return "TriangleFan";

      case SHPP_OUTERRING:
        return "OuterRing";

      case SHPP_INNERRING:
        return "InnerRing";

      case SHPP_FIRSTRING:
        return "FirstRing";

      case SHPP_RING:
        return "Ring";

      default:
        return "UnknownPartType";
    }
}

// End of ShpRead.cpp
