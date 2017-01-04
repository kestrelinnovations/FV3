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



// GPSTrailOverlayUtils.h

#pragma once

#include "gps.h"


namespace GPSTrailOverlayUtils
{
   void Add3DTrailPoint( FalconViewOverlayLib::IDisplayElements* pIDisplayElements,
      GPSPointIconPtr spPoint );

   void UpdateGPS3DPoint( FalconViewOverlayLib::IDisplayElements* pIDisplayElements,
      GPS3DPointPtr spPoint, int iUpdateSequence );

   void RemoveGPS3DPoint( FalconViewOverlayLib::IDisplayElements* pIDisplayElements,
      long point_handle, C_gps_trail* pGPSTrail );

}  // namespace GPSTrailOverlayUtils


class AddGPSTrailWorkItem :
   public CComObjectRootEx< CComMultiThreadModel >,
   public IDispatchImpl< FalconViewOverlayLib::IWorkItem,
      &FalconViewOverlayLib::IID_IWorkItem,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib >
{
public:
   ~AddGPSTrailWorkItem()
   {
   }
   void Initialize( GPSPointIconPtr spPoint )
   {
      m_spPoint = spPoint;
   }

BEGIN_COM_MAP( AddGPSTrailWorkItem )
   COM_INTERFACE_ENTRY( IDispatch )
   COM_INTERFACE_ENTRY( FalconViewOverlayLib::IWorkItem )
END_COM_MAP()

   STDMETHOD(raw_Handle)(
      FalconViewOverlayLib::IDisplayElements* pIDisplayElements )
   {
      GPSTrailOverlayUtils::Add3DTrailPoint( pIDisplayElements, m_spPoint );
      return S_OK;
   }

private:
   GPSPointIconPtr m_spPoint;
}; // class AddGPSTrailWorkItem


class UpdateGPS3DPointWorkItem :
   public CComObjectRootEx< CComMultiThreadModel >,
   public IDispatchImpl< FalconViewOverlayLib::IWorkItem,
      &FalconViewOverlayLib::IID_IWorkItem,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib >
{
public:
   ~UpdateGPS3DPointWorkItem()
   {
   }
   void Initialize( GPS3DPointPtr spPoint, int iUpdateSequence )
   {
      m_spPoint = spPoint;
      m_iUpdateSequence = iUpdateSequence;
   }

BEGIN_COM_MAP( UpdateGPS3DPointWorkItem )
   COM_INTERFACE_ENTRY( IDispatch )
   COM_INTERFACE_ENTRY( FalconViewOverlayLib::IWorkItem )
END_COM_MAP()

   STDMETHOD(raw_Handle)(
      FalconViewOverlayLib::IDisplayElements* pIDisplayElements )
   {
      GPSTrailOverlayUtils::UpdateGPS3DPoint( pIDisplayElements, m_spPoint, m_iUpdateSequence );
      return S_OK;
   }

private:
   GPS3DPointPtr m_spPoint;
   int m_iUpdateSequence;
}; // class UpdateGPSTrailWorkItem


class RemoveGPS3DPointWorkItem :
   public CComObjectRootEx< CComMultiThreadModel >,
   public IDispatchImpl< FalconViewOverlayLib::IWorkItem,
      &FalconViewOverlayLib::IID_IWorkItem,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib >
{
public:
   ~RemoveGPS3DPointWorkItem()
   {
   }
   void Initialize( long point_handle, C_gps_trail* pGPSTrail )
   {
      m_point_handle = point_handle;
      m_pGPSTrail = pGPSTrail;
   }

BEGIN_COM_MAP( RemoveGPS3DPointWorkItem )
   COM_INTERFACE_ENTRY( IDispatch )
   COM_INTERFACE_ENTRY( FalconViewOverlayLib::IWorkItem )
END_COM_MAP()

   STDMETHOD(raw_Handle)(
      FalconViewOverlayLib::IDisplayElements* pIDisplayElements )
   {
      GPSTrailOverlayUtils::RemoveGPS3DPoint( pIDisplayElements, m_point_handle, m_pGPSTrail );
      return S_OK;
   }
 
private:
   long           m_point_handle;
   C_gps_trail*   m_pGPSTrail;
};

// End of GPSTrailOverlayUtils.h
