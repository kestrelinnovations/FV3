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

#include "stdafx.h"
#include "TrackOrbitCommandMessageHandler.h"

#include "FalconView/include/ar_edit.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/mapview.h"
#include "FalconView/resource.h"

BEGIN_MESSAGE_MAP(TrackOrbitCommandMessageHandler, CCmdTarget)
   ON_COMMAND(ID_TOOL_AR_SELECT, OnArToolSelect)
   ON_UPDATE_COMMAND_UI(ID_TOOL_AR_SELECT, OnUpdateArToolSelect)
   ON_COMMAND(ID_TOOL_AR_TRACK, OnArToolTrack)
   ON_UPDATE_COMMAND_UI(ID_TOOL_AR_TRACK, OnUpdateArToolTrack)
   ON_COMMAND(ID_TOOL_AR_ANCHOR, OnArToolAnchor)
   ON_UPDATE_COMMAND_UI(ID_TOOL_AR_ANCHOR, OnUpdateArToolAnchor)
   ON_COMMAND(ID_TOOL_AR_NAVAID, OnArToolNavaid)
   ON_UPDATE_COMMAND_UI(ID_TOOL_AR_NAVAID, OnUpdateArToolNavaid)
   ON_COMMAND(ID_TOOL_AR_POINT, OnArToolPoint)
   ON_UPDATE_COMMAND_UI(ID_TOOL_AR_POINT, OnUpdateArToolPoint)
   ON_COMMAND(ID_AR_PROPERTY, OnArPropertyDialog)
   ON_UPDATE_COMMAND_UI(ID_AR_PROPERTY, OnUpdateArPropertyDialog)
END_MESSAGE_MAP()

void TrackOrbitCommandMessageHandler::OnArToolSelect()
{
   C_ar_edit::set_editor_mode(MM_AR_SELECT);
}

void TrackOrbitCommandMessageHandler::OnUpdateArToolSelect(CCmdUI* pCmdUI)
{
   // add check mark if in symbol sel mode
   pCmdUI->SetCheck(C_ar_edit::get_editor_mode() == MM_AR_SELECT);
}

void TrackOrbitCommandMessageHandler::OnArToolTrack()
{

   C_ar_edit::set_editor_mode(MM_AR_TRACK);
}

void TrackOrbitCommandMessageHandler::OnUpdateArToolTrack(CCmdUI* pCmdUI)
{
   // add check mark if in symbol sel mode
   pCmdUI->SetCheck(C_ar_edit::get_editor_mode() == MM_AR_TRACK);
}

void TrackOrbitCommandMessageHandler::OnArToolAnchor()
{
   C_ar_edit::set_editor_mode(MM_AR_ANCHOR);
}

void TrackOrbitCommandMessageHandler::OnUpdateArToolAnchor(CCmdUI* pCmdUI)
{
   // add check mark if in symbol sel mode
   pCmdUI->SetCheck(C_ar_edit::get_editor_mode() == MM_AR_ANCHOR);
}

void TrackOrbitCommandMessageHandler::OnArToolNavaid()
{
   C_ar_edit::set_editor_mode(MM_AR_NAVAID);
}

void TrackOrbitCommandMessageHandler::OnUpdateArToolNavaid(CCmdUI* pCmdUI)
{
   // add check mark if in symbol sel mode
   pCmdUI->SetCheck(C_ar_edit::get_editor_mode() == MM_AR_NAVAID);
}

void TrackOrbitCommandMessageHandler::OnArToolPoint()
{
   C_ar_edit::set_editor_mode(MM_AR_POINT);
}

void TrackOrbitCommandMessageHandler::OnUpdateArToolPoint(CCmdUI* pCmdUI)
{
   // add check mark if in symbol sel mode
   int rslt;
   C_ar_edit* ar_ovl;

   rslt = 0;
   ar_ovl = (C_ar_edit *)OVL_get_overlay_manager()->get_current_overlay();
   if (ar_ovl != NULL)
      rslt = ar_ovl->enable_property_dialog_button();

   switch(rslt)
   {
      case 0:  pCmdUI->Enable(FALSE);
// pCmdUI->SetCheck(FALSE);
               break;
      case 1:  pCmdUI->Enable(FALSE);
// pCmdUI->SetCheck(TRUE);
               break;
      case 2:  pCmdUI->Enable(TRUE);
// pCmdUI->SetCheck(FALSE);
               break;
   }

   pCmdUI->SetCheck(ar_ovl->get_editor_mode() == MM_AR_POINT);
}

void TrackOrbitCommandMessageHandler::OnArPropertyDialog()
{
   C_ar_edit* ovl;

   ovl = (C_ar_edit *)OVL_get_overlay_manager()->get_current_overlay();
   if (ovl != NULL)
      ovl->toggle_property_dialog();
}

void TrackOrbitCommandMessageHandler::OnUpdateArPropertyDialog(CCmdUI* pCmdUI)
{
   int rslt;
   C_ar_edit* ar_ovl;

   rslt = 0;
   ar_ovl = (C_ar_edit *)OVL_get_overlay_manager()->get_current_overlay();
   if (ar_ovl != NULL)
      rslt = ar_ovl->enable_property_dialog_button();

   pCmdUI->Enable(rslt != 0);
}
