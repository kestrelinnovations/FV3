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



// MapType member functions


#include "stdafx.h"

#include "maps_d.h"
#include "mapx.h"    // MAP_get_degrees_per_pixel_monitor
#include "..\MapView.h"
#include "Common\ComErrorObject.h"
#include "utils.h"
#include "..\MapEngineCOM.h"

// Constructor - if two or more map types from a given MapSource can have
// the same MapScale, then you must use this constructor with a valid 
// MapSeries.
MapType::MapType(const MapSource &source, const MapScale &scale, 
   const MapSeries &series) : 
   m_source(source), m_scale(scale), m_series(series)
{
}
   
// Constructor for MapTypes that do not need a MapSeries
MapType::MapType(const MapSource &source, const MapScale &scale) : 
   m_source(source), m_scale(scale)
{
}

// Gets the degrees per pixel values for display of this map type on a
// monitor.  That is, the dpp values that will be used to define a North Up
// Equal Arc WGS-84 map centered along the given latitude.  This function 
// uses the values returned by get_degrees_per_pixel_data, if they are
// within an acceptable tolerance.  Otherwise acceptable defaults are 
// provided.
int MapType::get_degrees_per_pixel_monitor(degrees_t center_lat,
   degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix)
{
   // get degrees per pixel values for the data
   degrees_t data_lat, data_lon;
   BOOL bLockDegreesPerPixel;
   if (get_degrees_per_pixel_data(center_lat, &data_lat, &data_lon, &bLockDegreesPerPixel) != SUCCESS)
   {
      ERR_report("get_degrees_per_pixel_data() failed.");
      return FAILURE;
   }

   if (bLockDegreesPerPixel)
   {
      *deg_lat_per_pix = data_lat;
      *deg_lon_per_pix = data_lon;
      return SUCCESS;
   }

   // get the nominal degrees per pixel values for this center and scale
   degrees_t default_lat, default_lon;
   if (MAP_get_degrees_per_pixel_monitor(center_lat, get_scale(),
      default_lat, default_lon) != SUCCESS)
   {
      ERR_report("MAP_get_degrees_per_pixel_monitor() failed.");
      return FAILURE;
   }

   // the degrees latitude per pixel value must be within 1% of nominal
   if (data_lat > 1.01 * default_lat)
   {
      data_lat = 1.01 * default_lat;

      // use dpp longitude value unless it is too large
      if (data_lon > 1.01 * default_lon)
         data_lon = 1.01 * default_lon;
   }
   else if (data_lat < 0.99 * default_lat)
   {
      data_lat = 0.99 * default_lat;

      // use dpp longitude value unless it is too small
      if (data_lon < 0.99 * default_lon)
         data_lon = 0.99 * default_lon;
   }

   *deg_lat_per_pix = data_lat;
   *deg_lon_per_pix = data_lon;

   return SUCCESS;
}

// Gets the degrees per pixel values for display of this map type on a
// printer.  That is, the dpp values that will be used to define a North Up
// Equal Arc WGS-84 map centered along the given latitude.  This function 
// uses the values returned by get_degrees_per_pixel_data, if they are
// within an acceptable tolerance.  Otherwise acceptable defaults are 
// provided.
int MapType::get_degrees_per_pixel_printer(degrees_t center_lat,
   degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix)
{
   // get degrees per pixel values for the data
   degrees_t data_lat, data_lon;
   BOOL bLockDegreesPerPixel;
   if (get_degrees_per_pixel_data(center_lat, &data_lat, &data_lon, &bLockDegreesPerPixel) != SUCCESS)
   {
      ERR_report("get_degrees_per_pixel_data() failed.");
      return FAILURE;
   }

   if (bLockDegreesPerPixel)
   {
      *deg_lat_per_pix = data_lat;
      *deg_lon_per_pix = data_lon;

      return SUCCESS;
   }

   // get the nominal degrees per pixel values for this center and scale
   degrees_t default_lat, default_lon;
   if (MAP_get_degrees_per_pixel_monitor(center_lat, get_scale(),
      default_lat, default_lon) != SUCCESS)
   {
      ERR_report("MAP_get_degrees_per_pixel_monitor() failed.");
      return FAILURE;
   }

   // avoid making degrees per pixel values too large so the overlays look good
   if (data_lat > 1.01 * default_lat)
   {
      data_lat = 1.01 * default_lat;

      // use dpp longitude value unless it is too large
      if (data_lon > 1.01 * default_lon)
         data_lon = 1.01 * default_lon;
   }
   // avoid making degrees per pixel values too small so we do not need too
   // much memory
   else if (data_lat < 0.5 * default_lat)
   {
      data_lat = 0.5 * default_lat;

      // use dpp longitude value unless it is too small
      if (data_lon < 0.5 * default_lon)
         data_lon = 0.5 * default_lon;
   }

   *deg_lat_per_pix = data_lat;
   *deg_lon_per_pix = data_lon;

   return SUCCESS;
}

// Get the degrees per pixel values of the data.  That is, the dpp values
// that most closely match your data if it is displayed in a North Up Equal 
// Arc WGS-84 map centered along the given latitude.
int MapType::get_degrees_per_pixel_data(degrees_t center_lat,
      degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix, BOOL *bLockDegreesPerPixel)
{
   int status = FAILURE;
   
   try
   {
      MapView *view = static_cast<MapView *>(UTL_get_frame()->GetActiveView());
      if (view != NULL)
      {
         IDispatchPtr handler;
         view->get_map_engine()->GetMapHandler(m_source, &handler);
         
         IMapRenderPtr render = handler;
         
         if (render)
         {
            VARIANT_BOOL bLock;
            render->GetDegreesPerPixelData(_bstr_t(m_source.get_string()),
               m_scale.GetScale(), m_scale.GetScaleUnits(), _bstr_t(m_series.get_string()),
               center_lat, deg_lat_per_pix, deg_lon_per_pix, &bLock);

            *bLockDegreesPerPixel = bLock == VARIANT_TRUE ? VARIANT_TRUE : VARIANT_FALSE;
            
            status = SUCCESS;
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Get degrees per pixel failed : (%s)", (char *)e.Description());
      ERR_report(msg);
   }

   return status;
}

// Returns true if there is any map data, anywhere in the world, for this
// map type available on the system, false otherwise.
bool MapType::is_data_available()
{
   // Check the region rowset for a map of this type
   try
   {
      IMapGroupsPtr spMapGroups;
      CO_CREATE(spMapGroups, CLSID_MapGroups);

      HRESULT hr = spMapGroups->SelectPositionOnProductScale(0, _bstr_t(m_source.get_string()), m_scale.GetScale(),
         m_scale.GetScaleUnits(), _bstr_t(m_series.get_string()) );

      if (hr == S_OK && spMapGroups->m_DoesDataExist)
         return true;
   }
   catch(_com_error &)
   {
      return false;
   }
   
   return false;
}

// Map type comparison

bool operator ==(const MapType &m1, const MapType &m2)
{
   return (m1.get_source() == m2.get_source() &&
      m1.get_scale() == m2.get_scale() &&
      m1.get_series() == m2.get_series() );
}

bool operator !=(const MapType &m1, const MapType &m2)
{
   return (m1.get_source() != m2.get_source() ||
      m1.get_scale() != m2.get_scale() ||
      m1.get_series() != m2.get_series() );
}
