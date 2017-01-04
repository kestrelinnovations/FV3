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

#if !defined(AFX_TEXTIMPORT_STEP3_H__D5AB1CB3_E6F8_4346_972B_1589F7AB2AA9__INCLUDED_)
#define AFX_TEXTIMPORT_STEP3_H__D5AB1CB3_E6F8_4346_972B_1589F7AB2AA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextImport_Step3.h : header file
//

#include "..\resource.h"
#include "TabularEditorDlg.h"
#include "ImportPropSheet.h"

class Parser;
class TabularEditorDataObject;
class ImportDataObject;
class FVGridCtrl;

/////////////////////////////////////////////////////////////////////////////
// CImport_Step3 dialog

class CImport_Step3 : public CImportPropertyPage
{
	DECLARE_DYNCREATE(CImport_Step3)

// Construction
public:
	CImport_Step3(Parser *parser = NULL, ImportDataObject *data_object = NULL);
	virtual ~CImport_Step3();

   virtual void Reinit();

	bool set_extended_formats(int index);
	void set_is_localpnt(BOOL pnt) { m_is_localpnt = pnt; }

// Dialog Data
	//{{AFX_DATA(CImport_Step3)
	enum { IDD = IDD_TAB_IMPORT_STEP3 };
	CComboBox	m_data_format_ext;
	CComboBox	m_data_format;
	//}}AFX_DATA
	FVGridCtrl *m_FVGrid;
	ImportDataObject *m_data_object;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImport_Step3)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImport_Step3)
	virtual BOOL OnInitDialog();
	afx_msg void OnModified();
	afx_msg void OnDataFormat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	Parser *m_parser;

	CList<column_info_t, column_info_t> m_column_info_list;
	BOOL m_is_localpnt;

public:
	void on_left_click(int col);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTIMPORT_STEP3_H__D5AB1CB3_E6F8_4346_972B_1589F7AB2AA9__INCLUDED_)
