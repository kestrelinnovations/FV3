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

// pntinfo.cpp : implementation file
//

#include "stdafx.h"
#include "pntinfo.h"
#include "geo_tool.h"
#include "factory.h"
#include "fvwutil.h"
#include "SnapTo.h"
#include "ovl_mgr.h"

C_ar_obj *CUserARPointSheetDlg::m_rte;
C_ar_point *CUserARPointSheetDlg::m_ar_pt;
C_ar_point *CUserARPointSheetDlg::m_temp_ar_pt;
BOOL CUserARPointInfoPageDlg::m_new;
BOOL CUserARPointLocPageDlg::m_new;

class CUserARPointLocPageDlg;

/////////////////////////////////////////////////////////////////////////////
// CUserARPointSheetDlg

IMPLEMENT_DYNAMIC(CUserARPointSheetDlg, CPropertySheet)

CUserARPointSheetDlg::CUserARPointSheetDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_temp_ar_pt = new C_ar_point;
	m_ar_pt = NULL;
	m_rte = NULL;
}

// *************************************************************
// *************************************************************

CUserARPointSheetDlg::CUserARPointSheetDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_temp_ar_pt = new C_ar_point;
	m_ar_pt = NULL;
	m_rte = NULL;
}

// *************************************************************
// *************************************************************

CUserARPointSheetDlg::~CUserARPointSheetDlg()
{
	if (m_temp_ar_pt)
	{
		delete m_temp_ar_pt;
		m_temp_ar_pt = NULL;
	}
}

// *************************************************************
// *************************************************************

BOOL CUserARPointSheetDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case VK_F1: 
				if (GetActiveIndex() == 0)
					AfxGetApp()->WinHelp(HID_BASE_RESOURCE + IDD_USER_AR_POINT_INFO_DLG);
				else if (GetActiveIndex() == 1)
					AfxGetApp()->WinHelp(HID_BASE_RESOURCE + IDD_USER_AR_POINT_LOC_DLG);
				return TRUE;
      }
   }
   
   return CPropertySheet::PreTranslateMessage(pMsg);  
}


// *************************************************************
// *****************************************************************

void CUserARPointSheetDlg::WinHelp(DWORD dwData, UINT nCmd) 
{
	if (GetActiveIndex() == 0)
		AfxGetApp()->WinHelp(HID_BASE_RESOURCE + IDD_USER_AR_POINT_INFO_DLG);
	else if (GetActiveIndex() == 1)
		AfxGetApp()->WinHelp(HID_BASE_RESOURCE + IDD_USER_AR_POINT_LOC_DLG);
	
	CPropertySheet::WinHelp(dwData, nCmd);
}

// *****************************************************************
// *************************************************************

BEGIN_MESSAGE_MAP(CUserARPointSheetDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CUserARPointSheetDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserARPointSheetDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CUserARPointInfoPageDlg dialog

IMPLEMENT_DYNCREATE(CUserARPointInfoPageDlg, CPropertyPage)

CUserARPointInfoPageDlg::CUserARPointInfoPageDlg() : CPropertyPage(CUserARPointInfoPageDlg::IDD)
{
	//{{AFX_DATA_INIT(CUserARPointInfoPageDlg)
	m_location = _T("");
	m_nav_bearing_str = _T("");
	m_nav_dist_str = _T("");
	m_nav_id = _T("");
	//}}AFX_DATA_INIT

	m_map = NULL;

	m_new = TRUE;
}

// *************************************************************
// *************************************************************

CUserARPointInfoPageDlg::~CUserARPointInfoPageDlg()
{

}


// *************************************************************
// *************************************************************

void CUserARPointInfoPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserARPointInfoPageDlg)
	DDX_Text(pDX, IDC_LOCATION, m_location);
	DDX_Text(pDX, IDC_NAV_BEARING, m_nav_bearing_str);
	DDX_Text(pDX, IDC_NAV_DIST, m_nav_dist_str);
	DDX_Text(pDX, IDC_NAV_ID, m_nav_id);
	//}}AFX_DATA_MAP
}


// *************************************************************
// *************************************************************

BEGIN_MESSAGE_MAP(CUserARPointInfoPageDlg, CDialog)
	//{{AFX_MSG_MAP(CUserARPointInfoPageDlg)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD1, OnAdd)
	ON_EN_KILLFOCUS(IDC_NAV_ID, OnKillfocusNavId)
	ON_LBN_SELCHANGE(IDC_POINT_TYPE_LIST, OnSelchangePointTypeList)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserARPointInfoPageDlg message handlers

void CUserARPointInfoPageDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CPropertyPage::OnCancel();
}

// *************************************************************
// *************************************************************

void CUserARPointInfoPageDlg::OnOK() 
{
/*
	if (m_new)
	{
		if (CUserARPointSheetDlg::m_temp_ar_pt->m_usage.GetCount() < 1)
		{
			AfxMessageBox("You must select a point type");
			return;
		}
		m_ar_pt = new C_ar_point;
		m_rte->m_point_list.AddTail(m_ar_pt);
		m_new = FALSE;
	}

	UpdateData(TRUE);

	CString tstr;
	double tf;

	*m_ar_pt = *CUserARPointSheetDlg::m_temp_ar_pt;
	tf = atof(m_nav_bearing_str);
	m_ar_pt->m_fix.Format("%3.0f", tf);
	tf = atof(m_nav_dist_str);
	tstr.Format("%3.0f", tf);
	m_ar_pt->m_fix += tstr;

	// is point is anchor position point, move it
	BOOL unique;
	if (m_ar_pt->has_usage_string("AN", &unique))
	{		
		m_rte->m_anchor_pt_lat = m_ar_pt->m_lat;
		m_rte->m_anchor_pt_lon = m_ar_pt->m_lon;
		m_rte->make_anchor((ActiveMap*) m_map);
	}

	ASSERT(m_ar_pt->m_usage.GetCount() > 0);
*/
	CPropertyPage::OnOK();
}

// *************************************************************
// *****************************************************************

void CUserARPointInfoPageDlg::OnClose() 
{
	CDialog::OnClose();
}

// *****************************************************************
// *************************************************************

BOOL CUserARPointInfoPageDlg::OnApply() 
{
	if (CUserARPointSheetDlg::m_ar_pt == NULL)
	{
		if (CUserARPointSheetDlg::m_temp_ar_pt->m_usage.GetCount() < 1)
		{
			AfxMessageBox("You must select a point type");
			return FALSE;
		}
		CUserARPointSheetDlg::m_ar_pt = new C_ar_point;
		CUserARPointSheetDlg::m_rte->m_point_list.AddTail(CUserARPointSheetDlg::m_ar_pt);
		m_new = FALSE;
		CUserARPointLocPageDlg::m_new = FALSE;
	}

	UpdateData(TRUE);

	CString tstr;
	double tf;

	*CUserARPointSheetDlg::m_ar_pt = *CUserARPointSheetDlg::m_temp_ar_pt;
	tf = atof(m_nav_bearing_str);
	CUserARPointSheetDlg::m_ar_pt->m_fix.Format("%3.0f", tf);
	tf = atof(m_nav_dist_str);
	tstr.Format("%3.0f", tf);
	CUserARPointSheetDlg::m_ar_pt->m_fix += tstr;
	OVL_get_overlay_manager()->InvalidateOverlaysOfType(FVWID_Overlay_TrackOrbit);

	ASSERT(CUserARPointSheetDlg::m_ar_pt->m_usage.GetCount() > 0);

	return CPropertyPage::OnApply();
}

// *************************************************************
// *************************************************************

void CUserARPointInfoPageDlg::OnSelchangePointTypeList() 
{
	int cnt, rslt;
	CWnd *wnd;
	BOOL enable = FALSE;
	char buf[100];

	wnd = GetDlgItem(IDC_REMOVE);
	if (wnd == NULL)
		return;

	cnt = SendDlgItemMessage(IDC_POINT_TYPE_LIST, LB_GETCOUNT, 0, 0);
	if (cnt > 1)
	{
		cnt = SendDlgItemMessage(IDC_POINT_TYPE_LIST, LB_GETCURSEL, 0, 0);
		if (cnt != LB_ERR)
		{
			rslt = SendDlgItemMessage(IDC_POINT_TYPE_LIST, LB_GETTEXT, cnt, (LPARAM) buf);
			if (strncmp(buf, "Anchor", 6))
				enable = TRUE;
		}
	}

	wnd->EnableWindow(enable);
}

// *************************************************************
// *************************************************************

void CUserARPointInfoPageDlg::OnRemove() 
{
	int num, rslt;
	char typestr[81];
	CString type;

	if (CUserARPointSheetDlg::m_temp_ar_pt == NULL)
		return;

	if (CUserARPointSheetDlg::m_temp_ar_pt->m_usage.GetCount() < 2)
		return;

	num = SendDlgItemMessage(IDC_POINT_TYPE_LIST, LB_GETCURSEL, 0, 0);
	if (num == CB_ERR)
		return;

	rslt = SendDlgItemMessage(IDC_POINT_TYPE_LIST, LB_GETTEXT, num, 
									(LPARAM) (LPCTSTR) typestr);
	if (rslt == CB_ERR)
		return;
	
	type = typestr;
	if (!type.CompareNoCase("Initial Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->add_usage("IP");
	else if (!type.CompareNoCase("Control Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->remove_usage("CP");
	else if (!type.CompareNoCase("Entry Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->remove_usage("ET");
	else if (!type.CompareNoCase("Exit Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->remove_usage("EX");
	else if (!type.CompareNoCase("Anchor Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->remove_usage("AN");
	else if (!type.CompareNoCase("Nav Check Pt"))
		CUserARPointSheetDlg::m_temp_ar_pt->remove_usage("NC");
	
		
	update_point_type_list();
}

// *************************************************************
// *************************************************************

void CUserARPointInfoPageDlg::OnAdd() 
{
	int num, rslt;
	char typestr[81];
	CString type;
	double dist, ang;
//	C_ar_point *pt;

	num = SendDlgItemMessage(IDC_PT_TYPE, CB_GETCURSEL, 0, 0);
	if (num == CB_ERR)
		return;

	rslt = SendDlgItemMessage(IDC_PT_TYPE, CB_GETLBTEXT, num, 
									(LPARAM) (LPCTSTR) typestr);
	if (rslt == CB_ERR)
		return;

	if (CUserARPointSheetDlg::m_rte == NULL)
		return;
	
	type = typestr;
/*
	if ((m_rte != NULL) && (m_ar_pt == NULL))
	{
		double dist, ang;

		pt = new C_ar_point(m_rte);
		pt->m_obj = m_rte;
		pt->m_id = m_rte->m_id;
		pt->m_direction = m_rte->m_direction;
		pt->m_lat = CUserARPointSheetDlg::m_temp_ar_pt->m_lat;
		pt->m_lon = CUserARPointSheetDlg::m_temp_ar_pt->m_lon;
		m_ar_pt = pt;

		// compute dist and angle
		GEO_geo_to_distance(m_rte->m_anchor_pt_lat, m_rte->m_anchor_pt_lon, 
							CUserARPointSheetDlg::m_temp_ar_pt->m_lat, 
							CUserARPointSheetDlg::m_temp_ar_pt->m_lon, &dist, &ang);
		pt->m_dist = dist / 1000.0;
		pt->m_angle = ang - m_rte->m_angle;

		m_rte->m_point_list.AddTail(pt);
	}
*/
	// compute dist and angle
	GEO_geo_to_distance(CUserARPointSheetDlg::m_rte->m_anchor_pt_lat, CUserARPointSheetDlg::m_rte->m_anchor_pt_lon, 
						CUserARPointSheetDlg::m_temp_ar_pt->m_lat, 
						CUserARPointSheetDlg::m_temp_ar_pt->m_lon, &dist, &ang);
	CUserARPointSheetDlg::m_temp_ar_pt->m_dist = dist / 1000.0;
	CUserARPointSheetDlg::m_temp_ar_pt->m_angle = ang - CUserARPointSheetDlg::m_rte->m_angle;

	C_ar_edit::m_old_cnt = CUserARPointSheetDlg::m_rte->m_point_list.GetCount();

	if (!type.CompareNoCase("Initial Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->add_usage("IP");
	else if (!type.CompareNoCase("Control Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->add_usage("CP");
	else if (!type.CompareNoCase("Entry Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->add_usage("ET");
	else if (!type.CompareNoCase("Exit Point"))
		CUserARPointSheetDlg::m_temp_ar_pt->add_usage("EX");
	else if (!type.CompareNoCase("Nav Check Pt"))
		CUserARPointSheetDlg::m_temp_ar_pt->add_usage("NC");
		
	update_point_type_list();
}
// end of OnAdd


// *************************************************************
// *************************************************************

void CUserARPointInfoPageDlg::OnKillfocusNavId() 
{
	SnapToInfo db_info;
	int pos;
	CString tstr, oldid;

	oldid = m_nav_id;

	UpdateData(TRUE);

	if (oldid.Compare(m_nav_id))
		SetModified();

	if (CUserARPointSheetDlg::m_temp_ar_pt == NULL)
		return;

	if (m_nav_id.GetLength() == 0)
	{
		CUserARPointSheetDlg::m_temp_ar_pt->m_fix = "";
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_type = "";
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_country = "";
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_type = "";
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lat = CUserARPointSheetDlg::m_temp_ar_pt->m_lat;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lon = CUserARPointSheetDlg::m_temp_ar_pt->m_lon;
		m_nav_bearing_str = "";
		m_nav_dist_str = "";
		return;
	}

	if (CFvwUtil::get_instance()->db_lookup(m_nav_id, db_info, TRUE))
	{
	   // check for navaid
		if (db_info.m_pnt_src_rs != SnapToInfo::NAVAID)
		{
			AfxMessageBox("Could not find Navaid in DAFIF database");
			goto FAIL;
		}

		m_nav_lat = db_info.m_lat;
		m_nav_lon = db_info.m_lon;
		m_nav_id = db_info.m_fix_rs;
		pos = m_nav_id.Find("/");
		if (pos > 0)
			m_nav_id.Left(pos);
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_type = db_info.m_nav_type;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_country = db_info.m_country_code;

		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id = db_info.m_fix_rs;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_type = C_ar_edit::get_navaid_type(db_info.m_nav_type);
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_flag = " ";
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_country = db_info.m_country_code;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lat = db_info.m_lat;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lon = db_info.m_lon;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_mag_var = db_info.m_mag_var;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_slave_var = db_info.m_slave_var;
		tstr = db_info.m_db_lookup_rs;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_key = tstr.Mid(5, 1);

		// compute fix
		CUserARPointSheetDlg::m_temp_ar_pt->calc_nav_fix();

		m_nav_bearing_str = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Left(3);
		m_nav_dist_str = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Right(3);
	}
	else
	{
		AfxMessageBox("Could not find Navaid in DAFIF database");
		goto FAIL;
	}

	UpdateData(FALSE);

	return;

FAIL:
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id = "";
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_type = "";
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_flag = " ";
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_country = "";
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lat = 0.0;
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lon = 0.0;
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_mag_var = 0;
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_slave_var = 0;
	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_key = "";
	m_nav_bearing_str = "";
	m_nav_dist_str = "";

	UpdateData(FALSE);
}

// *************************************************************
// *************************************************************

BOOL CUserARPointInfoPageDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   const int DATUM_TYPE_LEN = 21;
	char datum_type[DATUM_TYPE_LEN];
   const int TSTR_LEN = 255;
	char tstr[TSTR_LEN];
	CString sline;

	ASSERT(CUserARPointSheetDlg::m_rte);

	ASSERT(CUserARPointSheetDlg::m_temp_ar_pt);

	// set the route of the temp ar point
	if ((CUserARPointSheetDlg::m_rte != NULL) && (CUserARPointSheetDlg::m_temp_ar_pt->m_obj == NULL))
	{
		CUserARPointSheetDlg::m_temp_ar_pt->m_obj = CUserARPointSheetDlg::m_rte;
		CUserARPointSheetDlg::m_temp_ar_pt->m_id = CUserARPointSheetDlg::m_rte->m_id;
	}

	// init the point type combo box
	m_type = "Control Point";
	SendDlgItemMessage(IDC_PT_TYPE, CB_SETCURSEL, 0, 0);
	if (CUserARPointSheetDlg::m_ar_pt != NULL)
	{
//		SendDlgItemMessage(IDC_PT_TYPE, CB_SETCURSEL, 0, 0);

//		if (!CUserARPointSheetDlg::m_temp_ar_pt->m_id.Compare("Untitled"))
			*CUserARPointSheetDlg::m_temp_ar_pt = *CUserARPointSheetDlg::m_ar_pt;

		update_point_type_list();

		update_nav_info();

		GEO_lat_lon_to_string(CUserARPointSheetDlg::m_ar_pt->m_lat, CUserARPointSheetDlg::m_ar_pt->m_lon, tstr, TSTR_LEN);
	} 
	else
		GEO_lat_lon_to_string(CUserARPointSheetDlg::m_temp_ar_pt->m_lat, CUserARPointSheetDlg::m_temp_ar_pt->m_lon, tstr, TSTR_LEN);
	
	m_location = tstr;

	sline = " (";
	GEO_get_default_display(datum_type, DATUM_TYPE_LEN);
	if (!strcmp(datum_type, "SECONDARY"))
		GEO_get_secondary_datum(tstr, TSTR_LEN);
	else
		GEO_get_primary_datum(tstr, TSTR_LEN);
	sline += tstr;
	sline += ") ";
	m_location += sline;

	OnSelchangePointTypeList();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// end of OnInitDialog

// *****************************************************************
// *****************************************************************

void CUserARPointInfoPageDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	update_nav_info();
	
	// Do not call CDialog::OnPaint() for painting messages
}

// *****************************************************************
// *****************************************************************

void CUserARPointInfoPageDlg::update_point_type_list() 
{
	CString tstr("");
	CString typestr, tmp, tmp2;
	BOOL notdone;

	SendDlgItemMessage(IDC_POINT_TYPE_LIST, LB_RESETCONTENT, 0, 0);

	notdone = CUserARPointSheetDlg::m_temp_ar_pt->get_first_usage_string(typestr);
	if (notdone)
		SendDlgItemMessage(IDC_POINT_TYPE_LIST, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) typestr);
	while (notdone)
	{
		notdone = CUserARPointSheetDlg::m_temp_ar_pt->get_next_usage_string(typestr);
		if (notdone)
			SendDlgItemMessage(IDC_POINT_TYPE_LIST, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) typestr);
	}
	SendDlgItemMessage(IDC_POINT_TYPE_LIST, CB_SETCURSEL, 0, 0);
}

// *****************************************************************
// *****************************************************************

void CUserARPointInfoPageDlg::update_nav_info() 
{
	if (CUserARPointSheetDlg::m_temp_ar_pt == NULL)
		return;

	CString tstr;

//	CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id = CUserARPointSheetDlg::m_ar_pt->m_nav_id;
	if (CUserARPointSheetDlg::m_temp_ar_pt->m_fix.GetLength() > 5)
	{
		tstr = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Left(3);
		m_nav_bearing_str = tstr;
//		m_nav_bearing = atof(tstr);
		tstr = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Right(3);
//		m_nav_dist = atof(tstr);
		m_nav_dist_str = tstr;
		m_nav_id = CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id;
		UpdateData(FALSE);
	}
}

// *****************************************************************
// *****************************************************************

void CUserARPointInfoPageDlg::set_point_ptr(C_ar_point *pt) 
{
	CUserARPointSheetDlg::m_ar_pt = pt;
	if (pt != NULL)
		m_new = FALSE;
}

// *****************************************************************
// *****************************************************************

void CUserARPointInfoPageDlg::set_route_ptr(C_ar_obj *rte) 
{
	CUserARPointSheetDlg::m_rte = rte;
	CUserARPointSheetDlg::m_ar_pt = NULL;
}

// *****************************************************************
// *****************************************************************

void CUserARPointInfoPageDlg::set_map(ViewMapProj *map)
{
	m_map = map;
}

// *****************************************************************
// *****************************************************************

void CUserARPointInfoPageDlg::set_location(double lat, double lon) 
{
	CUserARPointSheetDlg::m_temp_ar_pt->m_lat = lat;
	CUserARPointSheetDlg::m_temp_ar_pt->m_lon = lon;
	m_old_lat = lat;
	m_old_lon = lon;
}

// *****************************************************************
// *****************************************************************

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CUserARPointLocPageDlg property page

IMPLEMENT_DYNCREATE(CUserARPointLocPageDlg, CPropertyPage)

CUserARPointLocPageDlg::CUserARPointLocPageDlg() : CPropertyPage(CUserARPointLocPageDlg::IDD)
{
	//{{AFX_DATA_INIT(CUserARPointLocPageDlg)
	m_auto_mag_var = FALSE;
	m_calced_magvar = _T("");
	m_mag_var_str = _T("");
	m_bearing_str = _T("");
	m_distance_str = _T("");
	m_fix = _T("");
	//}}AFX_DATA_INIT

	m_map = NULL;
	m_calced_fix = FALSE;
	m_changed_bearing = FALSE;
	m_changed_dist = FALSE;
}

// *************************************************************
// *************************************************************

CUserARPointLocPageDlg::~CUserARPointLocPageDlg()
{
}

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserARPointLocPageDlg)
	DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geocx);
	DDX_Check(pDX, IDC_AUTO_MAGVAR, m_auto_mag_var);
	DDX_Text(pDX, IDC_CALCED_MAGVAR, m_calced_magvar);
	DDX_Text(pDX, IDC_MAG_VAR, m_mag_var_str);
	DDX_Text(pDX, IDC_FIX_BEARING, m_bearing_str);
	DDX_Text(pDX, IDC_FIX_DISTANCE, m_distance_str);
	DDX_Text(pDX, IDC_FIX, m_fix);
	//}}AFX_DATA_MAP
}


// *************************************************************
// *************************************************************

BEGIN_MESSAGE_MAP(CUserARPointLocPageDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CUserARPointLocPageDlg)
	ON_EN_KILLFOCUS(IDC_FIX, OnKillfocusFix)
	ON_EN_KILLFOCUS(IDC_FIX_BEARING, OnKillfocusFixBearing)
	ON_EN_KILLFOCUS(IDC_FIX_DISTANCE, OnKillfocusFixDistance)
	ON_BN_CLICKED(IDC_RB_BEARING_MAG, OnRbBearingMag)
	ON_BN_CLICKED(IDC_RB_BEARING_TRUE, OnRbBearingTrue)
	ON_BN_CLICKED(IDC_AUTO_MAGVAR, OnAutoMagvar)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_FIX_BEARING, OnChangeFixBearing)
	ON_EN_CHANGE(IDC_FIX_DISTANCE, OnChangeFixDistance)
	ON_EN_SETFOCUS(IDC_FIX_BEARING, OnSetfocusFixBearing)
	ON_EN_KILLFOCUS(IDC_MAG_VAR, OnKillfocusMagVar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserARPointLocPageDlg message handlers

void CUserARPointLocPageDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CPropertyPage::OnCancel();
}

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::OnOK() 
{
	CPropertyPage::OnOK();
}

// *************************************************************
// *************************************************************

BOOL CUserARPointLocPageDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	ASSERT(CUserARPointSheetDlg::m_temp_ar_pt); // this should have been allocated by CUserARPointInfoPageDlg

	m_geocx.SetGeocx( CUserARPointSheetDlg::m_temp_ar_pt->m_lat, CUserARPointSheetDlg::m_temp_ar_pt->m_lon );
	
	m_new = CUserARPointInfoPageDlg::m_new;

	// init the true/mag controls
	CString tstr;
	CString magvar_str;
	double tf;

	m_fix = CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id;
	m_old_fix =  m_fix;
	if (CUserARPointSheetDlg::m_temp_ar_pt->m_fix.GetLength() > 5)
	{
		tstr = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Left(3);
		m_bearing_str = tstr;
		m_bearing = atof(tstr);
		tstr = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Right(3);
		m_distance = atof(tstr);
		m_distance_str = tstr;
	}

//	m_bearing = 0.0;
//	m_distance = 0.0;
	m_auto_mag_var = TRUE;
	calc_mag_var();
	if (m_mag_var < 0.0)
		magvar_str = "W";
	else
		magvar_str = "E";

	tf = fabs(m_mag_var);
	tstr.Format("%5.1f", tf);
	tstr.TrimRight();
	tstr.TrimLeft();
	magvar_str += tstr;
//	if (m_auto_mag_var)
		m_calced_magvar = magvar_str;
//	else
		m_mag_var_str = magvar_str;

	CUserARPointSheetDlg::m_temp_ar_pt->calc_nav_fix(m_auto_mag_var, m_mag_var);

//	m_bearing_str.Format("%5.1f", m_bearing);
//	m_distance_str.Format("%7.1f", m_distance);

	GetDlgItem(IDC_AUTO_MAGVAR)->EnableWindow(m_mag_bearing);
	GetDlgItem(IDC_MAG_VAR)->EnableWindow(m_mag_bearing && !m_auto_mag_var);
	GetDlgItem(IDC_MAN_MAG_TEXT)->EnableWindow(m_mag_bearing && !m_auto_mag_var); 
	GetDlgItem(IDC_CALCED_MAGVAR)->EnableWindow(m_mag_bearing && m_auto_mag_var); 

//	GetDlgItem(IDC_RB_BEARING_TRUE)->EnableWindow(FALSE);
//	GetDlgItem(IDC_RB_BEARING_MAG)->EnableWindow(FALSE);
//	GetDlgItem(IDC_AUTO_MAGVAR)->EnableWindow(FALSE);

	if (m_mag_bearing)
		CheckRadioButton(IDC_RB_BEARING_MAG, IDC_RB_BEARING_TRUE, IDC_RB_BEARING_MAG);
	else
		CheckRadioButton(IDC_RB_BEARING_MAG, IDC_RB_BEARING_TRUE, IDC_RB_BEARING_TRUE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// end of OnInitDialog

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::OnKillfocusFix() 
{
	UpdateData(TRUE);
	if (m_old_fix.Compare(m_fix))
	{
		m_calced_fix = TRUE;
		calc_fix();
		SetModified();	
	}
	m_changed_bearing = FALSE;
	m_changed_dist = FALSE;
}

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::OnKillfocusFixBearing() 
{
	double bearing;

	UpdateData(TRUE);

//	GetDlgItem(IDC_RB_BEARING_TRUE)->EnableWindow(FALSE);
//	GetDlgItem(IDC_RB_BEARING_MAG)->EnableWindow(FALSE);
//	GetDlgItem(IDC_AUTO_MAGVAR)->EnableWindow(FALSE);

	bearing = atof(m_bearing_str);	
	if ((bearing < 0.0) || (bearing > 360.0))
	{
		AfxMessageBox("Bearing must be between 0 and 360");
		m_bearing_str.Format("%5.1f", m_bearing);
		UpdateData(FALSE);
		return;
	}

//	if (m_bearing != bearing)
	if (m_bearing_str.Compare(m_calced_bearing))
	{
		m_bearing = bearing;
		m_calced_fix = !m_changed_bearing;
		calc_fix();
		SetModified();
		m_changed_bearing = FALSE;
	}
}

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::OnKillfocusFixDistance() 
{
	double range;

	UpdateData(TRUE);

	range = atof(m_distance_str);	
	if (range < 0)
	{
		AfxMessageBox("Distance must be greater then 0");
		m_distance_str.Format("%7.1f", m_distance);
		UpdateData(FALSE);
		return;
	}

//	if (m_distance != range)
	if (m_distance_str.Compare(m_calced_dist))
	{			 
		m_distance = range;
		m_calced_fix = !m_changed_dist;
		calc_fix();
		SetModified();
		m_changed_dist = FALSE;
	}
}

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::calc_fix() 
{
	SnapToInfo db_info;
	CString tstr, tstr2;
	double range, bearing;
	double lat, lon;
	double mvar;
	int rslt, pos;
   const int DATUM_TYPE_LEN = 21;
	char datum_type[DATUM_TYPE_LEN];
   const int TSTR_LEN = 100;
	char tstr_ch[TSTR_LEN];
	BOOL good_db;

	UpdateData(TRUE);

	m_fix.MakeUpper();
	m_fix.TrimRight();
	m_fix.TrimLeft();

   // if no match was found, make sure the string starts with a '.' to
   // indicate that it is a comment, i.e., no match was found
   if (!CFvwUtil::get_instance()->db_lookup(m_fix, db_info, TRUE))
   {
		// check for coordinate
		GEO_get_default_display(datum_type, DATUM_TYPE_LEN);
		if (!strcmp(datum_type, "SECONDARY"))
			GEO_get_secondary_datum(tstr_ch, TSTR_LEN);
		else
			GEO_get_primary_datum(tstr_ch, TSTR_LEN);

		rslt = GEO_string_to_lat_lon(m_fix, tstr_ch, &lat, &lon);
		if (rslt != SUCCESS)
		{
			m_fix = "<No Fix>";
			UpdateData(FALSE);
			return;
		}
		m_fix_lat = lat;
		m_fix_lon = lon;
		good_db = FALSE;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lat = 0.0;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lon = 0.0;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id = "";
	}
	else
	{
		m_fix_lat = db_info.m_lat;
		m_fix_lon = db_info.m_lon;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lat = m_fix_lat;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_lon = m_fix_lon;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id = db_info.m_fix_rs;
		pos = CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id.Find("/");
		if (pos > 0)
			CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id.Left(pos) = CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_type = db_info.m_nav_type;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_slave_var = db_info.m_slave_var;
		CUserARPointSheetDlg::m_temp_ar_pt->m_nav_country = db_info.m_country_code;
		good_db = TRUE;

		// compute the fix
		C_ar_point tpt;
		if (CUserARPointSheetDlg::m_temp_ar_pt != NULL)
		{
			tpt.m_lat = CUserARPointSheetDlg::m_temp_ar_pt->m_lat;
			tpt.m_lon = CUserARPointSheetDlg::m_temp_ar_pt->m_lon;
			tpt.m_nav_slave_var = CUserARPointSheetDlg::m_temp_ar_pt->m_nav_slave_var;
			tpt.m_nav_lat = m_fix_lat;
			tpt.m_nav_lon = m_fix_lon;
			tpt.calc_nav_fix(m_auto_mag_var, m_mag_var);
			tstr = tpt.m_fix.Left(3);
			m_magnetic_bearing = atof(tstr);
			tstr = tpt.m_true_fix.Left(3);
			m_true_bearing = atof(tstr);
			m_calced_bearing = tstr;
			if (m_calced_fix)
			{
				m_bearing_str = tstr;
				m_bearing = atof(tstr);
			}
			tstr = tpt.m_fix.Right(3);
			m_calced_dist = tstr;
			if (m_calced_fix)
			{
				m_distance = atof(tstr);
				m_distance_str = tstr;
				CUserARPointSheetDlg::m_temp_ar_pt->calc_nav_fix(m_auto_mag_var, m_mag_var); 
			}
			else
			{
				tstr.Format("%03d", (int) m_bearing);
				CUserARPointSheetDlg::m_temp_ar_pt->m_fix = tstr;
				tstr.Format("%03d", (int) m_distance);
				CUserARPointSheetDlg::m_temp_ar_pt->m_fix += tstr;
			}
		}
	}

//	m_bearing = atof(m_bearing_str);
	m_distance = atof(m_distance_str);
	bearing = m_bearing;
	range = atof(m_distance_str);
	if ((bearing < 0) || (bearing > 360))
	{
//		AfxMessageBox("Bearing must be between 0 and 360");
//		m_fix_bearing_str.Format("%5.1f", m_fix_bearing);
//		UpdateData(FALSE);
		return;
	}
	if (range < 0)
	{
//		AfxMessageBox("Range must be greater then 0");
//		m_fix_dist_str.Format("%7.1f", m_fix_distance);
//		UpdateData(FALSE);
		return;
	}


	if (good_db)
	{
		if (db_info.m_slave_var != 180.0)
			mvar = db_info.m_slave_var;
		else if (db_info.m_mag_var != 180.0)
			mvar = db_info.m_mag_var;
		else
			mvar = 0.0;
	}
	else
	{
		mvar = 0.0;
		if (m_mag_bearing)
		{
			// compute the local magnetic variation
			int year, month, alt, rslt;
			double magvar;
			SYSTEMTIME time;
			GetSystemTime(&time);
			year = time.wYear;
			month = time.wMonth;
			alt = 0;
			rslt = GEO_magnetic_variation(m_fix_lat, m_fix_lon, year, month, alt, &magvar);
			if (rslt != SUCCESS)
				magvar = 0.0;
			mvar = magvar;
		}
	}

	bearing += mvar;

	GEO_distance_to_geo(m_fix_lat, m_fix_lon, 
				m_distance * 1852.0, bearing, &lat, &lon);
	if (!m_calced_fix)
	{
		CUserARPointSheetDlg::m_temp_ar_pt->m_lat = lat;
		CUserARPointSheetDlg::m_temp_ar_pt->m_lon = lon;
		m_geocx.SetGeocx(CUserARPointSheetDlg::m_temp_ar_pt->m_lat, CUserARPointSheetDlg::m_temp_ar_pt->m_lon);
	}

	UpdateData(FALSE);
} // end of calc_fix

// calculates the magvar for this object at the current time
// uses m_lat, m_lon
// changes m_mag_var
BOOL CUserARPointLocPageDlg::calc_mag_var()
{
	// compute the local magnetic variation
	int year, month, alt, rslt;
	double magvar, tlat, tlon;
	SYSTEMTIME time;
	
	GetSystemTime(&time);
	year = time.wYear;
	month = time.wMonth;
	alt = 0;
	tlat = m_geocx.GetLatitude();
	tlon = m_geocx.GetLongitude();
	rslt = GEO_magnetic_variation(tlat, tlon, year, month, alt, &magvar);
	if (rslt != SUCCESS)
		return FALSE;

	m_mag_var = magvar;
	return TRUE;
}
// end calc_mag_var

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::OnRbBearingMag() 
{
	UpdateData(TRUE);

	m_mag_bearing = TRUE;

	CUserARPointSheetDlg::m_temp_ar_pt->calc_nav_fix(m_auto_mag_var, m_mag_var);
	m_bearing_str = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Left(3);
	GetDlgItem(IDC_AUTO_MAGVAR)->EnableWindow(m_mag_bearing);
	GetDlgItem(IDC_MAG_VAR)->EnableWindow(m_mag_bearing && !m_auto_mag_var); 
	GetDlgItem(IDC_MAN_MAG_TEXT)->EnableWindow(m_mag_bearing && !m_auto_mag_var); 
	GetDlgItem(IDC_CALCED_MAGVAR)->EnableWindow(m_mag_bearing && m_auto_mag_var); 

	UpdateData(FALSE);
}

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::OnRbBearingTrue() 
{
	UpdateData(TRUE);

	m_mag_bearing = FALSE;

	CUserARPointSheetDlg::m_temp_ar_pt->calc_nav_fix(m_auto_mag_var, m_mag_var);
	m_bearing_str = CUserARPointSheetDlg::m_temp_ar_pt->m_true_fix.Left(3);
	GetDlgItem(IDC_AUTO_MAGVAR)->EnableWindow(m_mag_bearing);
	GetDlgItem(IDC_MAG_VAR)->EnableWindow(m_mag_bearing && !m_auto_mag_var);
	GetDlgItem(IDC_MAN_MAG_TEXT)->EnableWindow(m_mag_bearing && !m_auto_mag_var); 
	GetDlgItem(IDC_CALCED_MAGVAR)->EnableWindow(m_mag_bearing && m_auto_mag_var); 

	UpdateData(FALSE);
}

// *************************************************************
// *************************************************************

void CUserARPointLocPageDlg::OnAutoMagvar() 
{
	UpdateData(TRUE);

	double tf;
	CString tstr, magvar;

	double mvar = atof(m_mag_var_str);

	CUserARPointSheetDlg::m_temp_ar_pt->calc_nav_fix(m_auto_mag_var, mvar);

	GetDlgItem(IDC_AUTO_MAGVAR)->EnableWindow(m_mag_bearing);
	GetDlgItem(IDC_MAG_VAR)->EnableWindow(m_mag_bearing && !m_auto_mag_var); 
	GetDlgItem(IDC_MAN_MAG_TEXT)->EnableWindow(m_mag_bearing && !m_auto_mag_var); 
	GetDlgItem(IDC_CALCED_MAGVAR)->EnableWindow(m_mag_bearing && m_auto_mag_var); 
	calc_mag_var();
	if (m_mag_var < 0.0)
		magvar = "W";
	else
		magvar = "E";

	tf = fabs(m_mag_var);
	tstr.Format("%5.1f", tf);
	tstr.TrimRight();
	tstr.TrimLeft();
	magvar += tstr;
//	if (m_auto_mag_var)
		m_calced_magvar = magvar;
//	else
		m_mag_var_str = magvar;

	UpdateData(FALSE);
}

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::set_point_ptr(C_ar_point *pt) 
{
	CUserARPointSheetDlg::m_ar_pt = pt;
}

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::set_route_ptr(C_ar_obj *rte) 
{
	CUserARPointSheetDlg::m_rte = rte;
	CUserARPointSheetDlg::m_ar_pt = NULL;
}

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::set_location(double lat, double lon) 
{
//	m_temp_ar_pt->m_lat = lat;
//	m_temp_ar_pt->m_lon = lon;
	m_old_lat = lat;
	m_old_lon = lon;
}

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::set_map(ViewMapProj *map)
{
	m_map = map;
}

// *****************************************************************
// *****************************************************************


BEGIN_EVENTSINK_MAP(CUserARPointLocPageDlg, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CUserARPointLocPageDlg)
	ON_EVENT(CUserARPointLocPageDlg, IDC_GEOCX1CTRL1, 2 /* OCXCHANGE */, OnOCXCHANGEGeocx1ctrl1, VTS_NONE)
	ON_EVENT(CUserARPointLocPageDlg, IDC_GEOCX1CTRL1, 1 /* OCXKILLFOCUS */, OnOCXKILLFOCUSGeocx1ctrl1, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::OnOCXCHANGEGeocx1ctrl1() 
{
	SetModified();	
}

// *****************************************************************
// *****************************************************************


BOOL CUserARPointLocPageDlg::OnApply() 
{
	ASSERT(CUserARPointSheetDlg::m_temp_ar_pt);

	if (CUserARPointSheetDlg::m_ar_pt == NULL)
	{
		if (CUserARPointSheetDlg::m_temp_ar_pt->m_usage.GetCount() < 1)
		{
			AfxMessageBox("You must select a point type");
			return FALSE;
		}
		CUserARPointSheetDlg::m_ar_pt = new C_ar_point;
		CUserARPointSheetDlg::m_rte->m_point_list.AddTail(CUserARPointSheetDlg::m_ar_pt);
		m_new = FALSE;
		CUserARPointInfoPageDlg::m_new = FALSE;
	}

	*CUserARPointSheetDlg::m_ar_pt = *CUserARPointSheetDlg::m_temp_ar_pt;

	if (CUserARPointSheetDlg::m_rte == NULL)
		return FALSE;

	UpdateData(TRUE);

	ASSERT(m_geocx);
	if (m_geocx)
	{
		degrees_t tlat = m_geocx.GetLatitude();
		degrees_t tlon = m_geocx.GetLongitude();
		CUserARPointSheetDlg::m_ar_pt->m_lat = tlat;
		CUserARPointSheetDlg::m_ar_pt->m_lon = tlon;
		CUserARPointSheetDlg::m_ar_pt->calc_dist_angle();
	}

	CString tstr;
	double tf;

	tf = m_bearing;
	CUserARPointSheetDlg::m_ar_pt->m_fix.Format("%03.0f", tf);
	tf = m_distance;
	tstr.Format("%03.0f", tf);
	CUserARPointSheetDlg::m_ar_pt->m_fix += tstr;

	if (m_fix.GetLength() > 0)
	{
//		m_ar_pt->m_fix = m_fix;
//		m_ar_pt->m_fix_lat = m_fix_lat;
//		m_ar_pt->m_fix_lon;
	}

	// is point is anchor position point, move it
	if (CUserARPointSheetDlg::m_ar_pt->m_anchor_point)
	{		
		CUserARPointSheetDlg::m_rte->m_anchor_pt_lat = CUserARPointSheetDlg::m_ar_pt->m_lat;
		CUserARPointSheetDlg::m_rte->m_anchor_pt_lon = CUserARPointSheetDlg::m_ar_pt->m_lon;

		CUserARPointSheetDlg::m_rte->make_anchor(m_map);
		OVL_get_overlay_manager()->InvalidateOverlaysOfType(FVWID_Overlay_TrackOrbit);
	}

	ASSERT(CUserARPointSheetDlg::m_ar_pt->m_usage.GetCount() > 0);

	return CPropertyPage::OnApply();
}


// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::OnClose() 
{	
	CPropertyPage::OnClose();
}

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::OnOCXKILLFOCUSGeocx1ctrl1() 
{
	CString tstr;

	degrees_t tlat = m_geocx.GetLatitude();
	degrees_t tlon = m_geocx.GetLongitude();
	CUserARPointSheetDlg::m_temp_ar_pt->m_lat = tlat;
	CUserARPointSheetDlg::m_temp_ar_pt->m_lon = tlon;
	CUserARPointSheetDlg::m_temp_ar_pt->calc_dist_angle();

	tstr = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Left(3);
	m_calced_bearing = tstr;
	m_bearing_str = tstr;
	m_bearing = atof(tstr);
	tstr = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Right(3);
	m_distance = atof(tstr);
	m_distance_str = tstr;
}

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	m_mag_bearing = TRUE;

	GetDlgItem(IDC_AUTO_MAGVAR)->EnableWindow(m_mag_bearing);
	GetDlgItem(IDC_MAG_VAR)->EnableWindow(m_mag_bearing && !m_auto_mag_var);
	GetDlgItem(IDC_MAN_MAG_TEXT)->EnableWindow(m_mag_bearing && !m_auto_mag_var); 
	GetDlgItem(IDC_CALCED_MAGVAR)->EnableWindow(m_mag_bearing && m_auto_mag_var); 

	if (m_mag_bearing)
		CheckRadioButton(IDC_RB_BEARING_MAG, IDC_RB_BEARING_TRUE, IDC_RB_BEARING_MAG);
	else
		CheckRadioButton(IDC_RB_BEARING_MAG, IDC_RB_BEARING_TRUE, IDC_RB_BEARING_TRUE);

	if (CUserARPointSheetDlg::m_temp_ar_pt->m_fix.GetLength() > 5)
	{
		CString tstr;

		tstr = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Left(3);
		m_bearing_str = tstr;
		m_bearing = atof(tstr);
		tstr = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Right(3);
		m_distance = atof(tstr);
		m_distance_str = tstr;
		m_fix = CUserARPointSheetDlg::m_temp_ar_pt->m_nav_id;
	}
	UpdateData(FALSE);
}

// *****************************************************************
// *****************************************************************


void CUserARPointLocPageDlg::OnChangeFixBearing() 
{
	m_changed_bearing = TRUE;
}

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::OnChangeFixDistance() 
{
	m_changed_dist = TRUE;
}

// *****************************************************************
// *****************************************************************


void CUserARPointLocPageDlg::OnSetfocusFixBearing() 
{
//	GetDlgItem(IDC_RB_BEARING_TRUE)->EnableWindow(TRUE);
//	GetDlgItem(IDC_RB_BEARING_MAG)->EnableWindow(TRUE);
//	GetDlgItem(IDC_AUTO_MAGVAR)->EnableWindow(TRUE);
	
}

// *****************************************************************
// *****************************************************************

void CUserARPointLocPageDlg::OnKillfocusMagVar() 
{
	UpdateData(TRUE);

	char dir;
	int len;
	double tf;
	CString tstr, oldstr;

	if (m_mag_var_str.GetLength() < 2)
		return;

	oldstr = m_mag_var_str;

	dir = m_mag_var_str[0];
	if ((dir != 'E') && (dir != 'e') && (dir != 'W') && (dir != 'w'))
	{
		AfxMessageBox("The MagVar must begin with E or W");
		m_mag_var_str = oldstr;
		UpdateData(FALSE);
		return;
	}

	len = m_mag_var_str.GetLength();
	tstr = m_mag_var_str.Right(len-1);
	tf = atof(tstr);
	if ((tf < 0.0) || (tf > 180.0))
	{
		AfxMessageBox("The MagVar must be between 0 and 180 degrees");
		m_mag_var_str = oldstr;
		UpdateData(FALSE);
		return;
	}

	m_mag_var = atof(tstr);
	if ((dir == 'W') || (dir == 'w'))
		m_mag_var = -m_mag_var;

	CUserARPointSheetDlg::m_temp_ar_pt->calc_nav_fix(m_auto_mag_var, m_mag_var);
	m_bearing_str = CUserARPointSheetDlg::m_temp_ar_pt->m_fix.Left(3);

	UpdateData(FALSE);
}

// *****************************************************************
// *****************************************************************

