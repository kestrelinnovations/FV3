// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// ModelDisplayElementProvider.cpp

#include "stdafx.h"
#include "ModelDisplayElementProvider.h"
#include "ModelRegionListener.h"

#include "FalconView/include/overlay.h"
#include "FalconView/include/param.h"
#include "..\include\TacModel.h"
#include "SafeArray.h"

namespace
{
   const double MIN_LOD_PIXELS = 2;
   const int INITIAL_LOD = -10;  // start at LC10
}

HRESULT ModelDisplayElementProvider::Initialize(C_overlay* overlay,
   FalconViewOverlayLib::IDisplayElements* display_elements,
   FalconViewOverlayLib::IWorkItems* work_items)
{
   C_model_ovl *ovl = static_cast<C_model_ovl*>(overlay);

   // Currently, we only support CDB model datasets
   if (ovl->m_model_type != MODEL_CDB)
      return S_OK;

   CCdb* cdb = static_cast<CCdb *>(ovl->m_cur_obj);


   // Initially, add a separate region, one per tile and one for each
   // dataset type
   cdb::DatasetType types[] = {
      cdb::Dataset_Type_Imagery,
      cdb::Dataset_Type_GSFeature,
      cdb::Dataset_Type_GTFeature
   };

   POSITION pos = cdb->m_tile_list.GetHeadPosition();
   while (pos)
   {
      C_cdb_tile* tile = cdb->m_tile_list.GetNext(pos);

      const int num_types = sizeof(types) /
         sizeof(cdb::DatasetType);
      for (int i=0; i<num_types; ++i)
      {
         CComObject<CModelRegionListener>* region_listener = nullptr;
         CComObject<CModelRegionListener>::CreateInstance(&region_listener);
         region_listener->Initialize(tile, INITIAL_LOD, 0, 0,
            tile->m_bnd_ll.lat, tile->m_bnd_ll.lon, tile->m_bnd_ur.lat,
            tile->m_bnd_ur.lon, types[i]);
         region_listener->AddRef();
         display_elements->StartMonitoringForRegion(region_listener,
            tile->m_bnd_ur.lat, tile->m_bnd_ll.lat, tile->m_bnd_ur.lon,
            tile->m_bnd_ll.lon, MIN_LOD_PIXELS, -1,
            cdb_utils::GetSignificantSize(INITIAL_LOD));
         region_listener->Release();
      }
   }

   return S_OK;
}
