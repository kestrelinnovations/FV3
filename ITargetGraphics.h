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

// ITargetGraphics.h
//

#pragma once

typedef struct
{
	double lat;
	double lon;
	CString id;
	CString description;
	CString comment;
	CString links;
} target_t;

class TargetGraphics: public CCmdTarget
{
	DECLARE_DYNCREATE(TargetGraphics)
	DECLARE_OLECREATE(TargetGraphics)

	TargetGraphics();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TargetGraphics)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~TargetGraphics();

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(TargetGraphics)

   afx_msg long AddTarget(double lat, double lon, LPCTSTR id, LPCTSTR description, LPCTSTR comment);
	afx_msg long DoModal(long *result);
	
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	CList<target_t, target_t> m_target_list;
};

