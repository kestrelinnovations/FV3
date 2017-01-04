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



// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file
//

#include "stdafx.h"  // e. g. stdafx.h
#include "resource.h"  // e.g. resource.h
#include <GdiPlus.h>

#include "Splash.h"  // e.g. splash.h
#include "appinfo.h"
#include "param.h"   // for PRM_
#include "err.h"     // for ERR_

using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd=NULL;
CString CSplashWnd::str="";
BOOL CSplashWnd::SILENT=TRUE;

CSplashWnd::CSplashWnd() : m_pFvSplashWnd(NULL)
{
}

CSplashWnd::~CSplashWnd()
{
   if (m_pFvSplashWnd != NULL)
      m_pFvSplashWnd->Release();

   if (SILENT) return;
   // Clear the static window pointer.
   ASSERT(c_pSplashWnd == this);
   c_pSplashWnd = NULL;
   delete m_pImage;
}

/* static */
FalconViewOverlayLib::IFvSplashWnd *CSplashWnd::GetFvSplashWnd()
{
   if (c_pSplashWnd == NULL)
      return NULL;

   if (c_pSplashWnd->m_pFvSplashWnd == NULL)
   {
      CComObject<CFvSplashWndImpl>::CreateInstance(&c_pSplashWnd->m_pFvSplashWnd);
      c_pSplashWnd->m_pFvSplashWnd->AddRef();
   }

   return c_pSplashWnd->m_pFvSplashWnd;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
   //{{AFX_MSG_MAP(CSplashWnd)
   ON_WM_CREATE()
   ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CWnd *CSplashWnd::GetWnd()
{
   if (SILENT) return NULL;
   else return c_pSplashWnd;
}

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
   if (SILENT) return;
   c_bShowSplashWnd = bEnable;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{
   if (SILENT) return;

   if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
      return;

   // Allocate a new splash screen, and create the window.
   c_pSplashWnd = new CSplashWnd;
   if (!c_pSplashWnd->Create(pParentWnd))
      delete c_pSplashWnd;
   else
      c_pSplashWnd->UpdateWindow();
}

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
   if (SILENT) 
      return FALSE;

   CString splash_path = PRM_get_registry_string("Main", "HD_DATA", "") + "\\res\\splash.png";
   m_pImage = Gdiplus::Image::FromFile((wchar_t *)_bstr_t(splash_path));
   if (m_pImage == NULL  || m_pImage->GetWidth() == 0) 
   {
      CString msg;
      msg.Format("The startup splash screen, %s, could not be found or is empty.", splash_path);
      ERR_report(msg);
      return FALSE;
   }

   m_font.CreatePointFont(100, "Arial");

   char appVersion[MAX_VERSION_SIZE];
   project_version_str(appVersion, MAX_VERSION_SIZE);
   m_strVersion = CString(appVersion);
  
   m_RectMessage.SetRectEmpty();

   return CreateEx(WS_EX_LAYERED | WS_EX_TRANSPARENT,
                   AfxRegisterWndClass(0, 
                   AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
                   NULL, WS_POPUP | WS_VISIBLE , 0, 0,
                   m_pImage->GetWidth(), m_pImage->GetHeight(), 
                   pParentWnd->GetSafeHwnd(), NULL);
}

void CSplashWnd::HideSplashScreen()
{
   if (SILENT) return;

   // Destroy the window, and update the mainframe.
   DestroyWindow();

   CWnd *MainWindow;

   MainWindow = AfxGetMainWnd();

   if (MainWindow)
      MainWindow->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
   if (SILENT) return;

   // Free the C++ class.
   delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (SILENT) return -1;

   if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   // Center the window.
   CenterWindow();
   //ModifyStyleEx(0, WS_EX_LAYERED );
   SetLayeredWindowAttributes(0, 255 , LWA_ALPHA);

   return 0;
}

void CSplashWnd::OnPaint()
{
   CRect wndrect;

   if (SILENT) return;

   CPaintDC dc(this);
   //draw the bitmap
   //m_bitmap.Draw(dc, NULL, NULL, FALSE, NULL, TRUE);

   if (m_pImage != NULL)
   {
      Graphics gc(dc.GetSafeHdc());
      // clear background to the colorkey used for transparency
      Color c(245,245,255);  
      
      // get away from gc.Clear() on every message change which causes a great 
      // deal of flickering of the splash screen, only clear if we need to 
      // redraw the entire splash image.
      CRect RectClip;
      RectClip.SetRectEmpty();
      dc.GetClipBox(&RectClip);
      if ( (RectClip.bottom != this->m_RectMessage.bottom) ||
           (RectClip.top != this->m_RectMessage.top) )
      {
         gc.Clear(c);
      }
      
      gc.DrawImage(m_pImage, 0, 0, m_pImage->GetWidth(), m_pImage->GetHeight());
      // tell windows that the wind is colorkeyed
      SetLayeredWindowAttributes(RGB(245,245,255), 255 , LWA_COLORKEY);
   }

   // Paint the text
   dc.SetBkMode(TRANSPARENT);
   dc.SetTextAlign(TA_CENTER);
   COLORREF oldcolor = dc.SetBkColor(RGB(0, 0, 0));
   COLORREF oldTextColor = dc.SetTextColor(RGB(255, 255, 255));

   CFont *pOldFont = dc.SelectObject(&m_font);
   
   GetWnd()->GetClientRect(&wndrect);
   dc.TextOut((wndrect.right / 2), SPLASH_MESSAGE_TOP, CSplashWnd::str);
   m_RectMessage.SetRect((wndrect.right / 2), SPLASH_MESSAGE_TOP, 
                         wndrect.right, SPLASH_MESSAGE_BOTTOM);

   dc.TextOut(100, 248, m_strVersion);
   dc.SelectObject(pOldFont);

   dc.SetBkColor(oldcolor);      //restore old color
   dc.SetTextColor(oldTextColor);
}

void CSplashWnd::Message(char *chars)
{
   if (!SILENT)
   {
      CSplashWnd::str = chars;
      // Invalidate only area where we are drawing the Message, not whole image
      CWnd *pWnd = GetWnd();
      CRect Rect;
      Rect.SetRectEmpty();
      pWnd->GetWindowRect(&Rect);
      CRect RectText(0, SPLASH_MESSAGE_TOP, Rect.Width(), SPLASH_MESSAGE_BOTTOM);
      pWnd->InvalidateRect(&RectText);
      pWnd->UpdateWindow();      
   }
}
