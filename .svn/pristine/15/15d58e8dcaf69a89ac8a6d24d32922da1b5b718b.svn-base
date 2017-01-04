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



#include "newptdef.h"
#include "FalconView/include/optndlg.h"

//
// Definition file: lp_pp.h
//

// Purpose: contains class definitions for the local points overlay options
// property page.

#ifndef LOCALPNT_PP_H
#define LOCALPNT_PP_H 1

/////////////////////////////////////////////////////////////////////////////
// CLocalPointsPage dialog

#include "FalconView/include/ovlelem.h"       // for OvlFont
#include "FalconView/include/overlay_pp.h"

class C_overlay;
class CLocalPointsPage;
class CFontDlg;

class CMyTreeCtrl : public CTreeCtrl
{
protected:
   afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint ptMousePos);
   afx_msg void OnCloseTreeItem();

   DECLARE_MESSAGE_MAP()

public:
   HTREEITEM m_hActiveItem;
   CLocalPointsPage *m_parent;
};

class CLocalPointsPage : public CPropertyPage
{
   DECLARE_DYNCREATE(CLocalPointsPage)
   
   CNewLocalPointDefaults m_new_point_defaults;


// Construction
public:
   CLocalPointsPage();
   ~CLocalPointsPage();
   
   CString m_current_list_box_database_filename;
   CString m_initial_database_filename;

   static bool s_default_database_failed;

// Dialog Data
   //{{AFX_DATA(CLocalPointsPage)
   enum { IDD = IDD_OVL_LOCAL_POINTS_OPTIONS };
   CComboBox m_display_threshold_control;
   CComboBox m_threshold_control;
   CString m_label_threshold;
   CString m_display_threshold;

   CEdit m_sample_id;
   CComboBox m_default_group_combo;
   CMyTreeCtrl m_tree_ctrl;
   CString m_database_filename;
   CString m_id_prefix;
   CString m_default_group_name;
   CComboBox m_DB_filename_CB;
   CString m_strFontDescription;
   //}}AFX_DATA

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CLocalPointsPage)
   public:
   virtual void OnOK();
   virtual BOOL OnApply();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CLocalPointsPage)
   virtual BOOL OnInitDialog();
   afx_msg void OnModified();
   afx_msg void OnBrowse();
   afx_msg void OnKillfocusDatabaseName();
   afx_msg void OnKillfocusLocalIdPrefix();
   afx_msg void OnKillfocusLocalDefaultGroupName();
   afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg LONG OnTreeClicked(UINT, LONG);
   afx_msg void OnOpen();
   afx_msg void OnClose();
   afx_msg void OnDefaultFont();
   afx_msg void OnFont();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()

   static void on_apply_font(CFontDlg *dlg);
   static void on_overlay_font(CFontDlg *dlg);

public:
   int fill_group_name_list_box();

private:
   int fill_tree_with_group_names(CString filename, HTREEITEM parent_tree_item);
   int fill_tree_with_group_names_unnamed(C_overlay *overlay, HTREEITEM parent);
   int save_changes_to_group_name_list_box();
   int save_changes_hlpr_unnamed(C_overlay *, HTREEITEM parent);
   int save_changes_hlpr_named(CString DB_name, HTREEITEM parent);


   BOOL database_filename_is_modified();
   BOOL group_name_list_box_is_modified();
   BOOL global_show_is_modified();
   BOOL save_global_show();
   
   void write_thresholds_to_registry();

   BOOL local_point_overlay_is_open();

   BOOL database_filespec_is_valid(void); 
   void UpdateFontDescription();
   BOOL UserChangedCheck(const CMyTreeCtrl& tree_control,
      const HTREEITEM& item) const;
public:
};

class CPointsPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CLocalPointsPage(); }
};

class ParentNodeList
{
public:
   CString m_DB_filename;
   C_overlay *m_overlay;

   bool operator<(const ParentNodeList &p) const
   {
      return m_DB_filename < p.m_DB_filename;
   }
};

#endif
