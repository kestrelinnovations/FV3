// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#include "StdAfx.h"
#include "gtest/gtest.h"
#include "GdalInitializer.h"
#include "ogrsf_frmts.h"

TEST(gdal_initializer_tests, test_init_uninit)
{
   // make sure that we can initialize and uninitialize with impunity

   EXPECT_FALSE(GdalInitializer::IsGDALInitialized());

   GdalInitializer::InitGDAL();
   EXPECT_TRUE(GdalInitializer::IsGDALInitialized());

   GdalInitializer::UninitGDAL();
   EXPECT_FALSE(GdalInitializer::IsGDALInitialized());

   GdalInitializer::InitGDAL();
   GdalInitializer::InitGDAL();
   EXPECT_TRUE(GdalInitializer::IsGDALInitialized());

   GdalInitializer::UninitGDAL();
   GdalInitializer::UninitGDAL();
   EXPECT_FALSE(GdalInitializer::IsGDALInitialized());
}

TEST(gdal_initializer_tests, test_no_errors)
{
   EXPECT_FALSE(GdalInitializer::IsGDALInitialized());

   GdalInitializer::InitGDAL();
   EXPECT_TRUE(GdalInitializer::IsGDALInitialized());

   CPLErr err_class;
   int err_no;
   std::string msg;

   GdalInitializer::GetLastError(&err_class, &err_no, &msg);

   EXPECT_EQ(CE_None, err_class);
   EXPECT_EQ(CPLE_None, err_no);
   EXPECT_STREQ("", msg.c_str());

   GdalInitializer::UninitGDAL();
   EXPECT_FALSE(GdalInitializer::IsGDALInitialized());
}

TEST(gdal_initializer_tests, test_errors)
{
   EXPECT_FALSE(GdalInitializer::IsGDALInitialized());

   // initialize in test mode to prevent log writing
   GdalInitializer::InitGDAL(true);
   EXPECT_TRUE(GdalInitializer::IsGDALInitialized());

   CPLErr err_class;
   int err_no;
   std::string msg;

   GdalInitializer::GetLastError(&err_class, &err_no, &msg);

   EXPECT_EQ(CE_None, err_class);
   EXPECT_EQ(CPLE_None, err_no);
   EXPECT_STREQ("", msg.c_str());

   ::GDALOpen("no such file foofoo", GA_Update);

   GdalInitializer::GetLastError(&err_class, &err_no, &msg);

   EXPECT_EQ(CE_Failure, err_class);
   EXPECT_EQ(CPLE_OpenFailed, err_no);
   EXPECT_LT((size_t)0, msg.length());

   GdalInitializer::UninitGDAL();
   EXPECT_FALSE(GdalInitializer::IsGDALInitialized());
}

TEST(gdal_initializer_tests, check_for_formats)
{
   // As we continue to upgrade GDAL, we need to make sure that we retain
   // support for various GDAL / OGR formats.  See #4661 in Trac.
   GdalInitializer::InitGDAL(true);
   EXPECT_TRUE(GdalInitializer::IsGDALInitialized());

   // alphabetized
   std::vector<std::string> expected_types;
   expected_types.push_back("AeronavFAA");
   expected_types.push_back("AVCBin");
   expected_types.push_back("ARCGEN");
   expected_types.push_back("AVCE00");
   expected_types.push_back("BNA");
   expected_types.push_back("CSV");
   expected_types.push_back("DGN");
   expected_types.push_back("DXF");
   expected_types.push_back("EDIGEO");
   expected_types.push_back("ESRI Shapefile");
   expected_types.push_back("Geoconcept");
   expected_types.push_back("GeoRSS");
   expected_types.push_back("GML");
   expected_types.push_back("GMT");
   expected_types.push_back("GPSBabel");
   expected_types.push_back("GPX");
   expected_types.push_back("GPSTrackMaker");
   expected_types.push_back("HTF");
   expected_types.push_back("Idrisi");
   expected_types.push_back("Memory");
   expected_types.push_back("OpenAir");
   expected_types.push_back("PCIDSK");
   expected_types.push_back("PDS");
   expected_types.push_back("PGDump");
   expected_types.push_back("PostgreSQL");
   expected_types.push_back("REC");
   expected_types.push_back("S57");
   expected_types.push_back("SDTS");
   expected_types.push_back("SEGUKOOA");
   expected_types.push_back("SEGY");
   expected_types.push_back("SQLite");
   expected_types.push_back("SUA");
   expected_types.push_back("TIGER");
   expected_types.push_back("UK .NTF");
   expected_types.push_back("VFK");
   expected_types.push_back("VRT");
   expected_types.push_back("XPlane");

   OGRSFDriverRegistrar* registrar = OGRSFDriverRegistrar::GetRegistrar();
   std::for_each(expected_types.begin(), expected_types.end(),
      [&](std::string name)
   {
      OGRSFDriver* driver = registrar->GetDriverByName(name.c_str());
      if (driver == nullptr)
      {
         // bit of hackery to print what driver failed
         EXPECT_STREQ(name.c_str(), "");
      }
   });

   GdalInitializer::UninitGDAL();
   EXPECT_FALSE(GdalInitializer::IsGDALInitialized());
}
