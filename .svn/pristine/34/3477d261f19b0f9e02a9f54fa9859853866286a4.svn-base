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




/*------------------------------------------------------------------
-  FILE NAME:    grid_properties.h
-  LIBRARY NAME: grid_map.lib
-  PROGRAMMER:   Robert Santiago
-  DATE:         June 1999
-
-  DESCRIPTION:
-      
-
-  FUNCTIONS:
-
-      
-  REVISION HISTORY: 
-
-      $Log: grid_line.h $
 * Revision 1.1  1999/06/11  013:51:05  robert
 * Initial revision
 * 
-------------------------------------------------------------------*/

#ifndef GRID_SPACING_H
#define GRID_SPACING_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"
#include "geo_tool_d.h"
#include "maps_d.h" 

typedef struct {
   degrees_t major_grid_line_spacing;
   degrees_t minor_grid_line_spacing;
   degrees_t major_tick_spacing;
   degrees_t minor_tick_spacing;
} grid_spacing_t;

class ActiveMap;


// Grid (Tick) spacing class for the lat-lon grid overlay
class GridSpacing 
{
public:
   // Constructor
   GridSpacing();

   // Destructor
   ~GridSpacing();

public:   
   int get_grid_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *major_grid_spacing, degrees_t *minor_grid_spacing );
   
   int get_major_gridline_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *major_grid_spacing );
   int get_minor_gridline_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *minor_grid_spacing );
   
   int get_tick_spacing( grid_line_type_t grid_line_type, MapScale scale, 
         degrees_t *major_tick_spacing, degrees_t *minor_tick_spacing );
  
   int get_major_tick_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *major_tick_spacing );
   int get_minor_tick_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *minor_tick_spacing );
   
private:
   int query_grid_spacing_table(grid_line_type_t grid_line_type, 
         MapScale scale, grid_spacing_t *grid_params );

   // Select the scale that matches or it is closest to the current scale
   int get_closest_scale( const map_scale_t &cur_scale, MapScale *closest_scale);

//friend int test( GridSpacing *grd_spacing );
};
#endif // GRID_SPACING_H
