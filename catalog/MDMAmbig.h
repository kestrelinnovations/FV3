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



#if !defined(AFX_MDMAMBIG_H__2F7C65D2_B497_11D2_B9E8_00105A9B4C37__INCLUDED_)
#define AFX_MDMAMBIG_H__2F7C65D2_B497_11D2_B9E8_00105A9B4C37__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MDMAmbig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMDMAmbiguous dialog

class CMDMAmbiguous : public CDialog
{
   int m_SelectRadioValue;
   BOOL m_bSelSrc;
   BOOL m_bSelTgt;
   BOOL m_bUnSelSrc;
   BOOL m_bUnSelTgt;

// Construction
public:
	CMDMAmbiguous(CWnd* pParent = NULL);   // standard constructor
   void EnableRadioValue(int i, BOOL b);

   void SetRadioValue(int i) {m_SelectRadioValue = i; };
   int  GetRadioValue(void) {return m_SelectRadioValue; };

// Dialog Data
	//{{AFX_DATA(CMDMAmbiguous)
	enum { IDD = IDD_MDM_AMBIGUOUS };

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDMAmbiguous)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Ambiguous_Map_Data_Selection.htm";}
	// Generated message map functions
	//{{AFX_MSG(CMDMAmbiguous)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnDoubleclickedMdmambigSelsrc();
	afx_msg void OnDoubleclickedMdmambigSeltgt();
	afx_msg void OnDoubleclickedMdmambigUnselsrc();
	afx_msg void OnDoubleclickedMdmambigUnseltgt();
	virtual void OnOK();
	afx_msg void OnMdmambigSelsrc();
	afx_msg void OnMdmambigSeltgt();
	afx_msg void OnMdmambigUnselsrc();
	afx_msg void OnMdmambigUnseltgt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMAMBIG_H__2F7C65D2_B497_11D2_B9E8_00105A9B4C37__INCLUDED_)