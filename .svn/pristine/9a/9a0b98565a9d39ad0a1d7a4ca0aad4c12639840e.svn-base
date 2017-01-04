// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// fid.cpp



#include "stdafx.h"
#include "ComErrorObject.h"
#include "fid.h"

using namespace std;


// ****************************************************************
// ****************************************************************

int CFid::open( const CString& filename )
{
	FILE* fp = NULL;
   INT iResult = FAILURE;
   try
   {
      do
      {
	      INT pos;

         pos = filename.Right( 5 ).Find( _T('.') );
         if ( pos < 0 )
            m_filename = filename + _T(".fid"); // No extension found
         else
            m_filename = filename.Left( filename.ReverseFind( '.' ) + 1 ) + "fid";
   
         fopen_s( &fp, m_filename, "rt");
	      if ( fp == NULL )
		      return FAILURE;

	      char buf[201];

	      // read the sentinel string
	      fgets(buf, 200, fp);
	      if ( buf != strstr( buf, FID_FILE_SENTINAL ) )
            break;   // Failed

	      m_list.clear();

	      while (!feof(fp))
	      {
		      // read the next line
		      fgets(buf, 200, fp);
		      if ( feof(fp) )
               break;
	   	
            buf[200] = '\0';

		      CString tstr = buf;
		      tstr.TrimRight();
		      INT len = tstr.GetLength();
		      pos = tstr.Find(':');
		      if (pos > 0)
               m_list.insert( FidMap::value_type( tstr.Left( pos ), tstr.Mid( pos + 1 ) ) );
	      }

         iResult = SUCCESS;
      } while ( FALSE );
   } catch ( ... ) { }

   if ( fp != NULL )
	   fclose(fp);

	return iResult;
}
// end of open()


// ****************************************************************
// ****************************************************************

int CFid::save() const
{
	FILE *f = NULL;
   const int BUF_LEN = 200;
	char buf[BUF_LEN];

	if (m_filename.GetLength() < 5)
		return FAILURE;

   if ( m_list.empty() )
		return FAILURE;

   CreateAllDirectories( CT2W( m_filename ) );
	fopen_s(&f, m_filename, "wb");
	if (f == NULL)
		return FAILURE;

   fwrite( FID_FILE_SENTINAL "\r\n", strlen( FID_FILE_SENTINAL "\r\n" ), sizeof(CHAR), f );

   for ( FidMap::const_iterator it = m_list.begin(); it != m_list.end(); it++ )
   {
#ifdef UNICODE
      INT c = sprintf_s( buf, BUF_LEN, "%S: %S\r\n", it->first, it->second );
#else
      INT c = sprintf_s( buf, BUF_LEN, "%s: %s\r\n", it->first, it->second );
#endif
      if (c > -1)
         fwrite( buf, c, sizeof(CHAR), f );
   }

	// close file
	fclose(f);

	return SUCCESS;
}

// ****************************************************************
// ****************************************************************

int CFid::save_as( const CString& filename)
{
	m_filename = filename;
	return save();
}

// ****************************************************************
// ****************************************************************

int CFid::remove_key( const CString& key )
{
   return m_list.erase( key ) == 0 ? FAILURE : SUCCESS;
}

// ****************************************************************
// ****************************************************************

int CFid::get_key( const CString& key, CString& value ) const
{
   FidMap::const_iterator it = m_list.find( key );
   if ( it == m_list.end() )
      return FAILURE;

   value = it->second;
   return SUCCESS;
}


// ****************************************************************
// ****************************************************************

BOOL CFid::set_key( const CString& key, const CString& value )
{
   pair< FidMap::iterator, BOOL > p =
      m_list.insert( FidMap::value_type( key, value ) );
   return p.second;  // TRUE if new, FALSE if replacement
}


// ****************************************************************
// ****************************************************************

int CFid::get_projected_cs_type(int *cs_type, CString & cs_type_str) const
{
	BOOL found = FALSE;
	CString tstr, value;
	int rslt;

	rslt = get_key(FID_CS_TYPE, value);
	if (rslt == SUCCESS)
	{
		found = TRUE;
		*cs_type = atoi(value);
	}
	else
		return FAILURE;

	// get optional string
	rslt = get_key(FID_CS_TYPE_STR, value);
	if (rslt == SUCCESS)
	{
		cs_type_str = value;
	}

	return SUCCESS;
}

// ****************************************************************
// ****************************************************************

int CFid::set_projected_cs_type(int cs_type, const CString& cs_type_str)
{
	CString type_str;

	// check for valid cs type
	if ((cs_type < 26703) || (cs_type > 32760))
		return FAILURE;

	// check to see if it is already there
	remove_key(FID_CS_TYPE);
	remove_key(FID_CS_TYPE_STR);

	type_str.Format("%d", cs_type);
   m_list.insert( FidMap::value_type( FID_CS_TYPE, type_str ) );

   if (cs_type_str.GetLength() > 0)
      m_list.insert( FidMap::value_type( FID_CS_TYPE_STR, cs_type_str ) );

	return SUCCESS;
}

// ****************************************************************
// ****************************************************************

int CFid::get_histogram(unsigned int *hist) const // 256 luminance values
{
	int j, k, ndx, rslt, sum;
	CString key, value, tstr;

	if (hist == NULL)
		return FAILURE;

	sum = 0;
	try
	{
		for (k=0; k<16; k++)
		{
			key.Format("Histogram%02d", k);
			rslt = get_key(key, value);
			if (rslt != SUCCESS)
				return FAILURE;
			for (j=0; j<16; j++)
			{
				ndx = (k * 16) + j;
				tstr = value.Mid(j*5, 4);
				hist[ndx] = atoi(tstr);
				sum += hist[ndx];
			}
		}
	}
	catch (...)
	{
		return FAILURE;
	}

	if (sum < 256)
		return FAILURE;

	return SUCCESS;
}

// ****************************************************************
// ****************************************************************

int CFid::set_histogram(unsigned int *hist)  // 256 luminance value counts
{
	int j, k, maxval, val, ndx;
	CString key, value, tstr;

	if (hist == NULL)
		return FAILURE;

	try
	{
		// find the highest value
		maxval = 0;
		for (k=0; k<256; k++)
		{
			if ((int) hist[k] > maxval)
				maxval = hist[k];
		}

		// scale the histogram if necessary
		if (maxval > 9999)
		{
			double scale;

			scale = 9999.0 / (double) maxval;
			for (k=0; k<256; k++)
			{
				val = (int) (((double) hist[k] * scale) + 0.5);
				hist[k] = val;
			}
		}

		// set the histogram values
		for (k=0; k< 16; k++)
		{
			key.Format("Histogram%02d", k);
			value = "";
			for (j=0; j<16; j++)
			{
				ndx = (k * 16) + j;
				tstr.Format("%04d ", hist[ndx]);
				value += tstr;
			}
         m_list.insert( FidMap::value_type( key, value ) );
		}
	}
	catch (...)
	{
		return FAILURE;
	}

	return SUCCESS;
}
// end of set_histogram

// ****************************************************************
// ****************************************************************

int CFid::get_contrast(int *minval, int *ctrval, int *maxval) const
{
	int rslt;
	CString value;
	
	rslt = get_key(FID_CONTRAST, value);
	if (rslt != SUCCESS)
		return FAILURE;

	if (value.GetLength() < 11)
		return FAILURE;

	*minval = atoi(value.Mid(0, 3));
	*ctrval = atoi(value.Mid(4, 3));
	*maxval = atoi(value.Mid(8, 3));

	return SUCCESS;
}

// ****************************************************************
// ****************************************************************

int CFid::set_contrast(int minval, int ctrval, int maxval)
{
   CString tstr;
   if ( minval < 0 || minval > ctrval || ctrval > maxval || maxval > 255 )
      return FAILURE;

	remove_key(FID_CONTRAST);
	tstr.Format("%03d %03d %03d", minval, ctrval, maxval);
   m_list.insert( FidMap::value_type( FID_CONTRAST, tstr ) );

	return SUCCESS;
}


// ****************************************************************
// ****************************************************************

int CFid::get_position(int *startx, int *starty, double *zoom) const
{
	int rslt;
	CString value;
	
	rslt = get_key(FID_POSITION, value);
	if (rslt != SUCCESS)
		return FAILURE;

	if (value.GetLength() < 19)
		return FAILURE;

	*startx = atoi(value.Mid(0, 5));
	*starty = atoi(value.Mid(6, 5));
	*zoom = atof(value.Mid(12, 8));

	return SUCCESS;
}


// ****************************************************************
// ****************************************************************

int CFid::set_position(int startx, int starty, double zoom)
{
	CString tstr;

	if ((startx < 0) || (startx > 99999))
		return FAILURE;
	if ((starty < 0) || (starty > 99999))
		return FAILURE;
	if ((zoom <= 0.0) || (zoom > 8.0))
		return FAILURE;

	remove_key(FID_POSITION);
	tstr.Format("%05d %05d %08.6f", startx, starty, zoom);
   m_list.insert( FidMap::value_type( FID_POSITION, tstr ) );

	return SUCCESS;
}


// ****************************************************************
// ****************************************************************

// End of fid.cpp
