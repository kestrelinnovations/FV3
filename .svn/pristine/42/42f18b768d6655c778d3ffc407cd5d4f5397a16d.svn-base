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



//
// CStaticLink implements a static control that's a hyperlink
// to any file on your desktop or web. You can use it in dialog boxes
// to create hyperlinks to web sites. When clicked, opens the file/URL
//
//  Code is from the December 1997 MSJ in the C++ Q+A column.
//
#include "StdAfx.h"
#include "StatLink.h"
#include "..\resource.h"  // for IDC_MYFINGER

IMPLEMENT_DYNAMIC(UTL_StaticLink, CStatic)

BEGIN_MESSAGE_MAP(UTL_StaticLink, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
   ON_WM_SETCURSOR()
END_MESSAGE_MAP()

///////////////////
// Constructor sets default colors = blue/purple.
//
UTL_StaticLink::UTL_StaticLink()
{
	m_colorUnvisited = RGB(0,0,255);		 // blue
	m_colorVisited   = RGB(128,0,128);	 // purple
	m_bVisited       = FALSE;				 // not visited yet
}

//////////////////
// Handle reflected WM_CTLCOLOR to set custom control color.
// For a text control, use visited/unvisited colors and underline font.
// For non-text controls, do nothing. Also ensures SS_NOTIFY is on.
//
HBRUSH UTL_StaticLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
	ASSERT(nCtlColor == CTLCOLOR_STATIC);
	DWORD dwStyle = GetStyle();
	if (!(dwStyle & SS_NOTIFY)) {
		// Turn on notify flag to get mouse messages and STN_CLICKED.
		// Otherwise, I'll never get any mouse clicks!
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
	}
	
	HBRUSH hbr = NULL;
	if ((dwStyle & 0xFF) <= SS_RIGHT) 
   {

		// this is a text control: set up font and colors
		if (!(HFONT)m_font) 
      {
			// first time init: create font
			LOGFONT lf;
			GetFont()->GetObject(sizeof(lf), &lf);

			m_font.CreateFontIndirect(&lf);
		}

		// use underline font and visited/unvisited colors
		pDC->SelectObject(&m_font);
		pDC->SetTextColor(m_bVisited ? m_colorVisited : m_colorUnvisited);
		pDC->SetBkMode(TRANSPARENT);

		// return hollow brush to preserve parent background color
		hbr = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	}
	return hbr;
}

/////////////////
// Handle mouse click: open URL/file.
//
void UTL_StaticLink::OnClicked()
{
	if (m_link.IsEmpty())		// if URL/filename not set..
		GetWindowText(m_link);	// ..get it from window text

	// Call ShellExecute to run the file.
	// For an URL, this means opening it in the browser.
	//
	HINSTANCE h = ShellExecute(NULL, "open", m_link, NULL, NULL, SW_SHOWNORMAL);
	if ((UINT)h > 32) 
   {
		m_bVisited = TRUE;	// (not really--might not have found link)
		Invalidate();			// repaint to show visited color
	}
   else 
   {
		MessageBeep(0);		// unable to execute file!
	}
}

BOOL UTL_StaticLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT nMsg)
{
   //
   // Windows 95 and NT 4.0 does not have a built-in stock finger pointing symbol 
   // like Windows 98 and NT 5.0 (IDC_FINGER), so use a custom one here
   //
   HCURSOR cursor = AfxGetApp()->LoadCursor(IDC_MYFINGER);

   if (cursor)
   {
      //
      //make sure to call ::SetCursor here and not CStatic::SetCursor
      //
      ::SetCursor(cursor);

      return TRUE;
   }

   return FALSE;
}
