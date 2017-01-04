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

// PointsCommandMessageHandler.cpp

#include "stdafx.h"
#include "PointsCommandMessageHandler.h"

#include "FalconView/include/IconDlg.h"
#include "FalconView/include/map.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/TabularEditorDlg.h"
#include "FalconView/include/utils.h"
#include "FalconView/include/wm_user.h"
#include "FalconView/localpnt/localpnt.h"
#include "FalconView/localpnt/lpeddlgc.h"
#include "FalconView/localpnt/factory.h"
#include "FalconView/mapview.h"
#include "FalconView/resource.h"

BEGIN_MESSAGE_MAP(PointsCommandMessageHandler, CCmdTarget)
   ON_COMMAND(ID_LOCAL_ADD, OnLocalPtAdd)
   ON_UPDATE_COMMAND_UI(ID_LOCAL_ADD, OnUpdateLocalPtAdd)
   ON_COMMAND(ID_LOCAL_SELECT, OnLocalPtSelect)
   ON_UPDATE_COMMAND_UI(ID_LOCAL_SELECT, OnUpdateLocalPtSelect)
   ON_COMMAND(ID_LOCAL_EDITOR_DIALOG_TOGGLE, OnLocalEditorDialogToggle)
   ON_UPDATE_COMMAND_UI(ID_LOCAL_EDITOR_DIALOG_TOGGLE, OnUpdateLocalEditorDialogToggle)
   ON_COMMAND(ID_LOCAL_DRAG_LOCK, OnLocalDragLock)
   ON_UPDATE_COMMAND_UI(ID_LOCAL_DRAG_LOCK, OnUpdateLocalDragLock)
   ON_COMMAND(ID_LOCAL_PT_TABULAR_EDITOR, OnLocalEditorTabularEditor)
   ON_UPDATE_COMMAND_UI(ID_LOCAL_PT_TABULAR_EDITOR, OnUpdateLocalEditorTabularEditor)
   ON_COMMAND(ID_DISPLAY_LOCALPNT_ICON_DLG, OnDisplayLocalPointIconDialog)
END_MESSAGE_MAP()

void PointsCommandMessageHandler::OnLocalPtAdd()
{
   C_localpnt_ovl::set_editor_mode(C_localpnt_ovl::ADD);
}

// ----------------------------------------------------------------------------

void PointsCommandMessageHandler::OnUpdateLocalPtAdd(CCmdUI* pCmdUI)
{
   if (C_localpnt_ovl::get_editor_mode() == C_localpnt_ovl::ADD)
   {
      // The only way to get a NON-NULL and a NOT visible icon palette
      // is if the user clicked the close (X) box on the palette.
      // This action should be (from user's perspective) the same
      // as going from Icon mode to Select mode, so force it.

      if ((CIconDragDlg::get_dlg() != NULL) && (!CIconDragDlg::get_dlg()->IsWindowVisible()))
         C_localpnt_ovl::set_editor_mode(C_localpnt_ovl::SELECT);
   }

   pCmdUI->SetRadio(C_localpnt_ovl::get_editor_mode() == C_localpnt_ovl::ADD);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void PointsCommandMessageHandler::OnLocalPtSelect()
{
   C_localpnt_ovl::set_editor_mode(C_localpnt_ovl::SELECT);
}

void PointsCommandMessageHandler::OnUpdateLocalPtSelect(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(C_localpnt_ovl::get_editor_mode() == C_localpnt_ovl::SELECT);
}

// toggle the local point editor dialog and set the edit focus to
// the topmost local point overlay, if we turned "on" the editor dialog
void PointsCommandMessageHandler::OnLocalEditorDialogToggle()
{
   C_localpnt_ovl* overlay;

   //Get the topmost local point overlay
   overlay = (C_localpnt_ovl*) OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_Points);

   ASSERT(overlay);  //overlay should exist when this button is hit

   if (!C_localpnt_ovl::m_EditDialogControl.DialogExists())  //if is the editor is not up...
   {
      //turn the editor dialog "on" and set the edit focus to the topmost overlay
      if (overlay != NULL)
         overlay->open_edit_dialog_and_set_focus_to_current_selection_if_any();
   }
   else
   {
      //TO DO: ask if the person wants to apply the changes in the edit dialog
      C_localpnt_ovl::m_EditDialogControl.ApplyChanges(TRUE);
      C_localpnt_ovl::m_EditDialogControl.DestroyDialog();
   }
}

void PointsCommandMessageHandler::OnUpdateLocalEditorDialogToggle(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck((int) C_localpnt_ovl::m_EditDialogControl.DialogExists());
}

void PointsCommandMessageHandler::OnLocalDragLock()
{
   C_localpnt_ovl* overlay = dynamic_cast<C_localpnt_ovl *>(OVL_get_overlay_manager()->get_current_overlay());
   ASSERT(overlay);

   // Toggle drag lock state.
   if (overlay)
      overlay->set_drag_lock_state(!overlay->get_drag_lock_state());
}

void PointsCommandMessageHandler::OnUpdateLocalDragLock(CCmdUI* pCmdUI)
{
   C_localpnt_ovl* overlay = dynamic_cast<C_localpnt_ovl *>(OVL_get_overlay_manager()->get_current_overlay());

   if (overlay)
      pCmdUI->SetCheck(overlay->get_drag_lock_state());
}

void PointsCommandMessageHandler::OnLocalEditorTabularEditor()
{
   C_localpnt_ovl* overlay;

   //Get the topmost local point overlay
   overlay = (C_localpnt_ovl*) OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_Points);

   ASSERT(overlay);

   overlay->OnTabularEditor();
}

void PointsCommandMessageHandler::OnUpdateLocalEditorTabularEditor(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(g_tabular_editor_dlg != NULL);
}

void PointsCommandMessageHandler::OnDisplayLocalPointIconDialog()
{
   boolean_t       show_dlg = FALSE;
   C_localpnt_ovl* overlay  = NULL;

   ViewMapProj* map  = UTL_get_current_view_map();
   if (!map)
      return;

   overlay = (C_localpnt_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Points);

   // see if we can display the icon dialog at this scale
   if (overlay)
      show_dlg = overlay->get_display_threshold() <= map->scale();

   if (show_dlg)
   {
      if (CIconDragDlg::get_dlg() == NULL)
      {
         CIconDragDlg* icon_dlg = new CIconDragDlg("Point Icons");
         icon_dlg->set_dlg(icon_dlg);
         icon_dlg->load_dir("localpnt");
         icon_dlg->sort_icons();
      }
      else if (!CIconDragDlg::get_dlg()->IsWindowVisible())
         CIconDragDlg::get_dlg()->ShowWindow(SW_SHOW);
   }
   else
   {
      // warn user that he/she can't draw things at this map scale
      CString msg;
      AfxFormatString2(msg, IDS_EDIT_TOOLS_NOT_SUPPORTED_AT_THIS_SCALE, "Point", "Points");
      AfxMessageBox(msg);

      OVL_get_overlay_manager()->overlay_options_dialog(FVWID_Overlay_Points);

      overlay->set_editor_mode(C_localpnt_ovl::SELECT);
   }
}