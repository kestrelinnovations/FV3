// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#include "stdafx.h"
#include "ModelRegionListener.h"

#include "Common/GenericWorkItem.h"
#include "param.h"

#include "model_util.h"
#include "TiledElevationDataset.h"
#include "tm_shp.h"

#include "Common/SafeArray.h"

using namespace cdb;

namespace
{
   const double MIN_LOD_PIXELS = 2;
   const double DISTANCE_TO_LOAD_MODEL_METERS = 30.0e6;
   const double DISTANCE_TO_LOAD_TEXTURE_METERS = 2500.0;

   bool ShowManMadeModels()
   {
      CString tstr = PRM_get_registry_string(
         "TacModel", "TacModelShowManmadeModels", "Y");
      return tstr.Compare("Y") == 0;
   }

   bool ShowNaturalModels()
   {
      CString tstr = PRM_get_registry_string(
         "TacModel", "TacModelShowNaturalModels", "Y");
      return tstr.Compare("Y") == 0;
   }

   CString BuildModelFilename(const CString& filename, const CString& path)
   {
      CString ret = path;

      // If the path's extension ends in ZIP then append the filename
      if (_stricmp(PathFindExtension(ret), ".zip") == 0)
      {
         ret += "\\";
         ret += filename;
      }

      // If the current path doesn't end in FLT, then add it
      if (_stricmp(PathFindExtension(ret), ".flt") != 0)
         ret += ".flt";

      return ret;
   }

   CString s_substitute_texture_name = PRM_get_registry_string("Main",
      "ReadOnlyAppData", "") + "\\textures\\bit_16x16.rgb";
}

namespace cdb_utils
{
   // From Table 2-4: CDB LOD versus Tile and Grid Size
   double s_significant_size[] = {
      1.11319e5,  // CDB -10
      5.56595e4, 2.78298e4, 1.39149e4, 6.95744e3, 3.47872e3, 1.73936e3,
      8.69680e2, 4.34840e2, 2.17420e2,
      1.08710e2,  // CDB LOD 0
      5.43550e1, 2.71775e1, 1.35887e1, 6.79437e0, 3.39719e0, 1.69859e0,
      8.49297e-1, 4.24648e-1, 2.12324e-1, 1.06162e-1, 5.30810e-2, 2.65405e-2,
      1.32703e-2, 6.63513e-3, 3.31756e-3, 1.65878e-3, 8.29391e-4, 4.14696e-4,
      2.07348e-4, 1.03674e-4, 5.18369e-5, 2.59185e-5,
      1.29592e-5  // CDB LOD 23
   };

   double GetSignificantSize(int lod)
   {
      // Signficant size starts at LOD -10
      return s_significant_size[lod + 10];
   }
}

HRESULT CModelRegionListener::raw_RegionActivated(
   FalconViewOverlayLib::IDisplayElements* display_elements,
   FalconViewOverlayLib::IWorkItems* work_items)
{
   // Add imagery from the tile
   if (m_dataset_type == Dataset_Type_Imagery)
   {
      CString filename = m_tile->CreateFilename(m_dataset_type, "Imagery", 1,
         1, m_lod, m_u, m_r);
      filename += ".jp2";

      long image_handle =
         display_elements->CreateImageFromFilename(_bstr_t(filename));

      if (image_handle != -1)
      {
         display_elements->AddGroundOverlay(image_handle, m_ur_lat, m_ll_lon,
            m_ll_lat, m_ur_lon);
      }

      return S_OK;
   }

   // Add models from the tile (from LOD, U, R)
   //
   // Dataset Dir   Selector 1 Selector 2 Dataset Name
   //
   //
   //
   // 100_GSFeature 001        001        Man-made point features
   //                          002        Man-made point features class-level attributes
   //               002        001        Natural point features
   //                          002        Natural point features class-level attributes
   //               003        001        Trees point features
   //                          002        Trees point features class-level attributes
   //
   // 101_GTFeature 001        001        Man-made point features
   //                          002        Man-made point features class-level attribute
   //               002        001        Trees point features
   //                          002        Trees point features class-level attribute
   //
   if (ShowManMadeModels())
   {
      if (m_dataset_type == Dataset_Type_GSFeature)
      {
         AddModels(Dataset_Type_GSFeature, "GSFeature", 1, display_elements);
      }
      else if (m_dataset_type == Dataset_Type_GTFeature)
      {
         AddModels(Dataset_Type_GTFeature, "GTFeature", 1, display_elements);
      }
   }

   if (ShowNaturalModels())
   {
      if (m_dataset_type == Dataset_Type_GSFeature)
      {
         AddModels(Dataset_Type_GSFeature, "GSFeature", 2, display_elements);
         AddModels(Dataset_Type_GSFeature, "GSFeature", 3, display_elements);
      }
      else if (m_dataset_type == Dataset_Type_GTFeature)
      {
         AddModels(Dataset_Type_GTFeature, "GTFeature", 2, display_elements);
      }
   }

   return S_OK;
}

void CModelRegionListener::AddModels(DatasetType dataset_code,
   const char* dataset_name, int component_selector_1,
   FalconViewOverlayLib::IDisplayElements* display_elements)
{
   // Create the DBF filename (uses component selector #2 = 2) and retrieve the
   // class-level attributes for the data set
   //
   CString classref = m_tile->CreateFilename(dataset_code, dataset_name,
      component_selector_1, 2, m_lod, m_u, m_r);
   classref += ".dbf";

   std::map<CString, ClassNameAttributes> classname_attr;
   m_tile->ReadClassNameAttributes(classref, dataset_code,
      component_selector_1, m_lod, m_u, m_r, classname_attr);

   // Create the base filename for the SHP files (component selector #2 = 1)
   CString basename = m_tile->CreateFilename(dataset_code, dataset_name,
      component_selector_1, 1, m_lod, m_u, m_r);
   AddModels(basename, dataset_code, classname_attr, display_elements);
}

CString CModelRegionListener::GetBestAvailableElevationTile(int lod, int u,
   int r, double* geocell_width, double* geocell_height, d_geo_t* geocell_ul)
{
   while (lod > -10)
   {
      CString ret = m_tile->CreateFilename(Dataset_Type_Elevation, "Elevation",
         1, 1, lod, u, r);
      ret += ".tif";
      if (::GetFileAttributes(ret) != INVALID_FILE_ATTRIBUTES)
      {
         if (lod <= 0)
         {
            *geocell_width = m_tile->m_bnd_ur.lon - m_tile->m_bnd_ll.lon;
            *geocell_height = m_tile->m_bnd_ur.lat - m_tile->m_bnd_ll.lat;
            geocell_ul->lat = m_tile->m_bnd_ur.lat;
            geocell_ul->lon = m_tile->m_bnd_ll.lon;
         }
         else
         {
            double geo_spacing_lat =
               (m_tile->m_bnd_ur.lat - m_tile->m_bnd_ll.lat) / (1 << lod);
            double geo_spacing_lon =
               (m_tile->m_bnd_ur.lon - m_tile->m_bnd_ll.lon) / (1 << lod);

            double southern_bound = m_tile->m_bnd_ll.lat +
               geo_spacing_lat * u;
            double northern_bound = southern_bound + geo_spacing_lat;
            double western_bound = m_tile->m_bnd_ll.lon +
               geo_spacing_lon * r;
            double eastern_bound = western_bound + geo_spacing_lon;

            *geocell_width = eastern_bound - western_bound;
            *geocell_height = northern_bound - southern_bound;
            geocell_ul->lat = northern_bound;
            geocell_ul->lon = western_bound;
         }

         return ret;
      }

      --lod;
      u >>= 1;
      r >>= 1;
   }

   return "";
}

int CModelRegionListener::AddModels(const CString& basename,
   DatasetType dataset_code,
   const std::map<CString, ClassNameAttributes>& classname_attr,
   FalconViewOverlayLib::IDisplayElements* display_elements)
{
   C_tm_shp_read shp;

   // check for existence of DBF file
   CDbase dbf;
   CString cdbfname = basename;
   cdbfname += ".dbf";
   int rslt = dbf.OpenDbf(cdbfname, OF_READ);
   if (rslt != DB_NO_ERROR)
      return FAILURE;

   // get field indexes for DBF file
   int cnam_index = dbf.GetFieldDataIndex("CNAM");
   int ao1_index = dbf.GetFieldDataIndex("AO1");
   int scalx_index = dbf.GetFieldDataIndex("SCALx");
   int scaly_index = dbf.GetFieldDataIndex("SCALy");
   int scalz_index = dbf.GetFieldDataIndex("SCALz");
   if (cnam_index < 0 || ao1_index < 0)
   {
      dbf.CloseDbf();
      return FAILURE;
   }

   shp_handle hSHP = shp.SHPOpen(basename, "rb");
   if (hSHP == NULL)
   {
      dbf.CloseDbf();
      return FAILURE;
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
      return FAILURE;
   }

   display_elements->SetAltitudeMode(
      FalconViewOverlayLib::ALTITUDE_MODE_ABSOLUTE);

   // Initialize tiled elevation data set
   double geocell_width, geocell_height;
   d_geo_t geocell_ul;
   CString elv_dataset_name = GetBestAvailableElevationTile(m_lod, m_u, m_r,
      &geocell_width, &geocell_height, &geocell_ul);
   TiledElevationDataset elevation_dataset(elv_dataset_name, geocell_width,
      geocell_height, geocell_ul);

   FalconViewOverlayLib::ModelInstance model_instance;
   memset(&model_instance, 0, sizeof(model_instance));

   // Used for geotypical models
   std::map<std::string, std::vector<FalconViewOverlayLib::ModelInstance>>
      model_instances;

   // For each entity
   for (int i = 0; i < nEntities; i++)
   {
      if (shp.SHPReadObjectLatLonZ(hSHP, i, &model_instance.lat,
         &model_instance.lon, &model_instance.alt_meters) != SUCCESS)
         continue;

      CString cnam;
      model_instance.scale_x = 1.0;
      model_instance.scale_y = 1.0;
      model_instance.scale_z = 1.0;
      shp.get_cdb_data(&dbf, i+1 /* one based */,
         cnam_index, cnam,
         ao1_index, &model_instance.heading,
         scalx_index, &model_instance.scale_x,
         scaly_index, &model_instance.scale_y,
         scalz_index, &model_instance.scale_z);

      if (model_instance.heading < -359.9)
         model_instance.heading = 0.0;

      auto it = classname_attr.find(cnam);
      if (it != classname_attr.end())
      {
         // If AHGT is true, the model's origin is position to the value
         // specified by the Z component (Absolute Terrain Altitude),
         // irrelevant of the terrain elevation dataset. If AHGT is
         // false, the model's origin is positioned to the value
         // specified by the underlying terrain offset by the Z
         // component value.
         if (!it->second.ahgt)
         {
            model_instance.alt_meters +=
               elevation_dataset.GetElevationMeters(model_instance.lat,
               model_instance.lon);
         }

         // Add geospecific features immediately (non-instanced)
         if (dataset_code == Dataset_Type_GSFeature)
         {
            long model_handle = display_elements->CreateModelFromFilename(
               _bstr_t(it->second.full_model_path.c_str()),
               DISTANCE_TO_LOAD_MODEL_METERS,
               DISTANCE_TO_LOAD_TEXTURE_METERS,
               _bstr_t(s_substitute_texture_name),
               it->second.bsr);

            if (model_handle > 0)
            {
               display_elements->AddModel(model_handle, model_instance);
            }
            else
               ASSERT(0);  // validate model name
         }
         // For geotypical features, store them in a list to be sorted
         // and added via the instancing API
         else if (dataset_code == Dataset_Type_GTFeature)
         {
            model_instances[it->second.full_model_path].push_back(
               model_instance);
         }
      }
      else
         ASSERT(0);  // validate class name attributes against dataset
   }

   shp.SHPClose(hSHP);
   dbf.CloseDbf();

   if (!model_instances.empty())
   {
      auto it = model_instances.begin();
      while (it != model_instances.end())
      {
         // Retrieve the number of models for the current model type
         size_t count = it->second.size();

         // Determine whether to add individual models or use the instancing
         // API (if there aren't enough models of a given type then using
         // instancing doesn't make sense).
         if (count < 32)
         {
            long model_handle = display_elements->CreateModelFromFilename(
               _bstr_t(it->first.c_str()), DISTANCE_TO_LOAD_MODEL_METERS,
               DISTANCE_TO_LOAD_TEXTURE_METERS,
               _bstr_t(s_substitute_texture_name),
               cdb_utils::GetSignificantSize(m_lod));

            auto it_vec = it->second.begin();
            while (it_vec != it->second.end())
            {
               display_elements->AddModel(model_handle, *it_vec);
               ++it_vec;
            }
         }
         else
         {
            using namespace FalconViewOverlayLib;
            SafeArray<ModelInstance, VT_RECORD> sa(LIBID_FalconViewOverlayLib,
               __uuidof(ModelInstance), count);

            sa.Append(it->second.data(), count);
            display_elements->AddModels(_bstr_t(it->first.c_str()), &sa);
         }

         ++it;
      }
   }

   return SUCCESS;
}

STDMETHODIMP CModelRegionListener::raw_InitializeExclusiveSubregions(
   FalconViewOverlayLib::IDisplayElements* display_elements,
   FalconViewOverlayLib::IWorkItems* work_items)
{
   int next_lod = m_lod + 1;

   // For nonpositive LODs add a single, subregion
   if (next_lod <= 0)
   {
      CComObject<CModelRegionListener>* region_listener = nullptr;
      CComObject<CModelRegionListener>::CreateInstance(&region_listener);
      region_listener->Initialize(m_tile, next_lod, 0, 0, m_tile->m_bnd_ll.lat,
         m_tile->m_bnd_ll.lon, m_tile->m_bnd_ur.lat, m_tile->m_bnd_ur.lon,
         m_dataset_type);
      region_listener->AddRef();
      display_elements->StartMonitoringForRegion(region_listener,
         m_tile->m_bnd_ur.lat, m_tile->m_bnd_ll.lat, m_tile->m_bnd_ur.lon,
         m_tile->m_bnd_ll.lon, MIN_LOD_PIXELS, -1,
         cdb_utils::GetSignificantSize(next_lod));
      region_listener->Release();
      return S_OK;
   }

   double geo_spacing_lat =
      (m_tile->m_bnd_ur.lat - m_tile->m_bnd_ll.lat) / (1 << next_lod);
   double geo_spacing_lon =
      (m_tile->m_bnd_ur.lon - m_tile->m_bnd_ll.lon) / (1 << next_lod);

   // Add the four subregions
   for (int u_offset = 0; u_offset < 2; ++ u_offset)
   {
      int u = 2 * m_u + u_offset;

      double southern_bound = m_tile->m_bnd_ll.lat +
         geo_spacing_lat * u;
      double northern_bound = southern_bound + geo_spacing_lat;

      for (int r_offset = 0; r_offset < 2; ++r_offset)
      {
         int r = 2 * m_r + r_offset;

         // Only add the subregion if there is available data at the next LOD
         // with the given U and R values
         if (DoesDataExist(next_lod, u, r))
         {
            double western_bound = m_tile->m_bnd_ll.lon +
               geo_spacing_lon * r;
            double eastern_bound = western_bound + geo_spacing_lon;

            CComObject<CModelRegionListener>* region_listener = nullptr;
            CComObject<CModelRegionListener>::CreateInstance(&region_listener);
            region_listener->Initialize(m_tile, next_lod, u, r, southern_bound,
               western_bound, northern_bound, eastern_bound,
               m_dataset_type);
            region_listener->AddRef();
            display_elements->StartMonitoringForRegion(region_listener,
               northern_bound, southern_bound, eastern_bound, western_bound,
               MIN_LOD_PIXELS, -1, cdb_utils::GetSignificantSize(next_lod));
            region_listener->Release();
         }
      }
   }

   return S_OK;
}

bool CModelRegionListener::DoesDataExist(int lod, int u, int r)
{
   if (ShowManMadeModels())
   {
      if (DataExists(Dataset_Type_GSFeature, "GSFeature", 1, lod, u, r) ||
          DataExists(Dataset_Type_GTFeature, "GTFeature", 1, lod, u, r))
      {
         return true;
      }
   }
   if (ShowNaturalModels())
   {
      if (DataExists(Dataset_Type_GSFeature, "GSFeature", 2, lod, u, r) ||
          DataExists(Dataset_Type_GSFeature, "GSFeature", 3, lod, u, r) ||
          DataExists(Dataset_Type_GTFeature, "GTFeature", 2, lod, u, r))
      {
         return true;
      }
   }

   if (DataExists(Dataset_Type_Imagery, "Imagery", 1, lod, u, r))
      return true;

   return false;
}

bool CModelRegionListener::DataExists(DatasetType dataset_code,
   const char* dataset_name, int component_selector_1, int lod, int u, int r)
{
   if (dataset_code != m_dataset_type)
      return false;

   return m_tile->DataExists(dataset_code, dataset_name, component_selector_1,
      lod, u, r);
}