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

// StatBarBullseyePropPage.h
//

#include "resource.h"

class CStatBarBullseyePropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStatBarBullseyePropPage)

// Construction
public:
   CStatBarBullseyePropPage();

   static CString m_strDescription;
   static double m_dLat;
   static double m_dLon;
   static bool m_bUnitsNMnotKM;
   static bool m_bCourseMagNotTrue;

	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Coordinate_Format_4.htm";}

// Dialog Data
	//{{AFX_DATA(CStatBarBullseyePropPage)
	enum { IDD = IDD_STATBAR_BULLSEYE_COORDINATES_PROPPAGE };
   CButton m_rbUnitsNM;
   CButton m_rbUnitsKM;
   CButton m_rbCourseMag;
   CButton m_rbCourseTrue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatBarBullseyePropPage)
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedFrom();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   void UpdateFromText();
   void SaveSettings();
};