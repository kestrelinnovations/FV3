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

#if !defined(AFX_RBLocationDlg_H__89582538_C12D_4C87_935F_3122ACD6CFFF__INCLUDED_)
#define AFX_RBLocationDlg_H__89582538_C12D_4C87_935F_3122ACD6CFFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// rbLocationDlg.h : header file
//

//{{AFX_INCLUDES()
#include "geocx1.h"
#include "rb.h"
//}}AFX_INCLUDES

#include "ctrakdlg.h"	// for CMappedBitmapButton

class SnapToInfo;

/////////////////////////////////////////////////////////////////////////////
// LocationDlg dialog

class LocationDlg : public CDialog
{
	// Construction
public:
	LocationDlg(CWnd* pParent = NULL);   // standard constructor

private:
	boolean_t m_invalid_input;
	RangeBearingData m_data;
	SnapToInfo m_snap_to;
	boolean_t m_snap_to_valid;

	CString m_description_str;
	CString m_fix_point_str;

	boolean_t m_is_from_not_to;

	CList <CString, CString> m_fix_pnt_lst;

public:
	void add_fix_point(CString fix_point)
	{
		m_fix_pnt_lst.AddTail(fix_point);
	}

private:
	
	// Dialog Data
	//{{AFX_DATA(LocationDlg)
	enum { IDD = IDD_RANGE_FROM_TO };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mvmp/Object_Position_Coordinates.htm";}

	CEdit	m_description;
	CEdit	m_fix_point;
	CGeocx1	m_geocx;
	CMappedBitmapButton m_bitmap_button;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LocationDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
   virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(LocationDlg)		
	afx_msg void OnKillfocusFixPoint();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnKillfocusDescription();
	afx_msg void OnHelp();
	afx_msg void OnGetPositionFromMap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetData(double lat, double lon, SnapToInfo snap_to, 
		boolean_t snap_to_valid, boolean_t is_from_not_to);

	double get_latitude() 
	{ 
		if (m_is_from_not_to)
			return m_data.m_from_latitude; 
		else
			return m_data.m_to_latitude;
	}
	double get_longitude() 
	{ 
		if (m_is_from_not_to)
			return m_data.m_from_longitude; 
		else
			return m_data.m_to_longitude;
	}
	CString get_description() { return m_description_str; }
	CString get_fix_point() { return m_fix_point_str; }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RBLocationDlg_H__89582538_C12D_4C87_935F_3122ACD6CFFF__INCLUDED_)