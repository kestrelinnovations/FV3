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



#if !defined(AFX_SCREENADJUST_H__9BFF2063_17DF_11D2_9196_00104B2886FE__INCLUDED_)
#define AFX_SCREENADJUST_H__9BFF2063_17DF_11D2_9196_00104B2886FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScreenAdjust.h : header file
//

#include "getobjpr.h"

#define SCRADJ_TIMER 453453

/////////////////////////////////////////////////////////////////////////////
// CScreenAdjust dialog

class CScreenAdjust : public CDialog
{
// Construction
public:
   int m_nBright;
   int m_nContrast;
   int m_nMidval;
   double m_brightness;
   double m_contrast;
   MapView *m_mapview;
   enum {nMin = -100};
   enum {nMax =  100};
   BOOL m_changed;

   CScreenAdjust(CWnd* pParent = NULL);   // standard constructor

   void wait(int time);  // time in milliseconds
   void set_time(int msec);
   BOOL time_expired();
   int limit(int num, int min, int max);

// Dialog Data
   //{{AFX_DATA(CScreenAdjust)
   enum { IDD = IDD_SCREEN_ADJUST_DLG };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Adjust_Brightness_and_Contrast.htm";}

   CSliderCtrl m_slide_contrast;
   CSliderCtrl m_slide_bright;
   CString m_contrast_val;
   CString m_bright_val;
 //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CScreenAdjust)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CScreenAdjust)
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   afx_msg void OnNormalBright();
   afx_msg void OnKillfocusBrightEdit();
   afx_msg void OnKillfocusContrastEdit();
   afx_msg void OnAuto();
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREENADJUST_H__9BFF2063_17DF_11D2_9196_00104B2886FE__INCLUDED_)