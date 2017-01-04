// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// MapDisplayParams.h

#ifndef FALCONVIEW_MAPDISPLAYPARAMS_H_
#define FALCONVIEW_MAPDISPLAYPARAMS_H_

struct MapDisplayParams
{
   MapDisplayParams() :
      auto_enhance_CIB(TRUE),
      brightness(0.0),
      bw_contrast(0.0),
      bw_contrast_midval(35),
      bw_brightness(0.0),
      surface_width(0),
      surface_height(0)
   {
   }

   // Brightness value for color maps
   double brightness;

   // Brightness and contrast values for black and white maps
   //
   double bw_contrast;
   int bw_contrast_midval;
   double bw_brightness;
   int auto_enhance_CIB;

   // Surface size of map
   int surface_width;
   int surface_height;
};

#endif  // FALCONVIEW_MAPDISPLAYPARAMS_H_