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



// ctrakdlg.cpp : implementation file
//

#include "stdafx.h"
#include "ctrakdlg.h"
#include "csttrack.h"
#include "err.h"
#include "unit.h"          // for CUnit
#include "Utility\AlignWnd.h"   // for CAlignWnd

#include "gpstools.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CoastTrackPage property page

IMPLEMENT_DYNCREATE(CoastTrackPage, CPropertyPage)

// constructor
CoastTrackPage::CoastTrackPage() : CPropertyPage(CoastTrackPage::IDD)
{
   m_coast_track_mgr = NULL;
   m_is_initialized = FALSE;
}

CoastTrackPage::CoastTrackPage(CoastTrackManager *ctrack_mgr) : 
   CPropertyPage(CoastTrackPage::IDD)
{
	//{{AFX_DATA_INIT(CoastTrackPage)
	m_update_rate = 0;
	//}}AFX_DATA_INIT

   m_coast_track_mgr = ctrack_mgr;
   m_is_initialized = FALSE;
}

// destructor
CoastTrackPage::~CoastTrackPage()
{
}

void CoastTrackPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CoastTrackPage)
	DDX_Control(pDX, IDC_LIST1, m_ctrack_list_ctrl);
	DDX_Text(pDX, IDC_UPDATE_RATE, m_update_rate);
	DDV_MinMaxInt(pDX, m_update_rate, 1, 600);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CoastTrackPage, CPropertyPage)
	//{{AFX_MSG_MAP(CoastTrackPage)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDoubleClickList)
	ON_BN_CLICKED(IDC_ADD, OnAddCoastTrack)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchanged)
	ON_EN_CHANGE(IDC_UPDATE_RATE, OnChangeUpdateRate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CoastTrackPage message handlers

BOOL CoastTrackPage::OnInitDialog() 
{
   LV_COLUMN lc;

	CPropertyPage::OnInitDialog();

   //m_add_button.AutoLoad(IDC_ADD ,this);

	// we want the list control to have full row select
	ListView_SetExtendedListViewStyle(m_ctrack_list_ctrl.GetSafeHwnd(), LVS_EX_FULLROWSELECT);

   // initialize lc common entries
	memset(&lc, 0, sizeof(LV_COLUMN));
	lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lc.fmt = LVCFMT_LEFT;
	
   // initialize the Label column
   lc.cx = 80;
   lc.pszText = "Label";
   lc.iSubItem = 0;
   if (m_ctrack_list_ctrl.InsertColumn(0, &lc) != 0)
      ERR_report("InsertColumn(0, &lc) failed.");

   // initialize the Current Position column
   lc.cx = 160;
   lc.pszText = "Current Position";
   lc.iSubItem = 1;
   if (m_ctrack_list_ctrl.InsertColumn(1, &lc) != 1)
      ERR_report("InsertColumn(1, &lc) failed.");

    // initialize the Course column
   lc.cx = 55;
   lc.pszText = "Course";
   lc.iSubItem = 2;
   if (m_ctrack_list_ctrl.InsertColumn(2, &lc) != 2)
      ERR_report("InsertColumn(2, &lc) failed.");

   // initialize the Speed column
   lc.cx = 55;
   lc.pszText = "Speed";
   lc.iSubItem = 3;
   if (m_ctrack_list_ctrl.InsertColumn(3, &lc) != 3)
      ERR_report("InsertColumn(3, &lc) failed.");

   // initialize the Time Started column
   lc.cx = 85;
   lc.pszText = "Time Started";
   lc.iSubItem = 4;
   if (m_ctrack_list_ctrl.InsertColumn(4, &lc) != 4)
      ERR_report("InsertColumn(4, &lc) failed.");

   // initialize the Time Stopped column
   lc.cx = 93;
   lc.pszText = "Time Stopped";
   lc.iSubItem = 5;
   if (m_ctrack_list_ctrl.InsertColumn(5, &lc) != 5)
      ERR_report("InsertColumn(5, &lc) failed.");

   m_ctrack_list_ctrl.SetBkColor(RGB(0,0,0));
   m_ctrack_list_ctrl.SetTextColor(RGB(255,255,255));
   m_ctrack_list_ctrl.SetTextBkColor(RGB(0,0,0));

   m_coast_track_mgr->initialize_dialog();

   m_is_initialized = TRUE;

   UpdateData(FALSE);

	if (m_ctrack_list_ctrl.GetSelectedCount() == 0)
	{
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_CENTER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_STOP)->EnableWindow();
		GetDlgItem(IDC_CENTER)->EnableWindow();
		GetDlgItem(IDC_EDIT)->EnableWindow();
		GetDlgItem(IDC_DELETE)->EnableWindow();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CoastTrackPage::add_coast_track(CoastTrackData *ctrack)
{
   LV_ITEM lv;

   if (!IsWindow(m_ctrack_list_ctrl))
      return -1;
   
   // clear all fields of LV_ITEM structure
   memset(&lv, 0, sizeof(LV_ITEM));

   // the item index will be the current number of elements in the list
   // control since the items are zero-indexed
   int item_index = m_ctrack_list_ctrl.GetItemCount();
   
   // set the common fields
   lv.iItem = item_index;
   lv.mask = LVIF_TEXT | LVIF_PARAM;
   
   m_ctrack_list_ctrl.InsertItem(&lv);

   update_coast_track(item_index, ctrack);

   return item_index;
}

void CoastTrackPage::update_coast_track(int item_index, CoastTrackData *ctrack)
{
   LV_ITEM lv;

   if (!IsWindow(m_ctrack_list_ctrl))
      return;

   // clear all fields of LV_ITEM structure
   memset(&lv, 0, sizeof(LV_ITEM));

   // set the common fields
   lv.iItem = item_index;
   lv.mask = LVIF_TEXT | LVIF_PARAM;

   // get the report data for the current time
   ReportData &report_data = ctrack->get_report(m_coast_track_mgr->get_current_view_time());

   // set the label field
   lv.mask = LVIF_TEXT;
   lv.lParam = item_index;
   lv.iSubItem = 0;
   CString label_txt;
   // if a label is specified then use this label
   if (ctrack->get_label() != "")
   {
      label_txt = ctrack->get_label();
      lv.pszText = label_txt.GetBuffer(label_txt.GetLength());
   }
   // otherwise, the default name will be TRACKx where x is the 
   // row number in the list control (since zero-indexed we need
   // to add one
   else
   {
      label_txt.Format("TRACK%d", item_index + 1);
      lv.pszText = label_txt.GetBuffer(256);
   }
   m_ctrack_list_ctrl.SetItem(&lv);

   // set the current position
   lv.iSubItem = 1;
   const int STR_LEN = 256;
   char latlon_string[STR_LEN];
   GEO_lat_lon_to_string(ctrack->get_current_position().lat, 
      ctrack->get_current_position().lon, latlon_string, STR_LEN);
   lv.pszText = latlon_string;
   m_ctrack_list_ctrl.SetItem(&lv);

   // set the course field
   lv.iSubItem = 2;
   CString heading_txt;
   heading_txt.Format("%.0f°",report_data.m_heading);
   lv.pszText = heading_txt.GetBuffer(256);
   m_ctrack_list_ctrl.SetItem(&lv);
   
   // set the speed field (show in knots [ft/s->knots])
   lv.iSubItem = 3;
   CString velocity_txt;
   velocity_txt.Format("%0.2f", FEET_PER_S_TO_KNOTS(report_data.m_velocity));
   lv.pszText = velocity_txt.GetBuffer(256);
   m_ctrack_list_ctrl.SetItem(&lv);
   
   // set the time start field
   lv.iSubItem = 4;
   CString time_started_txt = report_data.m_time_started.Format("%H:%M:%S");
   lv.pszText = time_started_txt.GetBuffer(256);
   m_ctrack_list_ctrl.SetItem(&lv);
   
   // set the time end field
   lv.iSubItem = 5;
   CString time_stopped_txt = ctrack->get_time_stopped().Format("%H:%M:%S");
   if (ctrack->get_time_stopped().GetStatus() == COleDateTime::null)
      time_stopped_txt = "N/A";
   lv.pszText = time_stopped_txt.GetBuffer(256);
   m_ctrack_list_ctrl.SetItem(&lv);
}

void CoastTrackPage::OnAddCoastTrack() 
{
   m_coast_track_mgr->add_coast_track();
}

void CoastTrackPage::OnStop() 
{
   int item_index = -1;
   do
   {
      item_index = m_ctrack_list_ctrl.GetNextItem(item_index,LVNI_SELECTED);
      
      if (item_index != -1)
         m_coast_track_mgr->stop_coast_track(item_index);
      
   } while (item_index != -1);
}

void CoastTrackPage::OnCenter() 
{
   int item_index = -1;

   item_index = m_ctrack_list_ctrl.GetNextItem(item_index, LVNI_SELECTED);
   if (item_index == -1)
      return;

   m_coast_track_mgr->center_coast_track(item_index);
}

void CoastTrackPage::OnDelete() 
{
   if (AfxMessageBox("Are you sure you want to permanently remove the selected "
      "coast track(s)?", MB_YESNO) == IDNO)
      return;

	const int num_selected = m_ctrack_list_ctrl.GetSelectedCount();
	int *item = NULL;
	
	// sanity check before allocation
	if (num_selected > 0)
		item = new int[num_selected];
	else
		return;

	// store the list of items that were selected so we can 
	// delete them in reverse order
	int i = 0;
	POSITION position = m_ctrack_list_ctrl.GetFirstSelectedItemPosition();
	while (position)
   {
      item[i++] = m_ctrack_list_ctrl.GetNextSelectedItem(position);
	}
   
	// delete the items in reverse order
	while (i)
		m_coast_track_mgr->delete_coast_track(item[--i]);

   delete [] item;
}

void CoastTrackPage::delete_coast_track(int index)
{
   m_ctrack_list_ctrl.DeleteItem(index);
}

void CoastTrackPage::OnDoubleClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CPoint point;

   // get the current cursor location in screen coordinates
   GetCursorPos(&point);

   // convert the cursor location to the coordinates of the list control
   m_ctrack_list_ctrl.ScreenToClient(&point);

   // get the index of the item that was double clicked, if any
   int index = m_ctrack_list_ctrl.HitTest(point);
   if (index == -1)
   {
      *pResult = SUCCESS;
      return;
   }

   // set the selected entry and the focus on the current list control
   SetFocus();
   m_ctrack_list_ctrl.SetItemState(index, 
      LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	// modify the selected entry
   m_coast_track_mgr->modify_coast_track(index);

	*pResult = SUCCESS;
}

void CoastTrackPage::OnEdit() 
{
   int index = -1;
   
   index = m_ctrack_list_ctrl.GetNextItem(index, LVNI_SELECTED);
   if (index == -1)
      return;

   m_coast_track_mgr->modify_coast_track(index);
}

// set_selected - will set a given element of the list control's
// selection status to the given value
void CoastTrackPage::set_selected(int index, boolean_t is_selected)
{
   if (is_selected)
   {
      m_ctrack_list_ctrl.SetItemState(index,
         LVIS_SELECTED, LVIS_FOCUSED);
   }
   else
   {
      m_ctrack_list_ctrl.SetItemState(index,
         ~LVIS_SELECTED, LVIS_SELECTED);
   }
}

void CoastTrackPage::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   int item = pNMListView->iItem;
   if (item >= 0)
   {
      m_coast_track_mgr->set_selected(item, 
         m_ctrack_list_ctrl.GetItemState(item,LVIS_SELECTED));
   }

	if (m_ctrack_list_ctrl.GetSelectedCount() == 0)
	{
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_CENTER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_STOP)->EnableWindow();
		GetDlgItem(IDC_CENTER)->EnableWindow();
		GetDlgItem(IDC_EDIT)->EnableWindow();
		GetDlgItem(IDC_DELETE)->EnableWindow();
	}

	*pResult = 0;
}

// hide the selected coast track from the dialog (but do not remove it)
void CoastTrackPage::hide_coast_track(int index)
{
   
}

// handle the OnSize message received from the GPSToolPropertySheet
// for this property page
void CoastTrackPage::on_size(UINT nType, int cx, int cy)
{
   CPropertyPage::OnSize(nType, cx, cy);

   // adjust the size of the list control
   if (m_is_initialized)
   {
      int margin = 8; 
      
      CRect rect;
      GetWindowRect(&rect);
      rect.DeflateRect(margin, margin);
      
      // align the button controls
      CAlignWnd buttonset;
      buttonset.Add(GetDlgItem(IDC_ADD)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_STOP)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_CENTER)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_EDIT)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_DELETE)->m_hWnd);
      
      buttonset.Align(m_hWnd, rect, ALIGN_RIGHT|ALIGN_TOP);
      
      // align the timeout controls
      CAlignWnd timeout_ctrl_set;
      timeout_ctrl_set.Add(GetDlgItem(IDC_UPDATE_TXT1)->m_hWnd);
      timeout_ctrl_set.Add(GetDlgItem(IDC_UPDATE_RATE)->m_hWnd);
      timeout_ctrl_set.Add(GetDlgItem(IDC_UPDATE_TXT2)->m_hWnd);
      
      timeout_ctrl_set.Align(m_hWnd, rect, ALIGN_LEFT|ALIGN_BOTTOM);
      
      CRect buttonsbox;
      buttonset.GetBounds(&buttonsbox);
      //rect.top += buttonsbox.Height() + 10;
      rect.right -= buttonsbox.Width();
      rect.right -= margin;
      rect.bottom -= 30;
      
      // stretch the list control based upon the current property page size
      CAlignWnd align_wnd;
      align_wnd.Add(m_ctrack_list_ctrl.m_hWnd);
      align_wnd.StretchXY(m_ctrack_list_ctrl.m_hWnd, rect);
   }

   // Adjust column widths if fully created
   if (m_is_initialized)
   {
      //const int columnmargin = 4;
      CRect list_ctrl_rect;
      m_ctrack_list_ctrl.GetClientRect(&list_ctrl_rect);
      
      double width = (double)list_ctrl_rect.Width();
      
      // Size each column to be a percentage of the control width
      m_ctrack_list_ctrl.SetColumnWidth(0, (int)(width * 0.15));
      m_ctrack_list_ctrl.SetColumnWidth(1, (int)(width * 0.30));
      m_ctrack_list_ctrl.SetColumnWidth(2, (int)(width * 0.10));
      m_ctrack_list_ctrl.SetColumnWidth(3, (int)(width * 0.10));
      m_ctrack_list_ctrl.SetColumnWidth(4, (int)(width * 0.175));
      m_ctrack_list_ctrl.SetColumnWidth(5, (int)(width * 0.175));
   }

   Invalidate();
}

// return the minimum rectangle that this property page can be
void CoastTrackPage::GetMinimumRect(CRect &minrect)
{
   minrect.TopLeft() = CPoint(0,0);
   
   CAlignWnd buttonset;
   buttonset.Add(GetDlgItem(IDC_ADD)->m_hWnd);
   buttonset.Add(GetDlgItem(IDC_STOP)->m_hWnd);
   buttonset.Add(GetDlgItem(IDC_CENTER)->m_hWnd);
   buttonset.Add(GetDlgItem(IDC_EDIT)->m_hWnd);
   buttonset.Add(GetDlgItem(IDC_DELETE)->m_hWnd);
   
   CRect buttonsbox;
   buttonset.GetBounds(&buttonsbox);
   
   minrect.BottomRight() = CPoint(250, buttonsbox.Height() + 40);
}

/////////////////////////////////////////////////////////////////////////////
// CoastTrackDataDialog dialog


CoastTrackDataDialog::CoastTrackDataDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CoastTrackDataDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CoastTrackDataDialog)
	m_vector_on = FALSE;
   m_initial_traj_on = FALSE;
	m_disttime = 0.0;
	m_heading = 0;
	m_velocity = 0.0;
	//}}AFX_DATA_INIT

   m_ctrack_data = new CoastTrackData;

   CoastTrackManager *m_coast_track_mgr = NULL;
   int m_index = -1;
   boolean_t m_already_added = TRUE;
   boolean_t m_is_modified = FALSE;
   m_modified_report = FALSE;
}

CoastTrackDataDialog::~CoastTrackDataDialog()
{
   delete m_ctrack_data;
}


void CoastTrackDataDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CoastTrackDataDialog)
	DDX_Control(pDX, IDC_VECTOR_ON, m_vector_on_check);
   DDX_Control(pDX, IDC_INITIAL_TRAJ, m_initial_traj_on_check);
	DDX_Control(pDX, IDC_DISTTIME_UNITS, m_disttime_units);
	DDX_Control(pDX, IDC_LABEL, m_label_editctrl);
	DDX_Control(pDX, IDC_VELOCITY_UNITS, m_velocity_units);
	DDX_Control(pDX, IDC_VELOCITY, m_velocity_editctrl);
	DDX_Control(pDX, IDC_HEADING, m_heading_editctrl);
	DDX_Control(pDX, IDC_DISTTIME_EDIT, m_disttime_editctrl);
	DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geoctrl);
	DDX_Check(pDX, IDC_VECTOR_ON, m_vector_on);
   DDX_Check(pDX, IDC_INITIAL_TRAJ, m_initial_traj_on);
   DDX_Control(pDX, IDC_RHUMBLINE, m_rhumbline_radio);
   DDX_Control(pDX, IDC_GREAT_CIRCLE, m_greatcircle_radio);
   DDX_Control(pDX, IDC_DISTANCE, m_distance_radio);
   DDX_Control(pDX, IDC_TIME, m_time_radio);
	DDX_Text(pDX, IDC_DISTTIME_EDIT, m_disttime);
	DDV_MinMaxDouble(pDX, m_disttime, 0., 9999999.);
	DDX_Text(pDX, IDC_HEADING, m_heading);
	DDV_MinMaxInt(pDX, m_heading, 1, 360);
	DDX_Text(pDX, IDC_VELOCITY, m_velocity);
	DDV_MinMaxDouble(pDX, m_velocity, 0., 99999.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CoastTrackDataDialog, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CoastTrackDataDialog)
	ON_BN_CLICKED(IDC_GET_FROM_MAP, OnGetPositionFromMap)
	ON_BN_CLICKED(IDC_RHUMBLINE, OnRhumbline)
	ON_BN_CLICKED(IDC_GREAT_CIRCLE, OnGreatCircle)
	ON_EN_CHANGE(IDC_LABEL, OnChangeLabel)
	ON_BN_CLICKED(IDC_VECTOR_ON, OnVectorOn)
   ON_BN_CLICKED(IDC_INITIAL_TRAJ, OnInitialTraj)
	ON_BN_CLICKED(IDC_DISTANCE, OnDistance)
	ON_BN_CLICKED(IDC_TIME, OnTime)
	ON_EN_KILLFOCUS(IDC_DISTTIME_EDIT, OnKillfocusDisttimeEdit)
	ON_CBN_SELCHANGE(IDC_DISTTIME_UNITS, OnSelchangeDisttimeUnits)
	ON_EN_KILLFOCUS(IDC_HEADING, OnKillfocusHeading)
	ON_CBN_SELCHANGE(IDC_VELOCITY_UNITS, OnSelchangeVelocityUnits)
	ON_EN_KILLFOCUS(IDC_VELOCITY, OnKillfocusVelocity)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CoastTrackDataDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CoastTrackDataDialog)
	ON_EVENT(CoastTrackDataDialog, IDC_GEOCX1CTRL1, 1 /* OCXKILLFOCUS */, OnGeocxKillFocus, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CoastTrackDataDialog message handlers

void CoastTrackDataDialog::OnGeocxKillFocus()
{
	d_geo_t geo;
   geo.lat = m_geoctrl.GetLatitude();
   geo.lon = m_geoctrl.GetLongitude(); 

	if (m_ctrack_data->get_last_report().m_initial_position.lat != geo.lat ||
		 m_ctrack_data->get_last_report().m_initial_position.lon != geo.lon)
   {
      m_modified_report = TRUE;
      
      // if the velocity of the report has changed and we have not yet
      // already added a report then go ahead and add it now
      if (!m_report_added)
      {
         ReportData &old_report = m_ctrack_data->get_last_report();
         ReportData &new_report = m_ctrack_data->add_report();
         m_report_added = TRUE;

         // copy over the old heading and initial position
         new_report.m_heading = old_report.m_heading;
         new_report.m_velocity = old_report.m_velocity;
      }
      
      m_ctrack_data->get_last_report().m_initial_position = geo;
   }

	GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

BOOL CoastTrackDataDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bitmap_button.AutoLoad(IDC_GET_FROM_MAP, this);
	m_bitmap_button.LoadMapped();

   // report data vars
   ReportData &report_data = m_ctrack_data->get_last_report();

   // initialize the geocx position control
   {
      // try using the current position of the coast track for an initial value
      // otherwise use the initial position of the last report.
      //m_geo_lat = m_ctrack_data->get_current_position().lat;
      //m_geo_lon = m_ctrack_data->get_current_position().lon;
      //if (!GEO_valid_degrees(m_geo_lat, m_geo_lon))
      {
         m_geo_lat = report_data.m_initial_position.lat;
         m_geo_lon = report_data.m_initial_position.lon;
      }

      // get the default datum, so it can be included with the location string
      const int DATUM_LEN = 8;
      char datum[DATUM_LEN];
      GEO_get_default_datum(datum, DATUM_LEN);
      m_geoctrl.SetDatumText(datum);
      
      // initialize location in the OCX Control (GEOCX)
      m_geoctrl.SetGeocx(m_geo_lat, m_geo_lon);
      
      // convert lat-lon to formated string in default format and datum
      const int STR_LEN = 256;
      char latlon_string[STR_LEN];
      GEO_lat_lon_to_string(m_geo_lat, m_geo_lon, latlon_string, STR_LEN);
      m_geoctrl.SetLocationText(latlon_string);
   }

   // initialize the projected heading buttons
   {
      GeoSegment::heading_type_t rhumbline_heading = 
         m_ctrack_data->get_projected_heading();

      m_rhumbline_radio.SetCheck(rhumbline_heading == GeoSegment::RHUMB_LINE);
      m_greatcircle_radio.SetCheck(rhumbline_heading != GeoSegment::RHUMB_LINE);
   }

   // initialize the label edit control
   m_label_editctrl.SetWindowText(m_ctrack_data->get_label());

   // initialize the course vector group
   update_course_vector();

   // initialize the heading
   m_heading = (int)report_data.m_heading;

   // initialize the velocity / units
   update_velocity();

   // initialize the initial trajectory on/off
   m_initial_traj_on_check.SetCheck(m_ctrack_data->get_initial_traj_on());
   m_initial_traj_on = m_ctrack_data->get_initial_traj_on();

   UpdateData(FALSE);

   GetDlgItem(IDC_APPLY)->EnableWindow(m_modified_report);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// returns a CoastTrackData object with the dialog data
CoastTrackData &CoastTrackDataDialog::GetData()
{
   CoastTrackData &data = *m_ctrack_data;
   ReportData &report_data = data.get_last_report();
   
   report_data.m_initial_position.lat = m_geo_lat;
   report_data.m_initial_position.lon = m_geo_lon;
   
   // set the starting time of this newly constructed coast track
	CTime time = CTime::GetCurrentTime();
	tm t1;
   time.GetGmtTm(&t1);
	report_data.m_time_started.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
		t1.tm_min, t1.tm_sec);

   COleDateTime time_stopped;
   time_stopped.SetStatus(COleDateTime::null);
   data.set_time_stopped(time_stopped);
   
   return data;
}

// set the dialog with the given CoastTrackData object
void CoastTrackDataDialog::SetData(CoastTrackData &data)
{
   *m_ctrack_data = data;
}

void CoastTrackDataDialog::OnGetPositionFromMap() 
{
   m_geo_lat = m_geoctrl.GetLatitude();
   m_geo_lon = m_geoctrl.GetLongitude();

   // we need to temporarily hide this modal dialog box, change
   // the cursor, and retreive a latitude longitude pair from the
   // map
   EndDialog(GET_POSITION_FROM_MAP);
}

void CoastTrackDataDialog::OnRhumbline() 
{
   m_ctrack_data->set_projected_heading(GeoSegment::RHUMB_LINE);
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::OnGreatCircle() 
{
   m_ctrack_data->set_projected_heading(GeoSegment::GREAT_CIRCLE);
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::OnChangeLabel() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
   m_label_editctrl.GetWindowText(m_ctrack_data->get_label());
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::OnVectorOn() 
{
   boolean_t on = m_vector_on_check.GetCheck();
   m_ctrack_data->get_course_vector().m_on = on;

   update_course_vector();
   UpdateData(FALSE);

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::OnInitialTraj()
{
   boolean_t on = m_initial_traj_on_check.GetCheck();
   m_ctrack_data->set_initial_traj_on(on);
   m_initial_traj_on = on;

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

// updates the course vector group based on the current coast track 
// data in m_ctrack_data
void CoastTrackDataDialog::update_course_vector()
{
   CourseVector &cvector = m_ctrack_data->get_course_vector();

   if (cvector.m_on)
   {
      m_vector_on_check.SetCheck(1);
      m_vector_on = TRUE;

      // enable the distance/time controls when the course vector check
      // box is selected
      m_distance_radio.EnableWindow();
      m_time_radio.EnableWindow();
      m_disttime_editctrl.EnableWindow();
      m_disttime_units.EnableWindow();

      // remove any units from the disttime units combo box
      m_disttime_units.ResetContent();
      
      if (cvector.m_is_dist)
      {
         CUnitConverter units;

         m_distance_radio.SetCheck(1);
         m_time_radio.SetCheck(0);

         // set the units
         units.SetMeters(cvector.m_distance);
         switch (cvector.m_units_dist)
         {
         case NM: m_disttime = units.GetNM(); break;
         case KM: m_disttime = units.GetMeters() / 1000.0; break;
         case M: m_disttime = units.GetMeters(); break;
         case YARDS: m_disttime = units.GetYards(); break;
         case FEET: m_disttime = units.GetFeet(); break;
         }

         // add the units pertinent to distance to the disttime combo
         m_disttime_units.InsertString(-1, "NM");
         m_disttime_units.InsertString(-1, "km");
         m_disttime_units.InsertString(-1, "meters");
         m_disttime_units.InsertString(-1, "yards");
         m_disttime_units.InsertString(-1, "feet");

         // select the units stored in the cvector 
         m_disttime_units.SetCurSel(enum_to_comboSel(cvector.m_units_dist));
      }
      else
      {
         m_distance_radio.SetCheck(0);
         m_time_radio.SetCheck(1);

         switch (cvector.m_units_time)
         {
         case SECONDS: m_disttime = cvector.m_time; break;
         case MINUTES: m_disttime = cvector.m_time / 60.0; break;
         }

         // add the units pertinent to time to the disttime combo
         m_disttime_units.InsertString(-1, "seconds");
         m_disttime_units.InsertString(-1, "minutes");

         // select the units stored in the cvector 
         m_disttime_units.SetCurSel(cvector.m_units_time);
      }
   }
   else
   {
      m_vector_on_check.SetCheck(0);
      m_vector_on = FALSE;

      // disable the distance/time controls when the course vector check
      // box is not selected
      m_distance_radio.EnableWindow(FALSE);
      m_time_radio.EnableWindow(FALSE);
      m_disttime_editctrl.EnableWindow(FALSE);
      m_disttime_units.EnableWindow(FALSE);

      // remove any units from the disttime units combo box
      m_disttime_units.ResetContent();
   }
}

void CoastTrackDataDialog::OnDistance() 
{
	m_ctrack_data->get_course_vector().m_is_dist = TRUE;
   update_course_vector();
   UpdateData(FALSE);

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::OnTime() 
{
	m_ctrack_data->get_course_vector().m_is_dist = FALSE;
   update_course_vector();
   UpdateData(FALSE);

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::OnKillfocusDisttimeEdit() 
{
   UpdateData();

   // get the value in the dist/time edit control based on which radio
   // button is selected (either the distance or time)
   if (m_distance_radio.GetCheck())
   {
      CUnitConverter units;
   
      switch (comboSel_to_enum(m_disttime_units.GetCurSel()))
      {
      case NM: units.SetNM(m_disttime); break;
      case KM: units.SetMeters(m_disttime * 1000.0); break;
      case M: units.SetMeters(m_disttime); break;
      case YARDS: units.SetYards(m_disttime); break;
      case FEET: units.SetFeet(m_disttime); break;
      }

      m_ctrack_data->get_course_vector().m_distance = units.GetMeters();
   }
   else
   {
      switch (m_disttime_units.GetCurSel())
      {
      case 0: m_ctrack_data->get_course_vector().m_time = m_disttime; break;
      case 1: m_ctrack_data->get_course_vector().m_time = m_disttime * 60.0; break;
      }

      m_ctrack_data->get_course_vector().m_time = m_disttime;
   }

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::OnSelchangeDisttimeUnits() 
{
   if (m_distance_radio.GetCheck())
   {
      m_ctrack_data->get_course_vector().m_units_dist = 
         comboSel_to_enum(m_disttime_units.GetCurSel());
      update_course_vector();
      UpdateData(FALSE);
   }
   else
   {
      m_ctrack_data->get_course_vector().m_units_time = (units_t)m_disttime_units.GetCurSel();
      update_course_vector();
      UpdateData(FALSE);
   }

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

units_t CoastTrackDataDialog::comboSel_to_enum(int index)
{
   switch (index)
   {
   case 0: return NM;
   case 1: return KM;
   case 2: return M;
   case 3: return YARDS;
   case 4: return FEET;
   }

   return M;
}

int CoastTrackDataDialog::enum_to_comboSel(units_t units)
{
   switch (units)
   {
   case NM: return 0;
   case KM: return 1;
   case M: return 2;
   case YARDS: return 3;
   case FEET: return 4;
   }  

   return 2;
}

void CoastTrackDataDialog::OnKillfocusHeading() 
{
   UpdateData();
   
   if (m_ctrack_data->get_last_report().m_heading != (double)m_heading)
   {
      m_modified_report = TRUE;
      
      // if the heading of the report has changed and we have not yet
      // already added a report then go ahead and add it now
      if (!m_report_added)
      {
         ReportData &old_report = m_ctrack_data->get_last_report();
         ReportData &new_report = m_ctrack_data->add_report();
         m_report_added = TRUE;
         
         // copy over the old velocity and initial position
         new_report.m_velocity = old_report.m_velocity;
         new_report.m_initial_position = old_report.m_initial_position;
      }
      
      m_ctrack_data->get_last_report().m_heading = (double)m_heading;
   }

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::update_velocity()
{
   // get velocity in ft/s from structure
   double velocity = m_ctrack_data->get_last_report().m_velocity;

   switch(m_ctrack_data->get_units_velocity())
   {
      // ft/s -> knots
   case KNOTS: m_velocity = FEET_PER_S_TO_KNOTS(velocity); break;
      // ft/s -> km/hr
   case KM_PER_H: m_velocity = (FEET_TO_METERS(velocity)/1000.0)/
              (1/60.0*1/60.0); break;
   }

   m_velocity_units.SetCurSel(m_ctrack_data->get_units_velocity());
}

void CoastTrackDataDialog::OnSelchangeVelocityUnits() 
{
	m_ctrack_data->set_units_velocity(m_velocity_units.GetCurSel());

   update_velocity();
   UpdateData(FALSE);

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void CoastTrackDataDialog::OnKillfocusVelocity() 
{
   UpdateData();

   double velocity;

   switch (m_velocity_units.GetCurSel())
   {
      // knots -> ft/s
   case 0: velocity = KNOTS_TO_FEET_PER_S(m_velocity); break;
      // km/hr -> ft/s
   case 1: velocity = METERS_TO_FEET(m_velocity*1000.0)/(60.0*60.0); break;
   }
   
   if (m_ctrack_data->get_last_report().m_velocity != velocity)
   {
      m_modified_report = TRUE;
      
      // if the velocity of the report has changed and we have not yet
      // already added a report then go ahead and add it now
      if (!m_report_added)
      {
         ReportData &old_report = m_ctrack_data->get_last_report();
         ReportData &new_report = m_ctrack_data->add_report();
         m_report_added = TRUE;

         // copy over the old heading and initial position
         new_report.m_heading = old_report.m_heading;
         new_report.m_initial_position = old_report.m_initial_position;
      }
      
      m_ctrack_data->get_last_report().m_velocity = velocity;
   }

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

// apply the coast track data (we need to know if we are modifying a 
// track or if we are adding a new one)
void CoastTrackDataDialog::OnApply() 
{
   d_geo_t geo;
   geo.lat = m_geoctrl.GetLatitude();
   geo.lon = m_geoctrl.GetLongitude(); 

   // if the initial position of the report has changed and we have not yet
   // already added a report then go ahead and add it now
   if (!m_report_added && (geo.lat != m_geo_lat || geo.lon != m_geo_lon))
   {
      m_geo_lat = geo.lat;
      m_geo_lon = geo.lon;

      ReportData &old_report = m_ctrack_data->get_last_report();
      ReportData &new_report = m_ctrack_data->add_report();
      m_report_added = TRUE;

      new_report.m_heading = old_report.m_heading;
      new_report.m_velocity = old_report.m_velocity;
      new_report.m_initial_position = geo;
   }
	else
	{
		m_geo_lat = geo.lat;
		m_geo_lon = geo.lon;
	}

   CoastTrackData &ctrack_data = GetData();
   
   if (!m_already_added)
   {
      // add the coast track with the given coast track data
      m_index = m_coast_track_mgr->add(ctrack_data);

      m_already_added = TRUE;
   }
   else
   {
      // modify the coast track with the given coast track data
      m_coast_track_mgr->modify(m_index, ctrack_data, m_modified_report);
      
      // update the coast track to the dialog, if it is open
      if (C_gps_trail::is_gps_tools_opened())
      {
         CoastTrackPage *ctrack_page = 
            (CoastTrackPage *)C_gps_trail::m_gpstools_dialog.GetPage(PAGE_INDEX_COAST_TRACK);
         ctrack_page->update_coast_track(m_index, &ctrack_data);
      } 
   }

   m_modified_report = FALSE;
   GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}

void CoastTrackDataDialog::OnOK() 
{
   d_geo_t geo;
   geo.lat = m_geoctrl.GetLatitude();
   geo.lon = m_geoctrl.GetLongitude(); 
   
   // if the initial position of the report has changed and we have not yet
   // already added a report then go ahead and add it now
   if (!m_report_added && (geo.lat != m_geo_lat || geo.lon != m_geo_lon))
   {
      ReportData &old_report = m_ctrack_data->get_last_report();
      ReportData &new_report = m_ctrack_data->add_report();
      m_report_added = TRUE;
      
      new_report.m_heading = old_report.m_heading;
      new_report.m_velocity = old_report.m_velocity;
      new_report.m_initial_position = geo;
   }

	m_geo_lat = geo.lat;
   m_geo_lon = geo.lon;
   
   CDialog::OnOK();
}

void CoastTrackPage::OnChangeUpdateRate() 
{
   UpdateData();

	m_coast_track_mgr->set_update_rate(m_update_rate);
}

BOOL CoastTrackPage::OnSetActive() 
{
	BOOL status = CPropertyPage::OnSetActive();
	on_size(0,0,0);
	return status;
}

// override LoadBitmaps so it maps the bitmaps colors
// to the current system colors
BOOL CMappedBitmapButton::LoadMapped()
{
	m_bitmap.DeleteObject();
	m_bitmapSel.DeleteObject();
	m_bitmap.LoadMappedBitmap(IDB_GETPOS_UP);
	m_bitmapSel.LoadMappedBitmap(IDB_GETPOS_DOWN);

	return TRUE;
}
LRESULT CoastTrackDataDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

