// Copyright (c) 1994-2009,2012,2013 Georgia Tech Research Corporation, Atlanta, GA
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

// IUserSimulator.cpp
//

#include "StdAfx.h"
#include "IUserSimulator.h"
#include "err.h"        // for ERR_report
#include "common.h"     // for SUCCESS, FAILURE

#include "utils.h"
#include "mainfrm.h"
#include "getobjpr.h"
#include "resource.h"

#include "wm_user.h"
#include "gps.h"
#include "FvTabCtrl.h"
#include "StatusBarManager.h"
#include "statbar.h"
#include "MovingMapOverlay\factory.h"
#include "TabularEditorDlg.h"
#include "TAMask\factory.h"
#include "ZoomPercentComboBox.h"

#include <hash_map>

IMPLEMENT_DYNCREATE(UserSimulator, CCmdTarget)

IMPLEMENT_OLECREATE(UserSimulator, "FalconView.UserSimulator", 0x3B3C5FC4, 0x0591, 0x4f4e, 
                   0xB2, 0x9B, 0x5A, 0x41, 0x74, 0x08, 0x71, 0x98)

static stdext::hash_map<std::string,int>
   iddCoreLookup,       // Core FV command IDs
   iddClientLookup,     // Known client button IDs
   iddFvToolbarLookup;  // Plugin buttons

UserSimulator::UserSimulator() :
   m_mainFrame( *fvw_get_frame() )
{
   EnableAutomation();

#if 1
   static const struct LookupInfo
   {
      LPCSTR   pszKey;
      UINT     uiID;
   } lookupInfo[] =
   {
      { "MAINFRAME\nMAXIMIZE",       SC_MAXIMIZE },
      { "MAINFRAME\nRESTORE",        SC_RESTORE },
      { "MAINFRAME\nMINIMIZE",       SC_MINIMIZE },

      { "MAIN\nCURRENT_NEW",         ID_CURRENT_NEW },
      { "MAIN\nCURRENT_OPEN",        ID_CURRENT_OPEN },
      { "MAIN\nFILE_CLOSE_FV",       ID_FILE_CLOSE_FV },
      { "MAIN\nCURRENT_SAVE",        ID_CURRENT_SAVE },
      { "MAIN\nSAVE_ALL",            ID_SAVE_ALL },
      { "MAIN\nMAIN_PRINT_DIRECT",   ID_MAIN_PRINT_DIRECT },
      { "MAIN\nMAIN_PRINT_PREVIEW",  ID_MAIN_PRINT_PREVIEW },
      { "MAIN\nDIM",                 ID_DIM },
      { "MAIN\nBRIGHT",              ID_BRIGHT },
      { "MAIN\nSCALE_PERCENT_ZOOM",  ID_SCALE_PERCENT_ZOOM },
      { "MAIN\nCOMBO_PLACEHOLDER",   ID_COMBO_PLACEHOLDER },
      { "MAIN\nSCALE_ZOOM",          ID_SCALE_ZOOM },
      { "MAIN\nMAP_SCALEIN",         ID_MAP_SCALEIN },
      { "MAIN\nMAP_SCALEOUT",        ID_MAP_SCALEOUT },
      { "MAIN\nHAND_MODE",           ID_HAND_MODE },
      { "MAIN\nMAP_PAN_UP",          ID_MAP_PAN_UP },
      { "MAIN\nMAP_PAN_DOWN",        ID_MAP_PAN_DOWN },
      { "MAIN\nMAP_PAN_LEFT",        ID_MAP_PAN_LEFT },
      { "MAIN\nMAP_PAN_RIGHT",       ID_MAP_PAN_RIGHT },
      { "MAIN\nMAP_NEW",             ID_MAP_NEW },
      { "MAIN\nCENTER_CROSSHAIR",    ID_CENTER_CROSSHAIR },
      { "MAIN\nVIEWTIME",            ID_VIEWTIME },
      { "MAIN\nSEARCH_EDIT",         ID_SEARCH_EDIT },
      { "MAIN\nHELP_FINDER",         ID_HELP_FINDER },
      { "MAIN\nCONTEXT_HELP",        ID_CONTEXT_HELP },
      { "MAIN\nEXIT",                ID_APP_EXIT },

      { "Navigation\nRotate Left",   ID_MAP_ROTATE_CLOCKWISE },
      { "Navigation\nNorth Up",      ID_MAP_ROTATE_NORTHUP },
      { "Navigation\nRotate Right",  ID_MAP_ROTATE_COUNTERCLOCKWISE },
      { "Navigation\nZoom Control",  ID_ZOOM_COMBO },
      { "Navigation\nStep In",       ID_ZOOM_IN_STEP },
      { "Navigation\nStep Out",      ID_ZOOM_OUT_STEP },
      { "Navigation\nPan Up",        ID_MAP_PAN_UP },
      { "Navigation\nPan Left",      ID_MAP_PAN_LEFT },
      { "Navigation\nPan Right",     ID_MAP_PAN_RIGHT },
      { "Navigation\nPan Down",      ID_MAP_PAN_DOWN },

      { "View\nMenubar",             ID_VIEW_MENUBAR },
      { "View\nTitlebar",            ID_VIEW_TITLEBAR },
      { "View\nMap Tabs Bar",        ID_VIEW_MAP_TABS_BAR },
      { "View\nStatus Bar",          ID_VIEW_STATUS_BAR },
      { "View\nView\nStatus Bar",    ID_VIEW_STATUS_BAR },

      { "AR\nSELECT",                ID_TOOL_AR_SELECT },
      { "AR\nTRACK",                 ID_TOOL_AR_TRACK },
      { "AR\nANCHOR",                ID_TOOL_AR_ANCHOR },
      { "AR\nNAVAID",                ID_TOOL_AR_NAVAID },
      { "AR\nPOINT",                 ID_TOOL_AR_POINT },
      { "AR\nPROPERTY",              ID_AR_PROPERTY },

      { "CHUM\nTOOLS_SELECT",        ID_CHUM_TOOLS_SELECT },
      { "CHUM\nADDPNT",              ID_CHUM_ADDPNT },
      { "CHUM\nADD_POWERLINE",       ID_CHUM_ADD_POWERLINE },
      { "CHUM\nDIALOG_SWITCH",       ID_CHUM_DIALOG_SWITCH },
      { "CHUM\nDRAG_LOCK",           ID_CHUM_DRAG_LOCK },
      { "CHUM\nTABULAR_EDITOR",      ID_CHUM_TABULAR_EDITOR },

#if 0 // LocalPoints now plugin
      { "LOCAL\nSELECT",             ID_LOCAL_SELECT },
      { "LOCAL\nADD",                ID_LOCAL_ADD },
      { "LOCAL\nEDITOR_DIALOG_TOGGLE", ID_LOCAL_EDITOR_DIALOG_TOGGLE },
      { "LOCAL\nDRAG_LOCK",          ID_LOCAL_DRAG_LOCK },
      { "LOCAL\nPT_TABULAR_EDITOR",  ID_LOCAL_PT_TABULAR_EDITOR },
#endif

      { "MDM\nTOOL_COMPATIBLE",      ID_MDM_TOOL_COMPATIBLE },
      { "MDM\nTOOL_SELCOPY",         ID_MDM_TOOL_SELCOPY },
      { "MDM\nTOOL_SELDELETE",       ID_MDM_TOOL_SELDELETE },

      { "SKYVIEW\nSELECT_SYM_TOOL",  ID_SELECT_SYM_TOOL },
      { "SKYVIEW\nSKYVIEW",          ID_SKYVIEW },
      { "SKYVIEW\nCENTER_SYMBOL",    ID_CENTER_SYMBOL },
      { "SKYVIEW\nOBJECTS",          ID_SKYVIEW_OBJECTS },
      { "SKYVIEW\nOBJ_PROPERTIES",   ID_SKYVIEW_OBJ_PROPERTIES },

      { "NITF\nSELECT",              ID_NITF_SELECT },
      { "NITF\nAREA_SELECT",         ID_NITF_AREA_SELECT },
      { "NITF\nERASE_AREAS",         ID_NITF_ERASE_AREAS },
      { "NITF\nQUERY_EDITOR",        ID_NITF_QUERY_EDITOR },
      { "NITF\nMETADATA_LIST",       ID_NITF_METADATA_LIST },
      { "NITF\nLOCAL_SEARCH",        ID_NITF_LOCAL_SEARCH },
      { "NITF\nWEB_SEARCH",          ID_NITF_WEB_SEARCH },
      { "NITF\nDISPLAY_FILTER",      ID_NITF_DISPLAY_FILTER },

      { "PRINTTOOL\nALIGN_LEG",      ID_PRINTTOOL_ALIGN_LEG },
      { "PRINTTOOL\nAREA",           ID_PRINTTOOL_AREA },
      { "PRINTTOOL\nFILE_PRINT_SETUP", ID_FILE_PRINT_SETUP },
      { "PRINTTOOL\nPAGE",           ID_PRINTTOOL_PAGE },
      { "PRINTTOOL\nPRINT",          ID_PRINT_TOOL_PRINT },
      { "PRINTTOOL\nPRINT_PREVIEW",  ID_PRINT_TOOL_PRINT_PREVIEW },
      { "PRINTTOOL\nPROPERTIES",     ID_PRINT_TOOL_PROPERTIES },
      { "PRINTTOOL\nSELECT",         ID_PRINTTOOL_SELECT },
      { "PRINTTOOL\nSNAPROUTE",      ID_PRINTTOOL_SNAPROUTE },
      { "PRINTTOOL\nSTRIP",          ID_PRINTTOOL_STRIP },

      { "TOOLS\nAXIS",               ID_TOOLS_AXIS },
      { "TOOLS\nBOX_SELECT",         ID_TOOLS_BOX_SELECT },
      { "TOOLS\nBULLSEYE",           ID_TOOLS_BULLSEYE },
      { "TOOLS\nCOLOR",              ID_TOOLS_COLOR },
      { "TOOLS\nDRAG_LOCK",          ID_TOOLS_DRAG_LOCK },
      { "TOOLS\nFILL",               ID_TOOLS_FILL },
      { "TOOLS\nFREEHAND2",          ID_TOOLS_FREEHAND2 },
      { "TOOLS\nLINE",               ID_TOOLS_LINE },
      { "TOOLS\nMANUAL_EDIT",        ID_TOOLS_MANUAL_EDIT },
      { "TOOLS\nMOVE",               ID_TOOLS_MOVE },
      { "TOOLS\nOVAL",               ID_TOOLS_OVAL },
      { "TOOLS\nPICTURE",            ID_TOOLS_PICTURE },
      { "TOOLS\nPOLYGON",            ID_TOOLS_POLYGON },
      { "TOOLS\nPOLYLINE",           ID_TOOLS_POLYLINE },
      { "TOOLS\nPROPERTY_SHEET",     ID_TOOLS_PROPERTY_SHEET },
      { "TOOLS\nRECT",               ID_TOOLS_RECT },
      { "TOOLS\nSELECT",             ID_TOOLS_SELECT },
      { "TOOLS\nSTYLE",              ID_TOOLS_STYLE },
      { "TOOLS\nTABULAR_EDITOR",     ID_OVLKIT_TABULAR_EDITOR },
      { "TOOLS\nTEXT",               ID_TOOLS_TEXT },
      { "TOOLS\nWIDTH",              ID_TOOLS_WIDTH },

      { "TG\nHIERARCHY",             ID_TG_HIERARCHY },
      { "TG\nUSER_PALETTE",          ID_TG_USER_PALETTE },
      { "TG\nPROPERTIES",            ID_TG_PROPERTIES },
      { "TG\nMOVE_BUTTON",           ID_TG_MOVE_BUTTON },
      { "TG\nBOX_SELECT_BUTTON",     ID_TG_BOX_SELECT_BUTTON },
      { "TG\nSELECT_GROUP_BUTTON",   ID_TG_SELECT_GROUP_BUTTON },
      { "TG\nSELECT_ALL_BUTTON",     ID_TG_SELECT_ALL_BUTTON },
      { "TG\nSHOW_HIDDEN_BUTTON",    ID_TG_SHOW_HIDDEN_BUTTON },

      { "GPS\nCONNECT",              ID_GPS_CONNECT },
      { "GPS\nOPTIONS",              ID_GPS_OPTIONS },
      { "GPS\nLOAD_SETTINGS",        ID_MOVING_MAP_LOAD_SETTINGS },
      { "GPS\nSAVE_SETTINGS",        ID_MOVING_MAP_SAVE_SETTINGS },
      { "GPS\nAUTOCENTER",           ID_GPS_AUTOCENTER },
      { "GPS\nAUTOROTATE",           ID_GPS_AUTOROTATE },
      { "GPS\nSMOOTH_SCROLLING",     ID_GPS_SMOOTH_SCROLLING },
      { "GPS\nCYCLE_CENTER",         ID_CYCLE_CENTER },
      { "GPS\nTRAILPOINTS",          ID_GPS_TRAILPOINTS },
      { "GPS\nCLOCK",                ID_GPS_CLOCK },
      { "GPS\nTOGGLE_PREDICTIVE_PATH", ID_GPS_TOGGLE_PREDICTIVE_PATH },
      { "GPS\nTOGGLE_TAMASK",        ID_GPS_TOGGLE_TAMASK },
      { "GPS\nCDI",                  ID_GPS_CDI },
      { "GPS\nRANGE_BEARING",        ID_GPS_RANGE_BEARING },
      { "GPS\nCOAST_TRACK",          ID_GPS_COAST_TRACK },
      { "GPS\nTABULAR_EDITOR",       ID_GPS_TABULAR_EDITOR },
      { "GPS\nSTYLES",               ID_GPS_STYLES },
      { NULL }
   };

   if ( iddCoreLookup.empty() )
   {
      const LookupInfo* pInfo = lookupInfo;
      do
      {
         iddCoreLookup[ pInfo->pszKey ] = pInfo->uiID;
      } while ( (++pInfo)->pszKey != NULL );
   }
#else
   iddLookup["MAIN\n\rCURRENT_NEW"] = ID_CURRENT_NEW;
   iddLookup["MAIN\n\rCURRENT_OPEN"] = ID_CURRENT_OPEN;
   iddLookup["MAIN\n\rFILE_CLOSE_FV"] = ID_FILE_CLOSE_FV;
   iddLookup["MAIN\n\rCURRENT_SAVE"] = ID_CURRENT_SAVE;
   iddLookup["MAIN\n\rSAVE_ALL"] = ID_SAVE_ALL;
   iddLookup["MAIN\n\rMAIN_PRINT_DIRECT"] = ID_MAIN_PRINT_DIRECT;
   iddLookup["MAIN\n\rMAIN_PRINT_PREVIEW"] = ID_MAIN_PRINT_PREVIEW;
   iddLookup["MAIN\n\rDIM"] = ID_DIM;
   iddLookup["MAIN\n\rBRIGHT"] = ID_BRIGHT;
   iddLookup["MAIN\n\rSCALE_PERCENT_ZOOM"] = ID_SCALE_PERCENT_ZOOM;
   iddLookup["MAIN\n\rCOMBO_PLACEHOLDER"] = ID_COMBO_PLACEHOLDER;
   iddLookup["MAIN\n\rSCALE_ZOOM"] = ID_SCALE_ZOOM;
   iddLookup["MAIN\n\rMAP_SCALEIN"] = ID_MAP_SCALEIN;
   iddLookup["MAIN\n\rMAP_SCALEOUT"] = ID_MAP_SCALEOUT;
   iddLookup["MAIN\n\rHAND_MODE"] = ID_HAND_MODE;
   iddLookup["MAIN\n\rMAP_PAN_UP"] = ID_MAP_PAN_UP;
   iddLookup["MAIN\n\rMAP_PAN_DOWN"] = ID_MAP_PAN_DOWN;
   iddLookup["MAIN\n\rMAP_PAN_LEFT"] = ID_MAP_PAN_LEFT;
   iddLookup["MAIN\n\rMAP_PAN_RIGHT"] = ID_MAP_PAN_RIGHT;
   iddLookup["MAIN\n\rMAP_NEW"] = ID_MAP_NEW;
   iddLookup["MAIN\n\rCENTER_CROSSHAIR"] = ID_CENTER_CROSSHAIR;
   iddLookup["MAIN\n\rVIEWTIME"] = ID_VIEWTIME;
   iddLookup["MAIN\n\rHELP_FINDER"] = ID_HELP_FINDER;
   iddLookup["MAIN\n\rCONTEXT_HELP"] = ID_CONTEXT_HELP;

   iddLookup["AR\n\rSELECT"] = ID_TOOL_AR_SELECT;
   iddLookup["AR\n\rTRACK"] = ID_TOOL_AR_TRACK;
   iddLookup["AR\n\rANCHOR"] = ID_TOOL_AR_ANCHOR;
   iddLookup["AR\n\rNAVAID"] = ID_TOOL_AR_NAVAID;
   iddLookup["AR\n\rPOINT"] = ID_TOOL_AR_POINT;
   iddLookup["AR\n\rPROPERTY"] = ID_AR_PROPERTY;

   iddLookup["CHUM\n\rTOOLS_SELECT"] = ID_CHUM_TOOLS_SELECT;
   iddLookup["CHUM\n\rADDPNT"] = ID_CHUM_ADDPNT;
   iddLookup["CHUM\n\rADD_POWERLINE"] = ID_CHUM_ADD_POWERLINE;
   iddLookup["CHUM\n\rDIALOG_SWITCH"] = ID_CHUM_DIALOG_SWITCH;
   iddLookup["CHUM\n\rDRAG_LOCK"] = ID_CHUM_DRAG_LOCK;
   iddLookup["CHUM\n\rTABULAR_EDITOR"] = ID_CHUM_TABULAR_EDITOR;

   iddLookup["MDM\n\rTOOL_COMPATIBLE"] = ID_MDM_TOOL_COMPATIBLE;
   iddLookup["MDM\n\rTOOL_SELCOPY"] = ID_MDM_TOOL_SELCOPY;
   iddLookup["MDM\n\rTOOL_SELDELETE"] = ID_MDM_TOOL_SELDELETE;

   iddLookup["SKYVIEW\n\rSELECT_SYM_TOOL"] = ID_SELECT_SYM_TOOL;
   iddLookup["SKYVIEW\n\rSKYVIEW"] = ID_SKYVIEW;
   iddLookup["SKYVIEW\n\rCENTER_SYMBOL"] = ID_CENTER_SYMBOL;
   iddLookup["SKYVIEW\n\rOBJECTS"] = ID_SKYVIEW_OBJECTS;
   iddLookup["SKYVIEW\n\rOBJ_PROPERTIES"] = ID_SKYVIEW_OBJ_PROPERTIES;

   iddLookup["NITF\n\rSELECT"] = ID_NITF_SELECT;
   iddLookup["NITF\n\rAREA_SELECT"] = ID_NITF_AREA_SELECT;
   iddLookup["NITF\n\rERASE_AREAS"] = ID_NITF_ERASE_AREAS;
   iddLookup["NITF\n\rQUERY_EDITOR"] = ID_NITF_QUERY_EDITOR;
   iddLookup["NITF\n\rMETADATA_LIST"] = ID_NITF_METADATA_LIST;
   iddLookup["NITF\n\rLOCAL_SEARCH"] = ID_NITF_LOCAL_SEARCH;
   iddLookup["NITF\n\rWEB_SEARCH"] = ID_NITF_WEB_SEARCH;
   iddLookup["NITF\n\rDISPLAY_FILTER"] = ID_NITF_DISPLAY_FILTER;

   iddLookup["PRINTTOOL\n\rALIGN_LEG"] = ID_PRINTTOOL_ALIGN_LEG;
   iddLookup["PRINTTOOL\n\rAREA"] = ID_PRINTTOOL_AREA;
   iddLookup["PRINTTOOL\n\rFILE_PRINT_SETUP"] = ID_FILE_PRINT_SETUP;
   iddLookup["PRINTTOOL\n\rPAGE"] = ID_PRINTTOOL_PAGE;
   iddLookup["PRINTTOOL\n\rPRINT"] = ID_PRINT_TOOL_PRINT;
   iddLookup["PRINTTOOL\n\rPRINT_PREVIEW"] = ID_PRINT_TOOL_PRINT_PREVIEW;
   iddLookup["PRINTTOOL\n\rPROPERTIES"] = ID_PRINT_TOOL_PROPERTIES;
   iddLookup["PRINTTOOL\n\rSELECT"] = ID_PRINTTOOL_SELECT;
   iddLookup["PRINTTOOL\n\rSNAPROUTE"] = ID_PRINTTOOL_SNAPROUTE;
   iddLookup["PRINTTOOL\n\rSTRIP"] = ID_PRINTTOOL_STRIP;

   iddLookup["TOOLS\n\rAXIS"] = ID_TOOLS_AXIS;
   iddLookup["TOOLS\n\rBOX_SELECT"] = ID_TOOLS_BOX_SELECT;
   iddLookup["TOOLS\n\rBULLSEYE"] = ID_TOOLS_BULLSEYE;
   iddLookup["TOOLS\n\rCOLOR"] = ID_TOOLS_COLOR;
   iddLookup["TOOLS\n\rDRAG_LOCK"] = ID_TOOLS_DRAG_LOCK;
   iddLookup["TOOLS\n\rFILL"] = ID_TOOLS_FILL;
   iddLookup["TOOLS\n\rFREEHAND2"] = ID_TOOLS_FREEHAND2;
   iddLookup["TOOLS\n\rLINE"] = ID_TOOLS_LINE;
   iddLookup["TOOLS\n\rMANUAL_EDIT"] = ID_TOOLS_MANUAL_EDIT;
   iddLookup["TOOLS\n\rMOVE"] = ID_TOOLS_MOVE;
   iddLookup["TOOLS\n\rOVAL"] = ID_TOOLS_OVAL;
   iddLookup["TOOLS\n\rPICTURE"] = ID_TOOLS_PICTURE;
   iddLookup["TOOLS\n\rPOLYGON"] = ID_TOOLS_POLYGON;
   iddLookup["TOOLS\n\rPOLYLINE"] = ID_TOOLS_POLYLINE;
   iddLookup["TOOLS\n\rPROPERTY_SHEET"] = ID_TOOLS_PROPERTY_SHEET;
   iddLookup["TOOLS\n\rRECT"] = ID_TOOLS_RECT;
   iddLookup["TOOLS\n\rSELECT"] = ID_TOOLS_SELECT;
   iddLookup["TOOLS\n\rSTYLE"] = ID_TOOLS_STYLE;
   iddLookup["TOOLS\n\rTABULAR_EDITOR"] = ID_OVLKIT_TABULAR_EDITOR;
   iddLookup["TOOLS\n\rTEXT"] = ID_TOOLS_TEXT;
   iddLookup["TOOLS\n\rWIDTH"] = ID_TOOLS_WIDTH;

   iddLookup["TG\n\rHIERARCHY"] = ID_TG_HIERARCHY;
   iddLookup["TG\n\rUSER_PALETTE"] = ID_TG_USER_PALETTE;
   iddLookup["TG\n\rPROPERTIES"] = ID_TG_PROPERTIES;
   iddLookup["TG\n\rMOVE_BUTTON"] = ID_TG_MOVE_BUTTON;
   iddLookup["TG\n\rBOX_SELECT_BUTTON"] = ID_TG_BOX_SELECT_BUTTON;
   iddLookup["TG\n\rSELECT_GROUP_BUTTON"] = ID_TG_SELECT_GROUP_BUTTON;
   iddLookup["TG\n\rSELECT_ALL_BUTTON"] = ID_TG_SELECT_ALL_BUTTON;
   iddLookup["TG\n\rSHOW_HIDDEN_BUTTON"] = ID_TG_SHOW_HIDDEN_BUTTON;

   iddLookup["GPS\n\rCONNECT"] = ID_GPS_CONNECT;
   iddLookup["GPS\n\rOPTIONS"] = ID_GPS_OPTIONS;
   iddLookup["GPS\n\rLOAD_SETTINGS"] = ID_MOVING_MAP_LOAD_SETTINGS;
   iddLookup["GPS\n\rSAVE_SETTINGS"] = ID_MOVING_MAP_SAVE_SETTINGS;
   iddLookup["GPS\n\rAUTOCENTER"] = ID_GPS_AUTOCENTER;
   iddLookup["GPS\n\rAUTOROTATE"] = ID_GPS_AUTOROTATE;
   iddLookup["GPS\n\rSMOOTH_SCROLLING"] = ID_GPS_SMOOTH_SCROLLING;
   iddLookup["GPS\n\rCYCLE_CENTER"] = ID_CYCLE_CENTER;
   iddLookup["GPS\n\rTRAILPOINTS"] = ID_GPS_TRAILPOINTS;
   iddLookup["GPS\n\rCLOCK"] = ID_GPS_CLOCK;
   iddLookup["GPS\n\rTOGGLE_PREDICTIVE_PATH"] = ID_GPS_TOGGLE_PREDICTIVE_PATH;
   iddLookup["GPS\n\rTOGGLE_TAMASK"] = ID_GPS_TOGGLE_TAMASK;
   iddLookup["GPS\n\rCDI"] = ID_GPS_CDI;
   iddLookup["GPS\n\rRANGE_BEARING"] = ID_GPS_RANGE_BEARING;
   iddLookup["GPS\n\rCOAST_TRACK"] = ID_GPS_COAST_TRACK;
   iddLookup["GPS\n\rTABULAR_EDITOR"] = ID_GPS_TABULAR_EDITOR;
   iddLookup["GPS\n\rSTYLES"] = ID_GPS_STYLES;
#endif
}  // UserSimulator()

UserSimulator::~UserSimulator()
{
}

void UserSimulator::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(UserSimulator, CCmdTarget)
   //{{AFX_MSG_MAP(UserSimulator)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(UserSimulator, CCmdTarget)
   //{{AFX_DISPATCH_MAP(UserSimulator)
   DISP_FUNCTION(UserSimulator, "ActivateMenuItem",ActivateMenuItem, VT_I4, VTS_BSTR)
   DISP_FUNCTION(UserSimulator, "ActivateToolbarButton",ActivateToolbarButton, VT_I4, VTS_BSTR)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IUserSimulator to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IUserSimulator =
{ 0x728AD61D, 0x3808, 0x401a, { 0xA9, 0x4C, 0x5B, 0xAC, 0x96, 0x50, 0x30, 0x63 } };

BEGIN_INTERFACE_MAP(UserSimulator, CCmdTarget)
   INTERFACE_PART(UserSimulator, IID_IUserSimulator, Dispatch)
END_INTERFACE_MAP()



/////////////////////////////////////////////////////////////////////////////
// UserSimulator message handlers


/**
* Simulate a user selecting a menu option from the main menu bar
*
* menu_string is the text representation of the menu command:
*    "File\n\rSave As" would activate the Save As command under the File menu
*    "Map\n\rMove\n\rRotate Left" would activate the Rotate Left command under the Move submenu (under the Map menu)
*/
long UserSimulator::ActivateMenuItem(LPCTSTR menu_string)
{
   return ActivateControlString( FALSE, menu_string );
}

long UserSimulator::ActivateControlString( __in BOOL bToolbarOp, __in LPCTSTR pszControlString )
{
   CString csControlString( pszControlString );

   // Look for a target value
   int iTargetState = 0, iCurrentState;

   // Assume can find current state if wanted
   BOOL bStateReady = GetTargetState( csControlString, iTargetState );
   ToolbarTypeEnum eToolbarType = CORE_TOOLBAR_TYPE;

   long id = LookupIDD( csControlString, eToolbarType );
   if ( id == 0 && !bToolbarOp )    // Not found but not toolbar type
      id = m_mainFrame.get_ribbon_item_id( csControlString );      // Menu string

   if ( id != 0 )
   {
      if ( bStateReady )
         bStateReady =
            GetCurrentState( id, iCurrentState )
            || GetCurrentState( bToolbarOp, csControlString, iCurrentState );

      // If query only
      if ( iTargetState < 0 )
      {
         if ( !bStateReady )
            id = 0;     // Can't determine current state
         else if ( iCurrentState == 0 )
            id = -id;   // Return minus id as indicator that c.s. is off
      }

      else if ( !bStateReady     // No known or needed current state
         || iCurrentState != iTargetState )  // or current state needs changing
      {
         if ( eToolbarType == CLIENT_TOOLBAR_TYPE )
            m_mainFrame.SendMessage( WM_SIMULATOR_TOOLBAR_EVENT, id,
               (LPARAM) _bstr_t( (LPCTSTR) csControlString ).Detach() );

         else if ( eToolbarType == FV_TOOLBAR_TYPE )
         {
            C_ovl_mgr& ovl_mgr = *OVL_get_overlay_manager();
            IFvToolbarSimClientPtr spsc = ovl_mgr.GetCurrentFvToolbar();
            if ( spsc != NULL )
               // Send to current FvToolbar
               spsc->SimulatorEvent( id );
         }

         else if ( id == ID_APP_EXIT )
            m_mainFrame.PostMessage( WM_COMMAND, id );   // Don't hang around

         else
         {
            if ( !ProcessSpecialCommand( id, csControlString ) )
               m_mainFrame.SendMessage( WM_COMMAND, id );
         }
      }
   }
   return id;
}


/**
* Simulate a user selecting a toolbar button from a currently active toolbar
*
* toolbar_string is one of the below possible options
*
* Main toolbar
* "MAIN\n\rCURRENT_NEW" - Create a new overlay.
* "MAIN\n\rCURRENT_OPEN" - Open an existing overlay
* "MAIN\n\rFILE_CLOSE_FV" - Close overlay
* "MAIN\n\rCURRENT_SAVE" - Save a dirty overlay
* "MAIN\n\rSAVE_ALL" - Save all overlays
* "MAIN\n\rMAIN_PRINT_DIRECT" - Print the current map using the default print options
* "MAIN\n\rMAIN_PRINT_PREVIEW" - Preview printout of the current map using default print options
* "MAIN\n\rDIM" - Dim the map
* "MAIN\n\rBRIGHT" - Brighten the map
* "MAIN\n\rSCALE_PERCENT_ZOOM" - Zoom Tool
* "MAIN\n\rCOMBO_PLACEHOLDER" - Zoom Control
* "MAIN\n\rSCALE_ZOOM" - Scale Tool
* "MAIN\n\rMAP_SCALEIN" - Change map to next scale in
* "MAIN\n\rMAP_SCALEOUT" - Change map to next scale out
* "MAIN\n\rHAND_MODE" - Continuous Pan
* "MAIN\n\rMAP_PAN_UP" - Pan up half the window height
* "MAIN\n\rMAP_PAN_DOWN" - Pan down half the window height
* "MAIN\n\rMAP_PAN_LEFT" - Pan left half the window width
* "MAIN\n\rMAP_PAN_RIGHT" - Pan right half the window width
* "MAIN\n\rMAP_NEW" - Move to a specified location, set the map center, type, or rotation
* "MAIN\n\rCENTER_CROSSHAIR" - Center Crosshair
* "MAIN\n\rVIEWTIME" - Activate the Playback Control
* "MAIN\n\rHELP_FINDER" - Contents, Index, and Find for FalconView Help Topics
* "MAIN\n\rCONTEXT_HELP" - Display help on selected toolbar, dialog, or menu item
*
* AR Toolbar
* NOTE: The AR Toolbar must be active for these commands to have meaning.
* You can activate the AR Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rTrack/Orbit Editor" );
*
* "AR\n\rSELECT" - Select an AR orbit or track
* "AR\n\rTRACK" - Draw an AR track
* "AR\n\rANCHOR" - Draw an AR orbit
* "AR\n\rNAVAID" - Associate a navaid with an AR point
* "AR\n\rPOINT" - Add an orbit point
* "AR\n\rPROPERTY" - Edit AR orbit or track properties
*
* CHUM Toolbar
* NOTE: The CHUM Toolbar must be active for these commands to have meaning.
* You can activate the AR Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rManual CHUM Editor" );
*
* "CHUM\n\rTOOLS_SELECT" - Enter select mode
* "CHUM\n\rADDPNT" - Enter add Manual CHUM Point mode
* "CHUM\n\rADD_POWERLINE" - Enter add Powerline mode
* "CHUM\n\rDIALOG_SWITCH" - Toggle the CHUM edit dialog
* "CHUM\n\rDRAG_LOCK" - Toggle drag lock to enable/disable dragging
* "CHUM\n\rTABULAR_EDITOR" - Display the Tabular Editor
*
* Points Toolbar
* NOTE: The Points Toolbar must be active for these commands to have meaning.
* You can activate the Points Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rPoints Editor" );
*
* "LOCAL\n\rSELECT" - Enter select mode
* "LOCAL\n\rADD" - Enter add Point mode
* "LOCAL\n\rEDITOR_DIALOG_TOGGLE" - Toggle the Point Edit Dialog
* "LOCAL\n\rDRAG_LOCK" - Toggle drag lock to enable/disable dragging
* "LOCAL\n\rPT_TABULAR_EDITOR" - Display the Tabular Editor
*
* Map Data Manager Toolbar
* NOTE: The Map Data Manager Toolbar must be active for these commands to have meaning.
* You can activate the Map Data Manager Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rMap Data Manager" );
*
* "MDM\n\rTOOL_COMPATIBLE" - Select map files from current map scale
* "MDM\n\rTOOL_SELCOPY" - Select area to copy to target device
* "MDM\n\rTOOL_SELDELETE" - Select area to delete from target device6
*
* SkyView Toolbar
* NOTE: The SkyView Toolbar must be active for these commands to have meaning.
* You can activate the SkyView Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rSkyView Mode" );
*
* "SKYVIEW\n\rSELECT_SYM_TOOL" - Symbol selection tool
* "SKYVIEW\n\rSKYVIEW" - Run SkyView 3D viewer 
* "SKYVIEW\n\rCENTER_SYMBOL" - Move viewer symbols to the center of the current display
* "SKYVIEW\n\rOBJECTS" - Open the 3D Object Palette
* "SKYVIEW\n\rOBJ_PROPERTIES" - Toggle the object edit dialog
*
* NITF Toolbar
* NOTE: The NITF Toolbar must be active for these commands to have meaning.
* You can activate the NITF Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rNITF Data Manager" );
*
* "NITF\n\rSELECT" - Select Objects on the Map
* "NITF\n\rAREA_SELECT" - Define NITF Query Editor Region(s)-of-Interest
* "NITF\n\rERASE_AREAS" - Erase All User-Drawn Regions-of-Interest
* "NITF\n\rQUERY_EDITOR" - Show/Hide NITF Query Editor
* "NITF\n\rMETADATA_LIST" - Show/Hide Query Result Image Information List
* "NITF\n\rLOCAL_SEARCH" - Apply filter to images already known to this system
* "NITF\n\rWEB_SEARCH" - Search the world-wide web for images matching the filter
* "NITF\n\rDISPLAY_FILTER" - Show only Imagery Query Tool results on the map view
*
* Page Layout Toolbar
* NOTE: The Page Layout Toolbar must be active for these commands to have meaning.
* You can activate the Page Layout Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rPage Layout Tool" );
*
* "PRINTTOOL\n\rSELECT" - Enter select mode
* "PRINTTOOL\n\rPAGE" - Single Page Tool
* "PRINTTOOL\n\rAREA" - Area Chart Tool
* "PRINTTOOL\n\rSTRIP" - Generate Strip Charts
* "PRINTTOOL\n\rSNAPROUTE" - Snap page to route leg when it is dropped
* "PRINTTOOL\n\rALIGN_LEG" - Align page with route leg as it is dragged
* "PRINTTOOL\n\rPRINT" - Print maps from the page layout overlay
* "PRINTTOOL\n\rPRINT_PREVIEW" - Preview pages before printing
* "PRINTTOOL\n\rFILE_PRINT_SETUP" - Change the printer and printing options
* "PRINTTOOL\n\rPROPERTIES" - Set the overall properties for the Page Layout Overlay
*
* Drawing Toolbar
* NOTE: The Drawing Toolbar must be active for these commands to have meaning.
* You can activate the Drawing Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rDrawing Editor" );
* "TOOLS\n\rSELECT" - Enter select mode
* "TOOLS\n\rBOX_SELECT" - Enter box select mode
* "TOOLS\n\rFREEHAND2" - Freehand Tool
* "TOOLS\n\rLINE" - Line Tool
* "TOOLS\n\rPOLYLINE" - Polyline Tool
* "TOOLS\n\rPOLYGON" - Polygon Tool
* "TOOLS\n\rRECT" - Rectangle Tool
* "TOOLS\n\rOVAL" - Ellipse Tool
* "TOOLS\n\rTEXT" - Text Tool
* "TOOLS\n\rBULLSEYE" - Bullseye Tool
* "TOOLS\n\rAXIS" - Axis Tool
* "TOOLS\n\rPICTURE" - Picture Tool
* "TOOLS\n\rMOVE" - Move Any Object Tool
* "TOOLS\n\rCOLOR" - Color Selection
* "TOOLS\n\rWIDTH" - Line Width Selection
* "TOOLS\n\rSTYLE" - Line Style Selection
* "TOOLS\n\rFILL" - Fill Selection
* "TOOLS\n\rPROPERTY_SHEET" - Bring Up Property Dialog
* "TOOLS\n\rMANUAL_EDIT" - Manual Entry/Edit Tool
* "TOOLS\n\rTABULAR_EDITOR" - Display the Tabular Editor
* "TOOLS\n\rDRAG_LOCK" - Toggle drag lock to enable/disable dragging
*
* Route Toolbar
* NOTE: The Route Toolbar must be active for these commands to have meaning.
* You can activate the Route Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rRoute Editor" );
* NOTE: Not all route commands are applicable to all installs.  Check your
* target install to confirm commands you wish to use.
*
* "ROUTE\n\rEDIT_SELECT_MODE" - Enter select mode
* "ROUTE\n\rADD_SERPENTINE" - Enter add Serpentine Leg mode
* "ROUTE\n\rEDIT_ADD_TP" - Enter add Turn Point mode
* "ROUTE\n\rEDIT_ADD_IP" - Enter add Initial Point mode
* "ROUTE\n\rEDIT_ADD_VIP" - Enter add Visual Initial Point mode
* "ROUTE\n\rEDIT_ADD_TARGET" - Enter add Target mode
* "ROUTE\n\rEDIT_ADD_DROP_ZONE" - Enter add Drop Zone mode
* "ROUTE\n\rEDIT_ADD_GM_WAYPOINT" - Enter add Waypoint mode
* "ROUTE\n\rEDIT_ADD_GM_CHECKPOINT" - Enter add Checkpoint mode
* "ROUTE\n\rEDIT_ADD_GM_RELEASE_POINT" - Enter add Release Point mode
* "ROUTE\n\rEDIT_ADD_GM_OBJECTIVE" - Enter add Objective mode
* "ROUTE\n\rEDIT_ADD_GM_TARGET" - Enter add Target mode
* "ROUTE\n\rEDIT_ADD_OAP" - Enter add Offset Aim Point mode
* "ROUTE\n\rEDIT_ADD_VRP" - Enter add Visual Reference Point mode
* "ROUTE\n\rOFFSET_TARGET_MODE" - Enter add Offset Target mode
* "ROUTE\n\rEDIT_ADD_TARGET_REFERENCE" - Enter add Target Reference mode
* "ROUTE\n\rCENTER_ON_TURNPOINT" - Center map on current turnpoint
* "ROUTE\n\rCALC_ROUTE" - Calculate the route
* "ROUTE\n\rRTE_POINT_EDITOR" - Toggle PFPS Point Editor
* "ROUTE\n\rRTE_CFPS" - Toggle Combat Flight Planning Software
* "ROUTE\n\rROUTE_PROP" - Open the Route Properties page
* "ROUTE\n\rTMARK_PROP" - Open the Tick Mark Properties page
* "ROUTE\n\rDHOUSE_PROP" - Open the Doghouse Properties page
* "ROUTE\n\rTHACK_PROP" - Open the Time Hack Properties page
* "ROUTE\n\rCORRIDOR_PROP" - Open the Route Corridor Properties page
* "ROUTE\n\rREHEARSAL_PROP" - Open the Route Rehearsal Properties page
* "ROUTE\n\rBULLSEYE_PROP" - Open the Ship Bullseye Properties page
* "ROUTE\n\rPREDICTIVE_PATH_PROP" - Open the Predictive Path Properties page
* "ROUTE\n\rRTE_PROFILE" - Open Route Elevation Profile Dialog
*
* Tactical Graphics Toolbar
* NOTE: The Tactical Graphics Toolbar must be active for these commands to have meaning.
* You can activate the Tactical Graphics Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rTactical Graphics Editor" );
*
* "TG\n\rHIERARCHY" - Toggle the tactical graphics hierarchy dialog.
* "TG\n\rUSER_PALETTE" - Toggle the tactical graphics user palette dialog.
* "TG\n\rPROPERTIES" - Toggle the tactical graphics properties dialog.
* "TG\n\rMOVE_BUTTON" - Toggle the tactical graphics move intact functionality.
* "TG\n\rBOX_SELECT_BUTTON" - Toggle the tactical graphics box selection functionality.
* "TG\n\rSELECT_GROUP_BUTTON" - Toggle the tactical graphics group selection dialog.
* "TG\n\rSELECT_ALL_BUTTON" - Select all tactical graphics.
* "TG\n\rSHOW_HIDDEN_BUTTON" - Show hidden tactical graphics.
*
* GPS Toolbar
* NOTE: The GPS Toolbar must be active for these commands to have meaning
* You can activate the GPS Toolbar programmatically by calling
* ActivateMenuItem( "Tools\n\rMoving Map Tool" );
*
* "GPS\n\rCONNECT" - Connect / Disconnect Moving Map data stream
* "GPS\n\rOPTIONS" - Display the Moving Map Communications Dialog
* "GPS\n\rLOAD_SETTINGS" - Load Moving Map Settings
* "GPS\n\rSAVE_SETTINGS" - Save Moving Map Settings
* "GPS\n\rAUTOCENTER" - Select Moving Map Auto Center mode
* "GPS\n\rAUTOROTATE" - Select Moving Map Course Up mode
* "GPS\n\rSMOOTH_SCROLLING" - Select Moving Map Smooth Scrolling Mode
* "GPS\n\rCYCLE_CENTER" - Cycle to the next center
* "GPS\n\rTRAILPOINTS" - Display the trail points
* "GPS\n\rCLOCK" - Toggle time display on map
* "GPS\n\rTOGGLE_PREDICTIVE_PATH" - Toggle Predictive Path
* "GPS\n\rTOGGLE_TAMASK" - Toggle Terrain Avoidance Mask
* "GPS\n\rCDI" - Display the Course Deviation Indicator
* "GPS\n\rRANGE_BEARING" - View the Range and Bearing Dialog
* "GPS\n\rCOAST_TRACK" - View the Coast Track Dialog
* "GPS\n\rTABULAR_EDITOR" - Display the Tabular Editor
* "GPS\n\rSTYLES" - Display the Moving Map Options dialog
*/
long UserSimulator::ActivateToolbarButton(LPCTSTR toolbar_string)
{
   return ActivateControlString( TRUE, toolbar_string );
}


///////////////////////
// GetCurrentState() - False if state determination not possible
///////////////////////
BOOL UserSimulator::GetCurrentState( __inout long& iID, __out int& iCurrentState )
{
   CControlBar* pcb;
   RECT rc, rc0;
   m_mainFrame.GetWindowRect( &rc0 );

   int iGPSTag = 0;
   switch( iID )
   {
      case SC_MAXIMIZE:
      case SC_RESTORE:
      case SC_MINIMIZE:
         iID = m_mainFrame.IsIconic() ? SC_MINIMIZE
            : m_mainFrame.IsZoomed() ? SC_MAXIMIZE : SC_RESTORE;
         iCurrentState = 1; // Always "on"
         return TRUE;
      
      case ID_VIEW_TITLEBAR:
         iCurrentState = m_mainFrame.m_titlebarVisible ? 1 : 0;
         return TRUE;

#if 0 // No menubars if ribbon
      case ID_VIEW_MENUBAR:
         iCurrentState = ( m_mainFrame.GetMenu()->GetSafeHmenu() == NULL ) ? 0 : 1;
         return TRUE;

      case ID_VIEW_MAIN_TOOLBAR:
         pcb = m_mainFrame.GetControlBar( IDR_MainFrameToolbar );
         iCurrentState = pcb != NULL && pcb->IsWindowVisible() ? 1 : 0;
         return TRUE;

      case ID_VIEW_EDITOR_TOOLBAR:
         pcb = m_mainFrame.GetControlBar( IDR_EDITOR_BUTTONS_TB );
         iCurrentState = pcb != NULL && pcb->IsWindowVisible() ? 1 : 0;
         return TRUE;

#if 0 // No file, custom, search toolbars in 5.2+
      case ID_VIEW_FILE_OVERLAY_LIST:
         iCurrentState = m_mainFrame.m_overlay_toolbar.IsWindowVisible() ? 1 : 0;
         return TRUE;

      case ID_VIEW_CUSTOMTOOLBAR:
         iCurrentState = m_mainFrame.m_custom_toolbar->IsWindowVisible() ? 1 : 0;
         return TRUE;

      case ID_VIEW_SEARCHBAR:
         iCurrentState = m_mainFrame.m_searchToolbar.IsWindowVisible() ? 1 : 0;
         return TRUE;
#endif
#endif
      
      case ID_HAND_MODE:
      {
         MapView* pView = (MapView*) m_mainFrame.GetActiveView();
         iCurrentState = ( pView != NULL && pView->m_hand_mode ) ? 1 : 0;
         return TRUE;
      }

      case ID_VIEW_PLAYBACK_TOOLBAR:
         pcb = m_mainFrame.GetControlBar( IDR_PLAYBACK );
         iCurrentState = pcb != NULL && pcb->IsWindowVisible() ? 1 : 0;
         return TRUE;

      case ID_VIEW_SPLIT:
         iCurrentState = m_mainFrame.m_wndSplitter.GetRowCount() > 1 ? 1 : 0;
         return TRUE;

      case ID_VIEW_MAP_TABS_BAR:
         iCurrentState = 0;
         if ( m_mainFrame.m_pMapTabsBar != NULL
               && ( m_mainFrame.m_pMapTabsBar->GetStyle() & WS_VISIBLE )  != 0 )
         {
            m_mainFrame.m_pMapTabsBar->GetWindowRect( &rc );
            if ( rc.right - rc.left > rc.top - rc.bottom )
               iID |= ( rc0.bottom - rc.top ) << 16;

            iCurrentState = 1;
         }
         return TRUE;

#if 1 //ndef RC225_DEV
      case ID_VIEW_STATUS_BAR:
      {
         LONG lTop = rc0.bottom;
         iCurrentState = 0;
         for( int i = 0; i < m_mainFrame.m_statusBarMgr->m_arrStatusBars.GetSize(); ++i )
         {
            CMapStatusBar& msb = *m_mainFrame.m_statusBarMgr->m_arrStatusBars[i];
            if ( ( msb.GetStyle() & WS_VISIBLE) != 0 )
            {
               msb.GetWindowRect( &rc );
               if ( rc.right - rc.left > rc.top - rc.bottom )
                  lTop = __min( lTop, rc.top );
               iCurrentState = 1;
            }
         }
         iID |= ( rc0.bottom - lTop ) << 16;   // Status bar(s) top in high word
         return TRUE;
      }
#endif
      case ID_GPS_AUTOCENTER:
         iGPSTag = 'ACEN';
         break;

      case ID_GPS_AUTOROTATE:
         iGPSTag = 'AROT';
         break;

      case ID_GPS_CDI:
         iGPSTag = 'CDI';
         break;

      case ID_GPS_CLOCK:
#if 1
         iCurrentState = m_mainFrame.IsPlaybackDialogActive() ? 1 : 0;
#else
         iCurrentState = m_mainFrame.m_view_time_dlg.get_draw_time_on_map() ? 1 : 0;
#endif
         return TRUE;

      case ID_GPS_CONNECT:
         iGPSTag = 'CONN';
         break;

      case ID_GPS_SMOOTH_SCROLLING:
         iGPSTag = 'CCEN';
         break;

      case ID_GPS_TOGGLE_PREDICTIVE_PATH:
         {
            C_gps_trail* trail =
               dynamic_cast<C_gps_trail*>( OVL_get_overlay_manager()
                  ->get_first_of_type( FVWID_Overlay_MovingMapTrail ) );
            if ( trail == NULL )
               break;

            iCurrentState = trail->get_properties()->GetPredictivePathProperties()->get_predictive_path_on()
               ? 1 : 0;
            return TRUE;
         }

      case ID_GPS_TABULAR_EDITOR:
         iCurrentState = g_tabular_editor_dlg != NULL ? 1 : 0;
         return TRUE;

      case ID_GPS_TOGGLE_TAMASK:
         iCurrentState = NULL != OVL_get_overlay_manager()->get_first_of_type( FVWID_Overlay_TerrainAvoidanceMask )
            ? 1 : 0;
         return TRUE;

      case ID_GPS_TRAILPOINTS:
         iGPSTag = 'TRAI';
         break;
   }
   if ( iGPSTag != 0 )
   {
      iCurrentState = C_gps_trail::get_toggle( iGPSTag );
      return TRUE;
   }

   return FALSE;
}

BOOL UserSimulator::GetCurrentState(
            __in BOOL bToolbarItem,
            __in const CString& csControlString,
            __out int& iCurrentState )
{
   CFvwTextLineList lstControlItems( csControlString );
   int cItems = lstControlItems.GetCount();

   // "Overlay" or "Tools" + overlay/tool display name
   const CString& csCategory = lstControlItems.GetNext();

   BOOL bToolsItem = str_contains( csCategory, _T("Tools") );
   if ( cItems == 2 && !bToolbarItem
      && ( bToolsItem || str_contains( (LPCSTR) csCategory, _T("Overlay") ) ) )
   {
      const CString& csMenuItemName = lstControlItems.GetNext();

      // See if this is a recognized overlay
      COverlayTypeDescriptorList& lstOvlDesc = *OVL_get_type_descriptor_list();
      for ( lstOvlDesc.ResetEnumerator(); lstOvlDesc.MoveNext(); )
      {
         if ( !str_contains( lstOvlDesc.m_pCurrent->editorDisplayName, csMenuItemName )
               && ( lstOvlDesc.m_pCurrent->editorDisplayName.GetLength() > 0
                  || !str_contains( lstOvlDesc.m_pCurrent->displayName, csMenuItemName ) ) )
            continue;

         C_ovl_mgr& ovl_mgr = *OVL_get_overlay_manager();
         iCurrentState =
            // Find whether tool/editor is displayed
            ( ( bToolsItem && ovl_mgr.GetCurrentEditor() != GUID_NULL
                  && ovl_mgr.GetCurrentEditor() == lstOvlDesc.m_pCurrent->overlayDescriptorGuid )
               // or overlay is displayed
               || ( !bToolsItem && ovl_mgr.FindOverlayByDisplayName( csMenuItemName ) != NULL ) )
            ? 1 : 0;
         return TRUE;
      }
   }

   // Look for client or custom editor buttons
   int id;
   ToolbarTypeEnum eToolbarType = CORE_TOOLBAR_TYPE;  // Assume not client or FV type
   if ( bToolbarItem
      && cItems == 2
      && 0 != ( id = LookupIDD( csControlString, eToolbarType ) ) )
   {
      if ( eToolbarType == CLIENT_TOOLBAR_TYPE )
      {
         // TODO:  Is this code still needed in 5.2?
         //CMainFrame& frm = *fvw_get_frame();

         //POSITION position = frm.m_client_toolbar_map.GetStartPosition();
         //CClientTBClass* pTBLast = NULL;
         //while ( position )
         //{
         //   long handle;
         //   CClientTBClass* pTB;
         //   frm.m_client_toolbar_map.GetNextAssoc( position, handle, pTB );
         //   if ( 0 == csCategory.CompareNoCase( pTB->get_toolbar_name() ) )
         //      pTBLast = pTB;
         //}
         //if ( pTBLast != NULL )
         //{
         //   iCurrentState = ( 0 != pTBLast->get_button_state( id - ID_CLIENT_TB1 ) ) ? 1 : 0;
         //   return TRUE;
         //}
      }

      else if ( eToolbarType == FV_TOOLBAR_TYPE )
      {
         C_ovl_mgr& om = *OVL_get_overlay_manager();
#if 1
         IFvToolbarPtr sptb = om.GetCurrentFvToolbar();
         if ( csCategory == om.GetCurrentEditorTypeName()
            && sptb != NULL )
         {
            BOOL b = sptb->GetButtonPushed( id );
            iCurrentState = b ? 1 : 0;
#else
         IFvToolbarSimClientPtr sptbsc = om.GetCurrentFvToolbar();
         if ( csCategory == om.GetCurrentEditorTypeName()
            && sptbsc != NULL )
         {
            iCurrentState = ( ( sptbsc->ButtonState[ id ] & ( TBSTATE_CHECKED | TBSTATE_PRESSED ) ) != 0 )
               ? 1 : 0;
#endif
            return TRUE;
         }
      }
   }

   // Look up other kinds of on/off items

   
   return FALSE;
}

/////////////////////
// GetTargetState()
/////////////////////
BOOL UserSimulator::GetTargetState( __inout CString& csControlString, __out int& iTargetState )
{
   BOOL bResult = FALSE;
   INT iPos = csControlString.Find( _T(":::") );
   if ( iPos >= 0 )
   {
      do
      {
         CString cs = csControlString.Mid( iPos + 3 );
         if ( cs == _T("?") )
            iTargetState = -1;      // Query only
         else if ( _stscanf_s( cs, _T("%ld"), &iTargetState ) != 1 )
         {
            if ( 0 == cs.CompareNoCase( _T("false") ) )
               iTargetState = 0;
            else if ( 0 == cs.CompareNoCase( _T("true") ) )
               iTargetState = +1;
            else
               break;
         }

         bResult = TRUE;
      } while ( FALSE );
      csControlString.Delete( iPos, csControlString.GetLength() );
   }
   return bResult;
}


////////////////////
// LookupIDD()
///////////////////
int UserSimulator::LookupIDD( __in const CString& csControlString, __inout ToolbarTypeEnum& eToolbarType )
{
   CString cs2 = csControlString;
   cs2.Replace( _T('\r'), _T('\n') );
   cs2.Replace( _T("\n\n"), _T("\n") );   // All crlf's and lfcr's become single lf's
      
   int iResult = iddCoreLookup[ (LPCSTR) cs2 ];
   if ( iResult == 0 )  // Couldn't find in core lookup table
   {
      if ( 0 != ( iResult = iddClientLookup[ (LPCSTR) cs2 ] ) )
         eToolbarType = CLIENT_TOOLBAR_TYPE;   // Need to find client
      else if ( 0 != ( iResult = iddFvToolbarLookup[ (LPCSTR) cs2 ] ) )
         eToolbarType = FV_TOOLBAR_TYPE;
   }
   return iResult;
}


////////////////////
// ProcessSpecialCommandIDD()
///////////////////
BOOL UserSimulator::ProcessSpecialCommand( __in UINT uiID, __in const CString& csControlString )
{
   CFvwTextLineList text_lines( csControlString );
   switch( uiID )
   {
      case ID_ZOOM_COMBO:
      {
#if 1
         ZoomPercentRibbonComboBox* pzpc = DYNAMIC_DOWNCAST(
            ZoomPercentRibbonComboBox, m_mainFrame.m_wndRibbonBar.FindByID( ID_ZOOM_COMBO ) );
         if ( pzpc == NULL
               || text_lines.GetCount() < 3 )
            break;

         text_lines.GetNext();
         text_lines.GetNext();
         pzpc->SetEditText( text_lines.GetNext() ); // 3rd element
         m_mainFrame.OnZoomCombo();
#else
         MapView* pView = (MapView*) m_mainFrame.GetActiveView();
         CMFCRibbonComboBox* pCombo = DYNAMIC_DOWNCAST( CMFCRibbonComboBox,
            m_mainFrame.m_wndRibbonBar. FindByID( ID_ZOOM_COMBO ) );
         if ( pView == NULL
               || pCombo == NULL
               || text_lines.GetCount() < 3 )
            break;

         text_lines.GetNext();
         text_lines.GetNext();
         pCombo->SetEditText( text_lines.GetNext() ); // 3rd element
         pView->UpdateZoomFromCombo( pCombo );
#endif
         return TRUE;
      }
   }  // switch( uiID )


   return FALSE;
}



// Static methods

/////////////////////////////////////////////////////////////////////////////////////////////
// AddToolbarButtonLookup() - Add tool name and button hint text to button ID lookup table
/////////////////////////////////////////////////////////////////////////////////////////////
void UserSimulator::AddToolbarButtonLookup( __in UINT uiID,
      __in LPCTSTR pszToolbarName, __in LPCTSTR pszHintText )
{
   if ( pszToolbarName[0] != _T('\0')
      && pszHintText[0] != _T('\0') )
   {
      // Build the lookup key
      CString cs = pszToolbarName;
      cs += _T('\n');

      // The button name is taken to be the alphanumeric part of the hint text
      // converted to uppercase
      for ( LPCTSTR psz = pszHintText; *psz != _T('\0'); psz++ )
         if ( isalnum( *psz ) )
            cs += (TCHAR) toupper( *psz );

      if ( uiID < 40 )  // If client editor
         iddClientLookup[ (LPCSTR) cs ] = ID_CLIENT_TB1 + uiID;
      else              // FvToolbar 
         iddFvToolbarLookup[ (LPCSTR) cs ] = uiID;
   }
}


LRESULT UserSimulator::OnSimulatorToolbarEvent( __in WPARAM nButtonID,
                     __in LPARAM /*(BSTR)*/ bsControlString )
{
   _bstr_t bstrControlString( (BSTR) bsControlString, FALSE );

   CMainFrame& frm = *fvw_get_frame();

   CString csClientName = (LPCTSTR) bstrControlString;
   csClientName.Delete( _tcscspn( (LPCTSTR) bstrControlString, _T("\n\r") ), 0x10000 );

   // Is this needed in 5.2?
   //POSITION position = frm.m_client_toolbar_map.GetStartPosition();
   //CClientTBClass* pTBLast = NULL;
   //while ( position )
   //{
   //   long handle;
   //   CClientTBClass* pTB;
   //   frm.m_client_toolbar_map.GetNextAssoc( position, handle, pTB );
   //   if ( 0 == csClientName.CompareNoCase( pTB->get_toolbar_name() ) )
   //      pTBLast = pTB;
   //}
   //if ( pTBLast != NULL )
   //   pTBLast->OnClientTB( nButtonID );

   return 0;
}

// End of IUserSimulator.cpp

