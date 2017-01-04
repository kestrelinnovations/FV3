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



// LinksDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fvwutil.h"
#include "LinksDlg.h"
#include "..\..\Common\ComErrorObject.h"
#include "..\..\Common\SafeArray.h"
#include "err.h"

/////////////////////////////////////////////////////////////////////////////
// CLinksEditDlg dialog

CLinksEditDlg::CLinksEditDlg(save_links_function save_function, CWnd* pParent /*=NULL*/)
{
   //{{AFX_DATA_INIT(CLinksEditDlg)
   //}}AFX_DATA_INIT
   m_pParentWnd = pParent;

   CComObject<CLinksEditDialogCallbackImpl>::CreateInstance(&m_pLinksEditDialogCallback);
   m_pLinksEditDialogCallback->AddRef();
   m_pLinksEditDialogCallback->Initialize(save_function);
}

CLinksEditDlg::~CLinksEditDlg()
{
   if (m_pLinksEditDialogCallback != NULL)
      m_pLinksEditDialogCallback->Release();
}

BOOL CLinksEditDlg::Create()
{
   try
   {
      if (m_spLinksEditDialog == NULL)
         CO_CREATE(m_spLinksEditDialog, CLSID_FvLinksEditDialog);

      HWND hwndParent = NULL;
      if (m_pParentWnd != NULL)
         hwndParent = m_pParentWnd->GetSafeHwnd();

      m_spLinksEditDialog->ShowDialog(reinterpret_cast<long>(hwndParent), m_pLinksEditDialogCallback);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("CLinksEditDlg::Create failed - %s", (char *)e.Description());
      ERR_report(msg);
      return FALSE;
   }

   return TRUE;
}

HWND CLinksEditDlg::GetHwnd()
{
   try
   {
      if (m_spLinksEditDialog != NULL)
         return reinterpret_cast<HWND>(m_spLinksEditDialog->hWnd);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("CLinksEditDlg::GetHwnd failed - %s", (char *)e.Description());
      ERR_report(msg);
   }

   return NULL;
}

/* static */
int CLinksEditDlg::view_link(CString link)
{
   CFvwUtil *util = CFvwUtil::get_instance();
   int err = 0;

   util->shell_execute((LPSTR)(LPCSTR)link, &err);

   // CFvwUtil::shell_execute() calls ::ShellExecute(), which returns
   // a value of 32 or less to indicate an error
   if (err <= 32)
      return FAILURE;
   else
      return SUCCESS;
}

void CLinksEditDlg::set_focus(CString handle, CStringArray *item_links)
{
   try
   {
      if (m_spLinksEditDialog != NULL && m_spLinksEditDialog->hWnd != NULL)
      {
         if (item_links == NULL)
            m_spLinksEditDialog->ClearItemLinks();
         else
         {
            // convert from CStringArray item links to SAFEARRAY of BSTRs
            BstrSafeArray bsaItemLinks;
            for (int i=0; i<item_links->GetCount(); ++i)
               bsaItemLinks.Append( _bstr_t(item_links->GetAt(i)).Detach() );

            m_spLinksEditDialog->SetItemLinks(_bstr_t(handle), &bsaItemLinks);
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("CLinksEditDlg::set_focus failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
}

STDMETHODIMP CLinksEditDialogCallbackImpl::raw_SaveItemLinks(BSTR linksHandle, SAFEARRAY* saItemLinks)
{
   // convert from SAFEARRAY of BSTRs to CStringArray
   CStringArray itemLinks;
   BstrSafeArray bsaItemLinks(saItemLinks);
   const int nNumElements = bsaItemLinks.GetNumElements();
   for (int i=0; i<nNumElements; ++i)
      itemLinks.Add( _bstr_t(bsaItemLinks[i]) );

   m_save_function((char *)_bstr_t(linksHandle), itemLinks);

   return S_OK;
}