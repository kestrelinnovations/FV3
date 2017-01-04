// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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
#include "ZoomPercentages.h"

extern const int ui::kZoomPercentages[] = { 
   800, 400, 200, 100, 75, 50, 25, 10 };

extern const int ui::kNumZoomPercentages =
   sizeof(ui::kZoomPercentages) / sizeof(int);

extern const int ui::kZoomPercentageStepSize = 5;

bool ui::GetNextLargestZoomPercent(int current_zoom, int* new_zoom)
{
   // Get the next largest zoom percent in ui::kZoomPercentages
   const int* new_zoom_percent = std::lower_bound(ui::kZoomPercentages,
      ui::kZoomPercentages + ui::kNumZoomPercentages, current_zoom,
      std::greater<int>()) - 1;

   if (new_zoom_percent < ui::kZoomPercentages)
      return false;

   *new_zoom = *new_zoom_percent;
   return true;
}

bool ui::GetNextSmallestZoomPercent(int current_zoom, int* new_zoom)
{
   // Get next smallest zoom percent in kZoomPercentages not equal to the
   // current zoom
   const int* new_zoom_percent = std::lower_bound(ui::kZoomPercentages,
      ui::kZoomPercentages + ui::kNumZoomPercentages, current_zoom,
      std::greater<int>());
   if (*new_zoom_percent == current_zoom)
      ++new_zoom_percent;

   if (new_zoom_percent >= ui::kZoomPercentages + ui::kNumZoomPercentages)
      return false;

   *new_zoom = *new_zoom_percent;
   return true;
}
