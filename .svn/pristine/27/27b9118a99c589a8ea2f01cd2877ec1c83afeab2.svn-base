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



// optpri.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsPri dialog
//{{AFX_INCLUDES()
#include "geocx2.h"
//}}AFX_INCLUDES

class COptionsPri : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsPri)

// Construction
public:
	COptionsPri();
	~COptionsPri();

// Dialog Data
	//{{AFX_DATA(COptionsPri)
	enum { IDD = IDD_PRI_OPTIONS };
	CGeocx2	m_ocxGeo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsPri)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionsPri)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1a();
	afx_msg void OnRadio2a();
	afx_msg void OnRadio3a();
	afx_msg void OnRadio4a();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString m_old_datum;
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Coordinate_Format_2.htm";}
};
