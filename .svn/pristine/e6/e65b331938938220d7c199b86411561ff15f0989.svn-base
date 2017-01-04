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

// cdb_tile.h

#ifndef CDB_TILE_H
#define CDB_TILE_H

#include "geo_tool_d.h"
#include "model_obj.h"
#include "model_util.h"
#include "cdb_data.h"
#include "DatasetType.h"

#define CDB_MANMADE 1
#define CDB_NATURAL 2

class C_model_item;
class CUtil;

class C_tile_lod
{
public:
    C_tile_lod();
    ~C_tile_lod();

    int m_lod;
    CList<CGeoCell*, CGeoCell*> m_geocell_list;
};

struct ClassNameAttributes
{
   std::string full_model_path;
   // bounding sphere radius in meters used by 2D rendering of model footprint
   double bsr;
   bool ahgt;
};

struct ModelItem2d
{
   CPoint pt;
   int radius;
   std::string tooltip;
};

struct ImageItem
{
   d_geo_rect_t bounds;
   std::string filename;
};

class C_cdb_tile
{
public:
   C_cdb_tile();
   ~C_cdb_tile();

   CString m_path;
   CString m_root_path;
   CString m_north_str;
   CString m_east_str;

   d_geo_t m_bnd_ll;
   d_geo_t m_bnd_ur;
   
   int draw(ActiveMap* map, int lod);
   int  draw_item(ActiveMap* map, C_cdb_item *item);
   int  draw_oval(ActiveMap* map, C_cdb_item *item);

   int GetImagesInBounds(int lod, cdb::DatasetType dataset_code,
      const char* dataset_name, const d_geo_rect_t& bounds,
      std::vector<ImageItem>& image_list);

   // Create class name attributes mapping
   int ReadClassNameAttributes(const CString& filename,
      cdb::DatasetType dataset_code, int component_selector_1, int lod,
      int u, int r,
      std::map<CString, ClassNameAttributes>& classname_attr) const;

   static int get_geotiff_elevation_data(CString filename, short number_NS_samples, short number_EW_samples,
                                  float *elevation_array);
   static int get_geotiff_elevation_data(CString filename, short number_NS_samples, short number_EW_samples,
                                          double *ll_lat, double *ll_lon, double *ur_lat, double *ur_lon,
                                          float *elevation_array);
   int GetBlockElevationMeters(double ll_lat, double ll_lon, double ur_lat,
      double ur_lon, short number_NS_samples, short number_EW_samples,
      float* elev);

   int GetManmadeItemsInBounds(ActiveMap* map, double meters_per_pix, int lod,
      const d_geo_rect_t& bounds, std::vector<ModelItem2d>& items);
   int GetNaturalItemsInBounds(ActiveMap* map, double meters_per_pix, int lod,
      const d_geo_rect_t& bounds, std::vector<ModelItem2d>& items);

   int parse_cdb_filename(CString filename, double *tile_lat, double *tile_lon, 
                        int *d_num, int *s_num, int *t_num, int *l_num, 
                        int *u_num, int *r_num);

   // Creates a filename for the given dataset and subtile parameters within
   // this tile
   CString CreateFilename(cdb::DatasetType dataset_code,
      const char* dataset_name, int component_selector_1,
      int component_selector_2, int lod, int u, int r);

   // Returns true if data exists in this tile for the given dataset and
   // subtile parameters
   bool DataExists(cdb::DatasetType dataset_code, const char* dataset_name,
      int component_selector_1, int lod, int u, int r);

private:
   int GetBlockElevationMeters(double ll_lat, double ll_lon, double ur_lat,
      double ur_lon, short number_NS_samples, short number_EW_samples,
      float* elev, int current_lod, int current_u, int current_r,
      int target_lod);

   void GetItemsInBounds(ActiveMap* map, double meters_per_pix, int lod,
      int crnt_u, int crnt_r, int max_lod, cdb::DatasetType dataset_code,
      const char* dataset_name, int component_selector_1,
      const d_geo_rect_t& bounds, std::vector<ModelItem2d>& items);

   void GetItems(ActiveMap* map, double meters_per_pix,
      const CString& basename, cdb::DatasetType dataset_code,
      const std::map<CString, ClassNameAttributes>& classname_attr,
      std::vector<ModelItem2d>& items);

   void GetImagesInBounds(int lod, int crnt_u, int crnt_r, int max_lod,
      cdb::DatasetType dataset_code, const char* dataset_name,
      const d_geo_rect_t& bounds, std::vector<ImageItem>& image_list);
};

#endif  // #ifndef CDB_TILE_H