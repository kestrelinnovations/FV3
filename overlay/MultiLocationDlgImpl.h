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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// FvOverlayDataObjectImpl.h
//

#pragma once

class CMultiLocationEditDlg;

// Implements IFvTabularEditorParser defined in FalconViewOverlay.tlb
//
class CMultiLocationDlgImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewTabularEditorLib::IMultiLocationDlg, 
      &FalconViewTabularEditorLib::IID_IMultiLocationDlg, &FalconViewTabularEditorLib::LIBID_FalconViewTabularEditorLib>
{
public:
   CMultiLocationDlgImpl();
   ~CMultiLocationDlgImpl();

   BEGIN_COM_MAP(CMultiLocationDlgImpl)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(FalconViewTabularEditorLib::IMultiLocationDlg)
   END_COM_MAP()

   // IMultiLocationDlg
public:
   STDMETHOD(raw_DoModal)(long parent_hwnd, SAFEARRAY* geo_locations, int* res);
   STDMETHOD(raw_GetLocations)(SAFEARRAY** geo_locations);

private:
   void DestroyDialog();

   CMultiLocationEditDlg* m_dlg;
};
