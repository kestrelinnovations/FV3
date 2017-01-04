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



#include "stdafx.h"
#include "maps_d.h"
#include "err.h"

static const ProjectionID FV_EQUALARC_PROJECTION("Equal Arc" );
static const ProjectionID FV_LAMBERT_PROJECTION("Lambert Conformal Conic" );
static const ProjectionID FV_MERCATOR_PROJECTION("Mercator" );
static const ProjectionID FV_ORTHOGRAPHIC_PROJECTION("Orthographic" );
static const ProjectionID FV_AZIMUTHAL_EQUIDISTANT_PROJECTION("Azimuthal Equidistant");
static const ProjectionID FV_GENERAL_PERSPECTIVE_PROJECTION("General Perspective (3D)");

// Constructor
// Use deserialize() or the assignment operator to set objects created with
// the default contructor.
ProjectionID::ProjectionID() : SerializableString() {
}

// The given string is the user interface name for this source.  It will be
// returned by get_string().
ProjectionID::ProjectionID(const char *string) :
   SerializableString(string) 
{
}

// Copy constructor
ProjectionID::ProjectionID(const ProjectionID &source) : SerializableString() {
   m_string = source.m_string;
}

ProjectionID::ProjectionID(ProjectionEnum eProjectionType)
{
   switch(eProjectionType)
   {
   case EQUALARC_PROJECTION: *this = FV_EQUALARC_PROJECTION; break;
   case LAMBERT_PROJECTION: *this = FV_LAMBERT_PROJECTION; break;
   case MERCATOR_PROJECTION: *this = FV_MERCATOR_PROJECTION; break;
   case ORTHOGRAPHIC_PROJECTION: *this = FV_ORTHOGRAPHIC_PROJECTION; break;
   case AZIMUTHAL_EQUIDISTANT_PROJECTION: *this = FV_AZIMUTHAL_EQUIDISTANT_PROJECTION; break;
   case GENERAL_PERSPECTIVE_PROJECTION: *this = FV_GENERAL_PERSPECTIVE_PROJECTION; break;
   default:
      ERR_report("Invalid projection type");
   }
}

ProjectionEnum ProjectionID::GetProjectionType()
{
   if (*this == FV_EQUALARC_PROJECTION)
      return EQUALARC_PROJECTION;
   else if (*this == FV_LAMBERT_PROJECTION)
      return LAMBERT_PROJECTION;
   else if (*this == FV_MERCATOR_PROJECTION)
      return MERCATOR_PROJECTION;
   else if (*this == FV_ORTHOGRAPHIC_PROJECTION)
      return ORTHOGRAPHIC_PROJECTION;
   else if (*this == FV_AZIMUTHAL_EQUIDISTANT_PROJECTION)
      return AZIMUTHAL_EQUIDISTANT_PROJECTION;
   else if (*this == FV_GENERAL_PERSPECTIVE_PROJECTION)
      return GENERAL_PERSPECTIVE_PROJECTION;

   ERR_report("Unknown projection type");
   return EQUALARC_PROJECTION;
}

// Assignment
const ProjectionID& ProjectionID::operator =(const ProjectionID &source) {
   m_string = source.m_string;

   return *this;
}

// Comparison
BOOL operator ==(const ProjectionID &s1, const ProjectionID &s2) {
   return s1.m_string == s2.m_string;
}

BOOL operator !=(const ProjectionID &s1, const ProjectionID &s2) {
   return s1.m_string != s2.m_string;
}
