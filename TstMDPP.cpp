// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



// TstMDPP.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TstMDPP.h"
#include "MapTpSel.h"
#include "catalog/CustomTreeControl.h"

/////////////////////////////////////////////////////////////////////////////
// CTestMapDataPP property page

IMPLEMENT_DYNCREATE(CTestMapDataPP, CPropertyPage)

CTestMapDataPP::CTestMapDataPP() : CPropertyPage(CTestMapDataPP::IDD)
{
   m_pMapTypeSelectionTree = new MapTypeSelectionTree(m_tree_ctrl);
}

CTestMapDataPP::~CTestMapDataPP()
{
   delete m_pMapTypeSelectionTree;
}

void CTestMapDataPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTestMapDataPP)
   DDX_Control(pDX, IDC_TREE1, m_tree_ctrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestMapDataPP, CPropertyPage)
   //{{AFX_MSG_MAP(CTestMapDataPP)
   ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
   ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestMapDataPP message handlers

BOOL CTestMapDataPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_pMapTypeSelectionTree->on_initialize(FALSE, TRUE);

   return TRUE;
}

void CTestMapDataPP::OnSelectall() 
{
   m_pMapTypeSelectionTree->on_initialize(TRUE, TRUE);
}

void CTestMapDataPP::OnClearall() 
{
   m_pMapTypeSelectionTree->on_initialize(FALSE, TRUE);
}

void CTestMapDataPP::OnOK()
{
   m_pMapTypeSelectionTree->get_selected_list(m_selectedMapTypes);
}