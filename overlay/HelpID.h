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



////////////////////////////////////////
//
// Context sensitive help IDs
//
// these IDs are needed for dynamically allocated menu items
// that do not have associated resource IDs.  Specifically each overlay 
// added to the Overlay menu needs a unique HelpID.  The overlay factory 
// contains the ID and help file name for help on the resource ID.  
// The ID must be unique to the help file but there is no problem if different
// help files share duplicate IDs.
//
// These ID's defined hereare for use with FVW.hlp
// 
////////////////////////////////////

enum enumHelpIDs {
ECHUM_OVERLAY_HELP      = 0x1004,
GRID_OVERLAY_HELP       = 0x1006,
LOCALPNT_OVERLAY_HELP   = 0x1007,
SCALEBAR_OVERLAY_HELP   = 0x100C,
MOV_SYM_OVERLAY_HELP    = 0x100D,
GPS_TRAIL_OVERLAY_HELP  = 0x1013,
OVLKIT_OVERLAY_HELP     = 0x1014,
CHUM_OVERLAY_HELP       = 0x1015,
PRINTTOOL_OVERLAY_HELP  = 0x1017,
COVERAGE_OVERLAY_HELP   = 0x1018,
MAP_SERVER_OVERLAY_HELP = 0x1019,
SHAPE_OVERLAY_HELP      = 0x101C,
POINT_EXPORT_OVERLAY_HELP = 0x101D,
VMAP_OVERLAY_HELP       = 0x101F,
ENV_OVERLAY_HELP       = 0x101E,
DNC_OVERLAY_HELP        = 0x1020,
AREDIT_OVERLAY_HELP     = 0x1021,
TACTICAL_GRAPHICS_OVERLAY_HELP = 0x1022,
CONTOUR_OVERLAY_HELP    = 0x1024,
TAMASK_OVERLAY_HELP     = 0x1025,
PLUG_IN_OVERLAY_HELP = 0x1026,
NITF_OVERLAY_HELP		   = 0x1027,
WVS_OVERLAY_HELP        = 0x1028
};
