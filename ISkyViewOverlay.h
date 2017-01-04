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

// ISkyViewOverlay.h
//


class SkyViewOverlay: public CCmdTarget
{
	DECLARE_DYNCREATE(SkyViewOverlay)
   DECLARE_OLECREATE(SkyViewOverlay)

	SkyViewOverlay();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SkyViewOverlay)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SkyViewOverlay();

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(SkyViewOverlay)

   afx_msg long ShowSkyViewSymbol(long show);

	afx_msg long UpdateStateIndicators(
		double camera_lat, double camera_lon, double camera_altitude, double camera_heading, 
		double map_node_load_center_lat, double map_node_load_center_lon,
		VARIANT_BOOL terrain_is_loaded,
		double terrain_bounding_lat_south, double terrain_bounding_lon_west,
		double terrain_bounding_lat_north, double terrain_bounding_lon_east,
		
		VARIANT_BOOL terrain_point_seen_at_upper_left_screen_point_found,
		double terrain_point_seen_at_upper_left_screen_point_lat,
		double terrain_point_seen_at_upper_left_screen_point_lon,
		double terrain_point_seen_at_upper_left_screen_point_elevation,
		
		VARIANT_BOOL terrain_point_seen_at_upper_right_screen_point_found,
		double terrain_point_seen_at_upper_right_screen_point_lat,
		double terrain_point_seen_at_upper_right_screen_point_lon,
		double terrain_point_seen_at_upper_right_screen_point_elevation,
		
		VARIANT_BOOL terrain_point_seen_at_lower_left_screen_point_found,
		double terrain_point_seen_at_lower_left_screen_point_lat,
		double terrain_point_seen_at_lower_left_screen_point_lon,
		double terrain_point_seen_at_lower_left_screen_point_elevation,
		
		VARIANT_BOOL terrain_point_seen_at_lower_right_screen_point_found,
		double terrain_point_seen_at_lower_right_screen_point_lat,
		double terrain_point_seen_at_lower_right_screen_point_lon,
		double terrain_point_seen_at_lower_right_screen_point_elevation);

	afx_msg long ClearStateIndicators();

   afx_msg long GetSkyView(VARIANT *pSkyViewInterface);
	
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};