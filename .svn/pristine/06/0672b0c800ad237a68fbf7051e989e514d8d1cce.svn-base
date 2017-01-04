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
#include "PredictivePathRenderer.h"
#include "PredictivePath.h"
#include "gpsstyle.h"
#include "mapx.h"
#include "utils.h"
#include "ovl_mgr.h"

#define TREND_DOT_SIZE 2
#define FG_LINE_WIDTH 1
#define BG_LINE_WIDTH 3

/* static */
void CPredictivePathRenderer::RenderTrendDots(MapProj* pMap, CDC* pDC, CPredictivePath *pPredictivePath, CPredictivePathProperties *pProp)
{
   UtilDraw util(pDC);

   const int nForegroundColor = pProp->get_predictive_path_icon_color();

   // determine the background color of a trend dot based on the foreground color
   int nBackgroundColor = (nForegroundColor == UTIL_COLOR_BLACK) ? UTIL_COLOR_WHITE : UTIL_COLOR_BLACK;

   // for each trend dot
   int nNumTrendDots = pProp->get_num_trend_dots();
	for (int i=0; i<nNumTrendDots; ++i)
	{
      // find the relative predictive path point
      if (pPredictivePath->MoveTo(pProp->get_trend_dot_time(i) - 1))
      {
         int x, y;
         pMap->geo_to_surface(pPredictivePath->get_m_Latitude(), pPredictivePath->get_m_Longitude(), &x, &y);

         // draw the background circle first
         util.set_pen(nBackgroundColor, UTIL_LINE_SOLID, TREND_DOT_SIZE + 1);
         util.draw_circle(TRUE, x, y, TREND_DOT_SIZE + 1);

         // draw the foreground circle
         util.set_pen(nForegroundColor, UTIL_LINE_SOLID, TREND_DOT_SIZE);
         util.draw_circle(TRUE, x, y, TREND_DOT_SIZE);
      }
	}
}

/* static */
void CPredictivePathRenderer::RenderCenterLine(MapProj* pMap, CDC* pDC, CPredictivePath *pPredictivePath, CPredictivePathProperties *pProp)
{
   if (!pProp->get_predictive_path_center_line_on())
      return;

   CLineSegmentResizableArray lineSegment(false);

   // for each predictive point
   if (pPredictivePath->MoveFirst())
   {
      do
      {
         // compute the predictive point's screen coordinates
         int x,y;
         pMap->geo_to_surface(pPredictivePath->get_m_Latitude(), pPredictivePath->get_m_Longitude(), &x, &y);

         // add the screen point to the line segment
         lineSegment.AddPoint(CPoint(x, y));

      } while (pPredictivePath->MoveNext());
   }

   UtilDraw util(pDC);

   // draw the background of the polyline first
   util.set_pen(RGB(0,0,0), UTIL_LINE_SOLID, BG_LINE_WIDTH, TRUE);
   util.DrawPolyLine(lineSegment);

   // draw the foreground of the polyline
   util.set_pen(RGB(255,255,255), UTIL_LINE_SOLID, FG_LINE_WIDTH);
   util.DrawPolyLine(lineSegment);
}

/* static */
void CPredictivePathRenderer::InvalidateTrendDots(MapProj* pMap, CPredictivePath *pPredictivePath, CPredictivePathProperties *pProp)
{
   // for each trend dot
   int nNumTrendDots = pProp->get_num_trend_dots();
	for (int i=0; i<nNumTrendDots; ++i)
	{
      // find the relative predictive path point
      if (pPredictivePath->MoveTo(pProp->get_trend_dot_time(i) - 1))
      {
         // compute the point's screen coordinates
         int x, y;
         pMap->geo_to_surface(pPredictivePath->get_m_Latitude(), pPredictivePath->get_m_Longitude(), &x, &y);

         // invalidate the rect associated with the trend dot.  The size of the rectangle is the size of
         // dot itself plus half the size of the pen width (rounded up, hence the extra + 1)
         const int nTrendDotSize = (TREND_DOT_SIZE + 1) + (TREND_DOT_SIZE + 2) / 2;
         CRect rect(CPoint(x - nTrendDotSize, y - nTrendDotSize), CPoint(x + nTrendDotSize, y + nTrendDotSize));
         OVL_get_overlay_manager()->invalidate_rect(rect);
      }
	}
}

/* static */
void CPredictivePathRenderer::InvalidateCenterLine(MapProj* pMap, CPredictivePath *pPredictivePath, CPredictivePathProperties *pProp)
{
   if (!pProp->get_predictive_path_center_line_on())
      return;

   CFvwUtil* pFvwUtil = CFvwUtil::get_instance();
   if (pPredictivePath->MoveFirst())
   {
      int prevX = 0;
      int prevY = 0;
      bool bPreviousValid = false;

      do
      {
         // compute the predictive point's screen coordinates
         int x,y;
         pMap->geo_to_surface(pPredictivePath->get_m_Latitude(), pPredictivePath->get_m_Longitude(), &x, &y);

         // if the previous point has been set, then invalidate the line between the previous point and
         // the current point
         if (bPreviousValid)
         {
            CRect boundingRect = pFvwUtil->get_bounding_rect(prevX, prevY, x, y, UTIL_LINE_SOLID, BG_LINE_WIDTH);
            OVL_get_overlay_manager()->invalidate_rect(boundingRect);
         }

         prevX = x;
         prevY = y;
         bPreviousValid = true;

      } while (pPredictivePath->MoveNext());
   }
}