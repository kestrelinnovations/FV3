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



// stdafx first
#include "stdafx.h" 

// this file's header
#include "sm_factory.h"

// system includes

// third party files

// other FalconView headers
#include "..\getobjpr.h"
#include "ComErrorObject.h"  // For CO_CREATE
#include "FalconView/include/InternalToolbarImpl.h"

// this project's headers
#include "scene_cov_ovl.h"


// {416C8169-BD83-48D0-97A0-FF110418B72D}
static const GUID FVWID_Overlay_SceneManager = 
   { 0x416c8169, 0xbd83, 0x48d0, { 0x97, 0xa0, 0xff, 0x11, 0x4, 0x18, 0xb7, 0x2d } };


namespace scene_mgr
{

CSceneCovOverlayEditor::CSceneCovOverlayEditor() :
   m_toolbar(new InternalToolbarImpl(IDR_SM_TOOLBAR))
{
}

HCURSOR CSceneCovOverlayEditor::get_default_cursor()
{
   return SceneCoverageOverlay::get_default_cursor();
}

int CSceneCovOverlayEditor::set_edit_on(boolean_t edit_on)
{
   m_toolbar->ShowToolbar(edit_on);
   return SceneCoverageOverlay::set_edit_on(edit_on);
}

// IFvOverlayLimitUserInterface

// Return TRUE if the user should not be able to change the projection of the map
HRESULT CSceneCovOverlayEditor::get_m_bDisableProjectionUI(long* pVal)
{
   // TO-DO: have the editor check with the overlay to see if it requires
   // projection UI disabled (likely only for select/copy/delete operations)
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the user should not be able to rotate the map while the editor is active
HRESULT CSceneCovOverlayEditor::get_m_bDisableRotationUI(long* pVal)
{
   // TO-DO: have the editor check with the overlay to see if it requires
   // rotation UI disabled (likely only for select/copy/delete operations)
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the map must be North-Up
HRESULT CSceneCovOverlayEditor::get_m_bRequiresNorthUp(long* pVal)
{
   // TO-DO: have the editor check with the overlay to see if it requires
   // north-up (likely only for select/copy/delete operations)
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the map must be projected with the Equal Arc projection when this editor is active
HRESULT CSceneCovOverlayEditor::get_m_bRequiresEqualArc(long* pVal)
{
   // TO-DO: have the editor check with the overlay to see if it requires
   // equal arc projection (likely only for select/copy/delete operations)
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the order of overlays associated with this editor cannot be changed
HRESULT CSceneCovOverlayEditor::get_m_bDisableOverlayOrdering(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}

};  // namespace scene_mgr
