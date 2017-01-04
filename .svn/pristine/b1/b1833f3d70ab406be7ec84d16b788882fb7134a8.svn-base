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

// ITargetGraphics2.h
//

#pragma once

using namespace FvDataSourcesLib;

typedef struct
{
	IFvDataSourcePtr dataSource;
	IFeaturePtr feature;
} feature_t;

class TargetGraphics2: public CCmdTarget
{
	DECLARE_DYNCREATE(TargetGraphics2)
	DECLARE_OLECREATE(TargetGraphics2)

	TargetGraphics2();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TargetGraphics2)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~TargetGraphics2();

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(TargetGraphics2)

   afx_msg long AddTarget(IDispatch *fvDataSource, IDispatch *feature);
   afx_msg long DoModal(long *result);
	
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	CList<feature_t, feature_t> m_feature_list;
};