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

#if !defined(AFX_TACTICALGRAPHICSSERVER_H__5416B82B_CEB0_4C1F_9B6D_AFDD802E5463__INCLUDED_)
#define AFX_TACTICALGRAPHICSSERVER_H__5416B82B_CEB0_4C1F_9B6D_AFDD802E5463__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TacticalGraphicsServer.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// TacticalGraphicsServer command target

class TacticalGraphicsServer : public CCmdTarget
{
	DECLARE_DYNCREATE(TacticalGraphicsServer)

	TacticalGraphicsServer();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TacticalGraphicsServer)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~TacticalGraphicsServer();

	// Generated message map functions
	//{{AFX_MSG(TacticalGraphicsServer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(TacticalGraphicsServer)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(TacticalGraphicsServer)
	afx_msg long AddGraphics(long tg_ovl_handle, LPCTSTR tg_xml_doc);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TACTICALGRAPHICSSERVER_H__5416B82B_CEB0_4C1F_9B6D_AFDD802E5463__INCLUDED_)
