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


// SDSWaitDlg.h : header file
//


#if !defined(AFX_SDSWAITDLG_H__7410F224_B751_4FAA_B0E1_2A065E11C7E0__INCLUDED_)
#define AFX_SDSWAITDLG_H__7410F224_B751_4FAA_B0E1_2A065E11C7E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../resource.h"


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CSDSWaitDlg dialog

class CSDSWaitDlg : public CDialog
{
// Construction
public:
   CSDSWaitDlg(CWnd* pParent = NULL);   // standard constructor
   ~CSDSWaitDlg();

// Dialog Data
   //{{AFX_DATA(CSDSWaitDlg)
   enum { IDD = IDD_SM_WAIT };
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSDSWaitDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   HCURSOR m_hWaitCursor;

   // Generated message map functions
   //{{AFX_MSG(CSDSWaitDlg)
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

};  // namespace scene_mgr

#endif // !defined(AFX_SDSWAITDLG_H__7410F224_B751_4FAA_B0E1_2A065E11C7E0__INCLUDED_)
