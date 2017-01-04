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
#include "..\resource.h"
#include "..\getobjpr.h"
#include "factory.h"
#include "param.h"  // for PRM_
#include "err.h"  // for ERR_
#include "file.h"  // for FIL_access()
#include "nitf.h"
#include "utils.h"

#include "FalconView/include/InternalToolbarImpl.h"

// {FA9F1270-14E1-487b-B3AD-F7847A01CB4F}
const GUID FVWID_Overlay_NitfFiles = 
   { 0xfa9f1270, 0x14e1, 0x487b, { 0xb3, 0xad, 0xf7, 0x84, 0x7a, 0x1, 0xcb, 0x4f } };


C_nitf_ovl_editor::C_nitf_ovl_editor() :
   m_toolbar(new InternalToolbarImpl(IDR_NITF_TOOLBAR))
{
}

HCURSOR C_nitf_ovl_editor::get_default_cursor()
{
   return NULL;
}

int C_nitf_ovl_editor::set_edit_on(boolean_t edit_on)
{
   m_toolbar->ShowToolbar(edit_on);
   return C_nitf_ovl::set_edit_on(edit_on);
}

// IFvOverlayLimitUserInterface

// Return TRUE if the user should not be able to change the projection of the map
HRESULT C_nitf_ovl_editor::get_m_bDisableProjectionUI(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}

// Return TRUE if the user should not be able to rotate the map while the editor is active
HRESULT C_nitf_ovl_editor::get_m_bDisableRotationUI(long* pVal)
{
   *pVal = TRUE;
   return S_OK;
}

// Return TRUE if the map must be North-Up
HRESULT C_nitf_ovl_editor::get_m_bRequiresNorthUp(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the map must be projected with the Equal Arc projection when this editor is active
HRESULT C_nitf_ovl_editor::get_m_bRequiresEqualArc(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if the order of overlays associated with this editor cannot be changed
HRESULT C_nitf_ovl_editor::get_m_bDisableOverlayOrdering(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}

// Return TRUE if FalconView should activate the editor when a static overlay is toggled on or when the user chooses File | New
   // for a file overlay.  Return FALSE otherwise.  FalconView's default behavior is TRUE.
HRESULT C_nitf_ovl_editor::get_m_bAutoEnterOverlayEditor(long* pVal)
{
   *pVal = FALSE;
   return S_OK;
}

// Normally, FalconView will use the overlay type's icon as the editor toolbar button.  The following method can be
// used to override the editor toolbar button's image.  Return an empty string if you would like to preserve the default behavior.
HRESULT C_nitf_ovl_editor::get_m_bEditorToolbarButtonBitmap(BSTR* pEditorToolbarButtonBmp)
{
   *pEditorToolbarButtonBmp = _bstr_t("").Detach();
   return S_OK;
}
