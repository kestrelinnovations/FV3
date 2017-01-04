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



// optsheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "optdis.h"
#include "optsheet.h"

#define VK_F	0x46



BEGIN_MESSAGE_MAP(COptionsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(COptionsSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionsSheet

IMPLEMENT_DYNAMIC(COptionsSheet, CPropertySheet)

COptionsSheet::COptionsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

COptionsSheet::COptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

COptionsSheet::~COptionsSheet()
{
}


/////////////////////////////////////////////////////////////////////////////
// COptionsSheet message handlers

BOOL COptionsSheet::OnInitDialog() 
{
   CRect rect;
   CWnd *pHelpButton = GetDlgItem(IDHELP);        //IDHELP == 0x0009

   ASSERT(pHelpButton);

   //set the visibility state of the help button (hide) and
   //hides the Apply button
   pHelpButton->ShowWindow(SW_HIDE);

   // resize the tab control & dialog box
	GetWindowRect(rect);

	CTabCtrl *pTabPage = (CTabCtrl *)GetDlgItem(0x3020);

	CRect tabRect( 10,10,245,175);

	if (pTabPage)
		pTabPage->MoveWindow(tabRect);

   //Positive values inflate CRect and negative values deflate it
	rect.InflateRect(0,0,-75,0);

	MoveWindow(rect);

	return CPropertySheet::OnInitDialog();
}


BOOL COptionsSheet::PreTranslateMessage(MSG* pMsg) 
{
   switch( pMsg->message )
   {
      case WM_KEYDOWN:

			if (GetKeyState(VK_CONTROL) < 0 && pMsg->wParam == VK_F)
			{
				COptionsDis *p = (COptionsDis *)GetPage(0);

				if (p->IsDlgButtonChecked(IDC_RADIO1) )
				{
					p->CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);

					//get the index number of the property sheet and 
					// check if it is the active page
					if (GetActiveIndex() == 0)
						p->GetDlgItem(IDC_RADIO2)->SetFocus();
				}
				else if (p->IsDlgButtonChecked(IDC_RADIO2) )
				{
					p->CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);

					if (GetActiveIndex() == 0)
						p->GetDlgItem(IDC_RADIO1)->SetFocus();
				}

				return TRUE;
			}
   }

	return CPropertySheet::PreTranslateMessage(pMsg);
}
