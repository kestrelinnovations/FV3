// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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



//  ACProp.cpp
//  Implementation of classes for the edit propeties dialog for strip charts in
// the printing tools overlay.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/ACProp.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/printdlg.h"                    // CPrintOptDlg
#include "FalconView/getobjpr.h"
#include "FalconView/include/ovl_mgr.h"

// this project's headers
#include "FalconView/PrintToolOverlay/factory.h"
#include "FalconView/PrintToolOverlay/PageProp.h"   // CPrintToolsPropertyPage
#include "FalconView/PrintToolOverlay/surf_lim.h"   // prn_settings_violate_limits
#include "FalconView/include/MapTpSel.h"            // MapTypeSelection

// {88EA5D83-884D-4345-BA39-E56542624E7A}
const GUID PROPPAGEID_PageLayout_AreaChart =
{
   0x88ea5d83,
   0x884d,
   0x4345,
   { 0xba, 0x39, 0xe5, 0x65, 0x42, 0x62, 0x4e, 0x7a }
};


IMPLEMENT_DYNCREATE(CAreaChartPropertyPage, CPropertyPage)

CAreaChartPropertyPage::CAreaChartPropertyPage(
boolean_t is_overlay_options /*= FALSE*/)
   : CPropertyPage(CAreaChartPropertyPage::IDD)
{
   // {{AFX_DATA_INIT(CAreaChartPropertyPage)
   m_effective_scale = _T("");
   m_dOverlap = 0.0;
   // }}AFX_DATA_INIT

   // TRUE when inside the Overlay Options dialog
   m_is_overlay_options = is_overlay_options;

   // assign default values to these public variables
   m_labeling_options = NULL;
   m_modified_labeling_options = FALSE;

   // create MapTypeSelection object for category and scale controls
   m_map_type = new MapTypeSelection(m_ctrlCategoryCombo, m_ctrlScaleCombo);

   if (m_is_overlay_options)
      initialize_area_chart();
}

// initialize the area chart property page
void CAreaChartPropertyPage::initialize_area_chart()
{
   CString value;
   MapSource source;
   MapScale scale;
   MapSeries series;
   double overlap;

   // get source and scale from registry
   // if no value is in the registry, or the value there can not be parsed,
   // resort to a set of valid default values
   source = MAP_get_registry_source(
      "Print Options\\Area Chart", "MapTypeSource", CADRG);
   scale = MAP_get_registry_scale(
      "Print Options\\Area Chart", "MapTypeScale", ONE_TO_500K);
   series = MAP_get_registry_series(
      "Print Options\\Area Chart", "MapTypeSeries", TPC_SERIES);

   // get overlap from registry
   value = PRM_get_registry_string("Print Options\\Area Chart",
      "Overlap", "0.25");
   overlap = atof(value);

   // Setup dialog vars
   m_source = source;
   m_scale = scale;
   m_series = series;
   m_projection_type = MAP_get_registry_projection("Print Options\\Area Chart",
      "Projection", PROJ_get_default_projection_type());
   m_dOverlap = overlap;
   m_scale_percent = CPrinterPage::get_scale_percent(source, scale, series);
}

BOOL CAreaChartPropertyPage::OnApply()
{
   if (m_is_overlay_options)
   {
      // save source, scale, and series
      MAP_set_registry_source(
         "Print Options\\Area Chart", "MapTypeSource", m_source);
      MAP_set_registry_scale(
         "Print Options\\Area Chart", "MapTypeScale", m_scale);
      MAP_set_registry_series(
         "Print Options\\Area Chart", "MapTypeSeries", m_series);

      // save overlap
      CString value;
      value.Format("%0.6lf", m_dOverlap);
      PRM_set_registry_string("Print Options\\Area Chart", "Overlap", value);

      MAP_set_registry_projection("Print Options\\Area Chart", "Projection",
         m_projection_type);

      CAreaChart::initialize_defaults();
      CPrintToolOverlay::apply_defaults_to_area_charts();
   }

   return TRUE;
}

CAreaChartPropertyPage::~CAreaChartPropertyPage()
{
   delete m_map_type;
}

void CAreaChartPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   // {{AFX_DATA_MAP(CAreaChartPropertyPage)
   DDX_Control(pDX, IDC_ZOOM, m_ctrlZoom);
   DDX_Control(pDX, IDC_AREAPROP_MAP_SCALE, m_ctrlScaleCombo);
   DDX_Control(pDX, IDC_AREAPROP_MAP_CATEGORY, m_ctrlCategoryCombo);
   DDX_Text(pDX, IDC_EFFECTIVE_SCALE, m_effective_scale);
   DDX_Text(pDX, IDC_PAGE_OVERLAP, m_dOverlap);
   DDX_Control(pDX, IDC_PROJECTION, m_ctrlProjectionCombo);
   // }}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAreaChartPropertyPage, CPropertyPage)
   // {{AFX_MSG_MAP(CAreaChartPropertyPage)
   ON_CBN_SELCHANGE(IDC_AREAPROP_MAP_SCALE, OnSelchangeScaleCombo)
   ON_CBN_CLOSEUP(IDC_ZOOM, OnCloseupZoom)
   ON_CBN_DROPDOWN(IDC_ZOOM, OnDropdownZoom)
   ON_CBN_KILLFOCUS(IDC_ZOOM, OnKillfocusZoom)
   ON_CBN_SETFOCUS(IDC_ZOOM, OnSetfocusZoom)
   ON_CBN_SELCHANGE(IDC_ZOOM, OnSelchangeZoom)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
   ON_EN_KILLFOCUS(IDC_PAGE_OVERLAP, OnKillFocusPageOverlap)
   ON_BN_CLICKED(IDC_LABELING_AREACHART, OnLabelingOptions)
   ON_CBN_SELCHANGE(IDC_PROJECTION, OnProjectionChange)
   // }}AFX_MSG_MAP
   ON_CBN_SELCHANGE(IDC_AREAPROP_MAP_CATEGORY, OnSelchangeCategory)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

BOOL CAreaChartPropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   MapCategory category = MAP_get_category(m_source);

   // initialize category list box
   m_map_type->set_category_list(category);

   // initialize scale list box
   m_map_type->set_scale_list(category, m_source, m_scale, m_series,
      ONE_TO_20M, MapScale(0.110, MapScale::METERS));

   // initialize the projection type
   if (m_projection_type == EQUALARC_PROJECTION)
      m_ctrlProjectionCombo.SetCurSel(0);
   else if (m_projection_type == LAMBERT_PROJECTION)
      m_ctrlProjectionCombo.SetCurSel(1);
   else
      ERR_report(
      "Invalid projection type [CAreaChartPropertyPage::OnInitDialog()]");

   if (m_is_overlay_options)
      m_scale_percent = CPrinterPage::get_scale_percent(m_source, m_scale,
      m_series);

   // initialize zoom percent
   m_ctrlZoom.SetZoomPercent(m_scale_percent);
   m_ctrlZoom.SetRange(10, 400);
   update_effective_scale();

   // if there isn't a print overlay open we need to create the
   // static printer DC so that the page sizes will be correct in
   // the property pages
   if (m_is_overlay_options &&
      OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_PageLayout)
         == nullptr &&
      CPrintToolOverlay::get_CreatePrinterDC_OK() == FALSE)
   {
      CWinApp* pApp = AfxGetApp();
      PRINTDLG pd;
      pApp->GetPrinterDeviceDefaults(&pd);
      pApp->CreatePrinterDC(CPrintToolOverlay::GetPrintDC());
   }

   // takes care of setting overlap value
   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void CAreaChartPropertyPage::OnSelchangeScaleCombo()
{
   int map_index;
   MapCategory category;
   MapSource mapSource;
   MapScale mapScale;
   MapSeries mapSeries;

   if (m_map_type->get_selected_map(
      category, map_index, mapSource, mapScale, mapSeries))
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

void CAreaChartPropertyPage::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN*>(pNMHDR);

   double max_overlap = CPrinterPage::GetMaxPageOverlapInInches();

   if (pNMUpDown->iDelta < 0 && m_dOverlap <= max_overlap - 0.25)
   {
      SetModified(TRUE);

      m_dOverlap += 0.25;
   }
   else if (pNMUpDown->iDelta > 0 && m_dOverlap >= 0.25)
   {
      SetModified(TRUE);

      m_dOverlap -= 0.25;
   }

   UpdateData(FALSE);
   *pResult = 0;
}

void CAreaChartPropertyPage::OnKillFocusPageOverlap()
{
   // retrieve the data from the dialog into the member variables
   UpdateData();

   // get the maximum page overlap which is half the virtual page
   // width or height which ever is smaller
   double max_overlap = CPrinterPage::GetMaxPageOverlapInInches();

   if (m_dOverlap > max_overlap)
   {
      CString s;
      m_dOverlap = max_overlap;

      // tell the user they cannot set the overlap so high
      s.Format("Page overlap cannot exceed %2.2lf inches.\n"
         "Page Width: %2.2lf\nPage Height: %2.2lf",
         max_overlap,
         CPrinterPage::GetPageWidthInInches(),
         CPrinterPage::GetPageHeightInInches());
      AfxMessageBox(s);

      // put the modified overlap back into the dialog box
      UpdateData(FALSE);
   }
   if (m_dOverlap < 0.0)
   {
      m_dOverlap = 0.0;
      UpdateData(FALSE);
   }

   SetModified(TRUE);
}

void CAreaChartPropertyPage::OnSelchangeCategory()
{
   MapCategory category;

   m_map_type->get_selected_category(category);
   m_map_type->set_scale_list(category, NULL_SOURCE, NULL_SCALE, NULL_SERIES,
      ONE_TO_20M, MapScale(0.110, MapScale::METERS));

   // update m_source and m_scale
   OnSelchangeScaleCombo();
}

void CAreaChartPropertyPage::OnDropdownZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CAreaChartPropertyPage::OnKillfocusZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CAreaChartPropertyPage::OnSetfocusZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CAreaChartPropertyPage::OnCloseupZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CAreaChartPropertyPage::OnSelchangeZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

// get the scale percent from the ZoomComboBox
void CAreaChartPropertyPage::retrieve_scale_percent()
{
   if (m_ctrlZoom.GetAndValidateZoomPercent(&m_scale_percent))
      m_ctrlZoom.SetZoomPercent(m_scale_percent);
}

void CAreaChartPropertyPage::update_effective_scale()
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

IMPLEMENT_DYNAMIC(CAreaChartProptertiesSheet, CPropertySheet)

CAreaChartProptertiesSheet::CAreaChartProptertiesSheet(
   UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CAreaChartProptertiesSheet::CAreaChartProptertiesSheet(
   LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CAreaChartProptertiesSheet::~CAreaChartProptertiesSheet()
{
}

void CAreaChartProptertiesSheet::ApplyNow()
{
   CAreaChartPropertyPage* pPropPage =
      reinterpret_cast<CAreaChartPropertyPage*>(GetPage(0));

   // if there is no change, do nothing
   if (m_pChart->get_source() == pPropPage->m_source &&
      m_pChart->get_scale() == pPropPage->m_scale &&
      m_pChart->get_series() == pPropPage->m_series &&
      m_pChart->get_overlap() == pPropPage->m_dOverlap &&
      m_pChart->get_scale_percent() == pPropPage->m_scale_percent &&
      m_pChart->get_projection_type() == pPropPage->m_projection_type &&
      !pPropPage->m_modified_labeling_options)
   {
      pPropPage->SetModified(FALSE);
      return;
   }

   d_geo_t center;
   if (m_pChart->get_center(&center) == false)
   {
      ERR_report("Undefined center.");
      return;
   }

   // make sure the settings are valid for the surface size of the current
   // printer page.  Note area charts are always North Up
   if (prn_settings_violate_limits(pPropPage->m_source, pPropPage->m_scale,
      pPropPage->m_series, center.lat, center.lon, 0.0,
      CPrinterPage::GetPageWidthInInches(),
      CPrinterPage::GetPageHeightInInches(),
      pPropPage->m_scale_percent))
      return;

   // backup defaults before changing them
   MapSource source = m_pChart->get_source();
   MapScale  scale = m_pChart->get_scale();
   MapSeries series = m_pChart->get_series();
   ProjectionEnum projection_type = m_pChart->get_projection_type();
   int          percent = m_pChart->get_scale_percent();
   double       overlap = m_pChart->get_overlap();

   // try setting map type
   if (m_pChart->set_map_type(pPropPage->m_source, pPropPage->m_scale,
      pPropPage->m_series, pPropPage->m_projection_type) != SUCCESS)
   {
      ERR_report("set_map_type() failed.");
      return;
   }

   if (m_pChart->set_scale_percent(pPropPage->m_scale_percent) !=
      SUCCESS)
   {
      ERR_report("set_scale_percent() failed.");
      return;
   }

   // try setting overlap
   if (m_pChart->set_overlap(pPropPage->m_dOverlap) != SUCCESS)
   {
      ERR_report("set_overlap() failed.");
      return;
   }

   // try to rebuild the area chart based on the new properties
   if (m_pChart->ReBuild(true, true, true) == 0)
   {
      // in case of failure, revert to original values
      pPropPage->m_source = source;
      pPropPage->m_scale = scale;
      pPropPage->m_series = series;
      pPropPage->m_dOverlap = overlap;
      pPropPage->m_projection_type = projection_type;
      m_pChart->set_map_type(source, scale, series, projection_type);
      m_pChart->set_scale_percent(percent);
      m_pChart->set_overlap(overlap);
      m_pChart->ReBuild(true, false, true);
   }
   else
   {
      // this overlay has been modified
      m_pChart->get_parent()->set_modified(TRUE);

      // store the labeling options
      m_pChart->set_labeling_options(pPropPage->m_labeling_options);
   }

   OVL_get_overlay_manager()->InvalidateOverlay(m_pChart->m_overlay);

   pPropPage->SetModified(FALSE);
}

BEGIN_MESSAGE_MAP(CAreaChartProptertiesSheet, CPropertySheet)
   // {{AFX_MSG_MAP(CAreaChartProptertiesSheet)
   ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
   // }}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAreaChartProptertiesSheet::OnApplyNow()
{
   ApplyNow();
}

BOOL CAreaChartPropertyPage::OnSetActive()
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

void CAreaChartPropertyPage::OnLabelingOptions()
{
   CPrintOptDlg dlg("Use Alternate Dialog (Removes Map Area Options group");
   CLabelingOptions labeling_options;

   // if available, use options from specific area chart
   if (m_labeling_options)

      labeling_options = *m_labeling_options;
   else
      labeling_options.initialize_from_registry("Print Options");

   dlg.set_labeling_options(&labeling_options);

   if (dlg.DoModal() == IDOK)
   {
      dlg.get_labeling_options(&labeling_options);

      // if options from specific area chart are available, change them
      if (m_labeling_options)
         *m_labeling_options = labeling_options;

      // save these values as the defaults - regardless
      labeling_options.save_in_registry("Print Options");

      m_modified_labeling_options = TRUE;

      SetModified(TRUE);
   }
}

void CAreaChartPropertyPage::OnProjectionChange()
{
   if (m_ctrlProjectionCombo.GetCurSel() == 0)
      m_projection_type = EQUALARC_PROJECTION;
   else if (m_ctrlProjectionCombo.GetCurSel() == 1)
      m_projection_type = LAMBERT_PROJECTION;

   SetModified(TRUE);
}

LRESULT CAreaChartPropertyPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}
