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

// ShapeGoToPropPageDisabled.cpp : implementation file
//

#include "stdafx.h"
#include "fvw.h"
#include "ShapeGoToPropPageDisabled.h"
#include "shp.h"
#include "utils.h"
#include "param.h"
#include "getobjpr.h"

/////////////////////////////////////////////////////////////////////////////
// CShapeGoToPropPageDisabled property page

IMPLEMENT_DYNCREATE(CShapeGoToPropPageDisabled, CPropertyPage)

CShapeGoToPropPageDisabled::CShapeGoToPropPageDisabled() : CPropertyPage(CShapeGoToPropPageDisabled::IDD)
{
	//{{AFX_DATA_INIT(CShapeGoToPropPageDisabled)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CShapeGoToPropPageDisabled::~CShapeGoToPropPageDisabled()
{
}

void CShapeGoToPropPageDisabled::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShapeGoToPropPageDisabled)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShapeGoToPropPageDisabled, CPropertyPage)
	//{{AFX_MSG_MAP(CShapeGoToPropPageDisabled)
		// NOTE: the ClassWizard will add message map macros here
		ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CShapeGoToPropPageDisabled message handlers


void CShapeGoToPropPageDisabled::OnOK() 
{
	// set the registry value to keep track of the last active page...
	CPropertySheet *pPropSheet = reinterpret_cast<CPropertySheet *>(GetParent());
	if (pPropSheet)
		PRM_set_registry_int("GoToPropSheet", "LastActivePageIndex", pPropSheet->GetActiveIndex());
}


void CShapeGoToPropPageDisabled::OnHelp() 
{
	// just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
	SendMessage(WM_COMMAND, ID_HELP, 0);
}

LRESULT CShapeGoToPropPageDisabled::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}