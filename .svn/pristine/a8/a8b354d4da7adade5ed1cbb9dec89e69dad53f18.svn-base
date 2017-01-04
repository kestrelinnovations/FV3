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



// TstMDPP.h : header file
//

#include "catalog/CustomTreeControl.h"

class MapTypeSelectionTree;
class MapType;

/////////////////////////////////////////////////////////////////////////////
// CTestMapDataPP dialog

class CTestMapDataPP : public CPropertyPage
{
   DECLARE_DYNCREATE(CTestMapDataPP)

// Construction
public:
   CTestMapDataPP();
   ~CTestMapDataPP();

   CList<MapType*, MapType*>& GetSelectedMapTypes()
   {
      return m_selectedMapTypes;
   }

// Dialog Data
   //{{AFX_DATA(CTestMapDataPP)
   enum { IDD = IDD_TEST_MAPDATA };
   CustomTreeControl m_tree_ctrl;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CTestMapDataPP)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CTestMapDataPP)
   afx_msg void OnSelectall();
   afx_msg void OnClearall();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   MapTypeSelectionTree* m_pMapTypeSelectionTree;
   CList<MapType*, MapType*> m_selectedMapTypes;
};
