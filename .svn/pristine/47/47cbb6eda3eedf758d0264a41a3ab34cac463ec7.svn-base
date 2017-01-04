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



// scaledlg.cpp : implementation file
//

#include "stdafx.h"
#include "colordlg.h"
#include "scalebar.h"
#include "scaledlg.h"
#include "param.h"
#include "fvwutil.h"
#include "factory.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// CScaleBarOptionsPage property page

IMPLEMENT_DYNCREATE(CScaleBarOptionsPage, CPropertyPage)

CScaleBarOptionsPage::CScaleBarOptionsPage()
	: CPropertyPage(CScaleBarOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CScaleBarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CScaleBarOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScaleBarOptionsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_ON, m_scalebar_on);
   DDX_Control(pDX, IDC_SCALEBAR_VERTICAL, m_vert);
	DDX_Control(pDX, IDC_SCALEBAR_HORIZONTAL, m_horz);
	DDX_Control(pDX, IDC_SCALEBAR_BOTH, m_both);
	DDX_Control(pDX, IDC_RB_SIZE_SMALL, m_size_small);
	DDX_Control(pDX, IDC_RB_SIZE_LARGE, m_size_large);
	DDX_Control(pDX, IDC_RB_UNITS_NM_YD, m_units_NM_YD);
	DDX_Control(pDX, IDC_RB_UNITS_NM_FT, m_units_NM_FT);
	DDX_Control(pDX, IDC_RB_UNITS_KM_M, m_units_KM_M);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScaleBarOptionsPage, CDialog)
	//{{AFX_MSG_MAP(CScaleBarOptionsPage)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_ON, OnModified)
	ON_BN_CLICKED(IDC_SCALEBAR_VERTICAL, OnModified)
	ON_BN_CLICKED(IDC_SCALEBAR_HORIZONTAL, OnModified)
	ON_BN_CLICKED(IDC_SCALEBAR_BOTH, OnModified)
	ON_BN_CLICKED(IDC_RB_SIZE_SMALL, OnModified)
	ON_BN_CLICKED(IDC_RB_SIZE_LARGE, OnModified)
	ON_BN_CLICKED(IDC_RB_UNITS_NM_YD, OnModified)
	ON_BN_CLICKED(IDC_RB_UNITS_NM_FT, OnModified)
	ON_BN_CLICKED(IDC_RB_UNITS_KM_M, OnModified)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_BN_CLICKED(IDC_COLOR_BACK, OnColorBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleBarOptionsPage message handlers

BOOL CScaleBarOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ScaleBar) != NULL)
		m_scalebar_on.SetCheck(1);

	CString tstr;
	CFvwUtil *util = CFvwUtil::get_instance();

	int color;
   tstr = PRM_get_registry_string("SCALEBAR", "Orientation", "Vertical");
	m_orientation = SCALEBAR_HORZ;
	if (tstr == "Vertical")
		m_orientation = SCALEBAR_VERT;
	if (tstr == "Both")
		m_orientation = SCALEBAR_BOTH;

   tstr = PRM_get_registry_string("SCALEBAR", "FontSize", "Large");
	if (tstr == "Large")
		m_size = SCALEBAR_SIZE_LARGE;
	else
		m_size = SCALEBAR_SIZE_SMALL;

   tstr = PRM_get_registry_string("SCALEBAR", "Units", "NM_YD");
	if (tstr == "NM_YD")
		m_units = SCALEBAR_UNITS_NM_YD;
	if (tstr == "NM_FT")
		m_units = SCALEBAR_UNITS_NM_FT;
	if (tstr == "KM_M")
		m_units = SCALEBAR_UNITS_KM_M;

   tstr = PRM_get_registry_string("SCALEBAR", "Color", "0");
	color = atoi(tstr.GetBuffer(3));
	if (util->is_valid_color(color))
		m_color = color;
   tstr = PRM_get_registry_string("SCALEBAR", "BackColor", "7");
	color = atoi(tstr.GetBuffer(3));
	if (util->is_valid_color(color))
		m_backcolor = color;

   m_vert.SetCheck(m_orientation == SCALEBAR_VERT);
   m_horz.SetCheck(m_orientation == SCALEBAR_HORZ);
   m_both.SetCheck(m_orientation == SCALEBAR_BOTH);

   m_size_small.SetCheck(m_size == SCALEBAR_SIZE_SMALL);
	m_size_large.SetCheck(m_size == SCALEBAR_SIZE_LARGE);

   m_units_NM_YD.SetCheck(m_units == SCALEBAR_UNITS_NM_YD);
	m_units_NM_FT.SetCheck(m_units == SCALEBAR_UNITS_NM_FT);
	m_units_KM_M.SetCheck(m_units == SCALEBAR_UNITS_KM_M);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// *************************************************************************
// *************************************************************************

void CScaleBarOptionsPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   switch (nIDCtl) 
   {
		case IDC_COLOR:
			DrawColorButton();
			break;
		case IDC_COLOR_BACK:
			DrawBackColorButton();
			break;
	}
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// *************************************************************************
// *************************************************************************

void CScaleBarOptionsPage::OnColor() 
{
	CDrawColorDlg dlg;
	int rslt;

	dlg.set_color(m_color);
	dlg.m_no_radio_buttons = TRUE;
	rslt = dlg.DoModal();
	if (rslt == IDOK)
		m_color = dlg.get_color();
	DrawColorButton();

	SetModified();
}

// *************************************************************************
// *************************************************************************

void CScaleBarOptionsPage::OnColorBack() 
{
	CDrawColorDlg dlg;
	int rslt;

	dlg.set_color(m_backcolor);
	dlg.m_no_radio_buttons = TRUE;
	dlg.set_title("BackColor");
	rslt = dlg.DoModal();
	if (rslt == IDOK)
		m_backcolor = dlg.get_color();
	DrawBackColorButton();

	SetModified();
}

// *************************************************************************
// *************************************************************************

void CScaleBarOptionsPage::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CPropertyPage::OnCancel();
}

// *************************************************************************
// *************************************************************************

void CScaleBarOptionsPage::OnOK() 
{
   if (m_vert.GetCheck())
      m_orientation = SCALEBAR_VERT;
   else if (m_horz.GetCheck())
      m_orientation = SCALEBAR_HORZ;
   else
      m_orientation = SCALEBAR_BOTH;

   if (m_size_small.GetCheck())
      m_size = SCALEBAR_SIZE_SMALL;
   else
      m_size = SCALEBAR_SIZE_LARGE;

   if (m_units_NM_YD.GetCheck())
      m_units = SCALEBAR_UNITS_NM_YD;
   else if (m_units_NM_FT.GetCheck())
	   m_units = SCALEBAR_UNITS_NM_FT;
   else
      m_units = SCALEBAR_UNITS_KM_M;
	
	// store params in registry
	CString tstr;
	
	if (m_orientation == SCALEBAR_VERT)
		PRM_set_registry_string("SCALEBAR", "Orientation", "Vertical");
	if (m_orientation == SCALEBAR_HORZ)
		PRM_set_registry_string("SCALEBAR", "Orientation", "Horizontal");
	if (m_orientation == SCALEBAR_BOTH)
		PRM_set_registry_string("SCALEBAR", "Orientation", "Both");
	
	if (m_size == SCALEBAR_SIZE_LARGE)
		PRM_set_registry_string("SCALEBAR", "FontSize", "Large");
	else
		PRM_set_registry_string("SCALEBAR", "FontSize", "Small");
	
	switch(m_units)
	{
	case SCALEBAR_UNITS_NM_YD:
		PRM_set_registry_string("SCALEBAR", "Units", "NM_YD");
		break;
	case SCALEBAR_UNITS_NM_FT:
		PRM_set_registry_string("SCALEBAR", "Units", "NM_FT");
		break;
	case SCALEBAR_UNITS_KM_M:
		PRM_set_registry_string("SCALEBAR", "Units", "KM_M");
		break;
	}
	tstr.Format("%3d", m_color);
	PRM_set_registry_string("SCALEBAR", "Color", tstr);
	tstr.Format("%3d", m_backcolor);
	PRM_set_registry_string("SCALEBAR", "BackColor", tstr);

	// toggle overlay if necessary
	C_overlay *overlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ScaleBar);
	if ((overlay == NULL && m_scalebar_on.GetCheck()) ||
		 (overlay != NULL && !m_scalebar_on.GetCheck()))
		OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_ScaleBar);
	else if (overlay)
	{
		((CScaleBarOverlay *)overlay)->reload_settings();
		OVL_get_overlay_manager()->InvalidateOverlay(overlay);
	}
	
	CPropertyPage::OnOK();
}
// end of OnOK

// **************************************************************************

void CScaleBarOptionsPage::DrawColorButton()
{
	CDrawColorDlg::draw_color_button(this, IDC_COLOR, m_color);
}
// end of DrawColorButton
  
// **************************************************************************
// **************************************************************************

void CScaleBarOptionsPage::DrawBackColorButton()
{
	CDrawColorDlg::draw_color_button(this, IDC_COLOR_BACK, m_backcolor);
}
// end of DrawBackColorButton

void CScaleBarOptionsPage::OnModified()
{
	SetModified();
}
