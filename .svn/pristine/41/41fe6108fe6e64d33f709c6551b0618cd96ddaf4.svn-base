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

// IAddToFavorites.cpp
//

#include "StdAfx.h"
#include "resource.h"
#include "IAddToFavorites.h"

#include "err.h"
#include "param.h"

#include "tabulareditor\FavoriteImportDlg.h"

IMPLEMENT_DYNCREATE(AddToFavorites, CCmdTarget)

IMPLEMENT_OLECREATE(AddToFavorites, "FalconView.AddToFavorites", 0xDA6500A0, 0x4839, 0x41DC, 
0xB7, 0xCA, 0xB7, 0x05, 0x73, 0xA6, 0x55, 0xFC)

AddToFavorites::AddToFavorites()
{
	EnableAutomation();
}

AddToFavorites::~AddToFavorites()
{
}

void AddToFavorites::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(AddToFavorites, CCmdTarget)
	//{{AFX_MSG_MAP(AddToFavorites)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(AddToFavorites, CCmdTarget)
	//{{AFX_DISPATCH_MAP(AddToFavorites)
	DISP_FUNCTION(AddToFavorites, "AddFavorite",AddFavorite, VT_I4, VTS_R8 VTS_R8 VTS_BSTR)
	DISP_FUNCTION(AddToFavorites, "DoModal",DoModal, VT_I4, VTS_PI4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAddToFavorites to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IAddToFavorites =
{ 0x85650D5E, 0xB4CE, 0x404F, { 0xAA, 0xD6, 0x31, 0xF0, 0x08, 0xB7, 0x03, 0xD5 } };

BEGIN_INTERFACE_MAP(AddToFavorites, CCmdTarget)
	INTERFACE_PART(AddToFavorites, IID_IAddToFavorites, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddToFavorites message handlers

long AddToFavorites::AddFavorite(double lat, double lon, LPCTSTR name)
{
	d_geo_t location;
	location.lat = lat;
	location.lon = lon;
	m_location_list.AddTail(location);
	m_name_list.AddTail(name);

	return SUCCESS;
}

long AddToFavorites::DoModal(long *result)
{
	CFavoriteImportDlg favorite_dlg;
	favorite_dlg.set_location_list(&m_location_list);
	favorite_dlg.set_name_list(&m_name_list);
	*result = favorite_dlg.DoModal();

	m_location_list.RemoveAll();
	m_name_list.RemoveAll();

	return SUCCESS;
}
