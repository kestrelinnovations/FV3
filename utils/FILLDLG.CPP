// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// filldlg.cpp : implementation file
//

#include "stdafx.h"
#include "filldlg.h"
#include "fvwutil.h"
#include "ovlutil.h"
#include "..\getobjpr.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawFillDlg dialog


CDrawFillDlg::CDrawFillDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawFillDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawFillDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


// **************************************************************************
// **************************************************************************

void CDrawFillDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawFillDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


// **************************************************************************
// **************************************************************************

BEGIN_MESSAGE_MAP(CDrawFillDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawFillDlg)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_BN_CLICKED(IDC_FILL_BACK_SLASH, OnFillBackSlash)
	ON_BN_CLICKED(IDC_FILL_DIAMOND_HASH, OnFillDiamondHash)
	ON_BN_CLICKED(IDC_FILL_FORWD_SLASH, OnFillForwdSlash)
	ON_BN_CLICKED(IDC_FILL_HORZLINE, OnFillHorzline)
	ON_BN_CLICKED(IDC_FILL_NONE, OnFillNone)
	ON_BN_CLICKED(IDC_FILL_SOLID, OnFillSolid)
	ON_BN_CLICKED(IDC_FILL_SQUARE_HASH, OnFillSquareHash)
	ON_BN_CLICKED(IDC_FILL_VERTLINE, OnFillVertline)
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawFillDlg message handlers

void CDrawFillDlg::OnFillBackSlash() 
{
	m_fill_style = UTIL_FILL_BDIAG;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::OnFillDiamondHash() 
{
	m_fill_style = UTIL_FILL_DIAGCROSS;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::OnFillForwdSlash() 
{
	m_fill_style = UTIL_FILL_FDIAG;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::OnFillHorzline() 
{
	m_fill_style = UTIL_FILL_HORZ;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::OnFillNone() 
{
	m_fill_style = UTIL_FILL_NONE;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::OnFillSolid() 
{
	m_fill_style = UTIL_FILL_SOLID;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::OnFillSquareHash() 
{
	m_fill_style = UTIL_FILL_CROSS;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::OnFillVertline() 
{
	m_fill_style = UTIL_FILL_VERT;	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::set_fill_style(int fill_style) 
{
	m_fill_style = fill_style;
}

// **************************************************************************
// **************************************************************************


int CDrawFillDlg::get_fill_style() 
{
	return m_fill_style;
}

// **************************************************************************
// **************************************************************************

void CDrawFillDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}
// **************************************************************************
// **************************************************************************

void CDrawFillDlg::FillHatchBox(CDC* dc, int fill_type, RECT rc)
{
	CBrush brush;
	CBrush* oldbrush;
	CPen lightpen;
	COLORREF back_color;
	int colorcode;
	CFvwUtil *futil = CFvwUtil::get_instance();
	COvlkitUtil util;

	util.DrawBasicButton(dc, rc);

	colorcode = UTIL_COLOR_BLUE;
	back_color = futil->code2color(colorcode);
	BOOL fill = TRUE;
	switch (fill_type)
	{
		case UTIL_FILL_NONE:
			brush.CreateStockObject(NULL_BRUSH);
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
		oldbrush = (CBrush*) dc->SelectObject(&brush);
		int oldbkmode = dc->SetBkMode(TRANSPARENT);
		dc->Rectangle(&rc);
		lightpen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* oldpen = (CPen*) dc->SelectObject(&lightpen);
		dc->MoveTo(rc.right-1, rc.top);
		dc->LineTo(rc.right-1, rc.bottom-1);
		dc->LineTo(rc.left, rc.bottom-1);
		dc->SelectObject(oldbrush);
		brush.DeleteObject();
		dc->SetBkMode(oldbkmode);
		dc->SelectObject(oldpen);
		lightpen.DeleteObject();
	}
}
// end of FillHatchBox
// **************************************************************************

void CDrawFillDlg::DrawFillButton(int fill_type, DRAWITEMSTRUCT* pDI)
{
	CPen blackpen;
	CBrush graybrush;

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
				FillHatchBox(&dc, fill_type, pDI->rcItem);

        	} 
        	break;
    	default:
        	break;
    }
}
// end of DrawFillButton

  
// **************************************************************************
// **************************************************************************


void CDrawFillDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CRect rc(lpDrawItemStruct->rcItem);		

	switch(nIDCtl)
	{
    	case IDC_FILL_NONE:
        	DrawFillButton(nIDCtl, lpDrawItemStruct);
        	break;
	    case IDC_FILL_HORZLINE:
			DrawFillButton(UTIL_FILL_HORZ, lpDrawItemStruct);
        	break;
	    case IDC_FILL_VERTLINE:
			DrawFillButton(UTIL_FILL_VERT, lpDrawItemStruct);
        	break;
	    case IDC_FILL_BACK_SLASH:
			DrawFillButton(UTIL_FILL_BDIAG, lpDrawItemStruct);
        	break;
	    case IDC_FILL_FORWD_SLASH:
			DrawFillButton(UTIL_FILL_FDIAG, lpDrawItemStruct);
        	break;
	    case IDC_FILL_SQUARE_HASH:
			DrawFillButton(UTIL_FILL_CROSS, lpDrawItemStruct);
        	break;
	    case IDC_FILL_DIAMOND_HASH:
			DrawFillButton(UTIL_FILL_DIAGCROSS, lpDrawItemStruct);
        	break;
	    case IDC_FILL_SOLID:
			DrawFillButton(UTIL_FILL_SOLID, lpDrawItemStruct);
        	break;
	}
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

LRESULT CDrawFillDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}
/////////////////////////////////////////////////////////////////////////////
// CDrawFill2Dlg dialog


CDrawFill2Dlg::CDrawFill2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawFill2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawFill2Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawFill2Dlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


// **************************************************************************
// **************************************************************************

BEGIN_MESSAGE_MAP(CDrawFill2Dlg, CDialog)
	//{{AFX_MSG_MAP(CDrawFillDlg)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_BN_CLICKED(IDC_FILL_BACK_SLASH, OnFillBackSlash)
	ON_BN_CLICKED(IDC_FILL_DIAMOND_HASH, OnFillDiamondHash)
	ON_BN_CLICKED(IDC_FILL_FORWD_SLASH, OnFillForwdSlash)
	ON_BN_CLICKED(IDC_FILL_HORZLINE, OnFillHorzline)
	ON_BN_CLICKED(IDC_FILL_NONE, OnFillNone)
	ON_BN_CLICKED(IDC_FILL_SOLID, OnFillSolid)
	ON_BN_CLICKED(IDC_FILL_SHADE, OnFillShade)
	ON_BN_CLICKED(IDC_FILL_SQUARE_HASH, OnFillSquareHash)
	ON_BN_CLICKED(IDC_FILL_VERTLINE, OnFillVertline)
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawFillDlg message handlers

void CDrawFill2Dlg::OnFillBackSlash() 
{
	m_fill_style = UTIL_FILL_BDIAG;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnFillDiamondHash() 
{
	m_fill_style = UTIL_FILL_DIAGCROSS;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnFillForwdSlash() 
{
	m_fill_style = UTIL_FILL_FDIAG;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnFillHorzline() 
{
	m_fill_style = UTIL_FILL_HORZ;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnFillNone() 
{
	m_fill_style = UTIL_FILL_NONE;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnFillSolid() 
{
	m_fill_style = UTIL_FILL_SOLID;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnFillSquareHash() 
{
	m_fill_style = UTIL_FILL_CROSS;	
	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnFillVertline() 
{
	m_fill_style = UTIL_FILL_VERT;	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnFillShade() 
{
	m_fill_style = UTIL_FILL_SHADE;	
	CDialog::OnOK();
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::set_fill_style(int fill_style) 
{
	m_fill_style = fill_style;
}

// **************************************************************************
// **************************************************************************


int CDrawFill2Dlg::get_fill_style() 
{
	return m_fill_style;
}

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}
// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::FillHatchBox(CDC* dc, int fill_type, RECT rc)
{
	CBrush brush, and_brush;
	CBrush* oldbrush;
	CPen lightpen;
	COLORREF back_color;
	int colorcode, oldmode;
	CFvwUtil *futil = CFvwUtil::get_instance();
	COvlkitUtil util;
	CBitmap shade_bits, and_bits;

	util.DrawBasicButton(dc, rc);

	colorcode = UTIL_COLOR_BLUE;
	back_color = futil->code2color(colorcode);
	BOOL fill = TRUE;
	switch (fill_type)
	{
		case UTIL_FILL_NONE:
			brush.CreateStockObject(NULL_BRUSH);
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
		case UTIL_FILL_SHADE:
			{
				WORD HatchBits[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA,
					0x55, 0xAA, 0x55 };
				shade_bits.CreateBitmap(8,8,1,1, HatchBits);
				brush.CreatePatternBrush(&shade_bits);
				break;
			}
		default:
			fill = FALSE;
	}
	if (fill)
	{
		oldbrush = (CBrush*) dc->SelectObject(&brush);
		int oldbkmode = dc->SetBkMode(TRANSPARENT);
		if (fill_type == UTIL_FILL_SHADE)
		{
			// AND the inverse bitmap with the screen   to blacken the soon to be colored bits
			and_bits.LoadBitmap(IDB_SHADE_COLOR_PREP);
			and_brush.CreatePatternBrush(&and_bits);
			CBrush* oldbrush = (CBrush*) dc->SelectObject(&and_brush);
			oldmode = dc->SetROP2(R2_MASKPEN);
			dc->Rectangle(&rc);

			// OR the colored bitmap with the screen
			dc->SetROP2(R2_MERGEPEN);
			dc->SelectObject(&brush);
			int old_fg = dc->SetTextColor(RGB(0,0,255));
			int old_bg = dc->SetBkColor(RGB(0,0,0));
			dc->Rectangle(&rc);
			dc->SetBkColor(old_bg);
			dc->SetTextColor(old_fg);
			dc->SetROP2(oldmode);
			and_brush.DeleteObject();
			and_bits.DeleteObject();
		}
		else
			dc->Rectangle(&rc);
		lightpen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* oldpen = (CPen*) dc->SelectObject(&lightpen);
		dc->MoveTo(rc.right-1, rc.top);
		dc->LineTo(rc.right-1, rc.bottom-1);
		dc->LineTo(rc.left, rc.bottom-1);
		dc->SelectObject(oldbrush);
		brush.DeleteObject();
		dc->SetBkMode(oldbkmode);
		dc->SelectObject(oldpen);
		lightpen.DeleteObject();
	}
}
// end of FillHatchBox

// **************************************************************************
// **************************************************************************

void CDrawFill2Dlg::DrawFillButton(int fill_type, DRAWITEMSTRUCT* pDI)
{
	CPen blackpen;
	CBrush graybrush;

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
				FillHatchBox(&dc, fill_type, pDI->rcItem);

        	} 
        	break;
    	default:
        	break;
    }
}
// end of DrawFillButton

  
// **************************************************************************
// **************************************************************************


void CDrawFill2Dlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CRect rc(lpDrawItemStruct->rcItem);		

	switch(nIDCtl)
	{
    	case IDC_FILL_NONE:
        	DrawFillButton(nIDCtl, lpDrawItemStruct);
        	break;
	    case IDC_FILL_HORZLINE:
			DrawFillButton(UTIL_FILL_HORZ, lpDrawItemStruct);
        	break;
	    case IDC_FILL_VERTLINE:
			DrawFillButton(UTIL_FILL_VERT, lpDrawItemStruct);
        	break;
	    case IDC_FILL_BACK_SLASH:
			DrawFillButton(UTIL_FILL_BDIAG, lpDrawItemStruct);
        	break;
	    case IDC_FILL_FORWD_SLASH:
			DrawFillButton(UTIL_FILL_FDIAG, lpDrawItemStruct);
        	break;
	    case IDC_FILL_SQUARE_HASH:
			DrawFillButton(UTIL_FILL_CROSS, lpDrawItemStruct);
        	break;
	    case IDC_FILL_DIAMOND_HASH:
			DrawFillButton(UTIL_FILL_DIAGCROSS, lpDrawItemStruct);
        	break;
	    case IDC_FILL_SOLID:
			DrawFillButton(UTIL_FILL_SOLID, lpDrawItemStruct);
        	break;
	    case IDC_FILL_SHADE:
			DrawFillButton(UTIL_FILL_SHADE, lpDrawItemStruct);
        	break;
	}
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

LRESULT CDrawFill2Dlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CDrawWidthDlg dialog


CDrawWidthDlg::CDrawWidthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawWidthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawWidthDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDrawWidthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawWidthDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawWidthDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawWidthDlg)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_BN_CLICKED(IDC_BUTTON_01, OnButton01)
	ON_BN_CLICKED(IDC_BUTTON_02, OnButton02)
	ON_BN_CLICKED(IDC_BUTTON_03, OnButton03)
	ON_BN_CLICKED(IDC_BUTTON_04, OnButton04)
	ON_BN_CLICKED(IDC_BUTTON_06, OnButton06)
	ON_BN_CLICKED(IDC_BUTTON_07, OnButton07)
	ON_BN_CLICKED(IDC_BUTTON_08, OnButton08)
	ON_BN_CLICKED(IDC_BUTTON_09, OnButton09)
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawWidthDlg message handlers

void CDrawWidthDlg::OnButton01() 
{
	m_width = 1;	
	
	CDialog::OnOK();
}

void CDrawWidthDlg::OnButton02() 
{
	m_width = 2;	
	
	CDialog::OnOK();
}

void CDrawWidthDlg::OnButton03() 
{
	m_width = 3;	
	
	CDialog::OnOK();
}

void CDrawWidthDlg::OnButton04() 
{
	m_width = 4;	
	
	CDialog::OnOK();
}

void CDrawWidthDlg::OnButton06() 
{
	m_width = 6;	
	
	CDialog::OnOK();
}

void CDrawWidthDlg::OnButton07() 
{
	m_width = 7;	
	
	CDialog::OnOK();
}

void CDrawWidthDlg::OnButton08() 
{
	m_width = 8;	
	
	CDialog::OnOK();
}

void CDrawWidthDlg::OnButton09() 
{
	m_width = 9;	
	
	CDialog::OnOK();
}

void CDrawWidthDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

BOOL CDrawWidthDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDrawWidthDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch(nIDCtl)
	{
    	case IDC_BUTTON_01:
        	DrawWidthButton(1, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_02:
			DrawWidthButton(2, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_03:
			DrawWidthButton(3, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_04:
			DrawWidthButton(4, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_06:
			DrawWidthButton(6, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_07:
			DrawWidthButton(7, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_08:
			DrawWidthButton(8, lpDrawItemStruct);
        	break;
	    case IDC_BUTTON_09:
			DrawWidthButton(9, lpDrawItemStruct);
        	break;
	}
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// **************************************************************************

void CDrawWidthDlg::DrawWidthButton(int width, DRAWITEMSTRUCT* pDI)
{
	CPen blackpen;
	COvlkitUtil util;
	int cy;

    switch (pDI->itemAction) 
    {
    	case ODA_DRAWENTIRE:
    	case ODA_SELECT: 
    		{
        		// Create a CDC object.
        		CDC dc;
        		dc.Attach(pDI->hDC);
				int k, width2;

				util.DrawBasicButton(&dc, pDI->rcItem);

        		// Draw the black rectangle around the outside
        		// and fill the center with either the off or on color.
				blackpen.CreatePen(PS_SOLID, 1, RGB (0,0,0));
				CPen *oldpen = dc.SelectObject(&blackpen);
				cy = (pDI->rcItem.bottom - pDI->rcItem.top) / 2;
				width2 = width / 2;
				for (k=0; k<width; k++)
				{
					dc.MoveTo(pDI->rcItem.left + 5, cy - width2 + k);
					dc.LineTo(pDI->rcItem.right - 5, cy - width2 + k);
				}
				dc.SelectObject(oldpen);
				blackpen.DeleteObject();
        	} 
        	break;
    	default:
        	break;
    }
}
// end of DrawWidthButton
  
// **************************************************************************

void CDrawWidthDlg::set_width(int width)
{
	m_width = width;
}

// **************************************************************************
// **************************************************************************

int CDrawWidthDlg::get_width()
{
	return m_width;
}

LRESULT CDrawWidthDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}
// **************************************************************************


// **************************************************************************
// **************************************************************************
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CDrawStyleDlg dialog


CDrawStyleDlg::CDrawStyleDlg(int idd /*CDrawStyleDlg::IDD*/,CWnd* pParent /*=NULL*/)
	: CDialog(idd, pParent)
{
	//{{AFX_DATA_INIT(CDrawStyleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_reverse = FALSE;
}

void CDrawStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawStyleDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDrawStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawStyleDlg)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_BN_CLICKED(IDC_STYLE_ARROW, OnStyleArrow)
	ON_BN_CLICKED(IDC_STYLE_ARROW2, OnStyleArrow2)
	ON_BN_CLICKED(IDC_STYLE_DASH, OnStyleDash)
	ON_BN_CLICKED(IDC_STYLE_DASHDOT, OnStyleDashdot)
	ON_BN_CLICKED(IDC_STYLE_DASHDOTDOT, OnStyleDashdotdot)
	ON_BN_CLICKED(IDC_STYLE_DOT, OnStyleDot)
	ON_BN_CLICKED(IDC_STYLE_SOLID, OnStyleSolid)
	ON_BN_CLICKED(IDC_STYLE_ZIGZAG, OnStyleZigzag)
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_STYLE_FEBA, OnStyleFeba)
	ON_BN_CLICKED(IDC_STYLE_FEBA2, OnStyleFeba2)
	ON_BN_CLICKED(IDC_STYLE_FLOT, OnStyleFlot)
	ON_BN_CLICKED(IDC_STYLE_FLOT2, OnStyleFlot2)
	ON_BN_CLICKED(IDC_STYLE_FLOT3, OnStyleFlot3)
	ON_BN_CLICKED(IDC_STYLE_NOTCH, OnStyleNotch)
	ON_BN_CLICKED(IDC_STYLE_WIRE, OnStyleWire)
	ON_BN_CLICKED(IDC_STYLE_BORDER, OnStyleBorder)
	ON_BN_CLICKED(IDC_STYLE_RAILROAD2, OnStyleRailroad)
	ON_BN_CLICKED(IDC_REVERSE, OnReverse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawStyleDlg message handlers

void CDrawStyleDlg::OnStyleArrow() 
{
	if (m_reverse)
		m_style = UTIL_LINE_ARROW_END;
	else
		m_style = UTIL_LINE_ARROW_START;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleArrow2() 
{
	if (m_reverse)
		m_style = UTIL_LINE_ARROW2_END;
	else
		m_style = UTIL_LINE_ARROW2_START;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleDash() 
{
	m_style = UTIL_LINE_DASH;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleDashdot() 
{
	m_style = UTIL_LINE_DASHDOT;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleDashdotdot() 
{
	m_style = UTIL_LINE_DASHDOTDOT;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleDot() 
{
	m_style = UTIL_LINE_DOT;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleSolid() 
{
	m_style = UTIL_LINE_SOLID;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleZigzag() 
{
	m_style = UTIL_LINE_ZIGZAG;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnReverse() 
{
	m_reverse = IsDlgButtonChecked(IDC_REVERSE);

	Invalidate();
/*
	if (m_reverse)
		DrawLineButton(IDC_STYLE_ARROW, 2, UTIL_LINE_ARROW_END, UTIL_COLOR_BLACK);
	else
		DrawLineButton(IDC_STYLE_ARROW, 2, UTIL_LINE_ARROW_START, UTIL_COLOR_BLACK);
	if (m_reverse)
		DrawLineButton(IDC_STYLE_FEBA, 2, UTIL_LINE_FEBA_END, UTIL_COLOR_BLACK);
	else
		DrawLineButton(IDC_STYLE_FEBA, 2, UTIL_LINE_FEBA_START, UTIL_COLOR_BLACK);
	if (m_reverse)
		DrawLineButton(IDC_STYLE_FLOT, 2, UTIL_LINE_FLOT_END, UTIL_COLOR_BLACK);
	else
		DrawLineButton(IDC_STYLE_FLOT, 2, UTIL_LINE_FLOT_START, UTIL_COLOR_BLACK);
*/
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	int color;

	color	= UTIL_COLOR_BLACK;

   switch (nIDCtl) 
   {
		case IDC_STYLE_DASH:
			DrawLineButton(lpDrawItemStruct, 2, PS_DASH, color);
			break;
		case IDC_STYLE_DASHDOT:
			DrawLineButton(lpDrawItemStruct, 2, PS_DASHDOT, color);
			break;
		case IDC_STYLE_DASHDOTDOT:
			DrawLineButton(lpDrawItemStruct, 2, PS_DASHDOTDOT, color);
			break;
		case IDC_STYLE_DOT:
			DrawLineButton(lpDrawItemStruct, 2, PS_DOT, color);
			break;
		case IDC_STYLE_SOLID:
			DrawLineButton(lpDrawItemStruct, 2, PS_SOLID, color);
			break;
		case IDC_STYLE_ZIGZAG:
			DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_ZIGZAG, color);
			break;
		case IDC_STYLE_NOTCH:
			DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_NOTCHED, color);
			break;
		case IDC_STYLE_RAILROAD2:
			DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_RAILROAD, color);
			break;
		case IDC_STYLE_WIRE:
			DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_WIRE, color);
			break;
		case IDC_STYLE_BORDER:
			if (m_reverse)
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_BORDER_END, color);
			else
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_BORDER_START, color);
			break;
		case IDC_STYLE_ARROW:
			if (m_reverse)
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_ARROW_END, color);
			else
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_ARROW_START, color);
			break;
		case IDC_STYLE_ARROW2:
			if (m_reverse)
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_ARROW2_END, color);
			else
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_ARROW2_START, color);
			break;
		case IDC_STYLE_FEBA:
			if (m_reverse)
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FEBA_END, color);
			else
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FEBA_START, color);
			break;
		case IDC_STYLE_FEBA2:
			if (m_reverse)
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FEBA2_END, color);
			else
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FEBA2_START, color);
			break;
		case IDC_STYLE_FLOT:
			if (m_reverse)
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FLOT_END, color);
			else
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FLOT_START, color);
			break;
		case IDC_STYLE_FLOT2:
			if (m_reverse)
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FLOT2_END, color);
			else
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FLOT2_START, color);
			break;
		case IDC_STYLE_FLOT3:
			if (m_reverse)
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FLOT3_END, color);
			else
				DrawLineButton(lpDrawItemStruct, 2, UTIL_LINE_FLOT3_START, color);
			break;
	}
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CDrawStyleDlg::DrawLineBox(CDC *dc, RECT rc, 
							int width, int style, int colorcode)
{
	int cy;
	COLORREF color;
	int backcolor;
	COvlkitUtil util;
	CFvwUtil *futil = CFvwUtil::get_instance();
	boolean_t special_case = FALSE;

	util.DrawBasicButton(dc, rc);

	cy = (rc.bottom - rc.top) / 2;
	color = futil->code2color(colorcode);
	backcolor = UTIL_COLOR_BLACK;
	switch (style)
	{
		case UTIL_LINE_FEBA_START:	  cy	+= 5; break;
		case UTIL_LINE_FEBA_END:	  cy	-= 5; break;
//		case UTIL_LINE_FEBA2_START:  cy	+= 5; break;
//		case UTIL_LINE_FEBA2_END:	  cy	-= 5; break;
		case UTIL_LINE_FLOT_START:	  cy	+= 5; break;
		case UTIL_LINE_FLOT_END:	  cy	-= 5; break;
		case UTIL_LINE_FLOT2_START:  cy	+= 5; break;
		case UTIL_LINE_FLOT2_END:	  cy	-= 5; break;
		case UTIL_LINE_FLOT3_START:  cy	+= 5; break;
		case UTIL_LINE_FLOT3_END:	  cy	-= 5; break;
		case UTIL_LINE_BORDER_START: cy	+= 5; break;
		case UTIL_LINE_BORDER_END:	  cy	-= 5; break;
      case UTIL_LINE_NOTCHED:      cy += 5; break;
      
		// special case for 
		case UTIL_LINE_ROUND_DOT:
		case UTIL_LINE_DIAMOND:
		case UTIL_LINE_SQUARE_DOT:
		case UTIL_LINE_DASH_DOT:
		case UTIL_LINE_LONG_DASH:
		case UTIL_LINE_ARROW:
			special_case = TRUE;
			break;
		case UTIL_LINE_FEBA2_START: cy += 5; special_case = TRUE; break;
		case UTIL_LINE_FEBA2_END: cy -= 5; special_case = TRUE; break;
	}

   UtilDraw utilDraw(dc);
	if (special_case) 
	{
		if ((style == UTIL_LINE_FEBA2_START) || (style == UTIL_LINE_FEBA2_END))
			utilDraw.set_pen(UTIL_COLOR_BLACK, style, 2);
		else
			if (style == UTIL_LINE_DIAMOND || style == UTIL_LINE_ROUND_DOT)
				utilDraw.set_pen(UTIL_COLOR_BLACK, style, 7);
			else
				utilDraw.set_pen(UTIL_COLOR_BLACK, style, 3);

      utilDraw.draw_line(NULL, rc.left+3, cy, rc.right-3, cy);
	}
	else 
   {
      utilDraw.set_pen(colorcode, style, 2);
		utilDraw.draw_line(NULL, rc.left+8, cy, rc.right - 8, cy);
   }
}
// end of DrawLineBox

// *************************************************************
// *************************************************************

void CDrawStyleDlg::DrawLineButton(DRAWITEMSTRUCT* pDI, int width,
										int style, int colorcode)
{
    switch (pDI->itemAction) 
    {
    	case ODA_DRAWENTIRE:
    	case ODA_SELECT: 
    		{
        		// Create a CDC object.
        		CDC dc;
        		dc.Attach(pDI->hDC);
				DrawLineBox(&dc, pDI->rcItem, width, style, colorcode);
        	} 
        	break;
    	default:
        	break;
    }
}
// end of DrawLineButton
  
// *************************************************************
// *************************************************************

BOOL CDrawStyleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::set_line_style(int style)
{
	m_style = style;
}

// *************************************************************
// *************************************************************

int CDrawStyleDlg::get_line_style()
{
	return m_style;
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleFeba() 
{
	if (m_reverse)
		m_style = UTIL_LINE_FEBA_END;	
	else
		m_style = UTIL_LINE_FEBA_START;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleFeba2() 
{
	if (m_reverse)
		m_style = UTIL_LINE_FEBA2_END;	
	else
		m_style = UTIL_LINE_FEBA2_START;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleFlot() 
{
	if (m_reverse)
		m_style = UTIL_LINE_FLOT_END;	
	else
		m_style = UTIL_LINE_FLOT_START;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleFlot2()
{
	if (m_reverse)
		m_style = UTIL_LINE_FLOT2_END;	
	else
		m_style = UTIL_LINE_FLOT2_START;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleFlot3()
{
	if (m_reverse)
		m_style = UTIL_LINE_FLOT3_END;	
	else
		m_style = UTIL_LINE_FLOT3_START;	
	
	CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleNotch()
{
   m_style = UTIL_LINE_NOTCHED;

   CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleWire()
{
   m_style = UTIL_LINE_WIRE;

   CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleBorder()
{
	if (m_reverse)
		m_style = UTIL_LINE_BORDER_END;	
	else
		m_style = UTIL_LINE_BORDER_START;	

   CDialog::OnOK();
}

// *************************************************************
// *************************************************************

void CDrawStyleDlg::OnStyleRailroad()
{
	m_style = UTIL_LINE_RAILROAD;	

   CDialog::OnOK();
}

LRESULT CDrawStyleDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


// COvalEditDlg dialog
//
//
//COvalEditDlg::COvalEditDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(COvalEditDlg::IDD, pParent)
//{
//	//{{AFX_DATA_INIT(COvalEditDlg)
//		// NOTE: the ClassWizard will add member initialization here
//	//}}AFX_DATA_INIT
//
//	m_overlay = NULL;
//	m_name[0] = '\0';
//	m_loc[0] = '\0';
//	m_radius[0] = '\0';
//}
//
//
//void COvalEditDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	//{{AFX_DATA_MAP(COvalEditDlg)
//		// NOTE: the ClassWizard will add DDX and DDV calls here
//	//}}AFX_DATA_MAP
//}
//
//
//BEGIN_MESSAGE_MAP(COvalEditDlg, CDialog)
//	//{{AFX_MSG_MAP(COvalEditDlg)
//	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
//	ON_WM_KEYDOWN()
//	ON_LBN_SETFOCUS(IDC_LIST, OnSetfocusList)
//	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
//	ON_BN_CLICKED(IDC_B_APPLY, OnBApply)
//	ON_BN_CLICKED(IDC_B_NEW, OnBNew)
//	ON_BN_CLICKED(IDC_B_REMOVE, OnBRemove)
//	ON_BN_CLICKED(ID_MY_OK, OnMyOk)
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
///////////////////////////////////////////////////////////////////////////////
//// COvalEditDlg message handlers
//
//
//// **************************************************************************
//
//void COvalEditDlg::add_string(int num, char* name, char* loc, char* radius) 
//{
//	CFvwUtil *futil = CFvwUtil::get_instance();
//   const int EDIT_LEN = 800;
//	char edit[EDIT_LEN];
//
//	sprintf_s(edit, EDIT_LEN, "%6d", num);
//	futil->add_tab(edit);
//	strcat_s(edit, EDIT_LEN, name);
//	futil->add_tab(edit);
//	strcat_s(edit, EDIT_LEN, "  ");
//	strcat_s(edit, EDIT_LEN, loc);
//	futil->add_tab(edit);
//	strcat_s(edit, EDIT_LEN, "  ");
//	strcat_s(edit, EDIT_LEN, radius);
//	SendDlgItemMessage(IDC_LIST, LB_ADDSTRING, 
//				0, (LPARAM) (LPCTSTR) edit);
//
//}
//// end of add_string
//	
//// **************************************************************************
//
//BOOL COvalEditDlg::OnInitDialog() 
//{
//	CDialog::OnInitDialog();
//	int cnt, num;
//   const int EDIT_LEN = 800;
//	char edit[EDIT_LEN];
//	COvlkitUtil util;
//	CFvwUtil *futil = CFvwUtil::get_instance();
//   const int GEOSTR_SIZE = 80;
//   const int NAME_LEN = 81;
//   const int TSTR_LEN = 81;
//	char name[NAME_LEN], geostr[GEOSTR_SIZE], tstr[TSTR_LEN];
//
//	C_ovlkit_ovl::get_circles(&m_oval_list);
//	if (m_oval_list->IsEmpty())
//		cnt = 0;
//	else
//		cnt = m_oval_list->GetCount();
//	num = 0;
//	if (cnt > 0)
//	{
//		C_ovlkit_ovl_list* list;
//		C_ovlkit_oval* oval;
//		POSITION next, tail_position, oldpos;
//		BOOL first;
//
//		tail_position = m_oval_list->GetTailPosition();
//		next = m_oval_list->GetHeadPosition();
//		first = TRUE;
//		edit[0] = '\0';
//		while (((next != tail_position) && (next != NULL)) || first)
//		{
//			if (next == tail_position)
//				first = FALSE;
//			oldpos = next;
//			list = m_oval_list->GetNext(next);
//			num = list->m_index;
//			oval = (C_ovlkit_oval*) list->m_icon;
//			sprintf_s(edit, EDIT_LEN, "%6d", num);
//			strncpy_s(name, NAME_LEN, oval->m_tool_tip_text.GetBuffer(20), 20);
//			futil->set_text_length(name, 20);
//			name[20] = '\0';
//			GEO_lat_lon_to_string(oval->m_lat, oval->m_lon, geostr, GEOSTR_SIZE);
//			sprintf_s(tstr, TSTR_LEN, "%.0f", oval->m_vert / 1.853245);
//			add_string(num, name, geostr, tstr);
//		}
//	}
//
//	m_newmode = TRUE;
//	CWnd *wnd = GetDlgItem(IDC_Name);
//	wnd->SetFocus();
//	
//	return TRUE;  // return TRUE unless you set the focus to a control
//	              // EXCEPTION: OCX Property Pages should return FALSE
//}
//
//// **************************************************************************
//
//void COvalEditDlg::OnOK() 
//{
//	OnBApply();
//}
//// end of OnOK
//
//// **************************************************************************
//
//void COvalEditDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
//{
//	if (nChar == VK_RETURN)
//		OnBApply();
//	else
//		CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
//}
//// end of OnBRemove
//
//// **************************************************************************
//
//void COvalEditDlg::OnSetfocusList() 
//{
//	// TODO: Add your control notification handler code here
//	
//}
//
//BOOL COvalEditDlg::parse_buf(char* buf, int* num, char* name, int name_len, char* loc, int loc_len, char* radius, int radius_len)
//{
//	int len;
//	char* pos;
//   const int TSTR_LEN = 300;
//   const int NUMSTR_LEN = 21;
//	char tstr[TSTR_LEN], numstr[NUMSTR_LEN];
//
//	strcpy_s(tstr, TSTR_LEN, buf);
//	pos = strchr( tstr, 9);
//	if (pos == NULL)
//		return FALSE;
//
//	strncpy_s(numstr, NUMSTR_LEN, tstr, pos-tstr);
//	numstr[pos-tstr] = '\0';
//	*num = atoi(numstr);
//	len = strlen(tstr);
//	strcpy_s(tstr, TSTR_LEN, pos+1);
//	
//	pos = strchr( tstr, 9);
//	if (pos == NULL)
//		return FALSE;
//
//	strncpy_s(name, name_len, tstr, pos-tstr);
//	name[pos-tstr] = '\0';
//	len = strlen(tstr);
//	strcpy_s(tstr, TSTR_LEN, pos+1);
//	
//	pos = strchr( tstr, 9);
//	if (pos == NULL)
//		return FALSE;
//
//	strncpy_s(loc, loc_len, tstr, pos-tstr);
//	loc[pos-tstr] = '\0';
//	strcpy_s(radius, radius_len, pos+1);
//	return TRUE;
//}
//// end of parse_buf
//
//// **************************************************************************
//
//void COvalEditDlg::OnSelchangeList() 
//{
//	int pos;
//	char buf[300];
//
//	pos = SendDlgItemMessage(IDC_LIST, LB_GETCURSEL, 0, 0);
//	if (pos != LB_ERR)
//	{
//		SendDlgItemMessage(IDC_LIST, LB_GETTEXT, pos, (LPARAM) (LPCTSTR) buf);
//		// select this circle on the map screen
//
//		//	fill the edit boxes with these values
//		if (parse_buf(buf, &m_cur_index, m_name, 200, m_loc, M_LOC_SIZE, m_radius, 20))
//		{
//			SetDlgItemText(IDC_Name, m_name);
//			SetDlgItemText(IDC_LOCATION, m_loc);
//			SetDlgItemText(IDC_RADIUS, m_radius);
//		}
//		m_newmode = FALSE;
//	}
//}
//// end of OnSelchangeList
//
//// **************************************************************************
//
//void COvalEditDlg::OnBApply() 
//{
//	GetDlgItemText(IDC_Name, (LPTSTR) m_name, sizeof(m_name));
//	GetDlgItemText(IDC_LOCATION, (LPTSTR) m_loc, sizeof(m_loc));
//	GetDlgItemText(IDC_RADIUS, (LPTSTR) m_radius, sizeof(m_radius));
//
//	double lat, lon, rad;
//	int rslt;
//
//	if (m_loc[0] == '\0')
//		return;
//
//   // standardize the geo
//	rslt = GEO_string_to_lat_lon(m_loc, "WGS84", &lat, &lon);
//	if (rslt < 0)
//	{
//		AfxMessageBox("Error in Location");
//		return;
//	}
//	rad = atof(m_radius);
//	if (rad <= 0)
//	{
//		AfxMessageBox("Zero or Negative Radius is not allowed");
//		return;
//	}
//
//	GEO_lat_lon_to_string(lat, lon, m_loc, M_LOC_SIZE);
//
//	if (m_newmode)
//	{
//		// add a new circle
//		if (m_overlay != NULL)
//			m_overlay->add_circle(m_name, m_loc, m_radius);
//		add_string(C_ovlkit_ovl_list::m_last_index, m_name, m_loc, m_radius);
//	}
//	else
//	{
//		// update the current circle
//		if (m_overlay != NULL)
//			m_overlay->update_circle(m_cur_index, m_name, m_loc, m_radius);
//	}
//
//	m_newmode = TRUE;
//	CWnd *wnd = GetDlgItem(IDC_Name);
//	wnd->SetFocus();
//}
//// end of OnBApply
//
//// **************************************************************************
//
//void COvalEditDlg::OnBNew() 
//{
//	// clear the edit params
//	m_name[0] = '\0';
//	m_loc[0] = '\0';
//	m_radius[0] = '\0';
//	SetDlgItemText(IDC_Name, m_name);
//	SetDlgItemText(IDC_LOCATION, m_loc);
//	SetDlgItemText(IDC_RADIUS, m_radius);
//
//	// select nothing
//	SendDlgItemMessage(IDC_LIST, LB_SETCURSEL, -1, 0);
//
//	m_newmode = TRUE;	
//	CWnd *wnd = GetDlgItem(IDC_Name);
//	wnd->SetFocus();
//}
//// end of OnBNew
//
//// **************************************************************************
//
//void COvalEditDlg::OnBRemove() 
//{
//	int pos;
//
//	pos = SendDlgItemMessage(IDC_LIST, LB_GETCURSEL, 0, 0);
//	if (pos != LB_ERR)
//	{
//		SendDlgItemMessage(IDC_LIST, LB_DELETESTRING, 0);
//
//		// delete the circle from the list
//		m_overlay->delete_circle(m_cur_index);
//
//		m_newmode = TRUE;
//	}
//}
//// end of OnBRemove
//
//// **************************************************************************
//// **************************************************************************
//
//void COvalEditDlg::set_overlay_ptr(C_ovlkit_ovl* ovl) 
//{
//	m_overlay = ovl;
//}
//
//// **************************************************************************
//
//// **************************************************************************
//
//
//void COvalEditDlg::OnMyOk() 
//{
//	OnBApply();	
//	CDialog::OnOK();
//}
//
//// **************************************************************************
//// **************************************************************************
//// **************************************************************************
//// **************************************************************************
//
//LRESULT COvalEditDlg::OnCommandHelp(WPARAM, LPARAM)
//{
//   CMainFrame *pFrame = fvw_get_frame();
//   // THIS PATH NEEDS TO BE CHANGED
//   if (pFrame != NULL)
//      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());
//
//	return 1;
//}