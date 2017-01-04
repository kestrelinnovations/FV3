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



// DataObject.cpp
//

#include "stdafx.h"
#include "DataObject.h"
#include "err.h"
#include "param.h"
#include "..\mainfrm.h"
#include "..\tabulareditor\FavoriteImportDlg.h"
#include "gps.h"
#include "..\PlaybackDialog\viewtime.h"
#include "ovl_mgr.h"

const int KNOTS = 0;
const int KM_HR = 1;
const int MPH = 2;

const int MAG_COURSE = 0;
const int TRUE_COURSE = 1;

#define READ_ONLY_TEXT_COLOR RGB(132,130,132)
#define READ_ONLY_BACK_COLOR RGB(214,211,206)

// constructor
GPSDataObject::GPSDataObject(C_overlay *overlay) : 
	TabularEditorDataObject(overlay)
{
	m_column_info_list.Add(column_info_t("Time", FALSE, FALSE, get_show_flag(1, TRUE)));
	m_column_info_list.Add(column_info_t("Date", FALSE, FALSE, get_show_flag(2, TRUE)));
	m_column_info_list.Add(column_info_t("Location", FALSE, FALSE, get_show_flag(3, TRUE)));
	m_column_info_list.Add(column_info_t("Altitude (ft MSL)", FALSE, FALSE, get_show_flag(4, TRUE)));
	m_column_info_list.Add(column_info_t("GS (knots)", FALSE, FALSE, get_show_flag(5, TRUE)));
	m_column_info_list.Add(column_info_t("GS (km/hr)", FALSE, FALSE, get_show_flag(6, TRUE)));
	m_column_info_list.Add(column_info_t("GS (mph)", FALSE, FALSE, get_show_flag(7, TRUE)));
	m_column_info_list.Add(column_info_t("MC", FALSE, FALSE, get_show_flag(8, TRUE)));
	m_column_info_list.Add(column_info_t("TC", FALSE, FALSE, get_show_flag(9, TRUE)));	
	m_column_info_list.Add(column_info_t("Comments", FALSE, TRUE, get_show_flag(10, TRUE)));
   m_column_info_list.Add(column_info_t("Links", TRUE, TRUE, get_show_flag(11, TRUE)));

	update_translation_table();
}

void GPSDataObject::update_translation_table()
{
	m_translation_table.RemoveAll();
	const int size = m_column_info_list.GetSize();
	for(int i = 0;i<size;i++)
	{
		if (m_column_info_list[i].is_shown)	
			m_translation_table.Add(i);
	}
}

void GPSDataObject::translate(int &col)
{
	if (m_translation_on && col >= 0 && col < GetNumCols())
		col = m_translation_table[col];
}

BOOL GPSDataObject::get_show_flag(int col, BOOL default_value)
{
	CString section = "TabularEditor\\GPS";
	CString key;
	key.Format("%d", col);

	return (BOOL)PRM_get_registry_int(section, key, default_value);
}

void GPSDataObject::set_show_flag(int col, BOOL is_shown)
{
	CString section = "TabularEditor\\GPS";
	CString key;
	key.Format("%d", col + 1);

	PRM_set_registry_int(section, key, is_shown);
	m_column_info_list[col].is_shown = is_shown;
}

GPSPointIcon *GPSDataObject::get_at_index(long row)
{
	C_gps_trail *overlay = dynamic_cast<C_gps_trail *>(m_parent_overlay);
	return overlay->get_at_index(row);
}

int GPSDataObject::GetNumCols()
{
	if (m_translation_on)
		return m_translation_table.GetSize();
	
	return m_column_info_list.GetSize();
}

BOOL GPSDataObject::GetIsShown(int col)
{
	return m_column_info_list[col].is_shown;
}

void GPSDataObject::SetIsShown(CList<BOOL, BOOL> &show_flags_list)
{
	POSITION position = show_flags_list.GetHeadPosition();
	int col_index = 0;

	while(position)
		set_show_flag(col_index++, show_flags_list.GetNext(position));

	update_translation_table();
}

int GPSDataObject::GetColName(int col, CString *string)
{
	translate(col);
	*string = m_column_info_list[col].column_name;

	return UG_SUCCESS;
}

long GPSDataObject::GetNumRows()
{
	C_gps_trail *trail = dynamic_cast<C_gps_trail *>(m_parent_overlay);

	int num_trail_points;
	trail->get_point_list_length(&num_trail_points);

	return num_trail_points;
}

int GPSDataObject::GetCell(int col, long row, CUGCell *cell)
{
	// draw arrow in left-most column to denote the selected icon
	if ( row >= 0 && col == -1 )
	{
		C_gps_trail *overlay = dynamic_cast<C_gps_trail *>(m_parent_overlay);
		bool selected = (get_at_index(row) == overlay->get_selected_point());

		if (selected)
		{
			cell->SetCellType(UGCT_ARROW);
			cell->SetCellTypeEx(UGCT_ARROWRIGHT);
		}
		else
			cell->SetCellType(UGCT_NORMAL);
	}

	// get column headers
	if ( col >= 0 && row == -1 )
	{
		CString column_name;
		GetColName(col, &column_name);
		cell->SetText(column_name);
	}

	// get data cells
	else if (col >= 0 && row >= 0)
	{
		translate(col);
		switch(col)
		{

		case 0:
			return GetCell_Time(row, cell);

		case 1:
			return GetCell_Date(row, cell);

		case 2:
			return GetCell_Location(row, cell);

		case 3:
			return GetCell_Altitude(row, cell);

		case 4:
			return GetCell_GroundSpeed(row, cell, KNOTS);

		case 5:
			return GetCell_GroundSpeed(row, cell, KM_HR);

		case 6:
			return GetCell_GroundSpeed(row, cell, MPH);

		case 7:
			return GetCell_Course(row, cell, MAG_COURSE);

		case 8:
			return GetCell_Course(row, cell, TRUE_COURSE);

		case 9:
			return GetCell_Comments(row, cell);

      case 10:
         return GetCell_Links(row, cell);

			break;
		}
	}

	return UG_SUCCESS;
}

int GPSDataObject::OnEditStart(int col, long row,CWnd **edit)
{
   translate(col);

	if (m_column_info_list[col].hard_edit && !m_hard_edit)
		return FALSE;

	if (!m_column_info_list[col].allow_edit)
		return FALSE;

	if (row < 0 || row >= GetNumRows())
		return FALSE;

   if (m_column_info_list[col].column_name == "Links")
	{
		OnEditLinks(row);
		return FALSE;
	}

	return TRUE;
}

int GPSDataObject::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	if (cancelFlag)
		return TRUE;

	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return TRUE;

	if (CString(string) == "")
		point->set_has_comment(FALSE);
	else
		point->set_has_comment(TRUE);

	point->set_comment_txt(string);
	m_parent_overlay->set_modified(TRUE);
	OVL_get_overlay_manager()->InvalidateOverlay(m_parent_overlay);

	return TRUE;
}

void GPSDataObject::OnMenuStart(int col, int row, int base_ID)
{
	m_ctrl->AddMenuItem(base_ID, "Center Map");
	m_ctrl->AddMenuItem(base_ID + 1, "Add to Favorites...");
	m_ctrl->AddMenuItem(-1, "");								// separator

	m_ctrl->AddMenuItem(base_ID + 2, "Set Clock");
}

void GPSDataObject::OnMenuCommand(int col, long row, int item)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return;

	CFavoriteImportDlg dlg;
	CList<d_geo_t, d_geo_t> location_list;
	d_geo_t location;
	CString string;
	CList<CString, CString> name_list;
	

	switch(item)
	{
		// Center Map
	case 0:
	
		center_map(point);
		break;

		// Add to Favorites...
	case 1:
		if (get_location_at(row, m_ctrl->GetCurrentTab(), location, string))
		{
			location_list.AddTail(location);
			name_list.AddTail(string);
			dlg.set_location_list(&location_list);
			dlg.set_name_list(&name_list);
			dlg.DoModal();
		}
		break;

		// Set Clock
	case 2:
		
		CMainFrame::GetPlaybackDialog().set_current_time(point->get_date_time());
		break;
	}
}

// the user has updated the selection in the overlay
void GPSDataObject::on_selection_changed() 
{
	// the selection was changed in the overlay, insure that the selected row is visible
	//
	C_gps_trail *overlay = dynamic_cast<C_gps_trail *>(m_parent_overlay);
	GPSPointIcon *current_selection = overlay->get_selected_point();

	const int row = overlay->get_index(current_selection);
	m_ctrl->GotoCell(0, row);
}

// a row was selected in the tabular editor - update
// highlight / selected state in the overlay
void GPSDataObject::on_selected(int row)
{
	C_gps_trail *overlay = dynamic_cast<C_gps_trail *>(m_parent_overlay);
	GPSPointIcon *point = get_at_index(row);
	GPSPointIcon *current_selection = overlay->get_selected_point();

	if (point)
	{
		if (current_selection)
			current_selection->set_selected(FALSE);

		overlay->set_selected_point(point);
		point->set_selected(TRUE);

		OVL_get_overlay_manager()->InvalidateOverlay(overlay);
	}
}

// center the map at the currently selected row
void GPSDataObject::center_map()
{
	C_gps_trail *overlay = dynamic_cast<C_gps_trail *>(m_parent_overlay);
	GPSPointIcon *current_selection = overlay->get_selected_point();

	if (current_selection)
		center_map(current_selection);
}

void GPSDataObject::center_map(GPSPointIcon *point)
{
	ASSERT(point);

	CView* view = UTL_get_active_non_printing_view();
	if (UTL_change_view_map_to_best(view, point->get_latitude(), point->get_longitude()) == FAILURE)
	{
		ERR_report("Failed to set current center");
	}
	OVL_get_overlay_manager()->InvalidateOverlay(point->m_overlay);
}

// return the geo location associated with the given row
BOOL GPSDataObject::get_location_at(long row, int tab, d_geo_t &location, CString &string)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return FALSE;

	location.lat = point->get_latitude();
	location.lon = point->get_longitude();

	string = "";

	return TRUE;
}

int GPSDataObject::GetCell_Location(int row, CUGCell *cell)
{
   int nCurrentTab = m_ctrl->GetCurrentTab();

	char geo_string[GEO_MAX_LAT_LON_STRING+1];

	if (row < 0 || row >= GetNumRows())
		return UG_ERROR;

	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return UG_ERROR;

   double dLat, dLon;
   point->get_center(nCurrentTab, dLat, dLon);

	if (m_geotrans.DLL_lat_lon_to_geo(dLat, dLon, geo_string, GEO_MAX_LAT_LON_STRING+1) != SUCCESS)
	{
		ERR_report("Unable to convert lat, lon to a string");
		return UG_ERROR;
	}

   cell->SetTextColor(READ_ONLY_TEXT_COLOR);
	cell->SetBackColor(READ_ONLY_BACK_COLOR);
	cell->SetReadOnly(TRUE);

	cell->SetText(CString(geo_string));

	return UG_SUCCESS;
}

int GPSDataObject::GetCell_Altitude(int row, CUGCell *cell)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return UG_ERROR;

   cell->SetTextColor(READ_ONLY_TEXT_COLOR);
	cell->SetBackColor(READ_ONLY_BACK_COLOR);
	cell->SetReadOnly(TRUE);

	cell->SetAlignment(UG_ALIGNRIGHT);
	cell->SetDataType(UGCELLDATA_NUMBER);
	cell->SetNumberDecimals(1);
	cell->SetNumber(METERS_TO_FEET(point->m_msl));

	return UG_SUCCESS;
}

int GPSDataObject::GetCell_GroundSpeed(int row, CUGCell *cell, int type)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return UG_ERROR;

	cell->SetAlignment(UG_ALIGNRIGHT);

   cell->SetTextColor(READ_ONLY_TEXT_COLOR);
	cell->SetBackColor(READ_ONLY_BACK_COLOR);
	cell->SetReadOnly(TRUE);

   if (point->m_speed_km_hr != -1.0 || point->m_speed_knots != -1.0)
   {
		float km_hr = point->m_speed_km_hr;
		float knots = point->m_speed_knots;
		
		if (km_hr == -1.0)
			km_hr = (float)KNOTS_TO_KM_PER_H(knots);
		if (knots == -1.0)
			knots = km_hr * 0.53995333005f;

		cell->SetDataType(UGCELLDATA_NUMBER);
		cell->SetNumberDecimals(0);

      if (point->m_speed_knots > 0.0)
      {
			switch(type)
			{
			case KNOTS:
				cell->SetNumber(knots);
				break;
			case KM_HR:
				cell->SetNumber(km_hr);
				break;
			case MPH:
				cell->SetNumber(km_hr * 0.621371192L);
				break;
			}
      }
      else 
			cell->SetNumber(0);
   }
   else
	{
		cell->SetDataType(UGCELLDATA_STRING);
		cell->SetText("Unknown");
	}

	return UG_SUCCESS;
}

int GPSDataObject::GetCell_Course(int row, CUGCell *cell, int type)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return UG_ERROR;

	cell->SetAlignment(UG_ALIGNCENTER);

   cell->SetTextColor(READ_ONLY_TEXT_COLOR);
	cell->SetBackColor(READ_ONLY_BACK_COLOR);
	cell->SetReadOnly(TRUE);

	// add SPEED and HEADING lines to edit string
   if (point->m_speed_km_hr != -1.0 || point->m_speed_knots != -1.0)
   {
      if (point->m_speed_knots > 0.0)
      {
			CString s;
			s.Format("%03.0f\260", type == MAG_COURSE ? point->m_magnetic_heading : point->m_true_heading);
			cell->SetText(s);
      }
      else 
			cell->SetText("Unknown");
   }
   else
		cell->SetText("Unknown");

	return UG_SUCCESS;
}

int GPSDataObject::GetCell_Date(int row, CUGCell *cell)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return UG_ERROR;

	cell->SetText("");

   cell->SetTextColor(READ_ONLY_TEXT_COLOR);
	cell->SetBackColor(READ_ONLY_BACK_COLOR);
	cell->SetReadOnly(TRUE);

	if (point->valid_time())
   {
      unsigned char hour   = point->get_hour();
      unsigned char minute = point->get_minute();
      float second = point->get_second();
      
      // if the date is valid, include the day and date in the output
      if (point->valid_date())
      {
         int year  = point->get_year();
         int month = point->get_month();
         int day   = point->get_day();
         
         //we might have a 2 digit year so massage it if necessary
         year = GPS_get_y2k_compliant_year(year);
         
         // construct a CTime to take advantage of the Format member, i.e.,
         // get the day and month as strings
         COleDateTime t(year, month, day, hour, minute, (int)second);
         cell->SetText(t.Format("%m/%d/%y"));
      }
   }

	return UG_SUCCESS;
}

int GPSDataObject::GetCell_Time(int row, CUGCell *cell)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return UG_ERROR;

	cell->SetText("");

   cell->SetTextColor(READ_ONLY_TEXT_COLOR);
	cell->SetBackColor(READ_ONLY_BACK_COLOR);
	cell->SetReadOnly(TRUE);

	if (point->valid_time())
   {
      unsigned char hour   = point->get_hour();
      unsigned char minute = point->get_minute();
      float second = point->get_second();
      
		CString t;
      t.Format("%02d:%02d:%02.0f", hour, minute, second);
		cell->SetText(t);
   }

	return UG_SUCCESS;

}

int GPSDataObject::GetCell_Comments(int row, CUGCell *cell)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return UG_ERROR;

	if (point->has_comment())
		cell->SetText(point->get_comment_txt());
	else
		cell->SetText("");

	return UG_SUCCESS;
}

int GPSDataObject::GetCell_Links(int row, CUGCell *cell)
{
	GPSPointIcon *point = get_at_index(row);
	if (point == NULL)
		return UG_ERROR;

	CStringArray links;
   point->GetLinks(links);

	if (links.GetSize())
	{
		cell->SetCellType(FVCT_NOTE);
		cell->SetText(links[0]);
		
		CString note_text;
		const int nNumLinks = links.GetSize();
		for(int i=0; i<nNumLinks; i++)
		{
			note_text += links[i];
			if (i != (nNumLinks - 1))
				note_text += "\n";
		}
		cell->SetLabelText(note_text);
	}
	else
		cell->SetCellType(UGCT_NORMAL);

	return UG_SUCCESS;
}

void GPSDataObject::OnEditLinks(int row)
{
   GPSPointIcon *pPoint = get_at_index(row);
	if (pPoint == NULL)
		return;

   C_gps_trail::edit_gps_point_links_callback(NULL, pPoint);
}

// get a list of tabs for this data object
void GPSDataObject::get_tab_list(CList<CString, CString> &listTabNames)
{
   GPSPointIcon *point = get_at_index(0);

	if (point == NULL || !point->has_multiple_centers())
   {
      listTabNames.AddTail("GPS Points");
      return;
   }

   int new_pos;
   CString strPrimaryCenter = extract_element(point->m_meta_data, "PrimaryCenterName", 0, new_pos);

   if (strPrimaryCenter.GetLength())
      listTabNames.AddTail(strPrimaryCenter);
   else
      listTabNames.AddTail("GPS Points");    // in case meta data doesn't include primary name

   CString strCenter = extract_element(point->m_meta_data, "Center", 0, new_pos);
   while (strCenter.GetLength())
   {
      int unused;
      CString strTabName = extract_element(strCenter, "Name", 0, unused);
      listTabNames.AddTail(strTabName);

      strCenter = extract_element(point->m_meta_data, "Center", new_pos, new_pos);
   }
}
