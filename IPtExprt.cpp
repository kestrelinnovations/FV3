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

// IPtExprt.cpp : implementation file
//

#include "stdafx.h"
#include "IPtExprt.h"
#include "PntExprt.h"
#include "pointex\factory.h"

// statics
//translates client color to server color
static
int translate_client_color(int client_color)
{
   int result;

   switch(client_color)
   {
      case 0 : result = BLACK; break;
      case 1 : result = STD_RED; break;
      case 2 : result = STD_GREEN; break;
      case 3 : result = YELLOW; break;
      case 4 : result = BLUE; break;
      case 5 : result = MAGENTA; break;
      case 6 : result = CYAN; break;
      case 7 : result = LIGHT_GREY; break;
      case 8 : result = PALE_GREEN; break;
      case 9 : result = LIGHT_BLUE; break;
      case 10 : result = OFF_WHITE; break;
      case 11 : result = MEDIUM_GRAY; break;
      case 12 : result = WHITE; break;
      case 13 : result = STD_BRIGHT_RED; break;
      case 14 : result = STD_BRIGHT_GREEN; break;
      case 15 : result = BRIGHT_YELLOW; break;
      case 16 : result = BRIGHT_BLUE; break;
      case 17 : result = BRIGHT_MAGENTA; break;
      case 18 : result = BRIGHT_CYAN; break;
      case 19 : result = BRIGHT_WHITE; break;
      default: 
         ASSERT(0); //client color not found
         result = BRIGHT_WHITE; 
         break;
   }

   return result;
}


/////////////////////////////////////////////////////////////////////////////
// PointExport

IMPLEMENT_DYNCREATE(PointExport, CCmdTarget)

// {D15DFFB2-7142-4A83-9452-0348026C4F10}
IMPLEMENT_OLECREATE(PointExport, "FalconView.PointExport", 0xD15DFFB2, 0x7142, 0x4A83, 
                    0x94, 0x52, 0x03, 0x48, 0x02, 0x6C, 0x4F, 0x10)
						  
PointExportDispatchContainer g_point_export_dispatch_lst;

PointExport::PointExport()
{
	EnableAutomation();

	EnableConnections();

	// initialize the dispatch pointer for the callback object to NULL
	m_pdisp = NULL;
}

PointExport::~PointExport()
{
	// remove our dispatch pointer from the global list since 
	// we are about to die
	g_point_export_dispatch_lst.remove(m_pdisp);
}

void PointExport::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(PointExport, CCmdTarget)
	//{{AFX_MSG_MAP(PointExport)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(PointExport, CCmdTarget)
	//{{AFX_DISPATCH_MAP(PointExport)
	DISP_FUNCTION(PointExport, "Register", Register, VT_I4, VTS_VARIANT)
	DISP_FUNCTION(PointExport, "AddRectangleRegion", AddRectangleRegion, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
	DISP_FUNCTION(PointExport, "AddCircleRegion", AddCircleRegion, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_R8)
	DISP_FUNCTION(PointExport, "MoveRegion", MoveRegion, VT_I4, VTS_I4 VTS_I4 VTS_R8 VTS_R8)
	DISP_FUNCTION(PointExport, "DeleteRegion", DeleteRegion, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(PointExport, "ChangeRegionColor", ChangeRegionColor, VT_I4, VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION(PointExport, "ChangeRegionLineWidth", ChangeRegionLineWidth, VT_I4, VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION(PointExport, "DoExport", DoExport, VT_I4, VTS_I4 VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IPointExport to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {A1D9A300-597D-4BD8-99F3-02412A4754BE}
static const IID IID_IPointExport =
{ 0xa1d9a300, 0x597d, 0x4bd8, { 0x99, 0xf3, 0x2, 0x41, 0x2a, 0x47, 0x54, 0xbe } };

/*
// connection points interface
// {181FD91D-034E-4c90-B71E-13FB09234BD8}
static const IID IID_IExportRegionChangeNotify = 
{ 0x181fd91d, 0x34e, 0x4c90, { 0xb7, 0x1e, 0x13, 0xfb, 0x9, 0x23, 0x4b, 0xd8 } };


DEFINE_GUID(IID_IExportRegionChangeNotify, 
0x181fd91d, 0x34e, 0x4c90, 0xb7, 0x1e, 0x13, 0xfb, 0x9, 0x23, 0x4b, 0xd8);
*/


BEGIN_INTERFACE_MAP(PointExport, CCmdTarget)
	INTERFACE_PART(PointExport, IID_IPointExport, Dispatch)
	INTERFACE_PART(PointExport, IID_IConnectionPointContainer, ConnPtContainer)
END_INTERFACE_MAP()


BEGIN_CONNECTION_MAP(PointExport, CCmdTarget)
CONNECTION_PART(PointExport, fvw::IID_IExportRegionChangeNotify, ExportRegionChangeConnPt)
END_CONNECTION_MAP()


/////////////////////////////////////////////////////////////////////////////
// PointExport message handlers

long PointExport::Register(const VARIANT FAR& dispatch_pointer) 
{
   try
   {
      // store the dispatch pointer of the client's callback object
      m_pdisp = _variant_t(dispatch_pointer);

      // add the dispatch object to the global list of dispatch pointers
      g_point_export_dispatch_lst.add(m_pdisp);

      return SUCCESS;
   }
   catch (_com_error &)
   {
      return FAILURE;
   }
}

long PointExport::AddRectangleRegion(long layer_handle, double nw_lat, double nw_lon, 
												 double se_lat, double se_lon) 
{
	// TODO: Add your dispatch handler code here
	// Initialize the handle to a bogus value
	long object_handle = -1;

	// get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CPointExportOverlay pointer
   if (overlay->get_m_overlayDescGuid() != FVWID_Overlay_PointExport)
      return FAILURE;

	object_handle = static_cast<CPointExportOverlay*>(overlay)->add_rectangle_region(nw_lat,nw_lon,se_lat,se_lon);
		
	OVL_get_overlay_manager()->invalidate_all(FALSE);
	

	return object_handle;
}

long PointExport::AddCircleRegion(long layer_handle, double center_lat, double center_lon, double radius) 
{
	// TODO: Add your dispatch handler code here
	// Initialize the handle to a bogus value
	long object_handle = -1;

	// get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CPointExportOverlay pointer
   if (overlay->get_m_overlayDescGuid() != FVWID_Overlay_PointExport)
      return FAILURE;

	object_handle = static_cast<CPointExportOverlay*>(overlay)->add_circle_region(center_lat, center_lon, radius);
		
	OVL_get_overlay_manager()->invalidate_all(FALSE);

	return object_handle;
}

long PointExport::MoveRegion(long layer_handle, long object_handle, double offset_lat, double offset_lon) 
{
	// TODO: Add your dispatch handler code here
	int status = FAILURE;
	
	// get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CPointExportOverlay pointer
   if (overlay->get_m_overlayDescGuid() != FVWID_Overlay_PointExport)
      return FAILURE;
		
	status = static_cast<CPointExportOverlay*>(overlay)->move_region(object_handle, offset_lat,offset_lon);

	OVL_get_overlay_manager()->invalidate_all(FALSE);

	// Multicast the event for connected points
	FireExportRegionChange(object_handle);

	return (long)status;
}

long PointExport::DeleteRegion(long layer_handle, long object_handle) 
{
	int status = FAILURE;
	
	// get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CPointExportOverlay pointer
   if (overlay->get_m_overlayDescGuid() != FVWID_Overlay_PointExport)
      return FAILURE;
		
	status = static_cast<CPointExportOverlay*>(overlay)->delete_region(object_handle);

	OVL_get_overlay_manager()->invalidate_all(FALSE);

	return (long)status;
}


long PointExport::ChangeRegionColor(long layer_handle, long object_handle, long client_color) 
{
	int status = FAILURE;
	
	// get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CPointExportOverlay pointer
   if (overlay->get_m_overlayDescGuid() != FVWID_Overlay_PointExport)
      return FAILURE;
	
	int color = translate_client_color(client_color);
	status = static_cast<CPointExportOverlay*>(overlay)->set_line_color(color); 
	
	OVL_get_overlay_manager()->invalidate_all(FALSE);
	
	return (long)status;
}

long PointExport::ChangeRegionLineWidth(long layer_handle, long object_handle, long width) 
{
	int status = FAILURE;
	
	// get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CPointExportOverlay pointer
   if (overlay->get_m_overlayDescGuid() != FVWID_Overlay_PointExport)
      return FAILURE;

	status = static_cast<CPointExportOverlay*>(overlay)->set_line_width(width);

	OVL_get_overlay_manager()->invalidate_all(FALSE);

	return (long)status;
}


void PointExport::FireExportRegionChange(long object_handle)
{
	// Retrieve the array of connected interfaces from this 
	// connection point
	const CPtrArray* pConnections = m_xExportRegionChangeConnPt.GetConnections();

	for (int i=0; i < pConnections->GetSize(); i++)
	{
		static_cast<fvw::IExportRegionChangeNotify*>(pConnections->GetAt(i))->OnExportRegionChange(object_handle);
	}
}


int PointExport::XExportRegionChangeConnPt::GetMaxConnections()
{
	return 10;
}


long PointExport::DoExport(long layer_handle, LPCSTR file_spec) 
{
	int status = FAILURE;
	
	if ( file_spec == NULL )
		return FAILURE;

	// get a pointer to the overlay associated with the given handle
	C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (overlay == NULL)
      return FAILURE;

   // make sure this is a CPointExportOverlay pointer
   if (overlay->get_m_overlayDescGuid() != FVWID_Overlay_PointExport)
      return FAILURE;

	CString export_file_name = file_spec;

	if ( !export_file_name.IsEmpty() )
		status = ((CPointExportOverlay*)overlay)->save_to_file(export_file_name);

	return (long)status;
}
