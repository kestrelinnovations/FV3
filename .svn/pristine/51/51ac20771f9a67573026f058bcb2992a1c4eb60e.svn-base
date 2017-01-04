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

// dlgutil.cpp

#include "stdafx.h"
#include "dlgutil.h"
#include "ovlutil.h" 
// temporary include
#include "map.h"

CDlgUtil::CDlgUtil()
{
}

// *************************************************************

void CDlgUtil::draw_basic_button(CDC * dc, RECT rc)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	CPen darkpen, graypen, lightpen;
	CBrush graybrush;

	graybrush.CreateSolidBrush(futil->code2color(UTIL_COLOR_MONEY_GREEN));
	darkpen.CreatePen(PS_SOLID, 1, RGB (0, 0, 0));
	graypen.CreatePen(PS_SOLID, 1, futil->code2color(UTIL_COLOR_DARK_GRAY));
	lightpen.CreatePen(PS_SOLID, 1, RGB (255, 255, 255));
	CPen *oldpen = dc->SelectObject(&graypen);
	CBrush *oldbrush = dc->SelectObject(&graybrush);

	futil->rectangle(dc, &(rc));

	dc->SelectObject(&lightpen);

	futil->move_to(dc, rc.right-1, rc.top);
	futil->line_to(dc, rc.right-1, rc.bottom-1);
	futil->line_to(dc, rc.left, rc.bottom-1);

	dc->SelectObject(oldbrush);
	dc->SelectObject(oldpen);
	graybrush.DeleteObject();
	darkpen.DeleteObject();
	graypen.DeleteObject();
	lightpen.DeleteObject();
}
// end of draw_basic_button

// *************************************************************
// *************************************************************

void CDlgUtil::draw_width_button(CWnd *button_wnd, int width)
{
	CPen blackpen;
	CBrush brush;
	int cy;
	int k, width2;
	RECT rc;

	CWindowDC dc(button_wnd);
	button_wnd->GetClientRect(&rc);

	draw_basic_button(&dc, rc);

	blackpen.CreatePen(PS_SOLID, 1, RGB (0,0,0));
	CPen *oldpen = dc.SelectObject(&blackpen);
	cy = (rc.bottom - rc.top) / 2;
	width2 = width / 2;
	for (k=0; k<width; k++)
	{
		dc.MoveTo(rc.left + 5, cy - width2 + k);
		dc.LineTo(rc.right - 5, cy - width2 + k);
	}
	dc.SelectObject(oldpen);
	blackpen.DeleteObject();
}
// end of draw_width_button
  
// *************************************************************
// *************************************************************

void CDlgUtil::draw_style_button(CWnd *button_wnd, int style)
{
	COvlkitUtil util;
	int cy;
	RECT rc;

	CWindowDC dc(button_wnd);
	button_wnd->GetClientRect(&rc);

	draw_basic_button(&dc, rc);

	cy = (rc.bottom - rc.top) / 2;
	switch (style)
	{
		case UTIL_LINE_FEBA_START:	cy	+= 5; break;
		case UTIL_LINE_FEBA_END:	cy	-= 5; break;
		case UTIL_LINE_FLOT_START:	cy	+= 5; break;
		case UTIL_LINE_FLOT_END:	cy	-= 5; break;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// temp fix, change as soon as new draw_line not requiring map is implemented
	CView* view;
	ViewMapProj* map;

	view = UTL_get_active_non_printing_view();
	if (view == NULL)
		return;
	map = UTL_get_current_view_map(view);
	if (map == NULL)
		return;

	if (!map->is_projection_set())
		return;
	util.draw_line(map, &dc, rc.left+8, cy, rc.right - 8, cy, UTIL_COLOR_BLACK, -1, style, 2);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
// end of draw_style_button
  
// *************************************************************
// *************************************************************

void CDlgUtil::draw_fill_button(CWnd *button_wnd, int fill_type)
{
	CBrush brush;
	CBrush* oldbrush;
	CPen lightpen;
	COLORREF back_color;
	int colorcode;
	CFvwUtil *futil = CFvwUtil::get_instance();
	COvlkitUtil util;
	RECT rc;
	POINT cpt[4];

	button_wnd->GetClientRect(&rc);
	CWindowDC dc(button_wnd);

	draw_basic_button(&dc, rc);

	colorcode = UTIL_COLOR_BLUE;
	back_color = util.code2color(colorcode);
	BOOL fill = TRUE;
	switch (fill_type)
	{
		case UTIL_FILL_NONE:
			futil->draw_text(&dc, "No", rc.left + 6, rc.top + 9, 
				UTIL_ANCHOR_UPPER_LEFT, "Arial", 16, 0, UTIL_BG_NONE, 
				UTIL_COLOR_BLACK, UTIL_COLOR_MONEY_GREEN, 0.0, cpt);
			fill = FALSE;
			break;
		case UTIL_FILL_SOLID:
			brush.CreateSolidBrush(back_color);
			break;
		case UTIL_FILL_HORZ:
		case UTIL_FILL_VERT:
		case UTIL_FILL_BDIAG:
		case UTIL_FILL_FDIAG:
		case UTIL_FILL_CROSS:
		case UTIL_FILL_DIAGCROSS:
			brush.CreateHatchBrush(futil->code2fill(fill_type), back_color);
			break;
		default:
			fill = FALSE;
	}
	if (fill)
	{
		oldbrush = (CBrush*) dc.SelectObject(&brush);
		int oldbkmode = dc.SetBkMode(TRANSPARENT);
		dc.Rectangle(&rc);
		lightpen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* oldpen = (CPen*) dc.SelectObject(&lightpen);
		dc.MoveTo(rc.right-1, rc.top);
		dc.LineTo(rc.right-1, rc.bottom-1);
		dc.LineTo(rc.left, rc.bottom-1);
		dc.SelectObject(oldbrush);
		brush.DeleteObject();
		dc.SetBkMode(oldbkmode);
		dc.SelectObject(oldpen);
		lightpen.DeleteObject();
	}
}
// end of draw_fill_button

// **************************************************************************
