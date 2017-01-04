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

// elevcalc.cpp

#include "stdafx.h"
#include "elevcalc.h"
#include "err.h"

#include "Common\SafeArray.h"

//// debug only
//#include "fvwutil.h"
//#include "map.h"

// *************************************************************
// *************************************************************

CElevSeg::CElevSeg()
{
	// init vars
	m_lat = 0.0;
	m_lon = 0.0;
	m_left_width = 0.0;
	m_right_width = 0.0;
	m_max_elev = -99999;
	m_max_types_used = 0;
	m_valid_data = FALSE;
}

// *************************************************************
// *************************************************************

CElevSeg::~CElevSeg()
{
}

// *************************************************************
// *************************************************************

CElevLeg::CElevLeg()
{
	// init vars
	m_lat1 = 0.0;
	m_lon1 = 0.0;
	m_lat2 = 0.0;
	m_lon2 = 0.0;
	m_left_width = 0.0;
	m_right_width = 0.0;
	m_num_shape_points = 0;
	m_shape_lat = NULL;
	m_shape_lon = NULL;
	m_shape_angle = NULL;
	m_length = 0.0;
	m_bearing = 0.0;
	m_max_elev = -99999;
	m_max_types_used = 0;
	m_max_elev_lat = 0.0;
	m_max_elev_lon = 0.0;
	m_start_dist = 0.0;
	m_end_dist = 0.0;
	m_inc_dist = 0.0;
	m_num_points = 0;
	m_elev_array = NULL;
	m_elev_no_vvod_array = NULL;
	m_valid_data = FALSE;
}

// *************************************************************
// *************************************************************

CElevLeg::~CElevLeg()
{
	if (m_elev_array != NULL)
		free(m_elev_array);
	if (m_elev_no_vvod_array != NULL)
		free(m_elev_no_vvod_array);
	if (m_shape_lat != NULL)
		free(m_shape_lat);
	if (m_shape_lon != NULL)
		free(m_shape_lon);
	if (m_shape_angle != NULL)
		free(m_shape_angle);
}

// *************************************************************
// *************************************************************

CElevCalc::CElevCalc()
{
	m_callback = NULL;
	m_max_dted_level = 2;
}

// *************************************************************
// *************************************************************

CElevCalc::~CElevCalc()
{

}

// *************************************************************
// *************************************************************

int CElevCalc::get_max_elev_of_leg(double lat1, double lon1,
									double lat2, double lon2,
									double left_width, double right_width, // in meters
									int *max_elev, double *max_elev_lat, double *max_elev_lon, // OUTPUT in feet
									int *types_used) // 1 = dted1, 2 = dted2, 4 = dted3
{
	combo_dted_hit_and_miss_cache dted_cache1;
	combo_dted_hit_and_miss_cache dted_cache2;
	int rslt;

	dted_cache1.set_DTED_type(1);
	dted_cache2.set_DTED_type(2);

	rslt = get_max_elev_of_leg(lat1, lon1, lat2, lon2, left_width, right_width, 
								&dted_cache1, &dted_cache2,
								max_elev, max_elev_lat, max_elev_lon, types_used);	

	return rslt;
}

// *************************************************************
// *************************************************************

int CElevCalc::get_max_elev_of_leg(double lat1, double lon1,
									double lat2, double lon2,
									double left_width, double right_width, // in meters
									combo_dted_hit_and_miss_cache *dted_cache1,
									combo_dted_hit_and_miss_cache *dted_cache2,
									int *max_elev, double *max_elev_lat, double *max_elev_lon, // OUTPUT in feet
									int *types_used)  // 1 = dted1, 2 = dted2, 4 = dted3
{
	double sdist, edist, angle;
	double tlat, tlon, slat, slon;
	double plat[4], plon[4];  // start left, start right, end right, end left
	int rslt;
	BOOL first = TRUE;
	double inc = 1.0 / 1200.0;
	double tdist;

	rslt = GEO_geo_to_distance(lat1, lon1, lat2, lon2, &tdist, &angle);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		ERR_report("Invalid coordinates");
		return FAILURE;
	}

	sdist = 0.0;
	edist = tdist;

	ASSERT(left_width >= 0.0);
	ASSERT(right_width >= 0.0);
	ASSERT(left_width < 50000.0);
	ASSERT(right_width < 50000.0);

	if ((left_width < 0.0) || (right_width < 0.0) || (left_width >= 50000.0) || (left_width >= 50000.0))
	{
		ERR_report("Invalid corridor widths");
		return FAILURE;
	}

	slat = lat1;
	slon = lon1;
	angle -= 90.0;
	if (angle < 0.0)
		angle += 360.0;
	rslt = GEO_distance_to_geo(lat1, lon1, left_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		return FAILURE;
	}
	plat[0] = tlat;
	plon[0] = tlon;
	angle += 180.0;
	if (angle >= 360.0)
		angle -= 360.0;
	rslt = GEO_distance_to_geo(lat1, lon1, right_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		return FAILURE;
	}
	plat[1] = tlat;
	plon[1] = tlon;
	rslt = GEO_distance_to_geo(lat2, lon2, right_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		return FAILURE;
	}
	plat[2] = tlat;
	plon[2] = tlon;
	angle += 180.0;
	if (angle >= 360.0)
		angle -= 360.0;
	rslt = GEO_distance_to_geo(lat2, lon2, left_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		return FAILURE;
	}
	plat[3] = tlat;
	plon[3] = tlon;

	rslt =  NDTD_get_max_elev_in_poly(dted_cache1, dted_cache2, 4, plat, plon, max_elev, max_elev_lat, max_elev_lon, types_used);

	return rslt;
}
// end of get_max_elev_of_leg

// *************************************************************
// *************************************************************

void CElevCalc::GetVVODMaxElevInPoly(IVvodAnalysisPtr &smpVvodAnalysis, int nNumVertices, double *pLat, double *pLon, 
                                     int *pMaxElev, double *pMaxElevLat, double *pMaxElevLon)
{
   try
   {
      DoubleSafeArray saPolyVertices;
      for(int i=0;i<nNumVertices;++i)
      {
         saPolyVertices.Append(pLat[i]);
         saPolyVertices.Append(pLon[i]);
      }
      
      // If the maximum elevation from VVOD is greater, in the polygon, than DTED posts then use that.  Also,
      // use the max VVOD elevation if we failed to get DTED earlier (rslt != SUCCESS).
      int nMaxElev;
      double dMaxElevLat, dMaxElevLon;
      HRESULT hr = smpVvodAnalysis->GetMaxElevationInFeetPoly(&saPolyVertices, &nMaxElev, &dMaxElevLat, &dMaxElevLon);
      if (hr == E_PENDING)
      {
         // No VVOD data has been loaded -- release the object to stop from asking again
         smpVvodAnalysis = 0;
      }
      else if (hr == S_OK && nMaxElev > *pMaxElev)
      {
         *pMaxElev = nMaxElev;
         *pMaxElevLat = dMaxElevLat;
         *pMaxElevLon = dMaxElevLon;
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed obtaining max elevation of VVOD in poly: (%s)", (char *)e.Description());
      ERR_report(msg);
   }
}

int CElevCalc::get_max_elev_of_leg2(double lat1, double lon1,
									double lat2, double lon2,
									double angle1, double angle2,
									double left_width, double right_width, // in meters
									combo_dted_hit_and_miss_cache *dted_cache1,
									combo_dted_hit_and_miss_cache *dted_cache2,
									IVvodAnalysisPtr &smpVvodAnalysis,
									int *max_elev, int *max_elev_no_vvod, 
									double *max_elev_lat, double *max_elev_lon, // OUTPUT in feet
									int *types_used)    // 1 = dted1, 2 = dted2, 4 = dted3
{
	double angle;
	double tlat, tlon, slat, slon;
	double plat[4], plon[4];  // start left, start right, end right, end left
	int rslt;
	BOOL first = TRUE;
	double inc = 1.0 / 1200.0;

	ASSERT(left_width >= 0.0);
	ASSERT(right_width >= 0.0);
	ASSERT(left_width < 50000.0);
	ASSERT(right_width < 50000.0);

	if ((left_width < 0.0) || (right_width < 0.0) || (left_width >= 50000.0) || (left_width >= 50000.0))
	{
		ERR_report("Invalid corridor widths");
		return FAILURE;
	}

	slat = lat1;
	slon = lon1;
	angle = angle1 - 90.0;
	if (angle < 0.0)
		angle += 360.0;
	rslt = GEO_distance_to_geo(lat1, lon1, left_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		return FAILURE;
	}
	plat[0] = tlat;
	plon[0] = tlon;
	angle += 180.0;
	if (angle >= 360.0)
		angle -= 360.0;
	rslt = GEO_distance_to_geo(lat1, lon1, right_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		return FAILURE;
	}
	plat[1] = tlat;
	plon[1] = tlon;
	angle = angle2 + 90.0;
	if (angle >= 360.0)
		angle -= 360.0;
	rslt = GEO_distance_to_geo(lat2, lon2, right_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		return FAILURE;
	}
	plat[2] = tlat;
	plon[2] = tlon;
	angle += 180.0;
	if (angle >= 360.0)
		angle -= 360.0;
	rslt = GEO_distance_to_geo(lat2, lon2, left_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		return FAILURE;
	}
	plat[3] = tlat;
	plon[3] = tlon;

	rslt = NDTD_get_max_elev_in_poly(dted_cache1, dted_cache2, 4, plat, plon, max_elev, max_elev_lat, max_elev_lon, types_used);

	*max_elev_no_vvod = *max_elev;

   if (smpVvodAnalysis != NULL && rslt == SUCCESS)
      GetVVODMaxElevInPoly(smpVvodAnalysis, 4, plat, plon, max_elev, max_elev_lat, max_elev_lon);

	return rslt;
}
// end of get_max_elev_of_leg2

// *************************************************************
// *************************************************************

int CElevCalc::get_max_elev_of_leg_ends(double lat1, double lon1, double lat2, double lon2,
										double left_width, double right_width, // in meters
                              IVvodAnalysisPtr &smpVvodAnalysis,
										int *seg_max_elev, double *seg_max_elev_lat, double *seg_max_elev_lon, // OUTPUT in feet
										int *types_used)
{
	combo_dted_hit_and_miss_cache dted_cache1;
	combo_dted_hit_and_miss_cache dted_cache2;
	int rslt;

	dted_cache1.set_DTED_type(1);
	dted_cache2.set_DTED_type(2);

	rslt = get_max_elev_of_leg_ends(lat1, lon1, lat2, lon2, left_width, right_width, 
									&dted_cache1, &dted_cache2, smpVvodAnalysis,
									seg_max_elev, seg_max_elev_lat, seg_max_elev_lon, types_used);

	return rslt;
}

// *************************************************************
// *************************************************************

int CElevCalc::get_max_elev_of_leg_ends(double lat1, double lon1, double lat2, double lon2,
										double left_width, double right_width, // in meters
										combo_dted_hit_and_miss_cache *dted_cache1,
										combo_dted_hit_and_miss_cache *dted_cache2,
										IVvodAnalysisPtr &smpVvodAnalysis,
										int *seg_max_elev,   // OUTPUT in feet
										double *seg_max_elev_lat, double *seg_max_elev_lon,
										int *types_used)   // 1 = dted1, 2 = dted2, 4 = dted3
{
	int k, rslt;
	double angle, dist, leg_angle, plat[4], plon[4], tlat, tlon;
	int max_elev, max_elev_no_vvod;
	double max_elev_lat, max_elev_lon, max_width;
	BOOL bad_dted = FALSE;

//// debug only
//int j;
//CView* view;
//ViewMapProj* map;
//CFvwUtil *futil = CFvwUtil::get_instance();
//view = UTL_get_active_non_printing_view();
//if (view == NULL)
//	return FAILURE;
//map = UTL_get_current_view_map(view);
//if (map == NULL)
//	return FAILURE;
//if (!map->is_projection_set())
//	return FAILURE;
//CClientDC dc(OVL_get_overlay_manager()->get_view());


	*seg_max_elev = -99999;
	*seg_max_elev_lat = 0.0;
	*seg_max_elev_lon = 0.0;

	// check the first leg end
	rslt = GEO_geo_to_distance(lat1, lon1, lat2, lon2, &dist, &leg_angle);
	ASSERT(rslt == SUCCESS);
	if (rslt != SUCCESS)
	{
		ERR_report("Invalid coordinates");
		return FAILURE;
	}

	max_width = left_width;
	if (right_width > max_width)
		max_width = right_width;

	ASSERT(left_width >= 0.0);
	ASSERT(right_width >= 0.0);
	ASSERT(left_width < 50000.0);
	ASSERT(right_width < 50000.0);

	// find the boundaries the leg segment
	angle = leg_angle + 90.0;
	if (angle < 0.0)
		angle += 360.0;
	rslt = GEO_distance_to_geo(lat1, lon1, right_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);
	plat[0] = lat1;
	plon[0] = lon1;
	plat[3] = lat1;
	plon[3] = lon1;
	plat[1] = tlat;
	plon[1] = tlon;

	// check the first half of the end
	for (k=0; k<9; k++)
	{
		angle += 10.0;
		if (angle > 360.0)
			angle -= 360.0;
		rslt = GEO_distance_to_geo(lat1, lon1, right_width, angle, &tlat, &tlon);
		plat[2] = tlat;
		plon[2] = tlon;
		rslt =  NDTD_get_max_elev_in_poly(dted_cache1, dted_cache2, 4, plat, plon, &max_elev, &max_elev_lat, &max_elev_lon, types_used);
		if (rslt != SUCCESS)
			bad_dted = TRUE;

		max_elev_no_vvod = max_elev;
		if (max_elev < -12000)
			bad_dted = TRUE;

      if (smpVvodAnalysis != NULL && rslt == SUCCESS)
         GetVVODMaxElevInPoly(smpVvodAnalysis, 4, plat, plon, &max_elev, &max_elev_lat, &max_elev_lon);

		if (max_elev < -12000)
			bad_dted = TRUE;

		if ((rslt == SUCCESS) && (max_elev > *seg_max_elev))
		{
			*seg_max_elev = max_elev;
			*seg_max_elev_lat = max_elev_lat;
			*seg_max_elev_lon = max_elev_lon;
		}
//// degug only
//int j;
//for (j=0; j<3; j++)
//	futil->draw_geo_line(map, &dc, plat[j], plon[j], plat[j+1], plon[j+1],
//							UTIL_COLOR_RED, PS_SOLID, 2,
//							UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
//futil->draw_geo_line(map, &dc, plat[3], plon[3], plat[0], plon[0],
//						UTIL_COLOR_RED, PS_SOLID, 2,
//						UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);

		plat[1] = plat[2];
		plon[1] = plon[2];
	}

	rslt = GEO_distance_to_geo(lat1, lon1, left_width, angle, &tlat, &tlon);
	plat[1] = tlat;
	plon[1] = tlon;

	// check the second half of the end
	for (k=0; k<9; k++)
	{
		angle += 10.0;
		if (angle > 360.0)
			angle -= 360.0;
		rslt = GEO_distance_to_geo(lat1, lon1, left_width, angle, &tlat, &tlon);
		plat[2] = tlat;
		plon[2] = tlon;
		rslt =  NDTD_get_max_elev_in_poly(dted_cache1, dted_cache2, 4, plat, plon, &max_elev, &max_elev_lat, &max_elev_lon, types_used);
		if (rslt != SUCCESS)
			bad_dted = TRUE;

      if (smpVvodAnalysis != NULL && rslt == SUCCESS)
         GetVVODMaxElevInPoly(smpVvodAnalysis, 4, plat, plon, &max_elev, &max_elev_lat, &max_elev_lon);

		if (max_elev < -12000)
			bad_dted = TRUE;

		if ((rslt == SUCCESS) && (max_elev > *seg_max_elev))
		{
			*seg_max_elev = max_elev;
			*seg_max_elev_lat = max_elev_lat;
			*seg_max_elev_lon = max_elev_lon;
		}
//// degug only
//for (j=0; j<3; j++)
//futil->draw_geo_line(map, &dc, plat[j], plon[j], plat[j+1], plon[j+1],
//							UTIL_COLOR_RED, PS_SOLID, 2,
//							UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
//futil->draw_geo_line(map, &dc, plat[3], plon[3], plat[0], plon[0],
//						UTIL_COLOR_RED, PS_SOLID, 2,
//						UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);

		plat[1] = plat[2];
		plon[1] = plon[2];
	}

	// check the last leg end
	rslt = GEO_geo_to_distance(lat1, lon1, lat2, lon2, &dist, &leg_angle);
	ASSERT(rslt == SUCCESS);

	max_width = left_width;
	if (right_width > max_width)
		max_width = right_width;

	ASSERT(left_width >= 0.0);
	ASSERT(right_width >= 0.0);
	ASSERT(left_width < 50000.0);
	ASSERT(right_width < 50000.0);

	// find the boundaries the leg segment
	angle = leg_angle - 90.0;
	if (angle < 0.0)
		angle += 360.0;
	rslt = GEO_distance_to_geo(lat2, lon2, left_width, angle, &tlat, &tlon);
	ASSERT(rslt == SUCCESS);

	plat[0] = lat2;
	plon[0] = lon2;
	plat[3] = lat2;
	plon[3] = lon2;
	plat[1] = tlat;
	plon[1] = tlon;

	// check the first half of the end
	for (k=0; k<9; k++)
	{
		angle += 10.0;
		if (angle > 360.0)
			angle -= 360.0;
		rslt = GEO_distance_to_geo(lat2, lon2, left_width, angle, &tlat, &tlon);
		plat[2] = tlat;
		plon[2] = tlon;
		rslt =  NDTD_get_max_elev_in_poly(dted_cache1, dted_cache2, 4, plat, plon, &max_elev, &max_elev_lat, &max_elev_lon, types_used);

      if (smpVvodAnalysis != NULL && rslt == SUCCESS)
         GetVVODMaxElevInPoly(smpVvodAnalysis, 4, plat, plon, &max_elev, &max_elev_lat, &max_elev_lon);

		if ((rslt == SUCCESS) && (max_elev > *seg_max_elev))
		{
			*seg_max_elev = max_elev;
			*seg_max_elev_lat = max_elev_lat;
			*seg_max_elev_lon = max_elev_lon;
		}
//// degug only
//for (j=0; j<3; j++)
//futil->draw_geo_line(map, &dc, plat[j], plon[j], plat[j+1], plon[j+1],
//							UTIL_COLOR_RED, PS_SOLID, 2,
//							UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
//futil->draw_geo_line(map, &dc, plat[3], plon[3], plat[0], plon[0],
//						UTIL_COLOR_RED, PS_SOLID, 2,
//						UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);

		plat[1] = plat[2];
		plon[1] = plon[2];
	}

	rslt = GEO_distance_to_geo(lat2, lon2, right_width, angle, &tlat, &tlon);
	plat[1] = tlat;
	plon[1] = tlon;

	// check the second half of the end
	for (k=0; k<9; k++)
	{
		angle += 10.0;
		if (angle > 360.0)
			angle -= 360.0;
		rslt = GEO_distance_to_geo(lat2, lon2, right_width, angle, &tlat, &tlon);
		plat[2] = tlat;
		plon[2] = tlon;
		rslt =  NDTD_get_max_elev_in_poly(dted_cache1, dted_cache2, 4, plat, plon, &max_elev, &max_elev_lat, &max_elev_lon, types_used);

      if (smpVvodAnalysis != NULL && rslt == SUCCESS)
         GetVVODMaxElevInPoly(smpVvodAnalysis, 4, plat, plon, &max_elev, &max_elev_lat, &max_elev_lon);

		if ((rslt == SUCCESS) && (max_elev > *seg_max_elev))
		{
			*seg_max_elev = max_elev;
			*seg_max_elev_lat = max_elev_lat;
			*seg_max_elev_lon = max_elev_lon;
		}
//// degug only
//for (j=0; j<3; j++)
//futil->draw_geo_line(map, &dc, plat[j], plon[j], plat[j+1], plon[j+1],
//							UTIL_COLOR_RED, PS_SOLID, 2,
//							UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);
//futil->draw_geo_line(map, &dc, plat[3], plon[3], plat[0], plon[0],
//						UTIL_COLOR_RED, PS_SOLID, 2,
//						UTIL_LINE_TYPE_SIMPLE, TRUE, FALSE);

		plat[1] = plat[2];
		plon[1] = plon[2];
	}

	if (bad_dted)
		return FAILURE;

	return SUCCESS;
}
// end of get_max_elev_of_leg_ends

// *************************************************************
// *************************************************************

int CElevCalc::get_elevation_array(CList<CElevLeg*, CElevLeg*> &leg_list, int num_points, int *types_used)
{
	int k, max_elev, max_elev_no_vvod;
	double tdist, total_dist, tang, dist_in_leg;
	POSITION next;
	combo_dted_hit_and_miss_cache dted_cache1;
	combo_dted_hit_and_miss_cache dted_cache2;
	int rslt;
	double max_elev_lat, max_elev_lon;
	double sdist, edist;
	double tlat, tlon, tlat2, tlon2;
	double inc_dist, percent;
	BOOL first, bad_dted;
	CString units;
	CElevLeg *leg;
	int cnt, total_points, leg_types_used;
   IVvodAnalysisPtr smpVvodAnalysis;

	total_points = 0;
	total_dist = 0.0;
	percent = 0.0;

	ASSERT(num_points >= 0);
	if (num_points < 0)
		return FAILURE;

	// get the lengths
	next = leg_list.GetHeadPosition();
	while (next != NULL)
	{
		leg = leg_list.GetNext(next);
		rslt = GEO_geo_to_distance(leg->m_lat1, leg->m_lon1, leg->m_lat2, leg->m_lon2, &tdist, &tang);
		ASSERT(rslt == SUCCESS);
		if (rslt != SUCCESS)
		{
			ERR_report("Invalid coordinates in leg");
			return FAILURE;
		}
		leg->m_length = tdist;
		leg->m_bearing = tang;
		total_dist += tdist;
		if (leg->m_num_shape_points > 0)
			total_points += leg->m_num_shape_points;
		else
			total_points++;
	}


	dted_cache1.set_DTED_type(1);
	dted_cache2.set_DTED_type(2);

   // Note, this object will be created at a later time and should
   // be NULL on a 4.0 release
   smpVvodAnalysis.CreateInstance(__uuidof(VvodAnalysis));
   if (smpVvodAnalysis != NULL)
   {
      try
      {
         // Check if VVOD data is available
         if (smpVvodAnalysis->Init() != S_OK)
            smpVvodAnalysis = 0;
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("VvodAnalysis failed to initialize: (%s)", (char *)e.Description());
         ERR_report(msg);
      }
   }

	if (num_points > 0)
	{
		inc_dist = total_dist / (double) num_points;
		// sanity check
		if (inc_dist <= 0.0)
			return FAILURE;
	}

	edist = 0.0;
	sdist = 0.0;
	dist_in_leg = 0.0;
	first = TRUE;
	cnt = 0;

	next = leg_list.GetHeadPosition();
	while (next != NULL)
	{
		leg = leg_list.GetNext(next);
		bad_dted = FALSE;
		leg->m_max_elev = -99999;

		if (num_points > 0)
		{
			// calculate the elevation points
			leg->m_num_points = (int) ((leg->m_length - sdist) / inc_dist);
			if (leg->m_elev_array != NULL)
				free(leg->m_elev_array);
			if (leg->m_elev_no_vvod_array != NULL)
				free(leg->m_elev_no_vvod_array);
			leg->m_elev_array = (int*) malloc(leg->m_num_points * sizeof(int));
			leg->m_elev_no_vvod_array = (int*) malloc(leg->m_num_points * sizeof(int));
			leg->m_inc_dist = inc_dist;
			dist_in_leg = sdist;
			tlat = leg->m_lat1;
			tlon = leg->m_lon1;
         leg->m_valid_data = TRUE;
			for (k=0; k<leg->m_num_points; k++)
			{
				dist_in_leg += inc_dist;
				rslt = GEO_distance_to_geo(leg->m_lat1, leg->m_lon1, dist_in_leg, leg->m_bearing, &tlat2, &tlon2);
				ASSERT(rslt == SUCCESS);
				if (rslt != SUCCESS)
					return FAILURE;
				leg_types_used = 0;
				rslt = get_max_elev_of_leg(tlat, tlon, tlat2, tlon2,
											leg->m_left_width, leg->m_right_width,
											&dted_cache1, &dted_cache2,
											&max_elev, &max_elev_lat, &max_elev_lon, &leg_types_used);
				if (rslt == USER_ABORT)
					return rslt;
				if (rslt == SUCCESS && max_elev > -32000)
				{
					*types_used |= leg_types_used;
					leg->m_elev_array[k] = max_elev;
					leg->m_elev_no_vvod_array[k] = max_elev;
					if (max_elev > leg->m_max_elev)
					{
						leg->m_max_elev = max_elev;
						leg->m_max_types_used = leg_types_used;
						leg->m_max_elev_lat = max_elev_lat;
						leg->m_max_elev_lon = max_elev_lon;
					}
				}
				else
				{
					leg->m_elev_array[k] = -32000;
					leg->m_elev_no_vvod_array[k] = -32000;
					bad_dted = TRUE;
				}
				tlat = tlat2;
				tlon = tlon2;
				if (bad_dted)
					leg->m_valid_data = FALSE;

				cnt++;
				// send percent back to callback function
				if (m_callback != NULL)
				{
					if ((cnt % 10) == 0)
					{
						m_percent = ((double) cnt / (double) num_points) * 100.0;
						m_callback(m_percent);
					}
				}
				// pump a message if one is waiting
				MSG msg;
				if (PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE))
				{
					if ( msg.wParam == VK_ESCAPE ) 
					{
					   next = NULL;
						PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE);
					}
					else
						AfxGetThread()->PumpMessage();
				} 
//				if ( ::PeekMessage( &msg,  NULL, 0, 0, PM_NOREMOVE ) )
//				AfxGetThread()->PumpMessage();
			}
		}
		else
		{
			if (leg->m_num_shape_points > 0)
			{
				double angle1, angle2, tdist, tangle;

				// use the specified shape points
				leg->m_num_points = leg->m_num_shape_points;
				if (leg->m_elev_array != NULL)
					free(leg->m_elev_array);
				leg->m_elev_array = (int*) malloc(leg->m_num_points * sizeof(int));
				if (leg->m_elev_no_vvod_array != NULL)
					free(leg->m_elev_no_vvod_array);
				leg->m_elev_no_vvod_array = (int*) malloc(leg->m_num_points * sizeof(int));
				tlat = leg->m_lat1;
				tlon = leg->m_lon1;
				if (leg->m_num_shape_points > 0)
				{
					rslt = GEO_geo_to_distance(tlat, tlon, leg->m_shape_lat[0], leg->m_shape_lon[0], &tdist, &tangle);
					angle1 = tangle;
				}

            leg->m_valid_data = TRUE;
				for (k=0; k<leg->m_num_shape_points; k++)
				{
					tlat2 = leg->m_shape_lat[k];
					tlon2 = leg->m_shape_lon[k];
					if (leg->m_shape_angle == NULL)
						angle2 = 0;
					else
						angle2 = leg->m_shape_angle[k];
					leg_types_used = 0;
					rslt = get_max_elev_of_leg2(tlat, tlon, tlat2, tlon2, angle1, angle2,
												leg->m_left_width, leg->m_right_width,
												&dted_cache1, &dted_cache2, smpVvodAnalysis,
												&max_elev, &max_elev_no_vvod, &max_elev_lat, &max_elev_lon, &leg_types_used);
					if (rslt == USER_ABORT)
						return rslt;
					if (rslt == SUCCESS && max_elev > -32000)
					{
						*types_used |= leg_types_used;
						leg->m_elev_array[k] = max_elev;
						leg->m_elev_no_vvod_array[k] = max_elev_no_vvod;
						if (max_elev > leg->m_max_elev)
						{
							leg->m_max_elev = max_elev;
							leg->m_max_types_used = leg_types_used;
							leg->m_max_elev_lat = max_elev_lat;
							leg->m_max_elev_lon = max_elev_lon;
						}
					}
					else
					{
						leg->m_elev_array[k] = -32000;
						leg->m_elev_no_vvod_array[k] = -32000;
						bad_dted = TRUE;
					}

					tlat = tlat2;
					tlon = tlon2;
					angle1 = angle2;
					if (bad_dted)
						leg->m_valid_data = FALSE;
					cnt++;

					// send percent back to callback function
					if (m_callback != NULL)
					{
						if ((cnt % 10) == 0)
						{
							m_percent = ((double) cnt / (double) total_points) * 100.0;
							m_callback(m_percent);
						}
					}

					// pump a message if one is waiting
					MSG msg;
					if (PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE))
					{
						if ( msg.wParam == VK_ESCAPE ) 
						{
						   next = NULL;
							PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE);
						}
						else
							AfxGetThread()->PumpMessage();
					} 
//					if ( ::PeekMessage( &msg,  NULL, 0, 0, PM_NOREMOVE ) )
//						AfxGetThread()->PumpMessage();
				}
			}
			else
			{
            leg->m_valid_data = TRUE;

				// no shape points, use only the start and end points
				leg->m_elev_array = (int*) malloc(1 * sizeof(int));
				leg->m_elev_no_vvod_array = (int*) malloc(1 * sizeof(int));
				leg->m_num_points = 1;
				leg_types_used = 0;
				rslt = get_max_elev_of_leg(leg->m_lat1, leg->m_lon1, leg->m_lat2, leg->m_lon2,
											leg->m_left_width, leg->m_right_width,
											&dted_cache1, &dted_cache2,
											&max_elev, &max_elev_lat, &max_elev_lon, &leg_types_used);
				if (rslt == SUCCESS && max_elev > -32000)
				{
					*types_used |= leg_types_used;
					leg->m_elev_array[0] = max_elev; //There is only one point [0] 
					leg->m_elev_no_vvod_array[0] = max_elev; //There is only one point [0] 
					if (max_elev > leg->m_max_elev)
					{
						leg->m_max_elev = max_elev;
						leg->m_max_types_used = *types_used;
						leg->m_max_elev_lat = max_elev_lat;
						leg->m_max_elev_lon = max_elev_lon;
					}
				}
				else
				{
					leg->m_elev_array[0] = -32000;
					leg->m_elev_no_vvod_array[0] = -32000;
					bad_dted = TRUE;
				}

				if (bad_dted)
					leg->m_valid_data = FALSE;
				cnt++;

				// send percent back to callback function
				if (m_callback != NULL)
				{
					m_percent = ((double) cnt / (double) total_points) * 100.0;
					m_callback(m_percent);
				}

				// pump a message if one is waiting
				MSG msg;
				if (PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE))
				{
					if ( msg.wParam == VK_ESCAPE ) 
					{
					   next = NULL;
						PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE);
					}
					else
						AfxGetThread()->PumpMessage();
				} 
//					if ( ::PeekMessage( &msg,  NULL, 0, 0, PM_NOREMOVE ) )
//						AfxGetThread()->PumpMessage();
			}
		}

		// check the ends for max elev
		leg_types_used = 0;
		rslt = get_max_elev_of_leg_ends(leg->m_lat1, leg->m_lon1, leg->m_lat2, leg->m_lon2,
										leg->m_left_width, leg->m_right_width, // in meters
										&dted_cache1, &dted_cache2, smpVvodAnalysis,
										&max_elev, &max_elev_lat, &max_elev_lon, &leg_types_used); // OUTPUT in feet
		if (rslt == SUCCESS)
		{
			if (max_elev > leg->m_max_elev)
			{
				*types_used |= leg_types_used;
				leg->m_max_elev = max_elev;
				leg->m_max_types_used = leg_types_used;
				leg->m_max_elev_lat = max_elev_lat;
				leg->m_max_elev_lon = max_elev_lon;
			}
		}
		else
      {
         leg->m_valid_data = FALSE;
			bad_dted = TRUE;
      }

		if (num_points > 0)
		{
			// calc the start in the next segment
			sdist = inc_dist - (leg->m_length - dist_in_leg);
			ASSERT(sdist < inc_dist);  // sanity check
			ASSERT (sdist >= 0);
		}
	}

	return SUCCESS;
}
// end of get_elevation_array

// *************************************************************
// *************************************************************

int CElevCalc::get_elev_in_feet(double lat, double lon, 
								combo_dted_hit_and_miss_cache &dted_cache1,
								combo_dted_hit_and_miss_cache &dted_cache2,
								combo_dted_hit_and_miss_cache &dted_cache3,
								BOOL *dted2, BOOL *dted3, int *elev )
{
	BOOL found;
	int telev, rslt;

	*dted2 = FALSE;
	*dted3 = FALSE;
	rslt = dted_cache3.get_elev_in_feet(lat, lon, &found, &telev);
	if (!found || (rslt != SUCCESS) || (telev < -12000))
	{
		rslt = dted_cache2.get_elev_in_feet(lat, lon, &found, &telev);
		if (!found || (rslt != SUCCESS) || (telev < -12000))
			rslt = dted_cache1.get_elev_in_feet(lat, lon, &found, &telev);
		else
			*dted2 = TRUE;
	}
	else
		*dted3 = TRUE;
	if (found && (rslt == SUCCESS) && (telev > -12000))
	{
		*elev = telev;
		return SUCCESS;
	}

	return FAILURE;
}	

// *************************************************************
// *************************************************************

int CElevCalc::get_path_distance(double lat1, double lon1, double lat2, double lon2, 
								combo_dted_hit_and_miss_cache &dted_cache1,
								combo_dted_hit_and_miss_cache &dted_cache2,
								combo_dted_hit_and_miss_cache &dted_cache3,
								double *dist_2d,  // total flat surface distance
								double *dist_3d,	// total distance including elevation changes
								int *types_used)
{
	double tdist, tang, slope_dist, sdist, total_dist;
	double tlat, tlon, a, c, dted1_post, dted2_post, dted3_post, dted_post;
	double dted_post2;
	BOOL dted2, old_dted2, using_dted2;
	BOOL dted3, old_dted3, using_dted3;
	int rslt;
	int elev1, elev2;

	dted1_post = 100.0;
	dted2_post = 30.0;
	dted3_post = 40.0 * 0.3048;  // meters

	rslt = GEO_geo_to_distance(lat1, lon1, lat2, lon2, &tdist, &tang);
	if (rslt != SUCCESS)
		return FAILURE;

	*dist_2d = tdist;

	// get the elev of the first point
	rslt = get_elev_in_feet(lat1, lon1, dted_cache1, dted_cache2, dted_cache3, &old_dted2, &old_dted3, &elev1);
	if (rslt != SUCCESS)
		return FAILURE;

	// get the elev of the second point
	rslt = get_elev_in_feet(lat2, lon2, dted_cache1, dted_cache2, dted_cache3, &dted2, &dted3, &elev2);
	if (rslt != SUCCESS)
		return FAILURE;



	if (dted3 && old_dted3)
	{
		if (tdist < dted3_post)
		{
			a = (elev2 - elev1);
			a = FEET_TO_METERS(a);
			c = (a * a) + (tdist * tdist);
			slope_dist = sqrt(c);
			*dist_3d = slope_dist;
			return SUCCESS;
		}
	}
	else if (dted2 && old_dted2)
	{
		if (tdist < dted2_post)
		{
			a = (elev2 - elev1);
			a = FEET_TO_METERS(a);
			c = (a * a) + (tdist * tdist);
			slope_dist = sqrt(c);
			*dist_3d = slope_dist;
			return SUCCESS;
		}
	}
	else
	{
		if (tdist < dted1_post)
		{
			a = (elev2 - elev1);
			a = FEET_TO_METERS(a);
			c = (a * a) + (tdist * tdist);
			slope_dist = sqrt(c);
			*dist_3d = slope_dist;
			return SUCCESS;
		}
	}

	using_dted2 = FALSE;
	using_dted3 = FALSE;
	// we must use figure the slope distance using grid spacing
	if (dted3 && old_dted3 && (m_max_dted_level == 3))
	{
		dted_post = dted3_post;
		using_dted3 = TRUE;
	}
	else if (dted2 && old_dted2  && (m_max_dted_level >= 2))
	{
		dted_post = dted2_post;
		using_dted2 = TRUE;
	}
	else
	{
		dted_post = dted1_post;
	}

	dted_post2 = dted_post * dted_post;

	total_dist = 0.0;
	sdist = dted_post;
	while (sdist < tdist)
	{
		rslt = GEO_distance_to_geo(lat1, lon1, sdist, tang, &tlat, &tlon);
		if (rslt != SUCCESS)
			return FAILURE;
		rslt = get_elev_in_feet(tlat, tlon, dted_cache1, dted_cache2, dted_cache3, &dted2, &dted3, &elev2);
		if (rslt != SUCCESS)
			return FAILURE;

		a = (elev2 - elev1);
		a = FEET_TO_METERS(a);
		c = (a * a) + dted_post2;
		slope_dist = sqrt(c);
		total_dist += slope_dist;
		if (dted2 && old_dted2)
		{
			dted_post = dted3_post;
			dted_post2 = dted_post * dted_post;
		}
		else if (dted2 && old_dted2)
		{
			dted_post = dted2_post;
			dted_post2 = dted_post * dted_post;
		}
		else
			dted_post = dted1_post;
		elev2 = elev1;
		old_dted2 = dted2;
		old_dted3 = dted3;
		sdist += dted_post;
	}
		
	// get the last little segment
	rslt = GEO_geo_to_distance(tlat, tlon, lat2, lon2, &tdist, &tang);
	if (rslt != SUCCESS)
		return FAILURE;

	rslt = get_elev_in_feet(lat2, lon2, dted_cache1, dted_cache2, dted_cache3, &dted2, &dted3, &elev2);
	if (rslt != SUCCESS)
		return FAILURE;

	a = (elev2 - elev1);
	a = FEET_TO_METERS(a);
	c = (a * a) + (tdist * tdist);
	slope_dist = sqrt(c);
	total_dist += slope_dist;

	*dist_3d = total_dist;
	// there could be slight differences between the methods of calculating distence
	// so don't let the 3d be smaller than the 2d
	if (*dist_3d < *dist_2d)
		*dist_3d = *dist_2d;

	return SUCCESS;
}
// end of get_path_distance

// *************************************************************
// *************************************************************

int CElevCalc::get_path_distance(double *lat, double *lon, int num_points, 
								  double *dist_2d,  // total flat surface distance
								  double *dist_3d,	// total distance along 3D path
								  int *types_used)
{
	int k, rslt;
	combo_dted_hit_and_miss_cache dted_cache1;
	combo_dted_hit_and_miss_cache dted_cache2;
	combo_dted_hit_and_miss_cache dted_cache3;
	double tdist_2d, tdist_3d, total_dist_2d, total_dist_3d;

	if (num_points < 2)
		return FAILURE;

	dted_cache1.set_DTED_type(1);
	dted_cache2.set_DTED_type(2);
	dted_cache3.set_DTED_type(3);

	total_dist_2d = 0.0;
	total_dist_3d = 0.0;

	for (k=0; k<num_points-1; k++)
	{
		rslt = get_path_distance(lat[k], lon[k], lat[k+1], lon[k+1], dted_cache1, dted_cache2, dted_cache3,
							&tdist_2d, &tdist_3d, types_used);
		if (rslt != SUCCESS)
			return FAILURE;

		total_dist_2d += tdist_2d;
		total_dist_3d += tdist_3d;
	}

	*dist_2d = total_dist_2d;
	*dist_3d = total_dist_3d;

	return SUCCESS;
}
// end of get_path_distance

// *************************************************************
// *************************************************************

void CElevCalc::set_max_dted_level(int max_dted_level)
{
	if ((max_dted_level >= 1) && (max_dted_level <= 3))
		m_max_dted_level = max_dted_level;
}

// *************************************************************
// *************************************************************
