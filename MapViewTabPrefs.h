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

// MapViewTabPrefs.h

#ifndef FALCONVIEW_MAP_VIEW_TAB_PREFS_H_
#define FALCONVIEW_MAP_VIEW_TAB_PREFS_H_

#include "FalconView/include/maps_d.h"

class MapViewTabPrefs
{
public:
   // Returns true if values for the given tab number exist in the registry.
   // Tab number zero will always true, with default values if necessary.
   bool Read(int tab_number, long* map_group_identity, MapType* map_type,
      MapProjectionParams* map_proj_params, std::string* tab_name);

   // Clear existing preferences before writing
   void Clear();

   void Write(int tab_number, long map_group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params,
      const std::string& tab_name);
};

#endif  // FALCONVIEW_MAP_VIEW_TAB_PREFS_H_
