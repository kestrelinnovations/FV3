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
#include "param.h"                  // for PRM_
#include "err.h"                    // for ERR_
#include "file.h"                   // for FIL_access()
#include "TacModel.h"
#include "..\getobjpr.h"

#include "FalconView/include/InternalToolbarImpl.h"
#include "FalconView/TacticalModel/ModelDisplayElementProvider.h"

   // {84BC3A3B-1567-4025-81C0-FC5C24310CDA}
const GUID FVWID_Overlay_TacticalModel = 
{ 0x84bc3a3b, 0x1567, 0x4025, { 0x81, 0xc0, 0xfc, 0x5c, 0x24, 0x31, 0xc, 0xda } };

   


C_model_ovl_editor::C_model_ovl_editor() :
   m_toolbar(new InternalToolbarImpl(IDR_MODEL_TB))
{
}

HCURSOR C_model_ovl_editor::get_default_cursor()
{
   return C_model_ovl::get_default_cursor();
}

int C_model_ovl_editor::set_edit_on(boolean_t edit_on)
{
   m_toolbar->ShowToolbar(edit_on);
   return C_model_ovl::set_edit_on(edit_on);
}


void ModelDisplayElementProviderFactory::CreateInstance(
   DisplayElementProvider_Interface** dep)
{
   *dep = new ModelDisplayElementProvider;
}