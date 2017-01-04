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

#ifndef FALCONVIEW_ZOOM_PERCENTAGES_H_
#define FALCONVIEW_ZOOM_PERCENTAGES_H_

namespace ui
{
   // Collection of zoom percentages used by
   //
   //    i) the zoom percent combo box hosted on the ribbon bar
   //
   //    ii) the Ctrl+wheel mouse handling routine to determine next/prev
   //        zoom percentages
   //
   extern const int kZoomPercentages[];
   extern const int kNumZoomPercentages;

   // Defines the default zoom percentage step size use by the Step In / Step
   // Out buttons on the ribbon as well as the shift+wheel handler.
   //
   extern const int kZoomPercentageStepSize;

   // Get the next largest zoom percent in ui::kZoomPercentages
   bool GetNextLargestZoomPercent(int current_zoom, int* new_zoom);

   // Get next smallest zoom percent in kZoomPercentages
   bool GetNextSmallestZoomPercent(int current_zoom, int* new_zoom);
}

#endif  // FALCONVIEW_ZOOM_PERCENTAGES_H_