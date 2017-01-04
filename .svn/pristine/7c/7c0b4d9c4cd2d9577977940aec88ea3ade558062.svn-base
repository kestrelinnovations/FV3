// Copyright (c) 1994-2011,2013 Georgia Tech Research Corporation, Atlanta, GA
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


// MapViewGestures.cpp : implementation of the MapView class
//


// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/MapView.h"

// this project's headers
#include "FalconView/include/mapx.h"
#include "FalconView/MapEngineCOM.h"


#ifndef PI
#define PI 3.14159265358979323846
#endif


BOOL MapView::OnGestureZoom(CPoint ptCenter, long lDelta)
{
   if (m_bInPan || m_bInRotate)
   {
      return FALSE;
   }

   ASSERT(m_pCurrentGestureInfo != NULL);
   ASSERT(ptCenter != CPoint(-1, -1));

   if ((m_pCurrentGestureInfo->dwFlags & GF_BEGIN) == GF_BEGIN)
   {
      m_bInZoom = true;
      m_lZoomValueStart = get_curr_map()->actual_zoom_percent();
      m_lGestureWidthStart = (LONG) m_pCurrentGestureInfo->ullArguments;
   }
   else if ((m_pCurrentGestureInfo->dwFlags & GF_END) == GF_END)
   {
      m_bInZoom = false;
   }
   else if (abs((int)lDelta) > 5)
   {
      int zoomPercentage = (int) (m_lZoomValueStart *
         m_pCurrentGestureInfo->ullArguments / m_lGestureWidthStart);
      const int status = m_map_engine->change_zoom_percent(zoomPercentage);
      if (status == SUCCESS)
         invalidate_view(FALSE);
      else
         OnMapError(status);
   }

   return TRUE;
}


BOOL MapView::OnGesturePan(CPoint ptFrom, CPoint ptTo)
{
   if (m_bInRotate || m_bInZoom)
   {
      return FALSE;
   }

   ASSERT(m_pCurrentGestureInfo != NULL);

   const int dx = ptTo.x - ptFrom.x;
   const int dy = ptTo.y - ptFrom.y;

   if ((m_pCurrentGestureInfo->dwFlags & GF_BEGIN) == GF_BEGIN)
   {
      m_bInPan = true;
   }
   else if ((m_pCurrentGestureInfo->dwFlags & GF_END) == GF_END)
   {
      m_bInPan = false;
   }
   else if (dx != 0 || dy != 0)
   {
      // Panning goes here
      RECT window_rect;
      GetClientRect(&window_rect);
      LONG x = (window_rect.right / 2) - dx;
      LONG y = (window_rect.bottom / 2) - dy;

      const int status = m_map_engine->change_center(x, y);
      if (status == SUCCESS)
         invalidate_view(FALSE);
      else
         OnMapError(status);
   }

   return TRUE;
}


BOOL MapView::OnGestureRotate(CPoint ptCenter, double dblAngle)
{
   if (curr_map_rotation_ui_disabled() || m_bInZoom || m_bInPan)
   {
      return FALSE;  // Default processing
   }

   ASSERT(m_pCurrentGestureInfo != NULL);

   if((m_pCurrentGestureInfo->dwFlags & GF_BEGIN) == GF_BEGIN)
   {
      m_dblAngleStart = get_curr_map()->actual_rotation();
      m_bInRotate = true;
   }
   else if((m_pCurrentGestureInfo->dwFlags & GF_END) == GF_END)
   {
      m_bInRotate = false;
   }
   else if (dblAngle != 0.)
   {
      double rot_angle = m_dblAngleStart - (dblAngle * 180.0 / PI);
      if (rot_angle >= 360.0)
         rot_angle -= 360.0;
      else if (rot_angle < 0.0)
         rot_angle += 360.0;

      if (rot_angle < 0.05)
         rot_angle = 0.0;

      const int status = m_map_engine->change_rotation(rot_angle);
      if (status == SUCCESS)
         invalidate_view(FALSE);
      else
         OnMapError(status);
   }

   return TRUE;
}


BOOL MapView::OnGestureTwoFingerTap(CPoint ptCenter)
{
   if (m_bInRotate || m_bInZoom)
   {
      return FALSE;  // Default processing
   }

   ASSERT(m_pCurrentGestureInfo != NULL);

   // Determine margin
   RECT window_rect;
   GetClientRect(&window_rect);
   LONG margin_width = window_rect.right / 8;

   // Check to see if gesture happened in the margin
   LONG dist_from_left = ptCenter.x;
   LONG dist_from_right = window_rect.right - ptCenter.x;
   LONG dist_from_top = ptCenter.y;
   LONG dist_from_bottom = window_rect.bottom - ptCenter.y;
   bool pan_left = (dist_from_left >= 0 && dist_from_left < margin_width);
   bool pan_right = (dist_from_right >= 0 && dist_from_right < margin_width);
   bool pan_up = (dist_from_top >= 0 && dist_from_top < margin_width);
   bool pan_down = (dist_from_bottom >= 0 && dist_from_bottom < margin_width);

   // If gesture was within one or more margins, pan
   if(pan_left || pan_right || pan_up || pan_down)
   {
      // Check for left or right pan
      int x_new = window_rect.right / 2;
      if(pan_left)
         x_new -= window_rect.right;
      else if(pan_right)
         x_new += window_rect.right;

      // Check for up or down pan
      int y_new = window_rect.bottom / 2;
      if(pan_up)
         y_new -= window_rect.bottom;
      else if(pan_down)
         y_new += window_rect.bottom;

      // Perform the actual pan operation
      const int status = m_map_engine->change_center(x_new, y_new);
      if (status == SUCCESS)
         invalidate_view(FALSE);
      else
         OnMapError(status);

      return TRUE;  // Event was handled
   }

   return FALSE;  // Event was not handled, do default processing
}


BOOL MapView::OnGesturePressAndTap(CPoint /*ptPress*/, long /*lDelta*/)
{
   return FALSE;  // Default processing
}
