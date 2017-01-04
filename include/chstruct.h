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



//
// chstruct.h
//

//
// implements structures common to both echum and chum
//

#ifndef CHSTRUCT_H
#define CHSTRUCT_H 1

class LatLonPos 
{
public:
   int   degLat;
   int   minLat;
   int   secLat;
   char  dirLat;
   int   degLon;
   int   minLon;
   int   secLon;
   char  dirLon;

public:
   LatLonPos();

   BOOL is_valid();

   void dmsToDeg(degrees_t* lat, degrees_t* lon);
};

//
// CHUM Global structures/defines
//
#define MAX_LINE_LEN				4096


//-----------------------------------------------

// Classified output record types

enum recordTypes {
	ADD_RECORD,
	DELETE_RECORD,
	CHANGE_RECORD,
	RELOCATE_TO_RECORD,
	RELOCATE_FROM_RECORD
};

//-----------------------------------------------

// Classified output record types

enum classTypes {
	POINT_CLASS,
	LINE_CLASS
};

//-----------------------------------------------

// Miscellaneous structure definitions

struct filePosChain {
	long						filePos;
	struct filePosChain*	nextPos;
};

//-----------------------------------------------

// Data structure for the chart database

struct EditionList {
	char	editionName[25];
	int	editionNumber;
	char	editionDate[16];
	int	SerialNumber;
};

//-----------------------------------------------

#endif
