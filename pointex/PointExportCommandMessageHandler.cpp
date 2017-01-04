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
#include "PointExportCommandMessageHandler.h"

#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/PntExprt.h"
#include "FalconView/mapview.h"
#include "FalconView/pointex/factory.h"
#include "FalconView/resource.h"

BEGIN_MESSAGE_MAP(PointExportCommandMessageHandler, CCmdTarget)
   ON_COMMAND(ID_EXPORT_POINT, OnExportPoint)
   ON_UPDATE_COMMAND_UI(ID_EXPORT_POINT, OnUpdateExportPoint)
   ON_COMMAND(ID_EXPORT_CIRCLE, OnExportCircle)
   ON_UPDATE_COMMAND_UI(ID_EXPORT_CIRCLE, OnUpdateExportCircle)
   ON_COMMAND(ID_EXPORT_PROP, OnExportProp)
   ON_UPDATE_COMMAND_UI(ID_EXPORT_PROP, OnUpdateExportProp)
   ON_COMMAND(ID_EXPORT_LINE, OnExportLine)
   ON_UPDATE_COMMAND_UI(ID_EXPORT_LINE, OnUpdateExportLine)
   ON_COMMAND(ID_EXPORT_RECT, OnExportRect)
   ON_UPDATE_COMMAND_UI(ID_EXPORT_RECT, OnUpdateExportRect)
   ON_COMMAND(ID_EXPORT_ROUTE, OnExportRoute)
   ON_UPDATE_COMMAND_UI(ID_EXPORT_ROUTE, OnUpdateExportRoute)
   ON_COMMAND(ID_EXPORT_SELECT, OnExportSelect)
   ON_UPDATE_COMMAND_UI(ID_EXPORT_SELECT, OnUpdateExportSelect)
END_MESSAGE_MAP()

void PointExportCommandMessageHandler::OnExportPoint()
{
   CPointExportOverlay::set_editor_mode(CPointExportOverlay::POINT_TOOL);
}

void PointExportCommandMessageHandler::OnUpdateExportPoint(CCmdUI* pCmdUI)
{
   // Not implemented yet
   pCmdUI->SetCheck(CPointExportOverlay::get_editor_mode() ==
      CPointExportOverlay::POINT_TOOL);
}

void PointExportCommandMessageHandler::OnExportCircle()
{
   CPointExportOverlay::set_editor_mode(CPointExportOverlay::CIRCLE_TOOL);
}

void PointExportCommandMessageHandler::OnUpdateExportCircle(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(CPointExportOverlay::get_editor_mode() ==
      CPointExportOverlay::CIRCLE_TOOL);
}

void PointExportCommandMessageHandler::OnExportProp()
{
   // get the topmost point export overlay
   CPointExportOverlay *overlay;

   overlay = (CPointExportOverlay*) OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_PointExport);


   if (overlay)
      overlay->display_properties();
}

void PointExportCommandMessageHandler::OnUpdateExportProp(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(FALSE);
}

void PointExportCommandMessageHandler::OnExportLine()
{
   CPointExportOverlay::set_editor_mode(CPointExportOverlay::LINE_TOOL);
}

void PointExportCommandMessageHandler::OnUpdateExportLine(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(CPointExportOverlay::get_editor_mode() ==
      CPointExportOverlay::LINE_TOOL);
}

void PointExportCommandMessageHandler::OnExportRect()
{
   CPointExportOverlay::set_editor_mode(CPointExportOverlay::RECTANGLE_TOOL);
}

void PointExportCommandMessageHandler::OnUpdateExportRect(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(CPointExportOverlay::get_editor_mode() ==
      CPointExportOverlay::RECTANGLE_TOOL);
}

void PointExportCommandMessageHandler::OnExportRoute()
{
   CPointExportOverlay::set_editor_mode(CPointExportOverlay::ROUTE_TOOL);
}

void PointExportCommandMessageHandler::OnUpdateExportRoute(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(CPointExportOverlay::get_editor_mode() ==
      CPointExportOverlay::ROUTE_TOOL);
}

void PointExportCommandMessageHandler::OnExportSelect()
{
   CPointExportOverlay::set_editor_mode(CPointExportOverlay::SELECT_TOOL);
}

void PointExportCommandMessageHandler::OnUpdateExportSelect(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(CPointExportOverlay::get_editor_mode() ==
      CPointExportOverlay::SELECT_TOOL);
}
