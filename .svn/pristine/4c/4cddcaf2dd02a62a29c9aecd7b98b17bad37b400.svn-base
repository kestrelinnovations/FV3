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



// Thu Feb 13 15:54:34 1997


// **********************************************************************

// COMPILING_DAFIF_DLL will be defined only when we compile the DAFIF DLL; in
// FalconView or an A/W/E project it will not be defined.  This allows us to 
// set a macro that specifies if this class is being exported (from the DLL) 
// or imported into an application.

#ifndef COMPILING_DAFIF_DLL
   #define DAFIFREC_DECLSPEC __declspec( dllimport )
   //#pragma message( "Compiling classes in " __FILE__ " as dllimport" ) 
#else
   #define DAFIFREC_DECLSPEC __declspec( dllexport )
   //#pragma message( "Compiling classes in " __FILE__ " as dllexport" ) 
#endif

#ifndef __DBRECORD_H
#define __DBRECORD_H

#include "..\..\..\..\government\Components\Dafif_Dll\records\rsserver.h"

#include <afxtempl.h>

class airport_set;
class waypoint_set;
class navaid_set;
class refueling_track_point_set;
class mtr_description_set;
class runway_set;
class CLocalPointSet;
class CAimpointSet;
class CDropzoneSet;

const char *RS_datum_to_DMA(short datum);
short DMA_datum_to_RS(const char *datum);

//Class prototype
class DAFIFREC_DECLSPEC CDBRecord	:public	CObject
{
	// Constructors and destructors
	public:
	BOOL get_state_name(CString state_code, CString &newone);
	BOOL get_country_name(CString ctry_code, CString &newone);
	virtual LPCSTR get_country_code()=0;
	virtual CString get_db_lookup_rs()=0;
	virtual CString get_description_rs()=0;
	virtual CString get_fix_rs()=0;
	char m_area_code;
		CDBRecord( );
		CDBRecord( CDBRecord& c );
		virtual	~CDBRecord( );

   public:
   // USER_DEFINED_ELV instead of USER_DEFINED because of a conflict with 
   // maddef.h.
   enum elv_src_t {DAFIF, ELV_UNKNOWN, USER_DEFINED_ELV, DTED};
   
   enum pnt_src_rs_t {USER = 0, WAYPOINT = 4, LOCAL, GPS = 11, AIR_REFUEL, 
      TRAIN_ROUTE, AIRPORT = 15, RUNWAY, HELIPORT, HELIPAD, NAVAID, 
      AIRSPACE_BOUNDARY, SUAS, AIMPOINT, OTHER = 99};

	// Member functions
		virtual	BOOL is_kind_of(LPCSTR newtype);
		virtual	LPCSTR get_class_name();
	   
    	virtual BOOL get_rs_point(RS_POINT &point)=0;
     	virtual char get_area_code()=0;

	protected:
		void	Init( );
		void	Copy( CDBRecord& c );
      int string_to_mag_var(LPCSTR str_mag_var,int &n_mag_var);


	//State variable functions
	public:
		virtual	double get_latitude();
		virtual	double get_longitude();
		virtual	void set_latitude(double newLat);
		virtual	void set_longitude(double newLong);

      virtual void set_pnt_src_rs(pnt_src_rs_t new_pnt_src);
      virtual int get_pnt_src_rs();


     	LPCSTR get_navaid_type(int type);

	//State variable declarations
		double	m_lat,m_long;
		pnt_src_rs_t	m_pnt_src_rs;
};

#endif

// **********************************************************************

#ifndef __RUNWAYRECORD_H
#define __RUNWAYRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CRunwayRecord
{
	// Constructors and destructors
	public:
		CRunwayRecord( );
		CRunwayRecord( CRunwayRecord& c );
		virtual	~CRunwayRecord( );

	// Member functions
		virtual	int get_adj_runway_length();

	protected:
		void	Init( );
		void	Copy( CRunwayRecord& c );

	//State variable functions
	public:
      const CRunwayRecord& operator = (runway_set *pSet);

	//State variable declarations

		double	m_GEODETIC_LAT_HI,m_GEODETIC_LAT_LO,m_GEODETIC_LONG_HI;
		double	m_GEODETIC_LONG_LO,m_LATITUDE_HI,m_LATITUDE_LO;
		double	m_LONGITUDE_HI,m_LONGITUDE_LO;
		CString	m_AIRPORT_ID,m_CYCLE_DATE,m_DSP_THRES_ELEV_HI;
		CString	m_DSP_THRES_ELEV_LO,m_DSP_THRES_HI,m_DSP_THRES_LO;
		CString	m_ELEVATION_HI,m_ELEVATION_LO,m_ICAO_CODE,m_LENGTH;
		CString	m_LIGHTING_SYS_1_HI,m_LIGHTING_SYS_1_LO,m_LIGHTING_SYS_2_HI;
		CString	m_LIGHTING_SYS_2_LO,m_LIGHTING_SYS_3_HI,m_LIGHTING_SYS_3_LO;
		CString	m_LIGHTING_SYS_4_HI,m_LIGHTING_SYS_4_LO,m_LIGHTING_SYS_5_HI;
		CString	m_LIGHTING_SYS_5_LO,m_LIGHTING_SYS_6_HI,m_LIGHTING_SYS_6_LO;
		CString	m_LIGHTING_SYS_7_HI,m_LIGHTING_SYS_7_LO,m_LIGHTING_SYS_8_HI;
		CString	m_LIGHTING_SYS_8_LO,m_MAG_HEADING_HI,m_MAG_HEADING_LO;
		CString	m_PAVEMENT_CLASS,m_RUNWAY_ID_HI,m_RUNWAY_ID_LO;
		CString	m_SLOPE_HI,m_SLOPE_LO,m_SURFACE,m_TDZE_HI,m_TDZE_LO;
		CString	m_WIDTH;
		CString  m_CLOSED_UNUSABLE;
};

#endif

// **********************************************************************

#ifndef __AIRPORTRECORD_H
#define __AIRPORTRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CAirportRecord	:public	CDBRecord
{
	// Constructors and destructors
	public:
	CString m_ctry_code;
	BOOL get_runway(int index, CRunwayRecord *&pRec);
	void empty_runway_list();
	BOOL m_runway_valid;
	BOOL is_runway_valid();
	BOOL m_bValid;
	BOOL is_valid();
	
	
		CAirportRecord( );
		CAirportRecord( CAirportRecord& c );
		virtual	~CAirportRecord( );

	// Member functions
		virtual	BOOL fill_runway_list();
		virtual	BOOL find_longest_adjusted_runway();
		virtual	int get_longest_runway();
		virtual	int get_number_of_runways();
      virtual LPCSTR get_country_code();
      virtual char get_area_code();
      virtual BOOL get_rs_point(RS_POINT &point);
      virtual void set_longest_runway(int rw_length);
	   virtual BOOL GetInfo(CString *db_info);

     	virtual LPCSTR get_class_name();
   	virtual BOOL is_kind_of(LPCSTR newType);
      virtual CString get_db_lookup_rs();
	   virtual CString get_description_rs();
	   virtual CString get_fix_rs();
	   

	   
	protected:
		void	Init( );
		void	Copy( CAirportRecord& c );

	//State variable functions
	public:

   //operators
      virtual const CAirportRecord& operator =(airport_set *pSet);
      
	//State variable declarations
	private:
		int	m_adj_runway_length;
      
      CArray<CRunwayRecord*,CRunwayRecord*> m_runway_list;

      int m_nMag_var;

   public:
		double	m_GEODETIC_LAT,m_GEODETIC_LONG,m_LATITUDE,m_LONGITUDE;
		CString	m_AIRPORT_ID,m_CYCLE_DATE,m_ELEVATION,m_FAA_ID;
		CString	m_GEODETIC_DATUM,m_ICAO_CODE,m_LOCAL_DATUM,m_MAG_VAR;
		CString	m_NAME,m_ROTATING_BEACON,m_STATE_CODE,m_TYPE;
		CString	m_WAC;
		
};

#endif

// **********************************************************************

#ifndef __WAYPOINTRECORD_H
#define __WAYPOINTRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CWaypointRecord : public CDBRecord
{
	// Constructors and destructors
	public:
	CString waypoint_type_to_string();
   virtual char get_area_code();
	virtual CString get_fix_rs();
	virtual CString get_description_rs();
	virtual LPCSTR get_country_code();
	virtual BOOL get_rs_point(RS_POINT &point);
		CWaypointRecord( );
		CWaypointRecord( CWaypointRecord& c );
		virtual	~CWaypointRecord( );

	protected:
		void	Init( );
		void	Copy( CWaypointRecord& c );

	//State variable functions
	public:
      virtual const CWaypointRecord& operator =(waypoint_set *pSet);
      virtual CString get_db_lookup_rs();
	
     	virtual LPCSTR get_class_name();
   	virtual BOOL is_kind_of(LPCSTR newType);
   

	//State variable declarations
		CString	m_BEARING_AND_DIST,m_CTRY_CODE,m_CYCLE_DATE;
		CString	m_DESC_NAME,m_GEODETIC_DATUM,m_ICAO_CODE,m_ID;
		CString	m_LOCAL_DATUM,m_MAG_VAR,m_NAV_CTRY_CODE,m_NAV_ID;
		CString	m_NAV_KEY_CODE,m_NAV_TYPE,m_STATE_CODE,m_TYPE;
		CString	m_USAGE_CODE,m_WAC,m_WAYPT_NAV_FLAG;

      double	m_LAT;
	   double	m_LON;
	   double	m_GEODETIC_LAT;
	   double	m_GEODETIC_LONG;


};

#endif

// **********************************************************************

#ifndef __CATSPOINTRECORD_H
#define __CATSPOINTRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CATSPointRecord	:public	CWaypointRecord
{
	// Constructors and destructors
	public:
		CATSPointRecord( );
		CATSPointRecord( CATSPointRecord& c );
		virtual	~CATSPointRecord( );

	// Member functions
		virtual BOOL get_rs_point(RS_POINT &point);
		virtual CString get_description_rs();
		virtual CString get_db_lookup_rs();
		virtual BOOL is_kind_of(LPCSTR newType);
		virtual CString get_fix_rs();
		virtual LPCSTR get_class_name();

	protected:
		void	Init( );
		void	Copy( CATSPointRecord& c );

	//State variable functions
	public:
		void set_route_id(CString newone);
		void set_seq_num(CString newone);
		void set_direction(CString newone);

	//State variable declarations
	private:
		CString	m_route_id;
		CString  m_seq_num;
		CString  m_direction;
};

#endif

// **********************************************************************

#ifndef __NAVIADRECORD_H
#define __NAVIADRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CNavaidRecord	:public	CDBRecord
{
	// Constructors and destructors
	public:
	CString nav_type_to_string(char new_type);
	CString get_fix_rs();
	virtual CString get_description_rs();
	virtual char get_area_code();
	float m_nav_chan;
	short m_slave_var;
	float m_nav_freq_MHz;
	virtual LPCSTR get_country_code();
	virtual BOOL get_rs_point(RS_POINT &point);
	virtual CString get_db_lookup_rs();
		CNavaidRecord( );
		CNavaidRecord( CNavaidRecord& c );
		virtual	~CNavaidRecord( );

	protected:
		void	Init( );
		void	Copy( CNavaidRecord& c );

	//State variable functions
	public:
      virtual const CNavaidRecord& operator = (navaid_set *pSet);
     	
      virtual LPCSTR get_class_name();
   	virtual BOOL is_kind_of(LPCSTR newType);

	//State variable declarations
		double	m_DME_GEODETIC_LAT,m_DME_GEODETIC_LONG,m_DME_LAT;
		double	m_DME_LONG,m_GEODETIC_LAT,m_GEODETIC_LONG,m_LAT;
		double	m_LON;

		CString	m_CHANNEL,m_CTRY_CODE,m_CYCLE_DATE,m_DME_ELEVATION;
		CString	m_ELEVATION,m_FREQ,m_FREQ_PROTECTION,m_GEODETIC_DATUM;
		CString	m_ICAO_CODE,m_ID,m_LOCAL_DATUM,m_MAG_VAR,m_NAME;
		CString	m_NAV_KEY_CODE,m_POWER,m_RADIO_CLASS_CODE,m_RANGE;
		CString	m_SLAVED_VAR,m_STATE_CODE,m_TYPE,m_USAGE_CODE;
		CString	m_WAC;
};

#endif

// **********************************************************************

#ifndef __MTRRECORD_H
#define __MTRRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CMTRRecord	:public	CDBRecord
{
	// Constructors and destructors
	public:
	virtual char get_area_code();
	virtual LPCSTR get_country_code();
	virtual BOOL get_rs_point(RS_POINT &point);
		CMTRRecord( );
		CMTRRecord( CMTRRecord& c );
		virtual	~CMTRRecord( );

	// Member functions
		virtual	BOOL is_kind_of(LPCSTR newType);
		virtual	CString get_db_lookup_rs();
		virtual	CString get_description_rs();
		virtual	CString get_fix_rs();
		virtual	LPCSTR get_class_name();

	protected:
		void	Init( );
		void	Copy( CMTRRecord& c );

	//State variable functions
	public:
      //operators
      virtual const CMTRRecord& operator =(mtr_description_set *pSet);


	//State variable declarations
		double	m_GEODETIC_LAT,m_GEODETIC_LONG,m_LAT,m_LON;
		CString	m_ADD_RTE_INFO,m_CROSS_ALT_DESC,m_CROSS_ALT_ONE;
		CString	m_CROSS_ALT_TWO,m_CYCLE_DATE,m_ENRTE_ALT_1,m_ENRTE_ALT_2;
		CString	m_ENRTE_ALT_DESC,m_FIX,m_ICAO_REGION,m_ID,m_NAV_CTRY_CODE;
		CString	m_NAV_ID,m_NAV_KEY_CODE,m_NAV_TYPE,m_NEXT_POINT_ID;
		CString	m_POINT_ID,m_POINT_NAV_FLAG,m_USAGE_CODE,m_WIDTH;

		//NEW FIELDS FOR DAFIF EDITION 5;
		//CString m_RADIUS,m_TURN_DIRECTION;
};

#endif

// **********************************************************************

#ifndef __REFUELRECORD_H
#define __REFUELRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CRefuelRecord	:public	CDBRecord
{
	// Constructors and destructors
	public:
	char get_area_code();
	virtual LPCSTR get_country_code();
	virtual BOOL get_rs_point(RS_POINT &point);
		CRefuelRecord( );
		CRefuelRecord( CRefuelRecord& c );
		virtual	~CRefuelRecord( );

	// Member functions
		virtual	BOOL is_kind_of(LPCSTR newType);
		virtual	CString get_db_lookup_rs();
		virtual	CString get_description_rs();
		virtual	CString get_fix_rs();
		virtual	LPCSTR get_class_name();

	protected:
		void	Init( );
		void	Copy( CRefuelRecord& c );

	//State variable functions
	public:
   //operators
      virtual const CRefuelRecord& operator =(refueling_track_point_set *pSet);


	//State variable declarations
	
		double	m_GEODETIC_LAT,m_GEODETIC_LONG,m_LAT,m_LON;
		CString	m_CYCLE_DATE,m_DIRECTION,m_FIX,m_ICAO_CODE,m_ID;
		CString	m_NAV_CTRY_CODE,m_NAV_FLAG,m_NAV_ID,m_NAV_KEY_CODE;
		CString	m_NAV_TYPE,m_SEQ_NUM,m_USAGE;
};

#endif

// **********************************************************************

#ifndef __LOCALPOINTRECORD_H
#define __LOCALPOINTRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CLocalPointRecord	:public	CDBRecord
{
	// Constructors and destructors
	public:
	virtual char get_area_code();
	virtual LPCSTR get_country_code();
	virtual BOOL get_rs_point(RS_POINT &point);
		CLocalPointRecord( );
		CLocalPointRecord( CLocalPointRecord& c );
		virtual	~CLocalPointRecord( );

	// Member functions
		virtual	BOOL is_kind_of(LPCSTR newType);
		virtual	CString get_db_lookup_rs();
		virtual	CString get_description_rs();
		virtual	CString get_fix_rs();
		virtual	LPCSTR get_class_name();

	protected:
		void	Init( );
		void	Copy( CLocalPointRecord& c );

	//State variable functions
	public:
      //operators
      virtual const CLocalPointRecord& operator =(CLocalPointSet *pSet);
   
	//State variable declarations
		float	m_Horz_Accuracy,m_Vert_Accuracy;
		double	m_Latitude,m_Longitude;
		CString	m_Area,m_Comment,m_Country_Code,m_Description;
		CString	m_Dtd_ID,m_Elevation_Source,m_Group_Name,m_ID;
		CString	m_Icon_Name,m_Link_Name,m_Pt_Quality;
		short	m_Elevation;

      CString m_Elevation_Source_Map_Handler_Name;
		CString m_Elevation_Source_Map_Series_ID;
};

#endif

// **********************************************************************

#ifndef __AIMPOINTRECORD_H
#define __AIMPOINTRECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CAimpointRecord	:public	CDBRecord
{
	// Constructors and destructors
	public:
	virtual char get_area_code();
	virtual LPCSTR get_country_code();
	virtual BOOL get_rs_point(RS_POINT &point);
		CAimpointRecord( );
		CAimpointRecord( CAimpointRecord& c );
		virtual	~CAimpointRecord( );

	// Member functions
		virtual	BOOL is_kind_of(LPCSTR newType);
		virtual	CString get_db_lookup_rs();
		virtual	CString get_description_rs();
		virtual	CString get_fix_rs();
		virtual	LPCSTR get_class_name();

	protected:
		void	Init( );
		void	Copy( CAimpointRecord& c );

	//State variable functions
	public:
      //operators
      virtual const CAimpointRecord& operator =(CAimpointSet *pSet);


	//State variable declarations
	   float	m_Horz_Accuracy,m_Vert_Accuracy;
		double	m_Latitude,m_Longitude;
		CString	m_Classification,m_Country_Code,m_Description;
		CString	m_Description_Code,m_Elevation_Source,m_ID,m_JOG_Code;
		CString	m_Plot,m_Producer_Code,m_Releasability_Code;
		CString	m_Sheet,m_TPC_Code,m_WAC;
		short	m_Descriptor_Code_1,m_Descriptor_Code_2,m_Descriptor_Code_3;
		short	m_Elevation;
		COleDateTime	m_Photo_Date,m_Photo_Review_Date,m_Publication_Date;
};

#endif

// **********************************************************************

#ifndef __CDROPZONERECORD_H
#define __CDROPZONERECORD_H

//Class prototype
class DAFIFREC_DECLSPEC CDropzoneRecord :public	CDBRecord
{
	// Constructors and destructors
	public:
	virtual CString get_fix_rs();
	virtual LPCSTR get_country_code();
		CDropzoneRecord( );
		CDropzoneRecord( CDropzoneRecord& c );
		virtual	~CDropzoneRecord( );

	// Member functions
		virtual	BOOL get_rs_point(RS_POINT &point);
		virtual	BOOL is_kind_of(LPCSTR newType);
		virtual	CString get_db_lookup_rs();
		virtual	CString get_description_rs();
		virtual	LPCSTR get_class_name();
		virtual	char get_area_code();

	protected:
		void	Init( );
		void	Copy( CDropzoneRecord& c );

	//State variable functions
	public:
      //operators
      virtual const CDropzoneRecord& operator =(CDropzoneSet *pSet);

		virtual	void	SetM_DZ_Length( float newM_DZ_Length );
		virtual	float	GetM_DZ_Length(  ) const;

	//State variable declarations
	private:
		float	m_CDS_PI_to_LE,m_CM_to_LE,m_DZ_Length,m_DZ_Radius;
		float	m_DZ_Width,m_HE_PI_to_LE,m_Mag_Course,m_Off_Centerline_Distance;
		float	m_PE_PI_to_LE;
		long	m_Chute_Approval;
		double	m_CDS_PI_Latitude,m_CDS_PI_Longitude,m_DZ_Centerpoint_Latitude;
		double	m_DZ_Centerpoint_Longitude,m_HE_PI_Latitude;
		double	m_HE_PI_Longitude,m_Index_Number,m_Left_LE_Latitude;
		double	m_Left_LE_Longitude,m_Left_TE_Latitude,m_Left_TE_Longitude;
		double	m_PE_PI_Latitude,m_PE_PI_Longitude,m_Right_LE_Latitude;
		double	m_Right_LE_Longitude,m_Right_TE_Latitude,m_Right_TE_Longitude;
		CString	m_AirOps_Approval_Date,m_AirOps_Approver_Info;
		CString	m_AirOps_Approver_Phone,m_Chart_Date,m_Chart_Edition;
		CString	m_Chart_Series,m_Chart_Sheet_Number,m_Chart_Spheroid;
		CString	m_Country_Code,m_DZ_Control_Agency_Phone,m_DZ_Control_Agency_Unit;
		CString	m_DZ_Diagram_Type,m_DZ_Remarks,m_FV_Overlay_Type;
		CString	m_GndOps_Approval_Date,m_GndOps_Approver_Info;
		CString	m_GndOps_Approver_Phone,m_Last_Changed_Date;
		CString	m_Location,m_Name,m_Point_of_Origin,m_Primary_PI;
		CString	m_Range_Control,m_Range_Control_Phone,m_Route_Name;
		CString	m_SOF_Review_Date,m_SOF_Reviewer_Info,m_SOF_Reviewer_Phone;
		CString	m_State,m_Survey_Date,m_Surveyor_Info,m_Surveyor_Phone;
		short	m_AirOps_Remarks,m_CDS_PI_Elevation,m_Chart_Datum;
		short	m_Chart_Easting,m_Chart_Northing,m_Circular_Elevation;
		short	m_Elevation_Source,m_GPS_Coordinates,m_GPS_Error;
		short	m_GndOps_Remarks,m_HE_PI_Elevation,m_Highest_Elevation;
		short	m_LowLevel_Routes_Available,m_Lowest_Elevation;
		short	m_PE_PI_Elevation,m_Photography_Available,m_Runin_Restrictions;
		short	m_SOF_Remarks,m_Survey_Validation_Level,m_Surveyor_Remarks;
};

#endif

#ifndef __STUB1TEST__
#define __STUB1TEST__
// does nothing except force AtsRoutes to be exported
class DAFIFREC_DECLSPEC CStub1
{
public:
   CStub1();
   virtual ~CStub1();
};
#endif

#ifndef __STUB2TEST__
#define __STUB2TEST__
// does nothing except force MtrRoutes to be exported
class DAFIFREC_DECLSPEC CStub2
{
public:
   CStub2();
   virtual ~CStub2();
};
#endif
