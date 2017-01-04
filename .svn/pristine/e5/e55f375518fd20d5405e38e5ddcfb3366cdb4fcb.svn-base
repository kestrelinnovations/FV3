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

// TacticalGraphicsServer2.cpp : implementation file
//

#include "stdafx.h"
#include "TacticalGraphicsServer2.h"
#include "FctryLst.h"
#include "err.h"

/////////////////////////////////////////////////////////////////////////////
// TacticalGraphicsServer2

IMPLEMENT_DYNCREATE(TacticalGraphicsServer2, CCmdTarget)

TacticalGraphicsServer2::TacticalGraphicsServer2()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	// AfxOleLockApp();
}

TacticalGraphicsServer2::~TacticalGraphicsServer2()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	// AfxOleUnlockApp();
}


void TacticalGraphicsServer2::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(TacticalGraphicsServer2, CCmdTarget)
	//{{AFX_MSG_MAP(TacticalGraphicsServer2)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(TacticalGraphicsServer2, CCmdTarget)
	//{{AFX_DISPATCH_MAP(TacticalGraphicsServer2)
	DISP_FUNCTION(TacticalGraphicsServer2, "UpdateGraphics", UpdateGraphics, VT_I4, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(TacticalGraphicsServer2, "RemoveGraphic", RemoveGraphic, VT_I4, VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ITacticalGraphicsServer2 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {EBFEEED1-642B-4bf0-9889-53E92C2AD6E1}
static const IID IID_ITacticalGraphicsServer2 =
{ 0xebfeeed1, 0x642b, 0x4bf0, { 0x98, 0x89, 0x53, 0xe9, 0x2c, 0x2a, 0xd6, 0xe1 } };

BEGIN_INTERFACE_MAP(TacticalGraphicsServer2, CCmdTarget)
	INTERFACE_PART(TacticalGraphicsServer2, IID_ITacticalGraphicsServer2, Dispatch)
END_INTERFACE_MAP()

// {75717315-6E0E-483e-98B0-6FB9B4B7F216}
IMPLEMENT_OLECREATE(TacticalGraphicsServer2, "FalconView.TacticalGraphicsServer2", 0x75717315, 0x6e0e, 0x483e, 0x98, 0xb0, 0x6f, 0xb9, 0xb4, 0xb7, 0xf2, 0x16)

/////////////////////////////////////////////////////////////////////////////
// TacticalGraphicsServer2 message handlers

long TacticalGraphicsServer2::UpdateGraphics(long tg_ovl_handle, LPCTSTR tg_xml_doc)
{
   int ret = SUCCESS;

   TRACE(
      "TacticalGraphicsServer2::UpdateGraphics(): tg_ovl_handle = %d; length of tg_xml_doc = %d\n",
      tg_ovl_handle,
      ::strlen(tg_xml_doc));

   FalconViewOverlayLib::IFvOverlayManager2Ptr ovl_mgr2 = OVL_get_overlay_manager()->GetFvOverlayManager();

   FalconViewOverlayLib::IFvOverlayPtr overlay = ovl_mgr2->GetOverlayForHandle(tg_ovl_handle);
   
   if (!overlay)
   {
      CString msg;
      msg.Format("TacticalGraphicsServer2::UpdateGraphics(): %d is not the handle of an overlay.", tg_ovl_handle);
      ERR_report(msg);
      return FAILURE;
   }

   if (overlay->OverlayDescGuid != FVWID_Overlay_TacticalGraphics)
   {
      CString msg;
      msg.Format("TacticalGraphicsServer2::UpdateGraphics(): %d is not the handle of a tactical graphics overlay.", tg_ovl_handle);
      ERR_report(msg);
      return FAILURE;
   }

   try
   {
      ITacticalGraphicsOverlayPtr tg_overlay = overlay;

      ret = tg_overlay->UpdateGraphics(tg_xml_doc);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("UpdateGraphics failed: %s", e.Description());
      ERR_report(msg);
   }

   return ret;
}

long TacticalGraphicsServer2::RemoveGraphic(long tg_handle)
{
   int ret = 1;

   FalconViewOverlayLib::IFvOverlayManagerPtr ovl_mgr = OVL_get_overlay_manager()->GetFvOverlayManager();

   ovl_mgr->SelectByOverlayDescGuid(FVWID_Overlay_TacticalGraphics);
   
   try
   {
      ITacticalGraphicsOverlayPtr tg_overlay = ovl_mgr->CurrentOverlay;
      
      ret = tg_overlay->RemoveGraphic(tg_handle);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("RemoveGraphic failed: %s", e.Description());
      ERR_report(msg);
   }
   
   return ret;
}
