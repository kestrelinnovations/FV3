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
-  FILE NAME:    graph.cpp
-  LIBRARY NAME: graphics.lib 
-
-  DESCRIPTION:
-
-       These functions are graphics library routines which access the
-  local variables private to this file.
-
-----------------------------------------------------------------*/


/*------------------------------------------------------------------
-                            Includes
- -----------------------------------------------------------------*/

#include "stdafx.h"
#include "clt.h"
#include "errx.h"

#include <gdiplus.h>

/*------------------------------------------------------------------
-  FUNCTION NAME: GRA_open_graphics
-
-  PURPOSE:
-
-      Do all initialization for the graphics library routines.
-
-  EXTERNALS MODIFIED:
-
-      mode                             - set to graphics_mode upon SUCCESS 
-
-  DESCRIPTION:
-
-      Sets up the graphics mode according to graphics_mode, initializes
-  all of the graphics library constants, and sets up the hardware
-  color lookup table. This function must return SUCCESS before any
-  other GRA_*() functions are called.
- -----------------------------------------------------------------*/

ULONG_PTR s_gdiplusToken;

int GRA_open_graphics(void)
{
   int return_code;
   int i=0;

   /* enable color functions and initialize the hardware clt */
   return_code = gra_open_clt();

   if (return_code != SUCCESS)
   {
      ERR_report("gra_open_clt() failed."); 
      return FAILURE;
   }

   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   Gdiplus::GdiplusStartup(&s_gdiplusToken, &gdiplusStartupInput, NULL);

   return SUCCESS;
}

void GRA_shutdown_graphics()
{
   Gdiplus::GdiplusShutdown(s_gdiplusToken);
}

