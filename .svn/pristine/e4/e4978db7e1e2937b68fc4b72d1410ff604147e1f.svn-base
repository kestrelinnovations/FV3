// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// mdmsheet.cpp : implementation file
//

#include "stdafx.h"
#include "param.h"
#include "cov_ovl.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMSheet

IMPLEMENT_DYNAMIC(CMDMSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CMDMSheet, CPropertySheet)
   //{{AFX_MSG_MAP(CMDMSheet)
   ON_WM_CLOSE()
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_KEYUP()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

   
CMDMSheet::CMDMSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

   CString sCaption(pszCaption);

   if (sCaption == "Map Data Paths")
   {
      m_bPathMode = TRUE;
      m_shtPaths.ShowTargetControls(FALSE);
      AddPage(&m_shtPaths);
   }
   else
   {
      m_bPathMode = FALSE;
      m_shtPaths.ShowTargetControls(TRUE);

      AddPage(&m_shtSingle);
      AddPage(&m_shtMultiple);
      AddPage(&m_shtPaths);

      // We create the Chart Index tab first, since the CDLib tab is optional and may not be turned on.
      AddPage(&m_shtChartIndex);

      if (m_shtCatalog.IsCDLibPresent())
         AddPage(&m_shtCatalog);

      #ifdef _DEBUG
      //AddPage(&m_shtTypes);
      #endif
   }

}

// Refresh active page (order must match constructor above)
void CMDMSheet::Refresh()
{
   if (m_bPathMode)
   {
      m_shtPaths.Refresh();
   }
   else
   {
      switch (GetActiveIndex())
      {
         case 0:  // Single
            m_shtSingle.Refresh();
            break;
         case 1:  // Multiple
            m_shtMultiple.Refresh();
            break;
         case 2:  // Paths
            m_shtPaths.Refresh();
            break;
         case 3:  // Chart Index
            m_shtChartIndex.Refresh();
            break;
         case 4:  // Catalog
            m_shtCatalog.Refresh();
            break;
         #ifdef _DEBUG
         case 5:  // Types
            //m_shtTypes.Refresh();
            break;
         #endif
         default:
            AfxMessageBox("error in handler");
      }
   }
}


void CMDMSheet::OnClose() 
{
   if (m_bPathMode)
   {
      // Simulate clicking the Options/Map Data Paths item on the menu.
      // This has the effect of closing the dialog as this can only be done if already visible.
      AfxGetMainWnd()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_DATA_PATHS, 0), NULL);
   }
   else
   {
      // Simulate clicking the Map Data Manager button on the toolbar.
      // This has the effect of closing the dialog as this can only be done if already visible.
      AfxGetMainWnd()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_MAP_DATAMANAGER, 0), NULL);
      SaveSheetPos();
   }

   CPropertySheet::OnClose();
}

void CMDMSheet::PostNcDestroy() 
{
   CPropertySheet::PostNcDestroy();
}

void CMDMSheet::OnSize(UINT nType, int cx, int cy) 
{
   CPropertySheet::OnSize(nType, cx, cy);

   if (m_bPathMode)
   {
      // Process the active page
      CPropertyPage* q = GetActivePage();
      if (q)
      {
         // resize the tab control and property page
         CRect rTabCtrl;
         GetClientRect(&rTabCtrl);
         GetTabControl()->MoveWindow(rTabCtrl, TRUE);
         GetTabControl()->AdjustRect(FALSE, &rTabCtrl);
         ((CMDMPaths*)GetPage(0))->MoveWindow(rTabCtrl, TRUE);
      }
   }
   else
   {
      CoverageOverlay *overlay = cat_get_coverage_overlay();
      if (overlay && overlay->IsMDMDialogVisible())
      {
         // Don't process if minimizing
         if (cx > 0 && cy > 0)
         {
            // Process the Paths page for this mode
            CPropertyPage* q = GetActivePage();
            if (q)
            {
               // resize the tab control and property page
               CRect rTabCtrl;
               GetClientRect(&rTabCtrl);
               GetTabControl()->MoveWindow(rTabCtrl, TRUE);
               GetTabControl()->AdjustRect(FALSE, &rTabCtrl);

               /*
                * TODO: Why does GetActiveIndex return a different page than GetActivePage?
                * force fit for now...
                */
               switch (GetActiveIndex())
               {
                  case 0:  // Single
                     ((CMDMSingle*)GetPage(0))->MoveWindow(rTabCtrl, TRUE);
                     break;
                  case 1:  // Multiple
                     ((CMDMMultiple*)GetPage(1))->MoveWindow(rTabCtrl, TRUE);
                     break;
                  case 2:  // Paths
                     ((CMDMPaths*)GetPage(2))->MoveWindow(rTabCtrl, TRUE);
                     break;
                  case 3: // Chart Index
                     ((CMDMChartIndex*)GetPage(3))->MoveWindow(rTabCtrl, TRUE);
                     break;
                  case 4:  // Catalog
                     ((CMDMLibPropPage*)GetPage(4))->MoveWindow(rTabCtrl, TRUE);
                     break;
                  #ifdef _DEBUG
                  case 5:  // Types
                     //((CMDMTypes*)GetPage(5))->MoveWindow(rTabCtrl, TRUE);
                     break;
                  #endif
                  default:
                     AfxMessageBox("error in handler");
               }
            }
         }
      }
   }
}

void CMDMSheet::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
   {
      CoverageOverlay *overlay = cat_get_coverage_overlay();
      if (m_bPathMode || (overlay && overlay->IsMDMDialogVisible()))
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
            if (m_bPathMode)
               ((CMDMPaths*)GetPage(0))->GetMinimumRect(&minrect);
            else
            {
               switch (GetActiveIndex())
               {
                  case 0:  // Single
                     ((CMDMSingle*)GetPage(0))->GetMinimumRect(&minrect);
                     break;
                  case 1:  // Multiple
                     ((CMDMMultiple*)GetPage(1))->GetMinimumRect(&minrect);
                     break;
                  case 2:  // Paths
                     ((CMDMPaths*)GetPage(2))->GetMinimumRect(&minrect);
                     break;
                  case 3:
                     ((CMDMChartIndex*)GetPage(3))->GetMinimumRect(&minrect);
                     break;
                  case 4:
                     ((CMDMLibPropPage*)GetPage(4))->GetMinimumRect(&minrect);
                     break;
                  /*
                  #ifdef _DEBUG
                  case 5:  // Types
                     //((CMDMTypes*)GetPage(5))->m_MinSizeBoxCtrl.GetClientRect(&minrect);
                     break;
                  #endif
                  */
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
      }
   }
      
   CPropertySheet::OnGetMinMaxInfo(lpMMI);
}

// Save the current sheet position in the registry
void CMDMSheet::SaveSheetPos(void)
{
   CRect rPos;
   int nPage;

   // Check if window has not been destroyed
   // If MDM is up when the application closes the MDM
   // window gets destroyed before this call is made.
   if (::IsWindow(m_hWnd))
   {
      // we don't save the position if the property sheet is minimized
      WINDOWPLACEMENT wndpl;
      ::GetWindowPlacement(m_hWnd, &wndpl);
      if (wndpl.showCmd != SW_SHOWMINIMIZED)
      {
         // Save position for next time.
         GetWindowRect(&rPos);
         nPage = GetActiveIndex();

         CString sRegistry;;
         sRegistry.Format("%d,%d,%d,%d,%d*", rPos.left, rPos.top, rPos.right, rPos.bottom, nPage);
         PRM_set_registry_string("Map Data Manager","MDM Position", sRegistry);
      }
   }
}


// Set the current sheet position from the registry
void CMDMSheet::SetSheetPos(CWnd *pFrame)
{
   CString sRegistry = PRM_get_registry_string("Map Data Manager","MDM Position");
   CRect rPos;
   BOOL bAutoPosition = FALSE;
   int nPage;

   int n = sscanf_s(sRegistry, "%d,%d,%d,%d,%d", &rPos.left, &rPos.top, &rPos.right, &rPos.bottom, &nPage);
   if (n != 5)
      bAutoPosition = TRUE;

   if (!bAutoPosition)
   {
      // Ensure saved location falls entirely within visible screen
      CRect rDesktop;
      GetDesktopWindow()->GetWindowRect(&rDesktop);
      if (!(rPos.left   >= rDesktop.left   && 
            rPos.right  <= rDesktop.right  && 
            rPos.top    >= rDesktop.top    && 
            rPos.bottom <= rDesktop.bottom))
         bAutoPosition = TRUE;
   }

   if (bAutoPosition)
   {
      // If first time or problem reading registry string or outside screen...
      // position the property sheet at the lower left corner of the frame
      CRect rMain;
      CRect rSheet;
      pFrame->GetWindowRect(&rMain);
      GetWindowRect(&rSheet);
      SetWindowPos(pFrame, 
         rMain.left, 
         rMain.bottom - rSheet.Height(), 
         rSheet.Width(), 
         rSheet.Height(), 
         SWP_NOZORDER | SWP_NOACTIVATE);
      SetActivePage(0);
   }
   else
   {
      // Restore the property sheet to the saved location
      SetActivePage(nPage);

      SetWindowPos(NULL, 
         rPos.left, rPos.top, rPos.Width(), rPos.Height(), 
         SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
   }
}

// Move and size the sheet position slightly to force size messages
// This needs some explanation:
//    Using SetWindowPos will change the window but unless the size
//    changes the framework won't send SIZE and GETMINMAXINFO messages
//    to the window.  Since our code has extensive computation performed
//    in these message handlers we use the following method to force
//    Windows to send these messages for us.  It is critical that we
//    'inflate' first and 'deflate' second as the GETMINMAXINFO handler
//    will prevent the window from exceeding a certain value and so
//    the inflate operation is always guaranteed to succeed whereas the
//    deflate operation might not change the size of the window.
//    This routine 'must' be called while the sheet is visible or else
//    Windows won't send those message either. >:(
// TODO: Rethink how sizing is handled in general.
void CMDMSheet::NudgeSheetPos(void)
{
   CRect rPos;
   GetWindowRect(rPos);

   rPos.InflateRect(2,2);
   SetWindowPos(NULL, 
      rPos.left, rPos.top, rPos.Width(), rPos.Height(), 
      SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);

   rPos.DeflateRect(2,2);
   SetWindowPos(NULL, 
      rPos.left, rPos.top, rPos.Width(), rPos.Height(), 
      SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
}

BOOL CMDMSheet::PreTranslateMessage(MSG* pMsg) 
{
   // if char is Ctrl-M
   if (pMsg->message == WM_CHAR && pMsg->wParam == 13)
   {
      OnClose();
      return TRUE;
   }
   else
      return CPropertySheet::PreTranslateMessage(pMsg);
}

BOOL CMDMSheet::OnInitDialog()
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   // Change the icon in the top corner
   HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME_FALCONVIEW);
   SetIcon(hIcon, FALSE);

   return bResult;
}

CMDMSheet::MDMActive_t CMDMSheet::GetMDMPageActive()
{
   CMDMSheet::MDMActive_t nPageActive;

   if (m_bPathMode)
   {
      nPageActive = cPATHS;
   }
   else
   {
      switch (GetActiveIndex())
      {
         case 0:  // Single
            nPageActive = cSINGLE;
            break;
         case 1:  // Multiple
            nPageActive = cMULTIPLE;
            break;
         case 2:  // Paths
            nPageActive = cPATHS;
            break;
         case 3: // Chart Index
            nPageActive = cCHARTINDEX;
            break;
         case 4:  // Catalog
            nPageActive = cCATALOG;
            break;
         #ifdef _DEBUG
         case 5:  // Types
            nPageActive = cTYPES;
            break;
         #endif
         default:
            AfxMessageBox("error in handler");
      }
   }
   return nPageActive;
}

void CMDMSheet::SetMDMPageActive(MDMActive_t page)
{
   switch(page)
   {
   case cSINGLE:
      SetActivePage(0);
      break;

   case cMULTIPLE:
      SetActivePage(1);
      break;

   case cPATHS:
      SetActivePage(2);
      break;

   case cCHARTINDEX:
      SetActivePage(3);
      break;

   case cCATALOG:
      SetActivePage(4);
      break;

   #ifdef _DEBUG
   case cTYPES:
      SetActivePage(5);
      break;
   #endif
   }
}


void CMDMSheet::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   CPropertySheet::OnKeyUp(nChar, nRepCnt, nFlags);
}
