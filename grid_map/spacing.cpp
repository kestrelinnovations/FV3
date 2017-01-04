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
-  FILE NAME:    grid_properties.cpp
-  LIBRARY NAME: grid_map.lib
-
-  DESCRIPTION:
-      This file contains the Lat-Long grid line implementation routines.
-
-  PUBLIC FUNCTIONS:
-
-
-  PRIVATE FUNCTIONS: NONE
-
-  STATIC FUNCTIONS:
-
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
-
-  REVISION HISTORY:
-
-       $Log: grid_ovl.cpp $
 * Revision 1.1  1999/06/11  14:12:24  Robert
 * Initial revision
 * 
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/


#include "stdafx.h"

#include "grid_map_d.h"
#include "spacing.h"
#include "mapx.h"


static 
int test( GridSpacing *grd_spacing );

const int max_scales = 18;

// Grid and tick spacing LUT
static grid_spacing_t grid_spacing_table[][2] = 
   {//{ {    Latitude        }, {     Longitude       }}
      {{45.0, 45.0, 15.0, 5.0}, {90.0, 45.0, 15.0, 5.0}}, // WORLD = 0,  
      {{60.0, 30.0, 10.0, 5.0}, {60.0, 30.0, 10.0, 5.0}}, // ONE_TO_100M, 
      {{45.0, 15.0, 5.0, 1.0}, {45.0, 15.0, 5.0, 1.0}}, // ONE_TO_50M,  
      {{30.0, 10.0, 5.0, 1.0}, {30.0, 10.0, 5.0, 1.0}}, // ONE_TO_20M,  
      {{10.0, 5.0, 1.0, MIN_TO_DEG(12)}, {10.0, 5.0, 1.0, MIN_TO_DEG(12)}}, // ONE_TO_10M,  
      {{5.0, 1.0, MIN_TO_DEG(15), MIN_TO_DEG(5)}, {5.0, 1.0, MIN_TO_DEG(15), MIN_TO_DEG(5)}}, // ONE_TO_5M,  
      {{1.0, 1.0, MIN_TO_DEG(10), MIN_TO_DEG(2)}, {3.0, 1.0, MIN_TO_DEG(10), MIN_TO_DEG(5)}}, // ONE_TO_2M,  
      {{1.0, 1.0, MIN_TO_DEG(5), MIN_TO_DEG(1)},  {1.0, 1.0, MIN_TO_DEG(5), MIN_TO_DEG(1)}}, // ONE_TO_1M, 
      {{MIN_TO_DEG(30), MIN_TO_DEG(30), MIN_TO_DEG(5), MIN_TO_DEG(1)}, 
                  {MIN_TO_DEG(30), MIN_TO_DEG(30), MIN_TO_DEG(5), MIN_TO_DEG(1)}},// ONE_TO_500K, 
      {{MIN_TO_DEG(15), MIN_TO_DEG(15), MIN_TO_DEG(5), MIN_TO_DEG(1)}, 
                  {MIN_TO_DEG(15), MIN_TO_DEG(15), MIN_TO_DEG(5), MIN_TO_DEG(1)}}, // ONE_TO_200K, 
      {{MIN_TO_DEG(5), MIN_TO_DEG(5), 0.0, 0.0}, {MIN_TO_DEG(5), MIN_TO_DEG(5), 0.0, 0.0}}, // ONE_TO_100K,
      {{MIN_TO_DEG(1), MIN_TO_DEG(1), 0.0, 0.0}, {MIN_TO_DEG(1),MIN_TO_DEG(1), 0.0, 0.0}}, // ONE_TO_50K,  
      {{MIN_TO_DEG(1), MIN_TO_DEG(1), 0.0, 0.0}, {MIN_TO_DEG(1), MIN_TO_DEG(1), 0.0, 0.0}}, // ONE_TO_20K,  
      {{SEC_TO_DEG(15), SEC_TO_DEG(15), 0.0, 0.0}, {SEC_TO_DEG(15), SEC_TO_DEG(15), 0.0, 0.0}}, // ONE_TO_10K,  
      {{SEC_TO_DEG(10), SEC_TO_DEG(10), 0.0, 0.0}, {SEC_TO_DEG(10), SEC_TO_DEG(10), 0.0, 0.0}}, // ONE_TO_5K,   
      {{SEC_TO_DEG(5), SEC_TO_DEG(5), 0.0, 0.0}, {SEC_TO_DEG(5), SEC_TO_DEG(5), 0.0, 0.0}}, // ONE_TO_2K,   
      {{SEC_TO_DEG(5), SEC_TO_DEG(5), 0.0, 0.0}, {SEC_TO_DEG(5), SEC_TO_DEG(5), 0.0, 0.0}}, // ONE_TO_1K,   
      {{0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}}, // NULL_SCALE
   };

// grid_scale_table() corrects a static order initialization problem that can occur if grid_scale_table were just a global array

MapScale& grid_scale_table(int i)
{
   static MapScale grid_scale_table[max_scales] = {
              WORLD,             
              MapScale(100000000), //ONE_TO_80M,    
              MapScale(50000000), //ONE_TO_40M,    
              ONE_TO_20M,    
              ONE_TO_10M,    
              ONE_TO_5M,     
              ONE_TO_2M,     
              ONE_TO_1M,     
              ONE_TO_500K,   
              ONE_TO_200K,   
              ONE_TO_100K,   
              ONE_TO_50K,    
              ONE_TO_20K,    
              ONE_TO_10K,    
              ONE_TO_5K,     
              ONE_TO_2K,     
              ONE_TO_1K,     
              NULL_SCALE, 
   }; // The array is initialized on first use

   return grid_scale_table[i];
}


// Grid Properties Class Implementation
GridSpacing::GridSpacing()
{
}

GridSpacing::~GridSpacing()
{ 
}

int GridSpacing::get_grid_spacing( grid_line_type_t grid_line_type, MapScale scale, 
         degrees_t *major_grid_spacing, degrees_t *minor_grid_spacing )
{
   grid_spacing_t grid_spacing;
   query_grid_spacing_table( grid_line_type, scale, &grid_spacing );
   *major_grid_spacing = grid_spacing.major_grid_line_spacing;
   *minor_grid_spacing = grid_spacing.minor_grid_line_spacing;

   return SUCCESS;
}

   
int GridSpacing::get_major_gridline_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *major_grid_spacing )
{
   grid_spacing_t grid_spacing;
   query_grid_spacing_table( grid_line_type, scale, &grid_spacing );
   *major_grid_spacing = grid_spacing.major_grid_line_spacing;

   return SUCCESS;
}
      
int GridSpacing::get_minor_gridline_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *minor_grid_spacing )
{
   grid_spacing_t grid_spacing;
   query_grid_spacing_table( grid_line_type, scale, &grid_spacing );
   *minor_grid_spacing = grid_spacing.minor_grid_line_spacing;

   return SUCCESS;
}   
  
 
int GridSpacing::get_tick_spacing( grid_line_type_t grid_line_type, MapScale scale, 
         degrees_t *major_tick_spacing, degrees_t *minor_tick_spacing )
{
   grid_spacing_t grid_spacing;
   query_grid_spacing_table( grid_line_type, scale, &grid_spacing );
   *major_tick_spacing = grid_spacing.major_tick_spacing;
   *minor_tick_spacing = grid_spacing.minor_tick_spacing;

   return SUCCESS;
}

int GridSpacing::get_major_tick_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *major_tick_spacing )
{
   grid_spacing_t grid_spacing;
   query_grid_spacing_table( grid_line_type, scale, &grid_spacing );
   *major_tick_spacing = grid_spacing.major_tick_spacing;

   return SUCCESS;
}

int GridSpacing::get_minor_tick_spacing( grid_line_type_t grid_line_type, MapScale scale, 
      degrees_t *minor_tick_spacing )
{
   grid_spacing_t grid_spacing;
   query_grid_spacing_table( grid_line_type, scale, &grid_spacing );
   *minor_tick_spacing = grid_spacing.minor_tick_spacing;

   return SUCCESS;
}

int GridSpacing::query_grid_spacing_table( grid_line_type_t grid_line_type, MapScale scale, 
                                          grid_spacing_t *grid_params )
{
   int index;
   if ( grid_line_type == LAT_POINT )
      index = 0;
   else if ( grid_line_type == LON_POINT )
      index = 1;
   else
   {
      ERR_report("Grid spacing query_grid_spacing_table(): invalid line type.");
      return FAILURE;
   }

   if ( scale < WORLD || scale > NULL_SCALE )
   {
      ERR_report("Grid spacing lookup table line type index out of bounds.");
      return FAILURE;
   }

   MapScale closest_scale;
   get_closest_scale( scale, &closest_scale);

   for ( int k = 0; k < max_scales; k++ )
   {
      if ( closest_scale == grid_scale_table(k) )
      {
         // OK, safe to lookup spacing values in the table
         grid_params->major_grid_line_spacing = grid_spacing_table[k][index].major_grid_line_spacing;
         grid_params->minor_grid_line_spacing = grid_spacing_table[k][index].minor_grid_line_spacing;
         grid_params->major_tick_spacing = grid_spacing_table[k][index].major_tick_spacing;
         grid_params->minor_tick_spacing = grid_spacing_table[k][index].minor_tick_spacing;
         
         return SUCCESS;
      }
   }

   ERR_report("Grid spacing lookup table scale index out of bounds.");
   return FAILURE;
}

// search for the closest scale on the grid scale table
int GridSpacing::get_closest_scale( const MapScale &cur_scale,
                                   MapScale *closest_scale)
{
   // Sanity check
   if ( cur_scale < WORLD || cur_scale > NULL_SCALE )
   {
      ERR_report( "Invalid scale in GridSpacing::get_closest_scale()." );
      return FAILURE;
   }
   
   MapScale scale = NULL_SCALE;
   int scale_index = 1;

   // Scale in thru the grid scale table until 
   while( TRUE )
   {
      scale = grid_scale_table(scale_index);

      // check for out-of-bounds scale
      if ( scale > NULL_SCALE )
      {
         ERR_report( "Invalid scale in GridSpacing::get_closest_scale()." );
         return FAILURE;
      }
      
      if ( cur_scale <= scale )
         break;

      scale_index++;
   }

   // if an exact match don't go further
   if ( cur_scale == grid_scale_table(scale_index) )
   {
      *closest_scale = cur_scale;
      return SUCCESS;
   }

   // scale is between the current index and the previous one
   MapScale next_out = grid_scale_table(scale_index - 1);
   MapScale next_in = grid_scale_table(scale_index);

   // the percent difference between cur_scale and the next largest scale with 
   // data: % = (cur_scale - in_scale) / cur_scale
   double in = cur_scale.get_nominal_degrees_lat_per_pixel() -
      next_in.get_nominal_degrees_lat_per_pixel();

   // the percent difference between the next smallest scale with data and 
   // cur_scale: % = (out_scale - cur_scale) / cur_scale
   double out = next_out.get_nominal_degrees_lat_per_pixel() -
      cur_scale.get_nominal_degrees_lat_per_pixel();

   // if scaling out is closer to the current scale, do that, otherwise scale-in
   if (out < in)
      *closest_scale = next_out;
   else
      *closest_scale = next_in;   


   return SUCCESS;
}   

/*
// Test some of the scales
static int test( GridSpacing *grd_spacing )
{
   map_scale_t scale, closest_scale;

   scale = ONE_TO_80M;
   grd_spacing->get_closest_scale( scale, &closest_scale );
   TRACE( "Scale <%s>, Closest <%s>\n", 
      MAP_get_scale_string(scale), MAP_get_scale_string(closest_scale) );

   scale = MapScale(60000000);
   grd_spacing->get_closest_scale( scale, &closest_scale );
   TRACE( "Scale <%s>, Closest <%s>\n", 
      MAP_get_scale_string(scale), MAP_get_scale_string(closest_scale) );

   scale = ONE_TO_800K;
   grd_spacing->get_closest_scale( scale, &closest_scale );
   TRACE( "Scale <%s>, Closest <%s>\n", 
      MAP_get_scale_string(scale), MAP_get_scale_string(closest_scale) );
   
   scale = ONE_TO_400K;
   grd_spacing->get_closest_scale( scale, &closest_scale );
   TRACE( "Scale <%s>, Closest <%s>\n", 
      MAP_get_scale_string(scale), MAP_get_scale_string(closest_scale) );
   
   scale = ONE_TO_40K;
   grd_spacing->get_closest_scale( scale, &closest_scale );
   TRACE( "Scale <%s>, Closest <%s>\n", 
      MAP_get_scale_string(scale), MAP_get_scale_string(closest_scale) );
   
   scale = ONE_TO_24K;
   grd_spacing->get_closest_scale( scale, &closest_scale );
   TRACE( "Scale <%s>, Closest <%s>\n", 
      MAP_get_scale_string(scale), MAP_get_scale_string(closest_scale) );
   
   scale = SIXTEEN_KILOMETER;
   grd_spacing->get_closest_scale( scale, &closest_scale );
   TRACE( "Scale <%s>, Closest <%s>\n", 
      MAP_get_scale_string(scale), MAP_get_scale_string(closest_scale) );

   scale = WORLD;
   grd_spacing->get_closest_scale( scale, &closest_scale );
   TRACE( "Scale <%s>, Closest <%s>\n", 
      MAP_get_scale_string(scale), MAP_get_scale_string(closest_scale) );

   return SUCCESS;
}

*/


