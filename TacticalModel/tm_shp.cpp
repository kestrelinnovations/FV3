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

// tm_shp.cpp -- Shape file overlay

#include "stdafx.h"
#include "mem.h"
#include "model_util.h"
#include "map.h"
#include "tm_shp.h"


#if UINT_MAX == 65535
typedef long         int32;
#else
typedef int       int32;
#endif

#ifndef FALSE
#  define FALSE      0
#  define TRUE    1
#endif

#define ByteCopy( a, b, c )   memcpy( b, a, c )
#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif


// ********************************************************************
// ********************************************************************

C_tm_shp_read::C_tm_shp_read()
{
   pabyRec = NULL;
   nBufSize = 0;
}

// ********************************************************************
// ********************************************************************

C_tm_shp_read::~C_tm_shp_read()
{
}



/************************************************************************/
/*                              SwapWord()                              */
/*                                                                      */
/*      Swap a 2, 4 or 8 byte word.                                     */
/************************************************************************/

static void SwapWord( int length, void * wordP )

{
    int     i;
    uchar   temp;

    for( i=0; i < length/2; i++ )
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

/************************************************************************/
/*                              SHPOpen()                               */
/*                                                                      */
/*      Open the .shp and .shx files based on the basename of the       */
/*      files or either file name.                                      */
/************************************************************************/
   
shp_handle C_tm_shp_read::SHPOpen( const char * pszLayer, const char * pszAccess )

{
    char    *pszFullname, *pszBasename;
    shp_handle    psSHP;
    
    uchar      *pabyBuf;
    int        i;
    double     dValue;
    
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
/* Initialize the info structure.               */
/* -------------------------------------------------------------------- */
    psSHP = (shp_handle) malloc(sizeof(shp_info_t));

    psSHP->bUpdated = FALSE;

/* -------------------------------------------------------------------- */
/* Compute the base (layer) name.  If there is any extension   */
/* on the passed in filename we will strip it off.       */
/* -------------------------------------------------------------------- */
    pszBasename = (char *) malloc(strlen(pszLayer)+5);
    strcpy_s( pszBasename, strlen(pszLayer)+5, pszLayer );
    for( i = strlen(pszBasename)-1; 
    i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
          && pszBasename[i] != '\\';
    i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

/* -------------------------------------------------------------------- */
/* Open the .shp and .shx files.  Note that files pulled from  */
/* a PC to Unix with upper case filenames won't work!    */
/* -------------------------------------------------------------------- */
    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf_s( pszFullname, strlen(pszBasename) + 5, "%s.shp", pszBasename );
    fopen_s(&(psSHP->fpSHP), pszFullname, pszAccess );
    if( psSHP->fpSHP == NULL )
    {
        sprintf_s( pszFullname, strlen(pszBasename) + 5,"%s.SHP", pszBasename );
        fopen_s(&(psSHP->fpSHP), pszFullname, pszAccess );
    }
    
    if( psSHP->fpSHP == NULL )
    {
        free( psSHP );
        free( pszBasename );
        free( pszFullname );
        return( NULL );
    }

    sprintf_s( pszFullname, strlen(pszBasename) + 5,"%s.shx", pszBasename );
    fopen_s(&(psSHP->fpSHX), pszFullname, pszAccess );
    if( psSHP->fpSHX == NULL )
    {
        sprintf_s( pszFullname, strlen(pszBasename) + 5,"%s.SHX", pszBasename );
        fopen_s(&(psSHP->fpSHX), pszFullname, pszAccess );
    }
    
    if( psSHP->fpSHX == NULL )
    {
      CString msg;
      msg.Format("The file - %s - is missing.", pszFullname);
//    AfxMessageBox(msg);
        fclose( psSHP->fpSHP );
        free( psSHP );
        free( pszBasename );
        free( pszFullname );
        return( NULL );
    }

    free( pszFullname );
    free( pszBasename );

/* -------------------------------------------------------------------- */
/*  Read the file size from the SHP file.          */
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

      return( NULL );
    }

/* -------------------------------------------------------------------- */
/*      Read the bounds.                                                */
/* -------------------------------------------------------------------- */
    memcpy( &dValue, pabyBuf+36, 8 );
    psSHP->adBoundsMin[0] = dValue;

    memcpy( &dValue, pabyBuf+44, 8 );
    psSHP->adBoundsMin[1] = dValue;

    memcpy( &dValue, pabyBuf+52, 8 );
    psSHP->adBoundsMax[0] = dValue;

    memcpy( &dValue, pabyBuf+60, 8 );
    psSHP->adBoundsMax[1] = dValue;

    memcpy( &dValue, pabyBuf+68, 8 );
    psSHP->adBoundsMin[2] = dValue;
    
    memcpy( &dValue, pabyBuf+76, 8 );
    psSHP->adBoundsMax[2] = dValue;
    
    memcpy( &dValue, pabyBuf+84, 8 );
    psSHP->adBoundsMin[3] = dValue;

    memcpy( &dValue, pabyBuf+92, 8 );
    psSHP->adBoundsMax[3] = dValue;

    free( pabyBuf );

/* -------------------------------------------------------------------- */
/* Read the .shx file to get the offsets to each record in  */
/* the .shp file.                   */
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
      int32    nOffset, nLength;

      memcpy( &nOffset, pabyBuf + i * 8, 4 );
      SwapWord( 4, &nOffset );

      memcpy( &nLength, pabyBuf + i * 8 + 4, 4 );
      SwapWord( 4, &nLength );

      psSHP->panRecOffset[i] = nOffset*2;
      psSHP->panRecSize[i] = nLength*2;
    }
    free( pabyBuf );

    return( psSHP );
}

/************************************************************************/
/*                              SHPClose()                              */
/*                               */
/* Close the .shp and .shx files.               */
/************************************************************************/

void C_tm_shp_read::SHPClose(shp_handle psSHP )

{
/* -------------------------------------------------------------------- */
/*      Free all resources, and close files.                            */
/* -------------------------------------------------------------------- */
    free( psSHP->panRecOffset );
    free( psSHP->panRecSize );

    fclose( psSHP->fpSHX );
    fclose( psSHP->fpSHP );

    free( psSHP );

    if( pabyRec != NULL )
    {
        free( pabyRec );
        pabyRec = NULL;
        nBufSize = 0;
    }
}

/************************************************************************/
/*                             SHPGetInfo()                             */
/*                                                                      */
/*      Fetch general information about the shape file.                 */
/************************************************************************/

void C_tm_shp_read::SHPGetInfo(shp_handle psSHP, int * pnEntities, int * pnShapeType,
           double * padfMinBound, double * padfMaxBound )

{
    int     i;
    
    if( pnEntities != NULL )
        *pnEntities = psSHP->nRecords;

    if( pnShapeType != NULL )
        *pnShapeType = psSHP->nShapeType;

    for( i = 0; i < 4; i++ )
    {
        if( padfMinBound != NULL )
            padfMinBound[i] = psSHP->adBoundsMin[i];
        if( padfMaxBound != NULL )
            padfMaxBound[i] = psSHP->adBoundsMax[i];
    }
}

// Read only the necessary elements of a shape file record without having
// to allocate a 104 byte shp_obj_t
int C_tm_shp_read::SHPReadObjectLatLonZ(shp_handle psSHP, int hEntity,
   double* lat, double* lon, double* z)
{
   // Validate the record/entity number
   if (hEntity < 0 || hEntity >= psSHP->nRecords)
      return FAILURE;

   // Grow record buffer as necessary
   if (psSHP->panRecSize[hEntity]+8 > nBufSize)
   {
      nBufSize = psSHP->panRecSize[hEntity]+8;
      pabyRec = (uchar *)SfRealloc(pabyRec,nBufSize);
   }

   // Read the record
   fseek(psSHP->fpSHP, psSHP->panRecOffset[hEntity], 0);
   fread(pabyRec, psSHP->panRecSize[hEntity]+8, 1, psSHP->fpSHP);

   // Get the shape type. Only POINT or POINTZ types are supported by this
   // method
   int shape_type;
   memcpy(&shape_type, pabyRec + 8, 4);
   if (shape_type != SHPT_POINT && shape_type != SHPT_POINTZ)
      return FAILURE;

   memcpy(lon, pabyRec + 12, 8);
   memcpy(lat, pabyRec + 20, 8);

   if (shape_type == SHPT_POINTZ)
      memcpy(z, pabyRec + 28, 8);

   return SUCCESS;
}

int C_tm_shp_read::get_cdb_data(CDbase *dbf, int recnum,
   int cnam_index, CString & cnam,
   int ao1_index, double *ao1,
   int scalx_index, double* scale_x,
   int scaly_index, double* scale_y,
   int scalz_index, double* scale_z)
{
   char data[256];
   int numrec, k, rslt;
   double tf;
   int ti;

   tf = 0.0;
   ti = 0;

   numrec = dbf->GetNumberOfRecords();
   if (numrec < 1)
   {
      return FAILURE;
   }

   if (recnum > numrec)
   {
      return FAILURE;
   }

   rslt = dbf->GotoRec(recnum);
   if (rslt != DB_NO_ERROR)
   {
      k = numrec;
      return FAILURE;
   }

   rslt = dbf->GetFieldDataByIndex(cnam_index, data);
   if (rslt != SUCCESS)
   {
      return FAILURE;
   }
   else
   {
      cnam = data;
      cnam.TrimRight();
   }

   rslt = dbf->GetFieldDataByIndex(ao1_index, data);
   if (rslt != SUCCESS)
   {
      return FAILURE;
   }
   
    tf = atof(data);
   if ((tf >= -360.0) && (tf <= 360.0))
      *ao1 = tf;
   else
      *ao1 = 0.0;
 
   if (scalx_index < 0)
      return SUCCESS;

   rslt = dbf->GetFieldDataByIndex(scalx_index, data);
   if (rslt != SUCCESS && scale_x)
   {
      *scale_x = 1.0;
   }
   *scale_x = atof(data);

   rslt = dbf->GetFieldDataByIndex(scaly_index, data);
   if (rslt != SUCCESS)
   {
      *scale_y = 1.0;
   }
   *scale_y = atof(data);

   rslt = dbf->GetFieldDataByIndex(scalz_index, data);
   if (rslt != SUCCESS)
   {
      *scale_z = 1.0;
   }
   *scale_z = atof(data);
 
   return SUCCESS;
}

int C_tm_shp_read::get_cnam(CDbase* dbf, int recnum, int cnam_index,
   CString& cnam)
{
   int numrec = dbf->GetNumberOfRecords();
   if (numrec < 1 || recnum > numrec)
      return FAILURE;

   if (dbf->GotoRec(recnum) != DB_NO_ERROR)
      return FAILURE;

   char data[FIELD_DATA_T_LEN];
   if (dbf->GetFieldDataByIndex(cnam_index, data) == SUCCESS)
   {
      cnam = data;
      cnam.TrimRight();
      return SUCCESS;
   }
   else
      return FAILURE;
}