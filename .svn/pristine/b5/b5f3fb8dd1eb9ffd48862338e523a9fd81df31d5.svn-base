// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#ifndef FALCONVIEW_HUDPROPERTIES_H_
#define FALCONVIEW_HUDPROPERTIES_H_

struct HudProperties
{
   int hud_on;

   COLORREF color;
   int attitude_on;
   int extra_information_on;
   int show_heading_only;

   double max_alt_meters;
   double max_velocity_kts;

   d_geo_t target;
   double target_alt_meters;
};

namespace fvw_prefs
{
   HudProperties LoadHudProperties();
   void StoreHudProperties(const HudProperties& hud_properties);
};

#endif  // FALCONVIEW_HUDPROPERTIES_H_