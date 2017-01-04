// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.



// SMSheet.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SMSheet.h"

// system includes

// third party files

// other FalconView headers
#include "param.h"

// this project's headers
#include "SDSGenCovDlg.h"
#include "SDSUtil.h"


namespace scene_mgr
{

// CSMSheet

IMPLEMENT_DYNAMIC(CSMSheet, CPropertySheet)

CSMSheet::CSMSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   AddPage(&m_pgManage);
   AddPage(&m_pgPaths);
   AddPage(&m_pgBrowse);
   AddPage(&m_pgSearch);
}


BEGIN_MESSAGE_MAP(CSMSheet, CPropertySheet)
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_CLOSE()
   ON_WM_KEYUP()
   ON_REGISTERED_MESSAGE(SM_COVGEN_STOP, &CSMSheet::OnCovGenStop)
END_MESSAGE_MAP()


// CSMSheet message handlers


void CSMSheet::OnSize(UINT nType, int cx, int cy)
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
            case 0:  // Manage
               reinterpret_cast<CSMManagePage*>(GetPage(0))->MoveWindow(rTabCtrl, TRUE);
               break;
            case 1:  // Paths
               reinterpret_cast<CSMPathsPage*>(GetPage(1))->MoveWindow(rTabCtrl, TRUE);
               break;
            case 2:  // Browse
               reinterpret_cast<CSMBrowsePage*>(GetPage(2))->MoveWindow(rTabCtrl, TRUE);
               break;
            case 3: // Search
               reinterpret_cast<CSMSearchPage*>(GetPage(3))->MoveWindow(rTabCtrl, TRUE);
               break;
            default:
               AfxMessageBox("error in handler");
         }
      }
   }
}


void CSMSheet::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
   CPropertySheet::OnGetMinMaxInfo(lpMMI);
}


void CSMSheet::OnClose()
{
   // Simulate clicking the Scene Data Manager button on the toolbar.
   // This has the effect of closing the dialog as this can only be done if already visible.
   AfxGetMainWnd()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_SCENE_DATA_MANAGER, 0), NULL);
   SaveSheetPos();

   CPropertySheet::OnClose();
}

void CSMSheet::Refresh()
{
   switch (GetActiveIndex())
   {
      case 0:  // Manage
         m_pgManage.Refresh();
         break;
      case 1:  // Paths
         m_pgPaths.Refresh();
         break;
      case 2:  // Browse
         m_pgBrowse.Refresh();
         break;
#if 0  // RP
      case 3:  // Search
         m_pgSearch.Refresh();
         break;
#endif
      default:
         AfxMessageBox("error in handler");
   }
}

// Save the current sheet position in the registry
void CSMSheet::SaveSheetPos(void)
{
   CRect rPos;
   int nPage;

   // Check if window has not been destroyed
   // If SDM is up when the application closes the SDM
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
         PRM_set_registry_string("Scene Data Manager","SM Position", sRegistry);
      }
   }
}


// Set the current sheet position from the registry
void CSMSheet::SetSheetPos(CWnd *pFrame)
{
   CString sRegistry = PRM_get_registry_string("Scene Data Manager","SM Position");
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
void CSMSheet::NudgeSheetPos(void)
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

BOOL CSMSheet::OnInitDialog()
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   // Change the icon in the top corner
   HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME_FALCONVIEW);
   SetIcon(hIcon, FALSE);

   // Set this dialog to be the owner of any progress dialogs, etc.
   if(m_hWnd != NULL)
   {
      CSDSUtil::GetInstance()->SetMainWindow(this->m_hWnd);
   }

   return bResult;
}

CSMSheet::SMActive_t CSMSheet::GetSMPageActive()
{
   CSMSheet::SMActive_t nPageActive;

   switch (GetActiveIndex())
   {
      case 0:  // Manage
         nPageActive = cMANAGE;
         break;
      case 1:  // Paths
         nPageActive = cPATHS;
         break;
      case 2: // Chart Index
         nPageActive = cBROWSE;
         break;
      case 3:  // Catalog
         nPageActive = cSEARCH;
         break;
      default:
         AfxMessageBox("error in handler");
   }

   return nPageActive;
}

void CSMSheet::SetSMPageActive(SMActive_t page)
{
   switch(page)
   {
   case cMANAGE:
      SetActivePage(0);
      break;

   case cPATHS:
      SetActivePage(1);
      break;

   case cBROWSE:
      SetActivePage(2);
      break;

   case cSEARCH:
      SetActivePage(3);
      break;
   }
}

void CSMSheet::PostNcDestroy() 
{
   CPropertySheet::PostNcDestroy();
}

BOOL CSMSheet::PreTranslateMessage(MSG* pMsg) 
{
#if 0  // RP - TODO: Update with new SDM values?
   // if char is Ctrl-M
   if (pMsg->message == WM_CHAR && pMsg->wParam == 13)
   {
      OnClose();
      return TRUE;
   }
   else
#endif
      return CPropertySheet::PreTranslateMessage(pMsg);
}

void CSMSheet::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   CPropertySheet::OnKeyUp(nChar, nRepCnt, nFlags);
}

LRESULT CSMSheet::OnCovGenStop(WPARAM wPrm, LPARAM lPrm)
{
   // Refresh the scene tree on the Browse page
   // Make sure the page has actually been created first!
   if(::IsWindow(m_pgBrowse.m_hWnd))
   {
      m_pgBrowse.UpdateScenes();
   }

   return LRESULT(true);
}

};  // namespace scene_mgr
