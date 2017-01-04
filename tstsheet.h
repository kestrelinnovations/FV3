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



// tstsheet.h : header file
//
#include "resource.h"
#include "TstMvZPP.h"
#include "TstOvlPP.h"
#include "TstMDPP.h"
#include "TstMapProjPP.h"

class CMapAutoTester;

/////////////////////////////////////////////////////////////////////////////
// CTestSheet

class CTestSheet : public CPropertySheet
{
   DECLARE_DYNAMIC(CTestSheet)

// Construction
public:
   CTestSheet(UINT nIDCaption, CWnd* pParentWnd = NULL,
      UINT iSelectPage = 0);
   CTestSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL,
      UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:
   int InitializeTestSuite(CMapAutoTester *pMapAutoTester);


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTestSheet)
   public:
   virtual BOOL OnInitDialog();
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CTestSheet();

   // Generated message map functions
protected:
   //{{AFX_MSG(CTestSheet)
   afx_msg void OnClose();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   CTestMoveZoomPP*  m_moveZoomPP;
   CTestMapDataPP*   m_mapDataPP;
   CTestOvlkitPP*    m_ovlKitPP;
   CTestMapProjPP*   m_mapProjPP;
};

/////////////////////////////////////////////////////////////////////////////
