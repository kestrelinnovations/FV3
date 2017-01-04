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



// dafroute.h


// usage codes:
//  A - Alternate Entry Point
//  B - Alternate Exit Point
//  C - Alternate Entry/Exit Point
//  S - Entry Point (Starting Point)
//  T - Turning Point
//  X - Exit Point (Ending Point)


// defines and types for DAFIF routes

#ifndef DAFROUTE_H
#define DAFROUTE_H 1

#include "common.h"
#include "geo_tool_d.h"

//
// forward declarations
//
class MapProj;
class ActiveMap;

#define MTR_TYPE_IR 1
#define MTR_TYPE_VR 2
#define MTR_TYPE_SR 4

// ********************************************************************

class C_dafroute_seg
{
public:
   C_dafroute_seg()
	{
		m_geo.lat = 0.0;
		m_geo.lon = 0.0;
		m_geo_left.lat = 0.0;
		m_geo_left.lon = 0.0;
		m_geo_right.lat = 0.0;
		m_geo_right.lon = 0.0;
		m_angle = 0.0;
		m_width_left = 0.0;
		m_width_right = 0.0;
		m_width_end_left = 0.0;
		m_width_end_right = 0.0;
		m_new_line = FALSE;
	}

	d_geo_t m_geo;
	d_geo_t m_geo_left;
	d_geo_t m_geo_right;
	double m_angle;
	double m_width_left;
	double m_width_right;
	double m_width_end_left;
	double m_width_end_right;
	BOOL m_new_line;
};

// ********************************************************************

class C_dafroute_leg
{
public:
   C_dafroute_leg()
   {
		m_width_left = 0.0;
		m_width_right = 0.0;
		m_width_end_left = 0.0;
		m_width_end_right = 0.0;
		m_turn_radius = 0.0;
		m_turn_direction = 'N';
		m_prev_width_left = 0.0;
		m_prev_width_right = 0.0;
		m_geo.lat = 0.0;
		m_geo.lon = 0.0;
		m_left_center.lat = 0.0;
		m_left_center.lon = 0.0;
		m_right_center.lat = 0.0;
		m_right_center.lon = 0.0;
		m_prev_pt.lat = 0.0;
		m_prev_pt.lon = 0.0;
		m_next_pt.lat = 0.0;
		m_next_pt.lon = 0.0;
		m_left_rollin.lat = 0.0;
		m_left_rollin.lon = 0.0;
		m_left_rollout.lat = 0.0;
		m_left_rollout.lon = 0.0;
		m_right_rollin.lat = 0.0;
		m_right_rollin.lon = 0.0;
		m_right_rollout.lat = 0.0;
		m_right_rollout.lon = 0.0;
		m_rollin.lat = 0.0;
		m_rollin.lon = 0.0;
		m_rollout.lat = 0.0;
		m_rollout.lon = 0.0;
		m_end_point = FALSE;
		m_start_point = FALSE;
		m_left_intersect = FALSE;
		m_right_intersect = FALSE;
		m_single_vertex = FALSE;
		m_next_pt_id = " ";
		m_count = 0;
		m_delta = 1.0;
   }

   ~C_dafroute_leg()
   {
   }

	CString m_id;
	CString m_pt_id;
	CString m_next_pt_id;
	CPoint m_pt;
	CPoint m_pt2;
	CString m_x_alt_desc;
	CString m_x_alt1;
	CString m_x_alt2;
	CString m_enr_alt_desc;
	CString m_enr_alt1;
	CString m_enr_alt2;
	double m_width_left;
	double m_width_right;
	double m_width_end_left;
	double m_width_end_right;
	double m_turn_radius;
	char m_turn_direction;
	double m_prev_width_left;
	double m_prev_width_right;
	d_geo_t m_geo;
	d_geo_t m_center;
	d_geo_t m_left_center;
	d_geo_t m_right_center;
	d_geo_t m_prev_pt;
	d_geo_t m_next_pt;
	d_geo_t m_left_rollin;
	d_geo_t m_left_rollout;
	d_geo_t m_right_rollin;
	d_geo_t m_right_rollout;
	d_geo_t m_rollin;
	d_geo_t m_rollout;
	d_geo_t m_prev_left_rollout;
	d_geo_t m_prev_right_rollout;
	d_geo_t m_prev_rollout;
	BOOL m_right_turn;
	BOOL m_end_point;
	BOOL m_start_point;
	BOOL m_left_intersect;
	BOOL m_right_intersect;
	BOOL m_single_vertex;
	int m_count;
	double m_delta;
	double m_radius;
	double m_left_radius;
	double m_right_radius;
	CString m_usage;
	int m_lx, m_ly, m_rx, m_ry;
	int m_lax, m_lay, m_lbx, m_lby;
	int m_rax, m_ray, m_rbx, m_rby;
	CString m_label;
	d_geo_t 	m_to_l, m_to_r, m_from_l, m_from_r, m_prev_l, m_prev_r;

int compute_arc(MapProj* map,
                double radius_nm, // in NM
					char turn_dir,	 // L, R, or something else
					CList <C_dafroute_seg*, C_dafroute_seg*> & left_seglist,
					CList <C_dafroute_seg*, C_dafroute_seg*> & center_seglist,
					CList <C_dafroute_seg*, C_dafroute_seg*> & right_seglist);

int compute_arc_no_radius(MapProj* map,
   CList <C_dafroute_seg*, C_dafroute_seg*> & left_seglist,
	CList <C_dafroute_seg*, C_dafroute_seg*> & center_seglist,
	CList <C_dafroute_seg*, C_dafroute_seg*> & right_seglist);

void draw_arc(ActiveMap* map,
   CList <C_dafroute_seg*, C_dafroute_seg*> & left_seglist,
	CList <C_dafroute_seg*, C_dafroute_seg*> & center_seglist,
	CList <C_dafroute_seg*, C_dafroute_seg*> & right_seglist);

//	int compute_arc(double radius_nm, // in NM
//						CList <C_dafroute_seg*, C_dafroute_seg*> & left_seglist,
//						CList <C_dafroute_seg*, C_dafroute_seg*> & right_seglist);
	void draw_arrow(ActiveMap* map, CDC* dc, int width, int color, int backcolor);
	int compute_lag_turn(double radius_nm, // in NM
								d_geo_t prev_pt, d_geo_t cur_pt, d_geo_t next_pt,
								CList <C_dafroute_seg*, C_dafroute_seg*> & seglist);

	int compute_lead_turn(double radius_nm, // in NM
								d_geo_t prev_pt, d_geo_t cur_pt, d_geo_t next_pt,
								CList <C_dafroute_seg*, C_dafroute_seg*> & seglist);
	int display_altitude(CDC *dc);
	int display_label(CDC *dc);
};

// ********************************************************************


#endif // ifndef DAFROUTE_H
