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
#include "mapx.h"
#include "..\MapEngineCOM.h"

////////////////////////////////////////////////////////////////////////////////////////////
// static function prototypes
////////////////////////////////////////////////////////////////////////////////////////////

static int get_vsurface_size_for_a_to_scale_hardcopy(
   double vsurface_width_in_inches, double vsurface_height_in_inches,
   double print_scale_denom, degrees_t center_lat, degrees_t center_lon,
   degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix,
   double* vsurface_width_in_logical_units,  
   double* vsurface_height_in_logical_units,
   double* ns_geo_inches_per_logical_unit,  
   double* ew_geo_inches_per_logical_unit);
static int get_num_geo_inches_per_logical_unit( 
  degrees_t center_lat, degrees_t center_lon,
  degrees_t deg_lat_per_pix, degrees_t deg_lon_per_pix,
  double* ns_geo_inches_per_logical_unit,
  double* ew_geo_inches_per_logical_unit);

////////////////////////////////////////////////////////////////////////////////////////////

//
// The basic strategy for printing a non-rotated to-scale equal arc map is the following:
//
// 1) Calculate the exact geographic area needed to fill a page with a north-up 
//    to-scale map
// 2) Using degrees per pixel for given map type and geo distance calculations,
//    determine the size of the pixmap P that covers the geographic area in step #1.
//    Every pixel in the pixmap is a "logical unit".
// 3) Set the mapping mode:
//      SetMapMode(MM_ANISOTROPIC)
//      SetWindowExt(dimensions of P in pixels)
//      SetViewportExt(page dimensions in pixels)
// 4) blt the pixmap P to the page
//
//
// Rotation complicates things, but the basic idea [calculate the size of the
// geo area that exactly covers the page and then calc the exact size of the
// pixmap to blt to the surface so that the printout will be to scale] is the same.
//
// Here are the steps for printing a rotated to-scale equal arc map:
//
// 1) You are given the size (in inches) of the surface S on which the
//    to-scale map will be printed.
// 2) Calculate the dimensions (in inches) of the minimal virtual surface Sv that
//    would completely contain the surface S if S was rotated by the given angle.
// 3) Calculate the dimensions of the pixmap Pnv such that Pnv fills the geographic
//    area for a north-up map being printed on the virtual surface Sv.
// 4) Draw the north-up map into Pnv.
// 5) Now that we have the north-up map for the virtual surface, we need to 
//    calculate the dimensions of the pixmap P into which to rotate Pnv such
//    that when P is blted to fill the surface, a to-scale rotated map will result:
// 
//    a) Calculate the dimensions of the pixmap Prv such that Prv would
//       completely contain the pixmap Pnv if it was rotated by the given angle.
//
//    b) At this point, we could theoretically rotate Pnv into Prv, and then take the 
//       rotated pixmap Prv, set the mapping mode appropriately, and blt Prv (at 
//       an appropriate off-page offset) and get a to-scale rotated printout. However, 
//       Prv would be very large [it includes a lot of area off the page], and the 
//       mapping mode and offset calculations would be more complex.  
//       So instead of doing this, let's calculate the dimensions of the smallest 
//       possible pixmap P such that when P is blted to fill the surface S, the printout 
//       will be a to-scale map and use P instead:
//
//         i) Calculate the geographic dimensions in inches of the north-up
//            map Mnv that covers the virtual surface Sv
//        ii) Calculate the dimensions of the map Mrv whose geographics dimensions
//            would completely contain the map Mnv if it were rotated by the given angle.
//       iii) Calculate the dimensions (in inches) of the surface Srv which would
//            completely contain a to-scale map of the geographic area Mrvs.
//        iv) Calculate (in each dimension) the percentage of Srv covered by S.
//
//    c) Since S is to Srv as Pnv is to Prv [that is, (Pnv/Prv) = (S/Srv)],
//       we will use this information to calculate the exact dimensions of the
//       pixmap P into which to rotate Pnv such that when P is blted to the 
//       surface S, a to-scale map will result.  Using the ratio of S to Srv, 
//       calculate the dimensions of P.
// 6) rotate Pnv into P
// 7) Set the mapping mode:
//      SetMapMode(MM_ANISOTROPIC)
//      SetWindowExt(dimensions of P)
//      SetViewportExt(page dimensions)
// 8) blt the pixmap P to the surface
//

//
// Returns the surface dimensions in logical units such that a to-scale
// printout on the surface area will be filled with data.
//
// surface_scale_denom is the denominator of the scale at which the map will 
// be printed.  it is not necessarily the same as the scale denominator of the map 
// data being printed.
//
int MAP_get_surface_size_in_logical_units_for_a_to_scale_surface(
   double surface_width_in_inches, double surface_height_in_inches,
   degrees_t center_lat, degrees_t center_lon, double rotation_angle, 
   degrees_t unzoomed_degrees_per_pixel_lat,
   degrees_t unzoomed_degrees_per_pixel_lon,
   double surface_scale_denom,
   int* surface_width_in_logical_units,
   int* surface_height_in_logical_units)
{
   const double rot_angle = DEG_TO_RAD(rotation_angle);

   //
   // Get the number of logical units in the pixmap that completely covers
   // virtual surface.
   //
   // Also get the number of geographic inches represented by one pixel of 
   // this pixmap.
   //
   double vsurface_width_in_logical_units;
   double vsurface_height_in_logical_units;
   double ns_geo_inches_per_logical_unit;
   double ew_geo_inches_per_logical_unit;
   {
      //
      // Calculate the virtual surface dimensions. 
      //
      // That is, using the surface dimensions (in inches), calculate the dimensions
      // of the minimal "virtual surface" that would completely contain the surface 
      // if it was rotated by the given angle.
      //
      const double virtual_surface_width_in_inches =
         surface_width_in_inches*fabs(cos(rot_angle)) + 
            surface_height_in_inches*fabs(sin(rot_angle));
      const double virtual_surface_height_in_inches =
         surface_height_in_inches*fabs(cos(rot_angle)) + 
            surface_width_in_inches*fabs(sin(rot_angle));

      //
      // Calculate the dimensions of the pixmap needed to fill the geo area for
      // a north-up map being printed on the virtual surface.  
      //
      // The geo area
      // is calculated by determining the number of geographic inches covered by
      // virtual surface (by using the scale).
      //
      // Also get the number of geographic inches represented by one pixel of 
      // this pixmap.
      //
      if (get_vsurface_size_for_a_to_scale_hardcopy(
         virtual_surface_width_in_inches, virtual_surface_height_in_inches,
         surface_scale_denom, center_lat, center_lon, 
         unzoomed_degrees_per_pixel_lat, unzoomed_degrees_per_pixel_lon,
         &vsurface_width_in_logical_units, &vsurface_height_in_logical_units,
         &ns_geo_inches_per_logical_unit, &ew_geo_inches_per_logical_unit) != SUCCESS)
      {
         ERR_report("get_vsurface_size_for_a_to_scale_hardcopy failed");
         return FAILURE;
      }
   }

   //
   // For an unrotated map, the virtual surface is identical to the surface
   //
   if (rot_angle == 0.0)
   {
      *surface_width_in_logical_units = 
         static_cast<int>(ceil(vsurface_width_in_logical_units));
      *surface_height_in_logical_units = 
         static_cast<int>(ceil(vsurface_height_in_logical_units));

      return SUCCESS;
   }

   //
   // We know that the map is rotated at this point.
   //
   // Now we must calculate:
   //
   // 1) the surface size for the rotated map such that its virtual surface size 
   // is equivalent to the calculated virtual surface size
   //
   // 2) the size of the pixmap into which to rotate the virtual map such that
   // when the pixmap is bltted to the printer dc with the appropriate mapping mode,
   // a to-scale printout results
   //

   int optimized_rotated_pixmap_width; 
   int optimized_rotated_pixmap_height;
   {
      //
      // calculate (in each dimension) the proportion of the rotated map that 
      // the surface covers
      //
      double width_prop;
      double height_prop;
      {
         //
         // calculate the geographic width and height (in inches) of the 
         // area covered by a northup map completely filling the virtual surface 
         //
         const double virtual_map_width_in_geo_inches =
            ew_geo_inches_per_logical_unit*vsurface_width_in_logical_units;
         const double virtual_map_height_in_geo_inches =
            ns_geo_inches_per_logical_unit*vsurface_height_in_logical_units;

         //
         // now calculate the geographic width and height (in inches) of a map rotated to the
         // given angle that completely fills the virtual surface.  Call this map
         // the "rotated map".
         //
         const double rotated_map_width_in_geo_inches =
            virtual_map_width_in_geo_inches*fabs(cos(rot_angle)) +
               virtual_map_height_in_geo_inches*fabs(sin(rot_angle));
         const double rotated_map_height_in_geo_inches =
            virtual_map_height_in_geo_inches*fabs(cos(rot_angle)) +
               virtual_map_width_in_geo_inches*fabs(sin(rot_angle));

         //
         // from the geographic dimensions (in inches) of the rotated map,
         // calculate the dimensions (in inches) of the "pseudo-surface"
         // that covers this geographic area at the given scale
         //
         const double rotated_map_width_in_paper_inches =
            rotated_map_width_in_geo_inches/surface_scale_denom;
         const double rotated_map_height_in_paper_inches =
            rotated_map_height_in_geo_inches/surface_scale_denom;

         //
         // calculate the proportion of the rotated map that the surface covers
         //
         width_prop = 
            surface_width_in_inches/rotated_map_width_in_paper_inches;
         height_prop = 
            surface_height_in_inches/rotated_map_height_in_paper_inches;
      }

      //
      // At this point, we could take the virtual map pixmap and rotate it into
      // the rotated pixmap, then (after setting the mapping mode correctly) take 
      // the rotated pixmap and blt it to get a printout.  But this rotated pixmap will be quite 
      // large (it includes alot of area off the page), so instead determine the 
      // dimensions of the smallest possible rotated pixmap such that the page will be filled.
      //
      {
         //
         // Calculate the dimensions of a pixmap such that the virtual map
         // pixmap can be completely rotated into it.
         //
         const double rotated_pixmap_width = 
            vsurface_width_in_logical_units*fabs(cos(rot_angle)) +
            vsurface_height_in_logical_units*fabs(sin(rot_angle));
         const double rotated_pixmap_height = 
            vsurface_height_in_logical_units*fabs(cos(rot_angle)) +
            vsurface_width_in_logical_units*fabs(sin(rot_angle));

         //
         // Truncate the rotated pixmap size proportionally to the 
         // ratio between the paper size and the number of inches corresponding to
         // the rotated pixmap.  This is done so that it fits the page exactly.
         //
         optimized_rotated_pixmap_width = 
            static_cast<int>(width_prop*rotated_pixmap_width + 0.5);
         optimized_rotated_pixmap_height = 
            static_cast<int>(height_prop*rotated_pixmap_height + 0.5);
      }
   }
 
   //
   // For a rotated map, be sure to set the dimensions of the logical units
   // for the page to the dimensions of the optimized rotated pixmap.  
   // Otherwise the dimensions will be that of the virtual screen [and 
   // you will hence 1) wind up with a "virtual surface for your virtual 
   // surface" and will use a lot more memory for the virtual map than 
   // you need to and 2) cause some artifacts where the map does not appear
   // on the edges such as in some corners on the printout].
   //
   *surface_width_in_logical_units = optimized_rotated_pixmap_width;
   *surface_height_in_logical_units = optimized_rotated_pixmap_height;

   return SUCCESS;
}

int MAP_scale_t_to_scale_denominator(MapScale s, double* d)
{
   *d = (double)s.get_to_scale_denominator();

   return SUCCESS;
}

// ------------------------------------------------------------------------

//
// converts a scale denominator to a string of the form,
// for example, "1:3,333,333"
//
CString MAP_convert_scale_denominator_to_scale_string(int scale_denom)
{
   CString scale_denom_string;

   if (scale_denom >= 1000000000)
   {
      scale_denom_string.Format("1:%d", scale_denom);
   }
   else if (scale_denom >= 1000000)
   {
      int m = scale_denom / 1000000;
      int t = (scale_denom - m * 1000000) / 1000;
      scale_denom_string.Format("1:%d,%03d,%03d", m, t, 
         scale_denom - (m * 1000000) - (t * 1000));
   }
   else if (scale_denom >= 1000)
   {
      int t = scale_denom / 1000;
      scale_denom_string.Format("1:%d,%03d", t, scale_denom - (t * 1000));
   }
   else
   {
      scale_denom_string.Format("1:%d", scale_denom);
   }

   return scale_denom_string;
}


int MAP_test_limits(const MapScale &scale, int surface_width, int surface_height,
   bool &valid_surface, bool &can_rotate, bool &can_zoom, int &minimum_zoom)
{
   // you are already looking at the world
   if (scale == WORLD)
   {
      valid_surface = true;
      can_rotate = false;
      can_zoom = false;
      minimum_zoom = 100;
      return SUCCESS;
   }

   //
   // use a rotation angle of 45 degrees, since that angle will result in the
   // largest virtual surface size (i.e. area)
   //
   const double rotation_angle = 45.0;

   int vwidth;
   int vheight;
   if (MAP_calc_screen_size_for_a_rotated_map(surface_width, surface_height, 
      &vwidth, &vheight, rotation_angle) != SUCCESS)
   {
      ERR_report("MAP_calc_screen_size_for_a_rotated_map failed");
      return FAILURE;
   }
   
   // get degrees per pixel at the equator in order to figure out how many
   // pixels there are in the world
   degrees_t degrees_lat_per_pixel, degrees_lon_per_pixel;
   if (MAP_get_degrees_per_pixel_monitor(0.0, scale, 
      degrees_lat_per_pixel, degrees_lon_per_pixel) != SUCCESS)
   {
      ERR_report("MAP_get_degrees_per_pixel_monitor() failed.");
      return FAILURE;
   }

   // how many pixels from pole to pole
   int pixels_pole_to_pole = (int)(180.0 / degrees_lat_per_pixel + 0.5);

   // how many pixels around the Equator
   int pixels_around_world = (int)(360.0 / degrees_lon_per_pixel + 0.5);

   // if either dimension of the surface is greater than the number of pixels
   // in the world, then the surface size is invalid for this map scale
   if (surface_width > pixels_around_world || surface_height > pixels_pole_to_pole)
   {
      valid_surface = can_rotate = can_zoom = false;
      minimum_zoom = 100;
      return SUCCESS;
   }

   // surface size is OK, must test for rotation and minimum zoom percent
   valid_surface = true;
   can_zoom = true;

   // The maximum possible value for virtual surface width equals the maximum
   // possible value for virtual surface height, which is the diagonal of the
   // surface.  If all possible angles are not valid for the given surface,
   // then rotation will not be allowed.
   vwidth = (int)ceil(sqrt((double)surface_width * (double)surface_width +
      (double)surface_height * (double)surface_height));
   vheight = vwidth;

   // If either dimension on the virtual surface is greater than the number
   // of pixels in the world, then rotation you cannot rotate the map.  The
   // minimum zoom percent will be the larger of the limits implied by the
   // virtual surface dimensions.  The surface dimensions will be used if the
   // map can not be rotated.
   double min_zoom_x, min_zoom_y;
   can_rotate = (vwidth <= pixels_around_world && vheight <= pixels_pole_to_pole);
   if (can_rotate)
   {
      min_zoom_x = (double)vwidth * 100.0 / (double)pixels_around_world;
      min_zoom_y = (double)vheight * 100.0 / (double)pixels_pole_to_pole;
   }
   else
   {
      min_zoom_x = (double)surface_width * 100.0 / (double)pixels_around_world;
      min_zoom_y = (double)surface_height * 100.0 / (double)pixels_pole_to_pole;
   }

   minimum_zoom = (int)(ceil(__max(min_zoom_x, min_zoom_y)));

   return SUCCESS;
}

int MAP_test_limits_MDM(const MapSource &source, const MapScale &scale, 
   const MapSeries &series, degrees_t center_lat, degrees_t center_lon,
   double width_inches, double height_inches,
   bool &valid_surface, bool &can_rotate, bool &can_zoom, int &minimum_zoom)
{
   if (scale == WORLD)
   {
      ERR_report("WORLD maps cannot be to scale.");
      return FAILURE;
   }

   // create a rendering engine used to compute the size surface dimensions 
   // in logical units and the scale denominator
   MapEngineCOM map_engine;
   map_engine.init(FALSE, FALSE);
   map_engine.set_view_dimensions(width_inches, height_inches, SURFACE_UNITS_INCHES);
   map_engine.SetIsPrinting(TRUE, TRUE); 

   const int ret = map_engine.change_map_type(source, scale, series, center_lat, center_lon,
      0.0, 100, EQUALARC_PROJECTION, FALSE, FALSE);

   if (ret == SURFACE_SIZE_TOO_LARGE)
   {
      // surface size is too large for this scale
      valid_surface = can_rotate = can_zoom = false;
      minimum_zoom = 100;
      
      return SUCCESS;
   }

   map_engine.ApplyMap();
   const int width_logical_units = map_engine.get_curr_map()->get_surface_width();
   const int height_logical_units = map_engine.get_curr_map()->get_surface_height();

   // need rotate and zoom limits for valid surfaces
   return MAP_test_limits(scale, width_logical_units, height_logical_units,
      valid_surface, can_rotate, can_zoom, minimum_zoom);
}

// Tests to see if a to-scale map is possible for the given source, scale,
// series, and center on a width_inches x height_inches surface.
//    -valid_surface is false if the surface size is too large for this scale
//    -can_rotate is false if the rotation must be 0
//    -can_zoom is false if the zoom percent must be 100
//    -minimum_zoom will be between 1 and 100
// This function will fail if scale == WORLD.
int MAP_test_limits(const MapSource &source, const MapScale &scale, 
   const MapSeries &series, degrees_t center_lat, degrees_t center_lon,
   double width_inches, double height_inches,
   bool &valid_surface, bool &can_rotate, bool &can_zoom, int &minimum_zoom)
{
   return MAP_test_limits_MDM(source, scale, series, center_lat, center_lon,
      width_inches, height_inches, valid_surface, can_rotate, can_zoom, minimum_zoom);
}

// ------------------------------------------------------------------------

//
// Returns the dimensions of the pixmap needed to fill the virtual page
// when printed north up and stretched/squashed to correctly print to scale.
// Also returns the number of geo inches for a logical unit (i.e. a pixel).
//
// The pixmap dimensions are calculated by:
//
// 1) calc the number of geo inches in one pixel (i.e. logical unit) of map data
// 2) calc the number of paper inches for a logical unit by dividing the
//    number of geo inches per logical unit by the scale
// 3) calc the number of logical units on the page by dividing the page size in inches
//    by the number of paper inches per logical unit
//
static
int get_vsurface_size_for_a_to_scale_hardcopy(
   double vsurface_width_in_inches, double vsurface_height_in_inches,
   double print_scale_denom, degrees_t center_lat, degrees_t center_lon,
   degrees_t unzoomed_deg_lat_per_pix, degrees_t unzoomed_deg_lon_per_pix,
   double* vsurface_width_in_logical_units,  
   double* vsurface_height_in_logical_units,
   double* ns_geo_inches_per_logical_unit,  
   double* ew_geo_inches_per_logical_unit)
{
   //
   //  get the number of geo inches in a logical unit in both the north-south
   //  and east-west directions
   //
   get_num_geo_inches_per_logical_unit(center_lat,
      center_lon, unzoomed_deg_lat_per_pix, 
      unzoomed_deg_lon_per_pix, 
      ns_geo_inches_per_logical_unit, 
      ew_geo_inches_per_logical_unit);

   //
   // Get the number of paper inches for a logical unit by dividing
   // the number of geo inches per logical unit by the scale
   //
   const double ns_paper_inches_per_logical_unit = 
      *ns_geo_inches_per_logical_unit / print_scale_denom;
   const double ew_paper_inches_per_logical_unit = 
      *ew_geo_inches_per_logical_unit / print_scale_denom; 

   //
   // calculate  the number of logical units on the page by dividing the 
   // page size in inches by the number of paper inches per logical unit
   //
   *vsurface_width_in_logical_units = 
      vsurface_width_in_inches/ew_paper_inches_per_logical_unit;
   *vsurface_height_in_logical_units = 
      vsurface_height_in_inches/ns_paper_inches_per_logical_unit;

   return SUCCESS;
}

// -----------------------------------------------------------------------

static
int get_num_geo_inches_per_logical_unit( 
  degrees_t center_lat, degrees_t center_lon,
  degrees_t deg_lat_per_pix, degrees_t deg_lon_per_pix,
  double* ns_geo_inches_per_logical_unit,
  double* ew_geo_inches_per_logical_unit)
{
   double ew_distance_in_kilometers_per_logical_unit;
   double ns_distance_in_kilometers_per_logical_unit;
   double dummy;

   GEO_distance(center_lat - (1000.0 * deg_lat_per_pix), center_lon, 
      center_lat, center_lon, 
      &ns_distance_in_kilometers_per_logical_unit, &dummy);
   *ns_geo_inches_per_logical_unit = 
     METERS_TO_FEET(ns_distance_in_kilometers_per_logical_unit)*12.0;

   //
   // make sure to account for wrapping around the IDL here
   //
   degrees_t lon = center_lon - (1000.0 * deg_lon_per_pix);
   if (lon < -180.0)
      lon += WORLD_DEG;
   GEO_distance(center_lat, lon,
      center_lat, center_lon, 
      &ew_distance_in_kilometers_per_logical_unit, &dummy);
   *ew_geo_inches_per_logical_unit = 
     METERS_TO_FEET(ew_distance_in_kilometers_per_logical_unit)*12.0;

   return SUCCESS;
}

