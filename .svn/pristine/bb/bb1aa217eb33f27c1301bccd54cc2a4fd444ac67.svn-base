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

// IGPS.cpp : implementation file
//

#include "stdafx.h"
#include "IGPS.h"
#include "err.h"
#include "MovingMapOverlay\factory.h"

/////////////////////////////////////////////////////////////////////////////
// GPS

IMPLEMENT_DYNCREATE(GPS, CCmdTarget)
IMPLEMENT_DYNCREATE(GPS2, CCmdTarget)

// {1E4408E2-7CFF-11D3-80A9-00C04F60B086}
IMPLEMENT_OLECREATE(GPS, "FalconView.GPS", 0x1E4408E2, 0x7CFF, 0x11D3, 
                    0x80, 0xA9, 0x00, 0xC0, 0x4F, 0x60, 0xB0, 0x86)

IMPLEMENT_OLECREATE(GPS2, "FalconView.GPS2", 0x94E84246, 0x7DCA, 0x4ec4, 
                    0xA4, 0x19, 0xD7, 0xCD, 0x07, 0x07, 0x5C, 0x3D)

GPS::GPS()
{
	EnableAutomation();

   m_registered_for_connect_disconnect = 0;
}

GPS2::GPS2()
{
	EnableAutomation();
}

GPS::~GPS()
{
}

GPS2::~GPS2()
{
}


void GPS::OnFinalRelease()
{
	CCmdTarget::OnFinalRelease();
}

void GPS2::OnFinalRelease()
{
	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(GPS, CCmdTarget)
	//{{AFX_MSG_MAP(GPS)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(GPS2, CCmdTarget)
	//{{AFX_MSG_MAP(GPS2)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(GPS, CCmdTarget)
	//{{AFX_DISPATCH_MAP(GPS)
	DISP_FUNCTION(GPS, "GetCurrentPoint", GetCurrentPoint, VT_I4, VTS_I4 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PDATE)
   DISP_FUNCTION(GPS, "RegisterForConnectDisconnect", RegisterForConnectDisconnect, VT_I4, VTS_I4)
	DISP_FUNCTION(GPS, "UnRegisterForConnectDisconnect", UnRegisterForConnectDisconnect, VT_I4, VTS_I4)
	DISP_FUNCTION(GPS, "RegisterForCurrentPosition", RegisterForCurrentPosition, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(GPS, "UnRegisterForCurrentPosition", UnRegisterForCurrentPosition, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(GPS, "GetConnectedHandle", GetConnectedHandle, VT_I4, VTS_NONE)
	DISP_FUNCTION(GPS, "AddPoint", AddPoint, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_DATE)
	DISP_FUNCTION(GPS, "Connect", Connect, VT_I4, VTS_NONE)
	DISP_FUNCTION(GPS, "Disconnect", Disconnect, VT_I4, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

BEGIN_DISPATCH_MAP(GPS2, CCmdTarget)
	//{{AFX_DISPATCH_MAP(GPS)
	DISP_FUNCTION(GPS2, "AddPointEx", AddPointEx, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_DATE VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IGPS to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {1E4408E0-7CFF-11D3-80A9-00C04F60B086}
static const IID IID_IGPS =
{ 0x1e4408e0, 0x7cff, 0x11d3, { 0x80, 0xa9, 0x0, 0xc0, 0x4f, 0x60, 0xb0, 0x86 } };

// {C021B657-6930-4cda-9918-7708329A4BC7}
static const IID IID_IGPS2 = 
{ 0xC021B657, 0x6930, 0x4cda, { 0x99, 0x18, 0x77, 0x08, 0x32, 0x9A, 0x4B, 0xC7} };

BEGIN_INTERFACE_MAP(GPS, CCmdTarget)
	INTERFACE_PART(GPS, IID_IGPS, Dispatch)
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(GPS2, CCmdTarget)
	INTERFACE_PART(GPS2, IID_IGPS2, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GPS message handlers
long GPS::GetCurrentPoint(long gps_overlay_handle, double FAR* latitude, 
                          double FAR* longitude, double FAR* altitude, 
                          double FAR* true_course, double FAR* mag_course,
                          double FAR* speed_knots, DATE FAR* date_time) 
{
	// get a pointer to the gps trail that will be processing the add point
   C_overlay *gps_trail = 
      OVL_get_overlay_manager()->lookup_overlay(gps_overlay_handle);

   // make sure we get a valid pointer
   if (gps_trail == NULL)
      return FAILURE;

   // make sure this is a C_gps_trail pointer
   if (gps_trail->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   GPSPointIcon gps_point;

   // get the trail's current point information
   (dynamic_cast<C_gps_trail *>(gps_trail))->get_current_point(&gps_point);

   *latitude = gps_point.get_latitude();
   *longitude = gps_point.get_longitude();
   *altitude = gps_point.m_msl;
   *true_course = gps_point.m_true_heading;
   *mag_course = gps_point.m_magnetic_heading;
   *speed_knots = gps_point.m_speed_knots;
   *date_time = gps_point.m_dateTime;

   return SUCCESS;
}

long GPS::RegisterForConnectDisconnect(long hWnd) 
{
   C_gps_trail::add_HWND_to_connect_notify_lst((HWND)hWnd);

	m_registered_for_connect_disconnect++;

	return SUCCESS;
}

long GPS::UnRegisterForConnectDisconnect(long hWnd) 
{
   C_gps_trail::remove_HWND_from_connect_notify_lst((HWND)hWnd);

	if (m_registered_for_connect_disconnect > 0)
      m_registered_for_connect_disconnect--;

	return SUCCESS;
}

long GPS::RegisterForCurrentPosition(long hWnd, long gps_overlay_handle) 
{
   // get a pointer to the gps trail that will be processing the add point
   C_overlay *gps_trail = 
      OVL_get_overlay_manager()->lookup_overlay(gps_overlay_handle);
   
   // make sure we get a valid pointer
   if (gps_trail == NULL)
      return FAILURE;
   
   // make sure this is a C_gps_trail pointer
   if (gps_trail->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;
   
   // add the given window handle to the notification list of the gps trail
   (dynamic_cast<C_gps_trail *>(gps_trail))->add_HWND_to_current_pos_notify_lst((HWND)hWnd);

   return SUCCESS;
}

long GPS::UnRegisterForCurrentPosition(long hWnd, long gps_overlay_handle) 
{
   // get a pointer to the gps trail that will be processing the add point
   C_overlay *gps_trail = 
      OVL_get_overlay_manager()->lookup_overlay(gps_overlay_handle);
   
   // make sure we get a valid pointer
   if (gps_trail == NULL)
      return FAILURE;
   
   // make sure this is a C_gps_trail pointer
   if (gps_trail->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;
   
   // remove thet given window handle from the notification list of the trail
   (dynamic_cast<C_gps_trail *>(gps_trail))->remove_HWND_from_current_pos_notify_lst((HWND)hWnd);

   return SUCCESS;
}

long GPS::GetConnectedHandle() 
{
   return C_gps_trail::m_connected_handle;
}

long GPS::AddPoint(long gps_overlay_handle, double latitude, double longitude, 
						 double altitude, double true_course, double mag_course, 
						 double speed_knots, DATE date_time) 
{
	// get a pointer to the gps trail that will be processing the add point
   C_overlay *gps_trail = 
      OVL_get_overlay_manager()->lookup_overlay(gps_overlay_handle);
      
   // make sure we get a valid pointer
   if (gps_trail == NULL)
      return FAILURE;

   // make sure this is a C_gps_trail pointer
   if (gps_trail->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // fill in the GPSPointIcon structure
   GPS_QUEUED_POINT qpoint;
   qpoint.latitude = (float)latitude;
   qpoint.longitude = (float)longitude;
   qpoint.msl = (float)altitude; // meters
   qpoint.true_heading = (float)true_course;
   qpoint.magnetic_heading = (float)mag_course;
   qpoint.speed_knots = (float)speed_knots;
	qpoint.speed_km_hr = (float)(KNOTS_TO_KM_PER_H(speed_knots));
   qpoint.dateTime = date_time;

	// add the gps point passed in
   return (dynamic_cast<C_gps_trail *>(gps_trail))->add_point(qpoint);
}

long GPS2::AddPointEx(long gps_overlay_handle, double latitude, double longitude, 
						 double altitude, double true_course, double mag_course, 
						 double speed_knots, DATE date_time, LPCTSTR meta_data) 
{
	// get a pointer to the gps trail that will be processing the add point
   C_overlay *gps_trail = 
      OVL_get_overlay_manager()->lookup_overlay(gps_overlay_handle);
      
   // make sure we get a valid pointer
   if (gps_trail == NULL)
      return FAILURE;

   // make sure this is a C_gps_trail pointer
   if (gps_trail->get_m_overlayDescGuid() != FVWID_Overlay_MovingMapTrail)
      return FAILURE;

   // fill in the GPSPointIcon structure
   GPS_QUEUED_POINT qpoint;
   qpoint.latitude = (float)latitude;
   qpoint.longitude = (float)longitude;
   qpoint.msl = (float)altitude; // meters
   qpoint.true_heading = (float)true_course;
   qpoint.magnetic_heading = (float)mag_course;
   qpoint.speed_knots = (float)speed_knots;
	qpoint.speed_km_hr = (float)(KNOTS_TO_KM_PER_H(speed_knots));
   qpoint.dateTime = date_time;
	qpoint.meta_data = meta_data;

	// add the gps point passed in
   return (dynamic_cast<C_gps_trail *>(gps_trail))->add_point(qpoint);
}

long GPS::Connect()
{
	if (m_registered_for_connect_disconnect != 0)
	{
		ERR_report("Client should not be registered to receive Connect/Disconnect messages when calling IGPS::Connect()");
		return FAILURE;
	}

	// see if an active GPS trail exists
	if (C_gps_trail::get_active_trail == NULL)
		return FAILURE;

	// if we are disconnected, then connect
	if (!C_gps_trail::get_toggle('CONN'))
		C_gps_trail::set_toggle('CONN');

	return SUCCESS;
}

long GPS::Disconnect() 
{
	if (m_registered_for_connect_disconnect != 0)
	{
		ERR_report("Client should not be registered to receive Connect/Disconnect messages when calling IGPS::Disconnect()");
		return FAILURE;
	}

	// see if an active GPS trail exists
	if (C_gps_trail::get_active_trail == NULL)
		return FAILURE;

	// if we are connected, then disconnect
	if (C_gps_trail::get_toggle('CONN'))
		C_gps_trail::set_toggle('CONN');

	return SUCCESS;
}
