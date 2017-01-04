// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).
//
// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.
//
// FalconView(tm) is a trademark of Georgia Tech Research Corporation.
//
// FileOverlayListComboBox.cpp
//
#include "stdafx.h"
#include "FalconView/include/overlay.h"
#include "FalconView/resource.h"
#include "FalconView/FileOverlayListComboBox.h"

FileOverlayListComboBox::FileOverlayListComboBox() :
   CMFCRibbonComboBox(ID_OVERLAY_COMBO_BOX, FALSE, 200),
   m_show_time_sensitive_only(false)
{
   EnableDropDownListResize(FALSE);
   OVL_get_overlay_manager()->RegisterEvents(this);
   Update();
}

FileOverlayListComboBox::~FileOverlayListComboBox()
{
   OVL_get_overlay_manager()->UnregisterEvents(this);
}

void FileOverlayListComboBox::Update()
{
   RemoveAllItems();

   // enumerate the list of file overlays
   C_overlay* overlay = OVL_get_overlay_manager()->get_first();
   while (overlay)
   {
      if (dynamic_cast<OverlayPersistence_Interface *>(overlay) != NULL)
      {
         // filter out non-time sensitive overlays if the filter
         // button on the toolbar is pressed
         PlaybackTimeSegment_Interface *pFvTimeSegment =
            dynamic_cast<PlaybackTimeSegment_Interface *>(overlay);

         DATE begin, end;
         long valid;
         if (!m_show_time_sensitive_only ||
            (pFvTimeSegment != NULL &&
            pFvTimeSegment->SupportsFvPlaybackTimeSegment() &&
            pFvTimeSegment->GetTimeSegment(&begin, &end, &valid) == S_OK &&
            valid))
         {
            AddItem(OVL_get_overlay_manager()->GetOverlayDisplayName(overlay),
               reinterpret_cast<DWORD_PTR>(overlay));
         }
       }

      overlay = OVL_get_overlay_manager()->get_next(overlay);
   }

   SelectItem(0);
}
