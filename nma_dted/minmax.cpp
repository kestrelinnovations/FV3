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

// minmax.cpp


#include "stdafx.h"
#include "nma_dted.h"
#include "fvwutil.h"
#include "refresh.h"


//#define debugdraw
#ifdef debugdraw
#include "mapx.h"
 //debug only
 int clr[3] = {UTIL_COLOR_WHITE, UTIL_COLOR_GREEN, UTIL_COLOR_YELLOW};
 int clrcnt = 0;
#endif

// *************************************************************
// *************************************************************
// *************************************************************
// *************************************************************

int NDTD_get_max_elev_in_poly(combo_dted_hit_and_miss_cache *dted_cache1, 
							  combo_dted_hit_and_miss_cache *dted_cache2, 
							  int numpt, double *lat, double *lon, 
							  int *max_elev, int *types_used)
{
	double tlat, tlon;
	int rslt;

	rslt = NDTD_get_max_elev_in_poly(dted_cache1, dted_cache2, numpt, lat, lon, max_elev, &tlat, &tlon, types_used);

	return rslt;
}

// *************************************************************
// *************************************************************

int NDTD_get_max_elev_in_poly(combo_dted_hit_and_miss_cache *dted_cache1, 
							  combo_dted_hit_and_miss_cache *dted_cache2,
							  int numpt, double *lat, double *lon, 
							  int *max_elev, double *max_elev_lat, double *max_elev_lon,
							  int *types_used)  // 1 = dted1, 2 = dted2, 4 = dted3
{
	CFvwUtil *futil = CFvwUtil::get_instance();

	int k, rslt;
	int topnum, botnum, leftnum, rightnum;
	int firstnum, secnum;
	double hx1, hy1, hx2, hy2;
	double vx1, vy1, vx2, vy2;
	double elevation = -99999.0;
	double toplat, botlat, leftlon, rightlon;
	double tlat, tlon, clat, clon, xlat, xlon;
	double slon, elon;
	double ew_dist, angle, tdist, ew_inc_dist;
	double inc = 1.0 / 1200.0;  // inc 3 arc sec = min DTED 1 spacing  (adj dor DTED2 below)
	double eps = 0.00000001;
	BOOL found;
	int elev;
	BOOL opposite = TRUE;
	BOOL ok;
	BOOL have_dted2 = FALSE;

	// init return values
	*max_elev = -32000;
	*max_elev_lat = 0.0;
	*max_elev_lon = 0.0;

	if (numpt < 3)
		return FAILURE;

	ok = FALSE;

	// search the perimeter for dted2
	for (k=0; k<numpt; k++)
	{
		//TRACE("Point Lat[%d] = %f, Lon[%d] = %f \n", k, lat[k], k, lon[k]);
		rslt = dted_cache2->get_elev_in_feet(lat[k], lon[k], &found, &elev);
		if (!found || (rslt != SUCCESS) || (elev < -12000))
		{
			rslt = dted_cache1->get_elev_in_feet(lat[k], lon[k], &found, &elev);
			if ((rslt == SUCCESS) && found && (elev > -12000))
			{
				ok = TRUE;
				if (elev > *max_elev)
				{
					*max_elev = elev;
					*max_elev_lat = lat[k];
					*max_elev_lon = lon[k];
					*types_used |= 1;
				}
			}
		}
		else
		{
			have_dted2 = TRUE;
			ok = TRUE;
			if (elev > *max_elev)
			{
				*max_elev = elev;
				*max_elev_lat = lat[k];
				*max_elev_lon = lon[k];
				*types_used |= 2;
			}
		}
	}

	// return if no dted found
	if (!ok)
		return FAILURE;

	// make the search increment smaller if we have dted2
	if (have_dted2)
		inc /= 3.0;

#ifdef debugdraw
// debug only
CView* view;
ViewMapProj* map;

view = UTL_get_active_non_printing_view();
if (view == NULL)
	return FAILURE;
map = UTL_get_current_view_map(view);
if (map == NULL)
	return FAILURE;

if (!map->is_projection_set())
	return FAILURE;

CClientDC dc(fvw_get_view());

//for (k=0; k<3; k++)
//	futil->draw_geo_line(map, &dc, lat[k], lon[k], lat[k+1], lon[k+1],
//								clr[k], PS_SOLID, 2,
//								UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
//    futil->draw_geo_line(map, &dc, lat[3], lon[3], lat[0], lon[0],
//							UTIL_COLOR_RED, PS_SOLID, 2,
//							UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
clrcnt++;
if (clrcnt > 2) 
	clrcnt = 0;
#endif

	// find the top-most point
	topnum = 0;
	for (k=1; k<4; k++)
		if (lat[k] > lat[topnum])
			topnum = k;

	// find the bottom-most point
	botnum = 0;
	for (k=1; k<4; k++)
		if (lat[k] < lat[botnum])
			botnum = k;

//	ASSERT (topnum != botnum);

	toplat = lat[topnum];
	botlat = lat[botnum];

	// find the left-most lon
	leftnum = 0;
	leftlon = lon[0];
	for (k=1; k<4; k++)
		if (GEO_east_of_degrees(leftlon, lon[k]))
		{
			leftlon = lon[k];
			leftnum = k;
		}

	// find the right-most lon
	rightnum = 0;
	rightlon = lon[0];
	for (k=1; k<4; k++)
	{
		if (GEO_east_of_degrees(lon[k], rightlon))
		{
			rightlon = lon[k];
			rightnum = k;
		}
	}

	// check for small poly
	if ((toplat - botlat) < inc)
	{
		if ((rightlon - leftlon) < inc)
		{
			// just check other three vetexes (we already checked the first)
			for (k=1; k<4; k++)
			{
				rslt = dted_cache2->get_elev_in_feet(lat[k], lon[k], &found, &elev);
				if (!found || (rslt != SUCCESS) || (elev < -12000))
				{
					rslt = dted_cache1->get_elev_in_feet(lat[k], lon[k], &found, &elev);
					*types_used |= 1;
				}
				else
				{
					*types_used |= 2;
				}

				if (!found || (rslt != SUCCESS) || (elev < -12000))
					goto BAD_VALUE;

// debug test
// futil->draw_geo_line(map, &dc, lat[k], lon[k], lat[k]+eps/1000.0, lon[k], UTIL_COLOR_RED, PS_SOLID, 2, UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
				if (rslt == SUCCESS)
				{
					if (elev > *max_elev)
					{
						*max_elev = elev;
						*max_elev_lat = lat[k];
						*max_elev_lon = lon[k];
					}
					if (elev < -12000)
						goto BAD_VALUE;
				}
			}
			return SUCCESS;
		}
		// find ew distance
		GEO_geo_to_distance(lat[leftnum], lon[leftnum], lat[rightnum], lon[rightnum], &ew_dist, &angle);
		// find post distance ew
		GEO_geo_to_distance(lat[leftnum], lon[leftnum], lat[leftnum], lon[leftnum] + inc, &ew_inc_dist, &angle);
		// step along the width
		tdist = 0;
		while (tdist < ew_dist)
		{
			GEO_distance_to_geo(lat[leftnum], lon[leftnum], tdist, angle, &tlat, &tlon);
			rslt = dted_cache2->get_elev_in_feet(tlat, tlon, &found, &elev);
			if (!found || (rslt != SUCCESS) || (elev < -12000))
			{
				rslt = dted_cache1->get_elev_in_feet(tlat, tlon, &found, &elev);
				*types_used |= 1;
			}
			else
			{
				*types_used |= 2;
			}
			if (!found || (rslt != SUCCESS) || (elev < -12000))
				goto BAD_VALUE;
// debug test
// futil->draw_geo_line(map, &dc, tlat, tlon, tlat+eps/1000.0, tlon, UTIL_COLOR_BLUE, PS_SOLID, 2, UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);

			if ((rslt == SUCCESS) && found)
			{
				if (elev > *max_elev)
				{
					*max_elev = elev;
					*max_elev_lat = tlat;
					*max_elev_lon = tlon;
				}
				if (elev < -12000)
					goto BAD_VALUE;
			}
			tdist += ew_inc_dist;
		}

		// also check the bounding points
		for (k=1; k<4; k++)
		{
			rslt = dted_cache2->get_elev_in_feet(lat[k], lon[k], &found, &elev);
			if (!found || (rslt != SUCCESS) || (elev < -12000))
			{
				rslt = dted_cache1->get_elev_in_feet(lat[k], lon[k], &found, &elev);
				*types_used |= 1;
			}
			else
			{
				*types_used |= 2;
			}
			if (!found || (rslt != SUCCESS) || (elev < -12000))
				goto BAD_VALUE;
// debug test
//  futil->draw_geo_line(map, &dc, lat[k], lon[k], lat[k]+eps/1000.0, lon[k], UTIL_COLOR_BLUE, PS_SOLID, 2, UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
			if ((rslt == SUCCESS) && found)
			{
				if (elev > *max_elev)
				{
					*max_elev = elev;
					*max_elev_lat = lat[k];
					*max_elev_lon = lon[k];
				}
				if (elev < -12000)
					goto BAD_VALUE;
			}
		}
		return SUCCESS;
	}

	if ((abs(topnum - botnum) % 2) != 0)
		opposite = FALSE;

	if (opposite)
	{
		leftnum = topnum - 1;
		if (leftnum < 0)
			leftnum += 4;
		rightnum = topnum + 1;
		if (rightnum > 3)
			rightnum -= 4;

		tlat = floor(lat[topnum]/inc)*inc; // start at the next post south of top latitude
		//tlon = floor(lon[topnum]/inc)*inc;

//		tlat -= inc;
		while (tlat > lat[botnum])
		{
			// find the intersection of a horizontal line at tlat and the "top" or "left" line of the polygon
			hy1 = hy2 = tlat;
			hx1 = leftlon - 1.0;
			hx2 = rightlon + 1.0;
			if (tlat >= lat[leftnum])
			{
				vx1 = lon[topnum];
				vy1 = lat[topnum]+eps;
				vx2 = lon[leftnum];
				vy2 = lat[leftnum]-eps;
			}
			else
			{
				vx1 = lon[botnum];
				vy1 = lat[botnum]-eps;
				vx2 = lon[leftnum];
				vy2 = lat[leftnum]+eps;
			}
			if (((hy1 < vy1) && (hy1 < vy2)) || ((hy1 > vy1) && (hy1 > vy2)))
				hy1 = hy2 = (vy1 + vy2) / 2.0;
			ok = futil->geo_line_segment_intersection(hy1, hx1, hy2, hx2, vy1, vx1, vy2, vx2, &clon, &clat);
			ASSERT(ok);
			slon = clon;

			// find the intersection of a horizontal line at tlat and the "right" or "bottom" line of the polygon
			if (tlat >= lat[rightnum])
			{
				vx1 = lon[topnum];
				vy1 = lat[topnum]+eps;
				vx2 = lon[rightnum];
				vy2 = lat[rightnum]-eps;
			}
			else
			{
				vx1 = lon[botnum];
				vy1 = lat[botnum]-eps;
				vx2 = lon[rightnum];
				vy2 = lat[rightnum]+eps;
			}
			if (((hy1 < vy1) && (hy1 < vy2)) || ((hy1 > vy1) && (hy1 > vy2)))
				hy1 = hy2 = (vy1 + vy2) / 2.0;
			ok = futil->geo_line_segment_intersection(hy1, hx1, hy2, hx2, vy1, vx1, vy2, vx2, &clon, &clat);
			ASSERT(ok);
			elon = clon;

			if (GEO_east_of_degrees(slon, elon))
			{
				clon = elon;
				elon = slon;
				slon = clon;
			}

			
			// check all the posts on the horizontal line between west and east intersection
			xlat = clat;
			xlon = ceil(slon/inc)*inc; // Start on the next post to the east of the intersection
			futil->normalize_geo(&xlat, &xlon);
			while (xlon <= elon)
			{
				if (FVW_is_escape_pressed())
					return USER_ABORT;
				rslt = dted_cache2->get_elev_in_feet(xlat, xlon, &found, &elev);
				if (!found || (rslt != SUCCESS) || (elev < -12000))
				{
					rslt = dted_cache1->get_elev_in_feet(xlat, xlon, &found, &elev);
					*types_used |= 1;
				}
				else
				{
					*types_used |= 2;
				}
				if (!found || (rslt != SUCCESS) || (elev < -12000))
					goto BAD_VALUE;

				
// debug test
//futil->draw_geo_line(map, &dc, xlat, xlon, xlat+eps/1000.0, xlon, UTIL_COLOR_BLACK, PS_SOLID, 2, UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
				if (rslt == SUCCESS)
				{
					if (elev > *max_elev)
					{
// debug test
//CRect bnds;
//POINT points[11];
//futil->draw_geo_ellipse(map, &dc, xlat, xlon, 0.08, 0.08,0.0, UTIL_COLOR_GREEN, PS_SOLID, 2, UTIL_LINE_TYPE_SIMPLE,1, FALSE, 10,points,&bnds);
						*max_elev = elev;
						*max_elev_lat = xlat;
						*max_elev_lon = xlon;
					}
					if (elev < -12000)
						goto BAD_VALUE;
				}
				xlon += inc;
			}
			tlat -= inc;
		}
	}
	else  // if !opposite
	{
		// are the two mid lat point to the left or right of the top/bottom line
		firstnum = topnum + 1;
		if (firstnum > 3)
			firstnum -= 4;
		secnum = topnum + 2;
		if (secnum > 3)
			secnum -= 4;
		if (firstnum == botnum)
		{
			firstnum = topnum - 1;
			if (firstnum < 0)
				firstnum += 4;
			secnum = topnum - 2;
			if (secnum < 0)
				secnum += 4;
		}

		tlat = floor(lat[topnum]/inc)*inc;  // start at first post south of top latitude
		//tlon = lon[topnum];

//		tlat -= inc;
		while (tlat > lat[botnum])
		{
			hy1 = hy2 = tlat;
			hx1 = leftlon - 1.0;
			hx2 = rightlon + 1.0;
			if (tlat >= lat[firstnum])
			{
				vx1 = lon[topnum];
				vy1 = lat[topnum]+eps;
				vx2 = lon[firstnum];
				vy2 = lat[firstnum]-eps;
			}
			else if (tlat >= lat[secnum])
			{
				vx1 = lon[firstnum];
				vy1 = lat[firstnum]+eps;
				vx2 = lon[secnum];
				vy2 = lat[secnum]-eps;
			}
			else
			{
				vx1 = lon[botnum];
				vy1 = lat[botnum]-eps;
				vx2 = lon[secnum];
				vy2 = lat[secnum]+eps;
			}
			if (((hy1 < vy1) && (hy1 < vy2)) || ((hy1 > vy1) && (hy1 > vy2)))
				hy1 = hy2 = (vy1 + vy2) / 2.0;
			ok = futil->geo_line_segment_intersection(hy1, hx1, hy2, hx2, vy1, vx1, vy2, vx2, &clon, &clat);
			ASSERT(ok);
			slon = clon;

			vx1 = lon[topnum];
			vy1 = lat[topnum]+eps;
			vx2 = lon[botnum];
			vy2 = lat[botnum]-eps;
			if (((hy1 < vy1) && (hy1 < vy2)) || ((hy1 > vy1) && (hy1 > vy2)))
				hy1 = hy2 = (vy1 + vy2) / 2.0;
			ok = futil->geo_line_segment_intersection(hy1, hx1, hy2, hx2, vy1, vx1, vy2, vx2, &clon, &clat);
			ASSERT(ok);
			elon = clon;

			if (GEO_east_of_degrees(slon, elon))
			{
				clon = elon;
				elon = slon;
				slon = clon;
			}

			// check all the posts on the horizontal line lat = tlat
			xlat = clat;
			xlon = ceil(slon/inc)*inc;  //start at first post east of intersection
			while (xlon <= elon)
			{
				rslt = dted_cache2->get_elev_in_feet(xlat, xlon, &found, &elev);
				if (!found || (rslt != SUCCESS) || (elev < -12000))
				{
					rslt = dted_cache1->get_elev_in_feet(xlat, xlon, &found, &elev);
					*types_used |= 1;
				}
				else
				{
					*types_used |= 2;
				}
				if (!found || (rslt != SUCCESS) || (elev < -12000))
					goto BAD_VALUE;
// debug test
//futil->draw_geo_line(map, &dc, xlat, xlon, xlat+eps/1000.0, xlon, UTIL_COLOR_GREEN, PS_SOLID, 2, UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
				if (rslt == SUCCESS)
				{
					if (elev > *max_elev)
					{
// debug test
//CRect bnds;
//POINT points[11];
//futil->draw_geo_ellipse(map, &dc, xlat, xlon, 0.1, 0.1,0.0, UTIL_COLOR_RED, PS_SOLID, 2, UTIL_LINE_TYPE_SIMPLE,1, FALSE, 10,points,&bnds);
						*max_elev = elev;
						*max_elev_lat = xlat;
						*max_elev_lon = xlon;
					}
					if (elev < -12000)
						goto BAD_VALUE;
				}
				xlon += inc;
			}
			tlat -= inc;
		}
	}

	return SUCCESS;

BAD_VALUE:
	*max_elev = -32000;
	*max_elev_lat = 0.0;
	*max_elev_lon = 0.0;
	return SUCCESS;
}
// end of get_max_elev_in_poly

// *************************************************************
// *************************************************************

