// Copyright (c) 1994-2009,2012 Georgia Tech Research Corporation, Atlanta, GA
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

// IUserSimulatorSetup.cpp
//

#include "StdAfx.h"
#include "IUserSimulator.h"
#include "IUserSimulatorSetup.h"

IMPLEMENT_DYNCREATE(UserSimulatorSetup, CCmdTarget)

// {8E48DBD3-1671-4ce9-B399-1A67BD598E60}
IMPLEMENT_OLECREATE( UserSimulatorSetup, "FalconView.UserSimulatorSetup", 
         0x8e48dbd3, 0x1671, 0x4ce9, 0xb3, 0x99, 0x1a, 0x67, 0xbd, 0x59, 0x8e, 0x60);


UserSimulatorSetup::UserSimulatorSetup()
{
   EnableAutomation();

}  // UserSimulatorSetup()


UserSimulatorSetup::~UserSimulatorSetup()
{
}

void UserSimulatorSetup::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(UserSimulatorSetup, CCmdTarget)
   //{{AFX_MSG_MAP(UserSimulatorSetup)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(UserSimulatorSetup, CCmdTarget)
   //{{AFX_DISPATCH_MAP(UserSimulatorSetup)
   DISP_FUNCTION( UserSimulatorSetup, "AddToolbarButtonLookup",
      AddToolbarButtonLookup, VT_I4, VTS_UI4 VTS_BSTR VTS_BSTR)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IUserSimulatorSetup to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IUserSimulatorSetup =
   { 0xFC6481D4, 0x7E7B, 0x4228, { 0xB0, 0x79, 0xf7, 0x51, 0xD1, 0x0A, 0xA5, 0x6A } };

BEGIN_INTERFACE_MAP(UserSimulatorSetup, CCmdTarget)
   INTERFACE_PART(UserSimulatorSetup, IID_IUserSimulatorSetup, Dispatch)
END_INTERFACE_MAP()



/////////////////////////////////////////////////////////////////////////////
// UserSimulatorSetup message handlers
/////////////////////////////////////////////////////////////////////////////

// Add FvToolbar button lookups
long UserSimulatorSetup::AddToolbarButtonLookup( __in UINT uiID,
                     __in LPCTSTR pszToolbarName, __in LPCTSTR pszHintText )
{
   UserSimulator::AddToolbarButtonLookup( uiID, pszToolbarName, pszHintText );
   return 0;
}


// End of IUserSimulatorSetup.cpp

