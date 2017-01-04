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
#include "ilayer2.h"
#include "layerovl.h"   // for CLayerOvl
#include "ovlElementContainer.h"

IMPLEMENT_DYNCREATE(Layer2, CCmdTarget)

IMPLEMENT_OLECREATE(Layer2, "FalconView.Layer2", 0xBA140F8C, 0xC77C, 0x4b80, 
                    0xB4, 0x88, 0x48, 0x35, 0x36, 0xFC, 0xBF, 0x61)

Layer2::Layer2()
{
	EnableAutomation();
}

Layer2::~Layer2()
{
}

void Layer2::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(Layer2, CCmdTarget)
	//{{AFX_MSG_MAP(Layer2)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Layer2, CCmdTarget)
	//{{AFX_DISPATCH_MAP(Layer2)
   DISP_FUNCTION(Layer2, "AddRegion", AddRegion, VT_I4, VTS_I4 VTS_I4 VTS_VARIANT VTS_I4)
   DISP_FUNCTION(Layer2, "CombineRegion", CombineRegion, VT_I4, VTS_I4 VTS_I4 VTS_I4 VTS_VARIANT VTS_I4 VTS_I4)
	DISP_FUNCTION(Layer2, "DisplayMask", DisplayMask, VT_I4, VTS_I4 VTS_BSTR VTS_R8 VTS_I4 VTS_I4 VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ILayer2 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_ILayer2 =
{ 0xCB8BF255, 0x3048, 0x4874, { 0x9A, 0x35, 0x34, 0x85, 0xA6, 0xBD, 0x7A, 0x20 } };

BEGIN_INTERFACE_MAP(Layer2, CCmdTarget)
	INTERFACE_PART(Layer2, IID_ILayer2, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Layer message handlers

long Layer2::AddRegion(long layer_handle, int type, const VARIANT FAR& lat_lon_array, 
				long num_points)
{
   // verify that type is within range
   if (type < 0 || type > 2)
   {
      ERR_report("AddRegion failure [type is invalid...must be 0,1, or 2]");
      return FAILURE;
   }

   COleSafeArray geo_points(lat_lon_array);
   
   // make sure the dimension of the array is 2
   if (geo_points.GetDim() != 2)
   {
      ERR_report("AddRegion failure [lat_lon_array must have 2 dimensions]");
      return FAILURE;
   }

   // get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
   {
      ERR_report("AddRegion failure [layer_handle is invalid]");
      return FAILURE;
   }

   // make sure this is a CBaseLayerOvl pointer
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL)
   {
      ERR_report("AddRegion failure [layer_handle does not correspond to a client layer]");
      return FAILURE;
   }

   return overlay->GetOvlElementContainer()->add_rgn(type, geo_points, num_points);
}

long Layer2::CombineRegion(long layer_handle, long region_handle, 
				int type, const VARIANT FAR &lat_lon_array, long num_points, int operation)
{
   // verify that type is within range
   if (type < 0 || type > 2)
   {
      ERR_report("AddRegion failure [type is invalid...must be 0,1, or 2]");
      return FAILURE;
   }

   // verify operation is within range
   if (operation < 0 || operation > 3)
   {
      ERR_report("AddRegion failure [operation is invalid...must be 0,1,2, or 3]");
      return FAILURE;
   }

   COleSafeArray geo_points(lat_lon_array);
   
   // make sure the dimension of the array is 2
   if (geo_points.GetDim() != 2)
   {
      ERR_report("AddRegion failure [lat_lon_array must have 2 dimensions]");
      return FAILURE;
   }

   // get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
   {
      ERR_report("AddRegion failure [layer_handle is invalid]");
      return FAILURE;
   }

   // make sure this is a CBaseLayerOvl pointer
   if (dynamic_cast<CBaseLayerOvl *>(overlay) == NULL)
   {
      ERR_report("AddRegion failure [layer_handle does not correspond to a client layer]");
      return FAILURE;
   }

   return overlay->GetOvlElementContainer()->combine_rgn(region_handle, type, 
                            geo_points, num_points, operation);

}


long Layer2::DisplayMask(long layer_handle, 
								 LPCTSTR file_spec, 
								 double flight_altitude, 
								 long base_elevation,      // 0 = AGL, 1 = MSL
								 long mask_color, 
								 long brush_style)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

	CString file_name(file_spec);

// get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CBaseLayerOvl pointer
   CBaseLayerOvl *pBaseLayerOverlay = dynamic_cast<CBaseLayerOvl *>(overlay);
   if (pBaseLayerOverlay == NULL)
      return FAILURE;

	CString file(file_spec);

	// display the mask for this file
	return pBaseLayerOverlay->DisplayMask( file, flight_altitude, 
		base_elevation, mask_color, brush_style);
}
