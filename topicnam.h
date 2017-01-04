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



// topicnam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTopicName dialog

class CTopicName : public CDialog
{
protected:
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
// Construction
public:
	CTopicName(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTopicName)
	enum { IDD = IDD_TOPIC_NAME };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/FalconView_Overview.htm";}
	CString	m_group_name;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CTopicName)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};