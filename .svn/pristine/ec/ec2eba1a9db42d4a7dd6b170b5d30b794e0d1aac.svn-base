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

// PageProp.cpp
// Implementation of classes for the edit propeties dialog for the Printing
// Tools Overlay.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/PageProp.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/getobjpr.h"
#include "FalconView/include/MapTpSel.h"  // MapTypeSelection
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/printdlg.h"  // CPrintOptDlg

// this project's headers
#include "FalconView/PrintToolOverlay/factory.h"
// for prn_settings_violate_limits:
#include "FalconView/PrintToolOverlay/surf_lim.h"

// {F7AFA536-5D1F-4547-95D7-1E803692CCC9}
const GUID PROPPAGEID_PageLayout_SinglePage =
{
   0xf7afa536, 0x5d1f, 0x4547,
   { 0x95, 0xd7, 0x1e, 0x80, 0x36, 0x92, 0xcc, 0xc9 }
};

IMPLEMENT_DYNCREATE(CPrintToolsPropertyPage, CPropertyPage)

CPrintToolsPropertyPage::CPrintToolsPropertyPage(
   boolean_t is_overlay_options /*= FALSE*/)
   : CPropertyPage(CPrintToolsPropertyPage::IDD)
{
   // {{AFX_DATA_INIT(CPrintToolsPropertyPage)
   m_effective_scale = _T("");
   // }}AFX_DATA_INIT

   // TRUE when inside the Overlay Options dialog
   m_is_overlay_options = is_overlay_options;

   // assign default values to these public variables
   m_labeling_options = NULL;
   m_modified_labeling_options = FALSE;

   // create MapTypeSelection object for category and scale controls
   m_map_type = new MapTypeSelection(m_ctrlCategoryCombo, m_ctrlScaleCombo);

   if (m_is_overlay_options)
      initialize_single_page();
}

// initialize the single page property page
void CPrintToolsPropertyPage::initialize_single_page()
{
   CString value;
   MapSource source;
   MapScale scale;
   MapSeries series;

   // get source and scale from registry
   // if no value is in the registry, or the value there can not be parsed,
   // resort to a set of valid default values
   source = MAP_get_registry_source(
      "Print Options\\Single Page", "MapTypeSource", CADRG);
   scale = MAP_get_registry_scale(
      "Print Options\\Single Page", "MapTypeScale", ONE_TO_500K);
   series = MAP_get_registry_series(
      "Print Options\\Single Page", "MapTypeSeries", TPC_SERIES);

   // Setup dialog vars
   m_source = source;
   m_scale = scale;
   m_series = series;
   m_dAngle = 0.0;
   m_scale_percent = CPrinterPage::get_scale_percent(source, scale, series);
   m_projection_type = MAP_get_registry_projection("Print Options\\Single Page",
      "Projection", PROJ_get_default_projection_type());
}

BOOL CPrintToolsPropertyPage::OnApply()
{
   if (m_is_overlay_options)
   {
      // save source, scale, and series
      MAP_set_registry_source(
         "Print Options\\Single Page", "MapTypeSource", m_source);
      MAP_set_registry_scale(
         "Print Options\\Single Page", "MapTypeScale", m_scale);
      MAP_set_registry_series(
         "Print Options\\Single Page", "MapTypeSeries", m_series);

      MAP_set_registry_projection("Print Options\\Single Page", "Projection",
         m_projection_type);

      CPrinterPage::initialize_defaults();
      CPrintToolOverlay::apply_defaults_to_single_pages();
   }

   return TRUE;
}

CPrintToolsPropertyPage::~CPrintToolsPropertyPage()
{
   delete m_map_type;
}

void CPrintToolsPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   // {{AFX_DATA_MAP(CPrintToolsPropertyPage)
   DDX_Control(pDX, IDC_ZOOM, m_ctrlZoom);
   DDX_Control(pDX, IDC_PAGEPROP_MAP_SCALE, m_ctrlScaleCombo);
   DDX_Control(pDX, IDC_PAGEPROP_MAP_CATEGORY, m_ctrlCategoryCombo);
   DDX_Control(pDX, IDC_PROJECTION, m_ctrlProjectionCombo);
   DDX_Text(pDX, IDC_EFFECTIVE_SCALE, m_effective_scale);
   // }}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrintToolsPropertyPage, CPropertyPage)
   // {{AFX_MSG_MAP(CPrintToolsPropertyPage)
   ON_CBN_SELCHANGE(IDC_PAGEPROP_MAP_SCALE, OnSelchangeScaleCombo)
   ON_EN_KILLFOCUS(IDC_PAGEPROP_MAP_ROTATION, OnKillFocusAngle)
   ON_CBN_DROPDOWN(IDC_ZOOM, OnDropdownZoom)
   ON_CBN_KILLFOCUS(IDC_ZOOM, OnKillfocusZoom)
   ON_CBN_SELCHANGE(IDC_ZOOM, OnSelchangeZoom)
   ON_BN_CLICKED(IDC_LABELING_SINGLEPAGE, OnLabelingOptions)
   ON_CBN_SELCHANGE(IDC_PROJECTION, OnProjectionChange)
   // }}AFX_MSG_MAP
   ON_CBN_SELCHANGE(IDC_PAGEPROP_MAP_CATEGORY, OnSelchangeCategory)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

BOOL CPrintToolsPropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   MapCategory category = MAP_get_category(m_source);

   // initialize category list box
   m_map_type->set_category_list(category);

   // initialize scale list box
   m_map_type->set_scale_list(category, m_source, m_scale, m_series,
      ONE_TO_20M, MapScale(0.110, MapScale::METERS));

   if (m_is_overlay_options)
      m_scale_percent = CPrinterPage::get_scale_percent(
      m_source, m_scale, m_series);

   // initialize the projection type
   if (m_projection_type == EQUALARC_PROJECTION)
      m_ctrlProjectionCombo.SetCurSel(0);
   else if (m_projection_type == LAMBERT_PROJECTION)
      m_ctrlProjectionCombo.SetCurSel(1);
   else if (m_projection_type == MERCATOR_PROJECTION)
      m_ctrlProjectionCombo.SetCurSel(2);
   else if (m_projection_type == ORTHOGRAPHIC_PROJECTION)
      m_ctrlProjectionCombo.SetCurSel(3);
   else
      ERR_report(
      "Invalid projection type [CPrintToolsPropertyPage::OnInitDialog()]");

   // initialize zoom percent
   m_ctrlZoom.SetZoomPercent(m_scale_percent);
   m_ctrlZoom.SetRange(10, 400);
   update_effective_scale();

   // If we are dealing with overlay options then we want to hide the
   // rotation group
   if (m_is_overlay_options)
   {
      GetDlgItem(IDC_ROTATION_TEXT)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_ROTATION_GROUP)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_PAGEPROP_MAP_ROTATION)->ShowWindow(SW_HIDE);
   }
   // otherwise, format angle display
   else
   {
      CString s;
      s.Format("%05.1lf\260", m_dAngle);
      GetDlgItem(IDC_PAGEPROP_MAP_ROTATION)->SetWindowText(s);
   }

   // if there isn't a print overlay open we need to create the
   // static printer DC so that the page sizes will be correct
   if (m_is_overlay_options &&
      OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_PageLayout) == NULL &&
      CPrintToolOverlay::get_CreatePrinterDC_OK() == FALSE)
   {
      CWinApp* pApp = AfxGetApp();
      PRINTDLG pd;
      pApp->GetPrinterDeviceDefaults(&pd);
      pApp->CreatePrinterDC(CPrintToolOverlay::GetPrintDC());
   }

   // Now, format the paper size display
   CString sPageSize = CPrinterPage::get_printable_area_string();
   sPageSize += "  (printable area)";
   GetDlgItem(IDC_PAGEPROP_PAPER_SIZE)->SetWindowText(sPageSize);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintToolsPropertyPage::OnSelchangeScaleCombo()
{
   int map_index;
   MapCategory category;
   MapSource mapSource;
   MapScale mapScale;
   MapSeries mapSeries;

   if (m_map_type->get_selected_map(category, map_index,
      mapSource, mapScale, mapSeries))
   {
      if (map_index >= 0)
      {
         // if the map index is valid, save the map type and scale-percent
         m_source = mapSource;
         m_scale = mapScale;
         m_series = mapSeries;
         m_scale_percent = CPrinterPage::get_scale_percent(m_source,
            m_scale, m_series);
         m_ctrlZoom.SetZoomPercent(m_scale_percent);
         update_effective_scale();

         SetModified(TRUE);
      }
   }
}

void CPrintToolsPropertyPage::OnKillFocusAngle()
{
   CString s;

   // test rotation
   GetDlgItem(IDC_PAGEPROP_MAP_ROTATION)->GetWindowText(s);

   // If the angle typed in is invalid, just set it to 0
   m_dAngle = atof(s);
   if ((m_dAngle < 0.0) || (m_dAngle >= 360.0))
   {
      m_dAngle = 0.0;
   }

   // Now, format the display nicely
   s.Format("%05.1lf\260", m_dAngle);
   GetDlgItem(IDC_PAGEPROP_MAP_ROTATION)->SetWindowText(s);

   SetModified(TRUE);
}

void CPrintToolsPropertyPage::OnSelchangeCategory()
{
   MapCategory category;

   m_map_type->get_selected_category(category);
   m_map_type->set_scale_list(category, NULL_SOURCE, NULL_SCALE, NULL_SERIES,
      ONE_TO_20M, MapScale(0.110, MapScale::METERS));

   // update m_source, m_scale, and m_series
   OnSelchangeScaleCombo();
}

void CPrintToolsPropertyPage::OnDropdownZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CPrintToolsPropertyPage::OnKillfocusZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CPrintToolsPropertyPage::OnSelchangeZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CPrintToolsPropertyPage::OnProjectionChange()
{
   if (m_ctrlProjectionCombo.GetCurSel() == 0)
      m_projection_type = EQUALARC_PROJECTION;
   else if (m_ctrlProjectionCombo.GetCurSel() == 1)
      m_projection_type = LAMBERT_PROJECTION;
   else if (m_ctrlProjectionCombo.GetCurSel() == 2)
      m_projection_type = MERCATOR_PROJECTION;
   else if (m_ctrlProjectionCombo.GetCurSel() == 3)
      m_projection_type = ORTHOGRAPHIC_PROJECTION;

   SetModified(TRUE);
}

// get the scale percent from the ZoomComboBox
void CPrintToolsPropertyPage::retrieve_scale_percent()
{
   if (m_ctrlZoom.GetAndValidateZoomPercent(&m_scale_percent))
      m_ctrlZoom.SetZoomPercent(m_scale_percent);
}

void CPrintToolsPropertyPage::update_effective_scale()
{
   double d;

   // get the current scale's denominator
   MAP_scale_t_to_scale_denominator(m_scale, &d);

   // set the effective scale string 1:X where X is the current scale denom
   // times the zoom percent (which is stored as an integer so we divide by
   // a hundred)
   const int effective = static_cast<int>(d * (100.0/m_scale_percent) + 0.5);

   m_effective_scale = MAP_convert_scale_denominator_to_scale_string(effective);


   // update the dialog with the modified effective scale string
   UpdateData(FALSE);

   // if we are dealing with Overlay Options then save the effective
   // scale to the registry
   if (m_is_overlay_options)
   {
      // save the default scale percent for the current map type
      CPrinterPage::set_scale_percent(m_source, m_scale, m_series,
         m_scale_percent);
   }
}


IMPLEMENT_DYNAMIC(CPrintToolsPropertiesSheet, CPropertySheet)

CPrintToolsPropertiesSheet::CPrintToolsPropertiesSheet(UINT nIDCaption,
CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPrintToolsPropertiesSheet::CPrintToolsPropertiesSheet(LPCTSTR pszCaption,
   CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPrintToolsPropertiesSheet::~CPrintToolsPropertiesSheet()
{
}

void CPrintToolsPropertiesSheet::ApplyNow()
{
   CPrintToolsPropertyPage *page =
      reinterpret_cast<CPrintToolsPropertyPage *>(GetPage(0));
   double rotation = 360.0 - page->m_dAngle;
   if (rotation == 360.0)
      rotation = 0.0;

   // if there is no change, do nothing
   if (m_pPrinterPage->get_source() == page->m_source &&
      m_pPrinterPage->get_scale() == page->m_scale &&
      m_pPrinterPage->get_series() == page->m_series &&
      m_pPrinterPage->get_rotation() == rotation &&
      m_pPrinterPage->get_scale_percent() == page->m_scale_percent &&
      !page->m_modified_labeling_options &&
      m_pPrinterPage->get_projection_type() == page->m_projection_type)
   {
      page->SetModified(FALSE);
      return;
   }

   // make sure the settings are valid for the surface size of the current
   // printer page
   if (prn_settings_violate_limits(
      page->m_source, page->m_scale, page->m_series,
      m_pPrinterPage->get_center().lat, m_pPrinterPage->get_center().lon,
      rotation, m_pPrinterPage->GetPageWidthInInches(),
      m_pPrinterPage->GetPageHeightInInches(), page->m_scale_percent))
      return;

   // apply changes to the selected page
   m_pPrinterPage->set_map_type(page->m_source, page->m_scale, page->m_series,
      page->m_projection_type);
   m_pPrinterPage->set_rotation(rotation);
   m_pPrinterPage->set_scale_percent(page->m_scale_percent);
   m_pPrinterPage->set_labeling_options(page->m_labeling_options);
   m_pPrinterPage->Calc();

   // this overlay has been modified
   m_pPrinterPage->get_parent()->set_modified(TRUE);

   // update the display - should only invalidate this page
   OVL_get_overlay_manager()->InvalidateOverlay(m_pPrinterPage->m_overlay);

   page->SetModified(FALSE);
}

BEGIN_MESSAGE_MAP(CPrintToolsPropertiesSheet, CPropertySheet)
   // {{AFX_MSG_MAP(CPrintToolsPropertiesSheet)
   ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
   // }}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPrintToolsPropertiesSheet::OnApplyNow()
{
   ApplyNow();
}

BOOL CPrintToolsPropertyPage::OnSetActive()
{
   // re read the scale percent and update the dialog
   if (m_is_overlay_options)
   {
      m_scale_percent = CPrinterPage::get_scale_percent(m_source, m_scale,
         m_series);
      m_ctrlZoom.SetZoomPercent(m_scale_percent);

      update_effective_scale();
   }

   return CPropertyPage::OnSetActive();
}

void CPrintToolsPropertyPage::OnLabelingOptions()
{
   CPrintOptDlg dlg("Use Alternate Dialog (Removes Map Area Options group");
   CLabelingOptions labeling_options;

   // if available, use options from specific page
   if (m_labeling_options)
      labeling_options = *m_labeling_options;
   else
      labeling_options.initialize_from_registry("Print Options");

   dlg.set_labeling_options(&labeling_options);

   if (dlg.DoModal() == IDOK)
   {
      dlg.get_labeling_options(&labeling_options);

      // if options from specific page are available, change them
      if (m_labeling_options)
         *m_labeling_options = labeling_options;

      // save these values as the defaults - regardless
      labeling_options.save_in_registry("Print Options");

      m_modified_labeling_options = TRUE;

      SetModified(TRUE);
   }
}


LRESULT CPrintToolsPropertyPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}
