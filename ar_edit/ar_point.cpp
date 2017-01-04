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

// ar_point.cpp

#include "stdafx.h"
#include "ar_edit.h"
#include "geo_tool.h"
#include "SnapTo.h"

// point usage codes
/*
		IP - AIR REFUELING INITIAL POINT
		CP - AIR REFUELING CONTROL POINT
		NC - NAVIGATION CHECK POINT
		EX - EXIT POINT
		ET - ENTRY POINT (ANCHORS ONLY)
		AN - ANCHOR POINT (ANCHORS ONLY)
		AP - ANCHOR PATTERN (ANCHORS ONLY)
*/

static POSITION usage_pos = NULL;

// *****************************************************************
// *****************************************************************


C_ar_point::C_ar_point()
{
	m_obj = NULL;
	m_id = "Untitled";
	m_lat = 0.0;
	m_lon = 0.0;
	m_pt.x = 0;
	m_pt.y = 0;
	m_rect.SetRect(0,0,0,0);
	m_fix = "";
	m_mag_var = 0.0;
	m_bEndPoint = FALSE;
	m_bStartPoint = FALSE;
	m_selected = FALSE;
	m_anchor_point = FALSE;
	m_mag_var = 0.0;
	m_dist = 0.0;
	m_angle = 0.0;
	m_type = ' ';
	m_num = 0;
	m_nav_lat = 0.0;
	m_nav_lon = 0.0;
	m_nav_mag_var = 0.0;
	m_nav_slave_var = 0.0;
}


// *****************************************************************
// *****************************************************************

C_ar_point::C_ar_point(C_ar_obj* obj)
{
//	ASSERT(obj);
	m_obj = obj;
	if (obj != NULL)
		m_id = obj->m_id;
	else
		m_id = "Untitled";
	m_lat = 0.0;
	m_lon = 0.0;
	m_pt.x = 0;
	m_pt.y = 0;
	m_rect.SetRect(0,0,0,0);
	m_fix = "";
	m_true_fix = "";
	m_mag_var = 0.0;
	m_bEndPoint = FALSE;
	m_bStartPoint = FALSE;
	m_selected = FALSE;
	m_anchor_point = FALSE;
	m_mag_var = 0.0;
	m_dist = 0.0;
	m_angle = 0.0;
	m_type = ' ';
	m_num = 0;
	m_nav_lat = 0.0;
	m_nav_lon = 0.0;
	m_nav_mag_var = 0.0;
	m_nav_slave_var = 0.0;
}

// *****************************************************************
// *****************************************************************

C_ar_point::~C_ar_point()
{
	clear_usages();
}

// *****************************************************************
// *****************************************************************

void C_ar_point::clear_usages()
{
	CString *txt;

	while (!m_usage.IsEmpty())
	{
		txt = m_usage.RemoveHead();
		delete txt;
	}
}

// *****************************************************************
// *****************************************************************

BOOL C_ar_point::hit_test(CPoint point)
{
//	CFvwUtil *futil = CFvwUtil::get_instance();
	BOOL hit;

	hit = FALSE;

	if (m_rect.PtInRect(point))
	{
		hit = TRUE;
	}

//	if (m_fuel_point_selected)	
//	{
//		m_selected = hit;
//		m_fuel_point_selected_pt = point;
//	}

//	if (futil.distance_to_line(m_pt.x, m_pt.y, m_pt2.x, m_pt2.y, point.x, point.y) < 2)
//		return TRUE;	
	
	return hit;
}
// end of hit_test

// *****************************************************************
// ****************************************************************

// get a list of the usage types of a string
void C_ar_point::get_usage_string(CString & usage)
{
	POSITION next;
	CString *txt;
   const int SLINE_LEN = 81;
	char sline[SLINE_LEN];

	usage = "";
	next = m_usage.GetHeadPosition();
	while (next != NULL)
	{
		txt = m_usage.GetNext(next);
		if (usage.GetLength() > 0)
			usage += ", ";
		C_ar_edit::refuel_usage_to_string(*txt, sline, SLINE_LEN);
		usage += sline;
	}
}
// end of get_usage_string

// ****************************************************************
// ****************************************************************

BOOL C_ar_point::get_first_usage_string(CString & usage)
{
	POSITION next;
	CString *txt;
   const int SLINE_LEN = 81;
	char sline[SLINE_LEN];

	usage = "";
	next = m_usage.GetHeadPosition();
	if (next != NULL)
	{
		txt = m_usage.GetNext(next);
		C_ar_edit::refuel_usage_to_string(*txt, sline, SLINE_LEN);
		usage = sline;
		usage_pos = next;
		return TRUE;
	}
	return FALSE;
}
// end of get_first_usage_string

// ****************************************************************
// ****************************************************************

BOOL C_ar_point::get_next_usage_string(CString & usage)
{
	CString *txt;
   const int SLINE_LEN = 81;
	char sline[SLINE_LEN];

	usage = "";
	if (usage_pos != NULL)
	{
		txt = m_usage.GetNext(usage_pos);
		C_ar_edit::refuel_usage_to_string(*txt, sline, SLINE_LEN);
		usage = sline;
		return TRUE;
	}
	return FALSE;
}
// end of get_next_usage_string

// ****************************************************************
// ****************************************************************

// get a list of the usage types of a string
void C_ar_point::add_usage(CString usage)
{
	CString *txt;
	POSITION next;

	// check to see if usage already exists
	next = m_usage.GetHeadPosition();
	while (next != NULL)
	{
		txt = m_usage.GetNext(next);
		if (!txt->Compare(usage))
			return;
	}

	txt = new CString;
	*txt = usage;
	m_usage.AddTail(txt);
	ASSERT(m_usage.GetCount() > 0);
}
// end of add_usage

// ****************************************************************
// ****************************************************************

// remove a usage type from a point, returns TRUE if successful
BOOL C_ar_point::remove_usage(CString usage)
{
	POSITION pos, oldpos;
	CString *txt;
	
	if (m_usage.GetCount() < 2)
		return FALSE;

	pos = m_usage.GetHeadPosition();
	while (pos != NULL)
	{
		oldpos = pos;
		txt = m_usage.GetNext(pos);
		if (!txt->CompareNoCase(usage))
		{
			txt = m_usage.GetAt(oldpos);
			m_usage.RemoveAt(oldpos);
			delete txt;
			return TRUE;
		}
	}

	return FALSE;
}
// end of add_usage

// ****************************************************************
// ****************************************************************

// determine if a point has a specific usage and if that usage is unique
BOOL C_ar_point::has_usage_string(CString usage, BOOL *unique)
{
	POSITION next;
	CString *txt;

	*unique = TRUE;
	if (m_usage.GetCount() > 1)
		*unique = FALSE;
	next = m_usage.GetHeadPosition();
	while (next != NULL)
	{
		txt = m_usage.GetNext(next);
		if (!txt->CompareNoCase(usage))
			return TRUE;
	}
	return FALSE;
}
// end of has_usage_string

// ****************************************************************
// ****************************************************************

// determine if a point has only one usage, if so return it
BOOL C_ar_point::has_one_usage(CString & usage)
{
	int cnt;
	CString *txt;

	usage = "";

	cnt = m_usage.GetCount();
	if (cnt < 1)
		return FALSE;

	txt = m_usage.GetHead();
	usage = *txt;
	if (cnt == 1)
		return TRUE;

	return FALSE;
}
// end of has_one_string

// ****************************************************************
// ****************************************************************

// calculate nav fix from nav geo
void C_ar_point::calc_nav_fix(BOOL is_auto_magvar, double manual_magvar)
{
	int year, month, alt, rslt;
	double magvar, dist, ang, tf, tmag, tang;
	CString tstr;
	SYSTEMTIME time;
	BOOL same = TRUE;

	if ((m_nav_lat == 0.0) && (m_nav_lon == 0.0))
		return;

	GetSystemTime(&time);
	year = time.wYear;
	month = time.wMonth;
	alt = 0;

	// compute nav lat/long
	GEO_geo_to_distance(m_nav_lat, m_nav_lon, m_lat, m_lon, &dist, &ang);
	tf = m_nav_slave_var;
	if (tf < 0.0)
		tf = -tf;

	magvar = 0.0;
	tmag = 0;

	if ((tf != 0.0) && (tf < 179.0))
	{
		magvar = m_nav_slave_var;
		tmag = magvar;
	}
	else 
	{
		if (m_nav_mag_var > 0.0)
			tmag = m_nav_mag_var;
		else
		{
			rslt = GEO_magnetic_variation(m_lat, m_lon, year, month, alt, &tmag);
			if (!is_auto_magvar)
				tmag = manual_magvar;
			if (rslt != SUCCESS)
				tmag = 0.0;
		}
	}

	tang = ang - magvar;
	if (dist == 0.0)
		tang = 0.0;
	m_true_fix.Format("%03.0f%03.0f", tang, dist / 1852.0);
	same = FALSE;

	ang -= tmag;

	if (dist == 0.0)
		ang = 0.0;
	m_fix.Format("%03.0f%03.0f", ang, dist / 1852.0);
}
// end of calc_nav_fix

// ****************************************************************
// ****************************************************************

// calculate nav geo from nav fix
void C_ar_point::calc_nav_geo()
{
	int year, month, alt, rslt;
	double magvar, dist, ang;
	CString tstr;
	SYSTEMTIME time;

	if (m_fix.GetLength() < 6)
		return;

	GetSystemTime(&time);
	year = time.wYear;
	month = time.wMonth;
	alt = 0;

	// compute nav lat/long
	rslt = GEO_magnetic_variation(m_lat, m_lon, year, month, alt, &magvar);
	if (rslt != SUCCESS)
		magvar = 0.0;
	tstr = m_fix.Left(3);
	ang = atof(tstr);
	tstr = m_fix.Right(3);
	dist = atof(tstr);
	dist *= 1852.0;
	ang -= 180.0 - magvar;
	if (ang > 360.0)
		ang -= 360.0;
	GEO_distance_to_geo(m_lat, m_lon, dist, ang, &m_nav_lat, &m_nav_lon);
}
// end of calc_nav_geo

// ****************************************************************
// ****************************************************************

// calculate distance and angle of point
void C_ar_point::calc_dist_angle()
{
	double dist, ang;

	ASSERT(m_obj);
	if (m_obj == NULL)
		return;

	GEO_geo_to_distance(m_obj->m_anchor_pt_lat, m_obj->m_anchor_pt_lon,	m_lat, m_lon, &dist, &ang);
	m_dist = dist / 1000.0;  // convert to km
	if (dist < 0.1) // check for coincidence
		m_angle = 0.0;
	else
		m_angle = ang - m_obj->m_angle;
}
// end of calc_dist_angle

// ****************************************************************
// ****************************************************************

void C_ar_point::set_crossref(SnapToInfo* snap_to_info)
{
	CString tstr;

	ASSERT(GEO_valid_degrees(snap_to_info->m_lat, snap_to_info->m_lon));

	m_nav_id = snap_to_info->m_fix_rs;
	m_nav_type = C_ar_edit::get_navaid_type(snap_to_info->m_nav_type);
	m_nav_flag = " ";
	m_nav_country = snap_to_info->m_country_code;
	m_nav_lat = snap_to_info->m_lat;
	m_nav_lon = snap_to_info->m_lon;
	m_nav_mag_var = snap_to_info->m_mag_var;
	m_nav_slave_var = snap_to_info->m_slave_var;
	tstr = snap_to_info->m_db_lookup_rs;
	m_nav_key = tstr.Mid(5, 1);

	// compute fix
	calc_nav_fix();
}
// end of set_crossref

// ****************************************************************
// ****************************************************************

void C_ar_point::move(SnapToInfo* snap_to_info)
{
	CString tstr;

	ASSERT(GEO_valid_degrees(snap_to_info->m_lat, snap_to_info->m_lon));

	m_lat = snap_to_info->m_lat;
	m_lon = snap_to_info->m_lon;
	set_crossref(snap_to_info);
}
// end of move

// ****************************************************************
// ****************************************************************

/*
void C_ar_point::copy(C_ar_point *vtx)
{
	CString *txt;

	vtx->m_id = m_id;
	vtx->m_direction = m_direction;
	vtx->m_seq_num = m_seq_num;
	vtx->m_obj = m_obj;
	vtx->m_label = m_label;
	vtx->m_lat = m_lat;
	vtx->m_lon = m_lon;
	vtx->m_pt = m_pt;
	vtx->m_rect = m_rect;
	vtx->m_obj = m_obj;
	vtx->m_nav_flag = m_nav_flag;
	vtx->m_nav_id = m_nav_id;
	vtx->m_nav_type = m_nav_type;
	vtx->m_nav_country = m_nav_country;
	vtx->m_nav_key = m_nav_key;
	vtx->m_nav_lat = m_nav_lat;
	vtx->m_nav_lon = m_nav_lon;
	vtx->m_fix = m_fix;
	vtx->m_dist = m_dist;
	vtx->m_angle = m_angle;
	vtx->m_type = m_type;
	vtx->m_bEndPoint = m_bEndPoint;
	vtx->m_bStartPoint = m_bStartPoint;
	while (!vtx->m_usage.IsEmpty())
	{
		txt = vtx->m_usage.RemoveHead();
		delete txt;
	}
	if (m_usage.GetCount() > 0)
	{
		CString *newtxt;
		POSITION next;

		next = m_usage.GetHeadPosition();
		ASSERT(next);
		while (next != NULL)
		{
			txt = m_usage.GetNext(next);
			newtxt = new CString;
			*newtxt = *txt;
			vtx->m_usage.AddTail(newtxt);
		}
	}
		
}
// end of copy
*/


// *****************************************************************
// ****************************************************************

C_ar_point &C_ar_point::operator=(C_ar_point &vtx) 
{
	CString *txt;

	m_id = vtx.m_id;
	m_direction = vtx.m_direction;
	m_seq_num = vtx.m_seq_num;
	m_label = vtx.m_label;
	m_obj = vtx.m_obj;
	m_lat = vtx.m_lat;
//ASSERT(m_lat);
	m_lon = vtx.m_lon;
	m_pt = vtx.m_pt;
	m_rect = vtx.m_rect;
	m_obj = vtx.m_obj;
	m_nav_flag = vtx.m_nav_flag;
	m_nav_id = vtx.m_nav_id;
	m_nav_type = vtx.m_nav_type;
	m_nav_country = vtx.m_nav_country;
	m_nav_key = vtx.m_nav_key;
	m_nav_lat = vtx.m_nav_lat;
	m_nav_lon = vtx.m_nav_lon;
	m_fix = vtx.m_fix;
	m_true_fix = vtx.m_true_fix;
	m_dist = vtx.m_dist;
	m_angle = vtx.m_angle;
	m_type = vtx.m_type;
	m_bEndPoint = vtx.m_bEndPoint;
	m_bStartPoint = vtx.m_bStartPoint;
	m_anchor_point = vtx.m_anchor_point;
	m_usage_code = vtx.m_usage_code;
	m_usage_str = vtx.m_usage_str;
	while (!m_usage.IsEmpty())
	{
		txt = m_usage.RemoveHead();
		delete txt;
	}

	if (vtx.m_usage.GetCount() > 0)
	{
		CString *newtxt;
		POSITION next;

		next = vtx.m_usage.GetHeadPosition();
		while (next != NULL)
		{
			txt = vtx.m_usage.GetNext(next);
			newtxt = new CString;
			*newtxt = *txt;
			m_usage.AddTail(newtxt);
		}
	}
	return *this;
}
// end of operator =

// *****************************************************************
// *****************************************************************
