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



// securdlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "securdlg.h"
#include "getobjpr.h"   // fvw_get_view()->update_title_bar()
#include "param.h"
#include "getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CSecurDlg dialog


CSecurDlg::CSecurDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CSecurDlg::IDD, pParent), m_label_on_title_bar("")
{
   //{{AFX_DATA_INIT(CSecurDlg)
   m_display_sec_label_on_printout = FALSE;
   m_display_sec_label_on_title_bar = FALSE;
   //}}AFX_DATA_INIT
}

// alternate constructor that will use the IDD2
// options dialog (removed check boxes)
CSecurDlg::CSecurDlg(CString str, CWnd* pParent /*=NULL*/)
   : CDialog(CSecurDlg::IDD2, pParent), m_label_on_title_bar("")
{
   //{{AFX_DATA_INIT(CSecurDlg)
   m_display_sec_label_on_printout = FALSE;
   m_display_sec_label_on_title_bar = FALSE;
   //}}AFX_DATA_INIT
}

void CSecurDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSecurDlg)
   DDX_Control(pDX, IDC_SC_CUSTOM_LABEL, m_custom_label);
   DDX_Control(pDX, IDC_SC_STANDARD_LB, m_std_secur_lb);
   DDX_Check(pDX, IDC_SC_PRINTOUT, m_display_sec_label_on_printout);
   DDX_Check(pDX, IDC_TITLE_BAR_TOGGLE, m_display_sec_label_on_title_bar);

   DDX_Control(pDX, IDC_SC_NONE, m_SC_none);
   DDX_Control(pDX, IDC_SC_CUSTOM, m_SC_custom);
   DDX_Control(pDX, IDC_SC_STANDARD, m_SC_standard);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSecurDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CSecurDlg)
   ON_BN_CLICKED(IDC_SC_CUSTOM, OnScCustom)
   ON_BN_CLICKED(IDC_SC_STANDARD, OnScStandard)
   ON_BN_CLICKED(IDC_SC_NONE, OnScNone)
   ON_BN_CLICKED(IDC_HELP_FVW, OnHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecurDlg message handlers

BOOL CSecurDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_display_sec_label_on_printout = 
      PRM_get_registry_int("Security", "DisplayLabelOnPrintout", 1);
   m_display_sec_label_on_title_bar =
      PRM_get_registry_int("Security", "DisplayLabelOnTitleBar", 1);

   CString custom_label =
      PRM_get_registry_string("Security", "MRUCustomClass", "");
   m_custom_label.SetLimitText(80);
   m_custom_label.SetWindowText(custom_label);
   
   m_std_secur_lb.AddString("UNCLASSIFIED");
   m_std_secur_lb.AddString("LIMITED DISTRIBUTION");
   m_std_secur_lb.AddString("CONFIDENTIAL");
   m_std_secur_lb.AddString("SECRET");
   m_std_secur_lb.AddString("TOP SECRET");

   //
   //  set this even if current label type is not standard (so that it will
   //  be set if the user switches to standard)
   //
   CString mru_standard_label = PRM_get_registry_string("Security", 
      "MRUStandardClass", "LIMITED DISTRIBUTION");
   int index;
   if (mru_standard_label.CompareNoCase("UNCLASSIFIED") == 0)
      index = 0;
   if (mru_standard_label.CompareNoCase("LIMITED DISTRIBUTION") == 0)
      index = 1;
   else if (mru_standard_label.CompareNoCase("CONFIDENTIAL") == 0)
      index = 2;
   else if (mru_standard_label.CompareNoCase("SECRET") == 0)
      index = 3;
   else if (mru_standard_label.CompareNoCase("TOP SECRET") == 0)
      index = 4;
   else 
      index = 0;
   m_std_secur_lb.SetCurSel(index);

   CString current =
      PRM_get_registry_string("Security", "CurrentClass", "STANDARD");

   UpdateData(FALSE);

   if (current.CompareNoCase("STANDARD") == 0)
   {
      set_button(IDC_SC_STANDARD);
      OnScStandard();
      if (m_display_sec_label_on_title_bar)
         m_label_on_title_bar = mru_standard_label;
   }
   else if (current.CompareNoCase("CUSTOM") == 0)
   {
      set_button(IDC_SC_CUSTOM);
      OnScCustom();
      if (m_display_sec_label_on_title_bar)
         m_label_on_title_bar = custom_label;
   }
   else
   {
      set_button(IDC_SC_NONE);
      OnScNone();
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

int CSecurDlg::get_checked_radio_button(void)
{
   if (m_SC_none.GetCheck())
      return IDC_SC_NONE;
   if (m_SC_standard.GetCheck())
      return IDC_SC_STANDARD;
   if (m_SC_custom.GetCheck())
      return IDC_SC_CUSTOM;

   return IDC_SC_NONE;
}

void CSecurDlg::set_button(int id)
{
   switch(id)
   {
   case IDC_SC_NONE : m_SC_none.SetCheck(1); break;
   case IDC_SC_STANDARD: m_SC_standard.SetCheck(1); break;
   case IDC_SC_CUSTOM: m_SC_custom.SetCheck(1); break;
   }
}

CWnd* CSecurDlg::get_std_secur_list_box(void)
{
   return GetDlgItem(IDC_SC_STANDARD_LB);
}

CWnd* CSecurDlg::get_custom_label_window(void)
{
   return GetDlgItem(IDC_SC_CUSTOM_LABEL);
}

CWnd* CSecurDlg::get_printout_checkbox(void)
{
   return GetDlgItem(IDC_SC_PRINTOUT);
}

CWnd* CSecurDlg::get_title_bar_checkbox(void)
{
   return GetDlgItem(IDC_TITLE_BAR_TOGGLE);
}

void CSecurDlg::OnScCustom() 
{
   get_std_secur_list_box()->EnableWindow(FALSE);
   get_custom_label_window()->EnableWindow(TRUE);
   get_printout_checkbox()->EnableWindow(TRUE);
   get_title_bar_checkbox()->EnableWindow(TRUE);

   //
   // set focus to the edit control
   //
   get_custom_label_window()->SetFocus();
}

void CSecurDlg::OnScStandard() 
{
   get_std_secur_list_box()->EnableWindow(TRUE);
   get_custom_label_window()->EnableWindow(FALSE);
   get_printout_checkbox()->EnableWindow(TRUE);
   get_title_bar_checkbox()->EnableWindow(TRUE);

   //
   // set focus to the list box
   //
   get_std_secur_list_box()->SetFocus();
}

void CSecurDlg::OnScNone() 
{
   get_std_secur_list_box()->EnableWindow(FALSE);
   get_custom_label_window()->EnableWindow(FALSE);
   get_printout_checkbox()->EnableWindow(FALSE);
   get_title_bar_checkbox()->EnableWindow(FALSE);
}

void CSecurDlg::OnOK() 
{
   CString label_on_title_bar("");

   UpdateData(TRUE);

   PRM_set_registry_int("Security", "DisplayLabelOnPrintout",
      m_display_sec_label_on_printout ? 1 : 0);

   PRM_set_registry_int("Security", "DisplayLabelOnTitleBar",
      m_display_sec_label_on_title_bar ? 1 : 0);
   MapView::m_security_label_on_title_bar = m_display_sec_label_on_title_bar ? 1 : 0;

   CString custom_label;
   m_custom_label.GetWindowText(custom_label);
   PRM_set_registry_string("Security", "MRUCustomClass",
      custom_label);

   if (get_checked_radio_button() == IDC_SC_CUSTOM)
   {
      PRM_set_registry_string("Security", "CurrentClass",
         "CUSTOM");
      MapView::m_current_security_class = "CUSTOM";

      // set the title bar label string if the box is checked
      // by default the string is empty
      if (m_display_sec_label_on_title_bar)
      {
         label_on_title_bar = custom_label;
         MapView::m_custom_label = custom_label;
      }
   }
   else if (get_checked_radio_button() == IDC_SC_STANDARD)
   {
      PRM_set_registry_string("Security", "CurrentClass",
         "STANDARD");
      MapView::m_current_security_class = "STANDARD";


      if (m_std_secur_lb.GetCurSel() == 4)
      {
         PRM_set_registry_string("Security", "MRUStandardClass",
            "TOP SECRET");
         MapView::m_mru_standard_label = "TOP SECRET";
         label_on_title_bar = "TOP SECRET";
      }
      else if (m_std_secur_lb.GetCurSel() == 3)
      {
         PRM_set_registry_string("Security", "MRUStandardClass",
            "SECRET");
         MapView::m_mru_standard_label = "SECRET";
         label_on_title_bar = "SECRET";
      }
      else if (m_std_secur_lb.GetCurSel() == 2)
      {
         PRM_set_registry_string("Security", "MRUStandardClass",
            "CONFIDENTIAL");
         MapView::m_mru_standard_label = "CONFIDENTIAL";
         label_on_title_bar = "CONFIDENTIAL";
      }
      else if (m_std_secur_lb.GetCurSel() == 1)
      {
         PRM_set_registry_string("Security", "MRUStandardClass",
            "LIMITED DISTRIBUTION");
         MapView::m_mru_standard_label = "LIMITED DISTRIBUTION";
         label_on_title_bar = "LIMITED DISTRIBUTION";
      }
      else
      {
          PRM_set_registry_string("Security", "MRUStandardClass",
            "UNCLASSIFIED");
          MapView::m_mru_standard_label = "UNCLASSIFIED";
          label_on_title_bar = "UNCLASSIFIED";
      }

      // if the label is include on title bar check box is not checked, 
      // clear the title bar label string
      if (!m_display_sec_label_on_title_bar)
         label_on_title_bar = "";
   }
   else
   {
      PRM_set_registry_string("Security", "CurrentClass",
         "NONE");
      MapView::m_current_security_class = "NONE";
   }

   // if the security label on the title bar has changed, or it has been turned
   // on or off, the title bar must be refreshed
   if (m_label_on_title_bar != label_on_title_bar)
   {
      MapView *view = fvw_get_view();
      if (view)
         view->update_title_bar();
   }

   CDialog::OnOK();
}

void CSecurDlg::OnHelp() 
{
   CWnd::OnHelp();
}
LRESULT CSecurDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

