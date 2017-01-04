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

// ITargetGraphics.cpp
//

#include "StdAfx.h"
#include "ITargetGraphics.h"

#include "err.h"
#include "param.h"

#include "tabulareditor\targetgraphicsdlg.h"

IMPLEMENT_DYNCREATE(TargetGraphics, CCmdTarget)

IMPLEMENT_OLECREATE(TargetGraphics, "FalconView.TargetGraphics", 0x1B2E183C, 0xCD09, 0x46b4, 
0x9E, 0xB3, 0x83, 0x4A, 0x0B, 0x8E, 0xE5, 0x1F)

TargetGraphics::TargetGraphics()
{
	EnableAutomation();
}

TargetGraphics::~TargetGraphics()
{
}

void TargetGraphics::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(TargetGraphics, CCmdTarget)
	//{{AFX_MSG_MAP(TargetGraphics)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(TargetGraphics, CCmdTarget)
	//{{AFX_DISPATCH_MAP(TargetGraphics)
	DISP_FUNCTION(TargetGraphics, "AddTarget",AddTarget, VT_I4, VTS_R8 VTS_R8 VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(TargetGraphics, "DoModal",DoModal, VT_I4, VTS_PI4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ITargetGraphics to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_ITargetGraphics =
{ 0xB2E86191, 0xC18B, 0x4d35, { 0xA9, 0x84, 0xB8, 0x87, 0x36, 0x00, 0xC7, 0xE6 } };

BEGIN_INTERFACE_MAP(TargetGraphics, CCmdTarget)
	INTERFACE_PART(TargetGraphics, IID_ITargetGraphics, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TargetGraphics message handlers

long TargetGraphics::AddTarget(double lat, double lon, LPCTSTR id, LPCTSTR description, LPCTSTR comment)
{
	target_t target;

	target.lat = lat;
	target.lon = lon;
	target.id = id;
	target.description = description;
	target.comment = comment;

	m_target_list.AddTail(target);

	return SUCCESS;
}

long TargetGraphics::DoModal(long *result)
{
	CTargetGraphicsDlg target_dlg;
	target_dlg.set_selected_list(&m_target_list);
	*result = target_dlg.DoModal();

	m_target_list.RemoveAll();

	return SUCCESS;
}
