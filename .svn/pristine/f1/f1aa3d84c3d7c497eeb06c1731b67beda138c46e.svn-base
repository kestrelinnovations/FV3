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

// CTIA_Ruler.cpp : implementation file
//

#include "stdafx.h"
#include "fvw.h"
#include "CTIA_Ruler.h"
#include "param.h"
#include "getobjpr.h"      // for UTL_get_active_non_printing_view
#include "mapx.h"
#include "getobjpr.h"
#include "utils.h"
#include "MapEngineCOM.h"

// CCTIA_Ruler dialog

IMPLEMENT_DYNAMIC(CCTIA_Ruler, CDialog)

   CCTIA_Ruler::CCTIA_Ruler(CWnd* pParent /*=NULL*/)
   : CDialog(CCTIA_Ruler::IDD, pParent),
   m_nRulerResizerPos(0), m_fPPIPerSliderPos(-1)
{

}

CCTIA_Ruler::~CCTIA_Ruler()
{
}

void CCTIA_Ruler::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_CTIA_SLIDER, m_RulerResizerCtrl);
   DDX_Control(pDX, IDC_CTIA_PPI, m_PPI);
}

BOOL CCTIA_Ruler::OnInitDialog()
{
   CDialog::OnInitDialog();

   //get registry settings
   m_RULER_END_POS = PRM_get_registry_int("CTIA", "NumPos", 4000 );
   m_MAX_PPI = PRM_get_registry_double("CTIA", "MaxPPI", 192.0 );
   double fPPI = PRM_get_registry_double("CTIA", "PPI", 96.0 );

   m_RulerResizerCtrl.SetRange(0,m_RULER_END_POS);

   m_sPPI.Format("%.03f", fPPI );
   m_fPPIPerSliderPos = ( m_MAX_PPI / m_RULER_END_POS );
   m_nRulerResizerPos = (LONG)( fPPI / m_fPPIPerSliderPos );
   m_RulerResizerCtrl.SetPos( m_nRulerResizerPos );
   INT nDialogRight = (INT)(8.50 * fPPI);

   GetWindowRect(&this->m_DialogBounds);
   SetWindowPos( NULL, m_DialogBounds.left, m_DialogBounds.top, 
      nDialogRight, m_DialogBounds.Height(), SWP_NOACTIVATE|SWP_NOMOVE );

   DEVMODE dm;
   // initialize the DEVMODE structure
   ZeroMemory(&dm, sizeof(dm));
   dm.dmSize = sizeof(dm);

   if (0 != EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
   {
      // inspect the DEVMODE structure to obtain details
      // about the display settings such as
      //  - Orientation
      //  - Width and Height
      //  - Frequency
      //  - etc.

      // this->m_nScreenPixelWidth = dm.dmPelsWidth;
      // this->m_nScreenPixelHeight = dm.dmPelsHeight;

      UpdateData(FALSE);
   }

   Invalidate();

   return TRUE;  // return TRUE unless you set the focus to a control
}


BEGIN_MESSAGE_MAP(CCTIA_Ruler, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_WM_ERASEBKGND()
   ON_WM_NCHITTEST()
   ON_WM_HSCROLL()
   ON_WM_CTLCOLOR()
   ON_BN_CLICKED(IDC_CTIA_CLOSE, &CCTIA_Ruler::OnBnClickedCtiaClose)
   ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCTIA_Ruler message handlers
//-----------------------------------------------------------------------------
BOOL CCTIA_Ruler::OnEraseBkgnd(CDC* pDC)
{
   // Set brush to desired background color
   CBrush backBrush(RGB(238,221,130));

   // save the DC state
   VERIFY(pDC->SaveDC());

   // Save old brush
   VERIFY(pDC->SelectObject(&backBrush));

   // erase the background area
   CRect rect;
   VERIFY(pDC->GetClipBox(&rect) != ERROR);

   pDC->PatBlt(rect.left, rect.top, rect.Width(),
      rect.Height(), PATCOPY);

   pDC->SetBkMode(TRANSPARENT);

   // 34 quarter inch segments = 8.5 inches
   GetClientRect(&rect);
   float segment = rect.Width() / 34.0f;
   for ( int i = 0; i < 34; i++ ) 
   {
      int left = (int)( i * segment );
      int top = rect.bottom - 15; 
      int right = left + (int)segment;
      int bottom = rect.bottom; 

      if (  i > 0 && ( i % 4 ) == 0 )
      {
         CString s; 
         s.Format("%d", i / 4 );
         pDC->TextOut( left, top - 15, s );
         pDC->MoveTo( left, bottom );
         pDC->LineTo( left, bottom - 15 );
      }
      else if ( i > 0 && ( i % 2 ) == 0 ) 
      {
         // half inch marks
         pDC->MoveTo( left, bottom );
         pDC->LineTo( left, bottom - 10 );
      }
      else 
      { 
         // quarter inch marks
         pDC->MoveTo( left, bottom );
         pDC->LineTo( left, bottom - 5 );
      }
   }

   this->m_PPI.GetWindowRect(&rect);
   ScreenToClient(&rect);
   pDC->TextOut( rect.left, rect.top + (rect.Height()/2), CString("PPI:") + m_sPPI );

   VERIFY(pDC->RestoreDC(-1));
   VERIFY(backBrush.DeleteObject() == TRUE);

   // return CDialog::OnEraseBkgnd(pDC);
   return TRUE;
}

//-----------------------------------------------------------------------------
LRESULT CCTIA_Ruler::OnNcHitTest(CPoint pt)
{
   CRect rc;
   GetClientRect(&rc);
   ClientToScreen(&rc);
   return rc.PtInRect(pt) ? HTCAPTION : CDialog::OnNcHitTest(pt);
}

//-----------------------------------------------------------------------------
void CCTIA_Ruler::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   CSliderCtrl* pSlider =  (CSliderCtrl*)pScrollBar;
   if ( pSlider == (CSliderCtrl*) GetDlgItem( IDC_CTIA_SLIDER ) )
   {
      INT nRulerResizerPos = this->m_RulerResizerCtrl.GetPos();
      DOUBLE fPPI = nRulerResizerPos * this->m_fPPIPerSliderPos;
      INT nDialogRight = (INT)(8.50 * fPPI);

      CRect rect;
      GetWindowRect(&rect);
      if ( nDialogRight > 70 && fPPI > 16.0 ) 
      {
         m_nRulerResizerPos = nRulerResizerPos;
         m_sPPI.Format("%.03f", fPPI );
         //save registry settings
         PRM_set_registry_double("CTIA", "PPI", fPPI );

         SetWindowPos( NULL, m_DialogBounds.left, m_DialogBounds.top, 
            nDialogRight, m_DialogBounds.Height(), SWP_NOACTIVATE|SWP_NOMOVE );
      }
      else // set it back
      {
         this->m_RulerResizerCtrl.SetPos( m_nRulerResizerPos );
      }

      // set the pixels per inch in the rendering engine
      //now 
      MapView *pMapView = static_cast<MapView *>(UTL_get_active_non_printing_view());
      if (pMapView != NULL)
      {
         MapEngineCOM *pMapEngine = pMapView->get_map_engine();
         if (pMapEngine != NULL)
         {
            if (pMapEngine->SetPixelsPerInchForToScaleZoom(fPPI) == SUCCESS && 
               pMapEngine->get_curr_map()->requested_zoom_percent() == TO_SCALE_ZOOM_PERCENT)
            {
               pMapEngine->SetCurrentMapInvalid();
               pMapView->invalidate_view();
            }
         }
      }

      Invalidate();
   }

   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//-----------------------------------------------------------------------------
HBRUSH CCTIA_Ruler::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   // if( pWnd->m_hWnd== GetDlgItem(IDC_TEXT)->m_hWnd)  //Use static ID of your static text control
   {
      // pDC->SetBkMode(TRANSPARENT);
      pDC->SetBkColor( RGB(238,221,130) );  //red background
      // return hbr;
   }

   return hbr;
}

//-----------------------------------------------------------------------------
void CCTIA_Ruler::OnBnClickedCtiaClose()
{
   OnOK();
}

//-----------------------------------------------------------------------------
void CCTIA_Ruler::OnClose()
{
   OnOK(); // CDialog::OnClose();
}
LRESULT CCTIA_Ruler::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

