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


/////////////////////////////////////////////////////////////////////////////
// Map command target

class MapType;

class combo_dted_hit_and_miss_cache;

// for internal usage (instead of needing the VARIANT).  The data array should
// already be allocated
long get_block_dted_in_feet(double NW_lat, double NW_lon, 
                              double SE_lat, double SE_lon, 
                              short number_NS_samples, short number_EW_samples, 
                              short DTED_type, short* array);

class Map : public CCmdTarget
{
	DECLARE_DYNCREATE(Map)
   DECLARE_OLECREATE(Map)

	Map();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Map)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~Map();

	// Generated message map functions
	//{{AFX_MSG(Map)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(Map)
	afx_msg SCODE GetMouseClick(long main_window_handle, long FAR* click_id);
	afx_msg SCODE GetSnapToInfo(long main_window_handle, long point_type_filter, long FAR* click_id);
	afx_msg long GetElevation(double lat, double lon, long FAR* elevation);
	afx_msg long GetMapDimensions(long FAR* height, long FAR* width);
	afx_msg long GetDegreesPerPixel(double lat, double lon, long category, long scale, double FAR* deg_lat_per_pix, double FAR* deg_lon_per_pix);
	afx_msg long CAPSPrint(LPCTSTR text, BOOL print_to_scale, BOOL show_map_info);
	afx_msg SCODE GetBlockDTEDinFeet(double NW_lat, double NW_lon, double SE_lat, double SE_lon, short number_NS_samples, short number_EW_samples, short DTED_type, VARIANT FAR* array);
	afx_msg long CreateMapFromGeoBounds(double southern_lat, double western_lon, double northern_lat, double eastern_lon, long category, long scale, double brightness, short draw_overlays, short draw_map_if_no_data, short FAR* no_data, long FAR* pixmap_width, long FAR* pixmap_height, VARIANT FAR* dib, VARIANT FAR* corners);
	afx_msg long CreateFixedSizeMapFromGeoBounds(double southern_lat, double western_lon, double northern_lat, double eastern_lon, long category, long scale, double brightness, short draw_overlays, short draw_map_if_no_data, long pixmap_width, long pixmap_height, short FAR* no_data, VARIANT FAR* dib, VARIANT FAR* corners);
	afx_msg long CalcPixmapSizeFromGeoBounds(double southern_lat, double western_lon, double northern_lat, double eastern_lon, long category, long scale, long FAR* pixmap_width, long FAR* pixmap_height, VARIANT FAR* corners, short check_for_data, short FAR* no_data);
	afx_msg long GetMapDisplayString(long category, long map_handle, BSTR FAR* display_str);
	afx_msg long QueryMapTypes(long category, VARIANT FAR* map_handle_lst, VARIANT FAR* deg_per_pixel_lst, long FAR* map_type_count, BOOL available_map_types_only);
	afx_msg SCODE GetGeoRectBounds(long main_window_handle, long FAR* click_id);
	afx_msg SCODE GetGeoCircleBounds(long main_window_handle, long FAR* click_id);
	afx_msg long SetMapDisplay(double lat, double lon, double rotation, long client_category, long map_handle, long zoom, long mask, long projection_type);
	afx_msg long GetMapDisplay(double FAR* lat, double FAR* lon, double FAR* rotation, long FAR* category, long FAR* map_handle, long FAR* zoom, long FAR* projection_type);
	afx_msg long CreateMap(double center_lat, double center_lon, long category, long map_handle, double rotation_angle, long zoom_percent, long projection_type, double brightness, short draw_overlays, long pixmap_width, long pixmap_height, short draw_map_if_no_data, short FAR* no_data, VARIANT FAR* dib, VARIANT FAR* corners);
	afx_msg long CalcPixmapGeoCorners(double center_lat, double center_lon, long category, long map_handle, double rotation_angle, long zoom_percent, long projection_type, long pixmap_width, long pixmap_height, VARIANT FAR* corners, short check_for_data, short FAR* no_data);
	afx_msg long GetLatLonPositionInMap(double center_lat, double center_lon, long category, long map_handle, double rotation_angle, long zoom_percent, long projection_type, long pixmap_width, long pixmap_height, long pixel_column, long pixel_row, double FAR* pixel_latitude, double FAR* pixel_longitude);
	afx_msg long SetCIBParameters(short auto_contrast, double brightness, double contrast);
	afx_msg SCODE GetMouseClickCallback(long FAR* click_id);
	afx_msg SCODE GetSnapToInfoCallback(long point_type_filter, long FAR* click_id);
	afx_msg SCODE GetGeoRectBoundsCallback(long FAR* click_id);
	afx_msg SCODE GetGeoCircleBoundsCallback(long FAR* click_id);
	afx_msg long SetDispatchPtr(const VARIANT FAR& dispatch_pointer);
	afx_msg long CancelClickRequest(long click_id);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
   // return the number of map types which have data available on the system
   long get_map_type_count(CList <MapType *, MapType *> &map_list);

	// parameters for CIB auto contrast
	bool m_auto_contrast_on;
	double m_cib_brightness;
	double m_cib_contrast;

	// dispatch pointer to the client's callback object
   IDispatch FAR* m_pdisp;

	long SetDispatch_hlpr(VARIANT FAR dispatch_pointer);
};

/////////////////////////////////////////////////////////////////////////////
