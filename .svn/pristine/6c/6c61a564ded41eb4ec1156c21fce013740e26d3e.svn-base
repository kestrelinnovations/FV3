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

// Import_Step3.cpp : implementation file
//

#include "stdafx.h"
#include "err.h"
#include "Import_Step3.h"


#include "Parser.h"
#include "FVGridCtrl.h"
#include "ImportPropSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CImport_Step3 property page

IMPLEMENT_DYNCREATE(CImport_Step3, CPropertyPage)

CImport_Step3::CImport_Step3(Parser *parser, ImportDataObject *data_object) : 
	CImportPropertyPage(CImport_Step3::IDD), m_parser(parser), m_data_object(data_object)
{
	//{{AFX_DATA_INIT(CImport_Step3)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// change the name of the dialog based on the parser
	if (m_parser->is_kind_of("ExcelParser"))
	{
		m_psp.dwFlags |= PSP_USETITLE;
		m_psp.pszTitle = "Excel Import Wizard - Step 1 of 2";
	}

	m_data_object->set_data_format_page(this);
	m_FVGrid = NULL;
}

CImport_Step3::~CImport_Step3()
{
	delete m_FVGrid;
}

void CImport_Step3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImport_Step3)
	DDX_Control(pDX, IDC_DATA_FORMAT_EXT, m_data_format_ext);
	DDX_Control(pDX, IDC_DATA_FORMAT, m_data_format);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImport_Step3, CPropertyPage)
	//{{AFX_MSG_MAP(CImport_Step3)
	ON_CBN_SELCHANGE(IDC_DATA_FORMAT_EXT, OnModified)
	ON_CBN_SELCHANGE(IDC_DATA_FORMAT, OnDataFormat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImport_Step3 message handlers

BOOL CImport_Step3::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

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
	}
	m_FVGrid->SetPaintMode(TRUE);

	m_data_format_ext.ShowWindow(SW_HIDE);
	
	// set potential column headers from the currently set data object
	//
	m_column_info_list.RemoveAll();

   m_data_object->GetParentDataObject()->GetColumnInfoList(m_column_info_list);
   POSITION pos = m_column_info_list.GetHeadPosition();
   while (pos)
      m_data_format.AddString(m_column_info_list.GetNext(pos).column_name);

	m_data_format.AddString("Do not import");

   CString strDefaultImportHeader = m_data_object->GetParentDataObject()->GetDefaultImportHeader();
	
	// auto header detection
	//
	m_parser->open_file();

	CList<CString,CString> token_lst;
	m_parser->get_tokens(token_lst);

	CList<CString, CString> &import_header_list = m_parser->get_properties()->get_column_header_list();
	POSITION position = import_header_list.GetHeadPosition();
	pos = token_lst.GetHeadPosition();
	while (position && pos)
	{
      CString &string = import_header_list.GetAt(position);

		CString token = token_lst.GetNext(pos);
		token.MakeUpper().TrimLeft().TrimRight();

      // if the value has already been set, then don't change it
      if (string != strDefaultImportHeader)
      {
         import_header_list.GetNext(position);
         continue;
      }

      // try matching header names with column names
      //
      POSITION posColumnInfoList = m_column_info_list.GetHeadPosition();
      while (posColumnInfoList)
      {
         CString strColumnName = m_column_info_list.GetNext(posColumnInfoList).column_name;
         if (token.CompareNoCase(strColumnName) == 0)
         {
            import_header_list.SetAt(position, strColumnName);
            break;
         }
      }

      // try matching with some other common header names
      //
		if (token == "ELEV")
			import_header_list.SetAt(position, "Elevation (in feet)");
		else if (token == "MGRS")
			import_header_list.SetAt(position, "Location [ MGRS ]");
		else if (token == "GEO" ||
			      token == "LAT/LONG" ||
					token == "LOCATION" ||
					token == "COORD" ||
					(token.Find("LAT") != -1 && token.Find("LON") != -1))
			import_header_list.SetAt(position, "Location [ Latitude and Longitude ]");
		else if (token.Find("LAT") != -1)
			import_header_list.SetAt(position, "Location [ Latitude Only ]");
		else if (token.Find("LON") != -1)
			import_header_list.SetAt(position, "Location [ Longitude Only ]");
		else if (token.Find("LINK") != -1)
			import_header_list.SetAt(position, "Links");

		import_header_list.GetNext(position);
	}

   m_parser->close_file();

	Reinit();

	on_left_click(m_data_object->get_current_col());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImport_Step3::Reinit()
{
   if (m_FVGrid == NULL || m_FVGrid->m_hWnd == NULL)
      return;

   m_data_object->update();
   m_FVGrid->Invalidate();
   m_FVGrid->BestFit(0, m_FVGrid->GetNumberCols() - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CImport_Step3::on_left_click(int col)
{
	CList<CString, CString> &import_header_list = m_parser->get_properties()->get_column_header_list();
	POSITION pos = import_header_list.FindIndex(col);
	if (pos)
	{
		CString str = import_header_list.GetAt(pos);
		m_data_format.SelectString(0, str);
		set_extended_formats(m_data_format.GetCurSel());
	}
}

void CImport_Step3::OnDataFormat()
{
	int index = m_data_format.GetCurSel();
	if (index == CB_ERR)
	{
		ERR_report("Unable to obtain current selection from combo box");
		return;
	}
	set_extended_formats(index);

	OnModified();
}

bool CImport_Step3::set_extended_formats(int index)
{
	if (index >= m_column_info_list.GetCount())
	{
		// index out of range
		m_data_format_ext.ShowWindow(SW_HIDE);
		m_data_format_ext.ResetContent();
		
		return true;
	}
	
	// add extended data formats, if any
	POSITION pos = m_column_info_list.FindIndex(index);
	if (pos)
	{
		column_info_t info = m_column_info_list.GetAt(pos);
		
		if (info.column_name == "Location")
		{
			m_data_format_ext.ShowWindow(SW_SHOW);
			m_data_format_ext.ResetContent();
			
			m_data_format_ext.AddString("Latitude and Longitude");
			m_data_format_ext.AddString("Latitude Only");
			m_data_format_ext.AddString("Longitude Only");
			m_data_format_ext.AddString("MGRS");
			
			m_data_format_ext.SetCurSel(0);
			
			return false;
		}
		else
		{
			m_data_format_ext.ShowWindow(SW_HIDE);
			m_data_format_ext.ResetContent();
			
			return true;
		}
	}
	
	return true;
}

void CImport_Step3::OnModified() 
{
	int index = m_data_format.GetCurSel();
	
	CString string;
	m_data_format.GetLBText(index, string);
	
	bool hidden = true;
	POSITION pos = m_column_info_list.FindIndex(index);
	if (pos)
	{
		column_info_t info = m_column_info_list.GetAt(pos);
		if (info.column_name == "Location")
			hidden = false;
	}
	
	if (!hidden)
	{
		int ext_index = m_data_format_ext.GetCurSel();
		if (ext_index != CB_ERR)
		{
			CString ext_str;
			m_data_format_ext.GetLBText(ext_index, ext_str);
			string += " [ " + ext_str + " ]";
		}
	}
	
	CList<CString, CString> &import_header_list = m_parser->get_properties()->get_column_header_list();
	pos = import_header_list.FindIndex(m_data_object->get_current_col());
	if (pos)
		import_header_list.SetAt(pos, string);
	m_FVGrid->Invalidate();
	m_FVGrid->BestFit(0, m_FVGrid->GetNumberCols() - 1, 0, UG_BESTFIT_TOPHEADINGS);

   // The next page, which contains a list of available filter columns,
   // needs to be reinitialized
   CPropertySheet* pPropSheet = (CPropertySheet*) GetParent();
   if (pPropSheet)
   {
      const int nActiveIndex = pPropSheet->GetActiveIndex();
      CImportPropertyPage *pImportPropPage = (CImportPropertyPage *)pPropSheet->GetPage(nActiveIndex + 1);
      if (pImportPropPage)
         pImportPropPage->Reinit();
   }
}

// This is the third wizard property page. Enable the Back 
// button and change the Next button to Finish. If dwFlags is equal 
// to PSWIZB_BACK | PSWIZB_DISABLEDFINISH, then the Back
// button is enabled but the Next button is disabled.
BOOL CImport_Step3::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_data_object->set_active_page(3);
	
	return CPropertyPage::OnSetActive();
}
