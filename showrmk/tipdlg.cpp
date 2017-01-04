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



// toolbarx.cpp : implementation of the CToolBarEx, CTipDialog, and CFrame classes
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// NOTE: MFC Undocumented feature usage
//
//  This code relies on internal knowledge of the class and structures
//  used to implement the MFC 2.0/2.5 CToolBar, and CFrame classes.
//
//  It relies on the following undocumented member functions from CToolBar
//    CToolBar::HitTest()
//
//  It uses the following undocumented messages and defines from afxpriv.h
//    WM_SETMESSAGESTRING
//    AFX_IDS_IDLEMESSAGE
//
//  This code is compatible with:
//    - MFC verions 2.0 and 2.5 for Win16 and Win32
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "tipdlg.h"
#include "utils.h"



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CTipDialog

// static members
CString near CTipDialog::m_strClassName = "";
CFont near CTipDialog::m_fntFont;
int near CTipDialog::m_nBubbleHeight = 0;
static BOOL tip_active = FALSE;

IMPLEMENT_DYNAMIC(CTipDialog, CWnd);

// ****************************************************************

CTipDialog::CTipDialog(CWnd *wnd)
{
   // set initial mode
   //  m_bBubbleOn = FALSE;
   
   m_pParent = wnd;
   
   tip_active = CreateTip();
   
   // set creation flag
   //  m_bCreated = FALSE;
}

// ****************************************************************

CTipDialog::~CTipDialog()
{
   if (m_fntFont.GetSafeHandle() != NULL)
      m_fntFont.DeleteObject();
}

//-----------------------------------------------------------------------------
void CTipDialog::GetTextRect( CPoint pt, CString strText, CRect& textRect )
{
   CPaintDC dc(this); // device context for retrieving text extents
   VERIFY(dc.SaveDC());
   VERIFY(dc.SelectObject(&m_fntFont));

   // remove any \r characters in the given string
   strText.Replace("\r", "");

   // insure there is a newline character at the end of the string
   // to avoid special cases in following loop
   if (strText.GetLength() && strText[strText.GetLength() - 1] != '\n')
      strText += '\n'; 

   // The following loop will i) populate m_saTextLines with newline separated
   // strings, and ii) compute maximum width and height, in pixels, across all
   // strings
   m_nMaxStringHeight = 0;
   int max_string_length = 0;
   int current_pos = 0;
   int pos = 0;
   while ((pos = strText.Find('\n', current_pos)) != -1)
   {
      CString line = strText.Mid(current_pos, pos - current_pos);
      m_saTextLines.Add(line);

      CSize stringSize = dc.GetTextExtent(line);
      
      if (stringSize.cy > m_nMaxStringHeight)
         m_nMaxStringHeight = stringSize.cy;

      if (stringSize.cx > max_string_length)
         max_string_length = stringSize.cx;

      current_pos = pos + 1;
   }

   // Resize the tip window based on the tip's text
   textRect.top = pt.y - (6*GetSystemMetrics(SM_CXBORDER));
   textRect.left = pt.x - (6*GetSystemMetrics(SM_CXBORDER));
   textRect.bottom = textRect.top + (m_nMaxStringHeight*m_saTextLines.GetSize()) + (6*GetSystemMetrics(SM_CYBORDER));
   textRect.right = textRect.left + max_string_length + (6*GetSystemMetrics(SM_CXBORDER));

   VERIFY(dc.RestoreDC(-1));
}

// ****************************************************************

BOOL CTipDialog::CreateTip()
{
   // create our bubble window but leave it invisible
   
   // do we need to register the class?
   if (m_strClassName == "")
   {
      // yes
      
      // first, create the background brush
      CBrush  brBrush;
      TRY
      {
         VERIFY(brBrush.CreateSolidBrush(GetSysColor(COLOR_INFOBK)));
         
         // BOUNDS_CHECKER_SAYS  resource leak, so added this delete object...
         // It seems that stack should unwind and the brBrush destructor should call
         // DeleteObject, but maybe not
         brBrush.DeleteObject();
      }
      CATCH(CResourceException, e)
      {
         // brush creation failed
         return 0;
      }
      END_CATCH
         
      // register the class name
      m_strClassName = ::AfxRegisterWndClass(
         CS_BYTEALIGNCLIENT | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW, 
         0 ,(HBRUSH)brBrush.Detach());

      brBrush.DeleteObject();

      // we're we successful?
      if (m_strClassName == "")
         return 0;  // return failed
   }
   
   // create the bubble window and set the created flag
   CRect rect;
   rect.SetRectEmpty();
   m_bCreated = CreateEx(0, m_strClassName, "", 
      WS_EX_TOOLWINDOW | WS_POPUP | WS_BORDER,
      rect.left, rect.top, rect.right, rect.bottom, 
      m_pParent->GetSafeHwnd(), (HMENU)NULL);
   
   // return status
   return m_bCreated;
}
// end Create

// ****************************************************************

BOOL CTipDialog::DestroyWindow()
{
   // clear our creation flag
   m_bCreated = FALSE;
   
   // call the base to destroy the window
   return CWnd::DestroyWindow();
}

// ****************************************************************

/* static */
BOOL CTipDialog::is_active()
{
   return tip_active;
}
// end of is_active

// ****************************************************************

void CTipDialog::EndTip()
{
   if (!tip_active)
      return;

   tip_active = FALSE;
   
   // if there is a window attached to this CWnd object
   if (GetSafeHwnd())
   {
      ShowWindow(SW_HIDE); // set its show state to SW_HIDE
      UpdateWindow();      // make it take affect immediately
      DestroyWindow();     // destroy the attached window
   }
   
   delete this;
}
// end of EndTip

// ****************************************************************

// void CTipDialog::Track(CPoint point, const CString& string, int nID)
void CTipDialog::ShowTip(int x, int y, const char* string)
{
   POINT   pt;
   CRect   vrect;
   RECT   rc;
   int   scrmaxx, scrmaxy;
   int   wx, wy;
   int   fvw_width;
   HWND   hwnd;


   // create the font
   
   CClientDC dc(this);  // used to calculate the font size
   TRY 
   {
      // get the system non-client metrics parameters, whose member
      // lfStatusFont stipulates the system setting for Tooltip fonts.
      NONCLIENTMETRICS nc_metrics;
      memset(&nc_metrics, 0, sizeof(NONCLIENTMETRICS));
      nc_metrics.cbSize = sizeof(NONCLIENTMETRICS);
      SystemParametersInfo(SPI_GETNONCLIENTMETRICS, NULL, &nc_metrics, NULL);

      // create a font based upon those metrics
      if (m_fntFont.GetSafeHandle() != NULL)
         VERIFY(m_fntFont.DeleteObject() == TRUE);

      VERIFY(m_fntFont.CreateFontIndirect(&nc_metrics.lfStatusFont));
   }
   CATCH(CResourceException, e)
   {
      // font creation failed
      return;
   }
   END_CATCH
      
   // select the font we're using
   VERIFY(dc.SelectObject(&m_fntFont));

   // get the text metrics
   TEXTMETRIC tm;
   VERIFY(dc.GetTextMetrics(&tm) == TRUE);
   
   // calc bubble window height now
   m_nBubbleHeight = tm.tmHeight + tm.tmExternalLeading + 
      (6*GetSystemMetrics(SM_CYBORDER));

   if (UTL_os_is_multiple_monitor_enabled())
   {
      scrmaxx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
      scrmaxy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
   }
   else
   {
      scrmaxx = GetSystemMetrics(SM_CXSCREEN);
      scrmaxy = GetSystemMetrics(SM_CYSCREEN);
   }
   
   hwnd = m_pParent->GetSafeHwnd();
   ::GetWindowRect(hwnd, &rc);
   fvw_width = rc.right - rc.left + 1;

   pt.x = x;
   pt.y = y;

   CRect rect;
   GetWindowRect(rect);  // get current size and position
   GetTextRect(pt, string, rect );
   
   m_x = x + rc.left;
   m_y = y + rc.top;
   
   wx = rect.right - rect.left + 1;
   wy = rect.bottom - rect.top + 1;
   
   if ((m_x + wx) > scrmaxx)
      m_x -= (m_x + wx) - scrmaxx;
   if ((m_y + wy) > scrmaxy)
      m_y -= (m_y + wy) - scrmaxy;
   
   if ((m_x + wx) > rc.right)
      m_x -= (m_x + wx) - rc.right;
   if (m_x < rc.left)
      m_x = rc.left;
   if ((m_y + wy) > rc.bottom)
      m_y -= (m_y + wy) - rc.bottom;
   
   if (rect.right > fvw_width)
   {
      int shift_left = rect.right - fvw_width;
      rect.left -= shift_left;
      rect.right -= shift_left;
   }

   // set window size
   MoveWindow(&rect);
   
   // hide it first so it moves cleanly
   ShowWindow(SW_HIDE);
   UpdateWindow();  // update now
   
   // set the text
   //SetWindowText(string);
   
   // if the x-position is negative we must center the bubble window
   // around the absolute value of the position specified.
   if (x < 0)
   {
      CRect rect;
      GetWindowRect(&rect);
      // calc the center point
      x = abs(x)-(rect.Width()/2);
   }
   
   // move the window
   SetWindowPos(&wndTop, m_x, m_y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
   
   // show the window but don't take focus
   ShowWindow(SW_SHOWNA);
   UpdateWindow();  // update now
}
// end of ShowTip

// ****************************************************************

// ****************************************************************

void CTipDialog::HideTip()
{
   // hide the bubble window
   ShowWindow(SW_HIDE); 
}

// ****************************************************************

void CTipDialog::ReShowTip()
{
   if (!tip_active)
      return;
   
   ShowWindow(SW_SHOWNA);
}

// ****************************************************************




BEGIN_MESSAGE_MAP(CTipDialog, CWnd)
//{{AFX_MSG_MAP(CTipDialog)
ON_WM_PAINT()
   ON_WM_ERASEBKGND()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTipDialog message handlers

// ****************************************************************

void CTipDialog::OnPaint()
{
   CPaintDC dc(this); // device context for painting
   VERIFY(dc.SaveDC());

   // select our font and setup for text painting
   VERIFY(dc.SelectObject(&m_fntFont));
   dc.SetBkMode(TRANSPARENT);
   dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));

   int x = (2*GetSystemMetrics(SM_CXBORDER));
   int y = (2*GetSystemMetrics(SM_CYBORDER));
   
   for ( int i = 0; i < this->m_saTextLines.GetSize(); i++ )
   {
      CString sText = this->m_saTextLines.GetAt(i);
      dc.TextOut(x,y+(this->m_nMaxStringHeight*i),sText);
   }

   VERIFY(dc.RestoreDC(-1));
}

// end of OnPaint

// ****************************************************************

BOOL CTipDialog::OnEraseBkgnd(CDC* pDC) 
{
   // TODO: Add your message handler code here and/or call default
   
   // Set brush to desired background color
   CBrush backBrush(GetSysColor(COLOR_INFOBK));
   
   // save the DC state
   VERIFY(pDC->SaveDC());

   // Save old brush
   VERIFY(pDC->SelectObject(&backBrush));
   
   // erase the background area
   CRect rect;
   VERIFY(pDC->GetClipBox(&rect) != ERROR);
   
   pDC->PatBlt(rect.left, rect.top, rect.Width(),
      rect.Height(), PATCOPY);

   VERIFY(pDC->RestoreDC(-1));
   VERIFY(backBrush.DeleteObject() == TRUE);

   return TRUE;
}

// ****************************************************************
// ****************************************************************
// ****************************************************************
