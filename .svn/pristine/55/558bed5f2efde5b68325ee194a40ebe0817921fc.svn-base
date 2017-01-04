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

// GenProp.cpp : implementation file
//

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/GenProp.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/getobjpr.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/resource.h"

// this project's headers
#include "FalconView/PrintToolOverlay/factory.h"
#include "FalconView/PrintToolOverlay/PrntPage.h"

IMPLEMENT_DYNCREATE(COvlPrintToolPropertyPage, CPropertyPage)

COvlPrintToolPropertyPage::COvlPrintToolPropertyPage() :
CPropertyPage(COvlPrintToolPropertyPage::IDD)
{
   // {{AFX_DATA_INIT(COvlPrintToolPropertyPage)
      // NOTE: the ClassWizard will add member initialization here
   // }}AFX_DATA_INIT
}

COvlPrintToolPropertyPage::~COvlPrintToolPropertyPage()
{
}

void COvlPrintToolPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   // {{AFX_DATA_MAP(COvlPrintToolPropertyPage)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   // }}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COvlPrintToolPropertyPage, CPropertyPage)
   // {{AFX_MSG_MAP(COvlPrintToolPropertyPage)
   // }}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL COvlPrintToolPropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void COvlPrintToolPropertyPage::PostNcDestroy()
{
   // if there isn't a print overlay open we need to created a static printer
   // DC in OnInitDialog so that the page sizes will be correct in the property
   // pages, we have to destroy it now
   if (OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_PageLayout) == NULL)
   {
      CDC &dc = CPrintToolOverlay::GetPrintDC();
      if (dc.m_hAttribDC != NULL || dc.m_hDC != NULL)
      {
         dc.m_hAttribDC = NULL;
         dc.m_hDC = NULL;
      }
   }

   CPropertyPage::PostNcDestroy();
}

void COvlPrintToolPropertyPage::on_help()
{
}

BOOL CPropertySheetPrintToolTrans::PreTranslateMessage(MSG* pMsg)
{
   if (pMsg->message == WM_KEYDOWN)
   {
      switch (pMsg->wParam)
      {
      case VK_F1:
         on_help();

         return TRUE;
      }
   }

   return CPropertySheet::PreTranslateMessage(pMsg);
}

void CPropertySheetPrintToolTrans::on_help()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      if (GetActiveIndex() == 0)
         pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0,
         "fvw.chm::/PageLayout.chm::/Page_Layout_topics/"
         "Page_Layout_Area_Chart_Options.htm");

      else if (GetActiveIndex() == 1)
         pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0,
         "fvw.chm::/PageLayout.chm::/Page_Layout_topics/"
         "Page_Layout_Single_Page_Options.htm");

      else if (GetActiveIndex() == 2)
         pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0,
         "fvw.chm::/PageLayout.chm::/Page_Layout_topics/"
         "Page_Layout_Strip_Chart_Options.htm");
   }
}
