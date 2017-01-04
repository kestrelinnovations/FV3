// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// fielddlg.cpp : implementation file
//

#include "stdafx.h"
#include "fielddlg.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CShpFieldSelectDlg dialog


CShpFieldSelectDlg::CShpFieldSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShpFieldSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShpFieldSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_active = FALSE;
}

// *****************************************************************
// *****************************************************************

CShpFieldSelectDlg::~CShpFieldSelectDlg()
{
}

// *****************************************************************
// *****************************************************************

void CShpFieldSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShpFieldSelectDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


// *****************************************************************
// *****************************************************************

BEGIN_MESSAGE_MAP(CShpFieldSelectDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CShpFieldSelectDlg)
	ON_LBN_DBLCLK(IDC_FIELD_LIST, OnDblclkFieldList)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShpFieldSelectDlg message handlers

// *****************************************************************
// *****************************************************************

BOOL CShpFieldSelectDlg::OnInitDialog() 
{
	RECT rc;

	CDialog::OnInitDialog();

	m_active = TRUE;

	GetWindowRect(&rc);

//	SetWindowPos(NULL, rc.left, rc.left, m_cx, m_cy, SWP_SHOWWINDOW);

	fill_field_list();

	update_field_list();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// end of OnInitDialog

// *****************************************************************
// *****************************************************************

void CShpFieldSelectDlg::OnCancel() 
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

void CShpFieldSelectDlg::OnOK() 
{
	int pos = SendDlgItemMessage(IDC_FIELD_LIST, LB_GETCURSEL, 0, 0);
	if (pos != LB_ERR)
	{
      char* tstr = new char[SendDlgItemMessage(IDC_FIELD_LIST, LB_GETTEXTLEN, pos) + 1];
		SendDlgItemMessage(IDC_FIELD_LIST, LB_GETTEXT, pos, (LPARAM) (LPCSTR) tstr);
		m_selection = tstr;
      delete [] tstr;
	}
	
	// delete the string list
	while (!m_list.IsEmpty())
		delete m_list.RemoveHead();
	
	CDialog::OnOK();
}
// end of OnOK

// *****************************************************************
// *****************************************************************

void CShpFieldSelectDlg::OnDblclkFieldList() 
{
	OnOK();
}

// *****************************************************************
// *****************************************************************

void CShpFieldSelectDlg::set_shape_database_name(CString filename) 
{
	m_filename = filename;
}
// end of init_list

// *****************************************************************
// *****************************************************************

void CShpFieldSelectDlg::get_selection(CString & selection) 
{
	CString *txt;

	selection = m_selection.Left(10);

	// delete the string list
	while (!m_list.IsEmpty())
	{
		txt = m_list.RemoveHead();
		delete txt;
	}
}
// end of get_selection

// *****************************************************************
// *****************************************************************

void CShpFieldSelectDlg::fill_field_list() 
{
	int rslt;
   const int FNAME_LEN = 21;
	char fname[FNAME_LEN];
	int len, dec;
	CString *txt; 
	CString msg;
	CString temp, data;

	// build the list of field names
//	CList<CString*, CString*> list;
	CDbase dbf;
   const int DBFNAME_LEN = 200;
	char dbfname[DBFNAME_LEN];

	if (m_filename.GetLength() < 1)
		return;

	strcpy_s(dbfname, DBFNAME_LEN, m_filename);
	strcat_s(dbfname, DBFNAME_LEN, ".dbf");
	rslt = dbf.OpenDbf(dbfname, OF_READ);
	if (rslt != DB_NO_ERROR)
	{
		return;
	}

	rslt = dbf.GetFirstFieldName(fname, FNAME_LEN, &len, &dec);
	if (rslt != DB_NO_ERROR)
		return;

	// add a choice of "none"
	txt = new CString;
	*txt = "<none>";
	m_list.AddTail(txt);
	while (rslt == DB_NO_ERROR)
	{
		txt = new CString;
		*txt = fname;
		*txt += "          ";
		*txt = txt->Left(10);
		*txt += ": \t";
		temp = fname;
		data = get_field_data(dbf, temp);
//		*txt += data.Left(80);
		*txt += data;
		m_list.AddTail(txt);
		rslt = dbf.GetNextFieldName(fname, FNAME_LEN, &len, &dec);
	}
	dbf.CloseDbf();
}
// end of fill_field_list

// *****************************************************************
// *****************************************************************

CString CShpFieldSelectDlg::get_field_data(CDbase & dbf, CString fieldname) 
{
   const int FNAME_LEN = 21;
	char fname[FNAME_LEN];
	char data[256];
	int numrec, k, rslt, cnt, len;
	CList<CString* , CString*> list;
	CString *txt;
	CString temp;
	CString sdata("");
	POSITION next;
	BOOL found;

	numrec = dbf.GetNumberOfRecords();
	if (numrec < 1)
		return sdata;

	strcpy_s(fname, FNAME_LEN, fieldname);

	cnt = 0;
	len = 0;

	for (k=1; k<=numrec; k++)
	{
		rslt = dbf.GotoRec(k);
		if (rslt != DB_NO_ERROR)
		{
			k = numrec;
			continue;
		}

		rslt = dbf.GetFieldData( fname, data);
		temp = data;
		temp.TrimRight();
		temp.TrimLeft();

		// check to see if the item is already in the list
		found = FALSE;
		next = list.GetHeadPosition();
		while (next != NULL)
		{
			txt = list.GetNext(next);
			if (!txt->Compare(temp))
			{
				found = TRUE;
				next = NULL;
				continue;
			}
		}
		if (!found)
		{
			txt = new CString;
			*txt = temp;
			len += temp.GetLength();
			list.AddTail(txt);
			cnt++;
//			if (len > 300)
			if (cnt > 100)
			{
				txt = new CString;
				*txt = "...";
				list.AddTail(txt);
				k = numrec;
			}
		}
	}

	sdata = "";
	while (!list.IsEmpty())
	{
		txt = list.RemoveHead();
		if ((txt->GetLength() + sdata.GetLength()) > 300)
			sdata += "...";
		else
		{
			if (sdata.GetLength() > 0)
				sdata += ", ";
			sdata += *txt;
			sdata.TrimRight();
		}
		delete txt;
	}
	return sdata;
}
// end of get_field_data

// ****************************************************************
// ****************************************************************

void CShpFieldSelectDlg::update_field_list()
{	 
	int cnt, len, maxlen;
	POSITION next;
	CString *txt;
	CString tstr;

	cnt = m_list.GetCount();
	if (cnt < 1)
		return;

	// clear the list
	SendDlgItemMessage(IDC_FIELD_LIST, LB_RESETCONTENT, 0, 0);

	maxlen = 0;
	next = m_list.GetHeadPosition();
	while (next != NULL)
	{
		txt = m_list.GetNext(next);
		tstr = *txt;
		len = tstr.GetLength();
		len *= 6;
		if (len > maxlen)
			maxlen = len;
 
		SendDlgItemMessage(IDC_FIELD_LIST, LB_SETHORIZONTALEXTENT, (WPARAM) maxlen, (LPARAM) 0);
		SendDlgItemMessage(IDC_FIELD_LIST, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) tstr);
	}		
	
}

// *****************************************************************
// ****************************************************************

void CShpFieldSelectDlg::AdjustControls(int cx, int cy)
{
   //get the richedit control
   HWND twnd = GetDlgItem(IDC_FIELD_LIST)->GetSafeHwnd();

   //trivial rejection
   if (twnd == NULL)
      return;
	
   int bdx=cx/2;     //button width
   int bdy=20;       //button height

   int bx = 0;       //x pos of button upperleft corner
   int by = cy-bdy;  //y pos of button upperleft corner
   
   //place the fieldlist box
   ::MoveWindow(twnd, 5, 20, cx-10, cy-50, TRUE);

   //place the print button
   twnd = GetDlgItem(IDOK)->GetSafeHwnd();
	::MoveWindow(twnd,  bdx-30, cy-25, 60, 22,	TRUE);

   //move over one button width
   bx+=bdx;

   //place the cancel button
   twnd = GetDlgItem(IDCANCEL)->GetSafeHwnd();
	::MoveWindow(twnd,  cx - 70, cy-25, 60, 22,	TRUE);

	update_field_list();
}

// *****************************************************************
// *****************************************************************

void CShpFieldSelectDlg::OnSize(UINT nType, int cx, int cy) 
{
	if (cx < 200)
		cx = 200;
	if (cy < 100)
		cy = 100;

	if (m_active)
	{
//		cx = m_cx;
//		cy = m_cy;
	}

	AdjustControls(cx, cy);
	
	m_cx = cx;
	m_cy = cy;
	
	CDialog::OnSize(nType, cx, cy);
}

// *****************************************************************
// *****************************************************************
LRESULT CShpFieldSelectDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

