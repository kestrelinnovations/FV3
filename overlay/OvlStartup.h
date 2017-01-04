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



// OvlStartup.h : header file
//




/////////////////////////////////////////////////////////////////////////////
// COvlStartup dialog

class COvlStartup : public CDialog
{
protected:
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
// Construction
public:
	COvlStartup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COvlStartup)
	enum { IDD = IDD_OVL_MGR_STARTUP };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Overlay_Startup_Options.htm";}

	CCheckListBox	m_ovl_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COvlStartup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COvlStartup)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};