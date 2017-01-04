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

#if !defined(AFX_MULTILOCATIONEDITDLG_H__6B05ECE0_F10A_4544_AFD7_AB64FB9105FE__INCLUDED_)
#define AFX_MULTILOCATIONEDITDLG_H__6B05ECE0_F10A_4544_AFD7_AB64FB9105FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiLocationEditDlg.h : header file
//

#include "TabularEditorDlg.h"
#include "geotrans.h"
#include "geo_tool_d.h"


/////////////////////////////////////////////////////////////////////////////
// CMultiLocationEditDlg dialog

class FVGridCtrl;

class MultiLocationDataObject : public TabularEditorDataObject
{
public:
	// constructor
	MultiLocationDataObject();

protected:
	CList<d_geo_t, d_geo_t &> m_location_list;
	CGeoTrans m_geotrans;
	int m_selected_row;

public:

	//
	// CUGDataSource overrides
	//
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int GetColName(int col, CString *string);
	
	virtual int	GetCell(int col,long row,CUGCell *cell);
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

	virtual void on_selected(int row);

	virtual int OnCanSizeCol(int col) { return FALSE; }
	virtual BOOL AllowSort() { return FALSE; }

	//virtual void OnMenuStart(int col, int row, int base_ID);
	//virtual void OnMenuCommand(int col, long row, int item);

	void add_location(d_geo_t location);
	void delete_selected_row();
	void insert_after_selected_row();

	int GetCount() { return m_location_list.GetCount(); }
	d_geo_t GetAt(int index)
	{
		POSITION position = m_location_list.FindIndex(index);
		if (position)
			return m_location_list.GetAt(position);

		// if not found
		d_geo_t empty = { 0.0, 0.0 };
		return empty;
	}
};

class CMultiLocationEditDlg : public CDialog
{
// Construction
public:
	CMultiLocationEditDlg(CWnd* pParent = NULL);   // standard constructor
	~CMultiLocationEditDlg();

// Dialog Data
	//{{AFX_DATA(CMultiLocationEditDlg)
	enum { IDD = IDD_TAB_MULTI_LOCATION };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/FalconView_Overview.htm";}
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiLocationEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMultiLocationEditDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteRow();
	afx_msg void OnInsertRow();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	FVGridCtrl *m_FVGrid;
	MultiLocationDataObject *m_data_object;
	CToolBar m_toolbar;

public:
	void add_location(d_geo_t location);
	int GetCount() { return m_data_object->GetCount(); }
	d_geo_t GetAt(int index) { return m_data_object->GetAt(index); }

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTILOCATIONEDITDLG_H__6B05ECE0_F10A_4544_AFD7_AB64FB9105FE__INCLUDED_)