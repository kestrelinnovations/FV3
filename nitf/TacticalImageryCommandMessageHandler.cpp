// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// TacticalImageryCommandMessageHandler.cpp

#include "stdafx.h"
#include "TacticalImageryCommandMessageHandler.h"

#include "FalconView/include/nitf.h"
#include "FalconView/mapview.h"
#include "FalconView/resource.h"

BEGIN_MESSAGE_MAP(TacticalImageryCommandMessageHandler, CCmdTarget)
   ON_COMMAND_RANGE(ID_NITF_TOOLBAR_BEGIN, ID_NITF_TOOLBAR_END, OnNITFToolbarCmd)
   ON_UPDATE_COMMAND_UI_RANGE(ID_NITF_TOOLBAR_BEGIN, ID_NITF_TOOLBAR_END, OnNITFToolbarUpdateCmdUI)
END_MESSAGE_MAP()

void TacticalImageryCommandMessageHandler::OnNITFToolbarCmd( UINT nID )
{
   C_nitf_ovl::s_pNITFOvl->OnToolbarCmd( nID );
}

void TacticalImageryCommandMessageHandler::OnNITFToolbarUpdateCmdUI( CCmdUI* pCmdUI )
{
   if (C_nitf_ovl::s_pNITFOvl)
      C_nitf_ovl::s_pNITFOvl->OnToolbarUpdateCmdUI( pCmdUI );
}
