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

// ILayer4.cpp
//

#include "StdAfx.h"
#include "ILayer4.h"
#include "LayerOvl.h"
#include "getobjpr.h"   // for fvw_get_frame
#include "ovl_mgr.h"

IMPLEMENT_DYNCREATE(Layer4, CCmdTarget)

IMPLEMENT_OLECREATE(Layer4, "FalconView.Layer4", 
0x4da20fc0, 0x736b, 0x4302, 0xaf, 0x57, 0x38, 0xfd, 0xce, 0x35, 0xda, 0x15);


Layer4::Layer4()
{
	EnableAutomation();
}

Layer4::~Layer4()
{
}

void Layer4::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(Layer4, CCmdTarget)
	//{{AFX_MSG_MAP(Layer4)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Layer4, CCmdTarget)
	//{{AFX_DISPATCH_MAP(Layer4)
   DISP_FUNCTION(Layer4, "SaveAs", SaveAs, VT_I4, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(Layer4, "GetOverlayDisplayName", GetOverlayDisplayName, VT_I4, VTS_I4 VTS_PBSTR)
   DISP_FUNCTION(Layer4, "DeactivateCurrentEditor", DeactivateCurrentEditor, VT_I4, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ILayer4 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_ILayer4 =
{ 0x6F6609F0, 0x923C, 0x4ad2, { 0x89, 0x71, 0x6E, 0x60, 0xEF, 0xAF, 0x25, 0xBF } };

BEGIN_INTERFACE_MAP(Layer4, CCmdTarget)
	INTERFACE_PART(Layer4, IID_ILayer4, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ILayer4 method implementations

long Layer4::SaveAs(long layer_handle, LPCTSTR file_spec)
{   // get a pointer to the overlay associated with the given handle
   C_overlay *pOverlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
	CFvOverlayPersistenceImpl *pFvOverlayPersistence = dynamic_cast<CFvOverlayPersistenceImpl *>(pOverlay);

   if (pOverlay == NULL || pFvOverlayPersistence == NULL)
   {
      ERR_report("Layer4::SaveAs failed - invalid layer handle");
      return FAILURE;
   }
	CString file_name(file_spec);

	return OVL_get_overlay_manager()->save_as(pOverlay, 0, file_name);
}


long Layer4::GetOverlayDisplayName(long overlay_handle, BSTR* overlay_display_name)
{
	C_overlay * pOverlay = OVL_get_overlay_manager()->lookup_overlay(overlay_handle);

   // if the overlay handle is invalid
   if (pOverlay == NULL)
   {
      ERR_report("The given overlay handle is invalid.");
      return FAILURE;
   }

	CString overlayName = OVL_get_overlay_manager()->GetOverlayDisplayName(pOverlay);

	*overlay_display_name = _bstr_t(overlayName).Detach();

	return SUCCESS;
}

long Layer4::DeactivateCurrentEditor()
{
   return OVL_get_overlay_manager()->set_mode(GUID_NULL);
}
