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

// StatusBarSetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fvw.h"
#include "StatusBarSetupDialog.h"
#include "getobjpr.h"
#include "mainfrm.h"
#include "StatusBarInfoPane.h"
#include "param.h"
#include "getobjpr.h"
#include "StatusBarManager.h"
#include "statbar.h"


const int NO_HIT = -2;
const int HIT_AVAILABLE_LIST_CONTROL = -1;

CStatusBarListCtrl::CStatusBarListCtrl()
{
   m_allowCalcSize = FALSE;
}

void CStatusBarListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
   if(m_allowCalcSize)
   {
      ModifyStyle(WS_HSCROLL | WS_VSCROLL, 0, 0);
      CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
   }
}

void CStatusBarListCtrl::HideScrollBars()
{
   RECT rect;
   GetClientRect(&rect);
   
   // Shrink the window rectangle one pixel, for the CListCtrl border
   rect.left++;
   rect.right--;
   rect.top++;
   rect.bottom--;

   // Allow the CListCtrl to draw where the scrollbar would have drawn
   int scroll_bar_width = GetSystemMetrics(SM_CXVSCROLL);
   int scroll_bar_height = GetSystemMetrics(SM_CYVSCROLL);

  
   SetWindowPos(NULL, rect.left, rect.top,
      rect.right + scroll_bar_width, rect.bottom + scroll_bar_height,
      SWP_NOMOVE | SWP_NOZORDER);

   SetWindowRgn(CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom), TRUE);
   m_allowCalcSize = FALSE;
}

void CStatusBarListCtrl::ShowScrollBars()
{
   m_allowCalcSize = TRUE;
}

// CStatusBarSetupDialog dialog
IMPLEMENT_DYNAMIC(CStatusBarSetupDialog, CDialog)

CStatusBarSetupDialog::CStatusBarSetupDialog(CWnd* pParent /*=NULL*/)
   : CDialog(CStatusBarSetupDialog::IDD, pParent),
   m_nMaxStatusBarRows(0),
   m_hOldCursor(NULL),
   m_bInDrag(false)
   , m_strDescription(_T(""))
{
}

CStatusBarSetupDialog::~CStatusBarSetupDialog()
{
}

void CStatusBarSetupDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_AVAILABLE_LISTCTRL, m_lcAvailableList);
   DDX_Text(pDX, IDC_STATIC_DESCRIPTION, m_strDescription);
   DDX_Control(pDX, IDC_ADD_ROW, m_btnAddRow);
   DDX_Control(pDX, IDC_DELETE_ROW, m_btnDeleteRow);
}


BEGIN_MESSAGE_MAP(CStatusBarSetupDialog, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_NOTIFY(LVN_BEGINDRAG, IDC_AVAILABLE_LISTCTRL, &CStatusBarSetupDialog::OnBeginDrag)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_AVAILABLE_LISTCTRL, &CStatusBarSetupDialog::OnItemChanged)
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONUP()
   ON_BN_CLICKED(IDC_ADD_ROW, &CStatusBarSetupDialog::OnBnClickedAddRow)
   ON_BN_CLICKED(IDC_DELETE_ROW, &CStatusBarSetupDialog::OnBnClickedDeleteRow)
   ON_BN_CLICKED(IDC_SAVE, &CStatusBarSetupDialog::OnBnClickedSave)
   ON_BN_CLICKED(IDC_LOAD, &CStatusBarSetupDialog::OnBnClickedLoad)
   ON_BN_CLICKED(IDC_RESTORE_DEFAULT, &CStatusBarSetupDialog::OnBnClickedRestoreDefault)
END_MESSAGE_MAP()

// CStatusBarSetupDialog message handlers
BOOL CStatusBarSetupDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // Reinitialize the CListCtrl
   m_lcAvailableList.HideScrollBars();
   ReinitAvailableList();
   m_lcAvailableList.ShowScrollBars();

   // maximum number of status bars stored in registry
   m_nMaxStatusBarRows = PRM_get_registry_int("Main", "MaxStatusBarRows", 4);

   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      m_btnAddRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() < m_nMaxStatusBarRows);
      m_btnDeleteRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() != 1);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CStatusBarSetupDialog::OnOK()
{
   CDialog::OnOK();
   DestroyWindow();
}

void CStatusBarSetupDialog::OnCancel()
{
   CDialog::OnCancel();
   DestroyWindow();
}

void CStatusBarSetupDialog::PostNcDestroy()
{
   CDialog::PostNcDestroy();
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      delete this;
      pFrame->m_pStatusBarSetupDialog = NULL;
   }
}

void CStatusBarSetupDialog::ReinitAvailableList()
{
   m_lcAvailableList.DeleteAllItems();

   CMainFrame *pFrame = static_cast<CMainFrame *>(fvw_get_frame());
   if (pFrame != NULL)
   {
      CList<CStatusBarInfoPane *, CStatusBarInfoPane *> &listAvailable = pFrame->m_statusBarMgr->m_availableInfoPaneList->GetAvailableList();
      POSITION position = listAvailable.GetHeadPosition();
      while (position)
      {
         CStatusBarInfoPane *pInfoPane = listAvailable.GetNext(position);
         if (pInfoPane->GetAvailable())
            m_lcAvailableList.InsertItem(m_lcAvailableList.GetItemCount(), pInfoPane->GetInfoPaneName());
      }
   }

   m_strDescription = "";
   UpdateData(FALSE);
}

void CStatusBarSetupDialog::OnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
   NM_LISTVIEW*   pNMListView = (NM_LISTVIEW*)pNMHDR;
   m_nDragIndex = pNMListView->iItem;

   if (m_nDragIndex != -1)
   {
      CString strItemText = m_lcAvailableList.GetItemText(m_nDragIndex, 0);
      StartInfoPaneDrag(strItemText);
   }

   *pResult = 0;
}

void CStatusBarSetupDialog::StartInfoPaneDrag(CString strDragInfoPaneName)
{
   m_strDragInfoPaneName = strDragInfoPaneName;

   // capture the mouse for this application
   SetCapture();

   m_hOldCursor = SetCursor(AfxGetApp()->LoadCursorA(IDC_DOTTED_RECTANGLE_CURSOR2));
   m_bInDrag = true;
}

void CStatusBarSetupDialog::OnMouseMove(UINT nFlags, CPoint point)
{
   if (m_bInDrag)
   {
      const int nHitStatus = HitTest(point);

      SetCursor(AfxGetApp()->LoadCursor(IDC_DOTTED_RECTANGLE_CURSOR2));

      CMainFrame *pFrame = fvw_get_frame();
      if (pFrame)
      {
         ClientToScreen(&point);
         for(int i=0; i<pFrame->m_statusBarMgr->m_arrStatusBars.GetCount(); ++i)
            pFrame->m_statusBarMgr->m_arrStatusBars[i]->SetDragInsertPos(nHitStatus == i ? &point : NULL);
      }
   }

   CDialog::OnMouseMove(nFlags, point);
}

int CStatusBarSetupDialog::HitTest(CPoint point)
{
   ClientToScreen(&point);

   CRect rectListControl;
   m_lcAvailableList.GetClientRect(&rectListControl);
   m_lcAvailableList.ClientToScreen(&rectListControl);

   if (rectListControl.PtInRect(point))
      return HIT_AVAILABLE_LIST_CONTROL;

   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      for(int i=0; i < pFrame->m_statusBarMgr->m_arrStatusBars.GetSize(); ++i)
      {
         CRect rectStatusBar;
         pFrame->m_statusBarMgr->m_arrStatusBars[i]->GetClientRect(&rectStatusBar);
         pFrame->m_statusBarMgr->m_arrStatusBars[i]->ClientToScreen(&rectStatusBar);

         if (rectStatusBar.PtInRect(point))
            return i;
      }
   }

   return NO_HIT;
}

void CStatusBarSetupDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
   CMainFrame *pFrame = fvw_get_frame();

   if (m_bInDrag && pFrame != NULL)
   {
      ReleaseCapture();

      int nHitStatus = HitTest(point);

      // remove m_strDragInfoPane from all status bars
      for(int i=0; i < pFrame->m_statusBarMgr->m_arrStatusBars.GetSize(); ++i)
      {
         pFrame->m_statusBarMgr->m_arrStatusBars[i]->RemoveInfoPane(m_strDragInfoPaneName);
         pFrame->m_statusBarMgr->m_arrStatusBars[i]->SetDragInsertPos(NULL);
      }

      // if dropped onto a status bar, add to the appropriate status bar
      if (nHitStatus >= 0)
      {
         ClientToScreen(&point);
         pFrame->m_statusBarMgr->m_arrStatusBars[nHitStatus]->AddInfoPane(pFrame->m_statusBarMgr->m_availableInfoPaneList->GetInfoPane(m_strDragInfoPaneName), &point);
      }

      ReinitAvailableList();

      SetCursor(m_hOldCursor);
      m_bInDrag = false;
   }

   m_strDragInfoPaneName = "";

   CDialog::OnLButtonUp(nFlags, point);
}

void CStatusBarSetupDialog::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
   if (m_lcAvailableList.GetSelectedCount() == 1)
   {
      POSITION pos = m_lcAvailableList.GetFirstSelectedItemPosition();
      CMainFrame *pFrame = fvw_get_frame();

      if (pos && pFrame != NULL)
      {
         const int nIndex = m_lcAvailableList.GetNextSelectedItem(pos);
         CString strInfoPane = m_lcAvailableList.GetItemText(nIndex, 0);

         CStatusBarInfoPane *pInfoPane = pFrame->m_statusBarMgr->m_availableInfoPaneList->GetInfoPane(strInfoPane);
         if (pInfoPane != NULL)
         {
            m_strDescription = pInfoPane->GetDescription();
         }
      }
   }
   else
      m_strDescription = "";

   UpdateData(FALSE);
}

void CStatusBarSetupDialog::OnBnClickedAddRow()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      pFrame->AddStatusBarRow();
      m_btnAddRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() < m_nMaxStatusBarRows);
      m_btnDeleteRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() != 1);
   }
}

void CStatusBarSetupDialog::OnBnClickedDeleteRow()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      pFrame->RemoveStatusBarRow();
      m_btnAddRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() < m_nMaxStatusBarRows);
      m_btnDeleteRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() != 1);
   }

   ReinitAvailableList();
}

void CStatusBarSetupDialog::OnBnClickedSave()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      CString initial_dir = PRM_get_registry_string("Main", "USER_DATA", "") + "\\StatusBar\\*.xml";
     
      static char szFilter[] = "StatusBar Config Files (*.xml)|*.xml||";
      CFileDialog dlg(FALSE, ".xml", initial_dir, OFN_HIDEREADONLY | 
         OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN, szFilter, this);
      dlg.m_ofn.lpstrTitle = "Save StatusBar Configuration";

      if (dlg.DoModal() == IDOK)
         pFrame->SaveStatusBarConfig(dlg.GetPathName());
   }
}

void CStatusBarSetupDialog::OnBnClickedLoad()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      CString initial_dir = PRM_get_registry_string("Main", "USER_DATA", "") + "\\StatusBar\\*.xml";

      static char szFilter[] = "StatusBar Config Files (*.xml)|*.xml||";
      CFileDialog dlg(TRUE, NULL, initial_dir, OFN_FILEMUSTEXIST, szFilter, this);
      dlg.m_ofn.lpstrTitle = "Select StatusBar Configuration";

      if (dlg.DoModal() == IDOK)
      {
         pFrame->LoadStatusBarConfig(dlg.GetPathName());
         m_btnAddRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() < m_nMaxStatusBarRows);
         m_btnDeleteRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() != 1);
         ReinitAvailableList();
      }
   }
}

void CStatusBarSetupDialog::OnBnClickedRestoreDefault()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      CString strDefault = PRM_get_registry_string("Main", "HD_DATA", "") + "\\StatusBar\\DefaultStatusBarConfig.xml";

      pFrame->LoadStatusBarConfig(strDefault);
      m_btnAddRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() < m_nMaxStatusBarRows);
      m_btnDeleteRow.EnableWindow(pFrame->m_statusBarMgr->m_arrStatusBars.GetSize() != 1);
      ReinitAvailableList();
   }
}
LRESULT CStatusBarSetupDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

