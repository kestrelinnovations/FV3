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
#include "stdafx.h"

// this file's header
#include "SMOperations.h"

// system includes
#include <afxwin.h>
#include <memory>
#include <sstream>
#include <vector>
#include <Windows.h>

// third party files

// other FalconView headers
#include "FvCore/include/registry.h"
#include "include/scenes/ScenesDataView.h"
#include "include/scenes/SceneFeature.h"

// this project's headers
#include "SDSWrapper.h"



namespace scene_mgr
{

   // SMCopyOperation implementation
   //
   SMCopyOperation::SMCopyOperation(
      long source_id,
      const std::string &scene_name,
      const std::string &scene_path,
      long scene_type,
      const std::vector<double> &bounds,
      unsigned long scene_size,
      const std::string &copy_from_path,
      const std::string &copy_to_path )
   {
      m_source_id = source_id;
      m_scene_name = scene_name;
      m_scene_path = scene_path;
      m_scene_type = scene_type;
      m_bounds = bounds;
      m_scene_size = scene_size;

      std::stringstream stream;
      stream << copy_from_path << std::ends;
      m_copy_from_path = stream.str();

      stream.str("");
      stream.clear();
      stream << copy_to_path << std::ends;
      m_copy_to_path = stream.str();
   }

   int SMCopyOperation::Execute(HWND hwnd_parent)
   {
      // Perform the copy
      SHFILEOPSTRUCT FileOpStruct;
      ZeroMemory(&FileOpStruct, sizeof(SHFILEOPSTRUCT));
      FileOpStruct.wFunc = FO_COPY;
      FileOpStruct.fFlags = FOF_SIMPLEPROGRESS | FOF_MULTIDESTFILES |
         FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION;
      FileOpStruct.pFrom = m_copy_from_path.c_str();
      FileOpStruct.pTo = m_copy_to_path.c_str();
      FileOpStruct.lpszProgressTitle = "Scene Data Manager";
      FileOpStruct.hwnd = hwnd_parent;
      int status = SHFileOperation(&FileOpStruct);
      if (status != SUCCESS)
      {
         return status;
      }

      // Add the scene to the database
      return SDSWrapper::GetSecondaryInstance()->AddScene(m_scene_name, m_scene_path,
         m_source_id, m_scene_type, m_bounds, m_scene_size);
   }



   // SMDeleteOperation implementation
   //
   SMDeleteOperation::SMDeleteOperation(
      long scene_identity,
      const std::string &delete_path)
   {
      m_scene_identity = scene_identity;

      std::stringstream stream;
      stream << delete_path << std::ends;
      m_delete_path = stream.str();
   }

   int SMDeleteOperation::Execute(HWND hwnd_parent)
   {
      // Delete the record from the database
      int status = SDSWrapper::GetSecondaryInstance()->DeleteScene(m_scene_identity);
      if (status != SUCCESS)
      {
         return status;
      }

      // Perform the delete
      SHFILEOPSTRUCT FileOpStruct;
      ZeroMemory(&FileOpStruct, sizeof(SHFILEOPSTRUCT));
      FileOpStruct.wFunc = FO_DELETE;
      FileOpStruct.fFlags = FOF_SIMPLEPROGRESS | FOF_NOCONFIRMATION;
      FileOpStruct.pFrom = m_delete_path.c_str();
      FileOpStruct.lpszProgressTitle = "Scene Data Manager";
      FileOpStruct.hwnd = hwnd_parent;
      return SHFileOperation(&FileOpStruct);
   }

};  // namespace scene_mgr
