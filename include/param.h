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



#pragma once

/*------------------------------------------------------------------
-  FILE NAME:      param.h
-  LIBRARY NAME:   param
-  PROGRAMMER:     Rob Gue
-  DATE:           January 1994
-
-  DESCRIPTION:
-
-      This contains definitions and prototypes for the parameter module.
-
-  FUNCTIONS:
-
-      PRM_process_ini_file(void);
-      PRM_set_value(const char *var_name, const char *value);
-      PRM_get_value(const char *var_name, char *value);
-
-  REVISION HISTORY: 

 [deleted]
 
 * 
 * Revision 1.1  1994/02/26  13:14:44  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

#ifndef PARAM_H
#define PARAM_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/
#include "common.h"
#include "maps_d.h"
/*------------------------------------------------------------------
-                           Definitions
-------------------------------------------------------------------*/

  /*
   *  The maximum length of a parameter value string
   */
#define PRM_MAX_VALUE_LENGTH 128


/*
 *  the following are defines for variable names that can be in the 
 *  initiazation file
 */
#define PRM_HD_DATA                  "HD_DATA"

/*------------------------------------------------------------------
-                       Function Prototypes 
-------------------------------------------------------------------*/

/*
 *  Get the value of a parameter.
 *
 *  This is done by copying the variable value into the <value> parameter. 
 *  Therefore <value> must be at least PRM_MAX_VALUE_LENGTH+1 chars long.
 */
int PRM_get_value(const char *var_name, char *value, int value_len);

int PRM_initialize();


//-------------------------------------------------------
// REGISTRY FUNCTIONS 
//
// These functions read and write registry values at the section,
// and value_name given.  The section name is relative the registry
// root key under HKEY_CURRENT_USER for most sections, e.g.,
// "HKEY_CURRENT_USER\Software\PFPS\FalconView\" + section is the
// registry key.  A small number of special section names indicate
// that the value is under HKEY_LOCAL_MACHINE.  These parameters are
// machine specific instead of user specific, and they are stored
// under the registry root key under HKEY_LOCAL_MACHINE, e.g.,
// "HKEY_LOCAL_MACHINE\SOFTWARE\PFPS\FalconView\" + section is the
// registry key.
//
// Here is a list of the HKEY_LOCAL_MACHINE sections:
//
// Main
// Data Sources
// Route Server
//

//analogous to CWinApp::GetProfileString ???
CString PRM_get_sub_key(const char* section);

// analogous to CWinApp::GetProfileString.
CString PRM_get_registry_string(const char* section, const char* value_name, 
   const char* default_value = NULL);

//analogous to CWinApp::WriteProfileString except returns SUCCESS/FAILURE
int PRM_set_registry_string(const char* section, 
   const char* value_name, const char* value);

//analogous to CWinApp::GetRegistryString except default = 0
int PRM_get_registry_int(const char* section, const char* value_name, 
   int default_value = 0);

//analogous to CWinApp::WriteProfileInt except returns SUCCESS/FAILURE
int PRM_set_registry_int(const char* section, const char* value_name, 
   int value);

float PRM_get_registry_float(const char* section, const char* value_name,
                           float default_value = 0.0);

int PRM_set_registry_float(const char* section, const char* value_name,
                           float value);

double PRM_get_registry_double(const char* section, const char* value_name,
                           double default_value = 0.0);

int PRM_set_registry_double(const char* section, const char* value_name,
                           double value);

int PRM_get_registry_binary(const char* section, const char* value_name, 
   BYTE* storage_loc, DWORD* storage_size);

CRect PRM_get_registry_crect(const char* section, const char* value_name, 
   CRect default_value);

int PRM_set_registry_binary(const char* section, const char* value_name, 
   const BYTE* storage_loc, DWORD storage_size);

int PRM_set_registry_crect(const char* section, const char* value_name, 
   CRect value);

MapSpec PRM_get_registry_map_spec(const char* section);
int PRM_set_registry_map_spec(const char* section, MapSpec mapSpec);

int PRM_read_registry(HKEY root_key, const char* sub_key, 
   const char* value_name, DWORD* type, BYTE* storage_loc, 
   DWORD* storage_size);

int PRM_write_registry(HKEY root_key, const char* sub_key, 
   const char* value_name, DWORD type, const BYTE* storage_loc, 
   DWORD storage_size);

int PRM_delete_registry_section(const char* section);

int PRM_delete_registry_entry(const char* section, const char* value);

int PRM_delete_registry_values(const char* section);

boolean_t PRM_test_string(const char* section, const char* value_name, 
   const char* test_value);

// The above functions are generally used for values that have static 
// default values, i.e., they can be hard coded as part of the function
// call because they are the same for every user.  In certain cases the
// default value will be a function of aircraft, mission, or some other
// parameter that requires the default value to be a variable stored
// in the registry.  In these cases the default value will be stored
// under "HKEY_LOCAL_MACHINE\SOFTWARE\PFPS\FalconView\User Defaults", e.g.,
// the default value for section = "Aircraft", value = "Category" would
// stored in "Aircraft\Category" under
// "HKEY_LOCAL_MACHINE\SOFTWARE\PFPS\FalconView\User Defaults\", while
// the user specific value will stored in "Aircraft\Category" under
// "HKEY_CURRENT_USER\Software\PFPS\FalconView\".
//

// This function returns the current user value if the key is found under
// HKEY_CURRENT_USER.  If the value is not found under HKEY_CURRENT_USER
// then the value under HKEY_LOCAL_MACHINE will be returned as the default.
// If neither value is present in the registry this function will return an
// empty string.
CString PRM_get_registry_string_local_machine_default(const char* section, 
   const char* value_name);

// This function returns the current user value if the key is found under
// HKEY_CURRENT_USER.  If the value is not found under HKEY_CURRENT_USER,
// then the value under HKEY_LOCAL_MACHINE will be returned as the default.
// If neither value is present in the registry this function will return -1.
int PRM_get_registry_int_local_machine_default(const char* section, 
   const char* value_name);

// This method will set group "Everyone" to have "full control" access to the registry
// key specified by hParentKey & lpSubKey.  All other security settings are overwritten.
// Example: PRM_set_registry_key_security_full_control(HKEY_LOCAL_MACHINE, 
//					"SOFTWARE\\PFPS\\FalconView\\Data Sources");
DWORD PRM_set_registry_key_security_full_control(HKEY hParentKey, LPCTSTR lpSubKey);

// implemented in common_registry_values.cpp
CString PRM_GetXPlanDir();

#endif  /* PARAM_H */





