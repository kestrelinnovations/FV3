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
#include "map.h"
#include "mapx.h"
#include "proj.h"




BOOL MapProj::pole_on_surface() const 
{
	const ProjectionEnum proj_type = actual_projection();

	if(scale() == WORLD)
	{
		return TRUE;
	}
	if ((proj_type == EQUALARC_PROJECTION) || (proj_type == MERCATOR_PROJECTION))
	{
		return FALSE;
	}
	else
	{
		int screen_width, screen_height;
		int tx, ty, rslt;

		get_surface_size(&screen_width, &screen_height);
		rslt = geo_to_surface(89.999999, 0.0, &tx, &ty);

		if (rslt != SUCCESS)
			return FALSE;

		if ((tx >= 0) && (tx <= screen_width) &&
			(ty >= 0) && (ty <= screen_height))
			return TRUE;

		rslt = geo_to_surface(-89.999999, 0.0, &tx, &ty);

		if (rslt != SUCCESS)
			return FALSE;

		if ((tx >= 0) && (tx <= screen_width) &&
			(ty >= 0) && (ty <= screen_height))
			return TRUE;
	}

	return FALSE;
}


// CMapProjWrapper implementation
//

// constructor
CMapProjWrapper::CMapProjWrapper(ISettableMapProj *pSettableMapProj)
{
   m_spSettableMapProj = pSettableMapProj;
   m_spSettableMapProj2 = m_spSettableMapProj;
}

ISettableMapProj* CMapProjWrapper::GetInterfacePtr()
{
   return m_spSettableMapProj;
}

boolean_t CMapProjWrapper::is_projection_set(void) const
{
   int is_set;
   m_spSettableMapProj->is_projection_set(&is_set);
   return is_set;
}

boolean_t CMapProjWrapper::is_spec_set(void) const
{
   int is_set;
   m_spSettableMapProj->is_spec_set(&is_set);
   return is_set;
}

boolean_t CMapProjWrapper::is_surface_set(void) const
{
   int is_set;
   m_spSettableMapProj->is_surface_set(&is_set);
   return is_set;
}

d_geo_t CMapProjWrapper::requested_center(void) const 
{
   d_geo_t center;
   m_spSettableMapProj->requested_center(&center.lat, &center.lon);
   return center;
}

MapSource CMapProjWrapper::source(void) const 
{
   _bstr_t src;
   m_spSettableMapProj->source(src.GetAddress());
   MapSource ret = MapSource(src);
   return ret;
}

MapSource CMapProjWrapper::actual_source(void) const
{
   _bstr_t actual_src = m_spSettableMapProj2->get_actual_source();
   return MapSource(actual_src);
}

MapScale CMapProjWrapper::scale(void) const 
{
   double dScale;
   MapScaleUnitsEnum eScaleUnits;
   m_spSettableMapProj->scale(&dScale, &eScaleUnits);

   if (eScaleUnits == MAP_SCALE_WORLD)
      return MapScale(WORLD);

   MapScale::unit_t units;
   switch(eScaleUnits)
   {
   case MAP_SCALE_NM: units = MapScale::NM; break;
   case MAP_SCALE_KILOMETER: units = MapScale::KILOMETER; break;
   case MAP_SCALE_METERS: units = MapScale::METERS; break;
   case MAP_SCALE_ARC_SECONDS: units = MapScale::ARC_SECONDS; break;
   case MAP_SCALE_ARC_MINUTES: units = MapScale::ARC_MINUTES; break;
   }

   if (eScaleUnits == MAP_SCALE_DENOMINATOR)
      return MapScale(static_cast<int>(dScale));

   return MapScale(dScale, units);
}

MapSeries CMapProjWrapper::series(void) const
{
   _bstr_t series_str;
   m_spSettableMapProj->series(series_str.GetAddress());
   MapSeries ret = MapSeries(series_str);
   return ret;
}

ProjectionEnum CMapProjWrapper::actual_projection(void) const
{
   ProjectionEnum proj;
   m_spSettableMapProj->actual_projection(&proj);
   return proj;
}

ProjectionEnum CMapProjWrapper::requested_projection(void) const
{
   ProjectionEnum proj;
   m_spSettableMapProj->requested_projection(&proj);
   return proj;
}

double CMapProjWrapper::requested_rotation(void) const 
{
   double rot;
   m_spSettableMapProj->requested_rotation(&rot);
   return rot;
}

int CMapProjWrapper::requested_zoom_percent(void) const 
{
   int zoom;
   m_spSettableMapProj->requested_zoom_percent(&zoom);
   return zoom;
}

degrees_t CMapProjWrapper::requested_center_lat(void) const 
{
   double lat;
   m_spSettableMapProj->requested_center_lat(&lat);
   return lat;
}

degrees_t CMapProjWrapper::requested_center_lon(void) const 
{
   double lon;
   m_spSettableMapProj->requested_center_lon(&lon);
   return lon;
}

int CMapProjWrapper::get_surface_width(void) const 
{
   int width;
   m_spSettableMapProj->get_surface_width(&width);
   return width;
}

int CMapProjWrapper::get_surface_height(void) const 
{
   int height;
   m_spSettableMapProj->get_surface_height(&height);
   return height;
}

int CMapProjWrapper::get_surface_size(int* surface_width, int* surface_height) const
{
   int result;
   m_spSettableMapProj->get_surface_size(surface_width, surface_height, &result);
   return result;
}

double CMapProjWrapper::actual_rotation(void) const
{
   double rot;
   m_spSettableMapProj->actual_rotation(&rot);
   return rot;
}

int CMapProjWrapper::actual_zoom_percent(void) const
{
   int zoom;
   m_spSettableMapProj->actual_zoom_percent(&zoom);
   return zoom;
}

int CMapProjWrapper::get_actual_center(d_geo_t* center) const
{
   int result;
   m_spSettableMapProj->get_actual_center(&center->lat, &center->lon, &result);
   return result;
}

degrees_t CMapProjWrapper::actual_center_lat(void) const
{
   double lat;
   m_spSettableMapProj->actual_center_lat(&lat);
   return lat;
}

degrees_t CMapProjWrapper::actual_center_lon(void) const
{
   double lon;
   m_spSettableMapProj->actual_center_lon(&lon);
   return lon;
}

void CMapProjWrapper::get_vsurface_size(int* virtual_surface_width, int* virtual_surface_height) const
{
   m_spSettableMapProj->get_vsurface_size(virtual_surface_width, virtual_surface_height,
      EQUALARC_VSURFACE);
}

int CMapProjWrapper::vwidth(void) const
{
   int vwidth;
   m_spSettableMapProj->vwidth(&vwidth, EQUALARC_VSURFACE);
   return vwidth;
}

int CMapProjWrapper::vheight(void) const
{
   int vheight;
   m_spSettableMapProj->vheight(&vheight, EQUALARC_VSURFACE);
   return vheight;
}

int CMapProjWrapper::get_vmap_bounds(d_geo_t *map_ll, d_geo_t *map_ur) const
{
   int result;
   m_spSettableMapProj->get_vmap_bounds(&map_ll->lat, &map_ll->lon, &map_ur->lat, &map_ur->lon,
      &result, EQUALARC_VSURFACE);
   return result;
}

d_geo_t CMapProjWrapper::vmap_sw_corner(void) const
{
   d_geo_t corner;
   m_spSettableMapProj->vmap_sw_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
   return corner;
}

d_geo_t CMapProjWrapper::vmap_nw_corner(void) const
{
   d_geo_t corner;
   m_spSettableMapProj->vmap_nw_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
   return corner;
}

d_geo_t CMapProjWrapper::vmap_ne_corner(void) const
{
   d_geo_t corner;
   m_spSettableMapProj->vmap_ne_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
   return corner;
}

d_geo_t CMapProjWrapper::vmap_se_corner(void) const
{
   d_geo_t corner;
   m_spSettableMapProj->vmap_se_corner(&corner.lat, &corner.lon, EQUALARC_VSURFACE);
   return corner;
}

degrees_t CMapProjWrapper::vmap_southern_bound(void) const
{
   double bound;
   m_spSettableMapProj->vmap_southern_bound(&bound, EQUALARC_VSURFACE);
   return bound;
}

degrees_t CMapProjWrapper::vmap_northern_bound(void) const
{
   double bound;
   m_spSettableMapProj->vmap_northern_bound(&bound, EQUALARC_VSURFACE);
   return bound;
}

degrees_t CMapProjWrapper::vmap_western_bound(void) const
{
   double bound;
   m_spSettableMapProj->vmap_western_bound(&bound, EQUALARC_VSURFACE);
   return bound;
}

degrees_t CMapProjWrapper::vmap_eastern_bound(void) const
{
   double bound;
   m_spSettableMapProj->vmap_eastern_bound(&bound, EQUALARC_VSURFACE);
   return bound;
}

int CMapProjWrapper::get_vmap_degrees_per_pixel(degrees_t *degrees_lat_per_pixel,
                                                degrees_t *degrees_lon_per_pixel) const
{
   int result;
   m_spSettableMapProj->get_vmap_degrees_per_pixel(degrees_lat_per_pixel,
      degrees_lon_per_pixel, &result, EQUALARC_VSURFACE);
   return result;
}

degrees_t CMapProjWrapper::vmap_degrees_per_pixel_lat(void) const
{
   double dpp;
   m_spSettableMapProj->vmap_degrees_per_pixel_lat(&dpp, EQUALARC_VSURFACE);
   return dpp;
}

degrees_t CMapProjWrapper::vmap_degrees_per_pixel_lon(void) const
{
   double dpp;
   m_spSettableMapProj->vmap_degrees_per_pixel_lon(&dpp, EQUALARC_VSURFACE);
   return dpp;
}

int CMapProjWrapper::get_vmap_bounds_at_pixel_edges(d_geo_t *map_ll, d_geo_t *map_ur) const
{
   int result;
   m_spSettableMapProj->get_vmap_bounds_at_pixel_edges(&map_ll->lat, &map_ll->lon, &map_ur->lat,
      &map_ur->lon, &result, EQUALARC_VSURFACE);
   return result;
}

int CMapProjWrapper::get_pixels_around_world(int *x_pixels_around_world) const
{
   int result;
   m_spSettableMapProj->get_pixels_around_world(x_pixels_around_world, &result);
   return result;
}

void CMapProjWrapper::geo_to_vsurface(degrees_t latitude, degrees_t longitude,
                                      int *x_coord, int *y_coord) const
{
   m_spSettableMapProj->geo_to_vsurface(latitude, longitude, x_coord, y_coord, EQUALARC_VSURFACE);
}

void CMapProjWrapper::geo_to_vsurface_rect(degrees_t ll_lat, degrees_t ll_lon, 
                                           degrees_t ur_lat, degrees_t ur_lon,
                                           int *ul_x, int *ul_y, int *lr_x, int *lr_y) const
{
   m_spSettableMapProj->geo_to_vsurface_rect(ll_lat, ll_lon, ur_lat, ur_lon, ul_x, ul_y,
      lr_x, lr_y, EQUALARC_VSURFACE);
}

void CMapProjWrapper::vsurface_to_geo(int x_coord, int y_coord,
                                      degrees_t *latitude, degrees_t *longitude) const
{
   m_spSettableMapProj->vsurface_to_geo(x_coord, y_coord, latitude, longitude, EQUALARC_VSURFACE);
}

void CMapProjWrapper::vsurface_to_geo_rect(int ul_x, int ul_y, int lr_x, int lr_y,
                                           degrees_t *ll_lat, degrees_t *ll_lon, degrees_t *ur_lat, degrees_t *ur_lon) const
{
   m_spSettableMapProj->vsurface_to_geo_rect(ul_x, ul_y, lr_x, lr_y, ll_lat, ll_lon, ur_lat, ur_lon,
      EQUALARC_VSURFACE);
}

int CMapProjWrapper::geo_to_surface(degrees_t latitude, degrees_t longitude,
                                    int *surface_x, int *surface_y) const
{
   int result;
   m_spSettableMapProj->geo_to_surface(latitude, longitude, surface_x, surface_y, &result);

   if (result == NONVISIBLE_RESULT)
   {
      *surface_x = -2147483647;
      *surface_y = -2147483647;
   }

   return result == FAILURE ? FAILURE : SUCCESS;
}

int CMapProjWrapper::geo_to_surface(degrees_t latitude, degrees_t longitude,
                                    double *surface_x, double *surface_y) const
{
   int result;
   m_spSettableMapProj->geo_to_surface_dbl(latitude, longitude, surface_x, surface_y, &result);

   // FalconView is not checking the NONVISIBLE_RESULT
   return result == FAILURE ? FAILURE : SUCCESS;
}

int CMapProjWrapper::surface_to_geo(int surface_x, int surface_y,
                                    degrees_t *latitude, degrees_t *longitude) const
{
   int result;
   m_spSettableMapProj->surface_to_geo(surface_x, surface_y, latitude, longitude, &result);
   return result;
}

int CMapProjWrapper::surface_to_geo(double surface_x, double surface_y,
                                    degrees_t *latitude, degrees_t *longitude) const
{
   int result;
   m_spSettableMapProj->surface_to_geo_dbl(surface_x, surface_y, latitude, longitude, &result);
   return result;
}

boolean_t CMapProjWrapper::geo_in_surface(double lat, double lon) const
{
   int in_surface;
   m_spSettableMapProj->geo_in_surface(lat, lon, &in_surface);
   return in_surface;
}

boolean_t CMapProjWrapper::geo_in_surface(double lat, double lon, int* s_x, int* s_y) const
{
   int in_surface;
   m_spSettableMapProj->geo_in_surface_scr(lat, lon, s_x, s_y, &in_surface);
   return in_surface;
}

boolean_t CMapProjWrapper::point_in_surface(int x, int y) const
{
   int in_surface;
   m_spSettableMapProj->point_in_surface(x, y, &in_surface);
   return in_surface;
}

int CMapProjWrapper::vsurface_to_surface(int vs_x, int vs_y, int* s_x, int* s_y) const
{
   int result;
   m_spSettableMapProj->vsurface_to_surface(vs_x, vs_y, s_x, s_y, &result, EQUALARC_VSURFACE);
   return result;
}

int CMapProjWrapper::vsurface_to_surface(double vs_x, double vs_y, double* s_x, double* s_y) const
{
   int result;
   m_spSettableMapProj->vsurface_to_surface_dbl(vs_x, vs_y, s_x, s_y, &result, EQUALARC_VSURFACE);
   return result;
}

int CMapProjWrapper::surface_to_vsurface(int s_x, int s_y, int* vs_x, int* vs_y) const
{
   int result;
   m_spSettableMapProj->surface_to_vsurface(s_x, s_y, vs_x, vs_y, &result, EQUALARC_VSURFACE);
   return result;
}

int CMapProjWrapper::surface_to_vsurface(double s_x, double s_y, double* vs_x, double* vs_y) const
{
   int result;
   m_spSettableMapProj->surface_to_vsurface_dbl(s_x, s_y, vs_x, vs_y, &result, EQUALARC_VSURFACE);
   return result;
}

BOOL CMapProjWrapper::geoline_to_surface(double dLat1, double dLon1, double dLat2, double dLon2,
                                         int* x1, int* y1, int* x2, int* y2, int* wrapped_x1, int* wrapped_y1, int* wrapped_x2, int* wrapped_y2)
{
   return m_spSettableMapProj2->geoline_to_surface(dLat1, dLon1, dLat2, dLon2,
      x1, y1, x2, y2, wrapped_x1, wrapped_y1, wrapped_x2, wrapped_y2);
}

int CMapProjWrapper::get_meridian_covergence(d_geo_t geo_pos, degrees_t *convergence)
{
   int result;
   m_spSettableMapProj->get_meridian_covergence(geo_pos.lat, geo_pos.lon, convergence, &result);
   return result;
}

boolean_t CMapProjWrapper::is_to_scale_projection(void) const
{
   int to_scale;
   m_spSettableMapProj->is_to_scale_projection(&to_scale);
   return to_scale;
}

double CMapProjWrapper::to_scale_surface_width_in_inches(void) const
{
   double width;
   m_spSettableMapProj->to_scale_surface_width_in_inches(&width);
   return width;
}

double CMapProjWrapper::to_scale_surface_height_in_inches(void) const
{
   double height;
   m_spSettableMapProj->to_scale_surface_height_in_inches(&height);
   return height;
}

int CMapProjWrapper::surface_scale_percent(void) const
{
   int percent;
   m_spSettableMapProj->surface_scale_percent(&percent);
   return percent;
}

double CMapProjWrapper::surface_scale_denom(void) const
{
   double scale;
   m_spSettableMapProj->surface_scale_denom(&scale);
   return scale;
}

// ActiveMap_TMP implementation
//

ActiveMap_TMP::ActiveMap_TMP(IActiveMapProj *pActiveMap)
{
   m_interface_ptr = pActiveMap;

   pActiveMap->GetSettableMapProj(&m_map);

   // query for the ISettableMapProj2 interface
   m_map2 = m_map;

   get_actual_center(&m_map_spec.center);
   m_map_spec.source = source();
   m_map_spec.scale = scale();
   m_map_spec.series = series();
   m_map_spec.rotation = actual_rotation();
   m_map_spec.zoom_percent = actual_zoom_percent();
   if (requested_zoom_percent() == NATIVE_ZOOM_PERCENT)
      m_map_spec.zoom_percent = NATIVE_ZOOM_PERCENT;
   else if (requested_zoom_percent() == TO_SCALE_ZOOM_PERCENT)
      m_map_spec.zoom_percent = TO_SCALE_ZOOM_PERCENT;
   m_map_spec.projection_type = projection_type();
   m_map_spec.m_dBrightness = m_map->get_brightness();
   m_map->get_contrast(&m_map_spec.m_dContrast, &m_map_spec.m_nContrastMidval);
}

CDC* ActiveMap_TMP::get_CDC()
{
   IGraphicsContextPtr GC;
   m_interface_ptr->GetGraphicsContext(&GC);

   VARIANT_BOOL is_printing;
   GC->IsPrinting(&is_printing);

   HDC hDC, hAttribDC;
   if (SUCCEEDED(GC->raw_GetDC((long *)&hDC, (long *)&hAttribDC)))
   {
      m_dc.Detach();
      m_dc.Attach(hDC);
      m_dc.m_bPrinting = is_printing == VARIANT_TRUE ? TRUE : FALSE;
      m_dc.m_hAttribDC = hAttribDC;
   }

   return &m_dc;
}

ActiveMap_TMP::~ActiveMap_TMP()
{
   m_dc.Detach();
}