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

// fielddlg.h

#if !defined(FIELDDLG_H)
#define FIELDDLG_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// fielddlg.h : header file
//

#include "dbase.h"
#include "..\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CShpFieldSelectDlg dialog

class CShpFieldSelectDlg : public CDialog
{
// Construction
public:
	CShpFieldSelectDlg(CWnd* pParent = NULL);   // standard constructor
	~CShpFieldSelectDlg();
// Dialog Data
	//{{AFX_DATA(CShpFieldSelectDlg)
	enum { IDD = IDD_SHP_FIELD_DLG };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Shape_File_Label_Fields_Selections_List.htm";}

		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShpFieldSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShpFieldSelectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDblclkFieldList();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int m_cx, m_cy;
	BOOL m_active;

public:
	CList<CString*, CString*> m_list;
	CString m_selection;
	CString m_filename;

	void fill_field_list(); 
	void update_field_list();
	CString get_field_data(CDbase & dbf, CString fieldname); 
	void get_selection(CString & selection); 
	void set_shape_database_name(CString filename); 
	void AdjustControls(int cx, int cy);

	void init_list(const CList<CString*, CString*> & strlist);


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(FIELDDLG_H)