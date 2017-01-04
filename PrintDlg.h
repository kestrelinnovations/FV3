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



// PrintDlg.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CPrintOptDlg dialog

class CLabelingOptions;
class CSecurityLabelExtensionCOM;

class CPrintOptDlg : public CDialog
{
   // Construction
public:
   // standard constructor
   CPrintOptDlg(CWnd* pParent = NULL);   

   // alternate constructor which will load IDD2, the alternate dialog.
   // The str passed in is irrelevant
   CPrintOptDlg(CString str, CWnd* pParent = NULL);   


   // Dialog Data
   //{{AFX_DATA(CPrintOptDlg)
   enum { IDD = IDD_PRINT_OPTIONS, IDD2 = IDD_PRINT_OPTIONS_ALT };
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Printing_and_Labeling_Options.htm";}
   CSliderCtrl m_text_slider;
   CStatic  m_text_percentage;
   CStatic  m_icon_percentage;
   CSliderCtrl m_line_slider;
   CStatic m_line_percentage;
   CSliderCtrl m_icon_slider;
   BOOL m_show_chart_info;
   BOOL m_show_map_type;
   BOOL m_print_chart_info;
   BOOL m_print_map_type;
   BOOL m_print_dafif_currency;
   BOOL m_print_date_time;
   BOOL m_print_echum_currency;
   BOOL m_print_cadrg_currency;
   BOOL m_print_compass_rose;
   BOOL m_print_compass_rose_when_not_north_up;

   //}}AFX_DATA


   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CPrintOptDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CPrintOptDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnSecurityButton();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnPrintCompassRose();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnHelp();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   // Security extensions permit customizable security labeling on 
   // printed materials.  In particular, JMPS uses its own security 
   // downgrade mechanism.
   CSecurityLabelExtensionCOM* m_pSecurityLabelMgr;


protected:
   void set_button(int id);
   int get_checked_radio_button(void);
   boolean_t m_using_alternate_dialog;

public:
   void set_labeling_options(CLabelingOptions *labeling_options);
   void get_labeling_options(CLabelingOptions *labeling_options);
};