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
#include "elevs.h"
#include "errx.h"

// ---------------------------------------------------------------------------

static int get_elev(degrees_t lat, degrees_t lon, 
   INT2* file, degrees_t sf_ll_lat, degrees_t sf_ll_lon, 
   degrees_t sf_geo_width, degrees_t sf_geo_height, 
   int sf_width_in_pixels, int sf_height_in_pixels,
   int subframe_index, INT2* elev);

// ---------------------------------------------------------------------------

nima_dted_hit_and_miss_cache::nima_dted_hit_and_miss_cache(int max_hit_cache_size /*= DEFAULT_MAX_CACHE_SIZE*/,
                                                           int max_miss_cache_size /*= DEFAULT_MAX_CACHE_SIZE*/) :
   m_DTED_type(1)
{ 
   if (FAILED(m_smpDted.CreateInstance(__uuidof(Dted))))
   {
      ERR_report("Unable to create Dted object");
   }
}

nima_dted_hit_and_miss_cache::~nima_dted_hit_and_miss_cache()
{
   if (m_smpDted != NULL)
      m_smpDted->Terminate();
}

//
// Searches the hit cache for the elevation for the given lat/lon.
// If it's not found, the miss cache is searched (to see if the needed file
// is not available). 
// If the file is not in the miss cache, the online data sources are searched
// for the file of interest.
//
int nima_dted_hit_and_miss_cache::get_elev_in_meters(
   degrees_t lat, degrees_t lon, boolean_t* found, INT2* elev)
{
   if (m_smpDted != NULL)
   {
      long lElevation;
      short sDTEDLevelUsed;
      try
      {
         lElevation = m_smpDted->GetElevation(lat, lon, m_DTED_type, 
            DTED_ELEVATION_METERS, &sDTEDLevelUsed);
      }
      catch(_com_error &)
      {
         return FAILURE;
      }
      
      *found = (lElevation != PARTIAL_DTED_ELEVATION && lElevation != MISSING_DTED_ELEVATION);
      if (found)
         *elev = static_cast<short>(lElevation);
      return SUCCESS;
   }

   return FAILURE;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

inline static 
double meters_to_feet(INT2 meters)
{
   return METERS_TO_FEET(meters);
}

// -------------------------------------------------------------------

inline static 
INT4 round(double val)
{
   if (val >= 0.0)
      return ((INT4)(val+0.5));
   else
      return ((INT4)(val-0.5));
}

int base_dted_hit_and_miss_cache::get_elev_in_feet(degrees_t lat, 
   degrees_t lon, boolean_t* found, INT4* elev)
{
   INT2 elev_in_meters;

   // default bad value
   *elev = -32000;

   if (get_elev_in_meters(lat, lon, found, &elev_in_meters) != SUCCESS)
   {
      ERR_report("get_elev_in_meters");
      return FAILURE;
   }

   if (*found)
   {
      *elev = round(meters_to_feet(elev_in_meters));
   }

   return SUCCESS;
}

// -------------------------------------------------------------------------


combo_dted_hit_and_miss_cache::combo_dted_hit_and_miss_cache()
{
   //
   // set the preferred dted type to nima dted and the secondary type to
   // cms dted
   //
   m_pref = &m_nima;
}

//
// Search the preferred dted type for the elevation.  If it wasn't found,
// search the secondary dted type.
//
int combo_dted_hit_and_miss_cache::get_elev_in_meters(degrees_t lat,
   degrees_t lon, boolean_t* found, INT2* elev)
{
   if (m_pref->get_elev_in_meters(lat, lon, found, elev) != SUCCESS)
   {
      ERR_report("get_elev_in_meters");
      return FAILURE;
   }

   return SUCCESS;
}
