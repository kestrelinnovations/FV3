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



#include "stdafx.h"
#include "param.h"
#include "err.h"


CString PRM_GetXPlanDir()
{
	DWORD type;
   unsigned char buffer[MAX_PATH];
   DWORD buffer_size = MAX_PATH;

   CString pfps_dir;

   if (PRM_read_registry(HKEY_LOCAL_MACHINE,"Software\\XPlan\\DIRECTORIES",
      "XPlan", &type, (unsigned char*) &buffer, &buffer_size) == SUCCESS)
   {
      pfps_dir = buffer;

      // check type to see that it was a string
      if (type != REG_SZ)
		{
			ERR_report("Invalid type");
			return "";
		}
   }
   else
	{
		ERR_report("XPlan directory not set in registry");
		return "";
	}

	return pfps_dir;
}