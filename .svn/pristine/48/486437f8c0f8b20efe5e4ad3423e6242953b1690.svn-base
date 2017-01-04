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

// PointsDisplayElementProvider.cpp

#include "stdafx.h"
#include "PointsDisplayElementProvider.h"

#include "FalconView/include/overlay.h"
#include "FalconView/include/param.h"

#include "FalconView/localpnt/DataObject.h"
#include "FalconView/localpnt/localpnt.h"
#include "FalconView/localpnt/lpicon.h"

HRESULT PointsDisplayElementProvider::Initialize(C_overlay* overlay,
   FalconViewOverlayLib::IDisplayElements* display_elements,
   FalconViewOverlayLib::IWorkItems* work_items)
{
   display_elements->SetAltitudeMode(
      FalconViewOverlayLib::ALTITUDE_MODE_RELATIVE_TO_TERRAIN);
   display_elements->SetIconDeclutterMode( FALSE );
   display_elements->SetIconAlignmentMode( FalconViewOverlayLib::ALIGN_CENTER_CENTER );

   C_localpnt_ovl* pnts_overlay = static_cast<C_localpnt_ovl*>(overlay);

   LocalPointDataObject* points_data_object = 
      static_cast<LocalPointDataObject*>(pnts_overlay->
         GetTabularEditorDataObject());

   // Add any existing points
   POSITION pos = points_data_object->GetHeadPosition();
   while (pos)
   {
      C_localpnt_point* pt = points_data_object->GetNext(pos);
      OvlFont& point_font = pnts_overlay->GetFont((LPCTSTR)pt->m_id,
         (LPCTSTR)pt->m_group_name);
      points_overlay_utils::Add3DPoint(display_elements, pt, point_font);
   }

   // Marshal the given IWorkItems
   IStream* stream = nullptr;
   ::CoMarshalInterThreadInterfaceInStream(
      FalconViewOverlayLib::IID_IWorkItems, work_items, &stream);
   pnts_overlay->SetWorkItemsStream(stream);

   return S_OK;
}
