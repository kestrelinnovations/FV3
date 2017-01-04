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



// stdafx first
#include "stdAfx.h"

// this file's header
#include "CDBHandler.h"

// system includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <ios>

// third party files

// other FalconView headers
#include "../TacticalModel/cdb.h"

// this project's headers
#include "SDSWrapper.h"
#include "SDSGenCovDlg.h"


#define DLG_UPDATE_INTERVAL 1


namespace scene_mgr
{


// CDBSceneFinder implementation

bool CDBSceneFinder::IsPathValid(WIN32_FIND_DATA findData,
   FIND_ELEMENT_DATA &elementData)
{
   // CDB scenes are contained in folders. If object is not a folder, can't be a scene
   if(! (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
   {
      return false;
   }

   // Determine the full path
   std::stringstream fullPathStream;
   if (m_stkhFindData.top().m_strFullPath.size() > 0)
   {
      fullPathStream << m_stkhFindData.top().m_strFullPath << "\\";
   }
   fullPathStream << findData.cFileName;

   // Check to see if the path looks like a CDB root directory
   if(! CCdb::is_valid_cdb_dir( fullPathStream.str().c_str() ) )
   {
      return false;
   }

   // Determine the scene's relative path
   std::stringstream relPathStream;
   if (m_stkhFindData.top().m_strCrntSubFolder.size() > 0)
   {
      relPathStream << m_stkhFindData.top().m_strCrntSubFolder << "\\";
   }
   relPathStream << findData.cFileName;

   // Determine the scene's bounds
   CCdb model;
   model.load(fullPathStream.str().c_str());
   double top = model.m_ur_lat;
   double right = model.m_ur_lon;
   double bottom = model.m_ll_lat;
   double left = model.m_ll_lon;

   // Determine the scene's size on disk
   unsigned long long sizeOnDisk = 0;
   CalculateSize(fullPathStream.str().c_str(), sizeOnDisk);

   // Save the scene metadata
   elementData.m_strSceneName = findData.cFileName;
   elementData.m_strSceneRelativePath = relPathStream.str();
   elementData.m_lSceneTypeId = CDB_TYPE_ID;
   elementData.m_bounds.clear();
   elementData.m_bounds.push_back(left);
   elementData.m_bounds.push_back(bottom);
   elementData.m_bounds.push_back(right);
   elementData.m_bounds.push_back(bottom);
   elementData.m_bounds.push_back(right);
   elementData.m_bounds.push_back(top);
   elementData.m_bounds.push_back(left);
   elementData.m_bounds.push_back(top);
   elementData.m_bounds.push_back(left);
   elementData.m_bounds.push_back(bottom);
   elementData.m_ulSceneSize = (unsigned long) (sizeOnDisk >> 10); // Store size in kB

   return true;
}


void CDBSceneFinder::CalculateSize(std::string _path, unsigned long long &dirSize)
{
   // Check to see if directory size has been cached. If so, retrieve and return size
   std::string sizeFilePath = _path + "\\sdm_cache.txt";
   try {
      std::ifstream sizeFile;
      sizeFile.open(sizeFilePath.c_str());
      if(! sizeFile.fail())
      {
         sizeFile >> dirSize;
         sizeFile.close();
         return;
      }
   }
   catch(...) {}  // If reading fails, we'll just calculated it the old-fashioned way

   WIN32_FIND_DATA data;
   HANDLE sh = NULL;

   sh = FindFirstFile((_path+"\\*").c_str(), &data);

   if (sh == INVALID_HANDLE_VALUE )
   {
      return;
   }

   do
   {
      // skip current and parent
      if (std::string(data.cFileName).compare(".") != 0
         && std::string(data.cFileName).compare("..") != 0)
      {

         // if found object is ...
         if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
            FILE_ATTRIBUTE_DIRECTORY)
         {
            // directory, then search it recursievly
            CalculateSize(_path+"\\"+data.cFileName, dirSize);
         }
         else
         {
            // otherwise get object size and add it to directory size
            dirSize += (__int64) (data.nFileSizeHigh * (MAXDWORD )
               + data.nFileSizeLow);
         }
      }

   } while (FindNextFileA(sh, &data)); // do

   // Save scene size in file
   try {
      std::ofstream sizeFile;
      sizeFile.open(sizeFilePath.c_str());
      if(! sizeFile.fail())
      {
         sizeFile << dirSize;
         sizeFile.close();
      }
   }
   catch(...) {}  // No big deal if we can't create the file

   FindClose(sh);
} 


// CCDBSceneHandler implementation

int CCDBHandler::GenerateCoverage(const std::vector<long>& data_sources,
   HWND hwndProgressDlg /* = NULL */)
{
   long lTotalFiles = 0;
   long lTotalDataSources = 0;

   // Post start message
   if(hwndProgressDlg != NULL)
   {
      // Note that the new instance of covgen_status_t created below will
      // be destroyed by the message handler
      covgen_start_t* pStruct = new covgen_start_t;
      pStruct->str = "CDB";
      ::PostMessage(hwndProgressDlg, SM_COVGEN_START,
         reinterpret_cast<WPARAM>(pStruct), (LPARAM) 0);
   }

   // Update status message
   if (hwndProgressDlg != NULL)
   {
      // Note that the new instance of covgen_status_t created below will
      // be destroyed by the message handler
      covgen_status_t* pStruct = new covgen_status_t;
      pStruct->strDest = "Initializing... Please wait...";
      pStruct->strSrc = "";
      pStruct->lFileCount = 0;
      pStruct->lPercentDone = 0;
      ::PostMessage(hwndProgressDlg, SM_COVGEN_STATUS,
         reinterpret_cast<WPARAM>(pStruct), (LPARAM) 0);
   }

   // Initialize SDSWrapper
   SDSWrapper *pWrapper = SDSWrapper::GetInstance();
   if(pWrapper == nullptr)
   {
      FAIL_WITH_ERROR ("CCDBHandler::GenerateCoverage(): SDSWrapper::GetInstance() failed.");
   }

   // Iterate through scene data sources
   std::string source_path, search_path;
   for(auto pSource = data_sources.begin(); pSource < data_sources.end(); pSource++)
   {
      // Update the count
      ++lTotalDataSources;

      // Retrieve scene data source path
      if(pWrapper->GetSourcePath(&source_path, (*pSource)) != SUCCESS)
      {
         FAIL_WITH_ERROR ("CCDBHandler::GenerateCoverage(): pWrapper->GetSourcePath() failed.");
      }
      search_path = source_path + "\\" + CDB_SUBFOLDER_NAME;

      // Update status message
      if(hwndProgressDlg != NULL)
      {
         // Note that the new instance of covgen_status_t created below will
         // be destroyed by the message handler
         covgen_status_t* pStruct = new covgen_status_t;
         pStruct->strDest = "Deleting old coverage: " + source_path;
         pStruct->strSrc = "";
         pStruct->lFileCount = 0;
         pStruct->lPercentDone = 0;
         ::PostMessage(hwndProgressDlg, SM_COVGEN_STATUS,
            reinterpret_cast<WPARAM>(pStruct), (LPARAM) 0);
      }

      // Delete existing coverage for source
      pWrapper->DeleteScenesBySourceAndType(*pSource, CDB_TYPE_ID);

      // Update status message
      if (hwndProgressDlg != NULL)
      {
         // Note that the new instance of covgen_status_t created below will
         // be destroyed by the message handler
         covgen_status_t* pStruct = new covgen_status_t;
         pStruct->strDest = "Accessing scene data: " + source_path;
         pStruct->strSrc = "";
         pStruct->lFileCount = 0;
         pStruct->lPercentDone = 0;
         ::PostMessage(hwndProgressDlg, SM_COVGEN_STATUS,
            reinterpret_cast<WPARAM>(pStruct), (LPARAM) 0);
      }

      // Search for CDB scenes in the appropriate source subfolder
      CDBSceneFinder finder;
      finder.Initialize(source_path);
      finder.AddSearchPath(source_path, "*");

      FIND_ELEMENT_DATA element;
      while(finder.FindNextElement(source_path, element) == S_OK)
      {
         lTotalFiles++;

         // Update the progress dialog occasionally
         if(hwndProgressDlg != NULL && (lTotalFiles % DLG_UPDATE_INTERVAL) == 0)
         {
            // Note that the new instance of covgen_status_t created below will
            // be destroyed by the message handler
            covgen_status_t* pStruct = new covgen_status_t;
            pStruct->strDest = "Adding scene: " + source_path + "\\" + element.m_strSceneRelativePath;
            pStruct->strSrc = "";
            pStruct->lFileCount = lTotalFiles;
            pStruct->lPercentDone = 0;
            ::PostMessage(hwndProgressDlg, SM_COVGEN_STATUS,
               reinterpret_cast<WPARAM>(pStruct), (LPARAM) 0);
         }

         // Add found scene to the database
         pWrapper->AddScene(element.m_strSceneName, element.m_strSceneRelativePath,
            (*pSource), CDB_TYPE_ID, element.m_bounds, element.m_ulSceneSize);
      }
   }

   if(hwndProgressDlg != NULL)
   {
      // Post stop message
      ::PostMessage(hwndProgressDlg, SM_COVGEN_STOP, (WPARAM) 0, (LPARAM) 0);
   }

   return SUCCESS;
}
};  // namespace scene_mgr