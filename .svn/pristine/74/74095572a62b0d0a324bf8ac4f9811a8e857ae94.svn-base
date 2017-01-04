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

#ifndef MODEL_REGIONLISTENER_H_
#define MODEL_REGIONLISTENER_H_

#include "resource.h"  // main symbols
#include "FalconView/DisplayElementProvider_Interface.h"
#include "geo_tool_d.h"
#include "DatasetType.h"

class C_cdb_tile;
struct ClassNameAttributes;

namespace cdb_utils
{
   double GetSignificantSize(int lod);
}

class ATL_NO_VTABLE CModelRegionListener :
   public CComObjectRootEx<CComMultiThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IRegionListener, 
   &FalconViewOverlayLib::IID_IRegionListener, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
public:
   CModelRegionListener()
   {
   }

   BEGIN_COM_MAP(CModelRegionListener)
      COM_INTERFACE_ENTRY(FalconViewOverlayLib::IRegionListener)
      COM_INTERFACE_ENTRY(IDispatch)
   END_COM_MAP()

   DECLARE_PROTECT_FINAL_CONSTRUCT()

   HRESULT FinalConstruct()
   {
      return S_OK;
   }

   void Initialize(C_cdb_tile* tile, int lod, int u, int r, double ll_lat,
      double ll_lon, double ur_lat, double ur_lon,
      cdb::DatasetType dataset_type)
   {
      m_tile = tile;
      m_lod = lod;
      m_u = u;
      m_r = r;
      m_ll_lat = ll_lat;
      m_ll_lon = ll_lon;
      m_ur_lat = ur_lat;
      m_ur_lon = ur_lon;
      m_dataset_type = dataset_type;
   }

   void FinalRelease()
   {
   }


   // IRegionListener Methods
   STDMETHOD(raw_RegionActivated)(
      FalconViewOverlayLib::IDisplayElements* display_elements,
      FalconViewOverlayLib::IWorkItems* work_items);

   STDMETHOD(raw_InitializeExclusiveSubregions)(
      FalconViewOverlayLib::IDisplayElements* display_elements,
      FalconViewOverlayLib::IWorkItems* work_items);

private:
   C_cdb_tile* m_tile;
   int m_lod;
   int m_u;
   int m_r;
   double m_ll_lat;
   double m_ll_lon;
   double m_ur_lat;
   double m_ur_lon;
   cdb::DatasetType m_dataset_type;

   void AddSubregions(
      FalconViewOverlayLib::IDisplayElements* display_elements);

   void AddModels(cdb::DatasetType dataset_code, const char* dataset_name,
      int component_selector_1,
      FalconViewOverlayLib::IDisplayElements* display_elements);
   int AddModels(const CString& basename, cdb::DatasetType dataset_code,
      const std::map<CString, ClassNameAttributes>& classname_attr,
      FalconViewOverlayLib::IDisplayElements* display_elements);

   bool DoesDataExist(int lod, int u, int r);
   bool DataExists(cdb::DatasetType dataset_code, const char* dataset_name,
      int component_selector_1, int lod, int u, int r);

   CString GetBestAvailableElevationTile(int lod, int u, int r,
      double* geocell_width, double* geocell_height, d_geo_t* geocell_ul);
};

#endif  // MODEL_REGIONLISTENER_H_
