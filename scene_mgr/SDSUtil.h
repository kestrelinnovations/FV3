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



// SDSUtil.h : Declaration of the CSDSUtil

#ifndef __SCENEMANAGER_SDSUTIL_H_
#define __SCENEMANAGER_SDSUTIL_H_

#include <vector>
#include "err.h"


#define MAX_TIME_BUFFER_SIZE    128
#define S_BCP_LOADED 0x0000013a

/////////////////////////////////////////////////////////////////////////////
// CSDSUtil


namespace scene_mgr
{


class CSDSUtil 
{
protected:

public:

   // CSDSUtil is a singleton
   //
   static CSDSUtil *GetInstance()
   {
      if (s_pInstance == NULL)
      {
         s_pInstance = new CSDSUtil();

         if (s_pInstance->init() != SUCCESS)
            ERR_report("SDSWrapper::init failed.");
      }

      return s_pInstance;
   }

   static void DestroyInstance()
   {
      if (s_pInstance != NULL)
      {
         if (s_pInstance->uninit() != SUCCESS)
            ERR_report("SDSWrapper::uninit failed.");
      }
      delete s_pInstance;
   }

   int init();
   int uninit();

protected:

   CSDSUtil() :
      m_mainWindow(0),
      m_bInitialized(false)
   {
   }

   bool m_bInitialized;

   static CSDSUtil *s_pInstance;

public:
   HRESULT GenCov(const std::vector<long>& Ids);
   HRESULT SetMainWindow(HWND MainWindow);

private:

   // Mechs to sync multiple covgen / load bcp requests.
   std::vector<long> m_current_change_ids;
   HWND m_mainWindow;
};

};  // namespace scene_mgr

#endif // #ifndef __SCENEMANAGER_SDSUTIL_H_
