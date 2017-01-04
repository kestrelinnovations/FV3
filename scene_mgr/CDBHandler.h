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



#ifndef _SCENEMANAGER_CDBHANDLER_H
#define _SCENEMANAGER_CDBHANDLER_H


#include "SceneFinder.h"
#include "SceneHandler.h"
#include <vector>

#define CDB_TYPE_ID 1
#define CDB_SUBFOLDER_NAME "CDB"


namespace scene_mgr
{

class CDBSceneFinder : public SceneFinder
{
public:
   virtual bool IsPathValid(WIN32_FIND_DATA findData,
      FIND_ELEMENT_DATA &elementData);

private:
   void CalculateSize(std::string _path, unsigned long long & dirSize);
};

class CCDBHandler : public ISceneHandler
{
public:
   virtual int GenerateCoverage(const std::vector<long>& data_sources,
      HWND hwndProgressDlg = NULL);
};

};  // namespace scene_mgr

#endif  // #ifndef _SCENEMANAGER_CDBHANDLER_H