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



// showtip.cpp  --  display a remark dialog

#include "stdafx.h"
#include "tipdlg.h"
#include "showrmk.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static CTipDialog *m_pTip = nullptr;
static int tipcnt = 0;
CIconImage* CTipDisplay::m_icon_drag_image= FALSE;

CTipDisplay::CTipDisplay()
{
}

// *************************************************************

void CTipDisplay::display(CWnd* wnd, int x, int y, const char* text)
{
   if (CTipDialog::is_active() && m_pTip != NULL)
   {
      m_pTip->EndTip();
      m_pTip = NULL;
   }
   
   m_pTip = new CTipDialog(wnd);
   m_pTip->ShowTip(x, y, text);
   tipcnt++;
}

// *************************************************************

void CTipDisplay::display_center(CWnd* wnd, int x, int y, const char* text)
{
   if (CTipDialog::is_active() && m_pTip != NULL)
   {
      m_pTip->EndTip();
      m_pTip = NULL;
   }
   else
      m_pTip = new CTipDialog(wnd);

 //  m_pTip->ShowTipCenter(x, y, text);
   tipcnt++;
}

// *************************************************************

void CTipDisplay::clear()
{
   if (CTipDialog::is_active() && m_pTip != NULL)
   {
      // turn off icon dragging so that 
      if (m_icon_drag_image)
         m_icon_drag_image->hide_drag();

      m_pTip->EndTip();
      m_pTip = NULL;

      // turn on icon dragging so that 
      if (m_icon_drag_image)
         m_icon_drag_image->show_drag();
   }
}
   
// *************************************************************

BOOL CTipDisplay::is_active()
{
   return CTipDialog::is_active();
}

// *************************************************************

int CTipDisplay::get_instance()
{
   return tipcnt;
}

// *************************************************************

void CTipDisplay::hide()
{
   if (CTipDialog::is_active() && m_pTip != NULL)
      m_pTip->HideTip();
}

// *************************************************************

void CTipDisplay::redisplay()
{
   if (CTipDialog::is_active() && m_pTip != NULL)
      m_pTip->ReShowTip();
}

// *************************************************************

void CTipDisplay::notify_icon_drag_in_use(CIconImage* image)
{
   m_icon_drag_image = image;
}

// *************************************************************
