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



//ats route defines for route filter

//This is the flag for the kind of filter being set
#define ATS_TYPE			1

//type filters

//code in database "A"
#define ATS_TYPE_ATLANTIC	2
//code in database "B"
#define ATS_TYPE_BAHAMA		4
//code in database "C"
#define ATS_TYPE_CORRIDOR	8
//code in database "D"
#define ATS_TYPE_ADR		16
//code in database "E"
#define ATS_TYPE_DIRECT		32
//code in database "M"
#define ATS_TYPE_MILITARY	64
//code in database "N"
#define ATS_TYPE_NAR		128
//code in database "O"
#define ATS_TYPE_OCEANIC	256
//code in database "R"
#define ATS_TYPE_RNAV		512
//code in database "S"
#define ATS_TYPE_SUBSTITUTE	1024
//code in database "T"
#define ATS_TYPE_TACAN		2048
//code in database "W"
#define ATS_TYPE_AIRWAY		4096



//return codes


#define ATS_SUCCESS_NO_ERRORS		1

#define ATS_SUCCESS_NO_RECORDS		2

//data check return codes

#define ATS_FAILURE					4

#define ATS_DATA_CHECK_OK			8

#define ATS_DATA_CHECK_NO_WAYPOINTS 16

#define ATS_DATA_CHECK_NO_ATS		32

#define ATS_DATA_CHECK_NO_DATA		64

