// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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
#include "ZoomPercentComboBox.h"

#include "FalconView/getobjpr.h"
#include "FalconView/include/mapx.h"
#include "FalconView/MapView.h"
#include "FalconView/resource.h"
#include "FalconView/ZoomPercentages.h"

namespace
{
   const char* NATIVE_ZOOM_PERCENT_STRING = "Native";
   const char* TO_SCALE_ZOOM_PERCENT_STRING = "To Scale";
}

// ZoomPercentComboBoxBase
//
ZoomPercentComboBoxBase::ZoomPercentComboBoxBase()
{
   m_last_valid_zoom_percentage = 100;

   // Use the MapView's range of supported zoom percentages by default
   m_lower_limit = MapView::MIN_ZOOM_PERCENT;
   m_upper_limit = MapView::MAX_ZOOM_PERCENT;
}

bool ZoomPercentComboBoxBase::GetAndValidateZoomPercent(int* zoom_percent)
{
   std::string zoom_percent_str = GetControlText();

   // Convert string to integral zoom percentage
   int value;
   if (sscanf_s(zoom_percent_str.c_str(), "%d", &value) != 1)
   {
      if (zoom_percent_str == NATIVE_ZOOM_PERCENT_STRING)
         value = NATIVE_ZOOM_PERCENT;

      else if (zoom_percent_str == TO_SCALE_ZOOM_PERCENT_STRING)
         value = TO_SCALE_ZOOM_PERCENT;
   }

   // Check that the zoom percentage is within bounds
   if ( (m_lower_limit <= value && value <= m_upper_limit) ||
         (value == NATIVE_ZOOM_PERCENT &&
            ContainsText(NATIVE_ZOOM_PERCENT_STRING)) ||
         (value == TO_SCALE_ZOOM_PERCENT &&
            ContainsText(TO_SCALE_ZOOM_PERCENT_STRING)))
   {
      *zoom_percent = value;
      return true;
   }

   // Validation failed. Put up an error message and reset the zoom.
   CString msg;
   msg.Format("The number must be between %d and %d.",
      m_lower_limit, m_upper_limit);
   AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);

   RevertToLastValidZoomPercentage();

   return false;
}

void ZoomPercentComboBoxBase::SetZoomPercent(int zoom_percent)
{
   if (zoom_percent == NATIVE_ZOOM_PERCENT)
      SetControlText(NATIVE_ZOOM_PERCENT_STRING);
   else if (zoom_percent == TO_SCALE_ZOOM_PERCENT)
      SetControlText(TO_SCALE_ZOOM_PERCENT_STRING);
   else
   {
      const int STR_LEN = 128;
      char str[STR_LEN];
      sprintf_s(str, STR_LEN, "%d%%", zoom_percent);
      SetControlText(str);
   }
   m_last_valid_zoom_percentage = zoom_percent;
}

// ZoomComboBox
//

std::string ZoomComboBox::GetControlText() const
{
   CString ret;

   int nIndex = GetCurSel();
   if (nIndex == CB_ERR)
      GetWindowText(ret);
   else
      GetLBText(nIndex, ret);

   return std::string(ret);
}

void ZoomComboBox::SetControlText(const std::string& str)
{
   int idx = FindStringExact(-1, str.c_str());
   SetCurSel((idx==CB_ERR) ? -1 : idx);
   if (idx == CB_ERR)
      SetWindowText(str.c_str());
}

bool ZoomComboBox::ContainsText(const std::string& str) const
{
   return FindString(-1, str.c_str()) != CB_ERR;
}

// ZoomPercentRibbonComboBox
//

ZoomPercentRibbonComboBox::ZoomPercentRibbonComboBox(UINT id) :
   CMFCRibbonComboBox(id, TRUE, ZOOM_PERCENT_RIBBON_COMBO_BOX_WIDTH)
{
   for (int i = 0; i < ui::kNumZoomPercentages; ++i)
   {
      char buf[5];  // e.g., 800%
      sprintf_s(buf, 5, "%d%%", ui::kZoomPercentages[i]);
      AddItem(buf);
   }
   AddItem(NATIVE_ZOOM_PERCENT_STRING);
   AddItem(TO_SCALE_ZOOM_PERCENT_STRING);

   EnableDropDownListResize(FALSE);
}

void ZoomPercentRibbonComboBox::RegisterForZoomPercentChanges()
{
   // The reason this is not done in the constructor is because the MapView
   // cannot be aquired, at least through fvw_get_view, at the time this object
   // is constructed.
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->AddMapParamsListener(this);
}

ZoomPercentRibbonComboBox::~ZoomPercentRibbonComboBox()
{
   // Unregister for zoom percent change notifications
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->RemoveMapParamsListener(this);
}

std::string ZoomPercentRibbonComboBox::GetControlText() const
{
   return std::string(GetEditText());
}

void ZoomPercentRibbonComboBox::SetControlText(const std::string& str)
{
   if (!SelectItem(str.c_str()))
      SetEditText(str.c_str());
}

bool ZoomPercentRibbonComboBox::ContainsText(const std::string& str) const
{
   return FindItem(str.c_str()) != -1;
}

void ZoomPercentRibbonComboBox::MapParamsChanged(int change_mask,
   long map_group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params,
   const MapDisplayParams& map_display_params)
{
   if ((change_mask & MAP_PROJ_ZOOM_PERCENT_CHANGED) ==
      MAP_PROJ_ZOOM_PERCENT_CHANGED)
   {
      SetZoomPercent(map_proj_params.zoom_percent);
   }
}