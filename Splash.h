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

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : header file
//
#include <gdiplus.h>
#include "dib.h"

// DEFINES
#define SPLASH_MESSAGE_TOP    191
#define SPLASH_MESSAGE_BOTTOM 221

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

class CFvSplashWndImpl;

class CSplashWnd : public CWnd
{
// Construction
protected:
   CSplashWnd();

// Attributes:
public:
   CDib m_bitmap;
   static FalconViewOverlayLib::IFvSplashWnd *GetFvSplashWnd();

// Operations
public:
   static void EnableSplashScreen(BOOL bEnable = TRUE);
   static void ShowSplashScreen(CWnd* pParentWnd = NULL);
   static CWnd *GetWnd();
   static void Message(char *chars);
   static BOOL SILENT;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSplashWnd)
   //}}AFX_VIRTUAL

// Implementation
public:
   ~CSplashWnd();
   virtual void PostNcDestroy();

protected:
   CComObject<CFvSplashWndImpl> *m_pFvSplashWnd;
   Gdiplus::Image* m_pImage;
   BOOL Create(CWnd* pParentWnd = NULL);
   void HideSplashScreen();
   static BOOL c_bShowSplashWnd;
   static CSplashWnd* c_pSplashWnd;
   static CString str;
   CFont m_font;
   CString m_strVersion;
   CRect m_RectMessage;

// Generated message map functions
protected:
   //{{AFX_MSG(CSplashWnd)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnPaint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

// Implements IFvFrameWnd defined in FalconViewOverlay.tlb
//
class CFvSplashWndImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvSplashWnd, &FalconViewOverlayLib::IID_IFvSplashWnd, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
public:
   CFvSplashWndImpl()
   {
   }

BEGIN_COM_MAP(CFvSplashWndImpl)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvSplashWnd)
END_COM_MAP()

// IFvSplashWnd

   STDMETHOD(put_MessageText)(BSTR messageText)
   {
      CSplashWnd::Message((char *)_bstr_t(messageText));
      return S_OK;
   }
};

#endif
