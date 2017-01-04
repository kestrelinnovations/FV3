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



// aredt_pp.cpp : implementation file
//

#include "stdafx.h"
#include "ar_edit.h"
#include "colordlg.h"
#include "param.h"
#include "factory.h"
#include "aredt_pp.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// CArEditOvlOptionsPage property page

IMPLEMENT_DYNCREATE(CArEditOvlOptionsPage, CPropertyPage)

CArEditOvlOptionsPage::CArEditOvlOptionsPage() : CPropertyPage(CArEditOvlOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CArEditOvlOptionsPage)
	m_background = FALSE;
	m_orbit_width_str = _T("");
   m_display_threshold = _T("");
	//}}AFX_DATA_INIT
}

CArEditOvlOptionsPage::~CArEditOvlOptionsPage()
{
}

void CArEditOvlOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArEditOvlOptionsPage)
	DDX_Check(pDX, IDC_BACKGROUND, m_background);
	DDX_Text(pDX, IDC_ORBIT_WIDTH, m_orbit_width_str);
   DDX_CBString(pDX, IDC_THRESHOLD, m_display_threshold);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArEditOvlOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CArEditOvlOptionsPage)
	ON_BN_CLICKED(IDC_MTR_COLOR, OnArEditColor)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_RB_NIMA, OnRbNima)
	ON_BN_CLICKED(IDC_RB_SOF, OnRbSof)
	ON_CBN_CLOSEUP(IDC_CB_RADIUS, OnCloseupCbRadius)
	ON_EN_KILLFOCUS(IDC_ORBIT_WIDTH, OnKillfocusOrbitWidth)
	ON_BN_CLICKED(IDC_BACKGROUND, OnModified)
	ON_EN_CHANGE(IDC_ORBIT_WIDTH, OnChangeOrbitWidth)
   ON_CBN_SELCHANGE(IDC_THRESHOLD, OnModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArEditOvlOptionsPage message handlers

void CArEditOvlOptionsPage::OnOK() 
{
	UpdateData(TRUE);

	//display threshold
	CString initial_display_threshold = PRM_get_registry_string("ArEdit", "ArEditDisplayAbove", "1:5 M");

	if (m_display_threshold.Compare(initial_display_threshold) != 0)
	{
		PRM_set_registry_string("ArEdit", "ArEditDisplayAbove", m_display_threshold);
//		dirty = TRUE;
	}
	
   const int DATA_LEN = 11;
	char data[DATA_LEN];
	int rslt;
	char radstr[81];

	strcpy_s(data, DATA_LEN, "N");
	
	if (IsDlgButtonChecked(IDC_BACKGROUND))
		strcpy_s(data, DATA_LEN, "Y");
	
	PRM_set_registry_string("ArEdit", "ArEditBackground", data);	 

	CString tmpstr;
	tmpstr.Format("%3d", m_color);
	PRM_set_registry_string("ArEdit", "ArEditDispColor", tmpstr);

	// store the values for the point display options
	if (IsDlgButtonChecked(IDC_RB_SOF))
		PRM_set_registry_string("ArEdit", "PointDisplayType", "SOF");
	else
		PRM_set_registry_string("ArEdit", "PointDisplayType", "NIMA");
	
	rslt = SendDlgItemMessage(IDC_CB_RADIUS, CB_GETCURSEL, 0, 0);
	if (rslt != CB_ERR)
	{
		SendDlgItemMessage(IDC_CB_RADIUS, WM_GETTEXT, 80, (LPARAM) (LPCSTR) radstr);
		PRM_set_registry_string("ArEdit", "PointDisplayRadius", radstr);
	}

	PRM_set_registry_string("ArEdit", "ArEditDefOrbitWidth", m_orbit_width_str);

	SetModified();
	
   OVL_get_overlay_manager()->InvalidateOverlaysOfType(FVWID_Overlay_TrackOrbit);
	
	CPropertyPage::OnOK();
}
// end of OnOK


// ******************************************************************
// ******************************************************************

void CArEditOvlOptionsPage::OnArEditColor() 
{
	CDrawColorDlg dlg;
	int rslt;

	dlg.set_color(m_color);
	dlg.m_no_radio_buttons = TRUE;
	dlg.set_title("Air Refuel Edit Color");
	rslt = dlg.DoModal();
	if (rslt == IDOK)
	{
		m_color = dlg.get_color();
		DrawColorButton();
//		m_dirty = TRUE;
		OnModified();
	}
}

// ******************************************************************
// ******************************************************************

BOOL CArEditOvlOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//display threshold
	m_display_threshold = PRM_get_registry_string("ArEdit", "ArEditDisplayAbove", "1:5 M");

	CString sdata;

	sdata = PRM_get_registry_string("ArEdit", "ArEditBackground", "Y");
	
	if (sdata == "Y")
		m_background = TRUE;
	else
		m_background = FALSE;
 
	sdata = PRM_get_registry_string("ArEdit", "ArEditDispColor", "249");
	m_color = atoi(sdata);
	m_orbit_width_str = PRM_get_registry_string("ArEdit", "ArEditDefOrbitWidth", "20");

	DrawColorButton();
	
	// init the point display options
	CString value;
   const int RADSTR_LEN = 81;
	char radstr[RADSTR_LEN];
	int rslt;
	BOOL disp_sof = FALSE;

	value = PRM_get_registry_string("ArEdit", "PointDisplayType", "NIMA");
	if (!value.Compare("NIMA"))
	{
		CheckRadioButton(IDC_RB_NIMA, IDC_RB_SOF, IDC_RB_NIMA);
	}
	else
	{
		CheckRadioButton(IDC_RB_NIMA, IDC_RB_SOF, IDC_RB_SOF);
		disp_sof = TRUE;
	}

	value = PRM_get_registry_string("ArEdit", "PointDisplayRadius", "20");
	strcpy_s(radstr, RADSTR_LEN, value);
	rslt = SendDlgItemMessage(IDC_CB_RADIUS, CB_FINDSTRINGEXACT, -1, (LPARAM) radstr);
	if (rslt != CB_ERR)
		SendDlgItemMessage(IDC_CB_RADIUS, CB_SETCURSEL, rslt, (LPARAM) 0);

	if (!disp_sof)
	{
		GetDlgItem(IDC_CB_RADIUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_CB_RADIUS_TEXT)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// end of OnInitDialog

// ******************************************************************
// ******************************************************************

void CArEditOvlOptionsPage::DrawColorButton()
{
	CDrawColorDlg::draw_color_button(this, IDC_MTR_COLOR, m_color);
}
// end of DrawColorButton
  
// ******************************************************************
// ******************************************************************

void CArEditOvlOptionsPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   switch (nIDCtl) 
   {
		case IDC_MTR_COLOR:
			DrawColorButton();
			break;
	}
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// ******************************************************************
// ******************************************************************

void CArEditOvlOptionsPage::OnBackground() 
{
	// TODO: Add your control notification handler code here
	
}

// ******************************************************************
// ******************************************************************


void CArEditOvlOptionsPage::OnRbNima() 
{
	GetDlgItem(IDC_CB_RADIUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_CB_RADIUS_TEXT)->EnableWindow(FALSE);
	OnModified();
}

// ******************************************************************
// ******************************************************************

void CArEditOvlOptionsPage::OnRbSof() 
{
	GetDlgItem(IDC_CB_RADIUS)->EnableWindow(TRUE);
	GetDlgItem(IDC_CB_RADIUS_TEXT)->EnableWindow(TRUE);
	OnModified();
}

// ******************************************************************
// ******************************************************************

void CArEditOvlOptionsPage::OnCloseupCbRadius() 
{
	OnModified();
}
// ******************************************************************
// ******************************************************************

void CArEditOvlOptionsPage::OnKillfocusOrbitWidth() 
{
	CString oldwidth;
	double width;

	oldwidth = m_orbit_width_str;

	UpdateData(TRUE);
	width = atof(m_orbit_width_str);
	if ((width < 1.0) || (width > 300.0))
	{
		m_orbit_width_str = oldwidth;
		AfxMessageBox("The width must be between 1 and 300 NM");
		UpdateData(FALSE);
	}
}

// ******************************************************************
// ******************************************************************

void CArEditOvlOptionsPage::OnChangeOrbitWidth() 
{
	OnModified();
}

void CArEditOvlOptionsPage::OnModified()
{
   SetModified(TRUE);
}                                     

// ******************************************************************
// ******************************************************************
// ******************************************************************
// ******************************************************************

