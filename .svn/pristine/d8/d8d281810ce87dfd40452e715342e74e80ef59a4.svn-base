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

// DisplayElementProviderCOM.cpp

#include "stdafx.h"
#include "DisplayElementProviderCOM.h"

#include "Common/ComErrorObject.h"

#include "FalconView/include/err.h"
#include "FalconView/overlay/OverlayCOM.h"
#include "FalconView/UIThreadOperation.h"

HRESULT DisplayElementProviderCOM::Initialize(C_overlay* overlay,
   FalconViewOverlayLib::IDisplayElements* display_elements,
   FalconViewOverlayLib::IWorkItems* work_items)
{
   try
   {
      CO_CREATE(m_provider, m_clsid);

      // marshal associated IFvOverlay interface
      FalconViewOverlayLib::IFvOverlayPtr marshaled_overlay;
      {
         // Prepare to marshal in the UI thread where the STA COM object was
         // created
         IStream* marshaled_stream;
         const bool wait_for_ui_thread_operation = true;
         new fvw::UIThreadOperation([&]()
         {
            marshaled_stream = overlay->PrepareMarshaledStream();
         }, wait_for_ui_thread_operation);

         // Unmarshal to this thread
         if (::CoGetInterfaceAndReleaseStream(marshaled_stream,
            FalconViewOverlayLib::IID_IFvOverlay,
            reinterpret_cast<void **>(&marshaled_overlay)) != S_OK)
         {
            ERR_report("Failed unmarshaling overlay to worker thread.");
            return E_FAIL;
         }
      }

      return m_provider->Initialize(marshaled_overlay, display_elements,
         work_items);
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
      return E_FAIL;
   }
}