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

// ILayer2.cpp
//

#include "StdAfx.h"
#include "IMapDataManager.h"
#include "err.h"        // for ERR_report
#include "maps.h"			// for MAP_get_map_list
#include "Common\ComErrorObject.h"

IMPLEMENT_DYNCREATE(MapDataManager, CCmdTarget)

IMPLEMENT_OLECREATE(MapDataManager, "FalconView.MapDataManger", 0xABBE468D, 0x4404, 0x4B47, 
                   0xA5, 0x81, 0xCD, 0xB1, 0x8B, 0x4B, 0x62, 0xFC)


MapDataManager::MapDataManager()
{
	EnableAutomation();

	//m_coverage_list = NULL;
	m_next_query_id = 1;
}

MapDataManager::~MapDataManager()
{
	//if (m_coverage_list != NULL)
		//delete m_coverage_list;
}

void MapDataManager::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(MapDataManager, CCmdTarget)
	//{{AFX_MSG_MAP(MapDataManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(MapDataManager, CCmdTarget)
	//{{AFX_DISPATCH_MAP(MapDataManager)
   DISP_FUNCTION(MapDataManager, "GetFramesInRect",GetFramesInRect, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_PI4 VTS_PI4)
   DISP_FUNCTION(MapDataManager, "GetFrameInfo", GetFrameInfo, VT_I4, VTS_I4 VTS_I4 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PBSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMapDataManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IMapDataManager =
{ 0xE05E95CB, 0xA139, 0x4F97, { 0xA0, 0x35, 0x0C, 0x8D, 0xD1, 0x58, 0x05, 0x3C } };

BEGIN_INTERFACE_MAP(MapDataManager, CCmdTarget)
	INTERFACE_PART(MapDataManager, IID_IMapDataManager, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MapDataManager message handlers


long MapDataManager::GetFramesInRect(double ll_lat, double ll_lon, double ur_lat,
				double ur_lon, int map_handle, int *query_id, int *num_elements)
{
	// create a map from the given handle and generate load the coverage
	// list with the given region.  The coverage list will be stored in a 
	// member variable used in the GetFrame

   m_vectorCoverageRects.clear();

   try
   {
      ICoverageRowsetPtr smpCoverageRowset;
      CO_CREATE(smpCoverageRowset, __uuidof(MDSUtilCoverage));

      IMapGroupsPtr smpMapGroups;
      CO_CREATE(smpMapGroups, _uuidof(MapGroups));

      smpMapGroups->SelectByIdentity(map_handle);

      smpCoverageRowset->Initialize(smpMapGroups->m_MapHandlerName);

      int nCurrentDS = -1;
      _bstr_t bstrCurrentDSPath;

      HRESULT hr = smpCoverageRowset->SelectByGeoRect(smpMapGroups->m_MapSeriesIdentity, ll_lat, ll_lon, ur_lat, ur_lon);
      while (hr == S_OK)
      {
         COVERAGE_RECT coverageRect;

         if (nCurrentDS != smpCoverageRowset->m_DataSourceId)
         {
            nCurrentDS = smpCoverageRowset->m_DataSourceId;
            bstrCurrentDSPath = smpCoverageRowset->GetLocalOrRemoteFolderName(nCurrentDS);
         }

         coverageRect.bstrFilename = bstrCurrentDSPath + "\\" + smpCoverageRowset->m_LocationSpec;
         smpCoverageRowset->GetBounds(&coverageRect.ll_lat, &coverageRect.ll_lon,
            &coverageRect.ur_lat, &coverageRect.ur_lon);

         m_vectorCoverageRects.push_back(coverageRect);

         hr = smpCoverageRowset->MoveNext();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("GetFramesInRect failed: %s", e.Description());
      ERR_report(msg);
   }

   if (m_vectorCoverageRects.size() == 0)
	{
      *query_id = -1;
      *num_elements = 0;
      return FAILURE;   // no data available for the given map handle
	}
  
	*query_id = m_next_query_id++;
	*num_elements = m_vectorCoverageRects.size();

	return SUCCESS;
}

long MapDataManager::GetFrameInfo(int query_id, int element_num, double *ll_lat, 
				double *ll_lon, double *ur_lat, double *ur_lon, BSTR *filename)
{
	if (query_id != (m_next_query_id - 1))
	{
		ERR_report("IMapDataManager::GetFrameInfo - query id invalid");
		return FAILURE;
	}

   if (element_num < 0 || (size_t)element_num >= m_vectorCoverageRects.size())
   {
      ERR_report("IMapDataManager::GetFrameInfo - element_num out of bounds");
      return FAILURE;
   }

   COVERAGE_RECT &coverageRect = m_vectorCoverageRects[element_num];

	*ll_lat = coverageRect.ll_lat;
	*ll_lon = coverageRect.ll_lon;
	*ur_lat = coverageRect.ur_lat;
	*ur_lon = coverageRect.ur_lon;

   *filename = coverageRect.bstrFilename.copy();

	return SUCCESS;
}
