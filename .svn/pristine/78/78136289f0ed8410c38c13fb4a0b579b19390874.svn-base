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



// GenCovThread.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SDSGenCovThread.h"

// system includes
#include <Windows.h>
#include <afxwin.h>
#include <memory>

// third party files

// other FalconView headers
#include "include\scenes\TypeEnumerator.h"

// this project's headers
#include "SceneHandler.h"
#include "SDSGenCovDlg.h"
#include "SDSWrapper.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// See public\fvw_core\MdsUtilities\MdsUtilities.cpp
extern DWORD PumpMessages(HANDLE hThread);


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// GenCovThread

IMPLEMENT_DYNCREATE(SDSGenCovThread, CWinThread)

SDSGenCovThread::SDSGenCovThread()
{
   m_hWorkerThread = NULL;
   m_hwndParent = NULL;
}

SDSGenCovThread::~SDSGenCovThread()
{
}

BOOL SDSGenCovThread::InitInstance()
{
   // TODO:  perform and per-thread initialization here
   return TRUE;
}

int SDSGenCovThread::ExitInstance()
{
   // TODO:  perform any per-thread cleanup here
   return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(SDSGenCovThread, CWinThread)
   //{{AFX_MSG_MAP(SDSGenCovThread)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GenCovThread message handlers


DWORD WINAPI WorkerThread(LPVOID lpParameter);



int SDSGenCovThread::Run() 
{
#if 0
   try
   {
#endif
      // Create the worker thread
      m_hWorkerThread = ::CreateThread(NULL, 0, &WorkerThread,
         (LPVOID) this, 0, NULL);

      // pump messages until worker thread is done.
      PumpMessages(m_hWorkerThread);

#if 0
      try
      {
         // was there an exception in the thread?
         if (m_errWorkerThread.Error())
         {
            if (pRemoteStatusCallback)
               pRemoteStatusCallback->StatusEnd(m_errWorkerThread.Error(), "",
                  IErrorInfoPtr(m_errWorkerThread.ErrorInfo(), false));
         }
         else
         {
            // all done
            if (pRemoteStatusCallback)
            {
               HRESULT hr = S_OK;
               if (m_bstrOfflineDSs)  // if there were offline DS's, set error code for StatusEnd
                  hr = E_FAIL;
               else if (exitcode == E_ABORT)
                  hr = E_ABORT;
               pRemoteStatusCallback->StatusEnd(hr, (BSTR)m_bstrOfflineDSs, NULL);
            }
         }
      }
      catch (_com_error &err)
      {
         ComErrorPopup(err);  // We should not be getting exceptions from StatusEnd
      }
   }

   catch (_com_error &err)
   {
      if (pRemoteStatusCallback)
      {
         try
         {
            pRemoteStatusCallback->StatusEnd(err.Error(), "", IErrorInfoPtr(err.ErrorInfo(), false));
         }
         catch (...)
         {
            ComErrorPopup(err);
         }
      }
      else
         ComErrorPopup(err);
   }

   ExitInstance();

   CoUninitialize();

   _Module.Unlock();  // decrement lock count
#endif

   return NOERROR;
}


DWORD WINAPI WorkerThread(LPVOID lpParameter)
{
   // Iterate through scene data sources
   SDSGenCovThread *pThis = (SDSGenCovThread *) lpParameter;

   // Get list of scene handler types
   
   scenes::TypeEnumerator *pTypeEnum;
   SDSWrapper::GetInstance()->SelectAllTypes(&pTypeEnum);
   std::unique_ptr<scenes::TypeEnumerator> smpTypeEnum(pTypeEnum);

   // Iterate through the handler types
   bool haveType = smpTypeEnum->MoveFirst();
   while(haveType)
   {
      ISceneHandler* pHandler;
      if(CSceneHandlerFactory::GetHandler(smpTypeEnum->GetTypeIdentity(), &pHandler) == FAILURE)
      {
         ERR_report("WorkerThread(): CSceneHandlerFactory::GetHandler() failed.");
         return E_FAIL;
      }

      std::unique_ptr<ISceneHandler> smpHandler(pHandler);
      if(smpHandler->GenerateCoverage(pThis->m_vectDataSources, pThis->m_hwndProgressDlg) == FAILURE)
      {
         ERR_report("WorkerThread(): smpHandler->GenerateCoverage() failed.");
         return E_FAIL;
      }

      haveType = smpTypeEnum->MoveNext();
   }

   return NOERROR;
}

};  // namespace scene_mgr
