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

// NewMapGroup.cpp : implementation file
//

#include "stdafx.h"
#include "fvw.h"
#include "NewMapGroup.h"
#include "getobjpr.h"



// CNewMapGroup dialog

IMPLEMENT_DYNAMIC(CNewMapGroup, CDialog)

CNewMapGroup::CNewMapGroup(CWnd* pParent /*=NULL*/)
	: CDialog(CNewMapGroup::IDD, pParent)
	, m_sMapGroupName(_T(""))
{

}

CNewMapGroup::~CNewMapGroup()
{
}

void CNewMapGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MAP_GROUP_NAME, m_sMapGroupName);
}


BEGIN_MESSAGE_MAP(CNewMapGroup, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()


// CNewMapGroup message handlers
LRESULT CNewMapGroup::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

