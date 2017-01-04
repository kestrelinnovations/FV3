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
-  FILE NAME:          file.h
-  LIBRARY NAME:       file
-  PROGRAMMER:         Rob Gue
-  DATE:               January 1994
-
-  DESCRIPTION:
-
-     This file contains definitions and function prototypes for the file
-  library.
-
-  FUNCTIONS:
-
-      FIL_access
-      FIL_cd_installed
-      FIL_cd_changed
-      FIL_get_int_4
-      FIL_get_int_2
-      FIL_get_int_1
-      FIL_mget_int_2
-      FIL_open  [macro]
-      FIL_close [macro]
-      _FIL_open
-      _FIL_close
-      FIL_get_free_bytes_on_disk
-      FIL_get_drive_block_size
-      FIL_get_disk_space_for_file
-
-  REVISION HISTORY: 
-      $Log: file.h $
 * Revision 1.15  1997/04/22  14:31:00  barnes
 * tweaked FIL_delete_all_files_in_a_dir to take a spec
 * Revision 1.14  1996/03/20  10:10:45  kevin
 * removed commented out code
 * 
 * Revision 1.13  1996/03/19  12:34:32  kevin
 * converted to c++
 * 
 * Revision 1.12  1996/03/18  11:42:06  kevin
 * added DirectoryPath class
 * 
 * Revision 1.11  1996/03/14  19:00:37  vinny
 * FIL_get_recur_dir_size, FIL_get_dir_size, FIL_get_free_bytes_on_disk, and
 * FIL_get_drive_block_size were changed to support Universal Naming
 * Convention (UNC) path names.  FIL_get_drive_block_size and
 * FIL_get_free_bytes_on_disk now take a const char *root_path instead of
 * a char drive_letter.
 * 
 * Revision 1.10  1996/01/20  19:45:22  vinny
 * Removed FIL_cd_change() from the file library.
 * 
 * Revision 1.9  1996/01/19  14:50:43  vinny
 * Changed FIL_create_directory to take a const char * instead of a char *.
 * 
 * Revision 1.8  1995/05/15  12:06:04  gue
 * added FIL_is_dir_empty
 * 
 * Revision 1.7  1995/05/01  10:15:43  gue
 * - remove FIL_is_dir_empty
 * - changed FIL_get_volume_id_for_drive to FIL_get_volume_info_for_drive,
 *   which now also passes back a serial number for the drive
 * 
 * Revision 1.6  1995/01/12  10:31:55  gue
 * CHange in FIL_cd_changed prototype.
 * 
 * Revision 1.5  1995/01/10  15:21:13  dave
 * removed stat.h header
 * 
 * Revision 1.4  1995/01/10  12:05:39  gue
 * Added param to FIL_cd_changed
 * 
 * Revision 1.3  1994/11/20  15:39:22  vinny
 * Added FIL_set_permissions() and FIL_create_directory().
 * 
 * Revision 1.2  1994/11/16  10:20:30  gue
 * Added comment.
 * 
 * Revision 1.1  1994/08/29  12:03:49  dave
 * Initial revision
 * 
 * Revision 1.6  1994/05/21  12:33:14  gue
 * ADded FIL_get_volume_id_for_drive.
 * 
 * Revision 1.5  1994/05/16  13:37:25  gue
 * Added prototype for FIL_is_dir_empty
 * 
 * Revision 1.4  1994/05/10  15:12:42  gue
 * Added prototype for FIL_getcwd
 * 
 * Revision 1.3  1994/05/10  14:01:40  gue
 * Added FIL_delete_all_files_in_a_dir.
 * 
 * Revision 1.2  1994/02/28  15:28:23  dave
 * added FIL_cd_installed and FIL_cd_changed prototypes
 * 
 * Revision 1.1  1994/02/26  11:17:16  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

#ifndef FILE_H
#define FILE_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include <stdio.h>
#include <io.h>
#include <direct.h>
#include "common.h"

/*------------------------------------------------------------------
-                           Definitions
-------------------------------------------------------------------*/

/*
 *  values for specifying the mode for FIL_access().
 *
 *  <mode> can either be a single value or it can be a combination of one or
 *  more of the values bitwise or'ed together
*/

/*
 *  Note: there is no equivalent for FIL_EXEC_OK
 */
#define FIL_READ_OK   4
#define FIL_WRITE_OK  2
#define FIL_EXISTS    0

#define FIL_ATTR_READONLY    _A_RDONLY

/*------------------------------------------------------------------
-                       Function Prototypes 
-------------------------------------------------------------------*/

/*
 *  determine whether the given file or directory can be accessed with the
 *  given permissions.  Returns SUCCESS or FAILURE.
 */
int FIL_access(const char *path, int mode);

/* set the file attributes FIL_READ_OK and FIL_WRITE_OK */
int FIL_set_permissions(const char *filename, int permissions);

/* creates the given directory */

int FIL_create_directory(const char *directory);

#endif 
