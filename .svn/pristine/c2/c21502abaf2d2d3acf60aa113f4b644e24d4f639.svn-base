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

// ILayer5.cpp
//

#include "StdAfx.h"
#include "ILayer5.h"
#include "mapview.h"
#include "getobjpr.h"
#include "mapx.h"
#include "utils.h"

IMPLEMENT_DYNCREATE(Layer5, CCmdTarget)

IMPLEMENT_OLECREATE(Layer5, "FalconView.Layer5", 
0x415BC6E0, 0x581B, 0x47e9, 0xAA, 0x9C, 0xC6, 0x43, 0x42, 0x24, 0x64, 0x0A);

Layer5::Layer5()
{
   EnableAutomation();
}

Layer5::~Layer5()
{
}

void Layer5::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(Layer5, CCmdTarget)
   //{{AFX_MSG_MAP(Layer5)
   // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Layer5, CCmdTarget)
   //{{AFX_DISPATCH_MAP(Layer5)
   DISP_FUNCTION(Layer5, "GetCurrentMapProj", GetCurrentMapProj, VT_I4, VTS_PDISPATCH)
   DISP_FUNCTION(Layer5, "GetFvOverlayManager", GetFvOverlayManager, VT_I4, VTS_PDISPATCH)
   DISP_FUNCTION(Layer5, "BringFvToForeground", BringFvToForeground, VT_I4, VTS_NONE)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ILayer5 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_ILayer5 =
{ 0xD99F1778, 0x9904, 0x4bb3, { 0xBF, 0x42, 0x1D, 0xC8, 0x8E, 0x5C, 0xF8, 0x27 } };

BEGIN_INTERFACE_MAP(Layer5, CCmdTarget)
   INTERFACE_PART(Layer5, IID_ILayer5, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ILayer5 method implementations

long Layer5::GetCurrentMapProj(IDispatch** ppDispatch)
{   
   *ppDispatch = NULL;

   MapView* pMapView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pMapView == NULL)
      return FAILURE;

   ViewMapProjImpl* pViewMapProj = pMapView->get_curr_map();
   if (pViewMapProj == NULL)
      return FAILURE;

   IDispatchPtr spDispatch = pViewMapProj->GetSettableMapProj();
   if (spDispatch == NULL)
      return FAILURE;

   *ppDispatch = spDispatch.Detach();

   return SUCCESS;
}


long Layer5::GetFvOverlayManager(IDispatch** ppDispatch)
{
   FalconViewOverlayLib::IFvOverlayManagerPtr spOverlayManager = OVL_get_overlay_manager()->GetFvOverlayManager();
   if (spOverlayManager != NULL)
   {
      IDispatchPtr spDispatch = spOverlayManager;
      if (spDispatch != NULL)
      {
         *ppDispatch = spDispatch.Detach();
         return SUCCESS;
      }
   }

   return FAILURE;
}

long Layer5::BringFvToForeground()
{
   CMainFrame* pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("Main frame window has not yet been created.  BringFvToForeground failed.");
      return FAILURE;
   }

   HWND hWnd = pFrame->GetSafeHwnd();

   // If FalconView is minimized, then it will need to be restored
   WINDOWPLACEMENT wndpl;
   ::GetWindowPlacement(hWnd, &wndpl);
   if (wndpl.showCmd == SW_SHOWMINIMIZED)
   {
      wndpl.showCmd = SW_NORMAL;
      ::SetWindowPlacement(hWnd, &wndpl);
   }

   ::SetForegroundWindow(hWnd);
   return SUCCESS;
}