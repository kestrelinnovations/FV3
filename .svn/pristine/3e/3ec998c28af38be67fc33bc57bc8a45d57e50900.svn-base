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

// model_obj.h

#ifndef TACTICAL_MODELS_MODEL_OBJ
#define TACTICAL_MODELS_MODEL_OBJ

#include "map.h"
#include "common.h"
#include "overlay.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "ovl_d.h"
//#include "utils.h"
//#include "OverlayElements.h"


class CGeoCell
{
public:
   CGeoCell();

   CGeoCell &operator=(CGeoCell &cell); 

   int m_sc1; // component selector 1
   int m_sc2; // component selector 2
   double m_ll_lat;
   double m_ll_lon;
   double m_ur_lat;
   double m_ur_lon;
   int m_lod;
   CString m_lod_str;
   CString m_path;
   CString m_north_str;
   CString m_east_str;
   CString m_uref_str;
   int m_up_num;
   int m_right_num;
   int m_model_cnt;
   CString m_shp_filename;
};

/*
class CIconImage
{
public:
   CIconImage(){}

   static CIconImage* load_images(CString path){return SUCCESS;}

};
*/

//#define MAX_REL_PATH  130

typedef struct
{
   char modl_str[41];
   char filename[101];
   char path[MAX_PATH]; 
} flt_item_t;

typedef struct
{
   double lat;
   double lon;
   double alt_meters;
   double angle;
   short lod;
   char filename[101];
   char path[MAX_PATH];  // relative path
} model_item_t;

typedef struct
{
   double lat;
   double lon;
   double alt_meters;
   double bsr;
   short lod;
   char path[MAX_PATH];  // relative path
} model_3d_item_t;

typedef struct
{
   char sentinel[5];
   short lod;
   unsigned int num_recs;
   double ll_lat;
   double ll_lon;
   double ur_lat;
   double ur_lon;
   char root_path[MAX_PATH];
   char datetime[16];  // YYYYMMDDHHMMSSZ
} model_header_t;


class C_image_buffer
{
public:

    bool m_image_loaded;
    int m_width;
    int m_height;
    double m_ll_lat;
    double m_ll_lon;
    double m_ur_lat;
    double m_ur_lon;
    double m_lat_per_pix;
    double m_lon_per_pix;
    BYTE *m_rgb_img;
    float *m_elev;

    C_image_buffer();
    ~C_image_buffer();
    int set_image_params(int width, int height, double ll_lat, double ll_lon, double ur_lat, double ur_lon);
    int xy_to_geo(int x, int y, double *lat, double *lon);
    int geo_to_xy(double lat, double lon, int *x, int *y);
    int get_pixel(int x, int y,  BYTE *red, BYTE *grn, BYTE *blu);
    int get_elev(int x, int y,  float *elev);
    int get_pixel(double lat, double lon,  BYTE *red, BYTE *grn, BYTE *blu);
    int get_elev(double lat, double lon,  float *elev);

};
    

class C_image_item
{
public:
   C_image_item();
   virtual ~C_image_item(){}

   CString m_path;
   CString m_filename;  // name only without extension
   double m_ll_lat;
   double m_ll_lon;
   double m_ur_lat;
   double m_ur_lon;
};


class C_model_item
{
public:
   C_model_item();
   virtual ~C_model_item(){}

   CString m_id;
   CString m_path;
   CString m_filename;  // name only without extension
   CString m_icon_name;
   double m_lat;
   double m_lon;
   double m_angle;
   double m_bsr;
   CPoint m_pt;
   int m_radius;
   short m_database_code;
   bool m_is_natural;
};

// **************************************************************************

class C_cdb_item : public C_model_item
{
public:
   C_cdb_item();
   virtual ~C_cdb_item(){}

   CString m_lod_str;
   CString m_uref_str;
   CString m_modl_str;
   CString m_cnam;
   short m_rtai;  // relative tactical importance from 0 to 100
   int m_lod_num;  // normally follows m_lod_str but for negative values m_lod_str would be "LC"
   int m_dataset_code;
   int m_cs1;  // component selector 1
   int m_cs2;  // component selector 2

   C_cdb_item &operator=(C_cdb_item &item); 

};


// **************************************************************************

class C_model_obj
{
public:
   C_model_obj();
   ~C_model_obj();

   CString m_filename;
   CString m_tool_tip;
   CString m_help_text;
   CString m_info;

   CString m_class;

   double m_geo_lat[4];
   double m_geo_lon[4];
   double m_ll_lat;
   double m_ll_lon;
   double m_ur_lat;
   double m_ur_lon;
   CRect m_rect;  
   bool m_loaded;

   // Returns the name of this overlay's class.
   virtual const char *get_class_name(){return "Unknown";}
   virtual int draw_model(ActiveMap* map){return FAILURE;}
   virtual int draw_icon(ActiveMap* map){return FAILURE;}
   virtual BOOL hit_test(CPoint pt);

   int get_center_geo(double *lat, double *lon);
};

class C_model_ovl;

class C_model_feature_work_item
{
public:
   C_model_feature_work_item()
   {
   }

   std::vector< C_cdb_item*>  m_item_list;

};

/*
class C_model_feature
{
public:
   C_model_feature()
   {
      m_overlay = NULL;
      m_ll_lat = 0.0;
      m_ll_lon = 0.0;
      m_ur_lat = 0.0;
      m_ur_lon = 0.0;
      m_lod = 0;
      m_min_lod_pixels = 0;
      m_max_lod_pixels = 0;
   }

   C_model_ovl *m_overlay;
   double m_ll_lat;
   double m_ll_lon;
   double m_ur_lat;
   double m_ur_lon;
   int m_lod;
   int m_min_lod_pixels;
   int m_max_lod_pixels;
   std::vector< C_image_item*>  m_image_list;
   CList<C_model_feature_work_item*, C_model_feature_work_item*> m_work_item_list;
   FalconViewOverlayLib::IWorkItems* m_work_items;
};
*/

class C_model_feature2
{
public:
   C_model_feature2()
   {
      m_overlay = NULL;
      m_ll_lat = 0.0;
      m_ll_lon = 0.0;
      m_ur_lat = 0.0;
      m_ur_lon = 0.0;
      m_lod = 0;
      m_min_lod_pixels = 0;
      m_max_lod_pixels = 0;
   }

   C_model_ovl *m_overlay;
   double m_ll_lat;
   double m_ll_lon;
   double m_ur_lat;
   double m_ur_lon;
   int m_lod;
   int m_min_lod_pixels;
   int m_max_lod_pixels;
   std::vector< C_image_item*>  m_image_list;
   CList<C_model_feature_work_item*, C_model_feature_work_item*> m_work_item_list;
   FalconViewOverlayLib::IWorkItems* m_work_items;
};

#endif  // ifndef TACTICAL_MODELS_MODEL_OBJ