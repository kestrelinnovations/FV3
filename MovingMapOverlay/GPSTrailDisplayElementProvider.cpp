// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

// GPSTrailDisplayElementProvider.cpp

#include "stdafx.h"
#include "GPSTrailDisplayElementProvider.h"
#include "GPSTrailOverlayUtils.h"

#include "FalconView/include/overlay.h"
#include "FalconView/include/param.h"

#include "FalconView/Include/gps.h"
#include "FalconView/MovingMapOverlay/DataObject.h"


HRESULT GPSTrailDisplayElementProvider::Initialize( C_overlay* pOverlay,
   FalconViewOverlayLib::IDisplayElements* pIDisplayElements,
   FalconViewOverlayLib::IWorkItems* pIWorkItems )
{
   pIDisplayElements->SetAltitudeMode( FalconViewOverlayLib::ALTITUDE_MODE_ABSOLUTE );
   pIDisplayElements->SetIconDeclutterMode( TRUE );
   pIDisplayElements->SetIconAlignmentMode( FalconViewOverlayLib::ALIGN_CENTER_CENTER );

   C_gps_trail* pGPSTrail = static_cast< C_gps_trail* >( pOverlay );

   // Add any existing trail points
   ATL::CCritSecLock lock( C_gps_trail::s_idThreadInterlock );

   GPSPointIconList* pIconList = pGPSTrail->get_icon_list();
   POSITION pos = pIconList->get_first_position();
   while ( pos != NULL )
   {      
      GPSPointIcon* pPoint = pIconList->get_next( pos );
      GPSTrailOverlayUtils::Add3DTrailPoint( pIDisplayElements, pPoint->m_sppThis );
   }

   lock.Unlock();

   // Setup work items for modifications to the overlay
   pGPSTrail->SetWorkItems( pIWorkItems );

   return S_OK;
}
