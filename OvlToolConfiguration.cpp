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



// OvlToolConfiguration.cpp : implementation of the COvlToolConfiguration class
//
////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "mapx.h"
#include "OvlToolConfiguration.h"



COvlToolConfiguration::COvlToolConfiguration(void)
{
   // create com component that is responsible for reading this file
   m_CustomOverlays.CreateInstance(CLSID_FvOverlayToolConfiguration);
   //m_CustomOverlays->Init
}

COvlToolConfiguration::~COvlToolConfiguration(void)
{
}


// This method compares the passed-in overlay/tool title name with the list of titles
// from the pfps\falcon\ovltoolconfig.txt file.  If a match is found, the method returns
// FALSE to indicate that this overlay/tool should be excluded from FalconView. 
BOOL COvlToolConfiguration::include_overlay_tool(CString name)
{
	BSTR bname(name.AllocSysString());
	BOOL ret = m_CustomOverlays->IncludeOverlayTool(&bname);
	::SysFreeString(bname);
	return ret;
}


int COvlToolConfiguration::create_exclusion_list()
{
	// call into com component
	HRESULT hr = m_CustomOverlays->ClearExclusionList();
	if(hr == S_OK)
	{
		return SUCCESS;
	}
	else
	{
		return FAILURE;
	}
}