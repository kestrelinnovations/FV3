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

// rbview.cpp : implementation file
//

#include "stdafx.h"
#include "rb.h"
#include "..\resource.h"
#include "fvwutil.h"
#include "param.h"

/////////////////////////////////////////////////////////////////////////////
// RangeBearingView

RangeBearingView::RangeBearingView()
{

   VERIFY(m_menu.LoadMenu(IDR_RANGEBEARING));
}

RangeBearingView::~RangeBearingView()
{
}


BEGIN_MESSAGE_MAP(RangeBearingView, CWnd)
	//{{AFX_MSG_MAP(RangeBearingView)
	ON_WM_PAINT()
   ON_WM_CONTEXTMENU()
   ON_COMMAND(ID_SWAP_DIR, OnSwapDir)
   ON_UPDATE_COMMAND_UI(ID_SWAP_DIR, OnUpdateCommandSwapDir)
   ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// RangeBearingView message handlers

void RangeBearingView::OnPaint() 
{
   CRect rect;
	CPaintDC paint_dc(this); // device context for painting
   CDC *dc = &paint_dc;
   GetClientRect(rect);
   CDC memory_dc;

   // create an equivalent dc in memory
   memory_dc.CreateCompatibleDC(dc);

   CBitmap bmp;
   bmp.CreateCompatibleBitmap(dc, rect.Width(), rect.Height());
   memory_dc.SelectObject(&bmp);

   // draw the background
   CBrush brush(RANGE_BEARING_VIEW_COLOR_KEY);
   memory_dc.FillRect(&rect, &brush);
   DeleteObject(brush);

   // draw the text
   //
   CFvwUtil* pFvwUtil = CFvwUtil::get_instance();

   POINT cpt[4];
   pFvwUtil->draw_text(&memory_dc, get_text(), 0, 0, UTIL_ANCHOR_UPPER_LEFT, "Times", 
      __min(rect.Height(), rect.Width()), 0, UTIL_BG_SHADOW, UTIL_COLOR_WHITE, UTIL_COLOR_BLACK, 0, cpt);

   // copy contents of memory dc into the dc
   dc->BitBlt(0, 0, rect.Width(), rect.Height(), &memory_dc, 
      rect.TopLeft().x, rect.TopLeft().y, SRCCOPY);
}

CString RangeBearingView::get_text()
{
   CString heading_txt;
   heading_txt.Format("%.0f", m_bearing);

   // make sure the bearing is of the form xxx
   while(heading_txt.GetLength() < 3)
      heading_txt = "0" + heading_txt;

   // append the degrees symbol
   heading_txt += "°";

   // tack on the mag/true units to the end of the heading
   if (m_mag)
      heading_txt += "M";
   else
      heading_txt += "T";

   CString range_txt;
   range_txt.Format("%0.1f", m_range);
    
   // tack on the NM/km units to the end of the range
   if (m_units_nm)
      range_txt += "NM";
   else
      range_txt += "km";

   CString return_txt;
   return_txt.Format("%s/%s",heading_txt, range_txt);

   return return_txt;
}

void RangeBearingView::set_range_bearing_obj(RangeBearing *rb_obj) 
{ 
   m_range_bearing_obj = rb_obj;
}

void RangeBearingView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   if (pWnd != this)
      return;

   CMenu* pPopup = m_menu.GetSubMenu(0);
   ASSERT(pPopup != NULL);

   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
      point.x, point.y, this);
}

void RangeBearingView::OnSwapDir()
{
   m_range_bearing_obj->swap_direction();
}

void RangeBearingView::OnUpdateCommandSwapDir(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(1);
}

void RangeBearingView::OnDestroy() 
{
   WINDOWPLACEMENT placement;
	placement.length = sizeof(WINDOWPLACEMENT);
	if (GetWindowPlacement(&placement) != 0)
   {
      PRM_set_registry_int("MovingMap", "PosX", placement.rcNormalPosition.left);
      PRM_set_registry_int("MovingMap", "PosY", placement.rcNormalPosition.top);
      PRM_set_registry_int("MovingMap", "Width", placement.rcNormalPosition.right-placement.rcNormalPosition.left);
      PRM_set_registry_int("MovingMap", "Height", placement.rcNormalPosition.bottom-placement.rcNormalPosition.top);
   }
}