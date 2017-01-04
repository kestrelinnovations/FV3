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




#if !defined(AFX_MAPGOTOPROPPAGE_H__FB98BCFA_112E_4824_9692_FAA768D6AE2D__INCLUDED_)
#define AFX_MAPGOTOPROPPAGE_H__FB98BCFA_112E_4824_9692_FAA768D6AE2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// MapGoToPropPage.h : header file
//

#include "common.h"  // boolean_t
#include "maps_d.h"  // MapSpec, MapCategory, and MapScale
#include "resource.h"  // IDD_MAP_SETTINGS_GOTO
#include "snapto.h"  // SnapToInfo

class MapTypeSelection;

//{{AFX_INCLUDES()
#include "geocx1.h"
#include "afxwin.h"
//}}AFX_INCLUDES


/////////////////////////////////////////////////////////////////////////////
// CMapGoToPropPage dialog

class CMapGoToPropPage : public CPropertyPage
{
   DECLARE_DYNCREATE(CMapGoToPropPage)

private:
   MapTypeSelection *m_map_type_selection_ctrl;
   MapType m_map_type;
   MapProjectionParams m_map_proj_params;
   MapCategory m_initialCategory;
   boolean_t m_invalid_input;
   CList<CString, CString &> m_listRecentlyUsed;

// Construction
public:
   CMapGoToPropPage();
   ~CMapGoToPropPage();

   boolean_t m_enable_rotate;
   int m_redraw;

   // Dialog Data
   //{{AFX_DATA(CMapGoToPropPage)
   enum { IDD = IDD_MAP_SETTINGS_GOTO };
   CEdit m_description;
   CEdit m_fix_point;
   CButton m_step_cw;
   CButton m_step_ccw;
   CEdit m_angle;
   CComboBox   m_scale;
   CComboBox   m_category;
   CGeocx1 m_geocx;
   CGeocx1 m_geocx_secondary;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CMapGoToPropPage)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CMapGoToPropPage)
   virtual void OnOK();
   virtual void OnCancel();
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeCategory();
   afx_msg void OnClickedStepCcw();
   afx_msg void OnClickedStepCw();
   afx_msg void OnSelchangeScaleList();
   afx_msg void OnKillfocusAngle();
   afx_msg void OnKillfocusFixPoint();
   afx_msg void OnKillfocusDescription();
   afx_msg void OnHelp();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   // based on the given map scale and the value of m_enable_rotate the enable
   // state of the rotation controls are set
   void set_rotation_state(const MapScale& scale);

   // search the shape file overlays
   int search_shapes(CString keystr, SnapToInfo & db_info);

   void IndicateSearchMiss(CString str, CWnd* pInputCtrl, CWnd* pAssocCtrl);

   CString rotation_to_text(double rotation);

   DECLARE_EVENTSINK_MAP()
   void OCXKILLFOCUSGeocx1ctrl1();
   void OCXKILLFOCUSGeocx1ctrl2();
   CStatic m_displayed_format;
   CStatic m_other_format;
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mapsetng/Go_To_Map_Settings.htm";}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPGOTOPROPPAGE_H__FB98BCFA_112E_4824_9692_FAA768D6AE2D__INCLUDED_)
