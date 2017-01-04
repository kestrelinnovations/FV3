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



// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "button.h"

/////////////////////////////////////////////////////////////////////////////
// CFVWColorButton

CFVWColorButton::CFVWColorButton()
{
}

CFVWColorButton::~CFVWColorButton()
{
}


BEGIN_MESSAGE_MAP(CFVWColorButton, CButton)
	//{{AFX_MSG_MAP(CFVWColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFVWColorButton message handlers

void CFVWColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
/*
itemAction   Defines the drawing action required. This will be one or more of the following bits:
ODA_DRAWENTIRE   This bit is set when the entire control needs to be drawn.
ODA_FOCUS   This bit is set when the control gains or loses input focus. The itemState member should be checked to determine whether the control has focus.
ODA_SELECT   This bit is set when only the selection status has changed. The itemState member should be checked to determine the new selection state.

itemState   Specifies the visual state of the item after the current drawing action takes place. That is, if a menu item is to be dimmed, the state flag ODS_GRAYED will be set. The state flags are as follows:
ODS_CHECKED   This bit is set if the menu item is to be checked. This bit is used only in a menu.
ODS_DISABLED   This bit is set if the item is to be drawn as disabled.
ODS_FOCUS   This bit is set if the item has input focus.
ODS_GRAYED   This bit is set if the item is to be dimmed. This bit is used only in a menu.
ODS_SELECTED   This bit is set if the item's status is selected.
ODS_COMBOBOXEDIT   The drawing takes place in the selection field (edit control) of an ownerdrawn combo box.
ODS_DEFAULT   The item is the default item
*/
/*
   CPen white(PS_SOLID,1,RGB(255,255,255));
   CPen bgray(PS_SOLID,1,RGB(192,192,192));
   CPen lgray(PS_SOLID,1,RGB(128,128,128));
   CPen dgray(PS_SOLID,1,RGB( 64, 64, 64));
   CPen black(PS_SOLID,1,RGB(  0,  0,  0));
*/
   CBrush white(RGB(255,255,255));
   CBrush gray2(RGB(192,192,192));
   CBrush gray1(RGB(128,128,128));
   CBrush gray0(RGB( 96, 96, 96));
   CBrush black(RGB(  0,  0,  0));

	// TODO: Add your code to draw the specified item
   CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

   CBrush brush(m_color);
   void *prevBrush = pDC->SelectObject(brush);
//   void *prevPen = pDC->SelectObject(black);

   CRect crect(lpDrawItemStruct->rcItem);
   pDC->FrameRect(crect,&white);

   crect.DeflateRect(1,1);
   pDC->FrameRect(crect,&gray2);

   if ((lpDrawItemStruct->itemState & ODS_SELECTED))
   {

   crect.DeflateRect(1,1);
   pDC->FrameRect(crect,&gray1);

   crect.DeflateRect(1,1);
   pDC->FrameRect(crect,&gray0);

   }

   crect.DeflateRect(1,1);
   pDC->Rectangle(crect);

/*
   if ((lpDrawItemStruct->itemState & ODS_FOCUS))
   {
      crect = CRect(lpDrawItemStruct->rcItem);
      pDC->FrameRect(crect,&black);
   }
*/

   pDC->SelectObject(prevBrush);
}

void CFVWColorButton::SetColor(COLORREF color)
{
   m_color = color;
   Invalidate();
}

COLORREF CFVWColorButton::GetColor(void)
{
   return m_color;
}
