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

#if !defined(AFX_ELEVATIONDLG_H__923AD822_C639_4CA2_B7C4_E992D7D2849F__INCLUDED_)
#define AFX_ELEVATIONDLG_H__923AD822_C639_4CA2_B7C4_E992D7D2849F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ElevationDlg.h : header file
//

class BackgroundStatic : public CStatic 
{
public:
   HBRUSH m_brush;
   BackgroundStatic();

protected:
   DECLARE_DYNAMIC(BackgroundStatic)

   // message handlers
   afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
   DECLARE_MESSAGE_MAP()
};

// Possible results
enum elevation_result_t { YES, YES_ALL, NO, NO_ALL };

/////////////////////////////////////////////////////////////////////////////
// CElevationDlg dialog

class CElevationDlg : public CDialog
{
// Construction
public:
   CElevationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CElevationDlg)
   enum { IDD = IDD_YES_NO_ALL };
    CStatic  m_icon;
   HBITMAP m_iconBitmap;
   BackgroundStatic m_static;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CElevationDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CElevationDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnYes();
   afx_msg void OnNo();
   afx_msg void OnYesAll();
   afx_msg void OnNoAll();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   elevation_result_t m_result;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEVATIONDLG_H__923AD822_C639_4CA2_B7C4_E992D7D2849F__INCLUDED_)
