// Copyright (c) 1994-2009,2013 Georgia Tech Research Corporation, Atlanta, GA
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

// ar_route.cpp

#include "stdafx.h"
#include "param.h"
#include "ar_edit.h"
#include "ovlutil.h"
#include "ar_util.h"
#include "ovl_mgr.h"
#include "FvwGraphicDrawingSize.h"

// *****************************************************************
// *****************************************************************

C_ar_obj::C_ar_obj(C_overlay *overlay) :
   C_icon(overlay) 
{
	CString sdata;

	sdata = PRM_get_registry_string("ArEdit", "AnchorWidth", "20");
	m_anchor_width = atoi(sdata);
	sdata = PRM_get_registry_string("ArEdit", "AnchorLength", "40");
	m_anchor_length = atoi(sdata);
	
	m_rect.SetRect(0,0,0,0);
	m_selected_vertex = NULL;
	m_start_vertex = NULL;
	m_selected = FALSE;
	m_created = FALSE;
	m_color = UTIL_COLOR_RED;
	m_id = "Untitled";
//	m_anchor_length = 1.0;
//	m_anchor_width = 20.0;
	m_lat = 0.0;
	m_lon = 0.0;
	m_angle = 0.0;
	m_anchor_left = FALSE;
	m_ip_dist = 20.0;
	m_exit_dist = 20.0;
	m_remark_ptr = 0;
	m_remark_cnt = 0;
	m_type = ' ';
	m_ll_lat = 0.0;
	m_ll_lon = 0.0;
	m_ur_lat = 0.0;
	m_ur_lon = 0.0;
	m_anchor_pt_lat = 0.0;
	m_anchor_pt_lon = 0.0;
	m_icao = " ";
	m_direction = " ";
	m_visible = TRUE;
	m_country = "US";
	m_anchor_point = new C_ar_point[4];

	m_first_draw = TRUE;

	int k;
	for (k=0; k<4; k++)
		m_anchor_point[k].m_obj = this;
}

// *****************************************************************
// *****************************************************************

C_ar_obj::~C_ar_obj()
{
	clear_points();
	delete [] m_anchor_point;
}

// *****************************************************************
// *****************************************************************

void C_ar_obj::invalidate()
{
	OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}
// end of invalidate

// *****************************************************************
// *****************************************************************

BOOL C_ar_obj::hit_test(CPoint point, C_ar_point **vertex)
{
	POSITION prev;
	C_ar_point *vtx;
	BOOL found;

	if (m_type == 'A')
	{
		int k;

		for (k=0; k<4; k++)
		{
			if (m_anchor_point[k].hit_test(point))
			{
				*vertex = &(m_anchor_point[k]);
				return TRUE;
			}
		}
	}

	found = FALSE;
	*vertex = NULL;
	if (!m_point_list.IsEmpty())
	{
		prev = m_point_list.GetTailPosition();
		do
		{
			// get this item and the position of the next one
			vtx = m_point_list.GetPrev(prev);
	   		if (vtx->hit_test(point))
			{
				*vertex = vtx;
				return TRUE;
			}
		} while (prev != NULL);
	}
	return FALSE;
}
// end of hit_test

// *****************************************************************
// *****************************************************************

BOOL C_ar_obj::hit_test(CPoint point)
{
	POSITION prev;
	C_ar_point *vtx;
	BOOL found;
	CRect rc;
	int k;

	found = FALSE;
	if (!m_point_list.IsEmpty())
	{
		prev = m_point_list.GetTailPosition();
		do
		{
			// get this item and the position of the next one
			vtx = m_point_list.GetPrev(prev);
	   		if (vtx->hit_test(point))
			{
				return TRUE;
			}
		} while (prev != NULL);
	}

	for (k=0; k<4; k++)
	{
		rc.SetRect(point.x - 8, point.y - 8, point.x + 8, point.y + 8);
		if (rc.PtInRect(point))
			found = TRUE;
	}

	return found;
}
// end of hit_test

// *****************************************************************
// *************************************************************

BOOL C_ar_obj::hit_test(MapProj* map, CPoint testpt, int* index, BOOL *anchor)
{
	POSITION next;
	C_ar_point* pt;
	C_ar_edit *ovl;
   COvlkitUtil util;
	CFvwUtil *futil = CFvwUtil::get_instance();
	BOOL notdone, found, first, last, first_time;
	int cnt;
	int tx, ty, k;
	int fx, fy;
	degrees_t tlat, tlon, flat, flon;
	CRect rc;

	*anchor = FALSE;

	// kludge until I find the real problem
	if (fabs(m_ll_lat) > 90.0)
		return FALSE;

	ovl = (C_ar_edit*) m_overlay;
	if (!C_ar_edit::valid_overlay(ovl))
	{
		ASSERT(0);
		return FALSE;
	}

	*index = -1;

	// check anchor points
   int pt_size = ICON_ELEMENT_DRAWING_SIZE( 8, FALSE );
	for (k=0; k<4; k++)
	{
		rc.SetRect(m_anchor_pt[k].x - pt_size, m_anchor_pt[k].y - pt_size,
         m_anchor_pt[k].x + pt_size, m_anchor_pt[k].y + pt_size);
		if (rc.PtInRect(testpt))
		{
			*index = k+1;
			*anchor = TRUE;
			return TRUE;
		}
	}

//	if ( m_point_list.IsEmpty() )
//		return FALSE;

	next = m_point_list.GetHeadPosition();
	first = TRUE;
	last = FALSE;
	notdone = TRUE;
	found = FALSE;
	cnt = 1;

	// check for a vertex hit
	while (next != NULL)
	{
		pt = m_point_list.GetNext(next);
		if (pt->m_rect.PtInRect(testpt))
		{
			found = TRUE;
			next = NULL;
			*index = cnt;
			ovl->m_cur_vertex = pt;
			continue;
		}
		cnt++;
	}
	if (found)
		return TRUE;

	if (map == NULL)
		return FALSE;

	if (m_type == 'T')
	{
		if (ovl->m_display_sof_points)
			found = sof_line_hit_test(testpt);
		else
		{
			next = m_point_list.GetHeadPosition();
			first = TRUE;
			last = FALSE;
			notdone = TRUE;
			found = FALSE;
			first_time = TRUE;
			tx = 0;
			ty = 0;
			cnt = 1;

			// check for a line hit
			while (next != NULL)
			{
				pt = m_point_list.GetNext(next);
				if (first_time)
				{
					first_time = FALSE;
					fx = pt->m_pt.x;
					fy = pt->m_pt.y;
					tlat = pt->m_lat;
					tlon = pt->m_lon;
					flat = pt->m_lat;
					flon = pt->m_lon;
				}
				else
				{
					if (util.geo_line_hit_test(map, pt->m_lat, pt->m_lon, tlat, tlon,
                  LINE_ELEMENT_LINE_WIDTH( 2, FALSE ), UTIL_LINE_TYPE_GREAT, testpt))
					{
						found = TRUE;
						next = NULL;
						*index = -1;
						continue;
					}
					tlat = pt->m_lat;
					tlon = pt->m_lon;
				}
				tx = pt->m_pt.x;
				ty = pt->m_pt.y;
				cnt++;
			}
		}
	}

	if (m_type == 'A')
		if (racetrack_hit(map, testpt))
			found = TRUE;
		
	if (found)
		return TRUE;

	return found;
}
// end of hit_test

// *****************************************************************
// *****************************************************************

BOOL C_ar_obj::racetrack_hit(MapProj* map, CPoint point)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	int dist, tx, ty;
	CPoint center_pt[2];
	POINT box[4];
	BOOL hit;
	int mag;
	double angle;
   int eff_line_width = LINE_ELEMENT_LINE_WIDTH( 5, FALSE );

	if (m_type != 'A')
		return FALSE;

	dist = futil->distance_to_line (	m_anchor_pt[0].x, m_anchor_pt[0].y, 
									m_anchor_pt[1].x, m_anchor_pt[1].y,
									point.x, point.y);
	if (dist < eff_line_width)
		return TRUE;

	dist = futil->distance_to_line (	m_anchor_pt[2].x, m_anchor_pt[2].y, 
									m_anchor_pt[3].x, m_anchor_pt[3].y,
									point.x, point.y);
	if (dist < eff_line_width)
		return TRUE;

	// check area of arcs
	double ang;
	ang = m_angle + 180.0;
	if (ang > 360.0)
		ang -= 360.0;
	box[0].x = m_anchor_pt[0].x;
	box[0].y = m_anchor_pt[0].y;
	box[3].x = m_anchor_pt[3].x;
	box[3].y = m_anchor_pt[3].y;
	futil->point_to_vector(m_anchor_pt[1].x, m_anchor_pt[1].y, m_anchor_pt[0].x, m_anchor_pt[0].y, &mag, &angle);
	dist = futil->magnitude(box[0].x, box[0].y, box[3].x, box[3].y);
	futil->vector_to_point(box[0].x, box[0].y, (dist / 2) + eff_line_width, angle, &tx, &ty);
	box[1].x = tx;
	box[1].y = ty;
	futil->vector_to_point(box[3].x, box[3].y, (dist / 2) + eff_line_width, angle, &tx, &ty);
	box[2].x = tx;
	box[2].y = ty;

	if (!futil->point_in_polygon(point, box, 4))
	{
		futil->point_to_vector(m_anchor_pt[0].x, m_anchor_pt[0].y, m_anchor_pt[1].x, m_anchor_pt[1].y, &mag, &angle);
		box[0].x = m_anchor_pt[1].x;
		box[0].y = m_anchor_pt[1].y;
		box[3].x = m_anchor_pt[2].x;
		box[3].y = m_anchor_pt[2].y;
		futil->vector_to_point(box[0].x, box[0].y, (dist / 2) + eff_line_width, angle, &tx, &ty);
		box[1].x = tx;
		box[1].y = ty;
		futil->vector_to_point(box[3].x, box[3].y, (dist / 2) + eff_line_width, angle, &tx, &ty);
		box[2].x = tx;
		box[2].y = ty;
		if (!futil->point_in_polygon(point, box, 4))
			return FALSE;
	}

	center_pt[0].x = (m_anchor_pt[0].x + m_anchor_pt[3].x) / 2;
	center_pt[0].y = (m_anchor_pt[0].y + m_anchor_pt[3].y) / 2;
	center_pt[1].x = (m_anchor_pt[1].x + m_anchor_pt[2].x) / 2;
	center_pt[1].y = (m_anchor_pt[1].y + m_anchor_pt[2].y) / 2;
	
	hit = point_on_arc(m_anchor_pt[3], m_anchor_pt[0], center_pt[0], FALSE, point);
	if (hit)
		return TRUE;

	hit = point_on_arc(m_anchor_pt[1], m_anchor_pt[2], center_pt[1], FALSE, point);

	return hit;
}
// end of racetrack_hit

// *****************************************************************
// *****************************************************************

BOOL C_ar_obj::list_hit_test(CPoint point)
{
	POSITION prev;
	C_ar_point *vtx, *new_vtx;
	BOOL found;
	C_ar_edit *ovl;

	ovl = (C_ar_edit*) m_overlay;

	found = FALSE;
	ovl->clear_vertex_hit_list();
	ovl->m_fuel_point_selected_pt.x = -1;
	ovl->m_fuel_point_selected_pt.y = -1;

	if (!m_point_list.IsEmpty())
	{
		prev = m_point_list.GetTailPosition();
		do
		{
			// get this item and the position of the next one
			vtx = m_point_list.GetPrev(prev);
		 	if (vtx->hit_test(point))
			{
				CString tstr;

				new_vtx = new C_ar_point(this);
				*new_vtx = *vtx;
				if (ovl->m_fuel_point_selected)
				{
					new_vtx->m_selected = TRUE;
//					m_selected = TRUE;
					C_ar_edit::normalize_id(vtx->m_id, vtx->m_direction, ovl->m_fuel_select_id);
					ovl->m_fuel_point_selected_lat = vtx->m_lat;
					ovl->m_fuel_point_selected_lon = vtx->m_lon;
					vtx->has_one_usage(tstr);
					ovl->m_fuel_point_selected_usage = tstr;
					C_ar_edit *ovl = (C_ar_edit*) m_overlay;
					ovl->m_point_selected = TRUE;
				}
				ovl->m_refuel_vertex_hit_list.AddTail(new_vtx);
			   	found = TRUE;
			}
		} while (prev != NULL);
	}

	if (!found)
	{
			// check the anchor points
		if (m_type == 'A')
		{
			int k;

			for (k=0; k<4; k++)
			{
				if (m_anchor_point[k].hit_test(point))
				{
					CString tstr;

					new_vtx = new C_ar_point(this);
					*new_vtx = m_anchor_point[k];
					if (ovl->m_fuel_point_selected)
					{
						new_vtx->m_selected = TRUE;
						C_ar_edit::normalize_id(new_vtx->m_id, new_vtx->m_direction, ovl->m_fuel_select_id);
						ovl->m_fuel_point_selected_lat = new_vtx->m_lat;
						ovl->m_fuel_point_selected_lon = new_vtx->m_lon;
						new_vtx->has_one_usage(tstr);
						ovl->m_fuel_point_selected_usage = tstr;
						C_ar_edit *ovl = (C_ar_edit*) m_overlay;
						ovl->m_point_selected = TRUE;
					}
					ovl->m_refuel_vertex_hit_list.AddTail(new_vtx);
			   		found = TRUE;
				}
			}
		}
	}
	return found;
}
// end of list_hit_test

// *****************************************************************
// *****************************************************************

void C_ar_obj::delete_selected()
{
	POSITION next, old;
	C_ar_point *pt;

	next = m_point_list.GetHeadPosition();
	while (next != NULL)
	{
		old = next;
		pt = m_point_list.GetNext(next);
		if (pt->m_selected)
		{
			m_point_list.RemoveAt(old);
			delete pt;
		}
	}
}
// end delete_selected

// *****************************************************************
// *****************************************************************

void C_ar_obj::draw_xor_anchor(CDC *dc, int x, int y)
{
	CPen pen, *oldpen;
	int oldmode;
	CPoint pt[4];

	pen.CreatePen(PS_SOLID, 2, RGB(255,255,255));
	oldpen = (CPen*) dc->SelectObject(&pen);
	oldmode = dc->SetROP2(R2_XORPEN);
   int size = ICON_ELEMENT_DRAWING_SIZE( 8, FALSE );
	pt[0].x = x;
	pt[0].y = y-size;
	pt[1].x = x+size;
	pt[1].y = y;
	pt[2].x = x;
	pt[2].y = y + size;
	pt[3].x = x - size;
	pt[3].y = y;
	
	dc->MoveTo(pt[0]);
	dc->LineTo(pt[1]);
	dc->LineTo(pt[2]);
	dc->LineTo(pt[3]);
	dc->LineTo(pt[0]);

	dc->SelectObject(oldpen);
	dc->SetROP2(oldmode);
	pen.DeleteObject();
}

// *****************************************************************
// *****************************************************************

void C_ar_obj::draw_arc(CDC *dc, POINT start, POINT end, POINT center,
									 BOOL clockwise, BOOL xor)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	double ang, anginc, dist;
	double ang1, ang2;
	int tx, ty, cnt;
	double rang1, rang2;
	int oldrop;

	dist = (double) futil->magnitude(start.x, start.y, center.x, center.y);
	ang1 = futil->line_angle(center.x, center.y, start.x, start.y);
	ang2 = futil->line_angle(center.x, center.y, end.x, end.y);
	rang1 = DEG_TO_RAD(ang1);
	rang2 = DEG_TO_RAD(ang2);
	anginc = DEG_TO_RAD(6.0);

	if (xor)
	{
		oldrop = dc->SetROP2(R2_XORPEN); 
	}


	dc->MoveTo(start);
	cnt = 0;
	if (clockwise)
	{
		ang = rang1 + anginc;
		while ((fabs(ang - rang2) > (anginc + 0.01)) && (cnt < 60))
		{
			tx = center.x + (int) (cos(ang) * dist);
			ty = center.y + (int) (sin(ang) * dist);
			dc->LineTo(tx, ty);
			ang += anginc;
			if (ang >= TWO_PI)
				ang -= TWO_PI;
			cnt++;
		}
	}
	else
	{
		ang = rang1 - anginc;
		while ((fabs(ang - rang2) > (anginc + 0.01)) && (cnt < 60))
		{
			tx = center.x + (int) (cos(ang) * dist);
			ty = center.y + (int) (sin(ang) * dist);
			dc->LineTo(tx, ty);
			ang -= anginc;
			if (ang < 0.0)
				ang += TWO_PI;
			cnt++;
		}
	}
	dc->LineTo(end);
	if (xor)
		dc->SetROP2(oldrop); 
}
// end of draw_arc

// *****************************************************************
// *****************************************************************

BOOL C_ar_obj::point_on_arc(CPoint start, CPoint end, CPoint center,
									 BOOL clockwise, CPoint test_pt)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	double ang, anginc, dist;
	double ang1, ang2;
	int tx, ty, cnt, tdist;
	double rang1, rang2;

	dist = (double) futil->magnitude(start.x, start.y, center.x, center.y);
	ang1 = futil->line_angle(center.x, center.y, start.x, start.y);
	ang2 = futil->line_angle(center.x, center.y, end.x, end.y);
	rang1 = DEG_TO_RAD(ang1);
	rang2 = DEG_TO_RAD(ang2);
	anginc = DEG_TO_RAD(1.0);

	cnt = 0;
   int eff_line_width = LINE_ELEMENT_LINE_WIDTH( 5, FALSE );

	if (clockwise)
	{
		ang = rang1 + anginc;
		while ((fabs(ang - rang2) > (anginc + 0.01)) && (cnt < 360))
		{
			tx = center.x + (int) (cos(ang) * dist);
			ty = center.y + (int) (sin(ang) * dist);
			tdist = futil->magnitude(test_pt.x, test_pt.y, tx, ty);
			if (tdist < eff_line_width)
				return TRUE;
			ang += anginc;
			if (ang >= TWO_PI)
				ang -= TWO_PI;
			cnt++;
		}
	}
	else
	{
		ang = rang1 - anginc;
		while ((fabs(ang - rang2) > (anginc + 0.01)) && (cnt < 360))
		{
			tx = center.x + (int) (cos(ang) * dist);
			ty = center.y + (int) (sin(ang) * dist);
			tdist = futil->magnitude(test_pt.x, test_pt.y, tx, ty);
			if (tdist < eff_line_width)
				return TRUE;
			ang -= anginc;
			if (ang < 0.0)
				ang += TWO_PI;
			cnt++;
		}
	}
	return FALSE;
}
// end of point_on_arc

// *****************************************************************
// *****************************************************************

int C_ar_obj::draw_racetrack(MapProj* map, CDC *dc, int color_code, BOOL background, BOOL xor)
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	POINT pt[4];
	double lat[4], lon[4];
	POINT ctr;
	int k, tx, ty;
	int dist1, dist2;
	double angle, ang1, ang2;
	double distance;
	CPen redpen, blackpen, *oldpen, yellowpen;
	BOOL clockwise;
	CString name;
	C_ar_edit *ovl;
	int style;

	if (m_first_draw)
		make_anchor(map);

	m_first_draw = FALSE;

	if (xor)
		style = UTIL_LINE_XOR;
	else
		style = PS_SOLID;

	ovl = (C_ar_edit*) m_overlay;

	for (k=0; k< 4; k++)
	{
		lat[k] = m_anchor_point[k].m_lat;
		lon[k] = m_anchor_point[k].m_lon;
		map->geo_to_surface(lat[k], lon[k], &tx, &ty);
		pt[k].x = tx;
		pt[k].y = ty;
		m_anchor_pt[k].x = tx;
		m_anchor_pt[k].y = ty;
	}

   const BOOL isPrinting = dc->IsPrinting();
   int
      red_line_width = LINE_ELEMENT_LINE_WIDTH( 2, isPrinting ),
      black_line_width = LINE_ELEMENT_LINE_WIDTH( 2 + 2, isPrinting ),
      yellow_line_width = LINE_ELEMENT_LINE_WIDTH( 2 + 4, isPrinting );

	redpen.CreatePen(PS_SOLID, red_line_width, futil->code2color(color_code));
	blackpen.CreatePen(PS_SOLID, black_line_width, futil->code2color(UTIL_COLOR_BLACK));
	yellowpen.CreatePen(PS_SOLID, yellow_line_width, futil->code2color(UTIL_COLOR_YELLOW));
	oldpen = (CPen*) dc->SelectObject(&blackpen);

//	dist1 = futil->magnitude(pt[0].x, pt[0].y, pt[1].x, pt[1].y);	
//	dist2 = futil->magnitude(pt[1].x, pt[1].y, pt[2].x, pt[2].y);	
	GEO_geo_to_distance(lat[0], lon[0], lat[1], lon[1], &distance, &angle);
	dist1 = (int) distance;
	GEO_geo_to_distance(lat[1], lon[1], lat[2], lon[2], &distance, &angle);
	dist2 = (int) distance;

dist1 = dist2 + 10;

//	if (m_selected && !ovl->m_bEdit && !xor)
	if (m_selected && !xor)
	{
		if (dist1 > dist2)
		{
			futil->draw_geo_line(map, dc, lat[0], lon[0], lat[1], lon[1],
				UTIL_COLOR_YELLOW, PS_SOLID, yellow_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
			futil->draw_geo_line(map,  dc, lat[2], lon[2], lat[3], lon[3],
				UTIL_COLOR_YELLOW, PS_SOLID, yellow_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
			ang1 = futil->line_angle(pt[0].x, pt[0].y, pt[1].x, pt[1].y);
			ang2 = futil->line_angle(pt[1].x, pt[1].y, pt[2].x, pt[2].y);
			angle = ang2 - ang1;
			if (angle < 0.0)
				angle += 360.0;
			if (angle < 180.0)
				clockwise = TRUE;
			else
				clockwise = FALSE;
			dc->SelectObject(&yellowpen);
			ctr.x = (pt[1].x + pt[2].x) / 2;
			ctr.y = (pt[1].y + pt[2].y) / 2;
			draw_arc(dc, pt[1], pt[2], ctr, clockwise);
			ctr.x = (pt[3].x + pt[0].x) / 2;
			ctr.y = (pt[3].y + pt[0].y) / 2;
			draw_arc(dc, pt[3], pt[0], ctr, clockwise);
			dc->SelectObject(&blackpen);
		}
		else
		{
			futil->draw_geo_line(map,  dc, lat[1], lon[1], lat[2], lon[2],
				UTIL_COLOR_YELLOW, PS_SOLID, yellow_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
			futil->draw_geo_line(map,  dc, lat[3], lon[3], lat[0], lon[0],
				UTIL_COLOR_YELLOW, PS_SOLID, yellow_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
			ang1 = futil->line_angle(pt[3].x, pt[3].y, pt[0].x, pt[0].y);
			ang2 = futil->line_angle(pt[0].x, pt[0].y, pt[1].x, pt[1].y);
			angle = ang2 - ang1;
			if (angle < 0.0)
				angle += 360.0;
			if (angle < 180.0)
				clockwise = TRUE;
			else
				clockwise = FALSE;
			dc->SelectObject(&yellowpen);
			ctr.x = (pt[0].x + pt[1].x) / 2;
			ctr.y = (pt[0].y + pt[1].y) / 2;
			draw_arc(dc, pt[0], pt[1], ctr, clockwise);
			ctr.x = (pt[2].x + pt[3].x) / 2;
			ctr.y = (pt[2].y + pt[3].y) / 2;
			draw_arc(dc, pt[2], pt[3], ctr, clockwise);
			dc->SelectObject(&blackpen);
		}
	}

	if (background && !xor)
	{
		if (dist1 > dist2)
		{
			futil->draw_geo_line(map, dc, lat[0], lon[0], lat[1], lon[1],
				UTIL_COLOR_BLACK, PS_SOLID, black_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
			futil->draw_geo_line(map,  dc, lat[2], lon[2], lat[3], lon[3],
				UTIL_COLOR_BLACK, PS_SOLID, black_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
			ang1 = futil->line_angle(pt[0].x, pt[0].y, pt[1].x, pt[1].y);
			ang2 = futil->line_angle(pt[1].x, pt[1].y, pt[2].x, pt[2].y);
			angle = ang2 - ang1;
			if (angle < 0.0)
				angle += 360.0;
			if (angle < 180.0)
				clockwise = TRUE;
			else
				clockwise = FALSE;
			ctr.x = (pt[1].x + pt[2].x) / 2;
			ctr.y = (pt[1].y + pt[2].y) / 2;
			draw_arc(dc, pt[1], pt[2], ctr, clockwise);
			ctr.x = (pt[3].x + pt[0].x) / 2;
			ctr.y = (pt[3].y + pt[0].y) / 2;
			draw_arc(dc, pt[3], pt[0], ctr, clockwise);
		}
		else
		{
			futil->draw_geo_line(map,  dc, lat[1], lon[1], lat[2], lon[2],
				UTIL_COLOR_BLACK, PS_SOLID, red_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
			futil->draw_geo_line(map,  dc, lat[3], lon[3], lat[0], lon[0],
				UTIL_COLOR_BLACK, PS_SOLID, red_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
			ang1 = futil->line_angle(pt[3].x, pt[3].y, pt[0].x, pt[0].y);
			ang2 = futil->line_angle(pt[0].x, pt[0].y, pt[1].x, pt[1].y);
			angle = ang2 - ang1;
			if (angle < 0.0)
				angle += 360.0;
			if (angle < 180.0)
				clockwise = TRUE;
			else
				clockwise = FALSE;
			ctr.x = (pt[0].x + pt[1].x) / 2;
			ctr.y = (pt[0].y + pt[1].y) / 2;
			draw_arc(dc, pt[0], pt[1], ctr, clockwise);
			ctr.x = (pt[2].x + pt[3].x) / 2;
			ctr.y = (pt[2].y + pt[3].y) / 2;
			draw_arc(dc, pt[2], pt[3], ctr, clockwise);
		}
	}

	dc->SelectObject(&redpen);
	if (dist1 > dist2)
	{
		futil->draw_geo_line(map,  dc, lat[0], lon[0], lat[1], lon[1],
			color_code, style, red_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		futil->draw_geo_line(map,  dc, lat[2], lon[2], lat[3], lon[3],
			color_code, style, red_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		ang1 = futil->line_angle(pt[0].x, pt[0].y, pt[1].x, pt[1].y);
		ang2 = futil->line_angle(pt[1].x, pt[1].y, pt[2].x, pt[2].y);
		angle = ang2 - ang1;
		if (angle < 0.0)
			angle += 360.0;
		if (angle < 180.0)
			clockwise = TRUE;
		else
			clockwise = FALSE;
		ctr.x = (pt[1].x + pt[2].x) / 2;
		ctr.y = (pt[1].y + pt[2].y) / 2;
		draw_arc(dc, pt[1], pt[2], ctr, clockwise, xor);
		ctr.x = (pt[3].x + pt[0].x) / 2;
		ctr.y = (pt[3].y + pt[0].y) / 2;
		draw_arc(dc, pt[3], pt[0], ctr, clockwise, xor);

	}
	else
	{
		futil->draw_geo_line(map,  dc, lat[1], lon[1], lat[2], lon[2],
			color_code, style, red_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		futil->draw_geo_line(map,  dc, lat[3], lon[3], lat[0], lon[0],
			color_code, style, red_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		ang1 = futil->line_angle(pt[3].x, pt[3].y, pt[0].x, pt[0].y);
		ang2 = futil->line_angle(pt[0].x, pt[0].y, pt[1].x, pt[1].y);
		angle = ang2 - ang1;
		if (angle < 0.0)
			angle += 360.0;
		if (angle < 180.0)
			clockwise = TRUE;
		else
			clockwise = FALSE;
		ctr.x = (pt[0].x + pt[1].x) / 2;
		ctr.y = (pt[0].y + pt[1].y) / 2;
		draw_arc(dc, pt[0], pt[1], ctr, clockwise, xor);
		ctr.x = (pt[2].x + pt[3].x) / 2;
		ctr.y = (pt[2].y + pt[3].y) / 2;
		draw_arc(dc, pt[2], pt[3], ctr, clockwise, xor);
	}
	dc->SelectObject(oldpen);
	redpen.DeleteObject();

	// calculate the object rectangle
	int minx, maxx, miny, maxy;
	minx = maxx = pt[0].x;
	miny = maxy = pt[0].y;
	for (k=1; k<4; k++)
	{
		if (pt[k].x < minx)
			minx = pt[k].x;
		if (pt[k].x > maxx)
			maxx = pt[k].x;
		if (pt[k].y < miny)
			miny = pt[k].y;
		if (pt[k].y > maxy)
			maxy = pt[k].y;
	}

	// expand for arcs
	minx -= abs(minx - maxx) / 2;
	maxx += abs(minx - maxx) / 2;
	miny -= abs(miny - maxy) / 2;
	maxy += abs(miny - maxy) / 2;

	m_rect.SetRect(minx, miny, maxx, maxy);

	return SUCCESS;
}
//	end of draw_racetrack

// *****************************************************************
// *****************************************************************

int C_ar_obj::draw_start_end_lines(MapProj* map, CDC *dc)
{
	POSITION next;
	C_ar_point *vtx;
	int k, tx, ty;
	int dist, tdist, distnum;
	CFvwUtil *futil = CFvwUtil::get_instance();
	POINT spt, ept;
	BOOL hit, unique;

	spt.x = 0;
	spt.y = 0;
	ept.x = 0;
	ept.y = 0;

	next = m_point_list.GetHeadPosition();
	while (next != NULL)
	{
		vtx = m_point_list.GetNext(next);
		tx = vtx->m_pt.x;
		ty = vtx->m_pt.y;
		if (!vtx->has_usage_string("IP", &unique))
		{
			// check to see is it's conincident with an anchor pattern
			hit = FALSE;
			for (k=0; k<4; k++)
				if ((tx == m_anchor_pt[k].x) && (ty == m_anchor_pt[k].y))
					hit = TRUE;
			if (!hit)
			{
				spt.x = tx;
				spt.y = ty;
			}
		}

		if (!vtx->has_usage_string("ET", &unique))
		{
			// check to see is it's conincident with an anchor pattern
			hit = FALSE;
			for (k=0; k<4; k++)
				if ((tx == m_anchor_pt[k].x) && (ty == m_anchor_pt[k].y))
					hit = TRUE;
			if (!hit)
			{
				spt.x = tx;
				spt.y = ty;
			}
		}

		if (!vtx->has_usage_string("EX", &unique))
		{
			// check to see is it's conincident with an anchor pattern
			hit = FALSE;
			for (k=0; k<4; k++)
				if ((tx == m_anchor_pt[k].x) && (ty == m_anchor_pt[k].y))
					hit = TRUE;
			if (!hit)
			{
				ept.x = tx;
				ept.y = ty;
			}
		}
	}

   UtilDraw utilDraw(dc);
   const BOOL isPrinting = dc->IsPrinting();
   utilDraw.set_pen(UTIL_COLOR_BLACK, LINE_ELEMENT_LINE_WIDTH(UTIL_LINE_DOT, isPrinting ), 1);

	// draw the start connecting line
	if ((spt.x != 0) && (spt.y != 0))
	{
		// find the closest anchor pattern
		dist = futil->magnitude(spt.x, spt.y, m_anchor_pt[0].x, m_anchor_pt[0].y);
		distnum = 0;
		for (k=1; k<4; k++)
		{
			tdist = futil->magnitude(spt.x, spt.y, m_anchor_pt[k].x, m_anchor_pt[k].y);
			if (tdist < dist)
			{
				dist = tdist;
				distnum = k;
			}
		}
		utilDraw.draw_line(map, spt.x, spt.y, m_anchor_pt[distnum].x, m_anchor_pt[distnum].y);
	}

	// draw the end connecting line
	if ((ept.x != 0) && (ept.y != 0))
	{
		// find the closest anchor pattern
		dist = futil->magnitude(ept.x, ept.y, m_anchor_pt[0].x, m_anchor_pt[0].y);
		distnum = 0;
		for (k=1; k<4; k++)
		{
			tdist = futil->magnitude(ept.x, ept.y, m_anchor_pt[k].x, m_anchor_pt[k].y);
			if (tdist < dist)
			{
				dist = tdist;
				distnum = k;
			}
		}
		utilDraw.draw_line(map, ept.x, ept.y, m_anchor_pt[distnum].x, m_anchor_pt[distnum].y);
	}

	return SUCCESS;
}
// end of draw_start_end_lines

// *****************************************************************
// *****************************************************************

int C_ar_obj::compute_route(MapProj* map, char type,  // A, T, or B (anchor, track, both)
				CList <C_ar_point*, C_ar_point*> & vtxlist,
				CList <C_dafroute_leg*, C_dafroute_leg*> & leglist,
				CList <C_dafroute_leg*, C_dafroute_leg*> & ptlist)
{
	POSITION next;
	BOOL yet, unique;
	BOOL single_vertex;
	C_ar_point *vtx;
	C_dafroute_leg *leg, *pt;
	int cnt;
	int tx, ty;
	CFvwUtil *futil = CFvwUtil::get_instance();
	CString tstr;

	cnt = vtxlist.GetCount();
	if (cnt < 1)
		return FAILURE;

	if (cnt == 1)
		single_vertex = TRUE;
	else
		single_vertex = FALSE;

	next = vtxlist.GetHeadPosition();
	yet = TRUE;
	while (yet)
	{
		vtx = vtxlist.GetNext(next);
		if (next == NULL)
			yet = FALSE;

		vtx->has_one_usage(tstr);

		if ((type == 'T')	|| (vtx->has_usage_string("AP", &unique)))
		{
			leg = new C_dafroute_leg;
			leg->m_id = vtx->m_id;
			leg->m_geo.lat = vtx->m_lat;
			leg->m_geo.lon = vtx->m_lon;
			futil->geo_to_view_short(map, leg->m_geo.lat, leg->m_geo.lon, &tx, &ty);
			leg->m_lx = tx;
			leg->m_ly = ty;
			leg->m_usage = tstr;
			leglist.AddTail(leg);
		}
	 	pt = new C_dafroute_leg;
		pt->m_id = vtx->m_id;
		pt->m_geo.lat = vtx->m_lat;
		pt->m_geo.lon = vtx->m_lon;
		futil->geo_to_view_short(map, pt->m_geo.lat, pt->m_geo.lon, &tx, &ty);
		pt->m_lx = tx;
		pt->m_ly = ty;
		pt->m_usage = tstr;
		ptlist.AddTail(pt);

	}

	return SUCCESS;
}
// end of compute_route

// *****************************************************************
// *****************************************************************

void C_ar_obj::draw_point_handles(CDC *dc, int x, int y)
{
	CBrush brush, *oldbrush;
	CPen pen, *oldpen;
	int px[5], py[5];
	int rad = ICON_ELEMENT_DRAWING_SIZE( 10, FALSE );
	int k;
	CRect rc;

	brush.CreateSolidBrush(RGB(0,0,255));
	pen.CreatePen(PS_DOT, 1, RGB(255,255,255));
	px[0] = x - rad;
	py[0] = y - rad;
	px[1] = x + rad;
	py[1] = y - rad;
	px[2] = x + rad;
	py[2] = y + rad;
	px[3] = px[0];
	py[3] = py[2];
	px[4] = px[0];
	py[4] = py[0];

	oldpen = (CPen*) dc->SelectObject(&pen);
	oldbrush = (CBrush*) dc->SelectObject(&brush);
	for (k=0; k<4; k++)
	{
		dc->MoveTo(px[k], py[k]);
		dc->LineTo(px[k+1], py[k+1]);
	}
	for (k=0; k<4; k++)
	{
		rc.SetRect(px[k]-3, py[k]-3, px[k]+3, py[k]+3);
		dc->Rectangle(&rc);
	}

	dc->SelectObject(oldpen);
	dc->SelectObject(oldbrush);
	pen.DeleteObject();
	brush.DeleteObject();
}
// end of draw_point_handles

// *****************************************************************
// *****************************************************************

void C_ar_obj::draw_point_to_navaid_line(ActiveMap* map, CDC *dc, C_ar_point* pt, int color)
{
//	double tlat, tlon;
	double dist, ang;
	CFvwUtil *futil = CFvwUtil::get_instance();

	if (pt->m_fix.GetLength() < 6)
		return;

	dist = atof(pt->m_fix.Right(3));
	if (dist < 0.0001)
		return;

	ang = atof(pt->m_fix.Left(3));
	ang += 180.0;
	if (ang > 360.0)
		ang -= 360.0;
	ang -= pt->m_mag_var;

//	GEO_distance_to_geo(pt->m_lat, pt->m_lon, dist * 1852.0, ang, &tlat, &tlon);
//	futil->draw_geo_line(map, dc, pt->m_lat, pt->m_lon, tlat, tlon, futil->code2color(color), 
//						PS_DOT, 2, UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
	futil->draw_geo_line(map, dc, pt->m_lat, pt->m_lon, pt->m_nav_lat, pt->m_nav_lon, futil->code2color(color), 
						PS_DOT, LINE_ELEMENT_LINE_WIDTH( 2, dc->IsPrinting() ),
                  UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);

}
// end of draw_point_to_navaid_line

// *****************************************************************
// *****************************************************************

CIconImage*	C_ar_obj::get_icon(CString code)
{
	CIconImage*	draw_icon;
	C_ar_edit *ovl;

	ovl = (C_ar_edit*) m_overlay;

	if (!code.CompareNoCase("IP"))
		draw_icon = ovl->ip_icon;
	else if (!code.CompareNoCase("EX"))
		draw_icon = ovl->ex_icon;
	else if (!code.CompareNoCase("ET"))
		draw_icon = ovl->et_icon;
	else if (!code.CompareNoCase("AN"))
		draw_icon = ovl->an_icon;
	else if (!code.CompareNoCase("AP"))
		draw_icon = ovl->ap_icon;
	else if (!code.CompareNoCase("CP"))
		draw_icon = ovl->cp_icon;
	else
		draw_icon = ovl->nc_icon;

	return draw_icon;
}

// *****************************************************************
// *****************************************************************

void C_ar_obj::draw_sof_lines(ActiveMap* map, CDC *dc, BOOL selected)
{
	POSITION next;
	C_ar_point* pt;
	BOOL first = TRUE;
	CFvwUtil *futil = CFvwUtil::get_instance();
	int px, py, tx, ty, cnt, x1, y1, x2, y2;
	double angle, tang, lat1, lon1, lat2, lon2;
	int radius;

	if (m_type != 'T')
		return;

	cnt = m_point_list.GetCount();
	if (cnt < 2)
		return;

	C_ar_edit *ovl = (C_ar_edit*) m_overlay;
	radius = ovl->m_sof_display_radius;

	next = m_point_list.GetHeadPosition();
	pt = m_point_list.GetNext(next);
	tx = pt->m_pt.x;
	ty = pt->m_pt.y;

   const BOOL isPrinting = dc->IsPrinting();
   int
      line_width = LINE_ELEMENT_LINE_WIDTH( 2, isPrinting ),
      bkg_line_width = LINE_ELEMENT_LINE_WIDTH( 2 + 2, isPrinting ),
      sel_line_width = LINE_ELEMENT_LINE_WIDTH( 2 + 4, isPrinting );

	while (next != NULL)
	{
		pt = m_point_list.GetNext(next);
		px = pt->m_pt.x;
		py = pt->m_pt.y;
		angle = futil->line_angle(tx, ty, px, py);
		tang = angle - 90.0;
		futil->vector_to_point(tx, ty, radius, tang, &x1, &y1);
		futil->vector_to_point(px, py, radius, tang, &x2, &y2);
		map->surface_to_geo(x1, y1, &lat1, &lon1);
		map->surface_to_geo(x2, y2, &lat2, &lon2);

		if (selected)
		{
			futil->draw_geo_line(map,  dc, lat1, lon1, lat2, lon2, UTIL_COLOR_YELLOW, 
								PS_SOLID, sel_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		}
		if (m_background)
		{
			futil->draw_geo_line(map,  dc, lat1, lon1, lat2, lon2, UTIL_COLOR_BLACK, 
								PS_SOLID, bkg_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		}

		futil->draw_geo_line(map,  dc, lat1, lon1, lat2, lon2, m_color, 
							PS_SOLID, line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);

		tang = angle + 90.0;
		futil->vector_to_point(tx, ty, radius, tang, &x1, &y1);
		futil->vector_to_point(px, py, radius, tang, &x2, &y2);
		map->surface_to_geo(x1, y1, &lat1, &lon1);
		map->surface_to_geo(x2, y2, &lat2, &lon2);
		if (selected)
		{
			futil->draw_geo_line(map,  dc, lat1, lon1, lat2, lon2, UTIL_COLOR_YELLOW, 
								PS_SOLID, sel_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		}
		if (m_background)
		{
			futil->draw_geo_line(map,  dc, lat1, lon1, lat2, lon2, UTIL_COLOR_BLACK, 
								PS_SOLID, bkg_line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		}

		futil->draw_geo_line(map,  dc, lat1, lon1, lat2, lon2, m_color, 
							PS_SOLID, line_width, UTIL_LINE_TYPE_GREAT, TRUE, FALSE);
		tx = px;
		ty = py;
	}
}
// end of draw_sof_lines

// *****************************************************************
// *****************************************************************

BOOL C_ar_obj::sof_line_hit_test(CPoint point)
{
	POSITION next;
	C_ar_point* pt;
	BOOL first = TRUE;
	CFvwUtil *futil = CFvwUtil::get_instance();
	int px, py, tx, ty, cnt, x1, y1, x2, y2;
	double angle, tang;
	int radius;
	int dist;

	if (m_type != 'T')
		return FALSE;

	cnt = m_point_list.GetCount();
	if (cnt < 2)
		return FALSE;

	C_ar_edit *ovl = (C_ar_edit*) m_overlay;
	radius = ovl->m_sof_display_radius;

	next = m_point_list.GetHeadPosition();
	pt = m_point_list.GetNext(next);
	tx = pt->m_pt.x;
	ty = pt->m_pt.y;

   int eff_line_width = LINE_ELEMENT_LINE_WIDTH( 3, FALSE );
	while (next != NULL)
	{
		pt = m_point_list.GetNext(next);
		px = pt->m_pt.x;
		py = pt->m_pt.y;
		angle = futil->line_angle(tx, ty, px, py);
		tang = angle - 90.0;
		futil->vector_to_point(tx, ty, radius, tang, &x1, &y1);
		futil->vector_to_point(px, py, radius, tang, &x2, &y2);
		dist = futil->distance_to_line(x1, y1, x2, y2, point.x, point.y);
		if (dist < eff_line_width)
			return TRUE;
		tang = angle + 90.0;
		futil->vector_to_point(tx, ty, radius, tang, &x1, &y1);
		futil->vector_to_point(px, py, radius, tang, &x2, &y2);
		dist = futil->distance_to_line(x1, y1, x2, y2, point.x, point.y);
		if (dist < eff_line_width)
			return TRUE;
		tx = px;
		ty = py;
	}

	return FALSE;
}
// end of sof_line_hit_test

// *****************************************************************
// *****************************************************************

void C_ar_obj::draw_point(CDC *dc, int x, int y, CString usage, BOOL hilighted, BOOL sof)
{
	CIconImage*	draw_icon;
	C_ar_edit *ovl;
	CFvwUtil *futil = CFvwUtil::get_instance();
   const BOOL isPrinting = dc->IsPrinting();

	ovl = (C_ar_edit*) m_overlay;

	if (sof)
	{
		CBrush nullbrush, *oldbrush;
		CPen pen, blackpen, yellowpen, *oldpen;
		COLORREF rgb_color = futil->code2color(m_color);
		int radius = ovl->m_sof_display_radius;

		nullbrush.CreateStockObject(NULL_BRUSH);
		pen.CreatePen(PS_SOLID, LINE_ELEMENT_LINE_WIDTH( 2, isPrinting ), rgb_color);
		blackpen.CreatePen(PS_SOLID, LINE_ELEMENT_LINE_WIDTH( 4, isPrinting ), RGB(0, 0, 0));
		yellowpen.CreatePen(PS_SOLID, LINE_ELEMENT_LINE_WIDTH( 6, isPrinting ), RGB(255, 255, 0));
		oldbrush = dc->SelectObject(&nullbrush);
		oldpen = dc->SelectObject(&blackpen);
		if (hilighted)
		{
			dc->SelectObject(&yellowpen);
			dc->Ellipse(x-radius, y-radius, x+radius, y+radius);
			dc->SelectObject(&pen);
		}
		if (!usage.Compare("AN"))
			dc->Ellipse(x-radius-2, y-radius-2, x+radius+2, y+radius+2);

		if (m_background)
      {
         dc->SelectObject(&blackpen);
			dc->Ellipse(x-radius, y-radius, x+radius, y+radius);
      }
		dc->SelectObject(&pen);
		dc->Ellipse(x-radius, y-radius, x+radius, y+radius);
		dc->SelectObject(oldbrush);
		dc->SelectObject(oldpen);
		pen.DeleteObject();
		return;
	}

	if (hilighted)
		ovl->sel_icon->draw_icon(dc, x+1, y+1, 32, 32);
	if (!usage.Compare("IP"))
		draw_icon = ovl->ip_icon;
	else if (!usage.Compare("EX"))
		draw_icon = ovl->ex_icon;
	else if (!usage.Compare("ET"))
		draw_icon = ovl->et_icon;
	else if (!usage.Compare("AN"))
		draw_icon = ovl->an_icon;
	else if (!usage.Compare("AP"))
		draw_icon = ovl->ap_icon;
	else if (!usage.Compare("CP"))
		draw_icon = ovl->cp_icon;
	else
		draw_icon = ovl->nc_icon;

   int icon_adjusted_size = ICON_ELEMENT_DRAWING_SIZE( 32, isPrinting );

//	if (hilighted && !is_printing)
//		draw_icon->draw_hilighted(dc, x+1, y+1, 32);
//	else
		draw_icon->draw_icon(dc, x+1, y+1, 32, icon_adjusted_size);

//	if (hilighted && ovl->m_bEdit)
//		draw_point_handles(dc, x, y);
}
// end of draw_point

// *****************************************************************
// *****************************************************************

void C_ar_obj::draw_point(CDC *dc, C_ar_point *pt, BOOL hilighted, BOOL sof)
{
	BOOL unique;
	int cnt;
	cnt = pt->m_usage.GetCount();
	if (cnt < 1)
		return;

	if (pt->has_usage_string("IP", &unique))
		draw_point(dc, pt->m_pt.x, pt->m_pt.y, "IP", hilighted, sof);
	else if (pt->has_usage_string("EX", &unique))
		draw_point(dc, pt->m_pt.x, pt->m_pt.y, "EX", hilighted, sof);
	else if (pt->has_usage_string("ET", &unique))
		draw_point(dc, pt->m_pt.x, pt->m_pt.y, "ET", hilighted, sof);
	else if (pt->has_usage_string("AN", &unique))
		draw_point(dc, pt->m_pt.x, pt->m_pt.y, "AN", hilighted, sof);
	else if (pt->has_usage_string("AP", &unique))
		draw_point(dc, pt->m_pt.x, pt->m_pt.y, "AP", hilighted, sof);
	else if (pt->has_usage_string("CP", &unique))
		draw_point(dc, pt->m_pt.x, pt->m_pt.y, "CP", hilighted, sof);
	else
		draw_point(dc, pt->m_pt.x, pt->m_pt.y, "  ", hilighted, sof);
}
// end of draw_point

// *****************************************************************

int C_ar_obj::draw(ActiveMap* map)
{
	POSITION next;
	C_ar_point* pt;
	double tlat, tlon;
	BOOL first = TRUE;
	CFvwUtil *futil = CFvwUtil::get_instance();
	int tx, ty, k, minx, maxx, miny, maxy;
	CString value;
	BOOL is_sof_point;

	if (!m_visible)
		return SUCCESS;

	C_ar_edit *ovl = (C_ar_edit*) m_overlay;

	// get the point display options
		value = PRM_get_registry_string("ArEdit", "PointDisplayType", "NIMA");
	if (!value.Compare("NIMA"))
		ovl->m_display_sof_points = FALSE;
	else
		ovl->m_display_sof_points = TRUE;

	value = PRM_get_registry_string("ArEdit", "PointDisplayRadius", "20");
	ovl->m_sof_display_radius = atoi(value);
	if ((ovl->m_sof_display_radius < 0) || (ovl->m_sof_display_radius > 50))
		ovl->m_sof_display_radius = 20;

	CString sdata = PRM_get_registry_string("ArEdit", "ArEditBackground", "Y");
	if (sdata.Compare("Y"))
		m_background = FALSE;
	else
		m_background = TRUE;

	sdata = PRM_get_registry_string("ArEdit", "ArEditDispColor", "249");
	m_color = atoi(sdata);

	CDC* dc = map->get_CDC();

	m_dc = dc;
   const BOOL isPrinting = dc->IsPrinting();
   int
      point_size = ICON_ELEMENT_DRAWING_SIZE( 8, isPrinting ),
      yellow_width = LINE_ELEMENT_LINE_WIDTH( 6, isPrinting ),
      black_width = LINE_ELEMENT_LINE_WIDTH( 4, isPrinting ),
      line_width = LINE_ELEMENT_LINE_WIDTH( 2, isPrinting );

	if (m_type == 'T')
	{
		next = m_point_list.GetHeadPosition();
		while (next != NULL)
		{
			pt = m_point_list.GetNext(next);
			if (!first && !ovl->m_display_sof_points)
			{
				if (m_selected)
					futil->draw_geo_line(map,  dc, tlat, tlon, pt->m_lat, pt->m_lon,
						UTIL_COLOR_YELLOW, PS_SOLID, yellow_width,
                  UTIL_LINE_TYPE_GREAT, TRUE, FALSE );
            
            if ( m_background )
		         futil->draw_geo_line(map,  dc, tlat, tlon, pt->m_lat, pt->m_lon,
					   UTIL_COLOR_BLACK, PS_SOLID, black_width,
                  UTIL_LINE_TYPE_GREAT, TRUE, FALSE );

				futil->draw_geo_line(map,  dc, tlat, tlon, pt->m_lat, pt->m_lon,
					m_color, PS_SOLID, line_width,
               UTIL_LINE_TYPE_GREAT, TRUE, m_background);
			}
			
			tlat = pt->m_lat;
			tlon = pt->m_lon;
			map->geo_to_surface(pt->m_lat, pt->m_lon, &tx, &ty);
			pt->m_pt.x = tx;
			pt->m_pt.y = ty;
			pt->m_rect.SetRect(tx-point_size, ty-point_size, tx+point_size, ty+point_size);
			
			if (first)
			{
				minx = maxx = tx;
				miny = maxy = ty;
			}
			else
			{
				if (tx < minx)
					minx = tx;
				if (tx > maxx)
					maxx = tx;
				if (ty < miny)
					miny = ty;
				if (ty > maxy)
					maxy = ty;
			}
			
			first = FALSE;
		}
		
		m_rect.SetRect(minx, miny, maxx, maxy);
	
		if (ovl->m_display_sof_points)
			draw_sof_lines(map, dc, m_selected);
		
	}


	if (m_type == 'A')
	{
		CString numstr;
//		POINT cpt[4];
		int sumx = 0;
		int sumy = 0;

		draw_racetrack(map, dc, m_color, m_background);
//		draw_start_end_lines(map, dc);
		for (k=0; k<4; k++)
		{
//			draw_point(dc, m_anchor_pt[k].x, m_anchor_pt[k].y, "AP", m_selected);
			if (k == 1)
				draw_point(dc, m_anchor_pt[k].x, m_anchor_pt[k].y, "AN", FALSE, FALSE);
			else
				draw_point(dc, m_anchor_pt[k].x, m_anchor_pt[k].y, "AP", FALSE, FALSE);
			m_anchor_point[k].m_rect.SetRect(m_anchor_pt[k].x-point_size, m_anchor_pt[k].y-point_size, 
											m_anchor_pt[k].x+point_size, m_anchor_pt[k].y+point_size);

			ASSERT(m_anchor_point[k].m_usage.GetCount() > 0);
		}
	}

	int cnt = m_point_list.GetCount();

	// draw the points
	next = m_point_list.GetHeadPosition();
	while (next != NULL)
	{
		pt = m_point_list.GetNext(next);
		ASSERT(pt->m_usage.GetCount() > 0);
		if (m_type == 'A')
		{
			// compute latlon
			GEO_distance_to_geo(m_anchor_pt_lat, m_anchor_pt_lon, pt->m_dist * 1000.0, pt->m_angle + m_angle, &tlat, &tlon);
			pt->m_lat = tlat;
			pt->m_lon = tlon;
			ASSERT(tlat != 0.0);
			ASSERT(tlon != 0.0);
		}

		map->geo_to_surface(pt->m_lat, pt->m_lon, &tx, &ty);
		pt->m_pt.x = tx;
		pt->m_pt.y = ty;
		if ((m_type != 'A') && (ovl->m_display_sof_points))
			is_sof_point = TRUE;
		else
			is_sof_point = FALSE;
		draw_point(dc, pt, m_selected, is_sof_point);
		//pt->m_usage, pt->m_pt.x, pt->m_pt.y, pt->m_selected);
		if ((ovl->m_nMode == MM_AR_NAVAID) && m_selected)
			draw_point_to_navaid_line(map, dc, pt, UTIL_COLOR_WHITE);
		pt->m_rect.SetRect(pt->m_pt.x-point_size, pt->m_pt.y-point_size, pt->m_pt.x+point_size, pt->m_pt.y+point_size);
	}

	if (m_type == 'A' && (ovl->m_nMode == MM_AR_NAVAID) && m_selected)
	{
		for (k=0; k<4; k++)
		{
			draw_point_to_navaid_line(map, dc, &(m_anchor_point[k]), UTIL_COLOR_WHITE);
//			m_anchor_point[k].m_rect.SetRect(m_anchor_point[k].m_pt.x-8, m_anchor_point[k].m_pt.y-8, 
//											m_anchor_point[k].m_pt.x+8, m_anchor_point[k].m_pt.y+8);
		}
	}


	return SUCCESS;
}
// end of C_ar_obj::draw

// *****************************************************************
// *****************************************************************

void C_ar_obj::clear_selects() 
{ 
	POSITION next;
	C_ar_point *vtx;
	CIconImage*	draw_icon;
	C_ar_edit *ovl;

	ovl = (C_ar_edit*) m_overlay;
	if (!C_ar_edit::valid_overlay(ovl))
	{
		ASSERT(0);
		return;
	}

	draw_icon = ovl->ip_icon;

	m_selected = FALSE;
	if (m_point_list.IsEmpty())
		return;

	next = m_point_list.GetHeadPosition();
	while (next != NULL)
	{
		vtx = m_point_list.GetNext(next);
		if (vtx->m_selected)
		{
			OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
		}

		vtx->m_selected = FALSE;
	}
}
// end of clear_selects

// *****************************************************************
// *****************************************************************

CString C_ar_obj::get_tool_tip()
{
	CString tstr, tstr2, usage;
	BOOL rslt;
	C_ar_point *vtx;
	C_ar_edit *ovl;
	int pos, bear, dist;

	ovl = (C_ar_edit*) m_overlay;
	if (!C_ar_edit::valid_overlay(ovl))
	{
		ASSERT(0);
		return "";
	}

	rslt = hit_test(ovl->m_fuel_mouse_point, &vtx);

	tstr = m_id;
	if (vtx != NULL)
	{
//		vtx->get_usage_string(usage);
//		tstr += ", ";
//		tstr += usage;
		if (vtx->m_nav_id.GetLength() > 0)
		{
			tstr2 = vtx->m_nav_id;
			pos = tstr2.Find('/');
			if (pos > 0)
				tstr2 = tstr2.Left(pos);
			tstr += ", ";
			tstr += tstr2;
			if (vtx->m_fix.GetLength() > 0)
			{
				bear = atoi(vtx->m_fix.Left(3));
				dist = atoi(vtx->m_fix.Right(3));
				tstr2.Format(" %03d/%03d", bear, dist);
				tstr += tstr2;
			}
		}
	}
	return tstr;
}
// end of get_tool_tip

// *****************************************************************
// *****************************************************************

/*

CString C_ar_obj::get_tool_tip()
{
	CString tstr;
	char sline[81];
	BOOL rslt;
	POSITION next;
	C_ar_point *vtx;
	C_ar_edit *ovl;

	ovl = (C_ar_edit*) m_overlay;

	tstr = m_label;
	list_hit_test(ovl->m_fuel_mouse_point);
	if (ovl->m_refuel_vertex_hit_list.GetCount() > 0)
	{
		next = ovl->m_refuel_vertex_hit_list.GetHeadPosition();
		while (next != NULL)
		{
			vtx = ovl->m_refuel_vertex_hit_list.GetNext(next);
			rslt = C_ar_edit::refuel_usage_to_string(vtx->m_usage, sline);
			if (rslt)
			{
				tstr += ", ";
				tstr += sline;
			}
		}
	}
	return tstr;
}
// end of get_tool_tip

*/

// *****************************************************************
// *****************************************************************

CString C_ar_obj::get_help_text()
{
	CString text, tstr, temp;
	CArUtil util;
//	int len;

	text = get_tool_tip();
	text += ", ";
	util.decode_altitude(m_altdesc1, m_fuel1alt1, m_fuel1alt2, tstr);
	text += tstr;
	text += ", ";

	// add the frequencies
	if (m_prfreq.GetLength() > 0)
	{
//		tstr = m_prfreq.Left(3);
//		tstr += ".";
//		temp = m_prfreq.Mid(3, 3);
//		len = temp.GetLength();
//		if ((temp.GetAt(len-2) == '0') && (temp.GetAt(len-1) == '0'))
//			temp = temp.Left(len-2);
//		if (temp.GetLength() > len-1)
//			if (temp.GetAt(len-1) == '0')
//				temp = temp.Left(len-1);
//		tstr += temp;
		tstr = m_prfreq;
		tstr += "Mhz";
		text += tstr;

		if (m_bufreq.GetLength() > 0)
		{
			text += "/";
//			temp = m_bufreq.Left(3);
//			temp += ".";
//			temp += m_bufreq.Mid(3, 3);
//			len = temp.GetLength();
//			if ((temp.GetAt(len-2) == '0') && (temp.GetAt(len-1) == '0'))
//				temp = temp.Left(len-2);
//			if (temp.GetLength() > len-1)
//				if (temp.GetAt(len-1) == '0')
//					temp = temp.Left(len-1);
			temp = m_bufreq;
			temp += "Mhz";
			text += temp;
		}
	}
	
	return text;
}

// *****************************************************************
// *****************************************************************

void C_ar_obj::clear_points()
{
	C_ar_point *vtx;
	CString *txt;
	int k;

	while (!m_point_list.IsEmpty())
	{
		vtx = m_point_list.RemoveHead();
		delete vtx;
	}

	for (k=0; k<4; k++)
	{
		while (!m_anchor_point[k].m_usage.IsEmpty())
		{
			txt = m_anchor_point[k].m_usage.RemoveHead();
			delete txt;
		}
	}

}
// end of clear_points

// *****************************************************************
// *****************************************************************

int C_ar_obj::add_point(double lat, double lon)
{
	C_ar_point *pt;

	pt = new C_ar_point(this);
	pt->m_lat = lat;
	pt->m_lon = lon;
	m_point_list.AddTail(pt);

	C_ar_edit::m_old_cnt = m_point_list.GetCount();

	return SUCCESS;
}
// end of add_point

// *****************************************************************
// *************************************************************

BOOL C_ar_obj::rubber_band_point(MapProj* map, CDC* dc, CPoint oldpt, CPoint newpt, int index)
{
	POSITION pos;
	int x1, y1, x2, y2;
	C_ar_point* pt;
	int numpt;
	degrees_t lat1, lon1, lat2, lon2;
	degrees_t tlat, tlon;
	CFvwUtil *futil = CFvwUtil::get_instance();

	if ( m_point_list.IsEmpty() )
		return FALSE;
	if ((index <= 0) || (index > m_point_list.GetCount()))
		return FALSE;

	numpt = m_point_list.GetCount();	

	x1 = -1000001;
	x2 = -1000001;
	if (index > 1)
	{
		pos = m_point_list.FindIndex(index-2);
		if (pos == NULL)
			return FALSE;
		pt = m_point_list.GetAt(pos);
		x1 = pt->m_pt.x;
		y1 = pt->m_pt.y;
		lat1 = pt->m_lat;
		lon1 = pt->m_lon;
	}

	if (index < numpt)
	{
		pos = m_point_list.FindIndex(index);
		if (pos == NULL)
			return FALSE;
		pt = m_point_list.GetAt(pos);
		x2 = pt->m_pt.x;
		y2 = pt->m_pt.y;
		lat2 = pt->m_lat;
		lon2 = pt->m_lon;
	}

	if (oldpt.x >= -1000000)
	{
		map->surface_to_geo(oldpt.x, oldpt.y, &tlat, &tlon);
		// clear old lines
		if (x1 >= -1000000)
		{
			futil->draw_geo_line(map, dc, lat1, lon1, tlat, tlon, -1, UTIL_LINE_XOR, 2, PS_SOLID, TRUE, FALSE);
		}
		if (x2 >= -1000000)
		{
			futil->draw_geo_line(map, dc, lat2, lon2, tlat, tlon, -1, UTIL_LINE_XOR, 2, PS_SOLID, TRUE, FALSE);
		}
	}
	if (newpt.x >= -1000000)
	{
		map->surface_to_geo(newpt.x, newpt.y, &tlat, &tlon);
		// draw new lines
		if (x1 >= -1000000)
		{
			futil->draw_geo_line(map, dc, lat1, lon1, tlat, tlon, -1, UTIL_LINE_XOR, 2, PS_SOLID, TRUE, FALSE);
		}
		if (x2 >= -1000000)
		{
			futil->draw_geo_line(map, dc, lat2, lon2, tlat, tlon, -1, UTIL_LINE_XOR, 2, PS_SOLID, TRUE, FALSE);
		}
	}
		
	return TRUE;
}
// end of rubber_band_point

// *************************************************************
// *************************************************************

void C_ar_edit::rubber_band_anchor(MapProj* map, CDC* dc, CPoint point1, CPoint point2) 
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	int length, width, tx, ty;
	double angle;
	CPoint sl1, sl2, sr1, sr2;
	CPoint ctr1, ctr2;

	sr1.x = point1.x;
	sr1.y = point1.y;
	sr2.x = point2.x;
	sr2.y = point2.y;
	futil->point_to_vector(point1.x, point1.y, point2.x, point2.y, &length, &angle);
	width = length / 2;
	futil->vector_to_point(point1.x, point1.y, width, angle - 90.0, &tx, &ty);
	sl1.x = tx;
	sl1.y = ty;
	futil->vector_to_point(point2.x, point2.y, width, angle - 90.0, &tx, &ty);
	sl2.x = tx;
	sl2.y = ty;

	ctr1.x = (sl1.x + sr1.x) / 2;
	ctr1.y = (sl1.y + sr1.y) / 2;
	ctr2.x = (sl2.x + sr2.x) / 2;
	ctr2.y = (sl2.y + sr2.y) / 2;

	futil->line_xor(dc, 2, sl1, sl2);
	futil->line_xor(dc, 2, sr1, sr2);
	futil->arc(dc, ctr1, sr1, sl1, UTIL_COLOR_WHITE, 3, TRUE);
	futil->arc(dc, ctr2, sl2, sr2, UTIL_COLOR_WHITE, 3, TRUE);

}
// end of rubber_band_anchor


// *************************************************************
// *************************************************************

//void C_ar_obj::rubber_band_anchor(MapProj* map, CDC* dc, CPoint point1, CPoint point2, BOOL left) 
//{
//}
// end of rubber_band_anchor

// *************************************************************
// *************************************************************

void C_ar_obj::rubber_band_anchor(MapProj* map, CDC* dc, CPoint point, CPoint *anchor_pt) 
{
	CFvwUtil *futil = CFvwUtil::get_instance();
	int length, width, tx, ty, anchor_num;
	double angle, angle2;
	CPoint sl1, sl2, sr1, sr2;
	CPoint ctr1, ctr2;
	C_ar_point *vtx = NULL;
	BOOL left = TRUE;

	if (anchor_pt->x < -1000000)
	{
		// calculate the anchor point
		anchor_num = find_anchor_point();
		if (anchor_num < 0)
			return;
		anchor_pt->x = m_anchor_pt[anchor_num].x;
		anchor_pt->y = m_anchor_pt[anchor_num].y;
	}

	anchor_num = find_anchor_point();
	if (anchor_num < 0)
		return;

//	rubber_band_anchor(map, dc, point, *anchor_pt, left);

	return;

//	if (anchor_num < 2)
//	{
		angle2 = angle + 90.0;
		left = FALSE;
//	}
//	else
//		angle2 = angle - 90.0;

	sr1.x = point.x;
	sr1.y = point.y;
	sr2.x = anchor_pt->x;
	sr2.y = anchor_pt->y;
	futil->point_to_vector(point.x, point.y, anchor_pt->x, anchor_pt->y, &length, &angle);
	width = length / 2;
	futil->vector_to_point(point.x, point.y, width, angle2, &tx, &ty);
	sl1.x = tx;
	sl1.y = ty;
	futil->vector_to_point(anchor_pt->x, anchor_pt->y, width, angle2, &tx, &ty);
	sl2.x = tx;
	sl2.y = ty;

	ctr1.x = (sl1.x + sr1.x) / 2;
	ctr1.y = (sl1.y + sr1.y) / 2;
	ctr2.x = (sl2.x + sr2.x) / 2;
	ctr2.y = (sl2.y + sr2.y) / 2;

	futil->line_xor(dc, 2, sl1, sl2);
	futil->line_xor(dc, 2, sr1, sr2);
//	if (!left)
//	{
		futil->arc(dc, ctr1, sl1, sr1, UTIL_COLOR_WHITE, 3, TRUE);
		futil->arc(dc, ctr2, sr2, sl2, UTIL_COLOR_WHITE, 3, TRUE);
//	}
//	else
//	{
//		futil->arc(dc, ctr1, sr1, sl1, UTIL_COLOR_WHITE, 3, TRUE);
//		futil->arc(dc, ctr2, sl2, sr2, UTIL_COLOR_WHITE, 3, TRUE);
//	}

}
// end of rubber_band_anchor

// *************************************************************
// *************************************************************

// given a point near one of the anchor positions, find the other point on the same leg

int C_ar_obj::find_anchor_point() 
{
	int k;
//	BOOL unique;

	k = 0;
	while (k<4)
	{
//		if (m_anchor_point[k].has_usage_string("AN", &unique))
		if (m_anchor_point[k].m_anchor_point)
			return k;
		k++;
	}

	ASSERT(0);
	return -1;
}
// end of find_anchor_point

// *************************************************************
// *************************************************************

// given a point near one of the anchor positions, find the other point on the same leg

void C_ar_obj::set_anchor_point(MapProj* map, CPoint point) 
{
	int k;
//	int tx, ty;
	int dist[4], maxdist, mindist;
	int curpt, minpt, ancpt, ancdist;
	int curdist;
	CFvwUtil *futil = CFvwUtil::get_instance();
	CPoint temppt;
	C_ar_point temp_ar_point(this);
//	double tlat, tlon;

	// find the current point
	curpt = 0;
	curdist = 0;
	minpt = 0;
	ancpt = 0;
	mindist = 99999;
	ancdist = 99999;
	for (k=0; k<4; k++)
	{
		dist[k] = futil->magnitude(point.x, point.y, m_anchor_pt[k].x, m_anchor_pt[k].y);
		if (dist[k] > curdist)
		{
			curpt = k;
			curdist = dist[k];
		}
		if ((dist[k] > 0) && (dist[k] < mindist))
		{
			minpt = k;
			mindist = dist[k];
		}
		if (dist[k] < ancdist)
		{
			ancpt = k;
			ancdist = dist[k];
		}
	}

	maxdist = curdist;
	curdist = 0;
	curpt = -1;

	// find the second longest distance
	for (k=0; k<4; k++)
	{
		if ((dist[k] < maxdist) && (dist[k] > curdist))
		{
			curdist = dist[k];
			curpt = k;
		}
	}

}
// end of set_anchor_point

// *************************************************************
// *************************************************************

BOOL C_ar_obj::get_index_before_pt(MapProj* map, CPoint testpt, int *index_before)
{
	POSITION next, tail_position;
	C_ar_point* pt;
	CFvwUtil *futil = CFvwUtil::get_instance();
   COvlkitUtil util;
	BOOL notdone, found, first, last, first_time;
	int cnt, rslt;
	int fx, fy, tx, ty, sx, sy;
	double tlat, tlon, flat, flon;
	CPoint testpoint;
	int dist;

	*index_before = 0;
	if ( m_point_list.IsEmpty() )
		return FALSE;

	// get point on the center line if using sof lines
	C_ar_edit* ovl;
	ovl = (C_ar_edit*) m_overlay;
	int radius = ovl->m_sof_display_radius;

	tx = 0;
	ty = 0;
	tail_position = m_point_list.GetTailPosition();
	next = m_point_list.GetHeadPosition();
	first = TRUE;
	last = FALSE;
	notdone = TRUE;
	found = FALSE;
	first_time = TRUE;
	cnt = 0;
	while (notdone)
	{
	    if (((next == tail_position) || (next == NULL)) && first)
		{
			last = TRUE;
		}
		if (next == tail_position)
			first = FALSE;
		pt = m_point_list.GetNext(next);

		if (!first_time)
		{
			testpoint.x = testpt.x;
			testpoint.y = testpt.y;
			if (ovl->m_display_sof_points)
			{
				// make sure the point is on a sof line
				dist = futil->distance_to_line(tx, ty, pt->m_pt.x, pt->m_pt.y, testpt.x, testpt.y);
				rslt = FAILURE;
				if (abs(dist - radius) < 3)
					rslt = futil->closest_point_on_line(tx, ty, pt->m_pt.x, pt->m_pt.y, testpt.x, testpt.y, &sx, &sy);
				if (rslt == SUCCESS)
				{
					testpoint.x = sx;
					testpoint.y = sy;
				}
/*
				// strictly a test thing, remove it after use
				CPen pen, *oldpen;
				CBrush brush, *oldbrush;
				extern CMapView* fvw_get_view();
				CMapView* view = fvw_get_view();
				CWindowDC dc(view);
				pen.CreatePen(PS_SOLID, 1, RGB(0,255,0));
				brush.CreateSolidBrush(RGB(0,0,255));
				oldpen = dc.SelectObject(&pen);
				oldbrush = dc.SelectObject(&brush);
				dc.MoveTo(tx, ty);
				dc.LineTo(pt->m_pt.x, pt->m_pt.y);
				dc.Ellipse(sx-4, sy-4, sx+4, sy+4);
				dc.SelectObject(oldpen);
				dc.SelectObject(oldbrush);
*/
			}
		}

		if (first_time)
		{
			first_time = FALSE;
			fx = pt->m_pt.x;
			fy = pt->m_pt.y;
			flat = pt->m_lat;
			flon = pt->m_lon;
		}
		else if (util.geo_line_hit_test(map, pt->m_lat, pt->m_lon, tlat, tlon, 2, UTIL_LINE_TYPE_GREAT, testpoint))
		{
			found = TRUE;
			notdone = FALSE;
			*index_before = cnt;
			continue;
		}
		tx = pt->m_pt.x;
		ty = pt->m_pt.y;
		tlat = pt->m_lat;
		tlon = pt->m_lon;
		if (last)
		{
			notdone = FALSE;
			continue;
		}
		cnt++;
	}
	if (!found)
		if (util.geo_line_hit_test(map, pt->m_lat, pt->m_lon, flat, flon, 2, UTIL_LINE_TYPE_GREAT, testpoint))
		{
			found = TRUE;
			*index_before = -1;
		}
	return found;
}
// end of get_index_before_pt

// *************************************************************
// *************************************************************

BOOL C_ar_obj::insert_vertex(int index_before, int x, int y, double lat, double lon) 
{
	POSITION pos;
	int count;
	C_ar_edit *ovl;

	if (!GEO_valid_degrees(lat, lon))
		ASSERT(0);

	if ( m_point_list.IsEmpty() )
		return FALSE;
	count = m_point_list.GetCount();
	if ((index_before <= 0) || (index_before > count))
		return FALSE;
	pos = m_point_list.FindIndex(index_before-1);
	if (pos == NULL)
		return FALSE;
	C_ar_point* pt = new C_ar_point(this);
	pt->m_id = m_id;
	pt->m_lat = lat;
	pt->m_lon = lon;
	pt->m_num = count+1;
	pt->m_type = m_type;
	m_point_list.InsertAfter(pos, pt);
	ovl = (C_ar_edit*) m_overlay;
	if (!C_ar_edit::valid_overlay(ovl))
	{
		ASSERT(0);
		return FALSE;
	}

	ovl->m_cur_point = pt;
	compute_bounds();
	return TRUE;
}
// end of intert_vertex

// *************************************************************
// *************************************************************

BOOL C_ar_obj::insert_vertex(MapProj* map, CPoint newpt) 
{
	int index_before;
	degrees_t lat, lon;
	BOOL rslt;

	if ( m_point_list.IsEmpty() )
		return FALSE;

	if (!get_index_before_pt(map, newpt, &index_before))
		return FALSE;

	map->surface_to_geo(newpt.x, newpt.y, &lat, &lon);

	rslt = insert_vertex(index_before, newpt.x, newpt.y, lat, lon);
	return rslt;
}
// end of intert_vertex

// *************************************************************
// *************************************************************

BOOL C_ar_obj::insert_vertex(MapProj* map, CPoint newpt, int* new_vertex_num) 
{
	int index_before;
	degrees_t lat, lon;
	BOOL rslt;

	if ( m_point_list.IsEmpty() )
		return FALSE;

	if (!get_index_before_pt(map, newpt, &index_before))
		return FALSE;

	map->surface_to_geo(newpt.x, newpt.y, &lat, &lon);

	rslt = insert_vertex(index_before, newpt.x, newpt.y, lat, lon);
	*new_vertex_num = index_before+1;
	return rslt;
}
// end of intert_vertex

// *************************************************************
// *************************************************************

BOOL C_ar_obj::insert_vertex_on_line(MapProj* map, CPoint newpt, int* new_vertex_num) 
{
	POSITION pos;
	C_ar_point* pt;
	int index_before;
	degrees_t lat, lon;
	BOOL rslt;
	int x1, y1, x2, y2, xn, yn;

	if ( m_point_list.IsEmpty() )
		return FALSE;

	if (!get_index_before_pt(map, newpt, &index_before))
		return FALSE;

	// new pt is between last and first point
	if (index_before < 0)
	{
		return FALSE;
	}

	pos = m_point_list.FindIndex(index_before-1);
	if (pos == NULL)
		return FALSE;
	pt = m_point_list.GetAt(pos);
	map->geo_to_surface(pt->m_lat, pt->m_lon, &x1, &y1);
	pos = m_point_list.FindIndex(index_before);
	if (pos == NULL)
		return FALSE;
	pt = m_point_list.GetAt(pos);
	map->geo_to_surface(pt->m_lat, pt->m_lon, &x2, &y2);
	rslt = point_on_line(x1, y1, x2, y2, newpt.x, newpt.y, &xn, &yn);
	if (!rslt)
		return FALSE;

	map->surface_to_geo(xn, yn, &lat, &lon);

	rslt = insert_vertex(index_before, xn, yn, lat, lon);
	*new_vertex_num = index_before+1;

	// fix up the point types
	BOOL first = TRUE;

	pos = m_point_list.GetHeadPosition();
	while (pos != NULL)
	{
		pt = m_point_list.GetNext(pos);
		if (!first && (pos != NULL))
		{
			pt->clear_usages();
			pt->add_usage("NC");
		}
		first = FALSE;
	}

//	pos = m_point_list.GetLastPosition();
//	pt = m_point_list.GetPrev(pos);
//	pt->add_usage("EX");
	pos = m_point_list.GetHeadPosition();
	pt = m_point_list.GetNext(pos);
	pt = m_point_list.GetNext(pos);
	if (pt != NULL)
	{
		pt->clear_usages();
		pt->add_usage("CP");
	}

	return rslt;
}
// end of insert_vertex_on_line

// *************************************************************
// *************************************************************

C_ar_point* C_ar_obj::get_vertex(int index)
{
	C_ar_point* pt = NULL;
	int cnt, k;
	BOOL notdone;
	POSITION pos;

	cnt = m_point_list.GetCount();
	if (cnt < 1)
		return NULL;

	if ((index > cnt) || (index < 0))
		return NULL;

	pos = m_point_list.GetHeadPosition();
	pt = m_point_list.GetNext(pos);
	k = 0;
	notdone = TRUE;
	while (notdone)
	{
		if (k == index)
		{
			notdone = FALSE;
			continue;
		}
		k++;
		if (k>=cnt)
		{
			pt = NULL;
			notdone = FALSE;
			continue;
		}
		pt = m_point_list.GetNext(pos);
	}
	return pt;
}
// end of get_vertex

// *************************************************************
// *************************************************************

BOOL C_ar_obj::move_vertex(int index, double lat, double lon)
{
	POSITION pos;
	C_ar_point* pt;

	if ( m_point_list.IsEmpty() )
		return FALSE;

	if ((index <= 0) || (index > m_point_list.GetCount()))
		return FALSE;

	pos = m_point_list.FindIndex(index-1);
	if (pos == NULL)
		return FALSE;
	pt = m_point_list.GetAt(pos);
	pt->m_lat = lat;
	pt->m_lon = lon;
	m_point_list.SetAt(pos, pt);
	compute_bounds();
	return TRUE;
}
// end of move_vertex

// *************************************************************
// *************************************************************

BOOL C_ar_obj::compute_bounds()
{
	POSITION next, tail_position, oldpos;
	C_ar_point* pt;
	BOOL first;
	double ll_lat, ll_lon, ur_lat, ur_lon;
	int llx, lly, urx, ury;

	if ( m_point_list.IsEmpty() )
		return FALSE;
	tail_position = m_point_list.GetTailPosition();
	next = m_point_list.GetHeadPosition();
	first = TRUE;
//	while (((next != tail_position) && (next != NULL)) || first)
	while (next != NULL)
	{
		oldpos = next;
		pt = m_point_list.GetNext(next);
		if (first)
		{
			first = FALSE;
			ll_lat = pt->m_lat;
			ll_lon = pt->m_lon;
			ur_lat = pt->m_lat;
			ur_lon = pt->m_lon;
			llx = pt->m_pt.x;
			lly = pt->m_pt.y;
			urx = pt->m_pt.x;
			ury = pt->m_pt.y;
		}
		else
		{
			if (pt->m_lat < ll_lat)
				ll_lat = pt->m_lat;
			if (pt->m_lat > ur_lat)
				ur_lat = pt->m_lat;
//			if (is_left_of(pt->m_lon, ll_lon))
			if (!GEO_east_of_degrees(pt->m_lon, ll_lon))
				ll_lon = pt->m_lon;
//			if (is_left_of(ur_lon, pt->m_lon))
			if (!GEO_east_of_degrees(ur_lon, pt->m_lon))
				ur_lon = pt->m_lon;
			if (llx > pt->m_pt.x)
				llx =  pt->m_pt.x;
			if (lly < pt->m_pt.y)
				lly =  pt->m_pt.y;
			if (urx < pt->m_pt.x)
				urx =  pt->m_pt.x;
			if (ury > pt->m_pt.y)
				ury =  pt->m_pt.y;
		}
	}
	// set the lat/long and xy bounds
	m_bounds.SetRect(llx, ury, urx, lly);
	m_ll_lat = ll_lat;
	m_ll_lon = ll_lon;
	m_ur_lat = ur_lat;
	m_ur_lon = ur_lon;	
	return TRUE;
	
}
// end of compute_bounds

// *************************************************************
// *************************************************************

// finds the perpendicular projection of a point onto a line
BOOL C_ar_obj::point_on_line (int x1, int y1,  /* pt on the line */
								  int x2, int y2,   /* pt on the line */
								  int xp, int yp,   /* pt to project */
								  int* xn, int* yn) // projected point
{
	double bx, by, cx, cy;
	double len_sq;
	double r;

	// --- translate all points to a = (0,0) --- 
	bx = (double) (x2 - x1);
	cx = (double) (xp - x1);
	by = (double) (y2 - y1);
	cy = (double) (yp - y1);

	// ---  check for coincident line points --- 
	if ((x1 == x2) && (y1 == y2))
		return FALSE;

	len_sq = (bx * bx) + (by * by);

	r = ((cy * by) + (cx * bx)) / len_sq;

	*xn = (int) (x1 + (r * bx));
	*yn = (int) (y1 + (r * by));

	if ((r < 0) || (r > 1))
		return FALSE;

	return TRUE;
}
// end of point_on_line 

// *************************************************************
// *************************************************************

BOOL C_ar_obj::select_point(int index, BOOL selected)
{
	POSITION pos;
	C_ar_point* pt;

	if ( m_point_list.IsEmpty() )
		return FALSE;
	if ((index <= 0) || (index > m_point_list.GetCount()))
		return FALSE;

	pos = m_point_list.FindIndex(index-1);
	if (pos == NULL)
		return FALSE;
	pt = m_point_list.GetAt(pos);
	pt->m_selected = selected;
	m_point_list.SetAt(pos, pt);
//	if (selected)
//		m_selected = TRUE;
	return TRUE;
}
// end of select_point

// *************************************************************
// *****************************************************************

void C_ar_obj::delete_route_point(C_ar_point *pt)
{
	POSITION next, old_pos;
	C_ar_point *vtx;

	if (m_point_list.IsEmpty())
		return;

	next = m_point_list.GetHeadPosition();
	while (next != NULL)
	{
		old_pos = next;
		vtx = m_point_list.GetNext(next);
		if (vtx == pt)
		{
			m_point_list.RemoveAt(old_pos);
			delete vtx;
			next = NULL;
		}
	}
}
// end of delete_route_point


// *****************************************************************
// *****************************************************************

// compute the four anchor based on the basic parameters
void C_ar_obj::compute_anchor_points(ActiveMap* map)
{
	int tx, ty;
	double tlat, tlon, ang, lat2, lon2;
	double oth_angle;

	ang = m_angle + 180.0;
	if (ang > 360.0)
		ang -= 360.0;

	if (m_anchor_left)
		oth_angle = m_angle - 90.0;
	else
		oth_angle = m_angle + 90.0;

	GEO_distance_to_geo(m_lat, m_lon, m_anchor_length * 1852, m_angle, &lat2, &lon2);
	map->geo_to_surface(m_lat, m_lon, &tx, &ty);
	m_anchor_pt[0].x = tx;
	m_anchor_pt[0].y = ty;
	map->geo_to_surface(lat2, lon2, &tx, &ty);
	m_anchor_pt[1].x = tx;
	m_anchor_pt[1].y = ty;
	GEO_distance_to_geo(lat2, lon2, m_anchor_width * 1852.0, oth_angle, &tlat, &tlon);
	map->geo_to_surface(tlat, tlon, &tx, &ty);
	m_anchor_pt[2].x = tx;
	m_anchor_pt[2].y = ty;
	GEO_distance_to_geo(m_lat, m_lon, m_anchor_width * 1852.0, oth_angle, &tlat, &tlon);
	map->geo_to_surface(tlat, tlon, &tx, &ty);
	m_anchor_pt[3].x = tx;
	m_anchor_pt[3].y = ty;
}
// end of compute_anchor_points	

// *****************************************************************
// *****************************************************************

// requires that m_anchor_pt_lat, m_anchor_pt_lon, m_anchor_length, m_anchor_width, m_angle, m_anchor_left be set

void C_ar_obj::make_anchor(MapProj* map)
{
	double ang, oth_angle, mag_angle, magvar;
	double tlat, tlon;
	double lat2, lon2;
	int k, tx, ty, rslt;
	CFvwUtil *futil = CFvwUtil::get_instance();
	C_ar_edit *ovl;
	C_ar_point tpt(this);

	if (m_type != 'A')
		return;

	ASSERT(GEO_valid_degrees(m_anchor_pt_lat, m_anchor_pt_lon));

	if (!GEO_valid_degrees(m_anchor_pt_lat, m_anchor_pt_lon))
		return;
	if (m_anchor_length <= 0.0)
		return;
	if (m_anchor_width <= 0.0)
		return;

	ovl = (C_ar_edit*) m_overlay;

	rslt = GEO_current_magnetic_variation(m_anchor_pt_lat, m_anchor_pt_lon, 0, &magvar);
	if (rslt != SUCCESS)
		magvar = 0.0;

	mag_angle = m_angle + magvar;

	mag_angle = futil->normalize_angle(mag_angle);

	if (ovl->m_originate)
		ang = mag_angle;
	else
		ang = mag_angle + 180.0;

	if (ang > 360.0)
		ang -= 360.0;

	if (!m_anchor_left)
		oth_angle = mag_angle - 90.0;
	else
		oth_angle = mag_angle + 90.0;

	if (TRUE)
		oth_angle += 180.0;
	if (oth_angle > 360.0)
		oth_angle -= 360.0;

	GEO_distance_to_geo(m_anchor_pt_lat, m_anchor_pt_lon, m_anchor_length * 1852.0, ang, &lat2, &lon2);
	GEO_distance_to_geo(m_anchor_pt_lat, m_anchor_pt_lon, m_ip_dist * 1852.0, ang, &tlat, &tlon);

	if (ovl->m_originate)
	{
		m_anchor_point[0].m_lat = m_anchor_pt_lat;
		m_anchor_point[0].m_lon = m_anchor_pt_lon;
		m_anchor_point[0].clear_usages();
		m_anchor_point[0].add_usage("AN");
		m_anchor_point[0].m_anchor_point = TRUE;
		m_anchor_point[1].m_lat = lat2;
		m_anchor_point[1].m_lon = lon2;
//		m_anchor_point[1].remove_usage("AN");
		m_anchor_point[1].clear_usages();
		m_anchor_point[1].m_anchor_point = FALSE;
	}
	else
	{
		m_anchor_point[0].m_lat = lat2;
		m_anchor_point[0].m_lon = lon2;
		m_anchor_point[0].clear_usages();
//		m_anchor_point[0].remove_usage("AN");
		m_anchor_point[0].m_anchor_point = FALSE;
		m_anchor_point[1].m_lat = m_anchor_pt_lat;
		m_anchor_point[1].m_lon = m_anchor_pt_lon;
		m_anchor_point[1].clear_usages();
		m_anchor_point[1].add_usage("AN");
		m_anchor_point[1].m_anchor_point = TRUE;
	}
	m_anchor_point[0].m_type = m_type;
	m_anchor_point[0].add_usage("AP");
	m_anchor_point[1].m_type = m_type;
	m_anchor_point[1].add_usage("AP");

	GEO_distance_to_geo(lat2, lon2, m_anchor_width * 1852.0, oth_angle, &tlat, &tlon);

	if (ovl->m_originate)
	{
		m_anchor_point[2].m_lat = tlat;
		m_anchor_point[2].m_lon = tlon;
	}
	else
	{
		m_anchor_point[3].m_lat = tlat;
		m_anchor_point[3].m_lon = tlon;
	}
	m_anchor_point[2].m_type = m_type;
	m_anchor_point[2].add_usage("AP");

	GEO_distance_to_geo(m_anchor_pt_lat, m_anchor_pt_lon, m_anchor_width * 1852.0, oth_angle, &tlat, &tlon);
	if (ovl->m_originate)
	{
		m_anchor_point[3].m_lat = tlat;
		m_anchor_point[3].m_lon = tlon;
	}
	else
	{
		m_anchor_point[2].m_lat = tlat;
		m_anchor_point[2].m_lon = tlon;
	}
	m_anchor_point[3].m_type = m_type;
	m_anchor_point[3].add_usage("AP");

	lat2 = 0.0;
	lon2 = 0.0;
	for (k=0; k< 4; k++)
	{
		tlat = m_anchor_point[k].m_lat;
		tlon = m_anchor_point[k].m_lon;
		m_anchor_point[k].m_id = m_id;
		m_anchor_point[k].m_direction = m_direction;
		map->geo_to_surface(tlat, tlon, &tx, &ty);
		m_anchor_pt[k].x = tx;
		m_anchor_pt[k].y = ty;

		lat2 += tlat;
		lon2 += tlon;
	}
}
// end of make_anchor


// *****************************************************************
// *************************************************************

// Equal operator   
C_ar_obj &C_ar_obj::operator=(C_ar_obj &obj) 
{
	POSITION next;
	C_ar_point *pt, *tpt;
	CString *txt, *ttxt;
	int k;

	m_id = obj.m_id;
	m_direction = obj.m_direction;
	m_icao = obj.m_icao;
	m_country = obj.m_country;
	m_prfreq = obj.m_prfreq;
	m_bufreq = obj.m_bufreq;
	m_apnset = obj.m_apnset;
	m_apxcode = obj.m_apxcode;
	m_rcvr_chan = obj.m_rcvr_chan;
	m_tank_chan = obj.m_tank_chan;
	m_rectankchan = obj.m_rectankchan;
	m_altdesc1 = obj.m_altdesc1;
	m_fuel1alt1 = obj.m_fuel1alt1;
	m_fuel1alt2 = obj.m_fuel1alt2;
	m_altdesc2 = obj.m_altdesc2;
	m_fuel2alt1 = obj.m_fuel2alt1;
	m_fuel2alt2 = obj.m_fuel2alt2;
	m_altdesc3 = obj.m_altdesc3;
	m_fuel3alt1 = obj.m_fuel3alt1;
	m_fuel3alt2 = obj.m_fuel3alt2;
	m_unit = obj.m_unit;
	m_atc = obj.m_atc;
	m_cycledate = obj.m_cycledate;
	m_rect = obj.m_rect;
	ll_lat = obj.ll_lat;
	ll_lng = obj.ll_lng, 
	ur_lat = obj.ur_lat, 
	ur_lng = obj.ur_lng;
	m_bnd_ll.lat = obj.m_bnd_ll.lat;
	m_bnd_ll.lon = obj.m_bnd_ll.lon;
	m_bnd_ur.lat = obj.m_bnd_ur.lat;
	m_bnd_ur.lon = obj.m_bnd_ur.lon;

	// clear the point list
	while (!m_point_list.IsEmpty())
	{
		tpt = m_point_list.RemoveHead();
		delete tpt;
	}

	next = obj.m_point_list.GetHeadPosition();
	while (next != NULL)
	{
		tpt = obj.m_point_list.GetNext(next);
		pt = new C_ar_point(this);
		*pt = *tpt;
//		tpt->copy(pt);
		m_point_list.AddTail(pt);
	}

	next = m_remark_list.GetHeadPosition();
	while (next != NULL)
	{
		ttxt = m_remark_list.GetNext(next);
		txt = new CString;
		*txt = *ttxt;
		obj.m_remark_list.AddTail(txt);
	}

	m_remark_ptr = obj.m_remark_ptr;
	m_remark_cnt = obj.m_remark_cnt;
	m_remarks = obj.m_remarks;
	m_tip_info = obj.m_tip_info;
	m_label = obj.m_label;
	m_selected = obj.m_selected;
	m_created = obj.m_created;
	m_selected_vertex = obj.m_selected_vertex;
	m_start_vertex = obj.m_start_vertex;
	m_type = obj.m_type;  // A = Anchor, T = Track, B = Anchor or Track

	m_color = obj.m_color;
	m_bounds = obj.m_bounds;
	m_ll_lat = obj.m_ll_lat;
	m_ll_lon = obj.m_ll_lon;
	m_ur_lat = obj.m_ur_lat;
	m_ur_lon = obj.m_ur_lon;

	// defining basic params for anchor
	m_anchor_length = obj.m_anchor_length;
	m_anchor_width = obj.m_anchor_width;
	m_lat = obj.m_lat;
	m_lon = obj.m_lon;
	m_angle = obj.m_angle;
	m_anchor_left = obj.m_anchor_left;

	m_ip_dist = obj.m_ip_dist; // in NM
	m_exit_dist = obj.m_exit_dist; // in NM

	for (k=0; k<4; k++)
	{
		m_anchor_pt[k] = obj.m_anchor_pt[k];
		m_anchor_point[k] = obj.m_anchor_point[k];
//		obj.m_anchor_point[k].copy(&(m_anchor_point[k]));
	}

	m_dc = obj.m_dc;

	return *this;
}
// end of operator=

// *************************************************************
// *****************************************************************

