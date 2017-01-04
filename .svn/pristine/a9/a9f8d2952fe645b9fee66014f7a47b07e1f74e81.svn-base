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

// LatLon.cpp: implementation of the CLatLonCoordinate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LatLon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLatLonCoordinate::CLatLonCoordinate() :lat (0), lon(0)
{
   AssertInvariant();  //check invarient (same idea as ASSERT_VALID):
}

CLatLonCoordinate::CLatLonCoordinate(const CLatLonCoordinate& rhs ) 
   :lat (rhs.lat), lon(rhs.lon)
{
   AssertInvariant();  //check invarient (same idea as ASSERT_VALID):
}

CLatLonCoordinate::CLatLonCoordinate(d_geo_t rhs ) 
   :lat (rhs.lat), lon(rhs.lon)
{
   AssertInvariant();  //check invarient (same idea as ASSERT_VALID):
}

CLatLonCoordinate::CLatLonCoordinate(degrees_t new_lat, degrees_t new_lon) 
   : lat (new_lat) , lon (new_lon)
{
   AssertInvariant();  //check invarient (same idea as ASSERT_VALID):
}


CLatLonCoordinate::~CLatLonCoordinate()
{
}

CLatLonCoordinate& CLatLonCoordinate::operator = ( const CLatLonCoordinate& rhs)
{
   //check for assignment to self
   //NOTE: this only works for single inheritance
   if (this == &rhs) 
      return *this;

   lat = rhs.lat;
   lon = rhs.lon;

   AssertInvariant();

   return *this;
}

bool CLatLonCoordinate::operator == ( const CLatLonCoordinate& rhs)
{
   return ( ( lat == rhs.lat ) && ( lon == rhs.lon ));
}

CString CLatLonCoordinate::toString()
{
   CString result;
   result.Format("(%f, %f)",lat, lon);
   return result;
}



