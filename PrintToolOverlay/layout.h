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

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_LAYOUT_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_LAYOUT_H_

// layout.h : header file
//

#define SQUASH 0.75

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/maps.h"    // page_orientation_t
#include "FalconView/resource.h"
// this project's headers
// forward definitions

class CVirtualPageLayout
{
public:
   // constructors
   CVirtualPageLayout();
   CVirtualPageLayout(int rows, int columns,
      float horz_spacing, float vert_spacing);

   // initialize number of rows, number of columns, horizontal spacing and
   // vertical spacing from the registry located at the given registry key
   // into this object
   void initialize_from_registry(CString key);

   // Store number of rows, number of columns, horizontal spacing and
   // vertical spacing into the registry located at the given registry key
   void save_to_registry(CString key);

private:
   int m_num_rows;
   int m_num_columns;
   float m_horz_spacing;
   float m_vert_spacing;

   // see maps.h for enumeration
   page_orientation_t m_orientation;

public:
   // get/set functions
   void set_num_rows(int num)
   {
      m_num_rows = num;
   }
   int get_num_rows(void) const
   {
      return m_num_rows;
   }

   void set_num_columns(int num)
   {
      m_num_columns = num;
   }
   int get_num_columns(void) const
   {
      return m_num_columns;
   }

   void set_horz_spacing(float spacing)
   {
      m_horz_spacing = spacing;
   }
   float get_horz_spacing(void) const
   {
      return m_horz_spacing;
   }
   void set_vert_spacing(float spacing)
   {
      m_vert_spacing = spacing;
   }
   float get_vert_spacing(void) const
   {
      return m_vert_spacing;
   }

   void set_orientation(page_orientation_t orientation)
   {
      m_orientation = orientation;
   }
   page_orientation_t get_orientation(void)
   {
      return m_orientation;
   }

public:
   // returns the upper left and lower right values in inches where
   // the map at the given row and column will be printed
   void get_map_bounds(int row, int column, double *upper_left_x,
      double *upper_left_y, double *lower_right_x, double *lower_right_y) const;

   // Return the virtual page width.  Returns -1.0 for error.  Returns 0.0,
   // if m_num_columns is too large for the current printer paper size.
   double get_page_width(void) const;
   double get_virtual_page_width(void) const;

   // Return the virtual page height.  Returns -1.0 for error.  Returns 0.0,
   // if m_num_rows is too large for the current printer paper size.
   double get_page_height(void) const;
   double get_virtual_page_height(void) const;

   // Minimum values for virtual page width and height.
   static double get_minimum_virtual_page_width()
   {
      return 2.0;
   }
   static double get_minimum_virtual_page_height()
   {
      return 4.0;
   }

   // makes sure the virtual page width and height do not go below
   // the set minimum.  If they do then adjust the number of rows
   // and columns until every thing is a-OK
   void validate_num_rows_and_columns(void);
};

class CPageLayoutDlg : public CDialog
{
// Construction
public:
   explicit CPageLayoutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   // {{AFX_DATA(CPageLayoutDlg)
   enum
   { IDD = IDD_PAGE_LAYOUT };
   CString getHelpURIPath()
   {
      return "fvw.chm::/PageLayout.chm::/Page_Layout_topics/"
         "Virtual_Page_Setup.htm";
   }

   CStatic m_strip_page_bitmap;
   CEdit m_vert_spacing;
   CSpinButtonCtrl m_spin_vert_spacing;
   CSpinButtonCtrl m_spin_rows;
   CSpinButtonCtrl m_spin_horz_spacing;
   CSpinButtonCtrl m_spin_columns;
   CEdit m_num_rows;
   CEdit m_num_columns;
   CEdit m_horz_spacing;
   int m_value_num_rows;
   int m_value_num_columns;
   double m_value_horz_spacing;
   double m_value_vert_spacing;
   // }}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   // {{AFX_VIRTUAL(CPageLayoutDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   // }}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   // {{AFX_MSG(CPageLayoutDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnPaint();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnChangeNumColumns();
   afx_msg void OnChangeNumRows();
   afx_msg void OnChangeHorzSpacing();
   afx_msg void OnChangeVertSpacing();
   afx_msg void OnDeltaposSpinHorzSpacing(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDeltaposSpinVertSpacing(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDeltaposSpinRows(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDeltaposSpinColumns(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnPrintSetup();
   afx_msg void OnChangeOrientation();
   afx_msg void OnHelp();
   virtual void OnOK();
   // }}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   CVirtualPageLayout m_vpage_layout;

   // has the virtual page layout object been initialized
   boolean_t m_vpage_layout_is_initialized;

   // stores the portrait / landscape bitmaps
   HBITMAP m_bitmap[3];

   // used to scale the page to depict portrait / landscape
   double m_scale_x, m_scale_y;

public:
   void set_virtual_page_layout(CVirtualPageLayout layout)
   {
      m_vpage_layout = layout;
      m_vpage_layout_is_initialized = TRUE;

      if (m_vpage_layout.get_page_width() < m_vpage_layout.get_page_height())
      {
         m_scale_x = SQUASH;
         m_scale_y = 1.0;
      }
      else
      {
         m_scale_x = 1.0;
         m_scale_y = SQUASH;
      }
   }

   CVirtualPageLayout get_virtual_page_layout(void)
   {
      return m_vpage_layout;
   }

private:
   // draw the preview of the page layout
   void draw_preview(HWND hwnd);
   void draw_page(CDC *pDC, CRect client_rect);
   void draw_dividers(CDC *pDC, CRect client_rect);
   void draw_text(CDC *pDC, CRect client_rect);
   void draw_arrows(CDC *pDC, CRect client_rect);

   void change_num_columns(boolean_t show_error_message,
      boolean_t update_data = TRUE);
   void change_num_rows(boolean_t show_error_message,
      boolean_t update_data = TRUE);
};

// {{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately
// before the previous line.

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_LAYOUT_H_
