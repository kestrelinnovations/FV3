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
#include "err.h"

// -------------------------------------------------------------

static boolean_t windows_is_full_drag_on(void);
static boolean_t windows_nt_is_full_drag_on(void);
static boolean_t windows_nt_full_drag(HKEY hkey);
static int windows_set_full_drag(boolean_t enabled);
static int windows_nt_set_full_drag(boolean_t enabled);

// -------------------------------------------------------------

boolean_t is_full_drag_on(void)
{
   /*
    *  if 95 or NT 4.0
    */
   if (UTL_get_windows_major_version_number() >= 4)
      return windows_is_full_drag_on();
   else if (UTL_is_win32_nt())  // NT 3.51 or lower
      return windows_nt_is_full_drag_on();
   else
      return FALSE;
}

static
boolean_t windows_is_full_drag_on(void)
{
   BOOL enabled;

   if (SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &enabled, 0))
      return enabled;
   else
      return FALSE;
}

static
boolean_t windows_nt_is_full_drag_on(void)
{
   HKEY hkey = NULL;
   long result;

   result = RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0,
      KEY_READ, &hkey);

   if (result != ERROR_SUCCESS)
      return FALSE;

   if (hkey == NULL)
      return FALSE;

   boolean_t ret = windows_nt_full_drag(hkey);

   if (RegCloseKey(hkey) != ERROR_SUCCESS)
      return FALSE;

   return ret;
}

static
boolean_t windows_nt_full_drag(HKEY hkey)
{
   DWORD dwType;
   long result;
   LPSTR lpszDataBuf;
   DWORD cbData = 0;

   // Obtain the size of key
   result = RegQueryValueEx(hkey, "DragFullWindows", NULL, &dwType,
      NULL, &cbData);

   if (result != ERROR_SUCCESS)
      return FALSE;

   if (cbData == 0)
      return FALSE; 

   // allocate sufficient memory for key value
   lpszDataBuf = (LPSTR) malloc(cbData * sizeof(char));
   if (lpszDataBuf == NULL)
   {
      ERR_report("memory allocation error");
      return FALSE;
   }

   // get the key value
   result = RegQueryValueEx(hkey, "DragFullWindows", NULL, &dwType,
      (LPBYTE) lpszDataBuf, &cbData);

   if (result != ERROR_SUCCESS)
   {
      free(lpszDataBuf);
      return FALSE;
   }

   boolean_t ret_val;
   ret_val = (*lpszDataBuf == '0' ? FALSE : TRUE);

   free(lpszDataBuf);

   return ret_val;
};

// -----------------------------------------------------------

int set_full_drag(boolean_t enabled)
{
   if (UTL_get_windows_major_version_number() >= 4)
      return windows_set_full_drag(enabled);
   else if (UTL_is_win32_nt())  // NT 3.51 or lower
      return windows_nt_set_full_drag(enabled);
   else
      return FAILURE;
}

static
int windows_set_full_drag(boolean_t enabled)
{
   SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, enabled ? 1 : 0, 
      NULL, SPIF_UPDATEINIFILE);

   // note that the call may fail in 95 if they don't have Microsoft Plus!

   return SUCCESS;
}

/*
 *  Note: this will set the registry entry, but the change doesn't take place
 *  immediately
 */
static
int windows_nt_set_full_drag(boolean_t enabled)
{
   HKEY key;
   long result;

   result = RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0,
      KEY_SET_VALUE, &key);
   if (result != ERROR_SUCCESS)
      return FAILURE;

   char val[2] = {'\0', '\0',};
   val[0] = enabled ? '1' : '0';
   if (RegSetValueEx(key, "DragFullWindows", 0, REG_SZ, (unsigned char*)val, 
      2) != ERROR_SUCCESS)
   {
      return FAILURE;
   }

   return SUCCESS;
}

