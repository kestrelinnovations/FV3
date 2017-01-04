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

// SetProjectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "map.h"
#include "mapview.h"
#include "proj.h"
#include "SetProjectionDlg.h"
#include "StatusBarInfoPane.h"
#include "getobjpr.h"
#include "getobjpr.h"
#include "statbar.h"
#include "utils.h"

/////////////////////////////////////////////////////////////////////////////
// CSetProjectionDlg dialog


CSetProjectionDlg::CSetProjectionDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CSetProjectionDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSetProjectionDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void CSetProjectionDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSetProjectionDlg)
   DDX_Control(pDX, IDC_EDIT_CURRENT_PROJECTION, m_current_projection_eb);
   DDX_Control(pDX, IDC_COMBO_REQUESTED_PROJECTION, m_requested_projection_cb);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetProjectionDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CSetProjectionDlg)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   ON_CBN_SELCHANGE(IDC_COMBO_REQUESTED_PROJECTION, OnSelchangeComboRequestedProjection)
   ON_WM_DESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetProjectionDlg message handlers

void CSetProjectionDlg::OnOK() 
{
   // TODO: Add extra validation here
   apply_projection();

   CDialog::OnOK();
}

void CSetProjectionDlg::OnCancel() 
{
   // TODO: Add extra cleanup here

   CDialog::OnCancel();
}

void CSetProjectionDlg::OnApply() 
{
   // TODO: Add your control notification handler code here
   apply_projection();

   // Disable the apply button
   GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

}

void CSetProjectionDlg::apply_projection()
{
   CString current_str;
   m_current_projection_eb.GetWindowText(current_str);

   int requested_index = m_requested_projection_cb.GetCurSel();

   // if the user type and invalida value return 
   if ( requested_index < 0 || requested_index > m_requested_projection_cb.GetCount())
   {
      GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
      GetDlgItem(IDOK)->EnableWindow(FALSE);

      return;
   }

   CString requested_str;
   m_requested_projection_cb.GetLBText( requested_index, requested_str );

   // If the user selected the same projection, nothing to do.
   if ( current_str == requested_str )
      return;

   // Set the requested projection via MapView
   CView* view = UTL_get_active_non_printing_view();
   if (view == NULL)
      return;
   
   if (view && view->IsKindOf(RUNTIME_CLASS(MapView)))
   {
      MapView *map_view = static_cast<MapView*>(view);

      map_view->ChangeProjectionType(static_cast<ProjectionEnum>(requested_index));
      m_current_projection_eb.SetWindowText(requested_str);
   }
}


BOOL CSetProjectionDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // Disable the apply and OK buttons until the user selects a different 
   // projection on the combo box
   GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
   GetDlgItem(IDOK)->EnableWindow(FALSE);

   MapView* view = static_cast<MapView *>(
      UTL_get_active_non_printing_view());
   if (view == NULL)
      return FALSE;

   CString current_str;
   ProjectionID id(view->GetProjectionType());

   current_str = id.get_string();
   m_current_projection_eb.SetWindowText(current_str);

   CMainFrame *pFrame = fvw_get_frame();

   // Get the available projections
   CArray <class ProjectionID, const class ProjectionID&> available_projection_array;
   PROJ_get_available_projection_types(&available_projection_array, pFrame->Is3DEnabledInRegistry());

   CString requested_str;
   int current_position = 0;
   for ( int i = 0; i < available_projection_array.GetSize(); i++)
   {
      requested_str = available_projection_array.GetAt(i).get_string();
      m_requested_projection_cb.AddString( requested_str );
      if ( current_str == requested_str )
         current_position = i;
   }

   // Set the requested and current to be the same initially
   m_requested_projection_cb.SetCurSel(current_position /* 0 */);

   // Get a pointer to the ComboBox's Edit box
   CEdit *pEdit = (CEdit*)m_requested_projection_cb.GetWindow(GW_CHILD);

   CStatusBarInfoPane *pInfoPane = NULL;
   if (pFrame)
      pInfoPane = pFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_MAP_PROJECTION);

   CMapStatusBar *pBar = NULL;
   int nIndex = -1;
   if (pInfoPane)
   {
      pBar = pInfoPane->GetParentStatusBar();
      nIndex = pInfoPane->GetIndex();
   }

   if (pBar)
   {
      CRect dlg_rect, bar_rect, bar_ctrl_rect, view_rect;

      view->GetWindowRect(&view_rect);

      CSetProjectionDlg *pDlg = static_cast<CSetProjectionDlg*>(this);
      pDlg->GetWindowRect(&dlg_rect);

      dlg_rect.top = view_rect.top + view_rect.Height() / 2 - dlg_rect.Height() / 2;
      dlg_rect.left = dlg_rect.left + view_rect.Width() / 2 - dlg_rect.Width() / 2;
      dlg_rect.bottom = dlg_rect.top + dlg_rect.Height();
      dlg_rect.right = dlg_rect.left + dlg_rect.Width();
      m_dlg_rect = dlg_rect;

      pBar->GetItemRect(nIndex, &bar_rect);

      pBar->GetWindowRect(&bar_ctrl_rect);

      int bar_rect_width = bar_rect.Width();
      bar_rect.top = bar_ctrl_rect.top;
      bar_rect.left = bar_rect.left + bar_ctrl_rect.left;
      bar_rect.bottom = bar_ctrl_rect.bottom;
      bar_rect.right = bar_rect.left + bar_rect_width;

      m_proj_item_rect = bar_rect;

      ::DrawAnimatedRects( this->GetSafeHwnd(), 3, &bar_rect, &dlg_rect);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetProjectionDlg::OnSelchangeComboRequestedProjection() 
{
   // TODO: Add your control notification handler code here
   CString current_str;
   m_current_projection_eb.GetWindowText(current_str);

   int requested_index = m_requested_projection_cb.GetCurSel();

   CString requested_str;
   m_requested_projection_cb.GetLBText( requested_index, requested_str );

   // If the user selected a different projection than the current, activate
   // the Apply and OK buttons, else do nothing
   if ( current_str != requested_str )
   {
      GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
      GetDlgItem(IDOK)->EnableWindow(TRUE);
   }
}

BOOL CSetProjectionDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
   // TODO: Add your specialized code here and/or call the base class

   return CDialog::PreCreateWindow(cs);
}

void CSetProjectionDlg::OnDestroy() 
{
   CDialog::OnDestroy();

   // TODO: Add your message handler code here

   // Get the current position of the dialog and animate it toward 
   // the cached position of the status bar projection item rect
#if 1
   this->GetWindowRect(&m_dlg_rect);
   ::DrawAnimatedRects( this->GetSafeHwnd(), 3, &m_dlg_rect, &m_proj_item_rect);
#endif
}
LRESULT CSetProjectionDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}
