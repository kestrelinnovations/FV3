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

#ifndef _FALCONVIEW_CAMERAMODEOPTIONSDIALOG_H_
#define _FALCONVIEW_CAMERAMODEOPTIONSDIALOG_H_

class HudOptionsPage;
struct HudProperties;

// The CameraModeOptionsDialog class is a property sheet that is used to
// host the HUD properties and, eventually, other 3D property pages
//
class CameraModeOptionsDialog : public CPropertySheet
{
   DECLARE_DYNAMIC(CameraModeOptionsDialog)

public:
   CameraModeOptionsDialog() { }
   CameraModeOptionsDialog(HudProperties* hud_properties);

private:
   HudOptionsPage* m_hud_options_page;
};

#endif  // _FALCONVIEW_CAMERAMODEOPTIONSDIALOG_H_
