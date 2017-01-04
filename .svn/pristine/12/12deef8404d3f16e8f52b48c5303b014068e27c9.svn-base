// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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
//

#include "StdAfx.h"
#include "CustomToolbarMgrEventSink.h"
#include "getobjpr.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomToolbarMgrEventSink

IMPLEMENT_DYNCREATE(CCustomToolbarMgrEventSink, CCmdTarget)

CCustomToolbarMgrEventSink::CCustomToolbarMgrEventSink()
{
	EnableAutomation();
}

BEGIN_MESSAGE_MAP(CCustomToolbarMgrEventSink, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCustomToolbarMgrEventSink, CCmdTarget)
	DISP_FUNCTION(CCustomToolbarMgrEventSink, "OnHelp", OnHelp, VT_EMPTY,VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CCustomToolbarMgrEventSink, CCmdTarget)
	INTERFACE_PART(CCustomToolbarMgrEventSink, __uuidof(_ICustomToolbarMgrEvents), Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomToolbarMgrEventSink message handlers

// This event gets fired when the user clicks the Help button in the 
// custom tools manager dialog
void CCustomToolbarMgrEventSink::OnHelp()
{
	fvw_get_frame()->LaunchHtmlHelp(0, HELP_CONTEXT, "", "fvw.chm::/fvw_core.chm::/FVW/Custom_Tools_Manager.htm");
}