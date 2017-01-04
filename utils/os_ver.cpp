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
#include "utils.h"

/////////////////////////////////////////////////////////////////////////////

class version_info
{

public:

    version_info()
    {
       OSVERSIONINFO info;

       info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
       GetVersionEx(&info);

       m_platform_id = info.dwPlatformId;
       m_major_version = info.dwMajorVersion;
       m_minor_version = info.dwMinorVersion;
    }

    DWORD get_platform_id(void) const { return m_platform_id; }
    DWORD get_major_version(void) const { return m_major_version; }
    DWORD get_minor_version(void) const { return m_minor_version; }

private:

    DWORD m_platform_id;
    DWORD m_major_version;
    DWORD m_minor_version;
};

/////////////////////////////////////////////////////////////////////////////

static version_info ver_inf;

/////////////////////////////////////////////////////////////////////////////

boolean_t UTL_is_win32s(void)
{
   return  (ver_inf.get_platform_id() == VER_PLATFORM_WIN32s ? TRUE : FALSE);
}

boolean_t UTL_is_win32_windows(void)
{
   return  (ver_inf.get_platform_id() == VER_PLATFORM_WIN32_WINDOWS ? TRUE 
      : FALSE);
}

boolean_t UTL_is_win32_nt(void)
{
   return  (ver_inf.get_platform_id() == VER_PLATFORM_WIN32_NT ? TRUE : FALSE);
}

int UTL_get_windows_major_version_number(void)
{
	return ver_inf.get_major_version();
}

int UTL_get_windows_minor_version_number(void)
{
	return ver_inf.get_minor_version();
}

//
// Windows 98 and NT 5.0 can handle multiple monitors.
// Windows 95 and NT 4.0 can't.
//
boolean_t UTL_os_is_multiple_monitor_enabled(void)
{
   if (UTL_is_win32_windows())
   {
      if (UTL_get_windows_major_version_number() >= 4 &&
         UTL_get_windows_minor_version_number() > 0)
      {
         return TRUE;
      }
   }
   else if (UTL_is_win32_nt())
   {
      if (UTL_get_windows_major_version_number() >= 5)
         return TRUE;
   }

   return FALSE;
}
