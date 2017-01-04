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



/************************************************************************************
*																					*
*	MODULE:		appinfo.cpp															*
*																					*
*	PURPOSE:		Returns information about the specific project build for the	*
*					currently executing application.								*
*																					*
*	FUNCTIONS:																		*
*					const char* this_project_name()									*
*							returns the elaborated project name						*
*							(i.e., "FalconView for Windows")						*
*																					*
*					void project_version(&int, &int, &int, &int)					*
*							returns the build version numbers						*
*																					*
************************************************************************************/

#include "stdafx.h"
#include "..\..\build.h"
#include "param.h"
#include "getobjpr.h"   // fvw_get_app()
#include "StrSafe.h"
#include "fvw\appname.h"
#include "appinfo.h"


//=============================================================================
//       H E L P E R      F U N C T I O N S                                   =
//=============================================================================

/****************************************************************************************
*	FUNCTION:	get_Version_Resources(resourceName)										*
*																						*
*	PURPOSE:		Retrieves the version string associated with "resourceName" 		*
*					from the version resource of the currently running executable.		*
*																						*
****************************************************************************************/
char* get_Version_Resources(char* resourceName, char* resourceValue, size_t size)
{
	//
	// Get the executable's full pathname
	//
	char	appExeName[_MAX_PATH];
	GetModuleFileName(NULL, appExeName, sizeof(appExeName));

	DWORD   dwVerHnd=0;			// An 'ignored' parameter, always '0'
	DWORD   dwVerInfoSize;		// Size of version information block
	BOOL    bRetCode;
	UINT    uVersionLen;

	//
	// Get the Version Info Block
	//
	dwVerInfoSize = GetFileVersionInfoSize(appExeName, &dwVerHnd);
	if (dwVerInfoSize) {
		LPSTR   lpstrVffInfo;
		HANDLE  hMem;
		char    szGetValQuery[256];
		LPSTR   lpVersionInfo;			// String pointer to 'version' text

		//
		// Allocate a buffer to hold the version info block
		//
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpstrVffInfo  = (LPSTR)GlobalLock(hMem);

		//
		// Grab the version info block into blocked pointed to by lpstrVffInfo
		//
		bRetCode = GetFileVersionInfo(appExeName, dwVerHnd, dwVerInfoSize, lpstrVffInfo);

		//
		// Per the requirements for this field, when specifying a specific
		// version string to return, you pass a query string of the
		// format: "\StringFileInfo\" followed by the language specific
		// block header structure id. In the case of the FalconView/SOF-Map
		// project, this is always "English, Unicode" (040904B0). To
		// find this string, open the resource file containing the version
		// information block, the hex string required is found in the
		// block header.
		//
		StringCchCopy(szGetValQuery, 256, "\\StringFileInfo\\040904b0\\");

		// Get the requested version string.
		StringCchCat(szGetValQuery, 256, resourceName);
		bRetCode = VerQueryValue(
			(LPVOID)lpstrVffInfo,
			(LPSTR)szGetValQuery,
			(LPVOID*)&lpVersionInfo,
			(UINT *)&uVersionLen);
		StringCchCopy(resourceValue, size, lpVersionInfo);

		GlobalUnlock(hMem);
		GlobalFree(hMem);

		return resourceValue;
	}


	return NULL;
}




//=============================================================================
//       I M P L E M E N T A T I O N      F U N C T I O N S                   =
//=============================================================================

/****************************************************************************************
*	FUNCTION:	project_product_name()													*
*																						*
*	PURPOSE:		This function returns the version string ProductName defined in		*
*					the version resource file.for this project.							*
*																						*
****************************************************************************************/
char* project_product_name(char* target_str, size_t size)
{
	get_Version_Resources("ProductName", target_str, size);
	return target_str;
}



/************************************************************************************
*	FUNCTION:	project_version_str()												*
*																					*
*	PURPOSE:		This function returns a formatted string in "target_str"		*
*					and the number of consumed characters as the function return.	*
*																					*
************************************************************************************/
char* project_version_str(char* target_str, size_t size)
{
	get_Version_Resources("ProductVersion", target_str, size);
	return target_str;
}



/************************************************************************************
*	FUNCTION:	project_short_version_str()											*
*																					*
*	PURPOSE:		This function returns a formatted string in "target_str"		*
*					and truncates the string after the Major.Minor.Maintenance    	*
*					portion.														*
*																					*
************************************************************************************/
char* project_short_version_str(char* target_str, size_t size)
{
   char* pStr;

	if (get_Version_Resources("ProductVersion", target_str, size) == NULL)
      return "3.1.1";

   // This returns Major.Minor.Maintenance without build info.
   pStr = target_str;

   // skip the first period
   while (!ispunct(*pStr))
      pStr++;
   pStr++;
   // skip the second period
   while (!ispunct(*pStr))
      pStr++;
   pStr++;
   // find the third period
   while (!ispunct(*pStr))
      pStr++;
   if (ispunct(*pStr))
      *pStr = '\0';

	return target_str;
}



/************************************************************************************
*	FUNCTION:	project_copyright()													*
*																					*
*	PURPOSE:		This function returns the copyright string from the 			*
*					version resource.												*
*																					*
************************************************************************************/
char* project_copyright(char* target_str, size_t size)
{
	get_Version_Resources("LegalCopyright", target_str, size);
	return target_str;
}



/********************************************************************************
*	FUNCTION:	project_build_type()											*
*																				*
*	PURPOSE:		This function returns the "type" of the build				*
*					(alpha, development, ...) 									*
*																				*
********************************************************************************/
char* project_build_type()
{
	switch (PROJECT_BUILD_TYPE)
   {
   case 'd':
      return "Development";
   case 'a':
      return "Alpha";
   case 'b':
      return "Beta";
   case 'r':
      return "Release";
   case 'p':
      return "Phase 0";
   case 'i':
      return "Phase 0c";
   };
	return NULL;
}



// returns the elaborated project name (i.e., "FalconView for Windows")
CString appShortName()
{
	return APP_SHORT_NAME;
}

// returns the application error file
CString appErrorFile()
{
	return APP_ERROR_FILE;
}

/********************************************************************************
*	FUNCTION:	project_build_type()											*
*																				*
*	PURPOSE:		This function returns the "type" of the build				*
*					(alpha, development, ...) 									*
*																				*
********************************************************************************/
BOOL need_to_display_non_release_warning_dialog()
{
   static boolean_t already_calced_need_to = FALSE;
	static BOOL need_to;

	if (already_calced_need_to)
		return need_to;

   // if interface is hidden the message will not be shown
   if (fvw_get_app()->is_gui_hidden())
	{
		need_to = FALSE;
		already_calced_need_to = TRUE;
      return FALSE;
	}

   char version_str[80];
   if (get_Version_Resources("ProductVersion", version_str, 80))
   {
      int major, minor, maintenance;
      CString value;

      if (sscanf_s(version_str, "%d.%d.%d.%*d", &major, &minor, &maintenance) == 3)
         value.Format("Beta%d%d%d", major, minor, maintenance);

   	// get the beta flag from the PFPS Configuration section of the registry
	   CString beta_flag = PRM_get_registry_string("Configuration", value);

		// MATT_FIX: added IsEmpty() to prevent access violation in 98
   	if (!beta_flag.IsEmpty() && beta_flag.CompareNoCase("OFF") == 0)
		{
			need_to = FALSE;
			already_calced_need_to = TRUE;
   		return FALSE;
		}

	   // return true if build type !=0
      need_to = (strcmp(project_build_type(), "Release") != 0);
		already_calced_need_to = TRUE;
		return need_to;
   }

	need_to = FALSE;
	already_calced_need_to = TRUE;
   return FALSE;
}

//returns the build date of the exe
COleDateTime get_app_build_date()
{
	//COleDateTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec );
	COleDateTime build_date(PROJECT_BUILD_YEAR, PROJECT_BUILD_MONTH, PROJECT_BUILD_DAY, 0,0,0);
	return build_date;
}

