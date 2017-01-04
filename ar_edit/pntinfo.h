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
#if !defined(AFX_PNTINFO_H__2478A11E_AAF2_416B_A330_92895A9E995C__INCLUDED_)
#define AFX_PNTINFO_H__2478A11E_AAF2_416B_A330_92895A9E995C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// pntinfo.h : header file
//

#include "ar_edit.h"
#include "..\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CUserARPointSheetDlg

class CUserARPointSheetDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CUserARPointSheetDlg)

// Construction
public:
	CUserARPointSheetDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CUserARPointSheetDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

	static C_ar_point *m_temp_ar_pt;
	static C_ar_point *m_ar_pt;
	static C_ar_obj * m_rte;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserARPointSheetDlg)
	public:
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUserARPointSheetDlg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
protected:
	//{{AFX_MSG(CUserARPointSheetDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CUserARPointInfoPageDlg dialog

class CUserARPointInfoPageDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CUserARPointInfoPageDlg)

// Construction
public:
	CUserARPointInfoPageDlg();   // standard constructor
	~CUserARPointInfoPageDlg();   // standard destructor

// Dialog Data
	//{{AFX_DATA(CUserARPointInfoPageDlg)
	enum { IDD = IDD_USER_AR_POINT_INFO_DLG };
	CString	m_location;
	CString	m_nav_bearing_str;
	CString	m_nav_dist_str;
	CString	m_nav_id;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserARPointInfoPageDlg)
	protected:
	virtual BOOL OnApply();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserARPointInfoPageDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnRemove();
	afx_msg void OnAdd();
	afx_msg void OnKillfocusNavId();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePointTypeList();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//	double m_lat;
//	double m_lon;
	double m_old_lat;
	double m_old_lon;
	CString m_type;
	double m_nav_lat;
	double m_nav_lon;
	double m_nav_bearing;
	double m_nav_dist;
	int m_op;
	ViewMapProj *m_map;

	void update_type_box_add();
	void update_type_box_change();
	void update_point_type_list();
	void update_nav_info();

public:

	static BOOL m_new;

	void set_point_ptr(C_ar_point *pt);
	void set_route_ptr(C_ar_obj *rte); 
	void set_map(ViewMapProj *map);
	void set_location(double lat, double lon);

};
/////////////////////////////////////////////////////////////////////////////
// CUserARPointLocPageDlg dialog

class CUserARPointLocPageDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CUserARPointLocPageDlg)

// Construction
public:
	CUserARPointLocPageDlg();
	~CUserARPointLocPageDlg();

// Dialog Data
	//{{AFX_DATA(CUserARPointLocPageDlg)
	enum { IDD = IDD_USER_AR_POINT_LOC_DLG };
	CGeocx1	m_geocx;
	BOOL	m_auto_mag_var;
	CString	m_calced_magvar;
	CString	m_mag_var_str;
	CString	m_bearing_str;
	CString	m_distance_str;
	CString	m_fix;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUserARPointLocPageDlg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUserARPointLocPageDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusFix();
	afx_msg void OnKillfocusFixBearing();
	afx_msg void OnKillfocusFixDistance();
	afx_msg void OnRbBearingMag();
	afx_msg void OnRbBearingTrue();
	afx_msg void OnAutoMagvar();
	afx_msg void OnOCXCHANGEGeocx1ctrl1();
	afx_msg void OnClose();
	afx_msg void OnOCXKILLFOCUSGeocx1ctrl1();
	afx_msg void OnPaint();
	afx_msg void OnChangeFixBearing();
	afx_msg void OnChangeFixDistance();
	afx_msg void OnSetfocusFixBearing();
	afx_msg void OnKillfocusMagVar();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	ViewMapProj *m_map;

//	double m_lat;
//	double m_lon;
	double m_old_lat;
	double m_old_lon;
	double m_fix_lat; 
	double m_fix_lon;
//	CString m_nav_flag;
//	CString m_nav_id;
//	CString m_nav_type;
//	CString m_nav_country;
//	CString m_nav_key;
	CString m_type;
//	double m_nav_lat;
//	double m_nav_lon;
	double m_nav_bearing;
	double m_nav_dist;
	BOOL m_mag_bearing;
	double m_mag_var;
	double m_bearing;
	double m_true_bearing;
	double m_magnetic_bearing;
	double m_distance;
	int m_op;
	CString m_old_fix;
	BOOL m_calced_fix;
	BOOL m_changed_dist;
	BOOL m_changed_bearing;
	CString m_calced_bearing;
	CString m_calced_dist;

	void calc_fix();
	BOOL calc_mag_var();

public:

	static BOOL m_new;

	void set_point_ptr(C_ar_point *pt);
	void set_route_ptr(C_ar_obj *rte); 
	void set_map(ViewMapProj *map);
	void set_location(double lat, double lon);
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PNTINFO_H__2478A11E_AAF2_416B_A330_92895A9E995C__INCLUDED_)
