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



#ifndef _SCENEMANAGER_SCENEFINDER_H
#define _SCENEMANAGER_SCENEFINDER_H

#include <Windows.h>
#include <stack>
#include <string>


namespace scene_mgr
{

struct FD_STACK_ELEMENT
{
   FD_STACK_ELEMENT()
   {
      m_hHandle = NULL;
      m_bIsDirWildcard = false;
   }
   HANDLE      m_hHandle;
   bool        m_bIsDirWildcard;
   std::string m_strCrntSubFolder;
   std::string m_strFullPath;
   std::string m_strFilename;
};


struct FIND_ELEMENT_DATA
{
   FIND_ELEMENT_DATA()
   {
      m_lSceneTypeId = 0;
   }

   std::string m_strSceneName;
   std::string m_strSceneRelativePath;
   long        m_lSceneTypeId;
   std::vector<double> m_bounds;
   unsigned long m_ulSceneSize;
};


class SceneFinder
{
public:
   SceneFinder();
   virtual ~SceneFinder();

   virtual void Initialize(const std::string& strLocalFolderName);
   void AddSearchPath(const std::string& strSearchPath, const std::string& strFilename);
   std::string GetRelativePath(std::string strPath);
   HRESULT FindNextElement(std::string strLocalFolderName,
      FIND_ELEMENT_DATA &elementData, const CHAR *pstrSubFolderName = NULL);
   virtual bool IsPathValid(WIN32_FIND_DATA findData,
      FIND_ELEMENT_DATA &elementData) = 0;

protected:

   void ClearStack();

   std::stack<FD_STACK_ELEMENT> m_stkhFindData;
   std::string m_strLocalFolderName;
};


};  // namespace scene_mgr


#endif  // #ifndef _SCENEMANAGER_SCENEFINDER_H