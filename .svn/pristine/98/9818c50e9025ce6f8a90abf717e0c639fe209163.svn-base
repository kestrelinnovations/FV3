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

// SCProp.h
// Description of classes for the edit propeties dialog for strip charts in the
// printing tools overlay.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_SCPROP_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_SCPROP_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/optndlg.h"
#include "FalconView/resource.h"
#include "FalconView/ZoomPercentComboBox.h"
// this project's headers
#include "FalconView/PrintToolOverlay/SChart.h"
// forward definitions
class MapTypeSelection;

class CStripChartPropertyPage : public CPropertyPage
{
   DECLARE_DYNCREATE(CStripChartPropertyPage)

private:
   MapTypeSelection *m_map_type;

// Construction
public:
   explicit CStripChartPropertyPage(boolean_t is_overlay_options = FALSE);
   ~CStripChartPropertyPage();

// Dialog Data
   // {{AFX_DATA(CStripChartPropertyPage)
   enum
   { IDD = IDD_OVL_STRIPCHART_PROP_DLG };
   ZoomComboBox m_ctrlZoom;
   CComboBox m_ctrlScaleCombo;
   CComboBox m_ctrlCategoryCombo;
   CComboBox m_ctrlProjectionCombo;
   CString m_effective_scale;
   double m_dOverlap;
   // }}AFX_DATA

   int m_nNorthUp;
   MapSource m_source;
   MapScale  m_scale;
   MapSeries    m_series;
   ProjectionEnum m_projection_type;
   int m_scale_percent;
   CLabelingOptions *m_labeling_options;
   boolean_t m_modified_labeling_options;

   CString getHelpURIPath()
   {
      return "fvw.chm::/PageLayout.chm::/Page_Layout_topics/"
         "Strip_Chart_Properties.htm";
   }

// Overrides
   // ClassWizard generate virtual function overrides
   // {{AFX_VIRTUAL(CStripChartPropertyPage)
   public:
   virtual BOOL OnSetActive();
   virtual BOOL OnApply();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   // }}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   // {{AFX_MSG(CStripChartPropertyPage)
   virtual BOOL OnInitDialog();
   afx_msg void OnNorthUpClicked();
   afx_msg void OnFollowRouteClicked();
    afx_msg void OnSelchangeScaleCombo();
   afx_msg void OnDropdownZoom();
   afx_msg void OnKillfocusZoom();
   afx_msg void OnSetfocusZoom();
   afx_msg void OnCloseupZoom();
   afx_msg void OnSelchangeZoom();
   afx_msg void OnKillFocusPageOverlap();
   afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnVPageSetup();
   afx_msg void OnLabelingOptions();
   afx_msg void OnProjectionChange();
   // }}AFX_MSG
   afx_msg void OnSelchangeCategory();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   DECLARE_MESSAGE_MAP()

private:
   // get the scale percent from the ZoomComboBox
   void retrieve_scale_percent();

   // calculates the effective scale from the current scale and scale percent
   // and updates the dialog box
   void update_effective_scale();

   // special initialization for the overlay options page
   // (defaults from registry)
   void initialize_strip_chart();

private:
   // is this property sheet being modified in the overlay options dialog
   boolean_t m_is_overlay_options;
};

extern const GUID PROPPAGEID_PageLayout_StripChart;

class CPageLayoutStripChartPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage()
   {
      return new CStripChartPropertyPage(TRUE);
   }
};

class CStripChartPropertiesSheet : public CPropertySheet
{
   DECLARE_DYNAMIC(CStripChartPropertiesSheet)

   CStripChart* m_pChart;

// Construction
public:
   CStripChartPropertiesSheet(UINT nIDCaption, CWnd* pParentWnd = NULL,
      UINT iSelectPage = 0);
   CStripChartPropertiesSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL,
      UINT iSelectPage = 0);

// Operations
public:
   void SetStripChart(CStripChart* pChart)
   {
      m_pChart = pChart;
   }
   void ApplyNow();

   CStripChart *get_stripchart(void)
   {
      return m_pChart;
   }

// Overrides
   // ClassWizard generated virtual function overrides
   // {{AFX_VIRTUAL(CStripChartPropertiesSheet)
   // }}AFX_VIRTUAL

// Implementation
public:
   virtual ~CStripChartPropertiesSheet();

   // Generated message map functions
protected:
   // {{AFX_MSG(CStripChartPropertiesSheet)
   afx_msg void OnApplyNow();
   // }}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
#endif  // FALCONVIEW_PRINTTOOLOVERLAY_SCPROP_H_
