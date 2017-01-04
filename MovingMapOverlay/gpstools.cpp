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
// gpstools.cpp
//
#include "stdafx.h"
#include "gpstools.h"
#include "ctrakdlg.h"
#include "rbdlg.h"
#include "..\MapView.h"

/////////////////////////////////////////////////////////////////////////////
// GPSToolsPropertySheet

IMPLEMENT_DYNAMIC(GPSToolsPropertySheet, CPropertySheet)

BEGIN_MESSAGE_MAP(GPSToolsPropertySheet, CPropertySheet)
   //{{AFX_MSG_MAP(CMDMSheet)
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_CLOSE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GPSToolsPropertySheet message handlers
void GPSToolsPropertySheet::PostNcDestroy()
{
   // [Bug 5084] After creating a modal dialog from a property page in a modeless property sheet
   // and closing the property sheet, another window, not FalconView, receives the keyboard focus
   // and is activated.  We set focus on the frame in GPSTools' PostNcDestroy to fix this.
   CWnd *pFrame = UTL_get_frame();
   if (pFrame)
   {
      BOOL bEnabled = pFrame->IsWindowEnabled();
      if (!bEnabled)
         pFrame->EnableWindow(TRUE);

      pFrame->SetFocus();
   }

   // remove the property pages
   while (GetPageCount())
   {
      delete GetPage(0);
      RemovePage(0);
   }
}

void GPSToolsPropertySheet::set_focus(C_gps_trail *gps_trail, int page_num)
{
   if (C_gps_trail::is_gps_tools_opened())
   {
      SetActivePage(page_num);
   }
   else
   {
      SetTitle("Moving Map Tools");

      AddPage(new RangeBearingPage(gps_trail->get_rb_mgr()));
      AddPage(new CoastTrackPage(gps_trail->get_coast_track_mgr()));

      SetActivePage(page_num);
      
      DWORD dwStyle = WS_OVERLAPPED|WS_SYSMENU|WS_THICKFRAME|WS_CAPTION|WS_VISIBLE;
      DWORD dwExStyle = 0; //WS_EX_DLGMODALFRAME|WS_EX_MDICHILD;
      Create(UTL_get_frame(), dwStyle, dwExStyle);
   }
}

// handle the WM_ON_SIZE message
void GPSToolsPropertySheet::OnSize(UINT nType, int cx, int cy) 
{
   CPropertySheet::OnSize(nType, cx, cy);
   CRect rect;

   // Process the active page
   CPropertyPage* q = GetActivePage();
   if (q)
   {
      // resize the tab control and property page
      {
         GetClientRect(&rect);
         GetTabControl()->MoveWindow(rect, TRUE);
         GetTabControl()->AdjustRect(FALSE, &rect);
         
         switch (GetActiveIndex())
         {
         case PAGE_INDEX_RANGE_BEARING:     // Range/Bearing
            // resize the tab control and property page
            ((RangeBearingPage*)GetPage(PAGE_INDEX_RANGE_BEARING))->MoveWindow(rect, TRUE);
            ((RangeBearingPage*)GetPage(PAGE_INDEX_RANGE_BEARING))->on_size(nType, cx, cy);
            break;
         case PAGE_INDEX_COAST_TRACK:   // Coast Tracks
            
            // resize the tab control and property page
            ((CoastTrackPage*)GetPage(PAGE_INDEX_COAST_TRACK))->MoveWindow(rect, TRUE);
            ((CoastTrackPage*)GetPage(PAGE_INDEX_COAST_TRACK))->on_size(nType, cx, cy);
            break;
         default:
            AfxMessageBox("error in handler");
         }
      }
   }

   // resize the property sheet
}

void GPSToolsPropertySheet::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
   // Process the active page
   CPropertyPage* q = GetActivePage();
   if (q)
   {
      // Limit the size of this property page
      const int margin = 8;
      CRect winrect;
      CRect clientrect;
      CRect minrect;
      CRect tabrect;
      
      // Get minimum size rectangle from active tab
      // Must cast to appropriate page type to access local member
      {
         switch (GetActiveIndex())
         {
         case 0:     // Range/Bearing
            ((RangeBearingPage*)GetPage(PAGE_INDEX_RANGE_BEARING))->GetMinimumRect(minrect);
            break;
         case 1:  // Coast Tracks
            ((CoastTrackPage*)GetPage(PAGE_INDEX_COAST_TRACK))->GetMinimumRect(minrect);
            break;
         default:
            AfxMessageBox("error in handler");
         }
      }
      
      GetWindowRect(&winrect);
      GetClientRect(&clientrect);
      
      //GetTabControl()->AdjustRect(FALSE, &minrect);
      GetTabControl()->GetItemRect(0, &tabrect);
      
      // Add margins onto minimum rectangle
      minrect.right += (clientrect.left - winrect.left) + (winrect.right - clientrect.right);
      minrect.bottom += (clientrect.top - winrect.top) + (winrect.bottom - clientrect.bottom);
      minrect.bottom += tabrect.bottom;
      
      CPoint minPoint(minrect.right + margin*2, minrect.bottom + margin*2);
      CPoint maxPoint(2048,2048);
      lpMMI->ptMinTrackSize = minPoint;
      lpMMI->ptMaxTrackSize = maxPoint;
   }
   
   CPropertySheet::OnGetMinMaxInfo(lpMMI);
}

BOOL GPSToolsPropertySheet::OnInitDialog() 
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   // Change the icon in the top corner
   HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME_FALCONVIEW);
   SetIcon(hIcon, FALSE);

   return bResult;
}

void GPSToolsPropertySheet::OnClose() 
{
   MapView::m_get_position_from_map = FALSE;
   CPropertySheet::OnClose();
}
