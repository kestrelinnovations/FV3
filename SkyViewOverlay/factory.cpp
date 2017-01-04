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

#include "Common\ComErrorObject.h"

#include "err.h"  // for ERR_
#include "file.h"  // for FIL_access()
#include "mov_sym.h"
#include "ovl_mgr.h"
#include "param.h"  // for PRM_
#include "..\getobjpr.h"

#include "FalconView/include/InternalToolbarImpl.h"

// {E94E5972-83E7-4e58-A777-A36D2838C7C9}
const GUID FVWID_Overlay_SkyView = 
   { 0xe94e5972, 0x83e7, 0x4e58, { 0xa7, 0x77, 0xa3, 0x6d, 0x28, 0x38, 0xc7, 0xc9 } };


Cmov_sym_overlay_editor::Cmov_sym_overlay_editor() :
   m_toolbar(new InternalToolbarImpl(IDR_MOV_SYM_TB))
{
}

HCURSOR Cmov_sym_overlay_editor::get_default_cursor()
{
   return Cmov_sym_overlay::get_default_cursor();
}

int Cmov_sym_overlay_editor::set_edit_on(boolean_t edit_on)
{
   m_toolbar->ShowToolbar(edit_on);
   return Cmov_sym_overlay::set_edit_on(edit_on);
}

bool skyview_overlay::IsOverlayTypeEnabled()
{
   static bool enabled =
      OVL_get_type_descriptor_list()->IsOverlayEnabled(FVWID_Overlay_SkyView);

   return enabled;
}