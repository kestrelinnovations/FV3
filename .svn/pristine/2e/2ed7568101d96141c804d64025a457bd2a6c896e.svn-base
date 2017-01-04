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

// IOverlayElementEnumerator.cpp
//

#include "StdAfx.h"
#include "IOverlayElementEnumerator.h"
#include "err.h"        // for ERR_report
#include "overlay.h"

IMPLEMENT_DYNCREATE(COverlayElementEnumerator, CCmdTarget)

IMPLEMENT_OLECREATE(COverlayElementEnumerator, "FalconView.COverlayElementEnumerator", 0xA0905E2E, 0xC1F3, 0x41A7, 
                   0xA5, 0xC0, 0xB8, 0x8F, 0xBA, 0x61, 0xD2, 0x2F)

COverlayElementEnumerator::COverlayElementEnumerator() :
   m_pEnumerator(NULL)
{
   EnableAutomation();
}

COverlayElementEnumerator::~COverlayElementEnumerator()
{
}

void COverlayElementEnumerator::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(COverlayElementEnumerator, CCmdTarget)
   //{{AFX_MSG_MAP(COverlayElementEnumerator)
   // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COverlayElementEnumerator, CCmdTarget)
   //{{AFX_DISPATCH_MAP(COverlayElementEnumerator)
   DISP_PROPERTY(COverlayElementEnumerator, "bstrCurrentXml", m_strCurrentXml, VT_BSTR)
   DISP_FUNCTION(COverlayElementEnumerator, "Reset", Reset, VT_I4, VTS_I4)
   DISP_FUNCTION(COverlayElementEnumerator, "MoveNext", MoveNext, VT_I4, VTS_NONE)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IOverlayElementEnumerator to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IOverlayElementEnumerator =
{ 0x9E347D13, 0x69E7, 0x40f2, { 0x84, 0xB3, 0x80, 0xEB, 0xB6, 0x3B, 0x5F, 0xD1 } };

BEGIN_INTERFACE_MAP(COverlayElementEnumerator, CCmdTarget)
   INTERFACE_PART(COverlayElementEnumerator, IID_IOverlayElementEnumerator, Dispatch)
END_INTERFACE_MAP()

long COverlayElementEnumerator::Reset(long lOverlayHandle)
{
   m_pEnumerator = NULL;

   C_overlay *pOverlay = OVL_get_overlay_manager()->lookup_overlay(lOverlayHandle);
   if (pOverlay == NULL)
   {
      ERR_report("COverlayElementEnumerator::Reset failed - given overlay handle is invalid");
      return FAILURE;
   }

   OverlayElementEnumerator_Interface *pEnumerator = dynamic_cast<OverlayElementEnumerator_Interface *>(pOverlay);
   if (pEnumerator == NULL || pEnumerator->SupportsFvOverlayElementEnumerator() == FALSE)
   {
      ERR_report("COverlayElementEnumerator::Reset failed - given overlay does not support IOverlayElementEnumerator");
      return FAILURE;
   }

   m_pEnumerator = pEnumerator;
   return m_pEnumerator->Reset() == S_OK ? SUCCESS : FAILURE;
}

long COverlayElementEnumerator::MoveNext()
{
   if (m_pEnumerator == NULL)
   {
      ERR_report("CoverlayElementEnumerator::MoveNext failed - current "
         "enumerator invalid.  Call Reset first");
      return FAILURE;
   }

   long bRet;
   HRESULT hr = m_pEnumerator->MoveNext(&bRet);
   if (bRet == TRUE && hr == S_OK)
   {
      _bstr_t currentXml = m_pEnumerator->bstrCurrentXml();
      m_strCurrentXml = (char *)currentXml;
   }
   else
      m_pEnumerator = NULL;

   return bRet ? SUCCESS : FAILURE;
}