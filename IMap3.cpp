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

// IMap3.cpp
//

#include "StdAfx.h"
#include "IMap3.h"

#include "err.h"
#include "getobjpr.h"
#include "mapx.h"
#include "MapEngineCOM.h"

#include "FalconView/include/maps.h"

IMPLEMENT_DYNCREATE(Map3, CCmdTarget)

IMPLEMENT_OLECREATE(Map3, "FalconView.Map3", 0x5C8FA605, 0xA4A1, 0x41cd, 
                    0x80, 0x07, 0x63, 0x0C, 0xFB, 0x14, 0xF4, 0xBE)

Map3::Map3()
{
   EnableAutomation();
}

Map3::~Map3()
{
}

void Map3::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(Map3, CCmdTarget)
   //{{AFX_MSG_MAP(Map3)
   // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Map3, CCmdTarget)
   //{{AFX_DISPATCH_MAP(Map2)
   DISP_FUNCTION(Map3, "GetMapDisplayEx", GetMapDisplayEx, VT_I4, VTS_PBSTR VTS_PR8 VTS_PI4 VTS_PBSTR VTS_PR8 VTS_PR8 VTS_PR8 VTS_PI4 VTS_PI4)
   DISP_FUNCTION(Map3, "SetMapDisplayEx", SetMapDisplayEx, VT_I4, VTS_BSTR VTS_R8 VTS_I4 VTS_BSTR VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_I4)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMap2 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IMap3 =
{ 0x3570F03C, 0x421F, 0x4f5a, { 0x8E, 0x45, 0xBD, 0xCF, 0xC4, 0x1A, 0x2D, 0x24 } };

BEGIN_INTERFACE_MAP(Map3, CCmdTarget)
   INTERFACE_PART(Map3, IID_IMap3, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Map message handlers

long Map3::GetMapDisplayEx(BSTR* pMapSource, double* pScale, int* pScaleUnits,
   BSTR* pMapSeries, double* pCenterLat, double* pCenterLon, double* pRotation,
   int* pZoomPercent, int* pProjection)
{
   MapView* map_view = fvw_get_view();
   if (map_view == NULL)
      return FAILURE;

   const MapType& map_type = map_view->GetMapType();
   const MapProjectionParams& map_proj_params = map_view->GetMapProjParams();

   *pMapSource = _bstr_t(map_type.get_source_string()).Detach();
   MapEngineCOM::ConvertScale(map_type.get_scale(),
      pScale, reinterpret_cast<MapScaleUnitsEnum *>(pScaleUnits));
   *pMapSeries = _bstr_t(map_type.get_series_string()).Detach();
   *pCenterLat = map_proj_params.center.lat;
   *pCenterLon = map_proj_params.center.lon;
   *pRotation = map_proj_params.rotation;
   *pZoomPercent = map_proj_params.zoom_percent;
   *pProjection = map_proj_params.type;

   return SUCCESS;
}

long Map3::SetMapDisplayEx(LPCTSTR strMapSource, double dScale, int nScaleUnits,
   LPCTSTR strMapSeries, double dCenterLat, double dCenterLon, double dRotation,
   int nZoomPercent, int nProjection)
{
   MapView* map_view = fvw_get_view();
   if (map_view == NULL)
   {
      ERR_report("Map3::SetMapDisplayEx - unable to get current view");
      return FAILURE;
   }

   MapCategory category = MAP_get_category(strMapSource);
   MapType map_type(strMapSource, MapScale(dScale,
      static_cast<MapScaleUnitsEnum>(nScaleUnits)), strMapSeries);
   MapProjectionParams map_proj_params;
   map_proj_params.center.lat = dCenterLat;
   map_proj_params.center.lon = dCenterLon;
   map_proj_params.rotation = dRotation;
   map_proj_params.zoom_percent = nZoomPercent;
   map_proj_params.type = static_cast<ProjectionEnum>(nProjection);
   map_proj_params.tilt = -90.0;

   int status =
      map_view->ChangeMapType(category.GetGroupId(), map_type, map_proj_params);

   if (status == SUCCESS)
      map_view->invalidate_view(FALSE);

   return status;
}