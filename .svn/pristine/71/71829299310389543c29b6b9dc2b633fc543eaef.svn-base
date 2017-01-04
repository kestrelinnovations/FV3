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



// showrmk.cpp  --  display a remark dialog

#include "stdafx.h"
#include "showrmk.h"
#include "Common\ComErrorObject.h"
#include "err.h"
#include "ovl_mgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

class ATL_NO_VTABLE CInfoDialogCallbackImpl : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<IFvInformationDialogCallback, &__uuidof(IFvInformationDialogCallback), &LIBID_FvCommonDialogsLib, /* wMajor = */ 1>
{
   C_overlay* m_focus_overlay;

public:
   CInfoDialogCallbackImpl() : m_focus_overlay(NULL)
   {
   }

BEGIN_COM_MAP(CInfoDialogCallbackImpl)
	COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(IFvInformationDialogCallback)
END_COM_MAP()

   STDMETHOD(raw_InfoDialogOwnerChanged)()
   {
      // be sure to inform the overlay it is loosing focus
      if (OVL_get_overlay_manager()->is_overlay_valid(m_focus_overlay))
         m_focus_overlay->release_focus();

      m_focus_overlay = NULL;

      return S_OK;
   }

   void set_focus_overlay(C_overlay* overlay)
   {
      C_overlay* old_overlay;

      old_overlay = m_focus_overlay;
      if ((old_overlay != NULL) && (old_overlay != overlay))
      {
         if (OVL_get_overlay_manager()->is_overlay_valid(old_overlay))
            old_overlay->release_focus();
      }

      m_focus_overlay = overlay;
   }

   C_overlay* get_focus_overlay()
   {
      return m_focus_overlay;
   }
};

static CComObject<CInfoDialogCallbackImpl>* s_pInfoDialogCallback;

// The text passed in to this function can be a preformatted Rich Text string.
// If this is the case, set format to MAP_INFO_RICH_TEXT
void CRemarkDisplay::display_dlg(CWnd* wnd, const char* text, const char *title, C_overlay* pFocusOverlay,
                                 MapInfoFormatEnum format /*= MAP_INFO_PLAIN_TEXT*/)
{
   try
   {
      IFvInformationDialogPtr spInformationDialog;
      CO_CREATE(spInformationDialog, CLSID_FvInformationDialog);

      TextFormatEnum eTextFormat;
      switch(format)
      {
      case MAP_INFO_PLAIN_TEXT: eTextFormat = TEXT_FORMAT_PLAIN_TEXT; break;
      case MAP_INFO_RICH_TEXT: eTextFormat = TEXT_FORMAT_RICH_TEXT; break;
      case MAP_INFO_HTML: eTextFormat = TEXT_FORMAT_HTML; break;
      }

      CString iconFilename;
      if (pFocusOverlay != NULL)
      {
         CIconImage *pIcon = OVL_get_overlay_manager()->get_icon_image(pFocusOverlay);
         iconFilename = pIcon->get_item_full_pathname();
      }

      if (s_pInfoDialogCallback == NULL)
      {
         CComObject<CInfoDialogCallbackImpl>::CreateInstance(&s_pInfoDialogCallback);
         s_pInfoDialogCallback->AddRef();
      }

      s_pInfoDialogCallback->set_focus_overlay(pFocusOverlay);

      spInformationDialog->ShowDialog(reinterpret_cast<long>(wnd->GetSafeHwnd()), _bstr_t(iconFilename), _bstr_t(title),
         _bstr_t(text), eTextFormat, s_pInfoDialogCallback);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

void CRemarkDisplay::DestroyCallbackInstance()
{
   if (s_pInfoDialogCallback != NULL)
      s_pInfoDialogCallback->Release();
}

BOOL CRemarkDisplay::test_active() // for static call testing
{
   try
   {
      IFvInformationDialogPtr spInformationDialog;
      CO_CREATE(spInformationDialog, CLSID_FvInformationDialog);

      return spInformationDialog->IsDialogActive;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   return FALSE;
}

void CRemarkDisplay::close_dlg()
{
   try
   {
      IFvInformationDialogPtr spInformationDialog;
      CO_CREATE(spInformationDialog, CLSID_FvInformationDialog);

      spInformationDialog->CloseDialog();
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}
	
C_overlay* CRemarkDisplay::get_focus_overlay()
{
	if (test_active() && s_pInfoDialogCallback != NULL)
		return s_pInfoDialogCallback->get_focus_overlay();
	else
		return NULL;
}
