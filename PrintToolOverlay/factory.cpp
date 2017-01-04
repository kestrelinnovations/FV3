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



// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/factory.h"

// system includes

// third party files

// other FalconView headers
#include "Common/ComErrorObject.h"
#include "FalconView/getobjpr.h"
#include "FalconView/include/err.h"    // for ERR_
#include "FalconView/include/file.h"   // for FIL_access()
#include "FalconView/include/InternalToolbarImpl.h"
#include "FalconView/include/param.h"  // for PRM_
#include "FalconView/resource.h"


// this project's headers
#include "FalconView/include/PrntTool.h"

typedef __int64 int32;  // use instead of long

// {0B517AAC-6421-458f-94F9-2AD66FC07940}
const GUID FVWID_Overlay_PageLayout =
{
   0xb517aac, 0x6421, 0x458f,
   { 0x94, 0xf9, 0x2a, 0xd6, 0x6f, 0xc0, 0x79, 0x40 }
};


CPrintToolOverlayEditor::CPrintToolOverlayEditor() :
   m_toolbar(new InternalToolbarImpl(IDR_OVL_PRINT_TOOLS))
{
}

HCURSOR CPrintToolOverlayEditor::get_default_cursor()
{
   return CPrintToolOverlay::get_default_cursor();
}

int CPrintToolOverlayEditor::set_edit_on(boolean_t edit_on)
{
   m_toolbar->ShowToolbar(edit_on);
   return CPrintToolOverlay::set_edit_on(edit_on);
}
