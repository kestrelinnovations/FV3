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



// daffuel.cpp  -- member functions of C_ar_edit

// usage codes:
//  IP - Air Refueling Initial Point
//  CP - Air Refueling Control Point
//  NC - Navigation Check Point
//  EX - Exit Point
//  ET - Entry Point (Anchors Only)
//  AN - Anchor Point (Anchors Only)
//  AP - Anchor Pattern (Anchors Only)

#include "stdafx.h"
#include "param.h"
#include "showrmk.h"
#include "geo_tool.h"
#include "FvCore\Include\RichTextFormatter.h"
#include "fvwutil.h"
#include "maps.h"       // for MAP_get_scale_from_string
#include "infodlg.h"
#include "ar_dlg.h"
#include "pntinfo.h"
#include "factory.h"
#include "ar_util.h"
#include "..\getobjpr.h"
#include "SnapTo.h"
#include "ovl_mgr.h"
#include "err.h"

#define INFO_DATUM_COLOR			RGB( 0, 128, 0)

MapScale C_ar_edit::m_smallest_scale;
static char fuel_label[20][81];
static BOOL in_refuel_show_info;

//static C_ar_edit *cur_ovl;
BOOL C_ar_edit::m_in_poly_mode = FALSE;
BOOL C_ar_edit::m_bEdit = FALSE;
BOOL C_ar_edit::m_bTempSelect = FALSE;
int C_ar_edit::m_nTempMode;
C_ar_obj* C_ar_edit::m_pTempObj = NULL;
CPoint C_ar_edit::m_temp_start_pt;
CPoint C_ar_edit::m_anchor_pt;
int C_ar_edit::m_nMode;
BOOL C_ar_edit::m_bGotPt;
int C_ar_edit::m_cur_vertex_num = -1;
C_ar_point* C_ar_edit::m_cur_point = NULL;
C_ar_point* C_ar_edit::m_menu_point = NULL;
C_ar_obj* C_ar_edit::m_cur_obj = NULL;
C_ar_obj* C_ar_edit::m_selected_obj = NULL;
C_ar_edit* C_ar_edit::m_cur_ovl = NULL;
int C_ar_edit::m_ovl_cnt = 1;
CString C_ar_edit::m_directory;
double C_ar_edit::m_cursor_lat;
double C_ar_edit::m_cursor_lon;
BOOL C_ar_edit::m_start_drag = FALSE;
static C_ar_obj *cur_obj = NULL;
int C_ar_edit::m_old_cnt = 0;
double C_ar_edit::m_point_lat;
double C_ar_edit::m_point_lon;
BOOL C_ar_edit::m_anchor_ext_pt = FALSE;
C_ar_point * C_ar_edit::m_anchor_ext_point;
CIconImage*	C_ar_edit::m_draw_icon = NULL;

// *****************************************************************
// *****************************************************************

C_ar_edit::C_ar_edit(CString name /* = "" */) : 
   CFvOverlayPersistenceImpl(name),
   m_bDragging(FALSE)
{
	// the block of code above was replaced by this in order to eliminate the use of PRM_get_value
	CString reg_string = PRM_get_registry_string("Main", PRM_HD_DATA);
	strncpy_s(OvlFileName, 100, reg_string, (sizeof(OvlFileName)-1));
	strcat_s(OvlFileName, 100, "\\ar\\");

//	m_pUtil = new CDafifUtil();
	m_point_selected = FALSE;

	m_fuel_select_id = "";
	m_fuel_point_selected_lat = 0;
	m_fuel_point_selected_lon = 0;
	m_fuel_point_selected_pt.x = -1;
	m_fuel_point_selected_pt.y = -1;
	m_fuel_point_selected_usage = "";
	m_fuel_point_selected = FALSE;
	m_fuel_selected_hit_test = FALSE;

	CString dir = "DAFIF\\";
	ip_icon		= CIconImage::load_images(dir + "fuel_ip.ico");
	cp_icon		= CIconImage::load_images(dir + "fuel_cp.ico");
	ap_icon		= CIconImage::load_images(dir + "fuel_ap.ico");
	an_icon		= CIconImage::load_images(dir + "fuel_an.ico");
	nc_icon		= CIconImage::load_images(dir + "fuel_nc.ico");
	et_icon		= CIconImage::load_images(dir + "fuel_et.ico");
	ex_icon		= CIconImage::load_images(dir + "fuel_ex.ico");
	sel_icon	= CIconImage::load_images(dir + "fuel_sel.ico");

	m_cur_ovl = this;
	m_cur_point = NULL;
	m_anchor_dragger = NULL;
	m_start_drag = FALSE;
	m_old_route = new C_ar_obj(this);
	m_originate = FALSE;
}
// end of constructor of C_ar_edit

// *****************************************************************
// *****************************************************************

C_ar_edit::~C_ar_edit()
{
	// Most, if not all, destruction should be accomplished in Finalize.  
	// Due to C# objects registering for notifications, pointers to this object may not be 
	// released until the next garbage collection.
}

// *****************************************************************
// *****************************************************************

void C_ar_edit::Finalize()
{
	clear_ovl();
//	delete m_pUtil;
	if (m_point_selected)
		CRemarkDisplay::close_dlg();

	m_cur_ovl = NULL;
	clear_vertex_hit_list();
	delete m_old_route;
	m_old_route = NULL;
}

// *****************************************************************
// *****************************************************************

// clear the CList of hit vertexes

void C_ar_edit::clear_vertex_hit_list()
{
	C_ar_point *vtx;

	while (!m_refuel_vertex_hit_list.IsEmpty())
	{
		vtx = m_refuel_vertex_hit_list.RemoveHead();
		delete vtx;
	}
}
// end of clear_vertex_hit_list

// *****************************************************************
// *****************************************************************

C_icon *C_ar_edit::hit_test(MapProj* map, CPoint point)
{
	POSITION next;
	C_ar_obj *obj;
	BOOL hit, hit_anchor_pt;
	int index;

	if (!m_map_displayed)
		return NULL;

	if (!m_obj_list.IsEmpty())
	{
		m_selected_obj= NULL;

		next = m_obj_list.GetHeadPosition();
		do
		{
			// get this item and the position of the next one
			obj = m_obj_list.GetNext(next);
			if (m_fuel_selected_hit_test)
				hit = obj->list_hit_test(point);
			else
				hit = obj->hit_test(map, point, &index, &hit_anchor_pt);
	   	if (hit)
			{
				normalize_id(obj->m_id, obj->m_direction, m_fuel_select_id);
				m_fuel_mouse_point = point;
				m_selected_obj = obj;
		   	return (C_icon*) obj;
			}
		} while (next != NULL);
	}
	return NULL;
}
// end of hit_test

// *****************************************************************
// *****************************************************************
// Find the waypoint associated with the screen x,y 

BOOL C_ar_edit::select_point(const CList<CString*, CString*> & strlist, CString & key, CString & usage)
{
	int cnt, rslt;
	CRemarkListDlg dlg;
	CString tstr;

	key = "";
	cnt = strlist.GetCount();
	if (cnt < 1)
		return FALSE;

	dlg.init_list(strlist);
	rslt = dlg.DoModal();
	if (rslt != IDOK)
		return FALSE;

	dlg.get_selection(key);
	cnt = key.GetLength();
	tstr = key.Right(cnt - 10);
	refuel_string_to_usage(tstr, usage);
	key = key.Left(8);
	key.TrimRight();
	return TRUE;
}
// end of select_point

// *****************************************************************
// *****************************************************************
// Find the waypoint associated with the screen x,y 

int C_ar_edit::show_info(C_icon* icon)
{
	C_ar_obj *ar;
	C_ar_point *vtx;
	CString tstr, key;
   const int DATUM_TYPE_LEN = 21;
	char datum_type[DATUM_TYPE_LEN];
   const int TSTR_LEN = 200;
	char tstr_ch[TSTR_LEN];
	CString tid;
	CList<CString*, CString*> strlist;
	BOOL onpoint = FALSE;
//	BOOL notdone;
//	BOOL rslt;
	CString pt_usage, pt_loc, pt_datum, pt_navid;
	CString pt_fix, pt_bearing, pt_dist;
	BOOL hit_anchor_pt;
	int data_color = UTIL_COLOR_DARK_BLUE;
	CArUtil util;

// for debug
int ptsize = sizeof(C_ar_point);
int objsize = sizeof(C_ar_obj);

	if (icon == NULL)
		return FAILURE;

	ar = (C_ar_obj*) icon;

	invalidate_and_clear_selects();
	
	if (!m_bEdit)
		ar->m_selected = TRUE;

	m_fuel_point_selected = TRUE;

	ar->list_hit_test(m_fuel_mouse_point);

	m_fuel_point_selected = FALSE;

	// mark the route as selected
//	ar->m_selected = TRUE;
//	fuel_select_id = ar->m_id;
	ar->invalidate();

	// build the formated string for the dialog box
	CString edit("");
	CString temp("");
	CString newone("");
	CString msg("");
	int i=0;

/*
	if ( !m_refuel_vertex_hit_list.IsEmpty() )
	{
		char sline[21];

		cnt = m_refuel_vertex_hit_list.GetCount();
		if (cnt == 1)
		{
			vtx = m_refuel_vertex_hit_list.GetHead();
			onpoint = TRUE;
		}
		else
		{
			// build the string list
			next = m_refuel_vertex_hit_list.GetHeadPosition();
			while (next != NULL)
			{
				vtx =m_refuel_vertex_hit_list.GetNext(next);
				txt = new CString;
				*txt = vtx->m_label;
				*txt += "          ";
				*txt = txt->Left(8);
				*txt += ": ";
				rslt = refuel_usage_to_string(vtx->m_usage, sline);
				if (rslt)
 					*txt += sline;
				strlist.AddTail(txt);
			}
	
//			ar->clear_vertex_hit_list();
//			if (select_point(strlist, key, fuel_point_selected_usage))
			{
//				ar->list_hit_test(fuel_mouse_point);
				next = m_refuel_vertex_hit_list.GetHeadPosition();
				if (next != NULL)
				{
					vtx = m_refuel_vertex_hit_list.GetNext(next);
					onpoint = TRUE;
				}

//				notdone = TRUE;
//				while (notdone)
//				{
//					vtx = refuel_vertex_hit_list.GetNext(next);
//					tstr = vtx->m_label;
//					tstr.TrimRight();
//					if (!tstr.Compare(key) && !vtx->m_usage.Compare(fuel_point_selected_usage))
//					{
//						onpoint = TRUE;
//						notdone = FALSE;
//					}
//					if (next == NULL)
//						notdone = FALSE;
//				}
			}

			// delete the string list
			while (!strlist.IsEmpty())
			{
				txt = strlist.RemoveHead();
				delete txt;
			}
		}

		if (onpoint)
		{
			// build the usage list
			rslt = refuel_usage_to_string(vtx->m_usage, sline);
			pt_usage = sline;
			GEO_get_default_display(datum_type);
			if (!strcmp(datum_type, "SECONDARY"))
				GEO_get_secondary_datum(tstr_ch);
			else
				GEO_get_primary_datum(tstr_ch);
			pt_datum = "(";
			pt_datum += tstr_ch;
			pt_datum += ")";

			GEO_lat_lon_to_string(vtx->m_lat, vtx->m_lon, tstr_ch);
			pt_loc = tstr_ch;

			pt_navid = vtx->m_nav_id;
			pt_navid.TrimRight();
			if (pt_navid.GetLength() > 0)
			{
				pt_bearing = vtx->m_fix.Left(3);
				pt_dist = vtx->m_fix.Mid(3, 3);
				pt_bearing.TrimRight();
				pt_dist.TrimRight();
			}
		}
	}
*/

	if (ar->hit_test(NULL, m_fuel_mouse_point, &m_vertex_num, &hit_anchor_pt))
	{
		vtx = ar->get_vertex(m_vertex_num-1);
		if (vtx != NULL)
		{
//			char sline[81];
			CString usage;

			// build the usage list
			vtx->get_usage_string(pt_usage);
			GEO_get_default_display(datum_type, DATUM_TYPE_LEN);
			if (!strcmp(datum_type, "SECONDARY"))
				GEO_get_secondary_datum(tstr_ch, TSTR_LEN);
			else
				GEO_get_primary_datum(tstr_ch, TSTR_LEN);
			pt_datum = "(";
			pt_datum += tstr_ch;
			pt_datum += ")";

			GEO_lat_lon_to_string(vtx->m_lat, vtx->m_lon, tstr_ch, TSTR_LEN);
			pt_loc = tstr_ch;

			pt_navid = vtx->m_nav_id;
			pt_navid.TrimRight();
			if (pt_navid.GetLength() > 0)
			{
				pt_bearing = vtx->m_fix.Left(3);
				pt_dist = vtx->m_fix.Mid(3, 3);
				pt_bearing.TrimRight();
				pt_dist.TrimRight();
			}
		}
	}

	// init the info display
	msg = "Track/Orbit Information --- ";
	msg += ar->m_id;
   CRichTextFormatter richTextFormatter;


	edit = ar->m_id;
	edit += " ";
	edit += pt_usage;
   richTextFormatter.add_text(std::string(edit), INFO_DATA_COLOR, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	edit = pt_loc;
	edit += " ";
	edit += pt_navid;
	edit += " ";
	if (pt_bearing.GetLength() > 0)
	{
		edit += pt_bearing;
		edit += "/";
		edit += pt_dist;
		edit += " ";
	}
   richTextFormatter.add_text(std::string(edit), INFO_DATA_COLOR, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	edit = pt_datum;
	richTextFormatter.add_text(std::string(edit), INFO_DATUM_COLOR, INFO_DATA_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	edit = "Altitude Codes: AA = At or Above, AB = At or Below, AT = At, BT = Between";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_DATA_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	edit = "Refueling Alt 1: \t";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_DATA_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
//	m_pUtil->decode_altitude(ar->m_altdesc1, ar->m_fuel1alt1, ar->m_fuel1alt2, tstr);
//	edit = tstr;
	if (ar->m_altdesc1.Compare("BT"))
		edit.Format("%s  %s", ar->m_altdesc1, ar->m_fuel1alt1);
	else
		edit.Format("%s  %s to %s", ar->m_altdesc1, ar->m_fuel1alt1, ar->m_fuel1alt2);

	richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	if (ar->m_fuel2alt1.GetLength() > 0)
	{
		edit = "Refueling Alt 2: \t";
		richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_DATA_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
		if (ar->m_altdesc2.Compare("BT"))
			edit.Format("%s  %s", ar->m_altdesc2, ar->m_fuel2alt1);
		else
			edit.Format("%s  %s to %s", ar->m_altdesc2, ar->m_fuel2alt1, ar->m_fuel2alt2);

		richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
		richTextFormatter.add_break();
	}

	if (ar->m_fuel3alt1.GetLength() > 0)
	{
		edit = "Refueling Alt 3: \t";
		richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_DATA_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
		if (ar->m_altdesc3.Compare("BT"))
			edit.Format("%s  %s", ar->m_altdesc3, ar->m_fuel3alt1);
		else
			edit.Format("%s  %s to %s", ar->m_altdesc3, ar->m_fuel3alt1, ar->m_fuel3alt2);

		richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
		richTextFormatter.add_break();
	}

	if (ar->m_prfreq.GetLength() > 0)
	{
		edit = "Frequencies:\t";
//		edit = "";
		richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
//		edit = ar->m_prfreq.Left(3);
//		edit += ".";
		tstr = ar->m_prfreq;
//		tstr += "       ";
//		temp = tstr.Mid(3, 3);
//		len = temp.GetLength();
//		if ((temp.GetAt(len-2) == '0') && (temp.GetAt(len-1) == '0'))
//			temp = temp.Left(len-2);
//		if (temp.GetLength() > len-1)
//			if (temp.GetAt(len-1) == '0')
//				temp = temp.Left(len-1);
//		edit += temp;
		edit = tstr;
		edit += "Mhz";
		richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);

		edit = " (primary)  ";
		richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);

		if (ar->m_bufreq.GetLength() > 0)
		{
//			edit = ar->m_bufreq.Left(3);
//			edit += ".";
//			edit += ar->m_bufreq.Mid(3, 3);
			edit = ar->m_bufreq;
//			len = edit.GetLength();
//			if ((edit.GetAt(len-2) == '0') && (edit.GetAt(len-1) == '0'))
//				edit = edit.Left(len-2);
//			if (edit.GetLength() > len-1)
//				if (edit.GetAt(len-1) == '0')
//					edit = edit.Left(len-1);
			edit += "Mhz";
			richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);

			edit = " (backup) ";
			richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
		}
		richTextFormatter.add_break();
	}

	edit = "APN 69/134/135:   \t";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
	edit = ar->m_apnset;
	richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	edit = "APX 78:                 \t";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
	edit = ar->m_apxcode;
	richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	edit = "Tacan Channel:   \t";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
//	edit = ar->m_rectankchan.Left(2);
	edit = ar->m_tac_chan.Left(2);
//	edit += "X";
	richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	edit = " (receiver) ";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
//	temp = ar->m_rectankchan;
//	temp += "      ";
//	edit = temp.Mid(2, 3);
//	if (edit.GetAt(0) == '0')
//		edit = edit.Right(2);
//	edit += "X ";
	edit = ar->m_tac_chan.Right(3);
	richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	edit = " (tanker)";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	edit = "ATC:                   \t";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
	edit = ar->m_atc;
	richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	edit = "Scheduling Unit:";
	richTextFormatter.add_text(std::string(edit), INFO_LABEL_COLOR, INFO_LABEL_NORM_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();
	edit = "     ";
	edit += ar->m_unit;
	richTextFormatter.add_text(std::string(edit), data_color, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();

	edit = "Remarks:";
	richTextFormatter.add_text(std::string(edit), INFO_DATA_COLOR, INFO_DATA_BOLD_SIZE, INFO_DATA_BOLD_ATTRIB);
	richTextFormatter.add_break();
	edit = ar->m_remarks;
	richTextFormatter.add_text(std::string(edit), data_color, INFO_LABEL_NORM_SIZE, INFO_LABEL_NORM_ATTRIB);
	richTextFormatter.add_break();
	
   CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, richTextFormatter.GetRichTextString().c_str(), msg, this, MAP_INFO_RICH_TEXT);

   return SUCCESS;
}
// end of show_info

// *****************************************************************
// *****************************************************************

void C_ar_edit::normalize_id(CString id, CString direction, CString &norm_id)
{
	CString dir, tstr;

	norm_id = id;
	norm_id.TrimRight();
	dir = direction;
	dir.TrimRight();
	tstr = norm_id.Right(dir.GetLength());
	if (tstr.Compare(dir))
	{
		norm_id += " ";
		norm_id += dir;
	}
	norm_id.TrimRight();
}
// end of normalize_id

// *****************************************************************
// *****************************************************************

BOOL C_ar_edit::find_id_in_leglist(CString id, 	
				CList <C_dafroute_leg*, C_dafroute_leg*> & leglist)
{
	POSITION next;
	BOOL notdone, found;
	C_dafroute_leg *leg;

	next = leglist.GetHeadPosition();
	notdone = TRUE;
	found = FALSE;
	while (notdone)
	{
		leg = leglist.GetNext(next);
		if (!leg->m_pt_id.Compare(id))
		{
			found = TRUE;
			notdone = FALSE;
			continue;
		}
		if (next == NULL)
			notdone = FALSE;
	}
	return found;
}
// end of find_id_in_leglist

// *****************************************************************
// *****************************************************************

int C_ar_edit::draw(ActiveMap* map)
{
	C_ar_obj *obj, *selected_obj;
	CFvwUtil *futil = CFvwUtil::get_instance();
	POSITION obj_next, select_pos;
	POSITION obj_tail;
	BOOL first;
	int cnt;
	int type_num, old_type_num;
//	boolean_t draw_labels;
//	int k;
//	BOOL background;
	MapScale key_scale, cur_scale;
	CString sdata;
//	char tstr[21], rtename[81];
//	char numstr[3];

	selected_obj = NULL;

	CDC* dc = map->get_CDC();

	// compare the current map scale to display threshold
	key_scale = get_smallest_scale();
	cur_scale = map->scale();
	if (key_scale > cur_scale)
	{
		m_map_displayed = FALSE;
		if (m_point_selected)
			CRemarkDisplay::close_dlg();
		m_point_selected = FALSE;

		if (m_bEdit)
			set_editor_mode(MM_AR_SELECT);

		return SUCCESS;
	}
	else
		m_map_displayed = TRUE;

 	cnt = m_obj_list.GetCount();
	if (cnt < 1)
		return SUCCESS;


	obj_tail = m_obj_list.GetTailPosition();
	obj_next = m_obj_list.GetHeadPosition();

	type_num = 0;
	old_type_num = 0;
	first = TRUE;
	select_pos = obj_tail;
	cnt = 0;
	while (obj_tail != NULL)
	{
		cnt++;
	
//		// check for interrupt
//		if ((cnt % 10) == 0)
//		{
//			if (FVW_is_draw_interrupted())
//			{
//				return FAILURE;
//			}
//		}

		obj = (C_ar_obj *)m_obj_list.GetPrev(obj_tail);
		obj->draw(map);
		cur_obj = obj;
	}

	set_valid(TRUE);

	return SUCCESS;
}
// end of draw

// *****************************************************************
// *****************************************************************

BOOL C_ar_edit::plot_option(char ch)
{
	BOOL rslt;

	rslt = FALSE;

	switch(ch)
	{
		case 'A': // Alert  
			if (m_data[0] == 'Y')
				rslt = TRUE;
			break;
		case 'D': 	// Danger
			if (m_data[1] == 'Y')
				rslt = TRUE;
			break;
		case 'M': 	// Military Operation Area 
			if (m_data[2] == 'Y')
				rslt = TRUE;
			break;
		case 'P': 	// Prohibited 
			if (m_data[3] == 'Y')
				rslt = TRUE;
			break;
		case 'R': 	// Restricted 
			if (m_data[4] == 'Y')
				rslt = TRUE;
			break;
		case 'W': 	// Warning
			if (m_data[5] == 'Y')
				rslt = TRUE;
			break;
		case 'T': 	// Temporary Reserved Airspace (TRA)
			if (m_data[6] == 'Y')
				rslt = TRUE;
			break;
	}
	return rslt;
}
// end of plot_option

// *****************************************************************
// *****************************************************************

int C_ar_edit::open(ActiveMap* map)
{

	return SUCCESS;
}
// end of open

// *****************************************************************
// *****************************************************************

int C_ar_edit::get_route_geo(CString route_id, double *lat, double *lon)
{
	BOOL done;
	degrees_t tlat, tlon;
	d_geo_t bnd_ll, bnd_ur;
	CString key_id, test_id;
	CFvwUtil *futil = CFvwUtil::get_instance();
	BOOL found;
	C_ar_obj *obj;
	CString tstr;
	POSITION next;

 	key_id = route_id;
//	key_id.TrimRight();

	found = FALSE;
	next = m_obj_list.GetHeadPosition();
	while (next != NULL)
	{
		obj = m_obj_list.GetNext(next);
		test_id = obj->m_id;

		if (test_id.Compare(key_id))
		{
			continue;
		}
		found = TRUE;
		bnd_ll.lat = obj->m_bnd_ll.lat;
		bnd_ll.lon = obj->m_bnd_ll.lon;
		bnd_ur.lat = obj->m_bnd_ur.lat;
		bnd_ur.lon = obj->m_bnd_ur.lon;
		futil->compute_center_geo(bnd_ll, bnd_ur, &tlat, &tlon);
		*lat = tlat;
		*lon = tlon;
		done = TRUE;
	}

	if (found)
		return SUCCESS;
	else 
		return FAILURE;
}
// end of get_route_geo

// *****************************************************************
// *****************************************************************

// usage codes:
//  IP - Air Refueling Initial Point
//  CP - Air Refueling Control Point
//  NC - Navigation Check Point
//  EX - Exit Point
//  ET - Entry Point (Anchors Only)
//  AN - Anchor Point (Anchors Only)
//  AP - Anchor Pattern (Anchors Only)

BOOL C_ar_edit::refuel_usage_to_string(CString usage, char * txt, int txt_len)
{
	if (!usage.Compare("IP"))
	{
		strcpy_s(txt, txt_len, "Initial Point");
		return TRUE;
	}
	if (!usage.Compare("CP"))
	{
		strcpy_s(txt, txt_len, "Control Point");
		return TRUE;
	}
	if (!usage.Compare("NC"))
	{
		strcpy_s(txt, txt_len, "Nav Check Pt");
		return TRUE;
	}
	if (!usage.Compare("EX"))
	{
		strcpy_s(txt, txt_len, "Exit Point");
		return TRUE;
	}
	if (!usage.Compare("ET"))
	{
		strcpy_s(txt, txt_len, "Entry Point");
		return TRUE;
	}
	if (!usage.Compare("AN"))
	{
		strcpy_s(txt, txt_len, "Anchor Point");
		return TRUE;
	}
	if (!usage.Compare("AP"))
	{
		strcpy_s(txt, txt_len, "Anchor Pattern");
		return TRUE;
	}
	return FALSE;
}
// end of refuel_usage_to_string

// *****************************************************************
// *****************************************************************

BOOL C_ar_edit::refuel_string_to_usage(CString txt, CString & usage)
{
	usage = "";

	if (!txt.Compare("Initial Point"))
		usage = "IP";
	if (!txt.Compare("Control Point"))
		usage = "CP";
	if (!txt.Compare("Nav Check Pt"))
		usage = "NC";
	if (!txt.Compare("Exit Point"))
		usage = "EX";
	if (!txt.Compare("Entry Point"))
		usage = "ET";
	if (!txt.Compare("Anchor Point"))
		usage = "AN";
	if (!txt.Compare("Anchor Pattern"))
		usage = "AP";

	if (usage.GetLength() > 0)
		return TRUE;
	else
		return FALSE;
}
// end of refuel_string_to_usage

int C_ar_edit::file_new()
{
   // build the instance filename from the template
   CString defaultDir = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_TrackOrbit)->fileTypeDescriptor.defaultDirectory;
   m_fileSpecification.Format(C_ar_edit::filename_template(), defaultDir, C_ar_edit::m_ovl_cnt++);

   return SUCCESS;
}

int C_ar_edit::open(const CString & filename)
{
	int rslt;

   m_fileSpecification = filename;
	rslt = load(filename);

	return rslt;
}
// end of open


// ******************************************************************************
// *****************************************************************

int C_ar_edit::save_as(const CString & pathname, long nSaveFormat)
{
	int rslt;
//	int pos;
	CString name, ext;

	if (m_in_poly_mode)
		end_poly_line();

	// strip junk off the end of the filename
	name = pathname;
	name.MakeLower();
//	pos = name.Find(".");
//	name = name.Left(pos+1);
//	name += "uar";

	// save file to new name
	CWaitCursor wait;
	
	rslt = save_to_file(name, nSaveFormat);
	
	if (rslt != SUCCESS)
	{
		AfxMessageBox("Error saving file.");
		ERR_report("Error saving file.");
	}
	else
	{
		m_filename = name;
		m_ovlname = name;
		m_fileSpecification = name;
	}
	return rslt;
}
// end of save_as(const CString & pathname, CString selected_filter)


// *****************************************************************
// *****************************************************************

// IMPLEMENT_DYNAMIC(C_dafnav_ovl, StaticOverlay)

int C_ar_edit::clear_ovl()
{
   // remove all points from trail
	remove_all();

   return SUCCESS;
}
// end of clear_all

// *****************************************************************
// *****************************************************************

void C_ar_edit::remove_all()
{
	POSITION next, pos;

	C_ar_obj *obj;

	m_fuel_select_id = "";

	next = m_obj_list.GetHeadPosition();
	while (next != NULL)
	{
		pos = next;
		obj = m_obj_list.GetNext(next);
		if (m_point_selected && obj->m_selected)
			normalize_id(obj->m_id, obj->m_direction, m_fuel_select_id);
		delete obj;
		obj = NULL;
		m_obj_list.RemoveAt(pos);
	}
}
// end of remove_all

// *****************************************************************
// *****************************************************************

BOOL C_ar_edit::remove_route(C_ar_obj* rte)
{
	POSITION position;
   
	if (rte == NULL)
		return FALSE;

	// search for icon in the list
	position = m_obj_list.Find(rte);
	if (position == NULL)
		return FALSE;

	// remove the icon from the list
	m_obj_list.RemoveAt(position);

	delete rte;
	// if m_anchor_dragger is currently the same pointer as rte, we must null it also
	if (m_anchor_dragger == rte)
		m_anchor_dragger = NULL;

	rte = NULL;

	return TRUE;
}

// end of remove_route

// *****************************************************************
// *****************************************************************

boolean_t C_ar_edit::test_snap_to(ViewMapProj* map, CPoint point)
{
	if (m_nMode == MM_AR_NAVAID)
		return FALSE;

	if (m_bEdit)
		return FALSE;

	BOOL hit = FALSE;
	C_ar_obj *obj;
	C_ar_point *vtx;

	obj = (C_ar_obj*) hit_test(map, point);
	if (obj != NULL)
		hit = obj->hit_test(point, &vtx);

	return hit;
}
// end of test_snap_to

// *****************************************************************
// *****************************************************************

boolean_t C_ar_edit::do_snap_to(ViewMapProj* map, CPoint point, 
      CList<SnapToInfo *, SnapToInfo *> &snap_to_list)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	C_ar_obj *obj;
	C_ar_point *vtx;
	SnapToInfo *info;
	CString tstr, tstr2;
	char area_code;
	int type, len;
	POSITION next;
	BOOL found = FALSE;
	CArUtil util;

	if (m_nMode == MM_AR_NAVAID)
		return FALSE;

	m_fuel_selected_hit_test = TRUE;
	obj = (C_ar_obj*) hit_test(map, point);
	m_fuel_selected_hit_test = FALSE;
	if (obj == NULL)
		return FALSE;

	next = m_refuel_vertex_hit_list.GetHeadPosition();
	while (next != NULL)
	{
		vtx = m_refuel_vertex_hit_list.GetNext(next);

		// create a snap to object 
	   info = new SnapToInfo();

		info->m_lat = vtx->m_lat;
		info->m_lon = vtx->m_lon;
		strcpy_s(info->m_src_datum, 6, "WGS84");

		// start of getting freq and channel from navaid database
#ifdef GOV_RELEASE
		CString nav_ctry_code, nav_key_code;
		CString nav_id, nav_type;
		double tf;
		int pos;

		nav_id = vtx->m_nav_id;
		pos = nav_id.Find("/", 0);
		if (pos > 0)
			nav_id = nav_id.Left(pos);
		nav_id.TrimRight();

		if (nav_id.GetLength() > 0)
		{
			nav_type = vtx->m_nav_type;
			nav_ctry_code = vtx->m_nav_country;
			nav_key_code = vtx->m_nav_key;

         // TODO: use IFVDataSource to obtain navaid record from database
			if (0) //!lookup.get_navaid_record(nav_id, nav_type, nav_ctry_code, 
						//								nav_key_code, &nav_rec))
			{
				ERR_report("Could not find Navaid in database");
			}
			else
			{
				//tstr = nav_rec.m_FREQ;
				tstr.TrimRight();
				if (tstr.GetLength() > 0)
				{
					tstr += "        ";
					tf = atof(tstr.Left(7));
					tf = tf / 1000.0;
					if (tstr[7] == 'K')
						tf = tf / 1000.0;
					info->m_nav_freq_MHz = (float) tf;
				}

//				tstr = nav_rec.m_CHANNEL;
				tstr.TrimRight();
				if (tstr.GetLength() > 0)
				{
					tstr += "    ";
					tf = atof(tstr.Left(3));
					if (tstr[3]	== 'X')
						tf = -tf;
					info->m_nav_chan = (float) tf;
				}
			}
		}
      #endif

		// build fix string
		normalize_id(obj->m_id, obj->m_direction, tstr);
		tstr += " ";

		// todo bring up a dialog if more than one usage
		tstr += *(vtx->m_usage.GetHead());

		strncpy_s(info->m_fix_rs, 13, tstr.GetBuffer(13), 13);
		info->m_fix_rs[12] = '\0';

		// build description string only if both m_fix and m_nav_id are not
      // empty, otherwise leave it empty
      tstr = vtx->m_fix;
      tstr.TrimRight();
      if (tstr.GetLength())
      {
         tstr2 = vtx->m_nav_id;
         tstr2.TrimRight();
         if (tstr2.GetLength())
         {
		      tstr = obj->m_icao.Left(1);
            tstr += tstr2;
      		tstr += "/";
		      type = atoi(vtx->m_nav_type);
            //		tstr2 = futil.get_navaid_usage(type);
            tstr2 = util.get_navaid_usage(type);
		      tstr += tstr2;

		      // disregard the tenths like CFPS
		      tstr2 = vtx->m_fix.Left(3);
		      tstr2 += vtx->m_fix.Mid(3, 3);

		      tstr += tstr2;
		      len = tstr.GetLength();
		      strncpy_s(info->m_description_rs, 41, tstr.GetBuffer(len), len);
		      info->m_description_rs[len] = '\0';
         }
      }

		tstr = obj->m_country;
		tstr += "  ";
		area_code = tstr[0];
		strncpy_s(info->m_country_code, 3, tstr.GetBuffer(2), 2);
		info->m_country_code[2] = '\0';
		info->m_area_code = area_code;

		// change the source
		info->m_pnt_src_rs = SnapToInfo::AIR_REFUEL;

		// change the type of object
		strcpy_s(info->m_icon_type, 11, "AirRefuel");
 		strcpy_s(info->m_icon_description, 51, info->m_fix_rs);
		if (info->m_description_rs[0] != '\0')
		{
			strcat_s(info->m_icon_description, 51, ", ");
			strcat_s(info->m_icon_description, 51, info->m_description_rs);
		}

		// create the db_lookup field
		tstr = obj->m_id;
		tstr += ",";
		tstr += vtx->m_direction;
		tstr += ",";
		tstr += vtx->m_seq_num;
		strcpy_s(info->m_db_lookup_rs, 41, tstr);

      // add the snap to object to the list
		snap_to_list.AddTail(info);
		found = TRUE;
	}
	return found;
}
// end of do_snap_to

// *****************************************************************
// *****************************************************************

// add commands to menu
void C_ar_edit::menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list)
{
	POSITION next;
	C_ar_obj *obj = NULL;
	C_ar_point *pt = NULL;
	int k, cnt, ptnum;
	BOOL hit_anchor_pt;

	m_fuel_mouse_point = point;

	map->surface_to_geo(point.x, point.y, &m_cursor_lat, &m_cursor_lon);

	k = 0;
	if (!m_obj_list.IsEmpty())
	{
		next = m_obj_list.GetHeadPosition();
		do
		{
			// get this item and the position of the next one
			obj = m_obj_list.GetNext(next);
	   		if (obj->hit_test(map, point, &ptnum, &hit_anchor_pt))
			{
				CString msg;
				msg = "Info: ";
				msg += obj->m_id;
//				msg += obj->m_label;
				msg.TrimRight();
				msg += "...";
				strncpy_s(fuel_label[k], 81, msg, 80);
				list.AddTail(new CCIconMenuItem(fuel_label[k], (C_icon*) obj, &point_info));
				if (k<19)
					k++;
				if (m_bEdit)
				{
					if (obj->m_type == 'A')
						msg = "Edit Orbit Info: ";
					else
						msg = "Edit Track Info: ";
					msg += obj->m_id;
					msg.TrimRight();
					strncpy_s(fuel_label[k], 81, msg, 80);
					list.AddTail(new CCIconMenuItem(fuel_label[k], (C_icon*) obj, &edit_route_info));
					if (k<19)
						k++;
				}
				obj->hit_test(point, &pt);
				if (m_bEdit && (pt != NULL))
				{
					m_menu_point = pt;
					msg = "Edit Point Info: ";
					msg += obj->m_id;
					msg.TrimRight();
					strncpy_s(fuel_label[k], 81, msg, 80);
					list.AddTail(new CCIconMenuItem(fuel_label[k], (C_icon*) obj, &edit_point_info));
					if (k<19)
						k++;
					m_menu_point = pt;
					if (obj->m_type == 'T')
					{
						msg = "Delete Point: ";
						msg += obj->m_id;
						msg.TrimRight();
						strncpy_s(fuel_label[k], 81, msg, 80);
						list.AddTail(new CCIconMenuItem(fuel_label[k], (C_icon*) obj, &delete_point));
						if (k<19)
							k++;
					}
				}
//				if (obj->m_selected)
//				{
//					msg = "Add Point to: ";
//					msg += obj->m_id;
//					msg.TrimRight();
//					strncpy(fuel_label[k], msg, 80);
//					list.AddTail(new CFVMenuNode(fuel_label[k], (C_icon*) obj, &add_point));
//					if (k<19)
//						k++;
//				}
				cnt = obj->m_point_list.GetCount();
//				if (cnt == 1)
//				{
//					msg = "Display: ";
//					msg += obj->m_id;
//					msg.TrimRight();
//					strncpy(fuel_label[k], msg, 80);
//					list.AddTail(new CFVMenuNode(fuel_label[k], (C_icon*) obj, &add_point));
//					if (k<19)
//						k++;
//				}
//				else
//				{
//					if (!m_display_all_routes)
//					{
//						msg = "Hide: ";
//						msg += obj->m_id;
//						msg.TrimRight();
//						strncpy(fuel_label[k], msg, 80);
//						list.AddTail(new CFVMenuNode(fuel_label[k], (C_icon*) obj, &delete_point));
//						if (k<19)
//							k++;
//					}
//				}
				if (m_bEdit)
				{
					msg = "Delete Track: ";
					msg += obj->m_id;
					msg.TrimRight();
					strncpy_s(fuel_label[k], 81, msg, 80);
					list.AddTail(new CCIconMenuItem(fuel_label[k], (C_icon*) obj, &delete_route));
					if (k<19)
						k++;
				}
			}
		} while (next != NULL);
	}

/*
   C_ar_obj * pt = (C_ar_obj *)hit_test(view, point);

   if (pt)
   {
      list.AddTail(new CFVMenuNode("Get Refuel Info...", (C_icon*) pt, &point_info));
   }
*/
}
// end of menu

// *****************************************************************
// *****************************************************************

void C_ar_edit::edit_route_info(ViewMapProj *map, C_icon *pt)
{
	C_ar_obj *obj;
	CUserArEditInfoDlg dlg;
	int rslt;
	
	obj = (C_ar_obj*) pt;
	
	dlg.set_route_ptr(obj);
	rslt = dlg.DoModal();
	if (rslt == IDOK)
	{
		C_ar_edit *ovl = get_current_overlay();
		if (ovl != NULL)
			ovl->set_modified(TRUE);

		obj->make_anchor(map);
		obj->invalidate();
	}
}
// end of edit_route_info

// *****************************************************************
// *****************************************************************

void C_ar_edit::edit_anchor_params(ViewMapProj *map, C_icon *pt)
{
	C_ar_obj *obj;
	CUserArAnchorParamsDlg dlg;
//	double length, width;
	int rslt;

	obj = (C_ar_obj*) pt;
	
	dlg.set_route_ptr(obj);
	rslt = dlg.DoModal();
	if (rslt == IDOK)
	{
		obj ->make_anchor(map);
		obj->invalidate();
	}
}
// end of edit_anchor_params

// *****************************************************************
// *****************************************************************

void C_ar_edit::edit_point_info(ViewMapProj *map, C_icon *pt)
{
	edit_point_info_dlg(map, (C_ar_obj*) pt, m_menu_point);
}
// end of edit_point_info

// *****************************************************************
// *****************************************************************

void C_ar_edit::add_point(ViewMapProj *map, C_icon *pt)
{
	edit_point_info_dlg(map, (C_ar_obj*) pt, NULL);
}
// end of add_point

// *****************************************************************
// *****************************************************************

void C_ar_edit::edit_point_info_dlg(ViewMapProj *map, C_ar_obj *obj, C_ar_point *ar_pt)
{
	int rslt;
	CString title;
	BOOL is_anchor_pt = FALSE;

	if (ar_pt == NULL)
		title = "Add Point";
	else
		title = "Edit Point";

	CUserARPointSheetDlg dlg(title);
	CUserARPointInfoPageDlg *info_page = NULL;
	CUserARPointLocPageDlg *loc_page = NULL;
	BOOL loc_ok = FALSE;

	info_page = new CUserARPointInfoPageDlg;
	info_page->set_route_ptr(obj);
	info_page->set_point_ptr(ar_pt);
	info_page->set_map(map);
	info_page->set_location(m_cursor_lat, m_cursor_lon);
	dlg.AddPage(info_page);

	// determine if a location page is appropriate
	if ((obj->m_type == 'A') && (ar_pt != NULL))
	{
		BOOL unique;

		// is it one of the anchor position points
		if (ar_pt->has_usage_string("AP", &unique))
		{
			// is is the anchor point
			if (ar_pt->m_anchor_point)
//			if (ar_pt->has_usage_string("AN", &unique))
			{
				ASSERT(ar_pt->m_anchor_point);
				loc_ok = TRUE;
				is_anchor_pt = TRUE;
			}
		}
		else
			loc_ok = TRUE;
	}
	else
		loc_ok = TRUE;

	if (loc_ok)
	{
		loc_page = new CUserARPointLocPageDlg;
		loc_page->set_route_ptr(obj);
		loc_page->set_point_ptr(ar_pt);
		loc_page->set_location(m_cursor_lat, m_cursor_lon);
		loc_page->set_map(map);
		dlg.AddPage(loc_page);
	}

	rslt = dlg.DoModal();
	if (rslt == IDOK)
	{
		C_ar_edit *ovl = get_current_overlay();
		if (ovl != NULL)
      {
         OVL_get_overlay_manager()->InvalidateOverlay(ovl);
			ovl->set_modified(TRUE);
      }
	}

	if (info_page != NULL)
		delete info_page;
	if (loc_page != NULL)
		delete loc_page;
}
// end of edit_point_info_dlg

// *****************************************************************
// *****************************************************************

void C_ar_edit::delete_point(ViewMapProj *map, C_icon *pt)
{
	C_ar_obj *obj;
	
	if (!m_bEdit)
		return;

	obj = (C_ar_obj*) pt;
	
	obj->delete_route_point(m_menu_point);
	C_ar_edit *ovl = get_current_overlay();
	if (ovl != NULL)
		ovl->set_modified(TRUE);

	OVL_get_overlay_manager()->InvalidateOverlay(ovl);
}
// end of delete_point

// *****************************************************************
// *****************************************************************

void C_ar_edit::delete_route(ViewMapProj *map, C_icon *pt)
{
	C_ar_obj *obj, *tobj;
	POSITION next, oldpos;
	C_ar_edit *ar_ovl;

	obj = (C_ar_obj*) pt;
	
	ar_ovl = get_current_overlay();
	if (ar_ovl == NULL)
	{
		ASSERT(0);
		return;
	}

	next = ar_ovl->m_obj_list.GetHeadPosition();
	while (next != NULL)
	{
		oldpos = next;
		tobj = ar_ovl->m_obj_list.GetNext(next);
		if (tobj == obj)
		{
			ar_ovl->m_obj_list.RemoveAt(oldpos);
			delete obj;
			obj = NULL;
			next = NULL;
		}
	}

	C_ar_edit *ovl = get_current_overlay();
	if (ovl != NULL)
	{
		ovl->set_modified(TRUE);
		ovl->m_anchor_dragger = NULL;
	}

	OVL_get_overlay_manager()->InvalidateOverlay(ovl);
}
// end of delete_route

// *****************************************************************
// *****************************************************************

void C_ar_edit::point_info(ViewMapProj *map, C_icon *pt)
{
	C_ar_edit *thisovl;

	// call info display routine
	thisovl =  (C_ar_edit *) pt->m_overlay;
	if (!valid_overlay(thisovl))
		return;

	thisovl->show_info(pt);
}
// end of point_info


// *****************************************************************
// *****************************************************************

MapScale C_ar_edit::get_smallest_scale()
{ 
	CString	display_above = PRM_get_registry_string("ArEdit", 
		"ArEditDisplayAbove", "1:5 M");
	set_smallest_scale(MAP_get_scale_from_string(display_above));
	return m_smallest_scale; 
}


// *****************************************************************
// *****************************************************************

void C_ar_edit::set_smallest_scale(MapScale scale) 
{ 
	m_smallest_scale = scale; 
}

// *****************************************************************
// *****************************************************************

MapScale C_ar_edit::get_smallest_labels_scale()
{ 
	CString display_above = PRM_get_registry_string("ArEdit", 
		"ArEditLabelThreshold", "1:250 K");

	set_smallest_labels_scale(MAP_get_scale_from_string(display_above));
	return m_smallest_labels_scale; 
}
// end of get_smallest_labels_scale

// *****************************************************************
// *****************************************************************

void C_ar_edit::set_smallest_labels_scale(MapScale scale) 
{ 
	m_smallest_labels_scale = scale; 
}

// *****************************************************************
// *****************************************************************

void C_ar_edit::clear_selects() 
{ 
	POSITION next;
	C_ar_obj* obj;

	if (m_obj_list.IsEmpty())
		return;

	next = m_obj_list.GetHeadPosition();
	while (next != NULL)
	{
		obj = m_obj_list.GetNext(next);
		obj->clear_selects();
	}
}
// end of clear_selects

// *****************************************************************
// *****************************************************************

void C_ar_edit::invalidate_and_clear_selects() 
{ 
	POSITION next;
	C_ar_obj* obj;
	CString tid;

	if (m_obj_list.IsEmpty())
		return;

	next = m_obj_list.GetHeadPosition();
	while (next != NULL)
	{
		obj = m_obj_list.GetNext(next);
		normalize_id(obj->m_id, obj->m_direction, tid);
//		if (!tid.Compare(m_fuel_select_id))
		if (obj->m_selected)
			obj->invalidate();
		obj->clear_selects();
	}
	m_fuel_select_id = "";
	m_cur_obj = NULL;
}
// end of invalidate_and_clear_selects

// *****************************************************************
// *****************************************************************

void C_ar_edit::release_focus()
{
	invalidate_and_clear_selects();
}
// end of release_focus

// *****************************************************************
// *****************************************************************

// determine if the point hits the object's selection region(s)

C_ar_obj *C_ar_edit::line_hit_test(MapProj* map, CPoint point)
{
	int found_vertex;
	POSITION prev;
	C_ar_obj *obj;
	BOOL hit_anchor_pt;

	if (!m_bGotPt)
		m_bGotHandle = FALSE;

	// search the overlay list first
	if (!m_obj_list.IsEmpty())
	{
		prev = m_obj_list.GetTailPosition();
		do
		{
			// get this item and the position of the next one
			obj = m_obj_list.GetPrev(prev);
			if (obj != NULL)
	   			if (obj->hit_test(map, point, &found_vertex, &hit_anchor_pt))
		   			return obj;
		} while (prev != NULL);
	}

	return NULL;
}
// end of line_hit_test

// *****************************************************************
// *****************************************************************

void C_ar_edit::set_line_hint(ViewMapProj* map, CPoint start_pt, CPoint cur_pt, HintText**hint)
{
	double lat1, lon1, lat2, lon2;
	double dist, ang;
	CString tstr;

	map->surface_to_geo(start_pt.x, start_pt.y, &lat1, &lon1);
	map->surface_to_geo(cur_pt.x, cur_pt.y, &lat2, &lon2);
	GEO_geo_to_distance(lat1, lon1, lat2, lon2, &dist, &ang);
	tstr.Format("%9.3f NM, %5.1f°", dist / 1852.0, ang);
	tstr.TrimLeft();
	m_hint.set_help_text(tstr);
	*hint = &m_hint;
}
// set_line_hint

// *****************************************************************
// *****************************************************************

void C_ar_edit::set_line_hint(ViewMapProj* map, CPoint start_pt, CPoint cur_pt, 
   CPoint end_pt, HintText**hint)
{
	double lat1, lon1, lat2, lon2, lat3, lon3;
	double dist, ang;
	CString tstr, tstr2;

	map->surface_to_geo(start_pt.x, start_pt.y, &lat1, &lon1);
	map->surface_to_geo(cur_pt.x, cur_pt.y, &lat2, &lon2);
	GEO_geo_to_distance(lat1, lon1, lat2, lon2, &dist, &ang);
	tstr.Format("%9.3f NM, %5.1f°", dist / 1852.0, ang);
	tstr.TrimLeft();
	tstr += "; ";
	map->surface_to_geo(end_pt.x, end_pt.y, &lat3, &lon3);
	GEO_geo_to_distance(lat3, lon3, lat2, lon2, &dist, &ang);
	tstr2.Format("%9.3f NM, %5.1f°", dist / 1852.0, ang);
	tstr2.TrimLeft();
	tstr += tstr2;
	m_hint.set_help_text(tstr);
//	m_hint.set_tool_tip(tstr);
	*hint = &m_hint;
}
// set_line_hint

// *****************************************************************
// *****************************************************************

int C_ar_edit::set_edit_on(boolean_t edit)
{
	if (m_bEdit == edit)
		return SUCCESS;

	if (m_in_poly_mode)
		end_poly_line();

	// remove the remark dialog
	CRemarkDisplay::close_dlg();

	m_nMode = MM_AR_SELECT;

	m_bEdit = edit;
	if (m_bEdit)
	{
		m_bGotPt = FALSE;
		m_cur_obj = NULL;
		get_default_cursor();
	}
	else
	{
//		m_nMode = MM_OVLKIT_NONE;
//		if (m_pCurOvl != NULL)
//			m_pCurOvl->clear_selects();

		// redraw everything in non_grayed mode
//		OVL_get_overlay_manager()->invalidate_all();

		//close and delete the editor dialog
//		if (m_edit_links_dlg != NULL) 
//		{
//			// make a bogus call to the dlg to save any changes made
//			m_edit_links_dlg->set_focus("Drawing Menu Links", &m_link_list);
//			m_edit_links_dlg->DestroyWindow(); 
//			delete m_edit_links_dlg;
//			m_edit_links_dlg = NULL;
//		}
	}

	C_ar_edit *ar_ovl = (C_ar_edit *)OVL_get_overlay_manager()->get_current_overlay();
	if (ar_ovl != NULL)
      if (ar_ovl->get_m_overlayDescGuid() == FVWID_Overlay_TrackOrbit)
			ar_ovl->invalidate_and_clear_selects();

   return SUCCESS;
}
// end of set_edit_on

// ****************************************************************
// ****************************************************************

void C_ar_edit::set_editor_mode(int mode)
{
	int oldmode = m_nMode;
	C_ar_edit *ar_ovl;

	ar_ovl = (C_ar_edit *)OVL_get_overlay_manager()->get_current_overlay();
	if (ar_ovl == NULL)
		return;

	// warn user that they can't draw things at this map scale
	if (!ar_ovl->m_map_displayed && (mode != MM_AR_SELECT))
	{
		AfxMessageBox("Track/Orbit edit tools are not supported at scales greater than\n"
			           "the current 'Hide overlay above' scale setting for Track/Orbit.\n\n"
			           "(See the Options command on the Overlay menu to change\n"
						  "this value.)");

      OVL_get_overlay_manager()->overlay_options_dialog(FVWID_Overlay_TrackOrbit);

		m_nMode = MM_AR_SELECT;
		return;
	}

	if ((mode == MM_AR_NAVAID) || (m_nMode == MM_AR_NAVAID))
		OVL_get_overlay_manager()->InvalidateOverlay(ar_ovl);

	m_nMode = mode;  

	if ((mode != MM_AR_TRACK) && (oldmode == MM_AR_TRACK))
		end_poly_line();
	else
		m_cur_obj = NULL;

	if ((mode == MM_AR_TRACK) || (mode == MM_AR_ANCHOR))
	{
		ar_ovl = (C_ar_edit *)OVL_get_overlay_manager()->get_current_overlay();
		if (ar_ovl != NULL)
			ar_ovl->invalidate_and_clear_selects();
	}
}

// ****************************************************************
// ****************************************************************

int C_ar_edit::get_editor_mode()
{
	return m_nMode;  
}

// ****************************************************************
// *****************************************************************

int C_ar_edit::pre_close(BOOL* cancel)
{
	int rslt = SUCCESS;

	if (m_in_poly_mode)
		end_poly_line();

   // indicate no cancel if cancel boolean was passed in
   if (cancel)
      *cancel = FALSE;

   return rslt;
}
// end of pre_close

// *****************************************************************
// ****************************************************************

void C_ar_edit::end_poly_line()
{
	C_ar_point *pt;

	if (m_cur_ovl == NULL)
		return;
	
	// make the last point an exit point
	if (m_cur_obj == NULL)
		return;

	if (m_cur_obj->m_created || (m_cur_obj->m_type != 'T'))
		return;

	if (m_cur_obj->m_point_list.IsEmpty())
		return;
	pt = m_cur_obj->m_point_list.GetTail();
	if (pt != NULL)
	{
		pt->clear_usages();
		pt->add_usage("EX");
		CRect rc(pt->m_pt.x-16, pt->m_pt.y-16, pt->m_pt.x+16, pt->m_pt.y+16);
		OVL_get_overlay_manager()->InvalidateOverlay(m_cur_ovl);
	}

	m_cur_obj->m_created = TRUE;

//	m_cur_ovl->m_bFirstPt = TRUE;
//	m_cur_ovl->m_bLineDraw = FALSE;
	m_cur_ovl->m_cur_obj = NULL;
	m_cur_ovl->m_in_poly_mode = FALSE;
	ReleaseCapture();

	m_cur_obj = NULL;

	m_cur_ovl = NULL;

	m_in_poly_mode = FALSE;
	m_cur_vertex_num = -1;
	get_default_cursor();

	m_in_poly_mode = FALSE;
}
// end of end_poly_line

// ****************************************************************
// ****************************************************************

C_ar_edit* C_ar_edit::get_current_overlay()
{
	C_overlay *ovl;
	C_ar_edit *ar_ovl;

	// get the current overlay
	ovl = OVL_get_overlay_manager()->get_current_overlay();
	if (ovl == NULL)
		return NULL;

	if (ovl->get_m_overlayDescGuid() != FVWID_Overlay_TrackOrbit)
		return NULL;

	ar_ovl = (C_ar_edit*) ovl;

	return ar_ovl;
}
// end of get_current_overlay

// ****************************************************************
// ****************************************************************

HCURSOR C_ar_edit::get_default_cursor()
{
	HCURSOR cursor;

	cursor = NULL;
	switch(m_nMode)
	{
		case MM_AR_SELECT:
			cursor = AfxGetApp()->LoadCursor(IDC_SELECT_CURSOR);
			break;
		case MM_AR_TRACK:
			cursor = AfxGetApp()->LoadCursor(IDC_ARTRACK_CURSOR);
			break;
		case MM_AR_ANCHOR:
			cursor = AfxGetApp()->LoadCursor(IDC_ARANCHOR_CURSOR);
			break;
		case MM_AR_NAVAID:
			cursor = AfxGetApp()->LoadCursor(IDC_ARNAVAID_CURSOR);
			break;
		case MM_AR_POINT:
			cursor = AfxGetApp()->LoadCursor(IDC_ARPOINT_CURSOR);
			break;
	}

	return cursor;
}
// end of get_default_cursor

// ****************************************************************
// ****************************************************************

void C_ar_edit::delete_selected()
{
	POSITION next;
	C_ar_obj *obj;
	BOOL rslt;

	// return if not in edit mode
	if (!m_bEdit)
		return;

	// remove and delete all lines in the line list
	if (!m_obj_list.IsEmpty())
	{
		next = m_obj_list.GetHeadPosition();
		while (next != NULL)
		{
			obj = m_obj_list.GetNext(next);
			if (obj->m_selected)
			{
				rslt = remove_route(obj);
				if (rslt)
					m_cur_obj = NULL;
			}
			else
			{
				obj->delete_selected();
			}
		}  
	}
	
//	edit_reset();

}
// end of delete_selected


// ****************************************************************
// ****************************************************************

BOOL C_ar_edit::valid_overlay(C_ar_edit *ovl)
{
	C_overlay *overlay;

	overlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TrackOrbit);
	if ((C_ar_edit*) overlay == ovl)
		return TRUE;

	while (overlay)
	{
		overlay = OVL_get_overlay_manager()->get_next_of_type(overlay, FVWID_Overlay_TrackOrbit);
		if ((C_ar_edit*) overlay == ovl)
			return TRUE;
	}

	return FALSE;
}
// end of vald_overlay

// ****************************************************************
// ****************************************************************

BOOL C_ar_edit::valid_object(C_ar_obj *obj)
{
	C_ar_obj *tobj;
	POSITION next;

	if (m_obj_list.IsEmpty())
		return FALSE;

	next = m_obj_list.GetHeadPosition();
	while (next != NULL)
	{
		tobj = m_obj_list.GetNext(next);
		if (tobj == obj)
			return TRUE;
	}

	return FALSE;
}
// end of vald_overlay

// ****************************************************************
// ************************************************************

// return the first (and only) selected icon
C_ar_obj* C_ar_edit::get_selected()
{
	POSITION previous;
	C_ar_obj *obj;

	if (m_obj_list.IsEmpty())
		return NULL;

	previous = m_obj_list.GetTailPosition();
	while (previous != NULL)
	{
		obj = m_obj_list.GetPrev(previous);
		if (obj->m_selected)
			return obj;
	}  
	return NULL;
}
// end of get_selected

// ****************************************************************
// ****************************************************************

CString C_ar_edit::get_navaid_type(CString usage_str)
{
	char usage;

	if (usage_str.GetLength() < 1)
		return "";

	usage = usage_str[0];

   switch(usage)
   {
      case 'T':   return "3";
      case 'V':   return "1";
      case 'N':   return "5";
      case 'M':   return "9";
      case 'R':   return "2";
      case 'E':   return "4";
      case 'B':   return "7";
      
      default:
           return "";   //not found
   }
}
// end of get_navaid_type

// ****************************************************************
// ****************************************************************

// return values: 0 = unselected, 1 = track selected, 2 = anchor selected

int C_ar_edit::enable_property_dialog_button()
{
	C_ar_obj *obj;

	obj = get_selected();
	if (obj == NULL)
		return 0;
	if (obj->m_type == 'T')
		return 1;
	else
		return 2;
}

// ****************************************************************
// ****************************************************************

void C_ar_edit::toggle_property_dialog()
{
	CView* view;
	ViewMapProj* map;

	// check so make sure we are in edit mode
	if (!m_bEdit)
		return;

	if (m_selected_obj == NULL)
		return;

	view = UTL_get_active_non_printing_view();
	if (view == NULL)
		return;
	map = UTL_get_current_view_map(view);
	if (map == NULL)
		return;  

	edit_route_info(map, (C_icon*) m_selected_obj);
}

// ****************************************************************
// ****************************************************************

int C_ar_edit::begin_drag(MapProj* map, CDC *dc, C_ar_obj *obj, CPoint point)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	double dist, ang;
	double tlat, tlon;
	int ax, ay;
	double alat, alon;

	if (m_anchor_dragger == NULL)
		return FAILURE;

	alat = m_anchor_dragger->m_anchor_pt_lat;
	alon = m_anchor_dragger->m_anchor_pt_lon;


	m_anchor_dragger->make_anchor(map);
	m_anchor_vtx = obj->find_anchor_point(); 

	ax = m_anchor_dragger->m_anchor_pt[m_anchor_vtx].x;
	ay = m_anchor_dragger->m_anchor_pt[m_anchor_vtx].y;

	map->surface_to_geo(point.x, point.y, &tlat, &tlon);
	GEO_geo_to_distance(alat, alon, tlat, tlon, &dist, &ang);
	m_start_angle = m_anchor_dragger->m_angle - ang;

	return SUCCESS;
}
// end of begin_drag

// ****************************************************************
// ****************************************************************

int C_ar_edit::move_drag(MapProj* map, CDC *dc, CPoint oldpt, CPoint newpt, HintText**hint)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	double distx, disty;
	double dist, ang;
	int ax, ay;
	double lat1, lon1, lat2, lon2;
	double alat, alon;
	CString tstr;

	if (m_anchor_dragger == NULL)
		return FAILURE;

	// erase the old image
	m_anchor_dragger->make_anchor(map);
	if (m_start_drag)
		m_anchor_dragger->draw_racetrack(map, dc, UTIL_COLOR_WHITE, FALSE, TRUE);

	alat = m_anchor_dragger->m_anchor_pt_lat;
	alon = m_anchor_dragger->m_anchor_pt_lon;

	map->surface_to_geo(oldpt.x, oldpt.y, &lat1, &lon1);
	map->surface_to_geo(newpt.x, newpt.y, &lat2, &lon2);

	map->geo_to_surface(alat, alon, &ax, &ay);

	GEO_geo_to_distance(alat, alon, lat2, lon2, &dist, &ang);
	m_anchor_dragger->m_angle = ang + m_start_angle;

	futil->pixels_to_km(map, ax, ay, newpt.x, newpt.y, &distx, &disty);
	if (m_originate)
		m_anchor_dragger->m_anchor_length = sqrt((distx*distx) + (disty*disty)) / 1.852;
	m_anchor_dragger->make_anchor(map);
	m_anchor_dragger->draw_racetrack(map, dc, UTIL_COLOR_WHITE, FALSE, TRUE);

	m_start_drag = TRUE;

	ang = m_anchor_dragger->m_angle;
	if (ang < 0.0)
		ang += 360.0;
	if (ang > 360.0)
		ang -= 360.0;
	tstr.Format("Inbound Course: %.0f degrees Magnetic", ang);
	m_hint.set_help_text(tstr);
	m_hint.set_tool_tip(tstr);
	*hint = &m_hint;

	return SUCCESS;
}
// end of move_drag

// ****************************************************************
// ****************************************************************

int C_ar_edit::end_drag(MapProj* map, CDC *dc, CPoint oldpt, CPoint newpt)
{
	m_anchor_dragger->draw_racetrack(map, dc, UTIL_COLOR_WHITE, FALSE, TRUE);

	return SUCCESS;
}
// end of end_drag

// ****************************************************************
// ****************************************************************

int C_ar_edit::begin_drag_all(MapProj* map, CDC *dc, C_ar_obj *obj, CPoint point)
{
	if (obj == NULL)
		return FAILURE;

	obj->draw_racetrack(map, dc, UTIL_COLOR_WHITE, FALSE, TRUE);
	m_anchor_dragger = obj;
	m_start_pt = point;
	m_orig_lat = obj->m_anchor_pt_lat;
	m_orig_lon = obj->m_anchor_pt_lon;
	ASSERT(GEO_valid_degrees(m_orig_lat, m_orig_lon));

	return SUCCESS;
}
// end of begin_drag

// ****************************************************************
// ****************************************************************

int C_ar_edit::move_drag_all(MapProj* map, CDC *dc, CPoint oldpt, CPoint newpt, HintText**hint)
{
	double ang, dist;
	double tlat, tlon, newlat, newlon;
	int ax, ay;
	CString tstr;
   const int BUF_LEN = 70;
	char buf[BUF_LEN];
	double dpp_lat, dpp_lon, dpp;

	if (m_anchor_dragger == NULL)
		return FAILURE;

	map->surface_to_geo(oldpt.x, oldpt.y, &tlat, &tlon);
	GEO_geo_to_distance(m_start_lat, m_start_lon, tlat, tlon, &dist, &ang);
	GEO_distance_to_geo(m_orig_lat, m_orig_lon, dist, ang, &newlat, &newlon);
	ASSERT(GEO_valid_degrees(newlat, newlon));
	m_anchor_dragger->m_anchor_pt_lat = newlat;
	m_anchor_dragger->m_anchor_pt_lon = newlon;
	m_anchor_dragger->make_anchor(map);
	m_anchor_dragger->draw_racetrack(map, dc, UTIL_COLOR_WHITE, FALSE, TRUE);
	map->geo_to_surface(newlat, newlon, &ax, &ay);
	m_anchor_dragger->draw_xor_anchor(dc, ax, ay);

	map->surface_to_geo(newpt.x, newpt.y, &tlat, &tlon);
	GEO_geo_to_distance(m_start_lat, m_start_lon, tlat, tlon, &dist, &ang);
	GEO_distance_to_geo(m_orig_lat, m_orig_lon, dist, ang, &newlat, &newlon);
	m_anchor_dragger->m_anchor_pt_lat = newlat;
	m_anchor_dragger->m_anchor_pt_lon = newlon;
	m_anchor_dragger->make_anchor(map);
	m_anchor_dragger->draw_racetrack(map, dc, UTIL_COLOR_WHITE, FALSE, TRUE);
	map->geo_to_surface(newlat, newlon, &ax, &ay);
	m_anchor_dragger->draw_xor_anchor(dc, ax, ay);

	map->get_vmap_degrees_per_pixel(&dpp_lat, &dpp_lon);
	dpp = dpp_lat < dpp_lon ? dpp_lat : dpp_lon;
	GEO_lat_lon_to_string(newlat, newlon, dpp, buf, BUF_LEN);
	tstr = "Anchor Point: ";
	tstr += buf;
	m_hint.set_help_text(tstr);
	m_hint.set_tool_tip(tstr);
	*hint = &m_hint;

	return SUCCESS;
}
// end of move_drag

// ****************************************************************
// ****************************************************************

int C_ar_edit::end_drag_all(MapProj* map, CDC *dc, CPoint oldpt, CPoint newpt)
{
	double ang, dist;
	double tlat, tlon, newlat, newlon;

	map->surface_to_geo(oldpt.x, oldpt.y, &tlat, &tlon);
	GEO_geo_to_distance(m_start_lat, m_start_lon, tlat, tlon, &dist, &ang);
	GEO_distance_to_geo(m_orig_lat, m_orig_lon, dist, ang, &newlat, &newlon);
	m_anchor_dragger->m_anchor_pt_lat = newlat;
	m_anchor_dragger->m_anchor_pt_lon = newlon;
	m_anchor_dragger->make_anchor(map);
	m_anchor_dragger->draw_racetrack(map, dc, UTIL_COLOR_WHITE, FALSE, TRUE);

	map->surface_to_geo(newpt.x, newpt.y, &tlat, &tlon);
	GEO_geo_to_distance(m_start_lat, m_start_lon, tlat, tlon, &dist, &ang);
	GEO_distance_to_geo(m_orig_lat, m_orig_lon, dist, ang, &newlat, &newlon);
	m_anchor_dragger->m_anchor_pt_lat = newlat;
	m_anchor_dragger->m_anchor_pt_lon = newlon;
	m_anchor_dragger->make_anchor(map);
	m_anchor_dragger->draw_racetrack(map, dc, UTIL_COLOR_WHITE, FALSE, TRUE);

	return SUCCESS;
}
// end of end_drag

// ****************************************************************
// *****************************************************************

void C_ar_edit::cancel_drag(ViewMapProj* map)
{
	CFvwUtil *futil = CFvwUtil::get_instance();

	CWindowDC dc(map->get_CView());

	RECT rc;

	if (m_cur_obj->m_type == 'T')
	{
		futil->line_xor(&dc, 1, m_start_pt, m_cur_pt);
	}

	if (m_cur_obj->m_type == 'A')
	{
		*m_cur_obj = *m_old_route;
		m_cur_obj->make_anchor(map);
	}

	m_cur_pt = m_start_pt;
	m_cur_lat = m_start_lat;
	m_cur_lon = m_start_lon;

   CWnd *pWnd = dc.GetWindow();
   if (pWnd != NULL)
   {
	   pWnd->GetWindowRect(&rc);
	   SetCursorPos(rc.left + m_cur_pt.x, rc.top + m_cur_pt.y);
   }

   m_bDragging = FALSE;

	OVL_get_overlay_manager()->InvalidateOverlay(this);
}
// end of cancel_drag

// *****************************************************************
// *****************************************************************
// *****************************************************************

// NNNNNNNNNNNNNNNNNNNNNNNNNNNNN  end of DAFBOUND.CPP  NNNNNNNNNNNNNNNNNNNNNNNNNNNN
