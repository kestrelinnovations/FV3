// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

#if !defined(AFX_MDMCHARTINDEX_H__615D2264_636B_11D2_B07E_00104B8C938E__INCLUDED_)
#define AFX_MDMCHARTINDEX_H__615D2264_636B_11D2_B07E_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "mdmlctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMChartIndex dialog

class CMDMChartIndex : public CPropertyPage
{
   DECLARE_DYNCREATE(CMDMChartIndex)

   BOOL m_bInitialized;
   BOOL m_bRefreshing;
   BOOL m_bSufficientSpace;

// Construction
public:
   CMDMChartIndex();
   ~CMDMChartIndex();
   void Refresh();
   void EnableUnselecting(BOOL b);
   void GetMinimumRect(CRect* pRect);
   void set_imported_data_date(void);

// Dialog Data
   //{{AFX_DATA(CMDMChartIndex)
   enum { IDD = IDD_MDM_CHART_INDEX };
   CStatic m_MinSizeBoxCtrl;
   CStatic m_ImportDatesBox;
   CStatic m_ImportIndexBox;
   CStatic m_ImportInformationBox;
   CStatic m_EcrgImportedCtrl;
   CStatic m_CSDImportedCtrl;
   CStatic m_TLMImportedCtrl;
   CStatic m_EcrgImportedDateCtrl;
   CStatic m_CSDImportedDateCtrl;
   CStatic m_TLMImportedDateCtrl;
   CEdit   m_ImportPathCtrl;
   CButton m_ImportBrowseCtrl;
   CButton m_ImportApplyCtrl;
   CButton m_ProductListCtrl;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CMDMChartIndex)
   public:
   virtual BOOL OnSetActive();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Map_Data_Manager_Chart_Currency_Index.htm";}
   // Generated message map functions
   //{{AFX_MSG(CMDMChartIndex)
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   virtual BOOL OnInitDialog();
   afx_msg void OnProductList();
   afx_msg void OnImportBrowse();
   afx_msg void OnImportApply();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnHelpMdmMultiple();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMCHARTINDEX_H__615D2264_636B_11D2_B07E_00104B8C938E__INCLUDED_)
