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

// MapSeries.cpp

#include "stdafx.h"

#include "maps_d.h"
#include "param.h"   // PRM_get_registry_string and PRM_set_registry_string

///////////////////////////////////////////////////////////////////////////////
// MapSeries Class Implementation

// Constructor
// Use deserialize() or the assignment operator to set objects created with
// the default contructor.
MapSeries::MapSeries() : m_string("") {}

// The given string is the user interface name for this series.  It will be
// returned by get_string().
MapSeries::MapSeries(const char *string) : m_string(string) {}

// Copy constructor
MapSeries::MapSeries(const MapSeries &series)
{
   m_string = series.get_string();
}

// Assignment
const MapSeries& MapSeries::operator =(const MapSeries &series)
{
   m_string = series.get_string();

   return *this;
}

// Comparison
BOOL operator ==(const MapSeries &s1, const MapSeries &s2)
{
   return (s1.get_string() == s2.get_string());
}

BOOL operator !=(const MapSeries &s1, const MapSeries &s2)
{
   return (s1.get_string() != s2.get_string());
}

BOOL operator ==(const MapSeries &series, const char *series_string)
{
   return (series.get_string() == series_string);
}

BOOL operator !=(const MapSeries &series, const char *series_string)
{
   return (series.get_string() != series_string);
}

// Read a MapSeries from the registry.
MapSeries MAP_get_registry_series(const char* section, const char* value_name, 
   const MapSeries &default_series)
{
   CString value = PRM_get_registry_string(section, value_name, default_series.get_string());

   MapSeries series((const char *)value);

   return series;
}

// Write a MapSeries to the registry.
int MAP_set_registry_series(const char* section, const char* value_name, 
   const MapSeries &series)
{
   return PRM_set_registry_string(section, value_name, series.get_string());
}
