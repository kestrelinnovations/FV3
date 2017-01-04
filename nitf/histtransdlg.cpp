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

// histtransdlg.cpp : implementation file
//



#include "stdafx.h"

#ifdef JMPS_VARIANT
   #include "NITFOverlayVPS.h"
#else
   #include "..\fvw.h"
#endif
#include "HistTransDlg.h"

#define NITF_CONVERSIONS_ONLY
#include "NITFUtilities.h"
#include "..\getobjpr.h"

#undef NITF_CONVERSIONS_ONLY

#ifdef _DEBUG
   #define new DEBUG_NEW
   #undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistTransDlg dialog


// ****************************************************************
// ****************************************************************
CHistTransDlg::CHistTransDlg(
#ifdef JMPS_VARIANT
      CNITFOverlayElement* pNITFOverlayElement,
      CImageCache*         pImageCache,
      CNITFOverlayVPS*     pNITFOverlayVPS,
      CWnd*                pParent /*=NULL*/) :
   m_pNITFOverlayVPS( pNITFOverlayVPS ),
#else
      C_nitf_obj*          pNITFObj,
      CWnd*                pParent /*=NULL*/) :
   m_pNITFObj( pNITFObj ),
#endif
   CDialog( CHistTransDlg::IDD, pParent )
{
   m_cbImageLibCallback.m_pHistTransDlg = this;
   m_hWaitCursor = ::LoadCursor( NULL, IDC_WAIT );


	//{{AFX_DATA_INIT(CHistTransDlg)
	m_center = 0;
	m_maximum = 0;
	m_minimum = 0;
	//}}AFX_DATA_INIT

#ifdef JMPS_VARIANT
   m_eEqualizationMode = pNITFOverlayElement->m_eEqualizationMode;
   m_trans_level = pNITFOverlayElement->m_lImageOpacity;
   m_white_val = pNITFOverlayElement->m_iMaxDisplayLevel;
   m_gray_val = pNITFOverlayElement->m_iMidDisplayLevel;
   m_black_val = pNITFOverlayElement->m_iMinDisplayLevel;
   m_bstrDisplayParamsXML = pNITFOverlayElement->m_bstrDisplayParamsXML;
   m_spImageLib = pImageCache->m_pImageLib;
#else
	if ( m_pNITFObj )
	{
		m_black_val = m_pNITFObj->m_contr_minval;
		m_gray_val = m_pNITFObj->m_contr_ctrval;
		m_white_val = m_pNITFObj->m_contr_maxval;
		m_eEqualizationMode = (EnumEqualizationMode) m_pNITFObj->m_eq_mode;
		m_trans_level = (INT) ( 0.5 + ( m_pNITFObj->m_trans_value / 2.55 ) );
      m_bstrDisplayParamsXML = m_pNITFObj->m_pxdDisplayParams->xml;
      m_spImageLib = m_pNITFObj->m_pImageLib;
	  m_is_multispectral = m_pNITFObj->m_is_multispectral;
	}
#endif

   m_puiFreqLum = NULL;
   m_bGetHistogramDone = FALSE;
   m_hit_marker = 0;
#if 0
	m_old_x = -1;
	m_old_y = -1;
#endif
	m_gray_percent = 0.5;
	m_in_process = FALSE;
}


// ****************************************************************
// ****************************************************************

void CHistTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistTransDlg)
	DDX_Control(pDX, IDC_TRANS_SLIDER, m_trans_slider);
	DDX_Text(pDX, IDC_CENTER, m_center);
	DDX_Text(pDX, IDC_MAXIMUM, m_maximum);
	DDX_Text(pDX, IDC_MINIMUM, m_minimum);
	DDX_Text(pDX, IDC_TRANS_LEVEL, m_trans_level);
	DDV_MinMaxInt(pDX, m_trans_level, 0, 100 );
#ifdef IDC_HISTOGRAM_PROGRESS
   DDX_Text(pDX, IDC_HISTOGRAM_PROGRESS, m_csHistogramProgress );
#endif
	//}}AFX_DATA_MAP
}


// ****************************************************************
// ****************************************************************

BEGIN_MESSAGE_MAP(CHistTransDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CHistTransDlg)
	ON_EN_KILLFOCUS(IDC_TRANS_LEVEL, OnKillfocusTransLevel)
	ON_EN_KILLFOCUS(IDC_CENTER, OnKillfocusCenter)
	ON_EN_KILLFOCUS(IDC_MAXIMUM, OnKillfocusMaximum)
	ON_EN_KILLFOCUS(IDC_MINIMUM, OnKillfocusMinimum)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(ID_AUTO, OnAuto)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_AUTO_SCREEN_CONTRAST, OnAutoScreenContrast)
#if 0
	ON_BN_CLICKED(IDC_HISTOGRAM_CONTRAST, OnHistogramContrast)
#endif
	ON_BN_CLICKED(IDC_ENHANCE_AUTO, OnEnhanceAuto)
	ON_BN_CLICKED(IDC_ENHANCE_HISTOGRAM, OnEnhanceHistogram)
	ON_BN_CLICKED(IDC_ENHANCE_NONE, OnEnhanceNone)
	ON_BN_CLICKED(IDC_MS_BANDS, OnMsBands)
	ON_WM_TIMER()
   ON_MESSAGE(WM_GET_HISTOGRAM, OnGetHistogram)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistTransDlg message handlers

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnOK() 
{
#ifdef JMPS_VARIANT
   wostringstream wssSetList;
   BOOL bOnce = FALSE;

   if ( m_eEqualizationMode != m_eFirstEqualizationMode )
   {
      wssSetList << L"EqualizationMode = " << (INT) m_eEqualizationMode;
      bOnce = TRUE;
   }

   if ( m_trans_level != m_first_trans_level )
   {
      if ( bOnce )
         wssSetList << L",\n";
      else
         bOnce = TRUE;
      wssSetList << L"Opacity = " << m_trans_level;
   }

   if ( m_black_val != m_first_min
      || m_gray_val != m_first_ctr
      || m_white_val != m_first_max )
   {     
      if ( bOnce )
         wssSetList << L",\n";
      else
         bOnce = TRUE;
      wssSetList <<
         L"MinDisplayLevel = " << m_black_val << L",\n"
         L"MidDisplayLevel = " << m_gray_val << L",\n"
         L"MaxDisplayLevel = " << m_white_val;
   }

   if ( m_bstrDisplayParamsXML != m_bstrFirstDisplayParamsXML )
   {
      if ( bOnce )
         wssSetList << L",\n";
      else
         bOnce = TRUE;

      wstring wstr = m_bstrDisplayParamsXML;
      NITFQuoteValue( wstr );
      wssSetList <<
         L"DisplayParamsXML = " << wstr;
   }

   if ( bOnce )
   {
      wssSetList << L"\n";
      m_bstrDBUpdateSQL = wssSetList.str().c_str();
   }
#else
   if ( m_pNITFObj != NULL )
   {
      UpdateDisplayParamsXML();
	   m_pNITFObj->set_display_params();
   }
#endif
	
	CDialog::OnOK();
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnCancel() 
{
#ifndef JMPS_VARIANT
   if ( m_pNITFObj != NULL )
   {
	   m_pNITFObj->m_contr_minval = m_first_min;
	   m_pNITFObj->m_contr_ctrval = m_first_ctr;
	   m_pNITFObj->m_contr_maxval = m_first_max;
      m_pNITFObj->m_eq_mode = m_eFirstEqualizationMode;
      m_pNITFObj->m_trans_value = (INT) ( 0.5 + ( m_first_trans_level * 2.55 ) );
      m_pNITFObj->m_pxdDisplayParams->loadXML( m_bstrFirstDisplayParamsXML );
      m_pNITFObj->set_display_params();   // Update db
	   redraw_obj();
   }
#endif
	
	CDialog::OnCancel();
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnKillfocusTransLevel() 
{
	UpdateData( TRUE );
	if (m_trans_level < 0)
		m_trans_level = 0;
	if (m_trans_level > 100)
		m_trans_level = 100;
	m_trans_slider.SetPos( 100 - m_trans_level );
	UpdateData( FALSE );
	KillTimer( m_uiTransTimerID );
	m_uiTransTimerID = SetTimer( NITF_TRANS_TIMER, 300, NULL );
#ifndef JMPS_VARIANT
	if ( m_pNITFObj != NULL )
	{
		m_pNITFObj->m_trans_value = (int) ( 0.5 + ( m_trans_level * 2.55 ) );
		m_pNITFObj->set_display_mode();
	}
#endif
}


// ****************************************************************
// ****************************************************************

BOOL CHistTransDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// calculate mouse offset from image windows
	CRect dlg_rc, img_rc;
	CWnd *wnd;

//	GetClientRect(&dlg_rc);
//	GetDlgItem(IDC_HISTVIEW)->GetClientRect(&img_rc);
	GetWindowRect(&dlg_rc);
	GetDlgItem(IDC_HISTVIEW)->GetWindowRect(&img_rc);

	m_mouse_off_x = img_rc.left - dlg_rc.left;
	m_mouse_off_y = img_rc.top - dlg_rc.top;

   m_trans_slider.SetPos( 100 - m_trans_level );

	// check ranges and correct if necessary
	if (m_black_val < 0)
		m_black_val = 0;
	if (m_white_val > 255)
		m_white_val = 255;
	if (m_black_val > m_white_val)
		m_black_val = 0;
	if ((m_gray_val <= m_black_val) || (m_gray_val >= m_white_val))
		m_gray_val = (m_black_val + m_white_val) / 2;

	if ( m_eEqualizationMode == EQUALIZATION_MODE_AUTO_LINEAR )
		CheckRadioButton(IDC_ENHANCE_NONE, IDC_ENHANCE_HISTOGRAM, IDC_ENHANCE_AUTO );
	else if ( m_eEqualizationMode == EQUALIZATION_MODE_MANUAL_LINEAR)
		CheckRadioButton( IDC_ENHANCE_NONE, IDC_ENHANCE_HISTOGRAM, IDC_ENHANCE_HISTOGRAM );
	else
		CheckRadioButton( IDC_ENHANCE_NONE, IDC_ENHANCE_HISTOGRAM, IDC_ENHANCE_NONE );

	show_histogram_stuff();

	m_minimum = m_black_val;
	m_center  = m_gray_val;
	m_maximum = m_white_val;
	m_first_min = m_black_val;
	m_first_ctr  = m_gray_val;
	m_first_max = m_white_val;
   m_eFirstEqualizationMode = m_eEqualizationMode;
   m_first_trans_level = m_trans_level;
   m_bstrFirstDisplayParamsXML = m_bstrDisplayParamsXML;

	CWindowDC dc(NULL);
	int bpp = GetDeviceCaps(dc.GetSafeHdc(), BITSPIXEL);

	if (FALSE)
//	if (bpp < 24)
	{
		wnd = GetDlgItem(IDC_TRANS_SLIDER);
		if (wnd)
			wnd->EnableWindow(FALSE);
	}

	wnd = GetDlgItem(IDC_MS_BANDS);
	if (wnd)
	{
		if (!m_is_multispectral)
			wnd->ShowWindow(SW_HIDE);
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnKillfocusCenter() 
{
	CString msg;
	BOOL good_values;
	int oldctr;

	if (m_in_process)
		return;

	good_values = ranges_ok();
	oldctr = m_center;
	m_in_process = TRUE;
	UpdateData(TRUE);
	if ((m_center < m_minimum) || (m_center > m_maximum))
	{
		if (good_values)
			m_center = oldctr;
		else;
			m_center = (m_minimum + m_maximum) / 2;
		UpdateData(FALSE);
		msg.Format( _T("The Center value must be between Black value (%d) and White value (%d)"),
         m_minimum, m_maximum );
		AfxMessageBox(msg);
		GetDlgItem(IDC_CENTER)->SetFocus();
		SendDlgItemMessage(IDC_CENTER, EM_SETSEL, 0, -1);
	}
	m_gray_val = m_center;
	draw_markers();
	refresh_screen();
	m_in_process = FALSE;
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnKillfocusMaximum() 
{
	CString msg;
	BOOL good_values;
	int oldmax;

	if (m_in_process)
		return;

	m_in_process = TRUE;
	good_values = ranges_ok();
	oldmax = m_maximum;
	UpdateData(TRUE);
	if ((m_maximum > 255) || (m_maximum <= m_center))
	{
		if (good_values)
			m_maximum = oldmax;
		else
			m_maximum = 255;
		UpdateData(FALSE);
		msg.Format( _T("The White value must be between Center value (%d) and 255" ), m_center);
		AfxMessageBox(msg);
		GetDlgItem(IDC_MAXIMUM)->SetFocus();
		SendDlgItemMessage(IDC_MAXIMUM, EM_SETSEL, 0, -1);
	}
	m_white_val = m_maximum;
	adjust_gray();
	m_center = m_gray_val;
	UpdateData(FALSE);
	draw_markers();
	refresh_screen();
	m_in_process = FALSE;
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnKillfocusMinimum() 
{
	CString msg;
	BOOL good_values;
	int oldmin;

	if (m_in_process)
		return;

	m_in_process = TRUE;
	good_values = ranges_ok();
	oldmin = m_minimum;
	UpdateData(TRUE);
	if ((m_minimum < 0) || (m_minimum >= m_center))
	{
		if (good_values)
			m_minimum = oldmin;
		else
			m_minimum = 0;
		UpdateData(FALSE);
		msg.Format( _T("The Black value must be between 0 and Center value (%d)"), m_center );
		AfxMessageBox(msg);
		GetDlgItem(IDC_MINIMUM)->SetFocus();
		SendDlgItemMessage(IDC_MINIMUM, EM_SETSEL, 0, -1);
	}
	m_black_val = m_minimum;
	adjust_gray();
	m_center = m_gray_val;
	UpdateData(FALSE);
	draw_markers();
	refresh_screen();
	m_in_process = FALSE;
}

// ****************************************************************
// ****************************************************************

BOOL CHistTransDlg::ranges_ok() 
{
	if ((m_minimum < 0) || (m_minimum >= m_center))
		return FALSE;
	if ((m_maximum > 255) || (m_maximum <= m_center))
		return FALSE;
	if ((m_center < m_minimum) || (m_center > m_maximum))
		return FALSE;

	return TRUE;
}

// **********************************************************************
// ****************************************************************

void CHistTransDlg::OnPaint() 
{
	if ( m_eEqualizationMode == EQUALIZATION_MODE_MANUAL_LINEAR )
	{
		draw_display();
		draw_markers();
	}

   CDialog::OnPaint();
}

// **********************************************************************
// **********************************************************************

#define PIXELS_FROM_EDGE 15

void CHistTransDlg::draw_page(CDC *pDC, CRect client_rect)
{
//	CUtil util;
	int width, height, k, rslt;
	
	width = client_rect.Width() - PIXELS_FROM_EDGE * 2;
	height = client_rect.Height() - PIXELS_FROM_EDGE * 2;

	CBrush dark_gray_brush(RGB(192, 192, 192));
	CBrush bright_white_brush(RGB(255,255,255));
	CBrush black_brush(RGB(0,0,0));
	CPen pen(PS_SOLID, 1, RGB(0,0,0));
	CPen graypen(PS_SOLID, 2, RGB(128, 128, 128));
	CPen yellowpen(PS_SOLID, 2, RGB(255,255,0));

	CRect rect;

	// fill background
	pDC->FillRect(&client_rect, &dark_gray_brush);

	rect = client_rect;
	rect.top +=  PIXELS_FROM_EDGE;
	rect.bottom -= PIXELS_FROM_EDGE + 5;  // account for the buttons
	rect.left += PIXELS_FROM_EDGE;
	rect.right -= PIXELS_FROM_EDGE;

	m_img_rect = rect;

	rect.TopLeft() += CPoint(6,6);
	rect.BottomRight() += CPoint(6,6);

	m_img_rect.TopLeft() += CPoint(1,1);
	m_img_rect.BottomRight() -= CPoint(1,1);

	m_marker_rect = m_img_rect;
	m_marker_rect.top = m_img_rect.bottom + 3;
	m_marker_rect.bottom = m_img_rect.bottom + 15;


	rslt = compute_hist_data();
	if (rslt == SUCCESS)
	{
		pDC->SelectObject(&pen);

		for (k=0; k<256; k++)
		{
			pDC->MoveTo(m_img_rect.left + k, m_img_rect.bottom);
			pDC->LineTo(m_img_rect.left + k, m_img_rect.bottom - m_hist[k] - 1);
		}
	}
	UpdateData(FALSE);
}
// end of draw_page

// **********************************************************************
// **********************************************************************

void CHistTransDlg::draw_display() 
{
   CWnd* wnd = GetDlgItem( IDC_HISTVIEW );
   CDC *pDC = wnd->GetDC();
   if ( !pDC )
//      ERR_report("GetDC() failed.");
	   AfxMessageBox( _T("GetDC() failed.") );

   else
   {
      CRect client_rect;
      wnd->GetClientRect( &client_rect );
      
      if ( m_puiFreqLum == NULL )
         PostMessage( WM_GET_HISTOGRAM );
      else
      {
         CDC memory_dc;
         CBitmap bmp;

         // create an equivalent dc in memory
         memory_dc.CreateCompatibleDC(pDC);
         bmp.CreateCompatibleBitmap(pDC, client_rect.Width(), client_rect.Height());
         memory_dc.SelectObject(&bmp);

         // draw the page
         draw_page(&memory_dc, client_rect);  

         // copy contents of memory dc into the preview window's dc
         pDC->BitBlt(0, 0, client_rect.Width(),
            client_rect.Height(), &memory_dc, client_rect.TopLeft().x, 
            client_rect.TopLeft().y, SRCCOPY);
      }
      
      wnd->ReleaseDC(pDC);
   }
   
	UpdateData( FALSE );

}
// end of draw_display

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_hit_marker = 0;
	
	if ( m_black_val != m_oldmin || m_gray_val != m_oldctr || m_white_val != m_oldmax )
   {
#ifdef JMPS_VARIANT
      m_pNITFOverlayVPS->RefreshActiveView();
#else
	   if ( m_pNITFObj != NULL )
	   {
		   m_pNITFObj->m_contr_minval = m_black_val;
		   m_pNITFObj->m_contr_ctrval = m_gray_val;
		   m_pNITFObj->m_contr_maxval = m_white_val;
         m_pNITFObj->set_display_params();
		   redraw_obj();
	   }
#endif
   }

	CDialog::OnLButtonUp(nFlags, point);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!(nFlags & MK_LBUTTON))
	{
		m_hit_marker = 0;
		return;
	}
	
	m_mouse_x = point.x - m_mouse_off_x;
	m_mouse_y = point.y - m_mouse_off_y;

	if (m_hit_marker)
		drag_marker();

	CDialog::OnMouseMove(nFlags, point);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_hit_marker = marker_hit(point.x+3 - m_mouse_off_x, point.y+3);
#ifdef JMPS_VARIANT
   m_oldmin = m_black_val;
   m_oldctr = m_gray_val;
   m_oldmax = m_white_val;
#else
   if ( m_pNITFObj != NULL )
   {
	   m_oldmin = m_pNITFObj->m_contr_minval;
	   m_oldctr = m_pNITFObj->m_contr_ctrval;
	   m_oldmax = m_pNITFObj->m_contr_maxval;
   }
#endif
	
	CDialog::OnLButtonDown(nFlags, point);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::draw_black_triangle(CDC *dc, int x, int y) 
{
	draw_triangle(dc, RGB(0,0,0), x, y);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::draw_white_triangle(CDC *dc, int x, int y) 
{
	draw_triangle(dc, RGB(255,255,255), x, y);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::draw_gray_triangle(CDC *dc, int x, int y) 
{
	draw_triangle(dc, RGB(128,128,128), x, y);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::draw_triangle(CDC *dc, COLORREF color, int x, int y) 
{
	CPen *oldpen;
	CBrush *oldbrush;
	CPen pen(PS_SOLID, 1, RGB(0,0,0));
	CBrush brush(color);
	POINT pt[3];
	
	oldpen = (CPen*) dc->SelectObject(&pen);
	oldbrush = (CBrush*) dc->SelectObject(&brush);
	pt[0].x = x;
	pt[0].y = y;
	pt[1].x = x - 5;
	pt[1].y = y + 12;
	pt[2].x = x + 5;
	pt[2].y = y + 12;

	dc->Polygon(pt, 3);
	dc->SelectObject(oldpen);
	dc->SelectObject(oldbrush);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::erase_triangle(CDC *dc, int oldval) 
{
	CPen *oldpen;
	CBrush *oldbrush;
	CPen pen(PS_SOLID, 1, RGB(192,192,192));
	CBrush brush(RGB(192,192,192));
	CRect rc;
	
	rc.top = m_marker_rect.top;
	rc.bottom = m_marker_rect.bottom + 3;
	rc.left  = m_img_rect.left + oldval - 5;
	rc.right = m_img_rect.left + oldval + 6;

	oldpen = (CPen*) dc->SelectObject(&pen);
	oldbrush = (CBrush*) dc->SelectObject(&brush);
	dc->Rectangle(&rc);
	dc->SelectObject(oldpen);
	dc->SelectObject(oldbrush);
}

// ****************************************************************
// ****************************************************************

int CHistTransDlg::marker_hit(int x, int y) 
{
	if ((y < m_marker_rect.top+60) || (y > m_marker_rect.bottom+60))
		return 0;

	if ((x > m_img_rect.left + m_black_val - 5) && (x < m_img_rect.left + m_black_val + 5))
	{
		return HISTOGRAM_BLACK_MARKER;
	}

	if ((x > m_img_rect.left + m_gray_val - 5) && (x < m_img_rect.left + m_gray_val + 5))
	{
		return HISTOGRAM_GRAY_MARKER;
	}

	if ((x > m_img_rect.left + m_white_val - 5) && (x < m_img_rect.left + m_white_val + 5))
	{
		return HISTOGRAM_WHITE_MARKER;
	}

	return 0;
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::erase_marker_rect() 
{
	CPen *oldpen;
	CBrush *oldbrush;
	CPen pen(PS_SOLID, 1, RGB(192,192,192));
	CBrush brush(RGB(192,192,192));
	CRect rc;
	CWnd *wnd;

	// create the DC
	wnd = GetDlgItem(IDC_HISTVIEW);
	CDC *dc = wnd->GetDC();
	
	rc = m_marker_rect;
	rc.left -= 7;
	rc.right += 7;
	rc.bottom += 2;
	oldpen = (CPen*) dc->SelectObject(&pen);
	oldbrush = (CBrush*) dc->SelectObject(&brush);
	dc->Rectangle(&rc);
	dc->SelectObject(oldpen);
	dc->SelectObject(oldbrush);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::draw_markers() 
{
	int tx;
	CWnd *wnd;

	erase_marker_rect();

	// create the DC
	wnd = GetDlgItem(IDC_HISTVIEW);
	CDC *pDC = wnd->GetDC();

	tx = m_img_rect.left + m_black_val;
	draw_black_triangle(pDC, tx, m_marker_rect.top);
	tx = m_img_rect.left + m_gray_val;
	draw_gray_triangle(pDC, tx, m_marker_rect.top);
	tx = m_img_rect.left + m_white_val;
	draw_white_triangle(pDC, tx, m_marker_rect.top);
	calc_gray_percent();
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::drag_marker() 
{
	int tx;
	CWnd *wnd;
	BOOL move_gray = FALSE;

	tx = m_mouse_x;

	if (m_hit_marker == 0)
		return;

	// create the DC
	wnd = GetDlgItem(IDC_HISTVIEW);
	CDC *pDC = wnd->GetDC();

	erase_triangle(pDC, m_black_val);
	erase_triangle(pDC, m_gray_val);
	erase_triangle(pDC, m_white_val);

//	calc_gray_percent();

	switch(m_hit_marker)
	{
		case HISTOGRAM_BLACK_MARKER:
			m_black_val = m_mouse_x - m_img_rect.left;
			if (m_black_val > m_white_val - 20)
				m_black_val = m_white_val - 20;
			if (m_black_val <= 0)
				m_black_val = 1;
			move_gray = TRUE;
			break;
		case HISTOGRAM_GRAY_MARKER:
			m_gray_val = m_mouse_x - m_img_rect.left;
			if (m_gray_val > m_white_val - 10)
				m_gray_val = m_white_val - 10;
			if (m_gray_val < m_black_val + 10)
				m_gray_val = m_black_val + 10;
			calc_gray_percent(); 
			break;
		case HISTOGRAM_WHITE_MARKER:
			m_white_val = m_mouse_x - m_img_rect.left;
			if (m_white_val < m_black_val + 20)
				m_white_val = m_black_val + 20;
			if (m_white_val > 255)
				m_white_val = 255;
			move_gray = TRUE;
			break;
	}

	if (move_gray)
		adjust_gray();

	tx = m_img_rect.left + m_black_val;
	draw_black_triangle(pDC, tx, m_marker_rect.top);
	tx = m_img_rect.left + m_gray_val;
	draw_gray_triangle(pDC, tx, m_marker_rect.top);
	tx = m_img_rect.left + m_white_val;
	draw_white_triangle(pDC, tx, m_marker_rect.top);

	m_minimum = m_black_val;
	m_center = m_gray_val;
	m_maximum = m_white_val;
	UpdateData(FALSE);
}
// end of drag_marker

// ****************************************************************
// ****************************************************************

void CHistTransDlg::calc_gray_percent() 
{
	m_gray_percent = (double) (m_gray_val-m_black_val) / (double) (m_white_val-m_black_val);
}


// ****************************************************************
// ****************************************************************

void CHistTransDlg::adjust_gray() 
{
	m_gray_val = (int) ((double) (m_white_val-m_black_val) * m_gray_percent) + m_black_val;
	if ((m_gray_val - m_black_val) < 10)
		m_gray_val = m_black_val + 10;
	if ((m_white_val - m_gray_val) < 10)
		m_gray_val = m_white_val - 10;
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::refresh_screen()
{
#ifdef JMPS_VARIANT
#else
	if ( m_pNITFObj != NULL )
   {
	   m_pNITFObj->m_contr_minval = m_black_val;
	   m_pNITFObj->m_contr_ctrval = m_gray_val;
	   m_pNITFObj->m_contr_maxval = m_white_val;
	   m_pNITFObj->set_display_params();
	   redraw_obj();
   }
#endif
}

// ****************************************************************
// ****************************************************************


void CHistTransDlg::OnAuto() 
{
#ifdef JMPS_VARIANT
      m_black_val = 0;
      m_gray_val = 128;
      m_white_val = 255;
      adjust_gray();
      m_minimum = m_black_val;
      m_center  = m_gray_val;
      m_maximum = m_white_val;
      UpdateData( FALSE );
      draw_markers();
      redraw_obj();
  #else
   if ( m_pNITFObj != NULL )
   {
      m_pNITFObj->auto_contrast();
      m_black_val = m_pNITFObj->m_contr_minval;
      m_gray_val = m_pNITFObj->m_contr_ctrval;
      m_white_val = m_pNITFObj->m_contr_maxval;
      adjust_gray();
      m_minimum = m_black_val;
      m_center  = m_gray_val;
      m_maximum = m_white_val;
      UpdateData( FALSE );
      draw_markers();
      m_pNITFObj->set_display_params();
      redraw_obj();
   }
#endif
}

// ****************************************************************
// ****************************************************************

BOOL CHistTransDlg::GetHistogram() 
{
   do
   {
#ifdef JMPS_VARIANT
      if ( m_puiFreqLum != NULL || m_bGetHistogramDone )
	  {
			int k, total = 0;
			for ( k=0; k<256; k++ )
				total += m_puiFreqLum[k];
			if (total > 1000)
				return FALSE;
	  }
      
      if ( m_spImageLib == NULL )   // ImageLib must have image loaded
         break;

      m_bGetHistogramDone = TRUE;   // One try only
      CWaitCursor wc;

      _variant_t varHist, varFreqRed, varFreqGreen, varFreqBlue;
      INT iErr;
      CComBSTR ccbsErrorMsg;
      if ( S_OK != m_spImageLib->set_callback( &m_cbImageLibCallback ) )
         break;
      if ( S_OK != m_spImageLib->get_histogram( &varHist, &varFreqRed, &varFreqGreen, &varFreqBlue, &iErr, &ccbsErrorMsg ) )
         break;
      m_spImageLib->set_callback( NULL );

      if ( iErr != 0 )
      {
         wc.Restore();
         AfxMessageBox( (COLE2T) ccbsErrorMsg );
         m_spImageLib = NULL;       // Something wrong w/ image
         break;
      }

      PUINT
         pFreqRed = (PUINT) varFreqRed.parray->pvData,
         pFreqGreen = (PUINT) varFreqGreen.parray->pvData,
         pFreqBlue = (PUINT) varFreqBlue.parray->pvData;

      m_apuiFreqLum = auto_ptr< UINT >( new UINT[ 256 ] );
      m_puiFreqLum = m_apuiFreqLum.get();

      BOOL bMonoImage = TRUE;
      for ( INT k = 0; k < 256; k++ )
      {
         if ( pFreqRed[ k ] != pFreqGreen[ k ]
         || pFreqGreen[ k ] != pFreqBlue[ k ] )
         {
            bMonoImage = FALSE;
            break;
         }
      }
      for ( INT k = 0; k < 256; k++ )
      {
         m_puiFreqLum[ k ] = bMonoImage
            ? pFreqRed[ k ]
         : (UINT) ( 0.5 + ( pFreqRed[ k ] * 0.3 )
            + ( pFreqGreen[ k ] * 0.59 ) + ( pFreqBlue[ k ] * 0.11 ) );
      }
      return TRUE;
#else // FalconView
	   if ( m_pNITFObj == NULL )
         break;

      if ( m_pNITFObj->m_freq_lum == NULL )
	   {
         CWaitCursor wc;
	      CString error_msg;
		   if ( SUCCESS !=  m_pNITFObj->get_histogram( error_msg ) )
         {
            wc.Restore();
			   AfxMessageBox( error_msg );
            break;
         }
      }
	  else
	  {
		  // check for bad histogram
		  int k;
		  BYTE maxval = 0;
		  for (k=0; k<256; k++)
		  {
			  if (maxval < m_pNITFObj->m_freq_lum[k])
				  maxval = m_pNITFObj->m_freq_lum[k];
		  }
		  if (maxval < 1)
		  {
			  // get the histogram from the image
			CWaitCursor wc;
			CString error_msg;
			if ( SUCCESS !=  m_pNITFObj->get_histogram( error_msg ) )
			{
				wc.Restore();
				AfxMessageBox( error_msg );
				break;
			}
		  }
	  }

      m_puiFreqLum = m_pNITFObj->m_freq_lum;
      return TRUE;
#endif
   } while ( FALSE );
   m_puiFreqLum = NULL;
   return FALSE;
}

// ****************************************************************
// ****************************************************************

int CHistTransDlg::compute_hist_data() 
{
#if 0
   GetHistogram();
#endif
   if ( m_puiFreqLum == NULL )
      return FAILURE;

	// find the maximum value
   INT k;
	UINT maxval = 0;
	UINT total = 0;
	for ( k=0; k<256; k++ )
	{
		UINT curval = m_puiFreqLum[k];
		total += curval;
		if ( curval > maxval )
			maxval = curval;
	}

	if (maxval < 1)
	{
		GetHistogram();
		maxval = 0;
		total = 0;
		for ( k=0; k<256; k++ )
		{
			UINT curval = m_puiFreqLum[k];
			total += curval;
			if ( curval > maxval )
				maxval = curval;
		}
		if (maxval < 1)
			return FAILURE;
	}

	// find center value
	UINT total2 = total / 2;
	total = 0;
	k = 0;
	while (total < total2)
	{
		total += m_puiFreqLum[k];
		k++;
	}

	m_ctrval = k - 1;

	if ((m_img_rect.Height() - 5) < 1)
	{
		ASSERT(0);
		return FAILURE;
	}

	if (maxval < 1)
	{
		ASSERT(0);
		return FAILURE;
	}

	DOUBLE scale = ( m_img_rect.Height() - 5.0 ) / maxval;
	for (k=0; k<256; k++)
	{
		m_hist[k] = (UINT) ( m_puiFreqLum[k] * scale );
	}

   return SUCCESS;
}
// end of compute_hist_data

// ****************************************************************
// ****************************************************************


void CHistTransDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   if ( m_trans_slider.m_hWnd == pScrollBar->m_hWnd )
   {
      switch( nSBCode )
      {
         case TB_THUMBPOSITION:
         case TB_THUMBTRACK:
            reinterpret_cast< CSliderCtrl* >( pScrollBar )->SetPos( nPos );
            break;
      }

	   if ( m_trans_level != 100 - m_trans_slider.GetPos() )
      {
         m_trans_level = 100 - m_trans_slider.GetPos();
         KillTimer( m_uiTransTimerID );
		   m_uiTransTimerID = SetTimer( NITF_TRANS_TIMER, 300, NULL );

#ifndef JMPS_VARIANT
	      if ( m_pNITFObj != NULL )
         {
            m_pNITFObj->m_trans_value = (int) ( 0.5 + ( m_trans_level * 2.55 ) );
		      m_pNITFObj->set_display_mode();
         }
#endif
      }
      UpdateData( FALSE );
   }

	CDialog::OnVScroll( nSBCode, nPos, pScrollBar );
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnAutoScreenContrast() 
{
	m_eEqualizationMode = EQUALIZATION_MODE_AUTO_LINEAR;

#ifndef JMPS_VARIANT
	if (m_pNITFObj == NULL)
		return;

	UpdateData(TRUE);
	
	m_pNITFObj->m_eq_mode = EQUALIZATION_MODE_AUTO_LINEAR;
	m_pNITFObj->set_display_mode();
#endif
	show_histogram_stuff();
	redraw_obj();
	Invalidate();

	UpdateData(FALSE);
}

// ****************************************************************
// ****************************************************************
#if 0
void CHistTransDlg::OnHistogramContrast() 
{
	UpdateData(TRUE);
   m_eEqualizationMode = EQUALIZATION_MODE_MANUAL_LINEAR;
#ifndef JMPS_VARIANT
	m_pNITFObj->m_eq_mode = (INT) EQUALIZATION_MODE_MANUAL_LINEAR;
	m_pNITFObj->set_display_mode();
#endif
	show_histogram_stuff();
	redraw_obj();

	UpdateData(FALSE);
}
#endif

// ****************************************************************
// ****************************************************************

void CHistTransDlg::show_histogram_stuff() 
{
   INT iShow = m_eEqualizationMode == EQUALIZATION_MODE_MANUAL_LINEAR ? SW_SHOW : SW_HIDE;
   GetDlgItem(IDC_MINIMUM)->ShowWindow(iShow);
   GetDlgItem(IDC_CENTER)->ShowWindow(iShow);
   GetDlgItem(IDC_MAXIMUM)->ShowWindow(iShow);
   GetDlgItem(IDC_HISTVIEW)->ShowWindow(iShow);
   GetDlgItem(IDC_BLACK_LABEL)->ShowWindow(iShow);
   GetDlgItem(IDC_CENTER_LABEL)->ShowWindow(iShow);
   GetDlgItem(IDC_WHITE_LABEL)->ShowWindow(iShow);
   GetDlgItem(ID_AUTO)->ShowWindow(iShow);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnEnhanceAuto() 
{
   m_eEqualizationMode = EQUALIZATION_MODE_AUTO_LINEAR;
#ifdef JMPS_VARIANT
   m_pNITFOverlayVPS->RefreshActiveView();
#else
   m_pNITFObj->m_eq_mode = (INT) EQUALIZATION_MODE_AUTO_LINEAR;
	m_pNITFObj->set_display_mode();
#endif
	show_histogram_stuff();
	redraw_obj();
	Invalidate();

	UpdateData(FALSE);
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnEnhanceHistogram() 
{
	UpdateData( TRUE );
	m_eEqualizationMode = EQUALIZATION_MODE_MANUAL_LINEAR;
	
#ifdef JMPS_VARIANT
   m_pNITFOverlayVPS->RefreshActiveView();
#else
	if ( m_pNITFObj != NULL )
   {
	   m_pNITFObj->m_eq_mode = (INT) EQUALIZATION_MODE_MANUAL_LINEAR;
	   m_pNITFObj->set_display_mode();
   }
#endif
   GetHistogram();
	show_histogram_stuff();
	redraw_obj();

	UpdateData( FALSE );
}


LRESULT CHistTransDlg::OnGetHistogram( WPARAM wParam, LPARAM lParam )
{
   if ( GetHistogram() )
      Invalidate( FALSE );
   return 0;
}


// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnEnhanceNone() 
{
	m_eEqualizationMode = EQUALIZATION_MODE_FIXED_LINEAR;
#ifdef JMPS_VARIANT
   m_pNITFOverlayVPS->RefreshActiveView();
#else
	if ( m_pNITFObj != NULL )
   {
	   m_pNITFObj->m_eq_mode = (INT) EQUALIZATION_MODE_FIXED_LINEAR;
	   m_pNITFObj->set_display_mode();
   }
#endif
	show_histogram_stuff();
	redraw_obj();

	UpdateData(FALSE);	
}

// ****************************************************************
// ****************************************************************

void CHistTransDlg::OnMsBands() 
{
	try
	{
      // Make sure ImageLib's display params are up-to-date
#ifndef JMPS_VARIANT
      if ( m_pNITFObj == NULL )
		   return;

      if ( !m_pNITFObj->m_pxdDisplayParams )
         return;

      m_bstrDisplayParamsXML = m_pNITFObj->m_pxdDisplayParams->xml;
#endif
      m_spImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_LOAD_DISPLAY_PARAMS_XML,
                        _variant_t( m_bstrDisplayParamsXML ), VARIANT_NULL,
                        NULL, NULL, NULL, NULL );

      INT iErrorCode;
	   CComBSTR ccbsErrorMsg;
		if ( S_OK == m_spImageLib->image_adjust_dialog( &iErrorCode, &ccbsErrorMsg ) )
         UpdateDisplayParamsXML();
	}
	catch(...)
	{
		AfxMessageBox( _T("Image Adjust Dialog call exception") );
	}

	redraw_obj();
	UpdateData(FALSE);	
}


// ****************************************************************
// ****************************************************************

VOID CHistTransDlg::redraw_obj()
{
#ifdef JMPS_VARIANT
   m_pNITFOverlayVPS->RefreshActiveView();
#else
   if ( m_pNITFObj != NULL )
   {
	   static_cast< C_nitf_ovl* >( m_pNITFObj->m_overlay )->m_bReload = FALSE;
	   m_pNITFObj->invalidate();
   }
#endif
}


// *************************************************************
// *************************************************************

void CHistTransDlg::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == NITF_TRANS_TIMER )
	{
		KillTimer( m_uiTransTimerID );
		redraw_obj();
	}
	
	CDialog::OnTimer(nIDEvent);
}


// *************************************************************
// *************************************************************

VOID CHistTransDlg::ShowHistogramProgress()
{
#ifdef IDC_HISTOGRAM_PROGRESS
   if ( m_cbImageLibCallback.m_dProgressPercent >= 0.0 )
   {
      GetDlgItem( IDC_HISTVIEW )->ShowWindow( SW_HIDE );
      GetDlgItem( IDC_HISTOGRAM_PROGRESS )->ShowWindow( SW_SHOWNA );
      CComBSTR ccbs;
      ccbs.LoadStringW( IDS_LOADING_HISTOGRAM );
      m_csHistogramProgress.Format( (LPCTSTR) ccbs, m_cbImageLibCallback.m_dProgressPercent );
      UpdateData( FALSE );
   }
   else
   {
      GetDlgItem( IDC_HISTVIEW )->ShowWindow( SW_SHOWNA );
      GetDlgItem( IDC_HISTOGRAM_PROGRESS )->ShowWindow( SW_HIDE );
   }
#endif

   MSG msg;
   while ( PeekMessage( &msg, m_hWnd, 0, 0, PM_REMOVE ) )
   {
      // Send the message through
      TranslateMessage( &msg );
      DispatchMessage( &msg );
   }
   SetCursor( m_hWaitCursor );      // Gets lost in the message handling
}


// *************************************************************
// *************************************************************

VOID CHistTransDlg::UpdateDisplayParamsXML()
{
   // Save modified parameters
   _variant_t var( VARIANT_NULL );
   m_spImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_LOAD_DISPLAY_PARAMS_XML,
      VARIANT_NULL, VARIANT_NULL, &var, NULL, NULL, NULL );
   if ( var.vt == VT_BSTR && var.bstrVal != NULL )
   {
#ifdef JMPS_VARIANT
      m_bstrDisplayParamsXML = var;
#else
      m_pNITFObj->m_pxdDisplayParams->loadXML( var.bstrVal );

      wstring wstrXML = (LPCWSTR) var.bstrVal;
      C_nitf_ovl::QuoteDBString( wstrXML );      // Surround with quote marks
      _bstr_t bstrSet = L"DisplayParamsXML = ";
      bstrSet += wstrXML.c_str();
      C_nitf_ovl::s_pNITFDBAgent->UpdateImageDisplayInfoRecord(
         _bstr_t( m_pNITFObj->m_csFilespec ), m_pNITFObj->m_iImageInFile,
         L"Generic", L"Generic",
         bstrSet,
         (long) FALSE );   // [in] long bNoNewRecord
#endif
   }  // String returned

}  // UpdateDisplayParamsXML()


// **************************************************************************
// **************************************************************************

STDMETHODIMP CHistTransDlg::ImageLibCallbackInterface::raw_imagelib_progress( DOUBLE dPercent, BSTR bsLabel )
{
#ifdef JMPS_VARIANT
	AFX_MANAGE_STATE(AfxGetModuleState())
#else
	AFX_MANAGE_STATE(AfxGetAppModuleState())
#endif

   if ( dPercent != m_dProgressPercent )
   {
      m_dProgressPercent = dPercent;       // For tracking
      m_pHistTransDlg->ShowHistogramProgress();
   }
   return S_OK;
}
// End of raw_imagelib_progress


// End of HistTransDlg.cpp
LRESULT CHistTransDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

