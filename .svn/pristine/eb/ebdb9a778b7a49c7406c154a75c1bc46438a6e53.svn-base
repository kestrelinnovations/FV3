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



//tiptimer
#include "stdafx.h"
#include "showrmk.h"
#include "tiptimer.h"
#include "mapx.h"
#include "getobjpr.h"  //to get pointer to view
#include "showrmk\\tipdlg.h"

IMPLEMENT_DYNAMIC(TipTimer, FVW_Timer)

CString TipTimer::m_crntTooltipText;
void *TipTimer::m_object = NULL;

CString TipTimer::m_tooltipText;

void TipTimer::expired()
{
   CTipDisplay tip;

   // if the tip is not active
   if (!tip.is_active()) 
   {
      CPoint point;

      // get cursor position in screen coordinates
      GetCursorPos(&point);
      
	   // do not display tool tip if another window is on top
      CWnd *window = CWnd::WindowFromPoint(point);
      if (window != m_view) 
         return;

      // convert cursor position in view (client) coordinates
      m_view->ScreenToClient(&point);

      if (m_tooltipText.GetLength())
      {
         tip.display(m_view, point.x, point.y + 22, m_tooltipText);

         m_view->SetToolTipPoint(point);
         CRect client;
         m_view->GetClientRect(client);
         m_view->ClientToScreen(client);
         ClipCursor(client);
         
         // check if tip is still valid in half a second
         set_period(500);
         reset();
         start();

         m_crntTooltipText = m_tooltipText;
         m_object = NULL;
      }
   }   
   
   // else if (nIDEvent == FV_TOOL_TIP_TIME_OUT)
   else
   {
      CPoint point;

      // remove tool tip if another window is on top
      GetCursorPos(&point);
      CWnd *window = m_view->WindowFromPoint(point);
      if (window != m_view && window->IsKindOf(RUNTIME_CLASS(CTipDialog)) == FALSE ) 
         m_view->clear_tool_tip();

      // if a tool tip currently exists
      if (m_tooltipText.GetLength() > 0)
      {
         // check if tip is still valid in 1/8 of a second
         set_period(125);
         reset();
         start();
      }
   }
}

IMPLEMENT_DYNAMIC(SmoothPanTimer, FVW_Timer)

void SmoothPanTimer::expired()
{
	m_view->force_smooth_pan();

	m_force_smooth_pan = TRUE;

	POINT point;
	GetCursorPos(&point);
	m_view->ScreenToClient(&point);
	DWORD x = point.x;
	DWORD y = point.y;
	m_view->SendMessage(WM_LBUTTONDOWN, 0, x | (y << 16));
}

