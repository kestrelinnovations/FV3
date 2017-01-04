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

// TextImport_Step1.cpp : implementation file
//

#include "stdafx.h"
#include "TextImport_Step1.h"
#include "TextParser.h"
#include "FVGridCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step1 property page

IMPLEMENT_DYNCREATE(CTextImport_Step1, CPropertyPage)

CTextImport_Step1::CTextImport_Step1(TextParser *text_parser, 
												 ImportDataObject *data_object) : 
	CImportPropertyPage(CTextImport_Step1::IDD), m_text_parser(text_parser), m_data_object(data_object)
{
	//{{AFX_DATA_INIT(CTextImport_Step1)
	m_start_row = 2;
	//}}AFX_DATA_INIT
	m_FVGrid = NULL;

   m_text_parser->get_properties()->set_start_row(m_start_row);
}

CTextImport_Step1::~CTextImport_Step1()
{
	delete m_FVGrid;
}

void CTextImport_Step1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextImport_Step1)
	DDX_Control(pDX, IDC_SPIN2, m_spin_ctrl);
	DDX_Control(pDX, IDC_IMPORT_DELIMITED, m_delimited);
	DDX_Control(pDX, IDC_IMPORT_FIXED_WIDTH, m_fixed_width);
	DDX_Text(pDX, IDC_IMPORT_START_ROW, m_start_row);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextImport_Step1, CPropertyPage)
	//{{AFX_MSG_MAP(CTextImport_Step1)
	ON_BN_CLICKED(IDC_IMPORT_DELIMITED, OnModified)
	ON_BN_CLICKED(IDC_IMPORT_FIXED_WIDTH, OnModified)
	ON_EN_CHANGE(IDC_IMPORT_START_ROW, OnModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step1 message handlers

BOOL CTextImport_Step1::OnInitDialog() 
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

void CTextImport_Step1::Reinit()
{
   TextParserProperties *prop = (TextParserProperties *)m_text_parser->get_properties();

   if (m_delimited.m_hWnd == NULL)
      return;

   // determine if the text data is delimited or fixed width and
	// set the radio buttons accordingly
	const int delimited = m_text_parser->is_text_delimited();
	m_delimited.SetCheck(delimited);
	m_fixed_width.SetCheck(!delimited);

   m_start_row = prop->get_start_row();

	// set the range for the spin control
	m_spin_ctrl.SetRange32(1, m_text_parser->get_number_lines());

   // update the data preview grid control
	m_data_object->update();

   UpdateData(FALSE);
}

// a value of a control has changed
void CTextImport_Step1::OnModified() 
{
	if (m_delimited.m_hWnd == NULL)
		return;

	UpdateData();

	// update the values in the text import object
	//

	TextParserProperties *prop = (TextParserProperties *)m_text_parser->get_properties();

	prop->set_is_delimited(m_delimited.GetCheck());
	prop->set_start_row(m_start_row);

	// change the pages in the propery sheet based on the selection
	CImportPropSheet *psheet = (CImportPropSheet*) GetParent();
	if (psheet != NULL)
      psheet->SetupPages(m_delimited.GetCheck(), m_page_list);

	// update the data preview grid control
	m_data_object->update();
   m_FVGrid->Invalidate();
}

// This is the first wizard property page.  Disable the Back 
// button but enable the Next button.
BOOL CTextImport_Step1::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_NEXT);

	m_data_object->set_active_page(1);
   
   return CPropertyPage::OnSetActive();
}
