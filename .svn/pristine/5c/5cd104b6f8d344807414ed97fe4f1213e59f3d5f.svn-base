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

// DataExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\fvw.h"
#include "DataExportDlg.h"
#include "err.h"
#include "tabulareditordlg.h"
#include "FVGridCtrl.h"
#include "param.h"
#include "overlay.h"
#include "OvlFctry.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// CDataExportDlg dialog


CDataExportDlg::CDataExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataExportDlg)
	m_output_path = _T("");
	//}}AFX_DATA_INIT
}

void CDataExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataExportDlg)
	DDX_Control(pDX, IDC_TAB_DELIMITED, m_tab_delimited_ctrl);
	DDX_Control(pDX, IDC_COMMA_DELIMITED, m_comma_delimited_ctrl);
	DDX_Text(pDX, IDC_OUTPUT_PATH, m_output_path);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress_ctrl);
   DDX_Control(pDX, IDC_EXPORT_HEADERS, m_ckExportHeaders);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataExportDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CDataExportDlg)
	ON_BN_CLICKED(IDC_BROWSE_PATH, OnBrowsePath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataExportDlg message handlers

BOOL CDataExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_tab_delimited_ctrl.SetCheck(1);

	// hide the export button until a file name has been set
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_progress_ctrl.ShowWindow(SW_HIDE);

   m_ckExportHeaders.SetCheck(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataExportDlg::OnBrowsePath() 
{
   CString strInitialDir;
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(
      m_data_object->get_parent_overlay()->get_m_overlayDescGuid());
   if (pOverlayTypeDesc != NULL)
   {
      if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      {
         strInitialDir = pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory;
         strInitialDir += "\\*.txt";
      }
   }
   else
      strInitialDir = PRM_get_registry_string("Main", "USER_DATA", "") + "\\*.txt";

	static char filter[] = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, "txt", strInitialDir, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, this);

	// get a filename from the user and save the file
	if (dlg.DoModal() == IDOK)
	{  
		// save the current template to disk
		m_output_path = dlg.GetPathName();
		UpdateData(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CDataExportDlg::OnOK() 
{
	// export the data to the specified file
	const char delimiter = m_tab_delimited_ctrl.GetCheck() ? '\t' : ',';
	const char* new_line = "\r\n";

	CWaitCursor wait;

	CFile file;
	CFileException ex;
	if (!file.Open(m_output_path, CFile::modeWrite | 
		CFile::shareExclusive | CFile::modeCreate, &ex))
   {
      ERR_report_exception("Couldn't open source file for writing", ex);
      AfxMessageBox("Couldn't open source file for writing.  See error log for details.");      
      return;
   }

	int row_count = m_data_object->GetNumRows();

   // local points should not export the last row (new uninitialized point)
   if (m_data_object->is_kind_of("LocalPointDataObject"))
      row_count--;

	m_progress_ctrl.ShowWindow(SW_SHOW);
	m_progress_ctrl.SetRange(0, row_count);
	m_progress_ctrl.SetPos(0);
	m_progress_ctrl.SetStep(1);

	m_progress_ctrl.Invalidate();
	m_progress_ctrl.UpdateWindow();

   const int col_count = m_data_object->GetNumCols();

   // write out column headings
   if (m_ckExportHeaders.GetCheck())
   {
      CString line;
      for(int col=0;col<col_count;col++)
		{
         CUGCell cell;
         m_FVGrid->GetCellIndirect(col, -1, &cell);

			line += cell.GetText() + CString(delimiter);
      }

      // write out line (except for extra last delimter) along with a new line
		file.Write(line.GetBuffer(line.GetLength() - 1), line.GetLength() - 1);
		file.Write(new_line, 2);
   }
	
   for(int row=0;row<row_count;row++)
   {
	   CString line;
	   for(int col=0;col<col_count;col++)
	   {
		   CUGCell cell;
		   m_FVGrid->GetCellIndirect(col,row,&cell);

		   if (cell.GetCellType() == FVCT_NOTE)
		   {
			   CString strLabelText = cell.GetLabelText();
			   line += strLabelText + CString(delimiter);
		   }
		   else
		   {
			   // replace commas (e.g. "1,246") so that the file can be reimported as comma-delimited
			   CString strCellText = cell.GetText();
			   strCellText.Replace(",", "");
			   line += strCellText + CString(delimiter);
		   }
	   }

		line.Replace("\n", "|");

		// write out line (except for extra last delimter) along with a new line
		file.Write(line.GetBuffer(line.GetLength() - 1), line.GetLength() - 1);
		file.Write(new_line, 2);

		m_progress_ctrl.StepIt();
		m_progress_ctrl.Invalidate();
		m_progress_ctrl.UpdateWindow();
	}
	
	CDialog::OnOK();
}
LRESULT CDataExportDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

