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



// Segment.cpp

#include "stdafx.h"

#include "ovlelem.h"
#include "err.h"

// Constructor
GeoSegment::GeoSegment(heading_type_t heading_type) :
   m_heading_type(GREAT_CIRCLE)
{
   if (set_heading_type(heading_type) != SUCCESS)
   {
      ERR_report("set_heading_type() failed.");
      ASSERT(0);
   }
}

// Calculate the distance in meters and the bearing in degrees.
int GeoSegment::calc_range_and_bearing(d_geo_t start, d_geo_t end, 
   double &distance, double &bearing)
{
   return GEO_calc_range_and_bearing(start, end, distance, bearing,
      get_heading_type() == GREAT_CIRCLE);
}

// Calculate the end point from the start point, the distance in meters,
// and the bearing in degrees.
int GeoSegment::calc_end_point(d_geo_t start, double distance, double bearing,
   d_geo_t &end)
{
   return GEO_calc_end_point(start, distance, bearing, end, 
      get_heading_type() == GREAT_CIRCLE);
}

// Get the starting point for the segment.
void GeoSegment::get_start(degrees_t &lat, degrees_t &lon)
{
   d_geo_t point = get_start();

   lat = point.lat;
   lon = point.lon;
}


// Get the ending point for the segment.
void GeoSegment::get_end(degrees_t &lat, degrees_t &lon)
{
   d_geo_t point = get_end();

   lat = point.lat;
   lon = point.lon;
}

// If 0 <= distance <= get_length() then lat and lon will be set to the
// point at that distance from the start of the segment, heading
// will be set to the heading at that point along the segment, and 
// the function will return TRUE.  Otherwise FALSE is returned and
// lat, lon, and heading are unchanged.  distance is in meters. 
boolean_t GeoSegment::get_point(double distance, degrees_t &lat, degrees_t &lon, 
	degrees_t &heading)
{
   d_geo_t point;

   if (get_point(distance, point, heading))
   {
      lat = point.lat;
      lon = point.lon;
      return TRUE;
   }

   return FALSE;
}

// set the heading type: rhumb-line vs great circle
int GeoSegment::set_heading_type(heading_type_t heading_type)
{
   // no change - do nothing
   if (m_heading_type == heading_type)
      return SUCCESS;

   if (heading_type != GREAT_CIRCLE && heading_type != RHUMB_LINE && heading_type != SIMPLE_LINE)
   {
      ERR_report("Invalid heading type.");
      return FAILURE;
   }

   m_heading_type = heading_type;
   m_allow_redraw = FALSE;

   return SUCCESS;
}


// returns TRUE if the class name is GeoSegment
boolean_t GeoSegment::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "GeoSegment") == 0)
      return TRUE;

   return OvlContour::is_kind_of(class_name);
}

