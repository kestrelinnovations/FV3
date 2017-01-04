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

// IMapDataManager.h
//

#include <comdef.h>
#include <vector>

struct COVERAGE_RECT
{
   _bstr_t bstrFilename;
   double ll_lat, ll_lon, ur_lat, ur_lon;
};

class MapDataManager: public CCmdTarget
{
	DECLARE_DYNCREATE(MapDataManager)
   DECLARE_OLECREATE(MapDataManager)

	MapDataManager();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MapDataManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~MapDataManager();

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(MapDataManager)
	afx_msg long GetFramesInRect(double ll_lat, double ll_lon, double ur_lat,
				double ur_lon, int map_handle, int *query_id, int *num_elements);
	afx_msg long GetFrameInfo(int query_id, int element_num, double *ll_lat, 
				double *ll_lon, double *ur_lat, double *ur_lon, BSTR *filename);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

   std::vector<COVERAGE_RECT> m_vectorCoverageRects;
	int m_next_query_id;
};

