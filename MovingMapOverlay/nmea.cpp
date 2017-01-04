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

// nmea.cpp

// This file contains the implementation of the NMEA_sentence class.  The
// purpose of this class is to extract latitude, longitude, time, speed,
// heading, altitude, and number of GPS satellites from NMEA sentences.
// Only GGA, GLL, RMC, and VTG are handled by this class.  See nmea.h
// for more information on what data is in which sentence.

#include "stdafx.h"
#include "nmea.h"
#include "param.h"
#include "file.h"
#include "err.h"

// step length in meters is the distance traveled between two GPS points that
// are 1 second apart assuming a speed of 300 knots
#define STEP_LENGTH_IN_METERS 154.333333333333L

NMEA_sentence::NMEA_sentence()
{
   // initialize parsing buffer
   m_buffer = NULL;

   // initialize data fields
   m_latitude = -1000.0f;
   m_longitude = -1000.0f;
   m_speed_knots = -1.0f;
   m_speed_km_hr = -1.0f;
   m_true_heading = -1.0f;
   m_magnetic_heading = -1.0f;
   m_satellites = 0;
   m_msl = -9999.0f;
	m_horizontal_dilution_of_precision = -9999;
	m_geoid_seperation_meters = -9999;

   m_dateTime.SetStatus(COleDateTime::null);
}

// Contructor - used for building NMEA sentences
NMEA_sentence::NMEA_sentence(GPSPointIcon &point)
{
   // initialize parsing buffer
   m_buffer = NULL;

   // initialize data fields
   m_latitude = point.get_latitude();
   m_longitude = point.get_longitude();
   m_speed_knots = point.m_speed_knots;
   m_speed_km_hr = point.m_speed_km_hr;
   m_true_heading = point.m_true_heading;
   m_magnetic_heading = point.m_magnetic_heading;
   m_msl = point.m_msl;

   m_dateTime = point.m_dateTime;
}

// copy a GPSPointIcon into this NMEA_sentence object
const NMEA_sentence& NMEA_sentence::operator =(GPSPointIcon &point)
{
   // copy data fields
   m_latitude = point.get_latitude();
   m_longitude = point.get_longitude();
   m_speed_knots = point.m_speed_knots;
   m_speed_km_hr = point.m_speed_km_hr;
   m_true_heading = point.m_true_heading;
   m_magnetic_heading = point.m_magnetic_heading;
   m_msl = point.m_msl;

   m_dateTime = point.m_dateTime;

   return *this;
}


// Parses all available data from the RMC sentence to sentence into this object.
// Returns TRUE if this sentence contained a valid geographic location  (latitude and longitude). 
//
// RMC sentences contain:
//    time              FIELD 0
//    loc flag          FIELD 1
//    latitude          FIELD 2,3
//    longitude         FIELD 4,5
//    speed in knots,   FIELD 6
//    true heading      FIELD 7
//    date              FIELD 8
//    magnetic heading. FIELD 9
// RMC sentences DON'T include: 
//    altitude
//    number of satellites
//    speed in km/hr

boolean_t NMEA_sentence::process_RMC(const char *RMC_sentence)
{
   int i = 0;
   const int BUFFER_LEN = 81;
   char buffer[BUFFER_LEN];
   char *fields[11];
   char *field;
   int day, month, year;
   
   // make sure this is a RMC sentence
   if (strncmp(RMC_sentence, "$GPRMC", 6) != 0)
      return FALSE;

   // copy the sentence into a buffer for parsing
   strncpy_s(buffer, BUFFER_LEN, RMC_sentence, 80);

   i = 0;
   field = field_parse(&buffer[7]);
   while (field && i < 11)
   {
      fields[i] = field;
      i++;
      field = field_parse(NULL);
   }

   // the line is incomplete
   if (i < 11)
      return FALSE;

   // the location invalid flag is set
   if (*fields[1] == 'V')
      return FALSE;

   // latitude or longitude is missing
   if (*fields[2] == '\0' || *fields[3] == '\0' ||
      *fields[4] == '\0' || *fields[5] == '\0')
      return FALSE;

   // convert strings to latitude
   m_latitude = make_degrees(fields[2], *fields[3]);
   if (m_latitude < -90.0 || m_latitude > 90.0)
   {
      m_latitude = -1000.0f;
      return FALSE;
   }

   // convert strings to longitude
   m_longitude = make_degrees(fields[4], *fields[5]);
   if (m_longitude < -180.0 || m_longitude > 180.0)
   {
      m_longitude = -1000.0f;
      return FALSE;
   }

   // set speed if available
   if (*fields[6] != '\0')
	   sscanf_s(fields[6], "%f", &m_speed_knots);

   // set true heading if available
   if (*fields[7] != '\0')
   {
	   sscanf_s(fields[7], "%f", &m_true_heading);
	
      // compute the magnetic heading from the true heading and the
      // magnetic variation - m_magnetic_heading must be between 0.0 - 360.0
      if (*fields[9] != '\0' && *fields[10] != '\0')
      {
         float magnetic_variation;

	      sscanf_s(fields[9], "%f", &magnetic_variation); 
	      if (*fields[10] == 'E' || *fields[10] == 'e')
         {
		      m_magnetic_heading = m_true_heading - magnetic_variation;
            if (m_magnetic_heading < 0.0f)
               m_magnetic_heading += 360.0f;
         }
	      else
         {
		      m_magnetic_heading = m_true_heading + magnetic_variation;
            if (m_magnetic_heading > 360.0f)
               m_magnetic_heading -= 360.0f;
         }
      }
   }

   // convert UTC string to time in seconds
   if (*fields[0] != '\0')
   {
      float secondsToday = UTC_to_time_in_seconds(fields[0]);
      set_time(secondsToday);
   }
   // set date if available
   if (*fields[8] != '\0')
   {
      if (sscanf_s(fields[8], "%02d%02d%02d", &day, &month, &year) == 3)
         set_date(year,month,day);
   }

   return TRUE;
}


// Parses all available data from the GGA sentence to sentence into this object.
// Returns TRUE if this sentence contained a valid geographic location (latitude and longitude).
//
// GGA sentences contain:
//    time        FIELD 0
//    latitude    FIELD 1,2
//    longitude   FIELD 3,4
//    loc flag    FIELD 5
//    satellites  FIELD 6
//    altitude    FIELD 8
//    msl flag    FIELD 9

// GGA sentences DON'T include
//    speed
//    heading

boolean_t NMEA_sentence::process_GGA(const char *GGA_sentence)
{
   int i = 0;
   const int BUFFER_LEN = 81;
   char buffer[BUFFER_LEN];
   char *fields[14];
   char *field;
   
   // make sure this is a GGA sentence
   if (strncmp(GGA_sentence, "$GPGGA", 6) != 0)
      return FALSE;

   // copy the sentence into a buffer for parsing
   strncpy_s(buffer, BUFFER_LEN, GGA_sentence, 80);

   i = 0;
   field = field_parse(&buffer[7]);
   while (field && i < 14)
   {
      fields[i] = field;
      i++;
      field = field_parse(NULL);
   }

   // the line is incomplete
   if (i < 14)
      return FALSE;

   // the location invalid flag is set
   if (*fields[5] == '0')
      return FALSE;

   // latitude or longitude is missing
   if (*fields[1] == '\0' || *fields[2] == '\0' ||
      *fields[3] == '\0' || *fields[4] == '\0')
      return FALSE;

   // convert strings to latitude
   m_latitude = make_degrees(fields[1], *fields[2]);
   if (m_latitude < -90.0 || m_latitude > 90.0)
   {
      m_latitude = -1000.0f;
      return FALSE;
   }

   // convert strings to longitude
   m_longitude = make_degrees(fields[3], *fields[4]);
   if (m_longitude < -180.0 || m_longitude > 180.0)
   {
      m_longitude = -1000.0f;
      return FALSE;
   }

   // convert UTC string to time in seconds
   if (*fields[0] != '\0')
   {
      float secondsToday = UTC_to_time_in_seconds(fields[0]);
      set_time(secondsToday);
   }

   // set number of satellites if available
   if (*fields[6] != '\0')
   {
      int satellites = 0;

      sscanf_s(fields[6], "%d", &satellites);
      m_satellites = (unsigned char)satellites;
   }

	// set the horizontal dilution of precision if available
	if (*fields[7] != '\0')
		sscanf_s(fields[7], "%f", &m_horizontal_dilution_of_precision);
	else
		m_horizontal_dilution_of_precision = -9999;

   // set altitude if available (but only if there are four or more satelites
   if (m_satellites > 3 && *fields[8] != '\0' && *fields[9] == 'M')
      sscanf_s(fields[8], "%f", &m_msl);

	// set the geoid seperation (meters)
	if (*fields[10] != '\0' && *fields[11] == 'M')
		sscanf_s(fields[10], "%f", &m_geoid_seperation_meters);
	else
		m_geoid_seperation_meters = -9999;

   return TRUE;
}


// Parses all available data from the GLL sentence to sentence into this object.
// Returns TRUE if this sentence contained a valid geographic location  (latitude and longitude).
//
// GLL sentences contain:
//    latitude    FIELD 0,1
//    longitude   FIELD 2,3
//    time        FIELD 4
// GLL sentences DON'T include:
//    altitude
//    number of satellites
//    speed
//    heading 

boolean_t NMEA_sentence::process_GLL(const char *GLL_sentence)
{
   int i = 0;
   const int BUFFER_LEN = 81;
   char buffer[BUFFER_LEN];
   char *fields[6];
   char *field;
   
   // make sure this is a GLL sentence
   if (strncmp(GLL_sentence, "$GPGLL", 6) != 0)
      return FALSE;

   // copy the sentence into a buffer for parsing
   strncpy_s(buffer, BUFFER_LEN, GLL_sentence, 80);

   i = 0;
   field = field_parse(&buffer[7]);
   while (field && i < 6)
   {
      fields[i] = field;
      i++;
      field = field_parse(NULL);
   }

   // the line is incomplete
   if (i < 6)
      return FALSE;

   // the location invalid flag is set
   if (*fields[5] == 'V')
      return FALSE;

   // latitude or longitude is missing
   if (*fields[0] == '\0' || *fields[1] == '\0' ||
      *fields[2] == '\0' || *fields[3] == '\0')
      return FALSE;

   // convert strings to latitude
   m_latitude = make_degrees(fields[0], *fields[1]);
   if (m_latitude < -90.0 || m_latitude > 90.0)
   {
      m_latitude = -1000.0f;
      return FALSE;
   }

   // convert strings to longitude
   m_longitude = make_degrees(fields[2], *fields[3]);
   if (m_longitude < -180.0 || m_longitude > 180.0)
   {
      m_longitude = -1000.0f;
      return FALSE;
   }

   // convert UTC string to time in seconds
   if (*fields[4] != '\0')
   {
      float secondsToday = UTC_to_time_in_seconds(fields[4]);
      set_time(secondsToday);
   }

   return TRUE;
}



// Parses all available data from the VTG sentence to sentence into this object.
// Returns FALSE if this sentence contains no valid speed or heading information.
// Returns TRUE otherwise.
//
// VTG sentences include:
//    true heading      FIELD 0
//    true flag         FIELD 1
//    magnetic heading  FIELD 2
//    mag flag          FIELD 3
//    speed in knots    FIELD 4
//    speed flag        FIELD 5
//    speed in km/hr    FIELD 6
//    speed flag        FIELD 7
// If there is no speed
//    there can be no heading.

boolean_t NMEA_sentence::process_VTG(const char *VTG_sentence)
{
   int i = 0;
   const int BUFFER_LEN = 81;
   char buffer[BUFFER_LEN];
   char *fields[8];
   char *field;
   boolean_t valid_data = FALSE;
   
   // make sure this is a VTG sentence
   if (strncmp(VTG_sentence, "$GPVTG", 6) != 0)
      return FALSE;

   // copy the sentence into a buffer for parsing
   strncpy_s(buffer, BUFFER_LEN, VTG_sentence, 80);

   i = 0;
   field = field_parse(&buffer[7]);
   while (field && i < 8)
   {
      fields[i] = field;
      i++;
      field = field_parse(NULL);
   }

   // the line is incomplete
   if (i < 8)
      return FALSE;

   // get true heading if available
   if (*fields[0] != '\0' && *fields[1] == 'T')
   {
      sscanf_s(fields[0], "%f", &m_true_heading);
      valid_data = TRUE;
   }

   // get magnetic heading if available
   if (*fields[2] != '\0' && *fields[3] == 'M')
   {
      sscanf_s(fields[2], "%f", &m_magnetic_heading);
      valid_data = TRUE;
   }

   // get speed in knots if available
   if (*fields[4] != '\0' && *fields[5] == 'N')
   {
      sscanf_s(fields[4], "%f", &m_speed_knots);
      valid_data = TRUE;
   }

   // get speed in kilometers/hour if available
   if (*fields[6] != '\0' && *fields[7] == 'K')
   {
      sscanf_s(fields[6], "%f", & m_speed_km_hr);
      valid_data = TRUE;
   }

   return valid_data;
}


// Places all available RMC sentence data from this object into the
// given buffer, RMC_sentence.  Returns TRUE if this sentence contained a
// valid geographic location  (latitude and longitude).  Buffer length of
// 80 characters plus 1 for '\0' is required by this function.
//
// Note: RMC sentences contain latitude, longitude, time, speed in knots,
// true heading, and magnetic heading.  RMC sentences DON'T include 
// altitude, number of satellites, or speed in kilometers per hour.

boolean_t NMEA_sentence::build_RMC(char *RMC_sentence, int sentence_len)
{
   char cr = 0x0d;
   char lf = 0x0a;
   const int NMEA_EOL_LEN = 6;
   char NMEA_eol[NMEA_EOL_LEN];    // *hh<CR><LF>
   char check_sum_value;
   float lat;
   short int deg_lat;
   float min_lat;
   char dir_lat;
   float lon;
   short int deg_lon;
   float min_lon;
   char dir_lon;
   const int BUFFER_LEN = 80;
   char buffer[BUFFER_LEN];

   // valid lat/lon required
   if (m_latitude < -90.0 || m_latitude > 90.0 || m_longitude < -180.0 ||
      m_longitude > 180.0)
      return FALSE;

   // set latitude direction character and make lat a magnitude
   if (m_latitude < 0.0)
   {
      dir_lat = 'S';
      lat = -m_latitude;
   }
   else
   {
      dir_lat = 'N';
      lat = m_latitude;
   }

   // set longitude direction character and make lon a magnitude
   if (m_longitude < 0.0)
   {
      dir_lon = 'W';
      lon = -m_longitude;
   }
   else
   {
      dir_lon = 'E';
      lon = m_longitude;
   }

   // get integer degrees from latitude and longitude
   deg_lat = (short int)lat;
   deg_lon = (short int)lon;

   // get remaining minutes of latitude and longitude
   min_lat = (float)(lat - (float)deg_lat) * (float)60.0;
   min_lon = (float)(lon - (float)deg_lon) * (float)60.0;

   // all RMC sentences start with $GPRMC
   strcpy_s(RMC_sentence, sentence_len, "$GPRMC,");

   // add time if it is known
   if (valid_time())
   {
      // get time in string(HHMMSS.SS)
      time_in_seconds_to_UTC(get_time(), buffer, BUFFER_LEN);
      strcat_s(RMC_sentence, sentence_len, buffer);
   }

   // add the lat/lon information
   sprintf_s(buffer, BUFFER_LEN, ",A,%02d%06.3f,%c,%03d%06.3f,%c,",
      deg_lat, min_lat, dir_lat, deg_lon, min_lon, dir_lon);
   strcat_s(RMC_sentence, sentence_len, buffer);

   // add speed if it is known
   if (m_speed_knots != -1.0)
   {
      sprintf_s(buffer, BUFFER_LEN, "%0.2f,", m_speed_knots);
      strcat_s(RMC_sentence, sentence_len, buffer);
   }
   else
      strcat_s(RMC_sentence, sentence_len, ",");

   // add true heading if it is known
   if (m_true_heading != -1.0)
   {
      sprintf_s(buffer, BUFFER_LEN, "%0.2f,", m_true_heading);
      strcat_s(RMC_sentence, sentence_len, buffer);
   }
   else
      strcat_s(RMC_sentence, sentence_len, ",");

   // add date if it is known
   if (valid_date())
   {
      sprintf_s(buffer, BUFFER_LEN, "%02d%02d%02d,", get_day(), get_month(), get_year()%100 );
      strcat_s(RMC_sentence, sentence_len, buffer);
   }
   else
      strcat_s(RMC_sentence, sentence_len, ",");
   
   // add magnetic variation if it is known
   if (m_true_heading != -1.0 && m_magnetic_heading != -1.0)
   {
      float mag_var;

      // M - T < 0 implies easterly variation (E)
      // M - T > 0 implies westerly variation (W)
      mag_var = m_magnetic_heading - m_true_heading;
      if (mag_var < -180.0f)
         mag_var += 360.0f;
      else if (mag_var > 180.0f)
         mag_var -= 360.0f;

      if (mag_var < 0.0)         
         sprintf_s(buffer, BUFFER_LEN, "%0.2f,E", -mag_var);
      else
         sprintf_s(buffer, BUFFER_LEN, "%0.2f,W", mag_var);

      strcat_s(RMC_sentence, sentence_len, buffer);
   }
   else
      strcat_s(RMC_sentence, sentence_len, ",");

   // compute check sum
   check_sum_value = check_sum(RMC_sentence + 1, strlen(RMC_sentence) - 1);

   // make the ending of the NMEA sentence
   sprintf_s(NMEA_eol, NMEA_EOL_LEN, "*%2hX%c%c", check_sum_value, cr, lf); 

   // test string length
   if ((strlen(RMC_sentence) + 5) > MAX_NMEA_SENTENCE_LENGTH)
   {
      ERR_report("Constructed RMC sentence is too long.");
      return FALSE;
   }

   // append NMEA end of line, including the check sum
   strcat_s(RMC_sentence, sentence_len, NMEA_eol);

   return TRUE;
}


// Places all available GGA sentence data from this object into the
// given buffer, GGA_sentence.  Returns TRUE if this sentence contained a
// valid geographic location  (latitude and longitude).  Buffer length of
// 80 characters plus 1 for '\0' is required by this function.
//
// Note: GGA sentences contain latitude, longitude, time, altitude, and
// number of satellites.  GGA sentences DON'T include speed or heading
// information.

boolean_t NMEA_sentence::build_GGA(char *GGA_sentence, int sentence_len)
{
   char cr = 0x0d;
   char lf = 0x0a;
   const int NMEA_EOL_LEN = 6;
   char NMEA_eol[NMEA_EOL_LEN];    // *hh<CR><LF>
   char check_sum_value;
   float lat;
   short int deg_lat;
   float min_lat;
   char dir_lat;
   float lon;
   short int deg_lon;
   float min_lon;
   char dir_lon;
   const int BUFFER_LEN = 80;
   char buffer[BUFFER_LEN];

   // valid lat/lon required
   if (m_latitude < -90.0 || m_latitude > 90.0 || m_longitude < -180.0 ||
      m_longitude > 180.0)
      return FALSE;

   // set latitude direction character and make lat a magnitude
   if (m_latitude < 0.0)
   {
      dir_lat = 'S';
      lat = -m_latitude;
   }
   else
   {
      dir_lat = 'N';
      lat = m_latitude;
   }

   // set longitude direction character and make lon a magnitude
   if (m_longitude < 0.0)
   {
      dir_lon = 'W';
      lon = -m_longitude;
   }
   else
   {
      dir_lon = 'E';
      lon = m_longitude;
   }

   // get integer degrees from latitude and longitude
   deg_lat = (short int)lat;
   deg_lon = (short int)lon;

   // get remaining minutes of latitude and longitude
   min_lat = (float)(lat - (float)deg_lat) * (float)60.0;
   min_lon = (float)(lon - (float)deg_lon) * (float)60.0;

   // all GGA sentences start with $GPGGA
   strcpy_s(GGA_sentence, sentence_len, "$GPGGA,");

   // add time if it is known
   if (valid_time())
   {
      // get time in string(HHMMSS.SS)
      time_in_seconds_to_UTC(get_time(), buffer, BUFFER_LEN);
      strcat_s(GGA_sentence, sentence_len, buffer);
   }

   // add the lat/lon, number of satellites, and altitude information
   // altitude may be unknown

	CString horz_dop_str;
	if (m_horizontal_dilution_of_precision != -9999)
		horz_dop_str.Format("%.1f", m_horizontal_dilution_of_precision);

	CString geoid_str;
	if (m_geoid_seperation_meters != -9999)
		geoid_str.Format("%.1f,M", m_geoid_seperation_meters);
	else
		geoid_str.Format(",");
		

   //gotta have 4 satellites for GPS file reader to pay attention to altitude
   m_satellites = 4;
   if (m_msl != -9999.0f)
   {
      sprintf_s(buffer, BUFFER_LEN, ",%02d%06.3f,%c,%03d%06.3f,%c,1,%02d,%s,%0.1f,M,%s,,",
         deg_lat, min_lat, dir_lat, deg_lon, min_lon, dir_lon,
         m_satellites, horz_dop_str, m_msl, geoid_str);
   }
   else
   {
      sprintf_s(buffer, BUFFER_LEN, ",%02d%06.3f,%c,%03d%06.3f,%c,1,%02d,%s,,,%s,,",
         deg_lat, min_lat, dir_lat, deg_lon, min_lon, dir_lon,
         m_satellites, horz_dop_str, geoid_str);
   }
   strcat_s(GGA_sentence, sentence_len, buffer);

   // compute check sum
   check_sum_value = check_sum(GGA_sentence + 1, strlen(GGA_sentence) - 1);

   // make the ending of the NMEA sentence
   sprintf_s(NMEA_eol, NMEA_EOL_LEN, "*%02hX%c%c", check_sum_value, cr, lf); 

   // test string length
   if ((strlen(GGA_sentence) + 5) > MAX_NMEA_SENTENCE_LENGTH)
   {
      ERR_report("Constructed GGA sentence is too long.");
      return FALSE;
   }

   // append NMEA end of line, including the check sum
   strcat_s(GGA_sentence, sentence_len, NMEA_eol);

   return TRUE;
}

// Places all available GLL sentence data from this object into the
// given buffer, GLL_sentence.  Returns TRUE if this sentence contained a
// valid geographic location  (latitude and longitude).  Buffer length of
// 80 characters plus 1 for '\0' is required by this function.
//
// Note: GLL sentences contain latitude, longitude, and time.  GLL sentences
// DON'T include altitude, number of satellites, speed, or heading 
// information.

boolean_t NMEA_sentence::build_GLL(char *GLL_sentence)
{
   // valid lat/lon required
   if (m_latitude < -90.0 || m_latitude > 90.0 || m_longitude < -180.0 ||
      m_longitude > 180.0)
      return FALSE;

   return FALSE;
}

// Places all available GLL sentence data from this object into the
// given buffer, GLL_sentence.  Returns FALSE if this sentence contains no
// valid speed or heading information, returns TRUE otherwise.  Buffer 
// length of 80 characters plus 1 for '\0' is required by this function.
//
// Note: VTG sentences ONLY include speed in knonts, speed in km/hr, true
// heading, and magnetic heading.  If there is no speed, there can be no
// heading.

boolean_t NMEA_sentence::build_VTG(char *VTG_sentence, int sentence_len)
{
   char cr = 0x0d;
   char lf = 0x0a;
   const int NMEA_EOL_LEN = 6;
   char NMEA_eol[NMEA_EOL_LEN];    // *hh<CR><LF>
   char check_sum_value;
   const int BUFFER_LEN = 20;
   char buffer[BUFFER_LEN];

   // one of these four fields must be valid
   if (m_speed_knots == -1.0 && m_speed_km_hr == -1.0 &&
      m_true_heading == -1.0 && m_magnetic_heading == -1.0)
      return FALSE;

   // VTG sentence always starts with $GPVTG,
   strcpy_s(VTG_sentence, sentence_len, "$GPVTG,");

   // add true heading if it is available, ",," otherwise
   if (m_true_heading != -1.0)
   {
      sprintf_s(buffer, BUFFER_LEN, "%0.2f,T,", m_true_heading);
      strcat_s(VTG_sentence, sentence_len, buffer);
   }
   else
      strcat_s(VTG_sentence, sentence_len, ",,");

   // add magnetic heading if it is available, ",," otherwise
   if (m_magnetic_heading != -1.0)
   {
      sprintf_s(buffer, BUFFER_LEN, "%0.2f,M,", m_magnetic_heading);
      strcat_s(VTG_sentence, sentence_len, buffer);
   }
   else
      strcat_s(VTG_sentence, sentence_len, ",,");

   // add speed in knots if it is available, ",," otherwise
   if (m_speed_knots != -1.0)
   {
      sprintf_s(buffer, BUFFER_LEN, "%0.2f,N,", m_speed_knots);
      strcat_s(VTG_sentence, sentence_len, buffer);
   }
   else
      strcat_s(VTG_sentence, sentence_len, ",,");

   // add speed in km/hr if it is available, "," otherwise
   if (m_speed_km_hr != -1.0)
   {
      sprintf_s(buffer, BUFFER_LEN, "%0.2f,K", m_speed_km_hr);
      strcat_s(VTG_sentence, sentence_len, buffer);
   }
   else
      strcat_s(VTG_sentence, sentence_len, ",");

   // compute check sum
   check_sum_value = check_sum(VTG_sentence + 1, strlen(VTG_sentence) - 1);

   // make the ending of the NMEA sentence
   sprintf_s(NMEA_eol, NMEA_EOL_LEN, "*%2hX%c%c", check_sum_value, cr, lf); 

   // test string length
   if ((strlen(VTG_sentence) + 5) > MAX_NMEA_SENTENCE_LENGTH)
   {
      ERR_report("Constructed GGA sentence is too long.");
      return FALSE;
   }

   // append NMEA end of line, including the check sum
   strcat_s(VTG_sentence, sentence_len, NMEA_eol);

   return TRUE;
}

char NMEA_sentence::check_sum(const char *buffer, int length)
{
   int i;
   char sum = 0;

   for (i=0; i<length; i++)
      sum ^= buffer[i];

   return sum;
}

boolean_t NMEA_sentence::NMEA_test(const char *line)
{
   int length;
   char *check_sum_loc;
   short int check_sum_sent;
   char check_sum_calc;

   // all NMEA sentences start with $
   if (line[0] != '$')
   {
      TRACE("Line doesn't start with $.\n");
      return FALSE;
   }

   // NMEA sentence length is 82 characters including the delimiting $ and the
   // terminating <CR><LF>.
   length = strlen(line);
   if (length > MAX_NMEA_SENTENCE_LENGTH)
   {
      TRACE("Line too long:\n%s",line);
      return FALSE;
   }

   // test the checksum only if it is present
   check_sum_loc = const_cast<char *>(strchr(line, '*'));
   if (check_sum_loc != NULL)
   {
      // get checksum sent in sentence
      sscanf_s(check_sum_loc+1,"%hX",&check_sum_sent);

      // calculate the checksum from the data
      *check_sum_loc = '\0';
      check_sum_calc = check_sum(line+1, strlen(line)-1);
      *check_sum_loc = '*';
   
      if ((short int)check_sum_calc != check_sum_sent)
      {
         TRACE("check_sum: received=%hX calculated=%hX\n",
            check_sum_sent, (short)check_sum_calc);
         TRACE("Line: %s\n", line);
         return FALSE;
      }      
   }

   return TRUE;
}

char* NMEA_sentence::field_parse(char *buffer)
{
   char *comma;
   char *star;
   char *field;

   // buffer != NULL sets m_buffer to start of buffer to be parsed
   if (buffer != NULL)
      m_buffer = buffer;

   // m_buffer == NULL means it hasn't been initialized, or parsing is done
   // *m_buffer == '\0' means the end of the line has been reached
   if (m_buffer == NULL || *m_buffer == '\0')
      return NULL;

   // look for next comma delimiter
   comma = strchr(m_buffer, ',');
   if (comma)
   {
      *comma = '\0';
      field = m_buffer;
      m_buffer = comma + 1;
      return field;
   }

   // Look for a '*' at the end of the last data field.  It preceeds the
   // checksum, when one is present.
   star = strchr(m_buffer, '*');
   if (star)
   {
      *star = '\0';
      field = m_buffer;
      m_buffer = NULL;
      return field;
   }

   // To handle lines that do not contain a checksum, it must be assumed that
   // what remains in m_buffer is the last field from the original string of
   // comma delimited fields.
   field = m_buffer;
   m_buffer = NULL;
   return field;
}


float NMEA_sentence::make_degrees(const char *geo_s, char dir_char)
{
   const int DEGREES_S_LEN = 4;
   char degrees_s[DEGREES_S_LEN];
   int degrees;
   float minutes;
   int sign;
   int degrees_digits;

   // Number of digits of degrees is 2 for latitude and 3 for longitude.
   // The dir_char is used to determine latitude or longitude as well as
   // the sign.
   if (dir_char == 'N' || dir_char == 'n')
   {
      degrees_digits = 2;
      sign = 1;
   }
   else if (dir_char == 'S' || dir_char == 's')
   {
      degrees_digits = 2;
      sign = -1;
   }
   else if (dir_char == 'E' || dir_char == 'e')
   {
      degrees_digits = 3;
      sign = 1;
   }
   else if (dir_char == 'W' || dir_char == 'w')
   {
      degrees_digits = 3;
      sign = -1;
   }
   else
      return -1000.0f;
     
   // get degrees  from geo string
   strncpy_s(degrees_s, DEGREES_S_LEN, geo_s, degrees_digits);
   degrees_s[degrees_digits] = '\0';
   degrees = atoi(degrees_s);

   // get the minutes from the string
   sscanf_s(geo_s + degrees_digits, "%f", &minutes);
      
   // return decimal degrees
   return (sign * (degrees + minutes/60.0f));
}


float NMEA_sentence::UTC_to_time_in_seconds(const char *UTC)
{
   int hour = 0;
   int minute = 0;
   float second = 0.0f;
   float time_point;

   if (sscanf_s(UTC, "%2d%2d%f", &hour, &minute, &second) == 3)
   {
      time_point = float(hour*3600)+float(minute*60)+second;
      return time_point;
   }

   return -1.0f;
}


void NMEA_sentence::time_in_seconds_to_UTC(float time, char *UTC, int UTC_len)
{
   int hour, minute;
   float second;

   if (time == -1.0) //unknown time
      *UTC = '\0';

   hour = int(time / float(3600.0));
   minute = int((time - float(hour * 3600)) / float(60.0));
   second = time - float(hour * 3600 + minute * 60);
   if (second < 10)
      sprintf_s(UTC, UTC_len, "%02d%02d%c%02.2f", hour, minute, '0',second);
   else
      sprintf_s(UTC, UTC_len, "%02d%02d%02.2f", hour, minute, second);
}