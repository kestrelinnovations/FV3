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



//registry.cpp

#include "stdafx.h"

#include "param.h"
#include "maps.h"
#include "proj.h"
#include "err.h"
#include "FvCore\Include\Registry.h"

// for the registry security stuff
#include <Aclapi.h>


CString FormatErrMsg(LPCSTR strFunction, LPCSTR strKey, LPCSTR strValue = NULL)
{
   CString errmsg = strFunction;
   errmsg += " failed on";
   if (strValue != NULL)
   {
      errmsg += " value '";
      errmsg += strValue;
      errmsg += "' in";
   }
   errmsg += " key ";
   errmsg += strKey;

   return errmsg;
}


//----------------------------------------------------------------------------
int PRM_write_registry(HKEY root_key, const char* sub_key, 
   const char* value_name, DWORD type, const BYTE* storage_loc, DWORD storage_size)
{
   return reg::write_registry(root_key, sub_key, value_name, type, storage_loc, storage_size);
}   

//----------------------------------------------------------------------------

// Note: failure may occur simply because the value_name does not exist
int PRM_read_registry(HKEY root_key, const char* sub_key, const char* value_name,
   DWORD* type, BYTE* storage_loc, DWORD* storage_size)
{   
   return reg::read_registry(root_key, sub_key, value_name, type, storage_loc, storage_size);
}

//----------------------------------------
//returns the root key designated for the section
HKEY get_root_key_for_section(const char* section)
{
   if (_strnicmp(section,"Main", 4) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section,"Route Server", 12) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "User Defaults", 13) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "Configuration", 13) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "Database Settings", 17) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "ECHUM_QUERY_DATES", 17) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "Executables", 11) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "Custom Tools",12) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "3D_viewer", 9) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "GPS Feeds", 9) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "Client Editors", 14) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "Map Options", 11) == 0)
      return HKEY_LOCAL_MACHINE;

   if (_strnicmp(section, "MapDll", 6) == 0)
      return HKEY_LOCAL_MACHINE;

   return HKEY_CURRENT_USER;
}

//----------------------------------------
CString PRM_get_sub_key(const char* section)   
{
   CString sub_key;

   // Configuration contains PFPS values - all other are application specific
   if (strcmp(section, "Configuration") == 0 || _strnicmp(section, "Database Settings", 17) == 0)
      sub_key = "Software\\XPlan\\";
   else 
      sub_key = "Software\\XPlan\\FalconView\\";

   sub_key += section;

   return sub_key;
}

//----------------------------------------------------------------------------
CString PRM_get_registry_string(const char* section, const char* value_name, 
   const char* default_value /*= NULL*/)
{
   return reg::get_registry_string(section, value_name, default_value).c_str();
}


//----------------------------------------------------------------------------
int PRM_get_registry_int(const char* section, const char* value_name, 
   int default_value /*= 0*/)
{
   return reg::get_registry_int(section, value_name, default_value);
}

//----------------------------------------------------------------------------
float PRM_get_registry_float(const char* section, const char* value_name, float default_value /*= 0.0*/)
{
   return reg::get_registry_float(section, value_name, default_value);
}

//----------------------------------------------------------------------------
double PRM_get_registry_double(const char* section, const char* value_name, double default_value /*= 0.0*/)
{
   return reg::get_registry_double(section, value_name, default_value);
}


//----------------------------------------------------------------------------
int PRM_get_registry_binary(const char* section, const char* value_name, BYTE* storage_loc, DWORD* storage_size)
{
   return reg::get_registry_binary(section, value_name, storage_loc, storage_size);
}


//----------------------------------------------------------------------------
CRect PRM_get_registry_crect(const char* section, const char* value_name, CRect default_value)
{
   CRect result;
   DWORD type;
   int buffer[4];
   DWORD buffer_size = sizeof(buffer);
   HKEY root_key = get_root_key_for_section(section);
   CString sub_key = PRM_get_sub_key(section);

   if (PRM_read_registry(root_key, sub_key, value_name, &type, (BYTE*) &buffer, &buffer_size) == SUCCESS)
   {
      result = CRect(buffer[0], buffer[1], buffer[2], buffer[3]);
      //check type to see that it was an int
      if (type != REG_BINARY)
         INFO_report(FormatErrMsg("Registry type == REG_BINARY", sub_key, value_name));
      // why isn't this an error like all the others??? Curt.
   }
   else
      result = default_value;
   
   return result;
}

//----------------------------------------------------------------------------
int PRM_set_registry_int(const char* section, const char* value_name, int value)
{
   return reg::set_registry_int(section, value_name, value);
}

//----------------------------------------------------------------------------
int PRM_set_registry_float(const char* section, const char* value_name, float value)
{
   return reg::set_registry_float(section, value_name, value);
}

//----------------------------------------------------------------------------
int PRM_set_registry_double(const char* section, const char* value_name,
                           double value)
{
   return reg::set_registry_double(section, value_name, value);
}

//----------------------------------------------------------------------------
boolean_t PRM_set_registry_string(const char* section, 
   const char* value_name, const char* value)
{
   return reg::set_registry_string(section, value_name, value);
}

//----------------------------------------------------------------------------
int PRM_set_registry_binary(const char* section, const char* value_name, const BYTE* storage_loc, DWORD storage_size)
{
   return reg::set_registry_binary(section, value_name, storage_loc, storage_size);
}

//----------------------------------------------------------------------------
int PRM_set_registry_crect(const char* section, const char* value_name, CRect value)
{
   int crect_block[4];

   HKEY root_key = get_root_key_for_section(section);
   CString sub_key = PRM_get_sub_key(section);
   
   crect_block[0] = value.TopLeft().x;
   crect_block[1] = value.TopLeft().y;
   crect_block[2] = value.BottomRight().x;
   crect_block[3] = value.BottomRight().y;

   return PRM_write_registry(root_key, sub_key, value_name, REG_BINARY, (const BYTE*) &crect_block, sizeof(crect_block)); 
}

MapSpec PRM_get_registry_map_spec(const char* section)
{
   MapSpec mapSpec;
   mapSpec.source = MAP_get_registry_source(section, "Source", TIROS); 
   mapSpec.scale = MAP_get_registry_scale(section, "Scale", WORLD);
   mapSpec.series = MAP_get_registry_series(section, "Series", "TopoBath");
   mapSpec.projection_type = MAP_get_registry_projection(section, "Projection", PROJ_get_default_projection_type());
   CString value = PRM_get_registry_string(section, "Latitude", "0.0");
   sscanf_s(value, "%lf", &mapSpec.center.lat);
   value = PRM_get_registry_string(section, "Longitude", "0.0");
   sscanf_s(value, "%lf", &mapSpec.center.lon); 
   mapSpec.rotation = 0.0;
   mapSpec.zoom_percent = 100;
   mapSpec.m_dBrightness = PRM_get_registry_double(section, "Brightness");
   mapSpec.m_dContrast = PRM_get_registry_double(section, "Contrast");
   mapSpec.m_nContrastMidval = PRM_get_registry_int(section, "ContrastMidval", 35);

   return mapSpec;
}

int PRM_set_registry_map_spec(const char* section, MapSpec mapSpec)
{
   MAP_set_registry_source(section, "Source", mapSpec.source);
   MAP_set_registry_scale(section, "Scale", mapSpec.scale);
   MAP_set_registry_series(section, "Series", mapSpec.series);
   MAP_set_registry_projection(section, "Projection", mapSpec.projection_type);

   CString value;
   value.Format("%lf", mapSpec.center.lat);
   PRM_set_registry_string(section, "Latitude", value);
   value.Format("%lf", mapSpec.center.lon);
   PRM_set_registry_string(section, "Longitude", value);

   PRM_set_registry_double(section, "Brightness", mapSpec.m_dBrightness);
   PRM_set_registry_double(section, "Contrast", mapSpec.m_dContrast);
   PRM_set_registry_int(section, "ContrastMidval", mapSpec.m_nContrastMidval);

   return SUCCESS;
}

//----------------------------------------------------------------------------
int PRM_delete_registry_section(const char* section)
{
   return reg::delete_registry_section(section);
}

//----------------------------------------------------------------------------
int PRM_delete_registry_entry(const char* section, const char* value)
{
   return reg::delete_registry_entry(section, value);
}

// Deletes all values under "section"
// This is preferred to deleting the section & recreating it because it does not alter the permission settings

int PRM_delete_registry_values(const char* section)
{
   HKEY hRootKey = get_root_key_for_section(section);
   CString strSubKey = PRM_get_sub_key(section);

   HKEY hSubKey;

   if (RegOpenKeyEx( hRootKey,
      strSubKey,  // subkey name
      NULL,  // reserved
      KEY_ALL_ACCESS,  // security access mask
      &hSubKey  // handle to open key
      ) != ERROR_SUCCESS)
   {
      // if the registry key doesn't exist there is nothing to do
      return SUCCESS;
   }

   char strValueName[MAX_PATH];

   while (TRUE)
   {
      DWORD dwNameSize = sizeof(strValueName);

      if (RegEnumValue(hSubKey,
         0,  // index of value to query
         strValueName,  // value buffer
         &dwNameSize,  // size of value buffer
         NULL,  // reserved
         NULL,  // type buffer
         NULL,  // data buffer
         NULL  // size of data buffer
         ) != ERROR_SUCCESS)
         break;

      if (RegDeleteValue(hSubKey,  // handle to key
         strValueName  // value name
         ) != ERROR_SUCCESS)
      {
         ERR_report( FormatErrMsg("RegDeleteValue", strSubKey));
         return FAILURE;
      }
   }

   if (RegCloseKey(hSubKey) != ERROR_SUCCESS)
      return FAILURE;

   return SUCCESS;
}


//----------------------------------------------------------------------------
boolean_t PRM_test_string(const char* section, const char* value_name, 
   const char* test_value)
{
   int result = FAILURE;
   HKEY root_key;
   CString sub_key;
   unsigned char buffer[256];
   DWORD buffer_size = 256;
   DWORD type;

   root_key = get_root_key_for_section(section);
   sub_key = PRM_get_sub_key(section);

   if (PRM_read_registry(root_key, sub_key, value_name, &type, (unsigned char*) &buffer, &buffer_size) == SUCCESS)
   {
      // make sure the type is correct
      if (type == REG_SZ)
         return (strcmp((const char*)buffer, test_value) == 0);

      ERR_report(FormatErrMsg("Registry type == REG_SZ", sub_key, value_name));
   }

   return FALSE;
}


// This function returns the current user value if the key is found under
// HKEY_CURRENT_USER.  If the value is not found under HKEY_CURRENT_USER
// then the value under HKEY_LOCAL_MACHINE will be returned as the default.
// If neither value is present in the registry this function will return an
// empty string.
CString PRM_get_registry_string_local_machine_default(const char* section, const char* value_name)
{
   CString result;

   // try reading the value under HKEY_CURRENT_USER
   result = PRM_get_registry_string(section, value_name);

   // if it isn't there try looking under HKEY_LOCAL_MACHINE
   if (result.IsEmpty())
   {
      CString user_defaults("User Defaults\\");

      user_defaults += section;
      result = PRM_get_registry_string(user_defaults, value_name);
   }

   return result;
}

// This function returns the current user value if the key is found under
// HKEY_CURRENT_USER.  If the value is not found under HKEY_CURRENT_USER,
// then the value under HKEY_LOCAL_MACHINE will be returned as the default.
// If neither value is present in the registry this function will return -1.
int PRM_get_registry_int_local_machine_default(const char* section, const char* value_name)
{
   return -1;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// This method will set group "Everyone" to have "full control" access to the registry
// key specified by hParentKey & lpSubKey.  All other security settings are overwritten.
// Example:  SetRegKeySecurity(HKEY_LOCAL_MACHINE, "SOFTWARE\\PFPS\\FalconView\\Data Sources");
DWORD PRM_set_registry_key_security_full_control(HKEY hParentKey, LPCTSTR lpSubKey)
{
   return reg::set_registry_key_security_full_control(hParentKey, lpSubKey);
}