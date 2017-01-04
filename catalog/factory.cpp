// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
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
#include "factory.h"
#include "Cov_ovl.h"
#include "..\getobjpr.h"

#include "FalconView/include/InternalToolbarImpl.h"

// {A8F72769-6996-4e97-86E5-E22B6067C1A8}
const GUID FVWID_Overlay_MapDataManager = 
   { 0xa8f72769, 0x6996, 0x4e97, { 0x86, 0xe5, 0xe2, 0x2b, 0x60, 0x67, 0xc1, 0xa8 } };


CCoverageOverlayEditor::CCoverageOverlayEditor() :
   m_toolbar(new InternalToolbarImpl(IDR_MDM_TOOLBAR))
{
}

HCURSOR CCoverageOverlayEditor::get_default_cursor()
{
   return CoverageOverlay::get_default_cursor();
}

int CCoverageOverlayEditor::set_edit_on(boolean_t edit_on)
{
   m_toolbar->ShowToolbar(edit_on);
   return CoverageOverlay::set_edit_on(edit_on);
}

// IFvOverlayLimitUserInterface

// Return TRUE if the user should not be able to change the projection of the map
HRESULT CCoverageOverlayEditor::get_m_bDisableProjectionUI(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}

// Return TRUE if the user should not be able to rotate the map while the editor is active
HRESULT CCoverageOverlayEditor::get_m_bDisableRotationUI(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}

// Return TRUE if the map must be North-Up
HRESULT CCoverageOverlayEditor::get_m_bRequiresNorthUp(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}

// Return TRUE if the map must be projected with the Equal Arc projection when this editor is active
HRESULT CCoverageOverlayEditor::get_m_bRequiresEqualArc(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}

// Return TRUE if the order of overlays associated with this editor cannot be changed
HRESULT CCoverageOverlayEditor::get_m_bDisableOverlayOrdering(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}
