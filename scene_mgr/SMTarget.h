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



#if !defined(AFX_SMTARGET_H__1B919EB6_643A_11D2_B07F_00104B8C938E__INCLUDED_)
#define AFX_SMTARGET_H__1B919EB6_643A_11D2_B07F_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// SMTarget.h : header file
//
#include "smlctrl.h"
#include "..\resource.h"

#include <string>


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CSMTarget dialog

class CSMTarget : public CDialog
{
   std::string m_TargetPath;
   long m_TargetID;

// Construction
public:
   CSMTarget(CWnd* pParent = NULL);   // standard constructor
   void Refresh(void);

   std::string GetTargetPath() { return m_TargetPath; };
   long GetTargetID() { return m_TargetID; };

// Dialog Data
   //{{AFX_DATA(CSMTarget)
   enum { IDD = IDD_SM_TARGET };
   CButton     m_HelpBtnCtrl;
   CSMListCtrl m_TargetListCtrl;
   CButton     m_OKBtnCtrl;
   CButton     m_CancelBtnCtrl;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSMTarget)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CSMTarget)
   afx_msg void OnSmtargetOk();
   afx_msg void OnSmtargetCancel();
   virtual BOOL OnInitDialog();
   afx_msg void OnHelpSmTarget();
   afx_msg void OnDblclkSmtargetTargetlist(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

};  // namespace scene_mgr

#endif // !defined(AFX_SMTARGET_H__1B919EB6_643A_11D2_B07F_00104B8C938E__INCLUDED_)
