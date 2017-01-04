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

// ImportPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "param.h"
#include "Import_Step3.h"
#include "err.h"
#include "file.h"
#include "TextParser.h"
#include "TextImport_Step1.h"

/////////////////////////////////////////////////////////////////////////////
// CImportPropSheet

IMPLEMENT_DYNAMIC(CImportPropSheet, CPropertySheet)

CImportPropSheet::CImportPropSheet(Parser *parser, CWnd* pParentWnd) : 
	CPropertySheet("", pParentWnd), m_parser(parser), m_bWizardFinishedOK(FALSE)
{
   m_pParentWnd = pParentWnd;
}

CImportPropSheet::CImportPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage), m_bWizardFinishedOK(FALSE)
{
   m_pParentWnd = pParentWnd;
}

CImportPropSheet::CImportPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), m_bWizardFinishedOK(FALSE)
{
   m_pParentWnd = pParentWnd;
}

CImportPropSheet::~CImportPropSheet()
{
}


BEGIN_MESSAGE_MAP(CImportPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CImportPropSheet)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_SETTINGS, OnLoadSettings)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_SETTINGS, OnSaveSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportPropSheet message handlers

BOOL CImportPropSheet::OnInitDialog() 
{
	CRect rect, tabrect;
	int width;

	BOOL bResult = CPropertySheet::OnInitDialog();

	GetDlgItem(IDCANCEL)->GetWindowRect(rect);
	GetTabControl()->GetWindowRect(tabrect);
	ScreenToClient(rect); ScreenToClient(tabrect);
	
	width = rect.Width();
	rect.left = tabrect.left; 
	rect.right = tabrect.left + width;
	
	// create the Save Settings button
	m_save_settings_btn.Create("Save Settings", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP, 
		rect, this, IDC_BUTTON_SAVE_SETTINGS);
	m_save_settings_btn.SetFont(GetFont());

	// create the Load Settings button
	rect.left += width;
	rect.right = rect.left + width;
	m_load_settings_btn.Create("Load Settings", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP, 
		rect, this, IDC_BUTTON_LOAD_SETTINGS);
	m_load_settings_btn.SetFont(GetFont());
	
	return 0;
}

void CImportPropSheet::OnLoadSettings()
{
	CString initial_dir = PRM_get_registry_string("Main", "USER_DATA", "");

   // Make sure the ImportSettings directory exists
   FIL_create_directory(initial_dir + "\\ImportSettings");

	initial_dir += "\\ImportSettings\\*.ipt";
   static char szFilter[] = "Import Settings (*.ipt)|*.ipt||";
	CFileDialog dlg(TRUE, ".ipt", initial_dir, OFN_FILEMUSTEXIST, szFilter, m_pParentWnd);
	dlg.m_ofn.lpstrTitle = "Load Import Settings";

	if (dlg.DoModal() == IDOK)
	{
		if (m_parser->load_settings(dlg.GetPathName()) == SUCCESS)
      {
         if (m_parser->is_kind_of("TextParser"))
         {
            TextParser *pTextParser = (TextParser *)m_parser;
            CTextImport_Step1 *pStep1 = (CTextImport_Step1 *)GetPage(0);
            
            CList<CPropertyPage *, CPropertyPage *> page_list;
            pStep1->get_page_list(page_list);
            
            SetupPages(pTextParser->is_text_delimited(), page_list); 
         }
         
         int nNumPages = GetPageCount();
         for (int i=0;i<nNumPages;++i)
         {
            CImportPropertyPage *pPage = (CImportPropertyPage *)GetPage(i);
            pPage->Reinit();
         }
         
         // if the settings are loaded, switch to the last page
         SetActivePage(GetPageCount() - 1);
         PressButton(PSBTN_FINISH);
      }
      else
         AfxMessageBox("Error loading .ipt file.");
	}
}

void CImportPropSheet::OnSaveSettings()
{
	CString initial_dir = PRM_get_registry_string("Main", "USER_DATA", "");
   
   // Make sure the ImportSettings directory exists
   FIL_create_directory(initial_dir + "\\ImportSettings");

	initial_dir += "\\ImportSettings\\*.ipt";

   static char szFilter[] = "Import Settings (*.ipt)|*.ipt||";
	CFileDialog dlg(FALSE, ".ipt", initial_dir, OFN_HIDEREADONLY | 
		OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN, szFilter, m_pParentWnd);
	dlg.m_ofn.lpstrTitle = "Save Import Settings";

	if (dlg.DoModal() == IDOK)
	{
		m_parser->save_settings(dlg.GetPathName());
	}
}

void CImportPropSheet::SetupPages(BOOL bDelimitedNotFixedWidth, CList<CPropertyPage *, CPropertyPage *> &page_list)
{
   POSITION position = page_list.GetHeadPosition();
   int index = 0;
   CPropertyPage *old_page_1 = GetPage(1);
   while (position)
   {
      CPropertyPage *page = page_list.GetNext(position);
      
      if (index < 2 && old_page_1 == page)
         RemovePage(page);
      else if (index > 1)
         RemovePage(page);
      
      index++;
   }
   
   position = page_list.GetHeadPosition();
   index = 0;
   while (position)
   {
      // conditionally add the Step2A or Step2B (one or the other but not both)
      CPropertyPage *page = page_list.GetNext(position);
      if (index == 0 && bDelimitedNotFixedWidth)
         AddPage(page);
      if (index == 1 && !bDelimitedNotFixedWidth)
         AddPage(page);
      
      if (index > 1)
         AddPage(page);
      
      index++;
   }	
}


// ImportDataObject
//

ImportDataObject::ImportDataObject() : TabularEditorDataObject(NULL)
{
	m_data_format_page = NULL;
}

int ImportDataObject::set_parser(Parser *parser)
{
	m_parser = parser;

	return create_preview();
}

long ImportDataObject::GetNumRows()
{
	return IMPORT_PREVIEW_ROW_COUNT;
}

int ImportDataObject::GetNumCols()
{
	return m_num_columns;
}

int ImportDataObject::GetColName(int col, CString *string)
{
	CList<CString, CString> &import_header_list = m_parser->get_properties()->get_column_header_list();

	POSITION pos = import_header_list.FindIndex(col);
	if (pos)
	{
		*string = import_header_list.GetAt(pos);
		return UG_SUCCESS;
	}

	return UG_ERROR;
}
	
int ImportDataObject::GetCell(int col,long row,CUGCell *cell)
{
	// get column headers
	if ( col >= 0 && row == -1 )
	{
		CString column_name;
		GetColName(col, &column_name);
		cell->SetText(column_name);
	}
	else if (col >= 0 && row >= 0 && row < IMPORT_PREVIEW_ROW_COUNT)
	{
		POSITION pos = m_cell_data[row].FindIndex(col);
		if (pos)
			cell->SetText(m_cell_data[row].GetAt(pos));
		else
			cell->SetText("");
		
		const int start_row = m_parser->get_properties()->get_start_row() - 1;
		if (row < start_row)
			cell->SetBackColor(RGB(214,211,206));
		else
		{
			cell->SetBackColor((col == m_current_col && m_active_page == 3) 
				? RGB(0xFF, 0xFF, 0xE0) : GetSysColor(COLOR_WINDOW));
		}
	}

	return UG_SUCCESS;
}

int ImportDataObject::OnEditStart(int col, long row,CWnd **edit)
{
	return FALSE;
}

int ImportDataObject::OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey)
{
	return TRUE;
}

int ImportDataObject::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	return TRUE;
}

void ImportDataObject::on_selected(int row)
{

}

int ImportDataObject::create_preview()
{
	if (m_parser->open_file() != SUCCESS)
	{
		ERR_report("Unable to open parser file");
		return FAILURE;
	}

	// calculate the necessary number of columns
	m_num_columns = 0;
	int i;
	for(i=0;i<IMPORT_PREVIEW_ROW_COUNT;i++)
	{
		CList<CString, CString> token_list;

		m_parser->get_tokens(token_list);

		POSITION pos = token_list.GetHeadPosition();
		m_cell_data[i].RemoveAll();
		while(pos)
			m_cell_data[i].AddTail(token_list.GetNext(pos));

		// figure out the max column width
		if (token_list.GetCount() > m_num_columns)
			m_num_columns = token_list.GetCount();
	}

	m_parser->close_file();

	// initially fill import headers
	CList<CString, CString> &import_header_list = m_parser->get_properties()->get_column_header_list();
   const CString strDefaultImportHeader = m_parent_data_object->GetDefaultImportHeader();
   if (import_header_list.GetCount() != m_num_columns)
   {
      import_header_list.RemoveAll();
      for(i=0;i<m_num_columns;i++)
         import_header_list.AddTail(strDefaultImportHeader);
   }

	m_current_col = 0;

	return SUCCESS;
}

void ImportDataObject::OnLClicked(int col, long row)
{
	if (col >= 0 && col < m_num_columns)
	{
		m_current_col = col;

		if (m_ctrl->m_hWnd)
			m_ctrl->Invalidate();

		if (m_active_page == 3)
			m_data_format_page->on_left_click(col);
	}
}

void ImportDataObject::OnTH_LClicked(int col, long row)
{
   OnLClicked(col, row);
}

// called when the parser properties are changed
void ImportDataObject::update()
{
	create_preview();
	
	if (m_ctrl->m_hWnd == NULL)
		return;

	m_ctrl->SetPaintMode(FALSE);
	{	
		m_ctrl->SetGridUsingDataSource(m_ctrl->GetDefDataSource());
		m_ctrl->BestFit(0, m_ctrl->GetNumberCols() - 1, 0, UG_BESTFIT_TOPHEADINGS);
	}
	m_ctrl->SetPaintMode(TRUE);
}

CImportPropertyPage::CImportPropertyPage() : CPropertyPage()
{
};

CImportPropertyPage::CImportPropertyPage( UINT nIDTemplate, UINT nIDCaption)
   : CPropertyPage(nIDTemplate, nIDCaption)
{
}

CImportPropertyPage::CImportPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption)
   : CPropertyPage(lpszTemplateName, nIDCaption)
{

}
