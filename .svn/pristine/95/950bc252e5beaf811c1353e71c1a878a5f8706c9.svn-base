// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

#include "stdafx.h"
#include "ClientToolbar.h"
#include "CCallback.h"
#include "Common\ComErrorObject.h"
#include "getobjpr.h"
#include "mainfrm.h"
#include "err.h"

int ClientToolbar::Initialize(const char* bitmap_filename, 
   int num_buttons_including_separators, IDispatch* pdisp, 
   CString toolbar_name, long* separator_lst, int num_separators,
   int toolbar_id)
{
   try
   {
      CO_CREATE(m_toolbar, CLSID_FvToolbar);

      CMainFrame* frame = fvw_get_frame();

      static int tb_resource_id = 0;
      m_toolbar->Initialize(
            reinterpret_cast<long>(frame->GetSafeHwnd()), 
            ID_CLIENT_TB1 + tb_resource_id++, frame->GetFvFrameWnd());

      IFvToolbarSimClientPtr( m_toolbar )->EditorTypeName = (LPCTSTR) toolbar_name;
      C_ovl_mgr& om = *OVL_get_overlay_manager();
      om.PutCurrentFvToolbar( m_toolbar );
      om.PutCurrentEditorTypeName( toolbar_name );

      RegisterNotifyEvents(true);

      // load the bitmap
      Gdiplus::Bitmap* b = new Gdiplus::Bitmap(_bstr_t(bitmap_filename));
      if (b->GetLastStatus() != Gdiplus::Ok)
      {
         CString msg;
         msg.Format("Unable to load: %s", bitmap_filename);
         ERR_report(msg);
         delete b;
         return FAILURE;
      }

      // create a Graphics that is associated with an image
      //
      Gdiplus::Bitmap bitmapSmall(16, 16);
      Gdiplus::Graphics gSmall(&bitmapSmall);
      Gdiplus::RectF destRectSmall(0, 0, 16.0f, 16.0f);

      // add a button for each button on the given toolbar
      int separator_indx = 0;
      float currentX = 0.0f;
      for (int i=0; i<num_buttons_including_separators; ++i)
      {
         if (separator_indx < num_separators && 
               separator_lst[separator_indx] == i)
         {
            m_toolbar->AppendSeparator();
            separator_indx++;
         }
         else
         {
            gSmall.DrawImage(b, destRectSmall, currentX, 0.0f, 16.0f, 15.0f, 
               Gdiplus::UnitPixel);

            HICON hicon;
            bitmapSmall.GetHICON(&hicon);

            long icon_handle = m_toolbar->AppendButtonFromIconHandle(
               reinterpret_cast<long>(hicon), L"", L"");

            // store first button ID so we can convert button index to
            // button ID
            if (i == 0)
               m_first_button_id = icon_handle;

            currentX += 16.0f;
         }
      }
      delete b;

      frame->RecalcLayout();

      m_toolbar_id = toolbar_id;
      m_dispatch = pdisp;
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
      return FAILURE;
   }

   return SUCCESS;
}

void ClientToolbar::SetupButton(int button_num, const char* tooltip, 
   const char* status)
{
   try
   {
      m_toolbar->SetButtonText(m_first_button_id + button_num,
         _bstr_t(tooltip), _bstr_t(status));
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
}

void ClientToolbar::SetButtonState(int button_num, short is_button_down)
{
   try
   {
      m_toolbar->SetButtonPushed(m_first_button_id + button_num, 
         is_button_down);
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
}

STDMETHODIMP ClientToolbar::raw_OnButtonPressed(long lButtonId)
{
   if (m_dispatch == NULL)
   {
      ERR_report("Client's callback dispatch pointer is not set");
      return S_OK;
   }

   try
   {
      fvw::ICallbackPtr callback = m_dispatch;
      if (callback != NULL)
      {
         callback->OnToolbarButtonPressed(m_toolbar_id, 
            lButtonId - m_first_button_id);
         return S_OK;
      }

      CCallback callbackImpl(m_dispatch);
      callbackImpl.OnToolbarButtonPressed(m_toolbar_id, 
         lButtonId - m_first_button_id);

      return S_OK;
   }
   catch (_com_error &e)
   {
      CString msg;
      msg.Format("Error OnToolbarButtonPressed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnToolbarButtonPressed failed", *e);
      e->Delete();
   }

   return E_FAIL;
}

void ClientToolbar::RegisterNotifyEvents(bool bRegister)
{
   if (bRegister)
   {
      THROW_IF_ERROR(AtlAdvise( m_toolbar, IUnknownPtr(this), IID_IFvToolbarEvents, &m_dwNotifyCookie) )
   }

   else if (m_dwNotifyCookie)
   {
      THROW_IF_ERROR(AtlUnadvise( m_toolbar, IID_IFvToolbarEvents, m_dwNotifyCookie ) )
   }
}