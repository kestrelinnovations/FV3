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

//{{AFX_INCLUDES()
#include "geocx1.h"
//}}AFX_INCLUDES

#if !defined(AFX_INFODLG_H__263C2B65_8864_4466_A190_3249F7FA7BBA__INCLUDED_)
#define AFX_INFODLG_H__263C2B65_8864_4466_A190_3249F7FA7BBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// infodlg.h : header file
//

#include "ar_edit.h"

#define OP_ADD    1
#define OP_CHANGE 2
#define OP_REMOVE 3


/////////////////////////////////////////////////////////////////////////////
// CUserArEditInfoDlg dialog

class CUserArEditInfoDlg : public CDialog
{
// Construction
public:
	CUserArEditInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserArEditInfoDlg)
	enum { IDD = IDD_USER_AR_EDIT_INFO };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/trackorb/Track_Orbit_Information.htm";}

	CString	m_apn;
	CString	m_apx;
	CString	m_atc;
	CString	m_remarks;
	CString	m_sched_unit;
	CString	m_name;
	CString	m_fuel1alt1;
	CString	m_fuel1alt2;
	CString	m_fuel2alt1;
	CString	m_fuel2alt2;
	CString	m_fuel3alt1;
	CString	m_fuel3alt2;
	CString	m_altdesc1;
	CString	m_altdesc2;
	CString	m_altdesc3;
	CString	m_course_str;
	CString	m_length_str;
	CString	m_width_str;
	double	m_freq_bck;
	double	m_freq_pri;
	CString	m_tac_chan;
	CString	m_length_units;
	CString	m_width_units;
	CString	m_tanker_chan;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserArEditInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserArEditInfoDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnKillfocusLength();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnKillfocusCourse();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnKillfocusTacChan();
	afx_msg void OnKillfocusApn();
	afx_msg void OnKillfocusApx();
	afx_msg void OnKillfocusFreqBck();
	afx_msg void OnKillfocusFreqPri();
	afx_msg void OnKillfocusAlt11();
	afx_msg void OnKillfocusAlt12();
	afx_msg void OnKillfocusAlt21();
	afx_msg void OnKillfocusAlt22();
	afx_msg void OnKillfocusAlt31();
	afx_msg void OnKillfocusAlt32();
	afx_msg void OnChangeTacChan();
	afx_msg void OnSelchangeUnits();
	afx_msg void OnCloseupCountry();
	afx_msg void OnKillfocusCountry();
	afx_msg void OnAlt1_AA();
	afx_msg void OnAlt2_AA();
	afx_msg void OnAlt3_AA();
	afx_msg void OnAlt1_AB();
	afx_msg void OnAlt2_AB();
	afx_msg void OnAlt3_AB();
	afx_msg void OnAlt1_AT();
	afx_msg void OnAlt2_AT();
	afx_msg void OnAlt3_AT();
	afx_msg void OnAlt1_BT();
	afx_msg void OnAlt2_BT();
	afx_msg void OnAlt3_BT();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	enum unit_t { UAR_UNITS_NM, UAR_UNITS_KM, UAR_UNITS_METERS, UAR_UNITS_YARDS, UAR_UNITS_FEET};

	C_ar_obj * m_rte;

	CString m_last_text;
	unit_t m_units;
	double m_length_nm;
	double m_width_nm;

	CString m_country;

	CString m_old_name;

	BOOL m_aborted;

	void fill_country_list(); 

public:
	void set_route_ptr(C_ar_obj *rte); 
	void show_orbit_params(BOOL show);
	void show_window(int id, BOOL show);
	void enable_window(int id, BOOL enable);
	double convert_to_NM(double num);
	double convert_from_NM(double nm); 
	void update_dlg();
	CString get_units_label();
	BOOL altitude_ok(int alt); 

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFODLG_H__263C2B65_8864_4466_A190_3249F7FA7BBA__INCLUDED_)