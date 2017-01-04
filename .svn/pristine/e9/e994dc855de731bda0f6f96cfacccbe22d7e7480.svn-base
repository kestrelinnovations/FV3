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

// HideShowColumnsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "hideshowcolumnsdlg.h"
#include "TabularEditorDlg.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CHideShowColumnsDlg dialog


CHideShowColumnsDlg::CHideShowColumnsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHideShowColumnsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHideShowColumnsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHideShowColumnsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHideShowColumnsDlg)
	DDX_Control(pDX, IDC_TREE, m_tree_ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHideShowColumnsDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CHideShowColumnsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHideShowColumnsDlg message handlers

BOOL CHideShowColumnsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_tree_ctrl.ModifyStyle(TVS_CHECKBOXES, 0);
   m_tree_ctrl.ModifyStyle(0, TVS_CHECKBOXES);
	
	// fill the tree control with available columns and set
	// the hide/show check as appropriate
	m_data_object->SetTranslation(FALSE);
	int num_columns = m_data_object->GetNumCols();
	for(int col_num=0;col_num<num_columns;col_num++)
	{
		CString column_name;
		m_data_object->GetColName(col_num, &column_name);

		const BOOL is_shown = m_data_object->GetIsShown(col_num);

		HTREEITEM hItem = m_tree_ctrl.InsertItem(column_name);
		m_tree_ctrl.SetCheck(hItem, is_shown);
	}
	m_data_object->SetTranslation(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHideShowColumnsDlg::OnOK() 
{
	// store hide/show flags for the columns
	//

	CList<BOOL, BOOL> show_flags_list;

	HTREEITEM hItem = m_tree_ctrl.GetNextItem(NULL, TVGN_CHILD );
	int col_num = 0;
	BOOL at_least_one_shown_flag_selected = FALSE;
	while(hItem)
	{
		BOOL is_shown = m_tree_ctrl.GetCheck(hItem);
		if (is_shown)
			at_least_one_shown_flag_selected = TRUE;

		show_flags_list.AddTail(is_shown);
		hItem = m_tree_ctrl.GetNextItem(hItem, TVGN_NEXT);	
	}

	// if no flags are set, then automatically select the first one
	if (!at_least_one_shown_flag_selected && show_flags_list.GetCount())
		show_flags_list.SetAt(show_flags_list.GetHeadPosition(), TRUE);

	m_data_object->SetIsShown(show_flags_list);

	CDialog::OnOK();
}
LRESULT CHideShowColumnsDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

