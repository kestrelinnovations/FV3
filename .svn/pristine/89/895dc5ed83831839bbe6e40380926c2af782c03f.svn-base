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

// layer2.h
//

class Layer2 : public CCmdTarget
{
	DECLARE_DYNCREATE(Layer2)
   DECLARE_OLECREATE(Layer2)

	Layer2();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Layer2)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~Layer2();

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(Layer2)
   afx_msg long AddRegion(long layer_handle, int type, const VARIANT FAR& lat_lon_array, 
				long num_points);
   afx_msg long CombineRegion(long layer_handle, long region_handle, 
				int type, const VARIANT FAR &lat_lon_array, long num_points, int operation);
	afx_msg long DisplayMask( long layer_handle, LPCTSTR file_spec, double flight_altitude,
		      long base_elevation, long mask_color, long brush_style);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};