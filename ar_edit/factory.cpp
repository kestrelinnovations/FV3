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
#include "..\resource.h"
#include "param.h"  // for PRM_
#include "err.h"  // for ERR_
#include "file.h"  // for FIL_access()
#include "ar_edit.h"
#include "..\getobjpr.h"

#include "FalconView/include/InternalToolbarImpl.h"

// {75B39954-5FE7-4c3a-B880-35EB05FA39AC}
const GUID FVWID_Overlay_TrackOrbit = 
   { 0x75b39954, 0x5fe7, 0x4c3a, { 0xb8, 0x80, 0x35, 0xeb, 0x5, 0xfa, 0x39, 0xac } };

// C_ar_edit_ovl_editor
//

C_ar_edit_ovl_editor::C_ar_edit_ovl_editor() :
   m_toolbar(new InternalToolbarImpl(IDR_AR_EDIT_TB))
{
}

HCURSOR C_ar_edit_ovl_editor::get_default_cursor()
{
   return C_ar_edit::get_default_cursor();
}

int C_ar_edit_ovl_editor::set_edit_on(boolean_t edit_on)
{
   m_toolbar->ShowToolbar(edit_on);
   return C_ar_edit::set_edit_on(edit_on);
}
