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
#include "Common/ComErrorObject.h"
#include "FalconView/include/err.h"    // for ERR_
#include "FalconView/include/param.h"  // for PRM_
#include "FalconView/include/file.h"   // for FIL_
#include "localpnt.h"
#include "FalconView/getobjpr.h"

#include "FalconView/include/InternalToolbarImpl.h"
#include "FalconView/localpnt/PointsDisplayElementProvider.h"


C_localpnt_ovl_editor::C_localpnt_ovl_editor() :
   m_toolbar(new InternalToolbarImpl(IDR_LOCAL_POINT_TB))
{
}

HCURSOR C_localpnt_ovl_editor::get_default_cursor()
{
   return C_localpnt_ovl::get_default_cursor();
}

int C_localpnt_ovl_editor::set_edit_on(boolean_t edit_on)
{
   m_toolbar->ShowToolbar(edit_on);
   return C_localpnt_ovl::set_edit_on(edit_on);
}

C_localpnt_ovl_factory::C_localpnt_ovl_factory()
{
}

C_localpnt_ovl_factory::~C_localpnt_ovl_factory()
{
}

HRESULT C_localpnt_ovl_factory::CreateOverlayInstance(C_overlay **ppOverlay)
{
   CComObject<C_localpnt_ovl> *pOverlay;
   CComObject<C_localpnt_ovl>::CreateInstance(&pOverlay);
   pOverlay->AddRef();

   *ppOverlay = pOverlay;

   return S_OK;
}

// PointsDisplayElementProviderFactory
//

void PointsDisplayElementProviderFactory::CreateInstance(
   DisplayElementProvider_Interface** dep)
{
   *dep = new PointsDisplayElementProvider;
}
