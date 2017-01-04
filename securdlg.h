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



// securdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSecurDlg dialog

class CSecurDlg : public CDialog
{
private:
   CString m_label_on_title_bar; // the security label displayed on the title
                                 // bar when the dialog is initialized, or "" 
                                 // for none

// Construction
public:
	CSecurDlg(CWnd* pParent = NULL);   // standard constructor

    // alternate constructor that will use the IDD2
   // options dialog (removed check boxes)
   CSecurDlg(CString str, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CSecurDlg)
	enum { IDD = IDD_SECUR_DLG, IDD2 = IDD_SECUR_DLG_ALT };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Security_Labeling_Options.htm";}
	CEdit	m_custom_label;
	CListBox	m_std_secur_lb;
	BOOL	m_display_sec_label_on_printout;
	BOOL	m_display_sec_label_on_title_bar;
   CButton m_SC_none;
   CButton m_SC_custom;
   CButton m_SC_standard;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecurDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSecurDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnScCustom();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnScStandard();
	afx_msg void OnScNone();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

   int get_checked_radio_button(void);
   void set_button(int id);

   CWnd* get_std_secur_list_box(void);
   CWnd* get_custom_label_window(void);
   CWnd* get_printout_checkbox(void);
   CWnd* get_title_bar_checkbox(void);
};