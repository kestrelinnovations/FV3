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

// ElevationDataTileSource.cpp
//

#include "stdafx.h"
#include "getobjpr.h"
#include "ElevationDataTileSource.h"
#include "FvCore/Include/Registry.h"
#include "ComErrorObject.h"
#include "ComErrorHandler.h"
#include "FalconView/include/err.h"

#include "osgEarth/Registry"
#include "osgDB/DatabasePager"

// viewshed usage
#include "OSGVariables.h"
#include "GeospatialViewController.h"
#include "RenderFunctions.h"

#include "TacModel.h"
extern const GUID FVWID_Overlay_TacticalModel;

namespace
{
   const int MINIMUM_ELEVATION_LOD = 8;
}

ElevationDataTileSource::ElevationDataTileSources
   ElevationDataTileSource::s_stElevationDataTileSources;
CRITICAL_SECTION ElevationDataTileSource::s_csStaticAccess;

ElevationDataTileSource::ElevationDataTileSource(
      const osgEarth::TileSourceOptions& options) :
   osgEarth::TileSource(options)
#if GOV_RELEASE
   , m_bDBDBReaderInitialized( FALSE )
#endif
{
   initialize(nullptr, nullptr);
}

void ElevationDataTileSource::initialize(const osgDB::Options* options,
   const osgEarth::Profile* profile)
{
   if ( s_stElevationDataTileSources.empty() )
      ::InitializeCriticalSection( &s_csStaticAccess );
   ATL::CCritSecLock lock( s_csStaticAccess );
   s_stElevationDataTileSources.insert( this );
   lock.Unlock();

   if (profile)
      setProfile(profile);
   else
      setProfile(osgEarth::Registry::instance()->getGlobalGeodeticProfile());

   ::InitializeCriticalSection(&m_critical_section);
}

ElevationDataTileSource::~ElevationDataTileSource()
{
   // Release elevation sources while we still have a lock available
   ATL::CCritSecLock lock1( m_critical_section );

   auto it = m_thread_id_to_elevation_reader.find( ::GetCurrentThreadId() );
   if ( it != m_thread_id_to_elevation_reader.end() )
      it->second->Terminate();   // Only call Terminate if current thread

   m_thread_id_to_elevation_reader.clear();

#if GOV_RELEASE
   m_mpDBDBElevationDataReader.clear();
   m_bDBDBReaderInitialized = FALSE;
#endif

   lock1.Unlock();
   ::DeleteCriticalSection(&m_critical_section);

   ATL::CCritSecLock lock2( s_csStaticAccess );
   s_stElevationDataTileSources.erase( this );
   if ( s_stElevationDataTileSources.empty() )
   {
      lock2.Unlock();
      ::DeleteCriticalSection( &s_csStaticAccess );
   }
}

osg::HeightField* ElevationDataTileSource::createHeightField(
   const osgEarth::TileKey& key,
   osgEarth::TileSource::HeightFieldOperation* prepOp,
   osgEarth::ProgressCallback* progress)
{
   if (key.getLOD() < MINIMUM_ELEVATION_LOD)
   {
      return nullptr;
   }

   // Get the extents of the requested tile
   //
   const osgEarth::GeoExtent& geo_extent = key.getExtent();
   osgEarth::Bounds bounds = geo_extent.bounds();

   const double southern_lat = bounds.yMin();
   const double western_lon = bounds.xMin();
   const double northern_lat = bounds.yMax();
   const double eastern_lon = bounds.xMax();

   int tile_size = getOptions().tileSize().value(); // can't be const
   if (OSGUserVars::GetElevationUpscaleNeed(key.str()))
   {
      //OSGUserVars::SetElevationUpscaleNeed(key.str(), false);
      tile_size = tile_size*OSGUserVars::GetElevationUpscale(key.str());
   }

   // now prevent cases where the tile wishes to subdivide too much
   tile_size = max(tile_size*min(pow(2.0,15.0-key.getLOD()), 1.0), 4.0);

   IDted3Ptr dted3 = GetElevationReaderForCurrentThread();
   if ( !(BOOL) dted3 )
      return nullptr;

#ifdef DTED_ELEVATION_TIMING_TEST
   LARGE_INTEGER liT1, liT2, liFreq;
   ::QueryPerformanceCounter( &liT1 );
#endif

#ifdef _DEBUG
   dted3->AddRef();
   long c = dted3->Release();
#endif

   osg::HeightField* ret = new osg::HeightField();
   ret->allocate(tile_size, tile_size);
   float* data = static_cast<float *>(const_cast<GLvoid *>(
      ret->getFloatArray()->getDataPointer()));
   size_t size = tile_size * tile_size * sizeof(float);

   // First populate the height field with DTED elevation data.
   // Use the best available (s.b. best post size)
   HRESULT hr = dted3->GetBlockDTEDMeters(northern_lat, western_lon, southern_lat,
      eastern_lon, tile_size, tile_size, -1, data);
   if ( hr != S_OK )
   {
      ATLASSERT( FALSE && "GetBlockDTEDMeters() failed" );
      CStringA msg;
      msg.Format( "GetBlockDTEDMeters( N=%.5f, W=%.5f, S=%.5f, E=%.5f ) failed, hr = %x",
         northern_lat, western_lon, southern_lat, eastern_lon, hr );
      ERR_report( msg );

      // Error occurred - zero out tile
      memset(data, 0, sizeof(float) * tile_size * tile_size);
   }
#ifdef DTED_ELEVATION_TIMING_TEST
   ::QueryPerformanceCounter( &liT2 );
   LONGLONG llTime = liT2.QuadPart - liT1.QuadPart;
   ATL::CCritSecLock cs( m_critical_section );
   static LONGLONG
      llTMax( 0 ),
      llTMin( LONGLONG_MAX ),
      llTTotal( 0 ),
      llLast10[ 10 ];
   static INT cBlockDtedCalls( 0 );
   static DWORD dwLastReportTicks( 0 );
   if ( llTime < llTMin )
      llTMin = llTime;
   if ( llTime > llTMax && cBlockDtedCalls > 0 )
      llTMax = llTime;
   llTTotal += llTime;
   llLast10[ cBlockDtedCalls % 10 ] = llTime;
   cBlockDtedCalls++;
   DWORD dwTicks = GetTickCount();
   if ( (INT)( dwTicks - dwLastReportTicks ) > 5 * 1000 )
   {
      dwLastReportTicks = dwTicks;
      ::QueryPerformanceFrequency( &liFreq );
      DOUBLE
         dRate = 1000.0 / liFreq.QuadPart,
         dTMin = dRate * llTMin,
         dTMax = dRate * llTMax,
         dTCur = dRate * llTime,
         dTAvg = ( dRate * llTTotal ) / cBlockDtedCalls;
      ATLTRACE( _T("HeightField: cur = %.3f, min = %.3f, avg = %.3f, max = %.3f, # = %d\n"),
         dTCur, dTMin, dTAvg, dTMax, cBlockDtedCalls );
      if ( cBlockDtedCalls >= 10 )
      {
         LONGLONG
            llMin = LONGLONG_MAX,
            llMax = 0,
            llTot = 0;
         for ( INT k = 0; k < 10; k++ )
         {
            llMin = __min( llMin, llLast10[ k ] );
            llMax = __max( llMax, llLast10[ k ] );
            llTot +=  llLast10[ k ];
         }
         ATLTRACE( _T("  Last 10: min = %.3f, avg = %.3f, max = %.3f\n"),
            dRate * llMin, 0.1 * dRate * llTot, dRate * llMax );
      }
   }
   cs.Unlock();
#endif

   std::auto_ptr< float > cdb_elevation_data(new float[tile_size * tile_size]);

   C_overlay* overlay = OVL_get_overlay_manager()->get_first_of_type(
      FVWID_Overlay_TacticalModel);
   for (; overlay != nullptr; overlay = OVL_get_overlay_manager()->
      get_next_of_type(overlay, FVWID_Overlay_TacticalModel))
   {
      C_model_ovl* model_overlay = static_cast<C_model_ovl*>(overlay);
      model_overlay->GetBlockElevationMeters(southern_lat, western_lon,
         northern_lat, eastern_lon, tile_size, tile_size,
         cdb_elevation_data.get());

      // Copy valid values into the tile (no flipping of the data required)
      float* crnt_cdb = cdb_elevation_data.get();
      float* crnt_data = data;
      int n = tile_size * tile_size;
      while (n--)
      {
         if (*crnt_cdb != -99999.0f)
            *crnt_data = *crnt_cdb;
         ++crnt_cdb;
         ++crnt_data;
      }
   }

   if (OSGUserVars::ContainsKey(key.str()))
   {
      // TODO: Remove viewshed specific code from the ElevationDataTileSource
      fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->
         SetCreateNewViewshed(true);
   }

#if GOV_RELEASE
   // Merge with DBDB data if relevant.  DBDB block data is returned
   // in column (lat) order, N to S, then W to E.  DTED3 data is returned
   // in row order, W to E then S to N.
   CMainFrame* frame = fvw_get_frame();
   if ( frame && frame->GetUnderseaElevationsEnable() )
   {
      IElevationDataPtr spDBDBElevationData;
      _variant_t varDBDBData;
      PINT piDBDBElevation;

      for ( INT kIn = 0, iX = 0; iX < tile_size; iX++ )
         for ( INT iY = tile_size - 1; iY >= 0; iY--, kIn++ )
         {
            INT kOut = iX + ( iY * tile_size );    // Row/col swap
            if ( data[ kOut ] <= 0.0 ) // Possible DBDB use
            {
               // Get the block of DBDB data once only
               if ( varDBDBData.vt == VT_EMPTY )
               {
                  spDBDBElevationData =
                     GetDBDBElevationReaderForCurrentThread();
                  if ( !m_bDBDBReaderInitialized )
                  {
                     // Get a single point value to initialize the DBDB database
                     _bstr_t bstrProductName, bstrMapSeries;
                     DOUBLE dMapScale;
                     MapScaleUnitsEnum eMapScaleUnits;
                     DOUBLE dElevation;

                     if ( S_OK !=
                           spDBDBElevationData->raw_GetBestPointElevation(
                              southern_lat, western_lon, ELEV_UNITS_METERS,
                              bstrProductName.GetAddress(), &dMapScale,
                              &eMapScaleUnits, bstrMapSeries.GetAddress(),
                              &dElevation ) )
                        return ret; // Quit if no DBDB data

                     m_bDBDBReaderInitialized = TRUE;
                  }

                  if ( S_OK != spDBDBElevationData->raw_GetBlockElevation(
                        northern_lat, western_lon, southern_lat, eastern_lon,
                        tile_size, tile_size, ELEV_UNITS_METERS, L"", 0.0,
                        (MapScaleUnitsEnum) 0, _bstr_t( L"" ), &varDBDBData ) )
                     return ret;

                  if ( varDBDBData.vt == VT_NULL )
                     return ret;

                  if ( varDBDBData.vt != ( VT_I4 | VT_ARRAY ) ||
                     varDBDBData.parray->cDims != 2 ||
                     varDBDBData.parray->rgsabound[0].cElements != tile_size ||
                     varDBDBData.parray->rgsabound[1].cElements != tile_size )
                  {
                     ATLASSERT( FALSE &&
                        "GetBlockElevation( DBDB ) result is wrong" );
                     return ret;
                  }
                  piDBDBElevation = (PINT) varDBDBData.parray->pvData;
               }

               // Replace sea level (or below ) DTED data with DBDB (if present)
               if ( piDBDBElevation[ kIn ] != -99999 )
                  data[ kOut ] = piDBDBElevation[ kIn ];

            }  // Possible DBDB
         }  // Point loop
   }  // DBDB enabled
#endif

   return ret;
}

IDtedPtr ElevationDataTileSource::GetElevationReaderForCurrentThread()
{
   const DWORD thread_id = ::GetCurrentThreadId();

   // If we've already created a Dted COM object for the current thread
   // then return it
   ATL::CCritSecLock cs(m_critical_section);
   auto it = m_thread_id_to_elevation_reader.find(thread_id);
   if (it != m_thread_id_to_elevation_reader.end())
   {
      return it->second;
   }

   // Create a new Dted COM object and add it to the map
   IDtedPtr ret;
   try
   {
      CO_CREATE(ret, CLSID_Dted);
      m_thread_id_to_elevation_reader[thread_id] = ret;

      // Hook our IDtedPtr cleanup to the DatabasePager thread exit
      osgDB::DatabasePager::ThreadCleanup::s_clsThreadCleanup.AddCleanup(
                              ThreadCleanup, (LPVOID) this );
      return ret;
   }
   catch (_com_error&)
   {
      ERR_report( "ElevationDataTileSource: unable to create CLSID_Dted" );
      return nullptr;
   }
}

#if GOV_RELEASE

IElevationDataPtr
   ElevationDataTileSource::GetDBDBElevationReaderForCurrentThread()
{
   const DWORD thread_id = ::GetCurrentThreadId();

   // If we've already created a DBDB COM object for the current thread
   // then return it
   ATL::CCritSecLock cs(m_critical_section);
   auto it =m_mpDBDBElevationDataReader.find( thread_id );
   if ( it != m_mpDBDBElevationDataReader.end() )
      return it->second;

   // Create a new DBDB COM object and add it to its map
   try
   {
      IElevationDataPtr ret;
      CO_CREATE( ret, CLSID_DBDBMapPlugIn );
      m_mpDBDBElevationDataReader[ thread_id ] = ret;

      // Hook our IElevationDataPtr cleanup to the DatabasePager thread exit
      // (duplicates harmless)
      osgDB::DatabasePager::ThreadCleanup::s_clsThreadCleanup.AddCleanup(
                        ThreadCleanup, (LPVOID) this );

      return ret;
   }
   catch (_com_error&)
   {
      ERR_report( "ElevationDataTileSource: unable to create "
         "CLSID_DBDBMapPlugIn" );
      return nullptr;
   }
}

#endif   // GOV_RELEASE

// These thread cleanup routines are called from any OSG DatabasePager thread
// that is about to exit but before it has called CoUninitialize()

VOID __stdcall ElevationDataTileSource::ThreadCleanup( LPVOID pvParameter )
{
   ElevationDataTileSource* pedts =
      reinterpret_cast< ElevationDataTileSource* >( pvParameter );

   // Cleanup only if parent ElevationDataTileSource hasn't been destroyed
   ATL::CCritSecLock lock( s_csStaticAccess );
   if ( s_stElevationDataTileSources.find( pedts ) !=
         s_stElevationDataTileSources.end() )
   {
      lock.Unlock();
      pedts->ThreadCleanup();
   }
}

VOID ElevationDataTileSource::ThreadCleanup()
{
   // Release any IDtedPtr or IElevationDataPtr for this thread
   ATL::CCritSecLock cs(m_critical_section);
   auto it = m_thread_id_to_elevation_reader.find( ::GetCurrentThreadId() );
   if ( it != m_thread_id_to_elevation_reader.end() )
   {
      it->second->AddRef();
      if ( it->second->Release() <= 2 )   // If no other references
         it->second->raw_Terminate();
      m_thread_id_to_elevation_reader.erase( it );
   }
#if GOV_RELEASE
   m_mpDBDBElevationDataReader.erase( ::GetCurrentThreadId() );
#endif
}

// End of ElevationDataTileSource.cpp
