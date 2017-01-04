// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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


// stdafx first
#include "stdafx.h" 

// this file's header
#include "SMCommandMessageHandler.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/mapview.h"
#include "FalconView/resource.h"

// this project's headers
#include "scene_cov_ovl.h"
#include "scene_mgr.h"
#include "sm_factory.h"


namespace scene_mgr
{

BEGIN_MESSAGE_MAP(SMCommandMessageHandler, CCmdTarget)
   ON_COMMAND(ID_SM_TOOL_SELCOPY, OnSmToolSelcopy)
   ON_COMMAND(ID_SM_TOOL_SELDELETE, OnSmToolSeldelete)
   ON_UPDATE_COMMAND_UI(ID_SM_TOOL_SELDELETE, OnUpdateSmToolSeldelete)
   ON_UPDATE_COMMAND_UI(ID_SM_TOOL_SELCOPY, OnUpdateSmToolSelcopy)
   ON_COMMAND(ID_SCENE_DATA_MANAGER, OnSceneManager)
   ON_COMMAND(ID_SM_TOOL_COMPATIBLE, OnSmToolCompatible)
   ON_UPDATE_COMMAND_UI(ID_SM_TOOL_COMPATIBLE, OnUpdateSmToolCompatible)
END_MESSAGE_MAP()

void SMCommandMessageHandler::OnSceneManager()
{
   OVL_get_overlay_manager()->toggle_editor(FVWID_Overlay_SceneManager);
}

void SMCommandMessageHandler::OnSmToolCompatible()
{
   SceneCoverageOverlay* overlay = (SceneCoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SceneManager);

   if (overlay)
      overlay->SetSelectMode(SM_COMPATIBLE_MODE);
}

void SMCommandMessageHandler::OnUpdateSmToolCompatible(CCmdUI* pCmdUI)
{
   SceneCoverageOverlay* overlay = (SceneCoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SceneManager);

   if (overlay)
      pCmdUI->SetCheck(overlay->GetSelectMode() == SM_COMPATIBLE_MODE ? 1 : 0);
}

void SMCommandMessageHandler::OnSmToolSelcopy()
{
   SceneCoverageOverlay* overlay = (SceneCoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SceneManager);

   if (overlay)
      overlay->SetSelectMode(SM_COPY_MODE);
}

void SMCommandMessageHandler::OnUpdateSmToolSelcopy(CCmdUI* pCmdUI)
{
   SceneCoverageOverlay* overlay = (SceneCoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SceneManager);

   if (overlay)
      pCmdUI->SetCheck(overlay->GetSelectMode() == SM_COPY_MODE ? 1 : 0);
}

void SMCommandMessageHandler::OnSmToolSeldelete()
{
   SceneCoverageOverlay* overlay = (SceneCoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SceneManager);

   if (overlay)
      overlay->SetSelectMode(SM_DELETE_MODE);
}

void SMCommandMessageHandler::OnUpdateSmToolSeldelete(CCmdUI* pCmdUI)
{
   SceneCoverageOverlay* overlay = (SceneCoverageOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SceneManager);

   if (overlay)
      pCmdUI->SetCheck(overlay->GetSelectMode() == SM_DELETE_MODE ? 1 : 0);
}

};  // namespace scene_mgr
