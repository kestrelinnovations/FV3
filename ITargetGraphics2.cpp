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

// ITargetGraphics2.cpp
//

#include "StdAfx.h"
#include "ITargetGraphics2.h"

#include "err.h"
#include "param.h"

#include "tabulareditor\targetgraphics2dlg.h"

IMPLEMENT_DYNCREATE(TargetGraphics2, CCmdTarget)

IMPLEMENT_OLECREATE(TargetGraphics2, "FalconView.TargetGraphics2", 0x5A02BA32, 0x37A5, 0x4A34, 
0x90, 0x87, 0x79, 0x24, 0x5E, 0xB1, 0x59, 0xFA)

TargetGraphics2::TargetGraphics2()
{
	EnableAutomation();
}

TargetGraphics2::~TargetGraphics2()
{
}

void TargetGraphics2::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(TargetGraphics2, CCmdTarget)
	//{{AFX_MSG_MAP(TargetGraphics)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(TargetGraphics2, CCmdTarget)
	//{{AFX_DISPATCH_MAP(TargetGraphics2)
	DISP_FUNCTION(TargetGraphics2, "AddTarget",AddTarget, VT_I4, VTS_DISPATCH VTS_DISPATCH)
	DISP_FUNCTION(TargetGraphics2, "DoModal",DoModal, VT_I4, VTS_PI4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ITargetGraphics2 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_ITargetGraphics2 =
{ 0x640AAE70, 0x210C, 0x4F21, { 0xB6, 0x7E, 0x95, 0x40, 0x98, 0xCC, 0x6E, 0xA4 } };

BEGIN_INTERFACE_MAP(TargetGraphics2, CCmdTarget)
	INTERFACE_PART(TargetGraphics2, IID_ITargetGraphics2, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TargetGraphics message handlers

long TargetGraphics2::AddTarget(IDispatch *iFvDataSource, IDispatch *iFeature)
{
	IFvDataSourcePtr pFvDataSource;
	IFeaturePtr pFeature;
	iFvDataSource->QueryInterface(IID_IFvDataSource, (void**)&pFvDataSource);
	iFeature->QueryInterface(IID_IFeature, (void**)&pFeature);

	feature_t feature;
	feature.dataSource = pFvDataSource;
	feature.feature = pFeature;

	m_feature_list.AddTail(feature);

	return SUCCESS;
}

long TargetGraphics2::DoModal(long *result)
{
	CTargetGraphics2Dlg target_dlg;
	target_dlg.set_selected_list(&m_feature_list);
	*result = target_dlg.DoModal();

	m_feature_list.RemoveAll();

	return SUCCESS;
}
