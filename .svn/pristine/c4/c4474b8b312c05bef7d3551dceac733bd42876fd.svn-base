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
#include "SkyViewCommandMessageHandler.h"

#include "FalconView/include/IconDlg.h"
#include "FalconView/include/map.h"
#include "FalconView/include/mov_sym.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/utils.h"
#include "FalconView/mapview.h"
#include "FalconView/resource.h"
#include "FalconView/SkyViewOverlay/factory.h"

BEGIN_MESSAGE_MAP(SkyViewCommandMessageHandler, CCmdTarget)
   ON_COMMAND(ID_SELECT_SYM_TOOL, OnSymTool)
   ON_UPDATE_COMMAND_UI(ID_SELECT_SYM_TOOL, OnUpdateSymTool)
   ON_COMMAND(ID_SKYVIEW, OnSkyview)
   ON_COMMAND(ID_CENTER_SYMBOL, OnSkyviewCenterSymbols)
   ON_COMMAND(ID_SKYVIEW_OBJECTS, OnSkyviewObjects)
   ON_COMMAND(ID_SKYVIEW_OBJ_PROPERTIES, OnSkyviewObjectProperties)
   ON_UPDATE_COMMAND_UI(ID_SKYVIEW_OBJ_PROPERTIES, OnUpdateSkyviewObjectProperties)
   ON_UPDATE_COMMAND_UI(ID_SKYVIEW, OnUpdateSkyview)
END_MESSAGE_MAP()

void SkyViewCommandMessageHandler::OnSymTool()
{
   Cmov_sym_overlay::set_editor_mode(Cmov_sym_overlay::SELECT_SYMBOL_MODE);
}

void SkyViewCommandMessageHandler::OnUpdateSymTool(CCmdUI* pCmdUI)
{
   // add check mark if in symbol sel mode
   pCmdUI->SetCheck(Cmov_sym_overlay::get_editor_mode() == Cmov_sym_overlay::SELECT_SYMBOL_MODE);
}


void SkyViewCommandMessageHandler::OnSkyview()
{
   Cmov_sym_overlay::open_skyview();
}

void SkyViewCommandMessageHandler::OnUpdateSkyview(CCmdUI* pCmdUI)
{
   Cmov_sym_overlay::OnUpdateSkyview(pCmdUI);
}

void SkyViewCommandMessageHandler::OnSkyviewCenterSymbols()
{
   ViewMapProj* map = UTL_get_current_view_map();
   if (!map)
      return;

   Cmov_sym_overlay::center_symbols(map);
}

void SkyViewCommandMessageHandler::OnSkyviewObjects()
{
   ViewMapProj* map = UTL_get_current_view_map();
   if (!map)
      return;

   Cmov_sym_overlay::skyview_object_dlg(map);
}

void SkyViewCommandMessageHandler::OnSkyviewObjectProperties()
{
   Cmov_sym_overlay* overlay;
   
   // get the topmost skyview overlay
   overlay = (Cmov_sym_overlay*) OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_SkyView);

   // if is the editor is not up...
   if (!Cmov_sym_overlay::m_obj_properties_dlg.m_hWnd)
   {
      // turn the editor dialog "on" and set the edit focus to the current
      // selection the topmost overlay (topmost overlay was found above)
      Cmov_sym_overlay::m_obj_properties_dlg.Create(ObjectDlg::IDD);

      if (overlay != NULL)
      {
         // Note: it's OK if current_selection is NULL
         Cmov_sym_overlay::m_obj_properties_dlg.set_focus(overlay->get_current_selection());
      }

      Cmov_sym_overlay::m_obj_properties_dlg.ShowWindow(SW_SHOW);
   }
   else
   {
      // force to save changes
      Cmov_sym_overlay::m_obj_properties_dlg.set_focus(NULL);

      // destroy the window
      Cmov_sym_overlay::m_obj_properties_dlg.DestroyWindow();
   }
}

void SkyViewCommandMessageHandler::OnUpdateSkyviewObjectProperties(CCmdUI* pCmdUI)
{
   if ((CIconDragDlg::get_dlg() != NULL) && (!CIconDragDlg::get_dlg()->IsWindowVisible()))
      CIconDragDlg::get_dlg()->close_dlg();

   pCmdUI->SetCheck(Cmov_sym_overlay::m_obj_properties_dlg.m_hWnd ? 1 : 0);
}