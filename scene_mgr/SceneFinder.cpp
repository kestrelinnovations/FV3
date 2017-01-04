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
#include "SceneFinder.h"

// system includes
#include <iostream>
#include <sstream>
#include <strsafe.h>

// third party files

// other FalconView headers

// this project's headers




#define DIR_WILDCARD "\\**"


namespace scene_mgr
{


SceneFinder::SceneFinder()
{
}


SceneFinder::~SceneFinder()
{
   ClearStack();
}


void SceneFinder::ClearStack()
{
   while (!m_stkhFindData.empty())
   {
      HANDLE h = m_stkhFindData.top().m_hHandle;
      if (h)
         ::FindClose(h);

      m_stkhFindData.pop();
   }
}


void SceneFinder::Initialize(const std::string& strLocalFolderName)
{
   ClearStack();

   m_strLocalFolderName = strLocalFolderName;

   // remove trailing backslash if necessary
   const size_t nSize = m_strLocalFolderName.size();
   if (nSize > 0 && m_strLocalFolderName[nSize - 1] == '\\')
      m_strLocalFolderName = m_strLocalFolderName.substr(0, nSize - 1);

   size_t nPos = m_strLocalFolderName.find(DIR_WILDCARD);
   if (nPos != std::string::npos)
      m_strLocalFolderName.replace(nPos, 3, "");
}


void SceneFinder::AddSearchPath(const std::string& strSearchPath, const std::string& strFilename)
{
   FD_STACK_ELEMENT newElement;
   newElement.m_strFullPath = strSearchPath;

   newElement.m_strCrntSubFolder = GetRelativePath(strSearchPath);
   newElement.m_strFilename = strFilename;

   m_stkhFindData.push(newElement);
}


std::string SceneFinder::GetRelativePath(std::string strPath)
{
   std::string strTemp = m_strLocalFolderName;
   strTemp += "\\";
   size_t nPos = strPath.find(strTemp);
   if (nPos == std::string::npos)
      return "";

   return strPath.substr(nPos + m_strLocalFolderName.size() + 1).c_str();
}


HRESULT SceneFinder::FindNextElement(std::string strLocalFolderName,
                                    FIND_ELEMENT_DATA &elementData,
                                    const CHAR *pstrSubFolderName /*= NULL*/)
{
   WIN32_FIND_DATA findData;

   while(!m_stkhFindData.empty())
   {
      // are we starting a new sub-folder?
      if (m_stkhFindData.top().m_hHandle == NULL)
      {
         std::string strFileName(m_stkhFindData.top().m_strFullPath);

         // if the full path contains a directory wildcard
         size_t pos = strFileName.find(DIR_WILDCARD);
         if (pos != std::string::npos)
         {
            strFileName = strFileName.substr(0, pos);
            strFileName += "\\*";
            m_stkhFindData.top().m_bIsDirWildcard = true;
         }
         // otherwise, just concat the full path and the filename
         else
         {
            strFileName += "\\";
            strFileName += std::string(m_stkhFindData.top().m_strFilename);
         }

         HANDLE hFind = ::FindFirstFile(strFileName.c_str(), &findData);
         if (hFind == INVALID_HANDLE_VALUE)
         {
            m_stkhFindData.pop();
            continue;
         }

         m_stkhFindData.top().m_hHandle = hFind;
      }

      else
      {
         if (::FindNextFile(m_stkhFindData.top().m_hHandle, &findData) == FALSE)
         {
            HANDLE h = m_stkhFindData.top().m_hHandle;
            ::FindClose(h);
            m_stkhFindData.pop();
            continue;
         }
      }

      do
      {
         CharLower(findData.cFileName);
         std::string strFileName(findData.cFileName);  // convert to string for more functionality

         // skip the ".", "..", and any other files & directories that clutter up the map data
         if ( (strFileName.compare(".") == 0) || (strFileName.compare("..") == 0) )
            continue;

         // Check to see if folder or file represents a valid scene
         if(IsPathValid(findData, elementData))  
         {
            return S_OK;
         }

         // If a non-scene folder, then check contents
         else if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
         {
            // save all the info for this folder level in our stack
            FD_STACK_ELEMENT newElement;

            // new element full path is crnt element full path + new sub folder
            newElement.m_strFullPath = m_stkhFindData.top().m_strFullPath;
            newElement.m_strFullPath += "\\";
            newElement.m_strFullPath += strFileName.c_str();

            newElement.m_strCrntSubFolder = m_stkhFindData.top().m_strCrntSubFolder;
            if (newElement.m_strCrntSubFolder.length())
               newElement.m_strCrntSubFolder += "\\";
            newElement.m_strCrntSubFolder += strFileName.c_str();

            newElement.m_strFilename = m_stkhFindData.top().m_strFilename;

            m_stkhFindData.push(newElement);  // push the stack element

            break;  // go back up to top of loop to start new level
         }

      }
      while (::FindNextFile(m_stkhFindData.top().m_hHandle, &findData));

      // No more files at this level

      // if we found anything at this level, clean up our info & pop up 1 level
      if (m_stkhFindData.top().m_hHandle)
      {
         HANDLE h = m_stkhFindData.top().m_hHandle;
         ::FindClose(h);
         m_stkhFindData.pop();
      }
   }

   return S_FALSE;  // all done
}

};  // namespace scene_mgr