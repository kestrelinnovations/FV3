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

// MapDataCoverageCommandMessageHandler.cpp

#include "stdafx.h"
#include "MapDataCoverageCommandMessageHandler.h"

#include "FalconView/catalog/Cov_ovl.h"
#include "FalconView/catalog/factory.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/mapview.h"
#include "FalconView/resource.h"

BEGIN_MESSAGE_MAP(MapDataCoverageCommandMessageHandler, CCmdTarget)
   ON_COMMAND(ID_MDM_TOOL_SELCOPY, OnMdmToolSelcopy)
   ON_COMMAND(ID_MDM_TOOL_SELDELETE, OnMdmToolSeldelete)
   ON_UPDATE_COMMAND_UI(ID_MDM_TOOL_SELDELETE, OnUpdateMdmToolSeldelete)
   ON_UPDATE_COMMAND_UI(ID_MDM_TOOL_SELCOPY, OnUpdateMdmToolSelcopy)
   ON_COMMAND(ID_MAP_DATAMANAGER, OnMapDataManager)
   ON_COMMAND(ID_MDM_TOOL_COMPATIBLE, OnMdmToolCompatible)
   ON_UPDATE_COMMAND_UI(ID_MDM_TOOL_COMPATIBLE, OnUpdateMdmToolCompatible)
END_MESSAGE_MAP()

void MapDataCoverageCommandMessageHandler::OnMapDataManager()
{
   OVL_get_overlay_manager()->toggle_editor(FVWID_Overlay_MapDataManager);
}

void MapDataCoverageCommandMessageHandler::OnMdmToolCompatible()
{
   CoverageOverlay* overlay = (CoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MapDataManager);

   if (overlay)
      overlay->SetSelectMode(COMPATIBLE_MODE);
}

void MapDataCoverageCommandMessageHandler::OnUpdateMdmToolCompatible(CCmdUI* pCmdUI)
{
   CoverageOverlay* overlay = (CoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MapDataManager);

   if (overlay)
      pCmdUI->SetCheck(overlay->GetSelectMode() == COMPATIBLE_MODE ? 1 : 0);
}

void MapDataCoverageCommandMessageHandler::OnMdmToolSelcopy()
{
   CoverageOverlay* overlay = (CoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MapDataManager);

   if (overlay)
      overlay->SetSelectMode(COPY_MODE);
}

void MapDataCoverageCommandMessageHandler::OnUpdateMdmToolSelcopy(CCmdUI* pCmdUI)
{
   CoverageOverlay* overlay = (CoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MapDataManager);

   if (overlay)
      pCmdUI->SetCheck(overlay->GetSelectMode() == COPY_MODE ? 1 : 0);
}

void MapDataCoverageCommandMessageHandler::OnMdmToolSeldelete()
{
   CoverageOverlay* overlay = (CoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MapDataManager);

   if (overlay)
      overlay->SetSelectMode(DELETE_MODE);
}

void MapDataCoverageCommandMessageHandler::OnUpdateMdmToolSeldelete(CCmdUI* pCmdUI)
{
   CoverageOverlay* overlay = (CoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MapDataManager);

   if (overlay)
      pCmdUI->SetCheck(overlay->GetSelectMode() == DELETE_MODE ? 1 : 0);
}
