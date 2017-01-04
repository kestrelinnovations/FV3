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

#if !defined(AFX_TACTICALGRAPHICSSERVER2_H__A4F53FF0_F2A6_4DC1_8DE3_71E7E71DAA72__INCLUDED_)
#define AFX_TACTICALGRAPHICSSERVER2_H__A4F53FF0_F2A6_4DC1_8DE3_71E7E71DAA72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TacticalGraphicsServer2.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// TacticalGraphicsServer2 command target

class TacticalGraphicsServer2 : public CCmdTarget
{
	DECLARE_DYNCREATE(TacticalGraphicsServer2)

	TacticalGraphicsServer2();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TacticalGraphicsServer2)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~TacticalGraphicsServer2();

	// Generated message map functions
	//{{AFX_MSG(TacticalGraphicsServer2)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(TacticalGraphicsServer2)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(TacticalGraphicsServer2)
	afx_msg long UpdateGraphics(long tg_ovl_handle, LPCTSTR tg_xml_doc);
	afx_msg long RemoveGraphic(long tg_handle);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TACTICALGRAPHICSSERVER2_H__A4F53FF0_F2A6_4DC1_8DE3_71E7E71DAA72__INCLUDED_)
