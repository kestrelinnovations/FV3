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



// ScrAdjst.cpp : implementation file
//

#include "stdafx.h"
#include "scradjst.h"
#include "getobjpr.h"
#include "MapEngineCOM.h"

static SYSTEMTIME old_time, new_time;
static int old_seconds, new_seconds, cnt_seconds;
static int old_bright, old_contrast;

/////////////////////////////////////////////////////////////////////////////
// CScreenAdjust dialog


CScreenAdjust::CScreenAdjust(CWnd* pParent /*=NULL*/)
   : CDialog(CScreenAdjust::IDD, pParent)
{
   //{{AFX_DATA_INIT(CScreenAdjust)
   m_contrast_val = _T("");
   m_bright_val = _T("");
   //}}AFX_DATA_INIT
}


void CScreenAdjust::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CScreenAdjust)
   DDX_Control(pDX, IDC_SLIDER_CONTRAST, m_slide_contrast);
   DDX_Control(pDX, IDC_SLIDER_BRIGHT, m_slide_bright);
   DDX_Text(pDX, IDC_CONTRAST_EDIT, m_contrast_val);
   DDX_Text(pDX, IDC_BRIGHT_EDIT, m_bright_val);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreenAdjust, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CScreenAdjust)
   ON_WM_VSCROLL()
   ON_BN_CLICKED(IDC_NORMAL_BRIGHT, OnNormalBright)
   ON_EN_KILLFOCUS(IDC_BRIGHT_EDIT, OnKillfocusBrightEdit)
   ON_EN_KILLFOCUS(IDC_CONTRAST_EDIT, OnKillfocusContrastEdit)
   ON_BN_CLICKED(IDC_AUTO, OnAuto)
   ON_WM_TIMER()
   ON_WM_LBUTTONUP()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenAdjust message handlers

void CScreenAdjust::set_time(int msec)
{
   cnt_seconds = msec;
   GetSystemTime(&old_time);
   old_seconds = (old_time.wMinute * 60000) + (old_time.wSecond * 1000) + old_time.wMilliseconds;
}

BOOL CScreenAdjust::time_expired()
{
   GetSystemTime(&new_time);
   new_seconds = (new_time.wMinute * 60000) + (new_time.wSecond * 1000) + new_time.wMilliseconds;
   if(new_seconds > (old_seconds + cnt_seconds))
      return TRUE;
   else
      return FALSE;
}

int CScreenAdjust::limit(int num, int min, int max)
{
   int tnum;

   tnum = num;
   if (tnum < min)
      tnum = min;
   if (tnum > max)
      tnum = max;
   return tnum;
}

void CScreenAdjust::wait(int time)  // time in milliseconds
{
   BOOL notdone;

   GetSystemTime(&old_time);
   old_seconds = (old_time.wSecond * 1000) + old_time.wMilliseconds;

   notdone = TRUE;
   while (notdone)
   {
      GetSystemTime(&new_time);
      new_seconds = (new_time.wSecond * 1000) + new_time.wMilliseconds;
      // check for overflow
      if (new_seconds < old_seconds)
         return;
      if ((old_seconds + time) < new_seconds)
         return;
   }
}

void CScreenAdjust::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   BOOL moved = TRUE;
   CSliderCtrl *pslider;

   UpdateData(TRUE);

   pslider = (CSliderCtrl*) pScrollBar;

   switch(nSBCode)
   {
   case TB_THUMBPOSITION:
   case TB_THUMBTRACK:
      pslider->SetPos(nPos);
      break;
   }

   if (pslider == &m_slide_bright) 
   {
      m_nBright = 100 - m_slide_bright.GetPos();
      m_brightness = (double) m_nBright / 100.0;
      m_mapview->SetContrast(m_brightness, m_contrast, m_nMidval);
   }
   else if (pslider == &m_slide_contrast) 
   {
      m_nContrast = 100 - m_slide_contrast.GetPos();
      m_contrast = (double) m_nContrast / 100.0;
      m_mapview->SetContrast(m_brightness, m_contrast, m_nMidval);
   }

   m_bright_val.Format("%d", m_nBright);
   m_contrast_val.Format("%d", m_nContrast);

   if ((m_nBright != old_bright) || (m_nContrast != old_contrast))
      m_changed = TRUE;
   old_bright = m_nBright;
   old_contrast = m_nContrast;

   if (m_changed)
   {
      fvw_get_view()->set_current_map_invalid();
      fvw_get_view()->invalidate_view(FALSE);
      m_changed = FALSE;
   }

   UpdateData(FALSE);

   CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
// end of OnVScroll

BOOL CScreenAdjust::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_nBright = (int) (m_brightness * 100.0);
   m_nContrast = (int) (m_contrast * 100.0);

   m_slide_bright.SetRange(0, 200);
   m_slide_bright.SetPageSize(10);
   m_slide_bright.SetLineSize(3);
   m_slide_contrast.SetRange(0, 200);
   m_slide_contrast.SetPageSize(10);
   m_slide_contrast.SetLineSize(3);
   m_slide_bright.SetPos(-m_nBright + 100);
   m_slide_contrast.SetPos(-m_nContrast + 100);

   m_bright_val.Format("%d", m_nBright);
   m_contrast_val.Format("%d", m_nContrast);
   UpdateData(FALSE);

   old_bright = m_nBright;
   old_contrast = m_nContrast;
   m_changed = FALSE;

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CScreenAdjust::OnCancel() 
{
   CDialog::OnCancel();
}

// actually set everything to normal
void CScreenAdjust::OnNormalBright() 
{
   m_mapview->SetContrast(0.0, 0.0, 35);

   m_mapview->set_current_map_invalid();
   m_mapview->invalidate_view(FALSE);
   m_nMidval = 35;
   m_contrast = 0.0;
   m_nContrast = 0;
   m_slide_contrast.SetPos(100);
   m_contrast_val = "0";
   m_brightness = 0.0;
   m_nBright = 0;
   m_slide_bright.SetPos(100);
   m_bright_val = "0";
   UpdateData(FALSE);
}

void CScreenAdjust::OnKillfocusBrightEdit() 
{
   int tval;

   UpdateData(TRUE);
   tval = atoi(m_bright_val);
   if ((tval > 100) || (tval < -100))
   {
      AfxMessageBox("Valid values for Brightness are -100 thru 100");
      m_bright_val.Format("%d", m_nBright);
      UpdateData(FALSE);
      GetDlgItem(IDC_BRIGHT_EDIT)->SetFocus();
      SendDlgItemMessage(IDC_BRIGHT_EDIT, EM_SETSEL, (WPARAM) 0, (LPARAM) -1);
   }
   else
   {
      m_nBright = tval;
      m_brightness = (double) m_nBright / 100.0;
      m_mapview->SetContrast(m_brightness, m_contrast, m_nMidval);
      fvw_get_view()->set_current_map_invalid();
      fvw_get_view()->invalidate_view(FALSE);
      m_slide_bright.SetPos(-m_nBright + 100);
      UpdateData(FALSE);
   }
}

void CScreenAdjust::OnKillfocusContrastEdit() 
{
   int tval;

   UpdateData(TRUE);
   tval = atoi(m_contrast_val);
   if ((tval > 100) || (tval < -100))
   {
      AfxMessageBox("Valid values for Contrast are -100 thru 100");
      m_contrast_val.Format("%d", m_nContrast);
      UpdateData(FALSE);
      GetDlgItem(IDC_CONTRAST_EDIT)->SetFocus();
      SendDlgItemMessage(IDC_CONTRAST_EDIT, EM_SETSEL, (WPARAM) 0, (LPARAM) -1);
   }
   else
   {
      m_nContrast = tval;
      m_contrast = (double) m_nContrast / 100.0;
      m_mapview->SetContrast(m_brightness, m_contrast, m_nMidval);
      fvw_get_view()->set_current_map_invalid();
      fvw_get_view()->invalidate_view(FALSE);
      m_slide_contrast.SetPos(-m_nContrast + 100);
      UpdateData(FALSE);
   }
}

void CScreenAdjust::OnAuto() 
{
   m_mapview->AutoBrightContrast();

   const MapDisplayParams& display_params = m_mapview->GetMapDisplayParams();

   m_brightness = display_params.bw_brightness;
   m_contrast = display_params.bw_contrast;
   m_nMidval = display_params.bw_contrast_midval;

   m_nBright = (int) (m_brightness * 100.0);
   m_nContrast = (int) (m_contrast * 100.0);
   m_slide_bright.SetPos(-m_nBright + 100);
   m_slide_contrast.SetPos(-m_nContrast + 100);

   m_bright_val.Format("%d", m_nBright);
   m_contrast_val.Format("%d", m_nContrast);
   UpdateData(FALSE);
   Invalidate();
}

void CScreenAdjust::OnTimer(UINT nIDEvent) 
{
}

void CScreenAdjust::OnLButtonUp(UINT nFlags, CPoint point) 
{
   CDialog::OnLButtonUp(nFlags, point);
}

LRESULT CScreenAdjust::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

