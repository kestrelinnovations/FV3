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



// TstMapProjPP.cpp : implementation file
//

#include "stdafx.h"
#include "TstMapProjPP.h"
#include "proj.h"
#include "testglob.h"

/*
#include "OvlFctry.h"
*/

/////////////////////////////////////////////////////////////////////////////
// CTestMapProjPP property page

IMPLEMENT_DYNCREATE(CTestMapProjPP, CPropertyPage)

CTestMapProjPP::CTestMapProjPP() : CPropertyPage(CTestMapProjPP::IDD)
{
   //{{AFX_DATA_INIT(CTestMapProjPP)
   m_testMapProjs = FALSE;
   m_test3d = FALSE;
   //}}AFX_DATA_INIT
}

CTestMapProjPP::~CTestMapProjPP()
{
}

void CTestMapProjPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTestMapProjPP)
   DDX_Check(  pDX, IDC_TEST_MAP_PROJ_CHECK, m_testMapProjs);
   DDX_Control(pDX, IDC_TEST_MAP_PROJ_LIST,  m_ctrlMapProjListBox);
   DDX_Check(  pDX, IDC_TEST_STD_MODE_CHECK, m_test3DStdMode);
   DDX_Check(  pDX, IDC_TEST_FLT_MODE_CHECK, m_test3DFltMode);
   DDX_Check(  pDX, IDC_TEST_GND_MODE_CHECK, m_test3DGndMode);
   DDX_Check(  pDX, IDC_TEST_TILT_CHECK, m_test3DTilt);
   DDX_Check(  pDX, IDC_TEST_EXAG_ELEV_CHECK, m_test3DElevExaggeration);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestMapProjPP, CPropertyPage)
   //{{AFX_MSG_MAP(CTestMapProjPP)
   ON_BN_CLICKED(IDC_TEST_MAP_PROJ_CHECK, OnClickTest2DProjs)
   ON_LBN_SELCHANGE(IDC_TEST_MAP_PROJ_LIST, OnSelChangedMapProjList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestMapProjPP message handlers

void CTestMapProjPP::OnClickTest2DProjs() 
{
   UpdateData(TRUE);
   UpdateControlStatuses();
}

void CTestMapProjPP::OnSelChangedMapProjList()
{
   UpdateData(TRUE);

   m_test3d = m_testMapProjs && IsGeneralPerspectiveSelected();
   UpdateControlStatuses();
}

BOOL CTestMapProjPP::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   // reset the contents of the map proj list box
   m_ctrlMapProjListBox.ResetContent();

   // Add the projections
   CArray <class ProjectionID, const class ProjectionID&> available_projection_array;
   PROJ_get_available_projection_types(&available_projection_array, TRUE);
   for (int z=0; z<available_projection_array.GetSize(); z++)
   {
      CString title = available_projection_array.GetAt(z).get_string();
      ProjectionEnum proj = available_projection_array.GetAt(z).GetProjectionType();
      int nIndex = m_ctrlMapProjListBox.AddString(title);
      m_ctrlMapProjListBox.SetItemDataPtr(nIndex, reinterpret_cast<void *>(proj));
   }

   // Disable the controls until they are explicitly enabled
   UpdateControlStatuses();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTestMapProjPP::OnApply() 
{
   // Make sure data members are up-to-date
   UpdateData(TRUE);

   // if the static overlay toggles option was selected on the Test Settings
   // property sheet, get the list of static overlays selected for toggling
   // from the CTestOvlkitPP property page list box...
   if(m_testMapProjs)
   {
      CArray<int, int> index_list;  // to store indexes of projections selected from the list box
      int num_selected_projections = m_ctrlMapProjListBox.GetSelCount();

      // set the size then populate the list of indexes for the current set of
      // selected projections...
      index_list.SetSize(num_selected_projections);
      m_ctrlMapProjListBox.GetSelItems(num_selected_projections,
         index_list.GetData());

      for(int i=0; i<num_selected_projections; i++)
      {
         int proj_id = reinterpret_cast<int>(
            m_ctrlMapProjListBox.GetItemDataPtr(index_list.GetAt(i)));
         ProjectionEnum pr_enum = (ProjectionEnum) proj_id;
         m_selectedMapProjections.AddTail(pr_enum);
      }
   }

   // Get the list of camera modes to be tested
   m_selectedCameraModes.RemoveAll();
   if(m_test3d)
   {
      if(m_test3DFltMode)
         m_selectedCameraModes.AddTail(fvw::CAMERA_MODE_FLIGHT);
      if(m_test3DStdMode)
         m_selectedCameraModes.AddTail(fvw::CAMERA_MODE_PAN);
      if(m_test3DGndMode)
         m_selectedCameraModes.AddTail(fvw::CAMERA_MODE_GROUND_VIEW);
   }

   return CPropertyPage::OnApply();
}

BOOL CTestMapProjPP::IsGeneralPerspectiveSelected()
{
   CArray<int, int> index_list;  // to retrieve indexes of selected projections
   int num_selected_projections = m_ctrlMapProjListBox.GetSelCount();

   // set the size then populate the list of indexes for the current set of
   // selected projections...
   index_list.SetSize(num_selected_projections);
   m_ctrlMapProjListBox.GetSelItems(num_selected_projections,
      index_list.GetData());

   BOOL is_3d_selected = FALSE;
   for(int i=0; i<num_selected_projections; i++)
   {
      int proj_id = reinterpret_cast<int>(
         m_ctrlMapProjListBox.GetItemDataPtr(index_list.GetAt(i)));
      ProjectionEnum pr_enum = (ProjectionEnum) proj_id;
      if(pr_enum == GENERAL_PERSPECTIVE_PROJECTION)
      {
         is_3d_selected = TRUE;
         break;
      }
   }

   return is_3d_selected;
}

void CTestMapProjPP::UpdateControlStatuses()
{
   GetDlgItem(IDC_TEST_MAP_PROJ_LIST)->EnableWindow(m_testMapProjs);
   GetDlgItem(IDC_TEST_STD_MODE_CHECK)->EnableWindow(m_testMapProjs && m_test3d);
   GetDlgItem(IDC_TEST_FLT_MODE_CHECK)->EnableWindow(m_testMapProjs && m_test3d);
   GetDlgItem(IDC_TEST_GND_MODE_CHECK)->EnableWindow(m_testMapProjs && m_test3d);
   GetDlgItem(IDC_TEST_TILT_CHECK)->EnableWindow(m_testMapProjs && m_test3d);
   GetDlgItem(IDC_TEST_EXAG_ELEV_CHECK)->EnableWindow(m_testMapProjs && m_test3d);
}

BOOL CTestMapProjPP::TestProjections()
{
   return m_testMapProjs;
}

BOOL CTestMapProjPP::Test3DMapTilt()
{
   return m_test3d && m_test3DTilt;
}

BOOL CTestMapProjPP::Test3DCameraModes()
{
   return m_test3d && (m_test3DStdMode || m_test3DFltMode || m_test3DGndMode);
}

BOOL CTestMapProjPP::Test3DElevationExaggeration()
{
   return m_test3d && m_test3DElevExaggeration;
}

