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

// drag_utils.h
//

namespace drag_utils {

   // Returns TRUE if current point is within the drag rectangle.
   // The drag rectangle uses the system metrics for the width and height, in 
   // pixels, of a rectangle centered on a drag point to allow for limited 
   // movement of the mouse pointer before a drag operation begins. This allows 
   // the user to click and release the mouse button easily without 
   // unintentionally starting a drag  operation.
   BOOL in_drag_threshold(CPoint start, CPoint current);
}