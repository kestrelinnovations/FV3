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



// optdis.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDis dialog

class COptionsDis : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsDis)

// Construction
public:
	COptionsDis();
	~COptionsDis();

// Dialog Data
	//{{AFX_DATA(COptionsDis)
	enum { IDD = IDD_DEFAULT_DISP };
   CButton m_rbPrimaryFormat;
   CButton m_rbSecondaryFormat;
	CButton	m_rbMeters;
   CButton m_rbFeet;
	CButton	m_rbMSL;
   CButton m_rbHAE;
   CButton m_rbEGM84;
   CButton m_rbEGM96;
   CButton m_rbEGM08;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsDis)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnHAE();
	afx_msg void OnMSL();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionsDis)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString m_old_format;
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Coordinate_Format_1.htm";}
};
