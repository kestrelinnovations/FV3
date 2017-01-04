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



#ifndef ELEVS_H 
#define ELEVS_H 

// ---------------------------------------------------------------------

#include "common.h"
#include "geo_tool.h" // for GEO_in_bounds

#import "DtedMapServer.tlb" no_namespace named_guids

#ifdef _DEBUG

/*
 *  define this if you want to keep track of the number of hits and misses
 *  in the caches
 */
#if 0
#define TRACK_DTED_CACHE_HITS_AND_MISSES
#endif

#ifdef TRACK_DTED_CACHE_HITS_AND_MISSES
#include "errx.h"
#endif

#endif


// -------------------------------------------------------------------

//
// This file contains the definitions for the dted caches.
//
// These dted caches should be used when a lot of elevation 
// requests are going to be made in a certain area (for example, 
// terrain masking). They should not be used for infrequent 
// elevation requests (e.g. running elevations). 
//
// NOTE that it is important that each cache be constructed before use and
// destroyed afterward.  That is, you should not keep an object instance
// around between uses.  Keeping a cache around between uses can cause at
// leat two problems:
//
// 1) if a dted file is in a hit list of a cache, you will not be able
// to delete this file with the MDM (because the file is open)
// 2) If a file is in a miss list and a data source that contains that file
// is added subsequently, the file will not be found because it is in the
// miss list.
//
// Thus the objects should be created and destroyed with each use to avoid
// these problems.
//

// -------------------------------------------------------------------

#if 1 // GUE
#define DEFAULT_MAX_CACHE_SIZE 7
#else
#define DEFAULT_MAX_CACHE_SIZE 5
#endif

// -------------------------------------------------------------------

//
// Both the width and height of a CMS frame (from pixel center to 
// pixel center, not from pixel edge to pixel edge) are 1 degree.
//
#define CMS_DTED_FRAME_GEO_HEIGHT 1.0  // in degrees
#define CMS_DTED_FRAME_GEO_WIDTH  1.0  // in degrees

//
// Both the width and height of a NIMA frame (from pixel center to 
// pixel center, not from pixel edge to pixel edge) are 1 degree.
//
#define NIMA_DTED_FRAME_GEO_HEIGHT 1.0  // in degrees
#define NIMA_DTED_FRAME_GEO_WIDTH  1.0  // in degrees


// -------------------------------------------------------------------

//
// abstract class for a dted cache consisting of both a hit list and a 
// miss list
//
class base_dted_hit_and_miss_cache
{

public:

   base_dted_hit_and_miss_cache(void) {} 
   virtual ~base_dted_hit_and_miss_cache() {}

   //
   // If there is no elevation available at the requested location, 
   // found is set to FALSE. If the data is available, found is set to TRUE.
   //
   // Return SUCCESS or FAILURE.
   //
   virtual int get_elev_in_meters(degrees_t lat, degrees_t lon, 
      boolean_t* found, INT2* elev) = 0;
   virtual int get_elev_in_feet(degrees_t lat, degrees_t lon, 
      boolean_t* found, INT4* elev);
};

//
// abstract class for dted cache consisting of a single hit list and
// a single miss list
//
class base_dted_single_hit_and_miss_cache : 
  public base_dted_hit_and_miss_cache
{

};

//
// nima dted cache class consisting of a hit list and a miss list
//
class nima_dted_hit_and_miss_cache : public base_dted_single_hit_and_miss_cache
{

protected:

   int m_DTED_type;

   IDtedPtr m_smpDted;

public:

   nima_dted_hit_and_miss_cache(int max_hit_cache_size = DEFAULT_MAX_CACHE_SIZE,
      int max_miss_cache_size = DEFAULT_MAX_CACHE_SIZE);

   virtual ~nima_dted_hit_and_miss_cache();

   // we assume DTED level 1, unless set here
   void set_DTED_type(int t) { m_DTED_type = t; }

   //
   // If there is no elevation available at the requested location, 
   // found is set to FALSE. If the data is available, found is set to TRUE.
   //
   // Return SUCCESS or FAILURE.
   //
   int get_elev_in_meters(degrees_t lat, degrees_t lon, 
      boolean_t* found, INT2* elev);
};

// -------------------------------------------------------------------

//
// Combines a cms and nima cache.  That is, it will search for one type
// of dted first (the preferred type).  If it doesn't find the data for this
// type, it searched the non-preferred type.  NIMA DTED is set as the
// preferred type, and CMS DTED is the non-preferred type.
//
class combo_dted_hit_and_miss_cache : public base_dted_hit_and_miss_cache
{

protected:

   nima_dted_hit_and_miss_cache m_nima; 

   //
   // the pointers for the preferred and nonpreferred dted types
   //
   base_dted_single_hit_and_miss_cache* m_pref;

public:

   combo_dted_hit_and_miss_cache();

   void set_DTED_type(int t) { m_nima.set_DTED_type(t); }

   //
   // If there is no elevation available at the requested location, 
   // found is set to FALSE. If the data is available, found is set to TRUE.
   //
   // Return SUCCESS or FAILURE.
   //
   virtual int get_elev_in_meters(degrees_t lat, degrees_t lon, 
      boolean_t* found, INT2* elev);
};

// -------------------------------------------------------------------

#endif
