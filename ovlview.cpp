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



// ovlview.cpp

#include "stdafx.h"
#include "getobjpr.h"
#include "IconDlg.h"
#include "OvlFctry.h"
#include "MovingMapOverlay\factory.h"
#include "PrintToolOverlay\factory.h"
#include "FctryLst.h"
#include "gps.h"
#include "TabularEditorDlg.h"
#include "mov_sym.h"
#include "mapx.h"
#include "PrntTool.h"
#include "nitf.h"
#include "ar_edit.h"
#include "localpnt/localpnt.h"

/*
 *  this is defined in \mfc\src\afximpl.h
 */
#ifndef OFN_EXPLORER
#define OFN_EXPLORER  0x00080000L
#endif

LRESULT MapView::OnInvalidateFromThread(WPARAM wParam, LPARAM lParam)
{
   ASSERT(lParam == 0);
   ASSERT(wParam != 0);

   // wParam contains a map view invalidation object which will be used
   // to invalidate all or part of the map view window
   MapViewInvalidation *invalidation = (MapViewInvalidation *)wParam;

   // The invalidation object passed to this function will be deleted.
   OVL_get_overlay_manager()->on_invalidate_from_thread(invalidation);

   return 1;
}

LRESULT MapView::OnInvalidate(WPARAM, LPARAM)
{
   OVL_get_overlay_manager()->invalidate_all();

   return 1;
}
