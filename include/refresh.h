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

// refresh.h

#ifndef DISPLAY_REFRESH_H
#define DISPLAY_REFRESH_H 1

#include "common.h"

// Update the main title bar - call this if the name of the current overlay
// has changed.
void FVW_update_title_bar();

int FVW_SetHelpText(LPCSTR text);

// Class to display a wait cursor and put text in the status help pane
//
class CFVWaitHelp : public CWaitCursor
{

public:
   CFVWaitHelp(LPCSTR help_text) { FVW_SetHelpText(help_text); }
   ~CFVWaitHelp() { FVW_SetHelpText(""); }
};


// This function is used to report errors directly to the top level of
// FalconView.  The first value is an error code.  The second is a pointer
// to an object of a specific type.
//
// error			            object
// COV_FILE_OUT_OF_SYNC		(DataSource *)

int FVW_error_report(int error, void *object);

// Use this function to have the frame display the given message to the user.
// The input CString* must be for a CString created with new, as message
// will be deleted with delete after it is displayed.  This function posts
// a windows message to the frame that contains the given CString*.  Therefore,
// a message box is not displayed until later.
int FVW_display_error_string(CString *message);

// Add this file to the recently open file list.
void FVW_add_to_recent_file_list(GUID overlayDescGuid, const char *file_specification);

// Used to check if the user has interrupted the draw.
boolean_t FVW_is_draw_interrupted(void);

// Used to check if the user has pressed the escape key.
boolean_t FVW_is_escape_pressed(void);

// Used to check if the user has interrupted the draw.  This is the same as
// calling FVW_is_draw_interrupted, except that it will detect an ESC key
// press, even if the main window is not in focus.  You should only use this
// function if you want to provide a draw interrupt capability for your
// overlay when a draw is generated from a dialog box. 
boolean_t FVW_is_draw_interrupted_ex(void);

// Causes the rotation state for the view window to be re-evaluated.  If the
// map needs to be changed to North Up, it will be.  If the user's ability to
// rotate the map needs to be enabled or disabled, it will be.
void FVW_refresh_rotation_state(void);

// Causes the vie map state (map specifications) for the view window to be re-evaluated. 
// If the map needs to be changed to North Up, Equal Arc, etc., it will be. If the user's 
// ability to change the map spec needs to be enabled or disabled, it will be.
void FVW_refresh_view_map_state(void);


#endif
