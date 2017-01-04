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



// colordlg.cpp : implementation file
//

#include "stdafx.h"
#include "fvwutil.h"
#include "colordlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawColorDlg dialog


CDrawColorDlg::CDrawColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawColorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_right_button = FALSE;
	m_back_checked = FALSE;
	m_no_radio_buttons = FALSE;
	m_title = "Colors";
}


void CDrawColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawColorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawColorDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawColorDlg)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
   ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_01, IDC_BUTTON_20, OnColorButton)
	ON_BN_CLICKED(IDC_BACK_CHECK, OnBackCheck)
	ON_BN_CLICKED(IDC_RB_BACK_COLOR, OnRbBackColor)
	ON_BN_CLICKED(IDC_RB_COLOR, OnRbColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawColorDlg message handlers

void CDrawColorDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

// **************************************************************************

BOOL CDrawColorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckRadioButton(IDC_RB_COLOR, IDC_RB_BACK_COLOR, IDC_RB_COLOR);
	m_back_checked = FALSE;
	SetWindowText(m_title);
	if (m_no_radio_buttons)
	{
		CWnd* wnd;
		wnd = GetDlgItem(IDC_RB_COLOR);
		wnd->ShowWindow(SW_HIDE);
		wnd = GetDlgItem(IDC_RB_BACK_COLOR);
		wnd->ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// **************************************************************************

void CDrawColorDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawColorDlg::OnRbBackColor() 
{
	m_back_checked = TRUE;
}

void CDrawColorDlg::OnRbColor() 
{
	m_back_checked = FALSE;
}
// **************************************************************************

void CDrawColorDlg::OnColorButton(UINT button_id)
{
	int color;
   switch(button_id)
   {
    	case IDC_BUTTON_01:
         color = UTIL_COLOR_BLACK;
        	break;
      case IDC_BUTTON_02:
         color = UTIL_COLOR_DARK_RED;
        	break;
      case IDC_BUTTON_03:
         color = UTIL_COLOR_DARK_GREEN;
        	break;
      case IDC_BUTTON_04:
         color = UTIL_COLOR_DARK_YELLOW;
        	break;
      case IDC_BUTTON_05:
         color = UTIL_COLOR_DARK_BLUE;
        	break;
      case IDC_BUTTON_06:
         color = UTIL_COLOR_DARK_MAGENTA;
        	break;
      case IDC_BUTTON_07:
         color = UTIL_COLOR_DARK_CYAN;
        	break;
      case IDC_BUTTON_08:
         color = UTIL_COLOR_LIGHT_GRAY;
        	break;
      case IDC_BUTTON_09:
         color = UTIL_COLOR_MONEY_GREEN;
        	break;
      case IDC_BUTTON_10:
         color = UTIL_COLOR_SKY_BLUE;
        	break;
     	case IDC_BUTTON_11:
         color = UTIL_COLOR_CREAM;
        	break;
      case IDC_BUTTON_12:
         color = UTIL_COLOR_MEDIUM_GRAY;
        	break;
      case IDC_BUTTON_13:
         color = UTIL_COLOR_DARK_GRAY;
        	break;
      case IDC_BUTTON_14:
         color = UTIL_COLOR_RED;
        	break;
      case IDC_BUTTON_15:
         color = UTIL_COLOR_GREEN;
        	break;
      case IDC_BUTTON_16:
         color = UTIL_COLOR_YELLOW;
        	break;
      case IDC_BUTTON_17:
         color = UTIL_COLOR_BLUE;
        	break;
	   case IDC_BUTTON_18:
         color = UTIL_COLOR_MAGENTA;
        	break;
	   case IDC_BUTTON_19:
         color = UTIL_COLOR_CYAN;
        	break;
	   case IDC_BUTTON_20:
         color = UTIL_COLOR_WHITE;
        	break;
		default:
			return;
			break;
   }
	if (m_back_checked)
		m_back_color = color;
	else
		m_color = color;
	OnOK();
}
// end of OnColorButton

// **************************************************************************

int CDrawColorDlg::get_color()
{
	return m_color;
}
// end of get_color
	
// **************************************************************************
// **************************************************************************

void CDrawColorDlg::set_color(int color)
{
	m_color = color;
}
// end of set_color
	
// **************************************************************************
// **************************************************************************

int CDrawColorDlg::get_back_color()
{
	return m_back_color;
}
// end of get_back_color
	
// **************************************************************************
// **************************************************************************

void CDrawColorDlg::set_back_color(int color)
{
	m_back_color = color;
}
// end of set_back_color
	
// **************************************************************************
// **************************************************************************

void CDrawColorDlg::DrawColorButton(int colorcode, DRAWITEMSTRUCT* pDI)
{
	CPen darkpen;
	CPen lightpen;
	CBrush brush;
	COLORREF color;
	CFvwUtil *util = CFvwUtil::get_instance();

	color = util->code2color(colorcode);
    switch (pDI->itemAction) 
    {
    	case ODA_DRAWENTIRE:
    	case ODA_SELECT: 
    		{
        		// Create a CDC object.
        		CDC dc;
        		dc.Attach(pDI->hDC);
        		// Draw the black rectangle around the outside
        		// and fill the center with either the off or on color.
				darkpen.CreatePen(PS_SOLID, 2, RGB (0, 0, 0));
				lightpen.CreatePen(PS_SOLID, 2, RGB (255, 255, 255));
				brush.CreateSolidBrush(color);
				CPen *oldpen = dc.SelectObject(&darkpen);
				CBrush *oldbrush = dc.SelectObject(&brush);
				dc.Rectangle(&(pDI->rcItem));
				dc.SelectObject(&lightpen);
				dc.MoveTo(pDI->rcItem.right-1, pDI->rcItem.top);
				dc.LineTo(pDI->rcItem.right-1, pDI->rcItem.bottom-1);
				dc.LineTo(pDI->rcItem.left, pDI->rcItem.bottom-1);
				dc.SelectObject(oldbrush);
				dc.SelectObject(oldpen);
				brush.DeleteObject();
				darkpen.DeleteObject();
				lightpen.DeleteObject();
        	} 
        	break;
    	default:
        	break;
    }
}
// end of DrawColorButton
  
// **************************************************************************

void CDrawColorDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    switch (nIDCtl) 
    {
    	case IDC_BUTTON_01:
        	DrawColorButton(UTIL_COLOR_BLACK, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_02:
			DrawColorButton(UTIL_COLOR_DARK_RED, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_03:
			DrawColorButton(UTIL_COLOR_DARK_GREEN, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_04:
			DrawColorButton(UTIL_COLOR_DARK_YELLOW, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_05:
			DrawColorButton(UTIL_COLOR_DARK_BLUE, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_06:
			DrawColorButton(UTIL_COLOR_DARK_MAGENTA, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_07:
			DrawColorButton(UTIL_COLOR_DARK_CYAN, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_08:
			DrawColorButton(UTIL_COLOR_LIGHT_GRAY, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_09:
			DrawColorButton(UTIL_COLOR_MONEY_GREEN, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_10:
			DrawColorButton(UTIL_COLOR_SKY_BLUE, lpDrawItemStruct);
        	break;
    	case IDC_BUTTON_11:
        	DrawColorButton(UTIL_COLOR_CREAM, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_12:
			DrawColorButton(UTIL_COLOR_MEDIUM_GRAY, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_13:
			DrawColorButton(UTIL_COLOR_DARK_GRAY, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_14:
			DrawColorButton(UTIL_COLOR_RED, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_15:
			DrawColorButton(UTIL_COLOR_GREEN, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_16:
			DrawColorButton(UTIL_COLOR_YELLOW, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_17:
			DrawColorButton(UTIL_COLOR_BLUE, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_18:
			DrawColorButton(UTIL_COLOR_MAGENTA, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_19:
			DrawColorButton(UTIL_COLOR_CYAN, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_20:
			DrawColorButton(UTIL_COLOR_WHITE, lpDrawItemStruct);
        	break;
//	    case IDC_BUTTON_CUR_COLOR:
//			DrawCurrentColor();
//        	break;
			
	    default:
    	    return;
    }
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
// end of OnDrawItem

// **************************************************************************
// **************************************************************************

void CDrawColorDlg::DrawCurrentColor()
{
	RECT rc;
	CWnd* wnd;

	wnd = GetDlgItem(IDC_BUTTON_CUR_COLOR);
	wnd->GetClientRect(&rc);
	CWindowDC dc(wnd);
	DrawColorBox(&dc, rc, m_back_color);
	rc.left += 8;
	rc.right -= 8;
	rc.top += 8;
	rc.bottom -= 8;
	DrawColorBox(&dc, rc, m_color);
}
// end of DrawCurrentColor

// **************************************************************************
// **************************************************************************

void CDrawColorDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
//	DrawCurrentColor();

	
	// Do not call CPropertyPage::OnPaint() for painting messages
}
// end of OnPaint

// **************************************************************************

void CDrawColorDlg::DrawColorBox(CDC *dc, RECT rc, int colorcode)
{
	CPen pen;
	CBrush brush;
	COLORREF color;
	CFvwUtil *util = CFvwUtil::get_instance();

	color = util->code2color(colorcode);
	pen.CreatePen(PS_SOLID, 1, RGB (0,0,0));
	brush.CreateSolidBrush(color);
	CPen *oldpen = dc->SelectObject(&pen);
	CBrush *oldbrush = dc->SelectObject(&brush);
    dc->Rectangle(rc.left + 1,
       		      rc.top + 1,
	               rc.right - 1,
                  rc.bottom - 1);
	dc->SelectObject(oldbrush);
	dc->SelectObject(oldpen);
	brush.DeleteObject();
	pen.DeleteObject();
}
// end of DrawColorBox


// **************************************************************************
// **************************************************************************

void CDrawColorDlg::OnBackCheck() 
{
	m_back_checked = SendDlgItemMessage(IDC_BACK_CHECK, BM_GETCHECK, 0, 0);

}
// end of OnBackCheck
// **************************************************************************
// **************************************************************************

void CDrawColorDlg::set_title(CString title) 
{
	m_title = title;
}
// end of set_title

// **************************************************************************
// *************************************************************

void CDrawColorDlg::draw_basic_button(CDC * dc, RECT rc)
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
// **************************************************************************

void CDrawColorDlg::draw_color_button(CWnd *cwnd, int button_id, int color)
{
	CPen lightpen, darkpen;
	CBrush brush;
	CFvwUtil *futil = CFvwUtil::get_instance();
	CWnd *wnd;
	RECT rc;

	wnd = cwnd->GetDlgItem(button_id);
	CWindowDC dc(wnd);
	wnd->GetClientRect(&rc);

	draw_basic_button(&dc, rc);

	darkpen.CreatePen(PS_SOLID, 2, futil->code2color(UTIL_COLOR_BLACK));
	lightpen.CreatePen(PS_SOLID, 2, futil->code2color(UTIL_COLOR_WHITE));
	brush.CreateSolidBrush(futil->code2color(color));
	CPen *oldpen = dc.SelectObject(&darkpen);
	CBrush *oldbrush = dc.SelectObject(&brush);
	dc.Rectangle(&rc);
	dc.SelectObject(&lightpen);
	dc.MoveTo(rc.right-1, rc.top);
	dc.LineTo(rc.right-1, rc.bottom-1);
	dc.LineTo(rc.left, rc.bottom-1);
	dc.SelectObject(oldbrush);
	dc.SelectObject(oldpen);
	brush.DeleteObject();
	darkpen.DeleteObject();
	lightpen.DeleteObject();
}
// end of draw_color_button
  
// **************************************************************************
// **************************************************************************

void CDrawColorDlg::draw_color_button_rgb(CWnd *cwnd, int button_id, COLORREF color)
{
	CPen lightpen, darkpen;
	CBrush brush;
	CFvwUtil *futil = CFvwUtil::get_instance();
	CWnd *wnd;
	RECT rc;

	wnd = cwnd->GetDlgItem(button_id);
	CWindowDC dc(wnd);
	wnd->GetClientRect(&rc);

	draw_basic_button(&dc, rc);

	darkpen.CreatePen(PS_SOLID, 2, futil->code2color(UTIL_COLOR_BLACK));
	lightpen.CreatePen(PS_SOLID, 2, futil->code2color(UTIL_COLOR_WHITE));
	brush.CreateSolidBrush(color);
	CPen *oldpen = dc.SelectObject(&darkpen);
	CBrush *oldbrush = dc.SelectObject(&brush);
	dc.Rectangle(&rc);
	dc.SelectObject(&lightpen);
	dc.MoveTo(rc.right-1, rc.top);
	dc.LineTo(rc.right-1, rc.bottom-1);
	dc.LineTo(rc.left, rc.bottom-1);
	dc.SelectObject(oldbrush);
	dc.SelectObject(oldpen);
	brush.DeleteObject();
	darkpen.DeleteObject();
	lightpen.DeleteObject();
}
// end of draw_color_button_rgb
  
// **************************************************************************

// **************************************************************************
// end of CDrawColorDlg functions
// **************************************************************************
// **************************************************************************

// **************************************************************************
