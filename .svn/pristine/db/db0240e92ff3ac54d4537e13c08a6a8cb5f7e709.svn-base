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



// refresh.cpp

#include "stdafx.h"
#include "wm_user.h"
#include "getobjpr.h"
#include "StatusBarInfoPane.h"

void FVW_update_title_bar()
{
   MapView *view = fvw_get_view();
   if (view)
      view->update_title_bar();
}

int FVW_SetHelpText(LPCSTR text)
{
   static CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(fvw_get_frame()->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));
   if (pTextInfoPane != NULL)
      pTextInfoPane->SetText(text);

   return SUCCESS;
}

int FVW_error_report(int error, void *object)
{
   if (fvw_get_frame()->PostMessage(WM_ERROR_REPORT, (WPARAM)error, (LPARAM)object))
      return SUCCESS;

   return FAILURE;
}

// Use this function to have the frame display the given message to the user.
// The input CString* must be for a CString created with new, as message
// will be deleted with delete after it is displayed.  This function posts
// a windows message to the frame that contains the given CString*.  Therefore,
// a message box is not displayed until later.
int FVW_display_error_string(CString *message)
{
   return FVW_error_report(DISPLAY_ERROR_STRING, message);
}

void FVW_add_to_recent_file_list(GUID overlayDescGuid, const char *file_specification)
{
   fvw_get_app()->add_to_recent_file_list(overlayDescGuid, file_specification);
}


boolean_t FVW_is_draw_interrupted(void)
{
   CWinApp *app = AfxGetApp();
   if (app == NULL)
      return FALSE;

   //
   // the following ensures that there is no attempt to call is_draw_interupted
   // when the view is a CPreviewView [i.e. in print preview], where the 
   // function does not exist
   //
   CFrameWndEx* frame = (CFrameWndEx*) (app->m_pMainWnd);
   if (frame == NULL || !frame->GetActiveView()->IsKindOf(RUNTIME_CLASS(MapView)))
      return FALSE;

   return fvw_get_view()->is_draw_interrupted();
}

boolean_t FVW_is_escape_pressed(void)
{
   CWinApp *app = AfxGetApp();
   if (app == NULL)
      return FALSE;

   //
   // the following ensures that there is no attempt to call is_escape_pressed
   // when the view is a CPreviewView [i.e. in print preview], where the 
   // function does not exist
   //
   CFrameWndEx* frame = (CFrameWndEx*) (app->m_pMainWnd);
   if (frame == NULL || !frame->GetActiveView()->IsKindOf(RUNTIME_CLASS(MapView)))
      return FALSE;

   return fvw_get_view()->is_escape_pressed();
}

// Used to check if the user has interrupted the draw.  This is the same as
// calling FVW_is_draw_interrupted, except that it will detect an ESC key
// press, even if the main window is not in focus.  You should only use this
// function if you want to provide a draw interrupt capability for your
// overlay when a draw is generated from a dialog box. 
boolean_t FVW_is_draw_interrupted_ex(void)
{
   //
   // the following ensures that there is no attempt to call is_draw_interupted
   // when the view is a CPreviewView [i.e. in print preview], where the 
   // function does not exist
   //
   CFrameWndEx* frame = (CFrameWndEx*) (AfxGetApp()->m_pMainWnd);
   if (!frame->GetActiveView()->IsKindOf(RUNTIME_CLASS(MapView)))
      return FALSE;

   // test for the ESC key anywhere in the main threads message queue, in
   // addition to checking for standard draw interrupt
   return fvw_get_view()->is_draw_interrupted_ex();
}

// Causes the rotation state for the view window to be re-evaluated.  If the
// map needs to be changed to North Up, it will be.  If the user's ability to
// rotate the map needs to be enabled or disabled, it will be.
void FVW_refresh_rotation_state(void)
{
   CFrameWndEx *frame = static_cast<CFrameWndEx*>(AfxGetApp()->m_pMainWnd);
   if (frame)
   {
      MapView *view = static_cast<MapView*>(frame->GetActiveView());
      if (view)
      {
         view->refresh_rotation_state();
      }
   }
}

// Causes the vie map state (map specifications) for the view window to be re-evaluated. 
// If the map needs to be changed to North Up, Equal Arc, etc., it will be. If the user's 
// ability to change the map spec needs to be enabled or disabled, it will be.
void FVW_refresh_view_map_state(void)
{
   CFrameWndEx *frame = static_cast<CFrameWndEx*>(AfxGetApp()->m_pMainWnd);
   if (frame)
   {
      MapView *view = static_cast<MapView*>(frame->GetActiveView());
      if (view)
      {
         view->refresh_view_map_state();
      }
   }
}
