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

// ClientToolbar - handler for toolbars added via ILayer::AddToolbar


class ATL_NO_VTABLE ClientToolbar :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<IFvToolbarEvents, &IID_IFvToolbarEvents, &LIBID_FvToolbarServerLib>
{
public:
   ClientToolbar() { }
   ~ClientToolbar()
   {
      C_ovl_mgr& om = *OVL_get_overlay_manager();
      if ( m_toolbar == om.GetCurrentFvToolbar() )
         om.PutCurrentFvToolbar( NULL );
   }
   int Initialize(const char* bitmap_filename, 
      int num_buttons_including_separators, IDispatch* pdisp, 
      CString toolbar_name, long* separator_lst, int num_separators,
      int toolbar_id);
   void SetupButton(int button_num, const char* tooltip, const char* status);
   void SetButtonState(int button_num, short is_button_down);

      // IFvToolbarEvents
public:
   STDMETHOD(raw_OnButtonPressed)(long lButtonId);
   STDMETHOD(raw_OnHelp)(long lButtonId) 
   { 
      return S_OK; 
   }
   STDMETHOD(raw_OnClosed)() 
   { 
      return S_OK; 
   }
   STDMETHOD(raw_OnModified)()
   {
      return S_OK;
   }

BEGIN_COM_MAP(ClientToolbar)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IFvToolbarEvents)
END_COM_MAP()

   void RegisterNotifyEvents(bool bRegister);

private:
   IDispatchPtr m_dispatch;
   IFvToolbarPtr m_toolbar;
   DWORD m_dwNotifyCookie;
   long m_toolbar_id;
   long m_first_button_id;
};