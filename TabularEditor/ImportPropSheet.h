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

#if !defined(AFX_IMPORTPROPSHEET_H__69096B7C_988D_4D78_81ED_B6AAF1502194__INCLUDED_)
#define AFX_IMPORTPROPSHEET_H__69096B7C_988D_4D78_81ED_B6AAF1502194__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportPropSheet

class Parser;
class CImport_Step3;

#include "TabularEditorDlg.h"

const int IMPORT_PREVIEW_ROW_COUNT = 10;

class ImportDataObject : public TabularEditorDataObject
{
public:
	// constructor
	ImportDataObject();
	void set_parent_data_object(TabularEditorDataObject *data_obj) { m_parent_data_object = data_obj; }
	int set_parser(Parser *parser);
	void set_data_format_page(CImport_Step3 *page) { m_data_format_page = page; }

protected:

	Parser *m_parser;
	TabularEditorDataObject *m_parent_data_object;
	CImport_Step3 *m_data_format_page;

	int m_num_columns;
	CList<CString, CString> m_cell_data[IMPORT_PREVIEW_ROW_COUNT];

	// the column that will be highlighted in the grid control
	int m_current_col;

	int m_active_page;

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

	virtual void OnLClicked(int col, long row);
   virtual void OnTH_LClicked(int col, long row);
	virtual BOOL EnableFocusRect() { return FALSE; }

public:

	int create_preview();
	int GetParentColName(int col, CString *string) { return m_parent_data_object->GetColName(col, string); }
	int GetParentNumCols() { return m_parent_data_object->GetNumCols(); }

   TabularEditorDataObject *GetParentDataObject() { return m_parent_data_object; }

	int get_current_col() { return m_current_col; }
	void set_active_page(int p) { m_active_page = p ; }

	// called when the parser properties are changed
	void update();
};

class CImportPropertyPage : public CPropertyPage
{
public:
   CImportPropertyPage();
   CImportPropertyPage( UINT nIDTemplate, UINT nIDCaption = 0 );
   CImportPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0 );

   virtual ~CImportPropertyPage() { }
   virtual void Reinit() = 0;
};

class CImportPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CImportPropSheet)

// Construction
public:
	CImportPropSheet(Parser *parser, CWnd* pParentWnd = NULL);
	CImportPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CImportPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

   void SetupPages(BOOL bDelimitedNotFixedWidth, CList<CPropertyPage *, CPropertyPage *> &page_list);
   BOOL GetWizardFinishedOK() { return m_bWizardFinishedOK; }
   void SetWizardFinishedOK(BOOL bFinishedOK) { m_bWizardFinishedOK = bFinishedOK; }

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportPropSheet)
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImportPropSheet();

	// Generated message map functions
public:
	//{{AFX_MSG(CImportPropSheet)
	afx_msg void OnLoadSettings();
	afx_msg void OnSaveSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	CButton m_save_settings_btn;
	CButton m_load_settings_btn;

	Parser *m_parser;
   CWnd *m_pParentWnd;
   BOOL m_bWizardFinishedOK;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTPROPSHEET_H__69096B7C_988D_4D78_81ED_B6AAF1502194__INCLUDED_)
