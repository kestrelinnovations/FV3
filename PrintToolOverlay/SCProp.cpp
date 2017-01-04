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

// SCProp.cpp
// Implementation of classes for the edit propeties dialog for strip charts in
// the printing tools overlay.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/SCProp.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/getobjpr.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/MapTpSel.h"  // MapTypeSelection
#include "FalconView/printdlg.h"  // CPrintOptDlg

// this project's headers
#include "FalconView/PrintToolOverlay/factory.h"
#include "FalconView/PrintToolOverlay/PageProp.h"  // CPrintToolsPropertyPage
#include "FalconView/PrintToolOverlay/surf_lim.h"
#include "FalconView/PrintToolOverlay/layout.h"

// {DEFEB40E-1F92-4F17-BCDA-0304FC611C06}
const GUID PROPPAGEID_PageLayout_StripChart =
{
   0xdefeb40e, 0x1f92, 0x4f17,
   { 0xbc, 0xda, 0x3, 0x4, 0xfc, 0x61, 0x1c, 0x6 }
};

IMPLEMENT_DYNCREATE(CStripChartPropertyPage, CPropertyPage)

CStripChartPropertyPage::CStripChartPropertyPage(boolean_t is_overlay_options
/*= FALSE*/)
   : CPropertyPage(CStripChartPropertyPage::IDD)
{
   // {{AFX_DATA_INIT(CStripChartPropertyPage)
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
      initialize_strip_chart();
}

// initialize the strip chart property page
void CStripChartPropertyPage::initialize_strip_chart()
{
   CString value;
   MapSource source;
   MapScale scale;
   MapSeries series;
   double overlap;
   CStripChart::eOrientation orientation;

   // get source and scale from registry
   // if no value is in the registry, or the value there can not be parsed,
   // resort to a set of valid default values
   source = MAP_get_registry_source(
      "Print Options\\Strip Chart", "MapTypeSource", CADRG);
   scale = MAP_get_registry_scale(
      "Print Options\\Strip Chart", "MapTypeScale", ONE_TO_500K);
   series = MAP_get_registry_series(
      "Print Options\\Strip Chart", "MapTypeSeries", TPC_SERIES);

   // overlap from the registry
   value = PRM_get_registry_string("Print Options\\Strip Chart",
      "Overlap", "1.0");
   overlap = atof(value);

   // get orientation from the registry
   orientation = (CStripChart::eOrientation)PRM_get_registry_int(
      "Print Options\\Strip Chart",
      "Orientation", CStripChart::FollowRoute);

   // Setup dialog vars
   m_nNorthUp = (orientation == CStripChart::NorthUp);
   m_source = source;
   m_scale = scale;
   m_series = series;
   m_dOverlap = overlap;
   m_scale_percent = CPrinterPage::get_scale_percent(source, scale, series);
   m_projection_type = MAP_get_registry_projection("Print Options\\Strip Chart",
      "Projection", PROJ_get_default_projection_type());
}

BOOL CStripChartPropertyPage::OnApply()
{
   if (m_is_overlay_options)
   {
      // save source, scale, and series
      MAP_set_registry_source(
         "Print Options\\Strip Chart", "MapTypeSource", m_source);
      MAP_set_registry_scale(
         "Print Options\\Strip Chart", "MapTypeScale", m_scale);
      MAP_set_registry_series(
         "Print Options\\Strip Chart", "MapTypeSeries", m_series);

      // save overlap
      CString value;
      value.Format("%0.6lf", m_dOverlap);
      PRM_set_registry_string("Print Options\\Strip Chart", "Overlap", value);

      // save orientation
      if (m_nNorthUp)
         PRM_set_registry_int("Print Options\\Strip Chart",
         "Orientation", CStripChart::NorthUp);
      else
         PRM_set_registry_int("Print Options\\Strip Chart",
         "Orientation", CStripChart::FollowRoute);

      // save projection type
      MAP_set_registry_projection("Print Options\\Strip Chart", "Projection",
         m_projection_type);

      CStripChart::initialize_defaults();
      CPrintToolOverlay::apply_defaults_to_strip_charts();
   }

   return TRUE;
}

CStripChartPropertyPage::~CStripChartPropertyPage()
{
   delete m_map_type;
}

void CStripChartPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   // {{AFX_DATA_MAP(CStripChartPropertyPage)
   DDX_Control(pDX, IDC_ZOOM, m_ctrlZoom);
   DDX_Control(pDX, IDC_STRIPPROP_MAP_SCALE, m_ctrlScaleCombo);
   DDX_Control(pDX, IDC_STRIPPROP_MAP_CATEGORY, m_ctrlCategoryCombo);
   DDX_Control(pDX, IDC_PROJECTION, m_ctrlProjectionCombo);
   DDX_Text(pDX, IDC_EFFECTIVE_SCALE, m_effective_scale);
   DDX_Text(pDX, IDC_PAGE_OVERLAP, m_dOverlap);
   // }}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStripChartPropertyPage, CPropertyPage)
   // {{AFX_MSG_MAP(CStripChartPropertyPage)
   ON_BN_CLICKED(IDC_SCPROP_NORTHUP, OnNorthUpClicked)
   ON_BN_CLICKED(IDC_SCPROP_FOLLOW, OnFollowRouteClicked)
    ON_CBN_SELCHANGE(IDC_STRIPPROP_MAP_SCALE, OnSelchangeScaleCombo)
   ON_CBN_DROPDOWN(IDC_ZOOM, OnDropdownZoom)
   ON_CBN_KILLFOCUS(IDC_ZOOM, OnKillfocusZoom)
   ON_CBN_SETFOCUS(IDC_ZOOM, OnSetfocusZoom)
   ON_CBN_CLOSEUP(IDC_ZOOM, OnCloseupZoom)
   ON_CBN_SELCHANGE(IDC_ZOOM, OnSelchangeZoom)
   ON_EN_KILLFOCUS(IDC_PAGE_OVERLAP, OnKillFocusPageOverlap)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
   ON_BN_CLICKED(IDC_VIRTUAL_PAGE_SETUP, OnVPageSetup)
   ON_BN_CLICKED(IDC_LABELING_STRIPCHART, OnLabelingOptions)
   ON_CBN_SELCHANGE(IDC_PROJECTION, OnProjectionChange)
   // }}AFX_MSG_MAP
   ON_CBN_SELCHANGE(IDC_STRIPPROP_MAP_CATEGORY, OnSelchangeCategory)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

BOOL CStripChartPropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   MapCategory category = MAP_get_category(m_source);

   // initialize category list box
   m_map_type->set_category_list(category);

   // initialize scale list box
   m_map_type->set_scale_list(category, m_source, m_scale, m_series,
      ONE_TO_20M, MapScale(0.110, MapScale::METERS));

   if (m_is_overlay_options)
      m_scale_percent = CPrinterPage::get_scale_percent(m_source, m_scale,
         m_series);

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

   // Setup radio buttons
   reinterpret_cast<CButton*>(GetDlgItem(IDC_SCPROP_FOLLOW))->
      SetCheck(!m_nNorthUp);
   reinterpret_cast<CButton*>(GetDlgItem(IDC_SCPROP_NORTHUP))->
      SetCheck(m_nNorthUp);

   // if there isn't a print overlay open we need to create the
   // static printer DC so that the page sizes will be correct in
   // the property pages
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

   // takes care of setting overlap value
   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CStripChartPropertyPage::OnFollowRouteClicked()
{
   m_nNorthUp = FALSE;

   SetModified(TRUE);
}

void CStripChartPropertyPage::OnNorthUpClicked()
{
   m_nNorthUp = TRUE;

   SetModified(TRUE);
}

void CStripChartPropertyPage::OnSelchangeScaleCombo()
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
         m_scale_percent = CPrinterPage::get_scale_percent(m_source, m_scale,
            m_series);
         m_ctrlZoom.SetZoomPercent(m_scale_percent);
         update_effective_scale();

         SetModified(TRUE);
      }
   }
}

void CStripChartPropertyPage::OnKillFocusPageOverlap()
{
   // retrieve the data from the dialog into the member variables
   UpdateData();

   // get the maximum virtual page overlap which is half the virtual page
   // width or height which ever is smaller
   CPrintToolOverlay *prnt_ovl = CPrintToolOverlay::GetActiveOverlay();
   if (prnt_ovl == NULL)
      return;

   double max_overlap = prnt_ovl->GetMaxVirtualPageOverlapInInches();

   if (m_dOverlap > max_overlap)
   {
      CString s;
      m_dOverlap = max_overlap;

      // tell the user they cannot set the overlap so high
      s.Format("Page overlap cannot exceed %2.2lf inches.\n"
         "Virtual Page Width: %2.2lf\nVirtual Page Height: %2.2lf",
         max_overlap,
         prnt_ovl->GetVirtualPageWidthInInches(),
         prnt_ovl->GetVirtualPageHeightInInches());
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


void CStripChartPropertyPage::OnSelchangeCategory()
{
   MapCategory category;

   m_map_type->get_selected_category(category);
   m_map_type->set_scale_list(category, NULL_SOURCE, NULL_SCALE, NULL_SERIES,
      ONE_TO_20M, MapScale(0.110, MapScale::METERS));

   // update m_source and m_scale
   OnSelchangeScaleCombo();
}

void CStripChartPropertyPage::OnDropdownZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CStripChartPropertyPage::OnKillfocusZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CStripChartPropertyPage::OnSetfocusZoom()
{
}

void CStripChartPropertyPage::OnCloseupZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CStripChartPropertyPage::OnSelchangeZoom()
{
   retrieve_scale_percent();
   update_effective_scale();

   SetModified(TRUE);
}

void CStripChartPropertyPage::OnProjectionChange()
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
void CStripChartPropertyPage::retrieve_scale_percent()
{
   if (m_ctrlZoom.GetAndValidateZoomPercent(&m_scale_percent))
      m_ctrlZoom.SetZoomPercent(m_scale_percent);
}

void CStripChartPropertyPage::update_effective_scale()
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

void CStripChartPropertyPage::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
   double max_overlap;
   NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN*>(pNMHDR);
   CPrintToolOverlay *prnt_ovl = CPrintToolOverlay::GetActiveOverlay();

   if (prnt_ovl == NULL)
   {
      CVirtualPageLayout vpage_layout;
      vpage_layout.initialize_from_registry(
         "Print Options\\Virtual Page Layout");

      max_overlap = __min(vpage_layout.get_virtual_page_width(),
         vpage_layout.get_virtual_page_height());
   }
   else
      max_overlap = prnt_ovl->GetMaxVirtualPageOverlapInInches();

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

void CStripChartPropertyPage::OnVPageSetup()
{
   CVirtualPageLayout vpage_layout;
   CPageLayoutDlg dlg;

   // If there is currently a page layout overlay opened in the Page Layout
   // Tool, use its virtual page settings
   CPrintToolOverlay *overlay = CPrintToolOverlay::GetActiveOverlay();
   if (overlay)
   {
      dlg.set_virtual_page_layout(*(overlay->get_virtual_page_layout()));
   }
   // otherwise, a print overlay is not opened.  This could happen when
   // we are editing properties in the Overlay Options - Print Tool tab.
   // In this case, read the virtual page layout from the registry and
   // manipulate this object instead.
   else
   {
      vpage_layout.initialize_from_registry(
         "Print Options\\Virtual Page Layout");
      dlg.set_virtual_page_layout(vpage_layout);
   }

   if (dlg.DoModal() == IDOK)
   {
      vpage_layout = dlg.get_virtual_page_layout();

      // Make sure the settings are valid for the surface size of the current
      // printer page.  Limits are determined by the map center and rotation,
      // as well as the scale and surface size.  Since each strip chart page
      // has its own center and rotation, to worst case is used as a test.
      // Note longitude does not matter.
      if (prn_settings_violate_limits(m_source, m_scale, m_series,
         80.0, 0.0, 45.0, vpage_layout.get_virtual_page_width(),
         vpage_layout.get_virtual_page_height(), m_scale_percent))
         return;

      // store the values of the page layout in the registry
      vpage_layout.save_to_registry("Print Options\\Virtual Page Layout");

      // If there is currently a page layout overlay opened in the Page Layout
      // Tool, the change will be applied to all strip charts in that overlay.
      if (overlay)
      {
         // update the options in the current page layout overlay
         overlay->set_virtual_page_layout(vpage_layout);

         // erase, rebuild, and draw all strip charts in this overlay
         overlay->update_all_strip_charts();
      }
   }
}

IMPLEMENT_DYNAMIC(CStripChartPropertiesSheet, CPropertySheet)

CStripChartPropertiesSheet::CStripChartPropertiesSheet(UINT nIDCaption,
CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CStripChartPropertiesSheet::CStripChartPropertiesSheet(LPCTSTR pszCaption,
   CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CStripChartPropertiesSheet::~CStripChartPropertiesSheet()
{
}

void CStripChartPropertiesSheet::ApplyNow()
{
   CStripChartPropertyPage* pPropPage =
      reinterpret_cast<CStripChartPropertyPage*>(GetPage(0));

   // if there is no change, do nothing
   if (CStripChart::m_nDefaultSource == pPropPage->m_source &&
      CStripChart::m_nDefaultScale == pPropPage->m_scale &&
      CStripChart::m_nDefaultSeries == pPropPage->m_series &&
      CStripChart::m_dDefaultOverlap == pPropPage->m_dOverlap &&
      ((m_pChart->GetOrientation() == CStripChart::NorthUp) ==
         pPropPage->m_nNorthUp) &&
      (CPrinterPage::get_scale_percent(pPropPage->m_source, pPropPage->m_scale,
      pPropPage->m_series) == pPropPage->m_scale_percent) &&
      !pPropPage->m_modified_labeling_options &&
      CStripChart::m_DefaultProjectionType == pPropPage->m_projection_type)
   {
      pPropPage->SetModified(FALSE);
      return;
   }

   // need parent overlay to get the virtual page size
   CPrintToolOverlay *parent = m_pChart->get_parent();
   if (parent == NULL)
   {
      ERR_report("NULL parent.");
      return;
   }

   // Make sure the settings are valid for the surface size of the current
   // printer page.  Limits are determined by the map center and rotation, as
   // well as the scale and surface size.  Since each strip chart page has its
   // own center and rotation, to worst case is used as a test.  Note longitude
   // does not matter.
   if (prn_settings_violate_limits(pPropPage->m_source, pPropPage->m_scale,
      pPropPage->m_series, 80.0, 0.0, 45.0,
      parent->GetVirtualPageWidthInInches(),
      parent->GetVirtualPageHeightInInches(), pPropPage->m_scale_percent))
      return;

   // backup defaults before changing them
   CStripChart::eOrientation orientation = m_pChart->GetOrientation();
   MapSource source = CStripChart::m_nDefaultSource;
   MapScale scale = CStripChart::m_nDefaultScale;
   MapSeries series = CStripChart::m_nDefaultSeries;
   double overlap = CStripChart::m_dDefaultOverlap;
   int percent = CPrinterPage::get_scale_percent(pPropPage->m_source,
      pPropPage->m_scale, pPropPage->m_series);
   ProjectionEnum proj_type = CStripChart::m_DefaultProjectionType;

   // Defaults are used for all other parameters
   CStripChart::m_nDefaultSource = pPropPage->m_source;
   CStripChart::m_nDefaultScale = pPropPage->m_scale;
   CStripChart::m_nDefaultSeries = pPropPage->m_series;
   CStripChart::m_dDefaultOverlap = pPropPage->m_dOverlap;
   CStripChart::m_DefaultProjectionType = pPropPage->m_projection_type;

   // orientation is set on a per strip chart basis
   if (pPropPage->m_nNorthUp)
   {
      m_pChart->SetOrientation(CStripChart::NorthUp);
   }
   else
   {
      m_pChart->SetOrientation(CStripChart::FollowRoute);
   }

   // save the default scale percent for the current map type
   CPrinterPage::set_scale_percent(pPropPage->m_source, pPropPage->m_scale,
      pPropPage->m_series, pPropPage->m_scale_percent);

   // try to rebuild the strip chart based on the new properties
   if (m_pChart->ReBuild(true, true) == 0)
   {
      // in case of failure revert to original values
      CStripChart::m_nDefaultSource = source;
      CStripChart::m_nDefaultScale = scale;
      CStripChart::m_nDefaultSeries = series;
      CStripChart::m_dDefaultOverlap = overlap;
      CStripChart::m_DefaultProjectionType = proj_type;
      m_pChart->SetOrientation(orientation);
      CPrinterPage::set_scale_percent(pPropPage->m_source, pPropPage->m_scale,
         pPropPage->m_series, percent);
      m_pChart->ReBuild(true, false);
   }
   else
   {
      // set only default value not already set
      CStripChart::m_DefaultOrientation = m_pChart->GetOrientation();

      // save the defaults in the registry
      CStripChart::save_defaults();

      // this overlay has been modified
      m_pChart->get_parent()->set_modified(TRUE);

      // store the labeling options
      m_pChart->set_labeling_options(pPropPage->m_labeling_options);
   }

   // update the display - should only invalidate this strip chart
   OVL_get_overlay_manager()->InvalidateOverlay(m_pChart->m_overlay);

   pPropPage->SetModified(FALSE);
}

BEGIN_MESSAGE_MAP(CStripChartPropertiesSheet, CPropertySheet)
   // {{AFX_MSG_MAP(CStripChartPropertiesSheet)
   ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
   // }}AFX_MSG_MAP
END_MESSAGE_MAP()

void CStripChartPropertiesSheet::OnApplyNow()
{
   ApplyNow();
}

BOOL CStripChartPropertyPage::OnSetActive()
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

void CStripChartPropertyPage::OnLabelingOptions()
{
   CPrintOptDlg dlg("Use Alternate Dialog (Removes Map Area Options group");
   CLabelingOptions labeling_options;

   // if available, use options from specific strip chart
   if (m_labeling_options)
      labeling_options = *m_labeling_options;
   else
      labeling_options.initialize_from_registry("Print Options");

   dlg.set_labeling_options(&labeling_options);

   if (dlg.DoModal() == IDOK)
   {
      dlg.get_labeling_options(&labeling_options);

      // if options from specific strip chart are available, change them
      if (m_labeling_options)
         *m_labeling_options = labeling_options;

      // save these values as the defaults - regardless
      labeling_options.save_in_registry("Print Options");

      m_modified_labeling_options = TRUE;

      SetModified(TRUE);
   }
}

LRESULT CStripChartPropertyPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}
