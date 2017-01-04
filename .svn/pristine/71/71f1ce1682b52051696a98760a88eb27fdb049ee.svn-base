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

#if !defined(AFX_MDMCATPRINT_H__5BA0EB25_30CC_11D4_A745_00105A9B4C37__INCLUDED_)
#define AFX_MDMCATPRINT_H__5BA0EB25_30CC_11D4_A745_00105A9B4C37__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mdmcatprint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMDMCatPrint dialog

class CMDMCatPrint : public CDialog
{
// Construction
public:
   enum {
      cUnknown = -1,
      cPrintList,
      cSaveAsXML,
   };

	int GetChoice(void);
	CMDMCatPrint(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMDMCatPrint)
	enum { IDD = IDD_MDM_CATPRINT };
	CListBox	m_PrintListCtrl;
	CString	m_sPrintType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDMCatPrint)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Print_CD_Library.htm";}
	// Generated message map functions
	//{{AFX_MSG(CMDMCatPrint)
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	afx_msg void OnMdmcatprintOk();
	afx_msg void OnMdmcatprintCancel();
	afx_msg void OnHelpMdmCatprint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_nChoice;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMCATPRINT_H__5BA0EB25_30CC_11D4_A745_00105A9B4C37__INCLUDED_)