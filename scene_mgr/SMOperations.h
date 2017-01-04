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



// SMPathsPage.h : implementation file
//

#ifndef __SCENEMANAGER_SMOPERATIONS_H
#define __SCENEMANAGER_SMOPERATIONS_H


#include "include/scenes/ScenesDataView.h"


namespace scene_mgr
{

// SMOperation

class SMOperation
{
public:
   virtual int Execute(HWND hwnd_parent) = 0;
};

class SMCopyOperation : public SMOperation
{
public:
   SMCopyOperation(
      long source_id,
      const std::string &scene_name,
      const std::string &scene_path,
      long scene_type,
      const std::vector<double> &bounds,
      unsigned long scene_size,
      const std::string &copy_from_path,
      const std::string &copy_to_path );

   virtual ~SMCopyOperation() {}

   int Execute(HWND hwnd_parent);

private:
   long                m_source_id;
   std::string         m_scene_name;
   std::string         m_scene_path;
   long                m_scene_type;
   std::vector<double> m_bounds;
   unsigned long       m_scene_size;
   std::string         m_copy_from_path;
   std::string         m_copy_to_path;
};

class SMDeleteOperation : public SMOperation
{
public:
   SMDeleteOperation(
      long scene_identity,
      const std::string &scene_path );

   virtual ~SMDeleteOperation() {}

   int Execute(HWND hwnd_parent);

private:
   long        m_scene_identity;
   std::string m_delete_path;
};


};  // namespace scene_mgr

#endif  // __SCENEMANAGER_SMOPERATIONS_H
