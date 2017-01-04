// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#include "stdafx.h"
#include "CameraModeOptionsDialog.h"

#include "HudOptionsPage.h"

namespace
{
   const char* CAMERA_MODE_OPTIONS_DIALOG_CAPTION = "Camera Mode (3D) Options";
}

IMPLEMENT_DYNAMIC(CameraModeOptionsDialog, CPropertySheet)

CameraModeOptionsDialog::CameraModeOptionsDialog(HudProperties* hud_properties) :
   CPropertySheet(CAMERA_MODE_OPTIONS_DIALOG_CAPTION)
{
   m_psh.dwFlags |= PSH_NOAPPLYNOW;

   // Create and add property pages
   m_hud_options_page = new HudOptionsPage(hud_properties);
   AddPage(m_hud_options_page);
}
