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
#include "SceneHandler.h"

// system includes

// third party files

// other FalconView headers

// this project's headers
#include "CDBHandler.h"
#include "TTVSHandler.h"


namespace scene_mgr
{

// CSceneHandlerFactory implementation

int CSceneHandlerFactory::GetHandler(long handler_type, ISceneHandler** ppHandler)
{
   try
   {
      switch(handler_type)
      {
      case CDB_TYPE_ID:
         *ppHandler = new CCDBHandler();
         return SUCCESS;
      case TTVS_TYPE_ID:
         *ppHandler = new CTTVSHandler();
         return SUCCESS;
      default:
         *ppHandler = nullptr;
         FAIL_WITH_ERROR ("CSceneHandlerFactory::GetHandler(): Invalid handler type.");
      }
   }
   catch(std::bad_alloc&)
   {
      *ppHandler = nullptr;
      FAIL_WITH_ERROR ("CSceneHandlerFactory::GetHandler(): SceneHandler creation failed.");
   }
}

};  // namespace scene_mgr
