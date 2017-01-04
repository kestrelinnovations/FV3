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

// SelectTableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SelectTableDlg.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CSelectTableDlg dialog

const int ICON_DIM_X = 16;
const int ICON_DIM_Y = 16;
const int ICON_LST_INITIAL_SIZE = 20;
const int ICON_LST_GROW_BY = 5;

CSelectTableDlg::CSelectTableDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectTableDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectTableDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelectTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectTableDlg)
	DDX_Control(pDX, IDC_FIRST_ROW, m_first_row_ctrl);
	DDX_Control(pDX, IDC_LIST, m_list_ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectTableDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CSelectTableDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectTableDlg message handlers

BOOL CSelectTableDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_images.Create(ICON_DIM_X, ICON_DIM_Y, ILC_COLORDDB | ILC_MASK, 
		ICON_LST_INITIAL_SIZE, ICON_LST_GROW_BY);
	m_images.SetBkColor(RGB(255,255,255));
	HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_TABLE));
	m_images.Add(hIcon);
	
	// initialize file list control
	//

	LV_COLUMN lc;

	// initialize lc common entries
	memset(&lc, 0, sizeof(LV_COLUMN));
	lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lc.fmt = LVCFMT_LEFT;

	// initialize the Name column
   lc.cx = 200;
   lc.pszText = "Name";
   lc.iSubItem = 0;
   m_list_ctrl.InsertColumn(1, &lc);

	// initialize the image list used in both the overlay list
	// and the list of files
	m_list_ctrl.SetImageList(&m_images, LVSIL_SMALL);

	// add the table names to the list
	POSITION position = m_table_list.GetHeadPosition();
	while (position)
	{
		const CString table_name = m_table_list.GetNext(position);
		const int image_pos = 0;

		m_list_ctrl.InsertItem(m_list_ctrl.GetItemCount(), table_name, image_pos);
	}

	m_list_ctrl.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	m_list_ctrl.SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
}

void CSelectTableDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}

void CSelectTableDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

   OnOK();
}


void CSelectTableDlg::OnOK() 
{
	int m_first_row_contains_header = m_first_row_ctrl.GetCheck();
	
	m_sheet_num = 0;
	POSITION pos = m_list_ctrl.GetFirstSelectedItemPosition();
	if (pos)
		m_sheet_num = m_list_ctrl.GetNextSelectedItem(pos);

	CDialog::OnOK();
}
LRESULT CSelectTableDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

