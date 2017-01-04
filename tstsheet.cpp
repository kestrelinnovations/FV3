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



// tstsheet.cpp : implementation file
//

#include "stdafx.h"
#include "tstsheet.h"
#include "testglob.h"
#include "fvwutil.h"

/////////////////////////////////////////////////////////////////////////////
// CTestSheet

IMPLEMENT_DYNAMIC(CTestSheet, CPropertySheet)

CTestSheet::CTestSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   m_mapDataPP  = new CTestMapDataPP();
   m_moveZoomPP = new CTestMoveZoomPP();
   m_ovlKitPP   = new CTestOvlkitPP();
   m_mapProjPP  = new CTestMapProjPP();
   AddPage(m_moveZoomPP);
   AddPage(m_ovlKitPP);
   AddPage(m_mapDataPP);
   AddPage(m_mapProjPP);
}

CTestSheet::CTestSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   m_mapDataPP  = new CTestMapDataPP();
   m_moveZoomPP = new CTestMoveZoomPP();
   m_ovlKitPP   = new CTestOvlkitPP();
   m_mapProjPP  = new CTestMapProjPP();
   AddPage(m_moveZoomPP);
   AddPage(m_ovlKitPP);
   AddPage(m_mapDataPP);
   AddPage(m_mapProjPP);
}

CTestSheet::~CTestSheet()
{
   delete m_mapDataPP;
   delete m_moveZoomPP;
   delete m_ovlKitPP;
   delete m_mapProjPP;
}


BEGIN_MESSAGE_MAP(CTestSheet, CPropertySheet)
   //{{AFX_MSG_MAP(CTestSheet)
   ON_WM_CLOSE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestSheet message handlers
BOOL CTestSheet::OnInitDialog() 
{
   return CPropertySheet::OnInitDialog();
}

/////////////////////////////////////////////////////////////////////////////
// CTestSheet public functions
int CTestSheet::InitializeTestSuite(CMapAutoTester *pMapAutoTester)
{
   ////// set the test flags based on input from the property pages

   //// map data
   pMapAutoTester->SetSelectedMapTypes(m_mapDataPP->GetSelectedMapTypes());

   //// map projections
   pMapAutoTester->SetSelectedMapProjections(m_mapProjPP->GetSelectedMapProjections());

   //// 3D camera modes
   pMapAutoTester->SetSelectedCameraModes(m_mapProjPP->GetSelectedCameraModes());

   //// Map proj, tilt, camera mode, elevation exaggeration, move & zoom settings
   // Test map proj?
   if (m_mapProjPP->TestProjections())
      pMapAutoTester->SetFlags(TEST_MAP_PROJ);
   // Test map tilt?
   if (m_mapProjPP->Test3DMapTilt())
      pMapAutoTester->SetFlags(TEST_3D_MAP_TILT);
   // Test 3d modes?
   if (m_mapProjPP->Test3DCameraModes())
      pMapAutoTester->SetFlags(TEST_3D_CAMERA_MODES);
   // Test elevation exaggeration?
   if (m_mapProjPP->Test3DElevationExaggeration())
      pMapAutoTester->SetFlags(TEST_3D_ELEV_EXAG);
   // Test map move?
   if (m_moveZoomPP->m_testMove)
      pMapAutoTester->SetFlags(TEST_MAP_MOVE);
   // Test scale?
   if (m_moveZoomPP->m_testScale)
      pMapAutoTester->SetFlags(TEST_MAP_SCALE);
   // Test zoom?
   if (m_moveZoomPP->m_testZoom)
      pMapAutoTester->SetFlags(TEST_MAP_ZOOM);
   // CONUS only?
   if (m_moveZoomPP->m_conusOnly)
      pMapAutoTester->SetFlags(TEST_CONUS_ONLY);

   pMapAutoTester->SetRandomSeed(m_moveZoomPP->m_nRandomSeed);
   pMapAutoTester->SetWriteToErrorLog(m_moveZoomPP->m_bWriteToErrorLog);

   //// Overlay settings
   // Test overlay toggles?
   if (m_ovlKitPP->m_testOvlToggle)
      pMapAutoTester->SetFlags(TEST_OVL_TOGGLE);
   // Test opening and closing of overlay files?
   if (m_ovlKitPP->m_testOpenCloseOvlFile)
   {
      pMapAutoTester->SetFlags(TEST_OPEN_OVL_FILE);
      pMapAutoTester->SetFlags(TEST_CLOSE_OVL_FILE);
   }

   //// General
   // How many iterations?
   pMapAutoTester->SetIterations(m_moveZoomPP->m_numIterations);

   // if no test type flags were checked - abort test
   if ((gTestFlags & TEST_TYPE_ALL) == 0)
      return FAILURE;


   // if the open/close overlay files option was selected, prompt the user
   // for the path to the test file folder then populate the test file array
   // with all fvw overlay files found in the specified test file folder...
   if (gTestFlags & TEST_OPEN_OVL_FILE)
   {
      // clear globals
      gTestFileFolderPath = "";
      gTestFileNames.RemoveAll();
      
      // open a directory selection dialog box to get the name of the test file folder
      CFvwUtil *futil = CFvwUtil::get_instance();
      gTestFileFolderPath = futil->get_path_from_user("Select path for overlay test files");

      // get the first test file name in the test file folder
      WIN32_FIND_DATA findFileData;
      HANDLE hFindFile = FindFirstFile(gTestFileFolderPath + "*.*", &findFileData);

      if (hFindFile != INVALID_HANDLE_VALUE)
      {
         // define the test file path...
         CString test_file_path = gTestFileFolderPath + findFileData.cFileName;

         if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)  // if this is not a directory
         {
            // store the path for the test file...
            gTestFileNames.Add(test_file_path);
         }

         // now get the remaining test file names in the test file folder
         BOOL bFilesRemain = 1;
         while (bFilesRemain)
         {
            bFilesRemain = FindNextFile(hFindFile, &findFileData);

            if (bFilesRemain)
            {
               // define the test file path...
               test_file_path = gTestFileFolderPath + findFileData.cFileName;

               if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)  // if this is not a directory
               {
                  // store the path for the test file...
                  gTestFileNames.Add(test_file_path);
               }
            }
         }
      }
   }

   return SUCCESS;
}

void CTestSheet::OnClose() 
{
   // TODO: Add your message handler code here and/or call default
   
   CPropertySheet::OnClose();
}
