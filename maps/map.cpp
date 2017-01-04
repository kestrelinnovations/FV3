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
#include "fvwutil.h"
#include "mapx.h"
#include "..\mapview.h"
#include "Common\ComErrorObject.h"
#include "maps.h"
#include "..\MapEngineCOM.h"

/////////////////////////////////////////////////////////////////////////////
// static function prototypes
/////////////////////////////////////////////////////////////////////////////

static
int calculate_component_map_offset_in_mosaic_map(
   const MapProj* mosaic_map, const MapProj* component_map,
   double* component_map_row_in_mosaic_map, 
   double* component_map_column_in_mosaic_map);
static
int get_surface_size_in_logical_units_for_a_to_scale_surface(double surface_width_in_inches,
   double surface_height_in_inches, const MapSource &source, const MapScale &data_scale, 
   const MapSeries &series, degrees_t center_lat, degrees_t center_lon, double rotation_angle,
   double surface_scale_denom, int* surface_width_in_logical_units,
   int* surface_height_in_logical_units);
static
bool adjust_center_lat_to_make_surface_size_valid(double surface_width_in_inches,
   double surface_height_in_inches, MapSource source, MapScale data_scale, MapSeries series,
   degrees_t center_lat, degrees_t center_lon, double rotation_angle, 
   int surface_scale_percent, double surface_scale_denom, 
   int *surface_width_in_logical_units, int *surface_height_in_logical_units, 
   degrees_t *adjusted_center_lat);
static
bool test_surface_size(MapSource source, MapScale scale, MapSeries series, double rotation,
   int width_lu, int height_lu);
static
int calculate_surface_scale_denom(MapScale data_scale, int surface_scale_percent,
   double* surface_scale_denom);

/////////////////////////////////////////////////////////////////////////////
   
inline static 
int round(double val)
{
   if (val >= 0.0)
      return static_cast<int>(val+0.5);
   else
      return static_cast<int>(val-0.5);
}


//
// This is an algorithm to convert from a mosaic map surface coordinate to
// a component map surface coordinate.
//
// It does a calculation employing the location of the upper left pixel of
// the component map in mosaic map coordinates and the differences in size
// of logical units between the mosaic map and the component map.
// It does this calculation rather than doing a surface to geo on the mosaic map
// followed by a geo to surface on the component map because problems can arise
// there when a line is greater than 180 degrees longitude.
//
int MapProjWithWriteableChartInfoImpl::mosaic_surface_to_component_surface(
      int mosaic_x, int mosaic_y,int* component_x, int* component_y) const
{
   ASSERT(is_mosaic_map_component());

   //
   // calculate the difference between the reference mosaic map coordinate
   // (i.e. the floating point location of the center of the component map
   // upper left pixel center) and the requested mosaic map coordinate
   //
   const double mosaic_x_diff = 
      m_component_map_column_offset_in_mosaic_map_pixmap - mosaic_x;
   const double mosaic_y_diff = 
      m_component_map_row_offset_in_mosaic_map_pixmap - mosaic_y;

   //
   // multiply the differences in mosaic map logical units by the
   // scale ratios to get the differences in component map logical units
   //
   const double component_x_diff =
      mosaic_x_diff*m_mosaic_to_component_pixel_lon_scale_ratio;
   const double component_y_diff =
      mosaic_y_diff*m_mosaic_to_component_pixel_lat_scale_ratio;

   *component_x = round(-component_x_diff);
   *component_y = round(-component_y_diff);

   return SUCCESS;
}

int MapProjWithWriteableChartInfoImpl::mosaic_physical_to_component_physical(
   int mosaic_x, int mosaic_y,
   double component_map_print_width_in_inches, double component_map_print_height_in_inches,
   double component_map_print_width_in_pixels, double component_map_print_height_in_pixels,
   int* component_x, int* component_y) const
{
   ASSERT(is_mosaic_map_component());

   //
   // get the printer pixels per inch
   //
   const double printer_pixels_per_inch_width = 
      component_map_print_width_in_pixels/component_map_print_width_in_inches;
   const double printer_pixels_per_inch_height = 
      component_map_print_height_in_pixels/component_map_print_height_in_inches;

   //
   // get the component map offset in physical units
   //
   const double cm_horiz_offset_in_physical_units =
      component_map_horizontal_offset_in_inches()*printer_pixels_per_inch_width;
   const double cm_vert_offset_in_physical_units =
      component_map_vertical_offset_in_inches()*printer_pixels_per_inch_height;

   *component_x = 
      static_cast<int>(mosaic_x - cm_horiz_offset_in_physical_units + 0.5);
   *component_y = 
      static_cast<int>(mosaic_y - cm_vert_offset_in_physical_units + 0.5);

   return SUCCESS;
}

int MapProjWithWriteableChartInfoImpl::get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence)  
{
   return FAILURE;
}


/////////////////////////////////////////////////////////////////////////////

int SettableMapProjImpl::lcl_bind_equal_arc_map_to_surface(boolean_t is_to_scale,
   boolean_t use_printer_deg_per_pixel_values, 
   degrees_t adjusted_center_lat, bool &geo_width_violation)
{
   // initialize to false in case of a FAILURE before bind_equal_arc_map_to_surface
   geo_width_violation = false;

   ASSERT(is_spec_set());
   ASSERT(is_surface_set());
   if (!is_spec_set() || !is_surface_set())
   {
      ERR_report("invalid state");
      return FAILURE;
   }
   
   bool valid_surface;
   bool can_rotate;
   bool can_zoom;
   int minimum_zoom;

   // test surface size, rotation, and zoom for this scale
   if (MAP_test_limits(scale(), get_surface_width(), get_surface_height(),
      valid_surface, can_rotate, can_zoom, minimum_zoom) != SUCCESS)
   {
      ERR_report("MAP_test_limits() failed");
      return FAILURE;
   }

   // make sure surface size is valid
   if (!valid_surface)
   {
      ERR_report("Surface size is too large for this scale.");
      return FAILURE;
   }

   //
   // determine what the actual (as opposed to the requested) rotation angle will be.
   // Do the same for the zoom percentage.
   //
   degrees_t actual_rotation = requested_rotation();
   if (can_rotate == false)
      actual_rotation = 0.0;
   int actual_zoom_percent = requested_zoom_percent();

   // reset the zoom percent to 100 if TIROS map
   if (source() == TIROS)
   {
      actual_zoom_percent = 100;
   }
   // otherwise if the requested zoom percent is set to 'native' then get 
   // the nominal degrees per pixel and the degrees per pixel of the data to 
   // determine the zoom percent of the Native Resolution
   else if (requested_zoom_percent() == NATIVE_ZOOM_PERCENT)
   {
      CFvwUtil *futil = CFvwUtil::get_instance();
      degrees_t center_lat = requested_center_lat();

      // set actual zoom percent to 100 just in case get_degrees_per_pixel_data()
      // or MAP_get_degrees_per_pixel_monitor fails
      actual_zoom_percent = 100;

      // get the degrees per pixel values for the data
      degrees_t data_lat, data_lon;
      BOOL bLockDegreesPerPixel;
      MapType map_type(source(), scale(), series());
      if (map_type.get_degrees_per_pixel_data(center_lat, &data_lat, &data_lon, &bLockDegreesPerPixel) == SUCCESS)
      {
         if (!bLockDegreesPerPixel)
         {
            // get the nominal degrees per pixel
            degrees_t default_lat, default_lon;
            if (MAP_get_degrees_per_pixel_monitor(center_lat, scale(),
               default_lat, default_lon) == SUCCESS)
            {
               actual_zoom_percent = futil->round(default_lat / data_lat * 100);
            }
         }
      }
   }

   if (can_zoom == false || actual_zoom_percent < minimum_zoom)
      actual_zoom_percent = minimum_zoom;

   // to-scale unique adjustments
   degrees_t center_lat;

   if (is_to_scale)
   {
      // zoom percent must be 100 when drawing to scale
      actual_zoom_percent = 100;

      // The center may have to be adjusted to make the surface size valid
      // on to-scale maps on very large surfaces.  In such cases using a
      // latitude closer to the equator can fix the problem.  However, it
      // reduces the surface size in logical units.  Because the surface
      // size is now smaller bind_equal_arc_map_to_surface may not adjust
      // the center in the same way, if at all.  That would mean the surface
      // dimensions in logical units would not be correct for this to be a
      // to-scale map.  In other words the same center latitude used to 
      // compute the surface dimensions must be used when binding the map,
      // and that latitude is in adjusted_center_lat.
      center_lat = adjusted_center_lat;
   }
   else
      center_lat = requested_center_lat();

   //
   // use m_proj here rather than proj() to avoid an ASSERT (since m_proj_set will not be
   // set to true until after this call)
   //
   // make sure to use the actual rotation angle and zoom percent rather than
   // the requested values
   //

   // Destroy old projector
   if(map()->m_proj != NULL)
      PROJ_destroy_projector(map()->m_proj);

   // Create new projector
   if(PROJ_create_projector( requested_projection(), &(map()->m_proj)) != SUCCESS) {
      ERR_report("PROJ_create_projector");
      map()->m_proj = NULL;
      return FAILURE;
   }

   // Bind the map to the surface
   int ret = map()->m_proj->bind_map_to_surface(source(), scale(), series(), use_printer_deg_per_pixel_values,
      requested_center_lat(), requested_center_lon(), actual_rotation, 
      actual_zoom_percent, get_surface_width(), get_surface_height(), 
      geo_width_violation, spec()->m_use_mosaic_map_parameters,
      spec()->m_mosaic_std_parallel_1, spec()->m_mosaic_std_parallel_2,
      spec()->m_mosaic_offset_x, spec()->m_mosaic_offset_x,
      spec()->m_mosaic_meters_per_pixel_lat, spec()->m_mosaic_meters_per_pixel_lon,
      spec()->m_mosaic_center_lat, spec()->m_mosaic_center_lon, spec()->m_mosaic_surface_width, 
      spec()->m_mosaic_surface_height, spec()->m_row, spec()->m_col,
      spec()->m_mosaic_num_row, spec()->m_mosaic_num_col);

   if (ret != FAILURE)
   {
      map()->m_actual_rotation = actual_rotation;
      map()->m_actual_zoom_percent = actual_zoom_percent;

      map()->m_actual_projection = requested_projection();

      map()->m_proj_set = TRUE;
   }

   return ret;
}

// Calculates the surface dimensions in logical units and the scale
// denominator if this MapProj is used on a surface with the given
// dimensions in inches and scale percent.
//
// Returns SUCCESS/FAILURE.  Check surface_size_valid upon SUCCESS to see
// if the computed surface dimensions are actually valid. Dimensions are
// invalid when they require more logical units than there are pixels of
// data in the world.
int SettableMapProjImpl::calc_surface_logical_units_and_scale_denom(
   double surface_width_in_inches, double surface_height_in_inches,
   int surface_scale_percent, double &surface_scale_denom,
   int &surface_width_in_logical_units, int &surface_height_in_logical_units,
   bool &surface_size_valid, degrees_t &adjusted_center_lat)
{
   // initialize to false in case of failure
   surface_size_valid = false;
   
   if (calculate_surface_scale_denom(scale(), surface_scale_percent,
      &surface_scale_denom) != SUCCESS)
   {
      ERR_report("calculate_surface_scale_denom");
      return FAILURE;
   }
   
   //
   // get the surface size if nothing is adjusted (e.g. the center doesn't change)
   //
   if (get_surface_size_in_logical_units_for_a_to_scale_surface(
      surface_width_in_inches, surface_height_in_inches, source(), scale(),
      series(), requested_center_lat(), requested_center_lon(),
      requested_rotation(), surface_scale_denom, 
      &surface_width_in_logical_units, &surface_height_in_logical_units) !=
      SUCCESS)
   {
      ERR_report("get_surface_size_in_logical_units_for_a_to_scale_surface");
      return FAILURE;
   }
   
   // lcl_bind_equal_arc_map_to_surface will fail if the surface dimensions are
   // too large.  Since calc_surface_logical_units_and_scale_denom is used to
   // test potential map projections as well as bind them, we want to avoid
   // writting an error report here.
   if (!test_surface_size(source(), scale(), series(), requested_rotation(), 
      surface_width_in_logical_units, surface_height_in_logical_units))
   {
      // Note surface_size_valid is false.  If we return SUCCESS here, the
      // flag will indicate to the caller that a to-scale map is not possible
      // with the given parameters.
      
      // If it is not possible to do a to-scale map on a surface with the given
      // at the given scale and surface_scale_denom even at the equator, then
      // this function will return false.  Otherwise, the center will be moved
      // close enough to the equator so that the surface dimensions become 
      // valid.  The adjusted center is returned along with the surface 
      // dimensions in logical units.
      if (adjust_center_lat_to_make_surface_size_valid(
         surface_width_in_inches, surface_height_in_inches, source(), scale(),
         series(), requested_center_lat(), requested_center_lon(), requested_rotation(), 
         surface_scale_percent, surface_scale_denom, 
         &surface_width_in_logical_units, &surface_height_in_logical_units, 
         &adjusted_center_lat) == false)
         return SUCCESS;
      
      TRACE("Initial surface was too large.\n");
   }
   else
      adjusted_center_lat = requested_center_lat();
   
   // Since lcl_bind_equal_arc_map_to_surface can cause the center latitude
   // to shift towards the equator in order to keep the map bounds between
   // N 90 and S 90 and the surface dimensions in logical units are a
   // function of center latitude, we need to bind a temporary map here and
   // recompute the surface dimensions if needed.
   SettableMapProjImpl temp_map;
   
   // Since the adjusted_center_lat may already differ from the requested 
   // center latitude, e.i., this->spec().center.lat, we need to use the
   // center adjusted map spec of this SettableMapProjImpl.
   MapSpec map_spec;
   
   // Typically this loop will be executed exactly once.  For large paper 
   // sizes, on the order of 3' x 8', with an effective scale around 1:7 M
   // lcl_bind_equal_arc_map_to_surface may fail due to the fact that the
   // width of the virtual map is greater than 360 degrees.  Since we made
   // it past test_surface_size (either directly or via adjust_center_lat_...),
   // a center latidue must exist for which a map with the requested parameters
   // (center latitude excluded) can be bound to the given surface.  In the
   // event of one of these rare and exceptional failures the do loop will
   // move the adjusted_center_lat towards the equator in 0.5 degree increments
   // until lcl_bind_equal_arc_map_to_surface succeeds.
   bool geo_width_violation;
   do 
   {
      // in case the center latitude already differs from requested_center_lat()
      map_spec = *this->spec();
      map_spec.center.lat = adjusted_center_lat;
      
      // copy your map spec with adjusted center_lat to the temp map projection
      if (temp_map.set_spec(map_spec) != SUCCESS)
      {
         ERR_report("set_spec");
         return FAILURE;
      }
      
      // set the surface dimensions
      if (temp_map.set_surface_size(surface_width_in_logical_units, 
         surface_height_in_logical_units) != SUCCESS)
      {
         ERR_report("set_spec");
         return FAILURE;
      }
      
      // attempt to bind the map to the specified surface   
      if (temp_map.lcl_bind_equal_arc_map_to_surface(TRUE, TRUE, adjusted_center_lat,
         geo_width_violation) != SUCCESS)
      {
         // if the failure was NOT caused by a surface width violation
         if (geo_width_violation == false)
         {
            ERR_report("bind_equal_arc_map_to_surface");
            return FAILURE;
         }
         
         // move towards the equator in 0.5 degree increments
         if (adjusted_center_lat > 0.5)
         {
            adjusted_center_lat -= 0.5;
         }
         else if (adjusted_center_lat < -0.5)
         {
            adjusted_center_lat += 0.5;
         }
         else if (adjusted_center_lat != 0.0)
         {
            // within 0.5 degrees of Equator go to Equator
            adjusted_center_lat = 0.0;
         }
         else
         {
            // Already tried Equator, we should not have gotton this far.  If
            // this fails test_surface_size should have failed above.
            ERR_report("Surface size invalid.");
            return FAILURE;
         }
         
         // changing center latitude changes surface dimensions in logical units
         get_surface_size_in_logical_units_for_a_to_scale_surface(
            surface_width_in_inches, surface_height_in_inches, source(), scale(),
            series(), adjusted_center_lat, requested_center_lon(), requested_rotation(), 
            surface_scale_denom, &surface_width_in_logical_units, 
            &surface_height_in_logical_units);
      }
      
      // if geo_width_violation, try again with new center latitude and new 
      // surface dimensions - otherwise the temp map was successfully bound
   } while (geo_width_violation);
   
   
   
   // if lcl_bind_equal_arc_map_to_surface changed the center we have to
   // re-compute and re-test the surface size in logical units
   if (temp_map.actual_center_lat() != adjusted_center_lat)
   {
      // must calculate a new surface size based on the new center latitude
      
      
      const degrees_t new_center_lat = temp_map.actual_center_lat();
      const degrees_t new_center_lon = temp_map.actual_center_lon();
      if (get_surface_size_in_logical_units_for_a_to_scale_surface(
         surface_width_in_inches, surface_height_in_inches, source(), scale(),
         series(), new_center_lat, new_center_lon, requested_rotation(), 
         surface_scale_denom, &surface_width_in_logical_units, 
         &surface_height_in_logical_units) != SUCCESS)
      {
         ERR_report("get_surface_size_in_logical_units_for_a_to_scale_surface");
         return FAILURE;
      }
      
      // In case the change in center lead to an invalid surface condition, the
      // center will be adjusted to fix the problem, if possible.
      if (!test_surface_size(source(), scale(), series(), requested_rotation(), 
         surface_width_in_logical_units, surface_height_in_logical_units))
      {
         // If adjust_center_lat_to_make_surface_size_valid was already called
         // above, then the surface size should already be valid.  We should
         // only wind up here if the change in center in the lcl_bind_...
         // function caused us to go from valid to invalid surface dimensions.
         // The lcl_bind_... function moves the center based on the assumption
         // that the surface size in logical units is constant.  In reality the
         // surface size in logical units for a to-scale chart are a function
         // of map center latitude.
         ASSERT(adjusted_center_lat == requested_center_lat());
         
         if (adjust_center_lat_to_make_surface_size_valid(
            surface_width_in_inches, surface_height_in_inches, source(), scale(),
            series(), new_center_lat, new_center_lon, requested_rotation(), 
            surface_scale_percent, surface_scale_denom, 
            &surface_width_in_logical_units, &surface_height_in_logical_units, 
            &adjusted_center_lat) == false)
         {
            ASSERT(0);
            return SUCCESS;
         }
      }
      else
      {
         adjusted_center_lat = temp_map.actual_center_lat();
      }
      
      // is the surface valid for this map?
      surface_size_valid = true;
   }
   else
   {
      // is the surface valid for this map?
      surface_size_valid = true;
   }
   
   ASSERT(test_surface_size(source(), scale(), series(), requested_rotation(), 
      surface_width_in_logical_units, surface_height_in_logical_units));
   
   
   //ASSERT((adjusted_center_lat >= 0.0 && temp_map.actual_center_lat() <= adjusted_center_lat) ||
   //   (adjusted_center_lat < 0.0 && temp_map.actual_center_lat() >= adjusted_center_lat));
   
   return SUCCESS;
}

int SettableMapProjImpl::bind_equal_arc_map_to_surface()
{
   const degrees_t unused1 = requested_center_lat();
   bool unused2;
   const int ret = lcl_bind_equal_arc_map_to_surface(FALSE, FALSE, unused1, unused2);

   if (ret !=  FAILURE)
   {
      map()->m_is_to_scale_projection = FALSE;
      map()->m_to_scale_surface_width_in_inches = 0.0;
      map()->m_to_scale_surface_height_in_inches = 0.0;
      map()->m_surface_scale_percent = 0;
      map()->m_surface_scale_denom = 0.0;
   }

   return ret;
}

static
int get_surface_size_in_logical_units_for_a_to_scale_surface(double surface_width_in_inches,
   double surface_height_in_inches, const MapSource &source, const MapScale &data_scale, 
   const MapSeries &series, degrees_t center_lat, degrees_t center_lon, double rotation_angle,
   double surface_scale_denom, int* surface_width_in_logical_units,
   int* surface_height_in_logical_units)
{  
   //
   // get the unzoomed degrees per pixel for the requested center
   //
   // note that surface dims are only used by MAP_get_degrees_per_pixel_world,
   // so they are not used here - since this is for to-scale surfaces and you
   // can not print a world map to scale
   //
   MapType map_type(source, data_scale, series);

   degrees_t deg_lat_per_pix;
   degrees_t deg_lon_per_pix;
   if (map_type.get_degrees_per_pixel_printer(center_lat,
      &deg_lat_per_pix, &deg_lon_per_pix) != SUCCESS)
   {
      ERR_report("get_degrees_per_pixel_printer() failed.");
      return FAILURE;
   }

   if (MAP_get_surface_size_in_logical_units_for_a_to_scale_surface(
      surface_width_in_inches, surface_height_in_inches, center_lat, center_lon, 
      rotation_angle, deg_lat_per_pix, deg_lon_per_pix,
      surface_scale_denom, surface_width_in_logical_units, 
      surface_height_in_logical_units) != SUCCESS)
   {
      ERR_report("MAP_get_surface_size_in_logical_units_for_a_to_scale_hardcopy");
      return FAILURE;
   }

   return SUCCESS;
}

// If the surface size will not be valid even if you move the center latitude
// to the equator, this function returns false.  Otherwise the
// adjusted_center_lat will be set to the closest latitude to center_lat for
// which the surface size is valid, the surface dimensions in logical units
// will be computed, and this function returns true.
bool adjust_center_lat_to_make_surface_size_valid(double surface_width_in_inches,
   double surface_height_in_inches, MapSource source, MapScale data_scale, MapSeries series,
   degrees_t center_lat, degrees_t center_lon, double rotation_angle,
   int surface_scale_percent, double surface_scale_denom, 
   int *surface_width_in_logical_units, int *surface_height_in_logical_units, 
   degrees_t *adjusted_center_lat)
{
   int width, height;

   // if you move the center to the equator and the surface size is still
   // invalid, adjusting the center will not fix the problem
   get_surface_size_in_logical_units_for_a_to_scale_surface(
      surface_width_in_inches, surface_height_in_inches, source, data_scale,
      series, 0.0, center_lon, rotation_angle, surface_scale_denom, &width, &height);
   if (!test_surface_size(source, data_scale, series, rotation_angle, width, height))
      return false;

   // Find the latitude closest to requested_center_lat() for which the
   // surface size is valid.  The loop will do a binary search for at least
   // 20 iterations.  The resulting center_lat will be within 90/2^20
   // degrees of the actual center, and width and height will be valid.
   // There may be a closed form solution to this problem, but the
   // functions called by get_surface_size_in_logical_units_for_a_to_scale_surface
   // look pretty complicated.
   degrees_t new_center_lat = center_lat / 2.0;
   degrees_t last_bad_lat = center_lat;
   degrees_t last_good_lat = 0.0;
   int i=0;
   while (1)
   {
      get_surface_size_in_logical_units_for_a_to_scale_surface(
         surface_width_in_inches, surface_height_in_inches, 
         source, data_scale, series, new_center_lat, center_lon, 
         rotation_angle, surface_scale_denom, &width, &height);
      if (test_surface_size(source, data_scale, series, rotation_angle, 
         width, height))
      {
         // If this is at least the 10th iteration, where close enough.
         // Break while we have valid surface dimensions.
         if (i >= 19)
            break;

         // save the center latitude that passed the test
         last_good_lat = new_center_lat;

         // move half the distance between new_center_lat and last_bad_lat
         new_center_lat = (new_center_lat + last_bad_lat) / 2.0;
      }
      else
      {
         ASSERT((last_bad_lat >= new_center_lat && new_center_lat > 0) || 
            (last_bad_lat <= new_center_lat && new_center_lat < 0));

         // new_center_lat did not pass the test.  Since new_center_lat is always
         // closer to the equator than last_bad_lat is, new_center_lat is a
         // better bound on how far from the equator a valid center
         // latitude can be.
         last_bad_lat = new_center_lat;

         // if last_bad_lat gets within 1 millionth of a degrees of 
         // last_good_lat, then last_good_lat is close enough
         if (fabs(last_bad_lat - last_good_lat) < 0.000001)
         {
            new_center_lat = last_good_lat;
            i = 19;
         }
         // move half the distance betwee the last good latitude and the
         // one the last bad latitude back towards last_good_lat
         else
            new_center_lat = (last_good_lat + last_bad_lat) / 2.0;
      }

      i++;
   }

   TRACE("Before: %d x %d, After: %d x %d\n",
      surface_width_in_logical_units, surface_height_in_logical_units,
      width, height);

   // the adjusted center latitude closest to center_lat that will all for a
   // valid surface, and the dimensions of that valid surface
   *adjusted_center_lat = new_center_lat;
   *surface_width_in_logical_units = width;
   *surface_height_in_logical_units = height;

   return true;
}

static
int calculate_surface_scale_denom(MapScale data_scale, int surface_scale_percent,
   double* surface_scale_denom)
{
   double data_scale_denom = (double)data_scale.get_to_scale_denominator();

   ASSERT(surface_scale_percent != 0.0);
   if (surface_scale_percent == 0.0)
   {
      ERR_report("invalid surface scale percentage");
      return FAILURE;
   }

   if (surface_scale_percent != 100)
   {
      const double multiple = 100.0/static_cast<double>(surface_scale_percent);
      *surface_scale_denom = floor(data_scale_denom*multiple + 0.5);
   }
   else
   {
      *surface_scale_denom = data_scale_denom;
   }

   return SUCCESS;
}

// Returns true if the surface size is valid for the given scale, rotation,
// and surface_scale_percent.  Returns false if the surface size is too big
// for the given scale.  This happens when either surface dimension is greater
// than the number of pixels of data at the given scale.  For example, width_lu
// cannot be greater than the number of pixels around the world.  Even if the
// surface size is valid at 100% scale percent and no rotation, either of these
// can lead to an invalid surface condition.
static
bool test_surface_size(MapSource source, MapScale scale, MapSeries series, double rotation,
   int width_lu, int height_lu)
{
   bool valid_surface;
   bool can_rotate;
   bool can_zoom;
   int minimum_zoom;

   // test surface size, rotation, and zoom for this scale
   if (MAP_test_limits(scale, width_lu, height_lu, valid_surface, 
      can_rotate, can_zoom, minimum_zoom) != SUCCESS)
   {
      ERR_report("MAP_test_limits() failed");
      return false;
   }

   // if the surface size is invalid
   if (valid_surface == false)
      return false;

   // if the map is rotated, but the surface size does not allow rotation, then
   // treat the surface size as invalid
   if (can_rotate == false && rotation != 0.0)
      return false;


   // surface size is valid
   return true;
}

//
// Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
//
int SettableMapProjImpl::bind_equal_arc_map_to_scale_surface(double surface_width_in_inches, double surface_height_in_inches,
   int surface_scale_percent, bool calc_surface_logical_units /*true*/,
   int surface_width, int surface_height)
{
   double surface_scale_denom;
   int surface_width_in_logical_units;
   int surface_height_in_logical_units;

   // calculate the surface dimensions in logical units and the scale
   // denominator if this MapProj is used on a surface with the given
   // dimensions in inches and scale percent
   bool surface_size_valid;
   degrees_t adjusted_center_lat;
   if (calc_surface_logical_units_and_scale_denom(surface_width_in_inches, 
      surface_height_in_inches, surface_scale_percent,
      surface_scale_denom, surface_width_in_logical_units, 
      surface_height_in_logical_units, surface_size_valid,
      adjusted_center_lat) != SUCCESS)
   {
      ERR_report("calc_surface_logical_units_and_scale_denom() failed.");
      return FAILURE;
   }

   if (!calc_surface_logical_units)
   {
      surface_size_valid = true;
      adjusted_center_lat = requested_center_lat();

      MapSpec map_spec;

      // center, rotation, and zoom may differ between the requested and actual
      // specification

      map_spec.center = requested_center();
      map_spec.rotation = requested_rotation(); 
      map_spec.zoom_percent = requested_zoom_percent();

      map_spec.source = source();
      map_spec.scale = scale();
      map_spec.series = series();

      map_spec.projection_type = spec()->projection_type;

      map_spec.m_use_mosaic_map_parameters = spec()->m_use_mosaic_map_parameters;
      map_spec.m_mosaic_std_parallel_1 = spec()->m_mosaic_std_parallel_1;
      map_spec.m_mosaic_std_parallel_2 = spec()->m_mosaic_std_parallel_2;

      map_spec.m_mosaic_offset_x = spec()->m_mosaic_offset_x;
      map_spec.m_mosaic_offset_y = spec()->m_mosaic_offset_y;

      map_spec.m_mosaic_meters_per_pixel_lat = spec()->m_mosaic_meters_per_pixel_lat *
         (double)surface_width_in_logical_units / (double)surface_width;
      map_spec.m_mosaic_meters_per_pixel_lon = spec()->m_mosaic_meters_per_pixel_lon *
         (double)surface_height_in_logical_units / (double)surface_height;

      map_spec.m_mosaic_center_lat = spec()->m_mosaic_center_lat;
      map_spec.m_mosaic_center_lon = spec()->m_mosaic_center_lon;
      map_spec.m_mosaic_surface_width = spec()->m_mosaic_surface_width;
      map_spec.m_mosaic_surface_height = spec()->m_mosaic_surface_height;
      map_spec.m_row = spec()->m_row;
      map_spec.m_col = spec()->m_col;

      map_spec.m_mosaic_num_row = spec()->m_mosaic_num_row;
      map_spec.m_mosaic_num_col = spec()->m_mosaic_num_col;

      set_spec(map_spec);

      surface_width_in_logical_units = surface_width;
      surface_height_in_logical_units = surface_height;
   }

   if (surface_size_valid == false)
   {
      ERR_report("Surface size is too large for this scale.");
      return FAILURE;
   }

   if (set_surface_size(surface_width_in_logical_units, 
      surface_height_in_logical_units) != SUCCESS)
   {
      ERR_report("set_surface_size");
      return FAILURE;
   }

   bool unused;
   if (lcl_bind_equal_arc_map_to_surface(TRUE, TRUE, adjusted_center_lat, unused) !=
      SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_surface");
      return FAILURE;
   }

   //
   // set the "to-scale" member vars.  Do it after calling
   // bind_equal_arc_map_to_surface because this resets these vars.
   //
   map()->m_is_to_scale_projection = TRUE;
   map()->m_to_scale_surface_width_in_inches = surface_width_in_inches;
   map()->m_to_scale_surface_height_in_inches = surface_height_in_inches;
   map()->m_surface_scale_percent = surface_scale_percent;
   map()->m_surface_scale_denom = surface_scale_denom;

   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

int SettableMapProjImpl::bind_equal_arc_map_to_surface_using_geo_bounds(
   degrees_t northern_lat, degrees_t southern_lat, degrees_t western_lon, 
   degrees_t eastern_lon)
{
   //
   // make sure that the requested rotation is 0 and that the requested zoom is 100
   //
   if (requested_rotation() != 0.0)
   {
      ERR_report("invalid rotation");
      return FAILURE;
   }
   if (requested_zoom_percent() != 100)
   {
      ERR_report("invalid zoom percent");
      return FAILURE;
   }

   //
   // calculate the geo width and height and ensure their correctness
   //
   const degrees_t geo_height = northern_lat - southern_lat;
   ASSERT(geo_height > 0.0);
   if (geo_height <= 0.0)
   {
      ERR_report("invalid lat bounds");
      return FAILURE;
   }
   degrees_t geo_width = eastern_lon - western_lon;
   if (geo_width < 0.0)
      geo_width += 360.0;
   ASSERT(geo_width > 0.0);
   if (geo_width <= 0.0)
   {
      ERR_report("invalid lon bounds");
      return FAILURE;
   }
   
   //
   // set the requested map center based on the given bounds
   //
   {
      MapSpec spec;
      spec.source = source();
      spec.scale = scale();
      spec.series = series();
      spec.rotation = requested_rotation();
      spec.zoom_percent = requested_zoom_percent();
      degrees_t center_lat;
      degrees_t center_lon;
      {
         center_lat = southern_lat + geo_height/2.0;
         center_lon = western_lon + geo_width/2.0;
         if (center_lon > 180.0)
            center_lon -= 360.0;
      }

      spec.center.lat = center_lat;
      spec.center.lon = center_lon;

      spec.projection_type = requested_projection();

      set_spec(spec);
   }

   // get the map type so you can get the degrees per pixel values from it
   MapType map_type(source(), scale(), series());

   //
   // get the degrees per pixel at the center of the map
   //
   // note that surface dims are only used by MAP_get_degrees_per_pixel_world,
   // so they are not used here - since this is for to-scale surfaces and you
   // can not print a world map to scale
   //
   degrees_t deg_lat_per_pix;
   degrees_t deg_lon_per_pix;
   if (map_type.get_degrees_per_pixel_printer(requested_center_lat(), 
      &deg_lat_per_pix, &deg_lon_per_pix) != SUCCESS)
   {
      ERR_report("get_degrees_per_pixel_printer() failed.");
      return FAILURE;
   }

   //
   // figure out how many logical units (based on the above dpp) are in the given 
   // geo area
   //
   const int num_logical_units_north_south = 
      static_cast<int>(ceil(geo_height/deg_lat_per_pix));
   const int num_logical_units_east_west = 
      static_cast<int>(ceil(geo_width/deg_lon_per_pix));

   //
   // set the surface size
   //
   if (set_surface_size(num_logical_units_east_west, 
      num_logical_units_north_south) != SUCCESS)
   {
      ERR_report("set_surface_size");
      return FAILURE;
   }

   {
      const degrees_t unused1 = requested_center_lat();
      bool unused2;
      const int ret = lcl_bind_equal_arc_map_to_surface(TRUE, TRUE, unused1, unused2);
      
      if (ret !=  FAILURE)
      {
         map()->m_is_to_scale_projection = FALSE;
         map()->m_to_scale_surface_width_in_inches = 0.0;
         map()->m_to_scale_surface_height_in_inches = 0.0;
         map()->m_surface_scale_percent = 0;
         map()->m_surface_scale_denom = 0.0;
      }
      
      if (ret != SUCCESS)
      {
         ERR_report("bind_equal_arc_map_to_surface");
         return FAILURE;
      }
   }

   //
   // Note that if the center was changed during the binding, then the surface size
   // may be incorrect here - because the deg lon per pix may have changed.  
   // The UI should prevent maps from being out 
   // of bounds, so this case is not handled here.
   //

   return SUCCESS;
}

// ---------------------------------------------------------------------------
// bind_equal_arc_map_to_surface_using_geo_bounds() - this version of the 
// function takes four geo-graphical coordinates of the map bounds, calculates 
// meters per pixel from the printers degrees per pixel, and uses these values 
// to calculate the surface width and height
// ---------------------------------------------------------------------------
int SettableMapProjImpl::bind_equal_arc_map_to_surface_using_geo_bounds(
   d_geo_t ul, d_geo_t ur, d_geo_t ll, d_geo_t lr)
{
   //
   // make sure that the requested rotation is 0 and that the requested zoom is 100
   //
   if (requested_rotation() != 0.0)
   {
      ERR_report("invalid rotation");
      return FAILURE;
   }
   if (requested_zoom_percent() != 100)
   {
      ERR_report("invalid zoom percent");
      return FAILURE;
   }

   //
   // sanity checks
   //
   if (ul.lat - ll.lat <= 0.0)
   {
      ERR_report("invalid lat bounds");
      return FAILURE;
   }
   degrees_t geo_width = ur.lon - ul.lon;
   if (geo_width < 0.0)
      geo_width += 360.0;
   ASSERT(geo_width > 0.0);
   if (geo_width <= 0.0)
   {
      ERR_report("invalid lon bounds");
      return FAILURE;
   }
   
   //
   // note : the center should already have been set

   // get the map type so you can get the degrees per pixel values from it
   MapType map_type(source(), scale(), series());

   //
   // get the degrees per pixel at the center of the map
   //
   // note that surface dims are only used by MAP_get_degrees_per_pixel_world,
   // so they are not used here - since this is for to-scale surfaces and you
   // can not print a world map to scale
   //
   degrees_t deg_lat_per_pix;
   degrees_t deg_lon_per_pix;
   if (map_type.get_degrees_per_pixel_printer(requested_center_lat(), 
      &deg_lat_per_pix, &deg_lon_per_pix) != SUCCESS)
   {
      ERR_report("get_degrees_per_pixel_printer() failed.");
      return FAILURE;
   }

   //
   // now, using the degrees per pixel calculate meters per pixel lat/lon
   //
   double meters_per_pixel_lat;
   {
      double m_p_p, dummy;
      GEO_geo_to_distance(requested_center_lat(), 
         requested_center_lon(), requested_center_lat() + deg_lat_per_pix, 
         requested_center_lon(), &m_p_p, &dummy);

      // Adjust resolution to take zoom into account
      const int zoom_percent = requested_zoom_percent();
      if (zoom_percent != 100) 
      {
         if (zoom_percent > 100) 
         {
            const double factor = ((double)zoom_percent/(double)100);
            m_p_p /= factor;
         }
         else if (zoom_percent < 100) 
         {
            const double factor = ((double)100/(double)zoom_percent);
            m_p_p *= factor;
         }
      }
      meters_per_pixel_lat = m_p_p;
   }

   double meters_per_pixel_lon;
   {
      double m_p_p, dummy;
      GEO_geo_to_distance(requested_center_lat(), 
         requested_center_lon(), requested_center_lat(), 
         requested_center_lon() + deg_lon_per_pix, 
         &m_p_p, &dummy);

      // Adjust resolution to take zoom into account
      const int zoom_percent = requested_zoom_percent();
      if (zoom_percent != 100) 
      {
         if (zoom_percent > 100) 
         {
            const double factor = ((double)zoom_percent/(double)100);
            m_p_p /= factor;
         }
         else if (zoom_percent < 100) 
         {
            const double factor = ((double)100/(double)zoom_percent);
            m_p_p *= factor;
         }
      }
      meters_per_pixel_lon = m_p_p;
   }

   //
   // figure out how many logical units (based on the above dpp) are in the given 
   // geo area
   //
   double distance_ns, distance_ew;
   double unused;
   GEO_geo_to_distance(ll.lat, ll.lon, ul.lat, ul.lon, &distance_ns, &unused);
   GEO_geo_to_distance(ul.lat, ul.lon, ur.lat, ur.lon, &distance_ew, &unused);

   const int num_logical_units_north_south = 
      static_cast<int>(ceil(distance_ns/meters_per_pixel_lat));
   const int num_logical_units_east_west = 
      static_cast<int>(ceil(distance_ew/meters_per_pixel_lon));

   //
   // set the surface size
   //
   if (set_surface_size(num_logical_units_east_west, 
      num_logical_units_north_south) != SUCCESS)
   {
      ERR_report("set_surface_size");
      return FAILURE;
   }

   {
      const degrees_t unused1 = requested_center_lat();
      bool unused2;
      const int ret = lcl_bind_equal_arc_map_to_surface(TRUE, TRUE, unused1, unused2);
      
      if (ret !=  FAILURE)
      {
         map()->m_is_to_scale_projection = FALSE;
         map()->m_to_scale_surface_width_in_inches = 0.0;
         map()->m_to_scale_surface_height_in_inches = 0.0;
         map()->m_surface_scale_percent = 0;
         map()->m_surface_scale_denom = 0.0;
      }
      
      if (ret != SUCCESS)
      {
         ERR_report("bind_equal_arc_map_to_surface");
         return FAILURE;
      }
   }

   //
   // Note that if the center was changed during the binding, then the surface size
   // may be incorrect here - because the deg lon per pix may have changed.  
   // The UI should prevent maps from being out 
   // of bounds, so this case is not handled here.
   //

   return SUCCESS;
}

int SettableMapProjImpl::bind_equal_arc_map_to_scale_surface_using_geo_bounds(
   degrees_t northern_lat, degrees_t southern_lat, 
   degrees_t western_lon, degrees_t eastern_lon,
   double surface_width_in_inches, double surface_height_in_inches,
   int surface_scale_percent)
{
   if (bind_equal_arc_map_to_surface_using_geo_bounds(northern_lat, southern_lat,
      western_lon, eastern_lon) != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_surface_using_geo_bounds");
      return FAILURE;
   }

   //
   // set the appropriate flags and attributes for a to-scale printout
   //
   map()->m_is_to_scale_projection = TRUE;
   map()->m_to_scale_surface_width_in_inches = surface_width_in_inches;
   map()->m_to_scale_surface_height_in_inches = surface_height_in_inches;
   map()->m_surface_scale_percent = surface_scale_percent;
   double surface_scale_denom;
   {
      if (calculate_surface_scale_denom(scale(), surface_scale_percent,
         &surface_scale_denom) != SUCCESS)
      {
         ERR_report("calculate_surface_scale_denom");
         return FAILURE;
      }
   }
   map()->m_surface_scale_denom = surface_scale_denom;

   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

//
// component_map_row is the row index of the component map in the mosaic map.  For example,
// if the component map is in the third rows of the component maps that compose that mosaic
// map, then (depending on the numbering system) component_map_row might be 2.  Similarly 
// for component_map_col.
//
int SettableMapProjImpl::bind_equal_arc_map_to_scale_mosaic_component_surface(
   degrees_t northern_lat, degrees_t southern_lat, 
   degrees_t western_lon, degrees_t eastern_lon, 
   double component_surface_width_in_inches, double component_surface_height_in_inches,
   int surface_scale_percent, const MapProj* mosaic_map, 
   int component_map_row, int component_map_col,
   double component_map_vertical_offset_in_inches,
   double component_map_horizontal_offset_in_inches)
{
   if (bind_equal_arc_map_to_surface_using_geo_bounds(northern_lat,
      southern_lat, western_lon, eastern_lon) != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_surface_using_geo_bounds");
      return FAILURE;
   }

   //
   // set the appropriate flags and attributes for a to-scale printout
   //
   map()->m_is_to_scale_projection = TRUE;
   map()->m_to_scale_surface_width_in_inches = component_surface_width_in_inches;
   map()->m_to_scale_surface_height_in_inches = component_surface_height_in_inches;
   map()->m_surface_scale_percent = surface_scale_percent;
   double surface_scale_denom;
   {
      if (calculate_surface_scale_denom(scale(), surface_scale_percent,
         &surface_scale_denom) != SUCCESS)
      {
         ERR_report("calculate_surface_scale_denom");
         return FAILURE;
      }
   }
   map()->m_surface_scale_denom = surface_scale_denom;

   //
   // now calculate the values that will be used for converting from mosaic
   // map logical unit coordinates to composite map logical unit coordinates
   //

   double component_map_row_offset_in_mosaic_map_pixmap;
   double component_map_col_offset_in_mosaic_map_pixmap;
   if (calculate_component_map_offset_in_mosaic_map(mosaic_map, this,
      &component_map_row_offset_in_mosaic_map_pixmap,
      &component_map_col_offset_in_mosaic_map_pixmap) != SUCCESS)
   {
      ERR_report("calculate_component_map_offset_in_mosaic_map");
      return FAILURE;
   }
   //
   // calculate the scale factors between the logical units of the
   // component map and the logical units of the mosaic map
   //
   const double mosaic_to_component_pixel_lat_scale_ratio =
      mosaic_map->vmap_degrees_per_pixel_lat()/this->vmap_degrees_per_pixel_lat();
   const double mosaic_to_component_pixel_lon_scale_ratio =
      mosaic_map->vmap_degrees_per_pixel_lon()/this->vmap_degrees_per_pixel_lon();

   //
   // set the appropriate mosaic map information in the map
   //

   map()->set_mosaic_map_ptr(mosaic_map);
   map()->set_is_mosaic_map_component(TRUE);
   map()->set_component_map_row(component_map_row);
   map()->set_component_map_column(component_map_col);

   map()->set_component_map_row_offset_in_mosaic_map_pixmap(
      component_map_row_offset_in_mosaic_map_pixmap);
   map()->set_component_map_col_offset_in_mosaic_map_pixmap(
      component_map_col_offset_in_mosaic_map_pixmap);

   map()->set_component_map_vertical_offset_in_inches(
      component_map_vertical_offset_in_inches);
   map()->set_component_map_horizontal_offset_in_inches(
      component_map_horizontal_offset_in_inches);

   map()->set_mosaic_to_component_pixel_lat_scale_ratio(
      mosaic_to_component_pixel_lat_scale_ratio);
   map()->set_mosaic_to_component_pixel_lon_scale_ratio(
      mosaic_to_component_pixel_lon_scale_ratio);

   return SUCCESS;
}

int SettableMapProjImpl::bind_equal_arc_map_to_scale_mosaic_component_surface_lambert(
   d_geo_t ul, d_geo_t ur, d_geo_t ll, d_geo_t lr,
   double component_surface_width_in_inches, double component_surface_height_in_inches,
   int surface_scale_percent, const MapProj* mosaic_map, 
   int component_map_row, int component_map_col,
   double component_map_vertical_offset_in_inches,
   double component_map_horizontal_offset_in_inches)
{
   if (bind_equal_arc_map_to_surface_using_geo_bounds(ul, ur, ll, lr) != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_surface_using_geo_bounds");
      return FAILURE;
   }

   //
   // set the appropriate flags and attributes for a to-scale printout
   //
   map()->m_is_to_scale_projection = TRUE;
   map()->m_to_scale_surface_width_in_inches = component_surface_width_in_inches;
   map()->m_to_scale_surface_height_in_inches = component_surface_height_in_inches;
   map()->m_surface_scale_percent = surface_scale_percent;
   double surface_scale_denom;
   {
      if (calculate_surface_scale_denom(scale(), surface_scale_percent,
         &surface_scale_denom) != SUCCESS)
      {
         ERR_report("calculate_surface_scale_denom");
         return FAILURE;
      }
   }
   map()->m_surface_scale_denom = surface_scale_denom;

   //
   // now calculate the values that will be used for converting from mosaic
   // map logical unit coordinates to composite map logical unit coordinates
   //

   double component_map_row_offset_in_mosaic_map_pixmap = spec()->m_row * get_surface_height();
   double component_map_col_offset_in_mosaic_map_pixmap = spec()->m_col * get_surface_width();

   //if (calculate_component_map_offset_in_mosaic_map(mosaic_map, this,
   //   &component_map_row_offset_in_mosaic_map_pixmap,
   //   &component_map_col_offset_in_mosaic_map_pixmap) != SUCCESS)
   //{
   //   ERR_report("calculate_component_map_offset_in_mosaic_map");
   //   return FAILURE;
   //}
   //
   // calculate the scale factors between the logical units of the
   // component map and the logical units of the mosaic map
   //
   const double mosaic_to_component_pixel_lat_scale_ratio =
      mosaic_map->vmap_degrees_per_pixel_lat()/this->vmap_degrees_per_pixel_lat();
   const double mosaic_to_component_pixel_lon_scale_ratio =
      mosaic_map->vmap_degrees_per_pixel_lon()/this->vmap_degrees_per_pixel_lon();

   //
   // set the appropriate mosaic map information in the map
   //

   map()->set_mosaic_map_ptr(mosaic_map);
   map()->set_is_mosaic_map_component(TRUE);
   map()->set_component_map_row(component_map_row);
   map()->set_component_map_column(component_map_col);

   map()->set_component_map_row_offset_in_mosaic_map_pixmap(
      component_map_row_offset_in_mosaic_map_pixmap);
   map()->set_component_map_col_offset_in_mosaic_map_pixmap(
      component_map_col_offset_in_mosaic_map_pixmap);

   map()->set_component_map_vertical_offset_in_inches(
      component_map_vertical_offset_in_inches);
   map()->set_component_map_horizontal_offset_in_inches(
      component_map_horizontal_offset_in_inches);

   map()->set_mosaic_to_component_pixel_lat_scale_ratio(
      mosaic_to_component_pixel_lat_scale_ratio);
   map()->set_mosaic_to_component_pixel_lon_scale_ratio(
      mosaic_to_component_pixel_lon_scale_ratio);

   return SUCCESS;
}

// Check if the map surface has any invalid or invisible points
// due to a change in scale/projection
bool SettableMapProjImpl::has_invalid_points(void) const
{
   if ( is_projection_set() == FALSE )
      return false;

   // dont's allow Lambert at smaller the 1:80M scales
   if ( this->actual_projection() == LAMBERT_PROJECTION &&
      this->scale() < ONE_TO_80M )
   {
      return true;
   }



   int w  = get_surface_width();
   int h = get_surface_height();

   degrees_t lat, lon;
   
   if (  surface_to_geo(0, 0, &lat, &lon) != SUCCESS 
      || surface_to_geo(w/2, 0, &lat, &lon) != SUCCESS 
      || surface_to_geo(w-1, 0, &lat, &lon) != SUCCESS 
      || surface_to_geo(w-1, h/2, &lat, &lon) != SUCCESS 
      || surface_to_geo(w-1, h-1, &lat, &lon) != SUCCESS 
      || surface_to_geo(w/2, h-1, &lat, &lon) != SUCCESS 
      || surface_to_geo(0, h-1, &lat, &lon) != SUCCESS 
      || surface_to_geo(0, h/2, &lat, &lon) != SUCCESS )
   {
      return true;
   }

   return false;
}

// Default Constructor to insure consistent initialization
MapSpec::MapSpec()
{
   center.lat = 0.0;
   center.lon = 0.0;
   source = NULL_SOURCE;
   scale = NULL_SCALE;
   series = NULL_SERIES;
   rotation = 0.0;
   zoom_percent = 100;
   projection_type = PROJ_get_default_projection_type();
   m_use_mosaic_map_parameters = false;
   m_mosaic_std_parallel_1 = 0.0;
   m_mosaic_std_parallel_2 = 0.0;
   m_mosaic_offset_x = 0.0;
   m_mosaic_offset_y = 0.0;
   m_mosaic_meters_per_pixel_lat = 0.0;
   m_mosaic_meters_per_pixel_lon = 0.0;
   m_mosaic_center_lat = 0.0;
   m_mosaic_center_lon = 0.0;
   m_mosaic_surface_width = 0;
   m_mosaic_surface_height = 0;
   m_row = 0;
   m_col = 0;
   m_mosaic_num_row = 0;
   m_mosaic_num_row = 0;
}

//
// NOTE: this can return negative values (when a component map
// extends beyond some edges of the mosaic map)
//
static
int calculate_component_map_offset_in_mosaic_map(
   const MapProj* mosaic_map, const MapProj* component_map,
   double* component_map_row_in_mosaic_map, 
   double* component_map_column_in_mosaic_map)
{
   //
   // get the geo bounds of the mosaic map.  NOTE: make sure to use
   // the bounds at the pixel edges here, not the pixel centers.
   //
   d_geo_t mosaic_map_ll;
   d_geo_t mosaic_map_ur;
   if (mosaic_map->get_vmap_bounds_at_pixel_edges(&mosaic_map_ll, 
      &mosaic_map_ur) != SUCCESS)
   {
      ERR_report("get_vmap_bounds_at_pixel_edges");
      return FAILURE;
   }
   const degrees_t mosaic_map_northern_bound = mosaic_map_ur.lat;
   const degrees_t mosaic_map_western_bound = mosaic_map_ll.lon;

   //
   // Find the pixel in the mosaic map in which the pixel center of the upper
   // left corner pixel of the composite map lies.  Note that the pixel 
   // (i.e. logical unit) sizes can differ between the mosaic map and the 
   // component map. Thus the pixels centers do not neccessaliry line up exactly.
   //
   double pixel_row_offset;
   double pixel_col_offset;
   {
      degrees_t component_map_upper_left_lat;
      degrees_t component_map_upper_left_lon;
      component_map->surface_to_geo(0, 0, &component_map_upper_left_lat,
         &component_map_upper_left_lon);

      //
      // determine the row offset
      //
      {
         const degrees_t geo_height_diff = 
            mosaic_map_northern_bound - component_map_upper_left_lat;
         const degrees_t mosaic_map_dpp_lat= mosaic_map->vmap_degrees_per_pixel_lat();
         pixel_row_offset = geo_height_diff/mosaic_map_dpp_lat;
      }

      //
      // determine the column offset
      //
      {
         degrees_t geo_width_diff = 
            component_map_upper_left_lon - mosaic_map_western_bound;
         //
         // handle the IDL
         //
         if (!GEO_east_of_degrees(component_map_upper_left_lon, mosaic_map_western_bound))
         {
            if (component_map_upper_left_lon > mosaic_map_western_bound)
               geo_width_diff -= 360;
         }
         else if (GEO_east_of_degrees(component_map_upper_left_lon, mosaic_map_western_bound))
         {
            if (component_map_upper_left_lon < mosaic_map_western_bound)
               geo_width_diff += 360;
         }
         const degrees_t mosaic_map_dpp_lon = mosaic_map->vmap_degrees_per_pixel_lon();
         pixel_col_offset = geo_width_diff/mosaic_map_dpp_lon;
      }
   }

   *component_map_row_in_mosaic_map = pixel_row_offset;
   *component_map_column_in_mosaic_map = pixel_col_offset;

   return SUCCESS;
}

//
// Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC or FV_NO_DATA.
//
int SettableMapProjImpl::bind_equal_arc_map_to_surface_and_check_data(void)
{
// SHOULD THE PROJECTION NOT BE SET HERE (i.e. should is_projection_set()
// be false)   ????

   //
   // calculate out the map bounds, etc.
   //
   if (bind_equal_arc_map_to_surface() != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_surface failed");
// ***** SHOULD DO A CLEAR HERE?
      return FAILURE;
   }

   //
   // Note that data_check_without_setting_projection can return
   // COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
   //
   const int ret = data_check_without_setting_projection();
   if (ret == FAILURE)
   {
      ERR_report("data_check_without_setting_projection");
// ***** SHOULD DO A CLEAR HERE?
   }

   return ret;
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA or COV_FILE_OUT_OF_SYNC.
//
int SettableMapProjImpl::bind_equal_arc_map_to_scale_surface_and_check_data(double surface_width_in_inches,
   double surface_height_in_inches, int surface_scale_percent)
{
// SHOULD THE PROJECTION NOT BE SET HERE (i.e. should is_projection_set()
// be false)   ????

   //
   // calculate out the map bounds, etc.
   //
   if (bind_equal_arc_map_to_scale_surface(surface_width_in_inches,
      surface_height_in_inches, surface_scale_percent) != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_surface failed");
// ***** SHOULD DO A CLEAR HERE?
      return FAILURE;
   }

   //
   // Note that data_check_without_setting_projection can return
   // COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
   //
   const int ret = data_check_without_setting_projection();
   if (ret == FAILURE)
   {
      ERR_report("data_check_without_setting_projection");
// ***** SHOULD DO A CLEAR HERE?
   }

   return ret;
}

//
// Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
//
int SettableMapProjImpl::data_check_without_setting_projection(void)
{
   ASSERT(is_projection_set());
   if (!is_projection_set())
   {
      ERR_report("proj not set");
      return FAILURE;

   }

   // 
   // Note that get_and_test_coverage_list can return COV_FILE_OUT_OF_SYNC
   // and FV_NO_DATA.
   //
   const int ret = get_and_test_coverage_list();

   //
   // mark the data check as having been performed
   //
   if (ret != FAILURE)
      m_data_check_done = TRUE; 

   //
   // store the data check return value
   //
   m_data_check_result = ret;

   return ret;
}

//
// Returns SUCCESS, FAILURE, FV_NO_DATA
//
int SettableMapProjImpl::get_and_test_coverage_list() const
{
   if (source() == BLANK_MAP)
      return SUCCESS;

   d_geo_t map_ll, map_ur;
   if (get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
   {  
      ERR_report("get_vmap_bounds failed");
      return FAILURE;
   }

   try
   {
      // get the map handler name from the source, scale, and series
      IMapGroupsPtr spMapGroups;
      CO_CREATE(spMapGroups, CLSID_MapGroups);
      if (spMapGroups->SelectPositionOnProductScale(0, _bstr_t(source().get_string()), scale().GetScale(), 
         scale().GetScaleUnits(), _bstr_t(series().get_string())) == S_OK)
      {
         if (spMapGroups->m_DoesDataExist)
         {
            MapView *pMapView = static_cast<MapView *>(UTL_get_active_non_printing_view());
            if (pMapView != NULL)
            {
               // get the IMapRender interface associated with the given product name (source)
               IDispatchPtr spDispatch;
               pMapView->get_map_engine()->GetMapHandler(source(), &spDispatch);

               IMapRenderPtr spMapRender = spDispatch;

               if (spMapRender != NULL)
               {
                  // determine if there is any data available within the given geographic bounds
                  //
                  MapStatusCodeEnum status;

                  HRESULT hr = spMapRender->DataCheck( _bstr_t(source().get_string()), scale().GetScale(),
                     scale().GetScaleUnits(), _bstr_t(series().get_string()), map_ll.lat, map_ll.lon,
                     map_ur.lat, map_ur.lon, &status);

                  if (hr == S_OK && status == MAP_STATUS_OK)
                     return SUCCESS;
               }
            }
         }

         return FV_NO_DATA;
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("SettableMapProjImpl::get_and_test_coverage_list failed - %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return FAILURE;
}
