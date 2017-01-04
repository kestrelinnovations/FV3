// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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

// MapRenderingOnTiles_UnitTests.cpp
//

#include "stdafx.h"

#include "gtest/gtest.h"

#include "osgEarth/TileKey"

#include "FalconView/MapEngineCOM.h"
#include "FalconView/MapRenderingOnTiles.h"
#include "FalconView/OsgEarthMapEngineCallback.h"
#include "FalconView/include/overlay.h"

// The anonymous namespace is to handle compiler issues with FakeOverlay being 
// defined in multiple Unit Tests.  
namespace
{
CComPtr<CComObject<OsgEarthMapEngineCallback>> GetCallback()
{
   CComObject<OsgEarthMapEngineCallback>* callback;
   CComObject<OsgEarthMapEngineCallback>::CreateInstance(&callback);
   CComPtr<CComObject<OsgEarthMapEngineCallback>> callback_ptr(callback);
   return callback_ptr;
}

// FakeOverlay - used because a real overlay is needed for testing
class FakeOverlay : public C_overlay
{
public:
   // constructor
   FakeOverlay()
   {
   }

   STDMETHOD_(ULONG, AddRef)() override
   {
      return 1;
   }
   STDMETHOD_(ULONG, Release)() override
   {
      delete this;
      return 0;
   }

   virtual FalconViewOverlayLib::IFvOverlayPtr GetFvOverlay()
   {
      return nullptr;
   }
   virtual int open(const CString & /*pathname*/)
   {
      return 0;
   }
   virtual int save_as(const CString & pathname, long nSaveFormat)
   {
      return 0;
   }
};

TEST(MapRenderingOnTiles,
   DISABLED_NOT_A_UNIT_TEST_TestGetMapEngineForCurrentThreadOneThread)
{
   // get the map rendering engine for this thread
   MapEngineCOM* engine = fvw::GetMapRenderingEngineForCurrentThread(nullptr);

   // get it again to confirm it's the same instance since we're on the same
   // thread
   MapEngineCOM* engine2 = fvw::GetMapRenderingEngineForCurrentThread(nullptr);

   ASSERT(engine == engine2);
}

TEST(MapRenderingOnTiles, DISABLED_NOT_A_UNIT_TEST_TestCreateMapEngine)
{
   CComPtr<CComObject<OsgEarthMapEngineCallback>> callback = GetCallback();

   MapEngineCOM* engine = fvw::CreateMapEngine(callback);

   ASSERT_EQ(callback, engine->GetCallback());
}

TEST(MapRenderingOnTiles, DISABLED_NOT_A_UNIT_TEST_TestCreateMapEngineCallback)
{
   auto result =
      fvw::CreateMapEngineCallback(nullptr);

   ASSERT(nullptr != result);
}

TEST(MapRenderingOnTiles, DISABLED_NOT_A_UNIT_TEST_SetCallbackMapType)
{
   CComPtr<CComObject<OsgEarthMapEngineCallback>> callback = GetCallback();

   osgEarth::GeoExtent geo_extent;
   MapSource source;
   MapScale scale;
   MapType map(source, scale);

   fvw::PrepareCallback(callback, map, geo_extent, 0, 0);

   ASSERT_TRUE(map == callback->GetMapType());
}

TEST(MapRenderingOnTiles, DISABLED_NOT_A_UNIT_TEST_SetCallbackSurfaceDimensions)
{
   CComPtr<CComObject<OsgEarthMapEngineCallback>> callback = GetCallback();

   osgEarth::GeoExtent geo_extent;

   double width = 100;
   double height = 100;

   fvw::PrepareCallback(callback, MapType(), geo_extent, width, height);

   ASSERT_EQ(width, callback->GetWidth());
   ASSERT_EQ(height, callback->GetHeight());
}

} // namespace
