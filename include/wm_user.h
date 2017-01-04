// Copyright (c) 1994-2009,2013 Georgia Tech Research Corporation, Atlanta, GA
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



/*
   Filename :  WM_APP.h
   Date     :  1999-APR-27
   Author   :  Ray E. Bornert II
   Purpose  :  Global user defined windows message header file

   Comments :

   This file maintains order in the name space for user defined messages.
   All developer defined messages will be based upon WM_APP.
   Each message will be some offset from this point

   WM_APP is defined by MFC to be 0x8000.
   It has an upper limit of       0xBFFF.
   This gives us 16384 message values to work with.
   ...should be enough for now...

   Any developer working within the FalconView code should
   define any and all messages here in this header
   and then include this file  in the necessary .CPP files
   that reference the message.

   You should not include this file inside another .h file.

   Messages should have the form:

   #define MY_MESSAGE (WM_APP + 0xNNNN)

   or

   #define MY_MESSAGE (WM_APP + nnnn)

   Thank you for your support.
*/

#ifndef _WM_USER_H_
#define _WM_USER_H_

//CATALOG
// This message indicates the selection has changed in the map data path
// control.  The lParam contains the index of the newly selected entry.
//#define MDP_SELECTION_CHANGED     (WM_APP + 0x0100)
//MAP DATA MANAGER
#define MDMLC_SELECTION_CHANGED      (WM_APP+1110) //selection has changed
#define MDMLC_ICON_CLICK             (WM_APP+1111) //icon has been clicked
#define MDMLC_HEADER_SIZED           (WM_APP+1112) //column has been sized
#define MDM_COVERAGE_CHANGED         (WM_APP+1113) //coverage has been updated

//COMM
#define WM_COM_TEXT               (WM_APP + 0x0200)
#define WM_COM_BINARY             (WM_APP + 0x0201)

//ECHUM
// Custom messages sent to the ECHUM Import dialog
// from the dialog itself as well as the worker thread
#define WM_START_PARSING          (WM_APP + 0x0400)
#define WM_PARSING_COMPLETE       (WM_APP + 0x0401)
#define WM_UPDATE_PROGRESS        (WM_APP + 0x0402)

//GPS
#define WM_GPS_UFO_UPDATE         (WM_APP + 0x0500)
#define WM_USER_UPDATE            (WM_APP + 0x0501)
#define WM_STATUS_BAR_UPDATE      (WM_APP + 0x0502)
#define WM_MOVING_MAP_POINT_ADDED (WM_APP + 0x0503)

//OVERLAY
#define WM_INVALIDATE_FROM_THREAD (WM_APP + 0x0600)
#define WM_UPDATE_TITLEBAR  (WM_APP + 0x0602)
#define WM_INVALIDATE_VIEW  (WM_APP + 0x0604)
#define WM_INVALIDATE_LAYER_OVERLAY  (WM_APP + 0x0605)

// SIMULATION (AUTOMATION)
#define WM_SIMULATOR_TOOLBAR_EVENT  (WM_APP + 0x0640)

//REFRESH
#define WM_ERROR_REPORT           (WM_APP + 0x0700)
#define WM_CLOSE_INFORMATION_DIALOG (WM_APP + 0x0701)

//CUSTOM TOOLBAR MODAL CHILD APPLICATION
#define WM_CHILD_CREATED            (WM_APP + 0x0A00)
#define WM_FIND_CHILD_WINDOW        (WM_APP + 0x0A01)
#define WM_CHILD_NOT_CREATED        (WM_APP + 0x0A02)
#define WM_CHILD_TERMINATED         (WM_APP + 0x0A03)

//CHUM
#define ID_ENTER_CHUM_EDIT_MODE   (WM_APP + 0x0B00)

//LOCALPNT
#define ID_DISPLAY_LOCALPNT_ICON_DLG (WM_APP + 0x0C00)
#define ID_TREE_CHECKED (WM_APP + 0x0C01)

// Page Layout
#define WM_INSERT_PRINT_MENU_ITEMS (WM_APP + 0x0D00)
#define WM_REMOVE_PRINT_MENU_ITEMS (WM_APP + 0x0D01)

// NITF overlay
#define WM_NITF_UTILITY_THREAD_EVENT (WM_APP + 0x0D80)

// Mission Binder dialog
#define WM_FILE_STATE_CHANGED (WM_APP + 0x0E00)

// Favorites list
#define WM_FAVORITES_DIRECTORY_CHANGED (WM_APP + 0x0E01)

// Vertical Profile Tool
#define WM_UPDATE_VERTICAL_PROFILE (WM_APP + 0x0F00)


#define WM_MOMENTARY_BUTTON_PRESSED (WM_APP + 0x0F80)
#define WM_MOMENTARY_BUTTON_RELEASED (WM_APP + 0x0F81)

#endif
/////////////////////////////////////////////////////////



