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


// SDSGenCovProgress.h : Declaration of the CSDSGenCovProgress


#ifndef __SCENEMANAGER_SDSGENCOVPROGRESS_H_
#define __SCENEMANAGER_SDSGENCOVPROGRESS_H_


#include <vector>


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CSDSGenCovProgress

class SDSGenCovThread;
class CSDSGenCovDlg;

class CSDSGenCovProgress
{
public:
   CSDSGenCovProgress()
   {
      m_hwndParent = NULL;
      m_pGenCovDlg = nullptr;
   }

   SDSGenCovThread *m_pthreadGenCov;

   HRESULT GenCov(/*[in]*/const std::vector<long>& vectDataSources, /*[in]*/void *pStatusCallback);
   HWND GetParent();
   void SetParent(HWND newVal);

private:
   HWND m_hwndParent;
   CSDSGenCovDlg *m_pGenCovDlg;
};

};  // namespace scene_mgr

#endif //__SCENEMANAGER_SDSGENCOVPROGRESS_H_
