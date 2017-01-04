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
   #define DAFIFATS_DECLSPEC __declspec( dllimport )
   //#pragma message( "Compiling classes in " __FILE__ " as dllimport" ) 
#else
   #define DAFIFATS_DECLSPEC __declspec( dllexport )
   //#pragma message( "Compiling classes in " __FILE__ " as dllexport" ) 
#endif

#include <afxtempl.h>

class CDaoDatabase;
class AtsRouteSet;
class AtsRouteRmkSet;
class AtsRouteCntySet;

// **********************************************************************

#ifndef __ATSROUTERECORD_H
#define __ATSROUTERECORD_H

//Class prototype
class DAFIFATS_DECLSPEC CATSRouteRecord
{
	// Constructors and destructors
	public:
		CATSRouteRecord( );
		CATSRouteRecord( CATSRouteRecord& c );
		virtual	~CATSRouteRecord( );

	// Member functions
		virtual	CString get_route_direction();
		virtual	CString get_route_id();

	protected:
		void	Init( );
		void	Copy( CATSRouteRecord& c );

	//State variable functions
	public:

	//State variable declarations
	CString	m_route_direction,m_route_id;

	//possible filter data
	CString	m_BI_DIRECTIONAL;
	CString	m_BOUNDARY_CODE;
	CString	m_FREQ_CLASS;
	CString	m_LEVEL;
	CString	m_STATUS;
	CString	m_TYPE;

};

#endif

// **********************************************************************

#ifndef __ATSROUTESEGMENTRECORD_H
#define __ATSROUTESEGMENTRECORD_H

//Class prototype
class DAFIFATS_DECLSPEC CATSRouteSegmentRecord
{
	// Constructors and destructors
	public:
		CATSRouteSegmentRecord( );
		CATSRouteSegmentRecord( CATSRouteSegmentRecord& c );
		virtual	~CATSRouteSegmentRecord( );

	// Member functions

		//call this function to see if the remarks are valid for this segment
		virtual	BOOL is_remark_valid();

	protected:

		//
		virtual	BOOL get_segments_points();
		void	Init( );
		void	Copy( CATSRouteSegmentRecord& c );

	//State variable functions
	public:
		const CATSRouteSegmentRecord& operator=(AtsRouteSet* pSet);

		//data that is stored in the database for ATS Routes
		CString	m_ICAO_CODE;
		CString	m_ID;
		CString	m_SEQ_NUM;
		CString	m_DIRECTION;
		CString	m_TYPE;
		CString	m_BI_DIRECTIONAL;
		CString	m_FREQ_CLASS;
		CString	m_LEVEL;
		CString	m_STATUS;
		CString	m_BOUNDARY_CODE;
		CString	m_WAYPT_1_ICAO_CODE;
		CString	m_WAYPT_1_NAV_TYPE;
		CString	m_WAYPT_1_ID;
		CString	m_WAYPT_1_CTRY_CODE;
		CString	m_WAYPT_1_DESC_CODE;
		CString	m_WAYPT_2_ICAO_CODE;
		CString	m_WAYPT_2_NAV_TYPE;
		CString	m_WAYPT_2_IDENTIFIER;
		CString	m_WAYPT_2_CTRY_CODE;
		CString	m_WAYPT_2_DESC_CODE;
		CString	m_OUT_MAG_COURSE;
		CString	m_DISTANCE;
		CString	m_IN_MAG_COURSE;
		CString	m_MINIMUM_ALT;
		CString	m_ENRTE_COMM_NAME;
		CString	m_ENRTE_COMM_FREQ_1;
		CString	m_ENRTE_COMM_FREQ_2;
		CString	m_ENRTE_COMM_FREQ_3;
		CString	m_ENRTE_COMM_FREQ_4;
		CString	m_CYCLE_DATE;
		CString	m_CRUISE_LEV_IND;
		CString	m_LOWER_LIMIT;
		CString	m_MAX_AUTHORIZED_ALT;
		CString	m_UPPER_LIMIT;

		//the lat long for the point are looked up automaticly when you load the record
		double m_lat_1;
		double m_lat_2;
		double m_lon_1;
		double m_lon_2;		

		//remarks that are sotred in a separate table
		//these vars are not set by default because there is 
		//some overhead involved with filling in the values
		//to set them call the fill_route_segment_remarks function in CATSRoutes
		//This function will look up the values for the segment and set the m_remark_valid flag
		//in this segment class.  Check thE flag by calling is_remark_valid()
		//to see if the remarks have been filled in.

		CString m_REMARK_1;
		CString m_REMARK_2;
		CString m_REMARK_3;

		BOOL	m_remark_valid;

	//State variable declarations
	private:

};

#endif

// **********************************************************************


#ifndef __ATSROUTECTRYSTATE_H
#define __ATSROUTECTRYSTATE_H

//Class prototype
class DAFIFATS_DECLSPEC CATSRouteCtryState
{
	// Constructors and destructors
	public:
		CATSRouteCtryState( );
		CATSRouteCtryState( CATSRouteCtryState& c );
		virtual	~CATSRouteCtryState( );

	// Member functions
		virtual	CString get_ctry_code();
		virtual	CString get_state_code();
      virtual  CString get_seq_num();

	protected:
		void	Init( );
		void	Copy( CATSRouteCtryState& c );

	//State variable functions
	public:

	//State variable declarations
	CString	m_ctry_code,m_state_code,m_seq_num;
};

#endif

// **********************************************************************

#ifndef __ATSROUTES_H
#define __ATSROUTES_H

//Class prototype
class DAFIFATS_DECLSPEC CATSRoutes
{
	// Constructors and destructors
	public:
		CATSRoutes( );
		CATSRoutes( CATSRoutes& c );
		virtual	~CATSRoutes( );

	// Member functions
		virtual	BOOL get_route_records(CList<CATSRouteRecord*,CATSRouteRecord*> &list,int filter = 0);
		virtual	BOOL get_route_seg_ctry_info(CList<CATSRouteCtryState*,CATSRouteCtryState*> &list,CATSRouteSegmentRecord *rec);
		virtual	BOOL get_route_segments(CList<CATSRouteSegmentRecord*,CATSRouteSegmentRecord*> &list,CATSRouteRecord *rec);
		virtual	BOOL open_database();
		virtual	void fill_route_segment_remarks(CATSRouteSegmentRecord &seg);


      //Test Function
      BOOL get_routes_through_waypoint(CList<CATSRouteRecord*,CATSRouteRecord*> &list,CString wp_id,CString ctry_code);

	protected:
		void	Init( );
		void	Copy( CATSRoutes& c );

	public:
		int ats_data_check();

	//State variable declarations
	private:
		CDaoDatabase*     m_database;
		AtsRouteSet*      m_pRouteSet;
		AtsRouteRmkSet*   m_pRouteRmkSet;
		AtsRouteCntySet*  m_pRouteCtrySet;
};

#endif

// **********************************************************************
