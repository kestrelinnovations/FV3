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



// mdmtypes.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "cov_ovl.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMTypes property page

IMPLEMENT_DYNCREATE(CMDMTypes, CPropertyPage)

CMDMTypes::CMDMTypes() : CPropertyPage(CMDMTypes::IDD)
{
	//{{AFX_DATA_INIT(CMDMTypes)
	//}}AFX_DATA_INIT
}

CMDMTypes::~CMDMTypes()
{
}

void CMDMTypes::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDMTypes)
	DDX_Control(pDX, IDC_HELP_MDM_TYPES, m_HelpBtnCtrl);
	DDX_Control(pDX, IDC_MDMTYPES_USELIST, m_UseListBoxCtrl);
	DDX_Control(pDX, IDC_MDMTYPES_AVAILLIST, m_AvailListBoxCtrl);
	DDX_Control(pDX, ID_MDMTYPES_REMOVEONE, m_RemoveOneBtnCtrl);
	DDX_Control(pDX, ID_MDMTYPES_REMOVEALL, m_RemoveAllBtnCtrl);
	DDX_Control(pDX, ID_MDMTYPES_COPYONE, m_CopyOneBtnCtrl);
	DDX_Control(pDX, ID_MDMTYPES_COPYALL, m_CopyAllBtnCtrl);
	DDX_Control(pDX, ID_MDMTYPES_CANCEL, m_CancelBtnCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMTypes, CPropertyPage)
	//{{AFX_MSG_MAP(CMDMTypes)
	ON_BN_CLICKED(ID_MDMTYPES_CANCEL, OnMdmtypesCancel)
	ON_BN_CLICKED(ID_MDMTYPES_COPYALL, OnMdmtypesCopyall)
	ON_BN_CLICKED(ID_MDMTYPES_COPYONE, OnMdmtypesCopyone)
	ON_BN_CLICKED(ID_MDMTYPES_REMOVEALL, OnMdmtypesRemoveall)
	ON_BN_CLICKED(ID_MDMTYPES_REMOVEONE, OnMdmtypesRemoveone)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_HELP_MDM_TYPES, OnHelpMdmTypes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMTypes message handlers

void CMDMTypes::OnMdmtypesCancel() 
{
	// TODO: Add your control notification handler code here
	
}

void CMDMTypes::OnMdmtypesCopyall() 
{
	// TODO: Add your control notification handler code here
	
}

void CMDMTypes::OnMdmtypesCopyone() 
{
	// TODO: Add your control notification handler code here
	
}

void CMDMTypes::OnMdmtypesRemoveall() 
{
	// TODO: Add your control notification handler code here
	
}

void CMDMTypes::OnMdmtypesRemoveone() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CMDMTypes::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CoverageOverlay *overlay = cat_get_coverage_overlay();
	if (!overlay)
		return FALSE;	// needed cause OnInitDialog gets called when removing pages!
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMDMTypes::GetMinimumRect(CRect* pRect)
{
   //m_MinSizeBoxCtrl.GetClientRect(pRect);
}

BOOL CMDMTypes::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnSetActive();
}

void CMDMTypes::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
}

void CMDMTypes::OnHelpMdmTypes() 
{
	SendMessage(WM_COMMAND, ID_HELP, 0);
}
