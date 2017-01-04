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

#if !defined(AFX_TEXTIMPORT_STEP1_H__694A531B_1C2D_4A0F_A813_6261EFE54D37__INCLUDED_)
#define AFX_TEXTIMPORT_STEP1_H__694A531B_1C2D_4A0F_A813_6261EFE54D37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextImport_Step1.h : header file
//

#include "..\resource.h"
#include "ImportPropSheet.h"

// forward declarations
class TextParser;
class ImportDataObject;
class FVGridCtrl;

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step1 dialog

class CTextImport_Step1 : public CImportPropertyPage
{
	DECLARE_DYNCREATE(CTextImport_Step1)

// Construction
public:
	CTextImport_Step1(TextParser *text_parser = NULL, ImportDataObject *data_object = NULL);
	virtual ~CTextImport_Step1();
   virtual void Reinit();

	void set_page_list(CList<CPropertyPage *, CPropertyPage *> *page_list) { m_page_list.AddTail(page_list); }
   void get_page_list(CList<CPropertyPage *, CPropertyPage *> &page_list) 
   { 
      page_list.RemoveAll(); 
      POSITION pos = m_page_list.GetHeadPosition();
      while (pos)
         page_list.AddTail(m_page_list.GetNext(pos));
   }

// Dialog Data
	//{{AFX_DATA(CTextImport_Step1)
	enum { IDD = IDD_TAB_IMPORT_TEXT_STEP1 };
	CSpinButtonCtrl	m_spin_ctrl;
	CButton m_delimited;
	CButton m_fixed_width;
	int		m_start_row;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTextImport_Step1)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTextImport_Step1)
	virtual BOOL OnInitDialog();
	afx_msg void OnModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	TextParser *m_text_parser;
	FVGridCtrl *m_FVGrid;
	ImportDataObject *m_data_object;

	CList<CPropertyPage *, CPropertyPage *> m_page_list;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTIMPORT_STEP1_H__694A531B_1C2D_4A0F_A813_6261EFE54D37__INCLUDED_)
