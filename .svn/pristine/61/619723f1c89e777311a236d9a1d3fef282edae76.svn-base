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

// surf_lim.h
// This file contains functions dealing with surface size related limitations
// on scale, rotation, and scale-percent for to-scale maps.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_SURF_LIM_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_SURF_LIM_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/maps_d.h"
// this project's headers
// forward definitions

// This function will test implied to-scale map to see if it violates any
// limits on surface size, rotation, or scale_percent for the given map
// type.  It returns true if the map is invalid and false if it is O.K.
// show_message == true causes this function to display a message explaining
// what is wrong in the event of a violation.
bool prn_settings_violate_limits(const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   degrees_t center_lat, degrees_t center_lon, double rotation,
   double width_inches, double height_inches, int scale_percent,
   bool show_message = true);

// Figure out the smallest scale that is going to be valid for a map
// on a page_width x page_height surface.  Find the largest scale that
// has a minimum scale factor that is greater than the
// minimum_scale_factor_desired.  The scale is returned in
// scale_with_scale_factor_limit and the scale factor is returned in
// scale_factor_limit.
int prn_get_limits_for_surface(MapCategory category, d_geo_t center,
   double page_width, double page_height, int minimum_scale_factor_desired,
   MapScale &smallest_valid_scale, map_scale_t &scale_with_scale_factor_limit,
   int &scale_factor_limit);

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_SURF_LIM_H_
