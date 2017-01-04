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

// SkyViewOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "SkyViewOptionsPage.h"

#include "mov_sym.h"
#include "param.h"
#include "..\FontDlg.h"
#include "gps.h"
#include "ovl_mgr.h"

SkyViewProperties::SkyViewProperties()
{
	m_symbol_type = "Helicopter";

	m_show_loaded_area = TRUE;
	m_loaded_area_color = RGB(255, 255, 0);

	m_show_visible_area = TRUE;
	m_visible_area_color = RGB(255,255,255);

	m_hide_vehicle_when_attached = FALSE;
	m_show_altitude = TRUE;
}

int SkyViewProperties::initialize_from_registry()
{
	const CString key("SkyViewOverlay");

	m_symbol_type = PRM_get_registry_string(key, "SymbolType", "Helicopter");
	
	m_show_loaded_area = PRM_get_registry_int(key, "ShowLoadedArea", TRUE);
	m_loaded_area_color = (COLORREF)PRM_get_registry_int(key, "LoadedAreaColor", RGB(255,255,0));
	
	m_show_visible_area = PRM_get_registry_int(key, "ShowVisibleArea", TRUE);
	m_visible_area_color = (COLORREF)PRM_get_registry_int(key, "VisibleAreaColor", RGB(255,255,255));
	
	m_hide_vehicle_when_attached = PRM_get_registry_int(key, "HideWhenAttached", FALSE);
	m_show_altitude = PRM_get_registry_int(key, "ShowAltitude", TRUE);

	m_altitude_font.initialize_from_registry("SkyViewOverlay", 
		"Arial",12,0,UTIL_COLOR_WHITE, UTIL_BG_SHADOW, UTIL_COLOR_BLACK);

	return SUCCESS;
}

int SkyViewProperties::save_to_registry()
{
	const CString key("SkyViewOverlay");

	PRM_set_registry_string(key, "SymbolType", m_symbol_type);
		
	PRM_set_registry_int(key, "ShowLoadedArea", m_show_loaded_area);
	PRM_set_registry_int(key, "LoadedAreaColor", m_loaded_area_color);
	
	PRM_set_registry_int(key, "ShowVisibleArea", m_show_visible_area);
	PRM_set_registry_int(key, "VisibleAreaColor", m_visible_area_color);
	
	PRM_set_registry_int(key, "HideWhenAttached", m_hide_vehicle_when_attached);
	PRM_set_registry_int(key, "ShowAltitude", m_show_altitude);

	m_altitude_font.save_in_registry(key);

	return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// CSkyViewOptionsPage property page

IMPLEMENT_DYNCREATE(CSkyViewOptionsPage, CPropertyPage)

CSkyViewOptionsPage::CSkyViewOptionsPage() : CPropertyPage(CSkyViewOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CSkyViewOptionsPage)
	//}}AFX_DATA_INIT
}

CSkyViewOptionsPage::~CSkyViewOptionsPage()
{
}

void CSkyViewOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkyViewOptionsPage)
	DDX_Control(pDX, IDC_SHIP_TYPE, m_ship_type_combo);
	DDX_Control(pDX, IDC_LOADED_AREA, m_loaded_area_check);
	DDX_Control(pDX, IDC_LOADED_AREA_COLOR, m_loaded_area_color);
	DDX_Control(pDX, IDC_VISIBLE_AREA, m_visible_area_check);
	DDX_Control(pDX, IDC_VISIBLE_AREA_COLOR, m_visible_area_color);
	DDX_Control(pDX, IDC_HIDE_WHEN_ATTACHED, m_hide_when_attached_check);
	DDX_Control(pDX, IDC_ALTITUDE, m_altitude_check);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSkyViewOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSkyViewOptionsPage)
	ON_CBN_SELCHANGE(IDC_SHIP_TYPE, OnModified)

	ON_BN_CLICKED(IDC_LOADED_AREA, OnModified)
	ON_BN_CLICKED(IDC_LOADED_AREA_COLOR, OnLoadedAreaColor)

	ON_BN_CLICKED(IDC_VISIBLE_AREA, OnModified)
	ON_BN_CLICKED(IDC_VISIBLE_AREA_COLOR, OnVisibleAreaColor)

	ON_BN_CLICKED(IDC_HIDE_WHEN_ATTACHED, OnModified)
	ON_BN_CLICKED(IDC_ALTITUDE, OnModified)

	ON_BN_CLICKED(IDC_FONT, OnFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkyViewOptionsPage message handlers

BOOL CSkyViewOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SkyViewProperties *prop = Cmov_sym_overlay::GetStateIndicators()->m_properties;

	// populate the ship type combo box with the symbols read in at startup
   const int nNumSymbols = SymbolContainer::GetInstance()->get_num_symbols();
	for(int i=0;i<nNumSymbols;i++)
		m_ship_type_combo.AddString(SymbolContainer::GetInstance()->get_symbol(i)->get_name());

	// select the ship type string based on the value store in the properties.
	// if the symbol type was not found then select the first entry in the combo
	if (m_ship_type_combo.SelectString(0, prop->m_symbol_type) == CB_ERR)
		m_ship_type_combo.SetCurSel(0); 

	m_loaded_area_check.SetCheck(prop->m_show_loaded_area);
	m_loaded_area_color.SetColor(prop->m_loaded_area_color);
	
	m_visible_area_check.SetCheck(prop->m_show_visible_area);
	m_visible_area_color.SetColor(prop->m_visible_area_color);

	m_hide_when_attached_check.SetCheck(prop->m_hide_vehicle_when_attached);
	m_altitude_check.SetCheck(prop->m_show_altitude);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSkyViewOptionsPage::OnModified()
{
	SetModified();
}

void CSkyViewOptionsPage::OnLoadedAreaColor() 
{
	CColorDialog dlg(m_loaded_area_color.GetColor());

	if (dlg.DoModal())
		m_loaded_area_color.SetColor(dlg.GetColor());
	OnModified();
}

void CSkyViewOptionsPage::OnVisibleAreaColor() 
{
	CColorDialog dlg(m_visible_area_color.GetColor());

	if (dlg.DoModal())
		m_visible_area_color.SetColor(dlg.GetColor());
	OnModified();
}

void CSkyViewOptionsPage::OnOK()
{
	SkyViewProperties *prop = Cmov_sym_overlay::GetStateIndicators()->m_properties;

	prop->m_show_loaded_area = m_loaded_area_check.GetCheck();
	prop->m_loaded_area_color = m_loaded_area_color.GetColor();

	prop->m_show_visible_area = m_visible_area_check.GetCheck();
	prop->m_visible_area_color = m_visible_area_color.GetColor();

	prop->m_hide_vehicle_when_attached = m_hide_when_attached_check.GetCheck();
	prop->m_show_altitude = m_altitude_check.GetCheck();

	if (Cmov_sym_overlay::GetStateIndicators()->m_symbol)
	{
		double lat = Cmov_sym_overlay::GetStateIndicators()->m_symbol->get_latitude();
		double lon = Cmov_sym_overlay::GetStateIndicators()->m_symbol->get_longitude();
		double heading = Cmov_sym_overlay::GetStateIndicators()->m_symbol->get_heading();
		double alt = Cmov_sym_overlay::GetStateIndicators()->m_symbol->get_msl();
		Cmov_sym_overlay::GetStateIndicators()->update_altitude(lat, lon, heading, alt);
	}

	m_ship_type_combo.GetLBText(m_ship_type_combo.GetCurSel(), prop->m_symbol_type);
	Cmov_sym_overlay::GetStateIndicators()->redefine_symbol();

	prop->save_to_registry();

	OVL_get_overlay_manager()->invalidate_all();
}

void CSkyViewOptionsPage::OnFont()
{
	SkyViewProperties *prop = Cmov_sym_overlay::GetStateIndicators()->m_properties;

	CFontDlg dlg;
	dlg.set_apply_callback(&on_apply_font);
	dlg.set_font(prop->m_altitude_font);
	
	if (dlg.DoModal() == IDOK) 
	{
		prop->m_altitude_font = dlg;
		SetModified();
	}
}

void CSkyViewOptionsPage::on_apply_font(CFontDlg *dlg)
{
	SkyViewProperties *prop = Cmov_sym_overlay::GetStateIndicators()->m_properties;
	prop->m_altitude_font = *dlg;

	if (Cmov_sym_overlay::GetStateIndicators()->m_symbol)
	{
		double lat = Cmov_sym_overlay::GetStateIndicators()->m_symbol->get_latitude();
		double lon = Cmov_sym_overlay::GetStateIndicators()->m_symbol->get_longitude();
		double heading = Cmov_sym_overlay::GetStateIndicators()->m_symbol->get_heading();
		double alt = Cmov_sym_overlay::GetStateIndicators()->m_symbol->get_msl();
		Cmov_sym_overlay::GetStateIndicators()->update_altitude(lat, lon, heading, alt);
	}

   OVL_get_overlay_manager()->invalidate_all();
}
