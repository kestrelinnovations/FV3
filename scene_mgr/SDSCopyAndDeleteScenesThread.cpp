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

// SDSCopyAndDeleteScenesThread.cpp : implementation file
//



// stdafx first
#include "stdafx.h"

// this file's header
#include "SDSCopyAndDeleteScenesThread.h"

// system includes
#include <Windows.h>
#include <afxwin.h>
#include <memory>
#include <sstream>

// third party files

// other FalconView headers
#include "common.h"
#include "FvCore/include/registry.h"
#include "include/scenes/ScenesDataView.h"

// this project's headers
#include "SMManagePage.h"
#include "SMOperations.h"
#include "SDSWrapper.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSDSCopyAndDeleteScenesThread

namespace scene_mgr
{

IMPLEMENT_DYNCREATE(CSDSCopyAndDeleteScenesThread, CWinThread)

CSDSCopyAndDeleteScenesThread::CSDSCopyAndDeleteScenesThread()
{
   m_hwnd_parent = NULL;
}

CSDSCopyAndDeleteScenesThread::~CSDSCopyAndDeleteScenesThread()
{
}

BOOL CSDSCopyAndDeleteScenesThread::InitInstance()
{
   return TRUE;
}

int CSDSCopyAndDeleteScenesThread::ExitInstance()
{
   return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSDSCopyAndDeleteScenesThread, CWinThread)
   //{{AFX_MSG_MAP(CSDSCopyAndDeleteScenesThread)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDSCopyAndDeleteScenesThread message handlers


void CSDSCopyAndDeleteScenesThread::SetParentWindow(HWND hwnd_parent)
{
   m_hwnd_parent = hwnd_parent;
}

void CSDSCopyAndDeleteScenesThread::Add(const SMCopyOperation& new_op)
{
   m_copy_queue.push_back(new_op);
}


void CSDSCopyAndDeleteScenesThread::Add(const SMDeleteOperation& new_op)
{
   m_delete_queue.push_back(new_op);
}


int CSDSCopyAndDeleteScenesThread::Run()
{
   int status = SUCCESS;

   // Perform the deletes
   for(auto it = m_delete_queue.begin(); it != m_delete_queue.end(); it++)
   {
      status = (*it).Execute(m_hwnd_parent);
      if(status != SUCCESS)
         break;
   }

   // If deletes were successful, perform the copies
   if(status == SUCCESS)
   {
      for(auto it = m_copy_queue.begin(); it != m_copy_queue.end(); it++)
      {
         status = (*it).Execute(m_hwnd_parent);
         if(status != SUCCESS)
            break;
      }
   }

   ::PostMessageA(m_hwnd_parent, SM_COPYDELETE_COMPLETE, 0, 0);
   return status;
}

};  // namespace scene_mgr