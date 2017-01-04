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

// cdb_tile.cpp

#include "stdafx.h"
#include "cdb_tile.h"

#include "DBASE.H"
#include "tm_shp.h"
#include "geo_tool.h"
#include "fvwutil.h"
#include <io.h>
#include <math.h>
#include "param.h"
#include <fstream>

#include "TiledElevationDataset.h"

namespace
{
   // Used for converting a FACC code to a directory for geotypical models
   std::map<std::string, std::string> s_facc_to_directory_map;

   void InitializeFaccToDirectoryMap()
   {
      CString filename =
         PRM_get_registry_string("Main", "ReadOnlyAppData", "") +
         "\\CDB\\fdd.dat";

      std::ifstream dat_file;
      dat_file.open(filename, std::ifstream::in);
      while (dat_file.good())
      {
         std::string facc_code, directory;
         dat_file >> facc_code >> directory;
         if (!facc_code.empty())
            s_facc_to_directory_map[facc_code] = directory;
      }
   }
};

C_cdb_item::C_cdb_item()
{
   m_rtai = 0;  // relative tactical importance from 0 to 100
   m_lod_num = 0;  // normally follows m_lod_str but for negative values m_lod_str would be "LC"
   m_dataset_code = 0;
   m_cs1 = 0;  // component selector 1
   m_cs2 = 0;  // component selector 2
}

// **************************************************************************
// **************************************************************************

C_tile_lod::C_tile_lod()
{
    m_lod = 0;
}

// **************************************************************************

C_tile_lod::~C_tile_lod()
{
    CGeoCell *cell;

    while (!m_geocell_list.IsEmpty())
    {
        cell = m_geocell_list.RemoveHead();
        delete cell;
    }
}

// **************************************************************************
// **************************************************************************

C_cdb_tile::C_cdb_tile()
{
   m_bnd_ll.lat = m_bnd_ll.lon = m_bnd_ur.lat = m_bnd_ur.lon = 0.0;

   if (s_facc_to_directory_map.empty())
      InitializeFaccToDirectoryMap();
}

// **************************************************************************
// **************************************************************************

C_cdb_tile::~C_cdb_tile()
{
}

#define RET_IF_NOT_OK(result) if (result != SUCCESS) \
   { \
      dbase.CloseDbf(); \
      return FAILURE; \
   }

// Create geospecific class name attributes mapping
int C_cdb_tile::ReadClassNameAttributes(const CString& filename,
   cdb::DatasetType dataset_code, int component_selector_1,
   int base_lod, int base_u, int base_r,
   std::map<CString, ClassNameAttributes>& classname_attr) const
{
   CDbase dbase;
   char buf[FIELD_DATA_T_LEN + 1];
   int rslt = dbase.OpenDbf(filename, OF_READ);
   if (rslt != SUCCESS)
      return FAILURE;

   int ahgt_index = dbase.GetFieldDataIndex("AHGT");
   int bsr_index = dbase.GetFieldDataIndex("BSR");
   int cnam_index = dbase.GetFieldDataIndex("CNAM");
   int facc_index = dbase.GetFieldDataIndex("FACC");
   int fsc_index = dbase.GetFieldDataIndex("FSC");
   int modl_index = dbase.GetFieldDataIndex("MODL");

   // MLOD is only used for Geospecific models
   int mlod_index = dataset_code == cdb::Dataset_Type_GSFeature ?
      dbase.GetFieldDataIndex("MLOD") : -1;

   dbase.GotoRec(1);
   while (rslt == SUCCESS)
   {
      ClassNameAttributes attr;
      CString cnam, facc, fsc, modl;
      int mlod;

      // Per the CDB specification the default if the AHGT field is not
      // present is false
      if (ahgt_index == DB_FIELD_NAME_ERROR)
         attr.ahgt = false;
      else
      {
         CString ahgt_str;
         RET_IF_NOT_OK(dbase.GetTrimmedStringByIndex(ahgt_index, buf,
            &ahgt_str));
         attr.ahgt = false;

         // The specification indicates that valid values for true are in
         // {T, t, Y, y}. However, there are some datasets that use '1' to
         // indicate true.
         if (ahgt_str.GetLength() > 0 && ahgt_str[0] == 'T' ||
            ahgt_str[0] == 't' || ahgt_str[0] == 'Y' || ahgt_str[0] == 'y' ||
            ahgt_str[0] == '1')
         {
            attr.ahgt = true;
         }
      }

      RET_IF_NOT_OK(dbase.GetDoubleByIndex(bsr_index, buf, &attr.bsr));
      RET_IF_NOT_OK(dbase.GetTrimmedStringByIndex(cnam_index, buf, &cnam));
      RET_IF_NOT_OK(dbase.GetTrimmedStringByIndex(facc_index, buf, &facc));
      RET_IF_NOT_OK(dbase.GetTrimmedStringByIndex(fsc_index, buf, &fsc));
      if (dataset_code == cdb::Dataset_Type_GSFeature)
      {
         RET_IF_NOT_OK(dbase.GetIntByIndex(mlod_index, buf, &mlod));
      }
      RET_IF_NOT_OK(dbase.GetTrimmedStringByIndex(modl_index, buf, &modl));

      std::stringstream ss;
      if (dataset_code == cdb::Dataset_Type_GSFeature)
      {
         // Construct geospecific model name from attributes
         //

         // update U, R for this model's MLOD
         int lod = base_lod;
         int u = base_u;
         int r = base_r;
         while (lod > 0 && lod > mlod)
         {
            u >>= 1;
            r >>= 1;
            --lod;
         }

         // use facc, fsc, modl to construct name
         ss << std::setfill('0') << m_path << "\\300_GSModelGeometry\\";
         if (lod < 0)
            ss << "LC";
         else
            ss << "L" << std::setw(2) << lod;

         std::stringstream base_name;
         base_name << m_north_str << m_east_str << "_D300_S" <<
            std::setw(3) << std::setfill('0') << component_selector_1 <<
            "_T001_L" << std::setw(2) << abs(lod) << "_U" << u << "_R" << r;

         ss << "\\U" << u << "\\" << base_name.str() << ".zip\\" <<
            base_name.str() << "_" << facc << "_" << std::setw(3) << fsc <<
            "_" << modl << ".flt";
      }
      else if (dataset_code == cdb::Dataset_Type_GTFeature)
      {
         // Construct geotypical model name from attributes
         //

         // use facc, fsc, modl to construct name
         ss << std::setfill('0') << m_root_path <<
            "GTModel\\500_GTModelGeometry\\";

         auto it = s_facc_to_directory_map.find(std::string(facc));
         if (it != s_facc_to_directory_map.end())
            ss << it->second;

         ss << "\\D500_S001_T001_"  << facc << "_" << std::setw(3) << fsc <<
            "_" << modl << ".flt";
      }

      attr.full_model_path = ss.str().c_str();
      classname_attr.insert(std::make_pair<>(cnam, std::move(attr)));

      rslt = dbase.GotoNext();
   }

   dbase.CloseDbf();

   return SUCCESS;
}

int C_cdb_tile::GetManmadeItemsInBounds(ActiveMap* map, double meters_per_pix,
   int lod, const d_geo_rect_t& bounds, std::vector<ModelItem2d>& items)
{
   GetItemsInBounds(map, meters_per_pix, 0, 0, 0, lod,
      cdb::Dataset_Type_GSFeature, "GSFeature", 1, bounds, items);
   GetItemsInBounds(map, meters_per_pix, 0, 0, 0, lod,
      cdb::Dataset_Type_GTFeature, "GTFeature", 1, bounds, items);

   return SUCCESS;
}

int C_cdb_tile::GetNaturalItemsInBounds(ActiveMap* map, double meters_per_pix,
   int lod, const d_geo_rect_t& bounds, std::vector<ModelItem2d>& items)
{
   GetItemsInBounds(map, meters_per_pix, 0, 0, 0, lod,
      cdb::Dataset_Type_GSFeature, "GSFeature", 2, bounds, items);
   GetItemsInBounds(map, meters_per_pix, 0, 0, 0, lod,
      cdb::Dataset_Type_GSFeature, "GSFeature", 3, bounds, items);
   GetItemsInBounds(map, meters_per_pix, 0, 0, 0, lod,
      cdb::Dataset_Type_GTFeature, "GTFeature", 2, bounds, items);

   return SUCCESS;
}

void C_cdb_tile::GetItemsInBounds(ActiveMap* map, double meters_per_pix,
   int lod, int crnt_u, int crnt_r, int max_lod, cdb::DatasetType dataset_code,
   const char* dataset_name, int component_selector_1,
   const d_geo_rect_t& bounds, std::vector<ModelItem2d>& items)
{
   int next_lod = lod + 1;

   double geo_spacing_lat = (m_bnd_ur.lat - m_bnd_ll.lat) / (1 << next_lod);
   double geo_spacing_lon = (m_bnd_ur.lon - m_bnd_ll.lon) / (1 << next_lod);

   // If one or more subtiles have data and intersect the given bounds, then
   // recursively add the four subtiles
   bool get_items_in_subtiles = false;
   if (next_lod < max_lod)
   {
      for (int u_offset = 0; u_offset < 2; ++ u_offset)
      {
         int u = 2 * crnt_u + u_offset;

         double southern_bound = m_bnd_ll.lat + geo_spacing_lat * u;
         double northern_bound = southern_bound + geo_spacing_lat;

         for (int r_offset = 0; r_offset < 2; ++r_offset)
         {
            int r = 2 * crnt_r + r_offset;

            double western_bound = m_bnd_ll.lon + geo_spacing_lon * r;
            double eastern_bound = western_bound + geo_spacing_lon;

            if (GEO_intersect(bounds.ll.lat, bounds.ll.lon, bounds.ur.lat,
                  bounds.ur.lon, southern_bound, western_bound, northern_bound,
                  eastern_bound) &&
               DataExists(dataset_code, dataset_name, component_selector_1,
                  next_lod, u, r))
            {
               get_items_in_subtiles = true;
               break;
            }
         }
      }
   }

   if (get_items_in_subtiles)
   {
      for (int u_offset = 0; u_offset < 2; ++ u_offset)
      {
         int u = 2 * crnt_u + u_offset;

         double southern_bound = m_bnd_ll.lat + geo_spacing_lat * u;
         double northern_bound = southern_bound + geo_spacing_lat;

         for (int r_offset = 0; r_offset < 2; ++r_offset)
         {
            int r = 2 * crnt_r + r_offset;

            double western_bound = m_bnd_ll.lon + geo_spacing_lon * r;
            double eastern_bound = western_bound + geo_spacing_lon;

            if (GEO_intersect(bounds.ll.lat, bounds.ll.lon, bounds.ur.lat,
               bounds.ur.lon, southern_bound, western_bound, northern_bound,
               eastern_bound))
            {
               GetItemsInBounds(map, meters_per_pix, next_lod, u, r, max_lod,
                  dataset_code, dataset_name, component_selector_1, bounds,
                  items);
            }
         }
      }
   }
   else
   {
      // Create the DBF filename (uses component selector #2 = 2) and retrieve the
      // class-level attributes for the data set
      //
      CString classref = CreateFilename(dataset_code, dataset_name,
         component_selector_1, 2, lod, crnt_u, crnt_r);
      classref += ".dbf";

      std::map<CString, ClassNameAttributes> classname_attr;
      ReadClassNameAttributes(classref, dataset_code,
         component_selector_1, lod, crnt_u, crnt_r, classname_attr);

      // Create the base filename for the SHP files (component selector #2 = 1)
      CString basename = CreateFilename(dataset_code, dataset_name,
         component_selector_1, 1, lod, crnt_u, crnt_r);
      GetItems(map, meters_per_pix, basename, dataset_code, classname_attr,
         items);
   }
}

void C_cdb_tile::GetItems(ActiveMap* map, double meters_per_pix,
   const CString& basename, cdb::DatasetType dataset_code,
   const std::map<CString, ClassNameAttributes>& classname_attr,
   std::vector<ModelItem2d>& items)
{
   C_tm_shp_read shp;

   // check for existence of DBF file
   CDbase dbf;
   CString cdbfname = basename;
   cdbfname += ".dbf";
   int rslt = dbf.OpenDbf(cdbfname, OF_READ);
   if (rslt != DB_NO_ERROR)
      return;

   // get field indexes for DBF file
   int cnam_index = dbf.GetFieldDataIndex("CNAM");
   if (cnam_index < 0)
   {
      dbf.CloseDbf();
      return;
   }

   shp_handle hSHP = shp.SHPOpen(basename, "rb");
   if (hSHP == NULL)
   {
      dbf.CloseDbf();
      return;
   }

   int nEntities, nShapeType;
   double  adfMinBound[4], adfMaxBound[4];
   shp.SHPGetInfo(hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound);

   d_geo_rect_t bnd;
   bnd.ll.lat = adfMinBound[1];
   bnd.ll.lon = adfMinBound[0];
   bnd.ur.lat = adfMaxBound[1];
   bnd.ur.lon = adfMaxBound[0];

   // check the coordinates of the boundaries read from the shape file
   if ((bnd.ll.lat < -180.0) || (bnd.ll.lat > 180.0) ||
      (bnd.ur.lat < -180.0) || (bnd.ur.lat > 180.0) ||
      (bnd.ll.lon < -360.0) || (bnd.ll.lon > 360.0) ||
      (bnd.ur.lon < -360.0) || (bnd.ur.lon > 360.0))
   {
      return;
   }

   // For each entity
   for (int i = 0; i < nEntities; i++)
   {
      double lat, lon, z;
      if (shp.SHPReadObjectLatLonZ(hSHP, i, &lat, &lon, &z) != SUCCESS)
         continue;

      ModelItem2d item;

      // Set the 2d items screen coordinates
      int x, y;
      map->geo_to_surface(lat, lon, &x, &y);
      item.pt = CPoint(x, y);

      CString cnam;
      shp.get_cnam(&dbf, i+1 /* one based */, cnam_index, cnam);

      // Lookup the class name attributes associated with the model
      auto it = classname_attr.find(cnam);
      if (it != classname_attr.end())
      {
         // Set the tooltip based on the full model path
         size_t pos1 = it->second.full_model_path.rfind('\\');
         size_t pos2 = it->second.full_model_path.rfind('.');
         item.tooltip = it->second.full_model_path.substr(
            pos1 + 1, pos2 - pos1 - 1);

         // Set the radius in pixels based on the bounding sphere radius
         item.radius = static_cast<int>(it->second.bsr / meters_per_pix);
         if (item.radius > 20)
            item.radius = 20;
         if (item.radius < 2)
            item.radius = 2;

         items.push_back(std::move(item));
      }
   }

   shp.SHPClose(hSHP);
   dbf.CloseDbf();
}

int C_cdb_tile::parse_cdb_filename(CString filename, double *tile_lat, double *tile_lon, 
                                    int *d_num, int *s_num, int *t_num, int *l_num, 
                                    int *u_num, int *r_num)
{
    CUtil util;
    char north, east, ch;
    CString fname, tstr;
    int pos, ndx;

    if (filename.GetLength() < 35)
        return FAILURE;

    fname = filename;

    // check for included path
    pos = fname.Find('\\');
    if (pos >= 0)
        fname = util.extract_filename(filename);

    north = fname.GetAt(0);
    east = fname.GetAt(3);

    tstr = fname.Mid(1, 2);
    *tile_lat = (double) atoi(tstr);
    if (north != 'N')
        *tile_lat = -(*tile_lat);
    tstr = fname.Mid(4,3);
    *tile_lon = (double) atoi(tstr);
    if (east != 'E')
        *tile_lon = -(*tile_lon);

    tstr = fname.Mid(9, 3);
    *d_num = atoi(tstr);
    tstr = fname.Mid(14, 3);
    *s_num = atoi(tstr);
    tstr = fname.Mid(19, 3);
    *t_num = atoi(tstr);
    ch = fname.GetAt(24);
    if (ch == 'C')
    {
        tstr = fname.Mid(25, 2);
        *l_num = -atoi(tstr);
        tstr = fname.Mid(29, 5);
        pos = tstr.Find('_');
        tstr = tstr.Left(pos);
        *u_num = atoi(tstr);
        ndx = 29 + pos + 1;
        tstr = fname.Mid(ndx, 5);
        pos = tstr.Find('.');
        tstr = tstr.Left(pos);
        tstr = tstr.Right(tstr.GetLength()-1);
        *r_num = atoi(tstr);
    }
    else
    {
        tstr = fname.Mid(24, 3);
        *l_num = atoi(tstr);
        tstr = fname.Mid(28, 5);
        pos = tstr.Find('_');
        tstr = tstr.Left(pos);
        *u_num = atoi(tstr);
        ndx = 28 + pos + 1;
        tstr = fname.Mid(ndx, 5);
        pos = tstr.Find('.');
        tstr = tstr.Left(pos);
        tstr = tstr.Right(tstr.GetLength()-1);
        *r_num = atoi(tstr);
    }

    // todo add range checking of numbers

    return SUCCESS;
}

int C_cdb_tile::GetImagesInBounds(int lod, cdb::DatasetType dataset_code,
   const char* dataset_name, const d_geo_rect_t& bounds,
    std::vector<ImageItem>& image_list)
{
   GetImagesInBounds(0, 0, 0, lod, dataset_code, dataset_name, bounds,
      image_list);
   return SUCCESS;
}

void C_cdb_tile::GetImagesInBounds(int lod, int crnt_u, int crnt_r,
   int max_lod, cdb::DatasetType dataset_code, const char* dataset_name,
   const d_geo_rect_t& bounds, std::vector<ImageItem>& image_list)
{
   int next_lod = lod + 1;

   double geo_spacing_lat = (m_bnd_ur.lat - m_bnd_ll.lat) / (1 << next_lod);
   double geo_spacing_lon = (m_bnd_ur.lon - m_bnd_ll.lon) / (1 << next_lod);

   // If one or more subtiles have data and intersect the given bounds, then
   // recursively add the four subtiles
   bool get_items_in_subtiles = false;
   if (next_lod < max_lod)
   {
      for (int u_offset = 0; u_offset < 2; ++ u_offset)
      {
         int u = 2 * crnt_u + u_offset;

         double southern_bound = m_bnd_ll.lat + geo_spacing_lat * u;
         double northern_bound = southern_bound + geo_spacing_lat;

         for (int r_offset = 0; r_offset < 2; ++r_offset)
         {
            int r = 2 * crnt_r + r_offset;

            double western_bound = m_bnd_ll.lon + geo_spacing_lon * r;
            double eastern_bound = western_bound + geo_spacing_lon;

            if (GEO_intersect(bounds.ll.lat, bounds.ll.lon, bounds.ur.lat,
                  bounds.ur.lon, southern_bound, western_bound, northern_bound,
                  eastern_bound) &&
               DataExists(dataset_code, dataset_name, 1, next_lod, u, r))
            {
               get_items_in_subtiles = true;
               break;
            }
         }
      }
   }

   if (get_items_in_subtiles)
   {
      for (int u_offset = 0; u_offset < 2; ++ u_offset)
      {
         int u = 2 * crnt_u + u_offset;

         double southern_bound = m_bnd_ll.lat + geo_spacing_lat * u;
         double northern_bound = southern_bound + geo_spacing_lat;

         for (int r_offset = 0; r_offset < 2; ++r_offset)
         {
            int r = 2 * crnt_r + r_offset;

            double western_bound = m_bnd_ll.lon + geo_spacing_lon * r;
            double eastern_bound = western_bound + geo_spacing_lon;

            if (GEO_intersect(bounds.ll.lat, bounds.ll.lon, bounds.ur.lat,
               bounds.ur.lon, southern_bound, western_bound, northern_bound,
               eastern_bound))
            {
               GetImagesInBounds(next_lod, u, r, max_lod, dataset_code,
                  dataset_name, bounds, image_list);
            }
         }
      }
   }
   else
   {
      double geo_spacing_lat = (m_bnd_ur.lat - m_bnd_ll.lat) / (1 << lod);
      double geo_spacing_lon = (m_bnd_ur.lon - m_bnd_ll.lon) / (1 << lod);

      ImageItem item;
      item.bounds.ll.lat = m_bnd_ll.lat + geo_spacing_lat * crnt_u;
      item.bounds.ur.lat = item.bounds.ll.lat + geo_spacing_lat;
      item.bounds.ll.lon = m_bnd_ll.lon + geo_spacing_lon * crnt_r;
      item.bounds.ur.lon = item.bounds.ll.lon + geo_spacing_lon;
      item.filename = CreateFilename(dataset_code, dataset_name, 1, 1, lod,
         crnt_u, crnt_r);
      image_list.push_back(std::move(item));
   }
}

int  C_cdb_tile::draw_oval(ActiveMap* map, C_cdb_item *item)
{
   CDC* dc = map->get_CDC();
   CBrush brush, *oldbrush;
   CPen pen, *oldpen;
   int tx, ty;

   map->geo_to_surface(item->m_lat, item->m_lon, &tx, &ty);

   item->m_pt.x = tx;
   item->m_pt.y = ty;

   pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
   if (item->m_dataset_code == 101)
      brush.CreateSolidBrush(RGB(192, 192, 0));
   else
      brush.CreateSolidBrush(RGB(192, 0, 192));
   oldpen = (CPen*) dc->SelectObject(&pen);
   oldbrush = (CBrush*) dc->SelectObject(&brush);
   dc->Ellipse(tx-5, ty-5, tx+5, ty+5);

   dc->SelectObject(oldpen);
   dc->SelectObject(oldbrush);
   pen.DeleteObject();
   brush.DeleteObject();

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int  C_cdb_tile::draw_item(ActiveMap* map, C_cdb_item *item)
{
   if ( map->projection_type() == GENERAL_PERSPECTIVE_PROJECTION )
   {
//      draw_model(map, item);
   }
   else
      draw_oval(map, item);

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int  C_cdb_tile::draw(ActiveMap* map, int lod)
{

   return SUCCESS;
}

int C_cdb_tile::get_geotiff_elevation_data(CString filename, short number_NS_samples, short number_EW_samples,
                                          float *elevation_array)
{
    int err, status;
    IImageLibPtr imagelib;

    try
    {
        CComBSTR berr_msg;

        if (imagelib == NULL)
           CO_CREATE(imagelib, CLSID_ImageLib);

        int size, width, height;
        double ullat, ullon, urlat, urlon, lrlat, lrlon, lllat, lllon;

        size = number_EW_samples * number_NS_samples;

        status = imagelib->load(bstr_t(filename), &width, &height, &err, &berr_msg);
        if (err != 0)
            return FAILURE;

        status = imagelib->get_corner_coords(&ullat, &ullon, &urlat, &urlon, &lrlat, &lrlon, &lllat, &lllon, &err);

        status = imagelib->get_elevation_grid(number_EW_samples, number_NS_samples,
                            ullat, ullon, lrlat, lrlon, 0,
                            elevation_array, &err, &berr_msg );

        if (err != 0)
            return FAILURE;

        return SUCCESS;
    }
    catch(...)
    {
        return FAILURE;
    }

}
// end of get_geotiff_elevation_data

int C_cdb_tile::get_geotiff_elevation_data(CString filename, short number_NS_samples, short number_EW_samples,
                                          double *ll_lat, double *ll_lon, double *ur_lat, double *ur_lon,
                                          float *elevation_array)
{
    int err, status;
    IImageLibPtr imagelib;

    try
    {
        CComBSTR berr_msg;

        if (imagelib == NULL)
           CO_CREATE(imagelib, CLSID_ImageLib);

        int size, width, height;
        double ullat, ullon, lrlat, lrlon;

        size = number_EW_samples * number_NS_samples;

        status = imagelib->load(bstr_t(filename), &width, &height, &err, &berr_msg);
        if (err != 0)
            return FAILURE;

        status = imagelib->get_corner_coords(&ullat, &ullon, ur_lat, ur_lon, &lrlat, &lrlon, ll_lat, ll_lon, &err);

        status = imagelib->get_elevation_grid(number_EW_samples, number_NS_samples,
                            ullat, ullon, lrlat, lrlon, 0,
                            elevation_array, &err, &berr_msg );

        if (err != 0)
            return FAILURE;

        return SUCCESS;
    }
    catch(...)
    {
        return FAILURE;
    }

}
// end of get_geotiff_elevation_data

// Helper method to construct a dataset file name, without extension, given
// the dataset and component selectors
CString C_cdb_tile::CreateFilename(cdb::DatasetType dataset_code,
   const char* dataset_name, int component_selector_1,
   int component_selector_2, int lod, int u, int r)
{
   std::stringstream ret;
   ret << std::setfill('0') << m_path << "\\" << std::setw(3) <<
      dataset_code << "_" << dataset_name << "\\";

   if (lod < 0)
      ret << "LC";
   else
      ret << "L" << std::setw(2) << lod;

   ret << "\\U" << std::setw(0) << u << "\\" << m_north_str <<
      m_east_str << "_D" << std::setw(3) << dataset_code << "_S" <<
      std::setw(3) << component_selector_1 << "_T" << std::setw(3) <<
      component_selector_2 << "_L";

   if (lod < 0)
      ret << "C";

   ret << std::setw(2) << abs(lod) << "_U" << std::setw(0) << u << "_R" << r;

   return ret.str().c_str();
}

bool C_cdb_tile::DataExists(cdb::DatasetType dataset_code,
   const char* dataset_name, int component_selector_1, int lod, int u, int r)
{
   CString filename = CreateFilename(dataset_code, dataset_name,
      component_selector_1, 1, lod, u, r);
   switch (dataset_code)
   {
   case cdb::Dataset_Type_Imagery : filename += ".jp2"; break;
   case cdb::Dataset_Type_Elevation: filename += ".tif"; break;
   default:
      filename += ".dbf";
   }
   return ::GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES;
}

int C_cdb_tile::GetBlockElevationMeters(double ll_lat, double ll_lon,
   double ur_lat, double ur_lon, short number_NS_samples,
   short number_EW_samples, float* elev)
{
   // An elevation tile of LOD -10 starts at 1 degree per pixel latitude.
   // Compute the target LOD for the given parameters
   double dpp_lat = (ur_lat - ll_lat) / number_NS_samples;
   int target_lod = static_cast<int>(fabs(log(dpp_lat)/log(2.0)) - 10 + 0.5);
   target_lod = max(-10, target_lod);

   // Recurse through available elevation data, up to the target level of
   // detail, starting with LOD = -10
   return GetBlockElevationMeters(ll_lat, ll_lon, ur_lat, ur_lon,
      number_NS_samples, number_EW_samples, elev, -10, 0, 0, target_lod);
}

int C_cdb_tile::GetBlockElevationMeters(double ll_lat, double ll_lon,
   double ur_lat, double ur_lon, short number_NS_samples,
   short number_EW_samples, float* elev, int current_lod, int current_u,
   int current_r, int target_lod)
{
   if (current_lod < target_lod)
   {
      // Determine if the next LOD for the current tile can fully cover the
      // tile. If not, we will fill the output buffer using this LOD.
      int next_lod = current_lod + 1;
      if (next_lod <= 0)
      {
         // If an elevation data tile exists for the next LOD then recurse with
         // next_lod, u, r. We know that the full tile must intersect the
         // output buffer for negative LODs (since the bounds match the
         // full tile bounds which we've already intersected against).
         if (DataExists(cdb::Dataset_Type_Elevation, "Elevation", 1, next_lod,
            0, 0))
         {
            return GetBlockElevationMeters(ll_lat, ll_lon, ur_lat, ur_lon,
               number_NS_samples, number_EW_samples, elev, next_lod, 0, 0,
               target_lod);
         }
      }
      else
      {
         double geo_spacing_lat =
            (m_bnd_ur.lat - m_bnd_ll.lat) / (1 << next_lod);
         double geo_spacing_lon =
            (m_bnd_ur.lon - m_bnd_ll.lon) / (1 << next_lod);

         bool subdivide = true;
         for (int u_offset = 0; u_offset < 2; ++u_offset)
         {
            int u = 2 * current_u + u_offset;

            double southern_bound = m_bnd_ll.lat + geo_spacing_lat * u;
            double northern_bound = southern_bound + geo_spacing_lat;

            for (int r_offset = 0; r_offset < 2; ++r_offset)
            {
               int r = 2 * current_r + r_offset;

               double western_bound = m_bnd_ll.lon + geo_spacing_lon * r;
               double eastern_bound = western_bound + geo_spacing_lon;

               // If the subtile doesn't intersect the requested bounds then
               // it doesn't matter if there is data available or not
               if (GEO_intersect(southern_bound, western_bound, northern_bound,
                  eastern_bound, ll_lat, ll_lon, ur_lat, ur_lon) &&
                  !DataExists(cdb::Dataset_Type_Elevation, "Elevation", 1,
                  next_lod, u, r))
               {
                  // At least one subtile intersects the output buffer but
                  // does not have data available. We will fill in the buffer
                  // with tiles from current LOD.
                  subdivide = false;
               }
            }
         }
         if (subdivide)
         {
            for (int u_offset = 0; u_offset < 2; ++u_offset)
            {
               int u = 2 * current_u + u_offset;
               double southern_bound = m_bnd_ll.lat + geo_spacing_lat * u;
               double northern_bound = southern_bound + geo_spacing_lat;

               for (int r_offset = 0; r_offset < 2; ++r_offset)
               {
                  int r = 2 * current_r + r_offset;
                  double western_bound = m_bnd_ll.lon + geo_spacing_lon * r;
                  double eastern_bound = western_bound + geo_spacing_lon;

                  if (GEO_intersect(southern_bound, western_bound,
                     northern_bound, eastern_bound, ll_lat, ll_lon, ur_lat,
                     ur_lon))
                  {
                     GetBlockElevationMeters(ll_lat, ll_lon, ur_lat, ur_lon,
                        number_NS_samples, number_EW_samples, elev, next_lod,
                        u, r, target_lod);
                  }
               }
            }
            // No need to fill the data at current LOD since all subtiles of
            // the next LOD have available data
            return SUCCESS;
         }
      }
   }

   // Compute the bounds of the current tile
   //
   double geo_spacing_lat = current_lod < 0 ? m_bnd_ur.lat - m_bnd_ll.lat :
      (m_bnd_ur.lat - m_bnd_ll.lat) / (1 << current_lod);
   double geo_spacing_lon = current_lod < 0 ? m_bnd_ur.lon - m_bnd_ll.lon :
      (m_bnd_ur.lon - m_bnd_ll.lon) / (1 << current_lod);

   double southern_bound = m_bnd_ll.lat + geo_spacing_lat * current_u;
   double northern_bound = southern_bound + geo_spacing_lat;
   double western_bound = m_bnd_ll.lon + geo_spacing_lon * current_r;
   double eastern_bound = western_bound + geo_spacing_lon;

   // Compute intersecting rectangle of output buffer and current subtile (in
   // the output buffer's coordinate system).
   //
   double dpp_lat = (ur_lat - ll_lat) / (number_NS_samples - 1);
   double dpp_lon = (ur_lon - ll_lon) / (number_EW_samples - 1);

   int nw_y = static_cast<int>((ur_lat - northern_bound) / dpp_lat);
   nw_y = max(0, nw_y);

   int nw_x = static_cast<int>((western_bound - ll_lon) / dpp_lon);
   nw_x = max(0, nw_x);

   int se_y = static_cast<int>((ur_lat - southern_bound) / dpp_lat);
   se_y = min(number_NS_samples - 1, se_y);

   int se_x = static_cast<int>((eastern_bound - ll_lon) / dpp_lon);
   se_x = min(number_EW_samples - 1, se_x);

   // Construct the TiledElevationDataset for performing queries
   //
   CString filename = CreateFilename(cdb::Dataset_Type_Elevation, "Elevation",
      1, 1, current_lod, current_u, current_r);
   filename += ".tif";

   d_geo_t geocell_ul = { northern_bound, western_bound };
   TiledElevationDataset ted(filename, geo_spacing_lon, geo_spacing_lat,
      geocell_ul);

   // Fill the output buffer (south to north, west to east)
   float* crnt = elev + nw_x +number_EW_samples*(number_NS_samples - nw_y - 1);
   double lat = ur_lat - dpp_lat * nw_y;
   for (int y=nw_y; y<=se_y; ++y)
   {
      double lon = ll_lon + dpp_lon * nw_x;
      for (int x=nw_x; x<=se_x; ++x)
      {
         *crnt = ted.GetElevationMeters(lat, lon);
         ++crnt;
         lon += dpp_lon;
      }
      lat -= dpp_lat;

      // Move the pointer to the start of the previous row in the output buffer
      // by moving the pointer back to where it was at the beginning of the
      // previous loop (se_x-nw_x+1) and moving to the previous row
      // (number_EW_samples).
      crnt += -number_EW_samples - (se_x - nw_x + 1);
   }

   return SUCCESS;
}
