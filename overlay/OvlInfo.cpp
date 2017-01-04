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



// ovlinfo.cpp

#include "stdafx.h"
#include "overlay.h"
#include "OvlFctry.h"
#include "..\OvlToolConfiguration.h"
#include "..\factory.h"                 // for CLayerOvlFactory
#include "ovl_mgr.h"

// Get the associated CIconImage class structure for a given overlay
CIconImage* C_ovl_mgr::get_icon_image(C_overlay *pOverlay)
{
   CIconImage* pIconImage = NULL;

   // if the overlay implements the IFvOverlayFactoryOverrides interface, then get the per-instance icon
   OverlayTypeOverrides_Interface* pFvOverlayTypeOverrides = dynamic_cast<OverlayTypeOverrides_Interface *>(pOverlay);
   if (pFvOverlayTypeOverrides != NULL && pFvOverlayTypeOverrides->SupportsFvOverlayTypeOverrides())
   {
      // if we successfully retrieve the icon's file name from the overlay and the file name is non-empty
      _bstr_t bstrIconName;
      if (pFvOverlayTypeOverrides->get_m_iconFilename(bstrIconName.GetAddress()) == S_OK && bstrIconName.length() > 0)
      {
         // then load icon with the given file name
         pIconImage = CIconImage::load_images((char *)bstrIconName);
      }
   }

   // otherwise, in most cases, we will get the icon image from the overlay type descriptor
   if (pIconImage == NULL)
      pIconImage = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(pOverlay->get_m_overlayDescGuid())->pIconImage;

   return pIconImage;
}
