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

// StatBarBullseyePropPage.cpp
//

#include "stdafx.h"
#include "StatBarBullseyePropPage.h"
#include "MovingMapOverlay\rblocdlg.h"
#include "MovingMapOverlay\gpstools.h"	// for GET_POSITION_FROM_MAP
#include "param.h"
#include "geo_tool.h"
#include "getobjpr.h"

CString CStatBarBullseyePropPage::m_strDescription;
double CStatBarBullseyePropPage::m_dLat;
double CStatBarBullseyePropPage::m_dLon;
bool CStatBarBullseyePropPage::m_bUnitsNMnotKM;
bool CStatBarBullseyePropPage::m_bCourseMagNotTrue;

/////////////////////////////////////////////////////////////////////////////
// CStatBarBullseyePropPage property page

IMPLEMENT_DYNCREATE(CStatBarBullseyePropPage, CPropertyPage)

CStatBarBullseyePropPage::CStatBarBullseyePropPage() : 
   CPropertyPage(CStatBarBullseyePropPage::IDD)
{
	//{{AFX_DATA_INIT(CStatBarBullseyePropPage)
	//}}AFX_DATA_INIT
}

void CStatBarBullseyePropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatBarBullseyePropPage)
   DDX_Control(pDX, IDC_UNITS_NM, m_rbUnitsNM);
   DDX_Control(pDX, IDC_UNITS_KM, m_rbUnitsKM);
   DDX_Control(pDX, IDC_COURSE_MAG, m_rbCourseMag);
   DDX_Control(pDX, IDC_COURSE_TRUE, m_rbCourseTrue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatBarBullseyePropPage, CPropertyPage)
   ON_BN_CLICKED(IDC_FROM, &CStatBarBullseyePropPage::OnBnClickedFrom)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatBarBullseyePropPage message handlers

BOOL CStatBarBullseyePropPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   m_rbUnitsNM.SetCheck(m_bUnitsNMnotKM);
   m_rbUnitsKM.SetCheck(!m_bUnitsNMnotKM);

   m_rbCourseMag.SetCheck(m_bCourseMagNotTrue);
   m_rbCourseTrue.SetCheck(!m_bCourseMagNotTrue);

   UpdateFromText();
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStatBarBullseyePropPage::UpdateFromText()
{
	GetDlgItem(IDC_FROM_DESCRIPTION)->SetWindowText(m_strDescription);
	
   const int STR_LEN = 256;
	char latlon_string[STR_LEN];
	GEO_lat_lon_to_string(m_dLat, m_dLon, latlon_string, STR_LEN);
	GetDlgItem(IDC_FROM_LOCATION)->SetWindowText(latlon_string);
}

void CStatBarBullseyePropPage::OnOK() 
{
   SaveSettings();

	CPropertyPage::OnOK();
}

void CStatBarBullseyePropPage::SaveSettings()
{
   m_bUnitsNMnotKM = m_rbUnitsNM.GetCheck() == 1;
   m_bCourseMagNotTrue = m_rbCourseMag.GetCheck() == 1;

   PRM_set_registry_string("StatusBar", "BullseyeDescription", m_strDescription);
   PRM_set_registry_double("StatusBar", "BullseyeLat", m_dLat);
   PRM_set_registry_double("StatusBar", "BullseyeLon", m_dLon);
   PRM_set_registry_int("StatusBar", "BullseyeNM", m_bUnitsNMnotKM);
   PRM_set_registry_int("StatusBar", "BullseyeCourse", m_bCourseMagNotTrue);
}

void CStatBarBullseyePropPage::OnBnClickedFrom()
{
   LocationDlg dlg;

   SnapToInfo snapToInfo;
   boolean_t bSnapToValid = FALSE;

   dlg.SetData(m_dLat, m_dLon, snapToInfo, bSnapToValid, TRUE);

   const int nResult = dlg.DoModal();
   if (nResult == IDOK)
	{
		m_dLat = dlg.get_latitude();
      m_dLon = dlg.get_longitude();

		// get the description from the dialog
		CString description = dlg.get_description();
		// if the description is blank, then try using the fix/point text
		if (description == "")
			description = dlg.get_fix_point();
		// if the description is still blank, then use 'Fixed Location' as
		// the description
		if (description == "")
			description = "Fixed Location";

		m_strDescription = description;

      UpdateFromText();
	}
   else if (nResult == GET_POSITION_FROM_MAP)
   {
      SaveSettings();
      EndDialog(GET_POSITION_FROM_MAP);
   }
}

LRESULT CStatBarBullseyePropPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}
