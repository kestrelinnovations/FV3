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



// TstMapProjPP.h : header file
//


#ifndef _TSTMAPPROJPP_H
#define _TSTMAPPROJPP_H


#include "resource.h"
#include "CameraModes.h"


/////////////////////////////////////////////////////////////////////////////
// CTestMapProjPP dialog

class CTestMapProjPP : public CPropertyPage
{
   DECLARE_DYNCREATE(CTestMapProjPP)

// Construction
public:
   CTestMapProjPP();
   ~CTestMapProjPP();

// Dialog Data
   //{{AFX_DATA(CTestMapProjPP)
   enum { IDD = IDD_TEST_MAPPROJ_PP };

   BOOL m_testMapProjs;
   BOOL m_test3d;
   BOOL m_test3DStdMode;
   BOOL m_test3DFltMode;
   BOOL m_test3DGndMode;
   BOOL m_test3DTilt;
   BOOL m_test3DElevExaggeration;
   CListBox m_ctrlMapProjListBox;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CTestMapProjPP)
public:
   virtual BOOL OnApply();

   CList<ProjectionEnum, ProjectionEnum>& GetSelectedMapProjections()
   {
      return m_selectedMapProjections;
   }

   CList<fvw::CameraMode, fvw::CameraMode>& GetSelectedCameraModes()
   {
      return m_selectedCameraModes;
   }

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CTestMapProjPP)
   afx_msg void OnClickTest2DProjs();
   afx_msg void OnSelChangedMapProjList();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   CList<ProjectionEnum, ProjectionEnum> m_selectedMapProjections;
   CList<fvw::CameraMode, fvw::CameraMode> m_selectedCameraModes;

   // Helper member functions
   BOOL IsGeneralPerspectiveSelected();
   void UpdateControlStatuses();

public:
   BOOL TestProjections();
   BOOL Test3DMapTilt();
   BOOL Test3DCameraModes();
   BOOL Test3DElevationExaggeration();
};

#endif  // #ifndef _TSTMAPPROJPP_H
