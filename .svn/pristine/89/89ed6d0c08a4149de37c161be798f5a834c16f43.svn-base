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

// TextImport_Step2A.cpp : implementation file
//

#include "stdafx.h"
#include "TextImport_Step2A.h"
#include "TextParser.h"
#include "FVGridCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step2A property page

IMPLEMENT_DYNCREATE(CTextImport_Step2A, CPropertyPage)

CTextImport_Step2A::CTextImport_Step2A(TextParser *text_parser, ImportDataObject *data_object) : 
	CImportPropertyPage(CTextImport_Step2A::IDD), m_text_parser(text_parser), m_data_object(data_object)
{
	//{{AFX_DATA_INIT(CTextImport_Step2A)
	m_edit = _T("");
	//}}AFX_DATA_INIT
	m_FVGrid = NULL;
}

CTextImport_Step2A::~CTextImport_Step2A()
{
	delete m_FVGrid;
}

void CTextImport_Step2A::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextImport_Step2A)
	DDX_Control(pDX, IDC_CONSECUTIVE, m_consecutive);
	DDX_Control(pDX, IDC_IMPORT_COMMA, m_comma);
	DDX_Control(pDX, IDC_IMPORT_OTHER, m_other);
	DDX_Control(pDX, IDC_IMPORT_SEMICOLON, m_semicolon);
	DDX_Control(pDX, IDC_IMPORT_SPACE, m_space);
	DDX_Control(pDX, IDC_IMPORT_TAB, m_tab);
	DDX_Text(pDX, IDC_OTHER_EDIT, m_edit);
	DDV_MaxChars(pDX, m_edit, 1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextImport_Step2A, CPropertyPage)
	//{{AFX_MSG_MAP(CTextImport_Step2A)
	ON_BN_CLICKED(IDC_CONSECUTIVE, OnModified)
	ON_BN_CLICKED(IDC_IMPORT_COMMA, OnModified)
	ON_BN_CLICKED(IDC_IMPORT_OTHER, OnModified)
	ON_BN_CLICKED(IDC_IMPORT_SEMICOLON, OnModified)
	ON_BN_CLICKED(IDC_IMPORT_SPACE, OnModified)
	ON_BN_CLICKED(IDC_IMPORT_TAB, OnModified)
	ON_EN_CHANGE(IDC_OTHER_EDIT, OnModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step2A message handlers

BOOL CTextImport_Step2A::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// setup data preview
	//
	m_FVGrid = new FVGridCtrl;
	const int offset = 10;
	CRect rect;
	CWnd *wnd = GetDlgItem(IDC_FRAME);
	if (wnd)
	{
		wnd->GetWindowRect(rect);
		ScreenToClient(rect);
	}

	const UINT id = 1234;
	m_FVGrid->CreateGrid(WS_CHILD|WS_VISIBLE, rect, this, id);
	m_FVGrid->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);

	m_FVGrid->SetPaintMode(FALSE);
	{	
		int index = m_FVGrid->AddDataSource(m_data_object);
		m_FVGrid->SetDefDataSource(index);
		m_FVGrid->SetGridUsingDataSource(index);
		m_FVGrid->BestFit(0, m_FVGrid->GetNumberCols() - 1, 0, UG_BESTFIT_TOPHEADINGS);
	}
	m_FVGrid->SetPaintMode(TRUE);

   Reinit();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextImport_Step2A::Reinit()
{
   if (m_tab.m_hWnd == NULL)
      return;

   TextParserProperties *prop = (TextParserProperties *)m_text_parser->get_properties();

   int nDelimiter = prop->get_delimiter();
   m_tab.SetCheck(nDelimiter & DELIM_TAB);
   m_space.SetCheck(nDelimiter & DELIM_SPACE);
   m_semicolon.SetCheck(nDelimiter & DELIM_SEMICOLON);
   m_other.SetCheck(nDelimiter & DELIM_OTHER);
   m_comma.SetCheck(nDelimiter & DELIM_COMMA);

   if (nDelimiter & DELIM_OTHER)
      m_edit = prop->get_other_delimiter();

   m_consecutive.SetCheck(prop->get_consecutive_delimiters());

   m_data_object->update();
   UpdateData(FALSE);
}

void CTextImport_Step2A::OnModified() 
{
	if (m_tab.m_hWnd == NULL)
		return;

	TextParserProperties *prop = (TextParserProperties *)m_text_parser->get_properties();
	UpdateData();

	int delimiter = DELIM_NONE;
	
	if (m_tab.GetCheck())
		delimiter |= DELIM_TAB;

	if (m_space.GetCheck())
		delimiter |= DELIM_SPACE;

	if (m_semicolon.GetCheck())
		delimiter |= DELIM_SEMICOLON;

	if (m_other.GetCheck())
		delimiter |= DELIM_OTHER;

	if (m_comma.GetCheck())
		delimiter |= DELIM_COMMA;

	prop->set_delimiter(delimiter);
	prop->set_consecutive_delimiters(m_consecutive.GetCheck());

	if (m_edit.GetLength() == 1)
		prop->set_other_delimiter(m_edit[0]);

	m_data_object->update();
}

// This is the second wizard property page. Enable both the 
// Back button and the Next button.
BOOL CTextImport_Step2A::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_data_object->set_active_page(2);

	return CPropertyPage::OnSetActive();
}
