// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// IMap2.cpp
//

#include "StdAfx.h"
#include "Imap2.h"	

#include "geo_tool.h"	// for GEO_valid_degrees


IMPLEMENT_DYNCREATE(Map2, CCmdTarget)

IMPLEMENT_OLECREATE(Map2, "FalconView.Map2", 0x8736CF90, 0x83B5, 0x49df, 
                    0xA4, 0x8C, 0x81, 0x5B, 0x79, 0x5F, 0x22, 0xBD)

Map2::Map2()
{
	EnableAutomation();
}

Map2::~Map2()
{
}

void Map2::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(Map2, CCmdTarget)
	//{{AFX_MSG_MAP(Map2)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Map2, CCmdTarget)
	//{{AFX_DISPATCH_MAP(Map2)
	DISP_FUNCTION(Map2, "GetPrimaryDatum", GetPrimaryDatum, VT_I4, VTS_PBSTR)
	DISP_FUNCTION(Map2, "GetSecondaryDatum", GetSecondaryDatum, VT_I4, VTS_PBSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMap2 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IMap2 =
{ 0x773FD23F, 0x43C9, 0x4F2B, { 0x8B, 0x51, 0x8F, 0x61, 0xDF, 0x79, 0x8F, 0x97 } };

BEGIN_INTERFACE_MAP(Map2, CCmdTarget)
	INTERFACE_PART(Map2, IID_IMap2, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Map message handlers

long Map2::GetPrimaryDatum(BSTR FAR* datum)
{
   const int DATUM_LEN = 10;
	char tmp[DATUM_LEN];
	GEO_get_primary_datum(tmp, DATUM_LEN);

   *datum = _bstr_t(tmp).Detach();

	return SUCCESS;
}

long Map2::GetSecondaryDatum(BSTR FAR* datum)
{
   const int DATUM_LEN = 10;
	char tmp[DATUM_LEN];
	GEO_get_secondary_datum(tmp, DATUM_LEN);
	
   *datum = _bstr_t(tmp).Detach();

	return SUCCESS;
}
