// Copyright (c) 1994-2009,2013 Georgia Tech Research Corporation, Atlanta, GA
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

#if !defined(SHP_PP_H)
#define SHP_PP_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// shp_pp.h : header file
//

#include "overlay_pp.h"
#include "shp.h"
#include "optndlg.h"

/////////////////////////////////////////////////////////////////////////////
// CShapeFileOvlOptionsPage dialog

class CShapeFileOvlOptionsPage : public COverlayPropertyPage
{
   DECLARE_DYNCREATE(CShapeFileOvlOptionsPage)

// Construction
public:
   CShapeFileOvlOptionsPage();
   ~CShapeFileOvlOptionsPage();

// Dialog Data
   //{{AFX_DATA(CShapeFileOvlOptionsPage)
   enum { IDD = IDD_OVL_SHAPE_DLG };
   CCheckListBox  m_filter_check_list;
   BOOL  m_background;
   CString  m_diameter_str;
   CString  m_filter_fieldname;
   BOOL  m_near_line_mode;
   CString  m_tooltip_fieldname1;
   CString  m_tooltip_fieldname2;
   CString  m_statbar_fieldname1;
   CString  m_statbar_fieldname2;
   BOOL  m_check_stat_1;
   BOOL  m_check_tip_1;
   BOOL  m_check_stat_2;
   BOOL  m_check_tip_2;
   BOOL  m_dotted_line;
   BOOL  m_use_icon;
   CString  m_message;
   CString  m_range_field1_text;
   CString  m_range_field2_text;
   CString  m_range_field1_edit1;
   CString  m_range_field1_edit2;
   CString  m_range_field2_edit1;
   CString  m_range_field2_edit2;
   //}}AFX_DATA

   BOOL m_dirty;
   int m_width;
   int m_fill_style;
   int m_line_style;
   int m_color;
   COLORREF m_color_rgb;
   int m_diameter; 
   int m_label_code;
   CString m_icon_text;
   int m_gen_width;
   int m_gen_color;
   COLORREF m_gen_color_rgb;
   int m_gen_fill_style;
   int m_gen_line_style;
   int m_gen_diameter;
   int m_gen_label_code;
   int m_gen_background;
   int m_gen_use_icon;
   CString m_gen_icon_text;
   int m_old_num_checked;
   int m_filter_mode;
   int m_field_op;
   static COLORREF m_CustClr[16]; // array of custom colors 

   static C_shp_ovl* m_cur_ovl;
   static C_shp_filter* m_cur_flt;
   CList<C_shp_filter* , C_shp_filter*> m_filter_list;
   C_shp_filter m_range_filter;

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CShapeFileOvlOptionsPage)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CShapeFileOvlOptionsPage)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnAdd();
   afx_msg void OnDelete();
   afx_msg void OnDblclkList1();
   afx_msg void OnSelchangeList1();
   afx_msg void OnMoveDown();
   afx_msg void OnMoveUp();
   afx_msg void OnProperties();
   afx_msg void OnBackground();
   afx_msg void OnCloseupDiameter();
   afx_msg void OnKillfocusDiameter();
   afx_msg void OnFillPgon();
   afx_msg void OnWidth();
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   afx_msg void OnSelectFieldname();
   afx_msg void OnBFilter();
   afx_msg void OnKillfocusFilterList();
   afx_msg void OnNearLine();
   afx_msg void OnColor();
   afx_msg void OnCheckTip1();
   afx_msg void OnCheckTip2();
   afx_msg void OnCheckStat1();
   afx_msg void OnCheckStat2();
   afx_msg void OnFilterSelectAll();
   afx_msg void OnFilterSelectNone();
   afx_msg void OnSelchangeFilterList();
   afx_msg void OnFilterDown();
   afx_msg void OnFilterUp();
   afx_msg void OnSaveGroup();
   afx_msg void OnDottedLine();
   afx_msg void OnBIcon();
   afx_msg void OnCbIcon();
   afx_msg void OnAutoColor();
   afx_msg void OnShpMngr();
   afx_msg void OnFilterSort();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnSearch();
   afx_msg void OnRbFilterList();
   afx_msg void OnRbFilterRange();
   afx_msg void OnRangeField1Button();
   afx_msg void OnRangeField2Button();
   afx_msg void OnRbFilterNone();
   afx_msg void OnCloseupRangeFieldOp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   void load_shapefile_list(); 
   void DrawWidthButton(int button_id, int width);
   void DrawFillButton();
   void DrawIconButton();
   void DrawStyleButton(int button_id, int style);
   C_shp_ovl* find_overlay(CString filename); 
   void show_line_stuff(BOOL show); 
   void show_pgon_stuff(BOOL show); 
   void show_point_stuff(BOOL show);
   void show_filter_stuff(BOOL show); 
   void show_tip_stuff(BOOL show); 
   void update_combo_box(int box_id, CString data); 
   void draw_the_buttons(); 
   int  fill_filter_list(); 
   void update_filter_list(); 
   void update_filter_check_list(C_shp_ovl* ovl); 
   void update_filter_check_list(); 
   void update_ovl_filter_list(C_shp_ovl* ovl); 
   void get_ovl_data(C_shp_ovl* ovl);
   void set_ovl_data(C_shp_ovl* ovl);
   void clear_filter_list();
   void set_ovl_filters(C_shp_ovl* ovl); 
   void get_ovl_filters(C_shp_ovl* ovl); 
   void redraw_the_overlays(); 
   void enable_window(int id, BOOL enable); 
   void show_window(int id, BOOL show); 
   void update_ovl_filter_list_numbers(C_shp_ovl* ovl); 
   BOOL is_valid_fieldname(CString filename, CString fieldname); 
   void update_check_list_from_filter_list();
   void update_filter_list_from_ovl(C_shp_ovl* ovl); 
   void update_point_items();
   int num_filters_checked(); 
   void get_list_of_checked(CStringArray & names); 
   void sort_filters(CArray<C_shp_filter*, C_shp_filter*> & list, int off, int n);
   BOOL valid_filter_ptr(C_shp_filter *testflt);
   void update_list_buttons();
   void update_filter_buttons();
   void set_filter_mode(int filter_mode);
   void update_range_controls_from_ovl(C_shp_ovl* ovl);
   BOOL get_shp_list_info(int *cnt, int *selcnt, int *selpos, CString & name);
   virtual BOOL OnApply();

public:
   static BOOL select_label_field(const C_shp_ovl* ovl, CString & fieldname);
   static void save_custom_colors();
   static void load_custom_colors();

private:
   BOOL SaveProperties();


};

/////////////////////////////////////////////////////////////////////////////
// CShpLabel dialog

class CShpLabel : public CDialog
{
// Construction
public:
   CShpLabel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CShpLabel)
   enum { IDD = IDD_SHP_LABEL_DLG };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Shape_File_Label_Fields.htm";}

   CString  m_tooltip_1;
   CString  m_tooltip_2;
   CString  m_statbar_1;
   CString  m_statbar_2;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CShpLabel)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CShpLabel)
   afx_msg void OnSelectTooltip1();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnSelectTooltip2();
   afx_msg void OnSelectStatbar1();
   afx_msg void OnSelectStatbar2();
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

class CShapeFilePropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CShapeFileOvlOptionsPage(); }
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHP_PP_H__FDDE031A_9B66_11D3_AC39_00C04F68ADE8__INCLUDED_)