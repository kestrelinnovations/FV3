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

// Analysis.cpp : implementation file
//

#include "stdafx.h"
#include "Analysis.h"
#include "CCallback2.h"
#include "err.h"

/////////////////////////////////////////////////////////////////////////////
// Analysis

IMPLEMENT_DYNCREATE(Analysis, CCmdTarget)

Analysis::Analysis() 
{
	EnableAutomation();

	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	//AfxOleLockApp();

   m_pDispatch = NULL;
}

Analysis::~Analysis()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	//AfxOleUnlockApp();
}


void Analysis::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(Analysis, CCmdTarget)
	//{{AFX_MSG_MAP(Analysis)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Analysis, CCmdTarget)
	//{{AFX_DISPATCH_MAP(Analysis)
		DISP_FUNCTION(Analysis, "CreateMask", CreateMask, VT_I4, VTS_BSTR VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4)
		DISP_FUNCTION(Analysis, "RegisterWithMaskServer", RegisterWithMaskServer, VT_I4, VTS_VARIANT)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAnalysis to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.


//	 E7AC544F-5899-4DEE-944E-18E419F9A564
static const IID IID_IAnalysis =
{ 0xe7ac544f, 0x5899, 0x4dee, { 0x94, 0x4e, 0x18, 0xe4, 0x19, 0xf9, 0xa5, 0x64 } };

// {F3F7136D-59B2-449C-84D4-149973768F0F}
// static const IID IID_IAnalysis =
// { 0xf3f7136d, 0x59b2, 0x449c, { 0x84, 0xd4, 0x14, 0x99, 0x73, 0x76, 0x8f, 0xf } };

BEGIN_INTERFACE_MAP(Analysis, CCmdTarget)
	INTERFACE_PART(Analysis, IID_IAnalysis, Dispatch)
END_INTERFACE_MAP()

// {5E7EBE32-9D0E-473A-932F-9A3BEFD860D2}
IMPLEMENT_OLECREATE(Analysis, "FalconView.Analysis", 0x5e7ebe32, 0x9d0e, 0x473a, 0x93, 0x2f, 0x9a, 0x3b, 0xef, 0xd8, 0x60, 0xd2)

/////////////////////////////////////////////////////////////////////////////
// Analysis message handlers

long Analysis::CreateMask(LPCSTR name,
								  double lat, 
								  double lon, 
								  double antenna_height, 
								  double dted_spacing, 
								  double sweep_spacing, 
								  double range,
								  BOOL bInvertMask) 
{
	CString sMaskFile;
   sMaskFile.Format("%s %f %f %f %f %f %f %d",
      name, lat, lon, antenna_height, dted_spacing, sweep_spacing, range, bInvertMask );

   try
   {
      fvw::ICallback2Ptr smpCallback = m_pDispatch;
      if ( smpCallback )
      {
         return smpCallback->MaskComplete( _bstr_t(sMaskFile), lat, lon ) == S_OK ?
            SUCCESS : FAILURE;
      }
      else
      {
         CCallback2 callback2Impl(m_pDispatch);
         callback2Impl.MaskComplete(_bstr_t(sMaskFile), lat, lon);
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("MaskComplete failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_ole_exception("MaskComplete failed", *e);
      e->Delete();
   }
	
	return FAILURE;
}

long Analysis::RegisterWithMaskServer(const VARIANT FAR& dispatch_pointer)
{
   try
   {
      // get dispatch interface
      IDispatch* pDisp = _variant_t(dispatch_pointer);
      if (pDisp == NULL)
         return FAILURE;

      // if the interface is the same, then release the reference just added
      if (pDisp == m_pDispatch)
         pDisp->Release();

      // otherwise, release the current dispatch interface if it is non-null
      else if (m_pDispatch != NULL)
         m_pDispatch->Release();

      // set current dispatch
      m_pDispatch = pDisp;

      return SUCCESS;
   }
   catch (_com_error &)
   {
      return FAILURE;
   }
}


