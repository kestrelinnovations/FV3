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



// topicnam.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "topicnam.h"
#include "getobjpr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopicName dialog

CTopicName::CTopicName(CWnd* pParent /*=NULL*/)
   : CDialog(CTopicName::IDD, pParent)
{
   //{{AFX_DATA_INIT(CTopicName)
   m_group_name = "";
   //}}AFX_DATA_INIT
}

void CTopicName::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTopicName)
   DDX_Text(pDX, IDC_EDIT1, m_group_name);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTopicName, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CTopicName)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopicName message handlers

void CTopicName::OnOK()
{
   /* get group name from dialog box */
   if (!UpdateData(TRUE))
   {
      TRACE0("UpdateData failed in OnOK()\n");
      return;
   }

   // force the entry of a topic name
   if (m_group_name == "")
      return;
   
   CDialog::OnOK();
}
LRESULT CTopicName::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

