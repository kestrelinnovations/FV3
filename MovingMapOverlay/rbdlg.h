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

#if !defined(AFX_RNGEBRNG_H__0390D281_DD81_11D3_80B8_00C04F60B086__INCLUDED_)
#define AFX_RNGEBRNG_H__0390D281_DD81_11D3_80B8_00C04F60B086__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// rbdlg.h : header file
//

#include "..\resource.h"
#include "gps.h"
#include "rb.h"
#include "rbdlg.h"
#include "geocx1.h"

// foward declarations
class RangeBearingManager;
class RangeBearingData;

/////////////////////////////////////////////////////////////////////////////
// RangeBearingPage dialog

class RangeBearingPage : public CPropertyPage
{
	DECLARE_DYNCREATE(RangeBearingPage)

// Construction
public:
	RangeBearingPage();
   RangeBearingPage(RangeBearingManager *rb_mgr);
	~RangeBearingPage();

// Dialog Data
	//{{AFX_DATA(RangeBearingPage)
	enum { IDD = IDD_RANGE_BEARING };
   CListCtrl	m_list_ctrl;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RangeBearingPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(RangeBearingPage)
   virtual BOOL OnInitDialog();
   afx_msg void OnAdd();
   afx_msg void OnStop();
	afx_msg void OnEdit();
	afx_msg void OnDoubleClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDelete();
   afx_msg void OnView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   boolean_t m_is_initialized;
   RangeBearingManager *m_rb_mgr;

public:
   void on_size(UINT nType, int cx, int cy);

   // return the minimum rectangle that this property page can be
   void GetMinimumRect(CRect &minrect);

public:
   // add - adds a range and bearing object to the list control with
   // the given r/b data.  Returns the index into the list
   // control where the range and bearing was added
   int add(RangeBearingData *data);

   // update - updates a range and bearing object with the given index
   // into the list control with the given data
   void update(int item_index, RangeBearingData *data);

   // delete a range/bearing object from the list control with the given index
   void delete_obj(int index);
};

/////////////////////////////////////////////////////////////////////////////
// RangeBearingDataDialog dialog

class RangeBearingDataDialog : public CDialog
{
// Construction
public:
	RangeBearingDataDialog(CWnd* pParent = NULL);   // standard constructor
   ~RangeBearingDataDialog();                      // destructor

// Dialog Data
	//{{AFX_DATA(RangeBearingDataDialog)
	enum { IDD = IDD_RANGE_BEARING_DATA };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mvmp/Range_and_Bearing_Object_Editor.htm";}

	CEdit	m_label_editctrl;
   CButton m_course_true;
   CButton m_course_mag;
   CButton m_units_nm;
   CButton m_units_km;
	//}}AFX_DATA

private:
   // used to handle the Apply
   boolean_t m_is_modified_not_add;
   RangeBearingManager *m_rb_mgr;
	CoastTrackManager *m_ctrack_mgr;
   int m_index;
   boolean_t m_already_added;
   boolean_t m_is_modified;

   RangeBearingData m_data;
	SnapToInfo m_snap_to;
	boolean_t m_snap_to_valid;

public:
   void set_already_added(boolean_t added) { m_already_added = added; }
   boolean_t get_already_added() { return m_already_added; }
   
   void set_rb_mgr(RangeBearingManager *mgr) { m_rb_mgr = mgr; }
	void set_ctrack_mgr(CoastTrackManager *mgr) { m_ctrack_mgr = mgr; }

	// if the user got a position from the map, then we
	// need to reopen the From or To dialog.  The variable m_type
	// will tell us 1) whether or not we need to reopen a dialog 
	// and 2) which one (From or To)
private:
	int m_type;
public:
	void set_type(int type) { m_type = type; }
	int get_type() { return m_type; }
   
   void set_index(int index) { m_index = index; }
   int get_index() { return m_index; }

   void SetData(RangeBearingData data, 
		SnapToInfo snap_to, boolean_t snap_to_valid)
   {
      m_data = data;
		m_snap_to = snap_to;
		m_snap_to_valid = snap_to_valid;
   }

   RangeBearingData GetData(); 

   void set_is_modified(boolean_t mod) { m_is_modified = mod; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RangeBearingDataDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RangeBearingDataDialog)
	virtual BOOL OnInitDialog();
   afx_msg void on_modified();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnApply();
	afx_msg void OnFrom();
	afx_msg void OnTo();
	afx_msg void OnSwap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   void update_from_to_text();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RNGEBRNG_H__0390D281_DD81_11D3_80B8_00C04F60B086__INCLUDED_)