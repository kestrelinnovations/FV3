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



// rblocdlg.cpp : implementation file
//

#include "stdafx.h"
#include "rblocdlg.h"
#include "gpstools.h"	// for GET_POSITION_FROM_MAP def
#include "geo_tool.h"
#include "fvwutil.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// LocationDlg dialog

LocationDlg::LocationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LocationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(LocationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void LocationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LocationDlg)
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_FIX_POINT_EDIT, m_fix_point);
	DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geocx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LocationDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(LocationDlg)
	ON_EN_KILLFOCUS(IDC_FIX_POINT_EDIT, OnKillfocusFixPoint)
	ON_EN_KILLFOCUS(IDC_DESCRIPTION, OnKillfocusDescription)
	ON_CBN_KILLFOCUS(IDC_FIX_POINT_COMBO, OnKillfocusFixPoint)
	ON_BN_CLICKED(IDC_GET_FROM_MAP, OnGetPositionFromMap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LocationDlg message handlers

BOOL LocationDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

	m_bitmap_button.AutoLoad(IDC_GET_FROM_MAP, this);
	m_bitmap_button.LoadMapped();

	// hide the fix point combo box if there are no entries
	// in the fix_point_lst
	if (m_fix_pnt_lst.IsEmpty())
		GetDlgItem(IDC_FIX_POINT_COMBO)->ShowWindow(FALSE);
	// otherwise, we have entries in the fix_point_lst so we will
	// be using the combo box.  hide the edit ctrl in this case
	else
	{
		GetDlgItem(IDC_FIX_POINT_EDIT)->ShowWindow(FALSE);
		
		CComboBox *combo = (CComboBox *)GetDlgItem(IDC_FIX_POINT_COMBO);
		
		POSITION position = m_fix_pnt_lst.GetHeadPosition();
		while (position)
			combo->AddString(m_fix_pnt_lst.GetNext(position));
	}

   // limit edit control string lengths
   m_fix_point.LimitText(12);
   m_description.LimitText(40);

	if (m_snap_to_valid)
	{
		m_fix_point.SetWindowText(m_snap_to.m_fix_rs);
		m_description.SetWindowText(m_snap_to.m_description_rs);
	}

	//////////////////////////////////////////////////////////////////////////
   // initialize location in the OCX Control (GEOCX)
	double lat;
	double lon;
	if (m_is_from_not_to)
	{
		lat = m_data.m_from_latitude;
		lon = m_data.m_from_longitude;
	}
	else
	{
		lat = m_data.m_to_latitude;
		lon = m_data.m_to_longitude;
	}

	// get the default datum, so it can be included with the location string
   const int DATUM_LEN = 8;
	char datum[DATUM_LEN];
	GEO_get_default_datum(datum, DATUM_LEN);
	m_geocx.SetDatumText(datum);
	
	// initialize location in the OCX Control (GEOCX)
	m_geocx.SetGeocx(lat, lon);
	
	// convert lat-lon to formated string in default format and datum
   const int STR_LEN = 256;
	char latlon_string[256];
	GEO_lat_lon_to_string(lat, lon, latlon_string, STR_LEN);
	m_geocx.SetLocationText(latlon_string);
   //////////////////////////////////////////////////////////////////////////

   UpdateData(FALSE);
      
   // move the focus to the Fix/Point input control
   CWnd *ctrl;
	if (m_fix_pnt_lst.IsEmpty())
		ctrl = GetDlgItem(IDC_FIX_POINT_EDIT);
	else
		ctrl = GetDlgItem(IDC_FIX_POINT_COMBO);
   if (ctrl)
      GotoDlgCtrl(ctrl);

   return FALSE;  // return TRUE  unless you set the focus to a control
}

void LocationDlg::OnOK()
{  
   // Force the focus to the OK button so the OnKillFocus function
   // gets called for the control that currently has the focus.
   CWnd *ok = GetDlgItem(IDOK);
   ASSERT(ok);
   if (ok != GetFocus())
   {
      if (ok)
         ok->SetFocus();

      // m_invalid_input is set to TRUE if a control detects bad input
      // on validation - focus should return to that control and the 
      // dialog box should not close
      if (m_invalid_input)
      {
         m_invalid_input = FALSE;
         return;
      }
   }                       

   // get data from controls
   UpdateData(TRUE);

   //////////////////////////////////////////////////////////////////////////
	// get latitude and longitude from the OCX Control (GEOCX)
	if (m_is_from_not_to)
	{
		m_data.m_from_latitude = m_geocx.GetLatitude();
		m_data.m_from_longitude = m_geocx.GetLongitude();
	}
	else
	{
		m_data.m_to_latitude = m_geocx.GetLatitude();
		m_data.m_to_longitude = m_geocx.GetLongitude();
	}
   //////////////////////////////////////////////////////////////////////////

	GetDlgItem(IDC_DESCRIPTION)->GetWindowText(m_description_str);

	if (m_fix_pnt_lst.IsEmpty())
		GetDlgItem(IDC_FIX_POINT_EDIT)->GetWindowText(m_fix_point_str);
	else
		GetDlgItem(IDC_FIX_POINT_COMBO)->GetWindowText(m_fix_point_str);

   EndDialog(IDOK);
}

void LocationDlg::OnKillfocusFixPoint() 
{
   CString string;
   SnapToInfo db_info;
	boolean_t do_db_lookup;
	CFvwUtil *futil = CFvwUtil::get_instance();
	
	if (m_fix_pnt_lst.IsEmpty())
	{
		// don't validate if nothing changed
		if (!m_fix_point.GetModify())
			return;

		do_db_lookup = TRUE;

		// if the current string is a valid fixed point, then it can be used to
		// retreive a database point
		m_fix_point.GetWindowText(string);
	}
	else
	{
		GetDlgItem(IDC_FIX_POINT_COMBO)->GetWindowText(string);
		
		// only do the db lookup if the string in the combo box
		// does not match one of the strings in the list
		do_db_lookup = TRUE;
		POSITION position = m_fix_pnt_lst.GetHeadPosition();
		while (position)
			if (m_fix_pnt_lst.GetNext(position) == string)
			{
				do_db_lookup = FALSE;
				break;
			}
	}
	
	// if no match was found, make sure the string starts with a '.' to
	// indicate that it is a comment, i.e., no match was found
	if (do_db_lookup && !futil->db_lookup(string, db_info, TRUE))
	{
		if (!string.IsEmpty())
		{
			// indicate a miss to the user
			if (string[0] != '.')
			{
				string = '.' + string;
				
			}
			// m_fix_point.SetWindowText(string); used to be inside the above
			// block, i.e., SetWindowText would only get called if a dot was
			// prepended to the string retrieved from the m_fix_point control.
			// FalconView was crashing on Windows 95 and 98 if you entered a
			// Fix/Point starting with a '.', clicked on the Description field,
			// and then clicked on the X button.  For some reason moving this
			// call here - so it always gets called - makes the crash go away.
			m_fix_point.SetWindowText(string);
			m_description.SetWindowText("No Match");
			
			// disable OnOK so the user can see the miss
			m_invalid_input = TRUE;
			
			// move the focus to the Fix/Point input control
			//CWnd *ctrl = GetDlgItem(IDC_FIX_POINT_EDIT);
			//if (ctrl)
			//	GotoDlgCtrl(ctrl);
			//ASSERT(ctrl);
		}
		
		return;
	}
	
	if (do_db_lookup)
	{
		// set string fields
		m_fix_point.SetWindowText(db_info.m_fix_rs);
		m_description.SetWindowText(db_info.m_description_rs);
		
		// set map center in the OCX control
		m_geocx.SetDatumText(db_info.m_src_datum);
		m_geocx.SetGeocx(db_info.m_lat, db_info.m_lon);
	}
}

void LocationDlg::OnKillfocusDescription() 
{
   CString string;
   SnapToInfo db_info;
	CFvwUtil *futil = CFvwUtil::get_instance();

   // don't validate if nothing changed
   if (!m_description.GetModify())
      return;

   // database points can also be retreived by description
   m_description.GetWindowText(string);

   // if no match was found, make sure the string starts with a '.' to
   // indicate that it is a comment, i.e., no match was found
   if (!futil->db_lookup(string, db_info, FALSE))
   {
      if (!string.IsEmpty())
      {
         // indicate a miss to the user
         if (string[0] != '.')
         {
            string = '.' + string;
         }
         // See m_fix_point.SetWindowText(string); comment in
         // OnKillfocusFixPoint above. 
         m_description.SetWindowText(string);
         m_fix_point.SetWindowText("No Match");

         // disable OnOK so the user can see the miss
         m_invalid_input = TRUE;

         // move the focus to the Description input control
         CWnd *ctrl = GetDlgItem(IDC_DESCRIPTION);
         if (ctrl)
            GotoDlgCtrl(ctrl);
         ASSERT(ctrl);
      }

      return;
   }

   // set string fields
   m_fix_point.SetWindowText(db_info.m_fix_rs);
   m_description.SetWindowText(db_info.m_description_rs);

   // set map center in the OCX control
   m_geocx.SetDatumText(db_info.m_src_datum);
   m_geocx.SetGeocx(db_info.m_lat, db_info.m_lon);
}

void LocationDlg::OnGetPositionFromMap()
{
	// we need to temporarily hide this modal dialog box, change
   // the cursor, and retreive a latitude longitude pair from the
   // map
   EndDialog(GET_POSITION_FROM_MAP);
}

void LocationDlg::SetData(double lat, double lon, SnapToInfo snap_to,
								  boolean_t snap_to_valid, boolean_t is_from_not_to)
{
	m_is_from_not_to = is_from_not_to;

	if (m_is_from_not_to)
	{
		m_data.m_from_latitude = lat;
		m_data.m_from_longitude = lon;
	}
	else
	{	
		m_data.m_to_latitude = lat;
		m_data.m_to_longitude = lon;
	}

	m_snap_to = snap_to;
	m_snap_to_valid = snap_to_valid;
}
LRESULT LocationDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

