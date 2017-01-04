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

// DisplayElementProviderCOM.h

#ifndef FALCONVIEW_DISPLAYELEMENTPROVIDERCOM_H_
#define FALCONVIEW_DISPLAYELEMENTPROVIDERCOM_H_

#include "./DisplayElementProvider_Interface.h"

class DisplayElementProviderCOM :
   public DisplayElementProvider_Interface
{
public:
   DisplayElementProviderCOM(CLSID clsid) : m_clsid(clsid)
   {
   }

   virtual HRESULT Initialize(
      C_overlay* overlay,
      FalconViewOverlayLib::IDisplayElements* display_elements,
      FalconViewOverlayLib::IWorkItems* work_items);
   virtual ~DisplayElementProviderCOM()
   {
   }

private:
   FalconViewOverlayLib::IDisplayElementProviderPtr m_provider;
   CLSID m_clsid;
};

#endif  // FALCONVIEW_DISPLAYELEMENTPROVIDERCOM_H_