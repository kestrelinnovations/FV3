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


// SDSUtil.cpp : Implementation of CSDSUtil


// stdafx first
#include "stdAfx.h"

// this file's header
#include "SDSUtil.h"

// system includes
#include <vector>

// third party files

// other FalconView headers

// this project's headers
#include "SDSGenCovProgress.h"


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CSDSUtil

CSDSUtil *CSDSUtil::s_pInstance = NULL;


int CSDSUtil::init()
{
   REPORT_ENTERING ("CSDSUtil::init()");

   if(m_bInitialized)
      return SUCCESS;

   m_bInitialized = true;  // Initialized at this point

   return SUCCESS;
}

int CSDSUtil::uninit()
{
   REPORT_ENTERING ("CSDSUtil::uninit()");

   if(!m_bInitialized)
      return SUCCESS;

   m_bInitialized = false;

   return SUCCESS;
}

HRESULT CSDSUtil::GenCov(const std::vector<long>& Ids)
{
   m_current_change_ids = Ids;

   if (Ids.size() > 0)
   {
      // Generate coverage.
      CSDSGenCovProgress coverage_gen;
      coverage_gen.SetParent(m_mainWindow);
      return coverage_gen.GenCov(Ids, NULL);
   }

   return S_OK;
}


HRESULT CSDSUtil::SetMainWindow(HWND MainWindow)
{
  m_mainWindow = MainWindow;
  return S_OK;
}

};  // namespace scene_mgr
