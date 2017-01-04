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

#if !defined(AFX_TEXTIMPORT_STEP2A_H__6A343055_5929_4B2D_8E38_289363E04F83__INCLUDED_)
#define AFX_TEXTIMPORT_STEP2A_H__6A343055_5929_4B2D_8E38_289363E04F83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextImport_Step2A.h : header file
//

#include "..\resource.h"
#include "ImportPropSheet.h"

// forward declarations
class TextParser;
class ImportDataObject;
class FVGridCtrl;

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step2A dialog

class CTextImport_Step2A : public CImportPropertyPage
{
	DECLARE_DYNCREATE(CTextImport_Step2A)

// Construction
public:
	CTextImport_Step2A(TextParser *text_parser = NULL, ImportDataObject *data_object = NULL);
	virtual ~CTextImport_Step2A();
   virtual void Reinit();

// Dialog Data
	//{{AFX_DATA(CTextImport_Step2A)
	enum { IDD = IDD_TAB_IMPORT_TEXT_STEP2A };
	CButton	m_consecutive;
	CButton	m_comma;
	CButton	m_other;
	CButton	m_semicolon;
	CButton	m_space;
	CButton	m_tab;
	CString	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTextImport_Step2A)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTextImport_Step2A)
	virtual BOOL OnInitDialog();
	afx_msg void OnModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	TextParser *m_text_parser;

	FVGridCtrl *m_FVGrid;
	ImportDataObject *m_data_object;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTIMPORT_STEP2A_H__6A343055_5929_4B2D_8E38_289363E04F83__INCLUDED_)
