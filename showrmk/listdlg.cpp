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



// listdlg.cpp : implementation file
//

#include "stdafx.h"
#include "showrmk.h"

/////////////////////////////////////////////////////////////////////////////
// CRemarkListDlg dialog


CRemarkListDlg::CRemarkListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemarkListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemarkListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_title = "";
}


void CRemarkListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemarkListDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemarkListDlg, CDialog)
	//{{AFX_MSG_MAP(CRemarkListDlg)
	ON_WM_SIZE()
	ON_LBN_DBLCLK(IDC_RMK_LIST, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemarkListDlg message handlers

void CRemarkListDlg::OnDblclkList1() 
{
	char tstr[81];
	int pos;

	pos = SendDlgItemMessage(IDC_RMK_LIST, LB_GETCURSEL, 0, 0);
	if (pos != LB_ERR)
	{
		SendDlgItemMessage(IDC_RMK_LIST, LB_GETTEXT, pos, (LPARAM) (LPCSTR) tstr);
		m_selection = tstr;
	}
	else
		return;
	
	CDialog::OnOK();
}

// *****************************************************************
// *****************************************************************

void CRemarkListDlg::OnCancel() 
{
	CString *txt;

	// delete the string list
	while (!m_list.IsEmpty())
	{
		txt = m_list.RemoveHead();
		delete txt;
	}
	
	CDialog::OnCancel();
}

// *****************************************************************
// *****************************************************************

void CRemarkListDlg::OnOK() 
{
	char tstr[81];
	int pos;
	CString *txt;

	pos = SendDlgItemMessage(IDC_RMK_LIST, LB_GETCURSEL, 0, 0);
	if (pos != LB_ERR)
	{
		SendDlgItemMessage(IDC_RMK_LIST, LB_GETTEXT, pos, (LPARAM) (LPCSTR) tstr);
		m_selection = tstr;
	}
	
	// delete the string list
	while (!m_list.IsEmpty())
	{
		txt = m_list.RemoveHead();
		delete txt;
	}
	CDialog::OnOK();
}

// *****************************************************************
// *****************************************************************

void CRemarkListDlg::set_title(CString title)
{
	m_title = title;
}

// *****************************************************************
// *****************************************************************

BOOL CRemarkListDlg::OnInitDialog() 
{
	POSITION next;
	CString *txt;
//	char tstr[81];
	CString tstr;
	int cnt;

	CDialog::OnInitDialog();

	cnt = m_list.GetCount();
	if (cnt < 1)
		return TRUE;

	next = m_list.GetHeadPosition();
	while (next != NULL)
	{
		txt = m_list.GetNext(next);
//		strncpy(tstr, (*txt).GetBuffer(30), 30);
//		tstr[30] = '\0';
		tstr = *txt;
	   SendDlgItemMessage(IDC_RMK_LIST, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) tstr);
	}		
	
	if (m_title.GetLength() > 0)
		SetWindowText(m_title);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// *****************************************************************
// *****************************************************************

void CRemarkListDlg::init_list(const CList<CString*, CString*> & strlist) 
{
	POSITION next;
	CString *txt, *intxt;
	CString tstr;

	if (strlist.GetCount() < 1)
		return;

	// make sure the list is empty 
	while (!m_list.IsEmpty())
	{
		txt = m_list.RemoveHead();
		delete txt;
	}

	next = strlist.GetHeadPosition();
	while (next != NULL)
	{
		intxt = new CString;
		txt = strlist.GetNext(next);
		tstr = *txt;
		*intxt = tstr;
		m_list.AddTail(intxt);
	}
}

// *****************************************************************
// *****************************************************************

void CRemarkListDlg::get_selection(CString & selection) 
{
	CString *txt;

	selection = m_selection;

	// delete the string list
	while (!m_list.IsEmpty())
	{
		txt = m_list.RemoveHead();
		delete txt;
	}
}

// *****************************************************************
// ****************************************************************

void CRemarkListDlg::OnSize(UINT nType, int cx, int cy)
{
	int ctx, width, height, max_width, max_height;
	CRect rc;
	HWND twnd;

	max_width = 290;
	max_height = 250;

	GetWindowRect(&rc);
	width = rc.Width();
	height = rc.Height();
	if ((width < max_width) || (height < max_height))
	{
		CDialog::OnSize(nType, max_width, max_height);
		MoveWindow(rc.left, rc.top, max_width, max_height);
		return;
	}

	ctx = width / 2;
	twnd = GetDlgItem(IDC_RMK_LIST)->GetSafeHwnd();
	if (twnd == NULL)
		return;
	::MoveWindow(twnd,  12, 12, width - 30, height - 80, TRUE);

	twnd = GetDlgItem(IDOK)->GetSafeHwnd();
	if (twnd == NULL)
		return;
	::MoveWindow(twnd,  ctx-43, height-59, 75, 23, TRUE);

	twnd = GetDlgItem(IDCANCEL)->GetSafeHwnd();
	if (twnd == NULL)
		return;
	::MoveWindow(twnd,  width-93, height-59, 75, 23, TRUE);

	CDialog::OnSize(nType, cx, cy);
}
// end of OnSize

// ****************************************************************
// *****************************************************************

// *****************************************************************
// *****************************************************************

