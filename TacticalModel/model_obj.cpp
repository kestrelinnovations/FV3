// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// model_obj.cpp

#include "stdafx.h"
#include "model_obj.h"
#include "model_util.h"

C_image_item::C_image_item()
{
    m_ll_lat = m_ll_lon = m_ur_lat = m_ur_lon = 0.0;
}

C_model_item::C_model_item()
{
   m_lat = m_lon = 0.0;
   m_angle = 0.0;
   m_pt.x = m_pt.y = 0;
   m_is_natural = false;
};

// ******************************************************************************
// **************************************************************************
// **************************************************************************


CGeoCell::CGeoCell()
{
    m_sc1 = m_sc2 = m_lod = m_right_num = m_up_num = m_model_cnt = 0;
    m_ll_lat = m_ll_lon = m_ur_lat = m_ur_lon = 0.0;
}

// **************************************************************************
// **************************************************************************

// Equal operator   
CGeoCell &CGeoCell::operator=(CGeoCell &cell) 
{
   m_sc1 = cell.m_sc1; // component selector 1
   m_sc2 = cell.m_sc2; // component selector 2
   m_ll_lat = cell.m_ll_lat;
   m_ll_lon = cell.m_ll_lon;
   m_ur_lat = cell.m_ur_lat;
   m_ur_lon = cell.m_ur_lon;
   m_lod = cell.m_lod;
   m_lod_str = cell.m_lod_str;
   m_path = cell.m_path;
   m_north_str = cell.m_north_str;
   m_east_str = cell.m_east_str;
   m_uref_str = cell.m_uref_str;
   m_right_num = cell.m_right_num;
   m_up_num = cell.m_up_num;
   m_model_cnt = cell.m_model_cnt;
   m_shp_filename = cell.m_shp_filename;

   return *this;
}
// end of copy operator

// ******************************************************************************

// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************

C_image_buffer::C_image_buffer()
{
    m_image_loaded = false;
    m_width = m_height = 0;
    m_ll_lat = m_ll_lon = m_ur_lat = m_ur_lon =  0.0;
    m_lat_per_pix = m_lon_per_pix = 0.0;
    m_rgb_img = NULL;
    m_elev = NULL;
}

// **************************************************************************
// **************************************************************************

C_image_buffer::~C_image_buffer()
{
    if (m_rgb_img != NULL)
        free(m_rgb_img);
    if (m_elev != NULL)
        free(m_elev);
}

// **************************************************************************
// **************************************************************************

int C_image_buffer::set_image_params(int width, int height, double ll_lat, double ll_lon, double ur_lat, double ur_lon)
{
    m_width = width;
    m_height = height;
    m_ll_lat = ll_lat;
    m_ll_lon = ll_lon;
    m_ur_lat = ur_lat;
    m_ur_lon = ur_lon;

    m_lat_per_pix = (m_ur_lat - m_ll_lat) / (double) m_height;
    m_lon_per_pix = (m_ur_lon - m_ll_lon) / (double) m_width;

    return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int C_image_buffer::xy_to_geo(int x, int y, double *lat, double *lon)
{
    if (!m_image_loaded)
        return FAILURE;

    if ((m_lat_per_pix == 0.0) || (m_lon_per_pix == 0.0))
        return FAILURE;

    *lat = m_ur_lat - ((double) y * m_lat_per_pix);
    *lon = m_ll_lon + ((double) x * m_lon_per_pix);

    return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int C_image_buffer::geo_to_xy( double lat, double lon, int *x, int *y)
{
    if (!m_image_loaded)
        return FAILURE;

    if ((m_lat_per_pix == 0.0) || (m_lon_per_pix == 0.0))
        return FAILURE;

    *x = (int) ((lon - m_ll_lon) / m_lon_per_pix);
    *y = (int) ((m_ur_lat - lat) / m_lat_per_pix);

    return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int C_image_buffer::get_pixel(int x, int y,  BYTE *red, BYTE *grn, BYTE *blu)
{
    int pos;

    if (!m_image_loaded)
        return FAILURE;

    if ((x < 0) || (x >= m_width) || (y < 0) || (y >= m_height))
        return FAILURE;

    pos = ((y * m_width) + x) * 3;
    *red = m_rgb_img[pos];
    *grn = m_rgb_img[pos+1];
    *blu = m_rgb_img[pos+2];

    return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int C_image_buffer::get_elev(int x, int y,  float *elev)
{
    int pos;

    if (!m_image_loaded)
        return FAILURE;

    if ((x < 0) || (x >= m_width) || (y < 0) || (y >= m_height))
        return FAILURE;

    pos = ((y * m_width) + x);
    *elev = m_elev[pos];
 
    return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int C_image_buffer::get_pixel(double lat, double lon,  BYTE *red, BYTE *grn, BYTE *blu)
{
    int tx, ty, rslt;

    if (!m_image_loaded)
        return FAILURE;

//    if ((lat < m_ll_lat) || (lat > m_ur_lat) || (lon < m_ll_lon) || (lon > m_ur_lon))
//        return FAILURE;

    rslt = geo_to_xy(lat, lon, &tx, &ty);
    if (rslt != SUCCESS)
        return rslt;

    rslt = get_pixel(tx, ty, red, grn, blu);

    return rslt;
}

// **************************************************************************
// **************************************************************************

int C_image_buffer::get_elev(double lat, double lon,  float *elev)
{
    int tx, ty, rslt;

    if (!m_image_loaded)
        return FAILURE;

    rslt = geo_to_xy(lat, lon, &tx, &ty);
    if (rslt != SUCCESS)
        return rslt;

    rslt = get_elev(tx, ty, elev);

    return rslt;
}

// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************

C_model_obj::C_model_obj()
{
   for (int k=0; k<4; k++)
   {
      m_geo_lat[k] = -999.0;
      m_geo_lon[k] = -999.0;
   }

   m_loaded = false;
}

// **************************************************************************
// **************************************************************************

C_model_obj::~C_model_obj()
{

}

// **************************************************************************
// **************************************************************************

BOOL C_model_obj::hit_test(CPoint pt)
{
   if (!m_rect.PtInRect(pt))
      return FALSE;

   return FALSE;
}

// **************************************************************************
// **************************************************************************

int C_model_obj::get_center_geo(double *lat, double *lon)
{
   CUtil util;
   double tlat, tlon;

   d_geo_t ll, ur;

   if (!m_loaded)
      return FAILURE;

   ll.lat = m_ll_lat;
   ll.lon = m_ll_lon;
   ur.lat = m_ur_lat;
   ur.lon = m_ur_lon;
   util.compute_center_geo(ll, ur, &tlat, &tlon);
   *lat = tlat;
   *lon = tlon;

   return SUCCESS;
}
