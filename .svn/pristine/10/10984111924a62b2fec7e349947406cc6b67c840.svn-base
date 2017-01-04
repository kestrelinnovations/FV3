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


#ifndef FALCONVIEW_ZOOMBAR_H_
#define FALCONVIEW_ZOOMBAR_H_

#include "FalconView/include/common.h"
#include "FalconView/MapParamsListener_Interface.h"

class ZoomPercentComboBoxBase
{
public:
   ZoomPercentComboBoxBase();

   void SetRange(int lower_limit, int upper_limit)
   {
      m_lower_limit = lower_limit;
      m_upper_limit = upper_limit;
   }

   bool GetAndValidateZoomPercent(int* zoom_percent);
   void SetZoomPercent(int zoom_percent);
   void RevertToLastValidZoomPercentage()
   {
      SetZoomPercent(m_last_valid_zoom_percentage);
   }

protected:
   virtual std::string GetControlText() const = 0;
   virtual void SetControlText(const std::string& str) = 0;
   virtual bool ContainsText(const std::string& str) const = 0;

private:
   // The range of valid zoom percentages
   int m_lower_limit;
   int m_upper_limit;

   int m_last_valid_zoom_percentage;
};

class ZoomComboBox :
   public CComboBox,
   public ZoomPercentComboBoxBase
{
protected:
   virtual std::string GetControlText() const;
   virtual void SetControlText(const std::string& str);
   virtual bool ContainsText(const std::string& str) const;
};

class ZoomPercentRibbonComboBox :
   public CMFCRibbonComboBox,
   public ZoomPercentComboBoxBase,
   public MapParamsListener_Interface
{
public:
   ZoomPercentRibbonComboBox(UINT id);
   ~ZoomPercentRibbonComboBox();

   void RegisterForZoomPercentChanges();

protected:
   // ZoomPercentComboBoxBase overrides
   virtual std::string GetControlText() const override;
   virtual void SetControlText(const std::string& str) override;
   virtual bool ContainsText(const std::string& str) const override;

   // MapParamsListener_Interface overrides
   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

private:
   static const int ZOOM_PERCENT_RIBBON_COMBO_BOX_WIDTH = 45;
};

#endif  // FALCONVIEW_ZOOMBAR_H_
