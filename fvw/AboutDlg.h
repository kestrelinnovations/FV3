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



// AboutDlg.h

#ifndef ABOUT_DLG_H
#define ABOUT_DLG_H

#include "AboutDlgResource.h"
#include "StatLink.h"		// for UTL_StaticLink

#ifdef GOV_RELEASE
   #define CONTACTS_TECHNICAL_SUPPORT_PAGE_NUMBER 5
#else
   #define CONTACTS_TECHNICAL_SUPPORT_PAGE_NUMBER 0
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
protected:
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
public:
   CAboutDlg();

   void SetInitialPage(long lInitialPage) { m_lInitialPage = lInitialPage; }
	
	// Dialog Data
	
   //{{AFX_DATA(CAboutDlg)
   enum { IDD = IDD_ABOUTBOX };
   CString getHelpURIPath(){return "fvw_core.chm::/FVW/About_FalconView.htm";}
   //}}AFX_DATA
	
	// Implementation
protected:

   UTL_StaticLink m_website;
   UTL_StaticLink m_mission_planning_website;
  
   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
   //{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   long m_lInitialPage;
};

#endif