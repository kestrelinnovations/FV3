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



/*------------------------------------------------------------------
-  FILE NAME:            access.c
-  LIBRARY NAME:         file
-
-  DESCRIPTION:
-
-     This file contains function(s) to determine if a file exists and what
-  permissions (read, write, execute) the file has.
-
-  PUBLIC FUNCTIONS:
-
-     FIL_access
-
-  PRIVATE FUNCTIONS: NONE
-
-  STATIC FUNCTIONS: NONE
-
-  PUBLIC VARIABLES: NONE
-
-  PRIVATE VARIABLES: NONE
-
-  REVISION HISTORY:
-       $Log: access.cpp $
//Revision 1.3  1996/03/19  21:33:11  vinny
//Added FIL_close for temp file created in FIL_access().
//
//Revision 1.2  1996/03/19  20:21:56  vinny
//Made FIL_access take into account the fact that access() always returns
//SUCCESS when testing for read or write access on Windows NT.  It still
//doesn't properly handle read access for directories (all directories
//have read and write access in NT) but it does correctly test for write
//access.
//
 * Revision 1.6  1996/03/18  16:22:41  vinny
 * Changed FIL_access so it can detect read-only access for directories.
 * 
 * Revision 1.5  1996/03/18  15:11:12  vinny
 * *** empty log message ***
 * 
 * Revision 1.4  1994/11/29  17:40:37  vinny
 * Made FIL_access return FAILURE if the path contains an empty string.
 * This was necessary under WIN 3.1 but not NT.
 * 
 * Revision 1.3  1994/11/20  15:01:54  vinny
 * Fixed spelling error.
 * 
 * Revision 1.2  1994/11/20  14:56:42  vinny
 * Added FIL_set_permissions function.
 * 
 * Revision 1.1  1994/10/26  08:52:44  gue
 * Initial revision
 * 
 * Revision 1.1  1994/02/26  11:13:09  gue
 * Initial revision
 * 
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "stdafx.h"
#include "file.h"
#include "err.h"

/*------------------------------------------------------------------
-  FUNCTION NAME:      FIL_access
-  PROGRAMMER:         Rob Gue
-  DATE:               January 1994
-
-  PURPOSE
-
-     Provide portable operations to determine if a file exists and what
-  permissions (read, write, execute) the file has.
-  
-  PARAMETERS:
-
-      path:           the file or directory name
-
-      mode:           the file permission that file will be tested for
-                      The file permission can be a single value or a 
-                      combination of one or more of the following values
-                      bitwise or'd together:
-
-                          FIL_READ_OK, FIL_WRITE_OK, FILE_EXEC_OK, FIL_EXISTS
-
-  RETURN VALUES:
-
-     SUCCESS
-     FAILURE
-
-  PRECONDITIONS: NONE
-
-  EXTERNALS MODIFIED: NONE
-
-  REQUIRED INCLUDES:
-
-      <io.h>
-      file.h
-      common.h
-
-  DESCRIPTION:
-
-      Returns SUCCESS if the file or directory exists and can be accessed
-  with the file permission given by the mode parameter. Returns FAILURE
-  otherwise.
-------------------------------------------------------------------*/

int FIL_access(const char *path, int mode)
{
   if (strlen(path) && _access(path, mode) == 0)
   {
      DWORD attributes;

      /* Use the files attributes to handle the fact that access() does not
         handle directories on Windows NT. */
      attributes = GetFileAttributes(path);
      if (attributes == -1)
      {
         ERR_report("GetFileAttributes() failed.");
         return FAILURE;
      }

      /* If the path is a file (not a directory) or it is only being tested for
         existance (not for read or write access), then the value returned by
         access() is valid. */
      if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0 || mode == FIL_EXISTS) 
         return SUCCESS;

      /* test for write access if it was requested */
      if (mode & FIL_WRITE_OK)
      {
         char test[MAX_PATH];
         int i;
         boolean_t end_with_backslash;
         FILE *tmp = NULL;

         /* detect the presence of the trailing backslash */
         if (path[strlen(path) - 1] == '\\')
            end_with_backslash = TRUE;
         else
            end_with_backslash = FALSE;

         i = 0;
         while (i < 99999999)
         {
            /* construct specification for a temporary file */
            if (end_with_backslash)
               sprintf_s(test, MAX_PATH, "%s%d.tmp", path, i);
            else
               sprintf_s(test, MAX_PATH, "%s\\%d.tmp", path, i);

            /* if the files does not already exists try to create on */
            if (_access(test, FIL_EXISTS) != 0)
            {
               /* if you can create this file then you have write access */
               fopen_s(&tmp, test, "wb");
               if (tmp != NULL)
               {
                  fclose(tmp);

                  remove(test);

                  /* assume read and execute access */
                  return SUCCESS;
               }
               else
                  return FAILURE;
            }

            i++;
         }

         return FAILURE;
      }

      /* assume read and execute access */
      return SUCCESS;
   }
   else
      return FAILURE;
}


int FIL_set_permissions(const char *filename, int permissions)
{
   long int attributes;
   
   attributes = GetFileAttributes(filename);
   if (attributes == 0xffffffff)
   {
      const int MESSAGE_LEN = 255;
      char message[MESSAGE_LEN];
      long int error;

      // get WIN32 error code generated by GetFileAttributes()
      error = GetLastError();

      // log file name and error code
      sprintf_s(message, MESSAGE_LEN, "Can't get attributes for %s\nGetLastError() == %lx", 
         filename, error);
      ERR_report(message);

      return FAILURE;
   }
   
   if ((permissions & FIL_WRITE_OK) == 0)
   {
      attributes |= FILE_ATTRIBUTE_READONLY;
      if (SetFileAttributes(filename, attributes) == FALSE)
      {
         const int MESSAGE_LEN = 255;
         char message[MESSAGE_LEN];
         long int error;

         // get WIN32 error code generated by GetFileAttributes()
         error = GetLastError();

         // log file name and error code
         sprintf_s(message, MESSAGE_LEN, "Can't set attributes for %s\nGetLastError() == %lx", 
            filename, error);
         ERR_report(message);

         return FAILURE;         
      }
   }

   if ((permissions & FIL_READ_OK) == 0)
   {
      ERR_report("All files are readable.");
      return FAILURE;
   }

   return SUCCESS;
}
