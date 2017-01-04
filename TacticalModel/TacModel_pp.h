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

#if !defined(TACMODEL_PP_H)
#define TACMODEL_PP_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TacModel_pp.h : header file
//

#include "overlay_pp.h"
#include "TacModel.h"
#include "optndlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTacticalModelOvlOptionsPage dialog

class CTacticalModelOvlOptionsPage : public COverlayPropertyPage
{
   DECLARE_DYNCREATE(CTacticalModelOvlOptionsPage)

// Construction
public:
   CTacticalModelOvlOptionsPage();
   ~CTacticalModelOvlOptionsPage();

// Dialog Data
   //{{AFX_DATA(CTacticalModelOvlOptionsPage)
   enum { IDD = IDD_OVL_MODEL_DLG };
   CString  m_message;
   //}}AFX_DATA

   BOOL m_dirty;
   CString m_show_models;
   int m_width;
   int m_fill_style;
   int m_line_style;
   int m_color;
   COLORREF m_color_rgb;
   int m_diameter; 
   int m_label_code;
   int m_back_display_code;
   int m_elev_exag;
   CString m_icon_text;
   int m_gen_width;
   int m_gen_color;
   COLORREF m_gen_color_rgb;
   int m_gen_fill_style;
   int m_gen_line_style;
   int m_gen_diameter;
   int m_gen_label_code;
   int m_gen_background;
//   CString m_lodstr;
   static COLORREF m_CustClr[16]; // array of custom colors 

   static C_model_ovl* m_cur_ovl;

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CTacticalModelOvlOptionsPage)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CTacticalModelOvlOptionsPage)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnAdd();
   afx_msg void OnDelete();
   afx_msg void OnCenter();
   afx_msg void OnBackDisplayNone();
   afx_msg void OnBackDisplayImagery();
   afx_msg void OnBackDisplayElevation();
   afx_msg void OnShowManmadeModels();
   afx_msg void OnShowNaturalModels();
   afx_msg void OnDblclkList1();
   afx_msg void OnSelchangeList1();
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   afx_msg void OnFilterUp();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnCloseupCbLod();
   afx_msg void OnCloseupCbElev();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   void load_model_list(); 
   void upload_model_list(); 
   C_model_ovl* find_overlay(CString filename); 
   void update_combo_box(int box_id, CString data); 
   void get_ovl_data(C_model_ovl* ovl);
   void set_ovl_data(C_model_ovl* ovl);
   void redraw_the_overlays(); 
   void enable_window(int id, BOOL enable); 
   void show_window(int id, BOOL show); 
   void update_list_buttons();
   void update_filter_buttons();
   void show_elev_slider(bool show);
   BOOL get_model_list_info(int *cnt, int *selcnt, int *selpos, CString & name);

   virtual BOOL OnApply();

public:

};



class CTacticalModelPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CTacticalModelOvlOptionsPage(); }
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ifdef TACMODEL_PP_H
