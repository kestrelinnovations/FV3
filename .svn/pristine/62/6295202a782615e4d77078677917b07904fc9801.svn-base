// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// infodlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "infodlg.h"
#include "err.h"
#include "fvwutil.h"
#include "param.h"
#include <math.h>
#include "..\getobjpr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static boolean_t db_lookup(CString &string, SnapToInfo &db_info, 
   boolean_t fix_point_search);


/////////////////////////////////////////////////////////////////////////////
// CUserArEditInfoDlg dialog


CUserArEditInfoDlg::CUserArEditInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserArEditInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserArEditInfoDlg)
	m_apn = _T("");
	m_apx = _T("");
	m_atc = _T("");
	m_remarks = _T("");
	m_sched_unit = _T("");
	m_name = _T("");
	m_fuel1alt1 = _T("");
	m_fuel1alt2 = _T("");
	m_fuel2alt1 = _T("");
	m_fuel2alt2 = _T("");
	m_fuel3alt1 = _T("");
	m_fuel3alt2 = _T("");
	m_course_str = _T("");
	m_length_str = _T("");
	m_width_str = _T("");
	m_freq_bck = 0.0;
	m_freq_pri = 0.0;
	m_tac_chan = _T("");
	m_length_units = _T("");
	m_width_units = _T("");
	m_tanker_chan = _T("");
	//}}AFX_DATA_INIT

	m_country = "US";
	m_rte = NULL;
	m_aborted = FALSE;
}


void CUserArEditInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserArEditInfoDlg)
	DDX_Text(pDX, IDC_APN, m_apn);
	DDV_MaxChars(pDX, m_apn, 5);
	DDX_Text(pDX, IDC_APX, m_apx);
	DDV_MaxChars(pDX, m_apx, 3);
	DDX_Text(pDX, IDC_ATC, m_atc);
	DDV_MaxChars(pDX, m_atc, 80);
	DDX_Text(pDX, IDC_REMARKS, m_remarks);
	DDX_Text(pDX, IDC_SCHED_UNIT, m_sched_unit);
	DDV_MaxChars(pDX, m_sched_unit, 80);
	DDX_Text(pDX, IDC_Name, m_name);
	DDX_Text(pDX, IDC_ALT1_1, m_fuel1alt1);
	DDV_MaxChars(pDX, m_fuel1alt1, 5);
	DDX_Text(pDX, IDC_ALT1_2, m_fuel1alt2);
	DDV_MaxChars(pDX, m_fuel1alt2, 5);
	DDX_Text(pDX, IDC_ALT2_1, m_fuel2alt1);
	DDV_MaxChars(pDX, m_fuel2alt1, 5);
	DDX_Text(pDX, IDC_ALT2_2, m_fuel2alt2);
	DDV_MaxChars(pDX, m_fuel2alt2, 5);
	DDX_Text(pDX, IDC_ALT3_1, m_fuel3alt1);
	DDV_MaxChars(pDX, m_fuel3alt1, 5);
	DDX_Text(pDX, IDC_ALT3_2, m_fuel3alt2);
	DDV_MaxChars(pDX, m_fuel3alt2, 5);
	DDX_Text(pDX, IDC_COURSE, m_course_str);
	DDX_Text(pDX, IDC_LENGTH, m_length_str);
	DDX_Text(pDX, IDC_WIDTH, m_width_str);
	DDX_Text(pDX, IDC_FREQ_BCK, m_freq_bck);
	DDX_Text(pDX, IDC_FREQ_PRI, m_freq_pri);
	DDX_Text(pDX, IDC_TAC_CHAN, m_tac_chan);
	DDV_MaxChars(pDX, m_tac_chan, 6);
	DDX_Text(pDX, IDC_LENGTH_UNITS, m_length_units);
	DDX_Text(pDX, IDC_WIDTH_UNITS, m_width_units);
	DDX_Text(pDX, IDC_TANKER_CHAN, m_tanker_chan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserArEditInfoDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CUserArEditInfoDlg)
	ON_EN_KILLFOCUS(IDC_LENGTH, OnKillfocusLength)
	ON_EN_KILLFOCUS(IDC_COURSE, OnKillfocusCourse)
	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_TAC_CHAN, OnKillfocusTacChan)
	ON_EN_KILLFOCUS(IDC_APN, OnKillfocusApn)
	ON_EN_KILLFOCUS(IDC_APX, OnKillfocusApx)
	ON_EN_KILLFOCUS(IDC_FREQ_BCK, OnKillfocusFreqBck)
	ON_EN_KILLFOCUS(IDC_FREQ_PRI, OnKillfocusFreqPri)
	ON_EN_KILLFOCUS(IDC_ALT1_1, OnKillfocusAlt11)
	ON_EN_KILLFOCUS(IDC_ALT1_2, OnKillfocusAlt12)
	ON_EN_KILLFOCUS(IDC_ALT2_1, OnKillfocusAlt21)
	ON_EN_KILLFOCUS(IDC_ALT2_2, OnKillfocusAlt22)
	ON_EN_KILLFOCUS(IDC_ALT3_1, OnKillfocusAlt31)
	ON_EN_KILLFOCUS(IDC_ALT3_2, OnKillfocusAlt32)
	ON_EN_CHANGE(IDC_TAC_CHAN, OnChangeTacChan)
	ON_CBN_SELCHANGE(IDC_UNITS, OnSelchangeUnits)
	ON_CBN_CLOSEUP(IDC_COUNTRY, OnCloseupCountry)
	ON_CBN_KILLFOCUS(IDC_COUNTRY, OnKillfocusCountry)
	ON_BN_CLICKED(IDC_ALT1_AA, OnAlt1_AA)
	ON_BN_CLICKED(IDC_ALT2_AA, OnAlt2_AA)
	ON_BN_CLICKED(IDC_ALT3_AA, OnAlt3_AA)
	ON_BN_CLICKED(IDC_ALT1_AB, OnAlt1_AB)
	ON_BN_CLICKED(IDC_ALT2_AB, OnAlt2_AB)
	ON_BN_CLICKED(IDC_ALT3_AB, OnAlt3_AB)
	ON_BN_CLICKED(IDC_ALT1_AT, OnAlt1_AT)
	ON_BN_CLICKED(IDC_ALT2_AT, OnAlt2_AT)
	ON_BN_CLICKED(IDC_ALT3_AT, OnAlt3_AT)
	ON_BN_CLICKED(IDC_ALT1_BT, OnAlt1_BT)
	ON_BN_CLICKED(IDC_ALT2_BT, OnAlt2_BT)
	ON_BN_CLICKED(IDC_ALT3_BT, OnAlt3_BT)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserArEditInfoDlg message handlers

BOOL CUserArEditInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int num;

	CString temp = PRM_get_registry_string("ArEdit", "Units", "NM");

	int pos = SendDlgItemMessage(IDC_UNITS, CB_FINDSTRINGEXACT, 0, 
											(LPARAM) (LPCTSTR) temp.GetBuffer(50));
	if (pos == CB_ERR)
		pos = 0;
	SendDlgItemMessage(IDC_UNITS, CB_SETCURSEL, pos, 0);
	OnSelchangeUnits();

	if (m_rte != NULL)
	{
		m_length_nm = m_rte->m_anchor_length;
		m_width_nm = m_rte->m_anchor_width;
		update_dlg();
		m_course_str.Format("%.2f", m_rte->m_angle);
		m_name = m_rte->m_id;
		m_old_name = m_name;
		m_country = m_rte->m_country;
		m_freq_pri = atof(m_rte->m_prfreq);
		m_freq_bck = atof(m_rte->m_bufreq);
		m_apn = m_rte->m_apnset;
		m_apx = m_rte->m_apxcode;
		m_tac_chan = m_rte->m_tac_chan.Left(2);
		num = atoi(m_tac_chan);
		if (num > 0)
			m_tanker_chan.Format("/ %d", num+63);
		m_sched_unit = m_rte->m_unit;
		m_atc = m_rte->m_atc;
		m_fuel1alt1 = m_rte->m_fuel1alt1;
		m_fuel1alt2 = m_rte->m_fuel1alt2;
		m_fuel2alt1 = m_rte->m_fuel2alt1;
		m_fuel2alt2 = m_rte->m_fuel2alt2;
		m_fuel3alt1 = m_rte->m_fuel3alt1;
		m_fuel3alt2 = m_rte->m_fuel3alt2;
		m_altdesc1 = m_rte->m_altdesc1;
		m_altdesc2 = m_rte->m_altdesc2;
		m_altdesc3 = m_rte->m_altdesc3;
		m_remarks = m_rte->m_remarks;
		if (m_rte->m_type != 'A')
		{
			show_orbit_params(FALSE);
			SetWindowText("Edit Track Info");
		}
		else
			SetWindowText("Edit Orbit Info");

	}

	enable_window(IDC_ALT1_2, FALSE);
	enable_window(IDC_ALT2_2, FALSE);
	enable_window(IDC_ALT3_2, FALSE);

	if (!m_altdesc1.Compare("AA"))
		CheckRadioButton(IDC_ALT1_AA, IDC_ALT1_BT, IDC_ALT1_AA);
	else if (!m_altdesc1.Compare("AB"))
		CheckRadioButton(IDC_ALT1_AA, IDC_ALT1_BT, IDC_ALT1_AB);
	else if (!m_altdesc1.Compare("AT"))
		CheckRadioButton(IDC_ALT1_AA, IDC_ALT1_BT, IDC_ALT1_AT);
	else
	{
		CheckRadioButton(IDC_ALT1_AA, IDC_ALT1_BT, IDC_ALT1_BT);
		enable_window(IDC_ALT1_2, TRUE);
	}

	if (!m_altdesc2.Compare("AA"))
		CheckRadioButton(IDC_ALT2_AA, IDC_ALT2_BT, IDC_ALT2_AA);
	else if (!m_altdesc2.Compare("AB"))
		CheckRadioButton(IDC_ALT2_AA, IDC_ALT2_BT, IDC_ALT2_AB);
	else if (!m_altdesc2.Compare("AT"))
		CheckRadioButton(IDC_ALT2_AA, IDC_ALT2_BT, IDC_ALT2_AT);
	else
	{
		CheckRadioButton(IDC_ALT2_AA, IDC_ALT2_BT, IDC_ALT2_BT);
		enable_window(IDC_ALT2_2, TRUE);
	}

	if (!m_altdesc3.Compare("AA"))
		CheckRadioButton(IDC_ALT3_AA, IDC_ALT3_BT, IDC_ALT3_AA);
	else if (!m_altdesc3.Compare("AB"))
		CheckRadioButton(IDC_ALT3_AA, IDC_ALT3_BT, IDC_ALT3_AB);
	else if (!m_altdesc3.Compare("AT"))
		CheckRadioButton(IDC_ALT3_AA, IDC_ALT3_BT, IDC_ALT3_AT);
	else
	{
		CheckRadioButton(IDC_ALT3_AA, IDC_ALT3_BT, IDC_ALT3_BT);
		enable_window(IDC_ALT3_2, TRUE);
	}

	fill_country_list();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// *****************************************************************
// *****************************************************************

void CUserArEditInfoDlg::OnCancel() 
{
	m_aborted = TRUE;
	
	CDialog::OnCancel();
}

// *****************************************************************
// *****************************************************************

void CUserArEditInfoDlg::OnOK() 
{
	double tf;
	int num;
	CString tstr;

	UpdateData(TRUE);
	

	m_rte->m_id = m_name;
	m_rte->m_label = m_name;
	m_rte->m_prfreq.Format("%.3f", m_freq_pri);
	m_rte->m_bufreq.Format("%.3f", m_freq_bck);
	m_rte->m_apnset = m_apn;
	m_rte->m_apxcode = m_apx;
	num = atoi(m_tac_chan);
	tstr.Format("%02d", num);
	m_rte->m_tac_chan = tstr;
	tstr.Format("%03d", num+63);
	m_rte->m_tac_chan += tstr;
	m_rte->m_unit = m_sched_unit;
	m_rte->m_atc = m_atc;
	m_rte->m_fuel1alt1 = m_fuel1alt1;
	m_rte->m_fuel1alt2 = m_fuel1alt2;
	m_rte->m_fuel2alt1 = m_fuel2alt1;
	m_rte->m_fuel2alt2 = m_fuel2alt2;
	m_rte->m_fuel3alt1 = m_fuel3alt1;
	m_rte->m_fuel3alt2 = m_fuel3alt2;
	m_rte->m_altdesc1 = m_altdesc1;
	m_rte->m_altdesc2 = m_altdesc2;
	m_rte->m_altdesc3 = m_altdesc3;
	m_rte->m_remarks = m_remarks;
	tf = atof(m_length_str);
	m_rte->m_anchor_length = convert_to_NM(tf);
	tf = atof(m_width_str);
	m_rte->m_anchor_width = convert_to_NM(tf);
	m_rte->m_angle = atof(m_course_str);
	m_rte->m_country = m_country;

	if (m_name.Compare(m_old_name))
	{
		// change the names of the points
		POSITION next;
		C_ar_point *pt;

		next = m_rte->m_point_list.GetHeadPosition();
		while (next != NULL)
		{
			pt = m_rte->m_point_list.GetNext(next);
			pt->m_id = m_name;
		}
	}

	CDialog::OnOK();
}
// end of OnOK

// *****************************************************************
// *****************************************************************

void CUserArEditInfoDlg::set_route_ptr(C_ar_obj *rte) 
{
	m_rte = rte;
}

// *****************************************************************
// *****************************************************************

void CUserArEditInfoDlg::show_orbit_params(BOOL show) 
{
	int showit;

	if (show)
		showit = SW_SHOW;
	else
		showit = SW_HIDE;

	show_window(IDC_ORBIT_FRAME, showit);
	show_window(IDC_UNITS, showit);
	show_window(IDC_UNITS_LABEL, showit);
	show_window(IDC_LENGTH, showit);
	show_window(IDC_LENGTH_LABEL, showit);
	show_window(IDC_LENGTH_UNITS, showit);
	show_window(IDC_WIDTH, showit);
	show_window(IDC_WIDTH_LABEL, showit);
	show_window(IDC_WIDTH_UNITS, showit);
	show_window(IDC_COURSE, showit);
	show_window(IDC_COURSE_LABEL, showit);
	show_window(IDC_COURSE_UNITS, showit);
}

// *****************************************************************
// ************************************************************************

void CUserArEditInfoDlg::show_window(int id, BOOL show) 
{
	CWnd *wnd;

	wnd = GetDlgItem(id);

	if (wnd != NULL)
	{
		wnd->ShowWindow(show);
	}
	else
	{
		ERR_report("GetDlgItem failed in User AR Info Dialog");
		ASSERT(0);
	}
}
// end of show_window

// ************************************************************************
// ************************************************************************

void CUserArEditInfoDlg::enable_window(int id, BOOL enable) 
{
	CWnd *wnd;

	wnd = GetDlgItem(id);

	if (wnd != NULL)
	{
		wnd->EnableWindow(enable);
	}
	else
	{
		ERR_report("GetDlgItem failed in User AR Info Dialog");
		ASSERT(0);
	}
}
// end of enable_window

// ************************************************************************
// *************************************************************

void CUserArEditInfoDlg::fill_country_list() 
{
	BOOL found;
   const int CTY_LEN = 121;
   const int CODE_LEN = 4;
	char cty[CTY_LEN], code[CODE_LEN];
	int rslt;
	CString value, tstr;
	CFvwUtil *futil = CFvwUtil::get_instance();

	found = futil->get_first_country(cty, CTY_LEN, code, CODE_LEN);
//	rslt = SendDlgItemMessage(IDC_COUNTRY, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) cty);
	while (found)
	{
		tstr = code;
		tstr += "  ";
		tstr += cty;
		rslt = SendDlgItemMessage(IDC_COUNTRY, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tstr);
		found = futil->get_next_country(cty, CTY_LEN, code, CODE_LEN);
	}

	// set the default
	value = PRM_get_registry_string("ArEdit", "RefuelCountry", m_country);
	strcpy_s(code, CODE_LEN, value);
	if (futil->find_country(code, cty))
//	if (!futil->find_country(code, cty))
//		ERR_report("Cound not find store code in country database");
//	else
	{
		tstr = code;
		tstr += "  ";
		tstr += cty;
		rslt = SendDlgItemMessage(IDC_COUNTRY, CB_FINDSTRINGEXACT, -1, (LPARAM) (LPCTSTR) tstr);
		if (rslt != CB_ERR)
			SendDlgItemMessage(IDC_COUNTRY, CB_SETCURSEL, rslt, (LPARAM) (LPCTSTR) 0);
	}
}

// *************************************************************
// *****************************************************************
// *****************************************************************
// *****************************************************************
// *****************************************************************
// *****************************************************************
// *****************************************************************
// *****************************************************************


// *************************************************************
// *************************************************************
// *************************************************************
// *************************************************************


void CUserArEditInfoDlg::OnKillfocusLength() 
{
	double tf, tf2;

	if (m_aborted)
		return;

	UpdateData(TRUE);

	tf = atof(m_length_str);
	tf2 = atof(m_width_str);

	if (tf < 0.0)
	{
		tf = -tf;
		m_length_str.Format("%6.1f", tf);
	}
	if (tf >1000.0)
	{
		tf = 1000.0;
		m_length_str.Format("%6.1f", tf);
	}

	if (tf <= tf2)
	{
		tf = tf2 / 0.9;
		m_length_str.Format("%6.1f", tf);
	}

	UpdateData(FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusCourse() 
{
	CString tmp, msg;
	double tf;

	if (m_aborted)
		return;

	tmp = m_course_str;

	UpdateData(TRUE);
	tf = atof(m_course_str);
	if ((tf < 0.0) || (tf > 360.0))
	{
		msg = "The course must be bewteen 0.0 and 360.0 degrees";
		AfxMessageBox(msg);
		m_course_str = "";
		m_rte->m_angle = 0.0;
		UpdateData(FALSE);
	}

	// TODO: Add your control notification handler code here
	
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusWidth() 
{
	double tf, tf2;
	CString old_width_str;

	if (m_aborted)
		return;

	old_width_str = m_width_str;
	UpdateData(TRUE);

	tf = atof(m_width_str);

	if (tf <= 0.0)
	{
		AfxMessageBox("Invalid Width");
		m_width_str = "20.0";
		m_rte->m_anchor_width = 20.0;
		return;
	}

	tf2 = atof(m_length_str);

	if (tf < 0.0)
	{
		tf = -tf;
		m_width_str.Format("%6.1f", tf);
	}

	if (tf > 500.0)
	{
		tf = 500.0;
		m_width_str.Format("%6.1f", tf);
	}

	if (tf >= tf2)
	{
		tf = tf2 * 0.9;
		m_width_str.Format("%6.1f", tf);
	}

	UpdateData(FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusTacChan() 
{
	CString msg, oldchan, tstr;
	int len, num1, num2;

	if (m_aborted)
		return;

	oldchan = m_tac_chan;
	UpdateData(TRUE);

	// check the data format
	len = m_tac_chan.GetLength();
	if (len == 0)
		return;

	tstr = m_tac_chan;
	num1 = atoi(tstr);
	if ((num1 < 1) || (num1 > 126))
		goto FAIL;

	if (num1 < 64)
		num2 = num1 + 63;
	else
		num2 = num1 - 63;

	m_tanker_chan.Format("/ %d", num2);

	UpdateData(FALSE);

	return;

FAIL:
	msg = "The A/A Tacan channel values entered are in the wrong format.\n";
	msg += "The Receiver channel must be between 1 and 126.";
//	msg += "It must be in the format A/B, A-B, or A.B where A = 01-63\n";
//	msg += "and B = 64-126.  The serparation between them (B-A) must be = 63";
	AfxMessageBox(msg);
	m_tac_chan = "";
	m_rte->m_tac_chan = "";
	UpdateData(FALSE);
	GetDlgItem(IDC_TAC_CHAN)->SetFocus();
	SendDlgItemMessage(IDC_TAC_CHAN, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusApn() 
{
	CString msg, oldapn;
	int len;
	char ch;

	if (m_aborted)
		return;

	oldapn = m_apn;
	UpdateData(TRUE);

	// check the data format
	len = m_apn.GetLength();
	if (len == 0)
		return;

	if ((len != 3) && (len != 5))
		goto FAIL;

	if (len == 3)
	{
		ch = m_apn[0];
		if ((ch != '1') && (ch != '2') && (ch != '3'))
			goto FAIL;
		ch = m_apn[1];
		if ((ch != '1') && (ch != '2') && (ch != '3') && (ch != '4') && (ch != '0'))
			goto FAIL;
		ch = m_apn[2];
		if ((ch != '1') && (ch != '2') && (ch != '3') && (ch != '4') && (ch != '0'))
			goto FAIL;
	}
	else
		goto FAIL;
/*
	if (len == 5)
	{
		ch = m_apn[0];
		if ((ch != '1') && (ch != '2') && (ch != '3'))
			goto FAIL;
		ch = m_apn[2];
		if ((ch != '1') && (ch != '2') && (ch != '3') && (ch != '4') && (ch != '0'))
			goto FAIL;
		ch = m_apn[4];
		if ((ch != '1') && (ch != '2') && (ch != '3') && (ch != '4') && (ch != '0'))
			goto FAIL;
		if (m_apn[1] != m_apn[3])
			goto FAIL;
		ch = m_apn[1];
		if ((ch != '/') && (ch != '-') && (ch != '.'))
			goto FAIL;
	}
*/
	return;

FAIL:
	msg = "The APN value entered is out of range or in the wrong format.\n";
//	msg += "It must be in the format ABC, A/B/C, A-B-C, or A.B.C where\n";
	msg += "It must be in the format ABC where\n";
	msg += "A = 1, 2, or 3   B = 0, 1, 2, 3, or 4 and C = 0,  1, 2, 3, or 4";
	AfxMessageBox(msg);
	m_apn = "";
	m_rte->m_apnset = m_apn;
	UpdateData(FALSE);
	GetDlgItem(IDC_APN)->SetFocus();
	SendDlgItemMessage(IDC_APN, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusApx() 
{
	CString msg, oldapx;
	int len;
	char ch;

	if (m_aborted)
		return;

	oldapx = m_apx;
	UpdateData(TRUE);

	// check the data format
	len = m_apx.GetLength();
	if (len == 0)
		return;

	if ((len != 2) && (len != 3))
		goto FAIL;

	if (len == 2)
	{
		ch = m_apx[1];
		if (ch != '1')
			goto FAIL;
		ch = m_apx[0];
		if ((ch != '2') && (ch != '3') && (ch != '4') && (ch != '5') && (ch != '6'))
			goto FAIL;
	}

	if (len == 3)
	{
		ch = m_apx[2];
		if (ch != '1')
			goto FAIL;
		ch = m_apx[0];
		if ((ch != '2') && (ch != '3') && (ch != '4') && (ch != '5') && (ch != '6'))
			goto FAIL;
		ch = m_apx[1];
		if ((ch != '/') && (ch != '-') && (ch != '.'))
			goto FAIL;
	}

	return;

FAIL:
	msg = "The APX value enterd is in the wrong format or is incomplete.\n";
	msg += "It must be in the format A1, A/1, or A.1 where A = 2, 3, 4, 5, or 6";
	AfxMessageBox(msg);
	m_apx = "";
	m_rte->m_apxcode = m_apx;
	UpdateData(FALSE);
	GetDlgItem(IDC_APX)->SetFocus();
	SendDlgItemMessage(IDC_APX, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusFreqBck() 
{
	CString msg;
	double oldfreq;
	BOOL bad_freq = TRUE;

	if (m_aborted)
		return;

	oldfreq = m_freq_bck;

	UpdateData(TRUE);

	if ((m_freq_bck >= 30.0) && (m_freq_bck <= 88.0))
		bad_freq = FALSE;
	if ((m_freq_bck >= 108.0) && (m_freq_bck <= 156.0))
		bad_freq = FALSE;
	if ((m_freq_bck >= 225.0) && (m_freq_bck <= 399.975))
		bad_freq = FALSE;

	if ((m_freq_bck != 0.0) && bad_freq)
	{
		msg = "The Backup Frequency is out of range...\n\n";
		msg += "Input must be in one of the following ranges:\n\n";
		msg += "  FM  in the range of 30.000 to 88.0\n";
		msg += "  VHF in the range of 108.000 to 156.0\n";
		msg += "  UHF in the range of 225.000 to 399.975\n\n";
		msg += "                     OR\n\n";
		msg += "               0  for  NONE";
		AfxMessageBox(msg);
		m_freq_bck = 0.0;
		m_rte->m_bufreq = "";
		UpdateData(FALSE);
		GetDlgItem(IDC_FREQ_BCK)->SetFocus();
		SendDlgItemMessage(IDC_FREQ_BCK, EM_SETSEL, 0, MAKELONG(0, -1));
	}
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusFreqPri() 
{
	CString msg;
	BOOL bad_freq = TRUE;
	double oldfreq;

	if (m_aborted)
		return;

	oldfreq = m_freq_pri;

	UpdateData(TRUE);

	if ((m_freq_pri >= 30.0) && (m_freq_pri <= 88.0))
		bad_freq = FALSE;
	if ((m_freq_pri >= 108.0) && (m_freq_pri <= 156.0))
		bad_freq = FALSE;
	if ((m_freq_pri >= 225.0) && (m_freq_pri <= 399.975))
		bad_freq = FALSE;

	if ((m_freq_pri != 0.0) && bad_freq)
	{
		msg = "The Primary Frequency is out of range...\n\n";
		msg += "Input must be in one of the following ranges:\n\n";
		msg += "  FM  in the range of 30.000 to 88.0\n";
		msg += "  VHF in the range of 108.000 to 156.0\n";
		msg += "  UHF in the range of 225.000 to 399.975\n\n";
		msg += "                     OR\n\n";
		msg += "               0  for  NONE";
		AfxMessageBox(msg);
		m_freq_pri = 0.0;
		m_rte->m_prfreq = "";
		UpdateData(FALSE);
		GetDlgItem(IDC_FREQ_PRI)->SetFocus();
		SendDlgItemMessage(IDC_FREQ_PRI, EM_SETSEL, 0, MAKELONG(0, -1));
	}
}

// *************************************************************
// *************************************************************

BOOL CUserArEditInfoDlg::altitude_ok(int alt) 
{
	if ((alt >= 0) && (alt < 100000))
		return TRUE;

	CString msg;

	msg = "The altitude is out of range.\n\n";
	msg += "Altitude must be positive and less then 100000";
	AfxMessageBox(msg);

	return FALSE;
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusAlt11() 
{
	int alt;
	CString oldalt;

	if (m_aborted)
		return;

	oldalt = m_fuel1alt1;

	UpdateData(TRUE);
	if (m_fuel1alt1.GetLength() < 1)
		return;

	alt = atoi(m_fuel1alt1);
	if (altitude_ok(alt))
		return;

	m_fuel1alt1 = "";
	m_rte->m_fuel1alt1 = m_fuel1alt1;

	UpdateData(FALSE);
	GetDlgItem(IDC_ALT1_1)->SetFocus();
	SendDlgItemMessage(IDC_ALT1_1, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusAlt12() 
{
	int alt, alt1;
	CString oldalt;
	BOOL ok = TRUE;

	if (m_aborted)
		return;

	oldalt = m_fuel1alt2;

	UpdateData(TRUE);

	if (m_fuel1alt2.GetLength() < 1)
		return;

	alt = atoi(m_fuel1alt2);
	alt1 = atoi(m_fuel1alt1);

	if (!altitude_ok(alt))
		ok = FALSE;

	if (alt1 > alt)
	{
		AfxMessageBox("The 'higher' altitude cannot be less then the 'lower'");
		ok = FALSE;
	}

	if (ok)
		return;

	m_fuel1alt2 = "";
	m_rte->m_fuel1alt2 = m_fuel1alt2;

	UpdateData(FALSE);
	GetDlgItem(IDC_ALT1_2)->SetFocus();
	SendDlgItemMessage(IDC_ALT1_2, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusAlt21() 
{
	int alt;
	CString oldalt;

	if (m_aborted)
		return;

	oldalt = m_fuel2alt1;

	UpdateData(TRUE);
	if (m_fuel2alt1.GetLength() < 1)
		return;

	alt = atoi(m_fuel2alt1);
	if (altitude_ok(alt))
		return;

	m_fuel2alt1 = "";
	m_rte->m_fuel2alt1 = m_fuel2alt1;

	UpdateData(FALSE);
	GetDlgItem(IDC_ALT2_1)->SetFocus();
	SendDlgItemMessage(IDC_ALT2_1, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusAlt22() 
{
	int alt, alt1;
	CString oldalt;
	BOOL ok = TRUE;

	if (m_aborted)
		return;

	oldalt = m_fuel2alt2;

	UpdateData(TRUE);
	if (m_fuel2alt2.GetLength() < 1)
		return;
	alt = atoi(m_fuel2alt2);
	alt1 = atoi(m_fuel2alt1);
	if (!altitude_ok(alt))
		ok = FALSE;

	if (alt1 > alt)
	{
		AfxMessageBox("The 'higher' altitude cannot be less then the 'lower'");
		ok = FALSE;
	}

	if (ok)
		return;

	m_fuel2alt2 = "";
	m_rte->m_fuel2alt2 = m_fuel2alt2;

	UpdateData(FALSE);
	GetDlgItem(IDC_ALT2_2)->SetFocus();
	SendDlgItemMessage(IDC_ALT2_2, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusAlt31() 
{
	int alt;
	CString oldalt;

	if (m_aborted)
		return;

	oldalt = m_fuel3alt1;

	UpdateData(TRUE);
	if (m_fuel3alt1.GetLength() < 1)
		return;

	alt = atoi(m_fuel3alt1);
	if (altitude_ok(alt))
		return;

	m_fuel3alt1 = "";
	m_rte->m_fuel3alt1 = m_fuel3alt1;

	UpdateData(FALSE);
	GetDlgItem(IDC_ALT3_1)->SetFocus();
	SendDlgItemMessage(IDC_ALT3_1, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusAlt32() 
{
	int alt, alt1;
	CString oldalt;
	BOOL ok = TRUE;

	if (m_aborted)
		return;

	oldalt = m_fuel3alt2;

	UpdateData(TRUE);
	if (m_fuel3alt2.GetLength() < 1)
		return;

	alt = atoi(m_fuel3alt2);
	alt1 = atoi(m_fuel3alt1);
	if (!altitude_ok(alt))
		ok = FALSE;

	if (alt1 > alt)
	{
		AfxMessageBox("The 'higher' altitude cannot be less then the 'lower'");
		ok = FALSE;
	}
	if (ok)
		return;

	m_fuel3alt2 = "";
	m_rte->m_fuel3alt2 = m_fuel3alt2;

	UpdateData(FALSE);
	GetDlgItem(IDC_ALT3_2)->SetFocus();
	SendDlgItemMessage(IDC_ALT3_2, EM_SETSEL, 0, MAKELONG(0, -1));
}

// *************************************************************
// *************************************************************


void CUserArEditInfoDlg::OnChangeTacChan() 
{
	CString tstr;
	int num;
	BOOL sepchar = FALSE;

	UpdateData(TRUE);
	
	if (m_tac_chan.GetLength() == 3)
	{
		tstr = m_tac_chan.Right(1);
		if (!tstr.Compare("/") || !tstr.Compare("-") || !tstr.Compare("."))
			sepchar = TRUE;
		if (sepchar && (m_last_text.GetLength() == 2))
		{
			tstr = m_tac_chan.Left(2);
			num = atoi(tstr);
			if ((num > 0) && (num < 64))
			{
				num += 63;
				tstr.Format("%d", num);
				m_tac_chan += tstr;
				UpdateData(FALSE);
//				SendDlgItemMessage(IDC_TAC_CHAN, EM_SETSEL, 0, MAKELONG(5, 5));
				SendDlgItemMessage(IDC_TAC_CHAN, EM_SETSEL, 0xffff, 0xffff);
			}
		}
	}

	m_last_text = m_tac_chan;
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::update_dlg() 
{
	double tf;

	tf = convert_from_NM(m_width_nm);
	m_width_str.Format("%.1f", tf);
	tf = convert_from_NM(m_length_nm);
	m_length_str.Format("%.1f", tf);

	m_width_units = get_units_label();
	m_length_units = get_units_label();

	UpdateData(FALSE);
}
// end of update_dlg

// *************************************************************
// *************************************************************

double CUserArEditInfoDlg::convert_to_NM(double num) 
{
	double nm;

	// default
	nm = num;

	switch(m_units)
	{
		case UAR_UNITS_NM:
			nm = num;
			break;
		case UAR_UNITS_KM:
			nm = num * 0.5399568;
			break;
		case UAR_UNITS_METERS:
			nm = num * 0.0005399568;
			break;
		case UAR_UNITS_YARDS:
			nm = num * 0.0004937364980724659;
			break;
		case UAR_UNITS_FEET:
			nm = num * 0.00016457883238984018;
			break;
	}
	return nm;
}
	
// *************************************************************
// *************************************************************

double CUserArEditInfoDlg::convert_from_NM(double nm) 
{
	double num;

	// default
	num = nm;

	switch(m_units)
	{
		case UAR_UNITS_NM:
			num = nm;
			break;
		case UAR_UNITS_KM:
			num = nm / 0.5399568;
			break;
		case UAR_UNITS_METERS:
			num = nm / 0.0005399568;
			break;
		case UAR_UNITS_YARDS:
			num = nm / 0.0004937364980724659;
			break;
		case UAR_UNITS_FEET:
			num = nm / 0.00016457883238984018;
			break;
	}
	return num;
}
	
// *************************************************************
// *************************************************************

CString CUserArEditInfoDlg::get_units_label() 
{
	CString label;

	// default
	label = "";

	switch(m_units)
	{
		case UAR_UNITS_NM:
			label = "NM";
			break;
		case UAR_UNITS_KM:
			label = "km";
			break;
		case UAR_UNITS_METERS:
			label = "m";
			break;
		case UAR_UNITS_YARDS:
			label = "yd";
			break;
		case UAR_UNITS_FEET:
			label = "ft";
			break;
	}
	return label;
}
	
// *************************************************************
/// *************************************************************

void CUserArEditInfoDlg::OnSelchangeUnits() 
{
	int pos, rslt;
	char unit_str[21];
	CString temp;

	pos = SendDlgItemMessage(IDC_UNITS, CB_GETCURSEL, 0, 0);
	if (pos == CB_ERR)
		return;

	rslt = SendDlgItemMessage(IDC_UNITS, CB_GETLBTEXT, pos, 
									(LPARAM) (LPCTSTR) unit_str);
	if (rslt == CB_ERR)
		return;

	// default
	m_units = UAR_UNITS_NM;

	switch(unit_str[0])
	{
		case 'N':
			m_units = UAR_UNITS_NM;
			break;
		case 'k':
			m_units = UAR_UNITS_KM;
			break;
		case 'm':
			m_units = UAR_UNITS_METERS;
			break;
		case 'y':
			m_units = UAR_UNITS_YARDS;
			break;
		case 'f':
			m_units = UAR_UNITS_FEET;
			break;
	}

	// update the registry
	PRM_set_registry_string("ArEdit", "Units", unit_str);

	update_dlg();
}

// *************************************************************
// *************************************************************


void CUserArEditInfoDlg::OnCloseupCountry() 
{
	int pos, rslt;
	char ctry_str[21];
	CString temp;

	pos = SendDlgItemMessage(IDC_COUNTRY, CB_GETCURSEL, 0, 0);
	if (pos == CB_ERR)
		return;

	rslt = SendDlgItemMessage(IDC_COUNTRY, CB_GETLBTEXT, pos, 
									(LPARAM) (LPCTSTR) ctry_str);
	if (rslt == CB_ERR)
		return;

	ctry_str[2] = '\0';

	m_country = ctry_str;

}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnKillfocusCountry() 
{
	int pos, rslt;
	char ctry_str[21];
	CString temp;

	if (m_aborted)
		return;

	pos = SendDlgItemMessage(IDC_COUNTRY, CB_GETCURSEL, 0, 0);
	if (pos == CB_ERR)
		return;

	rslt = SendDlgItemMessage(IDC_COUNTRY, CB_GETLBTEXT, pos, 
									(LPARAM) (LPCTSTR) ctry_str);
	if (rslt == CB_ERR)
		return;

	ctry_str[2] = '\0';

	m_country = ctry_str;
}

// *************************************************************
// *************************************************************
LRESULT CUserArEditInfoDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt1_AA() 
{
	m_altdesc1 = "AA";

	enable_window(IDC_ALT1_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt2_AA() 
{
	m_altdesc2 = "AA";

	enable_window(IDC_ALT2_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt3_AA() 
{
	m_altdesc3 = "AA";

	enable_window(IDC_ALT3_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt1_AB() 
{
	m_altdesc1 = "AB";

	enable_window(IDC_ALT1_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt2_AB() 
{
	m_altdesc2 = "AB";

	enable_window(IDC_ALT2_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt3_AB() 
{
	m_altdesc3 = "AB";

	enable_window(IDC_ALT3_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt1_AT() 
{
	m_altdesc1 = "AT";

	enable_window(IDC_ALT1_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt2_AT() 
{
	m_altdesc2 = "AT";

	enable_window(IDC_ALT2_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt3_AT() 
{
	m_altdesc3 = "AT";

	enable_window(IDC_ALT3_2, FALSE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt1_BT() 
{
	m_altdesc1 = "BT";

	enable_window(IDC_ALT1_2, TRUE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt2_BT() 
{
	m_altdesc2 = "BT";

	enable_window(IDC_ALT2_2, TRUE);
}

// *************************************************************
// *************************************************************

void CUserArEditInfoDlg::OnAlt3_BT() 
{
	m_altdesc3 = "BT";

	enable_window(IDC_ALT3_2, TRUE);
}

// *************************************************************
// *************************************************************

