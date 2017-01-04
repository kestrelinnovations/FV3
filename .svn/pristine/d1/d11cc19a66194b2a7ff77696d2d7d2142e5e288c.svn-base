// Copyright (c) 2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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

// GPSTrailOverlayUtils.cpp : implementation file
//

#include "stdafx.h"
#include "gps.h"
#include "GPSTrailOverlayUtils.h"


void GPSTrailOverlayUtils::Add3DTrailPoint(
   FalconViewOverlayLib::IDisplayElements* pIDisplayElements,
   GPSPointIconPtr spPoint )
{
   GPSPointIcon* pPoint = *spPoint;
   if ( pPoint != nullptr )   // Point has not been deleted
   {
      if ( pPoint->m_3DHandle != 0 )
         pIDisplayElements->raw_DeleteObject( pPoint->m_3DHandle );

      C_gps_trail* pGPSTrail = pPoint->get_overlay();
      CGPSProperties* pProp = pGPSTrail->get_properties();

      long image_handle = pIDisplayElements->CreateImageFromFilename(
         pProp->get_3D_icon_file_path() + pProp->get_trail_3D_icon_file() );

      FalconViewOverlayLib::TimeInterval ti;
      ti.dtBegin = (double) pPoint->get_date_time() + ( 0.1 / (24.0 * 3600.0 ) );   // Hedge by 0.1 sec to hide under ship
      ti.dtEnd = 1e6;      // Never expires
      pIDisplayElements->SetTimeInterval( ti );

      pIDisplayElements->SetIconDeclutterMode( TRUE );
      pIDisplayElements->SetIconAlignmentMode( FalconViewOverlayLib::ALIGN_CENTER_CENTER );

      pPoint->m_3DHandle = pIDisplayElements->AddImage( image_handle,
         pPoint->get_latitude(), pPoint->get_longitude(), pPoint->m_msl,
         1.0, pPoint->m_true_heading, _bstr_t(L"") );
   }
}  // Add3DPoint()


void GPSTrailOverlayUtils::UpdateGPS3DPoint(
   FalconViewOverlayLib::IDisplayElements* pIDisplayElements,
   GPS3DPointPtr spPoint, int iUpdateSequence )
{
   GPS3DPoint* pPoint = *spPoint;
   if ( pPoint != nullptr )
   {
      ATL::CCritSecLock lock( C_gps_trail::get_critical_section() );

      if ( iUpdateSequence != pPoint->m_iUpdateSequence )
         return;     // Obsolete update

      pIDisplayElements->SetAltitudeMode( FalconViewOverlayLib::ALTITUDE_MODE_ABSOLUTE );
      pIDisplayElements->SetIconDeclutterMode( pPoint->m_ePointType == GPS3DPoint::GPSPT_TRAIL );
      pIDisplayElements->SetIconAlignmentMode( FalconViewOverlayLib::ALIGN_CENTER_CENTER );

      double dAltMeters = pPoint->m_dAltMeters
         + ( pPoint->m_ePointType == GPS3DPoint::GPSPT_SHIP ? 1.0 : 0.0 ); // Attempt to keep on top

      // If already drawn update the position
      if ( pPoint->m_3DHandle != 0 )
      {
         pIDisplayElements->ModifyPosition( pPoint->m_3DHandle,
            pPoint->m_gptLatLon.lat, pPoint->m_gptLatLon.lon, dAltMeters );
      }
      else  // New point
      {
         double dRotation = 0.0; // Assume not god's eye view
         long image_handle;
         if ( pPoint->m_ePointType == GPS3DPoint::GPSPT_SHIP )
         {
            std::vector< BYTE >& aRawBytes = pPoint->m_pGPSTrail->GetShipSymbolRawBytes(); 
            image_handle = pIDisplayElements->CreateImageFromRawBytes( aRawBytes.data(), aRawBytes.size() );
            if ( !pPoint->m_pGPSTrail->get_update_3d_camera_enabled() )
               dRotation = pPoint->m_dHeadingDeg;  // God's eye view
         }
         else
         {
            CGPSProperties* pGPSProp = pPoint->m_pGPSTrail->get_properties();
            _bstr_t bstrIconFile = pGPSProp->get_3D_icon_file_path();

            CPredictivePathProperties* pPredProp = pGPSProp->GetPredictivePathProperties();
            switch ( pPoint->m_ePointType )
            {
               case GPS3DPoint::GPSPT_NORM_PATH:
                  bstrIconFile += pPredProp->get_normal_path_3D_icon_file();
                  break;
               case GPS3DPoint::GPSPT_WARN_PATH:
                  bstrIconFile += pPredProp->get_warning_path_3D_icon_file();
                  break;
               case GPS3DPoint::GPSPT_CRIT_PATH:
                  bstrIconFile += pPredProp->get_critical_path_3D_icon_file();
                  break;
               default:
                  return;
            }  // switch ( ePointType )

            image_handle = pIDisplayElements->CreateImageFromFilename( bstrIconFile );
         }

         pPoint->m_3DHandle = pIDisplayElements->AddImage( image_handle,
            pPoint->m_gptLatLon.lat, pPoint->m_gptLatLon.lon,
            dAltMeters, 1.0, dRotation, _bstr_t(L"") );

      }  // New point

   }  // pPoint != nullptr
}  // UpdateGPS3DPoint


void GPSTrailOverlayUtils::RemoveGPS3DPoint(
   FalconViewOverlayLib::IDisplayElements* pIDisplayElements,
   long point_handle, C_gps_trail* pGPSTrail )
{
   ATLASSERT( point_handle > 0 && "point_handle expected to be >0" );
   if ( point_handle > 0 )
   {
      ATL::CCritSecLock lock( C_gps_trail::get_critical_section() );

      pIDisplayElements->DeleteObject( point_handle );

   }  // point_handle != 0
}  // RemoveGPS3DPoint

// End of GPSTrailOverlayUtils.cpp
