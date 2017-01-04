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

// IOverlayElement.cpp
//

#include "StdAfx.h"
#include "IOverlayElement.h"
#include "err.h"        // for ERR_report
#include "overlay.h"

IMPLEMENT_DYNCREATE(COverlayElement, CCmdTarget)

IMPLEMENT_OLECREATE(COverlayElement, "FalconView.OverlayElement", 0x78E3B018, 0x51D2, 0x4595, 
                   0x8F, 0x9B, 0x31, 0xCA, 0xE7, 0xC6, 0x02, 0x2E)

COverlayElement::COverlayElement()
{
	EnableAutomation();
}

COverlayElement::~COverlayElement()
{
}

void COverlayElement::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(COverlayElement, CCmdTarget)
	//{{AFX_MSG_MAP(COverlayElement)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COverlayElement, CCmdTarget)
	//{{AFX_DISPATCH_MAP(COverlayElement)
   DISP_FUNCTION(COverlayElement, "RegisterForCallbacks", RegisterForCallbacks, VT_I4, VTS_I4 VTS_VARIANT)
   DISP_FUNCTION(COverlayElement, "UpdateElement", UpdateElement, VT_I4, VTS_I4 VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IOverlayElement to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IOverlayElement =
{ 0xE8816916, 0x8696, 0x4466, { 0x81, 0x2F, 0x15, 0x4C, 0xDA, 0x12, 0x1C, 0xFA } };

BEGIN_INTERFACE_MAP(COverlayElement, CCmdTarget)
	INTERFACE_PART(COverlayElement, IID_IOverlayElement, Dispatch)
END_INTERFACE_MAP()


long COverlayElement::RegisterForCallbacks(long lOverlayHandle, const VARIANT FAR& varDispatch)
{
   // verify that the variant is of the correct type
   if (varDispatch.vt != VT_DISPATCH)
   {
      CString msg;
      msg.Format("COverlayElement::RegisterForCallbacks failed - dispatch_pointer must be of type VT_DISPATCH.  "
         "Passed in type was %d", varDispatch.vt);
      ERR_report(msg);
      return FAILURE;
   }

   C_overlay *pOverlay = OVL_get_overlay_manager()->lookup_overlay(lOverlayHandle);
   if (pOverlay == NULL)
   {
      ERR_report("COverlayElement::RegisterForCallbacks failed - given overlay handle is invalid");
      return FAILURE;
   }

   OverlayElement_Interface *pOverlayElement = dynamic_cast<OverlayElement_Interface *>(pOverlay);
   if (pOverlayElement == NULL || pOverlayElement->SupportsFvOverlayElement() == FALSE)
   {
      INFO_report("CoverlayElement::RegisterForCallbacks failed - given overlay does not support IOverlayElement");
      return FAILURE;
   }

   m_smpCallback = IDispatchPtr(varDispatch.pdispVal, true);
	m_smpCallback2 = IDispatchPtr(varDispatch.pdispVal, true);

   return pOverlayElement->RegisterForCallbacks(varDispatch) == S_OK ? SUCCESS : FAILURE;
}

long COverlayElement::UpdateElement(long lOverlayHandle, LPCTSTR strElementXml)
{
   C_overlay *pOverlay = OVL_get_overlay_manager()->lookup_overlay(lOverlayHandle);
   if (pOverlay == NULL)
   {
      ERR_report("COverlayElement::UpdateElement failed - given overlay handle is invalid");
      return FAILURE;
   }

   OverlayElement_Interface *pOverlayElement = dynamic_cast<OverlayElement_Interface *>(pOverlay);
   if (pOverlayElement == NULL || pOverlayElement->SupportsFvOverlayElement() == FALSE)
   {
      ERR_report("CoverlayElement::UpdateElement failed - given overlay does not support IOverlayElement");
      return FAILURE;
   }

   return pOverlayElement->UpdateElement(_bstr_t(strElementXml)) == S_OK ? SUCCESS : FAILURE;
}