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



// TstMvZPP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestMoveZoomPP dialog

class CTestMoveZoomPP : public CPropertyPage
{
   DECLARE_DYNCREATE(CTestMoveZoomPP)

// Construction
public:
   CTestMoveZoomPP();
   ~CTestMoveZoomPP();

// Dialog Data
   //{{AFX_DATA(CTestMoveZoomPP)
   enum { IDD = IDD_TEST_MOVE_ZOOM };
   BOOL  m_testMove;
   UINT  m_numIterations;
   BOOL  m_conusOnly;
   BOOL  m_testScale;
   BOOL  m_testZoom;
   int m_nRandomSeed;
   BOOL m_bWriteToErrorLog;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CTestMoveZoomPP)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CTestMoveZoomPP)
   afx_msg void OnClickTestMove();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};
