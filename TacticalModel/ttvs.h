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

// ttvs.h

#ifndef TACTICAL_MODELS_TTVS_H
#define TACTICAL_MODELS_TTVS_H

#include "model_obj.h"

class C_ttvs_item : public C_model_item
{
public:
   C_ttvs_item(){}
   ~C_ttvs_item(){}

};

class C_ttvs_image_level
{
public:
   C_ttvs_image_level(){}
   ~C_ttvs_image_level(){}

   CString m_dir;
   double m_mosaic_altitude;
   double m_terrain_altitude;
   double m_resolution;
   double m_yaw;
   int m_row;
   int m_col;
   int m_row_cnt;
   int m_col_cnt;
   int m_file_cnt;
   int m_major_ver;
   int m_minor_ver;

};


class C_ttvs_model
{
public:
   C_ttvs_model();
   ~C_ttvs_model();

   CString m_path;
   double m_lat;
   double m_lon;
   double m_elevation;
   BOOL m_display;
   d_geo_t m_bnd_ll;
   d_geo_t m_bnd_ur;
   CRect m_rect;

    CList<C_ttvs_item*, C_ttvs_item*> m_item_list;


   void clear_item_list();
   int create_item_list();
   int process_line(char *line, CString & filename, double *lat, double *lon);
   int  draw(ActiveMap* map, int lod);
   int draw_item(ActiveMap* map, C_ttvs_item *item);
   int  draw_oval(ActiveMap* map, C_ttvs_item *item);
   C_ttvs_item* item_hit_test(CPoint pt);
   int create_database();
   int get_models_from_database(int lod, double ll_lat, double ll_lon, double ur_lat, double ur_lon, 
                                CList<model_item_t*, model_item_t*> & model_list, CString & error_msg);

};

class CTtvs : public C_model_obj
{
public:

   CTtvs();
    ~CTtvs();

   int get_xml_value(CString sline, CString & key, CString & val);
   int load(CString filename);
   int draw_model(ActiveMap* map);
   int draw_icon(ActiveMap* map);
   int draw_bounds(ActiveMap* map);
   BOOL hit_test(CPoint pt);
   void clear_model_list();
   int ReadXML(CString filename);
   CString get_info();
   int create_database();
   int get_models_from_database(int lod, double ll_lat, double ll_lon, double ur_lat, double ur_lon, 
                                   CList<model_item_t*, model_item_t*> & model_list, CString & error_msg);


   const char *get_class_name() { return "CTtvs"; }

   CString m_path;
   CString m_model_path;
   double m_mosaic_def_lat;
   double m_mosaic_def_lon;
   double m_mosaic_def_elev;
   CIconImage  *m_ttvs_icon;
   d_geo_t m_bnd_ll;
   d_geo_t m_bnd_ur;
   d_geo_t m_scrn_ll;
   d_geo_t m_scrn_ur;

   CList<C_ttvs_model*, C_ttvs_model*> m_model_list;
   CList<C_ttvs_image_level*, C_ttvs_image_level*> m_image_level_list;

};

#endif  // ifndef TACTICAL_MODELS_TTVS_H