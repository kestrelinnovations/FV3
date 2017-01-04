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



// rbdlg.cpp : implementation file
//

#include "stdafx.h"
#include "rbdlg.h"
#include "err.h"
#include "Utility\AlignWnd.h"  // for CAlignWnd
#include "gpstools.h"
#include "rblocdlg.h"	// for LocationDlg
#include "csttrack.h"
#include "..\mainfrm.h"
#include "factory.h"
#include "..\getobjpr.h"
#include "..\PlaybackDialog\viewtime.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// RangeBearingPage property page

IMPLEMENT_DYNCREATE(RangeBearingPage, CPropertyPage)

RangeBearingPage::RangeBearingPage() : CPropertyPage(RangeBearingPage::IDD)
{
	//{{AFX_DATA_INIT(RangeBearingPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_is_initialized = FALSE;
   m_rb_mgr = NULL;
}

RangeBearingPage::RangeBearingPage(RangeBearingManager *rb_mgr) :
CPropertyPage(RangeBearingPage::IDD)
{
   m_is_initialized = FALSE;
   m_rb_mgr = rb_mgr;
}

RangeBearingPage::~RangeBearingPage()
{
}

void RangeBearingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RangeBearingPage)
	DDX_Control(pDX, IDC_LIST, m_list_ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RangeBearingPage, CPropertyPage)
	//{{AFX_MSG_MAP(RangeBearingPage)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDoubleClickList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RangeBearingPage message handlers

BOOL RangeBearingPage::OnInitDialog()
{
   LV_COLUMN lc;

   CPropertyPage::OnInitDialog();

	// we want the list control to have full row select
	ListView_SetExtendedListViewStyle(m_list_ctrl.GetSafeHwnd(), LVS_EX_FULLROWSELECT);

   // initialize lc common entries
	memset(&lc, 0, sizeof(LV_COLUMN));
	lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lc.fmt = LVCFMT_LEFT;

   // initialize the Label column
   lc.cx = 80;
   lc.pszText = "Label";
   lc.iSubItem = 0;
   if (m_list_ctrl.InsertColumn(0, &lc) != 0)
      ERR_report("InsertColumn(0, &lc) failed.");

   // initialize the Course column
   lc.cx = 135;
   lc.pszText = "Course";
   lc.iSubItem = 1;
   if (m_list_ctrl.InsertColumn(1, &lc) != 1)
      ERR_report("InsertColumn(1, &lc) failed.");

   // initialize the Range column
   lc.cx = 135;
   lc.pszText = "Range";
   lc.iSubItem = 2;
   if (m_list_ctrl.InsertColumn(2, &lc) != 2)
      ERR_report("InsertColumn(2, &lc) failed.");

   // initialize the Time Started column
   lc.cx = 85;
   lc.pszText = "Time Started";
   lc.iSubItem = 3;
   if (m_list_ctrl.InsertColumn(3, &lc) != 3)
      ERR_report("InsertColumn(3, &lc) failed.");

   // initialize the Time Stopped column
   lc.cx = 93;
   lc.pszText = "Time Stopped";
   lc.iSubItem = 4;
   if (m_list_ctrl.InsertColumn(4, &lc) != 4)
      ERR_report("InsertColumn(4, &lc) failed.");

   m_list_ctrl.SetBkColor(RGB(0,0,0));
   m_list_ctrl.SetTextColor(RGB(255,255,255));
   m_list_ctrl.SetTextBkColor(RGB(0,0,0));

   m_rb_mgr->initialize_dialog();

   m_is_initialized = TRUE;

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RangeBearingPage::on_size(UINT nType, int cx, int cy)
{
   CPropertyPage::OnSize(nType, cx, cy);
   
   // adjust the size of the list control
   if (m_is_initialized)
   {
      int margin = 8; 
      
      CRect rect;
      GetWindowRect(&rect);
      rect.DeflateRect(margin, margin);
      
      CAlignWnd buttonset;
      buttonset.Add(GetDlgItem(IDC_ADD)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_VIEW)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_STOP)->m_hWnd);
		buttonset.Add(GetDlgItem(IDC_EDIT)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_DELETE)->m_hWnd);
      
      buttonset.Align(m_hWnd, rect, ALIGN_RIGHT|ALIGN_TOP);
      
      CRect buttonsbox;
      buttonset.GetBounds(&buttonsbox);
      //rect.top += buttonsbox.Height() + 10;
      rect.right -= buttonsbox.Width();
      rect.right -= margin;
      
      // stretch the list control based upon the current property page size
      CAlignWnd align_wnd;
      align_wnd.Add(m_list_ctrl.m_hWnd);
      align_wnd.StretchXY(m_list_ctrl.m_hWnd, rect);
   }
   
   // Adjust column widths if fully created
   if (m_is_initialized)
   {
      //const int columnmargin = 4;
      CRect list_ctrl_rect;
      m_list_ctrl.GetClientRect(&list_ctrl_rect);
      
      double width = (double)list_ctrl_rect.Width();
      
      // Size each column to be a percentage of the control width
      m_list_ctrl.SetColumnWidth(0, (int)(width * 0.25));
      m_list_ctrl.SetColumnWidth(1, (int)(width * 0.20));
      m_list_ctrl.SetColumnWidth(2, (int)(width * 0.20));
      m_list_ctrl.SetColumnWidth(4, (int)(width * 0.175));
      m_list_ctrl.SetColumnWidth(5, (int)(width * 0.175));
   }
   
   Invalidate();
}

// return the minimum rectangle that this property page can be
void RangeBearingPage::GetMinimumRect(CRect &minrect)
{
   minrect.TopLeft() = CPoint(0,0);
   
   CAlignWnd buttonset;
   buttonset.Add(GetDlgItem(IDC_ADD)->m_hWnd);
   buttonset.Add(GetDlgItem(IDC_VIEW)->m_hWnd);
   buttonset.Add(GetDlgItem(IDC_STOP)->m_hWnd);
	buttonset.Add(GetDlgItem(IDC_EDIT)->m_hWnd);
   buttonset.Add(GetDlgItem(IDC_DELETE)->m_hWnd);
   
   CRect buttonsbox;
   buttonset.GetBounds(&buttonsbox);
   
   minrect.BottomRight() = CPoint(250, buttonsbox.Height());
}

// add a range bearing object to the list
void RangeBearingPage::OnAdd()
{
   m_rb_mgr->add();
}

// stop any selected range/bearing objects
void RangeBearingPage::OnStop() 
{
   int item_index = -1;
   do
   {
      item_index = m_list_ctrl.GetNextItem(item_index,LVNI_SELECTED);
      
      if (item_index != -1)
         m_rb_mgr->stop(item_index);
      
   } while (item_index != -1);
}

void RangeBearingPage::OnEdit()
{
	int item_index = -1;
	item_index = m_list_ctrl.GetNextItem(item_index, LVNI_SELECTED);

	if (item_index != -1)
	{
		m_rb_mgr->modify(item_index);
	}
}

// OnDelete - handle the deletion of selected range and bearing
// objects
void RangeBearingPage::OnDelete() 
{
   if (AfxMessageBox("Are you sure you want to permanently remove the selected "
      "range/bearing object(s)?", MB_YESNO) == IDNO)
      return;

	const int num_selected = m_list_ctrl.GetSelectedCount();
	int *item = NULL;
	
	// sanity check before allocation
	if (num_selected > 0)
		item = new int[num_selected];
	else
		return;

	// store the list of items that were selected so we can 
	// delete them in reverse order
	int i = 0;
	POSITION position = m_list_ctrl.GetFirstSelectedItemPosition();
	while (position)
   {
      item[i++] = m_list_ctrl.GetNextSelectedItem(position);
	}
   
	// delete the items in reverse order
	while (i)
		m_rb_mgr->delete_obj(item[--i]);

	delete [] item;
}

void RangeBearingPage::OnView()
{
   int item_index = -1;
   do
   {
      item_index = m_list_ctrl.GetNextItem(item_index,LVNI_SELECTED);
      
      if (item_index != -1)
         m_rb_mgr->view(item_index);
      
   } while (item_index != -1);
}

void RangeBearingPage::OnDoubleClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CPoint point;

   // get the current cursor location in screen coordinates
   GetCursorPos(&point);

   // convert the cursor location to the coordinates of the list control
   m_list_ctrl.ScreenToClient(&point);

   // get the index of the item that was double clicked, if any
   int index = m_list_ctrl.HitTest(point);
   if (index == -1)
   {
      *pResult = SUCCESS;
      return;
   }

   // set the selected entry and the focus on the current list control
   SetFocus();
   m_list_ctrl.SetItemState(index, 
      LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	m_rb_mgr->modify(index);

	*pResult = SUCCESS;
}

void RangeBearingPage::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_list_ctrl.GetSelectedCount() == 0)
	{
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_VIEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_STOP)->EnableWindow();
		GetDlgItem(IDC_VIEW)->EnableWindow();
		GetDlgItem(IDC_EDIT)->EnableWindow();
		GetDlgItem(IDC_DELETE)->EnableWindow();
	}

	*pResult = 0;
}
      

// add - adds a range and bearing object to the list control with
// the given r/b data.  Returns the index into the list
// control where the range and bearing was added
int RangeBearingPage::add(RangeBearingData *data)
{
   LV_ITEM lv;
   
   if (!IsWindow(m_list_ctrl))
      return -1;
   
   // clear all fields of LV_ITEM structure
   memset(&lv, 0, sizeof(LV_ITEM));
   
   // the item index will be the current number of elements in the list
   // control since the items are zero-indexed
   int item_index = m_list_ctrl.GetItemCount();
   
   // set the common fields
   lv.iItem = item_index;
   lv.mask = LVIF_TEXT | LVIF_PARAM;
   
   m_list_ctrl.InsertItem(&lv);
   
   update(item_index, data);
   
   return item_index;
}

// update - updates a range and bearing object with the given index
// into the list control with the given data
void RangeBearingPage::update(int item_index, RangeBearingData *data)
{
   LV_ITEM lv;

   if (!IsWindow(m_list_ctrl))
      return;
   
   // clear all fields of LV_ITEM structure
   memset(&lv, 0, sizeof(LV_ITEM));
   
   // set the common fields
   lv.iItem = item_index;
   lv.mask = LVIF_TEXT | LVIF_PARAM;
   
   // set the label field
   lv.mask = LVIF_TEXT;
   lv.lParam = item_index;
   lv.iSubItem = 0;
   CString label_txt;
   // if a label is specified then use this label
   if (data->m_label != "")
   {
      lv.pszText = data->m_label.GetBuffer(256);
   }
   // otherwise, the default name will be TRACKx where x is the 
   // row number in the list control (since zero-indexed we need
   // to add one
   else
   {
      label_txt.Format("TRACK%d", item_index + 1);
      lv.pszText = label_txt.GetBuffer(256);
   }
   m_list_ctrl.SetItem(&lv);
   
   // set the course field
   lv.iSubItem = 1;
   CString heading_txt;
   heading_txt.Format("%.1f°",data->m_bearing);
   lv.pszText = heading_txt.GetBuffer(256);
   
   // add the leading zeros, heading should be of the form xxx.x
   while(heading_txt.GetLength() < 6)
      heading_txt = "0" + heading_txt;
   // add the mag/true specification
   if (data->m_mag)
      heading_txt += "M";
   else
      heading_txt += "T";
   lv.pszText = heading_txt.GetBuffer(256);
   m_list_ctrl.SetItem(&lv);

   // set the range (stored in meters)
   lv.iSubItem = 2;
   CString range_txt;

   // convert to NM or km as appropriate
   double range;
   if (data->m_units_nm)
      range = METERS_TO_NM(data->m_range);
   else
      range = data->m_range / 1000;

   range_txt.Format("%0.2f", range);
   
   // append the units
   if (data->m_units_nm)
      range_txt += "NM";
   else
      range_txt += "km";

   lv.pszText = range_txt.GetBuffer(256);
   m_list_ctrl.SetItem(&lv);
   
   // set the time start field
   lv.iSubItem = 3;
   CString time_started_txt = data->m_time_started.Format("%H:%M:%S");
   lv.pszText = time_started_txt.GetBuffer(256);
   m_list_ctrl.SetItem(&lv);
   
   // set the time end field
   lv.iSubItem = 4;
   CString time_stopped_txt = data->m_time_stopped.Format("%H:%M:%S");
   if (data->m_time_stopped.GetStatus() == COleDateTime::null)
      time_stopped_txt = "N/A";
   lv.pszText = time_stopped_txt.GetBuffer(256);
   m_list_ctrl.SetItem(&lv);
}

// delete a range/bearing object from the list control with the given index
void RangeBearingPage::delete_obj(int index)
{
   m_list_ctrl.DeleteItem(index);
}

BOOL RangeBearingPage::OnSetActive() 
{
	BOOL status = CPropertyPage::OnSetActive();
	on_size(0,0,0);
	return status;
}

/////////////////////////////////////////////////////////////////////////////
// RangeBearingDataDialog dialog

RangeBearingDataDialog::RangeBearingDataDialog(CWnd* pParent /*=NULL*/)
	: CDialog(RangeBearingDataDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(RangeBearingDataDialog)
	//}}AFX_DATA_INIT

   m_rb_mgr = NULL;

   //int m_index = -1;
   boolean_t m_already_added = TRUE;
   boolean_t m_is_modified = FALSE;
	m_snap_to_valid = FALSE;
	m_type = 0;
   //m_modified_report = FALSE;
}

RangeBearingDataDialog::~RangeBearingDataDialog()
{
}

void RangeBearingDataDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RangeBearingDataDialog)
	DDX_Control(pDX, IDC_LABEL, m_label_editctrl);

   DDX_Control(pDX, IDC_COURSE_TRUE, m_course_true);
   DDX_Control(pDX, IDC_COURSE_MAG, m_course_mag);
   DDX_Control(pDX, IDC_UNITS_NM, m_units_nm);
   DDX_Control(pDX, IDC_UNITS_KM, m_units_km);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(RangeBearingDataDialog, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(RangeBearingDataDialog)
   ON_BN_CLICKED(IDC_SNAP, on_modified)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_FROM, OnFrom)
	ON_BN_CLICKED(IDC_TO, OnTo)
   ON_EN_CHANGE(IDC_LABEL, on_modified)
   ON_BN_CLICKED(IDC_COURSE_TRUE, on_modified)
	ON_BN_CLICKED(IDC_COURSE_MAG, on_modified)
   ON_BN_CLICKED(IDC_UNITS_NM, on_modified)
	ON_BN_CLICKED(IDC_UNITS_KM, on_modified)
	ON_BN_CLICKED(IDC_SWAP, OnSwap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL RangeBearingDataDialog::OnInitDialog()
{
   CDialog::OnInitDialog();
   
   // initialize the course type (mag/true)
   m_course_mag.SetCheck(m_data.m_mag);
   m_course_true.SetCheck(!m_data.m_mag);
   
   // initialize the units type (nm/km)
   m_units_nm.SetCheck(m_data.m_units_nm);
   m_units_km.SetCheck(!m_data.m_units_nm);

   // initialize the label edit control
   m_label_editctrl.SetWindowText(m_data.m_label);

	// initialize the from/to static edit controls
	update_from_to_text();
   
   UpdateData(FALSE);
   
   GetDlgItem(IDC_APPLY)->EnableWindow(m_is_modified);

	if (m_type == 1 /*FROM*/)
		OnFrom();
	else if (m_type == 2 /*TO*/)
		OnTo();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
	
void RangeBearingDataDialog::on_modified()
{
   UpdateData();

   m_label_editctrl.GetWindowText(m_data.m_label);
   m_data.m_mag = m_course_mag.GetCheck();
   m_data.m_units_nm = m_units_nm.GetCheck();

   m_is_modified = TRUE;
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void RangeBearingDataDialog::OnApply()
{
   RangeBearingData data = GetData();

   if (!m_already_added)
   {
      // add the r/b object with the given coast track data
      m_index = m_rb_mgr->add_to_list(data);
      
      m_already_added = TRUE;
   }
   else
   {
      // modify the r/b object with the given coast track data
      m_rb_mgr->modify(m_index, m_data);
   }
   
   GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}

void RangeBearingDataDialog::OnFrom() 
{
	LocationDlg dlg;

	dlg.SetData(m_data.m_from_latitude, m_data.m_from_longitude, 
		m_snap_to, m_snap_to_valid, TRUE);

	// add the GPS ship to the combo box of fix points
	if (m_data.m_to_description != "Self")
		dlg.add_fix_point("Self");
	// add each of the coast tracks to the list if they are 
	// not already used by 'To'
	for(int i=0;i<m_ctrack_mgr->get_count();i++)
	{
		CoastTrack *ctrack = m_ctrack_mgr->get_at(i);

		if (ctrack != NULL)
		{
			CoastTrackData &data = ctrack->get_coast_track_data();

			if (m_data.m_to_description != data.get_label())
				dlg.add_fix_point(data.get_label());
		}
	}

	int result = dlg.DoModal();
	if (result == GET_POSITION_FROM_MAP)
	{
		m_type = 1; /*FROM*/
		EndDialog(GET_POSITION_FROM_MAP);
	}
	else if (result == IDOK)
	{
		m_data.m_from_latitude = dlg.get_latitude();
      m_data.m_from_longitude = dlg.get_longitude();

      const int STR_LEN = 256;
		char latlon_string[STR_LEN];
		GEO_lat_lon_to_string(m_data.m_from_latitude, m_data.m_from_longitude, 
			latlon_string, STR_LEN);
		GetDlgItem(IDC_FROM_LOCATION)->SetWindowText(latlon_string);

		// get the description from the dialog
		CString description = dlg.get_description();
		// if the description is blank, then try using the fix/point text
		if (description == "")
			description = dlg.get_fix_point();
		// if the description is still blank, then use 'Fixed Location' as
		// the description
		if (description == "")
			description = "Fixed Location";

		m_data.m_from_description = description;
		GetDlgItem(IDC_FROM_DESCRIPTION)->SetWindowText(description);
		m_snap_to_valid = FALSE;

		on_modified();
	}
}

void RangeBearingDataDialog::OnTo() 
{
	LocationDlg dlg;

	dlg.SetData(m_data.m_to_latitude, m_data.m_to_longitude, 
		m_snap_to, m_snap_to_valid, FALSE);
	
	// add the GPS ship to the combo box of fix points
	if (m_data.m_from_description != "Self")
		dlg.add_fix_point("Self");
	// add each of the coast tracks to the list if they are 
	// not already used by 'From'
	for(int i=0;i<m_ctrack_mgr->get_count();i++)
	{
		CoastTrack *ctrack = m_ctrack_mgr->get_at(i);

		if (ctrack != NULL)
		{
			CoastTrackData &data = ctrack->get_coast_track_data();

			if (m_data.m_from_description != data.get_label())
				dlg.add_fix_point(data.get_label());
		}
	}

	int result = dlg.DoModal();
	if (result == GET_POSITION_FROM_MAP)
	{
		m_type = 2; /*TO*/
		EndDialog(GET_POSITION_FROM_MAP);
	}
	else if (result == IDOK)
	{
		m_data.m_to_latitude = dlg.get_latitude();
      m_data.m_to_longitude = dlg.get_longitude();

      const int STR_LEN = 256;
		char latlon_string[STR_LEN];
		GEO_lat_lon_to_string(m_data.m_to_latitude, m_data.m_to_longitude, 
			latlon_string, STR_LEN);
		GetDlgItem(IDC_TO_LOCATION)->SetWindowText(latlon_string);

		// get the description from the dialog
		CString description = dlg.get_description();
		// if the description is blank, then try using the fix/point text
		if (description == "")
			description = dlg.get_fix_point();
		// if the description is still blank, then use 'Fixed Location' as
		// the description
		if (description == "")
			description = "Fixed Location";

		m_data.m_to_description = description;
		GetDlgItem(IDC_TO_DESCRIPTION)->SetWindowText(description);
		m_snap_to_valid = FALSE;

		on_modified();
	}
}

void RangeBearingDataDialog::OnSwap() 
{
	// swap the from/to fields
	double tmp_lat, tmp_lon;
	CString tmp_description;
	
	tmp_lat = m_data.m_from_latitude;
	tmp_lon = m_data.m_from_longitude;
	tmp_description = m_data.m_from_description;
	
	m_data.m_from_latitude = m_data.m_to_latitude;
	m_data.m_from_longitude = m_data.m_to_longitude;
	m_data.m_from_description = m_data.m_to_description;

	m_data.m_to_latitude = tmp_lat;
	m_data.m_to_longitude = tmp_lon;
	m_data.m_to_description = tmp_description;

	update_from_to_text();
	on_modified();
}

void RangeBearingDataDialog::update_from_to_text()
{
	GetDlgItem(IDC_FROM_DESCRIPTION)->SetWindowText(m_data.m_from_description);
	GetDlgItem(IDC_TO_DESCRIPTION)->SetWindowText(m_data.m_to_description);
	
   const int STR_LEN = 256;
	char latlon_string[STR_LEN];
	GEO_lat_lon_to_string(m_data.m_from_latitude, m_data.m_from_longitude, 
		latlon_string, STR_LEN);
	GetDlgItem(IDC_FROM_LOCATION)->SetWindowText(latlon_string);
	GEO_lat_lon_to_string(m_data.m_to_latitude, m_data.m_to_longitude, 
		latlon_string, STR_LEN);
	GetDlgItem(IDC_TO_LOCATION)->SetWindowText(latlon_string);
}

RangeBearingData RangeBearingDataDialog::GetData() 
{ 
   // use the latest time of the associated moving map.  if one doesn't
   // exist, get the current time from the FalconView clock instead
   //

   PlaybackTimeSegment_Interface* pseg = dynamic_cast<PlaybackTimeSegment_Interface *>(
      OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   DATE begin, end;
   long valid = FALSE;
   if (pseg != NULL)
   {
      pseg->GetTimeSegment(&begin, &end, &valid);
   }

   if (valid)
      m_data.m_time_started = end;
   else
      m_data.m_time_started = CMainFrame::GetPlaybackDialog().get_current_time();

   m_data.m_time_stopped.SetStatus(COleDateTime::null);

   return m_data; 
}
LRESULT RangeBearingDataDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

