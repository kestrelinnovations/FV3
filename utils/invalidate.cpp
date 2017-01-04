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
#include "..\getobjpr.h" // for MapView
#include "errx.h"

void UTL_invalidate_view(CWnd* wnd, BOOL bErase, 
   BOOL allow_animation_override, BOOL redraw_overlays_from_scratch_when_animating)
{
   if (!wnd->IsKindOf(RUNTIME_CLASS(MapView)))
   {
      // An overlay can invalidate the map while in print preview (e.g., a time-sensitive overlay)
      return;
   }

   MapView* view = static_cast<MapView*>(wnd);
   view->invalidate_view(bErase, allow_animation_override,
      redraw_overlays_from_scratch_when_animating);
}

void UTL_invalidate_view_rect(CWnd* wnd, LPCRECT lpRect, BOOL bErase,
   BOOL allow_animation_override, BOOL redraw_overlays_from_scratch_when_animating)
{
   if (!wnd->IsKindOf(RUNTIME_CLASS(MapView)))
   {
      // An overlay can invalidate the map while in print preview (e.g., a time-sensitive overlay)
      return;
   }

   MapView* view = static_cast<MapView*>(wnd);
   view->invalidate_view_rect(lpRect, bErase, allow_animation_override,
      redraw_overlays_from_scratch_when_animating);
}

void UTL_invalidate_view_rgn(CWnd* wnd, CRgn* pRgn, BOOL bErase,
   BOOL allow_animation_override, BOOL redraw_overlays_from_scratch_when_animating)
{
   if (!wnd->IsKindOf(RUNTIME_CLASS(MapView)))
   {
      // An overlay can invalidate the map while in print preview (e.g., a time-sensitive overlay)
      return;
   }

   MapView* view = static_cast<MapView*>(wnd);
   view->invalidate_view_rgn(pRgn, bErase, allow_animation_override,
      redraw_overlays_from_scratch_when_animating);
}
