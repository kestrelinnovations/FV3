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



//init.cpp


#include "stdafx.h"

#include "file.h"
#include "appinfo.h"
#include "param.h"
#include "err.h"

//----------------------------------------------------------------------------

static int make_directory_writeable(const char* path)
{
   const int BUFFER_LEN = 256;
   char buffer[BUFFER_LEN];

   if (FIL_access(path, 2 /*Write permission*/)!=SUCCESS)
   {
      // set the file attributes FIL_READ_OK and FIL_WRITE_OK 
      if (FIL_set_permissions(path, FIL_READ_OK | FIL_WRITE_OK))
      {
         sprintf_s(buffer,BUFFER_LEN,"You do not have write access to %s you must change "
            "the permissions in order to run "+appShortName(),path);
         AfxMessageBox(buffer);
         return FAILURE;
      }
   }
   return SUCCESS;   
}

int PRM_initialize()
{
 
   CString s;
   CString user_data;

   s = PRM_get_registry_string("Main","USER_DATA","");
   if (s.GetLength() == 0)
   {
      ERR_report("Unable to locate USER_DATA key in HKLM\\Software\\XPlan\\FalconView\\Main.");
      return FAILURE;
   }
   
   // check write access
   if (make_directory_writeable(s) !=SUCCESS)
   {
      ERR_report("make_user_data_directory_writeable() failed.");
      return FAILURE;
   }

   return SUCCESS;
}
