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



// TstOvlPP.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTestOvlkitPP dialog

class CTestOvlkitPP : public CPropertyPage
{
   DECLARE_DYNCREATE(CTestOvlkitPP)

// Construction
public:
   CTestOvlkitPP();
   ~CTestOvlkitPP();

// Dialog Data
   //{{AFX_DATA(CTestOvlkitPP)
   enum { IDD = IDD_TEST_OVLKIT };
   CListBox m_ctrlStaticOvlListBox;
   BOOL  m_testOvlToggle;
   BOOL  m_testOpenCloseOvlFile;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CTestOvlkitPP)
   public:
   virtual BOOL OnApply();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CTestOvlkitPP)
   afx_msg void OnClickTestStaticOvlToggles();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};
