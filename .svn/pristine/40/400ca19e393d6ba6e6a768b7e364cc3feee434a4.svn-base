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



//
// NOTE: most of this was cut-and-paste from the err library
//

#include "stdafx.h"
#include "appinfo.h"
#include "utils.h"

#define TIMING_FILE_NAME  "timing_log.txt"

/* 
 *  indicates whether init() has been called 
 */
static boolean_t initialized=FALSE;

/*------------------------------------------------------------------ -  
                  Static Function Prototypes
-------------------------------------------------------------------*/

static int init(void);
static int get_full_path_log_name(CString& name);
static FILE* open_log_file(const char* mode);

/////////////////////////////////////////////////////////////////////////////

/*
 *  Try to open the timing log according to the path in the command line.
 *  If that doesn't work, try opening it in the current directory.
 */
static
FILE* open_log_file(const char* mode)
{
   FILE* fp = NULL;

   CString timing_log_name;
   if (get_full_path_log_name(timing_log_name) != SUCCESS)
   {
      return NULL;
   }

   fopen_s(&fp, timing_log_name, mode);

   if (fp == NULL)
   {
      fopen_s(&fp, TIMING_FILE_NAME, mode);
   }

   return fp;
}

/////////////////////////////////////////////////////////////////////////////

static int get_full_path_log_name(CString& name)
{
   static boolean_t initialized = FALSE;
   static char full_path[_MAX_PATH + 1];

   if (initialized != TRUE)
   {
      ZeroMemory( full_path, sizeof(full_path) );
      ExpandEnvironmentStrings( "%APPDATA%", full_path, MAX_PATH );
      PathAppend(full_path, "MissionPlanning\\");
      PathAppend(full_path, TIMING_FILE_NAME);

      initialized = TRUE;
   }

   name = full_path;

   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

static
int init(void)
{
   time_t now;   /* the current time */
   FILE* timing_log_fp;


   timing_log_fp = open_log_file("w");
   if (timing_log_fp == NULL)
      return FAILURE;

   /*
    *  Write the version number and the time at which the error log was opened.
    */
   now = time(NULL);
   char ver[256];
   project_version_str(ver, 256);
   TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
   const int TIME_BUF = 200;
   char time[TIME_BUF];
   ctime_s(time, TIME_BUF, &now);
   char *context;
   char *token = strtok_s(time, "\n", &context);
   fprintf(timing_log_fp, "%s version %s timing log started %s:\n", 
      appShortName(), ver, token);

   /*
    *  Write platform information if possible
    */
   OSVERSIONINFO info;
   info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   if (GetVersionEx(&info) != 0)
   {
      DWORD build_num;
      CString os;
      if (info.dwPlatformId == VER_PLATFORM_WIN32s)
      {
         os = "Win32s";
         build_num = info.dwBuildNumber;
      }
      else if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
      {
         os = "Windows";
         build_num = LOWORD(info.dwBuildNumber);
      }
      else if (info.dwPlatformId == VER_PLATFORM_WIN32_NT)
      {
         os = "Windows NT";
         build_num = info.dwBuildNumber;
      }
 
      fprintf(timing_log_fp, "\n%s version %d.%d build %d %s\n", 
         (const char*) os, info.dwMajorVersion, info.dwMinorVersion, 
         build_num, info.szCSDVersion);
   }

   fprintf(timing_log_fp, "\n");

   fclose(timing_log_fp);

   initialized = TRUE;

   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

void UTL_timing_report(const char* str)
{
   if (!UTL_timing_on())
      return;

   FILE* timing_log_fp;

   if (!initialized)
   {
      if (init() != SUCCESS)
         return;
   }

   timing_log_fp = open_log_file("a");
   if (timing_log_fp == NULL)
      return;

   fprintf(timing_log_fp, "%s\n", str);
   fclose(timing_log_fp);
}

/////////////////////////////////////////////////////////////////////////////

int UTL_remove_timing_log_if_it_has_not_been_opened(void)
{
   if (initialized == FALSE)
   {
      CString timing_log_name;
      if (get_full_path_log_name(timing_log_name) != SUCCESS)
         return FAILURE;

      // if an old error log file already exists, backup the file and remove it
      if (_access(timing_log_name, 00) == 0)
      {
         CString backup(timing_log_name);
         int ext;

         // the backup file will have the same name and path as the error log
         // with the extension .bak
         ext = backup.ReverseFind('_');
         if (ext != -1)
            backup = timing_log_name.Left(ext);
         backup += "_bak.txt";

         // remove old backup file if it already exists
         remove(backup);

         // save old error log into backup file
         rename(timing_log_name, backup);
      }
   }

   return SUCCESS;
}
