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

// TacticalGraphicsServer.cpp : implementation file
//

#include "stdafx.h"
#include "TacticalGraphicsServer.h"
#include "FctryLst.h"
#include "err.h"

/////////////////////////////////////////////////////////////////////////////
// TacticalGraphicsServer

IMPLEMENT_DYNCREATE(TacticalGraphicsServer, CCmdTarget)

TacticalGraphicsServer::TacticalGraphicsServer()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	// AfxOleLockApp();
}

TacticalGraphicsServer::~TacticalGraphicsServer()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	// AfxOleUnlockApp();
}


void TacticalGraphicsServer::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(TacticalGraphicsServer, CCmdTarget)
	//{{AFX_MSG_MAP(TacticalGraphicsServer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(TacticalGraphicsServer, CCmdTarget)
	//{{AFX_DISPATCH_MAP(TacticalGraphicsServer)
	DISP_FUNCTION(TacticalGraphicsServer, "AddGraphics", AddGraphics, VT_I4, VTS_I4 VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


// Note: we add support for IID_ITacticalGraphicsServer to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {155AE0F6-999C-44C4-BB50-67668772A5EE}
static const IID IID_ITacticalGraphicsServer =
{ 0x155ae0f6, 0x999c, 0x44c4, { 0xbb, 0x50, 0x67, 0x66, 0x87, 0x72, 0xa5, 0xee } };

BEGIN_INTERFACE_MAP(TacticalGraphicsServer, CCmdTarget)
	INTERFACE_PART(TacticalGraphicsServer, IID_ITacticalGraphicsServer, Dispatch)
END_INTERFACE_MAP()

// {5815E279-B684-4C45-A343-1B95AB14D6F1}
IMPLEMENT_OLECREATE(TacticalGraphicsServer, "FalconView.TacticalGraphicsServer", 0x5815e279, 0xb684, 0x4c45, 0xa3, 0x43, 0x1b, 0x95, 0xab, 0x14, 0xd6, 0xf1)

/////////////////////////////////////////////////////////////////////////////
// TacticalGraphicsServer message handlers

long TacticalGraphicsServer::AddGraphics(long tg_ovl_handle, LPCTSTR tg_xml_doc) 
{
   long ret = SUCCESS;

   TRACE(
      "TacticalGraphicsServer::AddGraphics(): tg_ovl_handle = %d; length of tg_xml_doc = %d\n",
      tg_ovl_handle,
      ::strlen(tg_xml_doc));
   
   FalconViewOverlayLib::IFvOverlayManager2Ptr ovl_mgr2 = OVL_get_overlay_manager()->GetFvOverlayManager();

   FalconViewOverlayLib::IFvOverlayPtr overlay = ovl_mgr2->GetOverlayForHandle(tg_ovl_handle);

   if (!overlay)
   {
      CString msg;
      msg.Format("TacticalGraphicsServer::AddGraphics(): %d is not the handle of an overlay.", tg_ovl_handle);
      ERR_report(msg);
      return FAILURE;
   }

   if (overlay->OverlayDescGuid != FVWID_Overlay_TacticalGraphics)
   {
      CString msg;
      msg.Format("TacticalGraphicsServer::AddGraphics(): %d is not the handle of a tactical graphics overlay.", tg_ovl_handle);
      ERR_report(msg);
      return FAILURE;
   }

   try
   {
      ITacticalGraphicsOverlayPtr tg_overlay = overlay;
           
      ret = tg_overlay->AddGraphics(tg_xml_doc);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("AddGraphics failed: %s", e.Description());
      ERR_report(msg);
   }

   return ret;
}
