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



// optdis.cpp : implementation file
//
#include "stdafx.h"
#include "err.h"
#include "resource.h"
#include "optdis.h"
#include "param.h"
#include "dted_tmr.h"   // for DtedUnitsEnum
#include "mapview.h"
#include "getobjpr.h"
#include "geo_tool.h"

#include "FalconView/StatusBarInfoPane.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsDis property page

IMPLEMENT_DYNCREATE(COptionsDis, CPropertyPage)

COptionsDis::COptionsDis() : CPropertyPage(COptionsDis::IDD)
{
   //{{AFX_DATA_INIT(COptionsDis)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

COptionsDis::~COptionsDis()
{
}

void COptionsDis::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(COptionsDis)
   DDX_Control(pDX, IDC_RADIO1, m_rbPrimaryFormat);
   DDX_Control(pDX, IDC_RADIO2, m_rbSecondaryFormat);

   DDX_Control(pDX, IDC_RADIO_METERS, m_rbMeters);
   DDX_Control(pDX, IDC_RADIO_FEET, m_rbFeet);

   DDX_Control(pDX, IDC_RADIO_MSL, m_rbMSL);
   DDX_Control(pDX, IDC_RADIO_HAE, m_rbHAE);
   DDX_Control(pDX, IDC_RADIO_HAE_EGM84, m_rbEGM84);
   DDX_Control(pDX, IDC_RADIO_HAE_EGM96, m_rbEGM96);
   DDX_Control(pDX, IDC_RADIO_HAE_EGM08, m_rbEGM08);

   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDis, CPropertyPage)
   //{{AFX_MSG_MAP(COptionsDis)
   ON_BN_CLICKED(IDC_RADIO_HAE, OnHAE)
   ON_BN_CLICKED(IDC_RADIO_MSL, OnMSL)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDis message handlers

BOOL COptionsDis::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   
   char format[PRM_MAX_VALUE_LENGTH+1];

   // get coordinate format */
   GEO_get_default_display(format, PRM_MAX_VALUE_LENGTH+1);

   m_old_format = format;

   // set coordinate format from string 
   if (strcmp(format, "PRIMARY") == 0)
      m_rbPrimaryFormat.SetCheck(1);
   else if (strcmp(format, "SECONDARY") == 0)
      m_rbSecondaryFormat.SetCheck(1);
   else
   {
      ERR_report("GEO_get_default_display returned invalid format.");
      ERR_report(format);
      m_rbPrimaryFormat.SetCheck(1);
   }

   DtedUnitsEnum eElevationUnits = static_cast<DtedUnitsEnum>(PRM_get_registry_int("CoordinateFormat", "RunningElevationUnits", DTED_UNITS_FEET));
   DtedModeEnum eElevationMode = static_cast<DtedModeEnum>(PRM_get_registry_int("CoordinateFormat", "RunningElevationMode", DTED_MODE_MSL));
   DtedVertDatumEnum eDtedVertDatum = static_cast<DtedVertDatumEnum>(PRM_get_registry_int("CoordinateFormat", "RunningElevationVertDatum", DTED_VDATUM_EGM96));
   m_rbMeters.SetCheck(eElevationUnits == DTED_UNITS_METERS);
   m_rbFeet.SetCheck(eElevationUnits == DTED_UNITS_FEET);
   m_rbMSL.SetCheck(eElevationMode == DTED_MODE_MSL);
   m_rbHAE.SetCheck(eElevationMode == DTED_MODE_HAE);
   m_rbEGM84.SetCheck(eDtedVertDatum == DTED_VDATUM_EGM84);
   m_rbEGM96.SetCheck(eDtedVertDatum == DTED_VDATUM_EGM96);
   m_rbEGM08.SetCheck(eDtedVertDatum == DTED_VDATUM_EGM08);
   
   if (eElevationMode != DTED_MODE_HAE)
   {
      GetDlgItem(IDC_RADIO_HAE_EGM84)->EnableWindow(FALSE);
      GetDlgItem(IDC_RADIO_HAE_EGM96)->EnableWindow(FALSE);
      GetDlgItem(IDC_RADIO_HAE_EGM08)->EnableWindow(FALSE);
   }

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void COptionsDis::OnOK() 
{
   if (m_rbPrimaryFormat.GetCheck())
   {
      GEO_set_default_display( "PRIMARY" );
      if (m_old_format.Compare("PRIMARY"))
         OVL_get_overlay_manager()->invalidate_all();
   }
   else if (m_rbSecondaryFormat.GetCheck())
   {
      GEO_set_default_display( "SECONDARY" );
      if (m_old_format.Compare("SECONDARY"))
         OVL_get_overlay_manager()->invalidate_all();
   }

   DtedUnitsEnum eDtedUnits = m_rbFeet.GetCheck() ? DTED_UNITS_FEET : DTED_UNITS_METERS;
   PRM_set_registry_int("CoordinateFormat", "RunningElevationUnits", eDtedUnits);

   DtedModeEnum eDtedMode = m_rbMSL.GetCheck() ? DTED_MODE_MSL : DTED_MODE_HAE;
   PRM_set_registry_int("CoordinateFormat", "RunningElevationMode", eDtedMode);

   DtedVertDatumEnum eDtedVertDatum;
   if (m_rbEGM84.GetCheck())
      eDtedVertDatum = DTED_VDATUM_EGM84;
   if (m_rbEGM96.GetCheck())
      eDtedVertDatum = DTED_VDATUM_EGM96;
   if (m_rbEGM08.GetCheck())
      eDtedVertDatum = DTED_VDATUM_EGM08;

   PRM_set_registry_int("CoordinateFormat", "RunningElevationVertDatum", eDtedVertDatum);

   MapView *pMapView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pMapView && pMapView->GetDtedTimer())
   {
      pMapView->GetDtedTimer()->SetDtedUnits(eDtedUnits);
      pMapView->GetDtedTimer()->SetDtedMode(eDtedMode);
      pMapView->GetDtedTimer()->SetDtedVertDatum(eDtedVertDatum);
   }

   // Update the HeightAboveEllipsoidInfoPane since it depends on the value
   // of RunningElevationUnits.
   CMainFrame* frame = fvw_get_frame();
   if (frame)
   {
      HeightAboveEllipsoidInfoPane *p =
         static_cast<HeightAboveEllipsoidInfoPane*>(frame->
            GetStatusBarInfoPane(STATUS_BAR_PANE_HEIGHT_ABOVE_ELLIPSOID));
      if (p)
         p->UpdateText();
   }

   CPropertyPage::OnOK();
}

void COptionsDis::OnHAE() 
{
   GetDlgItem(IDC_RADIO_HAE_EGM84)->EnableWindow(TRUE);
   GetDlgItem(IDC_RADIO_HAE_EGM96)->EnableWindow(TRUE);
   GetDlgItem(IDC_RADIO_HAE_EGM08)->EnableWindow(TRUE);
}

void COptionsDis::OnMSL() 
{
   GetDlgItem(IDC_RADIO_HAE_EGM84)->EnableWindow(FALSE);
   GetDlgItem(IDC_RADIO_HAE_EGM96)->EnableWindow(FALSE);
   GetDlgItem(IDC_RADIO_HAE_EGM08)->EnableWindow(FALSE);
}

LRESULT COptionsDis::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}