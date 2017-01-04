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

#if !defined(AFX_SHAPEGOTOPROPPAGEDISABLED_H__2AE06968_9FCE_46E6_BEB5_7F50F78667C1__INCLUDED_)
#define AFX_SHAPEGOTOPROPPAGEDISABLED_H__2AE06968_9FCE_46E6_BEB5_7F50F78667C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShapeGoToPropPageDisabled.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShapeGoToPropPageDisabled dialog

class CShapeGoToPropPageDisabled : public CPropertyPage
{
	DECLARE_DYNCREATE(CShapeGoToPropPageDisabled)

// Construction
public:
	CShapeGoToPropPageDisabled();
	~CShapeGoToPropPageDisabled();

// Dialog Data
	//{{AFX_DATA(CShapeGoToPropPageDisabled)
	enum { IDD = IDD_SHP_SEARCH_DLG_DISABLED };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CShapeGoToPropPageDisabled)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Go_To_Shape_Files_Search.htm";}

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CShapeGoToPropPageDisabled)
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAPEGOTOPROPPAGEDISABLED_H__2AE06968_9FCE_46E6_BEB5_7F50F78667C1__INCLUDED_)
