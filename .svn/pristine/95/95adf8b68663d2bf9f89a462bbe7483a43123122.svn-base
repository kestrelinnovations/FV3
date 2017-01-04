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



// selovl.h : header file
//

#include "..\resource.h"
#include "common.h"
#include "overlay.h"

/////////////////////////////////////////////////////////////////////////////
// SelectOverlayDlg dialog

class C_overlay_factory;
struct OverlayTypeDescriptor;

class SelectOverlayDlg : public CDialog
{
public:
   enum selection_t {SELECT_OVERLAY_TYPE, SELECT_OVERLAY, SELECT_NULL};

private:
   selection_t m_mode;
   CList <CString, CString> m_selection;
   OverlayTypeDescriptor* m_pSelectedOverlayTypeDesc;
   char m_title[41];
   char m_label[43];
   char m_OK_label[16];

   long m_bFileOverlaysOnly;
   long m_bRequireFileExists;
   long m_bRequireDirty;
   long m_bSaveableOverlaysOnly;

   attr_word m_filter;

   boolean_t m_multi_select;
   
// Construction
public:
   SelectOverlayDlg(selection_t selection_mode = SELECT_NULL, 
      boolean_t file_overlays_only = FALSE,
      boolean_t dirty_overlays_only = FALSE,
      long bRequireFileExists = 0,
      boolean_t allow_multi_select = FALSE,
      boolean_t saveable_overlays_only = FALSE);

   // Returns a string indication the selection.
   // For SELECT_OVERLAY_TYPE the string is an overlay class name.
   // For SELECT_OVERLAY the string is an individual overlay's name.
   const char *get_selection() { return m_selection.GetHead(); }
   const char *get_selection(int index);
   OverlayTypeDescriptor *get_selected_overlay_type() { return m_pSelectedOverlayTypeDesc; }

   // Sets the title of the dialog box.
   int set_title(const char *title);

   // Sets the label of the list control.
   int set_label(const char *label);

   // Sets the label on the OK button.
   int set_OK_label(const char *label);

   // Dialog Data
   //{{AFX_DATA(SelectOverlayDlg)
   enum { IDD = IDD_SELECT_OVERLAY };
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Overlay_Selection_Dialogs.htm";}

   CButton m_OK;
   CStatic m_list_title;
   CListBox m_list;
   CListBox m_list_multi;
   //}}AFX_DATA

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SelectOverlayDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(SelectOverlayDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnDblclkOvlList();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

// private member functions
private:
   // Use the class title associated with the current overlay, provided its
   // title is in the list.
   void select_class_title_from_current_overlay(C_overlay *current_overlay);

   // Use the class title associated with the current mode, provided its title
   // is in the list.
   void select_class_title_from_current_mode(GUID overlayDescGuid);
};