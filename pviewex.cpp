// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// pviewex.cpp : implementation file
//

#include "stdafx.h"
#include "pviewex.h"
#include "tabulareditor\targetgraphicsdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFvPreviewView

IMPLEMENT_DYNCREATE(CFvPreviewView, CPreviewViewEx)

int CFvPreviewView::m_edit_selected;
int CFvPreviewView::m_page_number;

CFvPreviewView::CFvPreviewView()
{
}

CFvPreviewView::~CFvPreviewView()
{
}


BEGIN_MESSAGE_MAP(CFvPreviewView, CPreviewViewEx)
	//{{AFX_MSG_MAP(CFvPreviewView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
      ON_BN_CLICKED(ID_PREVIEW_EDIT_LEFT, OnEditLeft)
      ON_UPDATE_COMMAND_UI(ID_PREVIEW_EDIT_LEFT, OnUpdateEditLeft)
      ON_BN_CLICKED(ID_PREVIEW_EDIT_RIGHT, OnEditRight)
      ON_UPDATE_COMMAND_UI(ID_PREVIEW_EDIT_RIGHT, OnUpdateEditRight)
      ON_BN_CLICKED(AFX_ID_PREVIEW_NEXT, OnNextPage)
      ON_BN_CLICKED(AFX_ID_PREVIEW_PREV, OnPrevPage)
      ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFvPreviewView diagnostics

#ifdef _DEBUG
void CFvPreviewView::AssertValid() const
{
	CView::AssertValid();
}

void CFvPreviewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFvPreviewView message handlers

void CFvPreviewView::OnEditLeft()
{
   CFvPreviewView::m_edit_selected = TRUE;
   CFvPreviewView::m_page_number = m_nCurrentPage;

   CPreviewViewEx::OnPreviewClose();
}

void CFvPreviewView::OnUpdateEditLeft(CCmdUI* pCmdUI)
{
   char buf[8];

	CWnd *wnd = pCmdUI->m_pOther;
	if (!m_show_edit_buttons)
		wnd->ShowWindow(SW_HIDE);
	else
	{
		// set the left button's text
		_itoa_s(m_nCurrentPage, buf, 8, 10);
		pCmdUI->SetText(CString("Edit Page ") + CString(buf));
	}
}

void CFvPreviewView::OnEditRight()
{
   CFvPreviewView::m_edit_selected = TRUE;
   CFvPreviewView::m_page_number = m_nCurrentPage + 1;

   CPreviewViewEx::OnPreviewClose();
}

void CFvPreviewView::OnUpdateEditRight(CCmdUI* pCmdUI)
{
   char buf[8];

   // show/hide the right edit buttton depending whether we are dealing with 
   // one or two pages
	CWnd *wnd = pCmdUI->m_pOther;
   if (m_nPages == 1 || !m_show_edit_buttons)
      wnd->ShowWindow(SW_HIDE);
   else
   {
      // make sure the last page in the two page preview exists
      if (m_nCurrentPage + 1 > m_pPreviewInfo->GetMaxPage())
         wnd->ShowWindow(SW_HIDE);
      else
         wnd->ShowWindow(SW_SHOW);
   }

   // set the right button's text
   _itoa_s(m_nCurrentPage + 1, buf, 8, 10);
   pCmdUI->SetText(CString("Edit Page ") + CString(buf));
}

void CFvPreviewView::OnNextPage()
{
   if (m_nPages == 1)
      SetCurrentPage(m_nCurrentPage + 1, TRUE);
   else
      SetCurrentPage(m_nCurrentPage + 2, TRUE);
}

void CFvPreviewView::OnPrevPage()
{
   if (m_nPages == 1)
      SetCurrentPage(m_nCurrentPage - 1, TRUE);
   else
      SetCurrentPage(m_nCurrentPage - 2, TRUE);
}

int CFvPreviewView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPreviewViewEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFvPreviewView::m_edit_selected = FALSE;

	// if target graphics printing is active we need to hide the Edit xxx buttons
	m_show_edit_buttons = !CTargetGraphicsDlg::is_active();

   return 0;
}


