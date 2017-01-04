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

// PreviewEdit.cpp : implementation file
//

#include "stdafx.h"
#include "utils.h"
#include "TextParser.h"
#include "TextImport_Step2B.h"

/////////////////////////////////////////////////////////////////////////////
// PreviewEdit

PreviewEdit::PreviewEdit()
{
	m_brush.CreateSolidBrush(RGB(255,255,255));
	m_scroll_pos = 0;
}

void PreviewEdit::set_text_parser(TextParser *parser)
{
	m_text_parser = parser;
}

PreviewEdit::~PreviewEdit()
{
}


BEGIN_MESSAGE_MAP(PreviewEdit, CEdit)
	//{{AFX_MSG_MAP(PreviewEdit)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PreviewEdit message handlers

void PreviewEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTextImport_Step2B * parent = (CTextImport_Step2B *)GetParent();

	ClientToScreen(&point);
	parent->ScreenToClient(&point);

	parent->remove_position(point);
	parent->set_in_drag(true);

	CRect rect;
	GetWindowRect(&rect);
   parent->ScreenToClient(rect);

	parent->set_drag_pos(parent->get_position_from_point(rect, point));
}

void PreviewEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	CTextImport_Step2B * parent = (CTextImport_Step2B *)GetParent();
	ClientToScreen(&point);
	parent->ScreenToClient(&point);
	parent->on_mouse_move(point);
	
	CEdit::OnMouseMove(nFlags, point);
}


void PreviewEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTextImport_Step2B * parent = (CTextImport_Step2B *)GetParent();

	if (parent->get_in_drag())
	{
		ClientToScreen(&point);
		parent->ScreenToClient(&point);

		parent->add_position(point);
		parent->set_in_drag(false);
	}
}

void PreviewEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTextImport_Step2B * parent = (CTextImport_Step2B *)GetParent();

	ClientToScreen(&point);
	parent->ScreenToClient(&point);

	parent->remove_position(point);
}

HBRUSH PreviewEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return m_brush;
}

void PreviewEdit::OnPaint() 
{
	{
		CDC *dc = GetDC();
		CRgn rgn;
		CRect rect;
		GetWindowRect(&rect);
		ScreenToClient(rect);
		rgn.CreateRectRgn(rect.TopLeft().x, rect.TopLeft().y,
			rect.BottomRight().x, rect.BottomRight().y);
		
		dc->SelectClipRgn(&rgn, RGN_OR);

		ReleaseDC(dc);
	}

	CEdit::OnPaint();

	CTextImport_Step2B * parent = (CTextImport_Step2B *)GetParent();
	
	CClientDC dc(this); // device context for painting
	
	UtilDraw util(&dc);
	util.set_pen(RGB(0,0,0), UTIL_LINE_SOLID, 1);

	CRect rect;
	GetWindowRect(&rect);
	const int x1 = 0;
	const int x2 = rect.Width() - 1;
	const int y = 0;
	const int y2 = rect.Height();

	int text_width;
	dc.GetCharWidth('1','1', &text_width);
	
	TextParserProperties *prop = (TextParserProperties *)m_text_parser->get_properties();
	int num_pos = prop->get_num_field_positions();
	for (int i=0;i<num_pos;i++)
	{
		int pos = prop->get_field_position_at(i);
	
		int x_val = x1+text_width*(pos-m_scroll_pos);
		if (x_val >= x1 && x_val <= x2)
			util.draw_line(x_val, y, x_val, y2);
	}

	if (parent->get_in_drag())
	{
		int pos = parent->get_drag_pos();
		util.set_pen(RGB(0,0,0), UTIL_LINE_ROUND_DOT, 1);

		int x_val = x1+text_width*(pos-m_scroll_pos);
		if (x_val >= x1 && x_val <= x2)
		{
			util.draw_line(x_val, y, x_val, y2);
			util.draw_line(x_val, y + 2, x_val, y2);
			util.draw_line(x_val + 1, y +1, x_val+1, y2-1);
			util.draw_line(x_val + 1, y +3, x_val+1, y2-1);
		}
	}
}
