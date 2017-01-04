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

#if !defined(AFX_IGPS_H__1E4408E1_7CFF_11D3_80A9_00C04F60B086__INCLUDED_)
#define AFX_IGPS_H__1E4408E1_7CFF_11D3_80A9_00C04F60B086__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// IGPS.h : header file
//

class C_gps_trail;

#include "gps.h"

/////////////////////////////////////////////////////////////////////////////
// GPS command target

class GPS : public CCmdTarget
{
   DECLARE_DYNCREATE(GPS)
   DECLARE_OLECREATE(GPS)
      
   GPS();           // protected constructor used by dynamic creation
   
   // Attributes
private:

	int m_registered_for_connect_disconnect;
   
   // Operations
public:
   
   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GPS)
public:
   virtual void OnFinalRelease();
   //}}AFX_VIRTUAL
   
   // Implementation
protected:
   virtual ~GPS();
   
   // Generated message map functions
   //{{AFX_MSG(GPS)
   // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG
   
   DECLARE_MESSAGE_MAP()
      // Generated OLE dispatch map functions
      //{{AFX_DISPATCH(GPS)
	afx_msg long GetCurrentPoint(long gps_overlay_handle, double FAR* latitude, double FAR* longitude, double FAR* altitude, double FAR* true_course, double FAR* mag_course, double FAR* speed_knots, DATE FAR* date_time);
   afx_msg long RegisterForConnectDisconnect(long hWnd);
	afx_msg long UnRegisterForConnectDisconnect(long hWnd);
	afx_msg long RegisterForCurrentPosition(long hWnd, long gps_overlay_handle);
	afx_msg long UnRegisterForCurrentPosition(long hWnd, long gps_overlay_handle);
	afx_msg long GetConnectedHandle();
	afx_msg long AddPoint(long gps_overlay_handle, double latitude, double longitude, double altitude, double true_course, double mag_course, double speed_knots, DATE date_time);
	afx_msg long Connect();
	afx_msg long Disconnect();
	//}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
      DECLARE_INTERFACE_MAP()
};

class GPS2 : public CCmdTarget
{
	DECLARE_DYNCREATE(GPS2)
   DECLARE_OLECREATE(GPS2)
      
   GPS2();           // protected constructor used by dynamic creation
   
   // Attributes
private:

   // Operations
public:
   
   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GPS2)
public:
   virtual void OnFinalRelease();
   //}}AFX_VIRTUAL
   
   // Implementation
protected:
   virtual ~GPS2();
   
   // Generated message map functions
   //{{AFX_MSG(GPS2)
   // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG
   
   DECLARE_MESSAGE_MAP()
      // Generated OLE dispatch map functions
      //{{AFX_DISPATCH(GPS2)
	afx_msg long AddPointEx(long gps_overlay_handle, double latitude, double longitude, double altitude, double true_course, double mag_course, double speed_knots, DATE date_time, LPCTSTR meta_data);
	//}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IGPS_H__1E4408E1_7CFF_11D3_80A9_00C04F60B086__INCLUDED_)
