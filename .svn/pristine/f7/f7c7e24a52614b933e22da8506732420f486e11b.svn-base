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
#include "maps.h"    // for MAP_zoom_percent_valid

boolean_t MAP_zoom_percent_valid(const MapScale& scale)
{
   /*
    *  So that you don't end up with a map whose bounds are larger than
    *  the screen/paper, disable zoom at the TIROS scale. Also disable at 
    *  1:10M, because below 10% zoom on 1280x1024 (and maybe at lower 
    *  resolutions) the bounds can also exceed the size of the world.
    */
   return (scale >= ONE_TO_5M && scale != MapScale(500, MAP_SCALE_METERS));
}

