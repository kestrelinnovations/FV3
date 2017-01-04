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



// PrintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "common.h"
#include "PrintDlg.h"
#include "securdlg.h"
#include "PrintToolOverlay\PrntPage.h"   // for the CLabelingOptions class
#include "getobjpr.h"
#include "SecurityLabelExtensionCOM.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintOptDlg dialog


CPrintOptDlg::CPrintOptDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CPrintOptDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPrintOptDlg)
   m_print_chart_info = FALSE;
   m_print_map_type = FALSE;
   m_print_dafif_currency = FALSE;
   m_print_date_time = FALSE;
   m_print_echum_currency = FALSE;
   m_print_cadrg_currency = FALSE;
   m_print_compass_rose = TRUE;
   m_print_compass_rose_when_not_north_up = FALSE;
   //}}AFX_DATA_INIT

   m_using_alternate_dialog = FALSE;
}

// alternate constructor that will load IDD2, the alternate dialog
// which removes the 'Map Area Options' group
CPrintOptDlg::CPrintOptDlg(CString str, CWnd* pParent /*=NULL*/)
   : CDialog(CPrintOptDlg::IDD2, pParent)
{
   m_print_chart_info = FALSE;
   m_print_map_type = FALSE;
   m_print_dafif_currency = FALSE;
   m_print_date_time = FALSE;
   m_print_echum_currency = FALSE;
   m_print_cadrg_currency = FALSE;
   m_print_compass_rose = TRUE;
   m_print_compass_rose_when_not_north_up = FALSE;

   m_using_alternate_dialog = TRUE;
}

void CPrintOptDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPrintOptDlg)
   DDX_Control(pDX, IDC_TEXT_SLIDER, m_text_slider);
   DDX_Control(pDX, IDC_TEXT_PERCENTAGE, m_text_percentage);
   DDX_Control(pDX, IDC_ICON_PERCENTAGE, m_icon_percentage);
   DDX_Control(pDX, IDC_LINE_SLIDER, m_line_slider);
   DDX_Control(pDX, IDC_LINE_PERCENTAGE, m_line_percentage);
   DDX_Control(pDX, IDC_ICON_SLIDER, m_icon_slider);
   DDX_Check(pDX, IDC_PRINT_CHART_INFO, m_print_chart_info);
   DDX_Check(pDX, IDC_PRINT_MAP_TYPE, m_print_map_type);
   DDX_Check(pDX, IDC_PRINT_DAFIF_CURRENCY, m_print_dafif_currency);
   DDX_Check(pDX, IDC_PRINT_DATE_TIME, m_print_date_time);
   DDX_Check(pDX, IDC_PRINT_ECHUM_CURRENCY, m_print_echum_currency);
   DDX_Check(pDX, IDC_CADRG_CURRENCY, m_print_cadrg_currency);
   DDX_Check(pDX, IDC_PRINT_COMPASS_ROSE, m_print_compass_rose);
   DDX_Check(pDX, IDC_PRINT_COMPASS_ROSE_WHEN_NOT_NORTH_UP, m_print_compass_rose_when_not_north_up);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintOptDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CPrintOptDlg)
   ON_BN_CLICKED(IDC_SECURITY_BUTTON, OnSecurityButton)
   ON_BN_CLICKED(IDC_PRINT_COMPASS_ROSE, OnPrintCompassRose)
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_FVHELP, OnHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintOptDlg message handlers

BOOL CPrintOptDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   //
   // set the type of printing
   //
   CString print_type = 
      PRM_get_registry_string("Printing", "PrintType", "SCALE");
   if (print_type.CompareNoCase("WINDOW") == 0)
   {
      set_button(IDC_PRINT_WINDOW_RADIO);   
   }
   else
   {
      set_button(IDC_PRINT_TO_SCALE_RADIO);   
   }

   // if we are using the alternate dialog then the values will be 
   // passed in using set_labeling_options and do not need to be 
   // set here
   if (!m_using_alternate_dialog)
   {
      m_print_chart_info = PRM_get_registry_int("Printing", "PrintChartInfo", 1);
      m_print_map_type = PRM_get_registry_int("Printing", "PrintMapType", 1);
      m_print_date_time = PRM_get_registry_int("Printing",
         "PrintDateTime", 1);
      m_print_cadrg_currency = PRM_get_registry_int("Printing",
         "PrintCADRGCurrency", 1);
      m_print_dafif_currency = PRM_get_registry_int("Printing",
         "PrintDAFIFCurrency", 1);
      m_print_echum_currency = PRM_get_registry_int("Printing",
         "PrintECHUMCurrency", 1);
      m_print_compass_rose = PRM_get_registry_int("Printing",
         "PrintCompassRose", 1);
      m_print_compass_rose_when_not_north_up =  PRM_get_registry_int("Printing",
         "PrintCompassRoseWhenNotNorthUp", 0);
   }

   // if compass rose printing is disabled, disable cb to hide the
   // compass rose when north-up
   if ( m_print_compass_rose == 0 )
      GetDlgItem(IDC_PRINT_COMPASS_ROSE_WHEN_NOT_NORTH_UP)->EnableWindow(FALSE);

   // initialize the slider controls
   if (!m_using_alternate_dialog)
   {
      CString s;

      // set the range of each of the sliders 0 - 250%
      m_icon_slider.SetRange(0,250);
      m_text_slider.SetRange(0,250);
      m_line_slider.SetRange(0,250);

      // get the values of the enlargement percentages from the registry
      int icon_value = PRM_get_registry_int("Printing", 
         "IconAdjustSizePercentage", 0);
      int text_value = PRM_get_registry_int("Printing",
         "FontAdjustSizePercentage", 0);
      int line_value = PRM_get_registry_int("Printing",
         "LineWidthAdjustSizePercentage", 0);

      // set the position of each of the sliders
      m_icon_slider.SetPos(icon_value);
      m_text_slider.SetPos(text_value);
      m_line_slider.SetPos(line_value);

      // set the text displaying the percentage for each value
      s.Format("%d%%",icon_value);
      m_icon_percentage.SetWindowText(s);
      s.Format("%d%%",text_value);
      m_text_percentage.SetWindowText(s);
      s.Format("%d%%",line_value);
      m_line_percentage.SetWindowText(s);
   }

#ifdef GOV_RELEASE
   m_pSecurityLabelMgr = new CSecurityLabelExtensionCOM;
   m_pSecurityLabelMgr->Initialize();
   if ( m_pSecurityLabelMgr->UseCustomSecurityButton() )
   {
      CString button_text = m_pSecurityLabelMgr->CustomSecurityButtonLabel();
      GetDlgItem(IDC_SECURITY_BUTTON)->SetWindowText(button_text);
   }
   //GetDlgItem(IDC_SECURITY_BUTTON)->ShowWindow(SW_SHOW);  // Shows by default 
#else
   GetDlgItem(IDC_SECURITY_BUTTON)->ShowWindow(SW_HIDE); 
#endif

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintOptDlg::set_button(int id)
{
   CheckRadioButton(IDC_PRINT_TO_SCALE_RADIO, IDC_PRINT_WINDOW_RADIO, id);
}

//
// returns id if the checked radio button or 0 if none is selected
//
int CPrintOptDlg::get_checked_radio_button(void)
{
   return GetCheckedRadioButton(IDC_PRINT_TO_SCALE_RADIO,
      IDC_PRINT_WINDOW_RADIO);
}


void CPrintOptDlg::OnOK() 
{
   UpdateData(TRUE);

   if (get_checked_radio_button() == IDC_PRINT_WINDOW_RADIO)
   {
      PRM_set_registry_string("Printing", "PrintType", "WINDOW");
   }
   else
   {
      PRM_set_registry_string("Printing", "PrintType", "SCALE");
   }

   PRM_set_registry_int("Printing", "PrintChartInfo",
      m_print_chart_info ? 1 : 0);
   PRM_set_registry_int("Printing", "PrintMapType",
      m_print_map_type ? 1 : 0);
   PRM_set_registry_int("Printing", "PrintDateTime",
      m_print_date_time ? 1 : 0);
   PRM_set_registry_int("Printing", "PrintCADRGCurrency",
      m_print_cadrg_currency ? 1 : 0);
   PRM_set_registry_int("Printing", "PrintDAFIFCurrency",
      m_print_dafif_currency ? 1 : 0);
   PRM_set_registry_int("Printing", "PrintECHUMCurrency",
      m_print_echum_currency ? 1 : 0);

   PRM_set_registry_int("Printing", "PrintCompassRose",
      m_print_compass_rose ? 1 : 0);
   PRM_set_registry_int("Printing", "PrintCompassRoseWhenNotNorthUp",
      m_print_compass_rose_when_not_north_up ? 1 : 0);

   // store the values of the enlargement percentages in the registry
   if (!m_using_alternate_dialog)
   {
      PRM_set_registry_int("Printing", 
         "IconAdjustSizePercentage", m_icon_slider.GetPos());
      PRM_set_registry_int("Printing",
         "FontAdjustSizePercentage", m_text_slider.GetPos());
      PRM_set_registry_int("Printing",
         "LineWidthAdjustSizePercentage", m_line_slider.GetPos());
   }


   CDialog::OnOK();
}

void CPrintOptDlg::OnSecurityButton() 
{
   if ( m_pSecurityLabelMgr->UseCustomSecurityButton() )
      m_pSecurityLabelMgr->OnCustomSecurityButton(reinterpret_cast<long>(m_hWnd));
   else
   {
      if (m_using_alternate_dialog)
      {
         CSecurDlg dlg("Alternate Dialog");
         dlg.DoModal();
      }
      else
      {
         CSecurDlg dlg;
         dlg.DoModal();
      }
   }
}

void CPrintOptDlg::OnPrintCompassRose()
{
   int print_compass_rose = ((CButton*)GetDlgItem(IDC_PRINT_COMPASS_ROSE))->GetCheck();

   if ( print_compass_rose == 0 )
      GetDlgItem(IDC_PRINT_COMPASS_ROSE_WHEN_NOT_NORTH_UP)->EnableWindow(FALSE);
   else
      GetDlgItem(IDC_PRINT_COMPASS_ROSE_WHEN_NOT_NORTH_UP)->EnableWindow(TRUE);
}

void CPrintOptDlg::set_labeling_options(CLabelingOptions *labeling_options)
{
   m_print_chart_info = 
      labeling_options->get_print_chart_series_and_date();
   m_print_map_type = 
      labeling_options->get_print_map_type_and_scale();
   m_print_date_time = 
      labeling_options->get_print_date_and_time();
   m_print_cadrg_currency = 
      labeling_options->get_print_cadrg_currency();
   m_print_dafif_currency = 
      labeling_options->get_print_dafif_currency();
   m_print_echum_currency = 
      labeling_options->get_print_echum_currency();

   m_print_compass_rose = 
      labeling_options->get_print_compass_rose();
   m_print_compass_rose_when_not_north_up = 
      labeling_options->get_print_compass_rose_when_not_north_up();


}

void CPrintOptDlg::get_labeling_options(CLabelingOptions *labeling_options)
{
   labeling_options->set_print_chart_series_and_date(m_print_chart_info);
   labeling_options->set_print_map_type_and_scale(m_print_map_type);
   labeling_options->set_print_date_and_time(m_print_date_time);
   labeling_options->set_print_cadrg_currency(m_print_cadrg_currency);
   labeling_options->set_print_dafif_currency(m_print_dafif_currency);
   labeling_options->set_print_echum_currency(m_print_echum_currency);

   labeling_options->set_print_compass_rose(m_print_compass_rose);
   labeling_options->set_print_compass_rose_when_not_north_up(
      m_print_compass_rose_when_not_north_up);
}

void CPrintOptDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   // set the text displaying the percentage for each value
   CString s;
   s.Format("%d%%",m_icon_slider.GetPos());
   m_icon_percentage.SetWindowText(s);
   s.Format("%d%%",m_text_slider.GetPos());
   m_text_percentage.SetWindowText(s);
   s.Format("%d%%",m_line_slider.GetPos());
   m_line_percentage.SetWindowText(s);

   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPrintOptDlg::OnHelp()
{
   CWnd::OnHelp();
}
LRESULT CPrintOptDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

