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

// PredictivePathOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "PredictivePathOptionsPage.h"
#include "BullseyeOptionsPage.h"
#include "..\multisel.h"
#include "gps.h"
#include "optndlg.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// CPredictivePathOptionsPage property page

IMPLEMENT_DYNCREATE(CPredictivePathOptionsPage, CPropertyPage)

CPredictivePathOptionsPage::CPredictivePathOptionsPage(CPredictivePathProperties* pProperties /*= NULL */, 
                                                       C_overlay* pOverlay /*= NULL*/) : 
	CPropertyPage(CPredictivePathOptionsPage::IDD),
   m_prop(pProperties),
   m_pOverlay(pOverlay),
   m_bApplyImmediately(false)
{
	//{{AFX_DATA_INIT(CPredictivePathOptionsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
		
   m_modified = FALSE;
}

CPredictivePathOptionsPage::~CPredictivePathOptionsPage()
{
}

void CPredictivePathOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPredictivePathOptionsPage)
	DDX_Control(pDX, IDC_ON, m_on_ctrl);
	DDX_Control(pDX, IDC_PREDICTIVE_PATH_CENTER_LINE, m_center_line_ctrl);
	DDX_Control(pDX, IDC_PREDICTIVE_ICON_COLOR, m_icon_fgC);
	DDX_Control(pDX, IDC_SLIDER_DOT1, m_slider_ctrl[0]);
	DDX_Control(pDX, IDC_SLIDER_DOT2, m_slider_ctrl[1]);
	DDX_Control(pDX, IDC_SLIDER_DOT3, m_slider_ctrl[2]);
	DDX_Control(pDX, IDC_SLIDER_DOT4, m_slider_ctrl[3]);
	
	DDX_Control(pDX, IDC_EDIT1, m_edit_ctrl[0]);
	DDX_Text(pDX, IDC_EDIT1, m_edit_value[0]);

	DDX_Control(pDX, IDC_EDIT2, m_edit_ctrl[1]);
	DDX_Text(pDX, IDC_EDIT2, m_edit_value[1]);

	DDX_Control(pDX, IDC_EDIT3, m_edit_ctrl[2]);
	DDX_Text(pDX, IDC_EDIT3, m_edit_value[2]);

	DDX_Control(pDX, IDC_EDIT4, m_edit_ctrl[3]);
	DDX_Text(pDX, IDC_EDIT4, m_edit_value[3]);

	DDX_Control(pDX, IDC_NUM_DOTS, m_num_dots_ctrl);
	DDX_Control(pDX, IDC_UNITS1, m_units_ctrl[0]);
	DDX_Control(pDX, IDC_UNITS2, m_units_ctrl[1]);
	DDX_Control(pDX, IDC_UNITS3, m_units_ctrl[2]);
	DDX_Control(pDX, IDC_UNITS4, m_units_ctrl[3]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPredictivePathOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPredictivePathOptionsPage)
	ON_BN_CLICKED(IDC_ON, OnModified)
	ON_BN_CLICKED(IDC_PREDICTIVE_PATH_CENTER_LINE, OnModified)
	ON_BN_CLICKED(IDC_PREDICTIVE_ICON_COLOR, OnPredictiveIconColor)
	ON_CBN_SELCHANGE(IDC_NUM_DOTS, OnNumDotsChanged)
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT2, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT3, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT4, OnKillfocusEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPredictivePathOptionsPage message handlers

BOOL CPredictivePathOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
   InitializeControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPredictivePathOptionsPage::InitializeControls()
{
   CFvwUtil *futil = CFvwUtil::get_instance();

	m_on_ctrl.SetCheck(m_prop->get_predictive_path_on());
	m_center_line_ctrl.SetCheck(m_prop->get_predictive_path_center_line_on());

	m_num_dots_ctrl.SetCurSel(m_prop->get_num_trend_dots() - 1);
	for (int i=0;i<4;++i)
	{
		m_slider_ctrl[i].SetRange(1, 120);
		m_slider_ctrl[i].SetPos(m_prop->get_trend_dot_time(i));
		m_edit_value[i] = m_prop->get_trend_dot_time(i);
	}

	m_icon_fg = futil->code2color(m_prop->get_predictive_path_icon_color());
   m_icon_fgC.SetColor(m_icon_fg);

	hide_trend_dot_controls();

	UpdateData(FALSE);
}

BOOL CPredictivePathOptionsPage::OnApply() 
{
   if (!m_modified)
      return TRUE;

   store_values();

	// if the property page applies to a specific moving map trail then apply the properties now
   if (m_pOverlay != NULL)
   {
      m_pOverlay->set_modified(TRUE);
      OVL_get_overlay_manager()->InvalidateOverlay(m_pOverlay);
   }

   m_modified = FALSE;

	return TRUE;
}

void CPredictivePathOptionsPage::store_values()
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   if (m_prop != NULL)
   {
      m_prop->set_predictive_path_on(m_on_ctrl.GetCheck());
      m_prop->set_predictive_path_center_line_on(m_center_line_ctrl.GetCheck());
      m_prop->set_predictive_path_icon_color(futil->color2code(m_icon_fg));
      m_prop->set_num_trend_dots(m_num_dots_ctrl.GetCurSel() + 1);
      for(int i=0;i<4;++i)
         m_prop->set_trend_dot_time(i, m_slider_ctrl[i].GetPos());
   }
}

void CPredictivePathOptionsPage::OnModified() 
{
	m_modified = TRUE;
	SetModified(TRUE);

   if (m_bApplyImmediately)
      OnApply();
}

void CPredictivePathOptionsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnModified();

	check_slider_positions();

	for (int i=0;i<4;++i)
		m_edit_value[i] = m_slider_ctrl[i].GetPos();
	UpdateData(FALSE);

   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPredictivePathOptionsPage::OnOK() 
{
   OnApply();

	CPropertyPage::OnOK();
}


void CPredictivePathOptionsPage::OnPredictiveIconColor() 
{
	GetColor(m_icon_fg, "Predictive Point Color");
   m_icon_fgC.SetColor(m_icon_fg);
   OnModified();
}

void CPredictivePathOptionsPage::OnNumDotsChanged()
{
	hide_trend_dot_controls();
	check_slider_positions();
   UpdateData(FALSE);
	OnModified();
}

void CPredictivePathOptionsPage::hide_trend_dot_controls()
{
	int num = m_num_dots_ctrl.GetCurSel();
	for(int i=0;i<4;++i)
	{
		m_slider_ctrl[i].ShowWindow(i <= num ? SW_SHOW : SW_HIDE);
		m_edit_ctrl[i].ShowWindow(i <= num ? SW_SHOW : SW_HIDE);
		m_units_ctrl[i].ShowWindow(i <= num ? SW_SHOW : SW_HIDE);
	}
}

void CPredictivePathOptionsPage::check_slider_positions()
{
	int num = m_num_dots_ctrl.GetCurSel();

	for(int i=0;i<num;++i)
	{
		if (m_slider_ctrl[i].GetPos() > m_slider_ctrl[i+1].GetPos())
		{
			int value = max(1, m_slider_ctrl[i+1].GetPos() - 1);
			m_slider_ctrl[i].SetPos(value);
			m_edit_value[i] = value; 
			i = -1;	// restart loop
		}
	}
}

void CPredictivePathOptionsPage::OnKillfocusEdit()
{
	UpdateData();
	for(int i=0;i<4;i++)
	{
		m_edit_value[i] = min(120, max(1, m_edit_value[i]));
		m_slider_ctrl[i].SetPos(m_edit_value[i]);
	}
	check_slider_positions();
	UpdateData(FALSE);
	OnModified();
}

BOOL CMovingMapPredictivePathOptionsPage::OnApply()
{
   if (CPredictivePathOptionsPage::OnApply() == TRUE)
   {
      C_gps_trail *pMovingMapOverlay = dynamic_cast<C_gps_trail *>(m_pOverlay);
      if (pMovingMapOverlay != NULL)
         pMovingMapOverlay->ComputePredictivePath();

      return TRUE;
   }

   return FALSE;
}