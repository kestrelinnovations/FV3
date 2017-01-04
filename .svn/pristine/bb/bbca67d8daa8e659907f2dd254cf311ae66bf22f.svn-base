// Copyright (c) 1994-2009,2013 Georgia Tech Research Corporation, Atlanta, GA
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



#include "stdafx.h"
#include "factory.h"
#include "err.h"                    // for ERR_
#include "param.h"                  // for PRM_
#include "file.h"                   // for FIL_
#include "gps.h"
#include "appinfo.h"
#include "..\getobjpr.h"

#include "FalconView/include/InternalToolbarImpl.h"
#include "GPSTrailDisplayElementProvider.h"


// {9E30F658-9D21-417e-A1C5-B460C0E4F637}
const GUID FVWID_Overlay_MovingMapTrail = 
   { 0x9e30f658, 0x9d21, 0x417e, { 0xa1, 0xc5, 0xb4, 0x60, 0xc0, 0xe4, 0xf6, 0x37 } };


C_gps_trail_editor::C_gps_trail_editor() :
   m_toolbar(new InternalToolbarImpl(IDR_GPS_TB))
{
}

HCURSOR C_gps_trail_editor::get_default_cursor()
{ 
   return C_gps_trail::get_default_cursor(); 
}

int C_gps_trail_editor::set_edit_on(boolean_t edit_on)
{ 
   m_toolbar->ShowToolbar(edit_on);
   return C_gps_trail::set_edit_on(edit_on); 
}

// IFvOverlayLimitUserInterface

// Return TRUE if the user should not be able to change the projection of the map
HRESULT C_gps_trail_editor::get_m_bDisableProjectionUI(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the user should not be able to rotate the map while the editor is active
HRESULT C_gps_trail_editor::get_m_bDisableRotationUI(long* pVal)
{
   *pVal = C_gps_trail::auto_rotate_on();
   return S_OK;
}

// Return TRUE if the map must be North-Up
HRESULT C_gps_trail_editor::get_m_bRequiresNorthUp(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the map must be projected with the Equal Arc projection when this editor is active
HRESULT C_gps_trail_editor::get_m_bRequiresEqualArc(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the order of overlays associated with this editor cannot be changed
HRESULT C_gps_trail_editor::get_m_bDisableOverlayOrdering(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}


// GPSTrailDisplayElementProviderFactory
//

void GPSTrailDisplayElementProviderFactory::CreateInstance(
   DisplayElementProvider_Interface** ppIDEPF)
{
   *ppIDEPF = new GPSTrailDisplayElementProvider;
}

// End of MovidingMapOverlay\factory.cpp
