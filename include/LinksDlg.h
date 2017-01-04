// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
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



#if !defined(AFX_LINKSEDITDLG_H__4C3BF5B4_376B_11D2_9568_00104B242B5A__INCLUDED_)
#define AFX_LINKSEDITDLG_H__4C3BF5B4_376B_11D2_9568_00104B242B5A__INCLUDED_

#include "..\resource.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LinksDlg.h : header file
//

typedef void (*save_links_function)(CString, CStringArray &);
/////////////////////////////////////////////////////////////////////////////
// CLinksEditDlg dialog

class CLinksEditDialogCallbackImpl;

class CLinksEditDlg
{
   IFvLinksEditDialogPtr m_spLinksEditDialog;
   CWnd *m_pParentWnd;
   CComObject<CLinksEditDialogCallbackImpl> *m_pLinksEditDialogCallback;

// Construction
public:
   CLinksEditDlg(save_links_function save_function, CWnd* pParent = NULL);
   ~CLinksEditDlg();

   BOOL Create();
   HWND GetHwnd();

   void set_focus(CString handle, CStringArray *item_links);

   static int view_link(CString link);
};

class CLinksEditDialogCallbackImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<IFvLinksEditDialogCallback, &IID_IFvLinksEditDialogCallback, &LIBID_FvCommonDialogsLib>
{
   save_links_function m_save_function;   // call-back function pointer

public:
   CLinksEditDialogCallbackImpl() { }
   void Initialize(save_links_function save_function)
   {
      m_save_function = save_function;
   }

BEGIN_COM_MAP(CLinksEditDialogCallbackImpl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IFvLinksEditDialogCallback)
END_COM_MAP()

   // IFvLinksEditDialogCallback
   STDMETHOD(raw_SaveItemLinks)(BSTR linksHandle, SAFEARRAY* saItemLinks);
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKSEDITDLG_H__4C3BF5B4_376B_11D2_9568_00104B242B5A__INCLUDED_)
