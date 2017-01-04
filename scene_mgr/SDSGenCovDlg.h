// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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


// SDSGenCovDlg.h : header file
//


#if !defined(AFX_SDSGENCOVDLG_H__9FEEF4A3_0E1C_48B6_9840_ED1099B1A0CB__INCLUDED_)
#define AFX_SDSGENCOVDLG_H__9FEEF4A3_0E1C_48B6_9840_ED1099B1A0CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\resource.h"
#include "SDSWaitDlg.h"
#include <string>


namespace scene_mgr
{

// Custom status messages for this dialog
static UINT SM_COVGEN_START = RegisterWindowMessage(_T("SM_COVGEN_START"));
static UINT SM_COVGEN_STATUS = RegisterWindowMessage(_T("SM_COVGEN_STATUS"));
static UINT SM_COVGEN_STOP = RegisterWindowMessage(_T("SM_COVGEN_STOP"));


typedef struct {
   std::string str;
} covgen_start_t;

typedef struct {
   std::string strDest;
   std::string strSrc;
   long lFileCount;
   long lPercentDone;
} covgen_status_t;

typedef struct {
   std::string str;
} covgen_stop_t;

/////////////////////////////////////////////////////////////////////////////
// CSDSGenCovDlg dialog

class CSDSGenCovDlg : public CDialog
{
// Construction
public:
   CSDSGenCovDlg(CWnd* pParent = NULL);   // standard constructor
   ~CSDSGenCovDlg();

// Dialog Data
   //{{AFX_DATA(CSDSGenCovDlg)
   enum { IDD = IDD_SM_GEN_COV };
   CString m_strDataSource;
   CString m_strMapSeries;
   CString m_strNumFiles;
   CString m_strTotalTime;
   CAnimateCtrl m_animateCtrl;
   //}}AFX_DATA

   bool m_boolCloseSelected;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGenCovDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   CSDSWaitDlg m_dlgWait;

   // Generated message map functions
   //{{AFX_MSG(CGenCovDlg)
   virtual BOOL OnInitDialog();
   afx_msg LRESULT OnCovGenStart(WPARAM, LPARAM);
   afx_msg LRESULT OnCovGenStatus(WPARAM, LPARAM);
   afx_msg LRESULT OnCovGenStop(WPARAM, LPARAM);
   afx_msg void OnClose();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

};  // namespace scene_mgr

#endif // !defined(AFX_SDSGENCOVDLG_H__9FEEF4A3_0E1C_48B6_9840_ED1099B1A0CB__INCLUDED_)
