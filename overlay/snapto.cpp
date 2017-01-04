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



// snap_to.cpp

#include "stdafx.h"

#include "snapto.h"
#include "err.h"
#include "Common\ComErrorObject.h"

SnapToInfo::SnapToInfo()
{
   set_defaults();
}

/* static */
SnapToInfo* SnapToInfo::CreateFromSnapToPointsList(ISnapToPointsList* pSnapToPointsList)
{
   SnapToInfo *info = new SnapToInfo;

   ISnapToPointsListPtr smpSnapToPointsList = pSnapToPointsList;

   info->m_lat = smpSnapToPointsList->m_dLatitude;
   info->m_lon = smpSnapToPointsList->m_dLongitude;
   strncpy_s(info->m_src_datum, 6, (char *)smpSnapToPointsList->m_bstrSourceDatum, 6);
   info->m_elv = smpSnapToPointsList->m_sElevationFeet;
   info->m_elv_src = static_cast<SnapToInfo::elv_src_t>(smpSnapToPointsList->m_eElevationSource);
   info->m_mag_var = static_cast<float>(smpSnapToPointsList->m_dMagVar);
   info->m_slave_var = static_cast<float>(smpSnapToPointsList->m_dSlaveVar);
   info->m_pnt_src_rs = static_cast<SnapToInfo::pnt_src_rs_t>(smpSnapToPointsList->m_sPointSource);
   info->m_area_code = ((char *)smpSnapToPointsList->m_bstrAreaCode)[0];
   info->m_nav_type = ((char *)smpSnapToPointsList->m_bstrNavaidType)[0];
   strncpy_s(info->m_dtd_id, 9, (char *)smpSnapToPointsList->m_bstrDtdId, 9);
   strncpy_s(info->m_fix_rs, 13, (char *)smpSnapToPointsList->m_bstrRSFixPoint, 13);
   strncpy_s(info->m_description_rs, 41, (char *)smpSnapToPointsList->m_bstrRSDescription, 40);
   strncpy_s(info->m_country_code, 3, (char *)smpSnapToPointsList->m_bstrCountryCode, 3);
   info->m_qlt = ((char *)smpSnapToPointsList->m_bstrQuality)[0];
   info->m_cls = ((char *)smpSnapToPointsList->m_bstrClassification)[0];
   info->m_nav_freq_MHz = static_cast<float>(smpSnapToPointsList->m_dNavaidFreqMHz);
   info->m_nav_chan = static_cast<float>(smpSnapToPointsList->m_dNavaidChannel);
   info->m_horz_accuracy = static_cast<float>(smpSnapToPointsList->m_dHorzAccuracyFeet);
   info->m_vert_accuracy = static_cast<float>(smpSnapToPointsList->m_dVertAccuracyFeet);
   strncpy_s(info->m_db_lookup_rs, 41, (char *)smpSnapToPointsList->m_bstrRSDBLookup, 40);
   strncpy_s(info->m_db_lookup_filename_rs, 261, (char *)smpSnapToPointsList->m_bstrDBLookupFilename, 260);
   strncpy_s(info->m_icon_type, 11, (char *)smpSnapToPointsList->m_bstrIconType, 10);
   strncpy_s(info->m_icon_description, 51, (char *)smpSnapToPointsList->m_bstrIconDescription, 50);

   // retrieve the extended properties used by the point export tool and the IMap::GetSnapToInfo* automation methods
   ISnapToPointsList2Ptr spSnapToPointsList2 = smpSnapToPointsList;
   if (spSnapToPointsList2 != NULL)
   {
      info->m_strKey = (char *)spSnapToPointsList2->m_bstrKey;
      info->m_strTooltip = (char *)spSnapToPointsList2->m_bstrTooltip;
      info->m_overlayDescGuid = spSnapToPointsList2->Getm_overlayDescGuid();
      info->m_strOverlayName = (char *)spSnapToPointsList2->m_bstrOverlayName;
   }

   return info;
}

ISnapToPointsList* SnapToInfo::ConvertToSnapToPointsList()
{
   ISnapToPointsListPtr spSnapToPointsList;
   CO_CREATE(spSnapToPointsList, CLSID_SnapToPointsList);

   ISnapToPointsList2Ptr spSnapToPointsList2 = spSnapToPointsList;

   spSnapToPointsList->m_dLatitude = m_lat;
   spSnapToPointsList->m_dLongitude = m_lon;
   spSnapToPointsList->m_bstrSourceDatum = _bstr_t(m_src_datum);
   spSnapToPointsList->m_sElevationFeet = m_elv;
   spSnapToPointsList->m_eElevationSource = static_cast<ElevationSourceEnum>(m_elv_src);
   spSnapToPointsList->m_dMagVar = m_mag_var;
   spSnapToPointsList->m_dSlaveVar = m_slave_var;
   spSnapToPointsList->m_sPointSource = m_pnt_src_rs;
   spSnapToPointsList->m_bstrAreaCode = _bstr_t(&m_area_code);
   spSnapToPointsList->m_bstrNavaidType = _bstr_t(&m_nav_type);
   spSnapToPointsList->m_bstrDtdId = _bstr_t(m_dtd_id);
   spSnapToPointsList->m_bstrRSFixPoint = _bstr_t(m_fix_rs);
   spSnapToPointsList->m_bstrRSDescription = _bstr_t(m_description_rs);
   spSnapToPointsList->m_bstrCountryCode = _bstr_t(m_country_code);
   spSnapToPointsList->m_bstrQuality = _bstr_t(&m_qlt);
   spSnapToPointsList->m_bstrClassification = _bstr_t(&m_cls);
   spSnapToPointsList->m_dNavaidFreqMHz = m_nav_freq_MHz;
   spSnapToPointsList->m_dNavaidChannel = m_nav_chan;
   spSnapToPointsList->m_dHorzAccuracyFeet = m_horz_accuracy; 
   spSnapToPointsList->m_dVertAccuracyFeet = m_vert_accuracy; 
   spSnapToPointsList->m_bstrRSDBLookup = _bstr_t(m_db_lookup_rs);
   spSnapToPointsList->m_bstrDBLookupFilename = _bstr_t(m_db_lookup_filename_rs);
   spSnapToPointsList->m_bstrIconType = _bstr_t(m_icon_type);
   spSnapToPointsList->m_bstrIconDescription = _bstr_t(m_icon_description);

   spSnapToPointsList2->m_bstrKey = _bstr_t(m_strKey);
   spSnapToPointsList2->m_bstrTooltip = _bstr_t(m_strTooltip);
   spSnapToPointsList2->Putm_overlayDescGuid(m_overlayDescGuid);
   spSnapToPointsList2->m_bstrOverlayName = _bstr_t(m_strOverlayName);

   spSnapToPointsList->AddToList();
   spSnapToPointsList->MoveFirst();

   return spSnapToPointsList.Detach();
}

// Sets all fields to default values.  Latitude and longitude must be
// set after this call.  WGS-84 datum is assumed.  If a non-WGS-84 source
// datum is used, m_src_datum must be set.  Note: m_lat and m_lon are
// ALWAYS WGS-84, reguardless of m_src_datum.
void SnapToInfo::set_defaults()
{
   // Required data fields
	m_lat = -90.0;
	m_lon = -180.0;

 	strncpy_s(m_src_datum,6,"WGS84",5);       // Required
   m_src_datum[6] = '\0';

   // optional data fields
   m_elv_src = ELV_UNKNOWN;      // Unknown Elevation
	m_mag_var = (float)180.0;     // Unknown Magnetic Variation
   m_slave_var = (float)180.0;   // Unknown Slave Variation
   m_pnt_src_rs = USER_SRC;          // user input point
   m_area_code = ' ';            // first letter of ICAO code
   m_nav_type = ' ';             // ('T', 'V', 'N', 'G', 'M', 'R', 'E', 'B', 'A')
	m_qlt = '0';
	m_cls = 'U';
   m_horz_accuracy = -1.0;
   m_vert_accuracy = -1.0;
}

// Set magnetic variation, if available.  m_mag_var is left unchanged
// if a valid magnetic variation is not found in mag_var_str.
int SnapToInfo::set_mag_var(const char *mag_var_str)
{
   int degrees;      // number of degrees
   float minutes_10; // 10 * the number of minutes
   char direction;   // E/W direction character
   int count;

   // parse the magnitude and direction of the magnetic variation
   count = sscanf_s(mag_var_str, "%c%03d%f", &direction, 1, &degrees, &minutes_10);

   // if direction and magnitude are not there, forget about it
   if (count < 3)
      return SUCCESS;

   // must be between 0 and 179 degrees
   if (degrees < 0 || degrees > 179)
   {
      ERR_report("Degrees out of range.");
      return FAILURE;
   }

   // must be between 0.0 and 59.9 minutes
   if (minutes_10 < 0.0 || minutes_10 > 599.0)
   {
      ERR_report("Minutes out of range.");
      return FAILURE;
   }

   // use the input direction (E/W)
   if (direction == 'E' || direction == 'e')
      m_mag_var = (float)degrees + minutes_10 / (float)600.0;
   else if (direction == 'W' || direction == 'w')
      m_mag_var = -((float)degrees + minutes_10 / (float)600.0);
   else
   {
      ERR_report("Invalid direction character.");
      return FAILURE;
   }

   return SUCCESS;
}

// Set slave variation, if available.  m_slave_var is left unchanged
// if a valid magnetic variation is not found in m_slave_var.
int SnapToInfo::set_slave_var(const char *slave_var_str)
{
   int degrees;      // number of degrees
   char direction;   // E/W direction character
   int count;

   // parse the magnitude and direction of the magnetic variation
   count = sscanf_s(slave_var_str, "%c%03d", &direction, 1, &degrees);

   // if both direction and magnitude are not there, forget about it
   if (count < 2)
      return SUCCESS;

   // must be between 0 and 179 degrees
   if (degrees < 0 || degrees > 179)
   {
      ERR_report("Degrees out of range.");
      return FAILURE;
   }
   
   // use the input direction (E/W)
   if (direction == 'E' || direction == 'e')
      m_slave_var = (float)degrees;
   else if (direction == 'W' || direction == 'w')
      m_slave_var = -((float)degrees);
   else
   {
      ERR_report("Invalid direction character.");
      return FAILURE;
   }

   return SUCCESS;
}

// Set elevation, if available.  m_elv and m_elv_src are left unchanged
// if a valid elevation is not found in elv_str.
int SnapToInfo::set_elevation(const char *elv_str, elv_src_t elv_src)
{
   if (strlen(elv_str))
   {
      int temp = atoi(elv_str);

      if (temp < -32768 || temp > 32767)
      {
         ERR_report("Elevation out of range.");
         return FAILURE;
      }

      // since atoi returns 0 for failure, this check is needed to
      // handle a zero elevation
      if (temp != 0 || elv_str[0] == '0')
      {
         m_elv = temp;
         m_elv_src = elv_src;
      }
   }

   return SUCCESS;
}

// Set elevation, if available.  m_elv and m_elv_src are left unchanged
// if a valid elevation is not found in elv and elv_src_str.
int SnapToInfo::set_elevation(short elv, const char *elv_src_str)
{
   m_elv = elv;

   //get elevation source
   //source types in SnapToInfo
   //DAFIF, ELV_UNKNOWN, USER_DEFINED_ELV, DTED
   CString tempist = elv_src_str;
   
   if(tempist == "UNK")
   {
      m_elv_src = ELV_UNKNOWN;   
   }
   else if (tempist == "DTED")
   {
      m_elv_src = DTED;
   }
   else if (tempist == "USER")
   {
      m_elv_src = USER_DEFINED_ELV;
   }
   else if (tempist == "DAFIF")
   {
      m_elv_src = DAFIF;
   }

   return SUCCESS;
}
