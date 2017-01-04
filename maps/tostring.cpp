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
#include "maps.h"

/* get map category as a string*/

CString MAP_get_category_string(MapCategory category)
{
   return category.get_string();
}

// get category string from source
CString MAP_get_category_string(MapSource source)
{
   return MAP_get_category(source).get_string();
}


/* get map source as a string */
CString MAP_get_source_string(MapSource source)
{
   return source.get_string();
}

CString MAP_get_scale_string(MapScale scale)
{
   return scale.get_string();
}

MapScale MAP_get_scale_from_string(CString this_scale)
{
	// note: this is a special case handler for resource dialogs which use a "Never" scale
   if (_stricmp("Never", this_scale) == 0)
      return WORLD;

	// special case for resource dialogs using "ALWAYS"
	if (_stricmp("ALWAYS", this_scale) == 0)
	{
		MapScale max_scale;
		max_scale.set_from_string("1:1");
		return max_scale;
	}

   MapScale scale;

   scale.set_from_string(this_scale);

   return scale;
}

// Converts source dependent scale string into a scale.  You need this function
// so "100 meter" can be converted to DTED_SCALE.
MapScale MAP_get_scale_from_string(MapSource source, CString scale)
{
   CList<MapType *, MapType *> map_list;

   // get list of map types for the given source
   if (MAP_get_map_list(source, map_list) != SUCCESS)
      return NULL_SCALE;

   // search the list of map types until map_type->get_scale_string() == scale
   MapType *map_type;
   POSITION next = map_list.GetHeadPosition();
   while (next)
   {
      map_type = map_list.GetNext(next);
      if (map_type->get_scale_string() == scale)
         return map_type->get_scale();
   }

   return MAP_get_scale_from_string(scale);
}
