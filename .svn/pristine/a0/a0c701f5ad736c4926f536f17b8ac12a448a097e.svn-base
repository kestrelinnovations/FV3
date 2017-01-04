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

#ifndef FALCONVIEW_SHP_BOUNDARIESOVERLAYPROPERTYPAGE_H_
#define FALCONVIEW_SHP_BOUNDARIESOVERLAYPROPERTYPAGE_H_

#include "overlay_pp.h"
#include "BoundariesOverlay.h"
#include "optndlg.h"

/////////////////////////////////////////////////////////////////////////////
// BoundariesOverlayOptionsPage dialog

class BoundariesOverlayOptionsPage : public COverlayPropertyPage
{
   DECLARE_DYNCREATE(BoundariesOverlayOptionsPage)

// Construction
public:
   BoundariesOverlayOptionsPage();
   ~BoundariesOverlayOptionsPage();

// Dialog Data
   //{{AFX_DATA(BoundariesOverlayOptionsPage)
   enum { IDD = IDD_OVL_BOUNDARIES_OVERLAY_DLG };
   std::vector<BOOL> m_on;
   BOOL m_background;
   int m_poly_edge;
   BOOL m_near_line;
   BOOL m_water_area_labels;
   BOOL m_keep_area_labels_in_view;
   CComboBox m_area_label_threshold_control;
   CString m_area_label_largest_threshold;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(BoundariesOverlayOptionsPage)
public:
   virtual BOOL OnApply();
   virtual void toggle_control_states( boolean_t turn_on ) override;

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(BoundariesOverlayOptionsPage)
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   virtual void OnOK();
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   afx_msg void On0Color();
   afx_msg void On1Color();
   afx_msg void On2Color();
   afx_msg void On3Color();
   afx_msg void On4Color();
   afx_msg void On5Color();
   afx_msg void On6Color();
   afx_msg void On7Color();
   afx_msg void OnFill();
   afx_msg void OnNearLine();
   afx_msg void OnBDefaultColors();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   int m_fill_style;
   std::vector<COLORREF> m_colors;
   static COLORREF m_CustClr[16]; // array of custom colors

   void select_color(int ctrl, COLORREF & rgb_color);
   void save_custom_colors();
   void load_custom_colors();
   void DrawFillButton();

private:
   VOID SaveProperties();
   BOOL UpdateRegistry( const CString& csNewValue, LPCSTR pszValueName ) const;
   BOOL UpdateRegistry( LPCSTR pszNewValue, LPCSTR pszValueName ) const;
};

class CBoundariesOverlayPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage()
   {
      return new BoundariesOverlayOptionsPage();
   }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif  // ifndef FALCONVIEW_SHP_BOUNDARIESOVERLAYPROPERTYPAGE_H_
