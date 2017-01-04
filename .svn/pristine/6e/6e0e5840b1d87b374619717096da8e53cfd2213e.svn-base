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
// StatLink.h

#pragma once

// CStaticLink implements a static control that's a hyperlink
// to any file on your desktop or web. You can use it in dialog boxes
// to create hyperlinks to web sites. When clicked, opens the file/URL
//
// Code is from the December 1997 and March 1998 MSJ in the C++ Q+A column.
//
class UTL_StaticLink : public CStatic 
{
public:
	UTL_StaticLink();

	// you can change these any time:
	COLORREF	 m_colorUnvisited;		 // color for unvisited
	COLORREF	 m_colorVisited;			 // color for visited
	BOOL		 m_bVisited;				 // whether visited or not

	// URL/filename for non-text controls (eg icon, bitmap) or when link is
	// different from window text. If you don't set this, CStaticIcon will
	// use GetWindowText to get the link.
	CString	 m_link;

protected:
	DECLARE_DYNAMIC(UTL_StaticLink)
	CFont		 m_font;						 // underline font for text control

	// message handlers
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void	OnClicked();
	DECLARE_MESSAGE_MAP()
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};