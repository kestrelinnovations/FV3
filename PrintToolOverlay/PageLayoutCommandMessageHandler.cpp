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
#include "PageLayoutCommandMessageHandler.h"

#include "FalconView/getobjpr.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/PrntTool.h"
#include "FalconView/mapview.h"
#include "FalconView/PrintToolOverlay/factory.h"
#include "FalconView/resource.h"

BEGIN_MESSAGE_MAP(PageLayoutCommandMessageHandler, CCmdTarget)
   ON_COMMAND(ID_PRINTTOOL_SELECT, OnPrntToolSelect)
   ON_UPDATE_COMMAND_UI(ID_PRINTTOOL_SELECT, OnUpdatePrntToolSelect)
   ON_COMMAND(ID_PRINTTOOL_PAGE, OnPrntToolPage)
   ON_UPDATE_COMMAND_UI(ID_PRINTTOOL_PAGE, OnUpdatePrntToolPage)
   ON_COMMAND(ID_PRINTTOOL_AREA, OnPrntToolArea)
   ON_UPDATE_COMMAND_UI(ID_PRINTTOOL_AREA, OnUpdatePrntToolArea)
   ON_COMMAND(ID_PRINTTOOL_STRIP, OnPrntToolStrip)
   ON_COMMAND(ID_PRINTTOOL_SNAPROUTE, OnPrntToolSnapRoute)
   ON_UPDATE_COMMAND_UI(ID_PRINTTOOL_SNAPROUTE, OnUpdatePrntToolSnapRoute)
   ON_COMMAND(ID_PRINT_TOOL_PRINT_DIRECT, OnPrintToolPrintDirect)
   ON_COMMAND(ID_PRINT_TOOL_PRINT_PREVIEW, OnPrintToolPrintPreview)
   ON_UPDATE_COMMAND_UI(ID_PRINT_TOOL_PRINT_PREVIEW, OnUpdatePrintToolPrintPreview)
   ON_COMMAND(ID_PRINT_TOOL_PRINT, OnPrintToolPrint)
   ON_UPDATE_COMMAND_UI(ID_PRINT_TOOL_PRINT, OnUpdatePrintToolPrint)
   ON_COMMAND(ID_PRINTTOOL_ALIGN_LEG, OnPrintToolAlignLeg)
   ON_UPDATE_COMMAND_UI(ID_PRINTTOOL_ALIGN_LEG, OnUpdatePrintToolAlignLeg)
   ON_COMMAND(ID_PRINT_TOOL_PROPERTIES, OnPrintToolProperties)
END_MESSAGE_MAP()

void PageLayoutCommandMessageHandler::OnPrntToolSelect()
{
   CPrintToolOverlay::SetEditMode( CPrintToolOverlay::EDIT_MODE_SELECT );
}

void PageLayoutCommandMessageHandler::OnUpdatePrntToolSelect(CCmdUI* pCmdUI)
{
   bool bSetCheck =
      (CPrintToolOverlay::GetEditMode() == CPrintToolOverlay::EDIT_MODE_SELECT);

   pCmdUI->SetCheck( bSetCheck );
}

void PageLayoutCommandMessageHandler::OnPrntToolPage()
{
   CPrintToolOverlay::SetEditMode( CPrintToolOverlay::EDIT_MODE_PAGE );
}

void PageLayoutCommandMessageHandler::OnUpdatePrntToolPage(CCmdUI* pCmdUI)
{
   bool bSetCheck =
      (CPrintToolOverlay::GetEditMode() == CPrintToolOverlay::EDIT_MODE_PAGE);

   pCmdUI->SetCheck( bSetCheck );
}

void PageLayoutCommandMessageHandler::OnPrntToolArea()
{
   CPrintToolOverlay::SetEditMode( CPrintToolOverlay::EDIT_MODE_AREA );
}

void PageLayoutCommandMessageHandler::OnUpdatePrntToolArea(CCmdUI* pCmdUI)
{
   bool bSetCheck =
      (CPrintToolOverlay::GetEditMode() == CPrintToolOverlay::EDIT_MODE_AREA);

   pCmdUI->SetCheck( bSetCheck );
}

void PageLayoutCommandMessageHandler::OnPrntToolStrip()
{
   CPrintToolOverlay* pOvl = CPrintToolOverlay::GetActiveOverlay();
   pOvl->CreateStripCharts();
}

void PageLayoutCommandMessageHandler::OnPrintToolAlignLeg()
{
   CPrintToolOverlay::m_align_to_leg = !CPrintToolOverlay::m_align_to_leg;
}

void PageLayoutCommandMessageHandler::OnUpdatePrintToolAlignLeg(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(CPrintToolOverlay::m_align_to_leg);
}

void PageLayoutCommandMessageHandler::OnUpdatePrntToolSnapRoute(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(CPrintToolOverlay::m_snap_to_leg);
}

void PageLayoutCommandMessageHandler::OnPrntToolSnapRoute()
{
   CPrintToolOverlay::m_snap_to_leg = !CPrintToolOverlay::m_snap_to_leg;
}

void PageLayoutCommandMessageHandler::OnPrintToolPrintDirect()
{
   // a print action was initiated from the print tool bar
   CPrintToolOverlay::set_print_from_tool(TRUE);

   // do a print direct
   fvw_get_view()->SendMessage(WM_COMMAND, ID_FILE_PRINT_DIRECT, 0);
}

void PageLayoutCommandMessageHandler::OnPrintToolPrintPreview()
{
   // a print action was initiated from the print tool bar
   CPrintToolOverlay::set_print_from_tool(TRUE);

   // do a print preview
   fvw_get_view()->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW, 0);
}

void PageLayoutCommandMessageHandler::OnUpdatePrintToolPrintPreview(CCmdUI* pCmdUI)
{
   // Get the top most page layout overlay, NULL for none.
   CPrintToolOverlay *ovl = (CPrintToolOverlay*)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_PageLayout);

   // Enable if a non-empty page layout overlay is being edited.
   pCmdUI->Enable(ovl && ovl->IsEditing() && ovl->is_empty() == false);
}

void PageLayoutCommandMessageHandler::OnPrintToolPrint()
{
   // a print action was initiated from the print tool bar
   CPrintToolOverlay::set_print_from_tool(TRUE);

   // do a print
   fvw_get_view()->SendMessage(WM_COMMAND, ID_FILE_PRINT, 0);
}

void PageLayoutCommandMessageHandler::OnUpdatePrintToolPrint(CCmdUI* pCmdUI)
{
   // Get the top most page layout overlay, NULL for none.
   CPrintToolOverlay *ovl = (CPrintToolOverlay*)OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_PageLayout);

   // Enable if a non-empty page layout overlay is being edited.
   pCmdUI->Enable(ovl && ovl->IsEditing() && ovl->is_empty() == false);
}

void PageLayoutCommandMessageHandler::OnPrintToolProperties()
{
   CPrintToolOverlay* pOvl = CPrintToolOverlay::GetActiveOverlay();
   pOvl->DoProperties();
}
