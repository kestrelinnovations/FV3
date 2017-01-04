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

// IDialogNotify.h : header file
//

#if !defined(AFX_IDIALOGNOTIFY_H__7D4AF101_F633_486D_8AC1_D7AE2B067209__INCLUDED_)
#define AFX_IDIALOGNOTIFY_H__7D4AF101_F633_486D_8AC1_D7AE2B067209__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogNotifyID.h"

typedef CMap<long,long,long,long> CMapWordToWord;
typedef CList <IDispatch FAR*, IDispatch FAR*> DispatchList;

/////////////////////////////////////////////////////////////////////////////
// DialogNotify command target

class DialogNotify : public CCmdTarget
{
	DECLARE_DYNCREATE(DialogNotify)

	DispatchList m_dispatch_lst;
	CMapWordToWord	m_DlgIDs;

	static DialogNotify* m_pDlg;
	
	SHORT SuppressDialogDisplayEx( UINT dialog_id );

// Attributes
public:


// Operations
public:
	static SHORT SuppressDialogDisplay( UINT dialog_id );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DialogNotify)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	DialogNotify();           // protected constructor used by dynamic creation
	virtual ~DialogNotify();

	// Generated message map functions
	//{{AFX_MSG(DialogNotify)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(DialogNotify)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(DialogNotify)
	afx_msg long RegisterForCallbacks( VARIANT FAR& dispatch_pointer);
	afx_msg long UnregisterForCallbacks( VARIANT FAR& dispatch_pointer);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDIALOGNOTIFY_H__7D4AF101_F633_486D_8AC1_D7AE2B067209__INCLUDED_)
