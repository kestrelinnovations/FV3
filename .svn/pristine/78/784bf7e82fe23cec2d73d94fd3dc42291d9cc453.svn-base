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

////////////////////////////////////////////////////////////////////////
// DlgResizeHelper
//	
// Author: Stephan Keil (Stephan.Keil@gmx.de)
// Date:   2000-06-26
//
// Helps you with keeping dialog layout on resizing.
// It automatically collects all child windows by calling Init() (you
// can also explicitly add other windows by calling Add()) and resizes
// them in OnResize(). Default resizing is proportional to the parent
// window but you can change that behaviour for some or all child windows
// by calling the various Fix() members.
// 
//

#ifndef DLGRESIZEHELPER_H_
#define DLGRESIZEHELPER_H_

#pragma warning (disable: 4786)
#include <list>

class DlgResizeHelper
{
public:

  // fix horizontal dimension/position
  enum EHFix {
    kNoHFix     = 0,
    kWidth      = 1,
    kLeft       = 2,
    kRight      = 4,
    kWidthLeft  = 3,
    kWidthRight = 5,
    kLeftRight  = 6
  };

  // fix vertical dimension/position
  enum EVFix {
    kNoVFix       = 0,
    kHeight       = 1,
    kTop          = 2,
    kBottom       = 4,
    kHeightTop    = 3,
    kHeightBottom = 5,
    kTopBottom    = 6
  };

  // initialize with parent window, all child windows must already have
  // their original position/size
  void Init(HWND a_hParent);

  // explicitly add a window to the list of child windows (e.g. a sibling window)
  // Note: you've got to call Init() before you can add a window
  void Add(HWND a_hWnd);

  // fix position/dimension for a child window, determine child by...
  // ...HWND...
  BOOL Fix(HWND a_hCtrl, EHFix a_hFix, EVFix a_vFix);
  // ...item ID (if it's a dialog item)...
  BOOL Fix(int a_itemId, EHFix a_hFix, EVFix a_vFix);
  // ...all child windows with a common class name (e.g. "Edit")
  UINT Fix(LPCTSTR a_pszClassName, EHFix a_hFix, EVFix a_vFix);
  // ...or all registered windows
  BOOL Fix(EHFix a_hFix, EVFix a_vFix);

  // resize child windows according to changes of parent window and fix attributes
  void OnSize();
private:
  struct CtrlSize {
    CRect m_origSize;
    HWND  m_hCtrl;
    EHFix  m_hFix;
    EVFix  m_vFix;
    CtrlSize() : m_hFix(kNoHFix), m_vFix(kNoVFix) {
    }
  };
  typedef std::list<CtrlSize> CtrlCont_t;
  CtrlCont_t m_ctrls;
  HWND       m_hParent;
  CRect      m_origParentSize;
};

#endif // DLGRESIZEHELPER_H_
