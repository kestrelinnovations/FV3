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

// ar_file.cpp

#include "stdafx.h"
#include "err.h"
#include "param.h"
#include "ar_edit.h"
#include "geo_tool.h"
#include "fvwutil.h"
#include "SnapTo.h"
#include "ado.h"

static int segnum;

int compare_ar_points(const void *pt1, const void *pt2);

// ***********************************************************************

int C_ar_edit::load(const CString & filename)
{
	CAdoSet* set;
	CAdoSet* set_pt;
	CAdoSet* set_rmk;
	C_ar_obj* obj;
	C_ar_point *pt, *tpt;
	CString tstr, oldid, tablename;
	POSITION next, next_pt;
	int cnt;
	double dist, ang, magvar;
	CString msg("");
//	double tlat, tlon;
	BOOL found;
	int rslt;



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// compute the local magnetic variation
	//////////////////////////////////////////////////////////////////////////////////////////////////////
//	int year, month, alt, rslt;
//	double magvar;
//	SYSTEMTIME time;
//	GetSystemTime(&time);
//	year = time.wYear;
//	month = time.wMonth;
//	alt = 0;

	tstr = filename.Right(9);
	if (!tstr.CompareNoCase("DAFIF.MDB"))
	{
		AfxMessageBox("The Track/Orbit Editor cannot open the DAFIF database");
		return FAILURE;
	}
		
//#	C_ar_dao_route::set_filename(filename);
//#	set = (C_ar_dao_route*) new C_ar_dao_route(&db);

	set = new CAdoSet;

	if (set == NULL)
	{
		ERR_report("Track/Orbit Editor Access Open Error");
		return FALSE;
	}

	try
	{
		// set->Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbForwardOnly);

//#		if (m_read_only)  // if this db is read-only...
//#			db.Open(filename, FALSE, TRUE);  // open the db for read-only access
//#		else
//#			db.Open(filename, FALSE, FALSE);  // open the db for read/write access
		
		tablename = "REFUEL_TRK";

		rslt = set->Open(filename, tablename);	// open the REFUEL_TRK database table

		if (rslt != SUCCESS)
		{
			ERR_report("Track/Orbit Editor Database Open Error");
			return FAILURE;
		}

		while (!set->IsEOF())
		{
			obj = new C_ar_obj(this);

			obj->m_icao = set->get_string_value("ICAO_CODE");
			obj->m_id = set->get_string_value("ID");
			obj->m_direction = set->get_string_value("DIRECTION");

			tstr = set->get_string_value("TYPE");
			tstr += " ";
			obj->m_type = tstr[0];
			
			obj->m_country = set->get_string_value("CTRY_CODE");
			obj->m_local_datum = set->get_string_value("LOCAL_DATUM");
			obj->m_geodetic_datum = set->get_string_value("GEODETIC_DATUM");
			obj->m_prfreq = set->get_string_value("PRIMARY_UHF_FREQ");
			obj->m_bufreq = set->get_string_value("BACKUP_UHF_FREQ");
			obj->m_apnset = set->get_string_value("APN_69_134_135");
			obj->m_apxcode = set->get_string_value("APX_78_CODEC");
			obj->m_tac_chan = set->get_string_value("TACAN_CHANNEL");
			obj->m_altdesc1 = set->get_string_value("ALT_1_DESC");
			obj->m_fuel1alt1 = set->get_string_value("REFUEL_1_ALT_1");
			obj->m_fuel1alt2 = set->get_string_value("REFUEL_1_ALT_2");
			obj->m_altdesc2 = set->get_string_value("ALT_2_DESC");
			obj->m_fuel2alt1 = set->get_string_value("REFUEL_2_ALT_1");
			obj->m_fuel2alt2 = set->get_string_value("REFUEL_2_ALT_2");
			obj->m_altdesc3 = set->get_string_value("ALT_3_DESC");
			obj->m_fuel3alt1 = set->get_string_value("REFUEL_3_ALT_1");
			obj->m_fuel3alt2 = set->get_string_value("REFUEL_3_ALT_2");
			obj->m_unit = set->get_string_value("SCHEDULING_UNIT");
			obj->m_atc = set->get_string_value("ASSIGNED_ATC");
			obj->m_cycledate = set->get_string_value("CYCLE_DATE");
						
			m_obj_list.AddTail(obj);	

			set->MoveNext();
		}
		set->Close();	// close the database
		delete set;		// delete the set variable
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Track/Orbit Editor Database Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FAILURE;
	}
	catch(...)
	{
		msg = "Track/Orbit Editor Database Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FAILURE;
	}

	set_modified(FALSE);
	m_filename = filename;



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// get the turn points
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	cnt = 0;
	oldid = "";

	set_pt = new CAdoSet;

	if (set_pt == NULL)
	{
		ERR_report("Track/Orbit Editor Access Open Error");
		return FALSE;
	}

	try
	{
		tablename = "REFUEL_TRK_PT";

		rslt = set_pt->Open(filename, tablename);	// open the REFUEL_TRK_PT database table

		if (rslt != SUCCESS)
		{
			ERR_report("Track/Orbit Editor Database Open Error");
			return FAILURE;
		}

		while (!set_pt->IsEOF())
		{
			pt = new C_ar_point(NULL);
			
			pt->m_icao = set_pt->get_string_value("ICAO_CODE");
			pt->m_id = set_pt->get_string_value("ID");
			pt->m_direction = set_pt->get_string_value("DIRECTION");
			pt->m_seq_num = set_pt->get_string_value("SEQ_NUM");
			pt->m_usage_string = set_pt->get_string_value("USAGE");
			pt->add_usage(pt->m_usage_string);
			pt->m_nav_flag = set_pt->get_string_value("NAV_FLAG");
			pt->m_nav_id= set_pt->get_string_value("NAV_ID");
			pt->m_nav_type = set_pt->get_string_value("NAV_TYPE");
			pt->m_nav_country = set_pt->get_string_value("NAV_CTRY_CODE");
			pt->m_nav_key = set_pt->get_string_value("NAV_KEY_CODE");
			pt->m_fix = set_pt->get_string_value("FIX");
			pt->m_lat = set_pt->get_double_value("GEODETIC_LAT");
			pt->m_lon = set_pt->get_double_value("GEODETIC_LONG");

			if (!pt->m_usage_string.Compare("AN"))
				pt->m_anchor_point = TRUE;
			pt->calc_nav_geo();

			magvar = 0;

			// compute nav lat/long
			if (pt->m_fix.GetLength() > 5)
			{
				SnapToInfo db_info;
            CFvwUtil* pFvwUtil = CFvwUtil::get_instance();

				if (pFvwUtil->db_lookup(pt->m_nav_id, db_info, TRUE))
				{
					pt->m_nav_lat = db_info.m_lat;
					pt->m_nav_lon = db_info.m_lon;
				}
			}

			ASSERT(pt->m_usage.GetCount() > 0); // a point must have at least one usage string

			// put the point in the proper route
			next = m_obj_list.GetHeadPosition();
			while (next != NULL)
			{
				obj = m_obj_list.GetNext(next);
				if (!obj->m_id.Compare(pt->m_id))
				{
					pt->m_obj = obj;
					if (oldid.Compare(obj->m_id))
						cnt = 0;
					pt->m_type = obj->m_type;
					if (!pt->m_usage_string.Compare("AN"))
					{
						obj->m_anchor_pt_lat = pt->m_lat;
						obj->m_anchor_pt_lon = pt->m_lon;
						pt->add_usage("AP");
					}
					if ((obj->m_type == 'A') && (!pt->m_usage_string.Compare("AP")))
					{
						if (cnt < 4)
						{
							obj->m_anchor_point[cnt] = *pt;
							oldid = pt->m_id;
							if (cnt == 0)
							{
								obj->m_lat = pt->m_lat;
								obj->m_lon = pt->m_lon;
							}
							delete pt;
							pt = NULL;
						}
						cnt++;
					}
					else
					{
						// check for point already in list
						found = FALSE;
						next_pt = obj->m_point_list.GetHeadPosition();
						while (next_pt != NULL)
						{
							tpt = obj->m_point_list.GetNext(next_pt);
							if ((pt->m_lat == tpt->m_lat) && (pt->m_lon == tpt->m_lon))
							{
								found = TRUE;
								tpt->add_usage(pt->m_usage_string);
								next = NULL;
							}
						}
						if (!found)
						{
							obj->m_point_list.AddTail(pt);
						}
						else
						{
							oldid = pt->m_id;
							delete pt;
							pt = NULL;
						}
					}
					next = NULL;
				}
			}

			if (pt != NULL)
				oldid = pt->m_id;
			
			set_pt->MoveNext();
		}
		set_pt->Close();	// close the database
		delete set_pt;		// delete the set_pt variable
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Track/Orbit Editor Database Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FAILURE;
	}
	catch(...)
	{
		msg = "Track/Orbit Editor Database Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FAILURE;
	}




	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// fix up the anchor routes
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	double ang2;

	next = m_obj_list.GetHeadPosition();
	while (next != NULL)
	{
		obj = m_obj_list.GetNext(next);
		if (obj->m_type == 'A')
		{
			int a0num, a1num, a2num, a3num, k;
			double neardist, fardist;
			C_ar_point tpt[4];

			// find the anchor pt
			a0num = -1;
			a1num = -1;
			a2num = -1;
			a3num = -1;
			for (k=0; k<4; k++)
			{
				if ((obj->m_anchor_point[k].m_lat == obj->m_anchor_pt_lat) &&
					(obj->m_anchor_point[k].m_lon == obj->m_anchor_pt_lon))
					a1num = k;
			}

			ASSERT(a1num >= 0);

			if (a1num >= 0)
			{
				// find the nearest and farthest point
				neardist = 999999999.0;
				fardist = 0.0;
				for (k=0; k<4; k++)
				{
					if (k != a1num)
					{
						GEO_geo_to_distance(obj->m_anchor_point[a1num].m_lat, obj->m_anchor_point[a1num].m_lon, 
											obj->m_anchor_point[k].m_lat, obj->m_anchor_point[k].m_lon, &dist, &ang);
						if (dist < neardist)
						{
							neardist = dist;
							a2num = k;
						}
						if (dist > fardist)
						{
							fardist = dist;
							a3num = k;
						}
					}
				}

				// find the last point
				for (k=0; k<4; k++)
				{
					if ((k != a1num) && (k != a2num) && (k != a3num))
						a0num = k;
				}

				ASSERT(a0num >= 0);
				ASSERT(a2num >= 0);
				ASSERT(a3num >= 0);
				if ((a0num >= 0 ) && (a2num >= 0) && (a3num >= 0))
				{
					// store the points
					for (k=0; k<4; k++)
						tpt[k] = obj->m_anchor_point[k];

					// reassign the points
					obj->m_anchor_point[0] = tpt[a0num];
					obj->m_anchor_point[1] = tpt[a1num];
					obj->m_anchor_point[2] = tpt[a2num];
					obj->m_anchor_point[3] = tpt[a3num];

					obj->m_anchor_point[1].m_anchor_point = TRUE;

					rslt = GEO_current_magnetic_variation(obj->m_anchor_pt_lat, obj->m_anchor_pt_lon, 0, &magvar);
					if (rslt != SUCCESS)
						magvar = 0.0;

					obj->m_lat = obj->m_anchor_point[1].m_lat;			
					obj->m_lon = obj->m_anchor_point[1].m_lon;			
					GEO_geo_to_distance(obj->m_anchor_point[0].m_lat, obj->m_anchor_point[0].m_lon, 
										obj->m_lat, obj->m_lon, 
										&dist, &ang);
					obj->m_anchor_length = dist / 1852.0;
					obj->m_angle = ang - magvar;
					GEO_geo_to_distance(obj->m_lat, obj->m_lon, obj->m_anchor_point[2].m_lat,
										obj->m_anchor_point[2].m_lon, &dist, &ang2);
					obj->m_anchor_width = dist / 1852.0;
					obj->m_anchor_left = TRUE;
					// fix up the points
					next_pt = obj->m_point_list.GetHeadPosition();
					while (next_pt != NULL)
					{
						pt = obj->m_point_list.GetNext(next_pt);
						pt->calc_dist_angle();
					}
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// get the remarks
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	set_rmk = new CAdoSet;

	if (set_rmk == NULL)
	{
		ERR_report("Track/Orbit Editor Access Open Error");
		return FALSE;
	}

	try
	{
		tablename = "REFUEL_TRK_RMK";

		rslt = set_rmk->Open(filename, tablename);	// open the REFUEL_TRK_RMK database table

		if (rslt != SUCCESS)
		{
			ERR_report("Track/Orbit Editor Database Open Error");
			return FAILURE;
		}

		while (!set_rmk->IsEOF())
		{
			if (set_rmk->get_string_value("REMARK_1").GetLength() > 0)
			{
				tstr = set_rmk->get_string_value("REMARK_1");

				// put the point in the proper route
				next = m_obj_list.GetHeadPosition();
				while (next != NULL)
				{
					obj = m_obj_list.GetNext(next);
					if (!obj->m_id.Compare(set_rmk->get_string_value("ID")))
					{
						obj->m_remarks += tstr;
						next = NULL;
					}
				}
			}

			if (set_rmk->get_string_value("REMARK_2").GetLength() > 0)
			{
				tstr = set_rmk->get_string_value("REMARK_2");

				// put the point in the proper route
				next = m_obj_list.GetHeadPosition();
				while (next != NULL)
				{
					obj = m_obj_list.GetNext(next);
					if (!obj->m_id.Compare(set_rmk->get_string_value("ID")))
					{
						obj->m_remarks += tstr;
						next = NULL;
					}
				}
			}

			if (set_rmk->get_string_value("REMARK_3").GetLength() > 0)
			{
				tstr = set_rmk->get_string_value("REMARK_3");

				// put the point in the proper route
				next = m_obj_list.GetHeadPosition();
				while (next != NULL)
				{
					obj = m_obj_list.GetNext(next);
					if (!obj->m_id.Compare(set_rmk->get_string_value("ID")))
					{
						obj->m_remarks += tstr;
						next = NULL;
					}
				}
			}

			set_rmk->MoveNext();
		}
		set_rmk->Close();	// close the database
		delete set_rmk;		// delete the set_rmk variable
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Track/Orbit Editor Database Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FAILURE;
	}
	catch(...)
	{
		msg = "Track/Orbit Editor Database Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FAILURE;
	}

	set_modified(FALSE);
	m_filename = filename;

	// set drag lock on whenever a new file is loaded
	m_drag_lock = TRUE;
	m_drag_lock_checked = FALSE;

	// initialize the saved flag to be TRUE (this means the file has been saved
	// with this name before)
	put_m_bHasBeenSaved(TRUE);

	return SUCCESS;
}
// end of open



// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::create_db(CAdoSet **set, CString filepath, long nSaveFormat)
{
	char filename[MAX_PATH + 1];
	BOOL rslt;
	CFvwUtil *futil = CFvwUtil::get_instance();

	*set = NULL;

	// get HD_DATA data path...
	CString str = PRM_get_registry_string("Main", "HD_DATA");

   enum { 
      eSaveFormatUnspecified = 0, 
      eSaveFormatTrackOrbit = 1,
      eSaveFormatUserArTool = 2,
      eSaveFormatTrackOrbit_3_3_1 = 3
   };

	// if the "3.3.1 file" type was selected, use the old Access97 Track/Orbit template file...
   if (nSaveFormat == eSaveFormatTrackOrbit_3_3_1)
		str += "\\ar_user\\arblank.mdb";
	else // otherwise, use the new AccessXP Track/Orbit template file...
		str += "\\ar_user\\arblankXP.mdb";
		
	strncpy_s(filename, MAX_PATH + 1, filepath, MAX_PATH);

	// delete the file
	DeleteFile(filename);

	// copy the air refueling template file to the new file
   rslt = ::CopyFile(str, filename, FALSE);
	if (!rslt)
	{
		AfxMessageBox("Error copying template file: ARBLANK.MDB to ar_edit directory");
		return FALSE;
	}

	// make sure the newly-created copy of the air refueling template file is NOT read-only...
	SetFileAttributes(filename, FILE_ATTRIBUTE_NORMAL);

	// reset the read-only flag...
	m_bIsReadOnly = 0;


	*set = new CAdoSet;

	if (*set == NULL)
	{
		ERR_report("Track/Orbit Editor Access Open Error");
		return FALSE;
	}

	return TRUE;
}
// end of create_db


	
// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::update(CAdoSet *set)
{
	CString msg("");

	try
	{
		set->Update();
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Route Update Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FALSE;
	}
	catch(...)
	{
		msg = "Route Update Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FALSE;
	}

	return TRUE;
}
// end of update



// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::update_point(CAdoSet *set)
{
	CString msg("");

	try
	{
		set->Update();
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Route Update Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FALSE;
	}
	catch(...)
	{
		msg = "Route Update Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FALSE;
	}

	return TRUE;
}
// end of update_point



// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::update_remark(CAdoSet *set)
{
	CString msg("");

	try
	{
		set->Update();
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Route Update Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FALSE;
	}
	catch(...)
	{
		msg = "Route Update Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FALSE;
	}

	return TRUE;
}
// end of update_remark



// ***********************************************************************************************************
// ***********************************************************************************************************

int C_ar_edit::save_to_file(const CString & filename, long nSaveFormat)
{
	POSITION next;
	C_ar_obj *obj;
	CAdoSet* set;
	CAdoSet* set_pt;
	CAdoSet* set_rmk;
	BOOL rslt;
	int result;
	CString msg("");
	CString tablename;

	// init the counter
	m_cnt = 0;

	// create the database
	rslt = create_db(&set, filename, nSaveFormat);
	if (!rslt)
	{
		CString msg;
		msg.Format("The file -- %s -- could not be created", filename);
		AfxMessageBox(msg);
		return FAILURE;
	}

	// save the route info //////////////////////////////////////////////////////
	set = new CAdoSet;

	if (set == NULL)
	{
		ERR_report("Track/Orbit Editor Access Open Error");
		return FALSE;
	}

	try
	{
		tablename = "REFUEL_TRK";
		
		result = set->Open(filename, tablename);	// open the REFUEL_TRK database table

		if (result != SUCCESS)
		{
			ERR_report("Track/Orbit Editor Database Open Error");
			return FAILURE;
		}
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Track/Orbit Editor Database Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FAILURE;
	}
	catch(...)
	{
		msg = "Track/Orbit Editor Database Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FAILURE;
	}

	// add all the records
	if (!m_obj_list.IsEmpty())
	{
		next = m_obj_list.GetHeadPosition();
		do
		{
			obj = m_obj_list.GetNext(next);
			obj->m_direction = " ";
			if (obj->m_id.Compare("Untitled"))
			{
				save_route(set, obj);
				m_cnt++;
			}
		} while (next != NULL);   
	}

	try
	{
		// close database and delete pointer
		set->Close();
		delete set;


		// save the route points ////////////////////////////////////////////////////
		set_pt = new CAdoSet;

		if (set_pt == NULL)
		{
			ERR_report("Track/Orbit Editor Access Open Error");
			return FALSE;
		}

		tablename = "REFUEL_TRK_PT";

		result = set_pt->Open(filename, tablename);	// open the REFUEL_TRK_PT database table

		if (result != SUCCESS)
		{
			ERR_report("Track/Orbit Editor Database Open Error");
			return FAILURE;
		}

		// add all the records
		if (!m_obj_list.IsEmpty())
		{
			next = m_obj_list.GetHeadPosition();
			do
			{
				obj = m_obj_list.GetNext(next);
				obj->m_direction = " ";
				save_route_points(set_pt, obj);
				m_cnt++;
			} while (next != NULL);   
		}
		set_pt->Close();
		delete set_pt;
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Track/Orbit Editor Database Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FAILURE;
	}
	catch(...)
	{
		msg = "Track/Orbit Editor Database Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FAILURE;
	}



	// save the route remarks ///////////////////////////////////////////////////
	set_rmk = new CAdoSet;

	if (set_rmk == NULL)
	{
		ERR_report("Track/Orbit Editor Access Open Error");
		return FALSE;
	}

	try
	{
		tablename = "REFUEL_TRK_RMK";

		result = set_rmk->Open(filename, tablename);	// open the REFUEL_TRK_RMK database table

		if (result != SUCCESS)
		{
			ERR_report("Track/Orbit Editor Database Open Error");
			return FAILURE;
		}

		// add all the records
		if (!m_obj_list.IsEmpty())
		{
			next = m_obj_list.GetHeadPosition();
			do
			{
				obj = m_obj_list.GetNext(next);
				obj->m_direction = " ";
				save_remarks(set_rmk, obj);
				m_cnt++;
			} while (next != NULL);   
		}
		set_rmk->Close();
		delete set_rmk;
	}
	catch (_com_error &e)
	{
		_bstr_t bstrDescription ((char*) e.Description());
		msg.Format("Track/Orbit Editor Database Error: %s", (char*) bstrDescription);
		ERR_report(msg);
		AfxMessageBox(msg);
		return FAILURE;
	}
	catch(...)
	{
		msg = "Track/Orbit Editor Database Error";
		ERR_report(msg);
		AfxMessageBox(msg);
		TRACE ("*** Unhandled Exception ***");
		return FAILURE;
	}


	set_modified(FALSE);
	return SUCCESS;
}
// end of save_to_file



// ***********************************************************************************************************
// ***********************************************************************************************************

/*
BOOL C_ar_edit::save_record(CAdoSet *set, int code, CString tstr)
{
	set->AddNew();
	set->m_ItemNum = m_cnt;
	set->m_DataType = code;
	set->m_Data = tstr.Left(23);
	return update(set);
}
// end of save_record



// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::save_geo(CAdoSet *set, int code, double lat, double lon)
{
	char slat[12];
	char slon[13];
	CString tstr;

	util->geo2text(lat, lon, slat, slon);		
	tstr = slat;
	tstr += slon;
//	tstr += "      ";
	return save_record(set, code, tstr);
}
// end of save_record
*/



// ***********************************************************************************************************
// ***********************************************************************************************************

void C_ar_edit::get_date(CString & date)
{
	int year, month;
	SYSTEMTIME time;

	GetSystemTime(&time);
	year = time.wYear;
	month = time.wMonth;
	date.Format("%02d%02d", year % 100, month);
}
// end of get_date



// ***********************************************************************************************************
// ***********************************************************************************************************

CString C_ar_edit::get_new_segnum()
{
	CString numstr;

	segnum += 10;
	numstr.Format("%d", segnum);
	return numstr;
}
// end of get_new_segnum



// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::save_route(CAdoSet *set, C_ar_obj *obj)
{
	try
	{
		CString type_string(obj->m_type);  // construct a CString for use in writing the TYPE to the db
		CString date;

		get_date(date);


		set->AddNew();

		set->set_string_value("ICAO_CODE", obj->m_icao.Left(4));
		set->set_string_value("ID", obj->m_id.Left(15));
		set->set_string_value("DIRECTION", obj->m_direction.Left(2));
		set->set_string_value("TYPE", type_string);
		set->set_string_value("CTRY_CODE", obj->m_country.Left(2));
		set->set_string_value("LOCAL_DATUM", "WGE");
		set->set_string_value("GEODETIC_DATUM", "WGE");
		set->set_string_value("PRIMARY_UHF_FREQ", obj->m_prfreq.Left(6));
		set->set_string_value("BACKUP_UHF_FREQ", obj->m_bufreq.Left(6));
		set->set_string_value("APN_69_134_135", obj->m_apnset.Left(3));
		set->set_string_value("APX_78_CODEC", obj->m_apxcode.Left(2));
	//	obj->m_rcvr_chan += "  ";
	//	set->set_string_value("TACAN_CHANNEL", obj->m_rcvr_chan.Left(2));
	//	obj->m_tank_chan += "   ";
	//	set->set_string_value("TACAN_CHANNEL", obj->m_tank_chan.Left(3));
		set->set_string_value("TACAN_CHANNEL", obj->m_tac_chan);
		set->set_string_value("ALT_1_DESC", obj->m_altdesc1.Left(2));
		set->set_string_value("REFUEL_1_ALT_1", obj->m_fuel1alt1.Left(5));
		set->set_string_value("REFUEL_1_ALT_2", obj->m_fuel1alt2.Left(5));
		set->set_string_value("ALT_2_DESC", obj->m_altdesc2.Left(2));
		set->set_string_value("REFUEL_2_ALT_1", obj->m_fuel2alt1.Left(5));
		set->set_string_value("REFUEL_2_ALT_2", obj->m_fuel2alt2.Left(5));
		set->set_string_value("ALT_3_DESC", obj->m_altdesc3.Left(2));
		set->set_string_value("REFUEL_3_ALT_1", obj->m_fuel3alt1.Left(5));
		set->set_string_value("REFUEL_3_ALT_2", obj->m_fuel3alt2.Left(5));
		set->set_string_value("SCHEDULING_UNIT", obj->m_unit.Left(80));
		set->set_string_value("ASSIGNED_ATC", obj->m_atc.Left(80));
		set->set_string_value("CYCLE_DATE", date.Left(4));

		return update(set);
	}
	catch (CDaoException *e)
	{
		e->Delete();
		ERR_report("CDaoException saving route");
		return FALSE;
	}
	catch(...)
	{
		return FALSE;
	}
}
// end of save_route



// ***********************************************************************************************************
// ***********************************************************************************************************

int compare_ar_points(const void *pt1, const void *pt2)
{
	C_ar_point *apt1, *apt2;

	apt1 = (C_ar_point*) pt1;
	apt2 = (C_ar_point*) pt2;

	if (apt1->m_usage_code < apt2->m_usage_code)
		return -1;
	if (apt1->m_usage_code > apt2->m_usage_code)
		return 1;
	return 0;
}
// end of compare_ar_points



// ***********************************************************************************************************
// ***********************************************************************************************************

// this function takes the point list, puts the points in an array with one usage per entry,
// sorts the entries, and recreates the point list with one usage per entry

int C_ar_edit::sort_points(CList<C_ar_point*, C_ar_point*> & point_list, BOOL really_sort)
{
	C_ar_point *temp_points;
	C_ar_point *pt;
	POSITION next, next_usage;
	int k, cnt, code;
	CString *txt, tstr;

	// count the usages
	cnt = 0;
	next = point_list.GetHeadPosition();
	while (next != NULL)
	{
		pt = point_list.GetNext(next);
ASSERT(pt->m_lat != 0.0);
		k = pt->m_usage.GetCount();
		cnt += k;
	}

	// allocate memory for the array
	temp_points = new C_ar_point[cnt];

	// put the points in an array
	k = 0;
	next = point_list.GetHeadPosition();
	while (next != NULL)
	{
		pt = point_list.GetNext(next);
ASSERT(pt->m_lat != 0.0);
		next_usage = pt->m_usage.GetHeadPosition();
		while (next_usage != NULL)
		{
			txt = pt->m_usage.GetNext(next_usage);
			temp_points[k] = *pt;
			temp_points[k].clear_usages();
			if (ar_point_usage_to_code(*txt, &code))
			{
				temp_points[k].m_usage_code = code;
				temp_points[k].m_usage_str = *txt;
				k++;
			}
			else
				delete txt;
		}
	}
	
	cnt = k;

	// sort the points
	if (really_sort)
		qsort((void*) temp_points, cnt, sizeof(C_ar_point), compare_ar_points);

	// clear the temp list
	while (!point_list.IsEmpty())
	{
		pt = point_list.RemoveHead();
		delete pt;
	}

	// reconstruct the list
	for (k=0; k<cnt; k++)
	{
		pt = new C_ar_point;
		*pt = temp_points[k];
		pt->add_usage(pt->m_usage_str);
ASSERT(pt->m_lat != 0.0);
		point_list.AddTail(pt);
	}

	delete [] temp_points;

	return SUCCESS;
}
// end of sort_points



// ***********************************************************************************************************
// ***********************************************************************************************************

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

BOOL C_ar_edit::ar_point_usage_to_code(CString usage, int *code)
{
	*code = 0;

	if (!usage.Compare("ET"))
		*code = 1;
	else if (!usage.Compare("IP"))
		*code = 2;
	else if (!usage.Compare("CP"))
		*code = 3;
	else if (!usage.Compare("NC"))
		*code = 4;
	else if (!usage.Compare("AN"))
		*code = 5;
	else if (!usage.Compare("AP"))
		*code = 6;
	else if (!usage.Compare("EX"))
		*code = 7;
	else 
		return FALSE;

	return TRUE;
}
// end of ar_point_usage_to_code


// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::save_route_points(CAdoSet *set, C_ar_obj *obj)
{
	POSITION next;
	C_ar_point *pt, *newpt;
	int num, k, cnt;
	CString numstr;
	CList<C_ar_point*, C_ar_point*> temp_point_list;
	int ptcnt;
	BOOL sort;
	CString *txt;
	ptcnt = obj->m_point_list.GetCount();

	// copy the points
	cnt = obj->m_point_list.GetCount();
	k = 0;
	next = obj->m_point_list.GetHeadPosition();
	while (next != NULL)
	{
		pt = obj->m_point_list.GetNext(next);
		txt = pt->m_usage.GetHead();
		if (txt->Compare("AN") && txt->Compare("AP"))
		{
			newpt = new C_ar_point(obj);
			*newpt = *pt;
			temp_point_list.AddTail(newpt);
		}
//		else
//			ASSERT(0);
	}

	int anpt = 1;

	if (obj->m_type == 'A')
	{
		// add the anchor points
		newpt = new C_ar_point(obj);
		*newpt = obj->m_anchor_point[1];
		newpt->m_lat = obj->m_anchor_pt_lat;
		newpt->m_lon = obj->m_anchor_pt_lon;
	ASSERT(newpt->m_lat != 0.0);
		newpt->clear_usages();
		newpt->add_usage("AN");
		temp_point_list.AddTail(newpt);
		newpt = new C_ar_point(obj);
		*newpt = obj->m_anchor_point[1];
		newpt->remove_usage("AN");
		newpt->add_usage("AP");
	ASSERT(newpt->m_lat != 0.0);
		temp_point_list.AddTail(newpt);
		newpt = new C_ar_point(obj);
		*newpt = obj->m_anchor_point[2];
		newpt->remove_usage("AN");
		newpt->add_usage("AP");
	ASSERT(newpt->m_lat != 0.0);
		temp_point_list.AddTail(newpt);
		newpt = new C_ar_point(obj);
		*newpt = obj->m_anchor_point[3];
		newpt->remove_usage("AN");
		newpt->add_usage("AP");
	ASSERT(newpt->m_lat != 0.0);
		temp_point_list.AddTail(newpt);
		newpt = new C_ar_point(obj);
		*newpt = obj->m_anchor_point[0];
		newpt->remove_usage("AN");
		newpt->add_usage("AP");
	ASSERT(newpt->m_lat != 0.0);
		temp_point_list.AddTail(newpt);
	}

	// sort the points in dafif order for orbits
	if (obj->m_type == 'A')
		sort = TRUE;
	else
		sort = FALSE;

	sort_points(temp_point_list, sort);

	segnum = 0;

	next = temp_point_list.GetHeadPosition();
	while (next != NULL)
	{
		pt = temp_point_list.GetNext(next);

		// strip the type from the nav id
		num = pt->m_nav_id.ReverseFind('/');
		if (num > 0)
			pt->m_nav_id = pt->m_nav_id.Left(num);
		if (pt->m_nav_id.GetLength() > 0)
			pt->m_nav_key = "1";
		save_point(set, obj, pt);
	}

	// clear the temp list
	while (!temp_point_list.IsEmpty())
	{
		pt = temp_point_list.RemoveHead();
		delete pt;
	}

	return TRUE;
}
// end of save_route_points



// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::save_point(CAdoSet *set, C_ar_obj *obj, C_ar_point *pt)
{
	CString date;
	CString tstr;
	BOOL rslt;
	double alat, alon;

	try
	{
		alat = obj->m_anchor_pt_lat;
		alon = obj->m_anchor_pt_lon;

		get_date(date);

	//	ASSERT(pt->m_id.GetLength() > 0);
		if (pt->m_id.GetLength() < 1)
			pt->m_id = get_new_name();
		if (pt->m_direction.GetLength() < 1)
			pt->m_direction = " ";

		// compute fix
		pt->calc_nav_fix();
	//	GEO_geo_to_distance(pt->m_nav_lat, pt->m_nav_lon, pt->m_lat, pt->m_lon, &dist, &ang);
	//	pt->m_fix.Format("%03.0f%03.0f", ang, dist / 1852);

	//	next = pt->m_usage.GetHeadPosition();
	//	ASSERT(next);
	//	while (next != NULL)
		{
	//		txt = pt->m_usage.GetNext(next);
	//		if (!txt->Compare("AN"))
			tstr = pt->m_usage_str;
			if (!tstr.Compare("AN"))
			{
				// only add the anchor point if it is actually the real anchor point
	//			if ((alat != pt->m_lat) || (alon != pt->m_lon))
	//				return FALSE;
			}
			set->AddNew();

			set->set_string_value("ICAO_CODE", pt->m_icao.Left(4));
			set->set_string_value("ID", pt->m_id.Left(15));
			set->set_string_value("DIRECTION", pt->m_direction.Left(2));
			set->set_string_value("SEQ_NUM", get_new_segnum());
	//		set->set_string_value("USAGE", *txt);
	//ASSERT(txt->GetLength() > 0);
			set->set_string_value("USAGE", tstr);
	ASSERT(tstr.GetLength() > 0);
			set->set_string_value("NAV_FLAG", pt->m_nav_flag.Left(1));
			set->set_string_value("NAV_ID", pt->m_nav_id.Left(4));
			set->set_string_value("NAV_TYPE", pt->m_nav_type.Left(1));
			set->set_string_value("NAV_CTRY_CODE", pt->m_nav_country.Left(2));
			set->set_string_value("NAV_KEY_CODE", pt->m_nav_key.Left(2));
			set->set_string_value("FIX", pt->m_fix.Left(6));
			set->set_double_value("LAT", pt->m_lat);
	ASSERT(pt->m_lat != 0.0);
			set->set_double_value("LON", pt->m_lon);
	ASSERT(pt->m_lon != 0.0);
			set->set_double_value("GEODETIC_LAT", pt->m_lat);
			set->set_double_value("GEODETIC_LONG", pt->m_lon);
			set->set_string_value("CYCLE_DATE", date);

			rslt = update_point(set);
		}
		return rslt;
	}
	catch (CDaoException *e)
	{
		e->Delete();
		ERR_report("CDaoException saving route point");
		return FAILURE;
	}
	catch(...)
	{
		return FAILURE;
	}
}
// end of save_point



// ***********************************************************************************************************
// ***********************************************************************************************************

BOOL C_ar_edit::save_remarks(CAdoSet *set, C_ar_obj *obj)
{
	CString date, seq_str, rmk, rmks;
	int num_recs, pos, k, len, rlen;
	BOOL rslt;

	try
	{
		get_date(date);
		rmks = obj->m_remarks;
		len = rmks.GetLength();
		num_recs = len / 240;
		num_recs++;
		pos = 0;

		for (k=0; k<num_recs; k++)
		{
			rlen = rmks.GetLength();
			if (rlen == 0)
				continue;

			seq_str.Format("%d", k+1);
			set->AddNew();

			set->set_string_value("ICAO_CODE", obj->m_icao);
			set->set_string_value("ID", obj->m_id);
			set->set_string_value("DIRECTION", obj->m_direction);
			set->set_string_value("REMARKS_SEQ", seq_str);
			set->set_string_value("CYCLE_DATE", date);
			
			if (rlen > 80)
			{
				rmk = rmks.Left(80);
				rmks = rmks.Right(rlen - 80);
			}
			else
			{
				rmk = rmks;
				rmks = "";
			}

			set->set_string_value("REMARK_1", rmk);

			rlen = rmks.GetLength();
			if (rlen == 0)
			{
				rslt = update_remark(set);
				if (!rslt)
					return FALSE;
				continue;
			}

			if (rlen > 80)
			{
				rmk = rmks.Left(80);
				rmks = rmks.Right(rlen - 80);
			}
			else
			{
				rmk = rmks;
				rmks = "";
			}

			set->set_string_value("REMARK_2", rmk);

			rlen = rmks.GetLength();
			if (rlen == 0)
			{
				rslt = update_remark(set);
				if (!rslt)
					return FALSE;
				continue;
			}

			if (rlen > 80)
			{
				rmk = rmks.Left(80);
				rmks = rmks.Right(rlen - 80);
			}
			else
			{
				rmk = rmks;
				rmks = "";
			}

			set->set_string_value("REMARK_3", rmk);		
			
			
			rslt = update_remark(set);
			if (!rslt)
				return FALSE;
		}
		return TRUE;
	}
	catch (CDaoException *e)
	{
		e->Delete();
		ERR_report("CDaoException saving route remarks");
		return FAILURE;
	}
	catch(...)
	{
		return FAILURE;
	}
}
// end of save_remarks

// ***********************************************************************************************************
// ***********************************************************************************************************
