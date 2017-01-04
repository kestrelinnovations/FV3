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



//
// Implementation file:	overlay_pp.cpp
//

// Purpose:					implementation of the overlay options
//								property page base class.

#include "stdafx.h"

#include "..\resource.h"
#include "overlay_pp.h"
#include "OvlFctry.h"
#include "ovl_mgr.h"


// **************************************************************************

IMPLEMENT_DYNCREATE(COverlayPropertyPage, CPropertyPage)

// **************************************************************************

COverlayPropertyPage::COverlayPropertyPage(GUID overlayDescGuid, UINT pp_id) :
   CPropertyPage(pp_id),
   m_overlayDescGuid(overlayDescGuid)
{
	//{{AFX_DATA_INIT(COverlayPropertyPage)
	m_overlay_toggle = FALSE;
	m_label_threshold = _T("");
	m_display_threshold = _T("");
	//}}AFX_DATA_INIT
}

// **************************************************************************

COverlayPropertyPage::~COverlayPropertyPage()
{
}

// **************************************************************************

void COverlayPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COverlayPropertyPage)
	DDX_Control(pDX, IDC_LABEL_THRESHOLD, m_display_threshold_control);
	DDX_Control(pDX, IDC_THRESHOLD, m_threshold_control);
	DDX_CBString(pDX, IDC_LABEL_THRESHOLD, m_label_threshold);
	DDX_CBString(pDX, IDC_THRESHOLD, m_display_threshold);
	DDX_Check(pDX, IDC_TOGGLE, m_overlay_toggle);
	//}}AFX_DATA_MAP
}


// **************************************************************************

BEGIN_MESSAGE_MAP(COverlayPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(COverlayPropertyPage)
	ON_BN_CLICKED(IDC_TOGGLE, OnToggleOverlay)
	ON_CBN_SELCHANGE(IDC_LABEL_THRESHOLD, OnModified)
	ON_CBN_SELCHANGE(IDC_THRESHOLD, OnModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// **************************************************************************

BOOL COverlayPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// set the intial state of all of the controls based upon overlay toggle state
	if (OVL_get_overlay_manager()->get_first_of_type(m_overlayDescGuid))
		m_overlay_toggle = TRUE;
	else
		m_overlay_toggle = FALSE;

	// enable/disable based upon state of overlay
	toggle_control_states(TRUE);

   UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// **************************************************************************

void COverlayPropertyPage::OnToggleOverlay() 
{
	UpdateData(TRUE);
	SetModified(TRUE);	

	return;
}


// **************************************************************************

void COverlayPropertyPage::toggle_control_states(boolean_t turn_on) 
{
	m_threshold_control.EnableWindow(turn_on);
	m_display_threshold_control.EnableWindow(turn_on);
	return;
}


// **************************************************************************

void COverlayPropertyPage::OnModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);	
}


// **************************************************************************


void COverlayPropertyPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	C_ovl_mgr*	ovlmgr=OVL_get_overlay_manager(); 
	C_overlay*	ovl;

	ovl = ovlmgr->get_first_of_type(m_overlayDescGuid);

	// toggle the overlay if we need to to get its state the same as the checkbox
	boolean_t overlay_on = ovl != NULL;

	if (m_overlay_toggle != overlay_on)
	   ovlmgr->toggle_static_overlay(m_overlayDescGuid);

	ovl = ovlmgr->get_first_of_type(m_overlayDescGuid);
	if (ovl)
		m_overlay_toggle = TRUE;
	else
		m_overlay_toggle = FALSE;
	
	CPropertyPage::OnOK();
}
