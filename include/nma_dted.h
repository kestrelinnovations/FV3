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



#ifndef NMA_DTED_H 
#define NMA_DTED_H

// ------------------------------------------------------------------

#include "common.h"
#include "elevs.h"

//
// For latitudes lying on a zone boundary, this functions returns the dpp
// for the NORTHERNMOST zone.
//
int NDTD_get_max_elev_in_poly(combo_dted_hit_and_miss_cache *dted_cache1, 
							  combo_dted_hit_and_miss_cache *dted_cache2, 
							  int numpt, double *lat, double *lon, 
							  int *max_elev, int *types_used);


int NDTD_get_max_elev_in_poly(combo_dted_hit_and_miss_cache *dted_cache1, 
							  combo_dted_hit_and_miss_cache *dted_cache2,
							  int numpt, double *lat, double *lon, 
							  int *max_elev, double *max_elev_lat, double *max_elev_lon,
							  int *types_used);  // 1 = dted1, 2 = dted2, 4 = dted3


#endif
