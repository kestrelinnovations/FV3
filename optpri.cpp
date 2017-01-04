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



// optpri.cpp : implementation file
//
#include "stdafx.h"
#include "err.h"
#include "resource.h"
#include "optpri.h"
#include "geo_tool.h"
#include "param.h"
#include "overlay.h"
#include "getobjpr.h"

#include "FalconView/StatusBarInfoPane.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsPri property page

IMPLEMENT_DYNCREATE(COptionsPri, CPropertyPage)

COptionsPri::COptionsPri() : CPropertyPage(COptionsPri::IDD)
{
	//{{AFX_DATA_INIT(COptionsPri)
	//}}AFX_DATA_INIT
}

COptionsPri::~COptionsPri()
{
}

void COptionsPri::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsPri)
	DDX_Control(pDX, IDC_GEOCX2CTRL1, m_ocxGeo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsPri, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsPri)
	ON_BN_CLICKED(IDC_RADIO1A, OnRadio1a)
	ON_BN_CLICKED(IDC_RADIO2A, OnRadio2a)
	ON_BN_CLICKED(IDC_RADIO3A, OnRadio3a)
	ON_BN_CLICKED(IDC_RADIO4A, OnRadio4a)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsPri message handlers

BOOL COptionsPri::OnInitDialog() 
{
   const int DATUM_LEN = 6;
   char sDatum[DATUM_LEN];
   char format[PRM_MAX_VALUE_LENGTH+1];
   char lat_lon_format[PRM_MAX_VALUE_LENGTH+1];

   CPropertyPage::OnInitDialog();
   
   // get coordinate format */
	GEO_get_primary_format(format, PRM_MAX_VALUE_LENGTH+1);
   GEO_get_primary_lat_lon_format(lat_lon_format, PRM_MAX_VALUE_LENGTH+1);

	// set coordiante_format from format string 
	if (strcmp(format, "LAT_LON") == 0)
	{
		CheckRadioButton (IDC_RADIO1A, IDC_RADIO4A, IDC_RADIO1A);
	}
	else if (strcmp(format, "MILGRID") == 0)
	{
		CheckRadioButton(IDC_RADIO1A, IDC_RADIO4A, IDC_RADIO2A);
		GetDlgItem(IDC_RADIO1B)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2B)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO3B)->EnableWindow(FALSE);
	}
	else if (strcmp(format, "UTM") == 0)
	{
		CheckRadioButton(IDC_RADIO1A, IDC_RADIO4A, IDC_RADIO3A);
		GetDlgItem(IDC_RADIO1B)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2B)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO3B)->EnableWindow(FALSE);
	}
   else if (strcmp(format, "GARS") == 0)
   {
		CheckRadioButton(IDC_RADIO1A, IDC_RADIO4A, IDC_RADIO4A);
		GetDlgItem(IDC_RADIO1B)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2B)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO3B)->EnableWindow(FALSE);
   }
	else
	{
		ERR_report("GEO_get_primary_format returned invalid format.");
		ERR_report(format);
		CheckRadioButton (IDC_RADIO1A, IDC_RADIO4A, IDC_RADIO1A);
	}

   if (strcmp(lat_lon_format, "DEGREES") == 0)
   {
      CheckRadioButton (IDC_RADIO1B, IDC_RADIO3B, IDC_RADIO1B);
   }
   else if (strcmp(lat_lon_format, "DEGREES MINUTES") == 0)
	{      
		CheckRadioButton (IDC_RADIO1B, IDC_RADIO3B, IDC_RADIO2B);
	}
   else if (strcmp(lat_lon_format, "DEGREES MINUTES SECONDS") == 0)
	{
		CheckRadioButton (IDC_RADIO1B, IDC_RADIO3B, IDC_RADIO3B);
	}
   else
	{
		ERR_report("GEO_get_primary_lat_lon_format returned invalid format.");
		ERR_report(lat_lon_format);
		CheckRadioButton (IDC_RADIO1B, IDC_RADIO3B, IDC_RADIO2B);
	}

	GEO_get_primary_datum(sDatum, DATUM_LEN);

	m_old_datum = sDatum;

	m_ocxGeo.SetDatumText(sDatum);
   
   UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsPri::OnOK() 
{
   CString sDatum;

   if (IsDlgButtonChecked (IDC_RADIO1A) )
		GEO_set_primary_format( "LAT_LON" );
   else if (IsDlgButtonChecked(IDC_RADIO2A))
		GEO_set_primary_format( "MILGRID" );
   else if (IsDlgButtonChecked(IDC_RADIO3A))
		GEO_set_primary_format( "UTM" );
   else if (IsDlgButtonChecked(IDC_RADIO4A))
		GEO_set_primary_format( "GARS" );

	if (IsDlgButtonChecked (IDC_RADIO1B) )
		GEO_set_primary_lat_lon_format( "DEGREES" );
   else if (IsDlgButtonChecked (IDC_RADIO2B) )
		GEO_set_primary_lat_lon_format( "DEGREES MINUTES" );
   else if (IsDlgButtonChecked (IDC_RADIO3B) )
		GEO_set_primary_lat_lon_format( "DEGREES MINUTES SECONDS" );

 	sDatum = m_ocxGeo.GetDatumText();

	GEO_set_primary_datum(sDatum);
	if (m_old_datum.Compare(sDatum))
   {
      // Update the default-datum statusbar information pane
      CMainFrame* frame = fvw_get_frame();
      if (frame)
      {
         static DefaultDatumInfoPane *info_pane =
            static_cast<DefaultDatumInfoPane *>(frame->
               GetStatusBarInfoPane(STATUS_BAR_PANE_DEFAULT_DATUM));
         if (info_pane != NULL)
            info_pane->OnDefaultDatumUpdated();
      }

		OVL_get_overlay_manager()->invalidate_all();
   }

	CPropertyPage::OnOK();
}

void COptionsPri::OnRadio1a() 
{
	GetDlgItem(IDC_RADIO1B)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO2B)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO3B)->EnableWindow(TRUE);
}

void COptionsPri::OnRadio2a() 
{
	GetDlgItem(IDC_RADIO1B)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO2B)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO3B)->EnableWindow(FALSE);
}

void COptionsPri::OnRadio3a() 
{
	GetDlgItem(IDC_RADIO1B)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO2B)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO3B)->EnableWindow(FALSE);
}

void COptionsPri::OnRadio4a() 
{
	GetDlgItem(IDC_RADIO1B)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO2B)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO3B)->EnableWindow(FALSE);
}

LRESULT COptionsPri::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}