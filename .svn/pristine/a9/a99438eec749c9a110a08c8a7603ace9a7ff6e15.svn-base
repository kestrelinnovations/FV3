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


// SDSGenCovThread.h : header file
//


#if !defined(AFX_SDSGENCOVTHREAD_H__04A26B5B_60C5_495D_B682_5EFB4BFCA1BC__INCLUDED_)
#define AFX_SDSGENCOVTHREAD_H__04A26B5B_60C5_495D_B682_5EFB4BFCA1BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
#include <vector>


namespace scene_mgr
{

// Forward declarations
class SDSGenCovStatusCallback;


/////////////////////////////////////////////////////////////////////////////
// GenCovThread thread

class SDSGenCovThread : public CWinThread
{
   DECLARE_DYNCREATE(SDSGenCovThread)
protected:
   SDSGenCovThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SDSGenCovThread)
   public:
   virtual BOOL InitInstance();
   virtual int ExitInstance();
   virtual int Run();
   //}}AFX_VIRTUAL

   std::vector<long>        m_vectDataSources;
   HWND                     m_hwndParent;
   HWND                     m_hwndProgressDlg;
   HANDLE                   m_hWorkerThread;


// Implementation
protected:
   virtual ~SDSGenCovThread();
   void GenCov(SDSGenCovStatusCallback* pStatusCallback);

   // Generated message map functions
   //{{AFX_MSG(GenCovThread)
      // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

};  // namespace scene_mgr

#endif // !defined(AFX_SDSGENCOVTHREAD_H__04A26B5B_60C5_495D_B682_5EFB4BFCA1BC__INCLUDED_)
