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



// SnapToInfo class definition

#ifndef __SNAPTOINFO_H_
#define __SNAPTOINFO_H_

//Forward definitions 
class C_icon;
class CDBRecord;
class C_daf_info;

struct ISnapToPointsList;

struct SNAPTO_STRUCT
{
   // USER_DEFINED_ELV instead of USER_DEFINED because of a conflict with 
   // maddef.h.
   enum elv_src_t {DAFIF, ELV_UNKNOWN, USER_DEFINED_ELV, DTED};

   enum pnt_src_rs_t {USER_SRC = 0, WAYPOINT_SRC = 4, LOCAL_SRC, GPS_SRC = 11, AIR_REFUEL, 
      TRAIN_ROUTE, AIRPORT = 15, RUNWAY, HELIPORT, HELIPAD, NAVAID, 
      AIRSPACE_BOUNDARY, SUAS, AIMPOINT, DROPZONE = 24, ATS_ROUTE, LANDINGZONE, 
      UNIT_LANDINGZONE, UNIT_DROPZONE, OTHER = 99};

   SNAPTO_STRUCT()
   {
      // initialize every thing to 0, note NULL and '\0' are 0
      memset(this, 0, sizeof(*this));
   }

   double m_lat;              // Required
   double m_lon;              // Required
   char m_src_datum[6];       // Required
   short m_elv;               // elevation in feet
   elv_src_t m_elv_src;       // Conditional - set if m_elv is set
   float m_mag_var;           // magnetic variation
   float m_slave_var;         // slave variation (Navaids)
   pnt_src_rs_t m_pnt_src_rs; // optional - USER by default (RS)
   char m_area_code;          // first letter of icao code - ' ' by default
   char m_nav_type;           // point type (Navaids)
   char m_dtd_id[9];          // Data Transfer Device ID, SZ_EXTENDED_DTD
   char m_fix_rs[15];         // RS Fix/Point field (icao)
   char m_description_rs[41]; // RS Description field
   char m_country_code[3];    // country code
   char m_qlt;                // quality - '0' by default
   char m_cls;                // classification - 'U' by default
   float m_nav_freq_MHz;      // Navaid frequency in MHz
   float m_nav_chan;          // Navaid channel -/+ = X/Y
   float m_horz_accuracy;     // accuracy in feet, -1.0 by default
   float m_vert_accuracy;     // accuracy in feet, -1.0 by default
   char m_db_lookup_rs[41];   // RS database lookup
   char m_db_lookup_filename_rs[261];  // Database lookup source information

   // icon identifying members: what is being snaped to

   // broad class of objects the C_icon object is a particular instance of
   char m_icon_type[11];

   // identifying remark differentiating this icon from others of its type
   char m_icon_description[51];
};

// class to encapsulate a point that is snapped to
class SnapToInfo : public SNAPTO_STRUCT
{
   // enumerated types
public:


   // USER_SRC            0     User Entered
   // WAYPOINT_SRC        4     Waypoint (DAFIF)
   // LOCAL_SRC           5     Local Point-defined
   // GPS_SRC            11     Global Positioning Satellite Data
   // AIR_REFUEL         12     Air Refueling Track / Anchor (DAFIF)
   // TRAIN_ROUTE        13     Military Training Route (DAFIF)
   // AIRPORT            15     Airport (DAFIF)
   // RUNWAY             16     Airport - Runway (DAFIF)
   // HELIPORT           17     Heliport (DAFIF)
   // HELIPAD            18     Heliport - Helipad (DAFIF)
   // NAVAID             19     Navaid (DAFIF)
   // AIRSPACE_BOUNDARY  20     Airspace Boundary Point (DAFIF)
   // SUAS               21     Special Use Airspace Point (DAFIF)
   // AIMPOINT           22     Aimpoint
   // DROPZONE           24     Drop Zone
   // ATS_ROUTE          25     ATS Route Point (DAFIF)
   // OTHER              99     Other Non-User Entered Point

   // data members: possible data set by a snap to operation
public:
   CString m_strKey;       // mouse-click resource (snaptocallback) and local points uses this field.  We
   // might need to add this to the snap-to information objects

   CString m_strTooltip;   // only point export tool uses this
   GUID m_overlayDescGuid; // only point export tool uses this
   CString m_strOverlayName;  // only point export tool uses this

public:
   // Constructor
   SnapToInfo();

   static SnapToInfo* CreateFromSnapToPointsList(ISnapToPointsList* spSnapToPointsList);

   ISnapToPointsList* ConvertToSnapToPointsList() /*throw(_com_error)*/;

private:
   // Sets all fields to default values.  Latitude and longitude must be
   // set after this call.  WGS-84 datum is assumed.  If a non-WGS-84 source
   // datum is used, m_src_datum must be set.  Note: m_lat and m_lon are
   // ALWAYS WGS-84, reguardless of m_src_datum.
   void set_defaults();

   // Set magnetic variation, if available.  m_mag_var is left unchanged
   // if a valid magnetic variation is not found in mag_var_str.
   int set_mag_var(const char *mag_var_str);

   // Set slave variation, if available.  m_slave_var is left unchanged
   // if a valid magnetic variation is not found in m_slave_var.
   int set_slave_var(const char *slave_var_str);

   // Set elevation, if available.  m_elv and m_elv_src are left unchanged
   // if a valid elevation is not found in elv_str.
   int set_elevation(const char *elv_str, elv_src_t elv_src);

   // Set elevation, if available.  m_elv and m_elv_src are left unchanged
   // if a valid elevation is not found in elv and elv_src_str.
   int set_elevation(short elv, const char *elv_src_str);
};
// end SnapToInfo class definition

#endif