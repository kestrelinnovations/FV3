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

// cdb.h

#ifndef TACTICAL_MODELS_CDB_H
#define TACTICAL_MODELS_CDB_H

#include "model_obj.h"
#include "geo_tool_d.h"
#include "cdb_tile.h"

class C_cdb_tile;

class CCdb : public C_model_obj
{
public:

   CCdb();
    ~CCdb();

   int load(CString filename);
   void clear_tile_list();
   int draw_model(ActiveMap* map, int lod);
   int draw_bounds(ActiveMap* map);
   int draw_icon(ActiveMap* map);
   int draw_ovals(ActiveMap* map);
   int get_min_max_lods(int *minlod, int *maxlod);

   BOOL hit_test(CPoint pt);
   static BOOL is_valid_cdb_dir(CString dir_path);
   static int lod_from_dir(CString dir, CString & lod, CString & uref);
   int dir_to_degrees(CString dir, double *geo);
   static int dir_to_tile_bounds(CString dir, double *ll_lat, double *ll_lon, double *ur_lat, double *ur_lon);
   static int dir_to_tile_bounds_str(CString dir, CString & north_str, CString & east_str);
   static CString get_temp_dir();
   CString get_info();

   int GetBlockElevationMeters(double ll_lat, double ll_lon, double ur_lat,
      double ur_lon, short number_NS_samples, short number_EW_samples,
      float* elev);

   double calc_dot_prod(float Left, float Right, float Up, 
                           float meters_lat_per_pixel, float meters_lon_per_pixel,
                           float light_dir_x, float light_dir_y, float light_dir_z,
                           float exaggeration_factor);
   int display_elevation(ActiveMap* map);
   int display_elevation_rotated(ActiveMap* map);
   int display_images(ActiveMap* map);
   int display_images_rotated(ActiveMap* map);
   int get_imagery(int width, int height, double img_ll_lat, double img_ll_lon, double img_ur_lat, double img_ur_lon, BYTE *img);
   int render_elevation(int width, int height, float *elev, BYTE *img);
   int render_elevation(int width, int height, int min_val, int max_val, float *elev, BYTE *img);
   int get_rgb_image(CString filename, int width, int height, BYTE *img);
   int get_rgb_image(CString filename, int *width, int *height, double *ll_lat, double *ll_lon,
                        double *ur_lat, double *ur_lon, BYTE **img);
   int get_rgb_image(CString filename, int width, int height, IImageLibPtr imagelib, BYTE *img);

   const char *get_class_name() { return "CCdb"; }

   CString m_path;
   CList<C_cdb_tile*, C_cdb_tile*> m_tile_list;
   BOOL m_is_loaded;
   BOOL m_show_models;
   int m_lod;
   int m_minlod;
   int m_maxlod;
   int m_back_display_code;
   float m_elev_exag;

   std::vector<struct ModelItem2d> m_model_items_2d;

   CIconImage  *m_cdb_icon;

   d_geo_t m_bnd_ll;
   d_geo_t m_bnd_ur;
};


#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define PALVERSION   0x300


// ********************************************************************
// ********************************************************************


class CModelDib
{
public:

    BOOL m_initialized;
    int m_image_width, m_image_height, m_num_pixels;
    int m_num_row_bytes, m_num_extra_row_bytes;
    BITMAPINFO *m_dib_bitmapinfo;
    unsigned char *m_dib_data;
    int m_bits_pixel;
    int m_number_unique_colors;
    int m_dib_size;
    BYTE m_lut[1024];
    static BYTE *m_static_lut;
    int *m_histogram;
    BYTE *m_color_index;
    HANDLE m_hdib;
    CPalette m_pal;
    BOOL m_pal_set;
    CBitmap m_bitmap;

    static BITMAPINFO *m_static_dib_bitmapinfo;
    static unsigned char *m_static_dib_data;
    static int m_static_num_row_bytes;
    static int m_static_image_width;
    static int m_static_image_height;
    static int m_static_bits_pixel;




    CModelDib( );
    ~CModelDib( );
    void clear( );
    void erase( unsigned char red, unsigned char green, unsigned char blue );
    int initialize( int width, int height, unsigned char red, unsigned char green,
                    unsigned char blue );
    int display_image( CDC *pDC, int dest_hpix, int dest_vpix );
    int stretch_display_image( CDC *pDC, int dest_min_hpix, int dest_min_vpix,
                              int dest_max_hpix, int dest_max_vpix );
    void DrawBitmap( CDC *pDC, HBITMAP hBitmap, int xDest, int yDest );
    void set_rgb_image( unsigned char *rgb_array );

    int initialize_alpha( int width, int height, BYTE red, BYTE green, BYTE blue );
    void set_rgb_image_alpha( unsigned char *rgb_array);
    void display_alpha(CDC * pDC, int dest_hpix, int dest_vpix, BYTE transcode);

};


#endif  // ifndef TACTICAL_MODELS_CDB_H