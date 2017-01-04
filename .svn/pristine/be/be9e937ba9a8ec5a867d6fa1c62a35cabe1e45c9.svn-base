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



/*------------------------------------------------------------------
-  FILE NAME:         err.c
-  LIBRARY NAME:      err
-
-  DESCRIPTION:    
-
-      This file contains all of the functions for reporting errors to the 
-  error log.  The error log is opened upon the first call to _ERR_report, 
-  _ERR_fatal, or _INFO_report.
-
-  _ERR_report, _INFO_report are meant to be called using the 
-  macros ERR_report, ERR_fatal and INFO_report defined in err.h.
-
-  PUBLIC FUNCTIONS:  
-
-      _ERR_report
-      _INFO_report
-      ERR_get_mem_error_occurred
-      ERR_set_mem_error_occurred
-
-  PRIVATE FUNCTIONS: NONE
-
-  STATIC FUNCTIONS:  
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
-

//
// *** NOTE *** 
//
// It is very important that the error library doesn't use the memory library.
// If it did, you could get in a vicious cycle as follows:
//
// - mem lib gets error, calls ERR_report
// - error lib tries to allocate memory with the memory lib while 
//   reporting error
// - mem lib gets error, calls ERR_report
// ad nauseum
//
// The same applies for the file library.
//
// Basically the error library can not rely on any other modules that might
// call any error lib functions.
//

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "stdafx.h"
#include "err.h"
#include "appinfo.h"
#include "Common\ComErrorObject.h"

/*------------------------------------------------------------------
-                         Static Variables
-------------------------------------------------------------------*/

const int ERROR_MSG_BUFFER_SIZE = 1024;

/*
 *  mem_err_occurred indicates if a memory allocation error occured.
 *  This flag can be polled to see if a memory allocation error occurred so
 *  that an appropriate message can be displayed to the user.  The flag 
 *  should then be set back to FALSE.
 */
static boolean_t mem_error_occurred=FALSE;

static boolean_t err_has_occurred=FALSE;

void ERR_writeStartupInfoToLog()
{
   std::stringstream ssStartup;

   /*
    *  Write the version number and the time at which FalconView was startup
    */
   char ver[256];
   project_version_str(ver, 256);

   ssStartup <<"*** Starting " << (LPCTSTR)appShortName() << " version " << ver << ".";

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

      ssStartup << "  " << (LPCTSTR)os << " version ";
      ssStartup << info.dwMajorVersion << "." << info.dwMinorVersion;
      ssStartup << " build " << build_num << " " << info.szCSDVersion << "." ;
   }

   ssStartup << std::ends;


   WriteToLogFile(_bstr_t("*"));
   WriteToLogFile(_bstr_t(ssStartup.str().c_str()));
   WriteToLogFile(_bstr_t("*"));
}

/*------------------------------------------------------------------
-  FUNCTION NAME:       _ERR_report
-  PROGRAMMER:          Rob Gue
-  DATE:                January 1994
-
-  PURPOSE: 
-
-      To write error messages to the log, along with the date, time, file 
-  and line number at which the error occured.
-
-  PARAMETERS:
-
-      err msg:         the string containing the error message
-
-      caller:          the name of the file from which the call to ERR_report
-                       was made
-
-      caller_line:     the line number in the file containing the calling
-                       function where the call to ERR_report was made
-
-  RETURN VALUES: NONE
-
-  PRECONDITIONS: NONE
-
-  EXTERNALS MODIFIED: NONE
-
-  REQUIRED INCLUDES: 
-
-      <time.h>
-      <stdio.h>
-      <stdlib.h>
-      <string.h>
-      err.h
-      mouse.h
-      graphics.h
-
-  DESCRIPTION:
-
-      _ERR_report is meant to be called through the macro ERR_report
-  defined in "err.h".
-
-      Creates the error log if it hasn't been opened.
-      Writes an error message to the error log, along with the date, 
-  time, file and line number at which it occured.
-------------------------------------------------------------------*/
void _ERR_report(const char *err_msg, const char *caller, int caller_line)
{
   DWORD syserr = GetLastError();

   std::stringstream ssError;

   CString strErrorMessage(err_msg);
   strErrorMessage.Replace("\n", " / ");

   ssError << "ERROR: " << caller << " line " << caller_line << ".  ";
   ssError << (LPCTSTR)strErrorMessage;

   // is there a system error code?
   if (syserr)
   {
      LPVOID lpMsgBuf;
      DWORD rc = FormatMessage( 
                              FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                              FORMAT_MESSAGE_FROM_SYSTEM | 
                              FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL,
                              syserr,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                              (LPTSTR) &lpMsgBuf,
                              0,
                              NULL);

      if (rc)
      {
         ssError << ".  System error code - " << (LPCTSTR)lpMsgBuf << ".";
         LocalFree( lpMsgBuf );
      }
      else
         ssError << ".  System error code - Unable to format error message.";
   }

   ssError << std::ends;

   WriteToLogFile(_bstr_t(ssError.str().c_str()));

   err_has_occurred = TRUE;

   // clear the system error because our file operations above may have set it
   SetLastError(ERROR_SUCCESS);

#if _DEBUG

   std::stringstream debug_msg;

   debug_msg << "DEBUG ERR_report\n" << caller << " line " << caller_line <<
      " :\n\n" << err_msg << "\n";

	if (CComErrorObject::DebugMessageBox(_CRT_ERROR, debug_msg.str().c_str() ) == 1)
      __debugbreak();		// LOOK up the stack!
#endif
}

void _ERR_report_ole_exception(const char *err_msg, COleException& ex, const char *caller, int line)
{
   // DISP_E_UNKNOWNNAME is a common error.  We'll mask this particular error
   if (ex.m_sc == DISP_E_UNKNOWNNAME)
      return;

   _ERR_report_exception(err_msg, ex, caller, line);
}

// write an error to the error log with the given error message concated with the exception's error message
void _ERR_report_exception(const char *err_msg, CException& ex, const char *caller, int line)
{
   // get the exception's error message
   char szError[ERROR_MSG_BUFFER_SIZE];
   ex.GetErrorMessage(szError, ERROR_MSG_BUFFER_SIZE);
   
   // append the exception's error message to the given msg
   //
   CString msg(err_msg);

   // add a separator to the given message only if it is non-empty
   if (msg.GetLength() > 0)
      msg += " - ";

   msg += szError;

   // write it to the log
   _ERR_report(msg, caller, line);
}


/*------------------------------------------------------------------
-  FUNCTION NAME:        _INFO_report
-  PROGRAMMER:           Rob Gue
-  DATE:                 January 1994
-
-  PURPOSE:  
-
-      To help developers distinguish between error messages and messages 
-  intended as information (e.g. for debugging and testing).
-
-  PARAMETERS:
-
-       err_msg:         the string containing the information message
-
-       caller:          the name of the file from which the call to INFO_report
-                        is being made
-
-       caller_line:     the line number in the file containing the calling 
-                        function where the call to INFO_report was made
-
-  RETURN VALUES: NONE
-
-  PRECONDITIONS: NONE
-
-  EXTERNALS MODIFIED: NONE
-
-  REQUIRED INCLUDES:  
-
-      <stdlib.h>
-      <stdio.h>
-      <time.h>
-      mouse.h
-      graphics.h
-
-  DESCRIPTION:
-
-      _INFO_report is meant to be called through the macro INFO_report
-  defined in "err.h".
-
-      Creates the error log if it hasn't been opened.
-      Writes an information message to the error log, along with the date, 
-  time, file and line number at which it occured.
-------------------------------------------------------------------*/

void _INFO_report(const char *err_msg, const char *caller, int caller_line)
{
   std::stringstream ssInfo;

   CString strInfo(err_msg);
   strInfo.Replace("\n", " / ");

   ssInfo << "INFO: " << caller << " line " << caller_line << ".  ";
   ssInfo << (LPCTSTR)strInfo;
   ssInfo << std::ends;

   WriteToLogFile(_bstr_t(ssInfo.str().c_str()));

#ifdef _DEBUG
   CString trace_str;
   trace_str.Format("INFO: %s %1d: %s\n",
      caller, caller_line, err_msg);
   TRACE(trace_str);
#endif

}

void report_com_error(const _com_error &e, const char *filename, int line_number, const char *method)
{
   _bstr_t source(e.Source());
   _bstr_t description(e.Description());

   CString text;
   text.Format(
      "(%s) A COM error was caught near line %d of file %s:\n"
      "   code: %x\n"
      "   code meaning: %s\n"
      "   source: %s\n"
      "   description: %s",
      method,
      line_number,
      filename,
      e.Error(),
      e.ErrorMessage(),
      static_cast<LPCTSTR>(source),
      static_cast<LPCTSTR>(description));

   _ERR_report(text, filename, line_number);
}

boolean_t ERR_get_mem_error_occurred(void)
{
   return mem_error_occurred;
}

void ERR_set_mem_error_occurred(boolean_t val)
{
   mem_error_occurred = val;
}