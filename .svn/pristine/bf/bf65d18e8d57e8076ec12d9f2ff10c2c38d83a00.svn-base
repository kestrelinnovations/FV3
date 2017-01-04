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

// colorang.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "colordlg.h"
#include "shp_pp.h"
#include "err.h"
#include "colorang.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CShapeColorRangeDlg dialog


CShapeColorRangeDlg::CShapeColorRangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeColorRangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShapeColorRangeDlg)
	m_edit_blue = _T("");
	m_edit_green = _T("");
	m_edit_red = _T("");
	m_edit_lum = _T("");
	m_edit_hue = _T("");
	m_edit_sat = _T("");
	m_name0 = _T("");
	m_name1 = _T("");
	m_name2 = _T("");
	m_name3 = _T("");
	m_name4 = _T("");
	m_name5 = _T("");
	m_name6 = _T("");
	m_name7 = _T("");
	m_name8 = _T("");
	m_name9 = _T("");
	//}}AFX_DATA_INIT

	m_color_rgb = RGB(0, 255, 0);
	m_single_color_rgb  = RGB(0, 255, 0);
	m_single_hue = TRUE;
	m_num_colors = 5;
	int k;
	for (k=0; k<10; k++)
		m_color_range[k] = RGB(255, 0, 0);
	for (k=0; k<10; k++)
		m_color_selected[k] = FALSE;
	m_color_id[0] = IDC_COLOR0;
	m_color_id[1] = IDC_COLOR1;
	m_color_id[2] = IDC_COLOR2;
	m_color_id[3] = IDC_COLOR3;
	m_color_id[4] = IDC_COLOR4;
	m_color_id[5] = IDC_COLOR5;
	m_color_id[6] = IDC_COLOR6;
	m_color_id[7] = IDC_COLOR7;
	m_color_id[8] = IDC_COLOR8;
	m_color_id[9] = IDC_COLOR9;
	m_selected_color = -1;
}

// ************************************************************************
// ************************************************************************

CShapeColorRangeDlg::~CShapeColorRangeDlg()
{
	m_names.RemoveAll();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShapeColorRangeDlg)
	DDX_Control(pDX, IDC_SPIN_SAT, m_spin_sat);
	DDX_Control(pDX, IDC_SPIN_LUM, m_spin_lum);
	DDX_Control(pDX, IDC_SPIN_HUE, m_spin_hue);
	DDX_Control(pDX, IDC_SPIN_RED, m_spin_red);
	DDX_Control(pDX, IDC_SPIN_GREEN, m_spin_green);
	DDX_Control(pDX, IDC_SPIN_BLUE, m_spin_blue);
	DDX_Text(pDX, IDC_EDIT_BLUE, m_edit_blue);
	DDX_Text(pDX, IDC_EDIT_GREEN, m_edit_green);
	DDX_Text(pDX, IDC_EDIT_RED, m_edit_red);
	DDX_Text(pDX, IDC_EDIT_LUM, m_edit_lum);
	DDX_Text(pDX, IDC_EDIT_HUE, m_edit_hue);
	DDX_Text(pDX, IDC_EDIT_SAT, m_edit_sat);
	DDX_Text(pDX, IDC_NAME0, m_name0);
	DDX_Text(pDX, IDC_NAME1, m_name1);
	DDX_Text(pDX, IDC_NAME2, m_name2);
	DDX_Text(pDX, IDC_NAME3, m_name3);
	DDX_Text(pDX, IDC_NAME4, m_name4);
	DDX_Text(pDX, IDC_NAME5, m_name5);
	DDX_Text(pDX, IDC_NAME6, m_name6);
	DDX_Text(pDX, IDC_NAME7, m_name7);
	DDX_Text(pDX, IDC_NAME8, m_name8);
	DDX_Text(pDX, IDC_NAME9, m_name9);
	//}}AFX_DATA_MAP
}


// ************************************************************************
// ************************************************************************

BEGIN_MESSAGE_MAP(CShapeColorRangeDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CShapeColorRangeDlg)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_EN_KILLFOCUS(IDC_EDIT_BLUE, OnKillfocusEditBlue)
	ON_EN_KILLFOCUS(IDC_EDIT_GREEN, OnKillfocusEditGreen)
	ON_EN_KILLFOCUS(IDC_EDIT_RED, OnKillfocusEditRed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BLUE, OnDeltaposSpinBlue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GREEN, OnDeltaposSpinGreen)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RED, OnDeltaposSpinRed)
	ON_EN_KILLFOCUS(IDC_EDIT_HUE, OnKillfocusEditHue)
	ON_EN_KILLFOCUS(IDC_EDIT_LUM, OnKillfocusEditLum)
	ON_EN_KILLFOCUS(IDC_EDIT_SAT, OnKillfocusEditSat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HUE, OnDeltaposSpinHue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LUM, OnDeltaposSpinLum)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAT, OnDeltaposSpinSat)
	ON_BN_CLICKED(IDC_COLOR0, OnColor0)
	ON_BN_CLICKED(IDC_COLOR1, OnColor1)
	ON_BN_CLICKED(IDC_COLOR2, OnColor2)
	ON_BN_CLICKED(IDC_COLOR3, OnColor3)
	ON_BN_CLICKED(IDC_COLOR4, OnColor4)
	ON_BN_CLICKED(IDC_COLOR5, OnColor5)
	ON_BN_CLICKED(IDC_COLOR6, OnColor6)
	ON_BN_CLICKED(IDC_COLOR7, OnColor7)
	ON_BN_CLICKED(IDC_MULTIPLE_COLORS, OnMultipleColors)
	ON_BN_CLICKED(IDC_COLOR8, OnColor8)
	ON_BN_CLICKED(IDC_COLOR9, OnColor9)
	ON_WM_DRAWITEM()
	ON_BN_DOUBLECLICKED(IDC_COLOR0, OnDoubleclickedColor0)
	ON_BN_DOUBLECLICKED(IDC_COLOR1, OnDoubleclickedColor1)
	ON_BN_DOUBLECLICKED(IDC_COLOR2, OnDoubleclickedColor2)
	ON_BN_DOUBLECLICKED(IDC_COLOR3, OnDoubleclickedColor3)
	ON_BN_DOUBLECLICKED(IDC_COLOR4, OnDoubleclickedColor4)
	ON_BN_DOUBLECLICKED(IDC_COLOR5, OnDoubleclickedColor5)
	ON_BN_DOUBLECLICKED(IDC_COLOR6, OnDoubleclickedColor6)
	ON_BN_DOUBLECLICKED(IDC_COLOR7, OnDoubleclickedColor7)
	ON_BN_DOUBLECLICKED(IDC_COLOR8, OnDoubleclickedColor8)
	ON_BN_DOUBLECLICKED(IDC_COLOR9, OnDoubleclickedColor9)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapeColorRangeDlg message handlers

// ************************************************************************
// ************************************************************************

BOOL CShapeColorRangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_spin_blue.SetRange(0, 255);
	m_spin_green.SetRange(0, 255);
	m_spin_red.SetRange(0, 255);

	int k;

	for (k=0; k<8; k++)
		m_color_selected[k] = FALSE;

	// init the rgb edit windows
	m_red = GetRValue(m_color_rgb);
	m_grn = GetGValue(m_color_rgb);
	m_blu = GetBValue(m_color_rgb);
	rgb_to_hsl_int(m_red, m_grn, m_blu, &m_hue, &m_sat, &m_lum);
	show_color_windows();
	show_name_windows();
	fill_names_list();
	update_color_edit();
//	update_color_bar();
	draw_the_buttons();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor() 
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	int old_color, rslt;
	CString msg;

	old_color = m_single_color_rgb;

	CShapeFileOvlOptionsPage::load_custom_colors();
	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(CHOOSECOLOR));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = this->m_hWnd;
	cc.lpCustColors = (LPDWORD) CShapeFileOvlOptionsPage::m_CustClr;
	cc.rgbResult = m_color_rgb;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
 
	if (ChooseColor(&cc)!=TRUE) 
		return;

	m_single_color_rgb = cc.rgbResult;

	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR, m_single_color_rgb);

	msg = "Do you want to change all the filter\n";
	msg += "group's colors to shades of this color?";
	rslt = AfxMessageBox(msg, MB_YESNO);
	if (rslt == IDYES)
		update_color_bar();
}
// end of OnColor

// ************************************************************************
// ************************************************************************

int CShapeColorRangeDlg::color_id_to_num(int id)
{
	int k, num;
	BOOL notdone;

	k = 0;
	num = -1;
	notdone = TRUE;
	while (notdone)
	{
		if (id == m_color_id[k])
		{
			notdone = FALSE;
			num = k;
		}
		k++;
		if (k > 9)
			notdone = FALSE;
	}
	return num;
}
// end of color_id_to_num

// ************************************************************************
// ************************************************************************

BOOL CShapeColorRangeDlg::set_num_colors(int num) 
{
	if (num < 1)
		return FALSE;
	if (num > 10)
		return FALSE;

	m_num_colors = num;
	return TRUE;
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::show_color_windows() 
{
	int k;

	for (k=0; k<m_num_colors; k++)
		show_window(m_color_id[k], TRUE);

	for (k=m_num_colors; k<10; k++)
		show_window(m_color_id[k], FALSE);
}
// end of show_color_windows

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::show_name_windows() 
{
	int k;

	for (k=0; k<m_num_colors; k++)
	{
		switch(k)
		{
			case 0: show_window(IDC_NAME0, TRUE); break;
			case 1: show_window(IDC_NAME1, TRUE); break;
			case 2: show_window(IDC_NAME2, TRUE); break;
			case 3: show_window(IDC_NAME3, TRUE); break;
			case 4: show_window(IDC_NAME4, TRUE); break;
			case 5: show_window(IDC_NAME5, TRUE); break;
			case 6: show_window(IDC_NAME6, TRUE); break;
			case 7: show_window(IDC_NAME7, TRUE); break;
			case 8: show_window(IDC_NAME8, TRUE); break;
			case 9: show_window(IDC_NAME9, TRUE); break;
		}
	}

	for (k=m_num_colors; k<10; k++)
	{
		switch(k)
		{
			case 0: show_window(IDC_NAME0, FALSE); break;
			case 1: show_window(IDC_NAME1, FALSE); break;
			case 2: show_window(IDC_NAME2, FALSE); break;
			case 3: show_window(IDC_NAME3, FALSE); break;
			case 4: show_window(IDC_NAME4, FALSE); break;
			case 5: show_window(IDC_NAME5, FALSE); break;
			case 6: show_window(IDC_NAME6, FALSE); break;
			case 7: show_window(IDC_NAME7, FALSE); break;
			case 8: show_window(IDC_NAME8, FALSE); break;
			case 9: show_window(IDC_NAME9, FALSE); break;
		}
	}
}
// end of show_color_windows

// ************************************************************************
// ************************************************************************

// get the color range selected; range must be large enough to hold data

void CShapeColorRangeDlg::fill_names_list()
{
	int cnt;

	cnt = m_names.GetSize();
	if (cnt > 0)
		m_name0 = m_names.GetAt(0);
	if (cnt > 1)
		m_name1 = m_names.GetAt(1);
	if (cnt > 2)
		m_name2 = m_names.GetAt(2);
	if (cnt > 3)
		m_name3 = m_names.GetAt(3);
	if (cnt > 4)
		m_name4 = m_names.GetAt(4);
	if (cnt > 5)
		m_name5 = m_names.GetAt(5);
	if (cnt > 6)
		m_name6 = m_names.GetAt(6);
	if (cnt > 7)
		m_name7 = m_names.GetAt(7);
	if (cnt > 8)
		m_name8 = m_names.GetAt(8);
	if (cnt > 9)
		m_name9 = m_names.GetAt(9);
	UpdateData(FALSE);
}

// ************************************************************************
// ************************************************************************

// get the color range selected; range must be large enough to hold data

void CShapeColorRangeDlg::update_selected_color(COLORREF color) 
{
	if (m_selected_color >= 0)
	{
		m_color_range[m_selected_color] = m_color_rgb;
		CDrawColorDlg::draw_color_button_rgb(this, m_color_id[m_selected_color], m_color_rgb);
	}
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::set_selected_color(int num) 
{
	int k;

	if (num < 0)
		return;
	if (num > 9)
		return;

	for (k=0; k<10; k++)
		m_color_selected[k] = FALSE;

	m_selected_color = num;
	m_color_selected[m_selected_color] = TRUE;
	draw_the_buttons();
	m_color_rgb = m_color_range[m_selected_color];
	m_red = GetRValue(m_color_rgb);
	m_grn = GetGValue(m_color_rgb);
	m_blu = GetBValue(m_color_rgb);
	rgb_to_hsl_int(m_red, m_grn, m_blu, &m_hue, &m_sat, &m_lum);
	update_color_edit();
	draw_the_buttons();
}

// ************************************************************************
// ************************************************************************

// get the color range selected; range must be large enough to hold data

void CShapeColorRangeDlg::update_color_bar() 
{
	int k;
	double h, s, l, linc, tl, tf, fl;
	int red, grn, blu;
	double co2[3] = {0.25, 0.8};
	double co3[3] = {0.25, 0.55, 0.9};
	double co4[4] = {0.25, 0.4, 0.7, 0.9};
	double co5[5] = {0.25, 0.35, 0.55, 0.75, 0.9};
	double co6[6] = {0.25, 0.35, 0.47, 0.68, 0.78, 0.90};
	double co7[7] = {0.25, 0.33, 0.42, 0.56, 0.70, 0.80, 0.9};
	double co8[8] = {0.20, 0.28, 0.37, 0.50, 0.62, 0.70, 0.80, 0.90};
	double co9[9] = {0.20, 0.26, 0.34, 0.42, 0.50, 0.62, 0.70, 0.80, 0.90};
	double co10[10]={0.20, 0.26, 0.33, 0.41, 0.50, 0.60, 0.68, 0.76, 0.80, 0.90};

	if (m_num_colors < 1)
		return;

	if (m_single_hue)
	{
		red = GetRValue(m_single_color_rgb);
		grn = GetGValue(m_single_color_rgb);
		blu = GetBValue(m_single_color_rgb);
		rgb_to_hsl(red, grn, blu, &h, &s, &l);
		fl = l;
		linc = 0.65 / (double) m_num_colors;
		l = linc;
		for (k=0; k<m_num_colors; k++)
		{
			tl = l+0.2;
			tf = (tl - 0.5) * 0.2;
			if (tf < 0)
				tf = -0.15 + tf;
			else
				tf = 0.1 - tf;
			if ((tl < 0.4) || (tl > 0.6))
				tl += tf;
			if (tl < 0.25)
				tl = 0.25;

			switch(m_num_colors)
			{
				case 1: tl = fl; break;
				case 2: tl = co2[k]; break;
				case 3: tl = co3[k]; break;
				case 4: tl = co4[k]; break;
				case 5: tl = co5[k]; break;
				case 6: tl = co6[k]; break;
				case 7: tl = co7[k]; break;
				case 8: tl = co8[k]; break;
				case 9: tl = co9[k]; break;
				case 10: tl = co10[k]; break;
			}

			hsl_to_rgb(h, s, tl, &red, &grn, &blu);
			m_color_range[m_num_colors-k-1] = RGB(red, grn, blu);
			l += linc;
		}
	}
	draw_the_buttons();
}
// end of update_color_bar

// ************************************************************************
// ************************************************************************

// given h,s,l on [0..1], return r,g,b on [0..1]

void CShapeColorRangeDlg::draw_the_buttons()
{
	int k;

	for (k=0; k<m_num_colors; k++)
	{
		CDrawColorDlg::draw_color_button_rgb(this, m_color_id[k], m_color_range[k]);
		draw_hilight_rect(m_color_id[k], m_color_selected[k]);
	}

/*	
	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR0, m_color_range[0]);
	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR1, m_color_range[1]);
	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR2, m_color_range[2]);
	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR3, m_color_range[3]);
	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR4, m_color_range[4]);
	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR5, m_color_range[5]);
	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR6, m_color_range[6]);
	CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR7, m_color_range[7]);
	draw_hilight_rect(IDC_COLOR0, m_color_selected[0]);
	draw_hilight_rect(IDC_COLOR1, m_color_selected[1]);
	draw_hilight_rect(IDC_COLOR2, m_color_selected[2]);
	draw_hilight_rect(IDC_COLOR3, m_color_selected[3]);
	draw_hilight_rect(IDC_COLOR4, m_color_selected[4]);
	draw_hilight_rect(IDC_COLOR5, m_color_selected[5]);
	draw_hilight_rect(IDC_COLOR6, m_color_selected[6]);
	draw_hilight_rect(IDC_COLOR7, m_color_selected[7]);
*/
}

// ************************************************************************
// ************************************************************************

// given h,s,l on [0..1], return r,g,b on [0..1]

void CShapeColorRangeDlg::update_color_edit()
{
	m_red = GetRValue(m_color_rgb);
	m_grn = GetGValue(m_color_rgb);
	m_blu = GetBValue(m_color_rgb);
	m_edit_red.Format("%d", m_red);
	m_edit_green.Format("%d", m_grn);
	m_edit_blue.Format("%d", m_blu);
	m_edit_hue.Format("%d", m_hue);
	m_edit_sat.Format("%d", m_sat);
	m_edit_lum.Format("%d", m_lum);
	UpdateData(FALSE);
}

// ************************************************************************
// ************************************************************************

// given h,s,l on [0..1], return r,g,b on [0..1]

void CShapeColorRangeDlg::hsl_to_rgb(double h, double sl, double l, 
												 double *r, double *g, double *b)
{
    double v;

	 // do not allow h == 1
	 if (h > 0.99)
		 h = 0.99;

    v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
    if (v <= 0) 
	 {
		*r = *g = *b = 0.0;
    } 
	 else 
	 {
		double m;
		double sv;
		int sextant;
		double fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0;
		sextant = (int) h;	
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant) 
		{
			case 0: *r = v; *g = mid1; *b = m; break;
			case 1: *r = mid2; *g = v; *b = m; break;
			case 2: *r = m; *g = v; *b = mid1; break;
			case 3: *r = m; *g = mid2; *b = v; break;
			case 4: *r = mid1; *g = m; *b = v; break;
			case 5: *r = v; *g = m; *b = mid2; break;
		}
    }
}
// end of hsl_to_rgb

// ************************************************************************
// ************************************************************************

// given h,s,l on [0..1], return r,g,b on [0..255]
     
void CShapeColorRangeDlg::hsl_to_rgb(double h, double sl, double l, 
												 int *r, int *g, int *b)
{
	double red, grn, blu;

	hsl_to_rgb(h, sl, l, &red, &grn, &blu);
	*r = (int) (red * 255.0);
	*g = (int) (grn * 255.0);
	*b = (int) (blu * 255.0);
}

// ************************************************************************
// ************************************************************************

// given h,s,l on [0..255], return r,g,b on [0..255]
     
void CShapeColorRangeDlg::hsl_to_rgb_int(int h, int s, int l, 
												 int *r, int *g, int *b)
{
	double red, grn, blu;
	double hue, sat, lum;

	hue = (double) h / 255.0;
	sat = (double) s / 255.0;
	lum = (double) l / 255.0;

	hsl_to_rgb(hue, sat, lum, &red, &grn, &blu);

	*r = (int) (red * 255.0);
	*g = (int) (grn * 255.0);
	*b = (int) (blu * 255.0);
}

// ************************************************************************
// ************************************************************************

// given r,g,b on [0 ... 1], return (h,s,l) on [0 ... 1]
    
void CShapeColorRangeDlg::rgb_to_hsl(double r, double g, double b, double *h, double *s, double *l)
{
	double v;
	double m;
	double vm;
	double r2, g2, b2;

	v = r;
	if (g > v)
		v = g;
	if (b > v)
		v = b;
	m = r;
	if (g < m)
		m = g;
	if (b < m)
		m = b;

	if ((*l = (m + v) / 2.0) <= 0.0) 
		return;
	if ((*s = vm = v - m) > 0.0) 
	{
	*s /= (*l <= 0.5) ? (v + m ) :
		(2.0 - v - m) ;
	} 
	else
		return;


	r2 = (v - r) / vm;
	g2 = (v - g) / vm;
	b2 = (v - b) / vm;

	if (r == v)
		*h = (g == m ? 5.0 + b2 : 1.0 - g2);
	else if (g == v)
		*h = (b == m ? 1.0 + r2 : 3.0 - b2);
	else
		*h = (r == m ? 3.0 + g2 : 5.0 - r2);

	*h /= 6;
}

// ************************************************************************
// ************************************************************************

// given r,g,b on [0 ... 255], return (h,s,l) on [0 ... 1]
    
void CShapeColorRangeDlg::rgb_to_hsl(int r, int g, int b, double *h, double *s, double *l)
{
	double red, grn, blu;

	red = (double) r / 255.0;
	grn = (double) g / 255.0;
	blu = (double) b / 255.0;

	rgb_to_hsl(red, grn, blu, h, s, l);
}

// ************************************************************************
// ************************************************************************

// given r,g,b on [0 ... 255], return (h,s,l) on [0 ... 255]
    
void CShapeColorRangeDlg::rgb_to_hsl_int(int r, int g, int b, int *h, int *s, int *l)
{
	double red, grn, blu;
	double hue, sat, lum;

	red = (double) r / 255.0;
	grn = (double) g / 255.0;
	blu = (double) b / 255.0;

	rgb_to_hsl(red, grn, blu, &hue, &sat, &lum);
	*h = (int) (hue * 255.0);
	*s = (int) (sat * 255.0);
	*l = (int) (lum * 255.0);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::draw_hilight_rect(int nIDCtl, BOOL selected) 
{
	CWnd *wnd;
	CPen blackpen, graypen;
	CPen *oldpen;
	POINT pt[5];
	RECT rc, wrc;

	wnd = GetDlgItem(nIDCtl);
	if (wnd == NULL)
		return;

//	CWindowDC dc(wnd);
	CWindowDC dc(this);
//	wnd->GetClientRect(&rc);

	GetWindowRect(&wrc);

	wnd->GetWindowRect(&rc);
	rc.left -= wrc.left;
	rc.top  -= wrc.top;
	rc.right -= wrc.left;
	rc.bottom -= wrc.top;
	
	blackpen.CreatePen(PS_DOT, 1, RGB(0,0,0));
	graypen.CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
	
	pt[0].x = rc.left-2;
	pt[0].y = rc.top-2;
	pt[1].x = rc.right+2;
	pt[1].y = rc.top-2;
	pt[2].x = rc.right+2;
	pt[2].y = rc.bottom+2;
	pt[3].x = rc.left-2;
	pt[3].y = rc.bottom+2;
	pt[4].x = rc.left-2;
	pt[4].y = rc.top-2;

	int oldbkmode = dc.SetBkMode(TRANSPARENT);
	oldpen = (CPen*) dc.SelectObject(&blackpen);
	if (!selected)
		dc.SelectObject(&graypen);
	dc.Polyline(pt, 5);
	dc.SelectObject(oldpen);
	dc.SetBkMode(oldbkmode);

	blackpen.DeleteObject();
	graypen.DeleteObject();
}
// end of draw_hilight_rect

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   switch (nIDCtl) 
   {
		case IDC_COLOR:
			CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR, m_single_color_rgb);
			break;
		case IDC_COLOR0:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[0]);
			draw_hilight_rect(nIDCtl, m_color_selected[0]);
			break;
		case IDC_COLOR1:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[1]);
			draw_hilight_rect(nIDCtl, m_color_selected[1]);
			break;
		case IDC_COLOR2:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[2]);
			draw_hilight_rect(nIDCtl, m_color_selected[2]);
			break;
		case IDC_COLOR3:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[3]);
			draw_hilight_rect(nIDCtl, m_color_selected[3]);
			break;
		case IDC_COLOR4:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[4]);
			draw_hilight_rect(nIDCtl, m_color_selected[4]);
			break;
		case IDC_COLOR5:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[5]);
			draw_hilight_rect(nIDCtl, m_color_selected[5]);
			break;
		case IDC_COLOR6:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[6]);
			draw_hilight_rect(nIDCtl, m_color_selected[6]);
			break;
		case IDC_COLOR7:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[7]);
			draw_hilight_rect(nIDCtl, m_color_selected[7]);
			break;
		case IDC_COLOR8:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[8]);
			draw_hilight_rect(nIDCtl, m_color_selected[8]);
			break;
		case IDC_COLOR9:
			CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_color_range[9]);
			draw_hilight_rect(nIDCtl, m_color_selected[9]);
			break;
	}
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
// end of OnDrawItem

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnKillfocusEditBlue() 
{
	int num;

	UpdateData(TRUE);

	num = atoi(m_edit_blue);
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_blu = (BYTE) num;
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	rgb_to_hsl_int(m_red, m_grn, m_blu, &m_hue, &m_sat, &m_lum);
	update_color_edit();
	update_selected_color(m_color_rgb);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnKillfocusEditGreen() 
{
	int num;

	UpdateData(TRUE);
	
	num = atoi(m_edit_green);
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_grn = (BYTE) num;
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	rgb_to_hsl_int(m_red, m_grn, m_blu, &m_hue, &m_sat, &m_lum);
	update_color_edit();
	update_selected_color(m_color_rgb);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnKillfocusEditRed() 
{
	int num;

	UpdateData(TRUE);
	
	num = atoi(m_edit_red);
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_red = (BYTE) num;
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	rgb_to_hsl_int(m_red, m_grn, m_blu, &m_hue, &m_sat, &m_lum);
	update_color_edit();
	update_selected_color(m_color_rgb);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDeltaposSpinBlue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int num;

	UpdateData(TRUE);
	
	num = m_blu;
	num += pNMUpDown->iDelta;
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_blu = (BYTE) num;

	m_color_rgb = RGB(m_red, m_grn, m_blu);
	rgb_to_hsl_int(m_red, m_grn, m_blu, &m_hue, &m_sat, &m_lum);
	update_color_edit();
	update_selected_color(m_color_rgb);

	*pResult = 0;
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDeltaposSpinGreen(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int num;

	UpdateData(TRUE);
	m_red = GetRValue(m_color_rgb);
	m_grn = GetGValue(m_color_rgb);
	m_blu = GetBValue(m_color_rgb);
	
	num = m_grn;
	num += pNMUpDown->iDelta;
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_grn = (BYTE) num;

	m_color_rgb = RGB(m_red, m_grn, m_blu);
	rgb_to_hsl_int(m_red, m_grn, m_blu, &m_hue, &m_sat, &m_lum);
	update_color_edit();
	update_selected_color(m_color_rgb);
	
	*pResult = 0;
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDeltaposSpinRed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int num;

	UpdateData(TRUE);
	
	num = m_red;
	num += pNMUpDown->iDelta;
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_red = (BYTE) num;

	m_color_rgb = RGB(m_red, m_grn, m_blu);
	rgb_to_hsl_int(m_red, m_grn, m_blu, &m_hue, &m_sat, &m_lum);
	update_color_edit();
	update_selected_color(m_color_rgb);
	
	*pResult = 0;
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnKillfocusEditHue() 
{
	int num;

	UpdateData(TRUE);
	
	num = atoi(m_edit_hue);
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_hue = (BYTE) num;
	hsl_to_rgb_int(m_hue, m_sat, m_lum, &m_red, &m_grn, &m_blu);
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	update_color_edit();
	update_selected_color(m_color_rgb);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnKillfocusEditLum() 
{
	int num;

	UpdateData(TRUE);
	
	num = atoi(m_edit_lum);
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_lum = (BYTE) num;
	hsl_to_rgb_int(m_hue, m_sat, m_lum, &m_red, &m_grn, &m_blu);
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	update_color_edit();
	update_selected_color(m_color_rgb);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnKillfocusEditSat() 
{
	int num;

	UpdateData(TRUE);
	
	num = atoi(m_edit_sat);
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_sat = (BYTE) num;
	hsl_to_rgb_int(m_hue, m_sat, m_lum, &m_red, &m_grn, &m_blu);
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	update_color_edit();
	update_selected_color(m_color_rgb);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDeltaposSpinHue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int num;

	num = m_hue;
	num -= pNMUpDown->iDelta;
	if (num < 0)
		num = 255;
	if (num > 255)
		num = 0;
	m_hue = (BYTE) num;

	hsl_to_rgb_int(m_hue, m_sat, m_lum, &m_red, &m_grn, &m_blu);
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	update_color_edit();
	update_selected_color(m_color_rgb);
		
	*pResult = 0;
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDeltaposSpinLum(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int num;

	num = m_lum;
	num -= pNMUpDown->iDelta;
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_lum = (BYTE) num;

	hsl_to_rgb_int(m_hue, m_sat, m_lum, &m_red, &m_grn, &m_blu);
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	update_color_edit();
	update_selected_color(m_color_rgb);
	
	*pResult = 0;
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDeltaposSpinSat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int num;

	num = m_sat;
	num -= pNMUpDown->iDelta;
	if (num < 0)
		num = 0;
	if (num > 255)
		num = 255;
	m_sat = (BYTE) num;

	hsl_to_rgb_int(m_hue, m_sat, m_lum, &m_red, &m_grn, &m_blu);
	m_color_rgb = RGB(m_red, m_grn, m_blu);
	update_color_edit();
	update_selected_color(m_color_rgb);
	
	*pResult = 0;
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::show_window(int id, BOOL show) 
{
	CWnd *wnd;

	wnd = GetDlgItem(id);
	if (wnd != NULL)
	{
		wnd->ShowWindow(show);
	}
	else
	{
      ERR_report("GetDlgItem failed on Color Range Dlg");
		ASSERT(0);
	}
}
// end of show_window

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor0() 
{
	set_selected_color(0);
}	

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor1() 
{
	set_selected_color(1);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor2() 
{
	set_selected_color(2);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor3() 
{
	set_selected_color(3);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor4() 
{
	set_selected_color(4);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor5() 
{
	set_selected_color(5);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor6() 
{
	set_selected_color(6);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor7() 
{
	set_selected_color(7);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor8() 
{
	set_selected_color(8);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnColor9() 
{
	set_selected_color(9);
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnMultipleColors() 
{
	int inc, k, j;
	int red, grn, blu, hue, sat, lum;

	if (m_num_colors < 1)
		return;

	inc = 255 / m_num_colors;

	red = GetRValue(m_single_color_rgb);
	grn = GetGValue(m_single_color_rgb);
	blu = GetBValue(m_single_color_rgb);
	
	rgb_to_hsl_int(red, grn, blu, &hue, &sat, &lum);
	
	j = 0;
	for (k=0; k<m_num_colors; k++)
	{
		hsl_to_rgb_int(hue, sat, lum, &red, &grn, &blu);
		m_color_range[k] = RGB(red, grn, blu);
		hue += inc;
		if (hue > 255)
			hue -= 255;
	}
	draw_the_buttons();
}

// ************************************************************************
// ************************************************************************
// Public Functions
// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::set_names( CStringArray &names)
{
	int cnt, k;
	CString txt;

	m_names.RemoveAll();
	cnt = names.GetSize();
	m_num_colors = cnt;
	for (k=0; k<cnt; k++)
	{
		txt = names.GetAt(k);
		m_names.SetAtGrow(k, txt);
	}
}

// ************************************************************************
// ************************************************************************

// get the color range selected; range must be large enough to hold data

void CShapeColorRangeDlg::get_color_range(COLORREF *range) 
{
	memcpy(range, m_color_range, m_num_colors * sizeof(COLORREF));
}

// ************************************************************************
// ************************************************************************

// set the color range selected; range must be large enough to hold data

void CShapeColorRangeDlg::set_color_range(COLORREF *range) 
{
	memcpy(m_color_range, range, m_num_colors * sizeof(COLORREF));
}

// ************************************************************************
// ************************************************************************

// set the color range selected; range must be large enough to hold data

void CShapeColorRangeDlg::set_initial_color(COLORREF color) 
{
	m_single_color_rgb = color;
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::get_color() 
{
	CHOOSECOLOR cc;                 // common dialog box structure 

	CShapeFileOvlOptionsPage::load_custom_colors();

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(CHOOSECOLOR));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = this->m_hWnd;
	cc.lpCustColors = (LPDWORD) CShapeFileOvlOptionsPage::m_CustClr;
	cc.rgbResult = m_color_rgb;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
 
	if (ChooseColor(&cc)!=TRUE) 
		return;

	m_color_rgb = cc.rgbResult;

	update_selected_color(m_color_rgb);

	CShapeFileOvlOptionsPage::save_custom_colors();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor0() 
{
	set_selected_color(0);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor1() 
{
	set_selected_color(1);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor2() 
{
	set_selected_color(2);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor3() 
{
	set_selected_color(3);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor4() 
{
	set_selected_color(4);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor5() 
{
	set_selected_color(5);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor6() 
{
	set_selected_color(6);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor7() 
{
	set_selected_color(7);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor8() 
{
	set_selected_color(8);
	get_color();
}

// ************************************************************************
// ************************************************************************

void CShapeColorRangeDlg::OnDoubleclickedColor9() 
{
	set_selected_color(9);
	get_color();
}

// ************************************************************************
// ************************************************************************
LRESULT CShapeColorRangeDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

