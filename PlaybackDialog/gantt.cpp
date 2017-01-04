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



// gantt.cpp - implements the gantt chart window in the playback control
//

#include "stdafx.h"
#include "viewtime.h"
#include "ovl_mgr.h"

#define GANT_ROW_HEIGHT 25
#define GANT_TEXT_OFFSET_X 6

BEGIN_MESSAGE_MAP(GanttWnd, CWnd)
	//{{AFX_MSG_MAP(GanttWnd)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

GanttWnd::GanttWnd(CDialog *parent, int num_tsovl) : m_parent(parent),
 m_num_tsovl(num_tsovl)
{
	CRect window_rect;
   m_parent->GetWindowRect(window_rect);
	
	CRect gant_rect;
	m_parent->GetDlgItem(IDC_TOPLEFT)->GetWindowRect(gant_rect);
	int x = gant_rect.TopLeft().x - window_rect.TopLeft().x;
	int y = gant_rect.TopLeft().y - window_rect.TopLeft().y;
	
	int scrollbar_offset_x = 0;
	DWORD style = WS_CHILD | WS_VISIBLE | WS_VSCROLL;
	if (m_num_tsovl > 4)
	{
		scrollbar_offset_x = 14;
	}
	
	Create(NULL, "Gant Chart",  style,		
		CRect(x,y,x+gant_rect.Width()+scrollbar_offset_x, 
		y+(__min(m_num_tsovl,4) + 2)*GANT_ROW_HEIGHT), m_parent, 1234);
	
	SCROLLINFO scroll_info;
	scroll_info.cbSize = sizeof(SCROLLINFO);
	scroll_info.fMask = SIF_PAGE | SIF_RANGE;
	scroll_info.nMin = 0;
	scroll_info.nMax = m_num_tsovl - 4;
	scroll_info.nPage = 1;
	SetScrollInfo(SB_VERT, &scroll_info);
	m_scroll_position = 0;

   EnableScrollBarCtrl(SB_VERT, num_tsovl > 4);
}

GanttWnd::~GanttWnd()
{
	DestroyWindow();
}

void GanttWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode)
	{
	case   SB_LINEDOWN:      
	case   SB_BOTTOM:  
	case   SB_PAGEDOWN:  
		if (m_scroll_position < m_num_tsovl - 4)
			m_scroll_position++;	
		break;
	case   SB_LINEUP:     
   case   SB_TOP: 
   case   SB_PAGEUP: 
		if (m_scroll_position > 0)
			m_scroll_position--;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_scroll_position = (int)nPos;
	}

	SetScrollPos(SB_VERT, m_scroll_position);
	refresh(m_current_time);
}

void GanttWnd::refresh(COleDateTime current_time)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
   int i;
   COleDateTime all_begin, all_end;
   CDC memory_dc;
   CRect rect;

	m_num_tsovl = OVL_get_overlay_manager()->
      get_num_time_sensitive_overlays();

	// store the current time so we can refresh the gantt chart
	// without be told the current time from the playback control
	m_current_time = current_time;
	
	// get the dc associated with the gant window
   CDC *dc = GetDC();
	
   // get the client rect
   GetClientRect(&rect);
	
   // create an equivalent dc in memory
   CBitmap bmp;
   memory_dc.CreateCompatibleDC(dc);
   bmp.CreateCompatibleBitmap(dc, rect.Width(), rect.Height());
   memory_dc.SelectObject(&bmp);
   UtilDraw util(&memory_dc);
	
   // get the starting and ending time for all time-sensitive overlays
	all_begin = ((CViewTime *)m_parent)->get_starting_time();
	all_end = ((CViewTime *)m_parent)->get_ending_time();
	
   // draw the background (alternate shade of grey), pad the chart with 
   // two strips
   for (i = 0; i < __min(m_num_tsovl,4) + 2;i++)
   {
      CRect fill_rect(rect.TopLeft().x, rect.TopLeft().y+i*25,
         rect.TopLeft().x+rect.Width(), rect.TopLeft().y+(i+1)*25);
      CBrush brush(RGB(220 - (i%2)*13, 220 - (i%2)*13, 220 - (i%2)*13));
      memory_dc.FillRect(&fill_rect, &brush);
   }
	
   // draw a dashed line depicting the current slider position
   {
      util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_DASH2, 1);
      int x = futil->round(((DATE)m_current_time - 
         (DATE)all_begin)*((double)rect.Width()/((DATE)all_end-(DATE)all_begin)));
      if (x == rect.Width()) 
         x--;
      util.draw_line(x, 0, x, rect.Height()-1);
      
      CPoint points[3];
      util.set_pen(UTIL_COLOR_YELLOW, UTIL_LINE_SOLID, 1);
      
      points[0] = CPoint(x-3,0);
      points[1] = CPoint(x, 3);
      points[2] = CPoint(x+3,0);
      util.draw_polygon(TRUE, points, 3);
      
      points[0] = CPoint(x-3, rect.Height()-1);
      points[1] = CPoint(x, rect.Height() - 4);
      points[2] = CPoint(x+3, rect.Height()-1);
      util.draw_polygon(TRUE, points, 3);
   }

	// we have only room enough to display a maximum of 6 elements.
	// calculate the starting and ending indexes of the time sensitve
	// overlays that will be drawn.  start_at_top is set if we are
	// going to draw at the top strip (initially it is blank)
	const int start_at_top = m_scroll_position > 0;
	const int start_index = m_scroll_position - start_at_top;
	const int end_index = __min(start_index + 4 + start_at_top,	
		m_num_tsovl - 1);
	
   // draw the text of each of the time-sensitive overlays in the gant chart
   for(i=start_index;i<=end_index;i++)
   {
      C_overlay *overlay = OVL_get_overlay_manager()->get_time_sensitive_overlay(i);
      
      if (overlay != NULL)
      {
         int x = GANT_TEXT_OFFSET_X;
         int y = (i - start_index + !start_at_top)*25;
         CPoint cpt[4];
         
         futil->draw_text(&memory_dc, OVL_get_overlay_manager()->GetOverlayDisplayName(overlay), x,y, UTIL_ANCHOR_UPPER_LEFT,
            "Arial", 12, 0, UTIL_BG_NONE, UTIL_COLOR_BLACK,
            UTIL_COLOR_WHITE, 0, cpt, TRUE);
      }
   }
   
   // draw the line for each overlay in the gant chart; use the TMARK line style
   // so that the endcaps of the lines are square rather than round
   for(i=start_index;i<=end_index;i++)
   {
      C_overlay *overlay = OVL_get_overlay_manager()->get_time_sensitive_overlay(i);

      PlaybackTimeSegment_Interface *pFvTimeSegment = dynamic_cast<PlaybackTimeSegment_Interface *>(overlay);
      if (pFvTimeSegment == NULL || pFvTimeSegment->SupportsFvPlaybackTimeSegment() == FALSE)
         continue;
		
      // calculate the height in the window at which the line will be drawn
      int y = (i - start_index + !start_at_top)*25 + 15;
		
      // get the beginning and ending times for the overlay
      DATE begin, end;
      long bTimeSegmentValid;
      pFvTimeSegment->GetTimeSegment(&begin, &end, &bTimeSegmentValid);

      if (!bTimeSegmentValid)
         continue;

      COLORREF color; 
      pFvTimeSegment->GetGanntChartColor(&color);
      util.set_pen(color, UTIL_LINE_TMARK, 3);
		
      // calculate the starting and ending x
      int x_start = futil->round((begin - 
         (DATE)all_begin)*((double)rect.Width()/((DATE)all_end-(DATE)all_begin)));
      int x_end = futil->round((end - 
         (DATE)all_begin)*((double)rect.Width()/((DATE)all_end-(DATE)all_begin)));

      // if the line's endpoints are equal (as is the case in the extreme
      // case when two trails with largely differing dates are opened) then
      // we need to bump up the ending point by one in the case when the 
      // line is on the left and bump down the starting point when the 
      // line is on the right to insure that the line is displayed
      if (x_start == 0 && x_start == x_end)
         x_end++;
      else if (x_start == x_end)
         x_start--;

      util.draw_line(x_start,y,x_end,y);
   }
	
   // copy contents of memory dc into the preview window's dc
   dc->BitBlt(0, 0, rect.Width(), rect.Height(), &memory_dc, 
      rect.TopLeft().x, rect.TopLeft().y, SRCCOPY);
	
   ReleaseDC(dc);
}
