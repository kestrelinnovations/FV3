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

// MovingMapPropertyPageGroup.cpp
//
#include "stdafx.h"
#include "MovingMapPropertyPageGroup.h"
#include "gpsstyle.h"
#include "..\multisel.h"
#include "gps.h"
#include "factory.h"

CGPSProperties *CMovingMapRootPropertyPage::s_pProperties = NULL;

CMovingMapRootPropertyPage::CMovingMapRootPropertyPage()
{
   s_pProperties = new CGPSProperties();
   s_pProperties->initialize_from_registry("GPS Options");
}

CMovingMapRootPropertyPage::~CMovingMapRootPropertyPage()
{
   delete s_pProperties;
   s_pProperties = NULL;
}

HRESULT CMovingMapRootPropertyPage::OnApply()
{
   s_pProperties->store_in_registry("GPS Options");

   // If only one gps trail is open then the changes will be applied to the open trail.  
   // If more than one gps trail is open then the user will be presented with a dialog 
   // listing the names of all open trails
   MultiSelectOverlay dlg(NULL, FVWID_Overlay_MovingMapTrail);
   dlg.set_title("Select GPS Trail(s)");
   dlg.set_caption_txt("GPS Trails:");
   dlg.DoMultiSelect();

   C_gps_trail *pTrail = dynamic_cast<C_gps_trail *>(dlg.GetNextSelectedOverlay());

   while (pTrail) 
   {
      pTrail->set_properties(s_pProperties);
      pTrail->set_modified(TRUE);
      pTrail->ComputePredictivePath();

      pTrail = dynamic_cast<C_gps_trail *>(dlg.GetNextSelectedOverlay());
   }

   return S_OK;
}
