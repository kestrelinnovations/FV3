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


// SDSCopyAndDeleteScenesThread.h : Declaration of CSDSCopyAndDeleteScenesThread


#ifndef __SCENEMANAGER_SDSCOPYANDDELETESCENESTHREAD_H_
#define __SCENEMANAGER_SDSCOPYANDDELETESCENESTHREAD_H_


#include "SMOperations.h"


namespace scene_mgr
{


/////////////////////////////////////////////////////////////////////////////
// CSDSCopyAndDeleteScenesThread


class CSDSCopyAndDeleteScenesThread : public CWinThread
{
   DECLARE_DYNCREATE(CSDSCopyAndDeleteScenesThread)
protected:
   CSDSCopyAndDeleteScenesThread();  // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSDSCopyAndDeleteScenesThread)
public:
   virtual BOOL InitInstance();
   virtual int ExitInstance();
   virtual int Run();
   //}}AFX_VIRTUAL

   void SetParentWindow(HWND hwnd_parent);
   void Add(const SMCopyOperation& new_op);
   void Add(const SMDeleteOperation& new_op);

// Implementation
protected:
   virtual ~CSDSCopyAndDeleteScenesThread();

   HWND m_hwnd_parent;
   std::vector<SMCopyOperation> m_copy_queue;
   std::vector<SMDeleteOperation> m_delete_queue;

   // Generated message map functions
   //{{AFX_MSG(CSDSCopyAndDeleteScenesThread)
      // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

};  // namespace scene_mgr

#endif  // __SCENEMANAGER_SDSCOPYANDDELETESCENESTHREAD_H_