// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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


// stdafx first
#include "stdafx.h" 

// this file's header
#include "scene_mgr.h"

// system includes

// third party files

// other FalconView headers
#include "fvwutil.h"
#include "file.h"

// this project's headers
#include "scene_cov_ovl.h"


namespace scene_mgr
{

int sm_data_paths()
{
   int status = FAILURE;

   SceneCoverageOverlay *overlay = sm_get_coverage_overlay();
   if (overlay)
   {
      // TODO: This message should ultimately be removed.
      AfxMessageBox("Use the Paths tab on the Scene Data Manager dialog when the overlay is active.");
      return SUCCESS;
   }

   if (!g_sm_bDataPathsActiveMode)
   {
      // use the frame for the property sheet parent window
      CWnd *pFrame = UTL_get_frame();
      CSMSheet* pSheet;

      // Create the SM property sheet and add all pages
      if (!(pSheet = new CSMSheet("Scene Data Paths", pFrame, 0)))
         status = FAILURE;
      else
      {
         sm_setSMSheet(pSheet);

         DWORD dwStyle = WS_OVERLAPPED|WS_SYSMENU|WS_THICKFRAME|WS_CAPTION|WS_VISIBLE;
         DWORD dwExStyle = 0; //WS_EX_DLGMODALFRAME|WS_EX_MDICHILD;
         if (sm_getSMSheet()->Create(pFrame, dwStyle, dwExStyle))
            status = SUCCESS;
         else
         {
            delete sm_getSMSheet();
            status = FAILURE;
         }
      }

      if (pFrame)
         pFrame->EnableWindow(FALSE);  // disable main window so we can't tamper with anything

      g_sm_bDataPathsActiveMode = TRUE;
   }
   else  // close the modeless box...
   {
      // use the frame for the property sheet parent window
      CWnd *pFrame = UTL_get_frame();
      if (pFrame)
      {
         pFrame->EnableWindow(TRUE);   // This causes the main window to fall behind other applications...?
         pFrame->SetFocus();           // This prevents it.
      }

      // Delete the modeless dialog... 
      // NOTE: this must be done 'after' the main window is enabled!!!
      delete sm_getSMSheet();

      g_sm_bDataPathsActiveMode = FALSE;
      status = SUCCESS;
   }

   return status;
}

};  // namespace scene_mgr
