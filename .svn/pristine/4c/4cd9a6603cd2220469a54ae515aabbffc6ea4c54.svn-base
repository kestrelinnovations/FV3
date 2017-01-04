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



// surf_lim.cpp

// This file contains functions dealing with surface size related limitations
// on scale, rotation, and scale-percent for to-scale maps.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/surf_lim.h"
// system includes
// third party files
// other FalconView headers
// this project's headers
#include "FalconView/PrintToolOverlay/prntpage.h"

// This function will test implied to-scale map to see if it violates any
// limits on surface size, rotation, or scale_percent for the given map
// type.  It returns true if the map is invalid and false if it is O.K.
// show_message == true causes this function to display a message explaining
// what is wrong in the event of a violation.
bool prn_settings_violate_limits(const MapSource &source,
   const MapScale &scale, const MapSeries &series,
   degrees_t center_lat, degrees_t center_lon, double rotation,
   double width_inches, double height_inches, int scale_percent,
   bool show_message)
{
   bool valid_surface;
   bool can_rotate;
   bool can_zoom;
   int minimum_zoom;

   // test surface size, rotation, and zoom for this scale
   if (MAP_test_limits(source, scale, series, center_lat, center_lon,
      width_inches, height_inches, valid_surface, can_rotate, can_zoom,
      minimum_zoom) != SUCCESS)
   {
      ERR_report("MAP_test_limits() failed.");
      return true;
   }

   // the surface size must be valid for the requested scale on the current
   // printer page surface
   if (!valid_surface)
   {
      if (show_message)
      {
         CString msg;
         msg.Format("%s cannot be printed to-scale on a %1.2f\" x %1.2f\" "
            "(printable area).\nUse a smaller paper size or a map type with a "
            "larger scale.",
            CPrinterPage::get_map_type_string(source, scale, series),
            width_inches, height_inches);
         AfxMessageBox(msg);
      }
      return true;
   }

   // rotation must be allowed if the map is rotated
   if (can_rotate == false && rotation != 0.0)
   {
      if (show_message)
      {
         CString msg;
         msg.Format("A rotated %s cannot be printed to-scale on a "
            "%1.2f\" x %1.2f\" (printable area).\nUse a smaller paper size, a "
            "map type with a larger scale, or set the Heading to 360.0\260.",
            CPrinterPage::get_map_type_string(source, scale, series),
            width_inches, height_inches);
         AfxMessageBox(msg);
      }
      return true;
   }

   // the scale percent cannot be less than the minimum zoom percent for the
   // requested scale on the surface of the current printer page
   if (scale_percent < minimum_zoom)
   {
      if (show_message)
      {
         CString msg;
         msg.Format(
            "%d%% is the minimum Scale Factor for %s printed to-scale on a "
            "%1.2f\" x %1.2f\" (printable area).\nUse a Scale Factor of %d%%, "
            "or higher.", minimum_zoom,
            CPrinterPage::get_map_type_string(source, scale, series),
            width_inches, height_inches, minimum_zoom);
         AfxMessageBox(msg);
      }
      return true;
   }

   return false;
}

// Figure out the smallest scale that is going to be valid for a map
// on a page_width x page_height surface.  Find the largest scale that
// has a minimum scale factor that is greater than the
// minimum_scale_factor_desired.  The scale is returned in
// scale_with_scale_factor_limit and the scale factor is returned in
// scale_factor_limit.
int prn_get_limits_for_surface(MapCategory category, d_geo_t center,
   double page_width, double page_height, int minimum_scale_factor_desired,
   MapScale &smallest_valid_scale, MapScale &scale_with_scale_factor_limit,
   int &scale_factor_limit)
{
   int minimum;
   bool valid_surface;
   bool can_rotate;
   bool can_zoom;

   // get list of maps in the given category
   CList<MapType *, MapType *> map_list;
   if (MAP_get_map_list(category, map_list) != SUCCESS)
   {
      ERR_report("MAP_get_map_list()");
      return FAILURE;
   }

   if (minimum_scale_factor_desired < 5)
   {
      ERR_report("minimum_scale_factor_desired is too small.");
      return FAILURE;
   }

   // Initialize scale resize data for the default limits: any scale
   // from 1:20M to 1:1K with a scale factor from 5 to 800.
   smallest_valid_scale = ONE_TO_20M;
   scale_with_scale_factor_limit = ONE_TO_20M;
   scale_factor_limit = minimum_scale_factor_desired;

   MapType *map_type;
   bool no_valid_surface = true;
   POSITION position = map_list.GetHeadPosition();
   while (position)
   {
      map_type = map_list.GetNext(position);
      if (map_type->get_scale() >= ONE_TO_20M)
      {
         if (MAP_test_limits(map_type->get_source(), map_type->get_scale(),
            map_type->get_series(), center.lat, center.lon,
            page_width, page_height, valid_surface, can_rotate, can_zoom,
            minimum) == SUCCESS && valid_surface)
         {
            if (no_valid_surface)
            {
               smallest_valid_scale = map_type->get_scale();
               no_valid_surface = false;
            }

            if (minimum > minimum_scale_factor_desired)
            {
               scale_with_scale_factor_limit = map_type->get_scale();
               scale_factor_limit = minimum;
            }
            else
               position = NULL;  // break: limits are set
         }
      }
   }

   return SUCCESS;
}
