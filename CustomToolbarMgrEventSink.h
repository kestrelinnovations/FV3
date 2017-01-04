// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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

// CCustomToolbarMgrEventSink - sink for custom tool manager's OnHelp event
//

class CCustomToolbarMgrEventSink : public CCmdTarget
{
	DECLARE_DYNCREATE(CCustomToolbarMgrEventSink)

	CCustomToolbarMgrEventSink();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides

// Implementation
public:
	virtual ~CCustomToolbarMgrEventSink() { }

	// Generated message map functions

	DECLARE_MESSAGE_MAP()
	
	DECLARE_OLECREATE(CCustomToolbarMgrEventSink)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCustomToolbarMgrEventSink)
	afx_msg void OnHelp();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
}; // class CCustomToolbarMgrEventSink