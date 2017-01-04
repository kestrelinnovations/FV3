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

// ISkyViewOverlay.cpp
//

#include "StdAfx.h"
#include "ISkyViewOverlay.h"
#include "mov_sym.h"
#include "SkyViewOverlay\skyview.h"
#include "utils.h"
#include "mapx.h"
#include "SkyViewOverlay\factory.h"

IMPLEMENT_DYNCREATE(SkyViewOverlay, CCmdTarget)

IMPLEMENT_OLECREATE(SkyViewOverlay, "FalconView.SkyViewOverlay", 0x129E64DD, 0xDCBB, 0x462c, 
                   0x98, 0xD2, 0x1A, 0x0F, 0xDA, 0xF9, 0x34, 0xBD)

SkyViewOverlay::SkyViewOverlay()
{
	EnableAutomation();
}

SkyViewOverlay::~SkyViewOverlay()
{
}

void SkyViewOverlay::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(SkyViewOverlay, CCmdTarget)
	//{{AFX_MSG_MAP(SkyViewOverlay)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(SkyViewOverlay, CCmdTarget)
	//{{AFX_DISPATCH_MAP(SkyViewOverlay)
   DISP_FUNCTION(SkyViewOverlay, "ShowSkyViewSymbol",ShowSkyViewSymbol, VT_I4, VTS_I4)
	DISP_FUNCTION(SkyViewOverlay, "UpdateStateIndicators", UpdateStateIndicators, VT_I4, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BOOL VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BOOL VTS_R8 VTS_R8 VTS_R8 VTS_BOOL VTS_R8 VTS_R8 VTS_R8 VTS_BOOL VTS_R8 VTS_R8 VTS_R8 VTS_BOOL VTS_R8 VTS_R8 VTS_R8)
	DISP_FUNCTION(SkyViewOverlay, "ClearStateIndicators", ClearStateIndicators, VT_I4, VTS_NONE)
   DISP_FUNCTION(SkyViewOverlay, "GetSkyView", GetSkyView, VT_I4, VTS_PVARIANT)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISkyViewOverlay to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_ISkyViewOverlay =
{ 0xCEADE38E, 0x5B3A, 0x458f, { 0x9A, 0x53, 0xFB, 0x0C, 0xA4, 0xF5, 0xBA, 0x5B } };

BEGIN_INTERFACE_MAP(SkyViewOverlay, CCmdTarget)
	INTERFACE_PART(SkyViewOverlay, IID_ISkyViewOverlay, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SkyViewOverlay message handlers

long SkyViewOverlay::ShowSkyViewSymbol(long show)
{
	Cmov_sym_overlay::s_show_skyview_symbol = show;
	OVL_get_overlay_manager()->invalidate_all();

	return SUCCESS;
}

long SkyViewOverlay::UpdateStateIndicators(
		double camera_lat, double camera_lon, double camera_altitude, double camera_heading, 
		double map_node_load_center_lat, double map_node_load_center_lon,
		VARIANT_BOOL terrain_is_loaded,
		double terrain_bounding_lat_south, double terrain_bounding_lon_west,
		double terrain_bounding_lat_north, double terrain_bounding_lon_east,
		
		VARIANT_BOOL terrain_point_seen_at_upper_left_screen_point_found,
		double terrain_point_seen_at_upper_left_screen_point_lat,
		double terrain_point_seen_at_upper_left_screen_point_lon,
		double terrain_point_seen_at_upper_left_screen_point_elevation,
		
		VARIANT_BOOL terrain_point_seen_at_upper_right_screen_point_found,
		double terrain_point_seen_at_upper_right_screen_point_lat,
		double terrain_point_seen_at_upper_right_screen_point_lon,
		double terrain_point_seen_at_upper_right_screen_point_elevation,
		
		VARIANT_BOOL terrain_point_seen_at_lower_left_screen_point_found,
		double terrain_point_seen_at_lower_left_screen_point_lat,
		double terrain_point_seen_at_lower_left_screen_point_lon,
		double terrain_point_seen_at_lower_left_screen_point_elevation,
		
		VARIANT_BOOL terrain_point_seen_at_lower_right_screen_point_found,
		double terrain_point_seen_at_lower_right_screen_point_lat,
		double terrain_point_seen_at_lower_right_screen_point_lon,
		double terrain_point_seen_at_lower_right_screen_point_elevation)
{
	if (Cmov_sym_overlay::GetStateIndicators()->m_symbol == NULL)
	{
		Cmov_sym_overlay::AddSymbol('HELI', 0xFFFFFFFF);
	}

	Cmov_sym_overlay::MoveSymbol(camera_lat, camera_lon, camera_altitude, camera_heading);

	Cmov_sym_overlay::GetStateIndicators()->update_terrain_bounds(terrain_is_loaded, 
		terrain_bounding_lat_south, terrain_bounding_lon_west,
		terrain_bounding_lat_north, terrain_bounding_lon_east);

	Cmov_sym_overlay::GetStateIndicators()->update_frustum(
		camera_lat, camera_lon, terrain_is_loaded,
		terrain_bounding_lat_south, terrain_bounding_lon_west,
		terrain_bounding_lat_north, terrain_bounding_lon_east,

		terrain_point_seen_at_upper_left_screen_point_found,
		terrain_point_seen_at_upper_left_screen_point_lat,
		terrain_point_seen_at_upper_left_screen_point_lon,
		
		terrain_point_seen_at_upper_right_screen_point_found,
		terrain_point_seen_at_upper_right_screen_point_lat,
		terrain_point_seen_at_upper_right_screen_point_lon,
		
		terrain_point_seen_at_lower_left_screen_point_found,
		terrain_point_seen_at_lower_left_screen_point_lat,
		terrain_point_seen_at_lower_left_screen_point_lon,
		
		terrain_point_seen_at_lower_right_screen_point_found,
		terrain_point_seen_at_lower_right_screen_point_lat,
		terrain_point_seen_at_lower_right_screen_point_lon);
	
	return SUCCESS;
}

long SkyViewOverlay::ClearStateIndicators()
{
	Cmov_sym_overlay::GetStateIndicators()->clear_state_indicators();

	return SUCCESS;
}

long SkyViewOverlay::GetSkyView(VARIANT *pSkyViewInterface)
{
   if (pSkyViewInterface == NULL)
   {
      ERR_report("ISkyViewOverlay::GetSkyView - Invalid parameter.  pSkyViewInteface must not be NULL");
      return FAILURE;
   }

   pSkyViewInterface->vt = VT_DISPATCH;
   pSkyViewInterface->pdispVal = NULL;

   // Check to make sure FalconView is ready.  If this method is called before the first map 
   // is drawn, FalconView and SkyView will deadlock.  This is because SkyView's contructor
   // has a spin-loop waiting for FalconView to become ready.
   // 
   //
   CView *pView = UTL_get_active_non_printing_view();
   if (pView == NULL)
      return FV_NOT_READY;

   const ViewMapProj *pMapProj = UTL_get_current_view_map(pView);
   if (pMapProj == NULL)
      return FV_NOT_READY;

	// make sure the current map is valid
	if (!pMapProj->is_projection_set())
		return FV_NOT_READY;

   // Create a SkyView overlay if one does not already exist
   //
   Cmov_sym_overlay *pSkyViewOverlay = (Cmov_sym_overlay *)OVL_get_overlay_manager()->
		get_first_of_type(FVWID_Overlay_SkyView);
   if (pSkyViewOverlay == NULL)
   {
      C_overlay *pOverlay = NULL;
      if (OVL_get_overlay_manager()->create(FVWID_Overlay_SkyView, &pOverlay) != SUCCESS)
      {
         ERR_report("ISkyViewOverlay::GetSkyView - Unable to create new SkyView overlay");
         return FAILURE;
      }
   }

   // Make a connection to SkyView if necessary
   //
   if (Cmov_sym_overlay::m_skyview_interface == NULL)
      Cmov_sym_overlay::open_skyview();

   // Retrieve dispatch interface
   //
   if (Cmov_sym_overlay::m_skyview_interface != NULL)
   {
      IDispatch *pDispatch = Cmov_sym_overlay::m_skyview_interface->GetDispatch();
      if (pDispatch == NULL)
      {
         ERR_report("ISkyViewOverlay::GetSkyView - dispatch pointer is NULL");
         return FAILURE;
      }

      pDispatch->AddRef();
      pSkyViewInterface->pdispVal = pDispatch;
   }
   else
   {
      ERR_report("ISkyViewOverlay::GetSkyView - the SkyView interface is NULL");
      return FAILURE;
   }

   return SUCCESS;
}
