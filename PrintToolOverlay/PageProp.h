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

// PageProp.h
// Description of classes for the edit propeties dialog for the Printing Tools
// Overlay.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_PAGEPROP_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_PAGEPROP_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/maps_d.h"     // MapScale and MapSource
#include "FalconView/include/optndlg.h"
#include "FalconView/resource.h"
#include "FalconView/ZoomPercentComboBox.h"

// this project's headers
#include "FalconView/PrintToolOVerlay/PrntPage.h"

// forward definitions
class CPrinterPage;
class MapTypeSelection;

class CPrintToolsPropertyPage : public CPropertyPage
{
   DECLARE_DYNCREATE(CPrintToolsPropertyPage)

private:
   MapTypeSelection *m_map_type;

// Construction
public:
   explicit CPrintToolsPropertyPage(boolean_t is_overlay_options = FALSE);
   ~CPrintToolsPropertyPage();

// Dialog Data
   // {{AFX_DATA(CPrintToolsPropertyPage)
   enum
   { IDD = IDD_OVL_PRINTPAGE_PROP_DLG };
   ZoomComboBox m_ctrlZoom;
   CComboBox m_ctrlScaleCombo;
   CComboBox m_ctrlCategoryCombo;
   CComboBox m_ctrlProjectionCombo;
   double m_dAngle;
   CString m_effective_scale;
   // }}AFX_DATA

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
         "Single_Page_Properties.htm";
   }

// Overrides
   // ClassWizard generate virtual function overrides
   // {{AFX_VIRTUAL(CPrintToolsPropertyPage)
   public:
   virtual BOOL OnSetActive();
   virtual BOOL OnApply();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   // }}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   // {{AFX_MSG(CPrintToolsPropertyPage)
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeScaleCombo();
   afx_msg void OnKillFocusAngle();
   afx_msg void OnDropdownZoom();
   afx_msg void OnKillfocusZoom();
   afx_msg void OnSelchangeZoom();
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
   void initialize_single_page();

private:
   // is this property sheet being modified in the overlay options dialog
   boolean_t m_is_overlay_options;
};

extern const GUID PROPPAGEID_PageLayout_SinglePage;

class CPageLayoutSinglePagePropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage()
   {
      return new CPrintToolsPropertyPage(TRUE);
   }
};

class CPrintToolsPropertiesSheet : public CPropertySheet
{
   DECLARE_DYNAMIC(CPrintToolsPropertiesSheet)

   CPrinterPage* m_pPrinterPage;

// Construction
public:
   CPrintToolsPropertiesSheet(UINT nIDCaption, CWnd* pParentWnd = NULL,
      UINT iSelectPage = 0);
   CPrintToolsPropertiesSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL,
      UINT iSelectPage = 0);

// Operations
public:
   void SetPrinterPage(CPrinterPage* pPage)
   {
      m_pPrinterPage = pPage;
   };
   void ApplyNow();

// Overrides
   // ClassWizard generated virtual function overrides
   // {{AFX_VIRTUAL(CPrintToolsPropertiesSheet)
   // }}AFX_VIRTUAL

// Implementation
public:
   virtual ~CPrintToolsPropertiesSheet();

   // Generated message map functions
protected:
   // {{AFX_MSG(CPrintToolsPropertiesSheet)
   afx_msg void OnApplyNow();
   // }}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


#endif  // FALCONVIEW_PRINTTOOLOVERLAY_PAGEPROP_H_
