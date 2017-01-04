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

// TextImport_Step2B.cpp : implementation file
//

#include "stdafx.h"
#include "TextImport_Step2B.h"
#include "TextParser.h"
#include "err.h"
#include "ovlelem.h"
#include "param.h"

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step2B property page

const int OFFSET = 12;

IMPLEMENT_DYNCREATE(CTextImport_Step2B, CPropertyPage)

CTextImport_Step2B::CTextImport_Step2B(TextParser *text_parser /*= NULL*/,
													ImportDataObject *data_object /*= NULL */) : 
	CImportPropertyPage(CTextImport_Step2B::IDD), m_text_parser(text_parser), m_data_object(data_object)
{
	//{{AFX_DATA_INIT(CTextImport_Step2B)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_text_width = 0;
	m_in_drag = false;
}

CTextImport_Step2B::~CTextImport_Step2B()
{
}

void CTextImport_Step2B::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextImport_Step2B)
	DDX_Control(pDX, IDC_EDIT, m_edit_ctrl);
	DDX_Control(pDX, IDC_HORZ_SCROLL, m_horz_scroll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextImport_Step2B, CPropertyPage)
	//{{AFX_MSG_MAP(CTextImport_Step2B)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_EN_SETFOCUS(IDC_EDIT, OnSetfocusEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step2B message handlers

BOOL CTextImport_Step2B::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_edit_ctrl.set_text_parser(m_text_parser);
	
	// create the font if it has not already been created
	if ((HFONT)m_font == NULL)
	{
		OvlFont ovl_font; 
		ovl_font.set_font("Courier", 7);
		ovl_font.CreateFont(m_font);
	}

	m_edit_ctrl.SetFont(&m_font);
	m_edit_ctrl.SetReadOnly(TRUE);

	int max_char_width = 0;
	m_text_parser->open_file();
	{
		CString txt;
		int count = 0;
		const int max_num_lines_to_read = 10;
		
		while (count < max_num_lines_to_read)
		{
			CString line;
			if (m_text_parser->get_line(line) != SUCCESS)
			{
				ERR_report("get_line failed");
				break;
			}

			if (line.GetLength() > max_char_width)
				max_char_width = line.GetLength();
			
			txt += line;
			txt += "\n";
			count++;
		}

		m_edit_ctrl.SetWindowText(txt);
	}
	m_text_parser->close_file();

	m_edit_ctrl.SetSel(-1, -1);

	const int edit_width = 57;
	int maxPos = max_char_width - edit_width;
	if (maxPos <= 0)
		m_horz_scroll.EnableWindow(FALSE);
	else
	{
		SCROLLINFO scroll_info;

		scroll_info.cbSize = sizeof(SCROLLINFO);
		scroll_info.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		scroll_info.nMin = 0;
		scroll_info.nMax = maxPos;
		scroll_info.nPage = PRM_get_registry_int("Route Options","Page Size", 0);
		scroll_info.nPos = 0;

		m_horz_scroll.SetScrollInfo(&scroll_info);
	}

   Reinit();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextImport_Step2B::Reinit()
{
   if (GetDlgItem(IDC_INVALIDATE) == NULL)
      return;

   // invalidate the data preview region
   CRect rect;
   GetDlgItem(IDC_INVALIDATE)->GetWindowRect(&rect);
   ScreenToClient(rect);
   InvalidateRect(&rect);
   
   m_data_object->update();
}

void CTextImport_Step2B::OnPaint() 
{
	CRect rect;
	CPaintDC dc(this); // device context for painting

	UtilDraw util(&dc);
	util.set_pen(RGB(0,0,0), UTIL_LINE_SOLID, 2);

	GetDlgItem(IDC_EDIT)->GetWindowRect(&rect);
   ScreenToClient(rect);

	// draw border of edit control
	util.draw_rectangle(FALSE, rect.TopLeft().x, rect.TopLeft().y, rect.BottomRight().x, 
		rect.BottomRight().y);

	int scroll_pos = m_horz_scroll.GetScrollPos();

	const int x1 = rect.TopLeft().x;
	const int x2 = rect.BottomRight().x;
	const int y = rect.TopLeft().y;
	const int y2 = rect.BottomRight().y;

	// draw ruler about edit control
	{
		CFont *old_font = dc.SelectObject(&m_font);
		dc.GetCharWidth('1','1', &m_text_width);

		util.set_pen(RGB(0,0,0), UTIL_LINE_SOLID, 1);
		util.draw_line(x1, y - OFFSET, x2 - 1, y - OFFSET);

		util.set_pen(RGB(255,255,255), UTIL_LINE_SOLID, 1);
		util.draw_line(x1, y - OFFSET + 1, x2 - 1, y - OFFSET + 1);	

		// draw ticks
		util.set_pen(RGB(0,0,0), UTIL_LINE_SOLID, 1);
		int width = rect.Width() - 1;
		int i;
		for(i=0;i<width;i+=m_text_width)
			util.draw_line(x1+i, y - OFFSET - 1, x1+i, y-OFFSET-2);

		for(i=-scroll_pos*m_text_width;i<=width;i+=m_text_width*5)
		{
			if (i >= 0)
				util.draw_line(x1+i, y - OFFSET - 1, x1+i, y-OFFSET-4);
		}

		util.set_pen(RGB(0,0,0), UTIL_LINE_SOLID, 2);
		for(i=0;i<=width+scroll_pos*m_text_width;i+=m_text_width*10)
		{
			int x_val = x1+i - (scroll_pos*m_text_width);
			if (x_val >= x1)
				util.draw_line(x_val, y - OFFSET - 1, x_val, y-OFFSET-5);
		}

		if (m_text_width != 0)
		{
			for(i=m_text_width*10;i<width+scroll_pos*m_text_width;i+=m_text_width*10)
			{
				CString str;
				str.Format("%d", i/m_text_width);
				int x_val = x1+i - m_text_width - (scroll_pos*m_text_width);
				if (x_val >= x1)
					dc.TextOut(x_val, y-OFFSET-20, str);
			}
		}


		dc.SelectObject(old_font);
	}

	// draw field positions
	{
		TextParserProperties *prop = (TextParserProperties *)m_text_parser->get_properties();
		int num_pos = prop->get_num_field_positions();
		for (int i=0;i<num_pos;i++)
		{
			int pos = prop->get_field_position_at(i);

			util.set_pen(RGB(0,0,0), UTIL_LINE_SOLID, 1);
			int x_val = x1+m_text_width*(pos-scroll_pos);
			if (x_val >= x1 && x_val <= x2)
			{
				util.draw_line(x_val, y - OFFSET, x_val, y2);

				// draw arrow head
				POINT arrow[3];
				arrow[0].x = x_val;
				arrow[0].y = y - OFFSET + 2;
				arrow[1].x = x_val - 3;
				arrow[1].y = y - OFFSET + 5;
				arrow[2].x = x_val + 3;
				arrow[2].y = y - OFFSET + 5;
				util.draw_polygon(TRUE, arrow, 3);
			}
		}
	}

	// drag drag XOR line
	if (m_in_drag)
	{
		util.set_pen(RGB(0,0,0), UTIL_LINE_ROUND_DOT, 1);
		int x_val = x1+m_text_width*(m_drag_pos-scroll_pos);
		if (x_val >= x1 && x_val <= x2)
		{
			util.draw_line(x_val, y - OFFSET, x_val, y2);
			util.draw_line(x_val, y - OFFSET + 2, x_val, y2);
			util.draw_line(x_val+1, y - OFFSET+1, x_val+1, y2-1);
			util.draw_line(x_val+1, y - OFFSET+3, x_val+1, y2-1);
		}
	}
}

// This is the second wizard property page. Enable both the 
// Back button and the Next button.
BOOL CTextImport_Step2B::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_horz_scroll.SetScrollPos(0);
	m_edit_ctrl.set_scroll_pos(0);
	
	return CPropertyPage::OnSetActive();
}

void CTextImport_Step2B::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int min, max;
	pScrollBar->GetScrollRange(&min, &max);

	int old_pos = pScrollBar->GetScrollPos();

	if (nSBCode == SB_THUMBPOSITION ||
		 nSBCode == SB_THUMBTRACK)
	{
		pScrollBar->SetScrollPos(nPos);
	}
	else if (nSBCode == SB_LINELEFT || nSBCode == SB_PAGELEFT)
	{
		int pos = pScrollBar->GetScrollPos();
		pScrollBar->SetScrollPos(max(pos--,min));
	}
	else if (nSBCode == SB_LINERIGHT || nSBCode == SB_PAGERIGHT)
	{
		int pos = pScrollBar->GetScrollPos();
		pScrollBar->SetScrollPos(min(pos++,max));
	}

	int new_pos = pScrollBar->GetScrollPos();

	CEdit *edit = (CEdit *)GetDlgItem(IDC_EDIT);
	edit->LineScroll(0, new_pos - old_pos);

	m_edit_ctrl.set_scroll_pos(new_pos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);


	// invalidate the data preview region
	CRect rect;
	GetDlgItem(IDC_INVALIDATE)->GetWindowRect(&rect);
   ScreenToClient(rect);
	InvalidateRect(&rect);
}

void CTextImport_Step2B::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetDlgItem(IDC_EDIT)->GetWindowRect(&rect);
   ScreenToClient(rect);

	remove_position(point);

	m_drag_pos = get_position_from_point(rect, point);
	if (m_drag_pos != -1)
		m_in_drag = true;
	
	// invalidate the data preview region
	GetDlgItem(IDC_INVALIDATE)->GetWindowRect(&rect);
   ScreenToClient(rect);
	InvalidateRect(&rect);

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CTextImport_Step2B::OnMouseMove(UINT nFlags, CPoint point) 
{
	on_mouse_move(point);
	
	CPropertyPage::OnMouseMove(nFlags, point);
}

void CTextImport_Step2B::on_mouse_move(const CPoint &point)
{
	if (m_in_drag)
	{
		CRect rect;
		GetDlgItem(IDC_EDIT)->GetWindowRect(&rect);
		ScreenToClient(rect);
		
		const int x1 = rect.TopLeft().x;
		const int y = rect.TopLeft().y;
		const int y2 = rect.BottomRight().y;
		
		int old_drag_pos = m_drag_pos;
		m_drag_pos = get_position_from_point(rect, point);		
		
		if (old_drag_pos != m_drag_pos && m_drag_pos != -1)
		{
			// invalidate the data preview region
			GetDlgItem(IDC_INVALIDATE)->GetWindowRect(&rect);
			ScreenToClient(rect);
			InvalidateRect(&rect);
		}
	}
}

void CTextImport_Step2B::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_in_drag)
	{
		add_position(point);
		m_in_drag = false;
	}

	CPropertyPage::OnLButtonUp(nFlags, point);
}

int CTextImport_Step2B::get_position_from_point(const CRect &rect, const CPoint &point)
{
	int offset_x = point.x - rect.TopLeft().x;
	if (offset_x < m_text_width || offset_x > rect.Width())
		return -1;

	double d = offset_x;
	if (m_text_width != 0.0)
		d /= m_text_width;

	return (int)(d + m_horz_scroll.GetScrollPos() + 0.5);
}

void CTextImport_Step2B::add_position(CPoint point)
{
	CRect rect;
	GetDlgItem(IDC_EDIT)->GetWindowRect(&rect);
   ScreenToClient(rect);

	int pos = get_position_from_point(rect, point);

	((TextParserProperties *)m_text_parser->get_properties())->add_field_position(pos);

	// invalidate the data preview region
	GetDlgItem(IDC_INVALIDATE)->GetWindowRect(&rect);
   ScreenToClient(rect);
	InvalidateRect(&rect);

	m_data_object->update();
}

void CTextImport_Step2B::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	remove_position(point);
	CPropertyPage::OnLButtonDblClk(nFlags, point);
}

void CTextImport_Step2B::remove_position(CPoint point)
{
	CRect rect;
	GetDlgItem(IDC_EDIT)->GetWindowRect(&rect);
   ScreenToClient(rect);

	int offset_x = point.x - rect.TopLeft().x;
	if (offset_x < m_text_width || offset_x > rect.Width())
		return;

	int scroll_pos = m_horz_scroll.GetScrollPos();

	double d = offset_x;
	if (m_text_width != 0.0)
		d /= m_text_width;
	int pos = (int)(d + scroll_pos + 0.5);

	((TextParserProperties *)m_text_parser->get_properties())->remove_field_position(pos);
	
	// invalidate the data preview region
	GetDlgItem(IDC_INVALIDATE)->GetWindowRect(&rect);
   ScreenToClient(rect);
	InvalidateRect(&rect);

	m_data_object->update();
}

void CTextImport_Step2B::OnSetfocusEdit() 
{
	m_edit_ctrl.SetSel(-1, -1);
}
