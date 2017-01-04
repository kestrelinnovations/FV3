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
-  FILE NAME:    err.h
-  LIBRARY NAME: err.lib
-  PROGRAMMER:   Rob Gue
-  DATE:         January 1994
-
-  FUNCTIONS:
-
-      ERR_report   [macro]
-      INFO_report  [macro]
-      REPORT_COM_ERROR  [macro]
-      _ERR_report
-      _INFO_report
-      report_com_error
-      ERR_install_harderr_handler
-      ERR_set_mem_error_occurred
-      ERR_get_mem_error_occurred
-
-  REVISION HISTORY: 
-      $Log: err.h $
 * Revision 1.5  1995/03/02  17:33:00  gue
 * changed error log file name
 * 
 * Revision 1.4  1995/02/21  15:02:16  gue
 * took out ERR_printf stuff cuz it was causing some problems (through no
 * fault of its own)
 * 
 * Revision 1.3  1995/02/21  08:46:45  gue
 * added ERR_printf function.
 * 
 * Revision 1.2  1994/11/02  12:10:32  gue
 * added ERR_error_has_occurred and ERR_remove_log_if_it_has_not_been_opened.
 * 
 * Revision 1.1  1994/08/29  12:03:48  dave
 * Initial revision
 * 
 * Revision 1.5  1994/05/22  16:12:10  vinny
 * Updated FUNCTIONS section of file header.
 * 
 * Revision 1.4  1994/05/16  11:24:17  gue
 * Added #include "common.h"
 * 
 * Revision 1.3  1994/05/16  10:29:27  gue
 * Added ERR_get_mem_error_occurred and ERR_set_mem_err_occurred prototypes.
 * 
 * Revision 1.2  1994/05/13  14:47:18  gue
 * Added prototype for ERR_install_harderr_handler
 * 
 * Revision 1.1  1994/02/26  11:07:33  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

#ifndef ERR_H
#define ERR_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"

/*------------------------------------------------------------------
-                           Definitions
-------------------------------------------------------------------*/


/*------------------------------------------------------------------
-                       Function Prototypes
-------------------------------------------------------------------*/

#define ERR_report(msg)   _ERR_report(msg, __FILE__, __LINE__)
#define ERR_report_exception(msg, ex) _ERR_report_exception(msg, ex, __FILE__, __LINE__)
#define ERR_report_ole_exception(msg, ex) _ERR_report_ole_exception(msg, ex, __FILE__, __LINE__)
#define INFO_report(msg)  _INFO_report(msg, __FILE__, __LINE__)
#define REPORT_COM_ERROR(error) report_com_error(error, __FILE__, __LINE__, __FUNCTION__)

void ERR_writeStartupInfoToLog();

/*
 *  write an error to the error log
 */
void _ERR_report(const char *err_msg, const char *caller, int line);

// write an error to the error log with the given error message concated with the exception's error message
void _ERR_report_exception(const char *err_msg, CException& ex, const char *caller, int line);
void _ERR_report_ole_exception(const char *err_msg, COleException& ex, const char *caller, int line);

/*
 *  report information to the error log.  Used for testing and debugging
 *  purposes.
 */
void _INFO_report(const char *err_msg, const char *caller, int line);

// Forward declaration of _com_error used below
class _com_error;
// Report a COM error.  Uses _ERR_report().
void report_com_error(const _com_error &e, const char *filename, int line_number, const char *method);

/*
 *  set/get the memory allocation error flag
 */
void ERR_set_mem_error_occurred(boolean_t val);
boolean_t ERR_get_mem_error_occurred(void);

#endif  /* ERR_H */
