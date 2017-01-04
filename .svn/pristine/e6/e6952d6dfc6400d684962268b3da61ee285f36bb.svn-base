// Copyright (c) 1994-2010 Georgia Tech Research Corporation, Atlanta, GA
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

// fid.h



#pragma once

#include <map>

#ifndef SUCCESS
   #define SUCCESS 0
#endif
#ifndef FAILURE
   #define FAILURE -1
#endif


#define FID_CS_TYPE "Projected CS Type"
#define FID_CS_TYPE_STR    "Projected CS Type Text"
#define FID_HIST1	         "Histogram1"
#define FID_HIST2	         "Histogram2"
#define FID_HIST3	         "Histogram3"
#define FID_HIST4	         "Histogram4"
#define FID_HIST5	         "Histogram5"
#define FID_HIST6	         "Histogram6"
#define FID_HIST7	         "Histogram7"
#define FID_HIST8	         "Histogram8"
#define FID_DATA_SCALE     "ImageDataScale"
#define FID_DATA_OFFSET    "ImageDataOffset"
#define FID_CONTRAST       "Contrast Values"
#define FID_POSITION       "Position Values"
#define FID_FILE_SENTINAL  "FalconView Image Data"


// ****************************************************************
// ****************************************************************

class CFid
{
public:
	CString m_filename;

   typedef std::map< CString, CString > FidMap;
   FidMap m_list;

	int open( const CString& filename );
	int save() const;
	int save_as( const CString& filename);
	int get_projected_cs_type(int *cs_type, CString & cs_type_str) const;
	int set_projected_cs_type(int cs_type, const CString& cs_type_str);
	int get_histogram(unsigned int *hist) const;  // 256 luminance values
	int set_histogram(unsigned int *hist);  // 256 luminance values
	int get_contrast(int *minval, int *ctrval, int *maxval) const;
	int set_contrast(int minval, int ctrval, int maxval);
	int get_position(int *startx, int *starty, double *zoom) const;
	int set_position(int startx, int starty, double zoom);


	int remove_key( const CString& key );
	int get_key( const CString& key, CString& value ) const;
	BOOL set_key( const CString& key, const CString& value );   // TRUE if new, FALSE if replacement

}; // class CFid

// End of fid.h
