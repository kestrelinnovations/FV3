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



// COMPILING_DAFIF_DLL will be defined only when we compile the DAFIF DLL; in
// FalconView or an A/W/E project it will not be defined.  This allows us to 
// set a macro that specifies if this class is being exported (from the DLL) 
// or imported into an application.

#ifndef COMPILING_DAFIF_DLL
   #define DAFIFMTRRT_DECLSPEC __declspec( dllimport )
   //#pragma message( "Compiling classes in " __FILE__ " as dllimport" ) 
#else
   #define DAFIFMTRRT_DECLSPEC __declspec( dllexport )
   //#pragma message( "Compiling classes in " __FILE__ " as dllexport" ) 
#endif

#include <afxtempl.h>

#pragma once

class CMTRRouteRecord;
class CMTRRouteSegmentRecord;
class mtr_description_set;

//Class prototype
class DAFIFMTRRT_DECLSPEC CMTRRoutes
{
	// Constructors and destructors
	public:
		CMTRRoutes( );
		CMTRRoutes( CMTRRoutes& c );
		virtual	~CMTRRoutes( );

	// Member functions
		virtual	BOOL get_route_records(CList<CMTRRouteRecord*,CMTRRouteRecord*> &list,int filter =0);
		virtual	BOOL get_route_segments(CList<CMTRRouteSegmentRecord*,CMTRRouteSegmentRecord*> &list,CMTRRouteRecord *rec);
		virtual	BOOL open_database();
		virtual	void fill_route_segment_remarks(CMTRRouteRecord &rec);
		virtual int mtr_data_check();

	protected:
		void	Init( );
		void	Copy( CMTRRoutes& c );

	//State variable functions
	public:

	//State variable declarations
	private:
		CDaoDatabase	*m_database;
};


//Class prototype
class DAFIFMTRRT_DECLSPEC CMTRRouteRecord
{
	// Constructors and destructors
	public:
		CMTRRouteRecord( );
		CMTRRouteRecord( CMTRRouteRecord& c );
		virtual	~CMTRRouteRecord( );

	protected:
		void	Init( );
		void	Copy( CMTRRouteRecord& c );

	//State variable functions
	public:

	//State variable declarations
		CString	m_CTRY_CODE,m_CYCLE_DATE,m_EFFECTIVE_TIMES,m_GEODETIC_DATUM;
		CString	m_ICAO_REGION,m_ID,m_LOCAL_DATUM,m_ORIG_ACTIVITY;
		CString	m_SCHEDULED_ACTIVITY;

		//each record in the database is a separate element in the array
		//the 3 remark fields are combined with a space separating them
		CStringArray	m_remark_array;
		BOOL			m_remark_valid;


};

// **********************************************************************

//Class prototype
class DAFIFMTRRT_DECLSPEC CMTRRouteSegmentRecord
{
	// Constructors and destructors
	public:
		CMTRRouteSegmentRecord( );
		CMTRRouteSegmentRecord( CMTRRouteSegmentRecord& c );
		virtual	~CMTRRouteSegmentRecord( );


		const CMTRRouteSegmentRecord& operator =(mtr_description_set *pSet);


	protected:
		void	Init( );
		void	Copy( CMTRRouteSegmentRecord& c );

	//State variable functions
	public:

	//State variable declarations

		CString	m_ICAO_REGION;
		CString	m_ID;
		CString	m_POINT_ID;
		CString	m_NEXT_POINT_ID;
		CString	m_CROSS_ALT_DESC;
		CString	m_CROSS_ALT_ONE;
		CString	m_CROSS_ALT_TWO;
		CString	m_ENRTE_ALT_DESC;
		CString	m_ENRTE_ALT_1;
		CString	m_ENRTE_ALT_2;
		CString	m_POINT_NAV_FLAG;
		CString	m_NAV_ID;
		CString	m_NAV_TYPE;
		CString	m_NAV_CTRY_CODE;
		CString	m_NAV_KEY_CODE;
		CString	m_FIX;
		CString	m_WIDTH;
		CString	m_USAGE_CODE;
		double	m_LAT;
		double	m_LON;
		double	m_GEODETIC_LAT;
		double	m_GEODETIC_LONG;
		CString	m_ADD_RTE_INFO;
		CString	m_CYCLE_DATE;
		CString	m_TURN_DIRECTION;
		CString	m_TURN_RADIUS;

};
