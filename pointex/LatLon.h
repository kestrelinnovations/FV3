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

// LatLon.h: interface for the CLatLonCoordinate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LATLON_H__A8FF06E7_F024_11D1_8F0E_00104B242B5F__INCLUDED_)
#define AFX_LATLON_H__A8FF06E7_F024_11D1_8F0E_00104B242B5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <iostream>
#include "common.h"
#include "geo_tool_d.h"

class CLatLonCoordinate  
{
   
public:
   degrees_t lat;
   degrees_t lon;

   inline void AssertInvariant() const
   {
      ASSERT( lat >= -90 && lat <= 90);
      ASSERT( lon >= -180 && lon <= 180);
   }

	CLatLonCoordinate();
   CLatLonCoordinate(degrees_t new_lat, degrees_t new_lon);
   CLatLonCoordinate(const CLatLonCoordinate & rhs );
   CLatLonCoordinate(d_geo_t rhs );

   void SetLat(degrees_t new_lat) {lat = new_lat;  AssertInvariant();}
   void SetLon(degrees_t new_lon) {lon = new_lon;  AssertInvariant();}

  	virtual ~CLatLonCoordinate();

   friend std::ostream& operator <<( std::ostream& os, const CLatLonCoordinate &coord );
   friend std::istream& operator >> ( std::istream& is, CLatLonCoordinate &coord );

   CLatLonCoordinate& operator = ( const CLatLonCoordinate& rhs);
   bool operator == ( const CLatLonCoordinate& rhs);

   CString toString();

};


inline std::ostream& operator << ( std::ostream& os, const CLatLonCoordinate &coord )
{
  os << coord.lat << ' ' << coord.lon;
  return os;
}

inline std::istream& operator >> ( std::istream& is, CLatLonCoordinate &coord )
{
  is >> coord.lat >> coord.lon;
  return is;
}

#endif // !defined(AFX_LATLON_H__A8FF06E7_F024_11D1_8F0E_00104B242B5F__INCLUDED_)
