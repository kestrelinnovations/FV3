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

#pragma once

// Base class for context menu items created in FalconView
class CContextMenuItem : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvContextMenuItem, &FalconViewOverlayLib::IID_IFvContextMenuItem, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
public:
   BEGIN_COM_MAP(CFVMenuNodeContextMenuItem)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvContextMenuItem)
   END_COM_MAP()

   // IFvContextMenuItem
public:
   // By default, a menu item is enabled
   STDMETHOD(get_MenuItemEnabled)(long *pMenuItemEnabled) {
      *pMenuItemEnabled = TRUE;
      return S_OK;
   }
   // By default, a menu item is not checked
   STDMETHOD(get_MenuItemChecked)(long *pMenuItemChecked) {
      *pMenuItemChecked = FALSE;
      return S_OK;
   }

   STDMETHOD(get_MenuItemName)(BSTR *pMenuItemName) = 0;
   STDMETHOD(raw_MenuItemSelected)() = 0;
};

// CCenterMapContextMenuItem
class CCenterMapContextMenuItem : public CContextMenuItem
{
public:
   STDMETHOD(get_MenuItemName)(BSTR *pMenuItemName);
   STDMETHOD(raw_MenuItemSelected)();
};

class ScaleInOutContextMenuItemBase : public CContextMenuItem
{
public:
   void Initialize(const d_geo_t& cursor_pos_geo, bool cursor_pos_valid);

protected:
   d_geo_t m_cursor_position_geo;
   bool m_cursor_position_valid;
};

// CScaleInContextMenuItem
class CScaleInContextMenuItem : public ScaleInOutContextMenuItemBase
{
public:
   STDMETHOD(get_MenuItemName)(BSTR *pMenuItemName);
   STDMETHOD(raw_MenuItemSelected)();
};

// CScaleOutContextMenuItem
class CScaleOutContextMenuItem : public ScaleInOutContextMenuItemBase
{
public:
   STDMETHOD(get_MenuItemName)(BSTR *pMenuItemName);
   STDMETHOD(raw_MenuItemSelected)();
};

// COverlayOptionsContextMenuItem
class COverlayOptionsContextMenuItem : public CContextMenuItem
{
public:
   STDMETHOD(get_MenuItemName)(BSTR *pMenuItemName);
   STDMETHOD(raw_MenuItemSelected)();
};

// CNorthUpContextMenuItem
class CNorthUpContextMenuItem : public CContextMenuItem
{
public:
   STDMETHOD(get_MenuItemName)(BSTR *pMenuItemName);
   STDMETHOD(raw_MenuItemSelected)();
};

// CGetMapInfoContextMenuItem
class CGetMapInfoContextMenuItem : public CContextMenuItem
{
public:
   STDMETHOD(get_MenuItemName)(BSTR *pMenuItemName);
   STDMETHOD(raw_MenuItemSelected)();
};