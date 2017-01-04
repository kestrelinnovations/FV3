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

// ZoomPercentagesUnitTests.cpp

#include "stdafx.h"
#include "MapParams.h"

#include "gtest/gtest.h"

#include "FalconView/ZoomPercentages.h"

// The valid zoom percentages are: 800, 400, 200, 100, 75, 50, 25, 10

TEST(ZoomPercentagesUnitTests, NextLargestFrom100)
{
   int next_zoom;
   bool ret = ui::GetNextLargestZoomPercent(100, &next_zoom);
   EXPECT_EQ(ret, true);
   EXPECT_EQ(200, next_zoom);
}

TEST(ZoomPercentagesUnitTests, NextLargestFrom150)
{
   int next_zoom;
   bool ret = ui::GetNextLargestZoomPercent(150, &next_zoom);
   EXPECT_EQ(ret, true);
   EXPECT_EQ(200, next_zoom);
}

TEST(ZoomPercentagesUnitTests, NextLargestFrom275)
{
   int next_zoom;
   bool ret = ui::GetNextLargestZoomPercent(275, &next_zoom);
   EXPECT_EQ(ret, true);
   EXPECT_EQ(400, next_zoom);
}

TEST(ZoomPercentagesUnitTests, NextLargestBoundsCheck)
{
   int next_zoom;
   bool ret = ui::GetNextLargestZoomPercent(800, &next_zoom);
   EXPECT_EQ(ret, false);
}

TEST(ZoomPercentagesUnitTests, NextSmallestFrom100)
{
   int next_zoom;
   bool ret = ui::GetNextSmallestZoomPercent(100, &next_zoom);
   EXPECT_EQ(ret, true);
   EXPECT_EQ(75, next_zoom);
}

TEST(ZoomPercentagesUnitTests, NextSmallestFrom80)
{
   int next_zoom;
   bool ret = ui::GetNextSmallestZoomPercent(80, &next_zoom);
   EXPECT_EQ(ret, true);
   EXPECT_EQ(75, next_zoom);
}

TEST(ZoomPercentagesUnitTests, NextSmallestBoundsCheck)
{
   int next_zoom;
   bool ret = ui::GetNextSmallestZoomPercent(10, &next_zoom);
   EXPECT_EQ(ret, false);
}